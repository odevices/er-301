/*
 * edma3_drv_adv.c
 *
 * EDMA3 Driver Advanced Interface Implementation. This file contains
 * advanced-level EDMA3 Driver APIs which are required to:
 * a) Link and chain two channels.
 * b) Set/get the whole PaRAM Set in one shot.
 * c) Set/get each individual field of the PaRAM Set.
 * d) Poll mode APIs.
 * e) IOCTL interface.
 * These APIs are provided to have complete control on the EDMA3 hardware and
 * normally advanced users are expected to use them for their specific
 * use-cases.
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

/* Instrumentation Header File */
#ifdef EDMA3_INSTRUMENTATION_ENABLED
#include <ti/edma3/rm/edma3_log.h>
#endif

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
/** Local MemCpy function to copy PaRAM Set ONLY */
extern void edma3ParamCpy(volatile void *dst, const volatile void *src);

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

EDMA3_DRV_Result EDMA3_DRV_linkChannel(EDMA3_DRV_Handle hEdma,
                                       uint32_t lCh1, uint32_t lCh2)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    uint32_t linkBcntReld;
    int32_t paRAM1Id = 0;
    int32_t paRAM2Id = 0;
    uint32_t oldTccVal = 0;
    uint32_t optVal = 0;
    uint32_t newOptVal = 0;
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if ((lCh1 > edma3_log_ch_max_val[edma3Id]) || (lCh2 > edma3_log_ch_max_val[edma3Id]))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        if ((lCh1 < EDMA3_MAX_LOGICAL_CH) && (lCh2 < EDMA3_MAX_LOGICAL_CH))
        {
            paRAM1Id = edma3DrvChBoundRes[edma3Id][lCh1].paRAMId;
            paRAM2Id = edma3DrvChBoundRes[edma3Id][lCh2].paRAMId;

            if ((paRAM1Id < 0) || (paRAM1Id >= drvObject->gblCfgParams.numPaRAMSets))
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }

            if ((paRAM2Id < 0) || (paRAM2Id >= drvObject->gblCfgParams.numPaRAMSets))
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }
        else
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        /* Get the Link-bcntReload PaRAM set entry */
        linkBcntReld = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAM1Id].OPT) +
                                    EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD));
        linkBcntReld &= 0xFFFF0000U;
        /* Update the Link field with lch2 PaRAM set */
        linkBcntReld |= ((uint32_t)0xFFFFU & (uint32_t)(&(globalRegs->PARAMENTRY[paRAM2Id].OPT)));

        /* Store it back */
        *((&globalRegs->PARAMENTRY[paRAM1Id].OPT) + EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD) = linkBcntReld;

        /*
        * Set the TCC field of PaRAM set associated with lch2 to
        * the same as that of lch1. This should be done ONLY when
        * lch2 is NOT associated with any other TCC.
        */
        if (edma3DrvChBoundRes[edma3Id][lCh2].tcc == EDMA3_MAX_TCC)
        {
            /* for channel 1 */
            optVal = (uint32_t)(*(&globalRegs->PARAMENTRY[paRAM1Id].OPT));
            oldTccVal = EDMA3_DRV_OPT_TCC_GET_MASK(optVal);

            /* for channel 2 */
            optVal = (uint32_t)(*(&globalRegs->PARAMENTRY[paRAM2Id].OPT));
            newOptVal = (optVal & EDMA3_DRV_OPT_TCC_CLR_MASK) |
                        (EDMA3_DRV_OPT_TCC_SET_MASK(oldTccVal));
            *(&globalRegs->PARAMENTRY[paRAM2Id].OPT) = newOptVal;
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

EDMA3_DRV_Result EDMA3_DRV_unlinkChannel(EDMA3_DRV_Handle hEdma, uint32_t lCh)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    uint32_t linkBcntReld;
    int32_t paRAMId = 0;
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
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
        if (lCh < EDMA3_MAX_LOGICAL_CH)
        {
            paRAMId = edma3DrvChBoundRes[edma3Id][lCh].paRAMId;
            if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
            else
            {
                /* Get the Link-bcntReload PaRAM set entry */
                linkBcntReld = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD));

                /* Remove any linking */
                linkBcntReld |= 0xFFFFU;

                /* Store it back */
                *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD) = linkBcntReld;
            }
        }
        else
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
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

