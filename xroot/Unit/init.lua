local app = app
local Class = require "Base.Class"
local Base = require "Unit.Section"
local Branch = require "Chain.Branch"
local Persist = require "Persist"
local Keyboard = require "Keyboard"
local Utils = require "Utils"
local Settings = require "Settings"
local UnitFactory = require "Unit.Factory"
local ply = app.SECTION_PLY

-- Unit Class
local Unit = Class {}
Unit:include(Base)

function Unit:init(args)
  Base.init(self, args)
  self:setClassName("Unit")
  local title = args.title or app.logError("%s.init: title is missing.", self)
  self:setInstanceName(title)
  self:setInstanceKey(args.instanceKey or Persist.generateInstanceKey())
  local chain = args.chain or app.logError("%s.init: chain is missing.", self)
  local depth = args.depth or app.logError("%s.init: depth is missing.", self)
  local loadInfo = args.loadInfo or
                       app.logError("%s.init: loadInfo is missing.", self)
  local mnemonic = args.mnemonic or
                       app.logError("%s.init: mnemonic is missing.", self)
  local channelCount = args.channelCount or chain.channelCount
  self.suppressTitleGeneration = args.suppressTitleGeneration or false
  self.channelCount = channelCount
  self.loadInfo = loadInfo
  -- Started putting version numbers on units from v0.3.09
  self.version = args.version or 1
  self.mnemonic = mnemonic
  self.depth = depth
  self.title = title
  self.chain = chain
  self.stopCount = 1
  self.started = false
  self.branches = {}
  self.objects = {}

  local Source = require "Source.Internal"
  self.leftOutputSource = Source("local", self, 1)
  if channelCount > 1 then
    self.rightOutputSource = Source("local", self, 2)
  end

  self.aliases = args.aliases or {}
  self.pUnit = args.pUnit or app.Unit(title, channelCount)

  -- load and compile the graph
  self:onLoadGraph(channelCount)
  if not self.pUnit:compile() then
    app.logError("%s.init: Failed to compile the graph.", self)
  end

  -- Get the builtin controls and views from the unit implementation.
  local controls, viewDescriptors =
      self:onLoadViews(self.objects, self.branches)
  controls = controls or {}
  viewDescriptors = viewDescriptors or {
    expanded = {},
    collapsed = {}
  }
  -- register each control
  for id, control in pairs(controls) do
    self:addControl(id, control)
  end

  -- add scope contextual views (if any)
  local cviewDescriptors = {}
  if self.controls.scope == nil then
    local added = false
    for _, cname in ipairs(viewDescriptors["expanded"]) do
      if viewDescriptors[cname] == nil and cviewDescriptors[cname] == nil then
        cviewDescriptors[cname] = {
          "scope",
          cname
        }
        added = true
      end
    end
    if added then
      local OutputScope = require "Unit.ViewControl.OutputScope"
      local scope = OutputScope {
        monitor = self,
        width = 4 * ply
      }
      self:addControl("scope", scope)
    end
  end

  -- create each view and add its controls
  for viewName, descriptor in pairs(viewDescriptors) do
    self:parseViewDescriptor(viewName, descriptor)
  end

  -- process additional contextual views
  for viewName, descriptor in pairs(cviewDescriptors) do
    self:parseViewDescriptor(viewName, descriptor)
  end

  -- Mark default set of controls and branches as builtin.
  -- This distinguishes them from the ControlBranches added later.
  for _, control in pairs(self.controls) do
    control.isBuiltin = true
  end
  for _, branch in pairs(self.branches) do
    branch.isBuiltin = true
  end

  self:saveView("expanded")
end

function Unit:getRootChain()
  return self.chain:getRootChain()
end

function Unit:findUnitByTitle(title)
  for _, branch in pairs(self.branches) do
    local unit = branch:findUnitByTitle(title)
    if unit then
      return unit
    end
  end
end

function Unit:findByInstanceKey(key)
  if self:getInstanceKey() == key then
    return self
  end

  for _, branch in pairs(self.branches) do
    local o = branch:findByInstanceKey(key)
    if o then
      return o
    end
  end
end

function Unit:getOutputSource(i)
  if i == 1 or i == nil or self.channelCount == 1 then
    return self.leftOutputSource
  elseif i == 2 then
    return self.rightOutputSource
  end
end

function Unit:getOutputDisplayName(channel)
  return self.title
end

