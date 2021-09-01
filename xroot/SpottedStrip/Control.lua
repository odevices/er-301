local Class = require "Base.Class"
local BaseControl = require "Base.Control"

local Control = Class {}
Control:include(BaseControl)

function Control:init()
  BaseControl.init(self)
  self:setClassName("SpottedStrip.Control")
  self.spotDescriptors = {}
  self.verticalDivider = false
end

function Control:addSpotDescriptor(args)
  if args.center == nil then
    app.logError("%s.addSpotDescriptor: no center provided.", self)
  end
  self.spotDescriptors[#self.spotDescriptors + 1] = args
  return #self.spotDescriptors
end

function Control:getSpotDescriptor(spotIndex)
  return self.spotDescriptors[spotIndex]
end

function Control:getSpotValue(spotIndex, descriptorKey)
  local descriptor = self.spotDescriptors[spotIndex]
  return descriptor and descriptor[descriptorKey]
end

function Control:getFloatingMenuItems()
end

function Control:onFloatingMenuEnter()
end

function Control:onFloatingMenuChange(choice, index)
end

function Control:onFloatingMenuSelection(choice)
end

function Control:onCursorLeave(spotIndex, shifted)
end

function Control:onCursorEnter(spotIndex, shifted)
end

function Control:onCursorMove(spotIndex, spotIndex0, shifted)
end

function Control:spotPressed(spotIndex, shifted, isFocusedPress)
end

function Control:spotReleased(spotIndex, shifted)
end

return Control
