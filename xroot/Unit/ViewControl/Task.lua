local app = app
local Env = require "Env"
local Utils = require "Utils"
local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"
local ply = app.SECTION_PLY

-- Task Class
local Task = Class {}
Task:include(ViewControl)

function Task:init(args)
  ViewControl.init(self)
  self:setClassName("Unit.ViewControl.Task")
  local description = args.description or
                          app.logError("%s.init: description is missing.", self)
  self:setInstanceName(description)
  local task = args.task or app.logError("%s.init: task is missing.", self)

  local graphic = app.TextPanel(description, 1)
  graphic:setLeftBorder(args.leftBorder or 1)
  graphic:setRightBorder(args.rightBorder or 0)
  graphic:setJustification(args.justification or app.justifyCenter)
  self:setControlGraphic(graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }
  self.task = task
end

function Task:spotReleased(spot, shifted)
  local controls = self:queryUp("controls")
  self:callUp("switchView", "expanded", controls.header, 1)
  self:callUp(self.task)
  return true
end

return Task
