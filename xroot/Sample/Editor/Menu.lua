local app = app
local Env = require "Env"
local Class = require "Base.Class"
local MondrianMenu = require "MondrianMenu.AsWindow"
local MondrianControl = require "MondrianMenu.Control"
local Overlay = require "Overlay"
local ply = app.SECTION_PLY

local function confirm(o, method, msg)
  return function()
    local Verification = require "Verification"
    local Settings = require "Settings"
    if Settings.get("confirmSampleEdit") == "yes" then
      local dlg = Verification.Main(msg, "Are you sure?")
      local task = function(ans)
        if ans then o[method](o) end
      end
      dlg:subscribe("done", task)
      dlg:show()
    else
      o[method](o)
    end
  end
end

------------------------------------

local Control = Class {}
Control:include(MondrianControl)

function Control:init(title, task)
  MondrianControl.init(self)
  self:setClassName("Menu.Control")
  local graphic = app.FittedTextBox(title)
  self:setControlGraphic(graphic)
  self.task = task
end

function Control:disable(msg)
  self.disabledMsg = msg
  MondrianControl.disable(self)
end

function Control:onReleased()
  self:callUp("hide")
  self.task()
  return true
end

function Control:onReleasedWhenDisabled()
  if self.disabledMsg then Overlay.mainFlashMessage(self.disabledMsg) end
  return true
end

------------------------------------

local Menu = Class {}
Menu:include(MondrianMenu)

function Menu:init(editor)
  MondrianMenu.init(self)
  self:setClassName("Sample.Editor.Menu")

  if editor.sample == nil then
    self:addHeaderText("No sample assigned.")
    return
  end

  local hasSelection = editor:hasSelection()
  if hasSelection then
    local Utils = require "Utils"
    local duration = Utils.duration(editor:getSelectionDuration())
    local from = Utils.timestamp(editor:getSelectionBeginTime())
    local to = Utils.timestamp(editor:getSelectionEndTime())
    self:addHeaderText("Operating on %s from %s to %s.", duration, from, to)
  else
    self:addHeaderText("Operating on entire sample.")
  end

  local trim = self:add("Trim",
                        confirm(editor, "doTrim", "Trimming to selection..."))
  local cut = self:add("Cut",
                       confirm(editor, "doCut", "Cutting out selection..."))

  if hasSelection then
    self:add("Silence", confirm(editor, "doSilence", "Silencing selection..."))
    self:add("Fade In",
             confirm(editor, "doFadeIn", "Applying fade to selection..."))
    self:add("Fade Out",
             confirm(editor, "doFadeOut", "Applying fade to selection..."))
    self:add("Normalize",
             confirm(editor, "doNormalize", "Normalizing selection..."))
    self:add("Remove DC",
             confirm(editor, "doRemoveDC", "Removing DC from selection..."))
  else
    trim:disable("First select a region with SHIFT+KNOB.")
    cut:disable("First select a region with SHIFT+KNOB.")
    self:add("Silence",
             confirm(editor, "doSilence", "Silencing entire sample..."))
    self:add("Fade In",
             confirm(editor, "doFadeIn", "Applying fade to entire sample..."))
    self:add("Fade Out",
             confirm(editor, "doFadeOut", "Applying fade to entire sample..."))
    self:add("Normalize",
             confirm(editor, "doNormalize", "Normalizing entire sample..."))
    self:add("Remove DC",
             confirm(editor, "doRemoveDC", "Removing DC from entire sample..."))
  end

  self:addHeaderText("File Operations:")
  local save = self:add("Save", function()
    editor:doSave()
  end)
  local saveAs = self:add("Save As", function()
    editor:doSaveAs()
  end)
  local revert = self:add("Revert", function()
    editor:doRevert()
  end)

  if not editor.sample:isDirty() then
    save:disable("No changes to save.")
    saveAs:disable("No changes to save.")
    revert:disable("No changes to revert.")
  end
end

function Menu:add(text, task)
  local control = Control(text, task)
  self:addControl(control)
  return control
end

return Menu
