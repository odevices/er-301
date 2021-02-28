local function run()
  app.Audio_restart()
end

return {
  description = "Restart Audio System",
  batch = false,
  run = run,
  suppressReset = true
}
