/**
 *  \file Adopted from sbl_am335x_platform_ddr.c
 *
 * \brief  This file contain functions which configure EMIF and DDR.
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
#include <ti/am335x/error.h>
#include <ti/am335x/hw_types.h>
#include <ti/am335x/hw_emif4d.h>
#include <ti/am335x/hw_cm_dpll.h>
#include <ti/am335x/hw_cm_mpu.h>
#include <ti/am335x/hw_cm_per.h>
#include <ti/am335x/hw_cm_rtc.h>
#include <ti/am335x/hw_cm_wkup.h>
#include <ti/am335x/hw_control.h>
#include <ti/am335x/soc.h>

#define DDR_PHY_CTRL_REGS (SOC_CONTROL_REGS + 0x2000)
#define CMD0_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0x1C)
#define CMD0_SLAVE_FORCE_0 (DDR_PHY_CTRL_REGS + 0x20)
#define CMD0_SLAVE_DELAY_0 (DDR_PHY_CTRL_REGS + 0x24)
#define CMD0_LOCK_DIFF_0 (DDR_PHY_CTRL_REGS + 0x28)
#define CMD0_INVERT_CLKOUT_0 (DDR_PHY_CTRL_REGS + 0x2C)
#define CMD1_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0x50)
#define CMD1_SLAVE_FORCE_0 (DDR_PHY_CTRL_REGS + 0x54)
#define CMD1_SLAVE_DELAY_0 (DDR_PHY_CTRL_REGS + 0x58)
#define CMD1_LOCK_DIFF_0 (DDR_PHY_CTRL_REGS + 0x5C)
#define CMD1_INVERT_CLKOUT_0 (DDR_PHY_CTRL_REGS + 0x60)
#define CMD2_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0x84)
#define CMD2_SLAVE_FORCE_0 (DDR_PHY_CTRL_REGS + 0x88)
#define CMD2_SLAVE_DELAY_0 (DDR_PHY_CTRL_REGS + 0x8C)
#define CMD2_LOCK_DIFF_0 (DDR_PHY_CTRL_REGS + 0x90)
#define CMD2_INVERT_CLKOUT_0 (DDR_PHY_CTRL_REGS + 0x94)
#define DATA0_RD_DQS_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0xC8)
#define DATA0_RD_DQS_SLAVE_RATIO_1 (DDR_PHY_CTRL_REGS + 0xCC)
#define DATA0_WR_DQS_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0xDC)
#define DATA0_WR_DQS_SLAVE_RATIO_1 (DDR_PHY_CTRL_REGS + 0xE0)
#define DATA0_WRLVL_INIT_RATIO_0 (DDR_PHY_CTRL_REGS + 0xF0)
#define DATA0_WRLVL_INIT_RATIO_1 (DDR_PHY_CTRL_REGS + 0xF4)
#define DATA0_GATELVL_INIT_RATIO_0 (DDR_PHY_CTRL_REGS + 0xFC)
#define DATA0_GATELVL_INIT_RATIO_1 (DDR_PHY_CTRL_REGS + 0x100)
#define DATA0_FIFO_WE_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0x108)
#define DATA0_FIFO_WE_SLAVE_RATIO_1 (DDR_PHY_CTRL_REGS + 0x10C)
#define DATA0_WR_DATA_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0x120)
#define DATA0_WR_DATA_SLAVE_RATIO_1 (DDR_PHY_CTRL_REGS + 0x124)
#define DATA0_USE_RANK0_DELAYS_0 (DDR_PHY_CTRL_REGS + 0x134)
#define DATA0_LOCK_DIFF_0 (DDR_PHY_CTRL_REGS + 0x138)
#define DATA1_RD_DQS_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0x16c)
#define DATA1_RD_DQS_SLAVE_RATIO_1 (DDR_PHY_CTRL_REGS + 0x170)
#define DATA1_WR_DQS_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0x180)
#define DATA1_WR_DQS_SLAVE_RATIO_1 (DDR_PHY_CTRL_REGS + 0x184)
#define DATA1_WRLVL_INIT_RATIO_0 (DDR_PHY_CTRL_REGS + 0x194)
#define DATA1_WRLVL_INIT_RATIO_1 (DDR_PHY_CTRL_REGS + 0x198)
#define DATA1_GATELVL_INIT_RATIO_0 (DDR_PHY_CTRL_REGS + 0x1a0)
#define DATA1_GATELVL_INIT_RATIO_1 (DDR_PHY_CTRL_REGS + 0x1a4)
#define DATA1_FIFO_WE_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0x1ac)
#define DATA1_FIFO_WE_SLAVE_RATIO_1 (DDR_PHY_CTRL_REGS + 0x1b0)
#define DATA1_WR_DATA_SLAVE_RATIO_0 (DDR_PHY_CTRL_REGS + 0x1c4)
#define DATA1_WR_DATA_SLAVE_RATIO_1 (DDR_PHY_CTRL_REGS + 0x1c8)
#define DATA1_USE_RANK0_DELAYS_0 (DDR_PHY_CTRL_REGS + 0x1d8)
#define DATA1_LOCK_DIFF_0 (DDR_PHY_CTRL_REGS + 0x1dc)

/**< \brief Structure holding the values of control registers. */
typedef struct ddrCtrlCfg
{
    uint32_t addrIoCtrl;
    /**<
     *  DDR addr io control to configure
     *  -# slew rate of clock IO pads
     *  -# pull up and pull down output impedence of clock IO pads
     *  -# addr/cmd IO Pads output Slew Rate
     *  -# program addr/cmd IO Pad Pull up Pull down output Impedance
     */
    uint32_t dataIoCtrl;
    /**<
     *  DDR data0/1/2 io control to configure
     *  -# controls WD0 & WD1 of DQS0/1/2.
     *  -# controls WD0 & WD1 of DM0/1/2.
     *  -# controls WD0 & WD1 of DDR data pins.
     *  -# clock IO Pads (DQS(0/1/2)/DQ(0/1/2)S#) output Slew Rate.
     *  -# clock IO Pads (DQS/DQS#) Pull up Pull down output Impedance.
     *  -# data IO Pads output Slew Rate.
     *  -# data IO Pad Pull up Pull down output Impedance.
     * Please refer to DDR IO buffer spec for details
     */
    uint32_t ddrIoCtrl;
    /**<
     *  DDR io control
     */
} ddrCtrlCfg_t;

