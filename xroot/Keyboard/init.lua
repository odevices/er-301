local app = app
local Class = require "Base.Class"
local Base = require "MondrianMenu.AsWindow"
local BaseControl = require "MondrianMenu.Control"
local Env = require "Env"
local Utils = require "Utils"
local Message = require "Message"
local threshold = Env.EncoderThreshold.SlidingList

local function defaultValidator(text)
  if text == "" then
    return false, "Input is blank."
  else
    return true
  end
end

local KeyboardItem = Class {}
KeyboardItem:include(BaseControl)

function KeyboardItem:init(text, shiftedText, hasBorder)
  BaseControl.init(self)
  self:setClassName("Keyboard.Item")
  local graphic = app.FittedTextBox(text)
  if hasBorder then
    graphic:setBorder(1)
    graphic:setFontSize(10)
  else
    graphic:setBorder(0)
    graphic:setFontSize(12)
  end

  self:setControlGraphic(graphic)
  self.text = text
  self.shiftedText = shiftedText
end

function KeyboardItem:onPressed(_, shifted)
  if self.shiftedText and shifted then
    self:callUp("insertText", self.shiftedText)
  else
    self:callUp("insertText", self.text)
  end
end

function KeyboardItem:onRepeated(_, shifted)
  if self.shiftedText and shifted then
    self:callUp("insertText", self.shiftedText)
  else
    self:callUp("insertText", self.text)
  end
end

function KeyboardItem:onShiftPressed()
  if self.shiftedText then
    self.controlGraphic:setText(self.shiftedText)
  end
end

function KeyboardItem:onShiftReleased()
  if self.shiftedText then
    self.controlGraphic:setText(self.text)
  end
end

-- Keyboard
local Keyboard = Class {}
Keyboard:include(Base)

function Keyboard:init(msg, initial, extended, history)
  Base.init(self)
  self:setClassName("Keyboard")
  self.suppressQuickSave = true
  self.validate = defaultValidator
  self.mlist:beginJustifyLeft()

  if history then
    local Persist = require "Persist"
    local historyDB = Persist.getRearCardValue("Keyboard", history) or {}
    for _, text in ipairs(historyDB) do
      self:addItem(text, nil, true)
    end

    local Drawings = require "Drawings"
    local graphic = app.Drawing(0, 0, 256, 6)
    graphic:add(Drawings.Main.HorizontalLine)
    self.mlist:startNewRow()
    self.mlist:beginSelectableOff()
    self.mlist:addGraphic(graphic)
    self.mlist:startNewRow()
    self.mlist:endSelectable()

    self.history = history
    self.historyDB = historyDB
  end

  -- row 1
  local first = self:addItem("a", "A")
  self:addItem("b", "B")
  self:addItem("c", "C")
  self:addItem("d", "D")
  self:addItem("e", "E")
  self:addItem("f", "F")
  -- row 2
  self:addItem("g", "G")
  self:addItem("h", "H")
  self:addItem("i", "I")
  self:addItem("j", "J")
  self:addItem("k", "K")
  self:addItem("l", "L")
  -- row 3
  self:addItem("m", "M")
  self:addItem("n", "N")
  self:addItem("o", "O")
  self:addItem("p", "P")
  self:addItem("q", "Q")
  self:addItem("r", "R")
  -- row 4
  self:addItem("s", "S")
  self:addItem("t", "T")
  self:addItem("u", "U")
  self:addItem("v", "V")
  self:addItem("w", "W")
  self:addItem("x", "X")
  -- row 5
  self:addItem("y", "Y")
  self:addItem("z", "Z")
  self:addItem("0")
  self:addItem("1")
  self:addItem("2")
  self:addItem("3")
  -- row 6
  self:addItem("4")
  self:addItem("5")
  self:addItem("6")
  self:addItem("7")
  self:addItem("8")
  self:addItem("9")
  -- row 7
  self:addItem("+")
  self:addItem("-")
  self:addItem("_")
  self:addItem("=")
  self:addItem("~")
  self:addItem(".")
  if extended then
    -- row 8
    self:addItem("!")
    self:addItem("@")
    self:addItem("#")
    self:addItem("$")
    self:addItem("%")
    self:addItem("^")
    -- row 9
    self:addItem("&")
    self:addItem("*")
    self:addItem("(")
    self:addItem(")")
    self:addItem("[")
    self:addItem("]")
    -- row 10
    self:addItem("/")
    self:addItem("\\")
    self:addItem("|")
    self:addItem(":")
    self:addItem("<")
    self:addItem(">")
    -- row 11
    self:addItem(string.char(0177)) -- ±
    self:addItem("?")
    self:addItem(",")
    self:addItem(";")
    self:addItem("\"")
    self:addItem("'")
  end

  self.mlist:setAnchorGraphic(first.controlGraphic)

  -- sub display
  local editor = app.TextEditor(0, app.GRID4_LINE2 - 4, 128, 16, 12)
  editor:setText(initial or "")
  editor:setBorder(1)
  editor:setCornerRadius(3, 3, 3, 3)
  self.subGraphic:addChild(editor)
  self.textEditor = editor

  local label = app.Label(msg or "Enter text", 12)
  label:setCenter(64, app.GRID4_CENTER1)
  self.subGraphic:addChild(label)

  self.subGraphic:addChild(app.SubButton("bksp", 1))
  self.subGraphic:addChild(app.SubButton("cursor", 2))
  self.subGraphic:addChild(app.SubButton("space", 3))
