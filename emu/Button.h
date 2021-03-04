#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>
#include <string>

namespace emu
{

  class Window;
  struct Button
  {
    Button(const char * label, uint32_t id, int x, int y, int r, int g, int b);
    void draw(Window *window);

    std::string key;
    std::string label;
    uint32_t id;
    SDL_Rect rect;
    SDL_Color up;
    SDL_Color down;
  };

} // namespace emu