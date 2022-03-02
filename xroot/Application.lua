-- optimize some variable lookups
local app = app
local startEventTimer = app.UIThread.startEventTimer
local stopEventTimer = app.UIThread.stopEventTimer
local waitForEvent = app.Events_wait
local pullEvent = app.Events_pull
local updateDisplay = app.UIThread.updateDisplay
local getEncoderChange = app.Encoder_getChange

local Busy = require "Busy"
local Env = require "Env"

app.logInfo("Application: pwm and display start")
app.Pwm_start();
app.Display_start();

Busy.start("Sound Computer (%s)", app.FIRMWARE_VERSION)

local Settings = require "Settings"
Settings.init()

local Signal = require "Signal"
local Encoder = require "Encoder"
local Timer = require "Timer"
local Card = require "Card"
local PackageManager = require "Package.Manager"
local USB = require "USB"

local admin = require "AdminMode"
local user = require "UserMode"
local eject = require "Card.EjectMode"
local visibleContext = nil
local activeMode = nil

local function getRestorePath()
  local FS = require "Card.FileSystem"
  local Path = require "Path"
  return Path.join(FS.getRoot("rear-versioned"),
                   string.format("restore.%s", FS.getExt("quicksave")))
end

local function restore()
  local Path = require "Path"
  local path = getRestorePath()
  if Path.exists(path) then
    local QuickSavePreset = require "Persist.QuickSavePreset"
    local preset = QuickSavePreset()
    app.logInfo("Restoring saved state from %s", path)
    Busy.start("Restoring saved state...")
    if preset:read(path) then
      preset:apply()
    else
      app.logInfo("Could not read restore data.")
    end
    Busy.stop()
    app.deleteFile(path)
    return true
  end
  return false
end

local function restart(save)
  if save then
    local Persist = require "Persist"
    local path = getRestorePath()
    app.logInfo("Saving restore state to %s", path)
    Persist.quickSaveToFile(path, true, restart)
  else
    app.logInfo("Rebooting...")
    app.reboot()
  end
end

local function getVisibleContext()
  return visibleContext
end

local function setVisibleContext(context)
  if context ~= visibleContext then
    if visibleContext then
      visibleContext:hide()
    end
    visibleContext = context
    if visibleContext then
      visibleContext:show()
    end
  end
end

local function setActiveMode(mode)
  if mode ~= activeMode then
    if activeMode then
      activeMode:leave()
    end
    activeMode = mode
    if activeMode then
      activeMode:enter()
    end
  end
end

local function onStorageChanged()
  local s = app.getStorageToggleState()
  if s == "admin" then
    setActiveMode(admin)
  elseif s == "user" then
    setActiveMode(user)
  elseif s == "eject" then
    setActiveMode(eject)
  end
end

local function doScreenShot()
  local Overlay = require "Overlay"
  if Card.mounted() then
    local Path = require "Path"
    local Persist = require "Persist"
    local fullPath = Persist.getScreenShotFilename()
    local shortPath = Path.lastFolderAndFilename(fullPath)
    local result = app.UIThread.saveScreenShotTo(fullPath)
    if result then
      Overlay.flashMainMessage("Saved screenshot to %s", shortPath)
    else
      Overlay.flashMainMessage("Failed to save screenshot to %s", shortPath)
    end
  else
    Overlay.flashMainMessage("Screenshot failed. Card not mounted.")
  end
end

-- global event handlers
local shifted = false
local releaseShifted = false

local timerDelta = 0.2
local timerUpdatePeriod = timerDelta * app.GRAPHICS_REFRESH_RATE
local timerUpdateCount = timerUpdatePeriod
local function onDisplayReady()
  if timerUpdateCount < timerUpdatePeriod then
    timerUpdateCount = timerUpdateCount + 1
  else
    -- app.logInfo("timer update")
    Timer.update(timerDelta)
    timerUpdateCount = 0
  end
  updateDisplay()
end

