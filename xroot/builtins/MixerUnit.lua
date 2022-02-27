local Class = require "Base.Class"
local Unit = require "Unit"
local BranchMeter = require "Unit.ViewControl.BranchMeter"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local MixerUnit = Class {}
MixerUnit:include(Unit)

function MixerUnit:init(args)
  if args.channelCount and args.channelCount > 1 then
    if args.forceMono then
      self.forceMono = true
      args.title = "Mono Mix"
      args.mnemonic = "1Mx"
    else
      args.title = "Stereo Mix"
      args.mnemonic = "2Mx"
    end
  else
    args.title = "Mix"
    args.mnemonic = "Mx"
  end
  Unit.init(self, args)
end

function MixerUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    if self.forceMono then
      self:loadMonoToStereoGraph()
    else
      self:loadStereoGraph()
    end
  else
    self:loadMonoGraph()
  end
end

function MixerUnit:loadMonoGraph()
  local sum = self:addObject("sum", app.Sum())
  local gain = self:addObject("gain", app.ConstantGain())
  gain:setClampInDecibels(-59.9)
  gain:hardSet("Gain", 1.0)

  connect(self, "In1", sum, "Left")
  connect(gain, "Out", sum, "Right")
  connect(sum, "Out", self, "Out1")

  self:addMonoBranch("input", gain, "In", gain, "Out")
end

-- Used by Chain.MarkMenu when pasting to a mixer.
function MixerUnit:getInputBranch()
  return self:getBranch("input")
end

function MixerUnit:loadStereoGraph()
  local sum1 = self:addObject("sum1", app.Sum())
  local gain1 = self:addObject("gain1", app.ConstantGain())
  gain1:setClampInDecibels(-59.9)
  gain1:hardSet("Gain", 1.0)
  local sum2 = self:addObject("sum2", app.Sum())
  local gain2 = self:addObject("gain2", app.ConstantGain())
  gain2:setClampInDecibels(-59.9)
  gain2:hardSet("Gain", 1.0)
  local balance = self:addObject("balance", app.StereoPanner())
  local pan = self:addObject("pan", app.GainBias())
  local panRange = self:addObject("panRange", app.MinMax())

  connect(self, "In1", sum1, "Left")
  connect(gain1, "Out", balance, "Left In")
  connect(balance, "Left Out", sum1, "Right")
  connect(sum1, "Out", self, "Out1")

  connect(self, "In2", sum2, "Left")
  connect(gain2, "Out", balance, "Right In")
  connect(balance, "Right Out", sum2, "Right")
  connect(sum2, "Out", self, "Out2")

  connect(pan, "Out", balance, "Pan")
  connect(pan, "Out", panRange, "In")

  tie(gain2, "Gain", gain1, "Gain")

  self:addStereoBranch("input", gain1, "In", gain2, "In", gain1, "Out", gain2,
                       "Out")
  self:addMonoBranch("pan", pan, "In", pan, "Out")

  self.objects.gain = self.objects.gain1
end

function MixerUnit:loadMonoToStereoGraph()
  local gain = self:addObject("gain", app.ConstantGain())
  gain:setClampInDecibels(-59.9)
  gain:hardSet("Gain", 1.0)

  local sum1 = self:addObject("sum1", app.Sum())
  local sum2 = self:addObject("sum2", app.Sum())
  local balance = self:addObject("balance", app.MonoPanner())
  local pan = self:addObject("pan", app.GainBias())
  local panRange = self:addObject("panRange", app.MinMax())

  connect(gain, "Out", balance, "In")

  connect(self, "In1", sum1, "Left")
  connect(balance, "Left", sum1, "Right")
  connect(sum1, "Out", self, "Out1")

  connect(self, "In2", sum2, "Left")
  connect(balance, "Right", sum2, "Right")
  connect(sum2, "Out", self, "Out2")

  connect(pan, "Out", balance, "Pan")
  connect(pan, "Out", panRange, "In")

  self:addMonoBranch("input", gain, "In", gain, "Out")
  self:addMonoBranch("pan", pan, "In", pan, "Out")
end

function MixerUnit:onLoadViews(objects, branches)
  local views = {
    expanded = {
      "gain"
    },
    collapsed = {}
  }

  local controls = {}

  controls.gain = BranchMeter {
    button = "input",
    branch = branches.input,
    faderParam = objects.gain:getParameter("Gain")
  }
  self:addToMuteGroup(controls.gain)

  if objects.pan then
    controls.pan = GainBias {
      button = "pan",
      branch = branches.pan,
      description = "Pan",
      gainbias = objects.pan,
      range = objects.panRange,
      biasMap = Encoder.getMap("default")
    }
    views.expanded[2] = "pan"
  end

  return controls, views
end

function MixerUnit:serialize()
  local t = Unit.serialize(self)
  t.mute = self.controls.gain:isMuted()
  t.solo = self.controls.gain:isSolo()
  return t
end

function MixerUnit:deserialize(t)
  Unit.deserialize(self, t)
  if t.mute then
    self.controls.gain:mute()
  end
  if t.solo then
    self.controls.gain:solo()
  end
end

-- Replace the default implementation for bypassing.
function MixerUnit:toggleBypass()
  local chain = self.branches.input
  local wasMuted = chain:muteIfNeeded()
  if self.pUnit:getBypass() then
    self:disableBypass()
  else
    self:enableBypass()
  end
  chain:unmuteIfNeeded(wasMuted)
end

return MixerUnit
