#include <core/objects/oscillators/SawtoothOscillator.h>
#include <math.h>
#include <hal/simd.h>
#include <od/config.h>

namespace od
{

  SawtoothOscillator::SawtoothOscillator()
  {
    addInput(mVoltPerOctave);
    addInput(mSync);
    addOutput(mOutput);
    addInput(mFundamental);
    addParameter(mPhase);
    mPhase.enableSerialization();
  }

  SawtoothOscillator::~SawtoothOscillator()
  {
  }

  // [Aliasing Saw]: 0.7360% (19644 ticks, 374 Hz)
  void SawtoothOscillator::process()
  {
    float *octave = mVoltPerOctave.buffer();
    float *out = mOutput.buffer();
    float *sync = mSync.buffer();
    float *freq = mFundamental.buffer();
    float32x4_t glog2 = vdupq_n_f32(FULLSCALE_IN_VOLTS * logf(2.0f));
    float32x4_t two = vdupq_n_f32(2.0f);
    float32x4_t one = vdupq_n_f32(1.0f);
    float32x4_t zero = vdupq_n_f32(0.0f);
    float32x4_t negOne = vdupq_n_f32(-1.0f);
    float32x4_t sr = vdupq_n_f32(globalConfig.samplePeriod);
    float phase = mPhase.value();

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      // calculate phase increment
      float32x4_t q = vld1q_f32(octave + i);
      float32x4_t dP = sr * vld1q_f32(freq + i);
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
        phase += tmp[j];
        if (syncTrue[j])
        {
          phase = 0;
        }
        tmp[j] = phase;
      }
      q = vld1q_f32(tmp);

      // wrap to [0,1]
      q = vsubq_f32(q, vcvtq_f32_s32(vcvtq_s32_f32(q)));
      q += one;
      q = vsubq_f32(q, vcvtq_f32_s32(vcvtq_s32_f32(q)));
      // calculate ramp
      q = vsubq_f32(two * q, one);
      vst1q_f32(out + i, q);
    }

    phase -= (int)phase;
    mPhase.hardSet(phase);
  }

} /* namespace od */
