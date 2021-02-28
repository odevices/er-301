local PeriodWidget = require "SlicingView.PeriodWidget"
local DivisionWidget = require "SlicingView.DivisionWidget"
local OnsetWidget = require "SlicingView.OnsetWidget"
local ShiftWidget = require "SlicingView.ShiftWidget"
local Verification = require "Verification"
local Message = require "Message"
local Busy = require "Busy"
local MenuOperations = {}

local function prepare(self)
  if self.sample==nil or self.sample.pSample==nil then
    return
  end

  if self.mainDisplay:isMarked() then
    local from = self.mainDisplay:getMarkStart()
    local to = self.mainDisplay:getMarkEnd()
    return self.sample, from, to
  else
    return self.sample, 0, self.sample:length()
  end
end

function MenuOperations:doClearSlices()
  local Settings = require "Settings"
  if Settings.get("confirmClearSlices")=="yes" then
    local dlg = Verification.Main("Clearing all slices...","Are you sure?")
    local task = function(ans)
      if ans then
        self.mainDisplay:clearSlices()
      end
    end
    dlg:subscribe("done", task)
    dlg:show()
  else
    self.mainDisplay:clearSlices()
  end
end

function MenuOperations:doPeriodSlicing()
  PeriodWidget(self):show()
end

function MenuOperations:doDivisionSlicing()
  DivisionWidget(self):show()
end

function MenuOperations:doOnsetSlicing()
  OnsetWidget(self):show()
end

function MenuOperations:doShiftSlices()
  ShiftWidget(self):show()
end

function MenuOperations:doTrim()
  local sample, from, to = prepare(self)
  local Ns = to - from
  if Ns < 32 then
    local Overlay = require "Overlay"
    Overlay.mainFlashMessage("Trim failed. Region is too small.")
  end
  if sample then
    Busy.start("Trimming...")
    local SamplePool = require "Sample.Pool"
    local newSample, status = SamplePool.clone(sample,Ns)
    if not newSample then
      Busy.stop()
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Trim failed. Could not clone sample.")
      return
    end
    newSample:copyFrom(0,Ns,sample,from,true)
    if self.unit then
      self.unit:setSample(newSample)
    end
    if not sample:inUse() then
      SamplePool.unload(sample)
    end
    self.mainDisplay:movePointerToViewCenter()
    self.mainDisplay:clearMarking()
    Busy.stop()
  end
end

function MenuOperations:doCut()
  local sample, from, to = prepare(self)
  local As = from
  local Bs = sample:length() - to
  local Ns = As + Bs
  if Ns < 32 then
    local Overlay = require "Overlay"
    Overlay.mainFlashMessage("Cut failed. Remaining region is too small.")
  end
  if sample then
    Busy.start("Cutting...")
    local SamplePool = require "Sample.Pool"
    local newSample, status = SamplePool.clone(sample,Ns)
    if not newSample then
      Busy.stop()
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Cut failed. Could not clone sample.")
      return
    end
    if As > 0 then
      newSample:copyFrom(0,from,sample,0,true)
    end
    if Bs > 0 then
      newSample:copyFrom(As,Ns,sample,to,true)
    end
    if self.unit then
      self.unit:setSample(newSample)
    end
    if not sample:inUse() then
      SamplePool.unload(sample)
    end
    self.mainDisplay:movePointerToViewCenter()
    self.mainDisplay:clearMarking()
    Busy.stop()
  end
end

function MenuOperations:doSilence()
  local sample, from, to = prepare(self)
  if sample then
    Busy.start("Silencing...")
    local Ns = sample:length()
    local SamplePool = require "Sample.Pool"
    local newSample, status = SamplePool.clone(sample,Ns)
    if not newSample then
      Busy.stop()
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Silence failed. Could not clone sample.")
      return
    end
    newSample:copyFrom(0,Ns,sample,0,true)
    newSample.pSample:silence(from,to)
    if self.unit then
      self.unit:setSample(newSample)
    end
    if not sample:inUse() then
      SamplePool.unload(sample)
    end
    Busy.stop()
  end
end

