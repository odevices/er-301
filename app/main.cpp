#include <hal/priorities.h>
#include <hal/board.h>
#include <hal/gpio.h>
#include <hal/events.h>
#include <hal/modulation.h>
#include <hal/audio.h>
#include <hal/i2c.h>
#include <hal/adc.h>
#include <hal/rng.h>
#include <hal/pwm.h>
#include <hal/display.h>
#include <hal/card.h>
#include <hal/usb.h>
#include <hal/encoder.h>
#include <hal/uart.h>
#include <hal/dma.h>
#include <hal/heap.h>
#include <hal/log.h>
#include <hal/pump.h>
#include <od/config.h>
#include <od/glue/AppInterpreter.h>
#include <od/extras/Random.h>

#include <xdc/runtime/Error.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

typedef struct _Local
{
  Task_Handle appTaskHandle;
  od::AppInterpreter appInterp;
} Local;

static Local local;

extern "C"
{
  void _exit(int rc);
  void __assert_func(const char *file, int line, const char *func, const char *msg)
  {
    logFatal("Assertion:%s(%d):%s:%s", file, line, func, msg);
    _exit(0);
  }

  // This callback is setup in the sysbios cfg script.
  Void onSystemError(Error_Block *eb)
  {
    static int count = 0;
    count++;
    if (count == 1)
    {
      Error_print(eb);
      logError("onSystemError called.");
    }
  }

  // This callback is setup in the sysbios cfg script.
  Void onTaskSwitch(Task_Handle prev, Task_Handle next)
  {
    if (next == local.appTaskHandle)
    {
      // Reset the pump throttle when the main task is about to run.
      Pump_resetThrottle();
    }
  }

  Void app_task(UArg arg0, UArg arg1)
  {
    logInfo("Task(app): started.");
#ifdef BUILDOPT_TESTING
    Uart_enable();
#endif
#if TRACK_REFCOUNTED_OBJECTS
    ReferenceCounted::enablePrinting();
#endif

    Timing_init();
    Card_init();
    Config_init("0:/firmware.cfg", "1:", "0:", "x:");
    Pump_init();
    USB_init();
    Encoder_init();
    Pwm_init();
    Adc_init();
    Modulation_init();
    Audio_init();
    Display_init();
    
    local.appInterp.init();
    local.appInterp.execute("app.roots = {x='x:',front='1:',rear='0:'}");
    local.appInterp.execute("dofile('x:/boot/logging.lua')");
    local.appInterp.execute("dofile('x:/boot/start.lua')");
  }

  Int main()
  {
    Board_init();
    Heap_init();
    Uart_init();
    DMA_init();
    Rng_init();
    Gpio_init();
    Events_init();
    Log_init();

#if 0
  Priorities_enableTaskSlicing(20)
#endif

    {
      Task_Params params;
      Task_Params_init(&params);
      params.priority = TASK_PRIORITY_MAIN;
      params.stackSize = 32 * 1024;
      params.env = (Ptr)"app";
      local.appTaskHandle = Task_create(app_task, &params, NULL);
    }

    BIOS_start(); /* does not return */
    return 0;
  }

} // extern "C"