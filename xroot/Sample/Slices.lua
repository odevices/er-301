local app = app
local Class = require "Base.Class"
local Object = require "Base.Object"
local Path = require "Path"

local Slices = Class {}
Slices:include(Object)

function Slices:init()
  self:setClassName("Slices")
  self.pSlices = app.Slices()
end

function Slices:load(path)
  path = Path.expandRelativePath(path)
  if path == nil then
    return false
  end
  if app.pathExists(path) then
    if self.pSlices:load(path) then
      app.logInfo("%s:load(%s):success", self, path)
      return true
    else
      app.logInfo("%s:load(%s):failed", self, path)
      return false
    end
  else
    return false
  end
end

function Slices:loadWavFileCues(path)
  path = Path.expandRelativePath(path)
  if path == nil then
    app.logError("%s:loadWavFileCues: path is nil", self)
    return false
  end
  if app.pathExists(path) then
    if self.pSlices:loadWavFileCues(path) then
      app.logInfo("%s:loadWavFileCues(%s):success", self, path)
      return true
    else
      app.logInfo("%s:loadWavFileCues(%s):failed", self, path)
      return false
    end
  else
    return false
  end
end

function Slices:save(path)
  path = Path.expandRelativePath(path)
  if path == nil then
    app.logError("%s:save: failed, path is nil", self)
    return false
  end
  local result = self.pSlices:save(path)
  if result then
    app.logInfo("%s:save(%s):success", self, path)
  else
    app.logError("%s:save(%s):failed", self, path)
  end
  return result
end

function Slices:getCount()
  return self.pSlices:size()
end

function Slices:initFromLoadInfo(info)
  self.pSlices:initFromLoadInfo(info)
end

function Slices:copyFrom(from, to, slices, sourceStart)
  if self.pSlices and slices.pSlices then
    self.pSlices:copyFrom(from, to, slices.pSlices, sourceStart)
  end
end

function Slices:isTrivial(sampleCount)
  return self.pSlices:isTrivial(sampleCount)
end

return Slices
