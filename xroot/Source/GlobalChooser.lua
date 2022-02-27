local app = app
local Class = require "Base.Class"
local Signal = require "Signal"
local SpottedStrip = require "SpottedStrip"
local Section = require "SpottedStrip.Section"
local SpottedControl = require "SpottedStrip.Control"
local ply = app.SECTION_PLY

--------------------------------------------------
local Header = Class {}
Header:include(SpottedControl)

function Header:init(title, chain)
  SpottedControl.init(self)
  self:setClassName("Source.GlobalChooser.Header")
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
      center = 0.5 * ply,
      radius = ply
    }
    self:addSpotDescriptor{
      center = 1.5 * ply,
      radius = ply
    }
    self.panel = panel
  end

  self.chain = chain
end

function Header:onRename(oldName, newName)
  self.panel:setText(newName)
end

function Header:spotReleased(i, shifted)
  self:callUp("choose", self.chain:getOutputSource(i))
  return true
end

--------------------------------------------------
local Item = Class {}
Item:include(Section)

function Item:init(title, chain)
  Section.init(self, app.sectionMiddle)
  self:setClassName("Source.GlobalChooser.Item")
  self.chain = chain
  self:addView("default")
  self:addControl("default", Header(title, chain))
  self:switchView("default")
end

--------------------------------------------------
local GlobalChooser = Class {}
GlobalChooser:include(SpottedStrip)

function GlobalChooser:init(ring)
  SpottedStrip.init(self)
  self:setClassName("Source.GlobalChooser")
  self.ring = ring

  Signal.weakRegister("onCreateGlobalChain", self)
  Signal.weakRegister("onDeleteGlobalChain", self)
  Signal.weakRegister("onRenameGlobalChain", self)
  self.items = {}
  self:refresh()
end

function GlobalChooser:refresh()
  for _, item in pairs(self.items) do
    self:removeSection(item)
  end
  self.items = {}
  local GlobalChains = require "GlobalChains.Interface"
  local ordered = GlobalChains:getOrderedChains()
  for i, chain in ipairs(ordered) do
    self:add(chain.title, chain, i)
  end
end

function GlobalChooser:onCreateGlobalChain(chain, name, position)
  self:refresh()
end

function GlobalChooser:onDeleteGlobalChain(chain, name)
  local item = self.items[name]
  if item then
    self:removeSection(item)
    self.items[name] = nil
  end
end

function GlobalChooser:onRenameGlobalChain(chain, oldName, newName)
  local item = self.items[oldName]
  if item then
    self.items[oldName] = nil
    self.items[newName] = item
    item:notifyControls("onRename", oldName, newName)
  end
end

function GlobalChooser:add(title, chain, position)
  local item = Item(title, chain)
  self.items[title] = item

  if position == nil then
    position = self:getSectionCount()
  end

  if position == self:getSectionCount() then
    self:insertSection(item, position)
  else
    self:insertSection(item, position + 1)
  end
end

function GlobalChooser:choose(src)
  return self.ring:choose(src)
end

function GlobalChooser:homeReleased()
  return self.ring:homeReleased()
end

function GlobalChooser:subReleased(i, shifted)
  return self.ring:subReleased(i, shifted)
end

function GlobalChooser:cancelReleased(shifted)
  return self.ring:cancelReleased(shifted)
end

function GlobalChooser:upReleased(shifted)
  return self.ring:upReleased(shifted)
end

return GlobalChooser
