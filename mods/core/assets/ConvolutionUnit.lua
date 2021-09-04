local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local SamplePool = require "Sample.Pool"
local SamplePoolInterface = require "Sample.Pool.Interface"
local Unit = require "Unit"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"
local Encoder = require "Encoder"
local GainBias = require "Unit.ViewControl.GainBias"

local ConvolutionUnit = Class {}
ConvolutionUnit:include(Unit)

function ConvolutionUnit:init(args)
  args.title = "Exact Convo- lution"
  args.mnemonic = "Co"
  Unit.init(self, args)
end

function ConvolutionUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function ConvolutionUnit:loadMonoGraph()
  local convolve = self:addObject("convolve", libcore.MonoConvolution())
  local xfade = self:addObject("xfade", app.CrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  connect(self, "In1", convolve, "In")
  connect(convolve, "Out", xfade, "A")
  connect(self, "In1", xfade, "B")
  connect(xfade, "Out", self, "Out1")

  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  self:addMonoBranch("wet", fader, "In", fader, "Out")
end

function ConvolutionUnit:loadStereoGraph()
  local convolve = self:addObject("convolve", libcore.StereoConvolution())
  local xfade = self:addObject("xfade", app.StereoCrossFade())
  local fader = self:addObject("fader", app.GainBias())
  local faderRange = self:addObject("faderRange", app.MinMax())

  connect(self, "In1", convolve, "Left In")
  connect(self, "In2", convolve, "Right In")
  connect(convolve, "Left Out", xfade, "Left A")
  connect(convolve, "Right Out", xfade, "Right A")
  connect(self, "In1", xfade, "Left B")
  connect(self, "In2", xfade, "Right B")
  connect(xfade, "Left Out", self, "Out1")
  connect(xfade, "Right Out", self, "Out2")

  connect(fader, "Out", xfade, "Fade")
  connect(fader, "Out", faderRange, "In")

  self:addMonoBranch("wet", fader, "In", fader, "Out")
end

function ConvolutionUnit:setSample(sample)
  if self.sample then self.sample:release(self) end

  self.sample = sample
  if self.sample then self.sample:claim(self) end

  if sample then
    if sample:isPending() then
      local Timer = require "Timer"
      local handle = Timer.every(0.5, function()
        if self.sample == nil then
          return false
        elseif sample.path ~= self.sample.path then
          return false
        elseif not sample:isPending() then
          self.objects.convolve:setSample(sample.pSample)
          return false
        end
      end)
    else
      self.objects.convolve:setSample(sample.pSample)
    end
  end
end

function ConvolutionUnit:serialize()
  local t = Unit.serialize(self)
  local sample = self.sample
  if sample then t.sample = SamplePool.serializeSample(sample) end
  return t
end

function ConvolutionUnit:deserialize(t)
  Unit.deserialize(self, t)
  if t.sample then
    local sample = SamplePool.deserializeSample(t.sample, self.chain)
    if sample then
      self:setSample(sample)
    else
      local Utils = require "Utils"
      app.logError("%s:deserialize: failed to load sample.", self)
      Utils.pp(t.sample)
    end
  end
end

function ConvolutionUnit:doDetachSample()
  local Overlay = require "Overlay"
  Overlay.flashMainMessage("Sample detached.")
  self:setSample()
end

function ConvolutionUnit:doAttachSampleFromCard()
  local task = function(sample)
    if sample then
      local Overlay = require "Overlay"
      Overlay.flashMainMessage("Attached sample: %s", sample.name)
      self:setSample(sample)
    end
  end
  local Pool = require "Sample.Pool"
  Pool.chooseFileFromCard(self.loadInfo.id, task)
end

function ConvolutionUnit:doAttachSampleFromPool()
  local chooser = SamplePoolInterface(self.loadInfo.id, "choose")
  chooser:setDefaultChannelCount(self.channelCount)
  local task = function(sample)
    if sample then
      local Overlay = require "Overlay"
      Overlay.flashMainMessage("Attached sample: %s", sample.name)
      self:setSample(sample)
    end
  end
  chooser:subscribe("done", task)
  chooser:show()
end

local menu = {
  "sampleHeader",
  "selectFromCard",
  "selectFromPool",
  "detachBuffer"
}

function ConvolutionUnit:onShowMenu(objects, branches)
  local controls = {}

  controls.sampleHeader = MenuHeader {
    description = "Sample Menu"
  }

  controls.selectFromCard = Task {
    description = "Select from Card",
    task = function()
      self:doAttachSampleFromCard()
    end
  }

  controls.selectFromPool = Task {
    description = "Select from Pool",
    task = function()
      self:doAttachSampleFromPool()
    end
  }

  controls.detachBuffer = Task {
    description = "Detach Buffer",
    task = function()
      self:doDetachSample()
    end
  }

  local sub = {}
  if self.sample then
    sub[1] = {
      position = app.GRID5_LINE1,
      justify = app.justifyLeft,
      text = "Attached Sample:"
    }
    sub[2] = {
      position = app.GRID5_LINE2,
      justify = app.justifyLeft,
      text = "+ " .. self.sample:getFilenameForDisplay(24)
    }
    sub[3] = {
      position = app.GRID5_LINE3,
      justify = app.justifyLeft,
      text = "+ " .. self.sample:getDurationText()
    }
    sub[4] = {
      position = app.GRID5_LINE4,
      justify = app.justifyLeft,
      text = string.format("+ %s %s %s", self.sample:getChannelText(),
                           self.sample:getSampleRateText(),
                           self.sample:getMemorySizeText())
    }
  else
    sub[1] = {
      position = app.GRID5_LINE3,
      justify = app.justifyCenter,
      text = "No sample attached."
    }
  end

  return controls, menu, sub
end

local views = {
  collapsed = {},
  expanded = {
    "wet"
  }
}

function ConvolutionUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.wet = GainBias {
    button = "wet",
    description = "Wet/Dry Amount",
    branch = branches.wet,
    gainbias = objects.fader,
    range = objects.faderRange,
    biasMap = Encoder.getMap("unit"),
    initialBias = 0.5
  }
  return controls, views
end

function ConvolutionUnit:onRemove()
  self:setSample(nil)
  Unit.onRemove(self)
end

return ConvolutionUnit
