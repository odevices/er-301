local Message = require "Overlay.Message"
local FloatingMenu = require "Overlay.FloatingMenu"

local function clearAll()
  Message.closeAll()
  FloatingMenu.close()
end

return {
  startMainMessage = Message.startMainMessage,
  endMainMessage = Message.endMainMessage,
  flashMainMessage = Message.flashMainMessage,
  flashSubMessage = Message.flashSubMessage,
  startFloatingMenu = FloatingMenu.open,
  selectFloatingMenu = FloatingMenu.select,
  endFloatingMenu = FloatingMenu.close,
  clearAll = clearAll,
}
