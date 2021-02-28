local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local SlicingView = require "SlicingView"
local SamplePool = require "Sample.Pool"
local SamplePoolInterface = require "Sample.Pool.Interface"
local Encoder = require "Encoder"
local Fader = require "Unit.ViewControl.Fader"
local Gate = require "Unit.ViewControl.Gate"
local Clock = require "Unit.ViewControl.Clock"
local Pitch = require "Unit.ViewControl.Pitch"
local GainBias = require "Unit.ViewControl.GainBias"
local WaveForm = require "core.Player.SlicedWaveForm"
local OptionControl = require "Unit.MenuControl.OptionControl"
local ModeControl = require "Unit.MenuControl.ModeControl"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"

local ClockedStretch = Class {}
ClockedStretch:include(Unit)

function ClockedStretch:init(args)
  args.title = "Clocked Stretch"
  args.mnemonic = "CS"
  Unit.init(self, args)
end

function ClockedStretch:onLoadGraph(channelCount)
  local head = self:addObject("head", libcore.GrainStretch(channelCount, 8))
  local trig = self:addObject("trig", app.Comparator())
  local tune = self:addObject("tune", app.ParameterAdapter())
  tune:hardSet("Gain", 1.0)
  local duration = self:addObject("duration", app.ParameterAdapter())
  local slice = self:addObject("slice", app.GainBias())
  local shift = self:addObject("shift", app.GainBias())
  local sliceRange = self:addObject("sliceRange", app.MinMax())
  local shiftRange = self:addObject("shiftRange", app.MinMax())

  local clock = self:addObject("clock", app.Comparator())
  local tap = self:addObject("tap", libcore.TapTempo())
  connect(clock, "Out", tap, "In")
  local speed = self:addObject("speed", app.Constant())

  tie(head, "Sustain", trig, "State")
  tie(head, "Grain Pitch", tune, "Out")
  tie(head, "Grain Duration", duration, "Out")

  connect(speed, "Out", head, "Speed")

  connect(trig, "Out", head, "Trigger")

  connect(slice, "Out", head, "Slice Select")
  connect(slice, "Out", sliceRange, "In")
  connect(shift, "Out", head, "Slice Shift")
  connect(shift, "Out", shiftRange, "In")

  connect(head, "Left Out", self, "Out1")

  self:addMonoBranch("clock", clock, "In", clock, "Out")
  self:addMonoBranch("tune", tune, "In", tune, "Out")
  self:addMonoBranch("duration", duration, "In", duration, "Out")
  self:addMonoBranch("trig", trig, "In", trig, "Out")
  self:addMonoBranch("slice", slice, "In", slice, "Out")
  self:addMonoBranch("shift", shift, "In", shift, "Out")

  if channelCount > 1 then connect(head, "Right Out", self, "Out2") end

  self:setStretchMode("slice")
end

function ClockedStretch:setStretchMode(mode)
  mode = mode or "slice"
  local o = self.objects
  if mode == "all" then
    tie(o.speed, "Value", "*", o.head, "Sample Duration", o.tap,
        "Derived Frequency")
  elseif mode == "slice" then
    tie(o.speed, "Value", "*", o.head, "Actual Play Duration", o.tap,
        "Derived Frequency")
  end
  self.stretchMode = mode
end

function ClockedStretch:serialize()
  local t = Unit.serialize(self)
  local sample = self.sample
  if sample then
    t.sample = SamplePool.serializeSample(sample)
    local head = self.objects.head
    t.activeSliceIndex = head:getActiveSliceIndex()
    t.activeSliceShift = head:getActiveSliceShift()
    t.samplePosition = head:getPosition()
  end
  t.stretchMode = self.stretchMode
  return t
end

function ClockedStretch:deserialize(t)
  Unit.deserialize(self, t)
  if t.sample then
    local sample = SamplePool.deserializeSample(t.sample)
    if sample then
      self:setSample(sample)
      local head = self.objects.head
      local sliceIndex = t.activeSliceIndex
      local sliceShift = t.activeSliceShift or 0
      local samplePosition = t.samplePosition
      if sliceIndex and samplePosition then
        head:setActiveSlice(sliceIndex, sliceShift)
        head:setPosition(samplePosition)
      end
    else
      local Utils = require "Utils"
      app.logInfo("%s:deserialize: failed to load sample.", self)
      Utils.pp(t.sample)
    end
  end
  self:setStretchMode(t.stretchMode)
end

function ClockedStretch:setSample(sample)
  if self.sample then
    self.sample:release(self)
    self.sample = nil
  end
  self.sample = sample
  if self.sample then self.sample:claim(self) end

  if sample == nil or sample:getChannelCount() == 0 then
    self.objects.head:setSample(nil, nil)
  else
    self.objects.head:setSample(sample.pSample, sample.slices.pSlices)
  end

  if self.slicingView then self.slicingView:setSample(sample) end
  self:notifyControls("setSample", sample)
end

function ClockedStretch:doDetachSample()
  local Overlay = require "Overlay"
  Overlay.mainFlashMessage("Sample detached.")
  self:setSample()
end

