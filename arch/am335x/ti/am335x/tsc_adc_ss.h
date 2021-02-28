/**
 * \file       tsc_adc_ss.h
 *
 * \brief      This file contains the function prototypes for the device
 *             abstraction layer for TSC_ADC_SS IP. It also contains necessary
 *             structure, enum and macro definitions.
 *
 * \details    Programming sequence of TSC_ADC_SS is as follows:
 *             -# Enable the clocks for AFE by calling the API
 *                TSCADCClkDivConfig.
 *             -# Touchscreen configurations.
 *                -# Configure the idle step for touchscreen by calling the API
 *                   TSCADCTsIdleStepConfig.
 *                -# Configure the charge step for touchscreen by calling the
 *                   API TSCADCTsChargeStepConfig.
 *                -# Configure the charge step open delay for touchscreen by
 *                   calling the API TSCADCTsSetChargeStepDelay.
 *             -# Configure the Hardware event to either external hardware event
 *                or Pen touch IRQ by calling the API TSCADCSetHwEventMap.
 *             -# Configure the mode to either 4-wire, 5-wire or 8-wire general
 *                purpose ADC mode using the API TSCADCSetMode.
 *             -# Save the Step ID of a step into the FIFO by calling the
 *                API TSCADCStepIdTagEnable.
 *             -# Write protect the Step configuration registers by calling
 *                the API TSCADCStepConfigWrProtectEnable.
 *             -# Configure the steps by calling the API TSCADCStepConfig.
 *             -# Configure the FIFO for a step by calling the API
 *                TSCADCStepFifoConfig.
 *             -# Configure the mode for a step by calling the API
 *                TSCADCStepMode.
 *             -# Configure the number of samples per step by calling the
 *                API TSCADCStepSamplesAvg.
 *             -# Configure the open delay and sample delay for a step by
 *                calling the API TSCADCSetStepDelay.
 *             -# Configure the FIFO threshold level by calling the API
 *                TSCADCFifoThresholdConfig.
 *             -# Interrupts can be enabled by calling the API
 *                TSCADCIntrEnable.
 *             -# Enable the module by calling the API TSCADCEnable.
 *             -# Enable the steps by calling the API TSCADCStepEnable.
 *             -# Interrupt status can be read using the API TSCADCIntrStatus.
 *             -# Interrupt status can be cleared using the API TSCADCIntrClear.
 *             -# Words present in the FIFO can be read by calling the API
 *                TSCADCGetFifoWordCount.
 *             -# Read FIFO data by calling the API TSCADCGetFifoData.
 *
 */

/**
 * \copyright  Copyright (C) 2013 Texas Instruments Incorporated -
 *             http://www.ti.com/
 */

/**
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

#ifndef TSC_ADC_SS_H_
#define TSC_ADC_SS_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "hw_tsc_adc_ss.h"
#include "misc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*              API compatibility with Starterware 2.0                        */
/* ========================================================================== */
#define TSCADCTSChargeStepOpenDelayConfig    TSCADCTsSetChargeStepDelay
#define TSCADCBiasConfig                     TSCADCSetBias
#define TSCADCIntStatus                      TSCADCIntrStatus
#define TSCADCEventInterruptEnable           TSCADCIntrEnable
#define TSCADCHWEventMapSet                  TSCADCSetHwEventMap
#define TSCADCEventInterruptDisable          TSCADCIntrDisable
#define TSCADCTSModeConfig                   TSCADCSetMode
#define TSCADCSetADCPowerDown                TSCADCAfePowerDown
#define TSCADCSetADCPowerUp                  TSCADCAfePowerUp
#define TSCADCIdleModeSet                    TSCADCSetIdleMode
#define TSCADCRawIntStatusSet                TSCADCIntrTrigger
#define TSCADCWakeUpPenEventConfig           TSCADCTsWakeUpPenEventEnable
#define TSCADCDMAFIFOEnable                  TSCADCDmaFifoEnable
#define TSCADCConfigHWEventPrempt            TSCADCHwPreemptEnable
#define TSCADCOutputRangeConfig              TSCADCSetRange
#define TSCADCConfigureMisc                  TSCADCMiscConfig
#define TSCADCConfigureStepEnable            TSCADCStepEnable
#define TSCADCFIFOChannelIDRead              TSCADCGetFifoStepId
#define TSCADCFIFOADCDataRead                TSCADCGetFifoData
#define TSCADCFIFOWordCountRead              TSCADCGetFifoWordCount
#define TSCADCFIFOIRQThresholdLevelConfig    TSCADCStepFifoConfig
#define TSCADCFIFODMAThresholdLevelConfig    TSCADCStepFifoConfig
#define TSCADCStepConfigProtectionEnable     TSCADCStepConfigWrProtectEnable
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief    Values used to check the TSCADC revision.
 */
