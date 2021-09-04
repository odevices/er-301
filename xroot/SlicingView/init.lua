local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Window = require "Base.Window"
local Settings = require "Settings"
local Overlay = require "Overlay"
local Encoder = require "Encoder"
local MenuOperations = require "SlicingView.MenuOperations"

local threshold = Env.EncoderThreshold.Default
local pointerSensitivity = Env.EncoderSensitivity.SampleDisplay

local emptyButtonText = "      "

-- interface modes
local ManualSlicing = 1
local AutoSlicing = 2

-- navigation modes
local SliceNavigation = "slices"
local CursorNavigation = "cursor"
local FollowNavigation = "follow"

-- SlicingView
local SlicingView = Class {}
SlicingView:include(Window)
SlicingView:include(MenuOperations)

function SlicingView:init(unit, head)
  Window.init(self)
  self:setClassName("SlicingView")
  self.unit = unit
  self.head = head

  self.mainDisplay = app.SlicingViewMainDisplay(head)
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
  self.subDisplay = app.SlicingViewSubDisplay(self.mainDisplay)
  self.subGraphic:addChild(self.subDisplay)

  self.subButton1 = app.SubButton("|<<", 1)
  self.subGraphic:addChild(self.subButton1)

  self.subButton2 = app.SubButton("> / ||", 2)
  self.subGraphic:addChild(self.subButton2)

  self.subButton3 = app.SubButton("cycle", 3)
  self.subGraphic:addChild(self.subButton3)

  self.zooming = false
  self.encoderState = Encoder.Horizontal

  self:setInterfaceMode(ManualSlicing)
  self:setNavigationMode(CursorNavigation)
end

function SlicingView:hasSelection()
  return self.mainDisplay:isMarked()
end

function SlicingView:getSelectionDuration()
  return self.mainDisplay:getMarkedDuration()
end

function SlicingView:getSelectionBeginTime()
  return self.mainDisplay:getMarkedBeginTime()
end

function SlicingView:getSelectionEndTime()
  return self.mainDisplay:getMarkedEndTime()
end

function SlicingView:hideMainButtons()
  for i = 1, 6 do self.mainButton[i]:hide() end
end

function SlicingView:showMainButtons()
  for i = 1, 6 do self.mainButton[i]:show() end
end

function SlicingView:setMainButtons(b)
  b = b or {}
  for i = 1, 6 do self.mainButton[i]:setText(b[i] or "") end
end

function SlicingView:getMainButtons()
  local b = {}
  for i = 1, 6 do b[i] = self.mainButton[i]:getText() end
  return b
end

function SlicingView:highlightMainButton(i, value)
  self.mainButton[i]:setInvert(value)
end

function SlicingView:setInterfaceMode(mode, flash)
  if mode == ManualSlicing then
    self.interfaceMode = ManualSlicing
    self.mainButton[1]:setText("Loop In")
    self.mainButton[2]:setText("Loop Out")
    self.mainButton[3]:setText("+ Slice")
    self.mainButton[4]:setText("- Slice")
    self.mainButton[5]:setText("goto ZC")
    if flash then Overlay.flashMainMessage("Mode: Manual Slicing") end
  end

  -- buttons 6 is always same
  self.mainButton[6]:setText("menu")
end

function SlicingView:toggleNavigationMode()
  if self.navigationMode == SliceNavigation then
    self:setNavigationMode(CursorNavigation)
  elseif self.navigationMode == CursorNavigation then
    self:setNavigationMode(FollowNavigation)
  elseif self.navigationMode == FollowNavigation then
    self:setNavigationMode(SliceNavigation)
  end
end

function SlicingView:setNavigationMode(mode)
  if mode == SliceNavigation then
    self.mainDisplay:disableFollowing()
    self.subDisplay:setSlicesMode()
    self.subDisplay:syncSelectedSliceToPointer()
  elseif mode == CursorNavigation then
    self.mainDisplay:disableFollowing()
    self.subDisplay:setCursorMode()
  elseif mode == FollowNavigation then
    self.mainDisplay:enableFollowing()
    self.subDisplay:setFollowMode()
  end
  self.navigationMode = mode
