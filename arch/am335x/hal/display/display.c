#include <hal/display.h>
#include <hal/events.h>
#include <hal/log.h>
#include <hal/priorities.h>
#include <hal/fifo.h>
#include <hal/dma/edma3_drv.h>
#include <hal/gpio.h>
#include <hal/display/oled.h>

#include <ti/am335x/lcdc.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/hw_types.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/family/arm/a8/intcps/Hwi.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>


typedef struct sLocal
{
  DisplayBuffer ping, pong;
  fifo_t drawQ, sendQ;
  DisplayBuffer *lastPutBuffer;
  Hwi_Handle hDMADone;
  Event_Handle hEvents;
  bool started;
#define dmaDone Event_Id_00
#define queueInsert Event_Id_01
#define stopRequested Event_Id_02
} Local;

static Local local;

static inline void errataLiddDmaIssue(void)
{
  {
    uint32_t regVal = HW_RD_REG32(SOC_LCDC_0_REGS + LCDC_CLKC_RESET);

    HW_SET_FIELD(regVal, LCDC_CLKC_RESET_DMA, LCDC_CLKC_RESET_DMA_ENABLE);
    //HW_SET_FIELD(regVal, LCDC_CLKC_RESET_LIDD, LCDC_CLKC_RESET_LIDD_ENABLE);

    HW_WR_REG32((SOC_LCDC_0_REGS + LCDC_CLKC_RESET), regVal);
  }

  {
    uint32_t regVal = HW_RD_REG32(SOC_LCDC_0_REGS + LCDC_CLKC_RESET);

    HW_SET_FIELD(regVal, LCDC_CLKC_RESET_DMA, LCDC_CLKC_RESET_DMA_DISABLE);
    //HW_SET_FIELD(regVal, LCDC_CLKC_RESET_LIDD, LCDC_CLKC_RESET_LIDD_DISABLE);

    HW_WR_REG32((SOC_LCDC_0_REGS + LCDC_CLKC_RESET), regVal);
  }
}

static inline void disableDMA(void)
{
  // LIDD_CTRL register
  uint32_t regVal = HW_RD_REG32(SOC_LCDC_0_REGS + LCDC_LIDD_CTRL);

  // disable LIDD DMA
  HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_LIDD_DMA_EN, LCDC_LIDD_CTRL_LIDD_DMA_EN_DEACTIVATE);

  HW_WR_REG32((SOC_LCDC_0_REGS + LCDC_LIDD_CTRL), regVal);
}

static inline void enableDMAToCS0(void)
{
  // LIDD_CTRL register
  uint32_t regVal = HW_RD_REG32(SOC_LCDC_0_REGS + LCDC_LIDD_CTRL);

  // select DMA destination
  HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_DMA_CS0_CS1, LCDC_LIDD_CTRL_DMA_CS0_CS1_DMACS0);

  // enable LIDD DMA
  HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_LIDD_DMA_EN, LCDC_LIDD_CTRL_LIDD_DMA_EN_ACTIVATE);

  HW_WR_REG32((SOC_LCDC_0_REGS + LCDC_LIDD_CTRL), regVal);
}

static inline void enableDMAToCS1(void)
{
  // LIDD_CTRL register
  uint32_t regVal = HW_RD_REG32(SOC_LCDC_0_REGS + LCDC_LIDD_CTRL);

  // select DMA destination
  HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_DMA_CS0_CS1, LCDC_LIDD_CTRL_DMA_CS0_CS1_DMACS1);

  // enable LIDD DMA
  HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_LIDD_DMA_EN, LCDC_LIDD_CTRL_LIDD_DMA_EN_ACTIVATE);

  HW_WR_REG32((SOC_LCDC_0_REGS + LCDC_LIDD_CTRL), regVal);
}

static inline void sendToMain(uint8_t *buffer)
{
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;
  result = Edma3_CacheFlush((uint32_t)buffer, MAIN_FRAME_BUFFER_BYTES);
  logAssert(result == EDMA3_DRV_SOK);
  //errataLiddDmaIssue();
  LCDCDmaFrameBufConfig(
      SOC_LCDC_0_REGS,
      (uint32_t)buffer,
      (uint32_t)(&buffer[MAIN_FRAME_BUFFER_BYTES - 2]),
      LCDC_FRM_BUF_ID_0);
  enableDMAToCS0();
  Event_pend(local.hEvents, dmaDone, Event_Id_NONE, BIOS_WAIT_FOREVER);
  mainFlip();
}

