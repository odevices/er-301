#include <od/extras/Random.h>
#include <hal/rng.h>
#include <random>

namespace od
{

#if 1
  static std::mt19937 _engine;
#else
  static std::default_random_engine _engine{};
#endif

  void Random::init()
  {
    _engine.seed(Rng_read32());
  }

  unsigned int Random::generateUnsignedInteger()
  {
    static std::uniform_int_distribution<unsigned int> dist{};
    return dist(_engine);
  }

  int Random::generateInteger()
  {
    static std::uniform_int_distribution<int> dist{};
    return dist(_engine);
  }

  int Random::generateInteger(int from, int to)
  {
    static std::uniform_int_distribution<int> dist{};
    return dist(_engine, decltype(dist)::param_type{from, to});
  }

  float Random::generateFloat(float from, float to)
  {
    static std::uniform_real_distribution<float> dist{};
    return dist(_engine, decltype(dist)::param_type{from, to});
  }

  double Random::generateDouble(double from, double to)
  {
    static std::uniform_real_distribution<double> dist{};
    return dist(_engine, decltype(dist)::param_type{from, to});
  }

} // namespace od