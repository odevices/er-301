/*
 * eqep.h
 *
 *  Created on: 4 Nov 2015
 *      Author: clarkson
 */

#ifndef INCLUDE_ER301_EQEP_H_
#define INCLUDE_ER301_EQEP_H_

#include "hw_pwmss.h"

/**************************************************************************
* Register Macros
**************************************************************************/
#define EQEP_QPOSCMP                                        (0xCU)
#define EQEP_QFLG                                           (0x32U)
#define EQEP_QCTMRLAT                                       (0x3EU)
#define EQEP_QEPSTS                                         (0x38U)
#define EQEP_QWDPRD                                         (0x26U)
#define EQEP_QCPRDLAT                                       (0x40U)
#define EQEP_QPOSMAX                                        (0x8U)
#define EQEP_QCLR                                           (0x34U)
#define EQEP_QPOSCNT                                        (0x0U)
#define EQEP_QCAPCTL                                        (0x2CU)
#define EQEP_QPOSLAT                                        (0x18U)
#define EQEP_QPOSILAT                                       (0x10U)
#define EQEP_REVID                                          (0x5CU)
#define EQEP_QPOSSLAT                                       (0x14U)
#define EQEP_QCTMR                                          (0x3AU)
#define EQEP_QWDTMR                                         (0x24U)
#define EQEP_QCPRD                                          (0x3CU)
#define EQEP_QDECCTL                                        (0x28U)
#define EQEP_QUTMR                                          (0x1CU)
#define EQEP_QFRC                                           (0x36U)
#define EQEP_QPOSINIT                                       (0x4U)
#define EQEP_QEINT                                          (0x30U)
#define EQEP_QUPRD                                          (0x20U)
#define EQEP_QEPCTL                                         (0x2AU)
#define EQEP_QPOSCTL                                        (0x2EU)


/**************************************************************************
* Field Definition Macros
**************************************************************************/

/* QPOSCMP */

#define EQEP_QPOSCMP_QPOSCMP_MASK                           (0xFFFFFFFFU)
#define EQEP_QPOSCMP_QPOSCMP_SHIFT                          (0U)
#define EQEP_QPOSCMP_QPOSCMP_RESETVAL                       (0x00000000U)
#define EQEP_QPOSCMP_QPOSCMP_MAX                            (0xffffffffU)

#define EQEP_QPOSCMP_RESETVAL                               (0x00000000U)

/* QFLG */

#define EQEP_QFLG_INT_MASK                                  (0x00000001U)
#define EQEP_QFLG_INT_SHIFT                                 (0U)
#define EQEP_QFLG_INT_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_INT_EN_0X1                                (0x00000001U)
#define EQEP_QFLG_INT_EN_0X0                                (0x00000000U)

#define EQEP_QFLG_PCE_MASK                                  (0x00000002U)
#define EQEP_QFLG_PCE_SHIFT                                 (1U)
#define EQEP_QFLG_PCE_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_PCE_EN_0X1                                (0x00000001U)
#define EQEP_QFLG_PCE_EN_0X0                                (0x00000000U)

#define EQEP_QFLG_PHE_MASK                                  (0x00000004U)
#define EQEP_QFLG_PHE_SHIFT                                 (2U)
#define EQEP_QFLG_PHE_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_PHE_EN_0X1                                (0x00000001U)
#define EQEP_QFLG_PHE_EN_0X0                                (0x00000000U)

#define EQEP_QFLG_QDC_MASK                                  (0x00000008U)
#define EQEP_QFLG_QDC_SHIFT                                 (3U)
#define EQEP_QFLG_QDC_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_QDC_EN_0X1                                (0x00000001U)
#define EQEP_QFLG_QDC_EN_0X0                                (0x00000000U)

#define EQEP_QFLG_WTO_MASK                                  (0x00000010U)
#define EQEP_QFLG_WTO_SHIFT                                 (4U)
#define EQEP_QFLG_WTO_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_WTO_EN_0X0                                (0x00000000U)
#define EQEP_QFLG_WTO_EN_0X1                                (0x00000001U)

#define EQEP_QFLG_PCU_MASK                                  (0x00000020U)
#define EQEP_QFLG_PCU_SHIFT                                 (5U)
#define EQEP_QFLG_PCU_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_PCU_EN_0X1                                (0x00000001U)
#define EQEP_QFLG_PCU_EN_0X0                                (0x00000000U)

#define EQEP_QFLG_PCO_MASK                                  (0x00000040U)
#define EQEP_QFLG_PCO_SHIFT                                 (6U)
#define EQEP_QFLG_PCO_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_PCO_EN_0X1                                (0x00000001U)
#define EQEP_QFLG_PCO_EN_0X0                                (0x00000000U)

#define EQEP_QFLG_PCR_MASK                                  (0x00000080U)
#define EQEP_QFLG_PCR_SHIFT                                 (7U)
#define EQEP_QFLG_PCR_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_PCR_EN_0X1                                (0x00000001U)
#define EQEP_QFLG_PCR_EN_0X0                                (0x00000000U)

