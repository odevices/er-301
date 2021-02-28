/**
 * \file       mcspi.c
 *
 * \brief      This file contains the function definitions for the device
 *             abstraction layer for MCSPI.
 */

/**
 * \copyright  Copyright (C) 2013 Texas Instruments Incorporated -
 *             http://www.ti.com/
 */

/**
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
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "types.h"
#include "error.h"
#include "hw_types.h"
#include "mcspi.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 * \brief     Mask value used to extract MCSPI clock divider value.
 *
 */
#define MCSPI_CLKDIV_MASK       (0xFU)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */



/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void MCSPIClkConfig(uint32_t baseAddr,
                    uint32_t chNum,
                    uint32_t spiInClk,
                    uint32_t spiOutClk,
                    uint32_t clkMode)
{
    uint32_t clkRatio = 0U;
    uint32_t polarity = 0U;
    uint32_t regVal = 0U;
    uint32_t extClk = 0U;
    uint32_t clkDiv = 0U;
    uint32_t phase = 0U;

    /* Calculate the value of clkRatio. */
    clkRatio = (spiInClk / spiOutClk);

    /* If clkRatio is not a power of 2, set granularity of 1 clock cycle */
    if(0U != (clkRatio & (clkRatio - 1U)))
    {
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_CLKG,
            MCSPI_CHCONF_CLKG_ONECYCLE);

        /* Calculate the ratios clkD and extClk based on fClk */
        extClk = (clkRatio - 1U) >> 4U;
        clkDiv = (clkRatio - 1U) & MCSPI_CLKDIV_MASK;

        HW_WR_FIELD32((baseAddr + MCSPI_CHCTRL(chNum)),
            MCSPI_CHCTRL_EXTCLK,
            extClk);
    }
    else
    {
        /* Clock granularity of power of 2. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_CLKG,
            MCSPI_CHCONF_CLKG_POWERTWO);

        while(1U != clkRatio)
        {
            clkRatio /= 2U;
            clkDiv++;
        }
    }

    /* Configure the clkD field of MCSPI_CHCONF register. */
    HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)), MCSPI_CHCONF_CLKD, clkDiv);

    /* Configure the Phase and Polarity values. */
    phase = ((clkMode & MCSPI_CHCONF_PHA_MASK) >> MCSPI_CHCONF_PHA_SHIFT);
    polarity = ((clkMode & MCSPI_CHCONF_POL_MASK) >> MCSPI_CHCONF_POL_SHIFT);
    regVal = HW_RD_REG32(baseAddr + MCSPI_CHCONF(chNum));
    HW_SET_FIELD(regVal, MCSPI_CHCONF_PHA, phase);
    HW_SET_FIELD(regVal, MCSPI_CHCONF_POL, polarity);
    HW_WR_REG32((baseAddr + MCSPI_CHCONF(chNum)), regVal);
}

void MCSPISetWordLength(uint32_t baseAddr,
                        uint32_t chNum,
                        uint32_t wordLength)
{
    /* Configure the wordlength field. */
    HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
        MCSPI_CHCONF_WL,
        (wordLength - 1U));
}

void MCSPICsEnable(uint32_t baseAddr, uint32_t enableChipSel)
{
    if (TRUE == enableChipSel)
    {
        /* Enable MCSPI controller in 4 Pin mode of operation. */
        HW_WR_FIELD32((baseAddr + MCSPI_MODULCTRL),
            MCSPI_MODULCTRL_PIN34,
            MCSPI_MODULCTRL_PIN34_4PINMODE);
    }
    else
    {
        /* Disable MCSPI controller from 4 Pin mode of operation. */
        HW_WR_FIELD32((baseAddr + MCSPI_MODULCTRL),
            MCSPI_MODULCTRL_PIN34,
            MCSPI_MODULCTRL_PIN34_3PINMODE);
    }
}

void MCSPISetCsPol(uint32_t baseAddr, uint32_t chNum, uint32_t csPol)
{
    /* Configure the CS polarity. */
    HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
        MCSPI_CHCONF_EPOL,
        csPol);
}

void MCSPISetCsTimeControl(uint32_t baseAddr,
                           uint32_t chNum,
                           uint32_t csTimeControl)
{
    /* Configure the CS time control value. */
    HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
        MCSPI_CHCONF_TCS,
        csTimeControl);
}

void MCSPICsAssert(uint32_t baseAddr, uint32_t chNum)
{
    /* Assert the CS line. */
    HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
        MCSPI_CHCONF_FORCE,
        MCSPI_CHCONF_FORCE_ASSERT);
}

