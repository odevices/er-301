#pragma once

#include <od/tasks/InputTask.h>
#include <od/tasks/OutputTask.h>
#include <od/objects/measurement/FifoProbe.h>

namespace od
{

  class AudioThread
  {
  public:
    static void init();

    static void addTask(Task *task, int priority);
    static void removeTask(Task *task);
    static void beginTransaction();
    static void endTransaction();

    static InputTask *getInputTask();
    static OutputTask *getOutputTask();

    static void connect(Outlet *outlet, Inlet *inlet, Object *object = 0);
    static void disconnect(Inlet *inlet, Object *object = 0);
    static void disconnect(Outlet *outlet, Object *object = 0);

    static void printStatus();

#ifndef SWIGLUA
    static FifoProbe *getFifoProbe();
    static void releaseFifoProbe(FifoProbe *probe);

    static float *getFrame();
    static void releaseFrame(float *frame);

#endif

  private:
    AudioThread();
  };

} /* namespace od */
