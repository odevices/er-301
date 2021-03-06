#pragma once

#include <od/objects/Object.h>
#include <vector>

class EuclideanSequencer : public od::Object
{
public:
  EuclideanSequencer(int space);
  ~EuclideanSequencer();

#ifndef SWIGLUA
  // These declarations are omitted from the swig wrapper to reduce code size.
  virtual void process();
  od::Inlet mTrigger{"Trigger"};
  od::Inlet mReset{"Reset"};
  od::Outlet mOutput{"Out"};
  od::Parameter mBoxes{"Boxes"};
  od::Parameter mCats{"Cats"};
#endif

protected:
  // Protected declarations are also omitted from the swig wrapper.  
  std::vector<float> mSpace;
  std::vector<float> mScratch;
  int mPhase = 0;
  int mCachedBoxes = -1;
  int mCachedCats = -1;

  void simulateCatsInBoxes(int cats, int boxes);
};