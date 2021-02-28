/**
 * \file       mcspi.h
 *
 * \brief      This file contains the function prototypes for the device
 *             abstraction layer for MCSPI. It also contains necessary
 *             structure, enum and macro definitions.
 *
 * \details    Programming sequence of MCSPI is as follows:
 *             -# MCSPI can be put into reset by calling the API
 *                MCSPIReset.
 *             -# MCSPI can be configured into 3 pin(i.e. no CS enabled)
 *                or 4 pin(i.e. CS enabled) by calling the API MCSPICsEnable.
 *             -# Polarity of CS can be configured by calling the API
 *                MCSPISetCsPol.
 *             -# Master Mode configurations. //Need to be filled.
 *             -# MCSPI output clock can be configured by calling the API
 *                MCSPIClkConfig.
 *             -# Word length of MCSPI can be configured by calling the API
 *                MCSPISetWordLength.
 *             -# Transmit Fifo can be enabled/disabled by calling the API
 *                MCSPITxFifoEnable.
 *             -# Receive Fifo can be enabled/disabled by calling the API
 *                MCSPIRxFifoEnable.
 *             -# Assert the CS of MCSPI by calling the API MCSPICsAssert.
 *             -# Deassert the CS of MCSPI by calling the API MCSPICsDeAssert.
 *             -# MCSPI interrupts can be enabled by calling the API
 *                MCSPIIntrEnable.
 *             -# MCSPI channel can be enabled/disabled by calling the API
 *                MCSPIChEnable.
 *             -# Status on MCSPI interrupts can be checked by calling the API
 *                MCSPIIntrStatus.
 *             -# MCSPI interrupts can be cleared by calling the API
 *                MCSPIIntrClear.
 *             -# Data to be transmitted is to be written to the transmit
 *                register by calling the API MCSPITransmitData.
 *             -# MCSPI interrupts can be disabled by calling the API
 *                MCSPIIntrDisable.
 *             -# Received data into the MCSPI receive register can be read
 *                by calling the API MCSPIReceiveData.
 */

/**
 * \copyright  Copyright (C) 2013 Texas Instruments Incorporated -
 *             http://www.ti.com/
 */

/*
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef MCSPI_H_
#define MCSPI_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "hw_mcspi.h"
#include "misc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*              API compatibility with Starterware 2.0                        */
/* ========================================================================== */
#define McSPIWordLengthSet            MCSPISetWordLength
#define McSPICSAssert                 MCSPICsAssert
#define McSPICSDeAssert               MCSPICsDeAssert
#define McSPIReset                    MCSPIReset
#define McSPITxFIFOConfig             MCSPITxFifoEnable
#define McSPIRxFIFOConfig             MCSPIRxFifoEnable
#define McSPIFIFOTrigLvlSet           MCSPISetFifoTriggerLvl
#define McSPIWordCountSet             MCSPISetWordCount
#define McSPIIntEnable                MCSPIIntrEnable
#define McSPIIntDisable               MCSPIIntrDisable
#define McSPIInitDelayConfig          MCSPISetInitialDelay
#define McSPITransmitData             MCSPITransmitData
#define McSPIReceiveData              MCSPIReceiveData
#define McSPIIntStatusGet             MCSPIIntrStatus
#define McSPIIntStatusClear           MCSPIIntrClear
#define McSPIChannelStatusGet         MCSPIChStatus
#define McSPIMultipleWordAccessConfig MCSPIMultipleWordAccessEnable
#define McSPIFIFODatManagementConfig  MCSPIFifoDataMgmtEnable
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 * \name    Macros to handle interrupts.
 *
 * \details These macros can be passed to enable/disable/clear status
 *          of the various interrupts of MCSPI peripheral. These macros
 *          can also be used to check the status obtained from
 *          'MCSPIIntrStatus' API.
 *          Where ch can be in the range of
 *          - 0 <= ch <= 3
 */

