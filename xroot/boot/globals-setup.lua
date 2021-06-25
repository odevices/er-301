local app = app

app.logInfo("Setup global namespace...")

local unitInputNames = {
  ["In1"] = 0,
  ["In2"] = 1,
  ["In3"] = 2,
  ["In4"] = 3
}

local function connectUnitInput(pUnit, inputName, toObject, toPortName)
  local channel = unitInputNames[inputName]
  if channel == nil or channel >= pUnit:getInputCount() then
    app.logError("connect: Unit(%s) has no input named %s", pUnit:name(),
                 inputName)
  elseif not pUnit:addInput(channel, toObject, toPortName) then
    app.logError("connect: Object(%s) has no port named '%s'.", toObject:name(),
                 toPortName)
  end
end

local unitOutputNames = {
  ["Out1"] = 0,
  ["Out2"] = 1,
  ["Out3"] = 2,
  ["Out4"] = 3
}

local function connectUnitOutput(fromObject, fromPortName, pUnit, outputName)
  local channel = unitOutputNames[outputName]
  if channel == nil or channel >= pUnit:getOutputCount() then
    app.logError("connect: Unit(%s) has no output named %s", pUnit:name(),
                 outputName)
  elseif not pUnit:setOutput(channel, fromObject, fromPortName) then
    app.logError("connect: Object(%s) has no port named '%s'.",
                 fromObject:name(), fromPortName)
  end
end

local function connectObjects(fromObject, fromPortName, toObject, toPortName)
  if not app.connect(fromObject, fromPortName, toObject, toPortName) then
    app.logError("connect: Failed to connect %s(%s) to %s(%s)",
                 fromObject:name(), fromPortName, toObject:name(), toPortName)
  end
end

function connect(fromObject, fromPortName, toObject, toPortName)
  -- app.logInfo("connect: [%s].%s to [%s].%s", swig_type(fromObject),
  --            fromPortName, swig_type(toObject), toPortName)
  if fromObject.pUnit then
    connectUnitInput(fromObject.pUnit, fromPortName, toObject, toPortName)
  elseif toObject.pUnit then
    connectUnitOutput(fromObject, fromPortName, toObject.pUnit, toPortName)
  else
    connectObjects(fromObject, fromPortName, toObject, toPortName)
  end
end

-- tying parameters
local function tieExpression(slaveObj, slaveParamName, expression, ...)
  local slave = slaveObj:getParameter(slaveParamName) or
                    slaveObj:getOption(slaveParamName)
  if slave == nil then
    app.logError("tie: Object(%s) has no parameter/option named '%s'.",
                 slaveObj:name(), slaveParamName)
    return
  end
  local E = app.Expression()
  if not E:setFunction(expression) then
    app.logError("Invalid expression: %s", expression)
    return
  end
  local args = {
    ...
  }
  for i = 1, #args, 2 do
    local obj = args[i]
    local name = args[i + 1]
    local param = obj:getParameter(name)
    if param == nil then
      app.logError("tie: Object(%s) has no parameter/option named '%s'.",
                   obj:name(), name)
      return
    end
    E:addParameter(param)
  end
  E:compile()
  slave:tie(E)
end

local function tieParameter(slaveObj, slaveParamName, masterObj, masterParamName)
  -- app.logInfo("tie: [%s].%s to [%s].%s", swig_type(slaveObj), slaveParamName,
  --            swig_type(masterObj), masterParamName)
  local slave = slaveObj:getParameter(slaveParamName) or
                    slaveObj:getOption(slaveParamName)
  local master = masterObj:getParameter(masterParamName) or
                     masterObj:getOption(masterParamName)
  if master == nil then
    app.logError("tie: Object(%s) has no parameter/option named '%s'.",
                 masterObj:name(), masterParamName)
    return
  end
  if slave == nil then
    app.logError("tie: Object(%s) has no parameter/option named '%s'.",
                 slaveObj:name(), slaveParamName)
    return
  end
  slave:tie(master)
end

function tie(slaveObj, slaveParamName, ...)
  local args = {
    ...
  }
  if type(args[1]) == "string" then
    tieExpression(slaveObj, slaveParamName, ...)
  else
    tieParameter(slaveObj, slaveParamName, ...)
  end
end
