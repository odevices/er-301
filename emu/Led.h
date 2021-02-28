#pragma once

#include <emu/constants.h>
#include <SDL2/SDL.h>
#include <stdint.h>

namespace emu
{
  class Window;
  struct Led
  {
    Led(const char *label, uint32_t id, int x, int y, int red, int green, int blue, bool side = true);
    void draw(Window *window);

    bool state = false;
    SDL_Texture *texture = 0;
    const char *label;
    uint32_t id;
    int x;
    int y;
    int red;
    int green;
    int blue;
    bool side;
  };

  struct RedLed : public Led
  {
    RedLed(const char *label, uint32_t id, int x, int y, bool side = true)
        : Led(label, id, x, y, RL_RED, RL_GREEN, RL_BLUE, side)
    {
    }
  };

  struct OrangeLed : public Led
  {
    OrangeLed(uint32_t id, int x, int y)
        : Led(0, id, x, y, OL_RED, OL_GREEN, OL_BLUE, false)
    {
    }
  };

} // namespace emu