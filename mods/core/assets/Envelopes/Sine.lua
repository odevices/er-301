local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local InputGate = require "Unit.ViewControl.InputGate"
local Encoder = require "Encoder"

local Sine = Class {}
Sine:include(Unit)

function Sine:init(args)
  args.title = "Skewed Sine Env"
  args.mnemonic = "En"
  Unit.init(self, args)
end

function Sine:onLoadGraph(channelCount)
  local trig = self:addObject("trig", app.Comparator())
  trig:setTriggerMode()
  local env = self:addObject("env", libcore.SkewedSineEnvelope())
  local level = self:addObject("level", app.GainBias())
  local skew = self:addObject("skew", app.ParameterAdapter())
  local duration = self:addObject("duration", app.ParameterAdapter())
  local levelRange = self:addObject("levelRange", app.MinMax())

  connect(self, "In1", trig, "In")
  connect(trig, "Out", env, "Trigger")
  connect(env, "Out", self, "Out1")

  connect(level, "Out", env, "Level")
  connect(level, "Out", levelRange, "In")
  tie(env, "Duration", duration, "Out")
  tie(env, "Skew", skew, "Out")

  self:addMonoBranch("level", level, "In", level, "Out")
  self:addMonoBranch("skew", skew, "In", skew, "Out")
  self:addMonoBranch("duration", duration, "In", duration, "Out")

  if channelCount > 1 then
    connect(env, "Out", self, "Out2")
  end
end

local views = {
  expanded = {
    "input",
    "duration",
    "skew",
    "level"
  },
  collapsed = {}
}

function Sine:onLoadViews(objects, branches)
  local controls = {}

  controls.input = InputGate {
    button = "input",
    description = "Unit Input",
    comparator = objects.trig
  }

  controls.duration = GainBias {
    button = "dur",
    description = "Duration",
    branch = branches.duration,
    gainbias = objects.duration,
    range = objects.duration,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitSecs,
    initialBias = 0.1
  }

  controls.skew = GainBias {
    button = "skew",
    description = "Skew",
    branch = branches.skew,
    gainbias = objects.skew,
    range = objects.skew,
    biasMap = Encoder.getMap("default"),
    biasUnits = app.unitNone,
    initialBias = -0.5
  }

  controls.level = GainBias {
    button = "level",
    branch = branches.level,
    description = "Level",
    gainbias = objects.level,
    range = objects.levelRange,
    biasMap = Encoder.getMap("default"),
    biasUnits = app.unitNone,
    initialBias = 1.0
  }

  return controls, views
end

return Sine
