local app = app
local Class = require "Base.Class"
local Window = require "Base.Window"
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

-- Keyboard
local Keyboard = Class {}
Keyboard:include(Window)

function Keyboard:init(msg, initial, extended)
  Window.init(self)
  self:setClassName("Keyboard")
  self.suppressQuickSave = true
  self.validate = defaultValidator

  local b1 = app.BUTTON1_CENTER
  local b2 = app.BUTTON2_CENTER
  local b3 = app.BUTTON3_CENTER
  local b4 = app.BUTTON4_CENTER
  local b5 = app.BUTTON5_CENTER
  local b6 = app.BUTTON6_CENTER
  local c1 = (b1 + b2) // 2
  local c2 = (b2 + b3) // 2
  local c3 = (b3 + b4) // 2
  local c4 = (b4 + b5) // 2
  local c5 = (b5 + b6) // 2
  local h = 64

  local cols = {
    app.SlidingList(0, 0, c1, h),
    app.SlidingList(c1, 0, c2 - c1, h),
    app.SlidingList(c2, 0, c3 - c2, h),
    app.SlidingList(c3, 0, c4 - c3, h),
    app.SlidingList(c4, 0, c5 - c4, h),
    app.SlidingList(c5, 0, 256 - c5, h)
  }

  for _, col in ipairs(cols) do
    self.mainGraphic:addChild(col)
    col:setJustification(app.justifyCenter)
    col:setTextSize(12)
  end

  -- row 1
  cols[1]:add("Aa")
  cols[2]:add("Bb")
  cols[3]:add("Cc")
  cols[4]:add("Dd")
  cols[5]:add("Ee")
  cols[6]:add("Ff")
  -- row 2
  cols[1]:add("Gg")
  cols[2]:add("Hh")
  cols[3]:add("Ii")
  cols[4]:add("Jj")
  cols[5]:add("Kk")
  cols[6]:add("Ll")
  -- row 3
  cols[1]:add("Mm")
  cols[2]:add("Nn")
  cols[3]:add("Oo")
  cols[4]:add("Pp")
  cols[5]:add("Qq")
  cols[6]:add("Rr")
  -- row 4
  cols[1]:add("Ss")
  cols[2]:add("Tt")
  cols[3]:add("Uu")
  cols[4]:add("Vv")
  cols[5]:add("Ww")
  cols[6]:add("Xx")
  -- row 5
  cols[1]:add("Yy")
  cols[2]:add("Zz")
  cols[3]:add("0")
  cols[4]:add("1")
  cols[5]:add("2")
  cols[6]:add("3")
  -- row 6
  cols[1]:add("4")
  cols[2]:add("5")
  cols[3]:add("6")
  cols[4]:add("7")
  cols[5]:add("8")
  cols[6]:add("9")
  -- row 7
  cols[1]:add("+")
  cols[2]:add("-")
  cols[3]:add("_")
  cols[4]:add("=")
  cols[5]:add("~")
  cols[6]:add(".")
  if extended then
    -- row 8
    cols[1]:add("!")
    cols[2]:add("@")
    cols[3]:add("#")
    cols[4]:add("$")
    cols[5]:add("%")
    cols[6]:add("^")
    -- row 9
    cols[1]:add("&")
    cols[2]:add("*")
    cols[3]:add("(")
    cols[4]:add(")")
    cols[5]:add("[")
    cols[6]:add("]")
    -- row 10
    cols[1]:add("/")
    cols[2]:add("\\")
    cols[3]:add("|")
    cols[4]:add(":")
    cols[5]:add("<")
    cols[6]:add(">")
    -- row 11
    cols[1]:add(string.char(0177)) -- ±
    cols[2]:add("?")
    cols[3]:add(",")
    cols[4]:add(";")
    cols[5]:add("\"")
    cols[6]:add("'")
  end
  self.cols = cols

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

-- See defaultValidator() for an example.
function Keyboard:setValidator(func)
  self.validate = func or defaultValidator
end

function Keyboard:learnDirection()
  for _, col in ipairs(self.cols) do col:learnDirection() end
end

function Keyboard:mainPressed(i, shifted)
  local pressed = self.cols[i]:selectedText()
  if pressed:len() == 2 then
    if shifted then
      pressed = pressed:sub(1, 1)
    else
      pressed = pressed:sub(2, 2)
    end
  end
  self.textEditor:insertTextAtCaret(pressed)
  self:learnDirection()
  return true
end

function Keyboard:subPressed(i, shifted)
  if shifted then return false end
  if i == 1 then
    self.textEditor:doBackspace()
  elseif i == 2 then
    self.caretFocused = true
    self:setSubCursorController(self.textEditor)
  elseif i == 3 then
    -- SPC
    self.textEditor:insertTextAtCaret(" ")
  end
  self:learnDirection()
  return true

end

function Keyboard:subReleased(i, shifted)
  if shifted then return false end
  if i == 2 then
    self.caretFocused = false
    self:setSubCursorController(nil)
  end
  return true
end

function Keyboard:encoder(change, shifted)
  if self.caretFocused then
    self.textEditor:encoder(change, shifted, threshold)
  else
    for _, col in ipairs(self.cols) do
      col:encoder(change, shifted, threshold)
    end
  end
  return true
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
    self:learnDirection()
  else
    self:hide()
    self:emitSignal("done", text)
  end
  return true
end

return Keyboard
