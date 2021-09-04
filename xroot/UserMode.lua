local Signal = require "Signal"
local Mode = require "Base.Mode"
local Channels = require "Channels"

local mode = Mode("UserMode")

local prevState
local function refresh()
  if mode.active then
    local state = app.getModeToggleState()
    if state ~= prevState then
      prevState = state
      app.collectgarbage()
    end
    Channels.setViewMode(state)
    Channels.show()
  end
end

local function pushed(i)
  return app.Button_pressed(app.BUTTON_SELECT1 + i - 1)
end

local function showMessage(...)
  local SG = require "Overlay"
  SG.flashMainMessage(...)
end

local buttonHeld = false
local function selectPressed(i, shifted)
  -- app.logInfo("UserMode:selectPressed(%s,%s)",i,shifted)
  if shifted then
    Channels.toggleMute(i)
  else
    if not buttonHeld then
      buttonHeld = true
      Channels.select(i)
      refresh()
    else
      buttonHeld = false
      if i == 1 then
        if pushed(2) then
          if Channels.toggleLinkWithConfirmation(1) then
            refresh()
          else
            showMessage("Mono or Stereo only.")
          end
        else
          showMessage("Adjacent channels only.")
        end
      elseif i == 2 then
        if pushed(1) then
          if Channels.toggleLinkWithConfirmation(1) then
            refresh()
          else
            showMessage("Mono or Stereo only.")
          end
        elseif pushed(3) then
          if Channels.toggleLinkWithConfirmation(2) then
            refresh()
          else
            showMessage("Mono or Stereo only.")
          end
        else
          showMessage("Adjacent channels only.")
        end
      elseif i == 3 then
        if pushed(2) then
          if Channels.toggleLinkWithConfirmation(2) then
            refresh()
          else
            showMessage("Mono or Stereo only.")
          end
        elseif pushed(4) then
          if Channels.toggleLinkWithConfirmation(3) then
            refresh()
          else
            showMessage("Mono or Stereo only.")
          end
        else
          showMessage("Adjacent channels only.")
        end
      elseif i == 4 then
        if pushed(3) then
          if Channels.toggleLinkWithConfirmation(3) then
            refresh()
          else
            showMessage("Mono or Stereo only.")
          end
        else
          showMessage("Adjacent channels only.")
        end
      end
    end
  end
end

local function selectReleased(i, shifted)
  buttonHeld = false
  -- app.logInfo("UserMode:selectReleased(%s,%s)",i,shifted)
  if shifted then return end
end

---------------------

function mode:enter()
  Mode.enter(self)
  buttonHeld = false
  refresh()
  Signal.register("selectPressed", selectPressed)
  Signal.register("selectReleased", selectReleased)
  Signal.register("onModeChanged", refresh)
  Signal.register("channelsModified", refresh)
end

function mode:leave()
  Signal.remove("selectPressed", selectPressed)
  Signal.remove("selectReleased", selectReleased)
  Signal.remove("onModeChanged", refresh)
  Signal.remove("channelsModified", refresh)
  Mode.leave(self)
end

function mode:showRootChains()

end

return mode
