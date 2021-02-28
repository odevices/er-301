#include <od/extras/ZipArchiveWriter.h>
#include <string.h>
#include <hal/fileops.h>

namespace od
{

	ZipArchiveWriter::ZipArchiveWriter()
	{
	}

	ZipArchiveWriter::~ZipArchiveWriter()
	{
		close();
	}

	bool ZipArchiveWriter::open(const char *filename, bool overwrite)
	{
		close();
		if (overwrite && pathExists(filename))
		{
			deleteFile(filename);
		}
		// Now try to open the archive.
		memset(&mArchive, 0, sizeof(mArchive));
		mIsOpen = mz_zip_writer_init_file(&mArchive, filename, 0);
		return mIsOpen;
	}

	bool ZipArchiveWriter::add(const char *srcPath, const char *dstPath)
	{
		if (!mIsOpen)
		{
			return false;
		}

		return mz_zip_writer_add_file(&mArchive, dstPath, srcPath, NULL, 0,
																	MZ_DEFAULT_COMPRESSION);
	}

	void ZipArchiveWriter::close()
	{
		if (mIsOpen)
		{
			mz_zip_writer_finalize_archive(&mArchive);
			mz_zip_writer_end(&mArchive);
			mIsOpen = false;
		}
	}

} /* namespace od */
