/**
 * \file       wdt.c
 *
 * \brief      This file contains the function definitions for the device
 *             abstraction layer for WDT.
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
#include "wdt.h"
#include "error.h"
#include "hw_types.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 * \brief     This macro will check for write POSTED status of WDT registers
 *
 * \param     regField         Write posted status register field whose status
 *                             needs to be checked.
 *            regField will take values from the following enum
 *            #wdtWritePost_t
 */
#define WDT_WAIT_FOR_WRITE(baseAddr, regField) \
        while(0U != (HW_RD_REG32(baseAddr + WDT_WWPS) & regField))

#define WDT_DISABLE_SEQ1           (0x0000AAAAu)
#define WDT_DISABLE_SEQ2           (0x00005555u)
#define WDT_ENABLE_SEQ1            (0x0000BBBBu)
#define WDT_ENABLE_SEQ2            (0x00004444u)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief    Enumerates the values used to represent the WDT write
 *            posted status.
 */
typedef enum wdtWritePost
{
    WDT_WRITE_POST_WCLR = WDT_WWPS_W_PEND_WCLR_MASK,
    /**< Write pending for register WCLR. */
    WDT_WRITE_POST_WCRR = WDT_WWPS_W_PEND_WCRR_MASK,
    /**< Write pending for register WCRR. */
    WDT_WRITE_POST_WLDR = WDT_WWPS_W_PEND_WLDR_MASK,
    /**< Write pending for register WLDR. */
    WDT_WRITE_POST_WTGR = WDT_WWPS_W_PEND_WTGR_MASK,
    /**< Write pending for register WTGR. */
    WDT_WRITE_POST_WSPR = WDT_WWPS_W_PEND_WSPR_MASK,
    /**< Write pending for register WSPR. */
    WDT_WRITE_POST_WDLY = WDT_WWPS_W_PEND_WDLY_MASK
    /**< Write pending for register WDLY. */
} wdtWritePost_t;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

uint32_t WDTGetRevision(uint32_t baseAddr)
{
    /* Read the Revision number. */
    return(HW_RD_REG32(baseAddr + WDT_WIDR));
}

void WDTReset(uint32_t baseAddr)
{
    /* Reset the WDT module. */
    HW_WR_FIELD32((baseAddr + WDT_WDSC),
                   WDT_WDSC_SOFTRESET,
                   WDT_WDSC_SOFTRESET_INITIATE);

    /* Wait until the reset is done */
    while(1U != (HW_RD_REG32(baseAddr + WDT_WDST) &
          WDT_WDST_RESETDONE_DONE));
}

void WDTEnable(uint32_t baseAddr, uint32_t enableWdt)
{
    /* Wait for previous write to complete. */
    WDT_WAIT_FOR_WRITE(baseAddr, WDT_WRITE_POST_WSPR);

    if (TRUE == enableWdt)
    {
        /* Enable the WDT module. Sequence 1 is initiated. */
        HW_WR_FIELD32((baseAddr + WDT_WSPR),
                       WDT_WSPR_VALUE,
                       WDT_ENABLE_SEQ1);

        /* Wait for previous write to complete. */
        WDT_WAIT_FOR_WRITE(baseAddr, WDT_WRITE_POST_WSPR);

        /* Enable the WDT module. Sequence 2 is initiated. */
        HW_WR_FIELD32((baseAddr + WDT_WSPR),
                       WDT_WSPR_VALUE,
                       WDT_ENABLE_SEQ2);
    }
    else
    {
        /* Disable the WDT module. Sequence 1 is initiated. */
        HW_WR_FIELD32((baseAddr + WDT_WSPR),
                       WDT_WSPR_VALUE,
                       WDT_DISABLE_SEQ1);

        /* Wait for previous write to complete. */
        WDT_WAIT_FOR_WRITE(baseAddr, WDT_WRITE_POST_WSPR);

        /* Disable the WDT module. Sequence 2 is initiated. */
        HW_WR_FIELD32((baseAddr + WDT_WSPR),
                       WDT_WSPR_VALUE,
                       WDT_DISABLE_SEQ2);
    }
}

void WDTPrescalerClkEnable(uint32_t baseAddr,
                           uint32_t prescaler)
{
    /* Wait for previous write to complete. */
    WDT_WAIT_FOR_WRITE(baseAddr, WDT_WRITE_POST_WCLR);

    /* Enable the prescaler for WDT. */
    HW_WR_FIELD32((baseAddr + WDT_WCLR),
                   WDT_WCLR_PRE,
                   WDT_WCLR_PRE_ENABLED);

    /* Wait for previous write to complete. */
    WDT_WAIT_FOR_WRITE(baseAddr, WDT_WRITE_POST_WCLR);

    /* Configure the prescaler value. */
    HW_WR_FIELD32((baseAddr + WDT_WCLR),
                   WDT_WCLR_PTV,
                   prescaler);
}

