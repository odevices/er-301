local app = app
local Class = require "Base.Class"
local Window = require "Base.Window"
local Env = require "Env"
local Clipboard = require "Chain.Clipboard"

local MarkMenu = Class {}
MarkMenu:include(Window)

function MarkMenu:init(chain)
  Window.init(self)
  self:setClassName("MarkMenu")
  self.chain = chain
  self.sum = 0

  local graphic = self.mainGraphic
  local nMarked = chain:getMarkCount()
  if nMarked > 1 then
    local text = string.format("%d marked units", nMarked)
    graphic:addChild(app.TextPanel(text, 1))
  else
    graphic:addChild(app.TextPanel("1 marked unit", 1))
  end

  if nMarked < 6 then
    local j = 1
    for i = 1, chain:length() do
      local unit = chain:getUnit(i)
      if unit:marked() then
        j = j + 1
        local panel = app.TextPanel(unit.title, j)
        panel:imitateSectionStyle(app.sectionMiddle)
        graphic:addChild(panel)
      end
    end
  else
    local count = 0
    local j = 0
    local panel = app.TextPanel("...", 4)
    panel:imitateSectionStyle(app.sectionNoBorder)
    graphic:addChild(panel)
    for i = 1, chain:length() do
      local unit = chain:getUnit(i)
      if unit:marked() then
        count = count + 1
        if count < 3 then
          j = j + 1
          local panel = app.TextPanel(unit.title, j + 1)
          panel:imitateSectionStyle(app.sectionMiddle)
          graphic:addChild(panel)
        elseif count > nMarked - 2 then
          j = j + 1
          local panel = app.TextPanel(unit.title, j + 2)
          panel:imitateSectionStyle(app.sectionMiddle)
          graphic:addChild(panel)
        end
      end
    end
  end

  local graphic = self.subGraphic
  graphic:addChild(app.TextPanel("Copy", 1))
  graphic:addChild(app.TextPanel("Cut", 2))
  graphic:addChild(app.TextPanel("Move to Mixer", 3))
end

function MarkMenu:finish()
  self:hide()
  self.chain:clearMark()
  self.chain:disableSelection()
  self.chain:enableSelection()
end

function MarkMenu:encoder(change, shifted)
  self.sum = self.sum + change
  if math.abs(self.sum) > Env.EncoderThreshold.Default then self:finish() end
  return true
end

function MarkMenu:shiftReleased(shifted)
  self:finish()
  return true
end

function MarkMenu:enterReleased()
  self:finish()
  return true
end

function MarkMenu:upReleased(shifted)
  self:finish()
  return true
end

function MarkMenu:cancelReleased(shifted)
  self:finish()
  return true
end

function MarkMenu:homeReleased()
  self:finish()
  return true
end

function MarkMenu:mainReleased(i, shifted)
  self:finish()
  return true
end

function MarkMenu:subReleased(i, shifted)
  if i == 1 then
    -- copy
    Clipboard.copy(self.chain, 1)
  elseif i == 2 then
    -- cut
    local chain = self.chain
    Clipboard.copy(chain, 1, true)
    local wasMuted = chain:isMuted()
    if not wasMuted then chain:mute() end
    chain:stop()
    chain:removeMarkedUnits()
    chain:start()
    if not wasMuted then chain:unmute() end
  elseif i == 3 then
    -- move to mixer
    local UnitFactory = require "Unit.Factory"
    local chain = self.chain
    local index = Clipboard.copy(chain, 2, true)
    local wasMuted = chain:isMuted()
    if not wasMuted then chain:mute() end
    chain:stop()
    -- preserve the pins
    local root = chain:getRootChain()
    local pinData
    if root and root.serializePins then pinData = root:serializePins() end
    chain:removeMarkedUnits()
    local loadInfo = UnitFactory.getBuiltin("MixerUnit")
    if loadInfo then
      local mixer = chain:loadUnit(loadInfo, index)
      Clipboard.paste(mixer:getInputBranch(), 1, 2)
      Clipboard.clear(2)
    else
      app.logError("No mixer unit available!")
    end
    if root and root.deserializePins and pinData then
      root:deserializePins(pinData)
    end
    chain:start()
    if not wasMuted then chain:unmute() end
  end
  self:finish()
  return true
end

return MarkMenu
