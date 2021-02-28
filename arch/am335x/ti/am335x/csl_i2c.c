/*
 *  Copyright (C) 2013 Texas Instruments Incorporated -www.ti.com\
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
 *  \file   i2c.c
 *
 *  \brief  This file contains the device abstraction layer API implementation
 *          corresponding to I2C module.
 *
 **/
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/**
 * \brief This is to disable HW_SYNC_BARRIER for J7 due to performance
 *        requirement
 */
#if defined (SOC_J721E) || defined (SOC_J7200)
#define MEM_BARRIER_DISABLE
#endif

#include <stdint.h>
#include "hw_types.h"
#include "csl_i2c.h"

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */
#define I2C_MAX_CLK_PRESCALAR                      (255U)

#define I2C_INTERNAL_CLK_STEP                      (1000000U)

/*******************************************************************************
 *                        API FUNCTION DEFINITIONS
 *******************************************************************************/
/*TI_INSPECTED 8 D : MISRAC_2012_R_2.2
 *"No problem in redefining variable without being referenced after it's
 * definition" */
void I2CMasterInitExpClk(uint32_t baseAddr,
                         uint32_t sysClk,
                         uint32_t internalClk,
                         uint32_t outputClk)
{
    uint32_t prescalar;
    uint32_t divisor;
    uint32_t div_h, div_l;
    uint32_t actIntClk = 0U;

    /* Iterate through the valid pre-scalar values until one is found that is
     * the closest match to the desired internal clock speed
     */
    for (prescalar = 0U; prescalar < I2C_MAX_CLK_PRESCALAR; prescalar++)
    {
        /* Calculate the actual speed that would be used for the current
         * pre-scalar value, and if it is within 1 MHz of the desired value then
         * we have a match
         */
        actIntClk = sysClk / (prescalar + 1U);

        if (actIntClk <= (internalClk + I2C_INTERNAL_CLK_STEP))
        {
            break;
        }
    }

    if (outputClk > 400000U)
    {
        /* Prescalar bypassed in high speed mode */
        prescalar = 0;
        actIntClk = sysClk;
    }
    HW_WR_REG32(baseAddr + I2C_PSC, prescalar);

    /* Calculate the divisor to be used based on actual internal clock speed
     * based on pre-scalar value used
     */
    divisor = actIntClk / outputClk;
    if ((outputClk * divisor) != actIntClk)
    {
        /* Round up the divisor so that output clock never exceeds the
         * requested value if it is not exact
         */
        divisor += 1U;
    }

    /* Split into SCK HIGH and LOW time to take odd numbered divisors
     * into account and avoid reducing the output clock frequency
     */
    div_h = divisor / 2U;
    div_l = divisor - div_h;

    if (outputClk > 400000U)
    {
        HW_WR_REG32(baseAddr + I2C_SCLL, (div_l - 7U) << 8U);
        HW_WR_REG32(baseAddr + I2C_SCLH, (div_h - 5U) << 8U);
    }
    else
    {
        HW_WR_REG32(baseAddr + I2C_SCLL, div_l - 7U);
        HW_WR_REG32(baseAddr + I2C_SCLH, div_h - 5U);
    }
}

void I2CMasterEnable(uint32_t baseAddr)
{
    /* Bring the I2C module out of reset */
    HW_WR_FIELD32(baseAddr + I2C_CON, I2C_CON_I2C_EN,
                  I2C_CON_I2C_EN_ENABLE);
}

void I2CMasterEnableFreeRun(uint32_t baseAddr)
{
    /* Set the I2C module in free running mode */
    HW_WR_FIELD32(baseAddr + I2C_SYSTEST, I2C_SYSTEST_FREE,
                  I2C_SYSTEST_FREE_FREE);
}

void I2CMasterSetSysTest(uint32_t baseAddr, uint32_t sysTest)
{
    HW_WR_REG32(baseAddr + I2C_SYSTEST, sysTest);
}

uint32_t I2CMasterGetSysTest(uint32_t baseAddr)
{
    uint32_t sysTest;

    sysTest = HW_RD_REG32(baseAddr + I2C_SYSTEST);

    return (sysTest);
}

void I2CMasterDisable(uint32_t baseAddr)
{
    /* Put I2C module in reset */
    HW_WR_FIELD32(baseAddr + I2C_CON, I2C_CON_I2C_EN,
                  I2C_CON_I2C_EN_DISABLE);
}

