/********************************************************************
 * Copyright (C) 2013-2014 Texas Instruments Incorporated.
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
#ifndef CSLR_UART_H
#define CSLR_UART_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <ti/am335x/cslr.h>
#include <ti/am335x/tistdtypes.h>

    /**************************************************************************
* Register Overlay Structure for __ALL__
**************************************************************************/
    typedef struct
    {
        volatile Uint32 DLL;
        volatile Uint32 DLH;
        volatile Uint32 EFR;
        volatile Uint32 LCR;
        volatile Uint32 XON1_ADDR1;
        volatile Uint32 XON2_ADDR2;
        volatile Uint32 TCR;
        volatile Uint32 TLR;
        volatile Uint32 MDR1;
        volatile Uint32 MDR2;
        volatile Uint32 TXFLL;
        volatile Uint32 TXFLH;
        volatile Uint32 RXFLL;
        volatile Uint32 RXFLH;
        volatile Uint32 UASR;
        volatile Uint32 ACREG;
        volatile Uint32 SCR;
        volatile Uint32 SSR;
        volatile Uint32 EBLR;
        volatile Uint8 RSVD0[4];
        volatile Uint32 MVR;
        volatile Uint32 SYSC;
        volatile Uint32 SYSS;
        volatile Uint32 WER;
        volatile Uint32 CFPS;
        volatile Uint32 RXFIFO_LVL;
        volatile Uint32 TXFIFO_LVL;
        volatile Uint32 IER2;
        volatile Uint32 ISR2;
        volatile Uint32 FREQ_SEL;
        volatile Uint32 ABAUD_1ST_CHAR;
        volatile Uint32 BAUD_2ND_CHAR;
        volatile Uint32 MDR3;
        volatile Uint32 TX_DMA_THRESHOLD;
    } CSL_UartRegs;

/**************************************************************************
* Register Macros
**************************************************************************/

/* The receiver section consists of the receiver holding register (RHR) and 
 * the receiver shift register. The RHR is actually a 64-byte FIFO. The 
 * receiver shift register receives serial data from RX input. The data is 
 * converted to parallel data and moved to the RHR. If the FIFO is disabled 
 * location zero of the FIFO is used to store the single data character. Note: 
 * If an overflow occurs the data in the RHR is not overwritten. */
#define CSL_UART_RHR (0x0U)

/* The transmitter section consists of the transmit holding register (THR) and 
 * the transmit shift register. The transmit holding register is actually a 
 * 64-byte FIFO. The LH writes data to the THR. The data is placed into the 
 * transmit shift register where it is shifted out serially on the TX output. 
 * If the FIFO is disabled location zero of the FIFO is used to store the 
 * data. */
#define CSL_UART_THR (0x0U)

/* The interrupt enable register (IER) can be programmed to enable/disable any 
 * interrupt. There are seven types of interrupt in this mode: receiver error, 
 * RHR interrupt, THR interrupt, XOFF received and CTS*-/-RTS* change of state 
 * from low to high. Each interrupt can be enabled/disabled individually. 
 * There is also a sleep mode enable bit. */
#define CSL_UART_IER_UART (0x4U)

/* The IIR is a read-only register, which provides the source of the interrupt 
 * in a prioritized manner. */
#define CSL_UART_IIR_UART (0x8U)

/* Notes: Bits 4 and 5 can only be written to when EFR[4] = 1 Bits 0 to 3 can 
 * be changed only when the baud clock is not running (DLL and DLH set to 0) 
 * See Table 31 for FCR[5:4] setting restriction when SCR[6]=1 See Table 32 
 * for FCR[7:6] setting restriction when SCR[7]=1 */
#define CSL_UART_FCR (0x8U)

/* LCR[6:0] define parameters of the transmission and reception. Note: As soon 
 * as LCR[6] is set to 1, the TX line is forced to 0 and remains in this state 
 * as long as LCR[6] = 1. */
#define CSL_UART_LCR (0xCU)

/* MCR[3:0] controls the interface with the modem, data set or peripheral 
 * device that is emulating the modem. */
#define CSL_UART_MCR (0x10U)

/* LSR_UART */
#define CSL_UART_LSR_UART (0x14U)

/* This register provides information about the current state of the control 
 * lines from the modem, data set or peripheral device to the LH. It also 
 * indicates when a control input from the modem changes state. */
#define CSL_UART_MSR (0x18U)

/* This read/write register does not control the module in anyway. It is 
 * intended as a scratchpad register to be used by the programmer to hold 
 * temporary data. */
#define CSL_UART_SPR (0x1CU)

/* The mode of operation can be programmed by writing to MDR1[2:0] and 
 * therefore the MDR1 must be programmed on start-up after configuration of 
 * the configuration registers (DLL, DLH, LCR). The value of MDR1[2:0] must 
 * not be changed again during normal operation. Note: If the module is 
 * disabled by setting the MODE_SELECT field to */
#define CSL_UART_MDR1 (0x20U)

/* IR-IrDA and IR-CIR modes only. MDR2[0] describes the status of the 
 * interrupt in IIR[5]. The IRTX_UNDERRUN bit should be read after an IIR[5] 
 * TX_STATUS_IT interrupt has occurred. The bits [2:1] of this register sets 
 * the trigger level for the frame status FIFO (8 entries) and must be 
 * programmed before the mode is programmed in MDR1[2:0]. Note: The MDR2[6] 
 * gives the flexibility to invert the RX pin inside the UART module to ensure 
 * that the protocol at the input of the transceiver module has the same 
 * polarity at module level. By default, the RX pin is inverted because most 
 * of transceiver invert the IR receive pin. */
#define CSL_UART_MDR2 (0x24U)

/* IrDA modes only. Reading this register effectively reads frame status 
 * information from the status FIFO (this register doesn't physically exist). 
 * Reading this register will increment the status FIFO read pointer (SFREGL 
 * and SFREGH must be read first). */
#define CSL_UART_SFLSR (0x28U)

/* IrDA modes only. The registers TXFLL and TXFLH hold the 13-bit transmit 
 * frame length (expressed in bytes). TXFLL holds the least significant bits 
 * and TXFLH holds the most significant bits. The frame length value is used 
 * if the frame length method of frame closing is used. */
#define CSL_UART_TXFLL (0x28U)

/* IR-IrDA and IR-CIR modes only. This register is used to clear internal 
 * flags, which halt transmission/reception when an underrun/overrun error 
 * occurs. Reading this register resumes the halted operation. This register 
 * does not physically exist and reads always as 0x00. */
#define CSL_UART_RESUME (0x2CU)

/* IrDA modes only. The registers TXFLL and TXFLH hold the 13-bit transmit 
 * frame length (expressed in bytes). TXFLL holds the least significant bits 
 * and TXFLH holds the most significant bits. The frame length value is used 
 * if the frame length method of frame closing is used. */
#define CSL_UART_TXFLH (0x2CU)

/* IrDA modes only. The frame lengths of received frames are written into the 
 * status FIFO. This information can be read by reading the SFREGL and SFREGH 
 * registers (i.e. these registers do not physically exist). The least 
 * significant bits are read from SFREGL and the most significant bits are 
 * read from SFREGH. Reading these registers does not alter the status FIFO 
 * read pointer. These registers should be read before the pointer is 
 * incremented by reading the SFLSR. */
#define CSL_UART_SFREGL (0x30U)

/* IrDA modes only. The registers RXFLL and RXFLH hold the 12-bit receive 
 * maximum frame length. RXFLL holds the least significant bits and RXFLH 
 * holds the most significant bits. If the intended maximum receive frame 
 * length is n bytes, then program RXFLL and RXFLH to be n + 3 in SIR or MIR 
 * modes and n + 6 in FIR mode (+3 and +6 are due to frame format with CRC and 
 * stop flag; there are two bytes associated with the FIR stop flag). */
#define CSL_UART_RXFLL (0x30U)

/* IrDA modes only. The frame lengths of received frames are written into the 
 * status FIFO. This information can be read by reading the SFREGL and SFREGH 
 * registers (i.e. these registers do not physically exist). The least 
 * significant bits are read from SFREGL and the most significant bits are 
 * read from SFREGH. Reading these registers does not alter the status FIFO 
 * read pointer. These registers should be read before the pointer is 
 * incremented by reading the SFLSR. */
#define CSL_UART_SFREGH (0x34U)

/* IrDA modes only. The registers RXFLL and RXFLH hold the 12-bit receive 
 * maximum frame length. RXFLL holds the least significant bits and RXFLH 
 * holds the most significant bits. If the intended maximum receive frame 
 * length is n bytes, then program RXFLL and RXFLH to be n + 3 in SIR or MIR 
 * modes and n + 6 in FIR mode (+3 and +6 are due to frame format with CRC and 
 * stop flag; there are two bytes associated with the FIR stop flag). */
#define CSL_UART_RXFLH (0x34U)

/* IrDA modes only. Note that BLR[6] is used to select whether 0xC0 or 0xFF 
 * start patterns are to be used, when multiple start flags are required in 
 * SIR Mode. If only one start flag is required, this will always be 0xC0. If 
 * n start flags are required, then either (n-1) 0xC0 or (n-1) 0xFF flags are 
 * sent, followed by a single 0xC0 flag (immediately preceding the first data 
 * byte). */
#define CSL_UART_BLR (0x38U)

/* IR-IrDA and IR-CIR modes only. */
#define CSL_UART_ACREG (0x3CU)

/* Note: Bit 4 enables the wake-up interrupt, but this interrupt is not mapped 
 * into the IIR register. Therefore, when an interrupt occurs and there is no 
 * interrupt pending in the IIR register, the SSR[1] bit must be checked. To 
 * clear the wake-up interrupt, bit SCR[4] must be reset to 0. */
#define CSL_UART_SCR (0x40U)

/* Note: Bit 1 is reset only when SCR[4] is reset to 0. */
#define CSL_UART_SSR (0x44U)

/* IR-IrDA and IR-CIR modes only. In IR-IrDA SIR operation, this register 
 * specifies the number of BOF + xBOFs to transmit. Value set into this 
 * register must take into account the BOF character, therefore to only sent 
 * one BOF with no XBOF this register must be set to 1. To send one BOF with N 
 * XBOF this register must be set to N+1. Furthermore, the value 0 will send 1 
 * BOF plus 255 XBOF. In IR-IrDA MIR mode, this register specifies the number 
 * of additional start flags (MIR protocol mandates a minimum of 2 start 
 * flags). In IR-CIR mode, this register specifies the number of consecutive 
 * zeros to be received before generating the RX_STOP interrupt (IIR[2]). All 
 * the received zeros are stored in the RX FIFO. When the register is set to 
 * 0, this feature is de-activated and always in reception state which can be 
 * disabled by setting the ACREG[5] to */
#define CSL_UART_EBLR (0x48U)

/* The reset value is fixed by hardware and corresponds to the RTL revision of 
 * this module. A reset has no effect on the value returned Notes: UART / IRDA 
 * SIR only module is revision 1.x (WMU_012_1 specification). UART / IRDA with 
 * SIR, MIR and FIR support is revision 2.x (WMU_012_2 specification). UART / 
 * IRDA with SIR, MIR and FIR / CIR support is revision 3.x (this 
 * specification). For example: MVR = 0x30 => Version 3.0 MVR = 0x38 => 
 * Version 3.8 */
#define CSL_UART_MVR (0x50U)

/* The auto idle bit controls a power saving technique to reduce the logic 
 * power consumption of the OCP interface. That is to say when the feature is 
 * enabled, the clock will be gated off until an OCP command for this device 
 * has been detected. When the software reset bit is set high it causes a full 
 * device reset. */
#define CSL_UART_SYSC (0x54U)

/* SYSS */
#define CSL_UART_SYSS (0x58U)

/* The UART wakeup enable register is used to mask and unmask a UART event 
 * that would subsequently notify the system. The events are any activity in 
 * the logic that could cause an interrupt and/ or an activity that would 
 * require the system to wakeup. It should be noted that even if the wakeup is 
 * disabled for certain events, if these events are also an interrupt to the 
 * UART, then the UART will still register the interrupt as such. */
#define CSL_UART_WER (0x5CU)

/* Since the Consumer IR works at modulation rates of 30 56.8 KHz, the 48 MHz 
 * clock must be pre scaled before the clock can drive the IR logic. This 
 * register sets the divisor rate to give a range to accommodate the remote 
 * control requirements in BAUD multiples of 12x. The value of the CFPS at 
 * reset is 0105 decimal which equates to a 38.1 KHz output from starting 
 * conditions. The 48 MHz carrier is prescaled by the CFPS which is then 
 * divided by the 12x BAUD multiple. */
#define CSL_UART_CFPS (0x60U)

/* Level of the RX FIFO */
#define CSL_UART_RXFIFO_LVL (0x64U)