local function onUSBEvent(e)
  if e == app.EVENT_USB_CONNECT then
    app.logInfo("onUSBEvent: connect")
    Signal.emit("onUSBConnect")
  elseif e == app.EVENT_USB_DISCONNECT then
    app.logInfo("onUSBEvent: disconnect")
    Signal.emit("onUSBDisconnect")
  elseif e == app.EVENT_USB_FRONT_CARD_MOUNT then
    app.logInfo("onUSBEvent: front card mounted")
    Signal.emit("onUSBFrontCardMount")
  elseif e == app.EVENT_USB_FRONT_CARD_UNMOUNT then
    app.logInfo("onUSBEvent: front card unmounted")
    Signal.emit("onUSBFrontCardUnmount")
  elseif e == app.EVENT_USB_REAR_CARD_MOUNT then
    app.logInfo("onUSBEvent: rear card mounted")
  elseif e == app.EVENT_USB_REAR_CARD_UNMOUNT then
    app.logInfo("onUSBEvent: rear card unmounted")
  elseif e == app.EVENT_USB_ERROR then
    app.logInfo("onUSBEvent: error")
    Signal.emit("onUSBError")
  else
    app.logInfo("onUSBEvent(%d): unknown event", e)
  end
end

local function notify(e, ...)
  Signal.emit(e, ...)
  visibleContext:notify(e, ...)
end

local Fifo = require "Fifo"
local posted = Fifo()
local function post(f)
  posted:push(f)
end

local function sendEvent(target, e, ...)
  if target[e] then
    return target[e](target, ...)
  end
end

local function postEvent(target, e, ...)
  if target[e] then
    local args = {...}
    post(function()
      target[e](target, table.unpack(args))
    end)
  end
end

local __weak__ = {__mode = "k"}
local pendingTriggers = setmetatable({}, __weak__)
local elapsedTriggers = setmetatable({}, __weak__)
local function postTrigger(key, task)
  pendingTriggers[key] = task
end

local eventToIgnore = nil
local ignoreCount = 0
local function ignoreEvent(event, times)
  eventToIgnore = event
  ignoreCount = times or 1
end

