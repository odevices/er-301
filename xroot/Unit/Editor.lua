local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Signal = require "Signal"
local SpottedStrip = require "SpottedStrip"
local Section = require "SpottedStrip.Section"
local SpottedControl = require "SpottedStrip.Control"
local Keyboard = require "Keyboard"
local ply = app.SECTION_PLY

--------------------------------------------------
local ItemHeader = Class {}
ItemHeader:include(SpottedControl)

function ItemHeader:init(control)
  SpottedControl.init(self)
  self:setClassName("Unit.Editor.ItemHeader")
  self.id = control.id
  self.name = control:getDisplayName()
  self.type = control.type
  self.canEdit = control.canEdit
  self.canDelete = not control.isBuiltin
  self.verticalDivider = ply

  local panel = app.TextPanel(self:getDisplayString(), 1)
  if self.canEdit then
    panel:setBorderColor(app.WHITE)
  else
    panel:setBorderColor(app.GRAY5)
    panel:setForegroundColor(app.GRAY7)
  end
  self:setControlGraphic(panel)
  self:addSpotDescriptor{
    center = 0.5 * ply,
    radius = ply
  }
  self.panel = panel

  self.menuGraphic = app.Graphic(0, 0, 128, 64)
  if self.canEdit then
    self.menuGraphic:addChild(app.TextPanel("Edit Control", 1))
  else
    self.menuGraphic:addChild(app.TextPanel("", 1))
  end
  if self.canDelete then
    self.menuGraphic:addChild(app.TextPanel("Delete Control", 2))
  else
    self.menuGraphic:addChild(app.TextPanel("", 2))
  end
  self.movePanel = app.TextPanel("Hold to Move", 3)
  self.menuGraphic:addChild(self.movePanel)
end

function ItemHeader:getDisplayString(name, type)
  name = name or self.name
  type = type or self.type or "Control"
  return string.format("%s %s", name, type)
end

function ItemHeader:onRename(oldName, newName)
  self.name = newName
  self.panel:setText(self:getDisplayString())
end

function ItemHeader:onCursorEnter()
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased", "subPressed", "spotReleased")
end

function ItemHeader:spotReleased(spot, shifted)
  if shifted then return false end
  if self.canEdit then self:sendUp("doEditControl", self.id) end
  return true
end

function ItemHeader:onCursorLeave()
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased", "subPressed", "encoder", "spotReleased")
  self.controlGraphic:setBorder(0)
  self.movePanel:setText("Hold to Move")
end

function ItemHeader:subPressed(i, shifted)
  if shifted then return false end
  if i == 3 then
    self:grabFocus("encoder")
    self.controlGraphic:setBorder(3)
    self.movePanel:setText("Place with Knob")
  end
  return true
end

function ItemHeader:subReleased(i, shifted)
  self:releaseFocus("encoder")
  self.controlGraphic:setBorder(0)
  self.movePanel:setText("Hold to Move")
  if shifted then return false end
  if i == 1 and self.canEdit then
    self:sendUp("doEditControl", self.id)
  elseif i == 2 and self.canDelete then
    self:sendUp("doDeleteControl", self.id)
  elseif i == 3 then

  end
  return true
end

local threshold = Env.EncoderThreshold.Default
local encoderSum = 0
function ItemHeader:encoder(change, shifted)
  encoderSum = encoderSum + change
  if encoderSum > threshold then
    encoderSum = 0
    self:sendUp("doMoveControlRight", self.id)
  elseif encoderSum < -threshold then
    encoderSum = 0
    self:sendUp("doMoveControlLeft", self.id)
  end
  return true
end

--------------------------------------------------
local Insert = Class {}
Insert:include(SpottedControl)

local savedListIndex = 0

