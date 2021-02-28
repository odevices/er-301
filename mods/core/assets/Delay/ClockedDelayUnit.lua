local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Gate = require "Unit.ViewControl.Gate"
local GainBias = require "Unit.ViewControl.GainBias"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"
local Encoder = require "Encoder"
local Utils = require "Utils"

local ClockedDelayUnit = Class {}
ClockedDelayUnit:include(Unit)

function ClockedDelayUnit:init(args)
  args.title = "Clocked Delay"
  args.mnemonic = "CD"
  Unit.init(self, args)
end

function ClockedDelayUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function ClockedDelayUnit:loadMonoGraph()
  local delay = self:addObject("delay", libcore.Delay(1))
  local xfade = self:addObject("xfade", app.CrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  local feedback = self:addObject("feedback", app.GainBias())
  local feedbackRange = self:addObject("feedbackRange", app.MinMax())
  local snap = self:addObject("snap", libcore.SnapToZero())
  snap:setThresholdInDecibels(-35.9)

  local tap = self:addObject("tap", libcore.TapTempo())
  tap:setBaseTempo(120)
  local tapEdge = self:addObject("tapEdge", app.Comparator())

  local multiplier = self:addObject("multiplier", app.ParameterAdapter())
  local divider = self:addObject("divider", app.ParameterAdapter())
  tie(tap, "Multiplier", multiplier, "Out")
  tie(tap, "Divider", divider, "Out")

  local nudge = self:addObject("nudge", app.ParameterAdapter())
  tie(delay, "Left Delay", tap, "Derived Period")
  tie(delay, "Spread", nudge, "Out")

  connect(tapEdge, "Out", tap, "In")

  connect(delay, "Left Out", xfade, "A")
  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  connect(snap, "Out", feedback, "In")
  connect(feedback, "Out", delay, "Feedback")
  connect(feedback, "Out", feedbackRange, "In")

  connect(self, "In1", xfade, "B")
  connect(self, "In1", delay, "Left In")
  connect(xfade, "Out", self, "Out1")

  self:addMonoBranch("clock", tapEdge, "In", tapEdge, "Out")
  self:addMonoBranch("wet", fader, "In", fader, "Out")
  self:addMonoBranch("multiplier", multiplier, "In", multiplier, "Out")
  self:addMonoBranch("divider", divider, "In", divider, "Out")
  self:addMonoBranch("nudge", nudge, "In", nudge, "Out")
  self:addMonoBranch("feedback", snap, "In", snap, "Out")
end

function ClockedDelayUnit:loadStereoGraph()
  local delay = self:addObject("delay", libcore.Delay(2))
  local xfade = self:addObject("xfade", app.StereoCrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  local feedback = self:addObject("feedback", app.GainBias())
  local feedbackRange = self:addObject("feedbackRange", app.MinMax())
  local snap = self:addObject("snap", libcore.SnapToZero())
  snap:setThresholdInDecibels(-35.9)

  local tap = self:addObject("tap", libcore.TapTempo())
  tap:setBaseTempo(120)
  local tapEdge = self:addObject("tapEdge", app.Comparator())

  local multiplier = self:addObject("multiplier", app.ParameterAdapter())
  local divider = self:addObject("divider", app.ParameterAdapter())
  tie(tap, "Multiplier", multiplier, "Out")
  tie(tap, "Divider", divider, "Out")

  local spread = self:addObject("spread", app.ParameterAdapter())
  tie(delay, "Left Delay", tap, "Derived Period")
  tie(delay, "Right Delay", tap, "Derived Period")
  tie(delay, "Spread", spread, "Out")

  connect(tapEdge, "Out", tap, "In")

  connect(delay, "Left Out", xfade, "Left A")
  connect(delay, "Right Out", xfade, "Right A")
  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  connect(snap, "Out", feedback, "In")
  connect(feedback, "Out", delay, "Feedback")
  connect(feedback, "Out", feedbackRange, "In")

  connect(self, "In1", xfade, "Left B")
  connect(self, "In1", delay, "Left In")

  connect(self, "In2", xfade, "Right B")
  connect(self, "In2", delay, "Right In")

  connect(xfade, "Left Out", self, "Out1")
  connect(xfade, "Right Out", self, "Out2")

  self:addMonoBranch("clock", tapEdge, "In", tapEdge, "Out")
  self:addMonoBranch("wet", fader, "In", fader, "Out")
  self:addMonoBranch("multiplier", multiplier, "In", multiplier, "Out")
  self:addMonoBranch("divider", divider, "In", divider, "Out")
  self:addMonoBranch("spread", spread, "In", spread, "Out")
  self:addMonoBranch("feedback", snap, "In", snap, "Out")
end

function ClockedDelayUnit:setMaxDelayTime(secs)
  local requested = math.floor(secs + 0.5)
  self.objects.delay:allocateTimeUpTo(requested)
end

local menu = {
  "setHeader",
  "set100ms",
  "set1s",
  "set10s",
  "set30s"
}

function ClockedDelayUnit:onShowMenu(objects, branches)
  local controls = {}
  local allocated = self.objects.delay:maximumDelayTime()
  allocated = Utils.round(allocated, 1)

  controls.setHeader = MenuHeader {
    description = string.format("Current Maximum Delay is %0.1fs.", allocated)
  }

  controls.set100ms = Task {
    description = "0.1s",
    task = function()
      self:setMaxDelayTime(0.1)
    end
  }

  controls.set1s = Task {
    description = "1s",
    task = function()
      self:setMaxDelayTime(1)
    end
  }

  controls.set10s = Task {
    description = "10s",
    task = function()
      self:setMaxDelayTime(10)
    end
  }

  controls.set30s = Task {
    description = "30s",
    task = function()
      self:setMaxDelayTime(30)
    end
  }

  return controls, menu
end

function ClockedDelayUnit:onLoadViews(objects, branches)
  local controls = {}
  local views = {
    collapsed = {}
  }

  controls.clock = Gate {
    button = "clock",
    branch = branches.clock,
    description = "Clock",
    comparator = objects.tapEdge
  }

  -- other spots
  if self.channelCount == 2 then

    views.expanded = {
      "clock",
      "mult",
      "div",
      "spread",
      "feedback",
      "wet"
    }

    controls.spread = GainBias {
      button = "spread",
      branch = branches.spread,
      description = "Stereo Spread",
      gainbias = objects.spread,
      range = objects.spread,
      biasMap = Encoder.getMap("[-1,1]")
    }

  else
    views.expanded = {
      "clock",
      "mult",
      "div",
      "nudge",
      "feedback",
      "wet"
    }

    controls.nudge = GainBias {
      button = "nudge",
      branch = branches.nudge,
      description = "Nudge",
      gainbias = objects.nudge,
      range = objects.nudge,
      biasMap = Encoder.getMap("[-1,1]")
    }

  end

  controls.mult = GainBias {
    button = "mult",
    branch = branches.multiplier,
    description = "Clock Multiplier",
    gainbias = objects.multiplier,
    range = objects.multiplier,
    biasMap = Encoder.getMap("int[1,32]"),
    gainMap = Encoder.getMap("[-20,20]"),
    initialBias = 1,
    biasPrecision = 0
  }

  controls.div = GainBias {
    button = "div",
    branch = branches.divider,
    description = "Clock Divider",
    gainbias = objects.divider,
    range = objects.divider,
    biasMap = Encoder.getMap("int[1,32]"),
    gainMap = Encoder.getMap("[-20,20]"),
    initialBias = 1,
    biasPrecision = 0
  }

  controls.feedback = GainBias {
    button = "fdbk",
    description = "Feedback",
    branch = branches.feedback,
    gainbias = objects.feedback,
    range = objects.feedbackRange,
    biasMap = Encoder.getMap("feedback"),
    biasUnits = app.unitDecibels
  }
  controls.feedback:setTextBelow(-35.9, "-inf dB")

  controls.wet = GainBias {
    button = "wet",
    branch = branches.wet,
    description = "Wet/Dry",
    gainbias = objects.fader,
    range = objects.faderRange,
    biasMap = Encoder.getMap("unit")
  }

  self:setMaxDelayTime(1.0)

  return controls, views
end

function ClockedDelayUnit:deserializeLegacyPreset(t)
  -- v0.3.04: Changed Feedback from Constant to GainBias
  local pData = t.objects.feedback and t.objects.feedback.params
  if pData and pData.Value then
    local param = self.objects.feedback and
                      self.objects.feedback:getParameter("Bias")
    if param then
      app.logInfo(
          "ClockedDelayUnit:deserialize: porting legacy parameter, feedback.Value.")
      param:deserialize(pData.Value)
    end
  end
end

function ClockedDelayUnit:serialize()
  local t = Unit.serialize(self)
  t.maximumDelayTime = self.objects.delay:maximumDelayTime()
  return t
end

function ClockedDelayUnit:deserialize(t)
  local time = t.maximumDelayTime
  if time and time > 0 then self:setMaxDelayTime(time) end
  Unit.deserialize(self, t)
  if self:getPresetVersion(t) < 1 then self:deserializeLegacyPreset(t) end
end

function ClockedDelayUnit:onRemove()
  self.objects.delay:deallocate()
  Unit.onRemove(self)
end

return ClockedDelayUnit
