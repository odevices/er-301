local app = app
local Env = require "Env"
local Class = require "Base.Class"
local Window = require "Base.Window"
local Drawings = require "Drawings"
local threshold = Env.EncoderThreshold.Default

local defaultColumn = {
  name = "default",
  width = 1.0
}

-- ListWindow
local ListWindow = Class {}
ListWindow:include(Window)

function ListWindow:init(opts)
  Window.init(self)
  self:setClassName("ListWindow")

  local columns = opts.columns or {
    defaultColumn
  }
  local title = opts.title or "ListWindow"

  local x = 0
  local W = 256
  if opts.showDetailPanel then
    -- dividing boundary between list box and detail panel
    W = Env.DetailPanel.X
    -- detail panel to the side of the list
    self.detailText = app.RichTextBox(W, 12, 256 - W, 54)
    self.detailText:setJustification(app.justifyCenter)
    self.mainGraphic:addChild(self.detailText)
  end
  self.gridWidth = W

  self.colByIndex = {}
  self.colByName = {}
  for i, column in ipairs(columns) do
    local width = column.width * W
    local listBox = app.ListBox(x, 0, width, 64)
    x = x + width
    listBox:setFocus()
    if column.showCheck then listBox:showCheck() end
    listBox:setTextSize(column.textSize or 10)
    if i < #columns then listBox:hideScrollbar() end
    listBox:setEmptyText(column.emptyText or "")
    listBox:setJustification(column.justify or app.justifyLeft)
    self.mainGraphic:addChild(listBox)
    self.colByName[column.name] = listBox
    self.colByIndex[#self.colByIndex + 1] = listBox
  end

  -- sub display title and underline
  local drawing = app.Drawing(0, 0, 128, 64)
  drawing:add(Drawings.Sub.TitleLine)
  self.subGraphic:addChild(drawing)
  local label = app.Label(title, 10)
  label:setCenter(64, app.GRID5_CENTER1)
  self.subGraphic:addChild(label)
  self.titleLabel = label

  self.mainButtons = {}
  self.mainReleaseHandlers = {}
  self.mainPressHandlers = {}
  self.subButtons = {}
  self.subReleaseHandlers = {}
  self.subPressHandlers = {}
  self:setSubCommand(1, "")
  self:setSubCommand(2, "")
  self:setSubCommand(3, "")
end

function ListWindow:size()
  return self.colByIndex[1]:size()
end

function ListWindow:setColumnWidths(...)
  local x = 0
  for i, width in ipairs {
    ...
  } do
    local w = width * self.gridWidth
    self.colByIndex[i]:setPosition(x, 0)
    self.colByIndex[i]:setSize(w, 64)
    x = x + w
  end
end

function ListWindow:getColumnByName(name)
  return self.colByName[name]
end

function ListWindow:getColumnByIndex(i)
  return self.colByIndex[i]
end

function ListWindow:setTitle(...)
  self.titleLabel:setText(string.format(...))
end

function ListWindow:addRow(row)
  for i, cell in ipairs(row) do
    if cell.label and cell.data and cell.checked then
      self.colByIndex[i]:addItem(cell.label, cell.data, cell.checked)
    elseif cell.label and cell.data then
      self.colByIndex[i]:addItem(cell.label, cell.data)
    elseif cell.label then
      self.colByIndex[i]:addItem(cell.label)
    end
  end
end

function ListWindow:clearRows()
  for _, column in ipairs(self.colByIndex) do column:clear() end
end

function ListWindow:setMainCommand(i, label, releaseHandler, pressHandler)
  local button = self.mainButtons[i]
  if button == nil then
    button = app.MainButton(label, i, true)
    self.mainGraphic:addChild(button)
    self.mainButtons[i] = button
  else
    button:setText(label)
  end
  self.mainReleaseHandlers[i] = releaseHandler
  self.mainPressHandlers[i] = pressHandler
end

function ListWindow:clearMainCommand(i)
  local button = self.mainButtons[i]
  if button then button:setText("") end
  self.mainReleaseHandlers[i] = nil
  self.mainPressHandlers[i] = nil
end

function ListWindow:setSubCommand(i, label, releaseHandler, pressHandler)
  local button = self.subButtons[i]
  if button == nil then
    button = app.TextPanel(label, i, 0.5, app.GRID5_LINE1)
    self.subGraphic:addChild(button)
    self.subButtons[i] = button
  else
    button:setText(label)
  end
  self.subReleaseHandlers[i] = releaseHandler
  self.subPressHandlers[i] = pressHandler
end

function ListWindow:clearSubCommand(i)
  local button = self.subButtons[i]
  if button then button:setText("") end
  self.subReleaseHandlers[i] = nil
  self.subPressHandlers[i] = nil
end

function ListWindow:onSelectionChanged()
end

function ListWindow:getSelection()
  return self.colByIndex[1]:getSelectedData()
end

function ListWindow:setSelection(id)
  for _, column in ipairs(self.colByIndex) do column:chooseWithData(id) end
  self:onSelectionChanged()
end

function ListWindow:encoder(change, shifted)
  local prevIndex = self.colByIndex[1]:getSelectedIndex()
  for _, column in ipairs(self.colByIndex) do
    column:encoder(change, shifted, threshold)
  end
  local curIndex = self.colByIndex[1]:getSelectedIndex()
  if curIndex ~= prevIndex then self:onSelectionChanged(prevIndex, curIndex) end
  return true
end

function ListWindow:mainPressed(i, shifted)
  local handler = self.mainPressHandlers[i]
  if handler then
    handler(self, shifted)
    return true
  else
    return false
  end
end

function ListWindow:mainReleased(i, shifted)
  local handler = self.mainReleaseHandlers[i]
  if handler then
    handler(self, shifted)
    return true
  else
    return false
  end
end

function ListWindow:subPressed(i, shifted)
  local handler = self.subPressHandlers[i]
  if handler then
    handler(self, shifted)
    return true
  else
    return false
  end
end

function ListWindow:subReleased(i, shifted)
  local handler = self.subReleaseHandlers[i]
  if handler then
    handler(self, shifted)
    return true
  else
    return false
  end
end

function ListWindow:cancelReleased(shifted)
  if not shifted then
    self:hide()
    self:emitSignal("done")
  end
  return true
end

function ListWindow:upReleased(shifted)
  if not shifted then
    self:hide()
    self:emitSignal("done")
  end
  return true
end

function ListWindow:homeReleased()
  self:hide()
  self:emitSignal("done")
  return true
end

return ListWindow