/* Level of the TX FIFO */
#define CSL_UART_TXFIFO_LVL (0x68U)

/* Enables RX/TX FIFOs empty corresponding interrupts. */
#define CSL_UART_IER2 (0x6CU)

/* Status of RX/TX FIFOs empty corresponding interrupts. */
#define CSL_UART_ISR2 (0x70U)

/* Sample per bit value selector */
#define CSL_UART_FREQ_SEL (0x74U)

/* Unused */
#define CSL_UART_ABAUD_1ST_CHAR (0x78U)

/* Unused */
#define CSL_UART_BAUD_2ND_CHAR (0x7CU)

/* Mode definition register 3. */
#define CSL_UART_MDR3 (0x80U)

/* Use to manually set the TX DMA threshold level. MDR3[2] 
 * SET_TX_DMA_THRESHOLD must be one and must be value + tx_trigger_level <= 64 
 * (TX FIFO size). If not, 64-tx_trigger_level will be used w/o modifying the 
 * value of this register. */
#define CSL_UART_TX_DMA_THRESHOLD (0x84U)

/* This register, with DLH, stores the 14-bit divisor for generation of the 
 * baud clock in the baud rate generator. DLH stores the most-significant part 
 * of the divisor. DLL stores the least-significant part of the divisor. */
#define CSL_UART_DLL (0x0U)

/* There are 8 types of interrupt in these modes, received EOF, LSR interrupt, 
 * TX status, status FIFO interrupt, RX overrun, last byte in RX FIFO, THR 
 * interrupt and RHR interrupt and they can be enabled/disabled individually. */
#define CSL_UART_IER_IRDA (0x4U)

/* There are 6 types of interrupt in these modes, TX status, status FIFO 
 * interrupt, RX overrun, last byte in RX FIFO, THR interrupt and RHR 
 * interrupt and they can be enabled/disabled individually. */
#define CSL_UART_IER_CIR (0x4U)

/* This register, with DLL, stores the 14-bit divisor for generating the baud 
 * clock in the baud rate generator. DLH stores the most-significant part of 
 * the divisor. DLL stores the least-significant part of the divisor. */
#define CSL_UART_DLH (0x4U)

/* The interrupt line is activated whenever one of the 8 interrupts is active. */
#define CSL_UART_IIR_IRDA (0x8U)

/* The interrupt line is activated whenever one of the 6 interrupts is active. */
#define CSL_UART_IIR_CIR (0x8U)

/* Enhanced feature register This register enables or disables enhanced 
 * features. Most of the enhanced functions apply only to UART modes, but 
 * UART_EFR[4] enables write accesses to UART_FCR[5:4], the TX trigger level, 
 * which is also used in IrDA modes. */
#define CSL_UART_EFR (0x8U)

/* UART mode: XON1 character, IrDA mode: ADDR1 address */
#define CSL_UART_XON1_ADDR1 (0x10U)

/* When the LSR is read, LSR[4:2] reflect the error bits [FL, CRC, ABORT] of 
 * the frame at the top of the STATUS FIFO (next frame status to be read). */
#define CSL_UART_LSR_IRDA (0x14U)

/* Line status register in CIR mode */
#define CSL_UART_LSR_CIR (0x14U)

/* Stores the 8-bit XON2 character in UART moldes and ADDR2 address 2 for IrDA 
 * modes */
#define CSL_UART_XON2_ADDR2 (0x14U)

/* UART mode XOFF1 character */
#define CSL_UART_XOFF1 (0x18U)

/* Transmission control register This register stores the RX FIFO threshold 
 * levels to start/stop transmission during hardware/software flow control. 
 * Notes: Trigger levels from 0 to 60 bytes are available with a granularity 
 * of 4. (Trigger level = 4 x [4-bit register value]) The programmer must 
 * ensure that UART_TCR[3:0] > UART_TCR[7:4] when auto-RTS or software flow 
 * control is enabled to avoid a mis-operation of the device. In FIFO 
 * interrupt mode with flow control, the programmer must ensure that the 
 * trigger level to halt transmission is greater than or equal to the RX FIFO 
 * trigger level (UART_TLR[7:4] or UART_FCR[7:6]); otherwise, FIFO operation 
 * stalls. In FIFO DMA mode with flow control, this concept does not exist 
 * because a DMA request is sent each time a byte is received. */
#define CSL_UART_TCR (0x18U)

/* UART mode XOFF2 character */
#define CSL_UART_XOFF2 (0x1CU)

/* Trigger level register This register stores the programmable transmit and 
 * RX FIFO trigger levels for DMA and IRQ generation. */
#define CSL_UART_TLR (0x1CU)

/* UART autobauding status register UART autobauding mode only. This status 
 * register returns the speed, the number of bits by characters, and the type 
 * of the parity in UART autobauding mode. In autobauding mode, the input 
 * frequency of the UART modem must be fixed to 48 MHz. Any other module clock 
 * frequency results in incorrect baud rate recognition. Note: When the UART 
 * is in autobauding mode, this register, instead of the LCR, DLL, and DLH 
 * registers, is used to set up transmission according to the characteristics 
 * of the previous reception. To reset the autobauding hardware (to start a 
 * new AT detection), set MDR1[2:0] to 111 (reset value), then set MDR1[2:1] 
 * to 010 (UART in autobaud mode). To set the UART to standard mode (no 
 * autobaud), set MDR1[2:1] to 000. */
#define CSL_UART_UASR (0x38U)

    /**************************************************************************
* Field Definition Macros
**************************************************************************/

    /* RHR */

#define CSL_UART_RHR_RHR_MASK (0x000000FFU)
#define CSL_UART_RHR_RHR_SHIFT (0U)
#define CSL_UART_RHR_RHR_RESETVAL (0x00000000U)
#define CSL_UART_RHR_RHR_MAX (0x000000ffU)

#define CSL_UART_RHR_RESETVAL (0x00000000U)

    /* THR */

#define CSL_UART_THR_THR_MASK (0x000000FFU)
#define CSL_UART_THR_THR_SHIFT (0U)
#define CSL_UART_THR_THR_RESETVAL (0x00000000U)
#define CSL_UART_THR_THR_MAX (0x000000ffU)

#define CSL_UART_THR_RESETVAL (0x00000000U)

    /* IER_UART */

#define CSL_UART_IER_UART_RHR_IT_MASK (0x00000001U)
#define CSL_UART_IER_UART_RHR_IT_SHIFT (0U)
#define CSL_UART_IER_UART_RHR_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_UART_RHR_IT_RHR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_UART_RHR_IT_RHR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_UART_THR_IT_MASK (0x00000002U)
#define CSL_UART_IER_UART_THR_IT_SHIFT (1U)
#define CSL_UART_IER_UART_THR_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_UART_THR_IT_THR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_UART_THR_IT_THR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_UART_LINE_STS_IT_MASK (0x00000004U)
#define CSL_UART_IER_UART_LINE_STS_IT_SHIFT (2U)
#define CSL_UART_IER_UART_LINE_STS_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_UART_LINE_STS_IT_LINE_STS_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IER_UART_LINE_STS_IT_LINE_STS_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IER_UART_MODEM_STS_IT_MASK (0x00000008U)
#define CSL_UART_IER_UART_MODEM_STS_IT_SHIFT (3U)
#define CSL_UART_IER_UART_MODEM_STS_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_UART_MODEM_STS_IT_MODEM_STS_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_UART_MODEM_STS_IT_MODEM_STS_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_UART_SLEEP_MODE_MASK (0x00000010U)
#define CSL_UART_IER_UART_SLEEP_MODE_SHIFT (4U)
#define CSL_UART_IER_UART_SLEEP_MODE_RESETVAL (0x00000000U)
#define CSL_UART_IER_UART_SLEEP_MODE_SLEEP_MODE_VALUE_1 (0x00000001U)
#define CSL_UART_IER_UART_SLEEP_MODE_SLEEP_MODE_VALUE_0 (0x00000000U)

#define CSL_UART_IER_UART_XOFF_IT_MASK (0x00000020U)
#define CSL_UART_IER_UART_XOFF_IT_SHIFT (5U)
#define CSL_UART_IER_UART_XOFF_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_UART_XOFF_IT_XOFF_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_UART_XOFF_IT_XOFF_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_UART_RTS_IT_MASK (0x00000040U)
#define CSL_UART_IER_UART_RTS_IT_SHIFT (6U)
#define CSL_UART_IER_UART_RTS_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_UART_RTS_IT_RTS_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_UART_RTS_IT_RTS_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_UART_CTS_IT_MASK (0x00000080U)
#define CSL_UART_IER_UART_CTS_IT_SHIFT (7U)
#define CSL_UART_IER_UART_CTS_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_UART_CTS_IT_CTS_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_UART_CTS_IT_CTS_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_UART_RESETVAL (0x00000000U)

    /* IIR_UART */

#define CSL_UART_IIR_UART_IT_PENDING_MASK (0x00000001U)
#define CSL_UART_IIR_UART_IT_PENDING_SHIFT (0U)
#define CSL_UART_IIR_UART_IT_PENDING_RESETVAL (0x00000001U)
#define CSL_UART_IIR_UART_IT_PENDING_IT_PENDING_VALUE_0 (0x00000000U)
#define CSL_UART_IIR_UART_IT_PENDING_IT_PENDING_VALUE_1 (0x00000001U)

#define CSL_UART_IIR_UART_IT_TYPE_MASK (0x0000003EU)
#define CSL_UART_IIR_UART_IT_TYPE_SHIFT (1U)
#define CSL_UART_IIR_UART_IT_TYPE_RESETVAL (0x00000000U)
#define CSL_UART_IIR_UART_IT_TYPE_MAX (0x0000001fU)

#define CSL_UART_IIR_UART_FCR_MIRROR_MASK (0x000000C0U)
#define CSL_UART_IIR_UART_FCR_MIRROR_SHIFT (6U)
#define CSL_UART_IIR_UART_FCR_MIRROR_RESETVAL (0x00000000U)
#define CSL_UART_IIR_UART_FCR_MIRROR_MAX (0x00000003U)

#define CSL_UART_IIR_UART_RESETVAL (0x00000001U)

    /* FCR */

#define CSL_UART_FCR_FIFO_EN_MASK (0x00000001U)
#define CSL_UART_FCR_FIFO_EN_SHIFT (0U)
#define CSL_UART_FCR_FIFO_EN_RESETVAL (0x00000000U)
#define CSL_UART_FCR_FIFO_EN_FIFO_EN_VALUE_1 (0x00000001U)
#define CSL_UART_FCR_FIFO_EN_FIFO_EN_VALUE_0 (0x00000000U)

#define CSL_UART_FCR_RX_FIFO_CLEAR_MASK (0x00000002U)
#define CSL_UART_FCR_RX_FIFO_CLEAR_SHIFT (1U)
#define CSL_UART_FCR_RX_FIFO_CLEAR_RESETVAL (0x00000000U)
#define CSL_UART_FCR_RX_FIFO_CLEAR_RX_FIFO_CLEAR_VALUE_1 (0x00000001U)
#define CSL_UART_FCR_RX_FIFO_CLEAR_RX_FIFO_CLEAR_VALUE_0 (0x00000000U)

#define CSL_UART_FCR_TX_FIFO_CLEAR_MASK (0x00000004U)
#define CSL_UART_FCR_TX_FIFO_CLEAR_SHIFT (2U)
#define CSL_UART_FCR_TX_FIFO_CLEAR_RESETVAL (0x00000000U)
#define CSL_UART_FCR_TX_FIFO_CLEAR_TX_FIFO_CLEAR_VALUE_1 (0x00000001U)
#define CSL_UART_FCR_TX_FIFO_CLEAR_TX_FIFO_CLEAR_VALUE_0 (0x00000000U)

#define CSL_UART_FCR_DMA_MODE_MASK (0x00000008U)
#define CSL_UART_FCR_DMA_MODE_SHIFT (3U)
#define CSL_UART_FCR_DMA_MODE_RESETVAL (0x00000000U)
#define CSL_UART_FCR_DMA_MODE_DMA_MODE_VALUE_1 (0x00000001U)
#define CSL_UART_FCR_DMA_MODE_DMA_MODE_VALUE_0 (0x00000000U)

#define CSL_UART_FCR_TX_FIFO_TRIG_MASK (0x00000030U)
#define CSL_UART_FCR_TX_FIFO_TRIG_SHIFT (4U)
#define CSL_UART_FCR_TX_FIFO_TRIG_RESETVAL (0x00000000U)
#define CSL_UART_FCR_TX_FIFO_TRIG_MAX (0x00000003U)

#define CSL_UART_FCR_RX_FIFO_TRIG_MASK (0x000000C0U)
#define CSL_UART_FCR_RX_FIFO_TRIG_SHIFT (6U)
#define CSL_UART_FCR_RX_FIFO_TRIG_RESETVAL (0x00000000U)
#define CSL_UART_FCR_RX_FIFO_TRIG_MAX (0x00000003U)

