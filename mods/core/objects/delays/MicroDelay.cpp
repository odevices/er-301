#include <core/objects/delays/MicroDelay.h>
#include <od/config.h>
#include <string.h>

namespace od
{

  MicroDelay::MicroDelay(float secs)
  {
    addInput(mInput);
    addParameter(mDelay);
    addOutput(mOutput);
    setMaxDelay(globalConfig.sampleRate * secs);
  }

  MicroDelay::~MicroDelay()
  {
  }

  void MicroDelay::setMaxDelay(int n)
  {
    // Enforce multiples of 4 samples for Neon.
    n = (n / 4) * 4;
    if (n < 4)
      n = 4;
    mBuffer.resize(n + globalConfig.frameLength, 0);
    mMaxDelayInSamples = n;
    mWriteIndex = 0;
    mReadIndex = 0;
  }

  void MicroDelay::process()
  {
    // Set the new read position based on the updated delay
    float delay = mDelay.target();
    int n = delay * globalConfig.sampleRate;

    // Enforce multiples of 4 samples for Neon.
    n = (n / 4) * 4;
    if (n < 0)
      n = 0;
    else if (n > mMaxDelayInSamples)
      n = mMaxDelayInSamples;

    mReadIndex = mWriteIndex - n;
    if (mReadIndex < 0)
    {
      mReadIndex += mBuffer.size();
    }

    float *in = mInput.buffer();
    float *out = mOutput.buffer();

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      memcpy(&(mBuffer[mWriteIndex]), in + i, sizeof(float) * 4);
      memcpy(out + i, &(mBuffer[mReadIndex]), sizeof(float) * 4);

      mReadIndex += 4;
      if (mReadIndex >= (int)mBuffer.size())
      {
        mReadIndex = 0;
      }

      mWriteIndex += 4;
      if (mWriteIndex >= (int)mBuffer.size())
      {
        mWriteIndex = 0;
      }
    }
  }

} /* namespace od */
