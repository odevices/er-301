#include <CatCircle.h>
#include <math.h>

CatCircle::CatCircle(int left, int bottom, int width, int height) : od::Graphic(left, bottom, width, height)

{
}

CatCircle::~CatCircle()
{
  if (mpSequencer)
  {
    mpSequencer->release();
  }
}

void CatCircle::draw(od::FrameBuffer &fb)
{
  const int CURSOR = 3;
  const int MARGIN = 2;
  
  // The coordinate system.
  // (mWorldLeft, mWorldBottom) are the screen coordinates of the (left, bottom) of this graphic.
  // (mWidth, mHeight) are the width and height in pixels of this graphic.
  // All FrameBuffer methods assume screen coordinates.

  // Calculate the radius of a circle that will fit within this graphic's area including a margin and space for the cursor.
  int radius = MIN(mWidth / 2, mHeight / 2) - MARGIN - CURSOR;
  // Calculate the screen coordinates of the center of this graphic.
  int centerX = mWorldLeft + mWidth / 2;
  int centerY = mWorldBottom + mHeight / 2;

  // Draw the outer circular.
  // Colors available are: BLACK, GRAY1, ..., GRAY14, WHITE
  fb.circle(GRAY5, centerX, centerY, radius);

  // Do we have a sequencer object to render?
  if (mpSequencer)
  {
    float theta, x, y;
    int n = mpSequencer->mCachedBoxes;
    if(n < 1)
    {
      // There are no steps to render.
      return;
    }

    // Render each step in gray as a spoke with a circle at the end.
    for (int i=0;i<n;i++)
    {
      // Divide the circle into n wedges, 1 for each step.
      theta = i * M_PI * 2 / n;
      // Calculate the screen coordinates of the point on the circle that is theta radians along.
      x = sinf(theta);
      y = cosf(theta);
      x = centerX + radius * x;
      y = centerY + radius * y;
      // Draw a gray line from the center of the circle to this point.
      fb.line(GRAY7, centerX, centerY, x, y);
      if(mpSequencer->mSpace[i] > 0.0f)
      {
        // Draw a filled circle to indicate this step has a cat in it.
        fb.fillCircle(GRAY7, x, y, CURSOR);
      }
    }

    // Render the current position in white.
    int phase = mpSequencer->mPhase;  // Phase is synonym for current position.
    // Calculate the screen coordinates of the point on the circle that is theta radians along.
    theta =  phase * M_PI * 2 / n;
    x = sinf(theta);
    y = cosf(theta);
    x = centerX + x * radius;
    y = centerY + y * radius;
    // Draw a white line from the center of the circle to this point.
    fb.line(mForeground, centerX, centerY, x, y);
    // Draw a white filled circle to indicate this step promimently.
    fb.fillCircle(mForeground, x, y, CURSOR);
    
  }
}

void CatCircle::follow(EuclideanSequencer *pSequencer)
{
  if (mpSequencer)
  {
    mpSequencer->release();
  }
  mpSequencer = pSequencer;
  if (mpSequencer)
  {
    mpSequencer->attach();
  }
}
