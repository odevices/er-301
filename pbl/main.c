#include <hal/types.h>
#include <hal/priorities.h>
#include <hal/board.h>
#include <hal/card.h>
#include <hal/uart.h>
#include <hal/log.h>
#include <hal/fatfs/ff.h>
#include <stdint.h>
#include <stdbool.h>
#include <ti/sysbios/BIOS.h>

/* This header is used by the ROM Code to indentify the size of bootloader
 * and the location to which it should be loaded
 */
typedef struct _ti_header_
{
  uint32_t image_size;
  uint32_t load_addr;
} ti_header;

static FIL gFileObject;

static bool loadImage(const char *pFileName, uint32_t *pEntryPoint)
{
  FRESULT fResult;
  UINT bytesRead = 0U, total = 0U;
  ti_header imageHdr;
  void *pDestAddr;
  UINT retStat = FALSE;

  fResult = f_open(&gFileObject, pFileName, FA_READ);

  if (FR_OK == fResult)
  {
    retStat = TRUE;
  }
  else
  {
    logError("Unable to open application file.");
  }

  if (TRUE == retStat)
  {
    logInfo("Copying application image from MMC/SD card to RAM");
    fResult = f_read(&gFileObject, &imageHdr, 8U,
                     &bytesRead);

    logInfo("Image Header: size = %d bytes  load = 0x%x", imageHdr.image_size, imageHdr.load_addr);

    if (FR_OK != fResult)
    {
      logError("Error reading application file.");
      retStat = FALSE;
    }

    if (8U != bytesRead)
    {
      retStat = FALSE;
    }

    pDestAddr = (void *)imageHdr.load_addr;
    *pEntryPoint = imageHdr.load_addr;
  }

  if (TRUE == retStat)
  {
    do
    {
      bytesRead = 0;
      fResult = f_read(&gFileObject, pDestAddr, 512,
                       &bytesRead);
      if (fResult != FR_OK)
      {
        logError("Error reading application file.");
        retStat = FALSE;
      }

      if (bytesRead > 0)
      {
        pDestAddr += bytesRead;
        total += bytesRead;
      }
      else
      {
        retStat = FALSE;
      }
    } while (TRUE == retStat);

    logInfo("Finished copying image to RAM.");
    fResult = f_close(&gFileObject);
  }

  return retStat;
}

void runImage(const char *filename)
{
  uint32_t entryAddress = 0;
  loadImage(filename, &entryAddress);

  if (entryAddress)
  {
    void (*entry)();
    entry = (void (*)(void))entryAddress;
    (*entry)();
  }
}

Void taskFxn(UArg arg0, UArg arg1)
{
  Card_init();
  Card_mount(0);

  runImage("0:/sbl.bin");
  runImage("0:/kernel.bin");

  while (1)
  {
  }
}

Int main()
{
  Board_init();
#ifdef BUILDOPT_TESTING
  Uart_init();
  Uart_enable();
  Uart_puts("Starting PBL/MLO (");
  Uart_puts(PBL_VERSION);
  Uart_puts(")\r\n");
#endif
  BIOS_start(); /* does not return */
  return 0;
}
