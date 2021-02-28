/**
 *  \file   GPIO_v1_lld.h
 *
 *  \brief  Header file for hardware register write/read APIs.
 *
 *   This file contains the hardware register write/read APIs prototypes.
 */

/*
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef  GPIO_V1_LLD_H_
#define  GPIO_V1_LLD_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "cslr_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* ========================================================================== */
/*                             Macros & Typedefs                              */
/* ========================================================================== */

/** \brief GPIO pin is at logic low.*/
#define GPIO_PIN_LOW                     (0x0U)
/** \brief GPIO pin is at logic high.*/
#define GPIO_PIN_HIGH                    (0x1U)


/** \brief GPIO pin low level should be used for interrupt generation. */
#define GPIO_LEVEL_LOW                   (0U)
/** \brief GPIO pin high level should be used for interrupt generation. */
#define GPIO_LEVEL_HIGH                  (1U)

/** \brief Macro to configure GPIO pin as output */
#define GPIO_OE_OUTPUTEN_N_ENABLED       (0U)

/** \brief Macro to configure GPIO pin as input */
#define GPIO_OE_OUTPUTEN_N_DISABLED      (1U)

/** \brief Mask for all pins of single gpio port */
#define GPIO_PIN_MASK_ALL   0xFF

/* Interrupt line number 1 */
#define GPIO_INTR_LINE_1    (0U)

/* Interrupt line number 2 */
#define GPIO_INTR_LINE_2    (1U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/** \brief Enumerates the possible GPIO interrupt generation events. */
typedef enum gpioIntrMask
{
    GPIO_INTR_MASK_NO_LEVEL  = 0x01U,
    /**< No interrupt request on occurrence of either a
            logic LOW or a logic HIGH on the input GPIO pin. */
    GPIO_INTR_MASK_LEVEL_LOW = 0x04U,
    /**< Interrupt request on occurrence of a LOW level
            (logic 0) on the input GPIO pin. */
    GPIO_INTR_MASK_LEVEL_HIGH = 0x08U,
    /**< Interrupt request on occurrence of a HIGH level
            (logic 1) on the input GPIO pin. */
    GPIO_INTR_MASK_BOTH_LEVEL = 0x0CU,
    /**< Interrupt request on the occurrence of both the
           LOW level and HIGH level on the input GPIO pin. */
    GPIO_INTR_MASK_NO_EDGE    = 0x80U,
    /**< No interrupt request on either rising or
           falling edges on the pin. */
    GPIO_INTR_MASK_RISE_EDGE  = 0x10U,
    /**< Interrupt request on occurrence of a rising edge
           on the input GPIO pin. */
    GPIO_INTR_MASK_FALL_EDGE  = 0x20U,
    /**< Interrupt request on occurrence of a falling edge
           on the input GPIO pin. */
    GPIO_INTR_MASK_BOTH_EDGE  = 0x30U
    /**< Interrupt request on occurrence of both a rising
           and a falling edge on the pin. */
}gpioIntrMask_t;

/** \brief Enumerates the direction of GPIO pin. */
typedef enum gpioDirection
{
    GPIO_DIRECTION_INPUT  = GPIO_OE_OUTPUTEN_N_DISABLED,
    /**< Input pin. */
    GPIO_DIRECTION_OUTPUT = GPIO_OE_OUTPUTEN_N_ENABLED
    /**< Output pin. */
}gpioDirection_t;

/** \brief Enumerates the possible gating ratios of GPIO functional clock. */
typedef enum gpioClkGr
{
    GPIO_CLK_GR_FCLK_ICLK_BY_1 = CSL_GPIO_CTRL_GATINGRATIO_N_1,
    /**< Functional clock is interface clock. */
    GPIO_CLK_GR_FCLK_ICLK_BY_2 = CSL_GPIO_CTRL_GATINGRATIO_N_2,
    /**< Functional clock is interface clock divided by 2. */
    GPIO_CLK_GR_FCLK_ICLK_BY_4 = CSL_GPIO_CTRL_GATINGRATIO_N_4,
    /**< Functional clock is interface clock divided by 4. */
    GPIO_CLK_GR_FCLK_ICLK_BY_8 = CSL_GPIO_CTRL_GATINGRATIO_N_8
    /**< Functional clock is interface clock divided by 8. */
}gpioClkGr_t;

