local Class = require "Base.Class"
local Base = require "PinView.MasterControl"
local ply = app.SECTION_PLY

local EmptyControl = Class {}
EmptyControl:include(Base)

function EmptyControl:init(text)
  Base.init(self)
  self:setClassName("PinView.EmptyControl")
  local graphic = app.TextPanel(text, 1)
  self:setMainCursorController(graphic)
  self:setControlGraphic(graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }
end

function EmptyControl:setText(text)
  self.controlGraphic:setText(text)
end

return EmptyControl