#define EQEP_QFLG_PCM_MASK                                  (0x00000100U)
#define EQEP_QFLG_PCM_SHIFT                                 (8U)
#define EQEP_QFLG_PCM_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_PCM_EN_0X0                                (0x00000000U)
#define EQEP_QFLG_PCM_EN_0X1                                (0x00000001U)

#define EQEP_QFLG_SEL_MASK                                  (0x00000200U)
#define EQEP_QFLG_SEL_SHIFT                                 (9U)
#define EQEP_QFLG_SEL_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_SEL_EN_0X0                                (0x00000000U)
#define EQEP_QFLG_SEL_EN_0X1                                (0x00000001U)

#define EQEP_QFLG_IEL_MASK                                  (0x00000400U)
#define EQEP_QFLG_IEL_SHIFT                                 (10U)
#define EQEP_QFLG_IEL_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_IEL_EN_0X1                                (0x00000001U)
#define EQEP_QFLG_IEL_EN_0X0                                (0x00000000U)

#define EQEP_QFLG_UTO_MASK                                  (0x00000800U)
#define EQEP_QFLG_UTO_SHIFT                                 (11U)
#define EQEP_QFLG_UTO_RESETVAL                              (0x00000000U)
#define EQEP_QFLG_UTO_EN_0X1                                (0x00000001U)
#define EQEP_QFLG_UTO_EN_0X0                                (0x00000000U)

#define EQEP_QFLG_RESETVAL                                  (0x00000000U)

/* QCTMRLAT */

#define EQEP_QCTMRLAT_QCTMRLAT_MASK                         (0x0000FFFFU)
#define EQEP_QCTMRLAT_QCTMRLAT_SHIFT                        (0U)
#define EQEP_QCTMRLAT_QCTMRLAT_RESETVAL                     (0x00000000U)
#define EQEP_QCTMRLAT_QCTMRLAT_MAX                          (0x0000ffffU)

#define EQEP_QCTMRLAT_RESETVAL                              (0x00000000U)

/* QEPSTS */

#define EQEP_QEPSTS_PCEF_MASK                               (0x00000001U)
#define EQEP_QEPSTS_PCEF_SHIFT                              (0U)
#define EQEP_QEPSTS_PCEF_RESETVAL                           (0x00000000U)
#define EQEP_QEPSTS_PCEF_EN_0X1                             (0x00000001U)
#define EQEP_QEPSTS_PCEF_EN_0X0                             (0x00000000U)

#define EQEP_QEPSTS_FIMF_MASK                               (0x00000002U)
#define EQEP_QEPSTS_FIMF_SHIFT                              (1U)
#define EQEP_QEPSTS_FIMF_RESETVAL                           (0x00000000U)
#define EQEP_QEPSTS_FIMF_EN_0X0                             (0x00000000U)
#define EQEP_QEPSTS_FIMF_EN_0X1                             (0x00000001U)

#define EQEP_QEPSTS_CDEF_MASK                               (0x00000004U)
#define EQEP_QEPSTS_CDEF_SHIFT                              (2U)
#define EQEP_QEPSTS_CDEF_RESETVAL                           (0x00000000U)
#define EQEP_QEPSTS_CDEF_EN_0X0                             (0x00000000U)
#define EQEP_QEPSTS_CDEF_EN_0X1                             (0x00000001U)

#define EQEP_QEPSTS_COEF_MASK                               (0x00000008U)
#define EQEP_QEPSTS_COEF_SHIFT                              (3U)
#define EQEP_QEPSTS_COEF_RESETVAL                           (0x00000000U)
#define EQEP_QEPSTS_COEF_EN_0X0                             (0x00000000U)
#define EQEP_QEPSTS_COEF_EN_0X1                             (0x00000001U)

#define EQEP_QEPSTS_QDLF_MASK                               (0x00000010U)
#define EQEP_QEPSTS_QDLF_SHIFT                              (4U)
#define EQEP_QEPSTS_QDLF_RESETVAL                           (0x00000000U)
#define EQEP_QEPSTS_QDLF_EN_0X1                             (0x00000001U)
#define EQEP_QEPSTS_QDLF_EN_0X0                             (0x00000000U)

#define EQEP_QEPSTS_QDF_MASK                                (0x00000020U)
#define EQEP_QEPSTS_QDF_SHIFT                               (5U)
#define EQEP_QEPSTS_QDF_RESETVAL                            (0x00000000U)
#define EQEP_QEPSTS_QDF_EN_0X0                              (0x00000000U)
#define EQEP_QEPSTS_QDF_EN_0X1                              (0x00000001U)

#define EQEP_QEPSTS_FDF_MASK                                (0x00000040U)
#define EQEP_QEPSTS_FDF_SHIFT                               (6U)
#define EQEP_QEPSTS_FDF_RESETVAL                            (0x00000000U)
#define EQEP_QEPSTS_FDF_EN_0X0                              (0x00000000U)
#define EQEP_QEPSTS_FDF_EN_0X1                              (0x00000001U)

#define EQEP_QEPSTS_UPEVNT_MASK                             (0x00000080U)
#define EQEP_QEPSTS_UPEVNT_SHIFT                            (7U)
#define EQEP_QEPSTS_UPEVNT_RESETVAL                         (0x00000000U)
#define EQEP_QEPSTS_UPEVNT_EN_0X0                           (0x00000000U)
#define EQEP_QEPSTS_UPEVNT_EN_0X1                           (0x00000001U)

