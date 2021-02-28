/*
* hw_pwmss.h
*
* Register-level header file for PWMSS
*
* Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
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

#ifndef HW_PWMSS_H_
#define HW_PWMSS_H_

#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
* Register Definitions
****************************************************************************************************/

#define PWMSS_CLKSTATUS                                             (0xcU)
#define PWMSS_CLKCONFIG                                             (0x8U)
#define PWMSS_SYSCONFIG                                             (0x4U)
#define PWMSS_IDVER                                                 (0x0U)

/****************************************************************************************************
* Field Definition Macros
****************************************************************************************************/

#define PWMSS_CLKSTATUS_ECAP_CLK_EN_ACK_SHIFT                                           (0U)
#define PWMSS_CLKSTATUS_ECAP_CLK_EN_ACK_MASK                                            (0x00000001U)

#define PWMSS_CLKSTATUS_ECAP_CLKSTOP_ACK_SHIFT                                          (1U)
#define PWMSS_CLKSTATUS_ECAP_CLKSTOP_ACK_MASK                                           (0x00000002U)

#define PWMSS_CLKSTATUS_EQEP_CLK_EN_ACK_SHIFT                                           (4U)
#define PWMSS_CLKSTATUS_EQEP_CLK_EN_ACK_MASK                                            (0x00000010U)

#define PWMSS_CLKSTATUS_EQEP_CLKSTOP_ACK_SHIFT                                          (5U)
#define PWMSS_CLKSTATUS_EQEP_CLKSTOP_ACK_MASK                                           (0x00000020U)

#define PWMSS_CLKSTATUS_EPWM_CLK_EN_ACK_SHIFT                                           (8U)
#define PWMSS_CLKSTATUS_EPWM_CLK_EN_ACK_MASK                                            (0x00000100U)

#define PWMSS_CLKSTATUS_EPWM_CLKSTOP_ACK_SHIFT                                          (9U)
#define PWMSS_CLKSTATUS_EPWM_CLKSTOP_ACK_MASK                                           (0x00000200U)

#define PWMSS_CLKCONFIG_ECAPCLK_EN_SHIFT                                                (0U)
#define PWMSS_CLKCONFIG_ECAPCLK_EN_MASK                                                 (0x00000001U)

#define PWMSS_CLKCONFIG_ECAPCLKSTOP_REQ_SHIFT                                           (1U)
#define PWMSS_CLKCONFIG_ECAPCLKSTOP_REQ_MASK                                            (0x00000002U)

#define PWMSS_CLKCONFIG_EQEPCLK_EN_SHIFT                                                (4U)
#define PWMSS_CLKCONFIG_EQEPCLK_EN_MASK                                                 (0x00000010U)

#define PWMSS_CLKCONFIG_EQEPCLKSTOP_REQ_SHIFT                                           (5U)
#define PWMSS_CLKCONFIG_EQEPCLKSTOP_REQ_MASK                                            (0x00000020U)

#define PWMSS_CLKCONFIG_EPWMCLK_EN_SHIFT                                                (8U)
#define PWMSS_CLKCONFIG_EPWMCLK_EN_MASK                                                 (0x00000100U)

#define PWMSS_CLKCONFIG_EPWMCLKSTOP_REQ_SHIFT                                           (9U)
#define PWMSS_CLKCONFIG_EPWMCLKSTOP_REQ_MASK                                            (0x00000200U)

#define PWMSS_SYSCONFIG_SOFTRESET_SHIFT                                                 (0U)
#define PWMSS_SYSCONFIG_SOFTRESET_MASK                                                  (0x00000001U)

#define PWMSS_SYSCONFIG_FREEEMU_SHIFT                                                   (1U)
#define PWMSS_SYSCONFIG_FREEEMU_MASK                                                    (0x00000002U)
#define PWMSS_SYSCONFIG_FREEEMU_SENSITIVE_TO_SUSPEND                                     (0U)
#define PWMSS_SYSCONFIG_FREEEMU_NOT_SENSITIVE_TO_SUSPEND                                 (1U)

#define PWMSS_SYSCONFIG_IDLEMODE_SHIFT                                                  (2U)
#define PWMSS_SYSCONFIG_IDLEMODE_MASK                                                   (0x0000000cU)
#define PWMSS_SYSCONFIG_IDLEMODE_NO_IDLE                                                 (1U)
#define PWMSS_SYSCONFIG_IDLEMODE_FORCE_IDLE                                              (0U)
#define PWMSS_SYSCONFIG_IDLEMODE_SMART_IDLE                                              (2U)

#define PWMSS_SYSCONFIG_STANDBYMODE_SHIFT                                               (4U)
#define PWMSS_SYSCONFIG_STANDBYMODE_MASK                                                (0x00000030U)
#define PWMSS_SYSCONFIG_STANDBYMODE_NO_STANDBY                                           (1U)
#define PWMSS_SYSCONFIG_STANDBYMODE_SMART_STANDBY                                        (2U)
#define PWMSS_SYSCONFIG_STANDBYMODE_FORCE_STANDBY                                        (0U)

#define PWMSS_IDVER_Y_MINOR_SHIFT                                                       (0U)
#define PWMSS_IDVER_Y_MINOR_MASK                                                        (0x0000003fU)

#define PWMSS_IDVER_CUSTOM_SHIFT                                                        (6U)
#define PWMSS_IDVER_CUSTOM_MASK                                                         (0x000000c0U)

#define PWMSS_IDVER_X_MAJOR_SHIFT                                                       (8U)
#define PWMSS_IDVER_X_MAJOR_MASK                                                        (0x00000700U)

#define PWMSS_IDVER_R_RTL_SHIFT                                                         (11U)
#define PWMSS_IDVER_R_RTL_MASK                                                          (0x0000f800U)

#define PWMSS_IDVER_FUNC_SHIFT                                                          (16U)
#define PWMSS_IDVER_FUNC_MASK                                                           (0x0fff0000U)

#define PWMSS_IDVER_SCHEME_SHIFT                                                        (30U)
#define PWMSS_IDVER_SCHEME_MASK                                                         (0xc0000000U)

#ifdef __cplusplus
}
#endif
#endif  /* HW_PWMSS_H_ */