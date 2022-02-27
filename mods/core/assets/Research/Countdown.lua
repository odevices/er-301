local libcore = require "core.libcore"
-- The Class module implements basic OOP functionality
local Class = require "Base.Class"
-- The base class for all units
local Unit = require "Unit"
-- A graphical control for the unit menu
local OptionControl = require "Unit.MenuControl.OptionControl"
-- A graphical control for comparator objects on the unit input
local InputGate = require "Unit.ViewControl.InputGate"
-- A graphical control for gainbias objects
local GainBias = require "Unit.ViewControl.GainBias"
-- A graphical control for comparator objects
local Gate = require "Unit.ViewControl.Gate"
-- Needed to get access to the pre-defined encoder maps
local Encoder = require "Encoder"

-- Create an empty class definition for the Countdown unit
local Countdown = Class {}
-- Use inclusion to effectively inherit from the Unit class
Countdown:include(Unit)

-- The constructor
function Countdown:init(args)
  -- This is the default title shown in the unit header.
  args.title = "Count- down"
  -- This is the 2 letter abbreviation for this unit.
  args.mnemonic = "Cd"
  -- Optionally, set a version number unique to this unit.
  args.version = 1
  -- Make sure to call the parent class constructor also.
  Unit.init(self, args)
end

-- This method will be called during unit instantiation. Create the DSP graph here.
-- pUnit : an object containing the unit's input and output ports.
-- channelCount: used to determine if we are building a mono or stereo version of this unit.
function Countdown:onLoadGraph(channelCount)
  -- The addObject method is used to instantiate and name DSP objects.

  -- Create a Gate object for digitizing the incoming signal into triggers.
  local input = self:addObject("input", app.Comparator())
  -- A comparator has trigger, gate and toggle output modes.  Configure the comparator to output triggers.
  input:setTriggerMode()

  -- Create a Counter object, turn off wrapping and set its initial parameter values.
  local counter = self:addObject("counter", libcore.Counter())
  counter:setOptionValue("Wrap", app.CHOICE_NO)
  counter:hardSet("Gain", 1)
  counter:hardSet("Step Size", -1)
  counter:hardSet("Start", 0)

  -- Here we connect the output of the 'input' comparator to the input of the 'counter' object.
  connect(input, "Out", counter, "In")

  -- Since a comparator only fires when the threshold is exceeded from below, let's negate the output of the counter.
  local negate = self:addObject("negate", app.ConstantGain())
  negate:hardSet("Gain", -1)
  connect(counter, "Out", negate, "In")

  -- Create another Gate object for the output.
  local output = self:addObject("output", app.Comparator())
  output:setTriggerMode()
  output:hardSet("Threshold", -0.5)
  connect(negate, "Out", output, "In")

  -- And yet another Gate object for the reset control.
  local reset = self:addObject("reset", app.Comparator())
  reset:setTriggerMode()
  connect(reset, "Out", counter, "Reset")

  -- We need an external control for setting what value to start counting from.
  local start = self:addObject("start", app.ParameterAdapter())
  -- Give it an initial value, otherwise it will be zero.
  start:hardSet("Bias", 4)

  -- Unlike audio-rate signals, parameters are tied together like this slave parameter to master parameter.  Think of it as an assignment.
  -- Note: We need to use the counter's Finish parameter because our step size is negative.
  tie(counter, "Finish", start, "Out")

  -- Register sub-chains (internally called branches) for modulation.
  self:addMonoBranch("start", start, "In", start, "Out")
  self:addMonoBranch("reset", reset, "In", reset, "Out")

  -- Finally, connect the output of the 'output' Gate to the unit output(s).
  connect(output, "Out", self, "Out1")
  if channelCount > 1 then
    connect(output, "Out", self, "Out2")
  end

  -- Force a reset to occur, so that the counter is ready to go.
  reset:simulateRisingEdge()
  reset:simulateFallingEdge()
end

-- Describe the layout of the menu in terms of its controls.
local menu = {
  "wrap",
  "processingRate"
}

-- Here we create each control for the menu.
function Countdown:onShowMenu(objects, branches)
  local controls = {}

  controls.wrap = OptionControl {
    description = "Wrap?",
    option = objects.counter:getOption("Wrap"),
    choices = {
      "yes",
      "no"
    }
  }

  controls.processingRate = OptionControl {
    description = "Process Rate",
    option = objects.counter:getOption("Processing Rate"),
    choices = {
      "frame",
      "sample"
    }
  }

  return controls, menu
end

-- Describe the layout of unit expanded and collapsed views in terms of its controls.
local views = {
  expanded = {
    "input",
    "count",
    "reset"
  },
  collapsed = {}
}

-- Here we create each control for the unit.
function Countdown:onLoadViews(objects, branches)
  local controls = {}

  -- An InputGate control wraps a comparator object (passed into the comparator argument).
  controls.input = InputGate {
    button = "input",
    description = "Unit Input",
    comparator = objects.input
  }

  -- A GainBias control wraps any object with a Bias and Gain parameter.
  controls.count = GainBias {
    button = "count",
    description = "Count",
    branch = branches.start,
    gainbias = objects.start,
    range = objects.start,
    biasMap = Encoder.getMap("int[1,256]"),
    biasPrecision = 0
  }

  -- A Gate control wraps a comparator object (passed into the comparator parameter).
  controls.reset = Gate {
    button = "reset",
    description = "Reset Counter",
    branch = branches.reset,
    comparator = objects.reset,
    param = objects.counter:getParameter("Value"),
    readoutPrecision = 0
  }

  return controls, views
end

-- Don't forget to return the unit class definition
return Countdown
