/*
 *  Copyright (C) 2013 Texas Instruments Incorporated - www.ti.com/
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
 *  \defgroup CSL_I2C I2C
 *
 *  @{
 */
 /**
 *  \file       i2c.h
 *
 *  \brief      This file contains the function prototypes for the device
 *              abstraction layer for high speed I2C. It also contains some
 *              related macro definitions and some files to be included.
 *
 *  \details    Please find the below detailed description of i2c DAL.
 *              -# I2C controller will basically operates in the following modes
 *                  -# Master Transmitter or Master Receiver.
 *                  -# Slave.
 *              -# Programming sequence for initializing the i2c controller in
 *                 master mode.
 *                  -# Perform Clock and pinmux configuration.
 *                  -# Disable the module through API #I2CMasterDisable.
 *                  -# Disable the auto idle mode through API
 *                     #I2CAutoIdleDisable
 *                  -# Configure the internal clock dividers of i2c controller
 *                     through API #I2CMasterInitExpClk
 *                  -# Configure the slave address through the API
 *                     #I2CMasterSlaveAddrSet
 *                  -# Enable the i2c module through API #I2CMasterEnable
 *              -# Programming sequence for setting up i2c write
 *                  -# Set the transfer length (number of bytes) through the API
 *                     #I2CSetDataCount
 *                  -# Configure the i2c controller in master transmitter mode
 *                     using the API #I2CMasterControl
 *                  -# Enable necessary interrupts using the API in master mode
 *                     #I2CMasterIntEnableEx
 *                  -# Generate start condition using the API #I2CMasterStart
 *                  -# Check for data completion either through interrupts or
 *                     through status checking.
 *              -# Programming sequence for setting up i2c read is same as write
 *                 except configuring the i2c controller in master receiver mode
 *
 **/

#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "hw_i2c.h"

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */
/*
 * \name Values that can be passed to  I2CMasterControl API as command to
 * configure mode of operation of I2C in Master mode.
 *  @{
 */
/**
 * \brief I2C_CFG_MST_TX               - Master transmit mode.
 */
#define     I2C_CFG_MST_TX              (((uint32_t) I2C_CON_TRX_MASK) | \
                                         (uint32_t) (I2C_CON_MST_MASK))
/**
 * \brief I2C_CFG_MST_RX               - Matter receive mode.
 */
#define     I2C_CFG_MST_RX              ((uint32_t) I2C_CON_MST_MASK)
/**
 * \brief I2C_CFG_STOP                 - Stop condition.
 */
#define     I2C_CFG_STOP                ((uint32_t) I2C_CON_STP_MASK)
/**
 * \brief I2C_CFG_N0RMAL_MODE          - Normal mode.
 */
#define     I2C_CFG_N0RMAL_MODE         ((uint32_t) 0 << I2C_CON_STB_SHIFT)
/**
 * \brief I2C_CFG_SRT_BYTE_MODE        - Start byte mode.
 */
#define     I2C_CFG_SRT_BYTE_MODE       ((uint32_t) I2C_CON_STB_MASK)
/**
 * \brief I2C_CFG_7BIT_SLAVE_ADDR      - 7 bit slave address.
 */
#define     I2C_CFG_7BIT_SLAVE_ADDR     ((uint32_t) 0 << I2C_CON_XSA_SHIFT)
/**
 * \brief I2C_CFG_10BIT_SLAVE_ADDR     - 10 bit slave address.
 */
#define     I2C_CFG_10BIT_SLAVE_ADDR    ((uint32_t) I2C_CON_XSA_MASK)
/**
 * \brief I2C_CFG_10BIT_OWN_ADDR_0     - Master mode 10 bit own address 0
 */
#define     I2C_CFG_10BIT_OWN_ADDR_0    ((uint32_t) I2C_CON_XOA0_MASK)
/**
 * \brief I2C_CFG_10BIT_OWN_ADDR_1     - Master mode 10 bit own address 1
 */
#define     I2C_CFG_10BIT_OWN_ADDR_1    ((uint32_t) I2C_CON_XOA1_MASK)
/**
 * \brief I2C_CFG_10BIT_OWN_ADDR_2     - Master mode 10 bit own address 2
 */
#define     I2C_CFG_10BIT_OWN_ADDR_2    ((uint32_t) I2C_CON_XOA2_MASK)
/**
 * \brief I2C_CFG_10BIT_OWN_ADDR_3     - Master mode 10 bit own address 3
 */
#define     I2C_CFG_10BIT_OWN_ADDR_3    ((uint32_t) I2C_CON_XOA3_MASK)
/**
 * \brief I2C_CFG_7BIT_OWN_ADDR_0     - Master mode 7 bit own address 0
 */
#define     I2C_CFG_7BIT_OWN_ADDR_0     ((uint32_t) 0 << I2C_CON_XOA0_SHIFT)
/**
 * \brief I2C_CFG_7BIT_OWN_ADDR_1     - Master mode 7 bit own address 1
 */
#define     I2C_CFG_7BIT_OWN_ADDR_1     ((uint32_t) 0 << I2C_CON_XOA1_SHIFT)
/**
 * \brief I2C_CFG_7BIT_OWN_ADDR_2     - Master mode 7 bit own address 2
 */
#define     I2C_CFG_7BIT_OWN_ADDR_2     ((uint32_t) 0 << I2C_CON_XOA2_SHIFT)
/**
 * \brief I2C_CFG_7BIT_OWN_ADDR_3     - Master mode 7 bit own address 3
 */
#define     I2C_CFG_7BIT_OWN_ADDR_3     ((uint32_t) 0 << I2C_CON_XOA3_SHIFT)
/**
 * \brief I2C_CFG_MST_ENABLE          - I2C module enable
 */
#define     I2C_CFG_MST_ENABLE          ((uint32_t) I2C_CON_I2C_EN_MASK)
/**
 * \brief I2C_CFG_START               - Start condition, initiate I2C transfer
 */
#define     I2C_CFG_START               ((uint32_t) I2C_CON_STT_MASK)
/**
 * \brief I2C_CFG_MST                 - I2C configure master mode.
 */
#define     I2C_CFG_MST                 ((uint32_t) I2C_CON_MST_MASK)
/**
 * \brief I2C_CFG_HS_MODE             - High speed operation mode
 */
#define     I2C_CFG_HS_MOD              ((uint32_t) CSL_I2C_CON_OPMODE_HSI2C << CSL_I2C_CON_OPMODE_SHIFT)

