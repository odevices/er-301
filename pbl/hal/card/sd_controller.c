/*
 * sd_controller.c
 *
 *  Created on: 4 Dec 2015
 *      Author: clarkson
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <hal/log.h>
#include <hal/priorities.h>
#include <hal/constants.h>
#include <ti/am335x/mmcsd_lld.h>
#include <ti/am335x/csl_error.h>
#include <ti/am335x/hw_types.h>
#include <ti/sysbios/family/arm/a8/intcps/Hwi.h>
#include <ti/sysbios/knl/Event.h>
#include "sd_controller.h"

/* Interrupt masks for IE and ISE register */
#define CC_EN (1 << 0)
#define TC_EN (1 << 1)
#define BWR_EN (1 << 4)
#define BRR_EN (1 << 5)
#define ERR_EN (1 << 15)
#define CTO_EN (1 << 16)
#define CCRC_EN (1 << 17)
#define CEB_EN (1 << 18)
#define CIE_EN (1 << 19)
#define DTO_EN (1 << 20)
#define DCRC_EN (1 << 21)
#define DEB_EN (1 << 22)
#define ACE_EN (1 << 24)
#define CERR_EN (1 << 28)
#define BADA_EN (1 << 29)

#define INT_EN_MASK (BADA_EN | CERR_EN | ACE_EN | DEB_EN | DCRC_EN | \
                     DTO_EN | CIE_EN | CEB_EN | CCRC_EN | CTO_EN |   \
                     BRR_EN | BWR_EN | TC_EN | CC_EN)

static void hwiOnInterrupt(UArg arg)
{
    uint32_t errStatus;
    volatile uint32_t status = 0U;
    volatile uint32_t intrMask = 0U;
    sd_controller_t *this = NULL;

    /* Input parameter validation */
    logAssert(arg != NULL);
    this = (sd_controller_t *)arg;

    //if(this->xferNumBlocks>1)
    //	Breakpoint();

    status = HSMMCSDIntrStatus(this->baseAddr);
    intrMask = HSMMCSDIntrGet(this->baseAddr);
    HSMMCSDIntrClear(this->baseAddr, status);
    this->status = status;

    /* Command execution is complete */
    if (status & HS_MMCSD_INTR_MASK_CMDCOMP)
    {
        HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_CMDCOMP);
        this->cmdComp = 1;

        /* Indicate command complete */
        if (intrMask & HS_MMCSD_INTR_MASK_CMDCOMP)
        {
            Event_post(this->hEvents, EVENT_CMD_COMPLETE);
        }
    }

    /* Error occurred in execution of command */
    if (status & HS_MMCSD_INTR_MASK_ERR)
    {
        errStatus = status & 0xFFFF0000;

        if (errStatus & HS_MMCSD_INTR_MASK_CMDTIMEOUT)
        {
            HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_CMDTIMEOUT);
            this->cmdTimeout = 1;

            /* Indicate command complete */
            if (intrMask & HS_MMCSD_INTR_MASK_CMDTIMEOUT)
            {
                Event_post(this->hEvents, EVENT_CMD_COMPLETE);
            }
        }
    }

    /* Read data received from card */
    if (status & HS_MMCSD_INTR_MASK_BUFRDRDY)
    {
        HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_BUFRDRDY);

        if (this->xferBuffer != NULL)
        {
            HSMMCSDGetData(this->baseAddr,
                           this->xferBuffer + this->xferBytesTransferred,
                           this->xferBlockSize);
            this->xferBytesTransferred += this->xferBlockSize;
        }
    }

    /* Write data received from card */
    if (status & HS_MMCSD_INTR_MASK_BUFWRRDY)
    {
        HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_BUFWRRDY);

        if (this->xferBuffer != NULL)
        {
            HSMMCSDSetData(this->baseAddr,
                           this->xferBuffer + this->xferBytesTransferred,
                           this->xferBlockSize);
            this->xferBytesTransferred += this->xferBlockSize;
        }
    }

    /* Error occurred in data transfer */
    if (status & HS_MMCSD_INTR_MASK_DATATIMEOUT)
    {
        HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_DATATIMEOUT);
        this->xferTimeout = 1;

        /* Indicate transfer complete */
        if (intrMask & HS_MMCSD_INTR_MASK_DATATIMEOUT)
        {
            Event_post(this->hEvents, EVENT_XFER_COMPLETE);
        }
    }

    /* Data transfer is complete */
    if (status & HS_MMCSD_INTR_MASK_TRNFCOMP)
    {
        HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_TRNFCOMP);

        this->xferComp = 1;

        /* Indicate transfer complete */
        if (intrMask & HS_MMCSD_INTR_MASK_TRNFCOMP)
        {
            Event_post(this->hEvents, EVENT_XFER_COMPLETE);
        }
    }

    /* Card insertion detected */
    if (status & HS_MMCSD_INTR_MASK_CARDINS)
    {
        HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_CARDINS);

        /*if (NULL != this->cardDetectCallback)
         {
         this->cardDetectCallback((MMCSD_Config *)arg, 1U);
         }*/
    }

    /* Card removal detected */
    if (status & HS_MMCSD_INTR_MASK_CARDREM)
    {
        HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_CARDREM);

        /* Indicate transfer complete */
        /*if (NULL != this->cardDetectCallback)
         {
         this->cardDetectCallback((MMCSD_Config *)arg, 0U);
         }*/
    }

    return;
}

