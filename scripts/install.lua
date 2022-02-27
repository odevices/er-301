local function installFile(self, archive, filename)
  self:msg("Installing " .. filename)
  if not archive:exists(filename) then
    self:msg("Not found " .. filename)
    return false
  end
  local path = "0:/" .. filename
  if not archive:extract(filename, path) then
    self:msg("Update failed: could not copy " .. filename)
    return false
  end
  return true
end

local function install(self, archive)
  local failed = 0

  if not installFile(self, archive, "MLO") then
    failed = failed + 1
  end
  if not installFile(self, archive, "SBL") then
    failed = failed + 1
  end
  if not installFile(self, archive, "kernel.bin") then
    failed = failed + 1
  end
  if failed == 0 then
    if not installFile(self, archive, "core-FIRMWARE_VERSION.pkg") then
      failed = failed + 1
    end
    if not installFile(self, archive, "teletype-FIRMWARE_VERSION.pkg") then
      failed = failed + 1
    end
  end
  return failed == 0
end

return {
  install = install
}
