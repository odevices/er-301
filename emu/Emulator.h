#pragma once

#include <od/extras/LockFreeQueue.h>
#include <emu/Window.h>
#include <hal/display.h>
#include <SDL2/SDL.h>
#include <map>

namespace emu
{
  struct Emulator
  {
    Emulator();
    int run(int argc, char **argv);
    void putDisplayBuffer(DisplayBuffer *buffer);
    DisplayBuffer *getDisplayBuffer();
    int getEncoderValue();

  private:
    void loop();
    void handleKeyUp(SDL_Keysym sym);
    void handleKeyDown(SDL_Keysym sym);
    void handleMouseButton(SDL_MouseButtonEvent &e);
    void mapButtonToKey(uint32_t id, const std::string &key);

    bool writeDefaultConfiguration(const std::string &filename);
    void loadDefaultConfiguration();
    bool loadConfiguration(const std::string &filename);
    std::string xRoot;
    std::string rearRoot;
    std::string frontRoot;
    std::string configRoot;
    std::string sessionFilename;
    std::string configFilename;
    double mouseWheelToKnobFactor;
    double leftRightToKnobFactor;
    double upDownToKnobFactor;

    // Persist state between sessions.
    void saveState();
    void restoreState();

    Window *window = 0;
    DisplayBuffer ping, pong;
    od::LockFreeQueue<DisplayBuffer *, 4> readyQ, renderQ;
    uint customEventType = SDL_USEREVENT;
    double encoderValue = 0;
    bool quit = false;
    bool storageToggleFocused = false;
    bool modeToggleFocused = false;

    // Keyboard Mapping
    std::map<std::string, uint32_t> keyGpioMap;
    std::map<uint32_t, std::string> gpioKeyMap;
    std::string storageToggleFocusKey;
    std::string modeToggleFocusKey;
    std::string zoomInKey;
    std::string zoomOutKey;
    std::string quitKey; // Must be modified with CTRL.

    // Mouse Mapping
    std::map<uint32_t, SDL_Rect> buttonHitMap;
    std::map<uint32_t, SDL_Rect> toggleHitMap;
  };
}