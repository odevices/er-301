local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Pitch = require "Unit.ViewControl.Pitch"
local Encoder = require "Encoder"

local LadderFilterUnit = Class {}
LadderFilterUnit:include(Unit)

function LadderFilterUnit:init(args)
  args.title = "Ladder LPF"
  args.mnemonic = "LF"
  Unit.init(self, args)
end

function LadderFilterUnit:onLoadGraph(channelCount)
  local filter
  if channelCount == 2 then
    filter = self:addObject("filter", libcore.StereoLadderFilter())
    connect(self, "In1", filter, "Left In")
    connect(filter, "Left Out", self, "Out1")
    connect(self, "In2", filter, "Right In")
    connect(filter, "Right Out", self, "Out2")
  else
    -- Using a stereo filter here is actually cheaper!
    -- mono 80k ticks, stereo 36k ticks
    filter = self:addObject("filter", libcore.StereoLadderFilter())
    connect(self, "In1", filter, "Left In")
    connect(filter, "Left Out", self, "Out1")
  end

  local tune = self:addObject("tune", app.ConstantOffset())
  local tuneRange = self:addObject("tuneRange", app.MinMax())

  local f0 = self:addObject("f0", app.GainBias())
  local f0Range = self:addObject("f0Range", app.MinMax())

  local res = self:addObject("res", app.GainBias())
  local resRange = self:addObject("resRange", app.MinMax())

  local clipper = self:addObject("clipper", libcore.Clipper())
  clipper:setMaximum(0.999)
  clipper:setMinimum(0)

  connect(tune, "Out", filter, "V/Oct")
  connect(tune, "Out", tuneRange, "In")

  connect(f0, "Out", filter, "Fundamental")
  connect(f0, "Out", f0Range, "In")

  connect(res, "Out", clipper, "In")
  connect(clipper, "Out", filter, "Resonance")
  connect(clipper, "Out", resRange, "In")

  self:addMonoBranch("tune", tune, "In", tune, "Out")
  self:addMonoBranch("Q", res, "In", res, "Out")
  self:addMonoBranch("f0", f0, "In", f0, "Out")
end

local views = {
  expanded = {
    "tune",
    "freq",
    "resonance"
  },
  collapsed = {}
}

function LadderFilterUnit:onLoadViews(objects, branches)
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
    branch = branches.f0,
    description = "Fundamental",
    gainbias = objects.f0,
    range = objects.f0Range,
    biasMap = Encoder.getMap("filterFreq"),
    biasUnits = app.unitHertz,
    initialBias = 440,
    gainMap = Encoder.getMap("freqGain"),
    scaling = app.octaveScaling
  }

  controls.resonance = GainBias {
    button = "Q",
    branch = branches.Q,
    description = "Resonance",
    gainbias = objects.res,
    range = objects.resRange,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 0.25,
    gainMap = Encoder.getMap("[-10,10]")
  }

  return controls, views
end

function LadderFilterUnit:deserialize(t)
  Unit.deserialize(self, t)
  if self:getPresetVersion(t) < 1 then
    -- handle legacy preset (<v0.2.12)
    local Serialization = require "Persist.Serialization"
    local f0 = Serialization.get("objects/osc/params/Fundamental", t)
    if f0 then
      app.logInfo("%s:deserialize:legacy preset detected:setting f0 bias to %s",
                  self, f0)
      self.objects.f0:deserialize("Bias", f0)
    end
  end
end

return LadderFilterUnit
