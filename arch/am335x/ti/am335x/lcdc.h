/**
 *  \file    lcdc.h
 *
 *  \brief   This file contains the function prototypes for the device
 *           abstraction layer for LCD controller. It also contains related
 *           macro and structure definitions.
 *
 *  \details Programming sequence of LCDC:
 *           -# Perform clock and pin mux configurations of LCDC.
 *           -# Enable all the clocks of the LCDC using the API
 *              #LCDCClocksEnable.
 *           -# Disable Raster using the API #LCDCRasterEnable (Second
 *              parameter is FALSE).
 *           -# Set up LCD parameters:
 *             -# Configure the pixel clock using the API #LCDCClkConfig.
 *             -# Configure the LCDC DMA parameters using the API #LCDCDmaConfig
 *             -# Configure the Raster display attributes using the API
 *                #LCDCRasterDisplayModeConfig.
 *             -# Configure the LCD timing parameters using the API
 *                #LCDCRasterTimingConfig.
 *             -# Configure the LCD signal polarities using the API
 *                #LCDCRasterPolarityConfig.
 *             -# Configure the ACB signals using the API
 *                #LCDCRasterAcbiasConfig.
 *             -# Configure the DMA fifo delay using the API
 *                #LCDCRasterSetDmaFifoDelay.
 *           -# Configure the DMA frame buffers using the API
 *              #LCDCDmaFrameBufConfig.
 *           -# Interrupt configuration
 *              -# Configure the LCDC interrupts that need to be enabled using
 *                 the API #LCDCIntrEnable.
 *           -# After all the configurations are done enable raster using the
 *              API #LCDCRasterEnable (Second parameter is TRUE).
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

#ifndef LCDC_H_
#define LCDC_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "misc.h"
#include "hw_lcdc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Enumerates the supported raster display modes. */
typedef enum lcdcRasterMatrixMode
{
    LCDC_RASTER_MATRIX_MODE_PASSIVE = LCDC_RASTER_CTRL_TFT_STN_STN,
    /**< Passive matrix display. */
    LCDC_RASTER_MATRIX_MODE_ACTIVE  = LCDC_RASTER_CTRL_TFT_STN_TFT
    /**< Active matrix display. */
}lcdcRasterMatrixMode_t;


/** \brief Enumerates the supported raster display types. */
typedef enum lcdcRasterDisplayType
{
    LCDC_RASTER_DISPLAY_TYPE_COLOR = LCDC_RASTER_CTRL_MONO_COLOR_COLOR,
    /**< Color display. */
    LCDC_RASTER_DISPLAY_TYPE_MONO  = LCDC_RASTER_CTRL_MONO_COLOR_MONO
    /**< monochromatic display. */
}lcdcRasterDisplayType_t;


/** \brief Enumerates the supported raster pixel formats. */
typedef enum lcdcPixelFormat
{
    LCDC_PIXEL_FORMAT_MIN = 0U,
    /**< Minimum value of enumeration. Used for input validation. */
    LCDC_PIXEL_FORMAT_1BPP = LCDC_PIXEL_FORMAT_MIN,
    /**< 1 bit per pixel. */
    LCDC_PIXEL_FORMAT_2BPP = 1U,
    /**< 2 bits per pixel. */
    LCDC_PIXEL_FORMAT_4BPP = 2U,
    /**< 4 bits per pixel. */
    LCDC_PIXEL_FORMAT_8BPP = 3U,
    /**< 8 bits per pixel. */
    LCDC_PIXEL_FORMAT_RGB12 = 4U,
    /**< 12 bits per pixel (R4 G4 B4). */
    LCDC_PIXEL_FORMAT_RGB16 = 5U,
    /**< 16 bits per pixel. RGB 16 format: R5 G6 B5. */
    LCDC_PIXEL_FORMAT_RGB24_UNPACKED = 6U,
    /**< RGB 24 bit(R8 G8 B8) data unpacked in 32 bits. */
    LCDC_PIXEL_FORMAT_RGB24_PACKED = 7U,
    /**< RGB 24 bit(R8 G8 B8) data packed in 24 bits. */
    LCDC_PIXEL_FORMAT_MAX = LCDC_PIXEL_FORMAT_RGB24_PACKED
    /**< Maximum value of enumeration. Used for input validation*/
}lcdcPixelFormat_t;


/** \brief Enumerates the supported load modes of palette. */
typedef enum lcdcPaletteMode
{
    LCDC_PALETTE_MODE_DATA = LCDC_RASTER_CTRL_PLM_DATA,
    /**< Only Data will be loaded. */
    LCDC_PALETTE_MODE_PALETTE = LCDC_RASTER_CTRL_PLM_PALETTE,
    /**< Only palette will be loaded. */
    LCDC_PALETTE_MODE_BOTH_PALETTE_DATA = LCDC_RASTER_CTRL_PLM_PALETTE_DATA
    /**< Both palette and data will be loaded. */
}lcdcPaletteMode_t;


