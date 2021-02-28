#include <hal/usb/USBThread.h>
#include <hal/priorities.h>
#include <hal/board.h>
#include <hal/usb.h>
#define BUILDOPT_VERBOSE
#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <ti/am335x/hw_usb.h>
#include <ti/sysbios/family/arm/a8/intcps/Hwi.h>

namespace od
{

  static Hwi_Handle registerHandler(Int intNum, Hwi_FuncPtr hwiFxn, UArg arg)
  {
    Hwi_Params params;
    Hwi_Params_init(&params);
    params.priority = HWI_PRIORITY_USB;
    params.arg = arg;
    return Hwi_create(intNum, hwiFxn, &params, NULL);
  }

  USBThread::USBThread() : Thread("usb", TASK_PRIORITY_USB)
  {
  }

  USBThread::~USBThread()
  {
  }

  void USBThread::usbInterruptHandler(unsigned int arg)
  {
    Hwi_disableInterrupt(SYS_INT_USB0);
    USBThread *thread = (USBThread *)arg;
    logAssert(thread);
    thread->mEvents.post(USBThread::onUsbEvent);
  }

  void USBThread::dmaInterruptHandler(unsigned int arg)
  {
    Hwi_disableInterrupt(SYS_INT_USBSSINT);
    USBThread *thread = (USBThread *)arg;
    logAssert(thread);
    thread->mEvents.post(USBThread::onDmaEvent);
  }

  void USBThread::run()
  {
    Board_enableUSB();

    while (1)
    {
      Hwi_Handle hUsbHwi;
      Hwi_Handle hDmaHwi;

      // All USB device events are handled in this thread context
      uint32_t result = mEvents.waitForAny(onThreadQuit |
                                           onUsbEvent |
                                           onDmaEvent |
                                           onStartEvent |
                                           onStopEvent);

      mMutex.enter();
      if (mpActiveDevice)
      {
        /* calling the main event handler */
        if (result & onUsbEvent)
        {
          mpActiveDevice->handleUSBInterrupt();
          Hwi_enableInterrupt(SYS_INT_USB0);
        }

        /* calling the DMA event handler */
        if (result & onDmaEvent)
        {
          mpActiveDevice->handleDMAInterrupt();
          Hwi_enableInterrupt(SYS_INT_USBSSINT);
        }

        if (result & onStopEvent)
        {
          logDebug(1, "stopping device");
          mpActiveDevice->stop();
          mpActiveDevice = 0;
          Hwi_delete(&hUsbHwi);
          Hwi_delete(&hDmaHwi);
        }
      }

      if (mpActiveDevice == 0 && (result & onStartEvent))
      {
        mpActiveDevice = mpPendingDevice;
        if (mpActiveDevice)
        {
          hUsbHwi = registerHandler(SYS_INT_USB0, usbInterruptHandler, (UArg)this);
          hDmaHwi = registerHandler(SYS_INT_USBSSINT, dmaInterruptHandler, (UArg)this);
          logDebug(1, "starting device");
          mpActiveDevice->start();
        }
      }
      mMutex.leave();

      if (result & onThreadQuit)
      {
        break;
      }
    }

    Board_disableUSB();
  }

  void USBThread::setDevice(od::USBDevice *pDevice)
  {
    if (mpActiveDevice)
    {
      if (mpActiveDevice != pDevice)
      {
        stopDevice();
        mpPendingDevice = pDevice;
        Thread::sleep(250);
        startDevice();
      }
    }
    else
    {
      mpPendingDevice = pDevice;
    }
  }

  void USBThread::startDevice()
  {
    if (mpActiveDevice == 0)
    {
      mEvents.post(onStartEvent);
      while (mpActiveDevice == 0)
      {
        Thread::sleep(100);
      }
    }
  }

  void USBThread::stopDevice()
  {
    if (mpActiveDevice)
    {
      mEvents.post(onStopEvent);
      while (mpActiveDevice)
      {
        Thread::sleep(100);
      }
    }
  }

  bool USBThread::waitForHostToConnect()
  {
    if (mpActiveDevice)
    {
      while (mpActiveDevice && !mpActiveDevice->isConfigured())
      {
        Thread::sleep(100);
      }
    }
    return mpActiveDevice && mpActiveDevice->isConfigured();
  }

} //  namespace od