/*
 *  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 *
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

/**
 * \file   uart.c
 *
 * \brief  This file contains the Device Abstraction Layer(DAL) APIs
 *         for UART module. These APIs are used for configuration
 *         of instance, transmission and reception of data.
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <ti/am335x/hw_types.h>
#include <ti/am335x/uart.h>

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
uint32_t UARTOperatingModeSelect(uint32_t baseAddr,
                                 uint32_t modeFlag)
{
    uint32_t operMode = 0;

    operMode = HW_RD_REG32(baseAddr + UART_MDR1) & UART_MDR1_MODE_SELECT_MASK;

    /* Programming the MODESELECT field in MDR1. */
    HW_WR_FIELD32(baseAddr + UART_MDR1, UART_MDR1_MODE_SELECT,
                  modeFlag >> UART_MDR1_MODE_SELECT_SHIFT);

    return operMode;
}

static inline uint32_t UARTDivideRoundCloset(uint32_t divident,
                                             uint32_t divisor)
{
    return ((divident + (divisor / 2U)) / divisor);
}

uint32_t UARTDivisorValCompute(uint32_t moduleClk,
                               uint32_t baudRate,
                               uint32_t modeFlag,
                               uint32_t mirOverSampRate)
{
    uint32_t divisorValue = 0;
    uint32_t tempModeFlag = modeFlag & UART_MDR1_MODE_SELECT_MASK;

    switch (tempModeFlag)
    {
    case UART16x_OPER_MODE:
    case UART_SIR_OPER_MODE:
        divisorValue = UARTDivideRoundCloset(moduleClk, 16U * baudRate);
        break;

    case UART13x_OPER_MODE:
        divisorValue = UARTDivideRoundCloset(moduleClk, 13U * baudRate);
        break;

    case UART_MIR_OPER_MODE:
        divisorValue = UARTDivideRoundCloset(moduleClk, mirOverSampRate * baudRate);
        break;

    case UART_FIR_OPER_MODE:
        divisorValue = 0;
        break;

    default:
        break;
    }

    return divisorValue;
}

uint32_t UARTDivisorLatchWrite(uint32_t baseAddr,
                               uint32_t divisorValue)
{
    volatile uint32_t enhanFnBitVal = 0;
    volatile uint32_t sleepMdBitVal = 0;
    volatile uint32_t lcrRegValue = 0;
    volatile uint32_t operMode = 0;
    uint32_t divRegVal = 0;

    /* Switching to Register Configuration Mode B. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Collecting the current value of EFR[4] and later setting it. */
    enhanFnBitVal = HW_RD_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN);
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN,
                  UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_1);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Register Operational Mode. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /*
    ** Collecting the current value of IER[4](SLEEPMODE bit) and later
    ** clearing it.
    */
    sleepMdBitVal = HW_RD_FIELD32(baseAddr + UART_IER, UART_IER_SLEEP_MODE);

    HW_WR_FIELD32(baseAddr + UART_IER, UART_IER_SLEEP_MODE, 0);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Register Configuration Mode B. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Collecting the current value of Divisor Latch Registers. */
    divRegVal = HW_RD_REG32(baseAddr + UART_DLL) & 0xFFU;
    divRegVal |= (HW_RD_REG32(baseAddr + UART_DLH) & 0x3FU) << 8;

    /* Switch the UART instance to Disabled state. */
    operMode = UARTOperatingModeSelect(baseAddr,
                                       (uint32_t)UART_MDR1_MODE_SELECT_MASK);

    /* Writing to Divisor Latch Low(DLL) register. */
    HW_WR_REG32(baseAddr + UART_DLL, divisorValue & 0x00FFU);

    /* Writing to Divisor Latch High(DLH) register. */
    HW_WR_REG32(baseAddr + UART_DLH, (divisorValue & 0x3F00U) >> 8);

    /* Restoring the Operating Mode of UART. */
    (void)UARTOperatingModeSelect(baseAddr, operMode);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Register Operational Mode. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /* Restoring the value of IER[4] to its original value. */
    HW_WR_FIELD32(baseAddr + UART_IER, UART_IER_SLEEP_MODE, sleepMdBitVal);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Register Configuration Mode B. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Restoring the value of EFR[4] to its original value. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN, enhanFnBitVal);

    /* Restoring the value of LCR Register. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return divRegVal;
}

void UARTDivisorLatchEnable(uint32_t baseAddr)
{
    /* Enable access to Divisor Latch registers by setting LCR[7] bit. */
    HW_WR_FIELD32(baseAddr + UART_LCR, UART_LCR_DIV_EN,
                  UART_LCR_DIV_EN_DIV_EN_VALUE_1);
}

void UARTDivisorLatchDisable(uint32_t baseAddr)
{
    /* Disabling access to Divisor Latch registers by clearing LCR[7] bit. */
    HW_WR_FIELD32(baseAddr + UART_LCR, UART_LCR_DIV_EN,
                  UART_LCR_DIV_EN_DIV_EN_VALUE_0);
}

uint32_t UARTRegConfigModeEnable(uint32_t baseAddr, uint32_t modeFlag)
{
    uint32_t lcrRegValue = 0;

    /* Preserving the current value of LCR. */
    lcrRegValue = HW_RD_REG32(baseAddr + UART_LCR);

    switch (modeFlag)
    {
    case UART_REG_CONFIG_MODE_A:
    case UART_REG_CONFIG_MODE_B:
        HW_WR_REG32(baseAddr + UART_LCR, modeFlag & 0xFFU);
        break;

    case UART_REG_OPERATIONAL_MODE:
        HW_WR_REG32(baseAddr + UART_LCR, HW_RD_REG32(baseAddr + UART_LCR) & 0x7FU);
        break;

    default:
        break;
    }

    return lcrRegValue;
}

