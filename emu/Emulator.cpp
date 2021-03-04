#include <emu/Emulator.h>
#include <emu/tls.h>
#include <od/glue/AppInterpreter.h>
#include <od/extras/Random.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 5
#include <hal/log.h>
#include <hal/timing.h>
#include <hal/heap.h>
#include <hal/uart.h>
#include <hal/events.h>
#include <hal/card.h>
#include <hal/gpio.h>
#include <hal/rng.h>
#include <hal/usb.h>
#include <hal/encoder.h>
#include <hal/pwm.h>
#include <hal/adc.h>
#include <hal/fileops.h>
#include <hal/modulation.h>
#include <hal/audio.h>
#include <hal/channels.h>
#include <hal/ops.h>
#include <hal/pump.h>
#include <od/config.h>

using namespace od;

namespace emu
{

  Emulator::Emulator()
  {
  }

  static int switchState(uint32_t idA, uint32_t idB)
  {
    if (Gpio_read(idA))
      return 1;
    else if (Gpio_read(idB))
      return 3;
    else
      return 2;
  }

  static void switchDown(uint32_t idA, uint32_t idB)
  {
    if (Gpio_read(idA))
      Gpio_write(idA, false);
    else if (Gpio_read(idB))
      return;
    else
      Gpio_write(idB, true);
  }

  static void switchUp(uint32_t idA, uint32_t idB)
  {
    if (Gpio_read(idA))
      return;
    else if (Gpio_read(idB))
      Gpio_write(idB, false);
    else
      Gpio_write(idA, true);
  }

  void Emulator::handleKeyUp(SDL_Keysym keysym)
  {
    std::string name = SDL_GetKeyName(keysym.sym);
    if (name == storageToggleFocusKey)
    {
      storageToggleFocused = false;
    }
    else if (name == modeToggleFocusKey)
    {
      modeToggleFocused = false;
    }
    else if (name == zoomInKey)
    {
      window->setScale(window->scale + 0.05f);
    }
    else if (name == zoomOutKey)
    {
      window->setScale(window->scale - 0.05f);
    }
    else
    {
      auto i = keyGpioMap.find(name);
      if (i != keyGpioMap.end())
      {
        uint id = (*i).second;
        Gpio_write(id, true);
      }
    }
  }

  void Emulator::handleKeyDown(SDL_Keysym keysym)
  {
    std::string name = SDL_GetKeyName(keysym.sym);
    if (name == storageToggleFocusKey)
    {
      storageToggleFocused = true;
    }
    else if (name == modeToggleFocusKey)
    {
      modeToggleFocused = true;
    }
    else if (name == quitKey && (keysym.mod & KMOD_CTRL))
    {
      quit = true;
    }
    else if (keysym.scancode == SDL_SCANCODE_UP && storageToggleFocused)
    {
      switchUp(TOGGLE_STORAGE_A, TOGGLE_STORAGE_B);
    }
    else if (keysym.scancode == SDL_SCANCODE_UP && modeToggleFocused)
    {
      switchUp(TOGGLE_MODE_A, TOGGLE_MODE_B);
    }
    else if (keysym.scancode == SDL_SCANCODE_DOWN && storageToggleFocused)
    {
      switchDown(TOGGLE_STORAGE_A, TOGGLE_STORAGE_B);
    }
    else if (keysym.scancode == SDL_SCANCODE_DOWN && modeToggleFocused)
    {
      switchDown(TOGGLE_MODE_A, TOGGLE_MODE_B);
    }
    else if (keysym.scancode == SDL_SCANCODE_LEFT || keysym.scancode == SDL_SCANCODE_UP)
    {
      encoderValue -= ENCODER_SPEED;
      window->knob.rotate(-KNOB_SPEED);
    }
    else if (keysym.scancode == SDL_SCANCODE_RIGHT || keysym.scancode == SDL_SCANCODE_DOWN)
    {
      encoderValue += ENCODER_SPEED;
      window->knob.rotate(KNOB_SPEED);
    }
    else
    {
      auto i = keyGpioMap.find(name);
      if (i != keyGpioMap.end())
      {
        uint id = (*i).second;
        Gpio_write(id, false);
      }
    }
  }

