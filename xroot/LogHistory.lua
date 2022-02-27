local LogViewer = require "LogViewer"
local Signal = require "Signal"

-- Set up a log history and have it listen to all logs.
local history = LogViewer("Log History", 256)
local errors = 0

local function onInfo(text)
  history:add(text)
end

local function onWarn(text)
  history:add("WARN " .. text)
end

local function onError(text)
  history:add("ERROR " .. text)
  errors = errors + 1
end

Signal.register("logInfo", onInfo)
Signal.register("logWarn", onWarn)
Signal.register("logError", onError)

function history:hasErrors()
  return errors > 0
end

function history:clearErrors()
  errors = 0
end

function history:showErrors(...)
  local msg = string.format(...)
  local msg2
  if errors == 0 then
    msg2 = "View log history?"
  elseif errors == 1 then
    msg2 = "1 error. View log history?"
  else
    msg2 = string.format("%d errors.  View log history?", errors)
  end
  local Verification = require "Verification"
  local verify = Verification.Main(msg, msg2)
  local task = function(ans)
    if ans then
      local viewer = LogViewer("Log History")
      local n = history:count()
      for i = 1, n do
        viewer:add(history:get(i))
      end
      viewer:show()
    end
  end
  history:clearErrors()
  verify:subscribe("done", task)
  verify:show()
end

return history
