#include <hal/usb.h>
#include <hal/log.h>
#include <stdio.h>
#include <stdbool.h>

static struct USBLocals
{
  enum Mode {
    massStorage,
    serialPort,
    off
  };

  Mode mode = off;
  bool started = false;
} local;

extern "C"
{
  void USB_init(void)
  {
  }

  void USB_start(void)
  {
    local.started = true;
  }

  void USB_stop(void)
  {
    local.started = false;
  }

  void USB_setMassStorageMode()
  {
    local.mode = USBLocals::massStorage;
  }

  void USB_setSerialPortMode()
  {
    local.mode = USBLocals::serialPort;
  }

  int USB_write(const char *buffer, int size)
  {
    if (local.started && local.mode == USBLocals::serialPort)
    {
      int written = fwrite(buffer, size, 1, stderr);
      return written;
    }
    return 0;
  }

  int USB_read(char *buffer, int size)
  {
    return 0;
  }

  void USB_waitForHostToConnect()
  {
    
  }
}