/** @{ */
/** \brief MCSPI Tx empty interrupt. */
#define MCSPI_INTR_TX_EMPTY(ch)(MCSPI_IRQSTS_TX0_EMPTY_MASK << ((ch) * 4))

/** \brief MCSPI Tx underflow interrupt. */
#define MCSPI_INTR_TX_UNDERFLOW(ch)(MCSPI_IRQSTS_TX0_UNDERFLOW_EN_MASK << \
                                   ((ch) * 4))

/** \brief MCSPI Rx full interrupt. */
#define MCSPI_INTR_RX_FULL(ch) (MCSPI_IRQSTS_RX0_FULL_MASK << ((ch) * 4))

/** \brief MCSPI Rx0 overflow interrupt. */
#define MCSPI_INTR_RX0_OVERFLOW  (MCSPI_IRQSTS_RX0_OVERFLOW_MASK)

/** \brief MCSPI end of word interrupt. */
#define MCSPI_INTR_EOW         (MCSPI_IRQSTS_EOW_MASK)
/** @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *
 *  \brief    Enumerates the different channel numbers available for MCSPI module.
 *            Possible channel numbers range from 0 - 3.
 *            In AM335x support is present only for Channels 0 and 1
 */
typedef enum mcspiChNum
{
    MCSPI_CH_NUM_MIN,
    /**< Minimum value of the enum. */
    MCSPI_CH_NUM_0 = MCSPI_CH_NUM_MIN,
    /**< MCSPI channel 0. */
    MCSPI_CH_NUM_1,
    /**< MCSPI channel 1. */
    MCSPI_CH_NUM_2,
    /**< MCSPI channel 2. */
    MCSPI_CH_NUM_3,
    /**< MCSPI channel 3.*/
    MCSPI_CH_NUM_MAX = MCSPI_CH_NUM_3
    /**< Maximum value of the enum. */
} mcspiChNum_t;

/**
 *
 *  \brief    Enumerates the different clock modes available for MCSPI module.
 */
typedef enum mcspiClkMode
{
    MCSPI_CLK_MODE_0 = (MCSPI_CHCONF_POL_ACTIVEHIGH | MCSPI_CHCONF_PHA_ODD),
    /**< MCSPI clock mode 0, POL = 0, PHA = 0. */
    MCSPI_CLK_MODE_1 = (MCSPI_CHCONF_POL_ACTIVEHIGH | MCSPI_CHCONF_PHA_MASK),
    /**< MCSPI clock mode 1, POL = 0, PHA = 1. */
    MCSPI_CLK_MODE_2 = (MCSPI_CHCONF_POL_MASK | MCSPI_CHCONF_PHA_ODD),
    /**< MCSPI clock mode 2, POL = 1, PHA = 0. */
    MCSPI_CLK_MODE_3 = (MCSPI_CHCONF_POL_MASK | MCSPI_CHCONF_PHA_MASK)
    /**< MCSPI clock mode 3, POL = 1, PHA = 1.*/
} mcspiClkMode_t;

/**
 *
 *  \brief    Enumerates the different CS polarity values available for MCSPI
 *            module.
 */
typedef enum mcspiCsPol
{
    MCSPI_CS_POL_HIGH = MCSPI_CHCONF_EPOL_ACTIVEHIGH,
    /**< MCSPI CS polarity is maintained high. */
    MCSPI_CS_POL_LOW = MCSPI_CHCONF_EPOL_ACTIVELOW
    /**< MCSPI CS polarity is mainitained low. */
} mcspiCsPol_t;

/**
 *  \brief    Enumerates the different CS time control numbers available for
 *            MCSPI module.
 */
typedef enum mcspiCsTimeCtrl
{
    MCSPI_CS_TIME_CTRL_HALF = MCSPI_CHCONF_TCS_HALF_CYCLEDLY,
    /**< 0.5 clock cycles delay is used. */
    MCSPI_CS_TIME_CTRL_ONE_HALF = MCSPI_CHCONF_TCS_ONE_HALF_CYCLEDLY,
    /**< 1.5 clock cycles delay is used. */
    MCSPI_CS_TIME_CTRL_TWO_HALF = MCSPI_CHCONF_TCS_TWO_HALF_CYCLEDLY,
    /**< 2.5 clock cycles delay is used. */
    MCSPI_CS_TIME_CTRL_THREE_HALF = MCSPI_CHCONF_TCS_THREE_HALF_CYCLEDLY
    /**< 3.5 clock cycles delay is used. */
} mcspiCsTimeCtrl_t;

