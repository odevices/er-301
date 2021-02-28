local app = app
local Class = require "Base.Class"
local Object = require "Base.Object"

local Mode = Class {}
Mode:include(Object)

function Mode:init(name)
  self:setClassName("Mode")
  self:setInstanceName(name)
  self.active = false
end

function Mode:enter()
  self.active = true
end

function Mode:leave()
  self.active = false
end

return Mode
