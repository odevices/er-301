
/*
 * sample_cs.c
 *
 * Sample functions showing the implementation of critical section entry/exit
 * routines and various semaphore related routines (all BIOS6 depenedent).
 * These implementations MUST be provided by the user / application, using the
 * EDMA3 driver, for its correct functioning.
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
#define GCC_BUILD
#define BUILD_AM335X_A8

#ifdef GCC_BUILD
#if (defined(BUILD_TDA2XX_MPU) || defined(BUILD_DRA72X_MPU) || defined(BUILD_AM572X_MPU) || defined(BUILD_AM571X_MPU))
#include <ti/sysbios/family/arm/a15/Cache.h>
#elif (defined(BUILD_CENTAURUS_A8) || defined(BUILD_AM335X_A8))
#include <ti/sysbios/family/arm/a8/Cache.h>
#elif defined(BUILD_AM437X_A9)
#include <ti/sysbios/family/arm/a9/Cache.h>
#endif
#else
#include <ti/sysbios/hal/Cache.h>
#endif
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include "edma3_drv.h"

extern uint32_t ccXferCompInt[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_REGIONS];
extern uint32_t ccErrorInt[EDMA3_MAX_EDMA3_INSTANCES];
extern uint32_t tcErrorInt[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_TC];

/**
 * Shadow Region on which the executable is running. Its value is
 * populated with the DSP Instance Number here in this case.
 */
extern uint32_t region_id;

/**
 * \brief   EDMA3 OS Protect Entry
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
 * \param   level is numeric identifier of the desired degree of protection.
 * \param   intState is memory location where current state of protection is
 *      saved for future use while restoring it via edma3OsProtectExit() (Only
 *      for EDMA3_OS_PROTECT_INTERRUPT protection level).
 * \return  None
 */
void edma3OsProtectEntry(uint32_t edma3InstanceId,
                         int32_t level, uint32_t *intState)
{
    if (((level == EDMA3_OS_PROTECT_INTERRUPT) || (level == EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR)) && (intState == NULL))
    {
        /* Nothing to be done here */
    }
    else
    {
        switch (level)
        {
        /* Disable all (global) interrupts */
        case EDMA3_OS_PROTECT_INTERRUPT:
            *intState = Hwi_disable();
            break;

        /* Disable scheduler */
        case EDMA3_OS_PROTECT_SCHEDULER:
            Task_disable();
            break;

        /* Disable EDMA3 transfer completion interrupt only */
        case EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION:
            Hwi_disableInterrupt(ccXferCompInt[edma3InstanceId][region_id]);
            break;

        /* Disable EDMA3 CC error interrupt only */
        case EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR:
            Hwi_disableInterrupt(ccErrorInt[edma3InstanceId]);
            break;

        /* Disable EDMA3 TC error interrupt only */
        case EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR:
            switch (*intState)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                /* Fall through... */
                /* Disable the corresponding interrupt */
                Hwi_disableInterrupt(tcErrorInt[edma3InstanceId][*intState]);
                break;

            default:
                break;
            }

            break;

        default:
            break;
        }
    }
    return;
}

/**
 * \brief   EDMA3 OS Protect Exit
 *
 *      This function undoes the protection enforced to original state
 *      as is specified by the variable 'intState' passed, if the protection
 *      level is EDMA3_OS_PROTECT_INTERRUPT.
 *      For EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION and
 *      EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR, variable 'intState' is ignored,
 *      and the requested interrupt is enabled.
 *      For EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR, 'intState' specifies the
 *      Transfer Controller number whose interrupt needs to be enabled.
 * \param   level is numeric identifier of the desired degree of protection.
 * \param   intState is original state of protection at time when the
 *      corresponding edma3OsProtectEntry() was called (Only
 *      for EDMA3_OS_PROTECT_INTERRUPT protection level).
 * \return  None
 */
