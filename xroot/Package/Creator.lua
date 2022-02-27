local Class = require "Base.Class"
local Base = require "Base.Observable"
local FileChooser = require "Card.FileChooser"
local FileStaging = require "Card.FileStaging"
local FileSystem = require "Card.FileSystem"
local Path = require "Path"

local Creator = Class {}
Creator:include(Base)

function Creator:init()
  Base.init(self)
  self:setClassName("Package.Creator")
end

function Creator:show()
  local Card = require "Card"
  if Card.mounted() then
    self:askForSources()
  else
    local Message = require "Message"
    local dialog = Message.Main("SD card is not mounted.")
    dialog:show()
  end
end

local function quote(text)
  return string.format('"%s"', text)
end

function Creator:doneChoosing(srcPaths, dstPath, author)
  app.logInfo("Packaging %d presets to %s", #srcPaths, dstPath)
  local Writer = require "Package.Writer"
  local Busy = require "Busy"
  local Serialization = require "Persist.Serialization"
  local dstStem = Path.getStem(dstPath)
  local dstFilename = Path.getFilename(dstPath)
  local writer = Writer()
  Busy.start("Creating package...")
  if writer:open(dstPath) then
    local tmpPath = Path.join(FileSystem.getRoot("tmp"), "toc.lua")
    local toc = io.open(tmpPath, "w")
    toc:write("return {\n")
    toc:write("  name = ", quote(dstStem), ",\n")
    toc:write("  author = ", quote(author), ",\n")
    toc:write("  presets = {\n")
    for _, fullPath in ipairs(srcPaths) do
      local filename = Path.getFilename(fullPath)
      Busy.status("Packaging %s", filename)
      local data = Serialization.readTable(fullPath)
      toc:write("    {\n")
      if data.hasUserTitle then
        toc:write("      title = ", quote(data.title), ",\n")
      else
        toc:write("      title = ", quote(Path.getStem(fullPath)), ",\n")
      end
      toc:write("      filename = ", quote(filename), ",\n")
      toc:write("    },\n")
      if not writer:addFile(fullPath, filename) then
        Busy.error("Failed to add %s", filename)
      end
    end
    toc:write("  }\n")
    toc:write("}\n")
    toc:close()
    if not writer:addFile(tmpPath, "toc.lua") then
      Busy.error("Failed to add toc.lua")
    end
    app.deleteFile(tmpPath)
    writer:close()
    self:emitSignal("done", true)
  else
    Busy.error("Failed to create %s", dstFilename)
  end
  Busy.stop()
  local history = require "LogHistory"
  if history:hasErrors() then
    history:showErrors("%s had problems.", dstFilename)
    self:emitSignal("done")
  else
    local Overlay = require "Overlay"
    Overlay.flashMainMessage("%s created.", dstFilename)
  end
end

function Creator:askForAuthor(srcPaths, dstPath)
  local task = function(text)
    if text then
      self:doneChoosing(srcPaths, dstPath, text)
    else
      self:emitSignal("done")
    end
  end
  local Keyboard = require "Keyboard"
  local kb = Keyboard("Who is the author?", "", true, "Package/Creator/Author")
  kb:subscribe("done", task)
  kb:show()
end

function Creator:askForDestination(srcPaths)
  local task = function(result)
    if result and result.fullpath and
        FileSystem.checkPath("package", "w", result.fullpath) then
      self:askForAuthor(srcPaths, result.fullpath)
    else
      self:emitSignal("done")
    end
  end
  local chooser = FileChooser {
    msg = string.format("Save Package (*.%s)", FileSystem.getExt("package")),
    goal = "save file",
    path = app.roots.front,
    ext = "." .. FileSystem.getExt("package"),
    pattern = FileSystem.getPattern("package"),
    history = "Package/Creator/destination"
  }
  chooser:subscribe("done", task)
  chooser:show()
end

function Creator:checkSources(srcPaths)
  local failed = 0
  for _, fullPath in ipairs(srcPaths) do
    local filename = Path.getFilename(fullPath)
    local ext = Path.getExtension(filename)
    if not FileSystem.isValidExtension("packageable-presets", ext) then
      app.logWarn("%s: not a valid package source.", fullPath)
      failed = failed + 1
    end
  end
  if failed > 0 then
    local Message = require "Message"
    local dialog = Message.Main("%d of %d selected cannot be packaged.", failed,
                                #srcPaths)
    dialog:show()
    self:emitSignal("done")
  else
    self:askForDestination(srcPaths)
  end
end

function Creator:doStaging(sofar, morePaths)
  local doneTask = function(paths)
    if paths then
      self:checkSources(paths)
    else
      self:emitSignal("done")
    end
  end
  local moreTask = function(paths)
    self:askForSources(paths)
  end
  local staging = FileStaging(sofar)
  if morePaths then
    staging:addPaths(morePaths)
  end
  staging:subscribe("done", doneTask)
  staging:subscribe("more", moreTask)
  staging:show()
end

function Creator:askForSources(sofar)
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
        self:checkSources({
          results.fullpath
        })
      end
    elseif sofar then
      -- canceled but still have files in staging
      self:doStaging(sofar)
    end
  end
  local chooser = FileChooser {
    msg = "Choose Preset(s)",
    goal = "load files",
    path = app.roots.front,
    pattern = FileSystem.getPattern("packageable-presets"),
    history = "Package/Creator/sources",
    staged = sofar and #sofar
  }
  chooser:subscribe("done", task)
  chooser:show()
end

return Creator