/**< \brief Structure holding the values of DDR Emif registers. */
typedef struct ddrEmifCfg
{
    uint32_t ddrPhyCtrl;
    /**< DDR phy control register value. */
    uint32_t ddrPhyCtrlDyPwrdn;
    /**< DDR phy control dynamic power down register value. */
    uint32_t sdramTim1;
    /**< SDRAM timing1 register value. */
    uint32_t sdramTim2;
    /**< SDRAM timing2 register value. */
    uint32_t sdramTim3;
    /**< SDRAM timing3 register value. */
    uint32_t sdramCfg;
    /**< SDRAM config register value. */
    uint32_t sdramRefCtrl1;
    /**< SDRAM ref control register value 1. */
    uint32_t sdramRefCtrl2;
    /**< SDRAM ref control register value 2. */
    uint32_t zqConfig;
    /**< ZQ config register value. */
} ddrEmifCfg_t;

/**< \brief Structure holding the parameters to configure EMIF phy registers. */
typedef struct ddrEmifPhyCfg
{
    /**< COMMAND */
    uint32_t slaveRatio;
    /**< slave ratio. */
    uint32_t slaveForce;
    /**< slave force. */
    uint32_t slaveDelay;
    /**< slave delay. */
    uint32_t lockDiff;
    /**< lock diff. */
    uint32_t invertClkout;
    /**< invert clockout. */

    /**< DATA */
    uint32_t rdDqsSlaveRatio;
    /**< Read DQS slave ratio. */
    uint32_t wrDqsSlaveRatio;
    /**< Write DQS slave ratio. */
    uint32_t fifoWeSlaveRatio;
    /**< FIFO write enable slave ratio. */
    uint32_t wrDataSlaveRatio;
    /**< writer data slave ratio. */
} ddrEmifPhyCfg_t;

/**< \brief Structure holding the ddr configurations. */
typedef struct ddrCfg
{
    ddrCtrlCfg_t ddrCtrlCfg;
    /**< DDR ctrl config registers. */
    ddrEmifCfg_t ddrEmifCfg;
    /**< DDR Emif config registers. */
    ddrEmifPhyCfg_t ddrEmifPhyCfg;
    /**< EMIF phy config registers. */
} ddrCfg_t;

#define DDR_NOTAPPLICABLE_VALUE (0x00000000U)

/**
 * DDR3 configurations for BEAGLEBONEBLACK and others.
 */

