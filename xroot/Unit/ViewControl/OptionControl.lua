local app = app
local Drawings = require "Drawings"
local Utils = require "Utils"
local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"

local ply = app.SECTION_PLY
local line1 = app.GRID5_LINE1
local line2 = app.GRID5_LINE2
local line3 = app.GRID5_LINE3
local line4 = app.GRID5_LINE4

local Item = Class {}
local defaultTxt = "      "

function Item:init(name, choice)
  local graphic = app.Graphic(0, 0, ply, 64)
  self.graphic = graphic

  local txt1, txt2, txt3
  if choice then txt1, txt2, txt3 = table.unpack(Utils.split(choice, " ")) end

  local label = app.Label(txt1 or defaultTxt, 10)
  graphic:addChild(label)
  label:setPosition(0, line2)
  label:setSize(ply, label.mHeight)
  self.label1 = label

  label = app.Label(txt2 or defaultTxt, 10)
  graphic:addChild(label)
  label:setPosition(0, line3)
  label:setSize(ply, label.mHeight)
  self.label2 = label

  label = app.Label(txt3 or defaultTxt, 10)
  graphic:addChild(label)
  label:setPosition(0, line4)
  label:setSize(ply, label.mHeight)
  self.label3 = label

  label = app.MainButton(name, 1)
  graphic:addChild(label)
end

function Item:set(choice)
  local words = Utils.split(choice, " ")
  self.label1:setText(words[1] or "")
  self.label2:setText(words[2] or "")
  self.label3:setText(words[3] or "")
end

-- OptionControl Class
local OptionControl = Class {
  type = "Option",
  canEdit = false,
  canMove = true
}
OptionControl:include(ViewControl)

function OptionControl:init(args)
  ViewControl.init(self)
  self:setClassName("Unit.ViewControl.OptionControl")
  -- required arguments
  local button = args.button or app.logError("%s.init: button is missing.", self)
  self:setInstanceName(button)
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  local option = args.option or app.logError("%s.init: option is missing.", self)
  local choices = args.choices or
                      app.logError("%s.init: 'choices' is missing.", self)

  option:enableSerialization()

  self.description = description
  self.choices = choices
  self.option = option
  self.onUpdate = args.onUpdate
  self.muteOnChange = args.muteOnChange
  if args.boolean then
    self.offset = 1
  else
    self.offset = 0
  end

  local activeChoice = option:value()
  local text = string.format("%s: %s", description, choices[activeChoice])
  self.activeItem = Item("", text)
  self:setControlGraphic(self.activeItem.graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }

  self.subGraphic = app.Graphic(0, 0, 128, 64)
  local items = {}
  for i, choice in ipairs(choices) do
    items[i] = Item(string.format(" %d ", i), choice)
    self.subGraphic:addChild(items[i].graphic)
    items[i].graphic:setCenter(app.getButtonCenter(i), 32)
  end
  self.items = items

  -- vertical lines
  local drawing = app.Drawing(0, 0, 128, 64)
  drawing:add(Drawings.Sub.ThreeColumns)
  self.subGraphic:addChild(drawing)

end

function OptionControl:subReleased(i, shifted)
  if shifted then return false end
  if self.muteOnChange then
    local chain = self.parent.chain
    local wasMuted = chain:muteIfNeeded()
    self.option:set(i - self.offset)
    self:update()
    chain:unmuteIfNeeded(wasMuted)
  else
    self.option:set(i - self.offset)
    self:update()
  end
  return true
end

function OptionControl:update()
  local choice = self.choices[self.option:value() + self.offset]
  if choice then
    local text = string.format("%s: %s", self.description, choice)
    self.activeItem:set(text)
  end
  if self.onUpdate then self.onUpdate(choice) end
end

return OptionControl