void UARTRegConfModeRestore(uint32_t baseAddr, uint32_t lcrRegValue)
{
    /* Programming the Line Control Register(LCR). */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

void UARTBreakCtl(uint32_t baseAddr, uint32_t breakState)
{
    /* Programming the BREAK_EN bit in LCR. */
    HW_WR_FIELD32(baseAddr + UART_LCR, UART_LCR_BREAK_EN,
                  breakState >> UART_LCR_BREAK_EN_SHIFT);
}

void UARTLineCharacConfig(uint32_t baseAddr,
                          uint32_t wLenStbFlag,
                          uint32_t parityFlag)
{
    uint32_t lcrRegValue;

    lcrRegValue = HW_RD_REG32(baseAddr + UART_LCR);
    /* Clearing the CHAR_LENGTH and NB_STOP fields in LCR.*/
    lcrRegValue &= ~((uint32_t)UART_LCR_NB_STOP_MASK | (uint32_t)UART_LCR_CHAR_LENGTH_MASK);

    /* Programming the CHAR_LENGTH and NB_STOP fields in LCR. */
    lcrRegValue |= wLenStbFlag & (UART_LCR_NB_STOP_MASK |
                                  UART_LCR_CHAR_LENGTH_MASK);

    /* Clearing the PARITY_EN, PARITY_TYPE1 and PARITY_TYPE2 fields in LCR. */
    lcrRegValue &= ~((uint32_t)UART_LCR_PARITY_TYPE2_MASK |
                     (uint32_t)UART_LCR_PARITY_TYPE1_MASK |
                     (uint32_t)UART_LCR_PARITY_EN_MASK);

    /* Programming the PARITY_EN, PARITY_TYPE1 and PARITY_TYPE2 fields in LCR.*/
    lcrRegValue |= parityFlag & (UART_LCR_PARITY_TYPE2_MASK |
                                 UART_LCR_PARITY_TYPE1_MASK |
                                 UART_LCR_PARITY_EN_MASK);
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

void UARTParityModeSet(uint32_t baseAddr, uint32_t parityFlag)
{
    uint32_t lcrRegValue;

    lcrRegValue = HW_RD_REG32(baseAddr + UART_LCR);
    /* Clearing the PARITY_EN, PARITY_TYPE1 and PARITY_TYPE2 fields in LCR. */
    lcrRegValue &= ~((uint32_t)UART_LCR_PARITY_TYPE2_MASK |
                     (uint32_t)UART_LCR_PARITY_TYPE1_MASK |
                     (uint32_t)UART_LCR_PARITY_EN_MASK);

    /* Programming the PARITY_EN, PARITY_TYPE1 and PARITY_TYPE2 fields in LCR.*/
    lcrRegValue |= parityFlag & (UART_LCR_PARITY_TYPE2_MASK |
                                 UART_LCR_PARITY_TYPE1_MASK |
                                 UART_LCR_PARITY_EN_MASK);

    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

uint32_t UARTParityModeGet(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + UART_LCR) & (UART_LCR_PARITY_TYPE2_MASK |
                                                UART_LCR_PARITY_TYPE1_MASK |
                                                UART_LCR_PARITY_EN_MASK));
}

uint32_t UARTFIFOConfig(uint32_t baseAddr,
                        uint32_t fifoConfig)
{
    uint32_t txGra = (fifoConfig & UART_FIFO_CONFIG_TXGRA) >> 26;
    uint32_t rxGra = (fifoConfig & UART_FIFO_CONFIG_RXGRA) >> 22;

    uint32_t txTrig = (fifoConfig & UART_FIFO_CONFIG_TXTRIG) >> 14;
    uint32_t rxTrig = (fifoConfig & UART_FIFO_CONFIG_RXTRIG) >> 6;

    uint32_t txClr = (fifoConfig & UART_FIFO_CONFIG_TXCLR) >> 5;
    uint32_t rxClr = (fifoConfig & UART_FIFO_CONFIG_RXCLR) >> 4;

    uint32_t dmaEnPath = (fifoConfig & UART_FIFO_CONFIG_DMAENPATH) >> 3;
    uint32_t dmaMode = (fifoConfig & UART_FIFO_CONFIG_DMAMODE);

    uint32_t enhanFnBitVal = 0;
    uint32_t tcrTlrBitVal = 0;
    uint32_t tlrValue = 0;
    uint32_t fcrValue = 0;

    /* Setting the EFR[4] bit to 1. */
    enhanFnBitVal = UARTEnhanFuncEnable(baseAddr);

    tcrTlrBitVal = UARTSubConfigTCRTLRModeEn(baseAddr);

    /* Enabling FIFO mode of operation. */
    fcrValue |= UART_FCR_FIFO_EN_MASK;

    /* Setting the Receiver FIFO trigger level. */
    if (UART_TRIG_LVL_GRANULARITY_1 != rxGra)
    {
        /* Clearing the RXTRIGGRANU1 bit in SCR. */
        HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_RX_TRIG_GRANU1,
                      UART_SCR_RX_TRIG_GRANU1_RX_TRIG_GRANU1_VALUE_0);

        /* Clearing the RX_FIFO_TRIG_DMA field of TLR register. */
        HW_WR_FIELD32(baseAddr + UART_TLR, UART_TLR_RX_FIFO_TRIG_DMA,
                      0);

        fcrValue &= ~((uint32_t)UART_FCR_RX_FIFO_TRIG_MASK);

        /*
        ** Checking if 'rxTrig' matches with the RX Trigger level values
        ** in FCR.
        */
        if ((UART_FCR_RX_TRIG_LVL_8 == rxTrig) ||
            (UART_FCR_RX_TRIG_LVL_16 == rxTrig) ||
            (UART_FCR_RX_TRIG_LVL_56 == rxTrig) ||
            (UART_FCR_RX_TRIG_LVL_60 == rxTrig))
        {
            fcrValue |= rxTrig & UART_FCR_RX_FIFO_TRIG_MASK;
        }
        else
        {
            /* RX Trigger level will be a multiple of 4. */
            /* Programming the RX_FIFO_TRIG_DMA field of TLR register. */
            HW_WR_FIELD32(baseAddr + UART_TLR, UART_TLR_RX_FIFO_TRIG_DMA,
                          rxTrig);
        }
    }
    else
    {
        /* 'rxTrig' now has the 6-bit RX Trigger level value. */

        rxTrig &= 0x003FU;

        /* Collecting the bits rxTrig[5:2]. */
        tlrValue = (rxTrig & 0x003CU) >> 2;

        /* Collecting the bits rxTrig[1:0] and writing to 'fcrValue'. */
        fcrValue |= (rxTrig & 0x0003U) << UART_FCR_RX_FIFO_TRIG_SHIFT;

        /* Setting the RXTRIGGRANU1 bit of SCR register. */
        HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_RX_TRIG_GRANU1,
                      UART_SCR_RX_TRIG_GRANU1_RX_TRIG_GRANU1_VALUE_1);

        /* Programming the RX_FIFO_TRIG_DMA field of TLR register. */
        HW_WR_FIELD32(baseAddr + UART_TLR, UART_TLR_RX_FIFO_TRIG_DMA, tlrValue);
    }

    /* Setting the Transmitter FIFO trigger level. */
    if (UART_TRIG_LVL_GRANULARITY_1 != txGra)
    {
        /* Clearing the TXTRIGGRANU1 bit in SCR. */
        HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_TX_TRIG_GRANU1,
                      UART_SCR_TX_TRIG_GRANU1_TX_TRIG_GRANU1_VALUE_0);

        /* Clearing the TX_FIFO_TRIG_DMA field of TLR register. */
        HW_WR_FIELD32(baseAddr + UART_TLR, UART_TLR_TX_FIFO_TRIG_DMA,
                      0);

        fcrValue &= ~((uint32_t)UART_FCR_TX_FIFO_TRIG_MASK);

        /*
        ** Checking if 'txTrig' matches with the TX Trigger level values
        ** in FCR.
        */
        if ((UART_FCR_TX_TRIG_LVL_8 == (txTrig)) ||
            (UART_FCR_TX_TRIG_LVL_16 == (txTrig)) ||
            (UART_FCR_TX_TRIG_LVL_32 == (txTrig)) ||
            (UART_FCR_TX_TRIG_LVL_56 == (txTrig)))
        {
            fcrValue |= txTrig & UART_FCR_TX_FIFO_TRIG_MASK;
        }
        else
        {
            /* TX Trigger level will be a multiple of 4. */
            /* Programming the TX_FIFO_TRIG_DMA field of TLR register. */
            HW_WR_FIELD32(baseAddr + UART_TLR, UART_TLR_TX_FIFO_TRIG_DMA,
                          txTrig);
        }
    }
    else
    {
        /* 'txTrig' now has the 6-bit TX Trigger level value. */

        txTrig &= 0x003FU;

        /* Collecting the bits txTrig[5:2]. */
        tlrValue = (txTrig & 0x003CU) >> 2;

        /* Collecting the bits txTrig[1:0] and writing to 'fcrValue'. */
        fcrValue |= (txTrig & 0x0003U) << UART_FCR_TX_FIFO_TRIG_SHIFT;

        /* Setting the TXTRIGGRANU1 bit of SCR register. */
        HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_TX_TRIG_GRANU1,
                      UART_SCR_TX_TRIG_GRANU1_TX_TRIG_GRANU1_VALUE_1);

        /* Programming the TX_FIFO_TRIG_DMA field of TLR register. */
        HW_WR_FIELD32(baseAddr + UART_TLR, UART_TLR_TX_FIFO_TRIG_DMA, tlrValue);
    }

    if (UART_DMA_EN_PATH_FCR == dmaEnPath)
    {
        /* Configuring the UART DMA Mode through FCR register. */
        HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_DMA_MODE_CTL,
                      UART_SCR_DMA_MODE_CTL_DMA_MODE_CTL_VALUE_0);

        dmaMode &= 0x1U;

        /* Clearing the bit corresponding to the DMA_MODE in 'fcrValue'. */
        fcrValue &= ~((uint32_t)UART_FCR_DMA_MODE_MASK);

        /* Setting the DMA Mode of operation. */
        fcrValue |= dmaMode << UART_FCR_DMA_MODE_SHIFT;
    }
    else
    {
        dmaMode &= 0x3U;

        /* Configuring the UART DMA Mode through SCR register. */
        HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_DMA_MODE_CTL,
                      UART_SCR_DMA_MODE_CTL_DMA_MODE_CTL_VALUE_1);

        /* Programming the DMAMODE2 field in SCR. */
        HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_DMA_MODE_2, dmaMode);
    }

    /* Programming the bits which clear the RX and TX FIFOs. */
    fcrValue |= rxClr << UART_FCR_RX_FIFO_CLEAR_SHIFT;
    fcrValue |= txClr << UART_FCR_TX_FIFO_CLEAR_SHIFT;

    /* Writing 'fcrValue' to the FIFO Control Register(FCR). */
    UARTFIFORegisterWrite(baseAddr, fcrValue);

    /* Restoring the value of TCRTLR bit in MCR. */
    UARTTCRTLRBitValRestore(baseAddr, tcrTlrBitVal);

    /* Restoring the value of EFR[4] to the original value. */
    UARTEnhanFuncBitValRestore(baseAddr, enhanFnBitVal);

    return fcrValue;
}