/** \brief Enumerates the supported mono display widths. */
typedef enum lcdcMonoType
{
    LCDC_MONO_TYPE_4_BIT = LCDC_RASTER_CTRL_MONO8B_4PIXEL,
    /**< 4 bit monochromatic display. */
    LCDC_MONO_TYPE_8_BIT = LCDC_RASTER_CTRL_MONO8B_8PIXEL,
    /**< 4 bit monochromatic display. */
}lcdcMonoType_t;


/** \brief Enumerates the supported raster data order select modes. */
typedef enum lcdcRasterDataOrder
{
    LCDC_RASTER_DATA_ORDER_LSB = LCDC_RASTER_CTRL_RD_ORDER_L2H,
    /**< The frame buffer parsing for Palette Data lookup is from Bit 0 to
         bit 31 of the input word from the DMA output. */
    LCDC_RASTER_DATA_ORDER_MSB = LCDC_RASTER_CTRL_RD_ORDER_H2L,
    /**< The fame buffer parsing for Palette Data lookup is from Bit 31 to
         Bit 0 of the input word from the DMA output. */
}lcdcRasterDataOrder_t;


/** \brief Enumerates the supported LCDC interrupts. */
typedef enum lcdcIntrMask
{
    LCDC_INTR_MASK_FRM_DONE = LCDC_IRQENABLE_SET_DONE_MASK,
    /**< Frame done interrupt. */
    LCDC_INTR_MASK_RASTER_DONE = LCDC_IRQENABLE_SET_RECURRENT_RASTER_MASK,
    /**< Raster done interrupt. */
    LCDC_INTR_MASK_SYNC_LOST = LCDC_IRQENABLE_SET_SYNC_MASK,
    /**< Frame sync lost interrupt. */
    LCDC_INTR_MASK_ACB_COUNT = LCDC_IRQENABLE_SET_ACB_MASK,
    /**< Ac bias count interrupt. */
    LCDC_INTR_MASK_FIFO_UNDERFLOW = LCDC_IRQENABLE_SET_FUF_MASK,
    /**< DMA fifo underflow interrupt. */
    LCDC_INTR_MASK_PALETTE_LOADED = LCDC_IRQENABLE_SET_PL_MASK,
    /**< Palette loaded interrupt. */
    LCDC_INTR_MASK_END_OF_FRAME_0 = LCDC_IRQENABLE_SET_EOF0_MASK,
    /**< DMA end of frame 0 interrupt. */
    LCDC_INTR_MASK_END_OF_FRAME_1 = LCDC_IRQENABLE_SET_EOF1_MASK,
    /**< DMA end of frame 1 interrupt. */
    LCDC_INTR_MASK_ALL = (LCDC_INTR_MASK_FRM_DONE | LCDC_INTR_MASK_RASTER_DONE |
        LCDC_INTR_MASK_SYNC_LOST | LCDC_INTR_MASK_ACB_COUNT |
        LCDC_INTR_MASK_FIFO_UNDERFLOW | LCDC_INTR_MASK_PALETTE_LOADED |
        LCDC_INTR_MASK_END_OF_FRAME_0 | LCDC_INTR_MASK_END_OF_FRAME_1)
    /**< Interrupt mask of all the LCDC interrupts. */
}lcdcIntrMask_t;


/** \brief  Enumerates polarities of horizontal synchronization signal. */
typedef enum lcdcRasterHSyncPol
{
    LCDC_RASTER_H_SYNC_POL_HIGH = LCDC_RASTER_TIMING_2_IHS_ACTIVE_HIGH,
    /**< Polarity is active high. */
    LCDC_RASTER_H_SYNC_POL_LOW = LCDC_RASTER_TIMING_2_IHS_ACTIVE_LOW
    /**< Polarity is active low. */
}lcdcRasterHSyncPol_t;


/** \brief  Enumerates polarities of vertical synchronization signal. */
typedef enum lcdcRasterVSyncPol
{
    LCDC_RASTER_V_SYNC_POL_HIGH = LCDC_RASTER_TIMING_2_IVS_ACTIVE_HIGH,
    /**< Polarity is active high. */
    LCDC_RASTER_V_SYNC_POL_LOW = LCDC_RASTER_TIMING_2_IVS_ACTIVE_LOW
    /**< Polarity is active low. */
}lcdcRasterVSyncPol_t;


/** \brief Enumerates polarities of the Output enable signal. */
typedef enum lcdcRasterOutputEnablePol
{
    LCDC_RASTER_OUTPUT_ENABLE_POL_HIGH = LCDC_RASTER_TIMING_2_IEO_ACTIVE_HIGH,
    /**< Polarity is active high. */
    LCDC_RASTER_OUTPUT_ENABLE_POL_LOW  = LCDC_RASTER_TIMING_2_IEO_ACTIVE_LOW,
    /**< Polarity is active low. */
}lcdcRasterOutputEnablePol_t;