/* @} */

/*
 * \name Values that can be passed to  I2CMasterIntEnableEx API as intFlag to Enable
 * interrupts.
 * @{
*/
/**
 * \brief I2C_INT_ARBITRATION_LOST     - Arbitration lost interrupt
 */
#define     I2C_INT_ARBITRATION_LOST     ((uint32_t) I2C_IRQSTATUS_AL_MASK)
/**
 * \brief I2C_INT_NO_ACK               - No acknowledgement interrupt
 */
#define     I2C_INT_NO_ACK               ((uint32_t) I2C_IRQSTATUS_NACK_MASK)
/**
 * \brief I2C_INT_ADRR_READY_ACESS     - Register access ready interrupt
 */
#define     I2C_INT_ADRR_READY_ACESS     ((uint32_t) I2C_IRQSTATUS_ARDY_MASK)
/**
 * \brief I2C_INT_RECV_READY           - Receive data ready interrupt
 */
#define     I2C_INT_RECV_READY           ((uint32_t) I2C_IRQSTATUS_RRDY_MASK)
/**
 * \brief I2C_INT_TRANSMIT_READY       - Transmit data ready interrupt
 */
#define     I2C_INT_TRANSMIT_READY       ((uint32_t) I2C_IRQSTATUS_XRDY_MASK)
/**
 * \brief I2C_INT_GENERAL_CALL         - General call Interrupt
 */
#define     I2C_INT_GENERAL_CALL         ((uint32_t) I2C_IRQSTATUS_GC_MASK)
/**
 * \brief I2C_INT_START                - Start Condition interrupt
 */
#define     I2C_INT_START                ((uint32_t) I2C_IRQSTATUS_STC_MASK)
/**
 * \brief I2C_INT_ACCESS_ERROR         - Access Error interrupt
 */
#define     I2C_INT_ACCESS_ERROR         ((uint32_t) I2C_IRQSTATUS_AERR_MASK)
/**
 * \brief I2C_INT_STOP_CONDITION       - Bus Free interrupt
 */
#define     I2C_INT_STOP_CONDITION       ((uint32_t) I2C_IRQSTATUS_BF_MASK)
/**
 * \brief I2C_INT_ADRR_SLAVE           - Addressed as Slave interrupt
 */
#define     I2C_INT_ADRR_SLAVE           ((uint32_t) I2C_IRQSTATUS_AAS_MASK)
/**
 * \brief I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit underflow interrupt
 */
#define     I2C_INT_TRANSMIT_UNDER_FLOW  ((uint32_t) I2C_IRQSTATUS_XUDF_MASK)
/**
 * \brief I2C_INT_RECV_OVER_RUN        - Receive overrun interrupt
 */
#define     I2C_INT_RECV_OVER_RUN        ((uint32_t) I2C_IRQSTATUS_ROVR_MASK)
/**
 * \brief I2C_INT_RECV_DRAIN           - Receive Draining interrupt
 */
#define     I2C_INT_RECV_DRAIN           ((uint32_t) I2C_IRQSTATUS_RDR_MASK)
/**
 * \brief I2C_INT_TRANSMIT_DRAIN       - Transmit Draining interrupt
 */
#define     I2C_INT_TRANSMIT_DRAIN       ((uint32_t) I2C_IRQSTATUS_XDR_MASK)
/**
 * \brief I2C_INT_BUS_BUSY             - Bus busy interrupt raw status
 */
#define     I2C_INT_BUS_BUSY             ((uint32_t) I2C_IRQSTATUS_RAW_BB_MASK)
/**
 * \brief I2C_INT_BUS_FREE             - Bus free interrupt raw status
 */
#define     I2C_INT_BUS_FREE             ((uint32_t) I2C_IRQSTATUS_RAW_BF_MASK)
/**
 * \brief I2C_INT_ALL                  - Enable all interrupt
 */
#define     I2C_INT_ALL                  ((uint32_t) 0x7FFFU)
/* @} */

/*
 * \name Values that can be passed to I2CFIFOThersholdConfig/I2CFIFOClear API as
 * flag to select receive or transmit mode.
 * @{
 */
/**
 * \brief I2C_TX_MODE     - Transmit mode.
 */
#define     I2C_TX_MODE                  ((uint32_t) 1U)
/**
 * \brief I2C_RX_MODE     - Receive mode.
 */
#define     I2C_RX_MODE                  ((uint32_t) 0U)
/* @} */

/*
 * \name Values that can be passed to I2CBufferStatus API as flag to get status
 * of the internal buffer.
 * @{
 */
/**
 * \brief I2C_TX_BUFFER_STATUS     - TX buffer status flag
 */
#define     I2C_TX_BUFFER_STATUS         ((uint32_t) 0U)
/**
 * \brief I2C_RX_BUFFER_STATUS     - RX buffer status flag
 */
#define     I2C_RX_BUFFER_STATUS         ((uint32_t) 1U)
/**
 * \brief I2C_FIFO_DEPTH           - Internal FIFO depth flag
 */
#define     I2C_FIFO_DEPTH               ((uint32_t) 2U)
/* @} */

/*
 * \name Values that can be passed to I2CWakeUpEnable  API as eventFlag to select
 * the type of wakeup signal to be Enabled.
 * @{
 */
/**
 * \brief I2C_WAKE_UP_ARBITRATION_LOST    - Arbitration lost IRQ wakeup set
 */
#define     I2C_WAKE_UP_ARBITRATION_LOST  (I2C_WE_AL_MASK)
/**
 * \brief I2C_WAKE_UP_NO_ACK              - No acknowledgment IRQ wakeup set
 */
#define     I2C_WAKE_UP_NO_ACK            (I2C_WE_NACK_MASK)
/**
 * \brief I2C_WAKE_UP_ADRR_RDY_ACCESS     - Register access ready IRQ wakeup set
 */
#define     I2C_WAKE_UP_ADRR_RDY_ACCESS   (I2C_WE_ARDY_MASK)
/**
 * \brief I2C_WAKE_UP_GENERAL_CALL        - General call IRQ wakeup set
 */
#define     I2C_WAKE_UP_GENERAL_CALL      (I2C_WE_GC_MASK)
/**
 * \brief I2C_WAKE_UP_START               - Start Condition IRQ wakeup set
 */
