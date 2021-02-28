local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local SamplePool = require "Sample.Pool"
local SamplePoolInterface = require "Sample.Pool.Interface"
local Encoder = require "Encoder"
local Fader = require "Unit.ViewControl.Fader"
local Gate = require "Unit.ViewControl.Gate"
local Pitch = require "Unit.ViewControl.Pitch"
local GainBias = require "Unit.ViewControl.GainBias"
local Task = require "Unit.MenuControl.Task"
local OptionControl = require "Unit.MenuControl.OptionControl"
local MenuHeader = require "Unit.MenuControl.Header"
local WaveForm = require "core.Player.WaveForm"
local SampleEditor = require "Sample.Editor"

local ManualLoops = Class {}
ManualLoops:include(Unit)

function ManualLoops:init(args)
  args.title = "Manual Loops"
  args.mnemonic = "ML"
  Unit.init(self, args)
end

function ManualLoops:onLoadGraph(channelCount)
  local head = self:addObject("head", libcore.LoopHead(channelCount))

  local lstart = self:addObject("lstart", app.ParameterAdapter())
  local length = self:addObject("length", app.ParameterAdapter())
  tie(head, "Loop Start", lstart, "Out")
  tie(head, "Loop Length", length, "Out")

  local sync = self:addObject("sync", app.Comparator())
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

  connect(sync, "Out", head, "Sync")
  connect(head, "Left Out", self, "Out1")

  self:addMonoBranch("speed", speed, "In", speed, "Out")
  self:addMonoBranch("tune", tune, "In", tune, "Out")
  self:addMonoBranch("sync", sync, "In", sync, "Out")
  self:addMonoBranch("start", lstart, "In", lstart, "Out")
  self:addMonoBranch("length", length, "In", length, "Out")

  if channelCount > 1 then connect(head, "Right Out", self, "Out2") end
end

function ManualLoops:serialize()
  local t = Unit.serialize(self)
  local sample = self.sample
  if sample then t.sample = SamplePool.serializeSample(sample) end
  return t
end

function ManualLoops:deserialize(t)
  Unit.deserialize(self, t)
  if t.sample then
    local sample = SamplePool.deserializeSample(t.sample)
    if sample then
      self:setSample(sample)
    else
      local Utils = require "Utils"
      app.logInfo("%s:deserialize: failed to load sample.", self)
      Utils.pp(t.sample)
    end
  end
end

function ManualLoops:setSample(sample)
  if self.sample then
    self.sample:release(self)
    self.sample = nil
  end

  self.sample = sample
  if self.sample then self.sample:claim(self) end

  if sample then
    self.objects.head:setSample(sample.pSample)
  else
    self.objects.head:setSample(nil)
  end

  if self.sampleEditor then self.sampleEditor:setSample(sample) end

  self:notifyControls("setSample", sample)
end

function ManualLoops:doDetachSample()
  local Overlay = require "Overlay"
  Overlay.mainFlashMessage("Sample detached.")
  self:setSample()
end

function ManualLoops:doAttachSampleFromCard()
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

function ManualLoops:doAttachSampleFromPool()
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

function ManualLoops:showSampleEditor()
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
  "editSample",
  "optionsHeader",
  "routing",
  "interpolation"
}

function ManualLoops:onShowMenu(objects, branches)
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

  controls.optionsHeader = MenuHeader {
    description = "Playback Options"
  }

  controls.routing = OptionControl {
    description = "Stereo-to-Mono Routing",
    option = objects.head:getOption("Routing"),
    choices = {
      "left",
      "sum",
      "right"
    },
    descriptionWidth = 2,
    muteOnChange = true
  }

  controls.interpolation = OptionControl {
    description = "Interpolation Quality",
    option = objects.head:getOption("Interpolation"),
    choices = {
      "none",
      "linear",
      "2nd order"
    },
    descriptionWidth = 2
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
    "sync",
    "pitch",
    "speed",
    "lstart",
    "length",
    "maxLength",
    "fade"
  },
  sync = {
    "wave",
    "sync"
  },
  pitch = {
    "wave",
    "pitch"
  },
  speed = {
    "wave",
    "speed"
  },
  lstart = {
    "wave",
    "lstart"
  },
  length = {
    "wave",
    "length"
  },
  maxLength = {
    "wave",
    "maxLength"
  },
  collapsed = {}
}

function ManualLoops:onLoadViews(objects, branches)
  local controls = {}

  controls.wave = WaveForm {
    head = objects.head
  }

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

  controls.sync = Gate {
    button = "sync",
    description = "Sync",
    branch = branches.sync,
    comparator = objects.sync
  }

  controls.lstart = GainBias {
    button = "start",
    description = "Loop Start",
    branch = branches.start,
    gainbias = objects.lstart,
    range = objects.lstart,
    biasMap = Encoder.getMap("unit"),
    initialBias = 0.0
  }

  controls.length = GainBias {
    button = "len",
    description = "Loop Length",
    branch = branches.length,
    gainbias = objects.length,
    range = objects.length,
    biasMap = Encoder.getMap("[-1,1]"),
    initialBias = 1.0
  }

  controls.maxLength = Fader {
    button = "max.len",
    description = "Max Loop Length",
    param = objects.head:getParameter("Maximum Loop Duration"),
    map = Encoder.getMap("[0,36]"),
    units = app.unitSecs,
    initial = 5
  }
  controls.maxLength:setTextAbove(35.9, "inf")

  controls.fade = Fader {
    button = "fade",
    description = "Fade Time",
    param = objects.head:getParameter("Fade Time"),
    monitor = self,
    map = Encoder.getMap("[0,0.25]"),
    units = app.unitSecs
  }

  return controls, views
end

function ManualLoops:onRemove()
  self:setSample(nil)
  Unit.onRemove(self)
end

return ManualLoops