/**
 *  \brief    Enumerates the Start bit polarities available for MCSPI module.
 */
typedef enum mcspiStartBitPol
{
    MCSPI_START_BIT_POL_LOW = MCSPI_CHCONF_SBPOL_LOWLEVEL,
    /**< Low polarity is used for Start bit. */
    MCSPI_START_BIT_POL_HIGH = MCSPI_CHCONF_SBPOL_HIGHLEVEL
    /**< High polarity is used for Start bit. */
} mcspiStartBitPol_t;

/**
 *  \brief    Enumerates the available channel configuration values for
 *            MCSPI module.
 */
typedef enum mcspiCh
{
    MCSPI_CH_SINGLE = MCSPI_MODULCTRL_SINGLE_MODSINGLE,
    /**< Use single channel of MCSPI. */
    MCSPI_CH_MULTI = MCSPI_MODULCTRL_SINGLE_MODMULTI
    /**< Use Multi channel of MCSPI. */
} mcspiCh_t;

/**
 *  \brief    Enumerates the available Tx/Rx modes for
 *            MCSPI module.
 */
typedef enum mcspiTransferMode
{
    MCSPI_TRANSFER_MODE_TX_RX = MCSPI_CHCONF_TRM_TRANSRECEI,
    /**< Enable Tx and Rx mode. */
    MCSPI_TRANSFER_MODE_RX_ONLY = MCSPI_CHCONF_TRM_RECEIVONLY,
    /**< Enable Rx only mode. */
    MCSPI_TRANSFER_MODE_TX_ONLY = MCSPI_CHCONF_TRM_TRANSONLY
    /**< Enable Tx only mode. */
} mcspiTransferMode_t;

/**
 *  \brief    Enumerates the available modes for SPIDAT0 and SPIDAT1 pins of
 *            MCSPI module.
 */
typedef enum mcspiDataLineCommMode
{
    MCSPI_DATA_LINE_COMM_MODE_0 = (MCSPI_CHCONF_IS_LINE0 |
                                   MCSPI_CHCONF_DPE1_ENABLED |
                                   MCSPI_CHCONF_DPE0_ENABLED),
    /**< Pin mode 0 */
    MCSPI_DATA_LINE_COMM_MODE_1 = (MCSPI_CHCONF_IS_LINE0 |
                                   MCSPI_CHCONF_DPE1_ENABLED |
                                   MCSPI_CHCONF_DPE0_MASK),
    /**< Pin mode 1 */
    MCSPI_DATA_LINE_COMM_MODE_2 = (MCSPI_CHCONF_IS_LINE0 |
                                   MCSPI_CHCONF_DPE1_MASK |
                                   MCSPI_CHCONF_DPE0_ENABLED),
    /**< Pin mode 2 */
    MCSPI_DATA_LINE_COMM_MODE_3 = (MCSPI_CHCONF_IS_LINE0 |
                                   MCSPI_CHCONF_DPE1_MASK |
                                   MCSPI_CHCONF_DPE0_MASK),
    /**< Pin mode 3 */
    MCSPI_DATA_LINE_COMM_MODE_4 = (MCSPI_CHCONF_IS_MASK |
                                   MCSPI_CHCONF_DPE1_ENABLED |
                                   MCSPI_CHCONF_DPE0_ENABLED),
    /**< Pin mode 4 */
    MCSPI_DATA_LINE_COMM_MODE_5 = (MCSPI_CHCONF_IS_MASK |
                                   MCSPI_CHCONF_DPE1_ENABLED |
                                   MCSPI_CHCONF_DPE0_MASK),
    /**< Pin mode 5 */
    MCSPI_DATA_LINE_COMM_MODE_6 = (MCSPI_CHCONF_IS_MASK |
                                   MCSPI_CHCONF_DPE1_MASK |
                                   MCSPI_CHCONF_DPE0_ENABLED),
    /**< Pin mode 6 */
    MCSPI_DATA_LINE_COMM_MODE_7 = (MCSPI_CHCONF_IS_MASK |
                                   MCSPI_CHCONF_DPE1_MASK |
                                   MCSPI_CHCONF_DPE0_MASK)
    /**< Pin mode 7 */
} mcspiDataLineCommMode_t;

