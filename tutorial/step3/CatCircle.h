#pragma once

#include <od/graphics/Graphic.h>
#include <EuclideanSequencer.h>

class CatCircle : public od::Graphic
{
public:
  CatCircle(int left, int bottom, int width, int height);
  virtual ~CatCircle();

#ifndef SWIGLUA
  virtual void draw(od::FrameBuffer &fb);
#endif

  void follow(EuclideanSequencer *pSequencer);

private:
  EuclideanSequencer *mpSequencer = 0;
};
