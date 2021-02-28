local Class = require "Base.Class"
local MultiBand = require "core.Container.MultiBand"

local ThreeBands = Class {}
ThreeBands:include(MultiBand)

function ThreeBands:init(args)
  args.title = "Three Bands"
  args.bandCount = 3
  MultiBand.init(self, args)
end

return ThreeBands