int32_t I2CMasterBusBusy(uint32_t baseAddr)
{
    uint32_t status;

    if (HW_RD_FIELD32(baseAddr + I2C_IRQSTATUS_RAW,
                      I2C_IRQSTATUS_RAW_BB) == I2C_IRQSTATUS_RAW_BB_SET)
    {
        status = 1U;
    }
    else
    {
        status = 0U;
    }

    return (int32_t) status;
}

uint32_t I2CMasterBusy(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + I2C_CON) & I2C_CON_MST_MASK);
}

uint32_t I2CMasterErr(uint32_t baseAddr)
{
    uint32_t err;

    err = HW_RD_REG32(baseAddr + I2C_IRQSTATUS_RAW) &
                                                    (I2C_IRQSTATUS_RAW_AL_MASK
                                                    |
                                                    I2C_IRQSTATUS_RAW_AERR_MASK
                                                    |
                                                    I2C_IRQSTATUS_RAW_NACK_MASK
                                                    |
                                                    I2C_IRQSTATUS_RAW_ROVR_MASK);

    return err;
}

void I2CMasterControl(uint32_t baseAddr, uint32_t cmd)
{
    HW_WR_REG32(baseAddr + I2C_CON, cmd | I2C_CON_I2C_EN_MASK);
}

void I2CMasterStart(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_CON, I2C_CON_STT, I2C_CON_STT_STT);
}

void I2CMasterStop(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_CON, I2C_CON_STP, I2C_CON_STP_STP);
}

void I2CMasterIntEnableEx(uint32_t baseAddr, uint32_t intFlag)
{
    uint32_t i2cRegValue = HW_RD_REG32(baseAddr + I2C_IRQENABLE_SET);
    i2cRegValue |= intFlag;
    HW_WR_REG32(baseAddr + I2C_IRQENABLE_SET, i2cRegValue);
}

void I2CSlaveIntEnableEx(uint32_t baseAddr, uint32_t intFlag)
{
    uint32_t i2cRegValue = HW_RD_REG32(baseAddr + I2C_IRQENABLE_SET);
    i2cRegValue |= intFlag;
    HW_WR_REG32(baseAddr + I2C_IRQENABLE_SET, i2cRegValue);
}

void I2CMasterIntDisableEx(uint32_t baseAddr, uint32_t intFlag)
{
    HW_WR_REG32(baseAddr + I2C_IRQENABLE_CLR, intFlag);
}

void I2CSlaveIntDisableEx(uint32_t baseAddr, uint32_t intFlag)
{
    HW_WR_REG32(baseAddr + I2C_IRQENABLE_CLR, intFlag);
}

uint32_t I2CMasterIntStatus(uint32_t baseAddr)
{
    return HW_RD_REG32(baseAddr + I2C_IRQSTATUS);
}

uint32_t I2CSlaveIntStatus(uint32_t baseAddr)
{
    return HW_RD_REG32(baseAddr + I2C_IRQSTATUS);
}

uint32_t I2CMasterIntStatusEx(uint32_t baseAddr, uint32_t intFlag)
{
    return (HW_RD_REG32(baseAddr + I2C_IRQSTATUS) & intFlag);
}

uint32_t I2CSlaveIntStatusEx(uint32_t baseAddr, uint32_t intFlag)
{
    return (HW_RD_REG32(baseAddr + I2C_IRQSTATUS) & intFlag);
}

uint32_t I2CMasterIntRawStatus(uint32_t baseAddr)
{
    return HW_RD_REG32(baseAddr + I2C_IRQSTATUS_RAW);
}

uint32_t I2CSlaveIntRawStatus(uint32_t baseAddr)
{
    return HW_RD_REG32(baseAddr + I2C_IRQSTATUS_RAW);
}

uint32_t I2CMasterIntRawStatusEx(uint32_t baseAddr, uint32_t intFlag)
{
    return (HW_RD_REG32(baseAddr + I2C_IRQSTATUS_RAW) & intFlag);
}

uint32_t I2CSlaveIntRawStatusEx(uint32_t baseAddr, uint32_t intFlag)
{
    return (HW_RD_REG32(baseAddr + I2C_IRQSTATUS_RAW) & intFlag);
}

void I2CMasterIntClearEx(uint32_t baseAddr, uint32_t intFlag)
{
    HW_WR_REG32(baseAddr + I2C_IRQSTATUS, intFlag);
}

void I2CSlaveIntClearEx(uint32_t baseAddr, uint32_t intFlag)
{
    HW_WR_REG32(baseAddr + I2C_IRQSTATUS, intFlag);
}

