local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local OptionControl = require "Unit.ViewControl.OptionControl"
local Encoder = require "Encoder"

local SlewLimiter = Class {}
SlewLimiter:include(Unit)

function SlewLimiter:init(args)
  args.title = "Slew Limiter"
  args.mnemonic = "SL"
  Unit.init(self, args)
end

function SlewLimiter:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function SlewLimiter:loadMonoGraph()
  local slew = self:addObject("slew1", libcore.SlewLimiter())
  local time = self:addObject("time", app.ParameterAdapter())

  connect(self, "In1", slew, "In")
  connect(slew, "Out", self, "Out1")

  tie(slew, "Time", time, "Out")

  self:addMonoBranch("time", time, "In", time, "Out")
end

function SlewLimiter:loadStereoGraph()
  local slew1 = self:addObject("slew1", libcore.SlewLimiter())
  local slew2 = self:addObject("slew2", libcore.SlewLimiter())
  local time = self:addObject("time", app.ParameterAdapter())

  connect(self, "In1", slew1, "In")
  connect(self, "In2", slew2, "In")
  connect(slew1, "Out", self, "Out1")
  connect(slew2, "Out", self, "Out2")

  tie(slew1, "Time", time, "Out")
  tie(slew2, "Time", time, "Out")
  tie(slew2, "Direction", slew1, "Direction")

  self:addMonoBranch("time", time, "In", time, "Out")
end

local views = {
  expanded = {
    "time",
    "dir"
  },
  collapsed = {}
}

function SlewLimiter:onLoadViews(objects, branches)
  local controls = {}

  controls.time = GainBias {
    button = "time",
    branch = branches.time,
    description = "Slew Time",
    gainbias = objects.time,
    range = objects.time,
    biasMap = Encoder.getMap("slewTimes"),
    biasUnits = app.unitSecs,
    initialBias = 1.0,
    scaling = app.octaveScaling,
    gainMap = Encoder.getMap("gain")
  }

  controls.dir = OptionControl {
    button = "o",
    description = "Mode",
    option = objects.slew1:getOption("Direction"),
    choices = {
      "up",
      "both",
      "down"
    }
  }

  return controls, views
end

return SlewLimiter