/** \brief Idle mode Configurations for GPIO. */
typedef enum gpioIdleMode
{
    GPIO_IDLE_MODE_FORCE_IDLE        = CSL_GPIO_SYSCONFIG_IDLEMODE_FORCEIDLE,
    /**< Force Idle - Idle request is acknowledge unconditionally. */
    GPIO_IDLE_MODE_NO_IDLE           = CSL_GPIO_SYSCONFIG_IDLEMODE_NOIDLE,
    /**< No Idle - An idle request is never acknowledged. */
    GPIO_IDLE_MODE_SMART_IDLE        = CSL_GPIO_SYSCONFIG_IDLEMODE_SMARTIDLE,
    /**< Smart-idle. Acknowledgement to an idle request is given based
         on the internal activity of the module. */
    GPIO_IDLE_MODE_SMART_IDLE_WAKEUP =
        CSL_GPIO_SYSCONFIG_IDLEMODE_SMARTIDLEWAKEUP
    /**< Smart Idle Wakeup (GPIO0 only). */
}gpioIdleMode_t;

/** \brief Structure holding the GPIO context. */
typedef struct gpioContext
{
    uint32_t ctrl;
    /**< GPIO control register value. */
    uint32_t dir;
    /**< GPIO direction control register value. */
    uint32_t data;
    /**< GPIO pin value register. */
}gpioContext_t;

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/**
 * \brief  This API performs the module reset of the GPIO module. It also
 *         waits until the reset process is complete.
 *
 * \param  baseAddr    The memory address of the GPIO instance being used
 */
void GPIOModuleReset(uint32_t baseAddr);

/**
 * \brief    This API is used to enable/disable the GPIO module.
 *
 * \details  When the GPIO module is enabled, the clocks to the module are not
 *           gated. When the GPIO module is disabled, the clocks to the module
 *           are gated.
 *
 * \param    baseAddr     The memory address of the GPIO instance being used.
 *
 * \param    enableModule Flag to control enabling and disabling of the module.
 *           'enableModule' can take following values
 *              - TRUE  - Enable GPIO module
 *              - FALSE - Disable GPIO module
 *
 * \note     Enabling the GPIO module is a primary step before any other
 *           configurations can be done.
 */
void GPIOModuleEnable(uint32_t baseAddr, uint32_t enableModule);


/**
 * \brief  This API configures the direction of a specified GPIO pin as being
 *         either input or output.
 *
 * \param  baseAddr      The memory address of the GPIO instance being used.
 * \param  pinNumber     The number of the pin in the GPIO instance.
 *         'pinNumber' can take one of the following values:
 *         (0 <= pinNumber <= 31).
 * \param  pinDirection  The required direction for the GPIO pin.
 *         pinDirection' can take one of the value from the following enum:
 *         - #gpioDirection_t.
 */
void GPIOSetDirMode(uint32_t baseAddr,
                    uint32_t pinNumber,
                    uint32_t pinDirection);


/**
 * \brief  This API drives an output GPIO pin to a logic HIGH or a logic LOW
 *         state.
 *
 * \param  baseAddr    The memory address of the GPIO instance being used
 * \param  pinNumber   The number of the pin in the GPIO instance
 *         'pinNumber' can take one of the following values:
 *          (0 <= pinNumber <= 31).
 * \param  pinValue    This specifies whether a logic HIGH or a logic LOW
 *         should be driven on the output pin.
 *         'pinValue' can take one of the following values:
 *         - #GPIO_PIN_LOW  - indicating to drive a logic LOW(0) on the pin.
 *         - #GPIO_PIN_HIGH - indicating to drive a logic HIGH(1) on the pin.
 */
void GPIOPinWrite(uint32_t baseAddr,
                  uint32_t pinNumber,
                  uint32_t pinValue);

/**
 * \brief   This API determines the logic level(value) on a specified
 *          GPIO pin.
 *
 * \param   baseAddr    The memory address of the GPIO instance being used
 * \param   pinNumber   The number of the pin in the GPIO instance
 *          'pinNumber' can take one of the following values:
 *          (0 <= pinNumber <= 31)
 *
 * \retval  #GPIO_PIN_LOW  - indicating to drive a logic LOW(0) on the pin.
 * \retval  #GPIO_PIN_HIGH - indicating to drive a logic HIGH(1) on the pin.
 */
