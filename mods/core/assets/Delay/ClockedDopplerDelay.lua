local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local Gate = require "Unit.ViewControl.Gate"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

local ClockedDopplerDelay = Class {}
ClockedDopplerDelay:include(Unit)

function ClockedDopplerDelay:init(args)
  args.title = "Clocked Doppler Delay"
  args.mnemonic = "CD"
  Unit.init(self, args)
end

function ClockedDopplerDelay:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function ClockedDopplerDelay:loadMonoGraph()
  local delay = self:addObject("delay", libcore.DopplerDelay(2.0))
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

  local period = self:addObject("period", app.Constant())
  local multiplier = self:addObject("multiplier", app.ParameterAdapter())
  local divider = self:addObject("divider", app.ParameterAdapter())
  local split = self:addObject("split", libcore.Spread())

  local spread = self:addObject("spread", app.GainBias())
  local spreadRange = self:addObject("spreadRange", app.MinMax())

  tie(tap, "Multiplier", multiplier, "Out")
  tie(tap, "Divider", divider, "Out")
  tie(period, "Value", tap, "Derived Period")
  connect(period, "Out", split, "In")
  connect(split, "Left Out", delay, "Delay")
  connect(spread, "Out", split, "Spread")
  connect(spread, "Out", spreadRange, "In")

  connect(tapEdge, "Out", tap, "In")

  connect(delay, "Out", xfade, "A")
  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  connect(snap, "Out", feedback, "In")
  connect(feedback, "Out", delay, "Feedback")
  connect(feedback, "Out", feedbackRange, "In")

  connect(self, "In1", xfade, "B")
  connect(self, "In1", delay, "In")
  connect(xfade, "Out", self, "Out1")

  self:addMonoBranch("clock", tapEdge, "In", tapEdge, "Out")
  self:addMonoBranch("wet", fader, "In", fader, "Out")
  self:addMonoBranch("multiplier", multiplier, "In", multiplier, "Out")
  self:addMonoBranch("divider", divider, "In", divider, "Out")
  self:addMonoBranch("feedback", snap, "In", snap, "Out")
  self:addMonoBranch("spread", spread, "In", spread, "Out")
end

function ClockedDopplerDelay:loadStereoGraph()
  local delayL = self:addObject("delayL", libcore.DopplerDelay(2.0))
  local delayR = self:addObject("delayR", libcore.DopplerDelay(2.0))

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

  local period = self:addObject("period", app.Constant())
  local multiplier = self:addObject("multiplier", app.ParameterAdapter())
  local divider = self:addObject("divider", app.ParameterAdapter())
  local split = self:addObject("split", libcore.Spread())

  local spread = self:addObject("spread", app.GainBias())
  local spreadRange = self:addObject("spreadRange", app.MinMax())

  connect(tapEdge, "Out", tap, "In")
  tie(tap, "Multiplier", multiplier, "Out")
  tie(tap, "Divider", divider, "Out")
  tie(period, "Value", tap, "Derived Period")
  connect(period, "Out", split, "In")
  connect(split, "Left Out", delayL, "Delay")
  connect(split, "Right Out", delayR, "Delay")
  connect(spread, "Out", split, "Spread")
  connect(spread, "Out", spreadRange, "In")

  connect(delayL, "Out", xfade, "Left A")
  connect(delayR, "Out", xfade, "Right A")
  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  connect(snap, "Out", feedback, "In")
  connect(feedback, "Out", delayL, "Feedback")
  connect(feedback, "Out", delayR, "Feedback")
  connect(feedback, "Out", feedbackRange, "In")

  connect(self, "In1", xfade, "Left B")
  connect(self, "In1", delayL, "In")

  connect(self, "In2", xfade, "Right B")
  connect(self, "In2", delayR, "In")

  connect(xfade, "Left Out", self, "Out1")
  connect(xfade, "Right Out", self, "Out2")

  self:addMonoBranch("clock", tapEdge, "In", tapEdge, "Out")
  self:addMonoBranch("wet", fader, "In", fader, "Out")
  self:addMonoBranch("multiplier", multiplier, "In", multiplier, "Out")
  self:addMonoBranch("divider", divider, "In", divider, "Out")
  self:addMonoBranch("feedback", snap, "In", snap, "Out")
  self:addMonoBranch("spread", spread, "In", spread, "Out")
end

local views = {
  expanded = {
    "clock",
    "mult",
    "div",
    "spread",
    "feedback",
    "wet"
  },
  collapsed = {}
}

function ClockedDopplerDelay:onLoadViews(objects, branches)
  local controls = {}

  controls.clock = Gate {
    button = "clock",
    branch = branches.clock,
    description = "Clock",
    comparator = objects.tapEdge
  }

  controls.mult = GainBias {
    button = "mult",
    branch = branches.multiplier,
    description = "Multiplier",
    gainbias = objects.multiplier,
    range = objects.multiplier,
    biasMap = Encoder.getMap("int[1,32]"),
    gainMap = Encoder.getMap("[-20,20]"),
    initialBias = 1
  }

  controls.div = GainBias {
    button = "div",
    branch = branches.divider,
    description = "Divider",
    gainbias = objects.divider,
    range = objects.divider,
    biasMap = Encoder.getMap("int[1,32]"),
    gainMap = Encoder.getMap("[-20,20]"),
    initialBias = 1
  }

  -- other spots
  if self.channelCount == 2 then

    controls.spread = GainBias {
      button = "spread",
      branch = branches.spread,
      description = "Stereo Spread",
      gainbias = objects.spread,
      range = objects.spreadRange,
      biasMap = Encoder.getMap("[-1,1]")
    }

  else

    controls.spread = GainBias {
      button = "nudge",
      branch = branches.spread,
      description = "Nudge",
      gainbias = objects.spread,
      range = objects.spreadRange,
      biasMap = Encoder.getMap("[-1,1]")
    }

  end

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

  return controls, views
end

function ClockedDopplerDelay:deserialize(t)
  Unit.deserialize(self, t)
  if self:getPresetVersion(t) < 1 then
    -- v0.3.04: Changed Feedback from Constant to GainBias
    local pData = t.objects.feedback and t.objects.feedback.params
    if pData and pData.Value then
      local param = self.objects.feedback and
                        self.objects.feedback:getParameter("Bias")
      if param then
        app.logInfo(
            "ClockedDopplerDelay:deserialize: porting legacy parameter, feedback.Value.")
        param:deserialize(pData.Value)
      end
    end
  end
end

return ClockedDopplerDelay
