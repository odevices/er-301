/********************************************************************
 * Copyright (C) 2013- 2016 Texas Instruments Incorporated.
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
#ifndef CSLR_MMCHS_H
#define CSLR_MMCHS_H

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
        volatile Uint32 HL_REV;
        volatile Uint32 HL_HWINFO;
        volatile Uint8 RSVD0[8];
        volatile Uint32 HL_SYSCONFIG;
        volatile Uint8 RSVD1[252];
        volatile Uint32 SYSCONFIG;
        volatile Uint32 SYSSTATUS;
        volatile Uint8 RSVD2[12];
        volatile Uint32 CSRE;
        volatile Uint32 SYSTEST;
        volatile Uint32 CON;
        volatile Uint32 PWCNT;
        volatile Uint32 DLL;
        volatile Uint8 RSVD3[200];
        volatile Uint32 SDMASA;
        volatile Uint32 BLK;
        volatile Uint32 ARG;
        volatile Uint32 CMD;
        volatile Uint32 RSP10;
        volatile Uint32 RSP32;
        volatile Uint32 RSP54;
        volatile Uint32 RSP76;
        volatile Uint32 DATA;
        volatile Uint32 PSTATE;
        volatile Uint32 HCTL;
        volatile Uint32 SYSCTL;
        volatile Uint32 STAT;
        volatile Uint32 IE;
        volatile Uint32 ISE;
        volatile Uint32 AC12;
        volatile Uint32 CAPA;
        volatile Uint32 CAPA2;
        volatile Uint32 CUR_CAPA;
        volatile Uint8 RSVD4[4];
        volatile Uint32 FE;
        volatile Uint32 ADMAES;
        volatile Uint32 ADMASAL;
        volatile Uint8 RSVD5[4];
        volatile Uint32 PVINITSD;
        volatile Uint32 PVHSSDR12;
        volatile Uint32 PVSDR25SDR50;
        volatile Uint32 PVSDR104DDR50;
        volatile Uint8 RSVD6[140];
        volatile Uint32 REV;
    } CSL_MmchsRegs;

/**************************************************************************
* Register Macros
**************************************************************************/

/* IP Revision Identifier (X.Y.R) Used by software to track features, bugs,
 * and compatibility */
#define CSL_MMCHS_HL_REV ((uint32_t)0x0U)

/* Information about the IP module's hardware configuration, i.e. typically
 * the module's HDL generics (if any). Actual field format and encoding is up
 * to the module's designer to decide. */
#define CSL_MMCHS_HL_HWINFO ((uint32_t)0x4U)

/* Clock Management Configuration Register */
#define CSL_MMCHS_HL_SYSCONFIG ((uint32_t)0x10U)

/* System Configuration Register This register allows controlling various
 * parameters of the OCP interface. */
#define CSL_MMCHS_SYSCONFIG ((uint32_t)0x110U)

/* System Status Register This register provides status information about the
 * module excluding the interrupt status information */
#define CSL_MMCHS_SYSSTATUS ((uint32_t)0x114U)

/* Card Status Response Error This register enables the host controller to
 * detect card status errors of response type R1, R1b for all cards and of R5,
 * R5b and R6 response for cards types SD or SDIO. When a bit MMCHS_CSRE[i] is
 * set to 1, if the corresponding bit at the same position in the response
 * MMCHS_RSP0[i] is set to 1, the host controller indicates a card error
 * (MMCHS_STAT[CERR]) interrupt status to avoid the host driver reading the
 * response register (MMCHS_RSP0). Note: No automatic card error detection for
 * autoCMD12 is implemented; the host system has to check autoCMD12 response
 * register (MMCHS_RESP76) for possible card errors. */
#define CSL_MMCHS_CSRE ((uint32_t)0x124U)

/* System Test Register This register is used to control the signals that
 * connect to I/O pins when the module is configured in system test (SYSTEST)
 * mode for boundary connectivity verification. Note: In SYSTEST mode, a write
 * into MMCHS_CMD register will not start a transfer. The buffer behaves as a
 * stack accessible only by the local host (push and pop operations). In this
 * mode, the Transfer Block Size (MMCHS_BLK[BLEN]) and the Blocks count for
 * current transfer (MMCHS_BLK[NBLK]) are needed to generate a Buffer write
 * ready interrupt (MMCHS_STAT[BWR]) or a Buffer read ready interrupt
 * (MMCHS_STAT[BRR]) and DMA requests if enabled. */
#define CSL_MMCHS_SYSTEST ((uint32_t)0x128U)

/* Configuration Register This register is used: - to select the functional
 * mode or the SYSTEST mode for any card. - to send an initialization sequence
 * to any card. - to enable the detection on DAT[1] of a card interrupt for
 * SDIO cards only. and also to configure : - specific data and command
 * transfers for MMC cards only. - the parameters related to the card detect
 * and write protect input signals. */
#define CSL_MMCHS_CON ((uint32_t)0x12CU)

/* Power Counter Register This register is used to program a mmc counter to
 * delay command transfers after activating the PAD power, this value depends
 * on PAD characteristics and voltage. */
#define CSL_MMCHS_PWCNT ((uint32_t)0x130U)

/* DLL control and status register This register is used for tuning procedure
 * required for SDR104 speed mode. It gives visibility and control on the DLL */
#define CSL_MMCHS_DLL ((uint32_t)0x134U)

/* SDMA System Address / Argument 2 Register */
#define CSL_MMCHS_SDMASA ((uint32_t)0x200U)

/* Transfer Length Configuration Register MMCHS_BLK[BLEN] is the block size
 * register. MMCHS_BLK[NBLK] is the block count register. This register shall
 * be used for any card. */
#define CSL_MMCHS_BLK ((uint32_t)0x204U)

/* Command Argument Register This register contains command argument specified
 * as bit 39-8 of Command-Format These registers must be initialized prior to
 * sending the command itself to the card (write action into the register
 * MMCHS_CMD register). Only exception is for a command index specifying stuff
 * bits in arguments, making a write unnecessary. */
#define CSL_MMCHS_ARG ((uint32_t)0x208U)

/* Command and Transfer Mode Register MMCHS_CMD[31:16] = the command register
 * MMCHS_CMD[15:0] = the transfer mode. This register configures the data and
 * command transfers. A write into the most significant byte send the command.
 * A write into MMCHS_CMD[15:0] registers during data transfer has no effect.
 * This register shall be used for any card. Note: In SYSTEST mode, a write
 * into MMCHS_CMD register will not start a transfer. */
#define CSL_MMCHS_CMD ((uint32_t)0x20CU)

/* Command Response[31:0] Register This 32-bit register holds bits positions
 * [31:0] of command response type R1/R1b/R2/R3/R4/R5/R5b/R6 */
#define CSL_MMCHS_RSP10 ((uint32_t)0x210U)

/* Command Response[63:32] Register This 32-bit register holds bits positions
 * [63:32] of command response type R2 */
#define CSL_MMCHS_RSP32 ((uint32_t)0x214U)

/* Command Response[95:64] Register This 32-bit register holds bits positions
 * [95:64] of command response type R2 */
#define CSL_MMCHS_RSP54 ((uint32_t)0x218U)

/* Command Response[127:96] Register This 32-bit register holds bits positions
 * [127:96] of command response type R1(Auto CMD23)/R1b(Auto CMD12)/R2 */
#define CSL_MMCHS_RSP76 ((uint32_t)0x21CU)

/* Data Register This register is the 32-bit entry point of the buffer for
 * read or write data transfers. The buffer size is 32bits x256(1024 bytes).
 * Bytes within a word are stored and read in little endian format. This
 * buffer can be used as two 512 byte buffers to transfer data efficiently
 * without reducing the throughput. Sequential and contiguous access is
 * necessary to increment the pointer correctly. Random or skipped access is
 * not allowed. In little endian, if the local host accesses this register
 * byte-wise or 16bit-wise, the least significant byte (bits [7:0]) must
 * always be written/read first. The update of the buffer address is done on
 * the most significant byte write for full 32-bit DATA register or on the
 * most significant byte of the last word of block transfer. Example 1: Byte
 * or 16-bit access Mbyteen[3:0]=0001 (1-byte) => Mbyteen[3:0]=0010 (1-byte)
 * => Mbyteen[3:0]=1100 (2-bytes) OK Mbyteen[3:0]=0001 (1-byte) =>
 * Mbyteen[3:0]=0010 (1-byte) => Mbyteen[3:0]=0100 (1-byte) OK
 * Mbyteen[3:0]=0001 (1-byte) => Mbyteen[3:0]=0010 (1-byte) =>
 * Mbyteen[3:0]=1000 (1-byte) Bad */
#define CSL_MMCHS_DATA ((uint32_t)0x220U)

/* Present State Register The Host can get status of the Host Controller from
 * this 32-bit read only register. */
#define CSL_MMCHS_PSTATE ((uint32_t)0x224U)

/* Host Control Register This register defines the host controls to set power,
 * wakeup and transfer parameters. MMCHS_HCTL[31:24] = Wakeup control
 * MMCHS_HCTL[23:16] = Block gap control MMCHS_HCTL[15:8] = Power control
 * MMCHS_HCTL[7:0] = Host control */
#define CSL_MMCHS_HCTL ((uint32_t)0x228U)

/* SD System Control Register This register defines the system controls to set
 * software resets, clock frequency management and data timeout.
 * MMCHS_SYSCTL[31:24] = Software resets MMCHS_SYSCTL[23:16] = Timeout control
 * MMCHS_SYSCTL[15:0] = Clock control */
#define CSL_MMCHS_SYSCTL ((uint32_t)0x22CU)

/* Interrupt Status Register The interrupt status regroups all the status of
 * the module internal events that can generate an interrupt.
 * MMCHS_STAT[31:16] = Error Interrupt Status MMCHS_STAT[15:0] = Normal
 * Interrupt Status */
#define CSL_MMCHS_STAT ((uint32_t)0x230U)

/* Interrupt Status Enable Register This register allows to enable/disable the
 * module to set status bits, on an event-by-event basis. MMCHS_IE[31:16] =
 * Error Interrupt Status Enable MMCHS_IE[15:0] = Normal Interrupt Status
 * Enable */
#define CSL_MMCHS_IE ((uint32_t)0x234U)

/* Interrupt Signal Enable Register This register allows to enable/disable the
 * module internal sources of status, on an event-by-event basis.
 * MMCHS_ISE[31:16] = Error Interrupt Signal Enable MMCHS_ISE[15:0] = Normal
 * Interrupt Signal Enable */
#define CSL_MMCHS_ISE ((uint32_t)0x238U)

/* Host Control 2 Register and Auto CMD Error Status Register This register is
 * used to indicate CMD12 response error of Auto CMD12 and CMD23 response
 * error of Auto CMD23. The Host driver can determine what kind of Auto CMD12
 * / CMD23 errors occur by this register. Auto CMD23 errors are indicated in
 * bit 04-01.This register is valid only when the Auto CMD Error is set. */
#define CSL_MMCHS_AC12 ((uint32_t)0x23CU)

/* Capabilities Register This register lists the capabilities of the
 * MMC/SD/SDIO host controller. */
#define CSL_MMCHS_CAPA ((uint32_t)0x240U)

/* Capabilities 2 Register This register provides the Host Driver with
 * information specific to the Host Controller implementation. The Host
 * Controller may implement these values as fixed or loaded from flash memory
 * during power on initialization. Refer to Software Reset For All in the
 * Software Reset register for loading from flash memory and completion timing
 * control. */
#define CSL_MMCHS_CAPA2 ((uint32_t)0x244U)

/* Maximum Current Capabilities Register This register indicates the maximum
 * current capability for each voltage. The value is meaningful if the voltage
 * support is set in the capabilities register (MMCHS_CAPA). Initialization of
 * this register (via a write access to this register) depends on the system
 * capabilities. The host driver shall not modify this register after the
 * initilaization. This register is only reinitialized by a hard reset (via
 * RESETN signal) */
#define CSL_MMCHS_CUR_CAPA ((uint32_t)0x248U)

/* Force Event Register for Auto CMD Error Status and Error Interrupt status
 * The Force Event Register is not a physically implemented register. Rather,
 * it is an address at which the Auto CMD Error Status Register can be
 * written. Writing 1 : set each bit of the Auto CMD Error Status Register
 * Writing 0 : no effect Rather, it is an address at which the Error Interrupt
 * Status register can be written. The effect of a write to this address will
 * be reflected in the Error Interrupt Status Register if the corresponding
 * bit of the Error Interrupt Status Enable Register is set. Writing 1 : set
 * each bit of the Error Interrupt Status Register Writing 0 : no effect Note:
 * By setting this register, the Error Interrupt can be set in the Error
 * Interrupt Status register. In order to generate interrupt signal, both the
 * Error Interrupt Status Enable and Error Interrupt Signal Enable shall be
 * set. */
#define CSL_MMCHS_FE ((uint32_t)0x250U)

/* ADMA Error Status Register When ADMA Error Interrupt is occurred, the ADMA
 * Error States field in this register holds the ADMA state and the ADMA
 * System Address Register holds the address around the error descriptor. For
 * recovering the error, the Host Driver requires the ADMA state to identify
 * the error descriptor address as follows: ST_STOP: Previous location set in
 * the ADMA System Address register is the error descriptor address ST_FDS:
 * Current location set in the ADMA System Address register is the error
 * descriptor address ST_CADR: This sate is never set because do not generate
 * ADMA error in this state. ST_TFR: Previous location set in the ADMA System
 * Address register is the error descriptor address In case of write
 * operation, the Host Driver should use ACMD22 to get the number of written
 * block rather than using this information, since unwritten data may exist in
 * the Host Controller. The Host Controller generates the ADMA Error Interrupt
 * when it detects invalid descriptor data (Valid=0) at the ST_FDS state. In
 * this case, ADMA Error State indicates that an error occurs at ST_FDS state.
 * The Host Driver may find that the Valid bit is not set in the error
 * descriptor. */
#define CSL_MMCHS_ADMAES ((uint32_t)0x254U)

/* ADMA System address Low bits */
#define CSL_MMCHS_ADMASAL ((uint32_t)0x258U)

/* Preset Value for Initialization and Default Speed modes */
#define CSL_MMCHS_PVINITSD ((uint32_t)0x260U)

/* Preset Value for High Speed and SDR12 speed modes */
#define CSL_MMCHS_PVHSSDR12 ((uint32_t)0x264U)

/* Preset Value for SDR25 and SDR50 speed modes */
#define CSL_MMCHS_PVSDR25SDR50 ((uint32_t)0x268U)

/* Preset Value for SDR104 and DDR50 speed modes */
#define CSL_MMCHS_PVSDR104DDR50 ((uint32_t)0x26CU)

