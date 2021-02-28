local Class = require "Base.Class"
local Object = require "Base.Object"
local __weak__ = {
  __mode = "k"
}
local __strong__ = {}

local Observable = Class {}
Observable:include(Object)

function Observable:init()
  self:setClassName("Observable")
  self.signals = {}
end

local function createSignal(o, s)
  local listeners = setmetatable({}, __strong__)
  o.signals[s] = listeners
  return listeners
end

-- WARNING! These are strong references to the subscriber
function Observable:subscribe(s, f)
  local listeners = self.signals[s] or createSignal(self, s)
  listeners[f] = true
end

function Observable:unsubscribe(s, f)
  local listeners = self.signals[s] or createSignal(self, s)
  listeners[f] = nil
end

function Observable:unsubscribeAll(s)
  if self.signals[s] then self.signals[s] = setmetatable({}, __strong__) end
end

function Observable:emitSignal(s, ...)
  local listeners = self.signals[s]
  if listeners == nil then return end
  -- Move invocations outside of the loop
  -- in case a handler manipulates the signal table.
  local tmp = {}
  for x, _ in pairs(listeners) do tmp[#tmp + 1] = x end
  for _, x in ipairs(tmp) do
    if type(x) == "table" then
      x[s](x, ...)
    else
      x(...)
    end
  end
end

return Observable
