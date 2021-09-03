local app = app
local Timer = require "Timer"

-- create system graphic objects
local pMainOverlay = app.Graphic(0, 0, 256, 64)
app.UIThread.setMainOverlay(pMainOverlay)
local pSubOverlay = app.Graphic(0, 0, 128, 64)
app.UIThread.setSubOverlay(pSubOverlay)

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

  if not pMainOverlay:contains(mainLabel) then
    pMainOverlay:addChild(mainLabel)
    showingMainLabel = true
  end
end

local function endMainMessage()
  pMainOverlay:removeChild(mainLabel)
  showingMainLabel = false
  if lastMainTimerHandle then mainFlashLabel:setCenter(128, 32) end
end

local function endMainFlashMessage()
  pMainOverlay:removeChild(mainFlashLabel)
  if lastMainTimerHandle then
    Timer.cancel(lastMainTimerHandle)
    lastMainTimerHandle = nil
  end
  if showingMainLabel then mainLabel:setCenter(128, 32) end
end

local function mainFlashMessage(...)
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

  if not pMainOverlay:contains(mainFlashLabel) then
    pMainOverlay:addChild(mainFlashLabel)
  end

  if lastMainTimerHandle then
    Timer.cancel(lastMainTimerHandle)
    lastMainTimerHandle = nil
  end

  lastMainTimerHandle = Timer.after(1, endMainFlashMessage)
end

local function endSubFlashMessage()
  pSubOverlay:removeChild(subFlashLabel)
  if lastSubTimerHandle then
    Timer.cancel(lastSubTimerHandle)
    lastSubTimerHandle = nil
  end
end

local function subFlashMessage(...)
  local text = string.format(...)

  subFlashLabel:setText(text)
  subFlashLabel:fitToText(4)
  subFlashLabel:setCenter(64, 32)

  if not pSubOverlay:contains(subFlashLabel) then
    pSubOverlay:addChild(subFlashLabel)
  end

  if lastSubTimerHandle then
    Timer.cancel(lastSubTimerHandle)
    lastSubTimerHandle = nil
  end

  lastSubTimerHandle = Timer.after(1, endSubFlashMessage)
end

local floatingMenu = app.MenuArc()
local floatingMenuTimer
local floatingMenuObject

local function getMenuItemsAndDefault()
  local choices = { "cancel" }
  local defaultIndex = #choices + 1

  if floatingMenuObject then
    -- First get the default menu choice
    local defaultChoice
    if floatingMenuObject.getFloatingMenuDefaultChoice then
      defaultChoice = floatingMenuObject:getFloatingMenuDefaultChoice()
    end

    -- Now get the item list and set the default index
    if floatingMenuObject.getFloatingMenuItems then
      local items = floatingMenuObject:getFloatingMenuItems() or {}
      for i, item in ipairs(items) do
        local nextIndex = #choices + 1
        choices[nextIndex] = item
        if item == defaultChoice then defaultIndex = nextIndex end
      end
    end
  end

  return choices, defaultIndex
end

local function fireMenuEnter()
  if floatingMenuObject and floatingMenuObject.onFloatingMenuEnter then
    floatingMenuObject:onFloatingMenuEnter()
  end
end

local function fireMenuChange(choice)
  if floatingMenuObject and floatingMenuObject.onFloatingMenuChange then
    floatingMenuObject:onFloatingMenuChange(choice)
  end
end

local function fireMenuSelection(choice)
  if floatingMenuObject and floatingMenuObject.onFloatingMenuSelection then
    floatingMenuObject:onFloatingMenuSelection(choice)
  end
end

local function selectFloatingMenu(choice)
  floatingMenu:select(choice)
end

local function endFloatingMenu()
  local Application = require "Application"
  Application.setDispatcher()
  if floatingMenuTimer then
    Timer.cancel(floatingMenuTimer)
    floatingMenuTimer = nil
  end

  local choice
  if floatingMenu:hasParent() then
    pMainOverlay:removeChild(floatingMenu)
    choice = floatingMenu:selectedText()
    floatingMenu:clear()
    fireMenuSelection(choice)
  end

  floatingMenuObject = nil
  return choice
end

local function ignoreMainRelease(i)
  local Application = require "Application"
  if i == 1 then
    Application.ignoreEvent(app.EVENT_RELEASE_MAIN1)
  elseif i == 2 then
    Application.ignoreEvent(app.EVENT_RELEASE_MAIN2)
  elseif i == 3 then
    Application.ignoreEvent(app.EVENT_RELEASE_MAIN3)
  elseif i == 4 then
    Application.ignoreEvent(app.EVENT_RELEASE_MAIN4)
  elseif i == 5 then
    Application.ignoreEvent(app.EVENT_RELEASE_MAIN5)
  elseif i == 6 then
    Application.ignoreEvent(app.EVENT_RELEASE_MAIN6)
  end
