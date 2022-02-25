function app.enableLogging()
  local app = app
  local Signal = require "Signal"

  if not app.loggingEnabled then
    app.loggingEnabled = true

    local logCount = app.logCount
    local wallclock = app.wallclock
    local infoLabel = "\27[32mINFO\27[0m"
    local warnLabel = "\27[33mWARN\27[0m"
    local errorLabel = "\27[31mERROR\27[0m"
    local fatalLabel = "\27[31mFATAL\27[0m"

    local function logPrefix(label)
      return string.format("\27[34m[%d %0.4fs lua]\27[0m %s", logCount(),
                           wallclock(), label)
    end

    function app.logInfo(...)
      local text = string.format(...)
      print(string.format("%s %s", logPrefix(infoLabel), text))
      Signal.emit("logInfo", text)
    end

    function app.logWarn(...)
      local text = string.format(...)
      print(string.format("%s %s", logPrefix(warnLabel), text))
      Signal.emit("logWarn", text)
    end

    function app.logError(...)
      local text = string.format(...)
      print(string.format("%s %s", logPrefix(errorLabel), text))
      Signal.emit("logError", text)
    end

    function app.logFatal(...)
      local text = string.format(...)
      error(string.format("%s %s", logPrefix(fatalLabel), text), 2)
      Signal.emit("logFatal", text)
    end

    -- support legacy logging
    app.log = app.logInfo
  end
end

app.enableLogging()

function app.disableLogging()
  -- do nothing
end

if app.TESTING then

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

  function app.enableDevMode()
    -- do nothing
  end

  function app.disableDevMode()
    -- do nothing
  end

end