function Unit:setTitle(title)
  self.title = title
  self:broadcastDown("unitTitleChanged", title)
  for name, branch in pairs(self.branches) do
    branch:setTitle(title)
  end
  if self.leftOutputSource then
    self.leftOutputSource:onRename()
  end
  if self.rightOutputSource then
    self.rightOutputSource:onRename()
  end
end

--- Set the background graphic for the named view
-- @param name    The name of the view created by onLoadViews.
-- @param graphic The background graphic to use.
function Unit:setViewBackground(name, graphic)
  local view = self:getView(name)
  if view then
    -- Place the background graphic position after the unit header.
    graphic:setPosition(app.SECTION_PLY, 0)
    view:setBackground(graphic)
  else
    app.logError("Unit.setViewBackground: view '%s' does not exist.", name)
  end
end

-- syntax sugar for Unit definition
function Unit:addToMuteGroup(control)
  self.chain:addToMuteGroup(control)
end

function Unit:addObject(name, o)
  name = name or Utils.findNextUnusedKey(self.objects, "object")
  o:setName(name)
  self.pUnit:addObject(o)
  self.objects[name] = o
  return o
end

function Unit:addMonoBranch(name, inObject, inletName, outObject, outletName)
  name = name or Utils.findNextUnusedKey(self.branches, "branch")
  local branch = Branch {
    title = self.title,
    subTitle = name,
    depth = self.depth + 1,
    channelCount = 1,
    leftDestination = inObject:getInput(inletName),
    -- branch specific arguments
    leftOutObject = outObject,
    leftOutletName = outletName,
    unit = self
  }
  self.branches[name] = branch
  branch.name = name
  return branch
end

function Unit:addStereoBranch(name, leftInObject, leftInletName, rightInObject,
                              rightInletName, leftOutObject, leftOutletName,
                              rightOutObject, rightOutletName)
  name = name or Utils.findNextUnusedKey(self.branches, "branch")
  local branch = Branch {
    title = self.title,
    subTitle = name,
    depth = self.depth + 1,
    channelCount = 2,
    leftDestination = leftInObject:getInput(leftInletName),
    rightDestination = rightInObject:getInput(rightInletName),
    -- branch specific arguments
    leftOutObject = leftOutObject,
    leftOutletName = leftOutletName,
    rightOutObject = rightOutObject,
    rightOutletName = rightOutletName,
    unit = self
  }
  self.branches[name] = branch
  branch.name = name
  return branch
end

function Unit:addBranch(name, branch)
  self.branches[name] = branch
  branch.name = name
  branch:setTitle(self.title, name)
  branch:updateDepth(self.depth + 1)
  branch:setUnit(self)
end

function Unit:renameBranch(oldName, newName)
  local branch = self:getBranch(oldName)
  if branch then
    self.branches[branch.name] = nil
    self.branches[newName] = branch
    branch.name = newName
    branch:setTitle(self.title, newName)
  end
end

function Unit:removeBranch(name)
  local branch = self.branches[name]
  if branch then
    self.branches[name] = nil
  end
end

function Unit:getBranch(name)
  return self.branches[name]
end

function Unit:enable(soft)
  if not soft then
    self.disabled = false
  end
  if not self.disabled then
    self.pUnit:enable()
    self:notifyBranches("enable", true)
  end
end

function Unit:disable(soft)
  if not soft then
    self.disabled = true
  end
  self.pUnit:disable()
  self:notifyBranches("disable", true)
end

function Unit:start()
  if self.stopCount > 0 then
    self.stopCount = self.stopCount - 1
    if self.stopCount == 0 then
      self.started = true
      self:notifyBranches("start")
    end
  end
end

function Unit:stop()
  if self.stopCount == 0 then
    self.stopCount = 1
    self.started = false
    self:notifyBranches("stop")
  else
    self.stopCount = self.stopCount + 1
  end
end

function Unit:enableBypass()
  self.pUnit:setBypass(true)
  self.chain:rebuildGraph()
  if Settings.get("unitDisableOnBypass") == "yes" then
    self:disable()
  end
  self.controls.header:bypassEnabled()
end

function Unit:disableBypass()
  self:enable()
  self.pUnit:setBypass(false)
  self.chain:rebuildGraph()
  self.controls.header:bypassDisabled()
end

function Unit:toggleBypass()
  local chain = self.chain
  local wasMuted = chain:muteIfNeeded()
  if self.pUnit:getBypass() then
    self:disableBypass()
  else
    self:enableBypass()
  end
  chain:unmuteIfNeeded(wasMuted)
