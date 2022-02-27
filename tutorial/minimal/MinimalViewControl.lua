local app = app
local libMinimal = require "minimal.libMinimal"
local Class = require "Base.Class"
local ViewControl = require "Unit.ViewControl"
local ply = app.SECTION_PLY

-- Some background:
-- A unit can have multiple views.
-- A unit view is simply a list of ViewControls.
-- A view always starts with a Header (a type of ViewControl) which is followed by whatever ViewControls instantiated in the unit's onLoadViews method.

-- This class implements a ViewControl that displays a MinimalGraphic and nothing else.
local MinimalViewControl = Class {}
MinimalViewControl:include(ViewControl)

function MinimalViewControl:init(args)
  ViewControl.init(self)
  self:setClassName("MinimalViewControl")

  -- Some background:
  -- Each display is divided into columns centered above each soft button.
  -- The main display has 6 and the sub display has 3.
  -- The width of one of these columns is 1 ply (or about 42px).

  -- The width of this control defaults to 2 ply.
  local width = args.width or (2 * ply)

  -- Create an empty parent Graphic to hold our MinimalGraphic.
  local graphic
  graphic = app.Graphic(0, 0, width, 64)
  -- Create the MinimalGraphic
  self.pDisplay = libMinimal.MinimalGraphic(0, 0, width, 64)
  -- Add the MinimalGraphic to the parent Graphic.
  graphic:addChild(self.pDisplay)
  -- When this ViewControl gains focus, the system cursor's location should be controlled by the MinimalGraphic.
  self:setMainCursorController(self.pDisplay)
  -- Assign the parent Graphic that we prepared to this ViewControl.
  self:setControlGraphic(graphic)

  -- Define the spots on this ViewControl where the cursor should stop as you scroll horizontally.
  -- Here we simply have the cursor stop at the center of each ply.
  for i = 1, (width // ply) do
    self:addSpotDescriptor{
      center = (i - 0.5) * ply
    }
  end

  -- The sub display is empty for now.
  self.subGraphic = app.Graphic(0, 0, 128, 64)
end

return MinimalViewControl
