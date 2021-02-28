
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
#include <ti/am335x/musb.h>
#include <ti/am335x/hw_usb.h>

/** Enable relevant sections of the generated pinmux file. */
#define BUILDCFG_MOD_RTC
#define BUILDCFG_MOD_GPIO
#define BUILDCFG_MOD_ADC
#define BUILDCFG_MOD_MCASP
#define BUILDCFG_MOD_LCDC
#define BUILDCFG_MOD_PWMSS
#define BUILDCFG_MOD_MMCSD
#define BUILDCFG_MOD_USB
#define BUILDCFG_MOD_SPI
#define BUILDCFG_MOD_UART
#define BUILDCFG_MOD_I2C
#include "am335x_pinmux.h"
#include "pinmux_data.h"

#define PRCM_MODULEMODE_DISABLE (0U)
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
    ;
  /* Check clock activity - ungated */
  while (clkActMask != (HW_RD_REG32(domainOffset + clkStCtrlReg) & clkActMask))
    ;
  /* Check idle status value - should be in functional state */
  while ((PRCM_MODULE_IDLEST_FUNC << PRCM_IDLE_ST_SHIFT) !=
         (HW_RD_REG32(domainOffset + clkCtrlReg) & PRCM_IDLE_ST_MASK))
    ;
}

static void disableModule(uint32_t domainOffset, uint32_t clkCtrlReg)
{
  /* Disable the module */
  HW_WR_REG32(domainOffset + clkCtrlReg, PRCM_MODULEMODE_DISABLE);
  /* Check for module disable status */
  while (PRCM_MODULEMODE_DISABLE !=
         (HW_RD_REG32(domainOffset + clkCtrlReg) & PRCM_MODULEMODE_MASK))
    ;
}

static void applyPinMuxConfig(pinmuxPerCfg_t *pConfigData)
{
  uint32_t i;
  for (i = 0; (uint16_t)PINMUX_INVALID_PIN !=
              pConfigData[i].pinOffset;
       i++)
  {
    HW_WR_REG32((SOC_CONTROL_REGS + pConfigData[i].pinOffset),
                pConfigData[i].pinSettings);
  }
}

/* Has an unusual initialization procedure... */
void Board_enableI2C2()
{
  uint32_t regVal = HW_RD_REG32(SOC_PRCM_REGS + CM_PER_I2C2_CLKCTRL);
  regVal |= CM_PER_I2C2_CLKCTRL_MODULEMODE_ENABLE;
  HW_WR_REG32(SOC_PRCM_REGS + CM_PER_I2C2_CLKCTRL, regVal);

  /* Waiting for MODULEMODE field to reflect the written value */
  while (CM_PER_I2C2_CLKCTRL_MODULEMODE_ENABLE !=
         (HW_RD_REG32(SOC_PRCM_REGS + CM_PER_I2C2_CLKCTRL) &
          CM_PER_I2C2_CLKCTRL_MODULEMODE))
    ;

  regVal = HW_RD_REG32(SOC_PRCM_REGS + CM_PER_I2C2_CLKCTRL);
  while ((regVal & CM_PER_I2C2_CLKCTRL_MODULEMODE) !=
         CM_PER_I2C2_CLKCTRL_MODULEMODE_ENABLE)
    ;

  /*
    ** Waiting for CLKACTIVITY_I2C2_FCLK field in CM_PER_L4LS_CLKSTCTRL
    ** to attain desired value.
    */
  while (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_I2C_FCLK !=
         (HW_RD_REG32(SOC_PRCM_REGS + CM_PER_L4LS_CLKSTCTRL) &
          CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_I2C_FCLK))
    ;

  /*
    ** Waiting for IDLEST field in CM_PER_I2C2_CLKCTRL register to attain
    ** desired value.
    */
  while ((CM_PER_I2C2_CLKCTRL_IDLEST_FUNC << CM_PER_I2C2_CLKCTRL_IDLEST_SHIFT) !=
         (HW_RD_REG32(SOC_PRCM_REGS + CM_PER_I2C2_CLKCTRL) &
          CM_PER_I2C2_CLKCTRL_IDLEST))
    ;

  regVal = HW_RD_REG32(SOC_PRCM_REGS + CM_PER_L4LS_CLKSTCTRL);
  while (!((regVal) & (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK |
                       CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_I2C_FCLK)))
    ;
}

void Board_pinmuxI2C2()
{
  applyPinMuxConfig(gI2c2PinCfg);
}

void Board_pinmuxUART0()
{
  applyPinMuxConfig(gUart0PinCfg);
}