typedef enum tscAdcRevisionMask
{
    TSCADC_REVISION_MASK_Y_MINOR = ADC0_REVISION_Y_MINOR_MASK,
    /**< TSCADC minor revision. */
    TSCADC_REVISION_MASK_CUSTOM = ADC0_REVISION_CUSTOM_MASK,
    /**< TSCADC custom revision. */
    TSCADC_REVISION_MASK_X_MAJOR = ADC0_REVISION_X_MAJOR_MASK,
    /**< TSCADC major revision. */
    TSCADC_REVISION_MASK_R_RTL = ADC0_REVISION_R_RTL_MASK,
    /**< TSCADC RTL revision. */
    TSCADC_REVISION_MASK_FUNC = ADC0_REVISION_FUNC_MASK,
    /**< TSCADC Functional number. */
    TSCADC_REVISION_MASK_SCHEME = ADC0_REVISION_SCHEME_MASK
    /**< TSCADC SCHEME. */
} tscAdcRevision_t;

/**
 *  \brief    Values used to map the HW event.
 */
typedef enum tscAdcHwEventMap
{
    TSCADC_HW_EVENT_MAP_HW_INPUT =
                             ADC0_CTRL_HW_EVT_MAPPING_HWEVTINPUT,
    /**< TSCADC map Hw event to Hw event input. For e.g. input can be
         from Timer.*/
    TSCADC_HW_EVENT_MAP_PEN_TOUCH =
                             ADC0_CTRL_HW_EVT_MAPPING_PENTOUCHIRQ
    /**< TSCADC map Hw event to Pen touch Irq. */
} tscAdcHwEventMap_t;

/**
 *  \brief    Values used to configure the touch screen
 *            mode.
 */
typedef enum tscAdcMode
{
    TSCADC_MODE_GP_ADC = 0U,
    /**< TSCADC general purpose ADC mode. */
    TSCADC_MODE_FOUR_WIRE = 1U,
    /**< TSCADC 4 wire TS mode. */
    TSCADC_MODE_FIVE_WIRE = 2U,
    /**< TSCADC 5 wire TS mode. */
} tscAdcMode_t;

/**
 *  \brief    Values used to check the ADC status.
 */
typedef enum tscAdcStatusMask
{
    TSCADC_STATUS_MASK_STEP_ID = ADC0_ADCSTAT_STEP_ID_MASK,
    /**< TSCADC Step ID status. */
    TSCADC_STATUS_MASK_FSM_BUSY = ADC0_ADCSTAT_FSM_BUSY_MASK,
    /**< TSCADC FSM busy status. */
    TSCADC_STATUS_MASK_PEN_IRQ0 = ADC0_ADCSTAT_PEN_IRQ0_MASK,
    /**< TSCADC PEN IRQ0/AN0 status. */
    TSCADC_STATUS_MASK_PEN_IRQ1 = ADC0_ADCSTAT_PEN_IRQ1_MASK
    /**< TSCADC PEN IRQ1/AN4 status. */
} tscAdcStatusMask_t;

/**
 *  \brief    Values used for interrupt type.
 */
