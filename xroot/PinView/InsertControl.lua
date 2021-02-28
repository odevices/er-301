local app = app
local Class = require "Base.Class"
local SpottedControl = require "SpottedStrip.Control"
local ply = app.SECTION_PLY

-- InsertControl Class
local InsertControl = Class {}
InsertControl:include(SpottedControl)

function InsertControl:init()
  SpottedControl.init(self)
  self:setClassName("PinView.InsertControl")

  local graphic = app.Graphic(0, 0, 128, 64)
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

function InsertControl:paste()
end

function InsertControl:enterReleased()
  self:callUp("doInsertPinSet")
  return true
end

function InsertControl:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    self:paste()
  elseif i == 3 then
    self:callUp("doInsertPinSet")
  end
  return true
end

function InsertControl:onCursorEnter()
  if self:callUp("hasClipboardData") then
    self.pasteButton:show()
  else
    self.pasteButton:hide()
  end
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased", "enterReleased", "selectReleased")
end

function InsertControl:onCursorLeave()
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased", "enterReleased", "selectReleased")
end

return InsertControl
