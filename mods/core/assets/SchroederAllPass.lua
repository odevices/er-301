local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"

local SchroederAllPass = Class {}
SchroederAllPass:include(Unit)

function SchroederAllPass:init(args)
  args.title = "Schroeder Allpass"
  args.mnemonic = "SA"
  Unit.init(self, args)
end

function SchroederAllPass:onLoadGraph(channelCount)
  local delay = self:addObject("delay", libcore.Delay(1))
  delay:allocateTimeUpTo(0.05)
  local delayAdapter = self:addObject("delayAdapter", app.ParameterAdapter())
  local gainAdapter = self:addObject("gainAdapter", app.ParameterAdapter())
  local feedBackMix = self:addObject("feedBackMix", app.Sum())
  local feedBackGain = self:addObject("feedBackGain", app.ConstantGain())
  local feedForwardMix = self:addObject("feedForwardMix", app.Sum())
  local feedForwardGain = self:addObject("feedForwardGain", app.ConstantGain())
  local limiter = self:addObject("limiter", libcore.Limiter())

  connect(self, "In1", feedBackMix, "Left")
  connect(feedBackMix, "Out", delay, "Left In")
  connect(delay, "Left Out", feedForwardMix, "Left")
  connect(feedForwardMix, "Out", self, "Out1")
  connect(feedBackMix, "Out", feedForwardGain, "In")
  connect(feedForwardGain, "Out", feedForwardMix, "Right")
  connect(delay, "Left Out", feedBackGain, "In")
  connect(feedBackGain, "Out", limiter, "In")
  connect(limiter, "Out", feedBackMix, "Right")

  tie(delay, "Left Delay", delayAdapter, "Out")
  tie(feedBackGain, "Gain", gainAdapter, "Out")
  tie(feedForwardGain, "Gain", "negate", gainAdapter, "Out")

  self:addMonoBranch("delay", delayAdapter, "In", delayAdapter, "Out")
  self:addMonoBranch("gain", gainAdapter, "In", gainAdapter, "Out")
end

local views = {
  expanded = {
    "delay",
    "gain"
  },
  collapsed = {}
}

local function linMap(min, max, n)
  local map = app.LinearDialMap(min, max)
  map:setCoarseRadix(n)
  return map
end

local delayMap = linMap(0, 0.05, 100)

function SchroederAllPass:onLoadViews(objects, branches)
  local controls = {}

  controls.delay = GainBias {
    button = "delay",
    description = "Delay Time",
    branch = branches.delay,
    gainbias = objects.delayAdapter,
    range = objects.delayAdapter,
    biasMap = delayMap,
    biasUnits = app.unitSecs,
    initialBias = 0.005
  }

  controls.gain = GainBias {
    button = "gain",
    description = "Loop Gain",
    branch = branches.gain,
    gainbias = objects.gainAdapter,
    range = objects.gainAdapter,
    biasMap = nil,
    biasUnits = app.unitNone,
    initialBias = 0.7
  }

  return controls, views
end

function SchroederAllPass:onRemove()
  self.objects.delay:deallocate()
  Unit.onRemove(self)
end

return SchroederAllPass
