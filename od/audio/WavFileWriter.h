#pragma once

#include <od/audio/SoundFileWriter.h>
#include <od/audio/wav.h>

namespace od
{
	enum WavFileEncoding
	{
		wav16bit,
		wav24bit,
		wavFloat
	};

	class WavFileWriter : public SoundFileWriter
	{
	public:
		WavFileWriter();
		WavFileWriter(float sampleRate, uint32_t channelCount,
									WavFileEncoding encoding);
		virtual ~WavFileWriter();

		void init(float sampleRate, uint32_t channelCount,
							WavFileEncoding encoding);

		// len is in samples (channel-invariant)
		virtual uint32_t writeSamples(float *buffer, uint32_t len);
		virtual uint32_t seekSamples(uint32_t offset);
		virtual uint32_t tellSamples();

	protected:
		WavFileEncoding mWavFileEncoding = wav24bit;
		uint32_t mCurrentSamplePosition = 0;

		WavFormatData mFormat = {};

		virtual bool prepare();
		virtual bool finalize();
		void populateWavFormatData();

		bool writeHeader();
		uint32_t write16bit(float *buffer, uint32_t len);
		uint32_t write24bit(float *buffer, uint32_t len);
	};

} /* namespace od */
