local Tests = require "Tests"

local function runMono()
  local Channels = require "Channels"
  Channels.unlink(1)
  local chain = Channels.getChain(1)
  local UnitFactory = require "Unit.Factory"
  local units = UnitFactory.getUnits(nil, 1)
  chain:stop()
  Tests.printSystemState()
  for _, loadInfo in ipairs(units) do
    app.logInfo("Loading %s", loadInfo.title)
    local x = chain:loadUnit(loadInfo)
    local data = x:serialize()
    x:deserialize(data)
    chain:removeUnit(x)
    app.collectgarbage()
    Tests.printSystemState()
  end
  chain:start()
end

local function runStereo()
  local Channels = require "Channels"
  Channels.link(1)
  local chain = Channels.getChain(1)
  local UnitFactory = require "Unit.Factory"
  local units = UnitFactory.getUnits(nil, 2)
  chain:stop()
  Tests.printSystemState()
  for _, loadInfo in ipairs(units) do
    app.logInfo("Loading %s", loadInfo.title)
    local x = chain:loadUnit(loadInfo)
    local data = x:serialize()
    x:deserialize(data)
    chain:removeUnit(x)
    app.collectgarbage()
    Tests.printSystemState()
  end
  chain:start()
end

return {
  {
    description = "Load all units as mono.",
    batch = true,
    run = runMono
  },
  {
    description = "Load all units as stereo.",
    batch = true,
    run = runStereo
  }
}