function MenuOperations:doFadeIn()
  local sample, from, to = prepare(self)
  if sample then
    Busy.start("Fading in...")
    local Ns = sample:length()
    local SamplePool = require "Sample.Pool"
    local newSample, status = SamplePool.clone(sample,Ns)
    if not newSample then
      Busy.stop()
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Fade failed. Could not clone sample.")
      return
    end
    newSample:copyFrom(0,Ns,sample,0,true)
    newSample.pSample:fadeIn(from,to)
    if self.unit then
      self.unit:setSample(newSample)
    end
    if not sample:inUse() then
      SamplePool.unload(sample)
    end
    Busy.stop()
  end
end

function MenuOperations:doFadeOut()
  local sample, from, to = prepare(self)
  if sample then
    Busy.start("Fading out...")
    local Ns = sample:length()
    local SamplePool = require "Sample.Pool"
    local newSample, status = SamplePool.clone(sample,Ns)
    if not newSample then
      Busy.stop()
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Fade failed. Could not clone sample.")
      return
    end
    newSample:copyFrom(0,Ns,sample,0,true)
    newSample.pSample:fadeOut(from,to)
    if self.unit then
      self.unit:setSample(newSample)
    end
    if not sample:inUse() then
      SamplePool.unload(sample)
    end
    Busy.stop()
  end
end

function MenuOperations:doNormalize()
  local sample, from, to = prepare(self)
  if sample then
    Busy.start("Normalizing...")
    local Ns = sample:length()
    local SamplePool = require "Sample.Pool"
    local newSample, status = SamplePool.clone(sample,Ns)
    if not newSample then
      Busy.stop()
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Normalize failed. Could not clone sample.")
      return
    end
    newSample:copyFrom(0,Ns,sample,0,true)
    newSample.pSample:normalize(from,to)
    if self.unit then
      self.unit:setSample(newSample)
    end
    if not sample:inUse() then
      SamplePool.unload(sample)
    end
    Busy.stop()
  end
end

function MenuOperations:doRemoveDC()
  local sample, from, to = prepare(self)
  if sample then
    Busy.start("Removing DC...")
    local Ns = sample:length()
    local SamplePool = require "Sample.Pool"
    local newSample, status = SamplePool.clone(sample,Ns)
    if not newSample then
      Busy.stop()
      local Overlay = require "Overlay"
      Overlay.mainFlashMessage("Remove DC failed. Could not clone sample.")
      return
    end
    newSample:copyFrom(0,Ns,sample,0,true)
    newSample.pSample:removeDC(from,to)
    if self.unit then
      self.unit:setSample(newSample)
    end
    if not sample:inUse() then
      SamplePool.unload(sample)
    end
    Busy.stop()
  end
end

function MenuOperations:doSave()
  local SamplePool = require "Sample.Pool"
  local Overlay = require "Overlay"
  local sample = self.sample
  if sample then
    local success, status = SamplePool.save(sample)
    if success then
      Overlay.mainFlashMessage("Saved: %s",sample:getFullPathForDisplay(32))
    elseif status then
      Overlay.mainFlashMessage(status)
    end
  end
end

function MenuOperations:doSaveAs()
  local SamplePool = require "Sample.Pool"
  local Overlay = require "Overlay"
  local sample = self.sample
  if sample then
    local success, status = SamplePool.save(sample,true)
    if success then
      Overlay.mainFlashMessage("Saved: %s",sample:getFullPathForDisplay(32))
    elseif status then
      Overlay.mainFlashMessage(status)
    end
  end
end

function MenuOperations:doRevert()
  local sample = self.sample
  if sample then
    local Overlay = require "Overlay"
    local SamplePool = require "Sample.Pool"
    local revertedSample, status = SamplePool.revert(sample)
    if revertedSample then
      if self.unit then
        self.unit:setSample(revertedSample)
      end
      if not sample:inUse() then
        SamplePool.unload(sample)
      end
      self.mainDisplay:movePointerToViewCenter()
      Overlay.mainFlashMessage("Reverted: %s",revertedSample:getFullPathForDisplay(32))
    elseif status then
      Overlay.mainFlashMessage(status)
    end
  end
end

return MenuOperations
