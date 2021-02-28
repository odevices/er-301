local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Window = require "Base.Window"

local SpottedStrip = Class {}
SpottedStrip:include(Window)

function SpottedStrip:init()
  self.pDisplay = app.SpottedStrip(0, 0, 256, 64)
  Window.init(self, self.pDisplay)
  self:setClassName("SpottedStrip")
  self.sections = {}
  self.disableCount = 0
end

function SpottedStrip:clear()
  self:disableSelection()
  for hSection, section in pairs(self.sections) do
    section:onRemove()
    self:removeChildWidget(section)
    self.sections[hSection] = nil
  end
  self.pDisplay:clear()
  self:enableSelection()
  app.collectgarbage()
end

function SpottedStrip:getSectionCount()
  return self.pDisplay:size()
end

function SpottedStrip:getSelectedSectionPosition()
  return self.pDisplay:getSelectedSectionIndex() + 1
end

-- inserts before the given position (1-based)
function SpottedStrip:insertSection(section, position)
  self:disableSelection()
  if position == nil then
    self.pDisplay:appendSection(section.pSection)
  elseif position == 0 then
    app.logError("%s:insertSection(%s,%d): 1-based position is zero!", self,
              section, position)
  else
    self.pDisplay:insertSection(section.pSection, position - 1)
  end
  self.sections[section.hSection] = section
  section.pDisplay = self.pDisplay
  self:addChildWidget(section)
  section:onInsert()
  self:enableSelection()
end

function SpottedStrip:appendSection(section)
  self:disableSelection()
  self.pDisplay:appendSection(section.pSection)
  self.sections[section.hSection] = section
  section.pDisplay = self.pDisplay
  self:addChildWidget(section)
  section:onInsert()
  self:enableSelection()
end

function SpottedStrip:removeSection(section)
  if self.sections[section.hSection] then
    self:disableSelection()
    section:onRemove()
    self:removeChildWidget(section)
    self.pDisplay:removeSection(section.pSection)
    self.sections[section.hSection] = nil
    section.pDisplay = nil
    self:enableSelection()
  end
end

function SpottedStrip:moveSection(section, position)
  if self.sections[section.hSection] then
    local section0, viewName0, spotHandle0 = self:getSelection()
    self.pDisplay:removeSection(section.pSection)
    self.pDisplay:insertSection(section.pSection, position - 1)
    self:setSelection(section0, viewName0, spotHandle0)
  end
end

function SpottedStrip:getSectionByIndex(position)
  local pSection = self.pDisplay:getSectionByIndex(position - 1)
  return self.sections[pSection:handle()]
end

function SpottedStrip:disableSelection()
  if self.disableCount == 0 then
    self.disableCount = 1
    local section, viewName, spotHandle = self:getSelection()
    if section then
      section:onCursorLeave(viewName, spotHandle)
    else
      -- app.logInfo("%s:disableSelection: nothing selected!",self)
    end
  else
    self.disableCount = self.disableCount + 1
  end
end

function SpottedStrip:enableSelection()
  if self.disableCount > 0 then
    self.disableCount = self.disableCount - 1
    if self.disableCount == 0 and self.pDisplay:size() > 0 then
      local section, viewName, spotHandle = self:getSelection(true)
      if section then
        self.pDisplay:select(section.pSection, spotHandle)
        section:onCursorEnter(viewName, spotHandle)
      else
        app.logInfo("%s:enableSelection: nothing selected!", self)
      end
    end
  end
end

function SpottedStrip:getSelection(defaultToLast)
  local pDisplay = self.pDisplay
  local pSection = pDisplay:getSelectedSection()
  if pSection == nil and defaultToLast then
    pDisplay:selectLast()
    pSection = pDisplay:getSelectedSection()
  end
  if pSection then
    local hSection = pSection:handle()
    local section = self.sections[hSection]
    local viewName = section.currentViewName
    local spotHandle = pDisplay:getSelectedSpotIndex()
    return section, viewName, spotHandle
  else
    -- app.logInfo("%s.getSelection: nothing selected.",self)
    return nil
  end
end

-- view: if nil then keep current view
function SpottedStrip:setSelection(section, viewName, spotHandle)
  local section0, viewName0, spotHandle0 = self:getSelection()
  section:switchView(viewName or section.currentViewName)
  self.pDisplay:select(section.pSection, spotHandle or 1)
  if self.disableCount == 0 then
    section, viewName, spotHandle = self:getSelection()
    if section ~= section0 or viewName ~= viewName0 or spotHandle ~= spotHandle0 then
      self:onCursorMove(section, viewName, spotHandle, section0, viewName0,
                        spotHandle0)
    end
  end
