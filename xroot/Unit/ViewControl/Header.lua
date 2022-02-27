local app = app
local Class = require "Base.Class"
local Base = require "Unit.ViewControl"
local Verification = require "Verification"
local UnitChooser = require "Unit.Chooser"
local Drawings = require "Drawings"
local Utils = require "Utils"
local ply = app.SECTION_PLY

local function createColumn(x)
  local list
  list = app.SlidingList(x, 0, 42, 64)
  list:setTextSize(10)
  list:setMargin(0)
  list:setVerticalMargin(3)
  list:setJustification(app.justifyCenter)
  list:reverseDirection()
  list:setSelectionDrawStyle(app.beveledBox)
  return list
end

-- Header Class
local Header = Class {}
Header:include(Base)

function Header:init(args)
  Base.init(self)
  self:setClassName("Unit.Base.Header")
  local title = args.title or app.logError("%s.init: title is missing.", self)
  self:setInstanceName(title)
  local graphic

  -- control graphic
  graphic = app.TextPanel("", 1)
  -- graphic:setLeftBorder(1)
  graphic:setBackgroundColor(app.GRAY2)
  graphic:setOpaque(true)
  self:setControlGraphic(graphic)
  self:setMainCursorController(graphic)
  self:setText(title)

  self.moreGraphic = app.MoreThisWay(app.up)
  self.moreGraphic:hide()
  graphic:addChild(self.moreGraphic)

  -- submenu graphics
  graphic = app.Graphic(0, 0, ply, 64)

  local list
  list = createColumn(0)
  list:add("Load")
  list:add("Bypass")
  list:select("Bypass")
  graphic:addChild(list)
  self.commandList1 = list

  list = createColumn(ply)
  list:add("Save")
  list:add("Delete")
  list:select("Delete")
  graphic:addChild(list)
  self.commandList2 = list

  list = createColumn(ply * 2)
  list:add("Replace")
  list:add("Config")
  list:select("Config")
  graphic:addChild(list)
  self.commandList3 = list

  -- local drawing = app.Drawing(0, 0, 128, 64)
  -- drawing:add(Drawings.Sub.ThreeColumns)
  -- graphic:addChild(drawing)

  self.subGraphic = graphic

  -- define spots
  self:addSpotDescriptor{
    center = 0.5 * ply,
    radius = ply
  }
  self.originalTitle = title
  self.title = title
end

function Header:bypassEnabled()
  self.controlGraphic:setBackgroundColor(app.BLACK)
  self.controlGraphic:setOpaque(false)
  self.controlGraphic:setForegroundColor(app.GRAY3)
end

function Header:bypassDisabled()
  self.controlGraphic:setBackgroundColor(app.GRAY2)
  self.controlGraphic:setOpaque(true)
  self.controlGraphic:setForegroundColor(app.WHITE)
end

function Header:setText(text)
  local graphic = self:getControlGraphic()
  local words = Utils.split(text, " ")
  graphic:clear()
  for _, word in ipairs(words) do
    word = Utils.shorten(word, 8, "..")
    if word:len() > 0 then
      graphic:addLine(word)
    end
  end
end

function Header:getFloatingMenuItems()
  if self:queryUp("currentViewName") == "collapsed" then
    return {
      "expand",
      "config",
      "rename",
      "edit controls"
    }
  else
    return {
      "collapse",
      "config",
      "rename",
      "edit controls"
    }
  end
end

function Header:onFloatingMenuSelection(choice)
  if choice == "collapse" then
    self:callUp("switchView", "collapsed")
    self:callUp("leftJustify")
    return true
  elseif choice == "expand" then
    self:callUp("switchView", "expanded")
    self:callUp("leftJustify")
    return true
  elseif choice == "rename" then
    self:callUp("doRename")
  elseif choice == "edit controls" then
    self:callUp("doEditControls")
  elseif choice == "config" then
    self.parent:showMenu()
  else
    return Base.onFloatingMenuSelection(self, choice)
  end
