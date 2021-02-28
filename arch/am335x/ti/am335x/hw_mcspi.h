/*
* hw_mcspi.h
*
* Register-level header file for MCSPI
*
* Copyright (C) 2013 - 2019 Texas Instruments Incorporated - http://www.ti.com/
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

#ifndef HW_MCSPI_H_
#define HW_MCSPI_H_

#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
* Register Definitions
****************************************************************************************************/

#define MCSPI_HL_REV                                                (0x0U)
#define MCSPI_HL_HWINFO                                             (0x4U)
#define MCSPI_HL_SYSCONFIG                                          (0x10U)
#define MCSPI_REVISION                                              (0x100U)
#define MCSPI_SYSCONFIG                                             (0x110U)
#define MCSPI_SYSSTS                                                (0x114U)
#define MCSPI_IRQSTS                                                (0x118U)
#define MCSPI_IRQEN                                                 (0x11cU)
#define MCSPI_WAKEUPEN                                              (0x120U)
#define MCSPI_SYST                                                  (0x124U)
#define MCSPI_MODULCTRL                                             (0x128U)
#define MCSPI_CHCONF(m)                                             (0x12cU + ((m) * 0x14U))
#define MCSPI_CHCONF_NUM_ELEMS                                      (4U)
#define MCSPI_CHSTAT(m)                                             (0x130U + ((m) * 0x14U))
#define MCSPI_CHSTAT_NUM_ELEMS                                      (4U)
#define MCSPI_CHCTRL(m)                                             (0x134U + ((m) * 0x14U))
#define MCSPI_CHCTRL_NUM_ELEMS                                      (4U)
#define MCSPI_TX(m)                                                 (0x138U + ((m) * 0x14U))
#define MCSPI_TX_NUM_ELEMS                                          (4U)
#define MCSPI_RX(m)                                                 (0x13cU + ((m) * 0x14U))
#define MCSPI_RX_NUM_ELEMS                                          (4U)
#define MCSPI_XFERLEVEL                                             (0x17cU)
#define MCSPI_DAFTX                                                 (0x180U)
#define MCSPI_DAFRX                                                 (0x1a0U)

/****************************************************************************************************
* Field Definition Macros
****************************************************************************************************/

#define MCSPI_HL_REV_FUNC_SHIFT                                                         (16U)
#define MCSPI_HL_REV_FUNC_MASK                                                          (0x0fff0000U)

#define MCSPI_HL_REV_SCHEME_SHIFT                                                       (30U)
#define MCSPI_HL_REV_SCHEME_MASK                                                        (0xc0000000U)
#define MCSPI_HL_REV_SCHEME_HIGHLANDER                                                   (1U)
#define MCSPI_HL_REV_SCHEME_LEGACY                                                       (0U)

#define MCSPI_HL_REV_CUSTOM_SHIFT                                                       (6U)
#define MCSPI_HL_REV_CUSTOM_MASK                                                        (0x000000c0U)
#define MCSPI_HL_REV_CUSTOM_READ0                                                        (0U)

#define MCSPI_HL_REV_X_MAJOR_SHIFT                                                      (8U)
#define MCSPI_HL_REV_X_MAJOR_MASK                                                       (0x00000700U)

#define MCSPI_HL_REV_Y_MINOR_SHIFT                                                      (0U)
#define MCSPI_HL_REV_Y_MINOR_MASK                                                       (0x0000003fU)

#define MCSPI_HL_REV_R_RTL_SHIFT                                                        (11U)
#define MCSPI_HL_REV_R_RTL_MASK                                                         (0x0000f800U)

#define MCSPI_HL_REV_RSVD_SHIFT                                                         (28U)
#define MCSPI_HL_REV_RSVD_MASK                                                          (0x30000000U)

#define MCSPI_HL_HWINFO_USEFIFO_SHIFT                                                   (0U)
#define MCSPI_HL_HWINFO_USEFIFO_MASK                                                    (0x00000001U)
#define MCSPI_HL_HWINFO_USEFIFO_FIFOEN                                                   (1U)
#define MCSPI_HL_HWINFO_USEFIFO_NOFIFO                                                   (0U)

#define MCSPI_HL_HWINFO_FFNBYTE_SHIFT                                                   (1U)
#define MCSPI_HL_HWINFO_FFNBYTE_MASK                                                    (0x0000003eU)
#define MCSPI_HL_HWINFO_FFNBYTE_FF16BYTES                                                (1U)
#define MCSPI_HL_HWINFO_FFNBYTE_FF32BYTES                                                (2U)
#define MCSPI_HL_HWINFO_FFNBYTE_FF64BYTES                                                (4U)
#define MCSPI_HL_HWINFO_FFNBYTE_FF128BYTES                                               (8U)
#define MCSPI_HL_HWINFO_FFNBYTE_FF256BYTES                                               (16U)

#define MCSPI_HL_HWINFO_RSVD_SHIFT                                                      (7U)
#define MCSPI_HL_HWINFO_RSVD_MASK                                                       (0xffffff80U)

#define MCSPI_HL_HWINFO_RETMODE_SHIFT                                                   (6U)
#define MCSPI_HL_HWINFO_RETMODE_MASK                                                    (0x00000040U)
#define MCSPI_HL_HWINFO_RETMODE_NORETMODE                                                (0U)
#define MCSPI_HL_HWINFO_RETMODE_RETMODEEN                                                (1U)

#define MCSPI_HL_SYSCONFIG_IDLEMODE_SHIFT                                               (2U)
#define MCSPI_HL_SYSCONFIG_IDLEMODE_MASK                                                (0x0000000cU)
#define MCSPI_HL_SYSCONFIG_IDLEMODE_SMARTIDLEWAKEUP                                      (3U)
#define MCSPI_HL_SYSCONFIG_IDLEMODE_FORCEIDLE                                            (0U)
#define MCSPI_HL_SYSCONFIG_IDLEMODE_NOIDLE                                               (1U)
#define MCSPI_HL_SYSCONFIG_IDLEMODE_SMARTIDLE                                            (2U)