#define EQEP_QEPSTS_RESETVAL                                (0x00000000U)

/* QWDPRD */

#define EQEP_QWDPRD_QWDPRD_MASK                             (0x0000FFFFU)
#define EQEP_QWDPRD_QWDPRD_SHIFT                            (0U)
#define EQEP_QWDPRD_QWDPRD_RESETVAL                         (0x00000000U)
#define EQEP_QWDPRD_QWDPRD_MAX                              (0x0000ffffU)

#define EQEP_QWDPRD_RESETVAL                                (0x00000000U)

/* QCPRDLAT */

#define EQEP_QCPRDLAT_QCPRDLAT_MASK                         (0x0000FFFFU)
#define EQEP_QCPRDLAT_QCPRDLAT_SHIFT                        (0U)
#define EQEP_QCPRDLAT_QCPRDLAT_RESETVAL                     (0x00000000U)
#define EQEP_QCPRDLAT_QCPRDLAT_MAX                          (0x0000ffffU)

#define EQEP_QCPRDLAT_RESETVAL                              (0x00000000U)

/* QPOSMAX */

#define EQEP_QPOSMAX_QPOSMAX_MASK                           (0xFFFFFFFFU)
#define EQEP_QPOSMAX_QPOSMAX_SHIFT                          (0U)
#define EQEP_QPOSMAX_QPOSMAX_RESETVAL                       (0x00000000U)
#define EQEP_QPOSMAX_QPOSMAX_MAX                            (0xffffffffU)

#define EQEP_QPOSMAX_RESETVAL                               (0x00000000U)

/* QCLR */

#define EQEP_QCLR_INT_MASK                                  (0x00000001U)
#define EQEP_QCLR_INT_SHIFT                                 (0U)
#define EQEP_QCLR_INT_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_INT_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_INT_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_PCE_MASK                                  (0x00000002U)
#define EQEP_QCLR_PCE_SHIFT                                 (1U)
#define EQEP_QCLR_PCE_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_PCE_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_PCE_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_PHE_MASK                                  (0x00000004U)
#define EQEP_QCLR_PHE_SHIFT                                 (2U)
#define EQEP_QCLR_PHE_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_PHE_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_PHE_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_QDC_MASK                                  (0x00000008U)
#define EQEP_QCLR_QDC_SHIFT                                 (3U)
#define EQEP_QCLR_QDC_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_QDC_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_QDC_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_WTO_MASK                                  (0x00000010U)
#define EQEP_QCLR_WTO_SHIFT                                 (4U)
#define EQEP_QCLR_WTO_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_WTO_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_WTO_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_PCU_MASK                                  (0x00000020U)
#define EQEP_QCLR_PCU_SHIFT                                 (5U)
#define EQEP_QCLR_PCU_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_PCU_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_PCU_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_PCO_MASK                                  (0x00000040U)
#define EQEP_QCLR_PCO_SHIFT                                 (6U)
#define EQEP_QCLR_PCO_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_PCO_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_PCO_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_PCR_MASK                                  (0x00000080U)
#define EQEP_QCLR_PCR_SHIFT                                 (7U)
#define EQEP_QCLR_PCR_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_PCR_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_PCR_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_PCM_MASK                                  (0x00000100U)
#define EQEP_QCLR_PCM_SHIFT                                 (8U)
#define EQEP_QCLR_PCM_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_PCM_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_PCM_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_SEL_MASK                                  (0x00000200U)
#define EQEP_QCLR_SEL_SHIFT                                 (9U)
#define EQEP_QCLR_SEL_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_SEL_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_SEL_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_IEL_MASK                                  (0x00000400U)
#define EQEP_QCLR_IEL_SHIFT                                 (10U)
#define EQEP_QCLR_IEL_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_IEL_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_IEL_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_UTO_MASK                                  (0x00000800U)
#define EQEP_QCLR_UTO_SHIFT                                 (11U)
#define EQEP_QCLR_UTO_RESETVAL                              (0x00000000U)
#define EQEP_QCLR_UTO_EN_0X0                                (0x00000000U)
#define EQEP_QCLR_UTO_EN_0X1                                (0x00000001U)

#define EQEP_QCLR_RESETVAL                                  (0x00000000U)

/* QPOSCNT */

#define EQEP_QPOSCNT_QPOSCNT_MASK                           (0xFFFFFFFFU)
#define EQEP_QPOSCNT_QPOSCNT_SHIFT                          (0U)
#define EQEP_QPOSCNT_QPOSCNT_RESETVAL                       (0x00000000U)
#define EQEP_QPOSCNT_QPOSCNT_MAX                            (0xffffffffU)

#define EQEP_QPOSCNT_RESETVAL                               (0x00000000U)

/* QCAPCTL */