uint32_t GPIOPinRead(uint32_t baseAddr, uint32_t pinNumber);


/**
 * \brief   This API determines the output logic level(value) on a specified
 *          GPIO pin.
 *
 * \param   baseAddr    The memory address of the GPIO instance being used
 * \param   pinNumber   The number of the pin in the GPIO instance
 *          'pinNumber' can take one of the following values:
 *          (0 <= pinNumber <= 31)
 *
 * \retval  #GPIO_PIN_LOW  - indicating to drive a logic LOW(0) on the pin.
 * \retval  #GPIO_PIN_HIGH - indicating to drive a logic HIGH(1) on the pin.
 */
uint32_t GPIOPinOutValueRead(uint32_t baseAddr, uint32_t pinNumber);

/**
 * \brief  This API is used to collectively set and collectively clear the
 *         specified output pins of a GPIO instance.
 *
 * \param  baseAddr   The memory address of the GPIO instance being used
 * \param  setMask    The bit mask of the bits which have to be set in the
 *                    GPIO data output Register.
 * \param  clrMask    The bit mask of the bits which have to cleared in the
 *                    GPIO data output Register.
 */
void GPIOMultiplePinsWrite(uint32_t baseAddr,
                           uint32_t setMask,
                           uint32_t clrMask);

/**
 * \brief   This API reads the pin values of the specified pins of the GPIO
 *          instance.
 *
 * \param   baseAddr   The memory address of the GPIO instance being used
 * \param   readMask   The bit mask of the bits whose values have to be read
 *                     from the Data Input Register.
 *
 * \retval  pin_values Values of the specified pins of the GPIO instance.
 */
uint32_t GPIOMultiplePinsRead(uint32_t baseAddr, uint32_t readMask);


/**
 * \brief  This API configures the event type for a specified input GPIO pin.
 *         Whenever the selected event occurs on that GPIO pin and if interrupt
 *         generation is enabled for that pin, the GPIO module will send an
 *         interrupt to CPU.
 *
 * \param  baseAddr   The memory address of the GPIO instance being used
 * \param  pinNumber  The number of the pin in the GPIO instance
 *         'pinNumber' can take one of the following values:
 *         (0 <= pinNumber <= 31).
 * \param  eventType  This specifies the event type on whose detection,
 *                    the GPIO module will send an interrupt to CPU,
 *                    provided interrupt generation for that pin is enabled.
 *         'eventType' can take one of the following values following enum:
 *         - #gpioIntrMask_t.
 *
 * \note  A typical use case of this API is explained below:
 *        If it is initially required that interrupt should be generated on a
 *        LOW level only, then this API can be called with
 *        #GPIO_INTR_MASK_LEVEL_LOW as the parameter.
 *        At a later point of time, if logic HIGH level only should be made as
 *        the interrupt generating event, then this API has to be first called
 *        with #GPIO_INTR_MASK_NO_LEVEL as the parameter and later with
 *        #GPIO_INTR_MASK_LEVEL_HIGH as the parameter. Doing this ensures that
 *        logic LOW level trigger for interrupts is disabled.
 */
void GPIOSetIntrType(uint32_t baseAddr, uint32_t pinNumber, uint32_t eventType);


/**
 * \brief  This API enables the configured interrupt event on a specified input
 *         GPIO pin to trigger an interrupt request.
 *
 * \param  baseAddr    The memory address of the GPIO instance being used
 * \param  intrLine    This specifies the interrupt request line on which the
 *                     interrupt request due to the transitions on a specified
 *                     pin be propagated.
 *         'intrLine' can take one of the following two values:
 *         -# 0 - Interrupt line 0
 *         -# 1 - Interrupt line 1
 * \param  pinNumber   The number of the pin in the GPIO instance
 *         pinNumber' can take one of the following values:
 *         (0 <= pinNumber <= 31).
 */
void GPIOIntrEnable(uint32_t baseAddr, uint32_t intrLine, uint32_t pinNumber);

