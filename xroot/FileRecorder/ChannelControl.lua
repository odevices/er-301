local app = app
local Class = require "Base.Class"
local SpottedControl = require "SpottedStrip.Control"
local SourceChooser = require "Source.Chooser"
local ply = app.SECTION_PLY

local ChannelControl = Class {}
ChannelControl:include(SpottedControl)

function ChannelControl:init(track, index, canLink)
  SpottedControl.init(self)
  self:setClassName("FileRecorder.ChannelControl")
  self:setInstanceName(track.index .. "-" .. index)
  self.track = track
  self.index = index

  -- control graphic
  local graphic
  if not track.stereo and (track.index == 1 or track.index == 5) then
    graphic = app.Graphic(0, 0, ply - 1, 64)
  else
    graphic = app.Graphic(0, 0, ply, 64)
  end
  self:setControlGraphic(graphic)

  local scope = app.MiniScope(0, 14, ply, 50)
  self.scope = scope
  self.scope:hide()
  graphic:addChild(self.scope)
  if track.stereo and index == 1 then
    self.meter = app.VerticalMeter(ply - 10, 12, 4, 48)
    self.meter:displayTextOnLeft()
  else
    self.meter = app.VerticalMeter(6, 12, 4, 48)
    self.meter:displayTextOnRight()
  end
  self.meter:enableDisplayText()
  self.meter:hide()
  graphic:addChild(self.meter)

  self.button = app.MainButton(self:getDisplayName(), 1)
  self.button:setTopBorder(1)
  graphic:addChild(self.button)

  if track.stereo and index > 1 then
    self:addSpotDescriptor{
      center = 0,
      action = "unlink"
    }
    self.verticalDivider = 0
  elseif canLink then
    self:addSpotDescriptor{
      center = 0,
      action = "link"
    }
  end
  self:addSpotDescriptor{
    center = 0.5 * ply,
    action = "assign"
  }
end

function ChannelControl:getDisplayName()
  if self.source then
    return string.format("%d: %s", self.track.index,
                         self.source:getDisplayName())
  else
    return string.format("%d: ----", self.track.index)
  end
end

function ChannelControl:spotReleased(i, shifted)
  if shifted then return end
  if self:queryUp("state") == "setup" then
    local chooser = SourceChooser()
    local window = self:getWindow()
    if window then
      local task = function(src)
        window:setInputSource(self.track.index, self.index, src)
      end
      chooser:subscribe("choose", task)
      chooser:show()
    end
  elseif self.source then
    if self.meter:isHidden() then
      self.meter:show()
      self.scope:hide()
    else
      self.meter:hide()
      self.scope:show()
    end
  end
end

function ChannelControl:setInputSource(src)
  if src then
    local outlet = src:getOutlet()
    self.scope:watchOutlet(outlet)
    self.meter:watchOutlet(outlet)
    self.source = src
    self.meter:show()
  else
    self.scope:watchOutlet(nil)
    self.meter:watchOutlet(nil)
    self.meter:hide()
    self.source = nil
  end
  self.button:setText(self:getDisplayName())
end

function ChannelControl:getInputSource()
  return self.source
end

return ChannelControl
