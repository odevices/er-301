local Class = require "Base.Class"
local MondrianControl = require "MondrianMenu.Control"

local Control = Class {}
Control:include(MondrianControl)

function Control:init()
  MondrianControl.init(self)
  self:setClassName("Package.Menu.Control")
end

return Control
