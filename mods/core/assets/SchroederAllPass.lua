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

function SchroederAllPass:createChannel(i, delayAdapter, gainAdapter)
  local delay = self:addObject("delay"..i, libcore.Delay(1))
  delay:allocateTimeUpTo(0.05)
  local feedBackMix = self:addObject("feedBackMix"..i, app.Sum())
  local feedBackGain = self:addObject("feedBackGain"..i, app.ConstantGain())
  local feedForwardMix = self:addObject("feedForwardMix"..i, app.Sum())
  local feedForwardGain = self:addObject("feedForwardGain"..i, app.ConstantGain())
  local limiter = self:addObject("limiter"..i, libcore.Limiter())

  connect(self, "In"..i, feedBackMix, "Left")
  connect(feedBackMix, "Out", delay, "Left In")
  connect(delay, "Left Out", feedForwardMix, "Left")
  connect(feedForwardMix, "Out", self, "Out"..i)
  connect(feedBackMix, "Out", feedForwardGain, "In")
  connect(feedForwardGain, "Out", feedForwardMix, "Right")
  connect(delay, "Left Out", feedBackGain, "In")
  connect(feedBackGain, "Out", limiter, "In")
  connect(limiter, "Out", feedBackMix, "Right")

  tie(delay, "Left Delay", delayAdapter, "Out")
  tie(feedBackGain, "Gain", gainAdapter, "Out")
  tie(feedForwardGain, "Gain", "negate", gainAdapter, "Out")
end

function SchroederAllPass:onLoadGraph(channelCount)
  local delayAdapter = self:addObject("delayAdapter", app.ParameterAdapter())
  local gainAdapter = self:addObject("gainAdapter", app.ParameterAdapter())

  for i = 1, channelCount do
    self:createChannel(i, delayAdapter, gainAdapter)
  end
 
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
  self.objects.delay1:deallocate()
  if self.objects.delay2 then
    self.objects.delay2:deallocate()
  end
  Unit.onRemove(self)
end

return SchroederAllPass
