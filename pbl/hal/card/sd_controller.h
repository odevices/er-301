/*
 * sd_controller.h
 *
 *  Created on: 4 Dec 2015
 *      Author: clarkson
 */

#ifndef ER301_SD_SD_CONTROLLER_H_
#define ER301_SD_SD_CONTROLLER_H_

#define SD_INPUT_FREQ (96000000U)
#define SD_SETUP_FREQ (400000U)

#define SD_OK 1
#define SD_FAIL -1
#define SD_CMD_EXECUTION_FAIL -2
#define SD_DATA_TRANSFER_TIMEOUT -3

/** \brief Bit mask. */
#define BIT(x) (1U << x)

/**
 * SD Card information structure
 */

/** \brief SD Commands enumeration. */
#define MMCSD_CMD(x)   (x)

#define MMCSD_STOP_TRANSMISSION		MMCSD_CMD(12)
#define MMCSD_SET_BLOCKLEN			MMCSD_CMD(16)
#define MMCSD_READ_SINGLE_BLOCK		MMCSD_CMD(17)
#define MMCSD_READ_MULTIPLE_BLOCK	MMCSD_CMD(18)
#define MMCSD_WRITE_SINGLE_BLOCK	MMCSD_CMD(24)
#define MMCSD_WRITE_MULTIPLE_BLOCK	MMCSD_CMD(25)
#define MMCSD_SET_BLOCK_COUNT	    MMCSD_CMD(23)
#define MMCSD_APP_CMD				MMCSD_CMD(55)

/**
 * Command/Response flags for notifying some information to controller
 */

/** \brief To indicate no response. */
#define MMCSD_CMDRSP_NONE           BIT(0U)

/** \brief Response to indicate stop condition. */
#define MMCSD_CMDRSP_STOP           BIT(1U)

/** \brief Response to indicate stop condition. */
#define MMCSD_CMDRSP_FS             BIT(2U)

/** \brief Response to indicate abort condition. */
#define MMCSD_CMDRSP_ABORT          BIT(3U)

/** \brief Response to indicate bust state. */
#define MMCSD_CMDRSP_BUSY           BIT(4U)

/** \brief Command to configure for 136 bits data width. */
#define MMCSD_CMDRSP_136BITS        BIT(5U)

/** \brief Command to configure for data or response. */
#define MMCSD_CMDRSP_DATA           BIT(6U)

/** \brief Command to configure for data read. */
#define MMCSD_CMDRSP_READ           BIT(7U)

/** \brief Command to configure for data write. */
#define MMCSD_CMDRSP_WRITE          BIT(8U)

/** \brief SD voltage enumeration as per VHS field of the interface command. */
#define MMCSD_VOLT_2P7_3P6          (0x000100U)

/**
 * SD OCR register definitions.
 */

/** \brief High capacity card type. */
#define MMCSD_OCR_HIGH_CAPACITY     BIT(30U)

/**
 * Voltage configurations.
 */

/** \brief Configure for 2.7V to 2.8V VDD level. */
#define MMCSD_OCR_VDD_2P7_2P8       BIT(15U)

/** \brief Configure for 2.8V to 2.9V VDD level. */
#define MMCSD_OCR_VDD_2P8_2P9       BIT(16U)

/** \brief Configure for 2.9V to 3.0V VDD level. */
#define MMCSD_OCR_VDD_2P9_3P0       BIT(17U)

/** \brief Configure for 3.0V to 3.1V VDD level. */
#define MMCSD_OCR_VDD_3P0_3P1       BIT(18U)

/** \brief Configure for 3.1V to 3.2V VDD level. */
#define MMCSD_OCR_VDD_3P1_3P2       BIT(19U)

/** \brief Configure for 3.2V to 3.3V VDD level. */
#define MMCSD_OCR_VDD_3P2_3P3       BIT(20U)

/** \brief Configure for 3.3V to 3.4V VDD level. */
#define MMCSD_OCR_VDD_3P3_3P4       BIT(21U)

/** \brief Configure for 3.4V to 3.5V VDD level. */
#define MMCSD_OCR_VDD_3P4_3P5       BIT(22U)

/** \brief Configure for 3.5V to 3.6V VDD level. */
#define MMCSD_OCR_VDD_3P5_3P6       BIT(23U)

/** \brief Wild card to configure for VDD level. */
#define MMCSD_OCR_VDD_WILDCARD      (0x1FFU << 15U)

/**
 * SD CSD register definitions.
 */

/** \brief Card bus frequency configuration for 25 Mbps. */
#define MMCSD_TRANSPEED_25MBPS      (0x32U)

