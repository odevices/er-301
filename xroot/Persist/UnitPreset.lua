local Class = require "Base.Class"
local Preset = require "Persist.Preset"
local FileSystem = require "Card.FileSystem"

local UnitPreset = Class {}
UnitPreset:include(Preset)

function UnitPreset:init(data)
  Preset.init(self, data)
  self:setClassName("UnitPreset")
end

function UnitPreset:getLoadInfo()
  return self.data and self.data.loadInfo
end

function UnitPreset:read(fullpath)
  if Preset.read(self, fullpath) then
    local Persist = require "Persist"
    Persist.regenerateInstanceKeys(self.data)
    return true
  else
    return false
  end
end

local function nextBranch(t, k)
  local k2, v2 = next(t, k)
  if v2 then
    local ChainPreset = require "Persist.ChainPreset"
    return k2, ChainPreset(v2)
  else
    return nil
  end
end

function UnitPreset:iterateBranches()
  return nextBranch, self.data.branches, nil
end

function UnitPreset:getBranch(cname)
  if self.data.branches then
    local ChainPreset = require "Persist.ChainPreset"
    return ChainPreset(self.data.branches[cname])
  else
    return nil
  end
end

return UnitPreset
