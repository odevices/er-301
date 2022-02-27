local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Pitch = require "Unit.ViewControl.Pitch"
local GainBias = require "Unit.ViewControl.GainBias"
local Gate = require "Unit.ViewControl.Gate"
local Encoder = require "Encoder"

local Sine = Class {}
Sine:include(Unit)

function Sine:init(args)
  args.title = "Sine Osc"
  args.mnemonic = "Si"
  Unit.init(self, args)
end

function Sine:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function Sine:loadMonoGraph()
  local osc = self:addObject("osc", libcore.SineOscillator())
  local tune = self:addObject("tune", app.ConstantOffset())
  local tuneRange = self:addObject("tuneRange", app.MinMax())
  local f0 = self:addObject("f0", app.GainBias())
  local f0Range = self:addObject("f0Range", app.MinMax())
  local phase = self:addObject("phase", app.GainBias())
  local phaseRange = self:addObject("phaseRange", app.MinMax())
  local feedback = self:addObject("feedback", app.GainBias())
  local feedbackRange = self:addObject("feedbackRange", app.MinMax())
  local vca = self:addObject("vca", app.Multiply())
  local level = self:addObject("level", app.GainBias())
  local levelRange = self:addObject("levelRange", app.MinMax())

  local sync = self:addObject("sync", app.Comparator())
  sync:setTriggerMode()

  connect(sync, "Out", osc, "Sync")

  connect(tune, "Out", tuneRange, "In")
  connect(tune, "Out", osc, "V/Oct")

  connect(f0, "Out", osc, "Fundamental")
  connect(f0, "Out", f0Range, "In")

  connect(phase, "Out", osc, "Phase")
  connect(phase, "Out", phaseRange, "In")

  connect(feedback, "Out", osc, "Feedback")
  connect(feedback, "Out", feedbackRange, "In")

  connect(level, "Out", levelRange, "In")
  connect(level, "Out", vca, "Left")

  connect(osc, "Out", vca, "Right")
  connect(vca, "Out", self, "Out1")

  self:addMonoBranch("level", level, "In", level, "Out")
  self:addMonoBranch("tune", tune, "In", tune, "Out")
  self:addMonoBranch("sync", sync, "In", sync, "Out")
  self:addMonoBranch("f0", f0, "In", f0, "Out")
  self:addMonoBranch("phase", phase, "In", phase, "Out")
  self:addMonoBranch("feedback", feedback, "In", feedback, "Out")
end

function Sine:loadStereoGraph()
  self:loadMonoGraph()
  connect(self.objects.vca, "Out", self, "Out2")
end

local views = {
  expanded = {
    "tune",
    "freq",
    "phase",
    "feedback",
    "sync",
    "level"
  },
  collapsed = {}
}

function Sine:onLoadViews(objects, branches)
  local controls = {}

  controls.tune = Pitch {
    button = "V/oct",
    branch = branches.tune,
    description = "V/oct",
    offset = objects.tune,
    range = objects.tuneRange
  }

  controls.freq = GainBias {
    button = "f0",
    description = "Fundamental",
    branch = branches.f0,
    gainbias = objects.f0,
    range = objects.f0Range,
    biasMap = Encoder.getMap("oscFreq"),
    biasUnits = app.unitHertz,
    initialBias = 27.5,
    gainMap = Encoder.getMap("freqGain"),
    scaling = app.octaveScaling
  }

  controls.phase = GainBias {
    button = "phase",
    description = "Phase Offset",
    branch = branches.phase,
    gainbias = objects.phase,
    range = objects.phaseRange,
    biasMap = Encoder.getMap("[-1,1]"),
    initialBias = 0.0
  }

  controls.level = GainBias {
    button = "level",
    description = "Level",
    branch = branches.level,
    gainbias = objects.level,
    range = objects.levelRange,
    biasMap = Encoder.getMap("[-1,1]"),
    initialBias = 0.5
  }

  controls.sync = Gate {
    button = "sync",
    description = "Sync",
    branch = branches.sync,
    comparator = objects.sync
  }

  controls.feedback = GainBias {
    button = "fdbk",
    description = "Feedback",
    branch = branches.feedback,
    gainbias = objects.feedback,
    range = objects.feedbackRange,
    biasMap = Encoder.getMap("[-1,1]"),
    initialBias = 0
  }

  return controls, views
end

function Sine:deserialize(t)
  Unit.deserialize(self, t)
  if self:getPresetVersion(t) < 1 then
    -- handle legacy preset (<v0.2.12)
    local Serialization = require "Persist.Serialization"
    local f0 = Serialization.get("objects/osc/params/Fundamental", t)
    if f0 then
      app.logInfo("%s:deserialize:legacy preset detected:setting f0 bias to %s",
                  self, f0)
      self.objects.f0:deserialize("Bias", f0)
    end
  end
end

return Sine