#define CSL_UART_FCR_RESETVAL (0x00000000U)

    /* LCR */

#define CSL_UART_LCR_CHAR_LENGTH_MASK (0x00000003U)
#define CSL_UART_LCR_CHAR_LENGTH_SHIFT (0U)
#define CSL_UART_LCR_CHAR_LENGTH_RESETVAL (0x00000000U)
#define CSL_UART_LCR_CHAR_LENGTH_CHAR_LENGTH_VALUE_3 (0x00000003U)
#define CSL_UART_LCR_CHAR_LENGTH_CHAR_LENGTH_VALUE_2 (0x00000002U)
#define CSL_UART_LCR_CHAR_LENGTH_CHAR_LENGTH_VALUE_1 (0x00000001U)
#define CSL_UART_LCR_CHAR_LENGTH_CHAR_LENGTH_VALUE_0 (0x00000000U)

#define CSL_UART_LCR_NB_STOP_MASK (0x00000004U)
#define CSL_UART_LCR_NB_STOP_SHIFT (2U)
#define CSL_UART_LCR_NB_STOP_RESETVAL (0x00000000U)
#define CSL_UART_LCR_NB_STOP_NB_STOP_VALUE_1 (0x00000001U)
#define CSL_UART_LCR_NB_STOP_NB_STOP_VALUE_0 (0x00000000U)

#define CSL_UART_LCR_PARITY_EN_MASK (0x00000008U)
#define CSL_UART_LCR_PARITY_EN_SHIFT (3U)
#define CSL_UART_LCR_PARITY_EN_RESETVAL (0x00000000U)
#define CSL_UART_LCR_PARITY_EN_PARITY_EN_VALUE_1 (0x00000001U)
#define CSL_UART_LCR_PARITY_EN_PARITY_EN_VALUE_0 (0x00000000U)

#define CSL_UART_LCR_PARITY_TYPE1_MASK (0x00000010U)
#define CSL_UART_LCR_PARITY_TYPE1_SHIFT (4U)
#define CSL_UART_LCR_PARITY_TYPE1_RESETVAL (0x00000000U)
#define CSL_UART_LCR_PARITY_TYPE1_PARITY_TYPE1_VALUE_1 (0x00000001U)
#define CSL_UART_LCR_PARITY_TYPE1_PARITY_TYPE1_VALUE_0 (0x00000000U)

#define CSL_UART_LCR_PARITY_TYPE2_MASK (0x00000020U)
#define CSL_UART_LCR_PARITY_TYPE2_SHIFT (5U)
#define CSL_UART_LCR_PARITY_TYPE2_RESETVAL (0x00000000U)
#define CSL_UART_LCR_PARITY_TYPE2_MAX (0x00000001U)

#define CSL_UART_LCR_BREAK_EN_MASK (0x00000040U)
#define CSL_UART_LCR_BREAK_EN_SHIFT (6U)
#define CSL_UART_LCR_BREAK_EN_RESETVAL (0x00000000U)
#define CSL_UART_LCR_BREAK_EN_BREAK_EN_VALUE_1 (0x00000001U)
#define CSL_UART_LCR_BREAK_EN_BREAK_EN_VALUE_0 (0x00000000U)

#define CSL_UART_LCR_DIV_EN_MASK (0x00000080U)
#define CSL_UART_LCR_DIV_EN_SHIFT (7U)
#define CSL_UART_LCR_DIV_EN_RESETVAL (0x00000000U)
#define CSL_UART_LCR_DIV_EN_DIV_EN_VALUE_0 (0x00000000U)
#define CSL_UART_LCR_DIV_EN_DIV_EN_VALUE_1 (0x00000001U)

#define CSL_UART_LCR_RESETVAL (0x00000000U)

    /* MCR */

#define CSL_UART_MCR_LOOPBACK_EN_MASK (0x00000010U)
#define CSL_UART_MCR_LOOPBACK_EN_SHIFT (4U)
#define CSL_UART_MCR_LOOPBACK_EN_RESETVAL (0x00000000U)
#define CSL_UART_MCR_LOOPBACK_EN_LOOPBACK_EN_VALUE_1 (0x00000001U)
#define CSL_UART_MCR_LOOPBACK_EN_LOOPBACK_EN_VALUE_0 (0x00000000U)

#define CSL_UART_MCR_XON_EN_MASK (0x00000020U)
#define CSL_UART_MCR_XON_EN_SHIFT (5U)
#define CSL_UART_MCR_XON_EN_RESETVAL (0x00000000U)
#define CSL_UART_MCR_XON_EN_XON_EN_VALUE_1 (0x00000001U)
#define CSL_UART_MCR_XON_EN_XON_EN_VALUE_0 (0x00000000U)

#define CSL_UART_MCR_TCR_TLR_MASK (0x00000040U)
#define CSL_UART_MCR_TCR_TLR_SHIFT (6U)
#define CSL_UART_MCR_TCR_TLR_RESETVAL (0x00000000U)
#define CSL_UART_MCR_TCR_TLR_TCR_TLR_VALUE_1 (0x00000001U)
#define CSL_UART_MCR_TCR_TLR_TCR_TLR_VALUE_0 (0x00000000U)

#define CSL_UART_MCR_DTR_MASK (0x00000001U)
#define CSL_UART_MCR_DTR_SHIFT (0U)
#define CSL_UART_MCR_DTR_RESETVAL (0x00000000U)
#define CSL_UART_MCR_DTR_DTR_VALUE_1 (0x00000001U)
#define CSL_UART_MCR_DTR_DTR_VALUE_0 (0x00000000U)

#define CSL_UART_MCR_RTS_MASK (0x00000002U)
#define CSL_UART_MCR_RTS_SHIFT (1U)
#define CSL_UART_MCR_RTS_RESETVAL (0x00000000U)
#define CSL_UART_MCR_RTS_RTS_VALUE_1 (0x00000001U)
#define CSL_UART_MCR_RTS_RTS_VALUE_0 (0x00000000U)

#define CSL_UART_MCR_RI_STS_CH_MASK (0x00000004U)
#define CSL_UART_MCR_RI_STS_CH_SHIFT (2U)
#define CSL_UART_MCR_RI_STS_CH_RESETVAL (0x00000000U)
#define CSL_UART_MCR_RI_STS_CH_RI_STS_CH_VALUE_1 (0x00000001U)
#define CSL_UART_MCR_RI_STS_CH_RI_STS_CH_VALUE_0 (0x00000000U)

#define CSL_UART_MCR_CD_STS_CH_MASK (0x00000008U)
#define CSL_UART_MCR_CD_STS_CH_SHIFT (3U)
#define CSL_UART_MCR_CD_STS_CH_RESETVAL (0x00000000U)
#define CSL_UART_MCR_CD_STS_CH_CD_STS_CH_VALUE_1 (0x00000001U)
#define CSL_UART_MCR_CD_STS_CH_CD_STS_CH_VALUE_0 (0x00000000U)

#define CSL_UART_MCR_RESETVAL (0x00000000U)

    /* LSR_UART */

#define CSL_UART_LSR_UART_RX_FIFO_E_MASK (0x00000001U)
#define CSL_UART_LSR_UART_RX_FIFO_E_SHIFT (0U)
#define CSL_UART_LSR_UART_RX_FIFO_E_RESETVAL (0x00000000U)
#define CSL_UART_LSR_UART_RX_FIFO_E_RX_FIFO_E_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_UART_RX_FIFO_E_RX_FIFO_E_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_UART_RX_OE_MASK (0x00000002U)
#define CSL_UART_LSR_UART_RX_OE_SHIFT (1U)
#define CSL_UART_LSR_UART_RX_OE_RESETVAL (0x00000000U)
#define CSL_UART_LSR_UART_RX_OE_RX_OE_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_UART_RX_OE_RX_OE_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_UART_RX_PE_MASK (0x00000004U)
#define CSL_UART_LSR_UART_RX_PE_SHIFT (2U)
#define CSL_UART_LSR_UART_RX_PE_RESETVAL (0x00000000U)
#define CSL_UART_LSR_UART_RX_PE_RX_PE_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_UART_RX_PE_RX_PE_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_UART_RX_FE_MASK (0x00000008U)
#define CSL_UART_LSR_UART_RX_FE_SHIFT (3U)
#define CSL_UART_LSR_UART_RX_FE_RESETVAL (0x00000000U)
#define CSL_UART_LSR_UART_RX_FE_RX_FE_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_UART_RX_FE_RX_FE_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_UART_RX_BI_MASK (0x00000010U)
#define CSL_UART_LSR_UART_RX_BI_SHIFT (4U)
#define CSL_UART_LSR_UART_RX_BI_RESETVAL (0x00000000U)
#define CSL_UART_LSR_UART_RX_BI_RX_BI_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_UART_RX_BI_RX_BI_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_UART_TX_FIFO_E_MASK (0x00000020U)
#define CSL_UART_LSR_UART_TX_FIFO_E_SHIFT (5U)
#define CSL_UART_LSR_UART_TX_FIFO_E_RESETVAL (0x00000001U)
#define CSL_UART_LSR_UART_TX_FIFO_E_TX_FIFO_E_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_UART_TX_FIFO_E_TX_FIFO_E_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_UART_TX_SR_E_MASK (0x00000040U)
#define CSL_UART_LSR_UART_TX_SR_E_SHIFT (6U)
#define CSL_UART_LSR_UART_TX_SR_E_RESETVAL (0x00000001U)
#define CSL_UART_LSR_UART_TX_SR_E_TX_SR_E_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_UART_TX_SR_E_TX_SR_E_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_UART_RX_FIFO_STS_MASK (0x00000080U)
#define CSL_UART_LSR_UART_RX_FIFO_STS_SHIFT (7U)
#define CSL_UART_LSR_UART_RX_FIFO_STS_RESETVAL (0x00000000U)
#define CSL_UART_LSR_UART_RX_FIFO_STS_RX_FIFO_STS_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_UART_RX_FIFO_STS_RX_FIFO_STS_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_UART_RESETVAL (0x00000060U)

    /* MSR */

#define CSL_UART_MSR_CTS_STS_MASK (0x00000001U)
#define CSL_UART_MSR_CTS_STS_SHIFT (0U)
#define CSL_UART_MSR_CTS_STS_RESETVAL (0x00000000U)
#define CSL_UART_MSR_CTS_STS_CTS_STS_VALUE_1 (0x00000001U)

#define CSL_UART_MSR_DSR_STS_MASK (0x00000002U)
#define CSL_UART_MSR_DSR_STS_SHIFT (1U)
#define CSL_UART_MSR_DSR_STS_RESETVAL (0x00000000U)
#define CSL_UART_MSR_DSR_STS_DSR_STS_VALUE_1 (0x00000001U)

#define CSL_UART_MSR_RI_STS_MASK (0x00000004U)
#define CSL_UART_MSR_RI_STS_SHIFT (2U)
#define CSL_UART_MSR_RI_STS_RESETVAL (0x00000000U)
#define CSL_UART_MSR_RI_STS_MAX (0x00000001U)

#define CSL_UART_MSR_DCD_STS_MASK (0x00000008U)
#define CSL_UART_MSR_DCD_STS_SHIFT (3U)
#define CSL_UART_MSR_DCD_STS_RESETVAL (0x00000000U)
#define CSL_UART_MSR_DCD_STS_MAX (0x00000001U)

#define CSL_UART_MSR_NCTS_STS_MASK (0x00000010U)
#define CSL_UART_MSR_NCTS_STS_SHIFT (4U)
#define CSL_UART_MSR_NCTS_STS_RESETVAL (0x00000000U)
#define CSL_UART_MSR_NCTS_STS_MAX (0x00000001U)

#define CSL_UART_MSR_NDSR_STS_MASK (0x00000020U)
#define CSL_UART_MSR_NDSR_STS_SHIFT (5U)
#define CSL_UART_MSR_NDSR_STS_RESETVAL (0x00000000U)
#define CSL_UART_MSR_NDSR_STS_MAX (0x00000001U)

#define CSL_UART_MSR_NRI_STS_MASK (0x00000040U)
#define CSL_UART_MSR_NRI_STS_SHIFT (6U)
#define CSL_UART_MSR_NRI_STS_RESETVAL (0x00000000U)
#define CSL_UART_MSR_NRI_STS_MAX (0x00000001U)

#define CSL_UART_MSR_NCD_STS_MASK (0x00000080U)
#define CSL_UART_MSR_NCD_STS_SHIFT (7U)
#define CSL_UART_MSR_NCD_STS_RESETVAL (0x00000000U)
#define CSL_UART_MSR_NCD_STS_MAX (0x00000001U)

