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
-- arrow: branch to thresh
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

-- StateMachineView Class
local StateMachineView = Class {
  type = "State",
  canEdit = false,
  canMove = true
}
StateMachineView:include(Base)

function StateMachineView:init(args)
  -- required arguments
  local button = args.button or app.logError("%s.init: button is missing.", self)
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  local branch = args.branch or app.logError("%s.init: branch is missing.", self)
  local comparator = args.comparator or
                         app.logError("%s.init: comparator is missing.", self)
  local stateMachine = args.stateMachine or
                           app.logError("%s.init: stateMachine is missing.", self)
  Base.init(self, button)
  self:setClassName("Unit.ViewControl.StateMachineView")
  local showStateValue = args.showStateValue

  self.branch = branch
  self.comparator = comparator
  local graphic

  graphic = app.StateMachineView(0, 0, ply, 64, stateMachine, comparator)
  if showStateValue then graphic:showStateValue() end
  self.stateView = graphic
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

  local param = comparator:getParameter("Threshold")
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

  graphic = app.SubButton("empty", 1)
  self.subGraphic:addChild(graphic)
  self.modButton = graphic

  graphic = app.SubButton("thresh", 2)
  self.subGraphic:addChild(graphic)

  graphic = app.SubButton("fire", 3)
  self.subGraphic:addChild(graphic)

  self.focusedReadout = nil

  branch:subscribe("contentChanged", self)
end

function StateMachineView:onRemove()
  self.branch:unsubscribe("contentChanged", self)
  Base.onRemove(self)
end

function StateMachineView:rename(name)
  Base.rename(self, name)
  self.stateView:setLabel(name)
end

function StateMachineView:setFocusedReadout(readout)
  if readout then readout:save() end
  self.focusedReadout = readout
  self:setSubCursorController(readout)
end

function StateMachineView:contentChanged(chain)
  if chain == self.branch then
    local outlet = chain:getMonitoringOutput(1)
    self.scope:watchOutlet(outlet)
    self.modButton:setText(chain:mnemonic())
  end
end

function StateMachineView:spotReleased(spot, shifted)
  if Base.spotReleased(self, spot, shifted) then
    self:setFocusedReadout(nil)
    return true
  else
    return false
  end
end

------------------------------------------

function StateMachineView:enterReleased()
  if Base.enterReleased(self) then
    self:setFocusedReadout(nil)
    return true
  else
    return false
  end
end

function StateMachineView:zeroPressed()
  if self.focusedReadout then self.focusedReadout:zero() end
  return true
end

function StateMachineView:cancelReleased(shifted)
  if self.focusedReadout then self.focusedReadout:restore() end
  return true
end

function StateMachineView:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    local branch = self.branch
    if branch then
      self:unfocus()
      branch:show()
    end
  elseif i == 2 then
    if not self:hasFocus("encoder") then self:focus() end
    self:setFocusedReadout(self.threshold)
  elseif i == 3 then
    self.comparator:simulateFallingEdge();
  end
  return true
end

function StateMachineView:subPressed(i, shifted)
  if shifted then return false end
  if i == 3 then self.comparator:simulateRisingEdge(); end
  return true
end

function StateMachineView:encoder(change, shifted)
  if self.focusedReadout then
    self.focusedReadout:encoder(change, shifted,
                                self.encoderState == Encoder.Fine)
  end
  return true
end

return StateMachineView