typedef enum tscAdcIntrMask
{
    TSCADC_INTR_MASK_HW_PEN_EVENT_ASYNC =
                                ADC0_IRQSTS_HW_PEN_EVT_ASYNCHRONOUS_MASK,
    /**< TSCADC Hardware pen event async type interrupt. */
    TSCADC_INTR_MASK_END_OF_SEQUENCE =
                                ADC0_IRQSTS_END_OF_SEQUENCE_MASK,
    /**< TSCADC End of sequence interrupt. */
    TSCADC_INTR_MASK_FIFO0_THRESHOLD =
                                ADC0_IRQSTS_FIFO0_THR_MASK,
    /**< TSCADC Fifo 0 threshold interrupt. */
    TSCADC_INTR_MASK_FIFO0_OVERRUN = ADC0_IRQSTS_FIFO0_OVERRUN_MASK,
    /**< TSCADC Fifo 0 overrun interrupt. */
    TSCADC_INTR_MASK_FIFO0_UNDERFLOW =
                                     ADC0_IRQSTS_FIFO0_UNDERFLOW_MASK,
    /**< TSCADC Fifo 0 underflow interrupt. */
    TSCADC_INTR_MASK_FIFO1_THRESHOLD = ADC0_IRQSTS_FIFO1_THR_MASK,
    /**< TSCADC Fifo 1 threshold interrupt. */
    TSCADC_INTR_MASK_FIFO1_OVERRUN = ADC0_IRQSTS_FIFO1_OVERRUN_MASK,
    /**< TSCADC Fifo 1 overrun interrupt. */
    TSCADC_INTR_MASK_FIFO1_UNDERFLOW =
                                     ADC0_IRQSTS_FIFO1_UNDERFLOW_MASK,
    /**< TSCADC Fifo 1 underflow interrupt. */
    TSCADC_INTR_MASK_OUT_OF_RANGE = ADC0_IRQSTS_OUT_OF_RANGE_MASK,
    /**< TSCADC out of range interrupt. */
    TSCADC_INTR_MASK_PEN_UP_EVENT = ADC0_IRQSTS_PEN_UP_EVT_MASK,
    /**< TSCADC Pen up event interrupt. */
    TSCADC_INTR_MASK_HW_PEN_EVENT_SYNC =
                                   ADC0_IRQSTS_RAW_PEN_IRQ_SYNCHRONIZED_MASK
    /**< TSCADC Hardware pen event sync interrupt. */
} tscAdcIntrMask_t;

/**
 *  \brief    Values used for idle mode.
 */
typedef enum tscAdcIdleMode
{
    TSCADC_IDLE_MODE_FORCE_IDLE = ADC0_SYSCONFIG_IDLEMODE_FORCE,
    /**< TSCADC Force idle mode. The software can use this mode to force the
         TSCADC to sleep state. */
    TSCADC_IDLE_MODE_NO_IDLE = ADC0_SYSCONFIG_IDLEMODE_NO_IDLE,
    /**< TSCADC No idle mode. This mode is used to keep the TSCADC in
         active state. */
    TSCADC_IDLE_MODE_SMART_IDLE = ADC0_SYSCONFIG_IDLEMODE_SMART_IDLE,
    /**< TSCADC Smart idle mode. The software can use this mode to put the
         TSCADC automatically to sleep state based on no activity thats
         happening. */
    TSCADC_IDLE_MODE_SMART_IDLE_WAKEUP =
                           ADC0_SYSCONFIG_IDLEMODE_SMART_IDLE_WAKEUP
    /**< TSCADC Smart idle with wakeup mode. The software can use this mode
         to put the TSCADC automatically to sleep state based on no activity
         thats happening and TSCADC has the capability to wakeup based on a
         wake event. */
} tscAdcIdleMode_t;

/**
 *  \brief    Values used to select the Fifo.
 */
typedef enum tscAdcFifoSel
{
    TSCADC_FIFO_SEL_MIN = 0U,
    /**< Minimum value of enum. */
    TSCADC_FIFO_SEL_0 = TSCADC_FIFO_SEL_MIN,
    /**< TSCADC FIFO 0. */
    TSCADC_FIFO_SEL_1 = 1U,
    /**< TSCADC FIFO 1. */
    TSCADC_FIFO_SEL_MAX = TSCADC_FIFO_SEL_1
    /**< Maximum value of enum. */
} tscAdcFifoSel_t;

/**
 *  \brief    Values used to configure ADC bias.
 */
typedef enum tscAdcBias
{
    TSCADC_BIAS_INTERNAL_AC = ADC0_CTRL_ADC_BIAS_SELECT_INTERNAL,
    /**< TSCADC Internal AC bias. */
    TSCADC_BIAS_EXTERNAL_AC = ADC0_CTRL_ADC_BIAS_SELECT_EXTERNAL
    /**< TSCADC External AC bias. */
} tscAdcBias_t;

/**
 *  \brief    Values used to configure the negative reference
 *            voltage.
 */
typedef enum tscAdcNegativeRef
{
    TSCADC_NEGATIVE_REF_VSSA,
    /**< TSCADC negative reference voltage VSSA. */
    TSCADC_NEGATIVE_REF_XNUR,
    /**< TSCADC negative reference voltage XNUR. */
    TSCADC_NEGATIVE_REF_YNLR,
    /**< TSCADC negative reference voltage YNLR. */
    TSCADC_NEGATIVE_REF_VREFN
    /**< TSCADC negative reference voltage VREFN. */
} tscAdcNegativeRef_t;

