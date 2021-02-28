local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local BumpMap = Class {}
BumpMap:include(Unit)

function BumpMap:init(args)
  args.title = "Bump Scanner"
  args.mnemonic = "BS"
  Unit.init(self, args)
end

function BumpMap:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function BumpMap:loadMonoGraph()
  local bump = self:addObject("bump1", libcore.BumpMap())
  local center = self:addObject("center", app.ParameterAdapter())
  local width = self:addObject("width", app.ParameterAdapter())
  local height = self:addObject("height", app.ParameterAdapter())
  local fade = self:addObject("fade", app.ParameterAdapter())

  connect(self, "In1", bump, "In")
  connect(bump, "Out", self, "Out1")

  tie(bump, "Center", center, "Out")
  tie(bump, "Width", width, "Out")
  tie(bump, "Height", height, "Out")
  tie(bump, "Fade", fade, "Out")

  self:addMonoBranch("center", center, "In", center, "Out")
  self:addMonoBranch("width", width, "In", width, "Out")
  self:addMonoBranch("height", height, "In", height, "Out")
  self:addMonoBranch("fade", fade, "In", fade, "Out")
end

function BumpMap:loadStereoGraph()
  local bump1 = self:addObject("bump1", libcore.BumpMap())
  local bump2 = self:addObject("bump2", libcore.BumpMap())
  local center = self:addObject("center", app.ParameterAdapter())
  local width = self:addObject("width", app.ParameterAdapter())
  local height = self:addObject("height", app.ParameterAdapter())
  local fade = self:addObject("fade", app.ParameterAdapter())

  connect(self, "In1", bump1, "In")
  connect(self, "In2", bump2, "In")
  connect(bump1, "Out", self, "Out1")
  connect(bump2, "Out", self, "Out2")

  tie(bump1, "Center", center, "Out")
  tie(bump1, "Width", width, "Out")
  tie(bump1, "Height", height, "Out")
  tie(bump1, "Fade", fade, "Out")
  tie(bump2, "Center", center, "Out")
  tie(bump2, "Width", width, "Out")
  tie(bump2, "Height", height, "Out")
  tie(bump2, "Fade", fade, "Out")

  self:addMonoBranch("center", center, "In", center, "Out")
  self:addMonoBranch("width", width, "In", width, "Out")
  self:addMonoBranch("height", height, "In", height, "Out")
  self:addMonoBranch("fade", fade, "In", fade, "Out")
end

local views = {
  expanded = {
    "center",
    "width",
    "height",
    "fade"
  },
  collapsed = {}
}

function BumpMap:onLoadViews(objects, branches)
  local controls = {}

  controls.center = GainBias {
    button = "center",
    branch = branches.center,
    description = "Center",
    gainbias = objects.center,
    range = objects.center,
    biasMap = Encoder.getMap("[-1,1]"),
    biasUnits = app.unitNone,
    initialBias = 0.0,
    gainMap = Encoder.getMap("gain")
  }

  controls.width = GainBias {
    button = "width",
    branch = branches.width,
    description = "Width",
    gainbias = objects.width,
    range = objects.width,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 0.5,
    gainMap = Encoder.getMap("gain")
  }

  controls.height = GainBias {
    button = "height",
    branch = branches.height,
    description = "Height",
    gainbias = objects.height,
    range = objects.height,
    biasMap = Encoder.getMap("[-1,1]"),
    biasUnits = app.unitNone,
    initialBias = 0.5,
    gainMap = Encoder.getMap("gain")
  }

  controls.fade = GainBias {
    button = "fade",
    branch = branches.fade,
    description = "Fade",
    gainbias = objects.fade,
    range = objects.fade,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 0.25,
    gainMap = Encoder.getMap("gain")
  }
  return controls, views
end

return BumpMap
