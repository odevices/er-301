#include <hal/log.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

float testCosine(float x)
{
  return cosf(x);
}

int testDivMod(int x, int y)
{
  return x % y;
}

bool runTestC()
{
  logInfo("runTestC called.");
  logInfo("testCosine(3.1415) returned %f", testCosine(3.1415));
  logInfo("testDivMod(123,10) returned %d", testDivMod(123, 10));
  logInfo("runTestC exiting.");
  return true;
}