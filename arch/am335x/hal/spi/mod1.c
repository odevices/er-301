#include "modx.h"

static Connection self;

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

static Void taskMod1(UArg arg0, UArg arg1)
{
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;

  while (1)
  {
    self.keepRunning = true;

    // clear all events
    Event_pend(self.hEvents, Event_Id_NONE, pingDone | pongDone,
               BIOS_NO_WAIT);

    connection_Start(&self);
    while (self.keepRunning)
    {
      // ping
      if (Event_pend(self.hEvents, pingDone, Event_Id_NONE,
                     MOD_TASK_TIMEOUT) == 0)
      {
        // timed out
        logInfo("mod1: timed out");
        break;
      }
      result = Edma3_CacheInvalidate((uint32_t)self.ping,
                                     self.rxBufferSizeInBytes);
      logAssert(result == EDMA3_DRV_SOK);
      Mod1_callback(self.ping);

      // pong
      if (Event_pend(self.hEvents, pongDone, Event_Id_NONE,
                     MOD_TASK_TIMEOUT) == 0)
      {
        // timed out
        logInfo("mod1: timed out");
        break;
      }
      result = Edma3_CacheInvalidate((uint32_t)self.pong,
                                     self.rxBufferSizeInBytes);
      logAssert(result == EDMA3_DRV_SOK);
      Mod1_callback(self.pong);
    }

    connection_Stop(&self);
  }
}

Connection *createMod1()
{
  memset(&self, 0, sizeof(self));
  self.Id = 1;
  self.burstSize = 8; // in words
  self.evtTx = EDMA3_CHA_MCSPI1_TX0;
  self.evtRx = EDMA3_CHA_MCSPI1_RX0;
  self.spiBase = SOC_SPI_1_REGS;
  self.hEvents = Event_create(NULL, NULL);
  self.pinMode = MCSPI_DATA_LINE_COMM_MODE_1;
  self.pingCb = onPingCompletion;
  self.pongCb = onPongCompletion;
  return &self;
}

void startMod1()
{
  Task_Params params;
  Task_Params_init(&params);
  params.priority = TASK_PRIORITY_SPI;
  params.env = (Ptr)"spi1";
  self.keepRunning = true;
  Task_create(taskMod1, &params, NULL);
}
