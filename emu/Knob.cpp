#include <emu/Knob.h>
#include <emu/Window.h>
#include <emu/draw.h>
#include <hal/log.h>

namespace emu
{

  Knob::Knob()
  {
  }

  Knob::~Knob()
  {
    if (texture)
    {
      SDL_DestroyTexture(texture);
    }
  }

  void Knob::draw(Window *window)
  {
    if (texture == 0)
    {
      int w = KNOB_W * OVERSAMPLE;
      int h = KNOB_H * OVERSAMPLE;
      int r = KNOB_R * OVERSAMPLE;
      int r2 = 4 * OVERSAMPLE;

      SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
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
      drawFilledCircle(renderer, w / 2, h / 2, r);

      // knurls
      int n = 8;
      double dAngle = 2 * M_PI / n;
      SDL_SetRenderDrawColor(renderer, P_GRAY, P_GRAY, P_GRAY, 255);
      for (int i = 0; i < n; i++)
      {
        double a = i * dAngle;
        double s = sin(a), c = cos(a);
        int x = (r + 2) * s + w / 2;
        int y = (r + 2) * c + h / 2;
        drawFilledCircle(renderer, x, y, r2);
      }

      texture = SDL_CreateTextureFromSurface(window->renderer, surface);
      if (texture == NULL)
      {
        logFatal("Failed to create knob texture: %s", SDL_GetError());
      }

      SDL_DestroyRenderer(renderer);
      SDL_FreeSurface(surface);
    }

    SDL_RenderCopyEx(window->renderer, texture, 0, &dstRect, angle, &center, SDL_FLIP_NONE);
  }

  void Knob::rotate(double _angle)
  {
    angle += _angle;
  }

} // namespace emu
