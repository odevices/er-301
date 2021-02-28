
#include <emu/Jack.h>
#include <emu/Window.h>
#include <emu/constants.h>
#include <emu/draw.h>
#include <hal/log.h>
#include <SDL2/SDL.h>

namespace emu
{
  static SDL_Texture *texture = 0;

  Jack::Jack(uint32_t channel, int x, int y) : channel(channel), x(x), y(y)
  {
  }

  void Jack::draw(Window *window)
  {
    int r = J_OUTER * OVERSAMPLE;
    int r1 = J_HILITE * OVERSAMPLE;
    int r2 = J_INNER * OVERSAMPLE;

    if (texture == 0)
    {
      SDL_Surface *surface = SDL_CreateRGBSurface(0, 2 * r, 2 * r, 32, 0, 0, 0, 0);
      if (surface == NULL)
      {
        logFatal("Failed to create knob surface: %s", SDL_GetError());
      }

      SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
      if (renderer == NULL)
      {
        logFatal("Failed to create knob renderer: %s", SDL_GetError());
      }

      SDL_SetRenderDrawColor(renderer, P_GRAY, P_GRAY, P_GRAY, 0);
      SDL_RenderClear(renderer);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      drawCircleOS(renderer, r, r, r);
      drawCircleOS(renderer, r, r, r1);
      drawFilledCircle(renderer, r, r, r2);

      texture = SDL_CreateTextureFromSurface(window->renderer, surface);
      if (texture == NULL)
      {
        logFatal("Failed to create knob texture: %s", SDL_GetError());
      }

      SDL_DestroyRenderer(renderer);
      SDL_FreeSurface(surface);
    }

    SDL_Rect dstRect{.x = x - J_OUTER,
                     .y = y - J_OUTER,
                     .w = 2 * J_OUTER,
                     .h = 2 * J_OUTER};
    SDL_RenderCopy(window->renderer, texture, 0, &dstRect);
  }

} // namespace emu