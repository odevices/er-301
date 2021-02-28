/* =============================================================================
 *   Copyright (c) Texas Instruments Incorporated 2014-2016
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
 *  \file       hw_i2c.h
 *
 *  \brief      register-level header file for I2C
 *
**/

#ifndef HW_I2C_H_
#define HW_I2C_H_

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************************************
* Register Definitions
****************************************************************************************************/
#define I2C_REVNB_LO                                   (0x0U)
#define I2C_REVNB_HI                                   (0x4U)
#define I2C_SYSC                                       (0x10U)
#define I2C_IRQSTATUS_RAW                              (0x24U)
#define I2C_IRQSTATUS                                  (0x28U)
#define I2C_IRQENABLE_SET                              (0x2cU)
#define I2C_IRQENABLE_CLR                              (0x30U)
#define I2C_WE                                         (0x34U)
#define I2C_DMARXENABLE_SET                            (0x38U)
#define I2C_DMATXENABLE_SET                            (0x3cU)
#define I2C_DMARXENABLE_CLR                            (0x40U)
#define I2C_DMATXENABLE_CLR                            (0x44U)
#define I2C_DMARXWAKE_EN                               (0x48U)
#define I2C_DMATXWAKE_EN                               (0x4cU)
#define I2C_SYSS                                       (0x90U)
#define I2C_BUF                                        (0x94U)
#define I2C_CNT                                        (0x98U)
#define I2C_DATA                                       (0x9cU)
#define I2C_CON                                        (0xa4U)
#define I2C_OA                                         (0xa8U)
#define I2C_SA                                         (0xacU)
#define I2C_PSC                                        (0xb0U)
#define I2C_SCLL                                       (0xb4U)
#define I2C_SCLH                                       (0xb8U)
#define I2C_SYSTEST                                    (0xbcU)
#define I2C_BUFSTAT                                    (0xc0U)
#define I2C_OA1                                        (0xc4U)
#define I2C_OA2                                        (0xc8U)
#define I2C_OA3                                        (0xccU)
#define I2C_ACTOA                                      (0xd0U)
#define I2C_SBLOCK                                     (0xd4U)

/****************************************************************************************************
* Field Definition Macros
****************************************************************************************************/

#define I2C_REVNB_LO_REVISION_SHIFT                    ((uint32_t)0U)
#define I2C_REVNB_LO_REVISION_MASK                     (0x0000ffffU)

#define I2C_REVNB_HI_REVISION_SHIFT                    ((uint32_t)0U)
#define I2C_REVNB_HI_REVISION_MASK                     (0x0000ffffU)

#define I2C_SYSC_AUTOIDLE_SHIFT                        ((uint32_t)0U)
#define I2C_SYSC_AUTOIDLE_MASK                         (0x00000001U)
#define I2C_SYSC_AUTOIDLE_DISABLE                       ((uint32_t)0U)
#define I2C_SYSC_AUTOIDLE_ENABLE                        ((uint32_t)1U)

#define I2C_SYSC_SRST_SHIFT                            ((uint32_t)1U)
#define I2C_SYSC_SRST_MASK                             (0x00000002U)
#define I2C_SYSC_SRST_NMODE                             ((uint32_t)0U)
#define I2C_SYSC_SRST_RSTMODE                           ((uint32_t)1U)

#define I2C_SYSC_RESERVED_SHIFT                        ((uint32_t)5U)
#define I2C_SYSC_RESERVED_MASK                         (0x000000e0U)

#define I2C_SYSC_ENAWAKEUP_SHIFT                       ((uint32_t)2U)
#define I2C_SYSC_ENAWAKEUP_MASK                        (0x00000004U)
#define I2C_SYSC_ENAWAKEUP_DISABLE                      ((uint32_t)0U)
#define I2C_SYSC_ENAWAKEUP_ENABLE                       ((uint32_t)1U)

#define I2C_SYSC_RESERVED1_SHIFT                       ((uint32_t)10U)
#define I2C_SYSC_RESERVED1_MASK                        (0x0000fc00U)

#define I2C_SYSC_IDLEMODE_SHIFT                        ((uint32_t)3U)
#define I2C_SYSC_IDLEMODE_MASK                         (0x00000018U)
#define I2C_SYSC_IDLEMODE_FORCEIDLE                     ((uint32_t)0U)
#define I2C_SYSC_IDLEMODE_SMARTIDLE_WAKEUP              ((uint32_t)3U)
#define I2C_SYSC_IDLEMODE_NOIDLE                        ((uint32_t)1U)
#define I2C_SYSC_IDLEMODE_SMARTIDLE                     ((uint32_t)2U)

#define I2C_SYSC_CLKACTIVITY_SHIFT                     ((uint32_t)8U)
#define I2C_SYSC_CLKACTIVITY_MASK                      (0x00000300U)
#define I2C_SYSC_CLKACTIVITY_BOOTHOFF                   ((uint32_t)0U)
#define I2C_SYSC_CLKACTIVITY_OCPON                      ((uint32_t)1U)
#define I2C_SYSC_CLKACTIVITY_SYSON                      ((uint32_t)2U)
#define I2C_SYSC_CLKACTIVITY_BOOTHON                    ((uint32_t)3U)

#define I2C_IRQSTATUS_RAW_AL_SHIFT                     ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_AL_MASK                      (0x00000001U)
#define I2C_IRQSTATUS_RAW_AL_CLEAR                      ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_AL_SET                        ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_NACK_SHIFT                   ((uint32_t)1U)
#define I2C_IRQSTATUS_RAW_NACK_MASK                    (0x00000002U)
#define I2C_IRQSTATUS_RAW_NACK_CLEAR                    ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_NACK_SET                      ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_ARDY_SHIFT                   ((uint32_t)2U)
#define I2C_IRQSTATUS_RAW_ARDY_MASK                    (0x00000004U)
#define I2C_IRQSTATUS_RAW_ARDY_CLEAR                    ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_ARDY_SET                      ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_RRDY_SHIFT                   ((uint32_t)3U)
#define I2C_IRQSTATUS_RAW_RRDY_MASK                    (0x00000008U)
#define I2C_IRQSTATUS_RAW_RRDY_CLEAR                    ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_RRDY_SET                      ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_XRDY_SHIFT                   ((uint32_t)4U)
#define I2C_IRQSTATUS_RAW_XRDY_MASK                    (0x00000010U)
#define I2C_IRQSTATUS_RAW_XRDY_CLEAR                    ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_XRDY_SET                      ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_GC_SHIFT                     ((uint32_t)5U)
#define I2C_IRQSTATUS_RAW_GC_MASK                      (0x00000020U)
#define I2C_IRQSTATUS_RAW_GC_CLEAR                      ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_GC_SET                        ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_STC_SHIFT                    ((uint32_t)6U)
#define I2C_IRQSTATUS_RAW_STC_MASK                     (0x00000040U)
#define I2C_IRQSTATUS_RAW_STC_CLEAR                     ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_STC_SET                       ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_AERR_SHIFT                   ((uint32_t)7U)
#define I2C_IRQSTATUS_RAW_AERR_MASK                    (0x00000080U)
#define I2C_IRQSTATUS_RAW_AERR_CLEAR                    ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_AERR_SET                      ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_BF_SHIFT                     ((uint32_t)8U)
#define I2C_IRQSTATUS_RAW_BF_MASK                      (0x00000100U)
#define I2C_IRQSTATUS_RAW_BF_CLEAR                      ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_BF_SET                        ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_AAS_SHIFT                    ((uint32_t)9U)
#define I2C_IRQSTATUS_RAW_AAS_MASK                     (0x00000200U)
#define I2C_IRQSTATUS_RAW_AAS_CLEAR                     ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_AAS_SET                       ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_XUDF_SHIFT                   ((uint32_t)10U)
#define I2C_IRQSTATUS_RAW_XUDF_MASK                    (0x00000400U)
#define I2C_IRQSTATUS_RAW_XUDF_CLEAR                    ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_XUDF_SET                      ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_ROVR_SHIFT                   ((uint32_t)11U)
#define I2C_IRQSTATUS_RAW_ROVR_MASK                    (0x00000800U)
#define I2C_IRQSTATUS_RAW_ROVR_CLEAR                    ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_ROVR_SET                      ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_BB_SHIFT                     ((uint32_t)12U)
#define I2C_IRQSTATUS_RAW_BB_MASK                      (0x00001000U)
#define I2C_IRQSTATUS_RAW_BB_CLEAR                      ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_BB_SET                        ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_RDR_SHIFT                    ((uint32_t)13U)
#define I2C_IRQSTATUS_RAW_RDR_MASK                     (0x00002000U)
#define I2C_IRQSTATUS_RAW_RDR_CLEAR                     ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_RDR_SET                       ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_XDR_SHIFT                    ((uint32_t)14U)
#define I2C_IRQSTATUS_RAW_XDR_MASK                     (0x00004000U)
#define I2C_IRQSTATUS_RAW_XDR_CLEAR                     ((uint32_t)0U)
#define I2C_IRQSTATUS_RAW_XDR_SET                       ((uint32_t)1U)

