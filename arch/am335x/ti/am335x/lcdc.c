/**
 *  \file  lcdc.c
 *
 *  \brief This file contains the device abstraction layer APIs for
 *         configuration of lcd controller.
 *
 *  \copyright Copyright (C) 2013 Texas Instruments Incorporated -
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

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "types.h"
#include "hw_types.h"
#include "lcdc.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Mask for extracting the LSB of Pixel per panel value. */
#define LCDC_RASTER_PPL_LSB_MASK      (0x000003f0U)

/** \brief Mask for extracting the MSB of Pixel per panel value. */
#define LCDC_RASTER_PPL_MSB_MASK      (0x00000400U)

/** \brief Shift value of Pixel per panel MSB value. */
#define LCDC_RASTER_PPL_MSB_SHIFT     (7U)

/** \brief Mask for extracting the LSB of Lines per panel value. */
#define LCDC_RASTER_LPP_LSB_MASK      (0x3ffU)

/** \brief Mask for extracting the MSB of Lines per panel value. */
#define LCDC_RASTER_LPP_MSB_MASK      (0x400U)

/** \brief Shift value of Lines per panel MSB value. */
#define LCDC_RASTER_LPP_MSB_SHIFT     (10U)

/** \brief Mask for extracting the LSB of Sub panel Default pixel data value. */
#define LCDC_RASTER_SP_DPD_LSB_MASK   (0xffffU)

/** \brief Mask for extracting the MSB of Sub panel Default pixel data value. */
#define LCDC_RASTER_SP_DPD_MSB_MASK   (0xff0000U)

/** \brief Shift value of MSB of Sub panel Default pixel data value. */
#define LCDC_RASTER_SP_DPD_MSB_SHIFT  (15U)

/** \brief Mask for extracting the LSB of Sub panel LPP threshold value. */
#define LCDC_RASTER_SP_LPTT_LSB_MASK  (0x3ffU)

/** \brief Mask for extracting the MSB of Sub panel LPP threshold value. */
#define LCDC_RASTER_SP_LPTT_MSB_MASK  (0x400U)

/** \brief Shift value of MSB of Sub panel LPP threshold value. */
#define LCDC_RASTER_SP_LPTT_MSB_SHIFT (11U)

/** \brief Macro to enable byte swap feature. */
#define LCDC_LCDDMA_CTRL_BYTE_SWAP_ENABLE   (0x1U)

/** \brief Macro to disable byte swap feature. */
#define LCDC_LCDDMA_CTRL_BYTE_SWAP_DISABLE  (0x0U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

uint32_t LCDCGetRevision(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + LCDC_PID));
}


void LCDCClkConfig(uint32_t baseAddr, uint32_t pixelClk, uint32_t moduleClk)
{
    uint32_t clkDiv = 0U;
    uint32_t regVal = 0U;

    clkDiv = moduleClk / pixelClk ;

    /*Select the mode as raster and clock divider value */
    regVal = HW_RD_REG32(baseAddr + LCDC_LCD_CTRL);
    HW_SET_FIELD(regVal, LCDC_LCD_CTRL_MODESEL, LCDC_LCD_CTRL_MODESEL_RASTER);
    HW_SET_FIELD(regVal, LCDC_LCD_CTRL_CLKDIV, clkDiv);
    HW_WR_REG32((baseAddr + LCDC_LCD_CTRL), regVal);
}


void LCDCDmaConfig(uint32_t baseAddr, lcdcDmaCfg_t *pCfg)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + LCDC_LCDDMA_CTRL);

    /* Set frame mode */
    HW_SET_FIELD(regVal, LCDC_LCDDMA_CTRL_FRAME_MODE, pCfg->frameMode);
    /* Set DMA burst size*/
    HW_SET_FIELD(regVal, LCDC_LCDDMA_CTRL_BURST_SIZE, pCfg->burstSize);
    /* Set DMA FIFO threshold */
    HW_SET_FIELD(regVal, LCDC_LCDDMA_CTRL_TH_FIFO_READY, pCfg->fifoThreshold);
    /* Set endian */
    HW_SET_FIELD(regVal, LCDC_LCDDMA_CTRL_BIGENDIAN, pCfg->endian);

    HW_WR_REG32((baseAddr + LCDC_LCDDMA_CTRL), regVal);
}


