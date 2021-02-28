local app = app
local Env = require "Env"
local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

local line1 = app.GRID4_LINE1
local line2 = app.GRID4_LINE2
local line3 = app.GRID4_LINE3
local line4 = app.GRID4_LINE4
local col1 = app.BUTTON1_CENTER
local col2 = app.BUTTON2_CENTER
local col3 = app.BUTTON3_CENTER

-- OutputScope Class
local OutputScope = Class {}
OutputScope:include(ViewControl)

function OutputScope:init(args)
  ViewControl.init(self)
  self:setClassName("Unit.ViewControl.OutputScope")
  local width = args.width or ply
  local monitor = args.monitor or
                      app.logError("%s.init: monitor is missing.", self)

  local graphic = app.Graphic(0, 0, width, 64)
  self:setMainCursorController(graphic)
  self:setControlGraphic(graphic)
  self.subGraphic = app.Graphic(0, 0, 128, 64)

  -- add spots
  for i = 1, (width // ply) do
    self:addSpotDescriptor{
      center = (i - 0.5) * ply
    }
  end
  self.verticalDivider = width

  if monitor then
    if monitor.channelCount == 1 then
      local outlet = monitor:getMonitoringOutput(1)
      self.scope = app.MiniScope(0, 0, width, 64)
      graphic:addChild(self.scope)
      self.scope:watchOutlet(outlet)
    else
      local left = monitor:getMonitoringOutput(1)
      local right = monitor:getMonitoringOutput(2)
      local w1 = width // 2
      local w2 = width - w1
      self.leftScope = app.MiniScope(0, 0, w1, 64)
      graphic:addChild(self.leftScope)
      self.leftScope:watchOutlet(left)
      self.rightScope = app.MiniScope(w1, 0, w2, 64)
      graphic:addChild(self.rightScope)
      self.rightScope:watchOutlet(right)
      self.verticalDivider = w1
      local label = app.Label("L")
      label:setJustification(app.justifyLeft)
      label:setForegroundColor(app.GRAY7)
      label:setPosition(2, 51)
      graphic:addChild(label)
      label = app.Label("R")
      label:setJustification(app.justifyLeft)
      label:setForegroundColor(app.GRAY7)
      label:setPosition(w1 + 2, 51)
      graphic:addChild(label)
    end
  end
end

function OutputScope:spotReleased(spot, shifted)
  return true
end

return OutputScope
