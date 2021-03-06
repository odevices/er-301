#include <EuclideanSequencer.h>
#include <bjorklund.h>
#include <od/config.h>
#include <hal/ops.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>

EuclideanSequencer::EuclideanSequencer(int space)
{
  // Reserve space for the Bjorklund algorithm.
  mSpace.reserve(space);
  mScratch.reserve(2 * space);
  // Register inlets, outlets, and parameters.
  addInput(mTrigger);
  addInput(mReset);
  addOutput(mOutput);
  addParameter(mBoxes);
  mBoxes.enableSerialization();
  addParameter(mCats);
  mCats.enableSerialization();
  // Initialize the cats and boxes.
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
    if (boxes != mCachedBoxes)
    {
      mPhase = 0;
    }
    mCachedBoxes = boxes;
    mCachedCats = cats;
  }
}

// This method is called every audio frame.
// You are expected to process one frame of input and produce one frame of output.
void EuclideanSequencer::process()
{
  // Read the parameters to see if they have changed.
  int N = (int)mSpace.capacity();
  int boxes = CLAMP(1, N, mBoxes.roundTarget());
  int cats = CLAMP(0, boxes, mCats.roundTarget());
  simulateCatsInBoxes(cats, boxes);

  // Each inlet's buffer is filled with 1 frame of samples.
  float *trig = mTrigger.buffer();
  float *reset = mReset.buffer();
  // The output buffer that needs to be filled.
  float *out = mOutput.buffer();

  // FRAMELENGTH is a macro that signals to the compiler that it should try to vectorize the loop.
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
