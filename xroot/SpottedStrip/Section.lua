local Class = require "Base.Class"
local Widget = require "Base.Widget"
local SectionView = require "SpottedStrip.SectionView"
local Overlay = require "Overlay"

local Section = Class {}
Section:include(Widget)

function Section:init(style, markable)
  Widget.init(self)
  self:setClassName("Section")
  self.pSection = app.SpottedSection(0, 64, style or app.sectionMiddle,
                                    markable or false)
  self.hSection = self.pSection:handle()
  self.views = {}
  self.controls = {}
  self.currentViewName = nil
  self.guests = {}
end

function Section:setStyle(style)
  self.pSection:applyStyle(style)
end

-- Returns current view info when called with no arguments.
function Section:getView(name)
  return self.views[name or self.currentViewName]
end

function Section:hasView(name)
  return self.views[name] ~= nil
end

function Section:addView(name)
  local view = self.views[name]
  if view then
    -- already exists
    return view
  end

  view = SectionView(name)
  self.views[name] = view

  return view
end

function Section:removeView(name)
  self.views[name] = nil
end

function Section:mark()
  self.pSection:mark()
end

function Section:unmark()
  self.pSection:unmark()
end

function Section:marked()
  return self.pSection:marked()
end

function Section:toggleMarked()
  local pSection = self.pSection
  if pSection:marked() then
    pSection:unmark()
  else
    pSection:mark()
  end
end

function Section:rebuildView(viewName, focusControl, focusSpotIndex)
  local view = self.views[viewName or self.currentViewName]
  if view == nil then return end
  local parent = self.parent
  if parent then parent:disableSelection() end
  view:rebuild(self.pSection)
  self.currentViewName = view.name
  if parent then

    if focusControl and view:contains(focusControl) then
      local spotHandle =
          focusControl:getSpotValue(focusSpotIndex or 1, "handle")
      if spotHandle then parent:setSelection(self, view.name, spotHandle) end
    end

    parent:enableSelection()
  end
end

function Section:getControlFromSpotHandle(viewName, spotHandle)
  local view = self:getView(viewName)
  if view then
    local spot = view:getSpotByHandle(spotHandle)
    if spot then return spot:getControl() end
  end
end

function Section:switchView(viewName, focusControl, focusSpotIndex)
  -- app.logInfo("%s.switchView(%s)",self,name)
  if viewName == self.currentViewName then return end
  self:rebuildView(viewName, focusControl, focusSpotIndex)
end

function Section:registerControl(control)
  if control.parent == nil then
    self.controls[#self.controls + 1] = control
    self:addChildWidget(control)
    control:onInsert()
  else
    app.logInfo("%s:registerControl(%s): already registered.", self, control)
  end
end

function Section:unregisterControl(control)
  if control.parent == self then
    for i, control2 in ipairs(self.controls) do
      if control2 == control then
        table.remove(self.controls, i)
        break
      end
    end
    control:onRemove()
    self:removeChildWidget(control)
  else
    app.logInfo("%s:unregisterControl(%s): not registered.", self, control)
  end
end

function Section:addControl(viewName, control)
  self:registerControl(control)
  local view = self.views[viewName]
  if view == nil then
    app.logError("%s.addControl(%s,%s): invalid view", self, viewName, control)
  end
  view:addControl(control)
end

function Section:removeControl(viewName, control)
  self:unregisterControl(control)
  local view = self.views[viewName]
  if view == nil then
    app.logError("%s.removeControl(%s,%s): invalid view", self, viewName, control)
  end
  view:removeControl(control)
end

function Section:getControlCount(viewName)
  local view = self:getView(viewName)
  if view then return view:getControlCount() end
  return 0
end

function Section:notifyControls(method, ...)
  -- notify all the controls
  for _, control in pairs(self.controls) do
    local f = control[method]
    if f ~= nil then f(control, ...) end
  end
end

function Section:notifyVisibleControls(method, ...)
  -- notify only the controls in the current view
  if self.currentViewName == nil then return end
  local view = self:getView(self.currentViewName)
  view:notifyControls(method, ...)
end

function Section:leftJustify()
  self.pDisplay:leftJustify(self.pSection, 0)
end

-----------------------------------------------
-- custom handlers

function Section:onRemove()
  self:notifyControls("onRemove")
end

function Section:onInsert()
end

function Section:spotPressed(viewName, spotHandle, shifted, isFocusedPress)
  local view = self.views[viewName]
  if view then
    local spot = view:getSpotByHandle(spotHandle)
    if spot then
      local control = spot:getControl()
      local index = spot:getPositionOnControl()
      control:spotPressed(index, shifted, isFocusedPress)
      if control.getFloatingMenuItems then
        Overlay.startFloatingMenu(control)
      end
    end
  end
end

function Section:spotReleased(viewName, spotHandle, shifted)
  local view = self.views[viewName]
  if view then
    local spot = view:getSpotByHandle(spotHandle)
    if spot then
      local control = spot:getControl()
      local index = spot:getPositionOnControl()
      local choice = Overlay.endFloatingMenu()
      if not choice then
        control:spotReleased(index, shifted)
      end
    end
  end
end

function Section:onCursorMove(viewName, spotHandle, viewName0, spotHandle0,
                              shifted)
  local view0 = self.views[viewName0]
  if view0 == nil then return end
  local spot0 = view0:getSpotByHandle(spotHandle0)
  if spot0 == nil then return end
  local view = self.views[viewName]
  if view == nil then return end
  local spot = view:getSpotByHandle(spotHandle)
  if spot == nil then return end
  local control = spot:getControl()
  local control0 = spot0:getControl()
  local index = spot:getPositionOnControl()
  local index0 = spot0:getPositionOnControl()
  if control == control0 then
    control:onCursorMove(index, index0, shifted)
  else
    control0:onCursorLeave(index0)
    control:onCursorEnter(index)
  end
end

function Section:onCursorEnter(viewName, spotHandle, shifted)
  local view = self.views[viewName]
  if view then
    local spot = view:getSpotByHandle(spotHandle)
    if spot then
      local control = spot:getControl()
      local index = spot:getPositionOnControl()
      control:onCursorEnter(index, shifted)
    end
  end
end

function Section:onCursorLeave(viewName, spotHandle, shifted)
  local view = self.views[viewName]
  if view then
    local spot = view:getSpotByHandle(spotHandle)
    if spot then
      local control = spot:getControl()
      local index = spot:getPositionOnControl()
      control:onCursorLeave(index, shifted)
    end
  end
end

return Section