EDMA3_DRV_Result EDMA3_DRV_chainChannel(EDMA3_DRV_Handle hEdma,
                                        uint32_t lCh1,
                                        uint32_t lCh2,
                                        const EDMA3_DRV_ChainOptions *chainOptions)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    uint32_t opt = 0x0;
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

    if (chainOptions == NULL)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_DRV_SOK)
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if ((lCh1 > edma3_log_ch_max_val[edma3Id]) || (lCh2 > edma3_dma_ch_max_val[edma3Id]))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        paRAMId = edma3DrvChBoundRes[edma3Id][lCh1].paRAMId;
        if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        opt = (uint32_t)(*(&globalRegs->PARAMENTRY[paRAMId].OPT));

        /* set Transfer complete chaining */
        if (chainOptions->tcchEn == EDMA3_DRV_TCCHEN_EN)
        {
            opt |= EDMA3_DRV_OPT_TCCHEN_SET_MASK(1U);
        }
        else
        {
            opt &= EDMA3_DRV_OPT_TCCHEN_CLR_MASK;
        }

        /*set Intermediate transfer completion chaining */
        if (chainOptions->itcchEn == EDMA3_DRV_ITCCHEN_EN)
        {
            opt |= EDMA3_DRV_OPT_ITCCHEN_SET_MASK(1U);
        }
        else
        {
            opt &= EDMA3_DRV_OPT_ITCCHEN_CLR_MASK;
        }

        /*set Transfer complete interrupt */
        if (chainOptions->tcintEn == EDMA3_DRV_TCINTEN_EN)
        {
            opt |= EDMA3_DRV_OPT_TCINTEN_SET_MASK(1U);
        }
        else
        {
            opt &= EDMA3_DRV_OPT_TCINTEN_CLR_MASK;
        }

        /*set Intermediate transfer completion interrupt */
        if (chainOptions->itcintEn == EDMA3_DRV_ITCINTEN_EN)
        {
            opt |= EDMA3_DRV_OPT_ITCINTEN_SET_MASK(1U);
        }
        else
        {
            opt &= EDMA3_DRV_OPT_ITCINTEN_CLR_MASK;
        }

        opt &= EDMA3_DRV_OPT_TCC_CLR_MASK;
        opt |= EDMA3_DRV_OPT_TCC_SET_MASK(lCh2);

        *(&globalRegs->PARAMENTRY[paRAMId].OPT) = opt;

        /* Set the trigger mode of lch2 as the same as of lch1 */
        edma3DrvChBoundRes[edma3Id][lCh2].trigMode =
            edma3DrvChBoundRes[edma3Id][lCh1].trigMode;
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

