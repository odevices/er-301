
#include <hal/fatfs/ff.h>
#include <hal/log.h>
#include <hal/card.h>
#include <string.h>
#include <stdio.h>

#define TEST_STRING "This is a test."
static void test_ff(void)
{
    char buffer[128];
    FIL fp;
    FRESULT result;
    int nc;

    logInfo("FatFS Test ");

    result = f_open(&fp, "0:/test_ff.txt", FA_CREATE_ALWAYS);
    logAssert(result == FR_OK);
    nc = f_puts(TEST_STRING, &fp);
    logAssert(nc == strlen(TEST_STRING));
    result = f_close(&fp);
    logAssert(result == FR_OK);

    result = f_open(&fp, "0:/test_ff.txt", FA_READ);
    logAssert(result == FR_OK);
    logAssert(f_gets(buffer, 128, &fp));
    logInfo("SD0: [%s]", buffer);
    result = f_close(&fp);
    logAssert(result == FR_OK);

    result = f_open(&fp, "1:/test_ff.txt", FA_CREATE_ALWAYS);
    logAssert(result == FR_OK);
    nc = f_puts(TEST_STRING, &fp);
    logAssert(nc == strlen(TEST_STRING));
    result = f_close(&fp);
    logAssert(result == FR_OK);

    result = f_open(&fp, "1:/test_ff.txt", FA_READ);
    logAssert(result == FR_OK);
    logAssert(f_gets(buffer, 128, &fp));
    logInfo("SD1: [%s]", buffer);
    result = f_close(&fp);
    logAssert(result == FR_OK);
}

static void test_stdio(void)
{
    char buffer[128];
    FILE *fp;
    int nc;

    logInfo("STDIO Test ");

    fp = fopen("0:/test_ff.txt", "w");
    logAssert(fp != NULL);
    nc = fputs(TEST_STRING, fp);
    logAssert(nc == strlen(TEST_STRING));
    logAssert(fclose(fp) == 0);

    fp = fopen("0:/stdio_test.txt", "r");
    logAssert(fp != NULL);
    logAssert(fgets(buffer, 128, fp));
    logInfo("SD0: [%s]", buffer);
    logAssert(fclose(fp) == 0);

    fp = fopen("1:/test_ff.txt", "w");
    logAssert(fp != NULL);
    nc = fputs(TEST_STRING, fp);
    logAssert(nc == strlen(TEST_STRING));
    logAssert(fclose(fp) == 0);

    fp = fopen("1:/stdio_test.txt", "r");
    logAssert(fp != NULL);
    logAssert(fgets(buffer, 128, fp));
    logInfo("SD1: [%s]", buffer);
    logAssert(fclose(fp) == 0);
}

void Card_test(void)
{
    logAssert(Card_mount(0));
    logAssert(Card_mount(1));
    test_ff();
    test_stdio();
    Card_unmount(0);
    Card_unmount(1);
}
