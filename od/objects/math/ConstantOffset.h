#include <od/objects/Object.h>
#include <od/extras/LookupTables.h>
#include <limits>

namespace od
{

  class ConstantOffset : public Object
  {
  public:
    ConstantOffset();
    virtual ~ConstantOffset();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Parameter mOffset{"Offset"};
#endif

    void setClamp(float threshold);
    void setClampInDecibels(float threshold);

  private:
    float mClamp = std::numeric_limits<float>::min();
    float mPreviousBias = 0.0f;
  };

} /* namespace od */