void MCSPICsDeAssert(uint32_t baseAddr, uint32_t chNum)
{
    /* Deassert the CS line. */
    HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
        MCSPI_CHCONF_FORCE,
        MCSPI_CHCONF_FORCE_DEASSERT);
}

void MCSPIStartBitEnable(uint32_t baseAddr,
                         uint32_t chNum,
                         uint32_t enableStartBit)
{
    if(TRUE == enableStartBit)
    {
        /* Enable the start bit. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_SBE,
            MCSPI_CHCONF_SBE_ENABLED);
    }
    else
    {
        /* Disable the start bit. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_SBE,
            MCSPI_CHCONF_SBE_DISABLED);
    }
}

void MCSPISetStartBitPol(uint32_t baseAddr,
                         uint32_t chNum,
                         uint32_t startBitPol)
{
    /* Configure the start bit polarity. */
    HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
        MCSPI_CHCONF_SBPOL,
        startBitPol);
}

int32_t MCSPIModeConfig(uint32_t baseAddr,
                        uint32_t chNum,
                        uint32_t spiMode,
                        uint32_t chMode,
                        uint32_t trMode,
                        uint32_t pinMode)
{
    uint32_t retVal = E_FAIL;
    uint32_t inputSelect = 0U;
    uint32_t dataLine1 = 0U;
    uint32_t dataLine0 = 0U;
    uint32_t regVal;

    if (MCSPI_MODE_MASTER == spiMode)
    {
        /* Configure MCSPI controller in Master mode. */
        HW_WR_FIELD32((baseAddr + MCSPI_MODULCTRL),
            MCSPI_MODULCTRL_MS,
            MCSPI_MODULCTRL_MS_MASTER);

        /* Configure the MCSPI controller single/multichannel mode. */
        HW_WR_FIELD32((baseAddr + MCSPI_MODULCTRL),
            MCSPI_MODULCTRL_SINGLE,
            chMode);
    }
    else if (MCSPI_MODE_SLAVE == spiMode)
    {
        /* Configure MCSPI controller in Slave mode. */
        HW_WR_FIELD32((baseAddr + MCSPI_MODULCTRL),
            MCSPI_MODULCTRL_MS,
            MCSPI_MODULCTRL_MS_SLAVE);

        /* Only channel 0 is supported in Slave mode. */
        chNum = 0U;

        /* Configure the SPIENSLV field. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_SPIENSLV,
            chNum);
    }
    else
    {
        /* Perform nothing. Invalid parameter. */
    }

    /* Configure the MCSPI controller in Tx/Rx modes. */
    HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)), MCSPI_CHCONF_TRM, trMode);

    if(((MCSPI_TRANSFER_MODE_TX_RX == trMode) &&
       (MCSPI_DATA_LINE_COMM_MODE_3 == pinMode)) ||
       ((MCSPI_TRANSFER_MODE_TX_ONLY == trMode) &&
       (MCSPI_DATA_LINE_COMM_MODE_3 == pinMode)) ||
       ((MCSPI_TRANSFER_MODE_TX_RX == trMode) &&
       (MCSPI_DATA_LINE_COMM_MODE_7 == pinMode)) ||
       ((MCSPI_TRANSFER_MODE_TX_ONLY == trMode) &&
       (MCSPI_DATA_LINE_COMM_MODE_7 == pinMode)))
    {
        retVal = E_FAIL;
    }
    else
    {
        /* Extract the input select value. */
        inputSelect = ((pinMode & MCSPI_CHCONF_IS_MASK) >>
                        MCSPI_CHCONF_IS_SHIFT);
        /* Extract the transmission enable value for data line 1. */
        dataLine1 = ((pinMode & MCSPI_CHCONF_DPE1_MASK) >>
                      MCSPI_CHCONF_DPE1_SHIFT);
        /* Extract the transmission enable value for data line 0. */
        dataLine0 = ((pinMode & MCSPI_CHCONF_DPE0_MASK) >>
                      MCSPI_CHCONF_DPE0_SHIFT);

        /* Configure the IS, DPE0 and DPE1 field of MCSPI_CHCONF register. */
        regVal = HW_RD_REG32(baseAddr + MCSPI_CHCONF(chNum));
        HW_SET_FIELD(regVal, MCSPI_CHCONF_IS, inputSelect);
        HW_SET_FIELD(regVal, MCSPI_CHCONF_DPE1, dataLine1);
        HW_SET_FIELD(regVal, MCSPI_CHCONF_DPE0, dataLine0);
        HW_WR_REG32((baseAddr + MCSPI_CHCONF(chNum)), regVal);

        retVal = S_PASS;
    }
    return retVal;
}

