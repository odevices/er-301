#include <od/objects/measurement/CaptureProbe.h>
#include <od/config.h>

namespace od
{

  CaptureProbe::CaptureProbe()
  {
  }

  CaptureProbe::~CaptureProbe()
  {
  }

  void CaptureProbe::process()
  {
    if (mpBuffer == 0)
      return;

    float *in = mInput.buffer();
    float *end = in + globalConfig.frameLength;

    while (in < end && mCount < mBufferSize)
    {
      mpBuffer[mCount] = *(in++);
      mCount++;
    }
  }

} /* namespace od */
