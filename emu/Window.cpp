#include <emu/Window.h>
#include <emu/draw.h>
#include <hal/log.h>
#include <hal/timing.h>

namespace emu
{

  Window::Window()
  {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

    window = SDL_CreateWindow("ER-301 Emulator",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
      logFatal("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
      logFatal("Renderer could not be created! SDL Error: %s", SDL_GetError());
    }

    mainTexture = SDL_CreateTexture(renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    MAIN_HORIZONTAL_PIXELS, MAIN_VERTICAL_PIXELS);
    if (mainTexture == NULL)
    {
      logFatal("Failed to create main texture: %s", SDL_GetError());
    }

    subTexture = SDL_CreateTexture(renderer,
                                   SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   SUB_HORIZONTAL_PIXELS, SUB_VERTICAL_PIXELS);
    if (subTexture == NULL)
    {
      logFatal("Failed to create sub texture: %s", SDL_GetError());
    }

    pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
  }

  Window::~Window()
  {
    SDL_FreeFormat(pixelFormat);
    SDL_DestroyTexture(mainTexture);
    SDL_DestroyTexture(subTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
  }

  FC_Font *Window::getFont(int size)
  {
    static const char *fontFiles[] = {
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "libs/SDL_FontCache/test/fonts/FreeSans.ttf",
        NULL};
    auto i = fontMap.find(size);
    if (i == fontMap.end())
    {
      FC_Font *font = FC_CreateFont();
      for (const char **filename = fontFiles; *filename; filename++)
      {
        logDebug(10, "Trying %s, size=%d", *filename, size);
        if (FC_LoadFont(font, renderer, *filename, size, FC_MakeColor(0, 0, 0, 255), TTF_STYLE_NORMAL))
        {
          logDebug(10, "Loaded %s, size=%d", *filename, size);
          fontMap[size] = font;
          return font;
        }
      }
      // Prevent further attempts;
      logError("Could not load a font for size=%d.", size);
      fontMap[size] = NULL;
      return NULL;
    }
    else
    {
      return (*i).second;
    }
  }

#if 1
  void Window::drawText(int x, int y, int size, const char *fmt, ...)
  {
    FC_Font *font = getFont(size);
    if (font == NULL)
    {
      return;
    }

    char text[512];
    va_list lst;
    va_start(lst, fmt);
    vsnprintf(text, sizeof(text), fmt, lst);
    va_end(lst);

    FC_Draw(font, renderer, x, y, text);
  }
#else
  void Window::drawText(int x, int y, int size, const char *fmt, ...)
  {
    FC_Scale scale{.x = 1.0f / OVERSAMPLE, .y = 1.0f / OVERSAMPLE};
    FC_Font *font = getFont(size * OVERSAMPLE);
    if (font == NULL)
    {
      return;
    }

    char text[512];
    va_list lst;
    va_start(lst, fmt);
    vsnprintf(text, sizeof(text), fmt, lst);
    va_end(lst);

    FC_DrawScale(font, renderer, x, y, scale, text);
  }
#endif

  void Window::drawTextAligned(FC_AlignEnum align, int x, int y, int size, const char *fmt, ...)
  {
    FC_Font *font = getFont(size);
    if (font == NULL)
    {
      return;
    }

    char text[512];
    va_list lst;
    va_start(lst, fmt);
    vsnprintf(text, sizeof(text), fmt, lst);
    va_end(lst);

    FC_DrawAlign(font, renderer, x, y, align, text);
  }

  void Window::drawCircle(int x, int y, int r)
  {
    emu::drawCircle(renderer, x, y, r);
  }

  void Window::renderMainFrame(uint8_t *frame)
  {
    uint16_t *src = (uint16_t *)frame;
    uint8_t *dst;
    int pitch;
    SDL_LockTexture(mainTexture, NULL, (void **)&dst, &pitch);
    for (int y = 0; y < MAIN_VERTICAL_PIXELS; y++)
    {
      uint *row = (uint *)(dst + pitch * y);
      int yy = MAIN_VERTICAL_PIXELS - y - 1;
      for (int x = 0; x < MAIN_HORIZONTAL_PIXELS; x++)
      {
        int xx = MAIN_HORIZONTAL_PIXELS - x - 1;
        uint16_t cell = *(src + (yy << 7) + (xx >> 1));
        int shift = (((~xx) & 0b1) << 2);
        int value = (cell >> shift) & 0xF;
        value *= SCREEN_BRIGHTNESS;
        row[x] = SDL_MapRGBA(pixelFormat, value, value * SCREEN_TINT, 0, 255);
      }
    }
    SDL_UnlockTexture(mainTexture);
  }

  void Window::renderSubFrame(uint8_t *frame)
  {
    uint16_t *src = (uint16_t *)frame;
    uint8_t *dst;
    int pitch;
    SDL_LockTexture(subTexture, NULL, (void **)&dst, &pitch);
    for (int y = 0; y < SUB_VERTICAL_PIXELS; y++)
    {
      uint *row = (uint *)(dst + pitch * y);
      int yy = SUB_VERTICAL_PIXELS - y - 1;
      int shift = yy & 0b111;
      for (int x = 0; x < SUB_HORIZONTAL_PIXELS; x++)
      {
        int xx = SUB_HORIZONTAL_PIXELS - x - 1;
        uint16_t cell = *(src + ((yy >> 3) << 7) + xx);
        int value = (cell >> shift) & 0b1;
        value *= 0xF * SCREEN_BRIGHTNESS;
        row[x] = SDL_MapRGBA(pixelFormat, value, value * SCREEN_TINT, 0, 255);
      }
    }
    SDL_UnlockTexture(subTexture);
  }

  void Window::onResized(int w, int h)
  {
    width = w;
    height = h;
    pauseDisplayTime = 1;
  }

  void Window::update(uint8_t *mainFrame, uint8_t *subFrame)
  {
    static tick_t timestamp = ticks();

    SDL_SetRenderDrawColor(renderer, P_GRAY, P_GRAY, P_GRAY, 255);
    SDL_RenderClear(renderer);

    tick_t now = ticks();
    double t = ticks2secsD(now - timestamp);

    if (t < pauseDisplayTime)
    {
      SDL_RenderPresent(renderer);
      return;
    }
    pauseDisplayTime = 0;

    knob.draw(this);

    for (Button &o : buttons)
    {
      o.draw(this);
    }

    for (Jack &o : jacks)
    {
      o.draw(this);
    }

    for (Toggle &o : toggles)
    {
      o.draw(this);
    }

    for (Led &o : leds)
    {
      o.draw(this);
    }

    renderMainFrame(mainFrame);
    SDL_RenderCopy(renderer, mainTexture, NULL, &mainRect);
    renderSubFrame(subFrame);
    SDL_RenderCopy(renderer, subTexture, NULL, &subRect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &mainOutlineRect);
    SDL_RenderDrawRect(renderer, &subOutlineRect);
    SDL_RenderDrawRect(renderer, &panelOutlineRect);
    SDL_RenderDrawRect(renderer, &storageOutlineRect);
    SDL_RenderDrawRect(renderer, &abcdOutlineRect);

#ifdef DISPLAY_FRAMERATE
    static int frameCount = 0;
    static double frameRate = 0;
    if (t > 0.5)
    {
      frameRate = frameCount / t;
      frameCount = 0;
      timestamp = now;
    }
    else
    {
      frameCount++;
    }
    int statusX = scale * MARGIN;
    int statusY = scale * (height - STATUS_H);
    drawText(statusX, statusY, 12, "Frame Rate: %.1fHz  Scale Factor: %.1f", frameRate, scale);
#endif

    SDL_RenderPresent(renderer);
  }

  void Window::setScale(float _scale)
  {
    scale = _scale;
    if (scale < 0.1f)
    {
      scale = 0.1f;
    }
    SDL_RenderSetScale(renderer, scale, scale);
    SDL_SetWindowSize(window, scale * SCREEN_WIDTH, scale * SCREEN_HEIGHT);
  }

  void Window::setPosition(int x, int y, int correction)
  {
    // If the requested position is out of bounds then position in the center.
    SDL_Rect rect;
    SDL_GetDisplayBounds(0, &rect);
    if (x > rect.w)
    {
      x = rect.w / 2;
    }
    if (y > rect.h)
    {
      y = rect.h / 2;
    }
#define HACK_WINDOW_POSITION
#ifdef HACK_WINDOW_POSITION
    y -= correction;
    SDL_SetWindowPosition(window, x, y);
    SDL_GetWindowPosition(window, &x, &y);
    y -= correction;
    SDL_SetWindowPosition(window, x, y);
#else
    SDL_SetWindowPosition(window, x, y);
#endif
  }

  void Window::getPosition(int &x, int &y)
  {
    SDL_GetWindowPosition(window, &x, &y);
  }

  int Window::getTitleBarHeight()
  {
    int top;
    SDL_GetWindowBordersSize(window, &top, 0, 0, 0);
    return top;
  }

} // namespace emu