void UARTDMAEnable(uint32_t baseAddr, uint32_t dmaModeFlag)
{
    /* Setting the DMAMODECTL bit in SCR to 1. */
    HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_DMA_MODE_CTL,
                  UART_SCR_DMA_MODE_CTL_DMA_MODE_CTL_VALUE_1);

    /* Programming the DMAMODE2 field in SCR. */
    HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_DMA_MODE_2, dmaModeFlag);
}

void UARTDMADisable(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_DMA_MODE_CTL,
                  UART_SCR_DMA_MODE_CTL_DMA_MODE_CTL_VALUE_1);

    /* Programming DMAMODE2 field of SCR to DMA mode 0. */
    HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_DMA_MODE_2,
                  UART_SCR_DMA_MODE_2_DMA_MODE_2_VALUE_0);
}

void UARTFIFORegisterWrite(uint32_t baseAddr, uint32_t fcrValue)
{
    uint32_t divLatchRegVal = 0;
    uint32_t enhanFnBitVal = 0;
    uint32_t lcrRegValue = 0;

    /* Switching to Register Configuration Mode A of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_A);

    /* Clearing the contents of Divisor Latch Registers. */
    divLatchRegVal = UARTDivisorLatchWrite(baseAddr, 0x0000);

    /* Set the EFR[4] bit to 1. */
    enhanFnBitVal = UARTEnhanFuncEnable(baseAddr);

    /* Writing the 'fcrValue' to the FCR register. */
    HW_WR_REG32(baseAddr + UART_FCR, fcrValue);

    /* Restoring the value of EFR[4] to its original value. */
    UARTEnhanFuncBitValRestore(baseAddr, enhanFnBitVal);

    /* Programming the Divisor Latch Registers with the collected value. */
    (void)UARTDivisorLatchWrite(baseAddr, divLatchRegVal);

    /* Reinstating LCR with its original value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

void UARTFIFOCtrlRegWrite(uint32_t baseAddr, uint32_t fcrValue)
{
    uint32_t enhanFnBitVal = 0;
    uint32_t lcrRegValue = 0;

    /* Switching to Register Configuration Mode A of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_A);

    /* Set the EFR[4] bit to 1. */
    enhanFnBitVal = UARTEnhanFuncEnable(baseAddr);

    /* Writing the 'fcrValue' to the FCR register. */
    HW_WR_REG32(baseAddr + UART_FCR, fcrValue);

    /* Restoring the value of EFR[4] to its original value. */
    UARTEnhanFuncBitValRestore(baseAddr, enhanFnBitVal);

    /* Reinstating LCR with its original value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

uint32_t UARTEnhanFuncEnable(uint32_t baseAddr)
{
    uint32_t enhanFnBitVal = 0;
    uint32_t lcrRegValue = 0;

    /* Enabling Configuration Mode B of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Collecting the current value of ENHANCEDEN bit of EFR. */
    enhanFnBitVal = HW_RD_REG32(baseAddr + UART_EFR) & UART_EFR_ENHANCED_EN_MASK;

    /* Setting the ENHANCEDEN bit in EFR register. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN,
                  UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_1);

    /* Programming LCR with the collected value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return enhanFnBitVal;
}

void UARTEnhanFuncBitValRestore(uint32_t baseAddr,
                                uint32_t enhanFnBitVal)
{
    uint32_t lcrRegValue = 0;

    /* Enabling Configuration Mode B of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Restoring the value of EFR[4]. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN,
                  enhanFnBitVal >> UART_EFR_ENHANCED_EN_SHIFT);

    /* Programming LCR with the collected value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

uint32_t UARTSubConfigMSRSPRModeEn(uint32_t baseAddr)
{
    uint32_t enhanFnBitVal = 0;
    uint32_t tcrTlrValue = 0;
    uint32_t lcrRegValue = 0;

    /* Switching to Configuration Mode B of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Collecting the current value of EFR[4] and later setting it. */
    enhanFnBitVal = HW_RD_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN);

    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN,
                  UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_1);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Configuration Mode A of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_A);

    /* Collecting the bit value of MCR[6]. */
    tcrTlrValue = HW_RD_REG32(baseAddr + UART_MCR) & UART_MCR_TCR_TLR_MASK;

    /* Clearing the TCRTLR bit in MCR register. */
    HW_WR_FIELD32(baseAddr + UART_MCR, UART_MCR_TCR_TLR,
                  UART_MCR_TCR_TLR_TCR_TLR_VALUE_0);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Configuration Mode B of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Restoring the value of EFR[4]. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN, enhanFnBitVal);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return tcrTlrValue;
}

