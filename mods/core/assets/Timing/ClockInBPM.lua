local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local ClockBase = require "core.Timing.ClockBase"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local ClockInBPM = Class {}
ClockInBPM:include(ClockBase)

function ClockInBPM:init(args)
  ClockBase.init(self, args)
end

function ClockInBPM:onLoadGraph(channelCount)
  local clock = self:addObject("clock", libcore.ClockInBPM())
  ClockBase.loadBaseGraph(self, channelCount, clock)
  local tempo = self:addObject("tempo", app.ParameterAdapter())
  tie(clock, "Tempo", tempo, "Out")
  self:addMonoBranch("tempo", tempo, "In", tempo, "Out")
end

local views = {
  expanded = {
    "sync",
    "tempo",
    "mult",
    "div",
    "width"
  },
  collapsed = {}
}

function ClockInBPM:onLoadViews(objects, branches)
  local controls = ClockBase.loadBaseView(self, objects, branches)

  controls.tempo = GainBias {
    button = "bpm",
    description = "Clock Tempo",
    branch = branches.tempo,
    gainbias = objects.tempo,
    range = objects.tempo,
    biasMap = Encoder.getMap("tempo"),
    biasUnits = app.unitNone,
    initialBias = 120
  }

  return controls, views
end

return ClockInBPM
