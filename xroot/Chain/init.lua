local app = app
local Env = require "Env"
local Class = require "Base.Class"
local ChainBase = require "Chain.Base"
local HeaderSection = require "Chain.HeaderSection"
local FooterSection = require "Chain.FooterSection"
local Busy = require "Busy"

-- Chain Class
local Chain = Class {}
Chain:include(ChainBase)

function Chain:init(args)
  ChainBase.init(self, args)
  self:setClassName("Chain")
  self.stopCount = 1
  self.leftInputSource = args.leftInputSource
  self.rightInputSource = args.rightInputSource

  if args.subTitle then
    local name = string.format("%s(%s)", self.title, args.subTitle)
    self.pChain = app.UnitChain(name, self.channelCount)
  else
    self.pChain = app.UnitChain(self.title, self.channelCount)
  end

  self:connectOutputs(args.leftDestination, args.rightDestination)
  if self.leftInputSource then
    self.pChain:setInput(0, self.leftInputSource:getOutlet())
  end
  if self.rightInputSource then
    self.pChain:setInput(1, self.rightInputSource:getOutlet())
  end

  local Source = require "Source.Internal"
  local sourceType = args.sourceType or "local"
  if args.channelCount > 1 then
    self.leftOutputSource = Source(sourceType, self, 1)
    self.rightOutputSource = Source(sourceType, self, 2)
  else
    self.leftOutputSource = Source(sourceType, self)
  end

  self:disableSelection()
  self.headerSection = HeaderSection(self.title, self.subTitle,
                                     self.channelCount)
  self:subscribe("contentChanged", self.headerSection)
  self.footerSection = FooterSection(self.channelCount)
  self:subscribe("contentChanged", self.footerSection)
  self:appendSection(self.headerSection)
  self:appendSection(self.footerSection)
  self:insertEmptySection()
  self:enableSelection()
end

function Chain:connectOutputs(leftDestination, rightDestination)
  if leftDestination then self.leftDestination = leftDestination end
  if rightDestination then self.rightDestination = rightDestination end
  self.pChain:lock()
  self.pChain:connectOutput(0, self.leftDestination)
  self.pChain:connectOutput(1, self.rightDestination)
  self.pChain:unlock()
end

function Chain:disconnectOutputs()
  self.pChain:lock()
  self.pChain:disconnectOutputs()
  self.pChain:unlock()
end

function Chain:setTitle(title, subTitle)
  ChainBase.setTitle(self, title, subTitle)
  if self.leftOutputSource then self.leftOutputSource:onRename() end
  if self.rightOutputSource then self.rightOutputSource:onRename() end
end

function Chain:connectOutput(i, inlet)
  self.pChain:connectOutput(i - 1, inlet)
end

function Chain:navigateToXPath(xpath)
  local n = xpath:getNodeCount()
  if n == 0 then return end
  local chain = self
  local unit
  local type = "unit"
  self:hideOthers()
  for i = 1, n do
    -- local nodeName = xpath:getNodeName(i-1)
    local nodeIndex = xpath:getNodeIndex(i - 1)
    -- app.logInfo("%s:navigateToXPath: %s(%d)",self,nodeName,nodeIndex)
    if type == "unit" then
      if nodeIndex == 0 then
        -- This is a source, so end here.
        chain:setSelection(chain.headerSection, nil, 1)
        return
      end
      unit = chain:getUnit(nodeIndex)
      if unit == nil then
        app.logInfo("%s:navigateToXPath: no unit at index=%d", self, nodeIndex)
        chain:setSelection(chain.headerSection, nil, 0)
        return
      end
      -- select the unit header
      chain:setSelection(unit, "expanded", 1)
      unit:leftJustify()
      type = "branch"
    else
      local control = unit:getControlRaw("expanded", nodeIndex)
      if control == nil then
        app.logInfo("%s:navigateToXPath:%s has no control at index=%d", self, unit,
               nodeIndex)
        return
      end
      -- focus the first spot of the control
      local spotHandle = control:getSpotValue(1, "handle")
      if spotHandle then
        chain:setSelection(unit, "expanded", spotHandle)
        unit:leftJustify()
      end
      if i == n then
        -- Do not expand the branch/patch if this is the last node.
        return
      end
      if control.getPatch then
        local patch = control:getPatch()
        patch:show(chain.context)
        chain = patch
        chain:setSelection(chain.headerSection, nil, 0)
      elseif control.getBranch then
        local branch = control:getBranch()
        branch:show(chain.context)
        chain = branch
        chain:setSelection(chain.headerSection, nil, 0)
      else
        app.logInfo("%s:navigateToXPath: %s has no patch or branch", self, control)
        return
      end
      type = "unit"
    end
  end
