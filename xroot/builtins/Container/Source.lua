local Class = require "Base.Class"
local Unit = require "Unit"
local PatchMeter = require "Unit.ViewControl.PatchMeter"
local Patch = require "Chain.Patch"

local Source = Class {}
Source:include(Unit)

function Source:init(args)
  local chain = args.chain or app.logError("%s.init: chain is missing.", self)
  args.title = "Custom Source"
  args.mnemonic = "S"
  args.pUnit = app.SourceUnit(args.title, chain.channelCount)
  Unit.init(self, args)
end

function Source:notifyBranches(method, ...)
  -- propagate branch notifications to the patch chain
  if self.patch then
    local f = self.patch[method]
    if f ~= nil then f(self.patch, ...) end
  end
  Unit.notifyBranches(self, method, ...)
end

function Source:findUnitByTitle(title)
  local unit = Unit.findUnitByTitle(self, title)
  if unit then return unit end
  if self.patch then return self.patch:findUnitByTitle(title) end
end

function Source:findByInstanceKey(key)
  local o = Unit.findByInstanceKey(self, key)
  if o then return o end
  if self.patch then return self.patch:findByInstanceKey(key) end
end

function Source:onLoadGraph(channelCount)
  self.patch = Patch {
    title = self.title,
    depth = self.depth,
    channelCount = channelCount,
    unit = self
  }
end

local views = {
  expanded = {
    "meter"
  },
  collapsed = {}
}

function Source:onLoadViews(objects, branches)
  local controls = {}
  controls.meter = PatchMeter {
    button = "open",
    description = "Patch",
    patch = self.patch
  }
  return controls, views
end

function Source:onRemove()
  self.patch:stop()
  self.patch:releaseResources()
  self.patch:hide()
  Unit.onRemove(self)
end

function Source:onGenerateTitle()
  local Settings = require "Settings"
  local mode = Settings.get("containerUnitNameGen")
  if mode == "off" then
    Unit.onGenerateTitle(self)
  else
    local Random = require "Random"
    self:setTitle(Random.generateName(mode))
    self.hasUserTitle = true
  end
end

function Source:serialize()
  local t = Unit.serialize(self)
  t.bands = {
    self.patch:serialize()
  }
  return t
end

function Source:deserialize(t)
  Unit.deserialize(self, t)
  local Preset = require "Persist.Preset"
  local preset = Preset(t)
  local version = preset:getVersionString()
  if version < "0.4.00" then
    app.logInfo("%s: legacy preset version (%s) detected.", self, version)
    local Legacy = require "core.Container.Legacy"
    Legacy.deserializeCustomizations(self, t)
  elseif t.bands and t.bands[1] then
    self.patch:deserialize(t.bands[1])
  elseif t.patch then
    self.patch:deserialize(t.patch)
  end
end

return Source
