local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local VoltPerOctaveToHertz = Class {}
VoltPerOctaveToHertz:include(Unit)

function VoltPerOctaveToHertz:init(args)
  args.title = "V/oct to Hertz"
  args.mnemonic = "VH"
  Unit.init(self, args)
end

function VoltPerOctaveToHertz:onLoadGraph(channelCount)
  local exp = self:addObject("exp", libcore.VoltPerOctave())
  local tune = self:addObject("tune", app.ConstantOffset())
  local gain = self:addObject("gain", app.ConstantGain())
  local f0 = self:addObject("f0", app.ParameterAdapter())
  tie(gain, "Gain", f0, "Out")
  connect(self, "In1", tune, "In")
  connect(tune, "Out", exp, "In")
  connect(exp, "Out", gain, "In")
  connect(gain, "Out", self, "Out1")
  if channelCount > 1 then connect(gain, "Out", self, "Out2") end
  self:addMonoBranch("f0", f0, "In", f0, "Out")
end

local views = {
  expanded = {
    "tune",
    "f0"
  },
  collapsed = {}
}

function VoltPerOctaveToHertz:onLoadViews(objects, branches)
  local controls = {}

  controls.tune = Fader {
    button = "tune",
    param = objects.tune:getParameter("Offset"),
    description = "Tune",
    map = Encoder.getMap("cents"),
    units = app.unitCents
  }

  controls.f0 = GainBias {
    button = "f0",
    description = "Fundamental",
    branch = branches.f0,
    gainbias = objects.f0,
    range = objects.f0,
    biasMap = Encoder.getMap("oscFreq"),
    biasUnits = app.unitHertz,
    initialBias = 27.5,
    gainMap = Encoder.getMap("freqGain"),
    scaling = app.octaveScaling
  }

  return controls, views
end

return VoltPerOctaveToHertz
