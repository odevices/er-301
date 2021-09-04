local app = app
local Env = require "Env"
local Timer = require "Timer"
local Signal = require "Signal"
local Class = require "Base.Class"
local Path = require "Path"
local Window = require "Base.Window"
local Verification = require "Verification"
local Message = require "Message"
local LogHistory = require "LogHistory"
local Card = require "Card"
local Pool = require "Sample.Pool"
local Creator = require "Sample.Pool.Creator"
local FileChooser = require "Card.FileChooser"
local Drawings = require "Drawings"
local SlicingView = require "SlicingView"
local threshold = Env.EncoderThreshold.Default

-- dividing boundary between list box and detail panel
local X = Env.DetailPanel.X

-- memory usage display
local memUnit = 1024 * 1024

-- SamplePool Interface
local Interface = Class {}
Interface:include(Window)

function Interface:init(history, purpose)
  Window.init(self)
  self:setClassName("SamplePool.Interface")
  -- Used to associate FileChooser history
  self.history = history .. "/Samples"
  self.purpose = purpose

  local label

  local listBox = app.ListBox(0, 0, X, 64)
  listBox:setFocus()
  listBox:setTextSize(10)
  listBox:hideScrollbar()
  if purpose == "save" then
    listBox:setEmptyText("No samples loaded.")
  else
    listBox:setEmptyText("Empty.  Please load or create samples.")
  end
  listBox:setJustification(app.justifyLeft)
  self.mainGraphic:addChild(listBox)
  self.sampleColumn = listBox

  listBox = app.ListBox(X - 25, 0, 25, 64)
  listBox:setFocus()
  listBox:setTextSize(10)
  listBox:setEmptyText("")
  listBox:setJustification(app.justifyRight)
  listBox:hideSelection()
  self.mainGraphic:addChild(listBox)
  self.statusColumn = listBox

  -- sample detail info labels
  local x = (X + 256) // 2
  self.info1Label = app.Label("", 10)
  self.info1Label:setCenter(x, app.GRID5_CENTER1)
  self.mainGraphic:addChild(self.info1Label)

  self.info2Label = app.Label("", 10)
  self.info2Label:setCenter(x, app.GRID5_CENTER2)
  self.mainGraphic:addChild(self.info2Label)

  self.info3Label = app.Label("", 10)
  self.info3Label:setCenter(x, app.GRID5_CENTER3)
  self.mainGraphic:addChild(self.info3Label)

  self.info4Label = app.Label("", 10)
  self.info4Label:setCenter(x, app.GRID5_CENTER4)
  self.mainGraphic:addChild(self.info4Label)

  -- sample detail buttons
  label = app.MainButton("unload", 6)
  self.mainGraphic:addChild(label)
  self.delButton = label
  self.delButton:hide()
  label = app.MainButton("save as", 5)
  self.mainGraphic:addChild(label)
  self.saveButton = label
  self.saveButton:hide()
  label = app.MainButton("replace", 5)
  self.mainGraphic:addChild(label)
  self.replaceButton = label
  self.replaceButton:hide()

  local drawing = app.Drawing(0, 0, 128, 64)
  drawing:add(Drawings.Sub.TitleLine)
  self.subGraphic:addChild(drawing)
  label = app.Label("Sample Pool", 10)
  label:setCenter(64, app.GRID5_CENTER1)
  self.subGraphic:addChild(label)

  -- setup memory usage display
  self.memoryLabel = app.Label("", 10)
  self.memoryLabel:setCenter(64, app.GRID4_CENTER2)
  self.subGraphic:addChild(self.memoryLabel)

  if purpose == "save" then
    label = app.RichTextBox(0, 0, 2 * app.SECTION_PLY, app.GRID5_LINE3)
    label:setJustification(app.justifyCenter)
    label:setAlignment(app.alignBottom)
    label:setText("Continue with Quicksave")
    self.subGraphic:addChild(label)
    label = app.TextPanel("Purge Unused", 3, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
  else
    label = app.TextPanel("Load Samples", 1, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    label = app.TextPanel("Create Buffer", 2, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
    label = app.TextPanel("Purge Unused", 3, 0.5, app.GRID5_LINE3)
    self.subGraphic:addChild(label)
  end

  -- load samples currently in the pool
  for _, sample in pairs(Pool.samples) do self:addSample(sample) end

  self.timerHandleForDisplay = nil

  -- some child widgets
  Signal.weakRegister("sampleStatusChanged", self)
  Signal.weakRegister("deletingSample", self)
  Signal.weakRegister("renamedSample", self)
  Signal.weakRegister("newSample", self)
  Signal.weakRegister("memoryUsageChanged", self)
  self:updateDetail()
end

function Interface:setDefaultChannelCount(channelCount)
  self.defaultChannelCount = channelCount
end

function Interface:highlight(sample)
  if sample then
    if self.sampleColumn:highlightWithData(sample.path) then
      self.sampleColumn:showHighlight()
      self.sampleColumn:chooseWithData(sample.path)
      self.statusColumn:chooseWithData(sample.path)
    else
      self.sampleColumn:hideHighlight()
    end
  else
    self.sampleColumn:hideHighlight()
  end
end

function Interface:getPreview()
  return self.subGraphic
end

-- also signal sent by pool --
function Interface:memoryUsageChanged()
  local free = app.BigHeap.remaining(memUnit)
  -- local used = totalHeapSize - free
  local largest = app.BigHeap.largest(memUnit)
  local text = string.format("Free: %3dMB (%3dMB)", free, largest)
  self.memoryLabel:setText(text)
end

function Interface:updateDetail()
  local path = self.sampleColumn:getSelectedData()
  local sample = Pool.samples[path]
  if sample == nil then
    self.info1Label:setText("")
    self.info2Label:setText("")
    self.info3Label:setText("")
    self.info4Label:setText("")
    self.saveButton:hide()
    self.replaceButton:hide()
    self.delButton:hide()
  else
    self.info1Label:setText(sample:getDurationText())
    local tmp = string.format("%s %s", sample:getChannelText(),
                              sample:getSampleRateText())
    self.info2Label:setText(tmp)
    self.info3Label:setText(sample:getMemorySizeText())
    if sample.userCount < 1 then
      self.info4Label:setText("Not in use.")
    elseif sample.userCount < 2 then
      self.info4Label:setText("Used by 1 unit.")
    else
      self.info4Label:setText(string.format("Used by %d units.",
                                            sample.userCount))
    end
    if sample:isDirty() or not sample:isFileBased() or app.isShiftButtonPushed() then
      self.saveButton:show()
      self.replaceButton:hide()
    else
      self.saveButton:hide()
      self.replaceButton:show()
    end
    self.delButton:show()
  end
end

-- signal sent by sample pool
function Interface:sampleStatusChanged(sample)
  local name = sample:getDecoratedName()
  self.sampleColumn:updateNameByData(name, sample.path)
  self.statusColumn:updateNameByData(sample:getStatusText(), sample.path)
end

function Interface:onShow()
  self:memoryUsageChanged()
  self:updateDetail()
  local task = function()
    self:memoryUsageChanged()
    self:updateDetail()
  end
  for _, sample in pairs(Pool.samples) do self:sampleStatusChanged(sample) end
  self.timerHandleForDisplay = Timer.every(0.5, task)
end

function Interface:onHide()
  Timer.cancel(self.timerHandleForDisplay)
end

function Interface:encoder(change, shifted)
  self.statusColumn:encoder(change, shifted, threshold)
  if self.sampleColumn:encoder(change, shifted, threshold) then
    self:updateDetail()
  end
  return true
end

function Interface:addSample(sample)
  local name = sample:getDecoratedName()
  self.sampleColumn:addItem(name, sample.path)
  self.sampleColumn:scrollToBottom()
  self.statusColumn:addItem(sample:getStatusText(), sample.path)
  self.statusColumn:scrollToBottom()
end

-- signal from pool --
function Interface:newSample(sample)
  self:addSample(sample)
  self:updateDetail()
end

function Interface:renamedSample(sample, oldPath)
  local name = sample:getDecoratedName()
  if not self.sampleColumn:updateByData(name, sample.path, oldPath) then
    app.logWarn("renamedSample(%s): could not find %s", sample, oldPath)
  end
  self.statusColumn:updateByData(sample:getStatusText(), sample.path, oldPath)
end

function Interface:deleteSample()
  local path = self.sampleColumn:getSelectedData()
  if not path then return end
  local sample = Pool.samples[path]
  if not sample then return end
  self.sampleColumn:scrollUp()
  self.statusColumn:scrollUp()
  self:updateDetail()
  Pool.unload(sample, true)
end

-- signal sent by pool --
function Interface:deletingSample(sample)
  if sample then
    self.sampleColumn:removeItemByData(sample.path)
    self.statusColumn:removeItemByData(sample.path)
  end
  self:updateDetail()
end

function Interface:clearSamples()
  Timer.script(function(wait)
    self.sampleColumn:scrollToBottom()
    self.statusColumn:scrollToBottom()
    while self.sampleColumn:size() > 0 do
      self:deleteSample()
      wait(0.001)
    end
    collectgarbage()
  end)
end

function Interface:tryPurgeSample()
  local path = self.sampleColumn:getSelectedData()
  if not path then return false end
  local sample = Pool.samples[path]
  if sample then
    if sample:inUse() then
      return false
    else
      app.logInfo("purge: %s", path)
    end
  else
    return false
  end
  self.sampleColumn:scrollUp()
  self.statusColumn:scrollUp()
  self:updateDetail()
  Pool.unload(sample, false)
  return true
end

function Interface:purgeUnused()
  Timer.script(function(wait)
    self.sampleColumn:scrollToBottom()
    self.statusColumn:scrollToBottom()
    while self.sampleColumn:size() > 0 do
      if not self:tryPurgeSample() then
        local index = self.sampleColumn:getSelectedIndex()
        app.logInfo("purge: index = %d", index)
        if index == 0 then
          app.logInfo("purge: index = 0")
          break
        end
        self.sampleColumn:scrollUp()
        self.statusColumn:scrollUp()
      end
      wait(0.001)
    end
    app.logInfo("purge: empty")
    collectgarbage()
  end)
end

local FakeUnit = Class {}

function FakeUnit:init()
  self.head = app.SliceHead()
end

function FakeUnit:setSample(sample)
  if self.sample then
    self.sample:release(self)
    self.sample = nil
  end
  self.sample = sample
  if self.sample then self.sample:claim(self) end

  if sample == nil then
    self.head:setSample(nil, nil)
  else
    self.head:setSample(sample.pSample, sample.slices.pSlices)
  end

  if self.slicingView then self.slicingView:setSample(sample) end
end

function FakeUnit:showSampleEditor()
  if self.sample then
    if self.slicingView == nil then
      self.slicingView = SlicingView(self, self.head)
      self.slicingView:setSample(self.sample)
      local function task()
        self:setSample(nil)
      end
      self.slicingView:subscribe("done", task)
    end
    self.slicingView:show()
  else
    local Overlay = require "Overlay"
    Overlay.flashMainMessage("You must first select a sample.")
  end
end

function Interface:enterReleased()
  if self.purpose == "save" then
    self:emitSignal("done", true)
  elseif self.purpose == "admin" then
    local path = self.sampleColumn:getSelectedData()
    local sample = Pool.samples[path]
    if sample then
      local fakeUnit = FakeUnit()
      fakeUnit:setSample(sample)
      fakeUnit:showSampleEditor()
    end
  else
    local path = self.sampleColumn:getSelectedData()
    local sample = Pool.samples[path]
    self:hide()
    self:emitSignal("done", sample)
  end
  return true
end

function Interface:cancelReleased(shifted)
  if not shifted then
    self:hide()
    if self.purpose=="save" then
      self:emitSignal("done", false)
    else
      self:emitSignal("done")
    end
  end
  return true
end

function Interface:upReleased(shifted)
  if not shifted then
    self:hide()
    if self.purpose=="save" then
      self:emitSignal("done", true)
    else
      self:emitSignal("done")
    end
  end
  return true
end

function Interface:homeReleased()
  self:hide()
  if self.purpose=="save" then
    self:emitSignal("done", true)
  else
    self:emitSignal("done")
  end
  return true
end

function Interface:saveSample()
  local path = self.sampleColumn:getSelectedData()
  local sample = Pool.samples[path]
  Pool.save(sample, true)
end

function Interface:shiftPressed()
  self:updateDetail()
  return true
end

function Interface:shiftReleased()
  self:updateDetail()
  return true
end

function Interface:mainReleased(i, shifted)
  if not self.delButton:isHidden() and i == 6 then
    -- delete
    local task = function(yes)
      if yes then
        self:deleteSample()
        Pool.defrag()
      end
    end
    local path = self.sampleColumn:getSelectedData()
    local sample = Pool.samples[path]
    local dialog = Verification.Main(string.format("Unload %s?", sample.name))
    dialog:subscribe("done", task)
    dialog:show()
  elseif i == 5 then
    if not self.saveButton:isHidden() then
      self:saveSample()
    elseif not self.replaceButton:isHidden() then
      self:doReplaceSample()
    end
  end
  return true
end

function Interface:doneChoosing(selectedPaths)
  local FS = require "Card.FileSystem"
  local Busy = require "Busy"
  Busy.start()
  selectedPaths = selectedPaths or {}
  local failed = {}
  local outOfMemory = false
  for _, fullPath in ipairs(selectedPaths) do
    app.logInfo("SamplePool: loading %s", fullPath)
    local filename = Path.getFilename(fullPath)
    local ext = Path.getExtension(filename)
    if FS.isValidExtension("audio", ext) then
      local row = self.sampleColumn:findByData(fullPath)
      if row < self.sampleColumn:size() then
        -- already loaded, select it
        self.sampleColumn:chooseByIndex(row)
        self.statusColumn:chooseByIndex(row)
        self:updateDetail()
      else
        local success, status = Pool.load(fullPath)
        if not success then
          failed[#failed + 1] = filename
          if status == app.STATUS_OUT_OF_MEMORY then outOfMemory = true end
          if #failed > 100 then break end
        end
      end
    else
      app.logWarn("SamplePool: unsupported file type (%s) ignored.", filename)
    end
  end
  Busy.stop()
  local dialog
  if #failed == 1 then
    if outOfMemory then
      dialog = Message.Main("Failed to load: " .. failed[1],
                            "Insufficient memory.")
    else
      LogHistory:showErrors("Failed to load: " .. failed[1])
    end
  elseif #failed > 1 then
    local msg1
    local msg2
    if #failed > 100 then
      msg1 = "Failed to load: 100+ files"
    else
      msg1 = string.format("Failed to load: %d files", #failed)
    end
    if outOfMemory then
      msg2 = "Insufficient memory."
      dialog = Message.Main(msg1, msg2)
    else
      LogHistory:showErrors(msg1)
    end
  end
  if dialog then
    dialog:show()
    return false
  end
  return true
end

function Interface:doPurgeUnused()
  local Settings = require "Settings"
  local dirty = Pool.hasDirtySamples()
  if dirty or Settings.get("confirmPurgeUnusedSamples") == "yes" then
    local task = function(yes)
      if yes then self:purgeUnused() end
    end
    local dialog
    if dirty then
      dialog = Verification.Main("Purge unused samples?",
                                 "Warning: There are unsaved buffers.")
    else
      dialog = Verification.Main("Purge unused samples?")
    end
    dialog:subscribe("done", task)
    dialog:show()
  else
    self:purgeUnused()
  end
end

function Interface:askAboutMultiSample(paths)
  local msg = string.format("Load these %d samples", #paths)
  local dialog = Verification.Main(msg, "as a sample chain?")
  local task = function(choice)
    if choice == true then
      Pool.loadMulti(paths)
    elseif choice == false then
      self:doneChoosing(paths)
    else
      local Overlay = require "Overlay"
      Overlay.flashMainMessage("Sample load was canceled.")
    end
  end
  dialog:subscribe("done", task)
  dialog:show()
end

function Interface:doReplaceSample()
  if Card.mounted() then
    local path = self.sampleColumn:getSelectedData()
    if not path then return end
    local sample = Pool.samples[path]
    if not sample then return end
    local task = function(result)
      if result and result.fullpath then
        local SG = require "Overlay"
        local oldName = sample.name
        local new = Pool.replace(sample, result.fullpath)
        if new then
          SG.flashMainMessage("%s --> %s", oldName, new.name)
        else
          SG.flashMainMessage("Replace failed.")
        end
      end
    end
    local FS = require "Card.FileSystem"
    local chooser = FileChooser {
      msg = "Choose Sample",
      goal = "load file",
      path = app.roots.front,
      pattern = FS.getPattern("audio"),
      history = self.history
    }
    chooser:subscribe("done", task)
    chooser:show()
  else
    local dialog = Message.Main("SD card is not mounted.")
    dialog:show()
  end
end

function Interface:doLoadSample()
  local Loader = require "Sample.Pool.Loader"
  local task = function(paths)
    if #paths > 1 then
      self:askAboutMultiSample(paths)
    elseif #paths == 1 then
      self:doneChoosing(paths)
    end
  end
  Loader(self.history, task)
end

function Interface:subReleased(i, shifted)
  if shifted then return true end
  if self.purpose == "save" then
    if i == 1 then
      self:hide()
      self:emitSignal("done", true)
    elseif i == 2 then
    elseif i == 3 then
      self:doPurgeUnused()
    end
  else
    if i == 1 then
      self:doLoadSample()
    elseif i == 2 then
      Creator(self.defaultChannelCount):show()
    elseif i == 3 then
      self:doPurgeUnused()
    end
  end
  return true
end

return Interface
