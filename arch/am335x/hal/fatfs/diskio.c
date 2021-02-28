#include "ff.h"
#include "diskio.h"
#include <hal/card.h>
//#define BUILDOPT_VERBOSE
#include <hal/log.h>

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    logAssert(pdrv < 2);
    if (Card_getMode(pdrv) == CARD_MODE_FATFS)
    {
        return 0;
    }
    else
    {
        logWarn("Card not initialize. drv=%d", pdrv);
        return STA_NOINIT;
    }
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    logAssert(pdrv < 2);
    uint32_t mode = Card_getMode(pdrv);
    if (mode == CARD_MODE_FATFS)
    {
        return 0;
    }
    else if (mode == CARD_MODE_NOT_CONNECTED && Card_connect(pdrv, CARD_MODE_FATFS))
    {
        return 0;
    }
    else
    {
        logWarn("Card failed to initialize. drv=%d", pdrv);
        return STA_NOINIT;
    }
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv,    /* Physical drive nmuber to identify the drive */
                  BYTE *buffer, /* Data buffer to store read data */
                  DWORD sector, /* Sector address in LBA */
                  UINT count    /* Number of sectors to read */
)
{
    logAssert(pdrv < 2);

    if (Card_getMode(pdrv) != CARD_MODE_FATFS)
    {
        logWarn("Card not ready. drv=%d mode=%d", pdrv, Card_getMode(pdrv));
        return RES_NOTRDY;
    }

    if (Card_readBlocks(pdrv, buffer, sector, count))
    {
        return RES_OK;
    }
    else
    {
        logWarn("Read blocks failed. drv=%d", pdrv);
        return RES_ERROR;
    }
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write(BYTE pdrv,          /* Physical drive nmuber to identify the drive */
                   const BYTE *buffer, /* Data to be written */
                   DWORD sector,       /* Sector address in LBA */
                   UINT count          /* Number of sectors to write */
)
{
    logAssert(pdrv < 2);

    if (Card_getMode(pdrv) != CARD_MODE_FATFS)
    {
        logWarn("Card not ready. drv=%d mode=%d", pdrv, Card_getMode(pdrv));
        return RES_NOTRDY;
    }

    if (Card_writeBlocks(pdrv, (uint8_t *)buffer, sector, count))
    {
        return RES_OK;
    }
    else
    {
        logWarn("Write blocks failed. drv=%d", pdrv);
        return RES_ERROR;
    }
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE pdrv, /* Physical drive nmuber (0..) */
                   BYTE cmd,  /* Control code */
                   void *buff /* Buffer to send/receive control data */
)
{
    logAssert(pdrv < 2);
    if (Card_getMode(pdrv) == CARD_MODE_FATFS)
    {
        switch (cmd)
        {
        case CTRL_SYNC:
            return RES_OK;
        case GET_BLOCK_SIZE:
            if (buff)
            {
                DWORD *blockSize = (DWORD *)buff;
                *blockSize = 1;
                return RES_OK;
            }
            else
            {
                return RES_ERROR;
            }
        case GET_SECTOR_COUNT:
            if (buff)
            {
                DWORD *sectorCount = (DWORD *)buff;
                *sectorCount = Card_sizeInBlocks(pdrv);
                return RES_OK;
            }
            else
            {
                return RES_ERROR;
            }

        default:
            return RES_OK;
        }
    }
    else
    {
        return RES_NOTRDY;
    }
}
