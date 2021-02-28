local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local InputGate = require "Unit.ViewControl.InputGate"
local Encoder = require "Encoder"

local ADSR = Class {}
ADSR:include(Unit)

function ADSR:init(args)
  args.title = "ADSR"
  args.mnemonic = "En"
  Unit.init(self, args)
end

function ADSR:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function ADSR:loadMonoGraph()
  local gate = self:addObject("gate", app.Comparator())
  gate:setGateMode()
  local adsr = self:addObject("adsr", libcore.ADSR())
  local attack = self:addObject("attack", app.GainBias())
  local decay = self:addObject("decay", app.GainBias())
  local sustain = self:addObject("sustain", app.GainBias())
  local release = self:addObject("release", app.GainBias())
  local attackRange = self:addObject("attackRange", app.MinMax())
  local decayRange = self:addObject("decayRange", app.MinMax())
  local sustainRange = self:addObject("sustainRange", app.MinMax())
  local releaseRange = self:addObject("releaseRange", app.MinMax())

  connect(self, "In1", gate, "In")
  connect(gate, "Out", adsr, "Gate")
  connect(adsr, "Out", self, "Out1")

  connect(attack, "Out", adsr, "Attack")
  connect(decay, "Out", adsr, "Decay")
  connect(sustain, "Out", adsr, "Sustain")
  connect(release, "Out", adsr, "Release")

  connect(attack, "Out", attackRange, "In")
  connect(decay, "Out", decayRange, "In")
  connect(sustain, "Out", sustainRange, "In")
  connect(release, "Out", releaseRange, "In")

  self:addMonoBranch("attack", attack, "In", attack, "Out")
  self:addMonoBranch("decay", decay, "In", decay, "Out")
  self:addMonoBranch("sustain", sustain, "In", sustain, "Out")
  self:addMonoBranch("release", release, "In", release, "Out")
end

function ADSR:loadStereoGraph()
  self:loadMonoGraph()
  connect(self.objects.adsr, "Out", self, "Out2")
end

local views = {
  expanded = {
    "input",
    "attack",
    "decay",
    "sustain",
    "release"
  },
  collapsed = {}
}

function ADSR:onLoadViews(objects, branches)
  local controls = {}

  controls.input = InputGate {
    button = "input",
    description = "Unit Input",
    comparator = objects.gate
  }

  controls.attack = GainBias {
    button = "A",
    branch = branches.attack,
    description = "Attack",
    gainbias = objects.attack,
    range = objects.attackRange,
    biasMap = Encoder.getMap("ADSR"),
    biasUnits = app.unitSecs,
    initialBias = 0.050
  }

  controls.decay = GainBias {
    button = "D",
    branch = branches.decay,
    description = "Decay",
    gainbias = objects.decay,
    range = objects.decayRange,
    biasMap = Encoder.getMap("ADSR"),
    biasUnits = app.unitSecs,
    initialBias = 0.050
  }

  controls.sustain = GainBias {
    button = "S",
    branch = branches.sustain,
    description = "Sustain",
    gainbias = objects.sustain,
    range = objects.sustainRange,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 1
  }

  controls.release = GainBias {
    button = "R",
    branch = branches.release,
    description = "Release",
    gainbias = objects.release,
    range = objects.releaseRange,
    biasMap = Encoder.getMap("ADSR"),
    biasUnits = app.unitSecs,
    initialBias = 0.100
  }

  return controls, views
end

return ADSR
