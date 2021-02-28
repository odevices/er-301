local Class = require "Base.Class"
local ControlBranch = require "Unit.ControlBranch"
local Pitch = require "Unit.ViewControl.Pitch"

local PitchBranch = Class {
  classType = "Pitch",
  classDescription = "Pitch",
  classPrefix = "tune"
}
PitchBranch:include(ControlBranch)

function PitchBranch:init(args)
  local id = args.id or app.logError("%s:init: id is missing.", self)
  local tune = app.ConstantOffset()
  tune:setName(id)
  local range = app.MinMax()
  range:setName(id)
  connect(tune, "Out", range, "In")
  args.leftDestination = tune:getInput("In")
  args.leftOutObject = tune
  args.leftOutletName = "Out"
  args.channelCount = 1
  args.objects = {
    tune,
    range
  }
  ControlBranch.init(self, args)
  self:setClassName("Unit.ControlBranch.Pitch")

  self.control = Pitch {
    button = id,
    description = args.description or PitchBranch.classDescription,
    branch = self,
    offset = tune,
    range = range
  }
end

return PitchBranch
