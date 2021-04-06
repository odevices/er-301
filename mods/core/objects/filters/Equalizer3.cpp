// Adapted from: http://www.musicdsp.org/showArchiveComment.php?ArchiveID=236
//----------------------------------------------------------------------------
//
// 3 Band EQ :)
//
// EQ.C - Main Source file for 3 band EQ
//
// (c) Neil C / Etanza Systems / 2K6
//
// Shouts / Loves / Moans = etanza at lycos dot co dot uk
//
// This work is hereby placed in the public domain for all purposes, including
// use in commercial applications.
//
// The author assumes NO RESPONSIBILITY for any problems caused by the use of
// this software.
//
//----------------------------------------------------------------------------
// NOTES :
//
// - Original filter code by Paul Kellet (musicdsp.pdf)
//
// - Uses 4 first order filters in series, should give 24dB per octave
//
// - Now with P4 Denormal fix :)
#include <core/objects/filters/Equalizer3.h>
#include <od/config.h>
#include <hal/simd.h>
#include <math.h>

namespace od
{

  Equalizer3::Equalizer3()
  {
    addInput(mInput);
    addOutput(mOutput);
    addInput(mLowGain);
    addInput(mMidGain);
    addInput(mHighGain);
    addParameter(mLowFreq);
    addParameter(mHighFreq);
    lhf[0] = 2 * sinf(M_PI * (mLowFreq.value() * globalConfig.samplePeriod));
    lhf[1] = 2 * sinf(M_PI * (mHighFreq.value() * globalConfig.samplePeriod));
#if EQ3_USE_NEON
    p0 = vdup_n_f32(0);
    p1 = vdup_n_f32(0);
    p2 = vdup_n_f32(0);
    p3 = vdup_n_f32(0);
#endif
  }

  Equalizer3::~Equalizer3()
  {
  }

#if EQ3_USE_NEON
  // using Gain Parameters
  // [EQ3Unit]: 0.8540% (22756 ticks, 375 Hz) @ 48kHz
  // using Gain Inputs
  // [EQ3Unit]: 1.0709% (28399 ticks, 377 Hz) @ 48kHz
  void Equalizer3::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();

    // Calculate filter cutoff frequencies

    float lF = mLowFreq.value();
    float hF = mHighFreq.value();
    if (fabs(lF - mLastLowFreq) > 0.1f)
    {
      lhf[0] = 2 * sinf((float)M_PI * (mLowFreq.value() * globalConfig.samplePeriod));
      mLastLowFreq = lF;
    }
    if (fabs(hF - mLastHighFreq) > 0.1f)
    {
      lhf[1] = 2 * sinf((float)M_PI * (mHighFreq.value() * globalConfig.samplePeriod));
      mLastHighFreq = hF;
    }

    float *lg = mLowGain.buffer();
    float *mg = mMidGain.buffer();
    float *hg = mHighGain.buffer();
    float32x2_t z = vld1_f32(lhf);

    for (int i = 0; i < FRAMELENGTH; i++)
    {
      float l, m, h; // Low / Mid / High - Sample Values
      float p[2];
      float32x2_t s = vdup_n_f32(in[i]);
      float32x2_t x;

      // compute the two LPF simultaneously
      x = vsub_f32(s, p0);
      p0 = vmla_f32(p0, z, x);
      x = vsub_f32(p0, p1);
      p1 = vmla_f32(p1, z, x);
      x = vsub_f32(p1, p2);
      p2 = vmla_f32(p2, z, x);
      x = vsub_f32(p2, p3);
      p3 = vmla_f32(p3, z, x);

      vst1_f32(p, p3);
      l = p[0];
      // convert the 2nd LPF to HPF
      h = sdm3 - p[1];
      // Calculate midrange (signal - (low + high))
      m = sdm3 - (h + l);

      // Scale, Combine and store
      l *= lg[i];
      m *= mg[i];
      h *= hg[i];
      out[i] = l + m + h;

      // Shuffle history buffer
      sdm3 = sdm2;
      sdm2 = sdm1;
      sdm1 = in[i];
    }
  }
#else
  void Equalizer3::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float *end = out + globalConfig.frameLength;

    // Calculate filter cutoff frequencies

    float lF = mLowFreq.value();
    float hF = mHighFreq.value();
    if (fabs(lF - mLastLowFreq) > 0.1)
    {
      lhf[0] = 2 * sinf(M_PI * (mLowFreq.value() * globalConfig.samplePeriod));
      mLastLowFreq = lF;
    }
    if (fabs(hF - mLastHighFreq) > 0.1)
    {
      lhf[1] = 2 * sinf(M_PI * (mHighFreq.value() * globalConfig.samplePeriod));
      mLastHighFreq = hF;
    }

    float lg = mLowGain.value();
    float mg = mMidGain.value();
    float hg = mHighGain.value();

    while (out < end)
    {
      float l, m, h; // Low / Mid / High - Sample Values
      float sample = *(in++);

      // Filter #1 (lowpass)

      f1p0 += (lhf[0] * (sample - f1p0));
      f1p1 += (lhf[0] * (f1p0 - f1p1));
      f1p2 += (lhf[0] * (f1p1 - f1p2));
      f1p3 += (lhf[0] * (f1p2 - f1p3));

      l = f1p3;

      // Filter #2 (highpass)

      f2p0 += (lhf[1] * (sample - f2p0));
      f2p1 += (lhf[1] * (f2p0 - f2p1));
      f2p2 += (lhf[1] * (f2p1 - f2p2));
      f2p3 += (lhf[1] * (f2p2 - f2p3));

      h = sdm3 - f2p3;

      // Calculate midrange (signal - (low + high))

      m = sdm3 - (h + l);

      // Scale, Combine and store

      l *= lg;
      m *= mg;
      h *= hg;

      // Shuffle history buffer

      sdm3 = sdm2;
      sdm2 = sdm1;
      sdm1 = sample;

      *(out++) = l + m + h;
    }
  }
#endif

} /* namespace od */
