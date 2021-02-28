local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local FoldUnit = Class {}
FoldUnit:include(Unit)

function FoldUnit:init(args)
  args.title = "Fold"
  args.mnemonic = "F"
  Unit.init(self, args)
end

function FoldUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function FoldUnit:loadMonoGraph()
  local fold = self:addObject("fold", libcore.Fold())

  local upper = self:addObject("upper", app.GainBias())
  local upperRange = self:addObject("upperRange", app.MinMax())
  local lower = self:addObject("lower", app.GainBias())
  local lowerRange = self:addObject("lowerRange", app.MinMax())

  local threshold = self:addObject("threshold", app.GainBias())
  local thresholdRange = self:addObject("thresholdRange", app.MinMax())

  connect(threshold, "Out", thresholdRange, "In")
  connect(threshold, "Out", fold, "Threshold")
  connect(upper, "Out", upperRange, "In")
  connect(upper, "Out", fold, "Upper Gain")
  connect(lower, "Out", lowerRange, "In")
  connect(lower, "Out", fold, "Lower Gain")

  connect(self, "In1", fold, "In")
  connect(fold, "Out", self, "Out1")

  self:addMonoBranch("threshold", threshold, "In", threshold, "Out")
  self:addMonoBranch("upper", upper, "In", upper, "Out")
  self:addMonoBranch("lower", lower, "In", lower, "Out")
end

function FoldUnit:loadStereoGraph()
  local fold1 = self:addObject("fold1", libcore.Fold())
  local fold2 = self:addObject("fold2", libcore.Fold())

  local upper = self:addObject("upper", app.GainBias())
  local upperRange = self:addObject("upperRange", app.MinMax())
  local lower = self:addObject("lower", app.GainBias())
  local lowerRange = self:addObject("lowerRange", app.MinMax())

  local threshold = self:addObject("threshold", app.GainBias())
  local thresholdRange = self:addObject("thresholdRange", app.MinMax())

  connect(threshold, "Out", thresholdRange, "In")
  connect(threshold, "Out", fold1, "Threshold")
  connect(threshold, "Out", fold2, "Threshold")
  connect(upper, "Out", upperRange, "In")
  connect(upper, "Out", fold1, "Upper Gain")
  connect(upper, "Out", fold2, "Upper Gain")
  connect(lower, "Out", lowerRange, "In")
  connect(lower, "Out", fold1, "Lower Gain")
  connect(lower, "Out", fold2, "Lower Gain")

  connect(self, "In1", fold1, "In")
  connect(fold1, "Out", self, "Out1")
  connect(self, "In2", fold2, "In")
  connect(fold2, "Out", self, "Out2")

  self:addMonoBranch("threshold", threshold, "In", threshold, "Out")
  self:addMonoBranch("upper", upper, "In", upper, "Out")
  self:addMonoBranch("lower", lower, "In", lower, "Out")
end

local views = {
  expanded = {
    "threshold",
    "upper",
    "lower"
  },
  collapsed = {}
}

function FoldUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.threshold = GainBias {
    button = "thresh",
    branch = branches.threshold,
    description = "Threshold",
    gainbias = objects.threshold,
    range = objects.thresholdRange,
    biasMap = Encoder.getMap("[-1,1]"),
    biasUnits = app.unitNone,
    initialBias = 0.0,
    gainMap = Encoder.getMap("gain")
  }

  controls.upper = GainBias {
    button = "upper",
    branch = branches.upper,
    description = "Upper Gain",
    gainbias = objects.upper,
    range = objects.upperRange,
    biasMap = Encoder.getMap("[-5,5]"),
    biasUnits = app.unitNone,
    initialBias = 1.0,
    gainMap = Encoder.getMap("gain")
  }

  controls.lower = GainBias {
    button = "lower",
    branch = branches.lower,
    description = "Lower Gain",
    gainbias = objects.lower,
    range = objects.lowerRange,
    biasMap = Encoder.getMap("[-5,5]"),
    biasUnits = app.unitNone,
    initialBias = 1.0,
    gainMap = Encoder.getMap("gain")
  }

  return controls, views
end

return FoldUnit
