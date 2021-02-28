#pragma once

#include <od/graphics/GraphicContext.h>
#include <od/ui/JobQueue.h>

namespace od
{

  class UIThread
  {
  public:
    static void init();

    static void updateDisplay(void);
    static bool saveScreenShotTo(const char *filename);
    static void restartScreenSaverTimer();
    static void setScreenSaverTime(int secs);
    static void setScreenSaver(const char *name);

    static void setMainGraphicContext(GraphicContext *context);
    static void setSubGraphicContext(GraphicContext *context);
    static void setMainOverlay(Graphic *graphic);
    static void setSubOverlay(Graphic *graphic);

    static void startEventTimer();
    static void stopEventTimer();

    static JobQueue *getRealtimeJobQueue();

  private:
    UIThread();
  };

} // namespace od