void WDTPrescalerClkDisable(uint32_t baseAddr)
{
    /* Wait for previous write to complete. */
    WDT_WAIT_FOR_WRITE(baseAddr, WDT_WRITE_POST_WCLR);

    /* Disable the prescaler for WDT. */
    HW_WR_FIELD32((baseAddr + WDT_WCLR),
                   WDT_WCLR_PRE,
                   WDT_WCLR_PRE_DISABLED);
}

void WDTSetCounter(uint32_t baseAddr, uint32_t cntrVal)
{
    /* Wait for previous write to complete. */
    WDT_WAIT_FOR_WRITE(baseAddr, WDT_WRITE_POST_WCRR);

    /* Program the WDT counter value. */
    HW_WR_REG32((baseAddr + WDT_WCRR), cntrVal);
}

uint32_t WDTGetCounter(uint32_t baseAddr)
{
    /* Read the WDT counter value. */
    return(HW_RD_REG32(baseAddr + WDT_WCRR));
}

void WDTSetReloadVal(uint32_t baseAddr, uint32_t reloadVal)
{
    /* Wait for previous write to complete. */
    WDT_WAIT_FOR_WRITE(baseAddr, WDT_WRITE_POST_WLDR);

    /* Program the WDT counter value. */
    HW_WR_REG32((baseAddr + WDT_WLDR), reloadVal);
}

uint32_t WDTGetReloadVal(uint32_t baseAddr)
{
    /* Read the WDT reload value. */
    return(HW_RD_REG32(baseAddr + WDT_WLDR));
}

void WDTTriggerReload(uint32_t baseAddr, uint32_t trigVal)
{
    /* Wait for previous write to complete. */
    WDT_WAIT_FOR_WRITE(baseAddr, WDT_WRITE_POST_WTGR);

    /* Program the WDT trigger register. */
    HW_WR_REG32((baseAddr + WDT_WTGR), trigVal);
}

void WDTSetDelayVal(uint32_t baseAddr, uint32_t delayVal)
{
    /* Wait for previous write to complete. */
    WDT_WAIT_FOR_WRITE(baseAddr, WDT_WRITE_POST_WDLY);

    /* Program the WDT delay register. */
    HW_WR_REG32((baseAddr + WDT_WDLY), delayVal);
}

uint32_t WDTGetDelayVal(uint32_t baseAddr)
{
    /* Read the WDT delay register value. */
    return(HW_RD_REG32(baseAddr + WDT_WDLY));
}

void WDTIntrTrigger(uint32_t baseAddr, uint32_t intrMask)
{
    /* Trigger the raw interrupt. */
    HW_WR_REG32((baseAddr + WDT_WIRQSTATRAW), intrMask);
}

uint32_t WDTIntrRawStatus(uint32_t baseAddr)
{
    /* Read the interrupt raw status. */
    return(HW_RD_REG32(baseAddr + WDT_WIRQSTATRAW));
}

uint32_t WDTIntrStatus(uint32_t baseAddr)
{
    /* Read the interrupt status. */
    return(HW_RD_REG32(baseAddr + WDT_WISR));
}

void WDTIntrClear(uint32_t baseAddr, uint32_t intrMask)
{
    /* Clear the interrupt status. */
    HW_WR_REG32((baseAddr + WDT_WIRQSTAT), intrMask);
}

void WDTIntrEnable(uint32_t baseAddr, uint32_t intrMask)
{
    /* Enable the WDT interrupts. */
    HW_WR_REG32((baseAddr + WDT_WIRQENSET), intrMask);
}

void WDTIntrDisable(uint32_t baseAddr, uint32_t intrMask)
{
    /* Disable the WDT interrupts. */
    HW_WR_REG32((baseAddr + WDT_WIRQENCLR), intrMask);
}

void WDTSetIdleMode(uint32_t baseAddr, uint32_t idleMode)
{
    /* Configure the Idle mode of WDT. */
    HW_WR_FIELD32((baseAddr + WDT_WDSC),
                   WDT_WDSC_IDLEMODE,
                   idleMode);
}

void WDTEmuModeEnable(uint32_t baseAddr, uint32_t enableEmuMode)
{
    if (TRUE == enableEmuMode)
    {
        /* Counter is frozen during emulation. */
        HW_WR_FIELD32((baseAddr + WDT_WDSC),
                       WDT_WDSC_EMUFREE,
                       WDT_WDSC_EMUFREE_FROZEN);
    }
    else
    {
        /* Counter is free running during emulation. */
        HW_WR_FIELD32((baseAddr + WDT_WDSC),
                       WDT_WDSC_EMUFREE,
                       WDT_WDSC_EMUFREE_FREE_RUNNING);
    }
}

/* ========================================================================== */
/*                          Deprecated Function Definitions                   */
/* ========================================================================== */
