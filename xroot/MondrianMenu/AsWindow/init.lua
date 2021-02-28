local Env = require "Env"
local Window = require "Base.Window"
local Class = require "Base.Class"

local MondrianMenu = Class {}
MondrianMenu:include(Window)

function MondrianMenu:init()
  Window.init(self)
  self:setClassName("MondrianMenu.AsWindow")

  self.mlist = app.MondrianList(0, 0, 256, 64)
  self.mlist:setPadding(1, 1)
  self:addMainGraphic(self.mlist)
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
end

function MondrianMenu:cancelReleased(shifted)
  if shifted then return false end
  self:hide()
  self:emitSignal("done")
  return true
end

function MondrianMenu:upReleased(shifted)
  if shifted then return false end
  self:hide()
  self:emitSignal("done")
  return true
end

function MondrianMenu:homeReleased()
  self.mlist:scrollToTop()
  return true
end

function MondrianMenu:mainPressed(i, shifted)
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

return MondrianMenu
