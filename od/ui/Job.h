#pragma once

#include <hal/concurrency/EventFlags.h>
#include <od/extras/ReferenceCounted.h>
#include <od/extras/LockFreeQueue.h>
#include <od/constants.h>

namespace od
{

  class JobQueue;
  class Job : public ReferenceCounted
  {
  public:
    Job();
    virtual ~Job();

    void wait();
    void reset();
    bool pending();
    bool finished();

  protected:
    friend JobQueue;
    bool mCancelRequested = false;
    bool mFinished = true;
    bool mPending = false;

    EventFlags mEvents;
    const uint32_t onFinished = EventFlags::flag00;

    virtual void work() = 0;
    void notifyFinished();
  };
} // namespace od