#define CSL_UART_MSR_RESETVAL (0x00000000U)

    /* SPR */

#define CSL_UART_SPR_SPR_WORD_MASK (0x000000FFU)
#define CSL_UART_SPR_SPR_WORD_SHIFT (0U)
#define CSL_UART_SPR_SPR_WORD_RESETVAL (0x00000000U)
#define CSL_UART_SPR_SPR_WORD_MAX (0x000000ffU)

#define CSL_UART_SPR_RESETVAL (0x00000000U)

    /* MDR1 */

#define CSL_UART_MDR1_MODE_SELECT_MASK (0x00000007U)
#define CSL_UART_MDR1_MODE_SELECT_SHIFT (0U)
#define CSL_UART_MDR1_MODE_SELECT_RESETVAL (0x00000007U)
#define CSL_UART_MDR1_MODE_SELECT_MODE_SELECT_VALUE_7 (0x00000007U)
#define CSL_UART_MDR1_MODE_SELECT_MODE_SELECT_VALUE_6 (0x00000006U)
#define CSL_UART_MDR1_MODE_SELECT_MODE_SELECT_VALUE_5 (0x00000005U)
#define CSL_UART_MDR1_MODE_SELECT_MODE_SELECT_VALUE_4 (0x00000004U)
#define CSL_UART_MDR1_MODE_SELECT_MODE_SELECT_VALUE_3 (0x00000003U)
#define CSL_UART_MDR1_MODE_SELECT_MODE_SELECT_VALUE_2 (0x00000002U)
#define CSL_UART_MDR1_MODE_SELECT_MODE_SELECT_VALUE_1 (0x00000001U)
#define CSL_UART_MDR1_MODE_SELECT_MODE_SELECT_VALUE_0 (0x00000000U)

#define CSL_UART_MDR1_IR_SLEEP_MASK (0x00000008U)
#define CSL_UART_MDR1_IR_SLEEP_SHIFT (3U)
#define CSL_UART_MDR1_IR_SLEEP_RESETVAL (0x00000000U)
#define CSL_UART_MDR1_IR_SLEEP_IR_SLEEP_VALUE_1 (0x00000001U)
#define CSL_UART_MDR1_IR_SLEEP_IR_SLEEP_VALUE_0 (0x00000000U)

#define CSL_UART_MDR1_SET_TXIR_MASK (0x00000010U)
#define CSL_UART_MDR1_SET_TXIR_SHIFT (4U)
#define CSL_UART_MDR1_SET_TXIR_RESETVAL (0x00000000U)
#define CSL_UART_MDR1_SET_TXIR_SET_TXIR_VALUE_1 (0x00000001U)
#define CSL_UART_MDR1_SET_TXIR_SET_TXIR_VALUE_0 (0x00000000U)

#define CSL_UART_MDR1_SCT_MASK (0x00000020U)
#define CSL_UART_MDR1_SCT_SHIFT (5U)
#define CSL_UART_MDR1_SCT_RESETVAL (0x00000000U)
#define CSL_UART_MDR1_SCT_SCT_VALUE_1 (0x00000001U)
#define CSL_UART_MDR1_SCT_SCT_VALUE_0 (0x00000000U)

#define CSL_UART_MDR1_SIP_MODE_MASK (0x00000040U)
#define CSL_UART_MDR1_SIP_MODE_SHIFT (6U)
#define CSL_UART_MDR1_SIP_MODE_RESETVAL (0x00000000U)
#define CSL_UART_MDR1_SIP_MODE_SIP_MODE_VALUE_1 (0x00000001U)
#define CSL_UART_MDR1_SIP_MODE_SIP_MODE_VALUE_0 (0x00000000U)

#define CSL_UART_MDR1_FRAME_END_MODE_MASK (0x00000080U)
#define CSL_UART_MDR1_FRAME_END_MODE_SHIFT (7U)
#define CSL_UART_MDR1_FRAME_END_MODE_RESETVAL (0x00000000U)
#define CSL_UART_MDR1_FRAME_END_MODE_FRAME_END_MODE_VALUE_1 (0x00000001U)
#define CSL_UART_MDR1_FRAME_END_MODE_FRAME_END_MODE_VALUE_0 (0x00000000U)

#define CSL_UART_MDR1_RESETVAL (0x00000007U)

    /* MDR2 */

#define CSL_UART_MDR2_IRTX_UNDERRUN_MASK (0x00000001U)
#define CSL_UART_MDR2_IRTX_UNDERRUN_SHIFT (0U)
#define CSL_UART_MDR2_IRTX_UNDERRUN_RESETVAL (0x00000000U)
#define CSL_UART_MDR2_IRTX_UNDERRUN_IRTX_UNDERRUN_VALUE_1 (0x00000001U)
#define CSL_UART_MDR2_IRTX_UNDERRUN_IRTX_UNDERRUN_VALUE_0 (0x00000000U)

#define CSL_UART_MDR2_STS_FIFO_TRIG_MASK (0x00000006U)
#define CSL_UART_MDR2_STS_FIFO_TRIG_SHIFT (1U)
#define CSL_UART_MDR2_STS_FIFO_TRIG_RESETVAL (0x00000000U)
#define CSL_UART_MDR2_STS_FIFO_TRIG_STS_FIFO_TRIG_VALUE_3 (0x00000003U)
#define CSL_UART_MDR2_STS_FIFO_TRIG_STS_FIFO_TRIG_VALUE_2 (0x00000002U)
#define CSL_UART_MDR2_STS_FIFO_TRIG_STS_FIFO_TRIG_VALUE_1 (0x00000001U)
#define CSL_UART_MDR2_STS_FIFO_TRIG_STS_FIFO_TRIG_VALUE_0 (0x00000000U)

#define CSL_UART_MDR2_UART_PULSE_MASK (0x00000008U)
#define CSL_UART_MDR2_UART_PULSE_SHIFT (3U)
#define CSL_UART_MDR2_UART_PULSE_RESETVAL (0x00000000U)
#define CSL_UART_MDR2_UART_PULSE_UART_PULSE_VALUE_1 (0x00000001U)
#define CSL_UART_MDR2_UART_PULSE_UART_PULSE_VALUE_0 (0x00000000U)

#define CSL_UART_MDR2_CIR_PULSE_MODE_MASK (0x00000030U)
#define CSL_UART_MDR2_CIR_PULSE_MODE_SHIFT (4U)
#define CSL_UART_MDR2_CIR_PULSE_MODE_RESETVAL (0x00000000U)
#define CSL_UART_MDR2_CIR_PULSE_MODE_CIR_PULSE_MODE_VALUE_3 (0x00000003U)
#define CSL_UART_MDR2_CIR_PULSE_MODE_CIR_PULSE_MODE_VALUE_2 (0x00000002U)
#define CSL_UART_MDR2_CIR_PULSE_MODE_CIR_PULSE_MODE_VALUE_1 (0x00000001U)
#define CSL_UART_MDR2_CIR_PULSE_MODE_CIR_PULSE_MODE_VALUE_0 (0x00000000U)

#define CSL_UART_MDR2_IRRXINVERT_MASK (0x00000040U)
#define CSL_UART_MDR2_IRRXINVERT_SHIFT (6U)
#define CSL_UART_MDR2_IRRXINVERT_RESETVAL (0x00000000U)
#define CSL_UART_MDR2_IRRXINVERT_IRRXINVERT_VALUE_1 (0x00000001U)
#define CSL_UART_MDR2_IRRXINVERT_IRRXINVERT_VALUE_0 (0x00000000U)

#define CSL_UART_MDR2_SET_TXIR_ALT_MASK (0x00000080U)
#define CSL_UART_MDR2_SET_TXIR_ALT_SHIFT (7U)
#define CSL_UART_MDR2_SET_TXIR_ALT_RESETVAL (0x00000000U)
#define CSL_UART_MDR2_SET_TXIR_ALT_SET_TXIR_ALT_VALUE_0 (0x00000000U)
#define CSL_UART_MDR2_SET_TXIR_ALT_SET_TXIR_ALT_VALUE_1 (0x00000001U)

#define CSL_UART_MDR2_RESETVAL (0x00000000U)

    /* SFLSR */

#define CSL_UART_SFLSR_CRC_ERROR_MASK (0x00000002U)
#define CSL_UART_SFLSR_CRC_ERROR_SHIFT (1U)
#define CSL_UART_SFLSR_CRC_ERROR_RESETVAL (0x00000000U)
#define CSL_UART_SFLSR_CRC_ERROR_CRC_ERROR_VALUE_1 (0x00000001U)

#define CSL_UART_SFLSR_ABORT_DETECT_MASK (0x00000004U)
#define CSL_UART_SFLSR_ABORT_DETECT_SHIFT (2U)
#define CSL_UART_SFLSR_ABORT_DETECT_RESETVAL (0x00000000U)
#define CSL_UART_SFLSR_ABORT_DETECT_ABORT_DETECT_VALUE_1 (0x00000001U)

#define CSL_UART_SFLSR_FRAME_TOO_LONG_ERROR_MASK (0x00000008U)
#define CSL_UART_SFLSR_FRAME_TOO_LONG_ERROR_SHIFT (3U)
#define CSL_UART_SFLSR_FRAME_TOO_LONG_ERROR_RESETVAL (0x00000000U)
#define CSL_UART_SFLSR_FRAME_TOO_LONG_ERROR_FRAME_TOO_LONG_ERROR_VALUE_1 (0x00000001U)

#define CSL_UART_SFLSR_OE_ERROR_MASK (0x00000010U)
#define CSL_UART_SFLSR_OE_ERROR_SHIFT (4U)
#define CSL_UART_SFLSR_OE_ERROR_RESETVAL (0x00000000U)
#define CSL_UART_SFLSR_OE_ERROR_OE_ERROR_VALUE_1 (0x00000001U)

#define CSL_UART_SFLSR_RESETVAL (0x00000000U)

    /* TXFLL */

#define CSL_UART_TXFLL_TXFLL_MASK (0x000000FFU)
#define CSL_UART_TXFLL_TXFLL_SHIFT (0U)
#define CSL_UART_TXFLL_TXFLL_RESETVAL (0x00000000U)
#define CSL_UART_TXFLL_TXFLL_MAX (0x000000ffU)

#define CSL_UART_TXFLL_RESETVAL (0x00000000U)

    /* RESUME */

#define CSL_UART_RESUME_RESUME_MASK (0x000000FFU)
#define CSL_UART_RESUME_RESUME_SHIFT (0U)
#define CSL_UART_RESUME_RESUME_RESETVAL (0x00000000U)
#define CSL_UART_RESUME_RESUME_MAX (0x000000ffU)

#define CSL_UART_RESUME_RESETVAL (0x00000000U)

    /* TXFLH */

#define CSL_UART_TXFLH_TXFLH_MASK (0x0000001FU)
#define CSL_UART_TXFLH_TXFLH_SHIFT (0U)
#define CSL_UART_TXFLH_TXFLH_RESETVAL (0x00000000U)
#define CSL_UART_TXFLH_TXFLH_MAX (0x0000001fU)

#define CSL_UART_TXFLH_RESETVAL (0x00000000U)

    /* SFREGL */

#define CSL_UART_SFREGL_SFREGL_MASK (0x000000FFU)
#define CSL_UART_SFREGL_SFREGL_SHIFT (0U)
#define CSL_UART_SFREGL_SFREGL_RESETVAL (0x00000000U)
#define CSL_UART_SFREGL_SFREGL_MAX (0x000000ffU)

#define CSL_UART_SFREGL_RESETVAL (0x00000000U)

    /* RXFLL */

#define CSL_UART_RXFLL_RXFLL_MASK (0x000000FFU)
#define CSL_UART_RXFLL_RXFLL_SHIFT (0U)
#define CSL_UART_RXFLL_RXFLL_RESETVAL (0x00000000U)
#define CSL_UART_RXFLL_RXFLL_MAX (0x000000ffU)

#define CSL_UART_RXFLL_RESETVAL (0x00000000U)

    /* SFREGH */

#define CSL_UART_SFREGH_SFREGH_MASK (0x0000000FU)
#define CSL_UART_SFREGH_SFREGH_SHIFT (0U)
#define CSL_UART_SFREGH_SFREGH_RESETVAL (0x00000000U)
#define CSL_UART_SFREGH_SFREGH_MAX (0x0000000fU)

#define CSL_UART_SFREGH_RESETVAL (0x00000000U)

    /* RXFLH */

#define CSL_UART_RXFLH_RXFLH_MASK (0x0000000FU)
#define CSL_UART_RXFLH_RXFLH_SHIFT (0U)
#define CSL_UART_RXFLH_RXFLH_RESETVAL (0x00000000U)
#define CSL_UART_RXFLH_RXFLH_MAX (0x0000000fU)

