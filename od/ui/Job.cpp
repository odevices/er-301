#include <od/ui/Job.h>

namespace od
{

  Job::Job()
  {
    // enableTracking();
  }

  Job::~Job()
  {
    mCancelRequested = true;
    wait();
  }

  void Job::wait()
  {
    if (mPending)
    {
      mEvents.waitForAny(onFinished);
    }
  }

  void Job::reset()
  {
    mEvents.clear();
    mPending = true;
    mFinished = false;
    mCancelRequested = false;
  }

  bool Job::pending()
  {
    return mPending;
  }

  bool Job::finished()
  {
    return mFinished;
  }

  void Job::notifyFinished()
  {
    mEvents.post(onFinished);
  }

} /* namespace od */