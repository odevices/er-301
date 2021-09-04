local app = app

-- create system graphic objects
local pMainOverlay = app.Graphic(0, 0, 256, 64)
app.UIThread.setMainOverlay(pMainOverlay)
local pSubOverlay = app.Graphic(0, 0, 128, 64)
app.UIThread.setSubOverlay(pSubOverlay)


local function addMainGraphic(graphic)
  if graphic then pMainOverlay:addChildOnce(graphic) end
end

local function removeMainGraphic(graphic)
  if graphic then pMainOverlay:removeChild(graphic) end
end

local function addSubGraphic(graphic)
  if graphic then pSubOverlay:addChildOnce(graphic) end
end

local function removeSubGraphic(graphic)
  if graphic then pSubOverlay:removeChild(graphic) end
end

return {
  addMainGraphic = addMainGraphic,
  removeMainGraphic = removeMainGraphic,
  addSubGraphic = addSubGraphic,
  removeSubGraphic = removeSubGraphic,
}