/**
 *  \brief    Values used to configure the positive reference
 *            voltage.
 */
typedef enum tscAdcPositiveRef
{
    TSCADC_POSITIVE_REF_VDDA,
    /**< TSCADC positive reference voltage VDDA. */
    TSCADC_POSITIVE_REF_XPUL,
    /**< TSCADC positive reference voltage XPUL. */
    TSCADC_POSITIVE_REF_YPLL,
    /**< TSCADC positive reference voltage YPLL. */
    TSCADC_POSITIVE_REF_VREFP
    /**< TSCADC positive reference voltage VREFP. */
} tscAdcPositiveRef_t;

/**
 *  \brief    Values used to configure the input channels
 *            to ADC.
 */
typedef enum tscAdcInput
{
    TSCADC_INPUT_MIN = (0U),
    /**< Minimum value of enum. */
    TSCADC_INPUT_CHANNEL1 = TSCADC_INPUT_MIN,
    /**< TSCADC input channel 1. */
    TSCADC_INPUT_CHANNEL2 = (1U),
    /**< TSCADC input channel 2. */
    TSCADC_INPUT_CHANNEL3 = (2U),
    /**< TSCADC input channel 3. */
    TSCADC_INPUT_CHANNEL4 = (3U),
    /**< TSCADC input channel 4. */
    TSCADC_INPUT_CHANNEL5 = (4U),
    /**< TSCADC input channel 5. */
    TSCADC_INPUT_CHANNEL6 = (5U),
    /**< TSCADC input channel 6. */
    TSCADC_INPUT_CHANNEL7 = (6U),
    /**< TSCADC input channel 7. */
    TSCADC_INPUT_CHANNEL8 = (7U),
    /**< TSCADC input channel 8. */
    TSCADC_INPUT_VREFN = (8U),
    /**< Route VREFN signal. */
    TSCADC_INPUT_MAX = TSCADC_INPUT_VREFN
    /**< Maximum value of enum. */
} tscAdcInput_t;

/**
 *  \brief    Values used to configure the average samples.
 */
typedef enum tscAdcSamplesAvg
{
    TSCADC_SAMPLES_AVG_MIN = ADC0_STEPCONFIG_AVERAGING_NOAVG,
    /**< Minimum value of enum. */
    TSCADC_SAMPLES_NO_AVG = TSCADC_SAMPLES_AVG_MIN,
	/**< No average. */
    TSCADC_SAMPLES_AVG_2 = ADC0_STEPCONFIG_AVERAGING_SAMPLESAVG_2,
    /**< Average 2 samples. */
    TSCADC_SAMPLES_AVG_4 = ADC0_STEPCONFIG_AVERAGING_SAMPLESAVG_4,
    /**< Average 4 samples. */
    TSCADC_SAMPLES_AVG_8 = ADC0_STEPCONFIG_AVERAGING_SAMPLESAVG_8,
    /**< Average 8 samples. */
    TSCADC_SAMPLES_AVG_16 = ADC0_STEPCONFIG_AVERAGING_SAMPLESAVG_16,
    /**< Average 16 samples. */
    TSCADC_SAMPLES_AVG_MAX = TSCADC_SAMPLES_AVG_16
    /**< Maximum value of enum. */
} tscAdcSamplesAvg_t;

/**
 *  \brief    Values used to configure the ADC step mode.
 */
typedef enum tscAdcStepMode
{
    TSCADC_STEP_MODE_SW_ENABLED_ONE_SHOT =
                                 ADC0_STEPCONFIG_MODE_SW_EN_ONESHOT,
    /**< Configure ADC in software enabled with one shot mode. */
    TSCADC_STEP_MODE_SW_ENABLED_CONTINUOUS =
                                 ADC0_STEPCONFIG_MODE_SW_EN_CONTINUOUS,
    /**< Configure ADC in software enabled with continuous mode. */
    TSCADC_STEP_MODE_HW_SYNC_ONE_SHOT =
                                 ADC0_STEPCONFIG_MODE_HW_SYNC_ONESHOT,
    /**< Configure ADC in Hw sync with one shot mode. */
    TSCADC_STEP_MODE_HW_SYNC_CONTINUOUS =
                                 ADC0_STEPCONFIG_MODE_HW_SYNC_CONTINUOUS
    /**< Configure ADC in Hw sync with continuous mode. */
} tscAdcStepMode_t;

/**
 *  \brief    Structure holding the step configuration parameters.
 */
