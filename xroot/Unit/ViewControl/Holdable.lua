local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"

local Holdable = Class {}
Holdable:include(ViewControl)

function Holdable:init(name)
  ViewControl.init(self, name)
  self:setClassName("Unit.ViewControl.Holdable")
end

function Holdable:serialize()
  local t = ViewControl.serialize(self)

  return t
end

function Holdable:deserialize(t)
  ViewControl.deserialize(self, t)
end

return Holdable