#define EQEP_QCAPCTL_UPPS_MASK                              (0x0000000FU)
#define EQEP_QCAPCTL_UPPS_SHIFT                             (0U)
#define EQEP_QCAPCTL_UPPS_RESETVAL                          (0x00000000U)
#define EQEP_QCAPCTL_UPPS_EN_0X4                            (0x00000004U)
#define EQEP_QCAPCTL_UPPS_EN_0XC                            (0x0000000cU)
#define EQEP_QCAPCTL_UPPS_EN_0XF                            (0x0000000fU)
#define EQEP_QCAPCTL_UPPS_EN_0XE                            (0x0000000eU)
#define EQEP_QCAPCTL_UPPS_EN_0XB                            (0x0000000bU)
#define EQEP_QCAPCTL_UPPS_EN_0X1                            (0x00000001U)
#define EQEP_QCAPCTL_UPPS_EN_0XD                            (0x0000000dU)
#define EQEP_QCAPCTL_UPPS_EN_0X8                            (0x00000008U)
#define EQEP_QCAPCTL_UPPS_EN_0XA                            (0x0000000aU)
#define EQEP_QCAPCTL_UPPS_EN_0X6                            (0x00000006U)
#define EQEP_QCAPCTL_UPPS_EN_0X9                            (0x00000009U)
#define EQEP_QCAPCTL_UPPS_EN_0X7                            (0x00000007U)
#define EQEP_QCAPCTL_UPPS_EN_0X0                            (0x00000000U)
#define EQEP_QCAPCTL_UPPS_EN_0X5                            (0x00000005U)
#define EQEP_QCAPCTL_UPPS_EN_0X2                            (0x00000002U)
#define EQEP_QCAPCTL_UPPS_EN_0X3                            (0x00000003U)
#define EQEP_QCAPCTL_UPPS_MAX                               (0x0000000fU)

#define EQEP_QCAPCTL_CCPS_MASK                              (0x00000070U)
#define EQEP_QCAPCTL_CCPS_SHIFT                             (4U)
#define EQEP_QCAPCTL_CCPS_RESETVAL                          (0x00000000U)
#define EQEP_QCAPCTL_CCPS_EN_0X6                            (0x00000006U)
#define EQEP_QCAPCTL_CCPS_EN_0X2                            (0x00000002U)
#define EQEP_QCAPCTL_CCPS_EN_0X1                            (0x00000001U)
#define EQEP_QCAPCTL_CCPS_EN_0X5                            (0x00000005U)
#define EQEP_QCAPCTL_CCPS_EN_0X4                            (0x00000004U)
#define EQEP_QCAPCTL_CCPS_EN_0X3                            (0x00000003U)
#define EQEP_QCAPCTL_CCPS_EN_0X7                            (0x00000007U)
#define EQEP_QCAPCTL_CCPS_EN_0X0                            (0x00000000U)

#define EQEP_QCAPCTL_CEN_MASK                               (0x00008000U)
#define EQEP_QCAPCTL_CEN_SHIFT                              (15U)
#define EQEP_QCAPCTL_CEN_RESETVAL                           (0x00000000U)
#define EQEP_QCAPCTL_CEN_EN_0X1                             (0x00000001U)
#define EQEP_QCAPCTL_CEN_EN_0X0                             (0x00000000U)

#define EQEP_QCAPCTL_RESETVAL                               (0x00000000U)

/* QPOSLAT */

#define EQEP_QPOSLAT_QPOSLAT_MASK                           (0xFFFFFFFFU)
#define EQEP_QPOSLAT_QPOSLAT_SHIFT                          (0U)
#define EQEP_QPOSLAT_QPOSLAT_RESETVAL                       (0x00000000U)
#define EQEP_QPOSLAT_QPOSLAT_MAX                            (0xffffffffU)

#define EQEP_QPOSLAT_RESETVAL                               (0x00000000U)

/* QPOSILAT */

#define EQEP_QPOSILAT_QPOSILAT_MASK                         (0xFFFFFFFFU)
#define EQEP_QPOSILAT_QPOSILAT_SHIFT                        (0U)
#define EQEP_QPOSILAT_QPOSILAT_RESETVAL                     (0x00000000U)
#define EQEP_QPOSILAT_QPOSILAT_MAX                          (0xffffffffU)

#define EQEP_QPOSILAT_RESETVAL                              (0x00000000U)

/* REVID */

#define EQEP_REVID_REV_MASK                                 (0xFFFFFFFFU)
#define EQEP_REVID_REV_SHIFT                                (0U)
#define EQEP_REVID_REV_RESETVAL                             (0x44d31103U)
#define EQEP_REVID_REV_MAX                                  (0xffffffffU)

#define EQEP_REVID_RESETVAL                                 (0x44d31103U)

/* QPOSSLAT */

#define EQEP_QPOSSLAT_QPOSSLAT_MASK                         (0xFFFFFFFFU)
#define EQEP_QPOSSLAT_QPOSSLAT_SHIFT                        (0U)
#define EQEP_QPOSSLAT_QPOSSLAT_RESETVAL                     (0x00000000U)
#define EQEP_QPOSSLAT_QPOSSLAT_MAX                          (0xffffffffU)

#define EQEP_QPOSSLAT_RESETVAL                              (0x00000000U)

/* QCTMR */

