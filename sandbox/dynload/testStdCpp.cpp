#include <hal/log.h>
#include <vector>
#include <string>

namespace testStdCpp
{
  class MyAbstractClass2
  {
  public:
    MyAbstractClass2(const std::string &name) : mName(name)
    {
      logInfo("MyAbstractClass2::MyAbstractClass2(%s)", name.c_str());
    };
    ~MyAbstractClass2(){};
    virtual void process() = 0;
    const std::string &name() { return mName; }
    std::string mName;
    int m = 0;
  };

  class MyClass2 : public MyAbstractClass2
  {
  public:
    MyClass2(const std::string &name) : MyAbstractClass2(name)
    {
      logInfo("MyClass2::MyClass2(%s)", name.c_str());
      mData.resize(10);
    };
    ~MyClass2(){};
    void method();
    virtual void process();
    std::vector<int> mData;
    int n = 0;
  };

  void MyClass2::method()
  {
    n++;
    logInfo("MyClass2::method() n=%d", n);
  }

  void MyClass2::process()
  {
    m++;
#if 1
    mData[m % mData.size()] = n;
#else
    if (m >= (int)mData.size())
    {
      m = 0;
    }
    mData[m] = n;
#endif
    logInfo("MyClass2::process() m=%d", m);
  }

} // namespace testStdCpp

extern "C"
{
  bool runTestStdCpp()
  {
    logInfo("runTestStdCpp called.");
    testStdCpp::MyClass2 myClass("TestStdCpp");
    myClass.method();
    myClass.process();
    myClass.method();
    myClass.process();
    logInfo("runTestStdCpp exiting.");
    return true;
  }
}
