/**
 * \file       tsc_adc_ss.c
 *
 * \brief      This file contains the function definitions for the device
 *             abstraction layer for TSC_ADC_SS IP.
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
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "types.h"
#include "hw_types.h"
#include "tsc_adc_ss.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

uint32_t TSCADCGetRevision(uint32_t baseAddr)
{
    return(HW_RD_REG32(baseAddr + ADC0_REVISION));
}

void TSCADCClkDivConfig(uint32_t baseAddr, uint32_t moduleClk,
                        uint32_t afeInputClk)
{
    uint32_t clkDiv = 0U;

    clkDiv = moduleClk/afeInputClk;

    /* Configure the clock divider value. */
    HW_WR_FIELD32((baseAddr + ADC0_ADC_CLKDIV),
                   ADC0_ADC_CLKDIV,
                   (clkDiv - 1));
}

void TSCADCSetHwEventMap(uint32_t baseAddr, uint32_t hwEvent)
{
    /* Set the hardware event. */
    HW_WR_FIELD32((baseAddr + ADC0_CTRL),
                   ADC0_CTRL_HW_EVT_MAPPING, hwEvent);
}

void TSCADCSetMode(uint32_t baseAddr, uint32_t mode)
{
    if (TSCADC_MODE_GP_ADC == mode)
    {
        HW_WR_FIELD32((baseAddr + ADC0_CTRL),
               ADC0_CTRL_TOUCH_SCREEN_EN,
               ADC0_CTRL_TOUCH_SCREEN_EN_DISABLE);
    }
    else if ((TSCADC_MODE_FOUR_WIRE == mode) || (TSCADC_MODE_FIVE_WIRE == mode))
    {
        HW_WR_FIELD32((baseAddr + ADC0_CTRL),
               ADC0_CTRL_TOUCH_SCREEN_EN,
               ADC0_CTRL_TOUCH_SCREEN_EN_ENABLE);
    }
    else
    {
        /* Perform nothing. */
    }

    /* Configure the TSCADC mode. */
    HW_WR_FIELD32((baseAddr + ADC0_CTRL), ADC0_CTRL_AFE_PEN,
                   mode);
}

void TSCADCStepIdTagEnable(uint32_t baseAddr, uint32_t enableStepIdTag)
{
    /* Configure the Step ID tag. */
    if (TRUE == enableStepIdTag)
    {
        HW_WR_FIELD32((baseAddr + ADC0_CTRL),
                       ADC0_CTRL_STEP_ID_TAG,
                       ADC0_CTRL_STEP_ID_TAG_CHANNELID);
    }
    else
    {
        HW_WR_FIELD32((baseAddr + ADC0_CTRL),
                       ADC0_CTRL_STEP_ID_TAG,
                       ADC0_CTRL_STEP_ID_TAG_WRZERO);
    }
}

void TSCADCStepConfigWrProtectEnable(uint32_t baseAddr,
                                     uint32_t enableWrProtect)
{
    if (TRUE == enableWrProtect)
    {
        HW_WR_FIELD32((baseAddr + ADC0_CTRL),
                       ADC0_CTRL_STEPCONFIG_WRITEPROTECT_N,
                       ADC0_CTRL_STEPCONFIG_WRITEPROTECT_N_PROTECTED);
    }
    else
    {
        HW_WR_FIELD32((baseAddr + ADC0_CTRL),
                       ADC0_CTRL_STEPCONFIG_WRITEPROTECT_N,
                       ADC0_CTRL_STEPCONFIG_WRITEPROTECT_N_NOTPROTECTED);
    }
}

