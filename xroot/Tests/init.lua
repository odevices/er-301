local app = app
local allTests = {}

local function addTest(moduleName)
  local module = require("Tests." .. moduleName)
  if module.run and module.description then
    allTests[#allTests + 1] = module
  else
    for _, test in ipairs(module) do
      if test.run and test.description then allTests[#allTests + 1] = test end
    end
  end
end

local function init()
  addTest("GarbageCollect")
  addTest("FakeCrash")
  -- addTest("CustomUnitTest")
  addTest("LoadAllUnits")
  addTest("LoadCoreUnits")
  addTest("RestartAudio")
end

local function reset()
  app.logInfo("RESET")
  local Application = require "Application"
  Application.resetToInitialState()
end

local function run(i)
  local test = allTests[i]
  if test then
    if not test.suppressReset then reset() end
    app.logInfo("START: %s", test.description)
    test.run()
    app.logInfo("DONE: %s", test.description)
    if not test.suppressReset then reset() end
  end
end

local function runAll()
  for _, test in ipairs(allTests) do
    if test.batch then
      reset()
      app.logInfo("START: %s", test.description)
      test.run()
      app.logInfo("DONE: %s", test.description)
    end
  end
  reset()
end

local function printSystemState()
  local memUnit = 1024
  local ramMax = app.Heap_getSize(memUnit)
  local ramUsed = ramMax - app.Heap_getFreeSize(memUnit)
  local ramLua = math.ceil(app.luaMemoryUsage())
  local audioLoad = app.Audio_getLoad()

  app.logInfo("RAM: %dKB (lua %dKB) CPU: %d%%", ramUsed, ramLua, audioLoad)
end

return {
  init = init,
  run = run,
  runAll = runAll,
  getAll = function()
    return allTests
  end,
  printSystemState = printSystemState
}