static inline void sendToSub(uint8_t *buffer)
{
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;
  result = Edma3_CacheFlush((uint32_t)buffer, SUB_FRAME_BUFFER_BYTES);
  logAssert(result == EDMA3_DRV_SOK);
  errataLiddDmaIssue();
  LCDCDmaFrameBufConfig(
      SOC_LCDC_0_REGS,
      (uint32_t)buffer,
      (uint32_t)(&buffer[SUB_FRAME_BUFFER_BYTES - 2]),
      LCDC_FRM_BUF_ID_0);
  enableDMAToCS1();
  logAssert(result == EDMA3_DRV_SOK);
  Event_pend(local.hEvents, dmaDone, Event_Id_NONE, BIOS_WAIT_FOREVER);
}

static Void Display_task(UArg arg0, UArg arg1)
{
  local.started = true;
  while (1)
  {
    uint32_t e = Event_pend(local.hEvents, Event_Id_NONE, queueInsert | stopRequested, BIOS_WAIT_FOREVER);

    if (e & stopRequested)
    {
      local.started = false;
      return;
    }

    if (e & queueInsert)
    {
      DisplayBuffer *frame;
      while (fifo_pop(&local.sendQ, (uint32_t *)&frame))
      {
        sendToMain(frame->main);
        sendToSub(frame->sub);
        fifo_push(&local.drawQ, (uint32_t)frame);
        Events_push(EVENT_DISPLAY_READY);
      }
    }
  }
}

static Void hwiOnDMADone(UArg arg)
{
  uint32_t status;
  //Breakpoint();
  disableDMA();

  status = LCDCIntrStatus(SOC_LCDC_0_REGS);
  logAssert(status == (LCDC_INTR_MASK_FRM_DONE));
  LCDCIntrClear(SOC_LCDC_0_REGS, LCDC_INTR_MASK_FRM_DONE);

  Event_post(local.hEvents, dmaDone);
}

