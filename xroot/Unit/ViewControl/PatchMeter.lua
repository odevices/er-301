local app = app
local Env = require "Env"
local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"

local PatchMeter = Class {}
PatchMeter:include(ViewControl)

function PatchMeter:init(args)
  ViewControl.init(self)
  self:setClassName("Unit.ViewControl.PatchMeter")
  local button = args.button or app.logError("%s.init: button is missing.", self)
  self:setInstanceName(button)
  local patch = args.patch or app.logError("%s.init: patch is missing.", self)

  self.patch = patch
  local graphic
  local ply = app.SECTION_PLY

  if patch.channelCount == 1 then
    graphic = app.Graphic(0, 0, ply, 64)
    self.scope = app.MiniScope(0, 0, ply, 64)
    self.scope:setForegroundColor(app.GRAY5)
    self.scope:watchOutlet(patch:getMonitoringOutput(1))
    graphic:addChild(self.scope)
    local panel = app.TextPanel(button, 1, 0.6)
    graphic:addChild(panel)
    self.verticalDivider = ply
    self:addSpotDescriptor{
      center = 0.5 * ply,
      radius = ply
    }
  elseif patch.channelCount > 1 then
    graphic = app.Graphic(0, 0, 2 * ply, 64)
    self.scope1 = app.MiniScope(0, 0, ply, 64)
    self.scope1:setForegroundColor(app.GRAY5)
    self.scope1:watchOutlet(patch:getMonitoringOutput(1))
    graphic:addChild(self.scope1)
    self.scope2 = app.MiniScope(ply, 0, ply, 64)
    self.scope2:setForegroundColor(app.GRAY5)
    self.scope2:watchOutlet(patch:getMonitoringOutput(2))
    graphic:addChild(self.scope2)
    local panel = app.TextPanel(button, 1, 0.6)
    panel:setPosition(ply // 2, 0)
    graphic:addChild(panel)
    self.verticalDivider = 2 * ply
    self:addSpotDescriptor{
      center = ply,
      radius = 2 * ply
    }
    graphic:addChild(app.MainButton("left", 1))
    graphic:addChild(app.MainButton("right", 2))
  end

  self:setMainCursorController(graphic)
  self:setControlGraphic(graphic)

  patch:subscribe("contentChanged", self)
end

function PatchMeter:getPatch()
  return self.patch
end

function PatchMeter:contentChanged(chain)
  if chain == self.patch then
    if self.scope then self.scope:watchOutlet(chain:getMonitoringOutput(1)) end
    if self.scope1 then self.scope1:watchOutlet(chain:getMonitoringOutput(1)) end
    if self.scope2 then self.scope2:watchOutlet(chain:getMonitoringOutput(2)) end
  end
end

function PatchMeter:spotReleased(spot, shifted)
  if shifted then return false end
  self.patch:show()
  return true
end

function PatchMeter:onRemove()
  self.patch:unsubscribe("contentChanged", self)
  ViewControl.onRemove(self)
end

return PatchMeter