#define     I2C_WAKE_UP_START             (I2C_WE_STC_MASK)
/**
 * \brief I2C_WAKE_UP_STOP_CONDITION      - Bus Free IRQ wakeup set
 */
#define     I2C_WAKE_UP_STOP_CONDITION    (I2C_WE_BF_MASK)
/**
 * \brief I2C_WAKE_UP_ADRR_SLAVE          - Address as slave IRQ wakeup set
 */
#define     I2C_WAKE_UP_ADRR_SLAVE        (I2C_WE_AAS_MASK)
/**
 * \brief I2C_WAKE_UP_TX_UNDER_FLOW       - Transmit underflow wakeup set
 */
#define     I2C_WAKE_UP_TX_UNDER_FLOW     (I2C_WE_XUDF_MASK)
/**
 * \brief I2C_WAKE_UP_RECV_OVER_RUN       - Receive overrun wakeup set
 */
#define     I2C_WAKE_UP_RECV_OVER_RUN     (I2C_WE_ROVR_MASK)
/**
 * \brief I2C_WAKE_UP_RECV_DRAIN          - Receive Draining wakeup set
 */
#define     I2C_WAKE_UP_RECV_DRAIN        (I2C_WE_RDR_MASK)
/**
 * \brief I2C_WAKE_UP_TRANSMIT_DRAIN      - Transmit Draining wakeup set
 */
#define     I2C_WAKE_UP_TRANSMIT_DRAIN    (I2C_WE_XDR_MASK)
/**
 * \brief I2C_WAKE_UP_DATA_RECV_TX_RDY    - Receive/Transmit data ready IRQ wakeup set
 */
#define     I2C_WAKE_UP_DATA_RECV_TX_RDY  (I2C_WE_DRDY_MASK)
/* @} */

/*
 * \name Values that can be passed to I2CWakeUpEnable  API as flag to select
 * the request source to generate asynchronous signal.
 * @{
 */
/**
 * \brief I2C_WAKE_UP_IRQ           - IRQ request source
 */
#define     I2C_WAKE_UP_IRQ               ((uint32_t) 1U)
/**
 * \brief I2C_WAKE_UP_DMA_RECV      - DMA receive request source
 */
#define     I2C_WAKE_UP_DMA_RECV          ((uint32_t) 2U)
/**
 * \brief I2C_WAKE_UP_DMA_TRANSMIT  - DMA transmit request source
 */
#define     I2C_WAKE_UP_DMA_TRANSMIT      ((uint32_t) 3U)
/* @} */

/*
 * \name Values that can be passed to I2CMasterIntStatus/I2CSlaveIntStatus
 * /I2CMasterIntStatusEx/I2CSlaveIntStatusEx API as flag to select
 * RAWIRQSTATUS or IRQSTATUS.
 * @{
 */
/**
 * \brief I2C_STATUS_RAW       - RAW IRQ status
 */
#define     I2C_STATUS_RAW            ((uint32_t) 0U)
/**
 * \brief I2C_STATUS           - IRQ status
 */
#define     I2C_STATUS                ((uint32_t) 1U)

/**
 * \brief HAL_HSI2C_SUCCESS    - I2C success
 */
#define HAL_HSI2C_SUCCESS             ((uint32_t) 0U)
/**
 * \brief HAL_HSI2C_FAIL       - I2C fail
 */
#define HAL_HSI2C_FAIL                ((uint32_t) 1U)
/* @} */

/*
 * \name Values that can be passed to ownAddressSet API as flag to select
 * any one of the I2C own address field .
 * @{
 */
/**
 * \brief I2C_OWN_ADDR_0 - set own address 0
 */
#define     I2C_OWN_ADDR_0               ((uint32_t) 0U)
/**
 * \brief I2C_OWN_ADDR_1 - set own address 1
 */
#define     I2C_OWN_ADDR_1               ((uint32_t) 1U)
/**
 * \brief I2C_OWN_ADDR_2 - set own address 2
 */
#define     I2C_OWN_ADDR_2               ((uint32_t) 2U)
/**
 * \brief I2C_OWN_ADDR_3 - set own address 3
 */
#define     I2C_OWN_ADDR_3               ((uint32_t) 3U)
/* @} */

/*
 * \name MACROS used to enable or disable auto idle mechanism.
 * @{
 */
/**
 * \brief I2C_AUTOIDLE_DISABLE       Disable auto idle mechanism
 */
#define   I2C_AUTOIDLE_DISABLE    (I2C_SYSC_AUTOIDLE_DISABLE)
/**
 * \brief I2C_AUTOIDLE_ENABLE        Enable autoidle mechanism
 */
#define   I2C_AUTOIDLE_ENABLE     ((uint32_t) I2C_SYSC_AUTOIDLE_ENABLE << \
                                   I2C_SYSC_AUTOIDLE_SHIFT)
/* @} */

/*
 * \name MACRO used to select the type of clock activity.
 * @{
 */
/**
 * \brief I2C_CUT_OFF_BOTH_CLK       Both OCP and SYS Clk are cut off.
 */
#define    I2C_CUT_OFF_BOTH_CLK       (I2C_SYSC_CLKACTIVITY_BOOTHOFF)
/**
 * \brief I2C_CUT_OFF_SYS_CLK        system clock is cut off;OCP clock is
 * kept alive
 */
#define    I2C_CUT_OFF_SYS_CLK        ((uint32_t) I2C_SYSC_CLKACTIVITY_OCPON << \
                                       I2C_SYSC_CLKACTIVITY_SHIFT)
/**
 * \brief I2C_CUT_OFF_OCP_CLK        OCP clock is cut off;system clock is
 *  is kept alive
 */
#define    I2C_CUT_OFF_OCP_CLK        ((uint32_t) I2C_SYSC_CLKACTIVITY_SYSON << \
                                       I2C_SYSC_CLKACTIVITY_SHIFT)
/**
 * \brief I2C_KEEP_ALIVE_BOTH_CLK    BOTH OCP and SYS Clk are kept alive.
 */
#define    I2C_KEEP_ALIVE_BOTH_CLK    ((uint32_t) I2C_SYSC_CLKACTIVITY_BOOTHON << \
                                      I2C_SYSC_CLKACTIVITY_SHIFT)
/* @} */

/*
 * \name MACROS used to enable or disable wake up mechanism.
 * @{
 */
/**
 * \brief I2C_ENAWAKEUP__DISABLE       Disable wakeup mechanism.
 */
