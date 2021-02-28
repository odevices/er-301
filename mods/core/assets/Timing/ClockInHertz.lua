local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local ClockBase = require "core.Timing.ClockBase"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local ClockInHertz = Class {}
ClockInHertz:include(ClockBase)

function ClockInHertz:init(args)
  ClockBase.init(self, args)
end

function ClockInHertz:onLoadGraph(channelCount)
  local clock = self:addObject("clock", libcore.ClockInHertz())
  ClockBase.loadBaseGraph(self, channelCount, clock)
  local freq = self:addObject("freq", app.ParameterAdapter())
  tie(clock, "Frequency", freq, "Out")
  self:addMonoBranch("freq", freq, "In", freq, "Out")
end

local views = {
  expanded = {
    "sync",
    "freq",
    "mult",
    "div",
    "width"
  },
  collapsed = {}
}

function ClockInHertz:onLoadViews(objects, branches)
  local controls = ClockBase.loadBaseView(self, objects, branches)

  controls.freq = GainBias {
    button = "freq",
    description = "Clock Frequency",
    branch = branches.freq,
    gainbias = objects.freq,
    range = objects.freq,
    biasMap = Encoder.getMap("clockFreq"),
    biasUnits = app.unitHertz,
    scaling = app.octaveScaling,
    initialBias = 2
  }

  return controls, views
end

return ClockInHertz
