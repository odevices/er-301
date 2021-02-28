/*
 * edma3.h
 *
 * EDMA3 Driver Internal header file.
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

#ifndef EDMA3_H_
#define EDMA3_H_

/** Include EDMA3 Driver header file */
#include <ti/edma3/edma3_drv.h>

/* For the EDMA3 CC Register Layer functionality */
#include <ti/edma3/rm/edma3_rl_cc.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Mask defines */
/** Parameter RAM Set field OPT bit-field defines */
/** OPT-SAM bit Clear */
#define EDMA3_DRV_OPT_SAM_CLR_MASK (~EDMA3_CCRL_OPT_SAM_MASK)
/** OPT-SAM bit Set */
#define EDMA3_DRV_OPT_SAM_SET_MASK(mode) ((((uint32_t)(EDMA3_CCRL_OPT_SAM_MASK >> EDMA3_CCRL_OPT_SAM_SHIFT)) & (mode)) << EDMA3_CCRL_OPT_SAM_SHIFT)

/** OPT-DAM bit Clear */
#define EDMA3_DRV_OPT_DAM_CLR_MASK (~EDMA3_CCRL_OPT_DAM_MASK)
/** OPT-DAM bit Set */
#define EDMA3_DRV_OPT_DAM_SET_MASK(mode) ((((uint32_t)(EDMA3_CCRL_OPT_DAM_MASK >> EDMA3_CCRL_OPT_DAM_SHIFT)) & (mode)) << EDMA3_CCRL_OPT_DAM_SHIFT)

/** OPT-SYNCDIM bit Clear */
#define EDMA3_DRV_OPT_SYNCDIM_CLR_MASK (~EDMA3_CCRL_OPT_SYNCDIM_MASK)
/** OPT-SYNCDIM bit Set */
#define EDMA3_DRV_OPT_SYNCDIM_SET_MASK(synctype) ((((uint32_t)(EDMA3_CCRL_OPT_SYNCDIM_MASK >> EDMA3_CCRL_OPT_SYNCDIM_SHIFT)) & (synctype)) << EDMA3_CCRL_OPT_SYNCDIM_SHIFT)

/** OPT-STATIC bit Clear */
#define EDMA3_DRV_OPT_STATIC_CLR_MASK (~EDMA3_CCRL_OPT_STATIC_MASK)
/** OPT-STATIC bit Set */
#define EDMA3_DRV_OPT_STATIC_SET_MASK(en) (((EDMA3_CCRL_OPT_STATIC_MASK >> EDMA3_CCRL_OPT_STATIC_SHIFT) & (en)) << EDMA3_CCRL_OPT_STATIC_SHIFT)

/** OPT-FWID bitfield Clear */
#define EDMA3_DRV_OPT_FWID_CLR_MASK (~EDMA3_CCRL_OPT_FWID_MASK)
/** OPT-FWID bitfield Set */
#define EDMA3_DRV_OPT_FWID_SET_MASK(width) ((((uint32_t)(EDMA3_CCRL_OPT_FWID_MASK >> EDMA3_CCRL_OPT_FWID_SHIFT)) & (width)) << EDMA3_CCRL_OPT_FWID_SHIFT)

/** OPT-TCCMODE bit Clear */
#define EDMA3_DRV_OPT_TCCMODE_CLR_MASK (~EDMA3_CCRL_OPT_TCCMODE_MASK)
/** OPT-TCCMODE bit Set */
#define EDMA3_DRV_OPT_TCCMODE_SET_MASK(early) (((EDMA3_CCRL_OPT_TCCMODE_MASK >> EDMA3_CCRL_OPT_TCCMODE_SHIFT) & (early)) << EDMA3_CCRL_OPT_TCCMODE_SHIFT)

/** OPT-TCC bitfield Clear */
#define EDMA3_DRV_OPT_TCC_CLR_MASK (~EDMA3_CCRL_OPT_TCC_MASK)
/** OPT-TCC bitfield Set */
#define EDMA3_DRV_OPT_TCC_SET_MASK(tcc) (((EDMA3_CCRL_OPT_TCC_MASK >> EDMA3_CCRL_OPT_TCC_SHIFT) & (tcc)) << EDMA3_CCRL_OPT_TCC_SHIFT)

/** OPT-TCINTEN bit Clear */
#define EDMA3_DRV_OPT_TCINTEN_CLR_MASK (~EDMA3_CCRL_OPT_TCINTEN_MASK)
/** OPT-TCINTEN bit Set */
#define EDMA3_DRV_OPT_TCINTEN_SET_MASK(tcinten) (((EDMA3_CCRL_OPT_TCINTEN_MASK >> EDMA3_CCRL_OPT_TCINTEN_SHIFT) & (tcinten)) << EDMA3_CCRL_OPT_TCINTEN_SHIFT)

