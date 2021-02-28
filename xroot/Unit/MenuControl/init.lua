local Class = require "Base.Class"
local MondrianControl = require "MondrianMenu.Control"

local MenuControl = Class {}
MenuControl:include(MondrianControl)

function MenuControl:init()
  MondrianControl.init(self)
  self:setClassName("Unit.MenuControl")
end

return MenuControl
