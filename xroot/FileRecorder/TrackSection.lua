local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Section = require "SpottedStrip.Section"
local ChannelControl = require "FileRecorder.ChannelControl"

local TrackSection = Class {}
TrackSection:include(Section)

function TrackSection:init(i, stereo, canLink)
  Section.init(self, app.sectionMiddle)
  self:setClassName("FileRecorder.TrackSection")
  self.pSection:setVerticalDividerColor(app.GRAY3)
  local control
  self.stereo = stereo
  self.index = i

  self:addView("setup")
  control = ChannelControl(self, 1, canLink)
  self:addControl("setup", control)
  if stereo then
    control = ChannelControl(self, 2)
    self:addControl("setup", control)
  end
end

function TrackSection:setInputSource(channelIndex, source)
  local view = self:getView("setup")
  local channel = view.controls[channelIndex or 1]
  if channel then
    channel:setInputSource(source)
  end
end

function TrackSection:getInputSource(channelIndex)
  local view = self:getView("setup")
  local channel = view.controls[channelIndex or 1]
  if channel then
    return channel:getInputSource()
  end
end

return TrackSection
