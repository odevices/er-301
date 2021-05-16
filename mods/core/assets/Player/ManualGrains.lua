local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local SamplePool = require "Sample.Pool"
local SamplePoolInterface = require "Sample.Pool.Interface"
local Encoder = require "Encoder"
local Gate = require "Unit.ViewControl.Gate"
local Pitch = require "Unit.ViewControl.Pitch"
local GainBias = require "Unit.ViewControl.GainBias"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"
local GrainView = require "core.Player.GrainView"
local SampleEditor = require "Sample.Editor"

local ManualGrains = Class {}
ManualGrains:include(Unit)

function ManualGrains:init(args)
  args.title = "Manual Grains"
  args.mnemonic = "MG"
  Unit.init(self, args)
end

function ManualGrains:onLoadGraph(channelCount)
  local head = self:addObject("head", libcore.GranularHead(channelCount))

  local start = self:addObject("start", app.ParameterAdapter())
  local duration = self:addObject("duration", app.ParameterAdapter())
  duration:hardSet("Bias", 0.1)
  local gain = self:addObject("gain", app.ParameterAdapter())
  local squash = self:addObject("squash", app.ParameterAdapter())
  tie(head, "Duration", duration, "Out")
  tie(head, "Start", start, "Out")
  tie(head, "Gain", gain, "Out", head, "Gain")
  tie(head, "Squash", squash, "Out")

  local trig = self:addObject("trig", app.Comparator())
  local speed = self:addObject("speed", app.GainBias())
  local tune = self:addObject("tune", app.ConstantOffset())
  local pitch = self:addObject("pitch", libcore.VoltPerOctave())
  local multiply = self:addObject("multiply", app.Multiply())
  local clipper = self:addObject("clipper", libcore.Clipper())
  clipper:setMaximum(64.0)
  clipper:setMinimum(-64.0)

  local tuneRange = self:addObject("tuneRange", app.MinMax())
  local speedRange = self:addObject("speedRange", app.MinMax())

  -- Pitch and Linear FM
  connect(tune, "Out", pitch, "In")
  connect(tune, "Out", tuneRange, "In")
  connect(pitch, "Out", multiply, "Left")
  connect(speed, "Out", multiply, "Right")
  connect(speed, "Out", speedRange, "In")
  connect(multiply, "Out", clipper, "In")
  connect(clipper, "Out", head, "Speed")

  connect(trig, "Out", head, "Trigger")
  connect(head, "Left Out", self, "Out1")

  self:addMonoBranch("speed", speed, "In", speed, "Out")
  self:addMonoBranch("tune", tune, "In", tune, "Out")
  self:addMonoBranch("trig", trig, "In", trig, "Out")
  self:addMonoBranch("start", start, "In", start, "Out")
  self:addMonoBranch("duration", duration, "In", duration, "Out")
  self:addMonoBranch("gain", gain, "In", gain, "Out")
  self:addMonoBranch("squash", squash, "In", squash, "Out")

  if channelCount > 1 then
    local pan = self:addObject("pan", app.ParameterAdapter())
    tie(head, "Pan", pan, "Out")
    connect(head, "Right Out", self, "Out2")
    self:addMonoBranch("pan", pan, "In", pan, "Out")
  end

end

function ManualGrains:serialize()
  local t = Unit.serialize(self)
  local sample = self.sample
  if sample then t.sample = SamplePool.serializeSample(sample) end
  return t
end

function ManualGrains:deserialize(t)
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

function ManualGrains:setSample(sample)
  if self.sample then
    self.sample:release(self)
    self.sample = nil
  end

  -- construct a new slices object when the sample changes
  if sample == nil or sample:getChannelCount() == 0 then
    self.objects.head:setSample(nil)
  else
    self.objects.head:setSample(sample.pSample)
    self.sample = sample
    self.sample:claim(self)
  end

  if self.sampleEditor then self.sampleEditor:setSample(sample) end
  self:notifyControls("setSample", sample)
end

function ManualGrains:doDetachSample()
  local Overlay = require "Overlay"
  Overlay.mainFlashMessage("Sample detached.")
  self:setSample()
end

function ManualGrains:doAttachSampleFromCard()
  local task = function(sample)
    if sample then
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Attached sample: %s", sample.name)
      self:setSample(sample)
    end
  end
  local Pool = require "Sample.Pool"
  Pool.chooseFileFromCard(self.loadInfo.id, task)