#define EQEP_QCTMR_QCTMR_MASK                               (0x0000FFFFU)
#define EQEP_QCTMR_QCTMR_SHIFT                              (0U)
#define EQEP_QCTMR_QCTMR_RESETVAL                           (0x00000000U)
#define EQEP_QCTMR_QCTMR_MAX                                (0x0000ffffU)

#define EQEP_QCTMR_RESETVAL                                 (0x00000000U)

/* QWDTMR */

#define EQEP_QWDTMR_QWDTMR_MASK                             (0x0000FFFFU)
#define EQEP_QWDTMR_QWDTMR_SHIFT                            (0U)
#define EQEP_QWDTMR_QWDTMR_RESETVAL                         (0x00000000U)
#define EQEP_QWDTMR_QWDTMR_MAX                              (0x0000ffffU)

#define EQEP_QWDTMR_RESETVAL                                (0x00000000U)

/* QCPRD */

#define EQEP_QCPRD_QCPRD_MASK                               (0x0000FFFFU)
#define EQEP_QCPRD_QCPRD_SHIFT                              (0U)
#define EQEP_QCPRD_QCPRD_RESETVAL                           (0x00000000U)
#define EQEP_QCPRD_QCPRD_MAX                                (0x0000ffffU)

#define EQEP_QCPRD_RESETVAL                                 (0x00000000U)

/* QDECCTL */

#define EQEP_QDECCTL_QSP_MASK                               (0x00000020U)
#define EQEP_QDECCTL_QSP_SHIFT                              (5U)
#define EQEP_QDECCTL_QSP_RESETVAL                           (0x00000000U)
#define EQEP_QDECCTL_QSP_EN_0X0                             (0x00000000U)
#define EQEP_QDECCTL_QSP_EN_0X1                             (0x00000001U)

#define EQEP_QDECCTL_QIP_MASK                               (0x00000040U)
#define EQEP_QDECCTL_QIP_SHIFT                              (6U)
#define EQEP_QDECCTL_QIP_RESETVAL                           (0x00000000U)
#define EQEP_QDECCTL_QIP_EN_0X0                             (0x00000000U)
#define EQEP_QDECCTL_QIP_EN_0X1                             (0x00000001U)

#define EQEP_QDECCTL_QBP_MASK                               (0x00000080U)
#define EQEP_QDECCTL_QBP_SHIFT                              (7U)
#define EQEP_QDECCTL_QBP_RESETVAL                           (0x00000000U)
#define EQEP_QDECCTL_QBP_EN_0X1                             (0x00000001U)
#define EQEP_QDECCTL_QBP_EN_0X0                             (0x00000000U)

#define EQEP_QDECCTL_QAP_MASK                               (0x00000100U)
#define EQEP_QDECCTL_QAP_SHIFT                              (8U)
#define EQEP_QDECCTL_QAP_RESETVAL                           (0x00000000U)
#define EQEP_QDECCTL_QAP_EN_0X0                             (0x00000000U)
#define EQEP_QDECCTL_QAP_EN_0X1                             (0x00000001U)

#define EQEP_QDECCTL_IGATE_MASK                             (0x00000200U)
#define EQEP_QDECCTL_IGATE_SHIFT                            (9U)
#define EQEP_QDECCTL_IGATE_RESETVAL                         (0x00000000U)
#define EQEP_QDECCTL_IGATE_EN_0X0                           (0x00000000U)
#define EQEP_QDECCTL_IGATE_EN_0X1                           (0x00000001U)

#define EQEP_QDECCTL_SWAP_MASK                              (0x00000400U)
#define EQEP_QDECCTL_SWAP_SHIFT                             (10U)
#define EQEP_QDECCTL_SWAP_RESETVAL                          (0x00000000U)
#define EQEP_QDECCTL_SWAP_EN_0X0                            (0x00000000U)
#define EQEP_QDECCTL_SWAP_EN_0X1                            (0x00000001U)

#define EQEP_QDECCTL_XCR_MASK                               (0x00000800U)
#define EQEP_QDECCTL_XCR_SHIFT                              (11U)
#define EQEP_QDECCTL_XCR_RESETVAL                           (0x00000000U)
#define EQEP_QDECCTL_XCR_EN_0X0                             (0x00000000U)
#define EQEP_QDECCTL_XCR_EN_0X1                             (0x00000001U)

#define EQEP_QDECCTL_SPSEL_MASK                             (0x00001000U)
#define EQEP_QDECCTL_SPSEL_SHIFT                            (12U)
#define EQEP_QDECCTL_SPSEL_RESETVAL                         (0x00000000U)
#define EQEP_QDECCTL_SPSEL_EN_0X1                           (0x00000001U)
#define EQEP_QDECCTL_SPSEL_EN_0X0                           (0x00000000U)

#define EQEP_QDECCTL_SOEN_MASK                              (0x00002000U)
#define EQEP_QDECCTL_SOEN_SHIFT                             (13U)
#define EQEP_QDECCTL_SOEN_RESETVAL                          (0x00000000U)
#define EQEP_QDECCTL_SOEN_EN_0X0                            (0x00000000U)
#define EQEP_QDECCTL_SOEN_EN_0X1                            (0x00000001U)

