local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Gate = require "Unit.ViewControl.Gate"
local Fader = require "Unit.ViewControl.Fader"

local SampleHoldUnit = Class {}
SampleHoldUnit:include(Unit)

function SampleHoldUnit:init(args)
  args.title = "Sample & Hold"
  args.mnemonic = "SH"
  Unit.init(self, args)
end

function SampleHoldUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function SampleHoldUnit:loadMonoGraph()
  local holdL = self:addObject("holdL", libcore.TrackAndHold())
  local comparator = self:addObject("comparator", app.Comparator())
  comparator:setTriggerMode()

  connect(comparator, "Out", holdL, "Track")
  connect(self, "In1", holdL, "In")
  connect(holdL, "Out", self, "Out1")

  self:addMonoBranch("trig", comparator, "In", comparator, "Out")
end

function SampleHoldUnit:loadStereoGraph()
  local holdL = self:addObject("holdL", libcore.TrackAndHold())
  local holdR = self:addObject("holdR", libcore.TrackAndHold())
  local comparator = self:addObject("comparator", app.Comparator())
  comparator:setTriggerMode()

  connect(comparator, "Out", holdL, "Track")
  connect(comparator, "Out", holdR, "Track")

  connect(self, "In1", holdL, "In")
  connect(holdL, "Out", self, "Out1")

  connect(self, "In2", holdR, "In")
  connect(holdR, "Out", self, "Out2")

  self:addMonoBranch("trig", comparator, "In", comparator, "Out")
end

local monoViews = {
  expanded = {
    "trigger",
    "value"
  },
  collapsed = {}
}

local stereoViews = {
  expanded = {
    "trigger",
    "valueL",
    "valueR"
  },
  collapsed = {}
}

function SampleHoldUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.trigger = Gate {
    button = "trig",
    branch = branches.trig,
    description = "Trigger",
    comparator = objects.comparator
  }

  if self.channelCount > 1 then
    controls.valueL = Fader {
      button = "left",
      description = "Left Value",
      param = objects.holdL:getParameter("Value")
    }

    controls.valueR = Fader {
      button = "right",
      description = "Right Value",
      param = objects.holdR:getParameter("Value")
    }
    return controls, stereoViews
  else
    controls.value = Fader {
      button = "value",
      description = "Held Value",
      param = objects.holdL:getParameter("Value")
    }
    return controls, monoViews
  end
end

return SampleHoldUnit
