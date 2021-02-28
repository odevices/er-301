local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local ClockBase = require "core.Timing.ClockBase"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local ClockInSeconds = Class {}
ClockInSeconds:include(ClockBase)

function ClockInSeconds:init(args)
  ClockBase.init(self, args)
end

function ClockInSeconds:onLoadGraph(channelCount)
  local clock = self:addObject("clock", libcore.ClockInSeconds())
  ClockBase.loadBaseGraph(self, channelCount, clock)
  local period = self:addObject("period", app.ParameterAdapter())
  tie(clock, "Period", period, "Out")
  self:addMonoBranch("period", period, "In", period, "Out")
end

local views = {
  expanded = {
    "sync",
    "period",
    "mult",
    "div",
    "width"
  },
  collapsed = {}
}

function ClockInSeconds:onLoadViews(objects, branches)
  local controls = ClockBase.loadBaseView(self, objects, branches)

  controls.period = GainBias {
    button = "period",
    description = "Clock Period",
    branch = branches.period,
    gainbias = objects.period,
    range = objects.period,
    biasMap = Encoder.getMap("[0,10]"),
    biasUnits = app.unitSecs,
    initialBias = 0.5
  }

  return controls, views
end

return ClockInSeconds
