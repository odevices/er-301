local units = {
  {
    title = "Mix",
    moduleName = "MixerUnit",
    category = "Essentials",
    keywords = "mixing, routing",
    channelCount = 1
  },
  {
    title = "Test Osc",
    moduleName = "TestOscillator",
    category = "Experimental",
    keywords = "source, pitch, modulate"
  },
  {
    title = "Stereo Mix",
    moduleName = "MixerUnit",
    category = "Essentials",
    keywords = "mixing, routing",
    channelCount = 2
  },
  {
    title = "Mono Mix",
    moduleName = "MonoToStereo",
    category = "Essentials",
    keywords = "mixing, routing",
    channelCount = 2
  },
  {
    title = "Custom Effect",
    moduleName = "Container.Effect",
    category = "Containers",
    keywords = "effect, container",
    aliases = {
      "Custom"
    }
  },
  {
    title = "Custom Source",
    moduleName = "Container.Source",
    category = "Containers",
    keywords = "source, container"
  }
}

return {
  title = "builtins",
  name = "builtins",
  keyword = "builtins",
  units = units
}
