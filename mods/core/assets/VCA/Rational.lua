local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local Encoder = require "Encoder"

local Rational = Class {}
Rational:include(Unit)

function Rational:init(args)
  args.title = "Rational VCA"
  args.mnemonic = "RV"
  Unit.init(self, args)
end

function Rational:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function Rational:loadMonoGraph()
  local vca = self:addObject("vca", libcore.RationalMultiply(true))
  local numerator = self:addObject("numerator", app.GainBias())
  local numeratorRange = self:addObject("numeratorRange", app.MinMax())
  local divisor = self:addObject("divisor", app.GainBias())
  local divisorRange = self:addObject("divisorRange", app.MinMax())

  connect(numerator, "Out", numeratorRange, "In")
  connect(numerator, "Out", vca, "Numerator")
  connect(divisor, "Out", divisorRange, "In")
  connect(divisor, "Out", vca, "Divisor")

  connect(self, "In1", vca, "In")
  connect(vca, "Out", self, "Out1")

  self:addMonoBranch("numerator", numerator, "In", numerator, "Out")
  self:addMonoBranch("divisor", divisor, "In", divisor, "Out")
end

function Rational:loadStereoGraph()
  local vca1 = self:addObject("vca1", libcore.RationalMultiply(true))
  local vca2 = self:addObject("vca2", libcore.RationalMultiply(true))

  local numeratorL = self:addObject("numeratorL", app.GainBias())
  local numeratorRangeL = self:addObject("numeratorRangeL", app.MinMax())
  local divisorL = self:addObject("divisorL", app.GainBias())
  local divisorRangeL = self:addObject("divisorRangeL", app.MinMax())

  local numeratorR = self:addObject("numeratorR", app.GainBias())
  local numeratorRangeR = self:addObject("numeratorRangeR", app.MinMax())
  local divisorR = self:addObject("divisorR", app.GainBias())
  local divisorRangeR = self:addObject("divisorRangeR", app.MinMax())

  connect(numeratorL, "Out", numeratorRangeL, "In")
  connect(numeratorL, "Out", vca1, "Numerator")
  connect(divisorL, "Out", divisorRangeL, "In")
  connect(divisorL, "Out", vca1, "Divisor")

  connect(numeratorR, "Out", numeratorRangeR, "In")
  connect(numeratorR, "Out", vca2, "Numerator")
  connect(divisorR, "Out", divisorRangeR, "In")
  connect(divisorR, "Out", vca2, "Divisor")

  connect(self, "In1", vca1, "In")
  connect(self, "In2", vca2, "In")
  connect(vca1, "Out", self, "Out1")
  connect(vca2, "Out", self, "Out2")

  self:addMonoBranch("numeratorL", numeratorL, "In", numeratorL, "Out")
  self:addMonoBranch("divisorL", divisorL, "In", divisorL, "Out")
  self:addMonoBranch("numeratorR", numeratorR, "In", numeratorR, "Out")
  self:addMonoBranch("divisorR", divisorR, "In", divisorR, "Out")
end

function Rational:onLoadViews(objects, branches)
  local controls = {}
  local views = {
    collapsed = {}
  }

  if self.channelCount == 2 then
    views.expanded = {
      "leftNum",
      "leftDiv",
      "rightNum",
      "rightDiv"
    }

    controls.leftNum = GainBias {
      button = "num(L)",
      branch = branches.numeratorL,
      description = "Left Numerator",
      gainbias = objects.numeratorL,
      range = objects.numeratorRangeL,
      biasMap = Encoder.getMap("int[0,32]"),
      gainMap = Encoder.getMap("[-20,20]"),
      initialBias = 1,
      biasPrecision = 0
    }

    controls.leftDiv = GainBias {
      button = "div(L)",
      branch = branches.divisorL,
      description = "Left Divisor",
      gainbias = objects.divisorL,
      range = objects.divisorRangeL,
      biasMap = Encoder.getMap("int[1,32]"),
      gainMap = Encoder.getMap("[-20,20]"),
      initialBias = 1,
      biasPrecision = 0
    }

    controls.rightNum = GainBias {
      button = "num(R)",
      branch = branches.numeratorR,
      description = "Right Numerator",
      gainbias = objects.numeratorR,
      range = objects.numeratorRangeR,
      biasMap = Encoder.getMap("int[0,32]"),
      gainMap = Encoder.getMap("[-20,20]"),
      initialBias = 1,
      biasPrecision = 0
    }

    controls.rightDiv = GainBias {
      button = "div(R)",
      branch = branches.divisorR,
      description = "Right Divisor",
      gainbias = objects.divisorR,
      range = objects.divisorRangeR,
      biasMap = Encoder.getMap("int[1,32]"),
      gainMap = Encoder.getMap("[-20,20]"),
      initialBias = 1,
      biasPrecision = 0
    }
  else
    views.expanded = {
      "num",
      "div"
    }

    controls.num = GainBias {
      button = "num",
      branch = branches.numerator,
      description = "Numerator",
      gainbias = objects.numerator,
      range = objects.numeratorRange,
      biasMap = Encoder.getMap("int[0,32]"),
      gainMap = Encoder.getMap("[-20,20]"),
      initialBias = 1,
      biasPrecision = 0
    }

    controls.div = GainBias {
      button = "div",
      branch = branches.divisor,
      description = "Divisor",
      gainbias = objects.divisor,
      range = objects.divisorRange,
      biasMap = Encoder.getMap("int[1,32]"),
      gainMap = Encoder.getMap("[-20,20]"),
      initialBias = 1,
      biasPrecision = 0
    }
  end

  return controls, views
end

return Rational