/**
 *  \brief    Enumerates the values present for configuring the Initial delay.
 */
typedef enum mcspiInitDelayCfg
{
    MCSPI_INIT_DELAY_CFG_0 = MCSPI_MODULCTRL_INITDLY_NODELAY,
    /**< No delay. */
    MCSPI_INIT_DELAY_CFG_4 = MCSPI_MODULCTRL_INITDLY_4CLKDLY,
    /**< Delay of 4 clock cycles used. */
    MCSPI_INIT_DELAY_CFG_8 = MCSPI_MODULCTRL_INITDLY_8CLKDLY,
    /**< Delay of 8 clock cycles used. */
    MCSPI_INIT_DELAY_CFG_16 = MCSPI_MODULCTRL_INITDLY_16CLKDLY,
    /**< Delay of 16 clock cycles used. */
    MCSPI_INIT_DELAY_CFG_32 = MCSPI_MODULCTRL_INITDLY_32CLKDLY
    /**< Delay of 32 clock cycles used. */
} mcspiInitDelayCfg_t;

/**
 *  \brief    Enumerates the values used to check the status of
 *            MCSPI channel status registers and FIFO.
 */
typedef enum mcspiChStatus
{
    MCSPI_CH_STATUS_RXS_FULL = MCSPI_CHSTAT_RXS_MASK,
    /**< Receive register status. */
    MCSPI_CH_STATUS_TXS_EMPTY = MCSPI_CHSTAT_TXS_MASK,
    /**< Transmit register status. */
    MCSPI_CH_STATUS_END_OF_TRANS = MCSPI_CHSTAT_EOT_MASK,
    /**< End of transfer status. */
    MCSPI_CH_STATUS_TX_BUF_EMPTY = MCSPI_CHSTAT_TXFFE_MASK,
    /**< Transmit buffer empty status. */
    MCSPI_CH_STATUS_TX_BUF_FULL = MCSPI_CHSTAT_TXFFF_MASK,
    /**< Transmit buffer full status. */
    MCSPI_CH_STATUS_RX_BUF_EMPTY = MCSPI_CHSTAT_RXFFE_MASK,
    /**< Receive buffer empty status. */
    MCSPI_CH_STATUS_RX_BUF_FULL = MCSPI_CHSTAT_RXFFF_MASK
    /**< Receive buffer full status. */
} mcspiChStatus_t;

/**
 *  \brief    Enumerates the values used to enable/disable
 *            multiple word OCP access.
 */
typedef enum mcspiMultiWordAccess
{
    MCSPI_MULTI_WRD_ACCESS_ENABLE = MCSPI_MODULCTRL_MOA_MULTIACCES,
    /**< Enable multi word OCP access. */
    MCSPI_MULTI_WRD_ACCESS_DISABLE = MCSPI_MODULCTRL_MOA_NOMULTIACCESS
    /**< Disable multi word OCP access. */
} mcspiMultiWordAccess_t;

/**
 *  \brief    Enumerates the values used to enable/disable MCSPI Tx/Rx
 *            events to DMA.
 */
typedef enum mcspiDmaEvent
{
    MCSPI_DMA_EVENT_RX = MCSPI_CHCONF_DMAR_MASK,
    /**< MCSPI Rx event to DMA */
    MCSPI_DMA_EVENT_TX = MCSPI_CHCONF_DMAW_MASK
    /**< MCSPI Tx event to DMA */
} mcspiDmaEvent_t;

