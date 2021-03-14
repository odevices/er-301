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
  if path then
    path = Path.checkRelativePath(path)
    self.path = path
  end
  if path == nil then return false end
  if app.pathExists(path) then
    if self.pSlices:load(path) then
      app.logInfo("%s: loaded %s.", self, path)
      return true
    else
      app.logInfo("%s: failed to load %s.", self, path)
      return false
    end
  else
    return false
  end
end

function Slices:save(path)
  if path then
    self.path = path
  else
    path = self.path
  end
  if path == nil then return false end
  local result = self.pSlices:save(path)
  if result then
    -- app.logInfo("%s:saved:%s",self,path)
  else
    app.logError("%s:failed to save:%s", self, path)
  end
  return result
end

function Slices:setSampleRate(rate)
  self.pSlices:setSampleRate(rate)
  self.rate = rate
end

function Slices:getCount()
  return self.pSlices:size()
end

function Slices:initFromLoadInfo(info)
  self.pSlices:initFromLoadInfo(info)
end

function Slices:serialize()
  local t = {}
  t.path = self.path
  t.rate = self.rate
  if not self:save() then
    app.logError("%s:serialize: unable to save slices to %s.", self, self.path)
  end
  return t
end

function Slices:deserialize(t)
  if t.path then
    if not self:load(t.path) then
      app.logError("%s:deserialize: unable to load slices from %s.", self,
                  self.path)
    end
  end
  if t.rate then self:setSampleRate(t.rate) end
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
