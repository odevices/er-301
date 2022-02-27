local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Window = require "Base.Window"
local Path = require "Path"
local Utils = require "Utils"
local Signal = require "Signal"
local FS = require "Card.FileSystem"

local threshold = Env.EncoderThreshold.Default

local function showMessage(...)
  local Overlay = require "Overlay"
  Overlay.flashMainMessage(...)
end

local PresetChooser = Class {}
PresetChooser:include(Window)

function PresetChooser:init(ring)
  local browser = app.FileBrowser(0, 0, 256, 64)
  self.browser = browser
  Window.init(self, browser)
  self:setClassName("Unit.Chooser.Preset")
  self.ring = ring

  Signal.weakRegister("cardMounted", self)
  Signal.weakRegister("cardEjecting", self)

  for _, pattern in ipairs(FS.getPattern("unit-preset")) do
    browser:addPattern(pattern)
  end
  for _, pattern in ipairs(FS.getPattern("chain-preset")) do
    browser:addPattern(pattern)
  end

  local Persist = require "Persist"
  local data = Persist.getFrontCardValue("UnitPresetChooser", "lastChoice")
  if data and data.path then
    self:navigateTo(data.path)
    if data.choice then
      browser:choose(data.choice)
    end
  else
    self:navigateTo(FS.getRoot("unit-preset"))
  end
end

function PresetChooser:cardMounted()
  self.browser:notifyMounted()
end

function PresetChooser:cardEjecting()
  self.browser:notifyEjected()
end

function PresetChooser:navigateTo(path)
  local dirs = Utils.split(path, Path.sep)
  local browser = self.browser
  browser:setRootPath(dirs[1])
  for i = 2, #dirs do
    browser:pushDirectory(dirs[i])
  end
  browser:refresh()
end

function PresetChooser:finish()
  local browser = self.browser

  if browser:isEmpty() then
    showMessage("Nothing is selected ^(;,;)^")
    return
  end

  if browser:isDirectory() then
    showMessage("This choice is not a file ^(;,;)^")
    return
  end

  local fullpath = browser:getSelectedFullPath();
  self:setLastChoice()
  self.ring:loadPreset(fullpath)
end

function PresetChooser:setLastChoice()
  -- Save the path and choice in the history.
  local browser = self.browser
  local filename = browser:getSelectedName()
  local path = browser:getSelectedPath()
  local Persist = require "Persist"
  local data = {
    path = path,
    choice = filename
  }
  Persist.setFrontCardValue("UnitPresetChooser", "lastChoice", data)
end

function PresetChooser:doOpenFolder()
  local browser = self.browser
  local selection = browser:getSelectedName()
  browser:pushDirectory(selection)
  browser:refresh()
end

function PresetChooser:doCloseFolder()
  local browser = self.browser
  if browser:popDirectory() then
    browser:refresh()
  end
end

-------------------------------------------------
-- event handlers

function PresetChooser:encoder(change, shifted)
  self.browser:encoder(change, shifted, threshold)
  return true
end

function PresetChooser:enterReleased()
  self:finish()
  return true
end

function PresetChooser:subReleased(i, shifted)
  return self.ring:subReleased(i, shifted)
end

function PresetChooser:cancelReleased(shifted)
  return self.ring:cancelReleased(shifted)
end

function PresetChooser:upReleased(shifted)
  return self.ring:upReleased(shifted)
end

function PresetChooser:mainReleased(i, shifted)
  if shifted then
    return false
  end
  if i == 5 then
    self:doCloseFolder()
  elseif i == 6 then
    if self.browser:isDirectory() then
      self:doOpenFolder()
    end
  end
  return true
end

return PresetChooser