void LCDCRasterEnable(uint32_t baseAddr, uint32_t enableRaster)
{
    uint32_t enableFlag = 0U;

    enableFlag = (TRUE == enableRaster) ?
        LCDC_RASTER_CTRL_RASTER_EN_ENABLED :
        LCDC_RASTER_CTRL_RASTER_EN_DISABLED;
    HW_WR_FIELD32((baseAddr + LCDC_RASTER_CTRL), LCDC_RASTER_CTRL_RASTER_EN,
        enableFlag);
}


void LCDCRasterDisplayModeConfig(uint32_t baseAddr,
                                 lcdcRasterDisplayAttrCfg_t *pCfg)
{
    uint32_t regVal = 0U;
    uint32_t enableFlag = 0U;

    regVal = HW_RD_REG32(baseAddr + LCDC_RASTER_CTRL);

    /* Set display type, palette load mode */
    HW_SET_FIELD(regVal, LCDC_RASTER_CTRL_TFT_STN, pCfg->displayMode);
    HW_SET_FIELD(regVal, LCDC_RASTER_CTRL_PLM, pCfg->paletteMode);

    /* Set pixel format type */
    if (LCDC_PIXEL_FORMAT_RGB24_UNPACKED == pCfg->pixelFormat)
    {
        HW_SET_FIELD(regVal, LCDC_RASTER_CTRL_TFT24, LCDC_RASTER_CTRL_TFT24_ON);
        HW_SET_FIELD(regVal, LCDC_RASTER_CTRL_TFT24UNPACKED,
            LCDC_RASTER_CTRL_TFT24UNPACKED_UNPACKED);
    }
    else if (LCDC_PIXEL_FORMAT_RGB24_PACKED == pCfg->pixelFormat)
    {
        HW_SET_FIELD(regVal, LCDC_RASTER_CTRL_TFT24, LCDC_RASTER_CTRL_TFT24_ON);
        HW_SET_FIELD(regVal, LCDC_RASTER_CTRL_TFT24UNPACKED,
            LCDC_RASTER_CTRL_TFT24UNPACKED_PACKED);
    }
    else
    {
        /* No RGB 24 bit format. Format can be 1,2,4,8,12,16 BPP. */
        HW_SET_FIELD(regVal, LCDC_RASTER_CTRL_TFT24,
            LCDC_RASTER_CTRL_TFT24_OFF);
    }

    /* Extrapolate processing. */
    if ((LCDC_RASTER_MATRIX_MODE_ACTIVE == pCfg->displayMode) &&
        ((LCDC_PIXEL_FORMAT_1BPP == pCfg->pixelFormat) ||
        (LCDC_PIXEL_FORMAT_2BPP == pCfg->pixelFormat) ||
        (LCDC_PIXEL_FORMAT_4BPP == pCfg->pixelFormat) ||
        (LCDC_PIXEL_FORMAT_8BPP == pCfg->pixelFormat)))
    {
        enableFlag = (TRUE == pCfg->enableExtrapolate) ?
            LCDC_RASTER_CTRL_TFT_ALT_MAP_565 :
            LCDC_RASTER_CTRL_TFT_ALT_MAP_ALIGN;
        HW_SET_FIELD(regVal, LCDC_RASTER_CTRL_TFT_ALT_MAP, enableFlag);
    }

    if (LCDC_RASTER_MATRIX_MODE_PASSIVE == pCfg->displayMode)
    {
        /* Set color or monochromatic type */
        HW_SET_FIELD(regVal, LCDC_RASTER_CTRL_MONO_COLOR, pCfg->displayType);

        if (LCDC_RASTER_DISPLAY_TYPE_MONO == pCfg->displayType)
        {
            /* Set bit width for monochromatic data. */
            HW_SET_FIELD(regVal, LCDC_RASTER_CTRL_MONO8B, pCfg->mono8Bit);
        }
    }

    HW_WR_REG32((baseAddr + LCDC_RASTER_CTRL), regVal);
}


