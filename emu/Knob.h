#pragma once

#include <emu/constants.h>
#include <SDL2/SDL.h>

namespace emu
{
  class Window;
  struct Knob
  {
    Knob();
    ~Knob();
    void draw(Window *window);
    void rotate(double angle);

    SDL_Texture *texture = 0;
    SDL_Rect dstRect{
        .x = KNOB_X,
        .y = KNOB_Y,
        .w = KNOB_W,
        .h = KNOB_H};
    SDL_Point center{
        .x = KNOB_W / 2,
        .y = KNOB_H / 2};
    double angle = 0;
  };

} // namespace emu