local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local PitchCircle = require "core.Quantizer.PitchCircle"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

local ScaleQuantizerUnit = Class {}
ScaleQuantizerUnit:include(Unit)

function ScaleQuantizerUnit:init(args)
  args.title = "Scale Quantize"
  args.mnemonic = "SQ"
  Unit.init(self, args)
end

function ScaleQuantizerUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function ScaleQuantizerUnit:loadMonoGraph()
  local quantizer = self:addObject("quantizer", libcore.ScaleQuantizer())
  connect(self, "In1", quantizer, "In")
  connect(quantizer, "Out", self, "Out1")

  local pre = self:addObject("pre", app.ParameterAdapter())
  tie(quantizer, "Pre-Transpose", pre, "Out")

  self:addMonoBranch("pre", pre, "In", pre, "Out")

  local post = self:addObject("post", app.ParameterAdapter())
  tie(quantizer, "Post-Transpose", post, "Out")

  self:addMonoBranch("post", post, "In", post, "Out")
end

function ScaleQuantizerUnit:loadStereoGraph()
  local quantizer1 = self:addObject("quantizer1", libcore.ScaleQuantizer())
  local quantizer2 = self:addObject("quantizer2", libcore.ScaleQuantizer())
  connect(self, "In1", quantizer1, "In")
  connect(quantizer1, "Out", self, "Out1")
  connect(self, "In2", quantizer2, "In")
  connect(quantizer2, "Out", self, "Out2")

  local pre = self:addObject("pre", app.ParameterAdapter())
  tie(quantizer1, "Pre-Transpose", pre, "Out")
  tie(quantizer2, "Pre-Transpose", pre, "Out")

  self:addMonoBranch("pre", pre, "In", pre, "Out")

  local post = self:addObject("post", app.ParameterAdapter())
  tie(quantizer1, "Post-Transpose", post, "Out")
  tie(quantizer2, "Post-Transpose", post, "Out")

  self:addMonoBranch("post", post, "In", post, "Out")

  self.objects.quantizer = quantizer1
end

local views = {
  expanded = {
    "pre",
    "scale",
    "post"
  },
  collapsed = {}
}

function ScaleQuantizerUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.pre = GainBias {
    button = "pre",
    description = "Pre-Transpose",
    branch = branches.pre,
    gainbias = objects.pre,
    range = objects.pre,
    biasMap = Encoder.getMap("cents"),
    biasUnits = app.unitCents
  }

  controls.post = GainBias {
    button = "post",
    description = "Post-Transpose",
    branch = branches.post,
    gainbias = objects.post,
    range = objects.post,
    biasMap = Encoder.getMap("cents"),
    biasUnits = app.unitCents
  }

  controls.scale = PitchCircle {
    name = "scale",
    width = 2 * ply,
    quantizer = objects.quantizer
  }
  return controls, views
end

return ScaleQuantizerUnit