#define EQEP_QDECCTL_QSRC_MASK                              (0x0000C000U)
#define EQEP_QDECCTL_QSRC_SHIFT                             (14U)
#define EQEP_QDECCTL_QSRC_RESETVAL                          (0x00000000U)
#define EQEP_QDECCTL_QSRC_EN_0X1                            (0x00000001U)
#define EQEP_QDECCTL_QSRC_EN_0X2                            (0x00000002U)
#define EQEP_QDECCTL_QSRC_EN_0X3                            (0x00000003U)
#define EQEP_QDECCTL_QSRC_EN_0X0                            (0x00000000U)

#define EQEP_QDECCTL_RESETVAL                               (0x00000000U)

/* QUTMR */

#define EQEP_QUTMR_QUTMR_MASK                               (0xFFFFFFFFU)
#define EQEP_QUTMR_QUTMR_SHIFT                              (0U)
#define EQEP_QUTMR_QUTMR_RESETVAL                           (0x00000000U)
#define EQEP_QUTMR_QUTMR_MAX                                (0xffffffffU)

#define EQEP_QUTMR_RESETVAL                                 (0x00000000U)

/* QFRC */

#define EQEP_QFRC_PCE_MASK                                  (0x00000002U)
#define EQEP_QFRC_PCE_SHIFT                                 (1U)
#define EQEP_QFRC_PCE_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_PCE_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_PCE_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_PHE_MASK                                  (0x00000004U)
#define EQEP_QFRC_PHE_SHIFT                                 (2U)
#define EQEP_QFRC_PHE_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_PHE_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_PHE_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_QDC_MASK                                  (0x00000008U)
#define EQEP_QFRC_QDC_SHIFT                                 (3U)
#define EQEP_QFRC_QDC_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_QDC_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_QDC_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_WTO_MASK                                  (0x00000010U)
#define EQEP_QFRC_WTO_SHIFT                                 (4U)
#define EQEP_QFRC_WTO_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_WTO_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_WTO_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_PCU_MASK                                  (0x00000020U)
#define EQEP_QFRC_PCU_SHIFT                                 (5U)
#define EQEP_QFRC_PCU_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_PCU_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_PCU_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_PCO_MASK                                  (0x00000040U)
#define EQEP_QFRC_PCO_SHIFT                                 (6U)
#define EQEP_QFRC_PCO_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_PCO_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_PCO_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_PCR_MASK                                  (0x00000080U)
#define EQEP_QFRC_PCR_SHIFT                                 (7U)
#define EQEP_QFRC_PCR_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_PCR_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_PCR_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_PCM_MASK                                  (0x00000100U)
#define EQEP_QFRC_PCM_SHIFT                                 (8U)
#define EQEP_QFRC_PCM_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_PCM_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_PCM_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_SEL_MASK                                  (0x00000200U)
#define EQEP_QFRC_SEL_SHIFT                                 (9U)
#define EQEP_QFRC_SEL_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_SEL_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_SEL_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_IEL_MASK                                  (0x00000400U)
#define EQEP_QFRC_IEL_SHIFT                                 (10U)
#define EQEP_QFRC_IEL_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_IEL_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_IEL_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_UTO_MASK                                  (0x00000800U)
#define EQEP_QFRC_UTO_SHIFT                                 (11U)
#define EQEP_QFRC_UTO_RESETVAL                              (0x00000000U)
#define EQEP_QFRC_UTO_EN_0X0                                (0x00000000U)
#define EQEP_QFRC_UTO_EN_0X1                                (0x00000001U)

#define EQEP_QFRC_RESETVAL                                  (0x00000000U)

/* QPOSINIT */

#define EQEP_QPOSINIT_QPOSINIT_MASK                         (0xFFFFFFFFU)
#define EQEP_QPOSINIT_QPOSINIT_SHIFT                        (0U)
#define EQEP_QPOSINIT_QPOSINIT_RESETVAL                     (0x00000000U)
#define EQEP_QPOSINIT_QPOSINIT_MAX                          (0xffffffffU)

#define EQEP_QPOSINIT_RESETVAL                              (0x00000000U)

/* QEINT */

