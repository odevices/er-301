local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Window = require "Base.Window"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

local function createNode(o,type)
  return {
    o = o,
    type = type
  }
end

local LocalChooser = Class{}
LocalChooser:include(Window)

function LocalChooser:init(ring,chain,currentSource)
  Window.init(self)
  self:setClassName("Source.LocalChooser")
  self.ring = ring
  chain = chain:getRootChain()
  self.chain = chain
  self.nodes = {}

  local overview = app.ChainOverview(0,0,256-ply,64)
  self.ptr = overview
  self:addMainGraphic(overview)
  self:setMainCursorController(overview)
  self.encoderState = Encoder.Fine
  overview:setDepthFirstNavigation(true)
  self.scope = app.MiniScope(256-ply,0,ply,64)
  --self.scope:setBorder(1)
  --self.scope:setCornerRadius(3,3,3,3)
  self.scope:setOpaque(true)
  self.scope:showStatus(true)
  self:addMainGraphic(self.scope)
  self:loadChainHelper(self.chain)
  overview:rebuild()
  if currentSource and currentSource.type=="local" then
    local id = self:findLocalSource(currentSource)
    if id then
      overview:select(id)
    end
  else
    local xpath = chain:getXPathToSelection()
    overview:select(xpath)
  end
  self:onSelectionChanged()
end

function LocalChooser:findLocalSource(src)
  local o = src.object
  for id,node in pairs(self.nodes) do
    if node.o==o then
      return id
    end
  end
end

function LocalChooser:getXPath()
  local xpath = app.XPath()
  self.ptr:fillXPath(xpath,self.ptr:selected())
  return xpath
end

function LocalChooser:loadUnitHelper(unit)
  -- Traverse each control in the expanded view
  --app.logInfo("%s:loadUnitHelper(%s)",self,unit)
  local view = unit:getView("expanded")
  if view==nil then return end
  local overview = self.ptr
  for i,control in ipairs(view.controls) do
    if control.getPatch then
      local patch = control:getPatch()
      if patch then
        local name = control:getInstanceName() or patch.name
        local id = overview:startPatch(name, i)
        self.nodes[id] = createNode(patch,"Patch")
        self:loadChainHelper(patch)
        overview:endPatch()
      end
    elseif control.getBranch then
      local branch = control:getBranch()
      if branch then
        local name = control:getInstanceName() or branch.name
        local id = overview:startBranch(name, i)
        self.nodes[id] = createNode(branch,"Branch")
        self:loadChainHelper(branch)
        overview:endBranch()
      end
    end
  end
end

function LocalChooser:loadChainHelper(chain)
  -- Depth-first traversal of chain and its descendants
  --app.logInfo("%s:loadChainHelper(%s)",self,chain)
  local overview = self.ptr
  -- add sources if any
  if chain.getInputSource then
    local leftSource = chain:getInputSource(1)
    local rightSource = chain.channelCount > 1 and chain:getInputSource(2)
    if leftSource and rightSource then
      local name = leftSource:getDisplayName().."+"..rightSource:getDisplayName()
      local id = overview:addSource(name,0)
      self.nodes[id] = createNode({leftSource,rightSource},"StereoSource")
    elseif leftSource then
      local name = leftSource:getDisplayName()
      local id = overview:addSource(name,0)
      self.nodes[id] = createNode(leftSource,"MonoSource")
    elseif rightSource then
      local name = rightSource:getDisplayName()
      local id = overview:addSource(name,0)
      self.nodes[id] = createNode(rightSource,"MonoSource")
    end
  end
  -- add units if any
  for i=1,chain:length() do
    local unit = chain:getUnit(i)
    local id = overview:startUnit(unit.mnemonic,unit.title,i)
    self.nodes[id] = createNode(unit,"Unit")
    self:loadUnitHelper(unit)
    overview:endUnit()
  end
end

local threshold = Env.EncoderThreshold.Default
function LocalChooser:encoder(change,shifted)
  if self.ptr:encoder(change,shifted,threshold) then
    self:onSelectionChanged()
  end
end

function LocalChooser:enterReleased()
  local id = self.ptr:selected()
  local node = self.nodes[id]
  if node then
    local Channels = require "Channels"
    local side = Channels.getSide()
    if node.type=="StereoSource" then
      self:choose(node.o[side])
    elseif node.type=="MonoSource" then
      self:choose(node.o)
    else
      local src = node.o:getOutputSource(side)
      self:choose(src)
    end
  end
  return true
end

function LocalChooser:upReleased(shifted)
  if shifted then return true end
  if self.ptr:up() then
    self:onSelectionChanged()
  else
    self:cancelReleased(false)
  end
  return true
end

function LocalChooser:dialPressed(shifted)
  if shifted then return true end
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

function LocalChooser:selectReleased(i,shifted)
  self:onSelectionChanged()
  return true
end

function LocalChooser:onSelectionChanged()
  local Channels = require "Channels"
  local side = Channels.getSide()
  local id = self.ptr:selected()
  local node = self.nodes[id]
  if node then
    --app.logInfo("%s",node)
    if node.type=="StereoSource" then
      self.scope:watchOutlet(node.o[side]:getOutlet())
    elseif node.type=="MonoSource" then
      self.scope:watchOutlet(node.o:getOutlet())
    elseif node.type=="Unit" then
      self.scope:watchOutlet(node.o:getOutput(side))
    elseif node.type=="Branch" then
      self.scope:watchOutlet(node.o:getMonitoringOutput(side))
    elseif node.type=="Patch" then
      self.scope:watchOutlet(node.o:getMonitoringOutput(side))
    end
  end
end

function LocalChooser:mainReleased(i,shifted)
  if shifted or i==6 then return true end
  self.ptr:selectColumn(i-1,shifted)
  return true
end

function LocalChooser:onShow()
  Encoder.set(self.encoderState)
end

function LocalChooser:onHide()
  Encoder.set(Encoder.Neutral)
end

function LocalChooser:choose(src)
  return self.ring:choose(src)
end

function LocalChooser:homeReleased()
  return self.ring:homeReleased()
end

function LocalChooser:subReleased(i,shifted)
  return self.ring:subReleased(i,shifted)
end

function LocalChooser:cancelReleased(shifted)
  return self.ring:cancelReleased(shifted)
end


return LocalChooser
