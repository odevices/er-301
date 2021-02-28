#include <core/objects/granular/MonoGrainDelay.h>
#include <od/config.h>
#include <hal/ops.h>
#include <string.h>

namespace od
{

  MonoGrainDelay::MonoGrainDelay(float secs)
  {
    addInput(mInput);
    addInput(mDelay);
    addInput(mSpeed);
    addOutput(mOutput);
    setMaxDelay(secs);
    mGrainDurationInSamples = 11 * globalConfig.frameLength;
    mGrainPeriodInSamples = 9 * globalConfig.frameLength;
    for (int i = 0; i < MONOPSD_GRAIN_COUNT; i++)
    {
#if 0
		mGrains[i].setEnvelope(MonoGrain::mTrapezoidWindow);
		mGrains[i].setFade(2 * globalConfig.frameLength);
#else
      mGrains[i].setEnvelope(MonoGrain::mSineWindow);
      mGrains[i].setSquash(2);
#endif
    }
  }

  MonoGrainDelay::~MonoGrainDelay()
  {
  }

  void MonoGrainDelay::setMaxDelay(float secs)
  {
    for (int i = 0; i < MONOPSD_GRAIN_COUNT; i++)
    {
      mGrains[i].setSample(0);
    }
    if (secs < 0.0f)
      secs = 0.0f;
    mMaxDelayInSeconds = secs;
    mMaxDelayInSamples = (int)(secs * globalConfig.sampleRate);
    mSampleFifo.setSampleRate(globalConfig.sampleRate);
    mSampleFifo.allocateBuffer(1, mMaxDelayInSamples + 2 * globalConfig.frameLength);
    mSampleFifo.zeroAndFill();
    for (int i = 0; i < MONOPSD_GRAIN_COUNT; i++)
    {
      mGrains[i].setSample(mSampleFifo.getSample());
    }
  }

  MonoGrain *MonoGrainDelay::getFreeGrain()
  {
    for (int i = 0; i < MONOPSD_GRAIN_COUNT; i++)
    {
      if (!mGrains[i].mActive)
      {
        return &(mGrains[i]);
      }
    }
    return 0;
  }

  void MonoGrainDelay::process()
  {
    float *out = mOutput.buffer();
    float *in = mInput.buffer();

    mSampleFifo.pop(FRAMELENGTH);
    mSampleFifo.pushMono(in, FRAMELENGTH);

    // zero the output buffer
    memset(out, 0, sizeof(float) * FRAMELENGTH);

    for (int i = 0; i < MONOPSD_GRAIN_COUNT; i++)
    {
      if (mGrains[i].mActive)
      {
        mGrains[i].synthesizeFromMonoToMono(out);
      }
    }

    if (mSamplesUntilNextOnset < (int)globalConfig.frameLength)
    {
      MonoGrain *pGrain = getFreeGrain();
      if (pGrain)
      {
        int i = MAX(0, mSamplesUntilNextOnset);
        float delay = mDelay.buffer()[i];
        float speed = mSpeed.buffer()[i];

        int duration = mGrainDurationInSamples;
        int needed = duration * speed + 1;

        int targetDelay = delay * globalConfig.sampleRate;
        if (targetDelay < needed)
        {
          targetDelay = needed;
        }

        int start;
        if (targetDelay < mMaxDelayInSamples)
        {
          start = mMaxDelayInSamples - targetDelay;
        }
        else
        {
          targetDelay = mMaxDelayInSamples;
          start = 0;
          duration = mMaxDelayInSamples / speed;
        }

        // translate to fifo offset
        start += mSampleFifo.offsetToRecent(
            mMaxDelayInSamples + globalConfig.frameLength);

        pGrain->init(start, duration, speed, 1.0f, 0.0f);
        //pGrain->snapToZeroCrossing(4 * globalConfig.frameLength);
        pGrain->synthesizeFromMonoToMono(out);

        mSamplesUntilNextOnset = mGrainPeriodInSamples;
      }
    }
    mSamplesUntilNextOnset -= globalConfig.frameLength;
  }

} /* namespace od */
