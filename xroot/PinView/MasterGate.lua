local Class = require "Base.Class"
local Base = require "PinView.MasterControl"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

local MasterGate = Class {}
MasterGate:include(Base)

function MasterGate:init(args)
  Base.init(self, args)
  self:setClassName("PinView.MasterGate")
  local name = args.name or app.logError("%s.init: name is missing.", self)
  local triggerGroup = app.TriggerGroup()
  triggerGroup:setLabel(name)
  self:setMainCursorController(triggerGroup)
  self:setControlGraphic(triggerGroup)
  self.triggerGroup = triggerGroup
  self:addSpotDescriptor{
    center = 0.5 * ply
  }
end

function MasterGate:setName(name)
  self.triggerGroup:setLabel(name)
end

function MasterGate:showName()
  self.triggerGroup:showLabel()
end

function MasterGate:hideName()
  self.triggerGroup:hideLabel()
end

function MasterGate:setNameCenter(x)
  self.fader:setLabelCenter(x)
end

function MasterGate:addFollower(object)
  self.triggerGroup:add(object)
end

function MasterGate:removeFollower(object)
  self.triggerGroup:remove(object)
end

function MasterGate:clear()
  self.triggerGroup:clear()
end

function MasterGate:count()
  return self.triggerGroup:size()
end

function MasterGate:spotPressed(spot, shifted)
  if shifted then return false end
  self.triggerGroup:onPressed()
  return true
end

function MasterGate:spotReleased(spot, shifted)
  if shifted then return false end
  self.triggerGroup:onReleased()
  return true
end

function MasterGate:serialize()
  local t = Base.serialize(self)
  if self.encoderState ~= Encoder.Coarse then
    t.encoderState = Encoder.serialize(self.encoderState)
  end
  return t
end

function MasterGate:deserialize(t)
  Base.deserialize(self, t)
  local encoderState = t.encoderState
  if encoderState then self.encoderState = Encoder.deserialize(encoderState) end
end

return MasterGate
