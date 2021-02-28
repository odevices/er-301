local Class = require "Base.Class"
local Base = require "SpottedStrip.Control"
local ply = app.SECTION_PLY

local FooterControl = Class {}
FooterControl:include(Base)

function FooterControl:init()
  Base.init(self)
  self:setClassName("PinView.FooterControl")
  local graphic = app.TextPanel("+", 1)
  graphic:setTextSize(16)
  self:setMainCursorController(graphic)
  self:setControlGraphic(graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }
end

function FooterControl:setText(text)
  self.controlGraphic:setText(text)
end

function FooterControl:spotReleased(spot, shifted)
  if shifted then return false end
  self:callUp("doInsertPinSet")
  return true
end

return FooterControl
