#ifndef __testObject_H__
#define __testObject_H__

#include <od/objects/Object.h>
#include <vector>
#include <string>

namespace testlib
{

  class TestObject : public od::Object
  {
  public:
    TestObject(const std::string &name, float secs);
    virtual ~TestObject();

#ifndef SWIGLUA
    virtual void process();
    od::Inlet mInput{"In"};
    od::Parameter mDelay{"Delay"};
    od::Outlet mOutput{"Out"};
#endif

  private:
    std::vector<float> mBuffer;
    int mReadIndex = 0;
    int mWriteIndex = 0;
    int mMaxDelayInSamples = 0;

    void setMaxDelay(int n);
  };
} // namespace testlib

#endif // __testObject_H__
