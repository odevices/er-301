#pragma once
#include <od/audio/SoundFileReader.h>
#include <od/audio/wav.h>
#include <vector>

namespace od
{

	class WavFileReader : public SoundFileReader
	{
	public:
		WavFileReader();
		virtual ~WavFileReader();

		virtual bool open(const std::string &filename);

		virtual uint32_t readSamples(float *buffer, uint32_t len);
		virtual uint32_t seekSamples(uint32_t offset);
		virtual uint32_t tellSamples();

		void describe();

	protected:
		uint32_t mCurrentSamplePosition = 0;
		int mBytesPerChannel = 0;
		bool mDataIsFloat = false;
		uint32_t mDataPosition = 0;

		WavFormatExData mFormat = {};

		bool readHeader();
		void printFormat();

		std::vector<uint8_t> mUInt8Buffer;
		std::vector<int16_t> mInt16Buffer;
		std::vector<int32_t> mInt32Buffer;
	};

} /* namespace od */
