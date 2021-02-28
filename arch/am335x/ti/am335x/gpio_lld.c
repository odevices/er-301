/**
 *  \file   GPIO_v1_lld.c
 *
 *  \brief  Low lever APIs performing hardware register writes and reads for
 *          GPIO IP version 1.
 *
 *   This file contains the hardware register write/read APIs for GPIO.
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

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include "hw_types.h"
#include "gpio_lld.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/*!
 *  \brief Macro indicating true value
 */
#define TRUE_VAL 1U

/*!
 *  \brief Macro indicating false value
 */
#define FALSE_VAL 0U

/*!
 *  \brief GPIO register array offset definitions
 */
#define CSL_GPIO_LEVELDETECT(n)      (CSL_GPIO_LEVELDETECT0 + (n * 0x4U))
#define CSL_GPIO_IRQSTS_RAW(n)       (CSL_GPIO_IRQSTS_RAW_0 + (n * 0x4U))
#define CSL_GPIO_IRQSTS(n)           (CSL_GPIO_IRQSTS_0 + (n * 0x4U))
#define CSL_GPIO_IRQSTS_SET(n)       (CSL_GPIO_IRQSTS_SET_0 + (n * 0x4U))
#define CSL_GPIO_IRQSTS_CLR(n)       (CSL_GPIO_IRQSTS_CLR_0 + (n * 0x4U))
#define CSL_GPIO_IRQWAKEN(n)         (CSL_GPIO_IRQWAKEN_0 + (n * 0x4U))
#define GPIO_DEBOUNCEN_DEBOUNCEEN_N_NODEBOUNCE        (0U)
#define GPIO_DEBOUNCEN_DEBOUNCEEN_N_DEBOUNCE          (1U)

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
void GPIOModuleReset(uint32_t baseAddr)
{
    /*
     * Setting the SOFTRESET bit in System Configuration register.
     * Doing so would reset the GPIO module.
     */
    HW_WR_FIELD32((baseAddr + CSL_GPIO_SYSCONFIG),
        CSL_GPIO_SYSCONFIG_SOFTRESET, CSL_GPIO_SYSCONFIG_SOFTRESET_RESET);

    /* Waiting until the GPIO Module is reset.*/
    while(0U ==
        ((HW_RD_REG32(baseAddr + CSL_GPIO_SYSSTS)) &
        (CSL_GPIO_SYSSTS_RESETDONE_MASK)));
}


void GPIOModuleEnable(uint32_t baseAddr, uint32_t enableModule)
{
    if (TRUE_VAL == enableModule)
    {
        /* Enable GPIO module */
        HW_WR_FIELD32((baseAddr + CSL_GPIO_CTRL),
            CSL_GPIO_CTRL_DISABLEMODULE, CSL_GPIO_CTRL_DISABLEMODULE_ENABLED);
    }
    else
    {
        /* Disable GPIO modle */
        HW_WR_FIELD32((baseAddr + CSL_GPIO_CTRL),
            CSL_GPIO_CTRL_DISABLEMODULE, CSL_GPIO_CTRL_DISABLEMODULE_DISABLED);
    }
}


void GPIOSetDirMode(uint32_t baseAddr,
                    uint32_t pinNumber,
                    uint32_t pinDirection)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + CSL_GPIO_OE);

    /* Checking if pin is required to be an output pin. */
    if (GPIO_DIRECTION_OUTPUT == pinDirection)
    {
        regVal &= ~(1U << pinNumber);
    }
    else
    {
        regVal |= (1U << pinNumber);
    }
    HW_WR_REG32((baseAddr + CSL_GPIO_OE), regVal);
}


void GPIOPinWrite(uint32_t baseAddr, uint32_t pinNumber, uint32_t pinValue)
{
    if(GPIO_PIN_HIGH == pinValue)
    {
        HW_WR_REG32((baseAddr + CSL_GPIO_SETDATAOUT), (1U << pinNumber));
    }
    else
    {
        HW_WR_REG32((baseAddr + CSL_GPIO_CLRDATAOUT), (1U << pinNumber));
    }
}


uint32_t GPIOPinRead(uint32_t baseAddr, uint32_t pinNumber)
{
    return((HW_RD_REG32(baseAddr + CSL_GPIO_DATAIN)) & (1U << pinNumber));
}


