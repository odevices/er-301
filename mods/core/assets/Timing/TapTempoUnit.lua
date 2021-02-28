local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Gate = require "Unit.ViewControl.Gate"
local InputGate = require "Unit.ViewControl.InputGate"
local OptionControl = require "Unit.MenuControl.OptionControl"
local Encoder = require "Encoder"

local TapTempo = Class {}
TapTempo:include(Unit)

function TapTempo:init(args)
  args.title = "Tap Tempo"
  args.mnemonic = "TT"
  Unit.init(self, args)
end

function TapTempo:onLoadGraph(channelCount)
  local tap = self:addObject("tap", libcore.TapTempo())
  tap:setBaseTempo(120)
  local clock = self:addObject("clock", libcore.ClockInSeconds())
  local tapEdge = self:addObject("tapEdge", app.Comparator())
  local syncEdge = self:addObject("syncEdge", app.Comparator())
  local width = self:addObject("width", app.ParameterAdapter())
  local multiplier = self:addObject("multiplier", app.ParameterAdapter())
  local divider = self:addObject("divider", app.ParameterAdapter())

  connect(self, "In1", tapEdge, "In")
  connect(tapEdge, "Out", tap, "In")
  connect(clock, "Out", self, "Out1")
  connect(syncEdge, "Out", clock, "Sync")

  tie(clock, "Period", tap, "Base Period")
  tie(clock, "Pulse Width", width, "Out")
  tie(clock, "Multiplier", multiplier, "Out")
  tie(clock, "Divider", divider, "Out")

  self:addMonoBranch("sync", syncEdge, "In", syncEdge, "Out")
  self:addMonoBranch("width", width, "In", width, "Out")
  self:addMonoBranch("multiplier", multiplier, "In", multiplier, "Out")
  self:addMonoBranch("divider", divider, "In", divider, "Out")

  if channelCount > 1 then connect(self.objects.clock, "Out", self, "Out2") end
end

function TapTempo:setAny()
  local map = Encoder.getMap("[1,32]")
  self.controls.mult:setBiasMap(map)
  self.controls.mult:setBiasPrecision(3)
  self.controls.div:setBiasMap(map)
  self.controls.div:setBiasPrecision(3)
end

function TapTempo:setRational()
  local map = Encoder.getMap("int[1,32]")
  self.controls.mult:setBiasMap(map)
  self.controls.mult:setBiasPrecision(0)
  self.controls.div:setBiasMap(map)
  self.controls.div:setBiasPrecision(0)
end

local menu = {
  "rational"
}

function TapTempo:onShowMenu(objects, branches)
  local controls = {}

  controls.rational = OptionControl {
    description = "Allowed Mult/Div",
    option = objects.clock:getOption("Rational"),
    choices = {
      "any",
      "rational only"
    },
    boolean = true,
    onUpdate = function(choice)
      if choice == "any" then
        self:setAny()
      else
        self:setRational()
      end
    end
  }
  return controls, menu
end

function TapTempo:deserialize(t)
  Unit.deserialize(self, t)
  local Serialization = require "Persist.Serialization"
  local rational = Serialization.get("objects/clock/options/Rational", t)
  if rational and rational == 0 then self:setAny() end
end

local views = {
  expanded = {
    "tap",
    "mult",
    "div",
    "sync",
    "width"
  },
  collapsed = {}
}

function TapTempo:onLoadViews(objects, branches)
  local controls = {}

  controls.tap = InputGate {
    button = "tap",
    description = "Tap",
    comparator = objects.tapEdge
  }

  controls.mult = GainBias {
    button = "mult",
    description = "Clock Multiplier",
    branch = branches.multiplier,
    gainbias = objects.multiplier,
    range = objects.multiplier,
    biasMap = Encoder.getMap("int[1,32]"),
    biasPrecision = 0,
    initialBias = 1
  }

  controls.div = GainBias {
    button = "div",
    description = "Clock Divider",
    branch = branches.divider,
    gainbias = objects.divider,
    range = objects.divider,
    biasMap = Encoder.getMap("int[1,32]"),
    biasPrecision = 0,
    initialBias = 1
  }

  controls.sync = Gate {
    button = "sync",
    description = "Sync",
    branch = branches.sync,
    comparator = objects.syncEdge
  }

  controls.width = GainBias {
    button = "width",
    description = "Pulse Width",
    branch = branches.width,
    gainbias = objects.width,
    range = objects.width,
    biasMap = Encoder.getMap("unit"),
    biasUnits = app.unitNone,
    initialBias = 0.5
  }

  return controls, views
end

return TapTempo
