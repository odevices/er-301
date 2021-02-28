/*
 *  Copyright (C) 2013 - 2018 Texas Instruments Incorporated - http://www.ti.com/
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
/**
 *  \ingroup CSL_IP_MODULE
 *  \defgroup CSL_UART Uart
 *
 *  @{
 */
/**
 * \file   uart.h
 *
 * \brief  This file contains the prototypes of device abstraction layer APIs
 *         corresponding to the UART module. This also contains necessary
 *         structure, enum and macro definitions and the APIs are consolidated
 *         to make them more functional.
 *
 *  \details Programming sequence of UART is as follows:
 *           -# Enable Module Clocks for UART
 *           -# Configure the Pinmux for the UART instance required
 *           -# Perform a module reset of the UART instance through the API
 *              UARTModuleReset
 *           -# Configure the Fifo settings required through the following API
 *              UARTFifoConfig
 *           -# Set the Baud rate required for the UART and compute divisor
 *              value using the API UARTDivisorValCompute
 *           -# Configure the Line Characteristics like the Word Length, Parity,
 *              Stop Bits through the API UARTLineCharacConfig
 *           -# Disable the Break Control functionality through the following
 *              API UARTBreakCtl
 *           -# Configure the UART Interrupts like Receive and Transmit through
 *              the following APIs UARTIntEnable and UARTIntDisable
 *              and UARTInt2Enable and UARTInt2Disable
 *           -# Configure the UART DMA feature through the following API
 *              UARTDMAEnable and UARTDMADisable
 *           -# To read and write a character from the FIFO use the following
 *              APIs UARTFIFOCharGet UARTFIFOCharPut
 *           -# UART wakeup Events can be configured using the following APIs
 *              UARTWakeUpEventsEnable and UARTWakeUpEventsDisable
 *           -# FlowControl can be enabled through Hardware or Software methods
 *              -# Hardware Flow control can be controlled by the following
 *                 API UARTAutoRTSAutoCTSControl to control
 *                 Auto-RTS and Auto-CTS feature respectively
 *              -# Software Flow control combinations can be configured through
 *                 the following API UARTSoftwareFlowCtrlOptSet
 *              -# The XON and XOFF characters for software flow control are
 *                 configured using XON related API's
 *              -# The trigger levels for the selected Flow control method can
 *                 be configured through UARTFIFOTrigLvlGranControl API
 *           -# AutoBaud
 *              -# The other AutoBaud Characteristics can be read using the
 *                 following APIs UARTAutobaudParityGet UARTAutobaudWordLenGet
 *                 UARTAutobaudSpeedGet
 */

#ifndef UART_H_
#define UART_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/am335x/cslr_uart.h>
#include <ti/am335x/hw_uart.h>

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/** \name The size of the Transmitter FIFO. */
#define TX_FIFO_SIZE (64)

/** \name The size of the Receiver FIFO. */
#define RX_FIFO_SIZE (64)

/**
** \name Values to configure the Operating modes of UART.
* @{
*/
#define UART16x_OPER_MODE (UART_MDR1_MODE_SELECT_UART16X)
#define UART_SIR_OPER_MODE (UART_MDR1_MODE_SELECT_SIR)
#define UART16x_AUTO_BAUD_OPER_MODE (UART_MDR1_MODE_SELECT_UART16XAUTO)
#define UART13x_OPER_MODE (UART_MDR1_MODE_SELECT_UART13X)
#define UART_MIR_OPER_MODE (UART_MDR1_MODE_SELECT_MIR)
#define UART_FIR_OPER_MODE (UART_MDR1_MODE_SELECT_FIR)
#define UART_CIR_OPER_MODE (UART_MDR1_MODE_SELECT_CIR)
#define UART_DISABLED_MODE (UART_MDR1_MODE_SELECT_MASK)
/** @} */

/**
** \name Values to control the Line characteristics.
* @{
*/

/** Break condition generation controls. */
#define UART_BREAK_COND_DISABLE (UART_LCR_BREAK_EN_BREAK_EN_VALUE_0 \
                                 << UART_LCR_BREAK_EN_SHIFT)
#define UART_BREAK_COND_ENABLE (UART_LCR_BREAK_EN_BREAK_EN_VALUE_1 \
                                << UART_LCR_BREAK_EN_SHIFT)
        /** @} */

        /** \name Values to control parity feature.
 * @{
 */

#define UART_PARITY_REPR_1 (UART_LCR_PARITY_TYPE2_MASK |                \
                            (UART_LCR_PARITY_TYPE1_PARITY_TYPE1_VALUE_0 \
                             << UART_LCR_PARITY_TYPE1_SHIFT) |          \
                            UART_LCR_PARITY_EN_MASK)

#define UART_PARITY_REPR_0 (UART_LCR_PARITY_TYPE2_MASK |                \
                            (UART_LCR_PARITY_TYPE1_PARITY_TYPE1_VALUE_1 \
                             << UART_LCR_PARITY_TYPE1_SHIFT) |          \
                            UART_LCR_PARITY_EN_MASK)

#define UART_ODD_PARITY ((                                              \
                             UART_LCR_PARITY_TYPE1_PARITY_TYPE1_VALUE_0 \
                             << UART_LCR_PARITY_TYPE1_SHIFT) |          \
                         UART_LCR_PARITY_EN_MASK)

#define UART_EVEN_PARITY ((                                              \
                              UART_LCR_PARITY_TYPE1_PARITY_TYPE1_VALUE_1 \
                              << UART_LCR_PARITY_TYPE1_SHIFT) |          \
                          UART_LCR_PARITY_EN_MASK)

#define UART_PARITY_NONE ( \
    UART_LCR_PARITY_EN_PARITY_EN_VALUE_0 << UART_LCR_PARITY_EN_SHIFT)

        /** \brief Number of Stop Bits per frame. */

#define UART_FRAME_NUM_STB_1 (UART_LCR_NB_STOP_NB_STOP_VALUE_0 \
                              << UART_LCR_NB_STOP_SHIFT)
#define UART_FRAME_NUM_STB_1_5_2 (UART_LCR_NB_STOP_NB_STOP_VALUE_1 \
                                  << UART_LCR_NB_STOP_SHIFT)

        /** \brief Word Length per frame. */

#define UART_FRAME_WORD_LENGTH_5 ( \
    UART_LCR_CHAR_LENGTH_CHAR_LENGTH_VALUE_0)
#define UART_FRAME_WORD_LENGTH_6 ( \
    UART_LCR_CHAR_LENGTH_CHAR_LENGTH_VALUE_1)
#define UART_FRAME_WORD_LENGTH_7 ( \
    UART_LCR_CHAR_LENGTH_CHAR_LENGTH_VALUE_2)
#define UART_FRAME_WORD_LENGTH_8 ( \
    UART_LCR_CHAR_LENGTH_CHAR_LENGTH_VALUE_3)
        /** @} */

        /**
** \name Values associated with setting the Trigger Levels and DMA mode
*  selection.
* @{
*/

        /** \brief Values for trigger level for the Receiver FIFO. */

#define UART_FCR_RX_TRIG_LVL_8 (UART_FCR_RX_FIFO_TRIG_8CHAR << UART_FCR_RX_FIFO_TRIG_SHIFT)
#define UART_FCR_RX_TRIG_LVL_16 (UART_FCR_RX_FIFO_TRIG_16CHAR << UART_FCR_RX_FIFO_TRIG_SHIFT)
#define UART_FCR_RX_TRIG_LVL_56 (UART_FCR_RX_FIFO_TRIG_56CHAR << UART_FCR_RX_FIFO_TRIG_SHIFT)
#define UART_FCR_RX_TRIG_LVL_60 (UART_FCR_RX_FIFO_TRIG_60CHAR << UART_FCR_RX_FIFO_TRIG_SHIFT)

        /** \brief Values for the trigger level for the Transmitter FIFO. */

#define UART_FCR_TX_TRIG_LVL_8 (UART_FCR_TX_FIFO_TRIG_8SPACES << UART_FCR_TX_FIFO_TRIG_SHIFT)
#define UART_FCR_TX_TRIG_LVL_16 (UART_FCR_TX_FIFO_TRIG_16SPACES << UART_FCR_TX_FIFO_TRIG_SHIFT)
#define UART_FCR_TX_TRIG_LVL_32 (UART_FCR_TX_FIFO_TRIG_32SPACES << UART_FCR_TX_FIFO_TRIG_SHIFT)
#define UART_FCR_TX_TRIG_LVL_56 (UART_FCR_TX_FIFO_TRIG_56SPACES << UART_FCR_TX_FIFO_TRIG_SHIFT)

        /** \brief Values corresponding to DMA mode selection. */

#define UART_DMA_MODE_0_ENABLE ( \
    UART_SCR_DMA_MODE_2_DMA_MODE_2_VALUE_0)
#define UART_DMA_MODE_1_ENABLE ( \
    UART_SCR_DMA_MODE_2_DMA_MODE_2_VALUE_1)
#define UART_DMA_MODE_2_ENABLE ( \
    UART_SCR_DMA_MODE_2_DMA_MODE_2_VALUE_2)
#define UART_DMA_MODE_3_ENABLE ( \
    UART_SCR_DMA_MODE_2_DMA_MODE_2_VALUE_3)

/**
 * \brief Values used to choose the path for configuring the DMA Mode.
 * DMA Mode could be configured either through FCR or SCR.
*/
#define UART_DMA_EN_PATH_FCR ( \
    UART_SCR_DMA_MODE_CTL_DMA_MODE_CTL_VALUE_0)
#define UART_DMA_EN_PATH_SCR ( \
    UART_SCR_DMA_MODE_CTL_DMA_MODE_CTL_VALUE_1)
        /** @} */

        /**
** \name Values related to enabling/disabling of Interrupts.
* @{
*/

        /** \brief Values for enabling/disabling the interrupts of UART. */

#define UART_INT_CTS (UART_IER_CTS_IT_MASK)
#define UART_INT_RTS (UART_IER_RTS_IT_MASK)
#define UART_INT_XOFF (UART_IER_XOFF_IT_MASK)
#define UART_INT_SLEEPMODE (UART_IER_SLEEP_MODE_MASK)
#define UART_INT_MODEM_STAT (UART_IER_MODEM_STS_IT_MASK)
#define UART_INT_LINE_STAT (UART_IER_LINE_STS_IT_MASK)
#define UART_INT_THR (UART_IER_THR_IT_MASK)
#define UART_INT_RHR_CTI (UART_IER_RHR_IT_MASK)

#define UART_INT2_RX_EMPTY (UART_IER2_EN_RXFIFO_EMPTY_MASK)
#define UART_INT2_TX_EMPTY (UART_IER2_EN_TXFIFO_EMPTY_MASK)
        /** @} */

        /**
** \name Values related to Line Status information.
* @{
*/

        /** \brief Values pertaining to UART Line Status information. */

#define UART_FIFO_PE_FE_BI_DETECTED (UART_LSR_RX_FIFO_STS_MASK)
#define UART_BREAK_DETECTED_ERROR (UART_LSR_RX_BI_MASK)
#define UART_FRAMING_ERROR (UART_LSR_RX_FE_MASK)
#define UART_PARITY_ERROR (UART_LSR_RX_PE_MASK)
#define UART_OVERRUN_ERROR (UART_LSR_RX_OE_MASK)
        /** @} */

        /**
** \name Values related to status of Interrupt souces.
* @{
*/

        /** \brief Values pertaining to status of UART Interrupt sources. */

#define UART_INTID_MODEM_STAT (UART_IIR_IT_TYPE_IT_TYPE_VALUE_0 \
                               << UART_IIR_IT_TYPE_SHIFT)
#define UART_INTID_TX_THRES_REACH (UART_IIR_IT_TYPE_IT_TYPE_VALUE_1 \
                                   << UART_IIR_IT_TYPE_SHIFT)
#define UART_INTID_RX_THRES_REACH (UART_IIR_IT_TYPE_IT_TYPE_VALUE_2 \
                                   << UART_IIR_IT_TYPE_SHIFT)
#define UART_INTID_RX_LINE_STAT_ERROR (UART_IIR_IT_TYPE_IT_TYPE_VALUE_3 \
                                       << UART_IIR_IT_TYPE_SHIFT)
#define UART_INTID_CHAR_TIMEOUT (UART_IIR_IT_TYPE_IT_TYPE_VALUE_6 \
                                 << UART_IIR_IT_TYPE_SHIFT)
#define UART_INTID_XOFF_SPEC_CHAR_DETECT (UART_IIR_IT_TYPE_IT_TYPE_VALUE_8 \
                                          << UART_IIR_IT_TYPE_SHIFT)
#define UART_INTID_MODEM_SIG_STATE_CHANGE (UART_IIR_IT_TYPE_IT_TYPE_VALUE_10 \
                                           << UART_IIR_IT_TYPE_SHIFT)

/** \brief Values indicating the UART Interrupt pending status. */
#define UART_INT_PENDING (0U)
#define UART_N0_INT_PENDING (1U)
        /** @} */

        /**
** \name Values pertaining to control of Enhanced Features.
* @{
*/

        /** \brief Values for controlling Auto-CTS and Auto-RTS features. */

#define UART_AUTO_CTS_ENABLE ((uint32_t)(                                     \
                                  UART_EFR_AUTO_CTS_EN_AUTO_CTS_EN_U_VALUE_1) \
                              << (UART_EFR_AUTO_CTS_EN_SHIFT))
#define UART_AUTO_CTS_DISABLE ((uint32_t)(                                     \
                                   UART_EFR_AUTO_CTS_EN_AUTO_CTS_EN_U_VALUE_0) \
                               << (UART_EFR_AUTO_CTS_EN_SHIFT))