#define MCSPI_HL_SYSCONFIG_RSVD_SHIFT                                                   (4U)
#define MCSPI_HL_SYSCONFIG_RSVD_MASK                                                    (0xfffffff0U)

#define MCSPI_HL_SYSCONFIG_SOFTRESET_SHIFT                                              (0U)
#define MCSPI_HL_SYSCONFIG_SOFTRESET_MASK                                               (0x00000001U)
#define MCSPI_HL_SYSCONFIG_SOFTRESET_RESETDONE                                           (0U)
#define MCSPI_HL_SYSCONFIG_SOFTRESET_NOACTION                                            (0U)
#define MCSPI_HL_SYSCONFIG_SOFTRESET                                                     (1U)
#define MCSPI_HL_SYSCONFIG_SOFTRESET_RESETONGOING                                        (1U)

#define MCSPI_HL_SYSCONFIG_FREEEMU_SHIFT                                                (1U)
#define MCSPI_HL_SYSCONFIG_FREEEMU_MASK                                                 (0x00000002U)
#define MCSPI_HL_SYSCONFIG_FREEEMU_EMUDIS                                                (1U)
#define MCSPI_HL_SYSCONFIG_FREEEMU_EMUEN                                                 (0U)

#define MCSPI_REVISION_REV_SHIFT                                                        (0U)
#define MCSPI_REVISION_REV_MASK                                                         (0x000000ffU)

#define MCSPI_SYSCONFIG_SIDLEMODE_SHIFT                                                 (3U)
#define MCSPI_SYSCONFIG_SIDLEMODE_MASK                                                  (0x00000018U)
#define MCSPI_SYSCONFIG_SIDLEMODE_FORCE                                                  (0U)
#define MCSPI_SYSCONFIG_SIDLEMODE_NO                                                     (1U)
#define MCSPI_SYSCONFIG_SIDLEMODE_SMART                                                  (2U)
#define MCSPI_SYSCONFIG_SIDLEMODE_SMART_IDLE_WAKEUP                                      (3U)

#define MCSPI_SYSCONFIG_SOFTRESET_SHIFT                                                 (1U)
#define MCSPI_SYSCONFIG_SOFTRESET_MASK                                                  (0x00000002U)
#define MCSPI_SYSCONFIG_SOFTRESET_OFF                                                    (0U)
#define MCSPI_SYSCONFIG_SOFTRESET_ON                                                     (1U)

#define MCSPI_SYSCONFIG_CLOCKACTIVITY_SHIFT                                             (8U)
#define MCSPI_SYSCONFIG_CLOCKACTIVITY_MASK                                              (0x00000300U)
#define MCSPI_SYSCONFIG_CLOCKACTIVITY_NONE                                               (0U)
#define MCSPI_SYSCONFIG_CLOCKACTIVITY_OCP                                                (1U)
#define MCSPI_SYSCONFIG_CLOCKACTIVITY_FUNC                                               (2U)
#define MCSPI_SYSCONFIG_CLOCKACTIVITY_BOTH                                               (3U)

#define MCSPI_SYSCONFIG_AUTOIDLE_SHIFT                                                  (0U)
#define MCSPI_SYSCONFIG_AUTOIDLE_MASK                                                   (0x00000001U)
#define MCSPI_SYSCONFIG_AUTOIDLE_OFF                                                     (0U)
#define MCSPI_SYSCONFIG_AUTOIDLE_ON                                                      (1U)

#define MCSPI_SYSCONFIG_ENAWAKEUP_SHIFT                                                 (2U)
#define MCSPI_SYSCONFIG_ENAWAKEUP_MASK                                                  (0x00000004U)
#define MCSPI_SYSCONFIG_ENAWAKEUP_NOWAKEUP                                               (0U)
#define MCSPI_SYSCONFIG_ENAWAKEUP_ON                                                     (1U)

#define MCSPI_SYSSTS_RESETDONE_SHIFT                                                    (0U)
#define MCSPI_SYSSTS_RESETDONE_MASK                                                     (0x00000001U)
#define MCSPI_SYSSTS_RESETDONE_INPROGRESS                                                (0U)
#define MCSPI_SYSSTS_RESETDONE_COMPLETED                                                 (1U)

#define MCSPI_IRQSTS_RX3_FULL_SHIFT                                                     (14U)
#define MCSPI_IRQSTS_RX3_FULL_MASK                                                      (0x00004000U)
#define MCSPI_IRQSTS_RX3_FULL_NOEVNT_R                                                   (0U)
#define MCSPI_IRQSTS_RX3_FULL_EVNT_R                                                     (1U)
#define MCSPI_IRQSTS_RX3_FULL_NOEFFECT_W                                                 (0U)
#define MCSPI_IRQSTS_RX3_FULL_CLEARSRC_W                                                 (1U)

#define MCSPI_IRQSTS_WKS_SHIFT                                                          (16U)
#define MCSPI_IRQSTS_WKS_MASK                                                           (0x00010000U)
#define MCSPI_IRQSTS_WKS_NOEVNT_R                                                        (0U)
#define MCSPI_IRQSTS_WKS_EVNT_R                                                          (1U)
#define MCSPI_IRQSTS_WKS_NOEFFECT_W                                                      (0U)
#define MCSPI_IRQSTS_WKS_CLEARSRC_W                                                      (1U)

#define MCSPI_IRQSTS_RX2_FULL_SHIFT                                                     (10U)
#define MCSPI_IRQSTS_RX2_FULL_MASK                                                      (0x00000400U)
#define MCSPI_IRQSTS_RX2_FULL_NOEVNT_R                                                   (0U)
#define MCSPI_IRQSTS_RX2_FULL_EVNT_R                                                     (1U)
#define MCSPI_IRQSTS_RX2_FULL_NOEFFECT_W                                                 (0U)
#define MCSPI_IRQSTS_RX2_FULL_CLEARSRC_W                                                 (1U)

