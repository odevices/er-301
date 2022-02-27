local app = app
local teletype = require "teletype"
local libteletype = require "teletype.libteletype"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local TR = Class {}
TR:include(Unit)

function TR:init(args)
  args.title = "SC.TR (i2c)"
  args.suppressTitleGeneration = true
  args.mnemonic = "TR"
  Unit.init(self, args)
end

function TR:onLoadGraph(channelCount)
  local trig = self:addObject("trig", libteletype.TR(teletype.dispatcher))
  connect(trig, "Out", self, "Out1")
  if channelCount > 1 then
    connect(trig, "Out", self, "Out2")
  end

  local port = self:addObject("port", app.ParameterAdapter())
  tie(trig, "Port", port, "Out")
  self:addMonoBranch("port", port, "In", port, "Out")
end

local views = {
  expanded = {
    "port"
  },
  collapsed = {}
}

function TR:onLoadViews(objects, branches)
  local controls = {}
  controls.port = GainBias {
    button = "port",
    description = "Port",
    branch = branches.port,
    gainbias = objects.port,
    range = objects.port,
    biasMap = Encoder.getMap("int[0,100]"),
    biasPrecision = 0,
    initialBias = 1
  }
  controls.port:setTextBelow(1, "any")

  return controls, views
end

return TR
