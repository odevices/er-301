local app = app
local Class = require "Base.Class"
local Section = require "SpottedStrip.Section"
local ExpandedHeader = require "Source.ExternalChooser.ExpandedHeader"
local CollapsedHeader = require "Source.ExternalChooser.CollapsedHeader"
local Control = require "Source.ExternalChooser.Control"

local Group = Class {}
Group:include(Section)

function Group:init(title, sources)
  Section.init(self, app.sectionNoBorder)
  self:setClassName("Source.ExternalChooser.Group")

  self:addView("expanded")
  self:addControl("expanded", ExpandedHeader(title))
  for _, source in ipairs(sources) do
    self:addControl("expanded", Control(source, source:getDisplayName()))
  end

  self:addView("collapsed")
  self:addControl("collapsed", CollapsedHeader(title, sources))

  self:switchView("collapsed")
end

function Group:collapse()
  if self.currentViewName ~= "collapsed" then
    self:switchView("collapsed")
    self:setStyle(app.sectionNoBorder)
  end
end

function Group:expand()
  if self.currentViewName ~= "expanded" then
    self:switchView("expanded")
    self:setStyle(app.sectionMiddle)
  end
end

return Group