void sd_controller_init(sd_controller_t *this, uint32_t baseAddr,
                        uint32_t intNum)
{
    memset(this, 0, sizeof(sd_controller_t));
    this->baseAddr = baseAddr;
    this->intNum = intNum;

    this->hEvents = Event_create(NULL, NULL);

    {
        Hwi_Params params;
        Hwi_Params_init(&params);
        params.priority = HWI_PRIORITY_SD;
        params.arg = (UArg)this;
        this->hHwi = Hwi_create(this->intNum, hwiOnInterrupt, &params, NULL);
    }
}

int32_t sd_controller_open(sd_controller_t *this)
{
    uint8_t dataBuffer[64U] __attribute__((aligned(CACHELINE_SIZE_MAX)));
    int32_t ret = SD_OK;
    uint32_t retry = 0xFFFFU;
    sd_transaction_t transaction;
    int32_t status = CSL_ESYS_FAIL;
    hsMmcsdSysCfg_t sysCfg = {HS_MMCSD_CLK_ACT_ICLK_FCLK_OFF,
                              HS_MMCSD_STANDBY_MODE_FORCE,
                              HS_MMCSD_IDLE_MODE_FORCE, FALSE, TRUE};

    if (SD_OK == ret)
    {
        /*
         * Refer to the MMC Host and Bus configuration steps in TRM
         * controller Reset
         */
        status = HSMMCSDSoftReset(this->baseAddr);

        if (CSL_SOK != status)
        {
#ifdef LOG_EN
            MMCSD_drv_log(
                "MMC:(%p) HS MMC/SD Reset failed\n", this->baseAddr);
#endif
            ret = SD_FAIL;
        }
        else
        {
            ret = SD_OK;
        }
    }

    if (SD_OK == ret)
    {
        /* Lines Reset */
        HSMMCSDLinesReset(this->baseAddr, HS_MMCSD_RESET_LINE_MASK_ALL);

        /* Set supported voltage list */
        HSMMCSDSetSupportedVoltage(
            this->baseAddr,
            (HS_MMCSD_SUPP_VOLT_1P8 | HS_MMCSD_SUPP_VOLT_3P0));

        HSMMCSDSystemConfig(this->baseAddr, &sysCfg);

        // set DMA assertion type
        HW_WR_FIELD32((this->baseAddr + CSL_MMCHS_CON), CSL_MMCHS_CON_SDMA_LNE,
                      CSL_MMCHS_CON_SDMA_LNE_LATEDEASSERT);

        /* Set the bus width */
        HSMMCSDSetBusWidth(this->baseAddr, HS_MMCSD_BUS_WIDTH_1BIT);

        /* Set the bus voltage */
        HSMMCSDSetBusVolt(this->baseAddr, HS_MMCSD_BUS_VOLT_3P0);

        /* Bus power on */
        status = HSMMCSDBusPowerOnCtrl(this->baseAddr, HS_MMCSD_PWR_CTRL_ON);

        if (CSL_SOK != status)
        {
#ifdef LOG_EN
            MMCSD_drv_log(
                "MMC:(%p) HS MMC/SD Power on failed\n", this->baseAddr);
#endif
            ret = SD_FAIL;
        }
    }

    if (SD_OK == ret)
    {
        /* Set the initialization frequency */
        status = HSMMCSDSetBusFreq(this->baseAddr, SD_INPUT_FREQ, SD_SETUP_FREQ,
                                   FALSE);

        if (CSL_SOK != status)
        {
#ifdef LOG_EN
            MMCSD_drv_log(
                "MMC:(%p) HS MMC/SD Bus Frequency set failed\n", this->baseAddr);
#endif
            ret = SD_FAIL;
        }

        if (SD_OK == ret)
        {
            HSMMCSDInitStreamSend(this->baseAddr);
        }
    }

#if 0
    if(SD_OK == ret)
    {
        /* CMD0 - reset card */
        transaction.cmd = MMCSD_CMD(0U);
        transaction.flags = MMCSD_CMDRSP_NONE;
        transaction.arg = 0U;
        ret = sd_controller_xfer(this, &transaction);
    }

    if(SD_OK == ret)
    {
        /* APP cmd should be preceeded by a CMD55 */
        transaction.cmd = MMCSD_CMD(55U);
        transaction.flags = 0U;
        transaction.arg = this->rca << 16U;
        ret = sd_controller_xfer(this, &transaction);
    }

    if(SD_OK == ret)
    {
        /*
         * Card type can be found by sending CMD55. If the card responds,
         * it is a SD card. Else, we assume it is a MMC Card
         */
        transaction.cmd = MMCSD_CMD(55U);
        transaction.flags = 0U;
        transaction.arg = 0U;
        ret = sd_controller_xfer(this, &transaction);
    }
#endif

    /* SD Card */
    if (SD_OK == ret)
    {

        /* CMD0 - reset card */
        {
            transaction.cmd = MMCSD_CMD(0U);
            transaction.flags = MMCSD_CMDRSP_NONE;
            transaction.arg = 0U;
            ret = sd_controller_xfer(this, &transaction);
        }

        if (SD_OK == ret)
        {
            /* CMD8 - send oper voltage */
            transaction.cmd = MMCSD_CMD(8U);
            transaction.flags = 0U;
            transaction.arg = (MMCSD_CHECK_PATTERN | MMCSD_VOLT_2P7_3P6);
            ret = sd_controller_xfer(this, &transaction);
        }

        if (SD_OK != ret)
        {
            /*
             * If the cmd fails, it can be due to version < 2.0, since
             * we are currently supporting high voltage cards only
             */
        }

        if (SD_OK == ret)
        {
            /* Poll until we get the card status (BIT31 of OCR) is powered up */
            do
            {
                /* APP cmd should be preceeded by a CMD55 */
                transaction.cmd = MMCSD_CMD(55U);
                transaction.flags = 0U;
                transaction.arg = 0;
                ret = sd_controller_xfer(this, &transaction);

                if (SD_OK == ret)
                {
                    transaction.cmd = MMCSD_CMD(41U);
                    transaction.flags = 0U;
                    transaction.arg = MMCSD_OCR_HIGH_CAPACITY | MMCSD_OCR_VDD_WILDCARD;
                    sd_controller_xfer(this, &transaction);
                }
                else
                {
                    ret = SD_FAIL;
                    break;
                }
            } while (!(transaction.response[0U] & ((uint32_t)BIT(31U))) && retry--);

            if (0U == retry)
            {
                /* No point in continuing */
                ret = SD_FAIL;
            }
        }

        if (SD_OK == ret)
        {
            this->ocr = transaction.response[0U];

            this->highCap = (this->ocr & MMCSD_OCR_HIGH_CAPACITY) ? 1U : 0U;

            /* Send CMD2, to get the card identification register */
            transaction.cmd = MMCSD_CMD(2U);
            transaction.flags = MMCSD_CMDRSP_136BITS;
            transaction.arg = 0U;

            ret = sd_controller_xfer(this, &transaction);

            memcpy(this->cid, transaction.response, 16U);
        }

        if (SD_OK == ret)
        {
            /* Send CMD3, to get the card relative address */
            transaction.cmd = MMCSD_CMD(3U);
            transaction.flags = 0U;
            transaction.arg = 0U;

            ret = sd_controller_xfer(this, &transaction);

            this->rca = MMCSD_RCA_ADDR(transaction.response[0U]);
        }

        if (SD_OK == ret)
        {
            /* Send CMD9, to get the card specific data (CSD) */
            transaction.cmd = MMCSD_CMD(9U);
            transaction.flags = MMCSD_CMDRSP_136BITS;
            transaction.arg = this->rca << 16U;

            ret = sd_controller_xfer(this, &transaction);

            memcpy(this->csd, transaction.response, 16U);
        }

        if (SD_OK == ret)
        {
            if (MMCSD_CARD_CMMCSD_VERSION(this))
            {
                // SDHC and SDXC
                this->tranSpeed = MMCSD_CARD1_TRANSPEED(this);
                this->blockSize = 1U << (MMCSD_CARD1_RDBLKLEN(this));
                this->size = MMCSD_CARD1_SIZE(this);
                this->blockCount = this->size * (1024U / this->blockSize);
            }
            else
            {
                // SDSC
                this->tranSpeed = MMCSD_CARD0_TRANSPEED(this);
                this->blockSize = 1U << (MMCSD_CARD0_RDBLKLEN(this));
                this->blockCount = MMCSD_CARD0_NUMBLK(this);
                this->size = MMCSD_CARD0_SIZE(this);
            }

            /* Set data block length to 512 (for byte addressing cards) */
            if (!(this->highCap))
            {
                transaction.cmd = MMCSD_CMD(16U);
                transaction.flags = MMCSD_CMDRSP_NONE;
                transaction.arg = 512U;
                ret = sd_controller_xfer(this, &transaction);

                if (SD_OK == ret)
                {
                    this->blockSize = 512U;
                }
            }
        }

        if (SD_OK == ret)
        {
            /* Select the card */
            transaction.cmd = MMCSD_CMD(7U);
            transaction.flags = MMCSD_CMDRSP_BUSY;
            transaction.arg = this->rca << 16U;

            ret = sd_controller_xfer(this, &transaction);
        }

        if (SD_OK == ret)
        {
            /*
             * Send ACMD51, to get the SD Configuration register details.
             * Note, this needs data transfer (on data lines).
             */
            transaction.cmd = MMCSD_CMD(55U);
            transaction.flags = 0U;
            transaction.arg = this->rca << 16U;

            ret = sd_controller_xfer(this, &transaction);
        }

        if (SD_OK == ret)
        {
            transaction.cmd = MMCSD_CMD(51U);
            transaction.flags = MMCSD_CMDRSP_READ | MMCSD_CMDRSP_DATA;
            transaction.arg = this->rca << 16U;
            transaction.blockCount = 1U;
            transaction.blockSize = 8U;
            transaction.dataBuf = dataBuffer;

            ret = sd_controller_xfer(this, &transaction);
        }

        if (SD_OK == ret)
        {
            /*
             this->scr[0U] = (dataBuffer[3U] << 24U) | (dataBuffer[2U] << 16U) | \
                               (dataBuffer[1U] << 8U) | (dataBuffer[0U]);
             this->scr[1U] = (dataBuffer[7U] << 24U) | (dataBuffer[6U] << 16U) | \
                                       (dataBuffer[5U] << 8U) | (dataBuffer[4U]);
             */
            this->scr[0U] = (dataBuffer[0U] << 24U) | (dataBuffer[1U] << 16U) | (dataBuffer[2U] << 8U) | (dataBuffer[3U]);
            this->scr[1U] = (dataBuffer[4U] << 24U) | (dataBuffer[5U] << 16U) | (dataBuffer[6U] << 8U) | (dataBuffer[7U]);

            uint32_t spec, spec3, spec4;
            spec = MMCSD_CARD_VERSION(this);
            spec3 = (this->scr[0U] & (1 << 15)) >> 15; // bit 47
            spec4 = (this->scr[0U] & (1 << 10)) >> 10; // bit 42
            if (spec == 0 || spec == 1)
            {
                this->sdVer = 1;
            }
            else
            {
                if (spec3 == 0)
                {
                    this->sdVer = 2;
                }
                else if (spec4 == 0)
                {
                    this->sdVer = 3;
                }
                else
                {
                    this->sdVer = 4;
                }
            }
            this->busWidth = MMCSD_CARD_BUSWIDTH(this);
            this->supportsCMD23 = (this->scr[0U] & (1 << 1)) >> 1; // bit 33
        }

        if (SD_OK == ret)
        {
            /* APP cmd should be preceeded by a CMD55 */
            transaction.cmd = MMCSD_CMD(55U);
            transaction.flags = 0U;
            transaction.arg = this->rca << 16U;
            ret = sd_controller_xfer(this, &transaction);
        }

        if (SD_OK == ret)
        {
            /* ACMD6 - set bus width */
            transaction.cmd = MMCSD_CMD(6U);
            transaction.arg = MMCSD_BUS_WIDTH_4BIT;
            transaction.flags = 0U;

            transaction.arg = transaction.arg >> 1U; // why?
            ret = sd_controller_xfer(this, &transaction);

            if (SD_OK == ret)
            {
                HSMMCSDSetBusWidth(this->baseAddr, HS_MMCSD_BUS_WIDTH_4BIT);
            }
        }

        if (SD_OK == ret)
        {
            /* CMD6 - switch function command */
            transaction.cmd = MMCSD_CMD(6U);
            // select bus speed mode as High Speed
            transaction.arg = ((MMCSD_SWITCH_MODE & MMCSD_CMD6_GRP1_SEL) | (MMCSD_CMD6_GRP1_HS));
            transaction.flags = MMCSD_CMDRSP_READ | MMCSD_CMDRSP_DATA;
            transaction.blockCount = 1U;
            transaction.blockSize = 64U;
            transaction.dataBuf = dataBuffer;
            ret = sd_controller_xfer(this, &transaction);
        }

        if (SD_OK == ret)
        {
            if ((dataBuffer[16U] & 0xFU) == MMCSD_CMD6_GRP1_HS)
            {
                this->tranSpeed = MMCSD_TRANSPEED_50MBPS;
            }
            else
            {
                this->tranSpeed = MMCSD_TRANSPEED_25MBPS;
            }

            if (MMCSD_TRANSPEED_50MBPS == this->tranSpeed)
            {
                if (CSL_SOK == HSMMCSDSetBusFreq(this->baseAddr, SD_INPUT_FREQ,
                                                 50000000U, 0U))
                {
                    ret = SD_OK;
                }
            }
            else
            {
                if (CSL_SOK == HSMMCSDSetBusFreq(this->baseAddr, SD_INPUT_FREQ,
                                                 25000000U, 0U))
                {
                    ret = SD_OK;
                }
            }
        }
    }

    if (SD_OK != ret)
    {
        this->isOpen = false;
    }
    else
    {
        this->isOpen = true;
    }

    return (ret);
}

