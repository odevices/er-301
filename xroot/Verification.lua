local app = app
local Class = require "Base.Class"
local Window = require "Base.Window"
local Timer = require "Timer"

-- Main
local MainVerification = Class {}
MainVerification:include(Window)

function MainVerification:init(msg1, msg2, fontSize1, fontSize2)
  Window.init(self)

  self:setClassName("MainVerification")
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
    self.msg1 = msg1
    label = app.Label(msg1, fontSize1 or 12)
    self.label1 = label
    label:setCenter(128, line1)
    self.mainGraphic:addChild(label)
  end
  if msg2 then
    label = app.Label(msg2, fontSize2 or fontSize1 or 12)
    label:setCenter(128, line2)
    self.mainGraphic:addChild(label)
  end
  app.addButtons(self.mainGraphic, "", "yes", "", "", "no", "")
end

function MainVerification:onTimer(remaining, ans)
  if self.msg1 then
    local text = string.format(self.msg1, remaining)
    self.label1:setText(text)
  end
  if remaining > 0 then
    self.timer = Timer.after(1, function()
      self:onTimer(remaining - 1, ans)
    end)
  else
    self:hide()
    self:emitSignal("done", ans)
  end
end

function MainVerification:setTimer(secs, ans)
  self:onTimer(secs, ans)
end

function MainVerification:finish(ans)
  if self.timer then Timer.cancel(self.timer) end
  self:hide()
  self:emitSignal("done", ans)
end

function MainVerification:cancelReleased(shifted)
  self:finish(nil)
  return true
end

function MainVerification:mainReleased(i, shifted)
  if not shifted then
    if i == 2 then
      self:finish(true)
    elseif i == 5 then
      self:finish(false)
    end
  end
  return true
end

-- Sub
local SubVerification = Class {}
SubVerification:include(Window)

function SubVerification:init(msg1, msg2)
  local Application = require "Application"
  local context = Application.getVisibleContext()
  -- continue showing the top window's main graphic
  Window.init(self, context:top().mainGraphic)
  self:setClassName("SubVerification")
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
  app.addButtons(self.subGraphic, "yes", "", "no")
end

function SubVerification:cancelReleased()
  self:hide()
  self:emitSignal("done", nil)
  return true
end

function SubVerification:subReleased(i, shifted)
  if not shifted then
    if i == 1 then
      self:hide()
      self:emitSignal("done", true)
    elseif i == 3 then
      self:hide()
      self:emitSignal("done", false)
    end
  end
  return true
end

return {
  Main = MainVerification,
  Sub = SubVerification
}
