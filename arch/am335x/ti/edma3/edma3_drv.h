/*
 * edma3_drv.h
 *
 * This file contains Application Interface for the EDMA3 Driver. EDMA3 Driver
 * uses the EDMA3 Resource Manager internally for resource allocation, interrupt
 * handling and EDMA3 registers programming.
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

/** @defgroup EDMA3_LLD_DRV_API EDMA3 Driver
 *
 * @section Introduction
 *
 * @subsection xxx Overview
 *  EDMA3 Driver is a functional library providing APIs for programming,
 *  scheduling and synchronizing with EDMA transfers and many more.
 */

#ifndef EDMA3_DRV_H_
#define EDMA3_DRV_H_

#include <stdint.h>

/* Include the Resource Manager header file */
#include <ti/edma3/edma3_rm.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
@defgroup EDMA3_LLD_DRV_SYMBOL  EDMA3 Driver Symbols
@ingroup EDMA3_LLD_DRV_API
*/
    /**
@defgroup EDMA3_LLD_DRV_DATASTRUCT  EDMA3 Driver Data Structures
@ingroup EDMA3_LLD_DRV_API
*/
    /**
@defgroup EDMA3_LLD_DRV_FUNCTION  EDMA3 Driver APIs
@ingroup EDMA3_LLD_DRV_API
*/

    /**
@defgroup EDMA3_LLD_DRV_SYMBOL_DEFINE  EDMA3 Driver Defines
@ingroup EDMA3_LLD_DRV_SYMBOL
*/
    /**
@defgroup EDMA3_LLD_DRV_SYMBOL_ENUM  EDMA3 Driver Enums
@ingroup EDMA3_LLD_DRV_SYMBOL
*/

    /**
@defgroup EDMA3_LLD_DRV_FUNCTION_INIT  EDMA3 Driver Initialization APIs
@ingroup EDMA3_LLD_DRV_FUNCTION
*/
    /**
@defgroup EDMA3_LLD_DRV_FUNCTION_BASIC  EDMA3 Driver Basic APIs
@ingroup EDMA3_LLD_DRV_FUNCTION
*/
    /**
@defgroup EDMA3_LLD_DRV_FUNCTION_ADVANCED  EDMA3 Driver Advanced APIs
@ingroup EDMA3_LLD_DRV_FUNCTION
*/

    /** @addtogroup EDMA3_LLD_DRV_SYMBOL_DEFINE
 @{ */

    /**
  * @brief   This is the EDMA3 LLD Version. Versions numbers are encoded in the following 
  * format:
  *  0xAABBCCDD -> Arch (AA); API Changes (BB); Major (CC); Minor (DD) 
  */
#define EDMA3_LLD_DRV_VERSION_ID (0x020C0118U)

/**
 * @brief   This is the version string which describes the EDMA3 LLD along with the
 * date and build information.
 */
#define EDMA3_LLD_DRV_VERSION_STR "EDMA3 LLD Revision: 02.12.01.25"

/** @brief EDMA3 Driver Error Codes Base define */
#define EDMA3_DRV_E_BASE (-128)

/**
 * EDMA3 Driver Object Not Deleted yet.
 * So it cannot be created.
 */
#define EDMA3_DRV_E_OBJ_NOT_DELETED (EDMA3_DRV_E_BASE)

/**
 * EDMA3 Driver Object Not Closed yet.
 * So it cannot be deleted.
 */
#define EDMA3_DRV_E_OBJ_NOT_CLOSED (EDMA3_DRV_E_BASE - 1)

/**
 * EDMA3 Driver Object Not Opened yet
 * So it cannot be closed.
 */
#define EDMA3_DRV_E_OBJ_NOT_OPENED (EDMA3_DRV_E_BASE - 2)

/**
 * While closing EDMA3 Driver, Resource Manager
 * Close Failed.
 */
#define EDMA3_DRV_E_RM_CLOSE_FAIL (EDMA3_DRV_E_BASE - 3)

/** The requested DMA Channel not available */
#define EDMA3_DRV_E_DMA_CHANNEL_UNAVAIL (EDMA3_DRV_E_BASE - 4)

/** The requested QDMA Channel not available */
#define EDMA3_DRV_E_QDMA_CHANNEL_UNAVAIL (EDMA3_DRV_E_BASE - 5)

/** The requested PaRAM Set not available */
#define EDMA3_DRV_E_PARAM_SET_UNAVAIL (EDMA3_DRV_E_BASE - 6)

/** The requested TCC not available */
#define EDMA3_DRV_E_TCC_UNAVAIL (EDMA3_DRV_E_BASE - 7)

/** The registration of TCC failed */
#define EDMA3_DRV_E_TCC_REGISTER_FAIL (EDMA3_DRV_E_BASE - 8)

/** The binding of Channel and PaRAM Set failed */
#define EDMA3_DRV_E_CH_PARAM_BIND_FAIL (EDMA3_DRV_E_BASE - 9)

/**
 * The address of the memory location passed as argument
 * is not properly aligned. It should be 32 bytes aligned.
 */
#define EDMA3_DRV_E_ADDRESS_NOT_ALIGNED (EDMA3_DRV_E_BASE - 10)

/** Invalid Parameter passed to API */
#define EDMA3_DRV_E_INVALID_PARAM (EDMA3_DRV_E_BASE - 11)

/** Invalid State of EDMA3 HW Obj */
#define EDMA3_DRV_E_INVALID_STATE (EDMA3_DRV_E_BASE - 12)

/** EDMA3 Driver instance already exists for the specified region */
#define EDMA3_DRV_E_INST_ALREADY_EXISTS (EDMA3_DRV_E_BASE - 13)

/** FIFO width not supported by the requested TC */
#define EDMA3_DRV_E_FIFO_WIDTH_NOT_SUPPORTED (EDMA3_DRV_E_BASE - 14)

/** Semaphore related error */
#define EDMA3_DRV_E_SEMAPHORE (EDMA3_DRV_E_BASE - 15)

/** EDMA3 Driver Instance does not exist, it is not opened yet */
#define EDMA3_DRV_E_INST_NOT_OPENED (EDMA3_DRV_E_BASE - 16)

/**
 * This define is used to specify that a DMA channel is NOT tied to any PaRAM
 * Set and hence any available PaRAM Set could be used for that DMA channel.
 * It could be used in dmaChannelPaRAMMap [EDMA3_MAX_DMA_CH], in global
 * configuration structure EDMA3_RM_GblConfigParams.
 *
 * This value should mandatorily be used to mark DMA channels with no initial
 * mapping to specific PaRAM Sets.
 */
#define EDMA3_DRV_CH_NO_PARAM_MAP (EDMA3_RM_CH_NO_PARAM_MAP)

/**
 * This define is used to specify that the DMA/QDMA channel is not tied to any
 * TCC and hence any available TCC could be used for that DMA/QDMA channel.
 * It could be used in dmaChannelTccMap [EDMA3_MAX_DMA_CH], in global
 * configuration structure EDMA3_RM_GblConfigParams.
 *
 * This value should mandatorily be used to mark DMA channels with no initial
 * mapping to specific TCCs.
 */
