#pragma once

#include <od/audio/Sample.h>
#include <od/constants.h>
#include <od/ui/JobQueue.h>

namespace od
{

  class SampleLoader : public Job
  {
  public:
    SampleLoader();
    virtual ~SampleLoader();

    bool set(Sample *sample, SampleLoadInfo *info);

    int mStatus = STATUS_NONE;
    float mPercentDone = 0;

  private:
    SampleLoadInfo *mpLoadInfo = 0;
    uint32_t mSamplesRead = 0;
    uint32_t mSamplesRemaining = 0;
    Sample *mpSample = NULL;
    virtual void work();
    int load();
  };

} /* namespace od */
