local libcore = require "core.libcore"
local Class = require "Base.Class"
local Unit = require "Unit"
local OptionControl = require "Unit.ViewControl.OptionControl"

local RectifierUnit = Class {}
RectifierUnit:include(Unit)

function RectifierUnit:init(args)
  args.title = "Rectify"
  args.mnemonic = "Re"
  Unit.init(self, args)
end

function RectifierUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function RectifierUnit:loadMonoGraph()
  local rectify = self:addObject("rectify", libcore.Rectify())

  connect(self, "In1", rectify, "In")
  connect(rectify, "Out", self, "Out1")
end

function RectifierUnit:loadStereoGraph()
  local rectify1 = self:addObject("rectify1", libcore.Rectify())
  local rectify2 = self:addObject("rectify2", libcore.Rectify())

  connect(self, "In1", rectify1, "In")
  connect(self, "In2", rectify2, "In")
  connect(rectify1, "Out", self, "Out1")
  connect(rectify2, "Out", self, "Out2")

  tie(rectify2, "Type", rectify1, "Type")
  self.objects.rectify = self.objects.rectify1
end

local views = {
  expanded = {
    "type"
  },
  collapsed = {}
}

function RectifierUnit:onLoadViews(objects, branches)
  local controls = {}

  controls.type = OptionControl {
    button = "o",
    description = "Type",
    option = objects.rectify:getOption("Type"),
    choices = {
      "positive half",
      "negative half",
      "full"
    },
    muteOnChange = true
  }

  return controls, views
end

return RectifierUnit