end

function Keyboard:addItem(...)
  local control = KeyboardItem(...)
  self:addControl(control)
  return control
end

-- See defaultValidator() for an example.
function Keyboard:setValidator(func)
  self.validate = func or defaultValidator
end

function Keyboard:insertText(text)
  self.textEditor:insertTextAtCaret(text)
  return true
end

function Keyboard:subPressed(i, shifted)
  if shifted then
    return false
  end
  if i == 1 then
    self.textEditor:doBackspace()
  elseif i == 2 then
    self.caretFocused = true
    self:setSubCursorController(self.textEditor)
  elseif i == 3 then
    -- SPC
    self.textEditor:insertTextAtCaret(" ")
  end
  return true
end

function Keyboard:subRepeated(i, shifted)
  if shifted then
    return false
  end
  if i == 1 then
    self.textEditor:doBackspace()
  elseif i == 2 then
  elseif i == 3 then
    -- SPC
    self.textEditor:insertTextAtCaret(" ")
  end
  return true
end

function Keyboard:subReleased(i, shifted)
  if shifted then
    return false
  end
  if i == 2 then
    self.caretFocused = false
    self:setSubCursorController(nil)
  end
  return true
end

function Keyboard:encoder(change, shifted)
  if self.caretFocused then
    self.textEditor:encoder(change, shifted, threshold)
    return true
  else
    return Base.encoder(self, change, shifted)
  end
end

function Keyboard:cancelReleased()
  self:hide()
  self:emitSignal("done")
  return true
end

function Keyboard:upReleased()
  self:hide()
  self:emitSignal("done")
  return true
end

function Keyboard:zeroPressed()
  self.textEditor:setText("")
  return true
end

function Keyboard:enterReleased()
  local text = Utils.trim(self.textEditor:getText())
  local isValid, msg = self.validate(text)
  if not isValid then
    local dialog = Message.Main(msg)
    dialog:show()
  else
    self:hide()
    if self.history then
      local db = self.historyDB
      local tmp = {}
      local start = 1
      -- Keep only last 100 entries.
      if #db > 100 then
        start = #db - 100
      end
      -- Skip the entered text.
      for i = start, #db do
        local text2 = db[i]
        if text2 ~= text then
          tmp[#tmp + 1] = text2
        end
      end
      -- Finally, add the entered text to the end.
      tmp[#tmp + 1] = text
      local Persist = require "Persist"
      Persist.setRearCardValue("Keyboard", self.history, tmp)
    end
    self:emitSignal("done", text)
  end
  return true
end

function Keyboard:shiftPressed()
  for _, control in pairs(self.controls) do
    if control.onShiftPressed then
      control:onShiftPressed()
    end
  end
  return true
end

function Keyboard:shiftReleased()
  for _, control in pairs(self.controls) do
    if control.onShiftReleased then
      control:onShiftReleased()
    end
  end
  return true
end

return Keyboard