void TSCADCTsIdleStepConfig(uint32_t baseAddr,
                            uint32_t enableDiffCtrl,
                            tscAdcStepCfg_t *pStepCfg)
{
    uint32_t regVal = 0U;

    /* Configure the differential control. */
    if (TRUE == enableDiffCtrl)
    {
        HW_WR_FIELD32((baseAddr + ADC0_IDLECONFIG),
                       ADC0_IDLECONFIG_DIFF_CNTRL,
                       ADC0_IDLECONFIG_DIFF_CNTRL_DIFFERENTIAL);
    }
    else
    {
        HW_WR_FIELD32((baseAddr + ADC0_IDLECONFIG),
                       ADC0_IDLECONFIG_DIFF_CNTRL,
                       ADC0_IDLECONFIG_DIFF_CNTRL_SINGLE);
    }

    regVal = HW_RD_REG32(baseAddr + ADC0_IDLECONFIG);

    /* Configure the negative reference voltage. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_SEL_RFM_SWC,
                 pStepCfg->adcNegativeInpRef);

    /* Configure the positive reference voltage. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_SEL_RFP_SWC,
                 pStepCfg->adcPositiveInpRef);

    /* Configure the negative input channel. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_SEL_INM_SWM,
                 pStepCfg->adcNegativeInpChan);

    /* Configure the positive input channel. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_SEL_INP_SWC,
                 pStepCfg->adcPositiveInpChan);

    /* Control the XPPSW pin. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_XPPSW_SWC,
                 pStepCfg->enableXppsw);

    /* Control the XNPSW pin. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_XNPSW_SWC,
                 pStepCfg->enableXnpsw);

    /* Control the YPPSW pin. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_YPPSW_SWC,
                 pStepCfg->enableYppsw);

    /* Control the XNNSW pin. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_XNNSW_SWC,
                 pStepCfg->enableXnnsw);

    /* Control the YPNSW pin. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_YPNSW_SWC,
                 pStepCfg->enableYpnsw);

    /* Control the YNNSW pin. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_YNNSW_SWC,
                 pStepCfg->enableYnnsw);

    /* Control the WPNSW pin. */
    HW_SET_FIELD(regVal, ADC0_IDLECONFIG_WPNSW_SWC,
                 pStepCfg->enableWpnsw);

    /* Write variable value to register. */
    HW_WR_REG32((baseAddr + ADC0_IDLECONFIG), regVal);
}

void TSCADCTsChargeStepConfig(uint32_t baseAddr,
                              uint32_t enableDiffCtrl,
                              tscAdcStepCfg_t *pStepCfg)
{
    uint32_t regVal = 0U;

    /* Configure the differential control. */
    if (TRUE == enableDiffCtrl)
    {
        HW_WR_FIELD32((baseAddr + ADC0_TS_CHARGE_STEPCONFIG),
                       ADC0_TS_CHARGE_STEPCONFIG_DIFF_CNTRL,
                       ADC0_TS_CHARGE_STEPCONFIG_DIFF_CNTRL_DIFFERENTIAL);
    }
    else
    {
        HW_WR_FIELD32((baseAddr + ADC0_TS_CHARGE_STEPCONFIG),
                       ADC0_TS_CHARGE_STEPCONFIG_DIFF_CNTRL,
                       ADC0_TS_CHARGE_STEPCONFIG_DIFF_CNTRL_SINGLE);
    }

    regVal = HW_RD_REG32(baseAddr + ADC0_TS_CHARGE_STEPCONFIG);

    /* Configure the negative reference voltage. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_SEL_RFM_SWC,
                 pStepCfg->adcNegativeInpRef);

    /* Configure the positive reference voltage. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_SEL_RFP_SWC,
                 pStepCfg->adcPositiveInpRef);

    /* Configure the negative input channel. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_SEL_INM_SWM,
                 pStepCfg->adcNegativeInpChan);

    /* Configure the positive input channel. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_SEL_INP_SWC,
                 pStepCfg->adcPositiveInpChan);

    /* Control the XPPSW pin. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_XPPSW_SWC,
                 pStepCfg->enableXppsw);

    /* Control the XNPSW pin. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_XNPSW_SWC,
                 pStepCfg->enableXnpsw);

    /* Control the YPPSW pin. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_YPPSW_SWC,
                 pStepCfg->enableYppsw);

    /* Control the XNNSW pin. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_XNNSW_SWC,
                 pStepCfg->enableXnnsw);

    /* Control the YPNSW pin. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_YPNSW_SWC,
                 pStepCfg->enableYpnsw);

    /* Control the YNNSW pin. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_YNNSW_SWC,
                 pStepCfg->enableYnnsw);

    /* Control the WPNSW pin. */
    HW_SET_FIELD(regVal, ADC0_TS_CHARGE_STEPCONFIG_WPNSW_SWC,
                 pStepCfg->enableWpnsw);

    /* Write variable value to register. */
    HW_WR_REG32((baseAddr + ADC0_TS_CHARGE_STEPCONFIG), regVal);
}

