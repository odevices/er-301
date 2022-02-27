local Utils = require "Utils"

local function getValue(str, pattern)
  local first, last, value = string.find(str, pattern)
  return value
end

local toCents = 1200 / math.log(2)

--------------------------------------------------------------------------------
-- Function to load a Scala .scl file and generate a table of notes in cents
-- Based on work by Kieran Foster (aka dblue). January 26th, 2011.
-- Scala .scl file format: http://www.huygens-fokker.org/scala/scl_format.html
local function load(scl_filename, headerOnly)
  -- Table to hold converted tunings.
  local tunings = {}
  local description
  local count
  -- Counter to keep track of lines that actually have useful content.
  local lc = 0
  -- Attempt to open the .scl file for read access.
  local file = io.open(scl_filename, 'r')
  -- If we managed to open the file.
  if file then
    -- Iterate through each line.
    for line in file:lines() do
      -- If the current line is *not* a comment and is not a blank line...
      if (string.sub(line, 1, 1) ~= '!') and
          (string.len(string.gsub(line, ' ', '')) > 0) then
        -- Increment line counter.
        lc = lc + 1
        -- According to the .scl file format, the first uncommented line we
        -- encounter should be the description.
        if lc == 1 then
          -- You could store the description if you wanted to use it later.
          description = Utils.trim(line)
          -- The second uncommented line we encounter should be the note count.
        elseif lc == 2 then
          -- You could store the number of notes if you wanted to use it later.
          count = tonumber(line)
          if headerOnly then
            file:close()
            return {
              count = count,
              description = description
            }
          end
          -- All other uncommented lines should be valid tunings.
        else
          -- If the tuning contains a period (.) then it's in cents. ex: 100.0
          if string.find(line, '.', 1, true) then
            -- Extract cents value from the line.
            local cents = tonumber(getValue(line, '(-*%d+.%d*)'))
            -- Convert.
            table.insert(tunings, cents)
            -- If the tuning contains a forward slash (/) then it's a ratio. ex: 2/1
          elseif string.find(line, '/', 1, true) then
            -- Extract ratio value from the line.
            local ratio = getValue(line, '(-*%d+/%d+)')
            -- Split the ratio into its component parts.
            local ratio_parts = Utils.split(line, '/')
            -- Convert.
            table.insert(tunings, toCents *
                             math.log(
                                 tonumber(ratio_parts[1]) /
                                     tonumber(ratio_parts[2])))
            -- Otherwise, the tuning is probably a whole integer value. ex: 2
          else
            -- Extract value.
            local value = tonumber(getValue(line, '(-*%d+)'))
            -- Convert.
            table.insert(tunings, toCents * math.log(value))
          end
        end
      end
    end
    -- Close the .scl file when we're finished with it.
    file:close()
  end

  return {
    tunings = tunings,
    count = count or #tunings,
    description = description or "unknown"
  }
end

return {
  load = load
}
