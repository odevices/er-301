/********************************************************************
 * Copyright (C) 2013-2014 Texas Instruments Incorporated.
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
#ifndef _CSLR_GPIO_H_
#define _CSLR_GPIO_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <ti/am335x/cslr.h>
#include <ti/am335x/tistdtypes.h>

    /**************************************************************************
* Register Overlay Structure for __ALL__
**************************************************************************/
    typedef struct
    {
        volatile Uint32 REVISION;
        volatile Uint8 RSVD0[12];
        volatile Uint32 SYSCONFIG;
        volatile Uint8 RSVD1[12];
        volatile Uint32 EOI;
        volatile Uint32 IRQSTS_RAW_0;
        volatile Uint32 IRQSTS_RAW_1;
        volatile Uint32 IRQSTS_0;
        volatile Uint32 IRQSTS_1;
        volatile Uint32 IRQSTS_SET_0;
        volatile Uint32 IRQSTS_SET_1;
        volatile Uint32 IRQSTS_CLR_0;
        volatile Uint32 IRQSTS_CLR_1;
        volatile Uint32 IRQWAKEN_0;
        volatile Uint32 IRQWAKEN_1;
        volatile Uint8 RSVD2[200];
        volatile Uint32 SYSSTS;
        volatile Uint32 IRQSTS1;
        volatile Uint32 IRQEN1;
        volatile Uint32 WAKEUPEN;
        volatile Uint8 RSVD3[4];
        volatile Uint32 IRQSTS2;
        volatile Uint32 IRQEN2;
        volatile Uint32 CTRL;
        volatile Uint32 OE;
        volatile Uint32 DATAIN;
        volatile Uint32 DATAOUT;
        volatile Uint32 LEVELDETECT0;
        volatile Uint32 LEVELDETECT1;
        volatile Uint32 RISINGDETECT;
        volatile Uint32 FALLINGDETECT;
        volatile Uint32 DEBOUNCEN;
        volatile Uint32 DEBOUNCINGTIME;
        volatile Uint8 RSVD4[8];
        volatile Uint32 CLRIRQEN1;
        volatile Uint32 SETIRQEN1;
        volatile Uint8 RSVD5[8];
        volatile Uint32 CLRIRQEN2;
        volatile Uint32 SETIRQEN2;
        volatile Uint8 RSVD6[8];
        volatile Uint32 CLRWKUPENA;
        volatile Uint32 SETWKUENA;
        volatile Uint8 RSVD7[8];
        volatile Uint32 CLRDATAOUT;
        volatile Uint32 SETDATAOUT;
    } CSL_GpioRegs;

/**************************************************************************
* Register Macros
**************************************************************************/

/* IP Revision Identifier (X.Y.R) */
#define CSL_GPIO_REVISION (0x0U)

/* SYSTEM CONFIGURATION REGISTER */
#define CSL_GPIO_SYSCONFIG (0x10U)

/* EOI */
#define CSL_GPIO_EOI (0x20U)

/* Per-event raw interrupt status vector Showing all active events (enabled
 * and not enabled) (corresponding to first line of interrupt) */
#define CSL_GPIO_IRQSTS_RAW_0 (0x24U)

/* Per-event raw interrupt status vector Showing all active events (enabled
 * and not enabled) (corresponding to second line of interrupt) */
#define CSL_GPIO_IRQSTS_RAW_1 (0x28U)

/* Per-event enabled interrupt status vector Showing all active and enabled
 * events (corresponding to first line of interrupt) */
#define CSL_GPIO_IRQSTS_0 (0x2CU)

/* Per-event enabled interrupt status vector Showing all active and enabled
 * events (corresponding to second line of interrupt) */
#define CSL_GPIO_IRQSTS_1 (0x30U)

/* Per-event interrupt enable set vector (corresponding to first line of
 * interrupt) */
#define CSL_GPIO_IRQSTS_SET_0 (0x34U)

/* Per-event enable set interrupt vector (corresponding to second line of
 * interrupt) */
#define CSL_GPIO_IRQSTS_SET_1 (0x38U)

/* Per-event interrupt enable clear vector (corresponding to first line of
 * interrupt) */
#define CSL_GPIO_IRQSTS_CLR_0 (0x3CU)

/* Per-event enable clear interrupt vector (corresponding to second line of
 * interrupt) */
#define CSL_GPIO_IRQSTS_CLR_1 (0x40U)