uint32_t GPIOPinOutValueRead(uint32_t baseAddr, uint32_t pinNumber)
{
    return((HW_RD_REG32(baseAddr + CSL_GPIO_DATAOUT)) & (1U << pinNumber));
}


void GPIOMultiplePinsWrite(uint32_t baseAddr,
                           uint32_t setMask,
                           uint32_t clrMask)
{
    /* Setting the specified output pins in GPIO_DATAOUT register. */
    HW_WR_REG32((baseAddr + CSL_GPIO_SETDATAOUT), setMask);

    /* Clearing the specified output pins in GPIO_DATAOUT register. */
    HW_WR_REG32((baseAddr + CSL_GPIO_CLRDATAOUT), clrMask);
}


uint32_t GPIOMultiplePinsRead(uint32_t baseAddr, uint32_t readMask)
{
    return((HW_RD_REG32(baseAddr + CSL_GPIO_DATAIN)) & (readMask));
}


void GPIOSetIntrType(uint32_t baseAddr, uint32_t pinNumber, uint32_t eventType)
{
    uint32_t regVal = 0U;
    eventType &= 0xFFU;

    switch(eventType)
    {
        case GPIO_INTR_MASK_NO_LEVEL:
        {
            /* Disabling logic LOW level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW));
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW)),
                regVal);

            /* Disabling logic HIGH level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH));
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH)),
                regVal);
            break;
        }

        case GPIO_INTR_MASK_LEVEL_LOW:
        {
            /* Enabling logic LOW level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW));
            regVal |= (1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW)),
                regVal);

            /* Disabling logic HIGH level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH));
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH)),
                regVal);

            /* Disabling rising edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_RISINGDETECT);
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_RISINGDETECT), regVal);

            /* Disabling falling edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_FALLINGDETECT);
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_FALLINGDETECT), regVal);
            break;
        }

        case GPIO_INTR_MASK_LEVEL_HIGH:
        {
            /* Disabling logic LOW level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW));
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW)),
                regVal);

            /* Enabling logic HIGH level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH));
            regVal |= (1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH)),
                regVal);

            /* Disabling rising edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_RISINGDETECT);
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_RISINGDETECT), regVal);

            /* Disabling falling edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_FALLINGDETECT);
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_FALLINGDETECT), regVal);
            break;
        }

        case GPIO_INTR_MASK_BOTH_LEVEL:
        {
            /* Enabling logic LOW level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW));
            regVal |= (1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW)),
                regVal);

            /* Enabling logic HIGH level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH));
            regVal |= (1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH)),
                regVal);

            /* Disabling rising edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_RISINGDETECT);
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_RISINGDETECT), regVal);

            /* Disabling falling edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_FALLINGDETECT);
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_FALLINGDETECT), regVal);
            break;
        }

        case GPIO_INTR_MASK_NO_EDGE:
        {
            /* Disabling rising edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_RISINGDETECT);
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_RISINGDETECT), regVal);

            /* Disabling falling edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_FALLINGDETECT);
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_FALLINGDETECT), regVal);
            break;
        }

        case GPIO_INTR_MASK_RISE_EDGE:
        {
            /* Enabling rising edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_RISINGDETECT);
            regVal |= (1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_RISINGDETECT), regVal);

            /* Disabling falling edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_FALLINGDETECT);
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_FALLINGDETECT), regVal);

            /* Disabling logic LOW level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW));
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW)),
                regVal);

            /* Disabling logic HIGH level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH));
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH)),
                regVal);
            break;
        }

        case GPIO_INTR_MASK_FALL_EDGE:
        {
            /* Disabling rising edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_RISINGDETECT);
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_RISINGDETECT), regVal);;

            /* Enabling falling edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_FALLINGDETECT);
            regVal |= (1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_FALLINGDETECT), regVal);

            /* Disabling logic LOW level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW));
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW)),
                regVal);

            /* Disabling logic HIGH level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH));
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH)),
                regVal);
            break;
        }

        case GPIO_INTR_MASK_BOTH_EDGE:
        {
            /* Enabling rising edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_RISINGDETECT);
            regVal |= (1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_RISINGDETECT), regVal);

            /* Enabling falling edge detect interrupt generation. */
            regVal = HW_RD_REG32(baseAddr + CSL_GPIO_FALLINGDETECT);
            regVal |= (1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_FALLINGDETECT), regVal);

            /* Disabling logic LOW level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW));
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_LOW)),
                regVal);

            /* Disabling logic HIGH level detect interrupt generation. */
            regVal =
                HW_RD_REG32(baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH));
            regVal &= ~(1U << pinNumber);
            HW_WR_REG32((baseAddr + CSL_GPIO_LEVELDETECT(GPIO_LEVEL_HIGH)),
                regVal);
            break;
        }

        default:
        break;
    }
}


