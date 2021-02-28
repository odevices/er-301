local app = app
local Class = require "Base.Class"
local SpottedControl = require "SpottedStrip.Control"
local ply = app.SECTION_PLY

local CollapsedHeader = Class {}
CollapsedHeader:include(SpottedControl)

function CollapsedHeader:init(title, sources)
  SpottedControl.init(self)
  self:setClassName("Source.ExternalChooser.CollapsedHeader")
  local graphic = app.Graphic(0, 0, ply, 64)
  local label = app.Label(title, 10)
  label:fitToText(0)
  label:setPosition(5, 55)
  graphic:addChild(label)

  local n = #sources
  local width = 6
  local gap = 2
  local offset = ply // 2 - (n * (width + gap)) // 2

  for i, source in ipairs(sources) do
    local meter = app.VerticalMeter((i - 1) * (width + gap) + offset, 8, width,
                                   44)
    meter:watchOutlet(source:getOutlet())
    graphic:addChild(meter)
  end

  self:setControlGraphic(graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }
end

function CollapsedHeader:spotReleased()
  self:callUp("expand")
  self:callUp("leftJustify")
  return true
end

function CollapsedHeader:enterReleased()
  self:callUp("expand")
  self:callUp("leftJustify")
  return true
end

return CollapsedHeader
