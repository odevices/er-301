local app = app
local Env = require "Env"
local Timer = require "Timer"
local Signal = require "Signal"
local Class = require "Base.Class"
local Window = require "Base.Window"
local Card = require "Card"
local Settings = require "Settings"
local Overlay = require "Overlay"
local Encoder = require "Encoder"
local Busy = require "Busy"
local Menu = require "Sample.Editor.Menu"
local MenuOperations = require "Sample.Editor.MenuOperations"

local threshold = Env.EncoderThreshold.Default
local pointerSensitivity = Env.EncoderSensitivity.SampleDisplay

local emptyButtonText = "      "

-- interface modes
local EditOnly = 1

-- navigation modes
local CursorNavigation = "cursor"
local FollowNavigation = "follow"

-- Editor
local Editor = Class {}
Editor:include(Window)
Editor:include(MenuOperations)

function Editor:init(unit, head)
  Window.init(self)
  self:setClassName("Editor")
  self.unit = unit
  self.head = head

  self.mainDisplay = app.SampleEditorMainDisplay(head)
  self.mainGraphic:addChild(self.mainDisplay)
  self:setMainCursorController(self.mainDisplay)

  self.mainButton = {}

  self.mainButton[1] = app.MainButton(emptyButtonText, 1, true)
  self.mainGraphic:addChild(self.mainButton[1])

  self.mainButton[2] = app.MainButton(emptyButtonText, 2, true)
  self.mainGraphic:addChild(self.mainButton[2])

  self.mainButton[3] = app.MainButton(emptyButtonText, 3, true)
  self.mainGraphic:addChild(self.mainButton[3])

  self.mainButton[4] = app.MainButton(emptyButtonText, 4, true)
  self.mainGraphic:addChild(self.mainButton[4])

  self.mainButton[5] = app.MainButton(emptyButtonText, 5, true)
  self.mainGraphic:addChild(self.mainButton[5])

  self.mainButton[6] = app.MainButton(emptyButtonText, 6, true)
  self.mainGraphic:addChild(self.mainButton[6])

  -- sub display
  self.subDisplay = app.SampleEditorSubDisplay(self.mainDisplay)
  self.subGraphic:addChild(self.subDisplay)

  self.subButton1 = app.SubButton("|<<", 1)
  self.subGraphic:addChild(self.subButton1)

  if head.toggle then
    self.subButton2 = app.SubButton("> / ||", 2)
  else
    self.subButton2 = app.SubButton("", 2)
  end
  self.subGraphic:addChild(self.subButton2)

  self.subButton3 = app.SubButton("cycle", 3)
  self.subGraphic:addChild(self.subButton3)

  self.zooming = false
  self.encoderState = Encoder.Horizontal
  self:setInterfaceMode(EditOnly)
  self:setNavigationMode(CursorNavigation)
end

function Editor:setPointerLabel(label)
  self.mainDisplay:setPointerLabel(label)
end

function Editor:hasSelection()
  return self.mainDisplay:isMarked()
end

function Editor:getSelectionDuration()
  return self.mainDisplay:getMarkedDuration()
end

function Editor:getSelectionBeginTime()
  return self.mainDisplay:getMarkedBeginTime()
end

function Editor:getSelectionEndTime()
  return self.mainDisplay:getMarkedEndTime()
end

function Editor:hideMainButtons()
  for i = 1, 6 do self.mainButton[i]:hide() end
end

function Editor:showMainButtons()
  for i = 1, 6 do self.mainButton[i]:show() end
end

function Editor:setMainButtons(b)
  b = b or {}
  for i = 1, 6 do self.mainButton[i]:setText(b[i] or "") end
end

function Editor:getMainButtons()
  local b = {}
  for i = 1, 6 do b[i] = self.mainButton[i]:getText() end
  return b
end

function Editor:highlightMainButton(i, value)
  self.mainButton[i]:setInvert(value)
end

function Editor:setInterfaceMode(mode, flash)
  if mode == EditOnly then
    self.interfaceMode = mode
    self.mainButton[1]:setText("")
    self.mainButton[2]:setText("")
    self.mainButton[3]:setText("")
    self.mainButton[4]:setText("")
    self.mainButton[5]:setText("zc.snap")
    if flash then Overlay.flashMainMessage("Mode: Edit") end
  end

  -- buttons 6 is always same
  self.mainButton[6]:setText("menu")
