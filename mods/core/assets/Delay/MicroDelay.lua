local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local Encoder = require "Encoder"

local MicroDelay = Class {}
MicroDelay:include(Unit)

function MicroDelay:init(args)
  args.title = "uDelay"
  args.mnemonic = "uD"
  Unit.init(self, args)
end

function MicroDelay:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function MicroDelay:loadMonoGraph()
  local delayL = self:addObject("delayL", libcore.MicroDelay(0.1))
  connect(self, "In1", delayL, "In")
  connect(delayL, "Out", self, "Out1")
end

function MicroDelay:loadStereoGraph()
  local delayL = self:addObject("delayL", libcore.MicroDelay(0.1))
  local delayR = self:addObject("delayR", libcore.MicroDelay(0.1))
  connect(self, "In1", delayL, "In")
  connect(self, "In2", delayR, "In")
  connect(delayL, "Out", self, "Out1")
  connect(delayR, "Out", self, "Out2")
  tie(delayR, "Delay", delayL, "Delay")
end

local views = {
  expanded = {
    "delay"
  },
  collapsed = {}
}

function MicroDelay:onLoadViews(objects, branches)
  local controls = {}
  controls.delay = Fader {
    button = "delay",
    description = "Delay",
    param = objects.delayL:getParameter("Delay"),
    map = Encoder.getMap("[0,0.1]"),
    units = app.unitSecs
  }

  return controls, views
end

return MicroDelay