/**
 *  \brief    Enumerates the values used to configure the MCSPI in
 *            Master/Slave mode of operation.
 */
typedef enum mcspiMode
{
    MCSPI_MODE_MASTER = MCSPI_MODULCTRL_MS_MASTER,
    /**< MCSPI Master mode of operation. */
    MCSPI_MODE_SLAVE = MCSPI_MODULCTRL_MS_SLAVE
    /**< MCSPI Slave mode of operation. */
} mcspiMode_t;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief     Configures the clock.
 *
 * \details   This API will configure the clkD and extClk fields to generate
 *            required spi clock depending on the type of granularity. It will
 *            also set the phase and polarity of spiClk by the clkMode field.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     spiInClk       Clock frequency given to the MCSPI module.
 * \param     spiOutClk      Clock frequency on the MCSPI bus.
 * \param     clkMode        Clock mode.
 *            clkMode will contain values from the following enum.
 *            #mcspiClkMode_t.
 *
 * \note      Few points to be noted
 *            -# clkMode depends on phase and polarity of MCSPI clock.
 *            -# To pass the desired value for clkMode please refer the
 *               MCSPI_CH(i)CONF register.
 *            -# Please understand the polarity and phase of the slave device
 *               connected and accordingly set the clkMode.
 *            -# MCSPIClkConfig does not have any significance in slave mode
 *               because the clock signal required for communication is generated
 *               by the master device.
 */
void MCSPIClkConfig(uint32_t baseAddr,
                    uint32_t chNum,
                    uint32_t spiInClk,
                    uint32_t spiOutClk,
                    uint32_t clkMode);

/**
 * \brief     Configure the word length.
 *
 * \details   This API will configure the length of MCSPI word used for
 *            communication.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     wordLength     Length of a data word used for MCSPI communication.
 *            where 4 <= wordLength <= 32.
 *
 * \note      Few points to be noted
 *            -# wordLength can vary from 4-32 bits length.
 *            -# To program the required value of wordLength please refer
 *               the MCSPI_CH(i)CONF register.
 */
void MCSPISetWordLength(uint32_t baseAddr, uint32_t chNum, uint32_t wordLength);

/**
 * \brief     Enable/Disable the chip select pin.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     enableChipSel  Enable/Disable chip select.
 *            enableChipSel will take the following values.
 *            -# TRUE - Enable chip select.
 *            -# FALSE - Disable chip select.
 *
 * \note      Modification of CS polarity, SPI clock phase and polarity
 *            is not allowed when CS is enabled.
 */
void MCSPICsEnable(uint32_t baseAddr, uint32_t enableChipSel);

/**
 * \brief     Set the chip select polarity.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     csPol          Polarity of CS line.
 *            csPol will contain values from the following enum.
 *            #mcspiCsPol_t
 */
void MCSPISetCsPol(uint32_t baseAddr, uint32_t chNum, uint32_t csPol);

/**
 * \brief     Configure the chip select time control value.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     csTimeControl  Chip Select time control.
 *            csTimeControl will contain values from the following enum.
 *            #mcspiCsTimeCtrl_t
 */
void MCSPISetCsTimeControl(uint32_t baseAddr,
                           uint32_t chNum,
                           uint32_t csTimeControl);

/**
 * \brief     Assert the chip select line.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 */
void MCSPICsAssert(uint32_t baseAddr, uint32_t chNum);

/**
 * \brief     Deassert the chip select line.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 */
void MCSPICsDeAssert(uint32_t baseAddr, uint32_t chNum);

/**
 * \brief     Enable/Disable start bit for SPI transfer.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     enableStartBit Enable/Disable the start bit
 *            enableStartBit will take the following values.
 *            -# TRUE - Enable start bit.
 *            -# FALSE - Disable start bit.
 */
void MCSPIStartBitEnable(uint32_t baseAddr,
                         uint32_t chNum,
                         uint32_t enableStartBit);