#define UART_AUTO_RTS_ENABLE ((uint32_t)(                                     \
                                  UART_EFR_AUTO_RTS_EN_AUTO_RTS_EN_U_VALUE_1) \
                              << (UART_EFR_AUTO_RTS_EN_SHIFT))
#define UART_AUTO_RTS_DISABLE ((uint32_t)(                                     \
                                   UART_EFR_AUTO_RTS_EN_AUTO_RTS_EN_U_VALUE_0) \
                               << (UART_EFR_AUTO_RTS_EN_SHIFT))

        /** \brief Values to enable/disable detection of Special Character. */

#define UART_SPECIAL_CHAR_DETECT_ENABLE (UART_EFR_SPECIAL_CHAR_DETECT_MASK)
#define UART_SPECIAL_CHAR_DETECT_DISABLE ( \
    UART_EFR_SPECIAL_CHAR_DETECT_SPECIAL_CHAR_DETECT_U_VALUE_0)

        /** \brief Values to configure the options for Software Flow Control. */

#define UART_NO_SOFTWARE_FLOW_CONTROL ((                                                      \
                                           UART_EFR_SW_FLOW_CONTROL_SW_FLOW_CONTROL_U_VALUE_0 \
                                           << UART_EFR_SW_FLOW_CONTROL_TX_SHIFT) |            \
                                       (UART_EFR_SW_FLOW_CONTROL_SW_FLOW_CONTROL_U_VALUE_0    \
                                        << UART_EFR_SW_FLOW_CONTROL_RX_SHIFT))

#define UART_TX_RX_XON1_XOFF1 ((                                                      \
                                   UART_EFR_SW_FLOW_CONTROL_SW_FLOW_CONTROL_U_VALUE_2 \
                                   << UART_EFR_SW_FLOW_CONTROL_TX_SHIFT) |            \
                               (UART_EFR_SW_FLOW_CONTROL_SW_FLOW_CONTROL_U_VALUE_2    \
                                << UART_EFR_SW_FLOW_CONTROL_RX_SHIFT))

#define UART_TX_RX_XON2_XOFF2 ((                                                      \
                                   UART_EFR_SW_FLOW_CONTROL_SW_FLOW_CONTROL_U_VALUE_1 \
                                   << UART_EFR_SW_FLOW_CONTROL_TX_SHIFT) |            \
                               (UART_EFR_SW_FLOW_CONTROL_SW_FLOW_CONTROL_U_VALUE_1    \
                                << UART_EFR_SW_FLOW_CONTROL_RX_SHIFT))

#define UART_TX_RX_XON1_XOFF1_XON2_XOFF2 ((                                                      \
                                              UART_EFR_SW_FLOW_CONTROL_SW_FLOW_CONTROL_U_VALUE_3 \
                                              << UART_EFR_SW_FLOW_CONTROL_TX_SHIFT) |            \
                                          (UART_EFR_SW_FLOW_CONTROL_SW_FLOW_CONTROL_U_VALUE_3    \
                                           << UART_EFR_SW_FLOW_CONTROL_RX_SHIFT))
/** @} */

/**
** \name Values corresponding to Mode Definition Register 2(MDR2).
* @{
*/

/** \brief Values to enable/disable Pulse shaping for UART. */
#define UART_PULSE_NORMAL ( \
    UART_MDR2_UART_PULSE_UART_PULSE_VALUE_0 << UART_MDR2_UART_PULSE_SHIFT)
#define UART_PULSE_SHAPING ( \
    UART_MDR2_UART_PULSE_UART_PULSE_VALUE_1 << UART_MDR2_UART_PULSE_SHIFT)
/** @} */

/**
** \name Values corresponding to Mode Definition Register 3(MDR3).
* @{
*/

/** \brief Values used to control the method of setting the TX DMA Threshold value. */
#define UART_TX_DMA_THRESHOLD_64 (UART_MDR3_SET_DMA_TX_THRESHOLD_64 \
                                  << UART_MDR3_SET_DMA_TX_THRESHOLD_SHIFT)
#define UART_TX_DMA_THRESHOLD_REG (UART_MDR3_SET_DMA_TX_THRESHOLD_REG \
                                   << UART_MDR3_SET_DMA_TX_THRESHOLD_SHIFT)
        /** @} */

        /**
** \name Macros related to control and status of Modem Signals.
* @{
*/

        /** \brief Values to enable/disable XON any feature. */

#define UART_XON_ANY_ENABLE (UART_MCR_XON_EN_XON_EN_VALUE_1 << UART_MCR_XON_EN_SHIFT)
#define UART_XON_ANY_DISABLE (UART_MCR_XON_EN_XON_EN_VALUE_0 << UART_MCR_XON_EN_SHIFT)

        /** \brief Values to enable/disable Loopback mode of operation. */

#define UART_LOOPBACK_MODE_ENABLE ( \
    (uint32_t)UART_MCR_LOOPBACK_EN_LOOPBACK_EN_VALUE_1 << UART_MCR_LOOPBACK_EN_SHIFT)
#define UART_LOOPBACK_MODE_DISABLE ( \
    (uint32_t)UART_MCR_LOOPBACK_EN_LOOPBACK_EN_VALUE_0 << UART_MCR_LOOPBACK_EN_SHIFT)

        /** \brief Macros used to force the Modem Control lines to active/inactive states. */

#define UART_DCD_CONTROL (UART_MCR_CD_STS_CH_MASK)
#define UART_RI_CONTROL (UART_MCR_RI_STS_CH_MASK)
#define UART_RTS_CONTROL (UART_MCR_RTS_MASK)
#define UART_DTR_CONTROL (UART_MCR_DTR_MASK)

        /** \brief Values that indicate the values on Modem Control lines. */

#define UART_DCD_VALUE (UART_MSR_NCD_STS_MASK)
#define UART_RI_VALUE (UART_MSR_NRI_STS_MASK)
#define UART_DSR_VALUE (UART_MSR_NDSR_STS_MASK)
#define UART_CTS_VALUE (UART_MSR_NCTS_STS_MASK)

        /** \brief Values used to detect the changes in Modem Control lines. */

#define UART_DCD_STS_CHANGED (UART_MSR_DCD_STS_MASK)
#define UART_RI_STS_CHANGED (UART_MSR_RI_STS_MASK)
#define UART_DSR_STS_CHANGED (UART_MSR_DSR_STS_MASK)
#define UART_CTS_STS_CHANGED (UART_MSR_CTS_STS_MASK)
/** @} */

/**
** \name Values related to the control and status of Supplementary registers.
* @{
*/

/**
 * \brief Values used to enable/disable a granularity of 1 for TX
 * and RX FIFO triggerlevels.
*/
#define UART_RX_TRIG_LVL_GRAN_1_DISABLE ( \
    UART_SCR_RX_TRIG_GRANU1_RX_TRIG_GRANU1_VALUE_0 << UART_SCR_RX_TRIG_GRANU1_SHIFT)
#define UART_RX_TRIG_LVL_GRAN_1_ENABLE ( \
    UART_SCR_RX_TRIG_GRANU1_RX_TRIG_GRANU1_VALUE_1 << UART_SCR_RX_TRIG_GRANU1_SHIFT)

#define UART_TX_TRIG_LVL_GRAN_1_DISABLE ( \
    UART_SCR_TX_TRIG_GRANU1_TX_TRIG_GRANU1_VALUE_0 << UART_SCR_TX_TRIG_GRANU1_SHIFT)
#define UART_TX_FIFO_LVL_GRAN_1_ENABLE ( \
    UART_SCR_TX_TRIG_GRANU1_TX_TRIG_GRANU1_VALUE_1 << UART_SCR_TX_TRIG_GRANU1_SHIFT)

        /** \brief Value used to enable/disable DSRn interrupt. */

#define UART_DSRn_INT_DISABLE (UART_SCR_DSR_IT_DSR_IT_VALUE_0 << UART_SCR_DSR_IT_SHIFT)
#define UART_DSRn_INT_ENABLE (UART_SCR_DSR_IT_DSR_IT_VALUE_1 << UART_SCR_DSR_IT_SHIFT)

        /** \brief Values to control the module Wake-Up rights for RX,
 * CTSn and DSRn pins. */

#define UART_RX_CTS_DSR_WAKEUP_DISABLE ( \
    UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE_RX_CTS_DSR_WAKE_UP_ENABLE_VALUE_0 << UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE_SHIFT)
#define UART_RX_CTS_DSR_WAKEUP_ENABLE ( \
    UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE_RX_CTS_DSR_WAKE_UP_ENABLE_VALUE_1 << UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE_SHIFT)

        /** \brief Values to control the THR interrupt modes. */

#define UART_THR_INT_NORMAL ( \
    UART_SCR_TX_EMPTY_CTL_IT_TX_EMPTY_CTL_IT_VALUE_0 << UART_SCR_TX_EMPTY_CTL_IT_SHIFT)
#define UART_THR_INT_FIFO_TSR_EMPTY ( \
    UART_SCR_TX_EMPTY_CTL_IT_TX_EMPTY_CTL_IT_VALUE_1 << UART_SCR_TX_EMPTY_CTL_IT_SHIFT)

        /** \brief Values to control the DMA counter reset features. */

#define UART_DMA_CNTR_NO_RESET_FIFO_RESET ( \
    UART_SSR_DMA_COUNTER_RST_DMA_COUNTER_RST_VALUE_0 << UART_SSR_DMA_COUNTER_RST_SHIFT)
#define UART_DMA_CNTR_RESET_FIFO_RESET ( \
    UART_SSR_DMA_COUNTER_RST_DMA_COUNTER_RST_VALUE_1 << UART_SSR_DMA_COUNTER_RST_SHIFT)

        /** \brief Values indicating the Falling edge status on RX, CTSn and DSRn pins. */

#define UART_RX_CTS_DSR_NO_FALL_EDGE ( \
    UART_SSR_RX_CTS_DSR_WAKE_UP_STS_RX_CTS_DSR_WAKE_UP_STS_VALUE_0 << UART_SSR_RX_CTS_DSR_WAKE_UP_STS_SHIFT)
#define UART_RX_CTS_DSR_FALL_EDGE ( \
    UART_SSR_RX_CTS_DSR_WAKE_UP_STS_RX_CTS_DSR_WAKE_UP_STS_VALUE_1 << UART_SSR_RX_CTS_DSR_WAKE_UP_STS_SHIFT)

        /** \brief Values indicating the filled status of TX FIFO. */

#define UART_TX_FIFO_NOT_FULL ( \
    UART_SSR_TX_FIFO_FULL_TX_FIFO_FULL_VALUE_0)
#define UART_TX_FIFO_FULL ( \
    UART_SSR_TX_FIFO_FULL_TX_FIFO_FULL_VALUE_1)
        /** @} */

        /**
** \name Values related to Auxilliary Control Register(ACREG).
* @{
*/

        /** \brief Values to set/clear the SD pin. */

#define UART_SD_PIN_LOW (UART_ACREG_SD_MOD_SD_MOD_VALUE_1 \
                         << UART_ACREG_SD_MOD_SHIFT)
#define UART_SD_PIN_HIGH (UART_ACREG_SD_MOD_SD_MOD_VALUE_0 \
                          << UART_ACREG_SD_MOD_SHIFT)
        /** @} */

        /**
** \name Values controlling System Configuration functions.
* @{
*/

        /** \brief Values controlling Power Management Request/Acknowledgement modes. */

#define UART_IDLEMODE_FORCE_IDLE ( \
    UART_SYSC_IDLEMODE_IDLEMODE_VALUE_0 << UART_SYSC_IDLEMODE_SHIFT)
#define UART_IDLEMODE_NO_IDLE ( \
    UART_SYSC_IDLEMODE_IDLEMODE_VALUE_1 << UART_SYSC_IDLEMODE_SHIFT)
#define UART_IDLEMODE_SMART_IDLE ( \
    UART_SYSC_IDLEMODE_IDLEMODE_VALUE_2 << UART_SYSC_IDLEMODE_SHIFT)
#define UART_IDLEMODE_SMART_IDLE_WAKEUP ( \
    UART_SYSC_IDLEMODE_IDLEMODE_VALUE_3 << UART_SYSC_IDLEMODE_SHIFT)

        /** \brief Values enabling/disabling WakeUp capability. */

#define UART_WAKEUP_ENABLE ( \
    UART_SYSC_ENAWAKEUP_ENAWAKEUP_VALUE_1 << UART_SYSC_ENAWAKEUP_SHIFT)
#define UART_WAKEUP_DISABLE ( \
    UART_SYSC_ENAWAKEUP_ENAWAKEUP_VALUE_0 << UART_SYSC_ENAWAKEUP_SHIFT)

/** \brief Values to enable /disable Autoidle mode. */
#define UART_AUTO_IDLE_MODE_ENABLE ( \
    UART_SYSC_AUTOIDLE_AUTOIDLE_VALUE_1)
#define UART_AUTO_IDLE_MODE_DISABLE ( \
    UART_SYSC_AUTOIDLE_AUTOIDLE_VALUE_0)
/** @} */

/**
 * \name Values configuring Wake-up modes for the UART in Wake-Up Enable
 * Register.
 * @{
*/

/** \brief Values that enable/disable Wake-Up generation
 * ability for various signals. */
#define UART_WAKEUP_TX_INTERRUPT (UART_WER_EVENT_7_TX_WAKEUP_EN_MASK)
#define UART_WAKEUP_RLS_INTERRUPT ( \
    UART_WER_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT_MASK)
