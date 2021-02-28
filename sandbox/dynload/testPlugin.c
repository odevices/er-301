#include <hal/log.h>
#include <stdbool.h>

bool runTestC();
bool runTestCpp();
bool runTestStdCpp();
bool runTestObject();

void runTest()
{
#if 0
  if (runTestC())
  {
    logInfo("runTestC: passed.");
  }
  else
  {
    logInfo("runTestC: failed.");
  }

  if(runTestCpp()){
    logInfo("runTestCpp: passed.");
  } else {
    logInfo("runTestCpp: failed.");
  }

  if(runTestStdCpp()){
    logInfo("runTestStdCpp: passed.");
  } else {
    logInfo("runTestStdCpp: failed.");
  }
#endif

  if (runTestObject())
  {
    logInfo("runTestObject: passed.");
  }
  else
  {
    logInfo("runTestObject: failed.");
  }
}