end

function Unit:getInwardConnection(ch)
  if self.pUnit == nil then
    app.logError("Unit.getInwardConnection: pUnit is nil.")
  end
  return self.pUnit:getInwardConnection(ch - 1)
end

function Unit:getOutput(ch)
  if self.pUnit == nil then
    app.logError("Unit.getOutput: pUnit is nil.")
  end
  if self.channelCount == 1 or ch == nil or ch == 1 then
    return self.pUnit:getOutput(0)
  elseif ch == 2 then
    return self.pUnit:getOutput(1)
  end
end

function Unit:getMonitoringOutput(ch)
  return self:getOutput(ch)
end

function Unit:serialize()
  local t = {
    loadInfo = UnitFactory.serializeLoadInfo(self.loadInfo),
    activeView = self.currentViewName,
    bypass = self.pUnit:getBypass(),
    lastPresetPath = self.lastPresetPath,
    lastPresetFilename = self.lastPresetFilename,
    unitVersion = self.version,
    firmwareVersion = app.FIRMWARE_VERSION,
    instanceKey = self:getInstanceKey()
  }

  if self.hasUserTitle then
    t.hasUserTitle = true
    t.title = self.title
  end

  -- walk each control
  local controls = {}
  for cname, control in pairs(self.controls) do
    if control.isBuiltin and control.serialize then
      local tmp = control:serialize()
      if next(tmp) ~= nil then
        controls[cname] = tmp
      end
    end
  end
  t.controls = controls

  -- Make sure the menu generation code was run at least once.
  if not self.menuLoadedAtLeastOnce then
    self:showMenu(true)
  end

  -- walk each object
  t.objects = Persist.serializeObjects(self.objects)

  -- walk each Branch
  local branches = {}
  for bname, branch in pairs(self.branches) do
    if branch.isBuiltin and branch:isSerializationNeeded() then
      branches[bname] = branch:serialize()
    end
  end
  t.branches = branches

  -- walk each ControlBranch
  local controlBranches = {}
  for bname, branch in pairs(self.controlBranches) do
    controlBranches[bname] = branch:serialize()
  end
  t.controlBranches = controlBranches

  -- walk the expanded view and save the order
  t.controlOrder = self:getControlOrder("expanded")

  return t
end

function Unit:deserialize(t)
  self:removeAllControlBranches()

  if t.hasUserTitle and t.title then
    self:setTitle(t.title)
    self.hasUserTitle = true
  end

  self:setLastPreset(t.lastPresetPath, t.lastPresetFilename)
  if t.instanceKey then
    self:setInstanceKey(t.instanceKey)
  end

  if t.objects then
    Persist.deserializeObjects(self.objects, self.aliases, t.objects)
  else
    app.logInfo("%s:deserialize: no 'objects' table.", self)
  end

  -- walk each control in the expanded view
  if t.controls then
    -- deserialize each control (if it exists)
    local controls = self.controls
    for cname, cdata in pairs(t.controls) do
      local control = controls[cname]
      if control and control.isBuiltin and control.deserialize then
        control:deserialize(cdata)
      end
    end
  end

  -- load branches
  if t.branches then
    local branches = self.branches
    for bname, bdata in pairs(t.branches) do
      local branch = branches[bname]
      if branch then
        branch:deserialize(bdata)
      end
    end
  else
    app.logInfo("%s:deserialize: no 'branches' table.", self)
  end

  -- load control branches
  if t.controlBranches then
    for _, bdata in pairs(t.controlBranches) do
      if bdata.type and bdata.id then
        self:addControlBranch(bdata.type, bdata.id, bdata)
        self:placeControl(bdata.id, "expanded")
      end
    end
  end

  -- restore control order in each view
  if t.controlOrder then
    self:applyControlOrder("expanded", t.controlOrder)
  end

  -- tell all controls to update
  self:notifyControls("update")

  if t.bypass then
    self:enableBypass()
  end

  -- activate the saved view
  if t.activeView then
    self:switchView(t.activeView)
  end
end

function Unit:doRename()
  local keyboard = Keyboard("Rename unit", self.title, true, "NamingStuff")
  local task = function(text)
    if text then
      self:setTitle(text)
      self.hasUserTitle = true
    end
  end
  keyboard:subscribe("done", task)
  keyboard:show()
end

