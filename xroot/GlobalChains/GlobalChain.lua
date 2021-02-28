local Class = require "Base.Class"
local Chain = require "Chain"

-- GlobalChain Class
local GlobalChain = Class {}
GlobalChain:include(Chain)

function GlobalChain:init(args)
  args.depth = 9999 -- to force high priority
  args.sourceType = "global"
  Chain.init(self, args)
  self:setClassName("GlobalChain")
  self.isGlobal = true
end

function GlobalChain:getOutputDisplayName(channel)
  return self.title
end

function GlobalChain:upReleased(shifted)
  if shifted then return false end
  self:hide()
  return true
end

function GlobalChain:homeReleased()
  self:hide()
  return true
end

return GlobalChain
