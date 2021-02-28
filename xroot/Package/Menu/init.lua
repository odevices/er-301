local app = app
local Class = require "Base.Class"
local MondrianMenu = require "MondrianMenu.AsWindow"

local Menu = Class {}
Menu:include(MondrianMenu)

function Menu:init(title, description)
  MondrianMenu.init(self)
  self:setClassName("Package.Menu")
  self:setInstanceName(title)

  if description then
    local box = app.RichTextBox(0, 0, 256, 64)
    self.subGraphic:addChild(box)
    box:setText(description)
  end

end

return Menu