#define CSL_UART_RXFLH_RESETVAL (0x00000000U)

    /* BLR */

#define CSL_UART_BLR_XBOF_TYPE_MASK (0x00000040U)
#define CSL_UART_BLR_XBOF_TYPE_SHIFT (6U)
#define CSL_UART_BLR_XBOF_TYPE_RESETVAL (0x00000001U)
#define CSL_UART_BLR_XBOF_TYPE_XBOF_TYPE_VALUE_1 (0x00000001U)
#define CSL_UART_BLR_XBOF_TYPE_XBOF_TYPE_VALUE_0 (0x00000000U)

#define CSL_UART_BLR_STS_FIFO_RESET_MASK (0x00000080U)
#define CSL_UART_BLR_STS_FIFO_RESET_SHIFT (7U)
#define CSL_UART_BLR_STS_FIFO_RESET_RESETVAL (0x00000000U)
#define CSL_UART_BLR_STS_FIFO_RESET_MAX (0x00000001U)

#define CSL_UART_BLR_RESETVAL (0x00000040U)

    /* ACREG */

#define CSL_UART_ACREG_EOT_EN_MASK (0x00000001U)
#define CSL_UART_ACREG_EOT_EN_SHIFT (0U)
#define CSL_UART_ACREG_EOT_EN_RESETVAL (0x00000000U)
#define CSL_UART_ACREG_EOT_EN_MAX (0x00000001U)

#define CSL_UART_ACREG_ABORT_EN_MASK (0x00000002U)
#define CSL_UART_ACREG_ABORT_EN_SHIFT (1U)
#define CSL_UART_ACREG_ABORT_EN_RESETVAL (0x00000000U)
#define CSL_UART_ACREG_ABORT_EN_MAX (0x00000001U)

#define CSL_UART_ACREG_SCTX_EN_MASK (0x00000004U)
#define CSL_UART_ACREG_SCTX_EN_SHIFT (2U)
#define CSL_UART_ACREG_SCTX_EN_RESETVAL (0x00000000U)
#define CSL_UART_ACREG_SCTX_EN_MAX (0x00000001U)

#define CSL_UART_ACREG_SEND_SIP_MASK (0x00000008U)
#define CSL_UART_ACREG_SEND_SIP_SHIFT (3U)
#define CSL_UART_ACREG_SEND_SIP_RESETVAL (0x00000000U)
#define CSL_UART_ACREG_SEND_SIP_SEND_SIP_VALUE_1 (0x00000001U)
#define CSL_UART_ACREG_SEND_SIP_SEND_SIP_VALUE_0 (0x00000000U)

#define CSL_UART_ACREG_DIS_TX_UNDERRUN_MASK (0x00000010U)
#define CSL_UART_ACREG_DIS_TX_UNDERRUN_SHIFT (4U)
#define CSL_UART_ACREG_DIS_TX_UNDERRUN_RESETVAL (0x00000000U)
#define CSL_UART_ACREG_DIS_TX_UNDERRUN_DIS_TX_UNDERRUN_VALUE_1 (0x00000001U)
#define CSL_UART_ACREG_DIS_TX_UNDERRUN_DIS_TX_UNDERRUN_VALUE_0 (0x00000000U)

#define CSL_UART_ACREG_DIS_IR_RX_MASK (0x00000020U)
#define CSL_UART_ACREG_DIS_IR_RX_SHIFT (5U)
#define CSL_UART_ACREG_DIS_IR_RX_RESETVAL (0x00000000U)
#define CSL_UART_ACREG_DIS_IR_RX_DIS_IR_RX_VALUE_1 (0x00000001U)
#define CSL_UART_ACREG_DIS_IR_RX_DIS_IR_RX_VALUE_0 (0x00000000U)

#define CSL_UART_ACREG_SD_MOD_MASK (0x00000040U)
#define CSL_UART_ACREG_SD_MOD_SHIFT (6U)
#define CSL_UART_ACREG_SD_MOD_RESETVAL (0x00000000U)
#define CSL_UART_ACREG_SD_MOD_SD_MOD_VALUE_1 (0x00000001U)
#define CSL_UART_ACREG_SD_MOD_SD_MOD_VALUE_0 (0x00000000U)

#define CSL_UART_ACREG_PULSE_TYPE_MASK (0x00000080U)
#define CSL_UART_ACREG_PULSE_TYPE_SHIFT (7U)
#define CSL_UART_ACREG_PULSE_TYPE_RESETVAL (0x00000000U)
#define CSL_UART_ACREG_PULSE_TYPE_PULSE_TYPE_VALUE_1 (0x00000001U)
#define CSL_UART_ACREG_PULSE_TYPE_PULSE_TYPE_VALUE_0 (0x00000000U)

#define CSL_UART_ACREG_RESETVAL (0x00000000U)

    /* SCR */

#define CSL_UART_SCR_DMA_MODE_CTL_MASK (0x00000001U)
#define CSL_UART_SCR_DMA_MODE_CTL_SHIFT (0U)
#define CSL_UART_SCR_DMA_MODE_CTL_RESETVAL (0x00000000U)
#define CSL_UART_SCR_DMA_MODE_CTL_DMA_MODE_CTL_VALUE_1 (0x00000001U)
#define CSL_UART_SCR_DMA_MODE_CTL_DMA_MODE_CTL_VALUE_0 (0x00000000U)

#define CSL_UART_SCR_DMA_MODE_2_MASK (0x00000006U)
#define CSL_UART_SCR_DMA_MODE_2_SHIFT (1U)
#define CSL_UART_SCR_DMA_MODE_2_RESETVAL (0x00000000U)
#define CSL_UART_SCR_DMA_MODE_2_DMA_MODE_2_VALUE_3 (0x00000003U)
#define CSL_UART_SCR_DMA_MODE_2_DMA_MODE_2_VALUE_2 (0x00000002U)
#define CSL_UART_SCR_DMA_MODE_2_DMA_MODE_2_VALUE_1 (0x00000001U)
#define CSL_UART_SCR_DMA_MODE_2_DMA_MODE_2_VALUE_0 (0x00000000U)

#define CSL_UART_SCR_TX_EMPTY_CTL_IT_MASK (0x00000008U)
#define CSL_UART_SCR_TX_EMPTY_CTL_IT_SHIFT (3U)
#define CSL_UART_SCR_TX_EMPTY_CTL_IT_RESETVAL (0x00000000U)
#define CSL_UART_SCR_TX_EMPTY_CTL_IT_TX_EMPTY_CTL_IT_VALUE_1 (0x00000001U)
#define CSL_UART_SCR_TX_EMPTY_CTL_IT_TX_EMPTY_CTL_IT_VALUE_0 (0x00000000U)

#define CSL_UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE_MASK (0x00000010U)
#define CSL_UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE_SHIFT (4U)
#define CSL_UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE_RESETVAL (0x00000000U)
#define CSL_UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE_RX_CTS_DSR_WAKE_UP_ENABLE_VALUE_1 (0x00000001U)
#define CSL_UART_SCR_RX_CTS_DSR_WAKE_UP_ENABLE_RX_CTS_DSR_WAKE_UP_ENABLE_VALUE_0 (0x00000000U)

#define CSL_UART_SCR_DSR_IT_MASK (0x00000020U)
#define CSL_UART_SCR_DSR_IT_SHIFT (5U)
#define CSL_UART_SCR_DSR_IT_RESETVAL (0x00000000U)
#define CSL_UART_SCR_DSR_IT_DSR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_SCR_DSR_IT_DSR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_SCR_TX_TRIG_GRANU1_MASK (0x00000040U)
#define CSL_UART_SCR_TX_TRIG_GRANU1_SHIFT (6U)
#define CSL_UART_SCR_TX_TRIG_GRANU1_RESETVAL (0x00000000U)
#define CSL_UART_SCR_TX_TRIG_GRANU1_TX_TRIG_GRANU1_VALUE_1 (0x00000001U)
#define CSL_UART_SCR_TX_TRIG_GRANU1_TX_TRIG_GRANU1_VALUE_0 (0x00000000U)

#define CSL_UART_SCR_RX_TRIG_GRANU1_MASK (0x00000080U)
#define CSL_UART_SCR_RX_TRIG_GRANU1_SHIFT (7U)
#define CSL_UART_SCR_RX_TRIG_GRANU1_RESETVAL (0x00000000U)
#define CSL_UART_SCR_RX_TRIG_GRANU1_RX_TRIG_GRANU1_VALUE_1 (0x00000001U)
#define CSL_UART_SCR_RX_TRIG_GRANU1_RX_TRIG_GRANU1_VALUE_0 (0x00000000U)

#define CSL_UART_SCR_RESETVAL (0x00000000U)

    /* SSR */

#define CSL_UART_SSR_TX_FIFO_FULL_MASK (0x00000001U)
#define CSL_UART_SSR_TX_FIFO_FULL_SHIFT (0U)
#define CSL_UART_SSR_TX_FIFO_FULL_RESETVAL (0x00000000U)
#define CSL_UART_SSR_TX_FIFO_FULL_TX_FIFO_FULL_VALUE_1 (0x00000001U)
#define CSL_UART_SSR_TX_FIFO_FULL_TX_FIFO_FULL_VALUE_0 (0x00000000U)

#define CSL_UART_SSR_RX_CTS_DSR_WAKE_UP_STS_MASK (0x00000002U)
#define CSL_UART_SSR_RX_CTS_DSR_WAKE_UP_STS_SHIFT (1U)
#define CSL_UART_SSR_RX_CTS_DSR_WAKE_UP_STS_RESETVAL (0x00000000U)
#define CSL_UART_SSR_RX_CTS_DSR_WAKE_UP_STS_RX_CTS_DSR_WAKE_UP_STS_VALUE_1 (0x00000001U)
#define CSL_UART_SSR_RX_CTS_DSR_WAKE_UP_STS_RX_CTS_DSR_WAKE_UP_STS_VALUE_0 (0x00000000U)

#define CSL_UART_SSR_DMA_COUNTER_RST_MASK (0x00000004U)
#define CSL_UART_SSR_DMA_COUNTER_RST_SHIFT (2U)
#define CSL_UART_SSR_DMA_COUNTER_RST_RESETVAL (0x00000000U)
#define CSL_UART_SSR_DMA_COUNTER_RST_DMA_COUNTER_RST_VALUE_0 (0x00000000U)
#define CSL_UART_SSR_DMA_COUNTER_RST_DMA_COUNTER_RST_VALUE_1 (0x00000001U)

#define CSL_UART_SSR_RESETVAL (0x00000000U)

    /* EBLR */

#define CSL_UART_EBLR_EBLR_MASK (0x000000FFU)
#define CSL_UART_EBLR_EBLR_SHIFT (0U)
#define CSL_UART_EBLR_EBLR_RESETVAL (0x00000000U)
#define CSL_UART_EBLR_EBLR_MAX (0x000000ffU)

#define CSL_UART_EBLR_RESETVAL (0x00000000U)

    /* MVR */

#define CSL_UART_MVR_MINOR_MASK (0x0000003FU)
#define CSL_UART_MVR_MINOR_SHIFT (0U)
#define CSL_UART_MVR_MINOR_RESETVAL (0x00000003U)
#define CSL_UART_MVR_MINOR_MAX (0x0000003fU)

#define CSL_UART_MVR_CUSTOM_MASK (0x000000C0U)
#define CSL_UART_MVR_CUSTOM_SHIFT (6U)
#define CSL_UART_MVR_CUSTOM_RESETVAL (0x00000000U)
#define CSL_UART_MVR_CUSTOM_MAX (0x00000003U)

#define CSL_UART_MVR_MAJOR_MASK (0x00000700U)
#define CSL_UART_MVR_MAJOR_SHIFT (8U)
#define CSL_UART_MVR_MAJOR_RESETVAL (0x00000006U)
#define CSL_UART_MVR_MAJOR_MAX (0x00000007U)

#define CSL_UART_MVR_RTL_MASK (0x0000F800U)
#define CSL_UART_MVR_RTL_SHIFT (11U)
#define CSL_UART_MVR_RTL_RESETVAL (0x00000002U)
#define CSL_UART_MVR_RTL_MAX (0x0000001fU)

#define CSL_UART_MVR_FUNC_MASK (0x0FFF0000U)
#define CSL_UART_MVR_FUNC_SHIFT (16U)
#define CSL_UART_MVR_FUNC_RESETVAL (0x00000041U)
#define CSL_UART_MVR_FUNC_MAX (0x00000fffU)

#define CSL_UART_MVR_SCHEME_MASK (0xC0000000U)
#define CSL_UART_MVR_SCHEME_SHIFT (30U)
#define CSL_UART_MVR_SCHEME_RESETVAL (0x00000001U)
#define CSL_UART_MVR_SCHEME_MAX (0x00000003U)

