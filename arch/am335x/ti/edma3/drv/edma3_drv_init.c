/*
 * edma3_drv_init.c
 *
 * EDMA3 Driver Initialization Interface Implementation. This file contains
 * EDMA3 Driver APIs used to:
 * a) Create/delete EDMA3 Driver Object
 * b) Open/close EDMA3 Driver Instance.
 * These APIs are required to initialize EDMA3 properly.
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

/* EDMA3 Driver Internal Header Files */
#include <ti/edma3/drv/edma3.h>
/* Resource Manager Internal Header Files */
#include <ti/edma3/rm/edma3resmgr.h>

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
/** Local MemZero function */
extern void edma3MemZero(void *dst, uint32_t len);
/** Local MemCpy function */
extern void edma3MemCpy(void *dst, const void *src, uint32_t len);

/**
 * \brief EDMA3 Driver Objects, tied to each EDMA3 HW Controller.
 *
 * Typically one object will cater to one EDMA3 HW controller
 * and will have all regions' (ARM, DSP etc) specific config information.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(drvObj, ".fardata:.edma3Globals");
#endif
EDMA3_DRV_Object drvObj[EDMA3_MAX_EDMA3_INSTANCES];

/**
 * Handles of EDMA3 Driver Instances.
 *
 * Used to maintain information of the EDMA3 Driver Instances for
 * each region, for each HW controller.
 * There could be as many Driver Instances as there are shadow
 * regions. Multiple EDMA3 Driver instances on the same shadow
 * region are NOT allowed.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(drvInstance, ".fardata:.edma3Globals");
#endif
EDMA3_DRV_Instance drvInstance[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_REGIONS];

/**
 * \brief Resources bound to a Channel
 *
 * When a request for a channel is made, the resources PaRAM Set and TCC
 * get bound to that channel. This information is needed internally by the
 * driver when a request is made to free the channel (Since it is the
 * responsibility of the driver to free up the channel-associated resources
 * from the Resource Manager layer).
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(edma3DrvChBoundRes, ".fardata:.edma3Globals");
#endif
EDMA3_DRV_ChBoundResources edma3DrvChBoundRes[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_LOGICAL_CH];

/**
 * \brief Event Queue Number for DMA/QDMA Channels.
 *
 * If EDMA3_PROGRAM_QUEUE_NUM_REGISTER_INIT_TIME is defined, then the queue
 * number registers for DMA/QDMA channels will be programmed during init time
 * only; the queue number registers will NOT be changed during run-time to
 * avoid (potential) race conditions. The following arrays will be used to
 * program the DMA/QDMA queue number registers.
 *
 * User has to provide the array values depending on the resources'
 * availability and the system requirements.
 *
 */
#ifdef EDMA3_PROGRAM_QUEUE_NUM_REGISTER_INIT_TIME
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(edma3DmaQdmaQueueNumConfig, ".fardata:.edma3Globals");
#endif
EDMA3_DRV_DmaQdmaQueueNum edma3DmaQdmaQueueNumConfig = {
    {
        /* DMA Channels 0-63 */
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
    },

    {
        /* QDMA Channels 0-7 */
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
        0u,
        1u,
    },
};
#endif

/**
 * Used to reset the Internal EDMA3 Driver Data Structures for the first time.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(drvInitDone, ".fardata:.edma3Globals");
#endif
static uint16_t drvInitDone = FALSE;

/* Local functions prototypes */
/*---------------------------------------------------------------------------*/
/**
 * Local function to prepare the init config structure for
 * open of Resource Manager
 */
static EDMA3_DRV_Result edma3OpenResMgr(uint32_t instId,
                                        uint32_t regionId,
                                        uint16_t flag);

/*---------------------------------------------------------------------------*/

