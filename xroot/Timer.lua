--[[
Copyright (c) 2010-2013 Matthias Richter

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Except as contained in this notice, the name(s) of the above copyright holders
shall not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
]] --
local functions = {}
local function _nothing_()
end
local to_remove = {}

local function update(dt)

  for handle in pairs(functions) do
    -- handle: {
    --   time = <number>,
    --   after = <function>,
    --   during = <function>,
    --   limit = <number>,
    --   count = <number>,
    -- }

    handle.time = handle.time + dt
    handle.during(dt, math.max(handle.limit - handle.time, 0))

    while handle.time >= handle.limit and handle.count > 0 do
      if handle.after(handle.after) == false then
        handle.count = 0
        break
      end
      handle.time = handle.time - handle.limit
      handle.count = handle.count - 1
    end

    if handle.count == 0 then
      table.insert(to_remove, handle)
    end
  end

  for i = 1, #to_remove do
    functions[to_remove[i]] = nil
  end
end

local function during(delay, during, after)
  local handle = {
    time = 0,
    during = during,
    after = after or _nothing_,
    limit = delay,
    count = 1
  }
  functions[handle] = true
  return handle
end

local function after(delay, func)
  return during(delay, _nothing_, func)
end

local function every(delay, after, count)
  local count = count or math.huge -- exploit below: math.huge - 1 = math.huge
  local handle = {
    time = 0,
    during = _nothing_,
    after = after,
    limit = delay,
    count = count
  }
  functions[handle] = true
  return handle
end

local function active(handle)
  return functions[handle]
end

local function cancel(handle)
  functions[handle] = nil
end

local function clear()
  functions = {}
end

local function script(f)
  local co = coroutine.wrap(f)
  co(function(t)
    after(t, co)
    coroutine.yield()
  end)
end

return {
  update = update,
  after = after,
  every = every,
  active = active,
  cancel = cancel,
  script = script
}
