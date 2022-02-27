local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local OptionControl = require "Unit.MenuControl.OptionControl"
local GainBias = require "Unit.ViewControl.GainBias"
local Gate = require "Unit.ViewControl.Gate"
local InputGate = require "Unit.ViewControl.InputGate"
local Encoder = require "Encoder"

local CounterUnit = Class {}
CounterUnit:include(Unit)

function CounterUnit:init(args)
  args.title = "Counter"
  args.mnemonic = "C"
  Unit.init(self, args)
end

function CounterUnit:onLoadGraph(channelCount)
  local trig = self:addObject("trig", app.Comparator())
  trig:setTriggerMode()
  local reset = self:addObject("reset", app.Comparator())
  reset:setTriggerMode()
  local counter = self:addObject("counter", libcore.Counter())
  local start = self:addObject("start", app.ParameterAdapter())
  local finish = self:addObject("finish", app.ParameterAdapter())
  local step = self:addObject("step", app.ParameterAdapter())
  local gain = self:addObject("gain", app.ParameterAdapter())

  connect(self, "In1", trig, "In")
  connect(trig, "Out", counter, "In")
  connect(counter, "Out", self, "Out1")
  connect(reset, "Out", counter, "Reset")

  tie(counter, "Step Size", step, "Out")
  tie(counter, "Start", start, "Out")
  tie(counter, "Finish", finish, "Out")
  tie(counter, "Gain", gain, "Out")

  self:addMonoBranch("reset", reset, "In", reset, "Out")
  self:addMonoBranch("step", step, "In", step, "Out")
  self:addMonoBranch("start", start, "In", start, "Out")
  self:addMonoBranch("finish", finish, "In", finish, "Out")
  self:addMonoBranch("gain", gain, "In", gain, "Out")

  if channelCount > 1 then
    connect(counter, "Out", self, "Out2")
  end

  reset:simulateRisingEdge()
  reset:simulateFallingEdge()
end

local menu = {
  "wrap",
  "processingRate"
}

function CounterUnit:onShowMenu(objects, branches)
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

local views = {
  expanded = {
    "input",
    "reset",
    "start",
    "step",
    "finish",
    "gain"
  },
  collapsed = {}
}

function CounterUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.input = InputGate {
    button = "input",
    description = "Unit Input",
    comparator = objects.trig
  }

  controls.reset = Gate {
    button = "reset",
    description = "Reset To Start",
    branch = branches.reset,
    comparator = objects.reset,
    param = objects.counter:getParameter("Value"),
    readoutPrecision = 0
  }

  controls.start = GainBias {
    button = "start",
    description = "Start",
    branch = branches.start,
    gainbias = objects.start,
    range = objects.start,
    biasMap = Encoder.getMap("int[0,256]"),
    biasPrecision = 0,
    initialBias = 0
  }

  controls.step = GainBias {
    button = "step",
    description = "Step Size",
    branch = branches.step,
    gainbias = objects.step,
    range = objects.step,
    biasMap = Encoder.getMap("int[-32,32]"),
    biasPrecision = 0,
    initialBias = 1
  }

  controls.finish = GainBias {
    button = "finish",
    description = "Finish",
    branch = branches.finish,
    gainbias = objects.finish,
    range = objects.finish,
    biasMap = Encoder.getMap("int[1,256]"),
    biasPrecision = 0,
    initialBias = 16
  }

  controls.gain = GainBias {
    button = "gain",
    branch = branches.gain,
    description = "Output Gain",
    gainbias = objects.gain,
    range = objects.gain,
    biasMap = Encoder.getMap("gain"),
    biasUnits = app.unitNone,
    initialBias = 1.0
  }

  return controls, views
end

return CounterUnit
