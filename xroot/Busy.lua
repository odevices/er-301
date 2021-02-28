local Overlay = require "Overlay"

local busyThread = app.Busy()
busyThread:start()

local log = false
local refCount = 0

local function start(...)
  local text
  if ... == nil then
    text = "Working..."
  else
    text = string.format(...)
  end
  if log then
    app.logInfo("Busy(%d):start: %s", refCount, text)
    -- local trace = debug.traceback(nil,2)
    -- print(trace)
  end
  Overlay.startMainMessage(text)

  refCount = refCount + 1
  if refCount == 1 then busyThread:enable() end
end

local function status(...)
  if log then app.logInfo("Busy(%d):status: %s", refCount, string.format(...)) end

  if refCount > 0 then Overlay.startMainMessage(string.format(...)) end
end

local function stop(force)
  if force then refCount = 1 end
  refCount = refCount - 1
  if refCount == 0 then
    busyThread:disable()
    Overlay.endMainMessage()
  elseif refCount < 0 then
    refCount = 0
  end

  if log then app.logInfo("Busy(%d):stop", refCount) end

end

local function enable()
  busyThread:enable()
end

local function disable()
  busyThread:disable()
end

return {
  start = start,
  stop = stop,
  -- start=function() end,
  -- stop=function() end,
  status = status,
  enable = enable,
  disable = disable
}
