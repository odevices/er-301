local app = app
local Class = require "Base.Class"
local Keyboard = require "Keyboard"
local Persist = require "Persist"
local SpottedStrip = require "SpottedStrip"
local Section = require "SpottedStrip.Section"
local SpottedControl = require "SpottedStrip.Control"
local ply = app.SECTION_PLY
local selectedSlot = -1

local function showMessage(...)
  local SG = require "Overlay"
  SG.flashMainMessage(...)
end

local function getUnitCount(preset, chain)
  local n = preset:getUnitCount(chain, true)
  if n == 1 then
    return "1 unit  "
  else
    return string.format("%d units", n)
  end
end

local function addOUT1(graphic, instructions, preset, chain)
  local label = app.Label("OUT1:", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(5, app.GRID5_LINE1)
  graphic:addChild(label)
  label = app.Label(getUnitCount(preset, chain), 10)
  label:setJustification(app.justifyRight)
  label:setSize(ply, label.mHeight)
  label:setPosition(app.COL2_LEFT, app.GRID5_LINE1)
  graphic:addChild(label)
  instructions:hline(0, app.COL3_LEFT, app.GRID5_LINE1 + 1)
end

local function addOUT2(graphic, instructions, preset, chain)
  local label = app.Label("OUT2:", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(5, app.GRID5_LINE2)
  graphic:addChild(label)
  label = app.Label(getUnitCount(preset, chain), 10)
  label:setJustification(app.justifyRight)
  label:setSize(ply, label.mHeight)
  label:setPosition(app.COL2_LEFT, app.GRID5_LINE2)
  graphic:addChild(label)
  instructions:hline(0, app.COL3_LEFT, app.GRID5_LINE2 + 1)
end

local function addOUT3(graphic, instructions, preset, chain)
  local label = app.Label("OUT3:", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(5, app.GRID5_LINE3)
  graphic:addChild(label)
  label = app.Label(getUnitCount(preset, chain), 10)
  label:setJustification(app.justifyRight)
  label:setSize(ply, label.mHeight)
  label:setPosition(app.COL2_LEFT, app.GRID5_LINE3)
  graphic:addChild(label)
  instructions:hline(0, app.COL3_LEFT, app.GRID5_LINE3 + 1)
end

local function addOUT4(graphic, instructions, preset, chain)
  local label = app.Label("OUT4:", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(5, app.GRID5_LINE4)
  graphic:addChild(label)
  label = app.Label(getUnitCount(preset, chain), 10)
  label:setJustification(app.justifyRight)
  label:setSize(ply, label.mHeight)
  label:setPosition(app.COL2_LEFT, app.GRID5_LINE4)
  graphic:addChild(label)
end

local function addOUT12(graphic, instructions, preset, chain)
  local label = app.Label("OUT1+2:", 10)
  label:setJustification(app.justifyLeft)
  local y = (app.GRID5_LINE1 + app.GRID5_LINE2) // 2
  label:setPosition(5, y)
  graphic:addChild(label)
  label = app.Label(getUnitCount(preset, chain), 10)
  label:setJustification(app.justifyRight)
  label:setSize(ply, label.mHeight)
  label:setPosition(app.COL2_LEFT, y)
  graphic:addChild(label)
  instructions:hline(0, app.COL3_LEFT, app.GRID5_LINE2 + 1)
end

local function addOUT23(graphic, instructions, preset, chain)
  local label = app.Label("OUT2+3:", 10)
  label:setJustification(app.justifyLeft)
  local y = (app.GRID5_LINE2 + app.GRID5_LINE3) // 2
  label:setPosition(5, y)
  graphic:addChild(label)
  label = app.Label(getUnitCount(preset, chain), 10)
  label:setJustification(app.justifyRight)
  label:setSize(ply, label.mHeight)
  label:setPosition(app.COL2_LEFT, y)
  graphic:addChild(label)
  instructions:hline(0, app.COL3_LEFT, app.GRID5_LINE3 + 1)
end

local function addOUT34(graphic, instructions, preset, chain)
  local label = app.Label("OUT3+4:", 10)
  label:setJustification(app.justifyLeft)
  local y = (app.GRID5_LINE3 + app.GRID5_LINE4) // 2
  label:setPosition(5, y)
  graphic:addChild(label)
  label = app.Label(getUnitCount(preset, chain), 10)
  label:setJustification(app.justifyRight)
  label:setSize(ply, label.mHeight)
  label:setPosition(app.COL2_LEFT, y)
  graphic:addChild(label)
end

local Slot = Class {}
Slot:include(SpottedControl)

function Slot:init(slot)
  SpottedControl.init(self)
  self:setClassName("Slot")
  self:setInstanceName(slot)
  self.slot = slot
  local preset = Persist.getQuickSavePreset(slot)
  local name = Persist.getQuickSaveName(slot) or string.format("Slot %d", slot)
  local version = preset and preset:getVersionString()
  self.version = version
  self.isCompatible = version and version >= "0.3.18"
  local width = slot == 1 and ply - 1 or ply
  local graphic
  if preset then
    if version then
      graphic = app.TextPanel(string.format("%s v%s", name, version), 1)
    else
      graphic = app.TextPanel(name, 1)
    end
  else
    graphic = app.TextPanel(name .. " (empty)", 1)
  end
  if not self.isCompatible then
    graphic:setForegroundColor(app.GRAY5)
  end
  graphic:setCornerRadius(3, 3, 3, 3)
  graphic:setBorder(1)
  graphic:setBorderColor(app.GRAY3)
  self:setControlGraphic(graphic)

  self:addSpotDescriptor{
    center = 0.5 * ply,
    radius = 0.5 * ply
  }

  -- construct sub display
  graphic = app.Graphic(0, 0, 128, 64)
  self.subGraphic = graphic

  local button
  button = app.SubButton("save", 1)
  graphic:addChild(button)
  button = app.SubButton("rename", 2)
  graphic:addChild(button)
  button = app.SubButton("load", 3)
  graphic:addChild(button)

  local drawing = app.Drawing(0, 0, 128, 64)
  local instructions = app.DrawingInstructions()
  instructions:hline(0, 128, app.GRID5_LINE4)
  instructions:hline(0, 128, app.GRID5_LINE4 - 2)
  drawing:add(instructions)
  graphic:addChild(drawing)

  if preset then
    local links = preset:getLinks()
    if links then
      if links.link12 and links.link34 then
        addOUT12(graphic, instructions, preset, 1)
        addOUT34(graphic, instructions, preset, 2)
      elseif links.link23 then
        addOUT1(graphic, instructions, preset, 1)
        addOUT23(graphic, instructions, preset, 2)
        addOUT4(graphic, instructions, preset, 3)
      elseif links.link12 then
        addOUT12(graphic, instructions, preset, 1)
        addOUT3(graphic, instructions, preset, 2)
        addOUT4(graphic, instructions, preset, 3)
      elseif links.link34 then
        addOUT1(graphic, instructions, preset, 1)
        addOUT2(graphic, instructions, preset, 2)
        addOUT34(graphic, instructions, preset, 3)
      else
        addOUT1(graphic, instructions, preset, 1)
        addOUT2(graphic, instructions, preset, 2)
        addOUT3(graphic, instructions, preset, 3)
        addOUT4(graphic, instructions, preset, 4)
      end
    else
    end

    local nSamples = preset:getSampleCount()
    local text
    if nSamples == 1 then
      text = "1 sample"
    else
      text = string.format("%d samples", nSamples)
    end
    local nGlobals = preset:getGlobalChainCount()
    if nGlobals == 1 then
      text = text .. " 1 global"
    else
      text = string.format("%s %d globals", text, nGlobals)
    end
    local panel = app.TextPanel(text, 3, 0.5, 50)
    panel:setPosition(panel.mLeft, 15)
    graphic:addChild(panel)
  end
end

function Slot:onCursorEnter()
  self:addSubGraphic(self.subGraphic)
  self:grabFocus("subReleased")
  self.controlGraphic:setBorderColor(app.WHITE)
  selectedSlot = self.slot
end

function Slot:onCursorLeave()
  self:removeSubGraphic(self.subGraphic)
  self:releaseFocus("subReleased")
  self.controlGraphic:setBorderColor(app.GRAY3)
end

function Slot:doRename()
  local task = function(text)
    if text ~= nil then
      self.controlGraphic:setText(text)
      Persist.setQuickSaveName(self.slot, text)
    end
  end
  local name = Persist.getQuickSaveName(self.slot)
  local kb = Keyboard("Rename Slot " .. self.slot, name, true, "NamingStuff")
  kb:subscribe("done", task)
  kb:show()
end

function Slot:subReleased(i, shifted)
  if i == 1 then
    self:callUp("save", self.slot)
  elseif i == 2 then
    self:doRename()
  elseif i == 3 then
    if self.isCompatible then
      self:callUp("load", self.slot)
    elseif self.version then
      showMessage("%s: Preset version is incompatible with this firmware.",
                  self.version)
    else
      showMessage("Preset version is incompatible with this firmware.")
    end
  end
  return true
end

-------------------------------------------------------
local QuickSaver = Class {}
QuickSaver:include(SpottedStrip)

function QuickSaver:init()
  SpottedStrip.init(self)
  self:setClassName("QuickSaver")
  self.suppressQuickSave = true

  local section = Section(app.sectionNoBorder)
  self:appendSection(section)
  section:addView("default")
  self.slots = {}
  for i = 1, 24 do
    local slot = Slot(i)
    section:addControl("default", slot)
    self.slots[i] = slot
  end

  local lastSlot = Persist.meta["boot"].lastSlot
  if lastSlot and self.slots[lastSlot] then
    local graphic = self.slots[lastSlot].controlGraphic
    graphic:setBackgroundColor(app.GRAY2)
    graphic:setOpaque(true)
  end
end

function QuickSaver:onShow()
  local section, viewName, spotHandle = self:getSelection()
  if selectedSlot < 1 then
    local lastSlot = Persist.meta["boot"].lastSlot or -1
    selectedSlot = lastSlot > 0 and lastSlot or 1
  end

  self:disableSelection()
  section:switchView("default")
  self:setSelection(section, viewName, selectedSlot - 1)
  self:enableSelection()
end

function QuickSaver:load(slot)
  self:hide()
  self:clear()
  Persist.quickLoad(slot)
end

function QuickSaver:save(slot)
  self:hide()
  self:clear()
  Persist.quickSaveToSlot(slot, true)
end

function QuickSaver:homeReleased()
  local section, viewName, spotHandle = self:getSelection()
  self:setSelection(section, viewName, 0)
  return true
end

function QuickSaver:upReleased(shifted)
  if not shifted then
    self:hide()
    self:clear()
  end
  return true
end

function QuickSaver:cancelReleased(shifted)
  if not shifted then
    self:hide()
    self:clear()
  end
  return true
end

return QuickSaver
