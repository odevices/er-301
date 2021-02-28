local app = app
local Signal = require "Signal"
local StateMachine = require "Base.StateMachine"
local State = require "Base.State"

local EncoderState = StateMachine()
local Coarse = State("Coarse")
local Fine = State("Fine")
local Neutral = State("Neutral")

local function linMap(min, max, superCoarse, coarse, fine, superFine)
  local map = app.LinearDialMap(min, max)
  map:setSteps(superCoarse, coarse, fine, superFine)
  return map
end

local function bilinearMap(min1, max1, n1, min2, max2, n2)
  local map = app.LUTDialMap(n1 + n2 + 2)

  local scale1 = (max1 - min1) / n1
  for i = 0, n1 do map:add(i * scale1 + min1) end

  local scale2 = (max2 - min2) / n2
  for i = 0, n2 do map:add(i * scale2 + min2) end
  return map
end

local function intMap(min, max)
  local map = app.LinearDialMap(min, max)
  map:setSteps(5, 1, 0.25, 0.25);
  map:setRounding(1)
  return map
end

local function feedbackMap()
  local map = app.LinearDialMap(-36, 0)
  map:setZero(-160)
  map:setSteps(6, 1, 0.1, 0.01);
  return map
end

local function decibelMap(from, to)
  local map = app.LinearDialMap(from, to)
  map:setZero(-160)
  map:setSteps(6, 1, 0.1, 0.01);
  return map
end

local function octaveMap(from, to, F0, step)
  local n = 0
  for x = from, to, step do n = n + 1 end
  local map = app.LUTDialMap(n)
  for x = from, to, step do map:add((2 ^ x) * F0) end
  return map
end

local function octaveMapWithZero(from, to, F0, step)
  local n = 0
  for x = from, to, step do n = n + 1 end
  local map = app.LUTDialMap(n + 1)
  map:add(0)
  for x = from, to, step do map:add((2 ^ x) * F0) end
  return map
end

local function timeOctaveMap(low, high)
  local x = low
  local n = 0
  while x < high do
    n = n + 1
    x = x * 2
  end

  local map = app.LUTDialMap(n)
  local x = low
  while x < high do
    map:add(x)
    x = x * 2
  end
  return map
end

local function timeFactorMap()
  local map = app.LUTDialMap(12)
  map:add(0.0625)
  map:add(0.125)
  map:add(0.25)
  map:add(0.5)
  map:add(0.75)
  map:add(1.0)
  map:add(1.25)
  map:add(1.5)
  map:add(2.0)
  map:add(4.0)
  map:add(8.0)
  map:add(16.0)
  map:setZero(1.0)
  return map
end

local function speedFactorMap()
  local map = app.LUTDialMap(25)
  map:add(-16.0)
  map:add(-8.0)
  map:add(-4.0)
  map:add(-2.0)
  map:add(-1.5)
  map:add(-1.25)
  map:add(-1.0)
  map:add(-0.75)
  map:add(-0.5)
  map:add(-0.25)
  map:add(-0.125)
  map:add(-0.0625)
  map:add(0)
  map:add(0.0625)
  map:add(0.125)
  map:add(0.25)
  map:add(0.5)
  map:add(0.75)
  map:add(1.0)
  map:add(1.25)
  map:add(1.5)
  map:add(2.0)
  map:add(4.0)
  map:add(8.0)
  map:add(16.0)
  return map
end

local function rateMap()
  local map = app.LUTDialMap(5)
  map:add(0)
  map:add(10)
  map:add(100)
  map:add(1000)
  map:add(10000)
  return map
end

