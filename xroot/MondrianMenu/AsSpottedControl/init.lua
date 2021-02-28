local Env = require "Env"
local SpottedControl = require "SpottedStrip.Control"
local Class = require "Base.Class"
local ply = app.SECTION_PLY

local MondrianMenu = Class {}
MondrianMenu:include(SpottedControl)

function MondrianMenu:init(left, bottom, width, height)
  SpottedControl.init(self)
  self:setClassName("MondrianMenu.AsSpottedControl")
  self:addSpotDescriptor{
    center = 0.5 * width,
    radius = width
  }
  self.mlist = app.MondrianList(left, bottom, width, height)
  self.mlist:setPadding(1, 1)
  self.mlist:setCornerRadius(5, 5, 5, 5)
  self:setControlGraphic(self.mlist)
  self:setMainCursorController(self.mlist)
  self.controls = {}
end

function MondrianMenu:invalidateLayout()
  self.mlist:invalidateLayout()
end

function MondrianMenu:clear()
  self.mlist:clear()
  self.controls = {}
end

function MondrianMenu:addText(...)
  local text = string.format(...)
  local graphic = app.FittedTextBox(text)
  self.mlist:addGraphic(graphic)
end

function MondrianMenu:addHeaderText(...)
  self.mlist:startNewRow()
  self.mlist:beginSelectableOff()
  self.mlist:beginJustifyLeft()
  local text = string.format(...)
  local graphic = app.Label(text)
  self.mlist:addGraphic(graphic)
  self.mlist:startNewRow()
  self.mlist:endJustify()
  self.mlist:endSelectable()
end

function MondrianMenu:addControl(control, isHeader)
  self:addChildWidget(control)
  if control.controlGraphic then
    if isHeader then
      self.mlist:startNewRow()
      self.mlist:beginSelectableOff()
      self.mlist:beginJustifyLeft()
    end
    self.mlist:addGraphic(control.controlGraphic)
    self.controls[control.controlGraphic:handle()] = control
    if isHeader then
      self.mlist:startNewRow()
      self.mlist:endJustify()
      self.mlist:endSelectable()
    end
  end
end

function MondrianMenu:startNewRow()
  self.mlist:startNewRow()
end

function MondrianMenu:beginSelectable(value)
  if value then
    self.mlist:beginSelectableOn()
  else
    self.mlist:beginSelectableOff()
  end
end

function MondrianMenu:endSelectable()
  self.mlist:endSelectable()
end

function MondrianMenu:beginJustify(value)
  if value == "left" then
    self.mlist:beginJustifyLeft()
  elseif value == "center" then
    self.mlist:beginJustifyCenter()
  else
    self.mlist:beginJustifyRight()
  end
end

function MondrianMenu:endJustify()
  self.mlist:endJustify()
end

local threshold = Env.EncoderThreshold.Default
function MondrianMenu:encoder(change, shifted)
  self.mlist:encoder(change, shifted, threshold)
  return true
end

function MondrianMenu:upReleased(shifted)
  if not shifted then self:unfocus() end
  return true
end

function MondrianMenu:mainPressed(i, shifted)
  if i == 6 then
    self:unfocus()
    return false
  end
  if not self.focused then return true end
  local graphic = self.mlist:selectByButton(i - 1)
  if graphic then
    graphic:setBorderColor(app.WHITE)
    local control = self.controls[graphic:handle()]
    if control then
      for j = 1, 6 do
        if graphic:intersectsWithButton(j - 1) then
          if control.enabled then
            control:onPressed(i - j + 1, shifted)
          else
            control:onPressedWhenDisabled(i - j + 1, shifted)
          end
          return true
        end
      end
    end
  end
  return true
end

function MondrianMenu:mainRepeated(i, shifted)
  local graphic = self.mlist:selectByButton(i - 1)
  if graphic then
    local control = self.controls[graphic:handle()]
    if control then
      for j = 1, 6 do
        if graphic:intersectsWithButton(j - 1) then
          if control.enabled then
            control:onRepeated(i - j + 1, shifted)
          end
          return true
        end
      end
    end
  end
  return true
end

function MondrianMenu:mainReleased(i, shifted)
  if i == 6 then
    self:unfocus()
    return false
  end
  if not self.focused and i < 6 then
    self:focus()
    return false
  end
  local graphic = self.mlist:selectByButton(i - 1)
  if graphic then
    graphic:setBorderColor(app.GRAY7)
    local control = self.controls[graphic:handle()]
    if control then
      for j = 1, 6 do
        if graphic:intersectsWithButton(j - 1) then
          if control.enabled then
            control:onReleased(i - j + 1, shifted)
          else
            control:onReleasedWhenDisabled(i - j + 1, shifted)
          end
          return true
        end
      end
    end
  end
  return true
end

function MondrianMenu:onCursorEnter(spot)
  self:grabFocus("mainPressed", "mainReleased")
end

function MondrianMenu:onCursorLeave(spot)
  self:releaseFocus("mainPressed", "mainReleased", "upReleased", "encoder")
  self:disableHighlight()
end

function MondrianMenu:focus()
  self:grabFocus("encoder", "upReleased")
  self:enableHighlight()
end

function MondrianMenu:unfocus()
  self:releaseFocus("encoder", "upReleased")
  self:disableHighlight()
end

function MondrianMenu:enableHighlight()
  self.controlGraphic:setBorder(1)
  self.focused = true
end

function MondrianMenu:disableHighlight()
  self.controlGraphic:setBorder(0)
  self.focused = false
end

return MondrianMenu