#define DDR3_CONTROL_DDR_ADDRCTRL_IOCTRL_VALUE (0x18BU)
#define DDR3_CONTROL_DDR_DATA_IOCTRL_VALUE (0x18BU)
#define DDR3_CONTROL_DDR_IOCTRL_VALUE (0xefffffffU)

#define DDR3_EMIF_DDR_PHY_CTRL_VALUE (0x0E084007U)
#define DDR3_ZQ_CONFIG_VALUE (0x50074BE4U)

/**
 * DDR3 configurations for BEAGLEBONEBLACK.
 */

/**
 * DDR EMIF physical configuration for command
 */
#define BBB_DDR3_CMD_SLAVE_RATIO (0x80U)
#define BBB_DDR3_CMD_INVERT_CLKOUT (0x0U)

/**
 * DDR EMIF physical configuration for data
 */
#define BBB_DDR3_DATA_RD_DQS_SLAVE_RATIO (0x38U)
#define BBB_DDR3_DATA_WR_DQS_SLAVE_RATIO (0x44U)
#define BBB_DDR3_DATA_FIFO_WE_SLAVE_RATIO (0x94U)
#define BBB_DDR3_DATA_WR_DATA_SLAVE_RATIO (0x7dU)

/**
 *DDR Emif registers.
 */
#define BBB_DDR3_EMIF_DDR_PHY_CTRL_1 (0x07U)
#define BBB_DDR3_EMIF_DDR_PHY_CTRL_1_DY_PWRDN (0x00100000U)

#define BBB_DDR3_EMIF_SDRAM_TIM_1_VALUE (0x0AAAD4DBU)
#define BBB_DDR3_EMIF_SDRAM_TIM_2_VALUE (0x266B7FDAU)
#define BBB_DDR3_EMIF_SDRAM_TIM_3_VALUE (0x501F867FU)

#define BBB_DDR3_EMIF_SDRAM_CONFIG_VALUE (0x61C05332U)

#define BBB_DDR3_EMIF_SDRAM_REF_CTRL_VAL1 (0x00000C30U)

/** BEAGLEBONE BLACK DDR3 configuration */
static const ddrCfg_t BBB_DDR3_CONFIG =
    {
        {
            DDR3_CONTROL_DDR_ADDRCTRL_IOCTRL_VALUE, /* ddr address io control */
            DDR3_CONTROL_DDR_DATA_IOCTRL_VALUE,     /* ddr data io control */
            DDR3_CONTROL_DDR_IOCTRL_VALUE,          /* ddr io control */
        },                                          /* ddrCtrl_t */
        {
            BBB_DDR3_EMIF_DDR_PHY_CTRL_1,          /* ddr phy control */
            BBB_DDR3_EMIF_DDR_PHY_CTRL_1_DY_PWRDN, /* ddr phy control dynamic power down */

            BBB_DDR3_EMIF_SDRAM_TIM_1_VALUE,  /* sdram timing1 */
            BBB_DDR3_EMIF_SDRAM_TIM_2_VALUE,  /* sdram timing2 */
            BBB_DDR3_EMIF_SDRAM_TIM_3_VALUE,  /* sdram timing3 */
            BBB_DDR3_EMIF_SDRAM_CONFIG_VALUE, /* sdram config */

            BBB_DDR3_EMIF_SDRAM_REF_CTRL_VAL1, /* sdram ref control value 1 */
            DDR_NOTAPPLICABLE_VALUE,           /* sdram ref control value 2 */
            DDR3_ZQ_CONFIG_VALUE               /* ZQ config */
        },                                     /* ddrEmif_t */
        {
            BBB_DDR3_CMD_SLAVE_RATIO,   /* slave ratio */
            DDR_NOTAPPLICABLE_VALUE,    /* slave force */
            DDR_NOTAPPLICABLE_VALUE,    /* slave delay */
            DDR_NOTAPPLICABLE_VALUE,    /* lock diff */
            BBB_DDR3_CMD_INVERT_CLKOUT, /* invert clockout */

            BBB_DDR3_DATA_RD_DQS_SLAVE_RATIO,  /* rd dqs slave ratio */
            BBB_DDR3_DATA_WR_DQS_SLAVE_RATIO,  /* wr dqs slave ratio */
            BBB_DDR3_DATA_FIFO_WE_SLAVE_RATIO, /* fifo we slave ratio */
            BBB_DDR3_DATA_WR_DATA_SLAVE_RATIO  /* wr data slave ratio */
        }                                      /* ddrEmifPhy_t */
};