#define MCSPI_IRQSTS_TX3_EMPTY_SHIFT                                                    (12U)
#define MCSPI_IRQSTS_TX3_EMPTY_MASK                                                     (0x00001000U)
#define MCSPI_IRQSTS_TX3_EMPTY_NOEVNT_R                                                  (0U)
#define MCSPI_IRQSTS_TX3_EMPTY_EVNT_R                                                    (1U)
#define MCSPI_IRQSTS_TX3_EMPTY_NOEFFECT_W                                                (0U)
#define MCSPI_IRQSTS_TX3_EMPTY_CLEARSRC_W                                                (1U)

#define MCSPI_IRQSTS_RX1_FULL_SHIFT                                                     (6U)
#define MCSPI_IRQSTS_RX1_FULL_MASK                                                      (0x00000040U)
#define MCSPI_IRQSTS_RX1_FULL_NOEVNT_R                                                   (0U)
#define MCSPI_IRQSTS_RX1_FULL_EVNT_R                                                     (1U)
#define MCSPI_IRQSTS_RX1_FULL_NOEFFECT_W                                                 (0U)
#define MCSPI_IRQSTS_RX1_FULL_CLEARSRC_W                                                 (1U)

#define MCSPI_IRQSTS_TX3_UNDERFLOW_SHIFT                                                (13U)
#define MCSPI_IRQSTS_TX3_UNDERFLOW_MASK                                                 (0x00002000U)
#define MCSPI_IRQSTS_TX3_UNDERFLOW_NOEVNT_R                                              (0U)
#define MCSPI_IRQSTS_TX3_UNDERFLOW_EVNT_R                                                (1U)
#define MCSPI_IRQSTS_TX3_UNDERFLOW_NOEFFECT_W                                            (0U)
#define MCSPI_IRQSTS_TX3_UNDERFLOW_CLEARSRC_W                                            (1U)

#define MCSPI_IRQSTS_TX2_EMPTY_SHIFT                                                    (8U)
#define MCSPI_IRQSTS_TX2_EMPTY_MASK                                                     (0x00000100U)
#define MCSPI_IRQSTS_TX2_EMPTY_NOEVNT_R                                                  (0U)
#define MCSPI_IRQSTS_TX2_EMPTY_EVNT_R                                                    (1U)
#define MCSPI_IRQSTS_TX2_EMPTY_NOEFFECT_W                                                (0U)
#define MCSPI_IRQSTS_TX2_EMPTY_CLEARSRC_W                                                (1U)

#define MCSPI_IRQSTS_TX2_UNDERFLOW_SHIFT                                                (9U)
#define MCSPI_IRQSTS_TX2_UNDERFLOW_MASK                                                 (0x00000200U)
#define MCSPI_IRQSTS_TX2_UNDERFLOW_NOEVNT_R                                              (0U)
#define MCSPI_IRQSTS_TX2_UNDERFLOW_EVNT_R                                                (1U)
#define MCSPI_IRQSTS_TX2_UNDERFLOW_NOEFFECT_W                                            (0U)
#define MCSPI_IRQSTS_TX2_UNDERFLOW_CLEARSRC_W                                            (1U)

#define MCSPI_IRQSTS_TX1_EMPTY_SHIFT                                                    (4U)
#define MCSPI_IRQSTS_TX1_EMPTY_MASK                                                     (0x00000010U)
#define MCSPI_IRQSTS_TX1_EMPTY_NOEVNT_R                                                  (0U)
#define MCSPI_IRQSTS_TX1_EMPTY_EVNT_R                                                    (1U)
#define MCSPI_IRQSTS_TX1_EMPTY_NOEFFECT_W                                                (0U)
#define MCSPI_IRQSTS_TX1_EMPTY_CLEARSRC_W                                                (1U)

#define MCSPI_IRQSTS_TX0_UNDERFLOW_SHIFT                                                (1U)
#define MCSPI_IRQSTS_TX0_UNDERFLOW_MASK                                                 (0x00000002U)
#define MCSPI_IRQSTS_TX0_UNDERFLOW_NOEVNT_R                                              (0U)
#define MCSPI_IRQSTS_TX0_UNDERFLOW_EVNT_R                                                (1U)
#define MCSPI_IRQSTS_TX0_UNDERFLOW_NOEFFECT_W                                            (0U)
#define MCSPI_IRQSTS_TX0_UNDERFLOW_CLEARSRC_W                                            (1U)

#define MCSPI_IRQSTS_TX1_UNDERFLOW_SHIFT                                                (5U)
#define MCSPI_IRQSTS_TX1_UNDERFLOW_MASK                                                 (0x00000020U)
#define MCSPI_IRQSTS_TX1_UNDERFLOW_NOEVNT_R                                              (0U)
#define MCSPI_IRQSTS_TX1_UNDERFLOW_EVNT_R                                                (1U)
#define MCSPI_IRQSTS_TX1_UNDERFLOW_NOEFFECT_W                                            (0U)
#define MCSPI_IRQSTS_TX1_UNDERFLOW_CLEARSRC_W                                            (1U)

#define MCSPI_IRQSTS_RX0_FULL_SHIFT                                                     (2U)
#define MCSPI_IRQSTS_RX0_FULL_MASK                                                      (0x00000004U)
#define MCSPI_IRQSTS_RX0_FULL_NOEVNT_R                                                   (0U)
#define MCSPI_IRQSTS_RX0_FULL_EVNT_R                                                     (1U)
#define MCSPI_IRQSTS_RX0_FULL_NOEFFECT_W                                                 (0U)
#define MCSPI_IRQSTS_RX0_FULL_CLEARSRC_W                                                 (1U)

#define MCSPI_IRQSTS_RX0_OVERFLOW_SHIFT                                                 (3U)
#define MCSPI_IRQSTS_RX0_OVERFLOW_MASK                                                  (0x00000008U)
#define MCSPI_IRQSTS_RX0_OVERFLOW_NOEVNT_R                                               (0U)
#define MCSPI_IRQSTS_RX0_OVERFLOW_EVNT_R                                                 (1U)
#define MCSPI_IRQSTS_RX0_OVERFLOW_NOEFFECT_W                                             (0U)
#define MCSPI_IRQSTS_RX0_OVERFLOW_CLEARSRC_W                                             (1U)

