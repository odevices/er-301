local app = app
local Class = require "Base.Class"
local Unit = require "Unit"
local GainBias = require "Unit.ViewControl.GainBias"

local OffsetUnit = Class {}
OffsetUnit:include(Unit)

function OffsetUnit:init(args)
  args.title = "Offset"
  args.mnemonic = "Of"
  Unit.init(self, args)
end

function OffsetUnit:onLoadGraph(channelCount)
  if channelCount == 2 then
    self:loadStereoGraph()
  else
    self:loadMonoGraph()
  end
end

function OffsetUnit:loadMonoGraph()
  local sum = self:addObject("sum", app.Sum())
  local gainbias = self:addObject("gainbias", app.GainBias())
  local range = self:addObject("range", app.MinMax())

  connect(self, "In1", sum, "Left")
  connect(gainbias, "Out", sum, "Right")
  connect(gainbias, "Out", range, "In")
  connect(sum, "Out", self, "Out1")

  self:addMonoBranch("offset", gainbias, "In", gainbias, "Out")
end

function OffsetUnit:loadStereoGraph()
  local sum1 = self:addObject("sum1", app.Sum())
  local gainbias1 = self:addObject("gainbias1", app.GainBias())
  local range1 = self:addObject("range1", app.MinMax())

  connect(self, "In1", sum1, "Left")
  connect(gainbias1, "Out", sum1, "Right")
  connect(gainbias1, "Out", range1, "In")
  connect(sum1, "Out", self, "Out1")

  self:addMonoBranch("offsetL", gainbias1, "In", gainbias1, "Out")

  local sum2 = self:addObject("sum2", app.Sum())
  local gainbias2 = self:addObject("gainbias2", app.GainBias())
  local range2 = self:addObject("range2", app.MinMax())

  connect(self, "In2", sum2, "Left")
  connect(gainbias2, "Out", sum2, "Right")
  connect(gainbias2, "Out", range2, "In")
  connect(sum2, "Out", self, "Out2")

  self:addMonoBranch("offsetR", gainbias2, "In", gainbias2, "Out")
end

function OffsetUnit:onLoadViews(objects, branches)
  local views = {
    collapsed = {}
  }
  local controls = {}

  if self.channelCount == 2 then

    controls.leftOffset = GainBias {
      button = "left",
      description = "Left Offset",
      branch = branches.offsetL,
      gainbias = objects.gainbias1,
      range = objects.range1,
      initialBias = 0.0
    }

    controls.rightOffset = GainBias {
      button = "right",
      description = "Right Offset",
      branch = branches.offsetR,
      gainbias = objects.gainbias2,
      range = objects.range2,
      initialBias = 0.0
    }

    views.expanded = {
      "leftOffset",
      "rightOffset"
    }
  else

    controls.offset = GainBias {
      button = "offset",
      description = "Offset",
      branch = branches.offset,
      gainbias = objects.gainbias,
      range = objects.range,
      initialBias = 0.0
    }

    views.expanded = {
      "offset"
    }
  end

  return controls, views
end

function OffsetUnit:deserialize(t)
  Unit.deserialize(self, t)
  if self:getPresetVersion(t) < 1 then
    -- handle legacy preset (<v0.3.0)
    local Serialization = require "Persist.Serialization"
    local offset = Serialization.get("objects/offset/params/Offset", t)
    if self.channelCount == 1 then
      if offset then
        app.logInfo("%s:deserialize:legacy preset detected:setting offset to %s",
                self, offset)
        self.objects.gainbias:deserialize("Bias", offset)
      end
    elseif self.channelCount == 2 then
      local offset1 = Serialization.get("objects/offset1/params/Offset", t)
      if offset1 then
        app.logInfo("%s:deserialize:legacy preset detected:setting offset1 to %s",
                self, offset1)
        self.objects.gainbias1:deserialize("Bias", offset1)
      end
      local offset2 = Serialization.get("objects/offset2/params/Offset", t)
      if offset2 then
        app.logInfo("%s:deserialize:legacy preset detected:setting offset2 to %s",
                self, offset2)
        self.objects.gainbias2:deserialize("Bias", offset2)
      end
    end
  end
end

return OffsetUnit
