local Class = require "Base.Class"
local Base = require "SpottedStrip.Section"
local FooterControl = require "PinView.FooterControl"

local Footer = Class {}
Footer:include(Base)

function Footer:init()
  Base.init(self, app.sectionNoBorder, false)
  self:setClassName("PinView.Footer")
  self:addView("default")
  self:addControl("default", FooterControl())
  self:switchView("default")
end

return Footer
