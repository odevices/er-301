#pragma once

#include <hal/constants.h>
#include <hal/channels.h>
#include <ti/sysbios/knl/Event.h>

#define ADS8688_PRG_AUTO_SEQ_EN (0x1u)
#define ADS8688_PRG_INPUT_RANGE(channel) (0x5u + channel)

/*
 * Input ranges
 * 0000 = Input range is set to �2.5 x VREF
 * 0001 = Input range is set to �1.25 x VREF
 * 0010 = Input range is set to �0.625 x VREF
 * 0101 = Input range is set to 0 to 2.5 x VREF
 * 0110 = Input range is set to 0 to 1.25 x VREF
 */
#define ADS8688_BIPOLAR_2500 0b0000
#define ADS8688_BIPOLAR_1250 0b0001
#define ADS8688_BIPOLAR_0625 0b0010
#define ADS8688_UNIPOLAR_2500 0b0101
#define ADS8688_UNIPOLAR_1250 0b0110

#define ADS8688_CMD_AUTO_RST (0xA000u)
#define ADS8688_CMD_NO_OP (0x0000u)

#define ADS8688_CMD_MANUAL_CH_0 (0xC000u)
#define ADS8688_CMD_MANUAL_CH_1 (0xC400u)
#define ADS8688_CMD_MANUAL_CH_2 (0xC800u)
#define ADS8688_CMD_MANUAL_CH_3 (0xCC00u)
#define ADS8688_CMD_MANUAL_CH_4 (0xD000u)
#define ADS8688_CMD_MANUAL_CH_5 (0xD400u)
#define ADS8688_CMD_MANUAL_CH_6 (0xD800u)
#define ADS8688_CMD_MANUAL_CH_7 (0xDC00u)

#define MOD_RX_BUFFER_MAXBYTES (MOD_NUM_CHANNELS_PER_ADC * MAX_SPI_FRAME_LENGTH * sizeof(int))
#define MOD_TX_BUFFER_BYTES (MOD_NUM_CHANNELS_PER_ADC * sizeof(int))

typedef struct
{
  int ping[CACHE_ALIGNED_SIZE(MOD_RX_BUFFER_MAXBYTES) / sizeof(int)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
  int pong[CACHE_ALIGNED_SIZE(MOD_RX_BUFFER_MAXBYTES) / sizeof(int)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
  uint32_t bufTx[CACHE_ALIGNED_SIZE(MOD_TX_BUFFER_BYTES) / sizeof(uint32_t)] __attribute__((aligned(CACHELINE_SIZE_MAX)));

  uint32_t burstSize;
  uint32_t rxBufferSizeInBytes;

  uint32_t chIdTx;
  uint32_t linkIdTx;
  uint32_t tccTx;

  uint32_t pingTcc;
  uint32_t pongTcc;
  uint32_t chIdRx;
  uint32_t pingLink;
  uint32_t pongLink;
  EDMA3_RM_TccCallback pingCb;
  EDMA3_RM_TccCallback pongCb;

  uint32_t evtTx;
  uint32_t evtRx;

  uint32_t spiBase;
  uint32_t pinMode;
  uint32_t Id;

  EDMA3_DRV_Handle hEdma;

  Event_Handle hEvents;
#define pingDone Event_Id_00
#define pongDone Event_Id_01

  uint32_t range[MOD_NUM_CHANNELS_PER_ADC];
  bool keepRunning;
} Connection;

void connection_TXDMAInit(Connection *con);
void connection_RXDMAInit(Connection *con);
void connection_SPIInit(Connection *con);
void connection_ProgramRegisterWrite(Connection *con, uint32_t addr, uint32_t data, uint32_t retry);
uint32_t connection_TestProgramRegister(Connection *con, uint32_t addr, uint32_t data, uint32_t iterations, uint32_t sleep);
void connection_TestConversions(Connection *con, uint32_t iterations, uint32_t sleep);
void connection_Init(Connection *con, EDMA3_DRV_Handle hEdma);
void connection_Start(Connection *con);
void connection_Stop(Connection *con);

Connection *createMod0(void);
void startMod0(void);

Connection *createMod1(void);
void startMod1(void);

#define MOD_TASK_TIMEOUT BIOS_WAIT_FOREVER
