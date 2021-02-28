/*
 * sample_am335x_cfg.c
 *
 * Platform specific EDMA3 hardware related information like number of transfer
 * controllers, various interrupt ids etc. It is used while interrupts
 * enabling / disabling. It needs to be ported for different SoCs.
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
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

#include <ti/edma3/edma3_drv.h>

/* Number of EDMA3 controllers present in the system */
#define NUM_EDMA3_INSTANCES 1u
const uint32_t numEdma3Instances = NUM_EDMA3_INSTANCES;

uint16_t determineProcId(void)
{
  return 0U;
}

int8_t *getGlobalAddr(int8_t *addr)
{
  return (addr); /* The address is already a global address */
}

uint16_t isGblConfigRequired(uint32_t dspNum)
{
  (void)dspNum;
  return 0U;
}

/* Semaphore handles */
EDMA3_OS_Sem_Handle semHandle[NUM_EDMA3_INSTANCES] = {NULL};

/** Total number of DMA Channels supported by the EDMA3 Controller */
#define EDMA3_NUM_DMA_CHANNELS (64u)
/** Total number of QDMA Channels supported by the EDMA3 Controller */
#define EDMA3_NUM_QDMA_CHANNELS (8u)
/** Total number of TCCs supported by the EDMA3 Controller */
#define EDMA3_NUM_TCC (64u)
/** Total number of PaRAM Sets supported by the EDMA3 Controller */
#define EDMA3_NUM_PARAMSET (256u)
/** Total number of Event Queues in the EDMA3 Controller */
#define EDMA3_NUM_EVTQUE (3u)
/** Total number of Transfer Controllers (TCs) in the EDMA3 Controller */
#define EDMA3_NUM_TC (3u)
/** Number of Regions on this EDMA3 controller */
#define EDMA3_NUM_REGION (8u)

/**
 * \brief Channel mapping existence
 * A value of 0 (No channel mapping) implies that there is fixed association
 * for a channel number to a parameter entry number or, in other words,
 * PaRAM entry n corresponds to channel n.
 */
#define CHANNEL_MAPPING_EXISTENCE (1u)
/** Existence of memory protection feature */
#define MEM_PROTECTION_EXISTENCE (1u)

/** Global Register Region of CC Registers */
#define CC_BASE_ADDRESS (0x49000000u)
/** Transfer Controller 0 Registers */
#define TC0_BASE_ADDRESS (0x49800000u)
/** Transfer Controller 1 Registers */
#define TC1_BASE_ADDRESS (0x49900000u)
/** Transfer Controller 2 Registers */
#define TC2_BASE_ADDRESS (0x49A00000u)
/** Transfer Controller 3 Registers */
#define TC3_BASE_ADDRESS NULL
/** Transfer Controller 4 Registers */
#define TC4_BASE_ADDRESS NULL
/** Transfer Controller 5 Registers */
#define TC5_BASE_ADDRESS NULL
/** Transfer Controller 6 Registers */
#define TC6_BASE_ADDRESS NULL
/** Transfer Controller 7 Registers */
#define TC7_BASE_ADDRESS NULL

/** Interrupt no. for Transfer Completion */
#define EDMA3_CC_XFER_COMPLETION_INT (12u)
/** Interrupt no. for CC Error */
#define EDMA3_CC_ERROR_INT (14u)
/** Interrupt no. for TC 0 Error */
#define EDMA3_TC0_ERROR_INT (112u)
/** Interrupt no. for TC 1 Error */
#define EDMA3_TC1_ERROR_INT (113u)
/** Interrupt no. for TC 2 Error */
#define EDMA3_TC2_ERROR_INT (114u)
/** Interrupt no. for TC 3 Error */
#define EDMA3_TC3_ERROR_INT (0u)
/** Interrupt no. for TC 4 Error */
#define EDMA3_TC4_ERROR_INT (0u)
/** Interrupt no. for TC 5 Error */
#define EDMA3_TC5_ERROR_INT (0u)
/** Interrupt no. for TC 6 Error */
#define EDMA3_TC6_ERROR_INT (0u)
/** Interrupt no. for TC 7 Error */
#define EDMA3_TC7_ERROR_INT (0u)