uint32_t UARTSubConfigTCRTLRModeEn(uint32_t baseAddr)
{
    uint32_t enhanFnBitVal = 0;
    uint32_t tcrTlrValue = 0;
    uint32_t lcrRegValue = 0;

    /* Switching to Register Configuration Mode B. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Collecting the current value of EFR[4] and later setting it. */
    enhanFnBitVal = HW_RD_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN);

    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN,
                  UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_1);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Register Configuration Mode A. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_A);

    /* Collecting the bit value of MCR[6]. */
    tcrTlrValue = HW_RD_REG32(baseAddr + UART_MCR) & UART_MCR_TCR_TLR_MASK;

    /* Setting the TCRTLR bit in Modem Control Register(MCR). */
    HW_WR_FIELD32(baseAddr + UART_MCR, UART_MCR_TCR_TLR,
                  UART_MCR_TCR_TLR_TCR_TLR_VALUE_1);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Register Configuration Mode B. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Restoring the value of EFR[4] to its original value. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN, enhanFnBitVal);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return tcrTlrValue;
}

uint32_t UARTSubConfigXOFFModeEn(uint32_t baseAddr)
{
    uint32_t enhanFnBitVal = 0;
    uint32_t tcrTlrValue = 0;
    uint32_t lcrRegValue = 0;

    /* Switching to Register Configuration Mode B. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Collecting the current value of EFR[4] and later setting it. */
    enhanFnBitVal = HW_RD_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN);

    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN,
                  UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_1);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Configuration Mode A of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_A);

    /* Collecting the bit value of TCRTLR(MCR[6]). */
    tcrTlrValue = HW_RD_REG32(baseAddr + UART_MCR) & UART_MCR_TCR_TLR_MASK;

    /* Clearing the TCRTLR bit in MCR register. */
    HW_WR_FIELD32(baseAddr + UART_MCR, UART_MCR_TCR_TLR,
                  UART_MCR_TCR_TLR_TCR_TLR_VALUE_0);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Register Configuration Mode B. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Restoring the value of EFR[4] to its original value. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN, enhanFnBitVal);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return tcrTlrValue;
}

void UARTTCRTLRBitValRestore(uint32_t baseAddr,
                             uint32_t tcrTlrBitVal)
{
    uint32_t enhanFnBitVal = 0;
    uint32_t lcrRegValue = 0;

    /* Switching to Register Configuration Mode B. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Collecting the current value of EFR[4] and later setting it. */
    enhanFnBitVal = HW_RD_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN);

    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN,
                  UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_1);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Configuration Mode A of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_A);

    /* Programming MCR[6] with the corresponding bit value in 'tcrTlrBitVal'. */
    HW_WR_FIELD32(baseAddr + UART_MCR, UART_MCR_TCR_TLR, tcrTlrBitVal);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switching to Register Configuration Mode B. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Restoring the value of EFR[4] to its original value. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN, enhanFnBitVal);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

void UARTIntEnable(uint32_t baseAddr, uint32_t intFlag)
{
    uint32_t enhanFnBitVal = 0;
    uint32_t lcrRegValue = 0;

    /* Switch to mode B only when the upper 4 bits of IER needs to be changed */
    if ((intFlag & 0xF0U) > 0U)
    {
        /* Switching to Register Configuration Mode B. */
        lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

        /* Collecting the current value of EFR[4] and later setting it. */
        enhanFnBitVal = HW_RD_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN);

        HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN,
                      UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_1);

        /* Restoring the value of LCR. */
        HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

        /* Switching to Register Operational Mode of operation. */
        lcrRegValue =
            UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

        /*
        ** It is suggested that the System Interrupts for UART in the
        ** Interrupt Controller are enabled after enabling the peripheral
        ** interrupts of the UART using this API. If done otherwise, there
        ** is a risk of LCR value not getting restored and illicit characters
        ** transmitted or received from/to the UART. The situation is explained
        ** below.
        ** The scene is that the system interrupt for UART is already enabled
        ** and the current API is invoked. On enabling the interrupts
        ** corresponding to IER[7:4] bits below, if any of those interrupt
        ** conditions already existed, there is a possibility that the control
        ** goes to Interrupt Service Routine (ISR) without executing the
        ** remaining statements in this API. Executing the remaining statements
        ** is critical in that the LCR value is restored in them.
        ** However, there seems to be no risk in this API for enabling
        ** interrupts corresponding to IER[3:0] because it is done at the end
        ** and no statements follow that.
        */

        /************* ATOMIC STATEMENTS START *************************/

        /* Programming the bits IER[7:4]. */
        HW_WR_REG32(baseAddr + UART_IER, intFlag & 0xF0U);

        /* Restoring the value of LCR. */
        HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

        /* Switching to Register Configuration Mode B. */
        lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

        /* Restoring the value of EFR[4] to its original value. */
        HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN, enhanFnBitVal);

        /* Restoring the value of LCR. */
        HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

        /************** ATOMIC STATEMENTS END *************************/
    }

    /* Programming the bits IER[3:0]. */
    HW_WR_REG32(baseAddr + UART_IER, HW_RD_REG32(baseAddr + UART_IER) |
                                         (intFlag & 0x0FU));
}

void UARTIntDisable(uint32_t baseAddr, uint32_t intFlag)
{
    uint32_t enhanFnBitVal = 0;
    uint32_t lcrRegValue = 0;

    /* Switch to mode B only when the upper 4 bits of IER needs to be changed */
    if ((intFlag & 0xF0U) > 0U)
    {
        /* Switching to Register Configuration Mode B. */
        lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

        /* Collecting the current value of EFR[4] and later setting it. */
        enhanFnBitVal = HW_RD_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN);

        HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN,
                      UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_1);

        /* Restoring the value of LCR. */
        HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
    }

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    HW_WR_REG32(baseAddr + UART_IER, HW_RD_REG32(baseAddr + UART_IER) &
                                         ~(intFlag & 0xFFU));

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    /* Switch to mode B only when the upper 4 bits of IER needs to be changed */
    if ((intFlag & 0xF0U) > 0U)
    {
        /* Switching to Register Configuration Mode B. */
        lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

        /* Restoring the value of EFR[4] to its original value. */
        HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN, enhanFnBitVal);

        /* Restoring the value of LCR. */
        HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
    }
}

void UARTInt2Enable(uint32_t baseAddr, uint32_t intFlag)
{
    /* Programming the bits IER2[1:0]. */
    HW_WR_REG32(baseAddr + UART_IER2, HW_RD_REG32(baseAddr + UART_IER2) |
                                          (intFlag & 0x03U));
}

void UARTInt2Disable(uint32_t baseAddr, uint32_t intFlag)
{
    HW_WR_REG32(baseAddr + UART_IER2, HW_RD_REG32(baseAddr + UART_IER2) &
                                          ~(intFlag & 0x3U));
}

