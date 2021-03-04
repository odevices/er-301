#include "KernelLoader.h"

#include <hal/board.h>
#include <hal/dma.h>
#include <hal/gpio.h>
#include <hal/card.h>
#include <hal/uart.h>
#include <hal/usb.h>
#include <hal/display.h>
#include <hal/log.h>
#include <hal/priorities.h>
#include <hal/events.h>

#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/family/arm/a8/intcps/Hwi.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/System.h>

static od::KernelLoader loader;

extern "C"
{

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

  static void onExit(int status)
  {
    if (loader.EntryPoint)
    {
      void (*pfnSBLAppEntry)();
      DMA_deinit();
      logInfo("Jumping to kernel at 0x%x", loader.EntryPoint);
      pfnSBLAppEntry = (void (*)(void))loader.EntryPoint;
      (*pfnSBLAppEntry)();
    }
  }

  static void onDisplayReady()
  {
    // draw a frame (if available)
    DisplayBuffer *frame = Display_getBuffer();
    if (frame)
    {
      loader.draw(frame->main, frame->sub);
      // place on queue to have it sent to the LCD
      Display_putBuffer(frame);
    }
  }

  static void processEvents()
  {
    Events_wait();
    while (1)
    {
      uint32_t e = Events_pull();
      if (e == EVENT_NONE)
      {
        break;
      }
      else if (e == EVENT_DISPLAY_READY)
      {
        onDisplayReady();
      }
    }
  }

  Void sbl_task(UArg a0, UArg a1)
  {
    Card_init();
    Display_init();
    Display_start();
    Card_mount(0);

#define USB_TESTING 0
#if USB_TESTING
    USB_init();
    Card_mount(1);
    USB_start();
    USB_testWrite();
    USB_testEnumerate();
    USB_testEcho();
#else
    System_atexit(onExit);

    od::JobQueue thread("jobq");
    loader.init();
    loader.attach(); // Do not destruct when thread releases.
    thread.push(&loader);
    thread.start();

    // event loop
    while (!loader.finished())
    {
      processEvents();
    }

    // A 1s pause.
    int count = GRAPHICS_REFRESH_RATE;
    while (--count > 0)
    {
      processEvents();
    }

    thread.stop();
    thread.join();

    Display_stop();
    Hwi_disable();

    BIOS_exit(0);
#endif
  }

  Int main()
  {
    Board_init();
    DMA_init();
    Gpio_init();
    Events_init();

#ifdef BUILDOPT_TESTING
    Uart_init();
    Uart_enable();
    Log_init();
    logInfo("Starting SBL (%s)", SBL_VERSION);
#endif

    {
      Task_Params params;
      Task_Params_init(&params);
      params.priority = TASK_PRIORITY_MAIN;
      params.stackSize = 32 * 1024;
      params.env = (Ptr)"sbl";
      Task_create(sbl_task, &params, NULL);
    }

    BIOS_start(); /* does not return */
    return (0);
  }
} // extern "C"