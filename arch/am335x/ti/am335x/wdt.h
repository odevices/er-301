/**
 * \file       wdt.h
 *
 * \brief      This file contains the function prototypes for the device
 *             abstraction layer of Watchdog timer. It also contains necessary
 *             structure, enum and macro definitions.
 *
 * \details    Programming sequence of WDT is as follows:
 *             -# Put the WDT to reset state by calling the API WDTReset.
 *             -# Configure the Pre-scaler clock by calling the API
 *                WDTPrescalerClkEnable.
 *             -# Configure the counter value by calling the API
 *                WDTSetCounter.
 *             -# Configure the reload value by calling the API
 *                WDTSetReloadVal.
 *             -# Start/enable or Stop/Disable the WDT by calling the
 *                WDTEnable API
 *             -# To reload value from the load register into the counter
 *                register the WDTTriggerReload API has to be called.
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

#ifndef WDT_H_
#define WDT_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "hw_wdt.h"
#include "misc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*              API compatibility with Starterware 2.0                        */
/* ========================================================================== */
#define WatchdogTimerRevisionIDGet          WDTGetRevision
#define WatchdogTimerReset                  WDTReset
#define WatchdogTimerCounterSet             WDTSetCounter
#define WatchdogTimerCounterGet             WDTGetCounter
#define WatchdogTimerReloadSet              WDTSetReloadVal
#define WatchdogTimerReloadGet              WDTGetReloadVal
#define WatchdogTimerTriggerSet             WDTTriggerReload
#define WatchdogTimerDelaySet               WDTSetDelayVal
#define WatchdogTimerDelayGet               WDTGetDelayVal
#define WatchdogTimerIntRawStatusSet        WDTIntrTrigger
#define WatchdogTimerIntRawStatusGet        WDTIntrRawStatus
#define WatchdogTimerIntStatusGet           WDTIntrStatus
#define WatchdogTimerIntStatusClear         WDTIntrClear
#define WatchdogTimerIntEnable              WDTIntrEnable
#define WatchdogTimerIntDisable             WDTIntrDisable
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief    Enumerates the different prescaler values available for WDT
 *            module.
 */
typedef enum wdtPrescalerClkDiv
{
    WDT_PRESCALER_CLK_DIV_MIN,
    /**< Minimum value of the enum. */
    WDT_PRESCALER_CLK_DIV_1 = WDT_PRESCALER_CLK_DIV_MIN,
    /**< WDT functional clock divide by 1. */
    WDT_PRESCALER_CLK_DIV_2 = 0x1U,
    /**< WDT functional clock divide by 2. */
    WDT_PRESCALER_CLK_DIV_4 = 0x2U,
    /**< WDT functional clock divide by 4. */
    WDT_PRESCALER_CLK_DIV_8 = 0x3U,
    /**< WDT functional clock divide by 8. */
    WDT_PRESCALER_CLK_DIV_16 = 0x4U,
    /**< WDT functional clock divide by 16. */
    WDT_PRESCALER_CLK_DIV_32 = 0x5U,
    /**< WDT functional clock divide by 32. */
    WDT_PRESCALER_CLK_DIV_64 = 0x6U,
    /**< WDT functional clock divide by 64. */
    WDT_PRESCALER_CLK_DIV_128 = 0x7U,
    /**< WDT functional clock divide by 128. */
    WDT_PRESCALER_CLK_DIV_MAX = WDT_PRESCALER_CLK_DIV_128
    /**< Maximum value of the enum. */
} wdtPrescalerClkDiv_t;

/**
 *  \brief    Enumerates the values used to represent the WDT interrupts.
 */
typedef enum wdtIntrMask
{
    WDT_INTR_MASK_OVERFLOW = WDT_WIRQENSET_EN_OVF_MASK,
    /**< WDT overflow interrupt. */
    WDT_INTR_MASK_DELAY = WDT_WIRQENSET_EN_DLY_MASK
    /**< WDT delay interrupt. */
} wdtIntrMask_t;