void LCDCRasterTimingConfig(uint32_t baseAddr, lcdcRasterTimingCfg_t *pCfg)
{
    uint32_t ppl = 0U;
    uint32_t regVal = 0U;
    uint32_t lppLsb = 0U;
    uint32_t lppMsb = 0U;

    /* Configure number of pixels per line */
    ppl = pCfg->ppl - 1U;
    ppl = (ppl & LCDC_RASTER_PPL_LSB_MASK) |
        ((ppl & LCDC_RASTER_PPL_MSB_MASK) >> LCDC_RASTER_PPL_MSB_SHIFT);
    HW_WR_REG32((baseAddr + LCDC_RASTER_TIMING_0), ppl);

    /* Set hfp, hbp and hsw */
    regVal = HW_RD_REG32(baseAddr + LCDC_RASTER_TIMING_0);
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_0_HSW, (pCfg->hsw - 1U));
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_0_HFP, (pCfg->hfp - 1U));
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_0_HBP, (pCfg->hbp - 1U));
    HW_WR_REG32((baseAddr + LCDC_RASTER_TIMING_0), regVal);

    lppLsb = ((pCfg->lpp - 1U) & LCDC_RASTER_LPP_LSB_MASK);
    lppMsb = (((pCfg->lpp - 1U) & LCDC_RASTER_LPP_MSB_MASK) >>
        LCDC_RASTER_LPP_MSB_SHIFT);

    /* Configure Lines per Panel */
    HW_WR_REG32((baseAddr + LCDC_RASTER_TIMING_1), lppLsb);
    HW_WR_FIELD32((baseAddr + LCDC_RASTER_TIMING_2),
        LCDC_RASTER_TIMING_2_LPP_B10, lppMsb);

    /* Set vfp, vbp and vsw */
    regVal = HW_RD_REG32(baseAddr + LCDC_RASTER_TIMING_1);
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_1_VSW, (pCfg->vsw - 1U));
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_1_VFP, pCfg->vfp);
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_1_VBP, pCfg->vbp);
    HW_WR_REG32((baseAddr + LCDC_RASTER_TIMING_1), regVal);
}


void LCDCRasterPolarityConfig(uint32_t baseAddr, lcdcRasterPolarityCfg_t *pCfg)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + LCDC_RASTER_TIMING_2);

    /* Set hSync, vSync, pixel clock and output enable signal polarities. */
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_2_IHS, pCfg->hSyncPolarity);
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_2_IVS, pCfg->vSyncPolarity);
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_2_IEO, pCfg->outputEnablePolarity);
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_2_IPC, pCfg->pixelClkPolarity);

    /* Set sync edge and sync control */
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_2_SYNC_EDGE, pCfg->syncEdge);
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_2_SYNC_CTRL, pCfg->sycnEdgeCtrl);

    HW_WR_REG32((baseAddr + LCDC_RASTER_TIMING_2), regVal);
}


void LCDCRasterAcbiasConfig(uint32_t baseAddr, lcdcRasterAcbCfg_t *pCfg)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + LCDC_RASTER_TIMING_2);

    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_2_ACB, pCfg->acbFreq);
    HW_SET_FIELD(regVal, LCDC_RASTER_TIMING_2_ACB_I, pCfg->acbi);
    HW_WR_REG32((baseAddr + LCDC_RASTER_TIMING_2), regVal);
}


void LCDCRasterSetDmaFifoDelay(uint32_t baseAddr, uint32_t delay)
{
    HW_WR_FIELD32((baseAddr + LCDC_RASTER_CTRL),
        LCDC_RASTER_CTRL_FIFO_DMA_DELAY, delay);
}


void LCDCDmaFrameBufConfig(uint32_t baseAddr,
                           uint32_t bufAddr,
                           uint32_t ceiling,
                           uint32_t frmBufId)
{
    if (LCDC_FRM_BUF_ID_0 == frmBufId)
    {
        HW_WR_REG32((baseAddr + LCDC_LCDDMA_FB0_BASE), bufAddr);
        HW_WR_REG32((baseAddr + LCDC_LCDDMA_FB0_CEILING), ceiling);
    }
    else if (LCDC_FRM_BUF_ID_1 == frmBufId)
    {
        HW_WR_REG32((baseAddr + LCDC_LCDDMA_FB1_BASE), bufAddr);
        HW_WR_REG32((baseAddr + LCDC_LCDDMA_FB1_CEILING), ceiling);
    }
    else
    {
        /*
         * This condition will not occur. Application is expected to pass in
         * correct parameters.
         */
    }
}


void LCDCIntrEnable(uint32_t baseAddr, uint32_t intrMask)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + LCDC_IRQENABLE_SET);
    regVal |= intrMask;
    HW_WR_REG32((baseAddr + LCDC_IRQENABLE_SET), regVal);
}


void LCDCIntrDisable(uint32_t baseAddr, uint32_t intrMask)
{
    HW_WR_REG32((baseAddr + LCDC_IRQENABLE_CLEAR), intrMask);
}


uint32_t LCDCIntrStatus(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + LCDC_IRQSTATUS));
}


