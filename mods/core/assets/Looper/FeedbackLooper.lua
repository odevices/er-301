local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local SamplePoolInterface = require "Sample.Pool.Interface"
local Encoder = require "Encoder"
local Fader = require "Unit.ViewControl.Fader"
local Gate = require "Unit.ViewControl.Gate"
local GainBias = require "Unit.ViewControl.GainBias"
local SamplePool = require "Sample.Pool"
local RecordingView = require "core.Looper.RecordingView"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"
local OptionControl = require "Unit.MenuControl.OptionControl"
local ply = app.SECTION_PLY

local FeedbackLooper = Class {}
FeedbackLooper:include(Unit)

function FeedbackLooper:init(args)
  args.title = "Feedback Looper"
  args.mnemonic = "FL"
  Unit.init(self, args)
end

function FeedbackLooper:onLoadGraph(channelCount)
  local head = self:addObject("head", libcore.FeedbackLooper(channelCount))
  local reset = self:addObject("reset", app.Comparator())

  local punch = self:addObject("punch", app.Comparator())
  punch:setToggleMode()

  local engage = self:addObject("engage", app.Comparator())
  engage:setToggleMode()

  local feedback = self:addObject("feedback", app.GainBias())
  local feedbackRange = self:addObject("feedbackRange", app.MinMax())
  local feedbackClipper = self:addObject("feedbackClipper", libcore.Clipper())
  feedbackClipper:setMaximum(1.0)
  feedbackClipper:setMinimum(0.0)

  local start = self:addObject("start", app.ParameterAdapter())
  tie(head, "Start", start, "Out")

  connect(reset, "Out", head, "Reset")
  connect(punch, "Out", head, "Punch")
  connect(engage, "Out", head, "Engage")

  connect(feedback, "Out", feedbackClipper, "In")
  connect(feedbackClipper, "Out", head, "Feedback")
  connect(feedbackClipper, "Out", feedbackRange, "In")

  local wet = self:addObject("wet", app.GainBias())
  local wetRange = self:addObject("wetRange", app.MinMax())
  connect(wet, "Out", wetRange, "In")

  if channelCount < 2 then
    local xfade = self:addObject("xfade", app.CrossFade())
    connect(self, "In1", head, "Left In")
    connect(head, "Left Out", xfade, "A")
    connect(self, "In1", xfade, "B")
    connect(wet, "Out", xfade, "Fade")
    connect(xfade, "Out", self, "Out1")
  else
    local xfade = self:addObject("xfade", app.StereoCrossFade())
    connect(self, "In1", head, "Left In")
    connect(self, "In2", head, "Right In")
    connect(head, "Left Out", xfade, "Left A")
    connect(head, "Right Out", xfade, "Right A")
    connect(self, "In1", xfade, "Left B")
    connect(self, "In2", xfade, "Right B")
    connect(wet, "Out", xfade, "Fade")
    connect(xfade, "Left Out", self, "Out1")
    connect(xfade, "Right Out", self, "Out2")
  end

  self:addMonoBranch("reset", reset, "In", reset, "Out")
  self:addMonoBranch("engage", engage, "In", engage, "Out")
  self:addMonoBranch("punch", punch, "In", punch, "Out")
  self:addMonoBranch("feedback", feedback, "In", feedback, "Out")
  self:addMonoBranch("wet", wet, "In", wet, "Out")
  self:addMonoBranch("start", start, "In", start, "Out")
end

function FeedbackLooper:setSample(sample)
  if self.sample then
    self.sample:release(self)
  end
  self.sample = sample
  if self.sample then
    self.sample:claim(self)
    self.objects.head:setSample(sample.pSample)
  end
  self:notifyControls("setSample", sample)
end

function FeedbackLooper:doCreateBuffer()
  local Creator = require "Sample.Pool.Creator"
  local creator = Creator(self.channelCount)
  local task = function(sample)
    if sample then
      local Overlay = require "Overlay"
      Overlay.flashMainMessage("Attached buffer: %s", sample.name)
      self:setSample(sample)
    end
  end
  creator:subscribe("done", task)
  creator:show()
end

function FeedbackLooper:doAttachBufferFromPool()
  local chooser = SamplePoolInterface(self.loadInfo.id, "choose")
  chooser:setDefaultChannelCount(self.channelCount)
  chooser:highlight(self.sample)
  local task = function(sample)
    if sample then
      local Overlay = require "Overlay"
      Overlay.flashMainMessage("Attached buffer: %s", sample.name)
      self:setSample(sample)
    end
  end
  chooser:subscribe("done", task)
  chooser:show()
end

function FeedbackLooper:doDetachBuffer()
  local Overlay = require "Overlay"
  Overlay.flashMainMessage("Buffer detached.")
  self:setSample()
end

