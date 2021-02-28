local Class = require "Base.Class"
local Widget = require "Base.Widget"

local Control = Class {}
Control:include(Widget)

function Control:init()
  Widget.init(self)
  self:setClassName("Control")
end

function Control:getControlGraphic()
  return self.controlGraphic
end

function Control:setControlGraphic(graphic)
  self.controlGraphic = graphic
end

function Control:onRemove()
end

function Control:onInsert()
end

return Control
