#include "UIThread.h"
#include <od/graphics/MainFrameBuffer.h>
#include <od/graphics/SubFrameBuffer.h>
#include <od/graphics/screensavers/Blank.h>
#include <od/graphics/screensavers/Lines.h>
#include <od/graphics/screensavers/Bubbles.h>
#include <od/extras/Profiler.h>
#include <od/ui/ChannelLEDs.h>
#include <od/AudioThread.h>
#include <hal/events.h>
#include <hal/display.h>
#include <hal/encoder.h>
#include <hal/pwm.h>
#include <hal/log.h>
#include <hal/channels.h>
#include <lodepng.h>

namespace od
{

  struct UIThreadLocals
  {
    UIThreadLocals()
    {
      realtimeJobQueue.attach();
    }

    ExecutionTimer displayTimer;
    ExecutionTimer eventTimer;
    JobQueue realtimeJobQueue{"rtjobs", TASK_PRIORITY_REALTIME};
    MainFrameBuffer mainFrameBuffer;
    SubFrameBuffer subFrameBuffer;
    GraphicContext *mainGraphicContext = 0;
    GraphicContext *subGraphicContext = 0;
    Graphic *mainOverlay = 0;
    Graphic *subOverlay = 0;
    ScreenSaver *screenSaver;
    std::vector<unsigned char> screenShot;
    int screenSaverTimer = 0;
    int screenSaverThreshold = GRAPHICS_REFRESH_RATE * 60 * 30;
  };

  static UIThreadLocals *local = 0;

  UIThread::UIThread()
  {
  }

  void UIThread::init()
  {
    local = new UIThreadLocals();
    Profiler::add(&local->displayTimer, "disp", false);
    Profiler::add(&local->eventTimer, "event", true);
    local->screenSaver = new Bubbles();
    local->realtimeJobQueue.start();
  }

  void UIThread::startEventTimer(void)
  {
    local->eventTimer.start();
  }

  void UIThread::stopEventTimer(void)
  {
    local->eventTimer.stop();
  }

  void UIThread::restartScreenSaverTimer()
  {
    local->screenSaverTimer = 0;
  }

  void UIThread::setScreenSaverTime(int secs)
  {
    local->screenSaverThreshold = secs * GRAPHICS_REFRESH_RATE;
  }

  void UIThread::setScreenSaver(const char *name)
  {
    std::string tmp = name;
    delete local->screenSaver;
    if (tmp == "bubbles")
    {
      local->screenSaver = new Bubbles();
    }
    else if (tmp == "2lines")
    {
      local->screenSaver = new Lines(false);
    }
    else if (tmp == "grid")
    {
      local->screenSaver = new Lines(true);
    }
    else
    {
      local->screenSaver = new Blank();
    }
  }

  bool UIThread::saveScreenShotTo(const char *filename)
  {
    // Write hardware graphics buffers to 8-bit grayscale image.

    DisplayBuffer *frame = Display_getLastPutBuffer();
    const int border = 5;
    const int W = border + 256 + border;
    const int H = border + 64 + 2 * border + 64 + border;
    const int mainX0 = border;
    const int mainY0 = border;
    const int subX0 = border + 128;
    const int subY0 = border + 64 + 2 * border;
    int x, y;
    uint16_t *main = (uint16_t *)frame->main;
    uint16_t *sub = (uint16_t *)frame->sub;
    local->screenShot.resize(W * H, 0xEE);

    uint16_t *p = main + 128 * 64 - 1;
    for (y = mainY0; y < mainY0 + 64; y++)
    {
      for (x = mainX0; x < mainX0 + 256; x += 2)
      {
        local->screenShot[y * W + x] = 0xFF - ((0xF & (*p)) << 4);
        local->screenShot[y * W + x + 1] = 0xFF - (0xF0 & (*p));
        p--;
      }
    }

    p = sub + 8 * 128 - 1;
    for (int y = subY0; y < subY0 + 64; y += 8)
    {
      for (int x = subX0; x < subX0 + 128; x++)
      {
        local->screenShot[(y + 7) * W + x] =
            ((*p) & 0b00000001) ? 0x0 : 0xFF;
        local->screenShot[(y + 6) * W + x] =
            ((*p) & 0b00000010) ? 0x0 : 0xFF;
        local->screenShot[(y + 5) * W + x] =
            ((*p) & 0b00000100) ? 0x0 : 0xFF;
        local->screenShot[(y + 4) * W + x] =
            ((*p) & 0b00001000) ? 0x0 : 0xFF;
        local->screenShot[(y + 3) * W + x] =
            ((*p) & 0b00010000) ? 0x0 : 0xFF;
        local->screenShot[(y + 2) * W + x] =
            ((*p) & 0b00100000) ? 0x0 : 0xFF;
        local->screenShot[(y + 1) * W + x] =
            ((*p) & 0b01000000) ? 0x0 : 0xFF;
        local->screenShot[(y + 0) * W + x] =
            ((*p) & 0b10000000) ? 0x0 : 0xFF;
        p--;
      }
    }

    // Convert to PNG and write to file.

    unsigned error;
    std::vector<unsigned char> png;
    lodepng::State state;                                  //optionally customize this one
    state.info_raw.bitdepth = 8;                           // 8-bits per pixel
    state.info_raw.colortype = LodePNGColorType::LCT_GREY; // greyscale

    error = lodepng::encode(png, local->screenShot, W, H, state);
    if (error)
    {
      logError("UIThread::saveScreenShotTo(encode): %s ",
               lodepng_error_text(error));
      return false;
    }
    error = lodepng::save_file(png, filename);
    if (error)
    {
      logError("UIThread::saveScreenShotTo(save): %s ",
               lodepng_error_text(error));
      return false;
    }
    return true;
  }

