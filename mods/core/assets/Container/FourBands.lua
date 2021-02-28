local Class = require "Base.Class"
local MultiBand = require "core.Container.MultiBand"

local FourBands = Class {}
FourBands:include(MultiBand)

function FourBands:init(args)
  args.title = "Four Bands"
  args.bandCount = 4
  MultiBand.init(self, args)
end

return FourBands
