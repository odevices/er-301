
#include <stdint.h>
#include <stdbool.h>
#include <hal/types.h>
#include <hal/board.h>
#include <ti/am335x/hw_types.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/chipdb.h>
#include <ti/am335x/hw_cm_per.h>
#include <ti/am335x/hw_cm_wkup.h>
#include <ti/am335x/hw_cm_rtc.h>
#include <ti/am335x/wdt.h>
#include "ddr.h"
#include "pll.h"

/** Enable relevant sections of the generated pinmux file. */
//#define BUILDCFG_MOD_RTC
//#define BUILDCFG_MOD_GPIO
//#define BUILDCFG_MOD_ADC
//#define BUILDCFG_MOD_MCASP
//#define BUILDCFG_MOD_LCDC
//#define BUILDCFG_MOD_PWMSS
#define BUILDCFG_MOD_MMCSD
//#define BUILDCFG_MOD_USB
//#define BUILDCFG_MOD_SPI
#define BUILDCFG_MOD_UART
//#define BUILDCFG_MOD_I2C
#include "am335x_pinmux.h"
#include "pinmux_data.h"

#define PRCM_MODULEMODE_ENABLE (2U)
#define PRCM_MODULEMODE_MASK (3U)
#define PRCM_IDLE_ST_MASK (0x00030000U)
#define PRCM_IDLE_ST_SHIFT (16U)

#define PRCM_MODULE_IDLEST_FUNC (0U)
#define PRCM_MODULE_IDLEST_TRANS (1U)
#define PRCM_MODULE_IDLEST_IDLE (2U)
#define PRCM_MODULE_IDLEST_DISABLE (3U)

static void enableModule(uint32_t domainOffset, uint32_t clkCtrlReg,
                         uint32_t clkStCtrlReg, uint32_t clkActMask)
{
    /* Enable the module */
    HW_WR_REG32(domainOffset + clkCtrlReg, PRCM_MODULEMODE_ENABLE);
    /* Check for module enable status */
    while (PRCM_MODULEMODE_ENABLE !=
           (HW_RD_REG32(domainOffset + clkCtrlReg) & PRCM_MODULEMODE_MASK))
    {
    }

    /* Check clock activity - ungated */
    while (clkActMask != (HW_RD_REG32(domainOffset + clkStCtrlReg) & clkActMask))
    {
    }

    /* Check idle status value - should be in functional state */
    while ((PRCM_MODULE_IDLEST_FUNC << PRCM_IDLE_ST_SHIFT) !=
           (HW_RD_REG32(domainOffset + clkCtrlReg) & PRCM_IDLE_ST_MASK))
    {
    }
}

static void applyPinMuxConfig(pinmuxPerCfg_t *pConfigData)
{
    uint32_t i;
    for (i = 0; (uint16_t)PINMUX_INVALID_PIN != pConfigData[i].pinOffset; i++)
    {
        HW_WR_REG32((SOC_CONTROL_REGS + pConfigData[i].pinOffset),
                    pConfigData[i].pinSettings);
    }
}

void Board_init()
{
#if 1
    /* Init PLL */
    pllInit();
    /* Enable the control module */
    HW_WR_REG32((SOC_CM_WKUP_REGS + CM_WKUP_CONTROL_CLKCTRL),
                CM_WKUP_CONTROL_CLKCTRL_MODULEMODE_ENABLE);
    /* Init DDR */
    ddrInit();
#endif

    /* UART(0) */
    enableModule(SOC_CM_WKUP_REGS, CM_WKUP_UART0_CLKCTRL,
                 CM_WKUP_CLKSTCTRL, CM_WKUP_CLKSTCTRL_CLKACTIVITY_UART0_GFCLK);
    applyPinMuxConfig(gUart0PinCfg);

#if 0
    /* RTC */
    enableModule(SOC_CM_RTC_REGS, CM_RTC_RTC_CLKCTRL,
                 CM_RTC_CLKSTCTRL, CM_RTC_CLKSTCTRL_CLKACTIVITY_L4_RTC_GCLK);

    enableModule(SOC_CM_RTC_REGS, CM_RTC_RTC_CLKCTRL,
                 CM_RTC_CLKSTCTRL, CM_RTC_CLKSTCTRL_CLKACTIVITY_RTC_32KCLK);

    applyPinMuxConfig(gRtc0PinCfg);
#endif

    /* MMCSD(0) */
    enableModule(SOC_CM_PER_REGS, CM_PER_MMC0_CLKCTRL,
                 CM_PER_L3_CLKSTCTRL, CM_PER_L3_CLKSTCTRL_CLKACTIVITY_MMC_FCLK);
    applyPinMuxConfig(gMmcsd0PinCfg);

    /* WDT */
    enableModule(SOC_CM_WKUP_REGS, CM_WKUP_WDT1_CLKCTRL,
                 CM_WKUP_CLKSTCTRL, CM_WKUP_CLKSTCTRL_CLKACTIVITY_WDT1_GCLK);

    /* Disable the WDT */
    WDTEnable(SOC_WDT_1_REGS, false);
}
