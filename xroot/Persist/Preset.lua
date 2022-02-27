local Class = require "Base.Class"
local Object = require "Base.Object"
local Env = require "Env"
local FileSystem = require "Card.FileSystem"
local Serialization = require "Persist.Serialization"
local Utils = require "Utils"
local Path = require "Path"

local Preset = Class {}
Preset:include(Object)

function Preset:init(data)
  self:setClassName("Preset")
  self.data = data
end

function Preset:write(fullpath)
  local data = self.data
  if data then
    data.firmwareVersion = app.FIRMWARE_VERSION
    return Serialization.writeTable(fullpath, data)
  end
end

function Preset:read(fullpath)
  self.data = Serialization.readTable(fullpath)
  return self.data ~= nil
end

function Preset:get(xpath)
  return self.data and Serialization.get(xpath, self.data)
end

-- This routine is trying to cover the different ways version info was encoded in presets in the past.
function Preset:getVersionString()
  local defaultVersion = "0.0.00"

  if self.data == nil then
    return defaultVersion
  end
  local V = self.data.firmwareVersion or self.data.version
  if V == nil then
    return defaultVersion
  end

  if type(V) == "string" then
    return V
  end

  if type(V) == "table" then
    if V.SimpleString then
      return V.SimpleString
    elseif V.Major and V.Minor and V.Build then
      return string.format("%d.%d.%02d", V.Major, V.Minor, V.Build)
    end
  end

  return defaultVersion
end

local function walkHelper(value, path, callback)
  for k, v in pairs(value) do
    if type(v) == "table" then
      walkHelper(v, path .. "." .. k, callback)
    else
      callback(path, k, v)
    end
  end
end

-- Traverse the preset data executing the callback on each non-table element.
-- callback(path, key, value)
function Preset:walk(callback)
  walkHelper(self.data, "", callback)
end

return Preset
