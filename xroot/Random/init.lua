local Utils = require "Utils"
local Path = require "Path"
local FS = require "Card.FileSystem"

local rootPath = Path.join(FS.getRoot("front"), "namegen")

local defaultNounFile = app.roots.x.."/Random/Nouns.txt"
local userNounFile = rootPath .. "/nouns.txt"

local defaultAdjectiveFile = app.roots.x.."/Random/Adjectives.txt"
local userAdjectiveFile = rootPath .. "/adjectives.txt"

local userSuppressFile = rootPath .. "/suppress.txt"

local loaded = false
local nouns = {}
local adjectives = {}
local suppressed = {}

local function loadHelper(filename, t)
  local count = 0
  for line in io.lines(filename) do
    line = Utils.trim(line)
    if string.len(line) > 0 then
      line = string.lower(line)
      line = Utils.firstToUpper(line)
      table.insert(t, line)
      count = count + 1
    end
  end
  return count
end

local function load()
  local nounFile
  local adjectiveFile

  if Path.exists(userNounFile) then
    nounFile = userNounFile
  else
    nounFile = defaultNounFile
  end

  if Path.exists(userAdjectiveFile) then
    adjectiveFile = userAdjectiveFile
  else
    adjectiveFile = defaultAdjectiveFile
  end

  if loadHelper(nounFile, nouns) == 0 then loadHelper(defaultNounFile, nouns) end

  if loadHelper(adjectiveFile, adjectives) == 0 then
    loadHelper(defaultAdjectiveFile, adjectives)
  end

  if Path.exists(userSuppressFile) then
    for line in io.lines(userSuppressFile) do
      line = Utils.trim(line)
      if string.len(line) > 0 then
        line = string.lower(line)
        suppressed[line] = true
      end
    end
  end

  math.randomseed(app.Rng_read32())
  loaded = true
end

local function generateUnsafeName()
  return adjectives[math.random(#adjectives)] .. " " ..
             nouns[math.random(#nouns)]
end

local function generateRobotName(prefix)
  return string.format("%04x %04x %04x", math.random(256 * 256),
                       math.random(256 * 256), math.random(256 * 256))
end

local function generateName(mode)
  if mode == "poet" then
    if not loaded then load() end
    local retries = 10
    local name = generateUnsafeName()
    while suppressed[name:lower()] do
      name = generateUnsafeName()
      retries = retries - 1
      if retries == 0 then return generateRobotName() end
    end
    return name
  else
    return generateRobotName()
  end
end

local function onCardMounted()
  loaded = false
  nouns = {}
  adjectives = {}
  suppressed = {}
end

local function init()
  local Signal = require "Signal"
  local Card = require "Card"
  Signal.register("cardMounted", onCardMounted)
  if Card.mounted() then onCardMounted() end
end

return {
  init = init,
  generateName = generateName
}
