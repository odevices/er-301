local Class = require "Base.Class"
local Base = require "SpottedStrip.Section"
local InsertControl = require "PinView.InsertControl"
local EmptyControl = require "PinView.EmptyControl"
local MasterFader = require "PinView.MasterFader"
local MasterGate = require "PinView.MasterGate"
local Persist = require "Persist"
local ply = app.SECTION_PLY

local PinSet = Class {}
PinSet:include(Base)

function PinSet:init(args)
  local name = args.name or app.logError("%s.init: name is missing.", self)
  local task = args.task or app.logError("%s.init: task is missing.", self)
  Base.init(self, app.sectionMiddle, true)
  self:setClassName("PinView.PinSet")
  self:setInstanceName(name)
  self:setInstanceKey(args.instanceKey or Persist.generateInstanceKey())
  self.control2pin = {}
  self.pinOrder = {}
  self.isPinSet = true

  self:addView("collapsed")
  self:addView("expanded")
  self:switchView("expanded")
  self.insertControl = InsertControl()
  self.emptyControl = EmptyControl(name .. " (empty)")
  self.masterFader = MasterFader {
    name = name,
    task = task
  }
  self.masterGate = MasterGate {
    name = name
  }
  self:registerControl(self.insertControl)
  self:registerControl(self.emptyControl)
  self:registerControl(self.masterFader)
  self:registerControl(self.masterGate)
end

function PinSet:serialize()
  local t = {}
  for i, pinControl in ipairs(self.pinOrder) do
    local control = pinControl.delegate
    local target
    if pinControl.getEndValue then
      target = pinControl:getEndValue()
    end
    local entry = {
      unitKey = control.parent:getInstanceKey(),
      controlId = control.id,
      pinName = pinControl:getInstanceName(),
      targetValue = target
    }
    t[i] = entry
  end
  return {
    name = self:getInstanceName(),
    activeView = self.currentViewName,
    pins = t
  }
end

function PinSet:deserialize(t)
  self:unpinAll()

  if t.name then
    self:rename(t.name)
  end

  local chain = self.parent.chain
  if chain and t.pins then
    self:startViewModifications()
    for _, entry in ipairs(t.pins) do
      local unit = chain:findByInstanceKey(entry.unitKey)
      if unit then
        local control = unit:getControlByName(entry.controlId)
        if control then
          self:pinControl(control, entry.targetValue, entry.pinName)
        else
          app.logWarn("%s: could not find control id %s", self, entry.controlId)
        end
      else
        app.logWarn("%s: could not find unit instance %s", self, entry.unitKey)
      end
    end
    self:endViewModifications()
  end

  if t.activeView then
    self:switchView(t.activeView)
  end
end

function PinSet:leaveHoldMode()
  self.masterFader:disengage()
end

function PinSet:notifyPinnedControls(method, ...)
  for control, _ in pairs(self.control2pin) do
    local f = control[method]
    if f ~= nil then
      f(control, ...)
    end
  end
end

function PinSet:notifyPinControls(method, ...)
  for _, control in ipairs(self.pinOrder) do
    local f = control[method]
    if f ~= nil then
      f(control, ...)
    end
  end
end

function PinSet:unfocus()
  self:notifyControls("unfocus")
end

function PinSet:unpinAll()
  self:startViewModifications()
  for control, _ in pairs(self.control2pin) do
    self:unpinControl(control)
  end
  self:endViewModifications()
end

function PinSet:rename(name)
  self:setInstanceName(name)
  self.masterFader:setName(name)
  self.masterGate:setName(name)
  self.emptyControl:setText(name .. " (empty)")
end

function PinSet:doRename()
  local Keyboard = require "Keyboard"
  local suggested = self:getInstanceName()
  local kb = Keyboard("Rename PinSet", suggested, true, "NamingStuff")
  local task = function(text)
    if text then
      self:rename(text)
    end
  end
  kb:subscribe("done", task)
  kb:show()
