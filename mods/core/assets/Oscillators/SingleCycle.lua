local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local Pitch = require "Unit.ViewControl.Pitch"
local GainBias = require "Unit.ViewControl.GainBias"
local Gate = require "Unit.ViewControl.Gate"
local Encoder = require "Encoder"
local SamplePool = require "Sample.Pool"
local SamplePoolInterface = require "Sample.Pool.Interface"
local SlicingView = require "SlicingView"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"

local SingleCycle = Class {}
SingleCycle:include(Unit)

function SingleCycle:init(args)
  args.title = "Single Cycle"
  args.mnemonic = "SC"
  Unit.init(self, args)
end

function SingleCycle:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function SingleCycle:loadMonoGraph()
  local osc = self:addObject("osc", libcore.SingleCycle())
  local tune = self:addObject("tune", app.ConstantOffset())
  local tuneRange = self:addObject("tuneRange", app.MinMax())
  local f0 = self:addObject("f0", app.GainBias())
  local f0Range = self:addObject("f0Range", app.MinMax())
  local phase = self:addObject("phase", app.GainBias())
  local phaseRange = self:addObject("phaseRange", app.MinMax())
  local scan = self:addObject("scan", app.GainBias())
  local scanRange = self:addObject("scanRange", app.MinMax())
  local vca = self:addObject("vca", app.Multiply())
  local level = self:addObject("level", app.GainBias())
  local levelRange = self:addObject("levelRange", app.MinMax())

  local sync = self:addObject("sync", app.Comparator())
  sync:setTriggerMode()

  connect(sync, "Out", osc, "Sync")

  connect(tune, "Out", tuneRange, "In")
  connect(tune, "Out", osc, "V/Oct")

  connect(f0, "Out", osc, "Fundamental")
  connect(f0, "Out", f0Range, "In")

  connect(phase, "Out", osc, "Phase")
  connect(phase, "Out", phaseRange, "In")

  connect(scan, "Out", osc, "Slice Select")
  connect(scan, "Out", scanRange, "In")

  connect(level, "Out", levelRange, "In")
  connect(level, "Out", vca, "Left")

  connect(osc, "Out", vca, "Right")
  connect(vca, "Out", self, "Out1")

  self:addMonoBranch("level", level, "In", level, "Out")
  self:addMonoBranch("tune", tune, "In", tune, "Out")
  self:addMonoBranch("sync", sync, "In", sync, "Out")
  self:addMonoBranch("f0", f0, "In", f0, "Out")
  self:addMonoBranch("phase", phase, "In", phase, "Out")
  self:addMonoBranch("scan", scan, "In", scan, "Out")
end

function SingleCycle:loadStereoGraph()
  self:loadMonoGraph()
  connect(self.objects.vca, "Out", self, "Out2")
end

function SingleCycle:setSample(sample)
  if self.sample then
    self.sample:release(self)
    self.sample = nil
  end
  self.sample = sample
  if self.sample then self.sample:claim(self) end

  if sample == nil or sample:getChannelCount() == 0 then
    self.objects.osc:setSample(nil, nil)
  else
    self.objects.osc:setSample(sample.pSample, sample.slices.pSlices)
  end

  if self.slicingView then self.slicingView:setSample(sample) end
  self:notifyControls("setSample", sample)
end

function SingleCycle:showSampleEditor()
  if self.sample then
    if self.slicingView == nil then
      self.slicingView = SlicingView(self, self.objects.osc)
      self.slicingView:setSample(self.sample)
    end
    self.slicingView:show()
  else
    local Overlay = require "Overlay"
    Overlay.flashMainMessage("You must first select a sample.")
  end
end

function SingleCycle:doDetachSample()
  local Overlay = require "Overlay"
  Overlay.flashMainMessage("Sample detached.")
  self:setSample()
end

function SingleCycle:doAttachSampleFromCard()
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

function SingleCycle:doAttachSampleFromPool()
  local chooser = SamplePoolInterface(self.loadInfo.id, "choose")
  chooser:setDefaultChannelCount(self.channelCount)
  chooser:highlight(self.sample)
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
  "detachBuffer",
  "editSample",
  "interpolation"
}

function SingleCycle:onShowMenu(objects, branches)
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

  controls.editSample = Task {
    description = "Edit Buffer",
    task = function()
      self:showSampleEditor()
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
  expanded = {
    "scan",
    "tune",
    "freq",
    "phase",
    "sync",
    "level"
  },
  collapsed = {}
}

function SingleCycle:onLoadViews(objects, branches)
  local controls = {}

  controls.tune = Pitch {
    button = "V/oct",
    branch = branches.tune,
    description = "V/oct",
    offset = objects.tune,
    range = objects.tuneRange
  }

  controls.freq = GainBias {
    button = "f0",
    description = "Fundamental",
    branch = branches.f0,
    gainbias = objects.f0,
    range = objects.f0Range,
    biasMap = Encoder.getMap("oscFreq"),
    biasUnits = app.unitHertz,
    initialBias = 27.5,
    gainMap = Encoder.getMap("freqGain"),
    scaling = app.octaveScaling
  }

  controls.phase = GainBias {
    button = "phase",
    description = "Phase Offset",
    branch = branches.phase,
    gainbias = objects.phase,
    range = objects.phaseRange,
    biasMap = Encoder.getMap("[-1,1]"),
    initialBias = 0.0
  }

  controls.level = GainBias {
    button = "level",
    description = "Level",
    branch = branches.level,
    gainbias = objects.level,
    range = objects.levelRange,
    biasMap = Encoder.getMap("[-1,1]"),
    initialBias = 0.5
  }

  controls.sync = Gate {
    button = "sync",
    description = "Sync",
    branch = branches.sync,
    comparator = objects.sync
  }

  controls.scan = GainBias {
    button = "scan",
    description = "Table Scan",
    branch = branches.scan,
    gainbias = objects.scan,
    range = objects.scanRange,
    biasMap = Encoder.getMap("[0,1]"),
    initialBias = 0
  }

  return controls, views
end

function SingleCycle:serialize()
  local t = Unit.serialize(self)
  local sample = self.sample
  if sample then t.sample = SamplePool.serializeSample(sample) end
  return t
end

function SingleCycle:deserialize(t)
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

function SingleCycle:onRemove()
  self:setSample(nil)
  Unit.onRemove(self)
end

return SingleCycle
