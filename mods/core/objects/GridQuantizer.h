#pragma once

#include <od/objects/Object.h>

namespace od
{

  class GridQuantizer : public Object
  {
  public:
    GridQuantizer();
    virtual ~GridQuantizer();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Parameter mPreScale{"Pre-Scale", 1.0f};
    Parameter mLevels{"Levels", 12.0f};
    Parameter mPostScale{"Post-Scale", 1.0f};
#endif
  };

} /* namespace od */
