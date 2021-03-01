local Class = require "Base.Class"
local Object = require "Base.Object"
local Card = require "Card"
local Path = require "Path"
local Utils = require "Utils"
local FS = require "Card.FileSystem"

--[[
A Package is a zip file containing:
- toc.lua: a description script
- Files to be installed, usually elf files and lua files.

The name of the archive is <unique-name>-<major>.<minor>.<build>.pkg
Ex.: loopers-1.0.2.pkg

Package states:
- installed
- not installed
- missing dependencies
- malformed

Notes:
- Side-by-side installation of the same package with different versions is not supported.
- Installed packages not in the local repository are automatically uninstalled on card mount.

Using a package in your code:

local Stuff = require "ACoolPackage."

]] --

local Package = Class {
  packagers = {}
}
Package:include(Object)

function Package:init(filename)
  self:setClassName("Package")
  -- package id is the archive filename (w/o the extension)
  self.filename = filename
  self.id = Utils.removeExtension(filename)
  self:setInstanceName(self.id)
  self.installed = false
end

function Package:getTOC()
  if self.toc then return self.toc end
  local pathToArchive = self:getArchivePath()
  if not Card.mounted() or not Path.exists(pathToArchive) then return end

  local archive = app.ZipArchiveReader()
  if not archive:open(pathToArchive) then
    app.logInfo("%s: Failed to open package archive: %s", self, pathToArchive)
    return
  end

  -- Read the toc script into a string
  local chunk = archive:extractToString("toc.lua")
  archive:close()

  if chunk == "" then
    app.logInfo("%s: Failed to extract 'toc.lua' from %s.", self, pathToArchive)
    return
  end

  -- Compile the toc script
  local f, e = load(chunk)
  if f then
    -- Execute the toc script
    local status, ret = xpcall(f, debug.traceback)
    if status then
      archive:close()
      ret.external = true
      ret.name = self:getName()
      self.toc = ret
      return ret
    else
      app.logInfo("Failed to execute 'toc.lua' from %s.", pathToArchive)
      app.logInfo("Traceback of 'toc.lua':%s", ret)
    end
  else
    app.logInfo("Failed to load 'toc.lua' from %s.", pathToArchive)
    app.logInfo(e)
  end
end

-- Package name is the id without version.
function Package:getName()
  return self.id:match("^(.*)-.*$") or self.id
end

-- Package version is parsed from the id: <name>-<major>.<minor>.<build>
function Package:getVersion()
  local id = self.id
  return id:match("-(%d+[.]%d+[.]%d+)") or id:match("-(%d+[.]%d+)") or
             id:match("-(%d+)") or "0.0.0"
end

function Package:getInstallationPath()
  return Path.join(FS.getRoot("libs"), self:getName())
end

function Package:getArchivePath()
  return Path.join(FS.getRoot("packages"), self.filename)
end

function Package:getDependencies()
  local toc = self:getTOC()
  return toc and toc.requires
end

function Package:getUnits()
  local toc = self:getTOC()
  return toc and toc.units
end

function Package:getTitle()
  local toc = self:getTOC()
  return toc and toc.title
end

function Package:getAuthor()
  local toc = self:getTOC()
  return toc and toc.author
end

function Package:satisfies(dep)
  if type(dep) == "string" then
    return self:getName() == dep
  elseif type(dep) == "table" then
    if #dep == 1 then
      return self:getName() == dep[1]
    elseif #dep == 2 then
      local minVersion = Utils.convertVersionStringToNumber(dep[2])
      local version = Utils.convertVersionStringToNumber(self:getVersion())
      return self:getName() == dep[1] and version >= minVersion
    elseif #dep > 2 then
      local minVersion = Utils.convertVersionStringToNumber(dep[2])
      local maxVersion = Utils.convertVersionStringToNumber(dep[3])
      local version = Utils.convertVersionStringToNumber(self:getVersion())
      return self:getName() == dep[1] and version >= minVersion and version <=
                 maxVersion
    else
      return false
    end
  else
    return false
  end
end

function Package:libraryCall(method, ...)
  local library = self:getLibrary()
  return library:safeCall(method, ...)
end

function Package:onLoad()
  self:libraryCall("onLoad")
end

function Package:onUnload()
  if self.library then
    self.library:safeCall("onUnload")
    self.library = nil
  end
end

function Package:getLibrary()
  if self.library then return self.library end
  local pathToInstallation = self:getInstallationPath()
  local pathToInit = Path.join(pathToInstallation, "init.lua")
  if Path.exists(pathToInit) then
    local instantiate = function()
      local M = require(self:getName())
      return M {
        package = self
      }
    end
    local status, ret = xpcall(instantiate, debug.traceback)
    if status then
      self.library = ret
      return ret
    else
      app.logInfo("%s: Failed to instantiate.", self)
      app.logInfo("Traceback:%s", ret)
    end
  else
    app.logInfo("%s: No 'init.lua' to execute.", self)
  end
  -- Package cannot provide a valid library, so create the default one.
  local Library = require "Package.Library"
  self.library = Library {
    package = self
  }
  return self.library
end

return Package
