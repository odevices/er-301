local Class = require "Base.Class"
local Widget = require "Base.Widget"

local ShiftWidget = Class {}
ShiftWidget:include(Widget)

function ShiftWidget:init(parent)
  Widget.init(self)
  self:setClassName("ShiftWidget")
  self:setParentWidget(parent)
end

function ShiftWidget:show()
  self:grabFocus("encoder", "cancelReleased", "enterReleased", "mainReleased",
                 "mainPressed")
  self.parent.mainDisplay:showShiftGadget()
  self.parent:hideMainButtons()
end

function ShiftWidget:hide()
  self:releaseFocus("encoder", "cancelReleased", "enterReleased",
                    "mainReleased", "mainPressed")
  self.parent.mainDisplay:hideGadget()
  self.parent:showMainButtons()
end

function ShiftWidget:encoder(change, shifted)
  if self.parent.zooming then
    self.parent:encoderZoom(change, shifted)
  else
    self.parent.mainDisplay:encoderSliceShift(change, shifted, 2)
  end
  return true
end

function ShiftWidget:cancelReleased(shifted)
  self:hide()
  return true
end

function ShiftWidget:enterReleased()
  self.parent.mainDisplay:shiftSlices()
  self:hide()
  return true
end

function ShiftWidget:mainPressed(i, shifted)
  return true
end

function ShiftWidget:mainReleased(i, shifted)
  return true
end

return ShiftWidget