/** OPT-ITCINTEN bit Clear */
#define EDMA3_DRV_OPT_ITCINTEN_CLR_MASK (~EDMA3_CCRL_OPT_ITCINTEN_MASK)
/** OPT-ITCINTEN bit Set */
#define EDMA3_DRV_OPT_ITCINTEN_SET_MASK(itcinten) (((EDMA3_CCRL_OPT_ITCINTEN_MASK >> EDMA3_CCRL_OPT_ITCINTEN_SHIFT) & (itcinten)) << EDMA3_CCRL_OPT_ITCINTEN_SHIFT)

/** OPT-TCCHEN bit Clear */
#define EDMA3_DRV_OPT_TCCHEN_CLR_MASK (~EDMA3_CCRL_OPT_TCCHEN_MASK)
/** OPT-TCCHEN bit Set */
#define EDMA3_DRV_OPT_TCCHEN_SET_MASK(tcchen) (((EDMA3_CCRL_OPT_TCCHEN_MASK >> EDMA3_CCRL_OPT_TCCHEN_SHIFT) & (tcchen)) << EDMA3_CCRL_OPT_TCCHEN_SHIFT)

/** OPT-ITCCHEN bit Clear */
#define EDMA3_DRV_OPT_ITCCHEN_CLR_MASK (~EDMA3_CCRL_OPT_ITCCHEN_MASK)
/** OPT-ITCCHEN bit Set */
#define EDMA3_DRV_OPT_ITCCHEN_SET_MASK(itcchen) (((EDMA3_CCRL_OPT_ITCCHEN_MASK >> EDMA3_CCRL_OPT_ITCCHEN_SHIFT) & (itcchen)) << EDMA3_CCRL_OPT_ITCCHEN_SHIFT)

/** OPT-SAM bit Get */
#define EDMA3_DRV_OPT_SAM_GET_MASK(mode) ((mode)&1U)
/** OPT-DAM bit Get */
#define EDMA3_DRV_OPT_DAM_GET_MASK(mode) (((mode) & ((uint32_t)1U << 1U)) >> 1U)
/** OPT-SYNCDIM bit Get */
#define EDMA3_DRV_OPT_SYNCDIM_GET_MASK(synctype) (((synctype) & ((uint32_t)1U << 2U)) >> 2U)
/** OPT-STATIC bit Get */
#define EDMA3_DRV_OPT_STATIC_GET_MASK(en) (((en) & ((uint32_t)1U << 3U)) >> 3U)
/** OPT-FWID bitfield Get */
#define EDMA3_DRV_OPT_FWID_GET_MASK(width) (((width) & ((uint32_t)0x7U << 8U)) >> 8U)
/** OPT-TCCMODE bit Get */
#define EDMA3_DRV_OPT_TCCMODE_GET_MASK(early) (((early) & ((uint32_t)1U << 11U)) >> 11U)
/** OPT-TCC bitfield Get */
#define EDMA3_DRV_OPT_TCC_GET_MASK(tcc) (((tcc) & ((uint32_t)0x3fU << 12U)) >> 12U)
/** OPT-TCINTEN bit Get */
#define EDMA3_DRV_OPT_TCINTEN_GET_MASK(tcinten) (((tcinten) & ((uint32_t)1U << 20U)) >> 20U)
/** OPT-ITCINTEN bit Get */
#define EDMA3_DRV_OPT_ITCINTEN_GET_MASK(itcinten) (((itcinten) & ((uint32_t)1U << 21U)) >> 21U)
/** OPT-TCCHEN bit Get */
#define EDMA3_DRV_OPT_TCCHEN_GET_MASK(tcchen) (((tcchen) & ((uint32_t)1U << 22U)) >> 22U)
/** OPT-ITCCHEN bit Get */
#define EDMA3_DRV_OPT_ITCCHEN_GET_MASK(itcchen) (((itcchen) & ((uint32_t)1U << 23U)) >> 23U)

/** DMAQNUM bits Clear */
#define EDMA3_DRV_DMAQNUM_CLR_MASK(chNum) (~((uint32_t)0x7U << (((chNum) % 8U) * 4U)))
/** DMAQNUM bits Set */
#define EDMA3_DRV_DMAQNUM_SET_MASK(chNum, queNum) ((0x7U & (queNum)) << (((chNum) % 8U) * 4U))
/** QDMAQNUM bits Clear */
#define EDMA3_DRV_QDMAQNUM_CLR_MASK(chNum) (~((uint32_t)0x7U << ((chNum)*4U)))
/** QDMAQNUM bits Set */
#define EDMA3_DRV_QDMAQNUM_SET_MASK(chNum, queNum) ((0x7U & (queNum)) << ((chNum)*4U))