#define UART_WAKEUP_RHR_INTERRUPT ( \
    UART_WER_EVENT_5_RHR_INTERRUPT_MASK)
#define UART_WAKEUP_RX_ACTIVITY (UART_WER_EVENT_4_RX_ACTIVITY_MASK)
#define UART_WAKEUP_DCD_ACTIVITY ( \
    UART_WER_EVENT_3_DCD_CD_ACTIVITY_MASK)
#define UART_WAKEUP_RI_ACTIVITY (UART_WER_EVENT_2_RI_ACTIVITY_MASK)
#define UART_WAKEUP_DSR_ACTIVITY (UART_WER_EVENT_1_DSR_ACTIVITY_MASK)
#define UART_WAKEUP_CTS_ACTIVITY (UART_WER_EVENT_0_CTS_ACTIVITY_MASK)
        /** @} */

        /**
 * \name Values indicating the line characteristics of UART Autobauding mode
 * communication.
 * @{
*/

        /** \brief Values indicating the parity in UART Autobauding mode. */

#define UART_AUTOBAUD_NO_PARITY ( \
    UART_UASR_PARITY_TYPE_PARITY_TYPE_U_VALUE_0 << UART_UASR_PARITY_TYPE_SHIFT)
#define UART_AUTOBAUD_PARITY_SPACE ( \
    UART_UASR_PARITY_TYPE_PARITY_TYPE_U_VALUE_1 << UART_UASR_PARITY_TYPE_SHIFT)
#define UART_AUTOBAUD_EVEN_PARITY ( \
    UART_UASR_PARITY_TYPE_PARITY_TYPE_U_VALUE_2 << UART_UASR_PARITY_TYPE_SHIFT)
#define UART_AUTOBAUD_ODD_PARITY ( \
    UART_UASR_PARITY_TYPE_PARITY_TYPE_U_VALUE_3 << UART_UASR_PARITY_TYPE_SHIFT)

        /** \brief Values indicating the word length in UART Autobaud mode. */

#define UART_AUTOBAUD_CHAR_LENGTH_7 ( \
    UART_UASR_BIT_BY_CHAR_BIT_BY_CHAR_U_VALUE_0 << UART_UASR_BIT_BY_CHAR_SHIFT)
#define UART_AUTOBAUD_CHAR_LENGTH_8 ( \
    UART_UASR_BIT_BY_CHAR_BIT_BY_CHAR_U_VALUE_1 << UART_UASR_BIT_BY_CHAR_SHIFT)

        /** \brief Values indicating the baud rate in UART Autobaud mode. */

#define UART_AUTOBAUD_NO_SPEED_IDEN (UART_UASR_SPEED_SPEED_VALUE_0)
#define UART_AUTOBAUD_SPEED_115200 (UART_UASR_SPEED_SPEED_VALUE_1)
#define UART_AUTOBAUD_SPEED_57600 (UART_UASR_SPEED_SPEED_VALUE_2)
#define UART_AUTOBAUD_SPEED_38400 (UART_UASR_SPEED_SPEED_VALUE_3)
#define UART_AUTOBAUD_SPEED_28800 (UART_UASR_SPEED_SPEED_VALUE_4)
#define UART_AUTOBAUD_SPEED_19200 (UART_UASR_SPEED_SPEED_VALUE_5)
#define UART_AUTOBAUD_SPEED_14400 (UART_UASR_SPEED_SPEED_VALUE_6)
#define UART_AUTOBAUD_SPEED_9600 (UART_UASR_SPEED_SPEED_VALUE_7)
#define UART_AUTOBAUD_SPEED_4800 (UART_UASR_SPEED_SPEED_VALUE_8)
#define UART_AUTOBAUD_SPEED_2400 (UART_UASR_SPEED_SPEED_VALUE_9)
#define UART_AUTOBAUD_SPEED_1200 (UART_UASR_SPEED_SPEED_VALUE_10)
/** @} */

/**
** \name Miscellaneous macros.
* @{
*/
/**
 * \brief Values used to choose the trigger level granularity.
*/
#define UART_TRIG_LVL_GRANULARITY_4 ((uint32_t)0x0000U)
#define UART_TRIG_LVL_GRANULARITY_1 ((uint32_t)0x0001U)

        /** \brief Values to be used while switching between register
 * configuration modes. */

#define UART_REG_CONFIG_MODE_A (uint32_t)(0x0080)
#define UART_REG_CONFIG_MODE_B (uint32_t)(0x00BF)
#define UART_REG_OPERATIONAL_MODE (uint32_t)(0x007F)

/** \brief Parameterized macro to configure the FIFO settings. */
#define UART_FIFO_CONFIG(txGra, rxGra, txTrig, rxTrig, txClr, rxClr, dmaEnPath, \
                         dmaMode)                                               \
        (((uint32_t)(txGra & 0xFU) << (uint32_t)26U) |                          \
         ((uint32_t)(rxGra & 0xFU) << (uint32_t)22U) |                          \
         ((uint32_t)(txTrig & 0xFFU) << (uint32_t)14U) |                        \
         ((uint32_t)(rxTrig & 0xFFU) << (uint32_t)6U) |                         \
         ((uint32_t)(txClr & 0x1U) << (uint32_t)5U) |                           \
         ((uint32_t)(rxClr & 0x1U) << (uint32_t)4U) |                           \
         ((uint32_t)(dmaEnPath & 0x1U) << (uint32_t)3U) |                       \
         (uint32_t)(dmaMode & 0x7U))

/** \brief Parameterized macro used to determine a value to be written to FCR.*/
#define UART_FCR_PROGRAM(rxFIFOTrig, txFIFOTrig, dmaMode, txClr, rxClr, fifoEn) \
        (((uint32_t)(((uint32_t)(rxFIFOTrig)&0x3U) << 6)) |                     \
         ((uint32_t)(((uint32_t)(txFIFOTrig)&0x3U) << 4)) |                     \
         (((uint32_t))(((uint32_t)(dmaMode)&0x1U) << 3)) |                      \
         (((uint32_t))(((uint32_t)(txClr)&0x1U) << 2)) |                        \
         ((uint32_t)(((uint32_t)(rxClr)&0x1U) << 1)) |                          \
         ((uint32_t)(fifoEn)&0x1U))

#define UART_FIFO_CONFIG_TXGRA ((uint32_t)0xFU << 26)
#define UART_FIFO_CONFIG_RXGRA ((uint32_t)0xFU << 22)
#define UART_FIFO_CONFIG_TXTRIG ((uint32_t)0xFFU << 14)
#define UART_FIFO_CONFIG_RXTRIG ((uint32_t)0xFFU << 6)
#define UART_FIFO_CONFIG_TXCLR ((uint32_t)0x1U << 5)
#define UART_FIFO_CONFIG_RXCLR ((uint32_t)0x1U << 4)
#define UART_FIFO_CONFIG_DMAENPATH ((uint32_t)0x1U << 3)
#define UART_FIFO_CONFIG_DMAMODE ((uint32_t)0x7U << 0)

/** \brief Over-sampling rate for MIR mode used to obtain the Divisor Values. */
#define UART_MIR_OVERSAMPLING_RATE_41 ((uint32_t)41U)
#define UART_MIR_OVERSAMPLING_RATE_42 ((uint32_t)42U)

#define UART_FCR_TX_FIFO_TRIG_8SPACES ((uint32_t)0x0U)
#define UART_FCR_TX_FIFO_TRIG_16SPACES ((uint32_t)0x1U)
#define UART_FCR_TX_FIFO_TRIG_32SPACES ((uint32_t)0x2U)
#define UART_FCR_TX_FIFO_TRIG_56SPACES ((uint32_t)0x3U)

/**\brief used in uart console init function*/
#define UART_LCR_STB_SHIFT ((uint32_t)0x00000002U)
#define UART_LCR_PEN_SHIFT ((uint32_t)0x00000003U)
#define UART_LCR_BC_SHIFT ((uint32_t)0x00000006U)

#define UART_LCR_PARITY_EN_DISABLE ((uint32_t)0x0U)

#define UART_MDR1_MODE_SELECT_CIR ((uint32_t)0x6U)
#define UART_MDR1_MODE_SELECT_DISABLED ((uint32_t)0x7U)
#define UART_MDR1_MODE_SELECT_FIR ((uint32_t)0x5U)
#define UART_MDR1_MODE_SELECT_MIR ((uint32_t)0x4U)
#define UART_MDR1_MODE_SELECT_SIR ((uint32_t)0x1U)
#define UART_MDR1_MODE_SELECT_UART13X ((uint32_t)0x3U)
#define UART_MDR1_MODE_SELECT_UART16X ((uint32_t)0x0U)
#define UART_MDR1_MODE_SELECT_UART16XAUTO ((uint32_t)0x2U)

#define UART_FCR_RX_FIFO_TRIG_16CHAR ((uint32_t)0x1U)
#define UART_FCR_RX_FIFO_TRIG_56CHAR ((uint32_t)0x2U)
#define UART_FCR_RX_FIFO_TRIG_60CHAR ((uint32_t)0x3U)
#define UART_FCR_RX_FIFO_TRIG_8CHAR ((uint32_t)0x0U)

#define UART_EFR_SW_FLOW_CONTROL_RX_SHIFT ((uint32_t)0x0U)
#define UART_EFR_SW_FLOW_CONTROL_TX_SHIFT ((uint32_t)0x2U)

