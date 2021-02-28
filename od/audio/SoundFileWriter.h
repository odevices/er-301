#pragma once

#include <od/extras/FileWriter.h>

namespace od
{

	class SoundFileWriter : public FileWriter
	{
	public:
		SoundFileWriter();
		SoundFileWriter(float sampleRate, uint32_t channelCount);
		virtual ~SoundFileWriter();

		// for use with the empty construtor
		void init(float sampleRate, uint32_t channelCount);

		virtual bool open(const std::string &filename);
		virtual bool close();

		virtual uint32_t writeSamples(float *buffer, uint32_t len) = 0;
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

	protected:
		float mSampleRate = 96000.0f;
		uint32_t mChannelCount = 1;
		uint32_t mSampleCount = 0;

		// called after successful open
		virtual bool prepare() = 0;
		// called before closing file
		virtual bool finalize() = 0;
	};

} /* namespace od */