/** \brief Card bus frequency configuration for 50 Mbps. */
#define MMCSD_TRANSPEED_50MBPS      (0x5AU)

/** \brief Gives the card version. */
#define MMCSD_CARD_CMMCSD_VERSION(crd) \
    (((crd)->csd[3U] & 0xC0000000) >> 30U)

/** \brief Extract the size of device for SD version 0. */
#define MMCSD_CSD0_DEV_SIZE(csd3, csd2, csd1, csd0) \
    (((csd2 & 0x000003FFU) << 2U) | ((csd1 & 0xC0000000U) >> 30U))

/** \brief TBD for SD version 0. */
#define MMCSD_CSD0_MULT(csd3, csd2, csd1, csd0) \
    ((csd1 & 0x00038000U) >> 15U)

/** \brief Extract the read block length for SD version 0. */
#define MMCSD_CSD0_RDBLKLEN(csd3, csd2, csd1, csd0) \
    ((csd2 & 0x000F0000U) >> 16U)

/** \brief Extract the card transfer speed for SD version 0. */
#define MMCSD_CSD0_TRANSPEED(csd3, csd2, csd1, csd0) \
    ((csd3 & 0x000000FFU) >> 0U)

/** \brief Extracts the size of card for SD version 0. */
#define MMCSD_CARD0_DEV_SIZE(crd) \
    MMCSD_CSD0_DEV_SIZE((crd)->csd[3U], (crd)->csd[2U], \
    (crd)->csd[1U], (crd)->csd[0U])

/** \brief TBD for SD version 0. */
#define MMCSD_CARD0_MULT(crd) \
    MMCSD_CSD0_MULT((crd)->csd[3U], (crd)->csd[2U], \
    (crd)->csd[1U], (crd)->csd[0U])

/** \brief Gives the card block length for SD version 0. */
#define MMCSD_CARD0_RDBLKLEN(crd) \
    MMCSD_CSD0_RDBLKLEN((crd)->csd[3U], (crd)->csd[2U], \
    (crd)->csd[1U], (crd)->csd[0U])

/** \brief Gives the card transfer speed for SD version 0. */
#define MMCSD_CARD0_TRANSPEED(crd) \
    MMCSD_CSD0_TRANSPEED((crd)->csd[3U], (crd)->csd[2U], \
    (crd)->csd[1U], (crd)->csd[0U])

/** \brief Gives number of blocks on card for SD version 0. */
#define MMCSD_CARD0_NUMBLK(crd) \
    ((MMCSD_CARD0_DEV_SIZE((crd)) + 1U) * \
    (1U << (MMCSD_CARD0_MULT((crd)) + 2U)))

/** \brief Gives the size of card for SD version 0. */
#define MMCSD_CARD0_SIZE(crd) ((MMCSD_CARD0_NUMBLK((crd))) * \
    (1U << (MMCSD_CARD0_RDBLKLEN(crd))))

/** \brief Extracts the size of card for SD version 1. */
#define MMCSD_CSD1_DEV_SIZE(csd3, csd2, csd1, csd0) \
    (((csd2 & 0x0000003FU) << 16U) | ((csd1 & 0xFFFF0000U) >> 16U))

/** \brief Extracts the card block length for SD version 1. */
#define MMCSD_CSD1_RDBLKLEN(csd3, csd2, csd1, csd0) \
    ((csd2 & 0x000F0000U) >> 16U)

/** \brief Extracts the card transfer speed for SD version 1. */
#define MMCSD_CSD1_TRANSPEED(csd3, csd2, csd1, csd0) \
    ((csd3 & 0x000000FFU) >> 0U)

/** \brief Gives the size of card for SD version 1. */
#define MMCSD_CARD1_DEV_SIZE(crd) \
    MMCSD_CSD1_DEV_SIZE((crd)->csd[3U], (crd)->csd[2U], \
    (crd)->csd[1U], (crd)->csd[0U])

/** \brief Reads the card block length for SD version 1. */
#define MMCSD_CARD1_RDBLKLEN(crd) \
    MMCSD_CSD1_RDBLKLEN((crd)->csd[3U], (crd)->csd[2U], \
    (crd)->csd[1U], (crd)->csd[0U])

/** \brief Reads the card transfer speed for SD version 1. */
#define MMCSD_CARD1_TRANSPEED(crd) \
    MMCSD_CSD1_TRANSPEED((crd)->csd[3U], (crd)->csd[2U], \
    (crd)->csd[1U], (crd)->csd[0U])

/** \brief Gives the size of card for SD version 1. (in KiB) */
#define MMCSD_CARD1_SIZE(crd) ((MMCSD_CARD1_DEV_SIZE((crd)) + 1U) * 512U)

