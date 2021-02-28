#pragma once

#include <od/objects/Object.h>
#include <od/extras/LockFreeQueue.h>
#include <od/audio/WavFileWriter.h>

namespace od
{

	class FileSinkThread;
	class FileSink : public Object
	{
	public:
		FileSink(int channelCount);
		virtual ~FileSink();

		void setFilename(const std::string &filename);
		void setEncoding(WavFileEncoding encoding);

		void start();
		void stop();
		bool open();
		void close();
		void abort();

		bool error()
		{
			return mError;
		}

#ifndef SWIGLUA
		void writeBuffersToFile();
		void setThread(FileSinkThread *thread);
		int getPendingBufferCount();
#endif

	protected:
		int mChannelCount;
		bool mStarted = false;
		bool mError = false;
		bool mAbort = false;

		FileSinkThread *mpThread = 0;
		WavFileWriter mWriter;
		std::string mFilename;
		WavFileEncoding mEncoding = wavFloat;

		LockFreeQueue<float *, 128> mFilledBuffers;
		float *mWriteBuffer = 0;
		int mWritePosition = 0;
		float mTotalSeconds = 0;

		float *getWriteBuffer();
		void putWriteBuffer();

		friend FileSinkThread;
	};

} /* namespace od */
