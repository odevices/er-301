#include <od/objects/measurement/LoudnessProbe.h>
#include <od/AudioThread.h>
#include <od/config.h>

namespace od
{

  LoudnessProbe::LoudnessProbe()
  {
    mFastEWMA.setTimeConstant(globalConfig.sampleRate, 1.0f / 20.0f);
  }

  LoudnessProbe::~LoudnessProbe()
  {
  }

  void LoudnessProbe::process()
  {
    float *input = mInput.buffer();
    float *tmp = AudioThread::getFrame();
    float dc = mFastEWMA.push(input, FRAMELENGTH);
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      tmp[i] = input[i] - dc;
    }
    mRunningRMS.push(tmp, FRAMELENGTH);
    AudioThread::releaseFrame(tmp);
  }

} /* namespace od */
