local Class = require "Base.Class"
local MultiBand = require "core.Container.MultiBand"

local SixBands = Class {}
SixBands:include(MultiBand)

function SixBands:init(args)
  args.title = "Six Bands"
  args.bandCount = 6
  MultiBand.init(self, args)
end

return SixBands
