local app = app
local Class = require "Base.Class"
local Base = require "Unit.ViewControl.EncoderControl"
local Encoder = require "Encoder"
local Settings = require "Settings"
local swigTypeLinearDialMap = swig_type(Encoder.getMap("bias"))

-- customizable properties
local biasMapKeys = {
  "biasMin",
  "biasMax",
  "biasSuperCoarseStep",
  "biasCoarseStep",
  "biasFineStep",
  "biasSuperFineStep"
}
local gainMapKeys = {
  "gainMin",
  "gainMax",
  "gainSuperCoarseStep",
  "gainCoarseStep",
  "gainFineStep",
  "gainSuperFineStep"
}
local customKeys = {
  "name",
  "description",
  "biasUnits",
  "biasPrecision"
}
local isGainMapProperty = {}
local isBiasMapProperty = {}

for _, key in ipairs(gainMapKeys) do
  customKeys[#customKeys + 1] = key
  isGainMapProperty[key] = true
end

for _, key in ipairs(biasMapKeys) do
  customKeys[#customKeys + 1] = key
  isBiasMapProperty[key] = true
end

-- drawing related
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

-- shared drawing
local instructions = app.DrawingInstructions()
-- multiply
instructions:circle(col2, center3, 8)
instructions:line(col2 - 3, center3 - 3, col2 + 3, center3 + 3)
instructions:line(col2 - 3, center3 + 3, col2 + 3, center3 - 3)
-- sum
instructions:circle(col3, center3, 8)
instructions:hline(col3 - 5, col3 + 5, center3)
instructions:vline(col3, center3 - 5, center3 + 5)
-- arrow: branch to gain
instructions:hline(col1 + 20, col2 - 9, center3)
instructions:triangle(col2 - 12, center3, 0, 3)
-- arrow: gain to bias
instructions:hline(col2 + 9, col3 - 8, center3)
instructions:triangle(col3 - 11, center3, 0, 3)
-- arrow: bias to title
instructions:vline(col3, center3 + 8, line1 - 2)
instructions:triangle(col3, line1 - 2, 90, 3)

-- GainBias Class
local GainBias = Class {
  type = "GainBias",
  canMove = true,
  canEdit = false
}
GainBias:include(Base)

function GainBias:init(args)
  -- required arguments
  local button = args.button or app.logError("%s.init: button is missing.", self)
  local branch = args.branch or app.logError("%s.init: branch is missing.", self)
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  local gainbias = args.gainbias or
                       app.logError("%s.init: gainbias is missing.", self)
  local range = args.range or app.logError("%s.init: range is missing.", self)
  Base.init(self, button)
  self:setClassName("Unit.ViewControl.GainBias")
  -- optional arguments
  self:setDefaults(args)
  local initialBias = args.initialBias
  local initialGain = args.initialGain
  local biasMap = args.biasMap
  local gainMap = args.gainMap
  local biasUnits = args.biasUnits
  local biasPrecision = args.biasPrecision
  local scaling = args.scaling

  self.branch = branch

  local gainParam = gainbias:getParameter("Gain")
  if gainParam == nil then
    app.logError("%s.init: %s does not have a Gain parameter.", self, gainbias)
  end
  gainParam:enableSerialization()

  local biasParam = gainbias:getParameter("Bias")
  if biasParam == nil then
    app.logError("%s.init: %s does not have a Bias parameter.", self, gainbias)
  end
  biasParam:enableSerialization()

  if initialBias then biasParam:hardSet(initialBias) end

  if initialGain then gainParam:hardSet(initialGain) end

  local graphic
  local readoutParam
  if Settings.get("unitControlReadoutSource") == "actual" then
    readoutParam = range:getParameter("Center") or biasParam
  else
    readoutParam = biasParam
  end
  graphic = app.Fader(0, 0, ply, 64)
  graphic:setTargetParameter(biasParam)
  graphic:setValueParameter(biasParam)
  graphic:setControlParameter(readoutParam)
  graphic:setRangeObject(range)
  graphic:setLabel(button)
  self.fader = graphic
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

  graphic = app.MiniScope(col1 - 20, line4, 40, 45)
  graphic:setBorder(1)
  graphic:setCornerRadius(3, 3, 3, 3)
  self.scope = graphic
  self.subGraphic:addChild(graphic)

  -- gain
  graphic = app.Readout(0, 0, ply, 10)
  graphic:setParameter(gainParam)
  graphic:setCenter(col2, center4)
  self.gain = graphic
  self.subGraphic:addChild(graphic)

  -- bias
  graphic = app.Readout(0, 0, ply, 10)
  graphic:setParameter(biasParam)
  graphic:setCenter(col3, center4)
  self.bias = graphic
  self.subGraphic:addChild(graphic)

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

  graphic = app.SubButton("gain", 2)
  self.subGraphic:addChild(graphic)

  graphic = app.SubButton("bias", 3)
  self.subGraphic:addChild(graphic)

  self:setFocusedReadout(self.bias)

  self:setBiasUnits(biasUnits)
  self:setBiasMap(biasMap)
  self:setGainMap(gainMap)
  self:setFaderScaling(scaling)
  self:setBiasPrecision(biasPrecision)

  self.gainEncoderState = Encoder.Coarse

  branch:subscribe("contentChanged", self)

  if swig_type(biasMap) == swigTypeLinearDialMap then self.canEdit = true end
end

function GainBias:onRemove()
  self.branch:unsubscribe("contentChanged", self)
  Base.onRemove(self)
end

function GainBias:getPinControl()
  local Fader = require "PinView.Fader"
  local control = Fader {
    delegate = self,
    name = self:getCustomizableValue("name"),
    valueParam = self.bias:getParameter(),
    range = self.fader:getRangeObject(),
    units = self:getCustomizableValue("biasUnits"),
    map = self:getBiasMap(),
    scaling = self.defaults.scaling,
    precision = self:getCustomizableValue("biasPrecision")
  }
  return control
end

function GainBias:setDefaults(args)
  if args.biasMap == nil then args.biasMap = Encoder.getMap("bias") end
  if args.gainMap == nil then args.gainMap = Encoder.getMap("gain") end
  if args.biasUnits == nil then args.biasUnits = app.unitNone end
  if args.biasPrecision == nil then
    if args.biasMap:getRounding() > 0.9 or args.biasUnits == app.unitDecibels then
      args.biasPrecision = 1
    else
      args.biasPrecision = 3
    end
  end
  if args.scaling == nil then args.scaling = app.linearScaling end
  self.defaults = args
end

function GainBias:getDefaults()
  return self.defaults
end

function GainBias:customize(args)
  local t = self.customizations or {}
  local refreshBiasMap = false
  local refreshGainMap = false
  for k, v in pairs(args) do
    if isGainMapProperty[k] then
      refreshGainMap = true
    elseif isBiasMapProperty[k] then
      refreshBiasMap = true
    end
    t[k] = v
  end

  if args.name then self:rename(args.name) end
  if args.description then self.description:setText(args.description) end
  if args.biasUnits then self:setBiasUnits(args.biasUnits) end
  if args.biasPrecision then self:setBiasPrecision(args.biasPrecision) end

  if refreshBiasMap then
    local map = self.privateBiasMap
    if map == nil then
      map = app.LinearDialMap(self.defaults.biasMap)
      self.privateBiasMap = map
    end
    local superCoarse = t.biasSuperCoarseStep or map:superCoarseStep()
    local coarse = t.biasCoarseStep or map:coarseStep()
    local fine = t.biasFineStep or map:fineStep()
    local superFine = t.biasSuperFineStep or map:superFineStep()
    if t.biasMin then map:setMin(t.biasMin) end
    if t.biasMax then map:setMax(t.biasMax) end
    map:setSteps(superCoarse, coarse, fine, superFine)
    self:setBiasMap(map)
  end

  if refreshGainMap then
    local map = self.privateGainMap
    if map == nil then
      map = app.LinearDialMap(self.defaults.gainMap)
      self.privateGainMap = map
    end
    local superCoarse = t.gainSuperCoarseStep or map:superCoarseStep()
    local coarse = t.gainCoarseStep or map:coarseStep()
    local fine = t.gainFineStep or map:fineStep()
    local superFine = t.gainSuperFineStep or map:superFineStep()
    if t.gainMin then map:setMin(t.gainMin) end
    if t.gainMax then map:setMax(t.gainMax) end
    map:setSteps(superCoarse, coarse, fine, superFine)
    self:setGainMap(map)
  end

  self.customizations = t
end

function GainBias:getCustomizations()
  return self.customizations
end

function GainBias:getCustomizableValue(key)
  if key == "name" then
    return self.fader:getLabel()
  elseif key == "description" then
    return self.description:getText()
  elseif key == "biasUnits" then
    return self.bias:getUnits()
  elseif key == "biasPrecision" then
    return self:getBiasPrecision()
  elseif key == "biasMin" then
    return self:getBiasMap():min()
  elseif key == "biasMax" then
    return self:getBiasMap():max()
  elseif key == "biasSuperCoarseStep" then
    return self:getBiasMap():superCoarseStep()
  elseif key == "biasCoarseStep" then
    return self:getBiasMap():coarseStep()
  elseif key == "biasFineStep" then
    return self:getBiasMap():fineStep()
  elseif key == "biasSuperFineStep" then
    return self:getBiasMap():superFineStep()
  elseif key == "biasCurve" then
    return "linear"
  elseif key == "gainMin" then
    return self:getGainMap():min()
  elseif key == "gainMax" then
    return self:getGainMap():max()
  elseif key == "gainSuperCoarseStep" then
    return self:getGainMap():superCoarseStep()
  elseif key == "gainCoarseStep" then
    return self:getGainMap():coarseStep()
  elseif key == "gainFineStep" then
    return self:getGainMap():fineStep()
  elseif key == "gainSuperFineStep" then
    return self:getGainMap():superFineStep()
  end
end

function GainBias:getCustomizableKeys()
  return customKeys
end

function GainBias:removeCustomizations(force)
  if force or self.customizations then
    local defaults = self.defaults
    self:rename(defaults.button)
    self.description:setText(defaults.description)
    self:setBiasUnits(defaults.biasUnits)
    self:setBiasMap(defaults.biasMap)
    self.privateBiasMap = nil
    self:setGainMap(defaults.gainMap)
    self.privateGainMap = nil
    self:setBiasPrecision(defaults.biasPrecision)
    self.customizations = nil
  end
end

function GainBias:createPinMark()
  local Drawings = require "Drawings"
  local graphic = app.Drawing(-3, 0, app.SECTION_PLY, 64)
  graphic:add(Drawings.Control.Pin)
  self.controlGraphic:addChildOnce(graphic)
  self.pinMark = graphic
end

function GainBias:populateEditMenu(menu)
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

  -- bias
  menu:addHeaderText("Bias Properties")
  menu:addKeyValue{
    key = "biasMin",
    value = self:getCustomizableValue("biasMin"),
    description = "Bias Min",
    callback = "doEditDecimal"
  }
  menu:addKeyValue{
    key = "biasMax",
    value = self:getCustomizableValue("biasMax"),
    description = "Bias Max",
    callback = "doEditDecimal"
  }
  menu:addKeyValue{
    key = "biasSuperCoarseStep",
    value = self:getCustomizableValue("biasSuperCoarseStep"),
    description = "Super Coarse Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addKeyValue{
    key = "biasCoarseStep",
    value = self:getCustomizableValue("biasCoarseStep"),
    description = "Coarse Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addKeyValue{
    key = "biasFineStep",
    value = self:getCustomizableValue("biasFineStep"),
    description = "Fine Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addKeyValue{
    key = "biasSuperFineStep",
    value = self:getCustomizableValue("biasSuperFineStep"),
    description = "Super Fine Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addOption{
    key = "biasUnits",
    value = self:getCustomizableValue("biasUnits"),
    choices = {
      "Freq (1Hz)",
      "Time (1s)",
      "Gain (1dB)",
      "Scale (1x)"
    },
    values = {
      app.unitHertz,
      app.unitSecs,
      app.unitDecibels,
      app.unitMultiplier
    },
    noneValue = app.unitNone,
    description = "Unit Suffix",
    callback = "onOptionChanged",
    allowNone = true
  }
  menu:addOption{
    key = "biasPrecision",
    value = self:getCustomizableValue("biasPrecision"),
    choices = {
      0,
      1,
      2,
      3
    },
    description = "Visible Digits",
    callback = "onOptionChanged"
  }
  --[[
  menu:addOption{
    key = "biasCurve",
    value = self:getCustomizableValue("biasCurve"),
    choices = {"linear","exp","octaves"},
    description = "Bias Curve",
    callback = "onOptionChanged"
  }
  ]] --

  -- gain
  menu:addHeaderText("Gain Properties")
  menu:addKeyValue{
    key = "gainMin",
    value = self:getCustomizableValue("gainMin"),
    description = "Gain Min",
    callback = "doEditDecimal"
  }
  menu:addKeyValue{
    key = "gainMax",
    value = self:getCustomizableValue("gainMax"),
    description = "Gain Max",
    callback = "doEditDecimal"
  }
  menu:addKeyValue{
    key = "gainSuperCoarseStep",
    value = self:getCustomizableValue("gainSuperCoarseStep"),
    description = "Super Coarse Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addKeyValue{
    key = "gainCoarseStep",
    value = self:getCustomizableValue("gainCoarseStep"),
    description = "Coarse Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addKeyValue{
    key = "gainFineStep",
    value = self:getCustomizableValue("gainFineStep"),
    description = "Gain Fine Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
  menu:addKeyValue{
    key = "gainSuperFineStep",
    value = self:getCustomizableValue("gainSuperFineStep"),
    description = "Super Fine Step",
    callback = "doEditDecimal",
    options = {
      min = 1
    }
  }
end

function GainBias:serialize()
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

function GainBias:deserialize(t)
  Base.deserialize(self, t)
  if t.gainEncoderState then
    self.gainEncoderState = Encoder.deserialize(t.gainEncoderState)
  end
  self:removeCustomizations()
  if t.customizations then self:customize(t.customizations) end
end

function GainBias:rename(name)
  Base.rename(self, name)
  self.fader:setLabel(name)
end

function GainBias:setBias(x)
  self.bias:softSet(x)
end

function GainBias:setGain(x)
  self.gain:softSet(x)
end

function GainBias:setFaderScaling(scaling)
  self.fader:setScaling(scaling or app.linearScaling)
end

function GainBias:setTextBelow(value, text)
  self.fader:setTextBelow(value, text)
end

function GainBias:setBiasUnits(biasUnits)
  self.bias:setUnits(biasUnits)
  self.fader:setUnits(biasUnits)
end

function GainBias:setBiasPrecision(p)
  self.bias:setPrecision(p)
  self.fader:setPrecision(p)
end

function GainBias:getBiasPrecision()
  return self.bias:getPrecision()
end

function GainBias:setBiasMap(map)
  self.bias:setMap(map)
  self.fader:setMap(map)
end

function GainBias:getBiasMap()
  return self.privateBiasMap or self.defaults.biasMap
end

function GainBias:setGainMap(map)
  self.gain:setMap(map)
end

function GainBias:getGainMap()
  return self.privateGainMap or self.defaults.gainMap
end

function GainBias:setFocusedReadout(readout)
  if readout then readout:save() end
  self.focusedReadout = readout
  self:setSubCursorController(readout)
  if readout == self.gain then
    Encoder.set(self.gainEncoderState)
  else
    Encoder.set(self.encoderState)
  end
end

function GainBias:contentChanged(chain)
  if chain == self.branch then
    local outlet = chain:getMonitoringOutput(1)
    self.scope:watchOutlet(outlet)
    self.modButton:setText(chain:mnemonic())
  end
end

function GainBias:spotReleased(spot, shifted)
  if Base.spotReleased(self, spot, shifted) then
    self:setFocusedReadout(self.bias)
    return true
  else
    return false
  end
end

function GainBias:dialPressed(shifted)
  if self.focusedReadout == self.gain then
    if self.gainEncoderState == Encoder.Coarse then
      self.gainEncoderState = Encoder.Fine
    else
      self.gainEncoderState = Encoder.Coarse
    end
    Encoder.set(self.gainEncoderState)
  else
    Base.dialPressed(self, shifted)
  end
  return true
end

function GainBias:enterReleased()
  if Base.enterReleased(self) then
    self:setFocusedReadout(self.bias)
    return true
  else
    return false
  end
end

function GainBias:zeroPressed()
  self.focusedReadout:zero()
  return true
end

function GainBias:cancelReleased(shifted)
  if shifted then return false end
  self.focusedReadout:restore()
  return true
end

function GainBias:onFocused()
  self.focusedReadout:save()
end

function GainBias:doGainSet()
  local Decimal = require "Keyboard.Decimal"
  local desc = self.description:getText()
  local kb = Decimal {
    message = string.format("Modulation gain for the '%s' parameter.", desc),
    commitMessage = string.format("%s gain updated.", desc),
    initialValue = self.gain:getValueInUnits()
  }
  local task = function(value)
    if value then
      self.gain:save()
      self.gain:setValueInUnits(value)
      self:unfocus()
    end
  end
  kb:subscribe("done", task)
  kb:subscribe("commit", task)
  kb:show()
end

function GainBias:doBiasSet()
  local Decimal = require "Keyboard.Decimal"
  local desc = self.description:getText()
  local kb = Decimal {
    message = string.format("Modulation bias for the '%s' parameter.", desc),
    commitMessage = string.format("%s bias updated.", desc),
    initialValue = self.bias:getValueInUnits()
  }
  local task = function(value)
    if value then
      self.bias:save()
      self.bias:setValueInUnits(value)
      self:unfocus()
    end
  end
  kb:subscribe("done", task)
  kb:subscribe("commit", task)
  kb:show()
end

function GainBias:subReleased(i, shifted)
  if shifted then
    if i == 2 then
      self:doGainSet()
    elseif i == 3 then
      self:doBiasSet()
    end
    return true
  end
  if i == 1 then
    local branch = self.branch
    if branch then
      self:unfocus()
      branch:show()
    end
  elseif i == 2 then
    if self:hasFocus("encoder") then
      if self.focusedReadout == self.gain then
        self:doGainSet()
      else
        self:setFocusedReadout(self.gain)
      end
    else
      self:focus()
      self:setFocusedReadout(self.gain)
    end
  elseif i == 3 then
    if self:hasFocus("encoder") then
      if self.focusedReadout == self.bias then
        self:doBiasSet()
      else
        self:setFocusedReadout(self.bias)
      end
    else
      self:focus()
      self:setFocusedReadout(self.bias)
    end
  end
  return true
end

function GainBias:encoder(change, shifted)
  if self.focusedReadout == self.gain then
    self.gain:encoder(change, shifted, self.gainEncoderState == Encoder.Fine)
  else
    self.bias:encoder(change, shifted, self.encoderState == Encoder.Fine)
  end
  return true
end

return GainBias
