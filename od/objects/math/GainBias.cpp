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
    float gain1 = mGain.value();
    float bias1 = mBias.value();

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
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          out[i] = in[i] * gain1 + bias1;
        }
        mOutput.mIsConstant = false;
      }
    }
    else
    {
      float *ramp = LookupTables::FrameOfLinearRamp.mValues.data();
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        float w1 = ramp[i];
        float w0 = 1.0f - w1;
        out[i] = in[i] * (w0 * gain0 + w1 * gain1) + (w0 * bias0 + w1 * bias1);
      }
      mOutput.mIsConstant = false;
    }

    mPreviousGain = gain1;
    mPreviousBias = bias1;
  }

} /* namespace od */
