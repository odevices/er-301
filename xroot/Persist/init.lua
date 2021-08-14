local Signal = require "Signal"
local FileChooser = require "Card.FileChooser"
local Path = require "Path"
local Card = require "Card"
local Utils = require "Utils"
local Env = require "Env"
local Busy = require "Busy"
local Message = require "Message"
local Verification = require "Verification"
local Preset = require "Persist.Preset"
local QuickSavePreset = require "Persist.QuickSavePreset"
local UnitPreset = require "Persist.UnitPreset"
local ChainPreset = require "Persist.ChainPreset"
local Serialization = require "Persist.Serialization"
local FS = require "Card.FileSystem"

local readTable = Serialization.readTable
local writeTable = Serialization.writeTable

local function showMessage(...)
  local dialog = Message.Main(...)
  dialog:show()
end

local quickSaveMaxSlots = 24
local metaData = {}

local function readBootData()
  -- read boot data, if any
  app.logInfo("Meta: reading previous boot data.")
  local filename = FS.getRoot("front-meta") .. "/boot.lua"
  local data = readTable(filename)
  if data then
    metaData["prevBoot"] = data
    app.logInfo("Meta: prev booted version was %s",
                data.firmwareVersion or data.version)
  else
    app.logInfo("Meta: No previous boot data.")
    metaData["prevBoot"] = metaData["boot"]
  end
end

local function updateBootData()
  local data = metaData["boot"]
  local prev = metaData.prevBoot or data
  data.firmwareVersion = app.FIRMWARE_VERSION
  data.version = nil -- remove legacy version field
  data.count = prev.count + 1
  data.lastSlot = prev.lastSlot or -1
  app.logInfo("Meta: boot count = %d", data.count)
  app.logInfo("Meta: last slot = %d", data.lastSlot)
end

local function writeBootData()
  -- app.logInfo("Meta: writing boot data.")
  local filename = FS.getRoot("front-meta") .. "/boot.lua"
  local data = metaData["boot"]
  if not writeTable(filename, data) then
    app.logInfo("Meta: Failed to write boot data.")
  end
end

local function pathToFrontDatabase(dbName)
  return Path.join(FS.getRoot("front-meta"), dbName .. ".db")
end

local function getFrontCardValue(dbName, key)
  local path = pathToFrontDatabase(dbName)
  local data = readTable(path)
  return data and data[key]
end

local function setFrontCardValue(dbName, key, value)
  local path = pathToFrontDatabase(dbName)
  local data = readTable(path) or {}
  data[key] = value
  writeTable(path, data)
end

local function deleteFrontCardDatabase(dbName)
  local path = pathToFrontDatabase(dbName)
  if Path.exists(path) then app.deleteFile(path) end
end

local function pathToRearDatabase(dbName)
  return Path.join(FS.getRoot("rear-meta"), dbName .. ".db")
end

local function getRearCardValue(dbName, key)
  local path = pathToRearDatabase(dbName)
  local data = readTable(path)
  return data and data[key]
end

local function setRearCardValue(dbName, key, value)
  local path = pathToRearDatabase(dbName)
  local data = readTable(path) or {}
  data[key] = value
  writeTable(path, data)
end

local function deleteRearCardDatabase(dbName)
  local path = pathToRearDatabase(dbName)
  if Path.exists(path) then app.deleteFile(path) end
end

local function confirmFirmwareVersionAndExecute(preset, task)
  local version = preset:getVersionString()
  local x = Utils.convertVersionStringToNumber(version)
  local y = Utils.convertVersionStringToNumber(app.FIRMWARE_VERSION)
  if y < x then
    local text = string.format("Preset was created by newer firmware: v%s",
                               version)
    local dlg =
        Verification.Main(text, "Are you sure you want to continue?", 10)
    dlg:subscribe("done", task)
    dlg:show()
  else
    -- confirmed to be created with older firmware
    task(true)
  end
end

-- load/save unit preset

