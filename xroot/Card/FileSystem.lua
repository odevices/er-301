local Env = require "Env"
local Message = require "Message"
local Path = require "Path"
local Utils = require "Utils"
local Busy = require "Busy"

local function getVersionFolder()
  local t = Utils.split(app.FIRMWARE_VERSION, ".")
  local major = t[1] or 0
  local minor = t[2] or 0
  return string.format("/v%d.%d", major, minor)
end

local versionFolder = getVersionFolder()

local roots = {}

roots["x"] = app.roots.x

roots["rear"] = app.roots.rear
roots["rear-versioned"] = roots["rear"] .. versionFolder
roots["rear-meta"] = roots["rear-versioned"] .. "/meta"
roots["libs"] = roots["rear-versioned"] .. "/libs"
roots["package-configs"] = roots["rear-meta"] .. "/packages"

roots["front"] = app.roots.front .. "/ER-301"
roots["logs"] = roots["front"] .. "/logs"
roots["tmp"] = roots["front"] .. "/tmp"
roots["packages"] = roots["front"] .. "/packages"
roots["recordings"] = roots["front"] .. "/recorded"
roots["front-versioned"] = roots["front"] .. versionFolder
roots["front-meta"] = roots["front-versioned"] .. "/meta"
roots["screenshot"] = roots["front-versioned"] .. "/screenshots"
roots["quicksaves"] = roots["front-versioned"] .. "/quicksaves"
roots["presets"] = roots["front-versioned"] .. "/presets"
roots["unit-preset"] = roots["presets"]
roots["chain-preset"] = roots["presets"] .. "/chains"
roots["pool-preset"] = roots["presets"]
roots["6track-preset"] = roots["presets"]
roots["globals-preset"] = roots["presets"]
roots["preamp-preset"] = roots["presets"]
roots["control-preset"] = roots["presets"]

local extensions = {
  ["lua"] = "lua",
  ["unit-preset"] = "unit",
  ["chain-preset"] = "chain",
  ["pool-preset"] = "pool",
  ["6track-preset"] = "6track",
  ["globals-preset"] = "globals",
  ["preamp-preset"] = "preamp",
  ["quicksave"] = "save",
  ["audio"] = "wav",
  ["slices"] = "slc",
  ["multi"] = "multi",
  ["control-preset"] = "control",
  ["scala"] = "scl",
  ["package"] = "pkg",
  ["linkable"] = "so",
  ["log"] = "log"
}

local patterns = {
  ["audio"] = {
    "*.wav",
    "*.WAV",
    "*.multi"
  },
  ["audio-strict"] = {
    "*.wav",
    "*.WAV"
  },
  ["packageable-presets"] = {
    "*.unit",
    "*.chain"
  }
}

local validExtensions = {
  ["audio"] = {
    ["wav"] = true,
    ["multi"] = true
  },
  ["audio-strict"] = {
    ["wav"] = true
  },
  ["packageable-presets"] = {
    ["unit"] = true,
    ["chain"] = true
  }
}

local extHash = {}
for key, ext in pairs(extensions) do extHash[ext] = true end

local function getExt(key)
  return extensions[key] or "lua"
end

local function getPatternHelper(key, includeLua)
  local ext = extensions[key]
  if ext then
    if includeLua then
      return {
        "*." .. ext,
        "*.lua"
      }
    else
      return {
        "*." .. ext
      }
    end
  else
    if includeLua then
      return "*.lua"
    else
      return ""
    end
  end
end

local function getPattern(key, includeLua)
  return patterns[key] or getPatternHelper(key, includeLua)
end

local function isType(key, path)
  local ext = extensions[key]
  if ext == nil then
    app.logError("Uknown file type: " .. key)
    return false
  end
  return Utils.endsWith(string.lower(path), "." .. ext)
end

local function isValidExtension(key, ext)
  if ext == nil then return false end
  local supported = validExtensions[key]
  return supported and supported[string.lower(ext)]
end

