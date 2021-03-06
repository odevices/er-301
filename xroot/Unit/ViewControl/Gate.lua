local app = app
local Class = require "Base.Class"
local Base = require "Unit.ViewControl.EncoderControl"
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

-- customizable properties
local customKeys = {
  "name",
  "description",
  "outputMode",
  "inversion"
}

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

-- Gate Class
local Gate = Class {
  type = "Gate",
  canEdit = true,
  canMove = true
}
Gate:include(Base)

function Gate:init(args)
  -- required arguments
  local button = args.button or app.logError("Gate.init: button is missing.")
  Base.init(self, button)
  self:setClassName("Unit.ViewControl.Gate")
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  local branch = args.branch or app.logError("%s.init: branch is missing.", self)
  local comparator = args.comparator or
                         app.logError("%s.init: comparator is missing.", self)
  
  self.branch = branch
  self.comparator = comparator
  -- optional arguments
  self:setDefaults(args)
  local readoutUnits = args.readoutUnits or app.unitHertz
  local readoutPrecision = args.readoutPrecision or 3
  local param = args.param

  local graphic

  if param then
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
  self.description = graphic
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

function Gate:onRemove()
  self.branch:unsubscribe("contentChanged", self)
  Base.onRemove(self)
end

function Gate:getPinControl()
  local Control = require "PinView.Gate"
  local control = Control {
    name = self:getCustomizableValue("name"),
    delegate = self,
    comparator = self.comparator,
    param = self.comparatorView:getReadoutParameter(),
    units = app.unitHertz,
    precision = 3
  }
  return control
end

function Gate:setDefaults(args)
  if args.outputMode == nil then args.outputMode = self.comparator:getMode() end
  if args.inversion == nil then
    args.inversion = self.comparator:isOutputInverted()
  end
  self.defaults = args
end

function Gate:getDefaults()
  return self.defaults
end

function Gate:customize(args)
  local t = self.customizations or {}

  for k, v in pairs(args) do t[k] = v end

  if args.name then self:rename(args.name) end
  if args.description then self.description:setText(args.description) end
  if args.outputMode then self.comparator:setMode(args.outputMode) end
  if args.inversion ~= nil then
    self.comparator:setOutputInversion(args.inversion)
  end
  self.customizations = t
end

function Gate:getCustomizations()
  return self.customizations
end

function Gate:getCustomizableValue(key)
  if key == "name" then
    return self.comparatorView:getLabel()
  elseif key == "description" then
    return self.description:getText()
  elseif key == "outputMode" then
    return self.comparator:getMode()
  elseif key == "inversion" then
    return self.comparator:isOutputInverted()
  end
end

function Gate.getCustomizableKeys()
  return customKeys
end

function Gate:removeCustomizations(force)
  if force or self.customizations then
    local defaults = self.defaults
    self:rename(defaults.button)
    self.description:setText(defaults.description)
    self.comparator:setMode(defaults.outputMode)
    self.comparator:setOutputInversion(defaults.inversion)
    self.customizations = nil
  end
end

function Gate:serialize()
  local t = Base.serialize(self)
  if self.customizations then
    local Utils = require "Utils"
    t.customizations = Utils.shallowCopy(self.customizations)
  end
  return t
end

function Gate:deserialize(t)
  Base.deserialize(self, t)
  self:removeCustomizations()
  if t.customizations then self:customize(t.customizations) end
end

function Gate:populateEditMenu(menu)
  if not self.isBuiltin then
    menu:addHeaderText("General Properties")
    menu:addKeyValue{
      key = "name",
      value = self:getCustomizableValue("name"),
      description = "Name",
      callback = "doEditString"
    }
    menu:addKeyValue{
      key = "description",
      value = self:getCustomizableValue("description"),
      description = "Description",
      callback = "doEditString"
    }
  end
  menu:addOption{
    key = "outputMode",
    value = self:getCustomizableValue("outputMode"),
    choices = {
      "gate",
      "toggle",
      "trigger on rise",
      "trigger on fall"
    },
    values = {
      app.COMPARATOR_GATE,
      app.COMPARATOR_TOGGLE,
      app.COMPARATOR_TRIGGER_ON_RISE,
      app.COMPARATOR_TRIGGER_ON_FALL
    },
    description = "Output Mode",
    callback = "onOptionChanged",
    allowNone = false
  }
  menu:addOption{
    key = "inversion",
    value = self:getCustomizableValue("inversion"),
    choices = {
      "yes",
      "no"
    },
    values = {
      true,
      false
    },
    description = "Invert Output",
    callback = "onOptionChanged",
    allowNone = false
  }
end

function Gate:createPinMark()
  local Drawings = require "Drawings"
  local graphic = app.Drawing(-5, -15, app.SECTION_PLY, 64)
  graphic:add(Drawings.Control.Pin)
  self.controlGraphic:addChildOnce(graphic)
  self.pinMark = graphic
end

function Gate:rename(name)
  Base.rename(self, name)
  self.comparatorView:setLabel(name)
end

function Gate:setFocusedReadout(readout)
  if readout then readout:save() end
  self.focusedReadout = readout
  self:setSubCursorController(readout)
end

function Gate:contentChanged(chain)
  if chain == self.branch then
    local outlet = chain:getMonitoringOutput(1)
    self.scope:watchOutlet(outlet)
    self.modButton:setText(chain:mnemonic())
  end
end

function Gate:spotReleased(spot, shifted)
  if Base.spotReleased(self, spot, shifted) then
    self:setFocusedReadout(nil)
    return true
  else
    return false
  end
end

------------------------------------------

function Gate:enterReleased()
  if Base.enterReleased(self) then
    self:setFocusedReadout(nil)
    return true
  else
    return false
  end
end

function Gate:zeroPressed()
  if self.focusedReadout then self.focusedReadout:zero() end
  return true
end

function Gate:cancelReleased(shifted)
  if self.focusedReadout then self.focusedReadout:restore() end
  return true
end

function Gate:doThresholdSet()
  local Decimal = require "Keyboard.Decimal"
  local desc = self.description:getText()
  local kb = Decimal {
    message = string.format("Threshold for the '%s' parameter.", desc),
    commitMessage = string.format("%s threshold updated.", desc),
    initialValue = self.threshold:getValueInUnits()
  }
  local task = function(value)
    if value then
      self.threshold:save()
      self.threshold:setValueInUnits(value)
      self:unfocus()
    end
  end
  kb:subscribe("done", task)
  kb:subscribe("commit", task)
  kb:show()
end

function Gate:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    local branch = self.branch
    if branch then
      self:unfocus()
      branch:show()
    end
  elseif i == 2 then
    if self:hasFocus("encoder") then
      if self.focusedReadout == self.threshold then
        self:doThresholdSet()
      else
        self:setFocusedReadout(self.threshold)
      end
    else
      self:focus()
      self:setFocusedReadout(self.threshold)
    end
  elseif i == 3 then
    self.comparator:simulateFallingEdge()
  end
  return true
end

function Gate:subPressed(i, shifted)
  if shifted then return false end
  if i == 3 then self.comparator:simulateRisingEdge() end
  return true
end

function Gate:encoder(change, shifted)
  if self.focusedReadout then
    self.focusedReadout:encoder(change, shifted,
                                self.encoderState == Encoder.Fine)
  end
  return true
end

return Gate
