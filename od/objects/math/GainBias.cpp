#include <od/objects/math/GainBias.h>
#include <od/extras/LookupTables.h>
#include <hal/simd.h>
#include <od/config.h>
#include <math.h>

namespace od
{

  GainBias::GainBias()
  {
    addInput(mInput);
    addOutput(mOutput);
    addParameter(mGain);
    addParameter(mBias);
  }

  GainBias::~GainBias()
  {
  }

  void GainBias::process()
  {
    float gain0 = mPreviousGain;
    float bias0 = mPreviousBias;
    float32x4_t _gain0 = vdupq_n_f32(gain0);
    float32x4_t _bias0 = vdupq_n_f32(bias0);

    float gain1 = mGain.value();
    float bias1 = mBias.value();
    float32x4_t _gain1 = vdupq_n_f32(gain1);
    float32x4_t _bias1 = vdupq_n_f32(bias1);

    float *in = mInput.buffer();
    float *out = mOutput.buffer();

    if (fabs(gain1 - gain0) < 1e-8f && fabs(bias1 - bias0) < 1e-8f)
    {
      if (mInput.isConstant() || fabs(gain1) < 1e-10f)
      {
        simd_set(out, FRAMELENGTH, in[0] * gain1 + bias1);
        mOutput.mIsConstant = true;
      }
      else
      {
        for (int i = 0; i < FRAMELENGTH; i += 4)
        {
          vst1q_f32(out + i, vld1q_f32(in + i) * _gain1 + _bias1);
        }
        mOutput.mIsConstant = false;
      }
    }
    else
    {
      float *ramp = LookupTables::FrameOfLinearRamp.mValues.data();
      float32x4_t one = vdupq_n_f32(1);
      for (int i = 0; i < FRAMELENGTH; i += 4)
      {
        float32x4_t w1 = vld1q_f32(ramp + i);
        float32x4_t w0 = one - w1;

        float32x4_t g = w0 * _gain0 + w1 * _gain1;
        float32x4_t b = w0 * _bias0 + w1 * _bias1;
        vst1q_f32(out + i, vld1q_f32(in + i) * g + b);
      }
      mOutput.mIsConstant = false;
    }

    mPreviousGain = gain1;
    mPreviousBias = bias1;
  }

} /* namespace od */
