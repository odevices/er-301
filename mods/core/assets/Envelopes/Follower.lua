local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local Follower = Class {}
Follower:include(Unit)

function Follower:init(args)
  args.title = "Envelope Follower"
  args.mnemonic = "EF"
  Unit.init(self, args)
end

function Follower:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function Follower:loadMonoGraph()
  local env = self:addObject("env", libcore.EnvelopeFollower())
  local attack = self:addObject("attack", app.ParameterAdapter())
  local release = self:addObject("release", app.ParameterAdapter())

  connect(self, "In1", env, "In")
  connect(env, "Out", self, "Out1")

  tie(env, "Attack Time", attack, "Out")
  self:addMonoBranch("attack", attack, "In", attack, "Out")
  tie(env, "Release Time", release, "Out")
  self:addMonoBranch("release", release, "In", release, "Out")
end

function Follower:loadStereoGraph()

  local env1 = self:addObject("env1", libcore.EnvelopeFollower())
  local env2 = self:addObject("env2", libcore.EnvelopeFollower())
  local attack = self:addObject("attack", app.ParameterAdapter())
  local release = self:addObject("release", app.ParameterAdapter())

  -- connect objects
  connect(self, "In1", env1, "In")
  connect(env1, "Out", self, "Out1")

  connect(self, "In2", env2, "In")
  connect(env2, "Out", self, "Out2")

  tie(env1, "Attack Time", attack, "Out")
  tie(env2, "Attack Time", attack, "Out")
  self:addMonoBranch("attack", attack, "In", attack, "Out")
  tie(env1, "Release Time", release, "Out")
  tie(env2, "Release Time", release, "Out")
  self:addMonoBranch("release", release, "In", release, "Out")
end

local views = {
  expanded = {
    "attack",
    "release"
  },
  collapsed = {}
}

function Follower:onLoadViews(objects, branches)
  local controls = {}

  controls.attack = GainBias {
    button = "attack",
    description = "Attack Time",
    branch = branches.attack,
    gainbias = objects.attack,
    range = objects.attack,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitSecs,
    initialBias = 0.001
  }

  controls.release = GainBias {
    button = "release",
    description = "Release Time",
    branch = branches.release,
    gainbias = objects.release,
    range = objects.release,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitSecs,
    initialBias = 0.010
  }

  return controls, views
end

return Follower
