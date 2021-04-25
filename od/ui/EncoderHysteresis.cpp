#include <od/ui/EncoderHysteresis.h>

namespace od
{

  EncoderHysteresis::EncoderHysteresis()
  {
  }

  EncoderHysteresis::~EncoderHysteresis()
  {
  }

  void EncoderHysteresis::setThreshold(int threshold)
  {
    mThreshold = threshold;
  }

  void EncoderHysteresis::setReset(int reset)
  {
    mReset = reset;
  }

  int EncoderHysteresis::onChange(int change)
  {
    int result = 0;

    mSum += change;
    if (mSum > mThreshold)
    {
      result = mSum - mThreshold;
      mSum = mReset;
    }
    else if (mSum < -mThreshold)
    {
      result = mSum + mThreshold;
      mSum = -mReset;
    }

    return result;
  }

} /* namespace od */
