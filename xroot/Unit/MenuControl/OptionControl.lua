local app = app
local Class = require "Base.Class"
local MenuControl = require "Unit.MenuControl"
local ply = app.SECTION_PLY

-- OptionControl Class
local OptionControl = Class {}
OptionControl:include(MenuControl)

function OptionControl:init(args)
  MenuControl.init(self)
  self:setClassName("UnitMenu.OptionControl")
  -- required arguments
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  self:setInstanceName(description)
  local option = args.option or app.logError("%s.init: option is missing.", self)
  local choices = args.choices or
                      app.logError("%s.init: 'choices' is missing.", self)
  local descWidth = args.descriptionWidth or 1

  option:enableSerialization()

  self.description = description
  self.choices = choices
  self.option = option
  self.onUpdate = args.onUpdate
  self.muteOnChange = args.muteOnChange
  self.descWidth = descWidth
  if args.offset then
    self.offset = args.offset
  elseif args.boolean then
    self.offset = 1
  else
    self.offset = 0
  end

  local graphic = app.RichTextBox(description, 10)
  graphic:setBorder(1)
  graphic:setBorderColor(app.GRAY7)
  graphic:setCornerRadius(3, 3, 3, 3)
  graphic:setMargins(3, 1, (3 + descWidth - 1) * ply, 1)
  graphic:fitHeight((3 + descWidth) * ply - 2)
  local h = graphic.mHeight // 2
  local labels = {}
  for i, choice in ipairs(choices) do
    local label = app.RichTextBox(choice, 10)
    label:fitHeight(ply - 2)
    labels[choice] = label
    graphic:addChild(label)
    label:setForegroundColor(app.GRAY5)
    label:setJustification(app.justifyCenter)
    label:setCenter(app.getButtonCenter(i + descWidth), h)
  end

  local instructions = app.DrawingInstructions()
  instructions:color(app.GRAY7)
  instructions:vline(descWidth * ply - 2, 1, graphic.mHeight - 1)
  local drawing = app.Drawing(0, 0, 256, 64)
  drawing:add(instructions)
  graphic:addChild(drawing)

  self:setControlGraphic(graphic)
  self.labels = labels
  self:update()
end

function OptionControl:onReleased(i, shifted)
  if shifted then return false end
  if i > self.descWidth then
    local choice = i - self.descWidth - self.offset
    if choice <= #self.choices then
      if self.muteOnChange then
        local chain = self.parent.unit.chain
        local wasMuted = chain:muteIfNeeded()
        self.option:set(choice)
        self:update()
        chain:unmuteIfNeeded(wasMuted)
      else
        self.option:set(choice)
        self:update()
      end
    end
  end
  return true
end

function OptionControl:update()
  local choice = self.choices[self.option:value() + self.offset]
  for choice2, label in pairs(self.labels) do
    if choice == choice2 then
      label:setForegroundColor(app.WHITE)
    else
      label:setForegroundColor(app.GRAY5)
    end
  end
  if self.onUpdate then self.onUpdate(choice) end
end

return OptionControl
