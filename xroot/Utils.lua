local function pp(t)
  local print_r_cache = {}
  local function sub_print_r(t, indent)
    if (print_r_cache[tostring(t)]) then
      print(indent .. "*" .. tostring(t))
    else
      print_r_cache[tostring(t)] = true
      if (type(t) == "table") then
        for pos, val in pairs(t) do
          if (type(val) == "table") then
            print(indent .. "[" .. pos .. "] => " .. tostring(t) .. " {")
            sub_print_r(val, indent .. string.rep(" ", string.len(pos) + 8))
            print(indent .. string.rep(" ", string.len(pos) + 6) .. "}")
          elseif (type(val) == "string") then
            print(indent .. "[" .. pos .. '] => "' .. val .. '"')
          else
            print(indent .. "[" .. pos .. "] => " .. tostring(val))
          end
        end
      else
        print(indent .. tostring(t))
      end
    end
  end
  if (type(t) == "table") then
    print(tostring(t) .. " {")
    sub_print_r(t, "  ")
    print("}")
  else
    sub_print_r(t, "  ")
  end
  print()
end

local function timestamp(totalSecs)
  local mins = math.floor(totalSecs / 60)
  local secs = math.floor(totalSecs - mins * 60)
  local ms = math.floor(1000 * (totalSecs - mins * 60 - secs))
  return string.format("%02d:%02d.%03d", mins, secs, ms)
end

local function duration(totalSecs, sep)
  if totalSecs < 0.001 then
    return "0s"
  elseif totalSecs < 1 then
    return string.format("%3.0fms", totalSecs * 1000)
  elseif totalSecs < 60 then
    return string.format("%2.1fs", totalSecs)
  else
    local mins = math.floor(totalSecs / 60)
    local secs = math.floor(totalSecs - mins * 60)
    sep = sep or " "
    return string.format("%2dm%s%2ds", mins, sep, secs)
  end
end

local function durationNoSpace(totalSecs, sep)
  if totalSecs < 0.001 then
    return "0s"
  elseif totalSecs < 1 then
    return string.format("%03fms", totalSecs * 1000)
  elseif totalSecs < 60 then
    return string.format("%02.1fs", totalSecs)
  else
    local mins = math.floor(totalSecs / 60)
    local secs = math.floor(totalSecs - mins * 60)
    sep = sep or " "
    return string.format("%02dm%s%02ds", mins, sep, secs)
  end
end

local function split(s, delimiter)
  local result = {}
  for match in (s .. delimiter):gmatch("(.-)" .. delimiter:quote()) do
    if match then table.insert(result, match) end
  end
  return result
end

local function trim(s)
  return s:match '^()%s*$' and '' or s:match '^%s*(.*%S)'
end

local function startsWith(s, pattern)
  return s and pattern and (string.sub(s, 1, string.len(pattern)) == pattern)
end

local function endsWith(s, pattern)
  return s and pattern and (pattern == '' or string.sub(s, -string.len(pattern)) == pattern)
end

local function firstToUpper(s)
  return (s:gsub("^%l", string.upper))
end

local function removeExtension(text)
  return text:gsub("[.]%w*$", "", 1)
end

local function shorten(text, maxLen, div, forceRemoveExtension)
  if text == nil then return end
  if forceRemoveExtension then text = removeExtension(text) end
  if text:len() <= maxLen then return text end
  -- trim and remove consecutive white space
  text = text:match("^%s*(.*%S)")
  if text then text = text:gsub("%s%s*", " ") end
  if text:len() <= maxLen then return text end
  -- strip off the extension
  if not forceRemoveExtension then
    text = removeExtension(text)
    if text:len() <= maxLen then return text end
  end
  -- remove any whitespace
  text = text:gsub("%s%s*", "")
  if text:len() <= maxLen then return text end
  -- remove any non-alphanumeric characters
  -- text = text:gsub("[^%w]","")
  -- if text:len() <= maxLen then
  --  return text
  -- end
  -- telescope the string with ellipsis in the middle
  div = div or ".."
  local divLen
  if div == ".." then
    divLen = 1
  else
    divLen = div:len()
  end
  local n = maxLen - divLen
  local i = math.floor(n / 2)
  local j = n - i
  if i < 1 or j < 1 then
    return text:sub(1, maxLen - 1) .. text:sub(-1)
  else
    return text:sub(1, i) .. div .. text:sub(-j, -1)
  end
