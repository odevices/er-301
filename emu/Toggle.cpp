#include <emu/Toggle.h>
#include <emu/Window.h>
#include <emu/constants.h>
#include <emu/draw.h>
#include <hal/gpio.h>
#include <hal/log.h>

namespace emu
{
  Toggle::Toggle(const char *label, uint32_t idA, uint32_t idB, int x, int y,
                 const char *key, const char *up, const char *mid, const char *down)
      : label(label), idA(idA), idB(idB), x(x), y(y), key(key), up(up), mid(mid), down(down)
  {
  }

  void Toggle::draw(Window *window)
  {
    int cur;
    if (Gpio_read(idA))
    {
      // up
      cur = 3;
    }
    else if (Gpio_read(idB))
    {
      // dn
      cur = 1;
    }
    else
    {
      // mid
      cur = 2;
    }

    if ((cur != state) && (texture != 0))
    {
      SDL_DestroyTexture(texture);
      texture = 0;
    }

    state = cur;

    if (texture == 0)
    {
      int r1 = T_R1 * OVERSAMPLE;
      int r2 = T_R2 * OVERSAMPLE;
      int w = TOGGLE_W * OVERSAMPLE;
      int h = TOGGLE_H * OVERSAMPLE;
      int s = T_SPACING * OVERSAMPLE;

      SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
      if (surface == NULL)
      {
        logFatal("Failed to create toggle surface: %s", SDL_GetError());
      }

      SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
      if (renderer == NULL)
      {
        logFatal("Failed to create toggle renderer: %s", SDL_GetError());
      }

      SDL_SetRenderDrawColor(renderer, P_GRAY, P_GRAY, P_GRAY, 0);
      SDL_RenderClear(renderer);

      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      drawCircleOS(renderer, r2, h / 2 - s, r1);
      drawCircleOS(renderer, r2, h / 2, r1);
      drawCircleOS(renderer, r2, h / 2 + s, r1);

      SDL_SetRenderDrawColor(renderer, T_GRAY, T_GRAY, T_GRAY, 0);
      if (Gpio_read(idA))
      {
        // up
        drawFilledCircle(renderer, r2, h / 2 - s, r2);
      }
      else if (Gpio_read(idB))
      {
        // dn
        drawFilledCircle(renderer, r2, h / 2 + s, r2);
      }
      else
      {
        // mid
        drawFilledCircle(renderer, r2, h / 2, r2);
      }

      texture = SDL_CreateTextureFromSurface(window->renderer, surface);
      if (texture == NULL)
      {
        logFatal("Failed to create toggle texture: %s", SDL_GetError());
      }

      SDL_DestroyRenderer(renderer);
      SDL_FreeSurface(surface);
    }

    SDL_Rect dstRect{.x = x,
                     .y = y,
                     .w = TOGGLE_W,
                     .h = TOGGLE_H};
    SDL_RenderCopy(window->renderer, texture, 0, &dstRect);

    window->drawText(x + T_MARGIN, y + TOGGLE_H / 2 - T_SPACING - T_FONT / 2, T_FONT, up);
    window->drawText(x + T_MARGIN, y + TOGGLE_H / 2 - T_FONT / 2, T_FONT, mid);
    window->drawText(x + T_MARGIN, y + TOGGLE_H / 2 + T_SPACING - T_FONT / 2, T_FONT, down);
    if (label)
    {
      window->drawTextAligned(FC_ALIGN_LEFT, x, y - 12, 10, label);
    }
  }
} // namespace emu