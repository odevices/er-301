local app = app
local Class = require "Base.Class"
local Signal = require "Signal"
local SpottedControl = require "SpottedStrip.Control"
local ply = app.SECTION_PLY

-- MonitorControl Class
local MonitorControl = Class {}
MonitorControl:include(SpottedControl)

function MonitorControl:init(channelCount)
  SpottedControl.init(self)
  self:setClassName("MonitorControl")

  -- control graphic
  local graphic = app.Graphic(0, 0, ply, 64)
  self:setControlGraphic(graphic)

  self.meters = {}
  if channelCount == 1 then
    local meter = app.VerticalMeter(10, 8, 6, 48)
    graphic:addChild(meter)
    self.meters[1] = meter
  elseif channelCount == 2 then
    local meter = app.VerticalMeter(10, 8, 6, 48)
    graphic:addChild(meter)
    self.meters[1] = meter
    meter = app.VerticalMeter(18, 8, 6, 48)
    graphic:addChild(meter)
    self.meters[2] = meter
    Signal.weakRegister("selectReleased", self)
  end

  graphic = app.Graphic(0, 0, 128, 64)
  self.scope = app.MiniScope(0, 0, 128, 64)
  graphic:addChild(self.scope)
  local label = app.SubButton("insert", 3, true)
  graphic:addChild(label)
  label = app.SubButton("paste", 1, true)
  graphic:addChild(label)
  label:hide()
  self.pasteButton = label
  self.menuGraphic = graphic

  self:addSpotDescriptor{
    center = -1,
    radius = ply
  }
end

function MonitorControl:contentChanged(chain)
  for i, meter in ipairs(self.meters) do
    meter:watchOutlet(chain:getMonitoringOutput(i))
  end
end

function MonitorControl:activateChooser()
  local UnitChooser = require "Unit.Chooser"
  local chooser = UnitChooser {
    goal = "insert",
    chain = self:getWindow()
  }
  chooser:show()
end

function MonitorControl:enterReleased()
  self:activateChooser()
  return true
end

function MonitorControl:spotReleased(spot, shifted)
  return true
end

function MonitorControl:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    local Clipboard = require "Chain.Clipboard"
    if Clipboard.hasData(1) then
      local chain = self:getWindow()
      if chain then Clipboard.paste(self:getWindow(), nil, 1) end
    end
  elseif i == 3 then
    self:activateChooser()
  end
  return true
end

-- called by Signal module
function MonitorControl:selectReleased(i, shifted)
  local Channels = require "Channels"
  local side = Channels.getSide(i)
  local outlet = self:callUp("getMonitoringOutput", side)
  self.scope:watchOutlet(outlet)
  for _, meter in ipairs(self.meters) do meter:setForegroundColor(app.GRAY7) end
  self.meters[side]:setForegroundColor(app.WHITE)
  return true
end

function MonitorControl:onCursorEnter()
  local Channels = require "Channels"
  local side = Channels.getSide()
  local outlet = self:callUp("getMonitoringOutput", side)
  self.scope:watchOutlet(outlet)
  local Clipboard = require "Chain.Clipboard"
  if Clipboard.hasData(1) then
    self.pasteButton:show()
  else
    self.pasteButton:hide()
  end
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased", "enterReleased")
end

function MonitorControl:onCursorLeave()
  self.scope:watchOutlet(nil)
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased", "enterReleased")
end

return MonitorControl
