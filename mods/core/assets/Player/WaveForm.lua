local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Zoomable = require "Unit.ViewControl.Zoomable"
local Channels = require "Channels"
local Signal = require "Signal"
local ply = app.SECTION_PLY

-- WaveForm
local WaveForm = Class {}
WaveForm:include(Zoomable)

function WaveForm:init(args)
  Zoomable.init(self)
  self:setClassName("Player.WaveForm")
  local width = args.width or (4 * ply)
  local head = args.head or app.logError("%s: head is missing from args.", self)
  self.head = head

  local graphic
  graphic = app.Graphic(0, 0, width, 64)
  self.mainDisplay = app.HeadDisplay(head, 0, 0, width, 64)
  if args.label then self.mainDisplay:setPointerLabel(args.label) end
  graphic:addChild(self.mainDisplay)
  self:setMainCursorController(self.mainDisplay)
  self:setControlGraphic(graphic)

  -- add spots
  for i = 1, (width // ply) do
    self:addSpotDescriptor{
      center = (i - 0.5) * ply
    }
  end
  self.verticalDivider = width

  -- sub display
  self.subGraphic = app.Graphic(0, 0, 128, 64)

  self.subDisplay = app.HeadSubDisplay(head)
  self.subGraphic:addChild(self.subDisplay)

  Signal.weakRegister("selectReleased", self)
end

function WaveForm:setSample(sample)
  if sample then
    self.subDisplay:setName(sample.name)
    self.mainDisplay:setChannel(Channels.getSide() - 1)
  end
end

function WaveForm:selectReleased(i, shifted)
  self.mainDisplay:setChannel(Channels.getSide(i) - 1)
  return true
end

function WaveForm:getFloatingMenuItems()
  local choices = Zoomable.getFloatingMenuItems(self)
  choices[#choices + 1] = "collapse"
  choices[#choices + 1] = "open editor"
  return choices
end

function WaveForm:onFloatingMenuSelection(choice)
  if choice == "open editor" then
    self:callUp("showSampleEditor")
    return true
  elseif choice == "collapse" then
    self:callUp("switchView", "expanded")
  else
    return Zoomable.onFloatingMenuSelection(self, choice)
  end
end

function WaveForm:subReleased(i, shifted)
  if shifted then
    return false
  else
    if i == 1 then

    elseif i == 2 then

    elseif i == 3 then

    end
  end
  return true
end

return WaveForm
