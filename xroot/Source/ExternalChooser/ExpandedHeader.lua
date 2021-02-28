local app = app
local Env = require "Env"
local Class = require "Base.Class"
local SpottedControl = require "SpottedStrip.Control"
local ply = app.SECTION_PLY

local ExpandedHeader = Class {}
ExpandedHeader:include(SpottedControl)

function ExpandedHeader:init(title)
  SpottedControl.init(self)
  self:setClassName("Source.ExternalChooser.ExpandedHeader")
  local graphic = app.TextPanel(title, 1)
  -- graphic:setLeftBorder(1)
  graphic:setRightBorder(1)
  graphic:setBorderColor(app.GRAY7)
  self:setControlGraphic(graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }
end

function ExpandedHeader:spotReleased()
  self:callUp("collapse")
  return true
end

function ExpandedHeader:enterReleased()
  self:callUp("collapse")
  return true
end

return ExpandedHeader