/**
 *  \brief    Enumerates the supported WDT idle modes.
 */
typedef enum wdtIdleMode
{
    WDT_IDLE_MODE_FORCE_IDLE = WDT_WDSC_IDLEMODE_FORCE_IDLE,
    /**< Force Idle mode. */
    WDT_IDLE_MODE_NO_IDLE = WDT_WDSC_IDLEMODE_NO_IDLE,
    /**< No idle mode. */
    WDT_IDLE_MODE_SMART_IDLE = WDT_WDSC_IDLEMODE_SMART_IDLE,
    /**< Smart idle mode. */
    WDT_IDLE_MODE_SMART_IDLE_WAKEUP =
                                  WDT_WDSC_IDLEMODE_SMART_IDLE_WAKEUP
    /**< Smart idle with wake up mode. */
} wdtIdleMode_t;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief     This API shall enable the user to read the revision number of WDT.
 *
 * \param     baseAddr       Memory Address of the WDT instance used.
 *
 * \retval    revision       Returns the revision number of WDT.
 */
uint32_t WDTGetRevision(uint32_t baseAddr);

/**
 * \brief     This API shall perform a soft reset of the WDT.
 *
 * \param     baseAddr       Memory Address of the WDT instance used.
 */
void WDTReset(uint32_t baseAddr);

/**
 * \brief     This API shall enable(start)/disable(stop) the WDT.
 *
 * \param     baseAddr       Memory Address of the WDT instance used.
 * \param     enableWdt      Enable/disable the WDT.
 *            enableWdt will take the following values.
 *            -# TRUE - Enable/Start WDT.
 *            -# FALSE - Disable/Stop WDT.
 */
void WDTEnable(uint32_t baseAddr, uint32_t enableWdt);

/**
 * \brief     This API shall enable and configure the WDT prescaler clock
 *            value.
 *
 * \details   The clock which is given as an input to the WDT can be prescaled
 *            to the required value using the prescaler feature.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 * \param     prescaler        Prescaler value to be configured.
 *            prescaler will contain values from the following enum.
 *            #wdtPrescalerClkDiv_t
 */
void WDTPrescalerClkEnable(uint32_t baseAddr,
                           uint32_t prescaler);

/**
 * \brief     This API shall disable the WDT prescaler clock.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 */
void WDTPrescalerClkDisable(uint32_t baseAddr);

/**
 * \brief     Set the WDT counter value. When the WDT is started/enabled
 *            the count will start from the current value present in the
 *            counter. This API shall program the counter value.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 * \param     cntrVal          WDT counter value.
 */
void WDTSetCounter(uint32_t baseAddr, uint32_t cntrVal);

/**
 * \brief     This API shall enable the user to read the WDT counter value.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 *
 * \retval    counterVal       Returns the WDT counter value.
 */
uint32_t WDTGetCounter(uint32_t baseAddr);

/**
 * \brief     Set the WDT reload value.
 *
 * \details   Reload feature can be used to reload a value when a overflow
 *            occurs if reload is enabled. Also in trigger mode when a trigger
 *            happens the value present in the reload register is loaded into
 *            the counter register.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 * \param     reloadVal        WDT reload value.
 *
 * \note      It is recommended to not use reload value as 0xFFFFFFFFU.
 */
void WDTSetReloadVal(uint32_t baseAddr, uint32_t reloadVal);

/**
 * \brief     This API shall enable the user to read the WDT reload value.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 *
 * \retval    reloadVal        Returns WDT reload value.
 */
uint32_t WDTGetReloadVal(uint32_t baseAddr);

/**
 * \brief     Enable the trigger feature of WDT.
 *
 * \details   On calling this API a value is written to the trigger register
 *            which will cause the value in the Reload register to be loaded
 *            into the counter register.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 * \param     trigVal          Trigger value to be programmed.
 *
 * \note      This API should be called if the Watchdog timer WDT_WCRR
 *            register has to be loaded with the WDT_WLDR register before
 *            overflow occurs. If this API has to be called more than once in an
 *            application then ensure each time this API is called, pass a
 *            different value to the parameter 'trigVal' to ensure proper
 *            working.
 */
