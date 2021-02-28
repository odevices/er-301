#pragma once

#include <hal/usb/USBDevice.h>
#include <hal/concurrency/Mutex.h>
#include <hal/concurrency/Thread.h>

namespace od
{
  class USBThread : public Thread
  {
  public:
    USBThread();
    virtual ~USBThread();

    void startDevice();
    void stopDevice();
    void setDevice(od::USBDevice *pDevice);
    bool waitForHostToConnect();

    Mutex mMutex;
    USBDevice *mpActiveDevice = 0;
    USBDevice *mpPendingDevice = 0;

  protected:
    static void usbInterruptHandler(unsigned int);
    static void dmaInterruptHandler(unsigned int);
    virtual void run();

    static const uint32_t onUsbEvent = od::EventFlags::flag01;
    static const uint32_t onDmaEvent = od::EventFlags::flag02;
    static const uint32_t onStartEvent = od::EventFlags::flag03;
    static const uint32_t onStopEvent = od::EventFlags::flag04;
  };
} // namespace od