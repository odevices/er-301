#include <od/graphics/screensavers/Bubbles.h>
#include <od/extras/Random.h>
#include <hal/constants.h>
#include <math.h>

#define BUBBLE_N 36

namespace od
{

  Bubbles::Bubbles()
  {
    X.assign(BUBBLE_N, 0);
    Y.assign(BUBBLE_N, 0);
    R.assign(BUBBLE_N, 0);
  }

  Bubbles::~Bubbles()
  {
  }

  void Bubbles::reset()
  {
    R.assign(BUBBLE_N, 0);
  }

  void Bubbles::draw(FrameBuffer &m, FrameBuffer &s)
  {
    t += GRAPHICS_REFRESH_PERIOD;
    if (t > 24 * 3600)
    {
      t = 0.0f;
    }

    if (Random::generateFloat(0.0f, 1.0f) < 0.05f)
    {
      float p = Random::generateFloat(0.0f, 1.0f);
      int j = p * (BUBBLE_N - 1);
      Y[j] = Random::generateFloat(0.0f, 1.0f) * 64;
      X[j] = Random::generateFloat(0.0f, 1.0f) * 256;
      R[j] = p * p * 8 + 2;
    }

    for (int i = 0; i < BUBBLE_N; i++)
    {
      if (R[i] > 0)
      {
        int x = X[i];
        int y = Y[i];
        int r = R[i] * sinf(x + t * 3.14f);
        m.circle(WHITE, x, y, r);
        s.circle(WHITE, 128 - x, 64 - y, r);
      }
    }
  }

} /* namespace od */
