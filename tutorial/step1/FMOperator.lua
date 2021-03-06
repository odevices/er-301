-- Here we bring the modules that we will need.
local Class = require "Base.Class"
local Unit = require "Unit"
local Pitch = require "Unit.ViewControl.Pitch"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"
local libcore = require "core.libcore"

-- Create a new class definition.
local FMOperator = Class {}
-- This how we derive from an existing class, in this case, Unit.
FMOperator:include(Unit)

-- The class constructor
function FMOperator:init(args)
  -- Units are constructed with a table of parameters passed in as args.
  -- This is the title that is displayed in the unit header.
  args.title = "FM Op"
  -- This is a 2-letter code displayed in places where space is minimal.
  args.mnemonic = "FM"
  -- A version that is saved with presets that can be used during deserialization.
  args.version = 2
  -- Construct the parent class.
  Unit.init(self, args)
end

-- This method is called during unit construction.  Construct the DSP graph here.
function FMOperator:onLoadGraph(channelCount)
  -- Create and add th DSP objects that we will need to populate the graph.
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

  -- Connect object outlets to inlets.
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
  -- Here we connect the outlet of the final VCA object to the output of the unit (which is actually an inlet).
  connect(vca, "Out", self, "Out1")

  -- Branches are chains where the output is fed to a given inlet of a given object.
  self:addMonoBranch("level", level, "In", level, "Out")
  self:addMonoBranch("tune", tune, "In", tune, "Out")
  self:addMonoBranch("index", index, "In", index, "Out")
  self:addMonoBranch("f0", f0, "In", f0, "Out")
  self:addMonoBranch("num", num, "In", num, "Out")
  self:addMonoBranch("den", den, "In", den, "Out")

  -- Handle stereo case here by connecting up the 2nd channel as well.
  if channelCount > 1 then connect(self.objects.vca, "Out", self, "Out2") end
end

-- A shared instance of the views table to save resources.
-- The views table determines the order in which controls appear for each view.
local views = {
  -- The default (expanded) view for a unit.
  expanded = {
    "tune",
    "freq",
    "num",
    "den",
    "index",
    "level"
  },
  -- Generally the collapsed view has no controls, but it could have some if you desire.
  collapsed = {}
}

-- This method is called during construction.  Define the views and their controls here.
function FMOperator:onLoadViews(objects, branches)
  local controls = {}

  -- There are many types of controls, each deriving from Unit.ViewControl.

  -- The Pitch control is offset-type control specifically designed for 1V/octave parameters.
  controls.tune = Pitch {
    -- The label that sits above the button (Mx).
    button = "V/oct",
    -- Which branch does this control manage?
    branch = branches.tune,
    -- The description label shown in the sub display when this control is focused.
    description = "V/oct",
    -- The object that should be wrapped by this Pitch control.
    offset = objects.tune,
    -- The object that keeps track of certain statistics used for rendering modulation.
    range = objects.tuneRange
  }

  -- The GainBias is the most common control.  
  -- It is also commonly referred to as an attenuverter plus offset in modularspeak.
  controls.freq = GainBias {
    button = "f0",
    description = "Fundamental",
    branch = branches.f0,
    -- The object that this control wraps.
    gainbias = objects.f0,
    range = objects.f0Range,
    -- The dial map for the bias fader.  
    -- Dial maps control how encoder motion relates to changes in the value of the underlying parameter.
    biasMap = Encoder.getMap("oscFreq"),
    -- Display units.
    biasUnits = app.unitHertz,
    -- You can optionally set an initial value for the bias here.
    initialBias = 27.5,
    -- Same as for the bias, but this is the dial map for the gain parameter.
    gainMap = Encoder.getMap("freqGain"),
    -- This only affects how the fader position is drawn.
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

  -- This method expects us to return the table of controls and views.
  return controls, views
end

-- Return the class definition.
return FMOperator
