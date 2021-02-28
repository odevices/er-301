/*
* hw_wdt.h
*
* Register-level header file for WDT
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
#ifndef HW_WDT_H_
#define HW_WDT_H_

#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
* Register Definitions
****************************************************************************************************/
#define WDT_WWPS                                                (0x34U)
#define WDT_WIRQSTATRAW                                         (0x54U)
#define WDT_WIRQENSET                                           (0x5cU)
#define WDT_WLDR                                                (0x2cU)
#define WDT_WCLR                                                (0x24U)
#define WDT_WDLY                                                (0x44U)
#define WDT_WISR                                                (0x18U)
#define WDT_WIRQSTAT                                            (0x58U)
#define WDT_WSPR                                                (0x48U)
#define WDT_WIDR                                                (0x0U)
#define WDT_WDSC                                                (0x10U)
#define WDT_WCRR                                                (0x28U)
#define WDT_WIER                                                (0x1cU)
#define WDT_WTGR                                                (0x30U)
#define WDT_WDST                                                (0x14U)
#define WDT_WIRQENCLR                                           (0x60U)


/****************************************************************************************************
* Field Definition Macros
****************************************************************************************************/

#define WDT_WWPS_W_PEND_WCLR_SHIFT                              (0U)
#define WDT_WWPS_W_PEND_WCLR_MASK                               (0x00000001U)
#define WDT_WWPS_W_PEND_WCLR_NOT_PENDING                         (0U)
#define WDT_WWPS_W_PEND_WCLR_PENDING                             (1U)

#define WDT_WWPS_W_PEND_WCRR_SHIFT                              (1U)
#define WDT_WWPS_W_PEND_WCRR_MASK                               (0x00000002U)
#define WDT_WWPS_W_PEND_WCRR_NOT_PENDING                         (0U)
#define WDT_WWPS_W_PEND_WCRR_PENDING                             (1U)

#define WDT_WWPS_W_PEND_WLDR_SHIFT                              (2U)
#define WDT_WWPS_W_PEND_WLDR_MASK                               (0x00000004U)
#define WDT_WWPS_W_PEND_WLDR_NOT_PENDING                         (0U)
#define WDT_WWPS_W_PEND_WLDR_PENDING                             (1U)

#define WDT_WWPS_W_PEND_WTGR_SHIFT                              (3U)
#define WDT_WWPS_W_PEND_WTGR_MASK                               (0x00000008U)
#define WDT_WWPS_W_PEND_WTGR_NOT_PENDING                         (0U)
#define WDT_WWPS_W_PEND_WTGR_PENDING                             (1U)

#define WDT_WWPS_W_PEND_WSPR_SHIFT                              (4U)
#define WDT_WWPS_W_PEND_WSPR_MASK                               (0x00000010U)
#define WDT_WWPS_W_PEND_WSPR_NOT_PENDING                         (0U)
#define WDT_WWPS_W_PEND_WSPR_PENDING                             (1U)

#define WDT_WWPS_W_PEND_WDLY_SHIFT                              (5U)
#define WDT_WWPS_W_PEND_WDLY_MASK                               (0x00000020U)
#define WDT_WWPS_W_PEND_WDLY_NOT_PENDING                         (0U)
#define WDT_WWPS_W_PEND_WDLY_PENDING                             (1U)

#define WDT_WIRQSTATRAW_EVT_OVF_SHIFT                           (0U)
#define WDT_WIRQSTATRAW_EVT_OVF_MASK                            (0x00000001U)
#define WDT_WIRQSTATRAW_EVT_OVF_NO                               (0U)
#define WDT_WIRQSTATRAW_EVT_OVF_SET                              (1U)
#define WDT_WIRQSTATRAW_EVT_OVF_PENDING                          (1U)
#define WDT_WIRQSTATRAW_EVT_OVF_NO_ACTION                        (0U)

#define WDT_WIRQSTATRAW_EVT_DLY_SHIFT                           (1U)
#define WDT_WIRQSTATRAW_EVT_DLY_MASK                            (0x00000002U)
#define WDT_WIRQSTATRAW_EVT_DLY_NO                               (0U)
#define WDT_WIRQSTATRAW_EVT_DLY_SET                              (1U)
#define WDT_WIRQSTATRAW_EVT_DLY_PENDING                          (1U)
#define WDT_WIRQSTATRAW_EVT_DLY_NO_ACTION                        (0U)

