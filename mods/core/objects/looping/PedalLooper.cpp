#include <core/objects/looping/PedalLooper.h>
#include <hal/simd.h>
#include <od/extras/BigHeap.h>
#include <od/AudioThread.h>
#include <od/UIThread.h>
#include <od/config.h>

namespace od
{

  PedalLooper::PedalLooper(int channelCount) : mChannelCount(channelCount), mRequest(this)
  {
    own(mRequest);

    addOutput(mLeftOutput);
    addOutput(mRightOutput);
    addInput(mLeftInput);
    addInput(mRightInput);
    addInput(mRecord);
    addInput(mStop);
    addInput(mUndo);
    addParameter(mFeedback);
    addOption(mAfterRecord);
    addOption(mPassInput);

    addStateMachine(mRecordState);
    mRecordState.add("----");
    mRecordState.add("record");
    mRecordState.add("play");
    mRecordState.add("overdub");
    mRecordState.add("stop");
    mRecordState.setCurrentState(emptyState);
    mRecordState.setNextState(recordState);

    addStateMachine(mStopState);
    mStopState.add("----");
    mStopState.add("stop");
    mStopState.add("clear");
    mStopState.setCurrentState(0);
    mStopState.setNextState(1);

    addStateMachine(mUndoState);
    mUndoState.add("----", 0);
    mUndoState.add("undo", 0);
    mUndoState.setCurrentState(0);
    mUndoState.setNextState(1);

    mPlayLevel.setLength(5);
    mRecordLevel.setLength(5);
    mFeedbackLevel.setLength(5);
    mUndoLevel.setLength(5);
    setRecordLevel(0);
    setPlayLevel(0);
    setFeedbackLevel(1);
    mUndoLevel.conditionalReset(0);
  }

  PedalLooper::~PedalLooper()
  {
    deallocate();
  }

  bool PedalLooper::allocate(int Ns)
  {
    deallocate();
    int nbytes = Ns * sizeof(float);

    mpLeftBufferA = BigHeap::allocateZeroed(nbytes);
    if (mpLeftBufferA == 0)
    {
      return false;
    }

    if (mChannelCount > 1)
    {
      mpRightBufferA = BigHeap::allocateZeroed(nbytes);
      if (mpRightBufferA == 0)
      {
        deallocate();
        return false;
      }
    }

    mpLeftBufferB = BigHeap::allocateZeroed(nbytes);
    if (mpLeftBufferB == 0)
    {
      deallocate();
      return false;
    }

    if (mChannelCount > 1)
    {
      mpRightBufferB = BigHeap::allocateZeroed(nbytes);
      if (mpRightBufferB == 0)
      {
        deallocate();
        return false;
      }
    }

    return true;
  }

  void PedalLooper::deallocate()
  {
    if (mpLeftBufferA)
    {
      BigHeap::free(mpLeftBufferA);
      mpLeftBufferA = 0;
    }

    if (mpRightBufferA)
    {
      BigHeap::free(mpRightBufferA);
      mpRightBufferA = 0;
    }

    if (mpLeftBufferB)
    {
      BigHeap::free(mpLeftBufferB);
      mpLeftBufferB = 0;
    }

    if (mpRightBufferB)
    {
      BigHeap::free(mpRightBufferB);
      mpRightBufferB = 0;
    }
  }

  float PedalLooper::allocateTimeUpTo(float seconds)
  {
    int Ns = globalConfig.sampleRate * seconds;
    int Nf = (Ns / FRAMELENGTH + 1);
    if (Nf == mMaximumFrameCount)
    {
      // Already allocated.
      return Nf * globalConfig.framePeriod;
    }

    mMaximumFrameCount = 0;

    while (Nf > 1)
    {
      if (allocate(Nf * FRAMELENGTH))
      {
        mMaximumFrameCount = Nf;
        return Nf * globalConfig.framePeriod;
      }
      Nf /= 2;
    }

    // Failed to allocate memory
    return 0;
  }

  void PedalLooper::enterEmptyState()
  {
    mState = emptyState;
    mFrameCount = 0;
    mCurrentFrame = 0;
    setRecordLevel(0);
    setPlayLevel(0);
    setFeedbackLevel(1);
    mRecordState.setCurrentState(emptyState);
    mRecordState.setNextState(recordState);
    mStopState.setCurrentState(0);
    mStopState.setNextState(1);
    leaveUndoState();
    mUndoState.setStateName(0, "----");
    mCurrentTake = 0;
    mUndoState.setStateValue(0, 0);
    mUndoState.setStateValue(1, 0);
  }