#define MCSPI_IRQSTS_TX0_EMPTY_SHIFT                                                    (0U)
#define MCSPI_IRQSTS_TX0_EMPTY_MASK                                                     (0x00000001U)
#define MCSPI_IRQSTS_TX0_EMPTY_NOEVNT_R                                                  (0U)
#define MCSPI_IRQSTS_TX0_EMPTY_EVNT_R                                                    (1U)
#define MCSPI_IRQSTS_TX0_EMPTY_NOEFFECT_W                                                (0U)
#define MCSPI_IRQSTS_TX0_EMPTY_CLEARSRC_W                                                (1U)

#define MCSPI_IRQSTS_EOW_SHIFT                                                          (17U)
#define MCSPI_IRQSTS_EOW_MASK                                                           (0x00020000U)
#define MCSPI_IRQSTS_EOW_NOEVNT_R                                                        (0U)
#define MCSPI_IRQSTS_EOW_EVNT_R                                                          (1U)
#define MCSPI_IRQSTS_EOW_CLEARSRC_W                                                      (1U)
#define MCSPI_IRQSTS_EOW_NOEFFECT_W                                                      (0U)

#define MCSPI_IRQEN_TX2_UNDERFLOW_EN_SHIFT                                              (9U)
#define MCSPI_IRQEN_TX2_UNDERFLOW_EN_MASK                                               (0x00000200U)
#define MCSPI_IRQEN_TX2_UNDERFLOW_EN_IRQDISABLED                                         (0U)
#define MCSPI_IRQEN_TX2_UNDERFLOW_EN_IRQENABLED                                          (1U)

#define MCSPI_IRQEN_TX0_EMPTY_EN_SHIFT                                                  (0U)
#define MCSPI_IRQEN_TX0_EMPTY_EN_MASK                                                   (0x00000001U)
#define MCSPI_IRQEN_TX0_EMPTY_EN_IRQDISABLED                                             (0U)
#define MCSPI_IRQEN_TX0_EMPTY_EN_IRQENABLED                                              (1U)

#define MCSPI_IRQEN_RX0_FULL_EN_SHIFT                                                   (2U)
#define MCSPI_IRQEN_RX0_FULL_EN_MASK                                                    (0x00000004U)
#define MCSPI_IRQEN_RX0_FULL_EN_IRQDISABLED                                              (0U)
#define MCSPI_IRQEN_RX0_FULL_EN_IRQENABLED                                               (1U)

#define MCSPI_IRQEN_TX1_EMPTY_EN_SHIFT                                                  (4U)
#define MCSPI_IRQEN_TX1_EMPTY_EN_MASK                                                   (0x00000010U)
#define MCSPI_IRQEN_TX1_EMPTY_EN_IRQDISABLED                                             (0U)
#define MCSPI_IRQEN_TX1_EMPTY_EN_IRQENABLED                                              (1U)

#define MCSPI_IRQEN_RX1_FULL_EN_SHIFT                                                   (6U)
#define MCSPI_IRQEN_RX1_FULL_EN_MASK                                                    (0x00000040U)
#define MCSPI_IRQEN_RX1_FULL_EN_IRQDISABLED                                              (0U)
#define MCSPI_IRQEN_RX1_FULL_EN_IRQENABLED                                               (1U)

#define MCSPI_IRQEN_TX3_EMPTY_EN_SHIFT                                                  (12U)
#define MCSPI_IRQEN_TX3_EMPTY_EN_MASK                                                   (0x00001000U)
#define MCSPI_IRQEN_TX3_EMPTY_EN_IRQDISABLED                                             (0U)
#define MCSPI_IRQEN_TX3_EMPTY_EN_IRQENABLED                                              (1U)

#define MCSPI_IRQEN_TX0_UNDERFLOW_EN_SHIFT                                              (1U)
#define MCSPI_IRQEN_TX0_UNDERFLOW_EN_MASK                                               (0x00000002U)
#define MCSPI_IRQEN_TX0_UNDERFLOW_EN_IRQDISABLED                                         (0U)
#define MCSPI_IRQEN_TX0_UNDERFLOW_EN_IRQENABLED                                          (1U)

#define MCSPI_IRQEN_RX0_OVERFLOW_EN_SHIFT                                               (3U)
#define MCSPI_IRQEN_RX0_OVERFLOW_EN_MASK                                                (0x00000008U)
#define MCSPI_IRQEN_RX0_OVERFLOW_EN_IRQDISABLED                                          (0U)
#define MCSPI_IRQEN_RX0_OVERFLOW_EN_IRQENABLED                                           (1U)

#define MCSPI_IRQEN_RX3_FULL_EN_SHIFT                                                   (14U)
#define MCSPI_IRQEN_RX3_FULL_EN_MASK                                                    (0x00004000U)
#define MCSPI_IRQEN_RX3_FULL_EN_IRQDISABLED                                              (0U)
#define MCSPI_IRQEN_RX3_FULL_EN_IRQENABLED                                               (1U)

#define MCSPI_IRQEN_WKE_SHIFT                                                           (16U)
#define MCSPI_IRQEN_WKE_MASK                                                            (0x00010000U)
#define MCSPI_IRQEN_WKE_IRQDISABLED                                                      (0U)
#define MCSPI_IRQEN_WKE_IRQENABLED                                                       (1U)

#define MCSPI_IRQEN_TX2_EMPTY_EN_SHIFT                                                  (8U)
#define MCSPI_IRQEN_TX2_EMPTY_EN_MASK                                                   (0x00000100U)
#define MCSPI_IRQEN_TX2_EMPTY_EN_IRQDISABLED                                             (0U)
#define MCSPI_IRQEN_TX2_EMPTY_EN_IRQENABLED                                              (1U)

