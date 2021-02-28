local app = app
local Class = require "Base.Class"
local Object = require "Base.Object"

local Base = Class {}
Base:include(Object)

function Base:init(name, category, description, value, onSet)
  self.name = name
  self.category = category
  self.description = description
  self.value = value
  self.type = "generic"
  self.onSet = onSet
end

local Boolean = Class {}
Boolean:include(Base)

function Boolean:init(name, category, description, value, onSet)
  Base.init(self, name, category, description, value, onSet)
  self:setClassName("Boolean")
  self.type = "bool"
end

function Boolean:getValueText()
  if self.value then
    return "yes"
  else
    return "no"
  end
end

function Boolean:set(x)
  if type(x) == "string" then
    self.value = (x == "yes" or x == "true")
    app.logInfo("%s = %s", self.name, self.value)
  elseif type(x) == "boolean" then
    self.value = x
    app.logInfo("%s = %s", self.name, self.value)
  end
  if self.onSet then self.onSet(self.value) end
end

local String = Class {}
String:include(Base)

function String:init(name, category, description, value, choices, onSet)
  Base.init(self, name, category, description, value, onSet)
  self:setClassName("String")
  self.type = "string"
  self.choices = choices or {}
end

function String:getValueText()
  return self.value
end

function String:set(x)
  if type(x) == "string" then
    self.value = x
    app.logInfo("%s = %s", self.name, self.value)
  end
  if self.onSet then self.onSet(self.value) end
end

local Number = Class {}
Number:include(Base)

function Number:init(name, category, description, value, min, max, onSet)
  Base.init(self, name, category, description, value, onSet)
  self:setClassName("Number")
  self.type = "number"
  self.min = min or -1e9
  self.max = max or 1e9
end

function Number:getValueText()
  return tostring(self.value)
end

function Number:set(x)
  if x == nil then return end
  self.value = tonumber(x)
  app.logInfo("%s = %f", self.name, self.value)
  if self.onSet then self.onSet(self.value) end
end

return {
  Boolean = Boolean,
  String = String,
  Number = Number
}
