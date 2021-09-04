local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Signal = require "Signal"
local Path = require "Path"
local Window = require "Base.Window"
local Verification = require "Verification"
local Message = require "Message"
local Busy = require "Busy"
local Card = require "Card"
local StatusViewer = require "Card.StatusViewer"
local Overlay = require "Overlay"

local statusViewer = StatusViewer()
local rearCardInfo = app.CardInfo(app.roots.rear)
local frontCardInfo = app.CardInfo(app.roots.front)

local Console = Class {}
Console:include(Window)

function Console:init()
  Window.init(self)
  self:setClassName("Card.Console")

  local console = app.ListBox(0, 0, 256, 64)
  console:setTextSize(10)
  console:setFocus()
  self.mainGraphic:addChild(console)
  self.console = console

  local graphic
  graphic = app.TextPanel("Measure Speed", 1)
  self.subGraphic:addChild(graphic)

  graphic = app.TextPanel("Format Card", 2)
  self.subGraphic:addChild(graphic)

  graphic = app.TextPanel("Files In Use", 3)
  self.subGraphic:addChild(graphic)

  self:msg("Welcome to the Card Console.")

  Signal.register("cardMounted", self)
  Signal.register("cardEjected", self)
  Signal.register("cardFailed", self)
  Signal.register("cardEjecting", self)
  Signal.register("cardRemoteEjecting", self)
  Signal.register("cardRemoteMounted", self)
  Signal.register("onUSBConnect", self)
  Signal.register("onUSBDisconnect", self)
end

function Console:msg(...)
  local text = string.format(...)
  self.console:addItem(text)
  self.console:scrollToBottom()
end

------------------------------------
-- event handlers

function Console:cardMounted(totalSizeMB, freeSizeMB)
  self:msg("Card mounted. Total=%dMB Free=%dMB", totalSizeMB, freeSizeMB)
end

function Console:cardFailed()
  self:msg("Card failed to mount.")
end

function Console:cardEjected()
  self:msg("Card ejected.")
end

function Console:cardEjecting()
  self:msg("Card ejecting...")
end

function Console:cardRemoteEjecting()
  self:msg("USB host is ejecting card...")
end

function Console:cardRemoteMounted()
  self:msg("Card mounted by USB host.")
end

function Console:onUSBConnect()
  self:msg("USB host connected.")
end

function Console:onUSBDisconnect()
  self:msg("USB host disconnected.")
end

local threshold = Env.EncoderThreshold.Default
function Console:encoder(change, shifted)
  self.console:encoder(change, shifted, threshold)
  return true
end

function Console:printInfo(x)
  self:msg("Card Version: %s", x:getVersion())
  self:msg("Bus Width: %s bits", x:getBusWidth())
  self:msg("Bus Speed: %s Mhz", x:getTransferSpeed())
  self:msg("High Capacity: %s", x:isHighCapacity())
  self:msg("CMD23 Supported: %s", x:supportsCMD23())
  Busy.start("Measuring read/write speed...")
  x:measureSpeed(10 * 1024 * 1024, 3, 64 * 1024)
  Busy.stop()
  local theoretical = x:getBusWidth() * x:getTransferSpeed() / 8
  local speed = x:getReadSpeed() / (1024 * 1024)
  self:msg("Theoretical Maximum Speed: %3.2f MB/s", theoretical)
  self:msg("Actual Read Speed: %3.2f MB/s", speed)
  local speed = x:getWriteSpeed() / (1024 * 1024)
  self:msg("Actual Write Speed: %3.2f MB/s", speed)
end

function Console:subReleased(i, shifted)
  if shifted and i == 1 then
    self:msg("*** Rear Card Info ***")
    self:printInfo(rearCardInfo)
    return true
  end
  if shifted then return false end
  if i == 1 then
    if not Card.mounted() then
      self:msg("Card is not mounted.")
    else
      self:msg("*** Front Card Info ***")
      self:printInfo(frontCardInfo)
    end
  elseif i == 2 then
    if not Card.mounted() then
      self:msg("Format aborted. Card is not mounted.")
    elseif Card.busy() then
      self:msg("Format aborted. Files in use.")
    else
      local task = function(ans)
        if ans then
          if Card.format() then
            Overlay.flashMainMessage("Format was successful.")
          else
            Overlay.flashMainMessage("Format failed.")
          end
        end
      end
      local dialog = Verification.Main("Are you sure you want to format?",
                                       "All data will be lost!")
      dialog:subscribe("done", task)
      dialog:show()
    end
  elseif i == 3 then
    statusViewer:show()
  end
  return true
end

function Console:upReleased()
  self:hide()
  return true
end

function Console:cancelReleased()
  self:hide()
  return true
end

function Console:homeReleased()
  self:hide()
  return true
end

return Console()
