local Scala = require "core.Quantizer.Scala"
local Signal = require "Signal"
local Card = require "Card"
local FileSystem = require "Card.FileSystem"
local Path = require "Path"

local builtins = {
  ["12-TET"] = {
    100,
    200,
    300,
    400,
    500,
    600,
    700,
    800,
    900,
    1000,
    1100,
    1200
  },
  ["24-TET"] = {
    50,
    100,
    150,
    200,
    250,
    300,
    350,
    400,
    450,
    500,
    550,
    600,
    650,
    700,
    750,
    800,
    850,
    900,
    950,
    1000,
    1050,
    1100,
    1150,
    1200
  },
  ["Whole Tone"] = {
    200,
    400,
    600,
    800,
    1000,
    1200
  },
  Major = {
    200,
    400,
    500,
    700,
    900,
    1100,
    1200
  },
  ["Major Pentatonic"] = {
    200,
    400,
    700,
    900,
    1200
  },
  ["Minor Pentatonic"] = {
    300,
    500,
    700,
    1000,
    1200
  },
  ["Natural Minor"] = {
    200,
    300,
    500,
    700,
    800,
    1000,
    1200
  },
  ["Harmonic Minor"] = {
    200,
    300,
    500,
    700,
    800,
    1100,
    1200
  },
  Ionian = {
    200,
    400,
    500,
    700,
    900,
    1100,
    1200
  },
  Dorian = {
    200,
    300,
    500,
    700,
    900,
    1000,
    1200
  },
  Phrygian = {
    100,
    300,
    500,
    700,
    800,
    1000,
    1200
  },
  Lydian = {
    200,
    400,
    600,
    700,
    900,
    1100,
    1200
  },
  Mixolydian = {
    200,
    400,
    500,
    700,
    800,
    1000,
    1200
  },
  Aeolian = {
    200,
    300,
    500,
    700,
    800,
    1000,
    1200
  },
  Locrian = {
    100,
    300,
    500,
    600,
    800,
    1000,
    1200
  },
  ["22-Shruti"] = {
    90,
    112,
    182,
    204,
    294,
    316,
    386,
    408,
    498,
    520,
    590,
    612,
    702,
    792,
    814,
    884,
    906,
    996,
    1018,
    1088,
    1110,
    1200
  }
}

local builtinKeys = {
  "12-TET",
  "22-Shruti",
  "24-TET",
  "Major",
  "Harmonic Minor",
  "Natural Minor",
  "Major Pentatonic",
  "Minor Pentatonic",
  "Ionian",
  "Dorian",
  "Phrygian",
  "Lydian",
  "Mixolydian",
  "Aeolian",
  "Locrian",
  "Whole Tone"
}

local rootPath = Path.join(FileSystem.getRoot("front"), "scales")
local found = {}
local foundKeys = {}
local needRefresh = true
local needInit = true

local function saveErrorReport(msg, trace, logFile)
  local Card = require "Card"
  local reportSaved = false
  if Card.mounted() then
    local f = io.open(logFile, "w")
    if f then
      local Persist = require "Persist"
      f:write("---ERROR REPORT BEGIN\n")
      f:write(string.format("Time Since Boot: %0.3fs\n", app.wallclock()))
      f:write(string.format("Firmware Version: %s\n",
                            Persist.meta.boot.firmwareVersion))
      f:write(string.format("Boot Count: %d\n", Persist.meta.boot.count))
      f:write(string.format("Mount Count: %d\n", Persist.meta.mount.count))
      f:write("Error Message: ")
      f:write(msg .. "\n")
      f:write(trace .. "\n")
      local LogHistory = require "LogHistory"
      local count = LogHistory:count()
      if count > 0 then
        f:write("Recent Log Messages:\n")
        for i = 1, count do f:write(LogHistory:get(i), "\n") end
      end
      f:write("---ERROR REPORT END\n")
      f:close()
      app.logInfo("Error report written to '%s'.", logFile)
      reportSaved = true
    else
      app.logError("Failed to write '%s'.", logFile)
    end
  end
  return reportSaved
end

local function tryParseScala(fullPath, filename, headerOnly)
  local status, retval = xpcall(Scala.load, debug.traceback, fullPath,
                                headerOnly)
  if status then
    return retval
  else
    local logFile = Path.replaceExtension(fullPath, "log")
    local msg = string.format("Failed to parse: %s", filename)
    saveErrorReport(msg, retval, logFile)
  end
end

local function onCardMounted()
  needRefresh = true
  Path.createAll(rootPath)
end

local function init()
  if needInit then
    needInit = false
    Signal.register("cardMounted", onCardMounted)
    if Card.mounted() then onCardMounted() end
  end
end

local function refresh()
  init()
  if needRefresh and Card.mounted() and Path.exists(rootPath) then
    needRefresh = false
    found = {}
    foundKeys = {}
    for filename in dir(rootPath) do
      if FileSystem.isType("scala", filename) then
        local fullPath = Path.join(rootPath, filename)
        local data = tryParseScala(fullPath, filename)
        if data then
          local key = filename:gsub("[.]%w*$", "", 1)
          found[key] = data.tunings
          foundKeys[#foundKeys + 1] = key
        end
      end
    end
    return true
  else
    return false
  end
end

local function getScale(name)
  refresh()
  return found[name] or builtins[name]
end

local cachedKeys
local function getKeys()
  if refresh() or cachedKeys == nil then
    cachedKeys = {}
    for i, k in ipairs(foundKeys) do cachedKeys[#cachedKeys + 1] = k end
    for i, k in ipairs(builtinKeys) do cachedKeys[#cachedKeys + 1] = k end
    table.sort(cachedKeys)
  end
  return cachedKeys
end

return {
  getScale = getScale,
  getKeys = getKeys
}
