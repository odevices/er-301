local app = app
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local LinearBipolar = Class {}
LinearBipolar:include(Unit)

function LinearBipolar:init(args)
  args.title = "Linear Bipolar VCA"
  args.mnemonic = "VCA"
  Unit.init(self, args)
end

function LinearBipolar:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function LinearBipolar:loadMonoGraph()
  local vca = self:addObject("vca", app.Multiply())
  local level = self:addObject("level", app.GainBias())
  local levelRange = self:addObject("levelRange", app.MinMax())

  connect(level, "Out", levelRange, "In")
  connect(level, "Out", vca, "Left")
  connect(self, "In1", vca, "Right")
  connect(vca, "Out", self, "Out1")

  self:addMonoBranch("level", level, "In", level, "Out")
end

function LinearBipolar:loadStereoGraph()
  local vca1 = self:addObject("vca1", app.Multiply())
  local vca2 = self:addObject("vca2", app.Multiply())
  local level = self:addObject("level", app.GainBias())
  local levelRange = self:addObject("levelRange", app.MinMax())

  local balance = self:addObject("balance", app.StereoPanner())
  local pan = self:addObject("pan", app.GainBias())
  local panRange = self:addObject("panRange", app.MinMax())

  connect(level, "Out", levelRange, "In")
  connect(level, "Out", vca1, "Left")
  connect(level, "Out", vca2, "Left")

  connect(self, "In1", vca1, "Right")
  connect(self, "In2", vca2, "Right")

  connect(vca1, "Out", balance, "Left In")
  connect(balance, "Left Out", self, "Out1")
  connect(vca2, "Out", balance, "Right In")
  connect(balance, "Right Out", self, "Out2")

  connect(pan, "Out", balance, "Pan")
  connect(pan, "Out", panRange, "In")

  self:addMonoBranch("level", level, "In", level, "Out")
  self:addMonoBranch("pan", pan, "In", pan, "Out")
end

function LinearBipolar:onLoadViews(objects, branches)
  local views = {
    expanded = {
      "level"
    },
    collapsed = {}
  }

  local controls = {}

  controls.level = GainBias {
    button = "level",
    branch = branches.level,
    description = "Level",
    gainbias = objects.level,
    range = objects.levelRange,
    biasMap = Encoder.getMap("[-5,5]"),
    biasUnits = app.unitNone,
    initialBias = 0.0,
    gainMap = Encoder.getMap("gain")
  }

  if objects.pan then
    controls.pan = GainBias {
      button = "pan",
      branch = branches.pan,
      description = "Pan",
      gainbias = objects.pan,
      range = objects.panRange,
      biasMap = Encoder.getMap("default"),
      biasUnits = app.unitNone
    }

    views.expanded[2] = "pan"
  end

  return controls, views
end

return LinearBipolar