local lastMainButtonPressed = 1
local function defaultDispatcher(event)
  -- app.logInfo("Application.defaultDispatcher(%s)",event)
  if event == eventToIgnore then
    ignoreCount = ignoreCount - 1
    if ignoreCount < 1 then
      eventToIgnore = nil
    end
    return
  elseif event == app.EVENT_RELEASE_SHIFT then
    shifted = false
    notify("shiftReleased")
    return
  elseif event == app.EVENT_PRESS_SHIFT then
    shifted = true
    notify("shiftPressed")
    return
  end
  --------
  if event == app.EVENT_KNOB then
    local p = getEncoderChange()
    if p ~= 0 then
      visibleContext:notify("encoder", p, shifted)
    end
    ---- Press Events
  elseif event == app.EVENT_PRESS_UP then
    notify("upPressed", shifted)
  elseif event == app.EVENT_PRESS_HOME then
    if shifted then
      notify("zeroPressed")
    else
      notify("homePressed")
    end
  elseif event == app.EVENT_PRESS_ENTER then
    if shifted then
      notify("commitPressed")
    else
      notify("enterPressed")
    end
  elseif event == app.EVENT_PRESS_CANCEL then
    if not shifted then
      notify("cancelPressed", false)
    end
  elseif event == app.EVENT_PRESS_DIALMODE then
    notify("dialPressed", shifted)
  elseif event == app.EVENT_PRESS_MAIN1 then
    lastMainButtonPressed = 1
    notify("mainPressed", 1, shifted)
  elseif event == app.EVENT_PRESS_MAIN2 then
    lastMainButtonPressed = 2
    notify("mainPressed", 2, shifted)
  elseif event == app.EVENT_PRESS_MAIN3 then
    lastMainButtonPressed = 3
    notify("mainPressed", 3, shifted)
  elseif event == app.EVENT_PRESS_MAIN4 then
    lastMainButtonPressed = 4
    notify("mainPressed", 4, shifted)
  elseif event == app.EVENT_PRESS_MAIN5 then
    lastMainButtonPressed = 5
    notify("mainPressed", 5, shifted)
  elseif event == app.EVENT_PRESS_MAIN6 then
    lastMainButtonPressed = 6
    notify("mainPressed", 6, shifted)
  elseif event == app.EVENT_PRESS_SUB1 then
    notify("subPressed", 1, shifted)
  elseif event == app.EVENT_PRESS_SUB2 then
    notify("subPressed", 2, shifted)
  elseif event == app.EVENT_PRESS_SUB3 then
    notify("subPressed", 3, shifted)
  elseif event == app.EVENT_PRESS_SELECT1 then
    notify("selectPressed", 1, shifted)
  elseif event == app.EVENT_PRESS_SELECT2 then
    notify("selectPressed", 2, shifted)
  elseif event == app.EVENT_PRESS_SELECT3 then
    notify("selectPressed", 3, shifted)
  elseif event == app.EVENT_PRESS_SELECT4 then
    notify("selectPressed", 4, shifted)
    ---- Release Events
  elseif event == app.EVENT_RELEASE_UP then
    notify("upReleased", releaseShifted)
  elseif event == app.EVENT_RELEASE_HOME then
    if releaseShifted then
      notify("zeroReleased")
    else
      notify("homeReleased")
    end
  elseif event == app.EVENT_RELEASE_ENTER then
    if releaseShifted then
      notify("commitReleased")
    else
      notify("enterReleased")
    end
  elseif event == app.EVENT_RELEASE_CANCEL then
    if releaseShifted then
      defaultDispatcher(app.EVENT_RELEASE_SHIFT)
      Timer.after(0.1, doScreenShot)
    else
      notify("cancelReleased", false)
    end
  elseif event == app.EVENT_RELEASE_DIALMODE then
    notify("dialReleased", releaseShifted)
  elseif event == app.EVENT_RELEASE_MAIN1 then
    notify("mainReleased", 1, releaseShifted)
  elseif event == app.EVENT_RELEASE_MAIN2 then
    notify("mainReleased", 2, releaseShifted)
  elseif event == app.EVENT_RELEASE_MAIN3 then
    notify("mainReleased", 3, releaseShifted)
  elseif event == app.EVENT_RELEASE_MAIN4 then
    notify("mainReleased", 4, releaseShifted)
  elseif event == app.EVENT_RELEASE_MAIN5 then
    notify("mainReleased", 5, releaseShifted)
  elseif event == app.EVENT_RELEASE_MAIN6 then
    notify("mainReleased", 6, releaseShifted)
  elseif event == app.EVENT_RELEASE_SUB1 then
    notify("subReleased", 1, releaseShifted)
  elseif event == app.EVENT_RELEASE_SUB2 then
    notify("subReleased", 2, releaseShifted)
  elseif event == app.EVENT_RELEASE_SUB3 then
    notify("subReleased", 3, releaseShifted)
  elseif event == app.EVENT_RELEASE_SELECT1 then
    notify("selectReleased", 1, releaseShifted)
  elseif event == app.EVENT_RELEASE_SELECT2 then
    notify("selectReleased", 2, releaseShifted)
  elseif event == app.EVENT_RELEASE_SELECT3 then
    notify("selectReleased", 3, releaseShifted)
  elseif event == app.EVENT_RELEASE_SELECT4 then
    notify("selectReleased", 4, releaseShifted)
    ---- Repeat Events
  elseif event == app.EVENT_REPEAT_UP then
    notify("upRepeated", shifted)
  elseif event == app.EVENT_REPEAT_HOME then
    if shifted then
      notify("zeroRepeated")
    else
      notify("homeRepeated")
    end
  elseif event == app.EVENT_REPEAT_ENTER then
    if shifted then
      notify("commitRepeated")
    else
      notify("enterRepeated")
    end
  elseif event == app.EVENT_REPEAT_CANCEL then
    if not shifted then
      notify("cancelRepeated", false)
    end
  elseif event == app.EVENT_REPEAT_DIALMODE then
    notify("dialRepeated", shifted)
  elseif event == app.EVENT_REPEAT_MAIN1 then
    notify("mainRepeated", 1, shifted)
  elseif event == app.EVENT_REPEAT_MAIN2 then
    notify("mainRepeated", 2, shifted)
  elseif event == app.EVENT_REPEAT_MAIN3 then
    notify("mainRepeated", 3, shifted)
  elseif event == app.EVENT_REPEAT_MAIN4 then
    notify("mainRepeated", 4, shifted)
  elseif event == app.EVENT_REPEAT_MAIN5 then
    notify("mainRepeated", 5, shifted)
  elseif event == app.EVENT_REPEAT_MAIN6 then
    notify("mainRepeated", 6, shifted)
  elseif event == app.EVENT_REPEAT_SUB1 then
    notify("subRepeated", 1, shifted)
  elseif event == app.EVENT_REPEAT_SUB2 then
    notify("subRepeated", 2, shifted)
  elseif event == app.EVENT_REPEAT_SUB3 then
    notify("subRepeated", 3, shifted)
  elseif event == app.EVENT_REPEAT_SELECT1 then
    notify("selectRepeated", 1, shifted)
  elseif event == app.EVENT_REPEAT_SELECT2 then
    notify("selectRepeated", 2, shifted)
  elseif event == app.EVENT_REPEAT_SELECT3 then
    notify("selectRepeated", 3, shifted)
  elseif event == app.EVENT_REPEAT_SELECT4 then
    notify("selectRepeated", 4, shifted)
  elseif event == app.EVENT_MODE then
    Signal.emit("onModeChanged")
  elseif event == app.EVENT_STORAGE then
    onStorageChanged()
    Signal.emit("onStorageChanged")
  end
  releaseShifted = shifted