/** \brief Enumerates active polarities of the pixel clock. */
typedef enum lcdcRasterPclkPol
{
    LCDC_RASTER_PCLK_POL_RISING_EDGE = LCDC_RASTER_TIMING_2_IPC_RISING,
    /**< Data is driven on the LCD data lines on the rising-edge of the
         pixel clock. */
    LCDC_RASTER_PCLK_POL_FALLING_EDGE = LCDC_RASTER_TIMING_2_IPC_FALLING,
    /**< Data is driven on the LCD data lines on the falling-edge of the
         pixel clock. */
}lcdcRasterPclkPol_t;


/** \brief Enumerates active edges of H or v sync signals. */
typedef enum lcdcRasterActiveSyncEdge
{
    LCDC_RASTER_ACTIVE_SYNC_EDGE_FALLING =
        LCDC_RASTER_TIMING_2_SYNC_EDGE_FALLING,
    /**< HSYNC and VSYNC are driven on falling edge of pixel clock. */
    LCDC_RASTER_ACTIVE_SYNC_EDGE_RISING =
        LCDC_RASTER_TIMING_2_SYNC_EDGE_RISING,
    /**< HSYNC and VSYNC are driven on the rising edge of pixel clock. */
}lcdcRasterActiveSyncEdge_t;


/** \brief Enumerates HSYNC/VSYNC edges active or inactive modes. */
typedef enum lcdcRasterSyncCtrl
{
    LCDC_RASTER_SYNC_CTRL_INACTIVE  =
        LCDC_RASTER_TIMING_2_PHSVS_ON_OFF_OPPOSITE_EDGE,
    /**< HSYNC and VSYNC are driven on opposite edges of pixel clock
         than pixel data. */
    LCDC_RASTER_SYNC_CTRL_ACTIVE = LCDC_RASTER_TIMING_2_PHSVS_ON_OFF_BIT24
    /**< HSYNC and VSYNC are driven according to sync edge configuration. */
}lcdcRasterSyncCtrl_t;


/** \brief Enumerates the supported frame buffer modes of LCDC. */
typedef enum lcdcDmaFrmMode
{
    LCDC_DMA_FRM_MODE_SINGLE_BUF = LCDC_LCDDMA_CTRL_FRAME_MODE_ONE,
    /**< Single frame buffer mode. */
    LCDC_DMA_FRM_MODE_DOUBLE_BUF = LCDC_LCDDMA_CTRL_FRAME_MODE_TWO,
    /**< Double frame buffer mode. */
}lcdcDmaFrmMode_t;


/** \brief  Enumerates the supported values of DMA burst sizes. */
typedef enum lcdcDmaBurstSize
{
    LCDC_DMA_BURST_SIZE_1 = LCDC_LCDDMA_CTRL_BURST_SIZE_ONE,
    /**< Burst size is 1 32-bit word. */
    LCDC_DMA_BURST_SIZE_2 = LCDC_LCDDMA_CTRL_BURST_SIZE_TWO,
    /**< Burst size is 2 32-bit words. */
    LCDC_DMA_BURST_SIZE_4 = LCDC_LCDDMA_CTRL_BURST_SIZE_FOUR,
    /**< Burst size is 4 32-bit words. */
    LCDC_DMA_BURST_SIZE_8 = LCDC_LCDDMA_CTRL_BURST_SIZE_EIGHT,
    /**< Burst size is 8 32-bit words. */
    LCDC_DMA_BURST_SIZE_16 = LCDC_LCDDMA_CTRL_BURST_SIZE_SIXTEEN,
    /**< Burst size is 16 32-bit words. */
}lcdcDmaBurstSize_t;


/** \brief Enumerates the supported DMA FIFO Threshold values. */
typedef enum lcdcFifoThreshold
{
    LCDC_FIFO_THRESHOLD_8 = LCDC_LCDDMA_CTRL_TH_FIFO_READY_EIGHT,
    /**< Fifo threshold is 8 32-bit words. */
    LCDC_FIFO_THRESHOLD_16 = LCDC_LCDDMA_CTRL_TH_FIFO_READY_SIXTEEN,
    /**< Fifo threshold is 16 32-bit words. */
    LCDC_FIFO_THRESHOLD_32 = LCDC_LCDDMA_CTRL_TH_FIFO_READY_THIRTY_TWO,
    /**< Fifo threshold is 32 32-bit words. */
    LCDC_FIFO_THRESHOLD_64 = LCDC_LCDDMA_CTRL_TH_FIFO_READY_SIXTY_FOUR,
    /**< Fifo threshold is 64 32-bit words. */
    LCDC_FIFO_THRESHOLD_128 = LCDC_LCDDMA_CTRL_TH_FIFO_READY_ONE_TWENTY_EIGHT,
    /**< Fifo threshold is 128 32-bit words. */
    LCDC_FIFO_THRESHOLD_256 = LCDC_LCDDMA_CTRL_TH_FIFO_READY_TWO_FIFTY_SIX,
    /**< Fifo threshold is 256 32-bit words. */
    LCDC_FIFO_THRESHOLD_512 = LCDC_LCDDMA_CTRL_TH_FIFO_READY_FIVE_TWELVE,
    /**< Fifo threshold is 512 32-bit words. */
}lcdcFifoThreshold_t;


