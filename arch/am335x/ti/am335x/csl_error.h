/*  ============================================================================
 *  Copyright (c) Texas Instruments Incorporated 2002-2019
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
/** ============================================================================
 *   @file  csl/csl_error.h
 *
 *
 *   @brief  This file contains the Register Desciptions for ERRORs
 *
 */

/* ---- File: <csl_error.h> ---- */
#ifndef CSL_ERROR_H
#define CSL_ERROR_H

/** ===========================================================================
 *
 * @defgroup CSL_ERROR_CODES CSL ERROR CODES
 *
 * ============================================================================
 */

/**
@defgroup CSL_GLOBAL_ERROR_CODES CSL Global Error Codes
@ingroup CSL_ERROR_CODES
*/

/**
@defgroup CSL_PERIPHERAL_ERROR_CODES CSL Peripheral Error Codes
@ingroup CSL_ERROR_CODES
*/

/* Below Error codes are Global across all CSL Modules. */

/** ===========================================================================
 *  @addtogroup CSL_GLOBAL_ERROR_CODES
    @{
 * ============================================================================
 */

/** ---------------------------------------------------------------------------
 * @brief CSL API returns success
 *
 * ----------------------------------------------------------------------------
 */
#define CSL_SOK                 (1)

/** ---------------------------------------------------------------------------
 * @brief CSL API returns Generic Failure
 *
 * ----------------------------------------------------------------------------
 */
#define CSL_ESYS_FAIL           (-(int32_t) 1)

/** ---------------------------------------------------------------------------
 * @brief CSL API returns Peripheral resource is already in use
 *
 * ----------------------------------------------------------------------------
 */
#define CSL_ESYS_INUSE          (-(int32_t) 2)

/** ---------------------------------------------------------------------------
 * @brief CSL API returns Encountered a shared I/O(XIO) pin conflict
 *
 * ----------------------------------------------------------------------------
 */
#define CSL_ESYS_XIO            (-(int32_t) 3)

/** ---------------------------------------------------------------------------
 * @brief CSL API returns Encoutered CSL system resource overflow
 *
 * ----------------------------------------------------------------------------
 */
#define CSL_ESYS_OVFL           (-(int32_t) 4)

/** ---------------------------------------------------------------------------
 * @brief CSL API returns       Handle passed to CSL was invalid
 *
 * ----------------------------------------------------------------------------
 */
#define CSL_ESYS_BADHANDLE      (-(int32_t) 5)

/** ---------------------------------------------------------------------------
 * @brief CSL API returns       invalid parameters
 *
 * ----------------------------------------------------------------------------
 */
#define CSL_ESYS_INVPARAMS      (-(int32_t) 6)

/** ---------------------------------------------------------------------------
 * @brief CSL API returns       invalid command
 *
 * ----------------------------------------------------------------------------
 */
#define CSL_ESYS_INVCMD         (-(int32_t) 7)

/** ---------------------------------------------------------------------------
 * @brief CSL API returns       invalid query
 *
 * ----------------------------------------------------------------------------
 */
#define CSL_ESYS_INVQUERY       (-(int32_t) 8)

/** ---------------------------------------------------------------------------
 * @brief CSL API returns       action not supported
 *
 * ----------------------------------------------------------------------------
 */
#define CSL_ESYS_NOTSUPPORTED   (-(int32_t) 9)

/* @} */

/* Peripheral IDs */
#define CSL_MCBSP_ID            (7)
#define CSL_GPIO_ID             (8)
#define CSL_DFE_ID              (9)

/* Error codes individual to various modules. */

/** ===========================================================================
 *  @addtogroup CSL_PERIPHERAL_ERROR_CODES
    @{
 * ============================================================================
 */
/* Error code for DMA, individual error would be assigned as
 * eg: #define CSL_E<Peripheral name>_<error code>  CSL_EDMA_FIRST - 1
 */


#define CSL_EPWM_FIRST      (-( ((CSL_PWM_ID + 1) << 5 ) + 1 ))
#define CSL_EPWM_LAST       (-( (CSL_PWM_ID + 1) << 6 ))

#define CSL_EUART_FIRST     (-( ((CSL_UART_ID + 1) << 5 ) + 1 ))
#define CSL_EUART_LAST      (-( (CSL_UART_ID + 1) << 6 ))

#define CSL_ESPI_FIRST      (-( ((CSL_SPI_ID + 1) << 5 ) + 1 ))
#define CSL_ESPI_LAST       (-( (CSL_SPI_ID + 1) << 6 ))

#define CSL_EATA_FIRST      (-( ((CSL_ATA_ID + 1) << 5 ) + 1 ))
#define CSL_EATA_LAST       (-( (CSL_ATA_ID + 1) << 6 ))

#define CSL_EMMCSD_FIRST    (-( ((CSL_MMCSD_ID + 1) << 5 ) + 1 ))
#define CSL_EMMCSD_LAST     (-( (CSL_MMCSD_ID + 1) << 6 ))

#define CSL_EVLYNQ_FIRST    (-( ((CSL_VLYNQ_ID + 1) << 5 ) + 1 ))
#define CSL_EVLYNQ_LAST     (-( (CSL_VLYNQ_ID + 1) << 6 ))

#define CSL_EMCBSP_FIRST    (-( ((CSL_MCBSP_ID + 1) << 5 ) + 1 ))
#define CSL_EMCBSP_LAST     (-( (CSL_MCBSP_ID + 1) << 6 ))

#define CSL_EI2C_FIRST      (-( ((CSL_I2C_ID + 1) << 5 ) + 1 ))
#define CSL_EI2C_LAST       (-( (CSL_I2C_ID + 1) << 6 ))

#define CSL_EGPIO_FIRST     (-( ((CSL_GPIO_ID + 1) << 5 ) + 1 ))
#define CSL_EGPIO_LAST      (-( (CSL_GPIO_ID + 1) << 6 ))

#define CSL_EMS_FIRST       (-( ((CSL_MS_ID + 1) << 5 ) + 1 ))
#define CSL_EMS_LAST        (-( (CSL_MS_ID + 1) << 6 ))

#define CSL_EINTC_FIRST     (-( ((CSL_INTC_ID + 1) << 5 ) + 1 ))
#define CSL_EINTC_LAST      (-( (CSL_INTC_ID + 1) << 6 ))

#define CSL_EEMIF_FIRST     (-( ((CSL_EMIF_ID + 1) << 5 ) + 1 ))
#define CSL_EEMIF_LAST      (-( (CSL_EMIF_ID + 1) << 6 ))

#define CSL_EPLLC_FIRST     (-( ((CSL_PLLC_ID + 1) << 5 ) + 1 ))
#define CSL_EPLLC_LAST      (-( (CSL_PLLC_ID + 1) << 6 ))

#define CSL_EDDR_FIRST      (-( ((CSL_DDR_ID + 1) << 5 ) + 1 ))
#define CSL_EDDR_LAST       (-( (CSL_DDR_ID + 1) << 6 ))

#define CSL_EEDMA_FIRST     (-( ((CSL_EDMA_ID + 1) << 5 ) + 1 ))
#define CSL_EEDMA_LAST      (-( (CSL_EDMA_ID + 1) << 6 ))

#define CSL_EDFE_FIRST      (-( ((CSL_DFE_ID + 1) << 5 ) + 1 ))
#define CSL_EDFE_LAST       (-( (CSL_DFE_ID + 1) << 6 ))

/* @} */

#endif /* CSL_ERROR_H */

