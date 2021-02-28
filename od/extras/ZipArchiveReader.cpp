/*
 * ZipArchiveReader.cpp
 *
 *  Created on: 9 Nov 2016
 *      Author: clarkson
 */

#include <od/extras/ZipArchiveReader.h>
#include <string.h>

namespace od
{

    ZipArchiveReader::ZipArchiveReader()
    {
    }

    ZipArchiveReader::~ZipArchiveReader()
    {
        close();
    }

    bool ZipArchiveReader::open(const char *filename)
    {
        close();
        // Now try to open the archive.
        memset(&mArchive, 0, sizeof(mArchive));
        mIsOpen = mz_zip_reader_init_file(&mArchive, filename, 0);
        return mIsOpen;
    }

    void ZipArchiveReader::close()
    {
        if (mIsOpen)
        {
            mz_zip_reader_end(&mArchive);
            mIsOpen = false;
        }
    }

    bool ZipArchiveReader::exists(const char *filename)
    {
        if (mIsOpen)
        {
            int fileIndex = mz_zip_reader_locate_file(&mArchive, filename, NULL, getFlags());
            if (fileIndex >= 0)
            {
                return true;
            }
        }

        return false;
    }

    bool ZipArchiveReader::extract(const char *from, const char *to)
    {
        if (mIsOpen)
        {
            return mz_zip_reader_extract_file_to_file(&mArchive, from, to, getFlags());
        }
        else
        {
            return false;
        }
    }

    std::string ZipArchiveReader::extractToString(const char *filename)
    {
        if (mIsOpen)
        {
            size_t nbytes = 0;
            char *buffer = (char *)mz_zip_reader_extract_file_to_heap(&mArchive, filename, &nbytes, getFlags());
            if (nbytes > 0 && buffer)
            {
                return std::string(buffer, nbytes);
            }
        }

        return "";
    }

    void ZipArchiveReader::setIgnorePath(bool enable)
    {
        mIgnorePath = enable;
    }

    void ZipArchiveReader::setCaseSensitive(bool enable)
    {
        mCaseSensitive = enable;
    }

    mz_uint ZipArchiveReader::getFlags()
    {
        mz_uint flag = 0;
        if (mIgnorePath)
        {
            flag |= MZ_ZIP_FLAG_IGNORE_PATH;
        }

        if (mCaseSensitive)
        {
            flag |= MZ_ZIP_FLAG_CASE_SENSITIVE;
        }

        return flag;
    }

    int ZipArchiveReader::getFileCount()
    {
        if (mIsOpen)
        {
            return (int)mz_zip_reader_get_num_files(&mArchive);
        }
        else
        {
            return 0;
        }
    }

    std::string ZipArchiveReader::getFilename(int index)
    {
        if (mIsOpen)
        {
            mz_zip_archive_file_stat stat;
            if (mz_zip_reader_file_stat(&mArchive, index, &stat))
            {
                return stat.m_filename;
            }
        }
        return "";
    }

} /* namespace od */
