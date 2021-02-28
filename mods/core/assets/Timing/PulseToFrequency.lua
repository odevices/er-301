local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local InputGate = require "Unit.ViewControl.InputGate"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local PulseToFrequency = Class {}
PulseToFrequency:include(Unit)

function PulseToFrequency:init(args)
  args.title = "Pulse to Hertz"
  args.mnemonic = "PH"
  Unit.init(self, args)
end

function PulseToFrequency:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function PulseToFrequency:loadMonoGraph()
  local tap = self:addObject("tapL", libcore.TapTempo())
  local freq = self:addObject("freqL", app.Constant())
  local comparator = self:addObject("edgeL", app.Comparator())

  connect(self, "In1", comparator, "In")
  connect(comparator, "Out", tap, "In")
  tie(freq, "Value", tap, "Derived Frequency")
  connect(freq, "Out", self, "Out1")

  local multiplier = self:addObject("multiplier", app.ParameterAdapter())
  local divider = self:addObject("divider", app.ParameterAdapter())
  tie(tap, "Multiplier", multiplier, "Out")
  tie(tap, "Divider", divider, "Out")

  self:addMonoBranch("multiplier", multiplier, "In", multiplier, "Out")
  self:addMonoBranch("divider", divider, "In", divider, "Out")
end

function PulseToFrequency:loadStereoGraph()
  local tapL = self:addObject("tapL", libcore.TapTempo())
  local freqL = self:addObject("freqL", app.Constant())
  local edgeL = self:addObject("edgeL", app.Comparator())

  local tapR = self:addObject("tapR", libcore.TapTempo())
  local freqR = self:addObject("freqR", app.Constant())
  local edgeR = self:addObject("edgeR", app.Comparator())

  local multiplier = self:addObject("multiplier", app.ParameterAdapter())
  local divider = self:addObject("divider", app.ParameterAdapter())
  tie(tapL, "Multiplier", multiplier, "Out")
  tie(tapL, "Divider", divider, "Out")
  tie(tapR, "Multiplier", multiplier, "Out")
  tie(tapR, "Divider", divider, "Out")

  connect(self, "In1", edgeL, "In")
  connect(edgeL, "Out", tapL, "In")
  tie(freqL, "Value", tapL, "Derived Frequency")
  connect(freqL, "Out", self, "Out1")

  connect(self, "In2", edgeR, "In")
  connect(edgeR, "Out", tapR, "In")
  tie(freqR, "Value", tapR, "Derived Frequency")
  connect(freqR, "Out", self, "Out2")

  self:addMonoBranch("multiplier", multiplier, "In", multiplier, "Out")
  self:addMonoBranch("divider", divider, "In", divider, "Out")
end

local views = {
  expanded = {
    "input",
    "mult",
    "div"
  },
  collapsed = {}
}

function PulseToFrequency:onLoadViews(objects, branches)
  local controls = {}

  controls.input = InputGate {
    button = "input",
    description = "Unit Input",
    comparator = objects.edgeL
  }

  controls.mult = GainBias {
    button = "mult",
    branch = branches.multiplier,
    description = "Clock Multiplier",
    gainbias = objects.multiplier,
    range = objects.multiplier,
    biasMap = Encoder.getMap("int[1,32]"),
    gainMap = Encoder.getMap("[-20,20]"),
    initialBias = 1,
    biasPrecision = 0
  }

  controls.div = GainBias {
    button = "div",
    branch = branches.divider,
    description = "Clock Divider",
    gainbias = objects.divider,
    range = objects.divider,
    biasMap = Encoder.getMap("int[1,32]"),
    gainMap = Encoder.getMap("[-20,20]"),
    initialBias = 1,
    biasPrecision = 0
  }
  return controls, views
end

return PulseToFrequency