void GPIOIntrEnable(uint32_t baseAddr, uint32_t intrLine, uint32_t pinNumber)
{
    /* Enable the specified gpio pin interrupt */
    HW_WR_REG32((baseAddr + CSL_GPIO_IRQSTS_SET(intrLine)), (1U << pinNumber));
}


void GPIOIntrDisable(uint32_t baseAddr, uint32_t intrLine, uint32_t pinNumber)
{
    /* Disable the specified gpio pin interrupt */
    HW_WR_REG32((baseAddr + CSL_GPIO_IRQSTS_CLR(intrLine)), (1U << pinNumber));
}


uint32_t GPIOIntrStatus(uint32_t baseAddr,
                        uint32_t intrLine,
                        uint32_t pinNumber)
{
    uint32_t intrStatus = 0U;

    intrStatus = ((HW_RD_REG32(baseAddr + CSL_GPIO_IRQSTS(intrLine))) &
        (1U << pinNumber));

    return intrStatus;
}


void GPIOIntrClear(uint32_t baseAddr, uint32_t intrLine, uint32_t pinNumber)
{
    /* Clear the interrupt status of specified gpio pin */
    HW_WR_REG32((baseAddr + CSL_GPIO_IRQSTS(intrLine)), (1U << pinNumber));
}

void GPIOIntrClearMask(uint32_t baseAddr, uint32_t intrLine, uint32_t pinMask)
{
    /* Clear the interrupt status of specified gpio pin */
    HW_WR_REG32((baseAddr + CSL_GPIO_IRQSTS(intrLine)), (pinMask));
}


uint32_t GPIOIntrRawStatus(uint32_t baseAddr,
                           uint32_t intrLine,
                           uint32_t pinNumber)
{
    uint32_t intrStatus = 0U;

    if(GPIO_PIN_MASK_ALL != pinNumber)
    {
        intrStatus = ((HW_RD_REG32(baseAddr + CSL_GPIO_IRQSTS_RAW(intrLine))) &
            (1U << pinNumber));
    }
    else
    {
        intrStatus = HW_RD_REG32(baseAddr + CSL_GPIO_IRQSTS_RAW(intrLine));
    }

    return intrStatus;
}


void GPIOIntrTrigger(uint32_t baseAddr, uint32_t intrLine, uint32_t pinNumber)
{
    HW_WR_REG32((baseAddr + CSL_GPIO_IRQSTS_RAW(intrLine)), (1U << pinNumber));
}


void GPIOWakeupIntrEnable(uint32_t baseAddr,
                          uint32_t smartWakeupLine,
                          uint32_t pinNumber)
{
    uint32_t regVal =
        HW_RD_REG32(baseAddr + CSL_GPIO_IRQWAKEN(smartWakeupLine));
    regVal |= (1U << pinNumber);
    HW_WR_REG32((baseAddr + CSL_GPIO_IRQWAKEN(smartWakeupLine)), regVal);

    /* Check if Global wake up is enabled already, if not enable global wakeup*/
    if (0U ==
        ((HW_RD_REG32(baseAddr + CSL_GPIO_SYSCONFIG)) &
            (CSL_GPIO_SYSCONFIG_ENAWAKEUP_MASK)))
    {
        /* Enable GPIO global wake up */
        HW_WR_FIELD32((baseAddr + CSL_GPIO_SYSCONFIG),
            CSL_GPIO_SYSCONFIG_ENAWAKEUP, CSL_GPIO_SYSCONFIG_ENAWAKEUP_ENABLE);
    }
}


void GPIOWakeupIntrDisable(uint32_t baseAddr,
                           uint32_t smartWakeupLine,
                           uint32_t pinNumber)
{
    uint32_t regVal =
        HW_RD_REG32(baseAddr + CSL_GPIO_IRQWAKEN(smartWakeupLine));
    regVal &= ~(1U << pinNumber);
    HW_WR_REG32((baseAddr + CSL_GPIO_IRQWAKEN(smartWakeupLine)), regVal);
}


