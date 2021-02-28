local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Signal = require "Signal"
local SpottedStrip = require "SpottedStrip"
local Section = require "SpottedStrip.Section"
local SpottedControl = require "SpottedStrip.Control"
local GlobalChains = require "GlobalChains"
local Keyboard = require "Keyboard"
local ply = app.SECTION_PLY

local function nameValidator(text)
  if GlobalChains.exists(text) then
    return false, "Name already exists."
  elseif text == "" then
    return false, "Name is blank."
  else
    return true
  end
end

local function newChain(numChannels)
  local task = function(text)
    if text then GlobalChains.create(text, numChannels) end
  end
  local kb = Keyboard("Name this chain.", nil, true, "NamingStuff")
  kb:setValidator(nameValidator)
  kb:subscribe("done", task)
  kb:show()
end

local function loadChain()

end

local function renameChain(chain)
  local task = function(text)
    if text then GlobalChains.rename(chain.title, text) end
  end
  local kb = Keyboard("Rename this chain.", chain.title, true, "NamingStuff")
  kb:setValidator(nameValidator)
  kb:subscribe("done", task)
  kb:show()
end

local function deleteChain(chain)
  local Settings = require "Settings"
  if Settings.get("confirmGlobalChainDelete") == "yes" then
    local Verification = require "Verification"
    local dialog =
        Verification.Main("Deleting " .. chain.title, "Are you sure?")
    dialog:subscribe("done", function(ans)
      if ans then GlobalChains.delete(chain.title) end
    end)
    dialog:show()
  else
    GlobalChains.delete(chain.title)
  end
end

local function deleteAllChains()
  local Settings = require "Settings"
  if Settings.get("confirmGlobalChainDelete") == "yes" then
    local Verification = require "Verification"
    local dialog = Verification.Main("Deleting All Chains!", "Are you sure?")
    dialog:subscribe("done", function(ans)
      if ans then GlobalChains.deleteAll() end
    end)
    dialog:show()
  else
    GlobalChains.deleteAll()
  end
end

--------------------------------------------------
local Header = Class {}
Header:include(SpottedControl)

