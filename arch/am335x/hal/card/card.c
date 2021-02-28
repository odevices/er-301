#include <string.h>
#include <hal/fatfs/ff.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/mmcsd_lld.h>
#include <hal/dma.h>
#include <hal/constants.h>
#include <hal/gpio.h>
#include <hal/events.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <hal/card.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#include "sd_controller.h"

#define SD_CACHE_BYTES (256 * 1024)
#define SD_CACHE_BLOCKS (SD_CACHE_BYTES / 512)

typedef struct sd
{
  BYTE cache[CACHE_ALIGNED_SIZE(SD_CACHE_BYTES)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
  sd_controller_t controller;
  FATFS fs;
  TCHAR *path;
  int mode;
  Semaphore_Handle mutex;
} sd_t;

static sd_t sd[2];

static Semaphore_Handle mutex_create()
{
  Semaphore_Params params;
  Semaphore_Params_init(&params);
  params.mode = Semaphore_Mode_BINARY;
  return Semaphore_create(1, &params, NULL);
}

static void mutex_enter(uint32_t drv)
{
  Semaphore_pend(sd[drv].mutex, BIOS_WAIT_FOREVER);
}

static void mutex_leave(uint32_t drv)
{
  Semaphore_post(sd[drv].mutex);
}

static bool mutex_try_enter(uint32_t drv)
{
  return Semaphore_pend(sd[drv].mutex, BIOS_NO_WAIT);
}

void Card_init()
{
  EDMA3_DRV_Handle hEdma = DMA_getHandle();

  sd[0].mutex = mutex_create();
  sd[1].mutex = mutex_create();

  sd_controller_init(&sd[0].controller, SOC_MMCHS_0_REGS, MMCSD0_INT);
  sd_controller_init(&sd[1].controller, SOC_MMCHS_1_REGS, MMCSD1_INT);

  sd_controller_init_dma(&sd[0].controller, hEdma,
                         EDMA3_CHA_MMCHS0_TX,
                         EDMA3_CHA_MMCHS0_RX);
  sd_controller_init_dma(&sd[1].controller, hEdma,
                         EDMA3_CHA_MMCHS1_TX,
                         EDMA3_CHA_MMCHS1_RX);

  sd[0].controller.id = 0;
  sd[1].controller.id = 1;
  sd[0].mode = CARD_MODE_NOT_CONNECTED;
  sd[1].mode = CARD_MODE_NOT_CONNECTED;
  sd[0].path = "0:";
  sd[1].path = "1:";
}

bool Card_mount(uint32_t drv)
{
  if (drv < 2)
  {
    if (sd[drv].mode == CARD_MODE_NOT_CONNECTED)
    {
      if (Card_connect(drv, CARD_MODE_FATFS))
      {
        FRESULT result;
        result = f_mount(&sd[drv].fs, sd[drv].path, 1);
        if (result == FR_OK)
        {
          return true;
        }
        else
        {
          Card_disconnect(drv);
        }
      }
    }
  }

  return false;
}

void Card_unmount(uint32_t drv)
{
  if (Card_isMounted(drv))
  {
    f_unmount(sd[drv].path);
    Card_disconnect(drv);
  }
}

bool Card_isMounted(uint32_t drv)
{
  if (drv < 2)
  {
    return sd[drv].mode == CARD_MODE_FATFS;
  }
  else
    return false;
}

bool Card_format(uint32_t drv)
{
  if (drv < 2)
  {
    BYTE work[2 * FF_MAX_SS];
    bool success = false;
    bool wasMounted = Card_isMounted(drv);
    if (wasMounted)
    {
      Card_unmount(drv);
    }

    if (Card_connect(drv, CARD_MODE_FATFS))
    {
      success = f_mkfs(sd[drv].path, NULL, work, sizeof(work)) == FR_OK;
      Card_disconnect(drv);
    }

    if (wasMounted)
    {
      return Card_mount(drv) && success;
    }

    return success;
  }
  else
  {
    return false;
  }
}

uint32_t Card_sizeInBlocks(uint32_t drv)
{
  if (drv < 2 && sd[drv].controller.isOpen)
  {
    return sd[drv].controller.blockCount;
  }
  else
    return 0;
}

bool Card_readBlocks(uint32_t drv, uint8_t *buffer, uint32_t sector, uint32_t count)
{
  logAssert(drv < 2);
  uint32_t address = 0U;
  sd_transaction_t transaction;
  sd_controller_t *controller = &sd[drv].controller;
  uint32_t safe_count;

  if (!controller->isOpen)
  {
    logDebug(3, "Controller not open.");
    return false;
  }

  Led_on(LED_IO);
  while (count > 0)
  {
    mutex_enter(drv);
    uint32_t nbytes;

    if (count > SD_CACHE_BLOCKS)
    {
      safe_count = SD_CACHE_BLOCKS;
    }
    else
    {
      safe_count = count;
    }

    if (controller->supportsCMD23 && safe_count > 1U)
    {
      transaction.cmd = MMCSD_SET_BLOCK_COUNT;
      transaction.arg = safe_count;
      transaction.flags = 0;
      if (sd_controller_xfer(controller, &transaction) != SD_OK)
      {
        Led_off(LED_IO);
        mutex_leave(drv);
        logDebug(3, "MMCSD_SET_BLOCK_COUNT failed.");
        return false;
      }
    }

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
    transaction.blockCount = safe_count;
    transaction.blockSize = controller->blockSize;
    transaction.dataBuf = buffer;

    nbytes = safe_count * transaction.blockSize;

    if (safe_count > 1U)
    {
      transaction.cmd = MMCSD_READ_MULTIPLE_BLOCK;
    }
    else
    {
      transaction.cmd = MMCSD_READ_SINGLE_BLOCK;
    }

    if (safe_count > 1)
    {
      transaction.dataBuf = sd[drv].cache;
      if (sd_controller_dma_xfer(controller, &transaction) != SD_OK)
      {
        Led_off(LED_IO);
        mutex_leave(drv);
        logDebug(3, "MMCSD_READ_MULTIPLE_BLOCK failed.");
        return false;
      }
      else
      {
        EDMA3_DRV_Result result = EDMA3_DRV_SOK;
        result = Edma3_CacheInvalidate((uint32_t)transaction.dataBuf,
                                       nbytes);
        logAssert(result == EDMA3_DRV_SOK);
        memcpy(buffer, sd[drv].cache, nbytes);
      }
    }
    else
    {
      if (sd_controller_xfer(controller, &transaction) != SD_OK)
      {
        Led_off(LED_IO);
        mutex_leave(drv);
        logDebug(3, "MMCSD_READ_SINGLE_BLOCK failed.");
        return false;
      }
    }

    sector += safe_count;
    count -= safe_count;
    buffer += nbytes;
    mutex_leave(drv);
  }

  Led_off(LED_IO);
  return true;
}

bool Card_readAlignedBlocks(uint32_t drv, uint8_t *buffer, uint32_t sector, uint32_t count)
{
  logAssert(drv < 2);
  uint32_t address = 0U;
  sd_transaction_t transaction;
  sd_controller_t *controller = &sd[drv].controller;

  if (!controller->isOpen)
    return false;

  Led_on(LED_IO);
  mutex_enter(drv);
  uint32_t nbytes;

  if (controller->supportsCMD23 && count > 1U)
  {
    transaction.cmd = MMCSD_SET_BLOCK_COUNT;
    transaction.arg = count;
    transaction.flags = 0;
    if (sd_controller_xfer(controller, &transaction) != SD_OK)
    {
      Led_off(LED_IO);
      mutex_leave(drv);
      return false;
    }
  }

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
  transaction.blockCount = count;
  transaction.blockSize = controller->blockSize;
  transaction.dataBuf = buffer;

  nbytes = count * transaction.blockSize;

  if (count > 1U)
  {
    transaction.cmd = MMCSD_READ_MULTIPLE_BLOCK;
  }
  else
  {
    transaction.cmd = MMCSD_READ_SINGLE_BLOCK;
  }

  if (sd_controller_dma_xfer(controller, &transaction) != SD_OK)
  {
    Led_off(LED_IO);
    mutex_leave(drv);
    return false;
  }
  else
  {
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    result = Edma3_CacheInvalidate((uint32_t)transaction.dataBuf, nbytes);
    logAssert(result == EDMA3_DRV_SOK);
  }

  mutex_leave(drv);
  Led_off(LED_IO);
  return true;
}

static bool writeBlocksHelper(sd_controller_t *controller, uint8_t *buffer,
                              uint32_t safe_count, uint32_t sector, uint8_t *cache)
{
  sd_transaction_t transaction;

  if (controller->supportsCMD23 && safe_count > 1U)
  {
    transaction.cmd = MMCSD_SET_BLOCK_COUNT;
    transaction.arg = safe_count;
    transaction.flags = 0;
    if (sd_controller_xfer(controller, &transaction) != SD_OK)
    {
      return false;
    }
  }

  /*
     * Address is in blks for high cap cards and in actual bytes
     * for standard capacity cards
     */
  uint32_t address = 0U;
  if (0 != controller->highCap)
  {
    address = sector;
  }
  else
  {
    address = sector * controller->blockSize;
  }

  transaction.flags = MMCSD_CMDRSP_WRITE | MMCSD_CMDRSP_DATA;
  transaction.arg = address;
  transaction.blockCount = safe_count;
  transaction.blockSize = controller->blockSize;
  transaction.dataBuf = buffer;

  if (safe_count > 1U)
  {
    transaction.cmd = MMCSD_WRITE_MULTIPLE_BLOCK;
  }
  else
  {
    transaction.cmd = MMCSD_WRITE_SINGLE_BLOCK;
  }

  if (safe_count > 1)
  {
    uint32_t nbytes = safe_count * controller->blockSize;
    transaction.dataBuf = cache;
    memcpy(cache, buffer, nbytes);
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    result = Edma3_CacheFlush((uint32_t)transaction.dataBuf, nbytes);
    logAssert(result == EDMA3_DRV_SOK);
    if (sd_controller_dma_xfer(controller, &transaction) != SD_OK)
    {
      return false;
    }
  }
  else
  {
    if (sd_controller_xfer(controller, &transaction) != SD_OK)
    {
      return false;
    }
  }

  return true;
}

bool Card_writeBlocks(uint32_t drv, uint8_t *buffer, uint32_t sector, uint32_t count)
{
  logAssert(drv < 2);
  sd_controller_t *controller = &sd[drv].controller;
  uint32_t safe_count;

  if (!controller->isOpen)
    return false;

  Led_on(LED_IO);
  while (count > 0)
  {
    mutex_enter(drv);

    if (count > SD_CACHE_BLOCKS)
    {
      safe_count = SD_CACHE_BLOCKS;
    }
    else
    {
      safe_count = count;
    }

    int retry = 5;
    while (retry > 0)
    {
      if (writeBlocksHelper(controller, buffer, safe_count, sector, sd[drv].cache))
      {
        break;
      }
      retry--;
      sd_controller_close(controller);
      sd_controller_open(controller);
    }

    if (retry == 0)
    {
      Led_off(LED_IO);
      mutex_leave(drv);
      return false;
    }

    sector += safe_count;
    count -= safe_count;
    buffer += safe_count * controller->blockSize;
    mutex_leave(drv);
  }

  Led_off(LED_IO);
  return true;
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

uint32_t Card_getVersion(uint32_t drv)
{
  return sd[drv].controller.sdVer;
}

uint32_t Card_getBusWidth(uint32_t drv)
{
  return MMCSD_GET_CARD_BUSWIDTH(sd[drv].controller);
}

uint32_t Card_getTransferSpeed(uint32_t drv)
{
  return MMCSD_GET_CARD_FRE(sd[drv].controller);
}

bool Card_supportsCMD23(uint32_t drv)
{
  return sd[drv].controller.supportsCMD23;
}

bool Card_isConnected(uint32_t drv)
{
  return sd[drv].controller.isOpen;
}

uint32_t Card_getMode(uint32_t drv)
{
  return sd[drv].mode;
}

bool Card_connect(uint32_t drv, uint32_t requestedMode)
{
  if (sd[drv].controller.isOpen && sd[drv].mode == requestedMode)
  {
    // Already connected in the requested mode.
    return true;
  }

  Card_disconnect(drv);

  if (requestedMode == CARD_MODE_NOT_CONNECTED)
  {
    // This should never happen but let it pass anyways.
    return true;
  }

  if (sd_controller_open(&sd[drv].controller) == SD_OK)
  {
    sd[drv].mode = requestedMode;
    switch (requestedMode)
    {
    case CARD_MODE_RAW:
      logDebug(1, "drv=%d connected in raw mode", drv);
      if (drv == CARD_REAR)
      {
        Events_push(EVENT_USB_REAR_CARD_MOUNT);
      }
      else
      {
        Events_push(EVENT_USB_FRONT_CARD_MOUNT);
      }
      break;
    case CARD_MODE_FATFS:
      logDebug(1, "drv=%d connected in fatfs mode", drv);
      break;
    default:
      break;
    }
    return true;
  }
  else
  {
    logDebug(1, "drv=%d failed to connect", drv);
    return false;
  }
}

void Card_disconnect(uint32_t drv)
{
  if (sd[drv].controller.isOpen)
  {
    mutex_enter(drv);
    sd_controller_close(&sd[drv].controller);
    mutex_leave(drv);
    logDebug(1, "drv=%d disconnected", drv);
  }
  if (sd[drv].mode == CARD_MODE_RAW)
  {
    if (drv == CARD_REAR)
    {
      Events_push(EVENT_USB_REAR_CARD_UNMOUNT);
    }
    else
    {
      Events_push(EVENT_USB_FRONT_CARD_UNMOUNT);
    }
  }
  sd[drv].mode = CARD_MODE_NOT_CONNECTED;
}

bool Card_isPresent(uint32_t drv)
{
  if (!mutex_try_enter(drv))
  {
    // In use therefore it is present.
    return true;
  }
  else
  {
    bool present = false;
    if (sd[drv].controller.isOpen)
    {
      present = sd_controller_test_card_present(&sd[drv].controller);
      if (!present)
      {
        // No longer present so close the controller
        sd_controller_close(&sd[drv].controller);
        sd[drv].mode = CARD_MODE_NOT_CONNECTED;
      }
    }
    else if (sd_controller_open(&sd[drv].controller) == SD_OK)
    {
      sd_controller_close(&sd[drv].controller);
      present = true;
    }
    mutex_leave(drv);
    return present;
  }
}