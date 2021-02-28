local slots = {}

local function copy(chain,slot,isCut)
  local data, index0, index1 = chain:serializeMarkedUnits()
  data.isCut = isCut
  slots[slot] = data
  return index0, index1
end

local function paste(chain,position,slot)
  local data = slots[slot]
  if data then
    if position==nil then
      -- load before the currently selected unit
      position = chain:getSelectedSectionPosition() - 1
      if position < 1 then
        position = 1
      end
    end
    local Persist = require "Persist"
    if data.isCut then
      -- Suppress key regeneration for cuts.
      -- Only the first paste is a cut.
      data.isCut = false
    else
      Persist.regenerateInstanceKeys(data)
    end
    chain:deserializeUnits(data,position)
  end
end

local function clear(slot)
  if slot then
    slots[slot] = nil
  else
    slots = {}
  end
end

local function hasData(slot)
  return slots[slot]~=nil
end

local function serialize()
  return slots
end

local function deserialize(data)
  slots = data
end

local function describe(slot)
  local data = slots[slot]
  if data==nil then
    return "nothing"
  elseif #data==1 then
    return "1 unit"
  else
    return string.format("%d units",#data)
  end
end

return {
  copy = copy,
  paste = paste,
  clear = clear,
  hasData = hasData,
  describe = describe,
  serialize = serialize,
  deserialize = deserialize
}
