/*
 * edma3resmgr.h
 *
 * EDMA3 Resource Manager Internal header file.
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

#ifndef EDMA3_RES_MGR_H_
#define EDMA3_RES_MGR_H_

/** Include Resource Manager header file */
#include <ti/edma3/edma3_rm.h>

/* For the EDMA3 Register Layer functionality. */
#include <ti/edma3/rm/edma3_rl_cc.h>
#include <ti/edma3/rm/edma3_rl_tc.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * Number of PaRAM Sets actually present on the SoC. This will be updated
 * while creating the Resource Manager Object.
 */
    extern uint32_t edma3NumPaRAMSets;

/** Define for setting all bits of the EDMA3 Controller Registers */
#define EDMA3_RM_SET_ALL_BITS (0xFFFFFFFFU)

/* Other Mask defines */
/** DCHMAP-PaRAMEntry bitfield Clear */
#define EDMA3_RM_DCH_PARAM_CLR_MASK (~EDMA3_CCRL_DCHMAP_PAENTRY_MASK)
/** DCHMAP-PaRAMEntry bitfield Set */
#define EDMA3_RM_DCH_PARAM_SET_MASK(paRAMId) (((EDMA3_CCRL_DCHMAP_PAENTRY_MASK >> EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT) & (paRAMId)) << EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT)
/** QCHMAP-PaRAMEntry bitfield Clear */
#define EDMA3_RM_QCH_PARAM_CLR_MASK (~EDMA3_CCRL_QCHMAP_PAENTRY_MASK)
/** QCHMAP-PaRAMEntry bitfield Set */
#define EDMA3_RM_QCH_PARAM_SET_MASK(trWord) (((EDMA3_CCRL_QCHMAP_PAENTRY_MASK >> EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT) & (trWord)) << EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT)
/** QCHMAP-TrigWord bitfield Clear */
#define EDMA3_RM_QCH_TRWORD_CLR_MASK (~EDMA3_CCRL_QCHMAP_TRWORD_MASK)
/** QCHMAP-TrigWord bitfield Set */
#define EDMA3_RM_QCH_TRWORD_SET_MASK(paRAMId) (((EDMA3_CCRL_QCHMAP_TRWORD_MASK >> EDMA3_CCRL_QCHMAP_TRWORD_SHIFT) & (paRAMId)) << EDMA3_CCRL_QCHMAP_TRWORD_SHIFT)
/** QUEPRI bits Clear */
#define EDMA3_RM_QUEPRI_CLR_MASK(queNum) (~(EDMA3_CCRL_QUEPRI_PRIQ0_MASK << ((queNum)*EDMA3_CCRL_QUEPRI_PRIQ1_SHIFT)))
/** QUEPRI bits Set */
#define EDMA3_RM_QUEPRI_SET_MASK(queNum, quePri) ((EDMA3_CCRL_QUEPRI_PRIQ0_MASK & (quePri)) << ((queNum)*EDMA3_CCRL_QUEPRI_PRIQ1_SHIFT))
/** QUEWMTHR bits Clear */
#define EDMA3_RM_QUEWMTHR_CLR_MASK(queNum) (~(EDMA3_CCRL_QWMTHRA_Q0_MASK << ((queNum)*EDMA3_CCRL_QWMTHRA_Q1_SHIFT)))
/** QUEWMTHR bits Set */
#define EDMA3_RM_QUEWMTHR_SET_MASK(queNum, queThr) ((EDMA3_CCRL_QWMTHRA_Q0_MASK & (queThr)) << ((queNum)*EDMA3_CCRL_QWMTHRA_Q1_SHIFT))

/** OPT-TCC bitfield Clear */
#define EDMA3_RM_OPT_TCC_CLR_MASK (~EDMA3_CCRL_OPT_TCC_MASK)
/** OPT-TCC bitfield Set */
#define EDMA3_RM_OPT_TCC_SET_MASK(tcc) (((EDMA3_CCRL_OPT_TCC_MASK >> EDMA3_CCRL_OPT_TCC_SHIFT) & (tcc)) << EDMA3_CCRL_OPT_TCC_SHIFT)