typedef struct tscAdcStepCfg
{
    uint32_t adcNegativeInpRef;
    /**< Program the SEL_RFM bits to configure the negative reference voltage
         mux. Can take values from the enum #tscAdcNegativeRef_t. */
    uint32_t adcPositiveInpRef;
    /**< Program the SEL_RFP bits to configure the positive reference voltage
         mux. Can take values from the enum #tscAdcPositiveRef_t. */
    uint32_t adcNegativeInpChan;
    /**< Program the SEL_INM bits to configure the negative input channel
         mux. Can take values from the follwoing enum #tscAdcInput_t. */
    uint32_t adcPositiveInpChan;
    /**< Program the SEL_INP bits to configure the positive input channel
         mux. Can take values from the enum #tscAdcInput_t. */
    uint32_t enableXppsw;
    /**< Can take values TRUE/FALSE to enable/disable respectively. */
    uint32_t enableXnpsw;
    /**< Can take values TRUE/FALSE to enable/disable respectively. */
    uint32_t enableYppsw;
    /**< Can take values TRUE/FALSE to enable/disable respectively. */
    uint32_t enableXnnsw;
    /**< Can take values TRUE/FALSE to enable/disable respectively. */
    uint32_t enableYpnsw;
    /**< Can take values TRUE/FALSE to enable/disable respectively. */
    uint32_t enableYnnsw;
    /**< Can take values TRUE/FALSE to enable/disable respectively. */
    uint32_t enableWpnsw;
    /**< Can take values TRUE/FALSE to enable/disable respectively. */
    uint32_t fifo;
    uint32_t mode;
    uint32_t averaging;
} tscAdcStepCfg_t;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief     Read the TSCADC revision.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 *
 * \retval    tscAdcRevision Returns the revision ID. User can use the entries
 *                           from the below enum to check the status.
 *                           #tscAdcRevision_t.
 */
uint32_t TSCADCGetRevision(uint32_t baseAddr);

/**
 * \brief     Configure the clock divider.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 * \param     moduleClk      Module clock of ADC in MHz.
 * \param     afeInputClk    Input clock in MHz
 */
void TSCADCClkDivConfig(uint32_t baseAddr,
                        uint32_t moduleClk,
                        uint32_t afeInputClk);

/**
 * \brief     Map the hardware event.
 *
 * \param     baseAddr   Base Address of the TouchScreen Module Registers.
 * \param     hwEvent    Hardware event mapping.
 *            hwEvent can take values from the enum.
 *            #tscAdcHwEventMap_t.
 */
void TSCADCSetHwEventMap(uint32_t baseAddr, uint32_t hwEvent);

/**
 * \brief     Set the 4-wire, 5-wire or general purpose ADC mode.
 *
 * \param     baseAddr   Base Address of the TouchScreen Module Registers.
 * \param     mode       Configure the mode.
 *            mode can take values from the enum.
 *            #tscAdcMode_t.
 */
void TSCADCSetMode(uint32_t baseAddr, uint32_t mode);

/**
 * \brief     Configure the Step ID tag.
 *
 * \param     baseAddr          Base Address of the TouchScreen Module
 *                              Registers.
 * \param     enableStepIdTag   Enable/disable the step Id tag.
 *            enableStepIdTag can take the following values.
 *            -# TRUE - Enable the step id tag.
 *            -# FALSE - Disable the step id tag.
 */
void TSCADCStepIdTagEnable(uint32_t baseAddr, uint32_t enableStepIdTag);

/**
 * \brief     Write protect the step configuration registers.
 *
 * \param     baseAddr         Base Address of the TouchScreen Module Registers.
 * \param     enableWrProtect  Enable/disable write protect.
 *            enableWrProtect can take the following values.
 *            -# TRUE - Enable write protect.
 *            -# FALSE - Disable write protect.
 */
void TSCADCStepConfigWrProtectEnable(uint32_t baseAddr,
                                     uint32_t enableWrProtect);

/**
 * \brief     Configure the Idle step for touchscreen.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     enableDiffCtrl  Enable/Disable differential control.
 *            enableDiffCtrl can take the following values.
 *            -# TRUE - Enable differential control.
 *            -# FALSE - Disable differential control.
 * \param     pStepCfg        Pointer to the following structure which contains
 *                            the step configuration parameters for ADC.
 *                            - #tscAdcStepCfg_t.
 */
void TSCADCTsIdleStepConfig(uint32_t baseAddr,
                            uint32_t enableDiffCtrl,
                            tscAdcStepCfg_t *pStepCfg);

