local app = app
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local Encoder = require "Encoder"
local TestlibObjects = require "testlib.objects"

local TestUnit = Class {}
TestUnit:include(Unit)

function TestUnit:init(args)
  args.title = "Test Unit"
  args.mnemonic = "TU"
  Unit.init(self, args)
end

function TestUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function TestUnit:loadMonoGraph()
  local delayL = self:addObject("delayL", TestlibObjects.TestObject(0.1))
  connect(self, "In1", delayL, "In")
  connect(delayL, "Out", self, "Out1")
end

function TestUnit:loadStereoGraph()
  local delayL = self:addObject("delayL", TestlibObjects.TestObject(0.1))
  local delayR = self:addObject("delayR", TestlibObjects.TestObject(0.1))
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

function TestUnit:onLoadViews(objects, branches)
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

return TestUnit