/**
 * EDMA3 interrupts (transfer completion, CC error etc.) correspond to different
 * ECM events (SoC specific). These ECM events come
 * under ECM block XXX (handling those specific ECM events). Normally, block
 * 0 handles events 4-31 (events 0-3 are reserved), block 1 handles events
 * 32-63 and so on. This ECM block XXX (or interrupt selection number XXX)
 * is mapped to a specific HWI_INT YYY in the tcf file.
 * Define EDMA3_HWI_INT_XFER_COMP to specific HWI_INT, corresponding
 * to transfer completion interrupt.
 * Define EDMA3_HWI_INT_CC_ERR to specific HWI_INT, corresponding
 * to CC error interrupts.
 * Define EDMA3_HWI_INT_TC_ERR to specific HWI_INT, corresponding
 * to TC error interrupts.
 */
/* EDMA 0 */

#define EDMA3_HWI_INT_XFER_COMP (7U)
#define EDMA3_HWI_INT_CC_ERR (7U)
#define EDMA3_HWI_INT_TC0_ERR (10U)
#define EDMA3_HWI_INT_TC1_ERR (10U)
#define EDMA3_HWI_INT_TC2_ERR (10U)

/**
 * \brief Mapping of DMA channels 0-31 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_RM_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_RM_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
/* 31     0 */
#define DMA_CHANNEL_TO_EVENT_MAPPING_0 (0xFFCFCFFFu)

/**
 * \brief Mapping of DMA channels 32-63 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_RM_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_RM_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
/* DMA channels 32-63 DOES NOT exist in DA830. */
#define DMA_CHANNEL_TO_EVENT_MAPPING_1 (0xFF3FFFC0u)

/* Variable which will be used internally for referring number of Event Queues*/
uint32_t numEdma3EvtQue[NUM_EDMA3_INSTANCES] = {EDMA3_NUM_EVTQUE};

/* Variable which will be used internally for referring number of TCs.        */
uint32_t numEdma3Tc[NUM_EDMA3_INSTANCES] = {EDMA3_NUM_TC};

/**
 * Variable which will be used internally for referring transfer completion
 * interrupt.
 */
uint32_t ccXferCompInt[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] = {
    {
        EDMA3_CC_XFER_COMPLETION_INT,
        0u,
        0u,
        0u,
        0u,
        0u,
        0u,
        0u,
    },
};

/**
 * Variable which will be used internally for referring channel controller's
 * error interrupt.
 */
uint32_t ccErrorInt[NUM_EDMA3_INSTANCES] = {EDMA3_CC_ERROR_INT};

/**
 * Variable which will be used internally for referring transfer controllers'
 * error interrupts.
 */
uint32_t tcErrorInt[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] = {
    {
        EDMA3_TC0_ERROR_INT,
        EDMA3_TC1_ERROR_INT,
        EDMA3_TC2_ERROR_INT,
        EDMA3_TC3_ERROR_INT,
        EDMA3_TC4_ERROR_INT,
        EDMA3_TC5_ERROR_INT,
        EDMA3_TC6_ERROR_INT,
        EDMA3_TC7_ERROR_INT,
    }};

/**
 * Variables which will be used internally for referring the hardware interrupt
 * for various EDMA3 interrupts.
 */
uint32_t hwIntXferComp = EDMA3_HWI_INT_XFER_COMP;
uint32_t hwIntCcErr = EDMA3_HWI_INT_CC_ERR;
uint32_t hwIntTcErr[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] =
    {
        /* EDMA3 INSTANCE# 0 */
        {
            EDMA3_HWI_INT_TC0_ERR,
            EDMA3_HWI_INT_TC1_ERR,
            EDMA3_HWI_INT_TC2_ERR,
            0,
            0,
            0,
            0,
            0}};