void I2CMasterIntRawStatusClearEx(uint32_t baseAddr, uint32_t intFlag)
{
    HW_WR_REG32(baseAddr + I2C_IRQSTATUS_RAW, intFlag);
}

void I2CSlaveIntRawStatusClearEx(uint32_t baseAddr, uint32_t intFlag)
{
    HW_WR_REG32(baseAddr + I2C_IRQSTATUS_RAW, intFlag);
}

uint32_t I2CGetEnabledIntStatus(uint32_t baseAddr, uint32_t intFlag)
{
    return (HW_RD_REG32(baseAddr + I2C_IRQENABLE_SET) & intFlag);
}

void I2CMasterSlaveAddrSet(uint32_t baseAddr, uint32_t slaveAdd)
{
    /*Set the address of the slave with which the master will communicate.*/
    HW_WR_REG32(baseAddr + I2C_SA, slaveAdd);
}

uint32_t I2CMasterSlaveAddrGet(uint32_t baseAddr, uint32_t slaveAdd)
{
    /* Returns the address of the slave with which the master will
     *communicate.*/
    return (HW_RD_REG32(baseAddr + I2C_SA) & I2C_SA_SA_MASK);
}

void I2CSetDataCount(uint32_t baseAddr, uint32_t count)
{
    HW_WR_REG32(baseAddr + I2C_CNT, count);
}

uint32_t I2CDataCountGet(uint32_t baseAddr)
{
    return HW_RD_REG32(baseAddr + I2C_CNT);
}

void I2CFIFOThresholdConfig(uint32_t baseAddr,
                            uint32_t thresholdVal,
                            uint32_t flag)
{
    if (I2C_TX_MODE == flag)
    {
        HW_WR_FIELD32(baseAddr + I2C_BUF, I2C_BUF_TXTRSH, thresholdVal);
    }
    else
    {
        HW_WR_FIELD32(baseAddr + I2C_BUF, I2C_BUF_RXTRSH, thresholdVal);
    }
}

void I2CFIFOClear(uint32_t baseAddr, uint32_t flag)
{
    if (I2C_TX_MODE == flag)
    {
        HW_WR_FIELD32(baseAddr + I2C_BUF, I2C_BUF_TXFIFO_CLR,
                      I2C_BUF_TXFIFO_CLR_RSTMODE);
    }
    else
    {
        HW_WR_FIELD32(baseAddr + I2C_BUF, I2C_BUF_RXFIFO_CLR,
                      I2C_BUF_RXFIFO_CLR_RSTMODE);
    }
}

void I2CDMATxEventEnable(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_BUF, I2C_BUF_XDMA_EN,
                  I2C_BUF_XDMA_EN_ENABLE);

    HW_WR_REG32(baseAddr + I2C_DMATXENABLE_SET,
                (uint32_t) I2C_DMATXENABLE_SET_DMATX_ENABLE_SET_MASK);
}

void I2CDMARxEventEnable(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_BUF, I2C_BUF_RDMA_EN,
                  I2C_BUF_RDMA_EN_ENABLE);

    HW_WR_REG32(baseAddr + I2C_DMARXENABLE_SET,
                (uint32_t) I2C_DMARXENABLE_SET_DMARX_ENABLE_SET_MASK);
}

void I2CDMATxEventDisable(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_BUF, I2C_BUF_XDMA_EN,
                  I2C_BUF_XDMA_EN_DISABLE);

    HW_WR_REG32(baseAddr + I2C_DMATXENABLE_CLR,
                (uint32_t) I2C_DMATXENABLE_CLR_DMATX_ENABLE_CLEAR_MASK);
}

void I2CDMARxEventDisable(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_BUF, I2C_BUF_RDMA_EN,
                  I2C_BUF_RDMA_EN_DISABLE);

    HW_WR_REG32(baseAddr + I2C_DMARXENABLE_CLR,
                (uint32_t) I2C_DMARXENABLE_CLR_DMARX_ENABLE_CLEAR_MASK);
}

uint32_t I2CBufferStatus(uint32_t baseAddr, uint32_t flag)
{
    uint32_t status;

    switch (flag)
    {
        case I2C_TX_BUFFER_STATUS:
            status = HW_RD_FIELD32(baseAddr + I2C_BUFSTAT, I2C_BUFSTAT_TXSTAT);
            break;

        case I2C_RX_BUFFER_STATUS:
            status = HW_RD_FIELD32(baseAddr + I2C_BUFSTAT, I2C_BUFSTAT_RXSTAT);
            break;

        case I2C_FIFO_DEPTH:
            status = HW_RD_FIELD32(baseAddr + I2C_BUFSTAT, I2C_BUFSTAT_FIFODEPTH);
            break;

        default:
            /* Invalid input */
            status = 0U;
            break;
    }

    return status;
}

