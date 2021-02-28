local Class = require "Base.Class"
local Section = require "SpottedStrip.Section"
local MonitorControl = require "Chain.MonitorControl"

local FooterSection = Class {}
FooterSection:include(Section)

function FooterSection:init(channelCount)
  Section.init(self, app.sectionEnd)
  self:setClassName("FooterSection")
  self:addView("default")
  local control = MonitorControl(channelCount)
  self.monitorControl = control
  self:addControl("default", control)
  self:switchView("default")
end

function FooterSection:contentChanged(chain)
  self.monitorControl:contentChanged(chain)
end

return FooterSection
