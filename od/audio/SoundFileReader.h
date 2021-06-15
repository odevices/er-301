#pragma once

#include <od/extras/FileReader.h>

namespace od
{

	class SoundFileReader : public FileReader
	{
	public:
		SoundFileReader();
		virtual ~SoundFileReader();

		virtual uint32_t readSamples(float *buffer, uint32_t len) = 0;
		virtual uint32_t seekSamples(uint32_t offset) = 0;
		virtual uint32_t tellSamples() = 0;

		uint32_t getSampleCount()
		{
			return mSampleCount;
		}

		uint32_t getChannelCount()
		{
			return mChannelCount;
		}

		float getSampleRate()
		{
			return mSampleRate;
		}

		float getTotalSeconds()
		{
			return mTotalSeconds;
		}

		uint32_t getBitDepth()
		{
			return mBitDepth;
		}

	protected:
		uint32_t mSampleCount = 0;
		uint32_t mChannelCount = 0;
		uint32_t mBitDepth = 0;
		float mSampleRate = 0.0f;
		float mTotalSeconds = 0.0f;
	};

} /* namespace od */