  static bool hit(int x, int y, SDL_Rect &rect)
  {
    return x > rect.x && x < rect.x + rect.w && y > rect.y && y < rect.y + rect.h;
  }

  static void handleSwitchHit(uint32_t idA, uint32_t idB, float p)
  {
    int state = switchState(idA, idB);
    if (p < 0.333f)
    {
      if (state == 2)
      {
        switchUp(idA, idB);
      }
      else if (state == 3)
      {
        switchUp(idA, idB);
        switchUp(idA, idB);
      }
    }
    else if (p < 0.667f)
    {
      if (state == 1)
      {
        switchDown(idA, idB);
      }
      else if (state == 3)
      {
        switchUp(idA, idB);
      }
    }
    else
    {
      if (state == 2)
      {
        switchDown(idA, idB);
      }
      else if (state == 1)
      {
        switchDown(idA, idB);
        switchDown(idA, idB);
      }
    }
  }

  void Emulator::handleMouseButton(SDL_MouseButtonEvent &e)
  {
    for (auto &kv : buttonHitMap)
    {
      uint32_t id = kv.first;
      SDL_Rect &rect = kv.second;
      if (hit(e.x, e.y, rect))
      {
        // hit!
        Gpio_write(id, e.state == SDL_RELEASED);
        return;
      }
    }

    if (e.state == SDL_RELEASED)
    {
      SDL_Rect storage{.x = T_STORAGE_X, .y = T_STORAGE_Y, .w = TOGGLE_W, .h = TOGGLE_H};
      if (hit(e.x, e.y, storage))
      {
        float p = (e.y - T_STORAGE_Y) / (float)TOGGLE_H;
        handleSwitchHit(TOGGLE_STORAGE_A, TOGGLE_STORAGE_B, p);
        return;
      }

      SDL_Rect mode{.x = T_MODE_X, .y = T_MODE_Y, .w = TOGGLE_W, .h = TOGGLE_H};
      if (hit(e.x, e.y, mode))
      {
        float p = (e.y - T_MODE_Y) / (float)TOGGLE_H;
        handleSwitchHit(TOGGLE_MODE_A, TOGGLE_MODE_B, p);
        return;
      }
    }
  }

