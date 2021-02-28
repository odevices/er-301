local app = app
local Class = require "Base.Class"
local Object = require "Base.Object"

local State = Class {}
State:include(Object)

function State:init(name)
  self:setClassName("State")
  self:setInstanceName(name)
  self.active = false
  self.paused = false
end

function State:enter(prevState, ...)
  self.active = true
  self.paused = false
end

function State:leave(...)
  self.active = false
  self.paused = false
end

function State:resume(prevState, ...)
  self.paused = false
end

function State:pause(...)
  self.paused = true
end

return State
