#include <od/objects/measurement/TriggerProbe.h>

namespace od
{

  TriggerProbe::TriggerProbe()
  {
    addOption(mState);
    addParameter(mThreshold);
  }

  TriggerProbe::~TriggerProbe()
  {
  }

  void TriggerProbe::process()
  {
    float *in = mInput.buffer();
    if (in[0] > mThreshold.value())
    {
      mState.set(1);
    }
  }

  void TriggerProbe::reset()
  {
    mState.set(0);
  }

} /* namespace od */
