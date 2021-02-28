local app = app
local Class = require "Base.Class"
local Window = require "Base.Window"

-- Main
local MainMessage = Class {}
MainMessage:include(Window)

function MainMessage:init(msg1, msg2)
  Window.init(self)
  self:setClassName("MainMessage")

  local label
  local line1, line2
  if msg1 and msg2 then
    line1 = 50
    line2 = 34
  else
    line1 = 42
    line2 = 42
  end
  if msg1 then
    local textWidth = app.getTextWidth(msg1, 12)
    if textWidth > 255 then
      label = app.Label(msg1, 10)
    else
      label = app.Label(msg1, 12)
    end
    label:setCenter(128, line1)
    self.mainGraphic:addChild(label)
  end
  if msg2 then
    local textWidth = app.getTextWidth(msg2, 12)
    if textWidth > 255 then
      label = app.Label(msg2, 10)
    else
      label = app.Label(msg2, 12)
    end
    label:setCenter(128, line2)
    self.mainGraphic:addChild(label)
  end
  app.addButtons(self.mainGraphic, "", "", "", "ok", "", "")
end

function MainMessage:mainReleased()
  self:hide()
  self:emitSignal("done")
  return true
end

function MainMessage:enterReleased()
  self:hide()
  self:emitSignal("done")
  return true
end

-- Sub
local SubMessage = Class {}
SubMessage:include(Window)

function SubMessage:init(msg1, msg2, includeMainGraphic)
  if includeMainGraphic then
    Window.init(self)
  else
    local Application = require "Application"
    local context = Application.getVisibleContext()
    -- continue showing the top window's main graphic
    Window.init(self, context:top().mainGraphic)
  end
  self:setClassName("SubMessage")
  local label
  local line1, line2
  if msg1 and msg2 then
    line1 = 50
    line2 = 34
  else
    line1 = 42
    line2 = 42
  end
  if msg1 then
    label = app.Label(msg1, 10)
    label:setCenter(64, line1)
    self.subGraphic:addChild(label)
  end
  if msg2 then
    label = app.Label(msg2, 10)
    label:setCenter(64, line2)
    self.subGraphic:addChild(label)
  end
  app.addButtons(self.subGraphic, "", "ok", "")
end

function SubMessage:enterReleased()
  self:hide()
  self:emitSignal("done")
  return true
end

function SubMessage:subReleased(i, shifted)
  self:hide()
  self:emitSignal("done")
  return true
end

return {
  Main = MainMessage,
  Sub = SubMessage
}
