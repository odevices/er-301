local app = app
local Signal = require "Signal"
local ChannelGroup = require "Channels.Group"
local Busy = require "Busy"
local Overlay = require "Overlay"

local links
local c2g
local groups
local current

local function init()
  current = 1
  links = {
    false,
    false,
    false
  }
  c2g = {}
  c2g[1] = ChannelGroup(1)
  c2g[2] = ChannelGroup(2)
  c2g[3] = ChannelGroup(3)
  c2g[4] = ChannelGroup(4)
  groups = {
    c2g[1],
    c2g[2],
    c2g[3],
    c2g[4]
  }
end

local function update()

  if links[1] and c2g[1] ~= c2g[2] then
    Busy.start("Linking OUT1 + OUT2")
    c2g[1]:destroy()
    c2g[2]:destroy()
    local group = ChannelGroup(1, 2)
    c2g[1] = group
    c2g[2] = group
    Busy.stop()
    Overlay.flashMainMessage("Linked OUT1 + OUT2")
  elseif not links[1] and c2g[1] == c2g[2] then
    Busy.start("Unlinking OUT1+2")
    c2g[1]:destroy()
    c2g[2]:destroy()
    c2g[1] = ChannelGroup(1)
    c2g[2] = ChannelGroup(2)
    Busy.stop()
    Overlay.flashMainMessage("Unlinked OUT1+2")
  end

  if links[2] and c2g[2] ~= c2g[3] then
    Busy.start("Linking OUT2 + OUT3")
    c2g[2]:destroy()
    c2g[3]:destroy()
    local group = ChannelGroup(2, 3)
    c2g[2] = group
    c2g[3] = group
    Busy.stop()
    Overlay.flashMainMessage("Linked OUT2 + OUT3")
  elseif not links[2] and c2g[2] == c2g[3] then
    Busy.start("Unlinking OUT2+3")
    c2g[2]:destroy()
    c2g[3]:destroy()
    c2g[2] = ChannelGroup(2)
    c2g[3] = ChannelGroup(3)
    Busy.stop()
    Overlay.flashMainMessage("Unlinked OUT2+3")
  end

  if links[3] and c2g[3] ~= c2g[4] then
    Busy.start("Linking OUT3 + OUT4")
    c2g[3]:destroy()
    c2g[4]:destroy()
    local group = ChannelGroup(3, 4)
    c2g[3] = group
    c2g[4] = group
    Busy.stop()
    Overlay.flashMainMessage("Linked OUT3 + OUT4")
  elseif not links[3] and c2g[3] == c2g[4] then
    Busy.start("Unlinking OUT3+4")
    c2g[3]:destroy()
    c2g[4]:destroy()
    c2g[3] = ChannelGroup(3)
    c2g[4] = ChannelGroup(4)
    Busy.stop()
    Overlay.flashMainMessage("Unlinked OUT3 + OUT4")
  end

  groups = {
    c2g[1]
  }
  if c2g[2] ~= c2g[1] then groups[#groups + 1] = c2g[2] end
  if c2g[3] ~= c2g[2] then groups[#groups + 1] = c2g[3] end
  if c2g[4] ~= c2g[3] then groups[#groups + 1] = c2g[4] end

  app.collectgarbage()
end

local function checkLink(i)
  if i == 1 and links[2] then
    return false
  elseif i == 2 and (links[1] or links[3]) then
    return false
  elseif i == 3 and links[2] then
    return false
  else
    return true
  end
end

local function getChain(channel)
  return c2g[channel].chain
end

local function link(i, continued)
  if not checkLink(i) then return false end
  links[i] = true
  if not continued then
    update()
    Signal.emit("channelsModified")
  end
  return true
end

local function unlink(i, continued)
  links[i] = false
  if not continued then
    update()
    Signal.emit("channelsModified")
  end
  return true
end

local function unlinkAndKeep(i)
  local chain = getChain(i)
  local data = chain:serialize()
  unlink(i)
  local left = getChain(i)
  left:deserialize(data)
  -- Duplicate contents to the bottom chain. (Disabled)
  -- local right = getChain(i+1)
  -- right:deserialize(data)
  return true
end

local function linkAndKeep(i)
  local left = getChain(i)
  local data = left:serialize()
  if link(i) then
    local chain = getChain(i)
    chain:deserialize(data)
    return true
  else
    return false
  end
end

local function toggleLinkWithConfirmation(i)
  if links[i] then
    local chain = getChain(i)
    if chain:length() > 0 then
      local Verification = require "Verification"
      local msg = string.format("%s contains units.", chain.title)
      local dialog = Verification.Main(msg, "Are you sure you want to unlink?",
                                       12, 10)
      local task = function(ans)
        if ans then unlinkAndKeep(i) end
      end
      dialog:subscribe("done", task)
      dialog:show()
      return true
    else
      return unlink(i)
    end
  elseif checkLink(i) then
    local left = getChain(i)
    local right = getChain(i + 1)
    if left:length() > 0 or right:length() > 0 then
      local Verification = require "Verification"
      local msg
      if left:length() == 0 then
        msg = string.format("%s contains units.", right.title)
      elseif right:length() == 0 then
        msg = string.format("%s contains units.", left.title)
      else
        msg = string.format("%s and %s contain units.", left.title, right.title)
      end
      local dialog = Verification.Main(msg, "Are you sure you want to link?",
                                       12, 10)
      local task = function(ans)
        if ans then linkAndKeep(i) end
      end
      dialog:subscribe("done", task)
      dialog:show()
      return true
    else
      return link(i)
    end
  else
    return false
  end
end

local function toggleLink(i, continued)
  if links[i] then
    return unlink(i, continued)
  else
    return link(i, continued)
  end
end

local function isMono(i)
  i = i or current
  if i == 1 then
    return not links[1]
  elseif i == 2 then
    return not (links[1] or links[2])
  elseif i == 3 then
    return not (links[2] or links[3])
  else
    return not links[3]
  end
end

local function isRight(i)
  i = i or current
  if i == 1 then
    return false
  elseif i == 2 and links[1] then
    return true
  elseif i == 2 and links[2] then
    return false
  elseif i == 3 and links[2] then
    return true
  elseif i == 3 and links[3] then
    return false
  elseif i == 4 and links[3] then
    return true
  else
    return false
  end
end

local function getSide(i)
  if isRight(i or current) then
    return 2
  else
    return 1
  end
end

local function generic(method, channel, ...)
  if channel then
    local g = c2g[channel]
    g[method](g, ...)
  else
    for i, g in ipairs(groups) do g[method](g, ...) end
  end
end

local function start(channel)
  generic("start", channel)
end

local function stop(channel)
  generic("stop", channel)
end

local function mute(channel)
  generic("mute", channel)
end

local function unmute(channel)
  generic("unmute", channel)
end

local function toggleMute(channel)
  generic("toggleMute", channel)
end

local function clear(channel)
  generic("clear", channel)
end

local function setViewMode(s)
  generic("setMode", nil, s)
end

local function show()
  c2g[current]:show()

  app.ChannelLEDs_off(0)
  app.ChannelLEDs_off(1)
  app.ChannelLEDs_off(2)
  app.ChannelLEDs_off(3)
  app.ChannelLEDs_on(current - 1)

  if links[1] then
    app.Led_on(app.LED_LINK12)
  else
    app.Led_off(app.LED_LINK12)
  end

  if links[2] then
    app.Led_on(app.LED_LINK23)
  else
    app.Led_off(app.LED_LINK23)
  end

  if links[3] then
    app.Led_on(app.LED_LINK34)
  else
    app.Led_off(app.LED_LINK34)
  end
end

local function select(channel)
  current = channel
end

local function selected()
  return current
end

local function serialize()
  local linkData = {
    link12 = links[1],
    link23 = links[2],
    link34 = links[3]
  }

  local chainData = {}
  for i, g in ipairs(groups) do chainData[i] = g:serialize() end

  return {
    links = linkData,
    chains = chainData
  }
end

local function deserializeLegacy(linkData, chainData)
  links = {
    false,
    false,
    false
  }
  for _, i in ipairs(linkData) do links[i] = true end

  update()
  stop()
  for i, data in ipairs(chainData) do
    if data then c2g[i].chain:deserialize(data) end
  end
  start()
  unmute()
  Signal.emit("channelsModified")
end

local function deserialize(t)
  if t.links then
    links = {
      t.links.link12 == true,
      t.links.link23 == true,
      t.links.link34 == true
    }
  else
    links = {
      false,
      false,
      false
    }
  end

  update()

  local unmute = {}
  for i = 1, #groups do unmute[i] = true end

  if t.chains then
    stop()
    for i, data in ipairs(t.chains) do
      groups[i]:deserialize(data)
      if data.isMuted then unmute[i] = false end
    end
    start()
  end

  for i, flag in ipairs(unmute) do if flag then groups[i]:unmute() end end

  Signal.emit("channelsModified")
end

return {
  init = init,
  checkLink,
  checkLink,
  link = link,
  unlink = unlink,
  toggleLink = toggleLink,
  toggleLinkWithConfirmation = toggleLinkWithConfirmation,
  serialize = serialize,
  deserialize = deserialize,
  deserializeLegacy = deserializeLegacy,
  isMono = isMono,
  isRight = isRight,
  getSide = getSide,
  start = start,
  stop = stop,
  mute = mute,
  unmute = unmute,
  toggleMute = toggleMute,
  clear = clear,
  setViewMode = setViewMode,
  show = show,
  select = select,
  selected = selected,
  getChain = getChain
}
