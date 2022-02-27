local app = app
local Class = require "Base.Class"
local SpottedStrip = require "SpottedStrip"
local Section = require "SpottedStrip.Section"
local MondrianMenu = require "MondrianMenu.AsSpottedControl"
local Message = require "Message"
local ply = app.SECTION_PLY

local function showMessage(...)
  local dialog = Message.Main(...)
  dialog:show()
end

--------------------------------------------------
local MenuSection = Class {}
MenuSection:include(Section)

function MenuSection:init()
  Section.init(self, app.sectionNoBorder)
  self:setClassName("Unit.ControlEditor.MenuSection")

  local menu = MondrianMenu(0, 0, 256 - ply - 2, 64)
  self.menu = menu
  self.menuControls = {}

  local graphic = app.Graphic(0, 0, 128, 64)
  self.subGraphic = graphic
  local panel = app.TextPanel("Load Preset", 1)
  graphic:addChild(panel)
  panel = app.TextPanel("Reset to Defaults", 2)
  graphic:addChild(panel)
  panel = app.TextPanel("Save Preset", 3)
  graphic:addChild(panel)

  self:addView("default")
  self:addControl("default", menu)
  self:switchView("default")
end

function MenuSection:addHeaderText(text)
  self.menu:addHeaderText(text)
end

function MenuSection:addKeyValue(args)
  local KeyValue = require "MondrianMenu.KeyValue"
  local control = KeyValue(args)
  self.menu:addControl(control)
  self.menuControls[args.key] = control
end

function MenuSection:addOption(args)
  local Option = require "MondrianMenu.Option"
  local control = Option(args)
  self.menu:addControl(control)
  self.menuControls[args.key] = control
end

function MenuSection:setValue(key, value, invalidateLayout)
  local control = self.menuControls[key]
  if control then
    control:setValue(value, invalidateLayout)
  end
end

function MenuSection:invalidateLayout()
  self.menu:invalidateLayout()
end

function MenuSection:focus()
  self.menu:focus()
end

function MenuSection:onCursorEnter(viewName, spotHandle, shifted)
  Section.onCursorEnter(self, viewName, spotHandle, shifted)
  self:addSubGraphic(self.subGraphic)
  self:grabFocus("subReleased")
end

function MenuSection:onCursorLeave(viewName, spotHandle, shifted)
  self:removeSubGraphic(self.subGraphic)
  Section.onCursorLeave(self, viewName, spotHandle, shifted)
  self:releaseFocus("subReleased")
end

function MenuSection:subReleased(i, shifted)
  if shifted then
    return true
  end
  if i == 1 then
    self:callUp("doLoadPreset")
  elseif i == 2 then
    self:callUp("doResetToDefaults")
  elseif i == 3 then
    self:callUp("doSavePreset")
  end
  return true
end

--------------------------------------------------
local TestSection = Class {}
TestSection:include(Section)

function TestSection:init(args)
  Section.init(self)
  self:setClassName("Unit.ViewControl.Editor.TestSection")
  local type = args.type or app.logError("%s.init: type is missing.", self)
  local BranchClass = require("Unit.ControlBranch." .. type)
  local unit = args.unit
  local id = args.id or unit:generateUniqueControlName(BranchClass.classPrefix)
  self.controlBranch = BranchClass {
    id = id,
    depth = unit.depth + 1,
    title = unit.title
  }
  self.control = self.controlBranch.control
  if args.isBuiltin then
    self.control.isBuiltin = true
  end
  if args.defaults then
    self.control:setDefaults(args.defaults)
    self.control:removeCustomizations(true)
  end
  if args.customizations then
    self.control:customize(args.customizations)
  end
  self:addView("default")
  self:addControl("default", self.control)
  self:switchView("default")
  self.controlBranch:start()

  self.control.enterReleased = function(self, shifted)
    return false
  end
end

function TestSection:releaseResources()
  self.controlBranch:releaseResources()
end

--------------------------------------------------
local ControlEditor = Class {}
ControlEditor:include(SpottedStrip)

function ControlEditor:init(args)
  SpottedStrip.init(self)
  self:setClassName("Unit.ControlEditor")
  local unit = args.unit or app.logError("%s.init: unit is missing.", self)
  local test = TestSection(args)
  local menu = MenuSection()
  test.control:populateEditMenu(menu)

  self:appendSection(menu)
  self.menu = menu
  self:appendSection(test)
  self.test = test
  self.unit = unit
  self.changeCount = 0

  self.menu:focus()
end

function ControlEditor:updateMenu()
  local control = self.test.control
  local menu = self.menu
  local keys = control:getCustomizableKeys()
  for _, key in ipairs(keys) do
    menu:setValue(key, control:getCustomizableValue(key))
  end
  menu:invalidateLayout()
end

function ControlEditor:doEditString(kvp)
  local Keyboard = require "Keyboard"
  local msg = string.format("Set %s.", kvp:getDescription())
  local kb = Keyboard(msg, kvp:getValue(), true, "ControlNames")
  if kvp:getKey() == "name" then
    kb:setValidator(function(text)
      return self.unit:validateControlName(text)
    end)
  end
  local task = function(text)
    if text then
      kvp:setValue(text, true)
      local t = {}
      local key = kvp:getKey()
      t[key] = text
      self.test.control:customize(t)
      self:onChange()
    end
  end
  kb:subscribe("done", task)
  kb:show()