#define    I2C_ENAWAKEUP_DISABLE    (I2C_SYSC_ENAWAKEUP_DISABLE)
/**
 * \brief I2C_ENAWAKEUP__ENABLE        Enable wakeup mechanism
 */
#define    I2C_ENAWAKEUP_ENABLE     ((uint32_t) I2C_SYSC_ENAWAKEUP_ENABLE << \
                                     I2C_SYSC_ENAWAKEUP_SHIFT)
/* @} */

/*
 * \name MACROS used to select the type of idle mode operation.
 * @{
 */
/**
 *  \brief I2C_FORCE_IDLE_MODE           selects forced idle mode operation.
 */
#define    I2C_FORCE_IDLE_MODE          (I2C_SYSC_IDLEMODE_FORCEIDLE)
/**
 * \brief I2C_NO_IDLE_MODE              selects no idle mode operation.
 */
#define    I2C_NO_IDLE_MODE             ((uint32_t) I2C_SYSC_IDLEMODE_NOIDLE << \
                                         I2C_SYSC_IDLEMODE_SHIFT)
/**
 * \brief I2C_SMART_IDLE_MODE           selects smart idle mode operation.
 */
#define    I2C_SMART_IDLE_MODE          ((uint32_t) I2C_SYSC_IDLEMODE_SMARTIDLE << \
                                         I2C_SYSC_IDLEMODE_SHIFT)
/**
 * \brief I2C_SMART_IDLE_WAKEUP_MODE    selects smart idle
 * wakeup mode of operation.
 */
#define    I2C_SMART_IDLE_WAKEUP_MODE   ((uint32_t) I2C_SYSC_IDLEMODE_SMARTIDLE_WAKEUP << \
                                         I2C_SYSC_IDLEMODE_SHIFT)
/* @} */

/*
 * \name MACROS used to select F/S mode or HS mode of I2C.
 * @{
 */
/**
 * \brief I2C_OPMODE_FAST_STAND_MODE       Select Fast/Standard mode
 */
#define    I2C_OPMODE_FAST_STAND_MODE    (I2C_CON_OPMODE_FSI2C)
/**
 * \brief I2C_OPMODE_HIGH_SPEED_MODE       Select high speed mode
 */
#define    I2C_OPMODE_HIGH_SPEED_MODE    ((uint32_t) I2C_CON_OPMODE_HSI2C << \
                                          I2C_CON_OPMODE_SHIFT)
/* @} */

/*
 * \name MACROS used to select Start byte or normal mode of I2C.
 * @{
 */
/**
 * \brief I2C_NORMAL_MODE       Select Normal mode.
 */
#define    I2C_NORMAL_MODE   (I2C_CON_STB_NORMAL)
/**
 * \brief I2C_STB_MODE          Select start byte mode
 */
#define    I2C_STB_MODE      ((uint32_t) I2C_CON_STB_STB << I2C_CON_STB_SHIFT)
/* @} */

/*
 * \name MACROS used to slave address 7 or 10 bit mode of I2C.
 * @{
 */
/**
 * \brief I2C_XSA_7BIT        Slave address 7  bit mode.
 */
#define    I2C_XSA_7BIT     (I2C_CON_XSA_B07)
/**
 * \brief I2C_XSA_10BIT       Slave address 10 bit mode
 */
#define    I2C_XSA_10BIT    ((uint32_t) I2C_CON_XSA_B10 << I2C_CON_XSA_SHIFT)
/* @} */

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   This API will divide the system clock fed to I2C
 *          module between 12 and 100Mhz.It will also configure the I2C bus
 *          clock frequency.
 *
 * \param   baseAddr    I2C base address.
 * \param   sysClk      It is the System clock fed to I2C module.
 * \param   internalClk It is the internal clock used by I2C module.Which is
 *                      obtained by scaling System clock fed to I2C module.
 * \param   outputClk   It is the required I2C bus speed or frequency.
 *
 * @return  None.
 *
 */
void I2CMasterInitExpClk(uint32_t baseAddr,
                         uint32_t sysClk,
                         uint32_t internalClk,
                         uint32_t outputClk);

/**
 * \brief   This API Enables the I2C module.This will bring the I2C
 *          module out of reset.
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  None.
 *
 */
void I2CMasterEnable(uint32_t baseAddr);

/**
 * \brief   Enables the I2C free run module.
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  None.
 *
 */
void I2CMasterEnableFreeRun(uint32_t baseAddr);

/**
 * \brief   Set the I2C systest register.
 *
 * \param   baseAddr  It is the Memory address of the I2C instance used.
 *
 * \param   sysTest   The system test register value to be set.
 *
 * \return  None.
 *
 **/
void I2CMasterSetSysTest(uint32_t baseAddr, uint32_t sysTest);

/**
 * \brief   Get the I2C systest register.
 *
 * \param   baseAddr  It is the Memory address of the I2C instance used.
 *
 * \return  sysTest   The system test register value to be set.
 *
 **/
uint32_t I2CMasterGetSysTest(uint32_t baseAddr);

/**
 * \brief   Disables the I2C Module.This will put the I2C
 *          module in reset. Only Tx and Rx are cleared,status bits are set
 *          their default values and all configuration registers are not reset,
 *          they keep their initial values.
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  None.
 *
 */
void I2CMasterDisable(uint32_t baseAddr);

/**
 * \brief   This API determines whether bus is busy or not.
 *
 * \param   baseAddr    I2C base address.
 *
 * @return  returns 1 if bus is busy.
 *          returns 0 if bus is free.
 *
 */
int32_t I2CMasterBusBusy(uint32_t baseAddr);

/**
 * \brief   This API determines whether Master is busy or not.
 *
 * \param   baseAddr  I2C base address.
 *
 * @return  returns 1 if bus is busy.
 *          returns 0 if bus is free.
 *
 */
uint32_t I2CMasterBusy(uint32_t baseAddr);

/**
 * \brief   This API determines whether error occurred or not during
 *          I2C operation.
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns error status.If error has occurred it returns a non zero value.
 *          If no error has occurred then return status will be zero.
 *
 */
uint32_t I2CMasterErr(uint32_t baseAddr);

