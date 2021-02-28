local app = app
local Env = require "Env"
local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"
local Encoder = require "Encoder"
local Utils = require "Utils"
local Path = require "Path"
local ply = app.SECTION_PLY

local FileTransport = Class {
  type = "File",
  canEdit = false,
  canMove = false
}
FileTransport:include(ViewControl)

function FileTransport:init(args)
  ViewControl.init(self)
  self:setClassName("Unit.ViewControl.FileTransport")
  local source = args.source or
                     app.logError("%s: no source object provided.", self)
  local width = args.width or (2 * ply)
  local monitor = args.monitor

  local graphic
  graphic = app.Graphic(0, 0, width, 64)
  self.display = app.FileSourceDisplay(0, 0, width, 64)
  self.display:setFileSource(source)
  graphic:addChild(self.display)
  self:setMainCursorController(self.display)
  self:setControlGraphic(graphic)

  self.subGraphic = app.Graphic(0, 0, 128, 64)
  if monitor then
    if monitor.channelCount == 1 then
      local outlet = monitor:getMonitoringOutput(1)
      self:setMonoScopeTarget(outlet)
    else
      local left = monitor:getMonitoringOutput(1)
      local right = monitor:getMonitoringOutput(2)
      self:setStereoScopeTarget(left, right)
    end
  end

  self.rewButton = app.SubButton("rew", 1)
  self.pauseButton = app.SubButton("pause", 2)
  self.openButton = app.SubButton("open", 3)
  self.subGraphic:addChild(self.pauseButton)
  self.subGraphic:addChild(self.openButton)
  self.subGraphic:addChild(self.rewButton)

  -- add spots
  for i = 1, (width // ply) do
    self:addSpotDescriptor{
      center = (i - 0.5) * ply
    }
  end
  self.verticalDivider = width
end

function FileTransport:getScope()
  if self.scope == nil then
    self.scope = app.MiniScope(0, 0, 128, 64)
    self.subGraphic:addChild(self.scope)
  end
  return self.scope
end

function FileTransport:setMonoScopeTarget(outlet)
  self.leftScopeTarget = outlet
  self.rightScopeTarget = nil
end

function FileTransport:setStereoScopeTarget(leftOutlet, rightOutlet)
  self.leftScopeTarget = leftOutlet
  self.rightScopeTarget = rightOutlet
end

function FileTransport:onFileChanged(fullpath)
  if fullpath then
    local filename = Path.lastFolderAndFilename(fullpath)
    self.display:setDisplayName(Utils.shorten(filename, 16, ".."))
    self.openButton:setText("close")
  else
    self.display:setDisplayName("No file.")
    self.openButton:setText("open")
  end
end

function FileTransport:onPause()
  self.pauseButton:setText("unpause")
end

function FileTransport:onUnpause()
  self.pauseButton:setText("pause")
end

function FileTransport:onCursorEnter(spot)
  ViewControl.onCursorEnter(self, spot)
  local left = self.leftScopeTarget
  local right = self.rightScopeTarget
  local scope = self:getScope()
  local Channels = require "Channels"
  if Channels.isRight() then
    scope:watchOutlet(right)
  else
    scope:watchOutlet(left)
  end
end

function FileTransport:subReleased(i, shifted)
  if i == 1 then
    self:callUp("rewind")
  elseif i == 2 then
    if self:callUp("isPaused") then
      self:callUp("unpause")
    else
      self:callUp("pause")
    end
  elseif i == 3 then
    if self.openButton:getText() == "open" then
      self:callUp("doLoadFile")
    else
      self:callUp("closeFile")
    end
  end
end

return FileTransport