#define CSL_UART_MVR_RESETVAL (0x50411603U)

    /* SYSC */

#define CSL_UART_SYSC_AUTOIDLE_MASK (0x00000001U)
#define CSL_UART_SYSC_AUTOIDLE_SHIFT (0U)
#define CSL_UART_SYSC_AUTOIDLE_RESETVAL (0x00000000U)
#define CSL_UART_SYSC_AUTOIDLE_AUTOIDLE_VALUE_1 (0x00000001U)
#define CSL_UART_SYSC_AUTOIDLE_AUTOIDLE_VALUE_0 (0x00000000U)

#define CSL_UART_SYSC_SOFTRESET_MASK (0x00000002U)
#define CSL_UART_SYSC_SOFTRESET_SHIFT (1U)
#define CSL_UART_SYSC_SOFTRESET_RESETVAL (0x00000000U)
#define CSL_UART_SYSC_SOFTRESET_SOFTRESET_VALUE_1 (0x00000001U)
#define CSL_UART_SYSC_SOFTRESET_SOFTRESET_VALUE_0 (0x00000000U)

#define CSL_UART_SYSC_ENAWAKEUP_MASK (0x00000004U)
#define CSL_UART_SYSC_ENAWAKEUP_SHIFT (2U)
#define CSL_UART_SYSC_ENAWAKEUP_RESETVAL (0x00000000U)
#define CSL_UART_SYSC_ENAWAKEUP_ENAWAKEUP_VALUE_1 (0x00000001U)
#define CSL_UART_SYSC_ENAWAKEUP_ENAWAKEUP_VALUE_0 (0x00000000U)

#define CSL_UART_SYSC_IDLEMODE_MASK (0x00000018U)
#define CSL_UART_SYSC_IDLEMODE_SHIFT (3U)
#define CSL_UART_SYSC_IDLEMODE_RESETVAL (0x00000000U)
#define CSL_UART_SYSC_IDLEMODE_IDLEMODE_VALUE_3 (0x00000003U)
#define CSL_UART_SYSC_IDLEMODE_IDLEMODE_VALUE_2 (0x00000002U)
#define CSL_UART_SYSC_IDLEMODE_IDLEMODE_VALUE_1 (0x00000001U)
#define CSL_UART_SYSC_IDLEMODE_IDLEMODE_VALUE_0 (0x00000000U)

#define CSL_UART_SYSC_RESETVAL (0x00000000U)

    /* SYSS */

#define CSL_UART_SYSS_RESETDONE_MASK (0x00000001U)
#define CSL_UART_SYSS_RESETDONE_SHIFT (0U)
#define CSL_UART_SYSS_RESETDONE_RESETVAL (0x00000000U)
#define CSL_UART_SYSS_RESETDONE_RESETDONE_VALUE_1 (0x00000001U)
#define CSL_UART_SYSS_RESETDONE_RESETDONE_VALUE_0 (0x00000000U)

#define CSL_UART_SYSS_RESETVAL (0x00000000U)

    /* WER */

#define CSL_UART_WER_EVENT_0_CTS_ACTIVITY_MASK (0x00000001U)
#define CSL_UART_WER_EVENT_0_CTS_ACTIVITY_SHIFT (0U)
#define CSL_UART_WER_EVENT_0_CTS_ACTIVITY_RESETVAL (0x00000001U)
#define CSL_UART_WER_EVENT_0_CTS_ACTIVITY_EVENT_0_CTS_ACTIVITY_VALUE_1 (0x00000001U)
#define CSL_UART_WER_EVENT_0_CTS_ACTIVITY_EVENT_0_CTS_ACTIVITY_VALUE_0 (0x00000000U)

#define CSL_UART_WER_EVENT_1_DSR_ACTIVITY_MASK (0x00000002U)
#define CSL_UART_WER_EVENT_1_DSR_ACTIVITY_SHIFT (1U)
#define CSL_UART_WER_EVENT_1_DSR_ACTIVITY_RESETVAL (0x00000001U)
#define CSL_UART_WER_EVENT_1_DSR_ACTIVITY_EVENT_1_DSR_ACTIVITY_VALUE_1 (0x00000001U)
#define CSL_UART_WER_EVENT_1_DSR_ACTIVITY_EVENT_1_DSR_ACTIVITY_VALUE_0 (0x00000000U)

#define CSL_UART_WER_EVENT_2_RI_ACTIVITY_MASK (0x00000004U)
#define CSL_UART_WER_EVENT_2_RI_ACTIVITY_SHIFT (2U)
#define CSL_UART_WER_EVENT_2_RI_ACTIVITY_RESETVAL (0x00000001U)
#define CSL_UART_WER_EVENT_2_RI_ACTIVITY_EVENT_2_RI_ACTIVITY_VALUE_1 (0x00000001U)
#define CSL_UART_WER_EVENT_2_RI_ACTIVITY_EVENT_2_RI_ACTIVITY_VALUE_0 (0x00000000U)

#define CSL_UART_WER_EVENT_3_DCD_CD_ACTIVITY_MASK (0x00000008U)
#define CSL_UART_WER_EVENT_3_DCD_CD_ACTIVITY_SHIFT (3U)
#define CSL_UART_WER_EVENT_3_DCD_CD_ACTIVITY_RESETVAL (0x00000001U)
#define CSL_UART_WER_EVENT_3_DCD_CD_ACTIVITY_EVENT_3_DCD_CD_ACTIVITY_VALUE_1 (0x00000001U)
#define CSL_UART_WER_EVENT_3_DCD_CD_ACTIVITY_EVENT_3_DCD_CD_ACTIVITY_VALUE_0 (0x00000000U)

#define CSL_UART_WER_EVENT_4_RX_ACTIVITY_MASK (0x00000010U)
#define CSL_UART_WER_EVENT_4_RX_ACTIVITY_SHIFT (4U)
#define CSL_UART_WER_EVENT_4_RX_ACTIVITY_RESETVAL (0x00000001U)
#define CSL_UART_WER_EVENT_4_RX_ACTIVITY_EVENT_4_RX_ACTIVITY_VALUE_1 (0x00000001U)
#define CSL_UART_WER_EVENT_4_RX_ACTIVITY_EVENT_4_RX_ACTIVITY_VALUE_0 (0x00000000U)

#define CSL_UART_WER_EVENT_5_RHR_INTERRUPT_MASK (0x00000020U)
#define CSL_UART_WER_EVENT_5_RHR_INTERRUPT_SHIFT (5U)
#define CSL_UART_WER_EVENT_5_RHR_INTERRUPT_RESETVAL (0x00000001U)
#define CSL_UART_WER_EVENT_5_RHR_INTERRUPT_EVENT_5_RHR_INTERRUPT_VALUE_1 (0x00000001U)
#define CSL_UART_WER_EVENT_5_RHR_INTERRUPT_EVENT_5_RHR_INTERRUPT_VALUE_0 (0x00000000U)

#define CSL_UART_WER_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT_MASK (0x00000040U)
#define CSL_UART_WER_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT_SHIFT (6U)
#define CSL_UART_WER_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT_RESETVAL (0x00000001U)
#define CSL_UART_WER_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT_VALUE_1 (0x00000001U)
#define CSL_UART_WER_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT_EVENT_6_RECEIVER_LINE_STATUS_INTERRUPT_VALUE_0 (0x00000000U)

#define CSL_UART_WER_EVENT_7_TX_WAKEUP_EN_MASK (0x00000080U)
#define CSL_UART_WER_EVENT_7_TX_WAKEUP_EN_SHIFT (7U)
#define CSL_UART_WER_EVENT_7_TX_WAKEUP_EN_RESETVAL (0x00000001U)
#define CSL_UART_WER_EVENT_7_TX_WAKEUP_EN_EVENT_7_TX_WAKEUP_EN_VALUE_0 (0x00000000U)
#define CSL_UART_WER_EVENT_7_TX_WAKEUP_EN_EVENT_7_TX_WAKEUP_EN_VALUE_1 (0x00000001U)

#define CSL_UART_WER_RESETVAL (0x000000ffU)

    /* CFPS */

#define CSL_UART_CFPS_CFPS_MASK (0x000000FFU)
#define CSL_UART_CFPS_CFPS_SHIFT (0U)
#define CSL_UART_CFPS_CFPS_RESETVAL (0x00000069U)
#define CSL_UART_CFPS_CFPS_MAX (0x000000ffU)

#define CSL_UART_CFPS_RESETVAL (0x00000069U)

    /* RXFIFO_LVL */

#define CSL_UART_RXFIFO_LVL_RXFIFO_LVL_MASK (0x000000FFU)
#define CSL_UART_RXFIFO_LVL_RXFIFO_LVL_SHIFT (0U)
#define CSL_UART_RXFIFO_LVL_RXFIFO_LVL_RESETVAL (0x00000000U)
#define CSL_UART_RXFIFO_LVL_RXFIFO_LVL_MAX (0x000000ffU)

#define CSL_UART_RXFIFO_LVL_RESETVAL (0x00000000U)

    /* TXFIFO_LVL */

#define CSL_UART_TXFIFO_LVL_TXFIFO_LVL_MASK (0x000000FFU)
#define CSL_UART_TXFIFO_LVL_TXFIFO_LVL_SHIFT (0U)
#define CSL_UART_TXFIFO_LVL_TXFIFO_LVL_RESETVAL (0x00000000U)
#define CSL_UART_TXFIFO_LVL_TXFIFO_LVL_MAX (0x000000ffU)

#define CSL_UART_TXFIFO_LVL_RESETVAL (0x00000000U)

    /* IER2 */

#define CSL_UART_IER2_EN_RXFIFO_EMPTY_MASK (0x00000001U)
#define CSL_UART_IER2_EN_RXFIFO_EMPTY_SHIFT (0U)
#define CSL_UART_IER2_EN_RXFIFO_EMPTY_RESETVAL (0x00000000U)
#define CSL_UART_IER2_EN_RXFIFO_EMPTY_MAX (0x00000001U)

#define CSL_UART_IER2_EN_TXFIFO_EMPTY_MASK (0x00000002U)
#define CSL_UART_IER2_EN_TXFIFO_EMPTY_SHIFT (1U)
#define CSL_UART_IER2_EN_TXFIFO_EMPTY_RESETVAL (0x00000000U)
#define CSL_UART_IER2_EN_TXFIFO_EMPTY_MAX (0x00000001U)

#define CSL_UART_IER2_RESETVAL (0x00000000U)

    /* ISR2 */

#define CSL_UART_ISR2_RXFIFO_EMPTY_STS_MASK (0x00000001U)
#define CSL_UART_ISR2_RXFIFO_EMPTY_STS_SHIFT (0U)
#define CSL_UART_ISR2_RXFIFO_EMPTY_STS_RESETVAL (0x00000001U)
#define CSL_UART_ISR2_RXFIFO_EMPTY_STS_RXFIFO_EMPTY_STS_VALUE_0 (0x00000000U)
#define CSL_UART_ISR2_RXFIFO_EMPTY_STS_RXFIFO_EMPTY_STS_VALUE_1 (0x00000001U)

#define CSL_UART_ISR2_TXFIFO_EMPTY_STS_MASK (0x00000002U)
#define CSL_UART_ISR2_TXFIFO_EMPTY_STS_SHIFT (1U)
#define CSL_UART_ISR2_TXFIFO_EMPTY_STS_RESETVAL (0x00000001U)
#define CSL_UART_ISR2_TXFIFO_EMPTY_STS_TXFIFO_EMPTY_STS_VALUE_0 (0x00000000U)
#define CSL_UART_ISR2_TXFIFO_EMPTY_STS_TXFIFO_EMPTY_STS_VALUE_1 (0x00000001U)

#define CSL_UART_ISR2_RESETVAL (0x00000003U)

    /* FREQ_SEL */

#define CSL_UART_FREQ_SEL_FREQ_SEL_MASK (0x000000FFU)
#define CSL_UART_FREQ_SEL_FREQ_SEL_SHIFT (0U)
#define CSL_UART_FREQ_SEL_FREQ_SEL_RESETVAL (0x0000001aU)
#define CSL_UART_FREQ_SEL_FREQ_SEL_MAX (0x000000ffU)

#define CSL_UART_FREQ_SEL_RESETVAL (0x0000001aU)

    /* ABAUD_1ST_CHAR */

#define CSL_UART_ABAUD_1ST_CHAR_RESETVAL (0x00000000U)

    /* BAUD_2ND_CHAR */

#define CSL_UART_BAUD_2ND_CHAR_RESETVAL (0x00000000U)

    /* MDR3 */

