local app = app
local Class = require "Base.Class"
local Object = require "Base.Object"
local Utils = require "Utils"
local Slices = require "Sample.Slices"
local Card = require "Card"
local Path = require "Path"
local FS = require "Card.FileSystem"

local NotSet = 0
local Enqueued = 1
local Working = 2
local Good = 3
local Error = 4

local Sample = Class {}
Sample:include(Object)

local __weak__ = {
  __mode = "k"
}

function Sample:init(fullPath, opts, pretty)
  self:setClassName("Sample")
  self.opts = opts or {
    type = "single"
  }
  self.pSample = app.Sample()
  self.state = NotSet
  self.reason = nil
  self.progress = nil
  self.description = nil
  self.users = setmetatable({}, __weak__)
  self.userCount = 0
  self.pretty = pretty
  self:setPath(fullPath)
  self.slices = Slices()
end

local function populateLoadInfo(info, paths)
  local good = {}
  info:reserve(#paths)
  for _, path in ipairs(paths) do
    if info:add(path, false) then good[#good + 1] = path end
  end
  return good
end

function Sample:prepareForLoading()
  if self:isBuffer() then return end
  local info = app.SampleLoadInfo()
  if self:isMulti() then
    local good = populateLoadInfo(info, self.opts.paths)
    self.opts.paths = good
    if #good == 0 then return app.STATUS_ERROR_OPENING_FILE end
  elseif FS.isType("multi", self.path) then
    local paths = self:readPaths()
    if paths == nil then return app.STATUS_ERROR_OPENING_FILE end
    local good = populateLoadInfo(info, paths)
    if #good == 0 then return app.STATUS_ERROR_OPENING_FILE end
  elseif Path.exists(self.path) then
    info:reserve(1)
    if not info:add(self.path, false) then
      return app.STATUS_ERROR_OPENING_FILE
    end
  else
    return app.STATUS_FILE_NOT_EXIST
  end

  local status = self.pSample:prepareForLoading(info)
  if status == app.STATUS_PREPARED then
    self.loadInfo = info
    if info:getCount() > 0 and self.slices:getCount() == 0 then
      self.slices:initFromLoadInfo(info)
    end
  end
  return status
end

function Sample:prepareForSaving()
  if self:isFileBased() then self.slices:save() end
end

function Sample:claim(user)
  local x = self.users[user] or 0
  self.users[user] = x + 1
  if x == 0 then self.userCount = self.userCount + 1 end
end

function Sample:release(user)
  local x = self.users[user] or 0
  if x > 0 then
    self.users[user] = x - 1
    if x == 1 then
      self.userCount = self.userCount - 1
      self.users[user] = nil
    end
  end
end

function Sample:getUsers()
  local users = {}
  for user, count in pairs(self.users) do
    if count > 0 then users[#users + 1] = user end
  end
  return users
end

function Sample:inUse()
  return self.userCount > 0
end

function Sample:isShared()
  return self.userCount > 1
end

function Sample:setPath(fullPath)
  if self.state == Enqueued or self.state == Working then
    app.logError("%s(%s): trying to setPath() when enqueued or working.", self,
                 self.path)
  end
  self.path = fullPath
  if self.slices then self.slices.path = self:defaultSlicesPath() end
  if self:isFileBased() then self.opts.type = "single" end
  self.name = self:getLastFolderAndFilenameForDisplay(22)
end

function Sample:setRate(rate)
  self.pSample:setSampleRate(rate)
end

function Sample:getFilenameForDisplay(maxLength)
  if self.pretty then return Utils.shorten(self.pretty, maxLength, "..", true) end
  local path, filename = Path.split(self.path)
  local token = app.roots.front .. "/"
  filename = filename or path
  if Utils.startsWith(filename, token) then
    filename = string.sub(filename, #token + 1)
  end
  return Utils.shorten(filename or self.path, maxLength, "..", true)
end

function Sample:getLastFolderAndFilenameForDisplay(maxLength)
  if self.pretty then return Utils.shorten(self.pretty, maxLength, "..", true) end

  local path = self.path
  local folder, filename = Path.split(path)
  if folder and folder ~= app.roots.front and folder ~= app.roots.front then
    local parentFolder, lastFolder = Path.split(folder)
    if lastFolder then
      local tmp = Path.join(lastFolder, filename)
      return Utils.shorten(tmp, maxLength, "..", true)
    else
      return Utils.shorten(filename or path, maxLength, "..", true)
    end
  end
  return path
end

function Sample:getFullPathForDisplay(maxLength)
  if self.pretty then return Utils.shorten(self.pretty, maxLength, "..") end
  local path, filename = Path.split(self.path)
  local token = app.roots.front .. "/"
  if path and Utils.startsWith(path, token) then
    path = string.sub(path, #token + 1)
  end
  if filename and path then
    local M = maxLength // 2
    local shortPath = Utils.shortenPath(path, M)
    M = maxLength - #shortPath
    local shortFilename = Utils.shorten(filename, M, "..")
    return shortPath .. "/" .. shortFilename
  elseif filename then
    return Utils.shorten(filename, maxLength, "..")
  elseif path then
    return Utils.shorten(path, maxLength, "..")
  else
    return self.path
  end
end

function Sample:length()
  return self.pSample.mSampleCount
end

function Sample:isDirty()
  return self.pSample.mDirty
end

function Sample:setDirty()
  self.pSample:setDirty()
end

function Sample:setClean()
  self.pSample:setClean()
end

function Sample:isFileBased()
  return Utils.startsWith(self.path, app.roots.front) or
             Utils.startsWith(self.path, app.roots.rear)
end

function Sample:isBuffer()
  return self.opts.type == "buffer"
end

function Sample:isMulti()
  return self.opts.type == "multi"
end

function Sample:hasChildren()
  return self:isMulti() or self.paths ~= nil
end

function Sample:hasNonTrivialSlices()
  return not self.slices:isTrivial(self.pSample.mSampleCount)
end

local function quote(text)
  return string.format('"%s"', text)
end

function Sample:writePaths()
  local f = io.open(self.path, "w")
  if f then
    f:write("return {\n")
    for _, path in ipairs(self.opts.paths) do f:write("  ", quote(path), ",\n") end
    f:write("}\n")
    f:close()
    self.opts.paths = nil
    return true
  end
  return false
end

function Sample:readPaths()
  local Persist = require "Persist"
  local paths = Persist.readTable(self.path)
  if paths then self.paths = paths end
  return paths
end

function Sample:defaultSlicesPath()
  return Path.replaceExtension(self.path, FS.getExt("slices"))
end

function Sample:clone(Ns)
  local opts = {}
  local Nc = self:getChannelCount()
  local rate = self:rate()
  if self:isBuffer() then
    opts.type = "buffer"
    opts.channels = Nc
  else
    opts.type = "single"
  end
  local sample = Sample("unknown", opts)
  if sample:allocateBySamples(Nc, Ns, rate) then
    sample:setGood()
    -- first time this is cloned, save the path for reverting later (if needed)
    sample.revertPath = self.revertPath or self.path
    return sample
  end
end

function Sample:allocateBySamples(channels, Ns, rate)
  if self.pSample:allocateBuffer(channels, Ns) then
    rate = rate or app.Audio_getRate()
    self.pSample:setSampleRate(rate)
    self.pSample:setMemoryOnly()
    return true
  else
    return false
  end
end

function Sample:allocateBySeconds(channels, secs, rate)
  rate = rate or app.Audio_getRate()
  local Ns = math.tointeger(math.floor(rate * secs));
  if self.pSample:allocateBuffer(channels, Ns) then
    self.pSample:setSampleRate(rate)
    self.pSample:setMemoryOnly()
    return true
  else
    return false
  end
end

function Sample:setProgress(percentDone)
  self.progress = string.format("%3.0f%%", percentDone)
end

function Sample:setEnqueuedToLoad()
  self.state = Enqueued
  self.description = nil
  self.reason = nil
  Card.claim("Loading from", self.path)
end

function Sample:setEnqueuedToSave()
  self.state = Enqueued
  self.description = nil
  self.reason = nil
  Card.claim("Saving to", self.path)
end

function Sample:setWorking()
  self.state = Working
  self:setProgress(0)
  self.description = nil
  self.reason = nil
end

function Sample:setGood()
  self.state = Good
  self.description = nil
  self.reason = nil
  Card.release(self.path)
  if self.pSample then self.slices:setSampleRate(self.pSample.mSampleRate) end
end

function Sample:setError(reason)
  self.state = Error
  self.reason = reason
  self.description = nil
  Card.release(self.path)
end

function Sample:isPending()
  return self.state == Enqueued or self.state == Working
end

function Sample:getStatusText()
  local state = self.state
  if state == Enqueued then
    return "P"
  elseif state == Working then
    return self.progress
  elseif state == Good then
    return self:getMemorySizeText()
  elseif state == Error then
    return "!"
  end
  return " "
end

function Sample:getMemorySizeText()
  if self.memorySizeText == nil then
    local b = self.pSample:getSizeInBytes()
    local kb = b // 1024
    if kb > 1023 then
      if kb < 10 * 1024 then
        self.memorySizeText = string.format("%0.1fMB", kb // 1024)
      else
        self.memorySizeText = string.format("%0.0fMB", kb // 1024)
      end
    else
      self.memorySizeText = string.format("%dKB", kb)
    end
  end
  return self.memorySizeText
end

function Sample:getDurationText()
  if self.durationText == nil then
    local total = self.pSample.mTotalSeconds
    self.durationText = Utils.duration(total)
  end
  return self.durationText
end

local channelInfo = {
  [1] = "1ch",
  [2] = "2ch",
  [3] = "3ch",
  [4] = "4ch"
}

function Sample:getChannelText()
  return channelInfo[self.pSample.mChannelCount] or "?ch"
end

function Sample:getChannelCount()
  if self.pSample then
    return self.pSample.mChannelCount
  else
    return 0
  end
end

function Sample:getTotalSeconds()
  if self.pSample then
    return self.pSample.mTotalSeconds
  else
    return 0
  end
end

function Sample:rate()
  return self.pSample.mSampleRate
end

function Sample:getSampleRateText()
  if self.sampleRateText == nil then
    self.sampleRateText = string.format("%2.1fkHz",
                                        self.pSample.mSampleRate / 1000)
  end
  return self.sampleRateText
end

function Sample:getDescriptionText()
  if self.state == Good then
    if self.description then
      return self.description
    else
      local part1, part2, part3
      part1 = channelInfo[self.pSample.mChannelCount] or "?ch"
      part2 = string.format("%dbit", self.pSample.mOriginalBitDepth)
      part3 = string.format("%2.1fkHz", self.pSample.mSampleRate / 1000)
      self.description = string.format("%s %s %s", part1, part2, part3)
      return self.description
    end
  else
    return "Not loaded."
  end
end

function Sample:getSuggestedFilename()
  if self.opts and self.opts.type == "buffer" then
    return "buf" .. Path.getFilename(self.path)
  else
    return Path.getFilename(self.path)
  end
end

function Sample:getDecoratedName()
  local name = self.name
  if self:isDirty() then name = "*" .. name end
  if self:hasChildren() then name = "+" .. name end
  return name
end

function Sample:copyFrom(from, to, sample, sourceStart, includeSlices)
  if self.pSample and sample.pSample then
    self.pSample:copyFrom(from, to, sample.pSample, sourceStart)
    if includeSlices and sample.slices then
      self.slices:copyFrom(from, to, sample.slices, sourceStart)
    end
  end
end

return Sample
