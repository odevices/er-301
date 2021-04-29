local app = app
local Utils = require "Utils"
local Signal = require "Signal"
local Timer = require "Timer"
local Sample = require "Sample"
local Message = require "Message"
local Card = require "Card"
local Path = require "Path"
local FileChooser = require "Card.FileChooser"

local samples = {}
local saver = app.SampleSaver()
local loader = app.SampleLoader()
local jobQ = app.JobQueue("spjobs")
jobQ:start()

local ErrorReasons = {
  [app.STATUS_ERROR_OPENING_FILE] = "Error opening file.",
  [app.STATUS_OUT_OF_MEMORY] = "Insufficient memory.",
  [app.STATUS_ERROR_READING_FILE] = "Error reading file.",
  [app.STATUS_ERROR_WRITING_FILE] = "Error writing file.",
  [app.STATUS_SAMPLE_NOT_PREPARED] = "Sample not prepared.",
  [app.STATUS_FILE_NOT_EXIST] = "File does not exist."
}

local function getErrorString(status)
  return ErrorReasons[status] or "Failed for unknown reason."
end

local lastBufferId = 0
local function generateUniqueName(root)
  local id = lastBufferId + 1
  lastBufferId = id
  local Persist = require "Persist"
  local session = Persist.meta.boot.count
  return string.format("tmp/%s-%04d-%02d", root, session, id)
end