#define CSL_UART_MDR3_DISABLE_CIR_RX_DEMOD_MASK (0x00000001U)
#define CSL_UART_MDR3_DISABLE_CIR_RX_DEMOD_SHIFT (0U)
#define CSL_UART_MDR3_DISABLE_CIR_RX_DEMOD_RESETVAL (0x00000000U)
#define CSL_UART_MDR3_DISABLE_CIR_RX_DEMOD_DISABLE_CIR_RX_DEMOD_VALUE_0 (0x00000000U)
#define CSL_UART_MDR3_DISABLE_CIR_RX_DEMOD_DISABLE_CIR_RX_DEMOD_VALUE_1 (0x00000001U)

#define CSL_UART_MDR3_NONDEFAULT_FREQ_MASK (0x00000002U)
#define CSL_UART_MDR3_NONDEFAULT_FREQ_SHIFT (1U)
#define CSL_UART_MDR3_NONDEFAULT_FREQ_RESETVAL (0x00000000U)
#define CSL_UART_MDR3_NONDEFAULT_FREQ_MAX (0x00000001U)

#define CSL_UART_MDR3_SET_DMA_TX_THRESHOLD_MASK (0x00000004U)
#define CSL_UART_MDR3_SET_DMA_TX_THRESHOLD_SHIFT (2U)
#define CSL_UART_MDR3_SET_DMA_TX_THRESHOLD_RESETVAL (0x00000000U)
#define CSL_UART_MDR3_SET_DMA_TX_THRESHOLD_MAX (0x00000001U)

#define CSL_UART_MDR3_RESETVAL (0x00000000U)

    /* TX_DMA_THRESHOLD */

#define CSL_UART_TX_DMA_THRESHOLD_TX_DMA_THRESHOLD_MASK (0x0000001FU)
#define CSL_UART_TX_DMA_THRESHOLD_TX_DMA_THRESHOLD_SHIFT (0U)
#define CSL_UART_TX_DMA_THRESHOLD_TX_DMA_THRESHOLD_RESETVAL (0x00000000U)
#define CSL_UART_TX_DMA_THRESHOLD_TX_DMA_THRESHOLD_MAX (0x0000001fU)

#define CSL_UART_TX_DMA_THRESHOLD_RESETVAL (0x00000000U)

    /* DLL */

#define CSL_UART_DLL_CLOCK_LSB_MASK (0x000000FFU)
#define CSL_UART_DLL_CLOCK_LSB_SHIFT (0U)
#define CSL_UART_DLL_CLOCK_LSB_RESETVAL (0x00000000U)
#define CSL_UART_DLL_CLOCK_LSB_MAX (0x000000ffU)

#define CSL_UART_DLL_RESETVAL (0x00000000U)

    /* IER_IRDA */

#define CSL_UART_IER_IRDA_RHR_IT_MASK (0x00000001U)
#define CSL_UART_IER_IRDA_RHR_IT_SHIFT (0U)
#define CSL_UART_IER_IRDA_RHR_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_IRDA_RHR_IT_RHR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_IRDA_RHR_IT_RHR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_IRDA_THR_IT_MASK (0x00000002U)
#define CSL_UART_IER_IRDA_THR_IT_SHIFT (1U)
#define CSL_UART_IER_IRDA_THR_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_IRDA_THR_IT_THR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_IRDA_THR_IT_THR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_IRDA_LAST_RX_BYTE_IT_MASK (0x00000004U)
#define CSL_UART_IER_IRDA_LAST_RX_BYTE_IT_SHIFT (2U)
#define CSL_UART_IER_IRDA_LAST_RX_BYTE_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_IRDA_LAST_RX_BYTE_IT_LAST_RX_BYTE_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_IRDA_LAST_RX_BYTE_IT_LAST_RX_BYTE_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_IRDA_RX_OVERRUN_IT_MASK (0x00000008U)
#define CSL_UART_IER_IRDA_RX_OVERRUN_IT_SHIFT (3U)
#define CSL_UART_IER_IRDA_RX_OVERRUN_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_IRDA_RX_OVERRUN_IT_RX_OVERRUN_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_IRDA_RX_OVERRUN_IT_RX_OVERRUN_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_IRDA_STS_FIFO_TRIG_IT_MASK (0x00000010U)
#define CSL_UART_IER_IRDA_STS_FIFO_TRIG_IT_SHIFT (4U)
#define CSL_UART_IER_IRDA_STS_FIFO_TRIG_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_IRDA_STS_FIFO_TRIG_IT_STS_FIFO_TRIG_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_IRDA_STS_FIFO_TRIG_IT_STS_FIFO_TRIG_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_IRDA_TX_STATUS_IT_MASK (0x00000020U)
#define CSL_UART_IER_IRDA_TX_STATUS_IT_SHIFT (5U)
#define CSL_UART_IER_IRDA_TX_STATUS_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_IRDA_TX_STATUS_IT_TX_STATUS_VALUE_1 (0x00000001U)
#define CSL_UART_IER_IRDA_TX_STATUS_IT_TX_STATUS_VALUE_0 (0x00000000U)

#define CSL_UART_IER_IRDA_LINE_STS_IT_MASK (0x00000040U)
#define CSL_UART_IER_IRDA_LINE_STS_IT_SHIFT (6U)
#define CSL_UART_IER_IRDA_LINE_STS_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_IRDA_LINE_STS_IT_LINE_STS_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IER_IRDA_LINE_STS_IT_LINE_STS_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IER_IRDA_EOF_IT_MASK (0x00000080U)
#define CSL_UART_IER_IRDA_EOF_IT_SHIFT (7U)
#define CSL_UART_IER_IRDA_EOF_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_IRDA_EOF_IT_EOF_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IER_IRDA_EOF_IT_EOF_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IER_IRDA_RESETVAL (0x00000000U)

    /* IER_CIR */

#define CSL_UART_IER_CIR_RHR_IT_MASK (0x00000001U)
#define CSL_UART_IER_CIR_RHR_IT_SHIFT (0U)
#define CSL_UART_IER_CIR_RHR_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_CIR_RHR_IT_RHR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_CIR_RHR_IT_RHR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_CIR_THR_IT_MASK (0x00000002U)
#define CSL_UART_IER_CIR_THR_IT_SHIFT (1U)
#define CSL_UART_IER_CIR_THR_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_CIR_THR_IT_THR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IER_CIR_THR_IT_THR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IER_CIR_RX_STOP_IT_MASK (0x00000004U)
#define CSL_UART_IER_CIR_RX_STOP_IT_SHIFT (2U)
#define CSL_UART_IER_CIR_RX_STOP_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_CIR_RX_STOP_IT_RX_STOP_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IER_CIR_RX_STOP_IT_RX_STOP_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IER_CIR_RX_OVERRUN_IT_MASK (0x00000008U)
#define CSL_UART_IER_CIR_RX_OVERRUN_IT_SHIFT (3U)
#define CSL_UART_IER_CIR_RX_OVERRUN_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_CIR_RX_OVERRUN_IT_RX_OVERRUN_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IER_CIR_RX_OVERRUN_IT_RX_OVERRUN_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IER_CIR_TX_STATUS_IT_MASK (0x00000020U)
#define CSL_UART_IER_CIR_TX_STATUS_IT_SHIFT (5U)
#define CSL_UART_IER_CIR_TX_STATUS_IT_RESETVAL (0x00000000U)
#define CSL_UART_IER_CIR_TX_STATUS_IT_TX_STATUS_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IER_CIR_TX_STATUS_IT_TX_STATUS_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IER_CIR_RESETVAL (0x00000000U)

    /* DLH */

#define CSL_UART_DLH_CLOCK_MSB_MASK (0x0000003FU)
#define CSL_UART_DLH_CLOCK_MSB_SHIFT (0U)
#define CSL_UART_DLH_CLOCK_MSB_RESETVAL (0x00000000U)
#define CSL_UART_DLH_CLOCK_MSB_MAX (0x0000003fU)

#define CSL_UART_DLH_RESETVAL (0x00000000U)

    /* IIR_IRDA */

#define CSL_UART_IIR_IRDA_RHR_IT_MASK (0x00000001U)
#define CSL_UART_IIR_IRDA_RHR_IT_SHIFT (0U)
#define CSL_UART_IIR_IRDA_RHR_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_IRDA_RHR_IT_RHR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_IRDA_RHR_IT_RHR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_IRDA_THR_IT_MASK (0x00000002U)
#define CSL_UART_IIR_IRDA_THR_IT_SHIFT (1U)
#define CSL_UART_IIR_IRDA_THR_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_IRDA_THR_IT_THR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_IRDA_THR_IT_THR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_IRDA_RX_FIFO_LAST_BYTE_IT_MASK (0x00000004U)
#define CSL_UART_IIR_IRDA_RX_FIFO_LAST_BYTE_IT_SHIFT (2U)
#define CSL_UART_IIR_IRDA_RX_FIFO_LAST_BYTE_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_IRDA_RX_FIFO_LAST_BYTE_IT_RX_FIFO_LAST_BYTE_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_IRDA_RX_FIFO_LAST_BYTE_IT_RX_FIFO_LAST_BYTE_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_IRDA_RX_OE_IT_MASK (0x00000008U)
#define CSL_UART_IIR_IRDA_RX_OE_IT_SHIFT (3U)
#define CSL_UART_IIR_IRDA_RX_OE_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_IRDA_RX_OE_IT_RX_OE_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_IRDA_RX_OE_IT_RX_OE_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_IRDA_STS_FIFO_IT_MASK (0x00000010U)
#define CSL_UART_IIR_IRDA_STS_FIFO_IT_SHIFT (4U)
#define CSL_UART_IIR_IRDA_STS_FIFO_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_IRDA_STS_FIFO_IT_STS_FIFO_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_IRDA_STS_FIFO_IT_STS_FIFO_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_IRDA_TX_STATUS_IT_MASK (0x00000020U)
#define CSL_UART_IIR_IRDA_TX_STATUS_IT_SHIFT (5U)
#define CSL_UART_IIR_IRDA_TX_STATUS_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_IRDA_TX_STATUS_IT_TX_STATUS_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_IRDA_TX_STATUS_IT_TX_STATUS_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_IRDA_LINE_STS_IT_MASK (0x00000040U)
#define CSL_UART_IIR_IRDA_LINE_STS_IT_SHIFT (6U)
#define CSL_UART_IIR_IRDA_LINE_STS_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_IRDA_LINE_STS_IT_LINE_STS_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_IRDA_LINE_STS_IT_LINE_STS_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_IRDA_EOF_IT_MASK (0x00000080U)
#define CSL_UART_IIR_IRDA_EOF_IT_SHIFT (7U)
#define CSL_UART_IIR_IRDA_EOF_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_IRDA_EOF_IT_EOF_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_IRDA_EOF_IT_EOF_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_IRDA_RESETVAL (0x00000000U)

    /* IIR_CIR */

#define CSL_UART_IIR_CIR_RHR_IT_MASK (0x00000001U)
#define CSL_UART_IIR_CIR_RHR_IT_SHIFT (0U)
#define CSL_UART_IIR_CIR_RHR_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_CIR_RHR_IT_RHR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_CIR_RHR_IT_RHR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_CIR_THR_IT_MASK (0x00000002U)
#define CSL_UART_IIR_CIR_THR_IT_SHIFT (1U)
#define CSL_UART_IIR_CIR_THR_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_CIR_THR_IT_THR_IT_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_CIR_THR_IT_THR_IT_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_CIR_RX_STOP_IT_MASK (0x00000004U)
#define CSL_UART_IIR_CIR_RX_STOP_IT_SHIFT (2U)
#define CSL_UART_IIR_CIR_RX_STOP_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_CIR_RX_STOP_IT_RX_STOP_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_CIR_RX_STOP_IT_RX_STOP_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_CIR_RX_OE_IT_MASK (0x00000008U)
#define CSL_UART_IIR_CIR_RX_OE_IT_SHIFT (3U)
#define CSL_UART_IIR_CIR_RX_OE_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_CIR_RX_OE_IT_RX_OE_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_CIR_RX_OE_IT_RX_OE_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_CIR_TX_STATUS_IT_MASK (0x00000020U)
#define CSL_UART_IIR_CIR_TX_STATUS_IT_SHIFT (5U)
#define CSL_UART_IIR_CIR_TX_STATUS_IT_RESETVAL (0x00000000U)
#define CSL_UART_IIR_CIR_TX_STATUS_IT_TX_STATUS_IT_U_VALUE_1 (0x00000001U)
#define CSL_UART_IIR_CIR_TX_STATUS_IT_TX_STATUS_IT_U_VALUE_0 (0x00000000U)

#define CSL_UART_IIR_CIR_RESETVAL (0x00000000U)

    /* EFR */

#define CSL_UART_EFR_SW_FLOW_CONTROL_MASK (0x0000000FU)
#define CSL_UART_EFR_SW_FLOW_CONTROL_SHIFT (0U)
#define CSL_UART_EFR_SW_FLOW_CONTROL_RESETVAL (0x00000000U)
#define CSL_UART_EFR_SW_FLOW_CONTROL_MAX (0x0000000fU)

