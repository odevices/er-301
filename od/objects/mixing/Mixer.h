#include <od/objects/Object.h>

namespace od
{

  class Mixer : public Object
  {
  public:
    Mixer(int n);
    virtual ~Mixer();

#ifndef SWIGLUA
    virtual void process();
    // Inputs and Parameters are allocated dynamically.
    Outlet mOutput{"Out"};
#endif

    void setInputGain(int i, float gain)
    {
      if (i < 0 || i >= (int)mParameters.size())
        return;
      mParameters[i]->softSet(gain);
    }
  };

} /* namespace od */