void GPIOGlobalWakeupEnable(uint32_t baseAddr)
{
    HW_WR_FIELD32((baseAddr + CSL_GPIO_SYSCONFIG),
        CSL_GPIO_SYSCONFIG_ENAWAKEUP, CSL_GPIO_SYSCONFIG_ENAWAKEUP_ENABLE);
}


void GPIOGlobalWakeupDisable(uint32_t baseAddr)
{
    HW_WR_FIELD32((baseAddr + CSL_GPIO_SYSCONFIG),
        CSL_GPIO_SYSCONFIG_ENAWAKEUP, CSL_GPIO_SYSCONFIG_ENAWAKEUP_DISABLE);
}


void GPIOAutoIdleModeEnable(uint32_t baseAddr, uint32_t enableAutoIdle)
{
    if (TRUE_VAL == enableAutoIdle)
    {
        /* Enable Auto idle mode */
        HW_WR_FIELD32((baseAddr + CSL_GPIO_SYSCONFIG),
            CSL_GPIO_SYSCONFIG_AUTOIDLE, CSL_GPIO_SYSCONFIG_AUTOIDLE_AUTOMATIC);
    }
    else
    {
        /* Disable Auto idle mode */
        HW_WR_FIELD32((baseAddr + CSL_GPIO_SYSCONFIG),
            CSL_GPIO_SYSCONFIG_AUTOIDLE, CSL_GPIO_SYSCONFIG_AUTOIDLE_FREERUN);
    }
}


void GPIOSetIdleMode(uint32_t baseAddr, uint32_t idleModeType)
{
    HW_WR_FIELD32((baseAddr + CSL_GPIO_SYSCONFIG),
        CSL_GPIO_SYSCONFIG_IDLEMODE, idleModeType);
}


void GPIOSetGatingRatio(uint32_t baseAddr, uint32_t gatingRatio)
{
    HW_WR_FIELD32((baseAddr + CSL_GPIO_CTRL), CSL_GPIO_CTRL_GATINGRATIO,
        gatingRatio);
}


void GPIODebounceFuncEnable(uint32_t baseAddr,
                            uint32_t pinNumber,
                            uint32_t debounceEnable)
{
    uint32_t regVal = HW_RD_REG32(baseAddr + CSL_GPIO_DEBOUNCEN);

    if(TRUE_VAL == debounceEnable)
    {
        /* Enable debounce functionality */
        regVal &= ~(1U << pinNumber);
        regVal |= (GPIO_DEBOUNCEN_DEBOUNCEEN_N_DEBOUNCE << pinNumber);
    }
    else
    {
        /* Disable debounce functionality */
        regVal &= ~(1U << pinNumber);
        regVal |= (GPIO_DEBOUNCEN_DEBOUNCEEN_N_NODEBOUNCE << pinNumber);
    }
    HW_WR_REG32((baseAddr + CSL_GPIO_DEBOUNCEN), regVal);
}


void GPIOSetDebounceTime(uint32_t baseAddr, uint32_t debounceTime)
{
    HW_WR_FIELD32((baseAddr + CSL_GPIO_DEBOUNCINGTIME),
        CSL_GPIO_DEBOUNCINGTIME_DEBOUNCETIME, debounceTime);
}


uint32_t GPIOGetRevision(uint32_t baseAddr)
{
    return (HW_RD_REG32(baseAddr + CSL_GPIO_REVISION));
}


void GPIOContextSave(uint32_t baseAddr, gpioContext_t *pCtx)
{
    pCtx->ctrl = HW_RD_REG32(baseAddr + CSL_GPIO_CTRL);
    pCtx->dir  = HW_RD_REG32(baseAddr + CSL_GPIO_OE);
    pCtx->data = HW_RD_REG32(baseAddr + CSL_GPIO_SETDATAOUT);
}


void GPIOContextRestore(uint32_t baseAddr, gpioContext_t *pCtx)
{
    HW_WR_REG32((baseAddr + CSL_GPIO_CTRL), pCtx->ctrl);
    HW_WR_REG32((baseAddr + CSL_GPIO_OE), pCtx->dir);
    HW_WR_REG32((baseAddr + CSL_GPIO_SETDATAOUT), pCtx->data);
}