void edma3OsProtectExit(uint32_t edma3InstanceId,
                        int32_t level, uint32_t intState)
{
    switch (level)
    {
    /* Enable all (global) interrupts */
    case EDMA3_OS_PROTECT_INTERRUPT:
        Hwi_restore(intState);
        break;

    /* Enable scheduler */
    case EDMA3_OS_PROTECT_SCHEDULER:
        Task_enable();
        break;

    /* Enable EDMA3 transfer completion interrupt only */
    case EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION:
        Hwi_enableInterrupt(ccXferCompInt[edma3InstanceId][region_id]);
        break;

    /* Enable EDMA3 CC error interrupt only */
    case EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR:
        Hwi_enableInterrupt(ccErrorInt[edma3InstanceId]);
        break;

    /* Enable EDMA3 TC error interrupt only */
    case EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR:
        switch ((int32_t)intState)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            /* Fall through... */
            /* Enable the corresponding interrupt */
            Hwi_enableInterrupt(tcErrorInt[edma3InstanceId][intState]);
            break;

        default:
            break;
        }

        break;

    default:
        break;
    }
}

/**
 *  \brief   EDMA3 Cache Invalidate
 *
 *  This function invalidates the D cache.
 *
 *  \param  mem_start_ptr [IN]      Starting address of memory.
 *                                  Please note that this should be
 *                                  aligned according to the cache line size.
 *  \param  num_bytes [IN]          length of buffer
 *  \return  EDMA3_DRV_SOK if success, else error code in case of error
 *          or non-alignment of buffers.
 *
 * Note: This function is required if the buffer is in DDR.
 * For other cases, where buffer is NOT in DDR, user
 * may or may not require the below implementation and
 * should modify it according to her need.
 */
