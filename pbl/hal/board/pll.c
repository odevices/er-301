/**
 *  \file  Adopted from sbl_am335x_platform_pll.c
 *
 * \brief  This file contains the the function to configure PLLs of SoC and OPP.
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

#include <hal/types.h>
#include <ti/am335x/hw_types.h>
//#include "hw_cm_cefuse.h"
//#include "hw_cm_device.h"
#include <ti/am335x/hw_cm_dpll.h>
#include <ti/am335x/hw_cm_mpu.h>
#include <ti/am335x/hw_cm_per.h>
#include <ti/am335x/hw_cm_rtc.h>
#include <ti/am335x/hw_cm_wkup.h>
#include <ti/am335x/hw_control.h>
#include <ti/am335x/soc.h>

static void pllCoreInit(uint32_t dpllMult,
                        uint32_t dpllDiv,
                        uint32_t dpllPostDivM4,
                        uint32_t dpllPostDivM5,
                        uint32_t dpllPostDivM6)
{
    /* Enable the Core PLL */

    /* Put the PLL in bypass mode */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_CORE,
                      CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN,
                      CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN_SHIFT,
                      CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN_DPLL_MN_BYP_MODE);

    /* Wait for DPLL to go in to bypass mode */

    while (0U == HW_RD_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_CORE,
                                   CM_WKUP_CM_IDLEST_DPLL_CORE_ST_MN_BYPASS,
                                   CM_WKUP_CM_IDLEST_DPLL_CORE_ST_MN_BYPASS_SHIFT))
        ;

    /* Set the multipler and divider values for the PLL */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_CORE,
                      CM_WKUP_CM_CLKSEL_DPLL_CORE_DPLL_MULT,
                      CM_WKUP_CM_CLKSEL_DPLL_CORE_DPLL_MULT_SHIFT,
                      dpllMult);
    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_CORE,
                      CM_WKUP_CM_CLKSEL_DPLL_CORE_DPLL_DIV,
                      CM_WKUP_CM_CLKSEL_DPLL_CORE_DPLL_DIV_SHIFT,
                      dpllDiv);

    /* Configure the High speed dividers */

    /* Set M4 divider */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M4_DPLL_CORE,
                      CM_WKUP_CM_DIV_M4_DPLL_CORE_HSDIVIDER_CLKOUT1_DIV,
                      CM_WKUP_CM_DIV_M4_DPLL_CORE_HSDIVIDER_CLKOUT1_DIV_SHIFT,
                      dpllPostDivM4);

    /* Set M5 divider */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M5_DPLL_CORE,
                      CM_WKUP_CM_DIV_M5_DPLL_CORE_HSDIVIDER_CLKOUT2_DIV,
                      CM_WKUP_CM_DIV_M5_DPLL_CORE_HSDIVIDER_CLKOUT2_DIV_SHIFT,
                      dpllPostDivM5);

    /* Set M6 divider */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M6_DPLL_CORE,
                      CM_WKUP_CM_DIV_M6_DPLL_CORE_HSDIVIDER_CLKOUT3_DIV,
                      CM_WKUP_CM_DIV_M6_DPLL_CORE_HSDIVIDER_CLKOUT3_DIV_SHIFT,
                      dpllPostDivM6);

    /* Now LOCK the PLL by enabling it */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_CORE,
                      CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN,
                      CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN_SHIFT,
                      CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN);

    while (0U == HW_RD_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_CORE,
                                   CM_WKUP_CM_IDLEST_DPLL_CORE_ST_DPLL_CLK,
                                   CM_WKUP_CM_IDLEST_DPLL_CORE_ST_DPLL_CLK_SHIFT))
        ;
}

static void pllDisplayInit(uint32_t dpllMult,
                           uint32_t dpllDiv,
                           uint32_t dpllPostDivM2)
{
    /* Put the PLL in bypass mode */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DISP,
                      CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN,
                      CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN_SHIFT,
                      CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN_DPLL_MN_BYP_MODE);

    /* Wait for DPLL to go in to bypass mode */

    while (0U == HW_RD_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DISP,
                                   CM_WKUP_CM_IDLEST_DPLL_DISP_ST_MN_BYPASS,
                                   CM_WKUP_CM_IDLEST_DPLL_DISP_ST_MN_BYPASS_SHIFT))
        ;

    /* Set the multipler and divider values for the PLL */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DISP,
                      CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_MULT,
                      CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_MULT_SHIFT,
                      dpllMult);
    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DISP,
                      CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_DIV,
                      CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_DIV_SHIFT,
                      dpllDiv);

    /* Set the CLKOUT2 divider */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DISP,
                      CM_WKUP_CM_DIV_M2_DPLL_DISP_DPLL_CLKOUT_DIV,
                      CM_WKUP_CM_DIV_M2_DPLL_DISP_DPLL_CLKOUT_DIV_SHIFT,
                      dpllPostDivM2);

    /* Now LOCK the PLL by enabling it */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DISP,
                      CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN,
                      CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN_SHIFT,
                      CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN);

    while (0U == HW_RD_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DISP,
                                   CM_WKUP_CM_IDLEST_DPLL_DISP_ST_DPLL_CLK,
                                   CM_WKUP_CM_IDLEST_DPLL_DISP_ST_DPLL_CLK_SHIFT))
        ;
}

