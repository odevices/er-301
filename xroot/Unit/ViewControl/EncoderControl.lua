local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"
local Encoder = require "Encoder"

local EncoderControl = Class {}
EncoderControl:include(ViewControl)

function EncoderControl:init(button)
  ViewControl.init(self, button)
  self:setClassName("Unit.ViewControl.EncoderControl")
  self.encoderState = Encoder.Coarse
end

function EncoderControl:serialize()
  local t = ViewControl.serialize(self)
  if self.encoderState ~= Encoder.Coarse then
    t.encoderState = Encoder.serialize(self.encoderState)
  end
  return t
end

function EncoderControl:deserialize(t)
  ViewControl.deserialize(self, t)
  local encoderState = t.encoderState
  if encoderState then
    self.encoderState = Encoder.deserialize(encoderState)
  end
end

function EncoderControl:dialPressed(shifted)
  if self.encoderState == Encoder.Coarse then
    self.encoderState = Encoder.Fine
  else
    self.encoderState = Encoder.Coarse
  end
  Encoder.set(self.encoderState)
  return true
end

return EncoderControl
