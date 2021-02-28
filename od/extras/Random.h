#pragma once

namespace od
{
  class Random
  {
  public:
    static void init();

    static unsigned int generateUnsignedInteger();
    static int generateInteger();
    static int generateInteger(int from, int to);

    static float generateFloat(float from, float to);
    static double generateDouble(double from, double to);

    /* Calculate pseudo-random 32 bit number based on linear congruential method. */
    static unsigned int generateUnsignedIntegerFast(void)
    {
      static unsigned long randSeed = 22222; 
      randSeed = (randSeed * 196314165) + 907633515;
      return randSeed;
    }

  private:
    Random();
  };
} // namespace od