function ClockedStretch:doAttachSampleFromCard()
  local task = function(sample)
    if sample then
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Attached sample: %s", sample.name)
      self:setSample(sample)
      if sample:hasNonTrivialSlices() then
        self.objects.head:setOptionValue("How Much", app.HOWMUCH_SLICE)
      end
    end
  end
  local Pool = require "Sample.Pool"
  local loop = self.objects.head:getOptionValue("How Often") ==
                   app.HOWOFTEN_LOOP
  Pool.chooseFileFromCard(self.loadInfo.id, task, loop)
end

function ClockedStretch:doAttachSampleFromPool()
  local chooser = SamplePoolInterface(self.loadInfo.id, "choose")
  chooser:setDefaultChannelCount(self.channelCount)
  chooser:highlight(self.sample)
  local task = function(sample)
    if sample then
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Attached sample: %s", sample.name)
      self:setSample(sample)
      if sample:hasNonTrivialSlices() then
        self.objects.head:setOptionValue("How Much", app.HOWMUCH_SLICE)
      end
    end
  end
  chooser:subscribe("done", task)
  chooser:show()
end

function ClockedStretch:showSampleEditor()
  if self.sample then
    if self.slicingView == nil then
      self.slicingView = SlicingView(self, self.objects.head)
      self.slicingView:setSample(self.sample)
    end
    self.slicingView:show()
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
  "sliceBuffer",
  "optionsHeader",
  "howOften",
  "howMuch",
  "stretch",
  "polarity",
  "address"
}

function ClockedStretch:onShowMenu(objects, branches)
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

  controls.sliceBuffer = Task {
    description = "Edit Buffer",
    task = function()
      self:showSampleEditor()
    end
  }

  controls.optionsHeader = MenuHeader {
    description = "Playback Options"
  }

  controls.howOften = OptionControl {
    description = "Play Duration",
    option = objects.head:getOption("How Often"),
    choices = {
      "once",
      "repeat",
      "loop on gate hi"
    }
  }

  controls.howMuch = OptionControl {
    description = "Play Extent",
    option = objects.head:getOption("How Much"),
    choices = {
      "all",
      "slice",
      "cue"
    }
  }

  controls.stretch = ModeControl {
    description = "Stretch Duration",
    callback = function(choice)
      self:setStretchMode(choice)
    end,
    choices = {
      "all",
      "slice"
    },
    current = self.stretchMode
  }

  controls.polarity = OptionControl {
    description = "Slice Polarity",
    option = objects.head:getOption("Polarity"),
    choices = {
      "left",
      "symmetric",
      "right"
    }
  }

  controls.address = OptionControl {
    description = "CV-to-Slice Mapping",
    option = objects.head:getOption("Address"),
    choices = {
      "nearest",
      "index",
      "12TET"
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
    "clock",
    "pitch",
    "trigger",
    "slice",
    "shift",
    "duration",
    "jitter"
  },
  collapsed = {},
  pitch = {
    "wave",
    "pitch"
  },
  clock = {
    "wave",
    "clock"
  },
  trigger = {
    "wave",
    "trigger"
  },
  slice = {
    "wave",
    "slice"
  },
  shift = {
    "wave",
    "shift"
  },
  duration = {
    "wave",
    "duration"
  },
  jitter = {
    "wave",
    "jitter"
  }
}

function ClockedStretch:onLoadViews(objects, branches)
  local controls = {}

  controls.pitch = Pitch {
    button = "V/oct",
    description = "V/oct",
    branch = branches.tune,
    offset = objects.tune,
    range = objects.tune
  }

  controls.clock = Clock {
    button = "clock",
    description = "Tempo Clock",
    branch = branches.clock,
    comparator = objects.clock,
    tap = objects.tap
  }

  controls.trigger = Gate {
    button = "trig",
    description = "Trigger",
    branch = branches.trig,
    comparator = objects.trig
  }

  controls.slice = GainBias {
    button = "slice",
    description = "Slice Select",
    branch = branches.slice,
    gainbias = objects.slice,
    range = objects.sliceRange,
    biasMap = Encoder.getMap("unit")
  }

  controls.shift = GainBias {
    button = "shift",
    description = "Slice Shift",
    branch = branches.shift,
    gainbias = objects.shift,
    range = objects.shiftRange,
    biasMap = Encoder.getMap("[-5,5]"),
    biasUnits = app.unitSecs
  }

  controls.duration = GainBias {
    button = "dur",
    description = "Grain Duration",
    branch = branches.duration,
    gainbias = objects.duration,
    range = objects.duration,
    biasMap = Encoder.getMap("[0,0.1]"),
    biasUnits = app.unitSecs,
    initialBias = 0.07
  }

  controls.jitter = Fader {
    button = "jitter",
    description = "Grain Jitter",
    param = objects.head:getParameter("Grain Jitter"),
    map = Encoder.getMap("unit"),
    units = app.unitNone
  }

  controls.wave = WaveForm {
    head = objects.head
  }

  return controls, views
end

function ClockedStretch:onRemove()
  self:setSample(nil)
  Unit.onRemove(self)
end

return ClockedStretch
