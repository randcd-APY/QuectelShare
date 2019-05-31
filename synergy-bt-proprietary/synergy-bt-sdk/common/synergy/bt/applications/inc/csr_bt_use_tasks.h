#ifndef CSR_BT_USE_TASKS_H__
#define CSR_BT_USE_TASKS_H__

/****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"

#ifdef __cplusplus
extern "C" {
#endif

/* We always want to run the profiles as a part of the "stack" */
#define CSR_BT_RUN_TASK_DM                            (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_L2CAP_PROXY                   (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_RFCOMM_PROXY                  (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SDP                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_DM_HCI                        (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_DM_ACL                        (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_L2CAP                         (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_RFCOMM                        (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SDP_L2CAP                     (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_CM                            (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_HFG                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_HF                            (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SC                            (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_DG                            (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_DUNC                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_DUNC                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_OPC                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_OPS                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SMLC                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SMLS                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SPP                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_PAC                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_PAS                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_BNEP                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_BSL                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_BIPC                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_BIPS                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_FTC                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_FTS                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_MAPC                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_MAPS                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_BPPC                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SYNCC                         (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SYNCS                         (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_AV                            (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_AVRCP                         (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SAPS                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SAPC                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_GNSS_CLIENT                   (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_GNSS_SERVER                   (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_SD                            (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_BPPS                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_HCRP                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_HIDH                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_HIDD                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_JSR82                         (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_MCAP                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_HDP                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_AMPM                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_PHDC_MGR                      (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_PHDC_AG                       (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_ATT                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_GATT                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_HOGH                          (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_PROX_SRV                      (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_THERM_SRV                     (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_ASM                           (CSR_BT_RUN_STACK)
#define CSR_BT_RUN_TASK_AVRCP_IMAGING                 (CSR_BT_RUN_STACK)

/* The following tasks are Synergy frame work tasks */
#define CSR_RUN_TASK_BCCMD                            (CSR_BT_RUN_STACK)
#ifdef CSR_USE_QCA_CHIP
#define CSR_RUN_TASK_QCMD                             (CSR_BT_RUN_STACK)
#endif
#define CSR_RUN_TASK_HQ                               (CSR_BT_RUN_STACK)
#define CSR_RUN_TASK_TM_BLUECORE                      (CSR_BT_RUN_STACK)
#define CSR_RUN_TASK_UI                               (CSR_BT_RUN_APP)
#define CSR_RUN_TASK_APP                              (CSR_BT_RUN_APP)

/* The following profiles are "modem helpers" */
#define CSR_BT_RUN_TASK_AT                            (CSR_BT_RUN_MODEM)
#define CSR_BT_RUN_TASK_IWU                           (CSR_BT_RUN_MODEM)

/* These are IP stack helpers */
#define CSR_BT_RUN_TASK_IP                            1
#define CSR_BT_RUN_TASK_ICMP                          1
#define CSR_BT_RUN_TASK_UDP                           1
#define CSR_BT_RUN_TASK_DHCP                          1
#define CSR_BT_RUN_TASK_TFTP                          1
#define CSR_BT_RUN_TASK_CTRL                          1

/* This is a PPP helper */
#define CSR_BT_RUN_TASK_PPP                           (CSR_BT_RUN_PPP)

/* And these two usually comprises the actuaon "application" */
#define CSR_BT_RUN_TASK_EXTRA_TASK                    (CSR_BT_RUN_APP)
#define CSR_BT_RUN_TASK_TEST_TASK                     (CSR_BT_RUN_APP)


#ifdef __cplusplus
}
#endif

#endif
