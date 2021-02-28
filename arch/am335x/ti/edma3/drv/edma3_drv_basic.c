/*
 * edma3_drv_basic.c
 *
 * EDMA3 Driver Basic Interface Implementation. This file contains
 * beginner-level EDMA3 Driver APIs which are required to:
 * a) Request/free a DMA, QDMA and Link channel.
 * b) Program various fields in the PaRAM Set like source/destination
 * parameters, transfer parameters etc.
 * c) Enable/disable a transfer.
 * These APIs are provided to program a DMA/QDMA channel for simple use-cases
 * and don't expose all the features of EDMA3 hardware. Users who want to go
 * beyond this and have complete control on the EDMA3 hardware are advised
 * to refer edma3_drv_adv.c source file.
 *
 * Copyright (C) 2009-2018 Texas Instruments Incorporated - http://www.ti.com/
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

/* EDMa3 Driver Internal Header Files */
#include <ti/edma3/drv/edma3.h>
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

/* Externel Variables */
/*---------------------------------------------------------------------------*/
/**
 * Maximum Resource Manager Instances supported by the EDMA3 Package.
 */
extern const uint32_t EDMA3_MAX_RM_INSTANCES;

/**
 * \brief EDMA3 Resource Manager Objects, tied to each EDMA3 HW Controller.
 *
 * Typically one RM object will cater to one EDMA3 HW controller
 * and will have all the global config information.
 */
extern EDMA3_RM_Obj resMgrObj[EDMA3_MAX_EDMA3_INSTANCES];

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
extern far EDMA3_RM_InstanceInitConfig *ptrInitCfgArray;
#else
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
extern far EDMA3_RM_Instance *ptrRMIArray;
#else
extern EDMA3_RM_Instance *ptrRMIArray;
#endif

/** Local MemSet function */
extern void edma3MemZero(void *dst, uint32_t len);
/** Local MemCpy function */
extern void edma3MemCpy(void *dst, const void *src, uint32_t len);

/**
 * \brief EDMA3 Driver Objects, tied to each EDMA3 HW Controller.
 *
 * Typically one object will cater to one EDMA3 HW controller
 * and will have all regions' (ARM, DSP etc) specific config information.
 */
extern EDMA3_DRV_Object drvObj[EDMA3_MAX_EDMA3_INSTANCES];

/**
 * Handles of EDMA3 Driver Instances.
 *
 * Used to maintain information of the EDMA3 Driver Instances for
 * each region, for each HW controller.
 * There could be as many Driver Instances as there are shadow
 * regions. Multiple EDMA3 Driver instances on the same shadow
 * region are NOT allowed.
 */
extern EDMA3_DRV_Instance drvInstance[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_REGIONS];

/**
 * \brief Resources bound to a Channel
 *
 * When a request for a channel is made, the resources PaRAM Set and TCC
 * get bound to that channel. This information is needed internally by the
 * driver when a request is made to free the channel (Since it is the
 * responsibility of the driver to free up the channel-associated resources
 * from the Resource Manager layer).
 */
extern EDMA3_DRV_ChBoundResources edma3DrvChBoundRes[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_LOGICAL_CH];

/** Max of DMA Channels */
extern uint32_t edma3_dma_ch_max_val[EDMA3_MAX_EDMA3_INSTANCES];
/** Min of Link Channels */
extern uint32_t edma3_link_ch_min_val[EDMA3_MAX_EDMA3_INSTANCES];
/** Max of Link Channels */
extern uint32_t edma3_link_ch_max_val[EDMA3_MAX_EDMA3_INSTANCES];
/** Min of QDMA Channels */
extern uint32_t edma3_qdma_ch_min_val[EDMA3_MAX_EDMA3_INSTANCES];
/** Max of QDMA Channels */
extern uint32_t edma3_qdma_ch_max_val[EDMA3_MAX_EDMA3_INSTANCES];
/** Max of Logical Channels */
extern uint32_t edma3_log_ch_max_val[EDMA3_MAX_EDMA3_INSTANCES];

/* Local functions prototypes */
/*---------------------------------------------------------------------------*/
/** Remove various mappings and do cleanup for DMA/QDMA channels */
static EDMA3_DRV_Result edma3RemoveMapping(EDMA3_DRV_Handle hEdma,
                                           uint32_t channelId);

/** @brief Global Variable which describes the EDMA3 LLD Version Information */
const char edma3LldVersionStr[] = EDMA3_LLD_DRV_VERSION_STR ":" __DATE__ ":" __TIME__;
/*---------------------------------------------------------------------------*/

