#include "resource.h"
#include <unistd.h>
#include <errno.h>
#include "warning.h"
#include <hal/log.h>

static int card_close(int fd)
{
	FRESULT res;
	file_t *fp = lookupCardFileDescriptor(fd);

	if (fp == NULL)
	{
		logWarn("Bad file descriptor.");
		errno = EBADF;
		return -1;
	}

	res = f_sync(&fp->fil);
	if (res != FR_OK)
	{
		logWarn("f_sync failed. (res = %d)", res);
		errno = EIO;
		return -1;
	}

	res = f_close(&fp->fil);
	destroyCardFileDescriptor(fd);

	if (res != FR_OK)
	{
		logWarn("f_close failed. (res = %d)", res);
		errno = EIO;
		return -1;
	}

	return 0;
}

static int ramdisk_close(int fd)
{
	RamFile *fp = lookupRamFileDescriptor(fd);

	if (fp == NULL)
	{
		errno = EBADF;
		return -1;
	}

	destroyRamFileDescriptor(fd);

	return 0;
}

int _close(int fd)
{
	if (isCardFileDescriptor(fd))
	{
		return card_close(fd);
	}
	else if (isRamFileDescriptor(fd))
	{
		return ramdisk_close(fd);
	}
	else
	{
		errno = EBADF;
		return -1;
	}
}
