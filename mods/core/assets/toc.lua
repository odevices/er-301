local units = {
  {
    title = "Linear Bipolar VCA",
    moduleName = "VCA.LinearBipolar",
    category = "Essentials",
    keywords = "modulate",
    aliases = {
      "LinearVcaUnit"
    }
  },
  {
    title = "Linear Unipolar VCA",
    moduleName = "VCA.LinearUnipolar",
    category = "Essentials",
    keywords = "modulate"
  },
  {
    title = "Limiter",
    moduleName = "LimiterUnit",
    category = "Essentials",
    keywords = "effect, mixing"
  },
  {
    title = "Offset",
    moduleName = "OffsetUnit",
    category = "Essentials",
    keywords = ""
  },
  {
    title = "2 Bands",
    moduleName = "Container.TwoBands",
    category = "Containers",
    keywords = "effect, container",
    aliases = {
      "Custom.TwoBands"
    }
  },
  {
    title = "3 Bands",
    moduleName = "Container.ThreeBands",
    category = "Containers",
    keywords = "effect, container",
    aliases = {
      "Custom.ThreeBands"
    }
  },
  {
    title = "4 Bands",
    moduleName = "Container.FourBands",
    category = "Containers",
    keywords = "effect, container",
    aliases = {
      "Custom.FourBands"
    }
  },
  {
    title = "5 Bands",
    moduleName = "Container.FiveBands",
    category = "Containers",
    keywords = "effect, container",
    aliases = {
      "Custom.FiveBands"
    }
  },
  {
    title = "6 Bands",
    moduleName = "Container.SixBands",
    category = "Containers",
    keywords = "effect, container",
    aliases = {
      "Custom.SixBands"
    }
  },
  {
    title = "Variable Speed Player",
    moduleName = "Player.VariSpeed",
    category = "Sample Playback",
    keywords = "sampling, source",
    aliases = {
      "Player.PlayerUnit"
    }
  },
  {
    title = "Raw Player",
    moduleName = "Player.Raw",
    category = "Sample Playback",
    keywords = "sampling, source",
    aliases = {
      "Player.NativeSpeedPlayerUnit",
      "Player.NativeSpeed"
    }
  },
  {
    title = "Card Player",
    moduleName = "File.CardPlayerUnit",
    category = "Sample Playback",
    keywords = "source, sampling"
  },
  {
    title = "Clocked Player",
    moduleName = "Player.ClockedVariSpeed",
    keywords = "sampling, source"
  },
  {
    title = "Manual Loops",
    moduleName = "Player.ManualLoops",
    category = "Sample Playback",
    keywords = "sampling, source"
  },
  {
    title = "Manual Grains",
    moduleName = "Player.ManualGrains",
    category = "Granular Playback",
    keywords = "sampling, source"
  },
  {
    title = "Grain Stretch",
    moduleName = "Player.GrainStretch",
    category = "Granular Playback",
    keywords = "sampling, source",
    aliases = {
      "Player.GrainPlayer"
    }
  },
  {
    title = "Clocked Stretch",
    moduleName = "Player.ClockedStretch",
    category = "Granular Playback",
    keywords = "sampling, source"
  },
  {
    title = "Feedback Looper",
    moduleName = "Looper.FeedbackLooper",
    category = "Recording and Looping",
    keywords = "sampling, effect"
  },
  {
    title = "Dub Looper",
    moduleName = "Looper.DubLooper",
    category = "Recording and Looping",
    keywords = "sampling, effect",
    aliases = {
      "Looper.LooperUnit"
    }
  },
  {
    title = "Pedal Looper",
    moduleName = "Looper.PedalLooper",
    category = "Recording and Looping",
    keywords = "sampling, effect"
  },
  {
    title = "Micro Delay",
    moduleName = "Delay.MicroDelay",
    category = "Delays and Reverb",
    keywords = "delay"
  },
  {
    title = "Delay",
    moduleName = "Delay.DelayUnit",
    category = "Delays and Reverb",
    keywords = "delay, effect",
    aliases = {
      "Delay.FixedDelayUnit"
    }
  },
  {
    title = "Spread Delay",
    moduleName = "Delay.SpreadDelayUnit",
    category = "Delays and Reverb",
    keywords = "delay, effect",
    channelCount = 2
  },
  {
    title = "Clocked Delay",
    moduleName = "Delay.ClockedDelayUnit",
    category = "Delays and Reverb",
    keywords = "delay, effect"
  },
  {
    title = "Doppler Delay",
    moduleName = "Delay.DopplerDelayUnit",
    category = "Delays and Reverb",
    keywords = "delay, effect",
    aliases = {
      "Delay.VariableDelayUnit"
    }
  },
  {
    title = "Clocked Doppler Delay",
    moduleName = "Delay.ClockedDopplerDelay",
    category = "Delays and Reverb",
    keywords = "delay, effect"
  },
  {
    title = "Grain Delay",
    moduleName = "Delay.GrainDelayUnit",
    category = "Delays and Reverb",
    keywords = "delay, effect"
  },
  {
    title = "Freeverb",
    moduleName = "FreeverbUnit",
    category = "Delays and Reverb",
    keywords = "effect"
  },
  {
    title = "Ladder LPF",
    moduleName = "LadderFilterUnit",
    category = "Filtering",
    keywords = "filter, pitch"
  },
  {
    title = "Ladder HPF",
    moduleName = "LadderHPFUnit",
    category = "Filtering",
    keywords = "filter, pitch"
  },
  {
    title = "EQ3",
    moduleName = "EQ3Unit",
    category = "Filtering",
    keywords = "filter, mixing"
  },
  {
    title = "Fixed HPF",
    moduleName = "FixedHPFUnit",
    category = "Filtering",
    keywords = "filter"
  },
  {
    title = "Slew Limiter",
    moduleName = "SlewLimiter",
    category = "Filtering",
    keywords = "filter"
  },
  {
    title = "Exact Convolution",
    moduleName = "ConvolutionUnit",
    category = "Filtering",
    keywords = "filter, effect"
  },
  {
    title = "Deadband Filter",
    moduleName = "DeadbandFilter",
    category = "Filtering",
    keywords = "filter"
  },
  {
    title = "Sine Osc",
    moduleName = "Oscillators.Sine",
    category = "Oscillators",
    keywords = "source, pitch, modulate",
    aliases = {
      "SineOscillatorUnit"
    }
  },
  {
    title = "Aliasing Triangle",
    moduleName = "Oscillators.AliasingTriangle",
    category = "Oscillators",
    keywords = "source, pitch, modulate",
    aliases = {
      "AliasingTriangleUnit"
    }
  },
  {
    title = "Aliasing Saw",
    moduleName = "Oscillators.AliasingSaw",
    category = "Oscillators",
    keywords = "source, pitch, modulate",
    aliases = {
      "AliasingSawUnit"
    }
  },
  {
    title = "Single Cycle",
    moduleName = "Oscillators.SingleCycle",
    category = "Oscillators",
    keywords = "source, pitch, modulate"
  },
  {
    title = "White Noise",
    moduleName = "Noise.White",
    category = "Noise",
    keywords = "source, noise",
    aliases = {
      "Noise.WhiteNoiseUnit"
    }
  },
  {
    title = "Pink Noise",
    moduleName = "Noise.Pink",
    category = "Noise",
    keywords = "source, noise",
    aliases = {
      "Noise.PinkNoiseUnit"
    }
  },
  {
    title = "Velvet Noise",
    moduleName = "Noise.Velvet",
    category = "Noise",
    keywords = "source, noise",
    aliases = {
      "Noise.VelvetNoiseUnit"
    }
  },
  {
    title = "ADSR",
    moduleName = "Envelopes.ADSR",
    category = "Envelopes",
    keywords = "modulate, source",
    aliases = {
      "ADSRUnit"
    }
  },
  {
    title = "Skewed Sine Env",
    moduleName = "Envelopes.Sine",
    category = "Envelopes",
    keywords = "modulate, source",
    aliases = {
      "SineEnvelopeUnit"
    }
  },
  {
    title = "Envelope Follower",
    moduleName = "Envelopes.Follower",
    category = "Envelopes",
    keywords = "modulate, measure",
    aliases = {
      "EnvelopeFollowerUnit"
    }
  },
  {
    title = "Scale Quantizer",
    moduleName = "Quantizer.ScaleQuantizerUnit",
    category = "Mapping and Control",
    keywords = "pitch"
  },
  {
    title = "Grid Quantizer",
    moduleName = "Quantizer.GridQuantizerUnit",
    category = "Mapping and Control",
    keywords = "effect",
    aliases = {
      "QuantizerUnit"
    }
  },
  {
    title = "Sample & Hold",
    moduleName = "SampleHoldUnit",
    category = "Mapping and Control",
    keywords = "effect, modulate"
  },
  {
    title = "Track & Hold",
    moduleName = "TrackHoldUnit",
    category = "Mapping and Control",
    keywords = "effect, modulate"
  },
  {
    title = "Bump Scanner",
    moduleName = "BumpMap",
    category = "Mapping and Control",
    keywords = "effect"
  },
  {
    title = "Sample Scanner",
    moduleName = "SampleScanner",
    category = "Mapping and Control",
    keywords = "effect, sampling"
  },
  {
    title = "Rectify",
    moduleName = "RectifierUnit",
    category = "Mapping and Control",
    keywords = ""
  },
  {
    title = "Fold",
    moduleName = "FoldUnit",
    category = "Mapping and Control",
    keywords = "effect"
  },
  {
    title = "Rational VCA",
    moduleName = "VCA.Rational",
    category = "Mapping and Control",
    keywords = "modulate",
    aliases = {
      "RationalVcaUnit"
    }
  },
  {
    title = "Counter",
    moduleName = "CounterUnit",
    category = "Mapping and Control",
    keywords = ""
  },
  {
    title = "Clock (sec)",
    moduleName = "Timing.ClockInSeconds",
    category = "Timing",
    keywords = "source, timing"
  },
  {
    title = "Clock (BPM)",
    moduleName = "Timing.ClockInBPM",
    category = "Timing",
    keywords = "source, timing"
  },
  {
    title = "Clock (Hz)",
    moduleName = "Timing.ClockInHertz",
    category = "Timing",
    keywords = "source, timing"
  },
  {
    title = "Quantize to Clock",
    moduleName = "Timing.QuantizeToClock",
    category = "Timing",
    keywords = "effect, timing"
  },
  {
    title = "Tap Tempo",
    moduleName = "Timing.TapTempoUnit",
    category = "Timing",
    keywords = "measure, source, timing",
    aliases = {
      "TapTempoUnit"
    }
  },
  {
    title = "Pulse to Seconds",
    moduleName = "Timing.PulseToSeconds",
    category = "Measurement and Conversion",
    keywords = "measure, timing",
    aliases = {
      "PeriodMeterUnit"
    }
  },
  {
    title = "Pulse to Hertz",
    moduleName = "Timing.PulseToFrequency",
    category = "Measurement and Conversion",
    keywords = "measure, timing",
    aliases = {
      "PulseToFrequency"
    }
  },
  {
    title = "V/oct to Seconds",
    moduleName = "Converters.VoltPerOctaveToSeconds",
    category = "Measurement and Conversion",
    keywords = "pitch"
  },
  {
    title = "V/oct to Hertz",
    moduleName = "Converters.VoltPerOctaveToHertz",
    category = "Measurement and Conversion",
    keywords = "pitch"
  },
  {
    title = "Stress",
    moduleName = "StressUnit",
    category = "Experimental",
    keywords = "debug"
  },
  {
    title = "Countdown",
    moduleName = "Research.Countdown",
    category = "Experimental",
    keywords = "timing"
  },
  {
    title = "Schroeder Allpass",
    moduleName = "SchroederAllPass",
    category = "Experimental",
    keywords = "filter"
  }
}

local function find(moduleName)
  for i, e in ipairs(units) do if e.moduleName == moduleName then return i end end
end

if app.globalConfig.frameLength < 64 then
  -- remove the Convolution Unit from the low-latency firmware
  local index = find("ConvolutionUnit")
  if index then table.remove(units, index) end
end

return {
  title = "Core Library",
  name = "core",
  keyword = "core",
  author = "O|D",
  units = units
}
