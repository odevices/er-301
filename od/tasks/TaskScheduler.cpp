#include <od/tasks/TaskScheduler.h>
#include <od/tasks/Task.h>
#include <algorithm>

namespace od
{

  TaskScheduler::TaskScheduler()
  {
  }

  TaskScheduler::~TaskScheduler()
  {
    mMutex.enter();
    for (Task *task : mTasks)
    {
#if TASK_TIMING_ENABLED
      Profiler::remove(&task->mExecutionTimer);
#endif
      task->release();
    }
    mMutex.leave();
  }

  void TaskScheduler::process(float *inputs, float *outputs)
  {
    mMutex.enter();
    for (Task *task : mTasks)
    {
#if TASK_TIMING_ENABLED
      task->mExecutionTimer.start();
#endif
      task->process(inputs, outputs);
#if TASK_TIMING_ENABLED
      task->mExecutionTimer.stop();
#endif
    }
    mMutex.leave();
  }

  void TaskScheduler::add(Task *task)
  {
    mMutex.enter();
    if (mTransactionDepth == 0)
    {
      if (!task->mActive)
      {
        task->attach();
#if TASK_TIMING_ENABLED
        Profiler::add(&task->mExecutionTimer, task->mName.c_str(), false);
#endif
        // keep priorities sorted in decreasing order
        auto it = std::upper_bound(
            mTasks.begin(), mTasks.end(), task,
            [](Task *const lhs, Task *const rhs) { return lhs->mPriority > rhs->mPriority; });
        mTasks.insert(it, task);
        task->mActive = true;
      }
    }
    else
    {
      mTransactions.emplace_back(1, task);
    }
    mMutex.leave();
  }

  void TaskScheduler::remove(Task *task)
  {
    mMutex.enter();
    if (mTransactionDepth == 0)
    {
      auto i = std::find(mTasks.begin(), mTasks.end(), task);
      if (i != mTasks.end())
      {
        mTasks.erase(i);
#if TASK_TIMING_ENABLED
        Profiler::remove(&task->mExecutionTimer);
#endif
        task->mActive = false;
        task->release();
      }
    }
    else
    {
      mTransactions.emplace_back(0, task);
    }
    mMutex.leave();
  }

  void TaskScheduler::beginTransaction()
  {
    mTransactionDepth++;
  }

  void TaskScheduler::endTransaction()
  {
    if (mTransactionDepth > 0)
    {
      mTransactionDepth--;
      if (mTransactionDepth == 0)
      {
        for (std::pair<int, Task *> &trans : mTransactions)
        {
          if (trans.first == 0)
          {
            remove(trans.second);
          }
          else
          {
            add(trans.second);
          }
        }
        mTransactions.clear();
      }
    }
  }

} /* namespace od */
