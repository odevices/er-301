local units = {
  {
    category = "Essentials"
  },
  {
    title = "Linear Bipolar VCA",
    moduleName = "VCA.LinearBipolar",
    keywords = "modulate",
    aliases = {
      "LinearVcaUnit"
    }
  },
  {
    title = "Linear Unipolar VCA",
    moduleName = "VCA.LinearUnipolar",
    keywords = "modulate"
  },
  {
    title = "Limiter",
    moduleName = "LimiterUnit",
    keywords = "effect, mixing"
  },
  {
    title = "Offset",
    moduleName = "OffsetUnit",
    keywords = ""
  },
  {
    category = "Containers"
  },
  {
    title = "2 Bands",
    moduleName = "Container.TwoBands",
    keywords = "effect, container",
    aliases = {
      "Custom.TwoBands"
    }
  },
  {
    title = "3 Bands",
    moduleName = "Container.ThreeBands",
    keywords = "effect, container",
    aliases = {
      "Custom.ThreeBands"
    }
  },
  {
    title = "4 Bands",
    moduleName = "Container.FourBands",
    keywords = "effect, container",
    aliases = {
      "Custom.FourBands"
    }
  },
  {
    title = "5 Bands",
    moduleName = "Container.FiveBands",
    keywords = "effect, container",
    aliases = {
      "Custom.FiveBands"
    }
  },
  {
    title = "6 Bands",
    moduleName = "Container.SixBands",
    keywords = "effect, container",
    aliases = {
      "Custom.SixBands"
    }
  },
  {
    category = "Sample Playback"
  },
  {
    title = "Variable Speed Player",
    moduleName = "Player.VariSpeed",
    keywords = "sampling, source",
    aliases = {
      "Player.PlayerUnit"
    }
  },
  {
    title = "Raw Player",
    moduleName = "Player.Raw",
    keywords = "sampling, source",
    aliases = {
      "Player.NativeSpeedPlayerUnit",
      "Player.NativeSpeed"
    }
  },
  {
    title = "Card Player",
    moduleName = "File.CardPlayerUnit",
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
    keywords = "sampling, source"
  },
  {
    category = "Granular Playback"
  },
  {
    title = "Manual Grains",
    moduleName = "Player.ManualGrains",
    keywords = "sampling, source"
  },
  {
    title = "Grain Stretch",
    moduleName = "Player.GrainStretch",
    keywords = "sampling, source",
    aliases = {
      "Player.GrainPlayer"
    }
  },
  {
    title = "Clocked Stretch",
    moduleName = "Player.ClockedStretch",
    keywords = "sampling, source"
  },
  {
    category = "Recording and Looping"
  },
  {
    title = "Feedback Looper",
    moduleName = "Looper.FeedbackLooper",
    keywords = "sampling, effect"
  },
  {
    title = "Dub Looper",
    moduleName = "Looper.DubLooper",
    keywords = "sampling, effect",
    aliases = {
      "Looper.LooperUnit"
    }
  },
  {
    title = "Pedal Looper",
    moduleName = "Looper.PedalLooper",
    keywords = "sampling, effect"
  },
  {
    category = "Delays and Reverb"
  },
  {
    title = "Micro Delay",
    moduleName = "Delay.MicroDelay",
    keywords = "delay"
  },
  {
    title = "Delay",
    moduleName = "Delay.DelayUnit",
    keywords = "delay, effect",
    aliases = {
      "Delay.FixedDelayUnit"
    }
  },
  {
    title = "Spread Delay",
    moduleName = "Delay.SpreadDelayUnit",
    keywords = "delay, effect",
    channelCount = 2
  },
  {
    title = "Clocked Delay",
    moduleName = "Delay.ClockedDelayUnit",
    keywords = "delay, effect"
  },
  {
    title = "Doppler Delay",
    moduleName = "Delay.DopplerDelayUnit",
    keywords = "delay, effect",
    aliases = {
      "Delay.VariableDelayUnit"
    }
  },
  {
    title = "Clocked Doppler Delay",
    moduleName = "Delay.ClockedDopplerDelay",
    keywords = "delay, effect"
  },
  {
    title = "Grain Delay",
    moduleName = "Delay.GrainDelayUnit",
    keywords = "delay, effect"
  },
  {
    title = "Freeverb",
    moduleName = "FreeverbUnit",
    keywords = "effect"
  },
  {
    category = "Filtering"
  },
  {
    title = "Ladder LPF",
    moduleName = "LadderFilterUnit",
    keywords = "filter, pitch"
  },
  {
    title = "Ladder HPF",
    moduleName = "LadderHPFUnit",
    keywords = "filter, pitch"
  },
  {
    title = "EQ3",
    moduleName = "EQ3Unit",
    keywords = "filter, mixing"
  },
  {
    title = "Fixed HPF",
    moduleName = "FixedHPFUnit",
    keywords = "filter"
  },
  {
    title = "Slew Limiter",
    moduleName = "SlewLimiter",
    keywords = "filter"
  },
  {
    title = "Exact Convolution",
    moduleName = "ConvolutionUnit",
    keywords = "filter, effect"
  },
  {
    title = "Deadband Filter",
    moduleName = "DeadbandFilter",
    keywords = "filter"
  },
  {
    category = "Oscillators"
  },
  {
    title = "Sine Osc",
    moduleName = "Oscillators.Sine",
    keywords = "source, pitch, modulate",
    aliases = {
      "SineOscillatorUnit"
    }
  },
  {
    title = "Aliasing Triangle",
    moduleName = "Oscillators.AliasingTriangle",
    keywords = "source, pitch, modulate",
    aliases = {
      "AliasingTriangleUnit"
    }
  },
  {
    title = "Aliasing Saw",
    moduleName = "Oscillators.AliasingSaw",
    keywords = "source, pitch, modulate",
    aliases = {
      "AliasingSawUnit"
    }
  },
  {
    title = "Single Cycle",
    moduleName = "Oscillators.SingleCycle",
    keywords = "source, pitch, modulate"
  },
  {
    category = "Noise"
  },
  {
    title = "White Noise",
    moduleName = "Noise.White",
    keywords = "source, noise",
    aliases = {
      "Noise.WhiteNoiseUnit"
    }
  },
  {
    title = "Pink Noise",
    moduleName = "Noise.Pink",
    keywords = "source, noise",
    aliases = {
      "Noise.PinkNoiseUnit"
    }
  },
  {
    title = "Velvet Noise",
    moduleName = "Noise.Velvet",
    keywords = "source, noise",
    aliases = {
      "Noise.VelvetNoiseUnit"
    }
  },
  {
    category = "Envelopes"
  },
  {
    title = "ADSR",
    moduleName = "Envelopes.ADSR",
    keywords = "modulate, source",
    aliases = {
      "ADSRUnit"
    }
  },
  {
    title = "Skewed Sine Env",
    moduleName = "Envelopes.Sine",
    keywords = "modulate, source",
    aliases = {
      "SineEnvelopeUnit"
    }
  },
  {
    title = "Envelope Follower",
    moduleName = "Envelopes.Follower",
    keywords = "modulate, measure",
    aliases = {
      "EnvelopeFollowerUnit"
    }
  },
  {
    category = "Mapping and Control"
  },
  {
    title = "Scale Quantizer",
    moduleName = "Quantizer.ScaleQuantizerUnit",
    keywords = "pitch"
  },
  {
    title = "Grid Quantizer",
    moduleName = "Quantizer.GridQuantizerUnit",
    keywords = "effect",
    aliases = {
      "QuantizerUnit"
    }
  },
  {
    title = "Sample & Hold",
    moduleName = "SampleHoldUnit",
    keywords = "effect, modulate"
  },
  {
    title = "Track & Hold",
    moduleName = "TrackHoldUnit",
    keywords = "effect, modulate"
  },
  {
    title = "Bump Scanner",
    moduleName = "BumpMap",
    keywords = "effect"
  },
  {
    title = "Sample Scanner",
    moduleName = "SampleScanner",
    keywords = "effect, sampling"
  },
  {
    title = "Rectify",
    moduleName = "RectifierUnit",
    keywords = ""
  },
  {
    title = "Fold",
    moduleName = "FoldUnit",
    keywords = "effect"
  },
  {
    title = "Rational VCA",
    moduleName = "VCA.Rational",
    keywords = "modulate",
    aliases = {
      "RationalVcaUnit"
    }
  },
  {
    title = "Counter",
    moduleName = "CounterUnit",
    keywords = ""
  },
  {
    category = "Timing"
  },
  {
    title = "Clock (sec)",
    moduleName = "Timing.ClockInSeconds",
    keywords = "source, timing"
  },
  {
    title = "Clock (BPM)",
    moduleName = "Timing.ClockInBPM",
    keywords = "source, timing"
  },
  {
    title = "Clock (Hz)",
    moduleName = "Timing.ClockInHertz",
    keywords = "source, timing"
  },
  {
    title = "Quantize to Clock",
    moduleName = "Timing.QuantizeToClock",
    keywords = "effect, timing"
  },
  {
    title = "Tap Tempo",
    moduleName = "Timing.TapTempoUnit",
    keywords = "measure, source, timing",
    aliases = {
      "TapTempoUnit"
    }
  },
  {
    category = "Measurement and Conversion"
  },
  {
    title = "Pulse to Seconds",
    moduleName = "Timing.PulseToSeconds",
    keywords = "measure, timing",
    aliases = {
      "PeriodMeterUnit"
    }
  },
  {
    title = "Pulse to Hertz",
    moduleName = "Timing.PulseToFrequency",
    keywords = "measure, timing",
    aliases = {
      "PulseToFrequency"
    }
  },
  {
    title = "V/oct to Seconds",
    moduleName = "Converters.VoltPerOctaveToSeconds",
    keywords = "pitch"
  },
  {
    title = "V/oct to Hertz",
    moduleName = "Converters.VoltPerOctaveToHertz",
    keywords = "pitch"
  },
  {
    category = "Experimental"
  },
  {
    title = "Stress",
    moduleName = "StressUnit",
    keywords = "debug"
  },
  {
    title = "Countdown",
    moduleName = "Research.Countdown",
    keywords = "timing"
  },
  {
    title = "Schroeder Allpass",
    moduleName = "SchroederAllPass",
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