EDMA3_DRV_Result EDMA3_DRV_requestChannel(EDMA3_DRV_Handle hEdma,
                                          uint32_t *pLCh,
                                          uint32_t *pTcc,
                                          EDMA3_RM_EventQueue evtQueue,
                                          EDMA3_RM_TccCallback tccCb,
                                          void *cbData)
{
    EDMA3_RM_ResDesc resObj;
    EDMA3_RM_ResDesc channelObj;
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    uint32_t mappedTcc = EDMA3_DRV_CH_NO_TCC_MAP;
    int32_t paRAMId = (int32_t)EDMA3_RM_RES_ANY;
    EDMA3_DRV_ChannelType chType = EDMA3_DRV_CHANNEL_TYPE_NONE;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    int32_t mappedPaRAMId;
    uint32_t edma3Id;
    uint32_t freeDmaQdmaChannel = FALSE;
    uint32_t mapXbarEvtToChanFlag = FALSE;
    uint32_t xBarEvtBeforeMap = 0;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if ((pLCh == NULL) || (hEdma == NULL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }
    if (EDMA3_DRV_SOK == result)
    {
        if (drvInst->mapXbarToChan != NULL)
        {
            xBarEvtBeforeMap = *pLCh;
            edma3Id = drvObject->phyCtrllerInstId;
            if ((xBarEvtBeforeMap > edma3_dma_ch_max_val[edma3Id]) &&
                (xBarEvtBeforeMap < EDMA3_DRV_DMA_CHANNEL_ANY) &&
                ((*pLCh) == (*pTcc)))
            {
                result = drvInst->mapXbarToChan(xBarEvtBeforeMap,
                                                pLCh,
                                                &drvInst->drvXbarToEvtMapConfig);
                if (EDMA3_DRV_SOK == result)
                {
                    *pTcc = *pLCh;
                    mapXbarEvtToChanFlag = TRUE;
                }
            }
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        edma3Id = drvObject->phyCtrllerInstId;
        globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);

        /* Identify the request type and validate the appropriate arguments.
		 * Starting in the order of PaRAM Set availability,
		 * check for a specific DMA channel request first.
		 */
        if ((*pLCh) <= edma3_dma_ch_max_val[edma3Id])
        {
            /* A 'Specific' DMA channel is requested */
            chType = EDMA3_DRV_CHANNEL_TYPE_DMA;
            /* Fill the resObj structure as well */
            resObj.type = EDMA3_RM_RES_DMA_CHANNEL;
            resObj.resId = *pLCh;

            /* Check the TCC and Event Queue number */
            if ((evtQueue >= drvObject->gblCfgParams.numEvtQueue) || (pTcc == NULL))
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }

        if ((*pLCh) == EDMA3_DRV_DMA_CHANNEL_ANY)
        {
            /* 'Any' DMA channel is requested */
            chType = EDMA3_DRV_CHANNEL_TYPE_DMA;
            resObj.type = EDMA3_RM_RES_DMA_CHANNEL;
            resObj.resId = EDMA3_RM_RES_ANY;

            /* Check the TCC and Event Queue number */
            if ((evtQueue >= drvObject->gblCfgParams.numEvtQueue) || (pTcc == NULL))
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }

        if (((*pLCh) >= edma3_link_ch_min_val[edma3Id]) &&
            ((*pLCh) <= edma3_link_ch_max_val[edma3Id]))
        {
            /* A 'Specific' Link channel is requested, TCC may or may not be
			 * required.
			 */
            /* Save the PaRAM Id for future use */
            paRAMId = *pLCh;

            if (pTcc != NULL)
            {
                if (*pTcc < drvObject->gblCfgParams.numTccs)
                {
                    /* Argument passed as *pTcc is a valid TCC number */
                    chType = EDMA3_DRV_CHANNEL_TYPE_LINK_TCC;
                    resObj.type = EDMA3_RM_RES_PARAM_SET;
                }
                else
                {
                    /* Argument passed as *pTcc is Invalid TCC number */
                    result = EDMA3_DRV_E_INVALID_PARAM;
                }
            }
            else
            {
                /* pTcc is NULL, only Link Channel required */
                chType = EDMA3_DRV_CHANNEL_TYPE_LINK;
                resObj.type = EDMA3_RM_RES_PARAM_SET;
            }
        }

        if ((*pLCh) == EDMA3_DRV_LINK_CHANNEL)
        {
            /* 'Any' Link channel is requested, TCC is not required */
            chType = EDMA3_DRV_CHANNEL_TYPE_LINK;
            resObj.type = EDMA3_RM_RES_PARAM_SET;
        }

        if ((*pLCh) == EDMA3_DRV_LINK_CHANNEL_WITH_TCC)
        {
            if (pTcc != NULL)
            {
                /* 'Any' Link channel is requested, TCC is required */
                if (*pTcc < drvObject->gblCfgParams.numTccs)
                {
                    /* Argument passed as *pTcc is a valid TCC number */
                    chType = EDMA3_DRV_CHANNEL_TYPE_LINK_TCC;
                    resObj.type = EDMA3_RM_RES_PARAM_SET;
                }
                else
                {
                    /* Argument passed as *pTcc is Invalid TCC number */
                    result = EDMA3_DRV_E_INVALID_PARAM;
                }
            }
            else
            {
                /* pTcc is NULL */
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }

        if (((*pLCh) >= EDMA3_DRV_QDMA_CHANNEL_0) &&
            ((*pLCh) <= EDMA3_DRV_QDMA_CHANNEL_7))
        {
            /* A 'Specific' QDMA channel is requested */
            chType = EDMA3_DRV_CHANNEL_TYPE_QDMA;
            resObj.type = EDMA3_RM_RES_QDMA_CHANNEL;
            resObj.resId = *pLCh - EDMA3_DRV_QDMA_CHANNEL_0;

            /* Check the TCC and Event Queue number */
            if ((evtQueue >= drvObject->gblCfgParams.numEvtQueue) || (pTcc == NULL))
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }

        if ((*pLCh) == EDMA3_DRV_QDMA_CHANNEL_ANY)
        {
            /* 'Any' QDMA channel is requested */
            chType = EDMA3_DRV_CHANNEL_TYPE_QDMA;
            resObj.type = EDMA3_RM_RES_QDMA_CHANNEL;
            resObj.resId = EDMA3_RM_RES_ANY;

            /* Check the TCC and Event Queue number */
            if ((evtQueue >= drvObject->gblCfgParams.numEvtQueue) || (pTcc == NULL))
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }

        if (EDMA3_DRV_CHANNEL_TYPE_NONE == chType)
        {
            /* Invalid request */
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    /* Step 1: Allocate the DMA/QDMA channel first, if required */
    if (EDMA3_DRV_SOK == result)
    {
        switch (chType)
        {
        case EDMA3_DRV_CHANNEL_TYPE_DMA:
            result = EDMA3_RM_allocResource(drvInst->resMgrInstance,
                                            (EDMA3_RM_ResDesc *)&resObj);
            if (result == EDMA3_RM_SOK)
            {
                *pLCh = resObj.resId;

                mappedPaRAMId = drvObject->gblCfgParams.dmaChannelPaRAMMap[*pLCh];
                if (mappedPaRAMId != (int32_t)EDMA3_DRV_CH_NO_PARAM_MAP)
                {
                    paRAMId = mappedPaRAMId;
                }

                if (*pTcc == EDMA3_DRV_TCC_ANY)
                {
                    mappedTcc = drvObject->gblCfgParams.dmaChannelTccMap[*pLCh];
                    if (mappedTcc == EDMA3_DRV_CH_NO_TCC_MAP)
                    {
                        mappedTcc = EDMA3_RM_RES_ANY;
                    }
                }
                else
                {
                    mappedTcc = *pTcc;
                }

                /* Save the Resource Type/ID for TCC registeration */
                channelObj.type = EDMA3_RM_RES_DMA_CHANNEL;
                channelObj.resId = *pLCh;

                /* Free DMA channel in case the function fails in future */
                freeDmaQdmaChannel = TRUE;
            }
            else
            {
                result = EDMA3_DRV_E_DMA_CHANNEL_UNAVAIL;
            }

            break;

        case EDMA3_DRV_CHANNEL_TYPE_QDMA:
            result = EDMA3_RM_allocResource(drvInst->resMgrInstance,
                                            (EDMA3_RM_ResDesc *)&resObj);
            if (result == EDMA3_DRV_SOK)
            {
                (*pLCh) = resObj.resId + edma3_qdma_ch_min_val[edma3Id];

                if (*pTcc == EDMA3_DRV_TCC_ANY)
                {
                    mappedTcc = EDMA3_RM_RES_ANY;
                }
                else
                {
                    mappedTcc = *pTcc;
                }

                /* Save the Resource Type/ID for TCC registeration */
                channelObj.type = EDMA3_RM_RES_QDMA_CHANNEL;
                channelObj.resId = resObj.resId;

                /* Free DMA channel in case the function fails in future */
                freeDmaQdmaChannel = TRUE;
            }
            else
            {
                result = EDMA3_DRV_E_QDMA_CHANNEL_UNAVAIL;
            }
            break;

        default:
            break;
        }
    }

    /* Step 2: Allocate the PaRAM Set */
    if (EDMA3_DRV_SOK == result)
    {
        resObj.type = EDMA3_RM_RES_PARAM_SET;
        resObj.resId = (uint32_t)paRAMId;
        result = EDMA3_RM_allocResource(drvInst->resMgrInstance, (EDMA3_RM_ResDesc *)&resObj);
    }

    if (EDMA3_DRV_SOK == result)
    {
        paRAMId = (int32_t)resObj.resId;

        if (chType == EDMA3_DRV_CHANNEL_TYPE_LINK)
        {
            /* Link channel number should be same as the PaRAM Set */
            *pLCh = resObj.resId;
            edma3DrvChBoundRes[edma3Id][*pLCh].paRAMId = paRAMId;
            edma3DrvChBoundRes[edma3Id][*pLCh].trigMode = EDMA3_DRV_TRIG_MODE_NONE;
        }

        if (chType == EDMA3_DRV_CHANNEL_TYPE_LINK_TCC)
        {
            /* Link channel number should be same as the PaRAM Set */
            *pLCh = resObj.resId;
            edma3DrvChBoundRes[edma3Id][*pLCh].paRAMId = paRAMId;
            edma3DrvChBoundRes[edma3Id][*pLCh].trigMode = EDMA3_DRV_TRIG_MODE_NONE;

            /* save the tcc now */
            edma3DrvChBoundRes[edma3Id][*pLCh].tcc = *pTcc;

            /* Set TCC in ParamSet.OPT field */
            globalRegs->PARAMENTRY[paRAMId].OPT &= EDMA3_DRV_OPT_TCC_CLR_MASK;
            globalRegs->PARAMENTRY[paRAMId].OPT |= EDMA3_DRV_OPT_TCC_SET_MASK(*pTcc);
        }
    }
    else
    {
        /* PaRAM allocation failed, free the previously allocated DMA/QDMA
		 * channel, if required
		 */
        if (((chType == EDMA3_DRV_CHANNEL_TYPE_DMA) || (chType == EDMA3_DRV_CHANNEL_TYPE_QDMA)) &&
            (TRUE == freeDmaQdmaChannel))
        {
            EDMA3_RM_freeResource(drvInst->resMgrInstance, (EDMA3_RM_ResDesc *)&channelObj);
        }
    }

    /* Step 3: Allocate TCC only for DMA/QDMA channels */
    if ((EDMA3_DRV_SOK == result) &&
        ((chType == EDMA3_DRV_CHANNEL_TYPE_DMA) || (chType == EDMA3_DRV_CHANNEL_TYPE_QDMA)))
    {
        resObj.type = EDMA3_RM_RES_TCC;
        resObj.resId = mappedTcc;
        result = EDMA3_RM_allocResource(drvInst->resMgrInstance, (EDMA3_RM_ResDesc *)&resObj);

        if (EDMA3_DRV_SOK == result)
        {
            *pTcc = resObj.resId;

            /* Save TCC and PaRAM set */
            edma3DrvChBoundRes[edma3Id][*pLCh].tcc = *pTcc;
            edma3DrvChBoundRes[edma3Id][*pLCh].paRAMId = paRAMId;

            switch (chType)
            {
            case EDMA3_DRV_CHANNEL_TYPE_DMA:
            {
                /* Step 4: Register the callback function, if required */
                if (NULL != tccCb)
                {
                    result = EDMA3_RM_registerTccCb(drvInst->resMgrInstance,
                                                    (EDMA3_RM_ResDesc *)&channelObj,
                                                    *pTcc, tccCb, cbData);
                }
                if (result != EDMA3_DRV_SOK)
                {
                    EDMA3_DRV_freeChannel(hEdma, *pLCh);
                    result = EDMA3_DRV_E_TCC_REGISTER_FAIL;
                }
                else
                {
#ifndef EDMA3_PROGRAM_QUEUE_NUM_REGISTER_INIT_TIME
                    uint32_t intState = 0;
                    edma3OsProtectEntry(edma3Id, (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                        &intState);
                    /* Step 5: Associate DMA Channel to Event Queue */
                    globalRegs->DMAQNUM[(*pLCh) >> 3U] &= EDMA3_DRV_DMAQNUM_CLR_MASK(*pLCh);
                    globalRegs->DMAQNUM[(*pLCh) >> 3U] |= EDMA3_DRV_DMAQNUM_SET_MASK((*pLCh), evtQueue);

                    edma3OsProtectExit(edma3Id, (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                       intState);
#endif

                    /* Step 6: Map PaRAM Set to DMA Channel */
                    if (TRUE == drvObject->gblCfgParams.dmaChPaRAMMapExists)
                    {
                        globalRegs->DCHMAP[*pLCh] &= EDMA3_DRV_DCH_PARAM_CLR_MASK;
                        globalRegs->DCHMAP[*pLCh] |= EDMA3_DRV_DCH_PARAM_SET_MASK((uint32_t)paRAMId);
                    }

                    /* Step 7: Set TCC in ParamSet.OPT field */
                    globalRegs->PARAMENTRY[paRAMId].OPT &= EDMA3_DRV_OPT_TCC_CLR_MASK;
                    globalRegs->PARAMENTRY[paRAMId].OPT |= EDMA3_DRV_OPT_TCC_SET_MASK(*pTcc);

                    edma3DrvChBoundRes[edma3Id][*pLCh].trigMode = EDMA3_DRV_TRIG_MODE_NONE;
                }
            }
            break;

            case EDMA3_DRV_CHANNEL_TYPE_QDMA:
            {
                uint32_t qdmaChannel = channelObj.resId;

                /* Step 4: Register the callback function, if required */
                if (NULL != tccCb)
                {
                    result = EDMA3_RM_registerTccCb(drvInst->resMgrInstance,
                                                    (EDMA3_RM_ResDesc *)&channelObj,
                                                    *pTcc, tccCb, cbData);
                }
                if (result != EDMA3_DRV_SOK)
                {
                    EDMA3_DRV_freeChannel(hEdma, *pLCh);
                    result = EDMA3_DRV_E_TCC_REGISTER_FAIL;
                }
                else
                {
#ifndef EDMA3_PROGRAM_QUEUE_NUM_REGISTER_INIT_TIME
                    uint32_t intState = 0;
                    edma3OsProtectEntry(edma3Id, (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                        &intState);
                    /* Step 5: Associate QDMA Channel to Event Queue */
                    globalRegs->QDMAQNUM &= EDMA3_DRV_QDMAQNUM_CLR_MASK(qdmaChannel);
                    globalRegs->QDMAQNUM |= EDMA3_DRV_QDMAQNUM_SET_MASK(qdmaChannel, evtQueue);

                    edma3OsProtectExit(edma3Id, (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                       intState);
#endif

                    /* Step 6: Map PaRAM Set to DMA Channel and set the Default Trigger Word */
                    globalRegs->QCHMAP[qdmaChannel] &= EDMA3_DRV_QCH_PARAM_TRWORD_CLR_MASK;
                    globalRegs->QCHMAP[qdmaChannel] |= EDMA3_DRV_QCH_PARAM_SET_MASK((uint32_t)paRAMId);
                    globalRegs->QCHMAP[qdmaChannel] |= EDMA3_DRV_QCH_TRWORD_SET_MASK(EDMA3_RM_QDMA_TRIG_DEFAULT);

                    /* Step 7: Set TCC in ParamSet.OPT field */
                    globalRegs->PARAMENTRY[paRAMId].OPT &= EDMA3_DRV_OPT_TCC_CLR_MASK;
                    globalRegs->PARAMENTRY[paRAMId].OPT |= EDMA3_DRV_OPT_TCC_SET_MASK(*pTcc);

                    edma3DrvChBoundRes[edma3Id][*pLCh].trigMode = EDMA3_DRV_TRIG_MODE_QDMA;

                    /* Step 8: Enable the QDMA Channel */
                    drvInst->shadowRegs->QEESR = 1U << qdmaChannel;
                }
            }
            break;
            default:
                break;
            }
        }
        else
        {
            /* TCC allocation failed, free the PaRAM Set, */
            resObj.type = EDMA3_RM_RES_PARAM_SET;
            resObj.resId = (uint32_t)paRAMId;
            EDMA3_RM_freeResource(drvInst->resMgrInstance, (EDMA3_RM_ResDesc *)&resObj);

            /* And free the DMA/QDMA channel */
            EDMA3_RM_freeResource(drvInst->resMgrInstance, (EDMA3_RM_ResDesc *)&channelObj);

            result = EDMA3_DRV_E_TCC_UNAVAIL;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        if ((drvInst->configScrMapXbarToEvt != NULL) &&
            (mapXbarEvtToChanFlag == TRUE))
        {
            drvInst->configScrMapXbarToEvt(xBarEvtBeforeMap, *pLCh);
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

EDMA3_DRV_Result EDMA3_DRV_freeChannel(EDMA3_DRV_Handle hEdma,
                                       uint32_t channelId)
{
    EDMA3_RM_ResDesc resObj;
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    int32_t paRAMId;
    uint32_t tcc;
    EDMA3_DRV_ChannelType chType = EDMA3_DRV_CHANNEL_TYPE_NONE;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (hEdma == NULL)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if (drvObject == NULL)
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* Check the channel type */
        if (channelId <= edma3_dma_ch_max_val[edma3Id])
        {
            /* DMA Channel */
            chType = EDMA3_DRV_CHANNEL_TYPE_DMA;
        }

        if ((channelId >= edma3_link_ch_min_val[edma3Id]) && (channelId <= edma3_link_ch_max_val[edma3Id]))
        {
            /* LINK Channel */
            chType = EDMA3_DRV_CHANNEL_TYPE_LINK;
        }

        if ((channelId >= edma3_qdma_ch_min_val[edma3Id]) && (channelId <= edma3_qdma_ch_max_val[edma3Id]))
        {
            /* QDMA Channel */
            chType = EDMA3_DRV_CHANNEL_TYPE_QDMA;
        }

        if (chType == EDMA3_DRV_CHANNEL_TYPE_NONE)
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        if (channelId < EDMA3_MAX_LOGICAL_CH)
        {
            if (chType == EDMA3_DRV_CHANNEL_TYPE_LINK)
            {
                /* LINK Channel */
                resObj.type = EDMA3_RM_RES_PARAM_SET;

                /* Get the PaRAM id from the book-keeping info. */
                resObj.resId = (uint32_t)(edma3DrvChBoundRes[edma3Id][channelId].paRAMId);

                result = EDMA3_RM_freeResource(drvInst->resMgrInstance,
                                               (EDMA3_RM_ResDesc *)&resObj);

                if (EDMA3_DRV_SOK == result)
                {
                    edma3DrvChBoundRes[edma3Id][channelId].paRAMId = -1;
                }
            }
            else
            {
                /* DMA/QDMA Channel */
                paRAMId = edma3DrvChBoundRes[edma3Id][channelId].paRAMId;
                tcc = edma3DrvChBoundRes[edma3Id][channelId].tcc;

                /* Check the paRAMId and tcc values first */
                if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
                {
                    result = EDMA3_DRV_E_INVALID_PARAM;
                }

                if (tcc >= drvObject->gblCfgParams.numTccs)
                {
                    result = EDMA3_DRV_E_INVALID_PARAM;
                }

                if (EDMA3_DRV_SOK == result)
                {
                    /* Disable the transfer and remove various mappings. */
                    result = edma3RemoveMapping(hEdma, channelId);
                }

                if (EDMA3_DRV_SOK == result)
                {
                    /* Now Free the PARAM set and TCC */
                    resObj.type = EDMA3_RM_RES_PARAM_SET;
                    resObj.resId = (uint32_t)paRAMId;
                    result = EDMA3_RM_freeResource(drvInst->resMgrInstance, (EDMA3_RM_ResDesc *)&resObj);

                    if (EDMA3_DRV_SOK == result)
                    {
                        /* PaRAM Set Freed */
                        edma3DrvChBoundRes[edma3Id][channelId].paRAMId = -1;

                        /* Free the TCC */
                        resObj.type = EDMA3_RM_RES_TCC;
                        resObj.resId = tcc;
                        result = EDMA3_RM_freeResource(drvInst->resMgrInstance,
                                                       (EDMA3_RM_ResDesc *)&resObj);
                    }

                    if (EDMA3_DRV_SOK == result)
                    {
                        /* TCC Freed. */
                        edma3DrvChBoundRes[edma3Id][channelId].tcc = EDMA3_MAX_TCC;

                        /* Now free the DMA/QDMA Channel in the end. */
                        if (chType == EDMA3_DRV_CHANNEL_TYPE_QDMA)
                        {
                            resObj.type = EDMA3_RM_RES_QDMA_CHANNEL;
                            resObj.resId = (channelId - edma3_qdma_ch_min_val[edma3Id]);
                            result = EDMA3_RM_freeResource(drvInst->resMgrInstance,
                                                           (EDMA3_RM_ResDesc *)&resObj);
                        }
                        else
                        {
                            resObj.type = EDMA3_RM_RES_DMA_CHANNEL;
                            resObj.resId = channelId;
                            result = EDMA3_RM_freeResource(drvInst->resMgrInstance,
                                                           (EDMA3_RM_ResDesc *)&resObj);
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

EDMA3_DRV_Result EDMA3_DRV_clearErrorBits(EDMA3_DRV_Handle hEdma,
                                          uint32_t channelId)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    uint32_t count;
    uint32_t value = 0;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (hEdma == NULL)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;

            /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
            if (channelId > edma3_dma_ch_max_val[edma3Id])
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
#endif

            /* Check if the parameters are OK. */
            if (EDMA3_DRV_SOK == result)
            {
                globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);

#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("EMR =%l\r\n", globalRegs->EMR);
#endif
                if (channelId < 32U)
                {
                    /* Disables the DMA channels */
                    drvInst->shadowRegs->EECR = (1U << channelId);
                    /* Write to EMCR to clear the corresponding EMR bit */
                    globalRegs->EMCR = (1U << channelId);
                    /* Clears the SER */
                    drvInst->shadowRegs->SECR = (1U << channelId);
                }
                else
                {
#ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF("EMRH =%l\r\n", globalRegs->EMRH);
#endif
                    /* Disables the DMA channels */
                    drvInst->shadowRegs->EECRH = (1U << (channelId - 32U));
                    /* Write to EMCR to clear the corresponding EMR bit */
                    globalRegs->EMCRH = (1U << (channelId - 32U));
                    /* Clears the SER */
                    drvInst->shadowRegs->SECRH = (1U << (channelId - 32U));
                }

                /* Clear the global CC Error Register */
                for (count = 0; count < drvObject->gblCfgParams.numEvtQueue; count++)
                {
                    value |= (((uint32_t)1U) << count);
                }

                globalRegs->CCERRCLR = (EDMA3_CCRL_CCERR_TCCERR_MASK | value);
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

EDMA3_DRV_Result EDMA3_DRV_setOptField(EDMA3_DRV_Handle hEdma,
                                       uint32_t lCh,
                                       EDMA3_DRV_OptField optField,
                                       uint32_t newOptFieldVal)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    uint32_t newOptVal = 0;
    uint32_t oldOptVal = 0;
    int32_t paRAMId = 0;
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
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if ((hEdma == NULL) || (((int32_t)optField < (int32_t)EDMA3_DRV_OPT_FIELD_SAM) || (optField > EDMA3_DRV_OPT_FIELD_ITCCHEN)))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
            globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        paRAMId = edma3DrvChBoundRes[edma3Id][lCh].paRAMId;
        if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        oldOptVal = (uint32_t)(*(&globalRegs->PARAMENTRY[paRAMId].OPT));

        switch (optField)
        {
        case EDMA3_DRV_OPT_FIELD_SAM:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_SAM_CLR_MASK) |
                        (EDMA3_DRV_OPT_SAM_SET_MASK(newOptFieldVal));
            break;
        case EDMA3_DRV_OPT_FIELD_DAM:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_DAM_CLR_MASK) |
                        (EDMA3_DRV_OPT_DAM_SET_MASK(newOptFieldVal));
            break;
        case EDMA3_DRV_OPT_FIELD_SYNCDIM:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_SYNCDIM_CLR_MASK) |
                        (EDMA3_DRV_OPT_SYNCDIM_SET_MASK(newOptFieldVal));
            break;
        case EDMA3_DRV_OPT_FIELD_STATIC:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_STATIC_CLR_MASK) |
                        (EDMA3_DRV_OPT_STATIC_SET_MASK(newOptFieldVal));
            break;
        case EDMA3_DRV_OPT_FIELD_FWID:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_FWID_CLR_MASK) |
                        (EDMA3_DRV_OPT_FWID_SET_MASK(newOptFieldVal));
            break;
        case EDMA3_DRV_OPT_FIELD_TCCMODE:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_TCCMODE_CLR_MASK) |
                        (EDMA3_DRV_OPT_TCCMODE_SET_MASK(newOptFieldVal));
            break;
        case EDMA3_DRV_OPT_FIELD_TCC:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_TCC_CLR_MASK) |
                        (EDMA3_DRV_OPT_TCC_SET_MASK(newOptFieldVal));
            break;
        case EDMA3_DRV_OPT_FIELD_TCINTEN:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_TCINTEN_CLR_MASK) |
                        (EDMA3_DRV_OPT_TCINTEN_SET_MASK(newOptFieldVal));
            break;
        case EDMA3_DRV_OPT_FIELD_ITCINTEN:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_ITCINTEN_CLR_MASK) |
                        (EDMA3_DRV_OPT_ITCINTEN_SET_MASK(newOptFieldVal));
            break;
        case EDMA3_DRV_OPT_FIELD_TCCHEN:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_TCCHEN_CLR_MASK) |
                        (EDMA3_DRV_OPT_TCCHEN_SET_MASK(newOptFieldVal));
            break;
        case EDMA3_DRV_OPT_FIELD_ITCCHEN:
            newOptVal = (oldOptVal & EDMA3_DRV_OPT_ITCCHEN_CLR_MASK) |
                        (EDMA3_DRV_OPT_ITCCHEN_SET_MASK(newOptFieldVal));
            break;
        default:
            break;
        }

        *(&globalRegs->PARAMENTRY[paRAMId].OPT) = newOptVal;
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

EDMA3_DRV_Result EDMA3_DRV_getOptField(EDMA3_DRV_Handle hEdma,
                                       uint32_t lCh,
                                       EDMA3_DRV_OptField optField,
                                       uint32_t *optFieldVal)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    uint32_t optVal = 0;
    int32_t paRAMId = 0;
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
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (((hEdma == NULL) || (optFieldVal == NULL)) || (((int32_t)optField < (int32_t)EDMA3_DRV_OPT_FIELD_SAM) || (optField > EDMA3_DRV_OPT_FIELD_ITCCHEN)))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
            globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        paRAMId = edma3DrvChBoundRes[edma3Id][lCh].paRAMId;
        if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        optVal = (uint32_t)(*(&globalRegs->PARAMENTRY[paRAMId].OPT));

        switch (optField)
        {
        case EDMA3_DRV_OPT_FIELD_SAM:
            *optFieldVal = EDMA3_DRV_OPT_SAM_GET_MASK(optVal);
            break;
        case EDMA3_DRV_OPT_FIELD_DAM:
            *optFieldVal = EDMA3_DRV_OPT_DAM_GET_MASK(optVal);
            break;
        case EDMA3_DRV_OPT_FIELD_SYNCDIM:
            *optFieldVal = EDMA3_DRV_OPT_SYNCDIM_GET_MASK(optVal);
            break;
        case EDMA3_DRV_OPT_FIELD_STATIC:
            *optFieldVal = EDMA3_DRV_OPT_STATIC_GET_MASK(optVal);
            break;
        case EDMA3_DRV_OPT_FIELD_FWID:
            *optFieldVal = EDMA3_DRV_OPT_FWID_GET_MASK(optVal);
            break;
        case EDMA3_DRV_OPT_FIELD_TCCMODE:
            *optFieldVal = EDMA3_DRV_OPT_TCCMODE_GET_MASK(optVal);
            break;
        case EDMA3_DRV_OPT_FIELD_TCC:
            *optFieldVal = EDMA3_DRV_OPT_TCC_GET_MASK(optVal);
            break;
        case EDMA3_DRV_OPT_FIELD_TCINTEN:
            *optFieldVal = EDMA3_DRV_OPT_TCINTEN_GET_MASK(optVal);
            break;
        case EDMA3_DRV_OPT_FIELD_ITCINTEN:
            *optFieldVal = EDMA3_DRV_OPT_ITCINTEN_GET_MASK(optVal);
            break;
        case EDMA3_DRV_OPT_FIELD_TCCHEN:
            *optFieldVal = EDMA3_DRV_OPT_TCCHEN_GET_MASK(optVal);
            break;
        case EDMA3_DRV_OPT_FIELD_ITCCHEN:
            *optFieldVal = EDMA3_DRV_OPT_ITCCHEN_GET_MASK(optVal);
            break;
        default:
            break;
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

EDMA3_DRV_Result EDMA3_DRV_setSrcParams(EDMA3_DRV_Handle hEdma,
                                        uint32_t lCh,
                                        uint32_t srcAddr,
                                        EDMA3_DRV_AddrMode addrMode,
                                        EDMA3_DRV_FifoWidth fifoWidth)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    uint32_t opt = 0;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    int32_t paRAMId = 0;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    uint32_t mappedEvtQ = 0;
    uint32_t defaultBurstSize = 0;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if ((hEdma == NULL) || ((((int32_t)addrMode < (int32_t)EDMA3_DRV_ADDR_MODE_INCR) || (addrMode > EDMA3_DRV_ADDR_MODE_FIFO)) || (((int32_t)fifoWidth < (int32_t)EDMA3_DRV_W8BIT) || (fifoWidth > EDMA3_DRV_W256BIT))))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }

    /** In FIFO Addressing mode, memory location must be 32 bytes aligned */
    if ((addrMode == EDMA3_DRV_ADDR_MODE_FIFO) && ((srcAddr & 0x1FU) != 0x0))
    {
        /** Memory is not 32 bytes aligned */
        result = EDMA3_DRV_E_ADDRESS_NOT_ALIGNED;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
            globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        paRAMId = edma3DrvChBoundRes[edma3Id][lCh].paRAMId;
        if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /**
          * If request is for FIFO mode, check whether the FIFO size
          * is supported by the Transfer Controller which will be used for
          * this transfer or not.
          */
        if (addrMode == EDMA3_DRV_ADDR_MODE_FIFO)
        {
            if (lCh <= edma3_dma_ch_max_val[edma3Id])
            {
                mappedEvtQ = ((globalRegs->DMAQNUM[lCh >> 3U]) & (~(EDMA3_DRV_DMAQNUM_CLR_MASK(lCh)))) >> ((lCh % 8U) * 4U);
            }
            else
            {
                if ((lCh >= edma3_qdma_ch_min_val[edma3Id]) && (lCh <= edma3_qdma_ch_max_val[edma3Id]))
                {
                    mappedEvtQ = ((globalRegs->QDMAQNUM) & (~(EDMA3_DRV_QDMAQNUM_CLR_MASK(lCh - edma3_qdma_ch_min_val[edma3Id])))) >> (lCh * 4U);
                }
            }

            /**
               * mappedEvtQ contains the event queue and hence the TC which will
               * process this transfer request. Check whether this TC supports the
               * FIFO size or not.
               */
            defaultBurstSize = (uint32_t)1U << fifoWidth;
            if (defaultBurstSize > drvObject->gblCfgParams.tcDefaultBurstSize[mappedEvtQ])
            {
                result = EDMA3_DRV_E_FIFO_WIDTH_NOT_SUPPORTED;
            }
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* Set Src Address */
        *((&globalRegs->PARAMENTRY[paRAMId].OPT) +
          (uint32_t)EDMA3_DRV_PARAM_ENTRY_SRC) = srcAddr;

        opt = (uint32_t)(*(&globalRegs->PARAMENTRY[paRAMId].OPT));

        /* Set SAM */
        opt &= EDMA3_DRV_OPT_SAM_CLR_MASK;
        opt |= EDMA3_DRV_OPT_SAM_SET_MASK(addrMode);
        /* Set FIFO Width */
        opt &= EDMA3_DRV_OPT_FWID_CLR_MASK;
        opt |= EDMA3_DRV_OPT_FWID_SET_MASK(fifoWidth);

        /* Set the OPT */
        *(&globalRegs->PARAMENTRY[paRAMId].OPT) = opt;
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

EDMA3_DRV_Result EDMA3_DRV_setDestParams(EDMA3_DRV_Handle hEdma,
                                         uint32_t lCh,
                                         uint32_t destAddr,
                                         EDMA3_DRV_AddrMode addrMode,
                                         EDMA3_DRV_FifoWidth fifoWidth)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    uint32_t opt = 0;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    int32_t paRAMId = 0;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    uint32_t mappedEvtQ = 0;
    uint32_t defaultBurstSize = 0;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if ((hEdma == NULL) || ((((int32_t)addrMode < (int32_t)EDMA3_DRV_ADDR_MODE_INCR) || (addrMode > EDMA3_DRV_ADDR_MODE_FIFO)) || (((int32_t)fifoWidth < (int32_t)EDMA3_DRV_W8BIT) || (fifoWidth > EDMA3_DRV_W256BIT))))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }

    /** In FIFO Addressing mode, memory location must be 32 bytes aligned */
    if ((addrMode == EDMA3_DRV_ADDR_MODE_FIFO) && ((destAddr & 0x1FU) != 0x0))
    {
        /** Memory is not 32 bytes aligned */
        result = EDMA3_DRV_E_ADDRESS_NOT_ALIGNED;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
            globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        paRAMId = edma3DrvChBoundRes[edma3Id][lCh].paRAMId;
        if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /**
          * If request is for FIFO mode, check whether the FIFO size
          * is supported by the Transfer Controller which will be used for
          * this transfer or not.
          */
        if (addrMode == EDMA3_DRV_ADDR_MODE_FIFO)
        {
            if (lCh <= edma3_dma_ch_max_val[edma3Id])
            {
                mappedEvtQ = ((globalRegs->DMAQNUM[lCh >> 3U]) & (~(EDMA3_DRV_DMAQNUM_CLR_MASK(lCh)))) >> ((lCh % 8U) * 4U);
            }
            else
            {
                if ((lCh >= edma3_qdma_ch_min_val[edma3Id]) && (lCh <= edma3_qdma_ch_max_val[edma3Id]))
                {
                    mappedEvtQ = ((globalRegs->QDMAQNUM) & (~(EDMA3_DRV_QDMAQNUM_CLR_MASK(lCh - edma3_qdma_ch_min_val[edma3Id])))) >> (lCh * 4U);
                }
            }

            /**
               * mappedEvtQ contains the event queue and hence the TC which will
               * process this transfer request. Check whether this TC supports the
               * FIFO size or not.
               */
            defaultBurstSize = (uint32_t)1U << fifoWidth;
            if (defaultBurstSize > drvObject->gblCfgParams.tcDefaultBurstSize[mappedEvtQ])
            {
                result = EDMA3_DRV_E_FIFO_WIDTH_NOT_SUPPORTED;
            }
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* Set the Dest address */
        *((&globalRegs->PARAMENTRY[paRAMId].OPT) +
          (uint32_t)EDMA3_DRV_PARAM_ENTRY_DST) = destAddr;

        opt = (uint32_t)(*(&globalRegs->PARAMENTRY[paRAMId].OPT));

        /* Set DAM */
        opt &= EDMA3_DRV_OPT_DAM_CLR_MASK;
        opt |= EDMA3_DRV_OPT_DAM_SET_MASK(addrMode);
        /* Set FIFO Width */
        opt &= EDMA3_DRV_OPT_FWID_CLR_MASK;
        opt |= EDMA3_DRV_OPT_FWID_SET_MASK(fifoWidth);

        /* Set the OPT */
        *(&globalRegs->PARAMENTRY[paRAMId].OPT) = opt;
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

EDMA3_DRV_Result EDMA3_DRV_setSrcIndex(EDMA3_DRV_Handle hEdma,
                                       uint32_t lCh,
                                       int32_t srcBIdx, int32_t srcCIdx)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    uint32_t srcDstBidx;
    uint32_t srcDstCidx;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    int32_t paRAMId = 0;
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
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (hEdma == NULL)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }

    if (((srcBIdx > EDMA3_DRV_SRCBIDX_MAX_VAL) || (srcBIdx < EDMA3_DRV_SRCBIDX_MIN_VAL)) || ((srcCIdx > EDMA3_DRV_SRCCIDX_MAX_VAL) || (srcCIdx < EDMA3_DRV_SRCCIDX_MIN_VAL)))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
            globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        paRAMId = edma3DrvChBoundRes[edma3Id][lCh].paRAMId;
        if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            /* Get SrcDestBidx PaRAM Set entry */
            srcDstBidx = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX));

            srcDstBidx &= 0xFFFF0000U;
            /* Update it */
            srcDstBidx |= ((uint32_t)srcBIdx & (uint32_t)0xFFFF);

            /* Store it back */
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX) = srcDstBidx;

            /* Get SrcDestCidx PaRAM Set entry */
            srcDstCidx = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX));

            srcDstCidx &= 0xFFFF0000U;
            /* Update it */
            srcDstCidx |= ((uint32_t)srcCIdx & (uint32_t)0xFFFF);

            /* Store it back */
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX) = srcDstCidx;
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

EDMA3_DRV_Result EDMA3_DRV_setDestIndex(EDMA3_DRV_Handle hEdma, uint32_t lCh,
                                        int32_t destBIdx, int32_t destCIdx)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    uint32_t srcDstBidx;
    uint32_t srcDstCidx;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    int32_t paRAMId = 0;
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
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (hEdma == NULL)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }

    if (((destBIdx > EDMA3_DRV_DSTBIDX_MAX_VAL) || (destBIdx < EDMA3_DRV_DSTBIDX_MIN_VAL)) || ((destCIdx > EDMA3_DRV_DSTCIDX_MAX_VAL) || (destCIdx < EDMA3_DRV_DSTCIDX_MIN_VAL)))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
            globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        paRAMId = edma3DrvChBoundRes[edma3Id][lCh].paRAMId;
        if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            /* Get SrcDestBidx PaRAM Set entry */
            srcDstBidx = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX));

            srcDstBidx &= 0xFFFFU;
            /* Update it */
            srcDstBidx |= (((uint32_t)destBIdx & (uint32_t)0xFFFF) << 16U);

            /* Store it back */
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX) = srcDstBidx;

            /* Get SrcDestCidx PaRAM Set entry */
            srcDstCidx = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX));

            srcDstCidx &= 0xFFFFU;
            /* Update it */
            srcDstCidx |= (((uint32_t)destCIdx & (uint32_t)0xFFFF) << 16U);

            /* Store it back */
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX) = srcDstCidx;
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