#define I2C_IRQSTATUS_RAW_RESERVED_11_SHIFT            ((uint32_t)15U)
#define I2C_IRQSTATUS_RAW_RESERVED_11_MASK             (0x00008000U)

#define I2C_IRQSTATUS_AL_SHIFT                         ((uint32_t)0U)
#define I2C_IRQSTATUS_AL_MASK                          (0x00000001U)
#define I2C_IRQSTATUS_AL_CLEAR                          ((uint32_t)0U)
#define I2C_IRQSTATUS_AL_SET                            ((uint32_t)1U)

#define I2C_IRQSTATUS_NACK_SHIFT                       ((uint32_t)1U)
#define I2C_IRQSTATUS_NACK_MASK                        (0x00000002U)
#define I2C_IRQSTATUS_NACK_CLEAR                        ((uint32_t)0U)
#define I2C_IRQSTATUS_NACK_SET                          ((uint32_t)1U)

#define I2C_IRQSTATUS_ARDY_SHIFT                       ((uint32_t)2U)
#define I2C_IRQSTATUS_ARDY_MASK                        (0x00000004U)
#define I2C_IRQSTATUS_ARDY_CLEAR                        ((uint32_t)0U)
#define I2C_IRQSTATUS_ARDY_SET                          ((uint32_t)1U)

#define I2C_IRQSTATUS_RRDY_SHIFT                       ((uint32_t)3U)
#define I2C_IRQSTATUS_RRDY_MASK                        (0x00000008U)
#define I2C_IRQSTATUS_RRDY_CLEAR                        ((uint32_t)0U)
#define I2C_IRQSTATUS_RRDY_SET                          ((uint32_t)1U)

#define I2C_IRQSTATUS_XRDY_SHIFT                       ((uint32_t)4U)
#define I2C_IRQSTATUS_XRDY_MASK                        (0x00000010U)
#define I2C_IRQSTATUS_XRDY_CLEAR                        ((uint32_t)0U)
#define I2C_IRQSTATUS_XRDY_SET                          ((uint32_t)1U)

#define I2C_IRQSTATUS_GC_SHIFT                         ((uint32_t)5U)
#define I2C_IRQSTATUS_GC_MASK                          (0x00000020U)
#define I2C_IRQSTATUS_GC_CLEAR                          ((uint32_t)0U)
#define I2C_IRQSTATUS_GC_SET                            ((uint32_t)1U)

#define I2C_IRQSTATUS_STC_SHIFT                        ((uint32_t)6U)
#define I2C_IRQSTATUS_STC_MASK                         (0x00000040U)
#define I2C_IRQSTATUS_STC_CLEAR                         ((uint32_t)0U)
#define I2C_IRQSTATUS_STC_SET                           ((uint32_t)1U)

#define I2C_IRQSTATUS_AERR_SHIFT                       ((uint32_t)7U)
#define I2C_IRQSTATUS_AERR_MASK                        (0x00000080U)
#define I2C_IRQSTATUS_AERR_CLEAR                        ((uint32_t)0U)
#define I2C_IRQSTATUS_AERR_SET                          ((uint32_t)1U)

#define I2C_IRQSTATUS_BF_SHIFT                         ((uint32_t)8U)
#define I2C_IRQSTATUS_BF_MASK                          (0x00000100U)
#define I2C_IRQSTATUS_BF_CLEAR                          ((uint32_t)0U)
#define I2C_IRQSTATUS_BF_SET                            ((uint32_t)1U)

#define I2C_IRQSTATUS_AAS_SHIFT                        ((uint32_t)9U)
#define I2C_IRQSTATUS_AAS_MASK                         (0x00000200U)
#define I2C_IRQSTATUS_AAS_CLEAR                         ((uint32_t)0U)
#define I2C_IRQSTATUS_AAS_SET                           ((uint32_t)1U)

#define I2C_IRQSTATUS_XUDF_SHIFT                       ((uint32_t)10U)
#define I2C_IRQSTATUS_XUDF_MASK                        (0x00000400U)
#define I2C_IRQSTATUS_XUDF_CLEAR                        ((uint32_t)0U)
#define I2C_IRQSTATUS_XUDF_SET                          ((uint32_t)1U)

#define I2C_IRQSTATUS_ROVR_SHIFT                       ((uint32_t)11U)
#define I2C_IRQSTATUS_ROVR_MASK                        (0x00000800U)
#define I2C_IRQSTATUS_ROVR_CLEAR                        ((uint32_t)0U)
#define I2C_IRQSTATUS_ROVR_SET                          ((uint32_t)1U)

#define I2C_IRQSTATUS_BB_SHIFT                         ((uint32_t)12U)
#define I2C_IRQSTATUS_BB_MASK                          (0x00001000U)
#define I2C_IRQSTATUS_BB_CLEAR                          ((uint32_t)0U)
#define I2C_IRQSTATUS_BB_SET                            ((uint32_t)1U)

#define I2C_IRQSTATUS_RDR_SHIFT                        ((uint32_t)13U)
#define I2C_IRQSTATUS_RDR_MASK                         (0x00002000U)
#define I2C_IRQSTATUS_RDR_CLEAR                         ((uint32_t)0U)
#define I2C_IRQSTATUS_RDR_SET                           ((uint32_t)1U)

#define I2C_IRQSTATUS_XDR_SHIFT                        ((uint32_t)14U)
#define I2C_IRQSTATUS_XDR_MASK                         (0x00004000U)
#define I2C_IRQSTATUS_XDR_CLEAR                         ((uint32_t)0U)
#define I2C_IRQSTATUS_XDR_SET                           ((uint32_t)1U)

#define I2C_IRQSTATUS_RESERVED_11_SHIFT                ((uint32_t)15U)
#define I2C_IRQSTATUS_RESERVED_11_MASK                 (0x00008000U)

#define I2C_IRQENABLE_SET_AL_IE_SHIFT                  ((uint32_t)0U)
#define I2C_IRQENABLE_SET_AL_IE_MASK                   (0x00000001U)
#define I2C_IRQENABLE_SET_AL_IE_DISABLE                 ((uint32_t)0U)
#define I2C_IRQENABLE_SET_AL_IE_ENABLE                  ((uint32_t)1U)