end

local function dispatcherFloatingMenu(event)
  -- app.logInfo("Overlay.dispatcherFloatingMenu(%s)",event)
  local Application = require "Application"
  local m = Application.getLastMainButtonPressed()
  if event == app.EVENT_KNOB then
    local p = app.Encoder_getChange()
    if p ~= 0 then
      local Env = require "Env"
      local threshold = Env.EncoderThreshold.Default
      local changed = floatingMenu:encoder(p, false, threshold)
      if changed then fireMenuChange(floatingMenu:selectedText()) end
    end
  elseif event == app.EVENT_RELEASE_MAIN1 and m == 1 then
    Application.notify("mainReleased", 1, false)
  elseif event == app.EVENT_RELEASE_MAIN2 and m == 2 then
    Application.notify("mainReleased", 2, false)
  elseif event == app.EVENT_RELEASE_MAIN3 and m == 3 then
    Application.notify("mainReleased", 3, false)
  elseif event == app.EVENT_RELEASE_MAIN4 and m == 4 then
    Application.notify("mainReleased", 4, false)
  elseif event == app.EVENT_RELEASE_MAIN5 and m == 5 then
    Application.notify("mainReleased", 5, false)
  elseif event == app.EVENT_RELEASE_MAIN6 and m == 6 then
    Application.notify("mainReleased", 6, false)
  elseif event == app.EVENT_MODE then
    endFloatingMenu()
    Application.defaultDispatcher(event)
    ignoreMainRelease(m)
  elseif event == app.EVENT_STORAGE then
    endFloatingMenu()
    Application.defaultDispatcher(event)
    ignoreMainRelease(m)
  end
end

local function onStartFloatingMenu()
  floatingMenuTimer = nil
  local choices, defaultIndex = getMenuItemsAndDefault()

  if #choices > 1 then
    for _, choice in ipairs(choices) do floatingMenu:add(choice) end
    floatingMenu:select(defaultIndex - 1)
    pMainOverlay:addChildOnce(floatingMenu)
  end

  local Application = require "Application"
  Application.setDispatcher(dispatcherFloatingMenu)

  fireMenuEnter()
end

local function startFloatingMenu(o)
  pMainOverlay:removeChild(floatingMenu)
  floatingMenu:clear()
  if floatingMenuTimer then
    Timer.cancel(floatingMenuTimer)
    floatingMenuTimer = nil
  end
  floatingMenuObject = o
  local Application = require "Application"
  local x = app.getButtonCenter(Application.getLastMainButtonPressed())
  floatingMenu:setAnchor(x)
  floatingMenuTimer = Timer.after(0.3, onStartFloatingMenu)
end

local function clearAll()
  if lastMainTimerHandle then
    Timer.cancel(lastMainTimerHandle)
    lastMainTimerHandle = nil
    endMainFlashMessage()
  end
  if lastSubTimerHandle then
    Timer.cancel(lastSubTimerHandle)
    lastSubTimerHandle = nil
    endSubFlashMessage()
  end
  endMainMessage()
  endFloatingMenu()
end

local function addMainGraphic(graphic)
  if graphic then pMainOverlay:addChildOnce(graphic) end
end

local function removeMainGraphic(graphic)
  if graphic then pMainOverlay:removeChild(graphic) end
end

local function addSubGraphic(graphic)
  if graphic then pSubOverlay:addChildOnce(graphic) end
end

local function removeSubGraphic(graphic)
  if graphic then pSubOverlay:removeChild(graphic) end
end

local holdGraphic = app.Label("hold", 10)
holdGraphic:setOpaque(true)
holdGraphic:setPosition(6, 48)
holdGraphic:setBorder(1)
holdGraphic:setCornerRadius(3, 3, 3, 3)
local function enterHoldMode()
  addMainGraphic(holdGraphic)
end

local function leaveHoldMode()
  removeMainGraphic(holdGraphic)
end

return {
  startMainMessage = startMainMessage,
  endMainMessage = endMainMessage,
  mainFlashMessage = mainFlashMessage,
  subFlashMessage = subFlashMessage,
  clearAll = clearAll,
  addMainGraphic = addMainGraphic,
  removeMainGraphic = removeMainGraphic,
  addSubGraphic = addSubGraphic,
  removeSubGraphic = removeSubGraphic,
  startFloatingMenu = startFloatingMenu,
  selectFloatingMenu = selectFloatingMenu,
  endFloatingMenu = endFloatingMenu,
  enterHoldMode = enterHoldMode,
  leaveHoldMode = leaveHoldMode
}
