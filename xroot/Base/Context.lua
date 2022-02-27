local app = app
local Object = require "Base.Object"
local Class = require "Base.Class"
local Settings = require "Settings"

local function topChanged(context, oldWindow, window, slide)
  local animation = Settings.get("animation")
  if oldWindow and oldWindow.mainGraphic == window.mainGraphic then
    animation = "disabled"
  end
  if animation == "disabled" then
    context.mainGraphicContext:setGraphic(window.mainGraphic)
  elseif animation == "slow" then
    if slide == "down" then
      context.mainGraphicContext:slideDownTo(window.mainGraphic, 4)
    elseif slide == "up" then
      context.mainGraphicContext:slideUpTo(window.mainGraphic, 4)
    else
      context.mainGraphicContext:setGraphic(window.mainGraphic)
    end
  elseif animation == "normal" then
    if slide == "down" then
      context.mainGraphicContext:slideDownTo(window.mainGraphic, 8)
    elseif slide == "up" then
      context.mainGraphicContext:slideUpTo(window.mainGraphic, 8)
    else
      context.mainGraphicContext:setGraphic(window.mainGraphic)
    end
  elseif animation == "fast" then
    if slide == "down" then
      context.mainGraphicContext:slideDownTo(window.mainGraphic, 16)
    elseif slide == "up" then
      context.mainGraphicContext:slideUpTo(window.mainGraphic, 16)
    else
      context.mainGraphicContext:setGraphic(window.mainGraphic)
    end
  end
  context.subGraphicContext:setGraphic(window.subGraphic)
  local focus = window:getFocusedWidget("encoder")
  local controller = focus.subCursorController or focus.subGraphic
  context.subGraphicContext:setCursorController(controller)
  controller = focus.mainCursorController or focus.mainGraphic
  context.mainGraphicContext:setCursorController(controller)
end

local function showWindow(window)
  if not window.visible then
    window.visible = true
    window:broadcastDown("onShow")
  end
end

local function hideWindow(window)
  if window.visible then
    window.visible = false
    window:broadcastDown("onHide")
  end
end

---------------------------------------------------

local Context = Class {}
Context:include(Object)

function Context:init(name, window)
  self:setClassName("Context")
  self:setInstanceName(name)
  -- event filters
  self.filters = {}
  self.visible = false
  self.stack = {}
  self.mainGraphicContext = app.GraphicContext()
  self.subGraphicContext = app.GraphicContext()
  if window == nil then
    app.logError("%s.init: window cannot be nil.", self)
  end
  self:add(window)
end

function Context:top()
  return self.stack[#self.stack]
end

function Context:root()
  return self.stack[1]
end

-- Only the top window of the visible context is showing.
function Context:show()
  if not self.visible then
    self.visible = true
    showWindow(self:top())
    app.UIThread.setMainGraphicContext(self.mainGraphicContext)
    app.UIThread.setSubGraphicContext(self.subGraphicContext)
  end
end

function Context:hide()
  if self.visible then
    app.UIThread.setMainGraphicContext(nil)
    app.UIThread.setSubGraphicContext(nil)
    hideWindow(self:top())
    self.visible = false
  end
end

function Context:add(window)
  if not window.isWindow then
    app.logError("%s.add(%s): not a window.", self, window)
  end
  if window.context then
    app.logError("%s.add(%s): window is already in %s.", self, window,
                 window.context)
  end
  local stack = self.stack
  local prev = stack[#stack]
  stack[#stack + 1] = window
  window.context = self
  topChanged(self, prev, window, "up")
  if self.visible then
    hideWindow(prev)
    showWindow(window)
  end
end

function Context:remove(window)
  if window.context ~= self then
    app.logError("%s.remove(%s): window not in this context.", self, window)
  end
  local stack = self.stack
  if #stack < 2 then
    app.logInfo("%s.remove(%s): ignoring attempt to remove last window.", self,
                window)
    return
  end
  local prev = stack[#stack]
  for i = 1, #stack do
    if stack[i] == window then
      if self.visible and i == #stack then
        hideWindow(window)
      end
      table.remove(stack, i)
      window:broadcastDown("onClose")
      window.context = nil
      break
    end
  end
  local cur = stack[#stack]
  if prev ~= cur then
    -- top of the stack changed
    topChanged(self, prev, cur, "down")
    if self.visible then
      showWindow(cur)
    end
  end
end

function Context:replace(oldWindow, newWindow)
  if not oldWindow.isWindow or not newWindow.isWindow then
    app.logError("%s.replace(%s,%s): not a window.", self, oldWindow, newWindow)
  end
  if oldWindow.context ~= self then
    app.logError("%s.replace(%s,%s): old window is not in this context.", self,
                 oldWindow, newWindow)
  end
  if newWindow.context then
    app.logError("%s.replace(%s,%s): new window is already in a context.", self,
                 oldWindow, newWindow)
  end

  local i = self:getStackIndex(oldWindow)
  self.stack[i] = newWindow
  newWindow.context = self
  if i == #self.stack then
    topChanged(self, oldWindow, newWindow, "none")
    if self.visible then
      hideWindow(oldWindow)
      showWindow(newWindow)
    end
  end
  oldWindow:broadcastDown("onClose")
  oldWindow.context = nil
end

-- debug
function Context:upPressed(shifted)
  if shifted then
    self:dump()
    return true
  else
    return false
  end
end

-- capture shift M1 for quicksaving
function Context:mainPressed(i, shifted)
  if i == 1 and shifted then
    local root = self:top()
    if not root.suppressQuickSave then
      local Card = require "Card"
      if Card.mounted() then
        local Busy = require "Busy"
        local QuickSaver = require "Persist.QuickSaver"
        Busy.start()
        QuickSaver():show()
        Busy.stop()
      else
        local Overlay = require "Overlay"
        Overlay.flashMainMessage("Quicksave: No card mounted.")
      end
      return true
    end
  end
  return false
end

-- debug
function Context:dialPressed(shifted)
  if shifted then
    return true
  else
    return false
  end
end

function Context:notify(e, ...)
  -- pass the event through any local handlers first
  local handler = self[e]
  if handler and handler(self, ...) then
    return
  end
  -- otherwise pass onto the focused widget
  local focus = self:top():getFocusedWidget(e)
  focus:sendUp(e, ...)
end

function Context:dump()
  app.logInfo("%s: stack dump", self)
  for i, o in ipairs(self.stack) do
    app.logInfo("  %d: %s", i, o)
    for e, w in pairs(o.focus) do
      app.logInfo("  + %s --> %s", e, w)
    end
  end
end

function Context:getStack()
  return self.stack
end

function Context:getStackIndex(window)
  local stack = self.stack
  for i = 1, #stack do
    if window == stack[i] then
      return i
    end
  end
end

function Context:destroy()
  if not self.destroyed then
    self:hide()
    local stack = self.stack
    for i = 1, #stack do
      local window = stack[i]
      window:broadcastDown("onClose")
      window.context = nil
    end
    self.stack = {}
    self.mainGraphicContext:clear()
    self.subGraphicContext:clear()
    self.destroyed = true
  end
end

------------------------------

function Context:onEncoderFocusChanged()
  local window = self:top()
  if window.isCursorVisible then
    local focus = window:getFocusedWidget("encoder")
    local controller = focus.subCursorController or focus.subGraphic
    self.subGraphicContext:setCursorController(controller)
    controller = focus.mainCursorController or focus.mainGraphic
    self.mainGraphicContext:setCursorController(controller)
  else
    self.subGraphicContext:setCursorController(nil)
    self.mainGraphicContext:setCursorController(nil)
  end
end

return Context