function Header:init(title, chain)
  SpottedControl.init(self)
  self:setClassName("Header")
  if chain.channelCount == 1 then
    local source = chain:getOutputSource(1)
    local scope = app.MiniScope(0, 0, ply, 64)
    scope:setForegroundColor(app.GRAY5)
    scope:watchOutlet(source:getOutlet())
    local panel = app.TextPanel(title, 1, 0.6)
    scope:addChild(panel)
    self:setControlGraphic(scope)
    self:addSpotDescriptor{
      center = 0.5 * ply,
      radius = ply
    }
    self.panel = panel
  elseif chain.channelCount > 1 then
    local graphic = app.Graphic(0, 0, 2 * ply, 64)
    local source1 = chain:getOutputSource(1)
    local source2 = chain:getOutputSource(2)
    local scope1 = app.MiniScope(0, 0, ply, 64)
    scope1:setForegroundColor(app.GRAY5)
    scope1:watchOutlet(source1:getOutlet())
    local scope2 = app.MiniScope(ply, 0, ply, 64)
    scope2:setForegroundColor(app.GRAY5)
    scope2:watchOutlet(source2:getOutlet())
    local panel = app.TextPanel(title, 1, 0.6)
    panel:setPosition(ply // 2, 0)
    graphic:addChild(scope1)
    graphic:addChild(scope2)
    graphic:addChild(panel)
    graphic:addChild(app.MainButton("left", 1))
    graphic:addChild(app.MainButton("right", 2))
    self:setControlGraphic(graphic)
    self:addSpotDescriptor{
      center = ply,
      radius = 2 * ply
    }
    self.panel = panel
  end

  self.chain = chain

  self.menuGraphic = app.Graphic(0, 0, 128, 64)
  self.menuGraphic:addChild(app.TextPanel("Rename Chain", 1))
  self.menuGraphic:addChild(app.TextPanel("Delete Chain", 2))
  self.menuGraphic:addChild(app.TextPanel("Space for Rent", 3))
end

function Header:onRename(oldName, newName)
  self.panel:setText(newName)
end

function Header:onCursorEnter()
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased")
end

function Header:onCursorLeave()
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased")
end

function Header:spotReleased()
  self.chain:show()
end

function Header:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    renameChain(self.chain)
  elseif i == 2 then
    deleteChain(self.chain)
  elseif i == 3 then

  end
  return true
end

--------------------------------------------------
local Insert = Class {}
Insert:include(SpottedControl)

function Insert:init(text)
  SpottedControl.init(self)
  self:setClassName("Insert")
  if text then
    self:setControlGraphic(app.TextPanel(text, 1))
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
  self.menuGraphic:addChild(app.TextPanel("New Mono Chain", 1))
  self.menuGraphic:addChild(app.TextPanel("New Stereo Chain", 2))
  self.menuGraphic:addChild(app.TextPanel("Space for Rent", 3))
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
    newChain(1)
  elseif i == 2 then
    newChain(2)
  elseif i == 3 then
  end
  return true
end

--------------------------------------------------
local Filler = Class {}
Filler:include(Section)

function Filler:init(title, chain)
  Section.init(self, app.sectionNoBorder)
  self:setClassName("Interface.Filler")
  self:addView("default")
  self:addControl("default", Insert("Add Chain Here"))
  self:switchView("default")
end

--------------------------------------------------
local Item = Class {}
Item:include(Section)

function Item:init(title, chain)
  Section.init(self, app.sectionSimple)
  self:setClassName("Interface.Item")
  self.chain = chain
  self:addView("default")
  self:addControl("default", Header(title, chain))
  self:addControl("default", Insert())
  self:switchView("default")
end

--------------------------------------------------
local MenuHeader = Class {}
MenuHeader:include(SpottedControl)

function MenuHeader:init()
  SpottedControl.init(self)
  self:setClassName("MenuHeader")
  local panel = app.TextPanel("Global Chains", 1)
  self:setControlGraphic(panel)
  self:addSpotDescriptor{
    center = 0.5 * ply,
    radius = ply
  }
  self.menuGraphic = app.Graphic(0, 0, 128, 64)
  self.menuGraphic:addChild(app.TextPanel("Clear All", 1))
  self.menuGraphic:addChild(app.TextPanel("Load Chains", 2))
  self.menuGraphic:addChild(app.TextPanel("Save Chains", 3))
end

function MenuHeader:onRename(oldName, newName)
  self.controlGraphic:setText(newName)
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
    deleteAllChains()
  elseif i == 2 then
    local Persist = require "Persist"
    Persist.loadGlobalsPreset()
  elseif i == 3 then
    local Persist = require "Persist"
    Persist.saveGlobalsPreset()
  end
  return true
end

--------------------------------------------------
local BeginMenu = Class {}
BeginMenu:include(Section)

function BeginMenu:init()
  Section.init(self, app.sectionBegin)
  self:setClassName("Interface.BeginMenu")
  self:addView("default")
  self:addControl("default", MenuHeader())
  self:addControl("default", Insert())
  self:switchView("default")
end

--------------------------------------------------
local Interface = Class {}
Interface:include(SpottedStrip)

function Interface:init(name)
  SpottedStrip.init(self)
  self:setClassName("GlobalChain.Interface")
  self:setInstanceName(name)
  self:appendSection(BeginMenu())
  self:appendSection(Filler())

  Signal.weakRegister("onCreateGlobalChain", self)
  Signal.weakRegister("onDeleteGlobalChain", self)
  Signal.weakRegister("onRenameGlobalChain", self)
  self.items = {}
end

function Interface:onCreateGlobalChain(chain, name, position)
  self:add(chain.title, chain, position)
end

function Interface:onDeleteGlobalChain(chain, name)
  local item = self.items[name]
  if item then
    self:removeSection(item)
    self.items[name] = nil
  end
end

function Interface:onRenameGlobalChain(chain, oldName, newName)
  local item = self.items[oldName]
  if item then
    self.items[oldName] = nil
    self.items[newName] = item
    item:notifyControls("onRename", oldName, newName)
  end
end

function Interface:add(title, chain, position)
  local item = Item(title, chain)
  self.items[title] = item
  -- load before the currently selected unit
  if position == nil then position = self:getSelectedSectionPosition() end
  if position == self:getSectionCount() then
    self:insertSection(item, position)
  else
    self:insertSection(item, position + 1)
  end
end

function Interface:upReleased(shifted)
  if shifted then return false end
  self:hide()
  return true
end

function Interface:homeReleased()
  self:hide()
  return true
end

function Interface:getOrderedChains()
  local chains = {}
  local n = self:getSectionCount()
  local j = 1
  for i = 1, n do
    local item = self:getSectionByIndex(i)
    if item.chain then chains[#chains + 1] = item.chain end
  end
  return chains
end

-- There should be only one instance because
-- the interface keeps track of the order.
return Interface()
