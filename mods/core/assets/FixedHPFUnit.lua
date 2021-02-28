local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local Encoder = require "Encoder"

local FixedHPFUnit = Class {}
FixedHPFUnit:include(Unit)

function FixedHPFUnit:init(args)
  args.title = "Fixed HPF"
  args.mnemonic = "HF"
  Unit.init(self, args)
end

function FixedHPFUnit:onLoadGraph(channelCount)
  local filter
  if channelCount == 2 then
    filter = self:addObject("filter", libcore.StereoFixedHPF())
    connect(self, "In1", filter, "Left In")
    connect(filter, "Left Out", self, "Out1")
    connect(self, "In2", filter, "Right In")
    connect(filter, "Right Out", self, "Out2")
  else
    -- Using a stereo filter here is actually cheaper!
    -- mono 80k ticks, stereo 36k ticks
    filter = self:addObject("filter", libcore.StereoFixedHPF())
    connect(self, "In1", filter, "Left In")
    connect(filter, "Left Out", self, "Out1")
  end
end

local views = {
  expanded = {
    "freq"
  },
  collapsed = {}
}

function FixedHPFUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.freq = Fader {
    button = "freq",
    description = "Cutoff Freq",
    param = objects.filter:getParameter("Cutoff"),
    map = Encoder.getMap("filterFreq"),
    units = app.unitHertz,
    scaling = app.octaveScaling
  }

  return controls, views
end

return FixedHPFUnit
