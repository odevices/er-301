local Tests = require "Tests"

local function runLoadAll()
  local UnitFactory = require "Unit.Factory"
  local Channels = require "Channels"
  local chain = Channels.getChain(1)
  local custom = chain:loadUnit(UnitFactory.getBuiltin("custom.CustomUnit"))

  chain:stop()
  local units = UnitFactory.getUnitsByLibrary("builtins")
  for _, loadInfo in ipairs(units) do
    Tests.printSystemState()
    app.logInfo("Loading %s", loadInfo.title)
    custom.patch:loadUnit(loadInfo)
  end
  chain:start()
end

local function runLoadChainPreset()
  local UnitFactory = require "Unit.Factory"
  local Channels = require "Channels"
  local chain = Channels.getChain(1)
  local custom = chain:loadUnit(UnitFactory.getBuiltin("custom.CustomUnit"))
  Tests.printSystemState()
  -- custom.patch:loadPreset("1:/ER-301/sc/presets/chains/abc.lua")
end

return {
  {
    description = "Load all into custom unit",
    run = runLoadAll,
    batch = true
  },
  {
    description = "Load chain preset into custom unit",
    run = runLoadChainPreset,
    batch = true
  }
}
