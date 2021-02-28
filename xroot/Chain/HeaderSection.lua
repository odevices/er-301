local Class = require "Base.Class"
local Section = require "SpottedStrip.Section"
local ChainTitleControl = require "Chain.ChainTitleControl"
local InputControl = require "Chain.InputControl"

local HeaderSection = Class {}
HeaderSection:include(Section)

function HeaderSection:init(title, subTitle, channelCount)
  Section.init(self, app.sectionBegin)
  self:setClassName("HeaderSection")
  self:addView("default")
  local control = ChainTitleControl(title, subTitle)
  self:addControl("default", control)
  -- setup the input channels and their UI
  for i = 1, channelCount do
    control = InputControl(i)
    self:addControl("default", control)
  end
  self:switchView("default")
end

function HeaderSection:contentChanged(chain)
  self:notifyControls("contentChanged", chain)
end

return HeaderSection
