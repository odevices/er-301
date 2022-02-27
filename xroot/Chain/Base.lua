local app = app
local Class = require "Base.Class"
local SpottedStrip = require "SpottedStrip"
local EmptySection = require "Chain.EmptySection"
local MuteGroup = require "Chain.MuteGroup"
local Persist = require "Persist"
local Busy = require "Busy"
local MarkMenu = require "Chain.MarkMenu"
local UnitFactory = require "Unit.Factory"
local ply = app.SECTION_PLY

local ChainBase = Class {}
ChainBase:include(SpottedStrip)

function ChainBase:init(args)
  SpottedStrip.init(self, args)
  self:setClassName("ChainBase")
  self:setInstanceKey(args.instanceKey or Persist.generateInstanceKey())
  local title = args.title or app.logError("%s.init: title is missing.", self)
  local depth = args.depth or app.logError("%s.init: depth is missing.", self)
  if args.subTitle then
    self:setInstanceName(string.format("%s,%s,depth=%s", args.title,
                                       args.subTitle, args.depth))
  else
    self:setInstanceName(title .. ",depth=" .. tostring(depth))
  end
  local channelCount = args.channelCount or
                           app.logError("%s.init: channelCount is missing.",
                                        self)

  self.title = title
  self.subTitle = args.subTitle
  self.started = false
  self.depth = depth
  self.channelCount = channelCount
  self.units = {}
  self.muteGroup = MuteGroup()
  self.outputs = {}
end

function ChainBase:getRootChain()
  return self
end

function ChainBase:findUnitByTitle(title)
  for i = 1, self:length() do
    local unit = self:getUnit(i)
    if unit.title == title then
      return unit
    end
  end

  for i = 1, self:length() do
    local unit = self:getUnit(i)
    local unit2 = unit:findUnitByTitle(title)
    if unit2 then
      return unit2
    end
  end
end

function ChainBase:findByInstanceKey(key)
  if self:getInstanceKey() == key then
    return self
  end

  for i = 1, self:length() do
    local unit = self:getUnit(i)
    local o = unit:findByInstanceKey(key)
    if o then
      return o
    end
  end
end

function ChainBase:findParameter(unitKey, objectKey, paramKey)
  local unit = self:findByInstanceKey(unitKey)
  if unit and unit.objects then
    local o = unit.objects[objectKey]
    if o then
      return o:getParameter(paramKey)
    end
  end
end

function ChainBase:findControl(unitKey, controlId)
  local unit = self:findByInstanceKey(unitKey)
  if unit then
    return unit:getControlByName(controlId)
  end
end

function ChainBase:getXPathNode()
  local section, viewName, spotHandle = self:getSelection()
  if section.pUnit then
    -- subtract 1 for the header section
    local nodeIndex = self:getSelectedSectionPosition() - 1
    local nodeName = section.title
    if section.currentViewName ~= "expanded" then
      return nodeName, nodeIndex
    end
    local control = section:getControlFromSpotHandle("expanded", spotHandle)
    if control then
      local nodeName2 = control:getInstanceName()
      local nodeIndex2 = section:getControlPositionInView("expanded", control)
      if nodeIndex2 then
        return nodeName, nodeIndex, nodeName2, nodeIndex2
      else
        return nodeName, nodeIndex
      end
    end
  else
    return "source", 0
  end
end

function ChainBase:getXPathToSelection()
  local xpath = app.XPath()
  local root = self:getRootChain()
  local context = root.context
  if context == nil then
    return xpath
  end
  local start = context:getStackIndex(root)
  local stack = context:getStack()
  if start and stack then
    for i = start, #stack do
      local window = stack[i]
      -- app.logInfo("stack %d: %s",i, window)
      if window.getXPathNode then
        local nodeName, nodeIndex, nodeName2, nodeIndex2 = window:getXPathNode()
        -- app.logInfo("xpath: %s (%s)",nodeName,nodeIndex)
        xpath:add(nodeIndex)
        -- xpath:add(nodeName, nodeIndex)
        if nodeName2 and nodeIndex2 then
          -- app.logInfo("xpath2: %s (%s)",nodeName2,nodeIndex2)
          xpath:add(nodeIndex2)
          -- xpath:add(nodeName2, nodeIndex2)
        end
      end
    end
  end
  return xpath
end

function ChainBase:notifyBranches(method, ...)
  for handle, unit in pairs(self.units) do
    unit:notifyBranches(method, ...)
  end
end

function ChainBase:notifyUnits(method, ...)
  for handle, unit in pairs(self.units) do
    local f = unit[method]
    if f ~= nil then
      -- app.logInfo("%s:notifyUnits > %s:%s",self,unit,method)
      f(unit, ...)
    end
  end
end