/* Other Mask defines */
/** DCHMAP-PaRAMEntry bitfield Clear */
#define EDMA3_DRV_DCH_PARAM_CLR_MASK (~EDMA3_CCRL_DCHMAP_PAENTRY_MASK)
/** DCHMAP-PaRAMEntry bitfield Set */
#define EDMA3_DRV_DCH_PARAM_SET_MASK(paRAMId) ((((uint32_t)(EDMA3_CCRL_DCHMAP_PAENTRY_MASK >> EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT)) & (paRAMId)) << EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT)

/** QCHMAP-PaRAMEntry bitfield Clear */
#define EDMA3_DRV_QCH_PARAM_CLR_MASK (~EDMA3_CCRL_QCHMAP_PAENTRY_MASK)
/** QCHMAP-PaRAMEntry bitfield Set */
#define EDMA3_DRV_QCH_PARAM_SET_MASK(paRAMId) ((((uint32_t)(EDMA3_CCRL_QCHMAP_PAENTRY_MASK >> EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT)) & (paRAMId)) << EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT)
/** QCHMAP-TrigWord bitfield Clear */
#define EDMA3_DRV_QCH_TRWORD_CLR_MASK (~EDMA3_CCRL_QCHMAP_TRWORD_MASK)
/** QCHMAP-TrigWord bitfield Set */
#define EDMA3_DRV_QCH_TRWORD_SET_MASK(trWord) (((EDMA3_CCRL_QCHMAP_TRWORD_MASK >> EDMA3_CCRL_QCHMAP_TRWORD_SHIFT) & (trWord)) << EDMA3_CCRL_QCHMAP_TRWORD_SHIFT)
/** QCHMAP-PaRAMEntry & TrigWord bitfield Clear */
#define EDMA3_DRV_QCH_PARAM_TRWORD_CLR_MASK (EDMA3_DRV_QCH_PARAM_CLR_MASK | EDMA3_DRV_QCH_TRWORD_CLR_MASK)

