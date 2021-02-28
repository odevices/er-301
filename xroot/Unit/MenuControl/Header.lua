local app = app
local Class = require "Base.Class"
local MenuControl = require "Unit.MenuControl"

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
Header:include(MenuControl)

function Header:init(args)
  MenuControl.init(self)
  self:setClassName("UnitMenu.Header")
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  self:setInstanceName(description)
  local graphic = HeaderTextBox(description)
  self:setControlGraphic(graphic)
  self.isHeader = true
end

return Header
