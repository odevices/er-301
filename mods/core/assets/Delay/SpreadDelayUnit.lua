local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local GainBias = require "Unit.ViewControl.GainBias"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"
local Encoder = require "Encoder"
local Utils = require "Utils"

local SpreadDelayUnit = Class {}
SpreadDelayUnit:include(Unit)

function SpreadDelayUnit:init(args)
  args.title = "Spread Delay"
  args.mnemonic = "SD"
  Unit.init(self, args)
end

function SpreadDelayUnit:onLoadGraph(channelCount)
  if channelCount ~= 2 then app.logError("%s: can only load into a stereo chain.") end

  local delay = self:addObject("delay", libcore.Delay(2))

  local xfade = self:addObject("xfade", app.StereoCrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  local feedback = self:addObject("feedback", app.GainBias())
  local feedbackRange = self:addObject("feedbackRange", app.MinMax())
  local snap = self:addObject("snap", libcore.SnapToZero())
  snap:setThresholdInDecibels(-35.9)

  local secs = self:addObject("secs", app.ParameterAdapter())
  local spread = self:addObject("spread", app.ParameterAdapter())

  tie(delay, "Left Delay", secs, "Out")
  tie(delay, "Right Delay", secs, "Out")
  tie(delay, "Spread", spread, "Out")

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

  self:addMonoBranch("delay", secs, "In", secs, "Out")
  self:addMonoBranch("spread", spread, "In", spread, "Out")
  self:addMonoBranch("wet", fader, "In", fader, "Out")
  self:addMonoBranch("feedback", snap, "In", snap, "Out")
end

local function linMap(min, max, n)
  local map = app.LinearDialMap(min, max)
  map:setCoarseRadix(n)
  return map
end

function SpreadDelayUnit:setMaxDelayTime(secs)
  local requested = math.floor(secs + 0.5)
  local allocated = self.objects.delay:allocateTimeUpTo(requested)
  allocated = math.floor(allocated + 0.5)
  if allocated > 0 then
    local map = linMap(0, allocated, 100)
    self.controls.delay:setBiasMap(map)
  end
end

local menu = {
  "setHeader",
  "set100ms",
  "set1s",
  "set10s",
  "set30s"
}

function SpreadDelayUnit:onShowMenu(objects, branches)
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

function SpreadDelayUnit:onLoadViews(objects, branches)
  local controls = {}
  local views = {
    collapsed = {},
    expanded = {
      "delay",
      "spread",
      "feedback",
      "wet"
    }
  }

  controls.delay = GainBias {
    button = "delay",
    branch = branches.delay,
    description = "Delay",
    gainbias = objects.secs,
    range = objects.secs,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitSecs
  }

  controls.spread = GainBias {
    button = "spread",
    branch = branches.spread,
    description = "Stereo Spread",
    gainbias = objects.spread,
    range = objects.spread,
    biasMap = Encoder.getMap("[-1,1]")
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

  return controls, views
end

function SpreadDelayUnit:onLoadFinished()
  self:setMaxDelayTime(1.0)
end

function SpreadDelayUnit:serialize()
  local t = Unit.serialize(self)
  t.maximumDelayTime = self.objects.delay:maximumDelayTime()
  return t
end

function SpreadDelayUnit:deserialize(t)
  local time = t.maximumDelayTime
  if time and time > 0 then self:setMaxDelayTime(time) end
  Unit.deserialize(self, t)
end

function SpreadDelayUnit:onRemove()
  self.objects.delay:deallocate()
  Unit.onRemove(self)
end

local function factory(args)
  local chain = args.chain or
                    app.logError("SpreadDelayUnit.factory: chain is missing.")
  if chain.channelCount == 2 then
    return SpreadDelayUnit(args)
  else
    local DelayUnit = require "core.Delay.DelayUnit"
    return DelayUnit(args)
  end
end

return factory
