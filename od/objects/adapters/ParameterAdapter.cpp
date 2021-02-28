#include <od/objects/adapters/ParameterAdapter.h>
#include <od/extras/LookupTables.h>
#include <hal/simd.h>
#include <od/config.h>
#include <hal/ops.h>
#include <math.h>

namespace od
{

  ParameterAdapter::ParameterAdapter()
  {
    addInput(mInput);
    addOutput(mProbeOutput);
    addParameter(mOutput);
    addParameter(mGain);
    addParameter(mBias);
    addParameter(mMinimum);
    addParameter(mMaximum);
    addParameter(mCenter);
    mPeriod = globalConfig.frameRate * GRAPHICS_REFRESH_PERIOD;
  }

  ParameterAdapter::~ParameterAdapter()
  {
  }

  void ParameterAdapter::process()
  {
    float x = *(mInput.buffer() + globalConfig.frameLength - 1);
    float gain = mGain.value();
    float bias = mBias.value();

    x *= gain;
    x += bias;

    x = MAX(mLowerClamp, x);
    x = MIN(mUpperClamp, x);

    mOutput.hardSet(x);

    mCount++;

    if (mCount == mPeriod)
    {
      mMinimum.hardSet(mNextMin);
      mMaximum.hardSet(mNextMax);
      mCenter.hardSet(0.5f * (mNextMin + mNextMax));
      mCount = 0;
      // forget
      mNextMin = x;
      mNextMax = x;
    }
    else
    {
      mNextMin = MIN(x, mNextMin);
      mNextMax = MAX(x, mNextMax);
    }

    if (mProbeOutput.isConnected())
    {
      calculateProbeOutput();
    }
  }

  void ParameterAdapter::calculateProbeOutput()
  {
    float gain0 = mPreviousGain;
    float bias0 = mPreviousBias;
    float gain1 = mGain.value();
    float bias1 = mBias.value();

    float *in = mInput.buffer();
    float *out = mProbeOutput.buffer();

    if (fabs(gain1 - gain0) < 1e-8f && fabs(bias1 - bias0) < 1e-8f)
    {
      if (mInput.isConstant() || fabs(gain1) < 1e-10f)
      {
        simd_set(out, FRAMELENGTH, in[0] * gain1 + bias1);
        mProbeOutput.mIsConstant = true;
      }
      else
      {
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          out[i] = in[i] * gain1 + bias1;
        }
        mProbeOutput.mIsConstant = false;
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
      mProbeOutput.mIsConstant = false;
    }

    mPreviousGain = gain1;
    mPreviousBias = bias1;
  }

} /* namespace od */