function FeedbackLooper:doZeroBuffer()
  local Overlay = require "Overlay"
  Overlay.flashMainMessage("Buffer zeroed.")
  self.objects.head:zeroBuffer()
end

function FeedbackLooper:showSampleEditor()
  if self.sample then
    if self.sampleEditor == nil then
      local SampleEditor = require "Sample.Editor"
      self.sampleEditor = SampleEditor(self, self.objects.head)
      self.sampleEditor:setSample(self.sample)
      self.sampleEditor:setPointerLabel("R")
    end
    self.sampleEditor:show()
  else
    local Overlay = require "Overlay"
    Overlay.flashMainMessage("You must first select a sample.")
  end
end

local menu = {
  "bufferHeader",
  "createNew",
  "attachExisting",
  "detachBuffer",
  "zeroBuffer",
  "controlHeader",
  "engageLatch",
  "punchLatch"
}

function FeedbackLooper:onShowMenu(objects, branches)
  local controls = {}

  controls.bufferHeader = MenuHeader {
    description = "Buffer Menu"
  }

  controls.createNew = Task {
    description = "Create New...",
    task = function()
      self:doCreateBuffer()
    end
  }

  controls.attachExisting = Task {
    description = "Attach Existing...",
    task = function()
      self:doAttachBufferFromPool()
    end
  }

  controls.detachBuffer = Task {
    description = "Detach Buffer",
    task = function()
      self:doDetachBuffer()
    end
  }

  controls.zeroBuffer = Task {
    description = "Zero Buffer",
    task = function()
      self:doZeroBuffer()
    end
  }

  controls.controlHeader = MenuHeader {
    description = "Control Configuration"
  }

  controls.engageLatch = OptionControl {
    description = "Engage Latch",
    option = objects.engage:getOption("Mode"),
    choices = {
      "on",
      "off"
    }
  }

  controls.punchLatch = OptionControl {
    description = "Punch Latch",
    option = objects.punch:getOption("Mode"),
    choices = {
      "on",
      "off"
    }
  }

  local sub = {}
  if self.sample then
    sub[1] = {
      position = app.GRID5_LINE1,
      justify = app.justifyLeft,
      text = "Attached Buffer:"
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
      text = "No buffer attached."
    }
  end

  return controls, menu, sub
end

local views = {
  expanded = {
    "reset",
    "engage",
    "punch",
    "start",
    "feedback",
    "wet",
    "fade"
  },
  collapsed = {},
  reset = {
    "wave",
    "reset"
  },
  engage = {
    "wave",
    "engage"
  },
  punch = {
    "wave",
    "punch"
  },
  start = {
    "wave",
    "start"
  },
  feedback = {
    "wave",
    "feedback"
  },
  wet = {
    "wave",
    "wet"
  },
  fade = {
    "wave",
    "fade"
  }
}

function FeedbackLooper:onLoadViews(objects, branches)
  local controls = {}

  controls.engage = Gate {
    button = "engage",
    description = "Engage Motor",
    branch = branches.engage,
    comparator = objects.engage
  }

  controls.punch = Gate {
    button = "punch",
    description = "Punch In/Out",
    branch = branches.punch,
    comparator = objects.punch
  }

  controls.reset = Gate {
    button = "reset",
    description = "Reset",
    branch = branches.reset,
    comparator = objects.reset
  }

  controls.start = GainBias {
    button = "start",
    description = "Loop Start",
    branch = branches.start,
    gainbias = objects.start,
    range = objects.start,
    biasMap = Encoder.getMap("default"),
    biasUnits = app.unitNone
  }

  controls.feedback = GainBias {
    button = "fdbk",
    description = "Feedback Amount",
    branch = branches.feedback,
    gainbias = objects.feedback,
    range = objects.feedbackRange,
    biasMap = Encoder.getMap("unit"),
    initialBias = 1.0
  }

  controls.wet = GainBias {
    button = "wet",
    description = "Wet/Dry Amount",
    branch = branches.wet,
    gainbias = objects.wet,
    range = objects.wetRange,
    biasMap = Encoder.getMap("unit"),
    initialBias = 0.5
  }

  controls.fade = Fader {
    button = "fade",
    description = "Fade Time",
    param = objects.head:getParameter("Fade Time"),
    map = Encoder.getMap("[0,0.25]"),
    units = app.unitSecs
  }

  controls.wave = RecordingView {
    width = 4 * ply,
    head = objects.head
  }

  return controls, views
end

function FeedbackLooper:serialize()
  local t = Unit.serialize(self)
  if self.sample then
    t.sample = SamplePool.serializeSample(self.sample)
  end
  return t
end

function FeedbackLooper:deserialize(t)
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

function FeedbackLooper:onRemove()
  self:setSample(nil)
  Unit.onRemove(self)
end

return FeedbackLooper
