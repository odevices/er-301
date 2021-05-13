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

		bool readCues(std::vector<uint32_t> &positions);

		void describe();

		uint32_t getCueCount();

	protected:
		uint32_t mCurrentSamplePosition = 0;
		int mBytesPerChannel = 0;
		bool mDataIsFloat = false;
		uint32_t mDataPosition = 0;

		uint32_t mCueDataSize = 0;
		uint32_t mCuePointsPosition = 0;
		uint32_t mCuePointsCount = 0;

		WavFormatExData mFormat = {};

		bool readHeader();
		void printFormat();

		std::vector<uint8_t> mUInt8Buffer;
		std::vector<int16_t> mInt16Buffer;
		std::vector<int32_t> mInt32Buffer;
	};

} /* namespace od */
