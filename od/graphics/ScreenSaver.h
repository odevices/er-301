#pragma once

#include <od/graphics/FrameBuffer.h>
#include <od/graphics/SubFrameBuffer.h>

namespace od
{

  class ScreenSaver
  {
  public:
    ScreenSaver();
    virtual ~ScreenSaver();

    virtual void reset() = 0;
    virtual void draw(FrameBuffer &mainFrameBuffer,
                      FrameBuffer &subFrameBuffer) = 0;
  };

} /* namespace od */