/** \brief Enumerates the supported endian types of LCDC DMA data. */
typedef enum lcdcDmaDataEndian
{
    LCDC_DMA_DATA_ENDIAN_LITTLE = LCDC_LCDDMA_CTRL_BIGENDIAN_OFF,
    /**< Data is in little endian format. */
    LCDC_DMA_DATA_ENDIAN_BIG = LCDC_LCDDMA_CTRL_BIGENDIAN_ON,
    /**< Data is in big endian format. */
}lcdcDmaDataEndian_t;


/** \brief Enumerates the supported DMA frame buffer Ids. */
typedef enum lcdcFrmBufId
{
    LCDC_FRM_BUF_ID_MIN = 0U,
    /**< Minimum value of enumeration. Used for input validation. */
    LCDC_FRM_BUF_ID_0 = LCDC_FRM_BUF_ID_MIN,
    /**< DMA frame buffer 0. */
    LCDC_FRM_BUF_ID_1 = 1U,
    /**< DMA frame buffer 1. */
    LCDC_FRM_BUF_ID_MAX = LCDC_FRM_BUF_ID_1
    /**< Maximum value of enumeration. Used for input validation. */
}lcdcFrmBufId_t;


/** \brief Enumerates the supported positions of sub panel data based on the
           Lines Per Panel Threshold value. */
typedef enum lcdcRasterSubPanelPos
{
    LCDC_RASTER_SUB_PANEL_POS_TOP = LCDC_RASTER_SUBPANEL_HOLS_DATA_TOP,
    /**< Default Pixel Data lines are at the top of the screen and the
         active video lines are at the bottom of the screen. */
    LCDC_RASTER_SUB_PANEL_POS_BOTTOM = LCDC_RASTER_SUBPANEL_HOLS_DATA_BOTTOM,
    /**< Active video lines are at the top of the screen and Default Pixel
         Data lines are at the bottom of the screen. */
}lcdcRasterSubPanelPos_t;


/** \brief Enumerates the module ids of LCDC. */
typedef enum lcdcSubModuleId
{
    LCD_SUB_MODULE_ID_MIN = 0U,
    /**< Minimum value of enumeration. Used for input validation. */
    LCDC_SUB_MODULE_ID_CORE = LCD_SUB_MODULE_ID_MIN,
    /**< Core module which contains Raster active and passive matrix. */
    LCDC_SUB_MODULE_ID_DMA = 1U,
    /**< DMA sub module. */
    LCDC_SUB_MODULE_ID_LCD = 2U,
    /**< Entire LCD module which includes all its sub modules. */
    LCDC_SUB_MODULE_ID_MAX = LCDC_SUB_MODULE_ID_LCD,
    /**< Maximum value of enumeration. Used for input validation. */
}lcdcSubModuleId_t;


/** \brief Enumerates the supported standby modes of LCDC. */
typedef enum lcdcStandbyMode
{
    LCDC_STANDBY_MODE_FORCE = LCDC_SYSCONFIG_STANDBYMODE_FORCE,
    /**< Force standby mode. */
    LCDC_STANDBY_MODE_NO_STANDBY = LCDC_SYSCONFIG_STANDBYMODE_NOSTANDBY,
    /**< No standby mode. */
    LCDC_STANDBY_MODE_SMART = LCDC_SYSCONFIG_STANDBYMODE_SMART,
    /**< Smart standby mode. */
    LCDC_STANDBY_MODE_SMART_WAKEUP = LCDC_SYSCONFIG_STANDBYMODE_WAKEUP
    /**< Smart wakeup standby mode. */
}lcdcStandbyMode_t;


/** \brief Enumerates the supported idle modes of LCDC. */
typedef enum lcdcIdleMode
{
    LCDC_IDLE_MODE_FORCE = LCDC_SYSCONFIG_IDLEMODE_FORCE,
    /**< Force idle mode. */
    LCDC_IDLE_MODE_NO_IDLE = LCDC_SYSCONFIG_IDLEMODE_NOIDLE,
    /**< No idle mode. */
    LCDC_IDLE_MODE_SMART = LCDC_SYSCONFIG_IDLEMODE_SMART,
    /**< Smart idle mode. */
    LCDC_IDLE_MODE_SMART_WAKEUP = LCDC_SYSCONFIG_IDLEMODE_WAKEUP,
    /**< Smart wakeup idle mode. */
}lcdcIdleMode_t;