#define WDT_WIRQENSET_EN_OVF_SHIFT                              (0U)
#define WDT_WIRQENSET_EN_OVF_MASK                               (0x00000001U)
#define WDT_WIRQENSET_EN_OVF_ENABLE                              (1U)
#define WDT_WIRQENSET_EN_OVF_INT_ENABLED                         (1U)
#define WDT_WIRQENSET_EN_OVF_NO_ACTION                           (0U)
#define WDT_WIRQENSET_EN_OVF_INT_DISABLED                        (0U)

#define WDT_WIRQENSET_EN_DLY_SHIFT                              (1U)
#define WDT_WIRQENSET_EN_DLY_MASK                               (0x00000002U)
#define WDT_WIRQENSET_EN_DLY_ENABLE                              (1U)
#define WDT_WIRQENSET_EN_DLY_INT_ENABLED                         (1U)
#define WDT_WIRQENSET_EN_DLY_NO_ACTION                           (0U)
#define WDT_WIRQENSET_EN_DLY_INT_DISABLED                        (0U)

#define WDT_WLDR_TIMER_LOAD_SHIFT                               (0U)
#define WDT_WLDR_TIMER_LOAD_MASK                                (0xffffffffU)

#define WDT_WCLR_PTV_SHIFT                                      (2U)
#define WDT_WCLR_PTV_MASK                                       (0x0000001cU)

#define WDT_WCLR_PRE_SHIFT                                      (5U)
#define WDT_WCLR_PRE_MASK                                       (0x00000020U)
#define WDT_WCLR_PRE_DISABLED                                    (0U)
#define WDT_WCLR_PRE_ENABLED                                     (1U)

#define WDT_WDLY_VALUE_SHIFT                                    (0U)
#define WDT_WDLY_VALUE_MASK                                     (0xffffffffU)

#define WDT_WISR_OVF_IT_FLAG_SHIFT                              (0U)
#define WDT_WISR_OVF_IT_FLAG_MASK                               (0x00000001U)
#define WDT_WISR_OVF_IT_FLAG_INTERRUPT                           (1U)
#define WDT_WISR_OVF_IT_FLAG_UNCHANGED                           (0U)
#define WDT_WISR_OVF_IT_FLAG_NO_INTERRUPT                        (0U)
#define WDT_WISR_OVF_IT_FLAG_CLEARED                             (1U)

#define WDT_WISR_DLY_IT_FLAG_SHIFT                              (1U)
#define WDT_WISR_DLY_IT_FLAG_MASK                               (0x00000002U)
#define WDT_WISR_DLY_IT_FLAG_INTERRUPT                           (1U)
#define WDT_WISR_DLY_IT_FLAG_UNCHANGED                           (0U)
#define WDT_WISR_DLY_IT_FLAG_NO_INTERRUPT                        (0U)
#define WDT_WISR_DLY_IT_FLAG_CLEARED                             (1U)

#define WDT_WIRQSTAT_EVT_OVF_SHIFT                              (0U)
#define WDT_WIRQSTAT_EVT_OVF_MASK                               (0x00000001U)
#define WDT_WIRQSTAT_EVT_OVF_NO                                  (0U)
#define WDT_WIRQSTAT_EVT_OVF_CLR                                 (1U)
#define WDT_WIRQSTAT_EVT_OVF_PENDING                             (1U)
#define WDT_WIRQSTAT_EVT_OVF_NO_ACTION                           (0U)

#define WDT_WIRQSTAT_EVT_DLY_SHIFT                              (1U)
#define WDT_WIRQSTAT_EVT_DLY_MASK                               (0x00000002U)
#define WDT_WIRQSTAT_EVT_DLY_NO                                  (0U)
#define WDT_WIRQSTAT_EVT_DLY_CLEAR                               (1U)
#define WDT_WIRQSTAT_EVT_DLY_PENDING                             (1U)
#define WDT_WIRQSTAT_EVT_DLY_NO_ACTION                           (0U)

