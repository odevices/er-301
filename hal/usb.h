#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  void USB_init();
  void USB_start();
  void USB_stop();
  void USB_setMassStorageMode();
  void USB_setSerialPortMode();
  int USB_write(const char *buffer, int size);
  int USB_read(char *buffer, int size);
  void USB_waitForHostToConnect();

#ifdef __cplusplus
}
#endif