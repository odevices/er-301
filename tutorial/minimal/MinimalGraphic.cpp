#include <MinimalGraphic.h>
#include <math.h>

MinimalGraphic::MinimalGraphic(int left, int bottom, int width, int height) : od::Graphic(left, bottom, width, height)

{
}

MinimalGraphic::~MinimalGraphic()
{
}

void MinimalGraphic::draw(od::FrameBuffer &fb)
{
  const int MARGIN = 2;
  
  // The coordinate system.
  // (mWorldLeft, mWorldBottom) are the screen coordinates of the (left, bottom) of this graphic.
  // (mWidth, mHeight) are the width and height in pixels of this graphic.
  // All FrameBuffer methods assume screen coordinates.

  // Calculate the radius of a circle that will fit within this graphic's area including a margin.
  int radius = MIN(mWidth / 2, mHeight / 2) - MARGIN;
  // Calculate the screen coordinates of the center of this graphic.
  int centerX = mWorldLeft + mWidth / 2;
  int centerY = mWorldBottom + mHeight / 2;

  // Draw the outer circular.
  // Colors available are: BLACK, GRAY1, ..., GRAY14, WHITE
  fb.circle(GRAY5, centerX, centerY, radius);
}
