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
    int run(const char *xRoot, const char *rearRoot, const char *frontRoot);
    void putDisplayBuffer(DisplayBuffer *buffer);
    DisplayBuffer *getDisplayBuffer();
    int getEncoderValue();

  private:
    void loop();
    void handleKeyUp(SDL_Keysym sym);
    void handleKeyDown(SDL_Keysym sym);
    void handleMouseButton(SDL_MouseButtonEvent &e);
    void saveState();
    void restoreState();
    std::string getDBFilename();
    void mapButtonToKey(uint32_t id, const std::string& key);

    KeyValueStore db;
    Window *window = 0;
    DisplayBuffer ping, pong;
    od::LockFreeQueue<DisplayBuffer *, 4> readyQ, renderQ;
    uint customEventType = SDL_USEREVENT;
    int encoderValue = INT32_MAX / 2;
    bool quit = false;
    bool storageToggleFocused = false;
    bool modeToggleFocused = false;
    const char *xRoot;
    const char *rearRoot;
    const char *frontRoot;

    // Keyboard Mapping
    std::map<std::string, uint32_t> keyGpioMap;
    std::map<uint32_t, std::string> gpioKeyMap;
    std::string storageToggleFocusKey{"Z"};
    std::string modeToggleFocusKey{"X"};
    std::string zoomInKey{"="}; 
    std::string zoomOutKey{"-"};
    std::string quitKey{"Q"}; // CTRL+q

    // Mouse Mapping
    std::map<uint32_t, SDL_Rect> buttonHitMap;
    std::map<uint32_t, SDL_Rect> toggleHitMap;
  };
}