uint32_t UARTSpaceAvail(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0;
    uint32_t retVal = FALSE;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /*
    ** Checking if either TXFIFOE or TXSRE bits of Line Status Register(LSR)
    ** are set. TXFIFOE bit is set if TX FIFO(or THR in non-FIFO mode) is
    ** empty. TXSRE is set if both the TX FIFO(or THR in non-FIFO mode) and
    ** the transmitter shift register are empty.
    */

    if ((uint32_t)0 != (HW_RD_REG32(baseAddr + UART_LSR) &
                        (UART_LSR_TX_SR_E_MASK | UART_LSR_TX_FIFO_E_MASK)))
    {
        retVal = (uint32_t)TRUE;
    }

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

uint32_t UARTCharsAvail(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0;
    uint32_t retVal = FALSE;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /* Checking if the RHR(or RX FIFO) has atleast one byte to be read. */
    if ((uint32_t)UART_LSR_RX_FIFO_E_RX_FIFO_E_VALUE_0 !=
        (HW_RD_REG32(baseAddr + UART_LSR) &
         UART_LSR_RX_FIFO_E_MASK))
    {
        retVal = (uint32_t)TRUE;
    }

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

uint32_t UARTReadStatus(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0;
    uint32_t retVal = 0;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    retVal = HW_RD_REG32(baseAddr + UART_LSR);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

/**
 * \brief    This API attempts to write a byte into Transmitter Holding
 *           Register (THR). It checks only once if the transmitter is empty.
 *
 * \param    baseAddr     Memory address of the UART instance being used.
 * \param    byteWrite   Byte to be written into the THR register.
 *
 * \return   TRUE if the transmitter FIFO(or THR register in non-FIFO mode)
 *           was empty and the character was written. Else it returns FALSE.
 */

uint32_t UARTCharPutNonBlocking(uint32_t baseAddr,
                                uint8_t byteWrite)
{
    uint32_t lcrRegValue = 0;
    uint32_t retVal = FALSE;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /*
    ** Checking if either THR alone or both THR and Transmitter Shift Register
    ** are empty.
    */
    if ((uint32_t)0 != (HW_RD_REG32(baseAddr + UART_LSR) &
                        (UART_LSR_TX_SR_E_MASK | UART_LSR_TX_FIFO_E_MASK)))
    {
        HW_WR_REG32(baseAddr + UART_THR, (uint32_t)byteWrite);
        retVal = (uint32_t)TRUE;
    }

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

/**
 * \brief    This API reads a byte from the Receiver Buffer Register
 *           (RBR). It checks once if any character is ready to be read.
 *
 * \param    baseAddr     Memory address of the UART instance being used.
 *
 * \return   If the RX FIFO(or RHR) was found to have atleast one byte of
 *           data, then this API reads and returns that byte. Else it
 *           returns -1.
 */

int8_t UARTCharGetNonBlocking(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0;
    int8_t retVal = -((int8_t)1);

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /* Checking if the RX FIFO(or RHR) has atleast one byte of data. */
    if ((uint32_t)UART_LSR_RX_FIFO_E_RX_FIFO_E_VALUE_0 !=
        (HW_RD_REG32(baseAddr + UART_LSR) &
         UART_LSR_RX_FIFO_E_MASK))
    {
        uint32_t tempRetVal = HW_RD_REG32(baseAddr + UART_RHR);
        retVal = ((int8_t)tempRetVal);
    }

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

/**
 * \brief    This API waits indefinitely for the arrival of a byte in
 *           the receiver FIFO. Once a byte has arrived, it returns that
 *           byte.
 *
 * \param    baseAddr     Memory address of the UART instance being used.
 *
 * \return   This returns the read byte.
 */

int8_t UARTCharGet(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0;
    int8_t retVal = 0;
    uint32_t tempRetVal = 0;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /* Waits indefinitely until a byte arrives in the RX FIFO(or RHR). */
    while ((uint32_t)UART_LSR_RX_FIFO_E_RX_FIFO_E_VALUE_0 ==
           (HW_RD_REG32(baseAddr + UART_LSR) &
            UART_LSR_RX_FIFO_E_MASK))
    {
        /* Do nothing - Busy wait */
    }

    tempRetVal = HW_RD_REG32(baseAddr + UART_RHR);
    retVal = ((int8_t)tempRetVal);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

uint8_t UARTCharGetTimeout(uint32_t baseAddr, uint32_t timeOutVal)
{
    uint32_t lcrRegValue = 0;
    uint8_t retVal = (uint8_t)0xFFU;
    uint32_t tempTimeOutVal = timeOutVal;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /*
    ** Wait until either data is not received or the time out value is greater
    ** than zero.
    */
    while (((uint32_t)UART_LSR_RX_FIFO_E_RX_FIFO_E_VALUE_0 ==
            (HW_RD_REG32(baseAddr + UART_LSR) &
             UART_LSR_RX_FIFO_E_MASK)) &&
           (tempTimeOutVal > (uint32_t)0U))
    {
        tempTimeOutVal--;
    }

    if (0U != tempTimeOutVal)
    {
        retVal = (uint8_t)HW_RD_REG32(baseAddr + UART_RHR);
    }

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

void UARTCharPut(uint32_t baseAddr, uint8_t byteTx)
{
    uint32_t lcrRegValue = 0;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /*
    ** Waits indefinitely until the THR and Transmitter Shift Registers are
    ** empty.
    */
    while (((uint32_t)UART_LSR_TX_SR_E_MASK |
            (uint32_t)UART_LSR_TX_FIFO_E_MASK) !=
           (HW_RD_REG32(baseAddr + UART_LSR) &
            ((uint32_t)UART_LSR_TX_SR_E_MASK |
             (uint32_t)UART_LSR_TX_FIFO_E_MASK)))
    {
        /* Do nothing - Busy wait */
    }

    HW_WR_REG32(baseAddr + UART_THR, (uint32_t)byteTx);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

void UARTFIFOCharPut(uint32_t baseAddr, uint8_t byteTx)
{
    /* Write the byte to the Transmit Holding Register(or TX FIFO). */
    HW_WR_REG32(baseAddr + UART_THR, (uint32_t)byteTx);
}

int8_t UARTFIFOCharGet(uint32_t baseAddr)
{
    uint32_t tempRetVal = 0;
    tempRetVal = HW_RD_REG32(baseAddr + UART_RHR);
    return ((int8_t)tempRetVal);
}

uint32_t UARTFIFOWrite(uint32_t baseAddr,
                       const uint8_t *pBuffer,
                       uint32_t numTxBytes)
{
    uint32_t lIndex = 0;
    const uint8_t *pTmpBuffer = pBuffer;

    for (lIndex = 0; lIndex < numTxBytes; lIndex++)
    {
        /* Writing data to the TX FIFO. */
        HW_WR_REG32(baseAddr + UART_THR, (uint32_t)*pTmpBuffer);
        pTmpBuffer++;
    }

    return lIndex;
}

uint32_t UARTRxErrorGet(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0;
    uint32_t retVal = 0;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    retVal = HW_RD_REG32(baseAddr + UART_LSR) &
             (UART_LSR_RX_FIFO_STS_MASK |
              UART_LSR_RX_BI_MASK |
              UART_LSR_RX_FE_MASK |
              UART_LSR_RX_PE_MASK |
              UART_LSR_RX_OE_MASK);

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

uint32_t UARTIntIdentityGet(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0;
    uint32_t retVal = 0;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    retVal = HW_RD_REG32(baseAddr + UART_IIR) & UART_IIR_IT_TYPE_MASK;

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

uint32_t UARTIntPendingStatusGet(uint32_t baseAddr)
{
    uint32_t retVal = UART_N0_INT_PENDING;
    uint32_t lcrRegValue = 0;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /* Checking if an Interrupt is pending. */
    if ((uint32_t)UART_IIR_IT_PENDING_IT_PENDING_VALUE_0 ==
        (HW_RD_REG32(baseAddr + UART_IIR) & UART_IIR_IT_PENDING_MASK))
    {
        retVal = UART_INT_PENDING;
    }

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

uint32_t UARTInt2StatusGet(uint32_t baseAddr)
{
    uint32_t retVal = 0;

    retVal = HW_RD_REG32(baseAddr + UART_ISR2) &
             (UART_IER2_EN_RXFIFO_EMPTY_MASK | UART_IER2_EN_TXFIFO_EMPTY_MASK);

    return retVal;
}

uint32_t UARTFIFOEnableStatusGet(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0U;
    uint32_t retVal = (uint32_t)FALSE;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    if ((uint32_t)0 != (HW_RD_REG32(baseAddr + UART_IIR) &
                        UART_IIR_FCR_MIRROR_MASK))
    {
        retVal = (uint32_t)TRUE;
    }

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

void UARTAutoRTSAutoCTSControl(uint32_t baseAddr,
                               uint32_t autoCtsControl,
                               uint32_t autoRtsControl)
{
    uint32_t lcrRegValue = 0;

    /* Switching to Configuration Mode B of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Programming AUTOCTSEN and AUTORTSEN bits in EFR. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_AUTO_CTS_EN,
                  autoCtsControl >> UART_EFR_AUTO_CTS_EN_SHIFT);

    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_AUTO_RTS_EN,
                  autoRtsControl >> UART_EFR_AUTO_RTS_EN_SHIFT);

    /* Restoring LCR with the collected value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

void UARTSpecialCharDetectControl(uint32_t baseAddr, uint32_t controlFlag)
{
    uint32_t lcrRegValue = 0;

    /* Switch to Configuration Mode B of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Programming the SPECIALCHARDETECT bit in EFR. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_SPECIAL_CHAR_DETECT,
                  controlFlag >> UART_EFR_SPECIAL_CHAR_DETECT_SHIFT);

    /* Restoring LCR with the collected value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

void UARTSoftwareFlowCtrlOptSet(uint32_t baseAddr, uint32_t swFlowCtrl)
{
    uint32_t lcrRegValue = 0;

    /* Switching to Configuration Mode B of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Configuring the SWFLOWCONTROL field in EFR. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_SW_FLOW_CONTROL,
                  swFlowCtrl >> UART_EFR_SW_FLOW_CONTROL_SHIFT);

    /* Restoring LCR with the collected value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

void UARTPulseShapingControl(uint32_t baseAddr, uint32_t shapeControl)
{
    /* Programming the UARTPULSE bit in MDR2. */
    HW_WR_FIELD32(baseAddr + UART_MDR2, UART_MDR2_UART_PULSE,
                  shapeControl >> UART_MDR2_UART_PULSE_MASK);
}

/**
 * \brief  This API performs a module reset of the UART module. It also
 *         waits until the reset process is complete.
 *
 * \param  baseAddr  Memory address of the UART instance being used.
 *
 * \return None.
 *
 * \note   This API accesses the System Configuration Register(SYSC) and
 *         System Status Register(SYSS) to perform module reset and to
 *         wait until the same is complete.
 */

void UARTModuleReset(uint32_t baseAddr)
{
    /* Performing Software Reset of the module. */
    HW_WR_FIELD32(baseAddr + UART_SYSC, UART_SYSC_SOFTRESET,
                  UART_SYSC_SOFTRESET_SOFTRESET_VALUE_1);

    /* Wait until the process of Module Reset is complete. */
    while (0U == HW_RD_FIELD32(baseAddr + UART_SYSS, UART_SYSS_RESETDONE))
    {
        /* Do nothing - Busy wait */
    }
}

void UARTIdleModeConfigure(uint32_t baseAddr, uint32_t modeFlag)
{
    /* Programming the IDLEMODE field in SYSC. */
    HW_WR_FIELD32(baseAddr + UART_SYSC, UART_SYSC_IDLEMODE, modeFlag >> UART_SYSC_IDLEMODE_SHIFT);
}

void UARTWakeUpControl(uint32_t baseAddr, uint32_t controlFlag)
{
    /* Programming the ENAWAKEUP feature in SYSC register. */
    HW_WR_FIELD32(baseAddr + UART_SYSC, UART_SYSC_ENAWAKEUP, controlFlag >> UART_SYSC_ENAWAKEUP_SHIFT);
}

void UARTAutoIdleModeControl(uint32_t baseAddr, uint32_t modeFlag)
{
    /* Programming the AUTOIDLE bit in SYSC register. */
    HW_WR_FIELD32(baseAddr + UART_SYSC, UART_SYSC_AUTOIDLE, modeFlag >> UART_SYSC_AUTOIDLE_SHIFT);
}

void UARTFlowCtrlTrigLvlConfig(uint32_t baseAddr,
                               uint32_t rtsHaltFlag,
                               uint32_t rtsStartFlag)
{
    uint32_t tcrValue = 0;

    tcrValue = rtsHaltFlag & UART_TCR_RX_FIFO_TRIG_HALT_MASK;

    tcrValue |= (rtsStartFlag << UART_TCR_RX_FIFO_TRIG_START_SHIFT) &
                UART_TCR_RX_FIFO_TRIG_START_MASK;

    /* Writing to TCR register. */
    HW_WR_REG32(baseAddr + UART_TCR, tcrValue);
}

void UARTXON1XOFF1ValProgram(uint32_t baseAddr,
                             uint8_t xon1Value,
                             uint8_t xoff1Value)
{
    /* Programming the XON1 register. */
    HW_WR_REG32(baseAddr + UART_XON1_ADDR1, (uint32_t)xon1Value);

    /* Programming the XOFF1 register. */
    HW_WR_REG32(baseAddr + UART_XOFF1, (uint32_t)xoff1Value);
}

void UARTXON2XOFF2ValProgram(uint32_t baseAddr,
                             uint8_t xon2Value,
                             uint8_t xoff2Value)
{
    /* Programming the XON2 register. */
    HW_WR_REG32(baseAddr + UART_XON2_ADDR2, (uint32_t)xon2Value);

    /* Programming the XOFF2 register. */
    HW_WR_REG32(baseAddr + UART_XOFF2, (uint32_t)xoff2Value);
}

void UARTXONAnyFeatureControl(uint32_t baseAddr, uint32_t controlFlag)
{
    uint32_t lcrRegValue = 0;
    uint32_t enhanFnBitVal = 0;

    /* Switching to Register Configuration Mode B. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_B);

    /* Collecting the value of EFR[4] and later setting the same to 1. */
    enhanFnBitVal = HW_RD_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN);

    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN,
                  UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_1);

    /* Programming the XONEN bit in MCR. */
    HW_WR_FIELD32(baseAddr + UART_MCR, UART_MCR_XON_EN, controlFlag >> UART_MCR_XON_EN_SHIFT);

    /* Restoring the value of EFR[4] to its original value. */
    HW_WR_FIELD32(baseAddr + UART_EFR, UART_EFR_ENHANCED_EN, enhanFnBitVal);

    /* Restoring LCR to the original value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);
}

void UARTLoopbackModeControl(uint32_t baseAddr, uint32_t controlFlag)
{
    /* Programming the LOOPBACKEN bit in MCR. */
    HW_WR_FIELD32(baseAddr + UART_MCR, UART_MCR_LOOPBACK_EN, controlFlag >> UART_MCR_LOOPBACK_EN_SHIFT);
}

void UARTModemControlSet(uint32_t baseAddr, uint32_t modeFlag)
{
    /* Programming the specified bits of MCR. */
    HW_WR_REG32(baseAddr + UART_MCR, HW_RD_REG32(baseAddr + UART_MCR) |
                                         (modeFlag & (UART_MCR_CD_STS_CH_MASK |
                                                      UART_MCR_RI_STS_CH_MASK |
                                                      UART_MCR_RTS_MASK |
                                                      UART_MCR_DTR_MASK)));
}

void UARTModemControlClear(uint32_t baseAddr, uint32_t modeFlag)
{
    /* Clearing the specified bits in MCR. */
    HW_WR_REG32(baseAddr + UART_MCR, HW_RD_REG32(baseAddr + UART_MCR) &
                                         ~(modeFlag & (UART_MCR_CD_STS_CH_MASK |
                                                       UART_MCR_RI_STS_CH_MASK |
                                                       UART_MCR_RTS_MASK |
                                                       UART_MCR_DTR_MASK)));
}

void UARTModemControlReset(uint32_t baseAddr)
{
    uint32_t mcrResetVal = 0U;
    /* Resetting bits of MCR. */
    HW_WR_REG32(baseAddr + UART_MCR, mcrResetVal);
}

uint32_t UARTModemStatusGet(uint32_t baseAddr)
{
    /*
    ** Reading MSR[7:4] bits. These values indicate the complement of the
    ** signal levels on the Modem input lines.
    */

    return HW_RD_REG32(baseAddr + UART_MSR) &
           (UART_MSR_NCD_STS_MASK |
            UART_MSR_NRI_STS_MASK |
            UART_MSR_NDSR_STS_MASK |
            UART_MSR_NCTS_STS_MASK);
}

uint32_t UARTModemStatusChangeCheck(uint32_t baseAddr)
{
    /*
    ** Reading MSR[3:0] bits that indicate the change of state of Modem signal
    ** lines.
    */

    return HW_RD_REG32(baseAddr + UART_MSR) &
           (UART_MSR_DCD_STS_MASK |
            UART_MSR_RI_STS_MASK |
            UART_MSR_DSR_STS_MASK |
            UART_MSR_CTS_STS_MASK);
}

void UARTResumeOperation(uint32_t baseAddr)
{
    volatile uint32_t dummyVar = 0;
    /* Dummy read of RESUME register. */
    dummyVar = HW_RD_REG32(baseAddr + UART_RESUME);

    dummyVar = dummyVar;
}

void UARTWakeUpEventsEnable(uint32_t baseAddr, uint32_t wakeUpFlag)
{
    uint32_t werRegValue;

    werRegValue = HW_RD_REG32(baseAddr + UART_WER);
    /* Programming the Wake-Up configuration fields in WER. */
    werRegValue |= wakeUpFlag &
                   (UART_WER_EVENT_7_TX_WAKEUP_EN_MASK |
                    UART_WER_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT_MASK |
                    UART_WER_EVENT_5_RHR_INTERRUPT_MASK |
                    UART_WER_EVENT_4_RX_ACTIVITY_MASK |
                    UART_WER_EVENT_3_DCD_CD_ACTIVITY_MASK |
                    UART_WER_EVENT_2_RI_ACTIVITY_MASK |
                    UART_WER_EVENT_1_DSR_ACTIVITY_MASK |
                    UART_WER_EVENT_0_CTS_ACTIVITY_MASK);
    HW_WR_REG32(baseAddr + UART_WER, werRegValue);
}

void UARTWakeUpEventsDisable(uint32_t baseAddr, uint32_t wakeUpFlag)
{
    uint32_t werRegValue;

    werRegValue = HW_RD_REG32(baseAddr + UART_WER);
    /* Programming the Wake-Up configuration fields in WER. */
    werRegValue &= ~(wakeUpFlag &
                     (UART_WER_EVENT_7_TX_WAKEUP_EN_MASK |
                      UART_WER_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT_MASK |
                      UART_WER_EVENT_5_RHR_INTERRUPT_MASK |
                      UART_WER_EVENT_4_RX_ACTIVITY_MASK |
                      UART_WER_EVENT_3_DCD_CD_ACTIVITY_MASK |
                      UART_WER_EVENT_2_RI_ACTIVITY_MASK |
                      UART_WER_EVENT_1_DSR_ACTIVITY_MASK |
                      UART_WER_EVENT_0_CTS_ACTIVITY_MASK));
    HW_WR_REG32(baseAddr + UART_WER, werRegValue);
}

void UARTFIFOTrigLvlGranControl(uint32_t baseAddr,
                                uint32_t rxFIFOGranCtrl,
                                uint32_t txFIFOGranCtrl)
{
    /* Programming the RXTRIGGRANU1 bit in SCR. */
    HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_RX_TRIG_GRANU1,
                  rxFIFOGranCtrl >>
                      UART_SCR_RX_TRIG_GRANU1_SHIFT);

    /* Programming the TXTRIGGRANU1 bit in SCR. */
    HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_TX_TRIG_GRANU1,
                  txFIFOGranCtrl >>
                      UART_SCR_TX_TRIG_GRANU1_SHIFT);
}

void UARTDSRInterruptControl(uint32_t baseAddr, uint32_t controlFlag)
{
    /* Programming the DSRIT bit in SCR. */
    HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_DSR_IT, controlFlag >> UART_SCR_DSR_IT_SHIFT);
}

void UARTTxEmptyIntControl(uint32_t baseAddr,
                           uint32_t controlFlag)
{
    /* Programming the TXEMPTYCTLIT bit in SCR. */
    HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_TX_EMPTY_CTL_IT, controlFlag >> UART_SCR_TX_EMPTY_CTL_IT_SHIFT);
}

void UARTRXCTSDSRWakeUpConfigure(uint32_t baseAddr, uint32_t wakeUpFlag)
{
    /* Programming the RXCTSDSRWAKEUPENABLE bit in SCR. */
    HW_WR_FIELD32(baseAddr + UART_SCR, UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE,
                  wakeUpFlag >> UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE_SHIFT);
}

uint32_t UARTRXCTSDSRTransitionStatusGet(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + UART_SSR) &
            UART_SSR_RX_CTS_DSR_WAKE_UP_STS_MASK);
}

void UARTDMACounterResetControl(uint32_t baseAddr, uint32_t controlFlag)
{
    /* Programming the DMACOUNTERRST bit in SSR. */
    HW_WR_FIELD32(baseAddr + UART_SSR, UART_SSR_DMA_COUNTER_RST, controlFlag >> UART_SSR_DMA_COUNTER_RST_SHIFT);
}

uint32_t UARTTxFIFOFullStatusGet(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + UART_SSR) & UART_SSR_TX_FIFO_FULL_MASK);
}

uint32_t UARTTxFIFOLevelGet(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + UART_TXFIFO_LVL));
}