/**
 * \brief     Configure the Charge step for touchscreen.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     enableDiffCtrl  Enable/Disable differential control.
 *            enableDiffCtrl can take the following values.
 *            -# TRUE - Enable differential control.
 *            -# FALSE - Disable differential control.
 * \param     pStepCfg        Pointer to the following structure which contains
 *                            the step configuration parameters for ADC.
 *                            - #tscAdcStepCfg_t.
 */
void TSCADCTsChargeStepConfig(uint32_t baseAddr,
                              uint32_t enableDiffCtrl,
                              tscAdcStepCfg_t *pStepCfg);

/**
 * \brief     Configure the charge step open delay.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     openDelay       Open delay value to be configured.
 */
void TSCADCTsSetChargeStepDelay(uint32_t baseAddr, uint32_t openDelay);

/**
 * \brief     Configure the step for touchscreen.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     stepNum         Step number to be configured.
 *            where 1 <= stepNum <= 16.
 * \param     enableDiffCtrl  Enable/Disable differential control.
 *            enableDiffCtrl can take the following values.
 *            -# TRUE - Enable differential control.
 *            -# FALSE - Disable differential control.
 * \param     pStepCfg        Pointer to the following structure which contains
 *                            the step configuration parameters for ADC.
 *                            - #tscAdcStepCfg_t.
 */
void TSCADCStepConfig(uint32_t baseAddr,
                      uint32_t stepNum,
                      uint32_t enableDiffCtrl,
                      tscAdcStepCfg_t *pStepCfg);

/**
 * \brief     Configure FIFO
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     fifoSel         Select the FIFO to be configured.
 *            fifoSel can take the values from the enum.
 *            #tscAdcFifoSel_t.
 * \param     sampleLvl      FIFO sample threshold level.
 * \param     enableIrq      Enable/Disable IRQ.
 *            enableIrq can take the following values.
 *            -# TRUE - Enable IRQ request.
 *            -# FALSE - Enable DMA request.
 */
void TSCADCFifoConfig(uint32_t baseAddr,
                          uint32_t fifoSel,
                          uint32_t sampleLvl,
                          uint32_t enableIrq);

/**
 * \brief     Configure the mode for Step.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     stepNum         Step number to be configured.
 *            where 1 <= stepNum <= 16.
 * \param     stepMode        Configure the step mode.
 *            stepMode can take the values from the enum.
 *            #tscAdcStepMode_t.
 */
void TSCADCStepMode(uint32_t baseAddr, uint32_t stepNum, uint32_t stepMode);

/**
 * \brief     Configure the number of samples to be averaged.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     stepNum         Step number to be configured.
 *            where 1 <= stepNum <= 16.
 * \param     avgConfig       Number of samples to be averaged.
 *            avgConfig can take the values from the enum.
 *            #tscAdcSamplesAvg_t.
 */
void TSCADCStepSamplesAvg(uint32_t baseAddr,
                          uint32_t stepNum,
                          uint32_t avgConfig);

/**
 * \brief     Configure the sample and open delay for the Step.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     stepNum         Step number to be configured.
 *            where 1 <= stepNum <= 16.
 * \param     sampleDelay     Configure the sample delay.
 * \param     openDelay       Configure the open delay.
 */
void TSCADCSetStepDelay(uint32_t baseAddr,
                        uint32_t stepNum,
                        uint32_t sampleDelay,
                        uint32_t openDelay);

/**
 * \brief     Configure the FIFO threshold level to generate interrupt/dma
 *            request.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 * \param     fifoSel        FIFO whose threshold level needs to be configured.
 *            fifoSel can take the values from the enum.
 *            #tscAdcFifoSel_t.
 * \param     sampleLvl      FIFO sample threshold level.
 * \param     enableIrq      Enable/Disable IRQ.
 *            enableIrq can take the following values.
 *            -# TRUE - Enable IRQ request.
 *            -# FALSE - Enable DMA request.
 */
void TSCADCFifoThresholdConfig(uint32_t baseAddr,
                               uint32_t fifoSel,
                               uint32_t sampleLvl,
                               uint32_t enableIrq);

/**
 * \brief     Configure the spare input and output values.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     stepNum         Step number to be configured.
 *            where 1 <= stepNum <= 16.
 * \param     enableStep      Enable/Disable the step.
 *            enableStep can take the following values.
 *            -# TRUE - Enable step.
 *            -# FALSE - Disable step.
 */