void TSCADCTsChargeStepDelay(uint32_t baseAddr, uint32_t openDelay)
{
    /* Configure the open delay. */
    HW_WR_FIELD32((baseAddr + ADC0_TS_CHARGE_DELAY),
                   ADC0_TS_CHARGE_DELAY_OPENDELAY,
                   openDelay);
}

void TSCADCStepConfig(uint32_t baseAddr,
                      uint32_t stepNum,
                      uint32_t enableDiffCtrl,
                      tscAdcStepCfg_t *pStepCfg)
{
    uint32_t regVal = 0U;

    /* Configure the differential control. */
    if (TRUE == enableDiffCtrl)
    {
        HW_WR_FIELD32((baseAddr + ADC0_STEPCONFIG(stepNum - 1)),
                       ADC0_STEPCONFIG_DIFF_CNTRL,
                       ADC0_STEPCONFIG_DIFF_CNTRL_DIFFERENTIAL);
    }
    else
    {
        HW_WR_FIELD32((baseAddr + ADC0_STEPCONFIG(stepNum - 1U)),
                       ADC0_STEPCONFIG_DIFF_CNTRL,
                       ADC0_STEPCONFIG_DIFF_CNTRL_SINGLE);
    }

    regVal = HW_RD_REG32(baseAddr + ADC0_STEPCONFIG(stepNum - 1U));

    /* Configure the negative reference voltage. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_SEL_RFM_SWC,
                 pStepCfg->adcNegativeInpRef);

    /* Configure the positive reference voltage. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_SEL_RFP_SWC,
                 pStepCfg->adcPositiveInpRef);

    /* Configure the negative input channel. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_SEL_INM_SWC,
                 pStepCfg->adcNegativeInpChan);

    /* Configure the positive input channel. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_SEL_INP_SWC,
                 pStepCfg->adcPositiveInpChan);

    /* Control the XPPSW pin. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_XPPSW_SWC,
                 pStepCfg->enableXppsw);

    /* Control the XNPSW pin. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_XNPSW_SWC,
                 pStepCfg->enableXnpsw);

    /* Control the YPPSW pin. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_YPPSW_SWC,
                 pStepCfg->enableYppsw);

    /* Control the XNNSW pin. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_XNNSW_SWC,
                 pStepCfg->enableXnnsw);

    /* Control the YPNSW pin. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_YPNSW_SWC,
                 pStepCfg->enableYpnsw);

    /* Control the YNNSW pin. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_YNNSW_SWC,
                 pStepCfg->enableYnnsw);

    /* Control the WPNSW pin. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_WPNSW_SWC,
                 pStepCfg->enableWpnsw);

    /* Configure the Fifo. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_FIFO_SELECT,
    			 pStepCfg->fifo);

    /* Configure the step mode. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_MODE,
    			 pStepCfg->mode);

    /* Configure the number of samples to be averaged. */
    HW_SET_FIELD(regVal, ADC0_STEPCONFIG_AVERAGING,
    			 pStepCfg->averaging);

    /* Write variable value to register. */
    HW_WR_REG32((baseAddr + ADC0_STEPCONFIG(stepNum - 1U)), regVal);
}

