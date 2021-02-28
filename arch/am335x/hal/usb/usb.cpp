#include <hal/usb/USBThread.h>
#include <hal/usb/MassStorage.h>
#include <hal/usb/SerialPort.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <hal/constants.h>
#include <hal/ops.h>

static od::USBThread usbThread;
static od::MassStorage massStorageDevice;
static od::SerialPort serialPortDevice;

extern "C"
{

  void USB_init()
  {
    massStorageDevice.init();
    serialPortDevice.init();
    usbThread.start();
    usbThread.setDevice(&massStorageDevice);
  }

  void USB_setMassStorageMode()
  {
    usbThread.setDevice(&massStorageDevice);
  }

  void USB_setSerialPortMode()
  {
    usbThread.setDevice(&serialPortDevice);
  }

  void USB_start()
  {
    usbThread.startDevice();
  }

  void USB_stop()
  {
    usbThread.stopDevice();
  }

  static char gSendBuffer[CACHE_ALIGNED_SIZE(512)] __attribute__((aligned(CACHELINE_SIZE_MAX)));

  static int writeHelper(const char *buffer, int size)
  {
    bool writeStarted = false;

    usbThread.mMutex.enter();
    if (usbThread.mpActiveDevice == &serialPortDevice)
    {
      int n = 0;
      for (int i = 0; (i < size) && (n + 2 < (int)sizeof(gSendBuffer)); i++)
      {
        if (buffer[i] == '\n')
        {
          gSendBuffer[n++] = '\r';
        }
        gSendBuffer[n++] = buffer[i];
      }
      writeStarted = serialPortDevice.startWrite(gSendBuffer, n);
    }
    usbThread.mMutex.leave();

    if (writeStarted)
    {
      return serialPortDevice.waitForWriteCompletion();
    }
    else
    {
      return 0;
    }
  }

  int USB_write(const char *buffer, int size)
  {
    int sofar = 0;
    int remaining = size;
    while (remaining > 0)
    {
      int written = writeHelper(buffer + sofar, remaining);
      if (written > 0)
      {
        remaining -= written;
        sofar += written;
      }
      else
      {
        break;
      }
    }
    return sofar;
  }

  int USB_read(char *buffer, int size)
  {
    bool readStarted = false;

    if (usbThread.mpActiveDevice == &serialPortDevice)
    {
      usbThread.mMutex.enter();
      readStarted = serialPortDevice.startRead(buffer, size);
      usbThread.mMutex.leave();
    }

    if (readStarted)
    {
      return serialPortDevice.waitForReadCompletion();
    }
    else
    {
      return 0;
    }
  }

  bool USB_waitForHostToConnect()
  {
    return usbThread.waitForHostToConnect();
  }
}