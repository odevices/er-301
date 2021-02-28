local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local Velvet = Class {}
Velvet:include(Unit)

function Velvet:init(args)
  args.title = "Velvet Noise"
  args.mnemonic = "VN"
  Unit.init(self, args)
end

function Velvet:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end

  local rate = self:addObject("rate", app.ParameterAdapter())
  tie(self.objects.noise1, "Rate", rate, "Out")

  self:addMonoBranch("rate", rate, "In", rate, "Out")
end

function Velvet:loadMonoGraph()
  local noise1 = self:addObject("noise1", libcore.VelvetNoise())
  connect(noise1, "Out", self, "Out1")
end

function Velvet:loadStereoGraph()
  local noise1 = self:addObject("noise1", libcore.VelvetNoise())
  local noise2 = self:addObject("noise2", libcore.VelvetNoise())
  connect(noise1, "Out", self, "Out1")
  connect(noise2, "Out", self, "Out2")
  tie(noise2, "Rate", noise1, "Rate")
end

local views = {
  expanded = {
    "rate"
  },
  collapsed = {}
}

function Velvet:onLoadViews(objects, branches)
  local controls = {}

  controls.rate = GainBias {
    button = "rate",
    description = "Impulse Rate",
    branch = branches.rate,
    gainbias = objects.rate,
    range = objects.rate,
    biasMap = Encoder.getMap("oscFreq"),
    biasUnits = app.unitHertz,
    initialBias = 880,
    gainMap = Encoder.getMap("freqGain"),
    scaling = app.octaveScaling
  }

  return controls, views
end

return Velvet
