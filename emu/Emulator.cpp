#include <emu/Emulator.h>
#include <emu/CommandLine.h>
#include <emu/tls.h>
#include <emu/KeyValueStore.h>
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
#include <limits.h>
#include <iostream>
#include <fstream>

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
    else if (keysym.scancode == SDL_SCANCODE_LEFT)
    {
      encoderValue -= leftRightToKnobFactor * ENCODER_SPEED;
      window->knob.rotate(-KNOB_SPEED);
    }
    else if (keysym.scancode == SDL_SCANCODE_RIGHT)
    {
      encoderValue += leftRightToKnobFactor * ENCODER_SPEED;
      window->knob.rotate(KNOB_SPEED);
    }
    else if (keysym.scancode == SDL_SCANCODE_UP)
    {
      encoderValue += upDownToKnobFactor * ENCODER_SPEED;
      window->knob.rotate(-KNOB_SPEED);
    }
    else if (keysym.scancode == SDL_SCANCODE_DOWN)
    {
      encoderValue -= upDownToKnobFactor * ENCODER_SPEED;
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
            encoderValue += mouseWheelToKnobFactor * e.wheel.y * KNOB_SPEED;
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
    return encoderValue + INT32_MAX / 2;
  }

  bool Emulator::isRearCardPresent()
  {
    return rearCardPresent;
  }

  bool Emulator::isFrontCardPresent()
  {
    return frontCardPresent;
  }

  // Grrr. The realpath function doesn't handle the tilde.
  static char *realpathEx(const char *path, char *buff)
  {
    char *home;
    if (*path == '~' && (home = getenv("HOME")))
    {
      char s[PATH_MAX];
      return realpath(strcat(strcpy(s, home), path + 1), buff);
    }
    else
    {
      return realpath(path, buff);
    }
  }

  bool Emulator::writeDefaultConfiguration(const std::string &filename)
  {
    std::ofstream f;
    f.open(filename);
    if (!f.is_open())
      return false;

    f << "## ER-301 Emulator Configuration\n";
    f << '\n';
    f << "## Uncomment lines below to set your own values.\n";
    f << '\n';
    f << "## Root for the Lua interpreter\n";
    f << "# XROOT ./xroot\n";
    f << '\n';
    f << "## Use this root for the rear SD card.\n";
    f << "# REAR_ROOT ~/.od/rear\n";
    f << "# REAR_PRESENT true\n";
    f << '\n';
    f << "## Use this root for the front SD card.\n";
    f << "# FRONT_ROOT ~/.od/front\n";
    f << "# FRONT_PRESENT true\n";
    f << '\n';
    f << "## Key mapping\n";
    f << '\n';
    f << "# BUTTON_MAIN1_KEY " << gpioKeyMap[BUTTON_MAIN1] << '\n';
    f << "# BUTTON_MAIN2_KEY " << gpioKeyMap[BUTTON_MAIN2] << '\n';
    f << "# BUTTON_MAIN3_KEY " << gpioKeyMap[BUTTON_MAIN3] << '\n';
    f << "# BUTTON_MAIN4_KEY " << gpioKeyMap[BUTTON_MAIN4] << '\n';
    f << "# BUTTON_MAIN5_KEY " << gpioKeyMap[BUTTON_MAIN5] << '\n';
    f << "# BUTTON_MAIN6_KEY " << gpioKeyMap[BUTTON_MAIN6] << '\n';
    f << "# BUTTON_DIAL1_KEY " << gpioKeyMap[BUTTON_DIAL1] << '\n';
    f << "# BUTTON_DIAL2_KEY " << gpioKeyMap[BUTTON_DIAL2] << '\n';
    f << "# BUTTON_DIAL3_KEY " << gpioKeyMap[BUTTON_DIAL3] << '\n';
    f << "# BUTTON_SUB1_KEY " << gpioKeyMap[BUTTON_SUB1] << '\n';
    f << "# BUTTON_SUB2_KEY " << gpioKeyMap[BUTTON_SUB2] << '\n';
    f << "# BUTTON_SUB3_KEY " << gpioKeyMap[BUTTON_SUB3] << '\n';
    f << "# BUTTON_ENTER_KEY " << gpioKeyMap[BUTTON_ENTER] << '\n';
    f << "# BUTTON_UP_KEY " << gpioKeyMap[BUTTON_UP] << '\n';
    f << "# BUTTON_SHIFT_KEY " << gpioKeyMap[BUTTON_SHIFT] << '\n';
    f << "# BUTTON_SELECT1_KEY " << gpioKeyMap[BUTTON_SELECT1] << '\n';
    f << "# BUTTON_SELECT2_KEY " << gpioKeyMap[BUTTON_SELECT2] << '\n';
    f << "# BUTTON_SELECT3_KEY " << gpioKeyMap[BUTTON_SELECT3] << '\n';
    f << "# BUTTON_SELECT4_KEY " << gpioKeyMap[BUTTON_SELECT4] << '\n';
    f << "# STORAGE_FOCUS_KEY " << storageToggleFocusKey << '\n';
    f << "# MODE_FOCUS_KEY " << modeToggleFocusKey << '\n';
    f << "# ZOOM_IN_KEY " << zoomInKey << '\n';
    f << "# ZOOM_OUT_KEY " << zoomOutKey << '\n';
    f << "# QUIT_KEY " << quitKey << '\n';
    f << '\n';
    f << "## Knob mapping\n";
    f << '\n';
    f << "##  Scale factor for mouse wheel. Negate to invert.\n";
    f << "# MOUSE_WHEEL_FACTOR " << mouseWheelToKnobFactor << "\n";
    f << '\n';
    f << "##  Scale factor for LEFT/RIGHT arrow keys. Negate to invert.\n";
    f << "# LEFT_RIGHT_ARROWS_FACTOR " << leftRightToKnobFactor << "\n";
    f << '\n';
    f << "##  Scale factor for UP/DOWN arrow keys. Negate to invert.\n";
    f << "# UP_DOWN_ARROWS_FACTOR " << upDownToKnobFactor << '\n';
    f << '\n';

    f.close();
    return true;
  }

  void Emulator::loadDefaultConfiguration()
  {
    char tmp[PATH_MAX];

    // Set default paths
    realpathEx("~/.od", tmp);
    configRoot = tmp;
    createDirectory(tmp);
    rearRoot = configRoot + "/rear";
    rearCardPresent = true;
    frontRoot = configRoot + "/front";
    frontCardPresent = true;
    sessionFilename = configRoot + "/emu.session";
    configFilename = configRoot + "/emu.config";
    realpathEx("./xroot", tmp);
    xRoot = tmp;

    // Set default key map
    mapButtonToKey(BUTTON_MAIN1, "Q");
    mapButtonToKey(BUTTON_MAIN2, "W");
    mapButtonToKey(BUTTON_MAIN3, "E");
    mapButtonToKey(BUTTON_MAIN4, "R");
    mapButtonToKey(BUTTON_MAIN5, "T");
    mapButtonToKey(BUTTON_MAIN6, "Y");
    mapButtonToKey(BUTTON_DIAL1, "A");
    mapButtonToKey(BUTTON_DIAL2, "S");
    mapButtonToKey(BUTTON_DIAL3, "D");
    mapButtonToKey(BUTTON_SUB1, "F");
    mapButtonToKey(BUTTON_SUB2, "G");
    mapButtonToKey(BUTTON_SUB3, "H");
    mapButtonToKey(BUTTON_ENTER, "V");
    mapButtonToKey(BUTTON_UP, "B");
    mapButtonToKey(BUTTON_SHIFT, "N");
    mapButtonToKey(BUTTON_SELECT1, "1");
    mapButtonToKey(BUTTON_SELECT2, "2");
    mapButtonToKey(BUTTON_SELECT3, "3");
    mapButtonToKey(BUTTON_SELECT4, "4");
    storageToggleFocusKey = "Z";
    modeToggleFocusKey = "X";
    zoomInKey = "=";
    zoomOutKey = "-";
    quitKey = "Q";

    // Set default knob mapping
    mouseWheelToKnobFactor = 0.5;
    leftRightToKnobFactor = 1;
    upDownToKnobFactor = 0.25;
  }

  bool Emulator::loadConfiguration(const std::string &filename)
  {
    KeyValueStore db;
    if (db.load(filename))
    {
      // Override default key map
      mapButtonToKey(BUTTON_MAIN1, db.get("BUTTON_MAIN1_KEY", gpioKeyMap[BUTTON_MAIN1]));
      mapButtonToKey(BUTTON_MAIN2, db.get("BUTTON_MAIN2_KEY", gpioKeyMap[BUTTON_MAIN2]));
      mapButtonToKey(BUTTON_MAIN3, db.get("BUTTON_MAIN3_KEY", gpioKeyMap[BUTTON_MAIN3]));
      mapButtonToKey(BUTTON_MAIN4, db.get("BUTTON_MAIN4_KEY", gpioKeyMap[BUTTON_MAIN4]));
      mapButtonToKey(BUTTON_MAIN5, db.get("BUTTON_MAIN5_KEY", gpioKeyMap[BUTTON_MAIN5]));
      mapButtonToKey(BUTTON_MAIN6, db.get("BUTTON_MAIN6_KEY", gpioKeyMap[BUTTON_MAIN6]));
      mapButtonToKey(BUTTON_DIAL1, db.get("BUTTON_DIAL1_KEY", gpioKeyMap[BUTTON_DIAL1]));
      mapButtonToKey(BUTTON_DIAL2, db.get("BUTTON_DIAL2_KEY", gpioKeyMap[BUTTON_DIAL2]));
      mapButtonToKey(BUTTON_DIAL3, db.get("BUTTON_DIAL3_KEY", gpioKeyMap[BUTTON_DIAL3]));
      mapButtonToKey(BUTTON_SUB1, db.get("BUTTON_SUB1_KEY", gpioKeyMap[BUTTON_SUB1]));
      mapButtonToKey(BUTTON_SUB2, db.get("BUTTON_SUB2_KEY", gpioKeyMap[BUTTON_SUB2]));
      mapButtonToKey(BUTTON_SUB3, db.get("BUTTON_SUB3_KEY", gpioKeyMap[BUTTON_SUB3]));
      mapButtonToKey(BUTTON_ENTER, db.get("BUTTON_ENTER_KEY", gpioKeyMap[BUTTON_ENTER]));
      mapButtonToKey(BUTTON_UP, db.get("BUTTON_UP_KEY", gpioKeyMap[BUTTON_UP]));
      mapButtonToKey(BUTTON_SHIFT, db.get("BUTTON_SHIFT_KEY", gpioKeyMap[BUTTON_SHIFT]));
      mapButtonToKey(BUTTON_SELECT1, db.get("BUTTON_SELECT1_KEY", gpioKeyMap[BUTTON_SELECT1]));
      mapButtonToKey(BUTTON_SELECT2, db.get("BUTTON_SELECT2_KEY", gpioKeyMap[BUTTON_SELECT2]));
      mapButtonToKey(BUTTON_SELECT3, db.get("BUTTON_SELECT3_KEY", gpioKeyMap[BUTTON_SELECT3]));
      mapButtonToKey(BUTTON_SELECT4, db.get("BUTTON_SELECT4_KEY", gpioKeyMap[BUTTON_SELECT4]));
      storageToggleFocusKey = db.get("STORAGE_FOCUS_KEY", storageToggleFocusKey);
      modeToggleFocusKey = db.get("MODE_FOCUS_KEY", modeToggleFocusKey);
      zoomInKey = db.get("ZOOM_IN_KEY", zoomInKey);
      zoomOutKey = db.get("ZOOM_OUT_KEY", zoomOutKey);
      quitKey = db.get("QUIT_KEY", quitKey);

      // Override knob settings
      mouseWheelToKnobFactor = db.getFloat("MOUSE_WHEEL_FACTOR", mouseWheelToKnobFactor);
      leftRightToKnobFactor = db.getFloat("LEFT_RIGHT_ARROWS_FACTOR", leftRightToKnobFactor);
      upDownToKnobFactor = db.getFloat("UP_DOWN_ARROWS_FACTOR", upDownToKnobFactor);

      // Override default paths
      char tmp[PATH_MAX];

      realpath(db.get("XROOT", xRoot).c_str(), tmp);
      xRoot = tmp;

      realpath(db.get("REAR_ROOT", rearRoot).c_str(), tmp);
      rearRoot = tmp;
      if (db.get("REAR_PRESENT", "true") == "false")
      {
        rearCardPresent = false;
      }

      realpath(db.get("FRONT_ROOT", frontRoot).c_str(), tmp);
      frontRoot = tmp;
      if (db.get("FRONT_PRESENT", "true") == "false")
      {
        frontCardPresent = false;
      }

      return true;
    }
    else
    {
      return false;
    }
  }

  void Emulator::restoreState()
  {
    // Restore emulator state.
    if (pathExists(sessionFilename.c_str()))
    {
      KeyValueStore db;
      if (db.load(sessionFilename))
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

        logInfo("Restored emulator state from %s.", sessionFilename.c_str());
      }
      else
      {
        logWarn("Failed to load from %s.", sessionFilename.c_str());
      }
    }
  }

  void Emulator::saveState()
  {
    KeyValueStore db;

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

    if (db.save(sessionFilename))
    {
      logInfo("Saved emulator state to %s.", sessionFilename.c_str());
    }
    else
    {
      logWarn("Failed to save to %s.", sessionFilename.c_str());
    }
  }

  void Emulator::mapButtonToKey(uint32_t id, const std::string &key)
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
    interp.execute("package.path = '%s/?.lua;%s/?/init.lua'", globalConfig.xRoot, globalConfig.xRoot);
    interp.execute("app.EMULATION = true");
    interp.execute("app.roots = {x='%s',rear='%s',front='%s'}",
                   globalConfig.xRoot, globalConfig.rearRoot, globalConfig.frontRoot);
    interp.execute("dofile('%s/boot/logging.lua')", globalConfig.xRoot);
    interp.execute("dofile('%s/boot/start.lua')", globalConfig.xRoot);
    return 0;
  }

  int Emulator::run(int argc, char **argv)
  {
    CommandLine cmdLine(argc, argv);
    if (cmdLine.optionExists("-h") || cmdLine.optionExists("--help"))
    {
      printf("ER-301 Emulator (v" FIRMWARE_VERSION ")\n");
      printf("Usage: emu.elf [OPTIONS]\n\n");
      printf("Examples:\n");
      printf("  emu.elf              # Start emulator with default configuration file.\n");
      printf("  emu.elf -c foo.cfg   # Start emulator with 'foo.cfg'.\n");
      printf("\n");
      printf("  -h, --help         Show this help.\n");
      printf("  -c, --config FILE  Use the given configuration file.\n");
      printf("                     (Default: ~/.od/emu.config)\n");
      return 0;
    }

    TLS_setName("main");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0)
    {
      logFatal("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    }

    customEventType = SDL_RegisterEvents(1);
    window = new Window();

    loadDefaultConfiguration();
    // Optionally override configuration file with cmdline.
    if (cmdLine.optionExists("-c"))
    {
      configFilename = cmdLine.getOption("-c");
    }
    if (cmdLine.optionExists("--config"))
    {
      configFilename = cmdLine.getOption("--config");
    }

    if (!pathExists(configFilename.c_str()))
    {
      logWarn("%s does not exist, creating a default one.", configFilename.c_str());
      writeDefaultConfiguration(configFilename);
    }
    else if (!loadConfiguration(configFilename))
    {
      logWarn("There was a problem loading %s.  Using default emulator configuration.", configFilename.c_str());
    }

    Heap_init();
    Timing_init();
    Uart_init();
    Card_init();
    Uart_enable();
    Log_init();

    std::string firmwareCfg = rearRoot;
    firmwareCfg += "/firmware.cfg";
    Config_init(firmwareCfg.c_str(), xRoot.c_str(), rearRoot.c_str(), frontRoot.c_str());

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
