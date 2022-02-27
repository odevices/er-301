local Tests = require "Tests"

local function runMono()
  local Channels = require "Channels"
  Channels.unlink(1)
  local chain = Channels.getChain(1)
  local UnitFactory = require "Unit.Factory"
  local units = UnitFactory.getUnitsByLibrary("core")
  chain:stop()
  Tests.printSystemState()
  for _, loadInfo in ipairs(units) do
    if loadInfo.channel == nil or loadInfo.channelCount == 1 then
      app.logInfo("Loading %s", loadInfo.title)
      local x = chain:loadUnit(loadInfo)
      local data = x:serialize()
      x:deserialize(data)
      chain:removeUnit(x)
      app.collectgarbage()
      Tests.printSystemState()
    end
  end
  chain:start()
end

local function runStereo()
  local Channels = require "Channels"
  Channels.link(1)
  local chain = Channels.getChain(1)
  local UnitFactory = require "Unit.Factory"
  local units = UnitFactory.getUnitsByLibrary("core")
  chain:stop()
  Tests.printSystemState()
  for _, loadInfo in ipairs(units) do
    if loadInfo.channel == nil or loadInfo.channelCount == 2 then
      app.logInfo("Loading %s", loadInfo.title)
      local x = chain:loadUnit(loadInfo)
      local data = x:serialize()
      x:deserialize(data)
      chain:removeUnit(x)
      app.collectgarbage()
      Tests.printSystemState()
    end
  end
  chain:start()
end

return {
  {
    description = "Load core units as mono.",
    batch = true,
    run = runMono
  }, {
    description = "Load core units as stereo.",
    batch = true,
    run = runStereo
  }
}