int32_t sd_controller_xfer(sd_controller_t *this,
                           sd_transaction_t *transaction)
{
    int32_t ret = SD_OK;
    hsMmcsdCmdObj_t cmdObj = {{0U, 0U, 0U, 0U}, 0U, 0U, 0U, 0U};

    if (SD_OK == ret)
    {
        /* Configure the command type to be executed from the command flags */
        if (transaction->flags & MMCSD_CMDRSP_STOP)
        {
            cmdObj.cmd.cmdType = HS_MMCSD_CMD_TYPE_BUS_SUSPEND;
        }
        else if (transaction->flags & MMCSD_CMDRSP_FS)
        {
            cmdObj.cmd.cmdType = HS_MMCSD_CMD_TYPE_FUNC_SEL;
        }
        else if (transaction->flags & MMCSD_CMDRSP_ABORT)
        {
            cmdObj.cmd.cmdType = HS_MMCSD_CMD_TYPE_IO_ABORT;
        }

        /* Configure the response type from the command flags */
        if (transaction->flags & MMCSD_CMDRSP_NONE)
        {
            cmdObj.cmd.rspType = HS_MMCSD_RSP_TYPE_NONE;
        }
        else if (transaction->flags & MMCSD_CMDRSP_136BITS)
        {
            cmdObj.cmd.rspType = HS_MMCSD_RSP_TYPE_LEN_136;
        }
        else if (transaction->flags & MMCSD_CMDRSP_BUSY)
        {
            cmdObj.cmd.rspType = HS_MMCSD_RSP_TYPE_LEN_48_BUSY;
        }
        else
        {
            cmdObj.cmd.rspType = HS_MMCSD_RSP_TYPE_LEN_48;
        }

        /* Configure the transfer type */
        cmdObj.enableData =
            (transaction->flags & MMCSD_CMDRSP_DATA) ? TRUE : FALSE;

        if (cmdObj.enableData)
        {
            this->xferBuffer = transaction->dataBuf;
            this->xferNumBlocks = transaction->blockCount;
            this->xferBlockSize = transaction->blockSize;
            this->xferBytesTransferred = 0U;

            cmdObj.numBlks =
                (TRUE == cmdObj.enableData) ? this->xferNumBlocks : 0U;

            cmdObj.cmd.xferType =
                (transaction->flags & MMCSD_CMDRSP_READ) ? HS_MMCSD_XFER_TYPE_RX : HS_MMCSD_XFER_TYPE_TX;

            if (HS_MMCSD_XFER_TYPE_RX == cmdObj.cmd.xferType)
            {
                /* Configure the transfer for read operation */
                HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_BUFRDRDY);
                HSMMCSDIntrStatusEnable(this->baseAddr,
                                        HS_MMCSD_INTR_MASK_BUFRDRDY);
                HSMMCSDIntrStatusDisable(this->baseAddr,
                                         HS_MMCSD_INTR_MASK_BUFWRRDY);
                HSMMCSDIntrEnable(this->baseAddr, HS_MMCSD_INTR_MASK_BUFRDRDY);
                HSMMCSDIntrDisable(this->baseAddr, HS_MMCSD_INTR_MASK_BUFWRRDY);
            }
            else
            {
                /* Configure the transfer for write operation */
                HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_BUFWRRDY);
                HSMMCSDIntrStatusEnable(this->baseAddr,
                                        HS_MMCSD_INTR_MASK_BUFWRRDY);
                HSMMCSDIntrStatusDisable(this->baseAddr,
                                         HS_MMCSD_INTR_MASK_BUFRDRDY);
                HSMMCSDIntrEnable(this->baseAddr, HS_MMCSD_INTR_MASK_BUFWRRDY);
                HSMMCSDIntrDisable(this->baseAddr, HS_MMCSD_INTR_MASK_BUFRDRDY);
            }

            HSMMCSDIntrClear(this->baseAddr, HS_MMCSD_INTR_MASK_TRNFCOMP);

            HSMMCSDSetBlkLength(this->baseAddr, transaction->blockSize);
            HSMMCSDSetDataTimeout(this->baseAddr, 27U);

            cmdObj.cmd.cmdId = transaction->cmd;
            cmdObj.cmdArg = transaction->arg;
            cmdObj.enableDma = 0U;

            HSMMCSDCommandSend(this->baseAddr, &cmdObj);

            HSMMCSDIntrStatusEnable(
                this->baseAddr,
                (HS_MMCSD_INTR_MASK_CMDCOMP | HS_MMCSD_INTR_MASK_CMDTIMEOUT
                 //| HS_MMCSD_INTR_MASK_DATATIMEOUT
                 | HS_MMCSD_INTR_MASK_TRNFCOMP));

            HSMMCSDIntrEnable(
                this->baseAddr,
                (HS_MMCSD_INTR_MASK_CMDCOMP | HS_MMCSD_INTR_MASK_CMDTIMEOUT
                 //| HS_MMCSD_INTR_MASK_DATATIMEOUT
                 | HS_MMCSD_INTR_MASK_TRNFCOMP));

            /*
             * Wait for the transfer to complete here.
             */
#if 0
            Event_pend(this->hEvents, EVENT_CMD_COMPLETE, Event_Id_NONE,
                    BIOS_WAIT_FOREVER);
#else
            if (Event_pend(this->hEvents, EVENT_CMD_COMPLETE, Event_Id_NONE,
                           1000) == 0)
            {
                this->waitTimeout++;
                this->cmdTimeout = 1;
            }

#endif

            /* Command execution fail */
            if (1 == this->cmdTimeout)
            {
                ret = SD_CMD_EXECUTION_FAIL;
                this->lastXferError = ret;
                this->cmdTimeout = 0;
            }

            /* Command execution successful */
            if (1 == this->cmdComp)
            {
                ret = SD_OK;
                this->cmdComp = 0;

                /* Get response for command sent to MMC device */
                HSMMCSDGetResponse(this->baseAddr, transaction->response);
            }

            if (SD_OK == ret)
            {
                /*
                 * Wait for the transfer to complete here.
                 */
#if 0
                Event_pend(this->hEvents, EVENT_XFER_COMPLETE, Event_Id_NONE,
                        BIOS_WAIT_FOREVER);
#else
                if (Event_pend(this->hEvents, EVENT_XFER_COMPLETE,
                               Event_Id_NONE,
                               1000) == 0)
                {
                    this->waitTimeout++;
                    this->xferTimeout = 1;
                }
#endif

                /* Data transfer timed out */
                if (1 == this->xferTimeout)
                {
                    ret = SD_DATA_TRANSFER_TIMEOUT;
                    this->lastXferError = ret;
                    this->xferTimeout = 0;
                }

                /* Data transfer successful */
                if (1 == this->xferComp)
                {
                    ret = SD_OK;
                    this->xferComp = 0;
                }
            }
        }
        else
        {
            // Command-only (no data)

            cmdObj.cmd.cmdId = transaction->cmd;
            cmdObj.cmdArg = transaction->arg;
            cmdObj.enableDma = 0;

            HSMMCSDIntrStatusEnable(
                this->baseAddr,
                (HS_MMCSD_INTR_MASK_CMDCOMP | HS_MMCSD_INTR_MASK_CMDTIMEOUT));

            HSMMCSDIntrEnable(
                this->baseAddr,
                (HS_MMCSD_INTR_MASK_CMDCOMP | HS_MMCSD_INTR_MASK_CMDTIMEOUT));

            HSMMCSDCommandSend(this->baseAddr, &cmdObj);
            /*
             * Wait for the transfer to complete here.
             */
#if 0
            Event_pend(this->hEvents, EVENT_CMD_COMPLETE, Event_Id_NONE,
                    BIOS_WAIT_FOREVER);
#else
            if (Event_pend(this->hEvents, EVENT_CMD_COMPLETE, Event_Id_NONE,
                           1000) == 0)
            {
                this->waitTimeout++;
                this->cmdTimeout = 1;
            }

#endif

            /* Command execution successful */
            if (1 == this->cmdComp)
            {
                ret = SD_OK;
                this->cmdComp = 0;
            }

            /* Command execution fail */
            if (1 == this->cmdTimeout)
            {
                ret = SD_CMD_EXECUTION_FAIL;
                this->lastXferError = ret;
                this->cmdTimeout = 0;
            }

            /* Get response for command sent to MMC device */
            HSMMCSDGetResponse(this->baseAddr, transaction->response);
        }
    }

    /* Return the transaction status */
    return (ret);
}

void sd_controller_close(sd_controller_t *this)
{
    if (this->isOpen)
    {
        this->isOpen = false;
    }
}
