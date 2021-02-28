local Class = require "Base.Class"
local ControlBranch = require "Unit.ControlBranch"
local Gate = require "Unit.ViewControl.Gate"

local GateBranch = Class {
  classType = "Gate",
  classDescription = "Gate",
  classPrefix = "gate"
}
GateBranch:include(ControlBranch)

function GateBranch:init(args)
  local id = args.id or app.logError("%s:init: id is missing.", self)
  local object = app.Comparator()
  object:setName(id)
  args.leftDestination = object:getInput("In")
  args.leftOutObject = object
  args.leftOutletName = "Out"
  args.channelCount = 1
  args.objects = {
    object
  }
  ControlBranch.init(self, args)
  self:setClassName("Unit.ControlBranch.Gate")

  self.control = Gate {
    button = id,
    description = args.description or GateBranch.classDescription,
    branch = self,
    comparator = object
  }
end

return GateBranch
