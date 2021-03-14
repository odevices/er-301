local app = app
local libcore = require "core.libcore"
local Env = require "Env"
local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"
local Scales = require "core.Quantizer.Scales"
local ply = app.SECTION_PLY

local PitchCircle = Class {}
PitchCircle:include(ViewControl)

function PitchCircle:init(args)
  local name = args.name or app.logError("%s.init: name is missing.", self)
  local width = args.width or (2 * ply)
  local quantizer = args.quantizer or
                        app.logError("%s.init: quantizer is missing.", self)

  ViewControl.init(self, name)
  self:setClassName("PitchCircle")

  local graphic
  graphic = app.Graphic(0, 0, width, 64)
  self.pDisplay = libcore.PitchCircle(0, 0, width, 64)
  graphic:addChild(self.pDisplay)
  self:setMainCursorController(self.pDisplay)
  self:setControlGraphic(graphic)

  -- add spots
  for i = 1, (width // ply) do
    self:addSpotDescriptor{
      center = (i - 0.5) * ply
    }
  end

  -- sub display
  self.subGraphic = app.Graphic(0, 0, 128, 64)
  self.scaleList = app.SlidingList(0, 0, 88, 64)
  self.subGraphic:addChild(self.scaleList)
  self.notesList = app.SlidingList(88, 0, 40, 64)
  self.subGraphic:addChild(self.notesList)

  self:refreshScales()
  self:setQuantizer(quantizer)
end

function PitchCircle:refreshScales()
  local selectedText = self.scaleList:selectedText()
  local selectedIndex = self.notesList:selectedIndex()
  self.scaleList:clear()
  self.notesList:clear()
  local order = Scales.getKeys()
  for i, name in ipairs(order) do
    local scale = Scales.getScale(name)
    if scale and #scale > 0 then
      self.scaleList:add(name)
      self.notesList:add(#scale)
    end
  end
  if self.scaleList:select(selectedText) then
    self.notesList:select(selectedIndex)
  end
end

function PitchCircle:setQuantizer(q)
  self.pDisplay:setQuantizer(q)
  self.quantizer = q
  self:loadBuiltinScale(self.scaleList:selectedText())
end

function PitchCircle:loadScalaFile(filename)
  if self.quantizer and filename then self.quantizer:loadScalaFile(filename) end
end

function PitchCircle:serialize()
  local t = ViewControl.serialize(self)
  t.selectedScale = self.scaleList:selectedText()
  return t
end

function PitchCircle:deserialize(t)
  ViewControl.deserialize(self, t)
  if t.selectedScale then
    if self.scaleList:select(t.selectedScale) then
      self.notesList:select(self.scaleList:selectedIndex())
      self:loadBuiltinScale(t.selectedScale)
    else
      app.logError("%s.deserialize:invalid scale:%s ", self, t.selectedScale)
    end
  else
    app.logInfo("%s.deserialize:selectedScale missing", self)
  end
end

function PitchCircle:loadBuiltinScale(name)
  if self.quantizer and name then
    local scale = Scales.getScale(name)
    if scale then
      local quantizer = self.quantizer
      quantizer:beginScale()
      for i, pitch in ipairs(scale) do quantizer:addPitch(pitch) end
      quantizer:endScale()
    end
  end
end

function PitchCircle:onFocused()
  self:refreshScales()
end

local threshold = Env.EncoderThreshold.SlidingList
function PitchCircle:encoder(change, shifted)
  self.notesList:encoder(change, shifted, threshold)
  if self.scaleList:encoder(change, shifted, threshold) and self.quantizer then
    self:loadBuiltinScale(self.scaleList:selectedText())
  end
  return true
end

return PitchCircle
