local app = app
local Env = require "Env"
local Class = require "Base.Class"
local ChainBase = require "Chain.Base"
local HeaderSection = require "Chain.HeaderSection"
local FooterSection = require "Chain.FooterSection"
local EmptySection = require "Chain.EmptySection"
local Busy = require "Busy"
local ply = app.SECTION_PLY

-- Patch Class
local Patch = Class {}
Patch:include(ChainBase)

function Patch:init(args)
  ChainBase.init(self, args)
  self:setClassName("Chain.Patch")

  local unit = args.unit or app.logError("%s.init: unit is missing.", self)
  self.unit = unit
  if args.band then
    self.pChain = unit.pUnit:getBand(args.band - 1)
  else
    self.pChain = unit.pUnit -- expected type is CustomUnit
  end
  self.stopCount = 1

  self:disableSelection()
  self.headerSection = HeaderSection(self.title, self.subTitle, 0)
  self.footerSection = FooterSection(self.channelCount)
  self:subscribe("contentChanged", self.footerSection)
  self:appendSection(self.headerSection)
  self:appendSection(self.footerSection)
  self:insertEmptySection()
  self:setSelection(self.emptySection, "expanded", 0)
  self:enableSelection()
end

function Patch:getRootChain()
  return self.unit:getRootChain()
end

function Patch:getOutputSource(i)
  return self.unit:getOutputSource(i)
end

function Patch:getOutputDisplayName(channel)
  return self.unit:getOutputDisplayName(channel)
end

function Patch:insertEmptySection()
  if self.emptySection then return end
  self.emptySection = EmptySection(4 * ply)
  self:insertSection(self.emptySection, 2)
end

function Patch:clear()
  local Utils = require "Utils"
  Busy.start("Clearing: %s", Utils.shorten(self.unit.title, 42))
  self:removeAllUnits()
  Busy.stop()
end

function Patch:mnemonic()
  local n = self:length()
  if n == 0 then
    return "empty"
  elseif n == 1 then
    return self:getUnit(1).mnemonic
  else
    local u1 = self:getUnit(1).mnemonic
    local u2 = self:getUnit(n).mnemonic
    return u1 .. ".." .. u2
  end
end

function Patch:start()
  if self.stopCount > 0 then
    self.stopCount = self.stopCount - 1
    if self.stopCount == 0 then
      self.started = true
      for _, unit in pairs(self.units) do unit:start() end
    end
  end
end

function Patch:stop()
  if self.stopCount == 0 then
    self.stopCount = 1
    self.started = false
    for _, unit in pairs(self.units) do unit:stop() end
  else
    self.stopCount = self.stopCount + 1
  end
end

function Patch:serialize()
  local t = {
    lastPresetPath = self.lastPresetPath,
    lastPresetFilename = self.lastPresetFilename,
    firmwareVersion = app.FIRMWARE_VERSION,
    instanceKey = self:getInstanceKey()
  }
  t.units = self:serializeUnits()
  t.selection = self:serializeSelection()
  return t
end

function Patch:deserialize(t)
  self:startDeserialization()
  self:setLastPreset(t.lastPresetPath, t.lastPresetFilename)
  if t.instanceKey then self:setInstanceKey(t.instanceKey) end
  self:deserializeUnits(t.units)
  self:deserializeSelection(t.selection)
  self:finishDeserialization()
end

-- reset the patch to its blank state
function Patch:releaseResources()
  self:unsubscribe("contentChanged", self.footerSection)
  self:removeAllUnits()
end

function Patch:enable(soft)
  self:notifyUnits("enable", soft)
end

function Patch:disable(soft)
  self:notifyUnits("disable", soft)
end

function Patch:upReleased(shifted)
  if shifted then return false end
  self:hide()
  return true
end

function Patch:homeReleased()
  self:hide()
  return true
end

return Patch
