local Variable = require "Settings.Variable"
local Env = require "Env"
local Timer = require "Timer"

local settingsFile = app.roots.rear.."/settings.lua"
local variables = nil
local promiseToSave = nil

local defaults = {
  sampleRate = {
    category = "General",
    description = "Sample Rate:",
    value = "48kHz",
    choices = {
      "48kHz",
      "96kHz"
    }
  },
  frameLength = {
    category = "General",
    description = "Frame Size:",
    value = "128",
    choices = {
      "32",
      "64",
      "128"
    }
  },
  animation = {
    category = "General",
    description = "Set the GUI animation speed or disable it.",
    value = "normal",
    choices = {
      "slow",
      "normal",
      "fast",
      "disabled"
    }
  },
  screenSaver = {
    category = "Screen Saver",
    description = "Starts when inactive for:",
    value = "30 mins",
    choices = {
      "10 mins",
      "30 mins",
      "2 hours",
      "1 day"
    },
    onSet = function(value)
      if value == "10 mins" then
        app.UIThread.setScreenSaverTime(10 * 60)
      elseif value == "30 mins" then
        app.UIThread.setScreenSaverTime(30 * 60)
      elseif value == "2 hours" then
        app.UIThread.setScreenSaverTime(2 * 3600)
      elseif value == "1 day" then
        app.UIThread.setScreenSaverTime(24 * 3600)
      end
    end
  },
  screenSaverGraphics = {
    category = "Screen Saver",
    description = "Choice of graphics:",
    value = "bubbles",
    choices = {
      "blank",
      "2lines",
      "grid",
      "bubbles"
    },
    onSet = function(value)
      app.UIThread.setScreenSaver(value)
    end
  },
  enableDevMode = {
    category = "General",
    description = "Enable development mode? (requires reboot)",
    value = false,
    onSet = function(value)
      if value and app.TESTING then
        app.enableDevMode()
      else
        app.disableDevMode()
      end
    end
  },
  unitControlReadoutSource = {
    category = "Units",
    description = "Displayed value for unit control readouts.",
    value = "bias",
    choices = {
      "bias",
      "actual"
    }
  },
  unitDisableOnBypass = {
    category = "Units",
    description = "Does bypassing a unit also disable it?",
    value = "no",
    choices = {
      "yes",
      "no"
    }
  },
  unitBrowserDefault = {
    category = "Units",
    description = "Default unit browsing mode:",
    value = "last",
    choices = {
      "last",
      "category",
      "A-to-Z",
      "presets"
    }
  },
  containerUnitNameGen = {
    category = "Units",
    description = "Name generation for container units:",
    value = "poet",
    choices = {
      "poet",
      "robot",
      "off"
    }
  },
  fileRecorderChannelCount = {
    category = "Multitrack Recorder",
    description = "Channel Count:",
    value = "6",
    choices = {
      "6",
      "8",
      "10",
      "12"
    },
    onSet = function()
      local FileRecorder = require "FileRecorder"
      FileRecorder:onChannelCountChanged()
    end
  },
  fileRecorderSingleTrackSaving = {
    category = "Multitrack Recorder",
    description = "Save single tracks to file or folder?",
    value = "file",
    choices = {
      "file",
      "folder"
    }
  },
  restoreLastSlotAction = {
    category = "Quicksave",
    description = "Restore last quicksave on boot?",
    value = "prompt",
    choices = {
      "yes",
      "no",
      "prompt"
    }
  },
  quickSaveRestoresClipboard = {
    category = "QuickSave",
    description = "Restore the clipboard contents?",
    value = "no",
    choices = {
      "yes",
      "no"
    }
  },
  quickSaveRestoresRecorder = {
    category = "QuickSave",
    description = "Restore the recorder configuration?",
    value = "yes",
    choices = {
      "yes",
      "no"
    }
  },
  insertSliceAtZeroCrossing = {
    category = "Sample Player",
    description = "INS: slices at nearest zero crossing?",
    value = true
  },
  shiftInsertSliceAtZeroCrossing = {
    category = "Sample Player",
    description = "Shift+INS: slices at nearest zero crossing?",
    value = false
  },
  confirmChainClear = {
    category = "Confirmations",
    description = "When clearing a chain...",
    value = "yes",
    choices = {
      "yes",
      "no"
    }
  },
  confirmUnitDelete = {
    category = "Confirmations",
    description = "When deleting a unit...",
    value = "yes",
    choices = {
      "yes",
      "no"
    }
  },
  confirmGlobalChainDelete = {
    category = "Confirmations",
    description = "When deleting a global chain...",
    value = "yes",
    choices = {
      "yes",
      "no"
    }
  },
  confirmPurgeUnusedSamples = {
    category = "Confirmations",
    description = "When purging unused samples...",
    value = "yes",
    choices = {
      "yes",
      "no"
    }
  },
  confirmClearSlices = {
    category = "Confirmations",
    description = "When clearing all slices...",
    value = "yes",
    choices = {
      "yes",
      "no"
    }
  },
  confirmSampleEdit = {
    category = "Confirmations",
    description = "When editing a sample...",
    value = "yes",
    choices = {
      "yes",
      "no"
    }
  },
  samplePreviewStop = {
    category = "File Browser",
    description = "Stop sample preview when focus changes?",
    value = "yes",
    choices = {
      "yes",
      "no"
    }
  },
  usbEnable = {
    category = "USB",
    description = "USB enabled?",
    value = "no",
    choices = {
      "yes",
      "no"
    },
    onSet = function()
      local Signal = require "Signal"
      Signal.emit("onUSBEnableChanged")
    end
  },
  usbMode = {
    category = "USB",
    description = "USB Device Mode",
    value = "card",
    choices = {
      "card",
      "serial"
    },
    onSet = function()
      local Signal = require "Signal"
      Signal.emit("onUSBModeChanged")
    end
  },
  floatingMenuDelay = {
    category = "General",
    description = "Long press time for floating menu:",
    value = "0.3s",
    choices = {
      "0.2s",
      "0.3s",
      "0.5s"
    }
  },
}

