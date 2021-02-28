local function run()
  error("Faking an error in Lua.")
end

return {
  description = "Fake Crash in Lua",
  batch = false,
  run = run
}
