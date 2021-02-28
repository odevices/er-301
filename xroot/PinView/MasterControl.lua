local Class = require "Base.Class"
local Base = require "SpottedStrip.Control"
local Encoder = require "Encoder"

local MasterControl = Class {}
MasterControl:include(Base)

function MasterControl:init()
  Base.init(self)
  self:setClassName("PinView.MasterControl")
  self.encoderState = Encoder.Coarse

  -- sub display
  self.subGraphic = app.Graphic(0, 0, 128, 64)
  local graphic = app.TextPanel("Rename", 1)
  self.subGraphic:addChild(graphic)
  graphic = app.TextPanel("Delete", 2)
  self.subGraphic:addChild(graphic)
  graphic = app.TextPanel("Reset Targets", 3)
  self.subGraphic:addChild(graphic)
end

function MasterControl:isFirst()
  local view = self:callUp("getView")
  local controls = view.controls
  return self == controls[2]
end

function MasterControl:isLast()
  local view = self:callUp("getView")
  local controls = view.controls
  local n = #controls
  return self == controls[n]
end

function MasterControl:configureBorder()
  local first = self:isFirst()
  local last = self:isLast()
  if first and last then
    self.controlGraphic:setCornerRadius(5, 5, 5, 5)
  elseif first then
    self.controlGraphic:setCornerRadius(5, 5, 0, 0)
  elseif last then
    self.controlGraphic:setCornerRadius(0, 0, 5, 5)
  else
    self.controlGraphic:setCornerRadius(0, 0, 0, 0)
  end
end

function MasterControl:focus()
  if not self.focused then
    self.focused = true
    self:grabFocus("encoder", "upReleased", "cancelReleased")
    self:configureBorder()
    self.controlGraphic:setBorder(1)
    self:onFocus()
  end
end

function MasterControl:unfocus()
  if self.focused then
    self.focused = false
    self:releaseFocus("encoder", "upReleased", "cancelReleased")
    self.controlGraphic:setBorder(0)
    self:onDefocus()
  end
end

function MasterControl:onCursorEnter(spot)
  -- app.logInfo("%s.onCursorEnter(%s)",self,spot)
  self:addSubGraphic(self.subGraphic)
  self:grabFocus("dialPressed", "subPressed", "subReleased", "homeReleased",
                 "homePressed", "zeroPressed", "zeroReleased", "enterReleased",
                 "commitReleased", "selectReleased")
  Encoder.set(self.encoderState)
end

function MasterControl:onCursorLeave(spot)
  -- app.logInfo("%s.onCursorLeave(%s)",self,spot)
  self:removeSubGraphic(self.subGraphic)
  self:releaseFocus("dialPressed", "subPressed", "subReleased", "homeReleased",
                    "homePressed", "zeroPressed", "zeroReleased",
                    "enterReleased", "cancelReleased", "commitReleased",
                    "upReleased", "selectReleased", "encoder")
  self:unfocus()
end

function MasterControl:dialPressed()
  if self.encoderState == Encoder.Coarse then
    self.encoderState = Encoder.Fine
  else
    self.encoderState = Encoder.Coarse
  end
  Encoder.set(self.encoderState)
  return true
end

function MasterControl:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    self:callUp("doRename")
  elseif i == 2 then
    local name = self.parent:getInstanceName()
    self:callUp("removePinSetByName", name)
  elseif i == 3 then
    self:callUp("notifyPinControls", "resetTarget")
  end
  return true
end

function MasterControl:onRemove()
  self:removeSubGraphic(self.subGraphic)
  Base.onRemove(self)
end

return MasterControl