#define I2C_IRQENABLE_SET_NACK_IE_SHIFT                ((uint32_t)1U)
#define I2C_IRQENABLE_SET_NACK_IE_MASK                 (0x00000002U)
#define I2C_IRQENABLE_SET_NACK_IE_DISABLE               ((uint32_t)0U)
#define I2C_IRQENABLE_SET_NACK_IE_ENABLE                ((uint32_t)1U)

#define I2C_IRQENABLE_SET_ARDY_IE_SHIFT                ((uint32_t)2U)
#define I2C_IRQENABLE_SET_ARDY_IE_MASK                 (0x00000004U)
#define I2C_IRQENABLE_SET_ARDY_IE_DISABLE               ((uint32_t)0U)
#define I2C_IRQENABLE_SET_ARDY_IE_ENABLE                ((uint32_t)1U)

#define I2C_IRQENABLE_SET_RRDY_IE_SHIFT                ((uint32_t)3U)
#define I2C_IRQENABLE_SET_RRDY_IE_MASK                 (0x00000008U)
#define I2C_IRQENABLE_SET_RRDY_IE_DISABLE               ((uint32_t)0U)
#define I2C_IRQENABLE_SET_RRDY_IE_ENABLE                ((uint32_t)1U)

#define I2C_IRQENABLE_SET_XRDY_IE_SHIFT                ((uint32_t)4U)
#define I2C_IRQENABLE_SET_XRDY_IE_MASK                 (0x00000010U)
#define I2C_IRQENABLE_SET_XRDY_IE_DISABLE               ((uint32_t)0U)
#define I2C_IRQENABLE_SET_XRDY_IE_ENABLE                ((uint32_t)1U)

#define I2C_IRQENABLE_SET_GC_IE_SHIFT                  ((uint32_t)5U)
#define I2C_IRQENABLE_SET_GC_IE_MASK                   (0x00000020U)
#define I2C_IRQENABLE_SET_GC_IE_DISABLE                 ((uint32_t)0U)
#define I2C_IRQENABLE_SET_GC_IE_ENABLE                  ((uint32_t)1U)

#define I2C_IRQENABLE_SET_STC_IE_SHIFT                 ((uint32_t)6U)
#define I2C_IRQENABLE_SET_STC_IE_MASK                  (0x00000040U)
#define I2C_IRQENABLE_SET_STC_IE_DISABLE                ((uint32_t)0U)
#define I2C_IRQENABLE_SET_STC_IE_ENABLE                 ((uint32_t)1U)

#define I2C_IRQENABLE_SET_AERR_IE_SHIFT                ((uint32_t)7U)
#define I2C_IRQENABLE_SET_AERR_IE_MASK                 (0x00000080U)
#define I2C_IRQENABLE_SET_AERR_IE_DISABLE               ((uint32_t)0U)
#define I2C_IRQENABLE_SET_AERR_IE_ENABLE                ((uint32_t)1U)

#define I2C_IRQENABLE_SET_BF_IE_SHIFT                  ((uint32_t)8U)
#define I2C_IRQENABLE_SET_BF_IE_MASK                   (0x00000100U)
#define I2C_IRQENABLE_SET_BF_IE_DISABLE                 ((uint32_t)0U)
#define I2C_IRQENABLE_SET_BF_IE_ENABLE                  ((uint32_t)1U)

#define I2C_IRQENABLE_SET_ASS_IE_SHIFT                 ((uint32_t)9U)
#define I2C_IRQENABLE_SET_ASS_IE_MASK                  (0x00000200U)
#define I2C_IRQENABLE_SET_ASS_IE_DISABLE                ((uint32_t)0U)
#define I2C_IRQENABLE_SET_ASS_IE_ENABLE                 ((uint32_t)1U)

#define I2C_IRQENABLE_SET_XUDF_SHIFT                   ((uint32_t)10U)
#define I2C_IRQENABLE_SET_XUDF_MASK                    (0x00000400U)
#define I2C_IRQENABLE_SET_XUDF_DISABLE                  ((uint32_t)0U)
#define I2C_IRQENABLE_SET_XUDF_ENABLE                   ((uint32_t)1U)

#define I2C_IRQENABLE_SET_ROVR_SHIFT                   ((uint32_t)11U)
#define I2C_IRQENABLE_SET_ROVR_MASK                    (0x00000800U)
#define I2C_IRQENABLE_SET_ROVR_DISABLE                  ((uint32_t)0U)
#define I2C_IRQENABLE_SET_ROVR_ENABLE                   ((uint32_t)1U)

#define I2C_IRQENABLE_SET_RESERVED1_SHIFT              ((uint32_t)12U)
#define I2C_IRQENABLE_SET_RESERVED1_MASK               (0x00001000U)

#define I2C_IRQENABLE_SET_RDR_IE_SHIFT                 ((uint32_t)13U)
#define I2C_IRQENABLE_SET_RDR_IE_MASK                  (0x00002000U)
#define I2C_IRQENABLE_SET_RDR_IE_DISABLE                ((uint32_t)0U)
#define I2C_IRQENABLE_SET_RDR_IE_ENABLE                 ((uint32_t)1U)

#define I2C_IRQENABLE_SET_XDR_IE_SHIFT                 ((uint32_t)14U)
#define I2C_IRQENABLE_SET_XDR_IE_MASK                  (0x00004000U)
#define I2C_IRQENABLE_SET_XDR_IE_DISABLE                ((uint32_t)0U)
#define I2C_IRQENABLE_SET_XDR_IE_ENABLE                 ((uint32_t)1U)

#define I2C_IRQENABLE_SET_RESERVED_5_SHIFT             ((uint32_t)15U)
#define I2C_IRQENABLE_SET_RESERVED_5_MASK              (0x00008000U)

