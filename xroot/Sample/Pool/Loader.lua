local Class = require "Base.Class"
local Object = require "Base.Object"
local FileChooser = require "Card.FileChooser"
local FileStaging = require "Card.FileStaging"
local FileSystem = require "Card.FileSystem"
local Path = require "Path"

local Loader = Class {}
Loader:include(Object)

function Loader:init(history, after, loop)
  self:setClassName("Sample.Pool.Loader")
  local Card = require "Card"
  if Card.mounted() then
    self.history = history
    self.after = after
    self.loop = loop
    self:askForSamples()
  else
    local Message = require "Message"
    local dialog = Message.Main("SD card is not mounted.")
    dialog:show()
  end
end

function Loader:doneChoosing(paths)
  -- The paths argument is never nil.
  if self.after then
    self.after(paths)
  end
end

function Loader:doStaging(sofar, morePaths)
  local doneTask = function(paths)
    -- The paths argument is never nil.
    self:doneChoosing(paths)
  end
  local moreTask = function(paths)
    self:askForSamples(paths)
  end
  local staging = FileStaging(sofar)
  if morePaths then
    table.sort(morePaths)
    staging:addPaths(morePaths)
  end
  staging:subscribe("done", doneTask)
  staging:subscribe("more", moreTask)
  staging:show()
end

function Loader:askForSamples(sofar)
  local task = function(results)
    if results then
      if results.multi then
        self:doStaging(sofar, results.paths)
      elseif sofar then
        self:doStaging(sofar, {
          results.fullpath
        })
      else
        -- Skip staging for just one path.
        self:doneChoosing({
          results.fullpath
        })
      end
    elseif sofar then
      -- Adding more samples was canceled but we have files in staging,
      -- so return to staging.
      self:doStaging(sofar)
    end
  end

  local Channels = require "Channels"
  local chooser = FileChooser {
    msg = "Choose Sample(s)",
    goal = "load files",
    path = app.roots.front,
    pattern = FileSystem.getPattern("audio"),
    history = self.history,
    previewLoop = self.loop,
    auditionChannel = Channels.selected(),
    staged = sofar and #sofar
  }
  chooser:subscribe("done", task)
  chooser:show()
end

return Loader