/* Per-event wakeup enable set vector (corresponding to first line of
 * interrupt) Wakeup-enabled events take place when module is idle shall
 * generate an asynchronous wakeup */
#define CSL_GPIO_IRQWAKEN_0 (0x44U)

/* Per-event wakeup enable set vector (corresponding to second line of
 * interrupt) Wakeup-enabled events take place when module is idle shall
 * generate an asynchronous wakeup */
#define CSL_GPIO_IRQWAKEN_1 (0x48U)

/* System Status register. */
#define CSL_GPIO_SYSSTS (0x114U)

/* Obsolete register not to be used in Highlander environment! Interrupt
 * Status Register (legacy) for first line of interrupt. */
#define CSL_GPIO_IRQSTS1 (0x118U)

/* Obsolete register not to be used in Highlander environment! Interrupt
 * Enable Register (legacy) for first line of interrupt. 0 Disabled 1 Enabled */
#define CSL_GPIO_IRQEN1 (0x11CU)

/* Obsolete register not to be used in Highlander environment! Wakeup Enable
 * Register (legacy) for first line of interrupt. */
#define CSL_GPIO_WAKEUPEN (0x120U)

/* Obsolete register not to be used in Highlander environment! Interrupt
 * Status Register (legacy) for second line of interrupt. 0 No interrupt set 1
 * Interrupt set */
#define CSL_GPIO_IRQSTS2 (0x128U)

/* Obsolete register not to be used in Highlander environment! Interrupt
 * Enable Register (legacy) for second line of interrupt. 0 Disabled 1 Enabled */
#define CSL_GPIO_IRQEN2 (0x12CU)

/* GPIO Control register. */
#define CSL_GPIO_CTRL (0x130U)

/* Output Enable Register 0 Output Enabled 1 Output disabled */
#define CSL_GPIO_OE (0x134U)

/* Data Input Register (with sampled input data). */
#define CSL_GPIO_DATAIN (0x138U)

/* Data Output Register (data to set on output pins). */
#define CSL_GPIO_DATAOUT (0x13CU)

/* Detect Low Level Register 0 Low level detection disabled 1 Low level
 * detection enabled */
#define CSL_GPIO_LEVELDETECT0 (0x140U)

/* Detect High Level Register */
#define CSL_GPIO_LEVELDETECT1 (0x144U)

/* Detect Rising Edge Register */
#define CSL_GPIO_RISINGDETECT (0x148U)

/* Detect Falling Edge Register */
#define CSL_GPIO_FALLINGDETECT (0x14CU)

/* Debouncing Enable Register */
#define CSL_GPIO_DEBOUNCEN (0x150U)

/* Debouncing Value Register */
#define CSL_GPIO_DEBOUNCINGTIME (0x154U)

/* Obsolete register not to be used in Highlander environment! Clear Interrupt
 * Enable Register - Legacy Mode */
#define CSL_GPIO_CLRIRQEN1 (0x160U)

/* Obsolete register not to be used in Highlander environment! Set Interrupt
 * Enable Register */
#define CSL_GPIO_SETIRQEN1 (0x164U)

/* Obsolete register not to be used in Highlander environment! Clear Interrupt
 * Enable Register */
#define CSL_GPIO_CLRIRQEN2 (0x170U)

/* Obsolete register not to be used in Highlander environment! Set Interrupt
 * Enable Register */
#define CSL_GPIO_SETIRQEN2 (0x174U)

/* Obsolete register not to be used in Highlander environment! Clear Wake-up
 * Enable Register */
#define CSL_GPIO_CLRWKUPENA (0x180U)

/* Obsolete register not to be used in Highlander environment! Set Wake-up
 * Enable Register */
#define CSL_GPIO_SETWKUENA (0x184U)

/* Clear Data Output Register */
#define CSL_GPIO_CLRDATAOUT (0x190U)

/* Set Data Output Register */
#define CSL_GPIO_SETDATAOUT (0x194U)

    /**************************************************************************
* Field Definition Macros
**************************************************************************/

    /* REVISION */

#define CSL_GPIO_REVISION_MINOR_MASK (0x0000003FU)
#define CSL_GPIO_REVISION_MINOR_SHIFT (0U)
#define CSL_GPIO_REVISION_MINOR_RESETVAL (0x00000001U)
#define CSL_GPIO_REVISION_MINOR_MAX (0x0000003fU)

