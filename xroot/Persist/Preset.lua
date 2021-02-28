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
    data.firmwareVersion = Env.Version
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

function Preset:getVersionString()
  if self.data == nil then return end
  local result = self:get("firmwareVersion/SimpleString")
  if result then
    return result
  else
    local V = self.data.firmwareVersion or self.data.version
    if V and V.Major and V.Minor and V.Build then
      return string.format("%d.%d.%02d", V.Major, V.Minor, V.Build)
    end
    return "0.0.00"
  end
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