/**
 * \brief     Configure the start bit polarity.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     startBitPol    Polarity of start bit.
 *            startBitPol will contain values from the following enum.
 *            #mcspiStartBitPol_t
 */
void MCSPISetStartBitPol(uint32_t baseAddr,
                         uint32_t chNum,
                         uint32_t startBitPol);

/**
 * \brief     Configure the MCSPI controller in Master/Slave modes and other
 *            required parameters of the mode.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     spiMode        Master/Slave mode of operation.
 * \param     chMode         Single/Multi channel mode.
 *            chMode will contain values from the following enum.
 *            #mcspiCh_t.
 * \param     trMode         Tx/Rx mode used in master configuration.
 *            trMode will contain values from the following enum.
 *            #mcspiTransferMode_t.
 * \param     pinMode        Interface mode and pin assignment.
 *            pinMode will contain values from the following enum.
 *            #mcspiDataLineCommMode_t.
 *
 * \retval    S_PASS   Combination of trMode and pinmode is supported.
 * \retval    E_FAIL   Combination of trMode and pinmode is not supported.
 *
 * \note      Please refer the description about IS,DPE1,DPE0 and TRM bits for
 *            proper configuration of SPIDAT[1:0]. Slave mode support is only
 *            present for channel 0.
 */
int32_t MCSPIModeConfig(uint32_t baseAddr,
                        uint32_t chNum,
                        uint32_t spiMode,
                        uint32_t chMode,
                        uint32_t trMode,
                        uint32_t pinMode);

/**
 * \brief     Enable/Disable the channel of MCSPI controller.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     enableCh       Enable/Disable MCSPI channel
 *            enableCh will take the following values.
 *            -# TRUE - Enable channel.
 *            -# FALSE - Disable channel.
 *
 * \note      Please ensure to enable only channel 0 in slave mode.
 *            Channels other than 0 are not valid in slave mode.
 */
void MCSPIChEnable(uint32_t baseAddr,
                   uint32_t chNum,
                   uint32_t enableCh);

/**
 * \brief     Reset the MCSPI controller.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 */
void MCSPIReset(uint32_t baseAddr);

/**
 * \brief     Enable/Disable TURBO mode of operation.
 *
 * \param     baseAddr        Memory Address of the MCSPI instance used.
 * \param     chNum           Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     enableTurboMode Enable/Disable Turbo mode of operation.
 *            enableTurboMode will take the following values.
 *            -# TRUE - Enable turbo mode.
 *            -# FALSE - Disable turbo mode.
 */
void MCSPITurboModeEnable(uint32_t baseAddr,
                          uint32_t chNum,
                          uint32_t enableTurboMode);

/**
 * \brief     Enable/disable the Tx FIFO.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     enableTxFifo   Enable/disable Tx FIFO.
 *            enableTxFifo will take the following values.
 *            -# TRUE - Enable Tx Fifo.
 *            -# FALSE - Disable Tx Fifo.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 *
 * \note      Enabling FIFO is restricted to only 1 channel.
 */
void MCSPITxFifoEnable(uint32_t baseAddr,
                       uint32_t chNum,
                       uint32_t enableTxFifo);

/**
 * \brief     Enable/disable the Rx FIFO.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     enableRxFifo   Enable/disable Rx FIFO.
 *            enableRxFifo will take the following values.
 *            -# TRUE - Enable Rx Fifo.
 *            -# FALSE - Disable Rx Fifo.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 *
 * \note      Enabling FIFO is restricted to only 1 channel.
 */
void MCSPIRxFifoEnable(uint32_t baseAddr,
                       uint32_t chNum,
                       uint32_t enableRxFifo);

