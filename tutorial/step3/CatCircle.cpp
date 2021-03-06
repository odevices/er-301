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
  int radius = MIN(mWidth / 2, mHeight / 2) - MARGIN - CURSOR;
  int centerX = mWorldLeft + mWidth / 2;
  int centerY = mWorldBottom + mHeight / 2;
  fb.circle(GRAY5, centerX, centerY, radius);

  if (mpSequencer)
  {
    float theta, x, y;
    int phase = mpSequencer->mPhase;
    int n = mpSequencer->mCachedBoxes;
    if(n < 1)
    {
      return;
    }

    for (int i=0;i<n;i++)
    {
      theta = i * M_PI * 2 / n;
      x = sinf(theta);
      y = cosf(theta);
      x = centerX + radius * x;
      y = centerY + radius * y;
      fb.line(GRAY7, centerX, centerY, x, y);
      if(mpSequencer->mSpace[i] > 0.0f)
      {
        fb.fillCircle(GRAY7, x, y, CURSOR);
      }
    }

    // current position
    theta =  phase * M_PI * 2 / n;
    x = sinf(theta);
    y = cosf(theta);
    x = centerX + x * radius;
    y = centerY + y * radius;
    fb.line(mForeground, centerX, centerY, x, y);
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
