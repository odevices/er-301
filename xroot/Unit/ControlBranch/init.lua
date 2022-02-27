local Class = require "Base.Class"
local Branch = require "Chain.Branch"
local Persist = require "Persist"

local ControlBranch = Class {}
ControlBranch:include(Branch)

function ControlBranch:init(args)
  Branch.init(self, args)
  self:setInstanceName(args.id)
  self.id = args.id
  self.objects = args.objects or
                     app.logError("%s:init: objects are missing.", self)
  local task = app.ObjectList(args.id)
  for i, o in ipairs(args.objects) do
    task:add(o)
  end
  self.task = task
end

function ControlBranch:rename(name)
  self.id = name
  self:setTitle(self.title, name)
  self:setInstanceName(name)
end

function ControlBranch:onStart()
  Branch.onStart(self)
  -- Must add objects after branch so the objects are processed after the branch but before the parent chain.
  -- This is cleaner then registering the objects at depth-1.
  app.AudioThread.addTask(self.task, self.depth)
end

function ControlBranch:onStop()
  app.AudioThread.removeTask(self.task)
  Branch.onStop(self)
end

function ControlBranch:enable(soft)
  Branch.enable(self, soft)
  self.task:enable()
end

function ControlBranch:disable(soft)
  self.task:disable()
  Branch.disable(self, soft)
end

function ControlBranch:releaseResources()
  app.AudioThread.removeTask(self.task)
  self.task:clear()
  Branch.releaseResources(self)
end

function ControlBranch:isSerializationNeeded()
  return true
end

function ControlBranch:serialize()
  local t = Branch.serialize(self)
  t.id = self.control:getCustomizableValue("name")
  t.type = self.classType
  t.control = self.control:serialize()
  t.objects = Persist.serializeObjects(self.objects)
  return t
end

function ControlBranch:deserialize(t)
  if t.objects then
    Persist.deserializeObjects(self.objects, nil, t.objects)
  end
  if t.control then
    self.control:deserialize(t.control)
  end
  Branch.deserialize(self, t)
end

return ControlBranch
