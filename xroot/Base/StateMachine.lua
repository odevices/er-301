local app = app
local Class = require "Base.Class"
local Object = require "Base.Object"
local State = require "Base.State"

local StateMachine = Class {}
StateMachine:include(Object)

local nullState = State("Null")

function StateMachine:init(initialState)
  self:setClassName("StateMachine")
  self.stack = {
    initialState or nullState
  }
end

function StateMachine:switch(to, ...)
  to = to or nullState
  local stack = self.stack
  local pre = stack[#stack]
  -- app.logInfo("%s.switch: %s --> %s",self,pre,to)
  if to ~= pre then
    pre:leave()
    stack[#stack] = to
    to:enter(pre, ...)
  end
end

function StateMachine:push(to, ...)
  to = to or nullState
  local stack = self.stack
  local pre = stack[#stack]
  pre:pause()
  stack[#stack + 1] = to
  to:enter(pre, ...)
end

function StateMachine:pop(...)
  local stack = self.stack
  if #stack < 2 then
    app.logError("%s.pop: no more states to pop", self)
  else
    local pre, to = stack[#stack], stack[#stack - 1]
    stack[#stack] = nil
    pre:leave()
    to:resume(pre, ...)
  end
end

function StateMachine:current()
  local stack = self.stack
  return stack[#stack]
end

function StateMachine:stateIsNull()
  local stack = self.stack
  return stack[#stack] == nullState
end

return StateMachine
