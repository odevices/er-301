local app = app
local Class = require "Base.Class"
local Zoomable = require "Unit.ViewControl.Zoomable"
local Channels = require "Channels"
local Signal = require "Signal"
local ply = app.SECTION_PLY

-- SlicedWaveForm
local SlicedWaveForm = Class {}
SlicedWaveForm:include(Zoomable)

function SlicedWaveForm:init(args)
  Zoomable.init(self)
  self:setClassName("Player.SlicedWaveForm")

  local head = args.head or app.logError("%s.init: head is missing.", self)
  local width = args.width or (4 * ply)
  self.head = head

  local graphic
  graphic = app.Graphic(0, 0, width, 64)
  self.mainDisplay = app.SliceHeadDisplay(head, 0, 0, width, 64)
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

  self.subDisplay = app.SliceHeadSubDisplay(head)
  self.subGraphic:addChild(self.subDisplay)

  self.subButton1 = app.SubButton("|<<", 1)
  self.subGraphic:addChild(self.subButton1)

  self.subButton2 = app.SubButton("> / ||", 2)
  self.subGraphic:addChild(self.subButton2)

  self.subButton3 = app.SubButton("", 3)
  self.subGraphic:addChild(self.subButton3)

  Signal.weakRegister("selectReleased", self)
end

function SlicedWaveForm:setSample(sample)
  if sample then
    self.subDisplay:setName(sample.name)
    self.mainDisplay:setChannel(Channels.getSide() - 1)
  end
end

function SlicedWaveForm:selectReleased(i, shifted)
  self.mainDisplay:setChannel(Channels.getSide(i) - 1)
  return true
end

function SlicedWaveForm:getFloatingMenuItems()
  local choices = Zoomable.getFloatingMenuItems(self)
  choices[#choices + 1] = "collapse"
  choices[#choices + 1] = "open editor"
  return choices
end

function SlicedWaveForm:onFloatingMenuSelection(choice)
  if choice == "open editor" then
    self:callUp("showSampleEditor")
    return true
  elseif choice == "collapse" then
    self:callUp("switchView", "expanded")
  else
    return Zoomable.onFloatingMenuSelection(self, choice)
  end
end

function SlicedWaveForm:subReleased(i, shifted)
  if shifted then
    return false
  else
    if i == 1 then
      self.head:reset()
    elseif i == 2 then
      self.head:toggle()
    elseif i == 3 then

    end
  end
  return true
end

return SlicedWaveForm
