local app = app
local Class = require "Base.Class"
local MondrianMenu = require "MondrianMenu.AsWindow"

local Menu = Class {}
Menu:include(MondrianMenu)

function Menu:init(unit, sub)
  MondrianMenu.init(self)
  self:setClassName("Unit.Menu")
  self:setInstanceName(unit.title)

  if sub then
    for i, line in ipairs(sub) do
      local label = app.Label(line.text, line.fontSize or 10)
      label:setJustification(line.justify or app.justifyLeft)
      label:fitToText(line.margin or 2)
      label:setSize(128, label.mHeight)
      label:setPosition(0, line.position or (64 - i * 10))
      self:addSubGraphic(label)
    end
  end

  self.unit = unit
end

return Menu