/**
 * Check RCA/status.
 */

/** \brief Command relative address. */
#define MMCSD_RCA_ADDR(rca)             ((rca & 0xFFFF0000U) >> 16U)

/** \brief TBD. */
#define MMCSD_RCA_STAT(rca)             (rca & 0x0xFFFFU)

/** \brief Check pattern that can be used for card response validation. */
#define MMCSD_CHECK_PATTERN             (0xAAU)

/**
 * SD SCR related macros.
 */

/** \brief Card version 0. */
#define MMCSD_VERSION_1P0               (0U)

/** \brief Card version 1. */
#define MMCSD_VERSION_1P1               (1U)

/** \brief Card version 2. */
#define MMCSD_VERSION_2P0               (2U)

/**
 * Helper macros.
 * Note card registers are big endian.
 */

/** \brief Reads card version. */
//#define MMCSD_CARD_VERSION(sdcard)      ((sdcard)->scr[0U] & 0xFU)
#define MMCSD_CARD_VERSION(sdcard)      (((sdcard)->scr[0U] & (0xF << 24))>>24)

/** \brief Reads card bus width. */
//#define MMCSD_CARD_BUSWIDTH(sdcard) (((sdcard)->scr[0U] & 0xF00U) >> 8U)
#define MMCSD_CARD_BUSWIDTH(sdcard) (((sdcard)->scr[0U] & (0xF << 16)) >> 16)

/** \brief Check for bus width. Give below values
 *         - MMCSD_BUS_WIDTH_1BIT for 1-bit.
 *         - MMCSD_BUS_WIDTH_4BIT for 4-bit.
 *         - 0xFFU                    for invalid bus width.
 */
#define MMCSD_GET_CARD_BUSWIDTH(sdcard) \
    ((((sdcard.busWidth) & 0x0FU) == 0x01) ? \
    0x1 : ((((sdcard).busWidth & 0x04U) == 0x04U) ? 0x04U : 0xFFU))

/** \brief Check for bus width. Give below values
 *         - 50U for 50 MHz.
 *         - 25U for 25 Mhz.
 *         - 0U  for invalid bus width.
 */
#define MMCSD_GET_CARD_FRE(sdcard) (((sdcard.tranSpeed) == 0x5AU) ? 50U : \
    (((sdcard.tranSpeed) == 0x32U) ? 25U : 0U))

/** \brief Command argument to configure for switch mode. */
#define MMCSD_SWITCH_MODE               (0x80FFFFFFU)

/** \brief Command argument width to configure for transfer speed. */
#define MMCSD_CMD6_GRP1_SEL             (0xFFFFFFF0U)

/** \brief Command argument to configure for high speed. */
#define MMCSD_CMD6_GRP1_HS              (0x1U)

#define MMCSD_OPT_TCINTEN_SHIFT              (0x00000014U)
/**< Interrupt enable bit in OPT register for edma                            */

#define MMCSD_OPT_TCINTEN          (1U)
/**< Interrupt enable bit value in OPT register for edma                      */

#define UART_EDMA_BASE_ADDR                 (0x40000000U)
/**< EDMA_BASE_ADDR for DM814x DSP                                            */

#define MMCSD_EDMA3CC_OPT_TCC_MASK           (0x0003F000U)
/**< EDMA OPT TCC Mask value                                                  */

#define MMCSD_EDMA3CC_OPT_TCC_SHIFT          (0x0000000CU)
/**< EDMA OPT TCC Shift value                                                 */

#define UART_ERROR_COUNT            (0x00FFFFFFU)
/**< Count Value to check error in the recieved byte                          */

#define MMCSD_EDMA3CC_OPT_FIFO_WIDTH         (0xFFFFF8FFU)
/**< Set FIFO Width for edma transfer                                         */

#define MMCSD_EDMA3CC_OPT_SAM_CONST_MODE         (0x00000001U)
/**< Set SAM in Constant Addressing Mode                                      */

#define MMCSD_EDMA3CC_OPT_DAM_CONST_MODE         (0x00000002U)
/**< Set DAM in Constant Addressing Mode                                      */

#define MMCSD_EDMA3CC_OPT_SAM_INCR_MODE          (0xFFFFFFFEU)
/**< Set SAM in Increment Mode                                                */

#define MMCSD_EDMA3CC_OPT_DAM_INCR_MODE          (0xFFFFFFFDU)
/**< Set DAM in Increment Mode                                                */

#define MMCSD_EDMA3CC_OPT_SYNC_AB            (0x00000004U)
/**< It is AB-synchronized                                                    */

