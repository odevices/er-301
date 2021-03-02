local Signal = require "Signal"

local channels = {
  ["IN1"] = app.INPUT_IN1,
  ["IN2"] = app.INPUT_IN2,
  ["IN3"] = app.INPUT_IN3,
  ["IN4"] = app.INPUT_IN4
}

local gains = {
  [1] = app.MOD_BIPOLAR_2500,
  [2] = app.MOD_BIPOLAR_1250,
  [4] = app.MOD_BIPOLAR_0625
}

local function set(channel, gain, restart)
  local chId = channels[channel]
  if chId then
    local gId = gains[gain]
    if gId then
      -- app.logInfo("Preamp.set: %s to %s", channel, gain)
      app.Modulation_setChannelRange(chId, gId)
      if restart then
        app.Modulation_restart()
        Signal.emit("onPreampChanged")
      end
    else
      app.logInfo("Preamp.set: Invalid gain, %s.", gain)
    end
  else
    app.logInfo("Preamp.set: Invalid channel, %s.", channel)
  end
end

local function get(channel)
  local chId = channel and channels[channel]
  if chId then
    local gId = app.Modulation_getChannelRange(chId)
    for k, v in pairs(gains) do if v == gId then return k end end
  else
    app.logError("Preamp.get: Invalid channel, %s.", channel)
  end
end

local function serialize()
  return {
    ["IN1"] = get("IN1"),
    ["IN2"] = get("IN2"),
    ["IN3"] = get("IN3"),
    ["IN4"] = get("IN4")
  }
end

local function deserialize(t)
  for k, v in pairs(t) do set(k, v) end
  app.Modulation_restart()
  Signal.emit("onPreampChanged")
end

return {
  set = set,
  get = get,
  serialize = serialize,
  deserialize = deserialize
}
