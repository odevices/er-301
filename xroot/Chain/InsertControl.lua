local app = app
local Env = require "Env"
local Class = require "Base.Class"
local SpottedControl = require "SpottedStrip.Control"
local UnitChooser = require "Unit.Chooser"
local Clipboard = require "Chain.Clipboard"
local ply = app.SECTION_PLY

-- InsertControl Class
local InsertControl = Class {}
InsertControl:include(SpottedControl)

function InsertControl:init()
  SpottedControl.init(self)
  self:setClassName("Chain.InsertControl")

  local graphic = app.Graphic(0, 0, 128, 64)
  self.scope = app.MiniScope(0, 0, 128, 64)
  graphic:addChild(self.scope)
  local label = app.SubButton("insert", 3, true)
  graphic:addChild(label)
  label = app.SubButton("paste", 1, true)
  graphic:addChild(label)
  label:hide()
  self.pasteButton = label
  label = app.Label("No signal.", 10)
  label:setJustification(app.justifyCenter)
  label:setCenter(64, app.GRID5_LINE2)
  label:hide()
  graphic:addChild(label)
  self.noInputMsg = label
  self.menuGraphic = graphic
  self:addSpotDescriptor{
    center = -1,
    radius = ply
  }
end

function InsertControl:activateChooser()
  local chooser = UnitChooser {
    goal = "insert",
    chain = self:getWindow()
  }
  chooser:show()
end

function InsertControl:enterReleased()
  self:activateChooser()
  return true
end

function InsertControl:spotReleased(spot, shifted)
  return true
end

function InsertControl:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
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
function InsertControl:selectReleased(i, shifted)
  local Channels = require "Channels"
  local side = Channels.getSide(i)
  local outlet = self:callUp("getInwardConnection", side)
  self.scope:watchOutlet(outlet)
  return true
end

function InsertControl:onCursorEnter()
  local Channels = require "Channels"
  local side = Channels.getSide()
  local outlet = self:callUp("getInwardConnection", side)
  self.scope:watchOutlet(outlet)
  if outlet then
    self.scope:show()
    self.noInputMsg:hide()
  else
    self.scope:hide()
    self.noInputMsg:show()
  end
  if Clipboard.hasData(1) then
    self.pasteButton:show()
  else
    self.pasteButton:hide()
  end
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased", "enterReleased", "selectReleased")
end

function InsertControl:onCursorLeave()
  self.scope:watchOutlet(nil)
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased", "enterReleased", "selectReleased")
end

return InsertControl