end

function PinSet:renamePin(control, name)
  local pin = self:getPinControl(control)
  if pin then
    pin:rename(name)
  end
end

function PinSet:getPinControl(control)
  return self.control2pin[control]
end

function PinSet:pinControl(control, optionalTargetValue, optionalName)
  if self.control2pin[control] then
    -- prevent duplicates
    return
  end
  if control.getPinControl then
    local pinControl = control:getPinControl()
    self.control2pin[control] = pinControl
    self.pinOrder[#self.pinOrder + 1] = pinControl
    self:registerControl(pinControl)

    if pinControl.fader then
      local controlParam = pinControl.fader:getValueParameter()
      local targetParam = pinControl.fader:getTargetParameter()
      if optionalTargetValue then
        targetParam:hardSet(optionalTargetValue)
      end
      self.masterFader:addFollower(controlParam, targetParam)
    elseif pinControl.comparator then
      self.masterGate:addFollower(pinControl.comparator)
    end

    if optionalName then
      pinControl:rename(optionalName)
    end

    if control.onPinned then
      control:onPinned()
    end
  end
end

function PinSet:unpinControl(control)
  local pinControl = self.control2pin[control]
  if pinControl then
    pinControl:unfocus()
    self.control2pin[control] = nil
    for i, pinControl2 in ipairs(self.pinOrder) do
      if pinControl == pinControl2 then
        table.remove(self.pinOrder, i)
        break
      end
    end
    self:unregisterControl(pinControl)

    if pinControl.fader then
      local param = pinControl.fader:getValueParameter()
      self.masterFader:removeFollower(param)
    elseif pinControl.comparator then
      self.masterGate:removeFollower(pinControl.comparator)
    end

    if control.onUnpinned then
      control:onUnpinned()
    end

    return true
  end
  return false
end

function PinSet:startViewModifications()
  self.parent:disableSelection()
end

function PinSet:endViewModifications()
  local expanded = self:getView("expanded")
  local collapsed = self:getView("collapsed")
  expanded:clear()
  collapsed:clear()
  expanded:addControl(self.insertControl)
  collapsed:addControl(self.insertControl)
  local hasFaders = self.masterFader:count() > 0
  local hasGates = self.masterGate:count() > 0
  if hasFaders and hasGates then
    -- self.masterFader:setNameCenter(ply/2)
    self.masterFader:showName()
    self.masterGate:hideName()
    expanded:addControl(self.masterFader)
    expanded:addControl(self.masterGate)
    expanded:addDivider(2 * ply)
    collapsed:addControl(self.masterFader)
    collapsed:addControl(self.masterGate)
  elseif hasFaders then
    -- self.masterFader:setNameCenter(ply/2)
    self.masterFader:showName()
    expanded:addControl(self.masterFader)
    collapsed:addControl(self.masterFader)
    expanded:addDivider(ply)
  elseif hasGates then
    -- self.masterGate:setNameCenter(ply/2)
    self.masterGate:showName()
    expanded:addControl(self.masterGate)
    collapsed:addControl(self.masterGate)
    expanded:addDivider(ply)
  else
    expanded:addControl(self.emptyControl)
    collapsed:addControl(self.emptyControl)
  end
  for _, control in ipairs(self.pinOrder) do
    expanded:addControl(control)
  end

  self:rebuildView()
  self.parent:enableSelection()
  -- HACK: Unless I do this onCursorEnter is not called when the previous selection is past the end of the section.
  self.parent:disableSelection()
  self.parent:enableSelection()
end

function PinSet:expand()
  self:switchView("expanded")
end

function PinSet:collapse()
  self:switchView("collapsed")
end

function PinSet:disengage()
  self.masterFader:disengage()
end

function PinSet:releaseResources()
  self:unfocus()
  self:disengage()
  self:unpinAll()
end

function PinSet:onInsert()
  self:startViewModifications()
  self:endViewModifications()
end

return PinSet
