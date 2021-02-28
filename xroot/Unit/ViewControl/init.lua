local Class = require "Base.Class"
local Base = require "SpottedStrip.Control"
local Encoder = require "Encoder"

local ViewControl = Class {}
ViewControl:include(Base)

function ViewControl:init(name)
  Base.init(self)
  self:setClassName("Unit.ViewControl")
  self:setInstanceName(name)
  self.encoderState = Encoder.Neutral
  self.pinCount = 0
end

function ViewControl:getRootChain()
  local parent = self.parent
  return parent and parent.getRootChain and parent:getRootChain()
end

function ViewControl:configureBorderShape()
  local view = self:callUp("getView")
  local controls = view.controls
  local n = #controls
  -- Usually, the first visible control is after the insert control.
  if n == 1 and self == controls[1] then
    -- added this case for the control editor
    self.controlGraphic:setCornerRadius(5, 5, 5, 5)
  elseif self == controls[2] then
    if self == controls[n] then
      -- the one and only visible control
      self.controlGraphic:setCornerRadius(5, 5, 5, 5)
    else
      -- the first visible control
      self.controlGraphic:setCornerRadius(5, 5, 0, 0)
    end
  elseif self == controls[n] then
    -- the last visible control
    self.controlGraphic:setCornerRadius(0, 0, 5, 5)
  else
    -- in the middle
    self.controlGraphic:setCornerRadius(0, 0, 0, 0)
  end
end

function ViewControl:getBranch()
  return self.branch
end

function ViewControl:serialize()
  return {}
end

function ViewControl:deserialize(t)
end

function ViewControl:onFocused()
end

function ViewControl:onUnfocused()
end

function ViewControl:rename(name)
  self:setInstanceName(name)
end

function ViewControl:getDisplayName()
  return self:getInstanceName()
end

function ViewControl:enableHighlight()
  self:configureBorderShape()
  self.controlGraphic:setBorder(1)
  if self:callUp("hasView", self.id) then
    if self.moreLeft then
      self.moreLeft:show()
    else
      local graphic = self:getControlGraphic()
      self.moreLeft = app.MoreThisWay(app.left)
      graphic:addChild(self.moreLeft)
    end
  end
  self.focused = true
end

function ViewControl:disableHighlight()
  self.controlGraphic:setBorder(0)
  Encoder.set(Encoder.Neutral)
  if self.moreLeft then self.moreLeft:hide() end
  self.focused = false
end

function ViewControl:focus(notify)
  if notify == nil then notify = true end
  self:grabFocus("encoder", "upReleased", "cancelReleased")
  self:enableHighlight()
  if notify then self:onFocused() end
end

function ViewControl:unfocus(notify)
  if notify == nil then notify = true end
  self:releaseFocus("encoder", "upReleased", "cancelReleased")
  self:disableHighlight()
  if notify then self:onUnfocused() end
end

function ViewControl:dialPressed(shifted)
  if shifted then return false end
  self.encoderState = Encoder.Neutral
  Encoder.set(self.encoderState)
  return true
end

function ViewControl:onCursorMove(spot, spot0)
  -- app.logInfo("%s.onCursorMove(%s,%s)",self,spot,spot0)
end

function ViewControl:onCursorEnter(spot)
  -- app.logInfo("%s.onCursorEnter(%s)",self,spot)
  self:addSubGraphic(self.subGraphic)
  self:grabFocus("dialPressed", "dialReleased", "subPressed", "subReleased",
                 "homeReleased", "homePressed", "zeroPressed", "enterReleased",
                 "selectReleased")
  Encoder.set(self.encoderState)
end

function ViewControl:onCursorLeave(spot)
  -- app.logInfo("%s.onCursorLeave(%s)",self,spot)
  self:removeSubGraphic(self.subGraphic)
  self:releaseFocus("dialPressed", "dialReleased", "subPressed", "subReleased",
                    "homeReleased", "homePressed", "zeroPressed",
                    "enterReleased", "cancelReleased", "upReleased",
                    "selectReleased", "encoder")
  self:disableHighlight()
end

function ViewControl:toggleContext()
  local hadFocus = self:hasFocus("encoder")
  local viewChanged = false
  -- call up context view
  if self:callUp("hasView", self.id) then
    -- a context view for this control exists
    if self:queryUp("currentViewName") == self.id then
      self:callUp("switchView", "expanded", self)
      viewChanged = true
    else
      self:callUp("leftJustify")
      self:callUp("switchView", self.id, self)
      viewChanged = true
    end
  elseif self:queryUp("currentViewName") ~= "expanded" then
    self:callUp("switchView", "expanded")
    viewChanged = true
  end
  -- Hack
  if viewChanged then
    -- switchView will fail call CursorEnter
    self:callUp("disableSelection")
    self:callUp("enableSelection")
  end
  if hadFocus and viewChanged then
    -- need to grab focus again, since switchView will lose it
    self:focus(false)
  end