void WDTTriggerReload(uint32_t baseAddr, uint32_t trigVal);

/**
 * \brief     Set the WDT delay value.
 *
 * \details   In this feature a value is programmed into the delay
 *            register and if delay interrupts are enabled, when the
 *            counter value matches the value in the delay register a
 *            delay interrupt will occur.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 * \param     delayVal         Delay value to be programmed.
 *
 */
void WDTSetDelayVal(uint32_t baseAddr, uint32_t delayVal);

/**
 * \brief     This API shall enable the user to read the Delay value.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 *
 * \retval    delayVal         Returns the WDT delay value.
 */
uint32_t WDTGetDelayVal(uint32_t baseAddr);

/**
 * \brief     This API triggers an interrupt request for the specified
 *            event.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 * \param     intrMask         Interrupts to be triggered.
 *            intrMask will contain values from the following enum.
 *            #wdtIntrMask_t
 *
 * \note      Here an interrupt request is manually triggered by writing to a
 *            specified bit. As a prerequisite, the interrupt generation should
 *            be enabled for the given instance.
 */
void WDTIntrTrigger(uint32_t baseAddr, uint32_t intrMask);

/**
 * \brief     Read the raw interrupt status of WDT.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 *
 * \retval    rawIntrSts       Returns the Raw interrupt status.
 *            Application can use values from the below enum to check the
 *            status.
 *            #wdtIntrMask_t
 */
uint32_t WDTIntrRawStatus(uint32_t baseAddr);

/**
 * \brief     Read the interrupt status of WDT.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 *
 * \retval    intrSts          Returns the interrupt status.
 *            Application can use values from the below enum to check the
 *            status.
 *            #wdtIntrMask_t
 */
uint32_t WDTIntrStatus(uint32_t baseAddr);

/**
 * \brief     Clear the WDT interrupts.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 * \param     intrMask         Interrupts to be cleared.
 *            intrMask will contain values from the following enum.
 *            #wdtIntrMask_t
 */
void WDTIntrClear(uint32_t baseAddr, uint32_t intrMask);

/**
 * \brief     Enable the WDT interrupts.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 * \param     intrMask         Interrupts to be enabled.
 *            intrMask will contain values from the following enum.
 *            #wdtIntrMask_t
 */
void WDTIntrEnable(uint32_t baseAddr, uint32_t intrMask);

/**
 * \brief     Disable the WDT interrupts.
 *
 * \param     baseAddr         Memory Address of the WDT instance used.
 * \param     intrMask         Interrupts to be disabled.
 *            intrMask will contain values from the following enum.
 *            #wdtIntrMask_t
 */
void WDTIntrDisable(uint32_t baseAddr, uint32_t intrMask);

/**
 * \brief     This API configures one of the idle modes.
 *
 * \param     baseAddr         Memory address of the WDT instance used.
 * \param     idleMode         Value to determine idle mode selection
 *            idleMode can take macros from the following enum
 *            - #wdtIdleMode_t
 */
void WDTSetIdleMode(uint32_t baseAddr, uint32_t idleMode);

/**
 * \brief     Enable/Disable emulation mode of WDT.
 *
 * \details   On enabling the Emulation mode of WDT the counter will be stopped
 *            during emulation.
 *
 * \param     baseAddr         Memory address of the WDT instance used.
 * \param     enableEmuMode    Enable/disable the emulation mode.
 *            enableEmuMode will take the following values.
 *            -# TRUE - Enable emulation mode.
 *            -# FALSE - Disable emulation mode.
 */
void WDTEmuModeEnable(uint32_t baseAddr, uint32_t enableEmuMode);

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef WDT_H_ */
