local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local DeadbandFilter = Class {}
DeadbandFilter:include(Unit)

function DeadbandFilter:init(args)
  args.title = "Deadband Filter"
  args.mnemonic = "DF"
  Unit.init(self, args)
end

function DeadbandFilter:onLoadGraph(channelCount)
  local filterL = self:addObject("filterL", libcore.DeadbandFilter())
  connect(self, "In1", filterL, "In")
  connect(filterL, "Out", self, "Out1")

  if channelCount == 2 then
    local filterR = self:addObject("filterR", libcore.DeadbandFilter())
    connect(self, "In2", filterR, "In")
    connect(filterR, "Out", self, "Out2")
    tie(filterR, "Threshold", filterL, "Threshold")
  end

  local threshold = self:addObject("threshold", app.ParameterAdapter())
  tie(filterL, "Threshold", threshold, "Out")

  self:addMonoBranch("threshold", threshold, "In", threshold, "Out")
end

local views = {
  expanded = {
    "threshold"
  },
  collapsed = {}
}

function DeadbandFilter:onLoadViews(objects, branches)
  local controls = {}

  controls.threshold = GainBias {
    button = "thresh",
    description = "Threshold",
    branch = branches.threshold,
    gainbias = objects.threshold,
    range = objects.threshold,
    biasMap = Encoder.getMap("[-1,1]"),
    biasUnits = app.unitNone,
    initialBias = 0.1
  }

  return controls, views
end

return DeadbandFilter
