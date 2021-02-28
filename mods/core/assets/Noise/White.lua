local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"

local White = Class {}
White:include(Unit)

function White:init(args)
  args.title = "White Noise"
  args.mnemonic = "WN"
  Unit.init(self, args)
end

function White:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function White:loadMonoGraph()
  local noise1 = self:addObject("noise1", libcore.WhiteNoise())
  connect(noise1, "Out", self, "Out1")
end

function White:loadStereoGraph()
  local noise1 = self:addObject("noise1", libcore.WhiteNoise())
  local noise2 = self:addObject("noise2", libcore.WhiteNoise())
  connect(noise1, "Out", self, "Out1")
  connect(noise2, "Out", self, "Out2")
end

return White
