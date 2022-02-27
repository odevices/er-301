local app = app
local Signal = require "Signal"
local Class = require "Base.Class"
local Widget = require "Base.Widget"

local Window = Class {}
Window:include(Widget)

-- a window is a widget with no parent
-- it also has main and sub graphics that fill the displays
function Window:init(mainGraphic, subGraphic)
  Widget.init(self)
  self:setClassName("Window")
  self.isWindow = true
  self.context = nil
  self.visible = false
  self.focus = {}
  self.mainGraphic = mainGraphic or app.Graphic(0, 0, 256, 64)
  self.subGraphic = subGraphic or app.Graphic(0, 0, 128, 64)
  self.isCursorVisible = true
end

function Window:show(context)
  if context == nil then
    local Application = require "Application"
    context = Application:getVisibleContext()
  end
  if context then
    context:add(self)
  end
end

function Window:hide()
  local context = self.context
  if context then
    context:remove(self)
  end
end

function Window:replace(otherWindow)
  otherWindow:hide()
  if self.context then
    self.context:replace(self, otherWindow)
  end
end

function Window:hideOthers()
  local context = self.context
  if context then
    while context:top() ~= self do
      context:remove(context:top())
    end
  end
end

function Window:hideCursor()
  self.isCursorVisible = false
  if self.context then
    self.context:onEncoderFocusChanged()
  end
end

function Window:showCursor()
  self.isCursorVisible = true
  if self.context then
    self.context:onEncoderFocusChanged()
  end
end

function Window:setFocusedWidget(event, target)
  self.focus[event] = target
  if event == "encoder" and self.context then
    self.context:onEncoderFocusChanged()
  end
end

function Window:getFocusedWidget(event)
  return self.focus["all"] or self.focus[event] or self
end

function Window:addMainGraphic(graphic)
  if graphic then
    self.mainGraphic:addChildOnce(graphic)
  end
end

function Window:removeMainGraphic(graphic)
  if graphic then
    self.mainGraphic:removeChild(graphic)
  end
end

function Window:addSubGraphic(graphic)
  if graphic then
    self.subGraphic:addChildOnce(graphic)
  end
end

function Window:removeSubGraphic(graphic)
  if graphic then
    self.subGraphic:removeChild(graphic)
  end
end

return Window
