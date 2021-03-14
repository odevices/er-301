local Class = require "Base.Class"
local Object = require "Base.Object"
local Persist = require "Persist"
local Path = require "Path"
local FS = require "Card.FileSystem"

local Library = Class {}
Library:include(Object)

function Library:init(args)
  self:setClassName("Package.Library")
  self.package = args.package or app.logError("%s: 'package' missing.", self)
  self:setInstanceName(args.package:getName())
  self.defaults = args.defaults or {}
end

function Library:safeCall(method, ...)
  local f = self[method]
  if f then
    local status, ret = xpcall(f, debug.traceback, self, ...)
    if status then
      return ret
    else
      app.logError("%s: error calling %s().", self, method)
      app.logInfo("Traceback:%s", ret)
    end
  else
    app.logInfo("%s: method '%s' is not implemented.", self, method)
  end
end

function Library:getInstallationPath()
  return self.package:getInstallationPath()
end

function Library:setDefaultConfiguration(t)
  self.defaults = t
end

function Library:pathToConfigurationDatabase()
  return Path.join(FS.getRoot("package-configs"),
                   self:getInstanceName() .. ".db")
end

function Library:getConfiguration(key)
  local path = self:pathToConfigurationDatabase()
  local t = Persist.readTable(path)
  return t and t[key] or self.defaults[key]
end

function Library:setConfiguration(key, value)
  local path = self:pathToConfigurationDatabase()
  local t = Persist.readTable(path) or {}
  t[key] = value
  Persist.writeTable(path, t)
end

function Library:clearConfiguration()
  local path = self:pathToConfigurationDatabase()
  if Path.exists(path) then app.deleteFile(path) end
end

function Library:showMenu()
  if self.onShowMenu == nil then return end
  local controls, order, description = self:onShowMenu()

  if controls == nil then
    app.logInfo("%s: onShowMenu did not return any controls.", self)
    return
  end

  if order == nil or #order == 0 then
    app.logInfo("%s: onShowMenu did not return an order array.", self)
    return
  end

  local Menu = require "Package.Menu"
  local menu = Menu(self:getInstanceName(), description)

  for _, cname in ipairs(order) do
    if cname == "" or cname == "newRow" then
      menu:startNewRow()
    else
      local control = controls[cname]
      if control then menu:addControl(control, control.isHeader) end
    end
  end

  menu:show()
end

function Library:onLoad()
end

function Library:onUnload()
end

function Library:hasMenu()
  return self.onShowMenu ~= nil
end

return Library
