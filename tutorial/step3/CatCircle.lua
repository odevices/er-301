local app = app
local libFoo = require "tutorial.libFoo"
local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"
local ply = app.SECTION_PLY

local CatCircle = Class {}
CatCircle:include(ViewControl)

function CatCircle:init(args)
  local sequencer = args.sequencer or
                        app.logError("%s.init: sequencer is missing.", self)

  ViewControl.init(self, "circle")
  self:setClassName("CatCircle")

  local width = args.width or (2 * ply)

  local graphic
  graphic = app.Graphic(0, 0, width, 64)
  self.pDisplay = libFoo.CatCircle(0, 0, width, 64)
  graphic:addChild(self.pDisplay)
  self:setMainCursorController(self.pDisplay)
  self:setControlGraphic(graphic)

  -- add spots
  for i = 1, (width // ply) do
    self:addSpotDescriptor{
      center = (i - 0.5) * ply
    }
  end

  -- sub display is empty for now
  self.subGraphic = app.Graphic(0, 0, 128, 64)

  self:follow(sequencer)
end
function CatCircle:follow(sequencer)
  self.pDisplay:follow(sequencer)
  self.sequencer = sequencer
end

return CatCircle
