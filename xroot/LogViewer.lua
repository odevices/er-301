local Base = require "ListWindow"
local Class = require "Base.Class"

local LogViewer = Class {}
LogViewer:include(Base)

function LogViewer:init(title, maxHistory)
  local opts = {
    title = title,
    columns = {
      {
        name = "console",
        width = 1
      }
    }
  }
  Base.init(self, opts)
  self:setClassName("LogViewer")
  self.console = self:getColumnByName("console")
  self.maxHistory = maxHistory or 999
  self:setSubCommand(1, "Find Error", self.nextError)
  self:setSubCommand(2, "Clear", self.clearRows)
  self:setSubCommand(3, "Save", self.doSave)
end

function LogViewer:nextError(stopAt)
  local Utils = require "Utils"
  stopAt = stopAt or self.console:getSelectedIndex()
  if not self.console:scrollDown() then self.console:scrollToTop() end
  repeat
    local text = self.console:getSelected()
    if Utils.startsWith(text, "ERROR") then return end
    if not self.console:scrollDown() then self.console:scrollToTop() end
  until self.console:getSelectedIndex() == stopAt
  local Overlay = require "Overlay"
  Overlay.flashMainMessage("No error found.")
end

function LogViewer:doSave()
  local FileChooser = require "Card.FileChooser"
  local FileSystem = require "Card.FileSystem"
  local task = function(result)
    if result and result.fullpath then
      local f = io.open(result.fullpath, "w")
      if f then
        local n = self:count()
        for i = 1, n do f:write(self:get(i), "\n") end
        f:close()
        local Overlay = require "Overlay"
        Overlay.flashMainMessage("Log file saved.")
        app.logInfo("Saved log to %s.", result.fullpath)
      else
        local Overlay = require "Overlay"
        Overlay.flashMainMessage("Failed to save log file.")
        app.logError("Could not create %s.", result.fullpath)
      end
    end
  end
  local chooser = FileChooser {
    msg = string.format("Save Log (*.%s)", FileSystem.getExt("log")),
    goal = "save file",
    path = app.roots.front,
    ext = "." .. FileSystem.getExt("log"),
    pattern = FileSystem.getPattern("log"),
    history = "LogViewer"
  }
  chooser:subscribe("done", task)
  chooser:show()
end

function LogViewer:count()
  return self.console:size()
end

function LogViewer:get(i)
  return self.console:get(i - 1)
end

function LogViewer:onShow()
  self.console:scrollToBottom()
end

function LogViewer:add(text)
  self.console:addItem(text)
  if self.console:size() > self.maxHistory then self.console:removeTopItem() end
  if self.active then self.console:scrollToBottom() end
end

return LogViewer