uint32_t LCDCIntrRawStatus(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + LCDC_IRQSTATUS_RAW));
}


void LCDCIntrClear(uint32_t baseAddr, uint32_t intrMask)
{
    HW_WR_REG32((baseAddr + LCDC_IRQSTATUS), intrMask);
}


void LCDCIntrTrigger(uint32_t baseAddr, uint32_t intrMask)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + LCDC_IRQSTATUS_RAW);
    regVal |= intrMask;
    HW_WR_REG32((baseAddr + LCDC_IRQSTATUS_RAW), regVal);
}


void LCDCRasterSubPanelEnable(uint32_t baseAddr, uint32_t enableSubPanel)
{
    uint32_t enableFlag = 0U;

    enableFlag = (TRUE == enableSubPanel) ?
        LCDC_RASTER_SUBPANEL_SPEN_ENABLED :
        LCDC_RASTER_SUBPANEL_SPEN_DISABLED;
    HW_WR_FIELD32((baseAddr + LCDC_RASTER_SUBPANEL), LCDC_RASTER_SUBPANEL_SPEN,
        enableFlag);
}


void LCDCRasterSubPanelConfig(uint32_t baseAddr,
                              uint32_t subPanelPos,
                              uint32_t linesPerPanelThr,
                              uint32_t defaultPixelData)
{
    uint32_t regVal = 0U;
    uint32_t defaultPixelDataLsb =
        (defaultPixelData & LCDC_RASTER_SP_DPD_LSB_MASK);
    uint32_t defaultPixelDataMsb =
        ((defaultPixelData & LCDC_RASTER_SP_DPD_MSB_MASK) >>
        LCDC_RASTER_SP_DPD_MSB_SHIFT);
    uint32_t linesPerPanelThrLsb =
        (linesPerPanelThr & LCDC_RASTER_SP_LPTT_LSB_MASK);
    uint32_t linesPerPanelThrMsb =
        ((linesPerPanelThr & LCDC_RASTER_SP_LPTT_MSB_MASK) >>
        LCDC_RASTER_SP_LPTT_MSB_SHIFT);

    regVal = HW_RD_REG32(baseAddr + LCDC_RASTER_SUBPANEL);
    HW_SET_FIELD(regVal, LCDC_RASTER_SUBPANEL_DPD, defaultPixelDataLsb);
    HW_SET_FIELD(regVal, LCDC_RASTER_SUBPANEL_LPPT, linesPerPanelThrLsb);
    HW_SET_FIELD(regVal, LCDC_RASTER_SUBPANEL_HOLS, subPanelPos);
    HW_WR_REG32((baseAddr + LCDC_RASTER_SUBPANEL), regVal);

    regVal = HW_RD_REG32(LCDC_RASTER_SUBPANEL2);
    HW_SET_FIELD(regVal, LCDC_RASTER_SUBPANEL2_DPDMSB, defaultPixelDataMsb);
    HW_SET_FIELD(regVal, LCDC_RASTER_SUBPANEL2_LPPT_B10, linesPerPanelThrMsb);
    HW_WR_REG32((LCDC_RASTER_SUBPANEL2), regVal);
}


void LCDCClocksEnable(uint32_t baseAddr)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + LCDC_CLKC_ENABLE);

    /* Enable core, dma and lidd sub module clocks. */
    HW_SET_FIELD(regVal, LCDC_CLKC_ENABLE_CORE, LCDC_CLKC_ENABLE_CORE_ENABLE);
    HW_SET_FIELD(regVal, LCDC_CLKC_ENABLE_DMA, LCDC_CLKC_ENABLE_DMA_ENABLE);
    HW_SET_FIELD(regVal, LCDC_CLKC_ENABLE_LIDD, LCDC_CLKC_ENABLE_LIDD_ENABLE);

    HW_WR_REG32((baseAddr + LCDC_CLKC_ENABLE), regVal);
}


void LCDCRasterClkEnable(uint32_t baseAddr, uint32_t enableRasterClk)
{
    uint32_t enableFlag = 0U;

    enableFlag = (TRUE == enableRasterClk) ?
        LCDC_CLKC_ENABLE_CORE_ENABLE : LCDC_CLKC_ENABLE_CORE_DISABLE;
    HW_WR_FIELD32((baseAddr + LCDC_CLKC_ENABLE), LCDC_CLKC_ENABLE_CORE,
        enableFlag);
}


