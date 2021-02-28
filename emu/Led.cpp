
#include <emu/Led.h>
#include <emu/Window.h>
#include <emu/draw.h>
#include <hal/log.h>
#include <SDL2/SDL.h>

namespace emu
{
  Led::Led(const char *label, uint32_t id, int x, int y, int red, int green, int blue, bool side)
      : label(label), id(id), x(x), y(y), red(red), green(green), blue(blue), side(side)
  {
  }

  void Led::draw(Window *window)
  {
    int cur = Gpio_read(id);
    if ((cur != state) && (texture != 0))
    {
      SDL_DestroyTexture(texture);
      texture = 0;
    }
    state = cur;

    if (texture == 0)
    {
      int r = LED_R * OVERSAMPLE;
      SDL_Surface *surface = SDL_CreateRGBSurface(0, 2 * r, 2 * r, 32, 0, 0, 0, 0);
      if (surface == NULL)
      {
        logFatal("Failed to create led surface: %s", SDL_GetError());
      }

      SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
      if (renderer == NULL)
      {
        logFatal("Failed to create led renderer: %s", SDL_GetError());
      }

      SDL_SetRenderDrawColor(renderer, P_GRAY, P_GRAY, P_GRAY, 0);
      SDL_RenderClear(renderer);
      if (state)
      {
        SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
      }
      else
      {
        SDL_SetRenderDrawColor(renderer, red / 2, green / 2, blue / 2, 255);
      }
      drawFilledCircle(renderer, r, r, r);

      texture = SDL_CreateTextureFromSurface(window->renderer, surface);
      if (texture == NULL)
      {
        logFatal("Failed to create led texture: %s", SDL_GetError());
      }

      SDL_DestroyRenderer(renderer);
      SDL_FreeSurface(surface);
    }

    SDL_Rect dstRect{.x = x - LED_R,
                     .y = y - LED_R,
                     .w = 2 * LED_R,
                     .h = 2 * LED_R};
    SDL_RenderCopy(window->renderer, texture, 0, &dstRect);
    if (label)
    {
      if (side)
      {
        // to the right
        window->drawTextAligned(FC_ALIGN_LEFT, x + LED_R + 2, y - 4, 8, label);
      }
      else
      {
        // to the left
        window->drawTextAligned(FC_ALIGN_RIGHT, x - LED_R, y - 4, 8, label);
      }
    }
  }

} // namespace emu