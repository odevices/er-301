#include <hal/priorities.h>
#include <hal/audio.h>
#include <hal/gpio.h>
#include <hal/dma.h>
#include <hal/constants.h>
#include <hal/log.h>
#include <od/config.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/utils/Load.h>
#include <ti/sysbios/family/arm/a8/intcps/Hwi.h>
#include <ti/am335x/mcasp.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/hw_types.h>
#include <string.h>

#define TDM_ENABLED_SLOTS (0b1111u)
#define TDM_NUM_SLOTS (4u)

/* Choose McASP Serializer for Transmit */
#define MCASP_XSER_TX (0u)

#define MAX_AUDIO_BUFFER_BYTES (AUDIO_NUM_CHANNELS * MAX_AUDIO_FRAME_LENGTH * sizeof(int))

static struct AudioLocals
{
  int ping[CACHE_ALIGNED_SIZE(MAX_AUDIO_BUFFER_BYTES) / sizeof(int)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
  int pong[CACHE_ALIGNED_SIZE(MAX_AUDIO_BUFFER_BYTES) / sizeof(int)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
  EDMA3_DRV_Handle hEdma;

  uint32_t pingTcc;
  uint32_t pongTcc;
  uint32_t chId;
  uint32_t pingLink;
  uint32_t pongLink;

  Event_Handle hEvents;
#define pingDone Event_Id_00
#define pongDone Event_Id_01
#define errorOccurred Event_Id_02

  Hwi_Handle hOnError;
  uint32_t error_count;
  uint32_t last_error_status;
  bool timedout;
  bool restart;

  uint32_t sampleRate;
  uint32_t fifo_level;
  uint32_t frameSizeInBytes;
  uint32_t frameLength;

  // error counts
  uint32_t dmaerr, clkfail, syncerr, underrun, timeout;
  bool task_started;

  Task_Handle hTask;
} local;

/*
 ** Configures the McASP Transmit Section for 4 channels TDM.
 */
static void initMcasp()
{
  McASPTxReset(SOC_MCASP_0_CTRL_REGS);

  /* Set TDM format in the transmitter/receiver format units */
  switch (local.sampleRate)
  {
  case 192000:
    McASPTxFmtTDMSet(SOC_MCASP_0_CTRL_REGS, 24, 32, MCASP_TX_MODE_DMA);
    Gpio_write(PCM4104_FS1, 1);
    Gpio_write(PCM4104_FS0, 1);
    Gpio_write(AUDIO_EXTERNAL_CLOCK_ENABLE, 1);
    Task_sleep(10);
    break;
  case 96000:
    McASPTxFmtTDMSet(SOC_MCASP_0_CTRL_REGS, 24, 32, MCASP_TX_MODE_DMA);
    Gpio_write(PCM4104_FS1, 0);
    Gpio_write(PCM4104_FS0, 1);
    Gpio_write(AUDIO_EXTERNAL_CLOCK_ENABLE, 1);
    Task_sleep(10);
    break;
  case 48000:
    McASPTxFmtTDMSet(SOC_MCASP_0_CTRL_REGS, 24, 32, MCASP_TX_MODE_DMA);
    Gpio_write(PCM4104_FS1, 0);
    Gpio_write(PCM4104_FS0, 0);
    Gpio_write(AUDIO_EXTERNAL_CLOCK_ENABLE, 1);
    Task_sleep(10);
    break;
  case 62500:
    McASPTxFmtTDMSet(SOC_MCASP_0_CTRL_REGS, 24, 24, MCASP_TX_MODE_DMA);
    Gpio_write(PCM4104_FS1, 0);
    Gpio_write(PCM4104_FS0, 1);
    Gpio_write(AUDIO_EXTERNAL_CLOCK_ENABLE, 0);
    Task_sleep(10);
    break;
  }

  McASPTxFrameSyncCfg(SOC_MCASP_0_CTRL_REGS, TDM_NUM_SLOTS,
                      MCASP_TX_FS_WIDTH_BIT, MCASP_TX_FS_INT_BEGIN_ON_RIS_EDGE);

  /* configure the clock for transmitter */

  switch (local.sampleRate)
  {
  case 192000:
    // BCLK: 192000 Hz * (4 ch * 32 bits) = 24.576 MHz
    // external clock: 24.576 MHz
    // BCLK divider = 24.576/24.576 = 1
    McASPTxClkCfg(SOC_MCASP_0_CTRL_REGS, MCASP_TX_CLK_MIXED, 1, 1);
    break;
  case 96000:
    // BCLK: 96000 Hz * (4 ch * 32 bits) = 12.288 MHz
    // external clock: 24.576 MHz
    // BCLK divider = 24.576/12.288 = 2
    McASPTxClkCfg(SOC_MCASP_0_CTRL_REGS, MCASP_TX_CLK_MIXED, 2, 1);
    break;
  case 48000:
    // BCLK: 48000 Hz * (4 ch * 32 bits) = 6.144 MHz
    // external clock: 24.576 MHz
    // BCLK divider = 24.576/6.144 = 4
    McASPTxClkCfg(SOC_MCASP_0_CTRL_REGS, MCASP_TX_CLK_MIXED, 4, 1);
    break;
  case 62500:
    // BCLK: 62500 Hz * (4 ch * 24 bits) = 6 MHz
    // MCLK: 24 MHz (divider = 1)
    // BCLK divider = 24/6 = 4
    McASPTxClkCfg(SOC_MCASP_0_CTRL_REGS, MCASP_TX_CLK_INTERNAL, 4, 1);
    break;
  }

  // PCM4104 samples on rising edge, so clock data out on falling edge
  McASPTxClkPolaritySet(SOC_MCASP_0_CTRL_REGS, MCASP_TX_CLK_POL_FALL_EDGE);
  McASPTxClkCheckConfig(SOC_MCASP_0_CTRL_REGS, MCASP_TX_CLKCHCK_DIV32, 0x00,
                        0xFF);

  /* Enable the transmitter/receiver slots. */
  McASPTxTimeSlotSet(SOC_MCASP_0_CTRL_REGS, TDM_ENABLED_SLOTS);

  /* Set the serializer as transmitter */
  McASPSerializerTxSet(SOC_MCASP_0_CTRL_REGS, MCASP_XSER_TX);

  /*
	 ** Configure the McASP pins
	 */
  McASPPinMcASPSet(SOC_MCASP_0_CTRL_REGS,
                   MCASP_PIN_AXR(
                       MCASP_XSER_TX) |
                       MCASP_PIN_AFSX | MCASP_PIN_ACLKX | MCASP_PIN_AHCLKX);
  switch (local.sampleRate)
  {
  case 192000:
    McASPPinDirInputSet(SOC_MCASP_0_CTRL_REGS, MCASP_PIN_AHCLKX);
    McASPPinDirOutputSet(SOC_MCASP_0_CTRL_REGS,
                         MCASP_PIN_AXR(MCASP_XSER_TX) | MCASP_PIN_AFSX | MCASP_PIN_ACLKX);
    break;
  case 96000:
    McASPPinDirInputSet(SOC_MCASP_0_CTRL_REGS, MCASP_PIN_AHCLKX);
    McASPPinDirOutputSet(SOC_MCASP_0_CTRL_REGS,
                         MCASP_PIN_AXR(MCASP_XSER_TX) | MCASP_PIN_AFSX | MCASP_PIN_ACLKX);
    break;
  case 48000:
    McASPPinDirInputSet(SOC_MCASP_0_CTRL_REGS, MCASP_PIN_AHCLKX);
    McASPPinDirOutputSet(SOC_MCASP_0_CTRL_REGS,
                         MCASP_PIN_AXR(MCASP_XSER_TX) | MCASP_PIN_AFSX | MCASP_PIN_ACLKX);
    break;
  case 62500:
    McASPPinDirOutputSet(SOC_MCASP_0_CTRL_REGS,
                         MCASP_PIN_AXR(
                             MCASP_XSER_TX) |
                             MCASP_PIN_AFSX | MCASP_PIN_ACLKX | MCASP_PIN_AHCLKX);
    break;
  }

  /* Enable the FIFOs for DMA transfer */
  McASPWriteFifoEnable(SOC_MCASP_0_FIFO_REGS, 1, local.fifo_level);

  McASPTxIntEnable(SOC_MCASP_0_CTRL_REGS,
                   MCASP_TX_DMAERROR | MCASP_TX_CLKFAIL | MCASP_TX_SYNCERROR | MCASP_TX_UNDERRUN);
}

#define AUDIO_TASK_TIMEOUT BIOS_WAIT_FOREVER
static Void taskAudio(UArg arg0, UArg arg1)
{
  uint8_t state = 0;
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;
  uint32_t posted;

  local.timedout = false;

  while (1)
  {
    switch (state)
    {
    case 0:
      if (local.restart)
      {
        state = 3;
        local.restart = false;
        break;
      }
      // ping
      posted = Event_pend(local.hEvents, Event_Id_NONE,
                          pingDone | errorOccurred, AUDIO_TASK_TIMEOUT);
      if (posted & pingDone)
      {
        Audio_callback(local.ping);
        result = Edma3_CacheFlush((uint32_t)local.ping,
                                  local.frameSizeInBytes);
        logAssert(result == EDMA3_DRV_SOK);
        state = 1;
      }
      else if (posted & errorOccurred)
      {
        state = 2;
      }
      else
      {
        local.timedout = true;
        state = 2;
      }
      break;
    case 1:
      // pong
      posted = Event_pend(local.hEvents, Event_Id_NONE,
                          pongDone | errorOccurred, AUDIO_TASK_TIMEOUT);
      if (posted & pongDone)
      {
        Audio_callback(local.pong);
        result = Edma3_CacheFlush((uint32_t)local.pong,
                                  local.frameSizeInBytes);
        logAssert(result == EDMA3_DRV_SOK);
        state = 0;
      }
      else if (posted & errorOccurred)
      {
        state = 2;
      }
      else
      {
        local.timedout = true;
        state = 2;
      }
      break;
    case 2:
      // There was an error or time out.
      state = 0;

      if (local.last_error_status & MCASP_TX_STAT_CLKFAIL)
      {
        local.clkfail++;
        state = 3;
      }

      if (local.last_error_status & MCASP_TX_STAT_SYNCERR)
      {
        local.syncerr++;
        state = 3;
      }

      if (local.last_error_status & MCASP_TX_STAT_UNDERRUN)
      {
        local.underrun++;
        state = 3;
      }

      if (local.last_error_status & MCASP_TX_STAT_DMAERR)
      {
        local.dmaerr++;
        state = 3;
      }

      if (local.timedout)
      {
        local.timeout++;
        state = 3;
      }

      local.error_count++;

    case 3:
      // restart the audio engine
      Audio_stop();

      // clear all events
      Event_pend(local.hEvents, Event_Id_NONE,
                 pingDone | pongDone | errorOccurred, BIOS_NO_WAIT);

      Audio_start();
      state = 0;
      break;
    }
  }
}

static void hwiOnError(UArg arg)
{
  uint32_t status = McASPTxStatusGet(SOC_MCASP_0_CTRL_REGS);

  // clear the errors
  HW_WR_REG32(SOC_MCASP_0_CTRL_REGS + MCASP_XSTAT,
              MCASP_TX_CLKFAIL | MCASP_TX_DMAERROR | MCASP_TX_SYNCERROR | MCASP_TX_UNDERRUN);

  if (status & (1 << 8)) // XERR flag
  {
    local.last_error_status = status;
    Event_post(local.hEvents, errorOccurred);
  }
}

static void onPingCompletion(uint32_t tcc, EDMA3_RM_TccStatus status,
                             void *appData)
{
  if (status == EDMA3_RM_XFER_COMPLETE)
  {
    Event_post(local.hEvents, pingDone);
  }
  else
  {
    local.last_error_status = 0;
    Event_post(local.hEvents, errorOccurred);
  }
}

static void onPongCompletion(uint32_t tcc, EDMA3_RM_TccStatus status,
                             void *appData)
{
  if (status == EDMA3_RM_XFER_COMPLETE)
  {
    Event_post(local.hEvents, pongDone);
  }
  else
  {
    local.last_error_status = 0;
    Event_post(local.hEvents, errorOccurred);
  }
}

static void initDma()
{
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;
  int32_t bidx = 0, cidx = 0, acnt = 0, bcnt = 0, ccnt = 0, bcnt_reload = 0;

  if (local.chId == 0)
  {
    // Claim the MCASP0_TX channel
    local.chId = EDMA3_CHA_MCASP0_TX;
    // This can be any link channel since we will share the TCC with the master channel
    local.pingLink = EDMA3_DRV_LINK_CHANNEL_WITH_TCC;
    // Claim another channel that is open to use its TCC as the pong event
    local.pongLink = EDMA3_DRV_DMA_CHANNEL_ANY;

    local.pingTcc = EDMA3_DRV_TCC_ANY;
    local.pongTcc = EDMA3_DRV_TCC_ANY;

    result = EDMA3_DRV_requestChannel(local.hEdma, &local.chId, &local.pingTcc,
                                      (EDMA3_RM_EventQueue)DMA_QUEUE_MCASP, &onPingCompletion,
                                      &local);

    logAssert(result == EDMA3_DRV_SOK);

    result = EDMA3_DRV_requestChannel(local.hEdma, &local.pongLink,
                                      &local.pongTcc, (EDMA3_RM_EventQueue)DMA_QUEUE_MCASP,
                                      &onPongCompletion, &local);

    logAssert(result == EDMA3_DRV_SOK);

    result = EDMA3_DRV_requestChannel(local.hEdma, &local.pingLink,
                                      &local.pingTcc, (EDMA3_RM_EventQueue)DMA_QUEUE_MCASP,
                                      &onPingCompletion, &local);

    logAssert(result == EDMA3_DRV_SOK);
  }

  // Fill in PaRam sets

  // src params
  result = EDMA3_DRV_setSrcParams(local.hEdma, local.chId, (uint32_t)local.ping,
                                  EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W8BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcParams(local.hEdma, local.pingLink,
                                  (uint32_t)local.ping, EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W8BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcParams(local.hEdma, local.pongLink,
                                  (uint32_t)local.pong, EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W8BIT);
  logAssert(result == EDMA3_DRV_SOK);

  // dst params
  result = EDMA3_DRV_setDestParams(local.hEdma, local.chId,
                                   SOC_MCASP_0_DATA_REGS, EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestParams(local.hEdma, local.pingLink,
                                   SOC_MCASP_0_DATA_REGS, EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestParams(local.hEdma, local.pongLink,
                                   SOC_MCASP_0_DATA_REGS, EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  local.frameLength = globalConfig.frameLength;
  local.frameSizeInBytes = AUDIO_NUM_CHANNELS * sizeof(int) * local.frameLength;

  // Packing is 'channel major'.
  acnt = sizeof(int) * local.fifo_level;
  bcnt = local.frameSizeInBytes / acnt;
  bcnt_reload = 0;
  ccnt = 1;
  bidx = acnt;
  cidx = 0;

  // src indexing (bidx, cidx)
  result = EDMA3_DRV_setSrcIndex(local.hEdma, local.chId, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcIndex(local.hEdma, local.pingLink, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcIndex(local.hEdma, local.pongLink, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  // transfer params
  result = EDMA3_DRV_setTransferParams(local.hEdma, local.chId, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setTransferParams(local.hEdma, local.pingLink, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setTransferParams(local.hEdma, local.pongLink, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  // dest indexing (bidx, cidx)
  result = EDMA3_DRV_setDestIndex(local.hEdma, local.chId, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestIndex(local.hEdma, local.pingLink, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestIndex(local.hEdma, local.pongLink, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  // opt fields
  result = EDMA3_DRV_setOptField(local.hEdma, local.chId,
                                 EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(local.hEdma, local.pingLink,
                                 EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(local.hEdma, local.pongLink,
                                 EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(local.hEdma, local.chId,
                                 EDMA3_DRV_OPT_FIELD_TCCMODE, EDMA3_DRV_TCCMODE_NORMAL);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(local.hEdma, local.pingLink,
                                 EDMA3_DRV_OPT_FIELD_TCCMODE, EDMA3_DRV_TCCMODE_NORMAL);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(local.hEdma, local.pongLink,
                                 EDMA3_DRV_OPT_FIELD_TCCMODE, EDMA3_DRV_TCCMODE_NORMAL);
  logAssert(result == EDMA3_DRV_SOK);

  // Link channels to create the ping pong structure
  result = EDMA3_DRV_linkChannel(local.hEdma, local.chId, local.pongLink);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_linkChannel(local.hEdma, local.pongLink, local.pingLink);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_linkChannel(local.hEdma, local.pingLink, local.pongLink);
  logAssert(result == EDMA3_DRV_SOK);
}

void Audio_init()
{
  EDMA3_DRV_Handle hEdma = DMA_getHandle();

  memset(&local, 0, sizeof(local));

  local.fifo_level = 32;

  local.task_started = false;
  local.hEdma = hEdma;
  local.dmaerr = 0;
  local.clkfail = 0;
  local.syncerr = 0;
  local.underrun = 0;
  local.timeout = 0;
  local.last_error_status = 0;
  local.error_count = 0;
  local.hEvents = Event_create(NULL, NULL);
  local.restart = false;

  {
    Hwi_Params params;
    Hwi_Params_init(&params);
    params.priority = HWI_PRIORITY_AUDIO;
    local.hOnError = Hwi_create(MCASP0TX_INT, hwiOnError, &params, NULL);
  }

  if (!local.task_started)
  {
    Task_Params params;
    Task_Params_init(&params);
    params.priority = TASK_PRIORITY_AUDIO;
    params.env = (Ptr)"audio";
    local.hTask = Task_create(taskAudio, &params, NULL);
    local.task_started = true;
  }
}

void Audio_restart(void)
{
  local.restart = true;
}

void Audio_start(void)
{

  local.sampleRate = globalConfig.sampleRate;

  Gpio_write(PCM4104_MUTE, 1);
  Gpio_write(PCM4104_nRESET, 0);
  Task_sleep(1);
  Gpio_write(PCM4104_nRESET, 1);
  Task_sleep(15);

  initDma();
  initMcasp();

  /* Start the clocks */
  switch (local.sampleRate)
  {
  case 192000:
    McASPTxClkStart(SOC_MCASP_0_CTRL_REGS, MCASP_TX_CLK_MIXED);
    break;
  case 96000:
    McASPTxClkStart(SOC_MCASP_0_CTRL_REGS, MCASP_TX_CLK_MIXED);
    break;
  case 48000:
    McASPTxClkStart(SOC_MCASP_0_CTRL_REGS, MCASP_TX_CLK_MIXED);
    break;
  case 62500:
    McASPTxClkStart(SOC_MCASP_0_CTRL_REGS, MCASP_TX_CLK_INTERNAL);
    break;
  }

  /* Enable EDMA for the transfer */
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;
  result = EDMA3_DRV_enableTransfer(local.hEdma, local.chId,
                                    EDMA3_DRV_TRIG_MODE_EVENT);
  logAssert(result == EDMA3_DRV_SOK);

  /* Activate the  serializers */
  McASPTxSerActivate(SOC_MCASP_0_CTRL_REGS);

  int count = 0;
  while (McASPTxStatusGet(SOC_MCASP_0_CTRL_REGS) & MCASP_XSTAT_XDATA)
  {
    Task_sleep(1);
    count++;
    if (count > 1000)
    {
      logWarn("Audio_start: Timed out waiting for XDATA bit to be cleared.");
      break;
    }
  }

  /* Activate the state machines */
  McASPTxEnable(SOC_MCASP_0_CTRL_REGS);

  Gpio_write(PCM4104_MUTE, 0);
}

void Audio_stop(void)
{
  Gpio_write(PCM4104_MUTE, 1);
  Gpio_write(AUDIO_EXTERNAL_CLOCK_ENABLE, 0);

  McASPSerializerInactivate(SOC_MCASP_0_CTRL_REGS, MCASP_XSER_TX);

  /* Disable EDMA for the transfer */
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;
  result = EDMA3_DRV_disableTransfer(local.hEdma, local.chId,
                                     EDMA3_DRV_TRIG_MODE_EVENT);
  logAssert(result == EDMA3_DRV_SOK);
  result = EDMA3_DRV_clearErrorBits(local.hEdma, local.chId);
  logAssert(result == EDMA3_DRV_SOK);

  McASPTxIntDisable(SOC_MCASP_0_CTRL_REGS,
                    MCASP_TX_DMAERROR | MCASP_TX_CLKFAIL | MCASP_TX_SYNCERROR | MCASP_TX_UNDERRUN);
}

uint32_t Audio_errorCount(void)
{
  return local.error_count;
}

int Audio_getRate(void)
{
  return (int)local.sampleRate;
}

void Audio_printErrorStatus(void)
{
  logInfo("audio: total=%d, dmaerr=%d clkfail=%d syncerr=%d underrun=%d timeout=%d",
          local.error_count, local.dmaerr, local.clkfail, local.syncerr,
          local.underrun, local.timeout);
}

int Audio_getLoad()
{
  Load_Stat stat;
  if (Load_getTaskLoad(local.hTask, &stat))
  {
    // assumes a constant kernel overhead of 11%
    return (int)(1.12f * Load_calculateLoad(&stat));
  }
  else
  {
    return 0;
  }
}
