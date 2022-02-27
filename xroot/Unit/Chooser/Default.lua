local Env = require "Env"
local MondrianMenu = require "MondrianMenu.AsWindow"
local MondrianControl = require "MondrianMenu.Control"
local Class = require "Base.Class"
local Factory = require "Unit.Factory"
local Overlay = require "Overlay"
local Clipboard = require "Chain.Clipboard"
local Utils = require "Utils"
local ply = app.SECTION_PLY

local topCategories = {
  "Essentials",
  "Containers",
  "Sample Playback",
  "Granular Playback",
  "Recording and Looping",
  "Delays and Reverb",
  "Filtering",
  "Oscillators",
  "Noise",
  "Envelopes",
  "Mapping and Control",
  "Timing",
  "Measurement and Conversion"
}

local bottomCategories = {
  "Experimental"
}

local function isStandardUnit(loadInfo)
  return loadInfo.libraryName == "builtins" or loadInfo.libraryName == "core"
end

local ClipboardItem = Class {}
ClipboardItem:include(MondrianControl)

function ClipboardItem:init()
  MondrianControl.init(self)
  local graphic = app.FittedTextBox("Paste " .. Clipboard.describe(1))
  self:setControlGraphic(graphic)
end

function ClipboardItem:onReleased(i, shifted)
  self:callUp("choose", "paste")
end

------

local ChooserItem = Class {}
ChooserItem:include(MondrianControl)

function ChooserItem:init(loadInfo, suppressLibraryName)
  MondrianControl.init(self)
  local graphic
  if isStandardUnit(loadInfo) then
    graphic = app.FittedTextBox(loadInfo.title)
    graphic:setCornerRadius(0, 0, 0, 0)
    graphic:setBorderColor(app.GRAY10)
  elseif suppressLibraryName then
    graphic = app.FittedTextBox(loadInfo.title)
    graphic:setCornerRadius(0, 5, 5, 0)
  else
    local x = Utils.shorten(loadInfo.libraryName, 40)
    graphic = app.FittedTextBox(x .. ": " .. loadInfo.title)
    graphic:setCornerRadius(0, 5, 5, 0)
  end
  self:setControlGraphic(graphic)
  self.loadInfo = loadInfo
end

function ChooserItem:onReleased(i, shifted)
  self:callUp("choose", self.loadInfo)
end

------

local Chooser = Class {
  recent = {}
}
Chooser:include(MondrianMenu)

local function allFromSameExternalLibrary(units)
  local name
  for _, loadInfo in ipairs(units) do
    if isStandardUnit(loadInfo) then
      return false
    elseif name == nil then
      name = loadInfo.libraryName
    elseif name ~= loadInfo.libraryName then
      return false
    end
  end
  return true
end

function Chooser:init(ring, ordering)
  MondrianMenu.init(self)
  self:setClassName("Unit.Chooser.Default")
  self.ring = ring
  self.ordering = ordering
  self:refresh()
end

function Chooser:addUnitsInCategory(category)
  local units = Factory.getUnits(category, self.ring:getChannelCount())
  if units and #units > 0 then
    if allFromSameExternalLibrary(units) then
      if category == units[1].libraryName then
        self:addCategory(units[1].libraryName)
      else
        self:addCategory(units[1].libraryName .. " : " .. category)
      end
      for _, loadInfo in ipairs(units) do
        self:addUnit(loadInfo, true)
      end
    else
      self:addCategory(category)
      for _, loadInfo in ipairs(units) do
        self:addUnit(loadInfo)
      end
    end
  end
end

function Chooser:refresh()
  local ordering = self.ordering
  local ring = self.ring
  local row = self.mlist:getCurrentRow()
  self:clear()

  if ordering == "category" then
    if #Chooser.recent > 0 then
      self:addCategory("Recent:")
      for _, u in ipairs(Chooser.recent) do
        if u.channelCount then
          if ring.chain and ring.chain.channelCount == u.channelCount then
            self:addUnit(u)
          end
        else
          self:addUnit(u)
        end
      end
      if Clipboard.hasData(1) then
        self:addClipboard()
      end
    end

    local added = {}
    local addLast = {}
    for _, category in ipairs(bottomCategories) do
      addLast[category] = true
    end

    -- First add the top categories.
    for _, category in ipairs(topCategories) do
      if not added[category] and not addLast[category] then
        self:addUnitsInCategory(category)
        added[category] = true
      end
    end

    -- Next add the categories collected from the packages (sorted).
    local categories = Factory.getCategories()
    table.sort(categories)
    for _, category in ipairs(categories) do
      if not added[category] and not addLast[category] then
        self:addUnitsInCategory(category)
        added[category] = true
      end
    end

    -- Finally, add the bottom categories.
    for _, category in ipairs(bottomCategories) do
      if not added[category] then
        self:addUnitsInCategory(category)
      end
    end

  elseif ordering == "alphabet" then
    local units = Factory.getUnits()
    table.sort(units, function(a, b)
      return a.title:upper() < b.title:upper()
    end)
    local prev
    for _, loadInfo in ipairs(units) do
      local b = loadInfo.title:sub(1, 1):upper()
      if prev then
        local a = prev.title:sub(1, 1):upper()
        if b >= "A" and a ~= b then
          self:addCategory(b)
        end
      elseif b < "A" then
        self:addCategory("0-9")
      end
      self:addUnit(loadInfo)
      prev = loadInfo
    end
  end

  self.mlist:updateLayout()
  self.mlist:scrollToRow(row)
end

function Chooser:addCategory(category)
  self:addHeaderText(category)
end

function Chooser:addUnit(loadInfo, suppressLibraryName)
  local item = ChooserItem(loadInfo, suppressLibraryName)
  self:addControl(item)
end

function Chooser:addClipboard()
  local item = ClipboardItem()
  self:addControl(item)
end

function Chooser:updateRecent(loadInfo)
  if loadInfo.title == nil then
    return
  end
  local t = {
    loadInfo
  }
  for i, u in ipairs(Chooser.recent) do
    if u.title ~= loadInfo.title then
      t[#t + 1] = u
      if #t > 5 then
        break
      end
    end
  end
  Chooser.recent = t
end

function Chooser:choose(loadInfo)
  if loadInfo == "paste" then
    self.ring:loadClipboard()
  else
    self:updateRecent(loadInfo)
    self.ring:load(loadInfo)
  end
end

function Chooser:subReleased(i, shifted)
  return self.ring:subReleased(i, shifted)
end

function Chooser:cancelReleased(shifted)
  return self.ring:cancelReleased(shifted)
end

function Chooser:upReleased(shifted)
  return self.ring:upReleased(shifted)
end

return Chooser