void TSCADCFifoConfig(uint32_t baseAddr,
                          uint32_t fifoSel,
                          uint32_t sampleLvl,
                          uint32_t enableIrq)
{
    if (TRUE == enableIrq)
    {
        /* Enable IRQ request. */
        HW_WR_FIELD32((baseAddr + ADC0_FIFOTHR(fifoSel)),
                       ADC0_FIFOTHR_FIFO_THR_LEVEL,
                       (sampleLvl - 1));
    }
    else
    {
        /* Enable DMA request. */
        HW_WR_FIELD32((baseAddr + ADC0_DMAREQ(fifoSel)),
                       ADC0_DMAREQ_DMA_REQUEST_LEVEL,
                       (sampleLvl - 1));
    }
}

void TSCADCStepMode(uint32_t baseAddr, uint32_t stepNum, uint32_t stepMode)
{
    /* Configure the step mode. */
    HW_WR_FIELD32((baseAddr + ADC0_STEPCONFIG(stepNum - 1)),
                   ADC0_STEPCONFIG_MODE,
                   stepMode);
}

void TSCADCStepSamplesAvg(uint32_t baseAddr,
                          uint32_t stepNum,
                          uint32_t avgConfig)
{
    /* Configure the number of samples to be averaged. */
    HW_WR_FIELD32((baseAddr + ADC0_STEPCONFIG(stepNum - 1)),
                   ADC0_STEPCONFIG_AVERAGING,
                   avgConfig);
}

void TSCADCSetStepDelay(uint32_t baseAddr,
                           uint32_t stepNum,
                           uint32_t sampleDelay,
                           uint32_t openDelay)
{
    /* Configure the sample delay. */
    HW_WR_FIELD32((baseAddr + ADC0_STEPDELAY(stepNum - 1)),
                   ADC0_STEPDELAY_SAMPLEDELAY,
                   sampleDelay);

    /* Configure the open delay. */
    HW_WR_FIELD32((baseAddr + ADC0_STEPDELAY(stepNum - 1)),
                   ADC0_STEPDELAY_OPENDELAY,
                   openDelay);
}

void TSCADCStepEnable(uint32_t baseAddr, uint32_t stepNum, uint32_t enableStep)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + ADC0_STEPEN);

    if (TRUE == enableStep)
    {
        regVal |= (1 << stepNum);
    }
    else
    {
        regVal &= ~(1 << stepNum);
    }

    HW_WR_REG32((baseAddr + ADC0_STEPEN), regVal);
}

void TSCADCDmaFifoEnable(uint32_t baseAddr,
                         uint32_t fifoSel,
                         uint32_t enableDma)
{
    uint32_t regVal = 0U;

    if (TRUE == enableDma)
    {
        regVal = HW_RD_REG32(baseAddr + ADC0_DMAEN_SET);
        regVal |= 0x1 << fifoSel;
        HW_WR_REG32((baseAddr + ADC0_DMAEN_SET), regVal);
    }
    else
    {
        regVal = HW_RD_REG32(baseAddr + ADC0_DMAEN_CLR);
        regVal |= 0x1 << fifoSel;
        HW_WR_REG32((baseAddr + ADC0_DMAEN_CLR), regVal);
    }
}

void TSCADCAfePowerDown(uint32_t baseAddr)
{
    /* Power down the ADC AFE. */
    HW_WR_FIELD32((baseAddr + ADC0_CTRL), ADC0_CTRL_POWER_DOWN,
                   ADC0_CTRL_POWER_DOWN_AFEPOWERDOWN);
}

void TSCADCAfePowerUp(uint32_t baseAddr)
{
    /* Power up the ADC. */
    HW_WR_FIELD32((baseAddr + ADC0_CTRL), ADC0_CTRL_POWER_DOWN,
                   ADC0_CTRL_POWER_DOWN_AFEPOWERUP);
}

