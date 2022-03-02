local app = app
local Utils = require "Utils"
local Path = {sep = '/'}

function Path.join(...)
  local arg = {...}
  return table.concat(arg, Path.sep)
end

function Path.isAbsolute(path)
  return Utils.startsWith(path, app.roots.front) or
             Utils.startsWith(path, app.roots.rear) or
             Utils.startsWith(path, app.roots.x)
end

function Path.isRelative(path)
  return not Path.isAbsolute(path)
end

function Path.replaceExtension(filename, ext)
  return filename:gsub("%.[^%.\\/]*$", "", 1) .. "." .. ext
end

function Path.removeExtension(filename)
  return filename:gsub("%.[^%.\\/]*$", "", 1)
end

-- Includes the final slash.
function Path.getPath(path)
  local N = path:len()
  -- Find last occurence of the slash.
  local i = path:reverse():find("/")
  if i then
    -- Return everything before that.
    return path:sub(1, N - i + 1)
  else
    -- No path found.
    return ""
  end
end

function Path.getFilename(path)
  local N = path:len()
  -- Find last occurence of the slash.
  local i = path:reverse():find("/") or (N + 1)
  -- Return everything after that.
  return path:sub(N - i + 2)
end

function Path.getExtension(filename)
  return filename:match("%.([^%.\\/]*)$")
end

function Path.getStem(path)
  return Path.removeExtension(Path.getFilename(path))
end

-- returns path, filename
-- Path.split("1:/folder1/folder2/filename.ext")
-- returns: "1:/folder1/folder2", "filename.ext"
function Path.split(path)
  local N = path:len()
  -- Find last occurence of the slash.
  local i = path:reverse():find("/")
  if i then
    return path:sub(1, N - i), path:sub(N - i + 2)
  else
    -- No path found.
    return "", path
  end
end

function Path.lastFolderAndFilename(path)
  local folder, result = Path.split(path)
  if folder and folder ~= app.roots.front and folder ~= app.roots.rear then
    local lastFolder = Path.getFilename(folder)
    if lastFolder then
      result = Path.join(lastFolder, result)
    end
  end
  return result
end

function Path.create(x)
  if app.pathExists(x) then
    return true
  end
  return app.createDirectory(x)
end

-- creates all folders in the given path string
function Path.createAll(x, skipLast)
  local dirs = Utils.split(x, Path.sep)
  local sofar = dirs[1]
  local n = #dirs
  if skipLast then
    n = n - 1
  end
  -- skipping the drive portion
  for i = 2, n do
    sofar = sofar .. Path.sep .. dirs[i]
    if not app.pathExists(sofar) then
      -- app.logInfo("Creating %s", sofar)
      if not app.createDirectory(sofar) then
        return false
      end
    end
  end
  return true
end

function Path.exists(x)
  return app.pathExists(x)
end

function Path.isDirectory(x)
  return app.isDirectory(x)
end

function Path.recursiveCopy(fromPath, toPath, excludes)
  if not app.pathExists(fromPath) then
    return
  end
  if not app.isDirectory(fromPath) then
    return
  end
  Path.createAll(toPath)
  for fname in dir(fromPath) do
    local srcPath = Path.join(fromPath, fname)
    local dstPath = Path.join(toPath, fname)
    if app.isDirectory(srcPath) then
      Path.recursiveCopy(srcPath, dstPath)
    else
      if app.copyFile(srcPath, dstPath, true) then
        -- app.logInfo("Copied %s to %s", srcPath, dstPath)
      else
        app.logError("Failed: %s to %s", srcPath, dstPath)
      end
    end
  end
end

function Path.recursiveDelete(path)
  if not app.pathExists(path) then
    return false
  end
  if not app.isDirectory(path) then
    return app.deleteFile(path)
  end

  for fname in dir(path) do
    local path2 = Path.join(path, fname)
    if app.isDirectory(path2) then
      if Path.recursiveDelete(path2) then
        -- app.logInfo("Deleted folder %s", path2)
      else
        app.logError("Failed to delete %s", path2)
      end
    else
      if app.deleteFile(path2) then
        -- app.logInfo("Deleted %s", path2)
      else
        app.logError("Failed to delete %s", path2)
      end
    end
  end

  if app.deleteDirectory(path) then
    -- app.logInfo("Deleted %s", path)
  else
    app.logError("Failed to delete %s", path)
  end

  return true
end

local wdStack = {}

function Path.getWorkingDirectory()
  return wdStack[#wdStack] or app.roots.front
end

function Path.pushWorkingDirectory(path)
  wdStack[#wdStack + 1] = path
  app.logInfo("cwd: %s", Path.getWorkingDirectory())
end

function Path.popWorkingDirectory()
  if #wdStack > 0 then
    wdStack[#wdStack] = nil
  end
  app.logInfo("cwd: %s", Path.getWorkingDirectory())
end

function Path.expandRelativePath(path)
  if path then
    if Path.isAbsolute(path) then
      return path
    end
    return Path.join(Path.getWorkingDirectory(), path)
  else
    app.logError("Path.expandRelativePath: path is nil")
  end
end

return Path
