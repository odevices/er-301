local Class = require "Base.Class"
local Source = require "Source"

local Internal = Class {}
Internal:include(Source)

function Internal:init(type, object, channel)
  Source.init(self, type)
  self:setClassName("Source.Internal")
  self:setInstanceName(object:getInstanceName())
  self.object = object
  self.channel = channel -- if mono, might be nil
end

function Internal:getOutlet()
  return self.object:getOutput(self.channel or 1)
end

function Internal:getDisplayName()
  return self.object:getOutputDisplayName(self.channel)
end

function Internal:serialize()
  return {
    name = self:getDisplayName(), -- just for readibility
    type = self.type,
    instanceKey = self.object:getInstanceKey(),
    channel = self.channel
  }
end

return Internal
