local app = app
local Env = require "Env"
local Drawings = require "Drawings"
local Class = require "Base.Class"
local Window = require "Base.Window"
local Card = require "Card"
local Signal = require "Signal"
local Drawings = require "Drawings"

local StatusViewer = Class {}
StatusViewer:include(Window)

function StatusViewer:init()
  Window.init(self)
  self:setClassName("Card.StatusViewer")

  -- Set up graphical widgets.
  local line1 = app.GRID4_LINE1
  local line2 = app.GRID4_LINE2
  local line3 = app.GRID4_LINE3
  local line4 = app.GRID4_LINE4
  local graphic

  graphic = app.Label("Currently accessing the following files:", 10)
  graphic:setPosition(3, line1 + 3)
  graphic:setJustification(app.justifyLeft)
  self:addMainGraphic(graphic)

  graphic = app.ListBox(0, 0, 256, 50)
  graphic:setTextSize(10)
  graphic:setFocus()
  graphic:setEmptyText("None.")
  self:addMainGraphic(graphic)
  self.claims = graphic

  graphic = app.Drawing(0, 0, 256, 64)
  self:addMainGraphic(graphic)
  graphic:add(Drawings.Main.TitleLine)

  local Drawings = require "Drawings"
  local drawing = app.Drawing(0, 0, 128, 64)
  drawing:add(Drawings.Sub.TitleLine)
  self.subGraphic:addChild(drawing)

  graphic = app.Label("Card Status", 12)
  graphic:setPosition(0, line1 + 1)
  graphic:setJustification(app.justifyLeft)
  self:addSubGraphic(graphic)

  graphic = app.Label("----------", 12)
  graphic:setPosition(0, line2)
  graphic:setJustification(app.justifyLeft)
  self:addSubGraphic(graphic)
  self.status1 = graphic

  graphic = app.Label("----------", 12)
  graphic:setPosition(0, line3)
  graphic:setJustification(app.justifyLeft)
  self:addSubGraphic(graphic)
  self.status2 = graphic

  graphic = app.Label("----------", 12)
  graphic:setPosition(0, line4)
  graphic:setJustification(app.justifyLeft)
  self:addSubGraphic(graphic)
  self.status3 = graphic

  -- Initialize with current card state.
  for filename, who in pairs(Card.getClaimedFiles()) do
    self:fileClaimed(who, filename)
  end

  if Card.ejecting() then
    self:cardEjecting()
  elseif Card.mounted() then
    self:cardMounted()
  elseif Card.failed() then
    self:cardFailed()
  else
    self:cardEjected()
  end

  -- Subscribe to notifications of future changes in card state.
  Signal.weakRegister("fileClaimed", self)
  Signal.weakRegister("fileReleased", self)
  Signal.weakRegister("cardMounted", self)
  Signal.weakRegister("cardFailed", self)
  Signal.weakRegister("cardEjected", self)
  Signal.weakRegister("cardEjecting", self)
  Signal.weakRegister("cardRemoteMounted", self)
end

function StatusViewer:fileClaimed(who, filename)
  local text = who .. ": " .. filename
  local claims = self.claims
  if not claims:updateNameByData(text, filename) then
    claims:addItem(text, filename)
  end
end

function StatusViewer:fileReleased(filename)
  self.claims:removeItemByData(filename)
end

function StatusViewer:cardMounted(totalSizeMB, freeSizeMB)
  self.status1:setText("Mounted for local use.")
  local text1, text2
  if totalSizeMB > 10 * 1024 * 1024 then
    text1 = string.format("%5dGB total", math.ceil(totalSizeMB // 1024))
    text2 = string.format("%5dGB free", math.ceil(freeSizeMB // 1024))
  else
    text1 = string.format("%5dMB total", math.ceil(totalSizeMB))
    text2 = string.format("%5dMB free", math.ceil(freeSizeMB))
  end
  self.status2:setText(text1)
  self.status3:setText(text2)
end

function StatusViewer:cardRemoteMounted()
  self.status1:setText("")
  self.status2:setText("Mounted over USB.")
  self.status3:setText("")
end

function StatusViewer:cardFailed()
  self.status1:setText("Failed to ")
  self.status2:setText("mount.")
  self.status3:setText("")
end

function StatusViewer:cardEjected()
  self.status1:setText("Unmounted.")
  self.status2:setText("Safe to remove.")
  self.status3:setText("")
end

function StatusViewer:cardEjecting()
  local n = Card.getClaimCount()
  local text
  if n == 1 then
    text = "1 file in use!"
  else
    text = string.format("%d files in use!", n)
  end
  self.status1:setText(text)
  self.status2:setText("Do NOT remove.")
  self.status3:setText("")
end

local threshold = Env.EncoderThreshold.Default
function StatusViewer:encoder(change, shifted)
  self.claims:encoder(change, shifted, threshold)
  return true
end

function StatusViewer:cancelReleased(shifted)
  if not shifted then self:hide() end
  return true
end

function StatusViewer:upReleased(shifted)
  if not shifted then self:hide() end
  return true
end

function StatusViewer:homeReleased()
  self:hide()
  return true
end

return StatusViewer