/**
 * \brief   This API configure I2C in different modes of
 *          operation.
 *
 * \param   baseAddr      I2C base address.
 * \param   cmd           It is the value which configures I2C in different
 *                        modes of operation.\n
 *
 *          cmd can take following macros.\n
 *
 *         I2C_CFG_MST_TX            - Configure'sI2C as Master-Transmitter.\n
 *         I2C_CFG_MST_RX             - Configurers I2C as Master-Receiver.\n
 *         I2C_CFG_STOP               - Configurers I2C to generate stop
 *                                       condition when DCOUNT counts down to
 *                                       zero.\n
 *         I2C_CFG_N0RMAL_MODE        - Configurers I2C in normal mode.\n
 *         I2C_CFG_SRT_BYTE_MODE      - Configurers I2C in start byte mode.\n
 *         I2C_CFG_7BIT_SLAVE_ADDR    - Configurers I2C to address seven bit
 *                                       addressed slave.\n
 *         I2C_CFG_10BIT_SLAVE_ADDR   - Configurers I2C to address ten bit
 *                                       addressed slave.\n
 *         I2C_CFG_10BIT_OWN_ADDR_0   - Enable 10bit addressing mode for own
 *                                       address 0.\n
 *         I2C_CFG_10BIT_OWN_ADDR_1   - Enable 10bit addressing mode for own
 *                                       address 1.\n
 *         I2C_CFG_10BIT_OWN_ADDR_2   - Enable 10bit addressing mode for own
 *                                       address 2.\n
 *         I2C_CFG_10BIT_OWN_ADDR_3   - Enable 10bit addressing mode for own
 *                                       address 3.\n
 *         I2C_CFG_7BIT_OWN_ADDR_0   -  Enable 7bit addressing mode for own
 *                                       address 0.\n
 *         I2C_CFG_7BIT_OWN_ADDR_1   -  Enable 7bit addressing mode for own
 *                                       address 1 .\n
 *         I2C_CFG_7BIT_OWN_ADDR_2   -  Enable 7bit addressing mode for own
 *                                       address 2.\n
 *         I2C_CFG_7BIT_OWN_ADDR_3   -  Enable 7bit addressing mode for own
 *                                       address 3.\n
 *
 * @return  None.
 *
 */
void I2CMasterControl(uint32_t baseAddr, uint32_t cmd);

/**
 * \brief   This API starts a I2C transaction on the bus.
 *          This API must be called after all the configuration for the i2c
 *          module is done and after bringing I2C out of local reset
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  None.
 *
 */
void I2CMasterStart(uint32_t baseAddr);

/**
 * \brief   This API stops a I2C transaction on the bus.
 *          This API must be used in case a deliberate STOP needs to be sent
 *          on the bus.
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  None.
 *
 */
void I2CMasterStop(uint32_t baseAddr);

/**
 * \brief   This API enables only specified I2C interrupts
 *          in master mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts that are required to be
 *                      enabled.\n
 *
 *          intFlag can take following values.\n
 *
 *         I2C_INT_ARBITRATION_LOST     - Arbitration-lost interrupt.\n
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers are ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_OVER_RUN        - Receive overrun interrupt.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @return     None.
 *
 */
void I2CMasterIntEnableEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API enables only specified I2C interrupts
 *          in Slave mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts that are required to be
 *                      enabled.\n
 *
 *          intFlag can take following values.\n
 *
 *         I2C_INT_ARBITRATION_LOST     - Arbitration-lost interrupt.\n
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers are ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_OVER_RUN        - Receive overrun interrupt.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @return     None.
 *
 */
void I2CSlaveIntEnableEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API disables only specified I2C interrupts
 *          in master mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts that are required to be
 *                      disabled\n
 *
 *          intFlag can take following values.\n
 *
 *         I2C_INT_ARBITRATION_LOST     - Arbitration-lost interrupt.\n
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers are ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_OVER_RUN        - Receive overrun interrupt.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @return  None.
 *
 */
void I2CMasterIntDisableEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API disables only specified I2C interrupts
 *          in Slave mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts that are required to be
 *                      disabled\n
 *
 *          intFlag can take following values.\n
 *
 *         I2C_INT_ARBITRATION_LOST     - Arbitration-lost interrupt.\n
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers are ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_OVER_RUN        - Receive overrun interrupt.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @return  None.
 *
 */
void I2CSlaveIntDisableEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API returns the status of  interrupts in
 *          master mode.
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns status of interrupts.
 *
 */
uint32_t I2CMasterIntStatus(uint32_t baseAddr);

/**
 * \brief   This API returns the status of interrupts in
 *          slave mode.
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns status of interrupts.
 *
 */
uint32_t I2CSlaveIntStatus(uint32_t baseAddr);

/**
 * \brief   This API returns the status of specified
 *          interrupts in master mode.
 *
 * \param   baseAddr     I2C base address.
 * \param   intFlag      It specifies the interrupts whose status needs to be
 *                       returned.\n
 *
 *          intFlag can take following macros.\n
 *
 *         I2C_INT_ARBITRATION_LOST     - Arbitration-lost interrupt.\n
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C register ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_OVER_RUN        - Receive overrun interrupt.\n
 *         I2C_INT_BUS_BUSY             - Bus busy.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @returns status of specified interrupts.
 *
 */
uint32_t I2CMasterIntStatusEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API returns the status of specified
 *          interrupts in slave mode.
 *
 * \param   baseAddr     I2C base address.
 * \param   intFlag      It specifies the interrupts whose status needs to be
 *                       returned.\n
 *
 *          intFlag can take following macros.\n
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers are ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_OVER_RUN        - Receive overrun interrupt.\n
 *         I2C_INT_BUS_BUSY             - Bus busy.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @returns status of specified interrupts.
 *
 */
uint32_t I2CSlaveIntStatusEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API returns the raw status of interrupts in
 *          master mode.
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns Raw status of interrupts.
 *
 */
uint32_t I2CMasterIntRawStatus(uint32_t baseAddr);

/**
 * \brief   This API returns the raw status of interrupts in
 *          slave mode.
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns Raw status of interrupts.
 *
 */
uint32_t I2CSlaveIntRawStatus(uint32_t baseAddr);

/**
 * \brief   This API returns the raw status of specified
 *          interrupts in master mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts whose raw status needs to
 *                      be returned.\n
 *
 *          intFlag can take following macros.\n
 *
 *         I2C_INT_ARBITRATION_LOST     - Arbitration-lost interrupt.\n
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers are ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_OVER_RUN        - Receive overrun interrupt.\n
 *         I2C_INT_BUS_BUSY             - Bus busy.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @returns status of specified interrupts.
 *
 */
