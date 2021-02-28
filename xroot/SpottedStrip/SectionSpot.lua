local Class = require "Base.Class"

local SectionSpot = Class{}

function SectionSpot:init(handle,descriptor,control,positionOnControl)
  self.handle = handle
  self.descriptor = descriptor
  self.control = control
  self.positionOnControl = positionOnControl
end

function SectionSpot:getControl()
  return self.control
end

function SectionSpot:getDescriptor()
  return self.descriptor
end

function SectionSpot:getValue(key)
  return self.descriptor[key]
end

function SectionSpot:getPositionOnControl()
  return self.positionOnControl
end

function SectionSpot:getHandle()
  return self.handle
end

function SectionSpot:getPositionOnSection()
  return self.handle
end

return SectionSpot