/**
 * \brief     Set the transfer levels used by FIFO depending on the
 *            various MCSPI transmit/receive modes.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     bufFullVal     Buffer almost full value.
 * \param     bufEmptyVal    Buffer almost empty value.
 * \param     trMode         Transmit/Receive modes.
 *            trMode will contain values from the following enum.
 *            #mcspiTransferMode_t.
 *
 * \note      Few points to be noted.
 *            -# Values for bufFullVal and bufEmptyVal will have varying values
 *            depending on trMode. If trMode is MCSPI_TX_RX_MODE then
 *            bufFullVal and bufEmptyVal can take values ranging from 0-19.
 *            If trMode is MCSPI_RX_ONLY_MODE/MCSPI_TX_ONLY_MODE then bufFullVal
 *            and bufEmptyVal can take values from 0-39.
 *
 *            -# While configuring mode of operation using trMode please ensure
 *            the same value of trMode is used while using API's
 *            MCSPIModeConfig. Mismatch while using trMode for different APIs
 *            can result in unpredictable behaviour.
 *
 *            -# For 'bufEmptyVal' and 'bufFullVal' please send level values
 *            for both of the fields and do not send the value which has to
 *            be written into the register for the corresponding level value.
 *            For e.g. if 'bufEmptyVal' or 'bufFullVal' has to be 1 byte,
 *            then pass the parameter 'bufEmptyVal' or 'bufFullVal' as
 *            1 and not 0 because the value which has to be written into
 *            register is manipulated inside the driver itself.
**/
void MCSPISetFifoTriggerLvl(uint32_t baseAddr,
                            uint8_t bufFullVal,
                            uint8_t bufEmptyVal,
                            uint32_t trMode);

/**
 * \brief     Configure the SPI word count.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     wordCnt        Word count value.
 */
void MCSPISetWordCount(uint32_t baseAddr, uint16_t wordCnt);

/**
 * \brief     Enable/Disable the MCSPI DMA events.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     dmaFlags       Represents the flags need to enable DMA
 *                           events.
 *            dmaFlags will contain values from the follwing enum..
 *            #mcspiDmaEvent_t
 * \param     enableDma      Enable/Disable the DMA.
 *            enableDma will take the following values.
 *            -# TRUE - Enable DMA.
 *            -# FALSE - Disable DMA.
 */
void MCSPIDmaEnable(uint32_t baseAddr,
                    uint32_t chNum,
                    uint32_t dmaFlags,
                    uint32_t enableDma);

/**
 * \brief     Enable the MCSPI Interrupts.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     intrFlags      Represents the various interrupts to be enabled.
 *            intrFlags can take the following values.
 *            -# MCSPI_INTR_TX_EMPTY(ch) - Transmitter register empty
 *                                          interrupt.
 *            -# MCSPI_INTR_TX_UNDERFLOW(ch) - Transmitter register underflow
 *                                              interrupt.
 *            -# MCSPI_INTR_RX_FULL(ch) - Receiver register full interrupt.
 *            -# MCSPI_INTR_RX0_OVERFLOW - Receiver register 0 overflow
 *                                        interrupt.
 *            -# MCSPI_INTR_EOW - End of word count interrupt.
 *            -where 'ch' will contain values from the following enum.
 *            #mcspiChNum_t
 */
void MCSPIIntrEnable(uint32_t baseAddr, uint32_t intrFlags);

/**
 * \brief     Disable the MCSPI Interrupts.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     intrFlags      Represents the various interrupts to be enabled.
 *            intrFlags can take the following values.
 *            -# MCSPI_INTR_TX_EMPTY(ch) - Transmitter register empty
 *                                          interrupt.
 *            -# MCSPI_INTR_TX_UNDERFLOW(ch) - Transmitter register underflow
 *                                              interrupt.
 *            -# MCSPI_INTR_RX_FULL(ch) - Receiver register full interrupt.
 *            -# MCSPI_INTR_RX0_OVERFLOW - Receiver register 0 overflow
 *                                        interrupt.
 *            -# MCSPI_INTR_EOW - End of word count interrupt.
 *            -where 'ch' will contain values from the following enum.
 *            #mcspiChNum_t
 */
void MCSPIIntrDisable(uint32_t baseAddr, uint32_t intrFlags);

