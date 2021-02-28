#include <od/objects/measurement/FifoProbe.h>
#include <od/AudioThread.h>
#include <od/config.h>

namespace od
{

  FifoProbe::FifoProbe(int depth, int decimate)
  {
    setDepth(depth);
    mDecimate = decimate;
  }

  FifoProbe::~FifoProbe()
  {
  }

  void FifoProbe::process()
  {
    if (mDecimate > 1)
    {
      float *tmp = AudioThread::getFrame();
      float *input = mInput.buffer();
      int i, j;
      for (i = 0, j = 0; j < FRAMELENGTH; i++, j += mDecimate)
      {
        tmp[i] = input[j];
      }
      mBuffer.push(tmp, i);
      AudioThread::releaseFrame(tmp);
    }
    else
    {
      mBuffer.push(mInput.buffer(), globalConfig.frameLength);
    }
  }

  void FifoProbe::setDepth(int n)
  {
    mBuffer.allocate(n);
    mBuffer.clear();
  }

  void FifoProbe::setDecimation(int d)
  {
    mDecimate = d;
  }

  void FifoProbe::reset()
  {
    mBuffer.clear();
  }

  float *FifoProbe::get(uint32_t n)
  {
    return mBuffer.get(n);
  }

  int FifoProbe::getRate()
  {
    return globalConfig.sampleRate / mDecimate;
  }

  uint32_t FifoProbe::size()
  {
    return mBuffer.size();
  }

} /* namespace od */
