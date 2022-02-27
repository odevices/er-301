local app = app
local Class = require "Base.Class"
local Base = require "SpottedStrip"
local PinSet = require "PinView.PinSet"
local Footer = require "PinView.Footer"

local PinView = Class {}
PinView:include(Base)

function PinView:init(chain)
  Base.init(self)
  self:setClassName("PinView")
  self.chain = chain
  self.task = app.ObjectList(string.format("PinView(%s)", chain.title))
  self.isCursorVisible = true
  self.message = app.Label(chain.title .. ": No pinned controls.", 10)
  self.message:setCenter(128, 32)
  self:addMainGraphic(self.message)
  self:appendSection(Footer())
end

local alphabet = {
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  'G',
  'H',
  'I',
  'J',
  'K',
  'L',
  'M',
  'N',
  'O',
  'P',
  'Q',
  'R',
  'S',
  'T',
  'U',
  'V',
  'W',
  'X',
  'Y',
  'Z'
}

function PinView:suggestPinSetName()
  -- single letter
  for _, letter in ipairs(alphabet) do
    if self:getPinSetByName(letter) == nil then
      return letter
    end
  end
  -- two letters
  for _, letter1 in ipairs(alphabet) do
    for _, letter2 in ipairs(alphabet) do
      local name = letter1 .. letter2
      if self:getPinSetByName(name) == nil then
        return name
      end
    end
  end
  return ""
end

function PinView:makeNameUnique(name)
  while self:getPinSetByName(name) do
    name = name .. "'"
  end
  return name
end

function PinView:doInsertPinSet()
  local Keyboard = require "Keyboard"
  local suggested = self:callUp("suggestPinSetName")
  local kb = Keyboard("New PinSet", suggested, true, "NamingStuff")
  local position = self:getSelectedSectionPosition() - 1
  local task = function(text)
    if text then
      local section = self:addPinSet{
        name = text,
        position = position
      }
      self:setSelection(section)
    end
  end
  kb:subscribe("done", task)
  kb:show()
end

function PinView:addPinSet(args)
  args = args or {}
  if args.name then
    args.name = self:makeNameUnique(args.name)
  else
    args.name = self:suggestPinSetName()
  end
  args.task = self.task
  local section = PinSet(args)
  if args.position then
    self:insertSection(section, args.position + 1)
  else
    self:insertSection(section, self:getSectionCount())
  end
  self.message:hide()
  return section
end

function PinView:cloneSelectedPinSet()
  local position = self:getSelectedSectionPosition()
  local pinSet = self:getSectionByIndex(position)
  local name = pinSet:getInstanceName() .. "'"
  local pinSet2 = self:addPinSet{
    name = name,
    position = position
  }
  local data = pinSet:serialize()
  data.name = nil
  pinSet2:deserialize(data)
  return pinSet2
end

function PinView:removePinSetByName(name)
  for _, section in pairs(self.sections) do
    if section.isPinSet and section:getInstanceName() == name then
      section:releaseResources()
      self:removeSection(section)
      break
    end
  end
  if self:getSectionCount() == 1 then
    self.message:show()
  end
end

function PinView:removeAllPinSets()
  for _, section in pairs(self.sections) do
    if section.isPinSet then
      section:releaseResources()
      self:removeSection(section)
    end
  end
  self.message:show()
end

function PinView:getPinSetByName(name)
  for _, section in pairs(self.sections) do
    if section.isPinSet and section:getInstanceName() == name then
      return section
    end
  end
end

function PinView:getPinSetMembership(control)
  local t = {}
  local n = self:getSectionCount() - 1
  local pinCount = 0
  for i = 1, n do
    local section = self:getSectionByIndex(i)
    if section:getPinControl(control) then
      t[section:getInstanceName()] = true
      pinCount = pinCount + 1
    end
  end
  return t, pinCount
end

function PinView:pinControlToAllPinSets(control)
  local n = self:getSectionCount() - 1
  for i = 1, n do
    local pinSet = self:getSectionByIndex(i)
    pinSet:startViewModifications()
    pinSet:pinControl(control)
    pinSet:endViewModifications()
  end
end

function PinView:unpinControlFromAllPinSets(control)
  local n = self:getSectionCount() - 1
  for i = 1, n do
    local pinSet = self:getSectionByIndex(i)
    if pinSet:getPinControl(control) then
      pinSet:startViewModifications()
      pinSet:unpinControl(control)
      pinSet:endViewModifications()
    end
  end
end

function PinView:getPinSetNames()
  local t = {}
  local n = self:getSectionCount() - 1
  for i = 1, n do
    local section = self:getSectionByIndex(i)
    t[#t + 1] = section:getInstanceName()
  end
  return t
end

function PinView:onShow()
  for _, section in pairs(self.sections) do
    if section.isPinSet and section.enterHoldMode then
      section:enterHoldMode()
    end
  end
  app.AudioThread.addTask(self.task, 0)
end

function PinView:onHide()
  app.AudioThread.removeTask(self.task)
  for _, section in pairs(self.sections) do
    if section.isPinSet and section.leaveHoldMode then
      section:leaveHoldMode()
    end
  end
end

function PinView:releaseResources()
  self:removeAllPinSets()
  app.AudioThread.removeTask(self.task)
  self.task:clear()
end

function PinView:pin(control, pinSetName)
  local pinSet = self:getPinSetByName(pinSetName)
  if pinSet then
    pinSet:startViewModifications()
    pinSet:pinControl(control)
    pinSet:endViewModifications()
  else
    app.logInfo("%s:pin(%s,%s): no PinSet by that name", self, control,
                pinSetName)
  end
end

function PinView:unpin(control, pinSetName)
  local pinSet = self:getPinSetByName(pinSetName)
  if pinSet then
    pinSet:startViewModifications()
    pinSet:unpinControl(control)
    pinSet:endViewModifications()
  else
    app.logInfo("%s:unpin(%s,%s): no PinSet by that name", self, control,
                pinSetName)
  end
end

function PinView:serialize()
  local t = {}
  local section, view, spot = self:getSerializableSelection()
  t.selection = {
    section = section,
    view = view,
    spot = spot
  }
  local pinSets = {}
  local n = self:getSectionCount() - 1
  for i = 1, n do
    local pinSet = self:getSectionByIndex(i)
    pinSets[i] = pinSet:serialize()
  end
  t.pinSets = pinSets
  return t
end

function PinView:deserialize(t)
  self:disableSelection()
  self:removeAllPinSets()
  if t.pinSets then
    for _, data in ipairs(t.pinSets) do
      local pinSet = self:addPinSet()
      pinSet:deserialize(data)
    end
  end
  local selection = t.selection
  if selection then
    self:setSerializableSelection(selection.section, selection.view,
                                  selection.spot)
  end
  self:enableSelection()
end

function PinView:notifyPinSets(method, ...)
  for _, section in pairs(self.sections) do
    if section.isPinSet then
      local f = section[method]
      if f then
        f(section, ...)
      end
    end
  end
end

function PinView:notifyOtherPinSets(except, method, ...)
  for _, section in pairs(self.sections) do
    if section.isPinSet and section ~= except then
      local f = section[method]
      if f then
        f(section, ...)
      end
    end
  end
end

return PinView