EDMA3_DRV_Result EDMA3_DRV_unchainChannel(EDMA3_DRV_Handle hEdma,
                                          uint32_t lCh)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    uint32_t opt;
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
#endif

    if (result == EDMA3_DRV_SOK)
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (result == EDMA3_DRV_SOK)
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

    if (result == EDMA3_DRV_SOK)
    {
        opt = (uint32_t)(*(&globalRegs->PARAMENTRY[paRAMId].OPT));

        /* Reset TCCHEN */
        opt &= EDMA3_DRV_OPT_TCCHEN_CLR_MASK;
        /* Reset ITCCHEN */
        opt &= EDMA3_DRV_OPT_ITCCHEN_CLR_MASK;

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

EDMA3_DRV_Result EDMA3_DRV_setQdmaTrigWord(EDMA3_DRV_Handle hEdma,
                                           uint32_t lCh,
                                           EDMA3_RM_QdmaTrigWord trigWord)
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
    if ((hEdma == NULL) || (((int32_t)trigWord < (int32_t)EDMA3_RM_QDMA_TRIG_OPT) || (trigWord > EDMA3_RM_QDMA_TRIG_CCNT)))
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if ((lCh < edma3_qdma_ch_min_val[edma3Id]) || (lCh > edma3_qdma_ch_max_val[edma3Id]))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        globalRegs->QCHMAP[lCh - edma3_qdma_ch_min_val[edma3Id]] &= EDMA3_DRV_QCH_TRWORD_CLR_MASK;
        globalRegs->QCHMAP[lCh - edma3_qdma_ch_min_val[edma3Id]] |= EDMA3_DRV_QCH_TRWORD_SET_MASK(trigWord);
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

EDMA3_DRV_Result EDMA3_DRV_setPaRAM(EDMA3_DRV_Handle hEdma,
                                    uint32_t lCh,
                                    const EDMA3_DRV_PaRAMRegs *newPaRAM)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
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
    if ((hEdma == NULL) || (newPaRAM == NULL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_DRV_SOK)
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (result == EDMA3_DRV_SOK)
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

    if (result == EDMA3_DRV_SOK)
    {
        edma3ParamCpy((volatile void *)(&(globalRegs->PARAMENTRY[paRAMId].OPT)),
                      (const void *)newPaRAM);
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

EDMA3_DRV_Result EDMA3_DRV_getPaRAM(EDMA3_DRV_Handle hEdma,
                                    uint32_t lCh,
                                    EDMA3_DRV_PaRAMRegs *currPaRAM)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
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
    if ((hEdma == NULL) || (currPaRAM == NULL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_DRV_SOK)
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (result == EDMA3_DRV_SOK)
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
        if (lCh < EDMA3_MAX_LOGICAL_CH)
        {
            paRAMId = edma3DrvChBoundRes[edma3Id][lCh].paRAMId;
            if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }
        else
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        edma3ParamCpy((void *)currPaRAM,
                      (const volatile void *)(&(globalRegs->PARAMENTRY[paRAMId].OPT)));
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

EDMA3_DRV_Result EDMA3_DRV_setPaRAMEntry(EDMA3_DRV_Handle hEdma,
                                         uint32_t lCh,
                                         EDMA3_DRV_PaRAMEntry paRAMEntry,
                                         uint32_t newPaRAMEntryVal)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
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
    if ((hEdma == NULL) || (((int32_t)paRAMEntry < (int32_t)EDMA3_DRV_PARAM_ENTRY_OPT) || (paRAMEntry > EDMA3_DRV_PARAM_ENTRY_CCNT)))
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (result == EDMA3_DRV_SOK)
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
        if (lCh < EDMA3_MAX_LOGICAL_CH)
        {
            paRAMId = edma3DrvChBoundRes[edma3Id][lCh].paRAMId;
            if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
            else
            {
                *((&globalRegs->PARAMENTRY[paRAMId].OPT) +
                  (uint32_t)paRAMEntry) = newPaRAMEntryVal;
            }
        }
        else
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
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

EDMA3_DRV_Result EDMA3_DRV_getPaRAMEntry(EDMA3_DRV_Handle hEdma,
                                         uint32_t lCh,
                                         EDMA3_DRV_PaRAMEntry paRAMEntry,
                                         uint32_t *paRAMEntryVal)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
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
    if ((hEdma == NULL) || (paRAMEntryVal == NULL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }

    if (((int32_t)paRAMEntry < (int32_t)EDMA3_DRV_PARAM_ENTRY_OPT) || (paRAMEntry > EDMA3_DRV_PARAM_ENTRY_CCNT))
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (result == EDMA3_DRV_SOK)
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
            *paRAMEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + (uint32_t)paRAMEntry));
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

EDMA3_DRV_Result EDMA3_DRV_setPaRAMField(EDMA3_DRV_Handle hEdma,
                                         uint32_t lCh,
                                         EDMA3_DRV_PaRAMField paRAMField,
                                         uint32_t newPaRAMFieldVal)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    uint32_t paramEntryVal = 0;
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
    if ((hEdma == NULL) || (((int32_t)paRAMField < (int32_t)EDMA3_DRV_PARAM_FIELD_OPT) || (paRAMField > EDMA3_DRV_PARAM_FIELD_CCNT)))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_DRV_SOK)
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        /*
    * THIS API IS NOT ALLOWED FOR QDMA CHANNELS.
    * Reason being setting one PaRAM field might trigger the
    * transfer if the word written happends to be the trigger
    * word. One should use EDMA3_DRV_setPaRAMEntry ()
    * API instead to write the whole 32 bit word.
    */
        if ((lCh <= edma3_qdma_ch_min_val[edma3Id]) && (lCh >= edma3_qdma_ch_max_val[edma3Id]))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }

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

    if (result == EDMA3_DRV_SOK)
    {
        switch (paRAMField)
        {
        case EDMA3_DRV_PARAM_FIELD_OPT:
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_OPT) = newPaRAMFieldVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_SRCADDR:
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC) = newPaRAMFieldVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_ACNT:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_ACNT_BCNT));
            paramEntryVal &= 0xFFFF0000U;
            newPaRAMFieldVal &= 0x0000FFFFU;
            paramEntryVal |= newPaRAMFieldVal;
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_ACNT_BCNT) = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_BCNT:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_ACNT_BCNT));
            paramEntryVal &= 0x0000FFFFU;
            newPaRAMFieldVal <<= 0x10U;
            paramEntryVal |= newPaRAMFieldVal;
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_ACNT_BCNT) = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_DESTADDR:
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_DST) = newPaRAMFieldVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_SRCBIDX:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX));
            paramEntryVal &= 0xFFFF0000U;
            newPaRAMFieldVal &= 0x0000FFFFU;
            paramEntryVal |= newPaRAMFieldVal;
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX) = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_DESTBIDX:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX));
            paramEntryVal &= 0x0000FFFFU;
            newPaRAMFieldVal <<= 0x10U;
            paramEntryVal |= newPaRAMFieldVal;
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX) = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_LINKADDR:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD));
            paramEntryVal &= 0xFFFF0000U;
            newPaRAMFieldVal &= 0x0000FFFFU;
            paramEntryVal |= newPaRAMFieldVal;
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD) = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_BCNTRELOAD:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD));
            paramEntryVal &= 0x0000FFFFU;
            newPaRAMFieldVal <<= 0x10U;
            paramEntryVal |= newPaRAMFieldVal;
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD) = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_SRCCIDX:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX));
            paramEntryVal &= 0xFFFF0000U;
            newPaRAMFieldVal &= 0x0000FFFFU;
            paramEntryVal |= newPaRAMFieldVal;
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX) = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_DESTCIDX:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX));
            paramEntryVal &= 0x0000FFFFU;
            newPaRAMFieldVal <<= 0x10U;
            paramEntryVal |= newPaRAMFieldVal;
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX) = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_CCNT:
            newPaRAMFieldVal &= 0x0000FFFFU;
            *((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_CCNT) = newPaRAMFieldVal;
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

EDMA3_DRV_Result EDMA3_DRV_getPaRAMField(EDMA3_DRV_Handle hEdma,
                                         uint32_t lCh,
                                         EDMA3_DRV_PaRAMField paRAMField,
                                         uint32_t *currPaRAMFieldVal)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    uint32_t paramEntryVal = 0;
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
    if ((hEdma == NULL) || (currPaRAMFieldVal == NULL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }

    if (((int32_t)paRAMField < (int32_t)EDMA3_DRV_PARAM_FIELD_OPT) || (paRAMField > EDMA3_DRV_PARAM_FIELD_CCNT))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_DRV_SOK)
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (result == EDMA3_DRV_SOK)
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

    if (result == EDMA3_DRV_SOK)
    {
        switch (paRAMField)
        {
        case EDMA3_DRV_PARAM_FIELD_OPT:
            *currPaRAMFieldVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_OPT));
            break;

        case EDMA3_DRV_PARAM_FIELD_SRCADDR:
            *currPaRAMFieldVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC));
            break;

        case EDMA3_DRV_PARAM_FIELD_ACNT:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_ACNT_BCNT));
            paramEntryVal &= 0x0000FFFFU;
            *currPaRAMFieldVal = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_BCNT:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_ACNT_BCNT));
            paramEntryVal = paramEntryVal >> 0x10U;
            *currPaRAMFieldVal = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_DESTADDR:
            *currPaRAMFieldVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_DST));
            break;

        case EDMA3_DRV_PARAM_FIELD_SRCBIDX:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX));
            paramEntryVal &= 0x0000FFFFU;
            *currPaRAMFieldVal = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_DESTBIDX:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX));
            paramEntryVal = paramEntryVal >> 0x10U;
            *currPaRAMFieldVal = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_LINKADDR:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD));
            paramEntryVal &= 0x0000FFFFU;
            *currPaRAMFieldVal = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_BCNTRELOAD:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD));
            paramEntryVal = paramEntryVal >> 0x10U;
            *currPaRAMFieldVal = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_SRCCIDX:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX));
            paramEntryVal &= 0x0000FFFFU;
            *currPaRAMFieldVal = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_DESTCIDX:
            paramEntryVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX));
            paramEntryVal = paramEntryVal >> 0x10U;
            *currPaRAMFieldVal = paramEntryVal;
            break;

        case EDMA3_DRV_PARAM_FIELD_CCNT:
            *currPaRAMFieldVal = (uint32_t)(*((&globalRegs->PARAMENTRY[paRAMId].OPT) + EDMA3_DRV_PARAM_ENTRY_CCNT));
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