--------------------------------------------------------

local function get(name)
  local var = variables[name]
  return var and var.value
end

local function loadDefaults()
  variables = {}
  for name, data in pairs(defaults) do
    if type(data.value) == "boolean" then
      variables[name] = Variable.Boolean(name, data.category, data.description,
                                         data.value, data.onSet)
    elseif type(data.value) == "string" then
      variables[name] = Variable.String(name, data.category, data.description,
                                        data.value, data.choices, data.onSet)
    elseif type(data.value) == "number" then
      variables[name] = Variable.Number(name, data.category, data.description,
                                        data.value, data.min, data.max,
                                        data.onSet)
    end
  end
end

local function cancelScheduledSave()
  if promiseToSave then
    Timer.cancel(promiseToSave)
    promiseToSave = nil
  end
end

local function syncFirmwareConfig(allowReboot)
  -- Make sure the firmware.cfg matches with settings.
  local sampleRate = 1000 * tonumber(get("sampleRate"):sub(1, 2))
  local frameLength = tonumber(get("frameLength"))
  if app.globalConfig.sampleRate ~= sampleRate or app.globalConfig.frameLength ~=
      frameLength then
    if allowReboot then
      app.Config_store(sampleRate, frameLength)
      local Verification = require "Verification"
      local dialog = Verification.Main("These changes require a restart.",
                                       "Restart now?")
      local task = function(ans)
        if ans then
          local Application = require "Application"
          Application.restart(true)
        end
      end
      dialog:subscribe("done", task)
      dialog:show()
    else
      app.logInfo("Updating settings to match firmware configuration.")
      variables["sampleRate"]:set(tostring(sampleRate) .. "kHz")
      variables["frameLength"]:set(tostring(frameLength))
    end
  end
end

local function save()
  local Persist = require "Persist"
  local Overlay = require "Overlay"
  local Busy = require "Busy"
  Busy.start("Saving settings to rear card...")
  app.logInfo("Saving settings to %s.", settingsFile)
  local t = {
    firmwareVersion = app.FIRMWARE_VERSION
  }
  for name, v in pairs(variables) do t[name] = v.value end
  if not Persist.writeTable(settingsFile, t) then
    Busy.stop()
    Overlay.flashMainMessage("Failed to save settings (>_<)")
    app.logError("Failed to save settings.")
  else
    Busy.stop()
    Overlay.flashMainMessage("Settings saved.")
  end
  syncFirmwareConfig(true)
end

local function load()
  local Persist = require "Persist"
  local Busy = require "Busy"
  loadDefaults()
  -- overwrite defaults
  app.logInfo("Loading settings from %s.", settingsFile)
  Busy.start("Loading settings...")
  local t = Persist.readTable(settingsFile)
  if t then
    for name, var in pairs(variables) do
      local value = t[name]
      if value then var:set(value) end
    end
  else
    app.logInfo("No settings file found.")
  end
  Busy.stop()
end

local function scheduleSave()
  cancelScheduledSave()
  promiseToSave = Timer.after(5, function()
    promiseToSave = nil
    save()
  end)
end

local function saveIfNeeded()
  if promiseToSave then
    cancelScheduledSave()
    save()
  end
end

local function set(name, value)
  local var = variables[name]
  if var then
    var:set(value)
    scheduleSave()
  end
end

local function variable(name)
  return variables[name]
end

local function init()
  app.Card_mount(0)
  load()

  local screenSaver = variables["screenSaver"]
  if screenSaver and screenSaver.onSet then
    screenSaver.onSet(screenSaver.value)
  end

  syncFirmwareConfig(false)
end

return {
  init = init,
  get = get,
  set = set,
  variable = variable,
  save = save,
  load = load,
  scheduleSave = scheduleSave,
  saveIfNeeded = saveIfNeeded
}