/**
 * \brief  This API disables interrupt generation due to the detection
 *         of any event on a specified input GPIO pin.
 *
 * \param  baseAddr   The memory address of the GPIO instance being used
 * \param  intrLine   This specifies the interrupt request line which has to
 *                    disabled to transmit interrupt requests due to transitions
 *                    on a specified pin.
 *         'intrLine' can take one of the following two values:
 *         -# 0 - Interrupt line 0 - signifying that Interrupt Line 1 be
 *           disabled to transmit interrupt requests due to transitions on
 *           specified pin.
 *         -# 1 - Interrupt line 1 - signifying that Interrupt Line 2 be
 *           disabled to transmit interrupt requests due to transitions on
 *           specified pin.
 * \param  pinNumber  The number of the pin in the GPIO instance
 *         'pinNumber' can take one of the following values:
 *        (0 <= pinNumber <= 31).
 */
void GPIOIntrDisable(uint32_t baseAddr, uint32_t intrLine, uint32_t pinNumber);

/**
 * \brief  This API determines the enabled interrupt status of a specified pin.
 *
 * \param  baseAddr   The memory address of the GPIO instance being used
 * \param  intrLine   This specifies the interrupt line whose corresponding
 *                    enabled interrupt status register has to be accessed.
 *                    The status of the specified pin is returned in this API.
 *         'intrLine' can take one of the following two values:
 *         -# 0 - Interrupt line 0
 *         -# 1 - Interrupt line 1
 * \param  pinNumber  The number of the pin in the GPIO instance
 *         'pinNumber' can take one of the following values:
 *         (0 <= pinNumber <= 31)
 *
 * \retval int_status The enabled interrupt status of the pin on the specified
 *                    interrupt line. This could either be a non-zero or a
 *                    zero value.
 */
uint32_t GPIOIntrStatus(uint32_t baseAddr,
                        uint32_t intrLine,
                        uint32_t pinNumber);

/**
 * \brief  This API clears the enabled interrupt status of a specified GPIO
 *         pin.
 *
 * \param  baseAddr   The memory address of the GPIO instance being used
 * \param  intrLine   This specifies the interrupt line whose corresponding
 *                    enabled interrupt status register has to be accessed.
 *                    The status of the specified bit in it is cleared in this
 *                    API.
 *         'intrLine' can take one of the following two values:
 *         -# 0 - Interrupt line 0
 *         -# 1 - Interrupt line 1
 * \param  pinNumber  The number of the pin in the GPIO instance
 *         'pinNumber' can take one of the following values:
 *        (0 <= pinNumber <= 31)
 */
void GPIOIntrClear(uint32_t baseAddr, uint32_t intrLine, uint32_t pinNumber);

/**
 * \brief  This API clears the enabled interrupt status of a specified GPIO
 *         pin.
 *
 * \param  baseAddr   The memory address of the GPIO instance being used
 * \param  intrLine   This specifies the interrupt line whose corresponding
 *                    enabled interrupt status register has to be accessed.
 *                    The status of the specified bit in it is cleared in this
 *                    API.
 *         'intrLine' can take one of the following two values:
 *         -# 0 - Interrupt line 0
 *         -# 1 - Interrupt line 1
 * \param  pinMask  Mask of pins whose interrupts need to be cleared.
 */
void GPIOIntrClearMask(uint32_t baseAddr, uint32_t intrLine, uint32_t pinMask);

/**
 * \brief  This API determines the raw interrupt status of the specified GPIO
 *         pins in the instance corresponding to the specified interrupt line.
 *
 * \param  baseAddr   The memory address of the GPIO instance being used
 * \param  intLine    This specifies the interrupt line whose corresponding
 *                    raw interrupt status register has to be read.
 *         'intLine' can take one of the following two values:
 *         -# 0 - Interrupt line 0
 *         -# 1 - Interrupt line 1
 * \param  pinNumber  The number of the pin in the GPIO instance
 *         'pinNumber' can take one of the following values:
 *        (0 <= pinNumber <= 31)
 *
 * \retval raw_int_sts The raw interrupt status of the specified pins of the
 *                     GPIO instance corresponding to a specified interrupt line
 */
uint32_t GPIOIntrRawStatus(uint32_t baseAddr,
                           uint32_t intLine,
                           uint32_t pinNumber);