#define I2C_IRQENABLE_CLR_AL_IE_SHIFT                  ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_AL_IE_MASK                   (0x00000001U)
#define I2C_IRQENABLE_CLR_AL_IE_DISABLE                 ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_AL_IE_ENABLE                  ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_NACK_IE_SHIFT                ((uint32_t)1U)
#define I2C_IRQENABLE_CLR_NACK_IE_MASK                 (0x00000002U)
#define I2C_IRQENABLE_CLR_NACK_IE_DISABLE               ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_NACK_IE_ENABLE                ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_ARDY_IE_SHIFT                ((uint32_t)2U)
#define I2C_IRQENABLE_CLR_ARDY_IE_MASK                 (0x00000004U)
#define I2C_IRQENABLE_CLR_ARDY_IE_DISABLE               ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_ARDY_IE_ENABLE                ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_RRDY_IE_SHIFT                ((uint32_t)3U)
#define I2C_IRQENABLE_CLR_RRDY_IE_MASK                 (0x00000008U)
#define I2C_IRQENABLE_CLR_RRDY_IE_DISABLE               ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_RRDY_IE_ENABLE                ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_XRDY_IE_SHIFT                ((uint32_t)4U)
#define I2C_IRQENABLE_CLR_XRDY_IE_MASK                 (0x00000010U)
#define I2C_IRQENABLE_CLR_XRDY_IE_DISABLE               ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_XRDY_IE_ENABLE                ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_GC_IE_SHIFT                  ((uint32_t)5U)
#define I2C_IRQENABLE_CLR_GC_IE_MASK                   (0x00000020U)
#define I2C_IRQENABLE_CLR_GC_IE_DISABLE                 ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_GC_IE_ENABLE                  ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_STC_IE_SHIFT                 ((uint32_t)6U)
#define I2C_IRQENABLE_CLR_STC_IE_MASK                  (0x00000040U)
#define I2C_IRQENABLE_CLR_STC_IE_DISABLE                ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_STC_IE_ENABLE                 ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_AERR_IE_SHIFT                ((uint32_t)7U)
#define I2C_IRQENABLE_CLR_AERR_IE_MASK                 (0x00000080U)
#define I2C_IRQENABLE_CLR_AERR_IE_DISABLE               ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_AERR_IE_ENABLE                ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_BF_IE_SHIFT                  ((uint32_t)8U)
#define I2C_IRQENABLE_CLR_BF_IE_MASK                   (0x00000100U)
#define I2C_IRQENABLE_CLR_BF_IE_DISABLE                 ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_BF_IE_ENABLE                  ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_ASS_IE_SHIFT                 ((uint32_t)9U)
#define I2C_IRQENABLE_CLR_ASS_IE_MASK                  (0x00000200U)
#define I2C_IRQENABLE_CLR_ASS_IE_DISABLE                ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_ASS_IE_ENABLE                 ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_XUDF_SHIFT                   ((uint32_t)10U)
#define I2C_IRQENABLE_CLR_XUDF_MASK                    (0x00000400U)
#define I2C_IRQENABLE_CLR_XUDF_DISABLE                  ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_XUDF_ENABLE                   ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_ROVR_SHIFT                   ((uint32_t)11U)
#define I2C_IRQENABLE_CLR_ROVR_MASK                    (0x00000800U)
#define I2C_IRQENABLE_CLR_ROVR_DISABLE                  ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_ROVR_ENABLE                   ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_RESERVED1_SHIFT              ((uint32_t)12U)
#define I2C_IRQENABLE_CLR_RESERVED1_MASK               (0x00001000U)

#define I2C_IRQENABLE_CLR_RDR_IE_SHIFT                 ((uint32_t)13U)
#define I2C_IRQENABLE_CLR_RDR_IE_MASK                  (0x00002000U)
#define I2C_IRQENABLE_CLR_RDR_IE_DISABLE                ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_RDR_IE_ENABLE                 ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_XDR_IE_SHIFT                 ((uint32_t)14U)
#define I2C_IRQENABLE_CLR_XDR_IE_MASK                  (0x00004000U)
#define I2C_IRQENABLE_CLR_XDR_IE_DISABLE                ((uint32_t)0U)
#define I2C_IRQENABLE_CLR_XDR_IE_ENABLE                 ((uint32_t)1U)

#define I2C_IRQENABLE_CLR_RESERVED_5_SHIFT             ((uint32_t)15U)
#define I2C_IRQENABLE_CLR_RESERVED_5_MASK              (0x00008000U)

#define I2C_WE_AL_SHIFT                                ((uint32_t)0U)
#define I2C_WE_AL_MASK                                 (0x00000001U)
#define I2C_WE_AL_ENABLE                                ((uint32_t)1U)
#define I2C_WE_AL_DISABLE                               ((uint32_t)0U)

#define I2C_WE_NACK_SHIFT                              ((uint32_t)1U)
#define I2C_WE_NACK_MASK                               (0x00000002U)
#define I2C_WE_NACK_DISABLE                             ((uint32_t)0U)
#define I2C_WE_NACK_ENABLE                              ((uint32_t)1U)

#define I2C_WE_ARDY_SHIFT                              ((uint32_t)2U)
#define I2C_WE_ARDY_MASK                               (0x00000004U)
#define I2C_WE_ARDY_ENABLE                              ((uint32_t)1U)
#define I2C_WE_ARDY_DISABLE                             ((uint32_t)0U)

#define I2C_WE_DRDY_SHIFT                              ((uint32_t)3U)
#define I2C_WE_DRDY_MASK                               (0x00000008U)
#define I2C_WE_DRDY_DISABLE                             ((uint32_t)0U)
#define I2C_WE_DRDY_ENABLE                              ((uint32_t)1U)

#define I2C_WE_RESERVED1_SHIFT                         ((uint32_t)4U)
#define I2C_WE_RESERVED1_MASK                          (0x00000010U)

#define I2C_WE_GC_SHIFT                                ((uint32_t)5U)
#define I2C_WE_GC_MASK                                 (0x00000020U)
#define I2C_WE_GC_DISABLE                               ((uint32_t)0U)
#define I2C_WE_GC_ENABLE                                ((uint32_t)1U)

#define I2C_WE_STC_SHIFT                               ((uint32_t)6U)
#define I2C_WE_STC_MASK                                (0x00000040U)
#define I2C_WE_STC_DISABLE                              ((uint32_t)0U)
#define I2C_WE_STC_ENABLE                               ((uint32_t)1U)

#define I2C_WE_RESERVED2_SHIFT                         ((uint32_t)7U)
#define I2C_WE_RESERVED2_MASK                          (0x00000080U)

#define I2C_WE_BF_SHIFT                                ((uint32_t)8U)
#define I2C_WE_BF_MASK                                 (0x00000100U)
#define I2C_WE_BF_DISABLE                               ((uint32_t)0U)
#define I2C_WE_BF_ENABLE                                ((uint32_t)1U)

#define I2C_WE_AAS_SHIFT                               ((uint32_t)9U)
#define I2C_WE_AAS_MASK                                (0x00000200U)
#define I2C_WE_AAS_DISABLE                              ((uint32_t)0U)
#define I2C_WE_AAS_ENABLE                               ((uint32_t)1U)

#define I2C_WE_XUDF_SHIFT                              ((uint32_t)10U)
#define I2C_WE_XUDF_MASK                               (0x00000400U)
#define I2C_WE_XUDF_DISABLE                             ((uint32_t)0U)
#define I2C_WE_XUDF_ENABLE                              ((uint32_t)1U)

#define I2C_WE_ROVR_SHIFT                              ((uint32_t)11U)
#define I2C_WE_ROVR_MASK                               (0x00000800U)
#define I2C_WE_ROVR_DISABLE                             ((uint32_t)0U)
#define I2C_WE_ROVR_ENABLE                              ((uint32_t)1U)

#define I2C_WE_RESERVED3_SHIFT                         ((uint32_t)12U)
#define I2C_WE_RESERVED3_MASK                          (0x00001000U)

#define I2C_WE_RDR_SHIFT                               ((uint32_t)13U)
#define I2C_WE_RDR_MASK                                (0x00002000U)
#define I2C_WE_RDR_DISABLE                              ((uint32_t)0U)
#define I2C_WE_RDR_ENABLE                               ((uint32_t)1U)

#define I2C_WE_XDR_SHIFT                               ((uint32_t)14U)
#define I2C_WE_XDR_MASK                                (0x00004000U)
#define I2C_WE_XDR_DISABLE                              ((uint32_t)0U)
#define I2C_WE_XDR_ENABLE                               ((uint32_t)1U)

#define I2C_WE_RESERVED_SHIFT                          ((uint32_t)15U)
#define I2C_WE_RESERVED_MASK                           (0x00008000U)

#define I2C_DMARXENABLE_SET_DMARX_ENABLE_SET_SHIFT     ((uint32_t)0U)
#define I2C_DMARXENABLE_SET_DMARX_ENABLE_SET_MASK      (0x00000001U)

#define I2C_DMARXENABLE_SET_RESERVED_SHIFT             ((uint32_t)1U)
#define I2C_DMARXENABLE_SET_RESERVED_MASK              (0x0000fffeU)