local dialMaps = {}
dialMaps["default"] = linMap(-1, 1, 0.1, 0.01, 0.001, 0.0001)
dialMaps["[-1,1]"] = dialMaps["default"]
dialMaps["percent"] = linMap(0, 100, 10, 1, 0.1, 0.1)
dialMaps["[0,1]"] = linMap(0, 1, 0.1, 0.01, 0.001, 0.0001)
dialMaps["unit"] = dialMaps["[0,1]"]
dialMaps["speed"] = linMap(-3, 3, 1, 0.1, 0.010, 0.001)
dialMaps["cents"] = linMap(-3600, 3600, 1200, 100, 10, 1)
dialMaps["feedback"] = feedbackMap()
dialMaps["volume"] = decibelMap(-60, 12)
dialMaps["decibel36"] = decibelMap(-36, 36)
dialMaps["gain36dB"] = decibelMap(0, 36)
dialMaps["int[0,99]"] = intMap(0, 99)
dialMaps["int[0,100]"] = intMap(0, 100)
dialMaps["int[1,100]"] = intMap(1, 100)
dialMaps["int[1,256]"] = intMap(1, 256)
dialMaps["int[0,256]"] = intMap(0, 256)
dialMaps["int[1,32]"] = intMap(1, 32)
dialMaps["int[-32,32]"] = intMap(-32, 32)
dialMaps["int[-4,4]"] = intMap(-4, 4)
dialMaps["[1,32]"] = linMap(1, 32, 1, 0.1, 0.01, 0.001)
dialMaps["int[0,32]"] = intMap(0, 32)
dialMaps["[0,0.25]"] = linMap(0, 0.25, 0.05, 0.01, 0.001, 0.0001)
dialMaps["[0,2]"] = linMap(0, 2, 0.1, 0.01, 0.001, 0.0001)
dialMaps["[0,10]"] = linMap(0, 10, 1, 0.1, 0.01, 0.001)
dialMaps["[0,0.1]"] = linMap(0, 0.1, 0.05, 0.01, 0.001, 0.0001)
dialMaps["[0,36]"] = linMap(0, 36, 1, 0.1, 0.01, 0.001)
dialMaps["[-20,20]"] = linMap(-20, 20, 1, 0.1, 0.01, 0.001)
dialMaps["[-10,10]"] = linMap(-10, 10, 1, 0.1, 0.01, 0.001)
dialMaps["[-5,5]"] = linMap(-5, 5, 1, 0.1, 0.01, 0.001)
dialMaps["gain"] = dialMaps["[-10,10]"]
dialMaps["freqGain"] = linMap(-10000, 10000, 1000, 100, 10, 1)
dialMaps["bias"] = dialMaps["[-5,5]"]
dialMaps["clockFreq"] = octaveMapWithZero(-5, 5, 2, 1)
dialMaps["oscFreq"] = octaveMapWithZero(-10, 10, 27.5, 1.0 / 12)
dialMaps["filterFreq"] = octaveMapWithZero(-5, 5, 440, 1.0 / 12)
dialMaps["slewTimes"] = timeOctaveMap(0.003, 1000)
dialMaps["timeFactors"] = timeFactorMap()
dialMaps["speedFactors"] = speedFactorMap()
dialMaps["rate"] = rateMap()
dialMaps["ADSR"] = bilinearMap(0, 0.99, 99, 1, 10, 99)
dialMaps["tempo"] = linMap(1, 501, 10, 1, 0.1, 0.01)

local function dumpMap(name, map)
  app.logInfo("Map(%s)", name)
  app.logInfo("min,max: %s %s", map:min(), map:max())
  app.logInfo("radix: %s %s %s", map:coarseRadix(), map:fineRadix(),
         map:superFineRadix())
  if map.coarseStep then
    app.logInfo("steps: %s %s %s", map:coarseStep(), map:fineStep(),
           map:superFineStep())
  end
end

function Fine:enter(prevState, ...)
  State.enter(self, prevState, ...)
  app.Led_on(app.LED_DIAL1)
  app.Led_off(app.LED_DIAL2)
end

function Coarse:enter(prevState, ...)
  State.enter(self, prevState, ...)
  app.Led_off(app.LED_DIAL1)
  app.Led_on(app.LED_DIAL2)
end

function Neutral:enter(prevState, ...)
  State.enter(self, prevState, ...)
  app.Led_off(app.LED_DIAL1)
  app.Led_off(app.LED_DIAL2)
end

local function init()

end

local function getMap(name)
  return dialMaps[name] or dialMaps["default"]
end

local function set(state)
  EncoderState:switch(state or Neutral)
end

local function serialize(state)
  if state == Fine then
    return "Fine"
  elseif state == Coarse then
    return "Coarse"
  else
    return "Neutral"
  end
end

local function deserialize(data)
  if data == "Fine" then
    return Fine
  elseif data == "Coarse" then
    return Coarse
  else
    return Neutral
  end
end

return {
  init = init,
  set = set,
  getMap = getMap,
  serialize = serialize,
  deserialize = deserialize,
  Fine = Fine,
  Coarse = Coarse,
  Vertical = Fine,
  Horizontal = Coarse,
  Neutral = Neutral
}
