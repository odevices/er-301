local app = app
local Class = require "Base.Class"
local MenuControl = require "Package.Menu.Control"

-- Task Class
local Task = Class {}
Task:include(MenuControl)

function Task:init(args)
  MenuControl.init(self)
  self:setClassName("Package.Menu.Task")
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  self:setInstanceName(description)
  self.task = args.task or app.logError("%s.init: task is missing.", self)
  local graphic = app.FittedTextBox(description)
  self:setControlGraphic(graphic)
end

function Task:onReleased(i, shifted)
  self:callUp("hide")
  self.task()
  return true
end

return Task
