local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Task = require "Unit.MenuControl.Task"
local MenuHeader = require "Unit.MenuControl.Header"
local Encoder = require "Encoder"
local SamplePool = require "Sample.Pool"
local SamplePoolInterface = require "Sample.Pool.Interface"
local SampleEditor = require "Sample.Editor"
local WaveForm = require "core.Player.WaveForm"
local OptionControl = require "Unit.MenuControl.OptionControl"

local SampleScanner = Class {}
SampleScanner:include(Unit)

function SampleScanner:init(args)
  args.title = "Sample Scanner"
  args.mnemonic = "SS"
  Unit.init(self, args)
end

function SampleScanner:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function SampleScanner:loadMonoGraph()
  local bump = self:addObject("bump1", libcore.BumpMap())
  local center = self:addObject("center", app.ParameterAdapter())
  local width = self:addObject("width", app.ParameterAdapter())
  local height = self:addObject("height", app.ParameterAdapter())
  local fade = self:addObject("fade", app.ParameterAdapter())
  local phase = self:addObject("phase", app.GainBias())
  local phaseRange = self:addObject("phaseRange", app.MinMax())

  connect(self, "In1", bump, "In")
  connect(bump, "Out", self, "Out1")
  connect(phase, "Out", bump, "Phase")
  connect(phase, "Out", phaseRange, "In")

  tie(bump, "Center", center, "Out")
  tie(bump, "Width", width, "Out")
  tie(bump, "Height", height, "Out")
  tie(bump, "Fade", fade, "Out")

  self:addMonoBranch("center", center, "In", center, "Out")
  self:addMonoBranch("width", width, "In", width, "Out")
  self:addMonoBranch("height", height, "In", height, "Out")
  self:addMonoBranch("fade", fade, "In", fade, "Out")
  self:addMonoBranch("phase", phase, "In", phase, "Out")
end

function SampleScanner:loadStereoGraph()
  local bump1 = self:addObject("bump1", libcore.BumpMap())
  local bump2 = self:addObject("bump2", libcore.BumpMap())
  local center = self:addObject("center", app.ParameterAdapter())
  local width = self:addObject("width", app.ParameterAdapter())
  local height = self:addObject("height", app.ParameterAdapter())
  local fade = self:addObject("fade", app.ParameterAdapter())
  local phase = self:addObject("phase", app.GainBias())
  local phaseRange = self:addObject("phaseRange", app.MinMax())

  connect(self, "In1", bump1, "In")
  connect(self, "In2", bump2, "In")
  connect(bump1, "Out", self, "Out1")
  connect(bump2, "Out", self, "Out2")
  connect(phase, "Out", bump1, "Phase")
  connect(phase, "Out", bump2, "Phase")
  connect(phase, "Out", phaseRange, "In")

  tie(bump1, "Center", center, "Out")
  tie(bump1, "Width", width, "Out")
  tie(bump1, "Height", height, "Out")
  tie(bump1, "Fade", fade, "Out")
  tie(bump2, "Center", center, "Out")
  tie(bump2, "Width", width, "Out")
  tie(bump2, "Height", height, "Out")
  tie(bump2, "Fade", fade, "Out")
  tie(bump2, "Interpolation", bump1, "Interpolation")

  self:addMonoBranch("center", center, "In", center, "Out")
  self:addMonoBranch("width", width, "In", width, "Out")
  self:addMonoBranch("height", height, "In", height, "Out")
  self:addMonoBranch("fade", fade, "In", fade, "Out")
  self:addMonoBranch("phase", phase, "In", phase, "Out")
end

