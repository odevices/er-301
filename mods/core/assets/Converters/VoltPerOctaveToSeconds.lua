local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local VoltPerOctaveToSeconds = Class {}
VoltPerOctaveToSeconds:include(Unit)

function VoltPerOctaveToSeconds:init(args)
  args.title = "V/oct to Seconds"
  args.mnemonic = "VS"
  Unit.init(self, args)
end

function VoltPerOctaveToSeconds:onLoadGraph(channelCount)
  local negate = self:addObject("negate", app.ConstantGain())
  negate:hardSet("Gain", -1)
  local exp = self:addObject("exp", libcore.VoltPerOctave())
  local tune = self:addObject("tune", app.ConstantOffset())
  local gain = self:addObject("gain", app.ConstantGain())
  local p0 = self:addObject("p0", app.ParameterAdapter())
  tie(gain, "Gain", p0, "Out")
  connect(self, "In1", tune, "In")
  connect(tune, "Out", negate, "In")
  connect(negate, "Out", exp, "In")
  connect(exp, "Out", gain, "In")
  connect(gain, "Out", self, "Out1")
  if channelCount > 1 then
    connect(gain, "Out", self, "Out2")
  end
  self:addMonoBranch("p0", p0, "In", p0, "Out")
end

local views = {
  expanded = {
    "tune",
    "p0"
  },
  collapsed = {}
}

function VoltPerOctaveToSeconds:onLoadViews(objects, branches)
  local controls = {}

  controls.tune = Fader {
    button = "tune",
    param = objects.tune:getParameter("Offset"),
    description = "Tune",
    map = Encoder.getMap("cents"),
    units = app.unitCents
  }

  controls.p0 = GainBias {
    button = "p0",
    description = "Fundamental",
    branch = branches.p0,
    gainbias = objects.p0,
    range = objects.p0,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitSecs,
    initialBias = 1.0 / 27.5,
    gainMap = Encoder.getMap("gain")
  }

  return controls, views
end

return VoltPerOctaveToSeconds
