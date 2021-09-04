local app = app
local Class = require "Base.Class"
local EncoderControl = require "Unit.ViewControl.EncoderControl"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

local line1 = app.GRID5_LINE1
-- local line2 = app.GRID5_LINE2
-- local line3 = app.GRID5_LINE3
local line4 = app.GRID5_LINE4
local center1 = app.GRID5_CENTER1
-- local center2 = app.GRID5_CENTER2
local center3 = app.GRID5_CENTER3
local center4 = app.GRID5_CENTER4
local col1 = app.BUTTON1_CENTER
local col2 = app.BUTTON2_CENTER
local col3 = app.BUTTON3_CENTER

-- shared instructions
local instructions = app.DrawingInstructions()
-- threshold
instructions:box(col2 - 13, center3 - 8, 26, 16)
instructions:startPolyline(col2 - 8, center3 - 4, 0)
instructions:vertex(col2, center3 - 4)
instructions:vertex(col2, center3 + 4)
instructions:endPolyline(col2 + 8, center3 + 4)
instructions:color(app.GRAY3)
instructions:hline(col2 - 9, col2 + 9, center3)
instructions:color(app.WHITE)
-- or
instructions:circle(col3, center3, 8)
-- arrow:  to thresh
instructions:hline(col1 + 20, col2 - 13, center3)
instructions:triangle(col2 - 16, center3, 0, 3)
-- arrow: thresh to or
instructions:hline(col2 + 13, col3 - 8, center3)
instructions:triangle(col3 - 11, center3, 0, 3)
-- arrow: or to title
instructions:vline(col3, center3 + 8, line1 - 2)
instructions:triangle(col3, line1 - 2, 90, 3)
-- arrow: fire to or
instructions:vline(col3, line4, center3 - 8)
instructions:triangle(col3, center3 - 11, 90, 3)

-- InputGate Class
local InputGate = Class {
  type = "Gate",
  canEdit = false,
  canMove = false
}
InputGate:include(EncoderControl)

function InputGate:init(args)
  -- required arguments
  local button = args.button or app.logError("%s.init: button is missing.", self)
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  local comparator = args.comparator or
                         app.logError("%s.init: comparator is missing.", self)
  EncoderControl.init(self, button)
  self:setClassName("Unit.ViewControl.InputGate")
  -- optional arguments
  -- optional arguments
  local readoutUnits = args.readoutUnits or app.unitHertz
  local readoutPrecision = args.readoutPrecision or 3
  local param = args.param

  self.comparator = comparator
  local graphic

  if param then
    app.logInfo("%s: using optional param", self)
    graphic = app.ComparatorView(0, 0, ply, 64, comparator, param)
  else
    graphic = app.ComparatorView(0, 0, ply, 64, comparator)
  end
  graphic:setLabel(button)
  graphic:setUnits(readoutUnits)
  graphic:setPrecision(readoutPrecision)
  self.comparatorView = graphic
  self:setMainCursorController(graphic)
  self:setControlGraphic(graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }

  -- sub display
  self.subGraphic = app.Graphic(0, 0, 128, 64)

  graphic = app.Drawing(0, 0, 128, 64)
  graphic:add(instructions)
  self.subGraphic:addChild(graphic)
  graphic = app.Label("or", 10)
  graphic:fitToText(0)
  graphic:setCenter(col3, center3 + 1)
  self.subGraphic:addChild(graphic)

  self.scope = app.MiniScope(col1 - 20, line4, 40, 45)
  self.scope:setBorder(1)
  self.scope:setCornerRadius(3, 3, 3, 3)
  self.subGraphic:addChild(self.scope)

  param = comparator:getParameter("Threshold")
  param:enableSerialization()
  self.threshold = app.Readout(0, 0, ply, 10)
  self.threshold:setParameter(param)
  self.threshold:setAttributes(app.unitNone, Encoder.getMap("default"))
  self.threshold:setCenter(col2, center4)
  self.subGraphic:addChild(self.threshold)

  graphic = app.Label(description, 10)
  graphic:fitToText(3)
  graphic:setSize(ply * 2, graphic.mHeight)
  graphic:setBorder(1)
  graphic:setCornerRadius(3, 0, 0, 3)
  graphic:setCenter(0.5 * (col2 + col3), center1 + 1)
  self.subGraphic:addChild(graphic)

  graphic = app.SubButton("input", 1)
  self.subGraphic:addChild(graphic)
  self.modButton = graphic

  graphic = app.SubButton("thresh", 2)
  self.subGraphic:addChild(graphic)

  graphic = app.SubButton("fire", 3)
  self.subGraphic:addChild(graphic)

  self.focusedReadout = nil

  local inlet = comparator:getInput("In")
  self.scope:watchInlet(inlet)
end

function InputGate:getPinControl()
  local Control = require "PinView.Gate"
  local control = Control {
    delegate = self,
    name = self.comparatorView:getLabel(),
    comparator = self.comparator,
    param = self.comparatorView:getReadoutParameter(),
    units = app.unitHertz,
    precision = 3
  }
  return control
end

function InputGate:rename(name)
  EncoderControl.rename(self, name)
  self.comparatorView:setLabel(name)
end

function InputGate:setFocusedReadout(readout)
  if readout then readout:save() end
  self.focusedReadout = readout
  self:setSubCursorController(readout)
end

function InputGate:spotReleased(spot, shifted)
  if EncoderControl.spotReleased(self, spot, shifted) then
    self:setFocusedReadout(nil)
    return true
  else
    return false
  end
end

------------------------------------------

function InputGate:enterReleased()
  if EncoderControl.enterReleased(self) then
    self:setFocusedReadout(nil)
    return true
  else
    return false
  end
end

function InputGate:zeroPressed()
  if self.focusedReadout then self.focusedReadout:zero() end
  return true
end

function InputGate:cancelReleased(shifted)
  if shifted then return false end
  if self.focusedReadout then self.focusedReadout:restore() end
  return true
end

function InputGate:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    local Overlay = require "Overlay"
    Overlay.flashSubMessage("Unit input only.")
  elseif i == 2 then
    if not self:hasFocus("encoder") then self:focus() end
    self:setFocusedReadout(self.threshold)
  elseif i == 3 then
    self.comparator:simulateFallingEdge();
  end
  return true
end

function InputGate:subPressed(i, shifted)
  if shifted then return false end
  if i == 3 then self.comparator:simulateRisingEdge(); end
  return true
end

function InputGate:encoder(change, shifted)
  if self.focusedReadout then
    self.focusedReadout:encoder(change, shifted,
                                self.encoderState == Encoder.Fine)
  end
  return true
end

return InputGate
