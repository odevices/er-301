local Class = require "Base.Class"
local Object = require "Base.Object"
local Path = require "Path"
local Utils = require "Utils"
local Serialization = require "Persist.Serialization"
local Busy = require "Busy"

local Writer = Class {
  packagers = {}
}
Writer:include(Object)

function Writer:init()
  self:setClassName("Package.Writer")
end

function Writer:open(fullpath)
  self:close()
  local archive = app.ZipArchiveWriter()
  if not archive:open(fullpath, true) then
    app.logInfo("Failed to create zip archive at %s.", fullpath)
    return false
  end
  self.archive = archive
  local filename = Path.getFilename(fullpath)
  self.fullpath = fullpath
  self.filename = filename
  self.id = Utils.removeExtension(filename)
  return true
end

function Writer:addFile(srcPath, dstPath)
  local ext = Path.getExtension(srcPath)
  local f = ext and Writer.packagers[ext]
  if f then
    f(self, srcPath, dstPath)
  else
    Busy.status("%s: adding %s", self.id, Utils.shortenPath(dstPath, 25))
    self.archive:add(srcPath, dstPath)
  end
end

function Writer:close()
  if self.archive then
    self.archive:close()
    self.archive = nil
  end
end

-- Absolute paths are used for *.wav and are also inside *.multi files.
local function packagePreset(writer, srcPath, dstPath)
  local FileSystem = require "Card.FileSystem"
  local filename = Path.getFilename(srcPath)
  local tmpPath = Path.join(FileSystem.getRoot("tmp"), filename)
  -- Write out the preset data while replacing absolute paths.
  local hooks = {}
  local replaced = {}
  hooks["string"] = function(item)
    if Utils.startsWith(item, app.roots.rear .. "/") then
      app.logInfo("collecting %s", item)
      local newPath = item:gsub("^" .. app.roots.rear, "rear")
      replaced[item] = newPath
      return newPath
    elseif Utils.startsWith(item, app.roots.front .. "/") then
      app.logInfo("collecting %s", item)
      local newPath = item:gsub("^" .. app.roots.front, "front")
      replaced[item] = newPath
      return newPath
    else
      return item
    end
  end
  local data = Serialization.readTable(srcPath)
  Serialization.writeTable(tmpPath, data, hooks)
  Busy.status("%s: adding %s", writer.id, filename)
  writer.archive:add(tmpPath, dstPath)
  app.deleteFile(tmpPath)
  -- Collect assets that were found while walking the preset table.
  for k, v in pairs(replaced) do
    writer:addFile(k, v)
  end
  return true
end

Writer.packagers["unit"] = packagePreset
Writer.packagers["chain"] = packagePreset

return Writer
