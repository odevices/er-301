/*
 * edma3_rm.h
 *
 * EDMA3 Controller Resource Manager Interface
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

/** @defgroup EDMA3_LLD_RM_API EDMA3 Resource Manager
 *
 * @section Introduction
 *
 * @subsection xxx Overview
 *  EDMA3 Resource Manager is a TI mandated library for all EDMA3 peripheral
 * users to acquire and configure EDMA3 hardware resources (DMA/QDMA channels,
 * PaRAM Sets, TCCs etc.) and DMA Interrupt Service Routines.
 */

#ifndef EDMA3_RM_H_
#define EDMA3_RM_H_

#include <stdint.h>
/** Include common header file */
#include <ti/edma3/edma3_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
@defgroup EDMA3_LLD_RM_SYMBOL  EDMA3 Resource Manager Symbols
@ingroup EDMA3_LLD_RM_API
*/
    /**
@defgroup EDMA3_LLD_RM_DATASTRUCT  EDMA3 Resource Manager Data Structures
@ingroup EDMA3_LLD_RM_API
*/
    /**
@defgroup EDMA3_LLD_RM_FUNCTION  EDMA3 Resource Manager APIs
@ingroup EDMA3_LLD_RM_API
*/

    /**
@defgroup EDMA3_LLD_RM_SYMBOL_DEFINE  EDMA3 Resource Manager Defines
@ingroup EDMA3_LLD_RM_SYMBOL
*/
    /**
@defgroup EDMA3_LLD_RM_SYMBOL_ENUM  EDMA3 Resource Manager Enums
@ingroup EDMA3_LLD_RM_SYMBOL
*/
    /**
@defgroup EDMA3_LLD_RM_SYMBOL_TYPEDEF  EDMA3 Resource Manager Typedefs
@ingroup EDMA3_LLD_RM_SYMBOL
*/

    /**
@defgroup EDMA3_LLD_RM_FUNCTION_INIT  EDMA3 Resource Manager Initialization APIs
@ingroup EDMA3_LLD_RM_FUNCTION
*/
    /**
@defgroup EDMA3_LLD_RM_FUNCTION_BASIC  EDMA3 Resource Manager Basic APIs
@ingroup EDMA3_LLD_RM_FUNCTION
*/
    /**
@defgroup EDMA3_LLD_RM_FUNCTION_ADVANCED  EDMA3 Resource Manager Advanced APIs
@ingroup EDMA3_LLD_RM_FUNCTION
*/

    /** @addtogroup EDMA3_LLD_RM_SYMBOL_ENUM
 @{ */

    /**\enum    EDMA3_RM_TccStatus
 * \brief   This enum defines the channel specific status codes of
 * an EDMA3 transfer. It is returned while calling the channel
 * specific callback function to tell the status.
 */
    typedef enum
    {
        /**
     * DMA Transfer successfully completed (true completion mode)
     * or submitted to the TC (early completion mode).
     */
        EDMA3_RM_XFER_COMPLETE = 1,

        /** Channel Controller has reported an error */
        /**
     * DMA missed events:- for all 64 DMA channels.
     * These get latched in the event missed registers (EMR/EMRH).
     */
        EDMA3_RM_E_CC_DMA_EVT_MISS = 2,

        /**
     * QDMA missed events:- for all QDMA channels.
     * These get latched in the QDMA event missed register (QEMR).
     */
        EDMA3_RM_E_CC_QDMA_EVT_MISS = 3

    } EDMA3_RM_TccStatus;

    /**\enum    EDMA3_RM_GlobalError
 * \brief   This enum defines the global (not specific to any channel)
 * error codes of completion of an EDMA3 transfer.
 */
    typedef enum
    {
        /**
     * Threshold exceed:- for all event queues.
     * These get latched in EDMA3CC error register (CCERR).
     * This error has a direct relation with the setting of
     * EDMA3_RM_GblConfigParams.evtQueueWaterMarkLvl
     */
        EDMA3_RM_E_CC_QUE_THRES_EXCEED = 1,

        /**
     * TCC error:- for outstanding transfer requests expected to return
     * completion code (TCCHEN or TCINTEN bit in OPT is set to 1) exceeding
     * the maximum limit of 63. This also gets latched in the CCERR.
     */
        EDMA3_RM_E_CC_TCC = 2,

        /** Transfer Controller has reported an error */
        /**
     * Detection of a Read error signaled by the source or destination address
     */
        EDMA3_RM_E_TC_MEM_LOCATION_READ_ERROR = 3,

        /**
     * Detection of a Write error signaled by the source or destination address
     */
        EDMA3_RM_E_TC_MEM_LOCATION_WRITE_ERROR = 4,

        /**
     * Attempt to read or write to an invalid address in the configuration
     * memory map.
     */
        EDMA3_RM_E_TC_INVALID_ADDR = 5,

        /**
     * Detection of a FIFO mode TR violating the FIFO mode transfer rules
     * (the source/destination addresses and source/destination indexes must
     * be aligned to 32 bytes).
     */
        EDMA3_RM_E_TC_TR_ERROR = 6
    } EDMA3_RM_GlobalError;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_SYMBOL_TYPEDEF
 @{ */

    /**
 * \brief Global Error callback - caters to module events like bus error etc
 * which are not channel specific. Runs in ISR context.
 *
 * gblerrData is application provided data when open'ing the Resource Manager.
 */
    typedef void (*EDMA3_RM_GblErrCallback)(EDMA3_RM_GlobalError deviceStatus,
                                            uint32_t instanceId,
                                            void *gblerrData);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_DATASTRUCT
 @{ */

    /**\struct  EDMA3_RM_GblErrCallbackParams
 * \brief   Global Error Callback parameters
 *
 * Consists of the Callback function and the data to be passed to it.
 */
    typedef struct
    {
        /**
         * Instance wide callback function to catch non-channel specific errors.
         */
        EDMA3_RM_GblErrCallback gblerrCb;

        /** Application data to be passed back to the Global Error callback */
        void *gblerrData;

    } EDMA3_RM_GblErrCallbackParams;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_SYMBOL_TYPEDEF
 @{ */

    /**
 * \brief TCC callback - caters to channel-specific events like
 * "Event Miss Error" or "Transfer Complete". Runs in ISR context.
 *
 * appData is passed by the application during Register'ing of
 * TCC Callback function.
 */
    typedef void (*EDMA3_RM_TccCallback)(uint32_t tcc,
                                         EDMA3_RM_TccStatus status,
                                         void *appData);

/**
@}
*/

/** @addtogroup EDMA3_LLD_RM_SYMBOL_DEFINE
 @{ */

/**\def     EDMA3_RM_RES_ANY
 * \brief   Used to specify any available Resource Id (EDMA3_RM_ResDesc.resId)
 */
#define EDMA3_RM_RES_ANY (1010U)

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_SYMBOL_ENUM
 @{ */

    /**\enum    EDMA3_RM_ResType
 * \brief   EDMA3 Resource Type
 */
    typedef enum
    {
        /** DMA Channel resource */
        EDMA3_RM_RES_DMA_CHANNEL = 1,

        /** QDMA Channel resource*/
        EDMA3_RM_RES_QDMA_CHANNEL = 2,

        /** TCC resource*/
        EDMA3_RM_RES_TCC = 3,

        /** Parameter RAM Set resource*/
        EDMA3_RM_RES_PARAM_SET = 4

    } EDMA3_RM_ResType;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_DATASTRUCT
 @{ */

    /**\struct  EDMA3_RM_ResDesc
 * \brief   Handle to a Resource.
 */
    typedef struct
    {
        /** Resource Id */
        /**
     * Range of resId values :
     * As an example, for resource Type = EDMA3_RM_RES_DMA_CHANNEL,
     * resId can take values from 0 to EDMA3_MAX_DMA_CH
     * Or
     * resId can take the value EDMA3_RM_RES_ANY.
     */
        uint32_t resId;

        /** Resource Type */
        EDMA3_RM_ResType type;
    } EDMA3_RM_ResDesc;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_FUNCTION_BASIC
 @{ */

    /**
 * \brief   Registers a transfer completion handler for a specific DMA/QDMA
 *			channel
 *
 * This function registers a non-NULL callback function for a specific DMA or QDMA
 * channel and enables the completion interrupt for the TCC associated with
 * the underlying channel in the IER/IERH register. It also sets the DRAE/DRAEH
 * register for the TCC associated with the specified DMA/QDMA channel. If user
 * enables the transfer completion interrupts (intermediate or final) in the OPT
 * field of the associated PaRAM Set, the registered callback function will be
 * called by the EDMA3 Resource Manager.
 *
 * If a call-back function is already registered for the channel, the API fails
 * with the error code EDMA3_RM_E_CALLBACK_ALREADY_REGISTERED.
 *
 * \param   hEdmaResMgr         [IN]    Handle to the previously opened
 *                                      EDMA3 Resource Manager Instance
 * \param   channelObj          [IN]    Channel ID and type (DMA or QDMA
 *                                      Channel), allocated earlier, and
 *                                      corresponding to which a callback
 *                                      function needs to be registered
 *                                      against the associated TCC.
 * \param   tcc                 [IN]    TCC against which the handler needs to
 *                                      be registered.
 * \param   tccCb               [IN]    The Callback function to be registered
 *                                      against the TCC.
 * \param   cbData              [IN]    Callback data to be passed while calling
 *                                      the callback function.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant for unique tcc values. It is non-
 *          re-entrant for same tcc value.
 */
    EDMA3_RM_Result EDMA3_RM_registerTccCb(EDMA3_RM_Handle hEdmaResMgr,
                                           const EDMA3_RM_ResDesc *channelObj,
                                           uint32_t tcc,
                                           EDMA3_RM_TccCallback tccCb,
                                           void *cbData);

    /**
 * \brief   Unregister the previously registered callback function against a
 *          DMA/QDMA channel.
 *
 * This function un-registers the previously registered callback function for
 * the DMA/QDMA channel by removing any stored callback function. Moreover, it
 * clears the:
 *		Interrupt Enable Register (IER/IERH) by writing to the IECR/IECRH
 *			register, for the TCC associated with that particular channel,
 *		DRA/DRAEH register for the TCC associated with the specified DMA/QDMA
 *			channel
 *
 * \param   hEdmaResMgr         [IN]    Handle to the previously opened
 *                                      EDMA3 Resource Manager Instance
 * \param   channelObj          [IN]    Channel ID and type, allocated earlier
 *                                      (DMA or QDMA Channel ONLY), and
 *                                      corresponding to which a TCC is there.
 *                                      Against that TCC, the callback needs
 *                                      to be un-registered.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code.
 *
 * \note    This function is re-entrant for unique (channelObj->type +
 *          channelObj->resId) combination. It is non-re-entrant for same
 *          channelObj Resource.
 */
    EDMA3_RM_Result EDMA3_RM_unregisterTccCb(EDMA3_RM_Handle hEdmaResMgr,
                                             const EDMA3_RM_ResDesc *channelObj);

/**
@}
*/

/** @addtogroup EDMA3_LLD_RM_SYMBOL_DEFINE
 @{ */

/** Resource Manager Error Codes base define */
#define EDMA3_RM_E_BASE (-155)

/**
 * Resource Manager Object Not Deleted yet.
 * So the object cannot be created.
 */
#define EDMA3_RM_E_OBJ_NOT_DELETED (EDMA3_RM_E_BASE)

/**
 * Resource Manager Object Not Closed yet.
 * So the object cannot be deleted.
 */
#define EDMA3_RM_E_OBJ_NOT_CLOSED (EDMA3_RM_E_BASE - 1)

/**
 * Resource Manager Object Not Opened yet
 * So the object cannot be closed.
 */
#define EDMA3_RM_E_OBJ_NOT_OPENED (EDMA3_RM_E_BASE - 2)

/** Invalid Parameter passed to API */
#define EDMA3_RM_E_INVALID_PARAM (EDMA3_RM_E_BASE - 3)

/** Resource requested for freeing is already free */
#define EDMA3_RM_E_RES_ALREADY_FREE (EDMA3_RM_E_BASE - 4)

/** Resource requested for allocation/freeing is not owned */
#define EDMA3_RM_E_RES_NOT_OWNED (EDMA3_RM_E_BASE - 5)

/** Resource is not available */
#define EDMA3_RM_E_SPECIFIED_RES_NOT_AVAILABLE (EDMA3_RM_E_BASE - 6)

/** No Resource of specified type is available */
#define EDMA3_RM_E_ALL_RES_NOT_AVAILABLE (EDMA3_RM_E_BASE - 7)

/** Invalid State of EDMA3 RM Obj */
#define EDMA3_RM_E_INVALID_STATE (EDMA3_RM_E_BASE - 8)

/** Maximum no of Res Mgr Instances already Opened */
#define EDMA3_RM_E_MAX_RM_INST_OPENED (EDMA3_RM_E_BASE - 9)

/**
 * More than one Res Mgr Master Instance NOT supported.
 * Only 1 master can exist.
 */
#define EDMA3_RM_E_RM_MASTER_ALREADY_EXISTS (EDMA3_RM_E_BASE - 10)

/** Callback function already registered. */
#define EDMA3_RM_E_CALLBACK_ALREADY_REGISTERED (EDMA3_RM_E_BASE - 11)

/** Semaphore related error */
#define EDMA3_RM_E_SEMAPHORE (EDMA3_RM_E_BASE - 12)

/** Hardware feature NOT supported */
#define EDMA3_RM_E_FEATURE_UNSUPPORTED (EDMA3_RM_E_BASE - 13)

/** EDMA3 Resource NOT allocated */
#define EDMA3_RM_E_RES_NOT_ALLOCATED (EDMA3_RM_E_BASE - 14)

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_SYMBOL_TYPEDEF
 @{ */

    /**\typedef     EDMA3_RM_RegionId
 * \brief       EDMA3 Region Id
 *
 * Use this to assign channels/PaRAM sets/TCCs to a particular Region.
 */
    typedef uint32_t EDMA3_RM_RegionId;

    /**\typedef     EDMA3_RM_EventQueue
 * \brief       EDMA3 Event Queue assignment
 *
 * There can be 8 Event Queues.  Either of them can be assigned
 * to a DMA/QDMA channel using this.
 *
 */
    typedef uint32_t EDMA3_RM_EventQueue;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_DATASTRUCT
 @{ */

    /**\struct  EDMA3_RM_GblConfigParams
 * \brief   Init-time Configuration structure for EDMA3
 * controller, to provide Global SoC specific Information.
 *
 * This configuration structure is used to specify the EDMA3 Resource Manager
 * global settings, specific to the SoC. For e.g. number of DMA/QDMA channels,
 * number of PaRAM sets, TCCs, event queues, transfer controllers, base
 * addresses of CC global registers and TC registers, interrupt number for
 * EDMA3 transfer completion, CC error, event queues' priority, watermark
 * threshold level etc.
 * This configuration information is SoC specific and could be provided by the
 * user at run-time while creating the EDMA3 RM Object, using API
 * EDMA3_RM_create. In case user doesn't provide it, this information could be
 * taken from the SoC specific configuration file edma3_<SOC_NAME>_cfg.c, in
 * case it is available.
 */
    typedef struct
    {
        /** Number of DMA Channels supported by the underlying EDMA3 Controller. */
        uint32_t numDmaChannels;

        /** Number of QDMA Channels supported by the underlying EDMA3 Controller */
        uint32_t numQdmaChannels;

        /**
     * Number of Interrupt Channels supported by the underlying EDMA3
     * Controller
     */
        uint32_t numTccs;

        /** Number of PaRAM Sets supported by the underlying EDMA3 Controller */
        uint32_t numPaRAMSets;

        /** Number of Event Queues in the underlying EDMA3 Controller */
        uint32_t numEvtQueue;

        /**
     * Number of Transfer Controllers (TCs) in the underlying EDMA3 Controller
     */
        uint32_t numTcs;

        /** Number of Regions in the underlying EDMA3 Controller  */
        uint32_t numRegions;

        /**
     * \brief Channel mapping existence
     *
     * A value of 0 (No channel mapping) implies that there is fixed
     * association between a DMA channel and a PaRAM Set or, in other words,
     * DMA channel n can ONLY use PaRAM Set n (No availability of DCHMAP
     * registers) for transfers to happen.
     *
     * A value of 1 implies the presence of DCHMAP registers for the DMA
     * channels and hence the flexibility of associating any DMA channel to
     * any PaRAM Set. In other words, ANY PaRAM Set can be used for ANY DMA
     * channel (like QDMA Channels).
     */
        uint32_t dmaChPaRAMMapExists;

        /** Existence of memory protection feature */
        uint32_t memProtectionExists;

        /** Base address of EDMA3 CC memory mapped registers. */
        void *globalRegs;

        /** Base address of EDMA3 TCs memory mapped registers. */
        void *tcRegs[EDMA3_MAX_TC];

        /**
     * EDMA3 transfer completion interrupt line (could be different for ARM
     * and DSP)
     */
        uint32_t xferCompleteInt;

        /** EDMA3 CC error interrupt line (could be different for ARM and DSP) */
        uint32_t ccError;

        /** EDMA3 TCs error interrupt line (could be different for ARM and DSP) */
        uint32_t tcError[EDMA3_MAX_TC];

        /**
     * \brief EDMA3 TC priority setting
     *
     * User can program the priority of the Event Queues
     * at a system-wide level.  This means that the user can set the
     * priority of an IO initiated by either of the TCs (Transfer Controllers)
     * relative to IO initiated by the other bus masters on the
     * device (ARM, DSP, USB, etc)
     */
        uint32_t evtQPri[EDMA3_MAX_EVT_QUE];

        /**
     * \brief Event Queues Watermark Levels

     * To Configure the Threshold level of number of events
     * that can be queued up in the Event queues. EDMA3CC error register
     * (CCERR) will indicate whether or not at any instant of time the
     * number of events queued up in any of the event queues exceeds
     * or equals the threshold/watermark value that is set
     * in the queue watermark threshold register (QWMTHRA).
     */
        uint32_t evtQueueWaterMarkLvl[EDMA3_MAX_EVT_QUE];

        /**
     * \brief Default Burst Size (DBS) of TCs.

     * An optimally-sized command is defined by the transfer controller
     * default burst size (DBS). Different TCs can have different
     * DBS values. It is defined in Bytes.
     */
        uint32_t tcDefaultBurstSize[EDMA3_MAX_TC];

        /**
     * \brief Mapping from DMA channels to PaRAM Sets

     * If channel mapping exists (DCHMAP registers are present), this array
     * stores the respective PaRAM Set for each DMA channel. User can
     * initialize each array member with a specific PaRAM Set or with
     * EDMA3_DRV_CH_NO_PARAM_MAP.
     * If channel mapping doesn't exist, it is of no use as the EDMA3 RM
     * automatically uses the right PaRAM Set for that DMA channel.
     * Useful only if mapping exists, otherwise of no use.
     */
        uint32_t dmaChannelPaRAMMap[EDMA3_MAX_DMA_CH];

        /**
      * \brief Mapping from DMA channels to TCCs
      *
      * This array stores the respective TCC (interrupt channel) for each
      * DMA channel. User can initialize each array member with a specific TCC
      * or with EDMA3_DRV_CH_NO_TCC_MAP. This specific
      * TCC code will be returned when the transfer is completed
      * on the mapped DMA channel.
      */
        uint32_t dmaChannelTccMap[EDMA3_MAX_DMA_CH];

        /**
     * \brief Mapping from DMA channels to Hardware Events
     *
     * Each bit in this array corresponds to one DMA channel and tells whether
     * this DMA channel is tied to any peripheral. That is whether any
     * peripheral can send the synch event on this DMA channel or not.
     * 1 means the channel is tied to some peripheral; 0 means it is not.
     * DMA channels which are tied to some peripheral are RESERVED for that
     * peripheral only. They are not allocated when user asks for 'ANY' DMA
     * channel.
     * All channels need not be mapped, some can be free also.
     */
        uint32_t dmaChannelHwEvtMap[EDMA3_MAX_DMA_CHAN_DWRDS];
    } EDMA3_RM_GblConfigParams;

    /**\struct  EDMA3_RM_InstanceInitConfig
 * \brief   Init-time Region Specific Configuration structure for
 * EDMA3 RM, to provide region specific Information.
 *
 * This configuration structure is used to specify which EDMA3 resources are
 * owned and reserved by the EDMA3 RM instance. This configuration
 * structure is shadow region specific and will be provided by the user at
 * run-time while calling EDMA3_RM_open ().
 *
 * Owned resources:
 * ****************
 *
 * EDMA3 RM Instances are tied to different shadow regions and hence different
 * masters. Regions could be:
 *
 * a) ARM,
 * b) DSP,
 * c) IMCOP (Imaging Co-processor) etc.
 *
 * User can assign each EDMA3 resource to a shadow region using this structure.
 * In this way, user specifies which resources are owned by the specific EDMA3
 * RM Instance.
 * This assignment should also ensure that the same resource is not assigned
 * to more than one shadow regions (unless desired in that way). Any assignment
 * not following the above mentioned approach may have catastrophic
 * consequences.
 *
 *
 * Reserved resources:
 * *******************
 *
 * During EDMA3 RM initialization, user can reserve some of the EDMA3 resources
 * for future use, by specifying which resources to reserve in the configuration
 * data structure. These (critical) resources are reserved in advance so that
 * they should not be allocated to someone else and thus could be used in
 * future for some specific purpose.
 *
 * User can request different EDMA3 resources using two methods:
 * a) By passing the resource type and the actual resource id,
 * b) By passing the resource type and ANY as resource id
 *
 * For e.g. to request DMA channel 31, user will pass 31 as the resource id.
 * But to request ANY available DMA channel (mainly used for memory-to-memory
 * data transfer operations), user will pass EDMA3_DRV_DMA_CHANNEL_ANY as the
 * resource id.
 *
 * During initialization, user may have reserved some of the DMA channels for
 * some specific purpose (mainly for peripherals using EDMA). These reserved
 * DMA channels then will not be returned when user requests ANY as the
 * resource id.
 *
 * Same logic applies for QDMA channels and TCCs.
 *
 * For PaRAM Set, there is one difference. If the DMA channels are one-to-one
 * tied to their respective PaRAM Sets (i.e. user cannot 'choose' the PaRAM Set
 * for a particular DMA channel), EDMA3 RM automatically reserves all those
 * PaRAM Sets which are tied to the DMA channels. Then those PaRAM Sets would
 * not be returned when user requests for ANY PaRAM Set (specifically for
 * linking purpose). This is done in order to avoid allocating the PaRAM Set,
 * tied to a particular DMA channel, for linking purpose. If this constraint is
 * not there, that DMA channel thus could not be used at all, because of the
 * unavailability of the desired PaRAM Set.
 */
    typedef struct
    {
        /** PaRAM Sets owned by the EDMA3 RM Instance. */
        uint32_t ownPaRAMSets[EDMA3_MAX_PARAM_DWRDS];

        /** DMA Channels owned by the EDMA3 RM Instance. */
        uint32_t ownDmaChannels[EDMA3_MAX_DMA_CHAN_DWRDS];

        /** QDMA Channels owned by the EDMA3 RM Instance. */
        uint32_t ownQdmaChannels[EDMA3_MAX_QDMA_CHAN_DWRDS];

        /** TCCs owned by the EDMA3 RM Instance. */
        uint32_t ownTccs[EDMA3_MAX_TCC_DWRDS];

        /**
     * \brief   Reserved PaRAM Sets
     *
     * PaRAM Sets reserved during initialization for future use. These will not
     * be given when user requests for ANY available PaRAM Set using
     * 'EDMA3_RM_PARAM_ANY' as resource/channel id.
     */
        uint32_t resvdPaRAMSets[EDMA3_MAX_PARAM_DWRDS];

        /**
     * \brief   Reserved DMA channels
     *
     * DMA channels reserved during initialization for future use. These will
     * not be given when user requests for ANY available DMA channel using
     * 'EDMA3_RM_DMA_CHANNEL_ANY' as resource/channel id.
     */
        uint32_t resvdDmaChannels[EDMA3_MAX_DMA_CHAN_DWRDS];

        /**
     * \brief   Reserved QDMA channels
     *
     * QDMA channels reserved during initialization for future use. These will
     * not be given when user requests for ANY available QDMA channel using
     * 'EDMA3_RM_QDMA_CHANNEL_ANY' as resource/channel id.
     */
        uint32_t resvdQdmaChannels[EDMA3_MAX_QDMA_CHAN_DWRDS];

        /**
     * \brief   Reserved TCCs
     *
     * TCCs reserved during initialization for future use. These will not
     * be given when user requests for ANY available TCC using
     * 'EDMA3_RM_TCC_ANY' as resource/channel id.
     */
        uint32_t resvdTccs[EDMA3_MAX_TCC_DWRDS];
    } EDMA3_RM_InstanceInitConfig;

    /**\struct      EDMA3_RM_Param
 * \brief       Used to Initialize the Resource Manager Instance
 *
 * This configuration structure is used to initialize the EDMA3 RM Instance.
 * This configuration information is passed while opening the RM instance.
 */
    typedef struct
    {
        /** Shadow Region Identification */
        EDMA3_RM_RegionId regionId;

        /**
     * It tells whether the EDMA3 RM instance is Master or not. Only the shadow
     * region associated with this master instance will receive the EDMA3
     * interrupts (if enabled).
     */
        uint32_t isMaster;

        /**
     * EDMA3 resources related shadow region specific information. Which all
     * EDMA3 resources are owned and reserved by this particular instance are
     * told in this configuration structure.
     * User can also pass this structure as NULL. In that case, default static
     * configuration would be taken from the platform specific configuration
     * files (part of the Resource Manager), if available.
     */
        EDMA3_RM_InstanceInitConfig *rmInstInitConfig;

        /**
     * EDMA3 RM Instance specific semaphore handle.
     * Used to share resources (DMA/QDMA channels, PaRAM Sets, TCCs etc)
     * among different users.
     */
        void *rmSemHandle;

        /**
     * Whether initialization of Region Specific Registers is required or not?
     */
        uint32_t regionInitEnable;

        /** Instance wide Global Error callback parameters */
        EDMA3_RM_GblErrCallbackParams gblerrCbParams;
    } EDMA3_RM_Param;

    /**\struct      EDMA3_RM_MiscParam
 * \brief       Used to specify the miscellaneous options during Resource
 * Manager Initialization.
 *
 * This configuration structure is used to specify some misc options
 * while creating the RM Object. New options may also be added into this
 * structure in future.
 */
    typedef struct
    {
        /**
     * In a multi-master system (for e.g. ARM + DSP), this option is used to
     * distinguish between Master and Slave. Only the Master is allowed  to
     * program the global EDMA3 registers (like Queue priority, Queue water-
     * mark level, error registers etc).
     */
        uint16_t isSlave;

        /** For future use **/
        uint16_t param;
    } EDMA3_RM_MiscParam;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_FUNCTION_INIT
 @{ */

    /**
 * \brief   Create EDMA3 Resource Manager Object
 *
 * This API is used to create the EDMA3 Resource Manager Object. It should be
 * called only ONCE for each EDMA3 hardware instance.
 *
 * Init-time Configuration structure for EDMA3 hardware is provided to pass the
 * SoC specific information. This configuration information could be provided
 * by the user at init-time. In case user doesn't provide it, this information
 * could be taken from the SoC specific configuration file
 * edma3_<SOC_NAME>_cfg.c, in case it is available.
 *
 * This API clears the error specific registers (EMCR/EMCRh, QEMCR, CCERRCLR)
 * and sets the TCs priorities and Event Queues' watermark levels, if the 'miscParam'
 * argument is NULL. User can avoid these registers' programming (in some specific
 * use cases) by SETTING the 'isSlave' field of 'EDMA3_RM_MiscParam' configuration
 * structure and passing this structure as the third argument (miscParam).
 *
 * After successful completion of this API, Resource Manager Object's state
 * changes to EDMA3_RM_CREATED from EDMA3_RM_DELETED.
 *
 * \param phyCtrllerInstId  [IN]    EDMA3 Controller Instance Id
 *                                 (Hardware instance id, starting from 0).
 * \param gblCfgParams      [IN]    SoC specific configuration structure for the
 *                                  EDMA3 Hardware.
 * \param miscParam         [IN]    Misc configuration options provided in the
 *                                  structure 'EDMA3_RM_MiscParam'.
 *                                  For default options, user can pass NULL
 *                                  in this argument.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 */
    EDMA3_RM_Result EDMA3_RM_create(uint32_t phyCtrllerInstId,
                                    const EDMA3_RM_GblConfigParams *gblCfgParams,
                                    const void *miscParam);

    /**
 * \brief   Delete EDMA3 Resource Manager Object
 *
 * This API is used to delete the EDMA3 RM Object. It should be called
 * once for each EDMA3 hardware instance, ONLY after closing all the
 * previously opened EDMA3 RM Instances.
 *
 * After successful completion of this API, Resource Manager Object's state
 * changes to EDMA3_RM_DELETED.
 *
 * \param phyCtrllerInstId  [IN]    EDMA3 Phy Controller Instance Id (Hardware
 *                                  instance id, starting from 0).
 * \param   param           [IN]    For possible future use.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 */
    EDMA3_RM_Result EDMA3_RM_delete(uint32_t phyCtrllerInstId,
                                    const void *param);

    /**
 * \brief   Open EDMA3 Resource Manager Instance
 *
 * This API is used to open an EDMA3 Resource Manager Instance. It could be
 * called multiple times, for each possible EDMA3 shadow region. Maximum
 * EDMA3_MAX_RM_INSTANCES instances are allowed for each EDMA3 hardware
 * instance.
 *
 * Also, only ONE Master Resource Manager Instance is permitted. This master
 * instance (and hence the region to which it belongs) will only receive the
 * EDMA3 interrupts, if enabled.
 *
 * User could pass the instance specific configuration structure
 * (initParam->rmInstInitConfig) as a part of the 'initParam' structure,
 * during init-time. In case user doesn't provide it, this information could
 * be taken from the SoC specific configuration file edma3_<SOC_NAME>_cfg.c,
 * in case it is available.
 *
 * By default, this Resource Manager instance will clear the PaRAM Sets while
 * allocating them. To change the default behavior, user should use the IOCTL
 * interface appropriately.
 *
 * \param   phyCtrllerInstId    [IN]    EDMA3 Controller Instance Id (Hardware
 *                                      instance id, starting from 0).
 * \param   initParam           [IN]    Used to Initialize the Resource Manager
 *                                      Instance (Master or Slave).
 * \param   errorCode           [OUT]   Error code while opening RM instance.
 *
 * \return  Handle to the opened Resource Manager instance Or NULL in case of
 *          error.
 *
 * \note    This function disables the global interrupts (by calling API
 *          edma3OsProtectEntry with protection level
 *          EDMA3_OS_PROTECT_INTERRUPT) while modifying the global RM data
 *          structures, to make it re-entrant.
 */
    EDMA3_RM_Handle EDMA3_RM_open(uint32_t phyCtrllerInstId,
                                  const EDMA3_RM_Param *initParam,
                                  EDMA3_RM_Result *errorCode);

    /**
 * \brief  Close EDMA3 Resource Manager Instance
 *
 * This API is used to close a previously opened EDMA3 RM Instance.
 *
 * \param  hEdmaResMgr         [IN]    Handle to the previously opened Resource
 *                                     Manager Instance.
 * \param  param               [IN]    For possible future use.
 *
 * \return EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function disables the global interrupts (by calling API
 *          edma3OsProtectEntry with protection level
 *          EDMA3_OS_PROTECT_INTERRUPT) while modifying the global RM data
 *          structures, to make it re-entrant.
 */
    EDMA3_RM_Result EDMA3_RM_close(EDMA3_RM_Handle hEdmaResMgr,
                                   const void *param);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_FUNCTION_BASIC
 @{ */

    /**
 * \brief   This API is used to allocate specified EDMA3 Resources like
 * DMA/QDMA channel, PaRAM Set or TCC.
 *
 * Note: To free the resources allocated by this API, user should call
 * EDMA3_RM_freeResource () ONLY to de-allocate all the allocated resources.
 *
 * User can either request a specific resource by passing the resource id
 * in 'resObj->resId' OR request ANY available resource of the type
 * 'resObj->type'.
 *
 * ANY types of resources are those resources when user doesn't care about the
 * actual resource allocated; user just wants a resource of the type specified.
 * One use-case is to perform memory-to-memory data transfer operation. This
 * operation can be performed using any available DMA or QDMA channel.
 * User doesn't need any specific channel for the same.
 *
 * To allocate a specific resource, first this API checks whether that resource
 * is OWNED by the Resource Manager instance. Then it checks the current
 * availability of that resource.
 *
 * To allocate ANY available resource, this API tries to allocate a resource
 * from the pool of (owned && non_reserved && available_right_now) resources.
 *
 * After allocating a DMA/QDMA channel, the same resource is enabled in
 * the shadow region specific register (DRAE/DRAEH/QRAE). DRAE/DRAEH register
 * for a TCC is updated only when user registers a callback function against
 * that TCC using EDMA3_RM_registerTccCb().
 *
 * Allocated PaRAM Set is initialized to NULL before this API returns if user
 * has requested for one.
 *
 * \param  hEdmaResMgr      [IN]        Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   resObj          [IN/OUT]    Handle to the resource descriptor
 *                                      object, which needs to be allocated.
 *                                      In case user passes a specific resource
 *                                      Id, resObj value is left unchanged.
 *                                      In case user requests ANY available
 *                                      resource, the allocated resource id is
 *                                      returned in resObj.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function acquires a RM Instance specific semaphore
 *          to prevent simultaneous access to the global pool of resources.
 *          It is re-entrant, but should not be called from the user callback
 *          function (ISR context).
 */
    EDMA3_RM_Result EDMA3_RM_allocResource(EDMA3_RM_Handle hEdmaResMgr,
                                           EDMA3_RM_ResDesc *resObj);

    /**
 * \brief   This API is used to free previously allocated EDMA3 Resources like
 * DMA/QDMA channel, PaRAM Set or TCC.
 *
 * To free a specific resource, first this API checks whether that resource is
 * OWNED by the Resource Manager Instance. Then it checks whether that resource
 * has been allocated by the Resource Manager instance or not.
 *
 * After freeing a DMA/QDMA channel, the same resource is disabled in
 * the shadow region specific register (DRAE/DRAEH/QRAE). This operation is
 * performed for TCC while unregistering the callback function.
 *
 * \param  hEdmaResMgr         [IN]    Handle to the previously opened Resource
 *                                     Manager Instance.
 * \param   resObj             [IN]    Handle to the resource descriptor
 *                                     object, which needs to be freed.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function disables the global interrupts to prevent
 *          simultaneous access to the global pool of resources.
 *          It is re-entrant.
 */
    EDMA3_RM_Result EDMA3_RM_freeResource(EDMA3_RM_Handle hEdmaResMgr,
                                          const EDMA3_RM_ResDesc *resObj);

    /**
 * \brief   Allocate a contiguous region of specified EDMA3 Resource
 * like DMA channel, QDMA channel, PaRAM Set or TCC.
 *
 * This API is used to allocate a contiguous region of specified EDMA3
 * Resources like DMA channel, QDMA channel, PaRAM Set or TCC.
 *
 * User can specify a particular resource Id to start with and go up to the
 * number of resources requested. The specific resource id to start from could
 * be passed in 'firstResIdObject->resId' and the number of resources requested
 * in 'numResources'.
 *
 * User can also request ANY available resource(s) of the type
 * 'firstResIdObject->type' by specifying 'firstResIdObject->resId' as
 * EDMA3_RM_RES_ANY.
 *
 * ANY types of resources are those resources when user doesn't care about the
 * actual resource allocated; user just wants a resource of the type specified.
 * One use-case is to perform memory-to-memory data transfer operation. This
 * operation can be performed using any available DMA or QDMA channel. User
 * doesn't need any specific channel for the same.
 *
 * To allocate specific contiguous resources, first this API checks whether
 * those requested resources are OWNED by the Resource Manager instance. Then
 * it checks the current availability of those resources.
 *
 * To allocate ANY available contiguous resources, this API tries to allocate
 * resources from the pool of (owned && non_reserved && available_right_now)
 * resources.
 *
 * After allocating DMA/QDMA channels, the same resources are enabled in
 * the shadow region specific register (DRAE/DRAEH/QRAE). DRAE/DRAEH register
 * for a TCC is updated only when user registers a callback function against
 * that TCC using EDMA3_RM_registerTccCb(). Allocated PaRAM Sets are initialized
 * to NULL before this API returns.
 *
 * \param   hEdmaResMgr         [IN]    Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   firstResIdObj       [IN]    Handle to the first resource descriptor
 *                                      object, which needs to be allocated.
 *                                      firstResIdObject->resId could be a valid
 *                                      resource id in case user wants to
 *                                      allocate specific resources OR it could
 *                                      be EDMA3_RM_RES_ANY in case user wants
 *                                      only the required number of resources
 *                                      and doesn't care about which resources
 *                                      were allocated.
 * \param   numResources        [IN]    Number of contiguous resources user
 *                                      wants to allocate.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function acquires a RM Instance specific semaphore
 *          to prevent simultaneous access to the global pool of resources.
 *          It is re-entrant, but should not be called from the user callback
 *          function (ISR context).
 */
    EDMA3_RM_Result EDMA3_RM_allocContiguousResource(EDMA3_RM_Handle hEdmaResMgr,
                                                     EDMA3_RM_ResDesc *firstResIdObj,
                                                     uint32_t numResources);

    /**
 * \brief   Free a contiguous region of specified EDMA3 Resource
 * like DMA channel, QDMA channel, PaRAM Set or TCC, previously allocated.
 *
 * This API frees a contiguous region of specified EDMA3 Resources
 * like DMA channel, QDMA channel, PaRAM Set or TCC, which have been previously
 * allocated. In case of an error during the freeing of any specific resource,
 * user can check the 'firstResIdObj' object to know the last resource id
 * whose freeing has failed. In case of success, there is no need to check this
 * object.
 *
 * \param  hEdmaResMgr         [IN]         Handle to the previously opened
 *                                          Resource Manager Instance.
 * \param   firstResIdObj      [IN/OUT]     Handle to the first resource
 *                                          descriptor object, which needs to be
 *                                          freed. In case of an error while
 *                                          freeing any particular resource,
 *                                          the last resource id whose freeing has
 *                                          failed is returned in this resource
 *                                          descriptor object.
 * \param   numResources      [IN]          Number of contiguous resources allocated
 *                                          previously which user wants to release
 *
 * \note        This is a re-entrant function which internally calls
 *              EDMA3_RM_freeResource() for resource de-allocation.
 */
    EDMA3_RM_Result EDMA3_RM_freeContiguousResource(EDMA3_RM_Handle hEdmaResMgr,
                                                    EDMA3_RM_ResDesc *firstResIdObj,
                                                    uint32_t numResources);

/**
@}
*/

/** @addtogroup EDMA3_LLD_RM_SYMBOL_DEFINE
 @{ */

/* Defines for Logical Channel Values */
/*---------------------------------------------------------------------------*/
/**
 * Used to specify any available DMA Channel while requesting
 * one. Used in the API EDMA3_RM_allocLogicalChannel ().
 * DMA channel from the pool of (owned && non_reserved && available_right_now)
 * DMA channels will be chosen and returned.
 */
#define EDMA3_RM_DMA_CHANNEL_ANY (1011U)

/**
 * Used to specify any available QDMA Channel while requesting
 * one. Used in the API EDMA3_RM_allocLogicalChannel().
 * QDMA channel from the pool of (owned && non_reserved && available_right_now)
 * QDMA channels will be chosen and returned.
 */
#define EDMA3_RM_QDMA_CHANNEL_ANY (1012U)

/**
 * Used to specify any available TCC while requesting
 * one. Used in the API EDMA3_RM_allocLogicalChannel(), for
 * both DMA and QDMA channels.
 * TCC from the pool of (owned && non_reserved && available_right_now)
 * TCCs will be chosen and returned.
 */
#define EDMA3_RM_TCC_ANY (1013U)

/**
 * Used to specify any available PaRAM Set while requesting
 * one. Used in the API EDMA3_RM_allocLogicalChannel(), for
 * both DMA/QDMA and Link channels.
 * PaRAM Set from the pool of (owned && non_reserved && available_right_now)
 * PaRAM Sets will be chosen and returned.
 */
#define EDMA3_RM_PARAM_ANY (1014U)

/**
 * This define is used to specify that a DMA channel is NOT tied to any PaRAM
 * Set and hence any available PaRAM Set could be used for that DMA channel.
 * It could be used in dmaChannelPaRAMMap [EDMA3_MAX_DMA_CH], in global
 * configuration structure EDMA3_RM_GblConfigParams.
 *
 * This value should mandatorily be used to mark DMA channels with no initial
 * mapping to specific PaRAM Sets.
 */
#define EDMA3_RM_CH_NO_PARAM_MAP (1015U)

/**
 * This define is used to specify that the DMA/QDMA channel is not tied to any
 * TCC and hence any available TCC could be used for that DMA/QDMA channel.
 * It could be used in dmaChannelTccMap [EDMA3_MAX_DMA_CH], in global
 * configuration structure EDMA3_RM_GblConfigParams.
 *
 * This value should mandatorily be used to mark DMA channels with no initial
 * mapping to specific TCCs.
 */
#define EDMA3_RM_CH_NO_TCC_MAP (1016U)

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_SYMBOL_ENUM
 @{ */

    /**
 * \brief DMA Channels assigned to different Hardware Events.
 *
 * They should be used while requesting a specific DMA channel.
 * One possible usage is to maintain a SoC specific file, which will
 * contain the mapping of these hardware events to the respective
 * peripherals for better understanding and lesser probability of
 * errors. Also, if any event associated with a particular peripheral
 * gets changed, only that SoC specific file needs to be changed.
 * for eg, the sample SoC specific file "soc.h" can have these defines:
 *
 * #define EDMA3_RM_HW_CHANNEL_MCBSP_TX        EDMA3_RM_HW_CHANNEL_EVENT_2
 * #define EDMA3_RM_HW_CHANNEL_MCBSP_RX        EDMA3_RM_HW_CHANNEL_EVENT_3
 *
 * These defines will be used by the MCBSP driver. The same event
 * EDMA3_RM_HW_CHANNEL_EVENT_2/3 could be mapped to some other
 * peripheral also.
 */
    typedef enum
    {
        /** Channel assigned to EDMA3 Event 0 */
        EDMA3_RM_HW_CHANNEL_EVENT_0 = 0,
        /** Channel assigned to EDMA3 Event 1 */
        EDMA3_RM_HW_CHANNEL_EVENT_1,
        /** Channel assigned to EDMA3 Event 2 */
        EDMA3_RM_HW_CHANNEL_EVENT_2,
        /** Channel assigned to EDMA3 Event 3 */
        EDMA3_RM_HW_CHANNEL_EVENT_3,
        /** Channel assigned to EDMA3 Event 4 */
        EDMA3_RM_HW_CHANNEL_EVENT_4,
        /** Channel assigned to EDMA3 Event 5 */
        EDMA3_RM_HW_CHANNEL_EVENT_5,
        /** Channel assigned to EDMA3 Event 6 */
        EDMA3_RM_HW_CHANNEL_EVENT_6,
        /** Channel assigned to EDMA3 Event 7 */
        EDMA3_RM_HW_CHANNEL_EVENT_7,
        /** Channel assigned to EDMA3 Event 8 */
        EDMA3_RM_HW_CHANNEL_EVENT_8,
        /** Channel assigned to EDMA3 Event 9 */
        EDMA3_RM_HW_CHANNEL_EVENT_9,
        /** Channel assigned to EDMA3 Event 10 */
        EDMA3_RM_HW_CHANNEL_EVENT_10,
        /** Channel assigned to EDMA3 Event 11 */
        EDMA3_RM_HW_CHANNEL_EVENT_11,
        /** Channel assigned to EDMA3 Event 12 */
        EDMA3_RM_HW_CHANNEL_EVENT_12,
        /** Channel assigned to EDMA3 Event 13 */
        EDMA3_RM_HW_CHANNEL_EVENT_13,
        /** Channel assigned to EDMA3 Event 14 */
        EDMA3_RM_HW_CHANNEL_EVENT_14,
        /** Channel assigned to EDMA3 Event 15 */
        EDMA3_RM_HW_CHANNEL_EVENT_15,
        /** Channel assigned to EDMA3 Event 16 */
        EDMA3_RM_HW_CHANNEL_EVENT_16,
        /** Channel assigned to EDMA3 Event 17 */
        EDMA3_RM_HW_CHANNEL_EVENT_17,
        /** Channel assigned to EDMA3 Event 18 */
        EDMA3_RM_HW_CHANNEL_EVENT_18,
        /** Channel assigned to EDMA3 Event 19 */
        EDMA3_RM_HW_CHANNEL_EVENT_19,
        /** Channel assigned to EDMA3 Event 20 */
        EDMA3_RM_HW_CHANNEL_EVENT_20,
        /** Channel assigned to EDMA3 Event 21 */
        EDMA3_RM_HW_CHANNEL_EVENT_21,
        /** Channel assigned to EDMA3 Event 22 */
        EDMA3_RM_HW_CHANNEL_EVENT_22,
        /** Channel assigned to EDMA3 Event 23 */
        EDMA3_RM_HW_CHANNEL_EVENT_23,
        /** Channel assigned to EDMA3 Event 24 */
        EDMA3_RM_HW_CHANNEL_EVENT_24,
        /** Channel assigned to EDMA3 Event 25 */
        EDMA3_RM_HW_CHANNEL_EVENT_25,
        /** Channel assigned to EDMA3 Event 26 */
        EDMA3_RM_HW_CHANNEL_EVENT_26,
        /** Channel assigned to EDMA3 Event 27 */
        EDMA3_RM_HW_CHANNEL_EVENT_27,
        /** Channel assigned to EDMA3 Event 28 */
        EDMA3_RM_HW_CHANNEL_EVENT_28,
        /** Channel assigned to EDMA3 Event 29 */
        EDMA3_RM_HW_CHANNEL_EVENT_29,
        /** Channel assigned to EDMA3 Event 30 */
        EDMA3_RM_HW_CHANNEL_EVENT_30,
        /** Channel assigned to EDMA3 Event 31 */
        EDMA3_RM_HW_CHANNEL_EVENT_31,
        /** Channel assigned to EDMA3 Event 32 */
        EDMA3_RM_HW_CHANNEL_EVENT_32,
        /** Channel assigned to EDMA3 Event 33 */
        EDMA3_RM_HW_CHANNEL_EVENT_33,
        /** Channel assigned to EDMA3 Event 34 */
        EDMA3_RM_HW_CHANNEL_EVENT_34,
        /** Channel assigned to EDMA3 Event 35 */
        EDMA3_RM_HW_CHANNEL_EVENT_35,
        /** Channel assigned to EDMA3 Event 36 */
        EDMA3_RM_HW_CHANNEL_EVENT_36,
        /** Channel assigned to EDMA3 Event 37 */
        EDMA3_RM_HW_CHANNEL_EVENT_37,
        /** Channel assigned to EDMA3 Event 38 */
        EDMA3_RM_HW_CHANNEL_EVENT_38,
        /** Channel assigned to EDMA3 Event 39 */
        EDMA3_RM_HW_CHANNEL_EVENT_39,
        /** Channel assigned to EDMA3 Event 40 */
        EDMA3_RM_HW_CHANNEL_EVENT_40,
        /** Channel assigned to EDMA3 Event 41 */
        EDMA3_RM_HW_CHANNEL_EVENT_41,
        /** Channel assigned to EDMA3 Event 42 */
        EDMA3_RM_HW_CHANNEL_EVENT_42,
        /** Channel assigned to EDMA3 Event 43 */
        EDMA3_RM_HW_CHANNEL_EVENT_43,
        /** Channel assigned to EDMA3 Event 44 */
        EDMA3_RM_HW_CHANNEL_EVENT_44,
        /** Channel assigned to EDMA3 Event 45 */
        EDMA3_RM_HW_CHANNEL_EVENT_45,
        /** Channel assigned to EDMA3 Event 46 */
        EDMA3_RM_HW_CHANNEL_EVENT_46,
        /** Channel assigned to EDMA3 Event 47 */
        EDMA3_RM_HW_CHANNEL_EVENT_47,
        /** Channel assigned to EDMA3 Event 48 */
        EDMA3_RM_HW_CHANNEL_EVENT_48,
        /** Channel assigned to EDMA3 Event 49 */
        EDMA3_RM_HW_CHANNEL_EVENT_49,
        /** Channel assigned to EDMA3 Event 50 */
        EDMA3_RM_HW_CHANNEL_EVENT_50,
        /** Channel assigned to EDMA3 Event 51 */
        EDMA3_RM_HW_CHANNEL_EVENT_51,
        /** Channel assigned to EDMA3 Event 52 */
        EDMA3_RM_HW_CHANNEL_EVENT_52,
        /** Channel assigned to EDMA3 Event 53 */
        EDMA3_RM_HW_CHANNEL_EVENT_53,
        /** Channel assigned to EDMA3 Event 54 */
        EDMA3_RM_HW_CHANNEL_EVENT_54,
        /** Channel assigned to EDMA3 Event 55 */
        EDMA3_RM_HW_CHANNEL_EVENT_55,
        /** Channel assigned to EDMA3 Event 56 */
        EDMA3_RM_HW_CHANNEL_EVENT_56,
        /** Channel assigned to EDMA3 Event 57 */
        EDMA3_RM_HW_CHANNEL_EVENT_57,
        /** Channel assigned to EDMA3 Event 58 */
        EDMA3_RM_HW_CHANNEL_EVENT_58,
        /** Channel assigned to EDMA3 Event 59 */
        EDMA3_RM_HW_CHANNEL_EVENT_59,
        /** Channel assigned to EDMA3 Event 60 */
        EDMA3_RM_HW_CHANNEL_EVENT_60,
        /** Channel assigned to EDMA3 Event 61 */
        EDMA3_RM_HW_CHANNEL_EVENT_61,
        /** Channel assigned to EDMA3 Event 62 */
        EDMA3_RM_HW_CHANNEL_EVENT_62,
        /** Channel assigned to EDMA3 Event 63 */
        EDMA3_RM_HW_CHANNEL_EVENT_63
    } EDMA3_RM_HW_CHANNEL_EVENT;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_FUNCTION_ADVANCED
 @{ */

    /**
 * \brief   Request a DMA/QDMA/Link channel.
 *
 * This API is used to allocate a logical channel (DMA/QDMA/Link) along with
 * the associated resources. For DMA and QDMA channels, TCC and PaRAM Set are
 * also allocated along with the requested channel. For Link channel, ONLY a
 * PaRAM Set is allocated.
 *
 * Note: To free the logical channel allocated by this API, user should call
 * EDMA3_RM_freeLogicalChannel () ONLY to de-allocate all the allocated resources
 * and remove certain mappings.
 *
 * User can request a specific logical channel by passing the channel id in
 * 'lChObj->resId' and channel type in 'lChObj->type'. Note that the channel
 * id is the same as the actual resource id. For e.g. in the case of QDMA
 * channels, valid channel ids are from 0 to 7 only.
 *
 * User can also request ANY available logical channel of the type
 * 'lChObj->type' by specifying 'lChObj->resId' as:
 *  a)  EDMA3_RM_DMA_CHANNEL_ANY: For DMA channels
 *  b)  EDMA3_RM_QDMA_CHANNEL_ANY: For QDMA channels, and
 *  c)  EDMA3_RM_PARAM_ANY: For Link channels. Normally user should use this
 *      value to request link channels (PaRAM Sets used for linking purpose
 *      only), unless he wants to use some specific link channels (PaRAM Sets)
 *      which is also allowed.
 *
 * This API internally uses EDMA3_RM_allocResource () to allocate the desired
 * resources (DMA/QDMA channel, PaRAM Set and TCC).
 *
 * For DMA/QDMA channels, after allocating all the EDMA3 resources, this API
 * sets the TCC field of the OPT PaRAM Word with the allocated TCC.
 *
 * For DMA channel, it also sets the DCHMAP register, if required.
 *
 * For QDMA channel, it sets the QCHMAP register and CCNT as trigger word and
 * enables the QDMA channel by writing to the QEESR register.
 *
 * \param  hEdmaResMgr      [IN]        Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param  lChObj           [IN/OUT]    Handle to the requested logical channel
 *                                      object, which needs to be allocated.
 *                                      It could be a specific logical channel
 *                                      or ANY available logical channel of the
 *                                      requested type.
 *                                      In case user passes a specific resource
 *                                      Id, lChObj value is left unchanged. In
 *                                      case user requests ANY available
 *                                      resource, the allocated resource id is
 *                                      returned in lChObj->resId.
 *
 *  \param  pParam          [IN/OUT]    PaRAM Set for a particular logical
 *                                      (DMA/QDMA) channel. Not used if user
 *                                      requested for a Link channel.
 *                                      In case user passes a specific PaRAM
 *                                      Set value, pParam value is left
 *                                      unchanged. In case user requests ANY
 *                                      available PaRAM Set by passing
 *                                      'EDMA3_RM_PARAM_ANY' in pParam,
 *                                      the allocated one is returned in pParam.
 *
 *  \param  pTcc            [IN/OUT]    TCC for a particular logical (DMA/QDMA)
 *                                      channel. Not used if user requested for
 *                                      a Link channel.
 *                                      In case user passes a specific TCC
 *                                      value, pTcc value is left unchanged.
 *                                      In case user requests ANY
 *                                      available TCC by passing
 *                                      'EDMA3_RM_TCC_ANY' in pTcc,
 *                                      the allocated one is returned in pTcc.
 *
 * \return  EDMA3_RM_SOK or EDMA_RM Error Code
 *
 * \note    This function internally calls EDMA3_RM_allocResource (), which
 *          acquires a RM Instance specific semaphore to prevent simultaneous
 *          access to the global pool of resources. It is re-entrant for unique
 *          logical channel values, but SHOULD NOT be called from the user
 *          callback function (ISR context).
 */
    EDMA3_RM_Result EDMA3_RM_allocLogicalChannel(EDMA3_RM_Handle hEdmaResMgr,
                                                 EDMA3_RM_ResDesc *lChObj,
                                                 uint32_t *pParam,
                                                 uint32_t *pTcc);

    /** \fn     EDMA3_RM_Result EDMA3_RM_freeLogicalChannel (EDMA3_RM_Handle
 *                       hEdmaResMgr, EDMA3_RM_ResDesc *lChObj)
 *  \brief  This API is used to free the specified channel (DMA/QDMA/Link) and
 *          its associated resources (PaRAM Set, TCC etc).
 *
 * This API internally uses EDMA3_RM_freeResource () to free the desired
 * resources.
 *
 * For DMA/QDMA channels, it also clears the DCHMAP/QCHMAP registers
 *
 * \param  hEdmaResMgr      [IN]    Handle to the previously opened Resource
 *                                  Manager Instance.
 * \param  lChObj           [IN]    Handle to the logical channel object,
 *                                  which needs to be freed
 *
 * \return  EDMA3_RM_SOK or EDMA_RM Error Code
 *
 * \note    This is a re-entrant function which internally calls
 *          EDMA3_RM_freeResource () for resource de-allocation.
 */
    EDMA3_RM_Result EDMA3_RM_freeLogicalChannel(EDMA3_RM_Handle hEdmaResMgr,
                                                EDMA3_RM_ResDesc *lChObj);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_DATASTRUCT
 @{ */

    /**\struct  EDMA3_RM_ParamentryRegs
 * \brief   EDMA3 PaRAM Set
 *
 * This is a mapping of the EDMA3 PaRAM set provided to the user
 * for ease of modification of the individual PaRAM words.
 *
 * It could be used by the advanced users to program the PaRAM Set directly,
 * without using any API.
 */
    typedef struct
    {
        /** OPT field of PaRAM Set */
        volatile uint32_t OPT;

        /**
     * \brief Starting byte address of Source
     * For FIFO mode, srcAddr must be a 256-bit aligned address.
     */
        volatile uint32_t SRC;

        /**
     * Number of bytes in each Array (ACNT) (16 bits) and
     * Number of Arrays in each Frame (BCNT) (16 bits).
     */
        volatile uint32_t A_B_CNT;

        /**
     * \brief Starting byte address of destination
     * For FIFO mode, destAddr must be a 256-bit aligned address.
     * i.e. 5 LSBs should be 0.
     */
        volatile uint32_t DST;

        /**
     * Index between consec. arrays of a Source Frame (SRCBIDX) (16 bits) and
     * Index between consec. arrays of a Destination Frame (DSTBIDX) (16 bits).
     *
     * If SAM is set to 1 (via channelOptions) then srcInterArrIndex should
     * be an even multiple of 32 bytes.
     *
     * If DAM is set to 1 (via channelOptions) then destInterArrIndex should
     * be an even multiple of 32 bytes
     */
        volatile uint32_t SRC_DST_BIDX;

        /**
     * \brief Address for linking (AutoReloading of a PaRAM Set) (16 bits)
     * and Reload value of the numArrInFrame (BCNT) (16 bits).
     *
     * Link field must point to a valid aligned 32-byte PaRAM set
     * A value of 0xFFFF means no linking.
     *
     * B count reload field is relevant only for A-sync transfers.
     */
        volatile uint32_t LINK_BCNTRLD;

        /**
     * \brief Index between consecutive frames of a Source Block (SRCCIDX)
     * (16 bits) and Index between consecutive frames of a Dest Block
     * (DSTCIDX) (16 bits).
     */
        volatile uint32_t SRC_DST_CIDX;

        /**
     * \brief Number of Frames in a block (CCNT) (16 bits).
     */
        volatile uint32_t CCNT;
    } EDMA3_RM_ParamentryRegs;

#if defined(__GNUC__) && !defined(__TI_COMPILER_VERSION__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define EDMA_MODE_LITTLE_ENDIAN 1
#else
#define EDMA_MODE_BIG_ENDIAN 1
#endif
#else
#ifndef _BIG_ENDIAN
#define EDMA_MODE_LITTLE_ENDIAN 1
#else
#define EDMA_MODE_BIG_ENDIAN 1
#endif
#endif

/**\struct      EDMA3_RM_PaRAMRegs
 * \brief       EDMA3 PaRAM Set in User Configurable format
 *
 * This is a mapping of the EDMA3 PaRAM set provided to the user
 * for ease of modification of the individual fields.
 */
#if defined(EDMA_MODE_LITTLE_ENDIAN)
    /* LITTLE_ENDIAN_MODE */
    typedef struct
    {
        /** OPT field of PaRAM Set */
        volatile uint32_t opt;

        /**
         * \brief Starting byte address of Source
         * For FIFO mode, srcAddr must be a 256-bit aligned address.
         */
        volatile uint32_t srcAddr;

        /**
         * \brief Number of bytes in each Array (ACNT)
         */
        volatile uint16_t aCnt;

        /**
         * \brief Number of Arrays in each Frame (BCNT)
         */
        volatile uint16_t bCnt;

        /**
         * \brief Starting byte address of destination
         * For FIFO mode, destAddr must be a 256-bit aligned address.
         * i.e. 5 LSBs should be 0.
         */
        volatile uint32_t destAddr;

        /**
         * \brief Index between consec. arrays of a Source Frame (SRCBIDX)
         * If SAM is set to 1 (via channelOptions) then srcInterArrIndex should
         * be an even multiple of 32 bytes.
         */
        volatile int16_t srcBIdx;

        /**
         * \brief Index between consec. arrays of a Destination Frame (DSTBIDX)
         * If DAM is set to 1 (via channelOptions) then destInterArrIndex should
         * be an even multiple of 32 bytes
         */
        volatile int16_t destBIdx;

        /**
         * \brief Address for linking (AutoReloading of a PaRAM Set)
         * This must point to a valid aligned 32-byte PaRAM set
         * A value of 0xFFFF means no linking
         * Linking is especially useful for use with ping-pong buffers and
         * circular buffers
         */
        volatile uint16_t linkAddr;

        /**
         * \brief Reload value of the numArrInFrame (BCNT)
         * Relevant only for A-sync transfers
         */
        volatile uint16_t bCntReload;

        /**
         * \brief Index between consecutive frames of a Source Block (SRCCIDX)
         */
        volatile int16_t srcCIdx;

        /**
         * \brief Index between consecutive frames of a Dest Block (DSTCIDX)
         */
        volatile int16_t destCIdx;

        /**
         * \brief Number of Frames in a block (CCNT)
         */
        volatile uint16_t cCnt;

        /**
         * \brief Reserved
         */
        volatile int16_t reserved;
    } EDMA3_RM_PaRAMRegs;
#else
/* BIG_ENDIAN_MODE */
typedef struct
{
    /** OPT field of PaRAM Set */
    volatile uint32_t opt;

    /**
         * \brief Starting byte address of Source
         * For FIFO mode, srcAddr must be a 256-bit aligned address.
         */
    volatile uint32_t srcAddr;

    /**
         * \brief Number of Arrays in each Frame (BCNT)
         */
    volatile uint16_t bCnt;

    /**
         * \brief Number of bytes in each Array (ACNT)
         */
    volatile uint16_t aCnt;

    /**
         * \brief Starting byte address of destination
         * For FIFO mode, destAddr must be a 256-bit aligned address.
         * i.e. 5 LSBs should be 0.
         */
    volatile uint32_t destAddr;

    /**
         * \brief Index between consec. arrays of a Destination Frame (DSTBIDX)
         * If DAM is set to 1 (via channelOptions) then destInterArrIndex should
         * be an even multiple of 32 bytes
         */
    volatile int16_t destBIdx;

    /**
         * \brief Index between consec. arrays of a Source Frame (SRCBIDX)
         * If SAM is set to 1 (via channelOptions) then srcInterArrIndex should
         * be an even multiple of 32 bytes.
         */
    volatile int16_t srcBIdx;

    /**
         * \brief Reload value of the numArrInFrame (BCNT)
         * Relevant only for A-sync transfers
         */
    volatile uint16_t bCntReload;

    /**
         * \brief Address for linking (AutoReloading of a PaRAM Set)
         * This must point to a valid aligned 32-byte PaRAM set
         * A value of 0xFFFF means no linking
         * Linking is especially useful for use with ping-pong buffers and
         * circular buffers
         */
    volatile uint16_t linkAddr;

    /**
         * \brief Index between consecutive frames of a Dest Block (DSTCIDX)
         */
    volatile int16_t destCIdx;

    /**
         * \brief Index between consecutive frames of a Source Block (SRCCIDX)
         */
    volatile int16_t srcCIdx;

    /**
         * \brief Reserved
         */
    volatile int16_t reserved;

    /**
         * \brief Number of Frames in a block (CCNT)
         */
    volatile uint16_t cCnt;
} EDMA3_RM_PaRAMRegs;
#endif /* #if defined(__GNUC__) && !defined(__ti__) */

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_SYMBOL_ENUM
 @{ */

    /**\enum    EDMA3_RM_QdmaTrigWord
 * \brief   QDMA Trigger Word
 *
 * Use this enum to set the QDMA trigger word to any of the
 * 8 DWords(uint32_t) within a Parameter RAM set
 */
    typedef enum
    {
        /**
         * Set the OPT field (Offset Address 0h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_OPT = 0,
        /**
         * Set the SRC field (Offset Address 4h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_SRC = 1,
        /**
         * Set the (ACNT + BCNT) field (Offset Address 8h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_ACNT_BCNT = 2,
        /**
         * Set the DST field (Offset Address Ch Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_DST = 3,
        /**
         * Set the (SRCBIDX + DSTBIDX) field (Offset Address 10h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_SRC_DST_BIDX = 4,
        /**
         * Set the (LINK + BCNTRLD) field (Offset Address 14h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_LINK_BCNTRLD = 5,
        /**
         * Set the (SRCCIDX + DSTCIDX) field (Offset Address 18h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_SRC_DST_CIDX = 6,
        /**
         * Set the (CCNT + RSVD) field (Offset Address 1Ch Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_CCNT = 7,
        /** Default Trigger Word */
        EDMA3_RM_QDMA_TRIG_DEFAULT = 7
    } EDMA3_RM_QdmaTrigWord;

    /**\enum    EDMA3_RM_Cntrlr_PhyAddr
 * \brief   CC/TC Physical Address
 *
 * Use this enum to get the physical address of the Channel Controller or the
 * Transfer Controller. The address returned could be used by the advanced
 * users to set/get some specific registers direclty.
 */
    typedef enum
    {
        /** Channel Controller Physical Address */
        EDMA3_RM_CC_PHY_ADDR = 0,
        /** Transfer Controller 0 Physical Address */
        EDMA3_RM_TC0_PHY_ADDR,
        /** Transfer Controller 1 Physical Address */
        EDMA3_RM_TC1_PHY_ADDR,
        /** Transfer Controller 2 Physical Address */
        EDMA3_RM_TC2_PHY_ADDR,
        /** Transfer Controller 3 Physical Address */
        EDMA3_RM_TC3_PHY_ADDR,
        /** Transfer Controller 4 Physical Address */
        EDMA3_RM_TC4_PHY_ADDR,
        /** Transfer Controller 5 Physical Address */
        EDMA3_RM_TC5_PHY_ADDR,
        /** Transfer Controller 6 Physical Address */
        EDMA3_RM_TC6_PHY_ADDR,
        /** Transfer Controller 7 Physical Address */
        EDMA3_RM_TC7_PHY_ADDR
    } EDMA3_RM_Cntrlr_PhyAddr;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_FUNCTION_ADVANCED
 @{ */

    /**
 * \brief   Bind the resources DMA Channel and PaRAM Set. Both the DMA channel
 * and the PaRAM set should be previously allocated. If they are not,
 * this API will result in error.
 *
 * This API sets the DCHMAP register for a specific DMA channel. This register
 * is used to specify the PaRAM Set associated with that particular DMA Channel.
 *
 * \param   hEdmaResMgr         [IN]    Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   channelId           [IN]    Previously allocated DMA Channel on which
 *                                      Transfer will occur.
 * \param   paRAMId             [IN]    Previously allocated PaRAM Set which
 *                                      needs to be associated with the dma channel.
 *
 * \return  EDMA3_RM_SOK or EDMA_RM Error Code
 *
 * \note    This API is useful only for the EDMA3 Controllers which have a
 *          register for mapping a DMA Channel to a particular PaRAM Set
 *          (DCHMAP register).
 *          On platforms where this feature is not supported, this API
 *          returns error code: EDMA3_RM_E_FEATURE_UNSUPPORTED.
 *          This function is re-entrant for unique channelId. It is
 *          non-re-entrant for same channelId values.
 */
    EDMA3_RM_Result EDMA3_RM_mapEdmaChannel(EDMA3_RM_Handle hEdmaResMgr,
                                            uint32_t channelId,
                                            uint32_t paRAMId);

    /**
 * \brief   Bind the resources QDMA Channel and PaRAM Set. Also, Set the
 * trigger word for the QDMA channel. Both the QDMA channel and the PaRAM set
 * should be previously allocated. If they are not, this API will result in error.
 *
 * This API sets the QCHMAP register for a specific QDMA channel. This register
 * is used to specify the PaRAM Set associated with that particular QDMA
 * Channel along with the trigger word.
 *
 * \param   hEdmaResMgr     [IN]    Handle to the previously opened Resource
 *                                  Manager Instance.
 * \param   channelId       [IN]    Previously allocated  QDMA Channel on which
 *                                  Transfer will occur.
 * \param   paRAMId         [IN]    Previously allocated PaRAM Set, which needs to
 *                                      be associated with channelId
 * \param   trigWord        [IN]    The Trigger Word for the channel.
 *                                  Trigger Word is the word in the PaRAM
 *                                  Register Set which - when written to by CPU
 *                                  -will start the QDMA transfer automatically
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant for unique channelId. It is non-re-entrant
 *          for same channelId values.
 */
    EDMA3_RM_Result EDMA3_RM_mapQdmaChannel(EDMA3_RM_Handle hEdmaResMgr,
                                            uint32_t channelId,
                                            uint32_t paRAMId,
                                            EDMA3_RM_QdmaTrigWord trigWord);

    /**
 * \brief   Set the Channel Controller (CC) Register value
 *
 * \param   hEdmaResMgr     [IN]    Handle to the previously opened Resource
 *                                  Manager Instance.
 * \param   regOffset       [IN]    CC Register offset whose value needs to be
 *                                  set. It should be word-aligned.
 * \param   newRegValue     [IN]    New CC Register Value
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is non re-entrant for users using the same
 *          Resource Manager handle.
 *          Before modifying a register, it tries to acquire a semaphore
 *          (RM instance specific), to protect simultaneous
 *          modification of the same register by two different users.
 *          After the successful change, it releases the semaphore.
 *          For users using different RM handles, this function is re-entrant.
 */
    EDMA3_RM_Result EDMA3_RM_setCCRegister(EDMA3_RM_Handle hEdmaResMgr,
                                           uint32_t regOffset,
                                           uint32_t newRegValue);

    /**
 * \brief   Get the Channel Controller (CC) Register value
 *
 * \param   hEdmaResMgr     [IN]        Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   regOffset       [IN]        CC Register offset whose value is
 *                                      needed. It should be word-aligned.
 * \param   regValue        [IN/OUT]    Fetched CC Register Value
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_RM_Result EDMA3_RM_getCCRegister(EDMA3_RM_Handle hEdmaResMgr,
                                           uint32_t regOffset,
                                           uint32_t *regValue);

    /**
 * \brief   Wait for a transfer completion interrupt to occur and clear it.
 *
 * This is a blocking function that returns when the IPR/IPRH bit corresponding
 * to the tccNo specified, is SET. It clears the corresponding bit while
 * returning also.
 *
 * This function waits for the specific bit indefinitely in a tight loop, with
 * out any delay in between. USE IT CAUTIOUSLY.
 *
 * \param   hEdmaResMgr     [IN]        Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   tccNo           [IN]        TCC, specific to which the function
 *                                      waits on a IPR/IPRH bit.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant for different tccNo.
 *
 */
    EDMA3_RM_Result EDMA3_RM_waitAndClearTcc(EDMA3_RM_Handle hEdmaResMgr,
                                             uint32_t tccNo);

    /**
 * \brief   Returns the status of a previously initiated transfer.
 *
 * This is a non-blocking function that returns the status of a previously
 * initiated transfer, based on the IPR/IPRH bit. This bit corresponds to
 * the tccNo specified by the user. It clears the corresponding bit, if SET,
 * while returning also.
 *
 * \param   hEdmaResMgr     [IN]        Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   tccNo           [IN]        TCC, specific to which the function
 *                                      checks the status of the IPR/IPRH bit.
 * \param   tccStatus       [IN/OUT]    Status of the transfer is returned here.
 *                                      Returns "TRUE" if the transfer has
 *                                      completed (IPR/IPRH bit SET),
 *                                      "FALSE" if the transfer has not
 *                                      completed successfully (IPR/IPRH bit
 *                                      NOT SET).
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant for different tccNo.
 */
    EDMA3_RM_Result EDMA3_RM_checkAndClearTcc(EDMA3_RM_Handle hEdmaResMgr,
                                              uint32_t tccNo,
                                              uint16_t *tccStatus);

    /**
 * \brief   Copy the user specified PaRAM Set onto the PaRAM Set
 *          associated with the logical channel (DMA/QDMA/Link).
 *
 * This API takes a PaRAM Set as input and copies it onto the actual PaRAM Set
 * associated with the logical channel. OPT field of the PaRAM Set is written
 * first and the CCNT field is written last.
 *
 * Caution: It should be used carefully when programming the QDMA channels whose
 *          trigger words are not CCNT field.
 *
 * \param   hEdmaResMgr     [IN]        Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   lChObj          [IN]        Logical Channel object for which new
 *                                      PaRAM set is specified. User should pass
 *                                      the resource type and id in this object.
 * \param   newPaRAM        [IN]        PaRAM set to be copied onto existing one
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant for unique lChObj values. It is non-
 *          re-entrant for same lChObj value.
 */
    EDMA3_RM_Result EDMA3_RM_setPaRAM(EDMA3_RM_Handle hEdmaResMgr,
                                      const EDMA3_RM_ResDesc *lChObj,
                                      const EDMA3_RM_PaRAMRegs *newPaRAM);

    /**
 * \brief   Retrieve existing PaRAM set associated with specified logical
 *          channel (DMA/QDMA/Link).
 *
 * \param   hEdmaResMgr     [IN]        Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   lChObj          [IN]        Logical Channel object for which the
 *                                      PaRAM set is requested. User should pass
 *                                      the resource type and id in this object.
 * \param   currPaRAM       [IN/OUT]    User gets the existing PaRAM here.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_RM_Result EDMA3_RM_getPaRAM(EDMA3_RM_Handle hEdmaResMgr,
                                      const EDMA3_RM_ResDesc *lChObj,
                                      EDMA3_RM_PaRAMRegs *currPaRAM);

    /**
 * \brief   Get the PaRAM Set Physical Address associated with a logical channel
 *
 * This function returns the PaRAM Set Phy Address (unsigned 32 bits).
 * The returned address could be used by the advanced users to program the
 * PaRAM Set directly without using any APIs.
 *
 * Least significant 16 bits of this address could be used to program
 * the LINK field in the PaRAM Set.
 * Users which program the LINK field directly SHOULD use this API
 * to get the associated PaRAM Set address with the LINK channel.
 *
 * \param   hEdmaResMgr     [IN]        Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   lChObj          [IN]        Logical Channel object for which the
 *                                      PaRAM set physical address is required.
 *                                      User should pass the resource type and
 *                                      id in this object.
 * \param   paramPhyAddr [IN/OUT]       PaRAM Set physical address is returned
 *                                      here.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_RM_Result EDMA3_RM_getPaRAMPhyAddr(EDMA3_RM_Handle hEdmaResMgr,
                                             const EDMA3_RM_ResDesc *lChObj,
                                             uint32_t *paramPhyAddr);

    /**
 * \brief   Get the Channel Controller or Transfer Controller (n) Physical
 *          Address.
 *
 * \param   hEdmaResMgr     [IN]        Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   controllerId    [IN]        Channel Controller or Transfer
 *                                      Controller (n) for which the physical
 *                                      address is required.
 * \param   phyAddress      [IN/OUT]    Physical address is returned here.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_RM_Result EDMA3_RM_getBaseAddress(EDMA3_RM_Handle hEdmaResMgr,
                                            EDMA3_RM_Cntrlr_PhyAddr controllerId,
                                            uint32_t *phyAddress);

    /**
 * \brief   Get the SoC specific configuration structure for the EDMA3 Hardware.
 *
 * This API is used to fetch the global SoC specific configuration structure
 * for the EDMA3 Hardware. It is useful for the user who has not passed
 * this information during EDMA3_RM_create() and taken the default configuration
 * coming along with the package.
 *
 * \param phyCtrllerInstId  [IN]    EDMA3 Controller Instance Id
 *                                 (Hardware instance id, starting from 0).
 * \param gblCfgParams      [IN/OUT]    SoC specific configuration structure for the
 *                                  EDMA3 Hardware will be returned here.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_RM_Result EDMA3_RM_getGblConfigParams(uint32_t phyCtrllerInstId,
                                                EDMA3_RM_GblConfigParams *gblCfgParams);

    /**
 * \brief   Get the RM Instance specific configuration structure for different
 *           EDMA3 resources' usage (owned resources, reserved resources etc).
 *
 * This API is used to fetch the Resource Manager Instance specific configuration
 * structure, for a specific shadow region. It is useful for the user who has not passed
 * this information during EDMA3_RM_opn() and taken the default configuration
 * coming along with the package. EDMA3 resources, owned and reserved by this RM
 * instance, will be returned from this API.
 *
 * \param   hEdmaResMgr         [IN]    Handle to the previously opened Resource
 *                                      Manager Instance.
 * \param   instanceInitConfig      [IN/OUT]    RM Instance specific configuration
 *                                      structure will be returned here.
 *
 * \return  EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_RM_Result EDMA3_RM_getInstanceInitCfg(EDMA3_RM_Handle hEdmaResMgr,
                                                EDMA3_RM_InstanceInitConfig *instanceInitConfig);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_SYMBOL_ENUM
 @{ */

    /**\enum    EDMA3_RM_IoctlCmd
 * \brief   EDMA3 Resource Manager IOCTL commands
 */
    typedef enum
    {
        /* Min IOCTL */
        EDMA3_RM_IOCTL_MIN_IOCTL = 0,

        /**
     * PaRAM Sets will be cleared OR will not be cleared
     * during allocation, depending upon this option.
     *
     * For e.g.,
     * To clear the PaRAM Sets during allocation,
     * cmdArg = (void *)1;
     *
     * To NOT clear the PaRAM Sets during allocation,
     * cmdArg = (void *)0;
     *
     * For all other values, it will return error.
     *
     * By default, PaRAM Sets will be cleared during allocation.
     *
     * Note: Since this enum can change the behavior how the resources are
     * initialized during their allocation, user is adviced to not use this
     * command while allocating the resources. User should first change the
     * behavior of resources' initialization and then should use start
     * allocating resources.
     */
        EDMA3_RM_IOCTL_SET_PARAM_CLEAR_OPTION,

        /**
     * To check whether PaRAM Sets will be cleared or not
     * during allocation.
     * If the value read is '1', it means that PaRAM Sets are getting cleared
     * during allocation.
     * If the value read is '0', it means that PaRAM Sets are NOT getting cleared
     * during allocation.
     *
     * For e.g.,
     * uint32_t *isParamClearingDone = (uint32_t *)cmdArg;
     * (*isParamClearingDone) = paramClearingRequired;
     */
        EDMA3_RM_IOCTL_GET_PARAM_CLEAR_OPTION,

        /**
     * Global EDMA3 registers (DCHMAP/QCHMAP) and PaRAM Sets will be modified
     * OR will not be modified during EDMA3_RM_allocLogicalChannel (), depending
     * upon this option.
     *
     * For e.g.,
     * To modify the Registers or PaRAM Sets during allocation,
     * cmdArg = (void *)1;
     *
     * To NOT modify the Registers or PaRAM Sets during allocation,
     * cmdArg = (void *)0;
     *
     * For all other values, it will return error.
     *
     * By default, Registers or PaRAM Sets will be programmed during allocation.
     *
     * Note: Since this enum can change the behavior how the resources are
     * initialized during their allocation, user is adviced to not use this
     * command while allocating the resources. User should first change the
     * behavior of resources' initialization and then should use start
     * allocating resources.
     */
        EDMA3_RM_IOCTL_SET_GBL_REG_MODIFY_OPTION,

        /**
     * To check whether Global EDMA3 registers (DCHMAP/QCHMAP) and PaRAM Sets
     * will be programmed or not during allocation (EDMA3_RM_allocLogicalChannel ()).
     * If the value read is '1', it means that the registers/PaRAMs are getting programmed
     * during allocation.
     * If the value read is '0', it means that the registers/PaRAMs are NOT getting programmed
     * during allocation.
     *
     * For e.g.,
     * uint32_t *isParamClearingDone = (uint32_t *)cmdArg;
     * (*isParamClearingDone) = paramClearingRequired;
     */
        EDMA3_RM_IOCTL_GET_GBL_REG_MODIFY_OPTION,

        /* Max IOCTLs */
        EDMA3_RM_IOCTL_MAX_IOCTL
    } EDMA3_RM_IoctlCmd;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_RM_FUNCTION_ADVANCED
 @{ */

    /**
 *  \brief EDMA3 Resource Manager IOCTL
 *
 *  This function provides IOCTL functionality for EDMA3 Resource Manager
 *
 *  \param  hEdmaResMgr      [IN]       Handle to the previously opened Resource
 *                                      Manager Instance.
 *  \param  cmd             [IN]        IOCTL command to be performed
 *  \param  cmdArg          [IN/OUT]    IOCTL command argument (if any)
 *  \param  param           [IN/OUT]    Device/Cmd specific argument.
 *
 *  \return EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note For 'EDMA3_RM_IOCTL_GET_PARAM_CLEAR_OPTION', this function is re-entrant.
 * For 'EDMA3_RM_IOCTL_SET_PARAM_CLEAR_OPTION', this function is re-entrant for
 * different Resource Manager Instances (handles).
 */
    EDMA3_RM_Result EDMA3_RM_Ioctl(
        EDMA3_RM_Handle hEdmaResMgr,
        EDMA3_RM_IoctlCmd cmd,
        void *cmdArg,
        void *param);

#define EDMA3_RM_MAX_XBAR_EVENTS (63U)

    /**\struct  EDMA3_RM_GblXbarToChanConfigParams
 * \brief   Init-time Configuration structure for EDMA3
 * controller, to provide Global SoC specific Information.
 *
 * This configuration structure is used to specify the EDMA3 Driver
 * global settings, specific to the SoC. 
 * This configuraion structure provides the details of the mapping of cross bar 
 * events to available channels.
 * This configuration information is SoC specific and could be provided by the
 * user at run-time while creating the EDMA3 Driver Object, using API
 * EDMA3_RM_initXbarEventMap. In case user doesn't provide it, 
 * this information could be taken from the SoC specific configuration 
 * file edma3_<SOC_NAME>_cfg.c, incase it is available.
 */
    typedef struct
    {
        /**
     * \brief Mapping from DMA channels to Hardware Events
     *
     * Each element in this array corresponds to one cross bar event and tells 
     * whether this event is mapped to any DMA channel. That is whether any
     * free or unused DMA channel can be mapped to this event.
     * -1 means the cross bar event is not mapped to any DMA channel; 
     * Any number from 0 to 63 means this event is mapped to specified channel.
     * All channels need not be mapped, some can be free also.
     * For the cross bar event mapped to DMA channel, appropriate Control Config 
     * register of TPCC event mux register should be configured.
     */
        int32_t dmaMapXbarToChan[EDMA3_RM_MAX_XBAR_EVENTS];
    } EDMA3_RM_GblXbarToChanConfigParams;

    /**
 * \brief  Associates cross bar mapped event to channel
 *
 * This function have to be defined in the configuration file. 
 * This function will be called only if the channel requested for is beyond the
 * maximum number of channels.
 * This function should read from the global cross bar mapped configuration 
 * data structure and return the mapped channel number to this event.
 *
 * \param   eventNum            [IN]    Event number
 * \param   chanNum             [IN/OUT]Return the channel number to which the 
 *                                      request event is mapped to.
 * \param   edmaGblXbarConfig   [IN]    This is the configuration data structure
 *                                      for mapping the events to the channel
 *
 * \return      EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant for unique event values. It is
 *          non-re-entrant for same event values.
 */
    typedef EDMA3_RM_Result (*EDMA3_RM_mapXbarEvtToChan)(uint32_t eventNum,
                                                         uint32_t *chanNum,
                                                         const EDMA3_RM_GblXbarToChanConfigParams *edmaGblXbarConfig);

    /**
 * \brief  Writes to the cross bar mapped event to channel to system 
 *         configuration register
 *
 * This function have to be defined in the configuration file. 
 * This function will be called only if the event number requested for is 
 * beyond the maximum number of channels and if any channel is allocated to this 
 * event.
 * This function should read the cross bar mapped event number and write the 
 * allocated channel number in Control Config Event Mux registers.
 *
 * \param   eventNum            [IN]    Event number
 * \param   chanNum             [IN/OUT]Return the channel number to which the 
 *                                      request event is mapped to.
 *
 * \return      EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function is re-entrant for unique event values. It is
 *          non-re-entrant for same event values.
 */
    typedef EDMA3_RM_Result (*EDMA3_RM_xbarConfigScr)(uint32_t eventNum,
                                                      uint32_t chanNum);

    /**
 * \brief  Initialize the cross bar mapped event to channel function
 *
 * This API provides interface to associate the cross bar mapped event to edma 
 * channel in the driver. 
 * This function will called by the application during initilization.
 * User could pass the application specific configuration structure
 * during init-time. In case user doesn't provide it, this information could
 * be taken from the SoC specific configuration file edma3_<SOC_NAME>_cfg.c,
 * in case it is available.
 * \param   hEdma               [IN]    Handle to the EDMA Driver Instance.
 * \param   edmaGblXbarConfig   [IN]    This is the configuration data structure
 *                                      for mapping the events to the channel
 * \param   mapXbarEvtFunc      [IN]    This is the user defined function for 
 *                                      mapping the cross bar event to channel.
 *
 * \return      EDMA3_RM_SOK or EDMA3_RM Error Code
 *
 * \note    This function disables the global interrupts (by calling API
 *          edma3OsProtectEntry with protection level
 *          EDMA3_OS_PROTECT_INTERRUPT) while modifying the global data
 *          structures, to make it re-entrant.
 */
    EDMA3_RM_Result EDMA3_RM_initXbarEventMap(EDMA3_RM_Handle hEdma,
                                              const EDMA3_RM_GblXbarToChanConfigParams *edmaGblXbarConfig,
                                              EDMA3_RM_mapXbarEvtToChan mapXbarEvtFunc,
                                              EDMA3_RM_xbarConfigScr configXbarScr);

    /**
@}
*/

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _EDMA3_RM_H_ */