#define I2C_DMATXENABLE_SET_DMATX_ENABLE_SET_SHIFT     ((uint32_t)0U)
#define I2C_DMATXENABLE_SET_DMATX_ENABLE_SET_MASK      (0x00000001U)

#define I2C_DMATXENABLE_SET_RESERVED_SHIFT             ((uint32_t)1U)
#define I2C_DMATXENABLE_SET_RESERVED_MASK              (0x0000fffeU)

#define I2C_DMARXENABLE_CLR_DMARX_ENABLE_CLEAR_SHIFT   ((uint32_t)0U)
#define I2C_DMARXENABLE_CLR_DMARX_ENABLE_CLEAR_MASK    (0x00000001U)

#define I2C_DMARXENABLE_CLR_RESERVED_SHIFT             ((uint32_t)1U)
#define I2C_DMARXENABLE_CLR_RESERVED_MASK              (0x0000fffeU)

#define I2C_DMATXENABLE_CLR_DMATX_ENABLE_CLEAR_SHIFT   ((uint32_t)0U)
#define I2C_DMATXENABLE_CLR_DMATX_ENABLE_CLEAR_MASK    (0x00000001U)

#define I2C_DMATXENABLE_CLR_RESERVED_SHIFT             ((uint32_t)1U)
#define I2C_DMATXENABLE_CLR_RESERVED_MASK              (0x0000fffeU)

#define I2C_DMARXWAKE_EN_AL_SHIFT                      ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_AL_MASK                       (0x00000001U)
#define I2C_DMARXWAKE_EN_AL_ENABLE                      ((uint32_t)1U)
#define I2C_DMARXWAKE_EN_AL_DISABLE                     ((uint32_t)0U)

#define I2C_DMARXWAKE_EN_NACK_SHIFT                    ((uint32_t)1U)
#define I2C_DMARXWAKE_EN_NACK_MASK                     (0x00000002U)
#define I2C_DMARXWAKE_EN_NACK_DISABLE                   ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_NACK_ENABLE                    ((uint32_t)1U)

#define I2C_DMARXWAKE_EN_ARDY_SHIFT                    ((uint32_t)2U)
#define I2C_DMARXWAKE_EN_ARDY_MASK                     (0x00000004U)
#define I2C_DMARXWAKE_EN_ARDY_ENABLE                    ((uint32_t)1U)
#define I2C_DMARXWAKE_EN_ARDY_DISABLE                   ((uint32_t)0U)

#define I2C_DMARXWAKE_EN_DRDY_SHIFT                    ((uint32_t)3U)
#define I2C_DMARXWAKE_EN_DRDY_MASK                     (0x00000008U)
#define I2C_DMARXWAKE_EN_DRDY_DISABLE                   ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_DRDY_ENABLE                    ((uint32_t)1U)

#define I2C_DMARXWAKE_EN_RESERVED1_SHIFT               ((uint32_t)4U)
#define I2C_DMARXWAKE_EN_RESERVED1_MASK                (0x00000010U)

#define I2C_DMARXWAKE_EN_GC_SHIFT                      ((uint32_t)5U)
#define I2C_DMARXWAKE_EN_GC_MASK                       (0x00000020U)
#define I2C_DMARXWAKE_EN_GC_DISABLE                     ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_GC_ENABLE                      ((uint32_t)1U)

#define I2C_DMARXWAKE_EN_STC_SHIFT                     ((uint32_t)6U)
#define I2C_DMARXWAKE_EN_STC_MASK                      (0x00000040U)
#define I2C_DMARXWAKE_EN_STC_DISABLE                    ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_STC_ENABLE                     ((uint32_t)1U)

#define I2C_DMARXWAKE_EN_RESERVED2_SHIFT               ((uint32_t)7U)
#define I2C_DMARXWAKE_EN_RESERVED2_MASK                (0x00000080U)

#define I2C_DMARXWAKE_EN_BF_SHIFT                      ((uint32_t)8U)
#define I2C_DMARXWAKE_EN_BF_MASK                       (0x00000100U)
#define I2C_DMARXWAKE_EN_BF_DISABLE                     ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_BF_ENABLE                      ((uint32_t)1U)

#define I2C_DMARXWAKE_EN_AAS_SHIFT                     ((uint32_t)9U)
#define I2C_DMARXWAKE_EN_AAS_MASK                      (0x00000200U)
#define I2C_DMARXWAKE_EN_AAS_DISABLE                    ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_AAS_ENABLE                     ((uint32_t)1U)

#define I2C_DMARXWAKE_EN_XUDF_SHIFT                    ((uint32_t)10U)
#define I2C_DMARXWAKE_EN_XUDF_MASK                     (0x00000400U)
#define I2C_DMARXWAKE_EN_XUDF_DISABLE                   ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_XUDF_ENABLE                    ((uint32_t)1U)

#define I2C_DMARXWAKE_EN_ROVR_SHIFT                    ((uint32_t)11U)
#define I2C_DMARXWAKE_EN_ROVR_MASK                     (0x00000800U)
#define I2C_DMARXWAKE_EN_ROVR_DISABLE                   ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_ROVR_ENABLE                    ((uint32_t)1U)

#define I2C_DMARXWAKE_EN_RESERVED3_SHIFT               ((uint32_t)12U)
#define I2C_DMARXWAKE_EN_RESERVED3_MASK                (0x00001000U)

#define I2C_DMARXWAKE_EN_RDR_SHIFT                     ((uint32_t)13U)
#define I2C_DMARXWAKE_EN_RDR_MASK                      (0x00002000U)
#define I2C_DMARXWAKE_EN_RDR_DISABLE                    ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_RDR_ENABLE                     ((uint32_t)1U)

#define I2C_DMARXWAKE_EN_XDR_SHIFT                     ((uint32_t)14U)
#define I2C_DMARXWAKE_EN_XDR_MASK                      (0x00004000U)
#define I2C_DMARXWAKE_EN_XDR_DISABLE                    ((uint32_t)0U)
#define I2C_DMARXWAKE_EN_XDR_ENABLE                     ((uint32_t)1U)

#define I2C_DMARXWAKE_EN_RESERVED_SHIFT                ((uint32_t)15U)
#define I2C_DMARXWAKE_EN_RESERVED_MASK                 (0x00008000U)

#define I2C_DMATXWAKE_EN_AL_SHIFT                      ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_AL_MASK                       (0x00000001U)
#define I2C_DMATXWAKE_EN_AL_ENABLE                      ((uint32_t)1U)
#define I2C_DMATXWAKE_EN_AL_DISABLE                     ((uint32_t)0U)

#define I2C_DMATXWAKE_EN_NACK_SHIFT                    ((uint32_t)1U)
#define I2C_DMATXWAKE_EN_NACK_MASK                     (0x00000002U)
#define I2C_DMATXWAKE_EN_NACK_DISABLE                   ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_NACK_ENABLE                    ((uint32_t)1U)

#define I2C_DMATXWAKE_EN_ARDY_SHIFT                    ((uint32_t)2U)
#define I2C_DMATXWAKE_EN_ARDY_MASK                     (0x00000004U)
#define I2C_DMATXWAKE_EN_ARDY_ENABLE                    ((uint32_t)1U)
#define I2C_DMATXWAKE_EN_ARDY_DISABLE                   ((uint32_t)0U)

#define I2C_DMATXWAKE_EN_DRDY_SHIFT                    ((uint32_t)3U)
#define I2C_DMATXWAKE_EN_DRDY_MASK                     (0x00000008U)
#define I2C_DMATXWAKE_EN_DRDY_DISABLE                   ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_DRDY_ENABLE                    ((uint32_t)1U)

