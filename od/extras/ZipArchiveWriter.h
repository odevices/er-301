#pragma once

#include <od/extras/ReferenceCounted.h>
#include <miniz.h>

namespace od
{

	class ZipArchiveWriter : public ReferenceCounted
	{
	public:
		ZipArchiveWriter();
		virtual ~ZipArchiveWriter();

		bool open(const char *filename, bool overwrite = false);
		bool add(const char *srcPath, const char *dstPath);
		void close();

	private:
		mz_zip_archive mArchive;
		bool mIsOpen = false;
	};

} /* namespace od */