static void vtpEnable(void)
{
    /* Enable VTP */
    HW_WR_FIELD32_RAW(SOC_CONTROL_REGS + CONTROL_VTP_CTRL, CONTROL_VTP_CTRL_ENABLE,
                      CONTROL_VTP_CTRL_ENABLE_SHIFT, 1U);
    HW_WR_FIELD32_RAW(SOC_CONTROL_REGS + CONTROL_VTP_CTRL, CONTROL_VTP_CTRL_CLRZ,
                      CONTROL_VTP_CTRL_CLRZ_SHIFT, 0U);
    HW_WR_FIELD32_RAW(SOC_CONTROL_REGS + CONTROL_VTP_CTRL, CONTROL_VTP_CTRL_CLRZ,
                      CONTROL_VTP_CTRL_CLRZ_SHIFT, 1U);

    while (1U != HW_RD_FIELD32_RAW(SOC_CONTROL_REGS + CONTROL_VTP_CTRL,
                                   CONTROL_VTP_CTRL_READY, CONTROL_VTP_CTRL_READY_SHIFT))
    {
    }
}

static void ddrPhyInit(ddrEmifPhyCfg_t *pDdrEmifPhyCfg)
{
    volatile uint32_t regVal;

    /* Enable VTP */
    vtpEnable();

    /* DDR PHY CMD Register configuration */
    regVal = pDdrEmifPhyCfg->slaveRatio;
    HW_WR_REG32((CMD0_SLAVE_RATIO_0), regVal);
    HW_WR_REG32((CMD1_SLAVE_RATIO_0), regVal);
    HW_WR_REG32((CMD2_SLAVE_RATIO_0), regVal);

    regVal = pDdrEmifPhyCfg->invertClkout;
    HW_WR_REG32((CMD0_INVERT_CLKOUT_0), regVal);
    HW_WR_REG32((CMD1_INVERT_CLKOUT_0), regVal);
    HW_WR_REG32((CMD2_INVERT_CLKOUT_0), regVal);

    /* DDR PHY DATA Register configuration */
    regVal = pDdrEmifPhyCfg->rdDqsSlaveRatio;
    HW_WR_REG32((DATA0_RD_DQS_SLAVE_RATIO_0), regVal);
    HW_WR_REG32((DATA1_RD_DQS_SLAVE_RATIO_0), regVal);

    regVal = pDdrEmifPhyCfg->wrDqsSlaveRatio;
    HW_WR_REG32((DATA0_WR_DQS_SLAVE_RATIO_0), regVal);
    HW_WR_REG32((DATA1_WR_DQS_SLAVE_RATIO_0), regVal);

    regVal = pDdrEmifPhyCfg->fifoWeSlaveRatio;
    HW_WR_REG32((DATA0_FIFO_WE_SLAVE_RATIO_0), regVal);
    HW_WR_REG32((DATA1_FIFO_WE_SLAVE_RATIO_0), regVal);

    regVal = pDdrEmifPhyCfg->wrDataSlaveRatio;
    HW_WR_REG32((DATA0_WR_DATA_SLAVE_RATIO_0), regVal);
    HW_WR_REG32((DATA1_WR_DATA_SLAVE_RATIO_0), regVal);
}

