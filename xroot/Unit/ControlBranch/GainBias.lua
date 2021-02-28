local Class = require "Base.Class"
local ControlBranch = require "Unit.ControlBranch"
local GainBias = require "Unit.ViewControl.GainBias"

local GainBiasBranch = Class {
  classType = "GainBias",
  classDescription = "Gain/Bias",
  classPrefix = "gb"
}
GainBiasBranch:include(ControlBranch)

function GainBiasBranch:init(args)
  local id = args.id or app.logError("%s:init: id is missing.", self)
  local object = app.ParameterAdapter()
  args.leftDestination = object:getInput("In")
  args.leftOutObject = object
  args.leftOutletName = "Out"
  args.channelCount = 1
  args.objects = {
    object
  }
  ControlBranch.init(self, args)
  self:setClassName("Unit.ControlBranch.GainBias")

  self.control = GainBias {
    button = id,
    description = args.description or GainBiasBranch.classDescription,
    branch = self,
    gainbias = object,
    range = object
  }

end

return GainBiasBranch
