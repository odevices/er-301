local units = {
  {
    category = "Essentials"
  },
  {
    title = "Mix",
    moduleName = "MixerUnit",
    keywords = "mixing, routing",
    channelCount = 1
  },
  {
    title = "Test Osc",
    moduleName = "TestOscillator",
    keywords = "source, pitch, modulate",
  },
  {
    title = "Stereo Mix",
    moduleName = "MixerUnit",
    keywords = "mixing, routing",
    channelCount = 2
  },
  {
    title = "Mono Mix",
    moduleName = "MonoToStereo",
    keywords = "mixing, routing",
    channelCount = 2
  },
  {
    category = "Containers"
  },
  {
    title = "Custom Effect",
    moduleName = "Container.Effect",
    keywords = "effect, container",
    aliases = {
      "Custom"
    }
  },
  {
    title = "Custom Source",
    moduleName = "Container.Source",
    keywords = "source, container"
  }
}

return {
  title = "builtins",
  name = "builtins",
  keyword = "builtins",
  units = units
}
