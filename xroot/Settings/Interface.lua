local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Window = require "Base.Window"
local Settings = require "Settings"

local menuItems = {}
menuItems[#menuItems + 1] = {
  "addCategory",
  "General"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "sampleRate"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "frameLength"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "animation"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "floatingMenuDelay"
}
if app.TESTING then
  menuItems[#menuItems + 1] = {
    "addVariable",
    "enableDevMode"
  }
end
menuItems[#menuItems + 1] = {
  "addCategory",
  "Screen Saver"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "screenSaver"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "screenSaverGraphics"
}
menuItems[#menuItems + 1] = {
  "addCategory",
  "Units"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "unitDisableOnBypass"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "unitControlReadoutSource"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "unitBrowserDefault"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "containerUnitNameGen"
}
menuItems[#menuItems + 1] = {
  "addCategory",
  "QuickSaves"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "restoreLastSlotAction"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "quickSaveRestoresClipboard"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "quickSaveRestoresRecorder"
}
menuItems[#menuItems + 1] = {
  "addCategory",
  "Multitrack Recorder"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "fileRecorderChannelCount"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "fileRecorderSingleTrackSaving"
}
menuItems[#menuItems + 1] = {
  "addCategory",
  "Sample Slicing"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "insertSliceAtZeroCrossing"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "shiftInsertSliceAtZeroCrossing"
}
menuItems[#menuItems + 1] = {
  "addCategory",
  "File Browser"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "samplePreviewStop"
}
menuItems[#menuItems + 1] = {
  "addCategory",
  "Confirmations"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "confirmUnitDelete"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "confirmChainClear"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "confirmGlobalChainDelete"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "confirmClearSlices"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "confirmPurgeUnusedSamples"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "confirmSampleEdit"
}
menuItems[#menuItems + 1] = {
  "addCategory",
  "USB"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "usbEnable"
}
menuItems[#menuItems + 1] = {
  "addVariable",
  "usbMode"
}

local Interface = Class {}
Interface:include(Window)

function Interface:init(name)
  Window.init(self, name)
  self:setClassName("Settings.Interface")

  local graphic

  graphic = app.ListBox(0, 0, 256 - 40, 64)
  graphic:setTextSize(10)
  graphic:hideScrollbar()
  graphic:showHighlight()
  self.mainList = graphic
  graphic:setFocus()
  self.mainGraphic:addChild(graphic)

  graphic = app.ListBox(256 - 40, 0, 40, 64)
  graphic:setTextSize(10)
  graphic:showHighlight()
  self.valueList = graphic
  graphic:setFocus()
  self.mainGraphic:addChild(graphic)

  graphic = app.ListBox(10, 0, 128, 60)
  graphic:setTextSize(12)
  graphic:setEmptyText("")
  self.subList = graphic
  graphic:setFocus()
  self.subGraphic:addChild(graphic)

  graphic = app.SubButton("toggle", 3)
  graphic:hide()
  self.subGraphic:addChild(graphic)
  self.toggleButton = graphic

  self:reload()
  self:focusMain()
end

function Interface:addCategory(category)
  local mainList = self.mainList
  local valueList = self.valueList
  local name = string.format("o %s:", category)
  mainList:addItem(name, "category")
  mainList:highlight(name)
  valueList:addItem("", "category")
end

function Interface:addVariable(name)
  local mainList = self.mainList
  local valueList = self.valueList
  local var = Settings.variable(name)
  if var then
    mainList:addItem("  + " .. var.description, name)
    valueList:addItem(var:getValueText(), name)
  else
    app.logInfo("Settings.Interface:addVariable(%s): unknown variable.", name)
  end
end

function Interface:reload()
  local mainList = self.mainList
  local valueList = self.valueList
  mainList:clear()
  valueList:clear()

  for _, x in ipairs(menuItems) do
    local method = x[1]
    local value = x[2]
    self[method](self, value)
  end

  mainList:scrollToTop()
  valueList:scrollToTop()
  -- skip the category at the top
  mainList:scrollDown()
  valueList:scrollDown()
  self:updateDetail()
end

function Interface:updateDetail()
  local mainList = self.mainList
  local subList = self.subList
  local name = mainList:getSelectedData()
  subList:clear()
  if name == "category" then
    self.toggleButton:hide()
    return
  end
  local var = Settings.variable(name)
  if var then
    if var.type == "bool" then
      subList:addItem("yes")
      subList:addItem("no")
      if var.value then
        subList:choose("yes")
      else
        subList:choose("no")
      end
    elseif var.type == "string" then
      for _, choice in ipairs(var.choices) do
        subList:addItem(choice)
      end
      subList:choose(var.value)
    end
    self.toggleButton:show()
  else
    self.toggleButton:hide()
  end
end

function Interface:commit()
  local subList = self.subList
  local value = subList:getSelected()
  local valueList = self.valueList
  local name = valueList:getSelectedData()
  local var = Settings.variable(name)
  if var then
    var:set(value)
    valueList:updateNameByData(var:getValueText(), name)
    Settings.scheduleSave()
  end
end

function Interface:focusMain()
  self.focus = "main"
  self.mainList:setFocus()
  self.valueList:setFocus()
  self.subList:clearFocus()
end

function Interface:focusSub()
  self.focus = "sub"
  self.mainList:clearFocus()
  self.valueList:setFocus()
  self.subList:setFocus()
end

local threshold = Env.EncoderThreshold.Default
function Interface:encoder(change, shifted)
  if self.focus == "main" then
    local mainList = self.mainList
    local valueList = self.valueList
    local scrolled = mainList:encoder(change, shifted, threshold)
    valueList:encoder(change, shifted, threshold)
    if scrolled then
      if mainList:getSelectedData() == "category" then
        if change > 0 or mainList:getSelectedIndex() == 0 then
          mainList:scrollDown()
          valueList:scrollDown()
        elseif change < 0 then
          mainList:scrollUp()
          valueList:scrollUp()
        end
      end
      self:updateDetail()
    end
  else
    local subList = self.subList
    if subList:encoder(change, shifted, threshold) then
      self:commit()
    end
  end
  return true
end

function Interface:enterReleased()
  if self.focus == "main" then
    local mainList = self.mainList
    local name = mainList:getSelectedData()
    local var = Settings.variable(name)
    if var then
      self.savedName = name
      self.savedValue = var.value
      self:focusSub()
    end
  else
    self:focusMain()
  end
  return true
end

function Interface:cancelReleased(shifted)
  if not shifted and self.focus == "sub" then
    if self.savedName and self.savedValue then
      local var = Settings.variable(self.savedName)
      var:set(self.savedValue)
      self.savedName = nil
      self.savedValue = nil
    end
    self:focusMain()
  else
    self:hide()
  end
  return true
end

function Interface:upReleased(shifted)
  if not shifted then
    if self.focus == "sub" then
      self:focusMain()
    else
      self:hide()
    end
  end
  return true
end

function Interface:homeReleased()
  self:hide()
  return true
end

function Interface:mainReleased()
  return true
end

function Interface:subReleased(i, shifted)
  if not shifted and i == 3 then
    local subList = self.subList
    subList:enableCircularScrolling()
    subList:scrollDown()
    subList:disableCircularScrolling()
    self:commit()
  end
  return true
end

function Interface:onHide()
  Settings.saveIfNeeded()
end

function Interface:getPreview()
  return nil
end

return Interface()