EDMA3_DRV_GblConfigParams sampleEdma3GblCfgParams[NUM_EDMA3_INSTANCES] =
    {
        {/** Total number of DMA Channels supported by the EDMA3 Controller */
         EDMA3_NUM_DMA_CHANNELS,
         /** Total number of QDMA Channels supported by the EDMA3 Controller */
         EDMA3_NUM_QDMA_CHANNELS,
         /** Total number of TCCs supported by the EDMA3 Controller */
         EDMA3_NUM_TCC,
         /** Total number of PaRAM Sets supported by the EDMA3 Controller */
         EDMA3_NUM_PARAMSET,
         /** Total number of Event Queues in the EDMA3 Controller */
         EDMA3_NUM_EVTQUE,
         /** Total number of Transfer Controllers (TCs) in the EDMA3 Controller */
         EDMA3_NUM_TC,
         /** Number of Regions on this EDMA3 controller */
         EDMA3_NUM_REGION,

         /**
     * \brief Channel mapping existence
     * A value of 0 (No channel mapping) implies that there is fixed association
     * for a channel number to a parameter entry number or, in other words,
     * PaRAM entry n corresponds to channel n.
     */
         CHANNEL_MAPPING_EXISTENCE,

         /** Existence of memory protection feature */
         MEM_PROTECTION_EXISTENCE,

         /** Global Register Region of CC Registers */
         (void *)(CC_BASE_ADDRESS),
         /** Transfer Controller (TC) Registers */
         {
             (void *)(TC0_BASE_ADDRESS),
             (void *)(TC1_BASE_ADDRESS),
             (void *)(TC2_BASE_ADDRESS),
             (void *)(TC3_BASE_ADDRESS),
             (void *)(TC4_BASE_ADDRESS),
             (void *)(TC5_BASE_ADDRESS),
             (void *)(TC6_BASE_ADDRESS),
             (void *)(TC7_BASE_ADDRESS)},
         /** Interrupt no. for Transfer Completion */
         EDMA3_CC_XFER_COMPLETION_INT,
         /** Interrupt no. for CC Error */
         EDMA3_CC_ERROR_INT,
         /** Interrupt no. for TCs Error */
         {
             EDMA3_TC0_ERROR_INT,
             EDMA3_TC1_ERROR_INT,
             EDMA3_TC2_ERROR_INT,
             EDMA3_TC3_ERROR_INT,
             EDMA3_TC4_ERROR_INT,
             EDMA3_TC5_ERROR_INT,
             EDMA3_TC6_ERROR_INT,
             EDMA3_TC7_ERROR_INT},

         /**
     * \brief EDMA3 TC priority setting
     *
     * User can program the priority of the Event Queues
     * at a system-wide level.  This means that the user can set the
     * priority of an IO initiated by either of the TCs (Transfer Controllers)
     * relative to IO initiated by the other bus masters on the
     * device (ARM, DSP, USB, etc)
     */
         {
             0u,
             1u,
             2u,
             0u,
             0u,
             0u,
             0u,
             0u},
         /**
     * \brief To Configure the Threshold level of number of events
     * that can be queued up in the Event queues. EDMA3CC error register
     * (CCERR) will indicate whether or not at any instant of time the
     * number of events queued up in any of the event queues exceeds
     * or equals the threshold/watermark value that is set
     * in the queue watermark threshold register (QWMTHRA).
     */
         {
             16u,
             16u,
             16u,
             0u,
             0u,
             0u,
             0u,
             0u},

         /**
     * \brief To Configure the Default Burst Size (DBS) of TCs.
     * An optimally-sized command is defined by the transfer controller
     * default burst size (DBS). Different TCs can have different
     * DBS values. It is defined in Bytes.
     */
         {
             32u,
             32u,
             32u,
             0u,
             0u,
             0u,
             0u,
             0u},

         /**
     * \brief Mapping from each DMA channel to a Parameter RAM set,
     * if it exists, otherwise of no use.
     */
         {
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP,
             EDMA3_DRV_CH_NO_PARAM_MAP, EDMA3_DRV_CH_NO_PARAM_MAP},

         /**
      * \brief Mapping from each DMA channel to a TCC. This specific
      * TCC code will be returned when the transfer is completed
      * on the mapped channel.
      */
         {
             0u, 1u, 2u, 3u,
             EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP,
             8u, 9u, 10u, 11u,
             EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, 14u, 15u,
             16u, 17u, 18u, 19u,
             EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, 22u, 23u,
             24u, 25u, 26u, 27u,
             28u, 29u, 30u, 31u,
             EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP,
             EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, 38u, 39u,
             40u, 41u, 42u, 43u,
             44u, 45u, 46u, 47u,
             48u, 49u, 50u, 51u,
             52u, 53u, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP,
             56u, 57u, 58u, 59u,
             60u, 61u, 62u, 63u},

         /**
     * \brief Mapping of DMA channels to Hardware Events from
     * various peripherals, which use EDMA for data transfer.
     * All channels need not be mapped, some can be free also.
     */
         {
             DMA_CHANNEL_TO_EVENT_MAPPING_0,
             DMA_CHANNEL_TO_EVENT_MAPPING_1}}};

