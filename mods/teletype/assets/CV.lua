local app = app
local teletype = require "teletype"
local libteletype = require "teletype.libteletype"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local CV = Class {}
CV:include(Unit)

function CV:init(args)
  args.title = "CV (i2c)"
  args.suppressTitleGeneration = true
  args.mnemonic = "CV"
  Unit.init(self, args)
end

function CV:onLoadGraph(channelCount)
  local cv = self:addObject("cv", libteletype.CV(teletype.dispatcher))
  connect(cv, "Out", self, "Out1")
  if channelCount > 1 then connect(cv, "Out", self, "Out2") end

  local port = self:addObject("port", app.ParameterAdapter())
  tie(cv, "Port", port, "Out")
  self:addMonoBranch("port", port, "In", port, "Out")
end

local views = {
  expanded = {
    "port"
  },
  collapsed = {}
}

function CV:onLoadViews(objects, branches)
  local controls = {}
  controls.port = GainBias {
    button = "port",
    description = "Port",
    branch = branches.port,
    gainbias = objects.port,
    range = objects.port,
    biasMap = Encoder.getMap("int[1,100]"),
    biasPrecision = 0,
    initialBias = 1
  }

  return controls, views
end

return CV
