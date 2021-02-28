#include <od/objects/TestOscillator.h>
#include <math.h>
#include <hal/simd.h>
#include <od/config.h>

namespace od
{

  TestOscillator::TestOscillator()
  {
    addInput(mVoltPerOctave);
    addInput(mSync);
    addOutput(mOutput);
    addInput(mFundamental);
    addInput(mPhase);
    addInput(mFeedback);
    addParameter(mInternalPhase);
    mInternalPhase.enableSerialization();
  }

  TestOscillator::~TestOscillator()
  {
  }

  // [Sine Osc]: 0.9717% (26053 ticks, 373 Hz)
  void TestOscillator::process()
  {
    float *octave = mVoltPerOctave.buffer();
    float *out = mOutput.buffer();
    float *sync = mSync.buffer();
    float *freq = mFundamental.buffer();
    float *feedback = mFeedback.buffer();
    float *phase = mPhase.buffer();
    float32x4_t glog2 = vdupq_n_f32(FULLSCALE_IN_VOLTS * logf(2.0f));
    float32x4_t pi2 = vdupq_n_f32(2.0f * M_PI);
    float32x4_t one = vdupq_n_f32(1.0f);
    float32x4_t zero = vdupq_n_f32(0.0f);
    float32x4_t negOne = vdupq_n_f32(-1.0f);
    float32x4_t sr = vdupq_n_f32(globalConfig.samplePeriod);
    float32x4_t last = vld1q_f32(mLast);
    float32x4_t fdbkScale = vdupq_n_f32(0.18f);
    float internalPhase = mInternalPhase.value();

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      // calculate phase increment
      float32x4_t p = vld1q_f32(phase + i);
      float32x4_t dP = sr * vld1q_f32(freq + i);
      float32x4_t q = vld1q_f32(octave + i);
      q = vminq_f32(one, q);
      q = vmaxq_f32(negOne, q);
      q = dP * simd_exp(q * glog2);

      // accumulate phase while handling sync
      float tmp[4];
      uint32_t syncTrue[4];
      float32x4_t s = vld1q_f32(sync + i);
      vst1q_u32(syncTrue, vcgtq_f32(s, zero));
      vst1q_f32(tmp, q);
      for (int j = 0; j < 4; j++)
      {
        internalPhase += tmp[j];
        if (syncTrue[j])
        {
          internalPhase = 0;
        }
        tmp[j] = internalPhase;
      }
      q = vld1q_f32(tmp);

      // clamp and scale feedback amount
      float32x4_t f = vld1q_f32(feedback + i);
      f = vminq_f32(one, f);
      f = vmaxq_f32(negOne, f);
      f *= fdbkScale;

      last = simd_sin((q + p + last * f) * pi2);
      vst1q_f32(out + i, last);
    }

    vst1q_f32(mLast, last);
    internalPhase -= (int)internalPhase;
    mInternalPhase.hardSet(internalPhase);
  }

} /* namespace od */