local function saveUnitPreset(unit, fullpath)
  if fullpath == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("unit-preset", "w", result.fullpath) then
          saveUnitPreset(unit, result.fullpath)
        end
      end
    end
    local path, suggested = unit:getLastPreset()
    if path == nil or not Path.exists(path) then
      path = FS.getRoot("unit-preset")
    end
    local history
    if suggested == nil then
      history = unit.loadInfo.id .. "/Preset"
      if unit.hasUserTitle then
        suggested = string.lower(unit.title:gsub(" ", "-"):gsub("#", ""))
      end
    end
    local chooser = FileChooser {
      msg = string.format("Save Unit (*.%s)", FS.getExt("unit-preset")),
      goal = "save file",
      path = path,
      ext = "." .. FS.getExt("unit-preset"),
      pattern = FS.getPattern("unit-preset"),
      suggested = suggested,
      history = history
    }
    if suggested then chooser:choose(suggested) end
    chooser:subscribe("done", task)
    chooser:show()
  else
    local path, filename = Path.split(fullpath)
    local preset = UnitPreset(unit:serialize())
    Busy.start("Saving unit preset: %s", filename)
    if preset:write(fullpath) then
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Unit preset saved: %s", filename)
      unit:setLastPreset(path, filename)
    else
      showMessage("Failed to save unit preset:", filename)
    end
    Busy.stop()
  end
end

