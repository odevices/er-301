local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"
local Encoder = require "Encoder"
local Env = require "Env"

local Zoomable = Class {}
Zoomable:include(ViewControl)

function Zoomable:init(name)
  ViewControl.init(self, name)
  self:setClassName("Unit.ViewControl.Zoomable")
  self.zooming = nil
  self.encoderState = Encoder.Horizontal
end

function Zoomable:serialize()
  local t = ViewControl.serialize(self)
  t.time = self.mainDisplay:getViewTimeInSeconds()
  t.gain = self.mainDisplay:getViewGainInDecibels()
  return t
end

function Zoomable:deserialize(t)
  ViewControl.deserialize(self, t)
  if t.time then self.mainDisplay:setViewTimeInSeconds(t.time) end
  if t.gain then self.mainDisplay:setViewGainInDecibels(t.gain) end
end

function Zoomable:onCursorLeave(spot)
  ViewControl.onCursorLeave(self, spot)
  self.zooming = nil
  self.mainDisplay:hideZoomGadget()
end

function Zoomable:dialPressed(shifted)
  self.zooming = "waiting"
  self:grabFocus("encoder")
  return true
end

function Zoomable:dialReleased(shifted)
  if self.zooming == "waiting" then
    if self.encoderState == Encoder.Vertical then
      self.encoderState = Encoder.Horizontal
    else
      self.encoderState = Encoder.Vertical
    end
    Encoder.set(self.encoderState)
  end
  self.zooming = nil
  self:releaseFocus("encoder")
  self.mainDisplay:hideZoomGadget()
  return true
end

local threshold = Env.EncoderThreshold.Default
function Zoomable:encoder(change, shifted)
  if self.zooming == "waiting" then
    self.zooming = "active"
    if self.encoderState == Encoder.Vertical then
      self.mainDisplay:showGainZoomGadget()
    else
      self.mainDisplay:showTimeZoomGadget()
    end
  end
  if self.zooming == "active" then
    self.mainDisplay:encoderZoom(change, shifted, threshold)
  end
  return true
end

return Zoomable
