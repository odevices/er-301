local app = app
local Class = require "Base.Class"
local SpottedStrip = require "SpottedStrip"
local TrackSection = require "FileRecorder.TrackSection"
local SourceChooser = require "Source.Chooser"
local FileChooser = require "Card.FileChooser"
local Path = require "Path"
local Busy = require "Busy"
local Card = require "Card"
local FS = require "Card.FileSystem"
local Timer = require "Timer"
local Overlay = require "Overlay"
local Settings = require "Settings"

local FileRecorder = Class {}
FileRecorder:include(SpottedStrip)

function FileRecorder:init(root)
  SpottedStrip.init(self)
  self:setClassName("FileRecorder")
  self.root = root
  self.channelCount = 6
  self.task = app.ObjectList("recorder")
  self.thread = app.FileSinkThread("recorder")
  self.sinks = {}
  self.configs = {}
  self.state = "setup"
  self:onChannelCountChanged()

  local graphic
  self.setupMenu = app.Graphic(0, 0, 128, 64)
  graphic = app.TextPanel("Load Preset", 1, 0.5, app.GRID5_LINE3)
  self.setupMenu:addChild(graphic)
  graphic = app.TextPanel("Record", 2, 0.5, app.GRID5_LINE3)
  self.setupMenu:addChild(graphic)
  graphic = app.TextPanel("Save Preset", 3, 0.5, app.GRID5_LINE3)
  self.setupMenu:addChild(graphic)
  self:addSubGraphic(self.setupMenu)

  local label = app.Label("M1-M6 to assign sources.", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(3, app.GRID5_LINE1)
  self.setupMenu:addChild(label)
  label = app.Label("ENTER to link/unlink/assign.", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(3, app.GRID5_LINE2)
  self.hint = label
  self.setupMenu:addChild(label)
  label = app.Label("ZERO to reset all.", 10)
  label:setJustification(app.justifyLeft)
  label:setPosition(3, app.GRID5_LINE3)
  self.setupMenu:addChild(label)

  self.recordingMenu = app.RecordingGraphic(self.thread)

  self.stoppedMenu = app.Graphic(0, 0, 128, 64)
  graphic = app.TextPanel("Discard Audio", 2)
  self.stoppedMenu:addChild(graphic)
  graphic = app.TextPanel("Save Audio", 3)
  self.stoppedMenu:addChild(graphic)

  self.thread:start()
  self:updateHints()
end

function FileRecorder:onChannelCountChanged()
  local choice = Settings.get("fileRecorderChannelCount")
  self.channelCount = tonumber(choice)
  local tempPaths = {}
  for i = 1, self.channelCount do
    local filename = string.format("tmp-%d.wav", i)
    tempPaths[i] = Path.join(self.root, filename)
  end
  self.paths = tempPaths
  self:buildTracks()
end

function FileRecorder:clearTracks()
  self.configs = {}
  self:buildTracks()
end

function FileRecorder:updateHints()
  local track, viewName, spotHandle = self:getSelection()
  local view = track.views[viewName]
  if view then
    local spot = view:getSpotByHandle(spotHandle)
    if spot then
      local action = spot:getValue("action")
      if action == "link" then
        self.hint:setText("ENTER links to stereo.")
        Overlay.flashMainMessage("ENTER links to stereo.")
      elseif action == "unlink" then
        self.hint:setText("ENTER splits back to mono.")
        Overlay.flashMainMessage("ENTER splits back to mono.")
      else
        self.hint:setText("Press HOME to clear.")
      end
    end
  end
end

function FileRecorder:startRecording()
  if self.state ~= "setup" then
    return false
  end
  local noSources = true
  for i, config in ipairs(self.configs) do
    if config.stereo then
      if config.source1 or config.source2 then
        local sink = app.StereoFileSink()
        if config.source1 then
          app.connect(config.source1:getOutlet(), sink:getInput("Left In"))
          noSources = false
        end
        if config.source2 then
          app.connect(config.source2:getOutlet(), sink:getInput("Right In"))
          noSources = false
        end
        self.sinks[i] = sink
      end
    else
      if config.source then
        local sink = app.MonoFileSink()
        app.connect(config.source:getOutlet(), sink:getInput("In"))
        noSources = false
        self.sinks[i] = sink
      end
    end
  end

  if noSources then
    Overlay.flashMainMessage("Assign at least one channel.")
    return false
  elseif not Card.mounted() then
    Overlay.flashMainMessage("The SD card is not mounted.")
    return false
  else
    Busy.start("Preparing files for recording...")
    self.state = "waiting"
    for i, sink in pairs(self.sinks) do
      if sink then
        sink:setEncoding(app.wavFloat)
        sink:setFilename(self.paths[i])
        Busy.status("Opening %s", self.paths[i])
        if sink:open() then
          sink:start()
          self.task:add(sink)
          self.thread:add(sink)
          Card.claim("Recording to", self.paths[i])
        else
          app.logError("%s.startRecording: Failed to open track %d: %s.", self,
                       i, self.paths[i])
        end
      end
    end
    if not self.thread:allocateCache() then
      Overlay.flashMainMessage("Not enough memory for cache.")
    end
    app.AudioThread.addTask(self.task, -1)
    self.state = "recording"
    self.subGraphic:clear()
    self.subGraphic:addChild(self.recordingMenu)
    self.recordingMenu:reset()
    self:hideCursor()
    Busy.stop()
    Timer.script(function(wait)
      while self.state == "recording" or self.state == "paused" do
        if self.thread:getOverflowCount() > 99 then
          self:recordingFailed()
          break
        else
          wait(1)
        end
      end
    end)
    return true
  end
end

function FileRecorder:recordingFailed()
  self:stopRecording(false)
  self.state = "failed"
  self.recordingMenu:notifyFailed(true)
  local Message = require "Message"
  local dialog = Message.Main("Too many buffer overflows.",
                              "Please use a faster SD card.")
  dialog:subscribe("done", function()
    self:discardRecording()
  end)
  dialog:show()
end

function FileRecorder:pauseRecording()
  if self.state == "recording" then
    app.AudioThread.removeTask(self.task)
    self.state = "paused"
    self.recordingMenu:notifyPaused(true)
  end
end

function FileRecorder:unpauseRecording()
  if self.state == "paused" then
    app.AudioThread.addTask(self.task, -1)
    self.state = "recording"
    self.recordingMenu:notifyPaused(false)
  end
end

local function suggestSaveFolder(root)
  local path, folder
  for i = 1, 999 do
    folder = string.format("save%03d", i)
    path = Path.join(root, folder)
    if not app.pathExists(path) then
      break
    end
  end
  return folder
end

function FileRecorder:saveFilename(base, stereo)
  if self.saveIndex then
    if stereo then
      return
    else
      return string.format("%s-T%d-stereo.wav", base, self.saveIndex)
    end
  else
    return string.format("%s-%d.wav", base, self.index)
  end
end

function FileRecorder:hasSavedTracks(base, path)
  for i, sink in pairs(self.sinks) do
    local filename = string.format("%s-T%d.wav", base, i)
    local destPath = Path.join(path, filename)
    if app.pathExists(destPath) then
      return true
    end
  end
  return false
end

function FileRecorder:getMaxTake(base, path)
  local maxTake = 1
  for x in dir(path) do
    if string.find(x, base) then
      local a, b = string.find(x, "-take[0-9][0-9]*-T")
      if a and b then
        local tmp = tonumber(string.sub(x, a + 5, b - 2))
        if tmp and tmp > maxTake then
          maxTake = tmp
        end
      end
    end
  end
  return maxTake
end

function FileRecorder:saveSingleTrackTo(i, path)
  app.moveFile(self.paths[i], path, true)
  Overlay.flashMainMessage("Saved to %s", path)
  for i, path in ipairs(self.paths) do
    Card.release(path)
  end
  self.sinks = {}
  self.state = "setup"
  self.subGraphic:clear()
  self.subGraphic:addChild(self.setupMenu)
  self:showCursor()
end

function FileRecorder:saveMultipleTracksTo(base, path)
  if self:hasSavedTracks(base, path) then
    local nextTake = self:getMaxTake(base, path) + 1
    Overlay.flashMainMessage("Saved recording to %s (take %d)", base, nextTake)
    for i, sink in pairs(self.sinks) do
      local filename = string.format("%s-take%d-T%d.wav", base, nextTake, i)
      local destPath = Path.join(path, filename)
      app.moveFile(self.paths[i], destPath, false)
    end
  else
    Overlay.flashMainMessage("Saved recording to %s", base)
    for i, sink in pairs(self.sinks) do
      local filename = string.format("%s-T%d.wav", base, i)
      local destPath = Path.join(path, filename)
      app.moveFile(self.paths[i], destPath, false)
    end
  end
  for i, path in ipairs(self.paths) do
    Card.release(path)
  end
  self.sinks = {}
  self.state = "setup"
  self.subGraphic:clear()
  self.subGraphic:addChild(self.setupMenu)
  self:showCursor()
end

function FileRecorder:stopRecording(wait)
  if self.state == "paused" or self.state == "recording" then
    -- check that all pending buffers have been written
    app.AudioThread.removeTask(self.task)
    if wait then
      Busy.start("Flushing pending buffers to card...")
      self.state = "waiting"
      Timer.script(function(wait)
        while self.thread:getPendingBufferCount() > 0 do
          wait(1)
        end
      end)
      for i, sink in pairs(self.sinks) do
        if sink then
          sink:stop()
        end
      end
      Busy.stop()
    else
      for i, sink in pairs(self.sinks) do
        if sink then
          sink:abort()
        end
      end
    end
    self.thread:wait()
    self.thread:clear()
    self.thread:freeCache()
    self.task:clear()
    Busy.start("Closing files...")
    for i, sink in pairs(self.sinks) do
      if sink then
        Busy.status("Closing %s", self.paths[i])
        sink:close()
      end
    end
    Busy.stop()
    self.state = "stopped"
    self.subGraphic:clear()
    self.subGraphic:addChild(self.stoppedMenu)
  end
end

function FileRecorder:saveRecording()
  local sinkCount = 0
  local lastSink
  for i, sink in pairs(self.sinks) do
    if sink then
      sinkCount = sinkCount + 1
      lastSink = i
    end
  end
  if sinkCount == 1 then
    local Settings = require "Settings"
    if Settings.get("fileRecorderSingleTrackSaving") == "file" then
      self:saveSingleTrackRecording(lastSink)
    else
      self:saveMultiTrackRecording()
    end
  elseif sinkCount > 1 then
    self:saveMultiTrackRecording()
  end
end

function FileRecorder:saveSingleTrackRecording(i)
  local task = function(result)
    if result and result.fullpath then
      self:saveSingleTrackTo(i, result.fullpath)
    end
  end

  local chooser = FileChooser {
    msg = "Choose a filename.",
    path = self.root,
    goal = "save file",
    ext = ".wav",
    history = "saveSingleTrackRecording"
  }
  chooser:subscribe("done", task)
  chooser:show()
end

function FileRecorder:saveMultiTrackRecording()
  local task = function(result)
    if result and result.fullpath and result.foldername then
      self:saveMultipleTracksTo(result.foldername, result.fullpath)
    end
  end

  local chooser = FileChooser {
    msg = "Choose a folder.",
    path = self.root,
    goal = "save folder",
    suggested = suggestSaveFolder(self.root),
    history = "saveMultiTrackRecording"
  }
  chooser:subscribe("done", task)
  chooser:show()
end

function FileRecorder:discardRecording()
  for _, path in ipairs(self.paths) do
    Card.release(path)
  end
  self.sinks = {}
  self.state = "setup"
  self.subGraphic:clear()
  self.subGraphic:addChild(self.setupMenu)
  self:showCursor()
end

function FileRecorder:setInputSource(trackIndex, channelIndex, source)
  local config = self.configs[trackIndex]
  if config then
    if config.stereo then
      if channelIndex == 1 then
        config.source1 = source
      elseif channelIndex == 2 then
        config.source2 = source
      end
    elseif channelIndex == 1 then
      config.source = source
    end
  end
  local track = self.tracks[trackIndex]
  if track then
    track:setInputSource(channelIndex, source)
  end
end

function FileRecorder:buildTracks()
  self.tracks = {}
  self:clear()
  local channelCount = 0
  for i, config in ipairs(self.configs) do
    if config.stereo then
      channelCount = channelCount + 2
    else
      channelCount = channelCount + 1
    end
  end
  while channelCount < self.channelCount do
    self.configs[#self.configs + 1] = {
      stereo = false,
      source = nil
    }
    channelCount = channelCount + 1
  end
  while channelCount > self.channelCount do
    local config = self.configs[#self.configs]
    if config.stereo then
      channelCount = channelCount - 2
    else
      channelCount = channelCount - 1
    end
    self.configs[#self.configs] = nil
  end
  app.collectgarbage()
  for i, config in ipairs(self.configs) do
    local canLink = false
    if i > 1 then
      canLink = not self.configs[i - 1].stereo and not config.stereo
    end
    local track = TrackSection(i, config.stereo, canLink)
    self:appendSection(track)
    if config.stereo then
      track:setInputSource(1, config.source1)
      track:setInputSource(2, config.source2)
    else
      track:setInputSource(1, config.source)
    end
    track:switchView("setup")
    self.tracks[i] = track
  end
end

function FileRecorder:findSpotByAction(track, viewName, action)
  local view = track.views[viewName]
  for _, spot in pairs(view.spots) do
    if spot:getValue("action") == action then
      return spot:getHandle()
    end
  end
end

function FileRecorder:enterReleased()
  if self.state ~= "setup" then
    return true
  end
  local track, viewName, spotHandle = self:getSelection()
  local scroll = self:getScrollPosition()
  local view = track.views[viewName]
  if view then
    local spot = view:getSpotByHandle(spotHandle)
    if spot then
      local action = spot:getValue("action")
      if action == "link" and not track.stereo then
        -- link with previous track
        local index = track.index - 1
        local src1 = self.tracks[index]:getInputSource(1)
        local src2 = track:getInputSource(1)
        table.remove(self.configs, index)
        table.remove(self.configs, index)
        table.insert(self.configs, index, {
          stereo = true,
          source1 = src1,
          source2 = src2
        })
        self:buildTracks()
        track = self.tracks[index]
        spotHandle = self:findSpotByAction(track, viewName, "unlink")
        self:setSelection(track, viewName, spotHandle)
      elseif action == "unlink" and track.stereo then
        -- unlink stereo track
        local index = track.index
        local src1 = track:getInputSource(1)
        local src2 = track:getInputSource(2)
        table.remove(self.configs, index)
        table.insert(self.configs, index, {
          stereo = false,
          source = src1
        })
        table.insert(self.configs, index + 1, {
          stereo = false,
          source = src2
        })
        self:buildTracks()
        track = self.tracks[index + 1]
        spotHandle = self:findSpotByAction(track, viewName, "link")
        self:setSelection(track, viewName, spotHandle)
      elseif action == "assign" then
        local channel = spot:getControl()
        local chooser = SourceChooser()
        local task = function(src)
          self:setInputSource(channel.track.index, channel.index, src)
        end
        chooser:subscribe("choose", task)
        chooser:show()
      end
    end
  end
  self:setScrollPosition(scroll)
  return true
end

function FileRecorder:encoder(change, shifted)
  -- if self.state=="setup" then
  return SpottedStrip.encoder(self, change, shifted)
  -- else
  --  return true
  -- end
end

function FileRecorder:onCursorMove(section, view, spot, section0, view0, spot0)
  SpottedStrip.onCursorMove(self, section, view, spot, section0, view0, spot0)
  if self.state == "setup" then
    self:updateHints()
  end
end

function FileRecorder:zeroPressed()
  self:clearTracks()
  return true
end

function FileRecorder:homeReleased()
  if self.state ~= "setup" then
    return true
  end
  local track, viewName, spotHandle = self:getSelection()
  local view = track.views[viewName]
  if view then
    local spot = view:getSpotByHandle(spotHandle)
    if spot then
      local action = spot:getValue("action")
      if action == "assign" then
        local channel = spot:getControl()
        self:setInputSource(track.index, channel.index, nil)
      end
    end
  end
  return true
end

function FileRecorder:cancelReleased()
  self:hide()
  return true
end

function FileRecorder:upReleased(shifted)
  self:hide()
  return true
end

function FileRecorder:subReleased(i, shifted)
  if self.state == "setup" then
    if not Card.mounted() then
      Overlay.flashMainMessage("The SD card is not mounted.")
    elseif i == 1 then
      local Persist = require "Persist"
      Persist.loadRecorderPreset()
    elseif i == 2 then
      self:startRecording()
    elseif i == 3 then
      local Persist = require "Persist"
      Persist.saveRecorderPreset()
    end
  elseif self.state == "recording" then
    if i == 1 then
      self.recordingMenu:toggleDisplay()
    elseif i == 2 then
      self:pauseRecording()
    elseif i == 3 then
      self:stopRecording(true)
    end
  elseif self.state == "paused" then
    if i == 1 then
      self.recordingMenu:toggleDisplay()
    elseif i == 2 then
      self:unpauseRecording()
    elseif i == 3 then
      self:stopRecording(true)
    end
  elseif self.state == "stopped" then
    if i == 2 then
      self:discardRecording()
    elseif i == 3 then
      self:saveRecording()
    end
  end
  return true
end

function FileRecorder:serialize()
  local t = {}
  for i, config in ipairs(self.configs) do
    if config.stereo then
      t[i] = {
        stereo = true,
        source1 = config.source1 and config.source1:serialize(),
        source2 = config.source2 and config.source2:serialize()
      }
    else
      t[i] = {
        stereo = false,
        source = config.source and config.source:serialize()
      }
    end
  end
  return t
end

function FileRecorder:setInputWithSourceData(trackIndex, channelIndex, data)
  local Source = require "Source"
  local source = Source.deserialize(data)

  if source == nil then
    if type(data) == "table" then
      app.logInfo("%s:setInputWithSourceData:NOT found", self)
      local Utils = require "Utils"
      Utils.pp(data)
    else
      app.logInfo("%s:setInputWithSourceData(%s,%s,%s):NOT found", self,
                  trackIndex, channelIndex, data)
    end
  end

  -- Clears assigned source if nil.
  self:setInputSource(trackIndex, channelIndex, source)
end

function FileRecorder:deserialize(t)
  local Application = require "Application"
  -- only allow deserialization if in setup state
  if self.state ~= "setup" then
    return
  end
  self.configs = {}
  for i, cdata in ipairs(t) do
    if cdata.stereo then
      self.configs[i] = {
        stereo = true,
        source1 = nil,
        source2 = nil
      }
      if cdata.source1 then
        Application.post(function()
          self:setInputWithSourceData(i, 1, cdata.source1)
        end)
      end
      if cdata.source2 then
        Application.post(function()
          self:setInputWithSourceData(i, 2, cdata.source2)
        end)
      end
    else
      self.configs[i] = {
        stereo = false,
        source = nil
      }
      if cdata.source then
        Application.post(function()
          self:setInputWithSourceData(i, 1, cdata.source)
        end)
      end
    end
  end
  self:buildTracks()
end

return FileRecorder(FS.getRoot("recordings"))
