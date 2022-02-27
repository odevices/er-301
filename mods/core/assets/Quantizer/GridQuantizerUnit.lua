local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

local GridQuantizerUnit = Class {}
GridQuantizerUnit:include(Unit)

function GridQuantizerUnit:init(args)
  args.title = "Grid Quantize"
  args.mnemonic = "GQ"
  args.version = 2
  Unit.init(self, args)
end

function GridQuantizerUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function GridQuantizerUnit:loadMonoGraph()
  local quantizer = self:addObject("quantizer", libcore.GridQuantizer())
  connect(self, "In1", quantizer, "In")
  connect(quantizer, "Out", self, "Out1")
  local levels = self:addObject("levels", app.ParameterAdapter())
  tie(quantizer, "Levels", levels, "Out")
  self:addMonoBranch("levels", levels, "In", levels, "Out")
end

function GridQuantizerUnit:loadStereoGraph()
  local quantizer1 = self:addObject("quantizer1", libcore.GridQuantizer())
  local quantizer2 = self:addObject("quantizer2", libcore.GridQuantizer())
  connect(self, "In1", quantizer1, "In")
  connect(quantizer1, "Out", self, "Out1")
  connect(self, "In2", quantizer2, "In")
  connect(quantizer2, "Out", self, "Out2")

  tie(quantizer2, "Pre-Scale", quantizer1, "Pre-Scale")
  tie(quantizer2, "Post-Scale", quantizer1, "Post-Scale")

  local levels = self:addObject("levels", app.ParameterAdapter())
  tie(quantizer1, "Levels", levels, "Out")
  tie(quantizer2, "Levels", levels, "Out")
  self:addMonoBranch("levels", levels, "In", levels, "Out")

  self.objects.quantizer = quantizer1
end

local views = {
  expanded = {
    "pre",
    "levels",
    "post"
  },
  collapsed = {}
}

function GridQuantizerUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.pre = Fader {
    button = "pre",
    description = "Pre-Scale",
    param = objects.quantizer:getParameter("Pre-Scale"),
    map = Encoder.getMap("[-10,10]"),
    units = app.unitNone
  }

  controls.levels = GainBias {
    button = "levels",
    branch = branches.levels,
    description = "# of Levels",
    gainbias = objects.levels,
    range = objects.levels,
    biasMap = Encoder.getMap("int[1,256]"),
    biasPrecision = 0,
    initialBias = 12
  }

  controls.post = Fader {
    button = "post",
    description = "Post-Scale",
    param = objects.quantizer:getParameter("Post-Scale"),
    monitor = self,
    map = Encoder.getMap("[-10,10]"),
    units = app.unitNone
  }

  return controls, views
end

function GridQuantizerUnit:deserialize(t)
  Unit.deserialize(self, t)
  if self:getPresetVersion(t) < 2 then
    -- handle legacy preset (<v0.3.23)
    local Serialization = require "Persist.Serialization"
    local n = Serialization.get("objects/quantizer/params/Levels", t)
    if n then
      app.logInfo(
          "%s:deserialize:legacy preset detected:setting levels bias to %s",
          self, n)
      self.objects.levels:hardSet("Bias", n)
    end
  end
end

return GridQuantizerUnit