EDMA3_DRV_Result EDMA3_DRV_create(uint32_t phyCtrllerInstId,
                                  const EDMA3_DRV_GblConfigParams *gblCfgParams,
                                  const void *miscParam)
{
    uint32_t count = 0;
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_RM_GblConfigParams rmGblCfgParams;

    /**
     * We are NOT checking 'gblCfgParams' for NULL. Whatever user has passed
     * is given to RM. If user passed NULL, config info from config file will be
     * taken else user specific info will be passed to the RM.
     * Similarly, 'miscParam' is not being checked and passed as it is to the
     * Resource Manager layer.
     */
    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (phyCtrllerInstId >= EDMA3_MAX_EDMA3_INSTANCES)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        /* Initialize the global variables for the first time */
        if (FALSE == drvInitDone)
        {
            for (count = 0; count < EDMA3_MAX_EDMA3_INSTANCES; count++)
            {
                edma3MemZero((void *)&(drvObj[count]),
                             sizeof(EDMA3_DRV_Object));
            }
            drvInitDone = TRUE;
        }

        /* Initialization has been done */
        if (drvObj[phyCtrllerInstId].state != EDMA3_DRV_DELETED)
        {
            result = EDMA3_DRV_E_OBJ_NOT_DELETED;
        }
        else
        {
            if (NULL != gblCfgParams)
            {
                /* User has passed the configuration info */
                /* copy the global info */
                edma3MemCpy((void *)(&drvObj[phyCtrllerInstId].gblCfgParams),
                            (const void *)(gblCfgParams),
                            sizeof(EDMA3_DRV_GblConfigParams));

                /* Reset the RM global info struct first */
                edma3MemZero((void *)&(rmGblCfgParams),
                             sizeof(EDMA3_RM_GblConfigParams));

                /* Fill the RM global info struct with the DRV global info */
                edma3MemCpy((void *)(&rmGblCfgParams),
                            (const void *)(&drvObj[phyCtrllerInstId].gblCfgParams),
                            sizeof(EDMA3_RM_GblConfigParams));

                result = EDMA3_RM_create(phyCtrllerInstId, (EDMA3_RM_GblConfigParams *)&rmGblCfgParams, miscParam);
            }
            else
            {
                /* User has not passed any global info. */
                result = EDMA3_RM_create(phyCtrllerInstId, NULL, miscParam);

                if (EDMA3_RM_SOK == result)
                {
                    /**
                     * Copy the global config info from the RM object to the
                     * driver object for future use.
                     */
                    /* Fill the RM global info struct with the DRV global info */
                    edma3MemCpy((void *)(&drvObj[phyCtrllerInstId].gblCfgParams),
                                (const void *)(&resMgrObj[phyCtrllerInstId].gblCfgParams),
                                sizeof(EDMA3_RM_GblConfigParams));
                }
            }

            if (EDMA3_RM_SOK == result)
            {
                drvObj[phyCtrllerInstId].state = EDMA3_DRV_CREATED;
                drvObj[phyCtrllerInstId].numOpens = 0;
                drvObj[phyCtrllerInstId].phyCtrllerInstId = phyCtrllerInstId;

                /* Make all the Driver instances for this EDMA3 HW NULL */
                for (count = 0; count < drvObj[phyCtrllerInstId].gblCfgParams.numRegions; count++)
                {
                    edma3MemZero((void *)&(drvInstance[phyCtrllerInstId][count]),
                                 sizeof(EDMA3_DRV_Instance));
                }

                /* Reset edma3DrvChBoundRes Array*/
                for (count = 0; count < EDMA3_MAX_LOGICAL_CH; count++)
                {
                    edma3DrvChBoundRes[phyCtrllerInstId][count].paRAMId = -1;
                    edma3DrvChBoundRes[phyCtrllerInstId][count].tcc = EDMA3_MAX_TCC;
                    edma3DrvChBoundRes[phyCtrllerInstId][count].trigMode =
                        EDMA3_DRV_TRIG_MODE_NONE;
                }
            }

/* Initialize the Queue Number Registers, if required. */
#ifdef EDMA3_PROGRAM_QUEUE_NUM_REGISTER_INIT_TIME
            if (EDMA3_RM_SOK == result)
            {
                volatile EDMA3_CCRL_Regs *globalRegs = NULL;

                globalRegs = (volatile EDMA3_CCRL_Regs *)(drvObj[phyCtrllerInstId].gblCfgParams.globalRegs);

                if (NULL != globalRegs)
                {
                    /* Initialize DMA Queue Num Registers */
                    for (count = 0;
                         count < drvObj[phyCtrllerInstId].gblCfgParams.numDmaChannels;
                         count++)
                    {
                        globalRegs->DMAQNUM[count >> 3u] &= EDMA3_DRV_DMAQNUM_CLR_MASK(count);
                        globalRegs->DMAQNUM[count >> 3u] |= EDMA3_DRV_DMAQNUM_SET_MASK(count,
                                                                                       edma3DmaQdmaQueueNumConfig.dmaChannelQueueNum[count]);
                    }

                    /* Initialize QDMA Queue Num Registers */
                    for (count = 0;
                         count < drvObj[phyCtrllerInstId].gblCfgParams.numQdmaChannels;
                         count++)
                    {
                        globalRegs->QDMAQNUM &= EDMA3_DRV_QDMAQNUM_CLR_MASK(count);
                        globalRegs->QDMAQNUM |= EDMA3_DRV_QDMAQNUM_SET_MASK(count,
                                                                            edma3DmaQdmaQueueNumConfig.qdmaChannelQueueNum[count]);
                    }
                }
            }
#endif
        }
    }

    return result;
}

