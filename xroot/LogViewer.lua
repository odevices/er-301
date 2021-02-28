local Env = require "Env"
local Signal = require "Signal"
local Window = require "Base.Window"
local threshold = Env.EncoderThreshold.Default

app.logInfo("---------------")
app.logInfo("ER-301 (%s,%s)", Env.Version.String, Env.Version.Name)
app.logInfo("Sound Computer")
app.logInfo("---------------")

local window = Window()
window:setInstanceName("LogViewer")
window.zOrder = -1001
local console = app.ListBox(0, 0, 256, 64)
console:setTextSize(10)
console:setFocus()
local label = app.Label("Logs", 12)
label:setCenter(64, 32)
window:addMainGraphic(console)
window:addSubGraphic(label)

function window:encoder(change, shifted)
  console:encoder(change, shifted, threshold)
  return true
end

function window:count()
  return console:size()
end

function window:get()
  return console:getSelected()
end

function window:scrollToTop()
  console:scrollToTop()
end

function window:scrollDown()
  console:scrollDown()
end

function window:upReleased()
  self:hide()
  return true
end

function window:cancelReleased()
  self:hide()
  return true
end

local function log(text)
  console:addItem(text)
  if console:size() > 64 then console:removeTopItem() end
  if window.active then console:scrollToBottom() end
end

Signal.register("log", log)

return window