/* Default DRV Instance Initialization Configuration */
EDMA3_DRV_InstanceInitConfig sampleInstInitConfig[NUM_EDMA3_INSTANCES][EDMA3_NUM_REGION] =
    {
        {

            {
                /* Resources owned by Region 0 */
                /* ownPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0xFFFFFFFFu,
                    0xFFFFFFFFu,
                    0xFFFFFFFFu,
                    0xFFFFFFFFu,
                    /* 159  128     191  160     223  192     255  224 */
                    0xFFFFFFFFu,
                    0xFFFFFFFFu,
                    0xFFFFFFFFu,
                    0xFFFFFFFFu,
                    /* 287  256     319  288     351  320     383  352 */
                    0xFFFFFFFFu,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* ownDmaChannels */
                /* 31     0     63    32 */
                {0xFFFFFFFFu, 0xFFFFFFFFu},

                /* ownQdmaChannels */
                /* 31     0 */
                {0x000000FFu},

                /* ownTccs */
                /* 31     0     63    32 */
                {0xFFFFFFFFu, 0xFFFFFFFFu},

                /* Resources reserved by Region 0 */
                /* resvdPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* resvdDmaChannels */
                /* 31                                         0  63                                                  32 */
                {DMA_CHANNEL_TO_EVENT_MAPPING_0, DMA_CHANNEL_TO_EVENT_MAPPING_1},

                /* resvdQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* resvdTccs */
                /* 31                                         0  63                                                  32 */
                {DMA_CHANNEL_TO_EVENT_MAPPING_0, DMA_CHANNEL_TO_EVENT_MAPPING_1},
            },

            {
                /* Resources owned by Region 1 */
                /* ownPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* ownDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* ownQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* ownTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* Resources reserved by Region 1 */
                /* resvdPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* resvdDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* resvdQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* resvdTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},
            },

            {
                /* Resources owned by Region 2 */
                /* ownPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* ownDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* ownQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* ownTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* Resources reserved by Region 2 */
                /* resvdPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* resvdDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* resvdQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* resvdTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},
            },

            {
                /* Resources owned by Region 3 */
                /* ownPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* ownDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* ownQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* ownTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* Resources reserved by Region 3 */
                /* resvdPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* resvdDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* resvdQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* resvdTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},
            },

            {
                /* Resources owned by Region 4 */
                /* ownPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* ownDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* ownQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* ownTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* Resources reserved by Region 4 */
                /* resvdPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* resvdDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* resvdQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* resvdTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},
            },

            {
                /* Resources owned by Region 5 */
                /* ownPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* ownDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* ownQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* ownTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* Resources reserved by Region 5 */
                /* resvdPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* resvdDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* resvdQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* resvdTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},
            },

            {
                /* Resources owned by Region 6 */
                /* ownPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* ownDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* ownQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* ownTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* Resources reserved by Region 6 */
                /* resvdPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* resvdDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* resvdQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* resvdTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},
            },

            {
                /* Resources owned by Region 7 */
                /* ownPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* ownDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* ownQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* ownTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* Resources reserved by Region 7 */
                /* resvdPaRAMSets */
                /* 31     0     63    32     95    64     127   96 */
                {
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 159  128     191  160     223  192     255  224 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 287  256     319  288     351  320     383  352 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    /* 415  384     447  416     479  448     511  480 */
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                    0x00000000u,
                },

                /* resvdDmaChannels */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},

                /* resvdQdmaChannels */
                /* 31     0 */
                {0x00000000u},

                /* resvdTccs */
                /* 31     0     63    32 */
                {0x00000000u, 0x00000000u},
            }}};

/* End of File */
