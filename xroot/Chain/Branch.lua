local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Chain = require "Chain"

-- Branch Class
local Branch = Class {}
Branch:include(Chain)

function Branch:init(args)
  Chain.init(self, args)
  self:setClassName("Chain.Branch")
  self.unit = args.unit
  local leftOutObject = args.leftOutObject or
                            app.logError("%s.init: leftOutObject is missing.",
                                         self)
  local leftOutletName = args.leftOutletName or
                             app.logError("%s.init: leftOutletName is missing.",
                                          self)
  self.leftOutlet = leftOutObject:getOutput(leftOutletName)
  if self.channelCount > 1 then
    local rightOutObject = args.rightOutObject or
                               app.logError(
                                   "%s.init: rightOutObject is missing.", self)
    local rightOutletName = args.rightOutletName or
                                app.logError(
                                    "%s.init: rightOutletName is missing.", self)
    self.rightOutlet = rightOutObject:getOutput(rightOutletName)
  end
end

function Branch:setUnit(unit)
  self.unit = unit
end

function Branch:getRootChain()
  if self.unit then
    return self.unit:getRootChain()
  else
    app.logInfo("%s:getRootChain: no parent unit", self)
    return self
  end
end

function Branch:getOutputDisplayName(channel)
  return string.format("%s (%s)", self.title, self.subTitle)
end

function Branch:getMonitoringOutput(ch)
  return Chain.getOutput(self, ch)
end

function Branch:getOutput(ch)
  if ch == nil or ch == 1 or self.channelCount == 1 then
    return self.leftOutlet
  elseif ch == 2 then
    return self.rightOutlet
  end
end

function Branch:enable(soft)
  self:notifyUnits("enable", soft)
end

function Branch:disable(soft)
  self:notifyUnits("disable", soft)
end

function Branch:upReleased(shifted)
  if shifted then return false end
  self:hide()
  return true
end

function Branch:homeReleased()
  self:hide()
  return true
end

function Branch:isSerializationNeeded()
  -- Serialization is needed if this branch has any units, or any inputs, or its output is connected to another chain.
  return self:length() > 0 or self.leftInputSource or self.rightInputSource or
             (self.leftOutputSource and
                 self.leftOutputSource:getConnectionCount()) or
             (self.rightInputSource and
                 self.rightInputSource:getConnectionCount())
end

return Branch
