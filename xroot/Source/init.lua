local app = app
local Class = require "Base.Class"
local Observable = require "Base.Observable"

local Source = Class {}
Source:include(Observable)

function Source:init(type)
  Observable.init(self)
  self:setClassName("Source")
  self.type = type
  self.refCount = 0
end

function Source:claim()
  self.refCount = self.refCount + 1
end

function Source:release()
  self.refCount = self.refCount - 1
end

function Source:getConnectionCount()
  return self.refCount
end

function Source:getDisplayName()
  return "???"
end

function Source:getChannelName()
  if self.channel == 1 then
    return "left"
  elseif self.channel == 2 then
    return "right"
  end
end

function Source:subscribe(o)
  Observable.subscribe(self, "onRenameSource", o)
  Observable.subscribe(self, "onDeleteSource", o)
end

function Source:unsubscribe(o)
  Observable.unsubscribe(self, "onRenameSource", o)
  Observable.unsubscribe(self, "onDeleteSource", o)
end

function Source:onRename()
  self:emitSignal("onRenameSource", self)
end

function Source:onDelete()
  self:emitSignal("onDeleteSource", self)
  self:unsubscribeAll()
end

local function findLegacySource(data, optionalChain)
  if optionalChain then
    -- first search for a custom unit control
    local chain = optionalChain
    while chain.unit and chain:getClassName() ~= "Patch" do
      chain = chain.unit.chain
    end
    if chain:getClassName() == "Patch" then
      local unit = chain.unit
      for name, branch in pairs(unit.branches) do
        if name == data then
          return branch:getOutputSource()
        end
      end
    end
  end

  -- second search for a global chain
  local GlobalChains = require "GlobalChains"
  local Utils = require "Utils"
  -- data is a key with format: {title}{left|right}
  -- Ex.: clock or voiceleft
  local chain = GlobalChains.getByName(data)
  local channel
  if chain then
    channel = 1
  elseif Utils.endsWith(data, "left") then
    local key = string.sub(data, 1, -5)
    chain = GlobalChains.getByName(key)
    channel = 1
  elseif Utils.endsWith(data, "right") then
    local key = string.sub(data, 1, -6)
    chain = GlobalChains.getByName(key)
    channel = 2
  end

  if chain then
    return chain:getOutputSource(channel)
  end
end

-- A Class function that returns a Source object (if found)
function Source.deserialize(data, optionalChain)
  local source

  if type(data) == "string" then
    source = app.getExternalSource(data) or
                 findLegacySource(data, optionalChain)
  elseif type(data) == "table" then
    if data.type == "global" then
      local GlobalChains = require "GlobalChains"
      local chain = GlobalChains.findByInstanceKey(data.instanceKey) or
                        GlobalChains.getByName(data.name)
      if chain == nil and data.originalInstanceKey then
        chain = GlobalChains.findByInstanceKey(data.originalInstanceKey)
      end
      if chain then
        source = chain:getOutputSource(data.channel or 1)
      end
    elseif data.type == "local" and optionalChain then
      local root = optionalChain:getRootChain()
      if root then
        local object = root:findByInstanceKey(data.instanceKey)
        if object == nil and data.originalInstanceKey then
          object = root:findByInstanceKey(data.originalInstanceKey)
        end
        if object then
          source = object:getOutputSource(data.channel or 1)
        end
      end
    end
  end

  return source
end

return Source
