local Settings = require "Settings"

local function onUSBModeChanged()
  local Settings = require "Settings"
  local mode = Settings.get("usbMode")
  if mode == "card" then
    app.USB_setMassStorageMode()
    app.logInfo("USB switched to Mass Storage mode.")
  elseif mode == "serial" then
    app.USB_setSerialPortMode()
    app.logInfo("USB switched to Serial Port mode.")
  end
end

local function onUSBEnableChanged()
  local enabled = Settings.get("usbEnable") == "yes"
  local mode = Settings.get("usbMode")
  if enabled then
    app.USB_start()
    if mode == "serial" then
      app.enableLogging()
      app.USB_waitForHostToConnect()
    end
    app.logInfo("USB started.")
  else
    app.logInfo("USB stopped.")
    app.USB_stop()
    app.disableLogging()
  end
end

local function init()
  local Signal = require "Signal"
  Signal.register("onUSBModeChanged", onUSBModeChanged)
  Signal.register("onUSBEnableChanged", onUSBEnableChanged)
  onUSBModeChanged()
  onUSBEnableChanged()
end

return {
  init = init
}
