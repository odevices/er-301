/**
 * \file   chipdb.h
 *
 * \brief  This file contains the Chip Database APIs. This API functions are
 *         used to get the device definition information.
 *
 *   Chip DB enables the SW to scale across similar devices. The SW can query
 *   various aspects of the device using the Chip DB API and can make
 *   runtime decision. Examples of the device attributes that can be queried:
 *
 *   - Is the given resource present?
 *   - How many instances of the given module (IP) is in the device
 *   - Maximum frequency of the processing units in the device
 *   - Minimum and Maximum junction temperature of the device
 *   - Packaging type
 *   - Device revision
 *   - Security Level
 *   - and others
 *
 *   A resource in the SOC is uniquely identified using the pair
 *   (moduleID, instance).
 *   The module ID is unique for a given IP. If the same IP is present
 *   in different SoCs, they will have the same module ID. That is, module
 *   list is a global list with unique ID assigned to the list elements.
 *
 *   An SoC is defined by a list of Modules and number of instance of each
 *   module (IP). If an IP is not present in the SoC, the number of
 *   instance is zero.
 *
 *   To reiterate, the pair (moduleID, instance) uniquely identifies a
 *   SOC resource.
 *
 *  \copyright Copyright (C) 2013 Texas Instruments Incorporated -
 *             http://www.ti.com/
 */

/*
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


#ifndef CHIPDB_H_
#define CHIPDB_H_


/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "chipdb_defs.h"


#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 * \brief Invalid Frequency
 */
#define CHIPDB_INVALID_FREQUENCY (uint32_t)0x0U

/**
 * \brief Invalid Temperature
 */
#define CHIPDB_INVALID_TEMPERATURE (int32_t)INT_MAX

/**
 * \brief Invalid Address
 */
#define CHIPDB_INVALID_ADDRESS (uint32_t)UINT32_MAX

/**
 * \brief Invalid Interrupt Number
 */
#define CHIPDB_INVALID_INTERRUPT_NUM          (UINT32_MAX)

/**
 * \brief Invalid instance Number
 */
#define CHIPDB_INVALID_INSTANCE_NUM           (-1)


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   Enable Chip DB.
 *          Chip DB is enabled by default and provides an option to
 *          enable/disable for debugging.
 */
void CHIPDBEnable(void);

/**
 * \brief   Disable Chip DB. When disabled the Chip DB APIs consider all
 *          resources in the device are available for use - no restriction
 *          is assumed.
 */
void CHIPDBDisable(void);


/**
 * \brief  The maximum number of instance of the given module
 *
 *         Note that this can be different than the actual number of
 *         instances in an SoC. For example, say an SOC has 3 instances
 *         of IP X but for backward compatibilty reasons they are given
 *         instance numbers IP_X_0, IP_X_1, IP_X_4. There are only 3
 *         instances of the IP but the maximum instance number is 4.
 *
 * \param  moduleID      Unique ID that identifies a Module (IP).
 *
 * \retval Max_instances    If module is present in the SoC, returns the
 *                          maximum number of instances of the module.
 *         0                If module is not present in the SoC.
 */
uint32_t CHIPDBModuleMaxInstanceNumber(const chipdbModuleID_t moduleID);

/**
 * \brief   Is the given resource present?
 *
 * \param   moduleID      Unique ID that identifies a Module (IP).
 * \param   instance      Module instance number.
 *
 * \retval  TRUE        If the resource is present in device and allowed to be
 *                      used
 * \retval  FALSE       Otherwise. Invalid input results in returning FALSE.
 *
 */
uint32_t CHIPDBIsResourcePresent(const chipdbModuleID_t moduleID,
                                 const uint32_t         instance);

/**
 * \brief   The maximum frequency allowed for the given module.
 *
 *          The device specific maximum frequency only applies to certain
 *          modules, mostly the processing units. The maximum frequency is
 *          returned only for these valid modules. CHIPDB_INVALID_FREQUENCY
 *          is returned otherwise.
 *
 * \param   moduleID      Unique ID that identifies a Module (IP).
 *
 * \retval  Max_freq                    If the module has a maximum as defined
 *                                      in the data manual, return the maximum
 *                                      frequency (in KHz) for the module
 * \retval  CHIPDB_INVALID_FREQUENCY    Otherwise.
 */
uint32_t CHIPDBResourceMaxFrequency(const chipdbModuleID_t moduleID);

/**
 * \brief   The maximum junction temperature allowed for the device.
 *
 * \retval  Max_Junction_Temp           If known, returns the maximum junction
 *                                      temperature (in centigrade) for the
 *                                      device.
 * \retval  CHIPDB_INVALID_TEMPERATURE  Otherwise.
 */
int32_t CHIPDBMaxJunctionTemperature(void);

/**
 * \brief   The minimum junction temperature allowed for the device
 *
 * \retval  Min_Junction_Temp           If known, returns the minimum junction
 *                                      temperature (in centigrade) for the
 *                                      device. This is normally a negative
 *                                      value.
 * \retval  CHIPDB_INVALID_TEMPERATURE  Otherwise.
 */
int32_t CHIPDBMinJunctionTemperature(void);

/**
 * \brief   The package type of the device.
 *
 * \retval  Package_type    Return value of type #chipdbPackageType_t
 */
chipdbPackageType_t CHIPDBPackageType(void);

/**
 * \brief   The device revision.
 *
 * \retval  Device_Revision     Device revisions from #chipdbDeviceRevision_t
 */
chipdbDeviceRevision_t CHIPDBDeviceRevision(void);

/**
 * \brief   The device security level.
 *
 * \retval  Security_level      Security levels from #chipdbSecurityLevel_t
 */
chipdbSecurityLevel_t CHIPDBSecurityLevel(void);

/**
 * \brief   Get the base address of the given resource.
 *
 *          Each processor shall have its own base address table in
 *          a separate file. For example, if a device has IPU and DSP then
 *          ipu_baseaddr.c and dsp_baseaddr.c contains the base address
 *          view from these processors respectively. The right base address
 *          definition file shall be included when building for each
 *          processor.
 *
 *          This API supports the based addresses as seen by the processor
 *          which is running the code. It is assumed that the base address
 *          as seen by another processor is not needed.
 *
 * \param   moduleID      Unique ID that identifies a Module (IP).
 * \param   instance      Module instance number.
 *
 * \retval  Base_address            For valid modules, returns the base address
 *                                  of the given resource.
 * \retval  CHIPDB_INVALID_ADDRESS  Otherwise.
 */
uint32_t CHIPDBBaseAddress(const chipdbModuleID_t moduleID,
                           const uint32_t         instance);

/**
 * \brief   The interrupt number of the INTC line
 *          The INTC of the processor where this code is running is returned.
 *
 * \param   moduleID      Unique ID that identifies a Module (IP).
 * \param   instance      Module instance number.
 * \param   intSignal     Interrupt source signal
 *
 * \retval  Interrupt_number        For valid modules, returns the interrupt
 *                                  number of the INTC line to which the given
 *                                  input interrupt signal is connected.
 * \retval  CHIPDB_INVALID_INTERRUPT_NUM  Otherwise.
 **/
uint32_t ChipDBInterruptNum(const chipdbModuleID_t moduleID,
                            const uint32_t         instance,
                            const uint32_t         intSignal);


#ifdef  __cplusplus
}
#endif /* ifdef  __cplusplus */

#endif /* ifndef CHIPDB_H_ */