/* Versions Register This register contains the hard coded RTL vendor revision
 * number, the version number of SD specification compliancy and a slot status
 * bit. MMCHS_REV[31:16] = Host controller version MMCHS_REV[15:0] = Slot
 * Interrupt Status */
#define CSL_MMCHS_REV ((uint32_t)0x2FCU)

    /**************************************************************************
* Field Definition Macros
**************************************************************************/

    /* HL_REV */

#define CSL_MMCHS_HL_REV_SCHEME_MASK ((uint32_t)0xC0000000U)
#define CSL_MMCHS_HL_REV_SCHEME_SHIFT ((uint32_t)30U)
#define CSL_MMCHS_HL_REV_SCHEME_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_HL_REV_SCHEME_LEGACY ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_REV_SCHEME_HIGHLANDER ((uint32_t)0x00000001U)

#define CSL_MMCHS_HL_REV_FUNC_MASK ((uint32_t)0x0FFF0000U)
#define CSL_MMCHS_HL_REV_FUNC_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_HL_REV_FUNC_RESETVAL ((uint32_t)0x00000020U)
#define CSL_MMCHS_HL_REV_FUNC_MAX ((uint32_t)0x00000fffU)

#define CSL_MMCHS_HL_REV_R_RTL_MASK ((uint32_t)0x0000F800U)
#define CSL_MMCHS_HL_REV_R_RTL_SHIFT ((uint32_t)11U)
#define CSL_MMCHS_HL_REV_R_RTL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_REV_R_RTL_MAX ((uint32_t)0x0000001fU)

#define CSL_MMCHS_HL_REV_X_MAJOR_MASK ((uint32_t)0x00000700U)
#define CSL_MMCHS_HL_REV_X_MAJOR_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_HL_REV_X_MAJOR_RESETVAL ((uint32_t)0x00000003U)
#define CSL_MMCHS_HL_REV_X_MAJOR_MAX ((uint32_t)0x00000007U)

#define CSL_MMCHS_HL_REV_CUSTOM_MASK ((uint32_t)0x000000C0U)
#define CSL_MMCHS_HL_REV_CUSTOM_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_HL_REV_CUSTOM_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_REV_CUSTOM_READ0 ((uint32_t)0x00000000U)

#define CSL_MMCHS_HL_REV_Y_MINOR_MASK ((uint32_t)0x0000003FU)
#define CSL_MMCHS_HL_REV_Y_MINOR_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_HL_REV_Y_MINOR_RESETVAL ((uint32_t)0x00000002U)
#define CSL_MMCHS_HL_REV_Y_MINOR_MAX ((uint32_t)0x0000003fU)

#define CSL_MMCHS_HL_REV_RESETVAL ((uint32_t)0x40200302U)

    /* HL_HWINFO */

#define CSL_MMCHS_HL_HWINFO_MADMA_EN_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_HL_HWINFO_MADMA_EN_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_HL_HWINFO_MADMA_EN_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_HL_HWINFO_MADMA_EN_NOMASTERDMA ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_HWINFO_MADMA_EN_SUPPORTADMA ((uint32_t)0x00000001U)

#define CSL_MMCHS_HL_HWINFO_MERGE_MEM_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_HL_HWINFO_MERGE_MEM_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_HL_HWINFO_MERGE_MEM_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_HWINFO_MERGE_MEM_TWOMEMBUFFER ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_HWINFO_MERGE_MEM_SINGLEMEMBUFFER ((uint32_t)0x00000001U)

#define CSL_MMCHS_HL_HWINFO_MEM_SIZE_MASK ((uint32_t)0x0000003CU)
#define CSL_MMCHS_HL_HWINFO_MEM_SIZE_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_HL_HWINFO_MEM_SIZE_RESETVAL ((uint32_t)0x00000002U)
#define CSL_MMCHS_HL_HWINFO_MEM_SIZE_MAX ((uint32_t)0x0000000fU)

#define CSL_MMCHS_HL_HWINFO_RETMODE_MASK ((uint32_t)0x00000040U)
#define CSL_MMCHS_HL_HWINFO_RETMODE_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_HL_HWINFO_RETMODE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_HWINFO_RETMODE_RETDISABLED ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_HWINFO_RETMODE_RETENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_HL_HWINFO_RESETVAL ((uint32_t)0x00000009U)

    /* HL_SYSCONFIG */

#define CSL_MMCHS_HL_SYSCONFIG_SOFTRESET_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_HL_SYSCONFIG_SOFTRESET_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_HL_SYSCONFIG_SOFTRESET_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_SYSCONFIG_SOFTRESET_RESETDONE ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_SYSCONFIG_SOFTRESET_NOACTION ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_SYSCONFIG_SOFTRESET_SOFTRESET ((uint32_t)0x00000001U)
#define CSL_MMCHS_HL_SYSCONFIG_SOFTRESET_RESETONGOING ((uint32_t)0x00000001U)

#define CSL_MMCHS_HL_SYSCONFIG_FREEEMU_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_HL_SYSCONFIG_FREEEMU_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_HL_SYSCONFIG_FREEEMU_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_SYSCONFIG_FREEEMU_EMUEN ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_SYSCONFIG_FREEEMU_EMUDIS ((uint32_t)0x00000001U)

#define CSL_MMCHS_HL_SYSCONFIG_IDLEMODE_MASK ((uint32_t)0x0000000CU)
#define CSL_MMCHS_HL_SYSCONFIG_IDLEMODE_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_HL_SYSCONFIG_IDLEMODE_RESETVAL ((uint32_t)0x00000002U)
#define CSL_MMCHS_HL_SYSCONFIG_IDLEMODE_FORCEIDLE ((uint32_t)0x00000000U)
#define CSL_MMCHS_HL_SYSCONFIG_IDLEMODE_NOIDLE ((uint32_t)0x00000001U)
#define CSL_MMCHS_HL_SYSCONFIG_IDLEMODE_SMARTIDLE ((uint32_t)0x00000002U)
#define CSL_MMCHS_HL_SYSCONFIG_IDLEMODE_SMARTIDLEWAKEUP ((uint32_t)0x00000003U)

#define CSL_MMCHS_HL_SYSCONFIG_STANDBYMODE_MASK ((uint32_t)0x00000030U)
#define CSL_MMCHS_HL_SYSCONFIG_STANDBYMODE_SHIFT ((uint32_t)4U)
#define CSL_MMCHS_HL_SYSCONFIG_STANDBYMODE_RESETVAL ((uint32_t)0x00000002U)
#define CSL_MMCHS_HL_SYSCONFIG_STANDBYMODE_SMARTSTANDBYWAKEUP (0x00000003U)
#define CSL_MMCHS_HL_SYSCONFIG_STANDBYMODE_SMARTSTANDBY ((uint32_t)0x00000002U)
#define CSL_MMCHS_HL_SYSCONFIG_STANDBYMODE_NOSTANDBY ((uint32_t)0x00000001U)
#define CSL_MMCHS_HL_SYSCONFIG_STANDBYMODE_FORCESTANDBY ((uint32_t)0x00000000U)

#define CSL_MMCHS_HL_SYSCONFIG_RESETVAL ((uint32_t)0x00000028U)

    /* SYSCONFIG */

#define CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_MASK ((uint32_t)0x00000300U)
#define CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_NONE ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_FUNC ((uint32_t)0x00000002U)
#define CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_OCP ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSCONFIG_CLOCKACTIVITY_BOTH ((uint32_t)0x00000003U)

#define CSL_MMCHS_SYSCONFIG_SIDLEMODE_MASK ((uint32_t)0x00000018U)
#define CSL_MMCHS_SYSCONFIG_SIDLEMODE_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_SYSCONFIG_SIDLEMODE_RESETVAL ((uint32_t)0x00000002U)
#define CSL_MMCHS_SYSCONFIG_SIDLEMODE_SMART ((uint32_t)0x00000002U)
#define CSL_MMCHS_SYSCONFIG_SIDLEMODE_FORCE ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCONFIG_SIDLEMODE_NOIDLE ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSCONFIG_SIDLEMODE_SMARTWAKE ((uint32_t)0x00000003U)

#define CSL_MMCHS_SYSCONFIG_SOFTRESET_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_SYSCONFIG_SOFTRESET_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_SYSCONFIG_SOFTRESET_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCONFIG_SOFTRESET_NORESET_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCONFIG_SOFTRESET_ONRESET_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSCONFIG_SOFTRESET_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCONFIG_SOFTRESET_ST_RST_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSCONFIG_AUTOIDLE_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSCONFIG_AUTOIDLE_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_SYSCONFIG_AUTOIDLE_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSCONFIG_AUTOIDLE_OFF ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCONFIG_AUTOIDLE_ON ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSCONFIG_ENAWAKEUP_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_SYSCONFIG_ENAWAKEUP_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_SYSCONFIG_ENAWAKEUP_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSCONFIG_ENAWAKEUP_DISABLED ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCONFIG_ENAWAKEUP_ENABLE ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSCONFIG_STANDBYMODE_MASK ((uint32_t)0x00003000U)
#define CSL_MMCHS_SYSCONFIG_STANDBYMODE_SHIFT ((uint32_t)12U)
#define CSL_MMCHS_SYSCONFIG_STANDBYMODE_RESETVAL ((uint32_t)0x00000002U)
#define CSL_MMCHS_SYSCONFIG_STANDBYMODE_NOIDLE ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSCONFIG_STANDBYMODE_SMART ((uint32_t)0x00000002U)
#define CSL_MMCHS_SYSCONFIG_STANDBYMODE_SMARTWAKE ((uint32_t)0x00000003U)
#define CSL_MMCHS_SYSCONFIG_STANDBYMODE_FORCE ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSCONFIG_RESETVAL ((uint32_t)0x00002015U)

    /* SYSSTATUS */

#define CSL_MMCHS_SYSSTATUS_RESETDONE_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSSTATUS_RESETDONE_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_SYSSTATUS_RESETDONE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSSTATUS_RESETDONE_DONE ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSSTATUS_RESETDONE_ONGOING ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSSTATUS_RESETVAL ((uint32_t)0x00000000U)

    /* CSRE */

#define CSL_MMCHS_CSRE_CSRE_MASK ((uint32_t)0xFFFFFFFFU)
#define CSL_MMCHS_CSRE_CSRE_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_CSRE_CSRE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CSRE_CSRE_MAX ((uint32_t)0xffffffffU)

#define CSL_MMCHS_CSRE_RESETVAL ((uint32_t)0x00000000U)

    /* SYSTEST */

#define CSL_MMCHS_SYSTEST_WAKD_MASK ((uint32_t)0x00002000U)
#define CSL_MMCHS_SYSTEST_WAKD_SHIFT ((uint32_t)13U)
#define CSL_MMCHS_SYSTEST_WAKD_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_WAKD_DRIVENHIGH_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_WAKD_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_WAKD_ONE_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_WAKD_DRIVENLOW_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSTEST_D4D_MASK ((uint32_t)0x00000100U)
#define CSL_MMCHS_SYSTEST_D4D_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_SYSTEST_D4D_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D4D_DRIVEHIGH_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D4D_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D4D_ONE_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D4D_DRIVELOW_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSTEST_CDIR_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_SYSTEST_CDIR_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_SYSTEST_CDIR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_CDIR_IN_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_CDIR_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_CDIR_OUT_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_CDIR_ONE_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSTEST_D1D_MASK ((uint32_t)0x00000020U)
#define CSL_MMCHS_SYSTEST_D1D_SHIFT ((uint32_t)5U)
#define CSL_MMCHS_SYSTEST_D1D_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D1D_DRIVEHIGH_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D1D_ONE_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D1D_DRIVELOW_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D1D_ZERO_R ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSTEST_SDWP_MASK ((uint32_t)0x00004000U)
#define CSL_MMCHS_SYSTEST_SDWP_SHIFT ((uint32_t)14U)
#define CSL_MMCHS_SYSTEST_SDWP_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_SDWP_DRIVENHIGH ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_SDWP_DRIVENLOW ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSTEST_D2D_MASK ((uint32_t)0x00000040U)
#define CSL_MMCHS_SYSTEST_D2D_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_SYSTEST_D2D_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D2D_DRIVELOW_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D2D_ONE_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D2D_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D2D_DRIVEHIGH_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSTEST_DDIR_MASK ((uint32_t)0x00000008U)
#define CSL_MMCHS_SYSTEST_DDIR_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_SYSTEST_DDIR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_DDIR_IN_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_DDIR_ONE_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_DDIR_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_DDIR_OUT_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSTEST_CDAT_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_SYSTEST_CDAT_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_SYSTEST_CDAT_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_CDAT_ONE_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_CDAT_DRIVELOW_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_CDAT_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_CDAT_DRIVEHIGH_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSTEST_D5D_MASK ((uint32_t)0x00000200U)
#define CSL_MMCHS_SYSTEST_D5D_SHIFT ((uint32_t)9U)
#define CSL_MMCHS_SYSTEST_D5D_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D5D_DRIVELOW_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D5D_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D5D_DRIVEHIGH_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D5D_ONE_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSTEST_D6D_MASK ((uint32_t)0x00000400U)
#define CSL_MMCHS_SYSTEST_D6D_SHIFT ((uint32_t)10U)
#define CSL_MMCHS_SYSTEST_D6D_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D6D_DRIVELOW_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D6D_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D6D_DRIVEHIGH_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D6D_ONE_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSTEST_SSB_MASK ((uint32_t)0x00001000U)
#define CSL_MMCHS_SYSTEST_SSB_SHIFT ((uint32_t)12U)
#define CSL_MMCHS_SYSTEST_SSB_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_SSB_SETTHEMALL_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_SSB_CLEAR_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_SSB_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_SSB_ONE_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSTEST_D3D_MASK ((uint32_t)0x00000080U)
#define CSL_MMCHS_SYSTEST_D3D_SHIFT ((uint32_t)7U)
#define CSL_MMCHS_SYSTEST_D3D_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D3D_ONE_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D3D_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D3D_DRIVEHIGH_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D3D_DRIVELOW_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSTEST_MCKD_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_MCKD_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_SYSTEST_MCKD_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_MCKD_DRIVENLOW_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_MCKD_DRIVENHIGH_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_MCKD_ONE_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_MCKD_ZERO_R ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSTEST_D0D_MASK ((uint32_t)0x00000010U)
#define CSL_MMCHS_SYSTEST_D0D_SHIFT ((uint32_t)4U)
#define CSL_MMCHS_SYSTEST_D0D_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D0D_ONE_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D0D_DRIVEHIGH_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D0D_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D0D_ZERO_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSTEST_D7D_MASK ((uint32_t)0x00000800U)
#define CSL_MMCHS_SYSTEST_D7D_SHIFT ((uint32_t)11U)
#define CSL_MMCHS_SYSTEST_D7D_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D7D_ZERO_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D7D_DRIVEHIGH_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_D7D_DRIVELOW_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_D7D_ONE_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSTEST_SDCD_MASK ((uint32_t)0x00008000U)
#define CSL_MMCHS_SYSTEST_SDCD_SHIFT ((uint32_t)15U)
#define CSL_MMCHS_SYSTEST_SDCD_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_SDCD_DRIVENHIGH ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_SDCD_DRIVENLOW ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSTEST_OBI_MASK ((uint32_t)0x00010000U)
#define CSL_MMCHS_SYSTEST_OBI_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_SYSTEST_OBI_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSTEST_OBI_HIGHLEVEL ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSTEST_OBI_LOWLEVEL ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSTEST_RESETVAL ((uint32_t)0x00000000U)

    /* CON */

