local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Encoder = require "Encoder"
local GainBias = require "Unit.ViewControl.GainBias"

local FreeverbUnit = Class {}
FreeverbUnit:include(Unit)

function FreeverbUnit:init(args)
  args.title = "Freeverb"
  args.mnemonic = "FV"
  Unit.init(self, args)
end

function FreeverbUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end

  local verb = self.objects.verb

  local size = self:addObject("size", app.ParameterAdapter())
  tie(verb, "Size", size, "Out")
  self:addMonoBranch("size", size, "In", size, "Out")

  local damp = self:addObject("damp", app.ParameterAdapter())
  tie(verb, "Damp", damp, "Out")
  self:addMonoBranch("damp", damp, "In", damp, "Out")

  local width = self:addObject("width", app.ParameterAdapter())
  tie(verb, "Width", width, "Out")
  self:addMonoBranch("width", width, "In", width, "Out")
end

function FreeverbUnit:loadMonoGraph()
  local verb = self:addObject("verb", libcore.Freeverb())
  local xfade = self:addObject("xfade", app.CrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  connect(self, "In1", verb, "Left In")
  connect(self, "In1", verb, "Right In")
  connect(verb, "Left Out", xfade, "A")
  connect(self, "In1", xfade, "B")
  connect(xfade, "Out", self, "Out1")

  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  self:addMonoBranch("wet", fader, "In", fader, "Out")
end

function FreeverbUnit:loadStereoGraph()
  local verb = self:addObject("verb", libcore.Freeverb())
  local xfade = self:addObject("xfade", app.StereoCrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  connect(self, "In1", verb, "Left In")
  connect(self, "In2", verb, "Right In")
  connect(verb, "Left Out", xfade, "Left A")
  connect(verb, "Right Out", xfade, "Right A")
  connect(self, "In1", xfade, "Left B")
  connect(self, "In2", xfade, "Right B")
  connect(xfade, "Left Out", self, "Out1")
  connect(xfade, "Right Out", self, "Out2")

  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  self:addMonoBranch("wet", fader, "In", fader, "Out")
end

local views = {
  collapsed = {},
  expanded = {
    "size",
    "damp",
    "width",
    "wet"
  }
}

function FreeverbUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.size = GainBias {
    button = "size",
    description = "Room Size",
    branch = branches.size,
    gainbias = objects.size,
    range = objects.size,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 0.5
  }

  controls.damp = GainBias {
    button = "damp",
    description = "Damping",
    branch = branches.damp,
    gainbias = objects.damp,
    range = objects.damp,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 0.5
  }

  controls.width = GainBias {
    button = "width",
    description = "Width",
    branch = branches.width,
    gainbias = objects.width,
    range = objects.width,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 1.0
  }

  controls.wet = GainBias {
    button = "wet",
    description = "Wet/Dry Amount",
    branch = branches.wet,
    gainbias = objects.fader,
    range = objects.faderRange,
    biasMap = Encoder.getMap("unit"),
    initialBias = 0.5
  }

  return controls, views
end

return FreeverbUnit