end

function ManualGrains:doAttachSampleFromPool()
  local chooser = SamplePoolInterface(self.loadInfo.id, "choose")
  chooser:setDefaultChannelCount(self.channelCount)
  chooser:highlight(self.sample)
  local task = function(sample)
    if sample then
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Attached sample: %s", sample.name)
      self:setSample(sample)
    end
  end
  chooser:subscribe("done", task)
  chooser:show()
end

function ManualGrains:showSampleEditor()
  if self.sample then
    if self.sampleEditor == nil then
      self.sampleEditor = SampleEditor(self, self.objects.head)
      self.sampleEditor:setSample(self.sample)
      self.sampleEditor:setPointerLabel("G")
    end
    self.sampleEditor:show()
  else
    local Overlay = require "Overlay"
    Overlay.mainFlashMessage("You must first select a sample.")
  end
end

local menu = {
  "sampleHeader",
  "selectFromCard",
  "selectFromPool",
  "detachBuffer",
  "editSample"
}

function ManualGrains:onShowMenu(objects, branches)
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

local stereoViews = {
  expanded = {
    "trigger",
    "pitch",
    "speed",
    "start",
    "duration",
    "pan",
    "gain",
    "squash"
  },
  trigger = {
    "gview",
    "trigger"
  },
  pitch = {
    "gview",
    "pitch"
  },
  speed = {
    "gview",
    "speed"
  },
  start = {
    "gview",
    "start"
  },
  duration = {
    "gview",
    "duration"
  },
  squash = {
    "gview",
    "squash"
  },
  collapsed = {}
}

local monoViews = {
  expanded = {
    "trigger",
    "pitch",
    "speed",
    "start",
    "duration",
    "gain",
    "squash"
  },
  trigger = {
    "gview",
    "trigger"
  },
  pitch = {
    "gview",
    "pitch"
  },
  speed = {
    "gview",
    "speed"
  },
  start = {
    "gview",
    "start"
  },
  duration = {
    "gview",
    "duration"
  },
  squash = {
    "gview",
    "squash"
  },
  collapsed = {}
}

function ManualGrains:onLoadViews(objects, branches)
  local controls = {}

  controls.pitch = Pitch {
    button = "V/oct",
    description = "V/oct",
    branch = branches.tune,
    offset = objects.tune,
    range = objects.tuneRange
  }

  controls.speed = GainBias {
    button = "speed",
    description = "Speed",
    branch = branches.speed,
    gainbias = objects.speed,
    range = objects.speedRange,
    biasMap = Encoder.getMap("speed"),
    initialBias = 1.0
  }

  controls.trigger = Gate {
    button = "trig",
    description = "Trigger",
    branch = branches.trig,
    comparator = objects.trig
  }

  controls.start = GainBias {
    button = "start",
    description = "Start",
    branch = branches.start,
    gainbias = objects.start,
    range = objects.start,
    biasMap = Encoder.getMap("unit")
  }

  controls.duration = GainBias {
    button = "dur",
    description = "Duration",
    branch = branches.duration,
    gainbias = objects.duration,
    range = objects.duration,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitSecs
  }

  controls.gain = GainBias {
    button = "gain",
    description = "Gain",
    branch = branches.gain,
    gainbias = objects.gain,
    range = objects.gain,
    initialBias = 1.0
  }

  controls.squash = GainBias {
    button = "squash",
    description = "Squash",
    branch = branches.squash,
    gainbias = objects.squash,
    range = objects.squash,
    biasMap = Encoder.getMap("gain36dB"),
    biasUnits = app.unitDecibels,
    initialBias = 1.0
  }

  controls.gview = GrainView {
    head = objects.head
  }

  if objects.pan then
    controls.pan = GainBias {
      button = "pan",
      branch = branches.pan,
      description = "Pan",
      gainbias = objects.pan,
      range = objects.pan,
      biasMap = Encoder.getMap("default"),
      biasUnits = app.unitNone
    }

    return controls, stereoViews
  else
    return controls, monoViews
  end
end

function ManualGrains:onRemove()
  self:setSample(nil)
  Unit.onRemove(self)
end

return ManualGrains