#define WDT_WSPR_VALUE_SHIFT                                    (0U)
#define WDT_WSPR_VALUE_MASK                                     (0xffffffffU)

#define WDT_WIDR_REVISION_SHIFT                                 (0U)
#define WDT_WIDR_REVISION_MASK                                  (0xffffffffU)

#define WDT_WDSC_SOFTRESET_SHIFT                                (1U)
#define WDT_WDSC_SOFTRESET_MASK                                 (0x00000002U)
#define WDT_WDSC_SOFTRESET_INITIATE                              (1U)
#define WDT_WDSC_SOFTRESET_ONGOING                               (1U)
#define WDT_WDSC_SOFTRESET_DONE                                  (0U)
#define WDT_WDSC_SOFTRESET_NO_ACTION                             (0U)

#define WDT_WDSC_IDLEMODE_SHIFT                                 (3U)
#define WDT_WDSC_IDLEMODE_MASK                                  (0x00000018U)
#define WDT_WDSC_IDLEMODE_NO_IDLE                                (1U)
#define WDT_WDSC_IDLEMODE_SMART_IDLE                             (2U)
#define WDT_WDSC_IDLEMODE_SMART_IDLE_WAKEUP                      (3U)
#define WDT_WDSC_IDLEMODE_FORCE_IDLE                             (0U)

#define WDT_WDSC_EMUFREE_SHIFT                                  (5U)
#define WDT_WDSC_EMUFREE_MASK                                   (0x00000020U)
#define WDT_WDSC_EMUFREE_FROZEN                                  (0U)
#define WDT_WDSC_EMUFREE_FREE_RUNNING                            (1U)

#define WDT_WCRR_TIMER_CTR_SHIFT                                (0U)
#define WDT_WCRR_TIMER_CTR_MASK                                 (0xffffffffU)

#define WDT_WIER_OVF_IT_ENA_SHIFT                               (0U)
#define WDT_WIER_OVF_IT_ENA_MASK                                (0x00000001U)
#define WDT_WIER_OVF_IT_ENA_DISABLE                              (0U)
#define WDT_WIER_OVF_IT_ENA_ENABLE                               (1U)

#define WDT_WIER_DLY_IT_ENA_SHIFT                               (1U)
#define WDT_WIER_DLY_IT_ENA_MASK                                (0x00000002U)
#define WDT_WIER_DLY_IT_ENA_DISABLE                              (0U)
#define WDT_WIER_DLY_IT_ENA_ENABLE                               (1U)

#define WDT_WTGR_TTGR_VALUE_SHIFT                               (0U)
#define WDT_WTGR_TTGR_VALUE_MASK                                (0xffffffffU)

#define WDT_WDST_RESETDONE_SHIFT                                (0U)
#define WDT_WDST_RESETDONE_MASK                                 (0x00000001U)
#define WDT_WDST_RESETDONE_ONGOING                               (0U)
#define WDT_WDST_RESETDONE_DONE                                  (1U)

#define WDT_WIRQENCLR_EN_OVF_SHIFT                              (0U)
#define WDT_WIRQENCLR_EN_OVF_MASK                               (0x00000001U)
#define WDT_WIRQENCLR_EN_OVF_INT_ENABLED                         (1U)
#define WDT_WIRQENCLR_EN_OVF_NO_ACTION                           (0U)
#define WDT_WIRQENCLR_EN_OVF_INT_DISABLED                        (0U)
#define WDT_WIRQENCLR_EN_OVF_DISABLE                             (1U)

#define WDT_WIRQENCLR_EN_DLY_SHIFT                              (1U)
#define WDT_WIRQENCLR_EN_DLY_MASK                               (0x00000002U)
#define WDT_WIRQENCLR_EN_DLY_INT_ENABLED                         (1U)
#define WDT_WIRQENCLR_EN_DLY_NO_ACTION                           (0U)
#define WDT_WIRQENCLR_EN_DLY_INT_DISABLED                        (0U)
#define WDT_WIRQENCLR_EN_DLY_DISABLE                             (1U)

#ifdef __cplusplus
}
#endif

#endif /* HW_WDT_H_ */