#define EQEP_QEINT_PCE_MASK                                 (0x00000002U)
#define EQEP_QEINT_PCE_SHIFT                                (1U)
#define EQEP_QEINT_PCE_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_PCE_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_PCE_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_PHE_MASK                                 (0x00000004U)
#define EQEP_QEINT_PHE_SHIFT                                (2U)
#define EQEP_QEINT_PHE_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_PHE_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_PHE_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_QDC_MASK                                 (0x00000008U)
#define EQEP_QEINT_QDC_SHIFT                                (3U)
#define EQEP_QEINT_QDC_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_QDC_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_QDC_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_WTO_MASK                                 (0x00000010U)
#define EQEP_QEINT_WTO_SHIFT                                (4U)
#define EQEP_QEINT_WTO_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_WTO_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_WTO_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_PCU_MASK                                 (0x00000020U)
#define EQEP_QEINT_PCU_SHIFT                                (5U)
#define EQEP_QEINT_PCU_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_PCU_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_PCU_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_PCO_MASK                                 (0x00000040U)
#define EQEP_QEINT_PCO_SHIFT                                (6U)
#define EQEP_QEINT_PCO_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_PCO_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_PCO_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_PCR_MASK                                 (0x00000080U)
#define EQEP_QEINT_PCR_SHIFT                                (7U)
#define EQEP_QEINT_PCR_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_PCR_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_PCR_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_PCM_MASK                                 (0x00000100U)
#define EQEP_QEINT_PCM_SHIFT                                (8U)
#define EQEP_QEINT_PCM_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_PCM_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_PCM_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_SEL_MASK                                 (0x00000200U)
#define EQEP_QEINT_SEL_SHIFT                                (9U)
#define EQEP_QEINT_SEL_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_SEL_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_SEL_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_IEL_MASK                                 (0x00000400U)
#define EQEP_QEINT_IEL_SHIFT                                (10U)
#define EQEP_QEINT_IEL_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_IEL_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_IEL_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_UTO_MASK                                 (0x00000800U)
#define EQEP_QEINT_UTO_SHIFT                                (11U)
#define EQEP_QEINT_UTO_RESETVAL                             (0x00000000U)
#define EQEP_QEINT_UTO_EN_0X1                               (0x00000001U)
#define EQEP_QEINT_UTO_EN_0X0                               (0x00000000U)

#define EQEP_QEINT_RESETVAL                                 (0x00000000U)

/* QUPRD */

#define EQEP_QUPRD_QUPRD_MASK                               (0xFFFFFFFFU)
#define EQEP_QUPRD_QUPRD_SHIFT                              (0U)
#define EQEP_QUPRD_QUPRD_RESETVAL                           (0x00000000U)
#define EQEP_QUPRD_QUPRD_MAX                                (0xffffffffU)

#define EQEP_QUPRD_RESETVAL                                 (0x00000000U)

/* QEPCTL */

#define EQEP_QEPCTL_WDE_MASK                                (0x00000001U)
#define EQEP_QEPCTL_WDE_SHIFT                               (0U)
#define EQEP_QEPCTL_WDE_RESETVAL                            (0x00000000U)
#define EQEP_QEPCTL_WDE_EN_0X1                              (0x00000001U)
#define EQEP_QEPCTL_WDE_EN_0X0                              (0x00000000U)

#define EQEP_QEPCTL_UTE_MASK                                (0x00000002U)
#define EQEP_QEPCTL_UTE_SHIFT                               (1U)
#define EQEP_QEPCTL_UTE_RESETVAL                            (0x00000000U)
#define EQEP_QEPCTL_UTE_EN_0X1                              (0x00000001U)
#define EQEP_QEPCTL_UTE_EN_0X0                              (0x00000000U)

#define EQEP_QEPCTL_QCLM_MASK                               (0x00000004U)
#define EQEP_QEPCTL_QCLM_SHIFT                              (2U)
#define EQEP_QEPCTL_QCLM_RESETVAL                           (0x00000000U)
#define EQEP_QEPCTL_QCLM_EN_0X1                             (0x00000001U)
#define EQEP_QEPCTL_QCLM_EN_0X0                             (0x00000000U)

#define EQEP_QEPCTL_PHEN_MASK                               (0x00000008U)
#define EQEP_QEPCTL_PHEN_SHIFT                              (3U)
#define EQEP_QEPCTL_PHEN_RESETVAL                           (0x00000000U)
#define EQEP_QEPCTL_PHEN_EN_0X0                             (0x00000000U)
#define EQEP_QEPCTL_PHEN_EN_0X1                             (0x00000001U)

#define EQEP_QEPCTL_IEL_MASK                                (0x00000030U)
#define EQEP_QEPCTL_IEL_SHIFT                               (4U)
#define EQEP_QEPCTL_IEL_RESETVAL                            (0x00000000U)
#define EQEP_QEPCTL_IEL_EN_0X1                              (0x00000001U)
#define EQEP_QEPCTL_IEL_EN_0X3                              (0x00000003U)
#define EQEP_QEPCTL_IEL_EN_0X0                              (0x00000000U)
#define EQEP_QEPCTL_IEL_EN_0X2                              (0x00000002U)

#define EQEP_QEPCTL_SEL_MASK                                (0x00000040U)
#define EQEP_QEPCTL_SEL_SHIFT                               (6U)
#define EQEP_QEPCTL_SEL_RESETVAL                            (0x00000000U)
#define EQEP_QEPCTL_SEL_EN_0X0                              (0x00000000U)
#define EQEP_QEPCTL_SEL_EN_0X1                              (0x00000001U)

#define EQEP_QEPCTL_SWI_MASK                                (0x00000080U)
#define EQEP_QEPCTL_SWI_SHIFT                               (7U)
#define EQEP_QEPCTL_SWI_RESETVAL                            (0x00000000U)
#define EQEP_QEPCTL_SWI_EN_0X1                              (0x00000001U)
#define EQEP_QEPCTL_SWI_EN_0X0                              (0x00000000U)

