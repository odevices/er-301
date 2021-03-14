local app = app
local Context = require "Base.Context"
local Mode = require "Base.Mode"

local Menu = require "AdminMode.Menu"
local Settings = require "Settings"
local SamplePoolInterface = require "Sample.Pool.Interface"
local PreampInterface = require "Preamp.Interface"

local settings = require "Settings.Interface"
local gains = PreampInterface()
local load = require "LoadView"
local samples = SamplePoolInterface("Admin", "admin")
local recorder = require "FileRecorder"
local firmware = require "Firmware"
local card = require "Card.Console"
local packages = require "Package.Interface"
local globalChains = require "GlobalChains.Interface"
local menu = Menu("Admin")

menu:header("Global Audio:")
menu:add("Sample Pool", samples)
menu:add("Global Chains", globalChains)
menu:add("Multitrack Recorder", recorder)
menu:add("Preamp Settings", gains)

menu:header("Maintenance:")
menu:add("CPU Load", load)
menu:add("System Settings", settings)
menu:add("Install Firmware", firmware)
menu:add("Package Manager", packages)
menu:add("Card Console", card)

local history = require "LogHistory"
menu:add("Log History", history)

if app.TESTING and Settings.get("enableDevMode") then
  local tests = require "Tests.Console"
  menu:add("Test Console", tests())
end

local Drawings = require "Drawings"
local drawing = app.Drawing(0, 0, 128, 64)
drawing:add(Drawings.Sub.TitleLine)
menu.subGraphic:addChild(drawing)

local label = app.Label("Current Firmware", 12)
label:setJustification(app.justifyLeft)
label:setPosition(0, app.GRID4_LINE1 + 1)
menu.subGraphic:addChild(label)

local version = string.format("Version: %s (%s)", app.FIRMWARE_VERSION,
                              app.FIRMWARE_STATUS)
label = app.Label(version, 10)
label:setJustification(app.justifyLeft)
label:setPosition(0, app.GRID4_LINE2)
menu.subGraphic:addChild(label)

local desc = string.format("%s (%0.0fkHz, %dsmps)", app.FIRMWARE_NAME,
                           app.globalConfig.sampleRate / 1000.0,
                           app.globalConfig.frameLength)
label = app.Label(desc, 10)
label:setJustification(app.justifyLeft)
label:setPosition(0, app.GRID4_LINE3)
menu.subGraphic:addChild(label)

local context = Context("Admin", menu)
local mode = Mode("Admin")

function mode:enter()
  Mode.enter(self)
  local Application = require "Application"
  Application.setVisibleContext(context)
  app.ChannelLEDs_off(0)
  app.ChannelLEDs_off(1)
  app.ChannelLEDs_off(2)
  app.ChannelLEDs_off(3)
end

return mode
