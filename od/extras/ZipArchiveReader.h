/*
 * ZipArchiveReader.h
 *
 *  Created on: 9 Nov 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_ZIPARCHIVEREADER_H_
#define APP_UTILS_ZIPARCHIVEREADER_H_

#include <od/extras/ReferenceCounted.h>
#include <string>
#include <miniz.h>

namespace od
{

	class ZipArchiveReader : public ReferenceCounted
	{
	public:
		ZipArchiveReader();
		virtual ~ZipArchiveReader();

		bool open(const char *filename);
		bool exists(const char *filename);
		void close();
		bool extract(const char *from, const char *to);
		std::string extractToString(const char *filename);

		void setIgnorePath(bool enable);
		void setCaseSensitive(bool enable);

		int getFileCount();
		std::string getFilename(int index);

	private:
		mz_zip_archive mArchive;
		bool mIsOpen = false;
		bool mIgnorePath = false;
		bool mCaseSensitive = false;

		mz_uint getFlags();
	};

} /* namespace od */

#endif /* APP_UTILS_ZIPARCHIVEREADER_H_ */
