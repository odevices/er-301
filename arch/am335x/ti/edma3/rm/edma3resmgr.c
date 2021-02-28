/*
 * edma3resmgr.c
 *
 * EDMA3 Controller Resource Manager Interface Implementation
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/* Resource Manager Internal Header Files */
#include <ti/edma3/rm/edma3resmgr.h>

/* Instrumentation Header File */
#ifdef EDMA3_INSTRUMENTATION_ENABLED
#include <ti/edma3/rm/edma3_log.h>
#endif

/* For assert() */
/**
 * Define NDEBUG to ignore assert().
 * NDEBUG should be defined before including assert.h header file.
 */
#include <assert.h>

/* Global Defines, need to re-compile if values are changed */
/*---------------------------------------------------------------------------*/
/**
 * \brief EDMA3 Resource Manager behaviour of clearing CC ERROR interrupts.
 *         This macro controls the driver to enable/disable clearing of error
 *         status of all channels.
 *
 *         On disabling this (with value 0x0), the channels owned by the region
 *         is cleared and its expected that some other entity is responsible for
 *         clearing the error status for channels not owned.
 *
 *         Its recomended that this flag is a positive value, to ensure that
 *         error flags are cleared for all the channels.
 */
#define EDMA3_RM_RES_CLEAR_ERROR_STATUS_FOR_ALL_CHANNELS (TRUE)

/**
 * \brief EDMA3 Resource Manager retry count to check the pending interrupts inside ISR.
 *         This macro controls the driver to check the pending interrupt for
 *         'n' number of times.
 *         Minumum value is 1.
 */
#define EDMA3_RM_COMPL_HANDLER_RETRY_COUNT (10U)

/**
 * \brief EDMA3 Resource Manager retry count to check the pending CC Error Interrupt inside ISR
 *         This macro controls the driver to check the pending CC Error
 *         interrupt for 'n' number of times.
 *         Minumum value is 1.
 */
#define EDMA3_RM_CCERR_HANDLER_RETRY_COUNT (10U)

/* Externel Variables */
/*---------------------------------------------------------------------------*/
/**
 * Maximum Resource Manager Instances supported by the EDMA3 Package.
 */
extern const uint32_t EDMA3_MAX_RM_INSTANCES;

#ifndef GENERIC
/**
 * \brief Static Configuration structure for EDMA3
 * controller, to provide Global SoC specific Information.
 *
 * This configuration info can also be provided by the user at run-time,
 * while calling EDMA3_RM_create (). If not provided at run-time,
 * this info will be taken from the config file "edma3_<PLATFORM_NAME>_cfg.c",
 * for the specified platform.
 */
extern EDMA3_RM_GblConfigParams edma3GblCfgParams[EDMA3_MAX_EDMA3_INSTANCES];

/**
 * \brief Default Static Region Specific Configuration structure for
 * EDMA3 controller, to provide region specific Information.
 */
extern EDMA3_RM_InstanceInitConfig defInstInitConfig[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_REGIONS];

#endif

/**
 * \brief Region Specific Configuration structure for
 * EDMA3 controller, to provide region specific Information.
 *
 * This configuration info can also be provided by the user at run-time,
 * while calling EDMA3_RM_open (). If not provided at run-time,
 * this info will be taken from the config file "edma3_<PLATFORM_NAME>_cfg.c",
 * for the specified platform.
 */
#ifdef BUILD_C6XDSP
extern far EDMA3_RM_InstanceInitConfig *userInitConfig;
extern far EDMA3_RM_InstanceInitConfig *ptrInitCfgArray;
#else
extern EDMA3_RM_InstanceInitConfig *userInitConfig;
extern EDMA3_RM_InstanceInitConfig *ptrInitCfgArray;
#endif

/**
 * Handles of EDMA3 Resource Manager Instances.
 *
 * Used to maintain information of the EDMA3 RM Instances
 * for each HW controller.
 * There could be a maximum of EDMA3_MAX_RM_INSTANCES instances per
 * EDMA3 HW.
 */
#ifdef BUILD_C6XDSP
extern far EDMA3_RM_Instance *resMgrInstance;
extern far EDMA3_RM_Instance *ptrRMIArray;
#else
extern EDMA3_RM_Instance *ptrRMIArray;
extern EDMA3_RM_Instance *resMgrInstance;

#endif
/** Max of DMA Channels */
uint32_t edma3_dma_ch_max_val[EDMA3_MAX_EDMA3_INSTANCES];
/** Min of Link Channels */
uint32_t edma3_link_ch_min_val[EDMA3_MAX_EDMA3_INSTANCES];
/** Max of Link Channels */
uint32_t edma3_link_ch_max_val[EDMA3_MAX_EDMA3_INSTANCES];
/** Min of QDMA Channels */
uint32_t edma3_qdma_ch_min_val[EDMA3_MAX_EDMA3_INSTANCES];
/** Max of QDMA Channels */
uint32_t edma3_qdma_ch_max_val[EDMA3_MAX_EDMA3_INSTANCES];
/** Max of Logical Channels */
uint32_t edma3_log_ch_max_val[EDMA3_MAX_EDMA3_INSTANCES];

/* Globals */
/*---------------------------------------------------------------------------*/
/**
 * \brief EDMA3 Resource Manager Objects, tied to each EDMA3 HW Controller.
 *
 * Typically one RM object will cater to one EDMA3 HW controller
 * and will have all the global config information.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(resMgrObj, ".fardata:.edma3Globals");
#endif
EDMA3_RM_Obj resMgrObj[EDMA3_MAX_EDMA3_INSTANCES];

/**
 * Global Array to store the mapping between DMA channels and Interrupt
 * channels i.e. TCCs.
 * DMA channel X can use any TCC Y. Transfer completion
 * interrupt will occur on the TCC Y (IPR/IPRH Register, bit Y), but error
 * interrupt will occur on DMA channel X (EMR/EMRH register, bit X). In that
 * scenario, this DMA channel <-> TCC mapping will be used to point to
 * the correct callback function.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(edma3DmaChTccMapping, ".fardata:.edma3Globals");
#endif
static uint32_t edma3DmaChTccMapping[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_DMA_CH];

/**
 * Global Array to store the mapping between QDMA channels and Interrupt
 * channels i.e. TCCs.
 * QDMA channel X can use any TCC Y. Transfer completion
 * interrupt will occur on the TCC Y (IPR/IPRH Register, bit Y), but error
 * interrupt will occur on QDMA channel X (QEMR register, bit X). In that
 * scenario, this QDMA channel <-> TCC mapping will be used to point to
 * the correct callback function.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(edma3QdmaChTccMapping, ".fardata:.edma3Globals");
#endif
static uint32_t edma3QdmaChTccMapping[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_QDMA_CH];

/**
 * Global Array to maintain the Callback details registered
 * against a particular TCC. Used to call the callback
 * functions linked to the particular channel.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(edma3IntrParams, ".fardata:.edma3Globals");
#endif
static EDMA3_RM_TccCallbackParams edma3IntrParams[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_TCC];

/** edma3RegionId will be updated ONCE using the parameter regionId passed to
 * the EDMA3_RM_open() function, for the Master RM instance (one who
 * configures the Global Registers).
 * This global variable will be used within the Interrupt handlers to know
 * which shadow region registers to access. All other interrupts coming
 * from other shadow regions will not be handled.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(edma3RegionId, ".fardata:.edma3Globals");
#endif
static EDMA3_RM_RegionId edma3RegionId = EDMA3_MAX_REGIONS;

/** masterExists[] will be updated when the Master RM Instance modifies the
 * Global EDMA3 configuration registers. It is used to prevent any other
 * Master RM Instance creation.
 * masterExists[] is per EDMA3 hardware, hence it is created
 * as an array.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(masterExists, ".fardata:.edma3Globals");
#endif
static uint32_t masterExists[EDMA3_MAX_EDMA3_INSTANCES] = {FALSE, FALSE, FALSE, FALSE, FALSE};

/**
 * Number of PaRAM Sets actually present on the SoC. This will be updated
 * while creating the Resource Manager Object.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(edma3NumPaRAMSets, ".fardata:.edma3Globals");
#endif
uint32_t edma3NumPaRAMSets = EDMA3_MAX_PARAM_SETS;

/**
 * The list of Interrupt Channels which get allocated while requesting the
 * TCC. It will be used while checking the IPR/IPRH bits in the RM ISR.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(allocatedTCCs, ".fardata:.edma3Globals");
#endif
static uint32_t allocatedTCCs[EDMA3_MAX_EDMA3_INSTANCES][2U] =
    {
        {0x0U, 0x0U},
        {0x0U, 0x0U},
        {0x0U, 0x0U},
        {0x0U, 0x0U},
        {0x0U, 0x0U},
};

/**
 * Arrays ownDmaChannels[], resvdDmaChannels and avlblDmaChannels will be ANDed
 * and stored in this array. It will be referenced in
 * EDMA3_RM_allocContiguousResource () to look for contiguous resources.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(contiguousDmaRes, ".fardata:.edma3Globals");
#endif
static uint32_t contiguousDmaRes[EDMA3_MAX_DMA_CHAN_DWRDS] = {0x0U, 0x0U};

/**
 * Arrays ownDmaChannels[], resvdDmaChannels and avlblDmaChannels will be ANDed
 * and stored in this array. It will be referenced in
 * EDMA3_RM_allocContiguousResource () to look for contiguous resources.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(contiguousQdmaRes, ".fardata:.edma3Globals");
#endif
static uint32_t contiguousQdmaRes[EDMA3_MAX_QDMA_CHAN_DWRDS] = {0x0U};

/**
 * Arrays ownDmaChannels[], resvdDmaChannels and avlblDmaChannels will be ANDed
 * and stored in this array. It will be referenced in
 * EDMA3_RM_allocContiguousResource () to look for contiguous resources.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(contiguousTccRes, ".fardata:.edma3Globals");
#endif
static uint32_t contiguousTccRes[EDMA3_MAX_TCC_DWRDS] = {0x0U, 0x0U};

/**
 * Arrays ownDmaChannels[], resvdDmaChannels and avlblDmaChannels will be ANDed
 * and stored in this array. It will be referenced in
 * EDMA3_RM_allocContiguousResource () to look for contiguous resources.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(contiguousParamRes, ".fardata:.edma3Globals");
#endif
static uint32_t contiguousParamRes[EDMA3_MAX_PARAM_DWRDS];

/**
 * \brief Resources bound to a Channel
 *
 * When a request for a channel is made, the resources PaRAM Set and TCC
 * get bound to that channel. This information is needed internally by the
 * resource manager, when a request is made to free the channel,
 * to free up the channel-associated resources.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(edma3RmChBoundRes, ".fardata:.edma3Globals");
#endif
static EDMA3_RM_ChBoundResources edma3RmChBoundRes[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_LOGICAL_CH];

/**
 * Used to reset the Internal EDMA3 Resource Manager Data Structures for the first time.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(rmInitDone, ".fardata:.edma3Globals");
#endif
static uint16_t rmInitDone = FALSE;

/*---------------------------------------------------------------------------*/

/* Local functions prototypes */
/*---------------------------------------------------------------------------*/
/** EDMA3 Instance 0 Completion Handler Interrupt Service Routine */
void lisrEdma3ComplHandler0(uint32_t edma3InstanceId);
/** EDMA3 Instance 0 CC Error Interrupt Service Routine */
void lisrEdma3CCErrHandler0(uint32_t edma3InstanceId);
/**
 * EDMA3 Instance 0 TC[0-7] Error Interrupt Service Routines
 * for a maximum of 8 TCs (Transfer Controllers).
 */
void lisrEdma3TC0ErrHandler0(uint32_t edma3InstanceId);
void lisrEdma3TC1ErrHandler0(uint32_t edma3InstanceId);
void lisrEdma3TC2ErrHandler0(uint32_t edma3InstanceId);
void lisrEdma3TC3ErrHandler0(uint32_t edma3InstanceId);
void lisrEdma3TC4ErrHandler0(uint32_t edma3InstanceId);
void lisrEdma3TC5ErrHandler0(uint32_t edma3InstanceId);
void lisrEdma3TC6ErrHandler0(uint32_t edma3InstanceId);
void lisrEdma3TC7ErrHandler0(uint32_t edma3InstanceId);

/** Interrupt Handler for the Transfer Completion interrupt */
static void edma3ComplHandler(const EDMA3_RM_Obj *rmObj);
/** Interrupt Handler for the Channel Controller Error interrupt */
static void edma3CCErrHandler(const EDMA3_RM_Obj *rmObj);
/** Interrupt Handler for the Transfer Controller Error interrupt */
static void edma3TCErrHandler(const EDMA3_RM_Obj *rmObj, uint32_t tcNum);

/** Local MemZero function */
void edma3MemZero(void *dst, uint32_t len);
/** Local MemCpy function */
void edma3MemCpy(void *dst, const void *src, uint32_t len);
/* Local MemCopy function to copy Param Set ONLY */
void edma3ParamCpy(volatile void *dst, const volatile void *src);

/** Initialization of the Global region registers of the EDMA3 Controller */
static void edma3GlobalRegionInit(uint32_t phyCtrllerInstId, uint32_t numDmaChannels);
/** Initialization of the Shadow region registers of the EDMA3 Controller */
static void edma3ShadowRegionInit(const EDMA3_RM_Instance *pRMInstance);

/* Internal functions for contiguous resource allocation */
/**
 * Finds a particular bit ('0' or '1') in the particular word from 'start'.
 * If found, returns the position, else return -1.
 */
static int32_t findBitInWord(int32_t source, uint32_t start, uint16_t bit);

/**
 * Finds a particular bit ('0' or '1') in the specified resources' array
 * from 'start' to 'end'. If found, returns the position, else return -1.
 */
static int32_t findBit(EDMA3_RM_ResType resType,
                       uint32_t start,
                       uint32_t end,
                       uint16_t bit);

/**
 * If successful, this function returns EDMA3_RM_SOK and the position
 * of first available resource in 'positionRes'. Else returns error.
 */
static EDMA3_RM_Result allocAnyContigRes(EDMA3_RM_ResType resType,
                                         uint32_t numResources,
                                         uint32_t *positionRes);

/**
 * Starting from 'firstResIdObj', this function makes the next 'numResources'
 * Resources non-available for future. Also, it does some global resisters'
 * setting also.
 */
static EDMA3_RM_Result gblChngAllocContigRes(EDMA3_RM_Instance *rmInstance,
                                             const EDMA3_RM_ResDesc *firstResIdObj,
                                             uint32_t numResources);

/*---------------------------------------------------------------------------*/

EDMA3_RM_Result EDMA3_RM_create(uint32_t phyCtrllerInstId,
                                const EDMA3_RM_GblConfigParams *gblCfgParams,
                                const void *miscParam)
{
    uint32_t count = 0U;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    const EDMA3_RM_MiscParam *miscOpt = (const EDMA3_RM_MiscParam *)miscParam;

#ifdef GENERIC
    /* GENERIC libraries don't come with a default confifguration, always 
       needs to be supplied with a parameter */
    if (gblCfgParams == NULL)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /**
     * We are NOT checking 'gblCfgParams' for NULL.
     * If user has passed NULL, default config info will be
     * taken from config file.
     * 'param' is also not being checked because it could be
     * NULL also.
     */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (phyCtrllerInstId >= EDMA3_MAX_EDMA3_INSTANCES)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        /* Initialize the global variables for the first time */
        if (FALSE == rmInitDone)
        {
            edma3MemZero((void *)&(resMgrObj[0U]),
                         sizeof(resMgrObj));
            edma3MemZero((void *)(&(edma3IntrParams[0U][0U])),
                         sizeof(edma3IntrParams));
            rmInitDone = TRUE;
        }

        /* Initialization has been done */
        if (resMgrObj[phyCtrllerInstId].state != EDMA3_RM_DELETED)
        {
            result = EDMA3_RM_E_OBJ_NOT_DELETED;
        }
        else
        {
            /**
              * Check whether user has passed the Global Config Info.
              * If yes, copy it to the driver data structures. Else, use the
              * info from the config file edma3Cfg.c
              */
#ifndef GENERIC
            if (NULL == gblCfgParams)
            {
                /* Take info from the specific config file */
                edma3MemCpy((void *)(&resMgrObj[phyCtrllerInstId].gblCfgParams),
                            (const void *)(&edma3GblCfgParams[phyCtrllerInstId]),
                            sizeof(EDMA3_RM_GblConfigParams));
            }
            else
            {
#endif
                /* User passed the info, save it in the RM object first */
                edma3MemCpy((void *)(&resMgrObj[phyCtrllerInstId].gblCfgParams),
                            (const void *)(gblCfgParams),
                            sizeof(EDMA3_RM_GblConfigParams));
#ifndef GENERIC
            }
#endif

            /**
              * Check whether DMA channel to PaRAM Set mapping exists or not.
              * If it does not exist, set the mapping array as 1-to-1 mapped.
              */
            if (FALSE == resMgrObj[phyCtrllerInstId].gblCfgParams.dmaChPaRAMMapExists)
            {
                for (count = 0U; count < resMgrObj[phyCtrllerInstId].gblCfgParams.numDmaChannels; count++)
                {
                    resMgrObj[phyCtrllerInstId].gblCfgParams.dmaChannelPaRAMMap[count] = count;
                }
            }

            /**
             * Update the actual number of PaRAM sets and
             * Initialize Boundary Values for Logical Channel Ranges.
             */
            edma3NumPaRAMSets = resMgrObj[phyCtrllerInstId].gblCfgParams.numPaRAMSets;
            edma3_dma_ch_max_val[phyCtrllerInstId] = resMgrObj[phyCtrllerInstId].gblCfgParams.numDmaChannels - 1U;
            edma3_link_ch_min_val[phyCtrllerInstId] = edma3_dma_ch_max_val[phyCtrllerInstId] + 1U;
            edma3_link_ch_max_val[phyCtrllerInstId] = edma3_link_ch_min_val[phyCtrllerInstId] + (resMgrObj[phyCtrllerInstId].gblCfgParams.numPaRAMSets - 1U);
            edma3_qdma_ch_min_val[phyCtrllerInstId] = edma3_link_ch_max_val[phyCtrllerInstId] + 1U;
            edma3_qdma_ch_max_val[phyCtrllerInstId] = edma3_qdma_ch_min_val[phyCtrllerInstId] + (resMgrObj[phyCtrllerInstId].gblCfgParams.numQdmaChannels - 1U);
            edma3_log_ch_max_val[phyCtrllerInstId] = edma3_qdma_ch_max_val[phyCtrllerInstId];

            resMgrObj[phyCtrllerInstId].phyCtrllerInstId = phyCtrllerInstId;
            resMgrObj[phyCtrllerInstId].state = EDMA3_RM_CREATED;
            resMgrObj[phyCtrllerInstId].numOpens = 0U;

            /* Make all the RM instances for this EDMA3 HW NULL */
            for (count = 0U; count < EDMA3_MAX_RM_INSTANCES; count++)
            {
                edma3MemZero((void *)((EDMA3_RM_Instance *)(ptrRMIArray) + (phyCtrllerInstId * EDMA3_MAX_RM_INSTANCES) + count),
                             sizeof(EDMA3_RM_Instance));

                /* Also make this data structure NULL */
                edma3MemZero((void *)((EDMA3_RM_InstanceInitConfig *)(ptrInitCfgArray) + (phyCtrllerInstId * EDMA3_MAX_RM_INSTANCES) + count),
                             sizeof(EDMA3_RM_InstanceInitConfig));
            }

            /* Initialize the global edma3DmaChTccMapping array with EDMA3_MAX_TCC */
            for (count = 0U;
                 count < resMgrObj[phyCtrllerInstId].gblCfgParams.numDmaChannels;
                 count++)
            {
                edma3DmaChTccMapping[phyCtrllerInstId][count] = EDMA3_MAX_TCC;
            }

            /* Initialize the global edma3QdmaChTccMapping array with EDMA3_MAX_TCC */
            for (count = 0U;
                 count < resMgrObj[phyCtrllerInstId].gblCfgParams.numQdmaChannels;
                 count++)
            {
                edma3QdmaChTccMapping[phyCtrllerInstId][count] = EDMA3_MAX_TCC;
            }

            /* Reset edma3RmChBoundRes Array*/
            for (count = 0U; count < EDMA3_MAX_LOGICAL_CH; count++)
            {
                edma3RmChBoundRes[phyCtrllerInstId][count].paRAMId = -1;
                edma3RmChBoundRes[phyCtrllerInstId][count].tcc = EDMA3_MAX_TCC;
            }

            /* Make the contiguousParamRes array NULL */
            edma3MemZero((void *)(&(contiguousParamRes[0U])),
                         sizeof(contiguousParamRes));

            /**
             * Check the misc configuration options structure.
             * Check whether the global registers' initialization
             * is required or not.
             * It is required ONLY if RM is running on the Master Processor.
             */
            if (NULL != miscOpt)
            {
                if (miscOpt->isSlave == FALSE)
                {
                    /* It is a master. */
                    edma3GlobalRegionInit(phyCtrllerInstId, (resMgrObj[phyCtrllerInstId].gblCfgParams.numDmaChannels));
                }
            }
            else
            {
                /* By default, global registers will be initialized. */
                edma3GlobalRegionInit(phyCtrllerInstId, (resMgrObj[phyCtrllerInstId].gblCfgParams.numDmaChannels));
            }
        }
    }

    return result;
}

EDMA3_RM_Result EDMA3_RM_delete(uint32_t phyCtrllerInstId,
                                const void *param)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (phyCtrllerInstId >= EDMA3_MAX_EDMA3_INSTANCES)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        /**
         * If number of RM Instances is 0, then state should be
         * EDMA3_RM_CLOSED OR EDMA3_RM_CREATED.
         */
        if ((0 == resMgrObj[phyCtrllerInstId].numOpens) && ((resMgrObj[phyCtrllerInstId].state != EDMA3_RM_CLOSED) && (resMgrObj[phyCtrllerInstId].state != EDMA3_RM_CREATED)))
        {
            result = EDMA3_RM_E_OBJ_NOT_CLOSED;
        }
        else
        {
            /**
             * If number of RM Instances is NOT 0, then this function
             * SHOULD NOT be called by anybody.
             */
            if (0 != resMgrObj[phyCtrllerInstId].numOpens)
            {
                result = EDMA3_RM_E_INVALID_STATE;
            }
            else
            {
                /** Change state to EDMA3_RM_DELETED */
                resMgrObj[phyCtrllerInstId].state = EDMA3_RM_DELETED;

                /* Reset the Allocated TCCs Array also. */
                allocatedTCCs[phyCtrllerInstId][0U] = 0x0U;
                allocatedTCCs[phyCtrllerInstId][1U] = 0x0U;

                /* Also, reset the RM Object Global Config Info */
                edma3MemZero((void *)&(resMgrObj[phyCtrllerInstId].gblCfgParams),
                             sizeof(EDMA3_RM_GblConfigParams));
            }
        }
    }

    return result;
}

