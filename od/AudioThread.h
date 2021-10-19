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

    // Returns (i.e. allocates) a FRAMELENGTH buffer from the pre-allocated pool of frame buffers.
    static float *getFrame();
    // Releases (i.e. frees) the previously gotten frame buffer.
    static void releaseFrame(float *frame);
    // The previous 2 calls are constant-time non-locking and thus safe to call in the audio thread.

#endif

  private:
    AudioThread();
  };

} /* namespace od */