uint32_t UARTRxFIFOLevelGet(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + UART_RXFIFO_LVL));
}

uint32_t UARTAutobaudParityGet(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0;
    uint32_t retVal = 0;

    /* Switching to Register Configuration Mode A. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_A);

    retVal = HW_RD_REG32(baseAddr + UART_UASR) & UART_UASR_PARITY_TYPE_MASK;

    /* Restoring the value of LCR to its original value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

/**
 * \brief  This API determines the word length per frame(character length)
 *         being configured by the system in UART Autobauding mode.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 *
 * \return This returns one of the following two values:
 *         - UART_AUTOBAUD_CHAR_LENGTH_7 - indicating word length of 7 bits\n
 *         - UART_AUTOBAUD_CHAR_LENGTH_8 - indicating word length of 8 bits\n
 *
 * \note   UASR register used in this API can be accessed only when the UART
 *         is in Configuration Mode A or Configuration Mode B of operation.
 */

uint32_t UARTAutobaudWordLenGet(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0;
    uint32_t retVal = 0;

    /* Switching to Register Configuration Mode A. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_A);

    retVal = HW_RD_REG32(baseAddr + UART_UASR) & UART_UASR_BIT_BY_CHAR_MASK;

    /* Restoring the value of LCR to its original value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

uint32_t UARTAutobaudSpeedGet(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0;
    uint32_t retVal = 0;

    /* Switching to Register Configuration Mode A. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_CONFIG_MODE_A);

    retVal = HW_RD_REG32(baseAddr + UART_UASR) & UART_UASR_SPEED_MASK;

    /* Restoring the value of LCR to its original value. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

void UARTScratchPadRegWrite(uint32_t baseAddr, uint32_t scratchValue)
{
    /* Programming the SPR_WORD field of SPR. */
    HW_WR_FIELD32(baseAddr + UART_SPR, UART_SPR_SPR_WORD, scratchValue >> UART_SPR_SPR_WORD_SHIFT);
}

uint32_t UARTScratchPadRegRead(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + UART_SPR) & UART_SPR_SPR_WORD_MASK);
}