/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/** \brief Structure holding the LCDC DMA configuration parameters. */
typedef struct lcdcDmaCfg
{
    uint32_t frameMode;
    /**< Frame mode either single frame buffer or double frame buffer mode.
         This can take values from the following enum #lcdcDmaFrmMode_t. */
    uint32_t burstSize;
    /**< Burst size setting for DMA transfers. This can take values from the
         following enum #lcdcDmaBurstSize_t. */
    uint32_t fifoThreshold;
    /**< The DMA FIFO becomes ready when the number of words specified by this
         setting from the frame buffer have been loaded. This can take values
         from the following enum #lcdcFifoThreshold_t. */
    uint32_t endian;
    /**< Endian of the input data. This can take values from the following
         enum #lcdcDmaDataEndian_t. */
}lcdcDmaCfg_t;


/** \brief Structure holding the raster display parameters. */
typedef struct lcdcRasterDisplayAttrCfg
{
    uint32_t displayMode;
    /**< Display Matrix mode either passive or active. This can take values
         from the following enum #lcdcRasterMatrixMode_t. */
    uint32_t displayType;
    /**< Type of display either color or monochromatic. This can take values
         from the following enum #lcdcRasterDisplayType_t. */
    uint32_t pixelFormat;
    /**< Input pixel format. This can take values from the following enum
         #lcdcPixelFormat_t. */
    uint32_t paletteMode;
    /**< Load mode of color palette. This can take values from the following
         enum #lcdcPaletteMode_t. */
    uint32_t enableExtrapolate;
    /**< Extrapolate output pixel data to 565 format. This can take following
         two values: TRUE or FALSE. */
    uint32_t mono8Bit;
    /**< Monochromatic display output bit width. This can take values from the
         following enum #lcdcMonoType_t */
}lcdcRasterDisplayAttrCfg_t;


/** \brief Structure defining the LCD Panel parameters. */
typedef struct lcdcRasterTimingCfg
{
  uint32_t ppl;
  /**< Number of Pixels Per Line. This can take values in the following
       range: 1 to 2048. */
  uint32_t hfp;
  /**< Horizontal Front Porch. This can take values in the following
       range: 1 to 1024. */
  uint32_t hbp;
  /**< Horizontal Back Porch. This can take values in the following
       range: 1 to 1024. */
  uint32_t hsw;
  /**< Horizontal Sync Width. This can take values in the following
       range: 1 to 1024. */
  uint32_t lpp;
  /**< Number of Lines Per Panel. This can take values in the following
       range: 1 to 2048. */
  uint32_t vfp;
  /**< Vertical Front Porch. This can take values in the following
       range: 0 to 255. */
  uint32_t vbp;
  /**< Vertical Back Porch. This can take values in the following
       range: 0 to 255. */
  uint32_t vsw;
  /**< Vertical Sync Width. This can take values in the following
       range: 1 to 64. */
  uint32_t pixelClk;
  /**< Value of LCD pixel clock. */
}lcdcRasterTimingCfg_t;


/** \brief Structure holding the LCD signal polarity and edge configuration
           parameters. */
typedef struct lcdcRasterPolarityCfg
{
    uint32_t hSyncPolarity;
    /**< Polarity of Horizontal Sync. This can take values from the following
        enum #lcdcRasterHSyncPol_t. */
    uint32_t vSyncPolarity;
    /**< Polarity of Vertical Sync. This can take values from the following
        enum #lcdcRasterVSyncPol_t. */
    uint32_t outputEnablePolarity;
    /**< Polarity of Output Enable Signal. This can take values from the
         following enum #lcdcRasterOutputEnablePol_t. */
    uint32_t pixelClkPolarity;
    /**< Polarity of Pixel clock. This can take values from the following
        enum #lcdcRasterPclkPol_t. */
    uint32_t syncEdge;
    /**< Active edge of Horizontal/Vertical Sync. This can take values from the
        following enum #lcdcRasterActiveSyncEdge_t. */
    uint32_t sycnEdgeCtrl;
    /**< Control of Horizontal/Vertical Sync by pixel clock. This can take
         values from the following enum #lcdcRasterSyncCtrl_t. */
}lcdcRasterPolarityCfg_t;


/** \brief Structure holding the ac bias configuration parameters. */
typedef struct lcdcRasterAcbCfg
{
    uint32_t acbFreq;
    /**<  Number of line clocks to count before transitioning the
          ac-bias pin. */
    uint32_t acbi;
    /**<  Number of ac-bias transitions per interrupt. */
}lcdcRasterAcbCfg_t;


/** \brief Structure holding the LCDC register context. */
typedef struct lcdcContext
{
    uint32_t clkcEnable;
    /**< LCDC clock enable register. */
    uint32_t lcdCtrl;
    /**< LCDC control register. */
    uint32_t lcddmaCtrl;
    /**< LCDC DMA control register. */
    uint32_t rasterTiming0;
    /**< LCDC Raster timing 0 register. */
    uint32_t rasterTiming1;
    /**< LCDC Raster timing 1 register. */
    uint32_t rasterTiming2;
    /**< LCDC Raster timing 2 register. */
    uint32_t rasterCtrl;
    /**< Raster control register. */
    uint32_t irqEnableSet;
    /**< Interrupt enable register. */
    uint32_t lcddmaFb0Base;
    /**< Frame buffer 0 Base register. */
    uint32_t lcddmaFb0Ceiling;
    /**< Frame buffer 0 ceiling register. */
    uint32_t lcddmaFb1Base;
    /**< Frame buffer 1 Base register. */
    uint32_t lcddmaFb1Ceiling;
    /**< Frame buffer 1 ceiling register. */
}lcdcContext_t;


