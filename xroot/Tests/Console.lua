local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Signal = require "Signal"
local Window = require "Base.Window"
local Busy = require "Busy"
local Tests = require "Tests"

local Console = Class {}
Console:include(Window)

function Console:init()
  Window.init(self)
  self:setClassName("Card.Console")

  local console = app.ListBox(0, 0, 256, 64)
  console:setTextSize(10)
  console:setFocus()
  self.mainGraphic:addChild(console)
  self.console = console

  local list = app.SlidingList(0, 0, 128, 64)
  list:setTextSize(10)
  list:setJustification(app.justifyLeft)
  self.subGraphic:addChild(list)
  self.list = list

  local tests = Tests.getAll()
  list:add("Run All Batch Tests")
  for _, test in ipairs(tests) do
    if test.batch then
      list:add(test.description)
    else
      list:add("(ux) " .. test.description)
    end
  end

  self:msg("Welcome to the Test Console.")
  self:msg("Select test with knob and press ENTER.")
end

function Console:msg(...)
  local text = string.format(...)
  self.console:addItem(text)
  self.console:scrollToBottom()
end

local threshold = Env.EncoderThreshold.Default
function Console:encoder(change, shifted)
  if self.hold then
    self.console:encoder(change, shifted, threshold)
  else
    self.list:encoder(change, shifted, threshold)
  end
  return true
end

function Console:mainPressed(i, shifted)
  self.hold = true
  return true
end

function Console:mainReleased(i, shifted)
  self.hold = false
  return true
end

function Console:upReleased()
  self:hide()
  return true
end

function Console:cancelReleased()
  self:hide()
  return true
end

function Console:homeReleased()
  self:hide()
  return true
end

function Console:logInfo(text)
  self.console:addItem(text)
  self.console:scrollToBottom()
end

function Console:logWarn(text)
  self.console:addItem("WARN " .. text)
  self.console:scrollToBottom()
end

function Console:logError(text)
  self.console:addItem("ERROR " .. text)
  self.console:scrollToBottom()
end

function Console:enterReleased()
  local i = self.list:selectedIndex()
  Signal.weakRegister("logInfo", self)
  Signal.weakRegister("logWarn", self)
  Signal.weakRegister("logError", self)
  Busy.start()
  if i == 0 then
    Tests.runAll()
  else
    Tests.run(i)
  end
  Busy.stop()
  Signal.remove("logInfo", self)
  Signal.remove("logWarn", self)
  Signal.remove("logError", self)
end

return Console