  void PedalLooper::leaveEmptyState()
  {
  }

  void PedalLooper::enterRecordState()
  {
    mState = recordState;
    setRecordLevel(1);
    setPlayLevel(0);
    setFeedbackLevel(1);
    mRecordState.setCurrentState(recordState);
    mRecordState.setNextState(playState);
    mStopState.setCurrentState(0);
    mStopState.setNextState(1);
    leaveUndoState();
    mCurrentTake = 1;
    mUndoState.setStateValue(1, 0);
    mUndoState.setStateValue(0, 1);
  }

  void PedalLooper::leaveRecordState()
  {
    mFrameCount = mCurrentFrame;
    mCurrentFrame = 0;
    mUndoState.setStateName(0, "redo");
  }

  void PedalLooper::enterPlayState()
  {
    mState = playState;
    setRecordLevel(0);
    setPlayLevel(1);
    setFeedbackLevel(1);
    mRecordState.setCurrentState(playState);
    mRecordState.setNextState(overdubState);
    mStopState.setCurrentState(0);
    mStopState.setNextState(1);
    leaveUndoState();
  }

  void PedalLooper::leavePlayState()
  {
  }

  void PedalLooper::enterOverdubState()
  {
    mState = overdubState;

    if (mUndoState.mCurrentIndex == 1)
    {
      mBufferSwap = !mBufferSwap;
      leaveUndoState();
    }
    else
    {
      mCurrentTake++;
      mUndoState.setStateValue(1, mCurrentTake - 1);
      mUndoState.setStateValue(0, mCurrentTake);
    }

    setRecordLevel(1);
    setPlayLevel(1);
    setFeedbackLevel(mFeedback.value());
    mRecordState.setCurrentState(overdubState);
    mRecordState.setNextState(playState);
    mStopState.setCurrentState(0);
    mStopState.setNextState(1);
    commit();
  }

  void PedalLooper::leaveOverdubState()
  {
  }

  void PedalLooper::enterStopState()
  {
    mState = stopState;
    setRecordLevel(0);
    setPlayLevel(0);
    setFeedbackLevel(1);
    mRecordState.setCurrentState(stopState);
    mRecordState.setNextState(playState);
    mStopState.setCurrentState(1);
    mStopState.setNextState(2);
  }
  void PedalLooper::leaveStopState()
  {
    mCurrentFrame = 0;
  }

  void PedalLooper::process()
  {
    if (mMaximumFrameCount == 0)
    {
      // No memory available for operation.
      bypass();
      return;
    }

    bool record = simd_any_positive(mRecord.buffer(), FRAMELENGTH);
    bool stop = simd_any_positive(mStop.buffer(), FRAMELENGTH);
    bool undo = simd_any_positive(mUndo.buffer(), FRAMELENGTH);

    // Determine next state
    mPrevState = mState;
    switch (mState)
    {
    case emptyState:
      if (record)
      {
        leaveEmptyState();
        enterRecordState();
      }
      break;
    case recordState:
      if (stop)
      {
        leaveRecordState();
        enterStopState();
      }
      else if (record)
      {
        leaveRecordState();
        switch (mAfterRecord.value())
        {
        case 0:
          enterPlayState();
          break;
        case 1:
          enterOverdubState();
          break;
        case 2:
          enterStopState();
          break;
        }
      }
      break;
    case playState:
      if (undo)
      {
        if (mUndoState.mCurrentIndex)
        {
          leaveUndoState();
        }
        else
        {
          enterUndoState();
        }
      }
      if (stop)
      {
        leavePlayState();
        enterStopState();
      }
      else if (record)
      {
        leavePlayState();
        enterOverdubState();
      }
      break;
    case overdubState:
      if (stop)
      {
        leaveOverdubState();
        enterStopState();
      }
      else if (record || undo)
      {
        leaveOverdubState();
        enterPlayState();
      }
      if (undo)
      {
        enterUndoState();
      }
      break;
    case stopState:
      if (undo)
      {
        if (mUndoState.mCurrentIndex)
        {
          leaveUndoState();
        }
        else
        {
          enterUndoState();
        }
      }
      if (record)
      {
        leaveStopState();
        enterPlayState();
      }
      else if (stop)
      {
        leaveStopState();
        enterEmptyState();
      }
      break;
    }

    // Track feedback level while overdubbing.
    if (mFeedbackLevel.done() && mState == overdubState)
    {
      setFeedbackLevel(mFeedback.value());
    }

    if (mChannelCount > 1)
    {
      stereo();
    }
    else
    {
      mono();
    }

    switch (mState)
    {
    case emptyState:
      break;
    case recordState:
      mCurrentFrame++;
      // Handle buffer full event when recording.
      if (mCurrentFrame == mMaximumFrameCount)
      {
        leaveRecordState();
        enterPlayState();
      }
      break;
    case overdubState:
    case playState:
      mCurrentFrame++;
      if (mCurrentFrame == mFrameCount)
      {
        mCurrentFrame = 0;
      }
      break;
    case stopState:
      break;
    }

    mPlayLevel.step();
    mRecordLevel.step();
    mFeedbackLevel.step();
    mUndoLevel.step();
  }