function Insert:init(text)
  SpottedControl.init(self)
  self:setClassName("Unit.Editor.Insert")
  if text then
    local panel = app.TextPanel(text, 1)
    self:setControlGraphic(panel)
    panel:setBorderColor(app.GRAY5)
    panel:setForegroundColor(app.GRAY7)
    self:addSpotDescriptor{
      center = 0.5 * ply,
      radius = ply
    }
  else
    self:addSpotDescriptor{
      center = 0,
      radius = ply
    }
  end

  self.menuGraphic = app.Graphic(0, 0, 128, 64)

  local panel
  panel = app.TextPanel("Insert GainBias Control", 1)
  self.menuGraphic:addChild(panel)
  panel = app.TextPanel("Insert Gate Control", 2)
  self.menuGraphic:addChild(panel)
  panel = app.TextPanel("Insert Pitch Control", 3)
  self.menuGraphic:addChild(panel)
end

function Insert:onCursorEnter()
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased")
end

function Insert:onCursorLeave()
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased")
end

function Insert:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    self:sendUp("doInsertControl", "GainBias")
  elseif i == 2 then
    self:sendUp("doInsertControl", "Gate")
  elseif i == 3 then
    self:sendUp("doInsertControl", "Pitch")
  end
  return true
end

--------------------------------------------------
local Filler = Class {}
Filler:include(Section)

function Filler:init()
  Section.init(self, app.sectionEnd)
  self:setClassName("Editor.Filler")
  self:addView("default")
  self:addControl("default", Insert("End of Unit"))
  self:switchView("default")
end

--------------------------------------------------
local Item = Class {}
Item:include(Section)

function Item:init(control)
  Section.init(self, app.sectionNoBorder)
  self:setClassName("Unit.Editor.Item")
  self:addView("default")
  self:addControl("default", Insert())
  self:addControl("default", ItemHeader(control))
  self:switchView("default")
  self.id = control.id
end

--------------------------------------------------
local MenuHeader = Class {}
MenuHeader:include(SpottedControl)

function MenuHeader:init(title)
  SpottedControl.init(self)
  self:setClassName("Unit.Editor.MenuHeader")
  local panel = app.TextPanel(title, 1)
  panel:setBackgroundColor(app.GRAY2)
  panel:setOpaque(true)
  self:setControlGraphic(panel)
  self:addSpotDescriptor{
    center = 0.5 * ply,
    radius = ply
  }
  self.menuGraphic = app.Graphic(0, 0, 128, 64)
  self.menuGraphic:addChild(app.TextPanel("Remove All Edits", 1))
end

function MenuHeader:onCursorEnter()
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased")
end

function MenuHeader:onCursorLeave()
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased")
end

function MenuHeader:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    self:sendUp("doReset")
  elseif i == 2 then
  elseif i == 3 then
  end
  return true
end

--------------------------------------------------
local BeginMenu = Class {}
BeginMenu:include(Section)

function BeginMenu:init(title)
  Section.init(self, app.sectionBegin)
  self:setClassName("Unit.Editor.BeginMenu")
  self:addView("default")
  self:addControl("default", MenuHeader(title))
  self:switchView("default")
end

--------------------------------------------------
local Editor = Class {}
Editor:include(SpottedStrip)

function Editor:init(unit)
  SpottedStrip.init(self)
  self:setClassName("Unit.Editor")
  self:setInstanceName(unit.title)
  self.unit = unit
  self.restoreState = unit:serialize()
  self.changeCount = 0
  self.items = {}
  self:populate()
end

function Editor:populate()
  local unit = self.unit
  local view = unit:getView("expanded")
  if view then
    self:clear()
    self:appendSection(BeginMenu(unit.title))
    self:appendSection(Filler())
    for i, control in ipairs(view.controls) do
      if i > 2 then self:insert(control) end
    end
  end
end

function Editor:insert(control, position)
  if control.id == nil then app.logError("%s: no id", control) end
  local item = Item(control)
  local n = self:getSectionCount()
  self.items[item.id] = item
  -- after the BeginMenu but before the Filler
  if position == nil or position >= n then
    position = n
  else
    position = position + 1
  end
  self:insertSection(item, position)
end

function Editor:remove(item)
  if type(item) == "string" then item = self.items[item] end
  if item then
    self.items[item.id] = nil
    self:removeSection(item)
  end
end

function Editor:move(item, position)
  if position == self:getSectionCount() then
    self:moveSection(item, position)
  else
    self:moveSection(item, position + 1)
  end
