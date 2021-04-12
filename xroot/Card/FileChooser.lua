local Busy = require "Busy"
local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Window = require "Base.Window"
local Keyboard = require "Keyboard"
local Path = require "Path"
local Utils = require "Utils"
local Signal = require "Signal"
local Timer = require "Timer"
local Settings = require "Settings"
local FS = require "Card.FileSystem"
local threshold = Env.EncoderThreshold.Default

local FileChooser = Class {}
FileChooser:include(Window)

-- opts is a table containing any of the following keys:
-- msg, goal, path, pattern, ext, suggested, history, previewLoop, auditionChannel
-- valid goals:
--   save file
--   save folder
--   load file
--   load files
function FileChooser:init(opts)
  Window.init(self)
  self:setClassName("Card.FileChooser")

  -- default options
  opts.path = opts.path or app.roots.front

  self.multi = false
  self.staged = opts.staged or 0
  self.extension = opts.ext
  self.previewLoop = opts.previewLoop
  self.auditionChannel = opts.auditionChannel

  local browser = app.FileBrowser(0, 0, 256, 64)
  self.browser = browser
  self.mainGraphic:addChild(browser)

  local Drawings = require "Drawings"
  local drawing = app.Drawing(0, 0, 128, 64)
  drawing:add(Drawings.Sub.HelpfulButtons)
  self.subGraphic:addChild(drawing)

  -- Try to set initial path and choice via history
  local hasHistory = false
  if opts.history then
    -- app.logInfo("FileChooser: history=%s", opts.history)
    self.history = opts.history
    local Persist = require "Persist"
    local data = Persist.getFrontCardValue("FileChooser", opts.history)
    if data and data.path and data.choice then
      self:navigateTo(data.path)
      self:choose(data.choice)
      hasHistory = true
    end
  end

  if not hasHistory then
    self:navigateTo(opts.path)
    if opts.suggested then self:choose(opts.suggested) end
  end

  self.goal = opts.goal
  if opts.goal == "save file" then
    local label = app.Label(opts.msg or "Choose File", 10)
    label:setJustification(app.justifyCenter)
    label:setCenter(64, (65 + app.GRID5_LINE1) / 2)
    -- label:setPosition(3, app.GRID5_LINE1)
    self.subGraphic:addChild(label)
    label = app.Label("ENTER to choose file.", 10)
    label:setJustification(app.justifyLeft)
    label:setPosition(3, app.GRID5_LINE2 - 1)
    self.subGraphic:addChild(label)
    label = app.Label("CANCEL to abort.", 10)
    label:setJustification(app.justifyLeft)
    label:setPosition(3, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    -- sub buttons
    label = app.TextPanel("", 1, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S1 = label
    label = app.TextPanel("New File", 2, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S2 = label
    label = app.TextPanel("New Folder", 3, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S3 = label
  elseif opts.goal == "save folder" then
    browser:showOnlyFolders(true)
    local label = app.Label(opts.msg or "Choose Folder", 10)
    label:setJustification(app.justifyCenter)
    label:setCenter(64, (65 + app.GRID5_LINE1) / 2)
    -- label:setPosition(3, app.GRID5_LINE1)
    self.subGraphic:addChild(label)
    label = app.Label("ENTER to choose folder.", 10)
    label:setJustification(app.justifyLeft)
    label:setPosition(3, app.GRID5_LINE2 - 1)
    self.subGraphic:addChild(label)
    label = app.Label("CANCEL to abort.", 10)
    label:setJustification(app.justifyLeft)
    label:setPosition(3, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    -- sub buttons
    label = app.TextPanel("", 1, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S1 = label
    label = app.TextPanel("", 2, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S2 = label
    label = app.TextPanel("New Folder", 3, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S3 = label
  elseif opts.goal == "load file" then
    local label = app.Label(opts.msg or "Choose File", 10)
    label:setJustification(app.justifyCenter)
    label:setCenter(64, (65 + app.GRID5_LINE1) / 2)
    -- label:setPosition(3, app.GRID5_LINE1)
    self.subGraphic:addChild(label)
    label = app.Label("ENTER to choose file.", 10)
    label:setJustification(app.justifyLeft)
    label:setPosition(3, app.GRID5_LINE2 - 1)
    self.subGraphic:addChild(label)
    label = app.Label("CANCEL to abort.", 10)
    label:setJustification(app.justifyLeft)
    label:setPosition(3, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    -- sub buttons
    label = app.TextPanel("", 1, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S1 = label
    label = app.TextPanel("", 2, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S2 = label
    label = app.TextPanel("", 3, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S3 = label
  elseif opts.goal == "load files" then
    self.msg = opts.msg or "Choose File"
    local label = app.Label(self.msg, 10)
    self.titleLabel = label
    label:setJustification(app.justifyCenter)
    label:setCenter(64, (65 + app.GRID5_LINE1) / 2)
    -- label:setPosition(3, app.GRID5_LINE1)
    self.subGraphic:addChild(label)
    label = app.Label("When done press ENTER.", 10)
    self.label1 = label
    label:setJustification(app.justifyLeft)
    label:setPosition(3, app.GRID5_LINE2 - 1)
    self.subGraphic:addChild(label)
    label = app.Label("CANCEL to abort.", 10)
    self.label2 = label
    label:setJustification(app.justifyLeft)
    label:setPosition(3, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    -- sub buttons
    label = app.TextPanel("", 1, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S1 = label
    label = app.TextPanel("", 2, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S2 = label
    label = app.TextPanel("Enable Multi", 3, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    self.S3 = label
    if self.staged > 0 then self:toggleMultiSelection() end
  end

  Signal.weakRegister("cardMounted", self)
  Signal.weakRegister("cardEjecting", self)

  self:setPattern(opts.pattern)
end

function FileChooser:cardMounted()
  self.browser:notifyMounted()
end

function FileChooser:cardEjecting()
  self:stopPreview()
  self.browser:notifyEjected()
end

function FileChooser:choose(fileOrFolder)
  self.browser:choose(fileOrFolder)
  if self.extension then
    -- strip off the extension
    fileOrFolder = fileOrFolder:gsub("[.]%w*$", "", 1)
  end
  self.suggested = fileOrFolder
end

function FileChooser:navigateTo(path)
  -- app.logInfo("FileChooser:navigateTo(%s)", path)
  local browser = self.browser
  local root
  if Utils.startsWith(path, app.roots.front) then
    root = app.roots.front
  elseif Utils.startsWith(path, app.roots.rear) then
    root = app.roots.rear
  else
    root = app.roots.x
  end
  -- app.logInfo("FileChooser:setRootPath(%s)", root)
  browser:setRootPath(root)
  local rootFolders = Utils.split(root, Path.sep)
  local dirs = Utils.split(path, Path.sep)
  for i = #rootFolders + 1, #dirs do
    -- app.logInfo("FileChooser:push(%s)", dirs[i])
    browser:pushDirectory(dirs[i])
  end
  browser:refresh()
end

function FileChooser:validateFilename(filename)
  if self.extension and not Utils.endsWith(filename, self.extension) then
    filename = filename .. self.extension
  end
  return filename
end

function FileChooser:validateFoldername(foldername)
  return foldername
end

function FileChooser:setPattern(pattern)
  local browser = self.browser
  if pattern then
    if type(pattern) == "table" then
      browser:clearPatterns()
      for _, glob in ipairs(pattern) do browser:addPattern(glob) end
    else
      browser:setPattern(pattern)
    end
  else
    browser:clearPatterns()
  end
  browser:refresh()
end

function FileChooser:finish(filename, path, overwrite)
  if self.goal == "save file" then
    local browser = self.browser
    path = path or browser:getSelectedPath()
    filename = self:validateFilename(filename or browser:getSelectedName())
    local fullpath = Path.join(path, filename)
    if filename == nil or filename == "" or Path.isDirectory(fullpath) then
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("This choice is not a file ^(;,;)^")
      return
    elseif not overwrite and Path.exists(fullpath) then
      -- overwriting existing file...
      local Verification = require "Verification"
      local task = function(ans)
        if ans then self:finish(filename, path, true) end
      end
      local dialog = Verification.Main("Is it OK to overwrite this file?",
                                       filename)
      dialog:subscribe("done", task)
      dialog:show()
      return
    end
    -- Save the path and choice in the history.
    if self.history then
      local Persist = require "Persist"
      local data = {
        path = path,
        choice = filename
      }
      Persist.setFrontCardValue("FileChooser", self.history, data)
    end
    local result = {
      multi = false,
      filename = filename,
      path = path,
      fullpath = fullpath
    }
    self:hide()
    self:emitSignal("done", result)
  elseif self.goal == "save folder" then
    local browser = self.browser
    path = path or browser:getSelectedPath()
    filename = self:validateFoldername(filename or browser:getSelectedName())
    local fullpath = Path.join(path, filename)
    if filename == nil or filename == "" or not Path.isDirectory(fullpath) then
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("This choice is not a folder ^(;,;)^")
      return
    end
    -- Save the path and choice in the history.
    if self.history then
      local Persist = require "Persist"
      local data = {
        path = path,
        choice = filename
      }
      Persist.setFrontCardValue("FileChooser", self.history, data)
    end
    local result = {
      multi = false,
      foldername = filename,
      path = path,
      fullpath = fullpath
    }
    self:hide()
    self:emitSignal("done", result)
  elseif self.goal == "load file" or self.goal == "load files" then
    local browser = self.browser

    path = path or browser:getSelectedPath()
    filename = self:validateFilename(filename or browser:getSelectedName())
    if self.history then
      local Persist = require "Persist"
      local data = {
        path = path,
        choice = filename
      }
      Persist.setFrontCardValue("FileChooser", self.history, data)
    end

    if self.multi then
      -- multiple files
      local selectedPaths = self.browser:getCheckedPaths()
      if #selectedPaths == 0 then
        local Overlay = require "Overlay"
        Overlay.mainFlashMessage("Nothing is selected ^(;,;)^")
        return
      end
      local result = {
        multi = true,
        paths = selectedPaths
      }
      self:hide()
      self:emitSignal("done", result)
    else
      local fullpath = Path.join(path, filename)
      if filename == nil or filename == "" or Path.isDirectory(fullpath) then
        local Overlay = require "Overlay"
        Overlay.mainFlashMessage("This choice is not a file ^(;,;)^")
        return
      end
      local result = {
        multi = false,
        filename = filename,
        path = path,
        fullpath = fullpath
      }
      self:hide()
      self:emitSignal("done", result)
    end
  end
end

function FileChooser:abort()
  self:hide()
  self:emitSignal("done")
end

function FileChooser:getPlayer()
  if self.player == nil then
    local Player = require "Card.Player"
    self.player = Player()
    if self.auditionChannel then
      self.player:setDefaultChannel(self.auditionChannel)
    end
  end
  return self.player
end

function FileChooser:stopUpdateTimer()
  if self.timerHandle then
    Timer.cancel(self.timerHandle)
    self.timerHandle = nil
  end
end

function FileChooser:startUpdateTimer()
  self:stopUpdateTimer()
  self.timerHandle = Timer.every(0.2, function()
    self:updatePreview()
  end)
end

function FileChooser:doPreview()
  if self.player and self.player.state == "Playing" then
    self.player:stop()
    self.browser:setFileSource(nil)
    self:stopUpdateTimer()
  else
    local path = self.browser:getSelectedFullPath()
    local player = self:getPlayer()
    if FS.isType("audio", path) and player:play(path, nil, self.previewLoop) then
      local sticky = Settings.get("samplePreviewStop") == "no"
      self.browser:setFileSource(player:getFileSource(), sticky)
      self:startUpdateTimer()
    end
  end
end

function FileChooser:stopPreview()
  if self.player and self.player.state == "Playing" then
    self.player:stop()
    self.browser:setFileSource(nil)
    self:stopUpdateTimer()
  end
end

function FileChooser:updatePreview()
  local player = self.player
  if player then
    if Settings.get("samplePreviewStop") == "no" then
      local selected = self.browser:getSelectedFullPath()
      local playing = self.player:getPath()
      if playing ~= selected then
        player:stop()
        self.browser:setFileSource(nil)
        if FS.isType("audio", selected) and player:play(selected, nil, self.previewLoop) then
          self.browser:setFileSource(player:getFileSource(), true)
        else
          self:stopUpdateTimer()
        end
      end
    elseif player:isEOF() then
      self:stopPreview()
    end
  end
end

function FileChooser:doOpenFolder()
  local browser = self.browser
  local selection = browser:getSelectedName()
  browser:pushDirectory(selection)
  browser:refresh()
end

function FileChooser:doCloseFolder()
  local browser = self.browser
  if browser:popDirectory() then browser:refresh() end
end

function FileChooser:doNewFile()
  local task = function(text)
    if text ~= nil then
      local filename = self:validateFilename(text)
      local path = self.browser:getSelectedPath()
      self:finish(filename, path)
    end
  end
  local kb = Keyboard("Enter filename:", self.suggested, false, "NamingStuff")
  kb:subscribe("done", task)
  kb:show()
end

function FileChooser:doNewFolder()
  local task = function(text)
    if text ~= nil then
      local folderName = self:validateFoldername(text)
      local path = self.browser:getSelectedPath()
      local fullpath = Path.join(path, folderName)
      app.logInfo("%s: creating [%s] [%s] [%s]", self, folderName, path,
                  fullpath)
      Path.createAll(fullpath)
      self.browser:refresh()
      self.browser:choose(folderName)
    end
  end
  local suggested = self.suggested
  if self.goal == "save file" then suggested = "" end
  local kb = Keyboard("Enter folder name:", suggested, false, "NamingStuff")
  kb:subscribe("done", task)
  kb:show()
end

function FileChooser:toggleMultiSelection()
  if self.multi then
    self.multi = false
    self.browser:hideCheck()
    self.browser:clearChecked()
    self.S3:setText("Enable Multi")
    self.S1:setText("")
    self.S2:setText("")
    self.titleLabel:setText(self.msg)
    self.label1:setText("When done press ENTER.")
    self.label2:setText("CANCEL to abort.")
  else
    self.multi = true
    self.browser:showCheck()
    self.S3:setText("Disable Multi")
    self.S1:setText("Select None")
    self.S2:setText("Select All")
    self.label1:setText("Press ENTER to toggle.")
    self.label2:setText("SHIFT+ENTER to finish.")
    self:updateTitleLabel()
  end
end

function FileChooser:updateTitleLabel()
  local n = self.browser:countCheckedPaths()
  local text = string.format("%d selected (%d staged)", n, self.staged)
  self.titleLabel:setText(text)
end

function FileChooser:toggleSelected()
  Busy.start()
  self.browser:toggleSelected()
  self:updateTitleLabel()
  Busy.stop()
end

function FileChooser:checkAll()
  Busy.start()
  self.browser:checkAll()
  self:updateTitleLabel()
  Busy.stop()
end

function FileChooser:checkNone()
  Busy.start()
  self.browser:checkNone()
  self:updateTitleLabel()
  Busy.stop()
end

-------------------------------------------------
-- event handlers

function FileChooser:onHide()
  self:stopPreview()
end

function FileChooser:encoder(change, shifted)
  if self.browser:encoder(change, shifted, threshold) then
    if Settings.get("samplePreviewStop") == "yes" then self:stopPreview() end
  end
  return true
end

function FileChooser:enterReleased()
  self:stopPreview()
  if self.multi then
    self:toggleSelected()
  else
    self:finish()
  end
  return true
end

function FileChooser:commitReleased()
  self:stopPreview()
  self:finish()
  return true
end

function FileChooser:cancelReleased(shifted)
  if shifted then return false end
  self:stopPreview()
  self:abort()
  return true
end

function FileChooser:upReleased(shifted)
  if shifted then return false end
  self:stopPreview()
  if self.multi and self.browser:countCheckedPaths() > 0 then
    self:finish()
  else
    self:abort()
  end
  return true
end

function FileChooser:mainReleased(i, shifted)
  if shifted then return false end
  if i == 4 then
    self:stopPreview()
    if self.multi then self:toggleSelected() end
  elseif i == 5 then
    self:stopPreview()
    self:doCloseFolder()
  elseif i == 6 then
    if self.browser:isDirectory() then
      self:doOpenFolder()
    elseif self.player and self.player.state == "Playing" then
      self:stopPreview()
    else
      self:doPreview()
    end
  end
  return true
end

function FileChooser:subReleased(i, shifted)
  if shifted then return false end
  self:stopPreview()
  if self.goal == "save folder" then
    if i == 3 then self:doNewFolder() end
  elseif self.goal == "save file" then
    if i == 2 then
      self:doNewFile()
    elseif i == 3 then
      self:doNewFolder()
    end
  elseif self.goal == "load file" then
    -- nothing to do
  elseif self.goal == "load files" then
    if i == 1 then
      if self.multi then self:checkNone() end
    elseif i == 2 then
      if self.multi then self:checkAll() end
    elseif i == 3 then
      self:toggleMultiSelection()
    end
  end
  return true
end

function FileChooser:selectPressed(i, shifted)
  if shifted then return false end
  local Overlay = require "Overlay"
  local Channels = require "Channels"
  local chain = Channels.getChain(i)
  Overlay.mainFlashMessage("Audition Output: " .. chain.title)
  self:getPlayer():changeDestination(i)
  return true
end

return FileChooser