end

local function shortenPath(path, maxLen)
  -- local folders = split(path,"/")
  return shorten(path, maxLen, "..")
end

local function findNextUnusedKey(keysInUse, prefix)
  local index = 1
  local key = prefix .. index
  while keysInUse[key] do
    index = index + 1
    key = prefix .. index
  end
  return key
end

local function spaceWrap(text, width)
  text = text:gsub("(" .. ("."):rep(width) .. ")", "%1 ")
  return text
end

local function shallowCopy(orig)
  local orig_type = type(orig)
  local copy
  if orig_type == 'table' then
    copy = {}
    for orig_key, orig_value in pairs(orig) do copy[orig_key] = orig_value end
  else -- number, string, boolean, etc
    copy = orig
  end
  return copy
end

local function deepCopy(orig)
  local orig_type = type(orig)
  local copy
  if orig_type == 'table' then
    copy = {}
    for orig_key, orig_value in pairs(orig) do
      copy[orig_key] = deepCopy(orig_value)
    end
  else -- number, string, boolean, etc
    copy = orig
  end
  return copy
end

local function round(num, numDecimalPlaces)
  local mult = 10 ^ (numDecimalPlaces or 0)
  return math.floor(num * mult + 0.5) / mult
end

local function removeValueFromArray(t, value)
  local j = 1
  local n = #t;

  for i = 1, n do
    if t[i] == value then
      t[i] = nil;
    else
      -- Move i's kept value to j's position, if it's not already there.
      if (i ~= j) then
        t[j] = t[i];
        t[i] = nil;
      end
      j = j + 1; -- Increment position of where we'll place the next kept value.
    end
  end

  return t;
end

local function removeValuesFromArray(t, valueHash)
  local j = 1
  local n = #t;

  for i = 1, n do
    if valueHash[t[i]] then
      t[i] = nil;
    else
      -- Move i's kept value to j's position, if it's not already there.
      if (i ~= j) then
        t[j] = t[i];
        t[i] = nil;
      end
      j = j + 1; -- Increment position of where we'll place the next kept value.
    end
  end

  return t;
end

--[[
Ordered table iterator, allow to iterate on the natural order of the keys of a
table.

Example:
]]

local function genOrderedIndex(t)
  local orderedIndex = {}
  for key in pairs(t) do table.insert(orderedIndex, key) end
  table.sort(orderedIndex)
  return orderedIndex
end

local function orderedNext(t, state)
  -- Equivalent of the next function, but returns the keys in the alphabetic
  -- order. We use a temporary ordered key table that is stored in the
  -- table being iterated.

  local key = nil
  -- print("orderedNext: state = "..tostring(state) )
  if state == nil then
    -- the first time, generate the index
    t.__orderedIndex = genOrderedIndex(t)
    key = t.__orderedIndex[1]
  else
    -- fetch the next value
    for i = 1, #(t.__orderedIndex) do
      if t.__orderedIndex[i] == state then key = t.__orderedIndex[i + 1] end
    end
  end

  if key then return key, t[key] end

  -- no more value to return, cleanup
  t.__orderedIndex = nil
  return
end

local function orderedPairs(t)
  -- Equivalent of the pairs() function on tables. Allows to iterate
  -- in order
  return orderedNext, t, nil
end

return {
  pp = pp,
  duration = duration,
  durationNoSpace = durationNoSpace,
  timestamp = timestamp,
  removeExtension = removeExtension,
  shorten = shorten,
  shortenPath = shortenPath,
  split = split,
  trim = trim,
  endsWith = endsWith,
  startsWith = startsWith,
  firstToUpper = firstToUpper,
  findNextUnusedKey = findNextUnusedKey,
  spaceWrap = spaceWrap,
  shallowCopy = shallowCopy,
  deepCopy = deepCopy,
  round = round,
  removeValueFromArray = removeValueFromArray,
  removeValuesFromArray = removeValuesFromArray,
  orderedPairs = orderedPairs
}