#define CSL_MMCHS_CON_HR_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_CON_HR_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_CON_HR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_HR_HOSTRESP ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_HR_NOHOSTRESP ((uint32_t)0x00000000U)

#define CSL_MMCHS_CON_WPP_MASK ((uint32_t)0x00000100U)
#define CSL_MMCHS_CON_WPP_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_CON_WPP_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_WPP_ACTIVEHIGH ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_WPP_ACTIVELOW ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_STR_MASK ((uint32_t)0x00000008U)
#define CSL_MMCHS_CON_STR_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_CON_STR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_STR_STREAM ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_STR_BLOCK ((uint32_t)0x00000000U)

#define CSL_MMCHS_CON_OD_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_OD_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_CON_OD_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_OD_OPENDRAIN ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_OD_NOOPENDRAIN ((uint32_t)0x00000000U)

#define CSL_MMCHS_CON_DVAL_MASK ((uint32_t)0x00000600U)
#define CSL_MMCHS_CON_DVAL_SHIFT ((uint32_t)9U)
#define CSL_MMCHS_CON_DVAL_RESETVAL ((uint32_t)0x00000003U)
#define CSL_MMCHS_CON_DVAL_FILTERLEVEL1 ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_DVAL_FILTERLEVEL2 ((uint32_t)0x00000002U)
#define CSL_MMCHS_CON_DVAL_FILTERLEVEL0 ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_DVAL_FILTERLEVEL3 ((uint32_t)0x00000003U)

#define CSL_MMCHS_CON_INIT_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_CON_INIT_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_CON_INIT_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_INIT_NOINIT ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_INIT_INITSTREAM ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_MIT_MASK ((uint32_t)0x00000040U)
#define CSL_MMCHS_CON_MIT_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_CON_MIT_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_MIT_CTO ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_MIT_NO_CTO ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_CDP_MASK ((uint32_t)0x00000080U)
#define CSL_MMCHS_CON_CDP_SHIFT ((uint32_t)7U)
#define CSL_MMCHS_CON_CDP_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_CDP_ACTIVEHIGH ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_CDP_ACTIVELOW ((uint32_t)0x00000000U)

#define CSL_MMCHS_CON_DW8_MASK ((uint32_t)0x00000020U)
#define CSL_MMCHS_CON_DW8_SHIFT ((uint32_t)5U)
#define CSL_MMCHS_CON_DW8_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_DW8__8BITMODE ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_DW8__1_4BITMODE ((uint32_t)0x00000000U)

#define CSL_MMCHS_CON_MODE_MASK ((uint32_t)0x00000010U)
#define CSL_MMCHS_CON_MODE_SHIFT ((uint32_t)4U)
#define CSL_MMCHS_CON_MODE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_MODE_SYSTEST ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_MODE_FUNC ((uint32_t)0x00000000U)

#define CSL_MMCHS_CON_CTPL_MASK ((uint32_t)0x00000800U)
#define CSL_MMCHS_CON_CTPL_SHIFT ((uint32_t)11U)
#define CSL_MMCHS_CON_CTPL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_CTPL_MMC_SD ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_CTPL_SDIO ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_CEATA_MASK ((uint32_t)0x00001000U)
#define CSL_MMCHS_CON_CEATA_SHIFT ((uint32_t)12U)
#define CSL_MMCHS_CON_CEATA_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_CEATA_NORMALMODE ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_CEATA_CEATAMODE ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_OBIP_MASK ((uint32_t)0x00002000U)
#define CSL_MMCHS_CON_OBIP_SHIFT ((uint32_t)13U)
#define CSL_MMCHS_CON_OBIP_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_OBIP_ACTIVEHIGH ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_OBIP_ACTIVELOW ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_OBIE_MASK ((uint32_t)0x00004000U)
#define CSL_MMCHS_CON_OBIE_SHIFT ((uint32_t)14U)
#define CSL_MMCHS_CON_OBIE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_OBIE_OBINTMODE ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_OBIE_NORMALMODE ((uint32_t)0x00000000U)

#define CSL_MMCHS_CON_PADEN_MASK ((uint32_t)0x00008000U)
#define CSL_MMCHS_CON_PADEN_SHIFT ((uint32_t)15U)
#define CSL_MMCHS_CON_PADEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_PADEN_DISABLE ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_PADEN_ENABLE ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_CLKEXTFREE_MASK ((uint32_t)0x00010000U)
#define CSL_MMCHS_CON_CLKEXTFREE_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_CON_CLKEXTFREE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_CLKEXTFREE_AUTOGATING ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_CLKEXTFREE_FREERUNNING ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_BOOT_ACK_MASK ((uint32_t)0x00020000U)
#define CSL_MMCHS_CON_BOOT_ACK_SHIFT ((uint32_t)17U)
#define CSL_MMCHS_CON_BOOT_ACK_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_BOOT_ACK_BOOTNOACK ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_BOOT_ACK_BOOTACK ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_BOOT_CF0_MASK ((uint32_t)0x00040000U)
#define CSL_MMCHS_CON_BOOT_CF0_SHIFT ((uint32_t)18U)
#define CSL_MMCHS_CON_BOOT_CF0_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_BOOT_CF0_NOCMDFORCE ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_BOOT_CF0_CMDFORCED ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_BOOT_CF0_CMDFORCEREQ ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_BOOT_CF0_CMDRELEASED ((uint32_t)0x00000000U)

#define CSL_MMCHS_CON_DDR_MASK ((uint32_t)0x00080000U)
#define CSL_MMCHS_CON_DDR_SHIFT ((uint32_t)19U)
#define CSL_MMCHS_CON_DDR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_DDR_NORMALMODE ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_DDR_DDRMODE ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_DMA_MNS_MASK ((uint32_t)0x00100000U)
#define CSL_MMCHS_CON_DMA_MNS_SHIFT ((uint32_t)20U)
#define CSL_MMCHS_CON_DMA_MNS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_DMA_MNS_MASTERDMADIS ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_DMA_MNS_MASTERDMAEN ((uint32_t)0x00000001U)

#define CSL_MMCHS_CON_SDMA_LNE_MASK ((uint32_t)0x00200000U)
#define CSL_MMCHS_CON_SDMA_LNE_SHIFT ((uint32_t)21U)
#define CSL_MMCHS_CON_SDMA_LNE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CON_SDMA_LNE_LATEDEASSERT ((uint32_t)0x00000001U)
#define CSL_MMCHS_CON_SDMA_LNE_EARLYDEASSERT ((uint32_t)0x00000000U)

#define CSL_MMCHS_CON_RESETVAL ((uint32_t)0x00000600U)

    /* PWCNT */

#define CSL_MMCHS_PWCNT_PWRCNT_MASK ((uint32_t)0x0000FFFFU)
#define CSL_MMCHS_PWCNT_PWRCNT_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_PWCNT_PWRCNT_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PWCNT_PWRCNT_MAX ((uint32_t)0x0000ffffU)

#define CSL_MMCHS_PWCNT_RESETVAL ((uint32_t)0x00000000U)

    /* DLL */

#define CSL_MMCHS_DLL_DLL_LOCK_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_DLL_DLL_LOCK_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_DLL_DLL_LOCK_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_DLL_LOCK_NOTLOCKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_DLL_LOCK_LOCKED ((uint32_t)0x00000001U)

#define CSL_MMCHS_DLL_DLL_CALIB_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_DLL_DLL_CALIB_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_DLL_DLL_CALIB_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_DLL_CALIB_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_DLL_DLL_CALIB_DISABLED ((uint32_t)0x00000000U)

#define CSL_MMCHS_DLL_SLAVE_RATIO_MASK ((uint32_t)0x00000FC0U)
#define CSL_MMCHS_DLL_SLAVE_RATIO_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_DLL_SLAVE_RATIO_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_SLAVE_RATIO_MAX ((uint32_t)0x0000003fU)

#define CSL_MMCHS_DLL_MAX_LOCK_DIFF_MASK ((uint32_t)0x3FC00000U)
#define CSL_MMCHS_DLL_MAX_LOCK_DIFF_SHIFT ((uint32_t)22U)
#define CSL_MMCHS_DLL_MAX_LOCK_DIFF_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_MAX_LOCK_DIFF_MAX ((uint32_t)0x000000ffU)

#define CSL_MMCHS_DLL_LOCK_TIMER_MASK ((uint32_t)0x40000000U)
#define CSL_MMCHS_DLL_LOCK_TIMER_SHIFT ((uint32_t)30U)
#define CSL_MMCHS_DLL_LOCK_TIMER_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_LOCK_TIMER_DLL_FAST_MODE ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_LOCK_TIMER_OTHER ((uint32_t)0x00000001U)

#define CSL_MMCHS_DLL_FORCE_VALUE_MASK ((uint32_t)0x00001000U)
#define CSL_MMCHS_DLL_FORCE_VALUE_SHIFT ((uint32_t)12U)
#define CSL_MMCHS_DLL_FORCE_VALUE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_FORCE_VALUE_NO_FORCE ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_FORCE_VALUE_FORCE ((uint32_t)0x00000001U)

#define CSL_MMCHS_DLL_FORCE_SR_C_MASK ((uint32_t)0x000FE000U)
#define CSL_MMCHS_DLL_FORCE_SR_C_SHIFT ((uint32_t)13U)
#define CSL_MMCHS_DLL_FORCE_SR_C_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_FORCE_SR_C_MAX ((uint32_t)0x0000007fU)

#define CSL_MMCHS_DLL_FORCE_SR_F_MASK ((uint32_t)0x00300000U)
#define CSL_MMCHS_DLL_FORCE_SR_F_SHIFT ((uint32_t)20U)
#define CSL_MMCHS_DLL_FORCE_SR_F_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_FORCE_SR_F_MAX ((uint32_t)0x00000003U)

#ifndef CSL_MODIFICATION
#define CSL_MMCHS_DLL_FORCE_SWT_MASK ((uint32_t)0x00100000U)
#define CSL_MMCHS_DLL_FORCE_SWT_SHIFT ((uint32_t)20U)
#define CSL_MMCHS_DLL_FORCE_SWT_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_FORCE_SWT_ENABLE ((uint32_t)0x00000001U)
#define CSL_MMCHS_DLL_FORCE_SWT_DISABLE ((uint32_t)0x00000000U)
#endif

#define CSL_MMCHS_DLL_DLL_SOFT_RESET_MASK ((uint32_t)0x80000000U)
#define CSL_MMCHS_DLL_DLL_SOFT_RESET_SHIFT ((uint32_t)31U)
#define CSL_MMCHS_DLL_DLL_SOFT_RESET_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_DLL_DLL_SOFT_RESET_WRITE_1 ((uint32_t)0x00000001U)
#define CSL_MMCHS_DLL_DLL_SOFT_RESET_WRITE_0 ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_DLL_SOFT_RESET_READ_1 ((uint32_t)0x00000001U)
#define CSL_MMCHS_DLL_DLL_SOFT_RESET_READ_0 ((uint32_t)0x00000000U)

#define CSL_MMCHS_DLL_DLL_UNLOCK_STICKY_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_DLL_DLL_UNLOCK_STICKY_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_DLL_DLL_UNLOCK_STICKY_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_DLL_UNLOCK_STICKY_MAX ((uint32_t)0x00000001U)

#define CSL_MMCHS_DLL_DLL_UNLOCK_CLEAR_MASK ((uint32_t)0x00000008U)
#define CSL_MMCHS_DLL_DLL_UNLOCK_CLEAR_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_DLL_DLL_UNLOCK_CLEAR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DLL_DLL_UNLOCK_CLEAR__1 ((uint32_t)0x00000001U)
#define CSL_MMCHS_DLL_DLL_UNLOCK_CLEAR__0 ((uint32_t)0x00000000U)

#define CSL_MMCHS_DLL_RESETVAL ((uint32_t)0x80000000U)

    /* SDMASA */

#define CSL_MMCHS_SDMASA_SDMA_ARG2_MASK ((uint32_t)0xFFFFFFFFU)
#define CSL_MMCHS_SDMASA_SDMA_ARG2_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_SDMASA_SDMA_ARG2_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SDMASA_SDMA_ARG2_MAX ((uint32_t)0xffffffffU)

#define CSL_MMCHS_SDMASA_RESETVAL ((uint32_t)0x00000000U)

    /* BLK */

#define CSL_MMCHS_BLK_NBLK_MASK ((uint32_t)0xFFFF0000U)
#define CSL_MMCHS_BLK_NBLK_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_BLK_NBLK_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_BLK_NBLK_MAX ((uint32_t)0x0000ffffU)

#define CSL_MMCHS_BLK_BLEN_MASK ((uint32_t)0x00000FFFU)
#define CSL_MMCHS_BLK_BLEN_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_BLK_BLEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_BLK_BLEN_MAX ((uint32_t)0x00000fffU)

#define CSL_MMCHS_BLK_RESETVAL ((uint32_t)0x00000000U)

    /* ARG */

#define CSL_MMCHS_ARG_ARG_MASK ((uint32_t)0xFFFFFFFFU)
#define CSL_MMCHS_ARG_ARG_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_ARG_ARG_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ARG_ARG_MAX ((uint32_t)0xffffffffU)

#define CSL_MMCHS_ARG_RESETVAL ((uint32_t)0x00000000U)

    /* CMD */

#define CSL_MMCHS_CMD_RSP_TYPE_MASK ((uint32_t)0x00030000U)
#define CSL_MMCHS_CMD_RSP_TYPE_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_CMD_RSP_TYPE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_RSP_TYPE_LGHT48B ((uint32_t)0x00000003U)
#define CSL_MMCHS_CMD_RSP_TYPE_NORSP ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_RSP_TYPE_LGHT36 ((uint32_t)0x00000001U)
#define CSL_MMCHS_CMD_RSP_TYPE_LGHT48 ((uint32_t)0x00000002U)

