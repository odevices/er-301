local app = app
local Signal = require "Signal"
local GlobalChain = require "GlobalChains.GlobalChain"

local chains = {}
local chainCount = 0

local function exists(name)
  return chains[name] ~= nil
end

local function create(name, numChannels, position, continues)
  if exists(name) then return end
  local chain = GlobalChain {
    title = name,
    channelCount = numChannels
  }
  chains[name] = chain
  chainCount = chainCount + 1
  -- add chain to audio engine
  chain:start()
  Signal.emit("onCreateGlobalChain", chain, name, position)
  if not continues then Signal.emit("onGlobalChainCountChanged", chainCount) end
  return chain
end

local function rename(oldName, newName)
  local chain = chains[oldName]
  if chain == nil then return end
  chains[oldName] = nil
  chains[newName] = chain
  chain:setTitle(newName)
  Signal.emit("onRenameGlobalChain", chain, oldName, newName)
end

local function delete(name, continues)
  local chain = chains[name]
  if chain == nil then return end
  local Busy = require "Busy"
  Busy.start("Deleting: %s", name)
  Signal.emit("onDeleteGlobalChain", chain, name)
  chains[name] = nil
  chainCount = chainCount - 1
  -- remove chain from audio engine
  if not chain:isMuted() then chain:mute() end
  chain:stop()
  chain:releaseResources()
  chain:hide()
  if not continues then Signal.emit("onGlobalChainCountChanged", chainCount) end
  Busy.stop()
end

local function deleteAll()
  for name, chain in pairs(chains) do delete(name, true) end
  Signal.emit("onGlobalChainCountChanged", chainCount)
end

local function serialize()
  local interface = require "GlobalChains.Interface"
  local ordered = interface:getOrderedChains()
  local t = {}
  for i, chain in ipairs(ordered) do
    t[#t + 1] = {
      name = chain.title,
      channelCount = chain.channelCount,
      chainData = chain:serialize()
    }
    app.logInfo("global chain %d: %s", i, chain.title)
  end
  return t
end

local function reverseTable(t)
  local reversedTable = {}
  local itemCount = #t
  for k, v in ipairs(t) do reversedTable[itemCount + 1 - k] = v end
  return reversedTable
end

local function deserialize(t)
  for name, chain in pairs(chains) do delete(name, true) end

  if #t == 0 then
    if t.chains then
      -- legacy support
      local data = t.chains
      t = {}
      for name, chain in pairs(data) do t[#t + 1] = chain end
      app.logInfo("GlobalChains.deserialize:loading %d legacy chains", #t)
    else
      app.logInfo("GlobalChains.deserialize: no chains")
      return
    end
  end

  for i, data in ipairs(t) do
    if data.name and data.channelCount then
      local chain = create(data.name, data.channelCount, i, true)
      if data.chainData then
        chain:deserialize(data.chainData)
      else
        app.logInfo("GlobalChains.deserialize: %s has no chainData.", data.name)
      end
    else
      app.logInfo("GlobalChains.deserialize: %s has no name or channelCount",
              data.name)
    end
  end

  Signal.emit("onGlobalChainCountChanged", chainCount)
end

local function getByName(name)
  return chains[name]
end

local function getByDisplayName(name)
  local chain
  local Utils = require "Utils"
  -- data is a key with format: {title}{left|right}
  -- Ex.: clock or voiceleft
  chain = getByName(name)
  local channel
  if chain then
    channel = 1
  elseif Utils.endsWith(name, "left") then
    local key = string.sub(name, 1, -5)
    chain = getByName(key)
    channel = 1
  elseif Utils.endsWith(name, "right") then
    local key = string.sub(name, 1, -6)
    chain = getByName(key)
    channel = 2
  end

  if chain then return chain:getOutputSource(channel) end
end

local function findByInstanceKey(key)
  for name, chain in pairs(chains) do
    if key == chain:getInstanceKey() then return chain end
  end
end

local function count()
  return chainCount
end

return {
  create = create,
  rename = rename,
  delete = delete,
  deleteAll = deleteAll,
  getByName = getByName,
  exists = exists,
  serialize = serialize,
  deserialize = deserialize,
  chains = chains,
  findByInstanceKey = findByInstanceKey,
  count = count
}
