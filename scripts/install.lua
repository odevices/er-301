-- Firmware installation script
-- Warning: It is important to assume as little as possible about the environment executing this script.
local function log(...)
  if app.logInfo then
    app.logInfo(...)
  elseif app.log then
    app.log(...)
  else
    print(string.format(...))
  end
end

local function msg(self, ...)
  self:msg(string.format(...))
end

local function join(...)
  local arg = {
    ...
  }
  return table.concat(arg, '/')
end

local function split(s, delimiter)
  local result = {}
  for match in (s .. delimiter):gmatch("(.-)" .. delimiter) do
    if match then table.insert(result, match) end
  end
  return result
end

-- creates all folders in the given path string
local function createAll(x, skipLast)
  local sep = '/'
  local dirs = split(x, sep)
  local sofar = dirs[1]
  local n = #dirs
  if skipLast then n = n - 1 end
  -- skipping the drive portion
  for i = 2, n do
    sofar = sofar .. sep .. dirs[i]
    if not app.pathExists(sofar) then
      if not app.createDirectory(sofar) then return false end
    end
  end
  return true
end

local function extract(self, archive, filename, destination, description)
  log("Trying to extract and copy %s.", filename)
  msg(self, "Installing %s: %s...", description, filename)
  if not archive:exists(filename) then
    msg(self, "Update failed: %s was not found.", filename)
    return false
  end
  createAll(destination)
  if archive:extract(filename, join(destination, filename)) then
    log("%s copied successfully.", filename)
  else
    msg(self, "Update failed: could not copy %s.", filename)
    return false
  end
  return true
end

local function install(self, archive)
  local assets = {
    {
      filename = "MLO",
      destination = "0:",
      description = "initial boot loader"
    },
    {
      filename = "SBL",
      destination = "0:",
      description = "secondary boot loader"
    },
    {
      filename = "kernel.bin",
      destination = "0:",
      description = "kernel binary"
    },
    {
      filename = "core-0.6.02.pkg",
      destination = "0:",
      description = "package"
    },
    {
      filename = "teletype-0.6.02.pkg",
      destination = "0:",
      description = "package"
    }
  }

  for _, asset in ipairs(assets) do
    if asset.filename then
      if not extract(self, archive, asset.filename, asset.destination,
                     asset.description) then return false end
    end
  end

  return true
end

return {
  install = install
}
