#include <emu/draw.h>
#include <emu/constants.h>
#include <math.h>

namespace emu
{

  void drawCircleOS(SDL_Renderer *renderer, int xCenter, int yCenter, int r)
  {
    drawFilledRing(renderer, xCenter, yCenter, r - OVERSAMPLE / 2, r + OVERSAMPLE / 2 + 1);
  }

  void drawCircle(SDL_Renderer *renderer, int xCenter, int yCenter, int r)
  {
    int n = r * 16 * OVERSAMPLE;
    double dAngle = 2 * M_PI / n;
    for (int i = 0; i < n; i++)
    {
      double a = i * dAngle;
      double s = sin(a), c = cos(a);
      int x = r * s + xCenter;
      int y = r * c + yCenter;
      SDL_RenderDrawPoint(renderer, x, y);
    }
  }

  void drawFilledRing(SDL_Renderer *renderer, int xCenter, int yCenter, int r1, int r2)
  {
    for (int i = r1; i < r2; i++)
    {
      drawCircle(renderer, xCenter, yCenter, i);
    }
  }

  void drawFilledCircle(SDL_Renderer *renderer, int xCenter, int yCenter, int r)
  {
    for (int i = 0; i < r; i++)
    {
      drawCircle(renderer, xCenter, yCenter, i);
    }
  }

  void drawPolygon(SDL_Renderer *renderer, int xCenter, int yCenter, int r, int nsides)
  {
    double dAngle = 2 * M_PI / nsides;
    int x0 = xCenter, y0 = r + yCenter;
    for (int i = 1; i < nsides; i++)
    {
      double a = i * dAngle;
      double s = sin(a), c = cos(a);
      int x = r * s + xCenter;
      int y = r * c + yCenter;
      SDL_RenderDrawLine(renderer, x0, y0, x, y);
      x0 = x;
      y0 = y;
    }
    SDL_RenderDrawLine(renderer, x0, y0, xCenter, r + yCenter);
  }

  void drawFastCircle(SDL_Renderer *renderer, int xCenter, int yCenter, int r)
  {
    const int diameter = (r * 2);

    int x = (r - 1);
    int y = 0;
    int tx = 1;
    int ty = 1;
    int error = (tx - diameter);

    while (x >= y)
    {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, xCenter + x, yCenter - y);
      SDL_RenderDrawPoint(renderer, xCenter + x, yCenter + y);
      SDL_RenderDrawPoint(renderer, xCenter - x, yCenter - y);
      SDL_RenderDrawPoint(renderer, xCenter - x, yCenter + y);
      SDL_RenderDrawPoint(renderer, xCenter + y, yCenter - x);
      SDL_RenderDrawPoint(renderer, xCenter + y, yCenter + x);
      SDL_RenderDrawPoint(renderer, xCenter - y, yCenter - x);
      SDL_RenderDrawPoint(renderer, xCenter - y, yCenter + x);

      if (error <= 0)
      {
        ++y;
        error += ty;
        ty += 2;
      }

      if (error > 0)
      {
        --x;
        tx += 2;
        error += (tx - diameter);
      }
    }
  }

} // namespace emu
