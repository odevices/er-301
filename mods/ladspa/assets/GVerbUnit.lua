local app = app
local libladspa = require "ladspa.libladspa"
local Class = require "Base.Class"
local Unit = require "Unit"
local Encoder = require "Encoder"
local GainBias = require "Unit.ViewControl.GainBias"

local GVerbUnit = Class {}
GVerbUnit:include(Unit)

function GVerbUnit:init(args)
  args.title = "GVerb"
  args.mnemonic = "GV"
  Unit.init(self, args)
end

function GVerbUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end

  local verb = self.objects.verb

  -- Create parameter adapters for all controls
  local roomsize = self:addObject("roomsize", app.ParameterAdapter())
  tie(verb, "Roomsize (m)", roomsize, "Out")
  self:addMonoBranch("roomsize", roomsize, "In", roomsize, "Out")

  local revtime = self:addObject("revtime", app.ParameterAdapter())
  tie(verb, "Reverb time (s)", revtime, "Out")
  self:addMonoBranch("revtime", revtime, "In", revtime, "Out")

  local damping = self:addObject("damping", app.ParameterAdapter())
  tie(verb, "Damping", damping, "Out")
  self:addMonoBranch("damping", damping, "In", damping, "Out")

  local inputbw = self:addObject("inputbw", app.ParameterAdapter())
  tie(verb, "Input bandwidth", inputbw, "Out")
  self:addMonoBranch("inputbw", inputbw, "In", inputbw, "Out")

  local dry = self:addObject("dry", app.ParameterAdapter())
  tie(verb, "Dry signal level (dB)", dry, "Out")
  self:addMonoBranch("dry", dry, "In", dry, "Out")

  local early = self:addObject("early", app.ParameterAdapter())
  tie(verb, "Early reflection level (dB)", early, "Out")
  self:addMonoBranch("early", early, "In", early, "Out")

  local tail = self:addObject("tail", app.ParameterAdapter())
  tie(verb, "Tail level (dB)", tail, "Out")
  self:addMonoBranch("tail", tail, "In", tail, "Out")
end

function GVerbUnit:loadMonoGraph()
  -- GVerb is always mono-to-stereo
  local verb = self:addObject("verb", libladspa.GVerb())

  connect(self, "In1", verb, "Input")

  connect(verb, "Left output", self, "Out1")
end

function GVerbUnit:loadStereoGraph()
  -- GVerb is always mono-to-stereo
  local verb = self:addObject("verb", libladspa.GVerb())
  local mixer = self:addObject("mixer", app.Sum())
  local inGain = self:addObject("inGain", app.ConstantGain())

  connect(self, "In1", mixer, "Left")
  connect(self, "In2", mixer, "Right")

  connect(mixer, "Out", inGain, "In")
  inGain:hardSet("Gain", 0.5)

  connect(inGain, "Out", verb, "Input")

  connect(verb, "Left output", self, "Out1")
  connect(verb, "Right output", self, "Out2")
end

local views = {
  collapsed = {},
  expanded = {
    "roomsize",
    "revtime",
    "damping",
    "inputbw",
    "dry",
    "early",
    "tail"
  }
}

function GVerbUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.roomsize = GainBias {
    button = "roomsize",
    description = "Roomsize (m)",
    branch = branches.roomsize,
    gainbias = objects.roomsize,
    range = objects.roomsize,
    biasMap = Encoder.getMap("[1,300]"),
    biasUnits = app.unitNone,
    initialBias = 75.75
  }

  controls.revtime = GainBias {
    button = "revtime",
    description = "Reverb time (s)",
    branch = branches.revtime,
    gainbias = objects.revtime,
    range = objects.revtime,
    biasMap = Encoder.getMap("[0.1,30]"),
    biasUnits = app.unitNone,
    initialBias = 7.575
  }

  controls.damping = GainBias {
    button = "damp",
    description = "Damping",
    branch = branches.damping,
    gainbias = objects.damping,
    range = objects.damping,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 0.5
  }

  controls.inputbw = GainBias {
    button = "inputbw",
    description = "Input bandwidth",
    branch = branches.inputbw,
    gainbias = objects.inputbw,
    range = objects.inputbw,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 0.75
  }

  controls.dry = GainBias {
    button = "dry",
    description = "Dry signal level (dB)",
    branch = branches.dry,
    gainbias = objects.dry,
    range = objects.dry,
    biasMap = Encoder.getMap("[-70,0]"),
    biasUnits = app.unitNone,
    initialBias = -70.0
  }

  controls.early = GainBias {
    button = "early",
    description = "Early reflection level (dB)",
    branch = branches.early,
    gainbias = objects.early,
    range = objects.early,
    biasMap = Encoder.getMap("[-70,0]"),
    biasUnits = app.unitNone,
    initialBias = 0.0
  }

  controls.tail = GainBias {
    button = "tail",
    description = "Tail level (dB)",
    branch = branches.tail,
    gainbias = objects.tail,
    range = objects.tail,
    biasMap = Encoder.getMap("[-70,0]"),
    biasUnits = app.unitNone,
    initialBias = -17.5
  }

  return controls, views
end

return GVerbUnit
