local app = app
local Signal = require "Signal"
local StateMachine = require "Base.StateMachine"
local State = require "Base.State"
local drive = 1
local drivePath = app.roots.front
local totalSizeMB = 0
local claims = {}
local claimCount = 0

local CardState = StateMachine()
local EjectingState = State("Ejecting")
local EjectedState = State("Ejected")
local MountedState = State("Mounted")
local FailedState = State("Failed")

local RemoteEjectingState = State("Remote Ejecting")
local RemoteMountedState = State("Remote Mounted")

local function mount(drive)
  return app.Card_mount(drive)
end

local function unmount(drive)
  return app.Card_unmount(drive)
end

function EjectingState:enter(prevState, ...)
  if prevState == MountedState then
    app.logInfo("Card ejecting.")
    Signal.emit("cardEjecting")
  end
  State.enter(self, prevState, ...)
  if claimCount == 0 then CardState:switch(EjectedState) end
end

function RemoteEjectingState:enter(prevState, ...)
  if prevState == RemoteMountedState then
    app.logInfo("USB host is ejecting card.")
    Signal.emit("cardRemoteEjecting")
  end
  State.enter(self, prevState, ...)
  -- Automatically switch to ejected state.
  CardState:switch(EjectedState)
end

function EjectedState:enter(prevState, ...)
  if prevState == EjectingState then
    -- all files should be flushed now
    unmount(drive)
    totalSizeMB = 0
    app.logInfo("Card ejected.")
  elseif prevState == RemoteEjectingState then
    app.logInfo("Card ejected by USB host.")
  end
  app.Led_on(app.LED_SAFE)
  State.enter(self, prevState, ...)
  Signal.emit("cardEjected")
end

function EjectedState:leave(...)
  app.Led_off(app.LED_SAFE)
  State.leave(self, ...)
end

function MountedState:enter(prevState, ...)
  totalSizeMB = app.diskTotalSpaceMB(drivePath)
  local freeSizeMB = app.diskFreeSpaceMB(drivePath)
  app.logInfo("Card mounted. Total=%dMB Free=%dMB", totalSizeMB, freeSizeMB)
  local FS = require "Card.FileSystem"
  FS.init()
  Signal.emit("cardMounted", totalSizeMB, freeSizeMB)
  State.enter(self, prevState, ...)
end

function RemoteMountedState:enter(prevState, ...)
  app.logInfo("Card mounted over USB.")
  Signal.emit("cardRemoteMounted")
  State.enter(self, prevState, ...)
end

function FailedState:enter(prevState, ...)
  totalSizeMB = 0
  app.logInfo("Card failed to mount.")
  Signal.emit("cardFailed")
  State.enter(self, prevState, ...)
end

local function onStorageChanged()
  local state = CardState:current()
  local toggle = app.getStorageToggleState()

  if state == MountedState then
    if toggle == "eject" then CardState:switch(EjectingState) end
  elseif state == RemoteMountedState then
  elseif state == EjectedState then
    if toggle == "eject" then
    else
      -- try to mount locally
      if mount(drive) then
        CardState:switch(MountedState)
      else
        CardState:switch(FailedState)
      end
    end
  elseif state == EjectingState then
    if toggle ~= "eject" then CardState:switch(MountedState) end
  elseif state == RemoteEjectingState then
  elseif state == FailedState then
    if toggle == "eject" then
      CardState:switch(EjectedState)
    else
      if mount(drive) then
        CardState:switch(MountedState)
      else
        CardState:switch(FailedState)
      end
    end
  end
end

local function onUSBConnect()
end

local function onUSBDisconnect()
end

local function onUSBFrontCardMount()
  CardState:switch(RemoteMountedState)
end

local function onUSBFrontCardUnmount()
  CardState:switch(RemoteEjectingState)
end

local function onUSBError()
end

-- public interface

local function init()
  Signal.register("onStorageChanged", onStorageChanged)
  Signal.register("onUSBConnect", onUSBConnect)
  Signal.register("onUSBDisconnect", onUSBDisconnect)
  Signal.register("onUSBFrontCardMount", onUSBFrontCardMount)
  Signal.register("onUSBFrontCardUnmount", onUSBFrontCardUnmount)
  Signal.register("onUSBError", onUSBError)
  CardState:switch(EjectedState)
  onStorageChanged()
end

local function shutdown()
  Signal.remove("onStorageChanged", onStorageChanged)
  CardState:switch(EjectedState)
end

local function mounted()
  local state = CardState:current()
  return state == MountedState or state == EjectingState
end

local function remoteMounted()
  local state = CardState:current()
  return state == RemoteMountedState or state == RemoteEjectingState
end

local function failed()
  local state = CardState:current()
  return state == FailedState
end

local function ejecting()
  local state = CardState:current()
  return state == EjectingState
end

local function remoteEjecting()
  local state = CardState:current()
  return state == RemoteEjectingState
end

local function sizeMB()
  return totalSizeMB
end

local function freeMB()
  if mounted() then
    return app.diskFreeSpaceMB(drivePath)
  else
    return 0
  end
end

local function claim(who, filename)
  if not claims[filename] then
    claims[filename] = who
    claimCount = claimCount + 1
    Signal.emit("fileClaimed", who, filename)
  end
end

local function release(filename)
  if claims[filename] then
    claims[filename] = nil
    claimCount = claimCount - 1
    Signal.emit("fileReleased", filename)
    if claimCount == 0 and CardState:current() == EjectingState then
      -- CardState:switch(EjectedState)
      -- Since the calling context might be inside a signal...
      local Application = require "Application"
      Application.postEvent(CardState, "switch", EjectedState)
    end
  end
end

local function format()
  local state = CardState:current()
  if state == MountedState or state == FailedState then
    local Busy = require "Busy"
    Busy.start("Formatting...")
    local retval = app.Card_format(drive)
    Busy.stop()
    CardState:switch(EjectedState)
    if retval then
      CardState:switch(MountedState)
    else
      CardState:switch(FailedState)
    end
    return retval
  else
    return false
  end
end

local function getClaimedFiles()
  return claims
end

local function getClaimCount()
  return claimCount
end

local function busy()
  return claimCount > 0 or remoteMounted()
end

local function forceEject()
  claims = {}
  claimCount = 0
  if mounted() then unmount(drive) end
end

return {
  init = init,
  shutdown = shutdown,
  mounted = mounted,
  remoteMounted = remoteMounted,
  failed = failed,
  sizeMB = sizeMB,
  freeMB = freeMB,
  ejecting = ejecting,
  remoteEjecting = remoteEjecting,
  claim = claim,
  release = release,
  format = format,
  getClaimedFiles = getClaimedFiles,
  getClaimCount = getClaimCount,
  busy = busy,
  forceEject = forceEject
}
