local app = app
local Class = require "Base.Class"
local Base = require "Unit.ViewControl.EncoderControl"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

-- Fader Class
local Fader = Class {
  type = "Fader",
  canEdit = false,
  canMove = true
}
Fader:include(Base)

function Fader:init(args)
  Base.init(self)
  self:setClassName("Unit.ViewControl.Fader")
  -- required arguments
  local button = args.button or
                     app.logError("%s.init: button is missing.", self)
  self:setInstanceName(button)
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  local param = args.param or app.logError("%s.init: param is missing.", self)
  -- optional arguments
  self:setDefaults(args)
  local map = args.map
  local units = args.units
  local precision = args.precision
  local scaling = args.scaling
  local monitor = args.monitor
  local graphic

  param:enableSerialization()

  if args.initial then
    param:hardSet(args.initial)
  end

  self.description = description
  graphic = app.Fader(0, 0, ply, 64)
  graphic:setParameter(param)
  graphic:setAttributes(units, map, scaling)
  graphic:setPrecision(precision)
  graphic:setLabel(button)
  self.fader = graphic
  self:setMainCursorController(graphic)
  self:setControlGraphic(graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }

  self.subGraphic = app.Graphic(0, 0, 128, 64)

  if monitor then
    self:setScopeTarget(monitor)
  end
end

function Fader:setDefaults(args)
  if args.map == nil then
    args.map = Encoder.getMap("default")
  end
  if args.units == nil then
    args.units = app.unitNone
  end
  if args.scaling == nil then
    args.scaling = app.linearScaling
  end
  if args.precision == nil then
    args.precision = 3
  end
  self.defaults = args
end

function Fader:getDefaults()
  return self.defaults
end

function Fader:getPinControl()
  local PinFader = require "PinView.Fader"
  local control = PinFader {
    delegate = self,
    name = self.fader:getLabel(),
    valueParam = self.fader:getValueParameter(),
    units = self.defaults.units,
    map = self.defaults.map,
    scaling = self.defaults.scaling,
    precision = self.defaults.precision
  }
  return control
end

function Fader:createPinMark()
  local Drawings = require "Drawings"
  local graphic = app.Drawing(-8, 0, app.SECTION_PLY, 64)
  graphic:add(Drawings.Control.Pin)
  self.controlGraphic:addChildOnce(graphic)
  self.pinMark = graphic
end

function Fader:setScopeTarget(monitor)
  if monitor then
    self.hasMonitor = true
    if monitor.channelCount == 1 then
      local outlet = monitor:getMonitoringOutput(1)
      self:setMonoScopeTarget(outlet)
    else
      local left = monitor:getMonitoringOutput(1)
      local right = monitor:getMonitoringOutput(2)
      self:setStereoScopeTarget(left, right)
    end
  else
    self:setMonoScopeTarget()
    self:setStereoScopeTarget()
    self.hasMonitor = false
  end
end

function Fader:onInsert()
  if not self.hasMonitor then
    self:setScopeTarget(self.parent)
  end
end

function Fader:getScope()
  if self.scope == nil then
    self.scope = app.MiniScope(0, 0, 128, 64)
    self.subGraphic:addChild(self.scope)
  end
  return self.scope
end

function Fader:setMonoScopeTarget(outlet)
  self.leftScopeTarget = outlet
  self.rightScopeTarget = nil
end

function Fader:setStereoScopeTarget(leftOutlet, rightOutlet)
  self.leftScopeTarget = leftOutlet
  self.rightScopeTarget = rightOutlet
end

function Fader:setMonoMeterTarget(outlet)
  self.fader:watchOutlets(outlet, nil)
end

function Fader:setStereoMeterTarget(left, right)
  self.fader:watchOutlets(left, right)
end

function Fader:setTextBelow(value, text)
  self.fader:setTextBelow(value, text)
end

function Fader:setTextAbove(value, text)
  self.fader:setTextAbove(value, text)
end

function Fader:onCursorEnter(spot)
  Base.onCursorEnter(self, spot)
  local left = self.leftScopeTarget
  local right = self.rightScopeTarget
  local scope = self:getScope()
  local Channels = require "Channels"
  if Channels.isRight() then
    scope:watchOutlet(right)
  else
    scope:watchOutlet(left)
  end
end

function Fader:zeroPressed()
  self.fader:zero()
  return true
end

function Fader:cancelReleased(shifted)
  self.fader:restore()
  return true
end

function Fader:encoder(change, shifted)
  self.fader:encoder(change, shifted, self.encoderState == Encoder.Fine)
  return true
end

function Fader:onFocused()
  self.fader:save()
end

return Fader
