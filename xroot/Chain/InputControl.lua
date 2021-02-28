local app = app
local Env = require "Env"
local Class = require "Base.Class"
local SpottedControl = require "SpottedStrip.Control"
local Utils = require "Utils"
local ply = app.SECTION_PLY

-- InputControl Class
local InputControl = Class {}
InputControl:include(SpottedControl)

function InputControl:init(index)
  SpottedControl.init(self)
  self:setClassName("InputControl")
  self:setInstanceName(index)
  self.inputIndex = index

  -- graphic
  local graphic = app.Graphic(0, 0, ply, 64)
  local scope = app.MiniScope(0, 0, ply, 64)
  scope:setForegroundColor(app.GRAY7)
  self.scope = scope
  graphic:addChild(self.scope)

  self.panel = app.TextPanel("", 1)
  graphic:addChild(self.panel)

  self:setControlGraphic(graphic)

  -- define spots
  self:addSpotDescriptor{
    center = 0.5 * ply,
    radius = ply
  }
  self.verticalDivider = 0

  -- sub display
  graphic = app.Graphic(0, 0, 128, 64)
  local label = app.Label("Set Chain Input", 12)
  label:setCenter(64, app.GRID5_LINE2)
  graphic:addChild(label)
  label = app.SubButton("clear", 3)
  graphic:addChild(label)
  self.menuGraphic = graphic
end

function InputControl:spotReleased(i, shifted)
  if not shifted then
    local chain = self:getWindow()
    if chain then
      local SourceChooser = require "Source.Chooser"
      local chooser =
          SourceChooser(chain, chain:getInputSource(self.inputIndex))
      local task = function(src)
        chain:setInputSource(self.inputIndex, src)
      end
      chooser:subscribe("choose", task)
      chooser:show()
    end
  end
end

function InputControl:contentChanged(chain)
  local scope = self.scope
  local source = chain:getInputSource(self.inputIndex)
  if source then
    scope:watchOutlet(source:getOutlet())
    if source.isUnipolar then
      scope:setOffset(-1)
    else
      scope:setOffset(0)
    end
    local channelName = source:getChannelName()
    local displayName = source:getDisplayName()
    if channelName then
      local text = string.format("%s (%s)", displayName, channelName)
      self:setText(text)
    else
      self:setText(displayName)
    end
  else
    self.scope:watchOutlet(nil)
    self.panel:setText("")
  end
end

function InputControl:setText(text)
  local graphic = self.panel
  local words = Utils.split(text, " ")
  graphic:clear()
  for _, word in ipairs(words) do graphic:addLine(Utils.shorten(word, 8, "..")) end
end

function InputControl:onCursorEnter()
  self:addSubGraphic(self.menuGraphic)
  self:grabFocus("subReleased")
end

function InputControl:onCursorLeave()
  self:removeSubGraphic(self.menuGraphic)
  self:releaseFocus("subReleased")
end

function InputControl:subReleased(i, shifted)
  if shifted then return end
  if i == 3 then self:callUp("clearInputSource", self.inputIndex) end
  return true
end

return InputControl