uint32_t UARTModuleVersionNumberGet(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + UART_MVR) & 0x00FFU);
}

void UARTTxDMAThresholdControl(uint32_t baseAddr, uint32_t thrsCtrlFlag)
{
    /* Programming the SET_TX_DMA_THRESHOLD bit in MDR3 register. */
    HW_WR_FIELD32(baseAddr + UART_MDR3, UART_MDR3_SET_DMA_TX_THRESHOLD,
                  thrsCtrlFlag >> UART_MDR3_SET_DMA_TX_THRESHOLD_SHIFT);
}

void UARTTxDMAThresholdValConfig(uint32_t baseAddr, uint32_t thrsValue)
{
    /* Programming the TX_DMA_THRESHOLD field of TX DMA Threshold register. */
    HW_WR_FIELD32(baseAddr + UART_TX_DMA_THRESHOLD,
                  UART_TX_DMA_THRESHOLD_TX_DMA_THRESHOLD,
                  thrsValue >> UART_TX_DMA_THRESHOLD_TX_DMA_THRESHOLD_SHIFT);
}

void UARTFIFOLevelSet(uint32_t baseAddr, uint32_t rxLevel)
{
    HW_WR_REG32(baseAddr + UART_FCR, (rxLevel & UART_FCR_RX_FIFO_TRIG_MASK) |
                                         UART_FCR_FIFO_EN_MASK);
}

