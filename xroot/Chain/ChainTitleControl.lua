local app = app
local Env = require "Env"
local Class = require "Base.Class"
local SpottedControl = require "SpottedStrip.Control"
local Utils = require "Utils"
local ply = app.SECTION_PLY

local ChainTitleControl = Class {}
ChainTitleControl:include(SpottedControl)

local clearText = "Clear Chain"
local loadText = "Load Chain"
local saveText = "Save Chain"

function ChainTitleControl:init(title, subTitle)
  SpottedControl.init(self)
  self:setClassName("ChainTitleControl")

  -- control graphic
  local text
  if subTitle then
    text = string.format("%s (%s)", title, subTitle)
  else
    text = title
  end
  local graphic = app.TextPanel("", 1)
  graphic:setCornerRadius(10, 10, 0, 0)
  self:setControlGraphic(graphic)
  self:setText(text)

  -- submenu graphics
  graphic = app.Graphic(0, 0, 128, 64)
  local label = app.TextPanel(clearText, 2)
  graphic:addChild(label)
  self.clearButton = label
  label = app.TextPanel(loadText, 1)
  graphic:addChild(label)
  label = app.TextPanel(saveText, 3)
  graphic:addChild(label)
  self.menuGraphic = graphic

  -- define spots
  self:addSpotDescriptor{
    center = 0.5 * ply,
    radius = ply
  }
end

function ChainTitleControl:setText(text)
  local graphic = self:getControlGraphic()
  local words = Utils.split(text, " ")
  graphic:clear()
  for _, word in ipairs(words) do graphic:addLine(Utils.shorten(word, 8, "..")) end
end

function ChainTitleControl:enterReleased(i, shifted)
  if not shifted then self:callUp("toggleAllUnits") end
  return true
end

function ChainTitleControl:spotReleased(i, shifted)
  if not shifted then
    -- self:callUp("toggleAllUnits")
  end
  return true
end

function ChainTitleControl:doClear()
  local Settings = require "Settings"
  if Settings.get("confirmChainClear") == "yes" then
    local Verification = require "Verification"
    local dialog = Verification.Sub("Clearing chain!", "Are you sure?")
    dialog:subscribe("done", function(ans)
      if ans then self:callUp("clear") end
    end)
    dialog:show()
  else
    self:callUp("clear")
  end
end

function ChainTitleControl:subReleased(i, shifted)
  if shifted then
    -- no commands yet
  else
    if i == 1 then
      self:callUp("loadPreset")
    elseif i == 2 then
      self:doClear()
    elseif i == 3 then
      self:callUp("savePreset")
    end
  end
  return true
end

function ChainTitleControl:chainTitleChanged(title, subTitle)
  local text
  if subTitle then
    text = string.format("%s (%s)", title, subTitle)
  else
    text = title
  end
  self:setText(text)
end

function ChainTitleControl:onCursorEnter()
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased", "shiftPressed", "shiftReleased", "enterReleased")
  if self:callUp("canUndo") then
    self.clearButton:setText("Undo")
  else
    self.clearButton:setText(clearText)
  end
end

function ChainTitleControl:onCursorLeave()
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased", "shiftPressed", "shiftReleased",
                    "enterReleased")
end

return ChainTitleControl
