#include <hal/log.h>

namespace testCpp
{
  class MyAbstractClass
  {
  public:
    MyAbstractClass(const char *name) : mName(name)
    {
      logInfo("MyAbstractClass::MyAbstractClass(%s)", name);
    };
    ~MyAbstractClass(){};
    virtual void process() = 0;
    const char *name() { return mName; }
    const char *mName;
    int m = 0;
  };

  class MyClass : public MyAbstractClass
  {
  public:
    MyClass(const char *name) : MyAbstractClass(name)
    {
      logInfo("MyClass::MyClass(%s)", name);
    };
    ~MyClass(){};
    void method();
    virtual void process();
    int n = 0;
  };

  void MyClass::method()
  {
    n++;
    logInfo("MyClass::method() n=%d", n);
  }

  void MyClass::process()
  {
    m++;
    logInfo("MyClass::process() m=%d", m);
  }

} // namespace testCpp

extern "C"
{
  bool runTestCpp()
  {
    logInfo("runTestCpp called.");
    testCpp::MyClass myClass("TestCpp");
    myClass.method();
    myClass.process();
    myClass.method();
    myClass.process();
    logInfo("runTestCpp exiting.");
    return true;
  }
}
