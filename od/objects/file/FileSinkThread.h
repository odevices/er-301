#pragma once

#include <hal/concurrency/Thread.h>
#include <od/extras/ReferenceCounted.h>
#include <hal/concurrency/Mutex.h>
#include <od/extras/BufferPool.h>
#include <od/objects/file/FileSink.h>

namespace od
{

	class FileSink;
	class RecordingGraphic;

	class FileSinkThread : public ReferenceCounted, public Thread
	{
	public:
		FileSinkThread(const char * name);
		virtual ~FileSinkThread();

		void add(FileSink *sink);
		void remove(FileSink *sink);
		void clear();
		void wait();

		void freeCache();
		bool allocateCache();

		void resetOverflowCount()
		{
			mOverflowCount = 0;
		}

		int getPendingBufferCount();
		int getOverflowCount()
		{
			return mOverflowCount;
		}

	private:
		WatermarkedBufferPool<float> mPool;
    Mutex mPoolMutex;
		std::vector<FileSink *> mFileSinks;
		Mutex mFileSinksMutex;

		int mOverflowCount = 0;
		std::string mStatusText;
		bool mStatusDirty = true;
		float mRemainingSeconds = 0.0f;

		const uint32_t onBufferReady = EventFlags::flag01;
		virtual void run();

		friend FileSink;
		void notifyBufferReady();
		float *requestBuffer();
		void releaseBuffer(float *buffer);

		inline int getBufferSize()
		{
			return (int)mPool.mBufferSize;
		}

		friend RecordingGraphic;
		float percentUsed()
		{
			return mPool.percentUsed();
		}

		float percentMaximumUsed()
		{
			return mPool.percentMaximumUsed();
		}
    
		float duration();
		float remaining();
		const std::string &status();
	};

} /* namespace od */