end

function Editor:doInsertControl(type)
  local index = self:getSelectedSectionPosition() - 1
  local ControlEditor = require "Unit.ControlEditor"
  local editor = ControlEditor {
    unit = self.unit,
    type = type
  }
  local task = function(data)
    if data then
      self:onChange()
      local cb = self.unit:addControlBranch(data.type, data.id, data)
      self.unit:placeControl(data.id, "expanded", index)
      self:insert(cb.control, index)
    end
  end
  editor:subscribe("done", task)
  editor:show()
end

function Editor:doEditControl(id)
  local ControlEditor = require "Unit.ControlEditor"
  local control = self.unit.controls[id]
  local editor = ControlEditor {
    unit = self.unit,
    type = control.type,
    id = control.id,
    defaults = control:getDefaults(),
    customizations = control:getCustomizations(),
    isBuiltin = control.isBuiltin
  }
  local task = function(data)
    if data then
      self:onChange()
      local oldName = control:getCustomizableValue("name")
      control:deserialize(data.control)
      local newName = control:getCustomizableValue("name")
      if newName ~= oldName then
        self.unit:renameControl(oldName, newName)
        self.unit:renameControlBranch(oldName, newName)
      end
      self:populate()
    end
  end
  editor:subscribe("done", task)
  editor:show()
end

function Editor:doDeleteControl(id)
  local Verification = require "Verification"
  local control = self.unit.controls[id]
  local msg = string.format("Deleting %s.", control:getDisplayName())
  local dlg = Verification.Main(msg, "Are you sure?")
  local task = function(ok)
    if ok then
      self:onChange()
      self.unit:removeControlBranch(id)
      self:remove(id)
    end
  end
  dlg:subscribe("done", task)
  dlg:show()
end

function Editor:reset()
  local view = self.unit:getView("expanded")
  for i, control in ipairs(view.controls) do
    self:remove(control.id)
    if control.isBuiltin then
      if control.removeCustomizations then control:removeCustomizations() end
    else
      self.unit:removeControlBranch(control.id)
    end
  end
  self.unit:restoreView("expanded")
  self:populate()
end

function Editor:doReset()
  local Verification = require "Verification"
  local dlg = Verification.Main("Removing all customizations to this unit.",
                                "Are you sure?", 10)
  local task = function(ok)
    if ok then
      self:onChange()
      self:reset()
    end
  end
  dlg:subscribe("done", task)
  dlg:show()
end

function Editor:doMoveControlRight()
  local index = self:getSelectedSectionPosition() - 1
  if index == self:getSectionCount() - 2 then
    -- cannot move the last section right
    return
  end
  self:onChange()
  local control = self.unit:getControl("expanded", index)
  local item = self:getSelection()
  local newIndex = index + 1
  self:move(item, newIndex)
  self.unit:moveControl(control.id, "expanded", newIndex, index)
end

function Editor:doMoveControlLeft()
  local index = self:getSelectedSectionPosition() - 1
  if index == 1 then
    -- cannot move the first section left
    return
  end
  self:onChange()
  local control = self.unit:getControl("expanded", index)
  local item = self:getSelection()
  local newIndex = index - 1
  self:move(item, newIndex)
  self.unit:moveControl(control.id, "expanded", newIndex, index)
end

function Editor:onChange()
  self.changeCount = self.changeCount + 1
end

function Editor:upReleased(shifted)
  if shifted then return false end
  self:hide()
  return true
end

function Editor:enterReleased()
  self:hide()
  return true
end

function Editor:cancelReleased(shifted)
  if shifted then return false end
  if self.changeCount > 0 then
    local Verification = require "Verification"
    local dlg = Verification.Main("This will discard your edits.",
                                  "Are you sure?")
    local task = function(ok)
      if ok then
        self.unit:deserialize(self.restoreState)
        self:hide()
      end
    end
    dlg:subscribe("done", task)
    dlg:show()
  else
    self:hide()
  end
  return true
end

function Editor:homeReleased()
  self:hide()
  return true
end

return Editor
