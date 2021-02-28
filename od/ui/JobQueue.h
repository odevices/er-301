#pragma once

#include <hal/concurrency/Thread.h>
#include <od/extras/ReferenceCounted.h>
#include <od/extras/LockFreeQueue.h>
#include <od/ui/Job.h>
#include <od/constants.h>

namespace od
{

  class JobQueue : public ReferenceCounted, public Thread
  {
  public:
    JobQueue(const char * name);
    JobQueue(const char * name, int priority);
    virtual ~JobQueue();

    bool isIdle();
    void push(Job *work);
    void cancel(Job *work);

  private:
    LockFreeQueue<Job *, 128> mQueue;
    Job *mCurrentJob = 0;
    int mStatus = STATUS_NONE;

    const uint32_t onPush = EventFlags::flag01;
    virtual void run();
  };

} /* namespace od */
