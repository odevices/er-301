local app = app
-- This is how we get access to other Lua/C modules from this package.
-- require "<name of package>.<name of Lua/C module>"
local libFoo = require "tutorial.libFoo"
local Class = require "Base.Class"
local Unit = require "Unit"
local Fader = require "Unit.ViewControl.Fader"
local Gate = require "Unit.ViewControl.Gate"
local Encoder = require "Encoder"

local EuclidsCatsUnit = Class {}
EuclidsCatsUnit:include(Unit)

function EuclidsCatsUnit:init(args)
  args.title = "Euclid's Cats"
  args.mnemonic = "Eu"
  Unit.init(self, args)
end

function EuclidsCatsUnit:onLoadGraph(channelCount)
  local clockComparator = self:addObject("clockComparator", app.Comparator())
  clockComparator:setTriggerMode()
  local resetComparator = self:addObject("resetComparator", app.Comparator())
  resetComparator:setTriggerMode()
  local euclid = self:addObject("euclid", libFoo.EuclideanSequencer(32))

  connect(clockComparator, "Out", euclid, "Trigger")
  connect(resetComparator, "Out", euclid, "Reset")
  connect(euclid, "Out", self, "Out1")

  self:addMonoBranch("clock", clockComparator, "In", clockComparator, "Out")
  self:addMonoBranch("reset", resetComparator, "In", resetComparator, "Out")

  if channelCount > 1 then
    connect(euclid, "Out", self, "Out2")
  end
end

local views = {
  expanded = {
    "clock",
    "reset",
    "cats",
    "boxes"
  },
  collapsed = {}
}

function EuclidsCatsUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.clock = Gate {
    button = "clock",
    description = "Clock Input",
    branch = branches.clock,
    comparator = objects.clockComparator
  }

  controls.reset = Gate {
    button = "reset",
    description = "Reset Input",
    branch = branches.reset,
    comparator = objects.resetComparator
  }

  controls.cats = Fader {
    button = "cats",
    description = "Cat Count",
    param = objects.euclid:getParameter("Cats"),
    monitor = self,
    -- Using canned map (if the one you need is not available, you can create your own.)
    map = Encoder.getMap("int[0,32]"),
    precision = 0,
    initial = 0,
    units = app.unitNone
  }

  controls.boxes = Fader {
    button = "boxes",
    description = "Box Count",
    param = objects.euclid:getParameter("Boxes"),
    monitor = self,
    -- Using canned map (if the one you need is not available, you can create your own.)
    map = Encoder.getMap("int[1,32]"),
    precision = 0,
    initial = 1,
    units = app.unitNone
  }

  return controls, views
end

return EuclidsCatsUnit
