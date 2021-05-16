local Utils = require "Utils"
local app = app
local Class = require "Base.Class"
local Observable = require "Base.Observable"
local Signal = require "Signal"

local lastChooser = 1

local function showMessage(...)
  local Overlay = require "Overlay"
  Overlay.mainFlashMessage(...)
end

local Chooser = Class {}
Chooser:include(Observable)

function Chooser:init(opts)
  Observable.init(self)
  self:setClassName("Unit.Chooser")
  if opts.goal == "insert" then
    self.chain = opts.chain
    self.goal = "insert"
  elseif opts.goal == "replace" then
    self.unit = opts.unit
    self.chain = opts.unit.parent
    self.goal = "replace"
  end

  self.panels = {}
  local graphic = app.Graphic(0, 0, 128, 64)
  local Drawings = require "Drawings"
  local drawing = app.Drawing(0, 0, 128, 64)
  drawing:add(Drawings.Sub.HelpfulButtons)
  graphic:addChild(drawing)

  local panel = app.TextPanel("Category", 1, 0.5, app.GRID5_LINE3 - 1)
  self.panels[1] = panel
  graphic:addChild(panel)
  panel = app.TextPanel("A-to-Z", 2, 0.5, app.GRID5_LINE3 - 1)
  self.panels[2] = panel
  graphic:addChild(panel)
  panel = app.TextPanel("Presets", 3, 0.5, app.GRID5_LINE3 - 1)
  self.panels[3] = panel
  graphic:addChild(panel)

  local label = app.Label("Choose a unit.", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(3, app.GRID5_LINE1)
  graphic:addChild(label)
  self.help1 = label

  label = app.Label("M1-M6 to select.", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(3, app.GRID5_LINE2)
  graphic:addChild(label)
  self.help2 = label

  label = app.Label("CANCEL to abort.", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(3, app.GRID5_LINE3)
  graphic:addChild(label)
  self.help3 = label

  self.menuGraphic = graphic

  local Settings = require "Settings"
  local defaultChooser = Settings.get("unitBrowserDefault")
  if defaultChooser == "last" then
    self.index = lastChooser
  elseif defaultChooser == "category" then
    self.index = 1
  elseif defaultChooser == "A-to-Z" then
    self.index = 2
  elseif defaultChooser == "preset" then
    self.index = 3
  else
    self.index = lastChooser
  end
  self:setChooser(self.index)

  Signal.weakRegister("onUnitListChanged", self)
end

function Chooser:onUnitListChanged()
  if self.categoric then self.categoric:refresh() end
  if self.alphabetic then self.alphabetic:refresh() end
end

function Chooser:getChannelCount()
  return self.chain.channelCount
end

function Chooser:getCategoricChooser()
  if self.categoric == nil then
    local Chooser = require "Unit.Chooser.Default"
    self.categoric = Chooser(self, "category")
  end
  return self.categoric
end

function Chooser:getAlphabeticChooser()
  if self.alphabetic == nil then
    local Chooser = require "Unit.Chooser.Default"
    self.alphabetic = Chooser(self, "alphabet")
  end
  return self.alphabetic
end

function Chooser:getPresetChooser()
  if self.presets == nil then
    local PresetChooser = require "Unit.Chooser.Preset"
    self.presets = PresetChooser(self)
  end
  return self.presets
end

function Chooser:show(context)
  self.current:show(context)
end

function Chooser:hide()
  self.current:hide()
end

function Chooser:subReleased(i, shifted)
  if shifted then return false end
  if i == self.index then return end
  self:setChooser(i)
  return true
end

function Chooser:setChooser(i)
  local context = self.current and self.current.context
  if self.current then
    self.current:hide()
    self.current:removeSubGraphic(self.menuGraphic)
  end
  if i == 1 then
    self.current = self:getCategoricChooser()
    self.help1:setText("Choose a unit.")
    self.help2:setText("M1-M6 to select.")
  elseif i == 2 then
    self.current = self:getAlphabeticChooser()
    self.help1:setText("Choose a unit.")
    self.help2:setText("M1-M6 to select.")
  elseif i == 3 then
    self.current = self:getPresetChooser()
    self.help1:setText("Choose a preset.")
    self.help2:setText("ENTER to confirm.")
  end
  self.current:addSubGraphic(self.menuGraphic)
  self.index = i
  lastChooser = i
  self.panels[1]:deselect()
  self.panels[2]:deselect()
  self.panels[3]:deselect()
  self.panels[i]:select()
  if context then self.current:show(context) end
end

function Chooser:loadChainPreset(fullpath)
  self.current:hide()
  local Path = require "Path"
  local ChainPreset = require "Persist.ChainPreset"
  local path, filename = Path.split(fullpath)
  local preset = ChainPreset()
  if preset:read(fullpath) then
    if self.goal == "insert" then
      local UnitFactory = require "Unit.Factory"
      local loadInfo = UnitFactory.getBuiltin("MixerUnit")
      local mixer = self.chain:loadUnit(loadInfo)
      if mixer and preset.data then
        local branch = mixer:getInputBranch()
        Path.pushWorkingDirectory(path)
        branch:deserialize(preset.data)
        Path.popWorkingDirectory()
      end
    elseif self.goal == "replace" then
      local chain = self.chain
      local wasMuted = chain:muteIfNeeded()
      chain:removeUnit(self.unit)
      local UnitFactory = require "Unit.Factory"
      local loadInfo = UnitFactory.getBuiltin("MixerUnit")
      local mixer = self.chain:loadUnit(loadInfo)
      if mixer and preset.data then
        local branch = mixer:getInputBranch()
        Path.pushWorkingDirectory(path)
        branch:deserialize(preset.data)
        Path.popWorkingDirectory()
      end
      chain:unmuteIfNeeded(wasMuted)
    end
  else
    showMessage("Failed to read %s.", filename)
  end
end

function Chooser:loadUnitPreset(fullpath)
  self.current:hide()
  local Path = require "Path"
  local UnitPreset = require "Persist.UnitPreset"
  local path, filename = Path.split(fullpath)
  local preset = UnitPreset()
  if not preset:read(fullpath) then
    showMessage("Failed to read %s.", filename)
    return
  end
  local loadInfo = preset:getLoadInfo()
  if loadInfo == nil then
    showMessage("This preset does not contain unit type info.")
    return
  end
  if self.goal == "insert" then
    local unit = self.chain:loadUnit(loadInfo)
    if unit then
      Path.pushWorkingDirectory(path)
      unit:deserialize(preset.data)
      Path.popWorkingDirectory()
    else
      showMessage("Failed to load unit: %s", loadInfo.title)
    end
  elseif self.goal == "replace" then
    local state = self.unit:serialize()
    local chain = self.chain
    local wasMuted = chain:muteIfNeeded()
    chain:removeUnit(self.unit)
    local unit = chain:loadUnit(loadInfo)
    if unit then
      Path.pushWorkingDirectory(path)
      unit:deserialize(preset.data)
      Path.popWorkingDirectory()
      local originalOrder = unit:getControlOrder("expanded")
      unit:deserialize(state)
      unit:applyControlOrder("expanded", originalOrder)
    else
      showMessage("Failed to load unit: %s", loadInfo.title)
    end
    chain:unmuteIfNeeded(wasMuted)
  end
end

function Chooser:loadPreset(fullpath)
  local FileSystem = require "Card.FileSystem"
  if FileSystem.isType("chain-preset", fullpath) then
    self:loadChainPreset(fullpath)
  elseif FileSystem.isType("unit-preset", fullpath) then
    self:loadUnitPreset(fullpath)
  end
end

function Chooser:loadClipboard()
  self.current:hide()
  if self.goal == "insert" then
    local Clipboard = require "Chain.Clipboard"
    Clipboard.paste(self.chain, nil, 1)
  elseif self.goal == "replace" then
    local Clipboard = require "Chain.Clipboard"
    local chain = self.chain
    local wasMuted = chain:muteIfNeeded()
    chain:removeUnit(self.unit)
    Clipboard.paste(chain, nil, 1)
    chain:unmuteIfNeeded(wasMuted)
  end
end

function Chooser:load(loadInfo)
  if loadInfo.isPreset then
    app.logInfo("Loading preset: %s", loadInfo.filename)
    local FileSystem = require "Card.FileSystem"
    local Path = require "Path"
    local fullpath = Path.join(FileSystem.getRoot("libs"), loadInfo.libraryName,
                               loadInfo.filename)
    self:loadPreset(fullpath)
    return
  end
  self.current:hide()
  if self.goal == "insert" then
    local unit = self.chain:loadUnit(loadInfo)
    if not unit then showMessage("Failed to load unit: %s", loadInfo.title) end
  elseif self.goal == "replace" then
    local state = self.unit:serialize()
    local chain = self.chain
    local wasMuted = chain:muteIfNeeded()
    chain:removeUnit(self.unit)
    local unit = chain:loadUnit(loadInfo)
    if unit then
      local originalOrder = unit:getControlOrder("expanded")
      unit:deserialize(state)
      unit:applyControlOrder("expanded", originalOrder)
    else
      showMessage("Failed to load unit: %s", loadInfo.title)
    end
    chain:unmuteIfNeeded(wasMuted)
  end
end

function Chooser:cancelReleased(shifted)
  if not shifted then self.current:hide() end
  return true
end

function Chooser:upReleased(shifted)
  if not shifted then self.current:hide() end
  return true
end

function Chooser:homeReleased()
  self.current:hide()
  return true
end

function Chooser.serialize()
  local ChooserDefault = require "Unit.Chooser.Default"
  local t = {}
  t.recent = Utils.deepCopy(ChooserDefault.recent)
  return t
end

function Chooser.deserialize(t)
  if t.recent then
    local ChooserDefault = require "Unit.Chooser.Default"
    ChooserDefault.recent = Utils.deepCopy(t.recent)
  end
end

return Chooser