#define UART_MDR3_SET_DMA_TX_THRESHOLD_64 ((uint32_t)0x0U)
#define UART_MDR3_SET_DMA_TX_THRESHOLD_REG ((uint32_t)0x1U)
        /** @} */

        /* ========================================================================== */
        /*                          Function Declarations                             */
        /* ========================================================================== */

        /**
 * \brief  This API configures the operating mode for the UART instance.
 *         The different operating modes are:
 *         - UART(16x, 13x, 16x Auto-Baud)\n
 *         - IrDA(SIR, MIR, FIR)\n
 *         - CIR\n
 *         - Disabled state(default state)\n
 *
 * \param   baseAddr   Memory address of the UART instance being used.
 * \param   modeFlag  A value which holds the mode number. This mode
 *                    number is referred from the MODESELECT field in MDR1.
 *
 * 'modeFlag' can take one of the following values:
 *  UART16x_OPER_MODE : to switch to UART 16x operating mode\n
 *  UART_SIR_OPER_MODE : to swith to IrDA SIR operating mode\n
 *  UART16x_AUTO_BAUD_OPER_MODE : to switch to UART 16x Auto Baud operating
 *   mode\n
 *  UART13x_OPER_MODE : to switch to UART 13x operating mode\n
 *  UART_MIR_OPER_MODE : to switch to IrDA MIR operating mode\n
 *  UART_FIR_OPER_MODE : to switch to IrDA FIR operating mode\n
 *  UART_CIR_OPER_MODE : to switch to CIR operating mode\n
 *  UART_DISABLED_MODE : to switch to Disabled state\n
 *
 * \return  The mode number in the MODESELECT field of MDR1 before it
 *          was  modified.
 */
        extern uint32_t UARTOperatingModeSelect(uint32_t baseAddr,
                                                uint32_t modeFlag);

        /**
 * \brief   This API computes the divisor value for the specified operating
 *          mode. Not part of this API, the divisor value returned is written
 *          to the Divisor Latches to configure the Baud Rate.
 *
 * \param   moduleClk        The frequency of the input clock to the UART module
 * \param   modeFlag         A value which represents the current operating mode
 * \param   baudRate         The required baud rate of communication in bits
 *                           per second(bps)
 * \param   mirOverSampRate  Over-sampling rate for MIR mode.This is applicable
 *                           only when MIR mode of operation is chosen.
 *                           Otherwise, this value is not considered.
 *
 * 'modeFlag' can take one of the following values:\n
 * - UART16x_OPER_MODE - indicating 16x operating mode\n
 * - UART13x_OPER_MODE - indicating 13x operating mode\n
 * - UART_SIR_OPER_MODE - indicating SIR operating mode\n
 * - UART_MIR_OPER_MODE - indicating MIR operating mode\n
 * - UART_FIR_OPER_MODE - indicating FIR operating mode\n
 *
 * 'mirOverSampRate' can take one of the following values:
 * - UART_MIR_OVERSAMPLING_RATE_41 - for an over-sampling rate of 41\n
 * - UART_MIR_OVERSAMPLING_RATE_42 - for an over-smapling rate of 42\n
 *
 * \return The divisor value computed for the specified mode.
 *
 * \note   Refer to the section in the user guide that specifies the baud rate
 *         computation method to find the supported values of baud rates.
 */
        extern uint32_t UARTDivisorValCompute(uint32_t moduleClk,
                                              uint32_t baudRate,
                                              uint32_t modeFlag,
                                              uint32_t mirOverSampRate);

        /**
 * \brief  This API is used to write the specified divisor value to Divisor
 *         Latch registers DLL and DLH.
 *
 * \param  baseAddr       Memory address of the UART instance being used.
 * \param  divisorValue  The 14-bit value whose least 8 bits go to DLL
 *                       and highest 6 bits go to DLH.
 *
 * \return A concatenated value of DLH and DLL registers(DLH:DLL, a 14-bit
 *         value) before they are modified in the current API.
 */
        extern uint32_t UARTDivisorLatchWrite(uint32_t baseAddr,
                                              uint32_t divisorValue);

        /**
 * \brief  This API enables write access to Divisor Latch registers DLL and
 *         DLH.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 *
 *
 * \return None.
 */
        extern void UARTDivisorLatchEnable(uint32_t baseAddr);

        /**
 * \brief  This API disables write access to Divisor Latch registers DLL and
 *         DLH.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 *
 * \return None.
 *
 * \note   Disabling write access to Divisor Latch Registers enables access to
 *         MCR, FCR, IER, BLR, EBLR, RHR registers.
 */
        extern void UARTDivisorLatchDisable(uint32_t baseAddr);

        /**
 * \brief  This API configures the specified Register Configuration mode for
 *         the UART.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 * \param  modeFlag  This specifies the register configuration mode to be
 *                   enabled.
 *
 * \return The contents of the Line Control Register(LCR) before it was
 *         modified.
 *
 * \note   Since the UART module registers that can be accessed at any time
 *         depends on the value in the Line Control Register(LCR), three
 *         register configuration modes have been defined, each corresponding
 *         to a specific state of the LCR. The three register configuration
 *         modes are:\n
 *         - Configuration Mode A: LCR[7] = 1 and LCR[7:0] != 0xBF.\n
 *         - Configuration Mode B: LCR[7:0] = 0xBF.\n
 *         - Operational Mode    : LCR[7] = 0.\n
 *
 *         Refer to the Register Listing in the UART/IrDA/CIR peripheral
 *         document for more information.\n
 *
 */
        extern uint32_t UARTRegConfigModeEnable(uint32_t baseAddr,
                                                uint32_t modeFlag);

        /**
 * \brief  This API is used to restore the UART to the specified Register
 *         Configuration Mode.
 *
 * \param  baseAddr      Memory address of the UART instance being used.
 * \param  lcrRegValue  The value to be loaded to the Line Control
 *                      Register(LCR).
 *
 * \return None
 *
 * \note   The API UARTRegConfigModeEnable() and the current API are used
 *         hand-in-hand. While UARTRegConfigModeEnable() switches the UART to
 *         the requested operating mode, the current API restores the UART to
 *         that register configuration mode prevalent before
 *         UARTRegConfigModeEnable() was called.
 */
        extern void UARTRegConfModeRestore(uint32_t baseAddr,
                                           uint32_t lcrRegValue);

        /**
 * \brief  This API is used to introduce or to remove a Break condition.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 * \param  breakState  This specifies whether the break condition should be
 *                     introduced or removed.
 *
 * 'breakState' can take one of the following two values:
 * - UART_BREAK_COND_DISABLE - to disable the Break condition if it has
 *   already been enabled\n
 * - UART_BREAK_COND_ENABLE - to enable the Break condition\n
 *
 * \return None
 *
 * \note  When the Break Condition is imposed, the Transmitter output line TX
 *        goes low to alert the communication terminal.
 */
        extern void UARTBreakCtl(uint32_t baseAddr, uint32_t breakState);

        /**
 * \brief  This API configures the Line Characteristics for the
 *         UART instance. The Line Characteristics include:
 *         - Word length per frame\n
 *         - Number of Stop Bits per frame\n
 *         - Parity feature configuration\n
 *
 * \param   baseAddr     Memory address of the UART instance being used.
 * \param   wLenStbFlag Bit mask value of the bits pertaining to word
 *                      length selection and stop bits selection in LCR.
 * \param   parityFlag  Bit mask value of the bits pertaining to parity
 *                      feature selection in LCR.
 *
 * 'wLenStbFlag' can take one of the following macros:
 *  -- (UART_FRAME_WORD_LENGTH_n | UART_FRAME_NUM_STB_1), where
 *    n = 5,6,7 or 8. -- This signifies that 1 stop bit and
 *    one among 5,6,7 and 8 bits are chosen as the word length
 *    per frame.\n
 *  -- (UART_FRAME_WORD_LENGTH_n | UART_FRAME_NUM_STB_1_5_2), where
 *    n = 5,6,7 or 8. -- This signifies that the word length and
 *    number of stop bits per frame could be one among the below
 *    four choices:
 *    --- WL = 5          NUM_STB = 1.5
 *    --- WL = 6,7 or 8   NUM_STB = 2
 *
 * 'parityFlag' can take one of the following macros:
 *  - (UART_ODD_PARITY) - signifying that odd parity be enabled and the
 *     parity bit be represented in a default manner.\n
 *  - (UART_EVEN_PARITY) - signifying that even parity be enabled and the
 *     parity bit be represented in a default manner.\n
 *  - (UART_PARITY_REPR_1) - signifying that the parity bit be represented
 *     by a logic 1 in the transmitted and received data.\n
 *  - (UART_PARITY_REPR_0) - signifying that the parity bit be represented
 *     by a logic 0 in the transmitted and received data.\n
 *  - (UART_PARITY_NONE) - signifying that no parity be enabled.\n
 *
 * \return  None.
 */
        extern void UARTLineCharacConfig(uint32_t baseAddr,
                                         uint32_t wLenStbFlag,
                                         uint32_t parityFlag);

        /**
 * \brief  This API configures the Parity feature for the UART.
 *
 * \param  baseAddr     Memory address of the UART instance being used
 * \param  parityFlag  This specifies the parity configuration to be
 *                     programmed to the Line Control Register(LCR)
 *
 * 'parityFlag' can take one of the following values:
 *  - (UART_ODD_PARITY) - signifying that odd parity be enabled and the parity
 *     bit be represented in a default manner\n
 *  - (UART_EVEN_PARITY) - signifying that even parity be enabled and the
 *     parity bit be represented in a default manner\n
 *  - (UART_ODD_PARITY_REPR_1) - signifying that odd parity be enabled and the
 *     parity bit be represented by a logic 1\n
 *  - (UART_EVEN_PARITY_REPR_0)- signifying that even parity be enabled and the
 *     parity bit be represented by a logic 0\n
 *  - (UART_PARITY_NONE) - signifying that no parity be enabled\n
 *
 * \return None
 */
        extern void UARTParityModeSet(uint32_t baseAddr, uint32_t parityFlag);

        /**
 * \brief  This API reads the Parity configuration being set in the UART.
 *
 * \param  baseAddr     Memory address of the UART instance being used
 *
 *
 * \return This returs one of the following values:
 *  - (UART_ODD_PARITY) - signifying that odd parity is enabled and the parity
 *     bit is represented in a default manner\n
 *  - (UART_EVEN_PARITY) - signifying that even parity is enabled and the
 *     parity bit is represented in a default manner\n
 *  - (UART_ODD_PARITY_REPR_1) - signifying that odd parity is enabled and the
 *     parity bit is represented by a logic 1\n
 *  - (UART_EVEN_PARITY_REPR_0)- signifying that even parity is enabled and the
 *     parity bit is represented by a logic 0\n
 *  - (UART_PARITY_NONE) - signifying that no parity is enabled\n
 *
 */
        extern uint32_t UARTParityModeGet(uint32_t baseAddr);

        /**
 * \brief  This API enables the DMA mode of operation for the UART instance.
 *
 * \param  baseAddr      Memory address of the UART instance being used.
 * \param  dmaModeFlag  This specifies the DMA mode to be enabled for the
 *                       UART instance.
 *
 * 'dmaModeFlag' can take one of the following four values:
 * - UART_DMA_MODE_0_ENABLE - to enable DMA Mode 0(No DMA)\n
 * - UART_DMA_MODE_1_ENABLE - to enable DMA Mode 1(DMA for both TX and RX)\n
 * - UART_DMA_MODE_2_ENABLE - to enable DMA Mode 2(DMA only for RX)\n
 * - UART_DMA_MODE_3_ENABLE - to enable DMA Mode 3(DMA only for TX)\n
 *
 * \return  None
 *
 * \note   This API enables the specified DMA modes always in SCR and FCR is
 *         not used. This is because SCR[2:1] allows the program to enable any
 *         of the four available DMA modes while FCR[3] allows the program to
 *         use either DMA Mode 0 or DMA Mode 1.\n
 */
        extern void UARTDMAEnable(uint32_t baseAddr, uint32_t dmaModeFlag);

        /**
 * \brief  This API disables the DMA mode of operation.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 *
 *
 * \return None
 */
        extern void UARTDMADisable(uint32_t baseAddr);

        /**
 * \brief  This API configures the FIFO settings for the UART instance.
 *         Specifically, this does the following configurations:
 *         1> Configures the Transmitter and Receiver FIFO Trigger Level
 *            granularity\n
 *         2> Configures the Transmitter and Receiver FIFO Trigger Level\n
 *         3> Configures the bits which clear/not clear the TX and RX FIFOs\n
 *         4> Configures the DMA mode of operation\n
 *
 * \param  baseAddr       Memory address of the UART instance being used.
 * \param  fifoConfig    This specifies the desired FIFO configurations.
 *                       Use the macro UART_FIFO_CONFIG to pass the required
 *                       FIFO settings.
 *
 * The parameters of UART_FIFO_CONFIG can take the following values:\n
 * -- txGra - the Transmitter FIFO trigger level granularity\n
 * -- rxGra - the Receiver FIFO trigger level granularity\n
 *    These can take one of the following two values:\n
 *    - UART_TRIG_LVL_GRANULARITY_1 - for a granularity of 1,\n
 *    - UART_TRIG_LVL_GRANULARITY_4 - for a granularity of 4.\n
 *
 * -- txTrig - the Transmitter FIFO trigger level\n
 *
 *    For 'txGra' being UART_TRIG_LVL_GRANULARITY_4, this can take one of the
 *    values from one of the following two sets:
 *    Set 1> UART_FCR_TX_TRIG_LVL_n, where n = 8,16,32,56.
 *           These are programmed to the FCR[5:4] in FCR register.
 *    Set 2> m , where (1 <= m <= 15).
 *           The trigger level would then be (m * 4).
 *           For example: If m = 9, then trigger level = 36.
 *           The value 'm' is programmed into TLR[3:0] field in TLR register.
 *
 *    For granularity being UART_TRIG_LVL_GRANULARITY_1, this can take
 *    any decimal value from 1 to 63.
 *
 * -- rxTrig - the Receiver FIFO Trigger level\n
 *
 *    For granularity being UART_TRIG_LVL_GRANULARITY_4, this can
 *    take one of the macro from one of the following two sets:
 *    Set 1> UART_FCR_RX_TRIG_LVL_n, where n = 8,16,56,60.
 *           These are programmed into FCR[7:6] field in FCR register.
 *    Set 2> m , where (1 <= m <= 15).
 *           The trigger level would then be (m * 4).
 *           For example: If m = 7, then trigger level = 28.
 *           The value 'm' is programmed to the TLR[7:4] field of TLR.
 *
 *    For granularity being UART_TRIG_LVL_GRANULARITY_1, this can take
 *    any decimal value from 1 to 63.
 *
 * -- txClr - the Transmitter FIFO clear flag\n
 * -- rxClr - the Receiver FIFO clear flag\n
 *
 *    These can take the following values:
 *    0 - to not clear the FIFO\n
 *    1 - to clear the FIFO. Clearing the FIFO also results in resetting the
 *        FIFO counter logic to 0.\n
 *
 * -- dmaEnPath - specifies whether DMA Mode settings should be configured
 *    through FCR or SCR. This can take one of the following two values:
 *    UART_DMA_EN_PATH_FCR - to configure DMA through FCR\n
 *    UART_DMA_EN_PATH_SCR - to configure DMA through SCR\n
 *
 * -- dmaMode - specifies the DMA Mode to be used\n
 *    This can take one of the following four values:
 *    - UART_DMA_MODE_m_ENABLE, where m = 0, 1, 2 or 3.
 *
 * \return  The value that was written to the FIFO Control Register(FCR).
 *
 * \note    1> FIFO Control Register(FCR) is a write-only register and its
 *             contents cannot be read. Hence, it is  not possible for a
 *             read-modify-write operation on this register. Therefore it is
 *             expected that the FIFO configuration is done at once for both
 *             TX and RX directions and always the entire FCR related
 *             information is required during modification of the FIFO
 *             settings.
 *             Also the FIFO related settings are valid only when the FIFO is
 *             enabled. This means that FIFO mode of operation should be enabled
 *             for FIFO related settings to take effect.
 */
        extern uint32_t UARTFIFOConfig(uint32_t baseAddr,
                                       uint32_t fifoConfig);

        /**
 * \brief  This API sets a certain bit in Enhanced Feature Register(EFR) which
 *         shall avail the UART to use some Enhanced Features.
 *
 * \param  baseAddr    Memory address of the UART instance being used.
 *
 * \return The value of ENHANCEDEN bit in EFR before it is modified in this API.
 *
 * \note   This API switches UART to Configuration Mode B, sets the ENHANCEDEN
 *         bit in EFR and reverts the UART back to the original mode of
 *         operation.
 *
 */
        extern uint32_t UARTEnhanFuncEnable(uint32_t baseAddr);

        /**
 * \brief  This API restores the ENHANCEDEN bit value of EFR register(EFR[4])
 *         to the corresponding bit value in 'enhanFnBitVal' passed as a
 *         parameter to this API.
 *
 * \param  baseAddr        Memory address of the UART instance being used.
 * \param  enhanFnBitVal  The 4th bit of this 32-bit value shall hold a value
 *                        to which the ENHANCEDEN bit of EFR (EFR[4]) has to
 *                        be restored to.
 *
 * \return None.
 *
 * \note   The APIs UARTEnhanFuncEnable() and the current one are used
 *         hand-in-hand. While UARTEnhanFuncEnable() collects the EFR[4] bit
 *         value before modifying it, UARTEnhanFuncBitValRestore() can be used
 *         to restore EFR[4] bit value after the necessary operation is
 *         complete.
 *         This API switches the UART to Configuration Mode B, does the needful
 *         and reverts it back to original mode of operation.
 *
 */
        extern void UARTEnhanFuncBitValRestore(uint32_t baseAddr,
                                               uint32_t enhanFnBitVal);

        /**
 * \brief  This API enables the MSR_SPR Sub-Configuration Mode of operation.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 *
 * \return The value of the TCRTLR bit in MCR (MCR[6]) before it is modified
 *         in this API.
 *
 * \note   1> Each Register configuration mode(A, B and operational) has
 *            further sub-configuration modes corresponding to accesses to
 *            offset addresses 0x18 and 0x1C. They are:\n
 *            For Register Configuration Mode A:\n
 *            1> MSR_SPR:  EFR[4] = 0  or MCR[6] = 0\n
 *            2> TCR_TLR:  EFR[4] = 1 and MCR[6] = 1\n
 *            For Register Configuration Mode B:\n
 *            1> TCR_TLR - EFR[4] = 1 and MCR[6] = 1\n
 *            2> XOFF    - EFR[4] = 0 or  MCR[6] = 0\n
 *
 *            For Register Operational Mode:\n
 *            1> MSR_SPR - EFR[4] = 0 or  MCR[6] = 0\n
 *            2> TCR_TLR - EFR[4] = 1 and MCR[6] = 1\n
 *
 *            In any configuration mode, enabling one of the sub-configuration
 *            mode would disable access to the registers of the other
 *            sub-configuration mode.\n
 *
 *         2> The current API enables access to Modem Status Register(MSR) and
 *            Scratch Pad Register(SPR). This is applicable for Register
 *            Configuration Mode A and Operational mode of operation.\n
 */
        extern uint32_t UARTSubConfigMSRSPRModeEn(uint32_t baseAddr);

        /**
 * \brief  This API enables the TCR_TLR Sub_Configuration Mode of operation.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 *
 * \return The value of the TCRTLR bit in MCR (MCR[6]) before it is modified
 *         in this API.
 *
 * \note   The current API enables access to Transmission Control Register(TCR)
 *         and Trigger Level Register(TLR). This is applicable for all the
 *         three register configuration modes.
 *         Refer to the comments of UARTSubConfigMSRSPRModeEn() API for more
 *         details.
 */
        extern uint32_t UARTSubConfigTCRTLRModeEn(uint32_t baseAddr);

        /**
 * \brief  This API enables the XOFF Sub-Configuration Mode of operation.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 *
 * \return The value of the TCRTLR bit in MCR (MCR[6]) before it is modified
 *         in this API.
 *
 * \note   The current API enables access to the XOFF (XOFF1 and XOFF2)
 *         registers. The XOFF registers can be accessed in Register
 *         Configuration Mode B of operation.
 *         Refer to the comments of UARTSubConfigMSRSPRModeEn() API for more
 *         details.
 */
        extern uint32_t UARTSubConfigXOFFModeEn(uint32_t baseAddr);

        /**
 * \brief  This API restores the TCRTLR bit(MCR[6]) value in Modem Control
 *         Register(MCR) to the corresponding bit value in 'tcrTlrBitVal'
 *         passed as a parameter to this API.
 *
 * \param  baseAddr       Memory address of the UART instance being used.
 * \param  tcrTlrBitVal  The 6th bit in this 32-bit value shall hold a value
 *                       to which the TCRTLR bit(MCR[6]) of MCR has to be
 *                       restored to.
 *
 * \return None.
 */
        extern void UARTTCRTLRBitValRestore(uint32_t baseAddr,
                                            uint32_t tcrTlrBitVal);

        /**
 * \brief   This API enables the specified interrupts in the UART mode of
 *          operation.
 *
 * \param   baseAddr   Memory address of the UART instance being used.
 * \param   intFlag   Bit mask value of the bits corresponding to Interrupt
 *                    Enable Register(IER). This specifies the UART interrupts
 *                    to be enabled.
 *
 *  'intFlag' can take one or a combination of the following macros:
 *  - UART_INT_CTS - to enable Clear-To-Send interrupt,
 *  - UART_INT_RTS - to enable Request-To-Send interrupt,
 *  - UART_INT_XOFF - to enable XOFF interrupt,
 *  - UART_INT_SLEEPMODE - to enable Sleep Mode,
 *  - UART_INT_MODEM_STAT - to enable Modem Status interrupt,
 *  - UART_INT_LINE_STAT - to enable Line Status interrupt,
 *  - UART_INT_THR - to enable Transmitter Holding Register Empty interrupt,
 *  - UART_INT_RHR_CTI - to enable Receiver Data available interrupt and
 *                       Character timeout indication interrupt.
 *
 * \return  None.
 *
 * \note    This API modifies the contents of UART Interrupt Enable Register
 *          (IER). Modifying the bits IER[7:4] requires that EFR[4] be set.
 *          This API does the needful before it accesses IER.
 *          Moreover, this API should be called when UART is operating in
 *          UART 16x Mode, UART 13x Mode or UART 16x Auto-baud mode.\n
 *
 */
        extern void UARTIntEnable(uint32_t baseAddr, uint32_t intFlag);

        /**
 * \brief   This API disables the specified interrupts in the UART mode of
 *          operation.
 *
 * \param   baseAddr   Memory address of the UART instance being used.
 * \param   intFlag   Bit mask value of the bits corresponding to Interrupt
 *                    Enable Register(IER). This specifies the UART interrupts
 *                    to be disabled.
 *
 *  'intFlag' can take one or a combination of the following macros:
 *  - UART_INT_CTS - to disable Clear-To-Send interrupt,
 *  - UART_INT_RTS - to disable Request-To-Send interrupt,
 *  - UART_INT_XOFF - to disable XOFF interrupt,
 *  - UART_INT_SLEEPMODE - to disable Sleep Mode,
 *  - UART_INT_MODEM_STAT - to disable Modem Status interrupt,
 *  - UART_INT_LINE_STAT - to disable Line Status interrupt,
 *  - UART_INT_THR - to disable Transmitter Holding Register Empty interrupt,
 *  - UART_INT_RHR_CTI - to disable Receiver Data available interrupt and
 *                       Character timeout indication interrupt.
 *
 * \return  None
 *
 * \note  The note section of UARTIntEnable() also applies to this API.
 */
        extern void UARTIntDisable(uint32_t baseAddr, uint32_t intFlag);

        /**
 * \brief   This API enables the specified interrupts in the UART mode of
 *          operation for IER2
 *
 * \param   baseAddr  Memory address of the UART instance being used.
 * \param   intFlag   Bit mask value of the bits corresponding to Interrupt
 *                    Enable Register(IER2). This specifies the UART interrupts
 *                    to be enabled.
 *
 *  'intFlag' can take one or a combination of the following macros:
 *  - UART_INT2_RX_EMPTY - to enable receive FIFO empty interrupt
 *  - UART_INT2_TX_EMPTY - to enable TX FIFO empty interrupt
 *
 * \return  None.
 *
 * \note    This API modifies the contents of UART Interrupt Enable Register 2
 *          (IER2).
 *
 */
        extern void UARTInt2Enable(uint32_t baseAddr, uint32_t intFlag);

        /**
 * \brief   This API disables the specified interrupts in the UART mode of
 *          operation  for IER2
 *
 * \param   baseAddr  Memory address of the UART instance being used.
 * \param   intFlag   Bit mask value of the bits corresponding to Interrupt
 *                    Enable Register(IER2). This specifies the UART interrupts
 *                    to be disabled.
 *
 *  'intFlag' can take one or a combination of the following macros:
 *  - UART_INT2_RX_EMPTY - to enable receive FIFO empty interrupt
 *  - UART_INT2_TX_EMPTY - to enable TX FIFO empty interrupt
 *
 * \return  None
 *
 * \note  The note section of UARTInt2Enable() also applies to this API.
 */
        extern void UARTInt2Disable(uint32_t baseAddr, uint32_t intFlag);

        /**
 * \brief  This API checks whether the TX FIFO (or THR in non-FIFO mode)
 *         is empty or not.
 *
 * \param  baseAddr    Memory address of the UART instance being used.
 *
 *
 * \return TRUE - if the Transmitter FIFO(or THR in non-FIFO mode) is empty.
 *         FALSE - if the Transmitter FIFO(or THR in non-FIFO mode) is
 *         not empty.\n
 */
        extern uint32_t UARTSpaceAvail(uint32_t baseAddr);

        /**
 * \brief  This API checks if the RX FIFO (or RHR in non-FIFO mode) has atleast
 *         one byte of data to be read.
 *
 * \param  baseAddr    Memory address of the UART instance being used.
 *
 *
 * \return  TRUE - if there is atleast one data byte present in the RX FIFO
 *          (or RHR in non-FIFO mode)\n
 *          FALSE - if there are no data bytes present in the RX FIFO(or RHR
 *           in non-FIFO mode)\n
 */
        extern uint32_t UARTCharsAvail(uint32_t baseAddr);

        /**
 * \brief    This API attempts to write a byte into Transmitter Holding
 *           Register (THR). It checks only once if the transmitter is empty.
 *
 * \param    baseAddr     Memory address of the UART instance being used.
 * \param    byteWrite   Byte to be written into the THR register.
 *
 * \return   TRUE if the transmitter FIFO(or THR register in non-FIFO mode)
 *           was empty and the character was written. Else it returns FALSE.
 */
        extern uint32_t UARTCharPutNonBlocking(uint32_t baseAddr,
                                               uint8_t byteWrite);

        /**
 * \brief    This API reads a byte from the Receiver Buffer Register
 *           (RBR). It checks once if any character is ready to be read.
 *
 * \param    baseAddr     Memory address of the UART instance being used.
 *
 *
 * \return   If the RX FIFO(or RHR) was found to have atleast one byte of
 *           data, then this API reads and returns that byte. Else it
 *           returns -1.
 */
        extern int8_t UARTCharGetNonBlocking(uint32_t baseAddr);

        /**
 * \brief    This API waits indefinitely for the arrival of a byte in
 *           the receiver FIFO. Once a byte has arrived, it returns that
 *           byte.
 *
 * \param    baseAddr     Memory address of the UART instance being used.
 *
 *
 * \return   This returns the read byte.
 */
        extern int8_t UARTCharGet(uint32_t baseAddr);

        /**
 * \brief   This API waits for the arrival of atleast one byte into the
 *          Receiver FIFO or until a specified timeout value gets decremented
 *          to zero, whichever happens first.
 *
 * \param   baseAddr     Memory address of the UART instance being used.
 * \param   timeOutVal  The timeout value that is to be used. This timeout
 *                      value gets decremented once per iteration of the
 *                      wait loop.
 *                      wait loop.
 *
 * \return  This returns either of the below two values:\n
 *          1) If a character was keyed in before the timeout value gets
 *             decremented to zero, this API returns the entered byte.\n
 *          2) If no character was input within the timeout value getting
 *             decremented to zero, this API returns 0xFF.\n
 */
        extern uint8_t UARTCharGetTimeout(uint32_t baseAddr,
                                          uint32_t timeOutVal);

        /**
 * \brief   This API waits indefinitely until the Transmitter FIFO
 *          (THR register in non-FIFO mode) and Transmitter Shift
 *          Register are empty. On empty, it writes a byte to the THR.
 *
 * \param   baseAddr     Memory address of the UART instance being used
 * \param   byteTx      The byte to be transmitted by the UART.
 *
 *
 * \return  None
 */
        extern void UARTCharPut(uint32_t baseAddr, uint8_t byteTx);

        /**
 * \brief   This API writes a byte to the Transmitter FIFO without checking for
 *          the emptiness of the Transmitter FIFO or the Transmitter Shift
 *          Register(TSR).
 *
 * \param   baseAddr     Memory address of the UART instance being used.
 * \param   byteTx      The byte to be transmitted by the UART.
 *
 *
 * \return  None
 *
 * \note    Unlike the APIs UARTCharPut() or UARTCharPutNonBlocking(), this
 *          API does not check for the emptiness of the TX FIFO or TSR. This
 *          API is ideal for use in FIFO mode of operation where the 64-byte
 *          TX FIFO has to be written with successive bytes. If transmit
 *          interrupt is enabled, it provides a mechanism to control the
 *          writes to the TX FIFO.
 */
        extern void UARTFIFOCharPut(uint32_t baseAddr, uint8_t byteTx);

        /**
 * \brief   This API reads the data present at the top of the RX FIFO, that
 *          is, the data in the Receive Holding Register(RHR). However
 *          before reading the data from RHR, it does not check whether
 *          RHR has fresh data or not.
 *
 * \param   baseAddr     Memory address of the UART instance being used.
 *
 *
 * \return  The data read from the RHR.
 *
 * \note    1) Since this API does not check whether RX FIFO(RHR) has fresh
 *             data before reading the same, the application should ensure
 *             that RX FIFO has data before calling this API. The API
 *             UARTCharsAvail() can be used to check if the RX FIFO has
 *             atleast one byte of data.\n
 *          2) If the RX FIFO did not have any fresh data and this API is
 *             called, this API would return an unknown value.\n
 */
        extern int8_t UARTFIFOCharGet(uint32_t baseAddr);

        /**
 * \brief  This API copies the requested amount of data from the
 *         pointed data block to the UART Transmit FIFO.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 * \param  pBuffer     The starting address of the data block.
 * \param  numTxBytes  The number of data bytes to be transmitted.
 *
 *
 * \return The number of data bytes that were written to the TX FIFO.
 *
 * \note   This API does not check for the emptiness of the TX FIFO or for
 *         its space availability before writing to it. The application
 *         calling this API has the responsibility of checking the TX
 *         FIFO status before using this API.\n
 */
        extern uint32_t UARTFIFOWrite(uint32_t baseAddr,
                                      const uint8_t *pBuffer,
                                      uint32_t numTxBytes);

        /**
 * \brief  This API reads the receiver data error status.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 *
 *
 * \return This returns the error status. This can be one or a combination
 *         of the below values:
 *         - UART_OVERRUN_ERROR - indicating Overrun error occured\n
 *         - UART_PARITY_ERROR - indicating Parity error occured\n
 *         - UART_FRAMING_ERROR - indicating Framing error occured\n
 *         - UART_BREAK_DETECTED_ERROR - indicating a Break condition was
 *           detected\n
 *         - UART_FIFO_PE_FE_BI_DETECTED - indicating that atleast one parity
 *           error, framing error or a break indication is present in the
 *           RX FIFO\n
 */
        extern uint32_t UARTRxErrorGet(uint32_t baseAddr);

        /**
 * \brief  This API determines the UART Interrupt Status.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 *
 *
 * \return This returns one or a combination of the following macros:
 *         - UART_INTID_MODEM_STAT - indicating the occurence of a Modem Status
 *           interrupt\n
 *         - UART_INTID_TX_THRES_REACH - indicating that the TX FIFO Threshold
 *           number of bytes can be written to the TX FIFO.
 *         - UART_INTID_RX_THRES_REACH - indicating that the RX FIFO has
 *           reached its programmed Trigger Level\n
 *         - UART_INTID_RX_LINE_STAT_ERROR - indicating the occurence of a
 *           receiver Line Status error\n
 *         - UART_INTID_CHAR_TIMEOUT - indicating the occurence of a Receiver
 *           Timeout\n
 *         - UART_INTID_XOFF_SPEC_CHAR_DETECT - indicating the detection of XOFF
 *           or a Special character\n
 *         - UART_INTID_MODEM_SIG_STATE_CHANGE - indicating that atleast one of
 *           the Modem signals among CTSn, RTSn and DSRn have changed states
 *           from active(low) to inactive(high)\n
 */
        extern uint32_t UARTIntIdentityGet(uint32_t baseAddr);

        /**
 * \brief  This API determines whether any UART interrupt condition is
 *         still alive and is pending to be serviced.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 *
 *
 * \return UART_INT_PENDING - if servicing an interrupt is still pending\n
 *         UART_N0_INT_PENDING - if there are no interrupts to be serviced\n
 */
        extern uint32_t UARTIntPendingStatusGet(uint32_t baseAddr);

        /**
 * \brief  This API determines the UART Interrupt Status 2.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 *
 *
 * \return This returns one or a combination of the following macros:
 *         - UART_INT2_RX_EMPTY - to enable receive FIFO empty interrupt\n
 *         - UART_INT2_TX_EMPTY - to enable TX FIFO empty interrupt\n
 */
        extern uint32_t UARTInt2StatusGet(uint32_t baseAddr);

        /**
 * \brief  This API determines whether FIFO mode of operation is enabled
 *         for the UART instance or not.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 *
 *
 * \return TRUE - if FIFO mode of operation is enabled\n
 *         FALSE - if FIFO mode of operation is disabled\n
 */
        extern uint32_t UARTFIFOEnableStatusGet(uint32_t baseAddr);

        /**
 * \brief  This API controls the use of Auto-RTS and Auto-CTS features which
 *         are used in Hardware Flow Control Mode of operation. The
 *         Auto-RTS and Auto-CTS functions can be individually enabled or
 *         disabled.
 *
 * \param  baseAddr         Memory address of the UART instance being used.
 * \param  autoCtsControl  This specifies whether to enable or disable
 *                         Auto-CTS functionality
 * \param  autoRtsControl  This specifies whether to enable or disable
 *                         Auto-RTS functionality
 *
 * 'autoCtsControl' can take one of the following values:
 * - UART_AUTO_CTS_DISABLE - to disable Auto-CTS functionality\n
 * - UART_AUTO_CTS_ENABLE - to enable Auto-CTS functionality\n
 *
 * 'autoRtsControl' can take either of the following values:
 * - UART_AUTO_RTS_DISABLE - to disable Auto-RTS functionality\n
 * - UART_AUTO_RTS_ENABLE - to enable Auto-RTS functionality\n
 *
 * \return None.
 *
 * \note   This API switches UART to Configuration Mode B, programs
 *         AUTOCTSEN and AUTORTSEN bits in EFR and reverts the UART back
 *         to the original mode of operation.
 */
        extern void UARTAutoRTSAutoCTSControl(uint32_t baseAddr,
                                              uint32_t autoCtsControl,
                                              uint32_t autoRtsControl);

        /**
 * \brief  This API controls the feature of detecting a Special Character
 *         arriving in the receiver.
 *
 * \param   baseAddr     Memory address of the UART instance being used.
 * \param   controlFlag This specifies whether to enable or disable detection
 *                      of Special Character.
 *
 * 'controlFlag' can take either of the following two values:\n
 * - UART_SPECIAL_CHAR_DETECT_DISABLE - to disable detection of Special
 *   Character\n
 * - UART_SPECIAL_CHAR_DETECT_ENABLE - to enable detection of Special
 *   Character\n
 *
 * \return  None.
 *
 * \note   This API switches the UART to Configuration Mode B, programs
 *         SPECIALCHARDETECT field in EFR and reverts the UART back to
 *         the original mode of operation.
 *
 */
        extern void UARTSpecialCharDetectControl(uint32_t baseAddr,
                                                 uint32_t controlFlag);

        /**
 * \brief  This API configures the options for Software Flow Control.
 *
 * \param  baseAddr      Memory address of the UART instance being used.
 * \param  swFlowCtrl   This specifies one of the options available for
 *                      software flow control.
 *
 * 'swFlowCtrl' can take one of the following values:
 * - UART_NO_SOFTWARE_FLOW_CONTROL - To disable Software Flow control\n
 * - UART_TX_RX_XON1_XOFF1 - Transmitter transmits XON1, XOFF1;
 *                           Receiver expects XON1, XOFF1\n
 * - UART_TX_RX_XON2_XOFF2 - Transmitter transmits XON2, XOFF2;
 *                           Receiver expects XON2, XOFF2\n
 * - UART_TX_RX_XON1_XOFF1_XON2_XOFF2 - Transmitter transmits XON1,XON2,
 *                           XOFF1 and XOFF2; Receiver expects XON1,XON2,
 *                           XOFF1, XOFF2\n
 *
 * \return None.
 *
 * \note   This API switches the UART to Configuration Mode B, programs
 *         SWFLOWCONTROL field in EFR and reverts the UART back to the
 *         original mode of operation.
 */
        extern void UARTSoftwareFlowCtrlOptSet(uint32_t baseAddr,
                                               uint32_t swFlowCtrl);

        /**
 * \brief  Used only in UART mode, this API is used to control the pulse
 *         shaping feature. Pulse shaping feature could either be enabled
 *         or disabled in UART mode.
 *
 * \param  baseAddr       Memory address of the UART instance being used.
 * \param  shapeControl  This specifies whether to enable or disable Pulse
 *                       shaping feature in UART mode
 *
 * 'shapeControl' can take either of the two following values:\n
 * - UART_PULSE_NORMAL - to disable Pulse Shaping feature which is the Normal
 *   and default configuration\n
 * - UART_PULSE_SHAPING - to enable Pulse Shaping feature\n
 *
 * \return None
 */
        extern void UARTPulseShapingControl(uint32_t baseAddr,
                                            uint32_t shapeControl);

        /**
 * \brief  This API performs a module reset of the UART instance. It also waits
 *         until the reset process is complete.
 *
 * \param  baseAddr  Memory address of the UART instance being used.
 *
 * \return None.
 *
 * \note   This API accesses the System Configuration Register(SYSC) and
 *         System Status Register(SYSS) to perform module reset and to
 *         wait until the same is complete.
 */
        extern void UARTModuleReset(uint32_t baseAddr);

        /**
 * \brief  This API can be used to control the Power Management
 *         request/acknowledgement process.
 *
 * \param  baseAddr   Memory address of the UART instance being used.
 * \param  modeFlag  This specifies the Power Management
 *                   request/acknowledgement process to be followed
 *
 * 'modeFlag' can take one of the following values:
 * - UART_IDLEMODE_FORCE_IDLE - to enable Force Idle mode\n
 * - UART_IDLEMODE_NO_IDLE - to enable No-Idle mode\n
 * - UART_IDLEMODE_SMART_IDLE - to enable Smart Idle mode\n
 * - UART_IDLEMODE_SMART_IDLE_WAKEUP - to enable Smart Idle Wakeup mode\n
 *
 * \return None
 */
        extern void UARTIdleModeConfigure(uint32_t baseAddr, uint32_t modeFlag);

        /**
 * \brief  This API is used to control(enable/disable) the Wake-Up feature
 *         of the UART.
 *
 * \param  baseAddr      Memory address of the UART instance being used.
 * \param  controlFlag  This specifies whether the Wake Up feature should
 *                      be enabled or disabled for the UART instance
 *
 * 'controlFlag' can take one of the following two values:
 * - UART_WAKEUP_ENABLE - to enable Wake-Up feature\n
 * - UART_WAKEUP_DISABLE - to disable Wake-Up feature\n
 *
 * \return None
 */
        extern void UARTWakeUpControl(uint32_t baseAddr, uint32_t controlFlag);

        /**
 * \brief  This API is used to control(enable/disable) the Auto-Idle mode
 *         of operation of the UART.
 *
 * \param  baseAddr      Memory address of the UART instance being used.
 * \param  modeFlag     This specifies whether to enable or disable the
 *                      Auto-Idle mode of the UART
 *
 * 'modeFlag' can take one of the following two values:
 * - UART_AUTO_IDLE_MODE_DISABLE - to disable Auto-Idle mode\n
 * - UART_AUTO_IDLE_MODE_ENABLE - to enable Auto-Idle mode\n
 *
 * \return None
 */
        extern void UARTAutoIdleModeControl(uint32_t baseAddr,
                                            uint32_t modeFlag);

        /**
 * \brief  This API configures the Receiver FIFO threshold level to
 *         start/stop transmission during Hardware Flow Control.
 *
 * \param  baseAddr        Memory address of the UART instance being used.
 * \param  rtsHaltFlag    The receiver FIFO threshold level on attaining
 *                        which the RTS line is deasserted signalling the
 *                        transmitter of its counterpart to stop trasmitting.
 * \param  rtsStartFlag   The receiver FIFO threshold level on attaining
 *                        which the RTS line is asserted signalling the
 *                        transmitter of its counterpart to start transmitting.
 *
 * 'rtsHaltFlag' can take one among the following values:\n
 *  - m, where (0 <= m <= 15).\n
 *  The HALT trigger level would then be (m *  4).\n
 * 'rtsStartFlag' can take one among the following values:\n
 *  - n, where (0 <= n <= 15).\n
 *  The RESTORE trigger level would then be (n * 4).\n
 *
 * For Example: If m = 8 and n = 5, then the receiver trigger levels would be:\n
 * HALT = (8 * 4) = 32, RESTORE = (5 * 4) = 20.\n
 *
 * \return None.
 *
 * \note     Here two things should be taken care of:\n
 *        1> RX FIFO Threshold Level to Halt Transmission should be greater
 *           than the Threshold level to Start transmission
 *           i.e. TCR[3:0] > TCR[7:4].\n
 *        2> In FIFO Interrupt mode with Flow Control, the RX FIFO threshold
 *           level to Halt Transmission (TCR[3:0]) should be greater than or
 *           equal to the Receiver FIFO trigger level(TLR[7:4] or FCR[7:6]).\n
 */
        extern void UARTFlowCtrlTrigLvlConfig(uint32_t baseAddr,
                                              uint32_t rtsHaltFlag,
                                              uint32_t rtsStartFlag);

        /**
 * \brief  This API programs the XON1/ADDR1 and XOFF1 registers.
 *
 * \param  baseAddr      Memory address of the UART instance being used.
 * \param  xon1Value    The XON1 character.
 * \param  xoff1Value   The XOFF1 character.
 *
 * \return None.
 *
 * \note   In UART mode, the character in XON1/ADDR1 register is the XON1
 *         character. In IrDA mode, this is the address ADDR1.
 */
        extern void UARTXON1XOFF1ValProgram(uint32_t baseAddr,
                                            uint8_t xon1Value,
                                            uint8_t xoff1Value);

        /**
 * \brief  This API programs the XON2/ADDR2 and XOFF2 registers.
 *
 * \param  baseAddr      Memory address of the UART instance being used.
 * \param  xon2Value    The XON2 character.
 * \param  xoff2Value   The XOFF2 character.
 *
 * \return None.
 *
 * \note   In UART mode, the character in XON2/ADDR2 register is the XON2
 *         character. In IrDA mode, this is the address ADDR2.
 *
 */
        extern void UARTXON2XOFF2ValProgram(uint32_t baseAddr,
                                            uint8_t xon2Value,
                                            uint8_t xoff2Value);

        /**
 * \brief  This API controls(enables/disables) the XON-any feature in Modem
 *         Control Register(MCR).
 *
 * \param  baseAddr        Memory address of the UART instance being used.
 * \param  controlFlag    This specifies whether to enable or disable XON any
 *                        feature
 * 'xonAnyControl' can take one of the following values:
 * - UART_XON_ANY_ENABLE - to enable XON any functionality\n
 * - UART_XON_ANY_DISABLE - to disable XON any functionality\n
 *
 * \return None.
 *
 * \note   When XON-any feature is enabled, the transmission will resume after
 *         receiving any character after recognizing the XOFF character. The
 *         XON-any character is written into the RX FIFO even if it is a
 *         software flow character.\n
 */
        extern void UARTXONAnyFeatureControl(uint32_t baseAddr,
                                             uint32_t controlFlag);

        /**
 * \brief  This API controls(enables/disables) the Loopback mode of operation
 *         for the UART instance.
 *
 * \param  baseAddr      Memory address of the UART instance being used
 * \param  controlFlag  This specifies whether to enable or disable Loopback
 *                      mode of operation
 * 'controlFlag' can take one of the following values:
 * - UART_LOOPBACK_MODE_ENABLE - to enable Loopback mode of operation\n
 * - UART_LOOPBACK_MODE_DISABLE - to disable Loopback mode and thus resulting
 *                                in switching to Normal operating mode\n
 *
 * \return None
 */
        extern void UARTLoopbackModeControl(uint32_t baseAddr,
                                            uint32_t controlFlag);

        /**
 * \brief  This API switches the specified Modem Control Signals to active
 *         state. The Modem Control signals in context are DCD, RI, RTS and
 *         DTR.
 *
 * \param  baseAddr    Memory address of the UART instance being used.
 * \param  modeFlag   This specifies the signals that are required to be
 *                    switched to active state. Bits MCR[3:0] hold control
 *                    for switching Modem Control Signals to active/inactive
 *                    state.
 *
 * 'modeFlag' can take one or a combination of the following values:\n
 * - UART_DCD_CONTROL - specifying to force DCDn input to active state(low)\n
 * - UART_RI_CONTROL - specifying to force RIn input to active state(low)\n
 * - UART_RTS_CONTROL - specifying to force RTSn output to active state (low)\n
 * - UART_DTR_CONTROL - specifying to force DTRn output to active state (low)\n
 *
 * \return None.
 */
        extern void UARTModemControlSet(uint32_t baseAddr, uint32_t modeFlag);

        /**
 * \brief  This API switches the specified Modem Control signals to inactive
 *         state. The Modem Control signals in context are DCD, RI, RTS and
 *         DTR.
 *
 * \param  baseAddr    Memory address of the UART instance being used.
 * \param  modeFlag   This specifies the signals that are required to be
 *                    switched to inactive state. Bits MCR[3:0] hold control
 *                    for switching Modem Control Signals to active/inactive
 *                    state.
 * 'modeFlag' can take one or a combination of the following values:\n
 * - UART_DCD_CONTROL - specifying to force DCDn input to inactive state(high)\n
 * - UART_RI_CONTROL - specifying to force RIn input to inactive state (high)\n
 * - UART_RTS_CONTROL - specifying to force RTSn output to inactive
 * state(high)\n
 * - UART_DTR_CONTROL - specifying to force DTRn output to inactive
 * state(high)\n
 *
 * \return None
 */
        extern void UARTModemControlClear(uint32_t baseAddr, uint32_t modeFlag);

        /**
 * \brief  This API reads the values on Modem Signal Lines. The Modem Signals
 *         in context are:
 *         1> Data Carrier Detect(DCD)\n
 *         2> Ring Indicator(RI)\n
 *         3> Data Set Ready(DSR)\n
 *         4> Clear To Send(CTS)\n
 *
 * \param  baseAddr    Memory address of the UART instance being used.
 *
 * \return The value returned could be one of the following macros, a
 *         combination or all of it.\n
 *         - UART_DCD_VALUE - indicating DCDn line is active(low)\n
 *         - UART_RI_VALUE - indicating RIn line is active(low)\n
 *         - UART_DSR_VALUE - indicating DSRn line is active(low)\n
 *         - UART_CTS_VALUE - indicating CTSn line is active(low)\n
 */
        extern uint32_t UARTModemStatusGet(uint32_t baseAddr);

        /**
 * \brief  This API determines if the values on Modem Signal Lines have
 *         changed since the last read of Modem Status Register(MSR).
 *         The Modem Signals in context are DCD, RI, DSR, CTS.
 *
 * \param  baseAddr    Memory address of the UART instance being used.
 *
 * \return The value returned could be one of the following macros, a
 *         combination or all of it.
 *         - UART_DCD_STS_CHANGED - indicating that DCDn input changed state
 *           since the last read of MSR\n
 *         - UART_RI_STS_CHANGED - indicating that RIn input changed state
 *           since the last read of MSR\n
 *         - UART_DSR_STS_CHANGED - indicating that DSRn input changed state
 *           since the last read of MSR\n
 *         - UART_CTS_STS_CHANGED - indicating that CTSn input changed state
 *           since the last read of MSR\n
 */
        extern uint32_t UARTModemStatusChangeCheck(uint32_t baseAddr);

        /**
 * \brief  This API reads the RESUME register which clears the internal flags.
 *
 * \param  baseAddr    Memory address of the UART instance being used
 *
 * \return None
 *
 * \note   UART_RESUME is read only register that always reads 0x0.
 *         When conditions like TX Underrun/RX Overrun errors occur, the
 *         transmission/reception gets halted and some internal flags are set.
 *         Clearing these flags would resume the halted operation.
 *         Dummy operations are carried out in this API to remove compiler
 *         warnings.
 */
        extern void UARTResumeOperation(uint32_t baseAddr);

        /**
 * \brief  This API enables the Wake-Up capability for the specified events.
 *         On enabling Wake-Up capability for them, the occurence of the
 *         corresponding event shall wake up the system.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 * \param  wakeUpFlag  This specifies the event(s) for which Wake-Up
 *                     capability needs to be enabled.
 *
 * 'wakeUpFlag' can take one or a combination of the following values:
 * - UART_WAKEUP_TX_INTERRUPT - enabling THR, TXDMA and TXSTATUS
 *   interrupts to wake up the system\n
 * - UART_WAKEUP_RLS_INTERRUPT - enabling the Receiver Line Status
 *   interrupt to wake up the system\n
 * - UART_WAKEUP_RHR_INTERRUPT - enabling the RHR interrupt(RX FIFO
 *   threshold level reached) to wake up the system\n
 * - UART_WAKEUP_RX_ACTIVITY - enabling any activity on the Receiver line
 *   (RX) to wake up the system\n
 * - UART_WAKEUP_DCD_ACTIVITY - enabling any activity on DCD line to wake
 *   up the system\n
 * - UART_WAKEUP_RI_ACTIVITY - enabling any activity on RI line to wake up
 *   the system\n
 * - UART_WAKEUP_DSR_ACTIVITY - enabling any acivity on DSR line to wake up
 *   the system\n
 * - UART_WAKEUP_CTS_ACTIVITY - enabling any activity on CTS line to wake up
 *   the system\n
 *
 * \return  None
 */
        extern void UARTWakeUpEventsEnable(uint32_t baseAddr,
                                           uint32_t wakeUpFlag);

        /**
 * \brief  This API disables the Wake-Up capability for the specified events.
 *         On disabling Wake-Up capability for them, the occurence of the
 *         corresponding event shall not wake up the system.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 * \param  wakeUpFlag  This specifies the event(s) for which Wake-Up
 *                     capability needs to be disabled.
 *
 * 'wakeUpFlag' can take one or a combination of the following values:
 * - UART_WAKEUP_TX_INTERRUPT - disabling THR, TXDMA and TXSTATUS
 *   interrupts to wake up the system\n
 * - UART_WAKEUP_RLS_INTERRUPT - disabling the Receiver Line Status
 *   interrupt to wake up the system\n
 * - UART_WAKEUP_RHR_INTERRUPT - disabling the RHR interrupt(RX FIFO
 *   threshold level reached) to wake up the system\n
 * - UART_WAKEUP_RX_ACTIVITY - disabling any activity on the Receiver line
 *   (RX) to wake up the system\n
 * - UART_WAKEUP_DCD_ACTIVITY - disabling any activity on DCD line to wake
 *   up the system\n
 * - UART_WAKEUP_RI_ACTIVITY - disabling any activity on RI line to wake up
 *   the system\n
 * - UART_WAKEUP_DSR_ACTIVITY - disabling any acivity on DSR line to wake up
 *   the system\n
 * - UART_WAKEUP_CTS_ACTIVITY - disabling any activity on CTS line to wake up
 *   the system\n
 *
 * \return  None
 */
        extern void UARTWakeUpEventsDisable(uint32_t baseAddr,
                                            uint32_t wakeUpFlag);

        /**
 * \brief  This API controls the feature of setting the Trigger Level
 *         granularity as 1 for Transmitter and Receiver FIFOs.
 *
 * \param  baseAddr          Memory address of the UART instance being used.
 * \param  rxFIFOGranCtrl   This specifies whether the trigger level
 *                          granularity for the RX FIFO is to be 1 or not
 * \param  txFIFOGranCtrl   This specifies whether the trigger level
 *                          granularity for the TX FIFO is to be 1 or not
 *
 * 'rxFIFOGranCtrl' can take either of the following values:
 * - UART_RX_TRIG_LVL_GRAN_1_DISABLE - to disable usage of a granularity of 1
 *   for RX FIFO Trigger level\n
 * - UART_RX_TRIG_LVL_GRAN_1_ENABLE - to set a granularity of 1 for RX FIFO
 *   Trigger level\n
 *
 * 'txFIFOGranCtrl' can take either of the following values:
 * - UART_TX_TRIG_LVL_GRAN_1_DISABLE - to disable usage of a granularity of 1
 *   for TX FIFO Trigger level\n
 * - UART_TX_FIFO_LVL_GRAN_1_ENABLE - to set a granularity of 1 for TX FIFO
 *   Trigger level\n
 *
 * \return None
 */
        extern void UARTFIFOTrigLvlGranControl(uint32_t baseAddr,
                                               uint32_t rxFIFOGranCtrl,
                                               uint32_t txFIFOGranCtrl);

        /**
 * \brief  This API controls the interrupt enable and disable feature for
 *         Data Set Ready(DSRn) interrupt.
 *
 * \param  baseAddr     Memory address of the UART instance being used
 * \param  controlFlag This specifies whether to enable or disable DSRn
 *                     interrupt
 *
 * 'controlFlag' can take one of the following values:
 * - UART_DSRn_INT_DISABLE - to disable DSRn interrupt\n
 * - UART_DSRn_INT_ENABLE - to enable DSRn interrupt\n
 *
 * \return None
 */
        extern void UARTDSRInterruptControl(uint32_t baseAddr,
                                            uint32_t controlFlag);

        /**
 * \brief  This API is used to choose a condition under which a
 *         Transmit Holding Register(THR) Interrupt should occur.
 *         A THR interrupt can be configured to occur either when:\n
 *         1> TX FIFO becoming empty OR\n
 *         2> TX FIFO and TX Shift register becoming empty.
 *
 * \param  baseAddr      Memory address of the UART instance being used
 * \param  controlFlag  This specifies the condition under which a Transmitter
 *                      Holding Register Interrupt should occur.
 *
 * 'controlFlag' can take either of the following two values:
 * - UART_THR_INT_NORMAL - for THR Interrupt to be raised under normal
 *   conditions(guided by the TX FIFO Threshold value)\n
 * - UART_THR_INT_FIFO_TSR_EMPTY - for THR Interrupt to be raised when both
 *   Transmitter FIFO and Transmitter Shift Register are empty\n
 *
 * \return None
 */
        extern void UARTTxEmptyIntControl(uint32_t baseAddr,
                                          uint32_t controlFlag);

        /**
 * \brief  This API controls (enables/disables) a feature where a falling
 *         edge on the RX, CTSn or DSRs could send a wake-up interrupt to
 *         the CPU.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 * \param  wakeUpFlag  This specifies whether or not a wake-up interrupt
 *                     should be sent to the CPU when a falling edge occurs on
 *                     RX, CTSn or DSRn lines.
 *
 * 'wakeUpFlag' can take one of the following values:
 * - UART_RX_CTS_DSR_WAKEUP_DISABLE - to disable generation of a Wake-Up
 *   interrupt due to occurence of a falling edge on RX, CTSn or DSRn lines.
 * - UART_RX_CTS_DSR_WAKEUP_ENABLE - to enable generation of a Wake-Up
 *   interrupt due to occurence of a falling edge on RX, CTSn, DSRn lines.
 *
 * \return None
 */
        extern void UARTRXCTSDSRWakeUpConfigure(uint32_t baseAddr,
                                                uint32_t wakeUpFlag);

        /**
 * \brief  This API determines whether a falling edge occured on RX, CTSn or
 *         DSRn lines.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 *
 *
 * \return This returns one of the following values:
 *         - UART_RX_CTS_DSR_NO_FALL_EDGE - indicating that no falling edge
 *           occured on RX, CTSn and DSRn lines\n
 *         - UART_RX_CTS_DSR_FALL_EDGE - indicating that a falling edge
 *           occured on RX, CTSn and DSRn lines\n
 */
        extern uint32_t UARTRXCTSDSRTransitionStatusGet(uint32_t baseAddr);

        /**
 * \brief  This API controls the DMA Counter Reset options.
 *
 * \param  baseAddr      Memory address of the UART instance being used
 * \param  controlFlag  This specifies the DMA Counter Reset options
 *
 *
 * 'controlFlag' can take either of the following values:
 * - UART_DMA_CNTR_NO_RESET_FIFO_RESET - indicating that the DMA counter
 *   shall not be reset if the corresponding FIFO is reset\n
 * - UART_DMA_CNTR_RESET_FIFO_RESET - indicating that the DMA counter shall
 *   be reset if the corresponding FIFO is reset\n
 *
 * \return None
 */
        extern void UARTDMACounterResetControl(uint32_t baseAddr,
                                               uint32_t controlFlag);

        /**
 * \brief  This API determines whether the Transmitter FIFO is full or not.
 *
 * \param  baseAddr      Memory address of the UART instance being used
 *
 *
 * \return This returns either of the following values:
 *         - UART_TX_FIFO_NOT_FULL - indicating that the TX FIFO is not full\n
 *         - UART_TX_FIFO_FULL - indicating that the TX FIFO is full\n
 */
        extern uint32_t UARTTxFIFOFullStatusGet(uint32_t baseAddr);

        /**
 * \brief   This API determines the current level of the Transmitter FIFO.
 *
 * \param   baseAddr      Memory address of the UART instance being used.
 *
 *
 * \return  The current level of the Transmitter FIFO.
 */
        extern uint32_t UARTTxFIFOLevelGet(uint32_t baseAddr);

        /**
 * \brief   This API determines the current level of the Receiver FIFO.
 *
 * \param   baseAddr      Memory address of the UART instance being used.
 *
 *
 * \return  The current level of the Receiver FIFO.
 */
        extern uint32_t UARTRxFIFOLevelGet(uint32_t baseAddr);

        /**
 * \brief  This API determines the Parity mode being configured by the system
 *         in the UART Autobauding mode.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 *
 *
 * \return This returns one of the following values:
 *         - UART_AUTOBAUD_NO_PARITY - indicating that no parity was
 *           identified\n
 *         - UART_AUTOBAUD_PARITY_SPACE - indicating that space parity has
 *           been configured\n
 *         - UART_AUTOBAUD_EVEN_PARITY - indicating that even parity has been
 *           configured\n
 *         - UART_AUTOBAUD_ODD_PARITY - indicating that odd parity has been
 *           configured\n
 *
 * \note   UASR register used in this API can be accessed only when the UART
 *         is in Configuration Mode A or Configuration Mode B of operation.
 */
        extern uint32_t UARTAutobaudParityGet(uint32_t baseAddr);

        /**
 * \brief  This API determines the word length per frame(character length)
 *         being configured by the system in UART Autobauding mode.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 *
 *
 * \return This returns one of the following two values:
 *         - UART_AUTOBAUD_CHAR_LENGTH_7 - indicating word length of 7 bits\n
 *         - UART_AUTOBAUD_CHAR_LENGTH_8 - indicating word length of 8 bits\n
 *
 * \note   UASR register used in this API can be accessed only when the UART
 *         is in Configuration Mode A or Configuration Mode B of operation.
 */
        extern uint32_t UARTAutobaudWordLenGet(uint32_t baseAddr);

        /**
 * \brief  This API determines the baud rate being configured by the system
 *         in UART Autobauding mode.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 *
 *
 * \return This returns one of the following values:
 *         - UART_AUTOBAUD_SPEED_115200 - for baud rate of 115200 bps\n
 *         - UART_AUTOBAUD_SPEED_57600 - for baud rate of 57600 bps\n
 *         - UART_AUTOBAUD_SPEED_38400 - for baud rate of 38400 bps\n
 *         - UART_AUTOBAUD_SPEED_28800 - for baud rate of 28800 bps\n
 *         - UART_AUTOBAUD_SPEED_19200 - for baud rate of 19200 bps\n
 *         - UART_AUTOBAUD_SPEED_14400 - for baud rate of 14400 bps\n
 *         - UART_AUTOBAUD_SPEED_9600 - for baud rate of 9600 bps\n
 *         - UART_AUTOBAUD_SPEED_4800 - for baud rate of 4800 bps\n
 *         - UART_AUTOBAUD_SPEED_2400 - for baud rate of 2400 bps\n
 *         - UART_AUTOBAUD_SPEED_1200 - for baud ratebaud rate of 1200 bps\n
 *         - UART_AUTOBAUD_NO_SPEED_IDEN - for no speed identified\n
 *
 * \note   UASR register used in this API can be accessed only when the UART
 *         is in Configuration Mode A or Configuration Mode B of operation.
 */
        extern uint32_t UARTAutobaudSpeedGet(uint32_t baseAddr);

        /**
 * \brief  This API programs the Scratchpad Register with the specified
 *         value.
 *
 * \param  baseAddr      Memory address of the UART instance being used
 * \param  scratchValue This is the scratch value(temporary data) to be loaded
 *                      to the Scratchpad Register
 *
 * \return None
 */
        extern void UARTScratchPadRegWrite(uint32_t baseAddr,
                                           uint32_t scratchValue);

        /**
 * \brief  This API reads the value in Scratchpad Register.
 *
 * \param  baseAddr      Memory address of the UART instance being used
 *
 *
 * \return The value in Scratchpad Register
 */
        extern uint32_t UARTScratchPadRegRead(uint32_t baseAddr);

        /**
 * \brief  This API reads the Revision Number of the module from the Module
 *         Version Register(MVR).
 *
 * \param  baseAddr    Memory address of the UART instance being used
 *
 *
 * \return This returns the Major Revision Number(MVR[7:4] and Minor Revision
 *         Number(MVR[3:0])) of the module.
 */
        extern uint32_t UARTModuleVersionNumberGet(uint32_t baseAddr);

        /**
 * \brief  This API is used to write a specified value to the FIFO Control
 *         Register(FCR).
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 * \param  fcrValue    This specifies the value to be written to the
 *                     FCR.
 *
 * 'fcrValue' can be determined using a parameterized macro named
 * 'UART_FCR_PROGRAM'.
 * The parameters of 'UART_FCR_PROGRAM' are described below:
 * - rxFIFOTrig - specifies the Receiver FIFO Trigger Level\n
 * - txFIFOTrig - specifies the Transmitter FIFO Trigger Level\n
 * - dmaMode - specifies the DMA Mode of operation to be selected:\n
 *   Write 0 - for DMA Mode 0\n
 *         1 - for DMA Mode 1\n
 * - txClr - specifies whether or not to clear the Transmitter FIFO
 *   and resetting the counter logic to 0.\n
 * - rxClr - specifies whether or not to clear the Receiver FIFO and resetting
 *   the counter logic to 0.
 * - fifoEn - specifies whether to enable the FIFO mode for the UART or not\n
 *   0 - to enable Non-FIFO mode of operation\n
 *   1 - to enable FIFO mode of operation\n
 *
 * \return  None
 *
 * \note  1> The FIFO_EN and DMA_MODE bits of FCR can be written to only when
 *           the Baud Clock is not running(DLL and DLH register are cleared
 *           to 0). Modifying DLL and DLH registers in turn requires that the
 *           UART be operated in Disabled Mode(MDR1[2:0] = 0x7).\n
 *        2> Writing to 'TX_FIFO_TRIG' field in FCR requires that the
 *           ENHANCEDEN bit in EFR(EFR[4]) be set to 1.\n
 *        Prior to writing to the FCR, this API does the above two operations.
 *        It also restores the respective bit values after FCR has been
 *        written to.\n
 */
        extern void UARTFIFORegisterWrite(uint32_t baseAddr, uint32_t fcrValue);

        /**
 * \brief   This function controls the method of setting the Transmit DMA
 *          Threshold Value. The Transmit DMA Threshold Value can be set to
 *          a default value of 64 characters or can take the value in
 *          TX_DMA_THRESHOLD register.
 *
 * \param   baseAddr      Memory address of the UART instance being used.
 * \param   thrsCtrlFlag A value which signifies the method of setting the
 *                       Transmit DMA Threshold Value.
 *
 * 'thrsCtrlFlag' can take one of the following values:\n
 * - UART_TX_DMA_THRESHOLD_64 - for 64 characters TX DMA Threshold value\n
 * - UART_TX_DMA_THRESHOLD_REG - for Transmit DMA Threshold value to be
 *   the value in TX_DMA_THRESHOLD register.\n
 *
 * \return  None
 *
 */
        extern void UARTTxDMAThresholdControl(uint32_t baseAddr, uint32_t thrsCtrlFlag);

        /**
 * \brief   This function programs the TX_DMA_THRESHOLD register which holds
 *          Transmit DMA Threshold value to be used. When a TX DMA Threshold
 *          value other than 64 characters is required, this function can be
 *          used to program the same.
 *
 * \param   baseAddr     Memory address of the UART instance being used.
 * \param   thrsValue   The Transmit DMA Threshold Value.
 *
 * 'thrsValue' can take one of the following value - (0 <= thrsValue <= 63).\n
 *
 * \return  None
 *
 */
        extern void UARTTxDMAThresholdValConfig(uint32_t baseAddr, uint32_t thrsValue);

        /**
 *  \brief    This function sets the receiver FIFO trigger level.\n
 *
 *  \param    baseAddr     Memory address of the UART instance used.\n
 *  \param    rxLevel     This contains the settings for the FIFO threshold
 *                        level. While setting the FIFO threshold, it shall be
 *                        assumed that the FIFO is being enabled.This can take
 *                        one of the four following values:
 *                        1> UART_RX_TRIG_LEVEL_1
 *                        2> UART_RX_TRIG_LEVEL_4
 *                        3> UART_RX_TRIG_LEVEL_8
 *                        4> UART_RX_TRIG_LEVEL_14
 *  \return   None.\n
 *
 *  \note   This API needs needs to be called to configure the FIFO without
 *          enabling the DMA mode. If DMA mode is being used, the
 *          UARTDMAEnable() should be used to configure the FIFO.
 *
 * \note    FIFO Control Register(FCR) and the Interrupt Identification Register
 *          (IIR) share a common offset address. FCR register has write-only \n
 *          permissions and IIR register has read-only permissions. Therefore,
 *          reading from this address will read the value in IIR register and \n
 *          writing to this address will write the value into the FCR register.
 *          Since these share the same address, the integrity of bits in FCR \n
 *          register are at risk if we read from IIR register, modify this value
 *          and write the new value into the FCR register.
 *          Thus, we retain the status of the FIFOEN bit
 *          and the RXFIFTL bit in the FCR register by
 *          reinstating them to their previous values.\n
 *
 */
        extern void UARTFIFOLevelSet(uint32_t baseAddr, uint32_t rxLevel);

        /**
 * \brief   This function gets the status of the RTS and AFE bits
 *          in the MCR register.
 *
 * \param   baseAddr   Memory address of the UART instance being used.
 *
 *
 * \return  The status of RTS and AFE bits if MCR
 *
 */
        extern uint32_t UARTModemControlGet(uint32_t baseAddr);

        /**
 * \brief  This API is used to write a specified value to the FIFO Control
 *         Register(FCR) without disabling the divisor values. Preferable
 *         setting FCR during runtime.
 *
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 * \param  fcrValue    This specifies the value to be written to the
 *                     FCR.
 */
        extern void UARTFIFOCtrlRegWrite(uint32_t baseAddr, uint32_t fcrValue);

        /**
 * \brief  This API resets the Modem control register.
 *
 * \param  baseAddr    Memory address of the UART instance being used.
 *
 * \return None.
 */
        extern void UARTModemControlReset(uint32_t baseAddr);

        /**
 * \brief  This API reads the line status register value.
 *
 * \param  baseAddr     Memory address of the UART instance being used.
 *
 *
 * \return This returns the line status register value.
 *
 */
        extern uint32_t UARTReadStatus(uint32_t baseAddr);

        /**
 * \brief   This API returns the transmitter empty status i.e. if Transmitter
 *          FIFO (THR register in non-FIFO mode) and Transmitter Shift
 *          Register are empty.
 *
 * \param   baseAddr     Memory address of the UART instance being used
 *
 *
 * \return  Transmitter empty status
 *          TRUE:  Transmitter is empty
 *          FALSE: Transmitter is not empty
 */
        extern uint32_t UARTIsTransmitterEmpty(uint32_t baseAddr);

        /**
 * \brief    This API reads a byte from the Receiver Buffer Register
 *           (RBR). It checks once if any character is ready to be read.
 *
 * \param    baseAddr     Memory address of the UART instance being used.
 *
 * \param    pChar        Pointer to the byte variable which saves the byte
 *                        read from RBR if there is any char ready to be read
 *
 * \return   If the RX FIFO(or RHR) was found to have atleast one byte of
 *           data, then this API returns TRUE. Else it returns FALSE.
 */
        extern uint32_t UARTCharGetNonBlocking2(uint32_t baseAddr, uint8_t *pChar);

        /**
 * \brief   This API waits for the arrival of atleast one byte into the
 *          Receiver FIFO or until a specified timeout value gets decremented
 *          to zero, whichever happens first.This is a replacement for the
 *          UARTCharGetTimeout which will incorrectly return timeout if
 *          the UART receives 0xFF as data.
 *
 * \param   baseAddr     Memory address of the UART instance being used.
 * \param   timeOutVal  The timeout value that is to be used. This timeout
 *                      value gets decremented once per iteration of the
 *                      wait loop.
 *                      wait loop.
 * \param    pChar      Pointer to the byte variable which saves the byte
 *                      read from RHR if there is any char ready to be read
 *
 *
 * \return  This returns either of the below two values:\n
 *          1) If a character was read in before the timeout value gets
 *             decremented to zero, this API returns TRUE.\n
 *          2) If no character was read within the timeout value getting
 *             decremented to zero, this API returns FASLE.\n
 */
        extern uint32_t UARTCharGetTimeout2(uint32_t baseAddr,
                                            uint32_t timeOutVal,
                                            uint8_t *pChar);
#ifdef __cplusplus
}
#endif
#endif
/** @} */
/********************************* End of File********************************/
