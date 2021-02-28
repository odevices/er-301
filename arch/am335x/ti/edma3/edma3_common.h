/*
 * edma3_common.h
 *
 * EDMA3 common header providing generic defines/typedefs and debugging info.
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

#ifndef EDMA3_COMMON_H_
#define EDMA3_COMMON_H_

/***************************************************************\
* Standard Definition Header File For Null Definition *
\***************************************************************/
#include <stddef.h>

/***************************************************************\
* Standard Definition Header File For Bool Datatype *
\***************************************************************/
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** define to enable/disable Resource Manager debug messages*/
/* #define EDMA3_RM_DEBUG */

/** define to enable/disable EDMA3 Driver debug messages*/
/* #define EDMA3_DRV_DEBUG */

/** Debug mechanism used for Resource Manager */
#ifdef EDMA3_RM_DEBUG
#include <stdio.h>
#define EDMA3_RM_PRINTF printf
#endif

/** Debug mechanism used for EDMA Driver */
#ifdef EDMA3_DRV_DEBUG
#include <stdio.h>
#define EDMA3_DRV_PRINTF printf
#endif

/** Defines for boolean variables */
#ifndef TRUE
  /** TRUE */
  #define TRUE  (1U)
  /** FALSE */
  #define FALSE (0U)
#endif

/** EDMA3_RM Result - return value of a function  */
typedef int32_t             EDMA3_RM_Result;
/** EDMA3_DRV Result - return value of a function  */
typedef int32_t             EDMA3_DRV_Result;
/** EDMA3_Result - return value of a function for common functions of DRV and RM */
typedef int32_t             EDMA3_Result;

/** EDMA3 Resource Manager Result OK */
#define EDMA3_RM_SOK             (0)
/** EDMA3 Driver Result OK */
#define EDMA3_DRV_SOK            (0)

/**
 * EDMA3 Resource Manager Handle.
 * It will be returned from EDMA3_RM_open() and will be used to call
 * other Resource Manager APIs.
 */
typedef void    *EDMA3_RM_Handle;
/**
 * EDMA3 Driver Handle.
 * It will be returned from EDMA3_DRV_open() and will be used to call
 * other EDMA3 Driver APIs.
 */
typedef void    *EDMA3_DRV_Handle;

/**
 * OS specific Semaphore Handle.
 * Used to acquire/free the semaphore, used for sharing of resources
 * among multiple users.
 */
typedef void    *EDMA3_OS_Sem_Handle;

/** Blocking call without timeout */
#define EDMA3_OSSEM_NO_TIMEOUT    (-1)

/**
 * Defines used to support the maximum resources supported
 * by the EDMA3 controller. These are used to allocate the maximum
 * memory for different data structures of the EDMA3 Driver and Resource
 * Manager.
 */
/** Maximum EDMA3 Controllers on the SoC */
#define EDMA3_MAX_EDMA3_INSTANCES               (5U)
/** Maximum DMA channels supported by the EDMA3 Controller */
#define EDMA3_MAX_DMA_CH                        (64U)
/** Maximum QDMA channels supported by the EDMA3 Controller */
#define EDMA3_MAX_QDMA_CH                       (8U)
/** Maximum PaRAM Sets supported by the EDMA3 Controller */
#define EDMA3_MAX_PARAM_SETS                    (512U)
/** Maximum Logical channels supported by the EDMA3 Package */
#define EDMA3_MAX_LOGICAL_CH                    (EDMA3_MAX_DMA_CH + \
                                                 EDMA3_MAX_PARAM_SETS + \
                                                 EDMA3_MAX_QDMA_CH)
/** Maximum TCCs (Interrupt Channels) supported by the EDMA3 Controller */
#define EDMA3_MAX_TCC                           (64U)
/** Maximum Event Queues supported by the EDMA3 Controller */
#define EDMA3_MAX_EVT_QUE                       (8U)
/** Maximum Transfer Controllers supported by the EDMA3 Controller */
#define EDMA3_MAX_TC                            (8U)
/** Maximum Shadow Regions supported by the EDMA3 Controller */
#define EDMA3_MAX_REGIONS                       (8U)