/* ========================================================================== */
/*                            Global Variables Declarations                   */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   This API will return the revision number of the LCD controller.
 *
 * \param   baseAddr  Memory address of the LCDC.
 *
 * \retval  Revision  Revision number of LCD controller. This will give the
 *                    information regarding the major and minor revisions:
 *                    - Bits[15:11] - Release number for this IP
 *                    - Bits[10:8]  - Major revision
 *                    - Bits[7:6]   - Custom IP
 *                    - Bits[5:0]   - Minor revision
 */
uint32_t LCDCGetRevision(uint32_t baseAddr);


/**
 * \brief  This API configures clock divisor value to generate required
 *         frequency of pixel clock and also selects the raster control.
 *
 * \param  baseAddr    Memory address of LCDC.
 * \param  pixelClk    Required Pixel Clock frequency in Hz.
 * \param  moduleClk   Input clock to LCDC module from PLL in Hz.
 */
void LCDCClkConfig(uint32_t baseAddr, uint32_t pixelClk, uint32_t moduleClk);


/**
 * \brief   This API configures the DMA configuration parameters.
 *
 * \param   baseAddr Memory address of the LCDC.
 * \param   pCfg     Pointer to the structure #lcdcDmaCfg_t
 *                   containing the DMA configuration parameters.
 */
void LCDCDmaConfig(uint32_t baseAddr, lcdcDmaCfg_t *pCfg);


/**
 * \brief This API controls the enabling/disabling of Raster Control.
 *
 * \param baseAddr      Memory address of the LCDC.
 * \param enableRaster  Flag controlling the enable/disable of raster.
 *                      This can take following two values:
 *                      - TRUE  - Enable raster control.
 *                      - FALSE - Disable raster control.
 */
void LCDCRasterEnable(uint32_t baseAddr, uint32_t enableRaster);


/**
 * \brief  This API configures the attributes of raster display.
 *
 * \param  baseAddr  Memory address of the LCDC.
 * \param  pCfg      Pointer to the structure #lcdcRasterDisplayAttrCfg_t which
 *                   contains the display configuration parameters.
 */
void LCDCRasterDisplayModeConfig(uint32_t baseAddr,
                                 lcdcRasterDisplayAttrCfg_t *pCfg);


/**
 * \brief This API configures the LCD panel timing parameters.
 *
 * \param baseAddr  Memory address of the LCDC.
 * \param pCfg      Pointer to the structure #lcdcRasterTimingCfg_t
 *                  containing the LCD panel timing parameters.
 */
void LCDCRasterTimingConfig(uint32_t baseAddr, lcdcRasterTimingCfg_t *pCfg);


/**
 * \brief   This API configures the polarities of the various signals of LCD.
 *
 * \param   baseAddr Memory address of the LCDC.
 *
 * \param   pCfg     Pointer to the structure #lcdcRasterPolarityCfg_t
 *                   containing the polarities of various signals of LCD.
 */
void LCDCRasterPolarityConfig(uint32_t baseAddr,
                              lcdcRasterPolarityCfg_t *pCfg);


/**
 * \brief   This API configures the ACBias signal parameters.
 *
 * \param   baseAddr Memory address of the LCDC.
 * \param   pCfg     Pointer to the structure #lcdcRasterAcbCfg_t
 *                   containing the parameters of ACbias control.
 */
void LCDCRasterAcbiasConfig(uint32_t baseAddr, lcdcRasterAcbCfg_t *pCfg);


/**
 * \brief This API configures the DMA fifo delay.
 *
 * \param baseAddr  Memory address of the LCDC.
 * \param delay     DMA Fifo delay.
 */
void LCDCRasterSetDmaFifoDelay(uint32_t baseAddr, uint32_t delay);


/**
 * \brief  This API configures base and ceiling value for Frame buffer.
 *
 * \param  baseAddr Memory Address of the LCD Module
 * \param  bufAddr  Base address of array which contain pixels of image to be
 *                  displayed on LCD.
 * \param  ceiling  End address of the array which contain pixels of image to be
 *                  displayed on LCD.
 * \param  frmBufId Value identifying the frame buffer number. This can take
 *                  values from the following enum #lcdcFrmBufId_t;
 */
void LCDCDmaFrameBufConfig(uint32_t baseAddr,
                           uint32_t bufAddr,
                           uint32_t ceiling,
                           uint32_t frmBufId);


/**
 * \brief This API enables selected interrupts in LCD controller.
 *
 * \param baseAddr Memory address of the LCDC.
 * \param intrMask Mask specifying the interrupts that need to be enabled.
 *                 This mask can take single value or OR of multiple values
 *                 from the following enum #lcdcIntrMask_t.
 */
