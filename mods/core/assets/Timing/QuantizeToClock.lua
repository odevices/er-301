local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Gate = require "Unit.ViewControl.Gate"
local InputGate = require "Unit.ViewControl.InputGate"

local QuantizeToClock = Class {}
QuantizeToClock:include(Unit)

function QuantizeToClock:init(args)
  args.title = "Quantize to Clock"
  args.mnemonic = "QC"
  Unit.init(self, args)
end

function QuantizeToClock:onLoadGraph(channelCount)
  local quantizer = self:addObject("quantizer", libcore.QuantizeToClock())
  local clockEdge = self:addObject("clockEdge", app.Comparator())
  clockEdge:setGateMode()
  local inputEdge = self:addObject("inputEdge", app.Comparator())
  inputEdge:setGateMode()

  connect(self, "In1", inputEdge, "In")
  connect(inputEdge, "Out", quantizer, "In")
  connect(clockEdge, "Out", quantizer, "Clock")
  connect(quantizer, "Out", self, "Out1")

  if channelCount > 1 then connect(quantizer, "Out", self, "Out2") end

  self:addMonoBranch("clock", clockEdge, "In", clockEdge, "Out")
end

local views = {
  expanded = {
    "input",
    "clock"
  },
  collapsed = {}
}

function QuantizeToClock:onLoadViews(objects, branches)
  local controls = {}

  controls.input = InputGate {
    button = "input",
    description = "Unit Input",
    comparator = objects.inputEdge,
    readoutUnits = app.unitHertz
  }

  controls.clock = Gate {
    button = "clock",
    description = "Clock",
    branch = branches.clock,
    comparator = objects.clockEdge
  }

  return controls, views
end

return QuantizeToClock