#define CSL_GPIO_REVISION_CUSTOM_MASK (0x000000C0U)
#define CSL_GPIO_REVISION_CUSTOM_SHIFT (6U)
#define CSL_GPIO_REVISION_CUSTOM_RESETVAL (0x00000000U)
#define CSL_GPIO_REVISION_CUSTOM_MAX (0x00000003U)

#define CSL_GPIO_REVISION_MAJOR_MASK (0x00000700U)
#define CSL_GPIO_REVISION_MAJOR_SHIFT (8U)
#define CSL_GPIO_REVISION_MAJOR_RESETVAL (0x00000000U)
#define CSL_GPIO_REVISION_MAJOR_MAX (0x00000007U)

#define CSL_GPIO_REVISION_RTL_MASK (0x0000F800U)
#define CSL_GPIO_REVISION_RTL_SHIFT (11U)
#define CSL_GPIO_REVISION_RTL_RESETVAL (0x00000001U)
#define CSL_GPIO_REVISION_RTL_MAX (0x0000001fU)

#define CSL_GPIO_REVISION_FUNC_MASK (0x0FFF0000U)
#define CSL_GPIO_REVISION_FUNC_SHIFT (16U)
#define CSL_GPIO_REVISION_FUNC_RESETVAL (0x00000060U)
#define CSL_GPIO_REVISION_FUNC_MAX (0x00000fffU)

#define CSL_GPIO_REVISION_SCHEME_MASK (0xC0000000U)
#define CSL_GPIO_REVISION_SCHEME_SHIFT (30U)
#define CSL_GPIO_REVISION_SCHEME_RESETVAL (0x00000001U)
#define CSL_GPIO_REVISION_SCHEME_MAX (0x00000003U)

#define CSL_GPIO_REVISION_RESETVAL (0x50600801U)

    /* SYSCONFIG */

#define CSL_GPIO_SYSCONFIG_AUTOIDLE_MASK (0x00000001U)
#define CSL_GPIO_SYSCONFIG_AUTOIDLE_SHIFT (0U)
#define CSL_GPIO_SYSCONFIG_AUTOIDLE_RESETVAL (0x00000000U)
#define CSL_GPIO_SYSCONFIG_AUTOIDLE_FREERUN (0x00000000U)
#define CSL_GPIO_SYSCONFIG_AUTOIDLE_AUTOMATIC (0x00000001U)

#define CSL_GPIO_SYSCONFIG_SOFTRESET_MASK (0x00000002U)
#define CSL_GPIO_SYSCONFIG_SOFTRESET_SHIFT (1U)
#define CSL_GPIO_SYSCONFIG_SOFTRESET_RESETVAL (0x00000000U)
#define CSL_GPIO_SYSCONFIG_SOFTRESET_NORMAL (0x00000000U)
#define CSL_GPIO_SYSCONFIG_SOFTRESET_RESET (0x00000001U)

#define CSL_GPIO_SYSCONFIG_ENAWAKEUP_MASK (0x00000004U)
#define CSL_GPIO_SYSCONFIG_ENAWAKEUP_SHIFT (2U)
#define CSL_GPIO_SYSCONFIG_ENAWAKEUP_RESETVAL (0x00000000U)
#define CSL_GPIO_SYSCONFIG_ENAWAKEUP_DISABLE (0x00000000U)
#define CSL_GPIO_SYSCONFIG_ENAWAKEUP_ENABLE (0x00000001U)

#define CSL_GPIO_SYSCONFIG_IDLEMODE_MASK (0x00000018U)
#define CSL_GPIO_SYSCONFIG_IDLEMODE_SHIFT (3U)
#define CSL_GPIO_SYSCONFIG_IDLEMODE_RESETVAL (0x00000000U)
#define CSL_GPIO_SYSCONFIG_IDLEMODE_FORCEIDLE (0x00000000U)
#define CSL_GPIO_SYSCONFIG_IDLEMODE_NOIDLE (0x00000001U)
#define CSL_GPIO_SYSCONFIG_IDLEMODE_SMARTIDLE (0x00000002U)
#define CSL_GPIO_SYSCONFIG_IDLEMODE_SMARTIDLEWAKEUP (0x00000003U)

#define CSL_GPIO_SYSCONFIG_RESETVAL (0x00000000U)

    /* EOI */

#define CSL_GPIO_EOI_LINE_NUMBER_MASK (0x00000001U)
#define CSL_GPIO_EOI_LINE_NUMBER_SHIFT (0U)
#define CSL_GPIO_EOI_LINE_NUMBER_RESETVAL (0x00000000U)
#define CSL_GPIO_EOI_LINE_NUMBER_EOI_0 (0x00000000U)
#define CSL_GPIO_EOI_LINE_NUMBER_EOI_1 (0x00000001U)

