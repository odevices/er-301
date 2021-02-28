#pragma once

#include <od/objects/Object.h>
#include <od/extras/LinearRamp.h>
#include <od/ui/JobQueue.h>
#include <od/audio/Sample.h>
#include <od/config.h>

namespace od
{

  class PedalLooper : public Object
  {
  public:
    PedalLooper(int channelCount);
    virtual ~PedalLooper();

    // returns actual time allocated
    float allocateTimeUpTo(float seconds);
    void deallocate();
    float maximumLoopTime()
    {
      return mMaximumFrameCount * globalConfig.framePeriod;
    }

    // Returns the number of samples copied.
    int fillSample(Sample *sample);

    int getLoopLengthInSamples()
    {
      return mFrameCount * FRAMELENGTH;
    }

    float getLoopLengthInSeconds()
    {
      return mFrameCount * globalConfig.framePeriod;
    }

#ifndef SWIGLUA
    virtual void process();
    Inlet mLeftInput{"Left In"};
    Inlet mRightInput{"Right In"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
    Inlet mRecord{"Record"};
    Inlet mStop{"Stop"};
    Inlet mUndo{"Undo"};
    Parameter mFeedback{"Feedback", 1.0f};
    StateMachine mRecordState{"Record"};
    StateMachine mStopState{"Stop"};
    StateMachine mUndoState{"Undo"};
    Option mAfterRecord{"After Record", 0};   // 0 - play, 1 - overdub, 2 - stop
    Option mPassInput{"Pass Input", 0b11111}; // same order as LooperState
#endif

  protected:
    char *mpLeftBufferA = 0;
    char *mpRightBufferA = 0;
    char *mpLeftBufferB = 0;
    char *mpRightBufferB = 0;
    int mChannelCount;
    int mCurrentFrame = 0;
    int mFrameCount = 0;
    int mMaximumFrameCount = 0;
    int mCurrentTake = 0;

    enum LooperState
    {
      emptyState,
      recordState,
      playState,
      overdubState,
      stopState
    };

    LooperState mState = emptyState;
    LooperState mPrevState = emptyState;
    LinearRamp mPlayLevel;
    LinearRamp mRecordLevel;
    LinearRamp mFeedbackLevel;
    LinearRamp mUndoLevel;

    void setPlayLevel(float level);
    void setRecordLevel(float level);
    void setFeedbackLevel(float level);

    void enterUndoState();
    void leaveUndoState();

    bool mBufferSwap = false;
    float *getLeftMaster();
    float *getRightMaster();
    float *getLeftCopy();
    float *getRightCopy();
    float *getLeftPlaying();
    float *getRightPlaying();

    void enterEmptyState();
    void leaveEmptyState();

    void enterRecordState();
    void leaveRecordState();

    void enterPlayState();
    void leavePlayState();

    void enterOverdubState();
    void leaveOverdubState();

    void enterStopState();
    void leaveStopState();

    void mono();
    void stereo();
    void bypass();

    class Request : public Job
    {
    public:
      Request(PedalLooper *looper) : mpLooper(looper)
      {
      }

      PedalLooper *mpLooper;

      virtual void work()
      {
        mpLooper->commitRemaining();
      }
    };

    Request mRequest;

    void scheduleCommit();

    int mCommitStart = 0;
    int mCommitCount = 0;
    void commit();
    // do not call in the audio thread
    void commitRemaining();

    bool allocate(int Ns);
  };

} /* namespace od */