#define CSL_MMCHS_CMD_CCCE_MASK ((uint32_t)0x00080000U)
#define CSL_MMCHS_CMD_CCCE_SHIFT ((uint32_t)19U)
#define CSL_MMCHS_CMD_CCCE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_CCCE_NOCHECK ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_CCCE_CHECK ((uint32_t)0x00000001U)

#define CSL_MMCHS_CMD_ACEN_MASK ((uint32_t)0x0000000CU)
#define CSL_MMCHS_CMD_ACEN_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_CMD_ACEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_ACEN_DISABLE ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_ACEN_ENABLECMD12 ((uint32_t)0x00000001U)
#define CSL_MMCHS_CMD_ACEN_ENABLECMD23 ((uint32_t)0x00000002U)
#define CSL_MMCHS_CMD_ACEN_RESERVED ((uint32_t)0x00000003U)

#define CSL_MMCHS_CMD_INDX_MASK ((uint32_t)0x3F000000U)
#define CSL_MMCHS_CMD_INDX_SHIFT ((uint32_t)24U)
#define CSL_MMCHS_CMD_INDX_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_INDX_MAX ((uint32_t)0x0000003fU)

#define CSL_MMCHS_CMD_DE_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_CMD_DE_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_CMD_DE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_DE_DISABLE ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_DE_ENABLE ((uint32_t)0x00000001U)

#define CSL_MMCHS_CMD_BCE_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_CMD_BCE_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_CMD_BCE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_BCE_DISABLE ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_BCE_ENABLE ((uint32_t)0x00000001U)

#define CSL_MMCHS_CMD_CICE_MASK ((uint32_t)0x00100000U)
#define CSL_MMCHS_CMD_CICE_SHIFT ((uint32_t)20U)
#define CSL_MMCHS_CMD_CICE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_CICE_CHECK ((uint32_t)0x00000001U)
#define CSL_MMCHS_CMD_CICE_NOCHECK ((uint32_t)0x00000000U)

#define CSL_MMCHS_CMD_MSBS_MASK ((uint32_t)0x00000020U)
#define CSL_MMCHS_CMD_MSBS_SHIFT ((uint32_t)5U)
#define CSL_MMCHS_CMD_MSBS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_MSBS_MULTIBLK ((uint32_t)0x00000001U)
#define CSL_MMCHS_CMD_MSBS_SGLEBLK ((uint32_t)0x00000000U)

#define CSL_MMCHS_CMD_CMD_TYPE_MASK ((uint32_t)0x00C00000U)
#define CSL_MMCHS_CMD_CMD_TYPE_SHIFT ((uint32_t)22U)
#define CSL_MMCHS_CMD_CMD_TYPE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_CMD_TYPE_RESUME ((uint32_t)0x00000002U)
#define CSL_MMCHS_CMD_CMD_TYPE_SUSPEND ((uint32_t)0x00000001U)
#define CSL_MMCHS_CMD_CMD_TYPE_NORMAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_CMD_TYPE_ABORT ((uint32_t)0x00000003U)

#define CSL_MMCHS_CMD_DP_MASK ((uint32_t)0x00200000U)
#define CSL_MMCHS_CMD_DP_SHIFT ((uint32_t)21U)
#define CSL_MMCHS_CMD_DP_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_DP_NODATA ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_DP_DATA ((uint32_t)0x00000001U)

#define CSL_MMCHS_CMD_DDIR_MASK ((uint32_t)0x00000010U)
#define CSL_MMCHS_CMD_DDIR_SHIFT ((uint32_t)4U)
#define CSL_MMCHS_CMD_DDIR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CMD_DDIR_READ ((uint32_t)0x00000001U)
#define CSL_MMCHS_CMD_DDIR_WRITE ((uint32_t)0x00000000U)

#define CSL_MMCHS_CMD_RESETVAL ((uint32_t)0x00000000U)

    /* RSP10 */

#define CSL_MMCHS_RSP10_RSP1_MASK ((uint32_t)0xFFFF0000U)
#define CSL_MMCHS_RSP10_RSP1_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_RSP10_RSP1_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_RSP10_RSP1_MAX ((uint32_t)0x0000ffffU)

#define CSL_MMCHS_RSP10_RSP0_MASK ((uint32_t)0x0000FFFFU)
#define CSL_MMCHS_RSP10_RSP0_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_RSP10_RSP0_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_RSP10_RSP0_MAX ((uint32_t)0x0000ffffU)

#define CSL_MMCHS_RSP10_RESETVAL ((uint32_t)0x00000000U)

    /* RSP32 */

#define CSL_MMCHS_RSP32_RSP3_MASK ((uint32_t)0xFFFF0000U)
#define CSL_MMCHS_RSP32_RSP3_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_RSP32_RSP3_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_RSP32_RSP3_MAX ((uint32_t)0x0000ffffU)

#define CSL_MMCHS_RSP32_RSP2_MASK ((uint32_t)0x0000FFFFU)
#define CSL_MMCHS_RSP32_RSP2_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_RSP32_RSP2_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_RSP32_RSP2_MAX ((uint32_t)0x0000ffffU)

#define CSL_MMCHS_RSP32_RESETVAL ((uint32_t)0x00000000U)

    /* RSP54 */

#define CSL_MMCHS_RSP54_RSP5_MASK ((uint32_t)0xFFFF0000U)
#define CSL_MMCHS_RSP54_RSP5_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_RSP54_RSP5_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_RSP54_RSP5_MAX ((uint32_t)0x0000ffffU)

#define CSL_MMCHS_RSP54_RSP4_MASK ((uint32_t)0x0000FFFFU)
#define CSL_MMCHS_RSP54_RSP4_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_RSP54_RSP4_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_RSP54_RSP4_MAX ((uint32_t)0x0000ffffU)

#define CSL_MMCHS_RSP54_RESETVAL ((uint32_t)0x00000000U)

    /* RSP76 */

#define CSL_MMCHS_RSP76_RSP6_MASK ((uint32_t)0x0000FFFFU)
#define CSL_MMCHS_RSP76_RSP6_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_RSP76_RSP6_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_RSP76_RSP6_MAX ((uint32_t)0x0000ffffU)

#define CSL_MMCHS_RSP76_RSP7_MASK ((uint32_t)0xFFFF0000U)
#define CSL_MMCHS_RSP76_RSP7_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_RSP76_RSP7_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_RSP76_RSP7_MAX ((uint32_t)0x0000ffffU)

#define CSL_MMCHS_RSP76_RESETVAL ((uint32_t)0x00000000U)

    /* DATA */

#define CSL_MMCHS_DATA_DATA_MASK ((uint32_t)0xFFFFFFFFU)
#define CSL_MMCHS_DATA_DATA_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_DATA_DATA_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_DATA_DATA_MAX ((uint32_t)0xffffffffU)

#define CSL_MMCHS_DATA_RESETVAL ((uint32_t)0x00000000U)

    /* PSTATE */

#define CSL_MMCHS_PSTATE_RTA_MASK ((uint32_t)0x00000200U)
#define CSL_MMCHS_PSTATE_RTA_SHIFT ((uint32_t)9U)
#define CSL_MMCHS_PSTATE_RTA_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_RTA_TRANSFER ((uint32_t)0x00000001U)
#define CSL_MMCHS_PSTATE_RTA_NOTRANSFER ((uint32_t)0x00000000U)

#define CSL_MMCHS_PSTATE_WTA_MASK ((uint32_t)0x00000100U)
#define CSL_MMCHS_PSTATE_WTA_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_PSTATE_WTA_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_WTA_NOTRANSFER ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_WTA_TRANSFER ((uint32_t)0x00000001U)

#define CSL_MMCHS_PSTATE_BRE_MASK ((uint32_t)0x00000800U)
#define CSL_MMCHS_PSTATE_BRE_SHIFT ((uint32_t)11U)
#define CSL_MMCHS_PSTATE_BRE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_BRE_RDDISABLE ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_BRE_RDENABLE ((uint32_t)0x00000001U)

#define CSL_MMCHS_PSTATE_CSS_MASK ((uint32_t)0x00020000U)
#define CSL_MMCHS_PSTATE_CSS_SHIFT ((uint32_t)17U)
#define CSL_MMCHS_PSTATE_CSS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_CSS_STABLE ((uint32_t)0x00000001U)
#define CSL_MMCHS_PSTATE_CSS_DEBOUNCING ((uint32_t)0x00000000U)

#define CSL_MMCHS_PSTATE_DLEV_MASK ((uint32_t)0x00F00000U)
#define CSL_MMCHS_PSTATE_DLEV_SHIFT ((uint32_t)20U)
#define CSL_MMCHS_PSTATE_DLEV_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_DLEV_MAX ((uint32_t)0x0000000fU)

#define CSL_MMCHS_PSTATE_CDPL_MASK ((uint32_t)0x00040000U)
#define CSL_MMCHS_PSTATE_CDPL_SHIFT ((uint32_t)18U)
#define CSL_MMCHS_PSTATE_CDPL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_CDPL_ZERO ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_CDPL_ONE ((uint32_t)0x00000001U)

#define CSL_MMCHS_PSTATE_CLEV_MASK ((uint32_t)0x01000000U)
#define CSL_MMCHS_PSTATE_CLEV_SHIFT ((uint32_t)24U)
#define CSL_MMCHS_PSTATE_CLEV_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_CLEV_ZERO ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_CLEV_ONE ((uint32_t)0x00000001U)

#define CSL_MMCHS_PSTATE_CINS_MASK ((uint32_t)0x00010000U)
#define CSL_MMCHS_PSTATE_CINS_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_PSTATE_CINS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_CINS_ZERO ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_CINS_ONE ((uint32_t)0x00000001U)

#define CSL_MMCHS_PSTATE_DATI_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_PSTATE_DATI_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_PSTATE_DATI_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_DATI_CMDDIS ((uint32_t)0x00000001U)
#define CSL_MMCHS_PSTATE_DATI_CMDEN ((uint32_t)0x00000000U)

#define CSL_MMCHS_PSTATE_WP_MASK ((uint32_t)0x00080000U)
#define CSL_MMCHS_PSTATE_WP_SHIFT ((uint32_t)19U)
#define CSL_MMCHS_PSTATE_WP_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_WP_ONE ((uint32_t)0x00000001U)
#define CSL_MMCHS_PSTATE_WP_ZERO ((uint32_t)0x00000000U)

#define CSL_MMCHS_PSTATE_CMDI_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_PSTATE_CMDI_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_PSTATE_CMDI_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_CMDI_CMDDIS ((uint32_t)0x00000001U)
#define CSL_MMCHS_PSTATE_CMDI_CMDEN ((uint32_t)0x00000000U)

#define CSL_MMCHS_PSTATE_DLA_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_PSTATE_DLA_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_PSTATE_DLA_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_DLA_ZERO ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_DLA_ONE ((uint32_t)0x00000001U)

#define CSL_MMCHS_PSTATE_BWE_MASK ((uint32_t)0x00000400U)
#define CSL_MMCHS_PSTATE_BWE_SHIFT ((uint32_t)10U)
#define CSL_MMCHS_PSTATE_BWE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_BWE_WRENABLE ((uint32_t)0x00000001U)
#define CSL_MMCHS_PSTATE_BWE_WRDISABLE ((uint32_t)0x00000000U)

#define CSL_MMCHS_PSTATE_RTR_MASK ((uint32_t)0x00000008U)
#define CSL_MMCHS_PSTATE_RTR_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_PSTATE_RTR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_RTR_NOTUNING ((uint32_t)0x00000000U)
#define CSL_MMCHS_PSTATE_RTR_TUNING ((uint32_t)0x00000001U)

#define CSL_MMCHS_PSTATE_RESETVAL ((uint32_t)0x00000000U)

    /* HCTL */

#define CSL_MMCHS_HCTL_SDVS_MASK ((uint32_t)0x00000E00U)
#define CSL_MMCHS_HCTL_SDVS_SHIFT ((uint32_t)9U)
#define CSL_MMCHS_HCTL_SDVS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_SDVS__1V8 ((uint32_t)0x00000005U)
#define CSL_MMCHS_HCTL_SDVS__3V0 ((uint32_t)0x00000006U)
#define CSL_MMCHS_HCTL_SDVS__3V3 ((uint32_t)0x00000007U)

#define CSL_MMCHS_HCTL_LED_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_HCTL_LED_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_HCTL_LED_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_LED_MAX ((uint32_t)0x00000001U)

#define CSL_MMCHS_HCTL_DTW_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_HCTL_DTW_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_HCTL_DTW_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_DTW__1_BITMODE ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_DTW__4_BITMODE ((uint32_t)0x00000001U)

#define CSL_MMCHS_HCTL_SBGR_MASK ((uint32_t)0x00010000U)
#define CSL_MMCHS_HCTL_SBGR_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_HCTL_SBGR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_SBGR_TRANSFER ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_SBGR_STPBLK ((uint32_t)0x00000001U)

#define CSL_MMCHS_HCTL_REM_MASK ((uint32_t)0x04000000U)
#define CSL_MMCHS_HCTL_REM_SHIFT ((uint32_t)26U)
#define CSL_MMCHS_HCTL_REM_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_REM_ENABLE ((uint32_t)0x00000001U)
#define CSL_MMCHS_HCTL_REM_DISABLE ((uint32_t)0x00000000U)

#define CSL_MMCHS_HCTL_IBG_MASK ((uint32_t)0x00080000U)
#define CSL_MMCHS_HCTL_IBG_SHIFT ((uint32_t)19U)
#define CSL_MMCHS_HCTL_IBG_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_IBG_ITDIABLE ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_IBG_ITENABLE ((uint32_t)0x00000001U)

#define CSL_MMCHS_HCTL_SDBP_MASK ((uint32_t)0x00000100U)
#define CSL_MMCHS_HCTL_SDBP_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_HCTL_SDBP_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_SDBP_PWROFF ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_SDBP_PWRON ((uint32_t)0x00000001U)

#define CSL_MMCHS_HCTL_INS_MASK ((uint32_t)0x02000000U)
#define CSL_MMCHS_HCTL_INS_SHIFT ((uint32_t)25U)
#define CSL_MMCHS_HCTL_INS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_INS_ENABLE ((uint32_t)0x00000001U)
#define CSL_MMCHS_HCTL_INS_DISABLE ((uint32_t)0x00000000U)

