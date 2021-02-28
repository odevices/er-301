local app = app
app.logInfo("Entering start.lua...")
package.path = app.roots.x .. "/?.lua;" .. app.roots.x .. "/?/init.lua"

app.logInfo("X Root:"..app.roots.x)
app.logInfo("Front Root:"..app.roots.front)
app.logInfo("Rear Root:"..app.roots.rear)
app.logInfo("Package Path: "..package.path)

local function onError(msg)
  local trace = debug.traceback(nil, 2)
  collectgarbage("stop")
  app.logInfo("start: stopping audio")
  app.Audio_stop()

  print(msg)
  print(trace)
  app.Events_clear()

  if app.onErrorHook then
    app.logInfo("start: running onErrorHook")
    local hook = app.onErrorHook
    -- turn it off, in case onError gets called inside the hook
    app.onErrorHook = nil
    xpcall(function()
      hook(msg, trace)
    end, onError)
  end

  -- emergency event loop
  app.logInfo("start: entering EMERGENCY event loop.")
  while true do
    app.Events_wait()
    while true do
      local event = app.Events_pull()
      if event == app.EVENT_NONE then
        break
      elseif event == app.EVENT_QUIT then
        return
      elseif event == app.EVENT_DISPLAY_READY then
        app.UIThread.updateDisplay()
      end
    end
  end
end

local function start()
  dofile(app.roots.x.."/boot/globals-setup.lua")
  dofile(app.roots.x.."/boot/app-setup.lua")
  --local Debug = require "Debug"
  --Debug.traceRequires()
  app.logInfo("Entering Application")
  local Application = require "Application"
  Application.init()
  Application.loop()
end

-- Enhance the string class
if not string.quote then
  local quotepattern = '([' .. ("%^$().[]*+-?"):gsub("(.)", "%%%1") .. '])'
  string.quote = function(str)
    return str:gsub(quotepattern, "%%%1")
  end
end

local status, err = xpcall(start, onError)
if not status then
  app.logError(err)
end
app.logInfo("Exiting start.lua...")
