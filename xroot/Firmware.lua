local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Path = require "Path"
local Window = require "Base.Window"
local Verification = require "Verification"
local Message = require "Message"
local Card = require "Card"
local Busy = require "Busy"
local FileChooser = require "Card.FileChooser"

local pattern = "*.zip"

local Firmware = Class {}
Firmware:include(Window)

function Firmware:init()
  Window.init(self)
  self:setClassName("Firmware")

  local console = app.ListBox(0, 0, 256, 64)
  console:setTextSize(10)
  console:setFocus()
  self.mainGraphic:addChild(console)
  self.console = console

  local Drawings = require "Drawings"
  local drawing = app.Drawing(0, 0, 128, 64)
  drawing:add(Drawings.Sub.TitleLine)
  self.subGraphic:addChild(drawing)

  local label = app.Label("Current Firmware", 12)
  label:setJustification(app.justifyLeft)
  label:setPosition(0, app.GRID4_LINE1 + 1)
  self.subGraphic:addChild(label)

  label = app.Label("Version: " .. Env.Version.String, 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(0, app.GRID4_LINE2)
  self.subGraphic:addChild(label)

  label = app.Label(Env.Version.Name, 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(0, app.GRID4_LINE3)
  self.subGraphic:addChild(label)

  label = app.SubButton("update", 1)
  self.subGraphic:addChild(label)
  self.updateButton = label

  label = app.SubButton("reboot", 3)
  self.subGraphic:addChild(label)
  self.rebootButton = label

  self:msg("Press 'update' to install a new firmware.")
end

function Firmware:msg(text)
  self.console:addItem(text)
  self.console:scrollToBottom()
end

function Firmware:update()
  -- make sure the rear card is mounted
  if not app.Card_isMounted(0) then
    if not app.Card_mount(0) then
      app.logInfo("No rear card inserted.")
      self:msg("No rear card inserted.")
      return
    end
  end

  local task = function(result)
    if result then
      self.rebootButton:hide()
      self.updateButton:hide()
      Busy.enable()
      self:backup()
      if not self:install(result.fullpath) then self:restore() end
      Busy.disable()
      self.rebootButton:show()
      self.updateButton:show()
    end
  end
  local chooser = FileChooser {
    msg = "Choose Firmware",
    goal = "load file",
    pattern = "*.zip",
    history = "firmware"
  }
  chooser:subscribe("done", task)
  chooser:show()
end

local function backup(orig, backup)
  if app.pathExists(orig) then app.moveFile(orig, backup, true) end
end

local function restore(orig, backup)
  if app.pathExists(backup) then app.moveFile(backup, orig, true) end
end

function Firmware:backup()
  backup(app.roots.rear .. "/MLO", app.roots.rear .. "/MLO_backup")
  backup(app.roots.rear .. "/SBL", app.roots.rear .. "/SBL_backup")
  backup(app.roots.rear .. "/kernel.bin", app.roots.rear .. "/kernel_backup.bin")
end

function Firmware:restore()
  restore(app.roots.rear .. "/MLO", app.roots.rear .. "/MLO_backup")
  restore(app.roots.rear .. "/SBL", app.roots.rear .. "/SBL_backup")
  restore(app.roots.rear .. "/kernel.bin",
          app.roots.rear .. "/kernel_backup.bin")
end

function Firmware:defaultInstallation(archive)
  local files = {
    "MLO",
    "SBL",
    "kernel.bin"
  }

  local descriptions = {
    "initial boot loader",
    "secondary boot loader",
    "kernel"
  }

  for i, filename in ipairs(files) do
    app.logInfo("Trying to extract and copy %s.", filename)
    self:msg("Installing %s...", descriptions[i])
    if not archive:exists(filename) then
      self:msg("Update failed: %s was not found.", filename)
      return false
    end
    if archive:extract(filename, Path.join(app.roots.rear, filename)) then
      app.logInfo("%s copied successfully.", filename)
    else
      self:msg("Update failed: could not copy %s.", filename)
      return false
    end
  end

  return true
end

local function onError(msg)
  local trace = debug.traceback(nil, 2)
  print(msg)
  print(trace)
end

function Firmware:install(filename)
  local installScript = "install.lua"
  local installScriptPath = Path.join(app.roots.rear, installScript)
  local archive = app.ZipArchiveReader()
  archive:setIgnorePath(true)

  app.logInfo("Opening firmware archive: %s.", filename)
  self:msg("Reading zip file...")
  if archive:open(filename) then
    app.logInfo("Opened archive: %s", filename)
  else
    app.logInfo("Failed to open archive: %s", filename)
    self:msg("Failed to open zip file.")
    return false
  end

  local success
  if archive:exists(installScript) then
    self:msg("Found custom installation script.")
    if app.pathExists(installScriptPath) then
      app.deleteFile(installScriptPath)
    end
    if archive:extract(installScript, installScriptPath) then
      app.logInfo("Install script copied successfully.")
      local script, err = loadfile(installScriptPath)
      if script == nil then
        app.logInfo("Error loading script: %s", err)
        success = false
      else
        local ok, result = xpcall(script, onError)
        if ok and result.install then
          local task = function()
            return result.install(self, archive)
          end
          local ok2, result2 = xpcall(task, onError)
          if ok2 then
            success = result2
          else
            success = false
          end
        else
          success = false
        end
      end
    else
      app.logInfo("Install script failed to copy.")
      self:msg("Update failed: could not extract install script.")
      success = false
    end
  else
    success = self:defaultInstallation(archive)
  end
  archive:close()

  if success then
    self:msg("Update succeeded. Reboot to use new firmware.")
    return true
  else
    return false
  end
end

function Firmware:getPreview()
  return self.subGraphic
end

------------------------------------
-- event handlers

local threshold = Env.EncoderThreshold.Default
function Firmware:encoder(change, shifted)
  self.console:encoder(change, shifted, threshold)
  return true
end

function Firmware:subReleased(i, shifted)
  if shifted then return false end
  if i == 1 and not self.updateButton:isHidden() then
    self:update()
  elseif i == 3 and not self.rebootButton:isHidden() then
    local dialog = Verification.Main("Do you really want to reboot?")
    local task = function(ans)
      if ans then
        app.reboot()
      else
        self:msg("Reboot aborted.")
      end
    end
    dialog:subscribe("done", task)
    dialog:show()
  end
  return true
end

function Firmware:upReleased()
  self:hide()
  return true
end

function Firmware:cancelReleased()
  self:hide()
  return true
end

function Firmware:homeReleased()
  self:hide()
  return true
end

return Firmware()
