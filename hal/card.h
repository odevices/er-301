#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#define CARD_FRONT 1
#define CARD_REAR 0

#define CARD_MODE_NOT_CONNECTED 0
#define CARD_MODE_FATFS 1
#define CARD_MODE_RAW 2

    void Card_init();

    bool Card_mount(uint32_t drv);
    void Card_unmount(uint32_t drv);
    bool Card_isMounted(uint32_t drv);
    bool Card_format(uint32_t drv);

    bool Card_isHighCapacity(uint32_t drv);
    uint32_t Card_getVersion(uint32_t drv);
    uint32_t Card_getBusWidth(uint32_t drv);
    uint32_t Card_getTransferSpeed(uint32_t drv);
    uint32_t Card_sizeInBlocks(uint32_t drv);
    bool Card_supportsCMD23(uint32_t drv);

    bool Card_isConnected(uint32_t drv);
    bool Card_connect(uint32_t drv, uint32_t mode);
    void Card_disconnect(uint32_t drv);
    uint32_t Card_getMode(uint32_t drv);
    bool Card_isPresent(uint32_t drv);

    bool Card_readBlocks(uint32_t drv, uint8_t *buffer, uint32_t sector, uint32_t count);
    bool Card_readAlignedBlocks(uint32_t drv, uint8_t *buffer, uint32_t sector, uint32_t count);
    bool Card_writeBlocks(uint32_t drv, uint8_t *buffer, uint32_t sector, uint32_t count);

    void Card_printErrorStatus();
    void Card_test(void);

#ifdef __cplusplus
}
#endif