#define CSL_GPIO_EOI_RESETVAL (0x00000000U)

    /* IRQSTS_RAW_0 */

#define CSL_GPIO_IRQSTS_RAW_0_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQSTS_RAW_0_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQSTS_RAW_0_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQSTS_RAW_0_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQSTS_RAW_0_RESETVAL (0x00000000U)

    /* IRQSTS_RAW_1 */

#define CSL_GPIO_IRQSTS_RAW_1_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQSTS_RAW_1_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQSTS_RAW_1_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQSTS_RAW_1_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQSTS_RAW_1_RESETVAL (0x00000000U)

    /* IRQSTS_0 */

#define CSL_GPIO_IRQSTS_0_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQSTS_0_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQSTS_0_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQSTS_0_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQSTS_0_RESETVAL (0x00000000U)

    /* IRQSTS_1 */

#define CSL_GPIO_IRQSTS_1_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQSTS_1_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQSTS_1_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQSTS_1_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQSTS_1_RESETVAL (0x00000000U)

    /* IRQSTS_SET_0 */

#define CSL_GPIO_IRQSTS_SET_0_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQSTS_SET_0_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQSTS_SET_0_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQSTS_SET_0_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQSTS_SET_0_RESETVAL (0x00000000U)

    /* IRQSTS_SET_1 */

#define CSL_GPIO_IRQSTS_SET_1_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQSTS_SET_1_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQSTS_SET_1_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQSTS_SET_1_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQSTS_SET_1_RESETVAL (0x00000000U)

    /* IRQSTS_CLR_0 */

#define CSL_GPIO_IRQSTS_CLR_0_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQSTS_CLR_0_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQSTS_CLR_0_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQSTS_CLR_0_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQSTS_CLR_0_RESETVAL (0x00000000U)

    /* IRQSTS_CLR_1 */

#define CSL_GPIO_IRQSTS_CLR_1_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQSTS_CLR_1_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQSTS_CLR_1_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQSTS_CLR_1_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQSTS_CLR_1_RESETVAL (0x00000000U)

    /* IRQWAKEN_0 */

#define CSL_GPIO_IRQWAKEN_0_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQWAKEN_0_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQWAKEN_0_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQWAKEN_0_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQWAKEN_0_RESETVAL (0x00000000U)

    /* IRQWAKEN_1 */

#define CSL_GPIO_IRQWAKEN_1_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQWAKEN_1_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQWAKEN_1_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQWAKEN_1_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQWAKEN_1_RESETVAL (0x00000000U)

    /* SYSSTS */

#define CSL_GPIO_SYSSTS_RESETDONE_MASK (0x00000001U)
#define CSL_GPIO_SYSSTS_RESETDONE_SHIFT (0U)
#define CSL_GPIO_SYSSTS_RESETDONE_RESETVAL (0x00000000U)
#define CSL_GPIO_SYSSTS_RESETDONE_INPROGRESS (0x00000000U)
#define CSL_GPIO_SYSSTS_RESETDONE_COMPLETE (0x00000001U)

#define CSL_GPIO_SYSSTS_RESETVAL (0x00000000U)

    /* IRQSTS1 */

#define CSL_GPIO_IRQSTS1_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQSTS1_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQSTS1_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQSTS1_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQSTS1_RESETVAL (0x00000000U)

    /* IRQEN1 */

#define CSL_GPIO_IRQEN1_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQEN1_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQEN1_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQEN1_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQEN1_RESETVAL (0x00000000U)

    /* WAKEUPEN */

#define CSL_GPIO_WAKEUPEN_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_WAKEUPEN_INTLINE_SHIFT (0U)
#define CSL_GPIO_WAKEUPEN_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_WAKEUPEN_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_WAKEUPEN_RESETVAL (0x00000000U)

    /* IRQSTS2 */

#define CSL_GPIO_IRQSTS2_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQSTS2_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQSTS2_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQSTS2_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQSTS2_RESETVAL (0x00000000U)

    /* IRQEN2 */

#define CSL_GPIO_IRQEN2_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_IRQEN2_INTLINE_SHIFT (0U)
#define CSL_GPIO_IRQEN2_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_IRQEN2_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_IRQEN2_RESETVAL (0x00000000U)

    /* CTRL */

