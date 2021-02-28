
local function run()
  local Busy = require "Busy"
  Busy.start()
  app.collectgarbage()
  Busy.stop()
end

return {
  {
    description = "Run garbage collector",
    batch = false,
    run = run,
    suppressReset = true
  }
}
