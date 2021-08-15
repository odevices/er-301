local libMinimal = require "minimal.libMinimal"
local Class = require "Base.Class"
local Unit = require "Unit"
-- This is how we get access to other Lua modules from this package.
-- require "<name of package>.<name of module>"
local MinimalViewControl = require "minimal.MinimalViewControl"

local MinimalUnit = Class {}
MinimalUnit:include(Unit)

function MinimalUnit:init(args)
  args.title = "Minimal Unit"
  args.mnemonic = "MU"
  Unit.init(self, args)
end

function MinimalUnit:onLoadGraph(channelCount)
  local obj1 = self:addObject("obj1", libMinimal.MinimalObject())
  connect(self, "In1", obj1, "In")
  connect(obj1, "Out", self, "Out1")

  if channelCount > 1 then
    local obj2 = self:addObject("obj2", libMinimal.MinimalObject())
    connect(self, "In2", obj2, "In")
    connect(obj2, "Out", self, "Out2")
  end
end

local views = {
  expanded = {
    "circle"
  },
  collapsed = {}
}

function MinimalUnit:onLoadViews(objects, branches)
  local controls = {}

  -- This new control is the custom ViewControl that we implemented in MinimalViewControl.lua.
  controls.circle = MinimalViewControl {
    -- Requires no args in this (minimal) case.
  }

  return controls, views
end

return MinimalUnit
