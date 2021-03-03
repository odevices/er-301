local Class = require "Base.Class"
local Settings = require "Settings"
local Library = require "Package.Library"
local Header = require "Package.Menu.Header"
local Choices = require "Package.Menu.Choices"
local Task = require "Package.Menu.Task"
local libteletype = require "teletype.libteletype"

local Teletype = Class {
  -- All teletype units share this one instance of the i2c message dispatcher.
  dispatcher = libteletype.Dispatcher()
}
Teletype:include(Library)

function Teletype:init(args)
  Library.init(self, args)
  self.enabled = false
  if self:uartInUse() then
    self.defaults["autoEnable"] = "no"
  else
    self.defaults["autoEnable"] = "yes"
  end
  self.defaults["address"] = "0x31"
end

function Teletype:uartInUse()
  return app.TESTING and Settings.get("enableDevMode")
end

function Teletype:enable(force)
  if not self.enabled and (force or not self:uartInUse()) then
    local address = self:getConfiguration("address")
    self.dispatcher:enable(tonumber(address))
    self.enabled = true
    app.logInfo("%s: i2c enabled (address=%s).", self, address)
  end
end

function Teletype:disable()
  if self.enabled then
    self.dispatcher:disable()
    if self:uartInUse() then app.Uart_enable() end
    self.enabled = false
    app.logInfo("%s: i2c disabled.", self)
  end
end

-- overrides

function Teletype:onLoad()
  if self:getConfiguration("autoEnable") == "yes" then self:enable() end
end

function Teletype:onUnload()
  self:disable()
end

function Teletype:onShowMenu()
  local order = {
    "header",
    "manualEnable",
    "autoEnable",
    "address"
  }
  local controls = {}

  controls.header = Header {
    description = string.format("I2C is %s.",
                                self.enabled and "enabled" or "disabled")
  }

  controls.autoEnable = Choices {
    description = "Auto Enable?",
    callback = function(choice)
      self:setConfiguration("autoEnable", choice)
      if choice == "yes" then
        self:enable()
      else
        self:disable()
      end
    end,
    choices = {
      "yes",
      "no"
    },
    current = self:getConfiguration("autoEnable")
  }

  controls.address = Choices {
    description = "I2C address",
    callback = function(choice)
      self:setConfiguration("address", choice)
      if self.enabled then
        self:disable()
        self:enable()
      end
    end,
    choices = {
      "0x31",
      "0x32",
      "0x33"
    },
    current = self:getConfiguration("address")
  }

  controls.manualEnable = Task {
    description = self.enabled and "Disable Now?" or "Enable Now?",
    task = function()
      if self.enabled then
        self:disable()
      else
        self:enable(true)
      end
    end
  }

  return controls, order, "Teletype Library"
end

-- WARNING: We return the class definition here BUT,
-- the Package Manager will instantiate this class and save it
-- in the package.loaded table so that further calls to require
-- will receive the same instance of this class.
return Teletype
