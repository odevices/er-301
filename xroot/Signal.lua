local signals = {}
local __weak__ = {__mode = "k"}

local function createSignal(s)
  local slots = {strong = {}, weak = setmetatable({}, __weak__)}
  signals[s] = slots
  return slots
end

local function register(s, f)
  local slots = signals[s] or createSignal(s)
  slots.strong[f] = true
  return f
end

local function weakRegister(s, f)
  local slots = signals[s] or createSignal(s)
  slots.weak[f] = true
  return f
end

local function emit(s, ...)
  -- if app.TESTING then app.logInfo("Signal:emit(%s)",s) end
  local slots = signals[s] or createSignal(s)
  -- Move invocations outside of the loop
  -- in case a handler manipulates the signal table.
  local tmp = {}
  for f, _ in pairs(slots.strong) do
    tmp[#tmp + 1] = f
  end
  for f, _ in pairs(slots.weak) do
    tmp[#tmp + 1] = f
  end
  for i = 1, #tmp do
    local f = tmp[i]
    if type(f) == "table" then
      f[s](f, ...)
    else
      f(...)
    end
  end
end

local function remove(s, ...)
  local f = {...}
  local slots = signals[s] or createSignal(s)
  local strong = slots.strong
  local weak = slots.weak
  for i = 1, select('#', ...) do
    strong[f[i]] = nil
    weak[f[i]] = nil
  end
end

local function clear(...)
  local s = {...}
  for i = 1, select('#', ...) do
    createSignal(s[i])
  end
end

local function emitPattern(p, ...)
  for s in pairs(signals) do
    if s:match(p) then
      emit(s, ...)
    end
  end
end

local function removePattern(p, ...)
  for s in pairs(signals) do
    if s:match(p) then
      remove(s, ...)
    end
  end
end

local function clearPattern(p)
  for s in pairs(signals) do
    if s:match(p) then
      createSignal(s)
    end
  end
end

return {
  register = register,
  weakRegister = weakRegister,
  emit = emit,
  remove = remove,
  clear = clear,
  emitPattern = emitPattern,
  removePattern = removePattern,
  clearPattern = clearPattern
}
