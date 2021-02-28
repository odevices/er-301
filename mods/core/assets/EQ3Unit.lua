local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local EQ3Unit = Class {}
EQ3Unit:include(Unit)

function EQ3Unit:init(args)
  args.title = "EQ3"
  args.mnemonic = "EQ"
  Unit.init(self, args)
end

function EQ3Unit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function EQ3Unit:loadMonoGraph()
  local equalizer = self:addObject("equalizer", libcore.Equalizer3())
  connect(self, "In1", equalizer, "In")
  connect(equalizer, "Out", self, "Out1")

  local lowGain = self:addObject("lowGain", app.GainBias())
  local lowGainRange = self:addObject("lowGainRange", app.MinMax())

  local midGain = self:addObject("midGain", app.GainBias())
  local midGainRange = self:addObject("midGainRange", app.MinMax())

  local highGain = self:addObject("highGain", app.GainBias())
  local highGainRange = self:addObject("highGainRange", app.MinMax())

  connect(lowGain, "Out", equalizer, "Low Gain")
  connect(lowGain, "Out", lowGainRange, "In")

  connect(midGain, "Out", equalizer, "Mid Gain")
  connect(midGain, "Out", midGainRange, "In")

  connect(highGain, "Out", equalizer, "High Gain")
  connect(highGain, "Out", highGainRange, "In")

  self:addMonoBranch("lowGain", lowGain, "In", lowGain, "Out")
  self:addMonoBranch("midGain", midGain, "In", midGain, "Out")
  self:addMonoBranch("highGain", highGain, "In", highGain, "Out")
end

function EQ3Unit:loadStereoGraph()
  local equalizer1 = self:addObject("equalizer1", libcore.Equalizer3())
  connect(self, "In1", equalizer1, "In")
  connect(equalizer1, "Out", self, "Out1")

  local equalizer2 = self:addObject("equalizer2", libcore.Equalizer3())
  connect(self, "In2", equalizer2, "In")
  connect(equalizer2, "Out", self, "Out2")

  local lowGain = self:addObject("lowGain", app.GainBias())
  local lowGainRange = self:addObject("lowGainRange", app.MinMax())

  local midGain = self:addObject("midGain", app.GainBias())
  local midGainRange = self:addObject("midGainRange", app.MinMax())

  local highGain = self:addObject("highGain", app.GainBias())
  local highGainRange = self:addObject("highGainRange", app.MinMax())

  connect(lowGain, "Out", equalizer1, "Low Gain")
  connect(lowGain, "Out", equalizer2, "Low Gain")
  connect(lowGain, "Out", lowGainRange, "In")

  connect(midGain, "Out", equalizer1, "Mid Gain")
  connect(midGain, "Out", equalizer2, "Mid Gain")
  connect(midGain, "Out", midGainRange, "In")

  connect(highGain, "Out", equalizer1, "High Gain")
  connect(highGain, "Out", equalizer2, "High Gain")
  connect(highGain, "Out", highGainRange, "In")

  self:addMonoBranch("lowGain", lowGain, "In", lowGain, "Out")
  self:addMonoBranch("midGain", midGain, "In", midGain, "Out")
  self:addMonoBranch("highGain", highGain, "In", highGain, "Out")

  tie(equalizer2, "Low Freq", equalizer1, "Low Freq")
  tie(equalizer2, "High Freq", equalizer1, "High Freq")

  self.objects.equalizer = self.objects.equalizer1
end

local views = {
  expanded = {
    "lowGain",
    "midGain",
    "highGain",
    "lowFreq",
    "highFreq"
  },
  collapsed = {}
}

function EQ3Unit:onLoadViews(objects, branches)
  local controls = {}

  controls.lowGain = GainBias {
    button = "low(dB)",
    branch = branches.lowGain,
    description = "Low Gain",
    gainbias = objects.lowGain,
    range = objects.lowGainRange,
    biasMap = Encoder.getMap("volume"),
    biasUnits = app.unitDecibels,
    gainMap = Encoder.getMap("[-10,10]"),
    initialBias = 1.0
  }

  controls.midGain = GainBias {
    button = "mid(dB)",
    branch = branches.midGain,
    description = "Mid Gain",
    gainbias = objects.midGain,
    range = objects.midGainRange,
    biasMap = Encoder.getMap("volume"),
    biasUnits = app.unitDecibels,
    gainMap = Encoder.getMap("[-10,10]"),
    initialBias = 1.0
  }

  controls.highGain = GainBias {
    button = "high(dB)",
    branch = branches.highGain,
    description = "High Gain",
    gainbias = objects.highGain,
    range = objects.highGainRange,
    biasMap = Encoder.getMap("volume"),
    biasUnits = app.unitDecibels,
    gainMap = Encoder.getMap("[-10,10]"),
    initialBias = 1.0
  }

  controls.lowFreq = Fader {
    button = "low(Hz)",
    description = "Low Freq",
    param = objects.equalizer:getParameter("Low Freq"),
    map = Encoder.getMap("filterFreq"),
    units = app.unitHertz,
    scaling = app.octaveScaling
  }

  controls.highFreq = Fader {
    button = "high(Hz)",
    description = "High Freq",
    param = objects.equalizer:getParameter("High Freq"),
    monitor = self,
    map = Encoder.getMap("filterFreq"),
    units = app.unitHertz,
    scaling = app.octaveScaling
  }

  return controls, views
end

return EQ3Unit