end

function SlicingView:setSample(sample)
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

function SlicingView:upReleased(shifted)
  if shifted then return false end
  self:hide()
  self:emitSignal("done")
  return true
end

function SlicingView:subReleased(i, shifted)
  if i == 1 then
    if self.head then
      self.head:setManualMode(true)
      if self.navigationMode == SliceNavigation then
        local sliceIndex = self.subDisplay:getSelectedSliceIndex()
        self.head:setActiveSlice(sliceIndex)
      elseif self.navigationMode == CursorNavigation then
        local pointer = self.mainDisplay:getPointer()
        self.head:setReset(pointer)
        self.head:clearStops()
      end
      self.head:reset()
    end
  elseif i == 2 then
    if self.head then self.head:toggle() end
  elseif i == 3 then
    self:toggleNavigationMode()
  end
  return true
end

function SlicingView:encoderZoom(change, shifted)
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

function SlicingView:encoder(change, shifted)
  if self.zooming then
    self:encoderZoom(change, shifted)
    return true
  end

  if self.navigationMode == SliceNavigation then
    if self.subDisplay:encoderSlices(change, false, threshold) then
      self.subDisplay:syncPointerToSelectedSlice()
    end
  else
    self.mainDisplay:encoderPointer(change, shifted, pointerSensitivity)
  end
  return true
end

function SlicingView:dialPressed(shifted)
  self.zooming = "waiting"
end

function SlicingView:dialReleased(shifted)
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
end

function SlicingView:shiftPressed()
  self.mainDisplay:beginMarking()
end

function SlicingView:shiftReleased()
  self.mainDisplay:endMarking()
end

function SlicingView:cancelReleased(shifted)
  if self.mainDisplay:isMarked() then self.mainDisplay:clearMarking() end
end

function SlicingView:mainPressed(i, shifted)
  return true
end

function SlicingView:mainReleased(i, shifted)
  -- Handle buttons common to all tasks.
  if i == 6 then
    local Menu = require "SlicingView.Menu"
    local menu = Menu(self)
    menu:show()
    return true
  end

  if self.interfaceMode == ManualSlicing then
    if i == 1 then
      if self.mainDisplay:haveSliceUnderPointer() then
        if shifted then
          self.mainDisplay:setLoopStart(Settings.get(
                                            "shiftInsertSliceAtZeroCrossing"))
        else
          self.mainDisplay:setLoopStart(
              Settings.get("insertSliceAtZeroCrossing"))
        end
      else
        Overlay.flashMainMessage("No slice under cursor.")
      end
    elseif i == 2 then
      if self.mainDisplay:haveSliceUnderPointer() then
        if shifted then
          self.mainDisplay:setLoopEnd(Settings.get(
                                          "shiftInsertSliceAtZeroCrossing"))
        else
          self.mainDisplay:setLoopEnd(Settings.get("insertSliceAtZeroCrossing"))
        end
      else
        Overlay.flashMainMessage("No slice under cursor.")
      end
    elseif i == 3 then
      if shifted then
        self.mainDisplay:insertSlice(Settings.get(
                                         "shiftInsertSliceAtZeroCrossing"))
      else
        self.mainDisplay:insertSlice(Settings.get("insertSliceAtZeroCrossing"))
      end
    elseif i == 4 then
      self.mainDisplay:deleteSlice()
    elseif i == 5 then
      local n = self.mainDisplay:movePointerToNearestZeroCrossing()
      Overlay.flashMainMessage(string.format("Cursor moved %d samples.", n))
    end
  end
  return true
end

function SlicingView:onShow()
  Encoder.set(self.encoderState)
end

function SlicingView:onHide()
  Encoder.set(Encoder.Neutral)
end

function SlicingView:onClose()
  self.head:setManualMode(false)
end

return SlicingView
