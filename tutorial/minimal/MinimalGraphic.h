#pragma once

#include <od/graphics/Graphic.h>

// This class renders a circle to a given FrameBuffer.
class MinimalGraphic : public od::Graphic
{
public:
  // The constructor args define the size of the drawing area.
  MinimalGraphic(int left, int bottom, int width, int height);
  virtual ~MinimalGraphic();

#ifndef SWIGLUA
  // This required virtual method will be called periodically by the graphics subsystem.
  virtual void draw(od::FrameBuffer &fb);
#endif

};
