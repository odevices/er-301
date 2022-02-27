local Base = require "ListWindow"
local Class = require "Base.Class"
local Utils = require "Utils"

local FileStaging = Class {}
FileStaging:include(Base)

function FileStaging:init(paths)
  -- Specify each column.
  local opts = {
    title = "Confirm Selection",
    columns = {
      {
        name = "number",
        width = 0.1
        -- justify = app.justifyRight
      }, {
        name = "path",
        width = 0.9,
        showCheck = true,
        emptyText = "No files selected."
      }
    }
  }
  Base.init(self, opts)
  self:setClassName("Card.FileStaging")

  -- Set up buttons.
  self:setSubCommand(1, "Select None", self.doDeselectAll)
  self:setSubCommand(2, "Select All", self.doSelectAll)
  self:setSubCommand(3, " Add More ...", self.doAddMore)
  self:setMainCommand(6, "grab", self.doEndGrab, self.doStartGrab)
  -- self:setMainCommand(1, "sort", self.doSort)

  self.pathHash = {}
  if paths then
    self:addPaths(paths)
  end
end

function FileStaging:doSort()
  local pathColumn = self:getColumnByName("path")
  if self.sorted then
    pathColumn:sortDescending()
    self.sorted = false
  else
    pathColumn:sortAscending()
    self.sorted = true
  end
end

function FileStaging:doAddMore()
  self:hide()
  self:emitSignal("more", self:getCheckedPaths())
end

function FileStaging:doDeselectAll()
  local pathColumn = self:getColumnByName("path")
  pathColumn:uncheckAll()
  self:updateTitle()
end

function FileStaging:doSelectAll()
  local pathColumn = self:getColumnByName("path")
  pathColumn:checkAll()
  self:updateTitle()
end

function FileStaging:doStartGrab()
  self.grabbing = true
  local pathColumn = self:getColumnByName("path")
  pathColumn:showHighlight()
end

function FileStaging:doEndGrab()
  self.grabbing = false
  local pathColumn = self:getColumnByName("path")
  pathColumn:hideHighlight()
end

function FileStaging:onSelectionChanged(prevIndex, curIndex)
  if self.grabbing then
    local pathColumn = self:getColumnByName("path")
    pathColumn:swap(prevIndex, curIndex)
  end
end

function FileStaging:updateTitle()
  local pathColumn = self:getColumnByName("path")
  local count = pathColumn:countCheckState()
  self:setTitle("%d of %d selected", count, pathColumn:size())
end

function FileStaging:addPaths(paths)
  local Busy = require "Busy"
  Busy.start()
  local n = self:size()
  for i, path in ipairs(paths) do
    if not self.pathHash[path] then
      self:addRow{
        {
          label = tostring(n + i)
        },
        {
          label = path,
          data = path,
          checked = true
        }
      }
      self.pathHash[path] = true
    end
  end
  self:optimize()
  self:updateTitle()
  Busy.stop()
end

function FileStaging:optimize()
  -- Determine required width fof number column.
  local n = self:size()
  local numWidth
  if n < 100 then
    numWidth = 0.05
  elseif n < 1000 then
    numWidth = 0.075
  else
    numWidth = 0.1
  end
  self:setColumnWidths(numWidth, 1 - numWidth)

  local longestPath = app.LongestPath()
  local pathColumn = self:getColumnByName("path")
  for i = 1, pathColumn:size() do
    longestPath:add(pathColumn:getData(i - 1))
  end
  local longest = longestPath:calculate()
  if longest:len() > 2 then
    for i = 1, pathColumn:size() do
      local path = pathColumn:getData(i - 1)
      path = Utils.shortenPath(path:gsub(longest, ".."), 40)
      pathColumn:updateNameByIndex(path, i - 1)
    end
  else
    for i = 1, pathColumn:size() do
      local path = pathColumn:getData(i - 1)
      path = Utils.shortenPath(path, 40)
      pathColumn:updateNameByIndex(path, i - 1)
    end
  end
end

function FileStaging:getCheckedPaths()
  local checked = app.checkYes
  local pathColumn = self:getColumnByName("path")
  local paths = {}
  for i = 1, pathColumn:size() do
    if pathColumn:getCheckState(i - 1) == checked then
      paths[#paths + 1] = pathColumn:getData(i - 1)
    end
  end
  return paths
end

function FileStaging:enterReleased()
  local pathColumn = self:getColumnByName("path")
  if pathColumn:getSelectedCheckState() == app.checkNo then
    pathColumn:checkSelected()
  else
    pathColumn:uncheckSelected()
  end
  self:updateTitle()
  return true
end

function FileStaging:commitReleased()
  self:hide()
  self:emitSignal("done", self:getCheckedPaths())
  return true
end

function FileStaging:cancelReleased(shifted)
  if not shifted then
    self:hide()
    self:emitSignal("done", {})
  end
  return true
end

function FileStaging:upReleased(shifted)
  if not shifted then
    self:hide()
    self:emitSignal("done", {})
  end
  return true
end

function FileStaging:homeReleased()
  self:hide()
  self:emitSignal("done", {})
  return true
end

return FileStaging