#define I2C_DMATXWAKE_EN_RESERVED1_SHIFT               ((uint32_t)4U)
#define I2C_DMATXWAKE_EN_RESERVED1_MASK                (0x00000010U)

#define I2C_DMATXWAKE_EN_GC_SHIFT                      ((uint32_t)5U)
#define I2C_DMATXWAKE_EN_GC_MASK                       (0x00000020U)
#define I2C_DMATXWAKE_EN_GC_DISABLE                     ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_GC_ENABLE                      ((uint32_t)1U)

#define I2C_DMATXWAKE_EN_STC_SHIFT                     ((uint32_t)6U)
#define I2C_DMATXWAKE_EN_STC_MASK                      (0x00000040U)
#define I2C_DMATXWAKE_EN_STC_DISABLE                    ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_STC_ENABLE                     ((uint32_t)1U)

#define I2C_DMATXWAKE_EN_RESERVED2_SHIFT               ((uint32_t)7U)
#define I2C_DMATXWAKE_EN_RESERVED2_MASK                (0x00000080U)

#define I2C_DMATXWAKE_EN_BF_SHIFT                      ((uint32_t)8U)
#define I2C_DMATXWAKE_EN_BF_MASK                       (0x00000100U)
#define I2C_DMATXWAKE_EN_BF_DISABLE                     ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_BF_ENABLE                      ((uint32_t)1U)

#define I2C_DMATXWAKE_EN_AAS_SHIFT                     ((uint32_t)9U)
#define I2C_DMATXWAKE_EN_AAS_MASK                      (0x00000200U)
#define I2C_DMATXWAKE_EN_AAS_DISABLE                    ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_AAS_ENABLE                     ((uint32_t)1U)

#define I2C_DMATXWAKE_EN_XUDF_SHIFT                    ((uint32_t)10U)
#define I2C_DMATXWAKE_EN_XUDF_MASK                     (0x00000400U)
#define I2C_DMATXWAKE_EN_XUDF_DISABLE                   ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_XUDF_ENABLE                    ((uint32_t)1U)

#define I2C_DMATXWAKE_EN_ROVR_SHIFT                    ((uint32_t)11U)
#define I2C_DMATXWAKE_EN_ROVR_MASK                     (0x00000800U)
#define I2C_DMATXWAKE_EN_ROVR_DISABLE                   ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_ROVR_ENABLE                    ((uint32_t)1U)

#define I2C_DMATXWAKE_EN_RESERVED3_SHIFT               ((uint32_t)12U)
#define I2C_DMATXWAKE_EN_RESERVED3_MASK                (0x00001000U)

#define I2C_DMATXWAKE_EN_RDR_SHIFT                     ((uint32_t)13U)
#define I2C_DMATXWAKE_EN_RDR_MASK                      (0x00002000U)
#define I2C_DMATXWAKE_EN_RDR_DISABLE                    ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_RDR_ENABLE                     ((uint32_t)1U)

#define I2C_DMATXWAKE_EN_XDR_SHIFT                     ((uint32_t)14U)
#define I2C_DMATXWAKE_EN_XDR_MASK                      (0x00004000U)
#define I2C_DMATXWAKE_EN_XDR_DISABLE                    ((uint32_t)0U)
#define I2C_DMATXWAKE_EN_XDR_ENABLE                     ((uint32_t)1U)

#define I2C_DMATXWAKE_EN_RESERVED_SHIFT                ((uint32_t)15U)
#define I2C_DMATXWAKE_EN_RESERVED_MASK                 (0x00008000U)

#define I2C_SYSS_RDONE_SHIFT                           ((uint32_t)0U)
#define I2C_SYSS_RDONE_MASK                            (0x00000001U)
#define I2C_SYSS_RDONE_RSTONGOING                       ((uint32_t)0U)
#define I2C_SYSS_RDONE_RSTCOMP                          ((uint32_t)1U)

#define I2C_SYSS_RESERVED_SHIFT                        ((uint32_t)1U)
#define I2C_SYSS_RESERVED_MASK                         (0x0000fffeU)

#define I2C_BUF_TXTRSH_SHIFT                           ((uint32_t)0U)
#define I2C_BUF_TXTRSH_MASK                            (0x0000003fU)

#define I2C_BUF_TXFIFO_CLR_SHIFT                       ((uint32_t)6U)
#define I2C_BUF_TXFIFO_CLR_MASK                        (0x00000040U)
#define I2C_BUF_TXFIFO_CLR_NMODE                        ((uint32_t)0U)
#define I2C_BUF_TXFIFO_CLR_RSTMODE                      ((uint32_t)1U)

#define I2C_BUF_XDMA_EN_SHIFT                          ((uint32_t)7U)
#define I2C_BUF_XDMA_EN_MASK                           (0x00000080U)
#define I2C_BUF_XDMA_EN_DISABLE                         ((uint32_t)0U)
#define I2C_BUF_XDMA_EN_ENABLE                          ((uint32_t)1U)

#define I2C_BUF_RXTRSH_SHIFT                           ((uint32_t)8U)
#define I2C_BUF_RXTRSH_MASK                            (0x00003f00U)

#define I2C_BUF_RXFIFO_CLR_SHIFT                       ((uint32_t)14U)
#define I2C_BUF_RXFIFO_CLR_MASK                        (0x00004000U)
#define I2C_BUF_RXFIFO_CLR_NMODE                        ((uint32_t)0U)
#define I2C_BUF_RXFIFO_CLR_RSTMODE                      ((uint32_t)1U)

#define I2C_BUF_RDMA_EN_SHIFT                          ((uint32_t)15U)
#define I2C_BUF_RDMA_EN_MASK                           (0x00008000U)
#define I2C_BUF_RDMA_EN_DISABLE                         ((uint32_t)0U)
#define I2C_BUF_RDMA_EN_ENABLE                          ((uint32_t)1U)

#define I2C_CNT_DCOUNT_SHIFT                           ((uint32_t)0U)
#define I2C_CNT_DCOUNT_MASK                            (0x0000ffffU)

#define I2C_DATA_DATA_SHIFT                            ((uint32_t)0U)
#define I2C_DATA_DATA_MASK                             (0x000000ffU)

#define I2C_DATA_RESERVED_SHIFT                        ((uint32_t)8U)
#define I2C_DATA_RESERVED_MASK                         (0x0000ff00U)

#define I2C_CON_STT_SHIFT                              ((uint32_t)0U)
#define I2C_CON_STT_MASK                               (0x00000001U)
#define I2C_CON_STT_NSTT                                ((uint32_t)0U)
#define I2C_CON_STT_STT                                 ((uint32_t)1U)

#define I2C_CON_STP_SHIFT                              ((uint32_t)1U)
#define I2C_CON_STP_MASK                               (0x00000002U)
#define I2C_CON_STP_NSTP                                ((uint32_t)0U)
#define I2C_CON_STP_STP                                 ((uint32_t)1U)

#define I2C_CON_RESERVED1_SHIFT                        ((uint32_t)2U)
#define I2C_CON_RESERVED1_MASK                         (0x0000000cU)

#define I2C_CON_XOA3_SHIFT                             ((uint32_t)4U)
#define I2C_CON_XOA3_MASK                              (0x00000010U)
#define I2C_CON_XOA3_B07                                ((uint32_t)0U)
#define I2C_CON_XOA3_B10                                ((uint32_t)1U)

#define I2C_CON_XOA2_SHIFT                             ((uint32_t)5U)
#define I2C_CON_XOA2_MASK                              (0x00000020U)
#define I2C_CON_XOA2_B07                                ((uint32_t)0U)
#define I2C_CON_XOA2_B10                                ((uint32_t)1U)

