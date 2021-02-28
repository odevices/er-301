local Class = require "Base.Class"
local Base = require "builtins.MixerUnit"

local MonoToStereo = Class {}
MonoToStereo:include(Base)

function MonoToStereo:init(args)
  args.forceMono = true
  Base.init(self, args)
end

return MonoToStereo
