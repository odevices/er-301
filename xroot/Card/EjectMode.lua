local Context = require "Base.Context"
local Mode = require "Base.Mode"
local CardStatusViewer = require "Card.StatusViewer"
local CardConsole = require "Card.Console"

-- define the state
local mode = Mode("Eject")
local window = CardStatusViewer()
local context = Context("Eject", window)

function mode:enter()
  Mode.enter(self)
  local Application = require "Application"
  Application.setVisibleContext(context)
  if CardConsole.context then
    Application.setVisibleContext(CardConsole.context)
  else
    Application.setVisibleContext(context)
  end
end

return mode