uint32_t I2CMasterIntRawStatusEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API returns the raw status of specified
 *          interrupts in slave mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts whose raw status needs to
 *                      be returned.\n
 *
 *          intFlag can take following macros.\n
 *
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers are ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_OVER_RUN        - Receive overrun interrupt.\n
 *         I2C_INT_BUS_BUSY             - Bus busy.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @returns status of specified interrupts.
 *
 */
uint32_t I2CSlaveIntRawStatusEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API Clears the status of specified interrupts
 *          in master mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts whose status needs to be
 *                      cleared\n
 *
 *          intFlag can take following macros.\n
 *
 *         I2C_INT_ARBITRATION_LOST     - Arbitration-lost interrupt.\n
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @returns None
 *
 */
void I2CMasterIntClearEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API Clears the status of specified interrupts
 *          in Slave mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts whose status needs to be
 *                      cleared\n
 *
 *          intFlag can take following macros.\n
 *
 *         I2C_INT_ARBITRATION_LOST     - Arbitration-lost interrupt.\n
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @returns None
 *
 */
void I2CSlaveIntClearEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API Clears the raw status of
 *          specified interrupts in master mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts whose status needs to be
 *                      cleared\n
 *
 *          intFlag can take following macros.\n
 *
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers are ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *
 * @returns None
 *
 */
void I2CMasterIntRawStatusClearEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API Clears the raw status of
 *          specified interrupts in Slave mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts whose status needs to be
 *                      cleared\n
 *
 *          intFlag can take following macros.\n
 *
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C register are ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *
 * @returns None
 *
 */
void I2CSlaveIntRawStatusClearEx(uint32_t baseAddr, uint32_t intFlag);

/**
 * \brief   This API sets the address of the slave device
 *          with which I2C wants to communicate.
 *
 * \param   baseAddr   I2C base address.
 * \param   slaveAdd   slave address.
 *
 * @return     None.
 *
 */
void I2CMasterSlaveAddrSet(uint32_t baseAddr, uint32_t slaveAdd);

/**
 * \brief   This API returns the address of the slave device
 *          with which I2C wants to communicate.
 *
 * \param   baseAddr   I2C base address.
 * \param   slaveAdd   slave address.
 *
 * @return     slave address.
 *
 */
uint32_t I2CMasterSlaveAddrGet(uint32_t baseAddr, uint32_t slaveAdd);

/**
 * \brief   This API configure I2C data count register with a
 *          value. The value in the I2C data count register indicate how many
 *          data words to transfer when the I2C is configured as a
 *          master-transmitter and repeat mode is off.
 *
 * \param   baseAddr     I2C base address.
 * \param   count        It is value which is set to I2C data count register.
 *
 * @return  None.
 *
 */
void I2CSetDataCount(uint32_t baseAddr, uint32_t count);

/**
 * \brief   This API gets the number of bytes transferred over the
 *          I2C bus. The value in the I2C data count register indicate how many
 *          data words to transfer when the I2C is configured as a
 *          master-transmitter and repeat mode is off.
 *
 * \param   baseAddr   I2C base address.
 *
 * @return  number of bytes transferred over the I2C bus.
 *
 */
uint32_t I2CDataCountGet(uint32_t baseAddr);

/**
 * \brief   This API configures the threshold value for
 *          FIFO buffer.
 *
 * \param   baseAddr         I2C base address.
 * \param   thresholdVal     It is the threshold value for FIFO buffer.Which is
 *                           used to specify the trigger level for data transfer
 *                           or data receive transfers.
 * \param   flag             It specifies FIFO buffer in TX mode or RX mode.\n
 *
 *          flag can take following macros.\n
 *
 *         I2C_TX_MODE.\n
 *         I2C_RX_MODE.\n
 *
 * @return  None.
 *
 */
void I2CFIFOThresholdConfig(uint32_t baseAddr,
                            uint32_t thresholdVal,
                            uint32_t flag);

/**
 * \brief   This API clears Transmit and Receive FIFO.
 *
 * \param   baseAddr     I2C base address.
 * \param   flag         It specifies Transmit FIFO or Receive FIFO.\n
 *
 *          flag can take following macros.\n
 *
 *         I2C_TX_MODE - .\n
 *         I2C_RX_MODE - .\n
 *
 * @return  None.
 *
 */
void I2CFIFOClear(uint32_t baseAddr, uint32_t flag);

/**
 * \brief   This API Enables generation of Transmit Event,when
 *          I2C_DATA register is empty.
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  None.
 *
 */
void I2CDMATxEventEnable(uint32_t baseAddr);

/**
 * \brief   This API Enables generation of Receive DMA Event,
 *          when I2C_DATA register is full.
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  None.
 *
 */
void I2CDMARxEventEnable(uint32_t baseAddr);

/**
 * \brief   This API Disables Transmit event.
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  None.
 *
 */
void I2CDMATxEventDisable(uint32_t baseAddr);

/**
 * \brief   This API Disables Receive event.
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  None.
 *
 */
void I2CDMARxEventDisable(uint32_t baseAddr);

/**
 * \brief   This API returns the status of the internal buffers.
 *
 * \param   baseAddr   I2C base address.
 * \param   flag       It specifies required status field.\n
 *
 *         BufStatOp can take following macros.\n
 *
 *         I2C_TX_BUFFER_STATUS - Indicates the number of data bytes still.
 *                                  left to be written in TXFIFO\n
 *         I2C_RX_BUFFER_STATUS - Indicates the number of bytes to be
 *                                  transferred from the FIFO at the end
 *                                  of the I2C transfer.\n
 *         I2C_FIFO_DEPTH       - Internal FIFO buffer depth.\n
 *
 * @returns required status of internal buffer.
 *
 */
uint32_t I2CBufferStatus(uint32_t baseAddr, uint32_t flag);

/**
 * \brief   This API returns one of the module's four own
 *          addresses,which external master used when addressing the module.
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns one of the module's four own address the external master used
 *          when addressing the module. returns HSI2C_FAIL if failed to get
 *          own address
 *
 */
uint32_t I2CActiveOwnAddressGet(uint32_t baseAddr);

