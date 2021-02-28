/*
 * edma3_rm_gbl_data.c
 *
 * Source file for the Resource Manager, for internal data structures.
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

/* Resource Manager Internal Header Files */
#include <ti/edma3/rm/edma3resmgr.h>

/**
 * Maximum Resource Manager Instances supported by the EDMA3 Package.
 * USE THE SAME VALUE FOR BOTH THE #DEFINE AND CONST UNSIGNED INT BELOW.
 */
/* This #define is needed for array declarations. */
#define MAX_EDMA3_RM_INSTANCES (8U)
/* This const is required to access this constant in other header files */
const uint32_t EDMA3_MAX_RM_INSTANCES = 8U;

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
#pragma DATA_SECTION(userInstInitConfigArray, ".fardata:.edma3Globals");
#endif
EDMA3_RM_InstanceInitConfig userInstInitConfigArray[EDMA3_MAX_EDMA3_INSTANCES][MAX_EDMA3_RM_INSTANCES];

/**
 * Handles of EDMA3 Resource Manager Instances.
 *
 * Used to maintain information of the EDMA3 RM Instances
 * for each HW controller.
 * There could be a maximum of EDMA3_MAX_RM_INSTANCES instances per
 * EDMA3 HW.
 */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(resMgrInstanceArray, ".fardata:.edma3Globals");
#endif
EDMA3_RM_Instance resMgrInstanceArray[EDMA3_MAX_EDMA3_INSTANCES][MAX_EDMA3_RM_INSTANCES];

/* These pointers will be used to refer the above mentioned arrays. */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(resMgrInstance, ".fardata:.edma3Globals");
#pragma DATA_SECTION(userInitConfig, ".fardata:.edma3Globals");
#endif
EDMA3_RM_Instance *resMgrInstance = (EDMA3_RM_Instance *)resMgrInstanceArray;
EDMA3_RM_InstanceInitConfig *userInitConfig = (EDMA3_RM_InstanceInitConfig *)userInstInitConfigArray;

/* Pointer to the above mentioned 2-D arrays, used for address calculation purpose */
#ifdef BUILD_C6XDSP
#pragma DATA_SECTION(ptrRMIArray, ".fardata:.edma3Globals");
#pragma DATA_SECTION(ptrInitCfgArray, ".fardata:.edma3Globals");
#endif
EDMA3_RM_Instance *ptrRMIArray = (EDMA3_RM_Instance *)resMgrInstanceArray;
EDMA3_RM_InstanceInitConfig *ptrInitCfgArray = (EDMA3_RM_InstanceInitConfig *)userInstInitConfigArray;