EDMA3_DRV_Result EDMA3_DRV_setTransferParams(EDMA3_DRV_Handle hEdma,
                                             uint32_t lCh, uint32_t aCnt, uint32_t bCnt, uint32_t cCnt,
                                             uint32_t bCntReload, EDMA3_DRV_SyncType syncType)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    uint32_t abCnt = 0;
    uint32_t linkBCntReld = 0;
    uint32_t opt = 0;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    int32_t paRAMId = 0;
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
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (hEdma == NULL)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }

    if ((((aCnt > EDMA3_DRV_ACNT_MAX_VAL) || (bCnt > EDMA3_DRV_BCNT_MAX_VAL)) || ((cCnt > EDMA3_DRV_CCNT_MAX_VAL) || (bCntReload > EDMA3_DRV_BCNTRELD_MAX_VAL))) || (((int32_t)syncType < (int32_t)EDMA3_DRV_SYNC_A) || (syncType > EDMA3_DRV_SYNC_AB)))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
            globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        paRAMId = edma3DrvChBoundRes[edma3Id][lCh].paRAMId;
        if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            abCnt = aCnt | ((bCnt & 0xFFFFU) << 16U);

            /* Set aCnt and bCnt */
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_ACNT_BCNT) = abCnt;

            /* Set cCnt */
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_CCNT) = cCnt;

            linkBCntReld = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD));

            linkBCntReld |= ((bCntReload & 0xFFFFU) << 16U);

            /* Set bCntReload */
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD) = linkBCntReld;

            opt = (uint32_t)(*(&globalRegs->PARAMENTRY[paRAMId].OPT));

            /* Set Sync Type */
            opt &= EDMA3_DRV_OPT_SYNCDIM_CLR_MASK;
            opt |= EDMA3_DRV_OPT_SYNCDIM_SET_MASK(syncType);

            *(&globalRegs->PARAMENTRY[paRAMId].OPT) = opt;
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

