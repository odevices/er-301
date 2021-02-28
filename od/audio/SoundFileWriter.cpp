#include <od/audio/SoundFileWriter.h>

namespace od
{

	SoundFileWriter::SoundFileWriter()
	{
	}

	SoundFileWriter::SoundFileWriter(float sampleRate, uint32_t channelCount) : mSampleRate(sampleRate), mChannelCount(channelCount)
	{
	}

	SoundFileWriter::~SoundFileWriter()
	{
	}

	void SoundFileWriter::init(float sampleRate, uint32_t channelCount)
	{
		mSampleRate = sampleRate;
		mChannelCount = channelCount;
	}

	bool SoundFileWriter::open(const std::string &filename)
	{
		mSampleCount = 0;
		if (FileWriter::open(filename))
		{
			return prepare();
		}
		else
		{
			return false;
		}
	}

	bool SoundFileWriter::close()
	{
		bool result1 = finalize();
		bool result2 = FileWriter::close();
		return result1 && result2;
	}

} /* namespace od */
