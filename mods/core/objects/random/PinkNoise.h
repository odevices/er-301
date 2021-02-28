#pragma once

#include <od/objects/Object.h>

namespace od
{

  class PinkNoise : public Object
  {
  public:
    PinkNoise(int numRows = 16);
    virtual ~PinkNoise();

#ifndef SWIGLUA
    virtual void process();
    Outlet mOutput{"Out"};
#endif

  private:
#define PINK_MAX_RANDOM_ROWS (30)
    int mRows[PINK_MAX_RANDOM_ROWS];
    int mRunningSum; /* Used to optimize summing of generators. */
    int mIndex;       /* Incremented each sample. */
    int mIndexMask;   /* Index wrapped by ANDing with this mask. */
    float mScalar;    /* Used to scale within range of -1.0 to +1.0 */
  };

} /* namespace od */
