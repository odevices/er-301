local app = app
local libcore = require "core.libcore"
local Env = require "Env"
local Class = require "Base.Class"
local Signal = require "Signal"
local Unit = require "Unit"
local Slices = require "Sample.Slices"
local Encoder = require "Encoder"
local Fader = require "Unit.ViewControl.Fader"
local GainBias = require "Unit.ViewControl.GainBias"
local Gate = require "Unit.ViewControl.Gate"
local FileTransport = require "Unit.ViewControl.FileTransport"
local OptionControl = require "Unit.MenuControl.OptionControl"
local Task = require "Unit.MenuControl.Task"
local Card = require "Card"
local ply = app.SECTION_PLY

local CardPlayerUnit = Class {}
CardPlayerUnit:include(Unit)

function CardPlayerUnit:init(args)
  args.title = "Card Player"
  args.mnemonic = "CP"
  Unit.init(self, args)
  self:setClassName("CardPlayerUnit")
  Signal.weakRegister("cardEjecting", self)
  Signal.weakRegister("cardMounted", self)
end

function CardPlayerUnit:cardEjecting()
  if self.filename then
    self.objects.source:close()
    Card.release(self.filename)
  end
end

function CardPlayerUnit:cardMounted()
  if self.filename then
    Card.claim(self:getClassName(), self.filename)
    self.objects.source:open(self.filename)
  end
end

function CardPlayerUnit:onLoadGraph(channelCount)
  local source

  if channelCount == 2 then
    source = self:addObject("source", app.FileSource(2))
    connect(source, "Left Out", self, "Out1")
    connect(source, "Right Out", self, "Out2")
  else
    source = self:addObject("source", app.FileSource(1))
    connect(source, "Out", self, "Out1")
  end

  local speed = self:addObject("speed", app.ParameterAdapter())
  speed:clamp(0, 3)
  speed:hardSet("Bias", 1.0)
  local position = self:addObject("position", app.ParameterAdapter())
  position:clamp(0, 1)
  local reset = self:addObject("reset", app.Comparator())

  tie(source, "Reset", reset, "Last Edge Count")
  tie(source, "Speed", speed, "Out")
  tie(source, "Position", position, "Out")

  self:addMonoBranch("speed", speed, "In", speed, "Out")
  self:addMonoBranch("position", position, "In", position, "Out")
  self:addMonoBranch("reset", reset, "In", reset, "Out")
end

function CardPlayerUnit:rewind()
  self.objects.position:hardSet("Out", 0.0)
  self.objects.reset:simulateRisingEdge()
  self.objects.reset:simulateFallingEdge()
end

function CardPlayerUnit:serialize()
  local t = Unit.serialize(self)
  t.filename = self.filename
  t.paused = self.objects.source:isPaused()
  return t
end

function CardPlayerUnit:deserialize(t)
  Unit.deserialize(self, t)
  if t.filename then self:setFilename(t.filename) end
  if t.paused then
    self:pause()
  else
    self:unpause()
  end
end

function CardPlayerUnit:setFilename(filename)
  if self.filename then Card.release(self.filename) end
  self:notifyControls("onFileChanged", filename)
  Card.claim(self:getClassName(), filename)
  self.filename = filename
  self.objects.source:open(filename)
end

function CardPlayerUnit:doLoadFile()
  local task = function(result)
    if result and result.fullpath then self:setFilename(result.fullpath) end
  end
  local FileChooser = require "Card.FileChooser"
  local FS = require "Card.FileSystem"
  local chooser = FileChooser {
    msg = "Choose Sound File",
    goal = "load file",
    pattern = FS.getPattern("audio-strict"),
    history = "cardPlayerLoadSoundFile"
  }
  chooser:subscribe("done", task)
  chooser:show()
end

local menu = {
  "attach",
  "howOften",
  "infoHeader"
}

function CardPlayerUnit:onShowMenu(objects, branches)
  local controls = {}

  controls.attach = Task {
    description = "Load File",
    task = function()
      self:doLoadFile()
    end
  }

  controls.howOften = OptionControl {
    description = "Play Duration",
    option = objects.source:getOption("How Often"),
    choices = {
      "once",
      "loop"
    }
  }
  return controls, menu
end

local views = {
  expanded = {
    "transport",
    "speed",
    "position",
    "reset"
  },
  collapsed = {}
}

function CardPlayerUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.transport = FileTransport {
    width = 2 * ply,
    source = self.objects.source,
    monitor = self
  }

  controls.speed = GainBias {
    button = "speed",
    description = "Speed",
    branch = branches.speed,
    gainbias = objects.speed,
    range = objects.speed,
    biasMap = Encoder.getMap("[0,2]")
  }

  controls.position = GainBias {
    button = "pos",
    description = "Position",
    branch = branches.position,
    gainbias = objects.position,
    range = objects.position,
    biasMap = Encoder.getMap("unit")
  }

  controls.reset = Gate {
    button = "reset",
    description = "Reset",
    branch = branches.reset,
    comparator = objects.reset
  }

  return controls, views
end

function CardPlayerUnit:closeFile()
  self.objects.source:close()
  if self.filename then
    Card.release(self.filename)
    self.filename = nil
    self:notifyControls("onFileChanged")
  end
end

function CardPlayerUnit:onRemove()
  self:closeFile()
  Unit.onRemove(self)
end

function CardPlayerUnit:isPaused()
  return self.objects.source:isPaused()
end

function CardPlayerUnit:pause()
  self.objects.source:pause()
  self:notifyControls("onPause")
end

function CardPlayerUnit:unpause()
  self.objects.source:unpause()
  self:notifyControls("onUnpause")
end

return CardPlayerUnit
