local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Object = require "Base.Object"

local off = 0
local fg = 1
local bg = 2

local MuteGroup = Class {}
MuteGroup:include(Object)

function MuteGroup:init()
  self:setClassName("MuteGroup")
  self.channels = {}
  self.soloist = nil
end

function MuteGroup:add(o)
  self.channels[o] = false
  o:onMuteStateChanged(false, self.soloist)
end

function MuteGroup:remove(o)
  if self.channels[o] == nil then
    return
  end
  if self.soloist == o then
    self:unsolo()
  end
  self.channels[o] = nil
end

function MuteGroup:solo(o)
  if self.channels[o] == nil then
    return
  end
  self.soloist = o
  for ch, muted in pairs(self.channels) do
    ch:onMuteStateChanged(muted, self.soloist)
  end
end

function MuteGroup:unsolo()
  self.soloist = nil
  for ch, muted in pairs(self.channels) do
    ch:onMuteStateChanged(muted, nil)
  end
end

function MuteGroup:toggleSolo(o)
  if self.channels[o] == nil then
    return
  end
  if self.soloist == nil then
    self:solo(o)
  elseif self.soloist == o then
    self:unsolo()
  else
    self:unsolo()
    self:solo(o)
  end
end

function MuteGroup:mute(o)
  if self.channels[o] == nil then
    return
  end
  self.channels[o] = true
  o:onMuteStateChanged(true, self.soloist)
end

function MuteGroup:unmute(o)
  if self.channels[o] == nil then
    return
  end
  self.channels[o] = false
  o:onMuteStateChanged(false, self.soloist)
end

function MuteGroup:toggleMute(o)
  if self.channels[o] == nil then
    return
  end
  if self.channels[o] then
    self:unmute(o)
  else
    self:mute(o)
  end
end

return MuteGroup