end

function Header:spotPressed(spot, shifted, isFocusedPress)
  if shifted then
    return false
  end
  self.focusPress = isFocusedPress
  return true
end

function Header:spotReleased(spot, shifted)
  if shifted then
    return false
  end
  if self.focusPress then
    self.focusPress = false
    self.parent:showMenu()
  end
  return true
end

function Header:enterReleased()
  if self:queryUp("currentViewName") == "collapsed" then
    self:callUp("switchView", "expanded")
  else
    self:callUp("switchView", "collapsed")
  end
  -- This must happen after view switch so that
  -- the proper section width is used in the calculation.
  self:callUp("leftJustify")
  return true
end

function Header:upReleased(shifted)
  if not shifted then
    self:unfocus()
  end
  return true
end

local function deleteHelper(unit)
  local Busy = require "Busy"
  local chain = unit.chain
  local wasMuted = chain:muteIfNeeded()
  Busy.start("Deleting: %s", Utils.shorten(unit.title, 42))
  chain:removeUnit(unit)
  Busy.stop()
  chain:unmuteIfNeeded(wasMuted)
end

function Header:doDelete()
  local unit = self.parent
  local Settings = require "Settings"
  if Settings.get("confirmUnitDelete") == "yes" then
    local dialog = Verification.Sub("Deleting!", unit.title)
    dialog:subscribe("done", function(ans)
      if ans then
        deleteHelper(unit)
      end
    end)
    dialog:show()
  else
    deleteHelper(unit)
  end
end

function Header:doReplace()
  local chooser = UnitChooser {
    goal = "replace",
    unit = self.parent
  }
  chooser:show()
end

function Header:doCommand(cmd)
  if cmd == "Load" then
    local Persist = require "Persist"
    Persist.loadUnitPreset(self.parent)
  elseif cmd == "Save" then
    local Persist = require "Persist"
    Persist.saveUnitPreset(self.parent)
    -- elseif cmd=="Init" then
  elseif cmd == "Bypass" then
    self.parent:toggleBypass()
  elseif cmd == "Delete" then
    self:doDelete()
  elseif cmd == "Replace" then
    self:doReplace()
  elseif cmd == "Config" then
    self.parent:showMenu()
  elseif cmd == "Edit" then
    self.parent:doEditControls()
  elseif cmd == "Rename" then
    self.parent:doRename()
  end
end

function Header:subReleased(i)
  if i == 1 then
    self:doCommand(self.commandList1:selectedText())
  elseif i == 2 then
    self:doCommand(self.commandList2:selectedText())
  elseif i == 3 then
    self:doCommand(self.commandList3:selectedText())
  end
  self.commandList1:scrollToBottom()
  self.commandList2:scrollToBottom()
  self.commandList3:scrollToBottom()
  return true
end

function Header:homeReleased()
  self:callUp("selectHeader")
  return true
end

function Header:onCursorEnter(spot)
  Base.onCursorEnter(self, spot)
  self:grabFocus("shiftPressed", "shiftReleased", "enterReleased")
  self.moreGraphic:show()
end

function Header:onCursorLeave(spot)
  Base.onCursorLeave(self, spot)
  self:releaseFocus("shiftPressed", "shiftReleased", "enterReleased")
  self:setText(self.title)
  self.moreGraphic:hide()
  self.commandList1:scrollToBottom()
  self.commandList2:scrollToBottom()
  self.commandList3:scrollToBottom()
end

function Header:shiftPressed()
  self:setText(self.originalTitle)
  self.commandList1:scrollToTop()
  self.commandList2:scrollToTop()
  self.commandList3:scrollToTop()
  return false
end

function Header:shiftReleased()
  self:setText(self.title)
  self.commandList1:scrollToBottom()
  self.commandList2:scrollToBottom()
  self.commandList3:scrollToBottom()
  return false
end

function Header:unitTitleChanged(title)
  self:setText(title)
  self.title = title
end

return Header