#define MCSPI_IRQEN_RX2_FULL_EN_SHIFT                                                   (10U)
#define MCSPI_IRQEN_RX2_FULL_EN_MASK                                                    (0x00000400U)
#define MCSPI_IRQEN_RX2_FULL_EN_IRQDISABLED                                              (0U)
#define MCSPI_IRQEN_RX2_FULL_EN_IRQENABLED                                               (1U)

#define MCSPI_IRQEN_TX3_UNDERFLOW_EN_SHIFT                                              (13U)
#define MCSPI_IRQEN_TX3_UNDERFLOW_EN_MASK                                               (0x00002000U)
#define MCSPI_IRQEN_TX3_UNDERFLOW_EN_IRQDISABLED                                         (0U)
#define MCSPI_IRQEN_TX3_UNDERFLOW_EN_IRQENABLED                                          (1U)

#define MCSPI_IRQEN_TX1_UNDERFLOW_EN_SHIFT                                              (5U)
#define MCSPI_IRQEN_TX1_UNDERFLOW_EN_MASK                                               (0x00000020U)
#define MCSPI_IRQEN_TX1_UNDERFLOW_EN_IRQDISABLED                                         (0U)
#define MCSPI_IRQEN_TX1_UNDERFLOW_EN_IRQENABLED                                          (1U)

#define MCSPI_IRQEN_EOW_EN_SHIFT                                                        (17U)
#define MCSPI_IRQEN_EOW_EN_MASK                                                         (0x00020000U)
#define MCSPI_IRQEN_EOW_EN_IRQENABLED                                                    (1U)
#define MCSPI_IRQEN_EOW_EN_IRQDISABLED                                                   (0U)

#define MCSPI_WAKEUPEN_WKEN_SHIFT                                                       (0U)
#define MCSPI_WAKEUPEN_WKEN_MASK                                                        (0x00000001U)
#define MCSPI_WAKEUPEN_WKEN_NOWAKEUP                                                     (0U)
#define MCSPI_WAKEUPEN_WKEN_WAKEUP                                                       (1U)

#define MCSPI_SYST_SPIEN_2_SHIFT                                                        (2U)
#define MCSPI_SYST_SPIEN_2_MASK                                                         (0x00000004U)

#define MCSPI_SYST_WAKD_SHIFT                                                           (7U)
#define MCSPI_SYST_WAKD_MASK                                                            (0x00000080U)
#define MCSPI_SYST_WAKD_DRIVENLOW                                                        (0U)
#define MCSPI_SYST_WAKD_DRIVENHIGH                                                       (1U)

#define MCSPI_SYST_SPIDAT_0_SHIFT                                                       (4U)
#define MCSPI_SYST_SPIDAT_0_MASK                                                        (0x00000010U)

#define MCSPI_SYST_SPIEN_1_SHIFT                                                        (1U)
#define MCSPI_SYST_SPIEN_1_MASK                                                         (0x00000002U)

#define MCSPI_SYST_SPIDATDIR1_SHIFT                                                     (9U)
#define MCSPI_SYST_SPIDATDIR1_MASK                                                      (0x00000200U)
#define MCSPI_SYST_SPIDATDIR1_OUT                                                        (0U)
#define MCSPI_SYST_SPIDATDIR1_IN                                                         (1U)

#define MCSPI_SYST_SSB_SHIFT                                                            (11U)
#define MCSPI_SYST_SSB_MASK                                                             (0x00000800U)
#define MCSPI_SYST_SSB_OFF                                                               (0U)
#define MCSPI_SYST_SSB_SETTHEMALL                                                        (1U)

#define MCSPI_SYST_SPICLK_SHIFT                                                         (6U)
#define MCSPI_SYST_SPICLK_MASK                                                          (0x00000040U)

#define MCSPI_SYST_SPIDATDIR0_SHIFT                                                     (8U)
#define MCSPI_SYST_SPIDATDIR0_MASK                                                      (0x00000100U)
#define MCSPI_SYST_SPIDATDIR0_OUT                                                        (0U)
#define MCSPI_SYST_SPIDATDIR0_IN                                                         (1U)

#define MCSPI_SYST_SPIENDIR_SHIFT                                                       (10U)
#define MCSPI_SYST_SPIENDIR_MASK                                                        (0x00000400U)
#define MCSPI_SYST_SPIENDIR_OUT                                                          (0U)
#define MCSPI_SYST_SPIENDIR_IN                                                           (1U)

#define MCSPI_SYST_SPIEN_3_SHIFT                                                        (3U)
#define MCSPI_SYST_SPIEN_3_MASK                                                         (0x00000008U)

#define MCSPI_SYST_SPIEN_0_SHIFT                                                        (0U)
#define MCSPI_SYST_SPIEN_0_MASK                                                         (0x00000001U)

#define MCSPI_SYST_SPIDAT_1_SHIFT                                                       (5U)
#define MCSPI_SYST_SPIDAT_1_MASK                                                        (0x00000020U)

#define MCSPI_MODULCTRL_MS_SHIFT                                                        (2U)
#define MCSPI_MODULCTRL_MS_MASK                                                         (0x00000004U)
#define MCSPI_MODULCTRL_MS_MASTER                                                        (0U)
#define MCSPI_MODULCTRL_MS_SLAVE                                                         (1U)

#define MCSPI_MODULCTRL_SYSTEM_TEST_SHIFT                                               (3U)
#define MCSPI_MODULCTRL_SYSTEM_TEST_MASK                                                (0x00000008U)
#define MCSPI_MODULCTRL_SYSTEM_TEST_OFF                                                  (0U)
#define MCSPI_MODULCTRL_SYSTEM_TEST_ON                                                   (1U)

#define MCSPI_MODULCTRL_SINGLE_SHIFT                                                    (0U)
#define MCSPI_MODULCTRL_SINGLE_MASK                                                     (0x00000001U)
#define MCSPI_MODULCTRL_SINGLE_MODMULTI                                                  (0U)
#define MCSPI_MODULCTRL_SINGLE_MODSINGLE                                                 (1U)

