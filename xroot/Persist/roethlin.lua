--[[
Derived from roethlin.lua.
Original Copyright follows.

 Copyright (c) 2010 Gerhard Roethlin

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

 Value serialization
store(path, ...): Stores arbitrary items to the file at the given path
storeWithHooks(path, hooks, ...): Hooks keyed on the type are called before each write.
load(path): Loads files that were previously stored with store and returns them
Limitations: Does not export userdata, threads or most function values
]] --
local writers = {}
local typeHooks = {}

-- Private methods

-- write thing (dispatcher)
local function write(file, item, level, objRefNames)
  local itemType = type(item)
  local hook = typeHooks[itemType]
  if hook then
    item = hook(item)
  end
  writers[itemType](file, item, level, objRefNames);
end

-- write indent
local function writeIndent(file, level)
  for i = 1, level do
    file:write("\t");
  end
end

-- recursively count references
local function refCount(objRefCount, item)
  -- only count reference types (tables)
  if type(item) == "table" then
    -- Increase ref count
    if objRefCount[item] then
      objRefCount[item] = objRefCount[item] + 1;
    else
      objRefCount[item] = 1;
      -- If first encounter, traverse
      for k, v in pairs(item) do
        refCount(objRefCount, k);
        refCount(objRefCount, v);
      end
    end
  end
end

writers["nil"] = function(file, item)
  file:write("nil");
end

writers["number"] = function(file, item)
  file:write(tostring(item));
end

writers["string"] = function(file, item)
  file:write(string.format("%q", item));
end

writers["boolean"] = function(file, item)
  if item then
    file:write("true");
  else
    file:write("false");
  end
end

writers["table"] = function(file, item, level, objRefNames)
  local refIdx = objRefNames[item];
  if refIdx then
    -- Table with multiple references
    file:write("shared[" .. refIdx .. "]");
  else
    -- Single use table
    file:write("{\n");
    for k, v in pairs(item) do
      writeIndent(file, level + 1);
      file:write("[");
      write(file, k, level + 1, objRefNames);
      file:write("] = ");
      write(file, v, level + 1, objRefNames);
      file:write(";\n");
    end
    writeIndent(file, level);
    file:write("}");
  end
end

writers["function"] = function(file, item)
  -- Does only work for "normal" functions, not those
  -- with upvalues or c functions
  local dInfo = debug.getinfo(item, "uS");
  if dInfo.nups > 0 then
    file:write("nil --[[functions with upvalue not supported]]");
  elseif dInfo.what ~= "Lua" then
    file:write("nil --[[non-lua function not supported]]");
  else
    local r, s = pcall(string.dump, item);
    if r then
      file:write(string.format("loadstring(%q)", s));
    else
      file:write("nil --[[function could not be dumped]]");
    end
  end
end

writers["thread"] = function(file, item)
  file:write("nil --[[thread]]\n");
end

writers["userdata"] = function(file, item)
  file:write("nil --[[userdata]]\n");
end

-- Public Methods

local function store(path, ...)
  local file, e;
  if type(path) == "string" then
    -- Path, open a file
    file, e = io.open(path, "w");
    if not file then
      app.logError("Failed to open %s. %s", path, e)
      return false; -- error(e);
    end
  else
    -- Just treat it as file
    file = path;
  end
  local n = select("#", ...);
  -- Count references
  local objRefCount = {}; -- Stores reference that will be exported
  for i = 1, n do
    refCount(objRefCount, (select(i, ...)));
  end
  -- Export Objects with more than one ref and assign name
  -- First, create empty tables for each
  local objRefNames = {};
  local objRefIdx = 0;
  -- file:write("-- Persistent Data\n");
  file:write("local shared = {\n");
  for obj, count in pairs(objRefCount) do
    if count > 1 then
      objRefIdx = objRefIdx + 1;
      objRefNames[obj] = objRefIdx;
      file:write("{};"); -- table objRefIdx
    end
  end
  file:write("\n} -- shared\n");
  -- Then fill them (this requires all empty shared to exist)
  for obj, idx in pairs(objRefNames) do
    for k, v in pairs(obj) do
      file:write("shared[" .. idx .. "][");
      write(file, k, 0, objRefNames);
      file:write("] = ");
      write(file, v, 0, objRefNames);
      file:write(";\n");
    end
  end
  -- Create the remaining objects
  for i = 1, n do
    file:write("local " .. "obj" .. i .. " = ");
    write(file, (select(i, ...)), 0, objRefNames);
    file:write("\n");
  end
  -- Return them
  if n > 0 then
    file:write("return obj1");
    for i = 2, n do
      file:write(" ,obj" .. i);
    end
    file:write("\n");
  else
    file:write("return\n");
  end
  file:close();
  return true;
end

local function storeWithHooks(path, hooks, ...)
  local saved = typeHooks
  typeHooks = hooks
  local result = store(path, ...)
  typeHooks = saved
  return result
end

local function load(path)
  local f, e = loadfile(path);
  if f then
    return f();
  else
    return nil, e;
  end
end

return {
  store = store,
  load = load,
  storeWithHooks = storeWithHooks
}