end

function ControlEditor:doEditDecimal(kvp)
  local Decimal = require "Keyboard.Decimal"
  local msg = string.format("Set %s.", kvp:getDescription())
  local kb = Decimal {
    message = msg,
    initialValue = kvp:getValue()
  }
  local task = function(value)
    if value then
      kvp:setValue(value, true)
      local t = {}
      t[kvp:getKey()] = value
      self.test.control:customize(t)
      self:onChange()
    end
  end
  kb:subscribe("done", task)
  kb:show()
end

function ControlEditor:doEditInteger(kvp, options)
  local Decimal = require "Keyboard.Decimal"
  local msg = string.format("Set %s.", kvp:getDescription())
  local kb = Decimal {
    message = msg,
    initialValue = kvp:getValue(),
    integerOnly = true,
    mantissaFloor = options.min,
    mantissaCeiling = options.max
  }
  local task = function(value)
    if value then
      kvp:setValue(value, true)
      local t = {}
      t[kvp:getKey()] = value
      self.test.control:customize(t)
      self:onChange()
    end
  end
  kb:subscribe("done", task)
  kb:show()
end

function ControlEditor:onOptionChanged(option)
  local t = {}
  t[option:getKey()] = option:getValue()
  self.test.control:customize(t)
  self:onChange()
end

function ControlEditor:doLoadPreset()
  local FS = require "Card.FileSystem"
  local task = function(result)
    if result and result.fullpath then
      if FS.checkPath("control-preset", "r", result.fullpath) then
        local Preset = require "Persist.Preset"
        local preset = Preset()
        if preset:read(result.fullpath) then
          self:deserialize(preset.data)
        else
          showMessage("Failed to load control preset.")
        end
      else
        showMessage("Unable to read from that location.")
      end
    end
  end
  local FileChooser = require "Card.FileChooser"
  local chooser = FileChooser {
    msg = string.format("Load Preset (*.%s)", FS.getExt("control-preset")),
    goal = "load file",
    path = FS.getRoot("control-preset"),
    ext = "." .. FS.getExt("control-preset"),
    pattern = FS.getPattern("control-preset"),
    history = "controlEditorPreset"
  }
  chooser:subscribe("done", task)
  chooser:show()
end

function ControlEditor:doSavePreset()
  local FS = require "Card.FileSystem"
  local task = function(result)
    if result and result.fullpath then
      if FS.checkPath("control-preset", "w", result.fullpath) then
        local Preset = require "Persist.Preset"
        local preset = Preset(self:serialize())
        if not preset:write(result.fullpath) then
          showMessage("Failed to save control preset.")
        end
      else
        showMessage("Unable to write to that location.")
      end
    end
  end
  local FileChooser = require "Card.FileChooser"
  local chooser = FileChooser {
    msg = string.format("Save Preset (*.%s)", FS.getExt("control-preset")),
    goal = "save file",
    path = FS.getRoot("control-preset"),
    ext = "." .. FS.getExt("control-preset"),
    pattern = FS.getPattern("control-preset"),
    history = "controlEditorPreset"
  }
  chooser:subscribe("done", task)
  chooser:show()
end

function ControlEditor:doResetToDefaults()
  local Verification = require "Verification"
  local dlg = Verification.Main("Resetting to Defaults", "Are you sure?")
  local task = function(ans)
    if ans then
      self.test.control:removeCustomizations()
      self:updateMenu()
      self:onChange()
    end
  end
  dlg:subscribe("done", task)
  dlg:show()
end

function ControlEditor:releaseResources()
  self.test:releaseResources()
end

function ControlEditor:enterReleased()
  self:hide()
  self:releaseResources()
  self:emitSignal("done", self.test.controlBranch:serialize())
  return true
end

function ControlEditor:cancelReleased(shifted)
  if shifted then
    return false
  end
  self:doCancel()
  return true
end

function ControlEditor:upReleased(shifted)
  if shifted then
    return false
  end
  self:doCancel()
  return true
end

function ControlEditor:doCancel()
  if self.changeCount > 0 then
    local Verification = require "Verification"
    local dlg = Verification.Main("This will discard your edits.",
                                  "Are you sure?")
    local task = function(ok)
      if ok then
        self:hide()
        self:releaseResources()
        self:emitSignal("done")
      end
    end
    dlg:subscribe("done", task)
    dlg:show()
  else
    self:hide()
    self:releaseResources()
    self:emitSignal("done")
  end
end

function ControlEditor:onChange()
  self.changeCount = self.changeCount + 1
end

function ControlEditor:serialize()
  local control = self.test.control
  local keys = control:getCustomizableKeys()

  local c = {}
  for _, key in ipairs(keys) do
    c[key] = control:getCustomizableValue(key)
  end
  c.name = nil
  c.description = nil

  return {
    presetType = "ControlEditorPreset",
    controlType = control.type,
    customizations = c
  }
end

function ControlEditor:deserialize(t)
  self.test.control:removeCustomizations()
  if t.customizations then
    self.test.control:customize(t.customizations)
  end
  self:updateMenu()
  self:onChange()
end

return ControlEditor