#define MCSPI_MODULCTRL_PIN34_SHIFT                                                     (1U)
#define MCSPI_MODULCTRL_PIN34_MASK                                                      (0x00000002U)
#define MCSPI_MODULCTRL_PIN34_3PINMODE                                                   (1U)
#define MCSPI_MODULCTRL_PIN34_4PINMODE                                                   (0U)

#define MCSPI_MODULCTRL_INITDLY_SHIFT                                                   (4U)
#define MCSPI_MODULCTRL_INITDLY_MASK                                                    (0x00000070U)
#define MCSPI_MODULCTRL_INITDLY_4CLKDLY                                                  (1U)
#define MCSPI_MODULCTRL_INITDLY_NODELAY                                                  (0U)
#define MCSPI_MODULCTRL_INITDLY_8CLKDLY                                                  (2U)
#define MCSPI_MODULCTRL_INITDLY_16CLKDLY                                                 (3U)
#define MCSPI_MODULCTRL_INITDLY_32CLKDLY                                                 (4U)

#define MCSPI_MODULCTRL_MOA_SHIFT                                                       (7U)
#define MCSPI_MODULCTRL_MOA_MASK                                                        (0x00000080U)
#define MCSPI_MODULCTRL_MOA_MULTIACCES                                                   (1U)
#define MCSPI_MODULCTRL_MOA_NOMULTIACCESS                                                (0U)

#define MCSPI_MODULCTRL_FDAA_SHIFT                                                      (8U)
#define MCSPI_MODULCTRL_FDAA_MASK                                                       (0x00000100U)
#define MCSPI_MODULCTRL_FDAA_SHADOWREGEN                                                 (1U)
#define MCSPI_MODULCTRL_FDAA_NOSHADOWREG                                                 (0U)

#define MCSPI_CHCONF_CLKD_SHIFT                                                         (2U)
#define MCSPI_CHCONF_CLKD_MASK                                                          (0x0000003cU)
#define MCSPI_CHCONF_CLKD_DIVBY4K                                                        (12U)
#define MCSPI_CHCONF_CLKD_DIVBY8                                                         (3U)
#define MCSPI_CHCONF_CLKD_DIVBY8K                                                        (13U)
#define MCSPI_CHCONF_CLKD_DIVBY2K                                                        (11U)
#define MCSPI_CHCONF_CLKD_DIVBY1K                                                        (10U)
#define MCSPI_CHCONF_CLKD_DIVBY512                                                       (9U)
#define MCSPI_CHCONF_CLKD_DIVBY32K                                                       (15U)
#define MCSPI_CHCONF_CLKD_DIVBY4                                                         (2U)
#define MCSPI_CHCONF_CLKD_DIVBY1                                                         (0U)
#define MCSPI_CHCONF_CLKD_DIVBY64                                                        (6U)
#define MCSPI_CHCONF_CLKD_DIVBY256                                                       (8U)
#define MCSPI_CHCONF_CLKD_DIVBY128                                                       (7U)
#define MCSPI_CHCONF_CLKD_DIVBY32                                                        (5U)
#define MCSPI_CHCONF_CLKD_DIVBY16K                                                       (14U)
#define MCSPI_CHCONF_CLKD_DIVBY16                                                        (4U)
#define MCSPI_CHCONF_CLKD_DIVBY2                                                         (1U)

#define MCSPI_CHCONF_PHA_SHIFT                                                          (0U)
#define MCSPI_CHCONF_PHA_MASK                                                           (0x00000001U)
#define MCSPI_CHCONF_PHA_ODD                                                             (0U)
#define MCSPI_CHCONF_PHA_EVEN                                                            (1U)

#define MCSPI_CHCONF_TURBO_SHIFT                                                        (19U)
#define MCSPI_CHCONF_TURBO_MASK                                                         (0x00080000U)
#define MCSPI_CHCONF_TURBO_ON                                                            (1U)
#define MCSPI_CHCONF_TURBO_OFF                                                           (0U)

#define MCSPI_CHCONF_SPIENSLV_SHIFT                                                     (21U)
#define MCSPI_CHCONF_SPIENSLV_MASK                                                      (0x00600000U)
#define MCSPI_CHCONF_SPIENSLV_SPIEN2                                                     (2U)
#define MCSPI_CHCONF_SPIENSLV_SPIEN3                                                     (3U)
#define MCSPI_CHCONF_SPIENSLV_SPIEN0                                                     (0U)
#define MCSPI_CHCONF_SPIENSLV_SPIEN1                                                     (1U)

#define MCSPI_CHCONF_DPE0_SHIFT                                                         (16U)
#define MCSPI_CHCONF_DPE0_MASK                                                          (0x00010000U)
#define MCSPI_CHCONF_DPE0_DISABLED                                                       (1U)
#define MCSPI_CHCONF_DPE0_ENABLED                                                        (0U)

#define MCSPI_CHCONF_IS_SHIFT                                                           (18U)
#define MCSPI_CHCONF_IS_MASK                                                            (0x00040000U)
#define MCSPI_CHCONF_IS_LINE0                                                            (0U)
#define MCSPI_CHCONF_IS_LINE1                                                            (1U)

#define MCSPI_CHCONF_DMAR_SHIFT                                                         (15U)
#define MCSPI_CHCONF_DMAR_MASK                                                          (0x00008000U)
#define MCSPI_CHCONF_DMAR_DISABLED                                                       (0U)
#define MCSPI_CHCONF_DMAR_ENABLED                                                        (1U)

#define MCSPI_CHCONF_FORCE_SHIFT                                                        (20U)
#define MCSPI_CHCONF_FORCE_MASK                                                         (0x00100000U)
#define MCSPI_CHCONF_FORCE_DEASSERT                                                      (0U)
#define MCSPI_CHCONF_FORCE_ASSERT                                                        (1U)