EDMA3_DRV_Result Edma3_CacheInvalidate(uint32_t mem_start_ptr,
                                       uint32_t num_bytes)
{
    EDMA3_DRV_Result cacheInvResult = EDMA3_DRV_SOK;

    /* Verify whether the start address is cache aligned or not */
    if ((mem_start_ptr & (EDMA3_CACHE_LINE_SIZE_IN_BYTES - 1U)) != 0)
    {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("\r\n Cache : Memory is not %d bytes alinged\r\n",
                         EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif
        cacheInvResult = EDMA3_NON_ALIGNED_BUFFERS_ERROR;
    }
    else
    {
        Cache_inv((Ptr)mem_start_ptr, num_bytes, Cache_Type_ALL, (Bool)TRUE);
    }

    return cacheInvResult;
}

/**
 * \brief   EDMA3 Cache Flush
 *
 *  This function flushes (cleans) the Cache
 *
 *  \param  mem_start_ptr [IN]      Starting address of memory.
 *                                  Please note that this should be
 *                                  aligned according to the cache line size.
 *  \param  num_bytes [IN]          length of buffer
 *  \return  EDMA3_DRV_SOK if success, else error code in case of error
 *          or non-alignment of buffers.
 *
 * Note: This function is required if the buffer is in DDR.
 * For other cases, where buffer is NOT in DDR, user
 * may or may not require the below implementation and
 * should modify it according to her need.
 */
EDMA3_DRV_Result Edma3_CacheFlush(uint32_t mem_start_ptr,
                                  uint32_t num_bytes)
{
    EDMA3_DRV_Result cacheFlushResult = EDMA3_DRV_SOK;

    /* Verify whether the start address is cache aligned or not */
    if ((mem_start_ptr & (EDMA3_CACHE_LINE_SIZE_IN_BYTES - 1U)) != 0)
    {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("\r\n Cache : Memory is not %d bytes alinged\r\n",
                         EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif
        cacheFlushResult = EDMA3_NON_ALIGNED_BUFFERS_ERROR;
    }
    else
    {
        Cache_wb((Ptr)mem_start_ptr, num_bytes, Cache_Type_ALL, (Bool)TRUE);
    }

    return cacheFlushResult;
}

/**
  * Counting Semaphore related functions (OS dependent) should be
  * called/implemented by the application. A handle to the semaphore
  * is required while opening the driver/resource manager instance.
  */

/**
 * \brief   EDMA3 OS Semaphore Create
 *
 *      This function creates a counting semaphore with specified
 *      attributes and initial value. It should be used to create a semaphore
 *      with initial value as '1'. The semaphore is then passed by the user
 *      to the EDMA3 driver/RM for proper sharing of resources.
 * \param   initVal [IN] is initial value for semaphore
 * \param   semParams [IN] is the semaphore attributes.
 * \param   hSem [OUT] is location to recieve the handle to just created
 *      semaphore
 * \return  EDMA3_DRV_SOK if succesful, else a suitable error code.
 */
EDMA3_DRV_Result edma3OsSemCreate(int32_t initVal,
                                  const Semaphore_Params *semParams,
                                  EDMA3_OS_Sem_Handle *hSem)
{
    EDMA3_DRV_Result semCreateResult = EDMA3_DRV_SOK;

    if (NULL == hSem)
    {
        semCreateResult = EDMA3_DRV_E_INVALID_PARAM;
    }
    else
    {
        *hSem = (EDMA3_OS_Sem_Handle)Semaphore_create(initVal, semParams, NULL);
        if ((*hSem) == NULL)
        {
            semCreateResult = EDMA3_DRV_E_SEMAPHORE;
        }
    }

    return semCreateResult;
}

/**
 * \brief   EDMA3 OS Semaphore Delete
 *
 *      This function deletes or removes the specified semaphore
 *      from the system. Associated dynamically allocated memory
 *      if any is also freed up.
 * \param   hSem [IN] handle to the semaphore to be deleted
 * \return  EDMA3_DRV_SOK if succesful else a suitable error code
 */
EDMA3_DRV_Result edma3OsSemDelete(EDMA3_OS_Sem_Handle hSem)
{
    EDMA3_DRV_Result semDeleteResult = EDMA3_DRV_SOK;

    if (NULL == hSem)
    {
        semDeleteResult = EDMA3_DRV_E_INVALID_PARAM;
    }
    else
    {
        Semaphore_delete((Semaphore_Handle *)&hSem);
    }

    return semDeleteResult;
}

/**
 * \brief   EDMA3 OS Semaphore Take
 *
 *      This function takes a semaphore token if available.
 *      If a semaphore is unavailable, it blocks currently
 *      running thread in wait (for specified duration) for
 *      a free semaphore.
 * \param   hSem [IN] is the handle of the specified semaphore
 * \param   mSecTimeout [IN] is wait time in milliseconds
 * \return  EDMA3_Result if successful else a suitable error code
 */
EDMA3_Result edma3OsSemTake(EDMA3_OS_Sem_Handle hSem, int32_t mSecTimeout)
{
    EDMA3_Result semTakeResult = EDMA3_DRV_SOK;

    if (NULL == hSem)
    {
        semTakeResult = EDMA3_DRV_E_INVALID_PARAM;
    }
    else
    {
        if ((Semaphore_pend((Semaphore_Handle)hSem, (uint32_t)mSecTimeout)) == FALSE)
        {
            semTakeResult = EDMA3_DRV_E_SEMAPHORE;
        }
    }

    return semTakeResult;
}

/**
 * \brief   EDMA3 OS Semaphore Give
 *
 *      This function gives or relinquishes an already
 *      acquired semaphore token
 * \param   hSem [IN] is the handle of the specified semaphore
 * \return  EDMA3_Result if successful else a suitable error code
 */
EDMA3_Result edma3OsSemGive(EDMA3_OS_Sem_Handle hSem)
{
    EDMA3_Result semGiveResult = EDMA3_DRV_SOK;

    if (NULL == hSem)
    {
        semGiveResult = EDMA3_DRV_E_INVALID_PARAM;
    }
    else
    {
        Semaphore_post((Semaphore_Handle)hSem);
    }

    return semGiveResult;
}

/* End of File */