end

function ViewControl:createPinMark()
  local Drawings = require "Drawings"
  local graphic = app.Drawing(0, 0, app.SECTION_PLY, 64)
  graphic:add(Drawings.Control.Pin)
  self.controlGraphic:addChildOnce(graphic)
  self.pinMark = graphic
end

function ViewControl:onPinned()
  if self.pinCount == 0 then
    if self.pinMark == nil then self:createPinMark() end
    self.pinMark:show()
  end
  self.pinCount = self.pinCount + 1
end

function ViewControl:onUnpinned()
  if self.pinCount == 1 then self.pinMark:hide() end
  self.pinCount = self.pinCount - 1
end

function ViewControl:spotReleased(spot, shifted)
  if shifted then return false end
  if self.focused then
    self:unfocus()
  else
    self:focus()
  end
  return true
end

function ViewControl:getFloatingMenuItems()
  local t = {}
  if self:callUp("hasView", self.id) then
    if self:queryUp("currentViewName") == self.id then
      t[1] = "collapse"
    else
      t[1] = "expand"
    end
  end
  if self.canEdit then t[#t + 1] = "edit" end
  if self.getPinControl then
    local chain = self:getRootChain()
    if chain and chain.isRoot then
      local pinSetNames = chain:getPinSetNames()
      local pinned, pinCount = chain:getPinSetMembership(self)
      if #pinSetNames > 1 and #pinSetNames > pinCount then
        t[#t + 1] = "pin to all"
      end
      if pinCount > 1 then t[#t + 1] = "unpin from all" end
      for _, name in ipairs(pinSetNames) do
        if pinned[name] then
          t[#t + 1] = "unpin from " .. name
        else
          t[#t + 1] = "pin to " .. name
        end
      end
      t[#t + 1] = "pin to <new>"
    end
  end
  return t
end

function ViewControl:onFloatingMenuSelection(choice)
  local Utils = require "Utils"
  if choice == "expand" or choice == "collapse" then
    self:toggleContext()
    return true
  elseif choice == "edit" then
    self:callUp("doEditControl", self)
    return true
  elseif choice == "pin to all" then
    local chain = self:getRootChain()
    if chain and chain.isRoot then chain:pinControlToAllPinSets(self) end
  elseif choice == "unpin from all" then
    local chain = self:getRootChain()
    if chain and chain.isRoot then chain:unpinControlFromAllPinSets(self) end
  elseif choice == "pin to <new>" then
    local chain = self:getRootChain()
    if chain and chain.isRoot then
      local Keyboard = require "Keyboard"
      local kb = Keyboard("New PinSet", chain:suggestPinSetName(), true,
                          "NamingStuff")
      local task = function(text)
        if text then
          local pinSet = chain:addPinSet{
            name = text
          }
          pinSet:startViewModifications()
          pinSet:pinControl(self)
          pinSet:endViewModifications()
        end
      end
      kb:subscribe("done", task)
      kb:show()
    end
    return true
  elseif Utils.startsWith(choice, "pin to ") then
    local name = choice:sub(8)
    local chain = self:getRootChain()
    if chain and chain.isRoot then
      local pinSet = chain:getPinSetByName(name)
      pinSet:startViewModifications()
      pinSet:pinControl(self)
      pinSet:endViewModifications()
    end
    return true
  elseif Utils.startsWith(choice, "unpin from ") then
    local name = choice:sub(12)
    local chain = self:getRootChain()
    if chain and chain.isRoot then
      local pinSet = chain:getPinSetByName(name)
      pinSet:startViewModifications()
      pinSet:unpinControl(self)
      pinSet:endViewModifications()
    end
    return true
  else
    return Base.onFloatingMenuSelection(self, choice)
  end
end

function ViewControl:enterReleased()
  self:toggleContext()
  return true
end

function ViewControl:upReleased(shifted)
  if not shifted then self:unfocus() end
  return true
end

function ViewControl:onRemove()
  -- app.logInfo("%s:onRemove()",self)
  if self.pinCount > 0 then
    local chain = self:getRootChain()
    if chain and chain.isRoot then
      chain:unpinControlFromAllPinSets(self)
    else
      app.logInfo("%s:onRemove(): failed to get root chain.", self)
    end
  end
  Base.onRemove(self)
end

return ViewControl