void TSCADCStepRangeCheckEnable(uint32_t baseAddr,
                                uint32_t stepNum,
                                uint32_t enableRangeChk)
{
    if (TRUE == enableRangeChk)
    {
        /* Enable out of range check feature. */
        HW_WR_FIELD32((baseAddr + ADC0_STEPCONFIG(stepNum - 1)),
                       ADC0_STEPCONFIG_RANGE_CHECK,
                       ADC0_STEPCONFIG_RANGE_CHECK_ENABLE);
    }
    else
    {
        /* Disable out of range check feature. */
        HW_WR_FIELD32((baseAddr + ADC0_STEPCONFIG(stepNum - 1)),
                       ADC0_STEPCONFIG_RANGE_CHECK,
                       ADC0_STEPCONFIG_RANGE_CHECK_DISABLE);
    }
}

uint32_t TSCADCGetFifoStepId(uint32_t baseAddr, uint32_t fifoSel)
{
    uint32_t fifoReg = 0;

    if (TSCADC_FIFO_SEL_0 == fifoSel)
    {
        fifoReg = ADC0_FIFO0DATA;
    }
    else
    {
        fifoReg = ADC0_FIFO1DATA;
    }

    return(HW_RD_FIELD32((baseAddr + fifoReg), ADC0_FIFO0DATA_ADCCHNLID));
}

uint32_t TSCADCGetFifoData(uint32_t baseAddr, uint32_t fifoSel)
{
    uint32_t fifoReg = 0;

    if (TSCADC_FIFO_SEL_0 == fifoSel)
    {
        fifoReg = ADC0_FIFO0DATA;
	}
    else
    {
        fifoReg = ADC0_FIFO1DATA;
    }

    return(HW_RD_FIELD32((baseAddr + fifoReg), ADC0_FIFO0DATA_ADCDATA));
}

uint32_t TSCADCGetFifoWordCount(uint32_t baseAddr, uint32_t fifoSel)
{
    /* Return the number of words present in FIFO. */
    return(HW_RD_FIELD32((baseAddr + ADC0_FIFOCOUNT(fifoSel - 1)),
           ADC0_FIFOCOUNT_WORDS_IN_FIFO));
}

void TSCADCIntrClear(uint32_t baseAddr, uint32_t intrFlags)
{
    /* Clear the interrupt status. */
    HW_WR_REG32((baseAddr + ADC0_IRQSTS), intrFlags);
}

uint32_t TSCADCIntrStatus(uint32_t baseAddr)
{
    /* Return the interrupt status. */
    return(HW_RD_REG32(baseAddr + ADC0_IRQSTS));
}

void TSCADCIntrEnable(uint32_t baseAddr, uint32_t intrFlags)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + ADC0_IRQEN_SET);

    /* Enable the interrupts. */
    regVal |= intrFlags;
    HW_WR_REG32((baseAddr + ADC0_IRQEN_SET), regVal);
}

void TSCADCIntrDisable(uint32_t baseAddr, uint32_t intrFlags)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + ADC0_IRQEN_CLR);

    /* Disable the interrupts. */
    regVal |= intrFlags;
    HW_WR_REG32((baseAddr + ADC0_IRQEN_CLR), regVal);
}

void TSCADCIntrTrigger(uint32_t baseAddr, uint32_t intrFlags)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + ADC0_IRQSTS_RAW);

    /* Set the raw interrupts. */
    regVal |= intrFlags;
    HW_WR_REG32((baseAddr + ADC0_IRQSTS_RAW), regVal);
}

uint32_t TSCADCIntrRawStatus(uint32_t baseAddr)
{
    return(HW_RD_REG32(baseAddr + ADC0_IRQSTS_RAW));
}

uint32_t TSCADCStatus(uint32_t baseAddr)
{
    /* Return the ADC status. */
    return(HW_RD_REG32(baseAddr + ADC0_ADCSTAT));
}

