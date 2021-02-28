/*
 * bios6_edma3_drv_sample.h
 *
 * Header file for the sample application for the EDMA3 Driver.
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

#ifndef OD_EDMA3_DRV_H_
#define OD_EDMA3_DRV_H_

#include <stdio.h>
#include <ti/sysbios/knl/Semaphore.h>

/* Include EDMA3 Driver */
#include <ti/edma3/edma3_drv.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Cache line size on the underlying SoC. It needs to be modified
 * for different cache line sizes, if the Cache is Enabled.
 */
#define EDMA3_CACHE_LINE_SIZE_IN_BYTES (64U)
//#define EDMA3_CACHE_LINE_SIZE_IN_BYTES      (128U)

/* Error returned in case of buffers are not aligned on the cache boundary */
#define EDMA3_NON_ALIGNED_BUFFERS_ERROR (-1)

/* Error returned in case of data mismatch */
#define EDMA3_DATA_MISMATCH_ERROR (-2)

  /**
 * \brief   EDMA3 Initialization
 *
 * This function initializes the EDMA3 Driver for the given EDMA3 controller
 * and opens a EDMA3 driver instance. It internally calls EDMA3_DRV_create() and
 * EDMA3_DRV_open(), in that order.
 *
 * It also registers interrupt handlers for various EDMA3 interrupts like 
 * transfer completion or error interrupts.
 *
 *  \param  edma3Id 	[IN]		EDMA3 Controller Instance Id (Hardware
 *									instance id, starting from 0)
 *  \param  errorCode 	[IN/OUT]	Error code while opening DRV instance
 *  \return EDMA3_DRV_Handle: If successfully opened, the API will return the
 *                            associated driver's instance handle.
 */
  EDMA3_DRV_Handle edma3init(uint32_t edma3Id, EDMA3_DRV_Result *errorCode);

  /**
 * \brief   EDMA3 De-initialization
 *
 * This function de-initializes the EDMA3 Driver for the given EDMA3 controller
 * and closes the previously opened EDMA3 driver instance. It internally calls 
 * EDMA3_DRV_close and EDMA3_DRV_delete(), in that order.
 *
 * It also un-registers the previously registered interrupt handlers for various 
 * EDMA3 interrupts.
 *
 *  \param  edma3Id 	[IN]		EDMA3 Controller Instance Id (Hardware
 *									instance id, starting from 0)
 *  \param  hEdma		[IN]		EDMA3 Driver handle, returned while using
 *									edma3init().
 *  \return  EDMA3_DRV_SOK if success, else error code
 */
  EDMA3_DRV_Result edma3deinit(uint32_t edma3Id, EDMA3_DRV_Handle hEdma);

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
                                         uint32_t num_bytes);

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
                                    uint32_t num_bytes);

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
 * \param   hSem [OUT] is location to receive the handle to just created
 *      semaphore.
 * \return  EDMA3_DRV_SOK if successful, else a suitable error code.
 */
  EDMA3_DRV_Result edma3OsSemCreate(int32_t initVal,
                                    const Semaphore_Params *semParams,
                                    EDMA3_OS_Sem_Handle *hSem);

  /**
 * \brief   EDMA3 OS Semaphore Delete
 *
 *      This function deletes or removes the specified semaphore
 *      from the system. Associated dynamically allocated memory
 *      if any is also freed up.
 * \param   hSem [IN] handle to the semaphore to be deleted
 * \return  EDMA3_DRV_SOK if successful else a suitable error code
 */
  EDMA3_DRV_Result edma3OsSemDelete(EDMA3_OS_Sem_Handle hSem);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* OD_EDMA3_DRV_H_ */