local function generateUniqueNameFromPaths(paths)
  local id = lastBufferId + 1
  lastBufferId = id
  local Persist = require "Persist"
  local session = Persist.meta.boot.count
  local longestPath = app.LongestPath()
  for _, path in ipairs(paths) do longestPath:add(path) end
  local longest = longestPath:calculate()
  -- remove drive root
  longest = longest:gsub(app.roots.front, "")
  longest = longest:gsub(app.roots.rear, "")
  longest = longest:gsub(app.roots.x, "")
  return string.format("%s-%04d-%02d", longest, session, id),
         string.format("%02d-%s:%d", id, longest, #paths)
end

-- sample loading and saving queues
local loadQ = {}
local saveQ = {}
local timerHandleForQ = nil
local loadingSample = nil
local savingSample = nil

local function updateQueue()
  if savingSample ~= nil then
    -- update the display
    if saver.mStatus == app.STATUS_WORKING then
      -- still saving so just update progress display
      savingSample:setProgress(saver.mPercentDone)
      Signal.emit("sampleStatusChanged", savingSample)
    elseif saver.mStatus == app.STATUS_FINISHED then
      savingSample:setGood()
      savingSample:setClean()
      Signal.emit("sampleStatusChanged", savingSample)
      savingSample = nil
    elseif saver.mStatus == app.STATUS_CANCELED then
      savingSample:setError("Save was canceled.")
      savingSample = nil
    elseif saver.mStatus < 0 then
      -- error happened
      local reason = getErrorString(saver.mStatus)
      app.logError("Sample(%s):error saving:%s.", savingSample.name, reason)
      savingSample:setError(reason)
      Signal.emit("sampleStatusChanged", savingSample)
      savingSample = nil
    else
      app.logError("Sample(%s):unknown status:%d", savingSample.name,
                  saver.mStatus)
      savingSample = nil
    end
  end
  if loadingSample ~= nil then
    -- update the display
    if loader.mStatus == app.STATUS_WORKING then
      -- still loading so just update progress display
      loadingSample:setProgress(loader.mPercentDone)
      Signal.emit("sampleStatusChanged", loadingSample)
      if samples[loadingSample.path] == nil then jobQ:cancel(loader) end
    elseif loader.mStatus == app.STATUS_FINISHED then
      loadingSample:setGood()
      Signal.emit("sampleStatusChanged", loadingSample)
      loadingSample = nil
    elseif loader.mStatus == app.STATUS_CANCELED then
      loadingSample:setError("Load was canceled.")
      loadingSample = nil
    elseif loader.mStatus < 0 then
      -- error happened
      local reason = getErrorString(saver.mStatus)
      app.logError("Sample(%s):error loading:%s", loadingSample.name, reason)
      loadingSample:setError(reason)
      Signal.emit("sampleStatusChanged", loadingSample)
      loadingSample = nil
    else
      app.logError("Sample(%s):unknown status:%d", loadingSample.name,
                  saver.mStatus)
      loadingSample = nil
    end
  end
  if jobQ:isIdle() and Card.mounted() then
    if #saveQ > 0 then
      -- jobQ is idle, pop the next item from the queue
      savingSample = saveQ[#saveQ]
      saveQ[#saveQ] = nil
      if saver:set(savingSample.pSample, savingSample.path) then
        savingSample:setWorking()
        jobQ:push(saver)
        Signal.emit("sampleStatusChanged", savingSample)
      else
        app.logError("Sample(%s): unable to save to %s.", savingSample.name,
                    savingSample.path)
        savingSample:setError("Unable to start saving.")
        Signal.emit("sampleStatusChanged", savingSample)
        savingSample = nil
      end
    elseif #loadQ > 0 then
      -- jobQ is idle, pop the next item from the queue
      loadingSample = loadQ[#loadQ]
      loadQ[#loadQ] = nil
      if loader:set(loadingSample.pSample, loadingSample.loadInfo) then
        loadingSample:setWorking()
        jobQ:push(loader)
        Signal.emit("sampleStatusChanged", loadingSample)
      else
        loadingSample:setError("Unable to start loading.")
        Signal.emit("sampleStatusChanged", loadingSample)
        loadingSample = nil
      end
    end
  end
  -- turn off timer if there is nothing left to do
  if #loadQ == 0 and #saveQ == 0 and jobQ:isIdle() then
    timerHandleForQ = nil
    return false
  else
    return true
  end
end

local function addToSaveQueue(sample)
  sample:prepareForSaving()
  if not sample:isDirty() then return true end
  if sample:length() > 0 then
    saveQ[#saveQ + 1] = sample
    if timerHandleForQ == nil then
      timerHandleForQ = Timer.every(0.3, updateQueue)
    end
    return true
  else
    app.logError("Sample(%s):failed to add to save queue: %s", sample.name,
                sample.path)
    return false
  end
end

local function addToLoadQueue(sample)
  -- prepare the sample for loading
  local status = sample:prepareForLoading()
  if status == app.STATUS_PREPARED then
    loadQ[#loadQ + 1] = sample
    if timerHandleForQ == nil then
      timerHandleForQ = Timer.every(0.3, updateQueue)
    end
    return true, status
  else
    local reason = getErrorString(status)
    sample:setError(reason)
    app.logError("Sample(%s): failed to prepare %s (%s)", sample.name,
                sample.path, reason)
    local msg = Message.Main("Failed to load " .. Path.getFilename(sample.path),
                             reason)
    msg:show()
    return false, status
  end
end

local function removeFromQueues(sample)
  Utils.removeValueFromArray(loadQ, sample)
  Utils.removeValueFromArray(saveQ, sample)
end

local function unload(sample, detach)
  -- give others a chance to prepare
  Signal.emit("deletingSample", sample)
  if detach then
    local users = sample:getUsers()
    for _, user in ipairs(users) do
      if user.setSample then user:setSample(nil) end
    end
  end
  samples[sample.path] = nil
  sample = nil
  -- all references to the sample should gone by now
  app.collectgarbage()
  Signal.emit("memoryUsageChanged")
end

local function load(path, slicesInfo)
  path = Path.checkRelativePath(path)
  local sample = samples[path]
  if sample then return sample end
  sample = Sample(path, {
    type = "single"
  })
  samples[path] = sample
  local success, status = addToLoadQueue(sample)
  if success then
    if slicesInfo then
      sample.slices:deserialize(slicesInfo)
    else
      if not sample.slices:load(sample:defaultSlicesPath()) then
        sample.slices:loadWavFileCues(path);
      end
    end
    Signal.emit("memoryUsageChanged")
    sample:setEnqueuedToLoad()
    Signal.emit("newSample", sample)
    return sample
  else
    app.logError("SamplePool.load:Failed to enqueue: %s", path)
    samples[path] = nil
    return false, status
  end
end

local function loadMulti(paths, id, slicesInfo)
  if id then
    local sample = samples[id]
    if sample then return sample end
  end
  local opts = {
    type = "multi",
    paths = Utils.shallowCopy(paths)
  }
  local sample
  if id == nil then
    local uid, pretty = generateUniqueNameFromPaths(paths)
    id = uid
    sample = Sample(id, opts, pretty)
  else
    sample = Sample(id, opts)
  end
  samples[id] = sample
  local success, status = addToLoadQueue(sample)
  if success then
    if slicesInfo then sample.slices:deserialize(slicesInfo) end
    Signal.emit("memoryUsageChanged")
    sample:setEnqueuedToLoad()
    Signal.emit("newSample", sample)
    return sample
  else
    app.logError("SamplePool.load:Failed to enqueue: %s", id)
    samples[id] = nil
    return false, status
  end
end

local function revert(sample)
  if sample.revertPath then
    return load(sample.revertPath)
  else
    return false, "Nothing to revert."
  end
end

local function clone(sample, Ns)
  local newSample = sample:clone(Ns)
  if not newSample then
    app.logError("SamplePool.clone:Failed to clone %s with %d samples.",
                sample.path, Ns)
    return false, "Insufficient memory."
  end
  if sample:isFileBased() then
    local path, filename = Path.split(sample.revertPath or sample.path)
    local ext = Path.getExtension(filename)
    local stem = Path.removeExtension(filename)
    local i = 0
    local path2
    repeat
      i = i + 1
      local newFilename = string.format("%s(%d).%s", stem, i, ext)
      path2 = Path.join(path, newFilename)
    until samples[path2] == nil and not Path.exists(path2)
    app.logInfo("SamplePool.clone: new path = %s", path2)
    newSample:setPath(path2)
  else
    app.logInfo("Cloning %s: path=%s not file based", sample, sample.path)
    local path = generateUniqueName("buffer")
    newSample:setPath(path)
  end
  samples[newSample.path] = newSample
  Signal.emit("newSample", newSample)
  return newSample
end

-- A buffer is temporary sample at the system sampling rate.
local function create(opts, path, slicesInfo)
  local channelCount = opts.channels or 1
  local totalSeconds = opts.secs
  local sampleCount = opts.samples
  local root = opts.root or "buffer"

  if path then
    local sample = samples[path]
    if sample then return sample end
  end
  path = path or generateUniqueName(root)

  if sampleCount then
    -- length defined in samples
    local sample = Sample(path, {
      type = "buffer",
      channels = channelCount,
      samples = sampleCount
    })
    if sample:allocateBySamples(channelCount, sampleCount) then
      if slicesInfo then sample.slices:deserialize(slicesInfo) end
      sample:setGood()
      samples[path] = sample
      Signal.emit("newSample", sample)
      return sample
    else
      sample:setError("Insufficient memory.")
      return false, "Insufficient memory."
    end
  elseif totalSeconds then
    -- length defined in seconds
    local sample = Sample(path, {
      type = "buffer",
      channels = channelCount,
      secs = totalSeconds
    })
    if sample:allocateBySeconds(channelCount, totalSeconds) then
      if slicesInfo then sample.slices:deserialize(slicesInfo) end
      sample:setGood()
      samples[path] = sample
      Signal.emit("newSample", sample)
      return sample
    else
      sample:setError("Insufficient memory.")
      return false, "Insufficient memory."
    end
  end
end

local function rename(sample, newPath)
  local oldPath = sample.path
  samples[oldPath] = nil
  samples[newPath] = sample
  sample:setPath(newPath)
  sample:setDirty()
  -- let others know
  Signal.emit("renamedSample", sample, oldPath)
end

local function replace(sample, newPath)
  local users = sample:getUsers()
  for _, user in ipairs(users) do
    if user.setSample then user:setSample(nil) end
  end
  unload(sample, false)
  sample = nil
  app.collectgarbage()
  sample = load(newPath)
  if sample then
    for _, user in ipairs(users) do
      if user.setSample then user:setSample(sample) end
    end
    return sample
  end
end

local function saveMulti(sample, forceDialog)
  if not Card.mounted() then
    local msg = Message.Main("Card not mounted.")
    msg:show()
    return false, "Card not mounted."
  end

  local FS = require "Card.FileSystem"
  local path, filename = Path.split(sample.path)
  local pathValid = Path.exists(path)
  local ext = FS.getExt("multi")
  local filenameValid = Utils.endsWith(filename, ext)
  if not (pathValid and filenameValid) then forceDialog = true end
  if forceDialog then
    -- try getting a path from the user
    local task = function(result)
      if result and result.fullpath then
        sample.pretty = nil
        rename(sample, result.fullpath)
        saveMulti(sample)
      end
    end
    local chooser = FileChooser {
      msg = "Save Sample Chain",
      goal = "save file",
      path = (pathValid and path) or app.roots.front,
      ext = "." .. ext,
      suggested = (filenameValid and filename) or sample:getSuggestedFilename(),
      history = "Admin/Samples",
      pattern = FS.getPattern("multi", false)
    }
    chooser:subscribe("done", task)
    chooser:show()
    return false
  end
  sample:prepareForSaving()
  if sample:writePaths() then
    sample:setGood()
    sample:setClean()
    Signal.emit("sampleStatusChanged", sample)
    return true
  else
    sample:setError("Failed to save.")
    Signal.emit("sampleStatusChanged", sample)
    return false
  end
end

local function save(sample, forceDialog)
  if sample:isMulti() then return saveMulti(sample, forceDialog) end

  if not Card.mounted() then
    local msg = Message.Main("Card not mounted.")
    msg:show()
    return false, "Card not mounted."
  end

  local path, filename = Path.split(sample.path)
  local pathValid = Path.exists(path)
  local filenameValid = Utils.endsWith(filename, ".wav") or
                            Utils.endsWith(filename, ".WAV")
  if not (pathValid and filenameValid) then forceDialog = true end
  if forceDialog then
    -- try getting a path from the user
    local task = function(result)
      if result and result.fullpath then
        rename(sample, result.fullpath)
        save(sample)
      end
    end
    local FileRecorder = require "FileRecorder"
    local FS = require "Card.FileSystem"
    local chooser = FileChooser {
      msg = "Save Buffer",
      goal = "save file",
      path = (pathValid and path) or FileRecorder.root,
      ext = ".wav",
      suggested = (filenameValid and filename) or sample:getSuggestedFilename(),
      history = "SamplePool.save",
      pattern = FS.getPattern("audio-strict", false)
    }
    chooser:subscribe("done", task)
    chooser:show()
    return false
  end
  if addToSaveQueue(sample) then
    sample:setEnqueuedToSave()
    Signal.emit("sampleStatusChanged", sample)
    return true
  else
    sample:setError("Failed to enqueue.")
    Signal.emit("sampleStatusChanged", sample)
    return false
  end
end

local function cancel(sample)
  if sample:isPending() then
    if loadingSample == sample then
      jobQ:cancel(loader)
    elseif savingSample == sample then
      jobQ:cancel(saver)
    else
      removeFromQueues(sample)
    end
    sample:setError("Action was canceled.")
    Signal.emit("sampleStatusChanged", sample)
  end
end

local function defrag()
  -- TODO
  collectgarbage()
  collectgarbage()
  collectgarbage()
  app.BigHeap.print()
  Signal.emit("memoryUsageChanged")
end

local function serializeSample(sample)
  local t = {}
  t.path = sample.path
  t.opts = Utils.deepCopy(sample.opts)
  t.slices = sample.slices:serialize()
  return t
end

local function deserializeSample(t)
  -- Compatibility with old presets
  if t.opts == nil then
    t.opts = {}
    if t.path then t.opts.type = "single" end
  end
  if t.opts.type == nil or t.opts.type == "file" then t.opts.type = "single" end
  --
  if t.opts.type == "single" then
    return load(t.path, t.slices)
  elseif t.opts.type == "multi" then
    return loadMulti(t.opts.paths, t.path, t.slices)
  elseif t.opts.type == "buffer" then
    local sample, status = create(t.opts, t.path, t.slices)
    if sample then
      return sample
    else
      app.logError("SamplePool.deserializeSample:failed to create buffer (%s).",
                  status)
      return
    end
  else
    app.logError("SamplePool.deserializeSample: unsupported opts")
    return
  end
end

local function serialize()
  local t = {}
  for _, sample in pairs(samples) do t[#t + 1] = serializeSample(sample) end
  return t
end

local function deserialize(t)
  -- clear out pool and defrag for efficient memory usage
  for _, sample in pairs(samples) do
    local found = false
    for _, data in ipairs(t) do
      local path = data.path or data[2]
      if path == sample.path then
        found = true
        break
      end
    end
    if not found then unload(sample, true) end
  end
  defrag()
  for _, data in ipairs(t) do
    if data.path then
      deserializeSample(data)
    elseif data[2] then
      -- support legacy pool format
      load(data[2])
    end
  end
end

local function chooseFileFromCard(history, after, loop)
  local Loader = require "Sample.Pool.Loader"
  local task = function(paths)
    local sample
    local success, status
    if #paths > 1 then
      success, status = loadMulti(paths)
    elseif #paths == 1 then
      success, status = load(paths[1])
    end
    if success then
      sample = success
    else
      sample = nil
      local Overlay = require "Overlay"
      if status == app.STATUS_OUT_OF_MEMORY then
        Overlay.mainFlashMessage("Failed. Not enough sample memory.")
      elseif status == app.STATUS_ERROR_OPENING_FILE then
        Overlay.mainFlashMessage("Failed. Could not open file.")
      end
    end
    if after then after(sample) end
  end
  history = history or "Admin"
  Loader(history .. "/Samples", task, loop)
end

local function hasDirtySamples()
  for _, sample in pairs(samples) do if sample:isDirty() then return true end end
  return false
end

return {
  samples = samples,
  load = load,
  chooseFileFromCard = chooseFileFromCard,
  create = create,
  loadMulti = loadMulti,
  save = save,
  clone = clone,
  replace = replace,
  revert = revert,
  cancel = cancel,
  unload = unload,
  defrag = defrag,
  rename = rename,
  serialize = serialize,
  deserialize = deserialize,
  serializeSample = serializeSample,
  deserializeSample = deserializeSample,
  hasDirtySamples = hasDirtySamples
}
