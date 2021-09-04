local app = app
local Class = require "Base.Class"
local Window = require "Base.Window"
local Env = require "Env"
local Utils = require "Utils"
local Message = require "Message"

local Decimal = Class {}
Decimal:include(Window)

function Decimal:init(args)
  Window.init(self, app.SixDigits())
  self:setClassName("Keyboard.Decimal")
  self.suppressQuickSave = true

  if args.message then self.mainGraphic:setMessage(args.message) end

  if args.integerOnly then
    self.integerOnly = true
    self.mainGraphic:setIntegerValue(args.initialValue)
  else
    self.mainGraphic:setValue(args.initialValue)
  end

  if args.mantissaFloor then
    self.mainGraphic:setMantissaFloor(args.mantissaFloor)
  end

  if args.mantissaCeiling then
    self.mainGraphic:setMantissaCeiling(args.mantissaCeiling)
  end

  self.commitMessage = args.commitMessage

  local graphic = app.TextPanel("Zero Left Side", 1)
  self:addSubGraphic(graphic)
  graphic = app.TextPanel("Zero Right Side", 2)
  self:addSubGraphic(graphic)
  if not self.integerOnly then
    graphic = app.TextPanel("Grab Decimal Point", 3)
    self:addSubGraphic(graphic)
  end

  self.mainGraphic:selectDigit(self.mainGraphic:getDecimalPoint())
end

function Decimal:mainReleased(i, shifted)
  if shifted then return true end
  self.mainGraphic:selectDigit(i)
  return true
end

function Decimal:subPressed(i, shifted)
  if shifted then return true end
  if i == 3 and not self.integerOnly then self.mainGraphic:selectDecimalPoint() end
  return true
end

function Decimal:subReleased(i, shifted)
  if i == 1 then
    self.mainGraphic:zeroLeft()
  elseif i == 2 then
    self.mainGraphic:zeroRight()
  elseif i == 3 and not self.integerOnly then
    self.mainGraphic:selectDigit()
  end
  return true
end

function Decimal:encoder(change, shifted)
  self.mainGraphic:encoder(change, shifted, 5)
  return true
end

function Decimal:cancelReleased(shifted)
  if shifted then return false end
  self:hide()
  return true
end

function Decimal:upReleased(shifted)
  if shifted then return false end
  self:hide()
  return true
end

function Decimal:zeroPressed()
  self.mainGraphic:zero()
  return true
end

function Decimal:homePressed()
  self:hide()
  return true
end

function Decimal:commitReleased()
  local g = self.mainGraphic
  if g:isInteger() or self.integerOnly then
    self:emitSignal("commit", g:getIntegerPart())
  else
    self:emitSignal("commit", g:getValue())
  end
  local SG = require "Overlay"
  SG.flashMainMessage(self.commitMessage or "Value committed.")
  return true
end

function Decimal:enterReleased()
  self:hide()
  local g = self.mainGraphic
  if g:isInteger() or self.integerOnly then
    self:emitSignal("done", g:getIntegerPart())
  else
    self:emitSignal("done", g:getValue())
  end
  return true
end

return Decimal
