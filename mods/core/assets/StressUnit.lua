local app = app
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local StressUnit = Class {}
StressUnit:include(Unit)

function StressUnit:init(args)
  args.title = "Stress"
  args.mnemonic = "St"
  Unit.init(self, args)
end

function StressUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function StressUnit:loadMonoGraph()
  local stress = self:addObject("stress", app.Stress())
  local map = self:addObject("map", app.ParameterAdapter())
  map:clamp(0, 1)

  connect(self, "In1", stress, "In")
  connect(stress, "Out", self, "Out1")
  tie(stress, "Load", map, "Out")

  self:addMonoBranch("load", map, "In", map, "Out")
end

function StressUnit:loadStereoGraph()
  local stress1 = self:addObject("stress1", app.Stress())
  local stress2 = self:addObject("stress2", app.Stress())
  local map = self:addObject("map", app.ParameterAdapter())
  map:clamp(0, 1)

  connect(self, "In1", stress1, "In")
  connect(self, "In2", stress2, "In")
  connect(stress1, "Out", self, "Out1")
  connect(stress2, "Out", self, "Out2")
  tie(stress1, "Load", map, "Out")
  tie(stress2, "Load", map, "Out")

  self.objects.stress = self.objects.stress1
  stress1:setAdjustment(0.5)
  stress2:setAdjustment(0.5)

  self:addMonoBranch("load", map, "In", map, "Out")
end

local views = {
  expanded = {
    "cpu"
  },
  collapsed = {}
}

function StressUnit:onLoadViews(objects, branches)
  local controls = {}
  controls.cpu = GainBias {
    button = "load",
    description = "CPU Load",
    branch = branches.load,
    gainbias = objects.map,
    range = objects.map,
    biasMap = Encoder.getMap("unit")
  }
  return controls, views
end

return StressUnit
