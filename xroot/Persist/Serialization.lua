local Utils = require "Utils"
local roethlin = require "Persist.roethlin"

local function legacyDeserialize(data)
  local env = setmetatable({}, {
    __index = function(t, k)
      return t
    end,
    __call = function(t, ...)
      error("cannot call functions")
    end
  })
  local f, res = load('return ' .. data, nil, nil, env)
  if not f then f, res = load(data, nil, nil, env) end
  if not f then return f, res end
  return pcall(f)
end

local function legacyReadTable(filename)
  local stream = io.open(filename, "r")
  if stream == nil then return nil end
  local encoded = stream:read("*all")
  stream:close()
  app.collectgarbage()
  local status, result = legacyDeserialize(encoded)
  if status then
    return result
  else
    app.logInfo("Decode error: %s", result)
    return nil
  end
end

local function readTable(filename)
  return roethlin.load(filename) or legacyReadTable(filename)
end

local function writeTable(filename, t, hooks)
  if hooks then
    return roethlin.storeWithHooks(filename, hooks, t)
  else
    return roethlin.store(filename, t)
  end
end

local function get(path, t)
  local keys = Utils.split(path, "/")
  for _, key in ipairs(keys) do
    t = t[key]
    if t == nil then return nil end
  end
  return t
end

return {
  readTable = readTable,
  writeTable = writeTable,
  get = get
}