/**
 * \brief  This API manually triggers an interrupt request due to a specified
 *         GPIO pin.
 *
 * \param  baseAddr   The memory address of the GPIO instance being used
 * \param  intrLine   This specifies the interrupt line over which the
 *                    manually triggered interrupt request has to be
 *                    propagated.
 *         'intrLine' can take one of the following two values:
 *         -# 0 - Interrupt line 0
 *         -# 1 - Interrupt line 1
 * \param  pinNumber  The number of the pin in the GPIO instance.
 *         'pinNumber' can take one of the following values:
 *         (0 <= pinNumber <= 31).
 *
 * \note  Here an interrupt request is manually triggered by writing to a
 *        specified bit in the Raw Interrupt Status register. As a
 *        pre-requisite, the interrupt generation should be enabled for the
 *        GPIO pin.
 */
void GPIOIntrTrigger(uint32_t baseAddr, uint32_t intrLine, uint32_t pinNumber);

/**
 * \brief  This API enables the feature for the specified Input GPIO Pin
 *         (Interrupt Request Source) to generate an asynchronous wakeup
 *         signal to the CPU. This API also enables the global wakeup feature.
 *
 * \param  baseAddr        The memory address of the GPIO instance being used.
 * \param  smartWakeupLine This specifies the Smart Wakeup Interrupt Line over
 *                         which the asynchronous wakeup signal has to be
 *                         propagated due to the specified Input GPIO pin.
 *         'smartWakeupLine' can take one of the following values:
 *         -# 0 - Smart wakeup interrupt line 0
 *         -# 1 - Smart wakeup interrupt line 1
 * \param  pinNumber   The number of the pin in the GPIO instance.
 *         'pinNumber' can take one of the following values:
 *         (0 <= pinNumber <= 31).
 *
 * \note   Please find below notes regarding this api:
 *         -# Make sure that the GPIO instance in context is allowed to generate
 *            Wakeup interrupts to the CPU. If allowed, then enable Wakeup
 *            interrupt generation feature for that GPIO instance in the
 *            Interrupt Controller.
 *         -# Usually an Input GPIO Pin acts as an Interrupt Request(IRQ)Source.
 *            An expected transition on an Input GPIO Pin can generate a Wakeup
 *            request.
 *         -# A Wakeup signal is generated by an Input GPIO pin when an
 *            expected transition happens on that GPIO pin.
 *         -# For any Input pin of the GPIO module to generate Wakeup
 *            signal, the respective global feature for the module has
 *            to be enabled and this API takes care of enabling global wakeup
 *            feature.
 */
void GPIOWakeupIntrEnable(uint32_t baseAddr,
                          uint32_t smartWakeupLine,
                          uint32_t pinNumber);

/**
 * \brief  This API disables the feature for the specified Input GPIO Pin
 *         (Interrupt Request Source) to generate an asynchronous wakeup
 *         signal to the CPU.
 *
 * \param  baseAddr        The memory address of the GPIO instance being used.
 * \param  smartWakeupLine This specifies the Smart Wakeup Interrupt Line which
 *                         has to be disabled to propagate any asynchronous
 *                         wakeup signal due to the specified input GPIO Pin.
 *         'smartWakeupLine' can take one of the following values:
 *         -# 0 - Smart wakeup interrupt line 0
 *         -# 1 - Smart wakeup interrupt line 1
 * \param  pinNumber   The number of the pin in the GPIO instance.
 *         'pinNumber' can take one of the following values:
 *         (0 <= pinNumber <= 31).
 */
void GPIOWakeupIntrDisable(uint32_t baseAddr,
                           uint32_t smartWakeupLine,
                           uint32_t pinNumber);

/**
 * \brief  This API enables the GPIO Wakeup Generation feature for all
 *         the pins of the GPIO module.
 *
 * \param  baseAddr   The memory address of the GPIO instance being used.
 *
 * \note   Please find below notes
 *         -# A Wakeup signal is generated by an Input GPIO pin when an
 *            expected transition happens on that GPIO pin
 *         -# For any Input pin of the GPIO module to generate Wakeup
 *            signal, the respective global feature for the module has
 *            to be enabled by invoking this API.
 */
void GPIOGlobalWakeupEnable(uint32_t baseAddr);

