local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"
local Encoder = require "Encoder"
local Utils = require "Utils"

local DelayUnit = Class {}
DelayUnit:include(Unit)

function DelayUnit:init(args)
  args.title = "Delay"
  args.mnemonic = "D"
  Unit.init(self, args)
end

function DelayUnit:onLoadGraph(channelCount)
  if channelCount > 1 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function DelayUnit:loadMonoGraph()
  local delay = self:addObject("delay", libcore.Delay(1))

  local secs = self:addObject("secsL", app.ParameterAdapter())

  local xfade = self:addObject("xfade", app.CrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  local feedback = self:addObject("feedback", app.GainBias())
  local feedbackRange = self:addObject("feedbackRange", app.MinMax())
  local snap = self:addObject("snap", libcore.SnapToZero())
  snap:setThresholdInDecibels(-35.9)

  connect(delay, "Left Out", xfade, "A")
  tie(delay, "Left Delay", secs, "Out")

  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  connect(snap, "Out", feedback, "In")
  connect(feedback, "Out", delay, "Feedback")
  connect(feedback, "Out", feedbackRange, "In")

  connect(self, "In1", xfade, "B")
  connect(self, "In1", delay, "Left In")
  connect(xfade, "Out", self, "Out1")

  self:addMonoBranch("delayL", secs, "In", secs, "Out")
  self:addMonoBranch("wet", fader, "In", fader, "Out")
  self:addMonoBranch("feedback", snap, "In", snap, "Out")
end

function DelayUnit:loadStereoGraph()
  local delay = self:addObject("delay", libcore.Delay(2))
  local secsL = self:addObject("secsL", app.ParameterAdapter())
  local secsR = self:addObject("secsR", app.ParameterAdapter())

  local xfade = self:addObject("xfade", app.StereoCrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  local feedback = self:addObject("feedback", app.GainBias())
  local feedbackRange = self:addObject("feedbackRange", app.MinMax())
  local snap = self:addObject("snap", libcore.SnapToZero())
  snap:setThresholdInDecibels(-35.9)

  connect(delay, "Left Out", xfade, "Left A")
  tie(delay, "Left Delay", secsL, "Out")

  connect(delay, "Right Out", xfade, "Right A")
  tie(delay, "Right Delay", secsR, "Out")

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

  self:addMonoBranch("delayL", secsL, "In", secsL, "Out")
  self:addMonoBranch("delayR", secsR, "In", secsR, "Out")
  self:addMonoBranch("wet", fader, "In", fader, "Out")
  self:addMonoBranch("feedback", snap, "In", snap, "Out")
end

local function timeMap(max, n)
  local map = app.LinearDialMap(0, max)
  map:setCoarseRadix(n)
  return map
end

function DelayUnit:setMaxDelayTime(secs)
  local requested = Utils.round(secs, 1)
  local allocated = self.objects.delay:allocateTimeUpTo(requested)
  allocated = Utils.round(allocated, 1)
  if allocated > 0 then
    local map = timeMap(allocated, 100)
    self.controls.delayL:setBiasMap(map)
    if self.channelCount > 1 then
      self.controls.delayR:setBiasMap(map)
    end
  end
end

local menu = {
  "setHeader",
  "set200ms",
  "set2s",
  "set10s",
  "set30s"
}

function DelayUnit:onShowMenu(objects, branches)
  local controls = {}
  local allocated = self.objects.delay:maximumDelayTime()
  allocated = Utils.round(allocated, 1)
  controls.setHeader = MenuHeader {
    description = string.format("Current Maximum Delay is %0.1fs.", allocated)
  }

  controls.set200ms = Task {
    description = "0.2s",
    task = function()
      self:setMaxDelayTime(0.2)
    end
  }

  controls.set2s = Task {
    description = "2s",
    task = function()
      self:setMaxDelayTime(2)
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

local views = {
  expanded = {
    "delayL",
    "delayR",
    "feedback",
    "wet"
  },
  collapsed = {}
}

function DelayUnit:onLoadViews(objects, branches)
  local controls = {}

  if self.channelCount > 1 then
    controls.delayL = GainBias {
      button = "delay(L)",
      branch = branches.delayL,
      description = "Left Delay",
      gainbias = objects.secsL,
      range = objects.secsL,
      biasMap = Encoder.getMap("unit"),
      biasUnits = app.unitSecs
    }

    controls.delayR = GainBias {
      button = "delay(R)",
      branch = branches.delayR,
      description = "Right Delay",
      gainbias = objects.secsR,
      range = objects.secsR,
      biasMap = Encoder.getMap("unit"),
      biasUnits = app.unitSecs
    }
  else
    controls.delayL = GainBias {
      button = "delay",
      branch = branches.delayL,
      description = "Delay",
      gainbias = objects.secsL,
      range = objects.secsL,
      biasMap = Encoder.getMap("unit"),
      biasUnits = app.unitSecs
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

function DelayUnit:onLoadFinished()
  self:setMaxDelayTime(2.0)
end

function DelayUnit:deserializeLegacyPreset(t)
  local Serialization = require "Persist.Serialization"
  -- v0.3.09: Changed Feedback from Constant to GainBias
  local fdbk = Serialization.get("objects/feedback/params/Value", t)
  if fdbk then
    app.logInfo(
        "%s:deserialize:legacy preset detected:setting feedback bias to %s",
        self, fdbk)
    self.objects.feedback:deserialize("Bias", fdbk)
  end
  -- v0.3.09: Changed Wet/Dry from Constant to GainBias
  local wet = Serialization.get("objects/fader/params/Value", t)
  if wet then
    app.logInfo("%s:deserialize:legacy preset detected:setting wet bias to %s",
                self, wet)
    self.objects.fader:deserialize("Bias", wet)
  end
  -- v0.3.09: Changed Delay from parameter to ParameterAdapter
  local delay = Serialization.get("objects/delay/params/Delay", t)
  if delay then
    app.logInfo(
        "%s:deserialize:legacy preset detected:setting delay bias to %s", self,
        delay)
    self.objects.secsL:deserialize("Bias", delay)
    if self.channelCount > 1 then
      self.objects.secsR:deserialize("Bias", delay)
    end
  end
  local delayL = Serialization.get("objects/delayL/params/Delay", t)
  if delayL then
    app.logInfo(
        "%s:deserialize:legacy preset detected:setting delayL bias to %s", self,
        delayL)
    self.objects.secsL:deserialize("Bias", delayL)
  end
  if self.channelCount > 1 then
    local delayR = Serialization.get("objects/delayR/params/Delay", t)
    if delayR then
      app.logInfo(
          "%s:deserialize:legacy preset detected:setting delayR bias to %s",
          self, delayR)
      self.objects.secsR:deserialize("Bias", delayR)
    end
  end
end

function DelayUnit:serialize()
  local t = Unit.serialize(self)
  t.maximumDelayTime = self.objects.delay:maximumDelayTime()
  return t
end

function DelayUnit:deserialize(t)
  local time = t.maximumDelayTime
  if time and time > 0 then
    self:setMaxDelayTime(time)
  end
  Unit.deserialize(self, t)
  if self:getPresetVersion(t) < 1 then
    self:deserializeLegacyPreset(t)
  end
end

function DelayUnit:onRemove()
  self.objects.delay:deallocate()
  Unit.onRemove(self)
end

return DelayUnit
