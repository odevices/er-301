local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Path = require "Path"
local Window = require "Base.Window"
local Verification = require "Verification"
local Busy = require "Busy"
local FileChooser = require "Card.FileChooser"

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

  local offset = -5

  local version = "Version: " .. app.FIRMWARE_VERSION
  label = app.Label(version, 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(0, app.GRID5_LINE2 + offset)
  self.subGraphic:addChild(label)

  local build = "Build: " .. app.BUILD_PROFILE
  label = app.Label(build, 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(0, app.GRID5_LINE3 + offset)
  self.subGraphic:addChild(label)

  label = app.SubButton("update", 1)
  self.subGraphic:addChild(label)
  self.updateButton = label

  label = app.SubButton("reboot", 3)
  self.subGraphic:addChild(label)
  self.rebootButton = label

  self:msg("Press 'update' to install a new firmware.")
end

function Firmware:msg(...)
  text = string.format(...)
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
      self:install(result.fullpath)
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

function Firmware:defaultInstallation(archive)
  -- First extract all files to a temporary location.
  local renames = {}
  local n = archive:getFileCount()

  for i = 1, n do
    local filename = archive:getFilename(i - 1)
    app.logInfo("Trying to extract and copy %s.", filename)
    self:msg("Installing %s...", filename)
    if not archive:exists(filename) then
      self:msg("Update failed: %s was not found.", filename)
      return false
    end
    local tmpFilename = "_" .. filename
    local tmpPath = Path.join(app.roots.rear, tmpFilename)
    local actualPath = Path.join(app.roots.rear, filename)
    if archive:extract(filename, tmpPath) then
      app.logInfo("%s copied successfully.", filename)
      renames[#renames + 1] = {tmp = tmpPath, actual = actualPath}
    else
      self:msg("Update failed: could not copy %s.", filename)
      for _, e in ipairs(renames) do
        self:msg("Removing %s.", e.tmp)
        app.deleteFile(e.tmp)
      end
      return false
    end
  end

  -- All files extracted successfully, so rename them to their required location.
  self:msg("Commit newly installed files...")
  for _, e in ipairs(renames) do
    app.moveFile(e.tmp, e.actual, true)
  end
  self:msg("Done.")

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

  self:msg("Reading %s...", filename)
  if archive:open(filename) then
    app.logInfo("Opened archive: %s", filename)
  else
    app.logError("Failed to open archive: %s", filename)
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
        app.logError("Error loading script: %s", err)
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
      app.logError("Install script failed to copy.")
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
  if shifted then
    return false
  end
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