  void UIThread::updateDisplay(void)
  {
    // draw a frame (if available)
    DisplayBuffer *frame = Display_getBuffer();
    if (frame == 0)
    {
      return;
    }

    local->displayTimer.start();

    Graphic::updateTween();

    local->mainFrameBuffer.setBuffer((uint16_t *)frame->main);
    local->mainFrameBuffer.clear();

    local->subFrameBuffer.setBuffer((uint16_t *)frame->sub);
    local->subFrameBuffer.clear();

    local->screenSaverTimer++;
    if (local->screenSaverTimer == local->screenSaverThreshold)
    {
      local->screenSaver->reset();
    }
    if (local->screenSaverTimer > local->screenSaverThreshold)
    {
      local->screenSaver->draw(local->mainFrameBuffer,
                               local->subFrameBuffer);
    }
    else
    {
      if (local->mainGraphicContext)
      {
        local->mainGraphicContext->draw(local->mainFrameBuffer);
      }
      if (local->mainOverlay)
      {
        local->mainOverlay->draw(local->mainFrameBuffer);
      }

      if (local->subGraphicContext)
      {
        local->subGraphicContext->draw(local->subFrameBuffer);
      }
      if (local->subOverlay)
      {
        local->subOverlay->draw(local->subFrameBuffer);
      }
    }

    // place on queue to have it sent to the LCD
    Display_putBuffer(frame);

    // set pwm outputs
    InputTask *task = AudioThread::getInputTask();
    Pwm_set(0, task->mLastInput[INPUT_A1] / FULLSCALE_IN_VOLTS);
    Pwm_set(1, task->mLastInput[INPUT_B1] / FULLSCALE_IN_VOLTS);
    Pwm_set(2, task->mLastInput[INPUT_C1] / FULLSCALE_IN_VOLTS);
    Pwm_set(3, task->mLastInput[INPUT_D1] / FULLSCALE_IN_VOLTS);
    Pwm_set(4, task->mLastInput[INPUT_A2] / FULLSCALE_IN_VOLTS);
    Pwm_set(5, task->mLastInput[INPUT_B2] / FULLSCALE_IN_VOLTS);
    Pwm_set(6, task->mLastInput[INPUT_C2] / FULLSCALE_IN_VOLTS);
    Pwm_set(7, task->mLastInput[INPUT_D2] / FULLSCALE_IN_VOLTS);
    Pwm_set(8, task->mLastInput[INPUT_A3] / FULLSCALE_IN_VOLTS);
    Pwm_set(9, task->mLastInput[INPUT_B3] / FULLSCALE_IN_VOLTS);
    Pwm_set(10, task->mLastInput[INPUT_C3] / FULLSCALE_IN_VOLTS);
    Pwm_set(11, task->mLastInput[INPUT_D3] / FULLSCALE_IN_VOLTS);

    ChannelLEDs_update();

    local->displayTimer.stop();
  }

  void UIThread::setMainGraphicContext(GraphicContext *context)
  {
    if (local->mainGraphicContext)
    {
      local->mainGraphicContext->notifyHidden();
      local->mainGraphicContext->release();
    }
    local->mainGraphicContext = context;
    if (local->mainGraphicContext)
    {
      local->mainGraphicContext->attach();
    }
  }

  void UIThread::setSubGraphicContext(GraphicContext *context)
  {
    if (local->subGraphicContext)
    {
      local->subGraphicContext->notifyHidden();
      local->subGraphicContext->release();
    }
    local->subGraphicContext = context;
    if (local->subGraphicContext)
    {
      local->subGraphicContext->attach();
    }
  }

  void UIThread::setMainOverlay(Graphic *graphic)
  {
    if (local->mainOverlay)
    {
      local->mainOverlay->notifyHidden();
      local->mainOverlay->release();
    }
    local->mainOverlay = graphic;
    if (local->mainOverlay)
    {
      local->mainOverlay->attach();
    }
  }

  void UIThread::setSubOverlay(Graphic *graphic)
  {
    if (local->subOverlay)
    {
      local->subOverlay->notifyHidden();
      local->subOverlay->release();
    }
    local->subOverlay = graphic;
    if (local->subOverlay)
    {
      local->subOverlay->attach();
    }
  }

  JobQueue *UIThread::getRealtimeJobQueue()
  {
    return &local->realtimeJobQueue;
  }

} // namespace od