function SampleScanner:setSample(sample)
  if self.sample then
    self.sample:release(self)
    self.sample = nil
  end
  self.sample = sample
  if self.sample then self.sample:claim(self) end

  if self.channelCount == 1 then
    if sample == nil or sample:getChannelCount() == 0 then
      self.objects.bump1:setSample(nil, 0)
    elseif sample:getChannelCount() == 1 then
      self.objects.bump1:setSample(sample.pSample, 0)
    else -- 2 or more channels
      self.objects.bump1:setSample(sample.pSample, 0)
    end
  else
    if sample == nil or sample:getChannelCount() == 0 then
      self.objects.bump1:setSample(nil, 0)
      self.objects.bump2:setSample(nil, 0)
    elseif sample:getChannelCount() == 1 then
      self.objects.bump1:setSample(sample.pSample, 0)
      self.objects.bump2:setSample(sample.pSample, 0)
    else -- 2 or more channels
      self.objects.bump1:setSample(sample.pSample, 0)
      self.objects.bump2:setSample(sample.pSample, 1)
    end
  end
  if self.sampleEditor then self.sampleEditor:setSample(sample) end
  self:notifyControls("setSample", sample)
end

function SampleScanner:showSampleEditor()
  if self.sample then
    if self.sampleEditor == nil then
      self.sampleEditor = SampleEditor(self, self.objects.bump1)
      self.sampleEditor:setSample(self.sample)
      self.sampleEditor:setPointerLabel("X")
    end
    self.sampleEditor:show()
  else
    local Overlay = require "Overlay"
    Overlay.mainFlashMessage("You must first select a sample.")
  end
end

function SampleScanner:doDetachSample()
  local Overlay = require "Overlay"
  Overlay.mainFlashMessage("Sample detached.")
  self:setSample()
end

function SampleScanner:doAttachSampleFromCard()
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

function SampleScanner:doAttachSampleFromPool()
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

local menu = {
  "sampleHeader",
  "selectFromCard",
  "selectFromPool",
  "detachBuffer",
  "editSample",
  "interpolation"
}

function SampleScanner:onShowMenu(objects, branches)
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

  controls.interpolation = OptionControl {
    description = "Interpolation Quality",
    option = objects.bump1:getOption("Interpolation"),
    choices = {
      "none",
      "linear"
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
    "center",
    "width",
    "height",
    "fade",
    "phase"
  },
  collapsed = {},
  center = {
    "wave",
    "center"
  },
  phase = {
    "wave",
    "phase"
  }
}

function SampleScanner:onLoadViews(objects, branches)
  local controls = {}

  controls.wave = WaveForm {
    head = self.objects.bump1,
    label = "X"
  }

  controls.center = GainBias {
    button = "center",
    branch = branches.center,
    description = "Center",
    gainbias = objects.center,
    range = objects.center,
    biasMap = Encoder.getMap("[-1,1]"),
    biasUnits = app.unitNone,
    initialBias = 0.0,
    gainMap = Encoder.getMap("gain")
  }

  controls.width = GainBias {
    button = "width",
    branch = branches.width,
    description = "Width",
    gainbias = objects.width,
    range = objects.width,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 0.5,
    gainMap = Encoder.getMap("gain")
  }

  controls.height = GainBias {
    button = "height",
    branch = branches.height,
    description = "Height",
    gainbias = objects.height,
    range = objects.height,
    biasMap = Encoder.getMap("[-1,1]"),
    biasUnits = app.unitNone,
    initialBias = 0.5,
    gainMap = Encoder.getMap("gain")
  }

  controls.fade = GainBias {
    button = "fade",
    branch = branches.fade,
    description = "Fade",
    gainbias = objects.fade,
    range = objects.fade,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 0.25,
    gainMap = Encoder.getMap("gain")
  }

  controls.phase = GainBias {
    button = "phase",
    branch = branches.phase,
    description = "Phase",
    gainbias = objects.phase,
    range = objects.phaseRange,
    biasMap = Encoder.getMap("[-1,1]"),
    biasUnits = app.unitNone,
    initialBias = 0,
    gainMap = Encoder.getMap("gain")
  }
  return controls, views
end

function SampleScanner:serialize()
  local t = Unit.serialize(self)
  local sample = self.sample
  if sample then t.sample = SamplePool.serializeSample(sample) end
  return t
end

function SampleScanner:deserialize(t)
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

function SampleScanner:onRemove()
  self:setSample(nil)
  Unit.onRemove(self)
end

return SampleScanner
