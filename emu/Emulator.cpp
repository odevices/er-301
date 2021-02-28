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

  void Emulator::handleKeyUp(SDL_Keycode key)
  {
    if (key == SDLK_z)
    {
      zDown = false;
    }
    else if (key == SDLK_x)
    {
      xDown = false;
    }
    else if (key == SDLK_LCTRL)
    {
      lctrl = false;
    }
    else if (key == SDLK_RCTRL)
    {
      rctrl = false;
    }
    if (lctrl || rctrl)
    {
      // do nothing
    }
    else if (key == SDLK_EQUALS)
    {
      window->setScale(window->scale + 0.05f);
    }
    else if (key == SDLK_MINUS)
    {
      window->setScale(window->scale - 0.05f);
    }
    else
    {
      auto i = keyMap.find(key);
      if (i != keyMap.end())
      {
        uint id = (*i).second;
        Gpio_write(id, true);
      }
    }
  }

  void Emulator::handleKeyDown(SDL_Keycode key)
  {
    if (key == SDLK_z)
    {
      zDown = true;
    }
    else if (key == SDLK_x)
    {
      xDown = true;
    }
    else if (key == SDLK_LCTRL)
    {
      lctrl = true;
    }
    else if (key == SDLK_RCTRL)
    {
      rctrl = true;
    }
    else if (lctrl || rctrl)
    {
      if (key == SDLK_q)
      {
        quit = true;
      }
    }

    if (key == SDLK_UP && zDown)
    {
      switchUp(TOGGLE_STORAGE_A, TOGGLE_STORAGE_B);
    }
    else if (key == SDLK_UP && xDown)
    {
      switchUp(TOGGLE_MODE_A, TOGGLE_MODE_B);
    }
    else if (key == SDLK_DOWN && zDown)
    {
      switchDown(TOGGLE_STORAGE_A, TOGGLE_STORAGE_B);
    }
    else if (key == SDLK_DOWN && xDown)
    {
      switchDown(TOGGLE_MODE_A, TOGGLE_MODE_B);
    }
    else if (key == SDLK_LEFT || key == SDLK_UP)
    {
      encoderValue -= ENCODER_SPEED;
      window->knob.rotate(-KNOB_SPEED);
    }
    else if (key == SDLK_RIGHT || key == SDLK_DOWN)
    {
      encoderValue += ENCODER_SPEED;
      window->knob.rotate(KNOB_SPEED);
    }
    else
    {
      auto i = keyMap.find(key);
      if (i != keyMap.end())
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
    for (auto &kv : buttonMap)
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
            handleKeyDown(e.key.keysym.sym);
            break;
          case SDL_KEYUP:
            logDebug(2, "Key Up: [%s]", SDL_GetKeyName(e.key.keysym.sym));
            handleKeyUp(e.key.keysym.sym);
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

    keyMap[SDLK_q] = BUTTON_MAIN1;
    keyMap[SDLK_w] = BUTTON_MAIN2;
    keyMap[SDLK_e] = BUTTON_MAIN3;
    keyMap[SDLK_r] = BUTTON_MAIN4;
    keyMap[SDLK_t] = BUTTON_MAIN5;
    keyMap[SDLK_y] = BUTTON_MAIN6;
    keyMap[SDLK_a] = BUTTON_DIAL1;
    keyMap[SDLK_s] = BUTTON_DIAL2;
    keyMap[SDLK_d] = BUTTON_DIAL3;
    keyMap[SDLK_f] = BUTTON_SUB1;
    keyMap[SDLK_g] = BUTTON_SUB2;
    keyMap[SDLK_h] = BUTTON_SUB3;
    keyMap[SDLK_v] = BUTTON_ENTER;
    keyMap[SDLK_b] = BUTTON_UP;
    keyMap[SDLK_n] = BUTTON_SHIFT;
    keyMap[SDLK_1] = BUTTON_SELECT1;
    keyMap[SDLK_2] = BUTTON_SELECT2;
    keyMap[SDLK_3] = BUTTON_SELECT3;
    keyMap[SDLK_4] = BUTTON_SELECT4;
    keyMap[SDLK_KP_1] = BUTTON_SELECT1;
    keyMap[SDLK_KP_2] = BUTTON_SELECT2;
    keyMap[SDLK_KP_3] = BUTTON_SELECT3;
    keyMap[SDLK_KP_4] = BUTTON_SELECT4;

    buttonMap[BUTTON_MAIN1] = {.x = MB1_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_MAIN2] = {.x = MB2_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_MAIN3] = {.x = MB3_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_MAIN4] = {.x = MB4_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_MAIN5] = {.x = MB5_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_MAIN6] = {.x = MB6_X, .y = MB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_DIAL1] = {.x = MB1_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_DIAL2] = {.x = MB2_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_DIAL3] = {.x = MB3_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_SUB1] = {.x = MB4_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_SUB2] = {.x = MB5_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_SUB3] = {.x = MB6_X, .y = SB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_ENTER] = {.x = MB4_X, .y = HB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_UP] = {.x = MB5_X, .y = HB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_SHIFT] = {.x = MB6_X, .y = HB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_SELECT1] = {.x = JB1_X, .y = JB1_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_SELECT2] = {.x = JB1_X, .y = JB2_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_SELECT3] = {.x = JB1_X, .y = JB3_Y, .w = BUTTON_W, .h = BUTTON_H};
    buttonMap[BUTTON_SELECT4] = {.x = JB1_X, .y = JB4_Y, .w = BUTTON_W, .h = BUTTON_H};

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
