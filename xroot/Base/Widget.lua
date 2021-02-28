local app = app
local Signal = require "Signal"
local Class = require "Base.Class"
local Observable = require "Base.Observable"

local verbose = false

local Widget = Class {}
Widget:include(Observable)

function Widget:init()
  Observable.init(self)
  self:setClassName("Widget")
  self.children = {}
  self.parent = nil
end

function Widget:setModal()
  self.modal = true
end

function Widget:addChildWidget(child)
  -- app.logInfo("%s:addChildWidget(%s)",self,child)
  child:setParentWidget(self)
end

function Widget:removeChildWidget(child)
  -- app.logInfo("%s:removeChildWidget(%s)",self,child)
  if child.parent == self then child:setParentWidget(nil) end
end

function Widget:setParentWidget(parent)
  if self.parent then self.parent.children[self] = nil end
  self.parent = parent
  if parent then parent.children[self] = true end
end

function Widget:setMainCursorController(graphic)
  self.mainCursorController = graphic
  -- Propagate change if the parent window exists and has a context.
  local window = self:getWindow()
  if window and window.context then
    local focus = window:getFocusedWidget("encoder")
    if focus == self then window.context:onEncoderFocusChanged() end
  end
end

function Widget:setSubCursorController(graphic)
  self.subCursorController = graphic
  -- Propagate change if the parent window exists and has a context.
  local window = self:getWindow()
  if window and window.context then
    local focus = window:getFocusedWidget("encoder")
    if focus == self then window.context:onEncoderFocusChanged() end
  end
end

function Widget:getWindow()
  local widget = self
  while widget.parent do
    widget = widget.parent
    if widget.isWindow then return widget end
  end
  return widget.isWindow and widget
end

-- WARNING: This will fail if the widget's containing window is not active (i.e. has no conttext).
function Widget:getRootWindow()
  local window = self:getWindow()
  if window and window.context then return window.context:root() end
end

function Widget:addSubGraphic(graphic)
  local window = self:getWindow()
  if window then window:addSubGraphic(graphic) end
end

function Widget:removeSubGraphic(graphic)
  local window = self:getWindow()
  if window then window:removeSubGraphic(graphic) end
end

function Widget:grabFocus(...)
  local window = self:getWindow()
  if window == nil then
    app.logInfo("%s:grabFocus: This widget has no parent window!", self)
    return
  end
  local events = {
    ...
  }
  for _, event in ipairs(events) do
    -- app.logInfo("%s:grabFocus(%s)",self,event)
    window:setFocusedWidget(event, self)
  end
end

function Widget:releaseFocus(...)
  local window = self:getWindow()
  if window == nil then
    app.logInfo("%s:releaseFocus: This widget has no parent window!", self)
    return
  end
  local events = {
    ...
  }
  for _, event in ipairs(events) do
    -- app.logInfo("%s:releaseFocus(%s)",self,event)
    if window.focus[event] == self then window:setFocusedWidget(event, nil) end
  end
end

function Widget:hasFocus(event)
  local window = self:getWindow()
  if window == nil then
    app.logInfo("%s:hasFocus: This widget has no parent window!", self)
    return false
  end
  return window.focus[event] == self
end

function Widget:queryUp(key)
  if verbose then app.logInfo("%s:queryUp(%s)", self, key) end
  local value = self[key]
  if value then
    return value
  elseif self.parent then
    return self.parent:queryUp(key)
  else
    return nil
  end
end

-- callUp is for methods
function Widget:callUp(method, ...)
  if verbose then app.logInfo("%s:callUp(%s)", self, method) end
  local handler = self[method]
  if handler then
    return handler(self, ...)
  elseif self.parent then
    return self.parent:callUp(method, ...)
  else
    return nil
  end
end

-- sendUp is for events
local sendUp_uid = 0
function Widget:sendUp(event, ...)
  sendUp_uid = sendUp_uid + 1
  if not self:sendUpHelper(sendUp_uid, verbose and event ~= "encoder", event,
                           ...) and verbose then
    app.logInfo("%s:sendUp(%s,uid=%d): not handled", self, event, sendUp_uid)
  end
end

function Widget:sendUpHelper(uid, verbose, event, ...)
  -- app.logInfo("%s:sendUp(%s,uid=%d)",self,method,uid)
  local handler = self[event]
  if handler then
    if verbose then
      app.logInfo("%s:sendUp(%s,uid=%d): handler found", self, event, uid)
    end
    if handler(self, ...) or self.modal then
      if verbose then
        app.logInfo("%s:sendUp(%s,uid=%d): handler accepted", self, event, uid)
      end
      return true
    end
  end
  if self.parent then
    -- propagate to parent
    return self.parent:sendUpHelper(uid, verbose, event, ...)
  else
    return false
  end
end

function Widget:broadcastDown(method, ...)
  if verbose then app.logInfo("%s:broadcastDown(%s)", self, method) end
  self:broadcastDownHelper(method, ...)
end

function Widget:broadcastDownHelper(method, ...)
  local handler = self[method]
  if handler then
    if verbose then
      app.logInfo("%s:broadcastDown(%s): handler found", self, method)
    end
    handler(self, ...)
  end
  local tmp = {}
  for child, _ in pairs(self.children) do tmp[#tmp + 1] = child end
  for i = 1, #tmp do tmp[i]:broadcastDownHelper(method, ...) end
end

function Widget.enableVerbose()
  verbose = true
end

function Widget.disableVerbose()
  verbose = false
end

-------------------------------------------------------
-- default notification handlers

return Widget
