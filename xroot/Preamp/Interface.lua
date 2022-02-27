local app = app
local Class = require "Base.Class"
local Signal = require "Signal"
local SpottedStrip = require "SpottedStrip"
local Section = require "SpottedStrip.Section"
local SpottedControl = require "SpottedStrip.Control"
local Preamp = require "Preamp"
local ply = app.SECTION_PLY

--------------------------------------------------
local ChannelControl = Class {}
ChannelControl:include(SpottedControl)

function ChannelControl:init(channel)
  SpottedControl.init(self)
  self:setClassName("Preamp.Interface.ChannelControl")
  self.channel = channel
  local source = app.getExternalSource(channel)
  if source then
    local graphic = app.Graphic(0, 0, ply, 64)
    local scope = app.MiniScope(0, 14, ply, 50)
    scope:setForegroundColor(app.GRAY7)
    scope:watchOutlet(source:getOutlet())
    graphic:addChild(scope)
    self.button = app.MainButton(channel, 1)
    graphic:addChild(self.button)
    self:setControlGraphic(graphic)
    self:addSpotDescriptor{
      center = 0.5 * ply
    }
    self.verticalDivider = 0
  end

  self.menuGraphic = app.Graphic(0, 0, 128, 64)
  self.panels = {}
  local panel
  panel = app.TextPanel("+0dB 1x 14V-pp modular", 1)
  self.panels[1] = panel -- index refers to 1x
  self.menuGraphic:addChild(panel)
  panel = app.TextPanel("+6dB 2x 7V-pp ...", 2)
  self.panels[2] = panel -- index refers to 2x
  self.menuGraphic:addChild(panel)
  panel = app.TextPanel("+12dB 4x 3.5V-pp line", 3)
  self.panels[4] = panel -- index refers to 4x
  self.menuGraphic:addChild(panel)

  Signal.weakRegister("onPreampChanged", self)
  self:onPreampChanged()
end

function ChannelControl:onCursorEnter()
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased")
end

function ChannelControl:onCursorLeave()
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased")
end

function ChannelControl:subReleased(i, shifted)
  if shifted then
    return false
  end
  if i == 1 then
    -- 0dB
    Preamp.set(self.channel, 1, true)
  elseif i == 2 then
    -- 6dB
    Preamp.set(self.channel, 2, true)
  elseif i == 3 then
    -- 12dB
    Preamp.set(self.channel, 4, true)
  end
  return true
end

function ChannelControl:onPreampChanged()
  if self.channel then
    local gain = Preamp.get(self.channel)
    if gain then
      local text = string.format("%s: %dx", self.channel, gain)
      self.button:setText(text)
      self.panels[1]:deselect()
      self.panels[2]:deselect()
      self.panels[4]:deselect()
      self.panels[gain]:select()
    else
      app.logWarn("%s.onPreampChanged(): gain is nil for channel %s.", self,
                  self.channel)
    end
  else
    app.logWarn("%s.onPreampChanged(): channel is nil.", self)
  end
end

--------------------------------------------------
local ChannelSection = Class {}
ChannelSection:include(Section)

function ChannelSection:init(channel)
  Section.init(self, app.sectionNoBorder)
  self:setClassName("Preamp.Interface.ChannelSection")
  self:addView("default")
  self:addControl("default", ChannelControl(channel))
  self:switchView("default")
end

--------------------------------------------------
local MenuControl = Class {}
MenuControl:include(SpottedControl)

function MenuControl:init(type)
  SpottedControl.init(self)
  self:setClassName("Preamp.Interface.MenuControl")
  if type == "header" then
    local panel = app.TextPanel("Preamp Settings", 1)
    panel:setSize(ply - 2, 64)
    self:setControlGraphic(panel)
  else
    local graphic = app.Graphic(0, 0, ply, 64)
    self:setControlGraphic(graphic)
    local sources = {
      app.getExternalSource("IN1"),
      app.getExternalSource("IN2"),
      app.getExternalSource("IN3"),
      app.getExternalSource("IN4")
    }
    local margin = 2
    local width = 6
    for i, source in ipairs(sources) do
      local meter = app.VerticalMeter((i - 1) * (width + margin) + 6, 4, width,
                                      56)
      meter:watchOutlet(source:getOutlet())
      graphic:addChild(meter)
    end
    self.verticalDivider = 0
  end

  self:addSpotDescriptor{
    center = 0.5 * ply,
    radius = ply
  }
  self.menuGraphic = app.Graphic(0, 0, 128, 64)
  self.menuGraphic:addChild(app.TextPanel("Reset All", 1))
  self.menuGraphic:addChild(app.TextPanel("Load Preset", 2))
  self.menuGraphic:addChild(app.TextPanel("Save Preset", 3))
end

function MenuControl:onCursorEnter()
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased")
end

function MenuControl:onCursorLeave()
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased")
end

function MenuControl:subReleased(i, shifted)
  if shifted then
    return false
  end
  if i == 1 then
    Preamp.set("IN1", 1, false)
    Preamp.set("IN2", 1, false)
    Preamp.set("IN3", 1, false)
    Preamp.set("IN4", 1, true)
  elseif i == 2 then
    local Persist = require "Persist"
    Persist.loadPreampPreset()
  elseif i == 3 then
    local Persist = require "Persist"
    Persist.savePreampPreset()
  end
  return true
end

--------------------------------------------------
local MenuSection = Class {}
MenuSection:include(Section)

function MenuSection:init(type)
  Section.init(self, app.sectionNoBorder)
  self:setClassName("Preamp.Interface.MenuSection")
  self:addView("default")
  self:addControl("default", MenuControl(type))
  self:switchView("default")
end

--------------------------------------------------
local Interface = Class {}
Interface:include(SpottedStrip)

function Interface:init(name)
  SpottedStrip.init(self)
  self:setClassName("Preamp.Interface")
  self:setInstanceName(name)
  self:appendSection(MenuSection("header"))
  self:appendSection(ChannelSection("IN1"))
  self:appendSection(ChannelSection("IN2"))
  self:appendSection(ChannelSection("IN3"))
  self:appendSection(ChannelSection("IN4"))
  self:appendSection(MenuSection("footer"))
end

function Interface:upReleased(shifted)
  if shifted then
    return false
  end
  self:hide()
  return true
end

function Interface:homeReleased()
  self:hide()
  return true
end

return Interface
