local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"

local Pink = Class {}
Pink:include(Unit)

function Pink:init(args)
  args.title = "Pink Noise"
  args.mnemonic = "PN"
  Unit.init(self, args)
end

function Pink:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function Pink:loadMonoGraph()
  local noise1 = self:addObject("noise1", libcore.PinkNoise())
  connect(noise1, "Out", self, "Out1")
end

function Pink:loadStereoGraph()
  local noise1 = self:addObject("noise1", libcore.PinkNoise())
  local noise2 = self:addObject("noise2", libcore.PinkNoise())
  connect(noise1, "Out", self, "Out1")
  connect(noise2, "Out", self, "Out2")
end

return Pink
