#include <EuclideanSequencer.h>
#include <bjorklund.h>
#include <od/config.h>
#include <hal/ops.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>

EuclideanSequencer::EuclideanSequencer(int space)
{
  mSpace.reserve(space);
  mScratch.reserve(2 * space);
  addInput(mTrigger);
  addInput(mReset);
  addOutput(mOutput);
  addParameter(mBoxes);
  mBoxes.enableSerialization();
  addParameter(mCats);
  mCats.enableSerialization();
  simulateCatsInBoxes(0, 1);
}

EuclideanSequencer::~EuclideanSequencer()
{
}

void EuclideanSequencer::simulateCatsInBoxes(int cats, int boxes)
{
  if (cats != mCachedCats || boxes != mCachedBoxes)
  {
    // Recalculate and cache the results.
    logDebug(1, "cats=%d, boxes=%d", cats, boxes);
    bjorklund(cats, boxes, mSpace.data(), mScratch.data());
    mCachedBoxes = boxes;
    mCachedCats = cats;
    mPhase = 0;
  }
}

void EuclideanSequencer::process()
{
  int N = (int)mSpace.capacity();
  int boxes = CLAMP(1, N, mBoxes.roundTarget());
  int cats = CLAMP(0, boxes, mCats.roundTarget());
  simulateCatsInBoxes(cats, boxes);

  float *trig = mTrigger.buffer();
  float *reset = mReset.buffer();
  float *out = mOutput.buffer();
  for (int i = 0; i < FRAMELENGTH; i++)
  {
    if (reset[i] > 0.0f)
    {
      mPhase = 0;
    }

    if (trig[i] > 0.0f)
    {
      mPhase++;
      if (mPhase >= mCachedBoxes)
      {
        mPhase = 0;
      }
      out[i] = mSpace[mPhase];
    }
    else
    {
      out[i] = 0.0f;
    }
  }
}