#define MMCSD_EDMA3CC_OPT_SYNC_MASK_VALUE            (0xFFFFFFFFU)
/**< Mask Value for Transfer Synchronization                                  */

#define MMCSD_EDMA3CC_PARAM_LINK_ADDRESS         (0xFFFFU)
/**< Set link Address                                                         */

#define MMCSD_EDMA3CC_PARAM_LINK_ADDR_MASK_VALUE         (0x0000FFFFU)
/**< link Address Mask Value                                                  */

#define MMCSD_EDMA3CC_PARAM_ACNT         (1U)
/**< aCnt Value                                                               */

#define MMCSD_EDMA3CC_SRC_BINDEX         (1U)
/**< Src BIndex Value                                                         */

#define MMCSD_EDMA3CC_DST_BINDEX         (4U)
/**< Dst BIndex Value                                                         */

#define MMCSD_EDMA3CC_OPT_SYNC_A         (0x01U)
/**< It is A-synchronized                                                     */

#define MMCSD_EDMA3CC_OPT_SYNCDIM_SHIFT          (3U)
/**< Transfer synchronization dimension Shift Value                           */

#define MMCSD_EDMA3CC_COUNT_VALUE            (uint32_t) (0xFFFFU)
/**< Count Value                                                              */

/** \brief Card bus width configuration for 1-bit mode. */
#define MMCSD_BUS_WIDTH_1BIT            (1U)

/** \brief Card bus width configuration for 4-bit mode. */
#define MMCSD_BUS_WIDTH_4BIT            (4U)

/** \brief Card bus width configuration for 8-bit mode. */
#define MMCSD_BUS_WIDTH_8BIT            (8U)

/** \brief Card bus frequency configuration for 25 Mbps. */
#define MMCSD_TRANSPEED_25MBPS      (0x32U)

/** \brief Card bus frequency configuration for 50 Mbps. */
#define MMCSD_TRANSPEED_50MBPS      (0x5AU)

typedef struct {
	uint32_t id;
	uint32_t baseAddr;
	uint32_t intNum;

	Event_Handle hEvents;
#define EVENT_CMD_COMPLETE Event_Id_00
#define EVENT_XFER_COMPLETE Event_Id_01
#define EVENT_DMA_COMPLETE Event_Id_02

	Hwi_Handle hHwi; /*! Hwi object */

	uint32_t rca; /*! Relative card address. */
	uint32_t scr[2]; /*! First value of enumeration. Can be used
	 used for validation. */
	uint32_t csd[4]; /*! Card specific data. */
	uint8_t ecsd[512]; /*! eMMC specific data. */
	uint32_t cid[4]; /*! Card identification register. */
	uint32_t ocr; /*! Operation condition register. */
	uint8_t sdVer; /*! Version of card. */
	uint8_t busWidth; /*! Bus width. */
	uint8_t tranSpeed; /*! Transfer speed. */
	uint8_t highCap; /*! Is card of high capacity. */
	uint8_t supportsCMD23;
	uint32_t blockSize; /*! Size of a block. */
	uint32_t blockCount; /*! Number of blocks. */
	uint32_t size; /*! Size of the card in KiB. */

	uint32_t cmdComp; /*! Command completion flag */
	uint32_t cmdTimeout; /*! Command timeout flag */
	uint32_t status;
	uint32_t xferComp; /*! Transfer completion flag */
	uint32_t xferTimeout; /*! Transfer timeout flag */

	uint8_t *xferBuffer; /*! Internal inc. dataBuf index */
	uint32_t xferNumBlocks; /*! Internal dec. blockCounter */
	uint32_t xferBlockSize;
	uint32_t xferBytesTransferred;

	uint32_t isOpen; /*! flag to indicate module is open */
	int32_t lastXferError;

	int waitTimeout; /*! Waiting time out */


} sd_controller_t;

typedef struct {
	uint32_t cmd; /*!< Command as per MMC device specification */
	uint32_t flags; /*!< Command flag as per MMC device specification */
	uint32_t arg; /*!< Command argument as per MMC device specification */

	void *dataBuf; /*!< buffer containing data to be read into or written */
	size_t blockSize; /*!< Number of bytes to be transferred per block */
	size_t blockCount; /*!< Number of block to be transferred */

	uint32_t response[4]; /*!< Command response per MMC device specification */
} sd_transaction_t;

void sd_controller_init(sd_controller_t * this, uint32_t baseAddr, uint32_t intNum);
int32_t sd_controller_open(sd_controller_t * this);
int32_t sd_controller_xfer(sd_controller_t * this,
		sd_transaction_t * transaction);
void sd_controller_close(sd_controller_t * this);

#endif /* ER301_SD_SD_CONTROLLER_H_ */
