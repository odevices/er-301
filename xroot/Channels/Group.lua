local app = app
local Class = require "Base.Class"
local Object = require "Base.Object"
local Context = require "Base.Context"
local RootChain = require "Chain.Root"

local names = {
  "OUT1",
  "OUT2",
  "OUT3",
  "OUT4"
}

local ChannelGroup = Class {}
ChannelGroup:include(Object)

function ChannelGroup:init(left, right)
  self:setClassName("Channels.Group")
  app.ChannelLEDs_steady(left - 1)
  local leftName = names[left]
  local title, channelCount
  local leftDestination = app.getExternalDestination(leftName)
  local rightDestination
  if right then
    local rightName = names[right]
    title = string.format("%s + %s", leftName, rightName)
    rightDestination = app.getExternalDestination(rightName)
    channelCount = 2
    app.ChannelLEDs_steady(right - 1)
  else
    title = leftName
    channelCount = 1
  end

  self:setInstanceName(title)
  local chain = RootChain {
    title = title,
    depth = 1,
    leftDestination = leftDestination,
    rightDestination = rightDestination,
    channelCount = channelCount
  }

  self.chain = chain
  self.editContext = Context(chain.title .. " edit", chain)
  self.scopeContext = Context(chain.title .. " scope", chain.scopeView)
  self.holdContext = Context(chain.title .. " hold", chain.pinView)
  self.activeContext = self.editContext
  self.mode = "unknown"
  self.left = left
  self.right = right

  chain:start()
  chain:subscribe("contentChanged", self)
end

function ChannelGroup:contentChanged()
  if self.mode == "scope" then self.chain:enterScopeView() end
end

function ChannelGroup:show()
  if not self.activeContext.visible then
    local Application = require "Application"
    Application.setVisibleContext(self.activeContext)
  end
end

function ChannelGroup:setActiveContext(context)
  local visible = self.activeContext.visible
  self.activeContext = context
  if visible then self:show() end
end

function ChannelGroup:setMode(mode)
  if mode == self.mode then return end
  if mode == "edit" then
    if self.mode == "scope" then
      self.chain:leaveScopeView()
    else
      self.chain:leaveHoldMode()
    end
    self:setActiveContext(self.editContext)
  elseif mode == "scope" then
    self.chain:enterScopeView()
    self:setActiveContext(self.scopeContext)
  elseif mode == "hold" then
    self.chain:enterHoldMode()
    self:setActiveContext(self.holdContext)
  end
  self.mode = mode
end

function ChannelGroup:mute()
  self.chain:mute()
  self.chain:setSubTitle("muted")
  app.ChannelLEDs_flash(self.left - 1)
  if self.right then app.ChannelLEDs_flash(self.right - 1) end
end

function ChannelGroup:unmute()
  self.chain:unmute()
  self.chain:clearSubTitle()
  app.ChannelLEDs_steady(self.left - 1)
  if self.right then app.ChannelLEDs_steady(self.right - 1) end
end

function ChannelGroup:toggleMute()
  local Overlay = require "Overlay"
  local chain = self.chain
  if chain:isMuted() then
    Overlay.mainFlashMessage(chain.title .. ": Mute Off")
    self:unmute()
  else
    self:mute()
    Overlay.mainFlashMessage(chain.title .. ": Mute On")
  end
end

function ChannelGroup:start()
  self.chain:start()
end

function ChannelGroup:stop()
  self.chain:stop()
end

function ChannelGroup:clear()
  self.chain:clear()
end

function ChannelGroup:destroy()
  self.chain:unsubscribe("contentChanged", self)
  self.chain:mute()
  self.chain:stop()
  self.chain:releaseResources()
  self.editContext:destroy()
  self.scopeContext:destroy()
  self.holdContext:destroy()
end

function ChannelGroup:serialize()
  local t = self.chain:serialize()
  -- Used only by Channels module
  t.isMuted = self.chain:isMuted()
  return t
end

function ChannelGroup:deserialize(t)
  self.chain:deserialize(t)
  self.chain.stopCount = 1
end

return ChannelGroup