#define EQEP_QEPCTL_IEI_MASK                                (0x00000300U)
#define EQEP_QEPCTL_IEI_SHIFT                               (8U)
#define EQEP_QEPCTL_IEI_RESETVAL                            (0x00000000U)
#define EQEP_QEPCTL_IEI_EN_0X1                              (0x00000001U)
#define EQEP_QEPCTL_IEI_EN_0X3                              (0x00000003U)
#define EQEP_QEPCTL_IEI_EN_0X2                              (0x00000002U)
#define EQEP_QEPCTL_IEI_EN_0X0                              (0x00000000U)

#define EQEP_QEPCTL_SEI_MASK                                (0x00000C00U)
#define EQEP_QEPCTL_SEI_SHIFT                               (10U)
#define EQEP_QEPCTL_SEI_RESETVAL                            (0x00000000U)
#define EQEP_QEPCTL_SEI_EN_0X1                              (0x00000001U)
#define EQEP_QEPCTL_SEI_EN_0X2                              (0x00000002U)
#define EQEP_QEPCTL_SEI_EN_0X0                              (0x00000000U)
#define EQEP_QEPCTL_SEI_EN_0X3                              (0x00000003U)

#define EQEP_QEPCTL_PCRM_MASK                               (0x00003000U)
#define EQEP_QEPCTL_PCRM_SHIFT                              (12U)
#define EQEP_QEPCTL_PCRM_RESETVAL                           (0x00000000U)
#define EQEP_QEPCTL_PCRM_EN_0X1                             (0x00000001U)
#define EQEP_QEPCTL_PCRM_EN_0X0                             (0x00000000U)
#define EQEP_QEPCTL_PCRM_EN_0X2                             (0x00000002U)
#define EQEP_QEPCTL_PCRM_EN_0X3                             (0x00000003U)

#define EQEP_QEPCTL_FREE_SOFT_MASK                          (0x0000C000U)
#define EQEP_QEPCTL_FREE_SOFT_SHIFT                         (14U)
#define EQEP_QEPCTL_FREE_SOFT_RESETVAL                      (0x00000000U)
#define EQEP_QEPCTL_FREE_SOFT_EN_0X3                        (0x00000003U)
#define EQEP_QEPCTL_FREE_SOFT_EN_0X0                        (0x00000000U)
#define EQEP_QEPCTL_FREE_SOFT_EN_0X2                        (0x00000002U)
#define EQEP_QEPCTL_FREE_SOFT_EN_0X1                        (0x00000001U)

#define EQEP_QEPCTL_RESETVAL                                (0x00000000U)

/* QPOSCTL */

#define EQEP_QPOSCTL_PCSPW_MASK                             (0x00000FFFU)
#define EQEP_QPOSCTL_PCSPW_SHIFT                            (0U)
#define EQEP_QPOSCTL_PCSPW_RESETVAL                         (0x00000000U)
#define EQEP_QPOSCTL_PCSPW_EN_0X1                           (0x00000001U)
#define EQEP_QPOSCTL_PCSPW_EN_0XFFF                         (0x00000fffU)
#define EQEP_QPOSCTL_PCSPW_EN_0X2                           (0x00000002U)
#define EQEP_QPOSCTL_PCSPW_EN_0X0                           (0x00000000U)
#define EQEP_QPOSCTL_PCSPW_MAX                              (0x00000fffU)

#define EQEP_QPOSCTL_PCE_MASK                               (0x00001000U)
#define EQEP_QPOSCTL_PCE_SHIFT                              (12U)
#define EQEP_QPOSCTL_PCE_RESETVAL                           (0x00000000U)
#define EQEP_QPOSCTL_PCE_EN_0X0                             (0x00000000U)
#define EQEP_QPOSCTL_PCE_EN_0X1                             (0x00000001U)

#define EQEP_QPOSCTL_PCPOL_MASK                             (0x00002000U)
#define EQEP_QPOSCTL_PCPOL_SHIFT                            (13U)
#define EQEP_QPOSCTL_PCPOL_RESETVAL                         (0x00000000U)
#define EQEP_QPOSCTL_PCPOL_EN_0X0                           (0x00000000U)
#define EQEP_QPOSCTL_PCPOL_EN_0X1                           (0x00000001U)

#define EQEP_QPOSCTL_PCLOAD_MASK                            (0x00004000U)
#define EQEP_QPOSCTL_PCLOAD_SHIFT                           (14U)
#define EQEP_QPOSCTL_PCLOAD_RESETVAL                        (0x00000000U)
#define EQEP_QPOSCTL_PCLOAD_EN_0X1                          (0x00000001U)
#define EQEP_QPOSCTL_PCLOAD_EN_0X0                          (0x00000000U)

#define EQEP_QPOSCTL_PCSHDW_MASK                            (0x00008000U)
#define EQEP_QPOSCTL_PCSHDW_SHIFT                           (15U)
#define EQEP_QPOSCTL_PCSHDW_RESETVAL                        (0x00000000U)
#define EQEP_QPOSCTL_PCSHDW_EN_0X1                          (0x00000001U)
#define EQEP_QPOSCTL_PCSHDW_EN_0X0                          (0x00000000U)

#define EQEP_QPOSCTL_RESETVAL                               (0x00000000U)


#endif /* INCLUDE_ER301_EQEP_H_ */
