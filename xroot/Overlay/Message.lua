local app = app
local Timer = require "Timer"
local Manager = require "Overlay.Manager"

local lastMainTimerHandle = nil
local lastSubTimerHandle = nil

local mainFlashLabel = app.Label("...", 10)
mainFlashLabel:setBorder(1)
mainFlashLabel:setCornerRadius(3, 3, 3, 3)
mainFlashLabel:setOpaque(true)

local subFlashLabel = app.Label("...", 10)
subFlashLabel:setBorder(1)
subFlashLabel:setCornerRadius(3, 3, 3, 3)
subFlashLabel:setOpaque(true)

local mainLabel = app.Label("...", 10)
mainLabel:setBorder(1)
mainLabel:setCornerRadius(3, 3, 3, 3)
mainLabel:setOpaque(true)

local showingMainLabel = false

local function startMainMessage(...)
  local text = string.format(...)

  mainLabel:setText(text)
  mainLabel:fitToText(4)
  if lastMainTimerHandle then
    mainLabel:setCenter(128, 32 + 6)
    mainFlashLabel:setCenter(128, 32 - 6)
  else
    mainLabel:setCenter(128, 32)
  end

  Manager.addMainGraphic(mainLabel)
  showingMainLabel = true
end

local function endMainMessage()
  Manager.removeMainGraphic(mainLabel)
  showingMainLabel = false
  if lastMainTimerHandle then mainFlashLabel:setCenter(128, 32) end
end

local function endflashMainMessage()
  Manager.removeMainGraphic(mainFlashLabel)
  if lastMainTimerHandle then
    Timer.cancel(lastMainTimerHandle)
    lastMainTimerHandle = nil
  end
  if showingMainLabel then mainLabel:setCenter(128, 32) end
end

local function flashMainMessage(...)
  local text = string.format(...)

  mainFlashLabel:setText(text)
  mainFlashLabel:fitToText(4)
  mainFlashLabel:setCenter(128, 32)
  if showingMainLabel then
    mainLabel:setCenter(128, 32 + 6)
    mainFlashLabel:setCenter(128, 32 - 6)
  else
    mainFlashLabel:setCenter(128, 32)
  end

  Manager.addMainGraphic(mainFlashLabel)

  if lastMainTimerHandle then
    Timer.cancel(lastMainTimerHandle)
    lastMainTimerHandle = nil
  end

  lastMainTimerHandle = Timer.after(1, endflashMainMessage)
end

local function endflashSubMessage()
  Manager.removeSubGraphic(subFlashLabel)
  if lastSubTimerHandle then
    Timer.cancel(lastSubTimerHandle)
    lastSubTimerHandle = nil
  end
end

local function flashSubMessage(...)
  local text = string.format(...)

  subFlashLabel:setText(text)
  subFlashLabel:fitToText(4)
  subFlashLabel:setCenter(64, 32)

  Manager.addSubGraphic(subFlashLabel)

  if lastSubTimerHandle then
    Timer.cancel(lastSubTimerHandle)
    lastSubTimerHandle = nil
  end

  lastSubTimerHandle = Timer.after(1, endflashSubMessage)
end

local function closeAll()
  if lastMainTimerHandle then
    Timer.cancel(lastMainTimerHandle)
    lastMainTimerHandle = nil
    endflashMainMessage()
  end
  if lastSubTimerHandle then
    Timer.cancel(lastSubTimerHandle)
    lastSubTimerHandle = nil
    endflashSubMessage()
  end
  endMainMessage()
end

return {
  startMainMessage = startMainMessage,
  endMainMessage = endMainMessage,
  flashMainMessage = flashMainMessage,
  flashSubMessage = flashSubMessage,
  closeAll = closeAll,
}
