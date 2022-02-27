local Class = require "Base.Class"
local Object = require "Base.Object"

local Description = Class {}
Description:include(Object)

function Description:init()
  self:setClassName("Unit.Description")
end

function Description:serialize()
end

function Description:deserialize(t)
end

return Description