function ChainBase:notifyAll(method, ...)
  for handle, unit in pairs(self.units) do
    local f = unit[method]
    if f ~= nil then
      -- app.logInfo("%s:notifyAll > %s:%s",self,unit,method)
      f(unit, ...)
    end
    unit:notifyAll(method, ...)
  end
end

function ChainBase:setTitle(title, subTitle)
  self.title = title or self.title
  self.subTitle = subTitle or self.subTitle
  self:broadcastDown("chainTitleChanged", self.title, self.subTitle)
end

function ChainBase:setSubTitle(text)
  self.subTitle = text
  self:broadcastDown("chainTitleChanged", self.title, self.subTitle)
end

function ChainBase:clearSubTitle()
  self.subTitle = nil
  self:broadcastDown("chainTitleChanged", self.title)
end

function ChainBase:insertEmptySection()
  if self.emptySection then
    return
  end
  if self.channelCount == 1 then
    self.emptySection = EmptySection(3 * ply)
  else
    self.emptySection = EmptySection(2 * ply)
  end
  self:insertSection(self.emptySection, 2)
end

function ChainBase:removeEmptySection()
  if self.emptySection then
    self:removeSection(self.emptySection)
    self.emptySection = nil
  end
end

function ChainBase:createUndo()
  Busy.start("Saving undo data...")
  self.undoData = self:serialize()
  Busy.stop()
end

function ChainBase:clearUndo()
  self.undoData = nil
end

function ChainBase:undo()
  if self.undoData then
    Busy.start("Undoing...")
    self:deserialize(self.undoData)
    Busy.stop()
  end
end

function ChainBase:canUndo()
  return self.undoData ~= nil
end

function ChainBase:loadPreset(path)
  Persist.loadChainPreset(self, path)
end

function ChainBase:savePreset(path)
  Persist.saveChainPreset(self, path)
end

function ChainBase:expandAllUnits()
  self:disableSelection()
  for hSection, unit in pairs(self.units) do
    unit:expand()
  end
  self:enableSelection()
end

function ChainBase:collapseAllUnits()
  self:disableSelection()
  for hSection, unit in pairs(self.units) do
    unit:collapse()
  end
  self:enableSelection()
end

function ChainBase:toggleAllUnits()
  -- if any unit is collapsed then expand all, othewise collapse all
  for hSection, unit in pairs(self.units) do
    if unit:isCollapsed() then
      self:expandAllUnits()
      return
    end
  end
  self:collapseAllUnits()
end

function ChainBase:selectHeader()
  self:setSelection(self.headerSection, self.headerSection.currentViewName, 0)
end

function ChainBase:muteIfNeeded()
  local wasMuted = self.pChain:isMuted()
  if not wasMuted then
    self.pChain:mute()
  end
  return wasMuted
end

function ChainBase:unmuteIfNeeded(wasMuted)
  if not wasMuted then
    self.pChain:unmute()
  end
end

function ChainBase:mute()
  self.pChain:mute()
end

function ChainBase:unmute()
  self.pChain:unmute()
end

function ChainBase:isMuted()
  return self.pChain:isMuted()
end

function ChainBase:getOutput(ch)
  if ch == nil or self.channelCount == 1 then
    ch = 1
  end
  local outlet = self.outputs[ch]
  if outlet == nil then
    outlet = self.pChain:getOutput(ch - 1)
    self.outputs[ch] = outlet
  end
  return outlet
end

function ChainBase:getMonitoringOutput(ch)
  return self:getOutput(ch)
end

-- Unit Loading

