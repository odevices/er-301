#include <od/audio/WavFileWriter.h>
#include <hal/log.h>
#include <vector>

#define MAX_WRITE_SIZE 4096
#define OFFSET_TO_DATA (sizeof(FileTypeChunk) + sizeof(RiffChunk) * 2 + sizeof(WavFormatData))

namespace od
{

	WavFileWriter::WavFileWriter()
	{
	}

	WavFileWriter::WavFileWriter(float sampleRate, uint32_t channelCount,
															 WavFileEncoding encoding) : SoundFileWriter(sampleRate, channelCount), mWavFileEncoding(encoding)
	{
		populateWavFormatData();
	}

	WavFileWriter::~WavFileWriter()
	{
	}

	void WavFileWriter::populateWavFormatData()
	{
		// fill out format
		switch (mWavFileEncoding)
		{
		case wav16bit:
			mFormat.wFormatTag = WAV_FORMAT_PCM;
			mFormat.wBitsPerSample = 16;
			mFormat.nBlockAlign = 2 * mChannelCount;
			break;
		case wav24bit:
			mFormat.wFormatTag = WAV_FORMAT_PCM;
			mFormat.wBitsPerSample = 24;
			mFormat.nBlockAlign = 3 * mChannelCount;
			break;
		case wavFloat:
			mFormat.wFormatTag = WAV_FORMAT_FLOAT;
			mFormat.wBitsPerSample = 32;
			mFormat.nBlockAlign = 4 * mChannelCount;
			break;
		}

		mFormat.nChannels = mChannelCount;
		mFormat.nSamplesPerSec = (uint32_t)mSampleRate;
		mFormat.nAvgBytesPerSec = (mFormat.nSamplesPerSec * mFormat.nChannels * mFormat.wBitsPerSample) / 8;
	}

	void WavFileWriter::init(float sampleRate, uint32_t channelCount,
													 WavFileEncoding encoding)
	{
		SoundFileWriter::init(sampleRate, channelCount);
		mWavFileEncoding = encoding;
		populateWavFormatData();
	}

	bool WavFileWriter::prepare()
	{
		mCurrentSamplePosition = 0;
		return writeHeader();
	}

	bool WavFileWriter::finalize()
	{
		return writeHeader();
	}

	bool WavFileWriter::writeHeader()
	{
		uint32_t bw;
		uint32_t dataSize = mSampleCount * mFormat.nBlockAlign;
		uint32_t fileSize = OFFSET_TO_DATA + dataSize;
		FileTypeChunk firstChunk{{'R', 'I', 'F', 'F'}, fileSize - 8, {'W', 'A', 'V', 'E'}};
		RiffChunk fmtChunk{{'f', 'm', 't', ' '}, sizeof(WavFormatData)};
		RiffChunk dataChunk{{'d', 'a', 't', 'a'}, dataSize};

		logDebug(1, "WavFileWriter::writeHeader: %d samples %d bytes align", mSampleCount, mFormat.nBlockAlign);
		logDebug(1, "  FileTypeChunk: cksize=%d", firstChunk.cksize);
		logDebug(1, "  Format Chunk: cksize=%d", fmtChunk.cksize);
		logDebug(1, "  Data Chunk: cksize=%d", dataChunk.cksize);

		if (!mIsOpen)
			return false;

		if (seekBytes(0) != 0)
		{
			return false;
		}

		bw = writeBytes(&firstChunk, sizeof(FileTypeChunk));
		if (bw != sizeof(FileTypeChunk))
		{
			return false;
		}

		bw = writeBytes(&fmtChunk, sizeof(RiffChunk));
		if (bw != sizeof(RiffChunk))
		{
			return false;
		}

		bw = writeBytes(&mFormat, sizeof(WavFormatData));
		if (bw != sizeof(WavFormatData))
		{
			return false;
		}

		bw = writeBytes(&dataChunk, sizeof(RiffChunk));
		if (bw != sizeof(RiffChunk))
		{
			return false;
		}

		return true;
	}

	uint32_t WavFileWriter::seekSamples(uint32_t sampleOffset)
	{
		uint32_t byteOffset = seekBytes(OFFSET_TO_DATA + sampleOffset * mFormat.nBlockAlign);
		mCurrentSamplePosition = (byteOffset - OFFSET_TO_DATA) / mFormat.nBlockAlign;
		return mCurrentSamplePosition;
	}

	uint32_t WavFileWriter::tellSamples()
	{
		return mCurrentSamplePosition;
	}

	uint32_t WavFileWriter::writeSamples(float *buffer, uint32_t len)
	{
		uint32_t bytesWritten = 0;
		uint32_t samplesWritten;

		switch (mWavFileEncoding)
		{
		case wav16bit:
			bytesWritten = write16bit(buffer, len);
			break;
		case wav24bit:
			bytesWritten = write24bit(buffer, len);
			break;
		case wavFloat:
			bytesWritten = writeBytes(buffer, len * mFormat.nBlockAlign);
			break;
		default:
			break;
		}

		samplesWritten = bytesWritten / mFormat.nBlockAlign;
		mCurrentSamplePosition += samplesWritten;
		mSampleCount += samplesWritten;
		logDebug(1, "WavFileWriter::writeSamples: mSampleCount = %d", mSampleCount);
		return samplesWritten;
	}

	uint32_t WavFileWriter::write16bit(float *buffer, uint32_t len)
	{
		std::vector<int16_t> tmp;
		uint32_t writeLen, n, nbytes;
		uint32_t bw = 0;

		while (len > 0)
		{
			if (len > MAX_WRITE_SIZE)
			{
				writeLen = MAX_WRITE_SIZE;
			}
			else
			{
				writeLen = len;
			}

			n = writeLen * mFormat.nChannels;

			tmp.clear();
			tmp.reserve(n);
			for (uint32_t i = 0; i < n; i++)
			{
				tmp.push_back((int16_t)(buffer[i] * (1 << 15)));
			}

			nbytes = writeLen * mFormat.nBlockAlign;
			if (writeBytes(tmp.data(), nbytes) != nbytes)
				break;
			bw += nbytes;
			len -= writeLen;
			buffer += n;
		}
		return bw;
	}

	uint32_t WavFileWriter::write24bit(float *buffer, uint32_t len)
	{
		std::vector<uint8_t> tmp;
		uint32_t writeLen, n, nbytes;
		uint32_t bw = 0;
		int32_t sample;

		while (len > 0)
		{
			if (len > MAX_WRITE_SIZE)
			{
				writeLen = MAX_WRITE_SIZE;
			}
			else
			{
				writeLen = len;
			}

			n = writeLen * mFormat.nChannels;

			tmp.clear();
			tmp.reserve(3 * n);
			for (uint32_t i = 0; i < n; i++)
			{
				sample = (int32_t)(buffer[i] * (1 << 31));
				tmp.push_back((sample >> 8) & 0xFF);
				tmp.push_back((sample >> 16) & 0xFF);
				tmp.push_back((sample >> 24) & 0xFF);
			}

			nbytes = writeLen * mFormat.nBlockAlign;
			if (writeBytes(tmp.data(), nbytes) != nbytes)
				break;
			bw += nbytes;
			len -= writeLen;
			buffer += n;
		}
		return bw;
	}

} /* namespace od */
