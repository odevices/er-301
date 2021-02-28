#include <od/objects/file/FileSinkThread.h>
#include <algorithm>
#include <hal/fileops.h>
#include <od/config.h>
#include <hal/ops.h>
#include <stdio.h>

#define NTRACKS 6
#define BUFFERS_PER_CHANNEL 10
// # of frames for 48kHz
#define NFRAMES 150

namespace od
{

  FileSinkThread::FileSinkThread(const char * name) : Thread(name, TASK_PRIORITY_RECORDER)
  {
  }

  FileSinkThread::~FileSinkThread()
  {
    clear();
  }

  void FileSinkThread::freeCache()
  {
    mPool.deallocate();
    mOverflowCount = 0;
    mStatusDirty = true;
  }

  bool FileSinkThread::allocateCache()
  {
    // count the number of assigned channels
    int totalChannelCount = 0;
    int maxChannelCount = 0;
    for (FileSink *sink : mFileSinks)
    {
      totalChannelCount += sink->mChannelCount;
      if (sink->mChannelCount > maxChannelCount)
      {
        maxChannelCount = sink->mChannelCount;
      }
    }

    // assume float encoding to calculate byte rate
    float totalByteRate = totalChannelCount * globalConfig.sampleRate * 4;
    float maxByteRate = maxChannelCount * globalConfig.sampleRate * 4;
    float freeSpaceMB = diskFreeSpaceMB(globalConfig.frontRoot);
    float diskRemaining = 1024.0f * 1024.0f * (freeSpaceMB / totalByteRate);
    float fat32Limit = 4 * 1024 * 1024 * (1024.0f / maxByteRate);
    mRemainingSeconds = MIN(fat32Limit, diskRemaining);

    // adjust number of frames for sample rate (tested with 96kHz)
    int nframes = NFRAMES * ((2 * globalConfig.sampleRate) / 96000);

    return mPool.allocate(nframes * globalConfig.frameLength,
                          BUFFERS_PER_CHANNEL * totalChannelCount);
  }

  void FileSinkThread::clear()
  {
    mFileSinksMutex.enter();
    for (FileSink *sink : mFileSinks)
    {
      sink->setThread(0);
      sink->release();
    }
    mFileSinks.clear();
    mFileSinksMutex.leave();
    mOverflowCount = 0;
    mStatusDirty = true;
  }

  void FileSinkThread::add(FileSink *sink)
  {
    mFileSinksMutex.enter();
    sink->attach();
    sink->setThread(this);
    mFileSinks.push_back(sink);
    mStatusDirty = true;
    mFileSinksMutex.leave();
  }

  void FileSinkThread::remove(FileSink *sink)
  {
    mFileSinksMutex.enter();
    auto i = std::find(mFileSinks.begin(), mFileSinks.end(), sink);
    if (i != mFileSinks.end())
    {
      mFileSinks.erase(i);
      sink->setThread(0);
      sink->release();
      mStatusDirty = true;
    }
    mFileSinksMutex.leave();
  }

  void FileSinkThread::notifyBufferReady()
  {
    mEvents.post(onBufferReady);
  }

  float *FileSinkThread::requestBuffer()
  {
    mPoolMutex.enter();
    float *buffer = mPool.get();
    if (buffer == 0)
    {
      mOverflowCount++;
      mStatusDirty = true;
    }
    mPoolMutex.leave();
    return buffer;
  }

  void FileSinkThread::releaseBuffer(float *buffer)
  {
    mPoolMutex.enter();
    mPool.release(buffer);
    mPoolMutex.leave();
  }

  void FileSinkThread::wait()
  {
  }

  float FileSinkThread::duration()
  {
    if (mFileSinks.size() == 0)
    {
      return 0.0f;
    }
    else
    {
      FileSink *sink = *mFileSinks.begin();
      return sink->mTotalSeconds;
    }
  }

  float FileSinkThread::remaining()
  {
    return mRemainingSeconds - duration();
  }

  const std::string &FileSinkThread::status()
  {
    if (mStatusDirty)
    {
      char tmp[64];
      mStatusDirty = false;
      if (mOverflowCount > 0)
      {
        if (mOverflowCount < 100)
        {
          snprintf(tmp, sizeof(tmp), "%d overflows!", mOverflowCount);
        }
        else
        {
          snprintf(tmp, sizeof(tmp), "!!This card is too slow!!");
        }
      }
      else if (mFileSinks.size() == 1)
      {
        snprintf(tmp, sizeof(tmp), "Recording 1 track...");
      }
      else
      {
        snprintf(tmp, sizeof(tmp), "Recording %d tracks...", (int)mFileSinks.size());
      }
      mStatusText = tmp;
    }
    return mStatusText;
  }

  int FileSinkThread::getPendingBufferCount()
  {
    int n = 0;
    for (FileSink *sink : mFileSinks)
    {
      n += sink->getPendingBufferCount();
    }
    return n;
  }

  void FileSinkThread::run()
  {
    while (1)
    {
      if (mEvents.waitForAny(onThreadQuit | onBufferReady) &
          onThreadQuit)
      {
        break;
      }

      mFileSinksMutex.enter();
      for (FileSink *sink : mFileSinks)
      {
        sink->writeBuffersToFile();
      }
      mFileSinksMutex.leave();
    }
  }

} /* namespace od */