void ddrConfig(uint32_t dynPwrDown, ddrCfg_t *pDdrCfg)
{
    volatile uint32_t regVal, structVal;
    ddrCtrlCfg_t *pDdrCtrlCfg = &pDdrCfg->ddrCtrlCfg;
    ddrEmifCfg_t *pDdrEmifCfg = &pDdrCfg->ddrEmifCfg;

    /* DDR2 Phy Initialization */
    ddrPhyInit(&pDdrCfg->ddrEmifPhyCfg);

    regVal = pDdrCtrlCfg->addrIoCtrl;
    HW_WR_REG32((SOC_CONTROL_REGS + CONTROL_DDR_CMD_IOCTRL(0)), regVal);
    HW_WR_REG32((SOC_CONTROL_REGS + CONTROL_DDR_CMD_IOCTRL(1)), regVal);
    HW_WR_REG32((SOC_CONTROL_REGS + CONTROL_DDR_CMD_IOCTRL(2)), regVal);

    regVal = pDdrCtrlCfg->dataIoCtrl;
    HW_WR_REG32((SOC_CONTROL_REGS + CONTROL_DDR_DATA_IOCTRL(0)), regVal);
    HW_WR_REG32((SOC_CONTROL_REGS + CONTROL_DDR_DATA_IOCTRL(1)), regVal);

    /* IO to work for DDR */
    regVal = HW_RD_REG32(SOC_CONTROL_REGS + CONTROL_DDR_IO_CTRL);
    structVal = pDdrCtrlCfg->ddrIoCtrl;
    regVal &= structVal;
    HW_WR_REG32((SOC_CONTROL_REGS + CONTROL_DDR_IO_CTRL), regVal);

    HW_WR_FIELD32_RAW(SOC_CONTROL_REGS + CONTROL_DDR_CKE_CTRL,
                      CONTROL_DDR_CKE_CTRL_DDR_CKE_CTRL, CONTROL_DDR_CKE_CTRL_DDR_CKE_CTRL_SHIFT, 1U);

    regVal = pDdrEmifCfg->ddrPhyCtrl;
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1), regVal);

    /* Dynamic Power Down */
    if (TRUE == dynPwrDown)
    {
        regVal = HW_RD_REG32(SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1);
        structVal = pDdrEmifCfg->ddrPhyCtrlDyPwrdn;
        regVal |= structVal;
        HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1), regVal);
    }
    regVal = pDdrEmifCfg->ddrPhyCtrl;
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1_SHDW),
                regVal);

    /* Dynamic Power Down */
    if (TRUE == dynPwrDown)
    {
        regVal = HW_RD_REG32(SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1_SHDW);
        structVal = pDdrEmifCfg->ddrPhyCtrlDyPwrdn;
        regVal |= structVal;
        HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1_SHDW), regVal);
    }
    regVal = pDdrEmifCfg->ddrPhyCtrl;
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_2), regVal);

    regVal = pDdrEmifCfg->sdramTim1;
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_1), regVal);
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_1_SHDW), regVal);

    regVal = pDdrEmifCfg->sdramTim2;
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_2), regVal);
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_2_SHDW), regVal);

    regVal = pDdrEmifCfg->sdramTim3;
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_3), regVal);
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_3_SHDW), regVal);

    regVal = pDdrEmifCfg->sdramCfg;
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_SDRAM_CONFIG), regVal);

    regVal = pDdrEmifCfg->sdramRefCtrl1;
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_SDRAM_REF_CTRL), regVal);
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_SDRAM_REF_CTRL_SHDW), regVal);

    regVal = pDdrEmifCfg->zqConfig;
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_ZQ_CONFIG), regVal);

    regVal = pDdrEmifCfg->sdramCfg;
    HW_WR_REG32((SOC_EMIF_0_REGS + EMIF_SDRAM_CONFIG), regVal);

    /* The CONTROL_SECURE_EMIF_SDRAM_CONFIG register exports SDRAM configuration
       information to the EMIF */
    HW_WR_REG32((SOC_CONTROL_REGS + CONTROL_SECURE_EMIF_SDRAM_CONFIG), regVal);
}

void ddrEmifInit(void)
{
    HW_WR_FIELD32_RAW(SOC_CM_PER_REGS + CM_PER_EMIF_FW_CLKCTRL,
                      CM_PER_EMIF_FW_CLKCTRL_MODULEMODE,
                      CM_PER_EMIF_FW_CLKCTRL_MODULEMODE_SHIFT,
                      CM_PER_EMIF_FW_CLKCTRL_MODULEMODE_ENABLE);

    HW_WR_FIELD32_RAW(SOC_CM_PER_REGS + CM_PER_EMIF_CLKCTRL,
                      CM_PER_EMIF_CLKCTRL_MODULEMODE,
                      CM_PER_EMIF_CLKCTRL_MODULEMODE_SHIFT,
                      CM_PER_EMIF_CLKCTRL_MODULEMODE_ENABLE);

    while ((CM_PER_L3_CLKSTCTRL_CLKACTIVITY_EMIF_GCLK_ACT !=
            HW_RD_FIELD32_RAW(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL,
                              CM_PER_L3_CLKSTCTRL_CLKACTIVITY_EMIF_GCLK,
                              CM_PER_L3_CLKSTCTRL_CLKACTIVITY_EMIF_GCLK_SHIFT)) ||
           (CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK_ACT !=
            HW_RD_FIELD32_RAW(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL,
                              CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK,
                              CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK_SHIFT)))
    {
    }
}

void ddrInit()
{
    uint32_t dynPwrDown = FALSE;
    ddrCfg_t ddrCfg = BBB_DDR3_CONFIG;

    /* EMIF Initialization */
    ddrEmifInit();

    ddrConfig(dynPwrDown, &ddrCfg);
}