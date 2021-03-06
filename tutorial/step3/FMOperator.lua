local Class = require "Base.Class"
local Unit = require "Unit"
local Pitch = require "Unit.ViewControl.Pitch"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"
local libcore = require "core.libcore"

local FMOperator = Class {}
FMOperator:include(Unit)

function FMOperator:init(args)
  args.title = "FM Op"
  args.mnemonic = "FM"
  args.version = 2
  Unit.init(self, args)
end

function FMOperator:onLoadGraph(channelCount)
  local carrier = self:addObject("carrier", libcore.SineOscillator())
  local modulator = self:addObject("modulator", libcore.SineOscillator())
  local rational = self:addObject("rational", libcore.RationalMultiply(true))
  local multiply = self:addObject("multiply", app.Multiply())
  local tune = self:addObject("tune", app.ConstantOffset())
  local tuneRange = self:addObject("tuneRange", app.MinMax())
  local f0 = self:addObject("f0", app.GainBias())
  local f0Range = self:addObject("f0Range", app.MinMax())
  local vca = self:addObject("vca", app.Multiply())
  local level = self:addObject("level", app.GainBias())
  local levelRange = self:addObject("levelRange", app.MinMax())
  local num = self:addObject("num", app.GainBias())
  local numRange = self:addObject("numRange", app.MinMax())
  local den = self:addObject("den", app.GainBias())
  local denRange = self:addObject("denRange", app.MinMax())
  local index = self:addObject("index", app.GainBias())
  local indexRange = self:addObject("indexRange", app.MinMax())

  connect(tune, "Out", tuneRange, "In")
  connect(tune, "Out", carrier, "V/Oct")

  connect(f0, "Out", carrier, "Fundamental")
  connect(f0, "Out", f0Range, "In")

  connect(f0, "Out", rational, "In")
  connect(rational, "Out", modulator, "Fundamental")
  connect(num, "Out", rational, "Numerator")
  connect(num, "Out", numRange, "In")
  connect(den, "Out", rational, "Divisor")
  connect(den, "Out", denRange, "In")

  connect(index, "Out", multiply, "Left")
  connect(index, "Out", indexRange, "In")
  connect(modulator, "Out", multiply, "Right")
  connect(multiply, "Out", carrier, "Phase")

  connect(level, "Out", levelRange, "In")
  connect(level, "Out", vca, "Left")

  connect(carrier, "Out", vca, "Right")
  connect(vca, "Out", self, "Out1")

  self:addMonoBranch("level", level, "In", level, "Out")
  self:addMonoBranch("tune", tune, "In", tune, "Out")
  self:addMonoBranch("index", index, "In", index, "Out")
  self:addMonoBranch("f0", f0, "In", f0, "Out")
  self:addMonoBranch("num", num, "In", num, "Out")
  self:addMonoBranch("den", den, "In", den, "Out")

  if channelCount > 1 then connect(self.objects.vca, "Out", self, "Out2") end
end

local views = {
  expanded = {
    "tune",
    "freq",
    "num",
    "den",
    "index",
    "level"
  },
  collapsed = {}
}

function FMOperator:onLoadViews(objects, branches)
  local controls = {}

  controls.tune = Pitch {
    button = "V/oct",
    branch = branches.tune,
    description = "V/oct",
    offset = objects.tune,
    range = objects.tuneRange
  }

  controls.freq = GainBias {
    button = "f0",
    description = "Fundamental",
    branch = branches.f0,
    gainbias = objects.f0,
    range = objects.f0Range,
    biasMap = Encoder.getMap("oscFreq"),
    biasUnits = app.unitHertz,
    initialBias = 27.5,
    gainMap = Encoder.getMap("freqGain"),
    scaling = app.octaveScaling
  }

  controls.num = GainBias {
    button = "num",
    description = "Numerator",
    branch = branches.num,
    gainbias = objects.num,
    range = objects.numRange,
    biasMap = Encoder.getMap("int[1,32]"),
    biasPrecision = 0,
    initialBias = 1
  }

  controls.den = GainBias {
    button = "den",
    description = "Denominator",
    branch = branches.den,
    gainbias = objects.den,
    range = objects.denRange,
    biasMap = Encoder.getMap("int[1,32]"),
    biasPrecision = 0,
    initialBias = 1
  }

  controls.index = GainBias {
    button = "index",
    description = "Index",
    branch = branches.index,
    gainbias = objects.index,
    range = objects.indexRange,
    initialBias = 0.1
  }

  controls.level = GainBias {
    button = "level",
    description = "Level",
    branch = branches.level,
    gainbias = objects.level,
    range = objects.levelRange,
    initialBias = 0.5
  }

  return controls, views
end

return FMOperator
