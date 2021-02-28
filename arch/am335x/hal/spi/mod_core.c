#include <hal/priorities.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/mcspi.h>
#include <ti/am335x/error.h>
#include <hal/gpio.h>
#include <hal/modulation.h>
#include <hal/dma.h>
#include <hal/channels.h>
#include <hal/log.h>
#include <od/config.h>
#include "mod_core.h"

#include <ti/sysbios/knl/Task.h>

static struct
{
  Connection *mod0, *mod1;
} local;

void connection_TXDMAInit(Connection *con)
{
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;
  int32_t bidx = 0, cidx = 0, acnt = 0, bcnt = 0, ccnt = 0, bcnt_reload = 0;

  if (con->chIdTx == 0)
  {
    con->chIdTx = con->evtTx;
    con->linkIdTx = EDMA3_DRV_LINK_CHANNEL_WITH_TCC;
    con->tccTx = EDMA3_DRV_TCC_ANY;

    result = EDMA3_DRV_requestChannel(con->hEdma, &con->chIdTx, &con->tccTx,
                                      (EDMA3_RM_EventQueue)DMA_QUEUE_SPI,
                                      NULL, NULL);
    logAssert(result == EDMA3_DRV_SOK);

    result = EDMA3_DRV_requestChannel(con->hEdma, &con->linkIdTx,
                                      &con->tccTx,
                                      (EDMA3_RM_EventQueue)DMA_QUEUE_SPI,
                                      NULL, NULL);
    logAssert(result == EDMA3_DRV_SOK);
  }

  // Fill in PaRam sets

  // src params
  result = EDMA3_DRV_setSrcParams(con->hEdma, con->chIdTx,
                                  (uint32_t)con->bufTx,
                                  EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcParams(con->hEdma, con->linkIdTx,
                                  (uint32_t)con->bufTx,
                                  EDMA3_DRV_ADDR_MODE_INCR, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  // dst params
  result = EDMA3_DRV_setDestParams(
      con->hEdma, con->chIdTx,
      //con->spiBase + MCSPI_TX(0),
      con->spiBase + MCSPI_DAFTX, EDMA3_DRV_ADDR_MODE_FIFO,
      EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestParams(
      con->hEdma, con->linkIdTx,
      //con->spiBase + MCSPI_TX(0),
      con->spiBase + MCSPI_DAFTX, EDMA3_DRV_ADDR_MODE_FIFO,
      EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  acnt = MOD_TX_BUFFER_BYTES;
  bcnt = 1;
  bcnt_reload = 0;
  ccnt = 1;
  bidx = acnt;
  cidx = 0;

  // src indexing (bidx, cidx)
  result = EDMA3_DRV_setSrcIndex(con->hEdma, con->chIdTx, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcIndex(con->hEdma, con->linkIdTx, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  // dest indexing
  result = EDMA3_DRV_setDestIndex(con->hEdma, con->chIdTx, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestIndex(con->hEdma, con->linkIdTx, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  // transfer params
  result = EDMA3_DRV_setTransferParams(con->hEdma, con->chIdTx, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setTransferParams(con->hEdma, con->linkIdTx, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  // Link channels to repeat them indefinitely
  result = EDMA3_DRV_linkChannel(con->hEdma, con->chIdTx, con->linkIdTx);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_linkChannel(con->hEdma, con->linkIdTx, con->linkIdTx);
  logAssert(result == EDMA3_DRV_SOK);
}

void connection_RXDMAInit(Connection *con)
{
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;
  int32_t bidx = 0, cidx = 0, acnt = 0, bcnt = 0, ccnt = 0, bcnt_reload = 0;

  if (con->chIdRx == 0)
  {
    // Claim the dma event channel
    con->chIdRx = con->evtRx;
    // This can be any link channel since we will share the TCC with the master channel
    con->pingLink = EDMA3_DRV_LINK_CHANNEL_WITH_TCC;
    // Claim another channel that is open to use its TCC as the pong event
    con->pongLink = EDMA3_DRV_DMA_CHANNEL_ANY;

    con->pingTcc = EDMA3_DRV_TCC_ANY;
    con->pongTcc = EDMA3_DRV_TCC_ANY;

    result = EDMA3_DRV_requestChannel(con->hEdma, &con->chIdRx,
                                      &con->pingTcc,
                                      (EDMA3_RM_EventQueue)DMA_QUEUE_SPI,
                                      con->pingCb, con);

    logAssert(result == EDMA3_DRV_SOK);

    result = EDMA3_DRV_requestChannel(con->hEdma, &con->pongLink,
                                      &con->pongTcc,
                                      (EDMA3_RM_EventQueue)DMA_QUEUE_SPI,
                                      con->pongCb, con);

    logAssert(result == EDMA3_DRV_SOK);

    result = EDMA3_DRV_requestChannel(con->hEdma, &con->pingLink,
                                      &con->pingTcc,
                                      (EDMA3_RM_EventQueue)DMA_QUEUE_SPI,
                                      con->pingCb, con);

    logAssert(result == EDMA3_DRV_SOK);
  }

  // Fill in PaRam sets

  // src params
  result = EDMA3_DRV_setSrcParams(con->hEdma, con->chIdRx,
                                  //con->spiBase + MCSPI_RX(0),
                                  con->spiBase + MCSPI_DAFRX,
                                  EDMA3_DRV_ADDR_MODE_FIFO, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcParams(con->hEdma, con->pingLink,
                                  //con->spiBase + MCSPI_RX(0),
                                  con->spiBase + MCSPI_DAFRX,
                                  EDMA3_DRV_ADDR_MODE_FIFO, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcParams(con->hEdma, con->pongLink,
                                  //con->spiBase + MCSPI_RX(0),
                                  con->spiBase + MCSPI_DAFRX,
                                  EDMA3_DRV_ADDR_MODE_FIFO, EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  // dst params
  result = EDMA3_DRV_setDestParams(con->hEdma, con->chIdRx,
                                   (uint32_t)con->ping,
                                   EDMA3_DRV_ADDR_MODE_INCR,
                                   EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestParams(con->hEdma, con->pingLink,
                                   (uint32_t)con->ping,
                                   EDMA3_DRV_ADDR_MODE_INCR,
                                   EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestParams(con->hEdma, con->pongLink,
                                   (uint32_t)con->pong,
                                   EDMA3_DRV_ADDR_MODE_INCR,
                                   EDMA3_DRV_W32BIT);
  logAssert(result == EDMA3_DRV_SOK);

  con->rxBufferSizeInBytes = MOD_NUM_CHANNELS_PER_ADC * SPI_FRAME_LENGTH * sizeof(int);
  acnt = sizeof(int) * con->burstSize;
  bcnt = con->rxBufferSizeInBytes / acnt;
  bcnt_reload = 0;
  ccnt = 1;
  bidx = acnt;
  cidx = 0;

  // src indexing
  result = EDMA3_DRV_setSrcIndex(con->hEdma, con->chIdRx, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcIndex(con->hEdma, con->pingLink, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setSrcIndex(con->hEdma, con->pongLink, 0, 0);
  logAssert(result == EDMA3_DRV_SOK);

  // dest indexing (bidx, cidx)
  result = EDMA3_DRV_setDestIndex(con->hEdma, con->chIdRx, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestIndex(con->hEdma, con->pingLink, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setDestIndex(con->hEdma, con->pongLink, bidx, cidx);
  logAssert(result == EDMA3_DRV_SOK);

  // transfer params (acnt, bcnt, ccnt, bcnt_reload)
  result = EDMA3_DRV_setTransferParams(con->hEdma, con->chIdRx, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setTransferParams(con->hEdma, con->pingLink, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setTransferParams(con->hEdma, con->pongLink, acnt, bcnt,
                                       ccnt, bcnt_reload, EDMA3_DRV_SYNC_A);
  logAssert(result == EDMA3_DRV_SOK);

  // opt fields
  result = EDMA3_DRV_setOptField(con->hEdma, con->chIdRx,
                                 EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(con->hEdma, con->pingLink,
                                 EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(con->hEdma, con->pongLink,
                                 EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(con->hEdma, con->chIdRx,
                                 EDMA3_DRV_OPT_FIELD_TCCMODE,
                                 EDMA3_DRV_TCCMODE_NORMAL);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(con->hEdma, con->pingLink,
                                 EDMA3_DRV_OPT_FIELD_TCCMODE,
                                 EDMA3_DRV_TCCMODE_NORMAL);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_setOptField(con->hEdma, con->pongLink,
                                 EDMA3_DRV_OPT_FIELD_TCCMODE,
                                 EDMA3_DRV_TCCMODE_NORMAL);
  logAssert(result == EDMA3_DRV_SOK);

  // Link channels to create the ping pong structure
  result = EDMA3_DRV_linkChannel(con->hEdma, con->chIdRx, con->pongLink);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_linkChannel(con->hEdma, con->pongLink, con->pingLink);
  logAssert(result == EDMA3_DRV_SOK);

  result = EDMA3_DRV_linkChannel(con->hEdma, con->pingLink, con->pongLink);
  logAssert(result == EDMA3_DRV_SOK);
}

void connection_SPIInit(Connection *con)
{
  int32_t result;

  MCSPIReset(con->spiBase);
  MCSPICsEnable(con->spiBase, TRUE);
  MCSPISetCsPol(con->spiBase, MCSPI_CH_NUM_0, MCSPI_CS_POL_LOW);

  result = MCSPIModeConfig(con->spiBase, MCSPI_CH_NUM_0, MCSPI_MODE_MASTER,
                           MCSPI_CH_MULTI, //MCSPI_CH_SINGLE,
                           MCSPI_TRANSFER_MODE_TX_RX, con->pinMode);

  logAssert(result == S_PASS);

  MCSPIClkConfig(con->spiBase, MCSPI_CH_NUM_0, 48000000, 16000000,
                 MCSPI_CLK_MODE_1);

  MCSPITxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPIRxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPITurboModeEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);

  MCSPIChEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
}

void connection_ProgramRegisterWrite(Connection *con, uint32_t addr,
                                     uint32_t data, uint32_t retry)
{
  uint32_t tx; //, rx;
  uint32_t wait = 0;

  tx = ((addr << 9) | (1 << 8) | (data)) << 8;

  MCSPISetWordLength(con->spiBase, MCSPI_CH_NUM_0, 24);
  MCSPITxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPIRxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  //MCSPISetFifoTriggerLvl(con->spiBase, 4, 4, MCSPI_TRANSFER_MODE_TX_RX);
  //MCSPISetWordCount(con->spiBase, 1);
  MCSPIChEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);
  Task_sleep(10);

  //while (1) {

  //MCSPIIntrClear(con->spiBase, MCSPI_INTR_RX_FULL(0));
  wait = 0;
  MCSPITransmitData(con->spiBase, MCSPI_CH_NUM_0, tx);
  //while (!(MCSPIIntrStatus(con->spiBase) & MCSPI_INTR_RX_FULL(0))) {
  //while (!(MCSPIChStatus(con->spiBase, 0) & MCSPI_CH_STATUS_RXS_FULL)) {
  while (!(MCSPIChStatus(con->spiBase, 0) & MCSPI_CH_STATUS_END_OF_TRANS))
  {
    wait++;
    if (wait > 10000)
    {
      logFatal("ADS8688(%d): wait for transmission timed out.",
               con->Id);
    }
    Task_yield();
  }

  MCSPIChEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPITxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPIRxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPISetWordLength(con->spiBase, MCSPI_CH_NUM_0, 32);

  //logAssert(rx == data);
}

uint32_t connection_TestProgramRegister(Connection *con, uint32_t addr,
                                        uint32_t data, uint32_t iterations,
                                        uint32_t sleep)
{
  uint32_t failed = 0, count = 0, wait = 0;
  uint32_t tx, rx0, rx1;

  MCSPISetWordLength(con->spiBase, MCSPI_CH_NUM_0, 24);
  MCSPITxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);
  MCSPIRxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);
  MCSPIChEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);

  while (count < iterations)
  {

    if (count % 2)
    {
      data = count & 0xFF;
    }
    else
    {
      data = 0b10010001;
    }

    // program register write
    tx = ((addr << 9) | (1 << 8) | (data)) << 8;

    MCSPIIntrClear(con->spiBase, MCSPI_INTR_RX_FULL(0));
    wait = 0;
    MCSPITransmitData(con->spiBase, MCSPI_CH_NUM_0, tx);
    while (!(MCSPIIntrStatus(con->spiBase) & MCSPI_INTR_RX_FULL(0)))
    {
      wait++;
      if (wait > 10000)
      {
        logFatal("ADS8688(%d): wait for reception timed out.",
                 con->Id);
      }
      Task_yield();
    }

    rx0 = MCSPIReceiveData(con->spiBase, MCSPI_CH_NUM_0) & 0x00FFFFFF;
    MCSPIIntrClear(con->spiBase, MCSPI_INTR_RX_FULL(0));

    // program register read
    tx = ((addr << 9) | (0 << 8) | (0x0)) << 8;

    MCSPIIntrClear(con->spiBase, MCSPI_INTR_RX_FULL(0));
    wait = 0;
    MCSPITransmitData(con->spiBase, MCSPI_CH_NUM_0, tx);
    while (!(MCSPIIntrStatus(con->spiBase) & MCSPI_INTR_RX_FULL(0)))
    {
      wait++;
      if (wait > 10000)
      {
        logFatal("ADS8688(%d): wait for reception timed out.",
                 con->Id);
      }
      Task_yield();
    }

    rx1 = MCSPIReceiveData(con->spiBase, MCSPI_CH_NUM_0) & 0x00FFFFFF;
    MCSPIIntrClear(con->spiBase, MCSPI_INTR_RX_FULL(0));

    if (rx0 != data)
    {
      failed++;
    }
    logInfo("ADS8688(%d): %d failed of %d, data=0x%x rx0 = 0x%x rx1=0x%x",
            con->Id, failed, count, data, rx0, rx1);

    if (sleep)
      Task_sleep(sleep);
    else
      Task_yield();

    count++;
  }

  MCSPIChEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPITxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPIRxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPISetWordLength(con->spiBase, MCSPI_CH_NUM_0, 32);

  return failed;
}

#ifdef BUILDOPT_TESTING

void connection_TestConversions(Connection *con, uint32_t iterations,
                                uint32_t sleep)
{
  uint32_t i, count = 0, wait = 0;
  uint32_t rx[8];

  MCSPISetWordLength(con->spiBase, MCSPI_CH_NUM_0, 32);
  MCSPITxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);
  MCSPIRxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);
  MCSPIChEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);

  // first send auto_rst command and ignore the received data
  MCSPIIntrClear(con->spiBase, MCSPI_INTR_RX_FULL(0));
  wait = 0;
  MCSPITransmitData(con->spiBase, MCSPI_CH_NUM_0, ADS8688_CMD_AUTO_RST << 16);
  while (!(MCSPIIntrStatus(con->spiBase) & MCSPI_INTR_RX_FULL(0)))
  {
    wait++;
    if (wait > 10000)
    {
      logFatal("ADS8688(%d): wait for reception timed out.",
               con->Id);
    }
    Task_yield();
  }
  MCSPIReceiveData(con->spiBase, MCSPI_CH_NUM_0);
  MCSPIIntrClear(con->spiBase, MCSPI_INTR_RX_FULL(0));

  while (count < iterations)
  {
    for (i = 0; i < 8; i++)
    {
      MCSPIIntrClear(con->spiBase, MCSPI_INTR_RX_FULL(0));
      wait = 0;
      if (i == 7)
      {
        MCSPITransmitData(con->spiBase, MCSPI_CH_NUM_0,
                          ADS8688_CMD_AUTO_RST << 16);
      }
      else
      {
        MCSPITransmitData(con->spiBase, MCSPI_CH_NUM_0, 0);
      }
      while (!(MCSPIIntrStatus(con->spiBase) & MCSPI_INTR_RX_FULL(0)))
      {
        wait++;
        if (wait > 10000)
        {
          logFatal(
              "ADS8688(%d): wait for reception timed out.",
              con->Id);
        }
        Task_yield();
      }
      rx[i] = MCSPIReceiveData(con->spiBase, MCSPI_CH_NUM_0);
      MCSPIIntrClear(con->spiBase, MCSPI_INTR_RX_FULL(0));
    }

    logInfo(
        "ADS8688(%d): 0x%x 0x%x 0x%x 0x%x | 0x%x 0x%x 0x%x 0x%x",
        con->Id, rx[0], rx[1], rx[2], rx[3], rx[4], rx[5], rx[6],
        rx[7]);
    if (sleep)
      Task_sleep(sleep);
    else
      Task_yield();
    count++;
  }

  MCSPIChEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPITxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPIRxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);
  MCSPISetWordLength(con->spiBase, MCSPI_CH_NUM_0, 32);
}

#endif

void connection_Init(Connection *con, EDMA3_DRV_Handle hEdma)
{
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;

  con->hEdma = hEdma;
  con->keepRunning = true;

  // fill in the Tx buffer

#if 0
    // Auto Reset Mode
    con->bufTx[0] = 0;
    con->bufTx[1] = 0;
    con->bufTx[2] = 0;
    con->bufTx[3] = 0;
    con->bufTx[4] = 0;
    con->bufTx[5] = 0;
    con->bufTx[6] = 0;
    con->bufTx[7] = ADS8688_CMD_AUTO_RST<<16;
#else
  // Manual Sequence Mode
  con->bufTx[0] = ADS8688_CMD_MANUAL_CH_1 << 16;
  con->bufTx[1] = ADS8688_CMD_MANUAL_CH_2 << 16;
  con->bufTx[2] = ADS8688_CMD_MANUAL_CH_3 << 16;
  con->bufTx[3] = ADS8688_CMD_MANUAL_CH_4 << 16;
  con->bufTx[4] = ADS8688_CMD_MANUAL_CH_5 << 16;
  con->bufTx[5] = ADS8688_CMD_MANUAL_CH_6 << 16;
  con->bufTx[6] = ADS8688_CMD_MANUAL_CH_7 << 16;
  con->bufTx[7] = ADS8688_CMD_MANUAL_CH_0 << 16;
#endif
  result = Edma3_CacheFlush((uint32_t)con->bufTx, MOD_TX_BUFFER_BYTES);
  logAssert(result == EDMA3_DRV_SOK);

  // fill ping pong buffers with pattern
  int i, j;
  for (i = 0; i < MAX_SPI_FRAME_LENGTH; i++)
  {
    for (j = 0; j < MOD_NUM_CHANNELS_PER_ADC; j++)
    {
      con->ping[i * MOD_NUM_CHANNELS_PER_ADC + j] = j;
      con->pong[i * MOD_NUM_CHANNELS_PER_ADC + j] = j;
    }
  }
  result = Edma3_CacheFlush((uint32_t)con->ping, sizeof(con->ping));
  logAssert(result == EDMA3_DRV_SOK);
  result = Edma3_CacheFlush((uint32_t)con->pong, sizeof(con->pong));
  logAssert(result == EDMA3_DRV_SOK);

  con->range[0] = ADS8688_BIPOLAR_2500;
  con->range[1] = ADS8688_BIPOLAR_2500;
  con->range[2] = ADS8688_BIPOLAR_2500;
  con->range[3] = ADS8688_BIPOLAR_2500;
  con->range[4] = ADS8688_BIPOLAR_2500;
  con->range[5] = ADS8688_BIPOLAR_2500;
  con->range[6] = ADS8688_BIPOLAR_2500;
  con->range[7] = ADS8688_BIPOLAR_2500;
}

static void setChannelGainStage(uint32_t id, int channelOnADC, uint32_t range)
{
  uint32_t channel = id * MOD_NUM_CHANNELS_PER_ADC + channelOnADC;

  if (IS_AUDIO_INPUT(channel))
  {
    globalConfig.inputGain[channel] = (MOD_VOLTAGE_MAX / 7.0) / MOD_CODE_MAX;
  }
  else
  {
    globalConfig.inputGain[channel] = (MOD_VOLTAGE_MAX / FULLSCALE_IN_VOLTS) / MOD_CODE_MAX;
  }
}

void connection_Start(Connection *con)
{
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;
  int i;

  connection_RXDMAInit(con);
  connection_TXDMAInit(con);
  connection_SPIInit(con);

  // initialize the range of each channel
  for (i = 0; i < MOD_NUM_CHANNELS_PER_ADC; i++)
  {
    connection_ProgramRegisterWrite(con, ADS8688_PRG_INPUT_RANGE(i),
                                    con->range[i], 10);
    setChannelGainStage(con->Id, i, con->range[i]);
  }

  // enable all channels for auto sequencing
  connection_ProgramRegisterWrite(con, ADS8688_PRG_AUTO_SEQ_EN, 0b11111111,
                                  10);

  /* Enable EDMA for the transfer */
  result = EDMA3_DRV_enableTransfer(con->hEdma, con->chIdTx,
                                    EDMA3_DRV_TRIG_MODE_EVENT);
  logAssert(result == EDMA3_DRV_SOK);

  /* Enable EDMA for the reception */
  result = EDMA3_DRV_enableTransfer(con->hEdma, con->chIdRx,
                                    EDMA3_DRV_TRIG_MODE_EVENT);
  logAssert(result == EDMA3_DRV_SOK);

  MCSPISetWordLength(con->spiBase, MCSPI_CH_NUM_0, 32);
  MCSPITxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);
  MCSPIRxFifoEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);
  MCSPIFifoDataMgmtEnable(con->spiBase, TRUE);
  MCSPISetFifoTriggerLvl(con->spiBase, con->burstSize * 4, con->burstSize * 4,
                         MCSPI_TRANSFER_MODE_TX_RX);
  MCSPISetWordCount(con->spiBase, 0);

  MCSPIDmaEnable(con->spiBase, MCSPI_CH_NUM_0,
                 MCSPI_DMA_EVENT_TX | MCSPI_DMA_EVENT_RX, TRUE);
  MCSPIChEnable(con->spiBase, MCSPI_CH_NUM_0, TRUE);
}

void connection_Stop(Connection *con)
{
  EDMA3_DRV_Result result = EDMA3_DRV_SOK;

  MCSPIChEnable(con->spiBase, MCSPI_CH_NUM_0, FALSE);

  MCSPIDmaEnable(con->spiBase, MCSPI_CH_NUM_0,
                 MCSPI_DMA_EVENT_TX | MCSPI_DMA_EVENT_RX, FALSE);

  /* Disable EDMA for the transfer */
  result = EDMA3_DRV_disableTransfer(con->hEdma, con->chIdTx,
                                     EDMA3_DRV_TRIG_MODE_EVENT);
  logAssert(result == EDMA3_DRV_SOK);

  /* Disable EDMA for the reception */
  result = EDMA3_DRV_disableTransfer(con->hEdma, con->chIdRx,
                                     EDMA3_DRV_TRIG_MODE_EVENT);
  logAssert(result == EDMA3_DRV_SOK);

  MCSPIFifoDataMgmtEnable(con->spiBase, FALSE);
}

void Modulation_init()
{
  EDMA3_DRV_Handle hEdma = DMA_getHandle();

  local.mod0 = createMod0();
  local.mod1 = createMod1();

  Gpio_write(ADS8688_RESET, 0);
  Task_sleep(15);
  Gpio_write(ADS8688_RESET, 1);
  Task_sleep(15);

  connection_Init(local.mod0, hEdma);
  connection_Init(local.mod1, hEdma);
}

void Modulation_start(void)
{

  startMod0();
  startMod1();
}

void Modulation_restart(void)
{
  UInt key = Task_disable();
  local.mod0->keepRunning = false;
  local.mod1->keepRunning = false;
  Task_restore(key);
}

void Modulation_setChannelRange(uint32_t channel, uint32_t range)
{
  Connection *con;

  if (channel >= MOD_NUM_CHANNELS)
    return;

  if (channel < MOD_NUM_CHANNELS_PER_ADC)
  {
    con = local.mod0;
  }
  else
  {
    con = local.mod1;
    channel -= MOD_NUM_CHANNELS_PER_ADC;
  }

  con->range[channel] = range;
}

uint32_t Modulation_getChannelRange(uint32_t channel)
{
  Connection *con;

  if (channel >= MOD_NUM_CHANNELS)
    return MOD_BIPOLAR_2500;

  if (channel < MOD_NUM_CHANNELS_PER_ADC)
  {
    con = local.mod0;
  }
  else
  {
    con = local.mod1;
    channel -= MOD_NUM_CHANNELS_PER_ADC;
  }

  return con->range[channel];
}
