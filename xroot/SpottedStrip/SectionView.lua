local app = app
local Class = require "Base.Class"
local SectionSpot = require "SpottedStrip.SectionSpot"
local ply = app.SECTION_PLY

local SectionView = Class {}

function SectionView:init(name)
  self.name = name
  self.controls = {}
  self.spots = {}
  self.dividers = {}
end

function SectionView:getSpotByHandle(handle)
  return self.spots[handle]
end

function SectionView:addDivider(x)
  self.dividers[#self.dividers + 1] = x
end

function SectionView:rebuild(pSection)
  pSection:clear()
  self.spots = {}

  for _, x in ipairs(self.dividers) do
    pSection:addVerticalDivider(x)
  end

  for _, control in ipairs(self.controls) do
    -- add the control's graphic to the end of the view
    local graphic = control:getControlGraphic()
    local left = pSection.mWidth
    if graphic then
      graphic:setPosition(left, 0)
      pSection:setWidth(left + graphic.mWidth)
      pSection:addChild(graphic)
    end

    if control.verticalDivider then
      pSection:addVerticalDivider(left + control.verticalDivider)
    end

    -- register the control's spots
    -- spot := {name,center,width}
    for i, desc in ipairs(control.spotDescriptors) do
      local handle = pSection:addSpot(left + desc.center, desc.width or ply)
      self.spots[handle] = SectionSpot(handle, desc, control, i)
      desc.handle = handle
    end
  end
end

function SectionView:addControl(control)
  self.controls[#self.controls + 1] = control
end

function SectionView:removeControl(control)
  for i, control2 in ipairs(self.controls) do
    if control2 == control then
      table.remove(self.controls, i)
      return
    end
  end
end

function SectionView:getControlCount()
  return #self.controls
end

function SectionView:contains(control)
  for _, control2 in ipairs(self.controls) do
    if control == control2 then
      return true
    end
  end
  return false
end

function SectionView:where(control)
  for i, control2 in ipairs(self.controls) do
    if control == control2 then
      return i
    end
  end
end

function SectionView:clear()
  self.controls = {}
  self.dividers = {}
end

function SectionView:notifyControls(method, ...)
  for _, control in ipairs(self.controls) do
    local f = control[method]
    if f ~= nil then
      f(control, ...)
    end
  end
end

return SectionView
