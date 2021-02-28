#include <od/objects/math/ConstantOffset.h>
#include <od/extras/LookupTables.h>
#include <od/config.h>
#include <hal/simd.h>
#include <math.h>

namespace od
{

  ConstantOffset::ConstantOffset()
  {
    addInput(mInput);
    addOutput(mOutput);
    addParameter(mOffset);
  }

  ConstantOffset::~ConstantOffset()
  {
  }

  void ConstantOffset::setClamp(float threshold)
  {
    mClamp = threshold;
  }

  void ConstantOffset::setClampInDecibels(float threshold)
  {
    mClamp = powf(10.0f, threshold * (1.0f / 20.0f));
  }

  void ConstantOffset::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float bias0 = mPreviousBias;
    float bias1 = mOffset.value();

    if (fabs(bias1) < mClamp)
    {
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = in[i];
      }
    }
    else if (fabs(bias1 - bias0) < 1e-10f)
    {
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = in[i] + bias1;
      }
    }
    else
    {
      float *ramp = LookupTables::FrameOfLinearRamp.mValues.data();

      for (int i = 0; i < FRAMELENGTH; i++)
      {
        float w1 = ramp[i];
        float w0 = 1.0f - w1;
        out[i] = in[i] + (w0 * bias0 + w1 * bias1);
      }
    }

    mPreviousBias = bias1;
  }

} /* namespace od */