uint32_t I2CActiveOwnAddressGet(uint32_t baseAddr)
{
    uint32_t ownAddr;
    if (HW_RD_FIELD32(baseAddr + I2C_ACTOA,
                      I2C_ACTOA_OA0_ACT) == I2C_ACTOA_OA0_ACT_ACTIVE)
    {
        ownAddr = HW_RD_REG32(baseAddr + I2C_OA);
    }
    else if (HW_RD_FIELD32(baseAddr + I2C_ACTOA,
                           I2C_ACTOA_OA1_ACT) == I2C_ACTOA_OA1_ACT_ACTIVE)
    {
        ownAddr = HW_RD_REG32(baseAddr + I2C_OA1);
    }
    else if (HW_RD_FIELD32(baseAddr + I2C_ACTOA,
                           I2C_ACTOA_OA2_ACT) == I2C_ACTOA_OA2_ACT_ACTIVE)
    {
        ownAddr = HW_RD_REG32(baseAddr + I2C_OA2);
    }
    else if (HW_RD_FIELD32(baseAddr + I2C_ACTOA,
                           I2C_ACTOA_OA3_ACT) == I2C_ACTOA_OA3_ACT_ACTIVE)
    {
        ownAddr = HW_RD_REG32(baseAddr + I2C_OA3);
    }
    else
    {
        /*
         * Do nothing
         */
         ownAddr = 0U;
    }

    return ownAddr;
}

void I2CClockBlockingControl(uint32_t baseAddr,
                             uint32_t ownAdd0,
                             uint32_t ownAdd1,
                             uint32_t ownAdd2,
                             uint32_t ownAdd3)
{
    HW_WR_FIELD32(baseAddr + I2C_SBLOCK, I2C_SBLOCK_OA0_EN, ownAdd0);

    HW_WR_FIELD32(baseAddr + I2C_SBLOCK, I2C_SBLOCK_OA1_EN, ownAdd1);

    HW_WR_FIELD32(baseAddr + I2C_SBLOCK, I2C_SBLOCK_OA2_EN, ownAdd2);

    HW_WR_FIELD32(baseAddr + I2C_SBLOCK, I2C_SBLOCK_OA3_EN, ownAdd3);
}

void I2COwnAddressSet(uint32_t baseAddr, uint32_t ownAdd,
                      uint32_t flag)
{
    switch (flag)
    {
        case I2C_OWN_ADDR_0:
            HW_WR_REG32(baseAddr + I2C_OA, ownAdd);
            break;

        case I2C_OWN_ADDR_1:
            HW_WR_REG32(baseAddr + I2C_OA1, ownAdd);
            break;

        case I2C_OWN_ADDR_2:
            HW_WR_REG32(baseAddr + I2C_OA2, ownAdd);
            break;

        case I2C_OWN_ADDR_3:
            HW_WR_REG32(baseAddr + I2C_OA3, ownAdd);
            break;

        default:
            /* Invalid input */
            break;
    }
}

void I2CSoftReset(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_SYSC, I2C_SYSC_SRST,
                  I2C_SYSC_SRST_RSTMODE);
}

void I2CAutoIdleEnable(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_SYSC, I2C_SYSC_AUTOIDLE,
                  I2C_SYSC_AUTOIDLE_ENABLE);
}

void I2CAutoIdleDisable(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_SYSC, I2C_SYSC_AUTOIDLE,
                  I2C_SYSC_AUTOIDLE_DISABLE);
}

void I2CGlobalWakeUpEnable(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_SYSC, I2C_SYSC_ENAWAKEUP,
                  I2C_SYSC_ENAWAKEUP_ENABLE);
}

void I2CGlobalWakeUpDisable(uint32_t baseAddr)
{
    HW_WR_FIELD32(baseAddr + I2C_SYSC, I2C_SYSC_ENAWAKEUP,
                  I2C_SYSC_ENAWAKEUP_DISABLE);
}

void I2CIdleModeSelect(uint32_t baseAddr, uint32_t flag)
{
    HW_WR_FIELD32(baseAddr + I2C_SYSC, I2C_SYSC_IDLEMODE,
                  flag >> I2C_SYSC_IDLEMODE_SHIFT);
}