#define CSL_GPIO_CTRL_DISABLEMODULE_MASK (0x00000001U)
#define CSL_GPIO_CTRL_DISABLEMODULE_SHIFT (0U)
#define CSL_GPIO_CTRL_DISABLEMODULE_RESETVAL (0x00000000U)
#define CSL_GPIO_CTRL_DISABLEMODULE_ENABLED (0x00000000U)
#define CSL_GPIO_CTRL_DISABLEMODULE_DISABLED (0x00000001U)

#define CSL_GPIO_CTRL_GATINGRATIO_MASK (0x00000006U)
#define CSL_GPIO_CTRL_GATINGRATIO_SHIFT (1U)
#define CSL_GPIO_CTRL_GATINGRATIO_RESETVAL (0x00000001U)
#define CSL_GPIO_CTRL_GATINGRATIO_N_1 (0x00000000U)
#define CSL_GPIO_CTRL_GATINGRATIO_N_2 (0x00000001U)
#define CSL_GPIO_CTRL_GATINGRATIO_N_4 (0x00000002U)
#define CSL_GPIO_CTRL_GATINGRATIO_N_8 (0x00000003U)

#define CSL_GPIO_CTRL_RESETVAL (0x00000002U)

    /* OE */

#define CSL_GPIO_OE_OUTPUTEN_MASK (0xFFFFFFFFU)
#define CSL_GPIO_OE_OUTPUTEN_SHIFT (0U)
#define CSL_GPIO_OE_OUTPUTEN_RESETVAL (0xffffffffU)
#define CSL_GPIO_OE_OUTPUTEN_MAX (0xffffffffU)

#define CSL_GPIO_OE_RESETVAL (0xffffffffU)

    /* DATAIN */

#define CSL_GPIO_DATAIN_DATAIN_MASK (0xFFFFFFFFU)
#define CSL_GPIO_DATAIN_DATAIN_SHIFT (0U)
#define CSL_GPIO_DATAIN_DATAIN_RESETVAL (0x00000000U)
#define CSL_GPIO_DATAIN_DATAIN_MAX (0xffffffffU)

#define CSL_GPIO_DATAIN_RESETVAL (0x00000000U)

    /* DATAOUT */

#define CSL_GPIO_DATAOUT_DATAOUT_MASK (0xFFFFFFFFU)
#define CSL_GPIO_DATAOUT_DATAOUT_SHIFT (0U)
#define CSL_GPIO_DATAOUT_DATAOUT_RESETVAL (0x00000000U)
#define CSL_GPIO_DATAOUT_DATAOUT_MAX (0xffffffffU)

#define CSL_GPIO_DATAOUT_RESETVAL (0x00000000U)

    /* LEVELDETECT0 */

#define CSL_GPIO_LEVELDETECT0_LEVELDETECT0_MASK (0xFFFFFFFFU)
#define CSL_GPIO_LEVELDETECT0_LEVELDETECT0_SHIFT (0U)
#define CSL_GPIO_LEVELDETECT0_LEVELDETECT0_RESETVAL (0x00000000U)
#define CSL_GPIO_LEVELDETECT0_LEVELDETECT0_MAX (0xffffffffU)

#define CSL_GPIO_LEVELDETECT0_RESETVAL (0x00000000U)

    /* LEVELDETECT1 */

#define CSL_GPIO_LEVELDETECT1_LEVELDETECT1_MASK (0xFFFFFFFFU)
#define CSL_GPIO_LEVELDETECT1_LEVELDETECT1_SHIFT (0U)
#define CSL_GPIO_LEVELDETECT1_LEVELDETECT1_RESETVAL (0x00000000U)
#define CSL_GPIO_LEVELDETECT1_LEVELDETECT1_MAX (0xffffffffU)

#define CSL_GPIO_LEVELDETECT1_RESETVAL (0x00000000U)

    /* RISINGDETECT */

#define CSL_GPIO_RISINGDETECT_RISINGDETECT_MASK (0xFFFFFFFFU)
#define CSL_GPIO_RISINGDETECT_RISINGDETECT_SHIFT (0U)
#define CSL_GPIO_RISINGDETECT_RISINGDETECT_RESETVAL (0x00000000U)
#define CSL_GPIO_RISINGDETECT_RISINGDETECT_MAX (0xffffffffU)

#define CSL_GPIO_RISINGDETECT_RESETVAL (0x00000000U)

    /* FALLINGDETECT */