void LCDCDmaClkEnable(uint32_t baseAddr, uint32_t enableDmaClk)
{
    uint32_t enableFlag = 0U;

    enableFlag = (TRUE == enableDmaClk) ?
        LCDC_CLKC_ENABLE_DMA_ENABLE : LCDC_CLKC_ENABLE_DMA_DISABLE;
    HW_WR_FIELD32((baseAddr + LCDC_CLKC_ENABLE), LCDC_CLKC_ENABLE_DMA,
        enableFlag);
}


void LCDCSoftwareResetEnable(uint32_t baseAddr,
                             uint32_t moduleId,
                             uint32_t enableSoftwareReset)
{
    uint32_t enableFlag = 0U;

    if (LCDC_SUB_MODULE_ID_CORE == moduleId)
    {
        enableFlag = (TRUE == enableSoftwareReset) ?
            LCDC_CLKC_RESET_CORE_ENABLE :
            LCDC_CLKC_RESET_CORE_DISABLE;

        HW_WR_FIELD32((baseAddr + LCDC_CLKC_RESET), LCDC_CLKC_RESET_CORE,
            enableFlag);
    }
    else if (LCDC_SUB_MODULE_ID_DMA == moduleId)
    {
        enableFlag = (TRUE == enableSoftwareReset) ?
            LCDC_CLKC_RESET_DMA_ENABLE :
            LCDC_CLKC_RESET_DMA_DISABLE;

        HW_WR_FIELD32((baseAddr + LCDC_CLKC_RESET), LCDC_CLKC_RESET_DMA,
            enableFlag);
    }
    else if (LCDC_SUB_MODULE_ID_LCD == moduleId)
    {
        enableFlag = (TRUE == enableSoftwareReset) ?
            LCDC_CLKC_RESET_MAIN_ENABLE :
            LCDC_CLKC_RESET_MAIN_DISABLE;

        HW_WR_FIELD32((baseAddr + LCDC_CLKC_RESET), LCDC_CLKC_RESET_MAIN,
            enableFlag);
    }
    else
    {
        /*
         * This condition will not occur. Application is expected to pass in
         * correct parameters.
         */
    }
}


void LCDCSetDmaMasterPriority(uint32_t baseAddr, uint32_t priority)
{
    HW_WR_FIELD32((baseAddr + LCDC_LCDDMA_CTRL),
        LCDC_LCDDMA_CTRL_DMA_MASTER_PRIO, priority);
}


void LCDCSetStandbyMode(uint32_t baseAddr, uint32_t standbyMode)
{
    HW_WR_FIELD32((baseAddr + LCDC_SYSCONFIG),
        LCDC_SYSCONFIG_STANDBYMODE, standbyMode);
}


void LCDCSetIdleMode(uint32_t baseAddr, uint32_t idleMode)
{
    HW_WR_FIELD32((baseAddr + LCDC_SYSCONFIG),
        LCDC_SYSCONFIG_IDLEMODE, idleMode);
}


void LCDCAutoUnderFlowEnable(uint32_t baseAddr, uint32_t enableAutoUnderFlow)
{
    uint32_t enableFlag = 0U;

    enableFlag = (TRUE == enableAutoUnderFlow) ?
        LCDC_LCD_CTRL_AUTO_UFLOW_RESTART_AUTO :
        LCDC_LCD_CTRL_AUTO_UFLOW_RESTART_MANUAL;
    HW_WR_FIELD32((baseAddr + LCDC_LCD_CTRL), LCDC_LCD_CTRL_AUTO_UFLOW_RESTART,
        enableFlag);
}


void LCDCByteSwapEnable(uint32_t baseAddr, uint32_t enableByteSwap)
{
    uint32_t enableFlag = 0U;

    enableFlag = (TRUE == enableByteSwap) ?
        LCDC_LCDDMA_CTRL_BYTE_SWAP_ENABLE :
        LCDC_LCDDMA_CTRL_BYTE_SWAP_DISABLE;
    HW_WR_FIELD32((baseAddr + LCDC_LCDDMA_CTRL), LCDC_LCDDMA_CTRL_BYTE_SWAP,
        enableFlag);
}


void LCDCRasterDataOrderSelect(uint32_t baseAddr, uint32_t dataOrder)
{
    HW_WR_FIELD32((baseAddr + LCDC_RASTER_CTRL), LCDC_RASTER_CTRL_RD_ORDER,
        dataOrder);
}