#define I2C_CON_XOA1_SHIFT                             ((uint32_t)6U)
#define I2C_CON_XOA1_MASK                              (0x00000040U)
#define I2C_CON_XOA1_B07                                ((uint32_t)0U)
#define I2C_CON_XOA1_B10                                ((uint32_t)1U)

#define I2C_CON_XOA0_SHIFT                             ((uint32_t)7U)
#define I2C_CON_XOA0_MASK                              (0x00000080U)
#define I2C_CON_XOA0_B07                                ((uint32_t)0U)
#define I2C_CON_XOA0_B10                                ((uint32_t)1U)

#define I2C_CON_XSA_SHIFT                              ((uint32_t)8U)
#define I2C_CON_XSA_MASK                               (0x00000100U)
#define I2C_CON_XSA_B07                                 ((uint32_t)0U)
#define I2C_CON_XSA_B10                                 ((uint32_t)1U)

#define I2C_CON_TRX_SHIFT                              ((uint32_t)9U)
#define I2C_CON_TRX_MASK                               (0x00000200U)
#define I2C_CON_TRX_RCV                                 ((uint32_t)0U)
#define I2C_CON_TRX_TRX                                 ((uint32_t)1U)

#define I2C_CON_MST_SHIFT                              ((uint32_t)10U)
#define I2C_CON_MST_MASK                               (0x00000400U)
#define I2C_CON_MST_SLV                                 ((uint32_t)0U)
#define I2C_CON_MST_MST                                 ((uint32_t)1U)

#define I2C_CON_STB_SHIFT                              ((uint32_t)11U)
#define I2C_CON_STB_MASK                               (0x00000800U)
#define I2C_CON_STB_NORMAL                              ((uint32_t)0U)
#define I2C_CON_STB_STB                                 ((uint32_t)1U)

#define I2C_CON_OPMODE_SHIFT                           ((uint32_t)12U)
#define I2C_CON_OPMODE_MASK                            (0x00003000U)
#define I2C_CON_OPMODE_FSI2C                            ((uint32_t)0U)
#define I2C_CON_OPMODE_HSI2C                            ((uint32_t)1U)
#define I2C_CON_OPMODE_SCCB                             ((uint32_t)2U)
#define I2C_CON_OPMODE_RESERVED                         ((uint32_t)3U)

#define I2C_CON_RESERVED_SHIFT                         ((uint32_t)14U)
#define I2C_CON_RESERVED_MASK                          (0x00004000U)

#define I2C_CON_I2C_EN_SHIFT                           ((uint32_t)15U)
#define I2C_CON_I2C_EN_MASK                            (0x00008000U)
#define I2C_CON_I2C_EN_DISABLE                          ((uint32_t)0U)
#define I2C_CON_I2C_EN_ENABLE                           ((uint32_t)1U)

#define I2C_OA_OA_SHIFT                                ((uint32_t)0U)
#define I2C_OA_OA_MASK                                 (0x000003ffU)

#define I2C_OA_RESERVED_SHIFT                          ((uint32_t)10U)
#define I2C_OA_RESERVED_MASK                           (0x00001c00U)

#define I2C_OA_MCODE_SHIFT                             ((uint32_t)13U)
#define I2C_OA_MCODE_MASK                              (0x0000e000U)

#define I2C_SA_SA_SHIFT                                ((uint32_t)0U)
#define I2C_SA_SA_MASK                                 (0x000003ffU)

#define I2C_SA_RESERVED_SHIFT                          ((uint32_t)10U)
#define I2C_SA_RESERVED_MASK                           (0x0000fc00U)

#define I2C_PSC_PSC_SHIFT                              ((uint32_t)0U)
#define I2C_PSC_PSC_MASK                               (0x000000ffU)

#define I2C_PSC_RESERVED_SHIFT                         ((uint32_t)8U)
#define I2C_PSC_RESERVED_MASK                          (0x0000ff00U)

#define I2C_SCLL_SCLL_SHIFT                            ((uint32_t)0U)
#define I2C_SCLL_SCLL_MASK                             (0x000000ffU)

#define I2C_SCLL_HSSCLL_SHIFT                          ((uint32_t)8U)
#define I2C_SCLL_HSSCLL_MASK                           (0x0000ff00U)

#define I2C_SCLH_SCLH_SHIFT                            ((uint32_t)0U)
#define I2C_SCLH_SCLH_MASK                             (0x000000ffU)

#define I2C_SCLH_HSSCLH_SHIFT                          ((uint32_t)8U)
#define I2C_SCLH_HSSCLH_MASK                           (0x0000ff00U)

#define I2C_SYSTEST_SDA_O_SHIFT                        ((uint32_t)0U)
#define I2C_SYSTEST_SDA_O_MASK                         (0x00000001U)
#define I2C_SYSTEST_SDA_O_SDAOL                         ((uint32_t)0U)
#define I2C_SYSTEST_SDA_O_SDAOH                         ((uint32_t)1U)

#define I2C_SYSTEST_SDA_I_SHIFT                        ((uint32_t)1U)
#define I2C_SYSTEST_SDA_I_MASK                         (0x00000002U)
#define I2C_SYSTEST_SDA_I_SDAIL                         ((uint32_t)0U)
#define I2C_SYSTEST_SDA_I_SDAIH                         ((uint32_t)1U)

#define I2C_SYSTEST_SCL_O_SHIFT                        ((uint32_t)2U)
#define I2C_SYSTEST_SCL_O_MASK                         (0x00000004U)
#define I2C_SYSTEST_SCL_O_SCLOL                         ((uint32_t)0U)
#define I2C_SYSTEST_SCL_O_SCLOH                         ((uint32_t)1U)

#define I2C_SYSTEST_SDA_O_FUNC_SHIFT                   ((uint32_t)5U)
#define I2C_SYSTEST_SDA_O_FUNC_MASK                    (0x00000020U)
#define I2C_SYSTEST_SDA_O_FUNC_SDAOL                    ((uint32_t)0U)
#define I2C_SYSTEST_SDA_O_FUNC_SDAOH                    ((uint32_t)1U)

#define I2C_SYSTEST_SDA_I_FUNC_SHIFT                   ((uint32_t)6U)
#define I2C_SYSTEST_SDA_I_FUNC_MASK                    (0x00000040U)
#define I2C_SYSTEST_SDA_I_FUNC_SDAIL                    ((uint32_t)0U)
#define I2C_SYSTEST_SDA_I_FUNC_SDAIH                    ((uint32_t)1U)

#define I2C_SYSTEST_SCL_O_FUNC_SHIFT                   ((uint32_t)7U)
#define I2C_SYSTEST_SCL_O_FUNC_MASK                    (0x00000080U)
#define I2C_SYSTEST_SCL_O_FUNC_SCLIL                    ((uint32_t)0U)
#define I2C_SYSTEST_SCL_O_FUNC_SCLIH                    ((uint32_t)1U)

#define I2C_SYSTEST_SCL_I_FUNC_SHIFT                   ((uint32_t)8U)
#define I2C_SYSTEST_SCL_I_FUNC_MASK                    (0x00000100U)
#define I2C_SYSTEST_SCL_I_FUNC_SCLIL                    ((uint32_t)0U)
#define I2C_SYSTEST_SCL_I_FUNC_SCLIH                    ((uint32_t)1U)

#define I2C_SYSTEST_SSB_SHIFT                          ((uint32_t)11U)
#define I2C_SYSTEST_SSB_MASK                           (0x00000800U)
#define I2C_SYSTEST_SSB_NOACTION                        ((uint32_t)0U)
#define I2C_SYSTEST_SSB_SETSTATUS                       ((uint32_t)1U)