#define CSL_MMCHS_HCTL_IWE_MASK ((uint32_t)0x01000000U)
#define CSL_MMCHS_HCTL_IWE_SHIFT ((uint32_t)24U)
#define CSL_MMCHS_HCTL_IWE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_IWE_ENABLE ((uint32_t)0x00000001U)
#define CSL_MMCHS_HCTL_IWE_DISABLE ((uint32_t)0x00000000U)

#define CSL_MMCHS_HCTL_HSPE_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_HCTL_HSPE_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_HCTL_HSPE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_HSPE_NORMALSPEED ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_HSPE_HIGHSPEED ((uint32_t)0x00000001U)

#define CSL_MMCHS_HCTL_CR_MASK ((uint32_t)0x00020000U)
#define CSL_MMCHS_HCTL_CR_SHIFT ((uint32_t)17U)
#define CSL_MMCHS_HCTL_CR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_CR_NONE ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_CR_RESTART ((uint32_t)0x00000001U)

#define CSL_MMCHS_HCTL_RWC_MASK ((uint32_t)0x00040000U)
#define CSL_MMCHS_HCTL_RWC_SHIFT ((uint32_t)18U)
#define CSL_MMCHS_HCTL_RWC_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_RWC_RW ((uint32_t)0x00000001U)
#define CSL_MMCHS_HCTL_RWC_NORW ((uint32_t)0x00000000U)

#define CSL_MMCHS_HCTL_OBWE_MASK ((uint32_t)0x08000000U)
#define CSL_MMCHS_HCTL_OBWE_SHIFT ((uint32_t)27U)
#define CSL_MMCHS_HCTL_OBWE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_OBWE_DISABLE ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_OBWE_ENABLE ((uint32_t)0x00000001U)

#define CSL_MMCHS_HCTL_DMAS_MASK ((uint32_t)0x00000018U)
#define CSL_MMCHS_HCTL_DMAS_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_HCTL_DMAS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_DMAS_RESERVED ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_DMAS_RESERVED1 ((uint32_t)0x00000001U)
#define CSL_MMCHS_HCTL_DMAS_ADMA2 ((uint32_t)0x00000002U)
#define CSL_MMCHS_HCTL_DMAS_RESERVED2 ((uint32_t)0x00000003U)

#define CSL_MMCHS_HCTL_CDTL_MASK ((uint32_t)0x00000040U)
#define CSL_MMCHS_HCTL_CDTL_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_HCTL_CDTL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_CDTL_NOCARD ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_CDTL_CARDINS ((uint32_t)0x00000001U)

#define CSL_MMCHS_HCTL_CDSS_MASK ((uint32_t)0x00000080U)
#define CSL_MMCHS_HCTL_CDSS_SHIFT ((uint32_t)7U)
#define CSL_MMCHS_HCTL_CDSS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_CDSS_SDCDSEL ((uint32_t)0x00000000U)
#define CSL_MMCHS_HCTL_CDSS_CDTLSEL ((uint32_t)0x00000001U)

#define CSL_MMCHS_HCTL_RESETVAL ((uint32_t)0x00000000U)

    /* SYSCTL */

#define CSL_MMCHS_SYSCTL_ICS_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_SYSCTL_ICS_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_SYSCTL_ICS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_ICS_NOTREADY ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_ICS_READY ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSCTL_SRA_MASK ((uint32_t)0x01000000U)
#define CSL_MMCHS_SYSCTL_SRA_SHIFT ((uint32_t)24U)
#define CSL_MMCHS_SYSCTL_SRA_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_SRA_WORK ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_SRA_RESET ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSCTL_ICE_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSCTL_ICE_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_SYSCTL_ICE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_ICE_OSCILLATE ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSCTL_ICE_STOP ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSCTL_CEN_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_SYSCTL_CEN_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_SYSCTL_CEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_CEN_ENABLE ((uint32_t)0x00000001U)
#define CSL_MMCHS_SYSCTL_CEN_DISABLE ((uint32_t)0x00000000U)

#define CSL_MMCHS_SYSCTL_CLKD_MASK ((uint32_t)0x0000FFC0U)
#define CSL_MMCHS_SYSCTL_CLKD_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_SYSCTL_CLKD_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_CLKD_MAX ((uint32_t)0x000003ffU)

#define CSL_MMCHS_SYSCTL_SRD_MASK ((uint32_t)0x04000000U)
#define CSL_MMCHS_SYSCTL_SRD_SHIFT ((uint32_t)26U)
#define CSL_MMCHS_SYSCTL_SRD_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_SRD_WORK ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_SRD_RESET ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSCTL_DTO_MASK ((uint32_t)0x000F0000U)
#define CSL_MMCHS_SYSCTL_DTO_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_SYSCTL_DTO_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_DTO_MAX ((uint32_t)0x0000000fU)

#define CSL_MMCHS_SYSCTL_SRC_MASK ((uint32_t)0x02000000U)
#define CSL_MMCHS_SYSCTL_SRC_SHIFT ((uint32_t)25U)
#define CSL_MMCHS_SYSCTL_SRC_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_SRC_WORK ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_SRC_RESET ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSCTL_CGS_MASK ((uint32_t)0x00000020U)
#define CSL_MMCHS_SYSCTL_CGS_SHIFT ((uint32_t)5U)
#define CSL_MMCHS_SYSCTL_CGS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_SYSCTL_CGS_MAX ((uint32_t)0x00000001U)

#define CSL_MMCHS_SYSCTL_RESETVAL ((uint32_t)0x00000000U)

    /* STAT */

#define CSL_MMCHS_STAT_ERRI_MASK ((uint32_t)0x00008000U)
#define CSL_MMCHS_STAT_ERRI_SHIFT ((uint32_t)15U)
#define CSL_MMCHS_STAT_ERRI_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_ERRI_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_ERRI_IRQ_TRU_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_BGE_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_STAT_BGE_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_STAT_BGE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BGE_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BGE_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BGE_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_BGE_ST_RST_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_CERR_MASK ((uint32_t)0x10000000U)
#define CSL_MMCHS_STAT_CERR_SHIFT ((uint32_t)28U)
#define CSL_MMCHS_STAT_CERR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CERR_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CERR_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CERR_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CERR_ST_UN_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_STAT_CIRQ_MASK ((uint32_t)0x00000100U)
#define CSL_MMCHS_STAT_CIRQ_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_STAT_CIRQ_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CIRQ_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CIRQ_IRQ_TRU_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_CREM_MASK ((uint32_t)0x00000080U)
#define CSL_MMCHS_STAT_CREM_SHIFT ((uint32_t)7U)
#define CSL_MMCHS_STAT_CREM_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CREM_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CREM_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CREM_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CREM_IRQ_TRU_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_DEB_MASK ((uint32_t)0x00400000U)
#define CSL_MMCHS_STAT_DEB_SHIFT ((uint32_t)22U)
#define CSL_MMCHS_STAT_DEB_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DEB_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DEB_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_DEB_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DEB_ST_RST_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_CC_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CC_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_STAT_CC_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CC_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CC_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CC_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CC_ST_UN_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_STAT_BWR_MASK ((uint32_t)0x00000010U)
#define CSL_MMCHS_STAT_BWR_SHIFT ((uint32_t)4U)
#define CSL_MMCHS_STAT_BWR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BWR_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_BWR_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_BWR_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BWR_ST_UN_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_STAT_ACE_MASK ((uint32_t)0x01000000U)
#define CSL_MMCHS_STAT_ACE_SHIFT ((uint32_t)24U)
#define CSL_MMCHS_STAT_ACE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_ACE_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_ACE_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_ACE_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_ACE_IRQ_TRU_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_DMA_MASK ((uint32_t)0x00000008U)
#define CSL_MMCHS_STAT_DMA_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_STAT_DMA_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DMA_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DMA_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DMA_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_DMA_ST_RST_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_CTO_MASK ((uint32_t)0x00010000U)
#define CSL_MMCHS_STAT_CTO_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_STAT_CTO_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CTO_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CTO_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CTO_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CTO_ST_UN_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_STAT_DCRC_MASK ((uint32_t)0x00200000U)
#define CSL_MMCHS_STAT_DCRC_SHIFT ((uint32_t)21U)
#define CSL_MMCHS_STAT_DCRC_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DCRC_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_DCRC_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DCRC_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DCRC_IRQ_TRU_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_CCRC_MASK ((uint32_t)0x00020000U)
#define CSL_MMCHS_STAT_CCRC_SHIFT ((uint32_t)17U)
#define CSL_MMCHS_STAT_CCRC_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CCRC_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CCRC_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CCRC_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CCRC_ST_RST_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_CLE_MASK ((uint32_t)0x00800000U)
#define CSL_MMCHS_STAT_CLE_SHIFT ((uint32_t)23U)
#define CSL_MMCHS_STAT_CLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CLE_MAX ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_TC_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_STAT_TC_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_STAT_TC_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_TC_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_TC_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_TC_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_TC_ST_UN_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_STAT_BRR_MASK ((uint32_t)0x00000020U)
#define CSL_MMCHS_STAT_BRR_SHIFT ((uint32_t)5U)
#define CSL_MMCHS_STAT_BRR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BRR_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BRR_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_BRR_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BRR_IRQ_TRU_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_CIE_MASK ((uint32_t)0x00080000U)
#define CSL_MMCHS_STAT_CIE_SHIFT ((uint32_t)19U)
#define CSL_MMCHS_STAT_CIE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CIE_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CIE_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CIE_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CIE_IRQ_FAL_R ((uint32_t)0x00000000U)

#define CSL_MMCHS_STAT_DTO_MASK ((uint32_t)0x00100000U)
#define CSL_MMCHS_STAT_DTO_SHIFT ((uint32_t)20U)
#define CSL_MMCHS_STAT_DTO_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DTO_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DTO_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_DTO_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_DTO_ST_RST_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_CINS_MASK ((uint32_t)0x00000040U)
#define CSL_MMCHS_STAT_CINS_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_STAT_CINS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CINS_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CINS_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CINS_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CINS_ST_RST_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_BADA_MASK ((uint32_t)0x20000000U)
#define CSL_MMCHS_STAT_BADA_SHIFT ((uint32_t)29U)
#define CSL_MMCHS_STAT_BADA_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BADA_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BADA_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_BADA_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BADA_IRQ_TRU_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_CEB_MASK ((uint32_t)0x00040000U)
#define CSL_MMCHS_STAT_CEB_SHIFT ((uint32_t)18U)
#define CSL_MMCHS_STAT_CEB_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CEB_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_CEB_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CEB_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_CEB_ST_UN_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_STAT_OBI_MASK ((uint32_t)0x00000200U)
#define CSL_MMCHS_STAT_OBI_SHIFT ((uint32_t)9U)
#define CSL_MMCHS_STAT_OBI_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_OBI_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_OBI_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_OBI_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_OBI_IRQ_TRU_R ((uint32_t)0x00000001U)

#define CSL_MMCHS_STAT_BSR_MASK ((uint32_t)0x00000400U)
#define CSL_MMCHS_STAT_BSR_SHIFT ((uint32_t)10U)
#define CSL_MMCHS_STAT_BSR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BSR_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_BSR_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_BSR_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_BSR_ST_UN_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_STAT_ADMAE_MASK ((uint32_t)0x02000000U)
#define CSL_MMCHS_STAT_ADMAE_SHIFT ((uint32_t)25U)
#define CSL_MMCHS_STAT_ADMAE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_ADMAE_IRQ_TRU_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_ADMAE_ST_RST_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_ADMAE_IRQ_FAL_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_ADMAE_ST_UN_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_STAT_TE_MASK ((uint32_t)0x04000000U)
#define CSL_MMCHS_STAT_TE_SHIFT ((uint32_t)26U)
#define CSL_MMCHS_STAT_TE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_STAT_TE_ERROR ((uint32_t)0x00000001U)
#define CSL_MMCHS_STAT_TE_NOERROR ((uint32_t)0x00000000U)

#define CSL_MMCHS_STAT_RESETVAL ((uint32_t)0x00000000U)

    /* IE */