void LCDCRasterNibbleModeEnable(uint32_t baseAddr, uint32_t enableNibbleMode)
{
    uint32_t enableFlag = 0U;

    enableFlag = (TRUE == enableNibbleMode) ?
        LCDC_RASTER_CTRL_NIB_MODE_ENABLED :
        LCDC_RASTER_CTRL_NIB_MODE_DISABLED;
    HW_WR_FIELD32((baseAddr + LCDC_RASTER_CTRL), LCDC_RASTER_CTRL_NIB_MODE,
        enableFlag);
}


void LCDCContextSave(uint32_t baseAddr, lcdcContext_t *pCtx)
{
     pCtx->clkcEnable = HW_RD_REG32(baseAddr + LCDC_CLKC_ENABLE);
     pCtx->lcdCtrl = HW_RD_REG32(baseAddr + LCDC_LCD_CTRL);
     pCtx->lcddmaCtrl = HW_RD_REG32(baseAddr + LCDC_LCDDMA_CTRL);
     pCtx->rasterTiming2 = HW_RD_REG32(baseAddr + LCDC_RASTER_TIMING_2);
     pCtx->rasterTiming0 = HW_RD_REG32(baseAddr + LCDC_RASTER_TIMING_0);
     pCtx->rasterTiming1 = HW_RD_REG32(baseAddr + LCDC_RASTER_TIMING_1);
     pCtx->rasterCtrl = HW_RD_REG32(baseAddr + LCDC_RASTER_CTRL);
     pCtx->irqEnableSet = HW_RD_REG32(baseAddr + LCDC_IRQENABLE_SET);
     pCtx->lcddmaFb0Base = HW_RD_REG32(baseAddr + LCDC_LCDDMA_FB0_BASE);
     pCtx->lcddmaFb0Ceiling = HW_RD_REG32(baseAddr + LCDC_LCDDMA_FB0_CEILING);
     pCtx->lcddmaFb1Base = HW_RD_REG32(baseAddr + LCDC_LCDDMA_FB1_BASE);
     pCtx->lcddmaFb1Ceiling = HW_RD_REG32(baseAddr + LCDC_LCDDMA_FB1_CEILING);
}


void RasterContextRestore(uint32_t baseAddr, lcdcContext_t *pCtx)
{
    HW_WR_REG32((baseAddr + LCDC_CLKC_ENABLE), pCtx->clkcEnable);
    HW_WR_REG32((baseAddr + LCDC_LCD_CTRL),  pCtx->lcdCtrl);
    HW_WR_REG32((baseAddr + LCDC_LCDDMA_CTRL), pCtx->lcddmaCtrl);
    HW_WR_REG32((baseAddr + LCDC_RASTER_TIMING_2), pCtx->rasterTiming2);
    HW_WR_REG32((baseAddr + LCDC_RASTER_TIMING_0), pCtx->rasterTiming0);
    HW_WR_REG32((baseAddr + LCDC_RASTER_TIMING_1), pCtx->rasterTiming1);
    HW_WR_REG32((baseAddr + LCDC_RASTER_CTRL), pCtx->rasterCtrl);
    HW_WR_REG32((baseAddr + LCDC_LCDDMA_FB0_BASE), pCtx->lcddmaFb0Base);
    HW_WR_REG32((baseAddr + LCDC_LCDDMA_FB0_CEILING), pCtx->lcddmaFb0Ceiling);
    HW_WR_REG32((baseAddr + LCDC_LCDDMA_FB1_BASE), pCtx->lcddmaFb1Base);
    HW_WR_REG32((baseAddr + LCDC_LCDDMA_FB1_CEILING), pCtx->lcddmaFb1Ceiling);
    HW_WR_REG32((baseAddr + LCDC_IRQENABLE_SET),  pCtx->irqEnableSet);
}


/* ========================================================================== */
/*                    Deprecated Function Definitions                         */
/* ========================================================================== */

void RasterEndOfFrameIntEnable(uint32_t baseAddr)
{
    HW_WR_FIELD32((baseAddr + LCDC_LCDDMA_CTRL), LCDC_LCDDMA_CTRL_EOF_INTEN,
        LCDC_LCDDMA_CTRL_EOF_INTEN_MASK);
}


void RasterEndOfFrameIntDisable(uint32_t baseAddr)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + LCDC_LCDDMA_CTRL);
    regVal &= ~LCDC_LCDDMA_CTRL_EOF_INTEN_MASK;
    HW_WR_REG32((baseAddr + LCDC_LCDDMA_CTRL), regVal);
}
