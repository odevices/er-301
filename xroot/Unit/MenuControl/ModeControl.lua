local app = app
local Class = require "Base.Class"
local MenuControl = require "Unit.MenuControl"
local ply = app.SECTION_PLY

-- ModeControl Class
local ModeControl = Class {}
ModeControl:include(MenuControl)

function ModeControl:init(args)
  MenuControl.init(self)
  self:setClassName("UnitMenu.ModeControl")
  -- required arguments
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  self:setInstanceName(description)
  local choices = args.choices or
                      app.logError("%s.init: 'choices' is missing.", self)
  local callback = args.callback or
                       app.logError("%s.init: 'callback' is missing.", self)
  local current = args.current or
                      app.logError("%s.init: 'current' is missing.", self)
  local descWidth = args.descriptionWidth or 1

  self.description = description
  self.choices = choices
  self.onUpdate = callback
  self.muteOnChange = args.muteOnChange
  self.descWidth = descWidth

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
  self:update(current)
end

function ModeControl:onReleased(i, shifted)
  if shifted then
    return false
  end
  if i > self.descWidth then
    local j = i - self.descWidth
    if j <= #self.choices then
      local choice = self.choices[j]
      if self.muteOnChange then
        local chain = self.parent.unit.chain
        local wasMuted = chain:muteIfNeeded()
        if self.onUpdate then
          self.onUpdate(choice)
        end
        chain:unmuteIfNeeded(wasMuted)
      else
        if self.onUpdate then
          self.onUpdate(choice)
        end
      end
      self:update(choice)
    end
  end
  return true
end

function ModeControl:update(choice)
  for choice2, label in pairs(self.labels) do
    if choice == choice2 then
      label:setForegroundColor(app.WHITE)
    else
      label:setForegroundColor(app.GRAY5)
    end
  end
end

return ModeControl