#define CSL_GPIO_FALLINGDETECT_FALLINGDETECT_MASK (0xFFFFFFFFU)
#define CSL_GPIO_FALLINGDETECT_FALLINGDETECT_SHIFT (0U)
#define CSL_GPIO_FALLINGDETECT_FALLINGDETECT_RESETVAL (0x00000000U)
#define CSL_GPIO_FALLINGDETECT_FALLINGDETECT_MAX (0xffffffffU)

#define CSL_GPIO_FALLINGDETECT_RESETVAL (0x00000000U)

    /* DEBOUNCEN */

#define CSL_GPIO_DEBOUNCEN_DEBOUNCEEN_MASK (0xFFFFFFFFU)
#define CSL_GPIO_DEBOUNCEN_DEBOUNCEEN_SHIFT (0U)
#define CSL_GPIO_DEBOUNCEN_DEBOUNCEEN_RESETVAL (0x00000000U)
#define CSL_GPIO_DEBOUNCEN_DEBOUNCEEN_MAX (0xffffffffU)

#define CSL_GPIO_DEBOUNCEN_RESETVAL (0x00000000U)

    /* DEBOUNCINGTIME */

#define CSL_GPIO_DEBOUNCINGTIME_DEBOUNCETIME_MASK (0x000000FFU)
#define CSL_GPIO_DEBOUNCINGTIME_DEBOUNCETIME_SHIFT (0U)
#define CSL_GPIO_DEBOUNCINGTIME_DEBOUNCETIME_RESETVAL (0x00000000U)
#define CSL_GPIO_DEBOUNCINGTIME_DEBOUNCETIME_MAX (0x000000ffU)

#define CSL_GPIO_DEBOUNCINGTIME_RESETVAL (0x00000000U)

    /* CLRIRQEN1 */

#define CSL_GPIO_CLRIRQEN1_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_CLRIRQEN1_INTLINE_SHIFT (0U)
#define CSL_GPIO_CLRIRQEN1_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_CLRIRQEN1_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_CLRIRQEN1_RESETVAL (0x00000000U)

    /* SETIRQEN1 */

#define CSL_GPIO_SETIRQEN1_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_SETIRQEN1_INTLINE_SHIFT (0U)
#define CSL_GPIO_SETIRQEN1_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_SETIRQEN1_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_SETIRQEN1_RESETVAL (0x00000000U)

    /* CLRIRQEN2 */

#define CSL_GPIO_CLRIRQEN2_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_CLRIRQEN2_INTLINE_SHIFT (0U)
#define CSL_GPIO_CLRIRQEN2_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_CLRIRQEN2_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_CLRIRQEN2_RESETVAL (0x00000000U)

    /* SETIRQEN2 */

#define CSL_GPIO_SETIRQEN2_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_SETIRQEN2_INTLINE_SHIFT (0U)
#define CSL_GPIO_SETIRQEN2_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_SETIRQEN2_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_SETIRQEN2_RESETVAL (0x00000000U)

    /* CLRWKUPENA */

#define CSL_GPIO_CLRWKUPENA_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_CLRWKUPENA_INTLINE_SHIFT (0U)
#define CSL_GPIO_CLRWKUPENA_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_CLRWKUPENA_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_CLRWKUPENA_RESETVAL (0x00000000U)

    /* SETWKUENA */

#define CSL_GPIO_SETWKUENA_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_SETWKUENA_INTLINE_SHIFT (0U)
#define CSL_GPIO_SETWKUENA_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_SETWKUENA_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_SETWKUENA_RESETVAL (0x00000000U)

    /* CLRDATAOUT */

#define CSL_GPIO_CLRDATAOUT_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_CLRDATAOUT_INTLINE_SHIFT (0U)
#define CSL_GPIO_CLRDATAOUT_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_CLRDATAOUT_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_CLRDATAOUT_RESETVAL (0x00000000U)

    /* SETDATAOUT */

#define CSL_GPIO_SETDATAOUT_INTLINE_MASK (0xFFFFFFFFU)
#define CSL_GPIO_SETDATAOUT_INTLINE_SHIFT (0U)
#define CSL_GPIO_SETDATAOUT_INTLINE_RESETVAL (0x00000000U)
#define CSL_GPIO_SETDATAOUT_INTLINE_MAX (0xffffffffU)

#define CSL_GPIO_SETDATAOUT_RESETVAL (0x00000000U)

#ifdef __cplusplus
}
#endif
#endif
