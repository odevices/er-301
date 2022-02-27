local BaseControl = require "Base.Control"
local Class = require "Base.Class"

local Control = Class {}
Control:include(BaseControl)

function Control:init()
  BaseControl.init(self)
  self:setClassName("MondrianMenu.Control")
  self.enabled = true
end

function Control:enable()
  self.enabled = true
  if self.controlGraphic then
    self.controlGraphic:setForegroundColor(app.WHITE)
  end
end

function Control:disable()
  self.enabled = false
  if self.controlGraphic then
    self.controlGraphic:setForegroundColor(app.GRAY7)
  end
end

function Control:onPressed(i, shifted)
  return true
end

function Control:onRepeated(i, shifted)
  return true
end

function Control:onReleased(i, shifted)
  return true
end

function Control:onPressedWhenDisabled(i, shifted)
  return true
end

function Control:onReleasedWhenDisabled(i, shifted)
  return true
end

return Control
