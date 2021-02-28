local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Gate = require "Unit.ViewControl.Gate"
local OptionControl = require "Unit.ViewControl.OptionControl"
local Fader = require "Unit.ViewControl.Fader"

local TrackHoldUnit = Class {}
TrackHoldUnit:include(Unit)

function TrackHoldUnit:init(args)
  args.title = "Track & Hold"
  args.mnemonic = "TH"
  Unit.init(self, args)
end

function TrackHoldUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function TrackHoldUnit:loadMonoGraph()
  local holdL = self:addObject("holdL", libcore.TrackAndHold())
  local comparator = self:addObject("comparator", app.Comparator())
  comparator:setGateMode()

  connect(comparator, "Out", holdL, "Track")
  connect(self, "In1", holdL, "In")
  connect(holdL, "Out", self, "Out1")

  self:addMonoBranch("gate", comparator, "In", comparator, "Out")
end

function TrackHoldUnit:loadStereoGraph()
  local holdL = self:addObject("holdL", libcore.TrackAndHold())
  local holdR = self:addObject("holdR", libcore.TrackAndHold())
  local comparator = self:addObject("comparator", app.Comparator())
  comparator:setGateMode()

  connect(comparator, "Out", holdL, "Track")
  connect(comparator, "Out", holdR, "Track")

  connect(self, "In1", holdL, "In")
  connect(holdL, "Out", self, "Out1")

  connect(self, "In2", holdR, "In")
  connect(holdR, "Out", self, "Out2")

  self:addMonoBranch("gate", comparator, "In", comparator, "Out")

  tie(holdR, "Flavor", holdL, "Flavor")
end

local monoViews = {
  expanded = {
    "gate",
    "value",
    "type"
  },
  collapsed = {}
}

local stereoViews = {
  expanded = {
    "gate",
    "valueL",
    "valueR",
    "type"
  },
  collapsed = {}
}

function TrackHoldUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.gate = Gate {
    button = "gate",
    branch = branches.gate,
    description = "Gate",
    comparator = objects.comparator
  }

  controls.type = OptionControl {
    button = "o",
    description = "Type",
    option = objects.holdL:getOption("Flavor"),
    choices = {
      "high",
      "low",
      "minmax"
    },
    muteOnChange = true
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

return TrackHoldUnit
