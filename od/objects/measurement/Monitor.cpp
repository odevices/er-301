#include <od/objects/measurement/Monitor.h>
#include <hal/simd.h>
#include <od/config.h>

namespace od
{

  Monitor::Monitor()
  {
    addInput(mInput);
    addOutput(mOutput);
  }

  Monitor::~Monitor()
  {
  }

  void Monitor::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();

    if (mInput.isConstant())
    {
      simd_set(out, FRAMELENGTH, in[0]);
      mOutput.mIsConstant = true;
    }
    else
    {
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = in[i];
      }
      mOutput.mIsConstant = false;
    }
  }

} /* namespace od */