/** PaRAM Set Entry for Link and B count Reload field */
#define EDMA3_RM_PARAM_ENTRY_LINK_BCNTRLD (5U)

    /** To maintain the state of the EDMA3 Resource Manager Object */
    typedef enum
    {
        /** Object deleted */
        EDMA3_RM_DELETED = 0,
        /** Obect Created */
        EDMA3_RM_CREATED = 1,
        /** Object Opened */
        EDMA3_RM_OPENED = 2,
        /** Object Closed */
        EDMA3_RM_CLOSED = 3
    } EDMA3_RM_ObjState;

    /**
 * \brief EDMA3 Hardware Instance Configuration Structure.
 *
 * Used to maintain information of the EDMA3 HW configuration.
 * One such storage exists for each instance of the EDMA 3 HW.
 */
    typedef struct
    {
        /** HW Instance Id of the EDMA3 Controller */
        uint32_t phyCtrllerInstId;

        /** State information of the Resource Manager object */
        EDMA3_RM_ObjState state;

        /** Number of active opens of RM Instances */
        uint32_t numOpens;

        /**
     * \brief Init-time Configuration structure for EDMA3
     * controller, to provide Global SoC specific Information.
     *
     * This configuration will can be provided by the user at run-time,
     * while calling EDMA3_RM_create().
     */
        EDMA3_RM_GblConfigParams gblCfgParams;
    } EDMA3_RM_Obj;

    /**
 * \brief EDMA3 RM Instance Specific Configuration Structure.
 *
 * Used to maintain information of the EDMA3 Res Mgr instances.
 * One such storage exists for each instance of the EDMA3 Res Mgr.
 *
 * Maximum EDMA3_MAX_RM_INSTANCES instances are allowed for
 * each EDMA3 hardware instance, for same or different shadow regions.
 */
    typedef struct
    {
        /**
     * Configuration such as region id, IsMaster, Callback function
     * This configuration is passed to the "Open" API.
     * For a single EDMA3 HW controller, there can be EDMA3_MAX_REGIONS
     * different instances tied to different regions.
     */
        EDMA3_RM_Param initParam;

        /** Pointer to appropriate Shadow Register region of CC Registers */
        EDMA3_CCRL_ShadowRegs *shadowRegs;

        /**
     * Pointer to the EDMA3 RM Object (HW specific)
     * opened by RM instance.
     */
        EDMA3_RM_Obj *pResMgrObjHandle;

        /** Available DMA Channels to the RM Instance */
        uint32_t avlblDmaChannels[EDMA3_MAX_DMA_CHAN_DWRDS];

        /** Available QDMA Channels to the RM Instance */
        uint32_t avlblQdmaChannels[EDMA3_MAX_QDMA_CHAN_DWRDS];

        /** Available PaRAM Sets to the RM Instance */
        uint32_t avlblPaRAMSets[EDMA3_MAX_PARAM_DWRDS];

        /** Available TCCs to the RM Instance */
        uint32_t avlblTccs[EDMA3_MAX_TCC_DWRDS];

        /**
     * Sometimes, PaRAM clearing is not required for some particular RM
     * Instances. In that case, PaRAM Sets allocated will NOT be cleared before
     * allocating to any particular user. It is the responsibility of user
     * to program it accordingly, without assuming anything for a specific
     * field because the PaRAM Set might contain junk values. Not programming
     * it fully might result in erroneous behavior.
     * On the other hand, RM instances can also use this variable to get the
     * PaRAM Sets cleared before allocating them to the specific user.
     * User can program only the selected fields in this case.
     *
     * Value '0' : PaRAM Sets will NOT be cleared during their allocation.
     * Value '1' : PaRAM Sets will be cleared during their allocation.
     *
     * This value can be modified using the IOCTL commands.
     */
        uint32_t paramInitRequired;

        /**
     * Sometimes, global EDMA3 registers (DCHMAP/QCHMAP) and PaRAM Sets should
     * not be modified during EDMA3_RM_allocLogicalChannel (), for some particular RM
     * Instances. In that case, it is the responsibility of user
     * to program them accordingly, when needed, without assuming anything because
     * they might contain junk values. Not programming
     * the registers/PaRAMs fully might result in erroneous behavior.
     * On the other hand, RM instances can also use this variable to get the
     * global registers and PaRAM Sets minimally programmed before allocating them to
     * the specific user.
     * User can program only the remaining fields in this case.
     *
     * Value '0' : EDMA3 registers (DCHMAP/QCHMAP) and PaRAM Sets will NOT be
     *			programmed during their allocation.
     * Value '1' : EDMA3 registers (DCHMAP/QCHMAP) and PaRAM Sets will be
     *			programmed during their allocation.
     *
     * This value can be modified using the IOCTL commands.
     */
        uint32_t regModificationRequired;
        /** 
     * Pointer to the user defined function for mapping cross bar events to 
     * channel. 
     */
        EDMA3_RM_mapXbarEvtToChan mapXbarToChan;

        /** 
     * Pointer to the user defined function for configuring the cross bar 
     * events to appropriate channel in the Control Config TPCC Event Config 
     * registers. 
     */
        EDMA3_RM_xbarConfigScr configScrMapXbarToEvt;

        /** 
     * Pointer to the configuration data structure for
     * mapping cross bar events to channel. 
     */
        EDMA3_RM_GblXbarToChanConfigParams rmXbarToEvtMapConfig;
    } EDMA3_RM_Instance;

    /**
 * \brief EDMA3 Channel-Bound resources.
 *
 * Used to maintain information of the EDMA3 resources
 * (specifically Parameter RAM set and TCC), bound to the
 * particular channel within EDMA3_RM_allocLogicalChannel ().
 */
    typedef struct
    {
        /** PaRAM Set number associated with the particular channel */
        int32_t paRAMId;

        /** TCC associated with the particular channel */
        uint32_t tcc;
    } EDMA3_RM_ChBoundResources;

    /**
 * \brief TCC Callback - Caters to channel specific status reporting.
 */
    typedef struct
    {
        /** Callback function */
        EDMA3_RM_TccCallback tccCb;

        /** Callback data, passed to the Callback function */
        void *cbData;
    } EDMA3_RM_TccCallbackParams;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _EDMA3_RES_MGR_H_ */
