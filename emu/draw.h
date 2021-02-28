#pragma once

#include <SDL2/SDL.h>

namespace emu
{

  void drawCircle(SDL_Renderer *renderer, int xCenter, int yCenter, int r);
  void drawFilledCircle(SDL_Renderer *renderer, int xCenter, int yCenter, int r);
  void drawFilledRing(SDL_Renderer *renderer, int xCenter, int yCenter, int r1, int r2);
  void drawPolygon(SDL_Renderer *renderer, int xCenter, int yCenter, int r, int nsides);
  void drawFastCircle(SDL_Renderer *renderer, int xCenter, int yCenter, int r);

  // oversampled
  void drawCircleOS(SDL_Renderer *renderer, int xCenter, int yCenter, int r);

} // namespace emu