end

function SpottedStrip:getSerializableSelection()
  local section, viewName, spotHandle = self:getSelection()
  return self.pDisplay:getSelectedSectionIndex(), viewName, spotHandle
end

function SpottedStrip:setSerializableSelection(sectionIndex, viewName,
                                               spotHandle)
  local pSection = self.pDisplay:getSectionByIndex(sectionIndex)
  if pSection then
    local hSection = pSection:handle()
    local section = self.sections[hSection]
    self:setSelection(section, viewName, spotHandle)
  end
end

function SpottedStrip:notifySections(method, ...)
  -- notify all the sections
  for _, section in pairs(self.sections) do
    local f = section[method]
    if f ~= nil then f(section, ...) end
  end
end

function SpottedStrip:getMarkCount()
  return self.pDisplay:getMarkCount()
end

function SpottedStrip:clearMark()
  self.pDisplay:clearMark()
end

function SpottedStrip:getScrollPosition()
  return self.pDisplay:getScrollPosition()
end

function SpottedStrip:setScrollPosition(pos)
  self.pDisplay:setScrollPosition(pos)
end

-----------------------------------------------

local threshold = Env.EncoderThreshold.Default
function SpottedStrip:encoder(change, shifted)
  local section0, viewName0, spotHandle0 = self:getSelection()
  if self.pDisplay:encoder(change, shifted, threshold) then
    local section, viewName, spotHandle = self:getSelection()
    self:onCursorMove(section, viewName, spotHandle, section0, viewName0,
                      spotHandle0, shifted)
  end
  return true
end

function SpottedStrip:mainPressed(i, shifted)
  local pDisplay = self.pDisplay
  local section0, viewName0, spotHandle0 = self:getSelection()

  -- determine what would be selected
  local buttonCenter = app.getButtonCenter(i)
  local pSection1 = pDisplay:findSectionByScreenLocation(buttonCenter)
  if pSection1 == nil then
    app.logInfo("SpottedStrip(%s).mainPressed: no section at location %d.",
            self.name, buttonCenter)
    return true
  end
  local section1 = self.sections[pSection1:handle()]
  local viewName1 = section1.currentViewName
  local spotHandle1 = pSection1:findSpotIdByScreenLocation(buttonCenter)

  -- if already selected then this is a focused press
  local alreadySelected = section0 == section1 and viewName1 == viewName0 and
                              spotHandle0 == spotHandle1
  if not alreadySelected then
    -- set the new selection (simplified version of setSelection method)
    self.pDisplay:select(section1.pSection, spotHandle1)
    self:onCursorMove(section1, viewName1, spotHandle1, section0, viewName0,
                      spotHandle0)
  end
  -- fire event
  self.savedSpotPress = {
    section1,
    viewName1,
    spotHandle1,
    shifted
  }
  self:spotPressed(section1, viewName1, spotHandle1, shifted, alreadySelected)
  return true
end

function SpottedStrip:mainReleased(i, shifted)
  if self.savedSpotPress then
    -- fire event
    local section, viewName, spotHandle, shifted =
        table.unpack(self.savedSpotPress)
    self.savedSpotPress = nil
    self:spotReleased(section, viewName, spotHandle, shifted)
  end
  return true
end

-----------------------------------------------
-- custom handlers

function SpottedStrip:spotPressed(section, viewName, spotHandle, shifted,
                                  isFocusedPress)
  section:spotPressed(viewName, spotHandle, shifted, isFocusedPress)
end

function SpottedStrip:spotReleased(section, viewName, spotHandle, shifted)
  section:spotReleased(viewName, spotHandle, shifted)
end

function SpottedStrip:onCursorMove(section, viewName, spotHandle, section0,
                                   viewName0, spotHandle0, shifted)
  -- app.logInfo("%s.onCursorMove(%s,%s,%s,%s,%s,%s)",self,section,view,spot,section0,view0,spot0)
  if section == section0 then
    if section then
      section:onCursorMove(viewName, spotHandle, viewName0, spotHandle0, shifted)
    end
  else
    if section0 then section0:onCursorLeave(viewName0, spotHandle0, shifted) end
    if section then section:onCursorEnter(viewName, spotHandle, shifted) end
  end
end

return SpottedStrip