void MCSPIChEnable(uint32_t baseAddr,
                   uint32_t chNum,
                   uint32_t enableCh)
{
    if (TRUE == enableCh)
    {
        /* Enable the MCSPI channel. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCTRL(chNum)),
            MCSPI_CHCTRL_EN,
            MCSPI_CHCTRL_EN_ACT);
    }
    else
    {
        /* Disable the MCSPI channel. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCTRL(chNum)),
            MCSPI_CHCTRL_EN,
            MCSPI_CHCTRL_EN_NACT);
    }
}

void MCSPIReset(uint32_t baseAddr)
{
    /* Perform soft reset on the MCSPI controller. */
    HW_WR_FIELD32((baseAddr + MCSPI_SYSCONFIG),
        MCSPI_SYSCONFIG_SOFTRESET,
        MCSPI_SYSCONFIG_SOFTRESET_ON);

    /* Stay in the loop until reset is done. */
    while(!(MCSPI_SYSSTS_RESETDONE_MASK &
            HW_RD_REG32(baseAddr + MCSPI_SYSSTS)));
}

void MCSPITurboModeEnable(uint32_t baseAddr,
                          uint32_t chNum,
                          uint32_t enableTurboMode)
{
    if (TRUE == enableTurboMode)
    {
        /* Enable TURBO mode of operation. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_TURBO,
            MCSPI_CHCONF_TURBO_ON);
    }
    else
    {
        /* Disable TURBO mode of operation. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_TURBO,
            MCSPI_CHCONF_TURBO_OFF);
    }
}

void MCSPITxFifoEnable(uint32_t baseAddr,
                       uint32_t chNum,
                       uint32_t enableTxFifo)
{
    if (TRUE == enableTxFifo)
    {
        /* Enable Tx FIFO. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_FFEW,
            MCSPI_CHCONF_FFEW_FFENABLED);
    }
    else
    {
        /* Disable Tx FIFO. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_FFEW,
            MCSPI_CHCONF_FFEW_FFDISABLED);
    }
}

void MCSPIRxFifoEnable(uint32_t baseAddr,
                       uint32_t chNum,
                       uint32_t enableRxFifo)
{
    if (TRUE == enableRxFifo)
    {
        /* Enable Rx FIFO. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_FFER,
            MCSPI_CHCONF_FFER_FFENABLED);
    }
    else
    {
        /* Disable Rx FIFO. */
        HW_WR_FIELD32((baseAddr + MCSPI_CHCONF(chNum)),
            MCSPI_CHCONF_FFER,
            MCSPI_CHCONF_FFER_FFDISABLED);
    }
}

void MCSPISetFifoTriggerLvl(uint32_t baseAddr,
                            uint8_t bufFullVal,
                            uint8_t bufEmptyVal,
                            uint32_t trMode)
{
    uint32_t regVal;

    if(MCSPI_TRANSFER_MODE_TX_RX == trMode)
    {
        /* Configure the afl and ael field. */
        regVal = HW_RD_REG32(baseAddr + MCSPI_XFERLEVEL);
        HW_SET_FIELD(regVal, MCSPI_XFERLEVEL_AFL, (bufFullVal - 1U));
        HW_SET_FIELD(regVal, MCSPI_XFERLEVEL_AEL, (bufEmptyVal - 1U));
        HW_WR_REG32((baseAddr + MCSPI_XFERLEVEL), regVal);
    }
    else if(MCSPI_TRANSFER_MODE_TX_ONLY == trMode)
    {
        /* Configure the ael field. */
        HW_WR_FIELD32((baseAddr + MCSPI_XFERLEVEL),
            MCSPI_XFERLEVEL_AEL,
            (bufEmptyVal - 1U));
    }
    else if(MCSPI_TRANSFER_MODE_RX_ONLY == trMode)
    {
        /* Configure the afl field. */
        HW_WR_FIELD32((baseAddr + MCSPI_XFERLEVEL),
            MCSPI_XFERLEVEL_AFL,
            (bufFullVal - 1U));
    }
    else
    {
        /* Perform nothing. */
    }
}

void MCSPISetWordCount(uint32_t baseAddr, uint16_t wordCnt)
{
    /* Configure the SPI word count. */
    HW_WR_FIELD32((baseAddr + MCSPI_XFERLEVEL), MCSPI_XFERLEVEL_WCNT, wordCnt);
}

void MCSPIDmaEnable(uint32_t baseAddr,
                    uint32_t chNum,
                    uint32_t dmaFlags,
                    uint32_t enableDma)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + MCSPI_CHCONF(chNum));

    if (TRUE == enableDma)
    {
        /* Enable the DMA events. */
        regVal |= (dmaFlags & (MCSPI_CHCONF_DMAR_MASK |
                               MCSPI_CHCONF_DMAW_MASK));
    }
    else
    {
        /* Disable the DMA events. */
        regVal &= ~(dmaFlags & (MCSPI_CHCONF_DMAR_MASK |
                                MCSPI_CHCONF_DMAW_MASK));
    }

    HW_WR_REG32((baseAddr + MCSPI_CHCONF(chNum)), regVal);
}