EDMA3_DRV_Result EDMA3_DRV_setEvtQPriority(EDMA3_DRV_Handle hEdma,
                                           const EDMA3_DRV_EvtQuePriority *evtQPriObj)
{
    uint32_t intState;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    uint32_t evtQNum = 0;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if ((hEdma == NULL) || (evtQPriObj == NULL))
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;

            /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
            /* check event queue priority first*/
            while (evtQNum < drvObject->gblCfgParams.numEvtQueue)
            {
                if (evtQPriObj->evtQPri[evtQNum] > EDMA3_DRV_QPRIORITY_MAX_VAL)
                {
                    result = EDMA3_DRV_E_INVALID_PARAM;
                    break;
                }
                evtQNum++;
            }
#endif

            if (result == EDMA3_DRV_SOK)
            {
                edma3OsProtectEntry(drvObject->phyCtrllerInstId,
                                    (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                    &intState);

                /* Set TC Priority among system-wide bus-masters and Queue Watermark Level */
                evtQNum = 0;
                while (evtQNum < drvObject->gblCfgParams.numEvtQueue)
                {
                    globalRegs->QUEPRI = globalRegs->QUEPRI & EDMA3_RM_QUEPRI_CLR_MASK(evtQNum);
                    globalRegs->QUEPRI |= EDMA3_RM_QUEPRI_SET_MASK(evtQNum, evtQPriObj->evtQPri[evtQNum]);

                    evtQNum++;
                }

                edma3OsProtectExit(drvObject->phyCtrllerInstId,
                                   (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                   intState);
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

EDMA3_DRV_Result EDMA3_DRV_mapChToEvtQ(EDMA3_DRV_Handle hEdma,
                                       uint32_t channelId,
                                       EDMA3_RM_EventQueue eventQ)
{
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    uint32_t intState;
    uint32_t edma3Id;
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;

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

    if (result == EDMA3_DRV_SOK)
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (channelId > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        /* Check the event queue */
        if (eventQ >= drvObject->gblCfgParams.numEvtQueue)
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }
    if (result == EDMA3_DRV_SOK)
    {
        if (channelId <= edma3_dma_ch_max_val[edma3Id])
        {
            /* DMA channel */
            edma3OsProtectEntry(edma3Id,
                                (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                &intState);

            globalRegs->DMAQNUM[channelId >> 3U] &=
                EDMA3_DRV_DMAQNUM_CLR_MASK(channelId);
            globalRegs->DMAQNUM[channelId >> 3U] |=
                EDMA3_DRV_DMAQNUM_SET_MASK(channelId, eventQ);

            edma3OsProtectExit(edma3Id,
                               (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                               intState);
        }
        else
        {
            if ((channelId >= edma3_qdma_ch_min_val[edma3Id]) && (channelId <= edma3_qdma_ch_max_val[edma3Id]))
            {
                /* QDMA channel */
                edma3OsProtectEntry(edma3Id,
                                    (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                    &intState);

                globalRegs->QDMAQNUM &=
                    EDMA3_DRV_QDMAQNUM_CLR_MASK(channelId - edma3_qdma_ch_min_val[edma3Id]);
                globalRegs->QDMAQNUM |=
                    EDMA3_DRV_QDMAQNUM_SET_MASK(channelId - edma3_qdma_ch_min_val[edma3Id], eventQ);

                edma3OsProtectExit(edma3Id,
                                   (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                   intState);
            }
            else
            {
                /* API valid for DMA/QDMA channel only, return error... */
                result = EDMA3_DRV_E_INVALID_PARAM;
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

EDMA3_DRV_Result EDMA3_DRV_getMapChToEvtQ(EDMA3_DRV_Handle hEdma,
                                          uint32_t channelId,
                                          uint32_t *mappedEvtQ)
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
    if ((hEdma == NULL) || (mappedEvtQ == NULL))
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
    }

    if (EDMA3_DRV_SOK == result)
    {
        globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        edma3Id = drvObject->phyCtrllerInstId;

        if (channelId <= edma3_dma_ch_max_val[edma3Id])
        {
            *mappedEvtQ = ((globalRegs->DMAQNUM[channelId >> 3U]) & (~(EDMA3_DRV_DMAQNUM_CLR_MASK(channelId)))) >> ((channelId % 8U) * 4U);
        }
        else
        {
            if ((channelId >= edma3_qdma_ch_min_val[edma3Id]) && (channelId <= edma3_qdma_ch_max_val[edma3Id]))
            {
                *mappedEvtQ = ((globalRegs->QDMAQNUM) & (~(EDMA3_DRV_QDMAQNUM_CLR_MASK(channelId - edma3_qdma_ch_min_val[edma3Id])))) >> (channelId * 4U);
            }
            else
            {
                /* Only valid for DMA/QDMA channel, return error... */
                result = EDMA3_DRV_E_INVALID_PARAM;
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

EDMA3_DRV_Result EDMA3_DRV_setCCRegister(EDMA3_DRV_Handle hEdma,
                                         uint32_t regOffset,
                                         uint32_t newRegValue)
{
    uint32_t intState;
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    volatile uint32_t regPhyAddr = 0x0U;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if ((hEdma == NULL) || ((regOffset % 4U) != 0))
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
            if (drvObject->gblCfgParams.globalRegs != NULL)
            {
                /**
                  * Take the instance specific semaphore, to prevent simultaneous
                  * access to the shared resources.
                  */
                result = edma3OsSemTake(drvInst->drvSemHandle,
                                        EDMA3_OSSEM_NO_TIMEOUT);

                if (EDMA3_DRV_SOK == result)
                {
                    /* Semaphore taken successfully, modify the registers. */
                    edma3OsProtectEntry(drvObject->phyCtrllerInstId,
                                        (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                        &intState);
                    /* Global interrupts disabled, modify the registers. */
                    regPhyAddr = (uint32_t)drvObject->gblCfgParams.globalRegs + regOffset;

                    *(uint32_t *)regPhyAddr = newRegValue;

                    edma3OsProtectExit(drvObject->phyCtrllerInstId,
                                       (int32_t)EDMA3_OS_PROTECT_INTERRUPT,
                                       intState);
                    /* Return the semaphore back */
                    result = edma3OsSemGive(drvInst->drvSemHandle);
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

EDMA3_DRV_Result EDMA3_DRV_getCCRegister(EDMA3_DRV_Handle hEdma,
                                         uint32_t regOffset,
                                         uint32_t *regValue)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    volatile uint32_t regPhyAddr = 0x0U;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (((hEdma == NULL) || (regValue == NULL)) || ((regOffset % 4U) != 0))
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
            if (drvObject->gblCfgParams.globalRegs != NULL)
            {
                regPhyAddr = (uint32_t)drvObject->gblCfgParams.globalRegs + regOffset;

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

EDMA3_DRV_Result EDMA3_DRV_waitAndClearTcc(EDMA3_DRV_Handle hEdma,
                                           uint32_t tccNo)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
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
            /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
            if (tccNo >= drvObject->gblCfgParams.numTccs)
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
#endif

            /* Check if the parameters are OK. */
            if (EDMA3_DRV_SOK == result)
            {
                globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
                shadowRegs = (volatile EDMA3_CCRL_ShadowRegs *)&globalRegs->SHADOW[drvInst->regionId];

                if (shadowRegs != NULL)
                {
                    if (tccNo < 32U)
                    {
                        tccBitMask = 1U << tccNo;

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
                        tccBitMask = 1U << (tccNo - 32U);

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

EDMA3_DRV_Result EDMA3_DRV_checkAndClearTcc(EDMA3_DRV_Handle hEdma,
                                            uint32_t tccNo,
                                            uint16_t *tccStatus)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
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
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if ((hEdma == NULL) || (tccStatus == NULL))
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
            /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
            if (tccNo >= drvObject->gblCfgParams.numTccs)
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
#endif

            /* Check if the parameters are OK. */
            if (EDMA3_DRV_SOK == result)
            {
                globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
                shadowRegs = (volatile EDMA3_CCRL_ShadowRegs *)&globalRegs->SHADOW[drvInst->regionId];

                /* Reset the tccStatus */
                *tccStatus = FALSE;

                if (shadowRegs != NULL)
                {
                    if (tccNo < 32U)
                    {
                        tccBitMask = 1U << tccNo;

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
                        tccBitMask = 1U << (tccNo - 32U);

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

EDMA3_DRV_Result EDMA3_DRV_getPaRAMPhyAddr(EDMA3_DRV_Handle hEdma,
                                           uint32_t lCh,
                                           uint32_t *paramPhyAddr)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
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
    if ((hEdma == NULL) || (paramPhyAddr == NULL))
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
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
        }
    }

    if (result == EDMA3_DRV_SOK)
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
            *paramPhyAddr = (uint32_t) & (globalRegs->PARAMENTRY[paRAMId].OPT);
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

EDMA3_DRV_Result EDMA3_DRV_Ioctl(
    EDMA3_DRV_Handle hEdma,
    EDMA3_DRV_IoctlCmd cmd,
    void *cmdArg,
    void *param)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;

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

    if ((cmd <= EDMA3_DRV_IOCTL_MIN_IOCTL) || (cmd >= EDMA3_DRV_IOCTL_MAX_IOCTL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_DRV_SOK)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;

        if (drvInst == NULL)
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        switch (cmd)
        {
        case EDMA3_DRV_IOCTL_SET_PARAM_CLEAR_OPTION:
        {
            result = EDMA3_RM_Ioctl(drvInst->resMgrInstance,
                                    EDMA3_RM_IOCTL_SET_PARAM_CLEAR_OPTION,
                                    cmdArg,
                                    param);

            break;
        }

        case EDMA3_DRV_IOCTL_GET_PARAM_CLEAR_OPTION:
        {
            if (NULL == cmdArg)
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
            else
            {
                result = EDMA3_RM_Ioctl(drvInst->resMgrInstance,
                                        EDMA3_RM_IOCTL_GET_PARAM_CLEAR_OPTION,
                                        cmdArg,
                                        param);
            }

            break;
        }

        default:
            /* You passed invalid IOCTL cmd */
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

EDMA3_DRV_Handle EDMA3_DRV_getInstHandle(uint32_t phyCtrllerInstId,
                                         EDMA3_RM_RegionId regionId,
                                         EDMA3_DRV_Result *errorCode)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Object *drvObject = NULL;
    EDMA3_DRV_Instance *drvInstanceHandle = NULL;

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if ((phyCtrllerInstId >= EDMA3_MAX_EDMA3_INSTANCES) || (errorCode == NULL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (EDMA3_DRV_SOK == result)
    {
        drvObject = &drvObj[phyCtrllerInstId];

        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        /* Check regionId. */
        if (regionId >= drvObject->gblCfgParams.numRegions)
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* If the driver instance is already opened for this specific region,
        *  return it, else return an error.
        */
        drvInstanceHandle = &drvInstance[phyCtrllerInstId][regionId];

        if (NULL == drvInstanceHandle->pDrvObjectHandle)
        {
            /* Instance not opened yet!!! */
            drvInstanceHandle = NULL;
            result = EDMA3_DRV_E_INST_NOT_OPENED;
        }
    }
    if (errorCode != NULL)
    {
        *errorCode = result;
    }

    return (EDMA3_DRV_Handle)drvInstanceHandle;
}

EDMA3_DRV_Result EDMA3_DRV_registerTccCb(EDMA3_DRV_Handle hEdma,
                                         const uint32_t channelId,
                                         EDMA3_RM_TccCallback tccCb,
                                         void *cbData)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    uint32_t edma3Id;
    EDMA3_RM_ResDesc channelObj;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if ((hEdma == NULL) || (tccCb == NULL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    if (result == EDMA3_DRV_SOK)
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
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        if (channelId <= edma3_dma_ch_max_val[edma3Id])
        {
            channelObj.type = EDMA3_RM_RES_DMA_CHANNEL;
            channelObj.resId = channelId;
        }
        else
        {
            if ((channelId >= edma3_qdma_ch_min_val[edma3Id]) || (channelId <= edma3_qdma_ch_max_val[edma3Id]))
            {
                channelObj.type = EDMA3_RM_RES_QDMA_CHANNEL;
                channelObj.resId = channelId - edma3_qdma_ch_min_val[edma3Id];
            }
            else
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        /* Call the RM function now */
        result = EDMA3_RM_registerTccCb(drvInst->resMgrInstance,
                                        (EDMA3_RM_ResDesc *)&channelObj,
                                        edma3DrvChBoundRes[edma3Id][channelId].tcc,
                                        tccCb,
                                        cbData);
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

EDMA3_DRV_Result EDMA3_DRV_unregisterTccCb(EDMA3_DRV_Handle hEdma,
                                           const uint32_t channelId)

{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    uint32_t edma3Id;
    EDMA3_RM_ResDesc channelObj;

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

    if (result == EDMA3_DRV_SOK)
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
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        if (channelId <= edma3_dma_ch_max_val[edma3Id])
        {
            channelObj.type = EDMA3_RM_RES_DMA_CHANNEL;
            channelObj.resId = channelId;
        }
        else
        {
            if ((channelId >= edma3_qdma_ch_min_val[edma3Id]) || (channelId <= edma3_qdma_ch_max_val[edma3Id]))
            {
                channelObj.type = EDMA3_RM_RES_QDMA_CHANNEL;
                channelObj.resId = channelId - edma3_qdma_ch_min_val[edma3Id];
            }
            else
            {
                result = EDMA3_DRV_E_INVALID_PARAM;
            }
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        /* Call the RM function now */
        result = EDMA3_RM_unregisterTccCb(drvInst->resMgrInstance,
                                          (EDMA3_RM_ResDesc *)&channelObj);
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

EDMA3_DRV_Result EDMA3_DRV_setTcErrorInt(uint32_t phyCtrllerInstId,
                                         uint32_t tcId,
                                         EDMA3_DRV_Tc_Err tcErr)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Object *drvObject = NULL;
    volatile EDMA3_TCRL_Regs *tcRegs = NULL;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if (phyCtrllerInstId >= EDMA3_MAX_EDMA3_INSTANCES)
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }

    if (((int32_t)tcErr < (int32_t)EDMA3_DRV_TC_ERR_BUSERR_DIS) || (tcErr > EDMA3_DRV_TC_ERR_EN))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvObject = &drvObj[phyCtrllerInstId];

        if (tcId >= drvObject->gblCfgParams.numTcs)
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        tcRegs = (volatile EDMA3_TCRL_Regs *)drvObject->gblCfgParams.tcRegs[tcId];

        if (NULL == tcRegs)
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        switch (tcErr)
        {
        case EDMA3_DRV_TC_ERR_BUSERR_DIS:
            tcRegs->ERREN &= ~((uint32_t)1 << EDMA3_TCRL_ERREN_BUSERR_SHIFT);
            break;

        case EDMA3_DRV_TC_ERR_BUSERR_EN:
            tcRegs->ERREN |= ((uint32_t)1 << EDMA3_TCRL_ERREN_BUSERR_SHIFT);
            break;

        case EDMA3_DRV_TC_ERR_TRERR_DIS:
            tcRegs->ERREN &= ~((uint32_t)1 << EDMA3_TCRL_ERREN_TRERR_SHIFT);
            break;

        case EDMA3_DRV_TC_ERR_TRERR_EN:
            tcRegs->ERREN |= ((uint32_t)1 << EDMA3_TCRL_ERREN_TRERR_SHIFT);
            break;

        case EDMA3_DRV_TC_ERR_MMRAERR_DIS:
            tcRegs->ERREN &= ~((uint32_t)1 << EDMA3_TCRL_ERREN_MMRAERR_SHIFT);
            break;

        case EDMA3_DRV_TC_ERR_MMRAERR_EN:
            tcRegs->ERREN |= ((uint32_t)1 << EDMA3_TCRL_ERREN_MMRAERR_SHIFT);
            break;

        case EDMA3_DRV_TC_ERR_DIS:
            tcRegs->ERREN = EDMA3_TCRL_ERREN_RESETVAL;
            break;

        case EDMA3_DRV_TC_ERR_EN:
            tcRegs->ERREN = EDMA3_TCRL_ERREN_BUSERR_MASK | EDMA3_TCRL_ERREN_TRERR_MASK | EDMA3_TCRL_ERREN_MMRAERR_MASK;
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

EDMA3_DRV_Result EDMA3_DRV_getChannelStatus(EDMA3_DRV_Handle hEdma,
                                            uint32_t lCh, uint32_t *lchStatus)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    volatile EDMA3_CCRL_ShadowRegs *shadowRegs = NULL;
    uint32_t edma3Id;
    uint32_t status = 0x0;
    uint32_t tcc;

#ifdef EDMA3_INSTRUMENTATION_ENABLED
    EDMA3_LOG_EVENT(&DVTEvent_Log, "EDMA3",
                    EDMA3_DVT_DESC(EDMA3_DVT_eFUNC_START,
                                   EDMA3_DVT_dCOUNTER,
                                   EDMA3_DVT_dNONE,
                                   EDMA3_DVT_dNONE));
#endif /* EDMA3_INSTRUMENTATION_ENABLED */

    /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
    if ((hEdma == NULL) || (lchStatus == NULL))
    {
        result = EDMA3_DRV_E_INVALID_PARAM;
    }
#endif

    /* Check if the parameters are OK. */
    if (EDMA3_DRV_SOK == result)
    {
        drvInst = (EDMA3_DRV_Instance *)hEdma;
        drvObject = drvInst->pDrvObjectHandle;

        if ((drvObject == NULL) ||
            (drvObject->gblCfgParams.globalRegs == NULL))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;
            shadowRegs = (volatile EDMA3_CCRL_ShadowRegs *)&globalRegs->SHADOW[drvInst->regionId];
            edma3Id = drvObject->phyCtrllerInstId;
        }
    }

    if (result == EDMA3_DRV_SOK)
    {
        /* If parameter checking is enabled... */
#ifndef EDMA3_DRV_PARAM_CHECK_DISABLE
        if (lCh > edma3_log_ch_max_val[edma3Id])
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }

        /* Not valid for Link channels */
        if ((lCh >= edma3_link_ch_min_val[edma3Id]) && (lCh <= edma3_link_ch_max_val[edma3Id]))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
#endif
    }

    if (EDMA3_DRV_SOK == result)
    {
        tcc = edma3DrvChBoundRes[edma3Id][lCh].tcc;

        if (tcc >= drvObject->gblCfgParams.numTccs)
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        /* DMA Channel, check for event pending and event miss */
        if (lCh <= edma3_dma_ch_max_val[edma3Id])
        {
            if (lCh < 32U)
            {
                if ((globalRegs->EMR & ((uint32_t)1U << lCh)) != FALSE)
                {
                    status |= EDMA3_DRV_CHANNEL_ERR;
                }

                if ((shadowRegs->ER & ((uint32_t)1U << lCh)) != FALSE)
                {
                    status |= EDMA3_DRV_CHANNEL_EVENT_PENDING;
                }
            }
            else
            {
                if ((globalRegs->EMRH & ((uint32_t)1U << (lCh - 32U))) != FALSE)
                {
                    status |= EDMA3_DRV_CHANNEL_ERR;
                }
                if ((shadowRegs->ERH & ((uint32_t)1U << (lCh - 32U))) != FALSE)
                {
                    status |= EDMA3_DRV_CHANNEL_EVENT_PENDING;
                }
            }
        }

        /* QDMA Channel, check for event miss */
        if ((lCh >= edma3_qdma_ch_min_val[edma3Id]) && (lCh <= edma3_qdma_ch_max_val[edma3Id]))
        {
            uint32_t qdma_ch = lCh - edma3_qdma_ch_min_val[edma3Id];

            if ((globalRegs->QEMR & ((uint32_t)1U << qdma_ch)) != FALSE)
            {
                status |= EDMA3_DRV_CHANNEL_ERR;
            }
        }

        /* Check for xfer completion interrupt */
        if (tcc < 32U)
        {
            if ((shadowRegs->IPR & ((uint32_t)1U << tcc)) != FALSE)
            {
                status |= EDMA3_DRV_CHANNEL_XFER_COMPLETE;
            }
        }
        else
        {
            if ((shadowRegs->IPRH & ((uint32_t)1U << (tcc - 32U))) != FALSE)
            {
                status |= EDMA3_DRV_CHANNEL_XFER_COMPLETE;
            }
        }

        /* Update the status */
        *lchStatus = status;
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

EDMA3_DRV_Result EDMA3_DRV_mapTccLinkCh(EDMA3_DRV_Handle hEdma,
                                        uint32_t linkCh,
                                        uint32_t tcc)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_Instance *drvInst = NULL;
    EDMA3_DRV_Object *drvObject = NULL;
    int32_t paRAMId;
    volatile EDMA3_CCRL_Regs *globalRegs = NULL;
    uint32_t edma3Id;

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
    }

    if (EDMA3_DRV_SOK == result)
    {
        edma3Id = drvObject->phyCtrllerInstId;
        globalRegs = (volatile EDMA3_CCRL_Regs *)drvObject->gblCfgParams.globalRegs;

        /* Validate the arguments */
        if (((linkCh < edma3_link_ch_min_val[edma3Id]) ||
             (linkCh > edma3_link_ch_max_val[edma3Id])) ||
            (tcc >= drvObject->gblCfgParams.numTccs))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
    }

    if (EDMA3_DRV_SOK == result)
    {
        paRAMId = edma3DrvChBoundRes[edma3Id][linkCh].paRAMId;

        if ((paRAMId < 0) || (paRAMId >= drvObject->gblCfgParams.numPaRAMSets))
        {
            result = EDMA3_DRV_E_INVALID_PARAM;
        }
        else
        {
            /* Set TCC in ParamSet.OPT field */
            globalRegs->PARAMENTRY[paRAMId].OPT &= EDMA3_DRV_OPT_TCC_CLR_MASK;
            globalRegs->PARAMENTRY[paRAMId].OPT |= EDMA3_DRV_OPT_TCC_SET_MASK(tcc);

            /* Update the tcc */
            edma3DrvChBoundRes[edma3Id][linkCh].tcc = tcc;
        }
    }

    return result;
}

/* End of File */