EDMA3_DRV_Result EDMA3_DRV_delete(uint32_t phyCtrllerInstId,
                                  const void *param)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (phyCtrllerInstId >= EDMA3_MAX_EDMA3_INSTANCES)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        /**
         * If number of Driver Instances is 0, then state should be
         * EDMA3_DRV_CLOSED OR EDMA3_DRV_CREATED.
         */
        if ((0 == drvObj[phyCtrllerInstId].numOpens) && ((drvObj[phyCtrllerInstId].state != EDMA3_DRV_CLOSED) && (drvObj[phyCtrllerInstId].state != EDMA3_DRV_CREATED)))
        {
            result = EDMA3_DRV_E_OBJ_NOT_CLOSED;
        }
        else
        {
            /**
             * If number of Driver Instances is NOT 0, then this function
             * SHOULD NOT be called by anybody.
             */
            if (0 != drvObj[phyCtrllerInstId].numOpens)
            {
                result = EDMA3_DRV_E_INVALID_STATE;
            }
            else
            {
                /**
                 * State is correct. Delete the RM Object.
                 */
                result = EDMA3_RM_delete(phyCtrllerInstId, NULL);

                if (EDMA3_RM_SOK == result)
                {
                    /** Change state to EDMA3_DRV_DELETED */
                    drvObj[phyCtrllerInstId].state = EDMA3_DRV_DELETED;

                    /* Also, reset the Driver Object Global Config Info */
                    edma3MemZero((void *)&(drvObj[phyCtrllerInstId].gblCfgParams),
                                 sizeof(EDMA3_DRV_GblConfigParams));
                }
            }
        }
    }

    return result;
}

