local app = app
local Signal = require "Signal"
local Utils = require "Utils"
local Class = require "Base.Class"
local Base = require "ListWindow"
local Manager = require "Package.Manager"

-- Package Manager Interface
local Interface = Class {}
Interface:include(Base)

function Interface:init()
  local opts = {
    title = "Package Manager",
    showDetailPanel = true,
    columns = {
      {
        name = "name",
        width = 0.75,
        showCheck = true,
        emptyText = "No packages found."
      },
      {
        name = "status",
        width = 0.25,
        justify = app.justifyRight
      }
    }
  }
  Base.init(self, opts)
  self:setClassName("Package.Interface")

  self:setSubCommand(1, "uninstall all", self.doUninstallAll)
  self:setSubCommand(3, "create package", self.doCreatePackage)

  Signal.weakRegister("cardMounted", self)
  Signal.weakRegister("cardEjected", self)
end

function Interface:doRestart()
  local Application = require "Application"
  Application.restart(true)
end

function Interface:doUninstallAll()
  Manager.reset()
  self:refresh()
end

function Interface:doCreatePackage()
  local Creator = require "Package.Creator"
  Creator()
end

function Interface:doInstallPackage()
  local id = self:getSelection()
  local package = Manager.getPackage(id)
  if package then
    local success, msg = Manager.install(package)
    if success then
      self:refresh()
    else
      local Message = require "Message"
      local dialog = Message.Main("Install failed.", msg)
      dialog:show()
    end
  end
end

function Interface:doUninstallPackage()
  local id = self:getSelection()
  local package = Manager.getPackage(id)
  if package and Manager.uninstall(package) then self:refresh() end
end

function Interface:doDeletePackage()
  local id = self:getSelection()
  local package = Manager.getPackage(id)
  if package then
    local Verification = require "Verification"
    local dialog = Verification.Main(package.filename,
                                     "Do you really want to delete?")
    local task = function(ans)
      if ans then
        Manager.delete(package)
        self:refresh()
      end
    end
    dialog:subscribe("done", task)
    dialog:show()
  end
end

function Interface:doConfigurePackage()
  local id = self:getSelection()
  local package = Manager.getPackage(id)
  if package then package:libraryCall("showMenu") end
end

function Interface:refresh()
  local selected = self:getSelection()
  local packages = Manager.getPackages()
  self:clearRows()
  for id, package in Utils.orderedPairs(packages) do
    self:addRow{
      {
        label = package:getName(),
        data = id,
        checked = package.installed
      },
      {
        label = package:getVersion(),
        data = id
      }
    }
  end
  self:setSelection(selected)
  local Card = require "Card"
  if Card.mounted() then
    self.colByName["name"]:setEmptyText("No packages found.")
  else
    self.colByName["name"]:setEmptyText("No card present.")
  end
end

function Interface:onSelectionChanged()
  local id = self:getSelection()
  local package = Manager.getPackage(id)
  if package then
    if package.installed then
      self:setMainCommand(5, "uninstall", self.doUninstallPackage)
      if package:libraryCall("hasMenu") then
        self:setMainCommand(6, "config", self.doConfigurePackage)
      else
        self:setMainCommand(6, "")
      end
    else
      self:setMainCommand(5, "install", self.doInstallPackage)
      self:setMainCommand(6, "delete", self.doDeletePackage)
    end
    local title = package:getTitle()
    local author = package:getAuthor()
    local detail
    if title and author then
      detail = string.format("%s\nby\n%s", title, author)
    elseif title then
      detail = title
    elseif author then
      detail = string.format("\nby\n%s", author)
    else
      detail = ""
    end
    self.detailText:setText(detail)
  else
    self:clearMainCommand(5)
    self:clearMainCommand(6)
    self.detailText:setText("")
  end
  if Manager.isRebootNeeded() then
    self:setSubCommand(2, "restart", self.doRestart)
  else
    self:clearSubCommand(2)
  end
end

function Interface:cardMounted()
  if self.visible then self:refresh() end
end

function Interface:cardEjected()
  if self.visible then self:refresh() end
end

function Interface:onShow()
  self:refresh()
end

function Interface:enterReleased()
  local id = self:getSelection()
  local package = Manager.getPackage(id)
  if package.installed then
    Manager.uninstall(package)
  else
    Manager.install(package)
  end
  self:refresh()
  return true
end

return Interface()
