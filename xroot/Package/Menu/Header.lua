local app = app
local Class = require "Base.Class"
local Control = require "Package.Menu.Control"

local function HeaderTextBox(text)
  local graphic = app.RichTextBox(text, 10)
  local ply = app.SECTION_PLY
  graphic:setJustification(app.justifyLeft)
  graphic:setBorder(0)
  graphic:fitHeight(6 * ply)
  return graphic
end

-- Header Class
local Header = Class {}
Header:include(Control)

function Header:init(args)
  Control.init(self)
  self:setClassName("Package.Menu.Header")
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  self:setInstanceName(description)
  local graphic = HeaderTextBox(description)
  self:setControlGraphic(graphic)
  self.isHeader = true
end

return Header