/** Max value of ACnt */
#define EDMA3_DRV_ACNT_MAX_VAL (0xFFFFU)
/** Max value of BCnt */
#define EDMA3_DRV_BCNT_MAX_VAL (0xFFFFU)
/** Max value of CCnt */
#define EDMA3_DRV_CCNT_MAX_VAL (0xFFFFU)
/** Max value of BCntReld */
#define EDMA3_DRV_BCNTRELD_MAX_VAL (0xFFFFU)
/** Max value of SrcBIdx */
#define EDMA3_DRV_SRCBIDX_MAX_VAL (0x7FFF)
/** Min value of SrcBIdx */
#define EDMA3_DRV_SRCBIDX_MIN_VAL (-32768)
/** Max value of SrcCIdx */
#define EDMA3_DRV_SRCCIDX_MAX_VAL (0x7FFF)
/** Min value of SrcCIdx */
#define EDMA3_DRV_SRCCIDX_MIN_VAL (-32768)
/** Max value of DestBIdx */
#define EDMA3_DRV_DSTBIDX_MAX_VAL (0x7FFF)
/** Min value of DestBIdx */
#define EDMA3_DRV_DSTBIDX_MIN_VAL (-32768)
/** Max value of DestCIdx */
#define EDMA3_DRV_DSTCIDX_MAX_VAL (0x7FFF)
/** Min value of DestCIdx */
#define EDMA3_DRV_DSTCIDX_MIN_VAL (-32768)
/** Max value of Queue Priority */
#define EDMA3_DRV_QPRIORITY_MAX_VAL (7U)
/** Min value of Queue Priority */
#define EDMA3_DRV_QPRIORITY_MIN_VAL (0U)

    /** To maintain the state of the EDMA3 Driver object */
    typedef enum
    {
        /** Object deleted */
        EDMA3_DRV_DELETED = 0,
        /** Obect Created */
        EDMA3_DRV_CREATED = 1,
        /** Object Opened */
        EDMA3_DRV_OPENED = 2,
        /** Object Closed */
        EDMA3_DRV_CLOSED = 3
    } EDMA3_DRV_ObjState;

    /**
 * \brief EDMA3 Driver Object (HW Specific) Maintenance structure.
 *
 * Used to maintain information of the EDMA3 HW configuration
 * thoughout the lifetime of the EDMA3 Driver Object,
 * one for each EDMA3 hardware instance.
 *
 */
    typedef struct
    {
        /** Physical Instance ID of EDMA3 Controller */
        uint32_t phyCtrllerInstId;

        /** State information of the EDMA3 Driver object */
        EDMA3_DRV_ObjState state;

        /** Number of EDMA3 Driver instances */
        uint32_t numOpens;

        /**
     * \brief Init-time Configuration structure for EDMA3
     * controller, to provide Global SoC specific Information.
     *
     * This configuration info can be provided by the user at run-time,
     * while calling EDMA3_DRV_create(). If not provided at run-time,
     * this info will be taken from the config file edma3Cfg.c.
     */
        EDMA3_DRV_GblConfigParams gblCfgParams;

    } EDMA3_DRV_Object;

    /**
 * \brief EDMA3 Driver Instance Configuration Structure.
 *
 * Used to maintain information of the EDMA3 Driver Instances.
 * One such storage exists for each instance of the EDMA3 Driver.
 * There could be as many Driver Instances as there are shadow
 * regions. Multiple EDMA3 Driver instances on the same shadow
 * region are NOT allowed.
 */
    typedef struct
    {
        /** Region Identification */
        EDMA3_RM_RegionId regionId;

        /**
     * Whether EDMA3 driver instance is Master or not.
     * Only the master instance shadow region will receive the
     * EDMA3 interrupts, if enabled.
     */
        uint32_t isMaster;

        /**
     * EDMA3 Driver Instance (Shadow Region) specific
     * init configuration.
     * If NULL, static values will be taken
     */
        EDMA3_DRV_InstanceInitConfig drvInstInitConfig;

        /** EDMA3 Driver Instance specific semaphore handle */
        void *drvSemHandle;

        /** Instance wide Global Error callback parameters */
        EDMA3_RM_GblErrCallbackParams gblerrCbParams;

        /** Pointer to appropriate Shadow Register region of CC Registers */
        EDMA3_CCRL_ShadowRegs *shadowRegs;

        /**
     * Pointer to the EDMA3 Driver Object, for HW specific / Global
     * Information.
     */
        EDMA3_DRV_Object *pDrvObjectHandle;

        /** Pointer to the Resource Manager Instance opened by the EDMA3 Driver */
        EDMA3_RM_Handle resMgrInstance;

        /** 
     * Pointer to the user defined function for mapping cross bar events to 
     * channel. 
     */
        EDMA3_DRV_mapXbarEvtToChan mapXbarToChan;

        /** 
     * Pointer to the user defined function for configuring the cross bar 
     * events to appropriate channel in the Control Config TPCC Event Config 
     * registers. 
     */
        EDMA3_DRV_xbarConfigScr configScrMapXbarToEvt;

        /** 
     * Pointer to the configuration data structure for
     * mapping cross bar events to channel. 
     */
        EDMA3_DRV_GblXbarToChanConfigParams drvXbarToEvtMapConfig;
    } EDMA3_DRV_Instance;

    /**
 * \brief EDMA3 Channel-Bound resources.
 *
 * Used to maintain information of the EDMA3 resources
 * (specifically Parameter RAM set and TCC) and the mode of triggering
 * transfer (Manual, HW event driven etc) bound to the
 * particular channel within EDMA3_DRV_requestChannel().
 */
    typedef struct
    {
        /** PaRAM Set number associated with the particular channel */
        int32_t paRAMId;

        /** TCC associated with the particular channel */
        uint32_t tcc;

        /** Mode of triggering transfer */
        EDMA3_DRV_TrigMode trigMode;

    } EDMA3_DRV_ChBoundResources;

    /**
 * \brief EDMA3 Channel Type
 */
    typedef enum
    {
        /** Invalid Channel */
        EDMA3_DRV_CHANNEL_TYPE_NONE = 0,

        /** DMA Channel */
        EDMA3_DRV_CHANNEL_TYPE_DMA = 1,

        /** QDMA Channel */
        EDMA3_DRV_CHANNEL_TYPE_QDMA = 2,

        /** LINK Channel */
        EDMA3_DRV_CHANNEL_TYPE_LINK = 3,

        /** LINK Channel with TCC */
        EDMA3_DRV_CHANNEL_TYPE_LINK_TCC = 4
    } EDMA3_DRV_ChannelType;

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
    typedef struct
    {
        uint32_t dmaChannelQueueNum[EDMA3_MAX_DMA_CH];
        uint32_t qdmaChannelQueueNum[EDMA3_MAX_QDMA_CH];
    } EDMA3_DRV_DmaQdmaQueueNum;
#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _EDMA3_H_ */
