#pragma once

#include <od/extras/LockFreeQueue.h>
#include <emu/Window.h>
#include <emu/KeyValueStore.h>
#include <hal/display.h>
#include <SDL2/SDL.h>
#include <map>

namespace emu
{
  struct Emulator
  {
    Emulator();
    int run(const char * xRoot, const char * rearRoot, const char * frontRoot);
    void putDisplayBuffer(DisplayBuffer *buffer);
    DisplayBuffer *getDisplayBuffer();
    int getEncoderValue();

  private:
    void loop();
    void handleKeyUp(SDL_Keycode key);
    void handleKeyDown(SDL_Keycode key);
    void handleMouseButton(SDL_MouseButtonEvent & e);
    void saveState();
    void restoreState();
    std::string getDBFilename();

    KeyValueStore db;
    Window *window = 0;
    DisplayBuffer ping, pong;
    od::LockFreeQueue<DisplayBuffer *, 4> readyQ, renderQ;
    uint customEventType = SDL_USEREVENT;
    std::map<SDL_Keycode, uint32_t> keyMap;
    std::map<uint32_t, SDL_Rect> buttonMap;
    std::map<uint32_t, SDL_Rect> toggleMap;
    int encoderValue = INT32_MAX / 2;
    bool quit = false;
    bool lctrl = false;
    bool rctrl = false;
    bool zDown = false;
    bool xDown = false;
    const char * xRoot;
    const char * rearRoot;
    const char * frontRoot;
  };
}