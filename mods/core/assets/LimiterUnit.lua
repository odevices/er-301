local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local OptionControl = require "Unit.ViewControl.OptionControl"
local Encoder = require "Encoder"

local LimiterUnit = Class {}
LimiterUnit:include(Unit)

function LimiterUnit:init(args)
  args.title = "Limiter"
  args.mnemonic = "Li"
  Unit.init(self, args)
end

function LimiterUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
  self.objects.inGain:hardSet("Gain", 1.0)
  self.objects.outGain:hardSet("Gain", 1.0)
end

function LimiterUnit:loadMonoGraph()
  local inGain = self:addObject("inGain", app.ConstantGain())
  local outGain = self:addObject("outGain", app.ConstantGain())
  local limiter = self:addObject("limiter", libcore.Limiter())

  connect(self, "In1", inGain, "In")
  connect(inGain, "Out", limiter, "In")
  connect(limiter, "Out", outGain, "In")
  connect(outGain, "Out", self, "Out1")
end

function LimiterUnit:loadStereoGraph()
  local inGain1 = self:addObject("inGain1", app.ConstantGain())
  local outGain1 = self:addObject("outGain1", app.ConstantGain())
  local limiter1 = self:addObject("limiter1", libcore.Limiter())

  connect(self, "In1", inGain1, "In")
  connect(inGain1, "Out", limiter1, "In")
  connect(limiter1, "Out", outGain1, "In")
  connect(outGain1, "Out", self, "Out1")

  local inGain2 = self:addObject("inGain2", app.ConstantGain())
  local outGain2 = self:addObject("outGain2", app.ConstantGain())
  local limiter2 = self:addObject("limiter2", libcore.Limiter())

  connect(self, "In2", inGain2, "In")
  connect(inGain2, "Out", limiter2, "In")
  connect(limiter2, "Out", outGain2, "In")
  connect(outGain2, "Out", self, "Out2")

  tie(inGain2, "Gain", inGain1, "Gain")
  self.objects.inGain = inGain1

  tie(outGain2, "Gain", outGain1, "Gain")
  self.objects.outGain = outGain1

  tie(limiter2, "Type", limiter1, "Type")
  self.objects.limiter = limiter1
end

local views = {
  expanded = {
    "pre",
    "type",
    "post"
  },
  collapsed = {}
}

function LimiterUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.pre = Fader {
    button = "pre",
    description = "Pre-Gain",
    param = objects.inGain:getParameter("Gain"),
    map = Encoder.getMap("decibel36"),
    units = app.unitDecibels
  }

  controls.post = Fader {
    button = "post",
    description = "Post-Gain",
    param = objects.outGain:getParameter("Gain"),
    monitor = self,
    map = Encoder.getMap("decibel36"),
    units = app.unitDecibels
  }

  controls.type = OptionControl {
    button = "o",
    description = "Type",
    option = objects.limiter:getOption("Type"),
    choices = {
      "inv sqrt",
      "cubic",
      "hard"
    },
    muteOnChange = true
  }

  if self.channelCount == 1 then
    local outlet = objects.inGain:getOutput("Out")
    controls.pre:setMonoMeterTarget(outlet)
  else
    local left = objects.inGain1:getOutput("Out")
    local right = objects.inGain2:getOutput("Out")
    controls.pre:setStereoMeterTarget(left, right)
  end

  if self.channelCount == 1 then
    local outlet = objects.outGain:getOutput("Out")
    controls.post:setMonoMeterTarget(outlet)
  else
    local left = objects.outGain1:getOutput("Out")
    local right = objects.outGain2:getOutput("Out")
    controls.post:setStereoMeterTarget(left, right)
  end

  return controls, views
end

return LimiterUnit
