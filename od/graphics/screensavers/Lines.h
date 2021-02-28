#pragma once

#include <od/graphics/ScreenSaver.h>

namespace od
{

  class Lines : public ScreenSaver
  {
  public:
    Lines(bool enableGrid = true);
    virtual ~Lines();

    virtual void reset();
    virtual void draw(FrameBuffer &mainFrameBuffer,
                      FrameBuffer &subFrameBuffer);

  private:
    float t = 0.0f;
    bool mEnableGrid = true;
  };

} /* namespace od */
