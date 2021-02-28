local app = app
local Window = require "Base.Window"
local Timer = require "Timer"

local window = Window()
window:setInstanceName("LoadView")

local memUnit = 1024
local ramMax = app.Heap_getSize(memUnit)

local ram = app.StripChart(0, 0, 128, 64)
ram:setBorder(1)
ram:setCornerRadius(5, 5, 5, 5)
ram:setRange(0, ramMax)

local cpu = app.StripChart(128, 0, 128, 64)
cpu:setBorder(1)
cpu:setCornerRadius(5, 5, 5, 5)
cpu:setRange(0, 100)

window:addMainGraphic(ram)
window:addMainGraphic(cpu)

local label1 = app.Label("--------------", 12)
label1:setPosition(10, app.GRID5_LINE1)
local label2 = app.Label("--------------", 12)
label2:setPosition(10, app.GRID5_LINE2)
local label3 = app.Label("--------------", 12)
label3:setPosition(10, app.GRID5_LINE3)
local label4 = app.Label("--------------", 12)
label4:setPosition(10, app.GRID5_LINE4)

window:addSubGraphic(label1)
window:addSubGraphic(label2)
window:addSubGraphic(label3)
window:addSubGraphic(label4)

local label = app.Label("Kernel Memory", 10)
label:setPosition(10, app.GRID4_LINE1)
window:addMainGraphic(label)

label = app.Label("Critical Load", 10)
label:setPosition(128 + 10, app.GRID4_LINE1)
window:addMainGraphic(label)

local ramUsed = 0
local cpuLoad = 0

local foregroundTimer = nil
local function onForegroundTimer()
  label1:setText(string.format("CPU:   %5d%%", cpuLoad))
  label2:setText(string.format("used:  %5dKB", ramUsed))
  label3:setText(string.format("lua:   %5dKB", math.ceil(app.luaMemoryUsage())))
  label4:setText(string.format("total: %5dKB", ramMax))
  return true
end

local function onBackgroundTimer()
  ramUsed = ramMax - app.Heap_getFreeSize(memUnit)
  ram:addValue(ramUsed)
  cpuLoad = app.Audio_getLoad()
  cpu:addValue(cpuLoad)
  return true
end

function window:upReleased()
  self:hide()
  return true
end

function window:homeReleased()
  self:hide()
  return true
end

function window:cancelReleased()
  self:hide()
  return true
end

function window:onShow()
  foregroundTimer = Timer.every(1, onForegroundTimer)
  onForegroundTimer()
end

function window:onHide()
  Timer.cancel(foregroundTimer)
end

Timer.every(1, onBackgroundTimer)

return window
