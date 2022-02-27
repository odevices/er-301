local Class = require "Base.Class"
local Base = require "SpottedStrip.Control"

local Control = Class {}
Control:include(Base)

function Control:init(args)
  Base.init(self)
  self:setClassName("PinView.Control")
  local delegate = args.delegate or
                       app.logError("%s.init: delegate is missing.", self)
  self.delegate = delegate
end

function Control:isFirst()
  local view = self:callUp("getView")
  if view then
    local controls = view.controls
    return self == controls[2]
  end
end

function Control:isLast()
  local view = self:callUp("getView")
  if view then
    local controls = view.controls
    local n = #controls
    return self == controls[n]
  end
end

function Control:configureBorder()
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

function Control:focus()
  if not self.focused then
    self.focused = true
    self:grabFocus("encoder", "upReleased", "cancelReleased")
    self:configureBorder()
    self.controlGraphic:setBorder(1)
    self:onFocus()
  end
end

function Control:unfocus()
  if self.focused then
    self.focused = false
    self:releaseFocus("encoder", "upReleased", "cancelReleased")
    self.controlGraphic:setBorder(0)
    self:onDefocus()
  end
end

function Control:onCursorEnter(spot)
  -- app.logInfo("%s.onCursorEnter(%s)",self,spot)
  self:addSubGraphic(self.subGraphic)
  self:grabFocus("dialPressed", "subPressed", "subReleased", "homeReleased",
                 "homePressed", "commitReleased", "enterReleased",
                 "selectReleased")
end

function Control:onCursorLeave(spot)
  -- app.logInfo("%s.onCursorLeave(%s)",self,spot)
  self:removeSubGraphic(self.subGraphic)
  self:releaseFocus("dialPressed", "subPressed", "subReleased", "homeReleased",
                    "homePressed", "commitReleased", "enterReleased",
                    "cancelReleased", "upReleased", "selectReleased", "encoder")
  self:unfocus()
end

function Control:spotReleased(spot, shifted)
  if shifted then
    return false
  end
  if self.focused then
    self:unfocus()
  else
    self:focus()
  end
  return true
end

function Control:upReleased(shifted)
  if not shifted then
    self:unfocus()
  end
  return true
end

function Control:doRename()
  local Keyboard = require "Keyboard"
  local suggested = self:getInstanceName()
  local kb = Keyboard("Rename Pin", suggested, true, "ControlNames")
  local task = function(text)
    if text then
      self:callUp("notifyPinSets", "renamePin", self.delegate, text)
    end
  end
  kb:subscribe("done", task)
  kb:show()
end

function Control:rename(name)
end

function Control:onRemove()
  self:removeSubGraphic(self.subGraphic)
  Base.onRemove(self)
end

return Control