void MCSPIIntrEnable(uint32_t baseAddr, uint32_t intrFlags)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + MCSPI_IRQEN);

    /* Enable the interrupts. */
    regVal |= intrFlags;

    HW_WR_REG32((baseAddr + MCSPI_IRQEN), regVal);
}

void MCSPIIntrDisable(uint32_t baseAddr, uint32_t intrFlags)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + MCSPI_IRQEN);

    /* Disable the interrupts. */
    regVal &= ~intrFlags;

    HW_WR_REG32((baseAddr + MCSPI_IRQEN), regVal);
}

void MCSPISetInitialDelay(uint32_t baseAddr, uint32_t initDelay)
{
    /* Configure the initial delay value. */
    HW_WR_FIELD32((baseAddr + MCSPI_MODULCTRL),
        MCSPI_MODULCTRL_INITDLY,
        initDelay);
}

void MCSPITransmitData(uint32_t baseAddr,
                       uint32_t chNum,
                       uint32_t txData)
{
    /* Write data to be transmitted. */
    HW_WR_REG32((baseAddr + MCSPI_TX(chNum)), txData);
}

uint32_t MCSPIReceiveData(uint32_t baseAddr, uint32_t chNum)
{
    /* Read the received data. */
    return(HW_RD_REG32(baseAddr + MCSPI_RX(chNum)));
}

uint32_t MCSPIIntrStatus(uint32_t baseAddr)
{
    /* Return the interrupt status. */
    return(HW_RD_REG32(baseAddr + MCSPI_IRQSTS));
}

void MCSPIIntrClear(uint32_t baseAddr, uint32_t intrFlags)
{
    /* Clear the SSB field of MCSPI system register. */
    HW_WR_FIELD32((baseAddr + MCSPI_SYST), MCSPI_SYST_SSB, MCSPI_SYST_SSB_OFF);

    /* Clear the interrupt status. */
    HW_WR_REG32((baseAddr + MCSPI_IRQSTS), intrFlags);
}

uint32_t MCSPIChStatus(uint32_t baseAddr, uint32_t chNum)
{
    /* Return the MCSPI channel status. */
    return(HW_RD_REG32(baseAddr + MCSPI_CHSTAT(chNum)));
}

void MCSPIMultipleWordAccessEnable(uint32_t baseAddr, uint32_t enableMoa)
{
    if (TRUE == enableMoa)
    {
        /* Enable MOA feature. */
        HW_WR_FIELD32((baseAddr + MCSPI_MODULCTRL),
            MCSPI_MODULCTRL_MOA,
            MCSPI_MODULCTRL_MOA_MULTIACCES);
    }
    else
    {
        /* Disable MOA feature. */
        HW_WR_FIELD32((baseAddr + MCSPI_MODULCTRL),
            MCSPI_MODULCTRL_MOA,
            MCSPI_MODULCTRL_MOA_NOMULTIACCESS);
    }
}

void MCSPIFifoDataMgmtEnable(uint32_t baseAddr, uint32_t enableFifoData)
{
    if (TRUE == enableFifoData)
    {
        /* Enable access to shadow registers. */
        HW_WR_FIELD32((baseAddr + MCSPI_MODULCTRL),
                       MCSPI_MODULCTRL_FDAA,
                       MCSPI_MODULCTRL_FDAA_SHADOWREGEN);
    }
    else
    {
        /* Disable access to shadow registers. */
        HW_WR_FIELD32((baseAddr + MCSPI_MODULCTRL),
                       MCSPI_MODULCTRL_FDAA,
                       MCSPI_MODULCTRL_FDAA_NOSHADOWREG);
    }
}
/* ========================================================================== */
/*                          Deprecated Function Definitions                   */
/* ========================================================================== */