  void PedalLooper::commit()
  {
    float *bufferL = getLeftMaster();
    float *undoL = getLeftCopy();
    mCommitCount = mFrameCount;
    mCommitStart = mCurrentFrame;
    int n = MIN(10, mFrameCount);
    while (n > 0)
    {
      int j = mCommitStart * FRAMELENGTH;
      memcpy(undoL + j, bufferL + j, sizeof(float) * FRAMELENGTH);
      mCommitStart++;
      if (mCommitStart == mFrameCount)
      {
        mCommitStart = 0;
      }
      n--;
      mCommitCount--;
    }
    scheduleCommit();
  }

  // do not call in the audio thread
  void PedalLooper::commitRemaining()
  {
    if (mFrameCount == 0 || mCommitCount == 0)
    {
      return;
    }

    {
      float *bufferL = getLeftMaster();
      float *undoL = getLeftCopy();
      int i = mCommitStart;
      int n = mCommitCount;
      while (n > 0)
      {
        int j = i * FRAMELENGTH;
        memcpy(undoL + j, bufferL + j, sizeof(float) * FRAMELENGTH);
        i++;
        if (i == mFrameCount)
        {
          i = 0;
        }
        n--;
      }
    }

    if (mChannelCount > 1)
    {
      float *bufferR = getRightMaster();
      float *undoR = getRightCopy();
      int i = mCommitStart;
      int n = mCommitCount;
      while (n > 0)
      {
        int j = i * FRAMELENGTH;
        memcpy(undoR + j, bufferR + j, sizeof(float) * FRAMELENGTH);
        i++;
        if (i == mFrameCount)
        {
          i = 0;
        }
        n--;
      }
    }
  }

  void PedalLooper::scheduleCommit()
  {
    if (!mRequest.pending())
    {
      JobQueue *jobQueue = UIThread::getRealtimeJobQueue();
      jobQueue->push(&mRequest);
    }
  }

  void PedalLooper::setPlayLevel(float level)
  {
    mPlayLevel.conditionalReset(level);
  }

  void PedalLooper::setRecordLevel(float level)
  {
    mRecordLevel.conditionalReset(level);
  }

  void PedalLooper::setFeedbackLevel(float level)
  {
    mFeedbackLevel.conditionalReset(level);
  }

  void PedalLooper::bypass()
  {
    {
      float *inL = mLeftInput.buffer();
      float *outL = mLeftOutput.buffer();
      memcpy(outL, inL, sizeof(float) * FRAMELENGTH);
    }

    if (mChannelCount > 1)
    {
      float *rightIn = mRightInput.buffer();
      float *rightOut = mRightOutput.buffer();
      memcpy(rightOut, rightIn, sizeof(float) * FRAMELENGTH);
    }
  }

  void PedalLooper::enterUndoState()
  {
    mUndoLevel.conditionalReset(1);
    mUndoState.setCurrentState(1);
    mUndoState.setNextState(0);
  }

  void PedalLooper::leaveUndoState()
  {
    mUndoLevel.conditionalReset(0);
    mUndoState.setCurrentState(0);
    mUndoState.setNextState(1);
  }

  float *PedalLooper::getLeftMaster()
  {
    if (mBufferSwap)
    {
      return (float *)mpLeftBufferB;
    }
    else
    {
      return (float *)mpLeftBufferA;
    }
  }

