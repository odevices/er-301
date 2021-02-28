#pragma once

#include <od/objects/Object.h>
#include <core/objects/freeverb/ReverbModel.h>

namespace od
{

  class Freeverb : public Object
  {
  public:
    Freeverb();
    virtual ~Freeverb();

#ifndef SWIGLUA
    virtual void process();
    Inlet mLeftInput{"Left In"};
    Inlet mRightInput{"Right In"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
    Parameter mSize{"Size", initialroom};
    Parameter mDamp{"Damp", initialdamp};
    Parameter mWidth{"Width", initialwidth};
#endif

  private:
    ReverbModel model;
  };

} /* namespace od */
