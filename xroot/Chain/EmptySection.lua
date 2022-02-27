local Class = require "Base.Class"
local Section = require "SpottedStrip.Section"
local SpottedControl = require "SpottedStrip.Control"
local UnitChooser = require "Unit.Chooser"
local Clipboard = require "Chain.Clipboard"
local ply = app.SECTION_PLY

----------------------------------------------------
local EmptyControl = Class {}
EmptyControl:include(SpottedControl)

function EmptyControl:init(msg, width)
  SpottedControl.init(self)
  self:setClassName("EmptySection.EmptyControl")

  local graphic, label
  graphic = app.Graphic(0, 0, width, 64)
  label = app.Label(msg, 10)
  label:setCenter(width * 0.5, 32)
  graphic:addChild(label)
  self:setControlGraphic(graphic)

  graphic = app.Graphic(0, 0, 128, 64)
  self.scope = app.MiniScope(0, 0, 128, 64)
  graphic:addChild(self.scope)
  label = app.SubButton("insert", 3, true)
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

function EmptyControl:activateChooser()
  local chooser = UnitChooser {
    goal = "insert",
    chain = self:getWindow()
  }
  chooser:show()
end

function EmptyControl:enterReleased()
  self:activateChooser()
  return true
end

function EmptyControl:spotReleased(spot, shifted)
  self:activateChooser()
  return true
end

function EmptyControl:subReleased(i, shifted)
  if shifted then
    return false
  end
  if i == 1 then
    if Clipboard.hasData(1) then
      local chain = self:getWindow()
      if chain then
        Clipboard.paste(chain, nil, 1)
      end
    end
  elseif i == 3 then
    self:activateChooser()
  end
  return true
end

-- called by Signal module
function EmptyControl:selectReleased(i, shifted)
  local Channels = require "Channels"
  local side = Channels.getSide(i)
  local outlet = self:callUp("getOutput", side)
  self.scope:watchOutlet(outlet)
  return true
end

function EmptyControl:onCursorEnter()
  local Channels = require "Channels"
  local side = Channels.getSide()
  local outlet = self:callUp("getOutput", side)
  self.scope:watchOutlet(outlet)
  if Clipboard.hasData(1) then
    self.pasteButton:show()
  else
    self.pasteButton:hide()
  end
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased", "enterReleased", "selectReleased")
end

function EmptyControl:onCursorLeave()
  self.scope:watchOutlet(nil)
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased", "enterReleased", "selectReleased")
end

-----------------------------------------------
local EmptySection = Class {}
EmptySection:include(Section)

function EmptySection:init(width)
  Section.init(self, app.sectionNoBorder)
  local control
  self:setClassName("EmptySection")
  self:addView("expanded")
  control = EmptyControl("Insert a unit here.", width)
  self:addControl("expanded", control)
  self:switchView("expanded")
end

return EmptySection