  void Emulator::loop()
  {
    int delay = 0;
    quit = false;
    while (!quit)
    {
      tick_t start = wallclock();
      SDL_Event e;
      Pump_resetThrottle();
      //SDL_WaitEventTimeout(0, delay);
      if (delay > 0)
      {
        SDL_Delay(delay);
      }
      while (SDL_PollEvent(&e))
      {
        if (e.type == customEventType)
        {
        }
        else
        {
          switch (e.type)
          {
          case SDL_QUIT:
            quit = true;
            break;
          case SDL_KEYDOWN:
            logDebug(2, "Key Down: [%s]", SDL_GetKeyName(e.key.keysym.sym));
            handleKeyDown(e.key.keysym);
            break;
          case SDL_KEYUP:
            logDebug(2, "Key Up: [%s]", SDL_GetKeyName(e.key.keysym.sym));
            handleKeyUp(e.key.keysym);
            break;
          case SDL_MOUSEBUTTONDOWN:
          case SDL_MOUSEBUTTONUP:
            e.button.x /= window->scale;
            e.button.y /= window->scale;
            handleMouseButton(e.button);
            break;
          case SDL_MOUSEWHEEL:
            encoderValue += e.wheel.y * KNOB_SPEED;
            window->knob.rotate(e.wheel.y * KNOB_SPEED);
            break;
          case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
              window->onResized(e.window.data1, e.window.data2);
              logDebug(2, "Resize: %d x %d", e.window.data1, e.window.data2);
            }
            break;
          default:
            break;
          }
        }
      }

      DisplayBuffer *buffer;
      while (renderQ.pop(&buffer))
      {
        window->update(buffer->main, buffer->sub);
        readyQ.push(buffer);
      }
      Events_push(EVENT_DISPLAY_READY);

      double elapsed = ticks2secsD(wallclock() - start);
      double target = 1.0 / 70;
      double diff = MAX(0, elapsed - target);
      delay = (int)(1000 * (target - diff));
      if (delay < 0)
      {
        delay = 0;
      }
    }
  }

  DisplayBuffer *Emulator::getDisplayBuffer()
  {
    DisplayBuffer *buffer = 0;
    readyQ.pop(&buffer);
    return buffer;
  }

  void Emulator::putDisplayBuffer(DisplayBuffer *buffer)
  {
    renderQ.push(buffer);
    SDL_Event e;
    SDL_zero(e);
    e.type = customEventType;
    SDL_PushEvent(&e);
  }

  int Emulator::getEncoderValue()
  {
    return encoderValue;
  }

  std::string Emulator::getDBFilename()
  {
    std::string dbFilename = globalConfig.rearRoot;
    dbFilename += "/emu.db";
    return dbFilename;
  }

  void Emulator::restoreState()
  {
    // Restore emulator state.
    std::string dbFilename = getDBFilename();
    if (pathExists(dbFilename.c_str()))
    {
      if (db.load(dbFilename))
      {

        // Load key map
        mapButtonToKey(BUTTON_MAIN1, db.get("BUTTON_MAIN1_KEY", "Q"));
        mapButtonToKey(BUTTON_MAIN2, db.get("BUTTON_MAIN2_KEY", "W"));
        mapButtonToKey(BUTTON_MAIN3, db.get("BUTTON_MAIN3_KEY", "E"));
        mapButtonToKey(BUTTON_MAIN4, db.get("BUTTON_MAIN4_KEY", "R"));
        mapButtonToKey(BUTTON_MAIN5, db.get("BUTTON_MAIN5_KEY", "T"));
        mapButtonToKey(BUTTON_MAIN6, db.get("BUTTON_MAIN6_KEY", "Y"));
        mapButtonToKey(BUTTON_DIAL1, db.get("BUTTON_DIAL1_KEY", "A"));
        mapButtonToKey(BUTTON_DIAL2, db.get("BUTTON_DIAL2_KEY", "S"));
        mapButtonToKey(BUTTON_DIAL3, db.get("BUTTON_DIAL3_KEY", "D"));
        mapButtonToKey(BUTTON_SUB1, db.get("BUTTON_SUB1_KEY", "F"));
        mapButtonToKey(BUTTON_SUB2, db.get("BUTTON_SUB2_KEY", "G"));
        mapButtonToKey(BUTTON_SUB3, db.get("BUTTON_SUB3_KEY", "H"));
        mapButtonToKey(BUTTON_ENTER, db.get("BUTTON_ENTER_KEY", "V"));
        mapButtonToKey(BUTTON_UP, db.get("BUTTON_UP_KEY", "B"));
        mapButtonToKey(BUTTON_SHIFT, db.get("BUTTON_SHIFT_KEY", "N"));
        mapButtonToKey(BUTTON_SELECT1, db.get("BUTTON_SELECT1_KEY", "1"));
        mapButtonToKey(BUTTON_SELECT2, db.get("BUTTON_SELECT2_KEY", "2"));
        mapButtonToKey(BUTTON_SELECT3, db.get("BUTTON_SELECT3_KEY", "3"));
        mapButtonToKey(BUTTON_SELECT4, db.get("BUTTON_SELECT4_KEY", "4"));
        storageToggleFocusKey = db.get("STORAGE_FOCUS_KEY", storageToggleFocusKey);
        modeToggleFocusKey = db.get("MODE_FOCUS_KEY", modeToggleFocusKey);
        zoomInKey = db.get("ZOOM_IN_KEY", zoomInKey);
        zoomOutKey = db.get("ZOOM_OUT_KEY", zoomOutKey);
        quitKey = db.get("QUIT_KEY", quitKey);

        Gpio_write(TOGGLE_STORAGE_A, db["TOGGLE_STORAGE_A"] == "1");
        Gpio_write(TOGGLE_STORAGE_B, db["TOGGLE_STORAGE_B"] == "1");
        Gpio_write(TOGGLE_MODE_A, db["TOGGLE_MODE_A"] == "1");
        Gpio_write(TOGGLE_MODE_B, db["TOGGLE_MODE_B"] == "1");

        if (db.has("WINDOW_X"))
        {
          int x = db.getInteger("WINDOW_X");
          int y = db.getInteger("WINDOW_Y");
          int correction = db.getInteger("WINDOW_CORRECTION");
          window->setPosition(x, y, correction);
        }

        if (db.has("WINDOW_SCALE"))
        {
          float scale = db.getFloat("WINDOW_SCALE");
          if (scale > 0.0f)
          {
            window->setScale(scale);
          }
        }

        logInfo("Restored emulator state from %s.", dbFilename.c_str());
      }
      else
      {
        logWarn("Failed to load from %s.", dbFilename.c_str());
      }
    }
  }

  void Emulator::saveState()
  {
    // Save emulator state.
    if (Gpio_read(TOGGLE_STORAGE_A))
    {
      db["TOGGLE_STORAGE_A"] = "1";
    }
    else
    {
      db["TOGGLE_STORAGE_A"] = "0";
    }

    if (Gpio_read(TOGGLE_STORAGE_B))
    {
      db["TOGGLE_STORAGE_B"] = "1";
    }
    else
    {
      db["TOGGLE_STORAGE_B"] = "0";
    }

    if (Gpio_read(TOGGLE_MODE_A))
    {
      db["TOGGLE_MODE_A"] = "1";
    }
    else
    {
      db["TOGGLE_MODE_A"] = "0";
    }

    if (Gpio_read(TOGGLE_MODE_B))
    {
      db["TOGGLE_MODE_B"] = "1";
    }
    else
    {
      db["TOGGLE_MODE_B"] = "0";
    }

    db.setFloat("WINDOW_SCALE", window->scale);

    int x, y;
    window->getPosition(x, y);
    db.setInteger("WINDOW_X", x);
    db.setInteger("WINDOW_Y", y);
    db.setInteger("WINDOW_CORRECTION", window->getTitleBarHeight());

    db["BUTTON_MAIN1_KEY"] = gpioKeyMap[BUTTON_MAIN1];
    db["BUTTON_MAIN2_KEY"] = gpioKeyMap[BUTTON_MAIN2];
    db["BUTTON_MAIN3_KEY"] = gpioKeyMap[BUTTON_MAIN3];
    db["BUTTON_MAIN4_KEY"] = gpioKeyMap[BUTTON_MAIN4];
    db["BUTTON_MAIN5_KEY"] = gpioKeyMap[BUTTON_MAIN5];
    db["BUTTON_MAIN6_KEY"] = gpioKeyMap[BUTTON_MAIN6];
    db["BUTTON_DIAL1_KEY"] = gpioKeyMap[BUTTON_DIAL1];
    db["BUTTON_DIAL2_KEY"] = gpioKeyMap[BUTTON_DIAL2];
    db["BUTTON_DIAL3_KEY"] = gpioKeyMap[BUTTON_DIAL3];
    db["BUTTON_SUB1_KEY"] = gpioKeyMap[BUTTON_SUB1];
    db["BUTTON_SUB2_KEY"] = gpioKeyMap[BUTTON_SUB2];
    db["BUTTON_SUB3_KEY"] = gpioKeyMap[BUTTON_SUB3];
    db["BUTTON_ENTER_KEY"] = gpioKeyMap[BUTTON_ENTER];
    db["BUTTON_UP_KEY"] = gpioKeyMap[BUTTON_UP];
    db["BUTTON_SHIFT_KEY"] = gpioKeyMap[BUTTON_SHIFT];
    db["BUTTON_SELECT1_KEY"] = gpioKeyMap[BUTTON_SELECT1];
    db["BUTTON_SELECT2_KEY"] = gpioKeyMap[BUTTON_SELECT2];
    db["BUTTON_SELECT3_KEY"] = gpioKeyMap[BUTTON_SELECT3];
    db["BUTTON_SELECT4_KEY"] = gpioKeyMap[BUTTON_SELECT4];
    db["STORAGE_FOCUS_KEY"] = storageToggleFocusKey;
    db["MODE_FOCUS_KEY"] = modeToggleFocusKey;
    db["ZOOM_IN_KEY"] = zoomInKey;
    db["ZOOM_OUT_KEY"] = zoomOutKey;
    db["QUIT_KEY"] = quitKey;

    std::string dbFilename = getDBFilename();
    if (db.save(dbFilename))
    {
      logInfo("Saved emulator state to %s.", dbFilename.c_str());
    }
    else
    {
      logWarn("Failed to save to %s.", dbFilename.c_str());
    }
  }

  void Emulator::mapButtonToKey(uint32_t id, const std::string & key)
  {
    keyGpioMap[key] = id;
    gpioKeyMap[id] = key;
    for (Button &b : window->buttons)
    {
      if (id == b.id)
      {
        b.key = key;
      }
    }
  }

  static int interpreterThreadStart(void *ptr)
  {
    TLS_setName("lua");
    AppInterpreter interp;
    interp.init();
    interp.execute("app.EMULATION = true");
    interp.execute("app.roots = {x='%s',rear='%s',front='%s'}",
                   globalConfig.xRoot, globalConfig.rearRoot, globalConfig.frontRoot);
    interp.execute("dofile('%s/boot/logging.lua')", globalConfig.xRoot);
    interp.execute("dofile('%s/boot/start.lua')", globalConfig.xRoot);
    return 0;
  }

  int Emulator::run(const char *xRoot, const char *rearRoot, const char *frontRoot)
  {
    TLS_setName("main");

    logInfo("starting ER-301 Emulator");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0)
    {
      logFatal("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    }

    customEventType = SDL_RegisterEvents(1);
    window = new Window();

    std::string configFilename = rearRoot;
    configFilename += "/firmware.cfg";

    Heap_init();
    Timing_init();
    Uart_init();
    Card_init();
    Config_init(configFilename.c_str(), xRoot, rearRoot, frontRoot);
    Uart_enable();
    Log_init();
    Pump_init();
    Rng_init();
    Gpio_init();
    Events_init();
    USB_init();
    Encoder_init();
    Pwm_init();
    Adc_init();
    Modulation_init();
    Audio_init();
    Display_init();
    od::Random::init();

    memset(ping.main, 0, sizeof(ping.main));
    memset(pong.main, 0, sizeof(pong.main));
    memset(ping.sub, 0, sizeof(ping.sub));
    memset(pong.sub, 0, sizeof(pong.sub));

    readyQ.push(&ping);
    readyQ.push(&pong);
    Events_push(EVENT_DISPLAY_READY);

    buttonHitMap[BUTTON_MAIN1] = {.x = MB1_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_MAIN2] = {.x = MB2_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_MAIN3] = {.x = MB3_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_MAIN4] = {.x = MB4_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_MAIN5] = {.x = MB5_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_MAIN6] = {.x = MB6_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_DIAL1] = {.x = MB1_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_DIAL2] = {.x = MB2_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_DIAL3] = {.x = MB3_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_SUB1] = {.x = MB4_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_SUB2] = {.x = MB5_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_SUB3] = {.x = MB6_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_ENTER] = {.x = MB4_X, .y = HB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_UP] = {.x = MB5_X, .y = HB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_SHIFT] = {.x = MB6_X, .y = HB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_SELECT1] = {.x = JB1_X, .y = JB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_SELECT2] = {.x = JB1_X, .y = JB2_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_SELECT3] = {.x = JB1_X, .y = JB3_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonHitMap[BUTTON_SELECT4] = {.x = JB1_X, .y = JB4_Y, .w = BUTTON_W, .h = BUTTON_H};

    restoreState();

    SDL_Thread *interpreterThread = SDL_CreateThread(interpreterThreadStart, "interp", 0);
    if (interpreterThread == 0)
    {
      logError("Failed to create Interpreter Thread.");
      goto error;
    }

    logInfo("Entering emulator loop.");
    loop();
    logInfo("Exiting emulator loop.");

    Events_push(EVENT_QUIT);
    logInfo("Waiting for interpreter to finish...");
    SDL_WaitThread(interpreterThread, 0);

    saveState();

  error:
    logInfo("Exiting...");
    delete window;
    SDL_Quit();
    return 0;
  }

} // namespace emu