end

function Chain:clear()
  local wasMuted = self:isMuted()
  if not wasMuted then self:mute() end
  local Utils = require "Utils"
  Busy.start("Clearing: %s", Utils.shorten(self.title, 42))
  self:removeAllUnits()
  self:clearInputSources()
  Busy.stop()
  -- don't touch destinations
  if not wasMuted then self:unmute() end
end

function Chain:getOutputSource(i)
  if i == 1 or i == nil or self.channelCount == 1 then
    return self.leftOutputSource
  elseif i == 2 then
    return self.rightOutputSource
  end
end

function Chain:getInputSource(i)
  if i == 1 or i == nil or self.channelCount == 1 then
    return self.leftInputSource
  elseif i == 2 then
    return self.rightInputSource
  end
end

-- used by InputControl
function Chain:setInputSource(i, src)
  if i ~= 1 and i ~= 2 then return end
  if src then
    local prev = self:getInputSource(i)
    if prev then prev:unsubscribe(self) end
    if i == 1 or self.channelCount == 1 then
      self.leftInputSource = src
    else
      self.rightInputSource = src
    end
    self.pChain:lock()
    self.pChain:setInput(i - 1, src:getOutlet())
    self.pChain:unlock()
    src:subscribe(self)
    self:emitSignal("contentChanged", self)
  else
    self:clearInputSource(i)
  end
end

-- used by InputControl
function Chain:clearInputSource(i)
  if i ~= 1 and i ~= 2 then return end
  local src = self:getInputSource(i)
  if src then
    self.pChain:lock()
    self.pChain:setInput(i - 1, nil)
    self.pChain:unlock()
    src:unsubscribe(self)
    if i == 1 then
      self.leftInputSource = nil
    elseif i == 2 then
      self.rightInputSource = nil
    end
    self:emitSignal("contentChanged", self)
  end
end

function Chain:clearInputSources()
  for i = 1, self.channelCount do self:clearInputSource(i) end
end

function Chain:onRenameSource(src)
  self:emitSignal("contentChanged", self)
end

function Chain:onDeleteSource(src)
  if src == self.leftInputSource then
    self:clearInputSource(1)
  elseif src == self.rightInputSource then
    self:clearInputSource(2)
  end
end

local function summarizeSources(x1, x2)
  local ch1, ch2
  if x1 then ch1 = x1:getDisplayName() end
  if x2 then ch2 = x2:getDisplayName() end
  if ch2 == nil then return ch1 end
  if ch1 == nil then return ch2 end
  if ch1 == ch2 then return ch1 end
  local prefix1 = ch1:match("[^%d]+")
  local prefix2 = ch2:match("[^%d]+")
  local num1 = ch1:match("[%d]+")
  local num2 = ch2:match("[%d]+")
  if prefix1 == prefix2 and num1 and num2 then
    return prefix1 .. num1 .. "+" .. num2
  else
    return ch1 .. "+" .. ch2
  end
end

function Chain:mnemonic()
  local n = self:length()
  local any = self.leftInputSource or self.rightInputSource
  local sources = summarizeSources(self.leftInputSource, self.rightInputSource)
  if n == 0 then
    if not any then
      return "empty"
    else
      return sources
    end
  else
    if not any then
      if n == 1 then
        return self:getUnit(1).mnemonic
      else
        local u1 = self:getUnit(1).mnemonic
        local u2 = self:getUnit(n).mnemonic
        return u1 .. ".." .. u2
      end
    else
      return string.format("f(%s)", sources)
    end
  end