static void lcdControllerInit(void)
{
#if 0
	LCDCRasterEnable(SOC_LCDC_0_REGS,FALSE);
	Task_sleep(1);
	LCDCSoftwareResetEnable(SOC_LCDC_0_REGS,LCDC_SUB_MODULE_ID_CORE,TRUE);
	LCDCSoftwareResetEnable(SOC_LCDC_0_REGS,LCDC_SUB_MODULE_ID_DMA,TRUE);
	LCDCSoftwareResetEnable(SOC_LCDC_0_REGS,LCDC_SUB_MODULE_ID_LCD,TRUE);
	Task_sleep(1);
	LCDCSoftwareResetEnable(SOC_LCDC_0_REGS,LCDC_SUB_MODULE_ID_CORE,FALSE);
	LCDCSoftwareResetEnable(SOC_LCDC_0_REGS,LCDC_SUB_MODULE_ID_DMA,FALSE);
	LCDCSoftwareResetEnable(SOC_LCDC_0_REGS,LCDC_SUB_MODULE_ID_LCD,FALSE);
#endif

  LCDCClocksEnable(SOC_LCDC_0_REGS);

  // CTRL register
  {
    uint32_t regVal = HW_RD_REG32(SOC_LCDC_0_REGS + LCDC_LCD_CTRL);

    // CLKDIV (1 - 255), 200MHz/divider
    // div = 3 --> 55Hz
    // div = 5 --> 40Hz
    HW_SET_FIELD(regVal, LCDC_LCD_CTRL_CLKDIV, 3U);

    // MODESEL
    HW_SET_FIELD(regVal, LCDC_LCD_CTRL_MODESEL, LCDC_LCD_CTRL_MODESEL_LIDD);

    HW_WR_REG32((SOC_LCDC_0_REGS + LCDC_LCD_CTRL), regVal);
  }

  // LIDD_CTRL register
  {
    uint32_t regVal = HW_RD_REG32(SOC_LCDC_0_REGS + LCDC_LIDD_CTRL);

    // select LIDD mode
    HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_LIDD_MODE_SEL, LCDC_LIDD_CTRL_LIDD_MODE_SEL_ASYNC_MPU80);
    //HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_LIDD_MODE_SEL, LCDC_LIDD_CTRL_LIDD_MODE_SEL_SYNC_MPU80);

    // select DMA destination
    //HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_DMA_CS0_CS1, LCDC_LIDD_CTRL_DMA_CS0_CS1_DMACS0);
    //HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_DMA_CS0_CS1, LCDC_LIDD_CTRL_DMA_CS0_CS1_DMACS1);

    // enable LIDD DMA
    //HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_LIDD_DMA_EN, LCDC_LIDD_CTRL_LIDD_DMA_EN_ACTIVATE);

    // CS0/CS1 polarity
    HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_CS0_E0_POL, LCDC_LIDD_CTRL_CS0_E0_POL_NOINVERT);
    HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_CS1_E1_POL, LCDC_LIDD_CTRL_CS1_E1_POL_NOINVERT);

    // WS polarity
    HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_WS_DIR_POL, LCDC_LIDD_CTRL_WS_DIR_POL_NOINVERT);

    // RS polarity
    HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_RS_EN_POL, LCDC_LIDD_CTRL_RS_EN_POL_NOINVERT);

    // ALE polarity
    HW_SET_FIELD(regVal, LCDC_LIDD_CTRL_ALEPOL, LCDC_LIDD_CTRL_ALEPOL_NOINVERT);

    HW_WR_REG32((SOC_LCDC_0_REGS + LCDC_LIDD_CTRL), regVal);
  }

  // LIDD_CS0_CONF Register
  {
    uint32_t regVal = HW_RD_REG32(SOC_LCDC_0_REGS + LCDC_LIDD_CS0_CONF);

    // all timing is in units of MEMCLK (200MHz) cycles or 5ns

    // Write Strobe Set-Up cycles (0-31)  -->  t_AS = 10ns
    HW_SET_FIELD(regVal, LCDC_LIDD_CS0_CONF_W_SU, 3);
    //HW_SET_FIELD(regVal, LCDC_LIDD_CS0_CONF_W_SU, 2);

    // Write Strobe Duration cycles (0-63) --> t_PWLW = 60ns
    HW_SET_FIELD(regVal, LCDC_LIDD_CS0_CONF_W_STROBE, 15);
    //HW_SET_FIELD(regVal, LCDC_LIDD_CS0_CONF_W_STROBE, 10);

    // Write Strobe Hold cycles (0-15)  --> max(t_AH, t_DHW, t_PWHW) = 60ns min
    HW_SET_FIELD(regVal, LCDC_LIDD_CS0_CONF_W_HOLD, 12);
    //HW_SET_FIELD(regVal, LCDC_LIDD_CS0_CONF_W_HOLD, 1);

    // Read Strobe Set-Up cycles (0-31)  --> t_AS = 10 ns
    HW_SET_FIELD(regVal, LCDC_LIDD_CS0_CONF_R_SU, 3);

    // Read Strobe Duration cycles (0-63) --> t_PWLR = 150ns min
    HW_SET_FIELD(regVal, LCDC_LIDD_CS0_CONF_R_STROBE, 32);

    // Read Strobe Hold cycles (0-15)  -> max(t_AH, t_DHR, t_PWHR) = 60ns
    HW_SET_FIELD(regVal, LCDC_LIDD_CS0_CONF_R_HOLD, 12);

    // TA cycles (0-3)
    HW_SET_FIELD(regVal, LCDC_LIDD_CS0_CONF_TA, 1);

    HW_WR_REG32((SOC_LCDC_0_REGS + LCDC_LIDD_CS0_CONF), regVal);
  }

  // LIDD_CS1_CONF Register
  {
    uint32_t regVal = HW_RD_REG32(SOC_LCDC_0_REGS + LCDC_LIDD_CS1_CONF);

    // all timing is in units of MEMCLK (200MHz) cycles or 5ns

    // Write Strobe Set-Up cycles (0-31)  -->  t_AS = 10ns
    HW_SET_FIELD(regVal, LCDC_LIDD_CS1_CONF_W_SU, 3);

    // Write Strobe Duration cycles (0-63) --> t_PWLW = 60ns
    HW_SET_FIELD(regVal, LCDC_LIDD_CS1_CONF_W_STROBE, 15);
    //HW_SET_FIELD(regVal, LCDC_LIDD_CS1_CONF_W_STROBE, 5);

    // Write Strobe Hold cycles (0-15)  --> max(t_AH, t_DHW, t_PWHW) = 60ns min
    HW_SET_FIELD(regVal, LCDC_LIDD_CS1_CONF_W_HOLD, 12);
    //HW_SET_FIELD(regVal, LCDC_LIDD_CS1_CONF_W_HOLD, 5);

    // Read Strobe Set-Up cycles (0-31)  --> t_AS = 10 ns
    HW_SET_FIELD(regVal, LCDC_LIDD_CS1_CONF_R_SU, 3);

    // Read Strobe Duration cycles (0-63) --> t_PWLR = 120ns min
    HW_SET_FIELD(regVal, LCDC_LIDD_CS1_CONF_R_STROBE, 24);

    // Read Strobe Hold cycles (0-15)  -> max(t_AH, t_DHR, t_PWHR) = 60ns
    HW_SET_FIELD(regVal, LCDC_LIDD_CS1_CONF_R_HOLD, 12);

    // TA cycles (0-3)
    HW_SET_FIELD(regVal, LCDC_LIDD_CS1_CONF_TA, 1);

    HW_WR_REG32((SOC_LCDC_0_REGS + LCDC_LIDD_CS1_CONF), regVal);
  }

  // LCDDMA_CTRL Register
  {
    uint32_t regVal = HW_RD_REG32(SOC_LCDC_0_REGS + LCDC_LCDDMA_CTRL);

    // DMA Burst Size (2^n where n is 0-4)
    HW_SET_FIELD(regVal, LCDC_LCDDMA_CTRL_BURST_SIZE, 4);

    // DMA Byte Swap
    HW_SET_FIELD(regVal, LCDC_LCDDMA_CTRL_BYTE_SWAP, 0);

    // DMA Frame Mode (single or double buffer)
    HW_SET_FIELD(regVal, LCDC_LCDDMA_CTRL_FRAME_MODE, LCDC_LCDDMA_CTRL_FRAME_MODE_ONE);

    // DMA Priority (0-7, 0 is highest priority)
    HW_SET_FIELD(regVal, LCDC_LCDDMA_CTRL_DMA_MASTER_PRIO, 3);

    // DMA FIFO Threshold (8 bytes * 2^n where n is 0-6)
    HW_SET_FIELD(regVal, LCDC_LCDDMA_CTRL_TH_FIFO_READY, 0);

    HW_WR_REG32((SOC_LCDC_0_REGS + LCDC_LCDDMA_CTRL), regVal);
  }
}

