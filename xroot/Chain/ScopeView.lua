local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Window = require "Base.Window"
local Encoder = require "Encoder"
local Node = require "Chain.Node"

local ScopeView = Class {}
ScopeView:include(Window)

function ScopeView:init(chain)
  self.ptr = app.ChainOverview(0, 0, 256, 64)
  self.scope = app.MiniScope(0, 0, 128, 64)
  Window.init(self, self.ptr, self.scope)
  self:setClassName("Chain.ScopeView")
  self.chain = chain
  self.nodes = {}
  self:setMainCursorController(self.ptr)
  self.refreshNeeded = true
  self.encoderState = Encoder.Fine
  self.ptr:setDepthFirstNavigation(true)
  self.ptr:setEmptyString(chain.title .. ": No units.")
end

function ScopeView:getXPath()
  local xpath = app.XPath()
  self.ptr:fillXPath(xpath, self.ptr:selected())
  return xpath
end

function ScopeView:select(xpath)
  if self.refreshNeeded then
    self:refresh()
  end
  self.ptr:select(xpath)
  self:onSelectionChanged()
end

function ScopeView:loadUnitHelper(unit)
  -- Traverse each control in the scope or expanded view
  -- app.logInfo("%s:loadUnitHelper(%s)",self,unit)
  local view = unit:getView("scope") or unit:getView("expanded")
  if view == nil then
    return
  end
  local overview = self.ptr
  for i, control in ipairs(view.controls) do
    if control.getPatch then
      local patch = control:getPatch()
      if patch then
        local name = control:getInstanceName() or patch.name
        local id = overview:startPatch(name, i)
        self.nodes[id] = Node(id, name, patch, "Patch")
        self:loadChainHelper(patch)
        overview:endPatch()
      end
    elseif control.getBranch then
      local branch = control:getBranch()
      if branch then
        local name = control:getInstanceName() or branch.name
        local id = overview:startBranch(name, i)
        self.nodes[id] = Node(id, name, branch, "Branch")
        self:loadChainHelper(branch)
        overview:endBranch()
      end
    end
  end
end

function ScopeView:loadChainHelper(chain)
  -- Depth-first traversal of chain and its descendants
  -- app.logInfo("%s:loadChainHelper(%s)",self,chain)
  local overview = self.ptr
  -- add sources if any
  if chain.getInputSource then
    local leftSource = chain:getInputSource(1)
    local rightSource = chain.channelCount > 1 and chain:getInputSource(2)
    if leftSource and rightSource then
      local name = leftSource:getDisplayName() .. "+" ..
                       rightSource:getDisplayName()
      local id = overview:addSource(name, 0)
      self.nodes[id] = Node(id, name, {
        leftSource,
        rightSource
      }, "StereoSource")
    elseif leftSource then
      local name = leftSource:getDisplayName()
      local id = overview:addSource(name, 0)
      self.nodes[id] = Node(id, name, leftSource, "MonoSource")
    elseif rightSource then
      local name = rightSource:getDisplayName()
      local id = overview:addSource(name, 0)
      self.nodes[id] = Node(id, name, rightSource, "MonoSource")
    end
  end
  -- add units if any
  for i = 1, chain:length() do
    local unit = chain:getUnit(i)
    local id = overview:startUnit(unit.mnemonic, unit.title, i)
    self.nodes[id] = Node(id, unit.title, unit, "Unit")
    self:loadUnitHelper(unit)
    overview:endUnit()
  end
end

function ScopeView:refresh()
  local overview = self.ptr
  self.nodes = {}
  overview:clear()
  self:loadChainHelper(self.chain)
  overview:rebuild()
  self.refreshNeeded = false
end

local threshold = Env.EncoderThreshold.Default
function ScopeView:encoder(change, shifted)
  if self.ptr:encoder(change, shifted, threshold) then
    self:onSelectionChanged()
  end
end

function ScopeView:mainReleased(i, shifted)
  if shifted then
    return true
  end
  self.ptr:selectColumn(i - 1, shifted)
  return true
end

function ScopeView:enterReleased()
  if self.ptr:down() then
    self:onSelectionChanged()
  end
  return true
end

function ScopeView:upReleased(shifted)
  if self.ptr:up() then
    self:onSelectionChanged()
  end
  return true
end

function ScopeView:dialPressed(shifted)
  if self.encoderState == Encoder.Coarse then
    self.encoderState = Encoder.Fine
    self.ptr:setDepthFirstNavigation(true)
  else
    self.encoderState = Encoder.Coarse
    self.ptr:setDepthFirstNavigation(false)
  end
  Encoder.set(self.encoderState)
  return true
end

function ScopeView:selectReleased(i, shifted)
  self:onSelectionChanged()
  return true
end

function ScopeView:onSelectionChanged()
  local Channels = require "Channels"
  local side = Channels.getSide()
  local id = self.ptr:selected()
  local node = self.nodes[id]
  if node then
    -- app.logInfo("%s",node)
    if node.type == "StereoSource" then
      self.scope:watchOutlet(node.o[side]:getOutlet())
    elseif node.type == "MonoSource" then
      self.scope:watchOutlet(node.o:getOutlet())
    elseif node.type == "Unit" then
      self.scope:watchOutlet(node.o:getOutput(side))
    elseif node.type == "Branch" then
      self.scope:watchOutlet(node.o:getOutput(side))
    elseif node.type == "Patch" then
      self.scope:watchOutlet(node.o:getMonitoringOutput(side))
    end
  end
end

function ScopeView:onShow()
  if self.refreshNeeded then
    self:refresh()
  end
  self.initialSelectionId = self.ptr:selected()
  Encoder.set(self.encoderState)
end

function ScopeView:onHide()
  self.nodes = {}
  self.ptr:clear()
  self.refreshNeeded = true
  Encoder.set(Encoder.Neutral)
end

function ScopeView:selectionChanged()
  return self.ptr:selected() ~= self.initialSelectionId
end

function ScopeView:releaseResources()
  self.nodes = {}
  self.ptr:clear()
end

return ScopeView
