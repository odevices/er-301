#pragma once

#include <od/extras/ReferenceCounted.h>
#include <string>

// Only use STDIO when not building SBL image.
#ifndef BUILDOPT_SBL
#define BUILDOPT_USE_STDIO
#endif

#ifdef BUILDOPT_USE_STDIO
#include <stdio.h>
#else
#include <hal/fatfs/ff.h>
#endif

namespace od
{

	class FileReader : public ReferenceCounted
	{
	public:
		FileReader();
		virtual ~FileReader();

		virtual bool open(const std::string &filename);
		virtual void close();
		virtual uint32_t readBytes(void *buffer, uint32_t len);
		virtual uint32_t readBytes(void *buffer, uint32_t len, uint32_t offset);
		virtual uint32_t seekBytes(uint32_t offset);
		virtual uint32_t tellBytes();
		virtual uint32_t getSizeInBytes();

		bool readHeader(std::string &description, uint32_t &major, uint32_t &minor);
		bool readLine(std::string &line);

		std::string mFilename;
		bool mIsOpen;

	private:
#ifdef BUILDOPT_USE_STDIO
		FILE *mFileDescriptor;
#else
		FIL mFileDescriptor;
#endif
	};

} /* namespace od */