void Display_init(void)
{
  int i;

  // Make sure power is off
  Gpio_write(OLED_POWER, 0);

  local.started = false;

  for (i = 0; i < MAIN_FRAME_BUFFER_BYTES; i++)
  {
    local.ping.main[i] = 0;
    local.pong.main[i] = 0;
  }

  for (i = 0; i < SUB_FRAME_BUFFER_BYTES; i++)
  {
    local.ping.sub[i] = 0;
    local.pong.sub[i] = 0;
  }

  fifo_init(&local.drawQ);
  fifo_init(&local.sendQ);
  local.hEvents = Event_create(NULL, NULL);

  Display_putBuffer(&local.ping);
  Display_putBuffer(&local.pong);

  {
    Hwi_Params params;
    Hwi_Params_init(&params);
    params.priority = HWI_PRIORITY_DISPLAY;
    local.hDMADone = Hwi_create(LCDC_INT, hwiOnDMADone, &params, NULL);
  }

  lcdControllerInit();
  //errataLiddDmaIssue();

  // put both OLEDs into reset
  Gpio_write(MAIN_OLED_RESET, 0);
  Gpio_write(SUB_OLED_RESET, 0);

  Task_sleep(10);

  // bring both OLEDs out of reset
  Gpio_write(MAIN_OLED_RESET, 1);
  Gpio_write(SUB_OLED_RESET, 1);
  Task_sleep(1);

  // Run initialization commands through both displays
  mainInit();
  subInit();

  // clear out the display RAM
  mainFill(0x0);
  subFill(0x0);

  // turn on OLED power
  Gpio_write(OLED_POWER, 1);
  Task_sleep(300);

  // Final display on commands
  mainDisplayOn();
  subDisplayOn();
}

void Display_deinit()
{
  Display_stop();
  // turn off OLED power
  Gpio_write(OLED_POWER, 0);
}

void Display_start(void)
{
  if (!local.started)
  {
    Task_Params params;

    LCDCIntrClear(SOC_LCDC_0_REGS, LCDC_INTR_MASK_ALL);
    LCDCIntrEnable(SOC_LCDC_0_REGS, LCDC_INTR_MASK_FRM_DONE);

    Task_Params_init(&params);
    params.priority = TASK_PRIORITY_DISPLAY;
    params.env = (Ptr)"display";
    Task_create(Display_task, &params, NULL);
  }
}

void Display_stop()
{
  if (local.started)
  {
    Event_post(local.hEvents, stopRequested);
    while (local.started)
    {
      Task_sleep(1);
    }
    disableDMA();
    LCDCIntrClear(SOC_LCDC_0_REGS, LCDC_INTR_MASK_ALL);
    LCDCIntrDisable(SOC_LCDC_0_REGS, LCDC_INTR_MASK_FRM_DONE);
  }
}

DisplayBuffer *Display_getBuffer()
{
  uint32_t *handle;
  if (fifo_pop(&local.drawQ, (uint32_t *)&handle))
  {
    return (DisplayBuffer *)handle;
  }
  else
  {
    return NULL;
  }
}

void Display_putBuffer(DisplayBuffer *handle)
{
  local.lastPutBuffer = handle;
  fifo_push(&local.sendQ, (uint32_t)handle);
  Event_post(local.hEvents, queueInsert);
}

DisplayBuffer *Display_getLastPutBuffer()
{
  return local.lastPutBuffer;
}