void I2CClockActivitySelect(uint32_t baseAddr, uint32_t flag)
{
    HW_WR_FIELD32(baseAddr + I2C_SYSC, I2C_SYSC_CLKACTIVITY,
                  flag >> I2C_SYSC_CLKACTIVITY_SHIFT);
}

void I2CWakeUpEnable(uint32_t baseAddr,
                     uint32_t eventFlag,
                     uint32_t flag)
{
    uint32_t i2cRegValue;
    if (I2C_WAKE_UP_IRQ == flag)
    {
        i2cRegValue  = HW_RD_REG32(baseAddr + I2C_WE);
        i2cRegValue |= eventFlag;
        HW_WR_REG32(baseAddr + I2C_WE, i2cRegValue);
    }
    else if (I2C_WAKE_UP_DMA_RECV == flag)
    {
        i2cRegValue  = HW_RD_REG32(baseAddr + I2C_DMARXWAKE_EN);
        i2cRegValue |= eventFlag;
        HW_WR_REG32(baseAddr + I2C_DMARXWAKE_EN, i2cRegValue);
    }
    else if (I2C_WAKE_UP_DMA_TRANSMIT == flag)
    {
        i2cRegValue  = HW_RD_REG32(baseAddr + I2C_DMATXWAKE_EN);
        i2cRegValue |= eventFlag;
        HW_WR_REG32(baseAddr + I2C_DMATXWAKE_EN, i2cRegValue);
    }
    else
    {
        /*
         * Do nothing
         */
    }
}

void I2CWakeUpDisable(uint32_t baseAddr,
                      uint32_t eventFlag,
                      uint32_t flag)
{
    uint32_t i2cRegValue;
    if (I2C_WAKE_UP_IRQ == flag)
    {
        i2cRegValue  = HW_RD_REG32(baseAddr + I2C_WE);
        i2cRegValue &= ~eventFlag;
        HW_WR_REG32(baseAddr + I2C_WE, i2cRegValue);
    }
    else if (I2C_WAKE_UP_DMA_RECV == flag)
    {
        i2cRegValue  = HW_RD_REG32(baseAddr + I2C_DMARXWAKE_EN);
        i2cRegValue &= ~eventFlag;
        HW_WR_REG32(baseAddr + I2C_DMARXWAKE_EN, i2cRegValue);
    }
    else if (I2C_WAKE_UP_DMA_TRANSMIT == flag)
    {
        i2cRegValue  = HW_RD_REG32(baseAddr + I2C_DMATXWAKE_EN);
        i2cRegValue &= ~eventFlag;
        HW_WR_REG32(baseAddr + I2C_DMATXWAKE_EN, i2cRegValue);
    }
    else
    {
        /*
         * Do nothing
         */
    }
}

uint32_t I2CSystemStatusGet(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + I2C_SYSS) & I2C_SYSS_RDONE_MASK);
}

void I2CMasterDataPut(uint32_t baseAddr, uint8_t data)
{
    /*write data to be transmitted to Data transmit register */
    HW_WR_REG32(baseAddr + I2C_DATA, (uint32_t) data);
}

uint8_t I2CMasterDataGet(uint32_t baseAddr)
{
    uint8_t rData;

    rData = (uint8_t) HW_RD_REG32(baseAddr + I2C_DATA);
    return rData;
}

void I2CSlaveDataPut(uint32_t baseAddr, uint32_t data)
{
    HW_WR_REG32(baseAddr + I2C_DATA, data);
}

uint8_t I2CSlaveDataGet(uint32_t baseAddr)
{
    uint8_t rData;

    rData = (uint8_t) HW_RD_REG32(baseAddr + I2C_DATA);

    return rData;
}

void I2CSyscInit(uint32_t baseAddr, uint32_t syscFlag)
{
    HW_WR_REG32(baseAddr + I2C_SYSC, syscFlag);
}

void I2CConfig(uint32_t baseAddr, uint32_t conParams)
{
    HW_WR_REG32(baseAddr + I2C_CON, conParams);
}

void I2CFlushFifo(uint32_t baseAddr)
{
    uint32_t i2cRegValue = HW_RD_REG32(baseAddr + I2C_BUF);
    i2cRegValue |= I2C_BUF_RXFIFO_CLR_MASK | I2C_BUF_TXFIFO_CLR_MASK;
    HW_WR_REG32(baseAddr + I2C_BUF, i2cRegValue);
}
/********************************* End of file ******************************/
