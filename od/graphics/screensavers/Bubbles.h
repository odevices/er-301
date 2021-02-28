#pragma once

#include <od/graphics/ScreenSaver.h>

namespace od
{

  class Bubbles : public ScreenSaver
  {
  public:
    Bubbles();
    virtual ~Bubbles();

    virtual void reset();
    virtual void draw(FrameBuffer &mainFrameBuffer,
                      FrameBuffer &subFrameBuffer);

  private:
    std::vector<int> X, Y, R;
    float t = 0.0f;
  };

} /* namespace od */