/**
 * Maximum Words (4-bytes region) required for the book-keeping information
 * specific to the maximum possible DMA channels.
 */
#define EDMA3_MAX_DMA_CHAN_DWRDS                (EDMA3_MAX_DMA_CH / 32U)

/**
 * Maximum Words (4-bytes region) required for the book-keeping information
 * specific to the maximum possible QDMA channels.
 */
#define EDMA3_MAX_QDMA_CHAN_DWRDS               (1U)

/**
 * Maximum Words (4-bytes region) required for the book-keeping information
 * specific to the maximum possible PaRAM Sets.
 */
#define EDMA3_MAX_PARAM_DWRDS                   (EDMA3_MAX_PARAM_SETS / 32U)

/**
 * Maximum Words (4-bytes region) required for the book-keeping information
 * specific to the maximum possible TCCs.
 */
#define EDMA3_MAX_TCC_DWRDS                     (EDMA3_MAX_TCC / 32U)

/**
 * EDMA3 ISRs which need to be registered with the underlying OS by the user
 * (Not all TC error ISRs need to be registered, register only for the
 * available Transfer Controllers).
 */
/**  EDMA3 Completion Handler ISR Routine */
extern void lisrEdma3ComplHandler0 (uint32_t edma3InstanceId);

/**  EDMA3 CC Error Interrupt Handler ISR Routine */
extern void lisrEdma3CCErrHandler0 (uint32_t edma3InstanceId);

/**  EDMA3 TC0 Error Interrupt Handler ISR Routine */
extern void lisrEdma3TC0ErrHandler0(uint32_t edma3InstanceId);
/**  EDMA3 TC1 Error Interrupt Handler ISR Routine */
extern void lisrEdma3TC1ErrHandler0(uint32_t edma3InstanceId);
/**  EDMA3 TC2 Error Interrupt Handler ISR Routine */
extern void lisrEdma3TC2ErrHandler0(uint32_t edma3InstanceId);
/**  EDMA3 TC3 Error Interrupt Handler ISR Routine */
extern void lisrEdma3TC3ErrHandler0(uint32_t edma3InstanceId);
/**  EDMA3 TC4 Error Interrupt Handler ISR Routine */
extern void lisrEdma3TC4ErrHandler0(uint32_t edma3InstanceId);
/**  EDMA3 TC5 Error Interrupt Handler ISR Routine */
extern void lisrEdma3TC5ErrHandler0(uint32_t edma3InstanceId);
/**  EDMA3 TC6 Error Interrupt Handler ISR Routine */
extern void lisrEdma3TC6ErrHandler0(uint32_t edma3InstanceId);
/**  EDMA3 TC7 Error Interrupt Handler ISR Routine */
extern void lisrEdma3TC7ErrHandler0(uint32_t edma3InstanceId);

/**
 * Defines for the level of OS protection needed when calling
 * edma3OsProtectEntry()
 */
/** Protection from All Interrupts required */
#define EDMA3_OS_PROTECT_INTERRUPT                          1
/** Protection from scheduling required */
#define EDMA3_OS_PROTECT_SCHEDULER                          2
/** Protection from EDMA3 Transfer Completion Interrupt required */
#define EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION          3
/** Protection from EDMA3 CC Error Interrupt required */
#define EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR                 4
/** Protection from EDMA3 TC Error Interrupt required */
#define EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR                 5

