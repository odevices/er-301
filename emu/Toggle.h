#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>

namespace emu
{

  class Window;
  struct Toggle
  {
    Toggle(const char * label, uint32_t idA, uint32_t idB, int x, int y,
           const char *key, const char *up, const char *mid, const char *down);
    void draw(Window *window);

    int state = -1;
    SDL_Texture *texture = 0;
    const char * label;
    uint32_t idA;
    uint32_t idB;
    int x;
    int y;
    const char *key;
    const char *up;
    const char *mid;
    const char *down;
  };

} // namespace emu