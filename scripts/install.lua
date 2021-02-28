function install(self, archive)
  local Path = require "Path"
  local files = {
    "MLO",
    "SBL",
    "kernel.bin"
  }

  local descriptions = {
    "initial boot loader",
    "secondary boot loader",
    "kernel"
  }

  for i, filename in ipairs(files) do
    app.logInfo("Trying to extract and copy %s.", filename)
    self:msg("Installing %s...", descriptions[i])
    if not archive:exists(filename) then
      self:msg("Update failed: %s was not found.", filename)
      return false
    end
    if archive:extract(filename, Path.join(app.roots.rear, filename)) then
      app.logInfo("%s copied successfully.", filename)
    else
      self:msg("Update failed: could not copy %s.", filename)
      return false
    end
  end

  return true
end

return {
  install = install
}