static void pllPerInit(uint32_t dpllMult,
                       uint32_t dpllDiv,
                       uint32_t sigmaDelatDiv,
                       uint32_t dpllPostDivM2)
{
    /* Put the PLL in bypass mode */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_PER,
                      CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN,
                      CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN_SHIFT,
                      CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN_DPLL_MN_BYP_MODE);

    /* Wait for DPLL to go in to bypass mode */

    while (0U == HW_RD_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_PER,
                                   CM_WKUP_CM_IDLEST_DPLL_PER_ST_MN_BYPASS,
                                   CM_WKUP_CM_IDLEST_DPLL_PER_ST_MN_BYPASS_SHIFT))
        ;

    /* Set the multipler and divider values for the PLL */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_PERIPH,
                      CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_MULT,
                      CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_MULT_SHIFT,
                      dpllMult);
    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_PERIPH,
                      CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_DIV,
                      CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_DIV_SHIFT,
                      dpllDiv);

    /* Set the CLKOUT2 divider */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_PER,
                      CM_WKUP_CM_DIV_M2_DPLL_PER_DPLL_CLKOUT_DIV,
                      CM_WKUP_CM_DIV_M2_DPLL_PER_DPLL_CLKOUT_DIV_SHIFT,
                      dpllPostDivM2);
    /* Now LOCK the PLL by enabling it */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_PER,
                      CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN,
                      CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN_SHIFT,
                      CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN);

    while (0U == HW_RD_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_PER,
                                   CM_WKUP_CM_IDLEST_DPLL_PER_ST_DPLL_CLK,
                                   CM_WKUP_CM_IDLEST_DPLL_PER_ST_DPLL_CLK_SHIFT))
        ;
}

static void pllDdrInit(uint32_t dpllMult,
                       uint32_t dpllDiv,
                       uint32_t dpllPostDivM2,
                       uint32_t dpllPostDivM4)
{
    /* Put the PLL in bypass mode */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR,
                      CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN,
                      CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN_SHIFT,
                      CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN_DPLL_MN_BYP_MODE);

    /* Wait for DPLL to go in to bypass mode */

    while (0U == HW_RD_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DDR,
                                   CM_WKUP_CM_IDLEST_DPLL_DDR_ST_MN_BYPASS,
                                   CM_WKUP_CM_IDLEST_DPLL_DDR_ST_MN_BYPASS_SHIFT))
        ;

    /* Set the multipler and divider values for the PLL */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DDR,
                      CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_MULT,
                      CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_MULT_SHIFT,
                      dpllMult);
    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DDR,
                      CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_DIV,
                      CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_DIV_SHIFT,
                      dpllDiv);

    /* Set the CLKOUT2 divider */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DDR,
                      CM_WKUP_CM_DIV_M2_DPLL_DDR_DPLL_CLKOUT_DIV,
                      CM_WKUP_CM_DIV_M2_DPLL_DDR_DPLL_CLKOUT_DIV_SHIFT,
                      dpllPostDivM2);

    /* Now LOCK the PLL by enabling it */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR,
                      CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN,
                      CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN_SHIFT,
                      CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN);

    while (0U == HW_RD_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DDR,
                                   CM_WKUP_CM_IDLEST_DPLL_DDR_ST_DPLL_CLK,
                                   CM_WKUP_CM_IDLEST_DPLL_DDR_ST_DPLL_CLK_SHIFT))
        ;
}

static void pllMpuInit(uint32_t dpllMult,
                       uint32_t dpllDiv,
                       uint32_t dpllPostDivM2)
{
    /* Put the PLL in bypass mode */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_MPU,
                      CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN,
                      CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN_SHIFT,
                      CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN_DPLL_MN_BYP_MODE);

    /* Wait for DPLL to go in to bypass mode */

    while (0U == HW_RD_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_MPU,
                                   CM_WKUP_CM_IDLEST_DPLL_MPU_ST_MN_BYPASS,
                                   CM_WKUP_CM_IDLEST_DPLL_MPU_ST_MN_BYPASS_SHIFT))
        ;

    /* Set the multiplier and divider values for the PLL */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_MPU,
                      CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_MULT,
                      CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_MULT_SHIFT,
                      dpllMult);
    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_MPU,
                      CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_DIV,
                      CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_DIV_SHIFT,
                      dpllDiv);

    /* Set the CLKOUT2 divider */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_MPU,
                      CM_WKUP_CM_DIV_M2_DPLL_MPU_DPLL_CLKOUT_DIV,
                      CM_WKUP_CM_DIV_M2_DPLL_MPU_DPLL_CLKOUT_DIV_SHIFT,
                      dpllPostDivM2);

    /* Now LOCK the PLL by enabling it */

    HW_WR_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_MPU,
                      CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN,
                      CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN_SHIFT,
                      CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN);

    while (0U == HW_RD_FIELD32_RAW(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_MPU,
                                   CM_WKUP_CM_IDLEST_DPLL_MPU_ST_DPLL_CLK,
                                   CM_WKUP_CM_IDLEST_DPLL_MPU_ST_DPLL_CLK_SHIFT))
        ;
}