local function getRoot(key)
  return roots[key or "system"] or
             app.logError("FileSystem.getRoot(%s): Not a valid root.", key)
end

local function showMessage(...)
  local dialog = Message.Main(...)
  dialog:show()
end

local function checkPath(key, mode, fullpath)
  if fullpath == nil then
    showMessage("Invalid path.")
    return false
  end
  local path, filename = Path.split(fullpath)
  if path then
    if filename then
      local ext = Path.getExtension(filename)
      if ext then
        ext = string.lower(ext)
        if extensions[key] == ext or ext == "lua" then
          return true
        else
          showMessage(ext, "is not a valid extension.")
          return false
        end
      else
        showMessage(fullpath, "does not have a valid extension.")
        return false
      end
    else
      showMessage(fullpath, "is not a valid path.")
      return false
    end
  else
    showMessage(fullpath, "is not a valid path.")
    return false
  end
end

local function findPreviousRoot()
  local currentVersion =
      Utils.convertVersionStringToNumber(app.FIRMWARE_VERSION)
  local previousVersion = 0
  local previousRoot
  -- Find the most recent version that is less than the current version.
  if Path.exists(roots["front"]) then
    for x in dir(roots["front"]) do
      local path = Path.join(roots["front"], x)
      if app.isDirectory(path) then
        local version = x:match("v(%d+[.]%d+)")
        if version then
          version = Utils.convertVersionStringToNumber(version)
          app.logInfo("FS.findPreviousRoot: found %s", path)
          if version > previousVersion and version < currentVersion then
            previousVersion = version
            previousRoot = path
          end
        end
      end
    end
  end
  app.logInfo("FS:findPreviousRoot: choosing %s", previousRoot)
  return previousRoot
end

local function init()
  Busy.start("Initializing file system...")
  local sysPath = roots["front-versioned"]
  if not Path.exists(sysPath) then
    app.logInfo("FS.init: %s does not exist, check for previous version.",
                sysPath)
    local rootForPreviousVersion = findPreviousRoot()
    if rootForPreviousVersion then
      Busy.start("Populating %s...", sysPath)

      -- Initialize the system root with the contents of the previous system root.
      app.logInfo("FS.init: Populating %s with %s.", sysPath,
                  rootForPreviousVersion)
      local quotedSysPath = sysPath:quote()
      for _, x in ipairs({
        "front-meta",
        "presets",
        "quicksaves"
      }) do
        local path = roots[x]
        local prevPath = path:gsub(quotedSysPath, rootForPreviousVersion)
        if Path.exists(prevPath) then
          app.logInfo("FS.init: Recursive copy %s to %s", prevPath, path)
          Path.recursiveCopy(prevPath, path)
        else
          app.logInfo("FS.init: Populating %s: not in previous version (%s)", x,
                      prevPath)
        end
      end

      -- Update any versioned paths stored in the front-meta tables.
      app.logInfo("FS.init: Replacing old paths in front-meta db files.")
      local Persist = require "Persist"
      local pattern = roots["front"]:quote() .. "/v[0-9]%.[0-9]"
      local hooks = {}
      hooks["string"] = function(item)
        return item:gsub(pattern, roots["front-versioned"])
      end
      for x in dir(roots["front-meta"]) do
        if Path.getExtension(x) == "db" then
          local path = Path.join(roots["front-meta"], x)
          local t = Persist.readTable(path)
          Persist.writeTable(path, t, hooks)
        end
      end

      Busy.stop()
    else
      app.logInfo("FS.init: No previous version found.")
    end
  else
    app.logInfo("FS.init: %s already exists, skipping population.", sysPath)
  end
  -- Make sure all root folders exist on disk.
  for key, path in pairs(roots) do Path.createAll(path) end
  Busy.stop();
end

return {
  getExt = getExt,
  getPattern = getPattern,
  getRoot = getRoot,
  checkPath = checkPath,
  isType = isType,
  isValidExtension = isValidExtension,
  init = init
}
