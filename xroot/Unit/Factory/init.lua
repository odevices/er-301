local Utils = require "Utils"
local Signal = require "Signal"
local Path = require "Path"
local Card = require "Card"
local FS = require "Card.FileSystem"
local Library = require "Unit.Factory.Library"

-- Save the initial value of the package path
local initialPackagePath = package.path

-- External libraries
local IgnoredFolderNames = {}
local ExternalLibraryList = {}

-- These libraries will be loaded when LoadLibraries() is called.
local RegisteredLibraryList = {}
local RegisteredLibraryHash = {}

-- The result of loading all registered libraries.
local LoadedLibraryList = {}
local LoadedLibraryHash = {}
local CategoryList = {}
local CategoryHash = {}

local function addToPackagePath(pathToFolder)
  local path = string.format(";%s/?.lua;%s/?/init.lua", pathToFolder,
                             pathToFolder)
  package.path = package.path .. path
  app.logInfo("Package Path: %s", package.path)
end

local function resetPackagePath()
  package.path = initialPackagePath
end

local function getLoadInfo(libraryKey, unitKey)
  local library = LoadedLibraryHash[libraryKey]
  if library then return library:find(unitKey) end
end

local function saveErrorReport(msg, trace, logFile)
  local Card = require "Card"
  local reportSaved = false
  if Card.mounted() then
    local f = io.open(logFile, "a+")
    if f then
      local Persist = require "Persist"
      f:write("---ERROR REPORT BEGIN\n")
      f:write(string.format("Time Since Boot: %0.3fs\n", app.wallclock()))
      if Persist.meta and Persist.meta.boot then
        f:write(string.format("Firmware Version: %s\n",
                              Persist.meta.boot.firmwareVersion))
        f:write(string.format("Boot Count: %d\n", Persist.meta.boot.count))
        f:write(string.format("Mount Count: %d\n", Persist.meta.mount.count))
      end
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
      app.logInfo("Error report appended to '%s'.", logFile)
      reportSaved = true
    else
      app.logError("Failed to write '%s'.", logFile)
    end
  end
  return reportSaved
end

local function onErrorInExternal(library, origLoadInfo, usedLoadInfo, traceback)
  local logFile = Path.join(FS.getRoot("logs"), library.name..".log")
  local msg = string.format("Failed to construct unit: %s",
                            origLoadInfo.moduleName)
  if saveErrorReport(msg, traceback, logFile) then
    local Message = require "Message"
    local line1 = string.format("Error in %s (%s).", usedLoadInfo.moduleName,
                                library.name)
    local line2 = string.format("Details saved to: %s", logFile)
    local dialog = Message.Main(line1, line2)
    dialog:show()
  end
end

local function instantiateFromLibrary(library, loadInfo, args)
  local loadInfoFound = library:find(loadInfo.moduleName) or loadInfo
  local modulePath = library.name .. "." .. loadInfoFound.moduleName
  -- First try to load the unit's module
  local status, retval = xpcall(require, debug.traceback, modulePath)
  if status then
    args.loadInfo = loadInfoFound
    -- Now try to construct the unit
    local status2, retval2 = xpcall(retval, debug.traceback, args)
    if status2 then
      return retval2
    else
      app.logError("Factory.instantiate(%s):construction failed",
                   loadInfo.moduleName)
      app.logInfo("Traceback of unit construction:%s", retval2)
      if library.external then
        onErrorInExternal(library, loadInfo, loadInfoFound, retval2)
      end
    end
  else
    app.logError("Factory.instantiate(%s):require failed", loadInfo.moduleName)
    app.logInfo("Traceback of call to 'require(%s)':%s", modulePath, retval)
    if library.external then
      onErrorInExternal(library, loadInfo, loadInfoFound, retval)
    end
  end
end

local librarySearchOrder = {
  "builtins",
  "core"
}

local function instantiate(loadInfo, args)
  if loadInfo == nil then
    app.logError("Factory.instantiate(nil): invalid loadInfo.")
    return
  end
  -- First try the library given in the loadInfo.
  local library = LoadedLibraryHash[loadInfo.libraryName]
  if library and library:find(loadInfo.moduleName) then
    return instantiateFromLibrary(library, loadInfo, args)
  else
    -- Then try searching other libraries for a match.
    app.logWarn(
        "Factory.instantiate(%s.%s): not found, trying other libraries.",
        loadInfo.libraryName, loadInfo.moduleName)
    for _, libraryName in ipairs(librarySearchOrder) do
      local library = LoadedLibraryHash[libraryName]
      if library and library:find(loadInfo.moduleName) then
        local unit = instantiateFromLibrary(library, loadInfo, args)
        if unit then
          app.logInfo("Factory.instantiate(%s.%s): found replacement in %s.",
                      loadInfo.libraryName, loadInfo.moduleName, library.name)
          return unit
        end
      end
    end
  end
end

