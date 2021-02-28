/*
 * sd.h
 *
 *  Created on: 7 Dec 2015
 *      Author: clarkson
 */

#ifndef OS_CARD_H_
#define OS_CARD_H_

#ifdef __cplusplus
extern "C" {
#endif

void Card_init();
bool Card_mount(uint32_t drv);
void Card_unmount(uint32_t drv);
bool Card_isMounted(uint32_t drv);
uint32_t Card_sizeInBlocks(uint32_t drv);
bool Card_readBlocks(uint32_t drv, uint8_t * pData, uint32_t sector, uint32_t numBlocks);
bool Card_writeBlocks(uint32_t drv, uint8_t * pData, uint32_t sector, uint32_t numBlocks);
void Card_printErrorStatus();
bool Card_isHighCapacity(uint32_t drv);
int Card_getVersion(uint32_t drv);
int Card_getBusWidth(uint32_t drv);
int Card_getTransferSpeed(uint32_t drv);
bool Card_supportsCMD23(uint32_t drv);

#ifdef __cplusplus
}
#endif

#endif /* OS_CARD_H_ */
