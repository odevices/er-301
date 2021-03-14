#include <limits.h>
#include "resource.h"
#include <_ansi.h>
#include <_syslist.h>
#include <hal/gpio.h>
#include <hal/constants.h>

void reboot();

void _exit(int rc)
{
    // mute the audio DAC
    Gpio_write(PCM4104_MUTE, 1);
    // Breakpoint();
    // Check for 3 button chord to reboot
    while (1)
    {
        if (!Gpio_read(BUTTON_DIAL1) && !Gpio_read(BUTTON_DIAL2) && !Gpio_read(BUTTON_DIAL3))
        {
            reboot();
        }
    }
    /* Convince GCC that this function never returns.  */
    while (1)
    {
    }
}