local function load(library)
  LoadedLibraryList[#LoadedLibraryList + 1] = library
  LoadedLibraryHash[library.name] = library
  for _, category in ipairs(library.categoryList) do
    if not CategoryHash[category] then
      CategoryHash[category] = true
      CategoryList[#CategoryList + 1] = category
    end
  end
end

local function loadLibraryFromTable(t)
  local library = Library()
  if library:loadFromTable(t) then load(library) end
end

local function loadLibraries()
  for _, t in ipairs(LoadedLibraryList) do if t.onUnload then t.onUnload() end end
  LoadedLibraryList = {}
  LoadedLibraryHash = {}
  CategoryList = {}
  CategoryHash = {}
  for _, t in ipairs(RegisteredLibraryList) do
    loadLibraryFromTable(t)
    if t.onLoad then t.onLoad() end
  end
end

-----------------------------------
-- Public Interface

-- Note: 'category' and 'channelCount' are both optional.
local function getUnits(category, channelCount)
  local t = {}
  for _, library in ipairs(LoadedLibraryList) do
    library:getUnits(t, category, channelCount)
  end
  return t
end

local function getUnitsByLibrary(libraryKey)
  local library = LoadedLibraryHash[libraryKey]
  if library then
    return library.unitList
  else
    return {}
  end
end

local function getCategories()
  return CategoryList
end

local function getBuiltin(name)
  if name == nil then return nil end
  name = name:gsub("/", ".")
  local loadInfo = getLoadInfo("builtins", name)
  if loadInfo then
    return loadInfo
  else
    app.logError("Unit.Factory.getBuiltin: Unable to find [%s].", name)
  end
end

local function serializeLoadInfo(loadInfo)
  local t = Utils.shallowCopy(loadInfo)
  if loadInfo.args then t.args = Utils.shallowCopy(loadInfo.args) end
  return t
end

local function deserializeLoadInfo(t, legacy)
  if t then return t end
  -- legacy support
  return getBuiltin(legacy.loadName)
end

local function clearLibraryModulesFromCache(name)
  local prefix = name .. "."
  for modname, _ in pairs(package.loaded) do
    if Utils.startsWith(modname, prefix) then
      -- app.logInfo("Removing loaded module: %s",modname)
      package.loaded[modname] = nil
    end
  end
end

local function unregisterLibrary(name, refresh)
  if RegisteredLibraryHash[name] then
    for i, t in ipairs(RegisteredLibraryList) do
      if t.name == name then
        if t.onUnregister then t.onUnregister() end
        table.remove(RegisteredLibraryList, i)
        break
      end
    end
    RegisteredLibraryHash[name] = nil
  end
  if refresh then loadLibraries() end
end

local function registerLibrary(t, refresh)
  if t.name == nil then return false end
  if RegisteredLibraryHash[t.name] then unregisterLibrary(t.name) end
  clearLibraryModulesFromCache(t.name)
  RegisteredLibraryList[#RegisteredLibraryList + 1] = t
  RegisteredLibraryHash[t.name] = t
  if t.onRegister then t.onRegister() end
  if refresh then loadLibraries() end
  return true
end

-- Executes the '<folderName>/toc.lua' file and appends the returned table to ExternLibraryList[].
local function registerExternalLibrary(folderName, verbose)
  local libRoot = FS.getRoot("libs")
  local fullPath = Path.join(libRoot, folderName, "toc.lua")
  local f, errorMessage = loadfile(fullPath)
  if f then
    local status, result = xpcall(f, debug.traceback)
    if status then
      result.external = true
      -- Replace the library name with the folder name.
      result.name = folderName
      if registerLibrary(result) then
        ExternalLibraryList[#ExternalLibraryList + 1] = result
        return true
      else
        return false
      end
    elseif verbose then
      local logFile = Path.join(libRoot, folderName, "error.log")
      app.logError("UnitLibrary(%s): Failed to load toc.lua.", folderName)
      saveErrorReport("Failed to register.", result, logFile)
    end
  elseif verbose then
    app.logError("UnitLibrary(%s): Failed to open toc.lua.", folderName)
    app.logInfo(errorMessage)
  end
end

local function reloadLibraries()
  -- Unregister external libraries...
  for _, t in ipairs(ExternalLibraryList) do unregisterLibrary(t.name) end
  if Card.mounted() then
    local libRoot = FS.getRoot("libs")
    -- Register external libraries...
    if Path.exists(libRoot) then
      for folderName in dir(libRoot) do
        if not IgnoredFolderNames[folderName] then
          if registerExternalLibrary(folderName, true) then
            app.logInfo("Unit library found: %s", folderName)
          else
            app.logError("Unit library failed to register: %s", folderName)
          end
        end
      end
    end
  end
  loadLibraries()
  Signal.emit("onUnitListChanged")
end

local function init()
  local builtins = require "builtins.toc"
  registerLibrary(builtins, true)
  addToPackagePath(FS.getRoot("libs"))
end

return {
  instantiate = instantiate,
  getCategories = getCategories,
  getUnitsByLibrary = getUnitsByLibrary,
  getUnits = getUnits,
  serializeLoadInfo = serializeLoadInfo,
  deserializeLoadInfo = deserializeLoadInfo,
  getBuiltin = getBuiltin,
  registerLibrary = registerLibrary,
  registerExternalLibrary = registerExternalLibrary,
  unregisterLibrary = unregisterLibrary,
  reloadLibraries = reloadLibraries,
  init = init
}