/**
 * \brief     Configure delay for the first transfer from MCSPI peripheral.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     initialDelay   Initial delay value used.
 *            initialDelay will contain values from the following enum.
 *            #mcspiInitDelayCfg_t.
 *
 * \note      Please note that this option is available only in single master
 *            mode.
 */
void MCSPISetInitialDelay(uint32_t baseAddr, uint32_t initialDelay);

/**
 * \brief     Configure data to be transmitted.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 * \param     txData         Data to be transmitted.
 */
void MCSPITransmitData(uint32_t baseAddr, uint32_t chNum, uint32_t txData);

/**
 * \brief     Read received data.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 *
 * \retval    rxData         Returns the received data.
 */
uint32_t MCSPIReceiveData(uint32_t baseAddr, uint32_t chNum);

/**
 * \brief     Return the status of the MCSPI interrupts.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 *
 * \retval    #MCSPI_INTR_TX_EMPTY(ch)  Transmitter register empty interrupt.
 * \retval    #MCSPI_INTR_TX_UNDERFLOW(ch)  Transmitter register underflow
 *                                            interrupt.
 * \retval    #MCSPI_INTR_RX_FULL(ch)  Receiver register full interrupt.
 * \retval    #MCSPI_INTR_RX0_OVERFLOW  Receiver register 0 overflow interrupt.
 * \retval    #MCSPI_INTR_EOW  End of word count interrupt.
 *            where 'ch' will contain values from the following enum.
 *            #mcspiChNum_t
 */
uint32_t MCSPIIntrStatus(uint32_t baseAddr);

/**
 * \brief     Clear the MCSPI Interrupt status.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     intrFlags      Represents the various interrupt status to be
 *                           cleared.
 *            intrFlags can take the following values.
 *            -# MCSPI_INTR_TX_EMPTY(ch) - Transmitter register empty
 *                                          interrupt.
 *            -# MCSPI_INTR_TX_UNDERFLOW(ch) - Transmitter register underflow
 *                                              interrupt.
 *            -# MCSPI_INTR_RX_FULL(ch) - Receiver register full interrupt.
 *            -# MCSPI_INTR_RX0_OVERFLOW - Receiver register 0 overflow
 *                                        interrupt.
 *            -# MCSPI_INTR_EOW - End of word count interrupt.
 *            -where 'ch' will contain values from the following enum.
 *            #mcspiChNum_t
 *
 * \note      Please ensure the proper channel number is passed while using the
 *            macros.
 */
void MCSPIIntrClear(uint32_t baseAddr, uint32_t intrFlags);

/**
 * \brief     Read the MCSPI channel status.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     chNum          Channel number of the MCSPI instance used.
 *            chNum will contain values from the following enum.
 *            #mcspiChNum_t
 *
 * \retval    chStatus       Returns the MCSPI channel status.
 *            User can use the values present in the enum mentioned below to
 *            check the status
 *            #mcspiChStatus_t
 */
uint32_t MCSPIChStatus(uint32_t baseAddr, uint32_t chNum);

/**
 * \brief     Enable/disable the multi word OCP access feature of MCSPI.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     enableMoa      Enable/disable multi word OCP access.
 *            enableMoa will take the following values.
 *            -# TRUE - Enable moa.
 *            -# FALSE - Disable moa.
 */
void MCSPIMultipleWordAccessEnable(uint32_t baseAddr, uint32_t enableMoa);

/**
 * \brief     Enable/Disable the FIFO data management feature.
 *
 * \details   This feature needs to be used if MCSPI FIFOs are enabled
 *            and a DMA which is aligned to 256 bit is used.
 *
 * \param     baseAddr       Memory Address of the MCSPI instance used.
 * \param     enableFifoData Manage the FIFO data by passing corresponding
 *                           value.
 *            enableFifoData will take the following values.
 *            -# TRUE - Enable access to shadow registers.
 *            -# FALSE - Disable access to shadow registers.
 */
void MCSPIFifoDataMgmtEnable(uint32_t baseAddr, uint32_t enableFifoData);

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef MCSPI_H_ */
