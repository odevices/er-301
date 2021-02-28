#include <hal/log.h>
#include <hal/priorities.h>
#include <hal/adc.h>
#include <hal/dma.h>
#include <hal/ops.h>
#include <od/config.h>
#include <hal/constants.h>
#include <ti/am335x/tsc_adc_ss.h>
#include <ti/am335x/soc.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>
#include <string.h>

#define ADC_BUFFER_BYTES (ADC_NUM_CHANNELS * ADC_FRAME_LENGTH * sizeof(int))

#define ADC_MODCLK 24000000U
#define ADC_AFEINCLK 24000000U

typedef struct
{
  int ping[CACHE_ALIGNED_SIZE(ADC_BUFFER_BYTES) / sizeof(int)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
  int pong[CACHE_ALIGNED_SIZE(ADC_BUFFER_BYTES) / sizeof(int)] __attribute__((aligned(CACHELINE_SIZE_MAX)));

  uint32_t pingTcc;
  uint32_t pongTcc;
  uint32_t chIdRx;
  uint32_t pingLink;
  uint32_t pongLink;
  int burstSize;
  int bufferSizeInBytes;

  EDMA3_DRV_Handle hEdma;

  Event_Handle hEvents;
#define pingDone Event_Id_00
#define pongDone Event_Id_01

  bool running;
} adc_t;

static adc_t self;

static void onPingCompletion(uint32_t tcc, EDMA3_RM_TccStatus status,
                             void *appData)
{
  logAssert(status == EDMA3_RM_XFER_COMPLETE);
  Event_post(self.hEvents, pingDone);
}

static void onPongCompletion(uint32_t tcc, EDMA3_RM_TccStatus status,
                             void *appData)
{
  logAssert(status == EDMA3_RM_XFER_COMPLETE);
  Event_post(self.hEvents, pongDone);
}

// #define ADC_TASK_TIMEOUT BIOS_WAIT_FOREVER
#define ADC_TASK_TIMEOUT 1000
static Void taskAdc(UArg arg0, UArg arg1)
{
  while (1)
  {
    uint32_t e = Event_pend(self.hEvents, Event_Id_NONE,
                            pingDone | pongDone,
                            ADC_TASK_TIMEOUT);
    if (e)
    {
      if (e & pingDone)
      {
        EDMA3_DRV_Result result = Edma3_CacheInvalidate((uint32_t)self.ping,
                                                        self.bufferSizeInBytes);
        logAssert(result == EDMA3_DRV_SOK);
        Adc_callback(self.ping);
      }

      if (e & pongDone)
      {
        EDMA3_DRV_Result result = Edma3_CacheInvalidate((uint32_t)self.pong,
                                                        self.bufferSizeInBytes);
        logAssert(result == EDMA3_DRV_SOK);
        Adc_callback(self.pong);
      }
    }
    else if (self.running)
    {
      // Time out while running, restart the hardware.

      Adc_stop();

      // clear all events
      Event_pend(self.hEvents, Event_Id_NONE,
                 pingDone | pongDone,
                 BIOS_NO_WAIT);

      Adc_start();
    }
  }
}

static void initDma(void)
{
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;
  int32_t bidx = 0, cidx = 0, acnt = 0, bcnt = 0, ccnt = 0, bcnt_reload = 0;

  if (self.chIdRx == 0)
  {
    // Claim the ADC event channel
    self.chIdRx = EDMA3_CHA_ADC_FIFO0;
    // This can be any link channel since we will share the TCC with the master channel
    self.pingLink = EDMA3_DRV_LINK_CHANNEL_WITH_TCC;
    // Claim another channel that is open to use its TCC as the pong event
    self.pongLink = EDMA3_DRV_DMA_CHANNEL_ANY;

    self.pingTcc = EDMA3_DRV_TCC_ANY;
    self.pongTcc = EDMA3_DRV_TCC_ANY;

    result = EDMA3_DRV_requestChannel(self.hEdma, &self.chIdRx,
                                      &self.pingTcc,
                                      (EDMA3_RM_EventQueue)DMA_QUEUE_ADC,
                                      onPingCompletion, NULL);

    logAssert(result == EDMA3_DRV_SOK);

    result = EDMA3_DRV_requestChannel(self.hEdma, &self.pongLink,
                                      &self.pongTcc,
                                      (EDMA3_RM_EventQueue)DMA_QUEUE_ADC,
                                      onPongCompletion, NULL);

    logAssert(result == EDMA3_DRV_SOK);

    result = EDMA3_DRV_requestChannel(self.hEdma, &self.pingLink,
                                      &self.pingTcc,
                                      (EDMA3_RM_EventQueue)DMA_QUEUE_ADC,
                                      onPingCompletion, NULL);

    logAssert(result == EDMA3_DRV_SOK);
  }

  // Fill in PaRam sets

  // src params
  result = EDMA3_DRV_setSrcParams(self.hEdma, self.chIdRx,
                                  SOC_ADC_TSC_0_REGS + ADC0_FIFO0DATA,
                                  EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcParams(self.hEdma, self.pingLink,
                                  SOC_ADC_TSC_0_REGS + ADC0_FIFO0DATA,
                                  EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcParams(self.hEdma, self.pongLink,
                                  SOC_ADC_TSC_0_REGS + ADC0_FIFO0DATA,
                                  EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  // dst params
  result = EDMA3_DRV_setDestParams(self.hEdma, self.chIdRx,
                                   (uint32_t)self.ping,
                                   EDMA3_DRV_ADDR_MODE_INCR,
                                   EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestParams(self.hEdma, self.pingLink,
                                   (uint32_t)self.ping,
                                   EDMA3_DRV_ADDR_MODE_INCR,
                                   EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestParams(self.hEdma, self.pongLink,
                                   (uint32_t)self.pong,
                                   EDMA3_DRV_ADDR_MODE_INCR,
                                   EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  acnt = sizeof(int) * self.burstSize;
  bcnt = self.bufferSizeInBytes / acnt;
  bcnt_reload = 0;
  ccnt = 1;
  bidx = acnt;
  cidx = 0;

  // src indexing
  result = EDMA3_DRV_setSrcIndex(self.hEdma, self.chIdRx, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcIndex(self.hEdma, self.pingLink, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcIndex(self.hEdma, self.pongLink, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  // dest indexing (bidx, cidx)
  result = EDMA3_DRV_setDestIndex(self.hEdma, self.chIdRx, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestIndex(self.hEdma, self.pingLink, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestIndex(self.hEdma, self.pongLink, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  // transfer params (acnt, bcnt, ccnt, bcnt_reload)
  result = EDMA3_DRV_setTransferParams(self.hEdma, self.chIdRx, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setTransferParams(self.hEdma, self.pingLink, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setTransferParams(self.hEdma, self.pongLink, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  // opt fields
  result = EDMA3_DRV_setOptField(self.hEdma, self.chIdRx,
                                 EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(self.hEdma, self.pingLink,
                                 EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(self.hEdma, self.pongLink,
                                 EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(self.hEdma, self.chIdRx,
                                 EDMA3_DRV_OPT_FIELD_TCCMODE,
                                 EDMA3_DRV_TCCMODE_NORMAL);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(self.hEdma, self.pingLink,
                                 EDMA3_DRV_OPT_FIELD_TCCMODE,
                                 EDMA3_DRV_TCCMODE_NORMAL);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(self.hEdma, self.pongLink,
                                 EDMA3_DRV_OPT_FIELD_TCCMODE,
                                 EDMA3_DRV_TCCMODE_NORMAL);
  logAssert(result == EDMA3_DRV_SOK);

  // Link channels to create the ping pong structure
  result = EDMA3_DRV_linkChannel(self.hEdma, self.chIdRx, self.pongLink);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_linkChannel(self.hEdma, self.pongLink, self.pingLink);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_linkChannel(self.hEdma, self.pingLink, self.pongLink);
  logAssert(result == EDMA3_DRV_SOK);

  /* Enable EDMA for the transfer */
  result = EDMA3_DRV_enableTransfer(self.hEdma, self.chIdRx,
                                    EDMA3_DRV_TRIG_MODE_EVENT);
  logAssert(result == EDMA3_DRV_SOK);
}

#define MINWAIT 15

static void configureStep(uint32_t order, uint32_t step, uint32_t channel,
                          uint32_t rate, uint32_t fifo)
{
  tscAdcStepCfg_t adcStepCfg;

  adcStepCfg.adcNegativeInpRef = TSCADC_NEGATIVE_REF_VREFN;
  adcStepCfg.adcPositiveInpRef = TSCADC_POSITIVE_REF_VREFP;
  adcStepCfg.adcNegativeInpChan = channel; //TSCADC_INPUT_VREFN;
  adcStepCfg.adcPositiveInpChan = channel;
  adcStepCfg.enableXppsw = FALSE;
  adcStepCfg.enableXnpsw = FALSE;
  adcStepCfg.enableYppsw = FALSE;
  adcStepCfg.enableXnnsw = FALSE;
  adcStepCfg.enableYpnsw = FALSE;
  adcStepCfg.enableYnnsw = FALSE;
  adcStepCfg.enableWpnsw = FALSE;
  adcStepCfg.fifo = fifo;
  adcStepCfg.mode = TSCADC_STEP_MODE_SW_ENABLED_CONTINUOUS;

  switch (rate)
  {
  case 62500:
    // total step times: 96+96+96+96 = 384 cycles
    // ignore every other step
    // subtract the minimum time of 15 cycles for each step
    // 24MHz/384 cycles = 62.5kHz sampling rate
    adcStepCfg.averaging = TSCADC_SAMPLES_NO_AVG;
    TSCADCStepConfig(SOC_ADC_TSC_0_REGS, step, FALSE, &adcStepCfg);
    switch (order)
    {
    case 1:
      TSCADCSetStepDelay(SOC_ADC_TSC_0_REGS, step, 0, 48 - MINWAIT);
      break;
    case 2:
    case 3:
    case 4:
      TSCADCSetStepDelay(SOC_ADC_TSC_0_REGS, step, 0, 48 - MINWAIT);
      break;
    }
    break;
  case 48000:
    // total step times: 128+124+124+124 = 500 cycles
    // ignore every other step
    // subtract the minimum time of 15 cycles for each step
    // 24MHz/500 cycles = 48kHz sampling rate
    adcStepCfg.averaging = TSCADC_SAMPLES_NO_AVG;
    TSCADCStepConfig(SOC_ADC_TSC_0_REGS, step, FALSE, &adcStepCfg);
    switch (order)
    {
    case 1:
      TSCADCSetStepDelay(SOC_ADC_TSC_0_REGS, step, 0, 64 - MINWAIT);
      break;
    case 2:
    case 3:
    case 4:
      TSCADCSetStepDelay(SOC_ADC_TSC_0_REGS, step, 0, 62 - MINWAIT);
      break;
    }
    break;
  case 96000:
    // total step times: 64+62+62+62 = 250 cycles
    // ignore every other step
    // subtract the minimum time of 15 cycles for each step
    // 24MHz/250 cycles = 96kHz sampling rate
    adcStepCfg.averaging = TSCADC_SAMPLES_NO_AVG;
    TSCADCStepConfig(SOC_ADC_TSC_0_REGS, step, FALSE, &adcStepCfg);
    switch (order)
    {
    case 1:
      TSCADCSetStepDelay(SOC_ADC_TSC_0_REGS, step, 0, 32 - MINWAIT);
      break;
    case 2:
    case 3:
    case 4:
      TSCADCSetStepDelay(SOC_ADC_TSC_0_REGS, step, 0, 31 - MINWAIT);
      break;
    }
    break;
  }

  /* Enable the ADC steps. */
  TSCADCStepEnable(SOC_ADC_TSC_0_REGS, step, TRUE);
}

void Adc_init()
{
  EDMA3_DRV_Handle hEdma = DMA_getHandle();
  memset(&self, 0, sizeof(self));

  self.hEdma = hEdma;
  self.burstSize = MIN(ADC_FRAME_LENGTH, 64);
  self.bufferSizeInBytes = ADC_NUM_CHANNELS * ADC_FRAME_LENGTH * sizeof(int);

  self.hEvents = Event_create(NULL, NULL);

  {
    Task_Params params;
    Task_Params_init(&params);
    params.priority = TASK_PRIORITY_ADC;
    params.env = (Ptr)"adc";
    Task_create(taskAdc, &params, NULL);
  }
}

void Adc_start(void)
{

  TSCADCAfePowerUp(SOC_ADC_TSC_0_REGS);

  /* Configure the ADC AFE clock. */
  TSCADCClkDivConfig(SOC_ADC_TSC_0_REGS, ADC_MODCLK, ADC_AFEINCLK);

  /* Enable the step ID tag. */
  TSCADCStepIdTagEnable(SOC_ADC_TSC_0_REGS, FALSE);

  TSCADCSetMode(SOC_ADC_TSC_0_REGS, TSCADC_MODE_GP_ADC);

  /* Disable the write protection for Step config registers. */
  TSCADCStepConfigProtectionEnable(SOC_ADC_TSC_0_REGS, FALSE);

  configureStep(1, 1, TSCADC_INPUT_CHANNEL5, globalConfig.sampleRate, 0);
  configureStep(1, 2, TSCADC_INPUT_VREFN, globalConfig.sampleRate, 1);

  configureStep(2, 3, TSCADC_INPUT_CHANNEL6, globalConfig.sampleRate, 0);
  configureStep(2, 4, TSCADC_INPUT_VREFN, globalConfig.sampleRate, 1);

  configureStep(3, 5, TSCADC_INPUT_CHANNEL7, globalConfig.sampleRate, 0);
  configureStep(3, 6, TSCADC_INPUT_VREFN, globalConfig.sampleRate, 1);

  configureStep(4, 7, TSCADC_INPUT_CHANNEL8, globalConfig.sampleRate, 0);
  configureStep(4, 8, TSCADC_INPUT_VREFN, globalConfig.sampleRate, 1);

  /* Enable the write protection for Step config registers. */
  TSCADCStepConfigProtectionEnable(SOC_ADC_TSC_0_REGS, TRUE);

  /* Configure the ADC FIFO. */
  // up to 64 samples
  TSCADCFifoConfig(SOC_ADC_TSC_0_REGS, 0, self.burstSize, FALSE);

  initDma();
  TSCADCDmaFifoEnable(SOC_ADC_TSC_0_REGS, 0, TRUE);
  TSCADCEnable(SOC_ADC_TSC_0_REGS, TRUE);
  self.running = true;
}

void Adc_stop(void)
{
  TSCADCEnable(SOC_ADC_TSC_0_REGS, FALSE);
  TSCADCDmaFifoEnable(SOC_ADC_TSC_0_REGS, 0, FALSE);
  self.running = false;
}