end

function Editor:toggleNavigationMode()
  if self.navigationMode == CursorNavigation then
    self:setNavigationMode(FollowNavigation)
  elseif self.navigationMode == FollowNavigation then
    self:setNavigationMode(CursorNavigation)
  end
end

function Editor:setNavigationMode(mode)
  if mode == CursorNavigation then
    self.mainDisplay:disableFollowing()
    self.subDisplay:setCursorMode()
  elseif mode == FollowNavigation then
    self.mainDisplay:enableFollowing()
    self.subDisplay:setFollowMode()
  end
  self.navigationMode = mode
end

function Editor:setSample(sample)
  if sample then
    if sample:isFileBased() then
      self.subDisplay:setName(sample:getFilenameForDisplay(24))
    else
      self.subDisplay:setName(sample.name)
    end
  else
    self.subDisplay:setName("No sample.")
  end
  self.sample = sample
end

function Editor:upReleased(shifted)
  if shifted then return false end
  self:hide()
  return true
end

function Editor:subReleased(i, shifted)
  if i == 1 then
    if self.head then
      if self.head.setManualMode then self.head:setManualMode(true) end
      if self.navigationMode == CursorNavigation then
        local pointer = self.mainDisplay:getPointer()
        if self.head.setReset then
          self.head:setReset(pointer)
        else
          self.head:setPosition(pointer)
        end
        if self.head.clearStops then self.head:clearStops() end
      else
        self.head:setPosition(0)
      end
      if self.head.reset then self.head:reset() end
    end
  elseif i == 2 then
    if self.head.toggle then self.head:toggle() end
  elseif i == 3 then
    self:toggleNavigationMode()
  end
  return true
end

function Editor:encoderZoom(change, shifted)
  if self.zooming == "waiting" then
    self.zooming = "active"
    if self.encoderState == Encoder.Vertical then
      self.mainDisplay:showGainZoomGadget()
    else
      self.mainDisplay:showTimeZoomGadget()
    end
  end
  if self.zooming == "active" then
    self.mainDisplay:encoderZoom(change, shifted, threshold)
  end
end

function Editor:encoder(change, shifted)
  if self.zooming then
    self:encoderZoom(change, shifted)
  else
    self.mainDisplay:encoderPointer(change, shifted, pointerSensitivity)
  end
  return true
end

function Editor:dialPressed(shifted)
  self.zooming = "waiting"
  return true
end

function Editor:dialReleased(shifted)
  if self.zooming == "waiting" then
    if self.encoderState == Encoder.Vertical then
      self.encoderState = Encoder.Horizontal
    else
      self.encoderState = Encoder.Vertical
    end
    Encoder.set(self.encoderState)
  end
  self.zooming = nil
  self.mainDisplay:hideZoomGadget()
  return true
end

function Editor:shiftPressed()
  self.mainDisplay:beginMarking()
  return true
end

function Editor:shiftReleased()
  self.mainDisplay:endMarking()
  return true
end

function Editor:cancelReleased(shifted)
  if self.mainDisplay:isMarked() then self.mainDisplay:clearMarking() end
  return true
end

function Editor:mainPressed(i, shifted)
  return true
end

function Editor:mainReleased(i, shifted)
  -- Handle buttons common to all tasks.
  if i == 6 then
    local Menu = require "Sample.Editor.Menu"
    local menu = Menu(self)
    menu:show()
    return true
  end

  if self.interfaceMode == EditOnly then
    if i == 1 then
    elseif i == 2 then
    elseif i == 3 then
    elseif i == 4 then
    elseif i == 5 then
      local n = self.mainDisplay:movePointerToNearestZeroCrossing()
      Overlay.flashMainMessage(string.format("Cursor moved %d samples.", n))
    end
  end
  return true
end

function Editor:onShow()
  Encoder.set(self.encoderState)
end

function Editor:onHide()
  Encoder.set(Encoder.Neutral)
end

function Editor:onClose()
  if self.head.setManualMode then self.head:setManualMode(false) end
end

return Editor
