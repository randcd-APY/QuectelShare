#ifndef CSR_BT_DECLARE_TASKS_H__
#define CSR_BT_DECLARE_TASKS_H__

/****************************************************************************

Copyright (c) 2011-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"
#include "csr_time.h"
#ifdef CSR_LOG_ENABLE
#include "csr_log.h"
#endif
#include "csr_sched_init.h"
#include "csr_hci_task.h"
#include "csr_bccmd_task.h"
#ifdef CSR_USE_QCA_CHIP
#include "csr_qvsc_task.h"
#endif
#include "csr_fp_task.h"
#include "csr_hq_task.h"
#include "csr_vm_task.h"
#include "csr_tm_bluecore_task.h"
#include "csr_am_task.h"
#ifdef CSR_USE_DSPM
#include "csr_dspm_task.h"
#endif
#ifdef CSR_HYDRA_SSD
#include "csr_ssd_task.h"
#endif
#include "csr_bt_use_tasks.h"
#include "csr_bt_tasks.h"

#define ID_STACK    0
#define ID_APP      1

#ifdef __cplusplus
extern "C" {
#endif

/* Synergy Framework */
extern CsrSchedQid CSR_HCI_IFACEQUEUE;
extern CsrSchedQid CSR_TM_BLUECORE_IFACEQUEUE;
extern CsrSchedQid CSR_VM_IFACEQUEUE;
extern CsrSchedQid CSR_FP_IFACEQUEUE;
extern CsrSchedQid CSR_BCCMD_IFACEQUEUE;
#ifdef CSR_USE_QCA_CHIP
extern CsrSchedQid CSR_QVSC_IFACEQUEUE;
#endif
extern CsrSchedQid CSR_HQ_IFACEQUEUE;
#ifdef CSR_USE_DSPM
extern CsrSchedQid CSR_DSPM_IFACEQUEUE;
#endif
#ifdef CSR_HYDRA_SSD
extern CsrSchedQid CSR_SSD_IFACEQUEUE;
#endif
extern CsrSchedQid CSR_UI_IFACEQUEUE;
extern CsrSchedQid CSR_APP_IFACEQUEUE;

/* Synergy Bluetooth */
extern CsrSchedQid DM_IFACEQUEUE;
extern CsrSchedQid DM_ACLQUEUE;
extern CsrSchedQid DM_HCI_IFACEQUEUE;
extern CsrSchedQid L2CAP_IFACEQUEUE;
extern CsrSchedQid RFCOMM_IFACEQUEUE;
extern CsrSchedQid SDP_L2CAP_IFACEQUEUE;
extern CsrSchedQid SDP_IFACEQUEUE;
extern CsrSchedQid ATT_IFACEQUEUE;
extern CsrSchedQid CSR_BT_CM_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_DG_IFACEQUEUE;
extern CsrSchedQid CSR_BT_AT_IFACEQUEUE;
extern CsrSchedQid CSR_BT_OPS_IFACEQUEUE;
extern CsrSchedQid CSR_BT_OPC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_MAPC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_MAPC_EXTRA_IFACEQUEUE;
extern CsrSchedQid CSR_BT_MAPS_IFACEQUEUE;
extern CsrSchedQid CSR_BT_MAPS_EXTRA_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SYNCC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SYNCS_IFACEQUEUE;
extern CsrSchedQid CSR_BT_IWU_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SPP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SPP_EXTRA_IFACEQUEUE;
extern CsrSchedQid CSR_BT_HFG_IFACEQUEUE;
extern CsrSchedQid CSR_BT_HF_IFACEQUEUE;
extern CsrSchedQid CSR_BT_FTS_IFACEQUEUE;
extern CsrSchedQid CSR_BT_BNEP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_BSL_IFACEQUEUE;
extern CsrSchedQid CSR_BT_IP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_ICMP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_UDP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_DHCP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_TFTP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_CTRL_IFACEQUEUE;
extern CsrSchedQid CSR_BT_BIPC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_FTC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_PPP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_BPPC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_AV_IFACEQUEUE;
extern CsrSchedQid CSR_BT_AVRCP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SAPS_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SAPC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SD_IFACEQUEUE;
extern CsrSchedQid CSR_BT_HIDH_IFACEQUEUE;
extern CsrSchedQid CSR_BT_BPPS_IFACEQUEUE;
extern CsrSchedQid CSR_BT_HCRP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_BIPS_IFACEQUEUE;
extern CsrSchedQid CSR_BT_BIPS_EXTRA_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SMLC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SMLS_IFACEQUEUE;
extern CsrSchedQid CSR_BT_DUNC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_JSR82_IFACEQUEUE;
extern CsrSchedQid CSR_BT_HIDD_IFACEQUEUE;
extern CsrSchedQid CSR_BT_PAC_IFACEQUEUE;
extern CsrSchedQid CSR_BT_PAS_IFACEQUEUE;
extern CsrSchedQid CSR_BT_MCAP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_HDP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_AMPM_IFACEQUEUE;
extern CsrSchedQid CSR_BT_PHDC_MGR_IFACEQUEUE;
extern CsrSchedQid CSR_BT_PHDC_AG_IFACEQUEUE;
extern CsrSchedQid CSR_BT_GATT_IFACEQUEUE;
extern CsrSchedQid CSR_BT_GNSS_CLIENT_IFACEQUEUE;
extern CsrSchedQid CSR_BT_GNSS_SERVER_IFACEQUEUE;
extern CsrSchedQid TESTQUEUE;
extern CsrSchedQid CSR_BT_EXTRA_IFACEQUEUE;

extern CsrSchedQid CSR_IP_IFACEQUEUE;

/* Synergy BT Application queues */
extern CsrSchedQid CSR_BT_GAP_APP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_AV_APP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_AVRCP_APP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SPP_APP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_SPP_APP_EXTRA_IFACEQUEUE;
extern CsrSchedQid CSR_BT_HFG_APP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_PHDC_AG_APP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_PHDC_MGR_APP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_AMPWIFI_APP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_GNSS_CLIENT_APP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_GNSS_SERVER_APP_IFACEQUEUE;
extern CsrSchedQid CSR_BT_MPAA_IFACEQUEUE;
extern CsrSchedQid CSR_BT_AUDIO_APP_IFACEQUEUE;

extern CsrSchedQid CSR_BT_PAC_APP_IFACEQUEUE;

/* Task initialisation helpers */
extern void CsrBtAppTaskInitBt(void *data);
extern void CsrBtAppTaskInitFw(void *data);


#ifdef __cplusplus
}
#endif

#endif
