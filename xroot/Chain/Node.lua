local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Object = require "Base.Object"

local Node = Class {}
Node:include(Object)

function Node:init(id, name, o, type)
  self:setClassName("Chain.Node")
  self:setInstanceName(name)
  self.o = o
  self.type = type
end

return Node
