local Card = require "Card"

local function saveCrashReport(msg, trace)
  local reportSaved = false
  if Card.mounted() then
    local f = io.open(app.roots.front .. "/crash.log", "a+")
    if f then
      local Persist = require "Persist"
      f:write("---CRASH REPORT BEGIN\n")
      f:write(string.format("Time Since Boot: %0.3fs\n", app.wallclock()))
      f:write(string.format("Firmware Version: %s\n",
                            Persist.meta.boot.firmwareVersion))
      f:write(string.format("Boot Count: %d\n", Persist.meta.boot.count))
      f:write(string.format("Mount Count: %d\n", Persist.meta.mount.count))
      f:write("Error Message:\n")
      f:write(msg .. "\n")
      f:write(trace .. "\n")
      f:write("Recent Log Messages:\n")
      local LogHistory = require "LogHistory"
      local count = LogHistory:count()
      for i = 1, count do f:write(LogHistory:get(i), "\n") end
      f:write("---CRASH REPORT END\n")
      f:close()
      print("Crash report appended to 'crash.log'.")
      reportSaved = true
    else
      print("Failed to write 'crash.log'.")
    end
  end
  return reportSaved
end

local function showDialog(reportSaved)
  local app = app
  local Message = require "Message"
  local Overlay = require "Overlay"
  local Busy = require "Busy"
  local dialog = Message.Sub("Reboot required.", "", true)

  if reportSaved then
    local label = app.Label("Oops! Something went wrong.", 12)
    label:setCenter(128, app.GRID4_LINE1)
    dialog:addMainGraphic(label)
    label = app.Label("Report saved to crash.log. Please send the report to:",
                      10)
    label:setCenter(128, app.GRID4_LINE2)
    dialog:addMainGraphic(label)
    label = app.Label("clarkson@orthogonaldevices.com", 10)
    label:setCenter(128, app.GRID4_LINE3)
    dialog:addMainGraphic(label)
  else
    local label = app.Label("Oops! Something went wrong.", 12)
    label:setCenter(128, app.GRID4_LINE1)
    dialog:addMainGraphic(label)
    label = app.Label("No card present to save the crash report.", 10)
    label:setCenter(128, app.GRID4_LINE2)
    dialog:addMainGraphic(label)
  end

  Busy.stop(true)
  Overlay.clearAll()
  local Context = require "Base.Context"
  local context = Context("Crash", dialog)
  context:show()

  app.logInfo("start: entering CRASH event loop.")
  Card.forceEject()
  while true do
    app.Events_wait()
    while true do
      local event = app.Events_pull()
      if event == app.EVENT_NONE then
        break
      elseif event == app.EVENT_DISPLAY_READY then
        app.UIThread.updateDisplay()
      elseif event == app.EVENT_RELEASE_SUB1 then
        app.reboot()
      elseif event == app.EVENT_RELEASE_SUB2 then
        app.reboot()
      elseif event == app.EVENT_RELEASE_SUB3 then
        app.reboot()
      elseif event == app.EVENT_RELEASE_ENTER then
        app.reboot()
      end
    end
  end
  return true
end

local function onError(msg, trace)
  local reportSaved = saveCrashReport(msg, trace)
  showDialog(reportSaved)
end

local function init()
  app.onErrorHook = onError
end

return {
  init = init
}