local function loadUnitPreset(unit, fullpath)
  if fullpath == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("unit-preset", "r", result.fullpath) then
          loadUnitPreset(unit, result.fullpath)
        end
      end
    end
    local path, suggested = unit:getLastPreset()
    if path == nil or not Path.exists(path) then
      path = FS.getRoot("unit-preset")
    end
    local history
    if suggested == nil then history = unit.loadInfo.id .. "/Preset" end
    local chooser = FileChooser {
      msg = string.format("Load Unit (*.%s)", FS.getExt("unit-preset")),
      goal = "load file",
      path = path,
      pattern = FS.getPattern("unit-preset"),
      suggested = suggested,
      history = history
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local path, filename = Path.split(fullpath)
    local preset = UnitPreset()
    Busy.start("Reading unit preset: %s", filename)
    local result = preset:read(fullpath)
    Busy.stop()
    if result then
      local task = function(ans)
        if ans then
          Busy.start("Loading unit preset: %s", filename)
          Path.pushWorkingDirectory(path)
          unit:deserialize(preset.data)
          Path.popWorkingDirectory()
          unit:setLastPreset(path, filename)
          Busy.stop()
          local Overlay = require "Overlay"
          Overlay.mainFlashMessage("Unit preset loaded: %s", filename)
        end
      end
      confirmFirmwareVersionAndExecute(preset, task)
    else
      showMessage("Failed to read unit preset:", filename)
    end
  end
end

-- load/save chain

local function saveChainPreset(chain, fullpath)
  if fullpath == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("chain-preset", "w", result.fullpath) then
          saveChainPreset(chain, result.fullpath)
        end
      end
    end
    local path, suggested = chain:getLastPreset()
    if path == nil or not Path.exists(path) then
      path = FS.getRoot("chain-preset")
    end
    local chooser = FileChooser {
      msg = string.format("Save Chain (*.%s)", FS.getExt("chain-preset")),
      goal = "save file",
      path = path,
      ext = "." .. FS.getExt("chain-preset"),
      pattern = FS.getPattern("chain-preset"),
      suggested = suggested,
      history = "chainPreset"
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local path, filename = Path.split(fullpath)
    local preset = ChainPreset(chain:serialize())
    Busy.start("Saving chain preset: %s", filename)
    if preset:write(fullpath) then
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Chain preset saved: %s", filename)
      chain:setLastPreset(path, filename)
    else
      showMessage("Failed to save chain preset:", filename)
    end
    Busy.stop()
  end
end

local function loadChainPreset(chain, fullpath)
  if fullpath == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("chain-preset", "r", result.fullpath) then
          loadChainPreset(chain, result.fullpath)
        end
      end
    end
    local path, suggested = chain:getLastPreset()
    if path == nil or not Path.exists(path) then
      path = FS.getRoot("chain-preset")
    end
    local chooser = FileChooser {
      msg = string.format("Load Chain (*.%s)", FS.getExt("chain-preset")),
      goal = "load file",
      path = path,
      ext = "." .. FS.getExt("chain-preset"),
      pattern = FS.getPattern("chain-preset"),
      suggested = suggested,
      history = "chainPreset"
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local path, filename = Path.split(fullpath)
    local preset = ChainPreset()
    Busy.start("Reading chain preset: %s", filename)
    local result = preset:read(fullpath)
    Busy.stop()
    if result then
      local task = function(ans)
        if ans then
          Busy.start("Loading chain preset: %s", filename)
          Path.pushWorkingDirectory(path)
          local wasMuted = chain:muteIfNeeded()
          chain:stop()
          chain:deserialize(preset.data)
          chain:start()
          chain:unmuteIfNeeded(wasMuted)
          Path.popWorkingDirectory()
          chain:setLastPreset(path, filename)
          Busy.stop()
          local Overlay = require "Overlay"
          Overlay.mainFlashMessage("Chain preset loaded: %s", filename)
        end
      end
      confirmFirmwareVersionAndExecute(preset, task)
    else
      showMessage("Failed to load Chain Preset.")
    end
  end
end

-- load/save sample pool state

local function savePoolPreset(filename)
  if filename == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("pool-preset", "w", result.fullpath) then
          savePoolPreset(result.fullpath)
        end
      end
    end
    local chooser = FileChooser {
      msg = string.format("Save Pool (*.%s)", FS.getExt("pool-preset")),
      goal = "save file",
      path = FS.getRoot("pool-preset"),
      ext = "." .. FS.getExt("pool-preset"),
      pattern = FS.getPattern("pool-preset"),
      history = "poolPreset"
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local SamplePool = require "Sample.Pool"
    local preset = Preset(SamplePool.serialize())
    if not preset:write(filename) then
      showMessage("Failed to save Pool Preset.")
    end
  end
end

local function loadPoolPreset(filename)
  if filename == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("pool-preset", "r", result.fullpath) then
          loadPoolPreset(result.fullpath)
        end
      end
    end
    local chooser = FileChooser {
      msg = string.format("Lood Pool (*.%s)", FS.getExt("pool-preset")),
      goal = "load file",
      path = FS.getRoot("pool-preset"),
      ext = "." .. FS.getExt("pool-preset"),
      pattern = FS.getPattern("pool-preset"),
      history = "poolPreset"
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local SamplePool = require "Sample.Pool"
    local preset = Preset()
    if preset:read(filename) then
      SamplePool.clear()
      SamplePool.deserialize(preset.data)
    else
      showMessage("Failed to load Pool Preset.")
    end
  end
end

-- load/save multitrack state

local function saveRecorderPreset(filename)
  if filename == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("6track-preset", "w", result.fullpath) then
          saveRecorderPreset(result.fullpath)
        end
      end
    end
    local chooser = FileChooser {
      msg = string.format("Save Preset (*.%s)", FS.getExt("6track-preset")),
      goal = "save file",
      path = FS.getRoot("6track-preset"),
      ext = "." .. FS.getExt("6track-preset"),
      pattern = FS.getPattern("6track-preset"),
      history = "recorderPreset"
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local recorder = require "FileRecorder"
    local preset = Preset(recorder:serialize())
    if not preset:write(filename) then
      showMessage("Failed to save multitrack preset.")
    end
  end
end

local function loadRecorderPreset(filename)
  if filename == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("6track-preset", "r", result.fullpath) then
          loadRecorderPreset(result.fullpath)
        end
      end
    end
    local chooser = FileChooser {
      msg = string.format("Load Preset (*.%s)", FS.getExt("6track-preset")),
      goal = "load file",
      path = FS.getRoot("6track-preset"),
      ext = "." .. FS.getExt("6track-preset"),
      pattern = FS.getPattern("6track-preset"),
      history = "recorderPreset"
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local recorder = require "FileRecorder"
    local preset = Preset()
    if preset:read(filename) then
      recorder:deserialize(preset.data)
    else
      showMessage("Failed to load multitrack preset.")
    end
  end
end

-- load/save global chains state

local function saveGlobalsPreset(filename)
  if filename == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("globals-preset", "w", result.fullpath) then
          saveGlobalsPreset(result.fullpath)
        end
      end
    end
    local chooser = FileChooser {
      msg = string.format("Save Preset (*.%s)", FS.getExt("globals-preset")),
      goal = "save file",
      path = FS.getRoot("globals-preset"),
      ext = "." .. FS.getExt("globals-preset"),
      pattern = FS.getPattern("globals-preset"),
      history = "globalsPreset"
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local GlobalChains = require "GlobalChains"
    local preset = Preset(GlobalChains.serialize())
    if not preset:write(filename) then
      showMessage("Failed to save Global Chains Preset.")
    end
  end
end

local function loadGlobalsPreset(filename)
  if filename == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("globals-preset", "r", result.fullpath) then
          loadGlobalsPreset(result.fullpath)
        end
      end
    end
    local chooser = FileChooser {
      msg = string.format("Load Preset (*.%s)", FS.getExt("globals-preset")),
      goal = "load file",
      path = FS.getRoot("globals-preset"),
      ext = "." .. FS.getExt("globals-preset"),
      pattern = FS.getPattern("globals-preset"),
      history = "globalsPreset"
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local GlobalChains = require "GlobalChains"
    local preset = Preset()
    if preset:read(filename) then
      GlobalChains.deserialize(preset.data)
    else
      showMessage("Failed to load Global Chains Preset.")
    end
  end
end

-- load/save preamp state

local function savePreampPreset(filename)
  if filename == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("preamp-preset", "w", result.fullpath) then
          savePreampPreset(result.fullpath)
        end
      end
    end
    local chooser = FileChooser {
      msg = string.format("Save Preset (*.%s)", FS.getExt("preamp-preset")),
      goal = "save file",
      path = FS.getRoot("preamp-preset"),
      ext = "." .. FS.getExt("preamp-preset"),
      pattern = FS.getPattern("preamp-preset"),
      history = "preampPreset"
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local Preamp = require "Preamp"
    local preset = Preset(Preamp.serialize())
    if not preset:write(filename) then
      showMessage("Failed to save Preamp Preset.")
    end
  end
end

local function loadPreampPreset(filename)
  if filename == nil then
    -- try to get the filename from the user
    local task = function(result)
      if result and result.fullpath then
        if FS.checkPath("preamp-preset", "r", result.fullpath) then
          loadPreampPreset(result.fullpath)
        end
      end
    end
    local chooser = FileChooser {
      msg = string.format("Load Preset (*.%s)", FS.getExt("preamp-preset")),
      goal = "load file",
      path = FS.getRoot("preamp-preset"),
      ext = "." .. FS.getExt("preamp-preset"),
      pattern = FS.getPattern("preamp-preset"),
      history = "preampPreset"
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local Preamp = require "Preamp"
    local preset = Preset()
    if preset:read(filename) then
      Preamp.deserialize(preset.data)
    else
      showMessage("Failed to load Preamp Preset.")
    end
  end
end

-- load/save complete state

local maxQuickSaves = 4
local quickSaveCache = {}
local quickSaveNames = nil

local function quickSaveFile(slot, i)
  return FS.getRoot("quicksaves") .. string.format("/slot%d/Q%02d.", slot, i) ..
             FS.getExt("quicksave")
end

local function quickSaveFileLegacy(slot, i)
  return FS.getRoot("quicksaves") .. string.format("/slot%d/Q%02d.", slot, i) ..
             "lua"
end

local function loadQuickSaveNames()
  -- try to load the names
  local filename = FS.getRoot("quicksaves") .. "/names.lua"
  quickSaveNames = readTable(filename) or {}
end

local function saveQuickSaveNames()
  if quickSaveNames == nil then return end
  local filename = FS.getRoot("quicksaves") .. "/names.lua"
  quickSaveNames.firmwareVersion = app.FIRMWARE_VERSION
  if not writeTable(filename, quickSaveNames) then
    showMessage("Failed to write to card.")
  end
end

local function setQuickSaveName(slot, name)
  if quickSaveNames == nil then loadQuickSaveNames() end
  quickSaveNames[slot] = name
  saveQuickSaveNames()
end

local function getQuickSaveName(slot)
  if quickSaveNames == nil then loadQuickSaveNames() end
  return quickSaveNames[slot]
end

local function handleUnsavedBuffers(after)
  local SamplePool = require "Sample.Pool"
  if SamplePool.hasDirtySamples() then
    local Verification = require "Verification"
    local dlg = Verification.Main("You have unsaved audio buffers.",
                                  "Go to Sample Pool first?")
    local task = function(ans)
      if ans then
        local SamplePoolInterface = require "Sample.Pool.Interface"
        local window = SamplePoolInterface("Admin", "save")
        window:subscribe("done", function(ans)
          if ans then
            after()
          else
            local Overlay = require "Overlay"
            Overlay.mainFlashMessage("Quicksave canceled.")
          end
        end)
        window:show()
      else
        after()
      end
    end
    dlg:subscribe("done", task)
    dlg:show()
  else
    after()
  end
end

local function quickSaveToSlot(slot, checkForUnsavedBuffers, after)
  if checkForUnsavedBuffers then
    handleUnsavedBuffers(function()
      quickSaveToSlot(slot, false, after)
    end)
    return
  end
  local name = getQuickSaveName(slot) or string.format("Slot %d", slot)
  Busy.start("Quicksaving to '%s'...", name)
  quickSaveCache[slot] = nil
  -- from oldest to newest
  for i = maxQuickSaves - 1, 1, -1 do
    local newFile = quickSaveFile(slot, i + 1)
    local oldFile = quickSaveFile(slot, i)
    app.moveFile(oldFile, newFile, true)
  end
  local filename = quickSaveFile(slot, 1)
  local preset = QuickSavePreset()
  preset:populate()
  Busy.status("Writing save data to card...")
  local result = preset:write(filename)
  metaData["boot"].lastSlot = slot
  writeBootData()
  Busy.stop()
  local Overlay = require "Overlay"
  if result then
    app.logInfo("Quicksaved to %s", filename)
    Overlay.mainFlashMessage("Quicksaved to slot %d.", slot)
  else
    app.logError("Quicksave:failed")
    Overlay.mainFlashMessage("Quicksave to slot %d failed.", slot)
  end
  if after then after() end
end

local function quickSaveToFile(filename, checkForUnsavedBuffers, after)
  if checkForUnsavedBuffers then
    handleUnsavedBuffers(function()
      quickSaveToFile(filename, false, after)
    end)
    return
  end
  Busy.start("Quicksaving to '%s'...", filename)
  local preset = QuickSavePreset()
  preset:populate()
  Busy.status("Writing save data to card...")
  preset:write(filename)
  Busy.stop()
  if after then after() end
end

local function getQuickSavePreset(slot)
  local preset = quickSaveCache[slot]
  if preset == nil then
    -- try to load it
    local filename = quickSaveFile(slot, 1)
    if app.pathExists(filename) then
      local data = readTable(filename)
      if data then
        preset = QuickSavePreset(data, true)
        quickSaveCache[slot] = preset
      else
        app.logError(
            "Persist:getQuickSavePreset(%d):Invalid quicksave file: %s", slot,
            filename)
      end
    end
  end
  return preset
end

local function quickLoad(slot, i)
  local filename = quickSaveFile(slot, i or 1)
  if not app.pathExists(filename) then
    filename = quickSaveFileLegacy(slot, i or 1)
  end
  if app.pathExists(filename) then
    local Overlay = require "Overlay"
    local preset = QuickSavePreset()
    local name = getQuickSaveName(slot) or string.format("Slot %d", slot)
    Busy.start("Reading '%s'...", name)
    local result = preset:read(filename)
    Busy.stop()
    if result then
      local task = function(ans)
        if ans then
          Busy.start("Loading '%s'...", name)
          preset:apply()
          metaData["boot"].lastSlot = slot
          writeBootData()
          Busy.stop()
          Overlay.mainFlashMessage("'%s' loaded (^_^)y", name)
        end
      end
      confirmFirmwareVersionAndExecute(preset, task)
    else
      Overlay.mainFlashMessage("Failed to load '%s' (>_<)", name)
    end
  end
end

local firstMount = true
local function onCardMounted()
  Busy.start("Initializing quicksaves...")
  -- empty the quick save cache
  for i = 1, quickSaveMaxSlots do quickSaveCache[i] = nil end
  quickSaveNames = nil

  for i = 1, quickSaveMaxSlots do
    Path.createAll(string.format("%s/slot%d", FS.getRoot("quicksaves"), i))
  end

  if firstMount then
    firstMount = false
    if pcall(readBootData) then
      Busy.status("Initializing boot data..")
      updateBootData()
      pcall(writeBootData)
    end
  end

  metaData.mount.count = metaData.mount.count + 1
  app.logInfo("Meta: mount count = %d", metaData.mount.count)
  Busy.stop()
end

local function loadLastSlot(prompt)
  local slot = metaData["boot"].lastSlot or -1
  app.logInfo("loadLastSlot: slot = %d.", slot)
  if slot > 0 then
    local filename = quickSaveFile(slot, 1)
    app.logInfo("loadLastSlot: filename = %s", filename)
    if app.pathExists(filename) then
      if prompt then
        local after = function(ans)
          if ans then
            app.logInfo("Restoring from quicksave slot %d.", slot)
            quickLoad(slot)
          else
            app.logInfo("Restore canceled by user.")
          end
        end
        local name = getQuickSaveName(slot) or string.format("Slot %d", slot)
        local dialog = Verification.Main("Restore last Quicksave?",
                                         "[" .. name .. "]")
        dialog:subscribe("done", after)
        dialog:show()
      else
        local after = function(ans)
          if ans then
            app.logInfo("Restoring from quicksave slot %d.", slot)
            quickLoad(slot)
          else
            app.logInfo("Restore canceled by user.")
          end
        end
        local name = getQuickSaveName(slot) or string.format("Slot %d", slot)
        local dialog = Verification.Main("Restoring in %d secs...",
                                         "[" .. name .. "]")
        dialog:setTimer(3, true)
        dialog:subscribe("done", after)
        dialog:show()
      end
    else
      app.logInfo("Cannot restore quicksave because file does not exist.")
    end
  end
end

local function getScreenShotFilename()
  local maxId = 0
  for x in dir(FS.getRoot("screenshot")) do
    local a, b = string.find(x, "[0-9][0-9][0-9][0-9]")
    if a and b then
      local tmp = tonumber(string.sub(x, a, b))
      if tmp and tmp > maxId then maxId = tmp end
    end
  end
  return string.format("%s/%04d.png", FS.getRoot("screenshot"), maxId + 1)
end

local function getSessionNumber()
  return metaData["boot"].count
end

local function init()
  metaData["boot"] = {
    firmwareVersion = app.FIRMWARE_VERSION,
    count = 0,
    lastSlot = -1
  }
  metaData["mount"] = {
    count = 0
  }
  Signal.register("cardMounted", onCardMounted)
  -- if card was already mounted when loading this module...
  if Card.mounted() then onCardMounted() end
end

local function generateInstanceKey()
  return string.format("%08x", app.Rng_read32())
end

local function regenerateInstanceKeysHelper(presetData, keys)
  keys = keys or {}
  local originalKey = presetData.instanceKey
  if originalKey then
    local newKey = keys[originalKey] or generateInstanceKey()
    presetData.instanceKey = newKey
    if presetData.originalInstanceKey == nil then
      presetData.originalInstanceKey = originalKey
    end
    keys[originalKey] = newKey
    -- app.logInfo("Regenerate Keys: %s becomes %s", originalKey, newKey)
  end
  for k, v in pairs(presetData) do
    if type(v) == "table" then regenerateInstanceKeysHelper(v, keys) end
  end
  return keys
end

local function propagateReplacements(presetData, replace)
  for k, v in pairs(presetData) do
    if type(v) == "table" then
      propagateReplacements(v, replace)
    elseif type(v) == "string" then
      local newValue = replace[v]
      if newValue then presetData[k] = newValue end
    end
  end
end

local function regenerateInstanceKeys(presetData)
  local replaced = regenerateInstanceKeysHelper(presetData)
  -- Propagate the replacements to the values of each (key,value) pair.
  propagateReplacements(presetData, replaced)
end

local function serializeObjects(objects)
  local t = {}
  for oname, o in pairs(objects) do
    local hasOptionsToSerialize = o:hasOptionsToSerialize()
    local hasParametersToSerialize = o:hasParametersToSerialize()
    if hasOptionsToSerialize or hasParametersToSerialize then
      local oData = {}

      if hasParametersToSerialize then
        -- parameters
        local pData = {}
        local n = o:getParameterCount()
        for i = 0, n - 1 do
          local param = o:getParameter(i)
          if param:isSerializationNeeded() then
            pData[param:name()] = param:target()
          end
        end
        oData.params = pData
      end

      if hasOptionsToSerialize then
        -- options
        local mData = {}
        local n = o:getOptionCount()
        for i = 0, n - 1 do
          local option = o:getOption(i)
          if option:isSerializationNeeded() then
            mData[option:name()] = option:value()
          end
        end
        oData.options = mData
      end

      t[oname] = oData
    end
  end
  return t
end

local function deserializeObject(o, data)
  -- parameters
  local params = data.params
  if params then
    for k, v in pairs(params) do
      if not o:deserializeParameter(k, v) then
        app.logInfo("deserializeObject(%s): parameter '%s' not found", o:name(),
                    k)
      end
    end
  end
  -- options
  local options = data.options
  if options then
    for k, v in pairs(options) do
      if not o:setOptionValue(k, v) then
        app.logInfo("deserializeObject(%s): option '%s' not found", o:name(), k)
      end
    end
  end
end

local function deserializeObjectHelper(oname, data, objects, aliases)
  local o = objects[oname]
  if o then
    deserializeObject(o, data)
    return
  end
  if aliases and aliases[oname] then
    for _, oname in ipairs(aliases[oname]) do
      local o = objects[oname]
      if o then
        deserializeObject(o, data)
        return
      end
    end
  end
  -- match "gainbias1" or "gainbiasL" with "gainbias"
  if Utils.endsWith(oname, "1") or Utils.endsWith(oname, "L") then
    o = objects[string.sub(oname, 1, -2)]
    if o then
      deserializeObject(o, data)
      return
    end
  end
  -- match "gainbias" with "gainbias1" or "gainbiasL"
  o = objects[oname .. "1"]
  if o and objects[oname .. "2"] then
    deserializeObject(o, data)
    return
  end
  o = objects[oname .. "L"]
  if o and objects[oname .. "R"] then
    deserializeObject(o, data)
    return
  end
end

local function deserializeObjects(objects, aliases, data)
  for oname, oData in pairs(data) do
    deserializeObjectHelper(oname, oData, objects, aliases)
  end
end

return {
  init = init,
  readTable = readTable,
  writeTable = writeTable,
  getFrontCardValue = getFrontCardValue,
  setFrontCardValue = setFrontCardValue,
  deleteFrontCardDatabase = deleteFrontCardDatabase,
  getRearCardValue = getRearCardValue,
  setRearCardValue = setRearCardValue,
  deleteRearCardDatabase = deleteRearCardDatabase,
  loadRecorderPreset = loadRecorderPreset,
  saveRecorderPreset = saveRecorderPreset,
  loadGlobalsPreset = loadGlobalsPreset,
  saveGlobalsPreset = saveGlobalsPreset,
  loadPreampPreset = loadPreampPreset,
  savePreampPreset = savePreampPreset,
  loadUnitPreset = loadUnitPreset,
  saveUnitPreset = saveUnitPreset,
  saveChainPreset = saveChainPreset,
  loadChainPreset = loadChainPreset,
  savePoolPreset = savePoolPreset,
  loadPoolPreset = loadPoolPreset,
  quickSaveToSlot = quickSaveToSlot,
  quickSaveToFile = quickSaveToFile,
  quickLoad = quickLoad,
  loadLastSlot = loadLastSlot,
  getQuickSavePreset = getQuickSavePreset,
  getQuickSaveName = getQuickSaveName,
  setQuickSaveName = setQuickSaveName,
  meta = metaData,
  getScreenShotFilename = getScreenShotFilename,
  getSessionNumber = getSessionNumber,
  generateInstanceKey = generateInstanceKey,
  regenerateInstanceKeys = regenerateInstanceKeys,
  serializeObjects = serializeObjects,
  deserializeObjects = deserializeObjects,
  deserializeObject = deserializeObject
}
