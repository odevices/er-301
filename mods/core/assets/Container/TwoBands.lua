local Class = require "Base.Class"
local MultiBand = require "core.Container.MultiBand"

local TwoBands = Class {}
TwoBands:include(MultiBand)

function TwoBands:init(args)
  args.title = "Two Bands"
  args.bandCount = 2
  MultiBand.init(self, args)
end

return TwoBands
