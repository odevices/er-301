/*
 * VoltPerOctave.cpp
 *
 *  Created on: 13 Jun 2016
 *      Author: clarkson
 */

#include <core/objects/pitch/VoltPerOctave.h>
#include <od/config.h>
#include <math.h>

#define USE_POWF 0
#define USE_LUT 0
#define USE_INTRINSICS 1

#if USE_LUT
#include <od/extras/LookupTables.h>
#endif

#if USE_INTRINSICS
#include <hal/simd.h>
#endif

namespace od
{

  VoltPerOctave::VoltPerOctave()
  {
    addInput(mInput);
    addOutput(mOutput);
  }

  VoltPerOctave::~VoltPerOctave()
  {
  }

  void VoltPerOctave::process()
  {
    if (mInput.isConstant())
    {
      float *in = mInput.buffer();
      float *out = mOutput.buffer();

      // cost: 45 cycles/sample
      float32x4_t glog2 = vdupq_n_f32(FULLSCALE_IN_VOLTS) * simd_log(vdupq_n_f32(2.0f));
      float32x4_t one = vdupq_n_f32(1.0f);
      float32x4_t negOne = vdupq_n_f32(-1.0f);
      float32x4_t q = vld1q_f32(in);
      q = vminq_f32(one, q);
      q = vmaxq_f32(negOne, q);
      q = simd_exp(q * glog2);

      for (int i = 0; i < FRAMELENGTH; i += 4)
      {
        vst1q_f32(out + i, q);
      }
    }
    else
    {
      float *in = mInput.buffer();
      float *out = mOutput.buffer();

#if USE_POWF
      // cost: 1560 cycles/sample
      for (int i = 0; i < globalConfig.frameLength; i++)
      {
        out[i] = powf(2.0f, in[i]);
      }
#endif

#if USE_LUT
      // cost is independent of voltage magnitude
      // cost: 200 cycles/sample (190 of those cycles are for the LUT)
      float phase, factor, shifted;
      int twos;
      for (uint32_t i = 0; i < globalConfig.frameLength; i++)
      {
        shifted = FULLSCALE_IN_VOLTS * in[i] + 10.0f;
        twos = (int)shifted;
        phase = shifted - twos;
        factor = (1 << twos);
        factor *= 1.0f / (1 << 10);
        out[i] = factor * LookupTables::VoltPerOctave.getNoCheckInterpolated(phase);
      }
#endif

#if USE_INTRINSICS
      // cost: 45 cycles/sample
      float32x4_t glog2 = vdupq_n_f32(FULLSCALE_IN_VOLTS) * simd_log(vdupq_n_f32(2.0f));
      float32x4_t one = vdupq_n_f32(1.0f);
      float32x4_t negOne = vdupq_n_f32(-1.0f);

      for (int i = 0; i < FRAMELENGTH; i += 4)
      {
        float32x4_t q = vld1q_f32(in + i);
        q = vminq_f32(one, q);
        q = vmaxq_f32(negOne, q);
        q = simd_exp(q * glog2);
        vst1q_f32(out + i, q);
      }
#endif
    }
  }

} /* namespace od */
