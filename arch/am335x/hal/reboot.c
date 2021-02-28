#include <ti/am335x/wdt.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/types.h>

void startWatchDogTimer(uint32_t ticks)
{
  /* Reset the Watchdog timer. */
  WDTReset(SOC_WDT_1_REGS);

  /* Disable the Watchdog timer. */
  WDTEnable(SOC_WDT_1_REGS, FALSE);

  /* Enable and configure the prescaler clock. */
  WDTPrescalerClkEnable(SOC_WDT_1_REGS, WDT_PRESCALER_CLK_DIV_1);

  /* Configure the WDT counter. */
  WDTSetCounter(SOC_WDT_1_REGS, 0xFFFFFFFF - ticks);

  /* Configure the WDT reload value. */
  WDTSetReloadVal(SOC_WDT_1_REGS, 0xFFFFFFFF - ticks);

  /* Enable the Watchdog timer. */
  WDTEnable(SOC_WDT_1_REGS, TRUE);
}

// hard reboot
void reboot()
{
  startWatchDogTimer(1000);
}
