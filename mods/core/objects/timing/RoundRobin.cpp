#include <core/objects/timing/RoundRobin.h>
#include <hal/simd.h>
#include <od/config.h>
#include <sstream>
#include <cstring>

namespace od
{

  RoundRobin::RoundRobin(int n)
  {
    addInput(mNext);
    addInput(mInput);
    addOption(mProcessingRate);
    addOption(mActiveOutput);
    mNumOutputs = n;
    mBuffers = new float *[n];
    for (int i = 0; i < n; i++)
    {
      std::ostringstream ss;
      ss << "Out" << i + 1;
      Outlet *port = new Outlet(ss.str());
      port->mName = ss.str();
      addOutputFromHeap(port);
      mBuffers[i] = port->buffer();
    }
  }

  RoundRobin::~RoundRobin()
  {
    delete[] mBuffers;
  }

  void RoundRobin::next()
  {
    int i = mActiveOutput.value() + 1;
    if (i == mNumOutputs)
    {
      mActiveOutput.set(0);
    }
    else
    {
      mActiveOutput.set(i);
    }
  }

  void RoundRobin::reset()
  {
    mActiveOutput.set(0);
  }

  void RoundRobin::process()
  {
    float *in = mInput.buffer();
    float *trigger = mNext.buffer();

    if (mProcessingRate.value() == PER_SAMPLE)
    {
      float *tmp = mBuffers[mActiveOutput.value()];
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        if (trigger[i] > 0.0f)
        {
          next();
        }
        tmp[i] = in[i];
      }
    }
    else if (simd_any_positive(trigger, FRAMELENGTH))
    {
      next();
      float *tmp = mBuffers[mActiveOutput.value()];
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        tmp[i] = in[i];
      }
    }
  }

} /* namespace od */