#define MCSPI_CHCONF_WL_SHIFT                                                           (7U)
#define MCSPI_CHCONF_WL_MASK                                                            (0x00000f80U)
#define MCSPI_CHCONF_WL_21BITS                                                           (20U)
#define MCSPI_CHCONF_WL_26BITS                                                           (25U)
#define MCSPI_CHCONF_WL_18BITS                                                           (17U)
#define MCSPI_CHCONF_WL_20BITS                                                           (19U)
#define MCSPI_CHCONF_WL_5BITS                                                            (4U)
#define MCSPI_CHCONF_WL_6BITS                                                            (5U)
#define MCSPI_CHCONF_WL_11BITS                                                           (10U)
#define MCSPI_CHCONF_WL_8BITS                                                            (7U)
#define MCSPI_CHCONF_WL_13BITS                                                           (12U)
#define MCSPI_CHCONF_WL_29BITS                                                           (28U)
#define MCSPI_CHCONF_WL_12BITS                                                           (11U)
#define MCSPI_CHCONF_WL_32BITS                                                           (31U)
#define MCSPI_CHCONF_WL_31BITS                                                           (30U)
#define MCSPI_CHCONF_WL_30BITS                                                           (29U)
#define MCSPI_CHCONF_WL_9BITS                                                            (8U)
#define MCSPI_CHCONF_WL_7BITS                                                            (6U)
#define MCSPI_CHCONF_WL_10BITS                                                           (9U)
#define MCSPI_CHCONF_WL_16BITS                                                           (15U)
#define MCSPI_CHCONF_WL_17BITS                                                           (16U)
#define MCSPI_CHCONF_WL_14BITS                                                           (13U)
#define MCSPI_CHCONF_WL_15BITS                                                           (14U)
#define MCSPI_CHCONF_WL_RSVD1                                                            (1U)
#define MCSPI_CHCONF_WL_RSVD0                                                            (0U)
#define MCSPI_CHCONF_WL_27BITS                                                           (26U)
#define MCSPI_CHCONF_WL_23BITS                                                           (22U)
#define MCSPI_CHCONF_WL_RSVD2                                                            (2U)
#define MCSPI_CHCONF_WL_19BITS                                                           (18U)
#define MCSPI_CHCONF_WL_4BITS                                                            (3U)
#define MCSPI_CHCONF_WL_25BITS                                                           (24U)
#define MCSPI_CHCONF_WL_24BITS                                                           (23U)
#define MCSPI_CHCONF_WL_22BITS                                                           (21U)
#define MCSPI_CHCONF_WL_28BITS                                                           (27U)

#define MCSPI_CHCONF_DPE1_SHIFT                                                         (17U)
#define MCSPI_CHCONF_DPE1_MASK                                                          (0x00020000U)
#define MCSPI_CHCONF_DPE1_ENABLED                                                        (0U)
#define MCSPI_CHCONF_DPE1_DISABLED                                                       (1U)

#define MCSPI_CHCONF_EPOL_SHIFT                                                         (6U)
#define MCSPI_CHCONF_EPOL_MASK                                                          (0x00000040U)
#define MCSPI_CHCONF_EPOL_ACTIVELOW                                                      (1U)
#define MCSPI_CHCONF_EPOL_ACTIVEHIGH                                                     (0U)

#define MCSPI_CHCONF_DMAW_SHIFT                                                         (14U)
#define MCSPI_CHCONF_DMAW_MASK                                                          (0x00004000U)
#define MCSPI_CHCONF_DMAW_ENABLED                                                        (1U)
#define MCSPI_CHCONF_DMAW_DISABLED                                                       (0U)

#define MCSPI_CHCONF_TRM_SHIFT                                                          (12U)
#define MCSPI_CHCONF_TRM_MASK                                                           (0x00003000U)
#define MCSPI_CHCONF_TRM_TRANSONLY                                                       (2U)
#define MCSPI_CHCONF_TRM_RSVD                                                            (3U)
#define MCSPI_CHCONF_TRM_TRANSRECEI                                                      (0U)
#define MCSPI_CHCONF_TRM_RECEIVONLY                                                      (1U)

#define MCSPI_CHCONF_POL_SHIFT                                                          (1U)
#define MCSPI_CHCONF_POL_MASK                                                           (0x00000002U)
#define MCSPI_CHCONF_POL_ACTIVELOW                                                       (1U)
#define MCSPI_CHCONF_POL_ACTIVEHIGH                                                      (0U)

#define MCSPI_CHCONF_SBE_SHIFT                                                          (23U)
#define MCSPI_CHCONF_SBE_MASK                                                           (0x00800000U)
#define MCSPI_CHCONF_SBE_DISABLED                                                        (0U)
#define MCSPI_CHCONF_SBE_ENABLED                                                         (1U)

#define MCSPI_CHCONF_SBPOL_SHIFT                                                        (24U)
#define MCSPI_CHCONF_SBPOL_MASK                                                         (0x01000000U)
#define MCSPI_CHCONF_SBPOL_LOWLEVEL                                                      (0U)
#define MCSPI_CHCONF_SBPOL_HIGHLEVEL                                                     (1U)

#define MCSPI_CHCONF_TCS_SHIFT                                                          (25U)
#define MCSPI_CHCONF_TCS_MASK                                                           (0x06000000U)
#define MCSPI_CHCONF_TCS_THREE_HALF_CYCLEDLY                                             (3U)
#define MCSPI_CHCONF_TCS_TWO_HALF_CYCLEDLY                                               (2U)
#define MCSPI_CHCONF_TCS_ONE_HALF_CYCLEDLY                                               (1U)
#define MCSPI_CHCONF_TCS_HALF_CYCLEDLY                                                   (0U)

#define MCSPI_CHCONF_FFER_SHIFT                                                         (28U)
#define MCSPI_CHCONF_FFER_MASK                                                          (0x10000000U)
#define MCSPI_CHCONF_FFER_FFENABLED                                                      (1U)
#define MCSPI_CHCONF_FFER_FFDISABLED                                                     (0U)

#define MCSPI_CHCONF_FFEW_SHIFT                                                         (27U)
#define MCSPI_CHCONF_FFEW_MASK                                                          (0x08000000U)
#define MCSPI_CHCONF_FFEW_FFENABLED                                                      (1U)
#define MCSPI_CHCONF_FFEW_FFDISABLED                                                     (0U)

