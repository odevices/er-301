local app = app
local Env = require "Env"
local Drawings = require "Drawings"
local Utils = require "Utils"
local Class = require "Base.Class"
local Control = require "MondrianMenu.Control"
local ply = app.SECTION_PLY

-- Option Class
local Option = Class {}
Option:include(Control)

function Option:init(args)
  Control.init(self)
  self:setClassName("MondrianMenu.Option")
  self.key = args.key or app.logError("%s.init: no key provided", self)
  self:setInstanceName(self.key)
  if args.value == nil then
    app.logError("%s.init: no value provided", self)
  else
    self.value = args.value
  end
  self.description = args.description or args.key
  self.choices = args.choices or
                     app.logError("%s.init: 'choices' is missing.", self)
  self.values = args.values
  self.descWidth = args.descriptionWidth or 1
  self.callback = args.callback or "onOptionChanged"
  self.allowNone = args.allowNone
  if args.allowNone then
    self.noneValue = args.noneValue or
                         app.logError("%s.init: 'noneValue' is missing.", self)
  end
  if args.offset then
    self.offset = args.offset
  elseif args.boolean then
    self.offset = 1
  else
    self.offset = 0
  end

  local graphic = app.RichTextBox(self.description, 10)
  graphic:setBorder(1)
  graphic:setBorderColor(app.GRAY7)
  graphic:setCornerRadius(3, 3, 3, 3)
  graphic:setMargins(3, 1, (4 + self.descWidth - 1) * ply, 1)
  graphic:fitHeight((4 + self.descWidth) * ply - 2)
  local h = graphic.mHeight // 2
  local labels = {}
  for i, choice in ipairs(self.choices) do
    local label = app.RichTextBox(choice, 10)
    label:fitHeight(ply - 2)
    labels[choice] = label
    graphic:addChild(label)
    label:setForegroundColor(app.GRAY5)
    label:setJustification(app.justifyCenter)
    label:setCenter(app.getButtonCenter(i + self.descWidth), h)
  end

  local instructions = app.DrawingInstructions()
  instructions:color(app.GRAY7)
  instructions:vline(self.descWidth * ply - 2, 1, graphic.mHeight - 1)
  local drawing = app.Drawing(0, 0, 256, 64)
  drawing:add(instructions)
  graphic:addChild(drawing)

  self:setControlGraphic(graphic)
  self.labels = labels
  self:setValue(self.value)
end

function Option:onReleased(i, shifted)
  if shifted then
    return false
  end
  if i > self.descWidth then
    local choiceIndex = i - self.descWidth - self.offset
    if choiceIndex <= #self.choices then
      local choice = self.choices[choiceIndex]
      local value = self:getValueFromChoice(choice)
      if self.allowNone and self.value == value then
        value = self.noneValue
      end
      self:setValue(value)
    end
  end
  self:callUp(self.callback, self)
  return true
end

function Option:getChoiceFromValue(value)
  if self.values then
    for i, v in ipairs(self.values) do
      if v == value then
        return self.choices[i]
      end
    end
  else
    return value
  end
end

function Option:getValueFromChoice(choice)
  if choice == nil then
    return self.noneValue
  end
  if self.values then
    for i, c in ipairs(self.choices) do
      if c == choice then
        return self.values[i]
      end
    end
  else
    return choice
  end
end

function Option:setValue(value)
  self.value = value
  local choice = self:getChoiceFromValue(value)
  for choice2, label in pairs(self.labels) do
    if choice == choice2 then
      label:setForegroundColor(app.WHITE)
    else
      label:setForegroundColor(app.GRAY5)
    end
  end
end

function Option:getValue()
  return self.value
end

function Option:getKey()
  return self.key
end

function Option:getDescription()
  return self.description
end

return Option
