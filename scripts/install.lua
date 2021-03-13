local function installFile(self, archive, filename)
  self:msg("Installing %s...", filename)
  if not archive:exists(filename) then
    self:msg("Update failed: %s was not found.", filename)
    return false
  end
  local path = "0:/" .. filename
  if not archive:extract(filename, path) then
    self:msg("Update failed: could not copy %s.", filename)
    return false
  end
  return true
end

local function install(self, archive)
  local failed = 0
  -- First try to install MLO, SBL, kernel.bin by name.
  if not installFile(self, archive, "kernel.bin") then failed = failed + 1 end
  if not installFile(self, archive, "SBL") then failed = failed + 1 end
  if not installFile(self, archive, "MLO") then failed = failed + 1 end

  -- Next install any other files found in the archive.
  local skip = {}
  skip["kernel.bin"] = true
  skip["MLO"] = true
  skip["SBL"] = true
  skip["install.lua"] = true

  local n = archive:getFileCount()
  for i = 1, n do
    local filename = archive:getFilename(i - 1)
    if not skip[filename] then
      if not installFile(self, archive, filename) then failed = failed + 1 end
    end
  end

  return failed == 0
end

return {
  install = install
}