#define MCSPI_CHCONF_CLKG_SHIFT                                                         (29U)
#define MCSPI_CHCONF_CLKG_MASK                                                          (0x20000000U)
#define MCSPI_CHCONF_CLKG_ONECYCLE                                                       (1U)
#define MCSPI_CHCONF_CLKG_POWERTWO                                                       (0U)

#define MCSPI_CHSTAT_RXS_SHIFT                                                          (0U)
#define MCSPI_CHSTAT_RXS_MASK                                                           (0x00000001U)
#define MCSPI_CHSTAT_RXS_EMPTY                                                           (0U)
#define MCSPI_CHSTAT_RXS_FULL                                                            (1U)

#define MCSPI_CHSTAT_EOT_SHIFT                                                          (2U)
#define MCSPI_CHSTAT_EOT_MASK                                                           (0x00000004U)
#define MCSPI_CHSTAT_EOT_COMPLETED                                                       (1U)
#define MCSPI_CHSTAT_EOT_INPROGRESS                                                      (0U)

#define MCSPI_CHSTAT_TXS_SHIFT                                                          (1U)
#define MCSPI_CHSTAT_TXS_MASK                                                           (0x00000002U)
#define MCSPI_CHSTAT_TXS_EMPTY                                                           (1U)
#define MCSPI_CHSTAT_TXS_FULL                                                            (0U)

#define MCSPI_CHSTAT_RXFFF_SHIFT                                                        (6U)
#define MCSPI_CHSTAT_RXFFF_MASK                                                         (0x00000040U)
#define MCSPI_CHSTAT_RXFFF_FULL                                                          (1U)
#define MCSPI_CHSTAT_RXFFF_NOTFULL                                                       (0U)

#define MCSPI_CHSTAT_TXFFF_SHIFT                                                        (4U)
#define MCSPI_CHSTAT_TXFFF_MASK                                                         (0x00000010U)
#define MCSPI_CHSTAT_TXFFF_FULL                                                          (1U)
#define MCSPI_CHSTAT_TXFFF_NOTFULL                                                       (0U)

#define MCSPI_CHSTAT_RXFFE_SHIFT                                                        (5U)
#define MCSPI_CHSTAT_RXFFE_MASK                                                         (0x00000020U)
#define MCSPI_CHSTAT_RXFFE_EMPTY                                                         (1U)
#define MCSPI_CHSTAT_RXFFE_NOTEMPTY                                                      (0U)

#define MCSPI_CHSTAT_TXFFE_SHIFT                                                        (3U)
#define MCSPI_CHSTAT_TXFFE_MASK                                                         (0x00000008U)
#define MCSPI_CHSTAT_TXFFE_EMPTY                                                         (1U)
#define MCSPI_CHSTAT_TXFFE_NOTEMPTY                                                      (0U)

#define MCSPI_CHCTRL_EN_SHIFT                                                           (0U)
#define MCSPI_CHCTRL_EN_MASK                                                            (0x00000001U)
#define MCSPI_CHCTRL_EN_ACT                                                              (1U)
#define MCSPI_CHCTRL_EN_NACT                                                             (0U)

#define MCSPI_CHCTRL_EXTCLK_SHIFT                                                       (8U)
#define MCSPI_CHCTRL_EXTCLK_MASK                                                        (0x0000ff00U)
#define MCSPI_CHCTRL_EXTCLK_EXTZERO                                                      (0U)
#define MCSPI_CHCTRL_EXTCLK_EXTONE                                                       (1U)
#define MCSPI_CHCTRL_EXTCLK_EXT4080                                                      (255U)

#define MCSPI_TX_TDATA_SHIFT                                                            (0U)
#define MCSPI_TX_TDATA_MASK                                                             (0xffffffffU)

#define MCSPI_RX_RDATA_SHIFT                                                            (0U)
#define MCSPI_RX_RDATA_MASK                                                             (0xffffffffU)

#define MCSPI_XFERLEVEL_AEL_SHIFT                                                       (0U)
#define MCSPI_XFERLEVEL_AEL_MASK                                                        (0x000000ffU)
#define MCSPI_XFERLEVEL_AEL_256BYTES                                                     (255U)
#define MCSPI_XFERLEVEL_AEL_255BYTES                                                     (254U)
#define MCSPI_XFERLEVEL_AEL_2BYTES                                                       (1U)
#define MCSPI_XFERLEVEL_AEL_1BYTE                                                        (0U)

#define MCSPI_XFERLEVEL_WCNT_SHIFT                                                      (16U)
#define MCSPI_XFERLEVEL_WCNT_MASK                                                       (0xffff0000U)
#define MCSPI_XFERLEVEL_WCNT_65535WORD                                                   (65535U)
#define MCSPI_XFERLEVEL_WCNT_65534WORD                                                   (65534U)
#define MCSPI_XFERLEVEL_WCNT_1WORD                                                       (1U)
#define MCSPI_XFERLEVEL_WCNT_DISABLE                                                     (0U)

#define MCSPI_XFERLEVEL_AFL_SHIFT                                                       (8U)
#define MCSPI_XFERLEVEL_AFL_MASK                                                        (0x0000ff00U)
#define MCSPI_XFERLEVEL_AFL_256BYTES                                                     (255U)
#define MCSPI_XFERLEVEL_AFL_255BYTES                                                     (254U)
#define MCSPI_XFERLEVEL_AFL_2BYTES                                                       (1U)
#define MCSPI_XFERLEVEL_AFL_1BYTE                                                        (0U)

#define MCSPI_DAFTX_DAFTDATA_SHIFT                                                      (0U)
#define MCSPI_DAFTX_DAFTDATA_MASK                                                       (0xffffffffU)

#define MCSPI_DAFRX_DAFRDATA_SHIFT                                                      (0U)
#define MCSPI_DAFRX_DAFRDATA_MASK                                                       (0xffffffffU)

#ifdef __cplusplus
}
#endif
#endif  /* HW_MCSPI_H_ */

