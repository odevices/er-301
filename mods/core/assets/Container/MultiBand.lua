local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local PatchMeter = require "Unit.ViewControl.PatchMeter"
local Patch = require "Chain.Patch"

local MultiBand = Class {}
MultiBand:include(Unit)

function MultiBand:init(args)
  local title = args.title or app.logError("%s.init: title is missing.", self)
  local chain = args.chain or app.logError("%s.init: chain is missing.", self)
  args.mnemonic = string.format("%dB", args.bandCount)
  self.bandCount = args.bandCount or 2
  args.pUnit = libcore.BandedUnit(title, chain.channelCount, self.bandCount)
  Unit.init(self, args)
end

function MultiBand:notifyBranches(method, ...)
  -- propagate branch notifications to the bands
  for i = 1, self.bandCount do
    local band = self.bands[i]
    local f = band[method]
    if f ~= nil then f(band, ...) end
  end
  Unit.notifyBranches(self, method, ...)
end

function MultiBand:findUnitByTitle(title)
  local unit = Unit.findUnitByTitle(self, title)
  if unit then return unit end
  for i = 1, self.bandCount do
    local band = self.bands[i]
    unit = band and band:findUnitByTitle(title)
    if unit then return unit end
  end
end

function MultiBand:findByInstanceKey(key)
  local o = Unit.findByInstanceKey(self, key)
  if o then return o end
  for i = 1, self.bandCount do
    local band = self.bands[i]
    o = band and band:findByInstanceKey(key)
    if o then return o end
  end
end

function MultiBand:onLoadGraph(channelCount)
  self.bands = {}
  for i = 1, self.bandCount do
    self.bands[i] = Patch {
      title = self.title,
      subTitle = "band" .. i,
      depth = self.depth,
      channelCount = channelCount,
      unit = self,
      band = i
    }
  end
end

function MultiBand:onLoadViews(objects, branches)
  local controls = {}
  local views = {
    expanded = {},
    collapsed = {}
  }

  for i = 1, self.bandCount do
    controls["band" .. i] = PatchMeter {
      button = "band" .. i,
      description = "Band " .. i,
      patch = self.bands[i]
    }
    views.expanded[i] = "band" .. i
  end

  return controls, views
end

function MultiBand:onRemove()
  for i = 1, self.bandCount do
    local band = self.bands[i]
    band:stop()
    band:releaseResources()
    band:hide()
  end
  Unit.onRemove(self)
end

function MultiBand:onGenerateTitle()
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

function MultiBand:serialize()
  local t = Unit.serialize(self)
  local bands = {}
  for i = 1, self.bandCount do bands[i] = self.bands[i]:serialize() end
  t.bands = bands
  return t
end

function MultiBand:deserialize(t)
  Unit.deserialize(self, t)
  if t.bands then
    for i = 1, self.bandCount do self.bands[i]:deserialize(t.bands[i] or {}) end
  end
end

return MultiBand
