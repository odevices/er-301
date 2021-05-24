local Class = require "Base.Class"
local Unit = require "Unit"
local Encoder = require "Encoder"
local GainBias = require "Unit.ViewControl.GainBias"
local Fader = require "Unit.ViewControl.Fader"

local TieExpression = Class {}
TieExpression:include(Unit)

function TieExpression:init(args)
    args.title = "Tie Expr Test"
    args.mnemonic = "TE"
    args.version = 1
    Unit.init(self, args)
end

function TieExpression:onLoadGraph(channelCount)
    local upper = self:addObject("upper", app.Constant())
    local lower = self:addObject("lower", app.Constant())

    local adapter = self:addObject("adapter", app.ParameterAdapter())
    tie(upper, "Value", "function(x) return math.max(x, 0.0) end", adapter, "Out")
    tie(lower, "Value", "function(x) return math.min(x, 0.0) end", adapter, "Out")

    connect(upper, "Out", self, "Out1")
    if channelCount > 1 then
      connect(lower, "Out", self, "Out2")
    end

    self:addMonoBranch("wiggle", adapter, "In", adapter, "Out")
end

function TieExpression:onLoadViews(objects, branches)
    local controls = {}
    local views = {
        expanded = {"wiggle","low","high"},
        collapsed = {}
    }

    controls.wiggle = GainBias {
        button = "wiggle",
        description = "Wiggle Me",
        branch = branches.wiggle,
        gainbias = objects.adapter,
        range = objects.adapter,
        biasMap = Encoder.getMap("[-1,1]")
    }

    -- These two faders are just for display purposes.
    controls.low = Fader {
      button = "low",
      description = "Low",
      param = objects.lower:getParameter("Value"),
      map = Encoder.getMap("[-1,1]"),
    }

    controls.high = Fader {
      button = "high",
      description = "High",
      param = objects.upper:getParameter("Value"),
      map = Encoder.getMap("[-1,1]"),
    }

    return controls, views
end

return TieExpression
