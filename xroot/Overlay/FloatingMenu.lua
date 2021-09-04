local app = app
local Timer = require "Timer"
local Manager = require "Overlay.Manager"

-- MenuArc implements the graphics.
local menuArc = app.MenuArc()
-- A timer for timing the long press.
local timer
-- A target object to provide menu items and receive events.
local target

local function getMenuItemsAndDefault()
  local choices = { "cancel" }
  local defaultIndex = #choices + 1

  if target then
    -- First get the default menu choice
    local defaultChoice
    if target.getFloatingMenuDefaultChoice then
      defaultChoice = target:getFloatingMenuDefaultChoice()
    end

    -- Now get the item list and set the default index
    if target.getFloatingMenuItems then
      local items = target:getFloatingMenuItems() or {}
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
  if target and target.onFloatingMenuEnter then
    target:onFloatingMenuEnter()
  end
end

local function fireMenuChange(choice)
  if target and target.onFloatingMenuChange then
    target:onFloatingMenuChange(choice)
  end
end

local function fireMenuSelection(choice)
  if target and target.onFloatingMenuSelection then
    target:onFloatingMenuSelection(choice)
  end
end

local function select(choice)
  menuArc:select(choice)
end

local function close()
  local Application = require "Application"
  Application.setDispatcher()
  if timer then
    Timer.cancel(timer)
    timer = nil
  end

  local choice
  if menuArc:hasParent() then
    Manager.removeMainGraphic(menuArc)
    choice = menuArc:selectedText()
    menuArc:clear()
    fireMenuSelection(choice)
  end

  target = nil
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

local function dispatcher(event)
  -- app.logInfo("Overlay.FloatingMenu.dispatcher(%s)",event)
  local Application = require "Application"
  local m = Application.getLastMainButtonPressed()
  if event == app.EVENT_KNOB then
    local p = app.Encoder_getChange()
    if p ~= 0 then
      local Env = require "Env"
      local threshold = Env.EncoderThreshold.Default
      local changed = menuArc:encoder(p, false, threshold)
      if changed then fireMenuChange(menuArc:selectedText()) end
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
    close()
    Application.defaultDispatcher(event)
    ignoreMainRelease(m)
  elseif event == app.EVENT_STORAGE then
    close()
    Application.defaultDispatcher(event)
    ignoreMainRelease(m)
  end
end

local function onOpen()
  timer = nil
  local choices, defaultIndex = getMenuItemsAndDefault()

  if #choices > 1 then
    for _, choice in ipairs(choices) do menuArc:add(choice) end
    menuArc:select(defaultIndex - 1)
    Manager.addMainGraphic(menuArc)
  end

  local Application = require "Application"
  Application.setDispatcher(dispatcher)

  fireMenuEnter()
end

local function getDelayTime()
  local Settings = require "Settings"
  local choice = Settings.get("floatingMenuDelay")
  local values = {
    ["0.2s"] = 0.2,
    ["0.3s"] = 0.3,
    ["0.5s"] = 0.5,
   }
   return values[choice] or 0.3
end

local function open(o)
  Manager.removeMainGraphic(menuArc)
  menuArc:clear()
  if timer then
    Timer.cancel(timer)
    timer = nil
  end
  target = o
  local Application = require "Application"
  local x = app.getButtonCenter(Application.getLastMainButtonPressed())
  menuArc:setAnchor(x)
  timer = Timer.after(getDelayTime(), onOpen)
end

return {
  open = open,
  select = select,
  close = close,
}
