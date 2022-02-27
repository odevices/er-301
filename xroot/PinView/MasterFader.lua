local Class = require "Base.Class"
local Base = require "PinView.MasterControl"
local Encoder = require "Encoder"
local ply = app.SECTION_PLY

local MasterFader = Class {}
MasterFader:include(Base)

function MasterFader:init(args)
  Base.init(self, args)
  self:setClassName("PinView.MasterFader")
  local name = args.name or app.logError("%s.init: name is missing.", self)
  local task = args.task or app.logError("%s.init: task is missing.", self)
  local fader = app.MorphFader()
  self.morpher = app.ParamSetMorph()
  fader:setParamSetMorph(self.morpher)
  fader:setLabel(name)
  fader:setMap(Encoder.getMap("percent"))
  fader:setPrecision(0)
  self.fader = fader
  self.task = task
  self:setMainCursorController(fader)
  self:setControlGraphic(fader)
  self:addSpotDescriptor{
    center = 0.5 * ply
  }
  self.params = {}
  self.targets = {}
end

function MasterFader:getFloatingMenuItems()
  local t = {}
  if self:queryUp("currentViewName") == "expanded" then
    t[1] = "collapse"
  else
    t[1] = "expand"
  end
  t[2] = "clone"
  t[3] = "unpin all"
  return t
end

function MasterFader:onFloatingMenuSelection(choice)
  if choice == "collapse" then
    self:callUp("switchView", "collapsed", self)
    return true
  elseif choice == "expand" then
    self:callUp("switchView", "expanded", self)
    return true
  elseif choice == "clone" then
    self:callUp("cloneSelectedPinSet")
    return true
  elseif choice == "unpin all" then
    self:callUp("unpinAll")
    return true
  end
end

function MasterFader:setName(name)
  self.fader:setLabel(name)
  self.morpher:setName(name)
end

function MasterFader:showName()
  self.fader:showLabel()
end

function MasterFader:hideName()
  self.fader:hideLabel()
end

function MasterFader:setNameCenter(x)
  self.fader:setLabelCenter(x)
end

function MasterFader:addFollower(controlParam, targetParam)
  local i = #self.params + 1
  self.params[i] = controlParam
  self.targets[i] = targetParam
  if app.getModeToggleState() == "hold" then
    self.morpher:add(controlParam, targetParam:target())
  end
end

function MasterFader:removeFollower(param)
  local h = param:handle()
  for i, param2 in ipairs(self.params) do
    if param2:handle() == h then
      table.remove(self.params, i)
      table.remove(self.targets, i)
      break
    end
  end
  self.morpher:remove(param)
end

function MasterFader:clear()
  self.morpher:clear()
  self.params = {}
  self.targets = {}
end

function MasterFader:count()
  return #self.params
end

function MasterFader:onFocus()
  self:callUp("notifyPinControls", "highlightValue")
end

function MasterFader:onDefocus()
  self:callUp("notifyPinControls", "highlightTarget")
end

function MasterFader:engage()
  self:callUp("notifyOtherPinSets", self.parent, "disengage")
  self.morpher:clear()
  for i, param in ipairs(self.params) do
    self.morpher:add(param, self.targets[i]:target())
  end
  self.task:lock()
  self.task:clear()
  self.task:add(self.morpher)
  self.task:unlock()
  self.fader:save()
  self.engaged = true
end

function MasterFader:disengage()
  self.engaged = false
  self.task:lock()
  self.task:clear()
  self.task:unlock()
  self.morpher:clear()
end

function MasterFader:spotReleased(spot, shifted)
  if shifted then
    return false
  end
  if self.focused then
    self:unfocus()
  else
    self:focus()
  end
  return true
end

function MasterFader:upReleased(shifted)
  if not shifted then
    self:unfocus()
  end
  return true
end

function MasterFader:serialize()
  local t = Base.serialize(self)
  if self.encoderState ~= Encoder.Coarse then
    t.encoderState = Encoder.serialize(self.encoderState)
  end
  return t
end

function MasterFader:deserialize(t)
  Base.deserialize(self, t)
  local encoderState = t.encoderState
  if encoderState then
    self.encoderState = Encoder.deserialize(encoderState)
  end
end

function MasterFader:encoder(change, shifted)
  if not self.engaged then
    self:engage()
  end
  local fine = self.encoderState == Encoder.Fine
  self.fader:encoder(change, shifted, fine)
  return true
end

function MasterFader:cancelReleased(shifted)
  if shifted then
    return false
  end
  self.fader:restore()
  return true
end

function MasterFader:zeroPressed()
  if not self.engaged then
    self:engage()
  end
  self.morpher:softSet(0)
  return true
end

function MasterFader:homePressed()
  if not self.engaged then
    self:engage()
  end
  self.morpher:softSet(0)
  return true
end

function MasterFader:enterReleased()
  if not self.engaged then
    self:engage()
  end
  self.morpher:softSet(1)
  return true
end

function MasterFader:commitReleased()
  if not self.engaged then
    self:engage()
  end
  self.morpher:softSet(1)
  return true
end

return MasterFader
