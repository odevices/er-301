#include <hal/log.h>
#include <hal/breakpoint.h>
#include <hal/priorities.h>
#include <hal/timing.h>
#include <hal/uart.h>
#include <hal/usb.h>
#include <hal/ops.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>

#include <hal/concurrency/Thread.h>
#include <hal/concurrency/Mutex.h>
#include <od/extras/LockFreeQueue.h>
#include <od/extras/BufferPool.h>
#include <string>

using namespace od;

class LogThread : public Thread
{
public:
  LogThread() : Thread("log", TASK_PRIORITY_LOG)
  {
    mBufferPool.allocate(256, 256);
  }

  void put(const char *msg, int len)
  {
    mMutex.enter();
    char *buffer = mBufferPool.get();
    mMutex.leave();
    if (buffer)
    {
      int n = MIN((int)mBufferPool.mBufferSize, len);
      memcpy(buffer, msg, n);
      mQueue.push(Message{buffer, n});
      mEvents.post(onPush);
    }
  }

  void put(const char *msg)
  {
    put(msg, strlen(msg) + 1);
  }

  void format(const char *fmt, ...)
  {
    mMutex.enter();
    char *buffer = mBufferPool.get();
    mMutex.leave();
    if (buffer)
    {
      va_list args;
      va_start(args, fmt);
      int n = vsnprintf(buffer, mBufferPool.mBufferSize, fmt, args);
      va_end(args);

      mQueue.push(Message{buffer, n});
      mEvents.post(onPush);
    }
    
  }

  void vformat(const char *fmt, va_list args)
  {
    mMutex.enter();
    char *buffer = mBufferPool.get();
    mMutex.leave();
    if (buffer)
    {
      int n = vsnprintf(buffer, mBufferPool.mBufferSize, fmt, args);
      mQueue.push(Message{buffer, n});
      mEvents.post(onPush);
    }
  }

  void wait()
  {
    while (!mQueue.empty())
    {
      sleep(100);
    }
  }

protected:
  struct Message
  {
    char *buffer;
    int len;
  };

  Mutex mMutex;
  BufferPool<char> mBufferPool;
  LockFreeQueue<Message, 256> mQueue;
  static const uint32_t onPush = EventFlags::flag01;

  virtual void run()
  {
    while ((mEvents.waitForAny(onThreadQuit | onPush) & onThreadQuit) == 0)
    {
      Message msg;
      while (mQueue.pop(&msg))
      {
        USB_write(msg.buffer, msg.len);
        Uart_write(msg.buffer, msg.len);
        mMutex.enter();
        mBufferPool.release(msg.buffer);
        mMutex.leave();
      }
    }
  }
};

static LogThread logThread;

extern "C"
{

#define LOG_FMT_FILE_LINE "\033[36m[%s:%d]\033[0m"
#define LOG_FMT_FUNCTION "\033[34m%s()\033[0m"
#define LOG_FMT_PREFIX "\033[34m[%d %0.4fs %s]\033[0m"

  static int count = 0;

  static const char *thread()
  {
    Task_Handle hTask = Task_self();
    if (hTask)
    {
      Ptr hEnv = Task_getEnv(hTask);
      if (hEnv)
      {
        return (const char *)hEnv;
      }
      else
      {
        return "???";
      }
    }
    return "main";
  }

  static const char *skipRelativePaths(const char *path)
  {
    while (*path > 0 && (*path == '.' || *path == '/'))
    {
      path++;
    }
    return path;
  }

  void Log_init()
  {
    logThread.start();
  }

  int logCount()
  {
    return count++;
  }

  void logWrite(const char *buffer, int len)
  {
    logThread.put(buffer, len);
  }

  void _logSkipped()
  {
    count++;
  }

  void _logRaw(const char *fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    logThread.vformat(fmt, args);
    va_end(args);
  }

  void _logAssert(const char *filename, const char *function, int linenum, int expr)
  {
    if (expr)
    {
      return;
    }
    filename = skipRelativePaths(filename);
    logThread.format(LOG_FMT_PREFIX " \033[31;1mASSERT\033[0m " LOG_FMT_FUNCTION " " LOG_FMT_FILE_LINE "\n",
                     count++, wallclock(), thread(), function, filename, linenum);
    logThread.wait();
    Breakpoint();
  }

  void _logBriefNice(const char *label, const char *fmt, ...)
  {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    logThread.format(LOG_FMT_PREFIX " %s %s\n", count++, wallclock(), thread(), label, buffer);
  }

  void _logCompleteNice(const char *label,
                        const char *filename,
                        const char *function,
                        int linenum,
                        const char *fmt, ...)
  {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    filename = skipRelativePaths(filename);
    logThread.format(LOG_FMT_PREFIX " %s " LOG_FMT_FUNCTION " %s " LOG_FMT_FILE_LINE "\n",
                     count++, wallclock(), thread(), label, function, buffer, filename, linenum);
  }

  void _logRude(const char *label,
                const char *filename,
                const char *function,
                int linenum,
                const char *fmt, ...)
  {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    filename = skipRelativePaths(filename);
    logThread.format(LOG_FMT_PREFIX " %s " LOG_FMT_FUNCTION " %s " LOG_FMT_FILE_LINE "\n",
                     count++, wallclock(), thread(), label, function, buffer, filename, linenum);
    logThread.wait();
    Breakpoint();
  }
}