void TSCADCStepEnable(uint32_t baseAddr, uint32_t stepNum, uint32_t enableStep);

/**
 * \brief     Enable/disable the DMA for the TSCADC FIFO.
 *
 * \details   This API shall enable DMA control for the ADC FIFO. Using this API
 *            the DMA can be enabled to read FIFO data when ADC saves data to
 *            the FIFO.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 * \param     fifoSel        FIFO for which DMA should be enabled.
 *            fifoSel can take the values from the enum.
 *            #tscAdcFifoSel_t.
 * \param     enableDma      Enable/disable the DMA.
 *            dmaEnable can take the following values.
 *            -# TRUE - Enable DMA.
 *            -# FALSE - Disable DMA.
 */
void TSCADCDmaFifoEnable(uint32_t baseAddr,
                         uint32_t fifoSel,
                         uint32_t enableDma);

/**
 * \brief     Power down the ADC AFE.
 *
 * \details   This API powers down the Analog front end. AFE is the analog
 *            interface block between the resistive touchscreen and the TSC
 *            digital controller.
 *
 * \param     baseAddr   Base Address of the TouchScreen Module Registers.
 */
void TSCADCAfePowerDown(uint32_t baseAddr);

/**
 * \brief     Power up the ADC AFE.
 *
 * \details   This API powers up the Analog front end. AFE is the analog
 *            interface block between the resistive touchscreen and the TSC
 *            digital controller.
 *
 * \param     baseAddr   Base Address of the TouchScreen Module Registers.
 */
void TSCADCAfePowerUp(uint32_t baseAddr);

/**
 * \brief     Enable/Disable out of range check feature.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     stepNum         Step number to be configured.
 *            where 1 <= stepNum <= 16.
 * \param     enableRangeChk  Enable/Disable out of range check feature.
 *            enableRangeChk can take the following values.
 *            -# TRUE - Enable out of range check.
 *            -# FALSE - Disable out of range check.
 */
void TSCADCStepRangeCheckEnable(uint32_t baseAddr,
                                uint32_t stepNum,
                                uint32_t enableRangeChk);

/**
 * \brief     Read the step/channel ID whose data is captured in FIFO.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 * \param     fifoSel        FIFO whose step ID has to be read.
 *            fifoSel can take the values from the enum.
 *            #tscAdcFifoSel_t.
 *
 * \retval    fifoStepId     Returns the channel ID whose data is
 *                           captured in FIFO.
 */
uint32_t TSCADCGetFifoStepId(uint32_t baseAddr, uint32_t fifoSel);

/**
 * \brief     Read the FIFO data.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 * \param     fifoSel        FIFO whose data has to be read.
 *            fifoSel can take the values from the enum.
 *            #tscAdcFifoSel_t.
 *
 * \retval    fifoData       Returns the FIFO data.
 */
uint32_t TSCADCGetFifoData(uint32_t baseAddr, uint32_t fifoSel);

/**
 * \brief     Read the number of words present in the FIFO.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 * \param     fifoSel        FIFO whose number of words present have to be read.
 *            fifoSel can take the values from the enum.
 *            #tscAdcFifoSel_t.
 *
 * \retval    fifoWordCnt    Returns the word count present in the FIFO.
 */
uint32_t TSCADCGetFifoWordCount(uint32_t baseAddr, uint32_t fifoSel);

/**
 * \brief     Clear the interrupt status.
 *
 * \param     baseAddr      Memory Address of the TSC_ADC_SS instance used.
 * \param     intrFlags     Interrupt flags.
 *            intrFlags will contain values from the enum.
 *            #tscAdcIntrMask_t.
 */
void TSCADCIntrClear(uint32_t baseAddr, uint32_t intrFlags);

/**
 * \brief     Read the interrupt status.
 *
 * \param     baseAddr      Memory Address of the TSC_ADC_SS instance used.
 *
 * \retval    tscAdcIntrSts User can use the values from the below enum to
 *                          deduce the return values
 *                          #tscAdcIntrMask_t.
 *
 * \note      This API returns the complete register value. User can use the
 *            above mentioned values to fetch the required interrupt status.
 */
uint32_t TSCADCIntrStatus(uint32_t baseAddr);

/**
 * \brief     Enable the interrupts.
 *
 * \param     baseAddr   Base Address of the TouchScreen Module Registers.
 * \param     intrFlags  Enable the interrupts represented by flags.
 *            intrFlags will contain values from the enum.
 *            #tscAdcIntrMask_t.
 */
