function app.enableLogging()
  local app = app

  if not app.loggingEnabled then
    app.loggingEnabled = true

    local logCount = app.logCount
    local wallclock = app.wallclock
    local infoLabel = "\27[32mINFO\27[0m"
    local warnLabel = "\27[33mWARN\27[0m"
    local errorLabel = "\27[31mERROR\27[0m"

    local function logPrefix(label)
      return string.format("\27[34m[#%d %0.4fs lua]\27[0m %s", logCount(),
                           wallclock(), label)
    end

    function app.logInfo(...)
      local text = string.format(...)
      print(string.format("%s %s", logPrefix(infoLabel), text))
      if app.logDebug then
        local Signal = require "Signal"
        Signal.emit("log", text)
      end
    end

    function app.logWarn(...)
      local text = string.format(...)
      print(string.format("%s %s", logPrefix(warnLabel), text))
      if app.logDebug then
        local Signal = require "Signal"
        Signal.emit("warn", text)
      end
    end

    function app.logError(...)
      local text = string.format(...)
      error(string.format("%s %s", logPrefix(errorLabel), text), 2)
      if app.logDebug then
        local Signal = require "Signal"
        Signal.emit("error", text)
      end
    end
  end
end

if app.TESTING then
  app.enableLogging()

  function app.disableLogging()
    -- do nothing
  end

  function app.enableDevMode()
    app.Uart_enable()
    app.logInfo("Enabling UART logging...")
    app.logInfo("Enabling Dev mode...")
    local Debug = require "Debug"
    local Tests = require "Tests"
    Debug.init()
    Tests.init()
  end

  function app.disableDevMode()
    app.logInfo("Disabling UART logging...")
    app.Uart_disable()
  end

else
  local f = function()
    -- do nothing
  end
  app.logInfo = f
  app.logWarn = f
  app.logError = f

  function app.disableLogging()
    if app.loggingEnabled then
      app.loggingEnabled = false
      app.logInfo = f
      app.logWarn = f
      app.logError = f
    end
  end

  function app.enableDevMode()
    -- do nothing
  end

  function app.disableDevMode()
    -- do nothing
  end

end

