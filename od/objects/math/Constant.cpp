#include <od/objects/math/Constant.h>
#include <od/extras/LookupTables.h>
#include <od/config.h>
#include <math.h>

namespace od
{

  Constant::Constant()
  {
    addOutput(mOutput);
    addParameter(mValue);
  }

  Constant::~Constant()
  {
  }

  void Constant::setClamp(float threshold)
  {
    mClamp = threshold;
  }

  void Constant::setClampInDecibels(float threshold)
  {
    mClamp = powf(10.0f, threshold * (1.0f / 20.0f));
  }

  void Constant::process()
  {
    float *out = mOutput.buffer();
    float value0 = mPreviousValue;
    float value1 = mValue.value();

    if (fabs(value1) < mClamp)
    {
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = 0.0f;
      }
      mOutput.mIsConstant = true;
    }
    else if (fabs(value1 - value0) < 1e-10f)
    {
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = value1;
      }
      mOutput.mIsConstant = true;
    }
    else
    {
      float *ramp = LookupTables::FrameOfLinearRamp.mValues.data();
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        float w1 = ramp[i];
        float w0 = 1.0f - w1;
        out[i] = w0 * value0 + w1 * value1;
      }
      mOutput.mIsConstant = false;
    }

    mPreviousValue = value1;
  }

} /* namespace od */
