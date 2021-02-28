local app = app
local Class = require "Base.Class"
local Base = require "Unit.ViewControl.EncoderControl"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

local line1 = app.GRID5_LINE1
local line2 = app.GRID5_LINE2
local line3 = app.GRID5_LINE3
local line4 = app.GRID5_LINE4
local center1 = app.GRID5_CENTER1
local center2 = app.GRID5_CENTER2
local center3 = app.GRID5_CENTER3
local center4 = app.GRID5_CENTER4
local col1 = app.BUTTON1_CENTER
local col2 = app.BUTTON2_CENTER
local col3 = app.BUTTON3_CENTER

-- shared drawing
local instructions = app.DrawingInstructions()
-- multiply
instructions:circle(col2, center3, 8)
instructions:line(col2 - 3, center3 - 3, col2 + 3, center3 + 3)
instructions:line(col2 - 3, center3 + 3, col2 + 3, center3 - 3)
-- divide
instructions:circle(col3, center3, 8)
instructions:hline(col3 - 4, col3 + 4, center3)
instructions:vline(col3, center3 + 4, center3 + 3)
instructions:vline(col3, center3 - 4, center3 - 3)
-- arrow: branch to multiplier
instructions:hline(col1 + 20, col2 - 9, center3)
instructions:triangle(col2 - 12, center3, 0, 3)
-- arrow: gain to divider
instructions:hline(col2 + 9, col3 - 8, center3)
instructions:triangle(col3 - 11, center3, 0, 3)
-- arrow: bias to title
instructions:vline(col3, center3 + 8, line1 - 2)
instructions:triangle(col3, line1 - 2, 90, 3)

-- Clock Class
local Clock = Class {
  type = "Clock",
  canEdit = false,
  canMove = true
}
Clock:include(Base)

function Clock:init(args)
  -- required arguments
  local button = args.button or app.logError("%s.init: button is missing.", self)
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  local branch = args.branch or app.logError("%s.init: branch is missing.", self)
  local comparator = args.comparator or
                         app.logError("%s.init: comparator is missing.", self)
  local tap = args.tap or app.logError("%s.init: tap is missing.", self)
  Base.init(self, button)
  self:setClassName("Unit.ViewControl.Clock")
  -- optional arguments
  local readoutUnits = args.readoutUnits or app.unitHertz
  local param = args.param

  self.branch = branch
  self.comparator = comparator
  self.tap = tap
  local graphic

  if param then
    app.logInfo("%s: using optional param", self)
    graphic = app.ComparatorView(0, 0, ply, 64, comparator, param)
  else
    graphic = app.ComparatorView(0, 0, ply, 64, comparator)
  end
  graphic:setLabel(button)
  graphic:setUnits(readoutUnits)
  self.trigger = graphic
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

  self.scope = app.MiniScope(col1 - 20, line4, 40, 45)
  self.scope:setBorder(1)
  self.scope:setCornerRadius(3, 3, 3, 3)
  self.subGraphic:addChild(self.scope)

  param = tap:getParameter("Multiplier")
  param:enableSerialization()
  self.multiplier = app.Readout(0, 0, ply, 10)
  self.multiplier:setParameter(param)
  self.multiplier:setAttributes(app.unitNone, Encoder.getMap("int[1,32]"))
  self.multiplier:setPrecision(0)
  self.multiplier:setCenter(col2, center4)
  self.subGraphic:addChild(self.multiplier)

  param = tap:getParameter("Divider")
  param:enableSerialization()
  self.divider = app.Readout(0, 0, ply, 10)
  self.divider:setParameter(param)
  self.divider:setAttributes(app.unitNone, Encoder.getMap("int[1,32]"))
  self.divider:setPrecision(0)
  self.divider:setCenter(col3, center4)
  self.subGraphic:addChild(self.divider)

  graphic = app.Label(description, 10)
  graphic:fitToText(3)
  graphic:setSize(ply * 2, graphic.mHeight)
  graphic:setBorder(1)
  graphic:setCornerRadius(3, 0, 0, 3)
  graphic:setCenter(0.5 * (col2 + col3), center1 + 1)
  self.subGraphic:addChild(graphic)

  graphic = app.SubButton("empty", 1)
  self.subGraphic:addChild(graphic)
  self.modButton = graphic

  graphic = app.SubButton("multiply", 2)
  self.subGraphic:addChild(graphic)

  graphic = app.SubButton("divide", 3)
  self.subGraphic:addChild(graphic)

  self.focusedReadout = nil

  branch:subscribe("contentChanged", self)
end

function Clock:onRemove()
  self.branch:unsubscribe("contentChanged", self)
  Base.onRemove(self)
end

function Clock:rename(name)
  Base.rename(self, name)
  self.trigger:setLabel(name)
end

function Clock:setFocusedReadout(readout)
  if readout then readout:save() end
  self.focusedReadout = readout
  self:setSubCursorController(readout)
end

function Clock:contentChanged(chain)
  if chain == self.branch then
    local outlet = chain:getMonitoringOutput(1)
    self.scope:watchOutlet(outlet)
    self.modButton:setText(chain:mnemonic())
  end
end

function Clock:spotReleased(spot, shifted)
  if Base.spotReleased(self, spot, shifted) then
    self:setFocusedReadout(nil)
    return true
  else
    return false
  end
end

------------------------------------------

function Clock:enterReleased()
  if Base.enterReleased(self) then
    self:setFocusedReadout(nil)
    return true
  else
    return false
  end
end

function Clock:zeroPressed()
  if self.focusedReadout then self.focusedReadout:zero() end
  return true
end

function Clock:cancelReleased(shifted)
  if self.focusedReadout then self.focusedReadout:restore() end
  return true
end

function Clock:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    local branch = self.branch
    if branch then
      self:unfocus()
      branch:show()
    end
  elseif i == 2 then
    if not self:hasFocus("encoder") then self:focus() end
    self:setFocusedReadout(self.multiplier)
  elseif i == 3 then
    if not self:hasFocus("encoder") then self:focus() end
    self:setFocusedReadout(self.divider)
  end
  return true
end

function Clock:subPressed(i, shifted)
  return true
end

function Clock:encoder(change, shifted)
  if self.focusedReadout then
    self.focusedReadout:encoder(change, shifted,
                                self.encoderState == Encoder.Fine)
  end
  return true
end

return Clock
