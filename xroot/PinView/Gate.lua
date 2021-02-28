local Class = require "Base.Class"
local Base = require "PinView.Control"
local ply = app.SECTION_PLY

local Gate = Class {}
Gate:include(Base)

function Gate:init(args)
  Base.init(self, args)
  self:setClassName("PinView.Gate")
  -- required arguments
  local name = args.name or app.logError("%s.init: name is missing.", self)
  self:setInstanceName(name)
  local comparator = args.comparator or
                         app.logError("%s.init: comparator is missing.", self)
  self.comparator = comparator
  -- optional arguments
  local units = args.units or app.unitHertz
  local precision = args.precision or 3
  local param = args.param

  local graphic
  if param then
    graphic = app.ComparatorView(0, 0, ply, 64, comparator, param)
  else
    graphic = app.ComparatorView(0, 0, ply, 64, comparator)
  end
  graphic:setLabel(name)
  graphic:setUnits(units)
  graphic:setPrecision(precision)
  self.comparatorView = graphic
  self:setMainCursorController(graphic)
  self:setControlGraphic(graphic)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }

  -- sub display
  self.subGraphic = app.Graphic(0, 0, 128, 64)
  graphic = app.TextPanel("Rename", 1)
  self.subGraphic:addChild(graphic)
  graphic = app.TextPanel("Unpin", 2)
  self.subGraphic:addChild(graphic)
  graphic = app.TextPanel("", 3)
  self.subGraphic:addChild(graphic)
end

function Gate:rename(name)
  self:setInstanceName(name)
  self.comparatorView:setLabel(name)
end

function Gate:spotPressed(spot, shifted)
  if shifted then return false end
  self.comparator:simulateRisingEdge()
  return true
end

function Gate:spotReleased(spot, shifted)
  if shifted then return false end
  self.comparator:simulateFallingEdge()
  return true
end

function Gate:encoder(change, shifted)
  return true
end

function Gate:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 then
    self:doRename()
  elseif i == 2 then
    local pinSet = self.parent -- save it
    pinSet:startViewModifications()
    pinSet:unpinControl(self.delegate)
    pinSet:endViewModifications()
  elseif i == 3 then
    -- do nothing
  end
  return true
end

return Gate