end

local initialState
local function resetToInitialState()
  initialState:apply()
end

local function init()
  app.logInfo("Application.init: usb")
  USB.init()

  app.logInfo("Application.init: card")
  Card.init()

  app.logInfo("Application.init: package manager")
  PackageManager.init()

  app.logInfo("Application.init: persist module")
  local Persist = require "Persist"
  Persist.init()

  app.logInfo("Application.init: channels")
  local Channels = require "Channels"
  Channels.init()

  app.logInfo("Application.init: encoder")
  Encoder.init()

  local Random = require "Random"
  Random.init()

  -- trigger default state
  app.logInfo("Application.init: setting mode")
  onStorageChanged()

  app.logInfo("Application.init: selecting channel 1")
  notify("selectPressed", 1, false)
  notify("selectReleased", 1, false)

  collectgarbage()
  local seed = app.Rng_read32()
  math.randomseed(seed)
  app.logInfo("Application.init: random seed = 0x%08x", seed)

  local filename = app.roots.rear .. "/onNextBoot.lua"
  local Path = require "Path"
  if Path.exists(filename) then
    app.logInfo("%s: executing...", filename)
    local success, msg = dofile(filename)
    if success then
      app.logInfo("%s: success, %s", filename, msg)
      app.deleteFile(filename)
    else
      app.logError("%s: failed, %s", filename, msg)
    end
  end

  local QuickSavePreset = require "Persist.QuickSavePreset"
  initialState = QuickSavePreset()
  initialState:populate()

  if not restore() then
    local action = Settings.get("restoreLastSlotAction")
    if action == "prompt" then
      Persist.loadLastSlot(true)
    elseif action == "yes" then
      if app.isCancelButtonPushed() then
        app.logInfo("Restore last slot canceled by button.")
      else
        Persist.loadLastSlot(false)
      end
    end
  end

  app.logInfo("Application.init: adc, mod, and audio start")
  app.Adc_start();
  app.Modulation_start();
  app.Audio_start()
end

local activeDispatcher = defaultDispatcher
local function setDispatcher(dispatcher)
  activeDispatcher = dispatcher or defaultDispatcher
end

-- event loop
local function loop()
  app.logInfo("Application.loop: entering event loop.")
  local Crash = require "Crash"
  Crash.init()

  local eventNone = app.EVENT_NONE
  local eventDisplayReady = app.EVENT_DISPLAY_READY
  local eventUSB = app.EVENT_USB
  local eventQuit = app.EVENT_QUIT
  local quit = false
  while not quit do
    waitForEvent()
    startEventTimer()
    -- process spontaneous events
    while true do
      local e = pullEvent()
      if e == eventNone then
        break
      elseif e == eventDisplayReady then
        onDisplayReady()
      elseif (e >> 16) == eventUSB then
        onUSBEvent(e)
      elseif e == eventQuit then
        app.logInfo("Quitting...")
        quit = true
      else
        -- Debug.traceCalls(true)
        app.UIThread.restartScreenSaverTimer()
        activeDispatcher(e)
        -- Debug.traceCalls(false)
      end
    end
    -- process events posted during the spontaneous event processing
    while posted:length() > 0 do
      posted:pop()()
    end
    -- process triggers
    local save = elapsedTriggers
    elapsedTriggers = pendingTriggers
    pendingTriggers = save
    for key, task in pairs(elapsedTriggers) do
      local f = key[task]
      if f then
        f(key)
      end
      elapsedTriggers[key] = nil
    end
    stopEventTimer()
  end

  Busy.kill()
  app.logInfo("Application.loop: leaving event loop.")
end

Busy.stop()

return {
  init = init,
  loop = loop,
  post = post,
  sendEvent = sendEvent,
  postEvent = postEvent,
  postTrigger = postTrigger,
  getVisibleContext = getVisibleContext,
  setVisibleContext = setVisibleContext,
  setActiveMode = setActiveMode,
  resetToInitialState = resetToInitialState,
  getLastMainButtonPressed = function()
    return lastMainButtonPressed
  end,
  setDispatcher = setDispatcher,
  notify = notify,
  defaultDispatcher = defaultDispatcher,
  ignoreEvent = ignoreEvent,
  restart = restart
}