#define CSL_MMCHS_IE_DCRC_ENABLE_MASK ((uint32_t)0x00200000U)
#define CSL_MMCHS_IE_DCRC_ENABLE_SHIFT ((uint32_t)21U)
#define CSL_MMCHS_IE_DCRC_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_DCRC_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_DCRC_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_BWR_ENABLE_MASK ((uint32_t)0x00000010U)
#define CSL_MMCHS_IE_BWR_ENABLE_SHIFT ((uint32_t)4U)
#define CSL_MMCHS_IE_BWR_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_BWR_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_BWR_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_DMA_ENABLE_MASK ((uint32_t)0x00000008U)
#define CSL_MMCHS_IE_DMA_ENABLE_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_IE_DMA_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_DMA_ENABLE_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_DMA_ENABLE_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_CLE_MASK ((uint32_t)0x00800000U)
#define CSL_MMCHS_IE_CLE_SHIFT ((uint32_t)23U)
#define CSL_MMCHS_IE_CLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CLE_MAX ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_TC_ENABLE_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_IE_TC_ENABLE_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_IE_TC_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_TC_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_TC_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_CEB_ENABLE_MASK ((uint32_t)0x00040000U)
#define CSL_MMCHS_IE_CEB_ENABLE_SHIFT ((uint32_t)18U)
#define CSL_MMCHS_IE_CEB_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CEB_ENABLE_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CEB_ENABLE_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_BRR_ENABLE_MASK ((uint32_t)0x00000020U)
#define CSL_MMCHS_IE_BRR_ENABLE_SHIFT ((uint32_t)5U)
#define CSL_MMCHS_IE_BRR_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_BRR_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_BRR_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_CERR_ENABLE_MASK ((uint32_t)0x10000000U)
#define CSL_MMCHS_IE_CERR_ENABLE_SHIFT ((uint32_t)28U)
#define CSL_MMCHS_IE_CERR_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CERR_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_CERR_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_CIRQ_ENABLE_MASK ((uint32_t)0x00000100U)
#define CSL_MMCHS_IE_CIRQ_ENABLE_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_IE_CIRQ_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CIRQ_ENABLE_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CIRQ_ENABLE_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_CC_ENABLE_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_CC_ENABLE_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_IE_CC_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CC_ENABLE_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CC_ENABLE_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_CIE_ENABLE_MASK ((uint32_t)0x00080000U)
#define CSL_MMCHS_IE_CIE_ENABLE_SHIFT ((uint32_t)19U)
#define CSL_MMCHS_IE_CIE_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CIE_ENABLE_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CIE_ENABLE_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_BADA_ENABLE_MASK ((uint32_t)0x20000000U)
#define CSL_MMCHS_IE_BADA_ENABLE_SHIFT ((uint32_t)29U)
#define CSL_MMCHS_IE_BADA_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_BADA_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_BADA_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_DTO_ENABLE_MASK ((uint32_t)0x00100000U)
#define CSL_MMCHS_IE_DTO_ENABLE_SHIFT ((uint32_t)20U)
#define CSL_MMCHS_IE_DTO_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_DTO_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_DTO_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_CINS_ENABLE_MASK ((uint32_t)0x00000040U)
#define CSL_MMCHS_IE_CINS_ENABLE_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_IE_CINS_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CINS_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_CINS_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_DEB_ENABLE_MASK ((uint32_t)0x00400000U)
#define CSL_MMCHS_IE_DEB_ENABLE_SHIFT ((uint32_t)22U)
#define CSL_MMCHS_IE_DEB_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_DEB_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_DEB_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_ACE_ENABLE_MASK ((uint32_t)0x01000000U)
#define CSL_MMCHS_IE_ACE_ENABLE_SHIFT ((uint32_t)24U)
#define CSL_MMCHS_IE_ACE_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_ACE_ENABLE_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_ACE_ENABLE_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_BGE_ENABLE_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_IE_BGE_ENABLE_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_IE_BGE_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_BGE_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_BGE_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_CCRC_ENABLE_MASK ((uint32_t)0x00020000U)
#define CSL_MMCHS_IE_CCRC_ENABLE_SHIFT ((uint32_t)17U)
#define CSL_MMCHS_IE_CCRC_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CCRC_ENABLE_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CCRC_ENABLE_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_NULL_MASK ((uint32_t)0x00008000U)
#define CSL_MMCHS_IE_NULL_SHIFT ((uint32_t)15U)
#define CSL_MMCHS_IE_NULL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_NULL_MAX ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_CREM_ENABLE_MASK ((uint32_t)0x00000080U)
#define CSL_MMCHS_IE_CREM_ENABLE_SHIFT ((uint32_t)7U)
#define CSL_MMCHS_IE_CREM_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CREM_ENABLE_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CREM_ENABLE_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_CTO_ENABLE_MASK ((uint32_t)0x00010000U)
#define CSL_MMCHS_IE_CTO_ENABLE_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_IE_CTO_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_CTO_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_CTO_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_OBI_ENABLE_MASK ((uint32_t)0x00000200U)
#define CSL_MMCHS_IE_OBI_ENABLE_SHIFT ((uint32_t)9U)
#define CSL_MMCHS_IE_OBI_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_OBI_ENABLE_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_OBI_ENABLE_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_ADMAE_ENABLE_MASK ((uint32_t)0x02000000U)
#define CSL_MMCHS_IE_ADMAE_ENABLE_SHIFT ((uint32_t)25U)
#define CSL_MMCHS_IE_ADMAE_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_ADMAE_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_ADMAE_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_BSR_ENABLE_MASK ((uint32_t)0x00000400U)
#define CSL_MMCHS_IE_BSR_ENABLE_SHIFT ((uint32_t)10U)
#define CSL_MMCHS_IE_BSR_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_BSR_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_IE_BSR_ENABLE_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_IE_TE_ENABLE_MASK ((uint32_t)0x04000000U)
#define CSL_MMCHS_IE_TE_ENABLE_SHIFT ((uint32_t)26U)
#define CSL_MMCHS_IE_TE_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_TE_ENABLE_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_IE_TE_ENABLE_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_IE_RESETVAL ((uint32_t)0x00000000U)

    /* ISE */

#define CSL_MMCHS_ISE_BWR_SIGEN_MASK ((uint32_t)0x00000010U)
#define CSL_MMCHS_ISE_BWR_SIGEN_SHIFT ((uint32_t)4U)
#define CSL_MMCHS_ISE_BWR_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_BWR_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_BWR_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_DEB_SIGEN_MASK ((uint32_t)0x00400000U)
#define CSL_MMCHS_ISE_DEB_SIGEN_SHIFT ((uint32_t)22U)
#define CSL_MMCHS_ISE_DEB_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_DEB_SIGEN_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_ISE_DEB_SIGEN_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_ISE_CREM_SIGEN_MASK ((uint32_t)0x00000080U)
#define CSL_MMCHS_ISE_CREM_SIGEN_SHIFT ((uint32_t)7U)
#define CSL_MMCHS_ISE_CREM_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CREM_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CREM_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_NULL_MASK ((uint32_t)0x00008000U)
#define CSL_MMCHS_ISE_NULL_SHIFT ((uint32_t)15U)
#define CSL_MMCHS_ISE_NULL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_NULL_MAX ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_CIRQ_SIGEN_MASK ((uint32_t)0x00000100U)
#define CSL_MMCHS_ISE_CIRQ_SIGEN_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_ISE_CIRQ_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CIRQ_SIGEN_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_ISE_CIRQ_SIGEN_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_ISE_CCRC_SIGEN_MASK ((uint32_t)0x00020000U)
#define CSL_MMCHS_ISE_CCRC_SIGEN_SHIFT ((uint32_t)17U)
#define CSL_MMCHS_ISE_CCRC_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CCRC_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CCRC_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_ACE_SIGEN_MASK ((uint32_t)0x01000000U)
#define CSL_MMCHS_ISE_ACE_SIGEN_SHIFT ((uint32_t)24U)
#define CSL_MMCHS_ISE_ACE_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_ACE_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_ACE_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_BGE_SIGEN_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_ISE_BGE_SIGEN_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_ISE_BGE_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_BGE_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_BGE_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_CERR_SIGEN_MASK ((uint32_t)0x10000000U)
#define CSL_MMCHS_ISE_CERR_SIGEN_SHIFT ((uint32_t)28U)
#define CSL_MMCHS_ISE_CERR_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CERR_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CERR_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_CINS_SIGEN_MASK ((uint32_t)0x00000040U)
#define CSL_MMCHS_ISE_CINS_SIGEN_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_ISE_CINS_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CINS_SIGEN_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_ISE_CINS_SIGEN_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_ISE_CLE_MASK ((uint32_t)0x00800000U)
#define CSL_MMCHS_ISE_CLE_SHIFT ((uint32_t)23U)
#define CSL_MMCHS_ISE_CLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CLE_MAX ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_CC_SIGEN_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_ISE_CC_SIGEN_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_ISE_CC_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CC_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CC_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_CTO_SIGEN_MASK ((uint32_t)0x00010000U)
#define CSL_MMCHS_ISE_CTO_SIGEN_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_ISE_CTO_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CTO_SIGEN_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_ISE_CTO_SIGEN_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_ISE_TC_SIGEN_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_ISE_TC_SIGEN_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_ISE_TC_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_TC_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_TC_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_BADA_SIGEN_MASK ((uint32_t)0x20000000U)
#define CSL_MMCHS_ISE_BADA_SIGEN_SHIFT ((uint32_t)29U)
#define CSL_MMCHS_ISE_BADA_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_BADA_SIGEN_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_ISE_BADA_SIGEN_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_ISE_DCRC_SIGEN_MASK ((uint32_t)0x00200000U)
#define CSL_MMCHS_ISE_DCRC_SIGEN_SHIFT ((uint32_t)21U)
#define CSL_MMCHS_ISE_DCRC_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_DCRC_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_DCRC_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_DTO_SIGEN_MASK ((uint32_t)0x00100000U)
#define CSL_MMCHS_ISE_DTO_SIGEN_SHIFT ((uint32_t)20U)
#define CSL_MMCHS_ISE_DTO_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_DTO_SIGEN_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_ISE_DTO_SIGEN_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_ISE_CIE_SIGEN_MASK ((uint32_t)0x00080000U)
#define CSL_MMCHS_ISE_CIE_SIGEN_SHIFT ((uint32_t)19U)
#define CSL_MMCHS_ISE_CIE_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CIE_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CIE_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_CEB_SIGEN_MASK ((uint32_t)0x00040000U)
#define CSL_MMCHS_ISE_CEB_SIGEN_SHIFT ((uint32_t)18U)
#define CSL_MMCHS_ISE_CEB_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CEB_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_CEB_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_DMA_SIGEN_MASK ((uint32_t)0x00000008U)
#define CSL_MMCHS_ISE_DMA_SIGEN_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_ISE_DMA_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_DMA_SIGEN_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_ISE_DMA_SIGEN_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_ISE_BRR_SIGEN_MASK ((uint32_t)0x00000020U)
#define CSL_MMCHS_ISE_BRR_SIGEN_SHIFT ((uint32_t)5U)
#define CSL_MMCHS_ISE_BRR_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_BRR_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_BRR_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_OBI_SIGEN_MASK ((uint32_t)0x00000200U)
#define CSL_MMCHS_ISE_OBI_SIGEN_SHIFT ((uint32_t)9U)
#define CSL_MMCHS_ISE_OBI_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_OBI_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_OBI_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_ADMAE_SIGEN_MASK ((uint32_t)0x02000000U)
#define CSL_MMCHS_ISE_ADMAE_SIGEN_SHIFT ((uint32_t)25U)
#define CSL_MMCHS_ISE_ADMAE_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_ADMAE_SIGEN_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_ISE_ADMAE_SIGEN_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_ISE_BSR_SIGEN_MASK ((uint32_t)0x00000400U)
#define CSL_MMCHS_ISE_BSR_SIGEN_SHIFT ((uint32_t)10U)
#define CSL_MMCHS_ISE_BSR_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_BSR_SIGEN_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_ISE_BSR_SIGEN_MASKED ((uint32_t)0x00000000U)

#define CSL_MMCHS_ISE_TE_SIGEN_MASK ((uint32_t)0x04000000U)
#define CSL_MMCHS_ISE_TE_SIGEN_SHIFT ((uint32_t)26U)
#define CSL_MMCHS_ISE_TE_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_TE_SIGEN_MASKED ((uint32_t)0x00000000U)
#define CSL_MMCHS_ISE_TE_SIGEN_ENABLED ((uint32_t)0x00000001U)

#define CSL_MMCHS_ISE_RESETVAL ((uint32_t)0x00000000U)

    /* AC12 */

#define CSL_MMCHS_AC12_CNI_MASK ((uint32_t)0x00000080U)
#define CSL_MMCHS_AC12_CNI_SHIFT ((uint32_t)7U)
#define CSL_MMCHS_AC12_CNI_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_CNI_CMDNI ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_CNI_NOERR ((uint32_t)0x00000000U)

#define CSL_MMCHS_AC12_ACTO_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_AC12_ACTO_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_AC12_ACTO_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_ACTO_TIMEOUT ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_ACTO_NOERR ((uint32_t)0x00000000U)

#define CSL_MMCHS_AC12_ACEB_MASK ((uint32_t)0x00000008U)
#define CSL_MMCHS_AC12_ACEB_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_AC12_ACEB_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_ACEB_ERR ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_ACEB_NOERR ((uint32_t)0x00000000U)

#define CSL_MMCHS_AC12_ACIE_MASK ((uint32_t)0x00000010U)
#define CSL_MMCHS_AC12_ACIE_SHIFT ((uint32_t)4U)
#define CSL_MMCHS_AC12_ACIE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_ACIE_NOERR ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_ACIE_ERR ((uint32_t)0x00000001U)

#define CSL_MMCHS_AC12_ACCE_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_AC12_ACCE_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_AC12_ACCE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_ACCE_NOERR ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_ACCE_ERR ((uint32_t)0x00000001U)

#define CSL_MMCHS_AC12_ACNE_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_ACNE_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_AC12_ACNE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_ACNE_EXE ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_ACNE_NOTEXE ((uint32_t)0x00000001U)

#define CSL_MMCHS_AC12_UHSMS_MASK ((uint32_t)0x00070000U)
#define CSL_MMCHS_AC12_UHSMS_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_AC12_UHSMS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_UHSMS_SDR12 ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_UHSMS_SDR25 ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_UHSMS_SDR50 ((uint32_t)0x00000002U)
#define CSL_MMCHS_AC12_UHSMS_SDR104 ((uint32_t)0x00000003U)
#define CSL_MMCHS_AC12_UHSMS_DDR50 ((uint32_t)0x00000004U)
#define CSL_MMCHS_AC12_UHSMS_RESERVED1 ((uint32_t)0x00000005U)
#define CSL_MMCHS_AC12_UHSMS_RESERVED2 ((uint32_t)0x00000006U)
#define CSL_MMCHS_AC12_UHSMS_RESERVED3 ((uint32_t)0x00000007U)

#define CSL_MMCHS_AC12_V1V8_SIGEN_MASK ((uint32_t)0x00080000U)
#define CSL_MMCHS_AC12_V1V8_SIGEN_SHIFT ((uint32_t)19U)
#define CSL_MMCHS_AC12_V1V8_SIGEN_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_V1V8_SIGEN__1V8 ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_V1V8_SIGEN__3V3 ((uint32_t)0x00000000U)

#define CSL_MMCHS_AC12_DS_SEL_MASK ((uint32_t)0x00300000U)
#define CSL_MMCHS_AC12_DS_SEL_SHIFT ((uint32_t)20U)
#define CSL_MMCHS_AC12_DS_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_DS_SEL_DTB ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_DS_SEL_DTA ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_DS_SEL_DTC ((uint32_t)0x00000002U)
#define CSL_MMCHS_AC12_DS_SEL_DTD ((uint32_t)0x00000003U)

#define CSL_MMCHS_AC12_ET_MASK ((uint32_t)0x00400000U)
#define CSL_MMCHS_AC12_ET_SHIFT ((uint32_t)22U)
#define CSL_MMCHS_AC12_ET_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_ET_EXECUTE ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_ET_COMPLETED ((uint32_t)0x00000000U)

#define CSL_MMCHS_AC12_SCLK_SEL_MASK ((uint32_t)0x00800000U)
#define CSL_MMCHS_AC12_SCLK_SEL_SHIFT ((uint32_t)23U)
#define CSL_MMCHS_AC12_SCLK_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_SCLK_SEL_TUNED ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_SCLK_SEL_FIXED ((uint32_t)0x00000000U)

#define CSL_MMCHS_AC12_AI_ENABLE_MASK ((uint32_t)0x40000000U)
#define CSL_MMCHS_AC12_AI_ENABLE_SHIFT ((uint32_t)30U)
#define CSL_MMCHS_AC12_AI_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_AI_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_AI_ENABLE_DISABLED ((uint32_t)0x00000000U)

#define CSL_MMCHS_AC12_PV_ENABLE_MASK ((uint32_t)0x80000000U)
#define CSL_MMCHS_AC12_PV_ENABLE_SHIFT ((uint32_t)31U)
#define CSL_MMCHS_AC12_PV_ENABLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_AC12_PV_ENABLE_ENABLED ((uint32_t)0x00000001U)
#define CSL_MMCHS_AC12_PV_ENABLE_DISABLED ((uint32_t)0x00000000U)

