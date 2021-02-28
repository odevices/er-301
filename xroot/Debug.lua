local Signal = require "Signal"
local callFilter = {}

local function hook(why)
  local info = debug.getinfo(2, "Sn")
  if callFilter[info.name] then return end
  print(info.name, info.source, info.linedefined)
end -- hook

local function traceCalls(enable)
  if enable then
    debug.sethook(hook, "c", 0)
  else
    debug.sethook(nil, "c", 0)
  end
end

local function filterCall(call)
  callFilter[call] = true
end

local function enableRequireHook()
  -- GLOBALS: oldRequire, package
  -- local debug = require "debug"
  local oldRequire = require
  local depth = 0
  function require(modname)
    local module = package.loaded[modname]
    if module then return module end
    app.logInfo("%sstart require %s", string.rep("-", depth), modname)
    if depth > 15 then
      app.logError("Exceeded require recursion depth (require %s).", modname)
    end
    -- print(debug.traceback())
    depth = depth + 1
    module = oldRequire(modname)
    depth = depth - 1
    app.logInfo("%send require %s", string.rep("-", depth), modname)
    return module
  end
end

local function startProfile(shifted)
  if shifted then 
    app.logInfo("Debug: start profiling")
    app.Profiler.start() 
  end
end

local function stopProfile(shifted)
  if shifted then
    app.logInfo("Debug: stop profiling")
    app.Profiler.stop()
    app.Profiler.print()
    collectgarbage()
    app.Heap_print()
  end
end

local function cancelReleased(shifted)
  if shifted then error("Simulated Error.") end
end

local function printTraceback()
  local trace = debug.traceback(nil, 2)
  print(trace)
end

local function init()
  Signal.register("dialPressed", startProfile)
  Signal.register("dialReleased", stopProfile)
end

return {
  init = init,
  traceCalls = traceCalls,
  filterCall = filterCall,
  traceRequires = enableRequireHook,
  printTraceback = printTraceback
}