EDMA3_DRV_Handle EDMA3_DRV_open(uint32_t phyCtrllerInstId,
                                const EDMA3_DRV_InitConfig *initCfg,
                                EDMA3_DRV_Result *errorCode)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Object *drvObject = NULL;
    EDMA3_DRV_Instance *drvInstanceHandle = NULL;
    uint32_t intState = 0;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    uint16_t flag = 0;

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (((initCfg == NULL) || (phyCtrllerInstId >= EDMA3_MAX_EDMA3_INSTANCES)) || (errorCode == NULL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        /* Check whether the semaphore handle is null or not */
        if (NULL == initCfg->drvSemHandle)
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            drvObject = &drvObj[phyCtrllerInstId];
            if (initCfg->regionId >= drvObject->gblCfgParams.numRegions)
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
            else
            {
                /* if no instance is opened and this is the first one,
                    * then state should be created/closed.
                    */
                if (((drvObject->numOpens == 0) && (drvObject->state != EDMA3_DRV_CREATED)) && (drvObject->state != EDMA3_DRV_CLOSED))
                {
                    result = EDMA3_DRV_E_INVALID_STATE;
                }
                else
                {
                    /* if num of instances opened is more than 0 and less than no of regions,
                        * then state should be opened.
                        */
                    if (((drvObject->numOpens > 0) && (drvObject->numOpens < drvObject->gblCfgParams.numRegions)) && (drvObject->state != EDMA3_DRV_OPENED))
                    {
                        result = EDMA3_DRV_E_INVALID_STATE;
                    }
                    else
                    {
                        /* if a driver instance is already there for a specific region,
                            *  it should return an error.
                            */
                        drvInstanceHandle = &drvInstance[phyCtrllerInstId][initCfg->regionId];
                        if (drvInstanceHandle->pDrvObjectHandle != NULL)
                        {
                            drvInstanceHandle = NULL;
                            result = EDMA3_DRV_E_INST_ALREADY_EXISTS;
                        }
                    }
                }
            }
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* Save the region specific information in the region specific drv instance*/
        drvInstanceHandle->regionId = initCfg->regionId;
        drvInstanceHandle->isMaster = initCfg->isMaster;
        drvInstanceHandle->drvSemHandle = initCfg->drvSemHandle;
        drvInstanceHandle->gblerrCbParams.gblerrCb = initCfg->gblerrCb;
        drvInstanceHandle->gblerrCbParams.gblerrData = initCfg->gblerrData;

        if (NULL != initCfg->drvInstInitConfig)
        {
            edma3MemCpy((void *)(&drvInstanceHandle->drvInstInitConfig),
                        (const void *)(initCfg->drvInstInitConfig),
                        sizeof(EDMA3_DRV_InstanceInitConfig));

            /* Flag to remember that driver has passed config info to RM */
            flag = 1U;
        }

        if (NULL == drvObject->gblCfgParams.globalRegs)
        {
            drvInstanceHandle = NULL;
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;

            /* Update shadowRegs */
            drvInstanceHandle->shadowRegs = (EDMA3_CCRL_ShadowRegs *)(&(globalRegs->SHADOW[initCfg->regionId]));

            result = edma3OpenResMgr(phyCtrllerInstId, initCfg->regionId, flag);
            if (EDMA3_DRV_SOK != result)
            {
                drvInstanceHandle = NULL;
            }
            else
            {
                drvObject->state = EDMA3_DRV_OPENED;
                edma3OsProtectEntry(phyCtrllerInstId,
                                    (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                    &intState);
                drvObject->numOpens++;
                edma3OsProtectExit(phyCtrllerInstId,
                                   (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                   intState);
            }
        }
    }

    if (errorCode)
    {
        *errorCode = result;
    }
    return (EDMA3_DRV_Handle)drvInstanceHandle;
}

EDMA3_DRV_Result EDMA3_DRV_close(EDMA3_DRV_Handle hEdma,
                                 const void *param)
{
    uint32_t intState = 0;
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;

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
            /* Check state of driver */
            if (drvObject->state != EDMA3_DRV_OPENED)
            {
                result = EDMA3_DRV_E_OBJ_NOT_OPENED;
            }
            else
            {
                result = EDMA3_RM_close(drvInst->resMgrInstance, NULL);

                if (result != EDMA3_RM_SOK)
                {
                    result = EDMA3_DRV_E_RM_CLOSE_FAIL;
                }
                else
                {
                    /* Set the driver instance specific info null */
                    drvInst->resMgrInstance = NULL;
                    drvInst->pDrvObjectHandle = NULL;
                    edma3MemZero((void *)&(drvInst->drvInstInitConfig),
                                 sizeof(EDMA3_DRV_InstanceInitConfig));
                    drvInst->shadowRegs = NULL;

                    edma3OsProtectEntry(drvObject->phyCtrllerInstId,
                                        (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                        &intState);
                    /* Decrease the Number of Opens */
                    --drvObject->numOpens;
                    if (0 == drvObject->numOpens)
                    {
                        drvObject->state = EDMA3_DRV_CLOSED;
                    }
                    edma3OsProtectExit(drvObject->phyCtrllerInstId,
                                       (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                       intState);
                }
            }
        }
    }

    return (result);
}

/* Definitions of Local functions - Start */
/* Local function to prepare the init config structure for open of Resource Manager */
static EDMA3_DRV_Result edma3OpenResMgr(uint32_t instId,
                                        uint32_t regionId,
                                        uint16_t flag)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_RM_Param initParam;
    EDMA3_RM_InstanceInitConfig rmInstanceCfg;
    EDMA3_RM_Handle hResMgr = NULL;
    EDMA3_RM_Result rmResult;
    uint32_t resMgrIdx = 0U;
    EDMA3_RM_Instance *temp_rm_instance = NULL;

    assert((instId < EDMA3_MAX_EDMA3_INSTANCES) && (regionId < drvObj[instId].gblCfgParams.numRegions));

    initParam.regionId = regionId;
    initParam.rmSemHandle = drvInstance[instId][regionId].drvSemHandle;
    /*
    * If the EDMA driver instance is MASTER, do the
    * (global + region_specific) init. For all other instances,
    * only do the (region_specific) init.
    */
    initParam.isMaster = drvInstance[instId][regionId].isMaster;
    initParam.regionInitEnable = TRUE;

    initParam.gblerrCbParams.gblerrCb = drvInstance[instId][regionId].gblerrCbParams.gblerrCb;
    initParam.gblerrCbParams.gblerrData = drvInstance[instId][regionId].gblerrCbParams.gblerrData;

    if (flag == 1U)
    {
        /**
         * User has passed the instance initialization specific info,
         * which we have saved previously too, so use it.
         */
        edma3MemCpy((void *)(&rmInstanceCfg),
                    (const void *)(&drvInstance[instId][regionId].drvInstInitConfig),
                    sizeof(EDMA3_RM_InstanceInitConfig));

        initParam.rmInstInitConfig = &rmInstanceCfg;

        hResMgr = EDMA3_RM_open(instId, (EDMA3_RM_Param *)&initParam, &rmResult);

        if (NULL == hResMgr)
        {
            result = rmResult;
        }
    }
    else
    {
        /**
         * User has NOT passed the instance initialization specific info.
         * Pass NULL to the Resource Manager.
         */
        initParam.rmInstInitConfig = NULL;

        hResMgr = EDMA3_RM_open(instId, (EDMA3_RM_Param *)&initParam, &rmResult);

        if (NULL == hResMgr)
        {
            result = rmResult;
        }
        else
        {
            /**
             * Save the RM Instance specific information in the driver.
             * Earlier this was easier, now a bit tricky.
             * Search for the RM instance number based on the handle
             * just returned, to fetch the correct config info from  the
             * userInitConfig[].
             */
            for (resMgrIdx = 0U; resMgrIdx < EDMA3_MAX_RM_INSTANCES; resMgrIdx++)
            {
                temp_rm_instance = ((EDMA3_RM_Instance *)(ptrRMIArray) + (instId * EDMA3_MAX_RM_INSTANCES) + resMgrIdx);

                if (hResMgr == temp_rm_instance)
                {
                    /* RM Id found. Copy the specific config info to the drvInstance [] */
                    edma3MemCpy((void *)(&drvInstance[instId][regionId].drvInstInitConfig),
                                (const void *)((EDMA3_RM_InstanceInitConfig *)(ptrInitCfgArray) + (instId * EDMA3_MAX_RM_INSTANCES) + resMgrIdx),
                                sizeof(EDMA3_RM_InstanceInitConfig));
                    break;
                }
            }

            if (EDMA3_MAX_RM_INSTANCES == resMgrIdx)
            {
                /* RM Id not found, report error... */
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }
    }

    if (EDMA3_RM_SOK == result)
    {
        /* Save handle to Resource Manager Instance */
        drvInstance[instId][regionId].resMgrInstance = hResMgr;
        /* Save handle to EDMA Driver Object */
        drvInstance[instId][regionId].pDrvObjectHandle = &drvObj[instId];
    }

    return result;
}

EDMA3_DRV_Result EDMA3_DRV_initXbarEventMap(EDMA3_DRV_Handle hEdma,
                                            const EDMA3_DRV_GblXbarToChanConfigParams *edmaGblXbarConfig,
                                            EDMA3_DRV_mapXbarEvtToChan mapXbarEvtFunc,
                                            EDMA3_DRV_xbarConfigScr configXbarScr)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    uint32_t mapXbarToChanFuncPtrTemp = 0U;
    EDMA3_RM_mapXbarEvtToChan mapXbarToChanFuncPtr = NULL;
    uint32_t configScrMapXbarToEvtFuncPtrTemp = 0;
    EDMA3_RM_xbarConfigScr configScrMapXbarToEvtFuncPtr = NULL;
    EDMA3_RM_GblXbarToChanConfigParams drvXbarToEvtMapConfigTemp;
    const EDMA3_RM_GblXbarToChanConfigParams *drvXbarToEvtMapConfigTempPtr = NULL;

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

        if (mapXbarEvtFunc != NULL)
        {
            mapXbarToChanFuncPtrTemp = (uint32_t)&mapXbarEvtFunc;
            mapXbarToChanFuncPtr = (EDMA3_RM_mapXbarEvtToChan)mapXbarToChanFuncPtrTemp;
        }
        if (configXbarScr != NULL)
        {
            configScrMapXbarToEvtFuncPtrTemp = (uint32_t)&configXbarScr;
            configScrMapXbarToEvtFuncPtr = (EDMA3_RM_xbarConfigScr)configScrMapXbarToEvtFuncPtrTemp;
        }
        if (edmaGblXbarConfig != NULL)
        {
            edma3MemCpy((void *)(&drvXbarToEvtMapConfigTemp),
                        (const void *)(edmaGblXbarConfig),
                        sizeof(EDMA3_RM_GblXbarToChanConfigParams));
            drvXbarToEvtMapConfigTempPtr = &drvXbarToEvtMapConfigTemp;
        }
        result = EDMA3_RM_initXbarEventMap(drvInst->resMgrInstance,
                                           drvXbarToEvtMapConfigTempPtr,
                                           mapXbarToChanFuncPtr,
                                           configScrMapXbarToEvtFuncPtr);
    }

    return (result);
}
/* Definitions of Local functions - End */

/* End of File */
