/*
 * sample_init.c
 *
 * Sample Initialization for the EDMA3 Driver for BIOS 6 based applications.
 * It should be MANDATORILY done once before EDMA3 usage.
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

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#ifdef __TMS470__
#else
#include <ti/sysbios/family/c64p/EventCombiner.h>
#endif

#include "edma3_drv.h"

/** @brief EDMA3 Driver Instance specific Semaphore handle */
extern EDMA3_OS_Sem_Handle semHandle[EDMA3_MAX_EDMA3_INSTANCES];

/**  To Register the ISRs with the underlying OS, if required. */
extern void registerEdma3Interrupts(uint32_t edma3Id);
/**  To Unregister the ISRs with the underlying OS, if previously registered. */
extern void unregisterEdma3Interrupts(uint32_t edma3Id);

/* To find out the DSP# */
extern uint16_t determineProcId(void);

/**
 * To check whether the global EDMA3 configuration is required or not.
 * It should be done ONCE by any of the masters present in the system.
 * This function checks whether the global configuration is required by the
 * current master or not. In case of many masters, it should be done only
 * by one of the masters. Hence this function will return TRUE only once
 * and FALSE for all other masters. 
 */
extern uint16_t isGblConfigRequired(uint32_t dspNum);

/**
 * DSP instance number on which the executable is running. Its value is
 * determined by reading the processor specific register DNUM.
 */
uint32_t dsp_num;

/**
 * Shadow Region on which the executable is runnig. Its value is populated
 * with the DSP Instance Number here in this case.
 */
uint32_t region_id;

/* Number of EDMA3 controllers present in the system */
extern const uint32_t numEdma3Instances;

/* External Global Configuration Structure */
extern EDMA3_DRV_GblConfigParams sampleEdma3GblCfgParams[EDMA3_MAX_EDMA3_INSTANCES];

/* External Instance Specific Configuration Structure */
extern EDMA3_DRV_InstanceInitConfig sampleInstInitConfig[EDMA3_MAX_EDMA3_INSTANCES][EDMA3_MAX_REGIONS];

#ifdef DMA_XBAR_AVAILABLE
extern EDMA3_DRV_Result sampleInitXbarEvt(EDMA3_DRV_Handle hEdma, uint32_t edma3Id);
#endif

/**
 * \brief   EDMA3 Initialization
 *
 * This function initializes the EDMA3 Driver and registers the
 * interrupt handlers.
 *
  * \return  EDMA3_DRV_SOK if success, else error code
 */
EDMA3_DRV_Handle edma3init(uint32_t edma3Id, EDMA3_DRV_Result *errorCode)
{
	EDMA3_DRV_Result edma3Result = EDMA3_DRV_E_INVALID_PARAM;
	Semaphore_Params semParams;
	EDMA3_DRV_GblConfigParams *globalConfig = NULL;
	EDMA3_DRV_InstanceInitConfig *instanceConfig = NULL;
	EDMA3_DRV_InitConfig initCfg;
	EDMA3_RM_MiscParam miscParam;
	EDMA3_DRV_Handle hEdma = NULL;

	if ((edma3Id >= numEdma3Instances) || (errorCode == NULL))
	{
		hEdma = NULL;
	}
	else
	{
		/* DSP instance number */
		dsp_num = determineProcId();

		globalConfig = &sampleEdma3GblCfgParams[edma3Id];

		/* Configure it as master, if required */
		miscParam.isSlave = isGblConfigRequired(dsp_num);
		edma3Result = EDMA3_DRV_create(edma3Id, globalConfig,
									   (void *)&miscParam);

		if (edma3Result == EDMA3_DRV_SOK)
		{
			/**
		* Driver Object created successfully.
		* Create a semaphore now for driver instance.
		*/
			Semaphore_Params_init(&semParams);

			initCfg.drvSemHandle = NULL;
			edma3Result = edma3OsSemCreate((int32_t)1, &semParams, &initCfg.drvSemHandle);
		}

		if (edma3Result == EDMA3_DRV_SOK)
		{
			/* Save the semaphore handle for future use */
			semHandle[edma3Id] = initCfg.drvSemHandle;

			/* configuration structure for the Driver */
			instanceConfig = &sampleInstInitConfig[edma3Id][dsp_num];

			initCfg.isMaster = TRUE;
			/* Choose shadow region according to the DSP# */
			initCfg.regionId = (EDMA3_RM_RegionId)dsp_num;
			/*Saving the regionId for using it in the sample_cs.c file */
			region_id = (EDMA3_RM_RegionId)dsp_num;
			/* Driver instance specific config NULL */
			initCfg.drvInstInitConfig = instanceConfig;

			initCfg.gblerrCb = NULL;
			initCfg.gblerrData = NULL;

			/* Open the Driver Instance */
			hEdma = EDMA3_DRV_open(edma3Id, (const EDMA3_DRV_InitConfig *)&initCfg, &edma3Result);
		}

#ifdef DMA_XBAR_AVAILABLE
		{
			if ((hEdma != NULL) && (edma3Result == EDMA3_DRV_SOK))
			{
				edma3Result = sampleInitXbarEvt(hEdma, edma3Id);
			}
		}
#endif
		if ((hEdma != NULL) && (edma3Result == EDMA3_DRV_SOK))
		{
			/**
		* Register Interrupt Handlers for various interrupts
		* like transfer completion interrupt, CC error
		* interrupt, TC error interrupts etc, if required.
		*/
			registerEdma3Interrupts(edma3Id);
		}

		*errorCode = edma3Result;
	}
	return hEdma;
}

/**
 * \brief   EDMA3 De-initialization
 *
 * This function removes the EDMA3 Driver instance and unregisters the
 * interrupt handlers.
 *
  * \return  EDMA3_DRV_SOK if success, else error code
 */
EDMA3_DRV_Result edma3deinit(uint32_t edma3Id, EDMA3_DRV_Handle hEdma)
{
	EDMA3_DRV_Result edma3Result = EDMA3_DRV_E_INVALID_PARAM;

	/* Unregister Interrupt Handlers first */
	unregisterEdma3Interrupts(edma3Id);

	/* Delete the semaphore */
	edma3Result = edma3OsSemDelete(semHandle[edma3Id]);

	if (EDMA3_DRV_SOK == edma3Result)
	{
		/* Make the semaphore handle as NULL. */
		semHandle[edma3Id] = NULL;

		/* Now, close the EDMA3 Driver Instance */
		edma3Result = EDMA3_DRV_close(hEdma, NULL);
	}

	if (EDMA3_DRV_SOK == edma3Result)
	{
		/* Now, delete the EDMA3 Driver Object */
		edma3Result = EDMA3_DRV_delete(edma3Id, NULL);
	}

	return edma3Result;
}

/* End of File */