void pllInit()
{
    uint32_t crystalFreqSel = 0U;
    uint32_t inputClk = 0U;
    uint32_t dpllDiv = 0U;
    uint32_t mpuDpllMult = 0U;
    uint32_t mpuDpllPostDivM2 = 0U;
    uint32_t coreDpllMult = 0U;
    uint32_t coreDpllPostDivM4 = 0U;
    uint32_t coreDpllPostDivM5 = 0U;
    uint32_t coreDpllPostDivM6 = 0U;
    uint32_t perDpllMult = 0U;
    uint32_t perSigmaDelatDiv = 0U;
    uint32_t perDpllPostDivM2 = 0U;
    uint32_t ddrDpllMult = 0U;
    uint32_t ddrDpllPostDivM2 = 0U;
    uint32_t ddrDpllPostDivM4 = 0U;
    uint32_t dispDpllMult = 0U;
    uint32_t dispDpllPostDivM2 = 0U;

    /* Get input clock frequency. */
    crystalFreqSel = HW_RD_FIELD32_RAW(SOC_CONTROL_REGS + CONTROL_STATUS,
                                       CONTROL_STATUS_SYSBOOT1,
                                       CONTROL_STATUS_SYSBOOT1_SHIFT);

    switch (crystalFreqSel)
    {
    case 0U:
    {
        inputClk = 19U;
        break;
    }

    case 1U:
    {
        inputClk = 24U;
        break;
    }

    case 2U:
    {
        inputClk = 25U;
        break;
    }

    case 3U:
    {
        inputClk = 26U;
        break;
    }

    default:
    {
        break;
    }
    }

    /*
     * pllDiv = (inputClk/refClk) - 1U
     * PLL divider derived from input clock with referance clock as 1MHz.
     * PLL divider is same for all PLLs.
     */
    dpllDiv = inputClk - 1U;

    /* Need to check for clock frequency from board data */

    /* EVM independent pll configuration. */

    /*
     * mpuDpllMult = mpuClk/mpuDpllDiv
     * mpuDpllPostDivM2 = mpuM2Clk/mpuClk
     */
    //mpuDpllMult = 600U;
    mpuDpllMult = 1000U; // 1000MHz
    mpuDpllPostDivM2 = 1U;

    /*
     * coreDpllMult = coreClk/coreDpllDiv
     * coreDpllPostDivM4 = coreM4Clk/coreClk
     * coreDpllPostDivM5 = coreM5Clk/coreClk
     * coreDpllPostDivM6 = coreM6Clk/coreClk
     */
    coreDpllMult = 1000U;
    coreDpllPostDivM4 = 10U;
    coreDpllPostDivM5 = 8U;
    coreDpllPostDivM6 = 4U;

    /*
     * perDpllMult = perClk/perDpllDiv
     * perDpllPostDivM2 = perM2Clk/perClk
     * perSigmaDeltaDiv = (perClk/256U) + 1U
     */
    perDpllMult = 960U;
    perSigmaDelatDiv = ((perDpllMult / dpllDiv) / 256U) + 1U; /*** Need to check if it is required for AM335X ***/
    perDpllPostDivM2 = 5U;

    /*
     * dispDpllMult = dispClk/dispDpllDiv
     * dispDpllPostDivM2 = dispM2Clk/dispClk
     */
    dispDpllMult = 48U;
    dispDpllPostDivM2 = 1U;

    /* For BEAGLEBONE BLACK - 400 MHz */
    ddrDpllMult = 400U;
    ddrDpllPostDivM2 = 1U;
    ddrDpllPostDivM4 = 1U;

    /* TODO:  If possible replace with PRCM API or simplify the below
     *  implementation
     */
    pllMpuInit(mpuDpllMult, dpllDiv, mpuDpllPostDivM2);
    pllCoreInit(coreDpllMult, dpllDiv, coreDpllPostDivM4,
                coreDpllPostDivM5, coreDpllPostDivM6);
    pllPerInit(perDpllMult, dpllDiv, perSigmaDelatDiv,
               perDpllPostDivM2);
    pllDdrInit(ddrDpllMult, dpllDiv, ddrDpllPostDivM2,
               ddrDpllPostDivM4);

    /*
     * Display PLL initialization can be skipped for applications
     * not use display.
     */
    pllDisplayInit(dispDpllMult, dpllDiv, dispDpllPostDivM2);
}