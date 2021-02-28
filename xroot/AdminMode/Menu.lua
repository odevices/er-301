local app = app
local Env = require "Env"
local Class = require "Base.Class"
local MondrianMenu = require "MondrianMenu.AsWindow"
local MondrianControl = require "MondrianMenu.Control"
local ply = app.SECTION_PLY

local Control = Class {}
Control:include(MondrianControl)

function Control:init(title, destination)
  MondrianControl.init(self)
  self:setClassName("Menu.Control")
  local graphic = app.FittedTextBox(title)
  self:setControlGraphic(graphic)
  self.destination = destination
end

function Control:onReleased()
  self.destination:show()
end

--------------------------------------------------
local Menu = Class {}
Menu:include(MondrianMenu)

function Menu:init(name)
  MondrianMenu.init(self)
  self:setClassName("Menu")
  self:setInstanceName(name)
end

function Menu:add(title, destination)
  self:addControl(Control(title, destination))
end

function Menu:header(title)
  self:addHeaderText(title)
end

return Menu
