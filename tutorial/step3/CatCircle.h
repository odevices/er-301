#pragma once

#include <od/graphics/Graphic.h>
#include <EuclideanSequencer.h>

// This class renders a given EuclideanSequencer object to a given FrameBuffer.
class CatCircle : public od::Graphic
{
public:
  // The constructor args define the size of the drawing area.
  CatCircle(int left, int bottom, int width, int height);
  virtual ~CatCircle();

#ifndef SWIGLUA
  // This required virtual method will be called periodically by the graphics subsystem.
  virtual void draw(od::FrameBuffer &fb);
#endif

  // Tell this graphic which sequencer object it should be rendering.
  void follow(EuclideanSequencer *pSequencer);

private:
  EuclideanSequencer *mpSequencer = 0;
};
