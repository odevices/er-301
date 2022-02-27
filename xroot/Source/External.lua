local Class = require "Base.Class"
local Source = require "Source"

local External = Class {}
External:include(Source)

function External:init(name, outlet, isUnipolar)
  Source.init(self, "external")
  self:setClassName("Source.External")
  self:setInstanceName(name)
  self.name = name
  self.outlet = outlet
  self.isUnipolar = isUnipolar
end

function External:getOutlet()
  return self.outlet
end

function External:getDisplayName()
  return self.name
end

function External:serialize()
  return self.name
end

return External
