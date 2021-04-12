local app = app
local Class = require "Base.Class"
local Object = require "Base.Object"
local Channels = require "Channels"

local Player = Class {}
Player:include(Object)

function Player:init()
  self:setClassName("Card.Player")
  self.state = "Stopped"

  self.monoUnit = app.Unit("Card.Player.MonoUnit", 1)
  self.monoFileSource = app.FileSource(1)
  self.monoUnit:addObject(self.monoFileSource)
  self.monoUnit:setOutput(0, self.monoFileSource, "Out")
  self.monoUnit:compile()
  self.monoChain = app.UnitChain("Card.Player.MonoChain", 1)
  self.monoChain:appendUnit(self.monoUnit)
  self.monoChain:mute()

  self.stereoUnit = app.Unit("Card.Player.StereoUnit", 2)
  self.stereoFileSource = app.FileSource(2)
  self.stereoUnit:addObject(self.stereoFileSource)
  self.stereoUnit:setOutput(0, self.stereoFileSource, "Left Out")
  self.stereoUnit:setOutput(1, self.stereoFileSource, "Right Out")
  self.stereoUnit:compile()
  self.stereoChain = app.UnitChain("Card.Player.StereoChain", 2)
  self.stereoChain:appendUnit(self.stereoUnit)
  self.stereoChain:mute()
end

function Player:play(path, position, loop, channel)
  self:stop()

  channel = channel or self.preemptedChannel or self.defaultChannel or
                Channels.selected()
  local preemptedChain = Channels.getChain(channel)
  local leftDestination = preemptedChain.leftDestination
  local rightDestination = preemptedChain.rightDestination
  local channelCount = preemptedChain.channelCount

  if channelCount == 2 then
    self.unit = self.stereoUnit
    self.fileSource = self.stereoFileSource
    self.pChain = self.stereoChain
  else
    self.unit = self.monoUnit
    self.fileSource = self.monoFileSource
    self.pChain = self.monoChain
  end

  if not self.fileSource:open(path) then
    app.logInfo("%s: unable to open %s", self, path)
    return false
  end

  -- Samples < 50ms are assumed to be wavetables.
  if loop or self.fileSource:getDurationInSeconds() < 0.050 then
    self.fileSource:setLooping(true)
  else
    self.fileSource:setLooping(false)
  end

  if position ~= nil then self.fileSource:setPositionInSamples(position) end

  self.preemptedChannel = channel
  self.wasMuted = preemptedChain:muteIfNeeded()
  preemptedChain:disconnectOutputs()

  self:connectOutputs(leftDestination, rightDestination)
  app.AudioThread.addTask(self.pChain, 1)
  self.pChain:unmute()

  self.state = "Playing"
  self.path = path
  self.channelCount = channelCount
  app.ChannelLEDs_off(0)
  app.ChannelLEDs_off(1)
  app.ChannelLEDs_off(2)
  app.ChannelLEDs_off(3)
  app.ChannelLEDs_on(channel - 1)
  app.ChannelLEDs_steady(channel - 1)
  return true
end

function Player:connectOutputs(leftDestination, rightDestination)
  self.pChain:lock()
  self.pChain:disconnectOutputs()
  self.pChain:connectOutput(0, leftDestination)
  self.pChain:connectOutput(1, rightDestination)
  self.pChain:unlock()
end

function Player:disconnectOutputs()
  self.pChain:lock()
  self.pChain:disconnectOutputs()
  self.pChain:unlock()
end

function Player:stop()
  if self.state == "Playing" then
    self.pChain:mute()
    app.AudioThread.removeTask(self.pChain)
    self:disconnectOutputs()
    self.fileSource:close()
    local chain = Channels.getChain(self.preemptedChannel)
    chain:connectOutputs()
    chain:unmuteIfNeeded(self.wasMuted)
    self.state = "Stopped"
  end
end

function Player:isEOF()
  return self.state == "Playing" and self.fileSource:isEOF()
end

function Player:setDefaultChannel(channel)
  self.defaultChannel = channel
end

function Player:changeDestination(channel)
  if self.state == "Playing" then
    local position = self.fileSource:getPositionInSamples()
    self:stop()
    self:play(self.path, position, nil, channel)
  end
  self.preemptedChannel = channel
end

function Player:getFileSource()
  return self.fileSource
end

function Player:getPath()
  return self.path
end

function Player:getOutlets()
  if self.state == "Playing" then
    if self.channelCount == 2 then
      return self.fileSource:getOutput("Left Out"),
             self.fileSource:getOutput("Right Out")
    else
      return self.fileSource:getOutput("Out"), nil
    end
  else
    return nil, nil
  end
end

return Player
