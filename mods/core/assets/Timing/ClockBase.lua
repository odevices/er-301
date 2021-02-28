local Class = require "Base.Class"
local Unit = require "Unit"
local OptionControl = require "Unit.MenuControl.OptionControl"
local GainBias = require "Unit.ViewControl.GainBias"
local InputGate = require "Unit.ViewControl.InputGate"
local Encoder = require "Encoder"

local ClockBase = Class {}
ClockBase:include(Unit)

function ClockBase:init(args)
  args.title = "Clock"
  args.mnemonic = "C"
  Unit.init(self, args)
end

function ClockBase:loadBaseGraph(channelCount, clock)
  local syncEdge = self:addObject("syncEdge", app.Comparator())
  local width = self:addObject("width", app.ParameterAdapter())
  local multiplier = self:addObject("multiplier", app.ParameterAdapter())
  local divider = self:addObject("divider", app.ParameterAdapter())

  connect(self, "In1", syncEdge, "In")
  connect(clock, "Out", self, "Out1")
  connect(syncEdge, "Out", clock, "Sync")

  tie(clock, "Pulse Width", width, "Out")
  tie(clock, "Multiplier", multiplier, "Out")
  tie(clock, "Divider", divider, "Out")

  self:addMonoBranch("width", width, "In", width, "Out")
  self:addMonoBranch("multiplier", multiplier, "In", multiplier, "Out")
  self:addMonoBranch("divider", divider, "In", divider, "Out")

  if channelCount > 1 then connect(self.objects.clock, "Out", self, "Out2") end
end

function ClockBase:loadBaseView(objects, branches)
  local controls = {}

  controls.sync = InputGate {
    button = "sync",
    description = "Sync",
    comparator = objects.syncEdge
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

  return controls
end

function ClockBase:setAny()
  local map = Encoder.getMap("[1,32]")
  self.controls.mult:setBiasMap(map)
  self.controls.mult:setBiasPrecision(3)
  self.controls.div:setBiasMap(map)
  self.controls.div:setBiasPrecision(3)
end

function ClockBase:setRational()
  local map = Encoder.getMap("int[1,32]")
  self.controls.mult:setBiasMap(map)
  self.controls.mult:setBiasPrecision(0)
  self.controls.div:setBiasMap(map)
  self.controls.div:setBiasPrecision(0)
end

local menu = {
  "rational"
}

function ClockBase:onShowMenu(objects, branches)
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

function ClockBase:deserialize(t)
  Unit.deserialize(self, t)
  local Serialization = require "Persist.Serialization"
  local rational = Serialization.get("objects/clock/options/Rational", t)
  if rational and rational == 0 then self:setAny() end
end

return ClockBase