EDMA3_DRV_Result EDMA3_DRV_enableTransfer(EDMA3_DRV_Handle hEdma,
                                          uint32_t lCh,
                                          EDMA3_DRV_TrigMode trigMode)
{
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
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
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (hEdma == NULL)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
            globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }

        /* Trigger type is Manual */
        if ((EDMA3_DRV_TRIG_MODE_MANUAL == trigMode) && (lCh > edma3_dma_ch_max_val[edma3Id]))
        {
            /* Channel Id lies outside DMA channel range */
            result = EDMA3_DRV_E_INVALID_PARAM;
        }

        /* Trigger type is QDMA */
        if ((EDMA3_DRV_TRIG_MODE_QDMA == trigMode) && ((lCh < edma3_qdma_ch_min_val[edma3Id]) || (lCh > edma3_qdma_ch_max_val[edma3Id])))
        {
            /* Channel Id lies outside QDMA channel range */
            result = EDMA3_DRV_E_INVALID_PARAM;
        }

        if (lCh < EDMA3_MAX_DMA_CH)
        {
            /* Trigger type is Event */
            if ((EDMA3_DRV_TRIG_MODE_EVENT == trigMode) && ((drvObject->gblCfgParams.dmaChannelHwEvtMap[lCh / 32U] & ((uint32_t)1U << (lCh % 32U))) == FALSE))
            {
                /* Channel was not mapped to any Hw Event. */
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        switch (trigMode)
        {
        case EDMA3_DRV_TRIG_MODE_MANUAL:
        {
            if (lCh < 32U)
            {
                drvInst->shadowRegs->ESR = (1UL << lCh);
            }
            else
            {
                drvInst->shadowRegs->ESRH = (1UL << (lCh - 32U));
            }
            edma3DrvChBoundRes[edma3Id][lCh].trigMode =
                EDMA3_DRV_TRIG_MODE_MANUAL;
        }
        break;

        case EDMA3_DRV_TRIG_MODE_QDMA:
        {
            drvInst->shadowRegs->QEESR = (1U << (lCh - edma3_qdma_ch_min_val[edma3Id]));
            edma3DrvChBoundRes[edma3Id][lCh].trigMode =
                EDMA3_DRV_TRIG_MODE_QDMA;
        }
        break;

        case EDMA3_DRV_TRIG_MODE_EVENT:
        {
            if (lCh < 32U)
            {
                /*clear SECR to clean any previous NULL request */
                drvInst->shadowRegs->SECR = (1UL << lCh);

                /*clear EMCR to clean any previous NULL request */
                globalRegs->EMCR = (1UL << lCh);

                drvInst->shadowRegs->EESR = (1UL << lCh);
            }
            else
            {
                /*clear SECR to clean any previous NULL request */
                drvInst->shadowRegs->SECRH = (1UL << (lCh - 32U));

                /*clear EMCR to clean any previous NULL request */
                globalRegs->EMCRH = (1UL << (lCh - 32U));

                drvInst->shadowRegs->EESRH = (1UL << (lCh - 32U));
            }

            edma3DrvChBoundRes[edma3Id][lCh].trigMode =
                EDMA3_DRV_TRIG_MODE_EVENT;
        }
        break;

        default:
            result = EDMA3_DRV_E_INVALID_PARAM;
            break;
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

EDMA3_DRV_Result EDMA3_DRV_disableTransfer(EDMA3_DRV_Handle hEdma,
                                           uint32_t lCh, EDMA3_DRV_TrigMode trigMode)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
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
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (hEdma == NULL)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
            globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);
        }
    }
    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }

        /* Trigger type is Manual */
        if ((EDMA3_DRV_TRIG_MODE_MANUAL == trigMode) && (lCh > edma3_dma_ch_max_val[edma3Id]))
        {
            /* Channel Id lies outside DMA channel range */
            result = EDMA3_DRV_E_INVALID_PARAM;
        }

        /* Trigger type is QDMA */
        if ((EDMA3_DRV_TRIG_MODE_QDMA == trigMode) && ((lCh < edma3_qdma_ch_min_val[edma3Id]) || (lCh > edma3_qdma_ch_max_val[edma3Id])))
        {
            /* Channel Id lies outside QDMA channel range */
            result = EDMA3_DRV_E_INVALID_PARAM;
        }

        if (lCh < EDMA3_MAX_DMA_CH)
        {
            /* Trigger type is Event */
            if ((EDMA3_DRV_TRIG_MODE_EVENT == trigMode) && ((drvObject->gblCfgParams.dmaChannelHwEvtMap[lCh / 32U] & ((uint32_t)1U << (lCh % 32U))) == FALSE))
            {
                /* Channel was not mapped to any Hw Event. */
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        switch (trigMode)
        {
        case EDMA3_DRV_TRIG_MODE_MANUAL:
        {
            if (lCh < 32U)
            {
                if ((drvInst->shadowRegs->SER & ((uint32_t)1U << lCh)) != FALSE)
                {
                    drvInst->shadowRegs->SECR = (1U << lCh);
                }
                if ((globalRegs->EMR & ((uint32_t)1U << lCh)) != FALSE)
                {
                    globalRegs->EMCR = (1U << lCh);
                }
            }
            else
            {
                if ((drvInst->shadowRegs->SERH & ((uint32_t)1U << (lCh - 32U))) != FALSE)
                {
                    drvInst->shadowRegs->SECRH = (1U << (lCh - 32U));
                }

                if ((globalRegs->EMRH & ((uint32_t)1U << (lCh - 32U))) != FALSE)
                {
                    globalRegs->EMCRH = (1U << (lCh - 32U));
                }
            }
        }
        break;

        case EDMA3_DRV_TRIG_MODE_QDMA:
        {
            drvInst->shadowRegs->QEECR = (1U << (lCh - edma3_qdma_ch_min_val[edma3Id]));
        }
        break;

        case EDMA3_DRV_TRIG_MODE_EVENT:
        {
            if (lCh < 32U)
            {
                drvInst->shadowRegs->EECR = (1U << lCh);

                if ((drvInst->shadowRegs->ER & ((uint32_t)1U << lCh)) != FALSE)
                {
                    drvInst->shadowRegs->ECR = (1U << lCh);
                }
                if ((drvInst->shadowRegs->SER & ((uint32_t)1U << lCh)) != FALSE)
                {
                    drvInst->shadowRegs->SECR = (1U << lCh);
                }
                if ((globalRegs->EMR & ((uint32_t)1U << lCh)) != FALSE)
                {
                    globalRegs->EMCR = (1U << lCh);
                }
            }
            else
            {
                drvInst->shadowRegs->EECRH = (1U << (lCh - 32U));
                if ((drvInst->shadowRegs->ERH & ((uint32_t)1U << (lCh - 32U))) != FALSE)
                {
                    drvInst->shadowRegs->ECRH = (1U << (lCh - 32U));
                }

                if ((drvInst->shadowRegs->SERH & ((uint32_t)1U << (lCh - 32U))) != FALSE)
                {
                    drvInst->shadowRegs->SECRH = (1U << (lCh - 32U));
                }

                if ((globalRegs->EMRH & ((uint32_t)1U << (lCh - 32U))) != FALSE)
                {
                    globalRegs->EMCRH = (1U << (lCh - 32U));
                }
            }
        }
        break;

        default:
            result = EDMA3_DRV_E_INVALID_PARAM;
            break;
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

EDMA3_DRV_Result EDMA3_DRV_disableLogicalChannel(EDMA3_DRV_Handle hEdma,
                                                 uint32_t lCh, EDMA3_DRV_TrigMode trigMode)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    uint32_t edma3Id;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (hEdma == NULL)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if (drvObject == NULL)
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            edma3Id = drvObject->phyCtrllerInstId;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }

        /* Trigger type is QDMA */
        if ((EDMA3_DRV_TRIG_MODE_QDMA == trigMode) && ((lCh < edma3_qdma_ch_min_val[edma3Id]) || (lCh > edma3_qdma_ch_max_val[edma3Id])))
        {
            /* Channel Id lies outside QDMA channel range */
            result = EDMA3_DRV_E_INVALID_PARAM;
        }

        if (lCh < EDMA3_MAX_DMA_CH)
        {
            /* Trigger type is Event */
            if ((EDMA3_DRV_TRIG_MODE_EVENT == trigMode) && ((drvObject->gblCfgParams.dmaChannelHwEvtMap[lCh / 32U] & ((uint32_t)1U << (lCh % 32U))) == FALSE))
            {
                /* Channel was not mapped to any Hw Event. */
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        switch (trigMode)
        {
        case EDMA3_DRV_TRIG_MODE_QDMA:
        {
            drvInst->shadowRegs->QEECR = (1U << (lCh - edma3_qdma_ch_min_val[edma3Id]));
        }
        break;

        case EDMA3_DRV_TRIG_MODE_EVENT:
        {
            if (lCh < 32U)
            {
                drvInst->shadowRegs->EECR = (1U << lCh);
            }
            else
            {
                drvInst->shadowRegs->EECRH = (1U << (lCh - 32U));
            }
        }
        break;

        default:
            result = EDMA3_DRV_E_INVALID_PARAM;
            break;
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

/** Return the Param ID recently allocated for an EDMA request*/
EDMA3_DRV_Result EDMA3_DRV_getAllocatedPARAMId(EDMA3_DRV_Handle hEdma,
                                               uint32_t channelId, uint32_t *paRAMId)
{
    uint32_t edma3Id = 0;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;

    drvInst = (EDMA3_DRV_Instance *)hEdma;
    drvObject = drvInst->pDrvObjectHandle;

    if (drvObject == NULL)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }

    if (EDMA3_DRV_SOK == result)
    {
        edma3Id = drvObject->phyCtrllerInstId;
    }
    else
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }

    *paRAMId = edma3DrvChBoundRes[edma3Id][channelId].paRAMId;

    return result;
}

/* Definitions of Local functions - Start */
/** Remove various mappings and do cleanup for DMA/QDMA channels */
static EDMA3_DRV_Result edma3RemoveMapping(EDMA3_DRV_Handle hEdma,
                                           uint32_t channelId)
{
    uint32_t intState = 0;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    EDMA3_RM_ResDesc channelObj;
    uint32_t edma3Id;

    assert(hEdma != NULL);

    drvInst = (EDMA3_DRV_Instance *)hEdma;
    drvObject = drvInst->pDrvObjectHandle;

    if ((drvObject == NULL) || (drvObject->gblCfgParams.globalRegs == NULL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
    else
    {
        edma3Id = drvObject->phyCtrllerInstId;
        globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObject->gblCfgParams.globalRegs);
    }

    if (EDMA3_DRV_SOK == result)
    {
        assert(channelId <= edma3_log_ch_max_val[edma3Id]);
    }

    if (EDMA3_DRV_SOK == result)
    {

        /**
     * Disable any ongoing transfer on the channel, if transfer was
     * enabled earlier.
     */
        if (EDMA3_DRV_TRIG_MODE_NONE !=
            edma3DrvChBoundRes[edma3Id][channelId].trigMode)
        {
            result = EDMA3_DRV_disableTransfer(hEdma, channelId,
                                               edma3DrvChBoundRes[edma3Id][channelId].trigMode);
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /*
        * Unregister the TCC Callback function and disable the interrupts.
        */
        if (channelId < drvObject->gblCfgParams.numDmaChannels)
        {
            /* DMA channel */
            channelObj.type = EDMA3_RM_RES_DMA_CHANNEL;
            channelObj.resId = channelId;
        }
        else
        {
            /* QDMA channel */
            channelObj.type = EDMA3_RM_RES_QDMA_CHANNEL;
            channelObj.resId = channelId - edma3_qdma_ch_min_val[edma3Id];
        }

        result = EDMA3_RM_unregisterTccCb(drvInst->resMgrInstance,
                                          (EDMA3_RM_ResDesc *)&channelObj);
    }

    if (result == EDMA3_RM_SOK)
    {
        edma3OsProtectEntry(edma3Id, (int32_t)EDMA3_OS_PROTECT_INTERRUPT, &intState);

        if (channelId <= edma3_dma_ch_max_val[edma3Id])
        {
            if ((channelId >> 3U) < 8)
            {
                /* DMA channel */
#ifndef EDMA3_PROGRAM_QUEUE_NUM_REGISTER_INIT_TIME
                /* Remove the channel to Event Queue mapping */
                globalRegs->DMAQNUM[channelId >> 3U] &=
                    EDMA3_DRV_DMAQNUM_CLR_MASK(channelId);
#endif
                /**
                 * If DMA channel to PaRAM Set mapping exists,
                 * remove it too.
                 */
                if (TRUE == drvObject->gblCfgParams.dmaChPaRAMMapExists)
                {
                    globalRegs->DCHMAP[channelId] &=
                        EDMA3_RM_DCH_PARAM_CLR_MASK;
                }
            }
            else
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }
        else
        {
            /* QDMA channel */
#ifndef EDMA3_PROGRAM_QUEUE_NUM_REGISTER_INIT_TIME
            /* Remove the channel to Event Queue mapping */
            globalRegs->QDMAQNUM = (globalRegs->QDMAQNUM) &
                                   (EDMA3_DRV_QDMAQNUM_CLR_MASK(channelId - edma3_qdma_ch_min_val[edma3Id]));
#endif
            /* Remove the channel to PARAM set mapping */
            /* Unmap PARAM Set Number for specified channelId */
            globalRegs->QCHMAP[channelId - edma3_qdma_ch_min_val[edma3Id]] &=
                EDMA3_RM_QCH_PARAM_CLR_MASK;

            /* Reset the Trigger Word */
            globalRegs->QCHMAP[channelId - edma3_qdma_ch_min_val[edma3Id]] &=
                EDMA3_RM_QCH_TRWORD_CLR_MASK;
        }

        edma3OsProtectExit(edma3Id,
                           (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                           intState);
    }

    return result;
}
uint32_t EDMA3_DRV_getVersion(void)
{
    return EDMA3_LLD_DRV_VERSION_ID;
}

const char *EDMA3_DRV_getVersionStr(void)
{
    return edma3LldVersionStr;
}
/* Definitions of Local functions - End */

/* End of File */
