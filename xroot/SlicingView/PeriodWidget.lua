local Class = require "Base.Class"
local Widget = require "Base.Widget"

local PeriodWidget = Class{}
PeriodWidget:include(Widget)

function PeriodWidget:init(parent)
  Widget.init(self)
  self:setClassName("PeriodWidget")
  self:setParentWidget(parent)
end

function PeriodWidget:show()
  self:grabFocus("encoder","cancelReleased","enterReleased","mainReleased","mainPressed")
  self.parent.mainDisplay:showPeriodGadget()
  self.parent:hideMainButtons()
end

function PeriodWidget:hide()
  self:releaseFocus("encoder","cancelReleased","enterReleased","mainReleased","mainPressed")
  self.parent.mainDisplay:hideGadget()
  self.parent:showMainButtons()
end

function PeriodWidget:encoder(change, shifted)
  if self.parent.zooming then
    self.parent:encoderZoom(change,shifted)
  else
    self.parent.mainDisplay:encoderGridPeriod(change,shifted,10)
  end
  return true
end

function PeriodWidget:cancelReleased(shifted)
  self:hide()
  return true
end

function PeriodWidget:enterReleased()
  self.parent.mainDisplay:insertPeriodSlices()
  self:hide()
  return true
end

function PeriodWidget:mainPressed(i,shifted)
  return true
end

function PeriodWidget:mainReleased(i,shifted)
  return true
end

return PeriodWidget
