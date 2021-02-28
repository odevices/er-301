local Class = require "Base.Class"
local Preset = require "Persist.Preset"

local ChainPreset = Class {}
ChainPreset:include(Preset)

function ChainPreset:init(data)
  Preset.init(self, data)
  self:setClassName("ChainPreset")
end

function ChainPreset:read(fullpath)
  if Preset.read(self, fullpath) then
    local Persist = require "Persist"
    Persist.regenerateInstanceKeys(self.data)
    return true
  else
    return false
  end
end

local countUnitsInUnit

local function countUnitsInChain(chain)
  local nUnits = #chain.units
  local result = nUnits
  for i = 1, nUnits do
    local unit = chain.units[i]
    result = result + countUnitsInUnit(unit)
  end
  return result
end

-- already declared as local
function countUnitsInUnit(unit)
  local result = 0
  if unit.branches then
    for bname, branch in pairs(unit.branches) do
      result = result + countUnitsInChain(branch)
    end
  end
  if unit.controlBranches then
    for bname, branch in pairs(unit.controlBranches) do
      result = result + countUnitsInChain(branch)
    end
  end
  if unit.bands then
    for i, chain in ipairs(unit.bands) do
      result = result + countUnitsInChain(chain)
    end
  end
  return result
end

function ChainPreset:getUnitCount(recursive)
  if self.data then
    if recursive then
      return countUnitsInChain(self.data)
    else
      return #self.data.units
    end
  else
    return 0
  end
end

function ChainPreset:getUnit(i)
  if self.data.units then
    local UnitPreset = require "Persist.UnitPreset"
    return UnitPreset(self.data.units[i])
  else
    return nil
  end
end

function ChainPreset:getChannelCount()
  if self.data.channels and self.data.channels.count then
    return self.data.channels.count
  else
    return 0
  end
end

return ChainPreset
