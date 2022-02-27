local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"
local Encoder = require "Encoder"
local Utils = require "Utils"

local aliases = {
  secs = {
    "secsL",
    "secsR"
  },
  secsL = {
    "secsL"
  }
}

local DopplerDelayUnit = Class {}
DopplerDelayUnit:include(Unit)

function DopplerDelayUnit:init(args)
  args.title = "Doppler Delay"
  args.mnemonic = "DD"
  args.aliases = aliases
  Unit.init(self, args)
end

function DopplerDelayUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function DopplerDelayUnit:loadMonoGraph()
  local delay = self:addObject("delay", libcore.DopplerDelay(2.0))
  local secs = self:addObject("secs", app.GainBias())
  local secsRange = self:addObject("secsRange", app.MinMax())

  local xfade = self:addObject("xfade", app.CrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  local feedback = self:addObject("feedback", app.GainBias())
  local feedbackRange = self:addObject("feedbackRange", app.MinMax())
  local snap = self:addObject("snap", libcore.SnapToZero())
  snap:setThresholdInDecibels(-35.9)

  -- connect objects
  connect(delay, "Out", xfade, "A")
  connect(secs, "Out", delay, "Delay")
  connect(secs, "Out", secsRange, "In")

  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  connect(snap, "Out", feedback, "In")
  connect(feedback, "Out", delay, "Feedback")
  connect(feedback, "Out", feedbackRange, "In")

  connect(self, "In1", xfade, "B")
  connect(self, "In1", delay, "In")
  connect(xfade, "Out", self, "Out1")

  self:addMonoBranch("delay", secs, "In", secs, "Out")
  self:addMonoBranch("wet", fader, "In", fader, "Out")
  self:addMonoBranch("feedback", snap, "In", snap, "Out")
end

function DopplerDelayUnit:loadStereoGraph()
  local delayL = self:addObject("delayL", libcore.DopplerDelay(2.0))
  local secsL = self:addObject("secsL", app.GainBias())
  local secsLRange = self:addObject("secsLRange", app.MinMax())

  local delayR = self:addObject("delayR", libcore.DopplerDelay(2.0))
  local secsR = self:addObject("secsR", app.GainBias())
  local secsRRange = self:addObject("secsRRange", app.MinMax())

  local xfade = self:addObject("xfade", app.StereoCrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  local feedback = self:addObject("feedback", app.GainBias())
  local feedbackRange = self:addObject("feedbackRange", app.MinMax())
  local snap = self:addObject("snap", libcore.SnapToZero())
  snap:setThresholdInDecibels(-35.9)

  connect(delayL, "Out", xfade, "Left A")
  connect(secsL, "Out", delayL, "Delay")
  connect(secsL, "Out", secsLRange, "In")

  connect(delayR, "Out", xfade, "Right A")
  connect(secsR, "Out", delayR, "Delay")
  connect(secsR, "Out", secsRRange, "In")

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

function DopplerDelayUnit:setMaxDelayTime(secs)
  local requested = Utils.round(secs, 1)
  local allocated
  if self.channelCount > 1 then
    local allocatedL = self.objects.delayL:allocateTimeUpTo(requested)
    local allocatedR = self.objects.delayR:allocateTimeUpTo(requested)
    allocated = math.min(allocatedL, allocatedR)
  else
    allocated = self.objects.delay:allocateTimeUpTo(requested)
  end
  allocated = Utils.round(allocated, 1)
  if allocated > 0 then
    local map = timeMap(allocated, 100)
    if self.channelCount > 1 then
      self.controls.leftDelay:setBiasMap(map)
      self.controls.rightDelay:setBiasMap(map)
    else
      self.controls.delay:setBiasMap(map)
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

function DopplerDelayUnit:onShowMenu(objects, branches)
  local controls = {}
  local delay = self.objects.delay or self.objects.delayL
  local allocated = delay:maximumDelayTime()
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

function DopplerDelayUnit:onLoadViews(objects, branches)
  local controls = {}
  local views = {
    collapsed = {}
  }

  if self.channelCount == 2 then

    views.expanded = {
      "leftDelay",
      "rightDelay",
      "feedback",
      "wet"
    }

    controls.leftDelay = GainBias {
      button = "delay(L)",
      branch = branches.delayL,
      description = "Left Delay",
      gainbias = objects.secsL,
      range = objects.secsLRange,
      biasMap = Encoder.getMap("[0,2]"),
      biasUnits = app.unitSecs
    }

    controls.rightDelay = GainBias {
      button = "delay(R)",
      branch = branches.delayR,
      description = "Right Delay",
      gainbias = objects.secsR,
      range = objects.secsRRange,
      biasMap = Encoder.getMap("[0,2]"),
      biasUnits = app.unitSecs
    }

  else

    views.expanded = {
      "delay",
      "feedback",
      "wet"
    }

    controls.delay = GainBias {
      button = "delay",
      branch = branches.delay,
      description = "Delay",
      gainbias = objects.secs,
      range = objects.secsRange,
      biasMap = Encoder.getMap("[0,2]"),
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

function DopplerDelayUnit:serialize()
  local t = Unit.serialize(self)
  if self.channelCount > 1 then
    t.maximumDelayTime = self.objects.delayL:maximumDelayTime()
  else
    t.maximumDelayTime = self.objects.delay:maximumDelayTime()
  end
  return t
end

function DopplerDelayUnit:deserialize(t)
  local time = t.maximumDelayTime
  if time and time > 0 then
    self:setMaxDelayTime(time)
  end
  Unit.deserialize(self, t)
end

function DopplerDelayUnit:onRemove()
  if self.channelCount > 1 then
    self.objects.delayL:deallocate()
    self.objects.delayR:deallocate()
  else
    self.objects.delay:deallocate()
  end
  Unit.onRemove(self)
end

return DopplerDelayUnit