#define I2C_SYSTEST_TMODE_SHIFT                        ((uint32_t)12U)
#define I2C_SYSTEST_TMODE_MASK                         (0x00003000U)
#define I2C_SYSTEST_TMODE_FUNCTIONAL                    ((uint32_t)0U)
#define I2C_SYSTEST_TMODE_RESERVED                      ((uint32_t)1U)
#define I2C_SYSTEST_TMODE_TEST                          ((uint32_t)2U)
#define I2C_SYSTEST_TMODE_LOOPBACK                      ((uint32_t)3U)

#define I2C_SYSTEST_ST_EN_SHIFT                        ((uint32_t)15U)
#define I2C_SYSTEST_ST_EN_MASK                         (0x00008000U)
#define I2C_SYSTEST_ST_EN_DISABLE                       ((uint32_t)0U)
#define I2C_SYSTEST_ST_EN_ENABLE                        ((uint32_t)1U)

#define I2C_SYSTEST_SCL_I_SHIFT                        ((uint32_t)3U)
#define I2C_SYSTEST_SCL_I_MASK                         (0x00000008U)
#define I2C_SYSTEST_SCL_I_SCLIL                         ((uint32_t)0U)
#define I2C_SYSTEST_SCL_I_SCLIH                         ((uint32_t)1U)

#define I2C_SYSTEST_FREE_SHIFT                         ((uint32_t)14U)
#define I2C_SYSTEST_FREE_MASK                          (0x00004000U)
#define I2C_SYSTEST_FREE_STOP                           ((uint32_t)0U)
#define I2C_SYSTEST_FREE_FREE                           ((uint32_t)1U)

#define I2C_SYSTEST_RESERVED_1_SHIFT                   ((uint32_t)4U)
#define I2C_SYSTEST_RESERVED_1_MASK                    (0x00000010U)
#define I2C_SYSTEST_RESERVED_1_SCCBOH                   ((uint32_t)1U)
#define I2C_SYSTEST_RESERVED_1_SCCBOL                   ((uint32_t)0U)

#define I2C_SYSTEST_RESERVED_2_SHIFT                   ((uint32_t)9U)
#define I2C_SYSTEST_RESERVED_2_MASK                    (0x00000600U)

#define I2C_BUFSTAT_TXSTAT_SHIFT                       ((uint32_t)0U)
#define I2C_BUFSTAT_TXSTAT_MASK                        (0x0000003fU)

#define I2C_BUFSTAT_RXSTAT_SHIFT                       ((uint32_t)8U)
#define I2C_BUFSTAT_RXSTAT_MASK                        (0x00003f00U)

#define I2C_BUFSTAT_RESERVED_SHIFT                     ((uint32_t)6U)
#define I2C_BUFSTAT_RESERVED_MASK                      (0x000000c0U)

#define I2C_BUFSTAT_FIFODEPTH_SHIFT                    ((uint32_t)14U)
#define I2C_BUFSTAT_FIFODEPTH_MASK                     (0x0000c000U)

#define I2C_OA1_OA1_SHIFT                              ((uint32_t)0U)
#define I2C_OA1_OA1_MASK                               (0x000003ffU)

#define I2C_OA1_RESERVED_SHIFT                         ((uint32_t)10U)
#define I2C_OA1_RESERVED_MASK                          (0x0000fc00U)

#define I2C_OA2_OA2_SHIFT                              ((uint32_t)0U)
#define I2C_OA2_OA2_MASK                               (0x000003ffU)

#define I2C_OA2_RESERVED_SHIFT                         ((uint32_t)10U)
#define I2C_OA2_RESERVED_MASK                          (0x0000fc00U)

#define I2C_OA3_OA3_SHIFT                              ((uint32_t)0U)
#define I2C_OA3_OA3_MASK                               (0x000003ffU)

#define I2C_OA3_RESERVED_SHIFT                         ((uint32_t)10U)
#define I2C_OA3_RESERVED_MASK                          (0x0000fc00U)

#define I2C_ACTOA_OA0_ACT_SHIFT                        ((uint32_t)0U)
#define I2C_ACTOA_OA0_ACT_MASK                         (0x00000001U)
#define I2C_ACTOA_OA0_ACT_INACTIVE                      ((uint32_t)0U)
#define I2C_ACTOA_OA0_ACT_ACTIVE                        ((uint32_t)1U)

#define I2C_ACTOA_RESERVED_SHIFT                       ((uint32_t)4U)
#define I2C_ACTOA_RESERVED_MASK                        (0x0000fff0U)

#define I2C_ACTOA_OA1_ACT_SHIFT                        ((uint32_t)1U)
#define I2C_ACTOA_OA1_ACT_MASK                         (0x00000002U)
#define I2C_ACTOA_OA1_ACT_ACTIVE                        ((uint32_t)1U)
#define I2C_ACTOA_OA1_ACT_INACTIVE                      ((uint32_t)0U)

#define I2C_ACTOA_OA2_ACT_SHIFT                        ((uint32_t)2U)
#define I2C_ACTOA_OA2_ACT_MASK                         (0x00000004U)
#define I2C_ACTOA_OA2_ACT_ACTIVE                        ((uint32_t)1U)
#define I2C_ACTOA_OA2_ACT_INACTIVE                      ((uint32_t)0U)

#define I2C_ACTOA_OA3_ACT_SHIFT                        ((uint32_t)3U)
#define I2C_ACTOA_OA3_ACT_MASK                         (0x00000008U)
#define I2C_ACTOA_OA3_ACT_ACTIVE                        ((uint32_t)1U)
#define I2C_ACTOA_OA3_ACT_INACTIVE                      ((uint32_t)0U)

#define I2C_SBLOCK_OA0_EN_SHIFT                        ((uint32_t)0U)
#define I2C_SBLOCK_OA0_EN_MASK                         (0x00000001U)
#define I2C_SBLOCK_OA0_EN_LOCK                          ((uint32_t)1U)
#define I2C_SBLOCK_OA0_EN_UNLOCK                        ((uint32_t)0U)

#define I2C_SBLOCK_RESERVED_SHIFT                      ((uint32_t)4U)
#define I2C_SBLOCK_RESERVED_MASK                       (0x0000fff0U)

#define I2C_SBLOCK_OA1_EN_SHIFT                        ((uint32_t)1U)
#define I2C_SBLOCK_OA1_EN_MASK                         (0x00000002U)
#define I2C_SBLOCK_OA1_EN_LOCK                          ((uint32_t)1U)
#define I2C_SBLOCK_OA1_EN_UNLOCK                        ((uint32_t)0U)

#define I2C_SBLOCK_OA2_EN_SHIFT                        ((uint32_t)2U)
#define I2C_SBLOCK_OA2_EN_MASK                         (0x00000004U)
#define I2C_SBLOCK_OA2_EN_LOCK                          ((uint32_t)1U)
#define I2C_SBLOCK_OA2_EN_UNLOCK                        ((uint32_t)0U)

#define I2C_SBLOCK_OA3_EN_SHIFT                        ((uint32_t)3U)
#define I2C_SBLOCK_OA3_EN_MASK                         (0x00000008U)
#define I2C_SBLOCK_OA3_EN_LOCK                          ((uint32_t)1U)
#define I2C_SBLOCK_OA3_EN_UNLOCK                        ((uint32_t)0U)

#ifdef __cplusplus
}
#endif
#endif  /* _HW_I2C_H_ */
