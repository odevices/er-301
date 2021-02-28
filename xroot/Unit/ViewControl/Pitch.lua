local app = app
local Class = require "Base.Class"
local Base = require "Unit.ViewControl.EncoderControl"
local Encoder = require "Encoder"
local Settings = require "Settings"
local ply = app.SECTION_PLY

-- customizable properties
local mapKeys = {
  "min",
  "max",
  "superCoarseStep",
  "coarseStep",
  "fineStep",
  "superFineStep"
}
local customKeys = {
  "name",
  "description"
}

local isMapProperty = {}
for _, key in ipairs(mapKeys) do
  customKeys[#customKeys + 1] = key
  isMapProperty[key] = true
end

local defaultMap = Encoder.getMap("cents")

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

-- shared drawing
local instructions = app.DrawingInstructions()
-- sum
instructions:circle(col2, center3, 8)
instructions:hline(col2 - 5, col2 + 5, center3)
instructions:vline(col2, center3 - 5, center3 + 5)
-- 2^V
instructions:box(col3 - 8, center3 - 8, 16, 16)
-- arrow: branch to bias
instructions:hline(col1 + 20, col2 - 9, center3)
instructions:triangle(col2 - 12, center3, 0, 3)
-- arrow: bias to 2^V
instructions:hline(col2 + 9, col3 - 8, center3)
instructions:triangle(col3 - 11, center3, 0, 3)
-- arrow: 2^V to title
instructions:vline(col3, center3 + 8, line1 - 2)
instructions:triangle(col3, line1 - 2, 90, 3)

-- Pitch Class
local Pitch = Class {
  type = "Pitch",
  canEdit = true,
  canMove = true
}
Pitch:include(Base)

function Pitch:init(args)
  -- required arguments
  local button = args.button or app.logError("%s.init: button is missing.", self)
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  local branch = args.branch or app.logError("%s.init: branch is missing.", self)
  local offset = args.offset or app.logError("%s.init: offset is missing.", self)
  local range = args.range or app.logError("%s.init: range is missing.", self)
  Base.init(self, button)
  self:setClassName("Unit.ViewControl.Pitch")
  self:setDefaults(args)

  self.branch = branch
  local graphic
  local faderParam = offset:getParameter("Offset") or
                         offset:getParameter("Bias")
  faderParam:enableSerialization()
  local readoutParam
  if Settings.get("unitControlReadoutSource") == "actual" then
    readoutParam = range:getParameter("Center") or faderParam
  else
    readoutParam = faderParam
  end
  graphic = app.Fader(0, 0, ply, 64)
  graphic:setTargetParameter(faderParam)
  graphic:setValueParameter(faderParam)
  graphic:setControlParameter(readoutParam)
  graphic:setRangeObject(range)
  graphic:setLabel(button)
  graphic:setAttributes(app.unitCents, defaultMap)
  graphic:setPrecision(0)
  self.fader = graphic
  self:setMainCursorController(graphic)
  self:setControlGraphic(graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }

  -- sub display
  self.subGraphic = app.Graphic(0, 0, 128, 64)
  local drawing = app.Drawing(0, 0, 128, 64)
  drawing:add(instructions)
  self.subGraphic:addChild(drawing)
  graphic = app.Label("2", 12)
  graphic:fitToText(0)
  graphic:setCenter(col3 - 2, center3)
  self.subGraphic:addChild(graphic)
  graphic = app.Label("v", 10)
  graphic:fitToText(0)
  graphic:setCenter(col3 + 3, center3 + 5)
  self.subGraphic:addChild(graphic)

  self.scope = app.MiniScope(col1 - 20, line4, 40, 45)
  self.scope:setBorder(1)
  self.scope:setCornerRadius(3, 3, 3, 3)
  self.subGraphic:addChild(self.scope)

  self.readout = app.Readout(0, 0, ply, 10)
  self.readout:setParameter(faderParam)
  self.readout:setCenter(col2, center4)
  self.readout:setAttributes(app.unitCents, defaultMap)
  self.readout:setPrecision(0)
  self.subGraphic:addChild(self.readout)

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

  graphic = app.SubButton("tune", 2)
  self.subGraphic:addChild(graphic)

  branch:subscribe("contentChanged", self)
end

function Pitch:onRemove()
  self.branch:unsubscribe("contentChanged", self)
  Base.onRemove(self)
end

function Pitch:getPinControl()
  local Fader = require "PinView.Fader"
  local control = Fader {
    delegate = self,
    name = self:getCustomizableValue("name"),
    valueParam = self.fader:getValueParameter(),
    range = self.fader:getRangeObject(),
    units = app.unitCents,
    map = defaultMap,
    scaling = app.linearScaling,
    precision = 0
  }
  return control
end

function Pitch:setDefaults(args)
  self.defaults = args
end

function Pitch:getDefaults()
  return self.defaults
end

function Pitch:customize(args)
  local t = self.customizations or {}
  local refreshMap = false
  for k, v in pairs(args) do
    if isMapProperty[k] then refreshMap = true end
    t[k] = v
  end

  if args.name then self:rename(args.name) end
  if args.description then self.description:setText(args.description) end
  if args.precision then self:setPrecision(args.precision) end

  if refreshMap then
    local map = self.privateMap
    if map == nil then
      map = app.LinearDialMap(defaultMap)
      self.privateMap = map
    end
    local superCoarse = t.superCoarseStep or map:superCoarseStep()
    local coarse = t.coarseStep or map:coarseStep()
    local fine = t.fineStep or map:fineStep()
    local superFine = t.superFineStep or map:superFineStep()
    if t.min then map:setMin(t.min) end
    if t.max then map:setMax(t.max) end
    map:setSteps(superCoarse, coarse, fine, superFine)
    self:setMap(map)
  end

  self.customizations = t
end

function Pitch:getCustomizations()
  return self.customizations
end

function Pitch:getCustomizableValue(key)
  if key == "name" then
    return self.fader:getLabel()
  elseif key == "description" then
    return self.description:getText()
  elseif key == "precision" then
    return self:getPrecision()
  elseif key == "min" then
    return self:getMap():min()
  elseif key == "max" then
    return self:getMap():max()
  elseif key == "superCoarseStep" then
    return self:getMap():superCoarseStep()
  elseif key == "coarseStep" then
    return self:getMap():coarseStep()
  elseif key == "fineStep" then
    return self:getMap():fineStep()
  elseif key == "superFineStep" then
    return self:getMap():superFineStep()
  end
end

function Pitch:getCustomizableKeys()
  return customKeys
end

function Pitch:removeCustomizations(force)
  if force or self.customizations then
    local defaults = self.defaults
    self:rename(defaults.button)
    self.description:setText(defaults.description)
    self:setMap(defaultMap)
    self.privateMap = nil
    self:setPrecision(0)
    self.customizations = nil
  end
end

function Pitch:populateEditMenu(menu)
  if not self.isBuiltin then
    -- general
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

  menu:addHeaderText("Tune Properties")
  menu:addKeyValue{
    key = "min",
    value = self:getCustomizableValue("min"),
    description = "Tune Min",
    callback = "doEditDecimal"
  }
  menu:addKeyValue{
    key = "max",
    value = self:getCustomizableValue("max"),
    description = "Tune Max",
    callback = "doEditDecimal"
  }
  menu:addKeyValue{
    key = "superCoarseStep",
    value = self:getCustomizableValue("superCoarseStep"),
    description = "Super Coarse Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addKeyValue{
    key = "coarseStep",
    value = self:getCustomizableValue("coarseStep"),
    description = "Coarse Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addKeyValue{
    key = "fineStep",
    value = self:getCustomizableValue("fineStep"),
    description = "Fine Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addKeyValue{
    key = "superFineStep",
    value = self:getCustomizableValue("superFineStep"),
    description = "Super Fine Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addOption{
    key = "precision",
    value = self:getCustomizableValue("precision"),
    choices = {
      0,
      1,
      2,
      3
    },
    description = "Visible Digits",
    callback = "onOptionChanged"
  }
end

function Pitch:setMap(map)
  self.readout:setMap(map)
  self.fader:setMap(map)
end

function Pitch:getMap()
  return self.privateMap or defaultMap
end

function Pitch:setPrecision(p)
  self.readout:setPrecision(p)
  self.fader:setPrecision(p)
end

function Pitch:getPrecision()
  return self.readout:getPrecision()
end

function Pitch:serialize()
  local t = Base.serialize(self)
  if t.gainEncoderState ~= Encoder.Coarse then
    t.gainEncoderState = Encoder.serialize(self.gainEncoderState)
  end
  if self.customizations then
    local Utils = require "Utils"
    t.customizations = Utils.shallowCopy(self.customizations)
  end
  return t
end

function Pitch:deserialize(t)
  Base.deserialize(self, t)
  if t.gainEncoderState then
    self.gainEncoderState = Encoder.deserialize(t.gainEncoderState)
  end
  self:removeCustomizations()
  if t.customizations then self:customize(t.customizations) end
end

function Pitch:rename(name)
  Base.rename(self, name)
  self.fader:setLabel(name)
end

function Pitch:contentChanged(chain)
  if chain == self.branch then
    local outlet = chain:getMonitoringOutput(1)
    self.scope:watchOutlet(outlet)
    self.modButton:setText(chain:mnemonic())
  end
end

function Pitch:zeroPressed()
  self.readout:zero()
  return true
end

function Pitch:cancelReleased()
  self.readout:restore()
  return true
end

function Pitch:onFocused()
  self.readout:save()
end

function Pitch:doTuneSet()
  local Decimal = require "Keyboard.Decimal"
  local desc = self.description:getText()
  local kb = Decimal {
    message = string.format("Offset for the '%s' parameter.", desc),
    commitMessage = string.format("%s offset updated.", desc),
    initialValue = self.readout:getValueInUnits()
  }
  local task = function(value)
    if value then
      self.readout:save()
      self.readout:setValueInUnits(value)
      self:unfocus()
    end
  end
  kb:subscribe("done", task)
  kb:subscribe("commit", task)
  kb:show()
end

function Pitch:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    local branch = self.branch
    if branch then
      self:unfocus()
      branch:show()
    end
  elseif i == 2 then
    if self:hasFocus("encoder") then self:doTuneSet() end
  elseif i == 3 then

  end
  return true
end

function Pitch:encoder(change, shifted)
  self.readout:encoder(change, shifted, self.encoderState == Encoder.Fine)
  return true
end

return Pitch
