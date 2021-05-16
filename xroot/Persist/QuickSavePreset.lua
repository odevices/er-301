local Class = require "Base.Class"
local Preset = require "Persist.Preset"
local ChainPreset = require "Persist.ChainPreset"
local Env = require "Env"
local Busy = require "Busy"

local QuickSavePreset = Class {}
QuickSavePreset:include(Preset)

function QuickSavePreset:init(data)
  Preset.init(self, data)
  self:setClassName("QuickSavePreset")
  self:populateChainPresets()
end

function QuickSavePreset:populateChainPresets()
  local data = self.data
  self.chains = {}
  local legacy = data and data.chains
  local cdata = (data and data.channels and data.channels.chains) or legacy
  if cdata then
    for i, chain in ipairs(cdata) do self.chains[i] = ChainPreset(chain) end
  end
end

function QuickSavePreset:getSampleCount()
  if self.data and self.data.pool then
    return #self.data.pool
  else
    return 0
  end
end

function QuickSavePreset:getSamplePath(i)
  if self.data and self.data.pool then
    return self.data.pool[i].path or self.data.pool[i][2]
  else
    return nil
  end
end

function QuickSavePreset:populate()
  local Pool = require "Sample.Pool"
  local Channels = require "Channels"
  local recorder = require "FileRecorder"
  local GlobalChains = require "GlobalChains"
  local Preamp = require "Preamp"
  local Clipboard = require "Chain.Clipboard"
  local data = {
    firmwareVersion = app.FIRMWARE_VERSION
  }

  Busy.status("Serializing sample pool...")
  data.pool = Pool.serialize()

  Busy.status("Serializing global chains...")
  data.globals = GlobalChains.serialize()

  Busy.status("Serializing preamp settings...")
  data.preamp = Preamp.serialize()

  Busy.status("Serializing multitrack recorder...")
  data.recorder = recorder:serialize()

  Busy.status("Serializing clipboard...")
  data.clipboard = Clipboard.serialize()

  Busy.status("Serializing channels...")
  data.channels = Channels.serialize()

  self.data = data
end

function QuickSavePreset:apply()
  local data = self.data
  if data == nil then
    app.logInfo("%s:apply: no data.", self)
    return false
  end

  local result = true
  local Channels = require "Channels"
  Channels.mute()
  Channels.stop()
  Channels.clear()
  
  local Pool = require "Sample.Pool"
  Pool.clear()

  if data.preamp then
    local Preamp = require "Preamp"
    Preamp.deserialize(data.preamp)
  else
    app.logInfo("%s:apply: no preamp data.", self)
  end

  if data.globals then
    local GlobalChains = require "GlobalChains"
    GlobalChains.deserialize(data.globals)
  else
    app.logInfo("%s:apply: no globals data.", self)
  end

  local Settings = require "Settings"
  if Settings.get("quickSaveRestoresRecorder") == "yes" then
    if data.recorder then
      local recorder = require "FileRecorder"
      recorder:deserialize(data.recorder)
    else
      app.logInfo("%s:apply: no multitrack preset.", self)
    end
  end

  if Settings.get("quickSaveRestoresClipboard") == "yes" then
    if data.clipboard then
      local Clipboard = require "Chain.Clipboard"
      Clipboard.deserialize(data.clipboard)
    else
      app.logInfo("%s:apply: no clipboard data.", self)
      local Clipboard = require "Chain.Clipboard"
      Clipboard.clear()
    end
  end

  if data.channels and data.chains then
    Channels.deserializeLegacy(data.channels, data.chains)
  elseif data.channels then
    Channels.deserialize(data.channels)
  else
    app.logInfo("%s:apply: no channels data.", self)
    result = false
  end

  -- Sample Pool deserialization should be after Channels deserialization
  -- because buffers should deserialized in the chain context.
  if data.pool then
    Pool.deserialize(data.pool)
  else
    app.logInfo("%s:apply: no sample pool data.", self)
    result = false
  end

  -- Channels.start()
  -- Channels.unmute()

  return result
end

local function summarizeHelper(used, prefix, n)
  local keys = {}
  local exists = false
  for i = 1, n do
    keys[i] = prefix .. i
    if used[keys[i]] then exists = true end
  end
  if exists then
    local x = prefix
    for i, key in ipairs(keys) do
      if used[key] then
        x = x .. i
        used[key] = nil
      end
    end
    used[x] = true
  end
end

local function summarizeUsedInputs(used)
  summarizeHelper(used, "A", 3)
  summarizeHelper(used, "B", 3)
  summarizeHelper(used, "C", 3)
  summarizeHelper(used, "D", 3)
  summarizeHelper(used, "G", 4)
  summarizeHelper(used, "IN", 4)
end

function QuickSavePreset:getUsedInputs(summarize)
  local used = {}
  for _, chain in ipairs(self.chains) do chain:getUsedInputs(used) end
  if summarize then summarizeUsedInputs(used) end
  local result = {}
  for ch, _ in pairs(used) do result[#result + 1] = ch end
  table.sort(result)
  return result
end

function QuickSavePreset:getGlobalChainCount()
  if self.data then
    return #self.data.globals
  else
    return 0
  end
end

function QuickSavePreset:getChainCount()
  return #self.chains
end

function QuickSavePreset:getChainPreset(i)
  return self.chains[i]
end

function QuickSavePreset:getUnitCount(chain, recursive)
  local chainPreset = self.chains[chain]
  if chainPreset then
    return chainPreset:getUnitCount(recursive)
  else
    return 0
  end
end

function QuickSavePreset:getChannelCount(chain)
  local chainPreset = self.chains[chain]
  if chainPreset then
    return chainPreset:getChannelCount()
  else
    return 0
  end
end

function QuickSavePreset:getLinks()
  if self.data.channels == nil then return end
  local links = self.data.channels.links
  if links == nil then
    -- legacy
    local tmp = {
      false,
      false,
      false
    }
    for _, i in ipairs(self.data.channels) do tmp[i] = true end
    links = {
      link12 = tmp[1],
      link23 = tmp[2],
      link34 = tmp[3]
    }
  end
  return links
end

return QuickSavePreset
