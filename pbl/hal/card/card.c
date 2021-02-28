/*
 * sd.c
 *
 *  Created on: 4 Dec 2015
 *      Author: clarkson
 */

#include <stdint.h>
#include <stdbool.h>
#include <hal/types.h>
#include <hal/fatfs/ff.h>
#include <hal/fatfs/diskio.h> /* FatFs lower layer API */
#include <hal/log.h>
#include <hal/constants.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/mmcsd_lld.h>
#include <ti/sysbios/family/arm/a8/intcps/Hwi.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/gates/GateMutex.h>
#include "sd_controller.h"

#define SD_CACHE_BYTES (2 * 1024)
#define SD_CACHE_BLOCKS (SD_CACHE_BYTES / 512)

typedef struct sd
{
    BYTE cache[CACHE_ALIGNED_SIZE(SD_CACHE_BYTES)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
    sd_controller_t controller;
    DSTATUS state;
    FATFS fs;
    TCHAR *path;
    GateMutex_Handle mutex;
} sd_t;

static sd_t sd[2];

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    logAssert(pdrv < 2);
    switch (pdrv)
    {
    case 0:
        return sd[0].state;
    case 1:
        return sd[1].state;
    }
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    logAssert(pdrv < 2);
    IArg key = GateMutex_enter(sd[pdrv].mutex);
    if (sd[pdrv].controller.isOpen)
    {
        sd[pdrv].state = 0;
    }
    else if (sd_controller_open(&sd[pdrv].controller) == SD_OK)
    {
        sd[pdrv].state = 0;
    }
    else
    {
        sd_controller_close(&sd[pdrv].controller);
        sd[pdrv].state = STA_NOINIT;
    }
    GateMutex_leave(sd[pdrv].mutex, key);
    return sd[pdrv].state;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv,    /* Physical drive nmuber to identify the drive */
                  BYTE *buff,   /* Data buffer to store read data */
                  DWORD sector, /* Sector address in LBA */
                  UINT count    /* Number of sectors to read */
)
{
    logAssert(pdrv < 2);
    uint32_t address = 0U;
    sd_transaction_t transaction;
    sd_controller_t *controller = &sd[pdrv].controller;

    if (!controller->isOpen)
        return RES_NOTRDY;

    while (count > 0)
    {
        IArg key = GateMutex_enter(sd[pdrv].mutex);
        uint32_t nbytes;

        /*
         * Address is in blks for high cap cards and in actual bytes
         * for standard capacity cards
         */
        if (0 != controller->highCap)
        {
            address = sector;
        }
        else
        {
            address = sector * controller->blockSize;
        }

        transaction.flags = MMCSD_CMDRSP_READ | MMCSD_CMDRSP_DATA;
        transaction.arg = address;
        transaction.blockCount = 1;
        transaction.blockSize = controller->blockSize;
        transaction.dataBuf = buff;
        nbytes = transaction.blockSize;
        transaction.cmd = MMCSD_READ_SINGLE_BLOCK;

        if (sd_controller_xfer(controller, &transaction) != SD_OK)
        {
            GateMutex_leave(sd[pdrv].mutex, key);
            return RES_ERROR;
        }

        sector += 1;
        count -= 1;
        buff += nbytes;
        GateMutex_leave(sd[pdrv].mutex, key);
    }

    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write(BYTE pdrv,          /* Physical drive nmuber to identify the drive */
                   const BYTE *buffer, /* Data to be written */
                   DWORD sector,       /* Sector address in LBA */
                   UINT count          /* Number of sectors to write */
)
{ // not implemented
    return RES_ERROR;
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
    if (sd[pdrv].controller.isOpen)
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
                *sectorCount = sd[pdrv].controller.blockCount;
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

void Card_init()
{
    sd[0].mutex = GateMutex_create(NULL, NULL);
    sd[1].mutex = GateMutex_create(NULL, NULL);

    sd_controller_init(&sd[0].controller, SOC_MMCHS_0_REGS, MMCSD0_INT);
    sd_controller_init(&sd[1].controller, SOC_MMCHS_1_REGS, MMCSD1_INT);

    sd[0].controller.id = 0;
    sd[1].controller.id = 1;
    sd[0].state = STA_NOINIT;
    sd[1].state = STA_NOINIT;
    sd[0].path = "0:";
    sd[1].path = "1:";
}

bool Card_mount(uint32_t drv)
{
    if (drv < 2)
    {
        // forced mount
        FRESULT result;
        result = f_mount(&sd[drv].fs, sd[drv].path, 1);
        return result == FR_OK;
    }
    else
    {
        return false;
    }
}

void Card_unmount(uint32_t drv)
{
    if (drv < 2)
    {
        // forced unmount
        f_mount(NULL, sd[drv].path, 1);
        sd_controller_close(&sd[drv].controller);
    }
}

bool Card_isMounted(uint32_t drv)
{
    if (drv < 2)
    {
        return sd[drv].state == 0;
    }
    else
        return false;
}

uint32_t Card_sizeInBlocks(uint32_t drv)
{
    if (drv < 2)
    {
        return sd[drv].controller.blockCount;
    }
    else
        return 0;
}

bool Card_readBlocks(uint32_t drv, uint8_t *pData, uint32_t sector,
                     uint32_t numBlocks)
{
    return disk_read(drv, pData, sector, numBlocks) == RES_OK;
}

bool Card_writeBlocks(uint32_t drv, uint8_t *pData, uint32_t sector,
                      uint32_t numBlocks)
{
    return disk_write(drv, pData, sector, numBlocks) == RES_OK;
}

void Card_printErrorStatus(void)
{
    logInfo("card 0: timeout=%d", sd[0].controller.waitTimeout);
    logInfo("card 1: timeout=%d", sd[1].controller.waitTimeout);
}

bool Card_isHighCapacity(uint32_t drv)
{
    return sd[drv].controller.highCap;
}

int Card_getVersion(uint32_t drv)
{
    return sd[drv].controller.sdVer;
}

int Card_getBusWidth(uint32_t drv)
{
    return MMCSD_GET_CARD_BUSWIDTH(sd[drv].controller);
}

int Card_getTransferSpeed(uint32_t drv)
{
    return MMCSD_GET_CARD_FRE(sd[drv].controller);
}

bool Card_supportsCMD23(uint32_t drv)
{
    return sd[drv].controller.supportsCMD23;
}