void LCDCIntrEnable(uint32_t baseAddr, uint32_t intrMask);


/**
 * \brief This API disables selected interrupts in LCD controller.
 *
 * \param baseAddr Memory address of the LCDC.
 * \param intrMask Mask specifying the interrupts that need to be disabled.
 *                 This mask can take single value or OR of multiple values
 *                 from the following enum #lcdcIntrMask_t.
 */
void LCDCIntrDisable(uint32_t baseAddr, uint32_t intrMask);


/**
 * \brief   This API returns the status of the LCDC interrupts.
 *
 * \param   baseAddr    Memory address of the LCDC.
 *
 * \retval  intrStatus  Mask containing the status of all LCDC interrupts.
 */
uint32_t LCDCIntrStatus(uint32_t baseAddr);


/**
 * \brief   This API returns the raw status of the LCDC interrupts.
 *
 * \param   baseAddr    Memory address of the LCDC.
 *
 * \retval  intrStatus  Mask containing the raw status of all LCDC interrupts.
 */
uint32_t LCDCIntrRawStatus(uint32_t baseAddr);


/**
 * \brief   This API clears the status of the specific LCDC interrupts.
 *
 * \param   baseAddr  Memory address of the LCDC.
 * \param   intrMask  Mask specifying the interrupt, for which the status has
 *                    to be cleared. This mask can take single value or OR of
 *                    multiple values from the following enum #lcdcIntrMask_t.
 */
void LCDCIntrClear(uint32_t baseAddr, uint32_t intrMask);


/**
 * \brief   This API triggers the specific LCDC interrupts.
 *
 * \param   baseAddr  Memory address of the LCDC.
 * \param   intrMask  Mask specifying the interrupts that have to be triggered.
 *                    This mask can take single value or OR of multiple values
 *                    from the following enum #lcdcIntrMask_t.
 */
void LCDCIntrTrigger(uint32_t baseAddr, uint32_t intrMask);


/**
 * \brief   This API enables/disables the sub panel feature of raster.
 *
 * \param   baseAddr        Memory address of the LCDC.
 * \param   enableSubPanel  Flag controlling the enable/disable of sub panel
 *                          feature. This can take following two values:
 *                          - TRUE  - Enable sub panel feature.
 *                          - FALSE - Disable sub panel feature.
 */
void LCDCRasterSubPanelEnable(uint32_t baseAddr, uint32_t enableSubPanel);


/**
 * \brief   This API configures the Sub panel parameters.
 *
 * \param   baseAddr     Memory address of the LCDC.
 * \param   subPanelPos  Value indicates the position of sub panel
 *                       compared to the LPPT(lines per panel threshold) value.
 *                       This can take values from the following enum
 *                       #lcdcRasterSubPanelPos_t.
 * \param   linesPerPanelThr  Defines the number of lines to be refreshed.
 *                            This can take values in the range: 1 to 2048.
 * \param   defaultPixelData  Defines the default value of the pixel data sent
 *                            to the panel for the lines until LPPT is reached
 *                            or after passing the LPPT.
 */
void LCDCRasterSubPanelConfig(uint32_t baseAddr,
                              uint32_t subPanelPos,
                              uint32_t linesPerPanelThr,
                              uint32_t defaultPixelData);


/**
 * \brief This API Enables the clock for the DMA sub module, LIDD
 *        sub-module and for the core(which encompasses the Raster active matrix
 *        and Passive matrix).
 *
 * \param baseAddr  Memory Address of the LCDC.
 */
void LCDCClocksEnable(uint32_t baseAddr);


/**
 * \brief This API enables/disables clock for the core, which encompasses the
 *        raster active matrix and passive matrix logic.
 *
 * \param baseAddr         Memory Address of the LCDC.
 * \param enableRasterClk  Flag controlling the enable/disable of clock. This
 *                         can take following values:
 *                         - TRUE  - Enable clock.
 *                         - FALSE - Disable clock.
 */
void LCDCRasterClkEnable(uint32_t baseAddr, uint32_t enableRasterClk);


/**
 * \brief This API enables/disables clock for the DMA sub-module.
 *
 * \param baseAddr      Memory Address of the LCDC.
 * \param enableDmaClk  Flag controlling the enable/disable of clock. This can
 *                      take following values:
 *                      - TRUE  - Enable clock.
 *                      - FALSE - Disable clock.
 */
void LCDCDmaClkEnable(uint32_t baseAddr, uint32_t enableDmaClk);


/**
 * \brief This API enables/disables the software resets for LCD module or DMA
 *        sub-module or core which encompasses Raster Active Matrix and
 *        Passive Matrix logic based on the argument passed to this function.
 *
 * \param baseAddr   Memory Address of the LCDC.
 * \param moduleId   Flag identifying the module for which software reset has to
 *                   to be enabled. This can take values from the following
 *                   enum #lcdcSubModuleId_t.
 * \param enableSoftwareReset  Flag controls the enable/disable of software
 *                             reset. This can take following two values:
 *                             - TRUE  - Enable software reset.
 *                             - FALSE - Disable software reset.
 */