void TSCADCIntrEnable(uint32_t baseAddr, uint32_t intrFlags);

/**
 * \brief     Disable the interrupts.
 *
 * \param     baseAddr   Base Address of the TouchScreen Module Registers.
 * \param     intrFlags  Disable the interrupts represented by flags.
 *            intrFlags will contain values from the enum.
 *            #tscAdcIntrMask_t.
 */
void TSCADCIntrDisable(uint32_t baseAddr, uint32_t intrFlags);

/**
 * \brief     Trigger the TSCADC Raw interrupts.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 * \param     intrFlags      Trigger the raw interrupts.
 *            intrFlags will contain values from the enum.
 *            #tscAdcIntrMask_t.
 */
void TSCADCIntrTrigger(uint32_t baseAddr, uint32_t intrFlags);

/**
 * \brief     Read the TSCADC interrupt Raw status.
 *
 * \param     baseAddr         Base Address of the TouchScreen Module Registers.
 *
 * \retval    tscAdcIntrRawSts Returns the status of the TSCADC Raw interrupts.
 *                             User can use the entries present in the below
 *                             enum for checking status.
 *                             #tscAdcIntrMask_t.
 */
uint32_t TSCADCIntrRawStatus(uint32_t baseAddr);

/**
 * \brief     Return the ADC status.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 *
 * \retval    tscAdcSts      Return the status of ADC. User can use the entries
 *                           present in the below enum for checking the status.
 *                           #tscAdcStatusMask_t.
 */
uint32_t TSCADCStatus(uint32_t baseAddr);

/**
 * \brief     Enable/disable the module.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 * \param     enableAdc      Enable/disable the module.
 *            enableAdc can take the following values.
 *            -# TRUE - Enable the ADC.
 *            -# FALSE - Disable the ADC.
 */
void TSCADCEnable(uint32_t baseAddr, uint32_t enableAdc);

/**
 * \brief     Configure the Idle mode for TSCADC module.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 * \param     idleMode       Idle mode.
 *            idleMode can take values from the below enum.
 *            #tscAdcIdleMode_t.
 */
void TSCADCSetIdleMode(uint32_t baseAddr, uint32_t idleMode);

/**
 * \brief     Enable/disable the TSCADC Hw pen event wakeup.
 *
 * \param     baseAddr       Base Address of the TouchScreen Module Registers.
 * \param     enableWakeup   Enable/disable wakeup on Hw pen event.
 *            enableWakeup can take the following values.
 *            -# TRUE - Enable wakeup.
 *            -# FALSE - Disable wakeup.
 */
void TSCADCTsWakeupPenEventEnable(uint32_t baseAddr, uint32_t enableWakeup);

/**
 * \brief     Enable/disable the hardware preemption.
 *
 * \details   In this module there are software and hardware steps. Using this
 *            feature if preemption is enabled then a hardware step can preempt
 *            a software step.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     enableHwPreempt Enable/Disable hardware preemption.
 *            enableHwPreempt can take the following values
 *            -# TRUE - Enable hardware preemption.
 *            -# FALSE - Disable hardware preemption.
 */
void TSCADCHwPreemptEnable(uint32_t baseAddr,
                           uint32_t enableHwPreempt);

/**
 * \brief     Set the TSCADC Bias.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     adcBiasSel      Bias to be configured.
 *            adcBiasSel can take values from the enum
 *            #tscAdcBias_t.
 */
void TSCADCSetBias(uint32_t baseAddr, uint32_t adcBiasSel);

/**
 * \brief     Configure the TSCADC range.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     lowRange        Low range value to be configured.
 * \param     highRange       High range value to be configured.
 */
void TSCADCSetRange(uint32_t baseAddr,
                    uint32_t lowRange,
                    uint32_t highRange);

/**
 * \brief     Configure the spare input and output values.
 *
 * \param     baseAddr        Base Address of the TouchScreen Module Registers.
 * \param     spareInputVal   Spare input value to be configured.
 * \param     spareOutputVal  Spare output value to be configured.
 */
void TSCADCMiscConfig(uint32_t baseAddr,
                      uint32_t spareInputVal,
                      uint32_t spareOutputVal);

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/**
 * \brief     Read the DMA enable status for the given FIFO.
 **/
DEPRECATED(uint32_t TSCADCIsDMAFIFOEnabled(uint32_t baseAddr,
                                           uint32_t fifoSel));

#ifdef __cplusplus
}
#endif

#endif /* #ifndef TSC_ADC_SS_H_ */
