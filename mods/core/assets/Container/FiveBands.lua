local Class = require "Base.Class"
local MultiBand = require "core.Container.MultiBand"

local FiveBands = Class {}
FiveBands:include(MultiBand)

function FiveBands:init(args)
  args.title = "Five Bands"
  args.bandCount = 5
  MultiBand.init(self, args)
end

return FiveBands
