local app = app
local Class = require "Base.Class"
local MenuControl = require "Unit.MenuControl"
local ply = app.SECTION_PLY

-- FlagSelect Class
local FlagSelect = Class {}
FlagSelect:include(MenuControl)

function FlagSelect:init(args)
  MenuControl.init(self)
  self:setClassName("UnitMenu.FlagSelect")
  -- required arguments
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  self:setInstanceName(description)
  local option = args.option or
                     app.logError("%s.init: option is missing.", self)
  local flags = args.flags or app.logError("%s.init: 'flags' is missing.", self)
  local descWidth = args.descriptionWidth or 1

  option:enableSerialization()

  self.description = description
  self.flags = flags
  self.option = option
  self.onUpdate = args.onUpdate
  self.descWidth = descWidth

  local graphic = app.RichTextBox(description, 10)
  graphic:setBorder(1)
  graphic:setBorderColor(app.GRAY7)
  graphic:setCornerRadius(3, 3, 3, 3)
  graphic:setMargins(3, 1, #flags * ply, 1)
  graphic:fitHeight((#flags + descWidth) * ply - 2)

  local h = graphic.mHeight // 2
  local labels = {}
  for i, flag in ipairs(flags) do
    local label = app.RichTextBox(flag, 10)
    label:fitHeight(ply - 2)
    labels[flag] = label
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

function FlagSelect:onReleased(i, shifted)
  if shifted then
    return false
  end
  if i > self.descWidth then
    local flag = i - self.descWidth - 1
    if flag < #self.flags then
      self.option:toggleFlag(flag)
      self:update()
    end
  end
  return true
end

function FlagSelect:update()
  for flag, name in ipairs(self.flags) do
    local label = self.labels[name]
    if self.option:getFlag(flag - 1) then
      label:setForegroundColor(app.WHITE)
    else
      label:setForegroundColor(app.GRAY5)
    end
  end
  if self.onUpdate then
    self.onUpdate(self.option)
  end
end

return FlagSelect