void TSCADCEnable(uint32_t baseAddr, uint32_t enableAdc)
{
    /* Enable/Disable the TSCADC. */
    if (TRUE == enableAdc)
    {
        HW_WR_FIELD32((baseAddr + ADC0_CTRL),
                       ADC0_CTRL_EN,
                       ADC0_CTRL_EN_ENABLE);
    }
    else
    {
        HW_WR_FIELD32((baseAddr + ADC0_CTRL),
                       ADC0_CTRL_EN,
                       ADC0_CTRL_EN_DISABLE);
    }
}

void TSCADCSetIdleMode(uint32_t baseAddr, uint32_t idleMode)
{
    /* Configure the Idle mode. */
    HW_WR_FIELD32((baseAddr + ADC0_SYSCONFIG),
                   ADC0_SYSCONFIG_IDLEMODE,
                   idleMode);
}

void TSCADCTsWakeupPenEventEnable(uint32_t baseAddr, uint32_t enableWakeup)
{
    if (TRUE == enableWakeup)
    {
        /* Enable wakeup. */
        HW_WR_FIELD32((baseAddr + ADC0_IRQWAKEUP),
                       ADC0_IRQWAKEUP_WAKEEN0,
                       ADC0_IRQWAKEUP_WAKEEN0_ENABLED);
    }
    else
    {
        /* Disable wakeup. */
        HW_WR_FIELD32((baseAddr + ADC0_IRQWAKEUP),
                       ADC0_IRQWAKEUP_WAKEEN0,
                       ADC0_IRQWAKEUP_WAKEEN0_DISABLED);
    }
}

void TSCADCHwPreemptEnable(uint32_t baseAddr, uint32_t enableHwPreempt)
{
    if (TRUE == enableHwPreempt)
    {
        /* Enable preemption. */
        HW_WR_FIELD32((baseAddr + ADC0_CTRL),
                       ADC0_CTRL_HW_PREEMPT,
                       ADC0_CTRL_HW_PREEMPT);
    }
    else
    {
        /* Disable preemption. */
        HW_WR_FIELD32((baseAddr + ADC0_CTRL),
                       ADC0_CTRL_HW_PREEMPT,
                       ADC0_CTRL_HW_PREEMPT_NOPREEMPT);
    }
}

void TSCADCSetBias(uint32_t baseAddr, uint32_t adcBiasSel)
{
    /* Configure the TSCADC Bias. */
    HW_WR_FIELD32((baseAddr + ADC0_CTRL),
                   ADC0_CTRL_ADC_BIAS_SELECT,
                   adcBiasSel);
}

void TSCADCSetRange(uint32_t baseAddr,
                    uint32_t lowRange,
                    uint32_t highRange)
{
    /* Configure the low range value. */
    HW_WR_FIELD32((baseAddr + ADC0_ADCRANGE),
                   ADC0_ADCRANGE_LOW_RANGE_DATA,
                   lowRange);

    /* Configure the high range value. */
    HW_WR_FIELD32((baseAddr + ADC0_ADCRANGE),
                   ADC0_ADCRANGE_HIGH_RANGE_DATA,
                   highRange);
}

void TSCADCMiscConfig(uint32_t baseAddr,
                      uint32_t spareInputVal,
                      uint32_t spareOutputVal)
{
    /* Configure the spare input value. */
    HW_WR_FIELD32((baseAddr + ADC0_ADC_MISC),
                   ADC0_ADC_MISC_AFE_SPARE_INPUT,
                   spareInputVal);

    /* Configure the spare output value. */
    HW_WR_FIELD32((baseAddr + ADC0_ADC_MISC),
                   ADC0_ADC_MISC_AFE_SPARE_OUTPUT,
                   spareOutputVal);
}

/* ========================================================================== */
/*                          Deprecated Function Definitions                   */
/* ========================================================================== */

uint32_t TSCADCIsDMAFIFOEnabled(uint32_t baseAddr, uint32_t fifoSel)
{
    return((HW_RD_REG32(baseAddr + ADC0_DMAEN_SET) >>
                        fifoSel) & 0x1U);
}