#define CSL_UART_EFR_ENHANCED_EN_MASK (0x00000010U)
#define CSL_UART_EFR_ENHANCED_EN_SHIFT (4U)
#define CSL_UART_EFR_ENHANCED_EN_RESETVAL (0x00000000U)
#define CSL_UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_1 (0x00000001U)
#define CSL_UART_EFR_ENHANCED_EN_ENHANCED_EN_U_VALUE_0 (0x00000000U)

#define CSL_UART_EFR_SPECIAL_CHAR_DETECT_MASK (0x00000020U)
#define CSL_UART_EFR_SPECIAL_CHAR_DETECT_SHIFT (5U)
#define CSL_UART_EFR_SPECIAL_CHAR_DETECT_RESETVAL (0x00000000U)
#define CSL_UART_EFR_SPECIAL_CHAR_DETECT_SPECIAL_CHAR_DETECT_U_VALUE_1 (0x00000001U)
#define CSL_UART_EFR_SPECIAL_CHAR_DETECT_SPECIAL_CHAR_DETECT_U_VALUE_0 (0x00000000U)

#define CSL_UART_EFR_AUTO_RTS_EN_MASK (0x00000040U)
#define CSL_UART_EFR_AUTO_RTS_EN_SHIFT (6U)
#define CSL_UART_EFR_AUTO_RTS_EN_RESETVAL (0x00000000U)
#define CSL_UART_EFR_AUTO_RTS_EN_AUTO_RTS_EN_U_VALUE_1 (0x00000001U)
#define CSL_UART_EFR_AUTO_RTS_EN_AUTO_RTS_EN_U_VALUE_0 (0x00000000U)

#define CSL_UART_EFR_AUTO_CTS_EN_MASK (0x00000080U)
#define CSL_UART_EFR_AUTO_CTS_EN_SHIFT (7U)
#define CSL_UART_EFR_AUTO_CTS_EN_RESETVAL (0x00000000U)
#define CSL_UART_EFR_AUTO_CTS_EN_AUTO_CTS_EN_U_VALUE_1 (0x00000001U)
#define CSL_UART_EFR_AUTO_CTS_EN_AUTO_CTS_EN_U_VALUE_0 (0x00000000U)

#define CSL_UART_EFR_RESETVAL (0x00000000U)

    /* XON1_ADDR1 */

#define CSL_UART_XON1_ADDR1_XON_WORD1_MASK (0x000000FFU)
#define CSL_UART_XON1_ADDR1_XON_WORD1_SHIFT (0U)
#define CSL_UART_XON1_ADDR1_XON_WORD1_RESETVAL (0x00000000U)
#define CSL_UART_XON1_ADDR1_XON_WORD1_MAX (0x000000ffU)

#define CSL_UART_XON1_ADDR1_RESETVAL (0x00000000U)

    /* LSR_IRDA */

#define CSL_UART_LSR_IRDA_RX_FIFO_E_MASK (0x00000001U)
#define CSL_UART_LSR_IRDA_RX_FIFO_E_SHIFT (0U)
#define CSL_UART_LSR_IRDA_RX_FIFO_E_RESETVAL (0x00000000U)
#define CSL_UART_LSR_IRDA_RX_FIFO_E_RX_FIFO_E_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_IRDA_RX_FIFO_E_RX_FIFO_E_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_IRDA_STS_FIFO_E_MASK (0x00000002U)
#define CSL_UART_LSR_IRDA_STS_FIFO_E_SHIFT (1U)
#define CSL_UART_LSR_IRDA_STS_FIFO_E_RESETVAL (0x00000000U)
#define CSL_UART_LSR_IRDA_STS_FIFO_E_STS_FIFO_E_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_IRDA_STS_FIFO_E_STS_FIFO_E_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_IRDA_CRC_MASK (0x00000004U)
#define CSL_UART_LSR_IRDA_CRC_SHIFT (2U)
#define CSL_UART_LSR_IRDA_CRC_RESETVAL (0x00000000U)
#define CSL_UART_LSR_IRDA_CRC_CRC_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_IRDA_CRC_CRC_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_IRDA_ABORT_MASK (0x00000008U)
#define CSL_UART_LSR_IRDA_ABORT_SHIFT (3U)
#define CSL_UART_LSR_IRDA_ABORT_RESETVAL (0x00000000U)
#define CSL_UART_LSR_IRDA_ABORT_ABORT_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_IRDA_ABORT_ABORT_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_IRDA_FRAME_TOO_LONG_MASK (0x00000010U)
#define CSL_UART_LSR_IRDA_FRAME_TOO_LONG_SHIFT (4U)
#define CSL_UART_LSR_IRDA_FRAME_TOO_LONG_RESETVAL (0x00000000U)
#define CSL_UART_LSR_IRDA_FRAME_TOO_LONG_FRAME_TOO_LONG_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_IRDA_FRAME_TOO_LONG_FRAME_TOO_LONG_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_IRDA_RX_LAST_BYTE_MASK (0x00000020U)
#define CSL_UART_LSR_IRDA_RX_LAST_BYTE_SHIFT (5U)
#define CSL_UART_LSR_IRDA_RX_LAST_BYTE_RESETVAL (0x00000001U)
#define CSL_UART_LSR_IRDA_RX_LAST_BYTE_RX_LAST_BYTE_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_IRDA_RX_LAST_BYTE_RX_LAST_BYTE_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_IRDA_STS_FIFO_FULL_MASK (0x00000040U)
#define CSL_UART_LSR_IRDA_STS_FIFO_FULL_SHIFT (6U)
#define CSL_UART_LSR_IRDA_STS_FIFO_FULL_RESETVAL (0x00000001U)
#define CSL_UART_LSR_IRDA_STS_FIFO_FULL_STS_FIFO_FULL_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_IRDA_STS_FIFO_FULL_STS_FIFO_FULL_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_IRDA_THR_EMPTY_MASK (0x00000080U)
#define CSL_UART_LSR_IRDA_THR_EMPTY_SHIFT (7U)
#define CSL_UART_LSR_IRDA_THR_EMPTY_RESETVAL (0x00000000U)
#define CSL_UART_LSR_IRDA_THR_EMPTY_THR_EMPTY_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_IRDA_THR_EMPTY_THR_EMPTY_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_IRDA_RESETVAL (0x00000060U)

    /* LSR_CIR */

#define CSL_UART_LSR_CIR_RX_FIFO_E_MASK (0x00000001U)
#define CSL_UART_LSR_CIR_RX_FIFO_E_SHIFT (0U)
#define CSL_UART_LSR_CIR_RX_FIFO_E_RESETVAL (0x00000000U)
#define CSL_UART_LSR_CIR_RX_FIFO_E_RX_FIFO_E_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_CIR_RX_FIFO_E_RX_FIFO_E_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_CIR_RX_STOP_MASK (0x00000020U)
#define CSL_UART_LSR_CIR_RX_STOP_SHIFT (5U)
#define CSL_UART_LSR_CIR_RX_STOP_RESETVAL (0x00000001U)
#define CSL_UART_LSR_CIR_RX_STOP_RX_STOP_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_CIR_RX_STOP_RX_STOP_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_CIR_THR_EMPTY_MASK (0x00000080U)
#define CSL_UART_LSR_CIR_THR_EMPTY_SHIFT (7U)
#define CSL_UART_LSR_CIR_THR_EMPTY_RESETVAL (0x00000000U)
#define CSL_UART_LSR_CIR_THR_EMPTY_THR_EMPTY_VALUE_1 (0x00000001U)
#define CSL_UART_LSR_CIR_THR_EMPTY_THR_EMPTY_VALUE_0 (0x00000000U)

#define CSL_UART_LSR_CIR_RESETVAL (0x00000060U)

    /* XON2_ADDR2 */

#define CSL_UART_XON2_ADDR2_XON_WORD2_MASK (0x000000FFU)
#define CSL_UART_XON2_ADDR2_XON_WORD2_SHIFT (0U)
#define CSL_UART_XON2_ADDR2_XON_WORD2_RESETVAL (0x00000000U)
#define CSL_UART_XON2_ADDR2_XON_WORD2_MAX (0x000000ffU)

#define CSL_UART_XON2_ADDR2_RESETVAL (0x00000000U)

    /* XOFF1 */

#define CSL_UART_XOFF1_XOFF_WORD1_MASK (0x000000FFU)
#define CSL_UART_XOFF1_XOFF_WORD1_SHIFT (0U)
#define CSL_UART_XOFF1_XOFF_WORD1_RESETVAL (0x00000000U)
#define CSL_UART_XOFF1_XOFF_WORD1_MAX (0x000000ffU)

#define CSL_UART_XOFF1_RESETVAL (0x00000000U)

    /* TCR */

#define CSL_UART_TCR_RX_FIFO_TRIG_HALT_MASK (0x0000000FU)
#define CSL_UART_TCR_RX_FIFO_TRIG_HALT_SHIFT (0U)
#define CSL_UART_TCR_RX_FIFO_TRIG_HALT_RESETVAL (0x00000000U)
#define CSL_UART_TCR_RX_FIFO_TRIG_HALT_MAX (0x0000000fU)

#define CSL_UART_TCR_RX_FIFO_TRIG_START_MASK (0x000000F0U)
#define CSL_UART_TCR_RX_FIFO_TRIG_START_SHIFT (4U)
#define CSL_UART_TCR_RX_FIFO_TRIG_START_RESETVAL (0x00000000U)
#define CSL_UART_TCR_RX_FIFO_TRIG_START_MAX (0x0000000fU)

#define CSL_UART_TCR_RESETVAL (0x00000000U)

    /* XOFF2 */

#define CSL_UART_XOFF2_XOFF_WORD2_MASK (0x000000FFU)
#define CSL_UART_XOFF2_XOFF_WORD2_SHIFT (0U)
#define CSL_UART_XOFF2_XOFF_WORD2_RESETVAL (0x00000000U)
#define CSL_UART_XOFF2_XOFF_WORD2_MAX (0x000000ffU)

#define CSL_UART_XOFF2_RESETVAL (0x00000000U)

    /* TLR */

#define CSL_UART_TLR_TX_FIFO_TRIG_DMA_MASK (0x0000000FU)
#define CSL_UART_TLR_TX_FIFO_TRIG_DMA_SHIFT (0U)
#define CSL_UART_TLR_TX_FIFO_TRIG_DMA_RESETVAL (0x00000000U)
#define CSL_UART_TLR_TX_FIFO_TRIG_DMA_MAX (0x0000000fU)

#define CSL_UART_TLR_RX_FIFO_TRIG_DMA_MASK (0x000000F0U)
#define CSL_UART_TLR_RX_FIFO_TRIG_DMA_SHIFT (4U)
#define CSL_UART_TLR_RX_FIFO_TRIG_DMA_RESETVAL (0x00000000U)
#define CSL_UART_TLR_RX_FIFO_TRIG_DMA_MAX (0x0000000fU)

#define CSL_UART_TLR_RESETVAL (0x00000000U)

    /* UASR */

#define CSL_UART_UASR_SPEED_MASK (0x0000001FU)
#define CSL_UART_UASR_SPEED_SHIFT (0U)
#define CSL_UART_UASR_SPEED_RESETVAL (0x00000000U)
#define CSL_UART_UASR_SPEED_MAX (0x0000001fU)

#define CSL_UART_UASR_BIT_BY_CHAR_MASK (0x00000020U)
#define CSL_UART_UASR_BIT_BY_CHAR_SHIFT (5U)
#define CSL_UART_UASR_BIT_BY_CHAR_RESETVAL (0x00000000U)
#define CSL_UART_UASR_BIT_BY_CHAR_BIT_BY_CHAR_U_VALUE_1 (0x00000001U)
#define CSL_UART_UASR_BIT_BY_CHAR_BIT_BY_CHAR_U_VALUE_0 (0x00000000U)

#define CSL_UART_UASR_PARITY_TYPE_MASK (0x000000C0U)
#define CSL_UART_UASR_PARITY_TYPE_SHIFT (6U)
#define CSL_UART_UASR_PARITY_TYPE_RESETVAL (0x00000000U)
#define CSL_UART_UASR_PARITY_TYPE_PARITY_TYPE_U_VALUE_3 (0x00000003U)
#define CSL_UART_UASR_PARITY_TYPE_PARITY_TYPE_U_VALUE_2 (0x00000002U)
#define CSL_UART_UASR_PARITY_TYPE_PARITY_TYPE_U_VALUE_1 (0x00000001U)
#define CSL_UART_UASR_PARITY_TYPE_PARITY_TYPE_U_VALUE_0 (0x00000000U)

#define CSL_UART_UASR_RESETVAL (0x00000000U)

#ifdef __cplusplus
}
#endif
#endif
