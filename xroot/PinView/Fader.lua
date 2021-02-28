local Class = require "Base.Class"
local Base = require "PinView.Control"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

local Fader = Class {}
Fader:include(Base)

function Fader:init(args)
  Base.init(self, args)
  self:setClassName("PinView.Fader")
  local name = args.name or app.logError("%s.init: name is missing.", self)
  self:setInstanceName(name)
  local valueParam = args.valueParam or
                         app.logError("%s.init: valueParam is missing.", self)
  local range = args.range
  local targetValue = args.targetValue or valueParam:target()
  local leftOutlet = args.leftOutlet
  local rightOutlet = args.rightOutlet
  local units = args.units or app.unitNone
  local map = args.map or Encoder.getMap("default")
  local scaling = args.scaling or app.linearScaling
  local precision = args.precision
  if precision == nil then
    if map:getRounding() > 0.9 or units == app.unitDecibels then
      precision = 1
    else
      precision = 3
    end
  end

  local graphic
  graphic = app.Fader(0, 0, ply, 64)
  local param = app.Parameter(name, targetValue)
  self.targetParam = param
  self.valueParam = valueParam
  graphic:setTargetParameter(param)
  graphic:setValueParameter(valueParam)
  graphic:setControlParameter(param)
  graphic:setRangeObject(range)
  graphic:setAttributes(units, map, scaling)
  graphic:setPrecision(precision)
  graphic:setLabel(name)
  graphic:watchOutlets(leftOutlet, rightOutlet)
  if units == app.unitDecibels then
    graphic:setTextBelow(-59, "-inf dB")
    -- Tells the ParamSetMorph object to interpolate in decibels.
    valueParam:enableDecibelMorph()
  elseif scaling ~= app.linearScaling then
    valueParam:enableDecibelMorph()
  end
  self.fader = graphic
  self:setMainCursorController(graphic)
  self:setControlGraphic(graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }

  -- sub display
  self.subGraphic = app.Graphic(0, 0, 128, 64)
  graphic = app.TextPanel("Rename", 1)
  self.subGraphic:addChild(graphic)
  graphic = app.TextPanel("Unpin", 2)
  self.subGraphic:addChild(graphic)
  graphic = app.TextPanel("Reset Target", 3)
  self.subGraphic:addChild(graphic)
end

function Fader:rename(name)
  self:setInstanceName(name)
  self.fader:setLabel(name)
  self.targetParam:setName(name)
end

function Fader:resetTarget()
  self.targetParam:hardSet(self.valueParam:target())
end

function Fader:getEndValue()
  return self.targetParam:target()
end

function Fader:onCursorEnter(spot)
  Base.onCursorEnter(self, spot)
  Encoder.set(self.delegate.encoderState)
end

function Fader:encoder(change, shifted)
  if change > 0 or change < 0 and self.waitingForEncoder then
    self.waitingForEncoder = false
    self:callUp("notifyPinSets", "disengage")
  end
  local fine = self.delegate.gainEncoderState == Encoder.Fine
  self.fader:encoder(change, shifted, fine)
  return true
end

function Fader:dialPressed(shifted)
  return self.delegate:dialPressed(shifted)
end

function Fader:onFocus()
  self.waitingForEncoder = true
  self.fader:save()
end

function Fader:onDefocus()
end

function Fader:highlightTarget()
  self.fader:highlightTarget()
end

function Fader:highlightValue()
  self.fader:highlightValue()
end

function Fader:commitReleased()
  self.valueParam:softSet(self.targetParam:target())
  return true
end

function Fader:enterReleased()
  self.valueParam:softSet(self.targetParam:target())
  return true
end

function Fader:cancelReleased(shifted)
  if shifted then return false end
  self.fader:restore()
  return true
end

function Fader:zeroPressed()
  self.fader:zero()
  return true
end

function Fader:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    self:doRename()
  elseif i == 2 then
    local pinSet = self.parent -- save it
    pinSet:startViewModifications()
    pinSet:unpinControl(self.delegate)
    pinSet:endViewModifications()
  elseif i == 3 then
    self:resetTarget()
  end
  return true
end

return Fader
