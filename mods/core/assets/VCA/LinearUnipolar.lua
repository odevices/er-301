local app = app
local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"
local OptionControl = require "Unit.MenuControl.OptionControl"
local Encoder = require "Encoder"

local LinearUnipolar = Class {}
LinearUnipolar:include(Unit)

function LinearUnipolar:init(args)
  args.title = "Linear Unipolar VCA"
  args.mnemonic = "VCA"
  Unit.init(self, args)
end

function LinearUnipolar:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function LinearUnipolar:loadMonoGraph()
  local vca = self:addObject("vca", app.Multiply())
  local level = self:addObject("level", app.GainBias())
  local levelRange = self:addObject("levelRange", app.MinMax())
  local rectify = self:addObject("rectify", libcore.Rectify())
  rectify:setOptionValue("Type", libcore.RECTIFY_POSITIVEHALF)

  connect(level, "Out", rectify, "In")
  connect(level, "Out", levelRange, "In")
  connect(rectify, "Out", vca, "Left")

  connect(self, "In1", vca, "Right")
  connect(vca, "Out", self, "Out1")

  self:addMonoBranch("level", level, "In", level, "Out")
end

function LinearUnipolar:loadStereoGraph()
  local vca1 = self:addObject("vca1", app.Multiply())
  local vca2 = self:addObject("vca2", app.Multiply())
  local level = self:addObject("level", app.GainBias())
  local levelRange = self:addObject("levelRange", app.MinMax())
  local rectify = self:addObject("rectify", libcore.Rectify())
  rectify:setOptionValue("Type", libcore.RECTIFY_POSITIVEHALF)

  local balance = self:addObject("balance", app.StereoPanner())
  local pan = self:addObject("pan", app.GainBias())
  local panRange = self:addObject("panRange", app.MinMax())

  connect(level, "Out", rectify, "In")
  connect(level, "Out", levelRange, "In")
  connect(rectify, "Out", vca1, "Left")
  connect(rectify, "Out", vca2, "Left")

  connect(self, "In1", vca1, "Right")
  connect(self, "In2", vca2, "Right")

  connect(vca1, "Out", balance, "Left In")
  connect(balance, "Left Out", self, "Out1")
  connect(vca2, "Out", balance, "Right In")
  connect(balance, "Right Out", self, "Out2")

  connect(pan, "Out", balance, "Pan")
  connect(pan, "Out", panRange, "In")

  self:addMonoBranch("level", level, "In", level, "Out")
  self:addMonoBranch("pan", pan, "In", pan, "Out")
end

local menu = {
  "rectify"
}

function LinearUnipolar:onShowMenu(objects, branches)
  local controls = {}

  controls.rectify = OptionControl {
    description = "Rectify Type",
    option = objects.rectify:getOption("Type"),
    choices = {
      "positive half",
      "negative half",
      "full"
    },
    muteOnChange = true
  }

  return controls, menu
end

function LinearUnipolar:onLoadViews(objects, branches)
  local controls = {}
  local views = {
    expanded = {
      "level"
    },
    collapsed = {}
  }

  controls.level = GainBias {
    button = "level",
    branch = branches.level,
    description = "Level",
    gainbias = objects.level,
    range = objects.levelRange,
    biasMap = Encoder.getMap("[-5,5]"),
    biasUnits = app.unitNone,
    initialBias = 0.0,
    gainMap = Encoder.getMap("gain")
  }

  if objects.pan then
    controls.pan = GainBias {
      button = "pan",
      branch = branches.pan,
      description = "Pan",
      gainbias = objects.pan,
      range = objects.panRange,
      biasMap = Encoder.getMap("default"),
      biasUnits = app.unitNone
    }

    views.expanded[2] = "pan"
  end

  return controls, views
end

return LinearUnipolar