uint32_t UARTModemControlGet(uint32_t baseAddr)
{
    return HW_RD_REG32(baseAddr + UART_MCR) &
           (UART_MCR_XON_EN_MASK | UART_MCR_RTS_MASK);
}

uint32_t UARTIsTransmitterEmpty(uint32_t baseAddr)
{
    uint32_t lcrRegValue = 0, retVal = FALSE;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    if (((uint32_t)UART_LSR_TX_SR_E_MASK |
         (uint32_t)UART_LSR_TX_FIFO_E_MASK) ==
        (HW_RD_REG32(baseAddr + UART_LSR) &
         ((uint32_t)UART_LSR_TX_SR_E_MASK |
          (uint32_t)UART_LSR_TX_FIFO_E_MASK)))
    {
        retVal = (uint32_t)TRUE;
    }
    else
    {
        retVal = (uint32_t)FALSE;
    }

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

uint32_t UARTCharGetNonBlocking2(uint32_t baseAddr, uint8_t *pChar)
{
    uint32_t lcrRegValue = 0;
    uint32_t retVal = FALSE;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /* Checking if the RX FIFO(or RHR) has atleast one byte of data. */
    if ((uint32_t)UART_LSR_RX_FIFO_E_RX_FIFO_E_VALUE_0 !=
        (HW_RD_REG32(baseAddr + UART_LSR) &
         UART_LSR_RX_FIFO_E_MASK))
    {
        uint32_t tempRetVal = HW_RD_REG32(baseAddr + UART_RHR);
        *pChar = (uint8_t)tempRetVal;
        retVal = TRUE;
    }

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}

uint32_t UARTCharGetTimeout2(uint32_t baseAddr, uint32_t timeOutVal, uint8_t *pChar)
{
    uint32_t lcrRegValue = 0;
    int32_t retVal = FALSE;
    uint32_t tempTimeOutVal = timeOutVal;

    /* Switching to Register Operational Mode of operation. */
    lcrRegValue = UARTRegConfigModeEnable(baseAddr, UART_REG_OPERATIONAL_MODE);

    /*
    ** Wait until either data is not received or the time out value is greater
    ** than zero.
    */
    while (((uint32_t)UART_LSR_RX_FIFO_E_RX_FIFO_E_VALUE_0 ==
            (HW_RD_REG32(baseAddr + UART_LSR) &
             UART_LSR_RX_FIFO_E_MASK)) &&
           (tempTimeOutVal > (uint32_t)0U))
    {
        tempTimeOutVal--;
    }

    if (0U != tempTimeOutVal)
    {
        *pChar = (uint8_t)HW_RD_REG32(baseAddr + UART_RHR);
        retVal = TRUE;
    }

    /* Restoring the value of LCR. */
    HW_WR_REG32(baseAddr + UART_LCR, lcrRegValue);

    return retVal;
}
/********************************* End of File ******************************/