#define CSL_MMCHS_AC12_RESETVAL ((uint32_t)0x00000000U)

    /* CAPA */

#define CSL_MMCHS_CAPA_HSS_MASK ((uint32_t)0x00200000U)
#define CSL_MMCHS_CAPA_HSS_SHIFT ((uint32_t)21U)
#define CSL_MMCHS_CAPA_HSS_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_HSS_NOTSUPPORTED ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_HSS_SUPPORTED ((uint32_t)0x00000001U)

#define CSL_MMCHS_CAPA_SRS_MASK ((uint32_t)0x00800000U)
#define CSL_MMCHS_CAPA_SRS_SHIFT ((uint32_t)23U)
#define CSL_MMCHS_CAPA_SRS_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_SRS_NOTSUPPORTED ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_SRS_SUPPORTED ((uint32_t)0x00000001U)

#define CSL_MMCHS_CAPA_DS_MASK ((uint32_t)0x00400000U)
#define CSL_MMCHS_CAPA_DS_SHIFT ((uint32_t)22U)
#define CSL_MMCHS_CAPA_DS_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_DS_NOTSUPPORTED ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_DS_SUPPORTED ((uint32_t)0x00000001U)

#define CSL_MMCHS_CAPA_BCF_MASK ((uint32_t)0x0000FF00U)
#define CSL_MMCHS_CAPA_BCF_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_CAPA_BCF_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_BCF_MAX ((uint32_t)0x000000ffU)

#define CSL_MMCHS_CAPA_MBL_MASK ((uint32_t)0x00030000U)
#define CSL_MMCHS_CAPA_MBL_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_CAPA_MBL_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_MBL__512 ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_MBL__1024 ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_MBL__2048 ((uint32_t)0x00000002U)

#define CSL_MMCHS_CAPA_VS18_MASK ((uint32_t)0x04000000U)
#define CSL_MMCHS_CAPA_VS18_SHIFT ((uint32_t)26U)
#define CSL_MMCHS_CAPA_VS18_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_VS18_ST_1V8SUP_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_VS18__1V8_SUP_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_VS18__1V8_NOTSUP_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_VS18_ST_1V8NOTSUP_W ((uint32_t)0x00000000U)

#define CSL_MMCHS_CAPA_TCU_MASK ((uint32_t)0x00000080U)
#define CSL_MMCHS_CAPA_TCU_SHIFT ((uint32_t)7U)
#define CSL_MMCHS_CAPA_TCU_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_TCU_MHZ ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_TCU_KHZ ((uint32_t)0x00000001U)

#define CSL_MMCHS_CAPA_VS33_MASK ((uint32_t)0x01000000U)
#define CSL_MMCHS_CAPA_VS33_SHIFT ((uint32_t)24U)
#define CSL_MMCHS_CAPA_VS33_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_VS33_ST_3V3SUP_W ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_VS33_ST_3V3NOTSUP_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_VS33__3V3_SUP_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_VS33__3V3_NOTSUP_R ((uint32_t)0x00000000U)

#define CSL_MMCHS_CAPA_VS30_MASK ((uint32_t)0x02000000U)
#define CSL_MMCHS_CAPA_VS30_SHIFT ((uint32_t)25U)
#define CSL_MMCHS_CAPA_VS30_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_VS30_ST_3V0NOTSUP_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_VS30__3V0_NOTSUP_R ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_VS30__3V0_SUP_R ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_VS30_ST_3V0SUP_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_CAPA_TCF_MASK ((uint32_t)0x0000003FU)
#define CSL_MMCHS_CAPA_TCF_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_CAPA_TCF_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_TCF_MAX ((uint32_t)0x0000003fU)

#define CSL_MMCHS_CAPA_BIT64_MASK ((uint32_t)0x10000000U)
#define CSL_MMCHS_CAPA_BIT64_SHIFT ((uint32_t)28U)
#define CSL_MMCHS_CAPA_BIT64_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_BIT64_SYSADDR32B ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_BIT64_SYSADDR64B ((uint32_t)0x00000001U)

#define CSL_MMCHS_CAPA_AD2S_MASK ((uint32_t)0x00080000U)
#define CSL_MMCHS_CAPA_AD2S_SHIFT ((uint32_t)19U)
#define CSL_MMCHS_CAPA_AD2S_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_AD2S_ADMA2NOTSUPPORTED ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA_AD2S_ADMA2SUPPORTED ((uint32_t)0x00000001U)

#define CSL_MMCHS_CAPA_AIS_MASK ((uint32_t)0x20000000U)
#define CSL_MMCHS_CAPA_AIS_SHIFT ((uint32_t)29U)
#define CSL_MMCHS_CAPA_AIS_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_AIS_AIS_SUP ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA_AIS_AIS_NOTSUP ((uint32_t)0x00000000U)

#define CSL_MMCHS_CAPA_RESETVAL ((uint32_t)0x20e90080U)

    /* CAPA2 */

#define CSL_MMCHS_CAPA2_SDR50_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_SDR50_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_CAPA2_SDR50_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_SDR50_SUPPORTED ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_SDR50_NOTSUPPORTED ((uint32_t)0x00000000U)

#define CSL_MMCHS_CAPA2_SDR104_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_CAPA2_SDR104_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_CAPA2_SDR104_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_SDR104_SUPPORTED ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_SDR104_NOTSUPPORTED ((uint32_t)0x00000000U)

#define CSL_MMCHS_CAPA2_DDR50_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_CAPA2_DDR50_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_CAPA2_DDR50_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_DDR50_SUPPORTED ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_DDR50_NOTSUPPORTED ((uint32_t)0x00000000U)

#define CSL_MMCHS_CAPA2_DTA_MASK ((uint32_t)0x00000010U)
#define CSL_MMCHS_CAPA2_DTA_SHIFT ((uint32_t)4U)
#define CSL_MMCHS_CAPA2_DTA_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_DTA_SUPPORTED ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_DTA_NOTSUPPORTED ((uint32_t)0x00000000U)

#define CSL_MMCHS_CAPA2_DTC_MASK ((uint32_t)0x00000020U)
#define CSL_MMCHS_CAPA2_DTC_SHIFT ((uint32_t)5U)
#define CSL_MMCHS_CAPA2_DTC_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_DTC_SUPPORTED ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_DTC_NOTSUPPORTED ((uint32_t)0x00000000U)

#define CSL_MMCHS_CAPA2_DTD_MASK ((uint32_t)0x00000040U)
#define CSL_MMCHS_CAPA2_DTD_SHIFT ((uint32_t)6U)
#define CSL_MMCHS_CAPA2_DTD_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_DTD_SUPPORTED ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_DTD_NOTSUPPORTED ((uint32_t)0x00000000U)

#define CSL_MMCHS_CAPA2_TCRT_MASK ((uint32_t)0x00000F00U)
#define CSL_MMCHS_CAPA2_TCRT_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_CAPA2_TCRT_RESETVAL ((uint32_t)0x0000000fU)
#define CSL_MMCHS_CAPA2_TCRT_MAX ((uint32_t)0x0000000fU)

#define CSL_MMCHS_CAPA2_TSDR50_MASK ((uint32_t)0x00002000U)
#define CSL_MMCHS_CAPA2_TSDR50_SHIFT ((uint32_t)13U)
#define CSL_MMCHS_CAPA2_TSDR50_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA2_TSDR50_REQUIRED ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_TSDR50_NOTREQUIRED ((uint32_t)0x00000000U)

#define CSL_MMCHS_CAPA2_RTM_MASK ((uint32_t)0x0000C000U)
#define CSL_MMCHS_CAPA2_RTM_SHIFT ((uint32_t)14U)
#define CSL_MMCHS_CAPA2_RTM_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA2_RTM_MODE1 ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA2_RTM_MODE2 ((uint32_t)0x00000001U)
#define CSL_MMCHS_CAPA2_RTM_MODE3 ((uint32_t)0x00000002U)
#define CSL_MMCHS_CAPA2_RTM_RESERVED ((uint32_t)0x00000003U)

#define CSL_MMCHS_CAPA2_CM_MASK ((uint32_t)0x00FF0000U)
#define CSL_MMCHS_CAPA2_CM_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_CAPA2_CM_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CAPA2_CM_MAX ((uint32_t)0x000000ffU)

#define CSL_MMCHS_CAPA2_RESETVAL ((uint32_t)0x00000f77U)

    /* CUR_CAPA */

#define CSL_MMCHS_CUR_CAPA_CUR_3V3_MASK ((uint32_t)0x000000FFU)
#define CSL_MMCHS_CUR_CAPA_CUR_3V3_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_CUR_CAPA_CUR_3V3_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CUR_CAPA_CUR_3V3_MAX ((uint32_t)0x000000ffU)

#define CSL_MMCHS_CUR_CAPA_CUR_1V8_MASK ((uint32_t)0x00FF0000U)
#define CSL_MMCHS_CUR_CAPA_CUR_1V8_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_CUR_CAPA_CUR_1V8_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CUR_CAPA_CUR_1V8_MAX ((uint32_t)0x000000ffU)

#define CSL_MMCHS_CUR_CAPA_CUR_3V0_MASK ((uint32_t)0x0000FF00U)
#define CSL_MMCHS_CUR_CAPA_CUR_3V0_SHIFT ((uint32_t)8U)
#define CSL_MMCHS_CUR_CAPA_CUR_3V0_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_CUR_CAPA_CUR_3V0_MAX ((uint32_t)0x000000ffU)

#define CSL_MMCHS_CUR_CAPA_RESETVAL ((uint32_t)0x00000000U)

    /* FE */

#define CSL_MMCHS_FE_FE_CEB_MASK ((uint32_t)0x00040000U)
#define CSL_MMCHS_FE_FE_CEB_SHIFT ((uint32_t)18U)
#define CSL_MMCHS_FE_FE_CEB_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_CEB_NOACTION ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_CEB_INTFORCED ((uint32_t)0x00000001U)

#define CSL_MMCHS_FE_FE_CTO_MASK ((uint32_t)0x00010000U)
#define CSL_MMCHS_FE_FE_CTO_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_FE_FE_CTO_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_CTO_ST_UN_W ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_CTO_ST_RST_W ((uint32_t)0x00000001U)

#define CSL_MMCHS_FE_FE_DCRC_MASK ((uint32_t)0x00200000U)
#define CSL_MMCHS_FE_FE_DCRC_SHIFT ((uint32_t)21U)
#define CSL_MMCHS_FE_FE_DCRC_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_DCRC_INTFORCED ((uint32_t)0x00000001U)
#define CSL_MMCHS_FE_FE_DCRC_NOACTION ((uint32_t)0x00000000U)

#define CSL_MMCHS_FE_FE_BADA_MASK ((uint32_t)0x20000000U)
#define CSL_MMCHS_FE_FE_BADA_SHIFT ((uint32_t)29U)
#define CSL_MMCHS_FE_FE_BADA_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_BADA_NOACTION ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_BADA_INTFORCED ((uint32_t)0x00000001U)

#define CSL_MMCHS_FE_FE_CCRC_MASK ((uint32_t)0x00020000U)
#define CSL_MMCHS_FE_FE_CCRC_SHIFT ((uint32_t)17U)
#define CSL_MMCHS_FE_FE_CCRC_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_CCRC_INTFORCED ((uint32_t)0x00000001U)
#define CSL_MMCHS_FE_FE_CCRC_NOACTION ((uint32_t)0x00000000U)

#define CSL_MMCHS_FE_FE_ACE_MASK ((uint32_t)0x01000000U)
#define CSL_MMCHS_FE_FE_ACE_SHIFT ((uint32_t)24U)
#define CSL_MMCHS_FE_FE_ACE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_ACE_INTFORCED ((uint32_t)0x00000001U)
#define CSL_MMCHS_FE_FE_ACE_NOACTION ((uint32_t)0x00000000U)

#define CSL_MMCHS_FE_FE_DEB_MASK ((uint32_t)0x00400000U)
#define CSL_MMCHS_FE_FE_DEB_SHIFT ((uint32_t)22U)
#define CSL_MMCHS_FE_FE_DEB_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_DEB_NOACTION ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_DEB_INTFORCED ((uint32_t)0x00000001U)

#define CSL_MMCHS_FE_FE_CIE_MASK ((uint32_t)0x00080000U)
#define CSL_MMCHS_FE_FE_CIE_SHIFT ((uint32_t)19U)
#define CSL_MMCHS_FE_FE_CIE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_CIE_INTFORCED ((uint32_t)0x00000001U)
#define CSL_MMCHS_FE_FE_CIE_NOACTION ((uint32_t)0x00000000U)

#define CSL_MMCHS_FE_FE_DTO_MASK ((uint32_t)0x00100000U)
#define CSL_MMCHS_FE_FE_DTO_SHIFT ((uint32_t)20U)
#define CSL_MMCHS_FE_FE_DTO_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_DTO_NOACTION ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_DTO_INTFORCED ((uint32_t)0x00000001U)

#define CSL_MMCHS_FE_FE_CERR_MASK ((uint32_t)0x10000000U)
#define CSL_MMCHS_FE_FE_CERR_SHIFT ((uint32_t)28U)
#define CSL_MMCHS_FE_FE_CERR_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_CERR_INTFORCED ((uint32_t)0x00000001U)
#define CSL_MMCHS_FE_FE_CERR_NOACTION ((uint32_t)0x00000000U)

#define CSL_MMCHS_FE_FE_CLE_MASK ((uint32_t)0x00800000U)
#define CSL_MMCHS_FE_FE_CLE_SHIFT ((uint32_t)23U)
#define CSL_MMCHS_FE_FE_CLE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_CLE_MAX ((uint32_t)0x00000001U)

#define CSL_MMCHS_FE_FE_ADMAE_MASK ((uint32_t)0x02000000U)
#define CSL_MMCHS_FE_FE_ADMAE_SHIFT ((uint32_t)25U)
#define CSL_MMCHS_FE_FE_ADMAE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_ADMAE_NOACTION ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_ADMAE_INTFORCED ((uint32_t)0x00000001U)

#define CSL_MMCHS_FE_FE_ACNE_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_FE_FE_ACNE_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_FE_FE_ACNE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_ACNE_INTFORCED ((uint32_t)0x00000001U)
#define CSL_MMCHS_FE_FE_ACNE_NOACTION ((uint32_t)0x00000000U)

#define CSL_MMCHS_FE_FE_ACCE_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_FE_FE_ACCE_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_FE_FE_ACCE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_ACCE_INTFORCED ((uint32_t)0x00000001U)
#define CSL_MMCHS_FE_FE_ACCE_NOACTION ((uint32_t)0x00000000U)

