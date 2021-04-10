%module core_libcore
%include <od/glue/mod.cpp.swig>

%{

#undef SWIGLUA

// units

#include <core/units/BandedUnit.h>
#include <core/units/ChainBand.h>

// objects

#include <core/objects/Spread.h>
#include <core/objects/RationalMultiply.h>
#include <core/objects/Limiter.h>
#include <core/objects/Clipper.h>
#include <core/objects/Fold.h>
#include <core/objects/Rectify.h>
#include <core/objects/GridQuantizer.h>
#include <core/objects/SnapToZero.h>
#include <core/objects/Counter.h>

#include <core/objects/random/WhiteNoise.h>
#include <core/objects/random/PinkNoise.h>
#include <core/objects/random/VelvetNoise.h>

#include <core/objects/delays/Delay.h>
#include <core/objects/delays/MicroDelay.h>
#include <core/objects/delays/DopplerDelay.h>

#include <core/objects/timing/RoundRobin.h>
#include <core/objects/timing/TrackAndHold.h>
#include <core/objects/timing/TapTempo.h>
#include <core/objects/timing/Clock.h>
#include <core/objects/timing/QuantizeToClock.h>
#include <core/objects/timing/ZeroCrossingDetector.h>

#include <core/objects/heads/RawHead.h>
#include <core/objects/heads/LoopHead.h>
#include <core/objects/heads/VariSpeedHead.h>
#include <core/objects/heads/RecordHead.h> 

#include <core/objects/control/BumpMap.h>

#include <core/objects/looping/PedalLooper.h>
#include <core/objects/looping/FeedbackLooper.h>
#include <core/objects/looping/DubLooper.h>

#include <core/objects/granular/GranularHead.h>
#include <core/objects/granular/GrainStretch.h>
#include <core/objects/granular/MonoGrainDelay.h>

#include <core/objects/env/ADSR.h>
#include <core/objects/env/SkewedSineEnvelope.h>
#include <core/objects/env/PopReducer.h>
#include <core/objects/env/EnvelopeFollower.h>

#include <core/objects/pitch/VoltPerOctave.h>
#include <core/objects/pitch/ScaleQuantizer.h>

#include <core/objects/filters/FixedFilter.h>
#include <core/objects/filters/DeadbandFilter.h>
#include <core/objects/filters/Equalizer3.h>
#include <core/objects/filters/AllPassFilter.h>
#include <core/objects/filters/LadderFilter.h>
#include <core/objects/filters/StereoLadderFilter.h>
#include <core/objects/filters/StereoLadderHPF.h>
#include <core/objects/filters/StereoFixedHPF.h>
#include <core/objects/filters/MonoConvolution.h>
#include <core/objects/filters/StereoConvolution.h>
#include <core/objects/filters/SlewLimiter.h>

#include <core/objects/oscillators/SineOscillator.h>
#include <core/objects/oscillators/TriangleOscillator.h>
#include <core/objects/oscillators/SawtoothOscillator.h>
#include <core/objects/oscillators/SingleCycle.h>

#include <core/objects/wavetable/OverlapAddTest.h>
#include <core/objects/freeverb/Freeverb.h>

// graphics

#include <core/graphics/PitchCircle.h>
#include <core/graphics/GranularHeadDisplay.h>
#include <core/graphics/RecordHeadDisplay.h>
	
#define SWIGLUA

%}

// units

%include <core/units/BandedUnit.h>
%include <core/units/ChainBand.h>

// objects

%include <core/objects/Spread.h>
%include <core/objects/RationalMultiply.h>
%include <core/objects/Limiter.h>
%include <core/objects/Clipper.h>
%include <core/objects/Fold.h>
%include <core/objects/Rectify.h>
%include <core/objects/GridQuantizer.h>
%include <core/objects/SnapToZero.h>
%include <core/objects/Counter.h>

%include <core/objects/random/WhiteNoise.h>
%include <core/objects/random/PinkNoise.h>
%include <core/objects/random/VelvetNoise.h>

%include <core/objects/timing/RoundRobin.h>
%include <core/objects/timing/TrackAndHold.h>
%include <core/objects/timing/TapTempo.h>
%include <core/objects/timing/Clock.h>
%include <core/objects/timing/QuantizeToClock.h>
%include <core/objects/timing/ZeroCrossingDetector.h>

%include <core/objects/control/BumpMap.h>

%include <core/objects/heads/RawHead.h>
%include <core/objects/heads/LoopHead.h>
%include <core/objects/heads/VariSpeedHead.h>
%include <core/objects/heads/RecordHead.h> 

%include <core/objects/looping/PedalLooper.h>
%include <core/objects/looping/FeedbackLooper.h>
%include <core/objects/looping/DubLooper.h>

%include <core/objects/granular/GranularHead.h>
%include <core/objects/granular/GrainStretch.h>
%include <core/objects/granular/MonoGrainDelay.h>

%include <core/objects/env/ADSR.h>
%include <core/objects/env/SkewedSineEnvelope.h>
%include <core/objects/env/PopReducer.h>
%include <core/objects/env/EnvelopeFollower.h>

%include <core/objects/pitch/VoltPerOctave.h>
%include <core/objects/pitch/ScaleQuantizer.h>

%include <core/objects/delays/Delay.h>
%include <core/objects/delays/MicroDelay.h>
%include <core/objects/delays/DopplerDelay.h>

%include <core/objects/filters/FixedFilter.h>
%include <core/objects/filters/DeadbandFilter.h>
%include <core/objects/filters/Equalizer3.h>
%include <core/objects/filters/LadderFilter.h>
%include <core/objects/filters/StereoLadderFilter.h>
%include <core/objects/filters/StereoLadderHPF.h>
%include <core/objects/filters/StereoFixedHPF.h>
%include <core/objects/filters/MonoConvolution.h>
%include <core/objects/filters/StereoConvolution.h>
%include <core/objects/filters/SlewLimiter.h>

%include <core/objects/oscillators/SineOscillator.h>
%include <core/objects/oscillators/TriangleOscillator.h>
%include <core/objects/oscillators/SawtoothOscillator.h>
%include <core/objects/oscillators/SingleCycle.h>

%include <core/objects/freeverb/Freeverb.h>
%include <core/objects/wavetable/OverlapAddTest.h>

// graphics

%include <core/graphics/PitchCircle.h>
%include <core/graphics/GranularHeadDisplay.h>
%include <core/graphics/RecordHeadDisplay.h>




