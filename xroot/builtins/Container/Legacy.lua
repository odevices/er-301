local Utils = require "Utils"
local Persist = require "Persist"

local function createGate(unit, id, t)
  local bdata = t.branches[id]
  local cb = unit:addControlBranch("Gate", id, bdata)
  unit:placeControl(id, "expanded")
  local cdata = t.controls and t.controls[id]
  if cdata then cb.control:deserialize(cdata) end
  cb.control:customize{
    outputMode = app.COMPARATOR_GATE
  }
  return cb
end

local function createToggle(unit, id, t)
  local cb = createGate(unit, id, t)
  cb.control:customize{
    outputMode = app.COMPARATOR_TOGGLE
  }
  return cb
end

local function createTrigger(unit, id, t)
  local cb = createGate(unit, id, t)
  cb.control:customize{
    outputMode = app.COMPARATOR_TRIGGER_ON_RISE
  }
  return cb
end

local function createLinear(unit, id, t)
  local bdata = t.branches[id]
  local cb = unit:addControlBranch("GainBias", id, bdata)
  unit:placeControl(id, "expanded")
  local cdata = t.controls and t.controls[id]
  if cdata then cb.control:deserialize(cdata) end
  local odata = t.objects[id .. "_gainbias"]
  if odata then
    local o = cb.objects[1]
    Persist.deserializeObject(o, odata)
  end
  return cb
end

local function createDecibel(unit, id, t)
  local cb = createLinear(unit, id, t)
  cb.control:customize{
    biasUnits = app.unitDecibels,
    biasMin = -60,
    biasMax = 12,
    biasSuperCoarseStep = 6,
    biasCoarseStep = 1,
    biasFineStep = 0.1,
    biasSuperFineStep = 0.01
  }
  return cb
end

local function createPitch(unit, id, t)
  local bdata = t.branches[id]
  local cb = unit:addControlBranch("Pitch", id, bdata)
  unit:placeControl(id, "expanded")
  local cdata = t.controls and t.controls[id]
  if cdata then cb.control:deserialize(cdata) end
  local odata = t.objects[id .. "_tune"]
  if odata then
    local o = cb.objects[1]
    Persist.deserializeObject(o, odata)
  end
  return cb
end

local descriptors = {
  {
    description = "Linear",
    prefix = "lin",
    aliases = {
      "linear",
      "Ax+B"
    },
    create = createLinear
  },
  {
    description = "Decibel",
    prefix = "dB",
    aliases = {
      "decibel",
      "dB(Ax+B)"
    },
    create = createDecibel
  },
  {
    description = "Pitch",
    prefix = "V/oct",
    aliases = {
      "pitch",
      "V/oct"
    },
    create = createPitch
  },
  {
    description = "Gate",
    prefix = "gate",
    aliases = {
      "gate",
      "Gate"
    },
    create = createGate
  },
  {
    description = "Toggle",
    prefix = "sw",
    aliases = {
      "toggle"
    },
    create = createToggle
  },
  {
    description = "Trigger",
    prefix = "trig",
    aliases = {
      "trigger"
    },
    create = createTrigger
  }
}

-- hash aliases
local aliases = {}
for i, czd in ipairs(descriptors) do
  czd.type = czd.aliases[1]
  for j, alias in ipairs(czd.aliases) do aliases[alias] = czd end
end

local function lookup(type)
  return aliases[type]
end

local function fixLegacyData(data)
  if data then
    local keys = {}
    local prefixes = {
      ["Ax+B"] = "cv",
      ["Gate"] = "gate",
      ["V/oct"] = "V/oct",
      ["db(Ax+B)"] = "dB"
    }
    for i, czd in ipairs(data) do
      if czd.id == nil and czd.controlName == nil then
        local prefix = prefixes[czd.controlType]
        if prefix then
          czd.controlName = Utils.generateUniqueKey(keys, prefix)
          keys[czd.controlName] = true
        end
      end
    end
  end
end

local function insertCustomization(unit, type, id, objects)
  local czd = lookup(type)
  if czd == nil then
    app.logInfo("%s: Unknown customization type [%s %s]", unit, type, id)
    return
  end
  app.logInfo("%s: insert customization [%s %s]", unit, type, id)
  czd.create(unit, id, objects)
end

local function deserializeCustomizations(unit, t)
  if t.customizations then
    -- v0.3 legacy support
    fixLegacyData(t.customizations)
    for i, czd in ipairs(t.customizations) do
      local name = czd.controlName or czd.id
      insertCustomization(unit, czd.controlType, name, t)
    end
  end
  if t.patch and unit.patch then unit.patch:deserialize(t.patch) end
end

return {
  deserializeCustomizations = deserializeCustomizations
}
