local Class = require "Base.Class"
local Object = require "Base.Object"
local Persist = require "Persist"
local Card = require "Card"
local Path = require "Path"
local Utils = require "Utils"

local Library = Class {}
Library:include(Object)

function Library:init()
  self:setClassName("UnitLibrary")
  self.unitHash = {}
  self.aliasHash = {}
end

function Library:find(key)
  return self.unitHash[key] or self.aliasHash[key] or
             self.aliasHash[key:gsub("/", ".")]
end

function Library:loadFromTable(t)
  local name = t.name
  local title = t.title or t.name

  self:setInstanceName(t.name)
  self.name = name
  self.aliases = t.aliases or {}
  self.title = title
  self.contact = t.contact
  self.keyword = t.keyword
  self.onUnload = t.onUnload
  self.external = t.external
  local unitList = {}
  self.unitList = unitList
  local unitHash = {}
  self.unitHash = unitHash
  local aliasHash = {}
  self.aliasHash = aliasHash
  local categoryHash = {}
  self.categoryHash = categoryHash
  local categoryList = {}
  self.categoryList = categoryList
  local category = t.category or name

  local unitCount = 0
  local unitFailed = 0
  local presetCount = 0
  local presetFailed = 0

  if t.units then
    for i, e in ipairs(t.units) do

      if e.moduleName and e.title then
        -- app.logInfo("%s: %s",self,e.moduleName)
        local u = {
          id = e.id or e.moduleName,
          title = e.title,
          moduleName = e.moduleName,
          libraryName = name,
          category = e.category or category,
          keywords = e.keywords,
          channelCount = e.channelCount,
          args = e.args
        }

        unitList[#unitList + 1] = u
        unitHash[u.id] = u

        if not categoryHash[u.category] then
          -- app.logInfo("%s: %s",self,category)
          categoryHash[u.category] = true
          categoryList[#categoryList + 1] = u.category
        end

        if e.aliases then
          for _, alias in ipairs(e.aliases) do aliasHash[alias] = u end
        end
      elseif e.category then
        -- category marker
        category = e.category
      else
        unitFailed = unitFailed + 1
      end
    end
    unitCount = #unitList
  end

  if t.presets then
    category = t.category or name
    for i, e in ipairs(t.presets) do

      if e.filename and e.title then
        local u = {
          id = e.id or e.filename,
          title = e.title,
          filename = e.filename,
          libraryName = name,
          category = e.category or category,
          isPreset = true
        }

        unitList[#unitList + 1] = u
        unitHash[u.id] = u

        if not categoryHash[u.category] then
          -- app.logInfo("%s: %s",self,category)
          categoryHash[u.category] = true
          categoryList[#categoryList + 1] = u.category
        end

        if e.aliases then
          for _, alias in ipairs(e.aliases) do aliasHash[alias] = u end
        end
      elseif e.category then
        -- category marker
        category = e.category
      else
        presetFailed = presetFailed + 1
      end
    end
    presetCount = #unitList - unitCount
  end

  app.logInfo("Parsed %s TOC: %d of %d units, %d of %d presets, %d categories",
              name, unitCount, unitCount + unitFailed, presetCount,
              presetCount + presetFailed, #categoryList)
  return true
end

function Library:loadFromFile(pathToFile)
  if not Card.mounted() or not Path.exists(pathToFile) then return end
  local t = Persist.readTable(pathToFile)
  if t then return self:loadFromTable(t) end
end

function Library:getUnits(t, category, channelCount)
  t = t or {}

  if category and self.categoryHash[category] == nil then
    -- unknown category
    return t
  end

  local unitList = self.unitList
  if category and channelCount then
    -- select on category and channelCount
    for _, u in ipairs(unitList) do
      if u.category == category and
          (u.channelCount == nil or u.channelCount == channelCount) then
        t[#t + 1] = u
      end
    end
  elseif category then
    -- select on just category
    for _, u in ipairs(unitList) do
      if u.category == category then t[#t + 1] = u end
    end
  elseif channelCount then
    -- select on just channelCount
    for _, u in ipairs(unitList) do
      if u.channelCount == nil or u.channelCount == channelCount then
        t[#t + 1] = u
      end
    end
  else
    -- return all units
    for _, u in ipairs(unitList) do t[#t + 1] = u end
  end

  return t
end

return Library