end

function Chain:updateDepth(depth)
  local wasStarted = self.started
  if wasStarted then self:stop() end
  self.depth = depth
  if wasStarted then self:start() end
end

function Chain:start()
  -- app.logInfo("%s:start:stopCount=%d",self,self.stopCount)
  if self.stopCount > 0 then
    self.stopCount = self.stopCount - 1
    if self.stopCount == 0 then
      self.started = true
      self:onStart()
    end
  end
end

function Chain:onStart()
  for _, unit in pairs(self.units) do unit:start() end
  app.AudioThread.addTask(self.pChain, self.depth)
end

function Chain:stop()
  -- app.logInfo("%s:stop:stopCount=%d",self,self.stopCount)
  if self.stopCount == 0 then
    self.stopCount = 1
    self.started = false
    self:onStop()
  else
    self.stopCount = self.stopCount + 1
  end
end

function Chain:onStop()
  app.AudioThread.removeTask(self.pChain)
  for _, unit in pairs(self.units) do unit:stop() end
end

function Chain:serialize()
  local t = {
    lastPresetPath = self.lastPresetPath,
    lastPresetFilename = self.lastPresetFilename,
    firmwareVersion = app.FIRMWARE_VERSION,
    instanceKey = self:getInstanceKey()
  }
  t.units = self:serializeUnits()
  t.selection = self:serializeSelection()
  -- sources
  local channels = {
    count = self.channelCount,
    inputs = {}
  }
  if self.leftInputSource then
    channels.inputs[1] = self.leftInputSource:serialize()
  end
  if self.rightInputSource then
    channels.inputs[2] = self.rightInputSource:serialize()
  end
  t.channels = channels
  return t
end

function Chain:deserialize(t)
  local wasMuted = self:isMuted()
  if not wasMuted then self:mute() end
  self:startDeserialization()
  self:setLastPreset(t.lastPresetPath, t.lastPresetFilename)
  if t.instanceKey then self:setInstanceKey(t.instanceKey) end
  self:deserializeUnits(t.units)
  -- clear existing input channels
  self:clearInputSources()
  local channels = t.channels
  if channels and channels.inputs then
    local Application = require "Application"
    -- do not change the number of channels
    for i = 1, self.channelCount do
      local sourceData = channels.inputs[i]
      if sourceData then
        -- Delay this until all deserialization is done.
        Application.post(function()
          self:setInputWithSourceData(i, sourceData)
        end)
      end
    end
  else
    app.logInfo("%s:deserialize: 'channels' or 'channels.inputs' is null.", self)
  end
  self:deserializeSelection(t.selection)
  self:finishDeserialization()
  if not wasMuted then self:unmute() end
end

function Chain:setInputWithSourceData(i, data)
  local Source = require "Source"
  local source = Source.deserialize(data, self)

  if source == nil then
    if type(data) == "table" then
      app.logInfo("%s:setInputWithSourceData:NOT found", self)
      local Utils = require "Utils"
      Utils.pp(data)
    else
      app.logInfo("%s:setInputWithSourceData(%s,%s):NOT found", self, i, data)
    end
  end

  -- Clears assigned source if nil.
  self:setInputSource(i, source)
end

-- Manually free resources instead of waiting for the gc.
function Chain:releaseResources()
  -- app.logInfo("%s:releaseResources()",self)
  self:unsubscribe("contentChanged", self.headerSection)
  self:unsubscribe("contentChanged", self.footerSection)

  self:clearInputSources()
  if self.leftOutputSource then self.leftOutputSource:onDelete() end
  if self.rightOutputSource then self.rightOutputSource:onDelete() end

  self:removeAllUnits()
  self.pChain:lock()
  self.pChain:disconnectOutputs()
  self.pChain:unlock()
end

function Chain:onShow()

end

function Chain:onHide()

end

return Chain