EDMA3_RM_Handle EDMA3_RM_open(uint32_t phyCtrllerInstId,
                              const EDMA3_RM_Param *initParam,
                              EDMA3_RM_Result *errorCode)
{
    uint32_t intState = 0U;
    uint32_t resMgrIdx = 0U;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Obj *rmObj = NULL;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Instance *temp_ptr_rm_inst = NULL;
    EDMA3_RM_Handle retVal = NULL;
    uint32_t dmaChDwrds = 0U;
    uint32_t paramSetDwrds = 0U;
    uint32_t tccDwrds = 0U;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;

#ifdef GENERIC
    /* GENERIC libraries don't come with a default confifguration, always 
       needs to be supplied with a parameter */
    if ((initParam == NULL) || (initParam->rmInstInitConfig == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (((initParam == NULL) || (phyCtrllerInstId >= EDMA3_MAX_EDMA3_INSTANCES)) || (errorCode == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        /* Check whether the semaphore handle is null or not */
        if (NULL == initParam->rmSemHandle)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            rmObj = &resMgrObj[phyCtrllerInstId];
            if (
                (NULL == rmObj) || (initParam->regionId >=
                                    resMgrObj[phyCtrllerInstId].gblCfgParams.numRegions))
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
            else
            {
                edma3OsProtectEntry(phyCtrllerInstId,
                                    (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                    &intState);

                /** Check state of RM Object.
                  * If no RM instance is opened and this is the first one,
                  * then state should be created/closed.
                  */
                if ((rmObj->numOpens == 0) &&
                    ((rmObj->state != EDMA3_RM_CREATED) &&
                     (rmObj->state != EDMA3_RM_CLOSED)))
                {
                    result = EDMA3_RM_E_INVALID_STATE;
                    edma3OsProtectExit(phyCtrllerInstId,
                                       (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                       intState);
                }
                else
                {
                    /**
                     * If num of instances opened is more than 0 and less than
                     *  max allowed, then state should be opened.
                     */
                    if (((rmObj->numOpens > 0) &&
                         (rmObj->numOpens < EDMA3_MAX_RM_INSTANCES)) &&
                        (rmObj->state != EDMA3_RM_OPENED))
                    {
                        result = EDMA3_RM_E_INVALID_STATE;
                        edma3OsProtectExit(phyCtrllerInstId,
                                           (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                           intState);
                    }
                    else
                    {
                        /* Check if max opens have passed */
                        if (rmObj->numOpens >= EDMA3_MAX_RM_INSTANCES)
                        {
                            result = EDMA3_RM_E_MAX_RM_INST_OPENED;
                            edma3OsProtectExit(phyCtrllerInstId,
                                               (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                               intState);
                        }
                    }
                }
            }
        }
    }

    if (EDMA3_RM_SOK == result)
    {
        /*
        * Check whether the RM instance is Master or not.
        * If it is master, check whether a master already exists
        * or not. There should NOT be more than 1 master.
        * Return error code if master already exists
        */
        if ((TRUE == masterExists[phyCtrllerInstId]) && (TRUE == initParam->isMaster))
        {
            /* No two masters should exist, return error */
            result = EDMA3_RM_E_RM_MASTER_ALREADY_EXISTS;
            edma3OsProtectExit(phyCtrllerInstId,
                               (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                               intState);
        }
        else
        {
            /* Create Res Mgr Instance */
            for (resMgrIdx = 0U; resMgrIdx < EDMA3_MAX_RM_INSTANCES; resMgrIdx++)
            {
                temp_ptr_rm_inst = ((EDMA3_RM_Instance *)(ptrRMIArray) + (phyCtrllerInstId * EDMA3_MAX_RM_INSTANCES) + resMgrIdx);

                if (NULL != temp_ptr_rm_inst)
                {
                    if (NULL == temp_ptr_rm_inst->pResMgrObjHandle)
                    {
                        /* Handle to the EDMA3 HW Object */
                        temp_ptr_rm_inst->pResMgrObjHandle = rmObj;
                        /* Handle of the Res Mgr Instance */
                        rmInstance = temp_ptr_rm_inst;

                        /* Also make this data structure NULL, just for safety. */
                        edma3MemZero((void *)((EDMA3_RM_InstanceInitConfig *)(ptrInitCfgArray) + (phyCtrllerInstId * EDMA3_MAX_RM_INSTANCES) + resMgrIdx),
                                     sizeof(EDMA3_RM_InstanceInitConfig));

                        break;
                    }
                }
            }

            /* Check whether a RM instance has been created or not */
            if (NULL == rmInstance)
            {
                result = EDMA3_RM_E_MAX_RM_INST_OPENED;
                edma3OsProtectExit(phyCtrllerInstId,
                                   (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                   intState);
            }
            else
            {
                /* Copy the InitPaRAM first */
                edma3MemCpy((void *)(&rmInstance->initParam),
                            (const void *)(initParam),
                            sizeof(EDMA3_RM_Param));

                if (rmObj->gblCfgParams.globalRegs != NULL)
                {
                    globalRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);
                    rmInstance->shadowRegs = (EDMA3_CCRL_ShadowRegs *)&(globalRegs->SHADOW[rmInstance->initParam.regionId]);

                    /* copy the instance specific semaphore handle */
                    rmInstance->initParam.rmSemHandle = initParam->rmSemHandle;

                    /**
                    * Check whether user has passed information about resources
                    * owned and reserved by this instance. This is region specific
                    * information. If he has not passed, dafault static config info will be taken
                    * from the config file edma3Cfg.c, according to the regionId specified.
                    *
                    * resMgrIdx specifies the RM instance number created just now.
                    * Use it to populate the userInitConfig [].
                    */
#ifndef GENERIC
                    if (NULL == initParam->rmInstInitConfig)
                    {
                        /* Take the info from the specific config file */
                        edma3MemCpy((void *)((EDMA3_RM_InstanceInitConfig *)(ptrInitCfgArray) + (phyCtrllerInstId * EDMA3_MAX_RM_INSTANCES) + resMgrIdx),
                                    (const void *)(&defInstInitConfig[phyCtrllerInstId][initParam->regionId]),
                                    sizeof(EDMA3_RM_InstanceInitConfig));
                    }
                    else
                    {
#endif
                        /* User has passed the region specific info. */
                        edma3MemCpy((void *)((EDMA3_RM_InstanceInitConfig *)(ptrInitCfgArray) + (phyCtrllerInstId * EDMA3_MAX_RM_INSTANCES) + resMgrIdx),
                                    (const void *)(initParam->rmInstInitConfig),
                                    sizeof(EDMA3_RM_InstanceInitConfig));
#ifndef GENERIC
                    }
#endif

                    rmInstance->initParam.rmInstInitConfig =
                        ((EDMA3_RM_InstanceInitConfig *)(ptrInitCfgArray) + (phyCtrllerInstId * EDMA3_MAX_RM_INSTANCES) + resMgrIdx);

                    dmaChDwrds = rmObj->gblCfgParams.numDmaChannels / (uint32_t)32U;
                    if (dmaChDwrds == 0)
                    {
                        /* In case DMA channels are < 32 */
                        dmaChDwrds = 1U;
                    }

                    paramSetDwrds = rmObj->gblCfgParams.numPaRAMSets / (uint32_t)32U;
                    if (paramSetDwrds == 0)
                    {
                        /* In case PaRAM Sets are < 32 */
                        paramSetDwrds = 1U;
                    }

                    tccDwrds = rmObj->gblCfgParams.numTccs / (uint32_t)32U;
                    if (tccDwrds == 0)
                    {
                        /* In case TCCs are < 32 */
                        tccDwrds = 1U;
                    }

                    for (resMgrIdx = 0U; resMgrIdx < dmaChDwrds; ++resMgrIdx)
                    {
                        rmInstance->avlblDmaChannels[resMgrIdx] = rmInstance->initParam.rmInstInitConfig->ownDmaChannels[resMgrIdx];
                    }

                    rmInstance->avlblQdmaChannels[0U] = rmInstance->initParam.rmInstInitConfig->ownQdmaChannels[0U];

                    for (resMgrIdx = 0U; resMgrIdx < paramSetDwrds; ++resMgrIdx)
                    {
                        rmInstance->avlblPaRAMSets[resMgrIdx] = rmInstance->initParam.rmInstInitConfig->ownPaRAMSets[resMgrIdx];
                    }

                    for (resMgrIdx = 0U; resMgrIdx < tccDwrds; ++resMgrIdx)
                    {
                        rmInstance->avlblTccs[resMgrIdx] = rmInstance->initParam.rmInstInitConfig->ownTccs[resMgrIdx];
                    }

                    /*
                     * Mark the PaRAM Sets corresponding to DMA channels as RESERVED.
                     * For e.g. on a platform where only 32 DMA channels exist,
                     * mark the first 32 PaRAM Sets as reserved. These param sets
                     * will not be returned in case user requests for ANY link
                     * channel.
                     */
                    for (resMgrIdx = 0U; resMgrIdx < rmObj->gblCfgParams.numDmaChannels; ++resMgrIdx)
                    {
                        rmInstance->initParam.rmInstInitConfig->resvdPaRAMSets[resMgrIdx / 32U] |= ((uint32_t)1U << (resMgrIdx % 32U));
                    }

                    /*
                    * If the EDMA RM instance is MASTER (ie. initParam->isMaster
                    * is TRUE), save the region ID.
                    * Only this shadow region will receive the
                    * EDMA3 interrupts, if enabled.
                    */
                    if (TRUE == initParam->isMaster)
                    {
                        /* Store the region id to use it in the ISRs */
                        edma3RegionId = rmInstance->initParam.regionId;
                        masterExists[phyCtrllerInstId] = TRUE;
                    }

                    if (TRUE == initParam->regionInitEnable)
                    {
                        edma3ShadowRegionInit(rmInstance);
                    }

                    /**
                     * By default, PaRAM Sets allocated using this RM Instance
                     * will get cleared during their allocation.
                     * User can stop their clearing by calling specific IOCTL
                     * command.
                     */
                    rmInstance->paramInitRequired = TRUE;

                    /**
                     * By default, during the EDMA3_RM_allocLogicalChannel (),
                     * global EDMA3 registers (DCHMAP/QCHMAP) and the allocated
                     * PaRAM Set will be programmed accordingly, for users using this
                     * RM Instance.
                     * User can stop their pre-programming by calling
                     * EDMA3_RM_IOCTL_SET_GBL_REG_MODIFY_OPTION
                     * IOCTL command.
                     */
                    rmInstance->regModificationRequired = TRUE;

                    if (EDMA3_RM_SOK == result)
                    {
                        rmObj->state = EDMA3_RM_OPENED;
                        /* Increase the Instance count */
                        resMgrObj[phyCtrllerInstId].numOpens++;
                        retVal = rmInstance;
                    }
                }
                else
                {
                    result = EDMA3_RM_E_INVALID_PARAM;
                }

                edma3OsProtectExit(phyCtrllerInstId,
                                   (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                   intState);
            }
        }
    }

    if (errorCode != NULL)
    {
        *errorCode = result;
    }
    return (EDMA3_RM_Handle)retVal;
}

EDMA3_RM_Result EDMA3_RM_close(EDMA3_RM_Handle hEdmaResMgr,
                               const void *param)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    uint32_t intState = 0U;
    uint32_t resMgrIdx = 0U;
    EDMA3_RM_Obj *rmObj = NULL;
    EDMA3_RM_Instance *rmInstance = NULL;
    uint32_t dmaChDwrds;
    uint32_t paramSetDwrds;
    uint32_t tccDwrds;

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (NULL == hEdmaResMgr)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = (EDMA3_RM_E_INVALID_PARAM);
        }
        else
        {
            /* Check state of driver, state should be opened */
            if (rmObj->state != EDMA3_RM_OPENED)
            {
                result = (EDMA3_RM_E_OBJ_NOT_OPENED);
            }
            else
            {
                dmaChDwrds = rmObj->gblCfgParams.numDmaChannels / (uint32_t)32U;
                paramSetDwrds = rmObj->gblCfgParams.numPaRAMSets / (uint32_t)32U;
                tccDwrds = rmObj->gblCfgParams.numTccs / (uint32_t)32U;

                /* Set the instance config as NULL*/
                for (resMgrIdx = 0U; resMgrIdx < dmaChDwrds; ++resMgrIdx)
                {
                    rmInstance->avlblDmaChannels[resMgrIdx] = 0x0U;
                }
                for (resMgrIdx = 0U; resMgrIdx < paramSetDwrds; ++resMgrIdx)
                {
                    rmInstance->avlblPaRAMSets[resMgrIdx] = 0x0U;
                }
                rmInstance->avlblQdmaChannels[0U] = 0x0U;
                for (resMgrIdx = 0U; resMgrIdx < tccDwrds; ++resMgrIdx)
                {
                    rmInstance->avlblTccs[resMgrIdx] = 0x0U;
                }

                /**
                 * If this is the Master Instance, reset the static variable
                 * 'masterExists[]'.
                 */
                if (TRUE == rmInstance->initParam.isMaster)
                {
                    masterExists[rmObj->phyCtrllerInstId] = FALSE;
                    edma3RegionId = EDMA3_MAX_REGIONS;
                }

                /* Reset the Initparam for this RM Instance */
                edma3MemZero((void *)&(rmInstance->initParam),
                             sizeof(EDMA3_RM_Param));

                /* Critical section starts */
                edma3OsProtectEntry(rmObj->phyCtrllerInstId,
                                    (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                    &intState);

                /* Decrease the Number of Opens */
                --rmObj->numOpens;
                if (0 == rmObj->numOpens)
                {
                    edma3MemZero((void *)&(edma3RmChBoundRes[rmObj->phyCtrllerInstId][0]),
                                 sizeof(edma3RmChBoundRes[rmObj->phyCtrllerInstId]));

                    rmObj->state = EDMA3_RM_CLOSED;
                }

                /* Critical section ends */
                edma3OsProtectExit(rmObj->phyCtrllerInstId,
                                   (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                   intState);

                rmInstance->pResMgrObjHandle = NULL;
                rmInstance->shadowRegs = NULL;
                rmInstance = NULL;
            }
        }
    }

    return result;
}

EDMA3_RM_Result EDMA3_RM_allocResource(EDMA3_RM_Handle hEdmaResMgr,
                                       EDMA3_RM_ResDesc *resObj)
{
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Result semResult = EDMA3_RM_SOK;
    uint32_t avlblIdx = 0U;
    uint32_t resIdClr = 0x0;
    uint32_t resIdSet = 0x0;
    uint32_t resId;
    volatile EDMA3_CCRL_Regs *gblRegs = NULL;
    uint32_t mapXbarEvtToChanFlag = FALSE;
    uint32_t xBarEvtBeforeMap = 0;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((hEdmaResMgr == NULL) || (resObj == NULL))
    {
        result = (EDMA3_RM_E_INVALID_PARAM);
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        rmObj = rmInstance->pResMgrObjHandle;

        if ((rmObj == NULL) ||
            (rmObj->gblCfgParams.globalRegs == NULL))
        {
            result = (EDMA3_RM_E_INVALID_PARAM);
        }
        else
        {
            gblRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);
            edma3Id = rmObj->phyCtrllerInstId;
            resId = resObj->resId;

            resIdClr = (uint32_t)(~((uint32_t)1U << (resId % 32U)));
            resIdSet = (1U << (resId % 32U));

            if (rmInstance->mapXbarToChan != NULL)
            {
                xBarEvtBeforeMap = resId;
                if ((resId > rmObj->gblCfgParams.numDmaChannels) &&
                    (resId != EDMA3_RM_RES_ANY) &&
                    (resObj->type == EDMA3_RM_RES_DMA_CHANNEL))
                {
                    result = rmInstance->mapXbarToChan(xBarEvtBeforeMap,
                                                       &resObj->resId,
                                                       &rmInstance->rmXbarToEvtMapConfig);
                    if (EDMA3_RM_SOK == result)
                    {
                        resId = resObj->resId;
                        mapXbarEvtToChanFlag = TRUE;
                    }
                }
            }

            if (result == EDMA3_RM_SOK)
            {
                /**
	              * Take the instance specific semaphore, to prevent simultaneous
	              * access to the shared resources.
	              */
                semResult = edma3OsSemTake(rmInstance->initParam.rmSemHandle,
                                           EDMA3_OSSEM_NO_TIMEOUT);
                if (EDMA3_RM_SOK == semResult)
                {
                    switch (resObj->type)
                    {
                    case EDMA3_RM_RES_DMA_CHANNEL:
                    {
                        if (resId == EDMA3_RM_RES_ANY)
                        {
                            for (avlblIdx = 0U;
                                 avlblIdx <
                                 rmObj->gblCfgParams.numDmaChannels;
                                 ++avlblIdx)
                            {
                                if (((rmInstance->initParam.rmInstInitConfig->ownDmaChannels[avlblIdx / 32U]) &
                                     (rmInstance->avlblDmaChannels[avlblIdx / 32U]) &
                                     ~(rmInstance->initParam.rmInstInitConfig->resvdDmaChannels[avlblIdx / 32U]) &
                                     ((uint32_t)1U << (avlblIdx % 32U))) != FALSE)
                                {
                                    /*
	                                         * Match found.
	                                         * A resource which is owned by this instance of the
	                                         * Resource Manager and which is presently available
	                                         * and which has not been reserved - is found.
	                                         */
                                    resObj->resId = avlblIdx;
                                    /*
	                                         * Mark the 'match found' resource as "Not Available"
	                                         * for future requests
	                                         */
                                    rmInstance->avlblDmaChannels[avlblIdx / 32U] &= (uint32_t)(~((uint32_t)1U << (avlblIdx % 32U)));

                                    /**
	                                         * Check if the register modification flag is
	                                         * set or not.
	                                         */
                                    if (TRUE == rmInstance->regModificationRequired)
                                    {
                                        /**
	                                             * Enable the DMA channel in the
	                                             * DRAE/DRAEH registers also.
	                                             */
                                        if (avlblIdx < 32U)
                                        {
                                            gblRegs->DRA[rmInstance->initParam.regionId].DRAE |= ((uint32_t)0x1U << avlblIdx);
                                        }
                                        else
                                        {
                                            gblRegs->DRA[rmInstance->initParam.regionId].DRAEH |= ((uint32_t)0x1U << (avlblIdx - 32U));
                                        }
                                    }

                                    result = EDMA3_RM_SOK;
                                    break;
                                }
                            }
                            /*
	                                 * If none of the owned resources of this type is available
	                                 * then report "All Resources of this type not available" error
	                                 */
                            if (avlblIdx == rmObj->gblCfgParams.numDmaChannels)
                            {
                                result = EDMA3_RM_E_ALL_RES_NOT_AVAILABLE;
                            }
                        }
                        else
                        {
                            if (resId < rmObj->gblCfgParams.numDmaChannels)
                            {
                                /*
	                                     * Check if specified resource is owned
	                                     * by this instance of the resource manager
	                                     */
                                if (((rmInstance->initParam.rmInstInitConfig->ownDmaChannels[resId / 32U]) & (resIdSet)) != FALSE)
                                {
                                    /* Now check if specified resource is available presently*/
                                    if (((rmInstance->avlblDmaChannels[resId / 32U]) & (resIdSet)) != FALSE)
                                    {
                                        /*
	                                             * Mark the specified channel as "Not Available"
	                                             * for future requests
	                                             */
                                        rmInstance->avlblDmaChannels[resId / 32U] &= resIdClr;

                                        /**
	                                             * Check if the register modification flag is
	                                             * set or not.
	                                             */
                                        if (TRUE == rmInstance->regModificationRequired)
                                        {
                                            if (resId < 32U)
                                            {
                                                rmInstance->shadowRegs->EECR = (1UL << resId);

                                                /**
	                                                     * Enable the DMA channel in the
	                                                     * DRAE registers also.
	                                                     */
                                                gblRegs->DRA[rmInstance->initParam.regionId].DRAE |= ((uint32_t)0x1U << resId);
                                            }
                                            else
                                            {
                                                rmInstance->shadowRegs->EECRH = (1UL << resId);

                                                /**
	                                                     * Enable the DMA channel in the
	                                                     * DRAEH registers also.
	                                                     */
                                                gblRegs->DRA[rmInstance->initParam.regionId].DRAEH |= ((uint32_t)0x1U << (resId - 32U));
                                            }
                                        }

                                        result = EDMA3_RM_SOK;
                                    }
                                    else
                                    {
                                        /* Specified resource is owned but is already booked */
                                        result = EDMA3_RM_E_SPECIFIED_RES_NOT_AVAILABLE;
                                    }
                                }
                                else
                                {
                                    /*
	                                         * Specified resource is not owned by this instance
	                                         * of the Resource Manager
	                                         */
                                    result = EDMA3_RM_E_RES_NOT_OWNED;
                                }
                            }
                            else
                            {
                                result = EDMA3_RM_E_INVALID_PARAM;
                            }
                        }
                    }
                    break;

                    case EDMA3_RM_RES_QDMA_CHANNEL:
                    {
                        if (resId == EDMA3_RM_RES_ANY)
                        {
                            for (avlblIdx = 0U; avlblIdx < rmObj->gblCfgParams.numQdmaChannels; ++avlblIdx)
                            {
                                if (((rmInstance->initParam.rmInstInitConfig->ownQdmaChannels[avlblIdx / 32U]) &
                                     (rmInstance->avlblQdmaChannels[avlblIdx / 32U]) &
                                     ~(rmInstance->initParam.rmInstInitConfig->resvdQdmaChannels[avlblIdx / 32U]) &
                                     ((uint32_t)1U << (avlblIdx % 32U))) != FALSE)
                                {
                                    resObj->resId = avlblIdx;
                                    rmInstance->avlblQdmaChannels[avlblIdx / 32U] &= (uint32_t)(~((uint32_t)1U << (avlblIdx % 32U)));

                                    /**
	                                     * Check if the register modification flag is
	                                     * set or not.
	                                     */
                                    if (TRUE == rmInstance->regModificationRequired)
                                    {
                                        /**
	                                         * Enable the QDMA channel in the
	                                         * QRAE register also.
	                                         */
                                        gblRegs->QRAE[rmInstance->initParam.regionId] |= ((uint32_t)0x1U << avlblIdx);
                                    }

                                    result = EDMA3_RM_SOK;
                                    break;
                                }
                            }
                            /*
	                             * If none of the owned resources of this type is available
	                             * then report "All Resources of this type not available" error
	                             */
                            if (avlblIdx == rmObj->gblCfgParams.numQdmaChannels)
                            {
                                result = EDMA3_RM_E_ALL_RES_NOT_AVAILABLE;
                            }
                        }
                        else
                        {
                            if (resId < rmObj->gblCfgParams.numQdmaChannels)
                            {
                                if (((rmInstance->initParam.rmInstInitConfig->ownQdmaChannels[resId / 32U]) & (resIdSet)) != FALSE)
                                {
                                    if (((rmInstance->avlblQdmaChannels[resId / 32U]) & (resIdSet)) != FALSE)
                                    {
                                        rmInstance->avlblQdmaChannels[resId / 32U] &= resIdClr;

                                        /**
	                                         * Check if the register modification flag is
	                                         * set or not.
	                                         */
                                        if (TRUE == rmInstance->regModificationRequired)
                                        {
                                            /**
	                                             * Enable the QDMA channel in the
	                                             * QRAE register also.
	                                             */
                                            gblRegs->QRAE[rmInstance->initParam.regionId] |= ((uint32_t)0x1U << resId);
                                        }

                                        result = EDMA3_RM_SOK;
                                    }
                                    else
                                    {
                                        /* Specified resource is owned but is already booked */
                                        result = EDMA3_RM_E_SPECIFIED_RES_NOT_AVAILABLE;
                                    }
                                }
                                else
                                {
                                    /*
	                                     * Specified resource is not owned by this instance
	                                     * of the Resource Manager
	                                     */
                                    result = EDMA3_RM_E_RES_NOT_OWNED;
                                }
                            }
                            else
                            {
                                result = EDMA3_RM_E_INVALID_PARAM;
                            }
                        }
                    }
                    break;

                    case EDMA3_RM_RES_TCC:
                    {
                        if (resId == EDMA3_RM_RES_ANY)
                        {
                            for (avlblIdx = 0U; avlblIdx < rmObj->gblCfgParams.numTccs; ++avlblIdx)
                            {
                                if (((rmInstance->initParam.rmInstInitConfig->ownTccs[avlblIdx / 32U]) & (rmInstance->avlblTccs[avlblIdx / 32U]) & ~(rmInstance->initParam.rmInstInitConfig->resvdTccs[avlblIdx / 32U]) & ((uint32_t)1U << (avlblIdx % 32U))) != FALSE)
                                {
                                    resObj->resId = avlblIdx;
                                    rmInstance->avlblTccs[avlblIdx / 32U] &= (uint32_t)(~((uint32_t)1U << (avlblIdx % 32U)));

                                    /**
	                                     * Check if the register modification flag is
	                                     * set or not.
	                                     */
                                    if (TRUE == rmInstance->regModificationRequired)
                                    {
                                        /**
	                                         * Enable the Interrupt channel in the
	                                         * DRAE/DRAEH registers also.
	                                         * Also, If the region id coming from this
	                                         * RM instance is same as the Master RM
	                                         * Instance's region id, only then we will be
	                                         * getting the interrupts on the same side.
	                                         * So save the TCC in the allocatedTCCs[] array.
	                                         */
                                        if (avlblIdx < 32U)
                                        {
                                            gblRegs->DRA[rmInstance->initParam.regionId].DRAE |= ((uint32_t)0x1U << avlblIdx);

                                            /**
	                                             * Do not modify this global array if the register
	                                             * modificatio flag is not set.
	                                             * Reason being is based on this flag, the IPR/ICR
	                                             * or error bit is cleared in the completion or
	                                             * error handler ISR.
	                                             */
                                            if (edma3RegionId == rmInstance->initParam.regionId)
                                            {
                                                allocatedTCCs[edma3Id][0U] |= ((uint32_t)0x1U << avlblIdx);
                                            }
                                        }
                                        else
                                        {
                                            gblRegs->DRA[rmInstance->initParam.regionId].DRAEH |= ((uint32_t)0x1U << (avlblIdx - 32U));

                                            /**
	                                             * Do not modify this global array if the register
	                                             * modificatio flag is not set.
	                                             * Reason being is based on this flag, the IPR/ICR
	                                             * or error bit is cleared in the completion or
	                                             * error handler ISR.
	                                             */
                                            if (edma3RegionId == rmInstance->initParam.regionId)
                                            {
                                                allocatedTCCs[edma3Id][1U] |= ((uint32_t)0x1U << (avlblIdx - 32U));
                                            }
                                        }
                                    }

                                    result = EDMA3_RM_SOK;
                                    break;
                                }
                            }
                            /*
	                             * If none of the owned resources of this type is available
	                             * then report "All Resources of this type not available" error
	                             */
                            if (avlblIdx == rmObj->gblCfgParams.numTccs)
                            {
                                result = EDMA3_RM_E_ALL_RES_NOT_AVAILABLE;
                            }
                        }
                        else
                        {
                            if (resId < rmObj->gblCfgParams.numTccs)
                            {
                                if (((rmInstance->initParam.rmInstInitConfig->ownTccs[resId / 32U]) & (resIdSet)) != FALSE)
                                {
                                    if (((rmInstance->avlblTccs[resId / 32U]) & (resIdSet)) != FALSE)
                                    {
                                        rmInstance->avlblTccs[resId / 32U] &= resIdClr;

                                        /**
	                                         * Check if the register modification flag is
	                                         * set or not.
	                                         */
                                        if (TRUE == rmInstance->regModificationRequired)
                                        {
                                            /**
	                                             * Enable the Interrupt channel in the
	                                             * DRAE/DRAEH registers also.
	                                             * Also, If the region id coming from this
	                                             * RM instance is same as the Master RM
	                                             * Instance's region id, only then we will be
	                                             * getting the interrupts on the same side.
	                                             * So save the TCC in the allocatedTCCs[] array.
	                                             */
                                            if (resId < 32U)
                                            {
                                                gblRegs->DRA[rmInstance->initParam.regionId].DRAE |= ((uint32_t)0x1U << resId);

                                                /**
	                                                 * Do not modify this global array if the register
	                                                 * modificatio flag is not set.
	                                                 * Reason being is based on this flag, the IPR/ICR
	                                                 * or error bit is cleared in the completion or
	                                                 * error handler ISR.
	                                                 */
                                                if (edma3RegionId == rmInstance->initParam.regionId)
                                                {
                                                    allocatedTCCs[edma3Id][0U] |= ((uint32_t)0x1U << resId);
                                                }
                                            }
                                            else
                                            {
                                                gblRegs->DRA[rmInstance->initParam.regionId].DRAEH |= ((uint32_t)0x1U << (resId - 32U));

                                                /**
	                                                 * Do not modify this global array if the register
	                                                 * modificatio flag is not set.
	                                                 * Reason being is based on this flag, the IPR/ICR
	                                                 * or error bit is cleared in the completion or
	                                                 * error handler ISR.
	                                                 */
                                                if (edma3RegionId == rmInstance->initParam.regionId)
                                                {
                                                    allocatedTCCs[edma3Id][1U] |= ((uint32_t)0x1U << (resId - 32U));
                                                }
                                            }
                                        }

                                        result = EDMA3_RM_SOK;
                                    }
                                    else
                                    {
                                        /* Specified resource is owned but is already booked */
                                        result = EDMA3_RM_E_SPECIFIED_RES_NOT_AVAILABLE;
                                    }
                                }
                                else
                                {
                                    /*
	                                     * Specified resource is not owned by this instance
	                                     * of the Resource Manager
	                                     */
                                    result = EDMA3_RM_E_RES_NOT_OWNED;
                                }
                            }
                            else
                            {
                                result = EDMA3_RM_E_INVALID_PARAM;
                            }
                        }
                    }
                    break;

                    case EDMA3_RM_RES_PARAM_SET:
                    {
                        if (resId == EDMA3_RM_RES_ANY)
                        {
                            for (avlblIdx = 0U; avlblIdx < rmObj->gblCfgParams.numPaRAMSets; ++avlblIdx)
                            {
                                if (((rmInstance->initParam.rmInstInitConfig->ownPaRAMSets[avlblIdx / 32U]) &
                                     (rmInstance->avlblPaRAMSets[avlblIdx / 32U]) &
                                     ~(rmInstance->initParam.rmInstInitConfig->resvdPaRAMSets[avlblIdx / 32U]) &
                                     ((uint32_t)1U << (avlblIdx % 32U))) != FALSE)
                                {
                                    resObj->resId = avlblIdx;
                                    rmInstance->avlblPaRAMSets[avlblIdx / 32U] &= (uint32_t)(~((uint32_t)1U << (avlblIdx % 32U)));

                                    /**
	                                     * Also, make the actual PARAM Set NULL, checking the flag
	                                     * whether it is required or not.
	                                     */
                                    if ((TRUE == rmInstance->regModificationRequired) && (TRUE == rmInstance->paramInitRequired))
                                    {
                                        edma3MemZero((void *)(&gblRegs->PARAMENTRY[avlblIdx]),
                                                     sizeof(gblRegs->PARAMENTRY[avlblIdx]));
                                    }

                                    result = EDMA3_RM_SOK;
                                    break;
                                }
                            }
                            /*
	                             * If none of the owned resources of this type is available
	                             * then report "All Resources of this type not available" error
	                             */
                            if (avlblIdx == rmObj->gblCfgParams.numPaRAMSets)
                            {
                                result = EDMA3_RM_E_ALL_RES_NOT_AVAILABLE;
                            }
                        }
                        else
                        {
                            if (resId < rmObj->gblCfgParams.numPaRAMSets)
                            {
                                if (((rmInstance->initParam.rmInstInitConfig->ownPaRAMSets[resId / 32U]) & (resIdSet)) != FALSE)
                                {
                                    if (((rmInstance->avlblPaRAMSets[resId / 32U]) & (resIdSet)) != FALSE)
                                    {
                                        rmInstance->avlblPaRAMSets[resId / 32U] &= resIdClr;

                                        /**
	                                         * Also, make the actual PARAM Set NULL, checking the flag
	                                         * whether it is required or not.
	                                         */
                                        if ((TRUE == rmInstance->regModificationRequired) && (TRUE == rmInstance->paramInitRequired))
                                        {
                                            edma3MemZero((void *)(&gblRegs->PARAMENTRY[resId]),
                                                         sizeof(gblRegs->PARAMENTRY[resId]));
                                        }

                                        result = EDMA3_RM_SOK;
                                    }
                                    else
                                    {
                                        /* Specified resource is owned but is already booked */
                                        result = EDMA3_RM_E_SPECIFIED_RES_NOT_AVAILABLE;
                                    }
                                }
                                else
                                {
                                    /*
	                                     * Specified resource is not owned by this instance
	                                     * of the Resource Manager
	                                     */
                                    result = EDMA3_RM_E_RES_NOT_OWNED;
                                }
                            }
                            else
                            {
                                result = EDMA3_RM_E_INVALID_PARAM;
                            }
                        }
                    }
                    break;

                    default:
                        result = EDMA3_RM_E_INVALID_PARAM;
                        break;
                    }

                    /* Return the semaphore back */
                    semResult = edma3OsSemGive(rmInstance->initParam.rmSemHandle);
                }
            }
        }
    }

    /**
     * Check the Resource Allocation Result 'result' first. If Resource
     * Allocation has resulted in an error, return it (having more priority than
     * semResult.
     * Else, return semResult.
     */
    if (EDMA3_RM_SOK == result)
    {
        /**
        * Resource Allocation successful, return semResult for returning
        * semaphore.
        */
        result = semResult;
        if ((rmInstance->configScrMapXbarToEvt != NULL) &&
            (mapXbarEvtToChanFlag == TRUE))
        {
            rmInstance->configScrMapXbarToEvt(xBarEvtBeforeMap, resObj->resId);
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_freeResource(EDMA3_RM_Handle hEdmaResMgr,
                                      const EDMA3_RM_ResDesc *resObj)
{
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Result semResult = EDMA3_RM_SOK;
    uint32_t resId;
    uint32_t resIdSet = 0x0;
    volatile EDMA3_CCRL_Regs *gblRegs = NULL;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((hEdmaResMgr == NULL) || (resObj == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        rmObj = rmInstance->pResMgrObjHandle;

        if ((rmObj == NULL) ||
            (rmObj->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            gblRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);
            edma3Id = rmObj->phyCtrllerInstId;
            resId = resObj->resId;

            resIdSet = 1U << (resId % 32U);

            /**
	      * Take the instance specific semaphore, to prevent simultaneous
	      * access to the shared resources.
	      */
            semResult = edma3OsSemTake(rmInstance->initParam.rmSemHandle,
                                       EDMA3_OSSEM_NO_TIMEOUT);

            if (EDMA3_RM_SOK == semResult)
            {
                switch (resObj->type)
                {
                case EDMA3_RM_RES_DMA_CHANNEL:
                {
                    if ((resId < rmObj->gblCfgParams.numDmaChannels) && ((resId / 32U) < 2))
                    {
                        if (((rmInstance->initParam.rmInstInitConfig->ownDmaChannels[resId / 32U]) & (resIdSet)) != FALSE)
                        {
                            if (((~(rmInstance->avlblDmaChannels[resId / 32U])) & (resIdSet)) != FALSE)
                            {
                                /*
                                     * Mark the specified channel as "Available"
                                     * for future requests
                                     */
                                rmInstance->avlblDmaChannels[resId / 32U] |= resIdSet;

                                /**
                                     * Check if the register modification flag is
                                     * set or not.
                                     */
                                if (TRUE == rmInstance->regModificationRequired)
                                {
                                    /**
                                         * DMA Channel is freed.
                                         * Reset the bit specific to the DMA channel
                                         * in the DRAE/DRAEH register also.
                                         */
                                    if (resId < 32U)
                                    {
                                        gblRegs->DRA[rmInstance->initParam.regionId].DRAE &= (~((uint32_t)0x1U << resId));
                                    }
                                    else
                                    {
                                        gblRegs->DRA[rmInstance->initParam.regionId].DRAEH &= (~((uint32_t)0x1U << (resId - 32U)));
                                    }
                                }

                                result = EDMA3_RM_SOK;
                            }
                            else
                            {
                                result = EDMA3_RM_E_RES_ALREADY_FREE;
                            }
                        }
                        else
                        {
                            /*
                                 * Specified resource is not owned by this instance
                                 * of the Resource Manager
                                 */
                            result = EDMA3_RM_E_RES_NOT_OWNED;
                        }
                    }
                    else
                    {
                        result = EDMA3_RM_E_INVALID_PARAM;
                    }
                }
                break;

                case EDMA3_RM_RES_QDMA_CHANNEL:
                {
                    if (resId < rmObj->gblCfgParams.numQdmaChannels)
                    {
                        if (((rmInstance->initParam.rmInstInitConfig->ownQdmaChannels[resId / 32U]) & (resIdSet)) != FALSE)
                        {
                            if (((~(rmInstance->avlblQdmaChannels[resId / 32U])) & (resIdSet)) != FALSE)
                            {
                                rmInstance->avlblQdmaChannels[resId / 32U] |= resIdSet;

                                /**
                                     * Check if the register modification flag is
                                     * set or not.
                                     */
                                if (TRUE == rmInstance->regModificationRequired)
                                {
                                    /**
                                         * QDMA Channel is freed.
                                         * Reset the bit specific to the QDMA channel
                                         * in the QRAE register also.
                                         */
                                    gblRegs->QRAE[rmInstance->initParam.regionId] &= (~((uint32_t)0x1U << resId));
                                }

                                result = EDMA3_RM_SOK;
                            }
                            else
                            {
                                result = EDMA3_RM_E_RES_ALREADY_FREE;
                            }
                        }
                        else
                        {
                            /*
                                 * Specified resource is not owned by this instance
                                 * of the Resource Manager
                                 */
                            result = EDMA3_RM_E_RES_NOT_OWNED;
                        }
                    }
                    else
                    {
                        result = EDMA3_RM_E_INVALID_PARAM;
                    }
                }
                break;

                case EDMA3_RM_RES_TCC:
                {
                    if (resId < rmObj->gblCfgParams.numTccs)
                    {
                        if (((rmInstance->initParam.rmInstInitConfig->ownTccs[resId / 32U]) & (resIdSet)) != FALSE)
                        {
                            if (((~(rmInstance->avlblTccs[resId / 32U])) & (resIdSet)) != FALSE)
                            {
                                rmInstance->avlblTccs[resId / 32U] |= resIdSet;

                                /**
                                     * Check if the register modification flag is
                                     * set or not.
                                     */
                                if (TRUE == rmInstance->regModificationRequired)
                                {
                                    /**
                                         * Interrupt Channel is freed.
                                         * Reset the bit specific to the Interrupt
                                         * channel in the DRAE/DRAEH register also.
                                         * Also, if we have earlier saved this
                                         * TCC in allocatedTCCs[] array,
                                         * remove it from there too.
                                         */
                                    if (resId < 32U)
                                    {
                                        gblRegs->DRA[rmInstance->initParam.regionId].DRAE &= (~((uint32_t)0x1U << resId));

                                        if (edma3RegionId == rmInstance->initParam.regionId)
                                        {
                                            allocatedTCCs[edma3Id][0U] &= (~((uint32_t)0x1U << resId));
                                        }
                                    }
                                    else
                                    {
                                        gblRegs->DRA[rmInstance->initParam.regionId].DRAEH &= (~((uint32_t)0x1U << (resId - 32U)));

                                        if (edma3RegionId == rmInstance->initParam.regionId)
                                        {
                                            allocatedTCCs[edma3Id][1U] &= (~((uint32_t)0x1U << (resId - 32U)));
                                        }
                                    }
                                }

                                result = EDMA3_RM_SOK;
                            }
                            else
                            {
                                result = EDMA3_RM_E_RES_ALREADY_FREE;
                            }
                        }
                        else
                        {
                            /*
                                 * Specified resource is not owned by this instance
                                 * of the Resource Manager
                                 */
                            result = EDMA3_RM_E_RES_NOT_OWNED;
                        }
                    }
                    else
                    {
                        result = EDMA3_RM_E_INVALID_PARAM;
                    }
                }
                break;

                case EDMA3_RM_RES_PARAM_SET:
                {
                    if (resId < rmObj->gblCfgParams.numPaRAMSets)
                    {
                        if (((rmInstance->initParam.rmInstInitConfig->ownPaRAMSets[resId / 32U]) & (resIdSet)) != FALSE)
                        {
                            if (((~(rmInstance->avlblPaRAMSets[resId / 32U])) & (resIdSet)) != FALSE)
                            {
                                rmInstance->avlblPaRAMSets[resId / 32U] |= resIdSet;

                                result = EDMA3_RM_SOK;
                            }
                            else
                            {
                                result = EDMA3_RM_E_RES_ALREADY_FREE;
                            }
                        }
                        else
                        {
                            /*
                                 * Specified resource is not owned by this instance
                                 * of the Resource Manager
                                 */
                            result = EDMA3_RM_E_RES_NOT_OWNED;
                        }
                    }
                    else
                    {
                        result = EDMA3_RM_E_INVALID_PARAM;
                    }
                }
                break;

                default:
                    result = EDMA3_RM_E_INVALID_PARAM;
                    break;
                }
                semResult = edma3OsSemGive(rmInstance->initParam.rmSemHandle);
            }
        }
    }

    /**
         * Check the Free Resource Result 'result' first. If Free Resource
         * has resulted in an error, return it (having more priority than
         * semResult.
         * Else, return semResult.
         */
    if (EDMA3_RM_SOK == result)
    {
        /**
            * Free Resource successful, return semResult for returning
            * semaphore.
            */
        result = semResult;
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_allocLogicalChannel(EDMA3_RM_Handle hEdmaResMgr,
                                             EDMA3_RM_ResDesc *lChObj,
                                             uint32_t *pParam,
                                             uint32_t *pTcc)
{
    EDMA3_RM_ResDesc *chObj;
    EDMA3_RM_ResDesc resObj;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    uint32_t mappedPaRAMId = 0U;
    uint32_t mappedTcc = EDMA3_RM_CH_NO_TCC_MAP;
    int32_t paRAMId = (int32_t)EDMA3_RM_RES_ANY;
    volatile EDMA3_CCRL_Regs *gblRegs = NULL;
    uint32_t qdmaChId = EDMA3_MAX_PARAM_SETS;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((lChObj == NULL) || (hEdmaResMgr == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        chObj = lChObj;

        if ((chObj->type == EDMA3_RM_RES_DMA_CHANNEL) || (chObj->type == EDMA3_RM_RES_QDMA_CHANNEL))
        {
            /**
             * If the request is for a DMA or QDMA channel, check the
             * pParam and pTcc objects also.
             * For the Link channel request, they could be NULL.
             */
            /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
            if ((pParam == NULL) || (pTcc == NULL))
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
#endif
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;

        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            if (rmObj->gblCfgParams.globalRegs == NULL)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        edma3Id = rmObj->phyCtrllerInstId;
        gblRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);

        switch (chObj->type)
        {
        case EDMA3_RM_RES_DMA_CHANNEL:
        {
            if ((chObj->resId == EDMA3_RM_DMA_CHANNEL_ANY) || (chObj->resId == EDMA3_RM_RES_ANY))
            {
                /* Request for ANY DMA channel. */
                resObj.type = EDMA3_RM_RES_DMA_CHANNEL;
                resObj.resId = EDMA3_RM_RES_ANY;
                result = EDMA3_RM_allocResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);

                if (result == EDMA3_RM_SOK)
                {
                    /* DMA channel allocated successfully. */
                    chObj->resId = resObj.resId;

                    /**
                         * Check the PaRAM Set user has specified for this DMA channel.
                         * Two cases exist:
                         * a) DCHMAP exists: Any PaRAM Set can be used
                         * b) DCHMAP does not exist: Should not be possible
                         * only if the channel allocated (ANY) and PaRAM requested
                         * are same.
                         */
                    if ((*pParam) == EDMA3_RM_PARAM_ANY)
                    {
                        /* User specified ANY PaRAM Set; Check the mapping. */
                        mappedPaRAMId = rmObj->gblCfgParams.dmaChannelPaRAMMap[resObj.resId];
                        if (mappedPaRAMId != EDMA3_RM_CH_NO_PARAM_MAP)
                        {
                            /** If some PaRAM set is statically mapped to the returned
                                * channel number, use that.
                                */
                            paRAMId = (int32_t)mappedPaRAMId;
                        }
                    }
                    else
                    {
                        /* User specified some PaRAM Set; check that can be used or not. */
                        if (TRUE == rmObj->gblCfgParams.dmaChPaRAMMapExists)
                        {
                            paRAMId = (int32_t)(*pParam);
                        }
                        else
                        {
                            /**
                                 * Channel mapping does not exist. If the PaRAM Set requested
                                 * is the same as dma channel allocated (coincidentally), it is fine.
                                 * Else return error.
                                 */
                            if ((*pParam) != (resObj.resId))
                            {
                                result = EDMA3_RM_E_INVALID_PARAM;

                                /**
                                     * Free the previously allocated DMA channel also.
                                     */
                                EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
                            }
                            else
                            {
                                paRAMId = (int32_t)(*pParam);
                            }
                        }
                    }

                    mappedTcc = rmObj->gblCfgParams.dmaChannelTccMap[resObj.resId];
                }
            }
            else
            {
                if (chObj->resId <= edma3_dma_ch_max_val[edma3Id])
                {
                    /* Request for a specific DMA channel */
                    resObj.type = EDMA3_RM_RES_DMA_CHANNEL;
                    resObj.resId = chObj->resId;
                    result = EDMA3_RM_allocResource(hEdmaResMgr,
                                                    (EDMA3_RM_ResDesc *)&resObj);

                    if (result == EDMA3_RM_SOK)
                    {
                        /**
                             * Check the PaRAM Set user has specified for this DMA channel.
                             * Two cases exist:
                             * a) DCHMAP exists: Any PaRAM Set can be used
                             * b) DCHMAP does not exist: Should not be possible
                             * only if the channel allocated (ANY) and PaRAM requested
                             * are same.
                             */
                        if ((*pParam) == EDMA3_RM_PARAM_ANY)
                        {
                            /* User specified ANY PaRAM Set; Check the mapping. */
                            mappedPaRAMId = rmObj->gblCfgParams.dmaChannelPaRAMMap[resObj.resId];
                            if (mappedPaRAMId != EDMA3_RM_CH_NO_PARAM_MAP)
                            {
                                /** If some PaRAM set is statically mapped to the returned
                                    * channel number, use that.
                                    */
                                paRAMId = (int32_t)mappedPaRAMId;
                            }
                        }
                        else
                        {
                            /* User specified some PaRAM Set; check that can be used or not. */
                            if (TRUE == rmObj->gblCfgParams.dmaChPaRAMMapExists)
                            {
                                paRAMId = (int32_t)(*pParam);
                            }
                            else
                            {
                                /**
                                     * Channel mapping does not exist. If the PaRAM Set requested
                                     * is the same as dma channel allocated (coincidentally), it is fine.
                                     * Else return error.
                                     */
                                if ((*pParam) != (resObj.resId))
                                {
                                    result = EDMA3_RM_E_INVALID_PARAM;

                                    /**
                                         * Free the previously allocated DMA channel also.
                                         */
                                    EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
                                }
                                else
                                {
                                    paRAMId = (int32_t)(*pParam);
                                }
                            }
                        }

                        mappedTcc = rmObj->gblCfgParams.dmaChannelTccMap[chObj->resId];
                    }
                }
                else
                {
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
            }
        }
        break;

        case EDMA3_RM_RES_QDMA_CHANNEL:
        {
            if ((chObj->resId == EDMA3_RM_QDMA_CHANNEL_ANY) || (chObj->resId == EDMA3_RM_RES_ANY))
            {
                /* First request for any available QDMA channel */
                resObj.type = EDMA3_RM_RES_QDMA_CHANNEL;
                resObj.resId = EDMA3_RM_RES_ANY;
                result = EDMA3_RM_allocResource(hEdmaResMgr,
                                                (EDMA3_RM_ResDesc *)&resObj);

                if (result == EDMA3_RM_SOK)
                {
                    /* Return the actual QDMA channel id. */
                    chObj->resId = resObj.resId;

                    /* Save the Logical-QDMA channel id for future use. */
                    qdmaChId = resObj.resId + edma3_qdma_ch_min_val[edma3Id];

                    /**
                         * Check the PaRAM Set user has specified for this QDMA channel.
                         * If he has specified any particular PaRAM Set, use that.
                         */
                    if ((*pParam) != EDMA3_RM_PARAM_ANY)
                    {
                        /* User specified ANY PaRAM Set; Check the mapping. */
                        paRAMId = (int32_t)(*pParam);
                    }
                }
            }
            else
            {
                if (chObj->resId < rmObj->gblCfgParams.numQdmaChannels)
                {
                    /* Request for a specific QDMA channel */
                    resObj.type = EDMA3_RM_RES_QDMA_CHANNEL;
                    resObj.resId = chObj->resId;
                    result = EDMA3_RM_allocResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);

                    if (result == EDMA3_RM_SOK)
                    {
                        /* Save the Logical-QDMA channel id for future use. */
                        qdmaChId = chObj->resId + edma3_qdma_ch_min_val[edma3Id];

                        /**
                             * Check the PaRAM Set user has specified for this QDMA channel.
                             * If he has specified any particular PaRAM Set, use that.
                             */
                        if ((*pParam) != EDMA3_RM_PARAM_ANY)
                        {
                            /* User specified ANY PaRAM Set; Check the mapping. */
                            paRAMId = (int32_t)(*pParam);
                        }
                    }
                }
                else
                {
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
            }
        }
        break;

        case EDMA3_RM_RES_PARAM_SET:
        {
            /* Request for a LINK channel. */
            if ((chObj->resId == EDMA3_RM_PARAM_ANY) || (chObj->resId == EDMA3_RM_RES_ANY))
            {
                /* Request for ANY LINK channel. */
                paRAMId = (int32_t)EDMA3_RM_RES_ANY;
            }
            else
            {
                if (chObj->resId < edma3NumPaRAMSets)
                {
                    /* Request for a Specific LINK channel. */
                    paRAMId = (int32_t)(chObj->resId);
                }
                else
                {
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
            }

            if (result == EDMA3_RM_SOK)
            {
                /* Try to allocate the link channel */
                resObj.type = EDMA3_RM_RES_PARAM_SET;
                resObj.resId = (uint32_t)paRAMId;
                result = EDMA3_RM_allocResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);

                if (result == EDMA3_RM_SOK)
                {
                    uint32_t linkCh = edma3_link_ch_min_val[edma3Id];

                    /* Return the actual PaRAM Id. */
                    chObj->resId = resObj.resId;

                    /*
                            * Search for the next Link channel place-holder available,
                            * starting from EDMA3_RM_LINK_CH_MIN_VAL.
                            * It will be used for future operations on the Link channel.
                            */
                    while ((edma3RmChBoundRes[rmObj->phyCtrllerInstId][linkCh].paRAMId != -1) && (linkCh <= edma3_link_ch_max_val[edma3Id]))
                    {
                        /* Move to the next place-holder. */
                        linkCh++;
                    }

                    /* Verify the returned handle, it should lie in the correct range */
                    if (linkCh > edma3_link_ch_max_val[edma3Id])
                    {
                        result = EDMA3_RM_E_INVALID_PARAM;

                        /* Free the PaRAM Set now. */
                        resObj.type = EDMA3_RM_RES_PARAM_SET;
                        resObj.resId = chObj->resId;
                        result = EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
                    }
                    else
                    {
                        /* Save the PaRAM Id for the Link Channel. */
                        edma3RmChBoundRes[rmObj->phyCtrllerInstId][linkCh].paRAMId = (int32_t)(chObj->resId);

                        /**
                                 * Remove any linking. Before doing that, check
                                 * whether it is permitted or not.
                                 */
                        if (TRUE == rmInstance->regModificationRequired)
                        {
                            *((&gblRegs->PARAMENTRY[chObj->resId].OPT) + (uint32_t)EDMA3_RM_PARAM_ENTRY_LINK_BCNTRLD) = 0xFFFFU;
                        }
                    }
                }
            }
        }
        break;

        default:
        {
            result = EDMA3_RM_E_INVALID_PARAM;
            break;
        }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        /**
         * For DMA/QDMA channels, we still have to allocate more resources like
         * TCC, PaRAM Set etc.
         * For Link channel, only the PaRAMSet is required and that has been
         * allocated so no further operations required.
         */

        /* Further resources' allocation for DMA channel. */
        if (chObj->type == EDMA3_RM_RES_DMA_CHANNEL)
        {
            /* First allocate a PaRAM Set */
            resObj.type = EDMA3_RM_RES_PARAM_SET;
            /* Use the saved param id now. */
            resObj.resId = (uint32_t)paRAMId;
            result = EDMA3_RM_allocResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
            if (result == EDMA3_RM_SOK)
            {
                /**
                 * PaRAM Set allocation succeeded.
                 * Save the PaRAM Set first.
                 */
                *pParam = resObj.resId;
                edma3RmChBoundRes[rmObj->phyCtrllerInstId][chObj->resId].paRAMId = (int32_t)(resObj.resId);

                /* Allocate the TCC now. */
                resObj.type = EDMA3_RM_RES_TCC;
                if ((*pTcc) == EDMA3_RM_TCC_ANY)
                {
                    if (mappedTcc == EDMA3_RM_CH_NO_TCC_MAP)
                    {
                        resObj.resId = EDMA3_RM_RES_ANY;
                    }
                    else
                    {
                        resObj.resId = mappedTcc;
                    }
                }
                else
                {
                    resObj.resId = *pTcc;
                }

                result = EDMA3_RM_allocResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
                if (result == EDMA3_RM_SOK)
                {
                    /* TCC allocation succeeded. Save it first. */
                    *pTcc = resObj.resId;
                    edma3RmChBoundRes[rmObj->phyCtrllerInstId][chObj->resId].tcc = resObj.resId;

                    /**
                     * Check first whether the global registers and the allocated
                     * PaRAM Set can be modified or not. If yes, do the needful.
                     * Else leave this for the user.
                     */
                    if (TRUE == rmInstance->regModificationRequired)
                    {
                        /* Set TCC of the allocated Param Set. */
                        gblRegs->PARAMENTRY[*pParam].OPT &= EDMA3_RM_OPT_TCC_CLR_MASK;
                        gblRegs->PARAMENTRY[*pParam].OPT |= EDMA3_RM_OPT_TCC_SET_MASK(*pTcc);

                        /**
                         * Do the mapping between DMA channel and PaRAM Set.
                         * Do this for the EDMA3 Controllers which have a register for mapping
                         * DMA Channel to a particular PaRAM Set.
                         */
                        if (TRUE == rmObj->gblCfgParams.dmaChPaRAMMapExists)
                        {
                            gblRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);

                            /* Map Parameter RAM Set Number for specified channelId */
                            gblRegs->DCHMAP[chObj->resId] &= EDMA3_RM_DCH_PARAM_CLR_MASK;
                            gblRegs->DCHMAP[chObj->resId] |= EDMA3_RM_DCH_PARAM_SET_MASK(*pParam);
                        }

                        /* Remove any linking */
                        *((&gblRegs->PARAMENTRY[*pParam].OPT) + (uint32_t)EDMA3_RM_PARAM_ENTRY_LINK_BCNTRLD) = 0xFFFFU;
                    }
                }
                else
                {
                    /**
                     * TCC allocation failed, free the previously allocated
                     * PaRAM Set and DMA channel.
                     */
                    resObj.type = EDMA3_RM_RES_PARAM_SET;
                    resObj.resId = *pParam;
                    EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);

                    /* Reset the book-keeping data structure also. */
                    edma3RmChBoundRes[rmObj->phyCtrllerInstId][chObj->resId].paRAMId = -1;

                    resObj.type = chObj->type;
                    resObj.resId = chObj->resId;
                    EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
                }
            }
            else
            {
                /**
                 * PaRAM Set allocation failed, free the previously allocated
                 * DMA channel also.
                 */
                resObj.type = chObj->type;
                resObj.resId = chObj->resId;
                EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
            }
        }

        /* Further resources' allocation for QDMA channel. */
        if (chObj->type == EDMA3_RM_RES_QDMA_CHANNEL)
        {
            /* First allocate a PaRAM Set */
            resObj.type = EDMA3_RM_RES_PARAM_SET;
            /* Use the saved param id now. */
            resObj.resId = (uint32_t)paRAMId;
            result = EDMA3_RM_allocResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
            if (result == EDMA3_RM_SOK)
            {
                /**
                 * PaRAM Set allocation succeeded.
                 * Save the PaRAM Set first.
                 */
                *pParam = resObj.resId;
                edma3RmChBoundRes[rmObj->phyCtrllerInstId][qdmaChId].paRAMId = (int32_t)(resObj.resId);

                /* Allocate the TCC now. */
                resObj.type = EDMA3_RM_RES_TCC;
                if ((*pTcc) == EDMA3_RM_TCC_ANY)
                {
                    if (mappedTcc == EDMA3_RM_CH_NO_TCC_MAP)
                    {
                        resObj.resId = EDMA3_RM_RES_ANY;
                    }
                    else
                    {
                        resObj.resId = mappedTcc;
                    }
                }
                else
                {
                    resObj.resId = *pTcc;
                }

                result = EDMA3_RM_allocResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
                if (result == EDMA3_RM_SOK)
                {
                    /* TCC allocation succeeded. Save it first. */
                    *pTcc = resObj.resId;
                    edma3RmChBoundRes[rmObj->phyCtrllerInstId][qdmaChId].tcc = resObj.resId;

                    /**
                     * Check first whether the global registers and the allocated
                     * PaRAM Set can be modified or not. If yes, do the needful.
                     * Else leave this for the user.
                     */
                    if (TRUE == rmInstance->regModificationRequired)
                    {
                        /* Set TCC of the allocated Param Set. */
                        gblRegs->PARAMENTRY[*pParam].OPT &= EDMA3_RM_OPT_TCC_CLR_MASK;
                        gblRegs->PARAMENTRY[*pParam].OPT |= EDMA3_RM_OPT_TCC_SET_MASK(*pTcc);

                        /* Do the mapping between QDMA channel and PaRAM Set. */
                        /* Map Parameter RAM Set Number for specified channelId */
                        gblRegs->QCHMAP[chObj->resId] &= EDMA3_RM_QCH_PARAM_CLR_MASK;
                        gblRegs->QCHMAP[chObj->resId] |= EDMA3_RM_QCH_PARAM_SET_MASK(*pParam);

                        /* Set the Trigger Word */
                        gblRegs->QCHMAP[chObj->resId] &= EDMA3_RM_QCH_TRWORD_CLR_MASK;
                        gblRegs->QCHMAP[chObj->resId] |= EDMA3_RM_QCH_TRWORD_SET_MASK(EDMA3_RM_QDMA_TRIG_DEFAULT);

                        /* Remove any linking */
                        *((&gblRegs->PARAMENTRY[*pParam].OPT) + (uint32_t)EDMA3_RM_PARAM_ENTRY_LINK_BCNTRLD) = 0xFFFFU;

                        /* Enable the transfer also. */
                        rmInstance->shadowRegs->QEESR = (1U << chObj->resId);
                    }
                }
                else
                {
                    /**
                     * TCC allocation failed, free the previously allocated
                     * PaRAM Set and QDMA channel.
                     */
                    resObj.type = EDMA3_RM_RES_PARAM_SET;
                    resObj.resId = *pParam;
                    EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);

                    /* Reset the book-keeping data structure also. */
                    edma3RmChBoundRes[rmObj->phyCtrllerInstId][qdmaChId].paRAMId = -1;

                    resObj.type = chObj->type;
                    resObj.resId = chObj->resId;
                    EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
                }
            }
            else
            {
                /**
                 * PaRAM Set allocation failed, free the previously allocated
                 * QDMA channel also.
                 */
                resObj.type = chObj->type;
                resObj.resId = chObj->resId;
                EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
            }
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_freeLogicalChannel(EDMA3_RM_Handle hEdmaResMgr,
                                            EDMA3_RM_ResDesc *lChObj)
{
    EDMA3_RM_ResDesc *chObj;
    EDMA3_RM_ResDesc resObj;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    int32_t paRAMId;
    uint32_t tcc;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    uint32_t qdmaChId;
    uint32_t dmaChId;
    EDMA3_RM_InstanceInitConfig *rmConfig = NULL;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((lChObj == NULL) || (hEdmaResMgr == NULL))
    {
        result = (EDMA3_RM_E_INVALID_PARAM);
    }
#endif

    /* Check if the parameters are OK. */
    if (result == EDMA3_RM_SOK)
    {
        chObj = lChObj;

        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;

        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        rmConfig = rmInstance->initParam.rmInstInitConfig;
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            if (rmObj->gblCfgParams.globalRegs == NULL)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
            else
            {
                edma3Id = rmObj->phyCtrllerInstId;
                globalRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        switch (chObj->type)
        {
        case EDMA3_RM_RES_DMA_CHANNEL:
        {
            /* Save the DMA channel first. */
            dmaChId = chObj->resId;

            /**
                 * Validate DMA channel id first.
                 * It should be a valid channel id.
                 */
            if (dmaChId >= EDMA3_MAX_DMA_CH)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }

            /* It should be owned and allocated by this RM only. */
            if (result == EDMA3_RM_SOK)
            {
                if (((rmConfig->ownDmaChannels[dmaChId / 32U]) &
                     (~(rmInstance->avlblDmaChannels[dmaChId / 32U])) &
                     ((uint32_t)1U << (dmaChId % 32U))) != FALSE)
                {
                    /** Perfectly valid channel id.
                         * Clear some channel specific registers, if it is permitted.
                         */
                    if (TRUE == rmInstance->regModificationRequired)
                    {
                        if (dmaChId < 32U)
                        {
                            if ((rmInstance->shadowRegs->SER & ((uint32_t)1U << dmaChId)) != FALSE)
                            {
                                rmInstance->shadowRegs->SECR = (1U << dmaChId);
                            }
                            if ((globalRegs->EMR & ((uint32_t)1U << dmaChId)) != FALSE)
                            {
                                globalRegs->EMCR = (1U << dmaChId);
                            }
                        }
                        else
                        {
                            if ((rmInstance->shadowRegs->SERH & ((uint32_t)1U << (dmaChId - 32U))) != FALSE)
                            {
                                rmInstance->shadowRegs->SECRH = (1U << (dmaChId - 32U));
                            }
                            if ((globalRegs->EMRH & ((uint32_t)1U << (dmaChId - 32U))) != FALSE)
                            {
                                globalRegs->EMCRH = (1U << (dmaChId - 32U));
                            }
                        }

                        /* Clear DCHMAP register also. */
                        if (TRUE == rmObj->gblCfgParams.dmaChPaRAMMapExists)
                        {
                            globalRegs->DCHMAP[dmaChId] &=
                                EDMA3_RM_DCH_PARAM_CLR_MASK;
                        }
                    }

                    /* Free the PaRAM Set Now. */
                    paRAMId = edma3RmChBoundRes[rmObj->phyCtrllerInstId][dmaChId].paRAMId;
                    resObj.type = EDMA3_RM_RES_PARAM_SET;
                    resObj.resId = (uint32_t)paRAMId;
                    result = EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
                }
                else
                {
                    /* Channel id has some problem. */
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
            }

            if (result == EDMA3_RM_SOK)
            {
                /* PaRAM Set Freed */
                edma3RmChBoundRes[rmObj->phyCtrllerInstId][dmaChId].paRAMId = -1;

                /* Free the TCC */
                tcc = edma3RmChBoundRes[rmObj->phyCtrllerInstId][dmaChId].tcc;
                resObj.type = EDMA3_RM_RES_TCC;
                resObj.resId = tcc;
                result = EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
            }

            if (result == EDMA3_RM_SOK)
            {
                /* TCC Freed */
                edma3RmChBoundRes[rmObj->phyCtrllerInstId][dmaChId].tcc = EDMA3_MAX_TCC;

                /**
                     * Try to free the DMA Channel now. DMA Channel should
                     * be freed only in the end because while freeing, DRAE
                     * registers will be RESET.
                     * After that, no shadow region specific DMA channel
                     * register can be modified. So reset that DRAE register
                     * ONLY in the end.
                     */
                resObj.type = EDMA3_RM_RES_DMA_CHANNEL;
                resObj.resId = dmaChId;
                result = EDMA3_RM_freeResource(hEdmaResMgr,
                                               (EDMA3_RM_ResDesc *)&resObj);
            }
        }
        break;

        case EDMA3_RM_RES_QDMA_CHANNEL:
        {
            /**
                 * Calculate QDMA Logical Channel Id first.
                 * User has given the actual QDMA channel id.
                 * So we have to convert it to make the logical
                 * QDMA channel id first.
                 */
            qdmaChId = chObj->resId + edma3_qdma_ch_min_val[edma3Id];

            /**
                 * Validate QDMA channel id first.
                 * It should be a valid channel id.
                 */
            if (chObj->resId >= EDMA3_MAX_QDMA_CH)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }

            /* It should be owned and allocated by this RM only. */
            if (result == EDMA3_RM_SOK)
            {
                if (((rmConfig->ownQdmaChannels[0U]) &
                     (~(rmInstance->avlblQdmaChannels[0U])) &
                     ((uint32_t)1U << chObj->resId)) != FALSE)
                {
                    /** Perfectly valid channel id.
                         * Clear some channel specific registers, if
                         * it is permitted.
                         */
                    if (TRUE == rmInstance->regModificationRequired)
                    {
                        rmInstance->shadowRegs->QEECR = (1U << chObj->resId);

                        if ((globalRegs->QEMR & ((uint32_t)1U << chObj->resId)) != FALSE)
                        {
                            globalRegs->QEMCR = (1U << chObj->resId);
                        }

                        /* Unmap PARAM Set Number for specified channelId */
                        globalRegs->QCHMAP[chObj->resId] &=
                            EDMA3_RM_QCH_PARAM_CLR_MASK;

                        /* Reset the Trigger Word */
                        globalRegs->QCHMAP[chObj->resId] &=
                            EDMA3_RM_QCH_TRWORD_CLR_MASK;
                    }

                    /* Free the PaRAM Set now */
                    paRAMId = edma3RmChBoundRes[rmObj->phyCtrllerInstId][qdmaChId].paRAMId;
                    resObj.type = EDMA3_RM_RES_PARAM_SET;
                    resObj.resId = (int32_t)paRAMId;
                    result = EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
                }
                else
                {
                    /* Channel id has some problem. */
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
            }

            if (result == EDMA3_RM_SOK)
            {
                /* PaRAM Set Freed */
                edma3RmChBoundRes[rmObj->phyCtrllerInstId][qdmaChId].paRAMId = -1;

                /* Free the TCC */
                tcc = edma3RmChBoundRes[rmObj->phyCtrllerInstId][qdmaChId].tcc;
                resObj.type = EDMA3_RM_RES_TCC;
                resObj.resId = tcc;
                result = EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
            }

            if (result == EDMA3_RM_SOK)
            {
                /* TCC Freed */
                edma3RmChBoundRes[rmObj->phyCtrllerInstId][qdmaChId].tcc = EDMA3_MAX_TCC;

                /**
                     * Try to free the QDMA Channel now. QDMA Channel should
                     * be freed only in the end because while freeing, QRAE
                     * registers will be RESET.
                     * After that, no shadow region specific QDMA channel
                     * register can be modified. So reset that QDRAE register
                     * ONLY in the end.
                     */
                resObj.type = EDMA3_RM_RES_QDMA_CHANNEL;
                resObj.resId = chObj->resId;
                result = EDMA3_RM_freeResource(hEdmaResMgr,
                                               (EDMA3_RM_ResDesc *)&resObj);
            }
        }
        break;

        case EDMA3_RM_RES_PARAM_SET:
        {
            /* Link Channel */
            if (chObj->resId < edma3NumPaRAMSets)
            {
                resObj.type = EDMA3_RM_RES_PARAM_SET;
                resObj.resId = chObj->resId;

                result = EDMA3_RM_freeResource(hEdmaResMgr, (EDMA3_RM_ResDesc *)&resObj);
                if (result == EDMA3_RM_SOK)
                {
                    /* PaRAM Set freed successfully. */
                    uint32_t linkCh = edma3_link_ch_min_val[edma3Id];

                    /* Reset the Logical-Link channel */
                    /* Search for the Logical-Link channel first */
                    for (linkCh = edma3_link_ch_min_val[edma3Id];
                         linkCh < edma3_link_ch_max_val[edma3Id];
                         linkCh++)
                    {
                        if (edma3RmChBoundRes[rmObj->phyCtrllerInstId][linkCh].paRAMId == chObj->resId)
                        {
                            edma3RmChBoundRes[rmObj->phyCtrllerInstId][linkCh].paRAMId = -1;
                            break;
                        }
                    }
                }
            }
            else
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        break;

        default:
        {
            result = EDMA3_RM_E_INVALID_PARAM;
            break;
        }
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_mapEdmaChannel(EDMA3_RM_Handle hEdmaResMgr,
                                        uint32_t channelId,
                                        uint32_t paRAMId)
{
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    volatile EDMA3_CCRL_Regs *gblRegs = NULL;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (hEdmaResMgr == NULL)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_RM_SOK)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;

        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            if (rmObj->gblCfgParams.globalRegs == NULL)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        gblRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);

        /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
        if ((paRAMId >= rmObj->gblCfgParams.numPaRAMSets) || (channelId >= rmObj->gblCfgParams.numDmaChannels))
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
#endif
    }

    /* DMA channel and PaRAM Set should be previously allocated. */
    if (result == EDMA3_RM_SOK)
    {
        if (((channelId / 32U) < 2) && ((paRAMId / 32U) < 16))
        {
            if (((rmInstance->initParam.rmInstInitConfig->ownDmaChannels[channelId / 32U]) &
                 (~(rmInstance->avlblDmaChannels[channelId / 32U])) &
                 ((uint32_t)1U << (channelId % 32U))) != FALSE)
            {
                /* DMA channel allocated, check for the PaRAM Set */
                if (((rmInstance->initParam.rmInstInitConfig->ownPaRAMSets[paRAMId / 32U]) &
                     (~(rmInstance->avlblPaRAMSets[paRAMId / 32U])) &
                     ((uint32_t)1U << (paRAMId % 32U))) == FALSE)
                {
                    /* PaRAM Set NOT allocated, return error */
                    result = EDMA3_RM_E_RES_NOT_ALLOCATED;
                }
            }
            else
            {
                /* DMA channel NOT allocated, return error */
                result = EDMA3_RM_E_RES_NOT_ALLOCATED;
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        /* Map the Dma Channel to the PaRAM Set corresponding to paramId */
        /**
          * Do this for the EDMA3 Controllers which have a register for mapping
          * DMA Channel to a particular PaRAM Set. So check
          * dmaChPaRAMMapExists first.
          */
        if (TRUE == rmObj->gblCfgParams.dmaChPaRAMMapExists)
        {
            /* Map Parameter RAM Set Number for specified channelId */
            gblRegs->DCHMAP[channelId] &= EDMA3_RM_DCH_PARAM_CLR_MASK;
            gblRegs->DCHMAP[channelId] |= EDMA3_RM_DCH_PARAM_SET_MASK(paRAMId);
        }
        else
        {
            /* Feature NOT supported on the current platform, return error. */
            result = EDMA3_RM_E_FEATURE_UNSUPPORTED;
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_mapQdmaChannel(EDMA3_RM_Handle hEdmaResMgr,
                                        uint32_t channelId,
                                        uint32_t paRAMId,
                                        EDMA3_RM_QdmaTrigWord trigWord)
{
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    volatile EDMA3_CCRL_Regs *gblRegs = NULL;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((hEdmaResMgr == NULL) || (((int32_t)trigWord < (int32_t)EDMA3_RM_QDMA_TRIG_OPT) || (trigWord > EDMA3_RM_QDMA_TRIG_CCNT)))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_RM_SOK)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;

        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            if (rmObj->gblCfgParams.globalRegs == NULL)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        gblRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);

        /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
        if ((paRAMId >= rmObj->gblCfgParams.numPaRAMSets) || (channelId >= rmObj->gblCfgParams.numQdmaChannels))
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
#endif
    }

    /* QDMA channel and PaRAM Set should be previously allocated. */
    if (result == EDMA3_RM_SOK)
    {
        if (((channelId / 32U) < 1) && ((paRAMId / 32U) < 16))
        {
            if (((rmInstance->initParam.rmInstInitConfig->ownQdmaChannels[channelId / 32U]) &
                 (~(rmInstance->avlblQdmaChannels[channelId / 32U])) &
                 ((uint32_t)1U << (channelId % 32U))) != FALSE)
            {
                /* QDMA channel allocated, check for the PaRAM Set */
                if (((rmInstance->initParam.rmInstInitConfig->ownPaRAMSets[paRAMId / 32U]) &
                     (~(rmInstance->avlblPaRAMSets[paRAMId / 32U])) &
                     ((uint32_t)1U << (paRAMId % 32U))) == FALSE)
                {
                    /* PaRAM Set NOT allocated, return error */
                    result = EDMA3_RM_E_RES_NOT_ALLOCATED;
                }
            }
            else
            {
                /* QDMA channel NOT allocated, return error */
                result = EDMA3_RM_E_RES_NOT_ALLOCATED;
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        /* Map Parameter RAM Set Number for specified channelId */
        gblRegs->QCHMAP[channelId] &= EDMA3_RM_QCH_PARAM_CLR_MASK;
        gblRegs->QCHMAP[channelId] |= EDMA3_RM_QCH_PARAM_SET_MASK(paRAMId);

        /* Set the Trigger Word */
        gblRegs->QCHMAP[channelId] &= EDMA3_RM_QCH_TRWORD_CLR_MASK;
        gblRegs->QCHMAP[channelId] |= EDMA3_RM_QCH_TRWORD_SET_MASK(trigWord);
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_registerTccCb(EDMA3_RM_Handle hEdmaResMgr,
                                       const EDMA3_RM_ResDesc *channelObj,
                                       uint32_t tcc,
                                       EDMA3_RM_TccCallback tccCb,
                                       void *cbData)
{
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    uint32_t edma3Id;
    volatile EDMA3_CCRL_Regs *gblRegs = NULL;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((NULL == hEdmaResMgr) || (NULL == channelObj))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }

    /* Callback function should NOT be NULL */
    if (NULL == tccCb)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_RM_SOK)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = rmObj->phyCtrllerInstId;
            gblRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        if ((gblRegs == NULL) || (tcc >= rmObj->gblCfgParams.numTccs))
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        /* Check whether the callback has already registered. */
        if (NULL != edma3IntrParams[edma3Id][tcc].tccCb)
        {
            result = EDMA3_RM_E_CALLBACK_ALREADY_REGISTERED;
        }
        else
        {
            /* Store the mapping b/w DMA/QDMA channel and TCC first. */
            if (channelObj->type == EDMA3_RM_RES_DMA_CHANNEL)
            {
                /* DMA channel */
                if (channelObj->resId < rmObj->gblCfgParams.numDmaChannels)
                {
                    /* Save the TCC */
                    edma3DmaChTccMapping[edma3Id][channelObj->resId] = tcc;
                }
                else
                {
                    /* Error!!! */
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
            }
            else
            {
                if (channelObj->type == EDMA3_RM_RES_QDMA_CHANNEL)
                {
                    /* QDMA channel */
                    if (channelObj->resId < rmObj->gblCfgParams.numQdmaChannels)
                    {
                        /* Save the TCC */
                        edma3QdmaChTccMapping[edma3Id][channelObj->resId] = tcc;
                    }
                    else
                    {
                        /* Error!!! */
                        result = EDMA3_RM_E_INVALID_PARAM;
                    }
                }
                else
                {
                    /* Error!!! */
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
            }
        }
    }

    if (EDMA3_RM_SOK == result)
    {

        /* Enable the interrupts in IESR/IESRH */
        if (tcc < 32U)
        {
            rmInstance->shadowRegs->IESR = (1UL << tcc);
        }
        else
        {
            rmInstance->shadowRegs->IESRH = (1UL << (tcc - 32U));
        }

        /* Save the callback functions also */
        edma3IntrParams[edma3Id][tcc].cbData = cbData;
        edma3IntrParams[edma3Id][tcc].tccCb = tccCb;
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_unregisterTccCb(EDMA3_RM_Handle hEdmaResMgr,
                                         const EDMA3_RM_ResDesc *channelObj)
{
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    uint32_t mappedTcc = EDMA3_MAX_TCC;
    uint32_t edma3Id;
    volatile EDMA3_CCRL_Regs *gblRegs = NULL;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((NULL == hEdmaResMgr) || (NULL == channelObj))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = rmObj->phyCtrllerInstId;
            gblRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        if (gblRegs == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            if (channelObj->type == EDMA3_RM_RES_DMA_CHANNEL)
            {
                /* DMA channel */
                if (channelObj->resId < rmObj->gblCfgParams.numDmaChannels)
                {
                    /* Save the mapped TCC */
                    mappedTcc = edma3DmaChTccMapping[edma3Id][channelObj->resId];

                    /* Remove the mapping now. */
                    edma3DmaChTccMapping[edma3Id][channelObj->resId] = EDMA3_MAX_TCC;
                }
                else
                {
                    /* Error!!! */
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
            }
            else
            {
                if (channelObj->type == EDMA3_RM_RES_QDMA_CHANNEL)
                {
                    /* QDMA channel */
                    if (channelObj->resId < rmObj->gblCfgParams.numQdmaChannels)
                    {
                        /* Save the mapped TCC */
                        mappedTcc = edma3QdmaChTccMapping[edma3Id][channelObj->resId];

                        /* Remove the mapping now. */
                        edma3QdmaChTccMapping[edma3Id][channelObj->resId] = EDMA3_MAX_TCC;
                    }
                    else
                    {
                        /* Error!!! */
                        result = EDMA3_RM_E_INVALID_PARAM;
                    }
                }
                else
                {
                    /* Error!!! */
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
            }
        }
    }

    if (EDMA3_RM_SOK == result)
    {

        /* Remove the callback function too */
        if (mappedTcc < 32U)
        {
            rmInstance->shadowRegs->IECR = (1UL << mappedTcc);
        }
        else if (mappedTcc < 64U)
        {
            rmInstance->shadowRegs->IECRH = (1UL << (mappedTcc - 32U));
        }
        else
        {
            /* To Comply MISRA C warning */
            result = EDMA3_RM_SOK;
        }

        if (mappedTcc < EDMA3_MAX_TCC)
        {
            edma3IntrParams[edma3Id][mappedTcc].cbData = NULL;
            edma3IntrParams[edma3Id][mappedTcc].tccCb = NULL;
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_allocContiguousResource(EDMA3_RM_Handle hEdmaResMgr,
                                                 EDMA3_RM_ResDesc *firstResIdObj,
                                                 uint32_t numResources)
{
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_ResDesc *resObj = NULL;
    uint32_t resAllocIdx = 0U;
    uint32_t firstResId;
    uint32_t lastResId = 0U;
    uint32_t maxNumResources = 0U;
    EDMA3_RM_Result semResult = EDMA3_RM_SOK;
    uint32_t resIdClr = 0x0;
    uint32_t resIdSet = 0x0;
    volatile EDMA3_CCRL_Regs *gblRegs = NULL;
    uint32_t i = 0U;
    uint32_t position = 0U;
    uint32_t edma3Id;
    uint32_t errFlag = 0U;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((hEdmaResMgr == NULL) || (firstResIdObj == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    if (EDMA3_RM_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (EDMA3_RM_SOK == result)
    {
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (EDMA3_RM_SOK == result)
    {
        edma3Id = rmObj->phyCtrllerInstId;
        gblRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);

        if (rmInstance->initParam.rmSemHandle == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (EDMA3_RM_SOK == result)
    {
        resObj = firstResIdObj;
        if (resObj != NULL)
        {
            firstResId = resObj->resId;
        }

        switch (resObj->type)
        {
        case EDMA3_RM_RES_DMA_CHANNEL:
            maxNumResources = rmObj->gblCfgParams.numDmaChannels;
            break;
        case EDMA3_RM_RES_QDMA_CHANNEL:
            maxNumResources = rmObj->gblCfgParams.numQdmaChannels;
            break;
        case EDMA3_RM_RES_TCC:
            maxNumResources = rmObj->gblCfgParams.numTccs;
            break;
        case EDMA3_RM_RES_PARAM_SET:
            maxNumResources = rmObj->gblCfgParams.numPaRAMSets;
            break;
        default:
            result = EDMA3_RM_E_INVALID_PARAM;
            break;
        }
    }

    if (EDMA3_RM_SOK == result)
    {
        /* First resource id (firstResId) can be a valid Resource ID as well as
         * 'EDMA3_RM_RES_ANY', in case user does not want to
         * start from a specific resource. For eg, user allocating link channels.
         */
        if (firstResId != EDMA3_RM_RES_ANY)
        {
            /* User want specific resources. */
            lastResId = firstResId + numResources;

            if (((firstResId >= maxNumResources) || (firstResId > lastResId)) || (lastResId > maxNumResources))
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        else
        {
            /* (firstResId == EDMA3_RM_RES_ANY)
             * So just check whether the number of resources
             * requested does not cross the limit.
             */
            if (numResources > maxNumResources)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        /* Now try to allocate resources for the first case */
        if (firstResId != EDMA3_RM_RES_ANY)
        {
            /* Request for specific resources */

            /**
              * Take the instance specific semaphore, to prevent simultaneous
              * access to the shared resources.
              */
            semResult = edma3OsSemTake(rmInstance->initParam.rmSemHandle,
                                       EDMA3_OSSEM_NO_TIMEOUT);

            if (EDMA3_RM_SOK == semResult)
            {
                switch (resObj->type)
                {
                case EDMA3_RM_RES_DMA_CHANNEL:
                {
                    for (resAllocIdx = firstResId; resAllocIdx < lastResId; ++resAllocIdx)
                    {
                        resIdClr = (uint32_t)(~((uint32_t)1U << (resAllocIdx % 32U)));
                        resIdSet = (1U << (resAllocIdx % 32U));

                        /* Check whether it is owned or not */
                        if (((resAllocIdx / 32U) < 2) && (((rmInstance->initParam.rmInstInitConfig->ownDmaChannels[resAllocIdx / 32U]) & (resIdSet)) != FALSE))
                        {
                            /* Now check if specified resource is available presently*/
                            if (((rmInstance->avlblDmaChannels[resAllocIdx / 32U]) & (resIdSet)) != FALSE)
                            {
                                /*
                                         * Mark the specified resource as "Not Available"
                                         * for future requests
                                         */
                                rmInstance->avlblDmaChannels[resAllocIdx / 32U] &= resIdClr;

                                if (resAllocIdx < 32U)
                                {
                                    rmInstance->shadowRegs->EECR = (1UL << resAllocIdx);

                                    /**
                                             * Enable the DMA channel in the
                                             * DRAE registers also.
                                             */
                                    gblRegs->DRA[rmInstance->initParam.regionId].DRAE |= ((uint32_t)0x1U << resAllocIdx);
                                }
                                else
                                {
                                    rmInstance->shadowRegs->EECRH = (1UL << (resAllocIdx - 32U));

                                    /**
                                             * Enable the DMA channel in the
                                             * DRAEH registers also.
                                             */
                                    gblRegs->DRA[rmInstance->initParam.regionId].DRAEH |= ((uint32_t)0x1U << (resAllocIdx - 32U));
                                }

                                result = EDMA3_RM_SOK;
                            }
                            else
                            {
                                /* Specified resource is owned but is already booked */
                                result = EDMA3_RM_E_SPECIFIED_RES_NOT_AVAILABLE;
                                errFlag = 1U;
                            }
                        }
                        else
                        {
                            /*
                                     * Specified resource is not owned by this instance
                                     * of the Resource Manager
                                     */
                            result = EDMA3_RM_E_RES_NOT_OWNED;
                            errFlag = 1U;
                        }
                        if (errFlag == 1U)
                        {
                            break;
                        }
                    }

                    break;
                }

                case EDMA3_RM_RES_QDMA_CHANNEL:
                {
                    for (resAllocIdx = firstResId; resAllocIdx < lastResId; ++resAllocIdx)
                    {
                        resIdClr = (uint32_t)(~((uint32_t)1U << resAllocIdx));
                        resIdSet = (1U << resAllocIdx);

                        /* Check whether it is owned or not */
                        if (((rmInstance->initParam.rmInstInitConfig->ownQdmaChannels[0U]) & (resIdSet)) != FALSE)
                        {
                            /* Now check if specified resource is available presently*/
                            if (((rmInstance->avlblQdmaChannels[0U]) & (resIdSet)) != FALSE)
                            {
                                /*
                                         * Mark the specified resource as "Not Available"
                                         * for future requests
                                         */
                                rmInstance->avlblQdmaChannels[0U] &= resIdClr;

                                /**
                                         * Enable the QDMA channel in the
                                         * QRAE register also.
                                         */
                                gblRegs->QRAE[rmInstance->initParam.regionId] |= ((uint32_t)0x1U << resAllocIdx);

                                result = EDMA3_RM_SOK;
                            }
                            else
                            {
                                /* Specified resource is owned but is already booked */
                                result = EDMA3_RM_E_SPECIFIED_RES_NOT_AVAILABLE;
                                errFlag = 1U;
                            }
                        }
                        else
                        {
                            /*
                                     * Specified resource is not owned by this instance
                                     * of the Resource Manager
                                     */
                            result = EDMA3_RM_E_RES_NOT_OWNED;
                            errFlag = 1U;
                        }
                        if (errFlag == 1U)
                        {
                            break;
                        }
                    }

                    break;
                }

                case EDMA3_RM_RES_TCC:
                {
                    for (resAllocIdx = firstResId; resAllocIdx < lastResId; ++resAllocIdx)
                    {
                        resIdClr = (uint32_t)(~((uint32_t)1U << (resAllocIdx % 32U)));
                        resIdSet = (1U << (resAllocIdx % 32U));

                        /* Check whether it is owned or not */
                        if (((resAllocIdx / 32U) < 2) && (((rmInstance->initParam.rmInstInitConfig->ownTccs[resAllocIdx / 32U]) & (resIdSet)) != FALSE))
                        {
                            /* Now check if specified resource is available presently*/
                            if (((rmInstance->avlblTccs[resAllocIdx / 32U]) & (resIdSet)) != FALSE)
                            {
                                /*
                                         * Mark the specified resource as "Not Available"
                                         * for future requests
                                         */
                                rmInstance->avlblTccs[resAllocIdx / 32U] &= resIdClr;

                                /**
                                         * If the region id coming from this
                                         * RM instance is same as the Master RM
                                         * Instance's region id, only then we will be
                                         * getting the interrupts on the same side.
                                         * So save the TCC in the allocatedTCCs[] array.
                                         */
                                if (edma3RegionId == rmInstance->initParam.regionId)
                                {
                                    if (resAllocIdx < 32U)
                                    {
                                        allocatedTCCs[edma3Id][0U] |= ((uint32_t)0x1U << resAllocIdx);
                                    }
                                    else
                                    {
                                        allocatedTCCs[edma3Id][1U] |= ((uint32_t)0x1U << (resAllocIdx - 32U));
                                    }
                                }
                                result = EDMA3_RM_SOK;
                            }
                            else
                            {
                                /* Specified resource is owned but is already booked */
                                result = EDMA3_RM_E_SPECIFIED_RES_NOT_AVAILABLE;
                                errFlag = 1U;
                            }
                        }
                        else
                        {
                            /*
                                     * Specified resource is not owned by this instance
                                     * of the Resource Manager
                                     */
                            result = EDMA3_RM_E_RES_NOT_OWNED;
                            errFlag = 1U;
                        }
                        if (errFlag == 1U)
                        {
                            break;
                        }
                    }

                    break;
                }

                case EDMA3_RM_RES_PARAM_SET:
                {
                    for (resAllocIdx = firstResId; resAllocIdx < lastResId; ++resAllocIdx)
                    {
                        resIdClr = (uint32_t)(~((uint32_t)1U << (resAllocIdx % 32U)));
                        resIdSet = (1U << (resAllocIdx % 32U));

                        /* Check whether it is owned or not */
                        if (((rmInstance->initParam.rmInstInitConfig->ownPaRAMSets[resAllocIdx / 32U]) & (resIdSet)) != FALSE)
                        {
                            /* Now check if specified resource is available presently*/
                            if (((resAllocIdx / 32U) < 16) && (((rmInstance->avlblPaRAMSets[resAllocIdx / 32U]) & (resIdSet)) != FALSE))
                            {
                                /*
                                         * Mark the specified resource as "Not Available"
                                         * for future requests
                                         */
                                rmInstance->avlblPaRAMSets[resAllocIdx / 32U] &= resIdClr;

                                /**
                                         * Also, make the actual PARAM Set NULL, checking the flag
                                         * whether it is required or not.
                                         */
                                if (TRUE == rmInstance->paramInitRequired)
                                {
                                    edma3MemZero((void *)(&gblRegs->PARAMENTRY[resAllocIdx]),
                                                 sizeof(gblRegs->PARAMENTRY[resAllocIdx]));
                                }

                                result = EDMA3_RM_SOK;
                            }
                            else
                            {
                                /* Specified resource is owned but is already booked */
                                result = EDMA3_RM_E_SPECIFIED_RES_NOT_AVAILABLE;
                                errFlag = 1U;
                            }
                        }
                        else
                        {
                            /*
                                     * Specified resource is not owned by this instance
                                     * of the Resource Manager
                                     */
                            result = EDMA3_RM_E_RES_NOT_OWNED;
                            errFlag = 1U;
                        }
                        if (errFlag == 1U)
                        {
                            break;
                        }
                    }

                    break;
                }

                default:
                {
                    result = EDMA3_RM_E_INVALID_PARAM;
                    break;
                }
                }

                /* resource allocation completed, release the semaphore first */
                semResult = edma3OsSemGive(rmInstance->initParam.rmSemHandle);
            }
        }
        else
        {
            /* (firstResId == EDMA3_RM_RES_ANY) */
            /**
            * Take the instance specific semaphore, to prevent simultaneous
            * access to the shared resources.
            */
            semResult = edma3OsSemTake(rmInstance->initParam.rmSemHandle,
                                       EDMA3_OSSEM_NO_TIMEOUT);

            if (EDMA3_RM_SOK == semResult)
            {
                /**
                * We have to search three different arrays, namely ownedResoures,
                * avlblResources and resvdResources, to find the 'common' contiguous
                * resources. For this, take an 'AND' of all three arrays in one single
                * array and use your algorithm on that array.
                */
                switch (resObj->type)
                {
                case EDMA3_RM_RES_DMA_CHANNEL:
                {
                    /* AND all the arrays to look into */
                    contiguousDmaRes[0U] = ((rmInstance->initParam.rmInstInitConfig->ownDmaChannels[0U] & rmInstance->avlblDmaChannels[0U]) & (~(rmInstance->initParam.rmInstInitConfig->resvdDmaChannels[0U])));
                    contiguousDmaRes[1U] = ((rmInstance->initParam.rmInstInitConfig->ownDmaChannels[1U] & rmInstance->avlblDmaChannels[1U]) & (~(rmInstance->initParam.rmInstInitConfig->resvdDmaChannels[1U])));
                }
                break;

                case EDMA3_RM_RES_QDMA_CHANNEL:
                {
                    /* AND all the arrays to look into */
                    contiguousQdmaRes[0U] = ((rmInstance->initParam.rmInstInitConfig->ownQdmaChannels[0U] & rmInstance->avlblQdmaChannels[0U]) & (~(rmInstance->initParam.rmInstInitConfig->resvdQdmaChannels[0U])));
                }
                break;

                case EDMA3_RM_RES_TCC:
                {
                    /* AND all the arrays to look into */
                    contiguousTccRes[0U] = ((rmInstance->initParam.rmInstInitConfig->ownTccs[0U] & rmInstance->avlblTccs[0U]) & (~(rmInstance->initParam.rmInstInitConfig->resvdTccs[0U])));
                    contiguousTccRes[1U] = ((rmInstance->initParam.rmInstInitConfig->ownTccs[1U] & rmInstance->avlblTccs[1U]) & (~(rmInstance->initParam.rmInstInitConfig->resvdTccs[1U])));
                }
                break;

                case EDMA3_RM_RES_PARAM_SET:
                {
                    /* AND all the arrays to look into */
                    for (i = 0U; i < (maxNumResources / 32U); ++i)
                    {
                        contiguousParamRes[i] = ((rmInstance->initParam.rmInstInitConfig->ownPaRAMSets[i] & rmInstance->avlblPaRAMSets[i]) & (~(rmInstance->initParam.rmInstInitConfig->resvdPaRAMSets[i])));
                    }
                }
                break;

                default:
                {
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
                break;
                }

                if (EDMA3_RM_SOK == result)
                {
                    /**
                     * Try to allocate 'numResources' contiguous resources
                     * of type RES_ANY.
                     */
                    result = allocAnyContigRes(resObj->type, numResources, &position);

                    /**
                    * If result != EDMA3_RM_SOK, resource allocation failed.
                    * Else resources successfully allocated.
                    */
                    if (result == EDMA3_RM_SOK)
                    {
                        /* Update the first resource id with the position returned. */
                        resObj->resId = position;

                        /*
                         * Do some further changes in the book-keeping
                         * data structures and global registers accordingly.
                         */
                        result = gblChngAllocContigRes(rmInstance, resObj, numResources);
                    }
                }

                /* resource allocation completed, release the semaphore first */
                semResult = edma3OsSemGive(rmInstance->initParam.rmSemHandle);
            }
        }
    }

    /**
     * Check the Resource Allocation Result 'result' first. If Resource
     * Allocation has resulted in an error, return it (having more priority than
     * semResult. Else, return semResult.
     */
    if (EDMA3_RM_SOK == result)
    {
        /**
          * Resource Allocation successful, return semResult for returning
          * semaphore.
          */
        result = semResult;
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_freeContiguousResource(EDMA3_RM_Handle hEdmaResMgr,
                                                EDMA3_RM_ResDesc *firstResIdObj,
                                                uint32_t numResources)
{
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_ResDesc *resObj;
    uint32_t resFreeIdx = 0U;
    uint32_t firstResId;
    uint32_t lastResId;
    uint32_t maxNumResources = 0U;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((hEdmaResMgr == NULL) || (firstResIdObj == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            resObj = firstResIdObj;
            if (resObj != NULL)
            {
                firstResId = resObj->resId;
                lastResId = firstResId + (numResources - 1U);
            }

            switch (resObj->type)
            {
            case EDMA3_RM_RES_DMA_CHANNEL:
                maxNumResources = rmObj->gblCfgParams.numDmaChannels;
                break;
            case EDMA3_RM_RES_QDMA_CHANNEL:
                maxNumResources = rmObj->gblCfgParams.numQdmaChannels;
                break;
            case EDMA3_RM_RES_TCC:
                maxNumResources = rmObj->gblCfgParams.numTccs;
                break;
            case EDMA3_RM_RES_PARAM_SET:
                maxNumResources = rmObj->gblCfgParams.numPaRAMSets;
                break;
            default:
                result = EDMA3_RM_E_INVALID_PARAM;
                break;
            }

            if (result == EDMA3_RM_SOK)
            {
                if ((firstResId > lastResId) || (lastResId >= maxNumResources))
                {
                    result = EDMA3_RM_E_INVALID_PARAM;
                }
                else
                {
                    for (resFreeIdx = firstResId; resFreeIdx <= lastResId; ++resFreeIdx)
                    {
                        resObj->resId = resFreeIdx;
                        result = EDMA3_RM_freeResource(rmInstance, resObj);

                        if (result != EDMA3_RM_SOK)
                        {
                            break;
                        }
                    }
                }
            }
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_setCCRegister(EDMA3_RM_Handle hEdmaResMgr,
                                       uint32_t regOffset,
                                       uint32_t newRegValue)
{
    uint32_t intState;
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    volatile uint32_t regPhyAddr = 0x0U;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((hEdmaResMgr == NULL) || ((regOffset % 4U) != 0))
    {
        result = (EDMA3_RM_E_INVALID_PARAM);
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = (EDMA3_RM_E_INVALID_PARAM);
        }
        else
        {
            if (rmObj->gblCfgParams.globalRegs != NULL)
            {
                /**
                  * Take the instance specific semaphore, to prevent simultaneous
                  * access to the shared resources.
                  */
                result = edma3OsSemTake(rmInstance->initParam.rmSemHandle,
                                        EDMA3_OSSEM_NO_TIMEOUT);

                if (EDMA3_RM_SOK == result)
                {
                    /* Semaphore taken successfully, modify the registers. */
                    edma3OsProtectEntry(rmObj->phyCtrllerInstId,
                                        (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                        &intState);
                    /* Global interrupts disabled, modify the registers. */
                    regPhyAddr = (uint32_t)(rmObj->gblCfgParams.globalRegs) + regOffset;

                    *(uint32_t *)regPhyAddr = newRegValue;
                    edma3OsProtectExit(rmObj->phyCtrllerInstId,
                                       (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                       intState);

                    /* Return the semaphore back */
                    result = edma3OsSemGive(rmInstance->initParam.rmSemHandle);
                }
            }
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_getCCRegister(EDMA3_RM_Handle hEdmaResMgr,
                                       uint32_t regOffset,
                                       uint32_t *regValue)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    volatile uint32_t regPhyAddr = 0x0U;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (((hEdmaResMgr == NULL) || (regValue == NULL)) || ((regOffset % 4U) != 0))
    {
        result = (EDMA3_RM_E_INVALID_PARAM);
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = (EDMA3_RM_E_INVALID_PARAM);
        }
        else
        {
            if (rmObj->gblCfgParams.globalRegs != NULL)
            {
                regPhyAddr = (uint32_t)(rmObj->gblCfgParams.globalRegs) + regOffset;

                *regValue = *(uint32_t *)regPhyAddr;
            }
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_waitAndClearTcc(EDMA3_RM_Handle hEdmaResMgr,
                                         uint32_t tccNo)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    volatile EDMA3_CCRL_ShadowRegs *shadowRegs = NULL;
    uint32_t tccBitMask = 0x0U;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (hEdmaResMgr == NULL)
    {
        result = (EDMA3_RM_E_INVALID_PARAM);
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        rmObj = rmInstance->pResMgrObjHandle;

        if ((rmObj == NULL) || (rmObj->gblCfgParams.globalRegs == NULL))
        {
            result = (EDMA3_RM_E_INVALID_PARAM);
        }
        else
        {
            /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
            if (tccNo >= rmObj->gblCfgParams.numTccs)
            {
                result = (EDMA3_RM_E_INVALID_PARAM);
            }
#endif

            /* Check if the parameters are OK. */
            if (EDMA3_RM_SOK == result)
            {
                globalRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);
                shadowRegs = (volatile EDMA3_CCRL_ShadowRegs *)(&globalRegs->SHADOW[rmInstance->initParam.regionId]);

                if (shadowRegs != NULL)
                {
                    if (tccNo < 32U)
                    {
                        tccBitMask = (1U << tccNo);

                        /* Check the status of the IPR[tccNo] bit. */
                        while ((uint32_t)FALSE == (shadowRegs->IPR & tccBitMask))
                        {
                            /* Transfer not yet completed, bit not SET */
                        }

                        /**
                         * Bit found SET, transfer is completed,
                         * clear the pending interrupt and return.
                         */
                        shadowRegs->ICR = tccBitMask;
                    }
                    else
                    {
                        tccBitMask = (1U << (tccNo - 32U));

                        /* Check the status of the IPRH[tccNo-32] bit. */
                        while ((uint32_t)FALSE == (shadowRegs->IPRH & tccBitMask))
                        {
                            /* Transfer not yet completed, bit not SET */
                        }

                        /**
                         * Bit found SET, transfer is completed,
                         * clear the pending interrupt and return.
                         */
                        shadowRegs->ICRH = tccBitMask;
                    }
                }
            }
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_checkAndClearTcc(EDMA3_RM_Handle hEdmaResMgr,
                                          uint32_t tccNo,
                                          uint16_t *tccStatus)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    volatile EDMA3_CCRL_ShadowRegs *shadowRegs = NULL;
    uint32_t tccBitMask = 0x0U;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((hEdmaResMgr == NULL) || (tccStatus == NULL))
    {
        result = (EDMA3_RM_E_INVALID_PARAM);
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_RM_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;
        rmObj = rmInstance->pResMgrObjHandle;

        if ((rmObj == NULL) || (rmObj->gblCfgParams.globalRegs == NULL))
        {
            result = (EDMA3_RM_E_INVALID_PARAM);
        }
        else
        {
            /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
            if (tccNo >= rmObj->gblCfgParams.numTccs)
            {
                result = (EDMA3_RM_E_INVALID_PARAM);
            }
#endif

            /* Check if the parameters are OK. */
            if (EDMA3_RM_SOK == result)
            {
                globalRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);
                shadowRegs = (volatile EDMA3_CCRL_ShadowRegs *)(&globalRegs->SHADOW[rmInstance->initParam.regionId]);

                /* Reset the tccStatus */
                *tccStatus = FALSE;

                if (shadowRegs != NULL)
                {
                    if (tccNo < 32U)
                    {
                        tccBitMask = (1U << tccNo);

                        /* Check the status of the IPR[tccNo] bit. */
                        if ((shadowRegs->IPR & tccBitMask) != FALSE)
                        {
                            /* Transfer completed, bit found SET */
                            *tccStatus = TRUE;

                            /* Clear the pending interrupt also. */
                            shadowRegs->ICR = tccBitMask;
                        }
                    }
                    else
                    {
                        tccBitMask = (1U << (tccNo - 32U));

                        /* Check the status of the IPRH[tccNo-32] bit. */
                        if ((shadowRegs->IPRH & tccBitMask) != FALSE)
                        {
                            /* Transfer completed, bit found SET */
                            *tccStatus = TRUE;

                            /* Clear the pending interrupt also. */
                            shadowRegs->ICRH = tccBitMask;
                        }
                    }
                }
            }
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_setPaRAM(EDMA3_RM_Handle hEdmaResMgr,
                                  const EDMA3_RM_ResDesc *lChObj,
                                  const EDMA3_RM_PaRAMRegs *newPaRAM)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    int32_t paRAMId = 0;
    uint32_t qdmaChId = 0U;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (hEdmaResMgr == NULL)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }

    if ((lChObj == NULL) || (newPaRAM == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (result == EDMA3_RM_SOK)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;

        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            if (rmObj->gblCfgParams.globalRegs == NULL)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        edma3Id = rmObj->phyCtrllerInstId;
        globalRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);

        switch (lChObj->type)
        {
        case EDMA3_RM_RES_DMA_CHANNEL:
        {
            if (lChObj->resId <= edma3_dma_ch_max_val[edma3Id])
            {
                paRAMId = edma3RmChBoundRes[edma3Id][lChObj->resId].paRAMId;
            }
            else
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        break;

        case EDMA3_RM_RES_QDMA_CHANNEL:
        {
            if (lChObj->resId < EDMA3_MAX_QDMA_CH)
            {
                qdmaChId = lChObj->resId + edma3_qdma_ch_min_val[edma3Id];
                paRAMId = edma3RmChBoundRes[edma3Id][qdmaChId].paRAMId;
            }
            else
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        break;

        case EDMA3_RM_RES_PARAM_SET:
        {
            if (lChObj->resId < edma3NumPaRAMSets)
            {
                /**
                     * User has passed the actual param set value here.
                     * Use this value only
                     */
                paRAMId = (int32_t)(lChObj->resId);
            }
            else
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        break;

        default:
        {
            result = EDMA3_RM_E_INVALID_PARAM;
            break;
        }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        /* Check the param id first. */
        if ((paRAMId != -1) && ((uint32_t)paRAMId < edma3NumPaRAMSets))
        {
            /* Set the PaRAM Set now. */
            edma3ParamCpy((volatile void *)(&(globalRegs->PARAMENTRY[paRAMId].OPT)),
                          (const void *)newPaRAM);
        }
        else
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_getPaRAM(EDMA3_RM_Handle hEdmaResMgr,
                                  const EDMA3_RM_ResDesc *lChObj,
                                  EDMA3_RM_PaRAMRegs *currPaRAM)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    int32_t paRAMId = 0;
    uint32_t qdmaChId = 0U;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (hEdmaResMgr == NULL)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }

    if ((lChObj == NULL) || (currPaRAM == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (result == EDMA3_RM_SOK)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;

        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            if (rmObj->gblCfgParams.globalRegs == NULL)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        edma3Id = rmObj->phyCtrllerInstId;
        globalRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);

        switch (lChObj->type)
        {
        case EDMA3_RM_RES_DMA_CHANNEL:
        {
            if (lChObj->resId <= edma3_dma_ch_max_val[edma3Id])
            {
                paRAMId = edma3RmChBoundRes[rmObj->phyCtrllerInstId][lChObj->resId].paRAMId;
            }
            else
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        break;

        case EDMA3_RM_RES_QDMA_CHANNEL:
        {
            if (lChObj->resId < EDMA3_MAX_QDMA_CH)
            {
                qdmaChId = lChObj->resId + edma3_qdma_ch_min_val[edma3Id];
                paRAMId = edma3RmChBoundRes[rmObj->phyCtrllerInstId][qdmaChId].paRAMId;
            }
            else
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        break;

        case EDMA3_RM_RES_PARAM_SET:
        {
            if (lChObj->resId < edma3NumPaRAMSets)
            {
                /**
                     * User has passed the actual param set value here.
                     * Use this value only
                     */
                paRAMId = (int32_t)(lChObj->resId);
            }
            else
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        break;

        default:
        {
            result = EDMA3_RM_E_INVALID_PARAM;
            break;
        }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        /* Check the param id first. */
        if ((paRAMId != -1) && ((uint32_t)paRAMId < edma3NumPaRAMSets))
        {
            /* Get the PaRAM Set now. */
            edma3ParamCpy((void *)currPaRAM,
                          (const volatile void *)(&(globalRegs->PARAMENTRY[paRAMId].OPT)));
        }
        else
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_getPaRAMPhyAddr(EDMA3_RM_Handle hEdmaResMgr,
                                         const EDMA3_RM_ResDesc *lChObj,
                                         uint32_t *paramPhyAddr)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    int32_t paRAMId = 0;
    uint32_t qdmaChId = 0U;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (hEdmaResMgr == NULL)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }

    if ((lChObj == NULL) || (paramPhyAddr == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_RM_SOK)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;

        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
        else
        {
            if (rmObj->gblCfgParams.globalRegs == NULL)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        edma3Id = rmObj->phyCtrllerInstId;
        globalRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);

        switch (lChObj->type)
        {
        case EDMA3_RM_RES_DMA_CHANNEL:
        {
            if (lChObj->resId <= edma3_dma_ch_max_val[edma3Id])
            {
                paRAMId = edma3RmChBoundRes[edma3Id][lChObj->resId].paRAMId;
            }
            else
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        break;

        case EDMA3_RM_RES_QDMA_CHANNEL:
        {
            if (lChObj->resId < EDMA3_MAX_QDMA_CH)
            {
                qdmaChId = lChObj->resId + edma3_qdma_ch_min_val[edma3Id];
                paRAMId = edma3RmChBoundRes[edma3Id][qdmaChId].paRAMId;
            }
            else
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        break;

        case EDMA3_RM_RES_PARAM_SET:
        {
            if (lChObj->resId < edma3NumPaRAMSets)
            {
                /**
                     * User has passed the actual param set value here.
                     * Use this value only
                     */
                paRAMId = (int32_t)(lChObj->resId);
            }
            else
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
        }
        break;

        default:
        {
            result = EDMA3_RM_E_INVALID_PARAM;
            break;
        }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        /* Check the param id first. */
        if ((paRAMId != -1) && (paRAMId < (int32_t)edma3NumPaRAMSets))
        {
            /* Get the PaRAM Set Address now. */
            *paramPhyAddr = (uint32_t)(&(globalRegs->PARAMENTRY[paRAMId].OPT));
        }
        else
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_getBaseAddress(EDMA3_RM_Handle hEdmaResMgr,
                                        EDMA3_RM_Cntrlr_PhyAddr controllerId,
                                        uint32_t *phyAddress)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((hEdmaResMgr == NULL) || (phyAddress == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_RM_SOK)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;

        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
        /* Verify the 'controllerId' */
        if (((int32_t)controllerId < (int32_t)((EDMA3_RM_Cntrlr_PhyAddr)(EDMA3_RM_CC_PHY_ADDR))) || (controllerId > (EDMA3_RM_Cntrlr_PhyAddr)(rmObj->gblCfgParams.numTcs)))
        {
            /* Invalid controllerId */
            result = EDMA3_RM_E_INVALID_PARAM;
        }
#endif

        /* Check if the parameters are OK. */
        if (EDMA3_RM_SOK == result)
        {
            if (controllerId == EDMA3_RM_CC_PHY_ADDR)
            {
                /* EDMA3 Channel Controller Address */
                *phyAddress = (uint32_t)(rmObj->gblCfgParams.globalRegs);
            }
            else
            {
                /**
                 * Since the TCs enum start from 1, and TCs start from 0,
                 * subtract 1 from the enum to get the actual address.
                 */
                *phyAddress = (uint32_t)(rmObj->gblCfgParams.tcRegs[controllerId - 1U]);
            }
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_getGblConfigParams(
    uint32_t phyCtrllerInstId,
    EDMA3_RM_GblConfigParams *gblCfgParams)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((phyCtrllerInstId >= EDMA3_MAX_EDMA3_INSTANCES) || (NULL == gblCfgParams))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    if (EDMA3_RM_SOK == result)
    {
        /* Return the previously saved global config information for the EDMA3 HW */
        edma3MemCpy((void *)(gblCfgParams),
                    (const void *)(&resMgrObj[phyCtrllerInstId].gblCfgParams),
                    sizeof(EDMA3_RM_GblConfigParams));
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_getInstanceInitCfg(
    EDMA3_RM_Handle hEdmaResMgr,
    EDMA3_RM_InstanceInitConfig *instanceInitConfig)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    uint32_t resMgrIdx = 0U;
    uint32_t hwId;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if ((hEdmaResMgr == NULL) || (instanceInitConfig == NULL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_RM_SOK)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;

        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        rmObj = rmInstance->pResMgrObjHandle;

        if (rmObj == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        hwId = rmObj->phyCtrllerInstId;

        for (resMgrIdx = 0U; resMgrIdx < EDMA3_MAX_RM_INSTANCES; resMgrIdx++)
        {
            if (rmInstance == ((EDMA3_RM_Instance *)(ptrRMIArray) +
                               (hwId * EDMA3_MAX_RM_INSTANCES) +
                               resMgrIdx))
            {
                /* RM Id found. Return the specific config info to the user. */
                edma3MemCpy((void *)(instanceInitConfig),
                            (const void *)((EDMA3_RM_InstanceInitConfig *)(ptrInitCfgArray) +
                                           (hwId * EDMA3_MAX_RM_INSTANCES) +
                                           resMgrIdx),
                            sizeof(EDMA3_RM_InstanceInitConfig));
                break;
            }
        }

        if (EDMA3_MAX_RM_INSTANCES == resMgrIdx)
        {
            /* RM Id not found, report error... */
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

EDMA3_RM_Result EDMA3_RM_Ioctl(
    EDMA3_RM_Handle hEdmaResMgr,
    EDMA3_RM_IoctlCmd cmd,
    void *cmdArg,
    void *param)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;
    uint32_t paramInitRequired = 0xFFU;
    uint32_t regModificationRequired = 0xFFU;
    uint32_t *ret_val = NULL;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
    if (hEdmaResMgr == NULL)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }

    if ((cmd <= EDMA3_RM_IOCTL_MIN_IOCTL) || (cmd >= EDMA3_RM_IOCTL_MAX_IOCTL))
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_RM_SOK)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdmaResMgr;

        if (rmInstance == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        switch (cmd)
        {
        case EDMA3_RM_IOCTL_SET_PARAM_CLEAR_OPTION:
        {
            paramInitRequired = (uint32_t)cmdArg;

            /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
            if ((paramInitRequired != 0U) && (paramInitRequired != 1U))
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
#endif

            /* Check if the parameters are OK. */
            if (EDMA3_RM_SOK == result)
            {
                /* Set/Reset the flag which is being used to do the PaRAM clearing. */
                rmInstance->paramInitRequired = paramInitRequired;
            }

            break;
        }

        case EDMA3_RM_IOCTL_GET_PARAM_CLEAR_OPTION:
        {
            /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
            if (NULL == cmdArg)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
#endif

            /* Check if the parameters are OK. */
            if (EDMA3_RM_SOK == result)
            {
                ret_val = (uint32_t *)cmdArg;

                /* Get the flag which is being used to do the PaRAM clearing. */
                *ret_val = rmInstance->paramInitRequired;
            }

            break;
        }

        case EDMA3_RM_IOCTL_SET_GBL_REG_MODIFY_OPTION:
        {
            regModificationRequired = (uint32_t)cmdArg;

            /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
            if ((regModificationRequired != 0U) && (regModificationRequired != 1U))
            {
                /* All other values are invalid. */
                result = EDMA3_RM_E_INVALID_PARAM;
            }
#endif

            /* Check if the parameters are OK. */
            if (EDMA3_RM_SOK == result)
            {
                /**
                     * Set/Reset the flag which is being used to do the global
                     * registers and PaRAM modification.
                     */
                rmInstance->regModificationRequired = regModificationRequired;
            }

            break;
        }

        case EDMA3_RM_IOCTL_GET_GBL_REG_MODIFY_OPTION:
        {
            /* If parameter checking is enabled... */
#ifndef EDMA3_RM_PARAM_CHECK_DISABLE
            if (NULL == cmdArg)
            {
                result = EDMA3_RM_E_INVALID_PARAM;
            }
#endif

            /* Check if the parameters are OK. */
            if (EDMA3_RM_SOK == result)
            {
                ret_val = (uint32_t *)cmdArg;

                /**
                     * Get the flag which is being used to do the global
                     * registers and PaRAM modification.
                     */
                *ret_val = rmInstance->regModificationRequired;
            }

            break;
        }

        default:
        {
            /* Hey dude! you passed invalid IOCTL cmd */
            result = EDMA3_RM_E_INVALID_PARAM;
            break;
        }
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_END,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    return result;
}

/**
 * edma3ComplHandler
 * \brief   Interrupt handler for successful transfer completion.
 *
 * \note    This function first disables its own interrupt to make it non-
 *          entrant. Later, after calling all the callback functions, it
 *          re-enables its own interrupt.
 *
 * \return  None.
 */
static void edma3ComplHandler(const EDMA3_RM_Obj *rmObj)
{
    uint32_t Cnt;
    volatile EDMA3_CCRL_Regs *ptrEdmaccRegs = NULL;
    volatile EDMA3_CCRL_ShadowRegs *shadowRegs = NULL;
    volatile uint32_t pendingIrqs;
    volatile uint32_t isIPR = 0;

    uint32_t indexl;
    uint32_t indexh;
    uint32_t edma3Id;
    uint32_t numTCCs;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eINT_START,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    assert(NULL != rmObj);

    edma3Id = rmObj->phyCtrllerInstId;
    numTCCs = rmObj->gblCfgParams.numTccs;
    ptrEdmaccRegs =
        (volatile EDMA3_CCRL_Regs *)rmObj->gblCfgParams.globalRegs;
    if (ptrEdmaccRegs != NULL)
    {
        shadowRegs = (volatile EDMA3_CCRL_ShadowRegs *)(&ptrEdmaccRegs->SHADOW[edma3RegionId]);
    }

    Cnt = 0U;
    pendingIrqs = 0U;
    indexl = 1U;
    indexh = 1U;

    if (shadowRegs != NULL)
    {
        if (numTCCs > 32U)
        {
            isIPR = shadowRegs->IPR | shadowRegs->IPRH;
        }
        else
        {
            isIPR = shadowRegs->IPR;
        }

        if (isIPR)
        {
            /**
         * Since an interrupt has found, we have to make sure that this
         * interrupt (TCC) belongs to the TCCs allocated by us only.
         * It might happen that someone else, who is using EDMA3 also,
         * is the owner of this interrupt channel i.e. the TCC.
         * For this, use the allocatedTCCs[], to check which all interrupt
         * channels are owned by the EDMA3 RM Instances.
         */

            edma3OsProtectEntry(edma3Id,
                                (int32_t)EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION,
                                NULL);

            /* Loop for EDMA3_RM_COMPL_HANDLER_RETRY_COUNT number of time,
		   breaks when no pending interrupt is found */
            while ((Cnt < EDMA3_RM_COMPL_HANDLER_RETRY_COUNT) && ((indexl != 0U) || (indexh != 0U)))
            {
                indexl = 0U;
                pendingIrqs = shadowRegs->IPR;

                /**
             * Choose interrupts coming from our allocated TCCs
             * and MASK remaining ones.
             */
                pendingIrqs = (pendingIrqs & allocatedTCCs[edma3Id][0U]);

                while (pendingIrqs)
                {
                    /*Process all the pending interrupts*/
                    if ((pendingIrqs & 1U) == TRUE)
                    {
                        /**
                     * If the user has not given any callback function
                     * while requesting the TCC, its TCC specific bit
                     * in the IPR register will NOT be cleared.
                     */
                        if (edma3IntrParams[edma3Id][indexl].tccCb != NULL)
                        {
                            /* here write to ICR to clear the corresponding IPR bits*/
                            shadowRegs->ICR = (1U << indexl);

                            edma3IntrParams[edma3Id][indexl].tccCb(indexl,
                                                                   EDMA3_RM_XFER_COMPLETE,
                                                                   edma3IntrParams[edma3Id][indexl].cbData);
                        }
                    }
                    ++indexl;
                    pendingIrqs >>= 1U;
                }

                if (numTCCs > 32U)
                {
                    indexh = 0U;
                    pendingIrqs = shadowRegs->IPRH;

                    /**
	             * Choose interrupts coming from our allocated TCCs
	             * and MASK remaining ones.
	             */
                    pendingIrqs = (pendingIrqs & allocatedTCCs[edma3Id][1U]);

                    while (pendingIrqs)
                    {
                        /*Process all the pending interrupts*/
                        if ((pendingIrqs & 1U) == TRUE)
                        {
                            /**
	                     * If the user has not given any callback function
	                     * while requesting the TCC, its TCC specific bit
	                     * in the IPRH register will NOT be cleared.
	                     */
                            if (edma3IntrParams[edma3Id][32U + indexh].tccCb != NULL)
                            {
                                /* here write to ICR to clear the corresponding IPR bits*/
                                shadowRegs->ICRH = (1U << indexh);

                                edma3IntrParams[edma3Id][32U + indexh].tccCb(32U + indexh,
                                                                             EDMA3_RM_XFER_COMPLETE,
                                                                             edma3IntrParams[edma3Id][32U + indexh].cbData);
                            }
                        }
                        ++indexh;
                        pendingIrqs >>= 1U;
                    }
                }

                Cnt++;
            }

            indexl = (shadowRegs->IPR & allocatedTCCs[edma3Id][0U]);
            if (numTCCs > 32U)
            {
                indexh = (shadowRegs->IPRH & allocatedTCCs[edma3Id][1U]);
            }

            if ((indexl != 0) || (indexh != 0))
            {
                shadowRegs->IEVAL = 0x1U;
            }

            edma3OsProtectExit(rmObj->phyCtrllerInstId,
                               (int32_t)EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION,
                               0);
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eINT_END,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */
}

void lisrEdma3ComplHandler0(uint32_t edma3InstanceId)
{
    /* Invoke Completion Handler ISR */
    edma3ComplHandler(&resMgrObj[edma3InstanceId]);
}

/**
 * \brief   Interrupt handler for Channel Controller Error.
 *
 * \note    This function first disables its own interrupt to make it non-
 *          entrant. Later, after calling all the callback functions, it
 *          re-enables its own interrupt.
 *
 * \return  None.
 */
static void edma3CCErrHandler(const EDMA3_RM_Obj *rmObj)
{
    uint32_t Cnt = 0U;
    uint32_t resMgrInstIdx = 0U;
    volatile EDMA3_CCRL_Regs *ptrEdmaccRegs = NULL;
    volatile EDMA3_CCRL_ShadowRegs *shadowRegs = NULL;
    volatile uint32_t pendingIrqs = 0;
    uint32_t index;
    uint32_t evtqueNum;
    EDMA3_RM_Instance *rm_instance = NULL;
    uint32_t edma3Id;
    uint32_t num_rm_instances_opened;
    EDMA3_RM_Instance *rmInstance = NULL;
    uint32_t ownedDmaError = 0;
    uint32_t ownedDmaHError = 0;
    uint32_t ownedQdmaError = 0;
    uint32_t numTCCs;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3_CCERR",
                    EDMA3_DVT_DESC(EDMA3_DVT_eINT_START,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    assert(rmObj != NULL);

    edma3Id = rmObj->phyCtrllerInstId;
    ptrEdmaccRegs = (volatile EDMA3_CCRL_Regs *)rmObj->gblCfgParams.globalRegs;
    numTCCs = rmObj->gblCfgParams.numTccs;
    if (ptrEdmaccRegs != NULL)
    {
        shadowRegs = (volatile EDMA3_CCRL_ShadowRegs *)&ptrEdmaccRegs->SHADOW[edma3RegionId];
        rmInstance = ((EDMA3_RM_Instance *)(ptrRMIArray) + (edma3Id * EDMA3_MAX_RM_INSTANCES) + edma3RegionId);

        if ((ptrEdmaccRegs->EMR != 0) || (ptrEdmaccRegs->QEMR != 0) || (ptrEdmaccRegs->CCERR != 0))
        {
            pendingIrqs = 1U;
        }
        if (numTCCs > 32U)
        {
            if (ptrEdmaccRegs->EMRH != 0)
            {
                pendingIrqs = 1U;
            }
        }
        index = 1U;

        if (pendingIrqs)
        {
            edma3OsProtectEntry(edma3Id,
                                (int32_t)EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR,
                                NULL);

            /* Loop for EDMA3_RM_CCERR_HANDLER_RETRY_COUNT number of time,
			   breaks when no pending interrupt is found */
            while ((Cnt < EDMA3_RM_CCERR_HANDLER_RETRY_COUNT) && (index != 0U))
            {
                index = 0U;
                pendingIrqs = ptrEdmaccRegs->EMR;

                while (pendingIrqs)
                {
                    /*Process all the pending interrupts*/
                    if ((pendingIrqs & 1U) == TRUE)
                    {
                        uint32_t mappedTcc = 0U;

                        /**
                         * Using the 'index' value (basically the DMA
                         * channel), fetch the corresponding TCC
                         * value, mapped to this DMA channel.
                         */
                        mappedTcc = edma3DmaChTccMapping[edma3Id][index];

                        /**
                         * Ensure that the mapped tcc is valid and the call
                         * back is not NULL
                         */
                        if (mappedTcc < EDMA3_MAX_TCC)
                        {
                            /**
                             * TCC owned and allocated by RM.
                             * Write to EMCR to clear the corresponding EMR bits.
                             */
                            ptrEdmaccRegs->EMCR = (1U << index);
                            /*Clear any SER*/
                            shadowRegs->SECR = (1U << index);

                            /* Call the callback function if registered earlier. */
                            if ((edma3IntrParams[edma3Id][mappedTcc].tccCb) != NULL)
                            {
                                edma3IntrParams[edma3Id][mappedTcc].tccCb(
                                    mappedTcc,
                                    EDMA3_RM_E_CC_DMA_EVT_MISS,
                                    edma3IntrParams[edma3Id][mappedTcc].cbData);
                            }
                        }
                        else
                        {
                            /**
                             * DMA channel not owned by the RM instance.
                             * Check the global error interrupt clearing option.
                             * If it is TRUE, clear the error interupt else leave
                             * it like that.
                             */
#if (EDMA3_RM_RES_CLEAR_ERROR_STATUS_FOR_ALL_CHANNELS == TRUE)
                            /* here write to EMCR to clear the corresponding EMR bits. */
                            ptrEdmaccRegs->EMCR = (1U << index);
                            /*Clear any SER*/
                            ptrEdmaccRegs->SECR = (1U << index);
#endif
                        }
                    }
                    ++index;
                    pendingIrqs >>= 1U;
                }

                if (numTCCs > 32U)
                {
                    index = 0U;
                    pendingIrqs = ptrEdmaccRegs->EMRH;
                    while (pendingIrqs)
                    {
                        /*Process all the pending interrupts*/
                        if ((pendingIrqs & 1U) == TRUE)
                        {
                            uint32_t mappedTcc = 0U;

                            /**
                         * Using the 'index' value (basically the DMA
                         * channel), fetch the corresponding TCC
                         * value, mapped to this DMA channel.
                         */
                            mappedTcc = edma3DmaChTccMapping[edma3Id][32U + index];

                            /**
                         * Ensure that the mapped tcc is valid and the call
                         * back is not NULL
                         */
                            if (mappedTcc < EDMA3_MAX_TCC)
                            {
                                /**
                             * TCC owned and allocated by RM.
                             * Write to EMCR to clear the corresponding EMR bits.
                             */
                                ptrEdmaccRegs->EMCRH = (1U << index);
                                /*Clear any SERH*/
                                shadowRegs->SECRH = (1U << index);

                                /* Call the callback function if registered earlier. */
                                if ((edma3IntrParams[edma3Id][mappedTcc].tccCb) != NULL)
                                {
                                    edma3IntrParams[edma3Id][mappedTcc].tccCb(
                                        mappedTcc,
                                        EDMA3_RM_E_CC_DMA_EVT_MISS,
                                        edma3IntrParams[edma3Id][mappedTcc].cbData);
                                }
                            }
                            else
                            {
                                /**
                             * DMA channel not owned by the RM instance.
                             * Check the global error interrupt clearing option.
                             * If it is TRUE, clear the error interupt else leave
                             * it like that.
                             */
#if (EDMA3_RM_RES_CLEAR_ERROR_STATUS_FOR_ALL_CHANNELS == TRUE)
                                /**
                                 * TCC NOT owned by RM.
                                 * Write to EMCRH to clear the corresponding EMRH bits.
                                 */
                                ptrEdmaccRegs->EMCRH = (1U << index);
                                /*Clear any SERH*/
                                shadowRegs->SECRH = (1U << index);
#endif
                            }
                        }
                        ++index;
                        pendingIrqs >>= 1U;
                    }
                }

                index = 0U;
                pendingIrqs = ptrEdmaccRegs->QEMR;
                while (pendingIrqs)
                {
                    /*Process all the pending interrupts*/
                    if ((pendingIrqs & 1U) == TRUE)
                    {
                        uint32_t mappedTcc = 0U;

                        /**
                         * Using the 'index' value (basically the QDMA
                         * channel), fetch the corresponding TCC
                         * value, mapped to this QDMA channel.
                         */
                        mappedTcc = edma3QdmaChTccMapping[edma3Id][index];

                        if (mappedTcc < EDMA3_MAX_TCC)
                        {
                            /* here write to QEMCR to clear the corresponding QEMR bits*/
                            ptrEdmaccRegs->QEMCR = (1U << index);
                            /*Clear any QSER*/
                            shadowRegs->QSECR = (1U << index);

                            if ((edma3IntrParams[edma3Id][mappedTcc].tccCb) != NULL)
                            {
                                edma3IntrParams[edma3Id][mappedTcc].tccCb(
                                    mappedTcc,
                                    EDMA3_RM_E_CC_QDMA_EVT_MISS,
                                    edma3IntrParams[edma3Id][mappedTcc].cbData);
                            }
                        }
                        else
                        {
                            /**
                             * QDMA channel not owned by the RM instance.
                             * Check the global error interrupt clearing option.
                             * If it is TRUE, clear the error interupt else leave
                             * the ISR.
                             */
#if (EDMA3_RM_RES_CLEAR_ERROR_STATUS_FOR_ALL_CHANNELS == TRUE)
                            /* here write to QEMCR to clear the corresponding QEMR bits*/
                            ptrEdmaccRegs->QEMCR = (1U << index);

                            /*Clear any QSER*/
                            ptrEdmaccRegs->QSECR = (1U << index);
#endif
                        }
                    }
                    ++index;
                    pendingIrqs >>= 1U;
                }

                index = 0U;
                pendingIrqs = ptrEdmaccRegs->CCERR;
                if (pendingIrqs != 0)
                {
                    /* Process all the pending CC error interrupts. */

                    /* Queue threshold error for different event queues.*/
                    for (evtqueNum = 0U; evtqueNum < rmObj->gblCfgParams.numEvtQueue; evtqueNum++)
                    {
                        if ((pendingIrqs & ((uint32_t)1U << evtqueNum)) != 0)
                        {
                            /**
                             * Queue threshold error for queue 'evtqueNum' raised.
                             * Inform all the RM instances working on this region
                             * about the error by calling their global callback functions.
                             */
                            resMgrInstIdx = 0U;
                            for (num_rm_instances_opened = resMgrObj[edma3Id].numOpens; num_rm_instances_opened != 0; num_rm_instances_opened--)
                            {
                                /* Check whether the RM instance opened working on this region */
                                rm_instance = ((EDMA3_RM_Instance *)(ptrRMIArray) + (edma3Id * EDMA3_MAX_RM_INSTANCES) + resMgrInstIdx);
                                if (NULL != rm_instance)
                                {
                                    if (rm_instance->initParam.regionId == edma3RegionId)
                                    {
                                        /* Region id matches, call the callback function */
                                        if (rm_instance->initParam.gblerrCbParams.gblerrCb != NULL)
                                        {
                                            rm_instance->initParam.gblerrCbParams.gblerrCb(
                                                EDMA3_RM_E_CC_QUE_THRES_EXCEED,
                                                evtqueNum,
                                                rm_instance->initParam.gblerrCbParams.gblerrData);
                                        }
                                    }
                                }

                                ++resMgrInstIdx;
                                /* Check next opened instance */
                            }

                            /* Clear the error interrupt. */
                            ptrEdmaccRegs->CCERRCLR = (1U << evtqueNum);
                        }
                    }

                    /* Transfer completion code error. */
                    if ((pendingIrqs & ((uint32_t)1 << EDMA3_CCRL_CCERR_TCCERR_SHIFT)) != 0)
                    {
                        /**
                         * Transfer completion code error raised.
                         * Inform all the RM instances working on this region
                         * about the error by calling their global callback functions.
                         */

                        resMgrInstIdx = 0U;
                        for (num_rm_instances_opened = resMgrObj[edma3Id].numOpens; num_rm_instances_opened != 0; num_rm_instances_opened--)
                        {
                            /* Check whether the RM instance opened working on this region */
                            rm_instance = ((EDMA3_RM_Instance *)(ptrRMIArray) + (edma3Id * EDMA3_MAX_RM_INSTANCES) + resMgrInstIdx);
                            if (NULL != rm_instance)
                            {
                                if (rm_instance->initParam.regionId == edma3RegionId)
                                {
                                    /* Region id matches, call the callback function */
                                    if (rm_instance->initParam.gblerrCbParams.gblerrCb != NULL)
                                    {
                                        rm_instance->initParam.gblerrCbParams.gblerrCb(
                                            EDMA3_RM_E_CC_TCC,
                                            0,
                                            rm_instance->initParam.gblerrCbParams.gblerrData);
                                    }
                                }
                            }

                            ++resMgrInstIdx;
                            /* Check next opened instance */
                        }

                        ptrEdmaccRegs->CCERRCLR = ((uint32_t)1 << EDMA3_CCRL_CCERR_TCCERR_SHIFT);
                    }

                    ++index;
                }

                Cnt++;
            }

            /**
             * Read the error registers again. If any interrupt is pending,
             * write the EEVAL register.
             * Moreover, according to the global error interrupt clearing
             * option, check either error bits associated with all the
             * DMA/QDMA channels (option is SET) OR check error bits
             * associated with owned DMA/QDMA channels.
             */
#if (EDMA3_RM_RES_CLEAR_ERROR_STATUS_FOR_ALL_CHANNELS == TRUE)
            /* To remove warning. */
            rmInstance = rmInstance;

            /* Check all the error bits. */
            ownedDmaError = ptrEdmaccRegs->EMR;
            if (numTCCs > 32U)
            {
                ownedDmaHError = ptrEdmaccRegs->EMRH;
            }
            ownedQdmaError = ptrEdmaccRegs->QEMR;
#else
            /* Check ONLY owned error bits. */
            ownedDmaError = (ptrEdmaccRegs->EMR & rmInstance->initParam.rmInstInitConfig->ownDmaChannels[0U]);
            if (numTCCs > 32)
                ownedDmaHError = (ptrEdmaccRegs->EMRH & rmInstance->initParam.rmInstInitConfig->ownDmaChannels[1U]);
            ownedQdmaError = (ptrEdmaccRegs->QEMR & rmInstance->initParam.rmInstInitConfig->ownQdmaChannels[0U]);
#endif

            if (((ownedDmaError != 0) || (ownedDmaHError != 0)) || ((ownedQdmaError != 0) || (ptrEdmaccRegs->CCERR != 0)))
            {
                ptrEdmaccRegs->EEVAL = 0x1U;
            }

            edma3OsProtectExit(rmObj->phyCtrllerInstId,
                               (int32_t)EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR,
                               0);
        }
        /* Write to EEVAL is required to re-evaluate the Error interrupt */
        /* Without this error interrupt will not be deassarted */
        ptrEdmaccRegs->EEVAL = 0x1U;
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3_CCERR",
                    EDMA3_DVT_DESC(EDMA3_DVT_eINT_END,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */
}

void lisrEdma3CCErrHandler0(uint32_t edma3InstanceId)
{
    /* Invoke CC Error Handler ISR */
    edma3CCErrHandler(&resMgrObj[edma3InstanceId]);
}

/**
 * \brief   Interrupt handler for Transfer Controller Error.
 *
 * \note    This function first disables its own interrupt to make it non-
 *          entrant. Later, after calling all the callback functions, it
 *          re-enables its own interrupt.
 *
 * \return  None.
 */
static void edma3TCErrHandler(const EDMA3_RM_Obj *rmObj, uint32_t tcNum)
{
    volatile EDMA3_TCRL_Regs *tcRegs = NULL;
    uint32_t tcMemErrRdWr = 0U;
    uint32_t resMgrInstIdx = 0U;
    EDMA3_RM_Instance *rm_instance = NULL;
    uint32_t edma3Id;
    uint32_t num_rm_instances_opened;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3_TCERR",
                    EDMA3_DVT_DESC(EDMA3_DVT_eINT_START,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    assert(rmObj != NULL);

    assert(tcNum < rmObj->gblCfgParams.numTcs);

    if (rmObj->gblCfgParams.tcRegs[tcNum] != NULL)
    {
        tcRegs = (volatile EDMA3_TCRL_Regs *)(rmObj->gblCfgParams.tcRegs[tcNum]);
        edma3Id = rmObj->phyCtrllerInstId;
    }

    if (tcRegs != NULL)
    {
        if (tcRegs->ERRSTAT != 0)
        {
            edma3OsProtectEntry(rmObj->phyCtrllerInstId,
                                (int32_t)EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR,
                                &tcNum);

            if ((tcRegs->ERRSTAT & ((uint32_t)1 << EDMA3_TCRL_ERRSTAT_BUSERR_SHIFT)) != 0)
            {
                /* Bus error event. */
                /**
                 * EDMA3TC has detected an error at source or destination
                 * address. Error information can be read from the error
                 * details register (ERRDET).
                 */
                tcMemErrRdWr = tcRegs->ERRDET & ((uint32_t)EDMA3_TCRL_ERRDET_STAT_MASK);
                if ((tcMemErrRdWr > 0U) && (tcMemErrRdWr < 8U))
                {
                    /**
                     * Inform all the RM instances working on this region
                     * about the error by calling their global callback functions.
                     */
                    resMgrInstIdx = 0U;
                    for (num_rm_instances_opened = resMgrObj[edma3Id].numOpens; num_rm_instances_opened != 0; num_rm_instances_opened--)
                    {
                        /* Check whether the RM instance opened working on this region */
                        rm_instance = ((EDMA3_RM_Instance *)(ptrRMIArray) + (edma3Id * EDMA3_MAX_RM_INSTANCES) + resMgrInstIdx);
                        if (NULL != rm_instance)
                        {
                            if (rm_instance->initParam.regionId == edma3RegionId)
                            {
                                /* Region id matches, call the callback function */
                                if (rm_instance->initParam.gblerrCbParams.gblerrCb != NULL)
                                {
                                    rm_instance->initParam.gblerrCbParams.gblerrCb(
                                        EDMA3_RM_E_TC_MEM_LOCATION_READ_ERROR,
                                        tcNum,
                                        rm_instance->initParam.gblerrCbParams.gblerrData);
                                }
                            }
                        }

                        ++resMgrInstIdx;
                        /* Check next opened instance */
                    }
                }
                else
                {
                    if ((tcMemErrRdWr >= 8U) && (tcMemErrRdWr <= 0xFU))
                    {
                        /**
                         * Inform all the RM instances working on this region
                         * about the error by calling their global callback functions.
                         */
                        resMgrInstIdx = 0U;
                        for (num_rm_instances_opened = resMgrObj[edma3Id].numOpens; num_rm_instances_opened != 0; num_rm_instances_opened--)
                        {
                            /* Check whether the RM instance opened working on this region */
                            rm_instance = ((EDMA3_RM_Instance *)(ptrRMIArray) + (edma3Id * EDMA3_MAX_RM_INSTANCES) + resMgrInstIdx);
                            if (NULL != rm_instance)
                            {
                                if (rm_instance->initParam.regionId == edma3RegionId)
                                {
                                    /* Region id matches, call the callback function */
                                    if (rm_instance->initParam.gblerrCbParams.gblerrCb != NULL)
                                    {
                                        rm_instance->initParam.gblerrCbParams.gblerrCb(
                                            EDMA3_RM_E_TC_MEM_LOCATION_WRITE_ERROR,
                                            tcNum,
                                            rm_instance->initParam.gblerrCbParams.gblerrData);
                                    }
                                }
                            }

                            ++resMgrInstIdx;
                            /* Check next opened instance */
                        }
                    }
                }
                tcRegs->ERRCLR = ((uint32_t)1 << EDMA3_TCRL_ERRSTAT_BUSERR_SHIFT);
            }
            else
            {
                /* Transfer request (TR) error event. */
                if ((tcRegs->ERRSTAT & ((uint32_t)1 << EDMA3_TCRL_ERRSTAT_TRERR_SHIFT)) != 0)
                {
                    resMgrInstIdx = 0U;
                    for (num_rm_instances_opened = resMgrObj[edma3Id].numOpens; num_rm_instances_opened != 0; num_rm_instances_opened--)
                    {
                        /* Check whether the RM instance opened working on this region */
                        rm_instance = ((EDMA3_RM_Instance *)(ptrRMIArray) + (edma3Id * EDMA3_MAX_RM_INSTANCES) + resMgrInstIdx);
                        if (NULL != rm_instance)
                        {
                            if (rm_instance->initParam.regionId == edma3RegionId)
                            {
                                /* Region id matches, call the callback function */
                                if (rm_instance->initParam.gblerrCbParams.gblerrCb != NULL)
                                {
                                    rm_instance->initParam.gblerrCbParams.gblerrCb(
                                        EDMA3_RM_E_TC_TR_ERROR,
                                        tcNum,
                                        rm_instance->initParam.gblerrCbParams.gblerrData);
                                }
                            }
                        }

                        ++resMgrInstIdx;
                        /* Check next opened instance */
                    }

                    tcRegs->ERRCLR = ((uint32_t)1 << EDMA3_TCRL_ERRSTAT_TRERR_SHIFT);
                }
                else
                {
                    if ((tcRegs->ERRSTAT & ((uint32_t)1 << EDMA3_TCRL_ERRSTAT_MMRAERR_SHIFT)) != 0)
                    {
                        resMgrInstIdx = 0U;
                        for (num_rm_instances_opened = resMgrObj[edma3Id].numOpens; num_rm_instances_opened != 0; num_rm_instances_opened--)
                        {
                            /* Check whether the RM instance opened working on this region */
                            rm_instance = ((EDMA3_RM_Instance *)(ptrRMIArray) + (edma3Id * EDMA3_MAX_RM_INSTANCES) + resMgrInstIdx);
                            if (NULL != rm_instance)
                            {
                                if (rm_instance->initParam.regionId == edma3RegionId)
                                {
                                    /* Region id matches, call the callback function */
                                    if (rm_instance->initParam.gblerrCbParams.gblerrCb != NULL)
                                    {
                                        rm_instance->initParam.gblerrCbParams.gblerrCb(
                                            EDMA3_RM_E_TC_INVALID_ADDR,
                                            tcNum,
                                            rm_instance->initParam.gblerrCbParams.gblerrData);
                                    }
                                }
                            }

                            ++resMgrInstIdx;
                            /* Check next opened instance */
                        }

                        tcRegs->ERRCLR = ((uint32_t)1 << EDMA3_TCRL_ERRSTAT_MMRAERR_SHIFT);
                    }
                }
            }

            edma3OsProtectExit(rmObj->phyCtrllerInstId,
                               (int32_t)EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR,
                               tcNum);
        }
    }

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3_TCERR",
                    EDMA3_DVT_DESC(EDMA3_DVT_eINT_END,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */
}

/*
 *  ======== lisrEdma3TC0ErrHandler0 ========
 *  EDMA3 instance 0 TC0 Error Interrupt Service Routine
 */
void lisrEdma3TC0ErrHandler0(uint32_t edma3InstanceId)
{
    /* Invoke Error Handler ISR for TC0*/
    edma3TCErrHandler(&resMgrObj[edma3InstanceId], (uint32_t)0U);
}

/*
 *  ======== lisrEdma3TC1ErrHandler0 ========
 *  EDMA3 instance 0 TC1 Error Interrupt Service Routine
 */
void lisrEdma3TC1ErrHandler0(uint32_t edma3InstanceId)
{
    /* Invoke Error Handler ISR for TC1*/
    edma3TCErrHandler(&resMgrObj[edma3InstanceId], (uint32_t)1U);
}

/*
 *  ======== lisrEdma3TC2ErrHandler0 ========
 *  EDMA3 instance 0 TC2 Error Interrupt Service Routine
 */
void lisrEdma3TC2ErrHandler0(uint32_t edma3InstanceId)
{
    /* Invoke Error Handler ISR for TC2*/
    edma3TCErrHandler(&resMgrObj[edma3InstanceId], (uint32_t)2U);
}

/*
 *  ======== lisrEdma3TC3ErrHandler0 ========
 *  EDMA3 instance 0 TC3 Error Interrupt Service Routine
 */
void lisrEdma3TC3ErrHandler0(uint32_t edma3InstanceId)
{
    /* Invoke Error Handler ISR for TC3*/
    edma3TCErrHandler(&resMgrObj[edma3InstanceId], (uint32_t)3U);
}

/*
 *  ======== lisrEdma3TC4ErrHandler0 ========
 *  EDMA3 instance 0 TC4 Error Interrupt Service Routine
 */
void lisrEdma3TC4ErrHandler0(uint32_t edma3InstanceId)
{
    /* Invoke Error Handler ISR for TC4*/
    edma3TCErrHandler(&resMgrObj[edma3InstanceId], (uint32_t)4U);
}

/*
 *  ======== lisrEdma3TC5ErrHandler0 ========
 *  EDMA3 instance 0 TC5 Error Interrupt Service Routine
 */
void lisrEdma3TC5ErrHandler0(uint32_t edma3InstanceId)
{
    /* Invoke Error Handler ISR for TC5*/
    edma3TCErrHandler(&resMgrObj[edma3InstanceId], (uint32_t)5U);
}

/*
 *  ======== lisrEdma3TC6ErrHandler0 ========
 *  EDMA3 instance 0 TC6 Error Interrupt Service Routine
 */
/* ARGSUSED */
void lisrEdma3TC6ErrHandler0(uint32_t edma3InstanceId)
{
    /* Invoke Error Handler ISR for TC6*/
    edma3TCErrHandler(&resMgrObj[edma3InstanceId], (uint32_t)6U);
}

/*
 *  ======== lisrEdma3TC7ErrHandler0 ========
 *  EDMA3 instance 0 TC7 Error Interrupt Service Routine
 */
void lisrEdma3TC7ErrHandler0(uint32_t edma3InstanceId)
{
    /* Invoke Error Handler ISR for TC7*/
    edma3TCErrHandler(&resMgrObj[edma3InstanceId], (uint32_t)7U);
}

/*  Resource Manager Internal functions - Start */

/** Initialization of the Global region registers of the EDMA3 Controller */
static void edma3GlobalRegionInit(uint32_t phyCtrllerInstId, uint32_t numDmaChannels)
{
    uint32_t evtQNum = 0U;
    volatile EDMA3_CCRL_Regs *ptrEdmaccRegs = NULL;

    assert(phyCtrllerInstId < EDMA3_MAX_EDMA3_INSTANCES);

    ptrEdmaccRegs = (volatile EDMA3_CCRL_Regs *)(resMgrObj[phyCtrllerInstId].gblCfgParams.globalRegs);

    if (ptrEdmaccRegs != NULL)
    {
        ptrEdmaccRegs->EMCR = EDMA3_RM_SET_ALL_BITS;
        if (numDmaChannels > 32U)
        {
            /* Clear the EMCRH only if available channels are more than 32 */
            ptrEdmaccRegs->EMCRH = EDMA3_RM_SET_ALL_BITS;
        }
        ptrEdmaccRegs->QEMCR = EDMA3_RM_SET_ALL_BITS;

        /*
        * Set all Instance-wide EDMA3 parameters (not channel-specific)
        */

        /**
         * Set TC Priority among system-wide bus-masters and Queue
         * Watermark Level
         */
        while (evtQNum <
               resMgrObj[phyCtrllerInstId].gblCfgParams.numEvtQueue)
        {
            ptrEdmaccRegs->QUEPRI &= EDMA3_RM_QUEPRI_CLR_MASK(evtQNum);
            ptrEdmaccRegs->QUEPRI |= EDMA3_RM_QUEPRI_SET_MASK(evtQNum,
                                                              resMgrObj[phyCtrllerInstId].gblCfgParams.evtQPri[evtQNum]);

            ptrEdmaccRegs->QWMTHRA |= EDMA3_RM_QUEWMTHR_SET_MASK(evtQNum,
                                                                 resMgrObj[phyCtrllerInstId].gblCfgParams.evtQueueWaterMarkLvl[evtQNum]);

            evtQNum++;
        }

        /* Clear CCERR register */
        ptrEdmaccRegs->CCERRCLR = 0xFFFFU;
    }

    return;
}

/** Initialization of the Shadow region registers of the EDMA3 Controller */
static void edma3ShadowRegionInit(const EDMA3_RM_Instance *pRMInstance)
{
    volatile EDMA3_CCRL_Regs *ptrEdmaccRegs = NULL;
    volatile EDMA3_CCRL_ShadowRegs *ptrEdmaShadowRegs = NULL;
    uint32_t phyCtrllerInstId;
    uint32_t regionId;
    const EDMA3_RM_InstanceInitConfig *rmInstInitConfig = NULL;

    assert(pRMInstance != NULL);

    rmInstInitConfig = pRMInstance->initParam.rmInstInitConfig;

    if (rmInstInitConfig != NULL)
    {
        phyCtrllerInstId = pRMInstance->pResMgrObjHandle->phyCtrllerInstId;
        regionId = pRMInstance->initParam.regionId;

        ptrEdmaccRegs = (volatile EDMA3_CCRL_Regs *)(resMgrObj[phyCtrllerInstId].gblCfgParams.globalRegs);

        if (ptrEdmaccRegs != NULL)
        {
            ptrEdmaShadowRegs = (volatile EDMA3_CCRL_ShadowRegs *)(&ptrEdmaccRegs->SHADOW[regionId]);

            ptrEdmaShadowRegs->ECR = (rmInstInitConfig->ownDmaChannels[0U] | rmInstInitConfig->ownTccs[0U]);
            ptrEdmaShadowRegs->ECRH = (rmInstInitConfig->ownDmaChannels[1U] | rmInstInitConfig->ownTccs[1U]);
            ptrEdmaShadowRegs->EECR = (rmInstInitConfig->ownDmaChannels[0U] | rmInstInitConfig->ownTccs[0U]);
            ptrEdmaShadowRegs->SECR = (rmInstInitConfig->ownDmaChannels[0U] | rmInstInitConfig->ownTccs[0U]);
            ptrEdmaShadowRegs->SECRH = (rmInstInitConfig->ownDmaChannels[1U] | rmInstInitConfig->ownTccs[1U]);
            ptrEdmaShadowRegs->EECR = (rmInstInitConfig->ownDmaChannels[0U] | rmInstInitConfig->ownTccs[0U]);
            ptrEdmaShadowRegs->EECRH = (rmInstInitConfig->ownDmaChannels[1U] | rmInstInitConfig->ownTccs[1U]);

            ptrEdmaShadowRegs->QEECR = rmInstInitConfig->ownQdmaChannels[0U];

            ptrEdmaShadowRegs->IECR = (rmInstInitConfig->ownDmaChannels[0U] | rmInstInitConfig->ownTccs[0U]);
            ptrEdmaShadowRegs->IECRH = (rmInstInitConfig->ownDmaChannels[1U] | rmInstInitConfig->ownTccs[1U]);
            ptrEdmaShadowRegs->ICR = (rmInstInitConfig->ownDmaChannels[0U] | rmInstInitConfig->ownTccs[0U]);
            ptrEdmaShadowRegs->ICRH = (rmInstInitConfig->ownDmaChannels[1U] | rmInstInitConfig->ownTccs[1U]);

            ptrEdmaShadowRegs->QSECR = rmInstInitConfig->ownQdmaChannels[0U];

            /*
            * Set all EDMA3 Resource<->Region mapping parameters
            */

            /* 1. Dma Channel (and TCC) <-> Region */
            ptrEdmaccRegs->DRA[regionId].DRAE = 0U;
            ptrEdmaccRegs->DRA[regionId].DRAEH = 0U;

            /* 2. Qdma Channel <-> Region */
            ptrEdmaccRegs->QRAE[regionId] = 0U;
        }
    }

    return;
}

/** Local MemZero function */
void edma3MemZero(void *dst, uint32_t len)
{
    uint32_t i = 0U;
    uint32_t *ds = NULL;

    assert(dst != NULL);

    ds = (uint32_t *)dst;

    for (i = 0U; i < (len / 4U); i++)
    {
        *ds = 0x0;
        ds++;
    }

    return;
}

/* Local MemCopy function */
void edma3MemCpy(void *dst, const void *src, uint32_t len)
{
    uint32_t i = 0U;
    const uint32_t *sr;
    uint32_t *ds;

    assert(src != NULL);
    assert(dst != NULL);
    assert((len % 4U) == 0);

    sr = (const uint32_t *)src;
    ds = (uint32_t *)dst;

    for (i = 0U; i < (len / 4U); i++)
    {
        *ds = *sr;
        ds++;
        sr++;
    }

    return;
}

/* Local MemCopy function to copy Param Set ONLY */
void edma3ParamCpy(volatile void *dst, const volatile void *src)
{
    uint32_t i = 0U;
    const volatile uint32_t *sr;
    volatile uint32_t *ds;

    assert(src != NULL);
    assert(dst != NULL);

    sr = (const volatile uint32_t *)src;
    ds = (volatile uint32_t *)dst;

    for (i = 0U; i < 8U; i++)
    {
        *ds = *sr;
        ds++;
        sr++;
    }

    return;
}

/**
 * Finds a particular bit ('0' or '1') in the particular word from 'start'.
 * If found, returns the position, else return -1.
 */
static int32_t findBitInWord(int32_t source, uint32_t start, uint16_t bit)
{
    uint32_t position = start;
    uint16_t found = 0;
    uint32_t iterations_left = 0;
    uint32_t sourceTmp = 0;

    switch (bit)
    {
    case 1U:
    {
        sourceTmp = (uint32_t)source >> (start % 32U);
        source = (int32_t)sourceTmp;

        while ((found == 0U) && (source != 0))
        {
            if (((uint32_t)source & 0x1U) == 0x1U)
            {
                /* 1 */
                found++;
            }
            else
            {
                /* 0 */
                sourceTmp = (uint32_t)source >> 1;
                source = (int32_t)sourceTmp;
                position++;
            }
        }
    }
    break;

    case 0:
    {
        sourceTmp = (uint32_t)source >> (start % 32U);
        source = (int32_t)sourceTmp;
        iterations_left = (uint32_t)32U - (start % 32U);

        while ((found == 0U) && (iterations_left > 0U))
        {
            if (((uint32_t)source & 0x1U) == 0x1U)
            {
                /* 1 */
                sourceTmp = (uint32_t)source >> 1;
                source = (int32_t)sourceTmp;
                position++;
                iterations_left--;
            }
            else
            {
                /* 0 */
                found++;
            }
        }
    }
    break;

    default:
        break;
    }

    return (found ? (int32_t)position : -1);
}

/**
 * Finds a particular bit ('0' or '1') in the specified resources' array
 * from 'start' to 'end'. If found, returns the position, else return -1.
 */
static int32_t findBit(EDMA3_RM_ResType resType,
                       uint32_t start,
                       uint32_t end,
                       uint16_t bit)
{
    int32_t position = -1;
    uint32_t start_index = start / 32U;
    uint32_t end_index = end / 32U;
    int32_t i;
    uint32_t *resPtr = 0x0;
    int32_t ret = -1;
    EDMA3_RM_Result result = EDMA3_RM_SOK;

    assert(start <= end);

    /**
     * job is to find 'bit' in an array[start_index:end_index]
     * algo used:
     * first search in array[start_index]
     * then search in array[start_index + 1 : end_index - 1]
     * then search in array[end_index]
     */
    switch (resType)
    {
    case EDMA3_RM_RES_DMA_CHANNEL:
        resPtr = &contiguousDmaRes[0];
        break;

    case EDMA3_RM_RES_QDMA_CHANNEL:
        resPtr = &contiguousQdmaRes[0];
        break;

    case EDMA3_RM_RES_TCC:
        resPtr = &contiguousTccRes[0];
        break;

    case EDMA3_RM_RES_PARAM_SET:
        resPtr = &contiguousParamRes[0];
        break;

    default:
        result = EDMA3_RM_E_INVALID_PARAM;
        break;
    }

    if (EDMA3_RM_SOK == result)
    {
        switch (bit)
        {
        case 1U:
        {
            /* Find '1' in first word. */
            position = findBitInWord(resPtr[start_index], start, (uint16_t)1U);

            if (position != -1)
            {
                ret = position;
            }
            else
            {
                /* '1' NOT found, look into other words. */
                for (i = ((int32_t)start_index + (int32_t)1U); i <= ((int32_t)end_index - (int32_t)1U); i++)
                {
                    position = findBitInWord(resPtr[i], (uint32_t)0U, (uint16_t)1U);
                    if (position != -1)
                    {
                        /* '1' Found... */
                        ret = (position + (i * 32));
                        break;
                    }
                }

                /* First check whether we have found '1' or not. */
                if (ret == -1)
                {
                    /* Still not found, look in the last word. */
                    position = findBitInWord(resPtr[end_index], (uint32_t)0U, (uint16_t)1U);
                    if (position != -1)
                    {
                        /* Finally got it. */
                        ret = (position + ((int32_t)end_index * (int32_t)32U));
                    }
                    else
                    {
                        /* Sorry, could not find it, return -1. */
                        ret = -1;
                    }
                }
            }
        }
        break;

        case 0U:
        {
            /* Find '0' in first word. */
            position = findBitInWord(resPtr[start_index], start, (uint16_t)0U);
            if (position != -1)
            {
                ret = position;
            }
            else
            {
                /* '0' NOT found, look into other words. */
                for (i = ((int32_t)start_index + (int32_t)1U); i <= ((int32_t)end_index - (int32_t)1U); i++)
                {
                    position = findBitInWord(resPtr[i], (uint32_t)0U, (uint16_t)0U);
                    if (position != -1)
                    {
                        /* '0' found... */
                        ret = (position + (i * 32));
                        break;
                    }
                }

                /* First check whether we have found '0' or not. */
                if (ret == -1)
                {
                    position = findBitInWord(resPtr[end_index], (uint32_t)0U, (uint16_t)0U);
                    if (position != -1)
                    {
                        /* Finally got it. */
                        ret = (position + ((int32_t)end_index * (int32_t)32U));
                    }
                    else
                    {
                        /* Sorry, could not find it, return -1. */
                        ret = -1;
                    }
                }
            }
        }
        break;

        default:
            break;
        }
    }

    return ((ret >= (int32_t)start) ? ret : -1);
}

/**
 * If successful, this function returns EDMA3_RM_SOK and the position
 * of first available resource in 'positionRes'. Else returns error.
 */
static EDMA3_RM_Result allocAnyContigRes(EDMA3_RM_ResType resType,
                                         uint32_t numResources,
                                         uint32_t *positionRes)
{
    uint16_t found = 0U;
    int32_t first_one, next_zero;
    uint32_t num_available;
    int32_t ret = -1;
    uint32_t start = 0;
    uint32_t end;
    EDMA3_RM_Result result = EDMA3_RM_SOK;

    assert(positionRes != NULL);

    switch (resType)
    {
    case EDMA3_RM_RES_DMA_CHANNEL:
        end = EDMA3_MAX_DMA_CH - 1U;
        break;

    case EDMA3_RM_RES_QDMA_CHANNEL:
        end = EDMA3_MAX_QDMA_CH - 1U;
        break;

    case EDMA3_RM_RES_TCC:
        end = EDMA3_MAX_TCC - 1U;
        break;

    case EDMA3_RM_RES_PARAM_SET:
        end = edma3NumPaRAMSets - 1U;
        break;

    default:
        result = EDMA3_RM_E_INVALID_PARAM;
        break;
    }

    if (result == EDMA3_RM_SOK)
    {
        /**
         * Algorithm used for finding N contiguous resources.
         * In the resources' array, '1' means available and '0' means
         * not-available.
         * Step a) Find first '1' starting from 'start'. If successful,
         * store it in first_one, else return error.
         * Step b) Find first '0' starting from (first_one+1) to 'end'.
         * If successful, store returned value in next_zero. If '0' could
         * not be located, it means all the resources are available.
         * Store 'end' (i.e. the last resource id) in next_zero.
         * Step c) Count the number of contiguous resources available
         * by subtracting first_one from next_zero.
         * Step d) If result < N, do the whole process again untill you
         * reach end. Else you have found enough resources, return success.
         */
        while ((found == 0) && (start <= end) && (((end - start) + 1U) >= numResources))
        {
            /* Find first '1' starting from 'start' till 'end'. */
            first_one = findBit(resType, start, end, (uint16_t)1U);
            if (first_one != -1)
            {
                /* Got first 1, search for first '0' now. */
                next_zero = findBit(resType, (uint32_t)first_one + (uint32_t)1, end, (uint16_t)0U);
                if (next_zero == -1)
                {
                    /* Unable to find next zero, all 1' are there */
                    next_zero = (int32_t)end + (int32_t)1U;
                }

                /* check no of resources available */
                num_available = (uint32_t)next_zero - (uint32_t)first_one;
                if (num_available >= numResources)
                {
                    /* hurrah..., we have found enough resources. */
                    found = 1U;
                    ret = first_one;
                }
                else
                {
                    /* Not enough resources, try again */
                    start = (uint32_t)next_zero + (uint32_t)1;
                }
            }
            else
            {
                /* do nothing, first 1 is not there, return.  */
                break;
            }
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        if (found == 1U)
        {
            /* required resources found, retrun the first available res id. */
            *positionRes = (uint32_t)ret;
        }
        else
        {
            /* No resources allocated */
            result = EDMA3_RM_E_SPECIFIED_RES_NOT_AVAILABLE;
        }
    }

    return result;
}

/**
 * Starting from 'firstResIdObj', this function makes the next 'numResources'
 * Resources non-available for future. Also, it does some global resisters'
 * setting also.
 */
static EDMA3_RM_Result gblChngAllocContigRes(EDMA3_RM_Instance *rmInstance,
                                             const EDMA3_RM_ResDesc *firstResIdObj,
                                             uint32_t numResources)
{
    EDMA3_RM_Result result = EDMA3_RM_SOK;
    volatile EDMA3_CCRL_Regs *gblRegs = NULL;
    EDMA3_RM_Obj *rmObj = NULL;
    uint32_t avlblIdx = 0U;
    uint32_t firstResId = 0U;
    uint32_t lastResId = 0U;
    uint32_t edma3Id;

    assert(rmInstance != NULL);
    assert(firstResIdObj != NULL);

    rmObj = rmInstance->pResMgrObjHandle;

    if (rmObj == NULL)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }

    if (EDMA3_RM_SOK == result)
    {
        edma3Id = rmObj->phyCtrllerInstId;
        gblRegs = (volatile EDMA3_CCRL_Regs *)(rmObj->gblCfgParams.globalRegs);

        if (gblRegs == NULL)
        {
            result = EDMA3_RM_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_RM_SOK)
    {
        switch (firstResIdObj->type)
        {
        case EDMA3_RM_RES_DMA_CHANNEL:
        {
            firstResId = firstResIdObj->resId;
            lastResId = firstResId + (numResources - 1U);

            for (avlblIdx = firstResId; avlblIdx <= lastResId; ++avlblIdx)
            {
                rmInstance->avlblDmaChannels[avlblIdx / 32U] &= (uint32_t)(~((uint32_t)1U << (avlblIdx % 32U)));

                /**
                     * Enable the DMA channel in the DRAE/DRAEH registers also.
                     */
                if (avlblIdx < 32U)
                {
                    gblRegs->DRA[rmInstance->initParam.regionId].DRAE |= ((uint32_t)0x1U << avlblIdx);
                }
                else
                {
                    gblRegs->DRA[rmInstance->initParam.regionId].DRAEH |= ((uint32_t)0x1U << (avlblIdx - 32U));
                }
            }
        }
        break;

        case EDMA3_RM_RES_QDMA_CHANNEL:
        {
            firstResId = firstResIdObj->resId;
            lastResId = firstResId + (numResources - 1U);

            for (avlblIdx = firstResId; avlblIdx <= lastResId; ++avlblIdx)
            {
                rmInstance->avlblQdmaChannels[avlblIdx / 32U] &= (uint32_t)(~((uint32_t)1U << (avlblIdx % 32U)));

                /**
                     * Enable the QDMA channel in the QRAE register also.
                     */
                gblRegs->QRAE[rmInstance->initParam.regionId] |= ((uint32_t)0x1U << avlblIdx);
            }
        }
        break;

        case EDMA3_RM_RES_TCC:
        {
            firstResId = firstResIdObj->resId;
            lastResId = firstResId + (numResources - 1U);

            for (avlblIdx = firstResId; avlblIdx <= lastResId; ++avlblIdx)
            {
                rmInstance->avlblTccs[avlblIdx / 32U] &= (uint32_t)(~((uint32_t)1U << (avlblIdx % 32U)));

                /**
					 * If the region id coming from this
                     * RM instance is same as the Master RM
                     * Instance's region id, only then we will be
                     * getting the interrupts on the same side.
                     * So save the TCC in the allocatedTCCs[] array.
                     */
                if (edma3RegionId == rmInstance->initParam.regionId)
                {
                    if (avlblIdx < 32U)
                    {
                        allocatedTCCs[edma3Id][0U] |= ((uint32_t)0x1U << avlblIdx);
                    }
                    else
                    {
                        allocatedTCCs[edma3Id][1U] |= ((uint32_t)0x1U << (avlblIdx - 32U));
                    }
                }
            }
        }
        break;

        case EDMA3_RM_RES_PARAM_SET:
        {
            firstResId = firstResIdObj->resId;
            lastResId = firstResId + (numResources - 1U);

            for (avlblIdx = firstResId; avlblIdx <= lastResId; ++avlblIdx)
            {
                rmInstance->avlblPaRAMSets[avlblIdx / 32U] &= (uint32_t)(~((uint32_t)1U << (avlblIdx % 32U)));

                /**
                     * Also, make the actual PARAM Set NULL, checking the flag
                     * whether it is required or not.
                     */
                if (TRUE == rmInstance->paramInitRequired)
                {
                    edma3MemZero((void *)(&gblRegs->PARAMENTRY[avlblIdx]),
                                 sizeof(gblRegs->PARAMENTRY[avlblIdx]));
                }
            }
        }
        break;

        default:
            result = EDMA3_RM_E_INVALID_PARAM;
            break;
        }
    }

    return result;
}

EDMA3_RM_Result EDMA3_RM_initXbarEventMap(EDMA3_RM_Handle hEdma,
                                          const EDMA3_RM_GblXbarToChanConfigParams *edmaGblXbarConfig,
                                          EDMA3_RM_mapXbarEvtToChan mapXbarEvtFunc,
                                          EDMA3_RM_xbarConfigScr configXbarScr)
{
    EDMA3_RM_Result result = EDMA3_DRV_SOK;
    EDMA3_RM_Instance *rmInstance = NULL;

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (hEdma == NULL)
    {
        result = EDMA3_RM_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        rmInstance = (EDMA3_RM_Instance *)hEdma;

        if (mapXbarEvtFunc != NULL)
        {
            rmInstance->mapXbarToChan = mapXbarEvtFunc;
        }
        if (configXbarScr != NULL)
        {
            rmInstance->configScrMapXbarToEvt = configXbarScr;
        }
        if (edmaGblXbarConfig != NULL)
        {
            edma3MemCpy((void *)(&rmInstance->rmXbarToEvtMapConfig),
                        (const void *)(edmaGblXbarConfig),
                        sizeof(EDMA3_RM_GblXbarToChanConfigParams));
        }
    }

    return (result);
}

/*  Resource Manager Internal functions - End */

/* End of File */
