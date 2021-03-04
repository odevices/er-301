#include <emu/Button.h>
#include <emu/Window.h>
#include <emu/constants.h>

namespace emu
{
  Button::Button(const char *label, uint32_t id, int x, int y, int r, int g, int b) : label(label), id(id)
  {
    rect.x = x;
    rect.y = y;
    rect.w = BUTTON_W;
    rect.h = BUTTON_H;
    up.r = r;
    up.g = g;
    up.b = b;
    up.a = 255;
    down.r = up.r * 0.75;
    down.g = up.g * 0.75;
    down.b = up.b * 0.75;
    down.a = 255;
  }

  void Button::draw(Window *window)
  {
    if (Button_pressed(id))
    {
      SDL_SetRenderDrawColor(window->renderer, down.r, down.g, down.b, down.a);
    }
    else
    {
      SDL_SetRenderDrawColor(window->renderer, up.r, up.g, up.b, up.a);
    }
    SDL_RenderFillRect(window->renderer, &rect);
    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(window->renderer, &rect);
    window->drawText(rect.x + B_SPACING, rect.y + B_SPACING, 16, key.c_str());
    window->drawTextAligned(FC_ALIGN_CENTER, rect.x + BUTTON_W / 2, rect.y - 12, 10, label.c_str());
  }
} // namespace emu