  float *PedalLooper::getRightMaster()
  {
    if (mBufferSwap)
    {
      return (float *)mpRightBufferB;
    }
    else
    {
      return (float *)mpRightBufferA;
    }
  }

  float *PedalLooper::getLeftCopy()
  {
    if (mBufferSwap)
    {
      return (float *)mpLeftBufferA;
    }
    else
    {
      return (float *)mpLeftBufferB;
    }
  }

  float *PedalLooper::getRightCopy()
  {
    if (mBufferSwap)
    {
      return (float *)mpRightBufferA;
    }
    else
    {
      return (float *)mpRightBufferB;
    }
  }

  float *PedalLooper::getLeftPlaying()
  {
    if (mUndoState.mCurrentIndex == 0)
    {
      return getLeftMaster();
    }
    else
    {
      return getLeftCopy();
    }
  }

  float *PedalLooper::getRightPlaying()
  {
    if (mUndoState.mCurrentIndex == 0)
    {
      return getRightMaster();
    }
    else
    {
      return getRightCopy();
    }
  }

  void PedalLooper::mono()
  {
    float *inL = mLeftInput.buffer();
    float *outL = mLeftOutput.buffer();
    float *bufferL = getLeftMaster();
    bufferL += mCurrentFrame * FRAMELENGTH;
    float *undoL = getLeftCopy();
    undoL += mCurrentFrame * FRAMELENGTH;

    float pass;
    if (mPassInput.getFlag(mState))
    {
      pass = 1.0f;
    }
    else
    {
      pass = 0.0f;
    }

    if (mRecordLevel.mGoal > 0.5f || !mRecordLevel.done())
    {
      if (mPrevState == recordState || mState == recordState)
      {
        // record
        float *recLevel = AudioThread::getFrame();
        mRecordLevel.getInterpolatedFrame(recLevel);
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          outL[i] = pass * inL[i];
          undoL[i] = 0;
          bufferL[i] = recLevel[i] * inL[i];
        }
        AudioThread::releaseFrame(recLevel);
      }
      else
      {
        // overdub
        float *playLevel = AudioThread::getFrame();
        float *recLevel = AudioThread::getFrame();
        float *fdbkLevel = AudioThread::getFrame();
        float *undoLevel = AudioThread::getFrame();
        mPlayLevel.getInterpolatedFrame(playLevel);
        mRecordLevel.getInterpolatedFrame(recLevel);
        mFeedbackLevel.getInterpolatedFrame(fdbkLevel);
        mUndoLevel.getInterpolatedFrame(undoLevel);

        for (int i = 0; i < FRAMELENGTH; i++)
        {
          float x = (1 - undoLevel[i]) * bufferL[i] + undoLevel[i] * undoL[i];
          outL[i] = playLevel[i] * x + pass * inL[i];
          bufferL[i] = fdbkLevel[i] * x + recLevel[i] * inL[i];
        }

        AudioThread::releaseFrame(playLevel);
        AudioThread::releaseFrame(recLevel);
        AudioThread::releaseFrame(fdbkLevel);
        AudioThread::releaseFrame(undoLevel);
      }
    }
    else
    {
      // play
      float *playLevel = AudioThread::getFrame();
      float *undoLevel = AudioThread::getFrame();
      mPlayLevel.getInterpolatedFrame(playLevel);
      mUndoLevel.getInterpolatedFrame(undoLevel);

      for (int i = 0; i < FRAMELENGTH; i++)
      {
        float x = (1 - undoLevel[i]) * bufferL[i] + undoLevel[i] * undoL[i];
        outL[i] = playLevel[i] * x + pass * inL[i];
      }

      AudioThread::releaseFrame(playLevel);
      AudioThread::releaseFrame(undoLevel);
    }
  }

  void PedalLooper::stereo()
  {
    float *inL = mLeftInput.buffer();
    float *inR = mRightInput.buffer();
    float *outL = mLeftOutput.buffer();
    float *bufferL = getLeftMaster();
    bufferL += mCurrentFrame * FRAMELENGTH;
    float *undoL = getLeftCopy();
    undoL += mCurrentFrame * FRAMELENGTH;

    float *outR = mRightOutput.buffer();
    float *bufferR = getRightMaster();
    bufferR += mCurrentFrame * FRAMELENGTH;
    float *undoR = getRightCopy();
    undoR += mCurrentFrame * FRAMELENGTH;

    float pass;
    if (mPassInput.getFlag(mState))
    {
      pass = 1.0f;
    }
    else
    {
      pass = 0.0f;
    }

    if (mRecordLevel.mGoal > 0.5f || !mRecordLevel.done())
    {
      if (mPrevState == recordState || mState == recordState)
      {
        // record
        float *recLevel = AudioThread::getFrame();
        mRecordLevel.getInterpolatedFrame(recLevel);
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          outL[i] = pass * inL[i];
          undoL[i] = 0;
          bufferL[i] = recLevel[i] * inL[i];

          outR[i] = pass * inR[i];
          undoR[i] = 0;
          bufferR[i] = recLevel[i] * inR[i];
        }
        AudioThread::releaseFrame(recLevel);
      }
      else
      {
        // overdub
        float *playLevel = AudioThread::getFrame();
        float *recLevel = AudioThread::getFrame();
        float *fdbkLevel = AudioThread::getFrame();
        float *undoLevel = AudioThread::getFrame();
        mPlayLevel.getInterpolatedFrame(playLevel);
        mRecordLevel.getInterpolatedFrame(recLevel);
        mFeedbackLevel.getInterpolatedFrame(fdbkLevel);
        mUndoLevel.getInterpolatedFrame(undoLevel);

        for (int i = 0; i < FRAMELENGTH; i++)
        {
          float x = (1 - undoLevel[i]) * bufferL[i] + undoLevel[i] * undoL[i];
          outL[i] = playLevel[i] * x + pass * inL[i];
          bufferL[i] = fdbkLevel[i] * x + recLevel[i] * inL[i];

          x = (1 - undoLevel[i]) * bufferR[i] + undoLevel[i] * undoR[i];
          outR[i] = playLevel[i] * x + pass * inR[i];
          bufferR[i] = fdbkLevel[i] * x + recLevel[i] * inR[i];
        }

        AudioThread::releaseFrame(playLevel);
        AudioThread::releaseFrame(recLevel);
        AudioThread::releaseFrame(fdbkLevel);
        AudioThread::releaseFrame(undoLevel);
      }
    }
    else
    {
      // play
      float *playLevel = AudioThread::getFrame();
      float *undoLevel = AudioThread::getFrame();
      mPlayLevel.getInterpolatedFrame(playLevel);
      mUndoLevel.getInterpolatedFrame(undoLevel);

      for (int i = 0; i < FRAMELENGTH; i++)
      {
        float x = (1 - undoLevel[i]) * bufferL[i] + undoLevel[i] * undoL[i];
        outL[i] = playLevel[i] * x + pass * inL[i];

        x = (1 - undoLevel[i]) * bufferR[i] + undoLevel[i] * undoR[i];
        outR[i] = playLevel[i] * x + pass * inR[i];
      }

      AudioThread::releaseFrame(playLevel);
      AudioThread::releaseFrame(undoLevel);
    }
  }

  int PedalLooper::fillSample(Sample *sample)
  {
    if (sample == 0)
    {
      return 0;
    }

    int Ns = MIN((int)sample->mSampleCount, mFrameCount * FRAMELENGTH);

    if (Ns == 0)
    {
      return 0;
    }

    if (sample->mChannelCount == 1)
    {
      // Sample is mono.  Just copy the left channel.
      memcpy(sample->mpData, getLeftPlaying(), sizeof(float) * Ns);
    }
    else if (mChannelCount == 2)
    {
      // Sample and loop buffer are stereo.
      float *bufferL = getLeftPlaying();
      float *bufferR = getRightPlaying();
      float *out = sample->mpData;
      if (sample->mChannelCount == 2)
      {
        for (int i = 0; i < Ns; i++)
        {
          out[2 * i] = bufferL[i];
          out[2 * i + 1] = bufferR[i];
        }
      }
    }
    else
    {
      // Sample is stereo but loop buffer is mono.
      float *bufferL = getLeftPlaying();
      float *out = sample->mpData;
      if (sample->mChannelCount == 2)
      {
        for (int i = 0; i < Ns; i++)
        {
          out[2 * i] = bufferL[i];
          out[2 * i + 1] = bufferL[i];
        }
      }
    }

    sample->mDirty = true;
    sample->alterWaterMark();

    return Ns;
  }

} /* namespace od */
