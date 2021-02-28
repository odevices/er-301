#include <od/objects/heads/TapeHead.h>
#include <hal/ops.h>

namespace od
{

  TapeHead::TapeHead()
  {
  }

  TapeHead::~TapeHead()
  {
  }

  void TapeHead::setSample(Sample *sample)
  {
    Base::setSample(sample);
    mLeftResetIndex = 0;
    mShadowIndex = 0;
  }

  void TapeHead::stop()
  {
    mPaused = true;
  }

  void TapeHead::start()
  {
    mPaused = false;
  }

  void TapeHead::toggle()
  {
    mPaused = !mPaused;
  }

  void TapeHead::setReset(int s)
  {
    if (mpSample == NULL)
      return;

    mLeftResetIndex = CLAMP(0, mEndIndex, s);
  }

  void TapeHead::setResets(int left, int right)
  {
    if (mpSample == NULL)
      return;

    mLeftResetIndex = CLAMP(0, mEndIndex, left);
    mRightResetIndex = CLAMP(0, mEndIndex, right);
  }

  void TapeHead::setResetProportional(float value)
  {
    if (mpSample == NULL)
      return;
    int N = mpSample->mSampleCount;
    if (value < 0)
    {
      int n = (int)(-value * N);
      if (n < N)
      {
        setReset(N - n);
      }
      else
      {
        setReset(0);
      }
    }
    else
    {
      setReset((int)(value * N));
    }
  }

  void TapeHead::setResetInSeconds(float secs)
  {
    if (mpSample == NULL)
      return;
    if (secs < 0)
    {
      int N = mpSample->mSampleCount;
      int n = (int)(-secs * mpSample->mSampleRate);
      if (n < N)
      {
        setReset(N - n);
      }
      else
      {
        setReset(0);
      }
    }
    else
    {
      setReset(secs * mpSample->mSampleRate);
    }
  }

  void TapeHead::setStops(int left, int right)
  {
    if (mpSample == NULL)
      return;

    mLeftStopIndex = CLAMP(0, mEndIndex, left);
    mRightStopIndex = CLAMP(0, mEndIndex, right);
  }

  void TapeHead::clearStops()
  {
    if (mpSample == NULL)
      return;

    mLeftStopIndex = 0;
    mRightStopIndex = mEndIndex;
  }

  void TapeHead::setLoops(int leftStart, int leftEnd, int rightStart,
                          int rightEnd)
  {
    if (mpSample == NULL)
      return;

    if (leftStart == 0 && leftEnd == 0)
    {
      mLeftLoopStartIndex = mLeftStopIndex;
      mLeftLoopEndIndex = mLeftResetIndex;
    }
    else
    {
      mLeftLoopStartIndex = CLAMP(0, mEndIndex, leftStart);
      mLeftLoopEndIndex = CLAMP(0, mEndIndex, leftEnd);
    }

    if (rightStart == 0 && rightEnd == 0)
    {
      mRightLoopStartIndex = mLeftResetIndex;
      mRightLoopEndIndex = mRightStopIndex;
    }
    else
    {
      mRightLoopStartIndex = CLAMP(0, mEndIndex, rightStart);
      mRightLoopEndIndex = CLAMP(0, mEndIndex, rightEnd);
    }
  }

} /* namespace od */
