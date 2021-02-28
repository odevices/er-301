local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Pitch = require "Unit.ViewControl.Pitch"
local Encoder = require "Encoder"

local GrainDelayUnit = Class {}
GrainDelayUnit:include(Unit)

function GrainDelayUnit:init(args)
  args.title = "Grain Delay"
  args.mnemonic = "GD"
  Unit.init(self, args)
end

function GrainDelayUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function GrainDelayUnit:loadMonoGraph()
  local grainL = self:addObject("grainL", libcore.MonoGrainDelay(1.0))

  local xfade = self:addObject("xfade", app.CrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())
  local multiply = self:addObject("multiply", app.Multiply())
  local pitch = self:addObject("pitch", libcore.VoltPerOctave())
  local tune = self:addObject("tune", app.ConstantOffset())
  local tuneRange = self:addObject("tuneRange", app.MinMax())
  local speed = self:addObject("speed", app.GainBias())
  speed:hardSet("Bias", 1.0)
  local speedRange = self:addObject("speedRange", app.MinMax())
  local speedClipper =
      self:addObject("speedClipper", libcore.Clipper(-100, 100))
  local delayL = self:addObject("delayL", app.GainBias())
  local delayLRange = self:addObject("delayLRange", app.MinMax())
  local delayLClipper = self:addObject("delayLClipper", libcore.Clipper(0, 1))

  connect(grainL, "Out", xfade, "A")
  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  connect(self, "In1", xfade, "B")
  connect(self, "In1", grainL, "In")
  connect(xfade, "Out", self, "Out1")

  connect(tune, "Out", pitch, "In")
  connect(tune, "Out", tuneRange, "In")
  connect(pitch, "Out", multiply, "Left")
  connect(speed, "Out", multiply, "Right")
  connect(multiply, "Out", speedClipper, "In")
  connect(speed, "Out", speedRange, "In")
  connect(speedClipper, "Out", grainL, "Speed")

  connect(delayL, "Out", delayLClipper, "In")
  connect(delayL, "Out", delayLRange, "In")
  connect(delayLClipper, "Out", grainL, "Delay")

  self:addMonoBranch("delay", delayL, "In", delayL, "Out")
  self:addMonoBranch("speed", speed, "In", speed, "Out")
  self:addMonoBranch("tune", tune, "In", tune, "Out")
  self:addMonoBranch("wet", fader, "In", fader, "Out")
end

function GrainDelayUnit:loadStereoGraph()
  local grainL = self:addObject("grainL", libcore.MonoGrainDelay(1.0))
  local grainR = self:addObject("grainR", libcore.MonoGrainDelay(1.0))

  local xfade = self:addObject("xfade", app.StereoCrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())
  local multiply = self:addObject("multiply", app.Multiply())
  local pitch = self:addObject("pitch", libcore.VoltPerOctave())
  local tune = self:addObject("tune", app.ConstantOffset())
  local tuneRange = self:addObject("tuneRange", app.MinMax())
  local speed = self:addObject("speed", app.GainBias())
  speed:hardSet("Bias", 1.0)
  local speedRange = self:addObject("speedRange", app.MinMax())
  local speedClipper =
      self:addObject("speedClipper", libcore.Clipper(-100, 100))
  local delayL = self:addObject("delayL", app.GainBias())
  local delayLRange = self:addObject("delayLRange", app.MinMax())
  local delayLClipper = self:addObject("delayLClipper", libcore.Clipper(0, 1))
  local delayR = self:addObject("delayR", app.GainBias())
  local delayRRange = self:addObject("delayRRange", app.MinMax())
  local delayRClipper = self:addObject("delayRClipper", libcore.Clipper(0, 1))

  connect(grainL, "Out", xfade, "Left A")
  connect(grainR, "Out", xfade, "Right A")
  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  connect(self, "In1", xfade, "Left B")
  connect(self, "In1", grainL, "In")

  connect(self, "In2", xfade, "Right B")
  connect(self, "In2", grainR, "In")

  connect(xfade, "Left Out", self, "Out1")
  connect(xfade, "Right Out", self, "Out2")

  connect(tune, "Out", pitch, "In")
  connect(tune, "Out", tuneRange, "In")
  connect(pitch, "Out", multiply, "Left")
  connect(speed, "Out", multiply, "Right")
  connect(multiply, "Out", speedClipper, "In")
  connect(speed, "Out", speedRange, "In")
  connect(speedClipper, "Out", grainL, "Speed")
  connect(speedClipper, "Out", grainR, "Speed")

  connect(delayL, "Out", delayLClipper, "In")
  connect(delayL, "Out", delayLRange, "In")
  connect(delayLClipper, "Out", grainL, "Delay")

  connect(delayR, "Out", delayRClipper, "In")
  connect(delayR, "Out", delayRRange, "In")
  connect(delayRClipper, "Out", grainR, "Delay")

  self:addMonoBranch("delayL", delayL, "In", delayL, "Out")
  self:addMonoBranch("delayR", delayR, "In", delayR, "Out")
  self:addMonoBranch("speed", speed, "In", speed, "Out")
  self:addMonoBranch("tune", tune, "In", tune, "Out")
  self:addMonoBranch("wet", fader, "In", fader, "Out")
end

function GrainDelayUnit:onLoadViews(objects, branches)
  local controls = {}
  local views = {
    collapsed = {}
  }

  if self.channelCount == 2 then
    views.expanded = {
      "leftDelay",
      "rightDelay",
      "pitch",
      "speed",
      "wet"
    }

    controls.leftDelay = GainBias {
      button = "delay(L)",
      branch = branches.delayL,
      description = "Left Delay",
      gainbias = objects.delayL,
      range = objects.delayLRange,
      biasMap = Encoder.getMap("unit"),
      biasUnits = app.unitSecs
    }

    controls.rightDelay = GainBias {
      button = "delay(R)",
      branch = branches.delayR,
      description = "Right Delay",
      gainbias = objects.delayR,
      range = objects.delayRRange,
      biasMap = Encoder.getMap("unit"),
      biasUnits = app.unitSecs
    }

  else
    views.expanded = {
      "delay",
      "pitch",
      "speed",
      "wet"
    }

    controls.delay = GainBias {
      button = "delay",
      branch = branches.delay,
      description = "Delay",
      gainbias = objects.delayL,
      range = objects.delayLRange,
      biasMap = Encoder.getMap("unit"),
      biasUnits = app.unitSecs
    }
  end

  controls.pitch = Pitch {
    button = "V/oct",
    description = "V/oct",
    branch = branches.tune,
    offset = objects.tune,
    range = objects.tuneRange
  }

  controls.speed = GainBias {
    button = "speed",
    branch = branches.speed,
    description = "Speed",
    gainbias = objects.speed,
    range = objects.speedRange,
    biasMap = Encoder.getMap("speed"),
    biasUnits = app.unitNone
  }

  controls.wet = GainBias {
    button = "wet",
    branch = branches.wet,
    description = "Wet/Dry",
    gainbias = objects.fader,
    range = objects.faderRange,
    biasMap = Encoder.getMap("unit")
  }

  return controls, views
end

function GrainDelayUnit:deserialize(t)
  Unit.deserialize(self, t)
  -- v0.2.21: Changed Wet/Dry from ConstantOffset to GainBias
  local pData = t.objects.fader and t.objects.fader.params
  if pData and pData.Offset then
    local param = self.objects.fader and self.objects.fader:getParameter("Bias")
    if param then
      app.logInfo(
          "GrainDelayUnit:deserialize: porting legacy parameter, fader.Offset.")
      param:deserialize(pData.Offset)
    end
  end
end

return GrainDelayUnit