#define CSL_MMCHS_FE_FE_ACIE_MASK ((uint32_t)0x00000010U)
#define CSL_MMCHS_FE_FE_ACIE_SHIFT ((uint32_t)4U)
#define CSL_MMCHS_FE_FE_ACIE_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_ACIE_INTFORCED ((uint32_t)0x00000001U)
#define CSL_MMCHS_FE_FE_ACIE_NOACTION ((uint32_t)0x00000000U)

#define CSL_MMCHS_FE_FE_ACEB_MASK ((uint32_t)0x00000008U)
#define CSL_MMCHS_FE_FE_ACEB_SHIFT ((uint32_t)3U)
#define CSL_MMCHS_FE_FE_ACEB_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_ACEB_NOACTION ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_ACEB_INTFORCED ((uint32_t)0x00000001U)

#define CSL_MMCHS_FE_FE_ACTO_MASK ((uint32_t)0x00000002U)
#define CSL_MMCHS_FE_FE_ACTO_SHIFT ((uint32_t)1U)
#define CSL_MMCHS_FE_FE_ACTO_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_ACTO_NOACTION ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_ACTO_INTFORCED ((uint32_t)0x00000001U)

#define CSL_MMCHS_FE_FE_CNI_MASK ((uint32_t)0x00000080U)
#define CSL_MMCHS_FE_FE_CNI_SHIFT ((uint32_t)7U)
#define CSL_MMCHS_FE_FE_CNI_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_CNI_NOACTION ((uint32_t)0x00000000U)
#define CSL_MMCHS_FE_FE_CNI_INTFORCED ((uint32_t)0x00000001U)

#define CSL_MMCHS_FE_RESETVAL ((uint32_t)0x00000000U)

    /* ADMAES */

#define CSL_MMCHS_ADMAES_LME_MASK ((uint32_t)0x00000004U)
#define CSL_MMCHS_ADMAES_LME_SHIFT ((uint32_t)2U)
#define CSL_MMCHS_ADMAES_LME_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ADMAES_LME_NOERROR ((uint32_t)0x00000000U)
#define CSL_MMCHS_ADMAES_LME_ERROR ((uint32_t)0x00000001U)

#define CSL_MMCHS_ADMAES_AES_MASK ((uint32_t)0x00000003U)
#define CSL_MMCHS_ADMAES_AES_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_ADMAES_AES_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ADMAES_AES_SYSSDR ((uint32_t)0x00000000U)
#define CSL_MMCHS_ADMAES_AES_LINKDESC ((uint32_t)0x00000001U)
#define CSL_MMCHS_ADMAES_AES_RESERVED ((uint32_t)0x00000002U)
#define CSL_MMCHS_ADMAES_AES_TRANSDATA ((uint32_t)0x00000003U)

#define CSL_MMCHS_ADMAES_RESETVAL ((uint32_t)0x00000000U)

    /* ADMASAL */

#define CSL_MMCHS_ADMASAL_ADMA_A32B_MASK ((uint32_t)0xFFFFFFFFU)
#define CSL_MMCHS_ADMASAL_ADMA_A32B_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_ADMASAL_ADMA_A32B_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_ADMASAL_ADMA_A32B_MAX ((uint32_t)0xffffffffU)

#define CSL_MMCHS_ADMASAL_RESETVAL ((uint32_t)0x00000000U)

    /* PVINITSD */

#define CSL_MMCHS_PVINITSD_INITSDCLK_SEL_MASK ((uint32_t)0x000003FFU)
#define CSL_MMCHS_PVINITSD_INITSDCLK_SEL_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_PVINITSD_INITSDCLK_SEL_RESETVAL ((uint32_t)0x000001e0U)
#define CSL_MMCHS_PVINITSD_INITSDCLK_SEL_MAX ((uint32_t)0x000003ffU)

#define CSL_MMCHS_PVINITSD_INITCLKGEN_SEL_MASK ((uint32_t)0x00000400U)
#define CSL_MMCHS_PVINITSD_INITCLKGEN_SEL_SHIFT ((uint32_t)10U)
#define CSL_MMCHS_PVINITSD_INITCLKGEN_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVINITSD_INITCLKGEN_SEL_PROG ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVINITSD_INITCLKGEN_SEL_HOST ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVINITSD_INITDS_SEL_MASK ((uint32_t)0x0000C000U)
#define CSL_MMCHS_PVINITSD_INITDS_SEL_SHIFT ((uint32_t)14U)
#define CSL_MMCHS_PVINITSD_INITDS_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVINITSD_INITDS_SEL_DTD ((uint32_t)0x00000003U)
#define CSL_MMCHS_PVINITSD_INITDS_SEL_DTC ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVINITSD_INITDS_SEL_DTA ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVINITSD_INITDS_SEL_DTB ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVINITSD_DSSDCLK_SEL_MASK ((uint32_t)0x03FF0000U)
#define CSL_MMCHS_PVINITSD_DSSDCLK_SEL_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_PVINITSD_DSSDCLK_SEL_RESETVAL ((uint32_t)0x00000004U)
#define CSL_MMCHS_PVINITSD_DSSDCLK_SEL_MAX ((uint32_t)0x000003ffU)

#define CSL_MMCHS_PVINITSD_DSCLKGEN_SEL_MASK ((uint32_t)0x04000000U)
#define CSL_MMCHS_PVINITSD_DSCLKGEN_SEL_SHIFT ((uint32_t)26U)
#define CSL_MMCHS_PVINITSD_DSCLKGEN_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVINITSD_DSCLKGEN_SEL_PROG ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVINITSD_DSCLKGEN_SEL_HOST ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVINITSD_DSDS_SEL_MASK ((uint32_t)0xC0000000U)
#define CSL_MMCHS_PVINITSD_DSDS_SEL_SHIFT ((uint32_t)30U)
#define CSL_MMCHS_PVINITSD_DSDS_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVINITSD_DSDS_SEL_DTD ((uint32_t)0x00000003U)
#define CSL_MMCHS_PVINITSD_DSDS_SEL_DTC ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVINITSD_DSDS_SEL_DTA ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVINITSD_DSDS_SEL_DTB ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVINITSD_RESETVAL ((uint32_t)0x000401e0U)

    /* PVHSSDR12 */

#define CSL_MMCHS_PVHSSDR12_HSSDCLK_SEL_MASK ((uint32_t)0x000003FFU)
#define CSL_MMCHS_PVHSSDR12_HSSDCLK_SEL_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_PVHSSDR12_HSSDCLK_SEL_RESETVAL ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVHSSDR12_HSSDCLK_SEL_MAX ((uint32_t)0x000003ffU)

#define CSL_MMCHS_PVHSSDR12_HSCLKGEN_SEL_MASK ((uint32_t)0x00000400U)
#define CSL_MMCHS_PVHSSDR12_HSCLKGEN_SEL_SHIFT ((uint32_t)10U)
#define CSL_MMCHS_PVHSSDR12_HSCLKGEN_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVHSSDR12_HSCLKGEN_SEL_PROG ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVHSSDR12_HSCLKGEN_SEL_HOST ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVHSSDR12_HSDS_SEL_MASK ((uint32_t)0x0000C000U)
#define CSL_MMCHS_PVHSSDR12_HSDS_SEL_SHIFT ((uint32_t)14U)
#define CSL_MMCHS_PVHSSDR12_HSDS_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVHSSDR12_HSDS_SEL_DTB ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVHSSDR12_HSDS_SEL_DTA ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVHSSDR12_HSDS_SEL_DTC ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVHSSDR12_HSDS_SEL_DTD ((uint32_t)0x00000003U)

#define CSL_MMCHS_PVHSSDR12_SDR12SDCLK_SEL_MASK ((uint32_t)0x03FF0000U)
#define CSL_MMCHS_PVHSSDR12_SDR12SDCLK_SEL_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_PVHSSDR12_SDR12SDCLK_SEL_RESETVAL ((uint32_t)0x00000004U)
#define CSL_MMCHS_PVHSSDR12_SDR12SDCLK_SEL_MAX ((uint32_t)0x000003ffU)

#define CSL_MMCHS_PVHSSDR12_SDR12CLKGEN_SEL_MASK ((uint32_t)0x04000000U)
#define CSL_MMCHS_PVHSSDR12_SDR12CLKGEN_SEL_SHIFT ((uint32_t)26U)
#define CSL_MMCHS_PVHSSDR12_SDR12CLKGEN_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVHSSDR12_SDR12CLKGEN_SEL_PROG ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVHSSDR12_SDR12CLKGEN_SEL_HOST ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVHSSDR12_SDR12DS_SEL_MASK ((uint32_t)0xC0000000U)
#define CSL_MMCHS_PVHSSDR12_SDR12DS_SEL_SHIFT ((uint32_t)30U)
#define CSL_MMCHS_PVHSSDR12_SDR12DS_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVHSSDR12_SDR12DS_SEL_DTD ((uint32_t)0x00000003U)
#define CSL_MMCHS_PVHSSDR12_SDR12DS_SEL_DTC ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVHSSDR12_SDR12DS_SEL_DTA ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVHSSDR12_SDR12DS_SEL_DTB ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVHSSDR12_RESETVAL ((uint32_t)0x00040002U)

    /* PVSDR25SDR50 */

#define CSL_MMCHS_PVSDR25SDR50_SDR25SDCLK_SEL_MASK ((uint32_t)0x000003FFU)
#define CSL_MMCHS_PVSDR25SDR50_SDR25SDCLK_SEL_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25SDCLK_SEL_RESETVAL ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25SDCLK_SEL_MAX ((uint32_t)0x000003ffU)

#define CSL_MMCHS_PVSDR25SDR50_SDR25CLKGEN_SEL_MASK ((uint32_t)0x00000400U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25CLKGEN_SEL_SHIFT ((uint32_t)10U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25CLKGEN_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25CLKGEN_SEL_PROG ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25CLKGEN_SEL_HOST ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVSDR25SDR50_SDR25DS_SEL_MASK ((uint32_t)0x0000C000U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25DS_SEL_SHIFT ((uint32_t)14U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25DS_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25DS_SEL_DTD ((uint32_t)0x00000003U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25DS_SEL_DTC ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25DS_SEL_DTA ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVSDR25SDR50_SDR25DS_SEL_DTB ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVSDR25SDR50_SDR50SDCLK_SEL_MASK ((uint32_t)0x03FF0000U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50SDCLK_SEL_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50SDCLK_SEL_RESETVAL ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50SDCLK_SEL_MAX ((uint32_t)0x000003ffU)

#define CSL_MMCHS_PVSDR25SDR50_SDR50CLKGEN_SEL_MASK ((uint32_t)0x04000000U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50CLKGEN_SEL_SHIFT ((uint32_t)26U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50CLKGEN_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50CLKGEN_SEL_PROG ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50CLKGEN_SEL_HOST ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVSDR25SDR50_SDR50DS_SEL_MASK ((uint32_t)0xC0000000U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50DS_SEL_SHIFT ((uint32_t)30U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50DS_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50DS_SEL_DTD ((uint32_t)0x00000003U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50DS_SEL_DTC ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50DS_SEL_DTA ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVSDR25SDR50_SDR50DS_SEL_DTB ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVSDR25SDR50_RESETVAL ((uint32_t)0x00010002U)

    /* PVSDR104DDR50 */

#define CSL_MMCHS_PVSDR104DDR50_SDR104SDCLK_SEL_MASK ((uint32_t)0x000003FFU)
#define CSL_MMCHS_PVSDR104DDR50_SDR104SDCLK_SEL_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104SDCLK_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104SDCLK_SEL_MAX ((uint32_t)0x000003ffU)

#define CSL_MMCHS_PVSDR104DDR50_SDR104CLKGEN_SEL_MASK ((uint32_t)0x00000400U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104CLKGEN_SEL_SHIFT ((uint32_t)10U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104CLKGEN_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104CLKGEN_SEL_PROG ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104CLKGEN_SEL_HOST ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVSDR104DDR50_SDR104DS_SEL_MASK ((uint32_t)0x0000C000U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104DS_SEL_SHIFT ((uint32_t)14U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104DS_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104DS_SEL_DTD ((uint32_t)0x00000003U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104DS_SEL_DTC ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104DS_SEL_DTA ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVSDR104DDR50_SDR104DS_SEL_DTB ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVSDR104DDR50_DDR50SDCLK_SEL_MASK ((uint32_t)0x03FF0000U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50SDCLK_SEL_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50SDCLK_SEL_RESETVAL ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50SDCLK_SEL_MAX ((uint32_t)0x000003ffU)

#define CSL_MMCHS_PVSDR104DDR50_DDR50CLKGEN_SEL_MASK ((uint32_t)0x04000000U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50CLKGEN_SEL_SHIFT ((uint32_t)26U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50CLKGEN_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50CLKGEN_SEL_PROG ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50CLKGEN_SEL_HOST ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVSDR104DDR50_DDR50DS_SEL_MASK ((uint32_t)0xC0000000U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50DS_SEL_SHIFT ((uint32_t)30U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50DS_SEL_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50DS_SEL_DTD ((uint32_t)0x00000003U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50DS_SEL_DTC ((uint32_t)0x00000002U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50DS_SEL_DTA ((uint32_t)0x00000001U)
#define CSL_MMCHS_PVSDR104DDR50_DDR50DS_SEL_DTB ((uint32_t)0x00000000U)

#define CSL_MMCHS_PVSDR104DDR50_RESETVAL ((uint32_t)0x00020000U)

    /* REV */

#define CSL_MMCHS_REV_VREV_MASK ((uint32_t)0xFF000000U)
#define CSL_MMCHS_REV_VREV_SHIFT ((uint32_t)24U)
#define CSL_MMCHS_REV_VREV_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_REV_VREV_MAX ((uint32_t)0x000000ffU)

#define CSL_MMCHS_REV_SIS_MASK ((uint32_t)0x00000001U)
#define CSL_MMCHS_REV_SIS_SHIFT ((uint32_t)0U)
#define CSL_MMCHS_REV_SIS_RESETVAL ((uint32_t)0x00000000U)
#define CSL_MMCHS_REV_SIS_MAX ((uint32_t)0x00000001U)

#define CSL_MMCHS_REV_SREV_MASK ((uint32_t)0x00FF0000U)
#define CSL_MMCHS_REV_SREV_SHIFT ((uint32_t)16U)
#define CSL_MMCHS_REV_SREV_RESETVAL ((uint32_t)0x00000002U)
#define CSL_MMCHS_REV_SREV_MAX ((uint32_t)0x000000ffU)

#define CSL_MMCHS_REV_RESETVAL ((uint32_t)0x00020000U)

#ifdef __cplusplus
}
#endif
#endif
