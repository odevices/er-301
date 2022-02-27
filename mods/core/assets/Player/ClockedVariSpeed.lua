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
local GainBias = require "Unit.ViewControl.GainBias"
local WaveForm = require "core.Player.SlicedWaveForm"
local OptionControl = require "Unit.MenuControl.OptionControl"
local ModeControl = require "Unit.MenuControl.ModeControl"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"

local ClockedVariSpeed = Class {}
ClockedVariSpeed:include(Unit)

function ClockedVariSpeed:init(args)
  args.title = "Clocked Player"
  args.mnemonic = "CP"
  Unit.init(self, args)
end

function ClockedVariSpeed:onLoadGraph(channelCount)
  local head = self:addObject("head", libcore.VariSpeedHead(channelCount))
  local trig = self:addObject("trig", app.Comparator())
  local slice = self:addObject("slice", app.GainBias())
  local shift = self:addObject("shift", app.GainBias())
  local sliceRange = self:addObject("sliceRange", app.MinMax())
  local shiftRange = self:addObject("shiftRange", app.MinMax())
  connect(trig, "Out", head, "Trigger")
  connect(slice, "Out", head, "Slice Select")
  connect(slice, "Out", sliceRange, "In")
  connect(shift, "Out", head, "Slice Shift")
  connect(shift, "Out", shiftRange, "In")
  tie(head, "Sustain", trig, "State")
  self:addMonoBranch("trig", trig, "In", trig, "Out")
  self:addMonoBranch("slice", slice, "In", slice, "Out")
  self:addMonoBranch("shift", shift, "In", shift, "Out")

  -- clock-controlled speed
  local clock = self:addObject("clock", app.Comparator())
  local tap = self:addObject("tap", libcore.TapTempo())
  connect(clock, "Out", tap, "In")
  local speed = self:addObject("speed", app.Constant())
  connect(speed, "Out", head, "Speed")

  self:addMonoBranch("clock", clock, "In", clock, "Out")

  connect(head, "Left Out", self, "Out1")
  if channelCount > 1 then
    connect(head, "Right Out", self, "Out2")
  end

  self:setStretchMode("slice")
end

function ClockedVariSpeed:setStretchMode(mode)
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

function ClockedVariSpeed:serialize()
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

function ClockedVariSpeed:deserialize(t)
  Unit.deserialize(self, t)
  if t.sample then
    local sample = SamplePool.deserializeSample(t.sample, self.chain)
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
      app.logError("%s:deserialize: failed to load sample.", self)
      Utils.pp(t.sample)
    end
  end
  self:setStretchMode(t.stretchMode)
end

function ClockedVariSpeed:setSample(sample)
  if self.sample then
    self.sample:release(self)
  end
  self.sample = sample
  if self.sample then
    self.sample:claim(self)
  end

  if sample then
    self.objects.head:setSample(sample.pSample, sample.slices.pSlices)
  else
    self.objects.head:setSample(nil, nil)
  end

  if self.slicingView then
    self.slicingView:setSample(sample)
  end
  self:notifyControls("setSample", sample)
end

function ClockedVariSpeed:doDetachSample()
  local Overlay = require "Overlay"
  Overlay.flashMainMessage("Sample detached.")
  self:setSample()
end

function ClockedVariSpeed:doAttachSampleFromCard()
  local task = function(sample)
    if sample then
      local Overlay = require "Overlay"
      Overlay.flashMainMessage("Attached sample: %s", sample.name)
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

function ClockedVariSpeed:doAttachSampleFromPool()
  local chooser = SamplePoolInterface(self.loadInfo.id, "choose")
  chooser:setDefaultChannelCount(self.channelCount)
  chooser:highlight(self.sample)
  local task = function(sample)
    if sample then
      local Overlay = require "Overlay"
      Overlay.flashMainMessage("Attached sample: %s", sample.name)
      self:setSample(sample)
      if sample:hasNonTrivialSlices() then
        self.objects.head:setOptionValue("How Much", app.HOWMUCH_SLICE)
      end
    end
  end
  chooser:subscribe("done", task)
  chooser:show()
end

function ClockedVariSpeed:showSampleEditor()
  if self.sample then
    if self.slicingView == nil then
      self.slicingView = SlicingView(self, self.objects.head)
      self.slicingView:setSample(self.sample)
    end
    self.slicingView:show()
  else
    local Overlay = require "Overlay"
    Overlay.flashMainMessage("You must first select a sample.")
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
  "address",
  "routing",
  "interpolation"
}

function ClockedVariSpeed:onShowMenu(objects, branches)
  local controls = {}

  controls.sampleHeader = MenuHeader {
    description = "Sample Operations"
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
    description = "Detach",
    task = function()
      self:doDetachSample()
    end
  }

  controls.sliceBuffer = Task {
    description = "Slice",
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

  if self.channelCount == 1 then
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
  end

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
    "clock",
    "trigger",
    "slice",
    "shift",
    "fade"
  },
  collapsed = {},
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
  fade = {
    "wave",
    "fade"
  }
}

function ClockedVariSpeed:onLoadViews(objects, branches)
  local controls = {}

  controls.trigger = Gate {
    button = "gate",
    description = "Gate",
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

  controls.fade = Fader {
    button = "fade",
    description = "Fade Time",
    param = objects.head:getParameter("Fade Time"),
    map = Encoder.getMap("[0,0.25]"),
    units = app.unitSecs
  }

  controls.wave = WaveForm {
    head = objects.head
  }

  controls.clock = Clock {
    button = "clock",
    description = "Tempo Clock",
    branch = branches.clock,
    comparator = objects.clock,
    tap = objects.tap
  }

  return controls, views
end

function ClockedVariSpeed:onRemove()
  self:setSample(nil)
  Unit.onRemove(self)
end

return ClockedVariSpeed