#define EDMA3_DRV_CH_NO_TCC_MAP (EDMA3_RM_CH_NO_TCC_MAP)

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_DATASTRUCT
 @{ */

    /**\struct  EDMA3_DRV_GblConfigParams
 * \brief   Init-time Configuration structure for EDMA3
 * controller, to provide Global SoC specific Information.
 *
 * This configuration structure is used to specify the EDMA3 Driver
 * global settings, specific to the SoC. For e.g. number of DMA/QDMA channels,
 * number of PaRAM sets, TCCs, event queues, transfer controllers, base
 * addresses of CC global registers and TC registers, interrupt number for
 * EDMA3 transfer completion, CC error, event queues' priority, watermark
 * threshold level etc.
 * This configuration information is SoC specific and could be provided by the
 * user at run-time while creating the EDMA3 Driver Object, using API
 * EDMA3_DRV_create. In case user doesn't provide it, this information could be
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
    } EDMA3_DRV_GblConfigParams;

    /**\struct  EDMA3_DRV_InstanceInitConfig
 * \brief   Init-time Region Specific Configuration structure for
 * EDMA3 Driver, to provide region specific Information.
 *
 * This configuration structure is used to specify which EDMA3 resources are
 * owned and reserved by the EDMA3 Driver instance. This configuration
 * structure is shadow region specific and will be provided by the user at
 * run-time while calling EDMA3_DRV_open ().
 *
 * Owned resources:
 * ****************
 *
 * EDMA3 Driver Instances are tied to different shadow regions and hence different
 * masters. Regions could be:
 *
 * a) ARM,
 * b) DSP,
 * c) IMCOP (Imaging Co-processor) etc.
 *
 * User can assign each EDMA3 resource to a shadow region using this structure.
 * In this way, user specifies which resources are owned by the specific EDMA3
 * DRV Instance.
 * This assignment should also ensure that the same resource is not assigned
 * to more than one shadow regions (unless desired in that way). Any assignment
 * not following the above mentioned approach may have catastrophic
 * consequences.
 *
 *
 * Reserved resources:
 * *******************
 *
 * During EDMA3 DRV initialization, user can reserve some of the EDMA3 resources
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
 * for a particular DMA channel), EDMA3 Driver automatically reserves all those
 * PaRAM Sets which are tied to the DMA channels. Then those PaRAM Sets would
 * not be returned when user requests for ANY PaRAM Set (specifically for
 * linking purpose). This is done in order to avoid allocating the PaRAM Set,
 * tied to a particular DMA channel, for linking purpose. If this constraint is
 * not there, that DMA channel thus could not be used at all, because of the
 * unavailability of the desired PaRAM Set.
 */
    typedef struct
    {
        /** PaRAM Sets owned by the EDMA3 Driver Instance. */
        uint32_t ownPaRAMSets[EDMA3_MAX_PARAM_DWRDS];

        /** DMA Channels owned by the EDMA3 Driver Instance. */
        uint32_t ownDmaChannels[EDMA3_MAX_DMA_CHAN_DWRDS];

        /** QDMA Channels owned by the EDMA3 Driver Instance. */
        uint32_t ownQdmaChannels[EDMA3_MAX_QDMA_CHAN_DWRDS];

        /** TCCs owned by the EDMA3 Driver Instance. */
        uint32_t ownTccs[EDMA3_MAX_TCC_DWRDS];

        /**
     * \brief   Reserved PaRAM Sets
     *
     * PaRAM Sets reserved during initialization for future use. These will not
     * be given when user requests for ANY available PaRAM Set for linking
     * using 'EDMA3_DRV_LINK_CHANNEL' as channel id.
     */
        uint32_t resvdPaRAMSets[EDMA3_MAX_PARAM_DWRDS];

        /**
     * \brief   Reserved DMA channels
     *
     * DMA channels reserved during initialization for future use. These will
     * not be given when user requests for ANY available DMA channel using
     * 'EDMA3_DRV_DMA_CHANNEL_ANY' as channel id.
     */
        uint32_t resvdDmaChannels[EDMA3_MAX_DMA_CHAN_DWRDS];

        /**
     * \brief   Reserved QDMA channels
     *
     * QDMA channels reserved during initialization for future use. These will
     * not be given when user requests for ANY available QDMA channel using
     * 'EDMA3_DRV_QDMA_CHANNEL_ANY' as channel id.
     */
        uint32_t resvdQdmaChannels[EDMA3_MAX_QDMA_CHAN_DWRDS];

        /**
     * \brief   Reserved TCCs
     *
     * TCCs reserved during initialization for future use. These will not
     * be given when user requests for ANY available TCC using
     * 'EDMA3_DRV_TCC_ANY' as resource id.
     */
        uint32_t resvdTccs[EDMA3_MAX_TCC_DWRDS];
    } EDMA3_DRV_InstanceInitConfig;

    /**\struct      EDMA3_DRV_InitConfig
 * \brief       Used to Initialize the EDMA3 Driver Instance
 *
 * This configuration structure is used to initialize the EDMA3 DRV Instance.
 * This configuration information is passed while opening the DRV instance.
 */
    typedef struct
    {
        /** Region Identification */
        EDMA3_RM_RegionId regionId;

        /**
     * It tells whether the EDMA3 DRV instance is Master or not. Only the shadow
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
        EDMA3_DRV_InstanceInitConfig *drvInstInitConfig;

        /**
     * EDMA3 Driver Instance specific semaphore handle.
     * Used to share resources (DMA/QDMA channels, PaRAM Sets, TCCs etc)
     * among different users.
     */
        void *drvSemHandle;

        /**
     * Instance wide global callback function to catch non-channel
     * specific errors from the Channel controller. for eg, TCC
     * error, queue threshold exceed error etc.
     */
        EDMA3_RM_GblErrCallback gblerrCb;

        /**
     * Application data to be passed back to the global error callback
     * function
     */
        void *gblerrData;
    } EDMA3_DRV_InitConfig;

    /**\struct      EDMA3_DRV_MiscParam
 * \brief       Used to specify the miscellaneous options during EDMA3 Driver
 * Initialization.
 *
 * This configuration structure is used to specify some misc options
 * while creating the Driver object. New options may also be added into this
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
    } EDMA3_DRV_MiscParam;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_FUNCTION_INIT
 @{ */

    /**
 * \brief   Create EDMA3 Driver Object
 *
 * This API is used to create the EDMA3 Driver Object. It should be
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
 * After successful completion of this API, Driver Object's state
 * changes to EDMA3_DRV_CREATED from EDMA3_DRV_DELETED.
 *
 * \param phyCtrllerInstId  [IN]    EDMA3 Controller Instance Id
 *                                 (Hardware instance id, starting from 0).
 * \param gblCfgParams      [IN]    SoC specific configuration structure for the
 *                                  EDMA3 Hardware.
 * \param miscParam         [IN]    Misc configuration options provided in the
 *                                  structure 'EDMA3_DRV_MiscParam'.
 *                                  For default options, user can pass NULL
 *                                  in this argument.
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error code
 */
    EDMA3_DRV_Result EDMA3_DRV_create(uint32_t phyCtrllerInstId,
                                      const EDMA3_DRV_GblConfigParams *gblCfgParams,
                                      const void *miscParam);
    /**
 * \brief Delete EDMA3 Driver Object
 *
 * Use this API to delete the EDMA3 Driver Object. It should be called only
 * ONCE for each EDMA3 hardware instance. It should be called ONLY after
 * closing all the EDMA3 Driver Instances.
 *
 * This API is used to delete the EDMA3 Driver Object. It should be called
 * once for each EDMA3 hardware instance, ONLY after closing all the
 * previously opened EDMA3 Driver Instances.
 *
 * After successful completion of this API, Driver Object's state
 * changes to EDMA3_DRV_DELETED.
 *
 * \param phyCtrllerInstId  [IN]    EDMA3 Phy Controller Instance Id (Hardware
 *                                  instance id, starting from 0).
 * \param param             [IN]    For possible future use.
 *
 * \return EDMA3_DRV_SOK or EDMA3_DRV Error code
 */
    EDMA3_DRV_Result EDMA3_DRV_delete(uint32_t phyCtrllerInstId,
                                      const void *param);

    /**
 * \brief   Open EDMA3 Driver Instance
 *
 * This API is used to open an EDMA3 Driver Instance. It could be
 * called multiple times, for each possible EDMA3 shadow region. Maximum
 * EDMA3_MAX_REGIONS instances are allowed for each EDMA3 hardware
 * instance. Multiple instances on the same shadow region are NOT allowed.
 *
 * Also, only ONE Master Driver Instance is permitted. This master
 * instance (and hence the region to which it belongs) will only receive the
 * EDMA3 interrupts, if enabled.
 *
 * User could pass the instance specific configuration structure
 * (initCfg.drvInstInitConfig) as a part of the 'initCfg' structure,
 * during init-time. In case user doesn't provide it, this information could
 * be taken from the SoC specific configuration file edma3_<SOC_NAME>_cfg.c,
 * in case it is available.
 *
 * By default, this EDMA3 Driver instance will clear the PaRAM Sets while
 * allocating them. To change the default behavior, user should use the IOCTL
 * interface appropriately.
 *
 * \param   phyCtrllerInstId    [IN]    EDMA3 Controller Instance Id (Hardware
 *                                      instance id, starting from 0).
 * \param   initCfg             [IN]    Used to Initialize the EDMA3 Driver
 *                                      Instance (Master or Slave).
 * \param   errorCode           [OUT]   Error code while opening DRV instance.
 *
 * \return EDMA3_DRV_Handle : If successfully opened, the API will return the
 *                            associated driver's instance handle.
 *
 * \note    This function disables the global interrupts (by calling API
 *          edma3OsProtectEntry with protection level
 *          EDMA3_OS_PROTECT_INTERRUPT) while modifying the global data
 *          structures, to make it re-entrant.
 */
    EDMA3_DRV_Handle EDMA3_DRV_open(uint32_t phyCtrllerInstId,
                                    const EDMA3_DRV_InitConfig *initCfg,
                                    EDMA3_DRV_Result *errorCode);

    /**
 * \brief Close the EDMA3 Driver Instance.
 *
 * This API is used to close a previously opened EDMA3 Driver Instance.
 *
 * \param  hEdma            [IN]    Handle to the previously opened EDMA3
 *                                  Driver Instance.
 * \param param             [IN]    For possible future use
 *
 * \return EDMA3_DRV_SOK or EDMA3_DRV Error code
 *
 * \note    This function disables the global interrupts (by calling API
 *          edma3OsProtectEntry with protection level
 *          EDMA3_OS_PROTECT_INTERRUPT) while modifying the global data
 *          structures, to make it re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_close(EDMA3_DRV_Handle hEdma,
                                     const void *param);

/**
@}
*/

/** @addtogroup EDMA3_LLD_DRV_SYMBOL_DEFINE
 @{ */

/* Defines for Logical Channel Values */
/*---------------------------------------------------------------------------*/
/**
 * Used to specify any available DMA Channel while requesting
 * one. Used in the API EDMA3_DRV_requestChannel().
 * DMA channel from the pool of (owned && non_reserved && available_right_now)
 * DMA channels will be chosen and returned.
 */
#define EDMA3_DRV_DMA_CHANNEL_ANY 1002U

/**
 * Used to specify any available QDMA Channel while requesting
 * one. Used in the API EDMA3_DRV_requestChannel().
 * QDMA channel from the pool of (owned && non_reserved && available_right_now)
 * QDMA channels will be chosen and returned.
 */
#define EDMA3_DRV_QDMA_CHANNEL_ANY 1003U

/**
 * Used to specify any available TCC while requesting
 * one. Used in the API EDMA3_DRV_requestChannel(), for
 * both DMA and QDMA channels.
 * TCC from the pool of (owned && non_reserved && available_right_now)
 * TCCs will be chosen and returned.
 */
#define EDMA3_DRV_TCC_ANY 1004U

/**
 * Used to specify any available PaRAM Set while requesting
 * one. Used in the API EDMA3_DRV_requestChannel(), for Link channels.
 * PaRAM Set from the pool of (owned && non_reserved && available_right_now)
 * PaRAM Sets will be chosen and returned.
 */
#define EDMA3_DRV_LINK_CHANNEL 1005U

/**
 * Used to specify any available PaRAM Set while requesting one. Used in the
 * API EDMA3_DRV_requestChannel(), for Link channels.
 * TCC code should also be specified and it will be used to populate the LINK
 * field of the PaRAM Set. Without TCC code, the call will fail.
 * PaRAM Set from the pool of (owned && non_reserved && available_right_now)
 * PaRAM Sets will be chosen and returned.
 */
#define EDMA3_DRV_LINK_CHANNEL_WITH_TCC 1006U

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_SYMBOL_ENUM
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
 * #define EDMA3_DRV_HW_CHANNEL_MCBSP_TX        EDMA3_DRV_HW_CHANNEL_EVENT_2
 * #define EDMA3_DRV_HW_CHANNEL_MCBSP_RX        EDMA3_DRV_HW_CHANNEL_EVENT_3
 *
 * These defines will be used by the MCBSP driver. The same event
 * EDMA3_DRV_HW_CHANNEL_EVENT_2/3 could be mapped to some other
 * peripheral also.
 */
    typedef enum
    {
        /** Channel assigned to EDMA3 Event 0 */
        EDMA3_DRV_HW_CHANNEL_EVENT_0 = 0,
        /** Channel assigned to EDMA3 Event 1 */
        EDMA3_DRV_HW_CHANNEL_EVENT_1,
        /** Channel assigned to EDMA3 Event 2 */
        EDMA3_DRV_HW_CHANNEL_EVENT_2,
        /** Channel assigned to EDMA3 Event 3 */
        EDMA3_DRV_HW_CHANNEL_EVENT_3,
        /** Channel assigned to EDMA3 Event 4 */
        EDMA3_DRV_HW_CHANNEL_EVENT_4,
        /** Channel assigned to EDMA3 Event 5 */
        EDMA3_DRV_HW_CHANNEL_EVENT_5,
        /** Channel assigned to EDMA3 Event 6 */
        EDMA3_DRV_HW_CHANNEL_EVENT_6,
        /** Channel assigned to EDMA3 Event 7 */
        EDMA3_DRV_HW_CHANNEL_EVENT_7,
        /** Channel assigned to EDMA3 Event 8 */
        EDMA3_DRV_HW_CHANNEL_EVENT_8,
        /** Channel assigned to EDMA3 Event 9 */
        EDMA3_DRV_HW_CHANNEL_EVENT_9,
        /** Channel assigned to EDMA3 Event 10 */
        EDMA3_DRV_HW_CHANNEL_EVENT_10,
        /** Channel assigned to EDMA3 Event 11 */
        EDMA3_DRV_HW_CHANNEL_EVENT_11,
        /** Channel assigned to EDMA3 Event 12 */
        EDMA3_DRV_HW_CHANNEL_EVENT_12,
        /** Channel assigned to EDMA3 Event 13 */
        EDMA3_DRV_HW_CHANNEL_EVENT_13,
        /** Channel assigned to EDMA3 Event 14 */
        EDMA3_DRV_HW_CHANNEL_EVENT_14,
        /** Channel assigned to EDMA3 Event 15 */
        EDMA3_DRV_HW_CHANNEL_EVENT_15,
        /** Channel assigned to EDMA3 Event 16 */
        EDMA3_DRV_HW_CHANNEL_EVENT_16,
        /** Channel assigned to EDMA3 Event 17 */
        EDMA3_DRV_HW_CHANNEL_EVENT_17,
        /** Channel assigned to EDMA3 Event 18 */
        EDMA3_DRV_HW_CHANNEL_EVENT_18,
        /** Channel assigned to EDMA3 Event 19 */
        EDMA3_DRV_HW_CHANNEL_EVENT_19,
        /** Channel assigned to EDMA3 Event 20 */
        EDMA3_DRV_HW_CHANNEL_EVENT_20,
        /** Channel assigned to EDMA3 Event 21 */
        EDMA3_DRV_HW_CHANNEL_EVENT_21,
        /** Channel assigned to EDMA3 Event 22 */
        EDMA3_DRV_HW_CHANNEL_EVENT_22,
        /** Channel assigned to EDMA3 Event 23 */
        EDMA3_DRV_HW_CHANNEL_EVENT_23,
        /** Channel assigned to EDMA3 Event 24 */
        EDMA3_DRV_HW_CHANNEL_EVENT_24,
        /** Channel assigned to EDMA3 Event 25 */
        EDMA3_DRV_HW_CHANNEL_EVENT_25,
        /** Channel assigned to EDMA3 Event 26 */
        EDMA3_DRV_HW_CHANNEL_EVENT_26,
        /** Channel assigned to EDMA3 Event 27 */
        EDMA3_DRV_HW_CHANNEL_EVENT_27,
        /** Channel assigned to EDMA3 Event 28 */
        EDMA3_DRV_HW_CHANNEL_EVENT_28,
        /** Channel assigned to EDMA3 Event 29 */
        EDMA3_DRV_HW_CHANNEL_EVENT_29,
        /** Channel assigned to EDMA3 Event 30 */
        EDMA3_DRV_HW_CHANNEL_EVENT_30,
        /** Channel assigned to EDMA3 Event 31 */
        EDMA3_DRV_HW_CHANNEL_EVENT_31,
        /** Channel assigned to EDMA3 Event 32 */
        EDMA3_DRV_HW_CHANNEL_EVENT_32,
        /** Channel assigned to EDMA3 Event 33 */
        EDMA3_DRV_HW_CHANNEL_EVENT_33,
        /** Channel assigned to EDMA3 Event 34 */
        EDMA3_DRV_HW_CHANNEL_EVENT_34,
        /** Channel assigned to EDMA3 Event 35 */
        EDMA3_DRV_HW_CHANNEL_EVENT_35,
        /** Channel assigned to EDMA3 Event 36 */
        EDMA3_DRV_HW_CHANNEL_EVENT_36,
        /** Channel assigned to EDMA3 Event 37 */
        EDMA3_DRV_HW_CHANNEL_EVENT_37,
        /** Channel assigned to EDMA3 Event 38 */
        EDMA3_DRV_HW_CHANNEL_EVENT_38,
        /** Channel assigned to EDMA3 Event 39 */
        EDMA3_DRV_HW_CHANNEL_EVENT_39,
        /** Channel assigned to EDMA3 Event 40 */
        EDMA3_DRV_HW_CHANNEL_EVENT_40,
        /** Channel assigned to EDMA3 Event 41 */
        EDMA3_DRV_HW_CHANNEL_EVENT_41,
        /** Channel assigned to EDMA3 Event 42 */
        EDMA3_DRV_HW_CHANNEL_EVENT_42,
        /** Channel assigned to EDMA3 Event 43 */
        EDMA3_DRV_HW_CHANNEL_EVENT_43,
        /** Channel assigned to EDMA3 Event 44 */
        EDMA3_DRV_HW_CHANNEL_EVENT_44,
        /** Channel assigned to EDMA3 Event 45 */
        EDMA3_DRV_HW_CHANNEL_EVENT_45,
        /** Channel assigned to EDMA3 Event 46 */
        EDMA3_DRV_HW_CHANNEL_EVENT_46,
        /** Channel assigned to EDMA3 Event 47 */
        EDMA3_DRV_HW_CHANNEL_EVENT_47,
        /** Channel assigned to EDMA3 Event 48 */
        EDMA3_DRV_HW_CHANNEL_EVENT_48,
        /** Channel assigned to EDMA3 Event 49 */
        EDMA3_DRV_HW_CHANNEL_EVENT_49,
        /** Channel assigned to EDMA3 Event 50 */
        EDMA3_DRV_HW_CHANNEL_EVENT_50,
        /** Channel assigned to EDMA3 Event 51 */
        EDMA3_DRV_HW_CHANNEL_EVENT_51,
        /** Channel assigned to EDMA3 Event 52 */
        EDMA3_DRV_HW_CHANNEL_EVENT_52,
        /** Channel assigned to EDMA3 Event 53 */
        EDMA3_DRV_HW_CHANNEL_EVENT_53,
        /** Channel assigned to EDMA3 Event 54 */
        EDMA3_DRV_HW_CHANNEL_EVENT_54,
        /** Channel assigned to EDMA3 Event 55 */
        EDMA3_DRV_HW_CHANNEL_EVENT_55,
        /** Channel assigned to EDMA3 Event 56 */
        EDMA3_DRV_HW_CHANNEL_EVENT_56,
        /** Channel assigned to EDMA3 Event 57 */
        EDMA3_DRV_HW_CHANNEL_EVENT_57,
        /** Channel assigned to EDMA3 Event 58 */
        EDMA3_DRV_HW_CHANNEL_EVENT_58,
        /** Channel assigned to EDMA3 Event 59 */
        EDMA3_DRV_HW_CHANNEL_EVENT_59,
        /** Channel assigned to EDMA3 Event 60 */
        EDMA3_DRV_HW_CHANNEL_EVENT_60,
        /** Channel assigned to EDMA3 Event 61 */
        EDMA3_DRV_HW_CHANNEL_EVENT_61,
        /** Channel assigned to EDMA3 Event 62 */
        EDMA3_DRV_HW_CHANNEL_EVENT_62,
        /** Channel assigned to EDMA3 Event 63 */
        EDMA3_DRV_HW_CHANNEL_EVENT_63
    } EDMA3_DRV_HW_CHANNEL_EVENT;

/**
@}
*/

/** @addtogroup EDMA3_LLD_DRV_SYMBOL_DEFINE
 @{ */

/**
 * \brief QDMA Channel defines
 *
 * They should be used while requesting a specific QDMA channel in API
 * EDMA3_DRV_requestChannel() as the argument (*pLch). Please note that
 * these defines should ONLY be used in the API EDMA3_DRV_requestChannel() and
 * not in any other API to perform further operations. They are only provided
 * to allow user allocate specific QDMA channels.
 */
/** QDMA Channel 0 */
#define EDMA3_DRV_QDMA_CHANNEL_0 (EDMA3_MAX_DMA_CH + EDMA3_MAX_PARAM_SETS)
/** QDMA Channel 1 */
#define EDMA3_DRV_QDMA_CHANNEL_1 (EDMA3_DRV_QDMA_CHANNEL_0 + 1U)
/** QDMA Channel 2 */
#define EDMA3_DRV_QDMA_CHANNEL_2 (EDMA3_DRV_QDMA_CHANNEL_0 + 2U)
/** QDMA Channel 3 */
#define EDMA3_DRV_QDMA_CHANNEL_3 (EDMA3_DRV_QDMA_CHANNEL_0 + 3U)
/** QDMA Channel 4 */
#define EDMA3_DRV_QDMA_CHANNEL_4 (EDMA3_DRV_QDMA_CHANNEL_0 + 4U)
/** QDMA Channel 5 */
#define EDMA3_DRV_QDMA_CHANNEL_5 (EDMA3_DRV_QDMA_CHANNEL_0 + 5U)
/** QDMA Channel 6 */
#define EDMA3_DRV_QDMA_CHANNEL_6 (EDMA3_DRV_QDMA_CHANNEL_0 + 6U)
/** QDMA Channel 7 */
#define EDMA3_DRV_QDMA_CHANNEL_7 (EDMA3_DRV_QDMA_CHANNEL_0 + 7U)

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_FUNCTION_BASIC
 @{ */

    /**
 *  \brief Request a DMA/QDMA/Link channel.
 *
 *  Each channel (DMA/QDMA/Link) must be requested  before initiating a DMA
 *  transfer on that channel.
 *
 * This API is used to allocate a logical channel (DMA/QDMA/Link) along with
 * the associated resources. For DMA and QDMA channels, TCC and PaRAM Set are
 * also allocated along with the requested channel. User can also specify a
 * specific TCC which needs to be allocated with the DMA/QDMA channel or else
 * can request any available TCC.
 *
 * For Link channels, ONLY a PaRAM Set is allocated and the allocated PaRAM Set
 * number is returned as the logical channel no. A TCC code can also be
 * specified while making the request. This TCC code will be copied to the
 * LINK field of the allocated PaRAM Set and will be associated with the Link
 * channel.
 *
 * User can request a specific logical channel - DMA, QDMA and Link, by passing
 * the channel id in 'pLCh'. Note that the channel id is the same as the actual
 * resource id in case of DMA channels and Link channels. For DMA channels,
 * channel id lies between 0 and (max dma channels - 1). For Link channels,
 * channel id lies between (max dma channels) and (max param sets - 1). To
 * allocate specific QDMA channels, user SHOULD use the defines
 * EDMA3_DRV_QDMA_CHANNEL_X mentioned above.
 *
 * User can also request ANY available logical channel by specifying the
 * below mentioned values in '*pLCh':
 *  a)  EDMA3_DRV_DMA_CHANNEL_ANY: For DMA channels
 *  b)  EDMA3_DRV_QDMA_CHANNEL_ANY: For QDMA channels, and
 *  c)  EDMA3_DRV_LINK_CHANNEL: For Link channels. Normally user should use this
 *      value to request link channels (PaRAM Sets used for linking purpose
 *      only), unless he wants to use some specific link channels (PaRAM Sets)
 *      which is also allowed.
 *  d)  EDMA3_DRV_LINK_CHANNEL_WITH_TCC: For Link channels. User should
 *      use this value to request link channels with TCC code.
 *
 * This API internally uses EDMA3_RM_allocResource () to allocate the desired
 * resources (DMA/QDMA channel, PaRAM Set and TCC).
 *
 * This API also registers a specific callback function, in case the same is
 * provided, against the allocated TCC. To do this, this API calls
 * EDMA3_RM_registerTccCb(), which is a part of the Resource Manager. Please
 * note that the interrupts are enabled for the specific TCC only if callback
 * function is provided. In the absence of this, the API assumes that the
 * requested logical channel is going to be used in Poll mode environment.
 *
 * For DMA/QDMA channels, after allocating all the EDMA3 resources, this API
 * sets the TCC field of the OPT PaRAM Word with the allocated TCC. It also sets
 * the event queue for the channel allocated. The event queue needs to be
 * specified by the user.
 *
 * For DMA channel, it also sets the DCHMAP register, if required.
 *
 * For QDMA channel, it sets the QCHMAP register and CCNT as trigger word and
 * enables the QDMA channel by writing to the QEESR register.
 *
 *  \param  hEdma           [IN]        Handle to the previously opened Driver
 *                                      Instance.
 *  \param  pLCh            [IN/OUT]    Requested logical channel id.
 *                                      Examples:
 *                                      - EDMA3_DRV_HW_CHANNEL_EVENT_0
 *                                      - To request a DMA Master Channel
 *                                        mapped to EDMA Event 0.
 *
 *                                      - EDMA3_DRV_DMA_CHANNEL_ANY
 *                                      - For requesting any DMA Master channel
 *                                        with no event mapping.
 *
 *                                      - EDMA3_DRV_QDMA_CHANNEL_ANY
 *                                      - For requesting any QDMA Master channel
 *
 *                                      - EDMA3_DRV_QDMA_CHANNEL_0
 *                                      - For requesting the QDMA Channel 0.
 *
 *                                      - EDMA3_DRV_LINK_CHANNEL
 *                                      - For requesting a DMA Slave Channel,
 *                                      - to be linked to some other Master
 *                                      - channel.
 *
 *                                      - EDMA3_DRV_LINK_CHANNEL_WITH_TCC
 *                                      - For requesting a DMA Slave Channel,
 *                                      - to be linked to some other Master
 *                                      - channel, with a TCC associated with it.
 *
 *                                      In case user passes a specific channel
 *                                      Id, pLCh value is left unchanged. In
 *                                      case user requests ANY available
 *                                      resource, the allocated channel id is
 *                                      returned in pLCh.
 *
 *  \note   To request  a PaRAM Set for the purpose of
 *          linking to another channel,  call the function with
 *
 *          *pLCh = EDMA3_DRV_LINK_CHANNEL or EDMA3_DRV_LINK_CHANNEL_WITH_TCC
 *
 *          This function will update *pLCh with the allocated Link channel
 *          handle.
 *
 *  \param  pTcc             [IN/OUT]   The channel number on which the
 *                                      completion/error interrupt is generated.
 *                                      Not used if user requested for a Link
 *                                      channel.
 *                                      Examples:
 *                                      - EDMA3_DRV_HW_CHANNEL_EVENT_0
 *                                      - To request TCC associated with
 *                                      - DMA Master Channel mapped to EDMA
 *                                      - event 0.
 *
 *                                      - EDMA3_DRV_TCC_ANY
 *                                      - For requesting any TCC with no
 *                                      - channel mapping.
 *                                      In case user passes a specific TCC
 *                                      value, pTcc value is left unchanged.
 *                                      In case user requests ANY available TCC,
 *                                      the allocated one is returned in pTcc
 *
 *  \param  evtQueue         [IN]       Event Queue Number to which the channel
 *                                      will be mapped (valid only for the
 *                                      Master Channel (DMA/QDMA) request)
 *
 *  \param tccCb             [IN]       TCC callback - caters to channel-
 *                                      specific events like "Event Miss Error"
 *                                      or "Transfer Complete"
 *
 *  \param cbData            [IN]       Data which will be passed directly to
 *                                      the tccCb callback function
 *
 *  \return EDMA3_DRV_SOK or EDMA3_DRV Error code
 *
 *  \note    This function internally uses EDMA3 Resource Manager, which
 *           acquires a RM Instance specific semaphore
 *           to prevent simultaneous access to the global pool of resources.
 *           It also disables the global interrupts while modifying
 *           the global CC registers.
 *           It is re-entrant, but SHOULD NOT be called from the user callback
 *           function (ISR context).
 */
    EDMA3_DRV_Result EDMA3_DRV_requestChannel(EDMA3_DRV_Handle hEdma,
                                              uint32_t *pLCh,
                                              uint32_t *pTcc,
                                              EDMA3_RM_EventQueue evtQueue,
                                              EDMA3_RM_TccCallback tccCb,
                                              void *cbData);

    /**
 * \brief Free the specified channel (DMA/QDMA/Link) and its associated
 * resources (PaRAM Set, TCC etc) and removes various mappings.
 *
 * This API internally uses EDMA3_RM_freeResource () to free the desired
 * resources.
 *
 * For Link channels, this API only frees the associated PaRAM Set.
 *
 * For DMA/QDMA channels, it does the following operations:
 * a) Disable any ongoing transfer on the channel,
 * b) Unregister the TCC Callback function and disable the interrupts,
 * c) Remove the channel to Event Queue mapping,
 * d) For DMA channels, clear the DCHMAP register, if available
 * e) For QDMA channels, clear the QCHMAP register,
 * f) Frees the DMA/QDMA channel in the end.
 *
 *  \param  hEdma            [IN]     Handle to the EDMA Driver Instance.
 *  \param  channelId        [IN]     Logical Channel number to be freed.
 *
 *  \return EDMA3_DRV_SOK or EDMA3_DRV Error code
 *
 *  \note    This function disables the global interrupts while modifying
 *           the global CC registers and while modifying global data structures,
 *           to prevent simultaneous access to the global pool of resources.
 *           It internally calls EDMA3_RM_freeResource () for resource
 *           de-allocation. It is re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_freeChannel(EDMA3_DRV_Handle hEdma,
                                           uint32_t channelId);

    /**
 *  \brief  Disables the DMA Channel by clearing the Event Enable Register and
 *	clears Error Register & Secondary Event Register for a specific DMA channel.
 *
 *  This API clears the Event Enable register, Event Miss register and Secondary
 *	Event register for a specific DMA channel. It also clears the CC Error
 *	register.
 *
 *  \param  hEdma            [IN]     Handle to the EDMA Driver Instance.
 *  \param  channelId        [IN]     DMA Channel needs to be cleaned.
 *
 *  \return EDMA3_DRV_SOK or EDMA3_DRV Error code
 *
 *  \note    This function is re-entrant for unique channelId values. It is non-
 *          re-entrant for same channelId value.
 */
    EDMA3_DRV_Result EDMA3_DRV_clearErrorBits(EDMA3_DRV_Handle hEdma,
                                              uint32_t channelId);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_FUNCTION_ADVANCED
 @{ */

    /**
 * \brief  Link two logical channels.
 *
 * This API is used to link two previously allocated logical (DMA/QDMA/Link)
 * channels.
 *
 * It sets the Link field of the PaRAM set associated with first logical
 * channel (lCh1) to point it to the PaRAM set associated with second logical
 * channel (lCh2).
 *
 * It also sets the TCC field of PaRAM set of second logical channel to the
 * same as that of the first logical channel, only if the TCC field doesnot
 * contain a valid TCC code. In case the second logical channel has its own TCC,
 * the TCC field remains unchanged.
 *
 * \param   hEdma           [IN]    Handle to the EDMA Driver Instance.
 * \param   lCh1            [IN]    Logical Channel to which particular channel
 *                                  will be linked.
 * \param   lCh2            [IN]    Logical Channel which needs to be linked to
 *                                  the first channel.
 *                                  After the transfer based on the PaRAM set
 *                                  of lCh1 is over, the PaRAM set of lCh2 will
 *                                  be copied to the PaRAM set of lCh1 and
 *                                  transfer will resume.
 *                                  For DMA channels, another sync event is
 *                                  required to initiate the transfer on the
 *                                  Link channel.
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh1 & lCh2 values. It is
 *          non-re-entrant for same lCh1 & lCh2 values.
 */
    EDMA3_DRV_Result EDMA3_DRV_linkChannel(EDMA3_DRV_Handle hEdma,
                                           uint32_t lCh1,
                                           uint32_t lCh2);

    /**
 * \brief  Unlink the channel from the earlier linked logical channel.
 *
 *         This function breaks the link between the specified
 *         channel and the earlier linked logical channel
 *         by clearing the Link Address field.
 *
 * \param  hEdma             [IN]    Handle to the EDMA Driver Instance.
 * \param  lCh               [IN]    Channel for which linking has to be removed
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_unlinkChannel(EDMA3_DRV_Handle hEdma,
                                             uint32_t lCh);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_SYMBOL_ENUM
 @{ */

    /**
 * \brief OPT Field Offset.
 *
 * Use this enum to set or get any of the
 * Fields within an OPT of a Parameter RAM set.
 */
    typedef enum
    {
        /**
         * Source addressing mode (INCR / FIFO)
         * (Bit 0)
         */
        EDMA3_DRV_OPT_FIELD_SAM = 0,

        /**
         * Destination addressing mode (INCR / FIFO)
         * (Bit 1)
         */
        EDMA3_DRV_OPT_FIELD_DAM = 1,

        /**
         * Transfer synchronization dimension (A-synchronized / AB-synchronized)
         * (Bit 2)
         */
        EDMA3_DRV_OPT_FIELD_SYNCDIM = 2,

        /**
         * The STATIC field
         * PaRAM set is static/non-static?
         * (Bit 3)
         */
        EDMA3_DRV_OPT_FIELD_STATIC = 3,

        /**
         * FIFO Width. Applies if either SAM or DAM is set to FIFO mode.
         * (Bitfield 8-10)
         */
        EDMA3_DRV_OPT_FIELD_FWID = 4,

        /**
         * Transfer complete code mode. Indicates the point at which a
         * transfer is considered completed for chaining and interrupt
         * generation.
         * (Bit 11)
         */
        EDMA3_DRV_OPT_FIELD_TCCMODE = 5,

        /**
         * Transfer Complete Code (TCC).
         * This 6-bit code is used to set the relevant bit in chaining enable
         * register (CER[TCC]/CERH[TCC]) for chaining or in interrupt pending
         * register (IPR[TCC]/IPRH[TCC]) for interrupts.
         * (Bitfield 12-17)
         */
        EDMA3_DRV_OPT_FIELD_TCC = 6,

        /**
         * Transfer complete interrupt enable/disable.
         * (Bit 20)
         */
        EDMA3_DRV_OPT_FIELD_TCINTEN = 7,

        /**
         * Intermediate transfer complete interrupt enable/disable.
         * (Bit 21)
         */
        EDMA3_DRV_OPT_FIELD_ITCINTEN = 8,

        /**
         * Transfer complete chaining enable/disable
         * (Bit 22)
         */
        EDMA3_DRV_OPT_FIELD_TCCHEN = 9,

        /**
         * Intermediate transfer completion chaining enable/disable
         * (Bit 23)
         */
        EDMA3_DRV_OPT_FIELD_ITCCHEN = 10

    } EDMA3_DRV_OptField;

    /**
 * \brief EDMA Addressing modes
 *
 * The EDMA3 TC supports two addressing modes
 *           -# Increment transfer
 *           -# FIFO transfer
 *
 * The SAM (Source Addressing Mode) and the DAM (Destination Addressing Mode)
 * can be independently set to either of the two via the OPT register.
 *
 */
    typedef enum
    {
        /**
         * Increment (INCR) mode. Source addressing within an array increments.
         * Source is not a FIFO.
         */
        EDMA3_DRV_ADDR_MODE_INCR = 0,

        /**
         * FIFO mode. Source addressing within an array wraps around upon
         * reaching FIFO width.
         */
        EDMA3_DRV_ADDR_MODE_FIFO = 1

    } EDMA3_DRV_AddrMode;

    /**
 * \brief EDMA Transfer Synchronization type.
 *
 * Two types of Synchronization of transfers are possible
 *     -# A Synchronized
 *     -# AB Syncronized
 * - A Sync
 *     -# Each Array is submitted as one TR
 *     -# (BCNT*CCNT) number of sync events are needed to completely service
 *        a PaRAM set. (Where BCNT = Num of Arrays in a Frame;
 *                            CCNT = Num of Frames in a Block)
 *     -# (S/D)CIDX = (Addr of First array in next frame)
 *              minus (Addr of Last array in present frame)
 *     (Where CIDX is the Inter-Frame index)
 *
 * - AB Sync
 *     -# Each Frame is submitted as one TR
 *     -# Only CCNT number of sync events are needed to completely service
 *        a PaRAM set
 *     -# (S/D)CIDX = (Addr of First array in next frame)
 *              minus (Addr of First array of present frame)
 *
 * \note ABC sync transfers can be achieved logically by chaining multiple
 *       AB sync transfers
 *
 */
    typedef enum
    {
        /**
         * A-synchronized.
         * Each event triggers the transfer of a single array of ACNT bytes
         */
        EDMA3_DRV_SYNC_A = 0,

        /**
         * AB-synchronized.
         * Each event triggers the transfer of BCNT arrays of ACNT bytes
         */
        EDMA3_DRV_SYNC_AB = 1

    } EDMA3_DRV_SyncType;

    /**
 * \brief True/False: PaRAM set is Static or not. A Static PaRAM set
 * is updated or linked after TR is submitted.
 */
    typedef enum
    {
        /**
     * PaRAM set is not Static. PaRAM set is updated or linked
     * after TR is submitted. A value of 0 should be used for
     * DMA channels and for nonfinal transfers in a linked list
     * of QDMA transfers
     */
        EDMA3_DRV_STATIC_DIS = 0,

        /**
     * PaRAM set is Static. PaRAM set is not updated or linked
     * after TR is submitted. A value of 1 should be used for
     * isolated QDMA transfers or for the final transfer in a
     * linked list of QDMA transfers.
     */
        EDMA3_DRV_STATIC_EN = 1
    } EDMA3_DRV_StaticMode;

    /**
 * \brief EDMA3 FIFO width.
 *
 * The user can set the width of the FIFO using this enum.
 * This is done via the OPT register.
 * This is valid only if the EDMA3_DRV_ADDR_MODE_FIFO value is used for the
 * enum EDMA3_DRV_AddrMode.
 */
    typedef enum
    {
        /** FIFO width is 8-bit. */
        EDMA3_DRV_W8BIT = 0,

        /** FIFO width is 16-bit. */
        EDMA3_DRV_W16BIT = 1,

        /** FIFO width is 32-bit. */
        EDMA3_DRV_W32BIT = 2,

        /** FIFO width is 64-bit. */
        EDMA3_DRV_W64BIT = 3,

        /** FIFO width is 128-bit. */
        EDMA3_DRV_W128BIT = 4,

        /** FIFO width is 256-bit. */
        EDMA3_DRV_W256BIT = 5

    } EDMA3_DRV_FifoWidth;

    /**
 * \brief Transfer complete code mode.
 * Indicates the point at which a transfer is considered completed for
 * chaining and interrupt generation
 */
    typedef enum
    {
        /** A transfer is considered completed after transfer of data */
        EDMA3_DRV_TCCMODE_NORMAL = 0,

        /**
     * A transfer is considered completed after the EDMA3CC submits a TR
     * to the EDMA3TC. TC may still be transferring data when interrupt/chain
     * is triggered.
     */
        EDMA3_DRV_TCCMODE_EARLY = 1
    } EDMA3_DRV_TccMode;

    /**
 * \brief Transfer complete interrupt enable.
 */
    typedef enum
    {
        /** Transfer complete interrupt is disabled */
        EDMA3_DRV_TCINTEN_DIS = 0,

        /**
     * Transfer complete interrupt is enabled.
     * When enabled, the interrupt pending register (IPR/IPRH) bit is set on
     * transfer completion (upon completion of the final TR in the PaRAM set).
     * The bit (position) set in IPR or IPRH is the TCC value specified. In
     * order to generate a completion interrupt to the CPU, the corresponding
     * IER [TCC] / IERH [TCC] bit must be set to 1.
     */
        EDMA3_DRV_TCINTEN_EN = 1
    } EDMA3_DRV_TcintEn;

    /**
 * \brief Intermediate Transfer complete interrupt enable.
 */
    typedef enum
    {
        /** Intermediate Transfer complete interrupt is disabled */
        EDMA3_DRV_ITCINTEN_DIS = 0,

        /**
     * Intermediate transfer complete interrupt is enabled.
     * When enabled, the interrupt pending register (IPR/IPRH) bit is set on
     * every intermediate transfer completion (upon completion of every
     * intermediate TR in the PaRAM set, except the final TR in the PaRAM set).
     * The bit (position) set in IPR or IPRH is the TCC value specified. In
     * order to generate a completion interrupt to the CPU, the corresponding
     * IER [TCC] / IERH [TCC] bit must be set to 1.
     */
        EDMA3_DRV_ITCINTEN_EN = 1
    } EDMA3_DRV_ItcintEn;

    /**
 * \brief Transfer complete chaining enable.
 */
    typedef enum
    {
        /** Transfer complete chaining is disabled */
        EDMA3_DRV_TCCHEN_DIS = 0,

        /**
     * Transfer complete chaining is enabled.
     * When enabled, the chained event register (CER/CERH) bit is set on final
     * chained transfer completion (upon completion of the final / last TR in
     * the PaRAM set). The bit (position) set in CER or CERH is the TCC value
     * specified.
     */
        EDMA3_DRV_TCCHEN_EN = 1
    } EDMA3_DRV_TcchEn;

    /**
 * \brief Intermediate Transfer complete chaining enable.
 */
    typedef enum
    {
        /** Intermediate Transfer complete chaining is disabled */
        EDMA3_DRV_ITCCHEN_DIS = 0,

        /**
     * Intermediate transfer complete chaining is enabled.
     * When enabled, the chained event register (CER/CERH) bit is set on every
     * intermediate chained transfer completion (upon completion of every
     * intermediate TR in the PaRAM set, except the final TR in the PaRAM set).
     * The bit (position) set in CER or CERH is the TCC value specified.
     */
        EDMA3_DRV_ITCCHEN_EN = 1
    } EDMA3_DRV_ItcchEn;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_DATASTRUCT
 @{ */

    /**
 * \brief Structure to be used to configure interrupt generation
 * and chaining options.
 */
    typedef struct
    {
        /** Transfer complete chaining enable */
        EDMA3_DRV_TcchEn tcchEn;

        /** Intermediate Transfer complete chaining enable */
        EDMA3_DRV_ItcchEn itcchEn;

        /** Transfer complete interrupt enable */
        EDMA3_DRV_TcintEn tcintEn;

        /** Intermediate Transfer complete interrupt enable */
        EDMA3_DRV_ItcintEn itcintEn;
    } EDMA3_DRV_ChainOptions;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_FUNCTION_BASIC
 @{ */

    /**
 * \brief   Set a particular OPT field in the PaRAM set associated with the
 *          logical channel 'lCh'.
 *
 * This API can be used to set various optional parameters for an EDMA3
 * transfer. Like enable/disable completion interrupts, enable/disable chaining,
 * setting the transfer mode (A/AB Sync), setting the FIFO width etc.
 *
 * \param   hEdma               [IN]        Handle to the EDMA Driver Instance.
 * \param   lCh                 [IN]        Logical Channel, bound to which
 *                                          PaRAM set OPT field needs to be set.
 * \param   optField            [IN]        The particular field of OPT Word
 *                                          that needs setting
 * \param   newOptFieldVal      [IN]        The new OPT field value
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_setOptField(EDMA3_DRV_Handle hEdma,
                                           uint32_t lCh,
                                           EDMA3_DRV_OptField optField,
                                           uint32_t newOptFieldVal);

    /**
 * \brief   Get a particular OPT field in the PaRAM set associated with the
 *          logical channel 'lCh'.
 *
 * This API can be used to read various optional parameters for an EDMA3
 * transfer. Like enable/disable completion interrupts, enable/disable chaining,
 * setting the transfer mode (A/AB Sync), setting the FIFO width etc.
 *
 * \param   hEdma               [IN]        Handle to the EDMA Driver Instance.
 * \param   lCh                 [IN]        Logical Channel, bound to which
 *                                          PaRAM set OPT field is required.
 * \param   optField            [IN]        The particular field of OPT Word
 *                                          that is needed
 * \param   optFieldVal         [IN/OUT]    Value of the OPT field
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_getOptField(EDMA3_DRV_Handle hEdma,
                                           uint32_t lCh,
                                           EDMA3_DRV_OptField optField,
                                           uint32_t *optFieldVal);

    /**
 * \brief  DMA source parameters setup
 *
 * It is used to program the source address, source side addressing mode
 * (INCR or FIFO) and the FIFO width in case the addressing mode is FIFO.
 *
 * In FIFO Addressing mode, memory location must be 32 bytes aligned.
 *
 * \param   hEdma       [IN]    Handle to the EDMA Driver Instance
 * \param   lCh         [IN]    Logical Channel for which the source parameters
 *                              are to be configured
 * \param   srcAddr     [IN]    Source address
 * \param   addrMode    [IN]    Address mode [FIFO or Increment]
 * \param   fifoWidth   [IN]    Width of FIFO (Valid only if addrMode is FIFO)
 *                                  -# 0 - 8 bit
 *                                  -# 1 - 16 bit
 *                                  -# 2 - 32 bit
 *                                  -# 3 - 64 bit
 *                                  -# 4 - 128 bit
 *                                  -# 5 - 256 bit
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_setSrcParams(EDMA3_DRV_Handle hEdma,
                                            uint32_t lCh,
                                            uint32_t srcAddr,
                                            EDMA3_DRV_AddrMode addrMode,
                                            EDMA3_DRV_FifoWidth fifoWidth);

    /**
 * \brief  DMA Destination parameters setup
 *
 * It is used to program the destination address, destination side addressing
 * mode (INCR or FIFO) and the FIFO width in case the addressing mode is FIFO.
 *
 * In FIFO Addressing mode, memory location must be 32 bytes aligned.
 *
 * \param   hEdma       [IN]    Handle to the EDMA Driver Instance
 * \param   lCh         [IN]    Logical Channel for which the destination
 *                              parameters are to be configured
 * \param   destAddr    [IN]    Destination address
 * \param   addrMode    [IN]    Address mode [FIFO or Increment]
 * \param   fifoWidth   [IN]    Width of FIFO (Valid only if addrMode is FIFO)
 *                                  -# 0 - 8 bit
 *                                  -# 1 - 16 bit
 *                                  -# 2 - 32 bit
 *                                  -# 3 - 64 bit
 *                                  -# 4 - 128 bit
 *                                  -# 5 - 256 bit
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_setDestParams(EDMA3_DRV_Handle hEdma,
                                             uint32_t lCh,
                                             uint32_t destAddr,
                                             EDMA3_DRV_AddrMode addrMode,
                                             EDMA3_DRV_FifoWidth fifoWidth);

    /**
 * \brief   DMA source index setup
 *
 * It is used to program the source B index and source C index.
 *
 * SRCBIDX is a 16-bit signed value (2s complement) used for source address
 * modification between each array in the 2nd dimension. Valid values for
 * SRCBIDX are between -32768 and 32767. It provides a byte address offset
 * from the beginning of the source array to the beginning of the next source
 * array. It applies to both A-synchronized and AB-synchronized transfers.
 *
 * SRCCIDX is a 16-bit signed value (2s complement) used for source address
 * modification in the 3rd dimension. Valid values for SRCCIDX are between
 * -32768 and 32767. It provides a byte address offset from the beginning of
 * the current array (pointed to by SRC address) to the beginning of the first
 * source array in the next frame. It applies to both A-synchronized and
 * AB-synchronized transfers. Note that when SRCCIDX is applied, the current
 * array in an A-synchronized transfer is the last array in the frame, while
 * the current array in an AB-synchronized transfer is the first array in the
 * frame.
 *
 * \param   hEdma           [IN]            Handle to the EDMA Driver Instance
 * \param   lCh             [IN]            Logical Channel for which source
 *                                          indices are to be configured
 * \param   srcBIdx         [IN]            Source B index
 * \param   srcCIdx         [IN]            Source C index
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_setSrcIndex(EDMA3_DRV_Handle hEdma,
                                           uint32_t lCh,
                                           int32_t srcBIdx,
                                           int32_t srcCIdx);

    /**
 * \brief   DMA destination index setup
 *
 * It is used to program the destination B index and destination C index.
 *
 * DSTBIDX is a 16-bit signed value (2s complement) used for destination
 * address modification between each array in the 2nd dimension. Valid values
 * for DSTBIDX are between -32768 and 32767. It provides a byte address offset
 * from the beginning of the destination array to the beginning of the next
 * destination array within the current frame. It applies to both
 * A-synchronized and AB-synchronized transfers.
 *
 * DSTCIDX is a 16-bit signed value (2s complement) used for destination address
 * modification in the 3rd dimension. Valid values are between -32768 and 32767.
 * It provides a byte address offset from the beginning of the current array
 * (pointed to by DST address) to the beginning of the first destination array
 * TR in the next frame. It applies to both A-synchronized and AB-synchronized
 * transfers. Note that when DSTCIDX is applied, the current array in an
 * A-synchronized transfer is the last array in the frame, while the current
 * array in a AB-synchronized transfer is the first array in the frame
 *
 * \param   hEdma           [IN]            Handle to the EDMA Driver Instance
 * \param   lCh             [IN]            Logical Channel for which dest
 *                                          indices are to be configured
 * \param   destBIdx        [IN]            Destination B index
 * \param   destCIdx        [IN]            Destination C index
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_setDestIndex(EDMA3_DRV_Handle hEdma,
                                            uint32_t lCh,
                                            int32_t destBIdx,
                                            int32_t destCIdx);

    /**
 * \brief       DMA transfer parameters setup
 *
 * It is used to specify the various counts (ACNT, BCNT and CCNT), B count
 * reload and the synchronization type
 *
 * ACNT represents the number of bytes within the 1st dimension of a transfer.
 * ACNT is a 16-bit unsigned value with valid values between 0 and 65535.
 * Therefore, the maximum number of bytes in an array is 65535 bytes (64K - 1
 * bytes). ACNT must be greater than or equal to 1 for a TR to be submitted to
 * EDMA3 Transfer Controller.
 * An ACNT equal to 0 is considered either a null or dummy transfer. A dummy or
 * null transfer generates a completion code depending on the settings of the
 * completion bit fields in OPT.

 * BCNT is a 16-bit unsigned value that specifies the number of arrays of length
 * ACNT. For normal operation, valid values for BCNT are between 1 and 65535.
 * Therefore, the maximum number of arrays in a frame is 65535 (64K - 1 arrays).
 * A BCNT equal to 0 is considered either a null or dummy transfer. A dummy or
 * null transfer generates a completion code depending on the settings of the
 * completion bit fields in OPT.
 *
 * CCNT is a 16-bit unsigned value that specifies the number of frames in a
 * block. Valid values for CCNT are between 1 and 65535. Therefore, the maximum
 * number of frames in a block is 65535 (64K - 1 frames). A CCNT equal to 0 is
 * considered either a null or dummy transfer. A dummy or null transfer
 * generates a completion code depending on the settings of the completion bit
 * fields in OPT. A CCNT value of 0 is considered either a null or dummy
 * transfer.
 *
 * BCNTRLD is a 16-bit unsigned value used to reload the BCNT field once the
 * last array in the 2nd dimension is transferred. This field is only used for
 * A-synchronized transfers. In this case, the EDMA3CC decrements the BCNT
 * value by 1 on each TR submission. When BCNT (conceptually) reaches 0, the
 * EDMA3CC decrements CCNT and uses the BCNTRLD value to reinitialize the BCNT
 * value.
 * For AB-synchronized transfers, the EDMA3CC submits the BCNT in the TR and the
 * EDMA3TC decrements BCNT appropriately. For AB-synchronized transfers,
 * BCNTRLD is not used.

 * \param       hEdma           [IN]    Handle to the EDMA Driver Instance
 * \param       lCh             [IN]    Logical Channel for which transfer
 *                                      parameters are to be configured
 * \param       aCnt            [IN]    Count for 1st Dimension.
 * \param       bCnt            [IN]    Count for 2nd Dimension.
 * \param       cCnt            [IN]    Count for 3rd Dimension.
 * \param       bCntReload      [IN]    Reload value for bCnt.
 * \param       syncType        [IN]    Transfer synchronization dimension
 *                                      0: A-synchronized. Each event triggers
 *                                      the transfer of a single array of
 *                                      ACNT bytes.
 *                                      1: AB-synchronized. Each event triggers
 *                                      the transfer of BCNT arrays of ACNT
 *                                      bytes.
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_setTransferParams(
        EDMA3_DRV_Handle hEdma,
        uint32_t lCh,
        uint32_t aCnt,
        uint32_t bCnt,
        uint32_t cCnt,
        uint32_t bCntReload,
        EDMA3_DRV_SyncType syncType);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_FUNCTION_ADVANCED
 @{ */

    /**
 * \brief   Chain the two specified channels.
 *
 * This API is used to chain a DMA channel to a previously allocated DMA/QDMA
 * channel.
 *
 * Chaining is different from Linking. The EDMA3 link feature reloads the
 * current channel parameter set with the linked parameter set. The EDMA3
 * chaining feature does not modify or update any channel parameter set;
 * it provides a synchronization event (or trigger) to the chained DMA channel,
 * as soon as the transfer (final or intermediate) completes on the main
 * DMA/QDMA channel.
 *
 * \param   hEdma               [IN]    Handle to the EDMA Driver Instance.
 *
 * \param   lCh1                [IN]    DMA/QDMA channel to which a particular
 *                                      DMA channel will be chained.
 * \param   lCh2                [IN]    DMA channel which needs to be chained to
 *                                      the first DMA/QDMA channel.
 * \param   chainOptions        [IN]    Options such as intermediate interrupts
 *                                      are required or not, intermediate/final
 *                                      chaining is enabled or not etc.
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh1 & lCh2 values. It is
 *          non-re-entrant for same lCh1 & lCh2 values.
 */
    EDMA3_DRV_Result EDMA3_DRV_chainChannel(EDMA3_DRV_Handle hEdma,
                                            uint32_t lCh1,
                                            uint32_t lCh2,
                                            const EDMA3_DRV_ChainOptions *chainOptions);

    /**
 * \brief   Unchain the two channels.
 *
 * \param   hEdma               [IN]    Handle to the EDMA Driver Instance.
 * \param   lCh                 [IN]    Channel whose chaining with the other
 *                                      channel has to be removed.
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_unchainChannel(EDMA3_DRV_Handle hEdma,
                                              uint32_t lCh);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_SYMBOL_ENUM
 @{ */

    /**
 * \brief EDMA Trigger Mode Selection
 *
 * Use this enum to select the EDMA trigger mode while enabling
 * the EDMA transfer
 */
    typedef enum
    {
        /**
         * Set the Trigger mode to Manual .
         * The CPU manually triggers a transfer by writing a 1 to the
         * corresponding bit in the event set register (ESR/ESRH).
         */
        EDMA3_DRV_TRIG_MODE_MANUAL = 0,

        /**
         * Set the Trigger mode to QDMA.
         * A QDMA transfer is triggered when a CPU (or other EDMA3
         * programmer) writes to the trigger word of the
         * QDMA channel parameter set (autotriggered) or when the
         * EDMA3CC performs a link update on a PaRAM
         * set that has been mapped to a QDMA channel (link triggered).
         */
        EDMA3_DRV_TRIG_MODE_QDMA = 1,

        /**
         * Set the Trigger mode to Event.
         * Allows for a peripheral, system, or externally-generated
         * event to trigger a transfer request.
         */
        EDMA3_DRV_TRIG_MODE_EVENT = 2,

        /** Used to specify the trigger mode NONE */
        EDMA3_DRV_TRIG_MODE_NONE = 3
    } EDMA3_DRV_TrigMode;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_FUNCTION_BASIC
 @{ */

    /**
 * \brief  Version information 
 *
 * The function is used to get the version information of the EDMA LLD.
 *
 * \return      Version Information.
 */
    uint32_t EDMA3_DRV_getVersion(void);

    /**
 * \brief  Version string querry 
 *
 * The function is used to get the version string for the EDMA LLD.
 *
 * \return      Version Information.
 */
    const char *EDMA3_DRV_getVersionStr(void);

    /**
 * \brief       Start EDMA transfer on the specified channel.
 *
 * There are multiple ways to trigger an EDMA3 transfer. The triggering mode
 * option allows choosing from the available triggering modes: Event,
 * Manual or QDMA.
 *
 * In event triggered, a peripheral or an externally generated event triggers
 * the transfer. This API clears the Secondary Event Register and Event Miss
 * Register and then enables the DMA channel by writing to the EESR.
 *
 * In manual triggered mode, CPU manually triggers a transfer by writing a 1
 * in the Event Set Register (ESR/ESRH). This API writes to the ESR/ESRH to
 * start the transfer.
 *
 * In QDMA triggered mode, a QDMA transfer is triggered when a CPU (or other
 * EDMA3 programmer) writes to the trigger word of the QDMA channel PaRAM set
 * (auto-triggered) or when the EDMA3CC performs a link update on a PaRAM set
 * that has been mapped to a QDMA channel (link triggered). This API enables
 * the QDMA channel by writing to the QEESR register.
 *
 * \param  hEdma        [IN]    Handle to the EDMA Driver Instance
 * \param  lCh          [IN]    Channel on which transfer has to be started
 * \param  trigMode     [IN]    Mode of triggering start of transfer (Manual,
 *                              QDMA or Event)
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_enableTransfer(EDMA3_DRV_Handle hEdma,
                                              uint32_t lCh,
                                              EDMA3_DRV_TrigMode trigMode);

    /**
 * \brief Disable DMA transfer on the specified channel
 *
 * There are multiple ways by which an EDMA3 transfer could be triggered.
 * The triggering mode option allows choosing from the available triggering
 * modes: Event, Manual or QDMA.
 *
 * To disable a channel which was previously triggered in manual mode,
 * this API clears the Secondary Event Register and Event Miss Register,
 * if set, for the specific DMA channel.
 *
 * To disable a channel which was previously triggered in QDMA mode, this
 * API clears the QDMA Event Enable Register, for the specific QDMA channel.
 *
 * To disable a channel which was previously triggered in event mode, this API
 * clears the Event Enable Register. It also clears Event Register, Secondary
 * Event Register and Event Miss Register, if set, for the specific DMA channel.
 *
 * \param   hEdma       [IN]    Handle to the EDMA Driver Instance
 * \param   lCh         [IN]    Channel on which transfer has to be stopped
 * \param   trigMode    [IN]    Mode of triggering start of transfer
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_disableTransfer(EDMA3_DRV_Handle hEdma,
                                               uint32_t lCh,
                                               EDMA3_DRV_TrigMode trigMode);

    /**
 * \brief Disable the event driven DMA channel or QDMA channel
 *
 * This API disables the DMA channel (which was previously triggered in event
 * mode) by clearing the Event Enable Register; it disables the QDMA channel by
 * clearing the QDMA Event Enable Register.
 *
 * This API should NOT be used for DMA channels which are not mapped to any
 * hardware events and are used for memory-to-memory copy based transfers. In
 * case of that, this API returns error.
 *
 * \param   hEdma       [IN]    Handle to the EDMA Driver Instance
 * \param   lCh         [IN]    DMA/QDMA Channel which needs to be disabled
 * \param   trigMode    [IN]    Mode of triggering start of transfer
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_disableLogicalChannel(
        EDMA3_DRV_Handle hEdma,
        uint32_t lCh,
        EDMA3_DRV_TrigMode trigMode);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_SYMBOL_ENUM
 @{ */

    /**
 * \brief PaRAM Set Entry type
 *
 * Use this enum to set or get any of the
 * 8 DWords(uint32_t) within a Parameter RAM set
 */
    typedef enum
    {
        /**
     * The OPT field (Offset Address 0x0 Bytes)
     */
        EDMA3_DRV_PARAM_ENTRY_OPT = 0U,

        /**
     * The SRC field (Offset Address 0x4 Bytes)
     */
        EDMA3_DRV_PARAM_ENTRY_SRC = 1U,

        /**
     * The (ACNT+BCNT) field (Offset Address 0x8 Bytes)
     */
        EDMA3_DRV_PARAM_ENTRY_ACNT_BCNT = 2U,

        /**
     * The DST field (Offset Address 0xC Bytes)
     */
        EDMA3_DRV_PARAM_ENTRY_DST = 3U,

        /**
     * The (SRCBIDX+DSTBIDX) field (Offset Address 0x10 Bytes)
     */
        EDMA3_DRV_PARAM_ENTRY_SRC_DST_BIDX = 4U,

        /**
     * The (LINK+BCNTRLD) field (Offset Address 0x14 Bytes)
     */
        EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD = 5U,

        /**
     * The (SRCCIDX+DSTCIDX) field (Offset Address 0x18 Bytes)
     */
        EDMA3_DRV_PARAM_ENTRY_SRC_DST_CIDX = 6U,

        /**
     * The (CCNT+RSVD) field (Offset Address 0x1C Bytes)
     */
        EDMA3_DRV_PARAM_ENTRY_CCNT = 7U

    } EDMA3_DRV_PaRAMEntry;

    /**
 * \brief PaRAM Set Field type
 *
 * Use this enum to set or get any of the PaRAM set fields
 */
    typedef enum
    {
        /** OPT field of PaRAM Set */
        EDMA3_DRV_PARAM_FIELD_OPT = 0,

        /**
     * \brief Starting byte address of Source
     * For FIFO mode, srcAddr must be a 256-bit aligned address.
     */
        EDMA3_DRV_PARAM_FIELD_SRCADDR = 1,

        /**
     * \brief Number of bytes in each Array (ACNT)
     */
        EDMA3_DRV_PARAM_FIELD_ACNT = 2,

        /**
     * \brief Number of Arrays in each Frame (BCNT)
     */
        EDMA3_DRV_PARAM_FIELD_BCNT = 3,

        /**
     * \brief Starting byte address of destination
     * For FIFO mode, destAddr must be a 256-bit aligned address.
     */
        EDMA3_DRV_PARAM_FIELD_DESTADDR = 4,

        /**
     * \brief Index between consec. arrays of a Source Frame (SRCBIDX)
     * If SAM is set to 1 (via channelOptions) then srcInterArrIndex should
     * be an even multiple of 32 bytes.
     */
        EDMA3_DRV_PARAM_FIELD_SRCBIDX = 5,

        /**
     * \brief Index between consec. arrays of a Destination Frame (DSTBIDX)
     * If DAM is set to 1 (via channelOptions) then destInterArrIndex should
     * be an even multiple of 32 bytes
     */
        EDMA3_DRV_PARAM_FIELD_DESTBIDX = 6,

        /**
     * \brief Address for linking (AutoReloading of a PaRAM Set)
     * This must point to a valid aligned 32-byte PaRAM set
     * A value of 0xFFFF means no linking
     * Linking is especially useful for use with ping-pong buffers and
     * circular buffers
     */
        EDMA3_DRV_PARAM_FIELD_LINKADDR = 7,

        /**
     * \brief Reload value of the numArrInFrame (BCNT)
     * Relevant only for A-sync transfers
     */
        EDMA3_DRV_PARAM_FIELD_BCNTRELOAD = 8,

        /**
     * \brief Index between consecutive frames of a Source Block (SRCCIDX)
     */
        EDMA3_DRV_PARAM_FIELD_SRCCIDX = 9,

        /**
     * \brief Index between consecutive frames of a Dest Block (DSTCIDX)
     */
        EDMA3_DRV_PARAM_FIELD_DESTCIDX = 10,

        /**
     * \brief Number of Frames in a block (CCNT)
     */
        EDMA3_DRV_PARAM_FIELD_CCNT = 11

    } EDMA3_DRV_PaRAMField;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_DATASTRUCT
 @{ */

    /**
 * \brief EDMA3 PaRAM Set
 *
 * This is a mapping of the EDMA3 PaRAM set provided to the user
 * for ease of modification of the individual PaRAM words.
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

    } EDMA3_DRV_ParamentryRegs;

/**
 * \brief EDMA3 Parameter RAM Set in User Configurable format
 *
 * This is a mapping of the EDMA3 PaRAM set provided to the user
 * for ease of modification of the individual fields
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
    } EDMA3_DRV_PaRAMRegs;
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
} EDMA3_DRV_PaRAMRegs;
#endif

    /**
 * \brief Event queue priorities setup
 *
 * It allows to change the priority of the individual queues and the
 * priority of the transfer request (TR) associated with the
 * events queued in the queue.
 */
    typedef struct
    {
        /**
     * \brief Event Queue Priorities
     */
        uint32_t evtQPri[EDMA3_MAX_EVT_QUE];
    } EDMA3_DRV_EvtQuePriority;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_FUNCTION_ADVANCED
 @{ */

    /**
 * \brief  Assign a Trigger Word to the specified QDMA channel
 *
 * This API sets the Trigger word for the specific QDMA channel in the QCHMAP
 * Register. Default QDMA trigger word is CCNT.
 *
 * \param   hEdma      [IN]    Handle to the EDMA Instance object
 * \param   lCh        [IN]    QDMA Channel which needs to be assigned
 *                             the Trigger Word
 * \param   trigWord   [IN]    The Trigger Word for the QDMA channel.
 *                             Trigger Word is the word in the PaRAM
 *                             Register Set which, when written to by CPU,
 *                             will start the QDMA transfer automatically.
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_setQdmaTrigWord(EDMA3_DRV_Handle hEdma,
                                               uint32_t lCh,
                                               EDMA3_RM_QdmaTrigWord trigWord);

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
 * \param   hEdma       [IN]  Handle to the EDMA Instance object
 * \param   lCh         [IN]  Logical Channel for which new PaRAM set is
 *                            specified
 * \param   newPaRAM    [IN]  Parameter RAM set to be copied onto existing PaRAM
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_setPaRAM(EDMA3_DRV_Handle hEdma,
                                        uint32_t lCh,
                                        const EDMA3_DRV_PaRAMRegs *newPaRAM);

    /**
 * \brief   Retrieve existing PaRAM set associated with specified logical
 *          channel (DMA/QDMA/Link).
 *
 * \param   hEdma           [IN]     Handle to the EDMA Instance object
 * \param   lCh             [IN]     Logical Channel whose PaRAM set is
 *                                   requested
 * \param   currPaRAM       [IN/OUT] User gets the existing PaRAM here
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_getPaRAM(EDMA3_DRV_Handle hEdma,
                                        uint32_t lCh,
                                        EDMA3_DRV_PaRAMRegs *currPaRAM);

    /**
 * \brief   Set a particular PaRAM set entry of the specified PaRAM set
 *
 *
 * \param   hEdma       [IN]    Handle to the EDMA Driver Instance
 * \param   lCh         [IN]    Logical Channel bound to the Parameter RAM set
 *                              whose specified field needs to be set
 * \param   paRAMEntry  [IN]    Specify the PaRAM set entry which needs
 *                              to be set
 * \param   newPaRAMEntryVal [IN]    The new field setting
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This API should be used while setting the PaRAM set entry
 *          for QDMA channels. If EDMA3_DRV_setPaRAMField () used,
 *          it will trigger the QDMA channel before complete
 *          PaRAM set entry is written. For DMA channels, no such
 *          constraint is there.
 *
 *          This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_setPaRAMEntry(EDMA3_DRV_Handle hEdma,
                                             uint32_t lCh,
                                             EDMA3_DRV_PaRAMEntry paRAMEntry,
                                             uint32_t newPaRAMEntryVal);

    /**
 * \brief   Get a particular PaRAM set entry of the specified PaRAM set
 *
 *
 * \param   hEdma       [IN]    Handle to the EDMA Driver Instance
 * \param   lCh         [IN]    Logical Channel bound to the Parameter RAM set
 *                              whose specified field value is needed
 * \param   paRAMEntry  [IN]    Specify the PaRAM set entry which needs
 *                              to be obtained
 * \param   paRAMEntryVal [IN/OUT]  The value of the field is returned here
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_getPaRAMEntry(EDMA3_DRV_Handle hEdma,
                                             uint32_t lCh,
                                             EDMA3_DRV_PaRAMEntry paRAMEntry,
                                             uint32_t *paRAMEntryVal);

    /**
 * \brief   Set a particular PaRAM set field of the specified PaRAM set
 *
 *
 * \param   hEdma               [IN]    Handle to the EDMA Driver Instance
 * \param   lCh                 [IN]    Logical Channel bound to the PaRAM set
 *                                      whose specified field needs to be set
 * \param   paRAMField          [IN]    Specify the PaRAM set field which needs
 *                                      to be set
 * \param   newPaRAMFieldVal    [IN]    The new field setting
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This API CANNOT be used while setting the PaRAM set
 *          field for QDMA channels. It can trigger the QDMA channel before
 *          complete PaRAM set ENTRY (4-bytes field) is written (for eg, as
 *          soon one sets the ACNT field for QDMA channel, transfer is started,
 *          before one modifies the BCNT field). For DMA channels, no such
 *          constraint is there.
 *
 *          This function is re-entrant for unique lCh values. It is non-
 *          re-entrant for same lCh value.
 */
    EDMA3_DRV_Result EDMA3_DRV_setPaRAMField(EDMA3_DRV_Handle hEdma,
                                             uint32_t lCh,
                                             EDMA3_DRV_PaRAMField paRAMField,
                                             uint32_t newPaRAMFieldVal);

    /**
 * \brief   Get a particular PaRAM set field of the specified PaRAM set
 *
 *
 * \param   hEdma               [IN]    Handle to the EDMA Driver Instance
 * \param   lCh                 [IN]    Logical Channel bound to the PaRAM set
 *                                      whose specified field value is needed
 * \param   paRAMField          [IN]    Specify the PaRAM set field which needs
 *                                      to be obtained
 * \param   currPaRAMFieldVal [IN/OUT]  The value of the field is returned here
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_getPaRAMField(EDMA3_DRV_Handle hEdma,
                                             uint32_t lCh,
                                             EDMA3_DRV_PaRAMField paRAMField,
                                             uint32_t *currPaRAMFieldVal);

    /**
 * \brief   Sets EDMA TC priority
 *
 * User can program the priority of the Event Queues at a system-wide level.
 * This means that the user can set the priority of an IO initiated by either
 * of the TCs (Transfer Ctrllers) relative to IO initiated by the other bus
 * masters on the device (ARM, DSP, USB, etc)
 *
 * \param   hEdma           [IN]    Handle to the EDMA Driver Instance
 * \param   evtQPriObj     [IN]    Priority of the Event Queues
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function disables the global interrupts while modifying
 *          the global CC Registers, to make it re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_setEvtQPriority(EDMA3_DRV_Handle hEdma,
                                               const EDMA3_DRV_EvtQuePriority *evtQPriObj);

    /**
 * \brief   Associate Channel to Event Queue
 *
 *
 * \param   hEdma       [IN]     Handle to the EDMA Driver Instance
 * \param   channelId   [IN]     Logical Channel to which the Event
 *                               Queue is to be mapped
 * \param   eventQ      [IN]     The Event Queue which is to be mapped
 *                               to the DMA channel
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note        There should not be any data transfer going on
 *              while setting the mapping. Results could be unpredictable.
 *
 *              This function disables the global interrupts while modifying
 *              the global CC Registers, to make it re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_mapChToEvtQ(EDMA3_DRV_Handle hEdma,
                                           uint32_t channelId,
                                           EDMA3_RM_EventQueue eventQ);

    /**
 * \brief   Get the Event Queue mapped to the specified DMA/QDMA channel.
 *
 * \param   hEdma       [IN]     Handle to the EDMA Driver Instance
 * \param   channelId   [IN]     Logical Channel whose associated
 *                               Event Queue is needed
 * \param   mappedEvtQ  [IN/OUT] The Event Queue which is mapped
 *                               to the DMA/QDMA channel
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_getMapChToEvtQ(EDMA3_DRV_Handle hEdma,
                                              uint32_t channelId,
                                              uint32_t *mappedEvtQ);

    /**
 * \brief   Set the Channel Controller (CC) Register value
 *
 * \param   hEdma       [IN]     Handle to the EDMA Driver Instance
 * \param   regOffset   [IN]     CC Register offset whose value needs to be set
 * \param   newRegValue [IN]     New CC Register Value
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is non re-entrant for users using the same
 *          EDMA handle i.e. working on the same shadow region.
 *          Before modifying a register, it tries to acquire a semaphore
 *          (Driver instance specific), to protect simultaneous
 *          modification of the same register by two different users.
 *          After the successful change, it releases the semaphore.
 *          For users working on different shadow regions, thus different
 *          EDMA handles, this function is re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_setCCRegister(EDMA3_DRV_Handle hEdma,
                                             uint32_t regOffset,
                                             uint32_t newRegValue);

    /**
 * \brief   Get the Channel Controller (CC) Register value
 *
 * \param   hEdma       [IN]     Handle to the EDMA Driver Instance
 * \param   regOffset   [IN]     CC Register offset whose value is needed
 * \param   regValue    [IN/OUT] CC Register Value
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_getCCRegister(EDMA3_DRV_Handle hEdma,
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
 * \param   hEdma       [IN]     Handle to the EDMA Driver Instance
 * \param   tccNo       [IN]     TCC, specific to which the function
 *                               waits on a IPR/IPRH bit.
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for different tccNo.
 */
    EDMA3_DRV_Result EDMA3_DRV_waitAndClearTcc(EDMA3_DRV_Handle hEdma,
                                               uint32_t tccNo);

    /**
 * \brief   Returns the status of a previously initiated transfer.
 *
 * This is a non-blocking function that returns the status of a previously
 * initiated transfer, based on the IPR/IPRH bit. This bit corresponds to
 * the tccNo specified by the user. It clears the corresponding bit, if SET,
 * while returning also.
 *
 * \param   hEdma       [IN]     Handle to the EDMA Driver Instance
 * \param   tccNo       [IN]     TCC, specific to which the function
 *                               checks the status of the IPR/IPRH bit.
 * \param   tccStatus   [IN/OUT] Status of the transfer is returned here.
 *                               Returns "TRUE" if the transfer has
 *                               completed (IPR/IPRH bit SET),
 *                               "FALSE" if the transfer has not completed
 *                               successfully (IPR/IPRH bit NOT SET).
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for different tccNo.
 */
    EDMA3_DRV_Result EDMA3_DRV_checkAndClearTcc(EDMA3_DRV_Handle hEdma,
                                                uint32_t tccNo,
                                                uint16_t *tccStatus);

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
 *
 * \param   hEdma       [IN]            Handle to the EDMA Driver Instance
 * \param   lCh         [IN]            Logical Channel for which the PaRAM set
 *                                      physical address is required
 * \param   paramPhyAddr [IN/OUT]       PaRAM Set physical address is returned
 *                                      here.
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_getPaRAMPhyAddr(EDMA3_DRV_Handle hEdma,
                                               uint32_t lCh,
                                               uint32_t *paramPhyAddr);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_SYMBOL_ENUM
 @{ */

    /**\enum    EDMA3_DRV_IoctlCmd
 * \brief   EDMA3 Driver IOCTL commands
 */
    typedef enum
    {
        /* Min IOCTL */
        EDMA3_DRV_IOCTL_MIN_IOCTL = 0,

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
     * Note: Since this enum can change the behavior how the resources are
     * initialized during their allocation, user is adviced to not use this
     * command while allocating the resources. User should first change the
     * behavior of resources' initialization and then should use start
     * allocating resources.
     */
        EDMA3_DRV_IOCTL_SET_PARAM_CLEAR_OPTION,

        /**
     * To check whether PaRAM Sets will be cleared or not
     * during allocation.
     * If the value read is '1', it means that PaRAM Sets are getting cleared
     * during allocation.
     * If the value read is '0', it means that PaRAM Sets are NOT getting cleared
     * during allocation.
     * For e.g.,
     * uint16_t isParamClearingDone;
     * cmdArg = &paramClearingRequired;
     */
        EDMA3_DRV_IOCTL_GET_PARAM_CLEAR_OPTION,

        /* Max IOCTLs */
        EDMA3_DRV_IOCTL_MAX_IOCTL
    } EDMA3_DRV_IoctlCmd;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_FUNCTION_ADVANCED
 @{ */

    /**
 *  \brief EDMA3 Driver IOCTL
 *
 *  This function provides IOCTL functionality for EDMA3 Driver.
 *
 *  \param   hEdma          [IN]        Handle to the EDMA Driver Instance
 *  \param  cmd             [IN]        IOCTL command to be performed
 *  \param  cmdArg          [IN/OUT]    IOCTL command argument (if any)
 *  \param  param           [IN/OUT]    Device/Cmd specific argument
 *
 *  \return EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note For 'EDMA3_DRV_IOCTL_GET_PARAM_CLEAR_OPTION', this function is re-entrant.
 * For 'EDMA3_DRV_IOCTL_SET_PARAM_CLEAR_OPTION', this function is re-entrant for
 * different EDMA3 Driver Instances (handles).
 */
    EDMA3_DRV_Result EDMA3_DRV_Ioctl(
        EDMA3_DRV_Handle hEdma,
        EDMA3_DRV_IoctlCmd cmd,
        void *cmdArg,
        void *param);

    /**
 * \brief	Return the previously opened EDMA3 Driver Instance handle
 *
 * This API is used to return the previously opened EDMA3 Driver's
 * Instance Handle (region specific), which could be used to call other
 * EDMA3 Driver APIs. Since EDMA3 Driver does not allow multiple instances,
 * for a single shadow region, this API is provided. This API is meant
 * for users who DO NOT want to / could not open a new Driver Instance and
 * hence re-use the existing Driver Instance to allocate EDMA3 resources
 * and use various other EDMA3 Driver APIs.
 *
 * In case the Driver Instance is not yet opened, NULL is returned as the
 * function return value whereas EDMA3_DRV_E_INST_NOT_OPENED is returned
 * in the errorCode.
 *
 * \param   phyCtrllerInstId    [IN]    EDMA3 Controller Instance Id (Hardware
 *                                      instance id, starting from 0).
 * \param   regionId			[IN]    Shadow Region id for which the previously
 *										opened driver's instance handle is
 *										required.
 * \param   errorCode           [OUT]   Error code while returning Driver Instance
 *										Handle.
 *
 * \return EDMA3_DRV_Handle : If successful, this API will return the
 *                            driver's instance handle.
 *
 * \note    1) This API returns the previously opened EDMA3 Driver's Instance
 * 		handle. The instance, if exists, could have been opened by some other
 *		user (most probably) or may be by the same user calling this API. If
 *		it was opened by some other user, then that user can very well close
 *		this instance anytime, without even knowing that the same instance
 *		handle is being used by other users as well. In that case, the
 *		handle becomes INVALID and user has to open a valid driver
 *		instance for his/her use.
 *
 * 			2) This function is re-entrant.
 */
    EDMA3_DRV_Handle EDMA3_DRV_getInstHandle(uint32_t phyCtrllerInstId,
                                             EDMA3_RM_RegionId regionId,
                                             EDMA3_DRV_Result *errorCode);

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
 * \param  hEdma            	[IN]	Handle to the EDMA Driver Instance.
 * \param  channelId        	[IN]    DMA/QDMA channel for which the callback
 *										function needs to be registered.
 * \param   tccCb               [IN]    The callback function to be registered.
 * \param   cbData              [IN]    Callback data to be passed while calling
 *                                      the callback function.
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique channelId values. It is non-
 *          re-entrant for same channelId value.
 */
    EDMA3_DRV_Result EDMA3_DRV_registerTccCb(EDMA3_DRV_Handle hEdma,
                                             const uint32_t channelId,
                                             EDMA3_RM_TccCallback tccCb,
                                             void *cbData);

    /**
 * \brief   Un-register the previously registered callback function against a
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
 * \param  hEdma            	[IN]	Handle to the EDMA Driver Instance.
 * \param  channelId        	[IN]    DMA/QDMA channel for which the callback
 *										function needs to be un-registered.
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique channelId. It is
 *			non-re-entrant for same channelId.
 */
    EDMA3_DRV_Result EDMA3_DRV_unregisterTccCb(EDMA3_DRV_Handle hEdma,
                                               const uint32_t channelId);

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_SYMBOL_ENUM
 @{ */

    /**\enum    EDMA3_DRV_Tc_Err
 * \brief   TC Error Enablers
 *
 * Use this enum to enable/disable the specific EDMA3 Transfer Controller
 * Interrupts.
 */
    typedef enum
    {
        /**
	 * Interrupt disable for bus error
	 */
        EDMA3_DRV_TC_ERR_BUSERR_DIS = 0,

        /**
	 * Interrupt enable for bus error
	 */
        EDMA3_DRV_TC_ERR_BUSERR_EN,

        /**
	 * Interrupt disable for transfer request error
	 */
        EDMA3_DRV_TC_ERR_TRERR_DIS,

        /**
	 * Interrupt enable for transfer request error
	 */
        EDMA3_DRV_TC_ERR_TRERR_EN,

        /**
	 * Interrupt disable for MMR address error
	 */
        EDMA3_DRV_TC_ERR_MMRAERR_DIS,

        /**
	 * Interrupt enable for MMR address error
	 */
        EDMA3_DRV_TC_ERR_MMRAERR_EN,

        /**
	 * Disable all TC error interrupts
	 */
        EDMA3_DRV_TC_ERR_DIS,

        /**
	 * Enable all TC error interrupts
	 */
        EDMA3_DRV_TC_ERR_EN
    } EDMA3_DRV_Tc_Err;

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_FUNCTION_ADVANCED
 @{ */

    /**
 * \brief   Enable/disable specific EDMA3 Transfer Controller Interrupts
 *
 * This function allows one to enable/disable specific EDMA3 Transfer Controller
 * Interrupts. Since these interrupts don't get enabled by default, this API can
 * be used to achieve the same.
 *
 * \param phyCtrllerInstId  [IN]    EDMA3 Controller Instance Id
 *                                 (Hardware instance id, starting from 0).
 * \param tcId  			[IN]    Transfer Controller Id. It starts from 0
 *									for each EDMA3 hardware and can go upto
 *									(TCs available on EDMA3 Hardware - 1).
 * \param tcErr 			[IN]    TC Error Interrupts which need to be
 *									enabled/disabled.
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error code
 *
 * \note    This function is re-entrant for unique combination of EDMA3 hw and
 *			TC. It is non-re-entrant for same combination.
 */
    EDMA3_DRV_Result EDMA3_DRV_setTcErrorInt(uint32_t phyCtrllerInstId,
                                             uint32_t tcId,
                                             EDMA3_DRV_Tc_Err tcErr);

/**
@}
*/

/** @addtogroup EDMA3_LLD_DRV_SYMBOL_DEFINE
 @{ */

/**
 * \brief Channel status defines
 * These defines suggest the current state of the DMA / QDMA channel. They
 * are used while returning the channel status from EDMA3_DRV_getChannelStatus().
 */
/** Channel is clean; no pending event, completion interrupt and event miss interrupt */
#define EDMA3_DRV_CHANNEL_CLEAN 0x0000U
/** Pending event is detected on the DMA channel */
#define EDMA3_DRV_CHANNEL_EVENT_PENDING 0x0001U
/** Transfer completion interrupt is detected on the DMA/QDMA channel */
#define EDMA3_DRV_CHANNEL_XFER_COMPLETE 0x0002U
/** Event miss error interrupt is detected on the DMA/QDMA channel */
#define EDMA3_DRV_CHANNEL_ERR 0x0004U

    /**
@}
*/

    /** @addtogroup EDMA3_LLD_DRV_FUNCTION_ADVANCED
 @{ */

    /**
 * \brief   Get the current status of the DMA/QDMA channel
 *
 * This function returns the current status of the specific DMA/QDMA channel.
 * For a DMA channel, it checks whether an event is pending in ER, transfer
 * completion interrupt is pending in IPR and event miss error interrupt is
 * pending in EMR or not. For a QDMA channel, it checks whether a transfer
 * completion interrupt is pending in IPR and event miss error interrupt is
 * pending in QEMR or not.
 *
 * \param  hEdma            	[IN]	Handle to the EDMA Driver Instance.
 * \param  lCh                 	[IN]    DMA/QDMA channel for which the current
 *										status is required.
 * \param  lchStatus         	[IN/OUT]Status of the channel. Defines mentioned
 *										above are used (and may be combined) to
 *										return the actual status.
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_getChannelStatus(EDMA3_DRV_Handle hEdma,
                                                uint32_t lCh, uint32_t *lchStatus);

    /**
 * \brief  Associates a link channel and a TCC
 *
 * This API is used to map a TCC to a LINK channel. It should be used with LINK
 * channels ONLY else it will fail. It will copy the TCC code in the OPT field
 * of the param set associated with the link channel.
 *
 * \param   hEdma           [IN]    Handle to the EDMA Driver Instance.
 * \param   linkCh          [IN]    Link Channel to which a particular TCC
 *                                  needs to be mapped.
 * \param   tcc             [IN]    TCC which needs to be mapped.
 *
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique linkCh values. It is
 *          non-re-entrant for same linkCh values.
 */
    EDMA3_DRV_Result EDMA3_DRV_mapTccLinkCh(EDMA3_DRV_Handle hEdma,
                                            uint32_t linkCh,
                                            uint32_t tcc);

#define EDMA3_DRV_MAX_XBAR_EVENTS (63U)

    /**\struct  EDMA3_DRV_GblXbarToChanConfigParams
 * \brief   Init-time Configuration structure for EDMA3
 * controller, to provide Global SoC specific Information.
 *
 * This configuration structure is used to specify the EDMA3 Driver
 * global settings, specific to the SoC. 
 * This configuraion structure provides the details of the mapping of cross bar 
 * events to available channels.
 * This configuration information is SoC specific and could be provided by the
 * user at run-time while creating the EDMA3 Driver Object, using API
 * EDMA3_DRV_initXbarEventMap. In case user doesn't provide it, 
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
        int32_t dmaMapXbarToChan[EDMA3_DRV_MAX_XBAR_EVENTS];
    } EDMA3_DRV_GblXbarToChanConfigParams;

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
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique event values. It is
 *          non-re-entrant for same event values.
 */
    typedef EDMA3_DRV_Result (*EDMA3_DRV_mapXbarEvtToChan)(uint32_t eventNum,
                                                           uint32_t *chanNum,
                                                           const EDMA3_DRV_GblXbarToChanConfigParams *edmaGblXbarConfig);

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
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is re-entrant for unique event values. It is
 *          non-re-entrant for same event values.
 */
    typedef EDMA3_DRV_Result (*EDMA3_DRV_xbarConfigScr)(uint32_t eventNum,
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
 * \return      EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function disables the global interrupts (by calling API
 *          edma3OsProtectEntry with protection level
 *          EDMA3_OS_PROTECT_INTERRUPT) while modifying the global data
 *          structures, to make it re-entrant.
 */
    EDMA3_DRV_Result EDMA3_DRV_initXbarEventMap(EDMA3_DRV_Handle hEdma,
                                                const EDMA3_DRV_GblXbarToChanConfigParams *edmaGblXbarConfig,
                                                EDMA3_DRV_mapXbarEvtToChan mapXbarEvtFunc,
                                                EDMA3_DRV_xbarConfigScr configXbarScr);

    /**
 * \brief   Returns the PARAM ID recently allocated for an EDMA channel
 *
 * This API will return the PARAM Id recently allocated by the EDMA3
 * resource manager for a requested DMA channel
 *
 * This function can be called by the application after
 * the EDMA3_DRV_requestChannel() function is invoked to check the
 * allocated PaRAM Id for a requested channel if needed.
 *
 * \param   hEdma              [IN]    Handle to the EDMA Driver Instance.
 * \param   channelId          [IN]    DMA channel for which the PaRAMId
 *                                     needs to be returned.
 * \param   paRAMId            [OUT]   Return the paRAMId of which the
 *                                     channel Id is mapped.
 *
 * \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 *
 * \note    This function is used to verify the requested DMA channel
 *          is allocated/mapped with the PaRAM Id same as DMA channel number.
 *          This function is not necessarily be used(invoked) for
 *          any other general functionality.
 *
 */
    EDMA3_DRV_Result EDMA3_DRV_getAllocatedPARAMId(EDMA3_DRV_Handle hEdma,
                                                   uint32_t channelId, uint32_t *paRAMId);

    /**
@}
*/

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _EDMA3_DRV_H_ */
