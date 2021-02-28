/*
 * SampleSaver.h
 *
 *  Created on: 24 Oct 2016
 *      Author: clarkson
 */

#ifndef APP_AUDIO_SAMPLESAVER_H_
#define APP_AUDIO_SAMPLESAVER_H_

#include <od/audio/Sample.h>
#include <od/constants.h>
#include <od/ui/JobQueue.h>

namespace od
{

  class SampleSaver : public Job
  {
  public:
    SampleSaver();
    virtual ~SampleSaver();

    bool set(Sample *sample, const char *filename);

    int mStatus = STATUS_NONE;
    float mPercentDone = 0;

  private:
    std::string mFilename;
    Sample *mpSample = NULL;
    uint32_t mSamplesPerBlock = 2048;
    uint32_t mSamplesWritten = 0;
    uint32_t mSamplesRemaining = 0;

    virtual void work();
    int save();
  };

} /* namespace od */

#endif /* APP_AUDIO_SAMPLESAVER_H_ */
