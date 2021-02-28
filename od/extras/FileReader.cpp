#include <od/extras/FileReader.h>
#include <hal/log.h>
#include <vector>

namespace od
{

	FileReader::FileReader() : mIsOpen(false)
	{
	}

	FileReader::~FileReader()
	{
		close();
	}

	bool FileReader::open(const std::string &filename)
	{
		close();
		mFilename = filename;
#ifdef BUILDOPT_USE_STDIO
		mFileDescriptor = fopen(mFilename.c_str(), "rb");
		mIsOpen = mFileDescriptor != 0;
#else
		mIsOpen = f_open(&mFileDescriptor, mFilename.c_str(), FA_READ) == FR_OK;
#endif
		return mIsOpen;
	}

	void FileReader::close()
	{
		if (mIsOpen)
#ifdef BUILDOPT_USE_STDIO
			fclose(mFileDescriptor);
#else
			f_close(&mFileDescriptor);
#endif
		mIsOpen = false;
	}

	bool FileReader::readHeader(std::string &description, uint32_t &major,
															uint32_t &minor)
	{
		uint32_t magic = 0, n = 0;
		if (readBytes(&magic, sizeof(uint32_t)) != sizeof(uint32_t))
		{
			logWarn("Failed to read magic.");
			return false;
		}

		if (magic != 0xABCDDCBA)
		{
			logWarn("Magic check failed.");
			return false;
		}

		if (readBytes(&n, sizeof(uint32_t)) != sizeof(uint32_t))
		{
			logWarn("Failed to read description length.");
			return false;
		}

		if (n == 0)
			return false;

		std::vector<char> buffer;
		buffer.resize(n);
		if (readBytes(buffer.data(), n) != n)
		{
			logWarn("Failed to read description string.");
			return false;
		}
		description.assign(buffer.begin(), buffer.end());
		if (readBytes(&major, sizeof(uint32_t)) != sizeof(uint32_t))
		{
			logWarn("Failed to read major version.");
			return false;
		}
		if (readBytes(&minor, sizeof(uint32_t)) != sizeof(uint32_t))
		{
			logWarn("Failed to read minor version.");
			return false;
		}

		return true;
	}

	uint32_t FileReader::readBytes(void *buffer, uint32_t len)
	{
#ifdef BUILDOPT_USE_STDIO
		uint32_t br = fread(buffer, sizeof(uint8_t), len, mFileDescriptor);
#else
		UINT br;
		FRESULT result = f_read(&mFileDescriptor, buffer, len, &br);
		if (result != FR_OK)
		{
			return 0;
		}
#endif
		return br;
	}

	uint32_t FileReader::readBytes(void *buffer, uint32_t len, uint32_t offset)
	{
#ifdef BUILDOPT_USE_STDIO
		uint32_t br;
		if (fseek(mFileDescriptor, offset, SEEK_SET) != 0)
		{
			return 0;
		}
		br = fread(buffer, sizeof(uint8_t), len, mFileDescriptor);
#else
		UINT br;
		FRESULT result;
		result = f_lseek(&mFileDescriptor, offset);
		if (result != FR_OK)
		{
			return 0;
		}
		result = f_read(&mFileDescriptor, buffer, len, &br);
		if (result != FR_OK)
		{
			return 0;
		}
#endif
		return br;
	}

	uint32_t FileReader::seekBytes(uint32_t offset)
	{
#ifdef BUILDOPT_USE_STDIO
		if (fseek(mFileDescriptor, offset, SEEK_SET) == 0)
		{
			return offset;
		}
		else
		{
			return 0;
		}
#else
		if (f_lseek(&mFileDescriptor, offset) != FR_OK)
		{
			return 0;
		}
		return f_tell(&mFileDescriptor);
#endif
	}

	uint32_t FileReader::tellBytes()
	{
#ifdef BUILDOPT_USE_STDIO
		return ftell(mFileDescriptor);
#else
		return f_tell(&mFileDescriptor);
#endif
	}

	uint32_t FileReader::getSizeInBytes()
	{
#ifdef BUILDOPT_USE_STDIO
		uint32_t save = ftell(mFileDescriptor);
		fseek(mFileDescriptor, 0, SEEK_END);
		uint32_t size = ftell(mFileDescriptor);
		fseek(mFileDescriptor, save, SEEK_SET);
		return size;
#else
		return f_size(&mFileDescriptor);
#endif
	}

	bool FileReader::readLine(std::string &line)
	{
		char buffer[512];
		buffer[0] = 0;
#ifdef BUILDOPT_USE_STDIO
		if (fgets(buffer, sizeof(buffer), mFileDescriptor))
		{
			line = buffer;
			return true;
		}
#else
		if (f_gets(buffer, sizeof(buffer), &mFileDescriptor))
		{
			line = buffer;
			return true;
		}
#endif
		return false;
	}

} /* namespace od */