/**
 * \brief   This API blocks or unblocks the clock for any
 *          of the module's four own addresses.
 *
 * \param   baseAddr     I2C base address.
 *
 * \param   ownAdd0      It is value which blocks or unblocks clock to i2c if
 *                       an external master using own address 0.
 *
 * \param   ownAdd1      It is value which blocks or unblocks clock to i2c if
 *                       an external master using own address 1.
 *
 * \param   ownAdd2      It is value which blocks or unblocks clock to i2c if
 *                       an external master using own address 2.
 *
 * \param   ownAdd3      It is value which blocks or unblocks clock to i2c if
 *                       an external master using own address 0.
 *
 *                       0 - Unblocks clock to i2c
 *                       1 - Blocks clock to i2c
 * @returns None.
 *
 */
void I2CClockBlockingControl(uint32_t baseAddr,
                             uint32_t ownAdd0,
                             uint32_t ownAdd1,
                             uint32_t ownAdd2,
                             uint32_t ownAdd3);

/**
 * \brief   This API configures any one of the own address field
 *          out of four present in I2C controller.
 *
 * \param   baseAddr     I2C base address.
 * \param   ownAdd       Own address to be set.
 * \param   flag         It specifies the any one of the own address field
 *                       out of four.\n
 *
 *          flag can take following values.\n
 *
 *         I2C_OWN_ADDR_0.\n
 *         I2C_OWN_ADDR_1.\n
 *         I2C_OWN_ADDR_2.\n
 *         I2C_OWN_ADDR_3.\n
 *
 * @returns None.
 *
 */
void I2COwnAddressSet(uint32_t baseAddr,
                      uint32_t ownAdd,
                      uint32_t flag);

/**
 * \brief   This API reset the entire I2C module.On reset,are set to
 *          power up reset values.
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns None.
 *
 */
void I2CSoftReset(uint32_t baseAddr);

/**
 * \brief   This API enables auto idle mechanism
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns None.
 *
 */
void I2CAutoIdleEnable(uint32_t baseAddr);

/**
 * \brief   This API disables auto idle mechanism
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns None.
 *
 */
void I2CAutoIdleDisable(uint32_t baseAddr);

/**
 * \brief   This API enables wakeup mechanism
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns None.
 *
 */
void I2CGlobalWakeUpEnable(uint32_t baseAddr);

/**
 * \brief   This API disables wakeup mechanism
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns None.
 *
 */
void I2CGlobalWakeUpDisable(uint32_t baseAddr);

/**
 * \brief   This API selects one of the idle mode operation
 *          mechanism.
 *
 * \param   baseAddr     I2C base address.
 * \param   flag         It is the value which determines the type of idle
 *                       mode operation.\n
 *
 *          idleMode can take following values.\n
 *
 *         I2C_FORCE_IDLE_MODE          - selects forced idle mode operation.\n
 *         I2C_NO_IDLE_MODE             - selects no idle mode operation.\n
 *         I2C_SMART_IDLE_MODE          - selects smart idle mode operation.\n
 *         I2C_SMART_IDLE_WAKEUP_MODE   - selects smart idle wakeup mode
 *                                         of operation.\n
 *
 * @returns None.
 *
 */
void I2CIdleModeSelect(uint32_t baseAddr, uint32_t flag);

/**
 * \brief   This API disable external clock gating
 *          mechanism by selecting appropriate type of clock activity.
 *
 * \param   baseAddr   I2C base address.
 * \param   flag       It is the value which determines the type of clock
 *                     activity.\n
 *
 *          clkAct can take following values.\n
 *
 *         I2C_CUT_OFF_BOTH_CLK        - Both OCP and SYS CLK are cut off.\n
 *         I2C_CUT_OFF_SYS_CLK         - system clock is cut off;OCP clock
 *                                         is kept alive.\n
 *         I2C_CUT_OFF_OCP_CLK         - OCP clock is cut off;system clock is
 *                                         is kept alive\n
 *         I2C_KEEP_ALIVE_BOTH_CLK     - BOTH OCP and SYS CLK are kept alive.\n
 *
 * @returns None.
 *
 */
void I2CClockActivitySelect(uint32_t baseAddr, uint32_t flag);

/**
 * \brief   This API Enables a specific IRQ/DMA request source to
 *          generate an asynchronous wakeup signal.A wakeup is signalled to the
 *          local host if the corresponding event is captured by the core of
 *          the I2C controller.
 *
 * \param   baseAddr    I2C base address.
 * \param  eventFlag    It specifies for which event wakeup signal has to
 *                         Enable.\n
 *
 *         eventFlag can take following values.\n
 *
 *        I2C_WAKE_UP_ARBITRATION_LOST    - Arbitration-lost wakeup signal.\n
 *        I2C_WAKE_UP_NO_ACK              - No-acknowledgement wakeup signal.\n
 *        I2C_WAKE_UP_ADRR_READY_ACCESS   - No-acknowledgement wakeup signal.\n
 *        I2C_WAKE_UP_GENERAL_CALL        - General call wakeup signal.\n
 *        I2C_WAKE_UP_START               - Start condition wakeup signal.\n
 *        I2C_WAKE_UP_STOP_CONDITION      - Stop condition wakeup signal.\n
 *        I2C_WAKE_UP_ADRR_SLAVE          - Address-as-slave wakeup signal.\n
 *        I2C_WAKE_UP_TX_UNDER_FLOW       - Transmit under flow wakeup signal.\n
 *        I2C_WAKE_UP_RECV_OVER_RUN       - Receive overrun wakeup signal.\n
 *        I2C_WAKE_UP_RECV_DRAIN          - Receive drain wakeup signal.\n
 *        I2C_WAKE_UP_TRANSMIT_DRAIN      - Transmit drain wakeup signal.\n
 *        I2C_WAKE_UP_DATA_RECV_TX_RDY    - Receive-data-ready wakeup signal.\n
 *
 * \param  flag        It specifies IRQ or DMA Transmit or DMA receive
 *                     request source to generate asynchronous wake up
 *                     signal.\n
 *
 *         flag can take following values.\n
 *
 *        I2C_WAKE_UP_IRQ.\n
 *        I2C_WAKE_UP_DMA_RECV.\n
 *        I2C_WAKE_UP_DMA_TRANMIT.\n
 *
 * @returns None.
 *
 */
void I2CWakeUpEnable(uint32_t baseAddr,
                     uint32_t eventFlag,
                     uint32_t flag);

