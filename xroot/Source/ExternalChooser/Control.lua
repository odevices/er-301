local app = app
local Class = require "Base.Class"
local SpottedControl = require "SpottedStrip.Control"
local ply = app.SECTION_PLY

local Control = Class {}
Control:include(SpottedControl)

function Control:init(source, label)
  SpottedControl.init(self)
  self:setClassName("Source.ExternalChooser.Control")
  self.source = source

  local scope = app.MiniScope(0, 0, ply, 64)
  scope:watchOutlet(source:getOutlet())
  if source.isUnipolar then scope:setOffset(-1) end
  scope:setLeftBorder(1)
  scope:setBorderColor(app.GRAY3)

  if label then scope:addChild(app.MainButton(label, 1)) end
  self:setControlGraphic(scope)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }
end

function Control:spotReleased()
  self:callUp("choose", self.source)
  return true
end

function Control:enterReleased()
  self:callUp("choose", self.source)
  return true
end

function Control:onCursorEnter(spot)
  SpottedControl.onCursorEnter(self, spot)
  if self.hasRange then self:callUp("showRangeButton") end
end

function Control:onCursorLeave(spot)
  SpottedControl.onCursorLeave(self, spot)
  self:callUp("hideRangeButton")
end

return Control
