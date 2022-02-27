local Class = require "Base.Class"
local Control = require "MondrianMenu.Control"

local function createTextBox(text, justify, shrink)
  local graphic = app.RichTextBox(text, 10)
  local ply = app.SECTION_PLY
  shrink = shrink or 2
  justify = justify or app.justifyCenter
  if graphic:flowText(ply - shrink, 2) then
    graphic:fitHeight(ply - shrink)
  elseif graphic:flowText(2 * ply - shrink, 2) then
    graphic:fitHeight(2 * ply - shrink)
  else
    graphic:fitHeight(3 * ply - shrink)
  end
  graphic:setJustification(justify)
  graphic:setBorder(1)
  graphic:setBorderColor(app.GRAY7)
  graphic:setCornerRadius(3, 3, 3, 3)
  return graphic
end

------------------------------------
local KeyValue = Class {}
KeyValue:include(Control)

function KeyValue:init(args)
  Control.init(self)
  self:setClassName("MondrianMenu.KeyValue")
  self.key = args.key or app.logError("%s.init: no key provided", self)
  if args.value == nil then
    app.logError("%s.init: no value provided", self)
  else
    self.value = args.value
  end
  self.description = args.description or args.key
  self.format = args.format or "%s:\n%s"
  self.callback = args.callback or "onKeyValueReleased"
  self.options = args.options
  local text = self:getDisplayString()
  local graphic = createTextBox(text)
  self:setControlGraphic(graphic)
end

function KeyValue:getDisplayString()
  local value = self.value
  if type(value) == "number" then
    value = app.Readout.formatQuantity(value, app.unitNone, 3, true)
  end
  return string.format(self.format, self.description, value)
end

function KeyValue:setValue(value, invalidateLayout)
  self.value = value
  local text = self:getDisplayString()
  local graphic = self.controlGraphic
  graphic:setText(text)
  local ply = app.SECTION_PLY
  if graphic:flowText(ply - 2, 2) then
    graphic:fitHeight(ply - 2)
  elseif graphic:flowText(2 * ply - 2, 2) then
    graphic:fitHeight(2 * ply - 2)
  else
    graphic:fitHeight(3 * ply - 2)
  end

  if invalidateLayout then
    self:callUp("invalidateLayout")
  end
end

function KeyValue:getValue()
  return self.value
end

function KeyValue:getKey()
  return self.key
end

function KeyValue:getDescription()
  return self.description
end

function KeyValue:onReleased(i, shifted)
  self:callUp(self.callback, self, self.options)
  return true
end

return KeyValue
