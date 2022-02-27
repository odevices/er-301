local Overlay = require "Overlay"
local LogHistory = require "LogHistory"
local busyThread = app.Busy()
busyThread:start()

local logStart = false
local logStatus = true
local logError = true
local refCount = 0

local function start(...)
  local text
  if ... == nil then
    text = "Busy..."
  else
    text = string.format(...)
    if logStart then
      app.logInfo(text)
    end
  end

  Overlay.startMainMessage(text)

  refCount = refCount + 1
  if refCount == 1 then
    busyThread:enable()
    LogHistory:clearErrors()
  end
end

local function status(...)
  if refCount > 0 then
    local msg = string.format(...)
    if logStatus then
      app.logInfo(msg)
    end
    Overlay.startMainMessage(msg)
  end
end

local function error(...)
  if refCount > 0 then
    local msg = string.format(...)
    if logError then
      app.logError(msg)
    end
    Overlay.startMainMessage(msg)
  end
end

local function stop()
  refCount = refCount - 1
  if refCount == 0 then
    busyThread:disable()
    Overlay.endMainMessage()
  elseif refCount < 0 then
    refCount = 0
  end
end

local function reset()
  refCount = 1
  stop()
end

local function enable()
  busyThread:enable()
end

local function disable()
  busyThread:disable()
end

local function kill()
  if busyThread:running() then
    busyThread:stop()
    busyThread:join()
  end
end

return {
  start = start,
  stop = stop,
  reset = reset,
  kill = kill,
  status = status,
  error = error,
  enable = enable,
  disable = disable
}
