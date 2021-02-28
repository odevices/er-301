/**
 *  \file     hs_mmcsd.h
 *
 *  \brief    This file contains the prototypes of the APIs present in the
 *            device abstraction layer file of High Speed MMC/SD controller.
 *            This also contains some related macros, enums and structures.
 *
 *  \details  programming sequence of MMC/SD controller is:
 *            -# Perform soft reset using the API #HSMMCSDSoftReset.
 *            -# Perform reset of all the lines MMC/SD controller using the
 *               API #HSMMCSDLinesReset.
 *            -# Set the supported voltages using the API
 *               #HSMMCSDSetSupportedVoltage.
 *            -# Perform the MMC/SD controller's system configuration using the
 *               API #HSMMCSDSystemConfig.
 *            -# Set the controller's bus width using the API
 *               #HSMMCSDSetBusWidth.
 *            -# Set the bus voltage using the API #HSMMCSDSetBusVolt.
 *            -# Power on the controller's bus using the API
 *               #HSMMCSDBusPowerOnCtrl. The second parameter should be passed
 *               is HS_MMCSD_PWR_CTRL_ON.
 *            -# Set the bus frequency using the API #HSMMCSDSetBusFreq.
 *            -# Send the initialization stream using the API
 *               #HSMMCSDInitStreamSend.
 *            -# Enable the required interrupts of the controller using the
 *               API #HSMMCSDIntrEnable.
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

#ifndef HS_MMCSD_H_
#define HS_MMCSD_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/am335x/cslr_mmchs.h>

#ifdef __cplusplus
extern "C"
{
#endif
    /* ========================================================================== */
    /*                             Macros & Typedefs                              */
    /* ========================================================================== */

    /* ========================================================================== */
    /*                         Structures and Enums                               */
    /* ========================================================================== */

    /** \brief Enumerates the types of lines fore hs mmcsd reset. */
    typedef enum hsMmcsdResetLineMask
    {
        HS_MMCSD_RESET_LINE_MASK_DATA = CSL_MMCHS_SYSCTL_SRD_MASK,
        /**< Data line reset. */
        HS_MMCSD_RESET_LINE_MASK_CMD = CSL_MMCHS_SYSCTL_SRC_MASK,
        /**< Command line reset. */
        HS_MMCSD_RESET_LINE_MASK_ALL = CSL_MMCHS_SYSCTL_SRA_MASK
        /**< Reset of all the lines. */
    } hsMmcsdResetLineMask_t;

    /** \breif Enumerates the clock activity configurations during wake up . */
    typedef enum hsMmcsdClkAct
    {
        HS_MMCSD_CLK_ACT_ICLK_FCLK_OFF = CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_NONE,
        /**< Both functional and interface clocks are off. */
        HS_MMCSD_CLK_ACT_FCLK_OFF = CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_FUNC,
        /**< Interface clock is on and Functional clock is off. */
        HS_MMCSD_CLK_ACT_ICLK_OFF = CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_OCP,
        /**< Interface clock is off and Functional clock is on. */
        HS_MMCSD_CLK_ACT_ICLK_FLCK_ON = CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_BOTH
        /**< Both functional and interface clocks are on. */
    } hsMmcsdClkAct_t;

    /** \breif Enumerates the standby modes. */
    typedef enum hsMmcsdStandbyMode
    {
        HS_MMCSD_STANDBY_MODE_FORCE = CSL_MMCHS_SYSCONFIG_STANDBYMODE_FORCE,
        /**< Force standby mode. */
        HS_MMCSD_STANDBY_MODE_NONE = CSL_MMCHS_SYSCONFIG_STANDBYMODE_NOIDLE,
        /**< No standby mode. */
        HS_MMCSD_STANDBY_MODE_SMART = CSL_MMCHS_SYSCONFIG_STANDBYMODE_SMART,
        /**< Smart standby mode. */
        HS_MMCSD_STANDBY_MODE_SMART_WAKEUP =
            CSL_MMCHS_SYSCONFIG_STANDBYMODE_SMARTWAKE
        /**< Smart standby with wakeup mode. */
    } hsMmcsdStandbyMode_t;

    /** \breif Enumerates the idle modes. */
    typedef enum hsMmcsdIdleMode
    {
        HS_MMCSD_IDLE_MODE_FORCE = CSL_MMCHS_SYSCONFIG_SIDLEMODE_FORCE,
        /**< Force idle mode. */
        HS_MMCSD_IDLE_MODE_NONE = CSL_MMCHS_SYSCONFIG_SIDLEMODE_NOIDLE,
        /**< No idle mode. */
        HS_MMCSD_IDLE_MODE_SMART = CSL_MMCHS_SYSCONFIG_SIDLEMODE_SMART
        /**< Smart idle mode. */
    } hsMmcsdIdleMode_t;

    /** \brief Enumerates mmc sd controller's supported bus widths. */
    typedef enum hsMmcsdBusWidth
    {
        HS_MMCSD_BUS_WIDTH_1BIT = 1U,
        /**< Bus width is 1 bit. */
        HS_MMCSD_BUS_WIDTH_4BIT = 4U,
        /**< Bus width is 4 bits. */
        HS_MMCSD_BUS_WIDTH_8BIT = 8U
        /**< Bus width is 8 bits. */
    } hsMmcsdBusWidth_t;

    /** \brief Enumerates mmc sd controller's bus voltages. */
    typedef enum hsMmcsdBusVolt
    {
        HS_MMCSD_BUS_VOLT_1P8 = CSL_MMCHS_HCTL_SDVS__1V8,
        /**< Bus voltage is 1.8 Volts. */
        HS_MMCSD_BUS_VOLT_3P0 = CSL_MMCHS_HCTL_SDVS__3V0,
        /**< Bus voltage is 3.0 volts. */
        HS_MMCSD_BUS_VOLT_3P3 = CSL_MMCHS_HCTL_SDVS__3V3
        /**< Bus voltage is 3.3 volts. */
    } hsMmcsdBusVolt_t;

    /** \brief Enumerates macros to configure power on/off. */
    typedef enum hsMmcsdPwrCtrl
    {
        HS_MMCSD_PWR_CTRL_ON = CSL_MMCHS_HCTL_SDBP_PWRON,
        /**< Power on the controller. */
        HS_MMCSD_PWR_CTRL_OFF = CSL_MMCHS_HCTL_SDBP_PWROFF
        /**< Power off the controller. */
    } hsMmcsdPwrCtrl_t;

    /** \brief Enumerates the list of supported bus voltages. */
    typedef enum hsMmcsdSuppVolt
    {
        HS_MMCSD_SUPP_VOLT_1P8 = CSL_MMCHS_CAPA_VS18_MASK,
        /**< Supported voltage is 1.8 Volts. */
        HS_MMCSD_SUPP_VOLT_3P0 = CSL_MMCHS_CAPA_VS30_MASK,
        /**< Supported voltage is 3.0 Volts. */
        HS_MMCSD_SUPP_VOLT_3P3 = CSL_MMCHS_CAPA_VS33_MASK
        /**< Supported voltage is 3.3 Volts. */
    } hsMmcsdSuppVolt_t;

    /** \brief Enumerates the list of command types. */
    typedef enum hsMmcsdCmdType
    {
        HS_MMCSD_CMD_TYPE_OTHER = CSL_MMCHS_CMD_CMD_TYPE_NORMAL,
        /**< Others commands. */
        HS_MMCSD_CMD_TYPE_BUS_SUSPEND = CSL_MMCHS_CMD_CMD_TYPE_SUSPEND,
        /**< Upon CMD52 "Bus Suspend" operation. */
        HS_MMCSD_CMD_TYPE_FUNC_SEL = CSL_MMCHS_CMD_CMD_TYPE_RESUME,
        /**< Upon CMD52 "Function Select" operation. */
        HS_MMCSD_CMD_TYPE_IO_ABORT = CSL_MMCHS_CMD_CMD_TYPE_ABORT
        /**< Upon CMD12 or CMD52 "I/O Abort" command. */
    } hsMmcsdCmdType_t;

    /** \brief Enumerates the list of response types. */
    typedef enum hsMmcsdRspType
    {
        HS_MMCSD_RSP_TYPE_NONE = CSL_MMCHS_CMD_RSP_TYPE_NORSP,
        /**< No response. */
        HS_MMCSD_RSP_TYPE_LEN_136 = CSL_MMCHS_CMD_RSP_TYPE_LGHT36,
        /**< Response Length 136 bits. */
        HS_MMCSD_RSP_TYPE_LEN_48 = CSL_MMCHS_CMD_RSP_TYPE_LGHT48,
        /**< Response Length 48 bits. */
        HS_MMCSD_RSP_TYPE_LEN_48_BUSY = CSL_MMCHS_CMD_RSP_TYPE_LGHT48B
        /**< Response Length 48 bits with busy after response. */
    } hsMmcsdRspType_t;

    /** \brief Enumerates the list of response types. */
    typedef enum hsMmcsdXferType
    {
        HS_MMCSD_XFER_TYPE_TX = CSL_MMCHS_CMD_DDIR_WRITE,
        /**< Data Write (host to card). */
        HS_MMCSD_XFER_TYPE_RX = CSL_MMCHS_CMD_DDIR_READ
        /**< Data Read (card to host). */
    } hsMmcsdXferType_t;

    /** \brief Enumerates the controller's interrupt masks. */
    typedef enum hsMmcsdIntrMask
    {
        HS_MMCSD_INTR_MASK_BADACCESS = CSL_MMCHS_IE_BADA_ENABLE_MASK,
        /**< Bad access to data space interrupt. */
        HS_MMCSD_INTR_MASK_CARDERROR = CSL_MMCHS_IE_CERR_ENABLE_MASK,
        /**< Card error interrupt. */
        HS_MMCSD_INTR_MASK_ADMAERROR = CSL_MMCHS_IE_ADMAE_ENABLE_MASK,
        /**< ADMA error interrupt. */
        HS_MMCSD_INTR_MASK_ACMD12ERR = CSL_MMCHS_IE_ACE_ENABLE_MASK,
        /**< Auto CMD12 error interrupt. */
        HS_MMCSD_INTR_MASK_DATABITERR = CSL_MMCHS_IE_DEB_ENABLE_MASK,
        /**< Data end bit error interrupt. */
        HS_MMCSD_INTR_MASK_DATACRCERR = CSL_MMCHS_IE_DCRC_ENABLE_MASK,
        /**< Data CRC error interrupt. */
        HS_MMCSD_INTR_MASK_DATATIMEOUT = CSL_MMCHS_IE_DTO_ENABLE_MASK,
        /**< Data time out error interrupt. */
        HS_MMCSD_INTR_MASK_CMDINDXERR = CSL_MMCHS_IE_CIE_ENABLE_MASK,
        /**< Command index error interrupt. */
        HS_MMCSD_INTR_MASK_CMDBITERR = CSL_MMCHS_IE_CEB_ENABLE_MASK,
        /**< Command end bit error. */
        HS_MMCSD_INTR_MASK_CMDCRCERR = CSL_MMCHS_IE_CCRC_ENABLE_MASK,
        /**< Command CRC error interrupt. */
        HS_MMCSD_INTR_MASK_CMDTIMEOUT = CSL_MMCHS_IE_CTO_ENABLE_MASK,
        /**< Command timeout error interrupt. */
        HS_MMCSD_INTR_MASK_ERR = CSL_MMCHS_IE_NULL_MASK,
        /**< Error interrupt. */
        HS_MMCSD_INTR_MASK_CARDREM = CSL_MMCHS_IE_CREM_ENABLE_MASK,
        /**< Card removal signal interrupt. */
        HS_MMCSD_INTR_MASK_CARDINS = CSL_MMCHS_IE_CINS_ENABLE_MASK,
        /**< Card insertion signal interrupt. */
        HS_MMCSD_INTR_MASK_BUFRDRDY = CSL_MMCHS_IE_BRR_ENABLE_MASK,
        /**< Buffer read ready interrupt. */
        HS_MMCSD_INTR_MASK_BUFWRRDY = CSL_MMCHS_IE_BWR_ENABLE_MASK,
        /**< Buffer write ready interrupt. */
        HS_MMCSD_INTR_MASK_TRNFCOMP = CSL_MMCHS_IE_TC_ENABLE_MASK,
        /**< Transfer completed signal interrupt. */
        HS_MMCSD_INTR_MASK_CMDCOMP = CSL_MMCHS_IE_CC_ENABLE_MASK
        /**< Command completed signal interrupt. */
    } hsMmcsdIntrMask_t;

    /** \brief Structure holding the hs mmcsd controller system configurations. */
    typedef struct hsMmcsdSysCfg
    {
        uint32_t clockActivity;
        /**< Configuration of clock activity during wake up period. This can take
         values from the following enum #hsMmcsdClkAct_t. */
        uint32_t standbyMode;
        /**< standby mode configuration. This can take values from the following
         enum #hsMmcsdStandbyMode_t. */
        uint32_t idleMode;
        /**< Idle mode configuration. This can take values from the following
         enum #hsMmcsdIdleMode_t. */
        uint32_t enableWakeup;
        /**< Wake up enable/disable control. This can take following two values
         - TRUE  - Enable Wakeup
         - FALSE - Disable wakeup */
        uint32_t enableAutoIdle;
        /**< Auto idle enable/disable control. This can take following two values
         - TRUE  - Enable auto idle.
         - FALSE - Disable auto idle. */
    } hsMmcsdSysCfg_t;

    /**
 * \brief Structure for MMCSD command.
 */
    typedef struct hsMmcsdCmd
    {
        uint32_t cmdId;
        /**< Command ID uses standard MMCSD card commands. */
        uint32_t cmdType;
        /**< Command type uses #hsMmcsdCmdType_t. */
        uint32_t rspType;
        /**< Response type uses #hsMmcsdRspType_t. */
        uint32_t xferType;
        /**< Command direction uses #hsMmcsdXferType_t. */
    } hsMmcsdCmd_t;

    /** \brief Structure holding the hs mmcsd controller command object. */
    typedef struct hsMmcsdCmdObj
    {
        hsMmcsdCmd_t cmd;
        /**< Command to be passed to the controller/card. */
        uint32_t cmdArg;
        /**< Argument for the command. */
        uint32_t enableData;
        /**< This flag enables/disables data transfer. This can take following
         two values:
         - TRUE  - Enable data transfer
         - FALSE - Disable data transfer. */
        uint32_t numBlks;
        /**< Transfer data length in number of blocks (multiple of BLEN). This can
         take values in the following range:
         1 <= numBlks <= 65535. Value of 0 means Stop count. */
        uint32_t enableDma;
        /**< This flag enables/disables Dma. This can take following two values:
         - TRUE  - Enable DMA.
         - FALSE - Disable DMA. */
    } hsMmcsdCmdObj_t;

    /** \brief Structure to save the MMCSD context. */
    typedef struct hsMmcsdCtx
    {
        uint32_t capa;
        /**< Value of the capabilities register. */
        uint32_t systemConfig;
        /**< Value of the system configuration register. */
        uint32_t ctrlInfo;
        /**< Value of the control information register. */
        uint32_t hostCtrl;
        /**< Value of the host control register. */
        uint32_t sysCtrl;
        /**< Value of the system control register. */
        uint32_t pState;
        /**< Value of the present state register. */
    } hsMmcsdCtx_t;

    /* ========================================================================== */
    /*                         Global Variables Declarations                      */
    /* ========================================================================== */

    /* None */

    /* ========================================================================== */
    /*                         Function Declarations                              */
    /* ========================================================================== */

    /**
 * \brief   This API performs soft reset of MMC/SD controller. It also waits
 *          until the reset process is complete.
 *
 * \param   baseAddr      Base Address of the MMC/SD controller Registers.
 */
    int32_t HSMMCSDSoftReset(uint32_t baseAddr);

    /**
 * \brief   This API performs the soft reset of the MMC/SD controller lines.
 *          It also waits until the reset process is complete.
 *
 * \param   baseAddr      Base Address of the MMC/SD controller Registers.
 * \param   resetMask     Mask indicating the lines to be reset. This can take
 *                        values from the following enum
 *                        #hsMmcsdResetLineMask_t.
 */
    void HSMMCSDLinesReset(uint32_t baseAddr, uint32_t resetMask);

    /**
 * \brief   This API performs the system configurations of the MMC/SD controller
 *          like standby, idle and wake up modes.
 *
 * \param   baseAddr Base Address of the MMC/SD controller Registers.
 * \param   pCfg     Pointer to the structure #hsMmcsdSysCfg_t which contains
 *                   system configuration parameters.
 */
    void HSMMCSDSystemConfig(uint32_t baseAddr, hsMmcsdSysCfg_t *pCfg);

    /**
 * \brief   This API configures the MMC/SD controller bus width.
 *
 * \param   baseAddr  Base Address of the MMC/SD controller Registers.
 * \param   width     SD/MMC bus width. This can take values from the
 *                    following enum #hsMmcsdBusWidth_t.
 */
    void HSMMCSDSetBusWidth(uint32_t baseAddr, uint32_t width);

    /**
 * \brief   This API configures the MMC/SD bus voltage.
 *
 * \param   baseAddr   Base Address of the MMC/SD controller Registers.
 * \param   voltage    SD/MMC bus voltage. This can take values from the
 *                     following enum #hsMmcsdBusVolt_t.
 */
    void HSMMCSDSetBusVolt(uint32_t baseAddr, uint32_t voltage);

    /**
 * \brief   This API turns MMC/SD bus power on / off.
 *
 * \param   baseAddr      Base Address of the MMC/SD controller Registers.
 * \param   pwrCtrl       power on / off setting. This can take values from
 *                        the following enum #hsMmcsdPwrCtrl_t.
 *
 * \retval  TRUE          Operation succeeded.
 * \retval  FALSE         Operation failed.
 */
    uint32_t HSMMCSDBusPowerOnCtrl(uint32_t baseAddr, uint32_t pwrCtrl);

    /**
 * \brief   This API controls the enable/disable of internal clocks.
 *
 * \param   baseAddr      Base Address of the MMC/SD controller Registers.
 * \param   enableIntClk  Flag controlling internal clocks enable/disable. This
 *                        parameter can take following values:
 *                        - TRUE   - Turn on internal clocks.
 *                        - FALSE  - Turn off internal clocks.
 *
 * \retval  0    Operation succeeded.
 * \retval  -1   Operation failed.
 */
    int32_t HSMMCSDIntClockEnable(uint32_t baseAddr, uint32_t enableIntClk);

    /**
 * \brief   This API gets the internal clock stable status.
 *
 * \param   baseAddr      Base Address of the MMC/SD controller Registers.
 * \param   retry         retry times to poll for stable.
 *
 * \retval  1 Clock is stable.
 * \retval  0 Clock is not stable.
 *
 * \note    If retry is zero the status is not polled. If it is non-zero status
 *          is polled for retry times
 */
    uint32_t HSMMCSDIsIntClockStable(uint32_t baseAddr, uint32_t retry);

    /**
 * \brief   This API sets the supported bus voltages.
 *
 * \param   baseAddr   Base Address of the MMC/SD controller Registers.
 * \param   voltMask   Mask specifying supported bus voltages. This value can
 *                     take one or combination(OR of) of macros from the
 *                     following enum #hsMmcsdSuppVolt_t.
 */
    void HSMMCSDSetSupportedVoltage(uint32_t baseAddr, uint32_t voltMask);

    /**
 * \brief   This API checks if the controller supports high speed.
 *
 * \param   baseAddr      Base Address of the MMC/SD controller Registers.
 *
 * \retval  0 High speed is not supported
 * \retval  1 High speed is supported
 */
    uint32_t HSMMCSDIsHighSpeedSupported(uint32_t baseAddr);

    /**
 * \brief   This API sets data time out value. This value determines the
 *          interval by which mmc_dat lines time outs are detected.
 *
 * \param   baseAddr      Base Address of the MMC/SD controller Registers.
 * \param   timeout       the data time out value. Timeout value is the
 *                        exponential of 2, as mentioned in the controller
 *                        reference manual.
 *                        This parameter can take values in the following
 *                        range:
 *                        - 13 <= timeout <= 27
 */
    void HSMMCSDSetDataTimeout(uint32_t baseAddr, uint32_t timeout);

    /**
 * \brief   This API sets output bus frequency.
 *
 * \param   baseAddr      Base Address of the MMC/SD controller Registers.
 * \param   inputFreq     The input/ref frequency to the controller
 * \param   outputFreq    The required output frequency on the bus
 * \param   bypass        Flag controlling bypassing of reference clock. This
 *                        can take following two values.
 *                        - TRUE  - Reference clock is bypassed.
 *                        - FALSE - Reference clock is not bypassed.
 *
 * \retval  0   Frequency set operation is successful.
 * \retval  -1  Frequency set operation is failed.
 *
 * \note    If the clock is set properly, the clocks are enabled to the
 *          card with the return of this function.
 */
    int32_t HSMMCSDSetBusFreq(uint32_t baseAddr,
                              uint32_t inputFreq,
                              uint32_t outputFreq,
                              uint32_t bypass);

    /**
 * \brief   This API sends INIT stream to the card.
 *
 * \param   baseAddr    Base Address of the MMC/SD controller Registers.
 *
 * \retval   0  If INIT command could be initiated
 * \retval   -1 If INIT command could not be completed/initiated
 */
    int32_t HSMMCSDInitStreamSend(uint32_t baseAddr);

    /**
 * \brief   This API enables the controller events to set flags in status
 *          register.
 *
 * \param   baseAddr    Base Address of the MMC/SD controller Registers.
 * \param   intMask     Mask specifying the events that has to be enabled. This
 *                      takes one or combination(OR of) macros that are present
 *                      in the enum #hsMmcsdIntrMask_t.
 *
 * \note    This function only enables the reflection of events in status
 *          register. To enable events to generate a h/w interrupt request
 *          use HSMMCSDIntrEnable().
 */
    void HSMMCSDIntrStatusEnable(uint32_t baseAddr, uint32_t intrMask);

    /**
 * \brief   This API disables the controller events to set flags in status
 *          register.
 *
 * \param   baseAddr    Base Address of the MMC/SD controller Registers.
 * \param   intrMask    Mask specifying the events that has to to disabled. This
 *                      takes one or combination(OR of) macros that are present
 *                      in the enum #hsMmcsdIntrMask_t.
 *
 * \note    This function only disables the reflection of events in status
 *          register. To disable events to generate a h/w interrupt request
 *          HSMMCSDIntrDisable().
 */
    void HSMMCSDIntrStatusDisable(uint32_t baseAddr, uint32_t intrMask);

    /**
 * \brief   This API enables the controller events to generate a h/w interrupt
 *          request. This API internally enables the flags to set the status
 *          bits.
 *
 * \param   baseAddr    Base Address of the MMC/SD controller Registers.
 * \param   intrMask    Mask specifying the events for which interrupts have to
 *                      be enabled. This takes one or combination(OR of) macros
 *                      that are present in the enum #hsMmcsdIntrMask_t. *
 */
    void HSMMCSDIntrEnable(uint32_t baseAddr, uint32_t intrMask);

    /**
 * \brief   This API disables the controller events to generate a h/w interrupt
 *          request.
 *
 * \param   baseAddr    Base Address of the MMC/SD controller Registers.
 * \param   intrMask    Mask specifying the events for which interrupts have to
 *                      be disabled. This takes one or combination(OR of) macros
 *                      that are present in the enum #hsMmcsdIntrMask_t.
 */
    void HSMMCSDIntrDisable(uint32_t baseAddr, uint32_t intrMask);

    uint32_t HSMMCSDIntrGet(uint32_t baseAddr);

    /**
 * \brief   This API gets the status of the hardware interrupts.
 *
 * \param   baseAddr    Base Address of the MMC/SD controller Registers.
 *
 * \retval  intrStatus  Status of all the interrupts of mmcsd controller.
 *
 * \note    To get the status of the specific interrupts, the return value
 *          should be OR ed with appropriate macros present in the following
 *          enum #hsMmcsdIntrMask_t.
 */
    uint32_t HSMMCSDIntrStatus(uint32_t baseAddr);

    /**
 * \brief   This API clears the interrupt status bits of the controller.
 *
 * \param   baseAddr    Base Address of the MMC/SD controller Registers.
 * \param   intrMask    Mask specifying the interrupts for which status has to
 *                      be cleared. This takes one or combination(OR of) macros
 *                      that are present in the enum #hsMmcsdIntrMask_t.
 */
    void HSMMCSDIntrClear(uint32_t baseAddr, uint32_t intrMask);

    /**
 * \brief    This API checks if the command is complete.
 *
 * \param    baseAddr    Base Address of the MMC/SD controller Registers
 * \param    retry       retry times to poll for completion
 *
 * \retval   1 If the command is complete.
 * \retval   0 If the command is not complete.
 */
    uint32_t HSMMCSDIsCmdComplete(uint32_t baseAddr, uint32_t retry);

    /**
 * \brief    This API checks if the transfer is complete.
 *
 * \param    baseAddr    Base Address of the MMC/SD controller Registers
 * \param    retry       retry times to poll for completion
 *
 * \retval   1 If the transfer is complete.
 * \retval   0 If the transfer is not complete.
 **/
    uint32_t HSMMCSDIsXferComplete(uint32_t baseAddr, uint32_t retry);

    /**
 * \brief    This API sets the block length/size (in bytes) for data transfer.
 *
 * \param    baseAddr    Base Address of the MMC/SD controller Registers
 * \param    blkLen      Block length in bytes. This parameter accepts the
 *                       values in the following range:
 *                       1 <= blkLen <= 2048.
 *
 * \note     blkLen = 0 configures no data transfer.
 */
    void HSMMCSDSetBlkLength(uint32_t baseAddr, uint32_t blkLen);

    /**
 * \brief    This API sets the block length/size (in bytes) for data transfer.
 *
 * \param    baseAddr    Base Address of the MMC/SD controller Registers
 * \param    blkLen      Block length in bytes. This parameter accepts the
 *                       values in the following range:
 *                       1 <= blkLen <= 2048.
 *
 * \note     blkLen = 0 configures no data transfer.
 */
    uint32_t HSMMCSDGetBlkLength(uint32_t baseAddr);

    /**
 * \brief   This command passes the MMC/SD command to the controller/card.
 *
 * \param   baseAddr    Base Address of the MMC/SD controller Registers
 * \param   pObj        Pointer to the object #hsMmcsdCmdObj_t which contains
 *                      command configuration parameters.
 *
 * \note    Please use HS_MMCSD_CMD(cmd, type, restype, rw) to form command
 */
    void HSMMCSDCommandSend(uint32_t baseAddr, hsMmcsdCmdObj_t *pObj);

    /**
 * \brief    This API gets the command response from the controller.
 *
 * \param    baseAddr    Base Address of the MMC/SD controller Registers
 * \param    pRsp        pointer to buffer which is to be filled with the
 *                       response.
 *
 * \note    Please find below notes:
 *          -# This function shall return the values from all response
 *             registers. Hence, pRsp, must be a pointer to memory which can
 *             hold max response length. It is the responsibility of the caller
 *             to use only the required/relevant parts of the response.
 *          -# Max response length =  4.
 */
    void HSMMCSDGetResponse(uint32_t baseAddr, uint32_t *pRsp);

    /**
 * \brief    This API gets the data from the card.
 *
 * \param    baseAddr  Base Address of the MMC/SD controller Registers
 * \param    pData     Buffer into which data has to be read.
 * \param    len       Number of bytes to read.
 */
    void HSMMCSDGetData(uint32_t baseAddr, uint8_t *pData, uint32_t len);
    void HSMMCSDSetData(uint32_t baseAddr, uint8_t *pData, uint32_t len);

    /**
 * \brief    This API checks if the card is inserted and detected.
 *
 * \param    baseAddr    Base Address of the MMC/SD controller Registers
 *
 * \retval   0  if the card is not inserted and detected.
 * \retval   1  if the card is inserted and detected.
 *
 * \note     This functional may not be available for all instances of the
 *           controller. This function, is only useful if the controller has
 *           a dedicated card detect pin. If not, the card detection mechanism
 *           is application implementation specific.
 */
    uint32_t HSMMCSDIsCardInserted(uint32_t baseAddr);

    /**
 * \brief    This API checks if the card is write protected.
 *
 * \param    baseAddr    Base Address of the MMC/SD controller Registers
 *
 * \retval   0  if the card is not write protected.
 * \retval   1  if the card is write protected.
 *
 * \note     This functional may not be available for all instances of the
 *           controller. This function, is only useful if the controller has a
 *           dedicated write protect detect pin. If not, the write protect
 *           detection mechanism is application implementation specific.
 */
    uint32_t HSMMCSDIsCardWriteProtected(uint32_t baseAddr);

    /**
 * \brief   This API can be used to save the register context of MMCSD
 *
 * \param   mmcsdBase     Base address of MMCSD instance
 * \param   pCtx          Pointer to the structure #hsMmcsdCtx_t where MMCSD
 *                        register context need to be saved.
 */
    void HSMMCSDContextSave(uint32_t mmcsdBase, hsMmcsdCtx_t *pCtx);

    /**
 * \brief   This API can be used to restore the register context of MMCSD
 *
 * \param   mmcsdBase     Base address of MMCSD instance
 * \param   pCtx          Pointer to the structure #hsMmcsdCtx_t where MMCSD
 *                        register context need to be saved.
 */
    void HSMMCSDContextRestore(uint32_t mmcsdBase, hsMmcsdCtx_t *pCtx);

    /* ========================================================================== */
    /*                   Deprecated Function Declarations                         */
    /* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef HS_MMCSD_H_ */