function ChainBase:serializeMarkedUnits()
  local units = {}
  local n = self:length()
  local index0 = n
  local index1 = 0
  for i = 1, n do
    local unit = self:getUnit(i)
    if unit:marked() then
      if i < index0 then
        index0 = i
      elseif i > index1 then
        index1 = i
      end
      units[#units + 1] = unit:serialize()
    end
  end
  return units, index0, index1
end

function ChainBase:serializeUnits()
  local units = {}
  local n = self:length()
  for i = 1, n do
    local unit = self:getUnit(i)
    units[i] = unit:serialize()
  end
  return units
end

function ChainBase:serializeSelection()
  local section, view, spot = self:getSerializableSelection()
  return {
    section = section,
    view = view,
    spot = spot
  }
end

function ChainBase:startDeserialization()
  self:stop()
  self:removeAllUnits()
  app.AudioThread.beginTransaction()
  self:disableSelection()
end

function ChainBase:deserializeUnits(units, start)
  local offset = self:length() + 1
  if start then
    offset = start
  end
  if units then
    for i, uData in ipairs(units) do
      local loadInfo = UnitFactory.deserializeLoadInfo(uData.loadInfo, uData)
      if loadInfo then
        local unit, position = self:loadUnit(loadInfo, offset, i < #units)
        if unit then
          offset = position + 1
          unit:deserialize(uData)
        end
      end
    end
  else
    app.logInfo("%s:deserialize: no 'units' table.", self)
  end
end

function ChainBase:deserializeSelection(selection)
  if selection then
    self:setSerializableSelection(selection.section, selection.view,
                                  selection.spot)
  end
end

function ChainBase:finishDeserialization()
  self:enableSelection()
  self:start()
  app.AudioThread.endTransaction()
  -- post to happen after all of the input sources are assigned
  local Application = require "Application"
  Application.post(function()
    self:emitSignal("contentChanged", self)
  end)
end

function ChainBase:loadUnit(loadInfo, position, continuing)
  Busy.start("Loading %s", loadInfo.title or loadInfo.moduleName)

  local args = {
    chain = self,
    depth = self.depth,
    channelCount = self.channelCount
  }
  local unit = UnitFactory.instantiate(loadInfo, args)

  if unit == nil then
    Busy.stop()
    local Overlay = require "Overlay"
    Overlay.flashMainMessage("Failed to load unit: %s",
                             loadInfo.title or loadInfo.moduleName)
    return nil
  end

  self.undoData = nil
  self:disableSelection()
  if self:length() == 0 then
    self:removeEmptySection()
    position = 1
  end
  if position == nil then
    -- load before the currently selected unit
    position = self:getSelectedSectionPosition() - 1
    if position < 1 then
      position = 1
    end
  end
  self:insertSection(unit, position + 1)
  -- insert unit into audio loop
  self.pChain:lock()
  self.pChain:insertUnit(unit.pUnit, position - 1)
  self.pChain:unlock()
  self.units[unit.hSection] = unit
  unit:onGenerateTitle()
  unit:onLoadFinished()
  unit:switchView("expanded")

  self:enableSelection()
  unit:start()
  if not continuing then
    unit:leftJustify()
    self:emitSignal("contentChanged", self)
  end
  Busy.stop()
  return unit, position
end

function ChainBase:getUnit(position)
  if self:length() == 0 then
    return nil
  end
  -- skip header
  return self:getSectionByIndex(position + 1)
end

function ChainBase:removeUnit(unit, continuing)
  self:disableSelection()
  if unit == nil then
    return
  end
  -- get these before removing
  self.pChain:lock()
  self.pChain:removeUnit(unit.pUnit)
  self.pChain:unlock()
  self.units[unit.hSection] = nil
  self:removeSection(unit)
  if self:length() == 0 then
    self:insertEmptySection()
  end
  if not continuing then
    app.collectgarbage()
    self:emitSignal("contentChanged", self)
  end
  self:enableSelection()
end

function ChainBase:removeMarkedUnits()
  Busy.start("Removing marked units...")
  self:stop()
  local units = {}
  for _, unit in pairs(self.units) do
    if unit:marked() then
      units[#units + 1] = unit
    end
  end
  for _, unit in ipairs(units) do
    self:removeUnit(unit, false)
  end
  self:emitSignal("contentChanged", self)
  self:start()
  Busy.stop()
end

function ChainBase:removeAllUnits()
  if self:length() == 0 then
    app.collectgarbage()
    return
  end
  self:disableSelection()
  self:stop()
  self.pChain:lock()
  self.pChain:clearUnits()
  self.pChain:unlock()
  for hSection, unit in pairs(self.units) do
    self:removeSection(unit)
    self.units[hSection] = nil
  end
  app.collectgarbage()
  self:insertEmptySection()
  self:enableSelection()
  self:emitSignal("contentChanged", self)
  self:start()
end

function ChainBase:rebuildGraph()
  self.pChain:lock()
  self.pChain:unlock()
  self:emitSignal("contentChanged", self)
end

function ChainBase:length()
  return self.pChain:size()
end

function ChainBase:addToMuteGroup(control)
  self.muteGroup:add(control)
end

function ChainBase:removeFromMuteGroup(control)
  self.muteGroup:remove(control)
end

function ChainBase:soloControl(control)
  self.muteGroup:solo(control)
end

function ChainBase:muteControl(control)
  self.muteGroup:mute(control)
end

function ChainBase:toggleSoloOnControl(control)
  self.muteGroup:toggleSolo(control)
end

function ChainBase:toggleMuteOnControl(control)
  self.muteGroup:toggleMute(control)
end

function ChainBase:setLastPreset(path, filename)
  self.lastPresetPath = path
  self.lastPresetFilename = filename
end

function ChainBase:getLastPreset()
  return self.lastPresetPath, self.lastPresetFilename
end

------------------------------------------------------

function ChainBase:homeReleased()
  self:setSelection(self.headerSection, self.headerSection.currentViewName, 0)
  return true
end

function ChainBase:shiftReleased()
  if self:getMarkCount() > 0 then
    local menu = MarkMenu(self)
    menu:show()
  end
  return true
end

return ChainBase