/**
 * Critical section entry and exit functions (OS dependent) should be
 * implemented by the application for proper linking with the EDMA3 Driver
 * and/or EDMA3 Resource Manager. Without the definitions being provided,
 * the image won’t get linked properly.
 *
 * It is possible that for some regions of code, user needs ultimate
 * degree of protection where some or all external interrupts are blocked,
 * essentially locking out the CPU exclusively for the critical
 * section of code. On the other hand, user may wish to merely avoid
 * thread or task switch from occuring inside said region of code,
 * but he may wish to entertain ISRs to run if so required.
 *
 * Depending on the underlying OS, the number of levels of protection
 * offered may vary. At the least, these basic levels of protection are
 * supported --
 * - EDMA3_OS_PROTECT_INTERRUPT - Mask interrupts globally. This has
 *   real-time implications and must be used with descretion.

 * - EDMA3_OS_PROTECT_SCHEDULER - Only turns off Kernel scheduler
 *   completely, but still allows h/w interrupts from being serviced.

 * - EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION - Mask EDMA3 Transfer
      Completion Interrupt.

 * - EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR - Mask EDMA3 CC Error Interrupt.

 * - EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR - Mask EDMA3 TC Error Interrupt.

 * These APIs should be mandatorily implemented ONCE by the global
 * initialization routine or by the user itself.
 */

/**
 *  \brief  EDMA3 OS Protect Entry
 *
 *      This function saves the current state of protection in 'intState'
 *      variable passed by caller, if the protection level is
 *      EDMA3_OS_PROTECT_INTERRUPT. It then applies the requested level of
 *      protection.
 *      For EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION and
 *      EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR, variable 'intState' is ignored,
 *      and the requested interrupt is disabled.
 *      For EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR, '*intState' specifies the
 *      Transfer Controller number whose interrupt needs to be disabled.
 *
 * \param   edma3InstanceId is EDMA3 hardware instance id.
 * \param   level is numeric identifier of the desired degree of protection.
 * \param   intState is memory location where current state of protection is
 *      saved for future use while restoring it via edma3OsProtectExit() (Only
 *      for EDMA3_OS_PROTECT_INTERRUPT protection level).
 *
 * \return  None
 */
extern void edma3OsProtectEntry (uint32_t edma3InstanceId,
										int32_t level,
										uint32_t *intState);

/**
 *  \brief  EDMA3 OS Protect Exit
 *
 *      This function undoes the protection enforced to original state
 *      as is specified by the variable 'intState' passed, if the protection
 *      level is EDMA3_OS_PROTECT_INTERRUPT.
 *      For EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION and
 *      EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR, variable 'intState' is ignored,
 *      and the requested interrupt is enabled.
 *      For EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR, 'intState' specifies the
 *      Transfer Controller number whose interrupt needs to be enabled.
 *
 * \param   edma3InstanceId is EDMA3 hardware instance id.
 * \param   level is numeric identifier of the desired degree of protection.
 * \param   intState is original state of protection at time when the
 *      corresponding edma3OsProtectEntry() was called (Only
 *      for EDMA3_OS_PROTECT_INTERRUPT protection level).
 *
 * \return  None
 */
extern void edma3OsProtectExit (uint32_t edma3InstanceId,
										int32_t level,
										uint32_t intState);

/**
 * Counting Semaphore related functions (OS dependent) should be
 * implemented by the application for proper linking with the EDMA3
 * Driver and Resource Manager. The EDMA3 Resource Manager
 * uses these functions for proper sharing of resources (among various users)
 * and assume the implementation of these functions
 * to be provided by the application. Without the definitions being provided,
 * the image won’t get linked properly.
 */

/**
 *  \brief  EDMA3 OS Semaphore Take
 *
 *      This function takes a semaphore token if available.
 *      If a semaphore is unavailable, it blocks currently
 *      running thread in wait (for specified duration) for
 *      a free semaphore.
 *
 * \param   hSem [IN] is the handle of the specified semaphore
 * \param   mSecTimeout [IN] is wait time in milliseconds
 *
 * \return  EDMA3_DRV_Result if successful else a suitable error code
 */
extern EDMA3_Result edma3OsSemTake (EDMA3_OS_Sem_Handle hSem,
                                        int32_t mSecTimeout);

/**
 *  \brief  EDMA3 OS Semaphore Give
 *
 *      This function gives or relinquishes an already
 *      acquired semaphore token
 *
 * \param   hSem [IN] is the handle of the specified semaphore
 *
 * \return  EDMA3_DRV_Result if successful else a suitable error code
 */
extern EDMA3_Result edma3OsSemGive(EDMA3_OS_Sem_Handle hSem);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* _EDMA3_COMMON_H_ */
