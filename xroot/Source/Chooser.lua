local app = app
local Class = require "Base.Class"
local Observable = require "Base.Observable"
local Signal = require "Signal"

local Chooser = Class {}
Chooser:include(Observable)

function Chooser:init(chain, currentSource)
  app.collectgarbage()
  Observable.init(self)
  self:setClassName("Source.Chooser")
  self.chain = chain

  self.panels = {}
  local graphic = app.Graphic(0, 0, 128, 64)
  local Drawings = require "Drawings"
  local drawing = app.Drawing(0, 0, 128, 64)
  drawing:add(Drawings.Sub.HelpfulButtons)
  graphic:addChild(drawing)

  local panel = app.TextPanel("Jacks", 1, 0.5, app.GRID5_LINE3 - 1)
  self.panels[1] = panel
  graphic:addChild(panel)
  if chain then
    panel = app.TextPanel("Locals", 2, 0.5, app.GRID5_LINE3 - 1)
  else
    panel = app.TextPanel("", 2, 0.5, app.GRID5_LINE3 - 1)
  end
  self.panels[2] = panel
  graphic:addChild(panel)
  local GlobalChains = require "GlobalChains"
  local nGlobals = GlobalChains.count()
  if nGlobals == 1 then
    panel = app.TextPanel("1 Global", 3, 0.5, app.GRID5_LINE3 - 1)
  else
    panel = app.TextPanel(string.format("%d Globals", nGlobals), 3, 0.5,
                          app.GRID5_LINE3 - 1)
  end
  self.panels[3] = panel
  Signal.weakRegister("onGlobalChainCountChanged", self)
  graphic:addChild(panel)

  local label = app.Label("Choose a source.", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(3, app.GRID5_LINE1)
  graphic:addChild(label)
  self.help1 = label

  label = app.Label("M1-M6 to commit.", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(3, app.GRID5_LINE2)
  graphic:addChild(label)
  self.help2 = label

  label = app.Label("CANCEL to abort.", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(3, app.GRID5_LINE3)
  graphic:addChild(label)
  self.help3 = label

  self.menuGraphic = graphic

  self.index = 1
  self.currentSource = currentSource
  if currentSource then
    if currentSource.type == "global" then
      self.index = 3
    elseif currentSource.type == "local" then
      self.index = 2
    end
  end

  if self.index == 3 and nGlobals == 0 then
    self.index = 1
  end

  self:setChooser(self.index)
end

function Chooser:onGlobalChainCountChanged(n)
  if n == 1 then
    self.panels[3]:setText("1 Global")
  else
    self.panels[3]:setText(string.format("%d Globals", n))
  end
  if n == 0 and self.index == 3 then
    self:setChooser(1)
  end
end

function Chooser:getExternalChooser()
  if self.externals == nil then
    local ExternalChooser = require "Source.ExternalChooser"
    self.externals = ExternalChooser(self)
  end
  return self.externals
end

function Chooser:getLocalChooser()
  if self.locals == nil then
    local LocalChooser = require "Source.LocalChooser"
    self.locals = LocalChooser(self, self.chain, self.currentSource)
  end
  return self.locals
end

function Chooser:getGlobalChooser()
  if self.globals == nil then
    local GlobalChooser = require "Source.GlobalChooser"
    self.globals = GlobalChooser(self)
  end
  return self.globals
end

function Chooser:show(context)
  self.current:show(context)
end

function Chooser:hide()
  self.current:hide()
end

function Chooser:subReleased(i, shifted)
  if shifted then
    return false
  end
  if i == self.index then
    return
  end
  local GlobalChains = require "GlobalChains"
  if i == 3 and GlobalChains.count() == 0 then
    local Overlay = require "Overlay"
    Overlay.flashMainMessage("No global chains.")
    return
  end
  self:setChooser(i)
  return true
end

function Chooser:setChooser(i)
  if self.chain == nil and i == 2 then
    return
  end

  local window
  if i == 2 then
    self.index = 2
    window = self:getLocalChooser()
    self.help1:setText("Select local source.")
    self.help2:setText("ENTER to choose.")
    self.help3:setText("CANCEL to abort.")
  elseif i == 3 then
    self.index = 3
    window = self:getGlobalChooser()
    self.help1:setText("Select global chain.")
    self.help2:setText("M1-M6 to choose.")
    self.help3:setText("CANCEL to abort.")
  else -- default
    self.index = 1
    window = self:getExternalChooser()
    self.help1:setText("Select jack.")
    self.help2:setText("M1-M6 to choose.")
    self.help3:setText("CANCEL to abort.")
  end

  self.panels[1]:deselect()
  self.panels[2]:deselect()
  self.panels[3]:deselect()
  self.panels[self.index]:select()

  if self.current then
    self.current:removeSubGraphic(self.menuGraphic)
  end

  window:addSubGraphic(self.menuGraphic)

  if self.current and self.current.context then
    -- active
    if self.current ~= window then
      self.current:replace(window)
    end
  end

  self.current = window
end

function Chooser:choose(src)
  self.current:hide()
  self:releaseResources()
  self:emitSignal("choose", src)
  return true
end

function Chooser:cancelReleased(shifted)
  if not shifted then
    self.current:hide()
    self:releaseResources()
  end
  return true
end

function Chooser:upReleased(shifted)
  if not shifted then
    self.current:hide()
    self:releaseResources()
  end
  return true
end

function Chooser:homeReleased()
  self.current:hide()
  return true
end

function Chooser:releaseResources()
  self.locals = nil
  self.globals = nil
  self.externals = nil
  app.collectgarbage()
end

return Chooser
