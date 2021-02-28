#pragma once

#include <od/graphics/ScreenSaver.h>

namespace od
{

  class Blank : public ScreenSaver
  {
  public:
    Blank();
    virtual ~Blank();

    virtual void reset();
    virtual void draw(FrameBuffer &mainFrameBuffer,
                      FrameBuffer &subFrameBuffer);
  };

} /* namespace od */
