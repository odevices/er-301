#include <od/graphics/screensavers/Lines.h>
#include <od/extras/Random.h>
#include <hal/constants.h>
#include <math.h>

namespace od
{

  Lines::Lines(bool enableGrid) : mEnableGrid(enableGrid)
  {
  }

  Lines::~Lines()
  {
  }

  void Lines::reset()
  {
    t = Random::generateFloat(0.0f, 1.0f);
  }

  void Lines::draw(FrameBuffer &m, FrameBuffer &s)
  {
    t += GRAPHICS_REFRESH_PERIOD / 4;
    if (t > 24 * 3600)
    {
      t = 0.0f;
    }

    //float p0 = 0.5f * (1.0f + sinf(t * 3.14f));
    //float p1 = 0.5f * (1.0f + sinf(t * 3.37f));
    float p0 = 0.5f + 0.6f * sinf(t * 3.14f);
    float p1 = 0.5f + 0.6f * sinf(t * 3.37f + 3.14f * 0.25f);
    int x0 = 256 * p0;
    int x1 = 256 * p1;
    if (mEnableGrid)
    {
      for (int i = -128; i < 128; i += 16)
      {
        m.line(WHITE, x0 + i, 0, x1 + i, 64);
      }
      for (int i = -64; i < 64; i += 16)
      {
        s.line(WHITE, x1 / 2 + i, 0, x0 / 2 + i, 64);
      }
      for (int i = -32; i < 32; i += 8)
      {
        m.line(WHITE, 0, x1 / 4 + i, 256, x0 / 4 + i);
        s.line(WHITE, 0, x0 / 4 + i, 128, x1 / 4 + i);
      }
    }
    else
    {
      m.line(WHITE, x0, 0, x1, 64);
      s.line(WHITE, x1 / 2, 0, x0 / 2, 64);
    }
  }

} /* namespace od */