/**
 * \brief   This API Disables a specific IRQ/DMA request
 *         source to generate an asynchronous wakeup signal.A wakeup is
 *         signalled to the local host if the corresponding event is captured
 *         by the core of the I2C controller.
 *
 * \param   baseAddr    I2C base address.
 * \param  eventFlag    It specifies for which event wakeup signal has to
 *                         disabled.\n
 *
 *         intFlag can take following macros.\n
 *
 *        I2C_WAKE_UP_ARBITRATION_LOST    - Arbitration-lost wakeup signal.\n
 *        I2C_WAKE_UP_NO_ACK              - No-acknowledgement wakeup signal.\n
 *        I2C_WAKE_UP_ADRR_RDY_ACCESS     - No-acknowledgement wakeup signal.\n
 *        I2C_WAKE_UP_GENERAL_CALL        - General call wakeup signal.\n
 *        I2C_WAKE_UP_START               - Start condition wakeup signal.\n
 *        I2C_WAKE_UP_STOP_CONDITION      - Stop condition wakeup signal.\n
 *        I2C_WAKE_UP_ADRR_SLAVE          - Address-as-slave wakeup signal.\n
 *        I2C_WAKE_UP_TX_UNDER_FLOW       - Transmit under flow wakeup signal.\n
 *        I2C_WAKE_UP_RECV_OVER_RUN       - Receive overrun wakeup signal.\n
 *        I2C_WAKE_UP_RECV_DRAIN          - Receive drain wakeup signal.\n
 *        I2C_WAKE_UP_TRANSMIT_DRAIN      - Transmit drain wakeup signal.\n
 *        I2C_WAKE_UP_DATA_RECV_TX_RDY    - Receive-data-ready
 *                                              wakeup signal.\n
 *
 * \param  flag        It specifies IRQ or DMA Transmit or DMA receive
 *                     request source to generate asynchronous wake up
 *                     signal.\n
 *
 *         flag can take following values.\n
 *
 *        I2C_WAKE_UP_IRQ.\n
 *        I2C_WAKE_UP_DMA_RECV.\n
 *        I2C_WAKE_UP_DMA_TRANMIT.\n
 *
 * @returns None.
 *
 */
void I2CWakeUpDisable(uint32_t baseAddr,
                      uint32_t eventFlag,
                      uint32_t flag);

/**
 * \brief   This API indicates the state of the reset in case of
 *          hardware reset,global reset or partial reset.
 *
 * \param   baseAddr     I2C base address.
 *
 * @returns returns "1" if reset is completed.
 *          returns "0" if internal module reset is ongoing.
 *
 */
uint32_t I2CSystemStatusGet(uint32_t baseAddr);

/**
 * \brief   This API transmits a byte from the I2C in Master mode.
 *
 * \param   baseAddr    I2C base address.
 * \param   data       data to be transmitted from the I2C Master.
 *
 * @return  None.\n
 *
 */
void I2CMasterDataPut(uint32_t baseAddr, uint8_t data);

/**
 * \brief   This API Receives a byte that has been sent to the
 *         I2C in Master mode.
 *
 * \param   baseAddr     I2C base address.
 *
 * @return Returns the byte received from by the I2C in Master mode.
 *
 */
uint8_t I2CMasterDataGet(uint32_t baseAddr);

/**
 * \brief   This API transmits a byte from the I2C in Slave mode.
 *
 * \param   baseAddr  I2C base address.
 * \param  data       data to be transmitted from the I2C in Slave mode.
 *
 * @return None.
 *
 */
void I2CSlaveDataPut(uint32_t baseAddr, uint32_t data);

/**
 * \brief   This API Receives a byte that has been sent to the
 *          I2C in Slave mode.
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  Returns the byte received from by the I2C in Slave mode.
 *
 */
uint8_t I2CSlaveDataGet(uint32_t baseAddr);

/**
 * \brief  This function configures SYSC register
 *
 * \param  baseAddr    I2C base address.
 * \param  syscFlag    value of sysc register.
 *
 */
void I2CSyscInit(uint32_t baseAddr, uint32_t syscFlag);

/**
 * \brief   This API configures the I2C operation mode(F/S or HS),
 *          slave address 7bit or 10bit, own address 7bit or 10bit and
 *          start byte mode or normal mode of operation
 *
 * \param   baseAddr     I2C base address.
 * \param   conParams    Is the structure defining the configure parameters.
 *
 * @see     struct       I2CconParams_s
 *
 */
void I2CConfig(uint32_t baseAddr, uint32_t conParams);

/**
 * \brief   This clears the I2C TX and RX FIFO
 *
 * \param   baseAddr     I2C base address.
 *
 * @return  None.
 *
 */
void I2CFlushFifo(uint32_t baseAddr);

/**
 * \brief   This API gets the status of enabled
 *          interrupt for the interrupt flag passed
 *
 * \param   baseAddr    I2C base address.
 * \param   intFlag     It specifies the interrupts that are required to be
 *                      enabled.\n
 *
 *          intFlag can take following values.\n
 *
 *         I2C_INT_ARBITRATION_LOST     - Arbitration-lost interrupt.\n
 *         I2C_INT_NO_ACK               - No-acknowledgement interrupt.\n
 *         I2C_INT_ADRR_READY_ACESS     - I2C registers are ready to access.\n
 *         I2C_INT_RECV_READY           - Receive-data-ready interrupt.\n
 *         I2C_INT_TRANSMIT_READY       - Transmit-data-ready interrupt.\n
 *         I2C_INT_GENERAL_CALL         - General call interrupt.\n
 *         I2C_INT_START                - Start condition interrupt.\n
 *         I2C_INT_ACCESS_ERROR         - Access error interrupt.\n
 *         I2C_INT_STOP_CONDITION       - Stop condition interrupt.\n
 *         I2C_INT_ADRR_SLAVE           - Address-as-slave interrupt.\n
 *         I2C_INT_TRANSMIT_UNDER_FLOW  - Transmit under flow interrupt.\n
 *         I2C_INT_RECV_OVER_RUN        - Receive overrun interrupt.\n
 *         I2C_INT_RECV_DRAIN           - Receive drain interrupt.\n
 *         I2C_INT_TRANSMIT_DRAIN       - Transmit drain interrupt.\n
 *
 * @returns status of specified interrupts
 *
 */
uint32_t I2CGetEnabledIntStatus(uint32_t baseAddr, uint32_t intFlag);

#ifdef __cplusplus
}
#endif

#endif
 /** @} */
/********************************* End of file ******************************/
