local Class = require "Base.Class"
local Widget = require "Base.Widget"
local Busy = require "Busy"

local OnsetWidget = Class{}
OnsetWidget:include(Widget)

function OnsetWidget:init(parent)
  Widget.init(self)
  self:setClassName("OnsetWidget")
  self:setParentWidget(parent)
end

function OnsetWidget:show()
  self:grabFocus("encoder","cancelReleased","enterReleased","mainReleased","mainPressed")
  Busy.start("Calculating candidate onsets...")
  self.parent.mainDisplay:showOnsetGadget()
  Busy.stop()
  self.parent:hideMainButtons()
end

function OnsetWidget:hide()
  self:releaseFocus("encoder","cancelReleased","enterReleased","mainReleased","mainPressed")
  self.parent.mainDisplay:hideGadget()
  self.parent:showMainButtons()
end

function OnsetWidget:encoder(change, shifted)
  if self.parent.zooming then
    self.parent:encoderZoom(change,shifted)
  else
    self.parent.mainDisplay:encoderOnsetThreshold(change,shifted,0.1)
  end
  return true
end

function OnsetWidget:cancelReleased(shifted)
  self:hide()
  return true
end

function OnsetWidget:enterReleased()
  self.parent.mainDisplay:insertOnsetSlices()
  self:hide()
  return true
end

function OnsetWidget:mainPressed(i,shifted)
  return true
end

function OnsetWidget:mainReleased(i,shifted)
  return true
end

return OnsetWidget
