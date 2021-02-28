local Class = require "Base.Class"
local Widget = require "Base.Widget"

local DivisionWidget = Class{}
DivisionWidget:include(Widget)

function DivisionWidget:init(parent)
  Widget.init(self)
  self:setClassName("DivisionWidget")
  self:setParentWidget(parent)
end

function DivisionWidget:show()
  self:grabFocus("encoder","cancelReleased","enterReleased","mainReleased","mainPressed")
  self.parent.mainDisplay:showDivisionGadget()
  self.parent:hideMainButtons()
end

function DivisionWidget:hide()
  self:releaseFocus("encoder","cancelReleased","enterReleased","mainReleased","mainPressed")
  self.parent.mainDisplay:hideGadget()
  self.parent:showMainButtons()
end

function DivisionWidget:encoder(change, shifted)
  if self.parent.zooming then
    self.parent:encoderZoom(change,shifted)
  else
    self.parent.mainDisplay:encoderGridDivision(change,shifted,1)
  end
  return true
end

function DivisionWidget:cancelReleased(shifted)
  self:hide()
  return true
end

function DivisionWidget:enterReleased()
  self.parent.mainDisplay:insertDivisionSlices()
  self:hide()
  return true
end

function DivisionWidget:mainPressed(i,shifted)
  return true
end

function DivisionWidget:mainReleased(i,shifted)
  return true
end

return DivisionWidget