void Board_init()
{

  /* GPIO(0,1,2,3) */
  enableModule(SOC_CM_WKUP_REGS, CM_WKUP_GPIO0_CLKCTRL,
               CM_WKUP_CLKSTCTRL,
               CM_WKUP_CLKSTCTRL_CLKACTIVITY_L4_WKUP_GCLK);
  enableModule(SOC_CM_PER_REGS, CM_PER_GPIO1_CLKCTRL,
               CM_PER_L4LS_CLKSTCTRL,
               CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK);
  enableModule(SOC_CM_PER_REGS, CM_PER_GPIO2_CLKCTRL,
               CM_PER_L4LS_CLKSTCTRL,
               CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK);
  enableModule(SOC_CM_PER_REGS, CM_PER_GPIO3_CLKCTRL,
               CM_PER_L4LS_CLKSTCTRL,
               CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK);
  applyPinMuxConfig(gGpio0PinCfg);
  applyPinMuxConfig(gGpio1PinCfg);
  applyPinMuxConfig(gGpio2PinCfg);
  applyPinMuxConfig(gGpio3PinCfg);

  /* UART(0) */
  enableModule(SOC_CM_WKUP_REGS, CM_WKUP_UART0_CLKCTRL,
               CM_WKUP_CLKSTCTRL,
               CM_WKUP_CLKSTCTRL_CLKACTIVITY_UART0_GFCLK);
  Board_pinmuxUART0();

#ifdef BUILDOPTION_ENABLE_RTC
  /* RTC */
  enableModule(SOC_CM_RTC_REGS, CM_RTC_RTC_CLKCTRL,
               CM_RTC_CLKSTCTRL,
               CM_RTC_CLKSTCTRL_CLKACTIVITY_L4_RTC_GCLK);
  enableModule(SOC_CM_RTC_REGS, CM_RTC_RTC_CLKCTRL,
               CM_RTC_CLKSTCTRL,
               CM_RTC_CLKSTCTRL_CLKACTIVITY_RTC_32KCLK);
  applyPinMuxConfig(gRtc0PinCfg);
#endif

  /* EDMA3CC and EDMA3TC(0,1,2) */
  enableModule(SOC_CM_PER_REGS, CM_PER_TPCC_CLKCTRL,
               CM_PER_L3_CLKSTCTRL, CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK);
  enableModule(SOC_CM_PER_REGS, CM_PER_TPTC0_CLKCTRL,
               CM_PER_L3_CLKSTCTRL, CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK);
  enableModule(SOC_CM_PER_REGS, CM_PER_TPTC1_CLKCTRL,
               CM_PER_L3_CLKSTCTRL, CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK);
  enableModule(SOC_CM_PER_REGS, CM_PER_TPTC2_CLKCTRL,
               CM_PER_L3_CLKSTCTRL, CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK);

  /* MMCSD(0,1) */
  enableModule(SOC_CM_PER_REGS, CM_PER_MMC0_CLKCTRL,
               CM_PER_L3_CLKSTCTRL, CM_PER_L3_CLKSTCTRL_CLKACTIVITY_MMC_FCLK);
  enableModule(SOC_CM_PER_REGS, CM_PER_MMC1_CLKCTRL,
               CM_PER_L3_CLKSTCTRL, CM_PER_L3_CLKSTCTRL_CLKACTIVITY_MMC_FCLK);
  applyPinMuxConfig(gMmcsd0PinCfg);
  applyPinMuxConfig(gMmcsd1PinCfg);

  /* MCASP(0) */
  enableModule(SOC_CM_PER_REGS, CM_PER_MCASP0_CLKCTRL,
               CM_PER_L3S_CLKSTCTRL, CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK);
  applyPinMuxConfig(gMcasp0PinCfg);

  /* MCSPI(0,1) */
  enableModule(SOC_CM_PER_REGS, CM_PER_SPI0_CLKCTRL,
               CM_PER_L3_CLKSTCTRL, CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK);
  enableModule(SOC_CM_PER_REGS, CM_PER_SPI1_CLKCTRL,
               CM_PER_L3_CLKSTCTRL, CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK);
  applyPinMuxConfig(gSpi0PinCfg);
  applyPinMuxConfig(gSpi1PinCfg);

  /* LCDC */
  enableModule(SOC_CM_PER_REGS, CM_PER_LCDC_CLKCTRL,
               CM_PER_LCDC_CLKSTCTRL,
               CM_PER_LCDC_CLKSTCTRL_CLKACTIVITY_LCDC_L3_OCP_GCLK);
  applyPinMuxConfig(gLcdc0PinCfg);

  /* PWM(0)*/
  enableModule(SOC_CM_PER_REGS, CM_PER_EPWMSS0_CLKCTRL,
               CM_PER_L4LS_CLKSTCTRL,
               CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK);
  applyPinMuxConfig(gPwmss0PinCfg);

  /* ADC */
  enableModule(SOC_CM_WKUP_REGS, CM_WKUP_ADC_TSC_CLKCTRL,
               CM_WKUP_CLKSTCTRL,
               CM_WKUP_CLKSTCTRL_CLKACTIVITY_ADC_FCLK);

  /* WDT */
  enableModule(SOC_CM_WKUP_REGS, CM_WKUP_WDT1_CLKCTRL,
               CM_WKUP_CLKSTCTRL,
               CM_WKUP_CLKSTCTRL_CLKACTIVITY_WDT1_GCLK);
}

void Board_enableUSB()
{
  /* USB(0) */
  enableModule(SOC_CM_PER_REGS, CM_PER_USB0_CLKCTRL,
               CM_PER_L3S_CLKSTCTRL, CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK);
  USBDevDisconnect(USB0_BASE);
}

void Board_disableUSB()
{
  /* USB(0) */
  USBDevDisconnect(USB0_BASE);
  disableModule(SOC_CM_PER_REGS, CM_PER_USB0_CLKCTRL);
}