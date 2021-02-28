#include <od/ui/JobQueue.h>
#include <hal/log.h>
//#include <algorithm>

namespace od
{

  JobQueue::JobQueue(const char * name) : Thread(name)
  {
  }

  JobQueue::JobQueue(const char * name, int priority) : Thread(name, priority)
  {
  }

  JobQueue::~JobQueue()
  {
  }

  void JobQueue::run()
  {

    while (1)
    {
      if (mEvents.waitForAny(onThreadQuit | onPush) &
          onThreadQuit)
      {
        break;
      }

      logDebug(1, "job pushed on Q");

      while (mQueue.pop(&mCurrentJob))
      {
        if (mCurrentJob)
        {
          logDebug(1, "starting job");
          if (!mCurrentJob->mCancelRequested)
          {
            mCurrentJob->work();
            mCurrentJob->mFinished = true;
          }
          mCurrentJob->mPending = false;
          mCurrentJob->notifyFinished();
          mCurrentJob = 0;
          logDebug(1, "finished job");
        }
      }
    }

    // Thread is quiting, mark any remaining jobs as finished.
    while (mQueue.pop(&mCurrentJob))
    {
      if (mCurrentJob)
      {
        mCurrentJob->mPending = false;
        mCurrentJob->notifyFinished();
      }
    }

    mCurrentJob = 0;
  }

  void JobQueue::push(Job *work)
  {
    work->reset();
    mQueue.push(work);
    mEvents.post(onPush);
  }

  void JobQueue::cancel(Job *work)
  {
    if (work)
    {
      work->mCancelRequested = true;
    }
  }

  bool JobQueue::isIdle()
  {
    return mQueue.empty() && mCurrentJob == 0;
  }

} /* namespace od */
