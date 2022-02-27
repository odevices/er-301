local Class = require "Base.Class"
local BaseSection = require "SpottedStrip.Section"
local Header = require "Unit.ViewControl.Header"
local InsertControl = require "Chain.InsertControl"

local UnitSection = Class {}
UnitSection:include(BaseSection)

function UnitSection:init(args)
  BaseSection.init(self, app.sectionMiddle, true)
  self.controlBranches = {}
  local title = args.title or app.logError("%s.init: title is missing.", self)
  self:addControl("insert", InsertControl())
  self:addControl("header", Header {
    title = title
  })
end

function UnitSection:parseViewDescriptor(viewName, descriptor)
  local controls = self.controls
  local view = self:addView(viewName)
  local vc = view.controls
  vc[1] = controls.insert
  vc[2] = controls.header
  for _, controlName in ipairs(descriptor) do
    local control = controls[controlName]
    if control then
      vc[#vc + 1] = control
    end
  end
  if #vc > 2 then
    -- add a divider after the header
    view:addDivider(app.SECTION_PLY)
  end
end

function UnitSection:leftJustify()
  -- spot 1 is the header spot
  self.pDisplay:leftJustify(self.pSection, 1)
end

function UnitSection:collapse()
  if self.currentViewName ~= "collapsed" then
    self:switchView("collapsed")
  end
end

function UnitSection:expand()
  if self.currentViewName ~= "expanded" then
    self:switchView("expanded")
  end
end

function UnitSection:isCollapsed()
  return self.currentViewName == "collapsed"
end

function UnitSection:doEditControls()
  local Editor = require "Unit.Editor"
  local editor = Editor(self)
  editor:show()
end

function UnitSection:doEditControl(control)
  local ControlEditor = require "Unit.ControlEditor"
  local editor = ControlEditor {
    unit = self,
    type = control.type,
    id = control.id,
    defaults = control:getDefaults(),
    customizations = control:getCustomizations(),
    isBuiltin = control.isBuiltin
  }
  local task = function(data)
    if data then
      local oldName = control:getCustomizableValue("name")
      control:deserialize(data.control)
      local newName = control:getCustomizableValue("name")
      if newName ~= oldName then
        self:renameControl(oldName, newName)
        self:renameControlBranch(oldName, newName)
      end
    end
  end
  editor:subscribe("done", task)
  editor:show()
end

function UnitSection:addControl(id, control)
  local controls = self.controls
  if controls[id] == nil then
    control.id = id
    controls[id] = control
    self:addChildWidget(control)
    control:onInsert()
  end
end

function UnitSection:placeControl(id, viewName, position)
  local control = self.controls[id]
  local view = self:addView(viewName)
  if control and view then
    if position == nil then
      -- append to the end
      view.controls[#view.controls + 1] = control
    else
      -- +2 to skip the insert and header controls
      if position + 2 > #view.controls then
        view.controls[#view.controls + 1] = control
      else
        table.insert(view.controls, position + 2, control)
      end
    end
    if self.currentViewName == viewName then
      local Application = require "Application"
      Application.postTrigger(self, "rebuildView")
    end
  end
end

function UnitSection:getControlOrder(viewName)
  local order = {}
  local view = self:getView(viewName)
  if view then
    for i, control in ipairs(view.controls) do
      if i > 2 then
        -- skip the insert and header controls
        order[#order + 1] = control.id
      end
    end
  end
  return order
end

function UnitSection:applyControlOrder(viewName, order)
  for i, id in ipairs(order) do
    self:moveControl(id, viewName, i)
  end
end

function UnitSection:saveView(name)
  local Utils = require "Utils"
  local view = self:getView(name)
  view.savedControls = Utils.shallowCopy(view.controls)
end

function UnitSection:restoreView(name)
  local view = self:getView(name)
  if view.savedControls then
    view.controls = view.savedControls
    view.savedControls = nil
    if self.currentViewName == name then
      local Application = require "Application"
      Application.postTrigger(self, "rebuildView")
    end
  end
end

-- currentPosition is optional
function UnitSection:moveControl(id, viewName, newPosition, currentPosition)
  -- app.logInfo("%s:moveControl(%s,%d,%d)",self,id,newPosition,currentPosition)
  local control = self.controls[id]
  if control then
    local view = self:getView(viewName)
    local controls = view.controls
    if currentPosition == nil then
      -- must find it
      for i, c in ipairs(controls) do
        if c == control then
          currentPosition = i - 2
        end
      end
    end
    -- +2 to skip the insert and header controls
    if currentPosition then
      table.remove(controls, currentPosition + 2)
    end
    if newPosition + 2 > #controls then
      controls[#controls + 1] = control
    else
      table.insert(controls, newPosition + 2, control)
    end
    if self.currentViewName == viewName then
      local Application = require "Application"
      Application.postTrigger(self, "rebuildView")
    end
  end
end

function UnitSection:renameControl(oldName, newName)
  local controls = self.controls
  local control = controls[oldName]
  if control then
    controls[oldName] = nil
    controls[newName] = control
    control:rename(newName)
    control.id = newName
  end
end

function UnitSection:getControlByName(id)
  return self.controls[id]
end

function UnitSection:removeControl(id)
  local controls = self.controls
  local control = controls[id]
  if control then
    controls[id] = nil
    -- remove the contextual view
    if self.currentViewName == id then
      self:switchView("expanded")
    end
    self:removeView(id)
    -- remove from all views
    local rebuildNeeded = false
    for viewName, view in pairs(self.views) do
      for i, control2 in ipairs(view.controls) do
        if control2 == control then
          table.remove(view.controls, i)
          if viewName == self.currentViewName then
            rebuildNeeded = true
          end
          break
        end
      end
    end
    if rebuildNeeded then
      local Application = require "Application"
      Application.postTrigger(self, "rebuildView")
    end
    control:onRemove()
    self:removeChildWidget(control)
  end
end

function UnitSection:notifyControls(method, ...)
  -- notify all the controls
  for _, control in pairs(self.controls) do
    local f = control[method]
    if f ~= nil then
      f(control, ...)
    end
  end
end

function UnitSection:getControl(viewName, index)
  local view = self:getView(viewName)
  -- +2 for insert and header controls
  return view and view.controls[index + 2]
end

function UnitSection:getControlRaw(viewName, index)
  local view = self:getView(viewName)
  return view and view.controls[index]
end

function UnitSection:getControlPositionInView(viewName, control)
  local view = self:getView(viewName)
  if view then
    -- app.logInfo("%s:getControlPositionInView(%s,%s)",self,viewName,control)
    for i, control2 in ipairs(view.controls) do
      -- app.logInfo("%s:getControlPositionInView: %s-->%s",self,i,control2)
      if control == control2 then
        return i
      end
    end
  end
end

function UnitSection:generateUniqueControlName(prefix)
  local suffix = 1
  local candidate
  local exists = true
  while exists do
    candidate = prefix .. tostring(suffix)
    exists = false
    for name, control in pairs(self.controls) do
      if candidate == name then
        exists = true
        suffix = suffix + 1
        break
      end
    end
  end
  return candidate
end

function UnitSection:validateControlName(candidate)
  if candidate == "" then
    return false, "Blank names are not allowed."
  end
  for id, control in pairs(self.controls) do
    if candidate == id then
      return false, candidate .. " already exists."
    end
  end
  return true
end

function UnitSection:addControlBranch(type, id, data)
  if self.controlBranches[id] then
    self:removeControlBranch(id)
  end

  local BranchClass = require("Unit.ControlBranch." .. type)
  local controlBranch = BranchClass {
    id = id,
    depth = self.depth + 1,
    title = self.title
  }
  self.controlBranches[id] = controlBranch

  if data then
    controlBranch:deserialize(data)
  end

  self:addControl(id, controlBranch.control)
  self:addBranch(id, controlBranch)
  if self.started then
    controlBranch:start()
  end

  return controlBranch
end

function UnitSection:renameControlBranch(oldName, newName)
  local controlBranches = self.controlBranches
  local branch = controlBranches[oldName]
  if branch then
    controlBranches[oldName] = nil
    controlBranches[newName] = branch
    branch:rename(newName)
  end
end

function UnitSection:removeControlBranch(id)
  local branch = self.controlBranches[id]
  if branch then
    branch:releaseResources()
    self:removeControl(id)
    self:removeBranch(branch)
    self.controlBranches[id] = nil
  end
end

function UnitSection:removeAllControlBranches()
  for id, branch in pairs(self.controlBranches) do
    self:removeControlBranch(id)
  end
end

function UnitSection:homeReleased()
  self.parent:setSelection(self, self.currentViewName, 1)
  return true
end

function UnitSection:onCursorEnter(view, spot)
  self:grabFocus("shiftReleased", "cancelReleased")
  BaseSection.onCursorEnter(self, view, spot)
end

function UnitSection:onCursorLeave(view, spot)
  self:releaseFocus("shiftReleased", "cancelReleased")
  BaseSection.onCursorLeave(self, view, spot)
end

return UnitSection
