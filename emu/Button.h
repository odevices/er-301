#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>

namespace emu
{

  class Window;
  struct Button
  {
    Button(const char *label, uint32_t id, int x, int y, int r, int g, int b, const char *key);
    void draw(Window *window);

    const char *label;
    uint32_t id;
    const char *key;
    SDL_Rect rect;
    SDL_Color up;
    SDL_Color down;
  };

} // namespace emu