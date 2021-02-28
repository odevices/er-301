local app = app
local Class = require "Base.Class"
local SpottedStrip = require "SpottedStrip"
local SourceGroup = require "Source.ExternalChooser.Group"

local externals = {
  INx = {
    app.getExternalSource("IN1"),
    app.getExternalSource("IN2"),
    app.getExternalSource("IN3"),
    app.getExternalSource("IN4")
  },
  Ax = {
    app.getExternalSource("A1"),
    app.getExternalSource("A2"),
    app.getExternalSource("A3")
  },
  Bx = {
    app.getExternalSource("B1"),
    app.getExternalSource("B2"),
    app.getExternalSource("B3")
  },
  Cx = {
    app.getExternalSource("C1"),
    app.getExternalSource("C2"),
    app.getExternalSource("C3")
  },
  Dx = {
    app.getExternalSource("D1"),
    app.getExternalSource("D2"),
    app.getExternalSource("D3")
  },
  Gx = {
    app.getExternalSource("G1"),
    app.getExternalSource("G2"),
    app.getExternalSource("G3"),
    app.getExternalSource("G4")
  },
  OUTx = {
    app.getExternalSource("OUT1"),
    app.getExternalSource("OUT2"),
    app.getExternalSource("OUT3"),
    app.getExternalSource("OUT4")
  }
}

local ExternalChooser = Class {}
ExternalChooser:include(SpottedStrip)

function ExternalChooser:init(ring)
  SpottedStrip.init(self)
  self:setClassName("Source.ExternalChooser")
  self.ring = ring

  self.groups = {}
  self:addSourceGroup("INx", externals["INx"])
  self:addSourceGroup("Ax", externals["Ax"])
  self:addSourceGroup("Bx", externals["Bx"])
  self:addSourceGroup("Cx", externals["Cx"])
  self:addSourceGroup("Dx", externals["Dx"])
  self:addSourceGroup("Gx", externals["Gx"])
  self:addSourceGroup("OUTx", externals["OUTx"])
end

function ExternalChooser:addSourceGroup(name, sources)
  local group = SourceGroup(name, sources)
  self.groups[name] = group
  self:appendSection(group)
end

function ExternalChooser:choose(src)
  return self.ring:choose(src)
end

function ExternalChooser:homeReleased()
  return self.ring:homeReleased()
end

function ExternalChooser:subReleased(i, shifted)
  return self.ring:subReleased(i, shifted)
end

function ExternalChooser:cancelReleased(shifted)
  return self.ring:cancelReleased(shifted)
end

function ExternalChooser:upReleased(shifted)
  return self.ring:upReleased(shifted)
end

return ExternalChooser