function Unit:notifyBranches(method, ...)
  for name, branch in pairs(self.branches) do
    local f = branch[method]
    if f ~= nil then
      -- app.logInfo("%s:notifyBranches > %s:%s",self,branch,method)
      f(branch, ...)
    end
  end
end

function Unit:notifyAll(method, ...)
  self:notifyControls(method, ...)
  for name, branch in pairs(self.branches) do
    local f = branch[method]
    if f ~= nil then
      -- app.logInfo("%s:notifyAll > %s:%s",self,branch,method)
      f(branch, ...)
    end
    branch:notifyAll(method, ...)
  end
end

function Unit:setLastPreset(path, filename)
  self.lastPresetPath = path
  self.lastPresetFilename = filename
end

function Unit:getLastPreset()
  return self.lastPresetPath, self.lastPresetFilename
end

local defaultMenuItems = {
  "infoHeader",
  "renameUnit",
  "loadPreset",
  "savePreset",
  "editControls"
}
local isDefaultMenuItem = {}
for i, cname in ipairs(defaultMenuItems) do
  isDefaultMenuItem[cname] = true
end

function Unit:addDefaultMenuItems(order, controls)
  -- create default controls if needed
  local Task = require "Unit.MenuControl.Task"
  local MenuHeader = require "Unit.MenuControl.Header"

  if controls.infoHeader == nil then
    controls.infoHeader = MenuHeader {
      description = string.format("%s  (Last Preset: %s)", self.title,
                                  self.lastPresetFilename)
    }
  end
  if controls.loadPreset == nil then
    controls.loadPreset = Task {
      description = "Load Preset",
      task = function()
        Persist.loadUnitPreset(self)
      end
    }
  end
  if controls.savePreset == nil then
    controls.savePreset = Task {
      description = "Save Preset",
      task = function()
        Persist.saveUnitPreset(self)
      end
    }
  end
  if controls.renameUnit == nil then
    controls.renameUnit = Task {
      description = "Rename Unit",
      task = function()
        self:doRename()
      end
    }
  end
  if controls.editControls == nil then
    controls.editControls = Task {
      description = "Edit Controls",
      task = function()
        self:doEditControls()
      end
    }
  end

  -- remove default items from order array
  Utils.removeValuesFromArray(order, isDefaultMenuItem)

  -- append the default items to the end
  for _, cname in ipairs(defaultMenuItems) do
    order[#order + 1] = cname
  end
end

function Unit:showMenu(justLoad)
  local controls, order, sub = self:onShowMenu(self.objects, self.branches)
  self.menuLoadedAtLeastOnce = true

  if justLoad then
    return
  end

  order = order or {}
  controls = controls or {}
  self:addDefaultMenuItems(order, controls)

  local Menu = require "Unit.Menu"
  local menu = Menu(self, sub)

  for _, cname in ipairs(order) do
    if cname == "" or cname == "newRow" then
      menu:startNewRow()
    else
      local control = controls[cname]
      if control then
        menu:addControl(control, control.isHeader)
      end
    end
  end

  menu:show()
end

function Unit:getPresetVersion(t)
  local version = t.unitVersion
  if type(version) == "number" then
    return version
  else
    return 0
  end
end

-----------------------------------------------
-- custom handlers

function Unit:onLoadGraph(channelCount)
end

function Unit:onLoadViews(objects, branches)
end

function Unit:onShowMenu(objects, branches)
end

function Unit:onLoadFinished()
end
-----------------------------------------------

function Unit:onGenerateTitle()
  if self.suppressTitleGeneration then
    return
  end

  local i = 1
  local title = self.title or "Unknown"
  local candidateName = title .. string.format(" #%d", i)
  local root = self:getRootChain()

  while root:findUnitByTitle(candidateName) do
    i = i + 1
    candidateName = title .. string.format(" #%d", i)
  end

  self:setTitle(candidateName)
end

function Unit:onRemove()
  if self.leftOutputSource then
    self.leftOutputSource:onDelete()
  end
  if self.rightOutputSource then
    self.rightOutputSource:onDelete()
  end

  self.pUnit:disable()
  -- self.pUnit:disconnectAllObjects()

  for _, control in pairs(self.controls) do
    self.chain:removeFromMuteGroup(control)
  end

  -- stop branches
  for name, branch in pairs(self.branches) do
    branch:stop()
    branch:releaseResources()
    branch:hide()
    -- prevent stop from being called again
    self.branches[name] = nil
  end

  Base.onRemove(self) -- this  will notify all controls
end

return Unit