/**
 * \brief  This API disables the GPIO Wakeup generation feature for all
 *         the pins of the GPIO module.
 *
 * \param  baseAddr    The memory address of the GPIO instance being used.
 */
void GPIOGlobalWakeupDisable(uint32_t baseAddr);

/**
 * \brief   This API is used to control(enable/disable) the Auto-Idle mode
 *          for GPIO.
 *
 * \param  baseAddr        The memory address of the GPIO instance being used
 * \param  enableAutoIdle  This specifies whether to enable or disable the
 *                         Auto-Idle mode for the GPIO instance.
 *         'enableAutoIdle' can take following macros:
 *         - TRUE  - Enable Auto idle mode.
 *         - FALSE - Disable Auto idle mode.
 */
void GPIOAutoIdleModeEnable(uint32_t baseAddr, uint32_t enableAutoIdle);

/**
 * \brief  This API is used to configure the Power Management
 *         Request/Acknowledgement process for GPIO.
 *
 * \param  baseAddr      The memory address of the GPIO instance being used
 * \param  idleModeType  This specifies the Power Management Request/Acknowledge
 *                       mode(Idle Mode Request and Response) to be enabled for
 *                       the GPIO instance.
 *         'idleModeType' can take values from the following enum:
 *         - #gpioIdleMode_t.
 */
void GPIOSetIdleMode(uint32_t baseAddr, uint32_t idleModeType);

/**
 * \brief  This API configures the clock gating ratio for the event detection
 *         logic.
 *
 * \param  baseAddr    The memory address of the GPIO instance being used
 * \param  gatingRatio This specifies the clock gating ratio value to be
 *                     programmed.
 *         'gatingRatio' can take values from the following enum:
 *         - #gpioClkGr_t.
 */
void GPIOSetGatingRatio(uint32_t baseAddr, uint32_t gatingRatio);

/**
 * \brief  This API enables/disables debouncing feature for a specified input
 *         GPIO pin.
 *
 * \param  baseAddr     The memory address of the GPIO instance being used
 * \param  pinNumber    The number of the pin in the GPIO instance
 *         'pinNumber' can take one of the following values:
 *         (0 <= pinNumber <= 31)
 * \param  debounceEnable  This specifies whether to enable/disable Debouncing
 *                         feature for the specified input pin
 *         'debounceEnable' can take values from the following macros:
 *         - TRUE  - Enable debouncing functionality.
 *         - FALSE - Disable debouncing functionality.
 */
void GPIODebounceFuncEnable(uint32_t baseAddr,
                            uint32_t pinNumber,
                            uint32_t debounceEnable);

/**
 * \brief  This API configures the debouncing time for all the input pins of
 *         a GPIO instance.
 *
 * \param  baseAddr      The memory address of the GPIO instance being used
 * \param  debounceTime  This specifies the number of debouncing clock pulses
 *                       each of 31 microseconds long to be used for debouncing
 *                       time. The formula for debouncing time is:
 *                       debounce time = ((debounceTime + 1) * 31) microseconds
 *         'debounceTime' can take a value as per below limits:
 *         - (0x00 <= debounceTime <= 0xFF)
 */
void GPIOSetDebounceTime(uint32_t baseAddr, uint32_t debounceTime);

/**
 * \brief  This API reads the contents in the Revision register of the GPIO
 *         module.
 *
 * \param  baseAddr    The memory address of the GPIO instance being used
 *
 * \retval Revision    The contents of the GPIO_REVISION register.
 */
uint32_t GPIOGetRevision(uint32_t baseAddr);

/**
 * \brief   This API stores the context of the gpio
 *
 * \param   baseAddr      Base Address of the GPIO Module Register.
 * \param   pCtx          Pointer to the structure where the GPIO context
 *                        need to be saved.
 **/
void GPIOContextSave(uint32_t baseAddr, gpioContext_t *pCtx);

/**
 * \brief   This API restores the context of the GPIO
 *
 * \param   baseAddr      Base Address of the GPIO Module Register.
 * \param   pCtx          Pointer to the structure where the GPIO context
 *                        need to be restored from.
 */
void GPIOContextRestore(uint32_t baseAddr, gpioContext_t *pCtx);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef GPIO_V1_LLD_H_ */