void LCDCSoftwareResetEnable(uint32_t baseAddr,
                             uint32_t moduleId,
                             uint32_t enableSoftwareReset);


/**
 * \brief  This API sets the DMA master priority for the L3 OCP Master Bus.
 *
 * \param  baseAddr  Memory address of the LCDC.
 * \param  priority  Value which sets the priority.
 */
void LCDCSetDmaMasterPriority(uint32_t baseAddr, uint32_t priority);


/**
 * \brief  This API configures the standby mode of LCD controller.
 *
 * \param  baseAddr    Memory address of the LCDC.
 * \param  standbyMode Standby mode of LCDC. This can take values from the
 *                     following enum #lcdcStandbyMode_t;
 */
void LCDCSetStandbyMode(uint32_t baseAddr, uint32_t standbyMode);


/**
 * \brief  This API configures the Idle mode of LCD controller.
 *
 * \param  baseAddr    Memory address of the LCDC.
 * \param  idleMode    Idle mode of LCDC. This can take values from the
 *                     following enum #lcdcIdleMode_t;
 */
void LCDCSetIdleMode(uint32_t baseAddr, uint32_t idleMode);


/**
 * \brief  This API enables/disables the auto under flow feature.
 *
 * \details When this feature is disabled, on an underflow, the software has to
 *          restart the module. If enabled, on an underflow, the hardware will
 *          restart on the next frame.
 *
 * \param  baseAddr             Memory address of LCDC.
 * \param  enableAutoUnderFlow  Flag controlling the enable/disable of feature.
 *                              This can take following two values:
 *                              - TRUE  - Enable auto under flow.
 *                              - FALSE - Disable auto under flow.
 */
void LCDCAutoUnderFlowEnable(uint32_t baseAddr, uint32_t enableAutoUnderFlow);


/**
 * \brief   This API enables/disables the byte swap feature, which controls the
 *          byte lane ordering of the data on the output of the DMA module.
 *
 * \param   baseAddr        Memory address of the LCDC.
 * \param   enableByteSwap  Flag enables/disables the byte swap feature.
 *                          This can take following two values:
 *                          - TRUE  - Enable byte swap.
 *                          - FALSE - Disable byte swap.
 */
void LCDCByteSwapEnable(uint32_t baseAddr, uint32_t enableByteSwap);


/**
 * \brief  This API selects the data order of frame buffer parsing for
 *         palette data look up.
 *
 * \param  baseAddr  Memory address of the LCDC.
 * \param  dataOrder Flag selecting the data order. This can take values from
 *                   the following enum #lcdcRasterDataOrder_t.
 *
 * \note   This API should not be called when the pixel format is one of the
 *         following: 12/16/24 bpp
 */
void LCDCRasterDataOrderSelect(uint32_t baseAddr, uint32_t dataOrder);


/**
 * \brief This API enables/disables the nibble mode.
 *
 * \param baseAddr         Memory address of the LCDC.
 * \param enableNibbleMode Flag controlling the enable/disable of nibble mode.
 *                         This can take values as inputs:
 *                         - TRUE  - Enable nibble mode.
 *                         - FALSE - Disable nibble mode.
 */
void LCDCRasterNibbleModeEnable(uint32_t baseAddr, uint32_t enableNibbleMode);


/**
 * \brief  This API saves the context of LCDC registers.
 *         This is useful in power management, where the power supply to raster
 *         controller will be cut off.
 *
 * \param  baseAddr    Memory address of the LCDC.
 * \param  pCtx        Pointer to the structure where the context has to be
 *                     saved.
 */
void LCDCContextSave(uint32_t baseAddr, lcdcContext_t *pCtx);


/**
 * \brief  This API restores the context of LCDC registers.
 *         This is useful in power management, where the power supply to raster
 *         controller will be cut off.
 *
 * \param  baseAddr    Memory address of the LCDC.
 * \param  pCtx        Pointer to the structure from where the context has to be
 *                     restored.
 */
void LCDCContextRestore(uint32_t baseAddr, lcdcContext_t *pCtx);


/* ========================================================================== */
/*                    Deprecated Function Declarations                        */
/* ========================================================================== */

/**
 * \brief   This API enables the end of frame interrupt. This API is valid
 *          only for am1808 raster. It should not be used for programming of
 *          am33xx raster.
 */
DEPRECATED(void RasterEndOfFrameIntEnable(uint32_t baseAddr));


/**
 * \brief   This API disables the end of frame interrupt. This API is valid
 *          only for am1808 raster. It should not be used for programming of
 *          am33xx raster.
 */
DEPRECATED(void RasterEndOfFrameIntDisable(uint32_t baseAddr));

#ifdef __cplusplus
}
#endif

#endif /* #ifndef LCDC_H_ */
