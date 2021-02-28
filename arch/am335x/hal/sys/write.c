/*
 * Stub version of write.
 */

#include "resource.h"
#include <unistd.h>
#include <errno.h>
#include "warning.h"
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>

int _write(int fd, char *ptr, int len)
{
	if (isCardFileDescriptor(fd))
	{
		UINT bw;
		FRESULT res;
		file_t *fp = lookupCardFileDescriptor(fd);

		if (fp == NULL)
		{
			errno = EBADF;
			return -1;
		}

    logDebug(1,"%d bytes", len);
		res = f_write(&fp->fil, ptr, len, &bw);
		if (res != FR_OK)
		{
			errno = EIO;
			return -1;
		}

		return bw;
	}
	else
	{
		/* Writing to a RamFile is not implemented. */
		errno = EBADF;
		return -1;
	}
}
