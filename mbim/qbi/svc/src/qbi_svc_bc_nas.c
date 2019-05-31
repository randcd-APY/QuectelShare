/*!
  @file
  qbi_svc_bc_nas.c

  @brief
  Functionality specific to the Network Access Stratum used by the Basic
  Connectivity device service.

  @details
  This file includes the implementation of the following Basic Connectivity
  CIDs: VISIBLE_PROVIDERS, REGISTER_STATE, PACKET_SERVICE, SIGNAL_STATE, and
  EMERGENCY_MODE.
*/

/*=============================================================================

  Copyright (c) 2012-2014, 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
01/04/18  nk   Moved common funtions and structures to header
11/16/17  rv   Allow Manual NW Sacn when data call is active
10/30/17  mm   Fixed issue related to register state failure
01/28/14  bd   Use CSG ID in PLMN name queries for REGISTER_STATE
11/18/13  bd   Combine PLMN name and SPN per 3GPP 22.101 A.4 (NV configurable)
10/16/13  bd   Incorporate data system status in PacketServiceState
09/27/13  bd   Use QMI_NAS_GET_PLMN_NAME for VISIBLE_PROVIDERS/ATDS_OPERATORS
07/25/13  bd   Add support for Sprint ERI and international roaming list
06/27/13  bd   Rework signal reporting
06/07/13  bd   Add support for Verizon ERI
04/16/13  bd   Add QMI indication (de)registration support to ATDS
04/10/13  bd   Send SIGNAL_STATE events based on UI signal bar thresholds
03/13/13  bd   Provide current RAT in network name queries for REGISTER_STATE
03/12/13  bd   Add QMI indication (de)registration support
02/22/13  bd   Limit time spent in cache only mode
10/30/12  bd   Report CS-only reject cause codes via REGISTER_STATE
10/18/12  bd   Add support for TD-SCDMA data class
08/30/12  bd   Ignore is_pref_data_path field in QMI NAS SYS_INFO messages;
               change internal terminology to "primary RAT" to avoid confusion
08/27/12  bd   Add support for new network scan result TLV
07/02/12  bd   Switch from SERVING_SYSTEM to SYS_INFO QMI_NAS messages
06/18/12  bd   Add workaround for WHCK network scan in automatic mode on
               multimode target (modem now requires 3GPP-only mode)
06/08/12  bd   Updates to registration detection logic for REGISTER_STATE set
06/01/12  bd   Rework REGISTER_STATE storage to only maintain net_sel_pref
05/30/12  bd   Split NAS functionality into new Basic Connectivity sub-module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_bc.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_svc_bc_nas.h"
#include "qbi_svc_bc_sim.h"

#include "qbi_svc_atds.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_msg_mbim.h"
#include "qbi_nv_store.h"
#include "qbi_svc.h"
#include "qbi_task.h"
#include "qbi_txn.h"
#include "qbi_qmi.h"
#include "qbi_qmi_defs.h"
#include "qbi_qmi_txn.h"

#include "network_access_service_v01.h"
#include "wireless_data_service_v01.h"

/*=============================================================================

  Private Macros

=============================================================================*/

/*=============================================================================

  Private Constants

=============================================================================*/

/*! Manual PS attach timeout */
#define QBI_SVC_BC_NAS_PS_ATTACH_TIMEOUT_MS (30 * 1000)

/*! Manual registration short timeout - used to return more quickly in cases
    where the modem completes the request without detach */
#define QBI_SVC_BC_NAS_REGISTER_SHORT_TIMEOUT_MS (10 * 1000)

/*! Manual registration long timeout - used to return PROVIDER_NOT_VISIBLE if we
    couldn't register to the network in the time expected by the host */
#define QBI_SVC_BC_NAS_REGISTER_LONG_TIMEOUT_MS  (38 * 1000)

/*! Extended timeout for network scan */
#define QBI_SVC_BC_NAS_VISIBLE_PROVIDERS_TIMEOUT_MS (600 * 1000)

/*! Delay between temporarily setting the mode preference and issuing a network
    scan request */
#define QBI_SVC_BC_NAS_VISIBLE_PROVIDERS_MODE_PREF_DELAY_MS (3000)

/*! Multiplication factor between 3GPP RXQUAL and the value reported by QMI */
#define QBI_SVC_BC_NAS_GSM_RXQUAL_FACTOR (100)

/*! Maximum time we will spend in cache only mode */
#define QBI_SVC_BC_NAS_CACHE_ONLY_TIME_LIMIT_MS (30000)

/* MBIM_CID_SIGNAL_STATE set request values sent by a Windows 8 host */
#define QBI_SVC_BC_NAS_ERROR_RATE_THRESHOLD_WINDOWS_8 \
  (QBI_SVC_BC_SIGNAL_STATE_ERROR_RATE_THRESHOLD_DISABLED)
#define QBI_SVC_BC_NAS_RSSI_INTERVAL_WINDOWS_8  (30)
#define QBI_SVC_BC_NAS_RSSI_THRESHOLD_WINDOWS_8 \
  (QBI_SVC_BC_SIGNAL_STATE_USE_DEFAULT)

/* MBIM_CID_SIGNAL_STATE set request values sent by a Windows 8.1 host */
#define QBI_SVC_BC_NAS_ERROR_RATE_THRESHOLD_WINDOWS_8_1 \
  (QBI_SVC_BC_SIGNAL_STATE_ERROR_RATE_THRESHOLD_DISABLED)
#define QBI_SVC_BC_NAS_RSSI_INTERVAL_WINDOWS_8_1  (60)
#define QBI_SVC_BC_NAS_RSSI_THRESHOLD_WINDOWS_8_1 (1)

/* Network reject cause codes allowing retry (refer to 3GPP TS 24.008) */
#define QBI_SVC_BC_NAS_REJECT_CAUSE_MS_IDENTITY_CANNOT_BE_DERIVED_BY_NW (9)
#define QBI_SVC_BC_NAS_REJECT_CAUSE_IMPLICITLY_DETACHED                 (10)
#define QBI_SVC_BC_NAS_REJECT_CAUSE_NO_SUITABLE_CELLS_IN_LAI            (15)

/* Network reject cause codes that cause the SIM to be marked as invalid */
#define QBI_SVC_BC_NAS_REJECT_CAUSE_IMSI_UNKNOWN_IN_HLR                    (2)
#define QBI_SVC_BC_NAS_REJECT_CAUSE_ILLEGAL_MS                             (3)
#define QBI_SVC_BC_NAS_REJECT_CAUSE_ILLEGAL_ME                             (6)
#define QBI_SVC_BC_NAS_REJECT_CAUSE_GPRS_SERVICES_NOT_ALLOWED              (7)
#define QBI_SVC_BC_NAS_REJECT_CAUSE_GPRS_AND_NON_GPRS_SERVICES_NOT_ALLOWED (8)

/*! @} */

/*! @addtogroup ERI_SPR
    @brief Definitions used in parsing the Sprint ERI text file and
    international roaming text file.
    @{ */

/*! MCC-MNC of the HPLMN ID for Sprint */
#define QBI_SVC_BC_NAS_HPLMN_MCC_SPR (310)
#define QBI_SVC_BC_NAS_HPLMN_MNC_SPR (120)

/*! @} */

/*! Default signal strength mappings based on the guidelines for WP8 */
static const qbi_svc_bc_nas_signal_mapping_s
  qbi_svc_bc_nas_default_signal_mappings[QBI_SVC_BC_NAS_SIGNAL_METRIC_MAX] =
{
  {{ /* QBI_SVC_BC_NAS_SIGNAL_METRIC_GSM_RSSI */
    { -80,  871,   -9323},
    { -89, 1125,   11125},
    { -98, 1125,   11250},
    {-104, 1000,   -1000},
    {-113,  375,  -66625}
  }},
  {{ /* QBI_SVC_BC_NAS_SIGNAL_METRIC_UMTS_RSCP */
    { -80,  491,  -39727},
    { -90, 1000,    1000},
    {-100, 1000,    1000},
    {-106, 1000,    1000},
    {-120,  231,  -81308}
  }},
  {{ /* QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSRP */
    { -85,  675,  -21625},
    { -95, 1000,    6000},
    {-105, 1000,    6000},
    {-115,  556,  -41111},
    {-140,  125,  -91500}
  }},
  {{ /* QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSSNR */
    {  13, 1688, -101640},
    {   5, 1286,  -95432},
    {   1, 3000, -102000},
    {  -3, 1667, -100000},
    { -20,  188, -105250}
  }},
  {{ /* QBI_SVC_BC_NAS_SIGNAL_METRIC_CDMA_RSSI */
    { -75,  509,  -40792},
    { -85, 1000,   -4000},
    { -95, 1000,   -4000},
    {-100, 1250,   20000},
    {-105,  750,  -30250}
  }},
  {{ /* QBI_SVC_BC_NAS_SIGNAL_METRIC_HDR_RSSI */
    { -65,  529,  -44588},
    { -75, 1000,  -14000},
    { -90,  643,  -41143},
    {-105,  357,  -67500},
    {-118,  250,  -79500}
  }}
};

/*=============================================================================

  Private Enums

=============================================================================*/

/*! Indicates the primary RAT, i.e. the one we select for supplying registration
    information to the host */
typedef enum {
  QBI_SVC_BC_NAS_PRIMARY_RAT_NONE = 0,
  QBI_SVC_BC_NAS_PRIMARY_RAT_CDMA,
  QBI_SVC_BC_NAS_PRIMARY_RAT_HDR,
  QBI_SVC_BC_NAS_PRIMARY_RAT_GSM,
  QBI_SVC_BC_NAS_PRIMARY_RAT_WCDMA,
  QBI_SVC_BC_NAS_PRIMARY_RAT_LTE,
  QBI_SVC_BC_NAS_PRIMARY_RAT_TDSCDMA
} qbi_svc_bc_nas_primary_rat_e;

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! Serving system information extracted from QMI_NAS_GET_SYS_INFO_RESP
    or QMI_NAS_SYS_INFO_IND. */
typedef struct {
  /*! Radio access technology containing the primary RAT. In the case of
      service on multiple RATs, we only pay attention to this one. */
  qbi_svc_bc_nas_primary_rat_e primary_rat;

  /*! Service status on the primary RAT */
  nas_service_status_enum_type_v01 srv_status;

  /*! Pointer to RAT-specific information for the primary RAT */
  union {
    const nas_cdma_sys_info_type_v01    *cdma;
    const nas_hdr_sys_info_type_v01     *hdr;
    const nas_gsm_sys_info_type_v01     *gsm;
    const nas_wcdma_sys_info_type_v01   *wcdma;
    const nas_lte_sys_info_type_v01     *lte;
    const nas_tdscdma_sys_info_type_v01 *tdscdma;
  } rat;

  /*! Pointer to CDMA-specific system information. Only valid when in hybrid
      mode, HDR is the primary RAT, and CDMA service is available */
  nas_service_status_enum_type_v01  hybrid_srv_status;
  const nas_cdma_sys_info_type_v01 *hybrid_cdma_sys_info;

  /*! Pointer to common system information structure that is included inside the
      RAT-specific structure. May be NULL if primary_rat is NONE */
  const nas_common_sys_info_type_v01 *common;

  /*! Pointer to common 3GPP information included inside the RAT-specific
      structure */
  const nas_3gpp_only_sys_info_type_v01 *common_3gpp;

  /*! Indicates whether we have information about the SIM's reject state */
  boolean sim_rej_info_valid;

  /*! SIM reject information - contains valid data only if sim_rej_info_valid is
      TRUE */
  nas_sim_rej_info_enum_type_v01 sim_rej_info;

  /*! Pointer to Closed Subscriber Group (CSG, e.g. femtocell) information, or
      NULL if not available/relevant */
  const nas_csg_info_type_v01 *csg_info;
} qbi_svc_bc_nas_sys_info_s;

/*! Network selection preference plus PCS digit flag, used with
    QBI_NV_STORE_CFG_ITEM_NET_SEL_PREF */
typedef struct {
  nas_net_sel_pref_type_v01 net_sel_pref;
  uint8_t                   mnc_includes_pcs_digit;
} qbi_svc_bc_nas_net_sel_pref_cfg_s;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static boolean qbi_svc_bc_nas_reject_cause_allows_retry
(
  uint32 nw_error
);

static void qbi_svc_bc_nas_sys_info_extract_common
(
  qbi_svc_bc_nas_sys_info_s        *sys_info,
  boolean                           cdma_srv_status_info_valid,
  nas_service_status_enum_type_v01  cdma_srv_status,
  boolean                           cdma_sys_info_valid,
  const nas_cdma_sys_info_type_v01 *cdma_sys_info,
  boolean                           sim_rej_info_valid,
  nas_sim_rej_info_enum_type_v01    sim_rej_info
);

static void qbi_svc_bc_nas_sys_info_extract_ind
(
  qbi_svc_bc_nas_sys_info_s      *sys_info,
  const nas_sys_info_ind_msg_v01 *qmi_ind
);

static void qbi_svc_bc_nas_sys_info_extract_rsp
(
  qbi_svc_bc_nas_sys_info_s           *sys_info,
  const nas_get_sys_info_resp_msg_v01 *qmi_rsp
);

static qbi_svc_bc_nas_primary_rat_e qbi_svc_bc_nas_sys_info_get_primary_rat
(
  uint8_t                                   cdma_srv_status_info_valid,
  const nas_3gpp2_srv_status_info_type_v01 *cdma_srv_status_info,
  uint8_t                                   hdr_srv_status_info_valid,
  const nas_3gpp2_srv_status_info_type_v01 *hdr_srv_status_info,
  uint8_t                                   gsm_srv_status_info_valid,
  const nas_3gpp_srv_status_info_type_v01  *gsm_srv_status_info,
  uint8_t                                   wcdma_srv_status_info_valid,
  const nas_3gpp_srv_status_info_type_v01  *wcdma_srv_status_info,
  uint8_t                                   lte_srv_status_info_valid,
  const nas_3gpp_srv_status_info_type_v01  *lte_srv_status_info,
  uint8_t                                   tdscdma_srv_status_info_valid,
  const nas_3gpp_srv_status_info_type_v01  *tdscdma_srv_status_info
);

static boolean qbi_svc_bc_nas_sys_info_is_ps_attached
(
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

static boolean qbi_svc_bc_nas_sys_info_is_reject_cause_valid
(
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

static qbi_svc_action_e qbi_svc_bc_nas_sys_info_qmi_ind_reg
(
  qbi_txn_s *txn,
  boolean    enable_inds
);

static qbi_svc_action_e qbi_svc_bc_nas_sys_info_qmi_ind_reg_nas03_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_nas_send_packet_service_then_register_state_events
(
  qbi_ctx_s *ctx
);

static void qbi_svc_bc_nas_send_packet_service_then_register_state_events_completion_cb
(
  const qbi_txn_s *txn
);

static void qbi_svc_bc_nas_spoof_detached_deregistered_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

/*! @addtogroup MBIM_CID_VISIBLE_PROVIDERS
    @{ */

static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_build_nas21_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_nas21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_nas_visible_providers_q_need_mode_pref_workaround
(
  const qbi_ctx_s *ctx
);

static boolean qbi_svc_bc_nas_visible_providers_q_prepare_info
(
  qbi_txn_s                                          *txn,
  const nas_3gpp_network_info_type_v01               *ntw_info,
  uint32                                              ntw_info_len,
  const nas_mnc_pcs_digit_include_status_type_v01    *pcs_digit_info,
  const nas_network_radio_access_technology_type_v01 *rat_info,
  boolean                                             skip_dups
);

static void qbi_svc_bc_nas_visible_providers_q_restore_mode_pref_completion_cb
(
  qbi_txn_s *completed_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_restore_mode_pref_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_set_mode_pref
(
  qbi_txn_s              *txn,
  qbi_svc_qmi_rsp_cb_f   *rsp_cb,
  boolean                 restore_mode_pref,
  mode_pref_mask_type_v01 mode_pref
);

static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_set_mode_pref_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_nas_visible_providers_q_set_mode_pref_timeout_cb
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_REGISTER_STATE
    @{ */

static void qbi_svc_bc_nas_register_state_cache_only_completion_cb
(
  const qbi_txn_s *txn
);

static void qbi_svc_bc_nas_register_state_cache_only_set_timeout
(
  qbi_ctx_s *ctx,
  boolean    cache_only
);

static void qbi_svc_bc_nas_register_state_cache_only_timeout_cb
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_nas_register_state_cached_provider_id_is_valid
(
  const qbi_ctx_s *ctx
);

static boolean qbi_svc_bc_nas_register_state_data_class_rat_matches
(
  uint32 available_data_class,
  uint32 requested_data_class
);

static qbi_util_comparison_result_e qbi_svc_bc_nas_register_state_eri_compare
(
  const void *array,
  uint32      index,
  const void *key
);

static void qbi_svc_bc_nas_register_state_eri_parse_spr
(
  qbi_ctx_s *ctx
);

static boolean qbi_svc_bc_nas_register_state_eri_parse_spr_entries
(
  const char                     *eri_data,
  uint32                          eri_data_len,
  uint8                           num_eri_entries,
  qbi_svc_bc_nas_eri_tbl_entry_s *parsed_eri_tbl
);

static void qbi_svc_bc_nas_register_state_eri_parse_spr_intl_roam
(
  qbi_ctx_s *ctx
);

static boolean qbi_svc_bc_nas_register_state_eri_parse_spr_intl_roam_entries
(
  const char *file_data,
  uint32      file_data_len,
  uint8       intl_roam_list_len,
  uint8      *intl_roam_list
);

static void qbi_svc_bc_nas_register_state_eri_parse_vzw
(
  qbi_ctx_s *ctx
);

static boolean qbi_svc_bc_nas_register_state_eri_parse_vzw_entries
(
  const uint8                    *eri_data,
  uint32                          eri_data_len,
  uint8                           num_eri_entries,
  qbi_svc_bc_nas_eri_tbl_entry_s *parsed_eri_tbl
);

static void qbi_svc_bc_nas_register_state_eri_reset
(
  qbi_svc_bc_nas_cache_s *cache
);

static uint32 qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state
(
  const qbi_svc_bc_nas_cache_s *cache,
  nas_roam_status_enum_type_v01 roam_status
);

static uint32 qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state_vzw
(
  nas_roam_status_enum_type_v01 roam_status
);

static uint32 qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state_spr
(
  const qbi_svc_bc_nas_cache_s *cache,
  nas_roam_status_enum_type_v01 roam_status
);

static boolean qbi_svc_bc_nas_register_state_nw_error_is_perm
(
  uint32 nw_error
);

static uint8 qbi_svc_bc_nas_register_state_qmi_roam_status_to_roam_ind
(
  nas_roam_status_enum_type_v01 roam_status
);

static uint32 qbi_svc_bc_nas_register_state_sys_info_to_available_data_class
(
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

static void qbi_svc_bc_nas_register_state_sys_info_to_provider_id_3gpp
(
  const qbi_svc_bc_nas_sys_info_s *sys_info,
  char                            *provider_id_ascii,
  uint32                           provider_id_ascii_size
);

static void qbi_svc_bc_nas_register_state_sys_info_to_provider_id_3gpp2
(
  const qbi_svc_bc_nas_sys_info_s *sys_info,
  char                            *provider_id_ascii,
  uint32                           provider_id_ascii_size
);

static uint32 qbi_svc_bc_nas_register_state_sys_info_to_register_state
(
  const qbi_ctx_s                 *ctx,
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

static uint16 qbi_svc_bc_nas_register_state_data_class_to_qmi_mode_pref
(
  uint32 data_class
);

static boolean qbi_svc_bc_nas_register_state_determine_cache_only_mode
(
  qbi_ctx_s *ctx,
  uint32     register_state
);

static void qbi_svc_bc_nas_register_state_e_completion_cb
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_eq_build_nas44_req
(
  qbi_txn_s            *txn,
  qbi_svc_qmi_rsp_cb_f *rsp_cb
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache_add_provider_info
(
  qbi_txn_s                    *txn,
  const qbi_svc_bc_nas_cache_s *cache
);

static void qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache_add_roaming_text
(
  qbi_txn_s                    *txn,
  const qbi_svc_bc_nas_cache_s *cache
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_nas_register_state_hybrid_primary_stack_changed
(
  uint32 old_available_data_class,
  uint32 new_available_data_class
);

static boolean qbi_svc_bc_nas_register_state_is_in_requested_mode
(
  qbi_ctx_s *ctx,
  uint32     register_action,
  uint32     data_class
);

static boolean qbi_svc_bc_nas_register_state_is_provider_id_and_name_reported
(
  uint32 register_state
);

static uint32 qbi_svc_bc_nas_register_state_primary_rat_to_cellular_class
(
  const qbi_ctx_s             *ctx,
  qbi_svc_bc_nas_primary_rat_e primary_rat
);

static nas_radio_if_enum_v01 qbi_svc_bc_nas_register_state_primary_rat_to_qmi_radio_if_3gpp
(
  qbi_svc_bc_nas_primary_rat_e primary_rat
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_q_nas34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_q_nas3b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_q_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_q_nas4d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_q_rsp
(
  qbi_txn_s *txn
);

static uint32 qbi_svc_bc_nas_register_state_qmi_hs_supp_to_mbim_data_class
(
  nas_hs_support_enum_type_v01 hs_supp
);

static uint32 qbi_svc_bc_nas_register_state_qmi_net_sel_pref_to_mbim_reg_mode
(
  nas_net_sel_pref_enum_v01 net_sel_pref
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_build_nas33_ps_attach_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_build_nas33_req
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_register_state_s_completion_cb
(
  const qbi_txn_s *txn
);

static boolean qbi_svc_bc_nas_register_state_s_is_request_complete
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_nas33_ps_attach_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_nas4e_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static boolean qbi_svc_bc_nas_register_state_s_sanity_check_req
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_nas_register_state_s_long_timeout_cb
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_nas_register_state_s_short_timeout_cb
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_wait_for_completion
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_nas_register_state_send_deregistered_before_searching
(
  qbi_ctx_s              *ctx,
  qbi_svc_bc_nas_cache_s *cache,
  uint32                  old_register_state
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_sync_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_register_state_sync_nas34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_nas_register_state_sync_read_net_sel_pref_compat
(
  qbi_ctx_s                         *ctx,
  qbi_svc_bc_nas_net_sel_pref_cfg_s *net_sel_pref_cfg
);

static boolean qbi_svc_bc_nas_register_state_upate_cache_available_data_class
(
  qbi_ctx_s                       *ctx,
  qbi_svc_bc_nas_cache_s          *cache,
  uint32                           new_register_state,
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

static boolean qbi_svc_bc_nas_register_state_update_cache_sys_info
(
  qbi_ctx_s                       *ctx,
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

static boolean qbi_svc_bc_nas_register_state_update_cache_current_cellular_class
(
  const qbi_ctx_s             *ctx,
  qbi_svc_bc_nas_cache_s      *cache,
  qbi_svc_bc_nas_primary_rat_e primary_rat
);

static boolean qbi_svc_bc_nas_register_state_update_cache_manual_reg_mcc_mnc
(
  qbi_ctx_s *ctx,
  uint16     mcc,
  uint16     mnc,
  boolean    mnc_is_3_digits
);

static boolean qbi_svc_bc_nas_register_state_update_cache_net_sel_pref
(
  qbi_ctx_s                *ctx,
  nas_net_sel_pref_enum_v01 net_sel_pref,
  mode_pref_mask_type_v01   mode_pref
);

static boolean qbi_svc_bc_nas_register_state_update_cache_nw_error
(
  qbi_svc_bc_nas_cache_s          *cache,
  const qbi_svc_bc_nas_sys_info_s *sys_info,
  boolean                          network_changed
);

static boolean qbi_svc_bc_nas_register_state_update_cache_plmn_mode
(
  qbi_ctx_s             *ctx,
  nas_plmn_mode_enum_v01 plmn_mode
);

static boolean qbi_svc_bc_nas_register_state_update_cache_provider_id
(
  qbi_ctx_s                       *ctx,
  qbi_svc_bc_nas_cache_s          *cache,
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

static boolean qbi_svc_bc_nas_register_state_update_cache_provider_name_3gpp
(
  const qbi_ctx_s *ctx,
  const uint8     *provider_name_3gpp,
  uint32           provider_name_3gpp_len
);

static boolean qbi_svc_bc_nas_register_state_update_cache_register_state
(
  qbi_ctx_s                       *ctx,
  qbi_svc_bc_nas_cache_s          *cache,
  uint32                           new_register_state,
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

static boolean qbi_svc_bc_nas_register_state_update_cache_roaming_text
(
  qbi_svc_bc_nas_cache_s          *cache,
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

static boolean qbi_svc_bc_nas_register_state_update_cache_sim_rej_info
(
  qbi_svc_bc_nas_cache_s          *cache,
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_SERVICE
    @{ */

static uint32 qbi_svc_bc_nas_packet_service_data_class_best_available
(
  uint32 available_data_class
);

static uint32 qbi_svc_bc_nas_packet_service_determine_packet_service_state
(
  const qbi_svc_bc_nas_cache_s *cache
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_eq_build_wds23_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_eq_wds23_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_nas_packet_service_nw_error_is_perm
(
  uint32 nw_error
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_q_nas4d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_q_wds6b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_q_rsp
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_qmi_ind_reg_nas70_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_qmi_ind_reg_wds01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_nas_packet_service_s_completion_cb
(
  const qbi_txn_s *txn
);

static boolean qbi_svc_bc_nas_packet_service_s_device_in_requested_state
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_nas34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_nas4e_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static void qbi_svc_bc_nas_packet_service_s_timeout_cb
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_wait_for_completion
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_wds01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static void qbi_svc_bc_nas_packet_service_send_detached_event_if_deregistered
(
  qbi_ctx_s                       *ctx,
  const qbi_svc_bc_nas_sys_info_s *sys_info,
  uint32                           old_register_state,
  uint32                           new_register_state
);

static boolean qbi_svc_bc_nas_packet_service_update_cache_channel_rates
(
  qbi_ctx_s *ctx,
  uint32     downlink_speed,
  uint32     uplink_speed
);

static boolean qbi_svc_bc_nas_packet_service_update_cache_ds_status
(
  const qbi_ctx_s                       *ctx,
  const wds_data_system_status_type_v01 *ds_status
);

static boolean qbi_svc_bc_nas_packet_service_update_cache_sys_info
(
  qbi_ctx_s                       *ctx,
  const qbi_svc_bc_nas_sys_info_s *sys_info
);

/*! @} */

/*! @addtogroup MBIM_CID_SIGNAL_STATE
    @{ */

static qbi_svc_action_e qbi_svc_bc_nas_signal_state_build_nas03_req
(
  qbi_txn_s            *txn,
  boolean               enable_inds,
  qbi_svc_qmi_rsp_cb_f *rsp_cb
);

static qbi_svc_action_e qbi_svc_bc_nas_signal_state_custom_mapping_apply_nas6c_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_nas_signal_state_custom_mapping_free
(
  qbi_svc_bc_nas_signal_mapping_s **mapping
);

static const qbi_svc_bc_nas_signal_mapping_s *qbi_svc_bc_nas_signal_state_mapping_get
(
  const qbi_ctx_s               *ctx,
  qbi_svc_bc_nas_signal_metric_e metric
);

static uint32 qbi_svc_bc_nas_signal_state_mbim_rssi_to_bars
(
  uint32 mbim_rssi
);

static boolean qbi_svc_bc_nas_signal_state_metric_is_valid
(
  qbi_svc_bc_nas_signal_metric_e metric
);

static uint32 qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi
(
  const qbi_ctx_s               *ctx,
  qbi_svc_bc_nas_signal_metric_e metric,
  int16                          value
);

static uint32 qbi_svc_bc_nas_signal_state_metrics_combine_to_mbim_rssi
(
  const qbi_ctx_s               *ctx,
  qbi_svc_bc_nas_signal_metric_e metric1,
  int16                          value1,
  qbi_svc_bc_nas_signal_metric_e metric2,
  int16                          value2
);

static boolean qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list
(
  const qbi_ctx_s               *ctx,
  qbi_svc_bc_nas_signal_metric_e metric,
  uint32_t                      *threshold_list_len,
  int16_t                       *threshold_list,
  uint32                         threshold_list_size
);

static qbi_svc_action_e qbi_svc_bc_nas_signal_state_q_nas4f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_nas_signal_state_q_nas52_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static uint32 qbi_svc_bc_nas_signal_state_qmi_error_rate_to_mbim
(
  uint8_t  cdma_frame_err_rate_valid,
  uint16_t cdma_frame_err_rate,
  uint8_t  hdr_packet_err_rate_valid,
  uint16_t hdr_packet_err_rate,
  uint8_t  gsm_bit_err_rate_valid,
  uint8_t  gsm_bit_err_rate
);

static uint32 qbi_svc_bc_nas_signal_state_qmi_error_rate_3gpp2_to_mbim
(
  uint16_t qmi_3gpp2_error_rate
);

static qbi_svc_action_e qbi_svc_bc_nas_signal_state_qmi_ind_reg_nas03_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static uint32 qbi_svc_bc_nas_signal_state_qmi_sig_info_to_mbim_rssi
(
  const qbi_ctx_s                          *ctx,
  uint8_t                                   cdma_sig_info_valid,
  const nas_common_sig_info_param_type_v01 *cdma_sig_info,
  uint8_t                                   hdr_sig_info_valid,
  const nas_hdr_sig_info_type_v01          *hdr_sig_info,
  uint8_t                                   gsm_rssi_valid,
  int8_t                                    gsm_rssi,
  uint8_t                                   wcdma_sig_info_valid,
  const nas_common_sig_info_param_type_v01 *wcdma_sig_info,
  uint8_t                                   lte_sig_info_valid,
  const nas_lte_sig_info_type_v01          *lte_sig_info,
  uint8_t                                   tdscdma_rscp_valid,
  int8_t                                    tdscdma_rscp
);

static uint32 qbi_svc_bc_nas_signal_state_rssi_to_mbim
(
  int16 rssi
);

static void qbi_svc_bc_nas_signal_state_s_detect_host
(
  const qbi_ctx_s                       *ctx,
  const qbi_svc_bc_signal_state_s_req_s *req
);

static qbi_svc_action_e qbi_svc_bc_nas_signal_state_s_nas03_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_nas_signal_state_update_cache_signal
(
  const qbi_ctx_s *ctx,
  uint32           rssi,
  uint32           error_rate
);

static void qbi_svc_bc_nas_signal_state_update_cache_reporting
(
  const qbi_ctx_s *ctx,
  uint32           rssi_interval,
  uint32           rssi_threshold,
  uint32           error_rate_threshold
);

/*! @} */

/*! @addtogroup MBIM_CID_EMERGENCY_MODE
    @{ */

static qbi_svc_action_e qbi_svc_bc_nas_emergency_mode_eq_build_rsp_from_cache
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_nas_emergency_mode_q_nas34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_nas_emergency_mode_update_cache
(
  qbi_ctx_s *ctx,
  uint32     qmi_emergency_mode
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity NAS module cache

    @details

    @param ctx

    @return qbi_svc_bc_nas_cache_s* Pointer to cache, or NULL on failure
*/
/*=========================================================================*/
qbi_svc_bc_nas_cache_s *qbi_svc_bc_nas_cache_get
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_module_cache_s *module_cache;
/*-------------------------------------------------------------------------*/
  module_cache = qbi_svc_bc_module_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(module_cache);

  return module_cache->nas;
} /* qbi_svc_bc_nas_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_reject_cause_allows_retry
===========================================================================*/
/*!
    @brief Checks whether a network reject cause code allows for the device
    to retry registration on the PLMN

    @details

    @param nw_error

    @return boolean TRUE if registration on the PLMN may be retried, FALSE
    otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_reject_cause_allows_retry
(
  uint32 nw_error
)
{
  boolean retry_allowed;
/*-------------------------------------------------------------------------*/
  switch (nw_error)
  {
    case QBI_SVC_BC_NAS_REJECT_CAUSE_MS_IDENTITY_CANNOT_BE_DERIVED_BY_NW:
    case QBI_SVC_BC_NAS_REJECT_CAUSE_IMPLICITLY_DETACHED:
    case QBI_SVC_BC_NAS_REJECT_CAUSE_NO_SUITABLE_CELLS_IN_LAI:
      retry_allowed = TRUE;
      break;

    default:
      retry_allowed = FALSE;
  }

  return retry_allowed;
} /* qbi_svc_bc_nas_reject_cause_allows_retry() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_sys_info_extract_common
===========================================================================*/
/*!
    @brief Copy data from individual fields in a QMI SYS_INFO indication or
    response structure into the common QBI sys_info structure

    @details
    The QBI sys_info should already be partially populated by either
    qbi_svc_bc_nas_sys_info_extract_ind() or
    qbi_svc_bc_nas_sys_info_extract_rsp()

    @param sys_info
    @param cdma_srv_status_info_valid
    @param cdma_srv_status
    @param cdma_sys_info_valid
    @param cdma_sys_info
    @param sim_rej_info_valid
    @param sim_rej_info
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_sys_info_extract_common
(
  qbi_svc_bc_nas_sys_info_s        *sys_info,
  boolean                           cdma_srv_status_info_valid,
  nas_service_status_enum_type_v01  cdma_srv_status,
  boolean                           cdma_sys_info_valid,
  const nas_cdma_sys_info_type_v01 *cdma_sys_info,
  boolean                           sim_rej_info_valid,
  nas_sim_rej_info_enum_type_v01    sim_rej_info
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(sys_info);
  QBI_CHECK_NULL_PTR_RET(cdma_sys_info);

  /* Special handling for hybrid 1xEV-DO support */
  if (sys_info->primary_rat == QBI_SVC_BC_NAS_PRIMARY_RAT_HDR)
  {
    if (cdma_srv_status_info_valid)
    {
      sys_info->hybrid_srv_status = cdma_srv_status;
    }
    else
    {
      sys_info->hybrid_srv_status = NAS_SYS_SRV_STATUS_NO_SRV_V01;
    }

    if (cdma_sys_info_valid)
    {
      sys_info->hybrid_cdma_sys_info = cdma_sys_info;
    }
  }

  /* Save SIM Reject Information */
  if (sim_rej_info_valid)
  {
    sys_info->sim_rej_info_valid = TRUE;
    sys_info->sim_rej_info = sim_rej_info;
  }
} /* qbi_svc_bc_nas_sys_info_extract_common() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_sys_info_extract_ind
===========================================================================*/
/*!
    @brief Extracts information from a QMI_NAS_SYS_INFO_IND into a
    structure that is common with the associated response

    @details

    @param sys_info
    @param qmi_ind
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_sys_info_extract_ind
(
  qbi_svc_bc_nas_sys_info_s      *sys_info,
  const nas_sys_info_ind_msg_v01 *qmi_ind
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(sys_info);
  QBI_CHECK_NULL_PTR_RET(qmi_ind);

  QBI_MEMSET(sys_info, 0, sizeof(qbi_svc_bc_nas_sys_info_s));
  sys_info->primary_rat =
    qbi_svc_bc_nas_sys_info_get_primary_rat(
      qmi_ind->cdma_srv_status_info_valid,    &qmi_ind->cdma_srv_status_info,
      qmi_ind->hdr_srv_status_info_valid,     &qmi_ind->hdr_srv_status_info,
      qmi_ind->gsm_srv_status_info_valid,     &qmi_ind->gsm_srv_status_info,
      qmi_ind->wcdma_srv_status_info_valid,   &qmi_ind->wcdma_srv_status_info,
      qmi_ind->lte_srv_status_info_valid,     &qmi_ind->lte_srv_status_info,
      qmi_ind->tdscdma_srv_status_info_valid, &qmi_ind->tdscdma_srv_status_info);
  switch (sys_info->primary_rat)
  {
    case QBI_SVC_BC_NAS_PRIMARY_RAT_CDMA:
      sys_info->srv_status = qmi_ind->cdma_srv_status_info.srv_status;
      sys_info->rat.cdma   = &qmi_ind->cdma_sys_info;
      sys_info->common     = &qmi_ind->cdma_sys_info.common_sys_info;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_HDR:
      sys_info->srv_status = qmi_ind->hdr_srv_status_info.srv_status;
      sys_info->rat.hdr    = &qmi_ind->hdr_sys_info;
      sys_info->common     = &qmi_ind->hdr_sys_info.common_sys_info;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_GSM:
      sys_info->srv_status  = qmi_ind->gsm_srv_status_info.srv_status;
      sys_info->rat.gsm     = &qmi_ind->gsm_sys_info;
      sys_info->common      = &qmi_ind->gsm_sys_info.common_sys_info;
      sys_info->common_3gpp = &qmi_ind->gsm_sys_info.threegpp_specific_sys_info;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_WCDMA:
      sys_info->srv_status  = qmi_ind->wcdma_srv_status_info.srv_status;
      sys_info->rat.wcdma   = &qmi_ind->wcdma_sys_info;
      sys_info->common      = &qmi_ind->wcdma_sys_info.common_sys_info;
      sys_info->common_3gpp =
        &qmi_ind->wcdma_sys_info.threegpp_specific_sys_info;
      if (qmi_ind->wcdma_csg_info_valid)
      {
        sys_info->csg_info = &qmi_ind->wcdma_csg_info;
      }
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_LTE:
      sys_info->srv_status  = qmi_ind->lte_srv_status_info.srv_status;
      sys_info->rat.lte     = &qmi_ind->lte_sys_info;
      sys_info->common      = &qmi_ind->lte_sys_info.common_sys_info;
      sys_info->common_3gpp = &qmi_ind->lte_sys_info.threegpp_specific_sys_info;
      if (qmi_ind->lte_csg_info_valid)
      {
        sys_info->csg_info = &qmi_ind->lte_csg_info;
      }
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_TDSCDMA:
      sys_info->srv_status  = qmi_ind->tdscdma_srv_status_info.srv_status;
      sys_info->rat.tdscdma = &qmi_ind->tdscdma_sys_info;
      sys_info->common      = &qmi_ind->tdscdma_sys_info.common_sys_info;
      sys_info->common_3gpp =
        &qmi_ind->tdscdma_sys_info.threegpp_specific_sys_info;
      break;

    default:
      sys_info->srv_status = NAS_SYS_SRV_STATUS_NO_SRV_V01;
  }

  qbi_svc_bc_nas_sys_info_extract_common(
    sys_info, qmi_ind->cdma_srv_status_info_valid,
    qmi_ind->cdma_srv_status_info.srv_status, qmi_ind->cdma_sys_info_valid,
    &qmi_ind->cdma_sys_info, qmi_ind->sim_rej_info_valid,
    qmi_ind->sim_rej_info);
} /* qbi_svc_bc_nas_sys_info_extract_ind() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_sys_info_extract_rsp
===========================================================================*/
/*!
    @brief Extracts information from a QMI_NAS_GET_SYS_INFO_RESP into a
    structure that is common with the associated indication

    @details

    @param sys_info
    @param qmi_rsp
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_sys_info_extract_rsp
(
  qbi_svc_bc_nas_sys_info_s           *sys_info,
  const nas_get_sys_info_resp_msg_v01 *qmi_rsp
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(sys_info);
  QBI_CHECK_NULL_PTR_RET(qmi_rsp);

  QBI_MEMSET(sys_info, 0, sizeof(qbi_svc_bc_nas_sys_info_s));
  sys_info->primary_rat =
    qbi_svc_bc_nas_sys_info_get_primary_rat(
      qmi_rsp->cdma_srv_status_info_valid,    &qmi_rsp->cdma_srv_status_info,
      qmi_rsp->hdr_srv_status_info_valid,     &qmi_rsp->hdr_srv_status_info,
      qmi_rsp->gsm_srv_status_info_valid,     &qmi_rsp->gsm_srv_status_info,
      qmi_rsp->wcdma_srv_status_info_valid,   &qmi_rsp->wcdma_srv_status_info,
      qmi_rsp->lte_srv_status_info_valid,     &qmi_rsp->lte_srv_status_info,
      qmi_rsp->tdscdma_srv_status_info_valid, &qmi_rsp->tdscdma_srv_status_info);
  switch (sys_info->primary_rat)
  {
    case QBI_SVC_BC_NAS_PRIMARY_RAT_CDMA:
      sys_info->srv_status = qmi_rsp->cdma_srv_status_info.srv_status;
      sys_info->rat.cdma   = &qmi_rsp->cdma_sys_info;
      sys_info->common     = &qmi_rsp->cdma_sys_info.common_sys_info;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_HDR:
      sys_info->srv_status = qmi_rsp->hdr_srv_status_info.srv_status;
      sys_info->rat.hdr    = &qmi_rsp->hdr_sys_info;
      sys_info->common     = &qmi_rsp->hdr_sys_info.common_sys_info;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_GSM:
      sys_info->srv_status  = qmi_rsp->gsm_srv_status_info.srv_status;
      sys_info->rat.gsm     = &qmi_rsp->gsm_sys_info;
      sys_info->common      = &qmi_rsp->gsm_sys_info.common_sys_info;
      sys_info->common_3gpp = &qmi_rsp->gsm_sys_info.threegpp_specific_sys_info;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_WCDMA:
      sys_info->srv_status  = qmi_rsp->wcdma_srv_status_info.srv_status;
      sys_info->rat.wcdma   = &qmi_rsp->wcdma_sys_info;
      sys_info->common      = &qmi_rsp->wcdma_sys_info.common_sys_info;
      sys_info->common_3gpp =
        &qmi_rsp->wcdma_sys_info.threegpp_specific_sys_info;
      if (qmi_rsp->wcdma_csg_info_valid)
      {
        sys_info->csg_info = &qmi_rsp->wcdma_csg_info;
      }
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_LTE:
      sys_info->srv_status  = qmi_rsp->lte_srv_status_info.srv_status;
      sys_info->rat.lte     = &qmi_rsp->lte_sys_info;
      sys_info->common      = &qmi_rsp->lte_sys_info.common_sys_info;
      sys_info->common_3gpp = &qmi_rsp->lte_sys_info.threegpp_specific_sys_info;
      if (qmi_rsp->lte_csg_info_valid)
      {
        sys_info->csg_info = &qmi_rsp->lte_csg_info;
      }
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_TDSCDMA:
      sys_info->srv_status  = qmi_rsp->tdscdma_srv_status_info.srv_status;
      sys_info->rat.tdscdma = &qmi_rsp->tdscdma_sys_info;
      sys_info->common      = &qmi_rsp->tdscdma_sys_info.common_sys_info;
      sys_info->common_3gpp =
        &qmi_rsp->tdscdma_sys_info.threegpp_specific_sys_info;
      break;

    default:
      sys_info->srv_status = NAS_SYS_SRV_STATUS_NO_SRV_V01;
  }

  qbi_svc_bc_nas_sys_info_extract_common(
    sys_info, qmi_rsp->cdma_srv_status_info_valid,
    qmi_rsp->cdma_srv_status_info.srv_status, qmi_rsp->cdma_sys_info_valid,
    &qmi_rsp->cdma_sys_info, qmi_rsp->sim_rej_info_valid,
    qmi_rsp->sim_rej_info);
} /* qbi_svc_bc_nas_sys_info_extract_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_sys_info_get_primary_rat
===========================================================================*/
/*!
    @brief Determines the primary RAT path based on the service status of all
    available RATs

    @details

    @param cdma_srv_status_info_valid
    @param cdma_srv_status_info
    @param hdr_srv_status_info_valid
    @param hdr_srv_status_info
    @param gsm_srv_status_info_valid
    @param gsm_srv_status_info
    @param wcdma_srv_status_info_valid
    @param wcdma_srv_status_info
    @param lte_srv_status_info_valid
    @param lte_srv_status_info
    @param tdscdma_srv_status_info_valid
    @param tdscdma_srv_status_info

    @return qbi_svc_bc_nas_primary_rat_e
*/
/*=========================================================================*/
static qbi_svc_bc_nas_primary_rat_e qbi_svc_bc_nas_sys_info_get_primary_rat
(
  uint8_t                                   cdma_srv_status_info_valid,
  const nas_3gpp2_srv_status_info_type_v01 *cdma_srv_status_info,
  uint8_t                                   hdr_srv_status_info_valid,
  const nas_3gpp2_srv_status_info_type_v01 *hdr_srv_status_info,
  uint8_t                                   gsm_srv_status_info_valid,
  const nas_3gpp_srv_status_info_type_v01  *gsm_srv_status_info,
  uint8_t                                   wcdma_srv_status_info_valid,
  const nas_3gpp_srv_status_info_type_v01  *wcdma_srv_status_info,
  uint8_t                                   lte_srv_status_info_valid,
  const nas_3gpp_srv_status_info_type_v01  *lte_srv_status_info,
  uint8_t                                   tdscdma_srv_status_info_valid,
  const nas_3gpp_srv_status_info_type_v01  *tdscdma_srv_status_info
)
{
  qbi_svc_bc_nas_primary_rat_e primary_rat = QBI_SVC_BC_NAS_PRIMARY_RAT_NONE;
/*-------------------------------------------------------------------------*/
  /* Except for the case of hybrid CDMA+HDR, QMI NAS will only set srv_status
     for one RAT to a value other than NO_SRV. Simultaneous GSM+WCDMA service
     uses a second QMI NAS client ID bound to the secondary subscription via
     QMI_NAS_BIND_SUBSCRIPTION, though this is not currently supported by
     QBI or MBIM. */
  if (cdma_srv_status_info_valid && cdma_srv_status_info != NULL &&
      cdma_srv_status_info->srv_status != NAS_SYS_SRV_STATUS_NO_SRV_V01)
  {
    primary_rat = QBI_SVC_BC_NAS_PRIMARY_RAT_CDMA;
  }
  if (hdr_srv_status_info_valid && hdr_srv_status_info != NULL &&
      hdr_srv_status_info->srv_status != NAS_SYS_SRV_STATUS_NO_SRV_V01)
  {
    /* HDR should not supersede CDMA if CDMA is in service and HDR is not */
    if (primary_rat == QBI_SVC_BC_NAS_PRIMARY_RAT_CDMA &&
        cdma_srv_status_info->srv_status == NAS_SYS_SRV_STATUS_SRV_V01 &&
        hdr_srv_status_info->srv_status != NAS_SYS_SRV_STATUS_SRV_V01)
    {
      QBI_LOG_D_2("CDMA in service (%d) but HDR not (%d); CDMA is primary RAT",
                  cdma_srv_status_info->srv_status,
                  hdr_srv_status_info->srv_status);
    }
    else
    {
      primary_rat = QBI_SVC_BC_NAS_PRIMARY_RAT_HDR;
    }
  }
  if (gsm_srv_status_info_valid && gsm_srv_status_info != NULL &&
      gsm_srv_status_info->srv_status != NAS_SYS_SRV_STATUS_NO_SRV_V01)
  {
    primary_rat = QBI_SVC_BC_NAS_PRIMARY_RAT_GSM;
  }
  if (tdscdma_srv_status_info_valid && tdscdma_srv_status_info != NULL &&
      tdscdma_srv_status_info->srv_status != NAS_SYS_SRV_STATUS_NO_SRV_V01)
  {
    primary_rat = QBI_SVC_BC_NAS_PRIMARY_RAT_TDSCDMA;
  }
  if (wcdma_srv_status_info_valid && wcdma_srv_status_info != NULL &&
      wcdma_srv_status_info->srv_status != NAS_SYS_SRV_STATUS_NO_SRV_V01)
  {
    primary_rat = QBI_SVC_BC_NAS_PRIMARY_RAT_WCDMA;
  }
  if (lte_srv_status_info_valid && lte_srv_status_info != NULL &&
      lte_srv_status_info->srv_status != NAS_SYS_SRV_STATUS_NO_SRV_V01)
  {
    primary_rat = QBI_SVC_BC_NAS_PRIMARY_RAT_LTE;
  }

  QBI_LOG_D_1("Primary RAT is %d", primary_rat);
  return primary_rat;
} /* qbi_svc_bc_nas_sys_info_get_primary_rat() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_sys_info_is_ps_attached
===========================================================================*/
/*!
    @brief Determines if the common sysem information structure indicates
    that the device is attached on the PS domain

    @details

    @param sys_info

    @return boolean TRUE if PS attached, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_sys_info_is_ps_attached
(
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(sys_info);

  return (sys_info->common != NULL &&
          sys_info->common->srv_domain_valid &&
          (sys_info->common->srv_domain == SYS_SRV_DOMAIN_PS_ONLY_V01 ||
           sys_info->common->srv_domain == SYS_SRV_DOMAIN_CS_PS_V01));
} /* qbi_svc_bc_nas_sys_info_is_ps_attached() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_sys_info_is_reject_reason_valid
===========================================================================*/
/*!
    @brief Checks whether the given sys_info strcuture contains a valid
    registration reject cause code

    @details

    @param sys_info

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_sys_info_is_reject_cause_valid
(
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
/*-------------------------------------------------------------------------*/
  return (sys_info != NULL && sys_info->common_3gpp != NULL &&
          sys_info->common_3gpp->reg_reject_info_valid);
} /* qbi_svc_bc_nas_sys_info_is_reject_reason_valid() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_sys_info_qmi_ind_reg
===========================================================================*/
/*!
    @brief Manages QMI indication registration for QMI_NAS_SYS_INFO

    @details
    Registration for QMI_NAS_CURRENT_PLMN_NAME_IND,
    QMI_NAS_OPERATOR_NAME_DATA_IND, and QMI_NAS_CSP_PLMN_MODE_BIT_IND also
    follow the QMI_NAS_SYS_INFO_IND registration.

    @param txn
    @param enable_inds

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_sys_info_qmi_ind_reg
(
  qbi_txn_s *txn,
  boolean    enable_inds
)
{
  qbi_svc_action_e action;
  nas_indication_register_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (qbi_svc_ind_qmi_ref_cnt_update_then_check_if_significant(
        txn->ctx, QBI_QMI_SVC_NAS, QMI_NAS_SYS_INFO_IND_MSG_V01, enable_inds))
  {
    qmi_req = (nas_indication_register_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_NAS, QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
        qbi_svc_bc_nas_sys_info_qmi_ind_reg_nas03_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    /* QMI_NAS_SYS_INFO, QMI_NAS_CURRENT_PLMN_NAME_IND,
       QMI_NAS_OPERATOR_NAME_DATA_IND, and QMI_NAS_CSP_PLMN_MODE_BIT_IND are all
       grouped together under the SYS_INFO reference count. If SYS_INFO events
       are enabled for any reason, then we will also be registered for the
       others, which are not sent very frequently compared to SYS_INFO. */
    qmi_req->sys_info_valid = TRUE;
    qmi_req->sys_info = enable_inds;
    qmi_req->reg_current_plmn_name_valid = TRUE;
    qmi_req->reg_current_plmn_name = enable_inds;
    qmi_req->reg_operator_name_data_valid = TRUE;
    qmi_req->reg_operator_name_data = enable_inds;
    qmi_req->reg_csp_plmn_mode_bit_valid = TRUE;
    qmi_req->reg_csp_plmn_mode_bit = enable_inds;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_bc_nas_sys_info_qmi_ind_reg() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_sys_info_qmi_ind_reg_nas03_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_INDICATION_REGISTER_RESP for enabling or disabling
    QMI_NAS_SYS_INFO_IND and related network name/PLMN mode indications

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_sys_info_qmi_ind_reg_nas03_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  nas_indication_register_req_msg_v01 *qmi_req;
  nas_indication_register_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (nas_indication_register_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (nas_indication_register_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for NAS indications! Error code %d",
                qmi_rsp->resp.error);
  }
  else
  {
    action = (qmi_txn->parent->qmi_txns_pending > 0) ?
      QBI_SVC_ACTION_WAIT_ASYNC_RSP : QBI_SVC_ACTION_SEND_RSP;
  }

  /* Refresh the cache if we are enabling indications */
  if (qmi_req->sys_info == TRUE)
  {
    qbi_svc_internal_query(
      qmi_txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_REGISTER_STATE);
  }

  return action;
} /* qbi_svc_bc_nas_sys_info_qmi_ind_reg_nas03_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_send_packet_service_then_register_state_events
===========================================================================*/
/*!
    @brief Forces sending a PACKET_SERVICE event (via query processing), then
    after that transaction is complete, sends a REGISTER_STATE event (also
    via query processing)

    @details

    @param ctx
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_send_packet_service_then_register_state_events
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
/*-------------------------------------------------------------------------*/
  txn = qbi_txn_alloc_event(
    ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PACKET_SERVICE);
  QBI_CHECK_NULL_PTR_RET(txn);

  qbi_txn_set_completion_cb(
    txn,
    qbi_svc_bc_nas_send_packet_service_then_register_state_events_completion_cb);

  (void) qbi_svc_proc_action(txn, qbi_svc_bc_nas_packet_service_q_req(txn));
} /* qbi_svc_bc_nas_send_packet_service_then_register_state_events() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_send_packet_service_then_register_state_events_completion_cb
===========================================================================*/
/*!
    @brief Called when the PACKET_SERVICE event triggered by
    qbi_svc_bc_nas_send_packet_service_then_register_state_events() is done;
    forces a REGISTER_STATE event

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_send_packet_service_then_register_state_events_completion_cb
(
  const qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  QBI_LOG_I_0("Forced PACKET_SERVICE event complete; forcing REGISTER_STATE "
              "event");
  qbi_svc_force_event(
    txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_REGISTER_STATE);
} /* qbi_svc_bc_nas_send_packet_service_then_register_state_events_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_spoof_detached_deregistered_cmd_cb
===========================================================================*/
/*!
    @brief Synchronously sends CID events to spoof that we deregistered from
    the network then returned to our current state

    @details

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_spoof_detached_deregistered_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
  qbi_txn_s *txn;
  qbi_svc_bc_nas_cache_s *cache;
  uint32 packet_service_state;
  uint32 register_state;
  uint32 available_data_class;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(cmd_id);
  QBI_ARG_NOT_USED(data);

  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  QBI_LOG_I_0("Spoofing detached & deregistered events, then restoring state");
  packet_service_state = cache->packet_service.packet_service_state;
  available_data_class = cache->register_state.available_data_class;
  register_state = cache->register_state.register_state;

  cache->packet_service.packet_service_state =
    QBI_SVC_BC_PACKET_SERVICE_STATE_DETACHED;
  txn = qbi_txn_alloc_event(
    ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PACKET_SERVICE);
  action = qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(txn);
  (void) qbi_svc_proc_action(txn, action);

  cache->register_state.register_state = QBI_SVC_BC_REGISTER_STATE_DEREGISTERED;
  cache->register_state.available_data_class = QBI_SVC_BC_DATA_CLASS_NONE;
  txn = qbi_txn_alloc_event(
    ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_REGISTER_STATE);
  action = qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache(txn);
  (void) qbi_svc_proc_action(txn, action);

  cache->register_state.register_state = register_state;
  cache->register_state.available_data_class = available_data_class;
  txn = qbi_txn_alloc_event(
    ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_REGISTER_STATE);
  action = qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache(txn);
  (void) qbi_svc_proc_action(txn, action);

  cache->packet_service.packet_service_state = packet_service_state;
  txn = qbi_txn_alloc_event(
    ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PACKET_SERVICE);
  action = qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(txn);
  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_nas_spoof_detached_deregistered_cmd_cb() */

/*! @addtogroup MBIM_CID_VISIBLE_PROVIDERS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_build_nas21_req
===========================================================================*/
/*!
    @brief Issues a QMI_NAS_PERFORM_NETWORK_SCAN_REQ

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_build_nas21_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_NAS, QMI_NAS_PERFORM_NETWORK_SCAN_REQ_MSG_V01,
    qbi_svc_bc_nas_visible_providers_q_nas21_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  /*! @todo Support aborting the network scan. */

  /* Network scans can take a long time, so extend the timeout */
  qbi_txn_set_timeout(
    txn, QBI_SVC_BC_NAS_VISIBLE_PROVIDERS_TIMEOUT_MS, NULL);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_nas_visible_providers_q_build_nas21_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_nas21_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_PERFORM_NETWORK_SCAN_RESP for
    MBIM_CID_VISIBLE_PROVIDERS query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_nas21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_perform_network_scan_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
  qbi_qmi_txn_s *nas44_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_perform_network_scan_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    /*! @note If the network scan is aborted due to internal modem behavior,
        e.g. CM_PH_EVENT_TERMINATE_GET_NETWORKS due to LTE registration
        activities, QMI NAS will return QMI_ERR_INTERNAL. Ideally, QMI NAS would
        return a more unique error code that we could map to MBIM_STATUS_BUSY,
        but currently we need to maintain this mapping to prevent failures in
        WHCK's DriverStress tool when using LTE. */
    if (qmi_rsp->resp.error == QMI_ERR_DEVICE_IN_USE_V01 ||
        qmi_rsp->resp.error == QMI_ERR_INTERNAL_V01)
    {
      QBI_LOG_W_0("Couldn't perform network scan: device busy");
      qmi_txn->parent->status = QBI_MBIM_STATUS_BUSY;
    }
    else
    {
      QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
      qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
    }
  }
  else if (qmi_rsp->scan_result_valid &&
           qmi_rsp->scan_result != NAS_SCAN_SUCCESS_V01)
  {
    QBI_LOG_E_1("Scan was aborted: result %d", qmi_rsp->scan_result);
    qmi_txn->parent->status = QBI_MBIM_STATUS_BUSY;
  }
  else
  {
    if (!qmi_rsp->nas_3gpp_network_info_valid)
    {
      QBI_LOG_W_0("TLV missing from response - assuming no networks visible");
      qmi_rsp->nas_3gpp_network_info_len = 0;
    }

    QBI_LOG_I_1("Manual scan returned %d visible networks",
                qmi_rsp->nas_3gpp_network_info_len);
    if (qmi_rsp->nas_3gpp_network_info_len == 0)
    {
      qmi_txn->parent->status = QBI_MBIM_STATUS_PROVIDERS_NOT_FOUND;
      action = QBI_SVC_ACTION_ABORT;
    }
    else if (!qmi_rsp->mnc_includes_pcs_digit_valid ||
             qmi_rsp->mnc_includes_pcs_digit_len !=
               qmi_rsp->nas_3gpp_network_info_len)
    {
      QBI_LOG_E_0("MNC includes PCS digit TLV not included or invalid!");
    }
    else if (!qmi_rsp->nas_network_radio_access_technology_valid ||
             qmi_rsp->nas_3gpp_network_info_len !=
               qmi_rsp->nas_network_radio_access_technology_len)
    {
      QBI_LOG_E_0("RAT info TLV not included or invalid!");
    }
    else if (!qbi_svc_bc_nas_visible_providers_q_prepare_info(
               qmi_txn->parent, qmi_rsp->nas_3gpp_network_info,
               qmi_rsp->nas_3gpp_network_info_len,
               qmi_rsp->mnc_includes_pcs_digit,
               qmi_rsp->nas_network_radio_access_technology,
               (qmi_txn->parent->svc_id == QBI_SVC_ID_BC) ? TRUE : FALSE))
    {
      QBI_LOG_E_0("Couldn't prepare info for PLMN name queries");
    }
    else
    {
      nas44_txn = qbi_qmi_txn_alloc(
        qmi_txn->parent, QBI_QMI_SVC_NAS, QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01,
        (qmi_txn->parent->svc_id == QBI_SVC_ID_BC) ?
          qbi_svc_bc_nas_visible_providers_q_nas44_rsp_cb :
          qbi_svc_atds_operators_q_nas44_rsp_cb);
      action = qbi_svc_bc_nas_visible_providers_q_get_next_plmn_name(
        qmi_txn->parent, nas44_txn);
    }
  }

  return action;
} /* qbi_svc_bc_nas_visible_providers_q_nas21_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_nas44_rsp_cb
===========================================================================*/
/*!
    @brief Combines the network name given by QMI_NAS_GET_PLMN_NAME_RESP with
    the saved information to populate the MBIM_PROVIDER strucutre for a
    visible network, then issues a query for the name of the next network

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  nas_get_plmn_name_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_nas_visible_providers_q_info_s *info;
  uint32 provider_name_len = 0;
  uint8 provider_name[QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES];
  uint32 provider_state;
  qbi_mbim_offset_size_pair_s *provider_field;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);

  info = (qbi_svc_bc_nas_visible_providers_q_info_s *) qmi_txn->parent->info;
  qmi_rsp = (nas_get_plmn_name_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_3("Couldn't get PLMN name for %03d-%02d: error %d",
                info->ntw[info->cur_index].mcc, info->ntw[info->cur_index].mnc,
                qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->eons_plmn_name_3gpp_valid)
  {
    QBI_LOG_E_0("Missing PLMN name TLV in successful response!");
  }
  else
  {
    provider_field = (qbi_mbim_offset_size_pair_s *)
      ((uint8 *) qmi_txn->parent->rsp.data + sizeof(qbi_svc_bc_provider_list_s) +
       (info->cur_index * sizeof(qbi_mbim_offset_size_pair_s)));

    provider_name_len = qbi_svc_bc_qmi_plmn_name_to_provider_name(
      qmi_rsp, provider_name, sizeof(provider_name),
      QBI_SVC_BC_PROVIDER_NAME_PREF_PLMN_NAME);
    provider_state =
      qbi_svc_bc_nas_visible_providers_qmi_ntw_status_to_provider_state(
        info->ntw[info->cur_index].network_status);
    if (!qbi_svc_bc_provider_add(
          qmi_txn->parent, provider_field, info->ntw[info->cur_index].mcc,
          info->ntw[info->cur_index].mnc,
          info->ntw[info->cur_index].mnc_is_3_digits, 0, provider_state,
          provider_name, provider_name_len, FALSE, QBI_SVC_BC_CELLULAR_CLASS_GSM,
          QBI_SVC_BC_RSSI_UNKNOWN, QBI_SVC_BC_ERROR_RATE_UNKNOWN))
    {
      QBI_LOG_E_0("Couldn't add provider!");
    }
    else
    {
      info->cur_index++;
      action = qbi_svc_bc_nas_visible_providers_q_get_next_plmn_name(
        qmi_txn->parent, qmi_txn);
    }
  }

  return action;
} /* qbi_svc_bc_nas_visible_providers_q_nas44_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_need_mode_pref_workaround
===========================================================================*/
/*!
    @brief Check whether we need to execute the WHCK-specific workaround to
    allow manual network scans

    @details
    Assumes that REGISTER_STATE has already been queried at least once.
    Since the modem does not support manual network scans when the mode
    preference includes 3GPP2 technologies, we must temporarily disable them
    to allow the network scan to complete successfully.

    @param ctx

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_visible_providers_q_need_mode_pref_workaround
(
  const qbi_ctx_s *ctx
)
{
  const qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  return (qbi_svc_bc_device_supports_3gpp2(ctx) &&
          (cache->register_state.qmi.mode_pref &
           (QMI_NAS_RAT_MODE_PREF_CDMA2000_1X_V01 |
            QMI_NAS_RAT_MODE_PREF_CDMA2000_HRPD_V01)) != 0 &&
          (cache->register_state.qmi.mode_pref &
           ~(QMI_NAS_RAT_MODE_PREF_CDMA2000_1X_V01 |
             QMI_NAS_RAT_MODE_PREF_CDMA2000_HRPD_V01)) != 0);
} /* qbi_svc_bc_nas_visible_providers_q_need_mode_pref_workaround() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_prepare_info
===========================================================================*/
/*!
    @brief Saves relevant information from a network scan result to the
    transaction's info field, in preparation for querying the modem for the
    name of each network

    @details

    @param txn
    @param ntw_info
    @param ntw_info_len Number of entries in ntw_info, pcs_digit_info, and
    rat_info
    @param pcs_digit_info
    @param rat_info
    @param skip_dups Set to TRUE to combine PLMNs visible on multiple RATs
    into a single entry

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_visible_providers_q_prepare_info
(
  qbi_txn_s                                          *txn,
  const nas_3gpp_network_info_type_v01               *ntw_info,
  uint32                                              ntw_info_len,
  const nas_mnc_pcs_digit_include_status_type_v01    *pcs_digit_info,
  const nas_network_radio_access_technology_type_v01 *rat_info,
  boolean                                             skip_dups
)
{
  qbi_svc_bc_provider_list_s *rsp;
  qbi_svc_bc_nas_visible_providers_q_info_s *info;
  int32 i;
  int32 j;
  boolean is_duplicate[NAS_3GPP_NETWORK_INFO_LIST_MAX_V01];
  uint32 num_providers;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(ntw_info);
  QBI_CHECK_NULL_PTR_RET_FALSE(pcs_digit_info);
  QBI_CHECK_NULL_PTR_RET_FALSE(rat_info);

  QBI_MEMSET(is_duplicate, 0, sizeof(is_duplicate));
  if (ntw_info_len > NAS_3GPP_NETWORK_INFO_LIST_MAX_V01)
  {
    QBI_LOG_E_2("Network info list too long! Got %d, max %d. Truncating result",
                ntw_info_len, NAS_3GPP_NETWORK_INFO_LIST_MAX_V01);
    ntw_info_len = NAS_3GPP_NETWORK_INFO_LIST_MAX_V01;
  }

  /* Determine how many unique PLMNs appear in the result. QMI will return the
     same PLMN info multiple times if it is visible on multiple RATs, but MBIM
     doesn't care about the RAT so it doesn't want duplicates. We need to
     determine this ahead of time so we can know how much memory to allocate. */
  num_providers = ntw_info_len;
  if (skip_dups)
  {
    for (i = 1; i < (int32) ntw_info_len; i++)
    {
      for (j = (i - 1); j >= 0; j--)
      {
        if (ntw_info[i].mobile_network_code == ntw_info[j].mobile_network_code &&
            ntw_info[i].mobile_country_code == ntw_info[j].mobile_country_code)
        {
          is_duplicate[i] = TRUE;
          num_providers--;
          QBI_LOG_D_3("Duplicate PLMN found: index %d matches %d. num_providers "
                      "now %d", i, j, num_providers);
          break;
        }
      }
    }
  }

  /* Allocate memory to hold the list header and the list of offset/size pairs
     for each provider. The rest of the data is treated as DataBuffer fields */
  rsp = (qbi_svc_bc_provider_list_s *)
    qbi_txn_alloc_rsp_buf(
      txn, (sizeof(qbi_svc_bc_provider_list_s) +
            num_providers * sizeof(qbi_mbim_offset_size_pair_s)));
  QBI_CHECK_NULL_PTR_RET_FALSE(rsp);

  txn->info = QBI_MEM_MALLOC_CLEAR(
    sizeof(qbi_svc_bc_nas_visible_providers_q_info_s));
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->info);

  info = (qbi_svc_bc_nas_visible_providers_q_info_s *) txn->info;
  rsp->element_count = num_providers;
  info->ntw_count = num_providers;
  for (i = 0; i < (int32) ntw_info_len; i++)
  {
    if (!is_duplicate[i])
    {
      info->ntw[info->cur_index].mcc = ntw_info[i].mobile_country_code;
      info->ntw[info->cur_index].mnc = ntw_info[i].mobile_network_code;
      info->ntw[info->cur_index].mnc_is_3_digits =
        pcs_digit_info[i].mnc_includes_pcs_digit;
      info->ntw[info->cur_index].network_status = ntw_info[i].network_status;
      info->ntw[info->cur_index].rat = rat_info[i].rat;
      info->cur_index++;
    }
  }
  info->cur_index = 0;
  success = TRUE;

  return success;
} /* qbi_svc_bc_nas_visible_providers_q_prepare_info() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_restore_mode_pref_completion_cb
===========================================================================*/
/*!
    @brief VISIBLE_PROVIDERS query completion callback used to restore the
    mode preference

    @details

    @param completed_txn
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_visible_providers_q_restore_mode_pref_completion_cb
(
  qbi_txn_s *completed_txn
)
{
  const qbi_svc_bc_nas_cache_s *cache;
  qbi_txn_s *txn;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(completed_txn);

  cache = qbi_svc_bc_nas_cache_get(completed_txn->ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  txn = qbi_txn_alloc(
    completed_txn->ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
    QBI_SVC_BC_MBIM_CID_REGISTER_STATE, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  action = qbi_svc_bc_nas_visible_providers_q_set_mode_pref(
    txn, qbi_svc_bc_nas_visible_providers_q_restore_mode_pref_nas33_rsp_cb,
    TRUE, cache->register_state.qmi.prev_mode_pref);

  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_nas_visible_providers_q_restore_mode_pref_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_restore_mode_pref_nas33_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP sent in
    response to a request to restore the mode preference to the cached value

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_restore_mode_pref_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_set_system_selection_preference_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_set_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
  {
    QBI_LOG_E_1("Error %d while trying to restore original mode preference",
                qmi_rsp->resp.error);
  }

  return QBI_SVC_ACTION_ABORT;
} /* qbi_svc_bc_nas_visible_providers_q_restore_mode_pref_nas33_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_set_mode_pref
===========================================================================*/
/*!
    @brief Sets a temporary mode preference

    @details
    Used in the WHCK mode preference workaround to allow network scans in
    automatic mode for multimode targets.

    @param txn
    @param rsp_cb Callback to handle the
    QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP
    @param restore_mode_pref Specify TRUE to restore the mode preference to
    the value provided in mode_pref, or FALSE to set the mode preference to
    the cached value from REGISTER_STATE minus 3GPP2 RATs (also results in
    allocating txn->info to save the previous preference and setting
    txn->completion_cb).

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_set_mode_pref
(
  qbi_txn_s              *txn,
  qbi_svc_qmi_rsp_cb_f   *rsp_cb,
  boolean                 restore_mode_pref,
  mode_pref_mask_type_v01 mode_pref
)
{
  qbi_svc_bc_nas_cache_s *cache;
  nas_set_system_selection_preference_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_req = (nas_set_system_selection_preference_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
      rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  /* Use a temporary change duration, so that the user requested mode preference
     will always be restored by power cycle. */
  qmi_req->change_duration_valid = TRUE;
  qmi_req->change_duration = NAS_POWER_CYCLE_V01;

  qmi_req->mode_pref_valid = TRUE;
  if (restore_mode_pref)
  {
    qmi_req->mode_pref = mode_pref;
    QBI_LOG_W_1("Restoring mode preference 0x%x after network scan completed",
                qmi_req->mode_pref);
  }
  else
  {
    qmi_req->mode_pref = (cache->register_state.qmi.mode_pref &
                          ~(QMI_NAS_RAT_MODE_PREF_CDMA2000_1X_V01 |
                            QMI_NAS_RAT_MODE_PREF_CDMA2000_HRPD_V01));
    QBI_LOG_W_2("Temporarily changing mode preference from 0x%x to 0x%x to allow "
                "network scan", cache->register_state.qmi.mode_pref,
                qmi_req->mode_pref);

    /* Save the previous mode preference and register a completion callback to
       restore it. */
    cache->register_state.qmi.prev_mode_pref =
      cache->register_state.qmi.mode_pref;

    qbi_txn_set_completion_cb(
      txn, (qbi_txn_completion_cb_f *)qbi_svc_bc_nas_visible_providers_q_restore_mode_pref_completion_cb);
  }

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_nas_visible_providers_q_set_mode_pref() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_set_mode_pref_nas33_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP sent as
    part of the WHCK mode preference workaround

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_set_mode_pref_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action;
  nas_set_system_selection_preference_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_set_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
  {
    /* Continue to issue the network scan request to the modem regardless of
       this failure. The request will most likely fail, but will be able to take
       advantage of the normal VISIBLE_PROVIDERS error handling logic. */
    QBI_LOG_W_1("Error %d while trying to set the 3GPP-only mode preference; "
                "attempting scan anyway", qmi_rsp->resp.error);
  }

  if (qbi_svc_bc_nas_is_registered(qmi_txn->ctx) &&
      qbi_svc_bc_nas_register_state_get_current_cellular_class(qmi_txn->ctx) ==
        QBI_SVC_BC_CELLULAR_CLASS_GSM)
  {
    /* Issue network scan request after delay, to allow the modem some time to
       register back to the network, since it will deregister because of the
       mode preference change we just made. */
    QBI_LOG_I_0("Delaying to allow re-registration before issuing scan request");
    qbi_txn_set_timeout(
      qmi_txn->parent, QBI_SVC_BC_NAS_VISIBLE_PROVIDERS_MODE_PREF_DELAY_MS,
      qbi_svc_bc_nas_visible_providers_q_set_mode_pref_timeout_cb);
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  else
  {
    action = qbi_svc_bc_nas_visible_providers_q_build_nas21_req(
      qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_nas_visible_providers_q_set_mode_pref_nas33_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_set_mode_pref_timeout_cb
===========================================================================*/
/*!
    @brief Timeout callback used to add a buffer between setting the mode
    preference and requesting a network scan

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_visible_providers_q_set_mode_pref_timeout_cb
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  /* Send the scan networks request - will also set a new timeout for the txn */
  (void) qbi_svc_proc_action(
    txn, qbi_svc_bc_nas_visible_providers_q_build_nas21_req(txn));
} /* qbi_svc_bc_nas_visible_providers_q_set_mode_pref_timeout_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_REGISTER_STATE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_cache_only_completion_cb
===========================================================================*/
/*!
    @brief Completion callback for the cache only timeout transaction

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_cache_only_completion_cb
(
  const qbi_txn_s *txn
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  cache->register_state.cache_only_txn = NULL;
  QBI_LOG_I_0("Cache only timeout txn completed");
} /* qbi_svc_bc_nas_register_state_cache_only_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_cache_only_set_timeout
===========================================================================*/
/*!
    @brief Manages the timeout used to place a limit on how much time we
    spend in cache only mode

    @details
    Cache only mode is entered when the modem loses its network registration
    while a data call is up. The host cannot handle the device being
    connected but not registered, so QBI spoofs that it is still registered.
    After a preset time limit is reached, QBI will end the data call,
    allowing it to exit cache only mode and report the true service status.

    @param ctx
    @param cache_only New cache only flag
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_cache_only_set_timeout
(
  qbi_ctx_s *ctx,
  boolean    cache_only
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache_only)
  {
    if (cache->register_state.cache_only_txn == NULL)
    {
      QBI_LOG_I_1("Setting time limit of %d ms for cache only mode",
                  QBI_SVC_BC_NAS_CACHE_ONLY_TIME_LIMIT_MS);
      cache->register_state.cache_only_txn = qbi_txn_alloc(
        ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
        QBI_SVC_BC_MBIM_CID_REGISTER_STATE, 0, NULL);
      QBI_CHECK_NULL_PTR_RET(cache->register_state.cache_only_txn);

      qbi_txn_set_completion_cb(
        cache->register_state.cache_only_txn,
        qbi_svc_bc_nas_register_state_cache_only_completion_cb);
      qbi_txn_set_timeout(
        cache->register_state.cache_only_txn,
        QBI_SVC_BC_NAS_CACHE_ONLY_TIME_LIMIT_MS,
        qbi_svc_bc_nas_register_state_cache_only_timeout_cb);

      (void) qbi_svc_proc_action(
        cache->register_state.cache_only_txn, QBI_SVC_ACTION_WAIT_ASYNC_RSP);
    }
    else
    {
      QBI_LOG_W_0("Cache only mode enabled, but timeout txn already exists!");
    }
  }
  else
  {
    if (cache->register_state.cache_only_txn != NULL)
    {
      QBI_LOG_I_0("Left cache only mode before timeout; aborting transaction");
      (void) qbi_svc_proc_action(
        cache->register_state.cache_only_txn, QBI_SVC_ACTION_ABORT);
      /* The completion callback will set cache_only_txn to NULL */
    }
    else
    {
      QBI_LOG_W_0("Left cache only mode with no timeout txn available");
    }
  }
} /* qbi_svc_bc_nas_register_state_cache_only_set_timeout() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_cache_only_timeout_cb
===========================================================================*/
/*!
    @brief Timeout callback invoked when the cache only time limit is
    reached

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_cache_only_timeout_cb
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache->register_state.cache_only)
  {
    QBI_LOG_I_0("Cache only mode time limit reached; disconnecting");
    cache->register_state.cache_only_caused_disconnect = TRUE;
    qbi_svc_bc_connect_deactivate_all_sessions(txn->ctx);
  }
  else
  {
    QBI_LOG_W_0("Cache only mode time limit reached, but cache only mode "
                "disabled!");
  }
  (void) qbi_svc_proc_action(txn, QBI_SVC_ACTION_ABORT);
} /* qbi_svc_bc_nas_register_state_cache_only_timeout_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_cached_provider_id_is_valid
===========================================================================*/
/*!
    @brief Checks whether the ProviderId in the MBIM_CID_REGISTER_STATE cache
    is valid (i.e. not empty)

    @details

    @param ctx

    @return boolean TRUE if valid, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_cached_provider_id_is_valid
(
  const qbi_ctx_s *ctx
)
{
  const qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  return (cache->register_state.provider_id_ascii[0] != '\0');
} /* qbi_svc_bc_nas_register_state_cached_provider_id_is_valid() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_data_class_rat_matches
===========================================================================*/
/*!
    @brief Checks whether the Radio Access Technology matches between to
    MBIM_DATA_CLASS values

    @details
    Match is found when one of the bits in requested data class is also set
    in available data class, or if one of the bits in both data classes
    belong to the same data class family.

    @param available_data_class
    @param requested_data_class

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_data_class_rat_matches
(
  uint32 available_data_class,
  uint32 requested_data_class
)
{
/*-------------------------------------------------------------------------*/
  return ((requested_data_class & available_data_class) != 0 ||
          ((requested_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_GSM) != 0 &&
           (available_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_GSM) != 0) ||
          ((requested_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_WCDMA) != 0 &&
           (available_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_WCDMA) != 0) ||
          ((requested_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_HDR) != 0 &&
           (available_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_HDR) != 0));
} /* qbi_svc_bc_nas_register_state_data_class_rat_matches() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_compare
===========================================================================*/
/*!
    @brief Comparison function for qbi_util_binary_search that operates on
    an ERI table

    @details

    @param array Pointer to start of ERI table
    @param index Index of ERI table to compare against key
    @param key Pointer to roaming indicator

    @return qbi_util_comparison_result_e
*/
/*=========================================================================*/
static qbi_util_comparison_result_e qbi_svc_bc_nas_register_state_eri_compare
(
  const void *array,
  uint32      index,
  const void *key
)
{
  const qbi_svc_bc_nas_eri_tbl_entry_s *tbl;
  const uint8 *roam_ind;
  qbi_util_comparison_result_e result;
/*-------------------------------------------------------------------------*/
  tbl = (const qbi_svc_bc_nas_eri_tbl_entry_s *) array;
  roam_ind = (const uint8 *) key;
  if (array == NULL || key == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer");
    result = QBI_UTIL_COMPARISON_RESULT_ERROR;
  }
  else if (tbl[index].roam_ind > *roam_ind)
  {
    result = QBI_UTIL_COMPARISON_RESULT_GREATER_THAN;
  }
  else if (tbl[index].roam_ind < *roam_ind)
  {
    result = QBI_UTIL_COMPARISON_RESULT_LESS_THAN;
  }
  else
  {
    result = QBI_UTIL_COMPARISON_RESULT_EQUAL_TO;
  }

  return result;
} /* qbi_svc_bc_nas_register_state_eri_compare() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_parse_spr
===========================================================================*/
/*!
    @brief Attempts to read and parse the contents of a Sprint ERI file into
    a more friendly format stored in the cache

    @details
    If an ERI file is present and valid, cache->register_state.eri.tbl will
    be allocated and populated with relevant data pulled from the file. The
    ERI cache must be reset before invoking this function, e.g. by
    calling qbi_svc_bc_nas_register_state_eri_reset().

    @param ctx
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_eri_parse_spr
(
  qbi_ctx_s *ctx
)
{
  qbi_util_buf_s buf;
  uint8 num_entries = 0;
  const char *file_buf;
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  qbi_util_buf_init(&buf);
  if (qbi_nv_store_file_read(ctx, QBI_NV_STORE_FILE_ID_ERI_SPR, &buf))
  {
    /* The file starts with the number of entries (max 255), followed by ; */
    file_buf = (const char *) buf.data;
    if (!qbi_util_ascii_decimal_str_to_uint8(file_buf, buf.size, &num_entries))
    {
      QBI_LOG_E_0("Couldn't parse first line of ERI.txt!");
    }
    else if (num_entries == 0)
    {
      QBI_LOG_W_0("ERI file contains no roaming indicator entries!");
    }
    else
    {
      cache->register_state.eri.tbl = QBI_MEM_MALLOC_CLEAR(
        (sizeof(qbi_svc_bc_nas_eri_tbl_entry_s) * num_entries));
      if (cache->register_state.eri.tbl == NULL)
      {
        QBI_LOG_E_1("Couldn't allocate %d bytes for ERI table!",
                    (sizeof(qbi_svc_bc_nas_eri_tbl_entry_s) * num_entries));
      }
      else if (!qbi_svc_bc_nas_register_state_eri_parse_spr_entries(
                 file_buf, buf.size, num_entries, cache->register_state.eri.tbl))
      {
        QBI_LOG_E_0("Failure parsing ERI entries");
        QBI_MEM_FREE(cache->register_state.eri.tbl);
        cache->register_state.eri.tbl = NULL;
      }
      else
      {
        QBI_LOG_I_1("Successfully parsed ERI file with %d entries", num_entries);
        cache->register_state.eri.num_tbl_entries = num_entries;
      }
    }

    qbi_util_buf_free(&buf);
  }
} /* qbi_svc_bc_nas_register_state_eri_parse_spr() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_parse_spr_entries
===========================================================================*/
/*!
    @brief Parses each entry of the Sprint ERI file

    @details

    @param eri_data Buffer containing the complete ERI file
    @param eri_data_len Size of eri_data in bytes
    @param num_eri_entries Number of ERI entries contained in eri_data
    @param parsed_eri_tbl Array with minimum size num_eri_entries which will
    be populated with parsed ERI data

    @return boolean TRUE if all entries successfully parsed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_eri_parse_spr_entries
(
  const char                     *eri_data,
  uint32                          eri_data_len,
  uint8                           num_eri_entries,
  qbi_svc_bc_nas_eri_tbl_entry_s *parsed_eri_tbl
)
{
  uint32 i;
  uint32 offset = 0;
  uint8 last_roam_ind = 0;
  boolean found;
  uint32 separator_count;
  uint32 roam_text_len;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(eri_data);
  QBI_CHECK_NULL_PTR_RET_FALSE(parsed_eri_tbl);

  /* The Sprint ERI file starts with <num_entries>;\r\n followed by ERI entries
     with format: <roam_ind>;<icon_mode>;<roam_text>;<banner_mode>;\r\n */
  for (i = 0; i < num_eri_entries; i++)
  {
    /* Advance offset to the start of the current line (first character
       following \r\n) */
    found = FALSE;
    while (!found && (offset + 1) < eri_data_len)
    {
      if (eri_data[offset] == '\r' && eri_data[offset + 1] == '\n')
      {
        found = TRUE;
        offset++;
      }
      offset++;
    }
    if (!found)
    {
      QBI_LOG_E_1("Couldn't find start of entry %d", i);
      success = FALSE;
      break;
    }

    /* Parse the first field - roaming indicator, and enforce ordering */
    if (!qbi_util_ascii_decimal_str_to_uint8(
          &eri_data[offset], (eri_data_len - offset),
          &parsed_eri_tbl[i].roam_ind))
    {
      QBI_LOG_E_1("Failure parsing roam_ind at entry %d", i);
      success = FALSE;
      break;
    }
    if (last_roam_ind > parsed_eri_tbl[i].roam_ind)
    {
      QBI_LOG_E_2("Roaming indicator %d at index %d is not in order!",
                  parsed_eri_tbl[i].roam_ind, i);
      success = FALSE;
      break;
    }
    last_roam_ind = parsed_eri_tbl[i].roam_ind;

    /* Advance offset to the start of the third field - roaming text */
    separator_count = 0;
    while (separator_count < 2 && offset < eri_data_len)
    {
      if (eri_data[offset++] == ';')
      {
        separator_count++;
      }
    }
    if (separator_count != 2)
    {
      QBI_LOG_E_1("Couldn't find start of roam_text at entry %d", i);
      success = FALSE;
      break;
    }

    /* Determine the length of the roaming text field by finding the trailing
       semicolon */
    roam_text_len = 0;
    found = FALSE;
    while (!found && (offset + roam_text_len) < eri_data_len)
    {
      if (eri_data[offset + roam_text_len] == ';')
      {
        found = TRUE;
        break;
      }
      roam_text_len++;
    }
    if (!found)
    {
      QBI_LOG_E_1("Couldn't find end of roaming text at entry %d", i);
      success = FALSE;
      break;
    }
    if (roam_text_len > QBI_SVC_BC_NAS_ERI_SPR_ROAM_TEXT_MAX_LEN_BYTES)
    {
      /* We set the maximum length based on the longest entry currently in the
         file - if longer names need to be accomodated, the constant should be
         increased, while minding the additional memory usage. If that happens,
         may want to consider using a storage method that is more memory
         efficient. */
      QBI_LOG_E_2("Roaming text length %d at entry %d is too large and will be "
                  "truncated!", roam_text_len, i);
      roam_text_len = QBI_SVC_BC_NAS_ERI_SPR_ROAM_TEXT_MAX_LEN_BYTES;
    }

    /* Roaming text is not NULL terminated in the file, but we do NULL term the
       parsed buffer via zero-init + 1 byte pad */
    QBI_MEMSCPY(parsed_eri_tbl[i].roam_text_ascii,
                sizeof(parsed_eri_tbl[i].roam_text_ascii),
                &eri_data[offset], roam_text_len);

    /* Advance the offset past roam_text, its trailing semicolon, the minimum 1
       digit for the following field and its trailing semicolon, to speed up
       finding the start of the next line */
    offset += roam_text_len + 3;
  }

  return success;
} /* qbi_svc_bc_nas_register_state_eri_parse_spr_entries() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_parse_spr_intl_roam
===========================================================================*/
/*!
    @brief Attempts to read and parse the contents of a Sprint ERI
    international roaming file into the cache

    @details
    If the file is present and valid, cache->register_state.eri.intl_roam_list
    will be allocated and populated with relevant data pulled from the file.
    The ERI cache must be reset before invoking this function, e.g. by
    calling qbi_svc_bc_nas_register_state_eri_reset().

    @param ctx
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_eri_parse_spr_intl_roam
(
  qbi_ctx_s *ctx
)
{
  qbi_util_buf_s buf;
  uint8 num_entries = 0;
  const char *file_buf;
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  qbi_util_buf_init(&buf);
  if (qbi_nv_store_file_read(ctx, QBI_NV_STORE_FILE_ID_INTL_ROAM_SPR, &buf))
  {
    /* The file starts with the number of entries (max 255), followed by \r\n */
    file_buf = (const char *) buf.data;
    if (!qbi_util_ascii_decimal_str_to_uint8(file_buf, buf.size, &num_entries))
    {
      QBI_LOG_E_0("Couldn't parse first line of ERIInt.txt!");
    }
    else if (num_entries == 0)
    {
      QBI_LOG_W_0("Sprint intl roam file contains no entries!");
    }
    else
    {
      cache->register_state.eri.intl_roam_list = QBI_MEM_MALLOC_CLEAR(
        (sizeof(uint8) * num_entries));
      if (cache->register_state.eri.intl_roam_list == NULL)
      {
        QBI_LOG_E_1("Couldn't allocate %d bytes for intl roam list!",
                    (sizeof(uint8) * num_entries));
      }
      else if (!qbi_svc_bc_nas_register_state_eri_parse_spr_intl_roam_entries(
                 file_buf, buf.size, num_entries,
                 cache->register_state.eri.intl_roam_list))
      {
        QBI_LOG_E_0("Failure parsing intl roam file");
        QBI_MEM_FREE(cache->register_state.eri.intl_roam_list);
        cache->register_state.eri.intl_roam_list = NULL;
      }
      else
      {
        QBI_LOG_I_1("Successfully parsed intl roam file with %d entries",
                    num_entries);
        cache->register_state.eri.intl_roam_list_len = num_entries;
      }
    }

    qbi_util_buf_free(&buf);
  }
} /* qbi_svc_bc_nas_register_state_eri_parse_spr_intl_roam() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_parse_spr_intl_roam_entries
===========================================================================*/
/*!
    @brief Parses each entry of the Sprint ERI international roaming file

    @details

    @param file_data Buffer containing the complete intl roam file
    @param file_data_len Size of file_data in bytes
    @param intl_roam_list_len Number of roaming indicators in the file
    @param intl_roam_list Array of size intl_roam_list_len to populate with
    roaming indicators parsed from file_data

    @return boolean TRUE if all intl_roam_list_len indicators parsed
    successfully, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_eri_parse_spr_intl_roam_entries
(
  const char *file_data,
  uint32      file_data_len,
  uint8       intl_roam_list_len,
  uint8      *intl_roam_list
)
{
  uint32 i;
  uint32 offset = 0;
  uint8 last_roam_ind = 0;
  boolean found;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(file_data);
  QBI_CHECK_NULL_PTR_RET_FALSE(intl_roam_list);

  /* The Sprint International Roaming list file starts with <num_entries>
     followed by entries with format: \r\n<roam_ind> */
  for (i = 0; i < intl_roam_list_len; i++)
  {
    /* Advance offset to the start of the current line (first character
       following \r\n) */
    found = FALSE;
    while (!found && (offset + 1) < file_data_len)
    {
      if (file_data[offset] == '\r' && file_data[offset + 1] == '\n')
      {
        found = TRUE;
        offset++;
      }
      offset++;
    }
    if (!found)
    {
      QBI_LOG_E_1("Couldn't find start of entry %d", i);
      success = FALSE;
      break;
    }

    /* Parse the roaming indicator */
    if (!qbi_util_ascii_decimal_str_to_uint8(
          &file_data[offset], (file_data_len - offset), &intl_roam_list[i]))
    {
      QBI_LOG_E_1("Failure parsing roam_ind at entry %d", i);
      success = FALSE;
      break;
    }

    /* Enforce ascending order */
    if (last_roam_ind > intl_roam_list[i])
    {
      QBI_LOG_E_2("Roaming indicator %d at index %d is not in order!",
                  intl_roam_list[i], i);
      success = FALSE;
      break;
    }
    last_roam_ind = intl_roam_list[i];
  }

  return success;
} /* qbi_svc_bc_nas_register_state_eri_parse_spr_intl_roam_entries() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_parse_vzw
===========================================================================*/
/*!
    @brief Attempts to read and parse the contents of a Verizon ERI file into
    a more friendly format stored in the cache

    @details
    If an ERI file is present and valid, cache->register_state.eri.tbl will
    be allocated and populated with relevant data pulled from the file. The
    ERI cache must be reset before invoking this function, e.g. by
    calling qbi_svc_bc_nas_register_state_eri_reset().

    @param ctx
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_eri_parse_vzw
(
  qbi_ctx_s *ctx
)
{
  qbi_util_buf_s buf;
  const uint8 *eri_data;
  uint16 eri_version;
  uint8 eri_entries;
  uint8 eri_type;
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  qbi_util_buf_init(&buf);
  if (qbi_nv_store_file_read(ctx, QBI_NV_STORE_FILE_ID_ERI_VZW, &buf))
  {
    if (buf.size < QBI_SVC_BC_NAS_ERI_VZW_MIN_LEN)
    {
      QBI_LOG_E_1("ERI file too small! Size %d", buf.size);
    }
    else
    {
      eri_data = (const uint8 *) buf.data;

      /* Extract first three fields from ERI: Version (16 bits), Number of ERI
         entries (6 bits), ERI Type (3 bits) */
      eri_version = (eri_data[0] << 8) | eri_data[1];
      eri_entries = (eri_data[2] & 0xFC) >> 2;
      eri_type = ((eri_data[2] & 0x03) << 1) | ((eri_data[3] & 0x80) >> 7);
      QBI_LOG_I_3("Parsing ERI.bin version %d type %d with %d entries",
                  eri_version, eri_type, eri_entries);

      if (eri_entries > QBI_SVC_BC_NAS_ERI_VZW_ROAM_IND_TBL_MAX_ENTRIES)
      {
        QBI_LOG_E_1("Number of ERI entries (%d) exceeds spec", eri_entries);
      }
      else if (eri_entries == 0)
      {
        QBI_LOG_W_0("ERI file contains no roaming indicator entries!");
      }
      else
      {
        cache->register_state.eri.tbl = QBI_MEM_MALLOC_CLEAR(
          sizeof(qbi_svc_bc_nas_eri_tbl_entry_s) * eri_entries);
        if (cache->register_state.eri.tbl == NULL)
        {
          QBI_LOG_E_1("Couldn't allocate %d bytes for ERI table!",
                      (sizeof(qbi_svc_bc_nas_eri_tbl_entry_s) * eri_entries));
        }
        else if (!qbi_svc_bc_nas_register_state_eri_parse_vzw_entries(
                   eri_data, buf.size, eri_entries,
                   cache->register_state.eri.tbl))
        {
          QBI_LOG_E_0("Failure parsing ERI entries");
          QBI_MEM_FREE(cache->register_state.eri.tbl);
          cache->register_state.eri.tbl = NULL;
        }
        else
        {
          QBI_LOG_I_0("Successfully parsed ERI file");
          cache->register_state.eri.num_tbl_entries = eri_entries;
        }
      }
    }
    qbi_util_buf_free(&buf);
  }
} /* qbi_svc_bc_nas_register_state_eri_parse_vzw() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_parse_vzw_entries
===========================================================================*/
/*!
    @brief Parses individual ERI entries from a Verizon ERI file into a
    format for use in QBI

    @details

    @param eri_data Buffer containing the complete ERI file
    @param eri_data_len Size of the eri_data buffer in bytes
    @param num_eri_entries Number of ERI entries in eri_data
    @param parsed_eri_tbl Array with minimum size num_eri_entries which will
    be populated with parsed ERI data

    @return boolean TRUE if all entries successfully parsed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_eri_parse_vzw_entries
(
  const uint8                    *eri_data,
  uint32                          eri_data_len,
  uint8                           num_eri_entries,
  qbi_svc_bc_nas_eri_tbl_entry_s *parsed_eri_tbl
)
{
  uint8 i;
  uint32 offset;
  uint8 encoding;
  uint8 roam_text_len;
  uint8 last_roam_ind = 0;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(eri_data);
  QBI_CHECK_NULL_PTR_RET_FALSE(parsed_eri_tbl);

  /* Skip over the header and 3 call prompt tables to find the start of the roam
     indicator table. */
  offset = QBI_SVC_BC_NAS_ERI_VZW_HEADER_LEN_BYTES;
  for (i = 0; i < QBI_SVC_BC_NAS_ERI_VZW_NUM_CALL_PROMPT_TBLS; i++)
  {
    offset += (QBI_SVC_BC_NAS_ERI_VZW_CALL_PROMPT_TBL_LEN_FIXED +
               eri_data[offset + 3]);
  }

  for (i = 0; i < num_eri_entries; i++)
  {
    if (offset > (eri_data_len - QBI_SVC_BC_NAS_ERI_VZW_ROAM_IND_TBL_LEN_FIXED))
    {
      QBI_LOG_E_1("Not enough data left in file for ERI entry %d", i);
      success = FALSE;
      break;
    }

    parsed_eri_tbl[i].roam_ind = eri_data[offset];
    if (last_roam_ind > parsed_eri_tbl[i].roam_ind)
    {
      QBI_LOG_E_2("Roaming indicator %d at index %d is not in order!",
                  parsed_eri_tbl[i].roam_ind, i);
      success = FALSE;
      break;
    }
    last_roam_ind = parsed_eri_tbl[i].roam_ind;
    encoding = (eri_data[offset + 2] & 0x1F);
    roam_text_len = eri_data[offset + 3];
    if (roam_text_len > QBI_SVC_BC_NAS_ERI_VZW_ROAM_TEXT_MAX_LEN_BYTES)
    {
      QBI_LOG_E_2("ERI entry %d: invalid text len %d", i, roam_text_len);
      success = FALSE;
      break;
    }
    else if (offset + 4 + roam_text_len > eri_data_len)
    {
      QBI_LOG_E_1("Not enough data left in file for roam text at index %d", i);
      success = FALSE;
      break;
    }

    /* Roaming text is not NULL terminated in the file, but we do NULL term the
       parsed buffer via zero-init + 1 byte pad */
    QBI_MEMSCPY(parsed_eri_tbl[i].roam_text_ascii,
                sizeof(parsed_eri_tbl[i].roam_text_ascii),
                &eri_data[offset + 4], roam_text_len);
    if (encoding == QBI_SVC_BC_NAS_ERI_VZW_ENCODING_GSM)
    {
      qbi_util_convert_gsm_alphabet_to_ascii(
        (uint8 *) parsed_eri_tbl[i].roam_text_ascii, roam_text_len);
    }
    else if (encoding != QBI_SVC_BC_NAS_ERI_VZW_ENCODING_ASCII &&
             encoding != QBI_SVC_BC_NAS_ERI_VZW_ENCODING_IA5)
    {
      QBI_LOG_E_2("Unsupported encoding %d at ERI entry %d", encoding, i);
      success = FALSE;
      break;
    }

    QBI_LOG_STR_3("%d. Roaming Indication = %d Text = '%s'",
                  i, parsed_eri_tbl[i].roam_ind,
                  parsed_eri_tbl[i].roam_text_ascii);
    offset += (QBI_SVC_BC_NAS_ERI_VZW_ROAM_IND_TBL_LEN_FIXED + roam_text_len);
  }

  return success;
} /* qbi_svc_bc_nas_register_state_eri_parse_vzw_entries() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_reset
===========================================================================*/
/*!
    @brief Resets the ERI cache to its initial state

    @details

    @param cache
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_eri_reset
(
  qbi_svc_bc_nas_cache_s *cache
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache->register_state.eri.algo != QBI_SVC_BC_NAS_ERI_ALGO_NONE)
  {
    if (cache->register_state.eri.tbl != NULL)
    {
      QBI_MEM_FREE(cache->register_state.eri.tbl);
    }
    if (cache->register_state.eri.intl_roam_list != NULL)
    {
      QBI_MEM_FREE(cache->register_state.eri.intl_roam_list);
    }
    QBI_MEMSET(&cache->register_state.eri, 0, sizeof(cache->register_state.eri));
    QBI_LOG_D_0("Reset ERI cache");
  }
  else if (cache->register_state.eri.tbl != NULL ||
           cache->register_state.eri.intl_roam_list != NULL)
  {
    /* Log potential memory leak/corruption */
    QBI_LOG_E_0("ERI/intl roam table not NULL with ERI algo set to NONE!!!");
  }
} /* qbi_svc_bc_nas_register_state_eri_reset() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state
===========================================================================*/
/*!
    @brief Maps a QMI NAS roaming status enum to an MBIM RegisterState using
    rules associated with the given ERI algorithm

    @details

    @param cache
    @param roam_status

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state
(
  const qbi_svc_bc_nas_cache_s *cache,
  nas_roam_status_enum_type_v01 roam_status
)
{
  uint32 register_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  if (cache->register_state.eri.algo == QBI_SVC_BC_NAS_ERI_ALGO_VZW &&
      cache->register_state.current_cellular_class ==
        QBI_SVC_BC_CELLULAR_CLASS_CDMA)
  {
    register_state =
      qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state_vzw(
        roam_status);
  }
  else if (cache->register_state.eri.algo == QBI_SVC_BC_NAS_ERI_ALGO_SPR &&
           cache->register_state.current_cellular_class ==
             QBI_SVC_BC_CELLULAR_CLASS_CDMA)
  {
    register_state =
      qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state_spr(
        cache, roam_status);
  }
  else
  {
    switch (roam_status)
    {
      case NAS_SYS_ROAM_STATUS_OFF_V01:
        register_state = QBI_SVC_BC_REGISTER_STATE_HOME;
        break;

      case NAS_SYS_ROAM_STATUS_BLINK_V01:
        register_state = QBI_SVC_BC_REGISTER_STATE_PARTNER;
        break;

      case NAS_SYS_ROAM_STATUS_ON_V01:
        register_state = QBI_SVC_BC_REGISTER_STATE_ROAMING;
        break;

      default:
        QBI_LOG_W_1("Unexpected roam_status %d outside of ERI condition, "
                    "assuming HOME", roam_status);
        register_state = QBI_SVC_BC_REGISTER_STATE_HOME;
    }
  }

  return register_state;
} /* qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state_vzw
===========================================================================*/
/*!
    @brief Maps a CDMA roaming status enum to MBIM RegisterState following
    ERI rules for Verizon

    @details

    @param roam_status

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state_vzw
(
  nas_roam_status_enum_type_v01 roam_status
)
{
  uint8 roam_ind;
  uint32 register_state;
/*-------------------------------------------------------------------------*/
  /* Roaming indicator values 1, 64, 65, and 76-83 are considered home. Refer
       to Verizon Enhanced Roaming Indicators Requirements Section 2.1 */
  roam_ind = qbi_svc_bc_nas_register_state_qmi_roam_status_to_roam_ind(
    roam_status);
  if (roam_ind == 1 || roam_ind == 64 || roam_ind == 65 ||
      (roam_ind >= 76 && roam_ind <= 83))
  {
    register_state = QBI_SVC_BC_REGISTER_STATE_HOME;
  }
  else
  {
    register_state = QBI_SVC_BC_REGISTER_STATE_ROAMING;
  }
  QBI_LOG_D_2("Roaming indicator %d mapped to RegisterState %d per VZW",
              roam_ind, register_state);

  return register_state;
} /* qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state_vzw() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state_spr
===========================================================================*/
/*!
    @brief Maps a CDMA roaming status enum to MBIM RegisterState following
    ERI rules and the international roaming list for Sprint

    @details

    @param cache
    @param roam_status

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state_spr
(
  const qbi_svc_bc_nas_cache_s *cache,
  nas_roam_status_enum_type_v01 roam_status
)
{
  uint8 roam_ind;
  uint8 i;
  uint32 register_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  if (roam_status == NAS_SYS_ROAM_STATUS_OFF_V01)
  {
    register_state = QBI_SVC_BC_REGISTER_STATE_HOME;
  }
  else if (cache->register_state.eri.intl_roam_list != NULL)
  {
    /* If the roaming indicator appears on the international roaming list, it
       is ROAMING, else PARTNER. */
    roam_ind = qbi_svc_bc_nas_register_state_qmi_roam_status_to_roam_ind(
      roam_status);
    for (i = 0; i < cache->register_state.eri.intl_roam_list_len; i++)
    {
      if (roam_ind <= cache->register_state.eri.intl_roam_list[i])
      {
        break;
      }
    }

    if (i >= cache->register_state.eri.intl_roam_list_len ||
        roam_ind != cache->register_state.eri.intl_roam_list[i])
    {
      register_state = QBI_SVC_BC_REGISTER_STATE_PARTNER;
    }
    else
    {
      register_state = QBI_SVC_BC_REGISTER_STATE_ROAMING;
    }
  }
  else
  {
    QBI_LOG_I_1("Mapping roam_status %d to ROAMING since no Sprint intl roam "
                "list available", roam_status);
    register_state = QBI_SVC_BC_REGISTER_STATE_ROAMING;
  }

  QBI_LOG_D_2("Roaming status %d mapped to RegisterState %d per SPR",
              roam_status, register_state);
  return register_state;
} /* qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state_spr() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_nw_error_is_perm
===========================================================================*/
/*!
    @brief Checks whether the sys_info structure contains a network reject
    cause code that makes the SIM invalid for CS or CS+PS service until SIM
    reset/removal

    @details

    @param nw_error

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_nw_error_is_perm
(
  uint32 nw_error
)
{
/*-------------------------------------------------------------------------*/
  return (nw_error == QBI_SVC_BC_NAS_REJECT_CAUSE_IMSI_UNKNOWN_IN_HLR ||
          nw_error == QBI_SVC_BC_NAS_REJECT_CAUSE_ILLEGAL_MS ||
          nw_error == QBI_SVC_BC_NAS_REJECT_CAUSE_ILLEGAL_ME ||
          nw_error == QBI_SVC_BC_NAS_REJECT_CAUSE_GPRS_AND_NON_GPRS_SERVICES_NOT_ALLOWED);
} /* qbi_svc_bc_nas_register_state_nw_error_is_perm() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_qmi_roam_status_to_roam_ind
===========================================================================*/
/*!
    @brief Converts a QMI NAS roam status to a roaming indicator value

    @details

    @param roam_status

    @return uint8
*/
/*=========================================================================*/
static uint8 qbi_svc_bc_nas_register_state_qmi_roam_status_to_roam_ind
(
  nas_roam_status_enum_type_v01 roam_status
)
{
  uint8 roam_ind;
/*-------------------------------------------------------------------------*/
  /* QMI NAS uses roam_status = 0 to indicate ROAM_OFF, and roam_status = 1 for
     ROAM_ON in QMI_NAS_SYS_INFO, whereas the actual roaming indicator
     used in QMI_NAS_SERVING_SYSTEM and referenced in ERI requirements uses
     the reverse mapping. */
  roam_ind = (uint8) roam_status;
  if (roam_ind <= 1)
  {
    roam_ind ^= 0x1;
  }

  return roam_ind;
} /* qbi_svc_bc_nas_register_state_qmi_roam_status_to_roam_ind() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_sys_info_to_available_data_class
===========================================================================*/
/*!
    @brief Determines the current MBIM AvailableDataClass value based off the
    common system information structure

    @details

    @param sys_info

    @return uint32 MBIM_DATA_CLASS bitmask
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_register_state_sys_info_to_available_data_class
(
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
  uint32 available_data_class;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(sys_info);

  switch (sys_info->primary_rat)
  {
    case QBI_SVC_BC_NAS_PRIMARY_RAT_CDMA:
      available_data_class = QBI_SVC_BC_DATA_CLASS_1XRTT;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_HDR:
      QBI_CHECK_NULL_PTR_RET_ZERO(sys_info->rat.hdr);
      if (sys_info->rat.hdr->hdr_specific_sys_info.hdr_active_prot_valid)
      {
        switch (sys_info->rat.hdr->hdr_specific_sys_info.hdr_active_prot)
        {
          case NAS_SYS_ACTIVE_PROT_HDR_RELA_V01:
            available_data_class = QBI_SVC_BC_DATA_CLASS_1XEVDO_REVA;
            break;

          case NAS_SYS_ACTIVE_PROT_HDR_RELB_V01:
            available_data_class = QBI_SVC_BC_DATA_CLASS_1XEVDO_REVB;
            break;

          case NAS_SYS_ACTIVE_PROT_HDR_REL0_V01:
          default:
            available_data_class = QBI_SVC_BC_DATA_CLASS_1XEVDO;
        }
      }
      else
      {
        available_data_class = QBI_SVC_BC_DATA_CLASS_1XEVDO;
      }

      if (sys_info->hybrid_srv_status == NAS_SYS_SRV_STATUS_SRV_V01)
      {
        available_data_class |= QBI_SVC_BC_DATA_CLASS_1XRTT;
      }
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_GSM:
      QBI_CHECK_NULL_PTR_RET_ZERO(sys_info->rat.gsm);
      if (sys_info->rat.gsm->gsm_specific_sys_info.egprs_supp_valid &&
          sys_info->rat.gsm->gsm_specific_sys_info.egprs_supp)
      {
        available_data_class = QBI_SVC_BC_DATA_CLASS_EDGE;
      }
      else
      {
        available_data_class = QBI_SVC_BC_DATA_CLASS_GPRS;
      }
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_WCDMA:
      QBI_CHECK_NULL_PTR_RET_ZERO(sys_info->rat.wcdma);
      available_data_class = QBI_SVC_BC_DATA_CLASS_UMTS;

      /* Include the superset of HSDPA+HSUPA from the cell capability indicator
         (hs_ind) and the active data call properties (hs_call_status) */
      if (sys_info->rat.wcdma->wcdma_specific_sys_info.hs_ind_valid)
      {
        available_data_class |=
          qbi_svc_bc_nas_register_state_qmi_hs_supp_to_mbim_data_class(
            sys_info->rat.wcdma->wcdma_specific_sys_info.hs_ind);
      }
      if (sys_info->rat.wcdma->wcdma_specific_sys_info.hs_call_status_valid)
      {
        available_data_class |=
          qbi_svc_bc_nas_register_state_qmi_hs_supp_to_mbim_data_class(
            sys_info->rat.wcdma->wcdma_specific_sys_info.hs_call_status);
      }
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_LTE:
      available_data_class = QBI_SVC_BC_DATA_CLASS_LTE;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_TDSCDMA:
      QBI_CHECK_NULL_PTR_RET_ZERO(sys_info->rat.tdscdma);
      available_data_class = QBI_SVC_BC_DATA_CLASS_CUSTOM;

      /* Include the superset of HSDPA+HSUPA from the cell capability indicator
         (hs_ind) and the active data call properties (hs_call_status) */
      if (sys_info->rat.tdscdma->tdscdma_specific_sys_info.hs_ind_valid)
      {
        available_data_class |=
          qbi_svc_bc_nas_register_state_qmi_hs_supp_to_mbim_data_class(
            sys_info->rat.tdscdma->tdscdma_specific_sys_info.hs_ind);
      }
      if (sys_info->rat.tdscdma->tdscdma_specific_sys_info.hs_call_status_valid)
      {
        available_data_class |=
          qbi_svc_bc_nas_register_state_qmi_hs_supp_to_mbim_data_class(
            sys_info->rat.tdscdma->tdscdma_specific_sys_info.hs_call_status);
      }
      break;

    default:
      available_data_class = QBI_SVC_BC_DATA_CLASS_NONE;
  }

  return available_data_class;
} /* qbi_svc_bc_nas_register_state_sys_info_to_available_data_class() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_sys_info_to_provider_id_3gpp
===========================================================================*/
/*!
    @brief Converts the 3GPP network ID into MBIM format with ASCII encoding

    @details

    @param sys_info
    @param provider_id_ascii Buffer to populate with the ProviderId
    @param provider_id_ascii_size Size of the provider_id_ascii buffer in
    bytes
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_sys_info_to_provider_id_3gpp
(
  const qbi_svc_bc_nas_sys_info_s *sys_info,
  char                            *provider_id_ascii,
  uint32                           provider_id_ascii_size
)
{
  uint32 i;
  boolean network_id_valid = FALSE;
  const nas_common_network_id_type_v01 *network_id = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(sys_info);
  QBI_CHECK_NULL_PTR_RET(provider_id_ascii);

  switch (sys_info->primary_rat)
  {
    case QBI_SVC_BC_NAS_PRIMARY_RAT_GSM:
      QBI_CHECK_NULL_PTR_RET(sys_info->rat.gsm);
      network_id_valid =
        sys_info->rat.gsm->threegpp_specific_sys_info.network_id_valid;
      network_id = &sys_info->rat.gsm->threegpp_specific_sys_info.network_id;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_WCDMA:
      QBI_CHECK_NULL_PTR_RET(sys_info->rat.wcdma);
      network_id_valid =
        sys_info->rat.wcdma->threegpp_specific_sys_info.network_id_valid;
      network_id = &sys_info->rat.wcdma->threegpp_specific_sys_info.network_id;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_LTE:
      QBI_CHECK_NULL_PTR_RET(sys_info->rat.lte);
      network_id_valid =
        sys_info->rat.lte->threegpp_specific_sys_info.network_id_valid;
      network_id = &sys_info->rat.lte->threegpp_specific_sys_info.network_id;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_TDSCDMA:
      QBI_CHECK_NULL_PTR_RET(sys_info->rat.tdscdma);
      network_id_valid =
        sys_info->rat.tdscdma->threegpp_specific_sys_info.network_id_valid;
      network_id = &sys_info->rat.tdscdma->threegpp_specific_sys_info.network_id;
      break;

    default:
      QBI_LOG_E_1("Unexpected primary_rat %d", sys_info->primary_rat);
  }

  if (provider_id_ascii_size < QBI_SVC_BC_PROVIDER_ID_ASCII_MAX_LEN)
  {
    QBI_LOG_E_2("Can't populate ASCII ProviderId! Buffer size %d too small "
                "(expecting at least %d)",
                provider_id_ascii_size, QBI_SVC_BC_PROVIDER_ID_ASCII_MAX_LEN);
  }
  else if (network_id_valid && network_id != NULL)
  {
    /* Network ID from QMI NAS contains the MCC-MNC in ASCII characters, but
       in cases where a digit is undefined, it will populate 0xFF. Relevant
       examples include: undefined PLMN (MCC and MNC both filled with 0xFF), and
       2-digit MNC (last digit of MNC set to 0xFF). We convert these to an ASCII
       format that is ready for use in MBIM by replacing 0xFF with 0x00. */
    QBI_MEMSCPY(provider_id_ascii, provider_id_ascii_size,
                network_id->mcc, NAS_MCC_MNC_MAX_V01);
    QBI_MEMSCPY(&provider_id_ascii[NAS_MCC_MNC_MAX_V01],
                (provider_id_ascii_size - NAS_MCC_MNC_MAX_V01),
                network_id->mnc, NAS_MCC_MNC_MAX_V01);
    for (i = 0;
         i < MIN(provider_id_ascii_size, QBI_SVC_BC_PROVIDER_ID_ASCII_MAX_LEN);
         i++)
    {
      if ((uint8) provider_id_ascii[i] == (uint8) QBI_QMI_NAS_MCC_MNC_UNUSED_DIGIT)
      {
        provider_id_ascii[i] = QBI_UTIL_ASCII_NULL;
      }
    }
  }
} /* qbi_svc_bc_nas_register_state_sys_info_to_provider_id_3gpp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_sys_info_to_provider_id_3gpp2
===========================================================================*/
/*!
    @brief Converts the 3GPP2 network ID into MBIM format with ASCII encoding

    @details

    @param sys_info
    @param provider_id_ascii
    @param provider_id_ascii_size
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_sys_info_to_provider_id_3gpp2
(
  const qbi_svc_bc_nas_sys_info_s *sys_info,
  char                            *provider_id_ascii,
  uint32                           provider_id_ascii_size
)
{
  uint16 sid;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(sys_info);
  QBI_CHECK_NULL_PTR_RET(provider_id_ascii);

  if (sys_info->primary_rat == QBI_SVC_BC_NAS_PRIMARY_RAT_CDMA &&
      sys_info->rat.cdma != NULL &&
      sys_info->rat.cdma->cdma_specific_sys_info.cdma_sys_id_valid)
  {
    sid = sys_info->rat.cdma->cdma_specific_sys_info.cdma_sys_id.sid;
  }
  else if (sys_info->primary_rat == QBI_SVC_BC_NAS_PRIMARY_RAT_HDR &&
           sys_info->hybrid_cdma_sys_info != NULL &&
           sys_info->hybrid_cdma_sys_info->cdma_specific_sys_info.
             cdma_sys_id_valid)
  {
    sid = sys_info->hybrid_cdma_sys_info->cdma_specific_sys_info.
      cdma_sys_id.sid;
  }
  else
  {
    sid = QBI_SVC_BC_CDMA_DEFAULT_PROVIDER_ID;
  }

  QBI_SNPRINTF(provider_id_ascii, provider_id_ascii_size, "%05d", sid);
} /* qbi_svc_bc_nas_register_state_sys_info_to_provider_id_3gpp2() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_sys_info_to_register_state
===========================================================================*/
/*!
    @brief Determines the current MBIM_REGISTER_STATE value based off
    information from QMI

    @details

    @param ctx
    @param sys_info

    @return uint32 MBIM_REGISTER_STATE enum
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_register_state_sys_info_to_register_state
(
  const qbi_ctx_s                 *ctx,
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
  uint32 register_state;
  const qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(sys_info);

  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  switch (sys_info->srv_status)
  {
    case NAS_SYS_SRV_STATUS_NO_SRV_V01:
    case NAS_SYS_SRV_STATUS_LIMITED_V01:
    case NAS_SYS_SRV_STATUS_LIMITED_REGIONAL_V01:
    case NAS_SYS_SRV_STATUS_PWR_SAVE_V01:
      if (sys_info->common != NULL &&
          sys_info->common->is_sys_forbidden_valid &&
          sys_info->common->is_sys_forbidden)
      {
        QBI_LOG_I_0("System is forbidden");
        register_state = QBI_SVC_BC_REGISTER_STATE_FORBIDDEN;
      }
      else if (sys_info->sim_rej_info_valid &&
               (sys_info->sim_rej_info == NAS_SIM_PS_INVALID_V01 ||
                sys_info->sim_rej_info == NAS_SIM_CS_PS_INVALID_V01))
      {
        QBI_LOG_I_0("SIM is invalid for PS service");
        register_state = QBI_SVC_BC_REGISTER_STATE_DENIED;
      }
      else if (sys_info->sim_rej_info_valid &&
               sys_info->sim_rej_info == NAS_SIM_CS_INVALID_V01 &&
               sys_info->common != NULL &&
               sys_info->common->srv_capability_valid &&
               sys_info->common->srv_capability == SYS_SRV_DOMAIN_CS_ONLY_V01)
      {
        QBI_LOG_I_0("Network only supports CS and SIM is CS invalid");
        register_state = QBI_SVC_BC_REGISTER_STATE_DENIED;
      }
      else if ((qbi_svc_bc_nas_sys_info_is_reject_cause_valid(sys_info) &&
                !qbi_svc_bc_nas_reject_cause_allows_retry(
                  sys_info->common_3gpp->reg_reject_info.rej_cause)) ||
               (cache->packet_service.nw_error != 0 &&
                !qbi_svc_bc_nas_reject_cause_allows_retry(
                  cache->packet_service.nw_error)))
      {
        /* This is necessary as we may be rejected by the network but in limited
           service, without an invalidated SIM or forbidden system */
        QBI_LOG_I_0("System information or cache contain valid reject cause code");
        register_state = QBI_SVC_BC_REGISTER_STATE_DENIED;
      }
      else if (sys_info->srv_status == NAS_SYS_SRV_STATUS_PWR_SAVE_V01 ||
               !qbi_svc_bc_radio_state_is_radio_on(ctx) ||
               cache->register_state.register_mode ==
                 QBI_SVC_BC_REGISTER_MODE_MANUAL)
      {
        QBI_LOG_I_0("Power save/radio off/manual registration - deregistered");
        register_state = QBI_SVC_BC_REGISTER_STATE_DEREGISTERED;
      }
      else
      {
        QBI_LOG_I_1("Searching for service (srv_status %d)",
                    sys_info->srv_status);
        register_state = QBI_SVC_BC_REGISTER_STATE_SEARCHING;
      }
      break;

    case NAS_SYS_SRV_STATUS_SRV_V01:
      if (sys_info->common != NULL && sys_info->common->roam_status_valid)
      {
        register_state =
          qbi_svc_bc_nas_register_state_eri_roam_status_to_register_state(
            cache, sys_info->common->roam_status);
        QBI_LOG_D_2("In service with roam_status %d: register state is %d",
                    sys_info->common->roam_status, register_state);
      }
      else
      {
        QBI_LOG_W_0("No roaming status available - assuming home system");
        register_state = QBI_SVC_BC_REGISTER_STATE_HOME;
      }
      break;

    default:
      QBI_LOG_E_1("Unexpected srv_status %d", sys_info->srv_status);
      register_state = QBI_SVC_BC_REGISTER_STATE_UNKNOWN;
  }

  return register_state;
} /* qbi_svc_bc_nas_register_state_sys_info_to_register_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_data_class_to_qmi_mode_pref
===========================================================================*/
/*!
    @brief Converts a MBIM data class bitmask to a QMI mode preference
    bitmask to use with QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ

    @details

    @param data_class

    @return uint16
*/
/*=========================================================================*/
static uint16 qbi_svc_bc_nas_register_state_data_class_to_qmi_mode_pref
(
  uint32 data_class
)
{
  uint16 qmi_mode_pref = 0;
/*-------------------------------------------------------------------------*/
  if (data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_GSM)
  {
    qmi_mode_pref |= (1 << QMI_NAS_RAT_MODE_PREF_GSM_BIT_V01);
  }
  if (data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_WCDMA)
  {
    qmi_mode_pref |= (1 << QMI_NAS_RAT_MODE_PREF_UMTS_BIT_V01);
  }
  if (data_class & QBI_SVC_BC_DATA_CLASS_LTE)
  {
    qmi_mode_pref |= (1 << QMI_NAS_RAT_MODE_PREF_LTE_BIT_V01);
  }
  if (data_class & QBI_SVC_BC_DATA_CLASS_CUSTOM)
  {
    qmi_mode_pref |= (1 << QMI_NAS_RAT_MODE_PREF_TDSCDMA_BIT_V01);
  }

  if ((data_class & QBI_SVC_BC_DATA_CLASS_1XRTT) ||
      (data_class & QBI_SVC_BC_DATA_CLASS_3XRTT))
  {
    qmi_mode_pref |= (1 << QMI_NAS_RAT_MODE_PREF_CDMA2000_1X_BIT_V01);
  }
  if (data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_HDR)
  {
    qmi_mode_pref |= (1 << QMI_NAS_RAT_MODE_PREF_CDMA2000_HRPD_BIT_V01);
  }

  QBI_LOG_I_2("Mapped MBIM data class 0x%08x to QMI mode preference 0x%08x",
              data_class, qmi_mode_pref);
  return qmi_mode_pref;
} /* qbi_svc_bc_nas_register_state_data_class_to_qmi_mode_pref() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_determine_cache_only_mode
===========================================================================*/
/*!
    @brief Assess whether cache only mode should be enabled or not

    @details
    Cache only mode is used when the device loses service but has a data
    connection up.

    @param ctx
    @param register_state

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_determine_cache_only_mode
(
  qbi_ctx_s *ctx,
  uint32     register_state
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_bc_connect_is_connected(ctx) &&
          !qbi_svc_bc_connect_is_loopback(ctx) &&
          !qbi_svc_bc_nas_register_state_is_registered(register_state));
} /* qbi_svc_bc_nas_register_state_determine_cache_only_mode() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_completion_cb
===========================================================================*/
/*!
    @brief Register state event completion callback used to force a packet
    service event when we temporarily dropped those events to wait on
    querying for the network name

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_e_completion_cb
(
  const qbi_txn_s *txn
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache->packet_service.hold_events)
  {
    QBI_LOG_I_0("Packet service hold_events flag disabled");
    cache->packet_service.hold_events = FALSE;
    if (cache->packet_service.event_pending)
    {
      QBI_LOG_I_0("Packet service event is pending; forcing");
      cache->packet_service.event_pending = FALSE;
      qbi_svc_force_event(
        txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PACKET_SERVICE);
    }
  }
} /* qbi_svc_bc_nas_register_state_e_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas44_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_PLMN_NAME_RESP for MBIM_CID_REGISTER_STATE
    event. Decodes the queried PLMN name, updates the cache, and sends an
    event if the cache changed.

    @details
    The only difference between this function and
    qbi_svc_bc_nas_register_state_q_nas44_rsp_cb() is that this one will
    return QBI_SVC_ACTION_ABORT if the provider name did not change, while
    the other one will return QBI_SVC_ACTION_SEND_RSP if no other QMI
    requests are pending.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_plmn_name_resp_msg_v01 *qmi_rsp;
  uint32 provider_name_len;
  uint8 provider_name[QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES];
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_bc_provider_name_pref_e name_pref;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_bc_nas_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = (nas_get_plmn_name_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Couldn't get PLMN name for current network: error %d",
                qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->eons_plmn_name_3gpp_valid)
  {
    QBI_LOG_E_0("Missing PLMN name TLV in successful response!");
  }
  else
  {
    name_pref = (cache->register_state.concat_plmn_spn) ?
      QBI_SVC_BC_PROVIDER_NAME_PREF_REGISTERED_WITH_CONCAT :
      QBI_SVC_BC_PROVIDER_NAME_PREF_REGISTERED;
    provider_name_len = qbi_svc_bc_qmi_plmn_name_to_provider_name(
      qmi_rsp, provider_name, sizeof(provider_name), name_pref);
    if (provider_name_len == 0)
    {
      QBI_LOG_E_0("Couldn't convert QMI PLMN name to ProviderName");
    }
    else if (qbi_svc_bc_nas_register_state_update_cache_provider_name_3gpp(
               qmi_txn->ctx, provider_name, provider_name_len))
    {
      action = qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache(
        qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_nas_register_state_e_nas44_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eq_build_nas44_req
===========================================================================*/
/*!
    @brief Builds a QMI_NAS_GET_PLMN_NAME_REQ to fetch the name of the 3GPP
    network we are currently registered to (according to the register state
    cache)

    @details

    @param txn
    @param rsp_cb

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_eq_build_nas44_req
(
  qbi_txn_s            *txn,
  qbi_svc_qmi_rsp_cb_f *rsp_cb
)
{
  qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  nas_get_plmn_name_req_msg_v01 *qmi_req;
  boolean mnc_is_3_digits;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_req = (nas_get_plmn_name_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_PLMN_NAME_REQ_MSG_V01, rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  /* Ignore SIM not initialized errors when trying to get the network name for
     REGISTER_STATE - we will accept a value from SE13/NITZ if the SIM is not
     available */
  qmi_req->suppress_sim_error_valid = TRUE;
  qmi_req->suppress_sim_error = TRUE;

  if (cache->register_state.qmi.rat != NAS_RADIO_IF_NO_SVC_V01)
  {
    qmi_req->rat_valid = TRUE;
    qmi_req->rat = cache->register_state.qmi.rat;
  }

  if (cache->register_state.qmi.csg_id_valid)
  {
    qmi_req->csg_id_valid = TRUE;
    qmi_req->csg_id = cache->register_state.qmi.csg_id;
  }

  qmi_req->send_all_information_valid = TRUE;
  qmi_req->send_all_information = TRUE;

  if (!qbi_svc_bc_provider_id_ascii_to_mcc_mnc(
        cache->register_state.provider_id_ascii,
        sizeof(cache->register_state.provider_id_ascii),
        (uint16 *) &qmi_req->plmn.mcc, (uint16 *) &qmi_req->plmn.mnc,
        &mnc_is_3_digits))
  {
    QBI_LOG_E_0("Couldn't convert cached ProviderId to binary!");
    QBI_LOG_STR_1("Cached ProviderId: '%s'",
                  cache->register_state.provider_id_ascii);
  }
  else
  {
    qmi_req->mnc_includes_pcs_digit_valid = TRUE;
    qmi_req->mnc_includes_pcs_digit = mnc_is_3_digits;
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_nas_register_state_eq_build_nas44_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Allocates a response/event InformationBuffer for
    MBIM_CID_REGISTER_STATE, and populates it with the values from the cache

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_register_state_rsp_s *rsp;
  const qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_bc_register_state_rsp_s *)
    qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_register_state_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->register_state         = cache->register_state.register_state;
  rsp->register_mode          = cache->register_state.register_mode;
  rsp->available_data_class   = cache->register_state.available_data_class;
  rsp->current_cellular_class =
    qbi_svc_bc_nas_register_state_get_current_cellular_class(txn->ctx);

  /* Don't return searching as the current register state to a set request */
  if (txn->cmd_type == QBI_MSG_CMD_TYPE_SET &&
      rsp->register_state == QBI_SVC_BC_REGISTER_STATE_SEARCHING)
  {
    rsp->register_state = QBI_SVC_BC_REGISTER_STATE_DEREGISTERED;
  }
  if (!qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache_add_provider_info(
        txn, cache))
  {
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    /* Set status when register state is forbidden only to a set request */
    if ((txn->cmd_type == QBI_MSG_CMD_TYPE_SET) &&
        (rsp->register_state == QBI_SVC_BC_REGISTER_STATE_FORBIDDEN) &&
        (rsp->register_mode == QBI_SVC_BC_REGISTER_MODE_MANUAL))
    {
      QBI_LOG_I_0("Setting status to Invalid parameters - in rsp_cb");
      txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
      action = QBI_SVC_ACTION_ABORT;
    }

    if ((rsp->register_state == QBI_SVC_BC_REGISTER_STATE_DENIED) ||
        (rsp->register_state == QBI_SVC_BC_REGISTER_STATE_FORBIDDEN) ||
        (rsp->register_state == QBI_SVC_BC_REGISTER_STATE_DEREGISTERED))
    {
      rsp->nw_error = cache->register_state.nw_error;
      if (rsp->nw_error == 0 && cache->register_state.perm_nw_error != 0)
      {
        rsp->nw_error = cache->register_state.perm_nw_error;
      }

      if(rsp->register_state == QBI_SVC_BC_REGISTER_STATE_FORBIDDEN)
      {
        rsp->register_state = QBI_SVC_BC_REGISTER_STATE_DENIED;
        QBI_LOG_I_1("RegState is FORBIDDEN, Returning DENIED to client with nw_error %d", rsp->nw_error);
      }
      else
      {
        QBI_LOG_I_1("RegState is DENIED/DEREGISTERED with nw_error %d",
                  rsp->nw_error);
      }
    }

    /*! @note The auto attach flag is currently always set, to indicate that the
        device will manage its own PS attach state. No scenario expected or
        encountered so far where we would want to disable this flag. */
    rsp->registration_flag = QBI_SVC_BC_REGISTRATION_FLAG_AUTO_ATTACH;
    if (cache->register_state.qmi.plmn_mode == NAS_PLMN_MODE_RESTRICT_V01)
    {
      QBI_LOG_I_0("Manual network registration is not allowed");
      rsp->registration_flag |= QBI_SVC_BC_REGISTRATION_FLAG_MANUAL_REG_NOT_AVAIL;
    }

    qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache_add_roaming_text(
      txn, cache);
  }

  return action;
} /* qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache_add_provider_info
===========================================================================*/
/*!
    @brief Populates the ProviderId and ProviderName fields in the
    MBIM_CID_REGISTER_STATE response

    @details
    CurrentCellularClass and RegisterState must already be populated in the
    response structure.

    @param txn
    @param cache

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache_add_provider_info
(
  qbi_txn_s                    *txn,
  const qbi_svc_bc_nas_cache_s *cache
)
{
  boolean success = TRUE;
  const uint8 *cdma_network_name;
  uint32 cdma_network_name_len = 0;
  qbi_svc_bc_register_state_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  rsp = (qbi_svc_bc_register_state_rsp_s *) txn->rsp.data;
  if (qbi_svc_bc_nas_register_state_cached_provider_id_is_valid(txn->ctx) &&
      qbi_svc_bc_nas_register_state_is_provider_id_and_name_reported(
        rsp->register_state))
  {
    if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          txn, &rsp->provider_id, 0, QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES,
          cache->register_state.provider_id_ascii,
          sizeof(cache->register_state.provider_id_ascii)))
    {
      QBI_LOG_E_0("Couldn't add ProviderId to response!");
      success = FALSE;
    }
    else if (rsp->current_cellular_class == QBI_SVC_BC_CELLULAR_CLASS_GSM)
    {
      if (cache->register_state.provider_name_3gpp_len > 0 &&
          !qbi_txn_rsp_databuf_add_field(
            txn, &rsp->provider_name, 0,
            cache->register_state.provider_name_3gpp_len,
            cache->register_state.provider_name_3gpp))
      {
        QBI_LOG_E_0("Couldn't add ProviderName to response!");
        success = FALSE;
      }
    }
    else if (rsp->current_cellular_class == QBI_SVC_BC_CELLULAR_CLASS_CDMA)
    {
      cdma_network_name = qbi_svc_bc_sim_home_provider_get_3gpp2_network_name(
        txn->ctx, &cdma_network_name_len);
      if (cdma_network_name == NULL || cdma_network_name_len == 0)
      {
        QBI_LOG_W_0("No ProviderName available for this CDMA network!");
      }
      else if (!qbi_txn_rsp_databuf_add_field(
                 txn, &rsp->provider_name, 0, cdma_network_name_len,
                 cdma_network_name))
      {
        QBI_LOG_E_0("Couldn't add ProviderName to response!");
        success = FALSE;
      }
    }
  }

  /* If we are in manual network registration mode, populate the ProviderId with
     what the host requested if we didn't already populate it with information
     from QMI - for example if we don't have service. */
  if (cache->register_state.register_mode == QBI_SVC_BC_REGISTER_MODE_MANUAL &&
      rsp->provider_id.offset == 0)
  {
    if (!qbi_svc_bc_provider_append_3gpp_provider_id(
          txn, &rsp->provider_id, 0, cache->register_state.qmi.manual_reg_mcc,
          cache->register_state.qmi.manual_reg_mnc,
          cache->register_state.qmi.manual_reg_mnc_is_3_digits))
    {
      QBI_LOG_E_0("Couldn't add manual registration ProviderId to response!");
      success = FALSE;
    }
  }

  return success;
} /* qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache_add_provider_info() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache_add_roaming_text
===========================================================================*/
/*!
    @brief Populates the RoamingText field in the MBIM_CID_REGISTER_STATE
    response with a string from the ERI, if applicable

    @details
    CurrentCellularClass must already be populated in the response structure.

    @param txn
    @param cache
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache_add_roaming_text
(
  qbi_txn_s                    *txn,
  const qbi_svc_bc_nas_cache_s *cache
)
{
  uint8 tbl_index;
  qbi_svc_bc_register_state_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET(cache);

  rsp = (qbi_svc_bc_register_state_rsp_s *) txn->rsp.data;
  if (rsp->current_cellular_class == QBI_SVC_BC_CELLULAR_CLASS_CDMA &&
      cache->register_state.eri.tbl_index_valid == TRUE)
  {
    tbl_index = cache->register_state.eri.tbl_index;
    if (tbl_index >= cache->register_state.eri.num_tbl_entries ||
        cache->register_state.eri.tbl == NULL ||
        cache->register_state.eri.algo == QBI_SVC_BC_NAS_ERI_ALGO_NONE)
    {
      QBI_LOG_E_0("ERI cache failed sanity!");
    }
    else if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
               txn, &rsp->roaming_text, 0, QBI_SVC_BC_ROAMING_TEXT_MAX_LEN_BYTES,
               cache->register_state.eri.tbl[tbl_index].roam_text_ascii,
               sizeof(cache->register_state.eri.tbl[tbl_index].roam_text_ascii)))
    {
      QBI_LOG_E_0("Couldn't add RoamingText to response!");
    }
  }
} /* qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache_add_roaming_text() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query
===========================================================================*/
/*!
    @brief Performs a query of REGISTER_STATE information if the cache isn't
    fully populated yet; otherwise uses cached information to build the
    response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (!cache->register_state.info_valid)
  {
    action = qbi_svc_bc_nas_register_state_q_req(txn);
  }
  else if (cache->register_state.current_cellular_class ==
             QBI_SVC_BC_CELLULAR_CLASS_GSM &&
           qbi_svc_bc_nas_register_state_is_provider_id_and_name_reported(
             cache->register_state.register_state) &&
           qbi_svc_bc_nas_register_state_cached_provider_id_is_valid(
             txn->ctx) &&
           cache->register_state.provider_name_3gpp_len == 0)
  {
    action = qbi_svc_bc_nas_register_state_eq_build_nas44_req(
      txn, qbi_svc_bc_nas_register_state_q_nas44_rsp_cb);
  }
  else
  {
    action = qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache(txn);
  }

  return action;
} /* qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_hybrid_primary_stack_changed
===========================================================================*/
/*!
    @brief Checks whether the hybrid primary stack has changed, i.e. we have
    moved between 1xEVDO and 1x only

    @details

    @param old_available_data_class
    @param new_available_data_class

    @return boolean TRUE if hybrid primary stack changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_hybrid_primary_stack_changed
(
  uint32 old_available_data_class,
  uint32 new_available_data_class
)
{
/*-------------------------------------------------------------------------*/
  return (((old_available_data_class & QBI_SVC_BC_DATA_CLASS_1XRTT) != 0 &&
           (old_available_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_HDR) != 0 &&
           (new_available_data_class & QBI_SVC_BC_DATA_CLASS_1XRTT) != 0 &&
           (new_available_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_HDR) == 0)
          ||
          ((old_available_data_class & QBI_SVC_BC_DATA_CLASS_1XRTT) != 0 &&
           (old_available_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_HDR) == 0 &&
           (new_available_data_class & QBI_SVC_BC_DATA_CLASS_1XRTT) != 0 &&
           (new_available_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_HDR) != 0));
} /* qbi_svc_bc_nas_register_state_hybrid_primary_stack_changed() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_is_in_requested_mode
===========================================================================*/
/*!
    @brief Checks whether we are currently in the RegisterMode requested by
    the given RegisterAction

    @details

    @param ctx
    @param register_action
    @param data_class

    @return boolean TRUE if in the requested mode, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_is_in_requested_mode
(
  qbi_ctx_s *ctx,
  uint32     register_action,
  uint32     data_class
)
{
  qbi_svc_bc_nas_cache_s *cache;
  mode_pref_mask_type_v01 qmi_mode_pref;
  boolean in_requested_mode = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (cache->register_state.info_valid &&
      ((register_action == QBI_SVC_BC_REGISTER_ACTION_AUTOMATIC &&
        cache->register_state.register_mode ==
          QBI_SVC_BC_REGISTER_MODE_AUTOMATIC) ||
       (register_action == QBI_SVC_BC_REGISTER_ACTION_MANUAL &&
        cache->register_state.register_mode == QBI_SVC_BC_REGISTER_MODE_MANUAL)))
  {
    qmi_mode_pref = qbi_svc_bc_nas_register_state_data_class_to_qmi_mode_pref(
      data_class);
    if (cache->register_state.qmi.mode_pref == qmi_mode_pref)
    {
      in_requested_mode = TRUE;
    }
  }

  return in_requested_mode;
} /* qbi_svc_bc_nas_register_state_is_in_requested_mode() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_is_provider_id_and_name_reported
===========================================================================*/
/*!
    @brief Checks whether ProviderId and ProviderName will be reported to the
    host

    @details

    @param register_state

    @return boolean TRUE if device is registered or denied, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_is_provider_id_and_name_reported
(
  uint32 register_state
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_bc_nas_register_state_is_registered(register_state) ||
          (register_state == QBI_SVC_BC_REGISTER_STATE_DENIED) ||
          (register_state == QBI_SVC_BC_REGISTER_STATE_FORBIDDEN));
} /* qbi_svc_bc_nas_register_state_is_provider_id_and_name_reported() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_is_registered
===========================================================================*/
/*!
    @brief Tells whether the current register state indicates that we are
    registered (i.e. home, roaming, or partner)

    @details

    @param register_state

    @return boolean TRUE if registered, FALSE if deregistered
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_register_state_is_registered
(
  uint32 register_state
)
{
/*-------------------------------------------------------------------------*/
  return (register_state == QBI_SVC_BC_REGISTER_STATE_HOME    ||
          register_state == QBI_SVC_BC_REGISTER_STATE_ROAMING ||
          register_state == QBI_SVC_BC_REGISTER_STATE_PARTNER);
} /* qbi_svc_bc_nas_register_state_is_registered() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_primary_rat_to_cellular_class
===========================================================================*/
/*!
    @brief Maps a primary RAT to MBIM_CELLULAR_CLASS

    @details

    @param ctx
    @param primary_rat

    @return uint32 MBIM_CELLULAR_CLASS enum
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_register_state_primary_rat_to_cellular_class
(
  const qbi_ctx_s             *ctx,
  qbi_svc_bc_nas_primary_rat_e primary_rat
)
{
  uint32 cellular_class;
/*-------------------------------------------------------------------------*/
  switch (primary_rat)
  {
    case QBI_SVC_BC_NAS_PRIMARY_RAT_CDMA:
    case QBI_SVC_BC_NAS_PRIMARY_RAT_HDR:
      cellular_class = QBI_SVC_BC_CELLULAR_CLASS_CDMA;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_GSM:
    case QBI_SVC_BC_NAS_PRIMARY_RAT_WCDMA:
    case QBI_SVC_BC_NAS_PRIMARY_RAT_LTE:
    case QBI_SVC_BC_NAS_PRIMARY_RAT_TDSCDMA:
      cellular_class = QBI_SVC_BC_CELLULAR_CLASS_GSM;
      break;

    default:
      cellular_class = QBI_SVC_BC_CELLULAR_CLASS_GSM;
      QBI_LOG_I_2("No primary RAT (or unexpected value %d); defaulting "
                  "cellular class to %d", primary_rat, cellular_class);
  }

  return cellular_class;
} /* qbi_svc_bc_nas_register_state_primary_rat_to_cellular_class() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_primary_rat_to_qmi_radio_if_3gpp
===========================================================================*/
/*!
    @brief Maps a QBI primary RAT enum into a QMI NAS radio interface enum
    (limited to 3GPP only - GSM, UMTS, LTE, TD-SCDMA)

    @details

    @param primary_rat

    @return nas_radio_if_enum_v01 Valid 3GPP RAT, or NAS_RADIO_IF_NO_SVC_V01
    if no suitable mapping found
*/
/*=========================================================================*/
static nas_radio_if_enum_v01 qbi_svc_bc_nas_register_state_primary_rat_to_qmi_radio_if_3gpp
(
  qbi_svc_bc_nas_primary_rat_e primary_rat
)
{
  nas_radio_if_enum_v01 qmi_radio_if;
/*-------------------------------------------------------------------------*/
  switch (primary_rat)
  {
    case QBI_SVC_BC_NAS_PRIMARY_RAT_GSM:
      qmi_radio_if = NAS_RADIO_IF_GSM_V01;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_WCDMA:
      qmi_radio_if = NAS_RADIO_IF_UMTS_V01;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_LTE:
      qmi_radio_if = NAS_RADIO_IF_LTE_V01;
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_TDSCDMA:
      qmi_radio_if = NAS_RADIO_IF_TDSCDMA_V01;
      break;

    default:
      qmi_radio_if = NAS_RADIO_IF_NO_SVC_V01;
  }

  return qmi_radio_if;
} /* qbi_svc_bc_nas_register_state_primary_rat_to_qmi_radio_if_3gpp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_q_nas34_rsp_cb
===========================================================================*/
/*!
    @brief Processes a QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_RESP for
    MBIM_CID_REGISTER_STATE query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_q_nas34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_system_selection_preference_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->net_sel_pref_valid || !qmi_rsp->mode_pref_valid)
  {
    QBI_LOG_E_0("Missing one or more expected TLVs in response!");
  }
  else
  {
    (void) qbi_svc_bc_nas_register_state_update_cache_net_sel_pref(
      qmi_txn->ctx, qmi_rsp->net_sel_pref, qmi_rsp->mode_pref);
    if (qmi_rsp->manual_net_sel_plmn_valid)
    {
      (void) qbi_svc_bc_nas_register_state_update_cache_manual_reg_mcc_mnc(
        qmi_txn->ctx, qmi_rsp->manual_net_sel_plmn.mcc,
        qmi_rsp->manual_net_sel_plmn.mnc,
        qmi_rsp->manual_net_sel_plmn.mnc_includes_pcs_digit);
    }
    action = qbi_svc_bc_nas_register_state_q_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_nas_register_state_q_nas34_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_q_nas3b_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_CSP_PLMN_MODE_BIT_RESP for
    MBIM_CID_REGISTER_STATE query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_q_nas3b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_csp_plmn_mode_bit_resp_msg_v01 *qmi_rsp;
  nas_plmn_mode_enum_v01 plmn_mode = NAS_PLMN_MODE_DO_NOT_RESTRICT_V01;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_csp_plmn_mode_bit_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if (qmi_rsp->resp.error == QMI_ERR_SIM_FILE_NOT_FOUND_V01)
    {
      QBI_LOG_I_0("CSP EF not found in SIM");
    }
    else
    {
      QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    }
  }
  else if (!qmi_rsp->plmn_mode_valid)
  {
    QBI_LOG_W_0("PLMN mode TLV not in response");
  }
  else
  {
    plmn_mode = qmi_rsp->plmn_mode;
  }
  (void) qbi_svc_bc_nas_register_state_update_cache_plmn_mode(
    qmi_txn->ctx, plmn_mode);

  return qbi_svc_bc_nas_register_state_q_rsp(qmi_txn->parent);
} /* qbi_svc_bc_nas_register_state_q_nas3b_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_q_nas44_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_PLMN_NAME_RESP for MBIM_CID_REGISTER_STATE
    query. Saves the ProviderName of the current 3GPP network to cache.

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_q_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uint32 provider_name_len;
  uint8 provider_name[QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES];
  nas_get_plmn_name_resp_msg_v01 *qmi_rsp;
  const qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_provider_name_pref_e name_pref;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_bc_nas_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = (nas_get_plmn_name_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Couldn't get PLMN name for current network: error %d",
                qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->eons_plmn_name_3gpp_valid)
  {
    QBI_LOG_E_0("Missing PLMN name TLV in successful response!");
  }
  else
  {
    name_pref = (cache->register_state.concat_plmn_spn) ?
      QBI_SVC_BC_PROVIDER_NAME_PREF_REGISTERED_WITH_CONCAT :
      QBI_SVC_BC_PROVIDER_NAME_PREF_REGISTERED;
    provider_name_len = qbi_svc_bc_qmi_plmn_name_to_provider_name(
      qmi_rsp, provider_name, sizeof(provider_name), name_pref);
    if (provider_name_len == 0)
    {
      QBI_LOG_E_0("Couldn't convert QMI PLMN name to ProviderName");
    }
    else
    {
      (void) qbi_svc_bc_nas_register_state_update_cache_provider_name_3gpp(
        qmi_txn->ctx, provider_name, provider_name_len);
    }

    action = qbi_svc_bc_nas_register_state_q_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_nas_register_state_q_nas44_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_q_nas4d_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_SYS_INFO_RESP for MBIM_CID_REGISTER_STATE
    query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_q_nas4d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_sys_info_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_nas_sys_info_s sys_info;
  const qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_bc_nas_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = (nas_get_sys_info_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    if (qmi_rsp->resp.error == QMI_ERR_INFO_UNAVAILABLE_V01)
    {
      QBI_LOG_W_0("Serving system information not available!");
      /* Use cached value for register state (should still be UNKNOWN from cache
         init). We should receive a QMI indication shortly informing us of the
         true status. */
      action = qbi_svc_bc_nas_register_state_q_rsp(qmi_txn->parent);
    }
  }
  else
  {
    /* Ensure that our cache has the newest values (note that this is actually
       expected to not modify the cache - we *should* receive a QMI indication
       any time a value changes, but in case we are out of sync, this will let
       us recover). The response is populated out of the cache when all QMI
       responses have been received and stored in the cache. */
    qbi_svc_bc_nas_sys_info_extract_rsp(&sys_info, qmi_rsp);
    (void) qbi_svc_bc_nas_register_state_update_cache_sys_info(
      qmi_txn->ctx, &sys_info);

    /* If we are registered on a 3GPP network, then we need an additional QMI
       query to determine the network name of the current PLMN. */
    if (qbi_svc_bc_nas_register_state_get_current_cellular_class(
          qmi_txn->ctx) == QBI_SVC_BC_CELLULAR_CLASS_GSM &&
        qbi_svc_bc_nas_register_state_cached_provider_id_is_valid(
          qmi_txn->ctx) &&
        qbi_svc_bc_nas_register_state_is_provider_id_and_name_reported(
          cache->register_state.register_state))
    {
      action = qbi_svc_bc_nas_register_state_eq_build_nas44_req(
        qmi_txn->parent, qbi_svc_bc_nas_register_state_q_nas44_rsp_cb);
    }
    else
    {
      action = qbi_svc_bc_nas_register_state_q_rsp(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_nas_register_state_q_nas4d_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_q_rsp
===========================================================================*/
/*!
    @brief Perform query response processing for MBIM_CID_REGISTER_STATE

    @details
    Checks whether we have the information required to send the response.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_q_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (txn->qmi_txns_pending == 0)
  {
    cache = qbi_svc_bc_nas_cache_get(txn->ctx);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);

    /* We have now performed a full REGISTER_STATE query, so our cached
       information is up to date with the modem */
    cache->register_state.info_valid = TRUE;
    action = qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache(txn);
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_nas_register_state_q_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_qmi_hs_supp_to_mbim_data_class
===========================================================================*/
/*!
    @brief Maps a QMI HSPA support enum to MBIM HSDPA/HSUPA data class

    @details
    Only sets the HSDPA and/or HSUPA bits if they are supported; support for
    the UMTS data class should already be assumed.

    @param hs_supp

    @return uint32 MBIM_DATA_CLASS bitmask
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_register_state_qmi_hs_supp_to_mbim_data_class
(
  nas_hs_support_enum_type_v01 hs_supp
)
{
  uint32 data_class;
/*-------------------------------------------------------------------------*/
  switch (hs_supp)
  {
    case SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL_V01:
      data_class = QBI_SVC_BC_DATA_CLASS_NONE;
      break;

    case SYS_HS_IND_HSDPA_SUPP_CELL_V01:
    case SYS_HS_IND_HSDPAPLUS_SUPP_CELL_V01:
    case SYS_HS_IND_DC_HSDPAPLUS_SUPP_CELL_V01:
    case SYS_HS_IND_HSDPAPLUS_64QAM_SUPP_CELL_V01:
      data_class = QBI_SVC_BC_DATA_CLASS_HSDPA;
      break;

    case SYS_HS_IND_HSUPA_SUPP_CELL_V01:
      data_class = QBI_SVC_BC_DATA_CLASS_HSUPA;
      break;

    case SYS_HS_IND_HSDPA_HSUPA_SUPP_CELL_V01:
    case SYS_HS_IND_HSDPAPLUS_HSUPA_SUPP_CELL_V01:
    case SYS_HS_IND_DC_HSDPAPLUS_HSUPA_SUPP_CELL_V01:
    case SYS_HS_IND_HSDPAPLUS_64QAM_HSUPA_SUPP_CELL_V01:
      data_class = (QBI_SVC_BC_DATA_CLASS_HSDPA |
                    QBI_SVC_BC_DATA_CLASS_HSUPA);
      break;

    default:
      QBI_LOG_E_1("Unexpected hs_supp %d", hs_supp);
      data_class = QBI_SVC_BC_DATA_CLASS_NONE;
  }

  return data_class;
} /* qbi_svc_bc_nas_register_state_qmi_hs_supp_to_mbim_data_class() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_qmi_net_sel_pref_to_mbim_reg_mode
===========================================================================*/
/*!
    @brief Converts a QMI NAS network selection preference enum into a
    MBIM RegisterMode value

    @details

    @param net_sel_pref

    @return uint32 MBIM RegisterMode value
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_register_state_qmi_net_sel_pref_to_mbim_reg_mode
(
  nas_net_sel_pref_enum_v01 net_sel_pref
)
{
  uint32 register_mode;
/*-------------------------------------------------------------------------*/
  if (net_sel_pref == NAS_NET_SEL_PREF_AUTOMATIC_V01)
  {
    register_mode = QBI_SVC_BC_REGISTER_MODE_AUTOMATIC;
  }
  else if (net_sel_pref == NAS_NET_SEL_PREF_MANUAL_V01)
  {
    register_mode = QBI_SVC_BC_REGISTER_MODE_MANUAL;
  }
  else
  {
    QBI_LOG_E_1("Unexpected net_sel_pref %d", net_sel_pref);
    register_mode = QBI_SVC_BC_REGISTER_MODE_UNKNOWN;
  }

  return register_mode;
} /* qbi_svc_bc_nas_register_state_qmi_net_sel_pref_to_mbim_reg_mode() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_build_nas33_ps_attach_req
===========================================================================*/
/*!
    @brief Allocates and populates a
    QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ with service domain
    preference PS_ATTACH

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_build_nas33_ps_attach_req
(
  qbi_txn_s *txn
)
{
  nas_set_system_selection_preference_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_QMI_REQ;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (nas_set_system_selection_preference_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
      qbi_svc_bc_nas_register_state_s_nas33_ps_attach_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->change_duration_valid = TRUE;
  qmi_req->change_duration = NAS_POWER_CYCLE_V01;

  qmi_req->srv_domain_pref_valid = TRUE;
  qmi_req->srv_domain_pref = QMI_SRV_DOMAIN_PREF_PS_ATTACH_V01;

  return action;
} /* qbi_svc_bc_nas_register_state_s_build_nas33_ps_attach_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_build_nas33_req
===========================================================================*/
/*!
    @brief Allocates and populates a
    QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_build_nas33_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_register_state_s_req_s *req;
  nas_set_system_selection_preference_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_QMI_REQ;
  boolean mnc_is_3_digits = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_register_state_s_req_s *) txn->req.data;
  qmi_req = (nas_set_system_selection_preference_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
      qbi_svc_bc_nas_register_state_s_nas33_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  /*! @note MBIM recommends that the device attempt to register first on
      the requested RAT, then any other available RATs (best first). This
      is not supported by the modem (it only tries to register on the
      requested RAT, and if that fails, it gives up). So this functionality
      would have to be either added to the modem or emulated in QBI.
      The latter approach is much less desirable when it comes to network
      reselection and trying to hand up to a better tech, etc. since we
      can only wait on a timeout before trying a new RAT, whereas the modem
      can actively scan and handoff accordingly. Not following this
      recommendation - the device will only register to the best available
      data class within the requested bitmask. */
  qmi_req->mode_pref_valid = TRUE;
  qmi_req->mode_pref = qbi_svc_bc_nas_register_state_data_class_to_qmi_mode_pref(
      req->data_class);

  qmi_req->net_sel_pref_valid = TRUE;
  if (req->register_action == QBI_SVC_BC_REGISTER_ACTION_AUTOMATIC)
  {
    qmi_req->net_sel_pref.net_sel_pref = NAS_NET_SEL_PREF_AUTOMATIC_V01;
  }
  else if (req->register_action == QBI_SVC_BC_REGISTER_ACTION_MANUAL)
  {
    qmi_req->net_sel_pref.net_sel_pref = NAS_NET_SEL_PREF_MANUAL_V01;
    if (!qbi_svc_bc_provider_id_to_mcc_mnc(
          txn, &req->provider_id, 0, &qmi_req->net_sel_pref.mcc,
          &qmi_req->net_sel_pref.mnc, &mnc_is_3_digits))
    {
      QBI_LOG_E_0("Couldn't decode manual registration MCC-MNC");
      action = QBI_SVC_ACTION_ABORT;
    }
    qmi_req->mnc_includes_pcs_digit_valid = TRUE;
    qmi_req->mnc_includes_pcs_digit = mnc_is_3_digits;
  }
  else
  {
    QBI_LOG_E_1("Unexpected RegisterAction %d", req->register_action);
    action = QBI_SVC_ACTION_ABORT;
  }

  return action;
} /* qbi_svc_bc_nas_register_state_s_build_nas33_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_register_state_s_completion_cb
===========================================================================*/
/*!
    @brief MBIM_CID_REGISTER_STATE set request completion callback used to
    decrement the reference count on QMI_NAS_SYS_INFO_IND

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_register_state_s_completion_cb
(
  const qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  qbi_svc_ind_qmi_reg_invoke(txn->ctx, txn->svc_id, txn->cid, FALSE, NULL);
} /* qbi_svc_bc_register_state_s_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_is_request_complete
===========================================================================*/
/*!
    @brief Checks whether the current REGISTER_STATE represents the completed
    state of the transaction's set request

    @details
    The set request is complete once we have registered to the requested
    network, or if we have been denied registration on it.

    @param txn

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_s_is_request_complete
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_nas_cache_s *cache;
  const qbi_svc_bc_register_state_s_req_s *req;
  const uint8 *provider_id_utf16;
  char provider_id_ascii[QBI_SVC_BC_PROVIDER_ID_ASCII_MAX_LEN];
  boolean req_complete = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  req = (const qbi_svc_bc_register_state_s_req_s *) txn->req.data;
  if (req->register_action == QBI_SVC_BC_REGISTER_ACTION_MANUAL &&
      cache->register_state.current_cellular_class ==
        QBI_SVC_BC_CELLULAR_CLASS_GSM)
  {
    provider_id_utf16 = qbi_txn_req_databuf_get_field(
      txn, &req->provider_id, 0, QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES);
    if (provider_id_utf16 == NULL)
    {
      QBI_LOG_E_0("Couldn't extract ProviderId from request!");
    }
    else if (!qbi_util_utf16_to_ascii(
               provider_id_utf16, req->provider_id.size,
               provider_id_ascii, sizeof(provider_id_ascii)))
    {
      QBI_LOG_E_0("Couldn't convert UTF-16 ProviderId to ASCII");
    }
    else if (QBI_STRNCMP(
               provider_id_ascii, cache->register_state.provider_id_ascii,
               sizeof(provider_id_ascii)) == 0)
    {
      /* We have selected the requested network; see if we are registered or
         denied */
      if (qbi_svc_bc_nas_register_state_is_registered(
            cache->register_state.register_state) &&
          qbi_svc_bc_nas_register_state_data_class_rat_matches(
            cache->register_state.available_data_class, req->data_class))
      {
        QBI_LOG_I_0("Manual registration to requested network successful");
        req_complete = TRUE;
      }
      else if ((cache->register_state.register_state ==
                  QBI_SVC_BC_REGISTER_STATE_DENIED ||
                cache->register_state.register_state ==
                  QBI_SVC_BC_REGISTER_STATE_FORBIDDEN ||
                cache->register_state.register_state ==
                  QBI_SVC_BC_REGISTER_STATE_DEREGISTERED) &&
               cache->packet_service.nw_error != 0 &&
               !qbi_svc_bc_nas_reject_cause_allows_retry(
                 cache->packet_service.nw_error))
      {
        QBI_LOG_W_0("Denied manual registration on requested network");
        txn->status = QBI_MBIM_STATUS_FAILURE;
        req_complete = TRUE;
      }
    }
    else
    {
      QBI_LOG_STR_2("Current ProviderId '%s' does not match requested "
                    "ProviderId '%s'",
                    cache->register_state.provider_id_ascii, provider_id_ascii);
    }
  }
  else if (req->register_action == QBI_SVC_BC_REGISTER_ACTION_AUTOMATIC &&
           qbi_svc_bc_nas_register_state_is_registered(
             cache->register_state.register_state) &&
           qbi_svc_bc_nas_register_state_data_class_rat_matches(
             cache->register_state.available_data_class, req->data_class))
  {
    req_complete = TRUE;
  }

  return req_complete;
} /* qbi_svc_bc_nas_register_state_s_is_request_complete() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_nas33_ps_attach_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP for
    PS attach during MBIM_CID_REGISTER_STATE set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_nas33_ps_attach_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_set_system_selection_preference_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_set_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    action = qbi_svc_bc_nas_register_state_s_build_nas33_req(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_nas_register_state_s_nas33_ps_attach_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_nas33_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP for
    MBIM_CID_REGISTER_STATE set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_set_system_selection_preference_req_msg_v01 *qmi_req;
  nas_set_system_selection_preference_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_nas_net_sel_pref_cfg_s nv_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (nas_set_system_selection_preference_req_msg_v01 *)
    qmi_txn->req.data;
  qmi_rsp = (nas_set_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_D_0("Successfully set registration mode");
    (void) qbi_svc_bc_nas_register_state_update_cache_net_sel_pref(
      qmi_txn->ctx, qmi_req->net_sel_pref.net_sel_pref, qmi_req->mode_pref);
    if (qmi_req->net_sel_pref.net_sel_pref == NAS_NET_SEL_PREF_MANUAL_V01)
    {
      (void) qbi_svc_bc_nas_register_state_update_cache_manual_reg_mcc_mnc(
        qmi_txn->ctx, qmi_req->net_sel_pref.mcc, qmi_req->net_sel_pref.mnc,
        qmi_req->mnc_includes_pcs_digit);
    }

    QBI_MEMSET(&nv_info, 0, sizeof(nv_info));
    nv_info.net_sel_pref = qmi_req->net_sel_pref;
    nv_info.mnc_includes_pcs_digit = qmi_req->mnc_includes_pcs_digit;
    if (!qbi_nv_store_cfg_item_write(
          qmi_txn->ctx, QBI_NV_STORE_CFG_ITEM_NET_SEL_PREF,
          &nv_info, sizeof(nv_info)))
    {
      QBI_LOG_E_0("Couldn't save network selection preference to NV!");
    }
    else
    {
      action = qbi_svc_bc_nas_register_state_s_wait_for_completion(
        qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_nas_register_state_s_nas33_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_nas4e_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYS_INFO_IND, looking for registration or
    rejection on the requested network to trigger sending a response to
    an MBIM_CID_REGISTER_STATE set request

    @details
    This function relies on the static indication handler
    qbi_svc_bc_nas_register_state_e_nas4e_ind_cb to update the cache before
    it is executed. This is currently guaranteed as indication handlers are
    executed in order based on when they were registered, and all static
    indication handlers are registered before dynamic handlers.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_nas4e_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  boolean *has_detached;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn->info);

  /* A change to mode preference or registration mode usually results in PS
     detach, so make sure that happens before we consider the request complete.
     In the event that the modem completes the request without detaching
     (currently only happens in a few scenarios), we will send the response from
     the short timeout callback without waiting too long. */
  has_detached = (boolean *) ind->txn->info;
  if (*has_detached == FALSE &&
      !qbi_svc_bc_nas_is_attached(ind->txn->ctx))
  {
    *has_detached = TRUE;
  }

  if (*has_detached &&
      qbi_svc_bc_nas_register_state_s_is_request_complete(ind->txn))
  {
    action = qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query(
      ind->txn);
    if (action == QBI_SVC_ACTION_SEND_QMI_REQ)
    {
      /* We are ready to send the response, but need to perform a query first.
         Deregister this dynamic indication handler, so any indications occuring
         before the QMI response do not interfere with the response. */
      qbi_svc_ind_dereg_txn(ind->txn);
    }
  }
  else
  {
    QBI_LOG_I_2("Still waiting on registration to requested network for txn "
                "iid %d (has detached: %d)", ind->txn->iid, *has_detached);
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_nas_register_state_s_nas4e_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_sanity_check_req
===========================================================================*/
/*!
    @brief Performs basic sanity checks on the parameters provided by the
    host in a REGISTER_STATE set request

    @details
    May modify req->data_class by clearing unsupported bits

    @param txn

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_s_sanity_check_req
(
  qbi_txn_s *txn
)
{
  boolean success = FALSE;
  qbi_svc_bc_register_state_s_req_s *req;
  uint32 device_caps_data_class;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  req = (qbi_svc_bc_register_state_s_req_s *) txn->req.data;

  /* Clean up the DataClass */
  device_caps_data_class = qbi_svc_bc_device_caps_get_data_class(txn->ctx);
  if (req->data_class & ~(device_caps_data_class))
  {
    QBI_LOG_W_2("Device attempting to register with unsupported data class "
                "0x%x (supported 0x%x) - removing unsupported bits",
                req->data_class, device_caps_data_class);
    req->data_class &= device_caps_data_class;
  }
  else if (req->data_class == QBI_SVC_BC_DATA_CLASS_NONE)
  {
    QBI_LOG_I_0("Requested data class is NONE - assuming automatic mode pref");
    req->data_class = device_caps_data_class;
  }

  /* Only allow 3GPP technologies for manual registration, following NAS
     System Selection guideline */
  if (req->register_action == QBI_SVC_BC_REGISTER_ACTION_MANUAL)
  {
    req->data_class &= QBI_SVC_BC_DATA_CLASS_3GPP_FAMILY;
    QBI_LOG_I_1("Requested data class is limited to 0x%x in manual mode",
                req->data_class);
  }

  if (req->data_class == QBI_SVC_BC_DATA_CLASS_NONE)
  {
    QBI_LOG_E_0("Host provided invalid data class!");
  }
  else if (req->register_action != QBI_SVC_BC_REGISTER_ACTION_AUTOMATIC &&
           req->register_action != QBI_SVC_BC_REGISTER_ACTION_MANUAL)
  {
    QBI_LOG_E_1("Invalid register_action %d", req->register_action);
  }
  else
  {
    success = TRUE;
  }

  if (!success)
  {
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }

  return success;
} /* qbi_svc_bc_nas_register_state_s_sanity_check_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_long_timeout_cb
===========================================================================*/
/*!
    @brief Invoked when the modem has not registered to the requested
    network after a period of time, so we send the response to the host

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_s_long_timeout_cb
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
  qbi_svc_bc_register_state_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_LOG_E_1("Timeout while waiting for registration to requested network! "
              "txn iid %d", txn->iid);
  qbi_txn_set_timeout(txn, QBI_TXN_TIMEOUT_DEFAULT_MS, NULL);

  req = (qbi_svc_bc_register_state_s_req_s *) txn->req.data;
  if (req == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer");
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    if (req->register_action == QBI_SVC_BC_REGISTER_ACTION_MANUAL)
    {
      txn->status = QBI_MBIM_STATUS_PROVIDER_NOT_VISIBLE;
    }
    action = qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query(txn);
  }
  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_nas_register_state_s_long_timeout_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_short_timeout_cb
===========================================================================*/
/*!
    @brief First stage of the manual registration timeout

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_s_short_timeout_cb
(
  qbi_txn_s *txn
)
{
  boolean *has_detached;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  if (txn == NULL || txn->info == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer");
    action = QBI_SVC_ACTION_ABORT;
  }
  else if (qbi_svc_bc_nas_register_state_s_is_request_complete(txn))
  {
    QBI_LOG_I_0("REGISTER_STATE set request completed by short timeout");
    qbi_txn_set_timeout(txn, QBI_SVC_BC_NAS_REGISTER_LONG_TIMEOUT_MS, NULL);
    action = qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query(txn);
    if (action == QBI_SVC_ACTION_SEND_QMI_REQ)
    {
      qbi_svc_ind_dereg_txn(txn);
    }
  }
  else
  {
    QBI_LOG_I_0("REGISTER_STATE set request not complete after short timeout");
    /* If we haven't detached by now, then we are not likely to do so, so allow
       the response as soon as we gauge the request to be complete. */
    has_detached = (boolean *) txn->info;
    *has_detached = TRUE;
    qbi_txn_set_timeout(txn, QBI_SVC_BC_NAS_REGISTER_LONG_TIMEOUT_MS,
                        qbi_svc_bc_nas_register_state_s_long_timeout_cb);
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_nas_register_state_s_short_timeout_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_wait_for_completion
===========================================================================*/
/*!
    @brief Waits for registration to the requested network before sending
    the response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_s_wait_for_completion
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
  boolean *has_detached;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  if (!qbi_svc_bc_radio_state_is_radio_on(txn->ctx))
  {
    QBI_LOG_I_0("Not waiting on registration - radio powered off");
    action = qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query(txn);
  }
  /* A request to set the mode preference while registered to a 3GPP network
     will always result in deregistration, but does not necessarily impact
     3GPP2 network status. */
  else if (qbi_svc_bc_nas_register_state_get_current_cellular_class(txn->ctx) ==
             QBI_SVC_BC_CELLULAR_CLASS_CDMA &&
           qbi_svc_bc_nas_register_state_s_is_request_complete(txn))
  {
    QBI_LOG_I_0("Already registered to requested network");
    action = qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query(txn);
  }
  else
  {
    /* Any change to registration state at this point will require the device
       to deregister from its current network. Track whether we have
       deregistered yet using the transaction's info pointer. */
    txn->info = QBI_MEM_MALLOC(sizeof(boolean));
    QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
    has_detached = (boolean *) txn->info;
    *has_detached = !qbi_svc_bc_nas_is_attached(txn->ctx);

    if (!qbi_svc_ind_reg_dynamic(
          txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_REGISTER_STATE,
          QBI_QMI_SVC_NAS, QMI_NAS_SYS_INFO_IND_MSG_V01,
          qbi_svc_bc_nas_register_state_s_nas4e_ind_cb, txn, NULL))
    {
      QBI_LOG_E_0("Failed to register dynamic indication handler");
      action = QBI_SVC_ACTION_ABORT;
    }
    else
    {
      QBI_LOG_I_1("Response to REGISTER_STATE set request (txn iid %d) pending "
                  "on registration to requested network", txn->iid);
      qbi_txn_set_timeout(txn, QBI_SVC_BC_NAS_REGISTER_SHORT_TIMEOUT_MS,
                          qbi_svc_bc_nas_register_state_s_short_timeout_cb);
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
  }

  return action;
} /* qbi_svc_bc_nas_register_state_s_wait_for_completion() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_send_deregistered_before_searching
===========================================================================*/
/*!
    @brief Sends a DEREGISTERED event if we are transitioning from registered
    to searching

    @details
    Needed to comply with Microsoft's implementation guidance document which
    requires the device to send a DEREGISTERED event when it loses signal,
    and the related WHCK tests.

    @param ctx
    @param cache
    @param old_register_state
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_register_state_send_deregistered_before_searching
(
  qbi_ctx_s              *ctx,
  qbi_svc_bc_nas_cache_s *cache,
  uint32                  old_register_state
)
{
  qbi_txn_s *txn;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(cache);

  if (qbi_svc_bc_nas_register_state_is_registered(old_register_state) &&
      cache->register_state.register_state ==
        QBI_SVC_BC_REGISTER_STATE_SEARCHING)
  {
    QBI_LOG_I_0("Spoofing DEREGISTERED state before reporting SEARCHING");
    txn = qbi_txn_alloc_event(
      ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_REGISTER_STATE);
    QBI_CHECK_NULL_PTR_RET(txn);

    cache->register_state.register_state =
      QBI_SVC_BC_REGISTER_STATE_DEREGISTERED;
    action = qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache(txn);
    (void) qbi_svc_proc_action(txn, action);
    cache->register_state.register_state = QBI_SVC_BC_REGISTER_STATE_SEARCHING;
  }
} /* qbi_svc_bc_nas_register_state_send_deregistered_before_searching() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_sync_nas33_rsp_cb
===========================================================================*/
/*!
    @brief Processes the QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP
    generated when synchronizing the modem to the last REGISTER_STATE set
    requst

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_sync_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_set_system_selection_preference_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_set_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    /* The cache will be updated after this response via regular processing of
       QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND */
    QBI_LOG_I_0("Successfully synchronized network selection preference");
  }

  return action;
} /* qbi_svc_bc_nas_register_state_sync_nas33_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_sync_nas34_rsp_cb
===========================================================================*/
/*!
    @brief Processes a QMI_NAS_GET_SYSTEM_SELECTION_PREF_RESP generated when
    synchronizing the modem to the last REGISTER_STATE set request

    @details
    Builds and sends a QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ if the
    modem is out of sync with the last REGISTER_STATE set request

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_register_state_sync_nas34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_system_selection_preference_resp_msg_v01 *qmi_rsp;
  nas_set_system_selection_preference_req_msg_v01 *nas33_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_nas_net_sel_pref_cfg_s nv_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->net_sel_pref_valid)
  {
    QBI_LOG_E_0("Missing network selection preference TLV!");
  }
  else if (!qbi_svc_bc_nas_register_state_sync_read_net_sel_pref_compat(
             qmi_txn->ctx, &nv_info))
  {
    /* Most likely the file doesn't exist; not an error */
    QBI_LOG_I_0("Couldn't read last network selection preference from NV");
  }
  else if (nv_info.net_sel_pref.net_sel_pref != qmi_rsp->net_sel_pref ||
            (nv_info.net_sel_pref.net_sel_pref == NAS_NET_SEL_PREF_MANUAL_V01 &&
             (!qmi_rsp->manual_net_sel_plmn_valid ||
              nv_info.net_sel_pref.mcc != qmi_rsp->manual_net_sel_plmn.mcc ||
              nv_info.net_sel_pref.mnc != qmi_rsp->manual_net_sel_plmn.mnc ||
              nv_info.mnc_includes_pcs_digit !=
                qmi_rsp->manual_net_sel_plmn.mnc_includes_pcs_digit)))
  {
    QBI_LOG_I_4("Syncing modem to net_sel_pref %d PLMN %03d-%02d w/PCS %d",
                nv_info.net_sel_pref.net_sel_pref, nv_info.net_sel_pref.mcc,
                nv_info.net_sel_pref.mnc, nv_info.mnc_includes_pcs_digit);
    nas33_req = (nas_set_system_selection_preference_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_NAS,
        QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
        qbi_svc_bc_nas_register_state_sync_nas33_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(nas33_req);

    nas33_req->net_sel_pref_valid = TRUE;
    nas33_req->net_sel_pref = nv_info.net_sel_pref;
    nas33_req->mnc_includes_pcs_digit_valid = TRUE;
    nas33_req->mnc_includes_pcs_digit = nv_info.mnc_includes_pcs_digit;
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    QBI_LOG_I_0("No REGISTER_STATE sync needed; modem is up to date");
  }

  return action;
} /* qbi_svc_bc_nas_register_state_sync_nas34_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_sync_read_net_sel_pref_compat
===========================================================================*/
/*!
    @brief Reads QBI_NV_STORE_CFG_ITEM_NET_SEL_PREF, maintaining
    compatibility with the old data structure which did not include the
    MNC includes PCS digit flag

    @details

    @param ctx
    @param net_sel_pref_cfg

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_sync_read_net_sel_pref_compat
(
  qbi_ctx_s                         *ctx,
  qbi_svc_bc_nas_net_sel_pref_cfg_s *net_sel_pref_cfg
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(net_sel_pref_cfg);

  if (qbi_nv_store_cfg_item_read(
        ctx, QBI_NV_STORE_CFG_ITEM_NET_SEL_PREF, net_sel_pref_cfg,
        sizeof(qbi_svc_bc_nas_net_sel_pref_cfg_s)))
  {
    QBI_LOG_D_0("Read network selection preference (new structure)");
    success = TRUE;
  }
  else if (qbi_nv_store_cfg_item_read(
             ctx, QBI_NV_STORE_CFG_ITEM_NET_SEL_PREF,
             &net_sel_pref_cfg->net_sel_pref,
             sizeof(net_sel_pref_cfg->net_sel_pref)))
  {
    QBI_LOG_D_0("Read network selection preference (old structure)");
    net_sel_pref_cfg->mnc_includes_pcs_digit =
      (net_sel_pref_cfg->net_sel_pref.mnc >= 100) ? TRUE : FALSE;
    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_nas_register_state_sync_read_net_sel_pref_compat() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_upate_cache_available_data_class
===========================================================================*/
/*!
    @brief Updates the cached AvailableDataClass value

    @details

    @param ctx
    @param cache
    @param new_register_state
    @param sys_info

    @return boolean TRUE if cached value changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_upate_cache_available_data_class
(
  qbi_ctx_s                       *ctx,
  qbi_svc_bc_nas_cache_s          *cache,
  uint32                           new_register_state,
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
  boolean changed = FALSE;
  uint32 new_available_data_class;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (!qbi_svc_bc_nas_register_state_is_registered(new_register_state))
  {
    if (cache->register_state.available_data_class != QBI_SVC_BC_DATA_CLASS_NONE)
    {
      QBI_LOG_I_1("AvailableDataClasses changed from 0x%x to NONE because not "
                  "registered", cache->register_state.available_data_class);
      cache->register_state.available_data_class = QBI_SVC_BC_DATA_CLASS_NONE;
      changed = TRUE;
    }
  }
  else
  {
    new_available_data_class =
      qbi_svc_bc_nas_register_state_sys_info_to_available_data_class(sys_info);
    if (cache->register_state.available_data_class != new_available_data_class)
    {
      QBI_LOG_I_2("AvailableDataClasses changed from 0x%x to 0x%x",
                  cache->register_state.available_data_class,
                  new_available_data_class);
      if (qbi_svc_bc_nas_register_state_hybrid_primary_stack_changed(
            cache->register_state.available_data_class,
            new_available_data_class))
      {
        QBI_LOG_I_0("Hybrid primary stack changed; forcing SIGNAL_STATE event");
        qbi_svc_force_event(
          ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_SIGNAL_STATE);
      }
      cache->register_state.available_data_class = new_available_data_class;
      changed = TRUE;
    }
  }

  return changed;
} /* qbi_svc_bc_nas_register_state_upate_cache_available_data_class() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_sys_info
===========================================================================*/
/*!
    @brief Updates the cache used by MBIM_CID_REGISTER_STATE with information
    from a QMI_NAS_GET_SYS_INFO_RESP or QMI_NAS_SYS_INFO_IND

    @details

    @param ctx
    @param sys_info

    @return boolean TRUE if a value in the cache changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_sys_info
(
  qbi_ctx_s                       *ctx,
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
  qbi_svc_bc_nas_cache_s *cache;
  boolean changed;
  boolean network_changed;
  boolean new_cache_only;
  uint32 new_register_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(sys_info);

  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  /* As part of the OOS handling, turn on register state cache only mode if
     out of service. In this mode only cached register state will be used in
     register state query, and cache will not be updated */
  new_register_state =
    qbi_svc_bc_nas_register_state_sys_info_to_register_state(ctx, sys_info);
  new_cache_only =
    qbi_svc_bc_nas_register_state_determine_cache_only_mode(
      ctx, new_register_state);
  if (new_cache_only != cache->register_state.cache_only)
  {
    QBI_LOG_I_2("Cache only mode changed from %d to %d",
                cache->register_state.cache_only, new_cache_only);
    cache->register_state.cache_only = new_cache_only;
    qbi_svc_bc_nas_register_state_cache_only_set_timeout(ctx, new_cache_only);
    qbi_svc_force_event(ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_SIGNAL_STATE);
  }

  if (cache->register_state.cache_only)
  {
    QBI_LOG_I_0("Register state cache only mode is enabled; preventing cache"
                "updates");
    return FALSE;
  }

  changed = qbi_svc_bc_nas_register_state_update_cache_current_cellular_class(
    ctx, cache, sys_info->primary_rat);
  network_changed = qbi_svc_bc_nas_register_state_update_cache_provider_id(
    ctx, cache, sys_info);

  /* Only report a change based on ProviderId update if it affects the CID */
  if (qbi_svc_bc_nas_register_state_is_provider_id_and_name_reported(
        new_register_state))
  {
    changed |= network_changed;
  }
  /* After this point, the network_changed variable considers both cellular
     class and ProviderId changes, including ProviderId changes that are not
     reported in the CID. */
  network_changed |= changed;

  changed |= qbi_svc_bc_nas_register_state_update_cache_nw_error(
    cache, sys_info, network_changed);
  if (changed)
  {
    /* Recalculate the register state since changes to the NwError cache may
       affect whether we judge it as denied or deregistered. */
    new_register_state =
      qbi_svc_bc_nas_register_state_sys_info_to_register_state(ctx, sys_info);
  }
  changed |= qbi_svc_bc_nas_register_state_upate_cache_available_data_class(
    ctx, cache, new_register_state, sys_info);
  changed |= qbi_svc_bc_nas_register_state_update_cache_register_state(
    ctx, cache, new_register_state, sys_info);
  changed |= qbi_svc_bc_nas_register_state_update_cache_roaming_text(
    cache, sys_info);

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_sys_info() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_current_cellular_class
===========================================================================*/
/*!
    @brief Updates the CurrentCellularClass value maintained in the cache
    for MBIM_CID_REGISTER_STATE with a new value based off the current
    primary RAT

    @details

    @param ctx
    @param cache
    @param primary_rat

    @return boolean TRUE if the cached value changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_current_cellular_class
(
  const qbi_ctx_s             *ctx,
  qbi_svc_bc_nas_cache_s      *cache,
  qbi_svc_bc_nas_primary_rat_e primary_rat
)
{
  boolean changed = FALSE;
  uint32 new_current_cellular_class;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  new_current_cellular_class =
    qbi_svc_bc_nas_register_state_primary_rat_to_cellular_class(
      ctx, primary_rat);
  if (cache->register_state.current_cellular_class !=
        new_current_cellular_class)
  {
    QBI_LOG_I_2("Current cellular class changed from %d to %d",
                cache->register_state.current_cellular_class,
                new_current_cellular_class);
    cache->register_state.current_cellular_class = new_current_cellular_class;

    /* Clear ProviderId and ProviderName since we changed network types */
    QBI_MEMSET(&cache->register_state.provider_id_ascii, 0,
               sizeof(cache->register_state.provider_id_ascii));
    (void) qbi_svc_bc_nas_register_state_update_cache_provider_name_3gpp(
      ctx, NULL, 0);
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_current_cellular_class() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_manual_reg_mcc_mnc
===========================================================================*/
/*!
    @brief Updates the cached MCC/MNC requested for manual network
    registration

    @details

    @param ctx
    @param mcc
    @param mnc

    @return boolean TRUE if a value changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_manual_reg_mcc_mnc
(
  qbi_ctx_s *ctx,
  uint16     mcc,
  uint16     mnc,
  boolean    mnc_is_3_digits
)
{
  qbi_svc_bc_nas_cache_s *cache;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (mcc != cache->register_state.qmi.manual_reg_mcc)
  {
    QBI_LOG_I_2("Manual reg MCC changed from %03d to %03d",
                cache->register_state.qmi.manual_reg_mcc, mcc);
    cache->register_state.qmi.manual_reg_mcc = mcc;
    changed = TRUE;
  }
  if (mnc != cache->register_state.qmi.manual_reg_mnc)
  {
    QBI_LOG_I_2("Manual reg MNC changed from %02d to %02d",
                cache->register_state.qmi.manual_reg_mnc, mnc);
    cache->register_state.qmi.manual_reg_mnc = mnc;
    changed = TRUE;
  }
  if (mnc_is_3_digits != cache->register_state.qmi.manual_reg_mnc_is_3_digits)
  {
    QBI_LOG_I_2("Manual reg MNC is 3 digits changed from %d to %d",
                cache->register_state.qmi.manual_reg_mnc_is_3_digits,
                mnc_is_3_digits);
    cache->register_state.qmi.manual_reg_mnc_is_3_digits = mnc_is_3_digits;
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_manual_reg_mcc_mnc() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_net_sel_pref
===========================================================================*/
/*!
    @brief Update the fields in the register state cache that come from QMI's
    network selection preference (RegisterMode and if in manual mode,
    ProviderId), including the mode preference

    @details
    Also resets the manual registration MCC/MNC if changing to a non-manual
    registration mode.

    @param ctx
    @param net_sel_pref
    @param mode_pref

    @return boolean TRUE if RegisterMode changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_net_sel_pref
(
  qbi_ctx_s                *ctx,
  nas_net_sel_pref_enum_v01 net_sel_pref,
  mode_pref_mask_type_v01   mode_pref
)
{
  qbi_svc_bc_nas_cache_s *cache;
  uint32 new_register_mode;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  new_register_mode =
    qbi_svc_bc_nas_register_state_qmi_net_sel_pref_to_mbim_reg_mode(
      net_sel_pref);
  if (cache->register_state.register_mode != new_register_mode)
  {
    QBI_LOG_I_2("RegisterMode changed from %d to %d",
                cache->register_state.register_mode, new_register_mode);
    cache->register_state.register_mode = new_register_mode;
    changed = TRUE;
    if (new_register_mode != QBI_SVC_BC_REGISTER_MODE_MANUAL)
    {
      (void) qbi_svc_bc_nas_register_state_update_cache_manual_reg_mcc_mnc(
        ctx, 0, 0, FALSE);
    }
  }

  /* Note that changing the mode preference doesn't result in this function
     returning TRUE, because the mode preference is not reported in the CID */
  if (cache->register_state.qmi.mode_pref != mode_pref)
  {
    QBI_LOG_I_2("Mode preference changed from 0x%x to 0x%x",
                cache->register_state.qmi.mode_pref, mode_pref);
    cache->register_state.qmi.mode_pref = mode_pref;
  }

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_net_sel_pref() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_nw_error
===========================================================================*/
/*!
    @brief Updates the cached NwError used in MBIM_CID_REGISTER_STATE

    @details
    Note that the same data source gets used for NwError in
    MBIM_CID_PACKET_SERVICE as well. That value should only differ in the
    case where we are registered but could not PS attach.

    @param cache
    @param sys_info
    @param network_changed Indicates whether the network changed, i.e.
    CellularClass or ProviderId changed

    @return boolean TRUE if cached value changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_nw_error
(
  qbi_svc_bc_nas_cache_s          *cache,
  const qbi_svc_bc_nas_sys_info_s *sys_info,
  boolean                          network_changed
)
{
  uint32 new_nw_error;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);
  QBI_CHECK_NULL_PTR_RET_FALSE(sys_info);

  if (cache->register_state.current_cellular_class ==
        QBI_SVC_BC_CELLULAR_CLASS_GSM &&
      qbi_svc_bc_nas_sys_info_is_reject_cause_valid(sys_info))
  {
    QBI_LOG_W_2("Network reject on domain %d with cause code %d",
                sys_info->common_3gpp->reg_reject_info.reject_srv_domain,
                sys_info->common_3gpp->reg_reject_info.rej_cause);
    new_nw_error = sys_info->common_3gpp->reg_reject_info.rej_cause;
  }
  else if (network_changed || qbi_svc_bc_nas_sys_info_is_ps_attached(sys_info))
  {
    new_nw_error = 0;
  }
  else
  {
    /* No change to NwError */
    new_nw_error = cache->register_state.nw_error;
  }

  if (cache->register_state.nw_error != new_nw_error)
  {
    QBI_LOG_I_2("REGISTER_STATE NwError changed from %d to %d",
                cache->register_state.nw_error, new_nw_error);
    cache->register_state.nw_error = new_nw_error;
    if (qbi_svc_bc_nas_register_state_nw_error_is_perm(new_nw_error) &&
        cache->register_state.perm_nw_error != new_nw_error)
    {
      QBI_LOG_W_2("REGISTER_STATE Persistent NwError changed from %d to %d",
                  cache->register_state.perm_nw_error, new_nw_error);
      cache->register_state.perm_nw_error = new_nw_error;
    }

    /* If zeroing out the REGISTER_STATE NwError cache, also clear the
       PACKET_SERVICE NwError cache, since the PACKET_SERVICE indication handler
       doesn't have the same information about whether the current PLMN changed,
       etc. Note that in the current design, it's possible for
       register_state.nw_error != 0 && packet_service.nw_error == 0, but not
       vice versa. */
    if (new_nw_error == 0 && cache->packet_service.nw_error != 0)
    {
      QBI_LOG_I_1("PACKET_SERVICE NwError changed from %d to 0",
                  cache->packet_service.nw_error);
      cache->packet_service.nw_error = 0;
    }
    changed = TRUE;
  }

  changed |= qbi_svc_bc_nas_register_state_update_cache_sim_rej_info(
    cache, sys_info);

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_nw_error() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_plmn_mode
===========================================================================*/
/*!
    @brief Updates the cached PLMN mode bit value

    @details

    @param ctx
    @param plmn_mode

    @return boolean TRUE if it changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_plmn_mode
(
  qbi_ctx_s             *ctx,
  nas_plmn_mode_enum_v01 plmn_mode
)
{
  qbi_svc_bc_nas_cache_s *cache;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (cache->register_state.qmi.plmn_mode != plmn_mode)
  {
    QBI_LOG_I_2("PLMN mode changed from %d to %d",
                cache->register_state.qmi.plmn_mode, plmn_mode);
    cache->register_state.qmi.plmn_mode = plmn_mode;
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_plmn_mode() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_provider_id
===========================================================================*/
/*!
    @brief Updates the cached ASCII representation of the ProviderId based
    on the common system information structure, along with the cached
    primary RAT and CSG ID used in network name queries

    @details
    Based on the primary RAT, will extract and convert either the
    3GPP2 SID or 3GPP PLMN ID into an ASCII encoded MBIM ProviderId.

    @param ctx
    @param cache
    @param sys_info
    @param new_register_state

    @return boolean TRUE if the cached ProviderId changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_provider_id
(
  qbi_ctx_s                       *ctx,
  qbi_svc_bc_nas_cache_s          *cache,
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
  boolean changed = FALSE;
  char new_provider_id_ascii[QBI_SVC_BC_PROVIDER_ID_ASCII_MAX_LEN];
  nas_radio_if_enum_v01 new_rat;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);
  QBI_CHECK_NULL_PTR_RET_FALSE(sys_info);

  QBI_MEMSET(new_provider_id_ascii, 0, sizeof(new_provider_id_ascii));
  switch (sys_info->primary_rat)
  {
    case QBI_SVC_BC_NAS_PRIMARY_RAT_NONE:
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_CDMA:
    case QBI_SVC_BC_NAS_PRIMARY_RAT_HDR:
      qbi_svc_bc_nas_register_state_sys_info_to_provider_id_3gpp2(
        sys_info, new_provider_id_ascii, sizeof(new_provider_id_ascii));
      break;

    case QBI_SVC_BC_NAS_PRIMARY_RAT_GSM:
    case QBI_SVC_BC_NAS_PRIMARY_RAT_WCDMA:
    case QBI_SVC_BC_NAS_PRIMARY_RAT_LTE:
    case QBI_SVC_BC_NAS_PRIMARY_RAT_TDSCDMA:
      qbi_svc_bc_nas_register_state_sys_info_to_provider_id_3gpp(
        sys_info, new_provider_id_ascii, sizeof(new_provider_id_ascii));
      break;

    default:
      QBI_LOG_E_1("Unexpected primary_rat %d", sys_info->primary_rat);
  }

  if (QBI_STRNCMP(
        cache->register_state.provider_id_ascii, new_provider_id_ascii,
        sizeof(new_provider_id_ascii)) != 0)
  {
    QBI_LOG_STR_2("ProviderId changed from '%s' to '%s'",
                  cache->register_state.provider_id_ascii,
                  new_provider_id_ascii);
    QBI_STRLCPY(cache->register_state.provider_id_ascii, new_provider_id_ascii,
                sizeof(cache->register_state.provider_id_ascii));
    changed = TRUE;
  }

  /* Primary RAT is also tracked, as it is passed in network name queries along
     with the PLMN ID. It's possible for the same PLMN ID to have different
     network names under different RATs. */
  new_rat = qbi_svc_bc_nas_register_state_primary_rat_to_qmi_radio_if_3gpp(
    sys_info->primary_rat);
  if (cache->register_state.qmi.rat != new_rat)
  {
    QBI_LOG_I_2("Current RAT changed from %d to %d",
                cache->register_state.qmi.rat, new_rat);
    cache->register_state.qmi.rat = new_rat;
    changed = TRUE;
  }

  /* PLMN name can also be affected by CSG ID */
  if (sys_info->csg_info == NULL && cache->register_state.qmi.csg_id_valid)
  {
    QBI_LOG_I_1("Left CSG ID %d", cache->register_state.qmi.csg_id);
    cache->register_state.qmi.csg_id_valid = FALSE;
    changed = TRUE;
  }
  else if (sys_info->csg_info != NULL &&
           (!cache->register_state.qmi.csg_id_valid ||
            cache->register_state.qmi.csg_id != sys_info->csg_info->id))
  {
    QBI_LOG_I_3("CSG ID changed from %d (valid = %d) to %d",
                cache->register_state.qmi.csg_id,
                cache->register_state.qmi.csg_id_valid, sys_info->csg_info->id);
    cache->register_state.qmi.csg_id_valid = TRUE;
    cache->register_state.qmi.csg_id = sys_info->csg_info->id;
    changed = TRUE;
  }

  /* Clear the old ProviderName if ProviderId or RAT changed, to force a network
     name query */
  if (changed &&
      cache->register_state.current_cellular_class ==
        QBI_SVC_BC_CELLULAR_CLASS_GSM)
  {
    (void) qbi_svc_bc_nas_register_state_update_cache_provider_name_3gpp(
      ctx, NULL, 0);
  }

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_provider_id() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_provider_name_3gpp
===========================================================================*/
/*!
    @brief Updates the ProviderName saved to cache for the current 3GPP
    network

    @details

    @param ctx
    @param provider_name_3gpp Pointer to buffer containing ProviderName
    encoded in UTF-16. Can be NULL if provider_name_3gpp_len == 0
    @param provider_name_3gpp_len Size of provider_name_3gpp in bytes

    @return boolean TRUE if the value changed and a REGISTER_STATE event
    should be sent, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_provider_name_3gpp
(
  const qbi_ctx_s *ctx,
  const uint8     *provider_name_3gpp,
  uint32           provider_name_3gpp_len
)
{
  boolean changed = FALSE;
  qbi_svc_bc_nas_cache_s *cache;
  char new_provider_name_ascii[QBI_SVC_BC_PROVIDER_NAME_ASCII_MAX_LEN];
  char old_provider_name_ascii[QBI_SVC_BC_PROVIDER_NAME_ASCII_MAX_LEN];
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (provider_name_3gpp_len > QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES)
  {
    QBI_LOG_E_1("Attempt to use invalid ProviderName length %d",
                provider_name_3gpp_len);
  }
  else if (provider_name_3gpp_len > 0 && provider_name_3gpp == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer");
  }
  else if (provider_name_3gpp_len !=
             cache->register_state.provider_name_3gpp_len ||
           (provider_name_3gpp_len != 0 &&
            QBI_MEMCMP(cache->register_state.provider_name_3gpp,
                       provider_name_3gpp, provider_name_3gpp_len) != 0))
  {
    (void) qbi_util_utf16_to_ascii(cache->register_state.provider_name_3gpp,
                                   cache->register_state.provider_name_3gpp_len,
                                   old_provider_name_ascii,
                                   sizeof(old_provider_name_ascii));
    if (provider_name_3gpp_len == 0)
    {
      new_provider_name_ascii[0] = '\0';
    }
    else
    {
      (void) qbi_util_utf16_to_ascii(provider_name_3gpp, provider_name_3gpp_len,
                                     new_provider_name_ascii,
                                     sizeof(new_provider_name_ascii));
      QBI_MEMSCPY(cache->register_state.provider_name_3gpp,
                  sizeof(cache->register_state.provider_name_3gpp),
                  provider_name_3gpp, provider_name_3gpp_len);
    }
    cache->register_state.provider_name_3gpp_len = provider_name_3gpp_len;
    QBI_LOG_STR_2("ProviderName changed from '%s' to '%s'",
                  old_provider_name_ascii, new_provider_name_ascii);

    /* ProviderName is only reported in the CID if we are registered or denied,
       so only report a change if it will be included in the CID */
    if (qbi_svc_bc_nas_register_state_is_provider_id_and_name_reported(
          cache->register_state.register_state))
    {
      changed = TRUE;
    }
  }

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_provider_name_3gpp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_register_state
===========================================================================*/
/*!
    @brief Updates the cached RegisterState value

    @details

    @param ctx
    @param cache
    @param new_register_state
    @param sys_info

    @return boolean TRUE if the cached value changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_register_state
(
  qbi_ctx_s                       *ctx,
  qbi_svc_bc_nas_cache_s          *cache,
  uint32                           new_register_state,
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
  boolean changed = FALSE;
  uint32 old_register_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (cache->register_state.register_state != new_register_state)
  {
    QBI_LOG_I_2("RegisterState changed from %d to %d",
                cache->register_state.register_state, new_register_state);
    old_register_state = cache->register_state.register_state;
    cache->register_state.register_state = new_register_state;
    changed = TRUE;

    /* Force a signal strength event if the effective registration state
       changed, because QMI will not give us an indication matching up with
       the recommended implementation here. This only applies for 3GPP. */
    if (cache->register_state.current_cellular_class ==
          QBI_SVC_BC_CELLULAR_CLASS_GSM &&
        qbi_svc_bc_nas_register_state_is_registered(old_register_state) !=
          qbi_svc_bc_nas_register_state_is_registered(new_register_state))
    {
      QBI_LOG_I_0("Effective registration status changed; forcing SIGNAL_STATE "
                  "event");
      qbi_svc_force_event(ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_SIGNAL_STATE);

      if (qbi_svc_bc_nas_register_state_is_registered(
            cache->register_state.register_state))
      {
        qbi_svc_atds_reset(ctx);
      }
    }

    qbi_svc_bc_nas_packet_service_send_detached_event_if_deregistered(
      ctx, sys_info, old_register_state, new_register_state);
    qbi_svc_bc_nas_register_state_send_deregistered_before_searching(
      ctx, cache, old_register_state);
  }

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_register_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_roaming_text
===========================================================================*/
/*!
    @brief Updates ERI-related cache values used to populate RoamingText

    @details

    @param cache
    @param sys_info

    @return boolean TRUE if RoamingText changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_roaming_text
(
  qbi_svc_bc_nas_cache_s          *cache,
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
  uint8 roam_ind;
  uint32 found_index;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);
  QBI_CHECK_NULL_PTR_RET_FALSE(sys_info);

  if (cache->register_state.eri.algo != QBI_SVC_BC_NAS_ERI_ALGO_NONE &&
      cache->register_state.eri.tbl != NULL &&
      sys_info->common != NULL && sys_info->common->roam_status_valid &&
      cache->register_state.current_cellular_class ==
        QBI_SVC_BC_CELLULAR_CLASS_CDMA)
  {
    roam_ind = qbi_svc_bc_nas_register_state_qmi_roam_status_to_roam_ind(
      sys_info->common->roam_status);
    if (roam_ind != cache->register_state.eri.roam_ind)
    {
      QBI_LOG_I_2("Roaming indicator changed from %d to %d",
                  cache->register_state.eri.roam_ind, roam_ind);
      cache->register_state.eri.roam_ind = roam_ind;
      if (qbi_util_binary_search(
            cache->register_state.eri.tbl, &roam_ind, 0,
            (cache->register_state.eri.num_tbl_entries - 1),
            qbi_svc_bc_nas_register_state_eri_compare, &found_index))
      {
        QBI_LOG_STR_2("Set roaming text for ERI %d to '%s'", roam_ind,
                      cache->register_state.eri.tbl[found_index].roam_text_ascii);
        cache->register_state.eri.tbl_index = (uint8) found_index;
        cache->register_state.eri.tbl_index_valid = TRUE;
      }
      else
      {
        cache->register_state.eri.tbl_index_valid = FALSE;
      }
      changed = TRUE;
    }
  }
  else if (cache->register_state.eri.tbl_index_valid)
  {
    QBI_LOG_I_0("Disabling roaming text");
    cache->register_state.eri.tbl_index_valid = FALSE;
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_roaming_text() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_update_cache_sim_rej_info
===========================================================================*/
/*!
    @brief Updates the SIM reject information cache, clearing perm_nw_error
    for both REGISTER_STATE and PACKET_SERVICE if the SIM moves out of an
    invalid state

    @details

    @param cache
    @param sys_info

    @return boolean TRUE if REGISTER_STATE's perm_nw_error field changed
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_register_state_update_cache_sim_rej_info
(
  qbi_svc_bc_nas_cache_s          *cache,
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);
  QBI_CHECK_NULL_PTR_RET_FALSE(sys_info);

  if (sys_info->sim_rej_info_valid &&
      cache->register_state.qmi.sim_rej_info != sys_info->sim_rej_info)
  {
    QBI_LOG_I_2("SIM reject info changed from %d to %d",
                cache->register_state.qmi.sim_rej_info,
                sys_info->sim_rej_info);
    /* Assuming that once a service domain is marked as invalid in the SIM,
       the only way it will become valid again is through AVAILABLE, e.g.
       AVAILABLE --> CS_PS_INVALID --> CS_INVALID is not a valid sequence */
    if (sys_info->sim_rej_info == NAS_SIM_AVAILABLE_V01 &&
        (cache->register_state.qmi.sim_rej_info == NAS_SIM_CS_INVALID_V01 ||
         cache->register_state.qmi.sim_rej_info == NAS_SIM_PS_INVALID_V01 ||
         cache->register_state.qmi.sim_rej_info == NAS_SIM_CS_PS_INVALID_V01))
    {
      if (cache->register_state.perm_nw_error != 0)
      {
        QBI_LOG_I_1("REGISTER_STATE Persistent NwError changed from %d to 0",
                    cache->register_state.perm_nw_error);
        cache->register_state.perm_nw_error = 0;
        changed = TRUE;
      }
      if (cache->packet_service.perm_nw_error != 0)
      {
        QBI_LOG_I_1("PACKET_SERVICE Persistent NwError changed from %d to 0",
                    cache->packet_service.perm_nw_error);
        cache->packet_service.perm_nw_error = 0;
      }
    }
    cache->register_state.qmi.sim_rej_info = sys_info->sim_rej_info;
  }

  return changed;
} /* qbi_svc_bc_nas_register_state_update_cache_sim_rej_info() */

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_SERVICE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_data_class_best_available
===========================================================================*/
/*!
    @brief Given a bitmask of multiple available data classes, returns the
    best individual data class

    @details

    @param available_data_class

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_packet_service_data_class_best_available
(
  uint32 available_data_class
)
{
  uint32 best_data_class = QBI_SVC_BC_DATA_CLASS_NONE;
/*-------------------------------------------------------------------------*/
  if (available_data_class & QBI_SVC_BC_DATA_CLASS_LTE)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_LTE;
  }
  else if (available_data_class & QBI_SVC_BC_DATA_CLASS_HSDPA)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_HSDPA;
    if (available_data_class & QBI_SVC_BC_DATA_CLASS_HSUPA)
    {
      best_data_class |= QBI_SVC_BC_DATA_CLASS_HSUPA;
      if (available_data_class & QBI_SVC_BC_DATA_CLASS_CUSTOM)
      {
        best_data_class |= QBI_SVC_BC_DATA_CLASS_CUSTOM;
      }
    }
  }
  else if (available_data_class & QBI_SVC_BC_DATA_CLASS_1XEVDO_REVB)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_1XEVDO_REVB;
  }
  else if (available_data_class & QBI_SVC_BC_DATA_CLASS_1XEVDO_REVA)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_1XEVDO_REVA;
  }
  else if (available_data_class & QBI_SVC_BC_DATA_CLASS_1XEVDO)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_1XEVDO;
  }
  else if (available_data_class & QBI_SVC_BC_DATA_CLASS_UMTS)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_UMTS;
  }
  else if (available_data_class & QBI_SVC_BC_DATA_CLASS_3XRTT)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_3XRTT;
  }
  else if (available_data_class & QBI_SVC_BC_DATA_CLASS_1XRTT)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_1XRTT;
  }
  else if (available_data_class & QBI_SVC_BC_DATA_CLASS_EDGE)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_EDGE;
  }
  else if (available_data_class & QBI_SVC_BC_DATA_CLASS_GPRS)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_GPRS;
  }
  else if (available_data_class & QBI_SVC_BC_DATA_CLASS_CUSTOM)
  {
    best_data_class = QBI_SVC_BC_DATA_CLASS_CUSTOM;
  }

  return best_data_class;
} /* qbi_svc_bc_nas_packet_service_data_class_best_available() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_determine_packet_service_state
===========================================================================*/
/*!
    @brief Determines the current MBIM PacketServiceState based on the
    current cached data

    @details

    @param register_state
    @param srv_domain
    @param dsd_rat_mask

    @return uint32 MBIM_PACKET_SERVICE_STATE
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_packet_service_determine_packet_service_state
(
  const qbi_svc_bc_nas_cache_s *cache
)
{
  uint32 packet_service_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  if (qbi_svc_bc_nas_register_state_is_registered(
        cache->register_state.register_state))
  {
    switch (cache->packet_service.qmi.srv_domain)
    {
      case SYS_SRV_DOMAIN_NO_SRV_V01:
      case SYS_SRV_DOMAIN_CS_ONLY_V01:
        packet_service_state = QBI_SVC_BC_PACKET_SERVICE_STATE_DETACHED;
        break;

      case SYS_SRV_DOMAIN_PS_ONLY_V01:
      case SYS_SRV_DOMAIN_CS_PS_V01:
        /* For LTE, QMI_NAS indicates PS attached state when the network accepts
           the attach request, but data services may reject a connection request
           until it activates the default EPS bearer and sends attach complete
           to the network. Similar logic also applies to 3GPP2. So wait on data
           system status to report the RAT is available before indicating we are
           attached. */
        if (((cache->register_state.available_data_class &
                QBI_SVC_BC_DATA_CLASS_LTE) != 0 ||
             (cache->register_state.available_data_class &
                QBI_SVC_BC_DATA_CLASS_3GPP2_FAMILY) != 0) &&
            (cache->packet_service.qmi.rat_mask == QMI_WDS_RAT_DONT_CARE_V01 ||
             cache->packet_service.qmi.rat_mask == QMI_WDS_RAT_NULL_BEARER_V01))
        {
          QBI_LOG_I_0("Waiting on DSD to report attach complete");
          packet_service_state = QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHING;
        }
        else
        {
          packet_service_state = QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED;
        }
        break;

      case SYS_SRV_DOMAIN_CAMPED_V01:
        packet_service_state = QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHING;
        break;

      default:
        QBI_LOG_E_1("Unrecognized srv_domain %d",
                    cache->packet_service.qmi.srv_domain);
        packet_service_state = QBI_SVC_BC_PACKET_SERVICE_STATE_UNKNOWN;
    }
  }
  else
  {
    packet_service_state = QBI_SVC_BC_PACKET_SERVICE_STATE_DETACHED;
  }

  return packet_service_state;
} /* qbi_svc_bc_nas_packet_service_determine_packet_service_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_eq_build_wds23_req
===========================================================================*/
/*!
    @brief Builds a QMI_WDS_GET_CURRENT_CHANNEL_RATE request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_eq_build_wds23_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_CURRENT_CHANNEL_RATE_REQ_V01,
    qbi_svc_bc_nas_packet_service_eq_wds23_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_nas_packet_service_eq_build_wds23_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_eq_wds23_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_CURRENT_CHANNEL_RATE_RESP for
    MBIM_CID_PACKET_SERVICE query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_eq_wds23_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_current_channel_rate_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_current_channel_rate_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    (void) qbi_svc_bc_nas_packet_service_update_cache_channel_rates(
      qmi_txn->ctx, qmi_rsp->rates.max_channel_rx_rate,
      qmi_rsp->rates.max_channel_tx_rate);
  }

  return qbi_svc_bc_nas_packet_service_q_rsp(qmi_txn->parent);
} /* qbi_svc_bc_nas_packet_service_eq_wds23_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Builds the response InformationBuffer for MBIM_CID_PACKET_SERVICE
    based on the information currently in the cache

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
  qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_bc_packet_service_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_bc_packet_service_rsp_s *)
    qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_packet_service_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->packet_service_state = cache->packet_service.packet_service_state;
  if (rsp->packet_service_state == QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED)
  {
    rsp->highest_available_data_class =
      cache->packet_service.highest_available_data_class;
    rsp->downlink_speed = cache->packet_service.downlink_speed;
    rsp->uplink_speed   = cache->packet_service.uplink_speed;
  }
  else
  {
    /* Populate cached register state NW error if available, leave data
       class and link speed set to 0 */
    rsp->nw_error = cache->packet_service.nw_error;
    if (rsp->nw_error == 0 && cache->packet_service.perm_nw_error != 0)
    {
      rsp->nw_error = cache->packet_service.perm_nw_error;
    }

    if (txn->cmd_type == QBI_MSG_CMD_TYPE_SET &&
        rsp->packet_service_state == QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHING)
    {
      rsp->packet_service_state = QBI_SVC_BC_PACKET_SERVICE_STATE_DETACHED;
    }
  }

  if (cache->packet_service.hold_events &&
      txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT)
  {
    QBI_LOG_I_0("Dropping packet service event because of hold_events flag");
    cache->packet_service.event_pending = TRUE;
    action = QBI_SVC_ACTION_ABORT;
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_nw_error_is_perm
===========================================================================*/
/*!
    @brief Checks whether the given attach reject cause code makes the SIM
    invalid for CS+PS/PS service until SIM reset/removal

    @details

    @param nw_error

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_packet_service_nw_error_is_perm
(
  uint32 nw_error
)
{
/*-------------------------------------------------------------------------*/
  return (nw_error == QBI_SVC_BC_NAS_REJECT_CAUSE_ILLEGAL_MS ||
          nw_error == QBI_SVC_BC_NAS_REJECT_CAUSE_ILLEGAL_ME ||
          nw_error == QBI_SVC_BC_NAS_REJECT_CAUSE_GPRS_SERVICES_NOT_ALLOWED ||
          nw_error == QBI_SVC_BC_NAS_REJECT_CAUSE_GPRS_AND_NON_GPRS_SERVICES_NOT_ALLOWED);
} /* qbi_svc_bc_nas_packet_service_nw_error_is_perm() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_q_nas4d_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_SYS_INFO_RESP for MBIM_CID_PACKET_SERVICE
    query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_q_nas4d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_sys_info_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_nas_sys_info_s sys_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_sys_info_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    if (qmi_rsp->resp.error == QMI_ERR_INFO_UNAVAILABLE_V01)
    {
      QBI_LOG_W_0("Serving system information not available!");
      /* Use default cached values. We should receive a QMI indication shortly
         informing us of the true status. */
      action = qbi_svc_bc_nas_packet_service_q_rsp(qmi_txn->parent);
    }
  }
  else
  {
    qbi_svc_bc_nas_sys_info_extract_rsp(&sys_info, qmi_rsp);

    /* Update the REGISTER_STATE cache, since we require it to have accurate
       RegisterState and AvailableDataClass information for PACKET_SERVICE */
    (void) qbi_svc_bc_nas_register_state_update_cache_sys_info(
      qmi_txn->ctx, &sys_info);
    (void) qbi_svc_bc_nas_packet_service_update_cache_sys_info(
      qmi_txn->ctx, &sys_info);

    action = qbi_svc_bc_nas_packet_service_q_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_q_nas4d_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_q_wds6b_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDS_GET_CURRENT_DATA_SYSTEM_STATUS_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_q_wds6b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_get_current_data_system_status_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_current_data_system_status_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("QMI gave error code %d", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->system_status_valid)
  {
    QBI_LOG_E_0("Missing expected TLV!");
  }
  else
  {
    (void) qbi_svc_bc_nas_packet_service_update_cache_ds_status(
      qmi_txn->ctx, &qmi_rsp->system_status);
    action = qbi_svc_bc_nas_packet_service_q_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_q_wds6b_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_q_rsp
===========================================================================*/
/*!
    @brief Perform query response processing for MBIM_CID_PACKET_SERVICE

    @details
    Checks whether we have the information required to send the response

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_q_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (txn->qmi_txns_pending == 0)
  {
    action = qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(txn);
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_q_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_qmi_ind_reg_nas70_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_LIMIT_SYS_INFO_REPORTING_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_qmi_ind_reg_nas70_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action;
  nas_limit_sys_info_ind_reporting_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_limit_sys_info_ind_reporting_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error %d setting NAS SYS_INFO limit", qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    action = (qmi_txn->parent->qmi_txns_pending > 0) ?
      QBI_SVC_ACTION_WAIT_ASYNC_RSP : QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /*! qbi_svc_bc_nas_packet_service_qmi_ind_reg_nas70_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_qmi_ind_reg_wds01_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDS_SET_EVENT_REPORT_RESP from enabling/disabling
    channel rate indications

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_qmi_ind_reg_wds01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action;
  wds_set_event_report_req_msg_v01 *qmi_req;
  wds_set_event_report_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wds_set_event_report_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wds_set_event_report_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for WDS indications!!! Error code %d",
                qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    action = (qmi_txn->parent->qmi_txns_pending > 0) ?
      QBI_SVC_ACTION_WAIT_ASYNC_RSP : QBI_SVC_ACTION_SEND_RSP;
  }

  /* Refresh the cache if we are enabling indications */
  if (qmi_req->limited_data_system_status == TRUE)
  {
    qbi_svc_internal_query(
      qmi_txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PACKET_SERVICE);
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_qmi_ind_reg_wds01_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_s_completion_cb
===========================================================================*/
/*!
    @brief MBIM_CID_PACKET_SERVICE set request completion callback used to
    decrement the reference count on applicable indications

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_packet_service_s_completion_cb
(
  const qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  qbi_svc_ind_qmi_reg_invoke(
    txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PACKET_SERVICE, FALSE, NULL);
} /* qbi_svc_bc_nas_packet_service_s_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_s_device_in_requested_state
===========================================================================*/
/*!
    @brief Checks whether device is currently in the packet attach state
    requested in the given MBIM_CID_PACKET_SERVICE set request transaction

    @details

    @param txn

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_packet_service_s_device_in_requested_state
(
  const qbi_txn_s *txn
)
{
  const qbi_svc_bc_packet_service_s_req_s *req;
  const qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  req = (const qbi_svc_bc_packet_service_s_req_s *) txn->req.data;
  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  return ((req->action == QBI_SVC_BC_PACKET_SERVICE_ACTION_ATTACH &&
           cache->packet_service.packet_service_state ==
             QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED) ||
          (req->action == QBI_SVC_BC_PACKET_SERVICE_ACTION_DETACH &&
           cache->packet_service.packet_service_state ==
             QBI_SVC_BC_PACKET_SERVICE_STATE_DETACHED));
} /* qbi_svc_bc_nas_packet_service_s_device_in_requested_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_s_nas33_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP for
    requesting explicit PS attach/detach

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  nas_set_system_selection_preference_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_set_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("QMI gave error code %d", qmi_rsp->resp.error);
  }
  else
  {
    action = qbi_svc_bc_nas_packet_service_s_wait_for_completion(
      qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_s_nas33_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_s_nas34_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_RESP, checking
    how to proceed with the MBIM_CID_PACKET_SERVICE set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_nas34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const qbi_svc_bc_packet_service_s_req_s *req;
  nas_get_system_selection_preference_resp_msg_v01 *qmi_rsp;
  nas_set_system_selection_preference_req_msg_v01 *nas33_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  req = (const qbi_svc_bc_packet_service_s_req_s *) qmi_txn->parent->req.data;
  qmi_rsp = (nas_get_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("QMI gave error code %d", qmi_rsp->resp.error);
  }
  else if (req->action == QBI_SVC_BC_PACKET_SERVICE_ACTION_DETACH &&
           qmi_rsp->srv_domain_pref_valid &&
           qmi_rsp->srv_domain_pref == QMI_SRV_DOMAIN_PREF_PS_ONLY_V01)
  {
    /* This check ensures we do not end up in a camped only state where the host
       may not send an attach request because we are not registered */
    QBI_LOG_E_0("Skipping sending detach request to modem: srv_domain_pref "
                "is PS only");
    qmi_txn->parent->status = QBI_MBIM_STATUS_OPERATION_NOT_ALLOWED;
  }
  else if (req->action == QBI_SVC_BC_PACKET_SERVICE_ACTION_ATTACH &&
           qmi_rsp->srv_domain_pref_valid &&
           (qmi_rsp->srv_domain_pref == QMI_SRV_DOMAIN_PREF_CS_PS_V01 ||
            qmi_rsp->srv_domain_pref == QMI_SRV_DOMAIN_PREF_PS_ONLY_V01))
  {
    /* If PS is in the domain preference, the modem will attempt to attach
       automatically, and sending a system selection preference request can
       cause the registration procedure to start over. To avoid this kind of
       delay, don't send the request and just wait on the attach to complete. */
    QBI_LOG_W_1("Skipping sending attach request to modem: srv_domain_pref "
                "is %d", qmi_rsp->srv_domain_pref);
    action = qbi_svc_bc_nas_packet_service_s_wait_for_completion(
      qmi_txn->parent);
  }
  else
  {
    nas33_req = (nas_set_system_selection_preference_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_NAS,
        QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
        qbi_svc_bc_nas_packet_service_s_nas33_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(nas33_req);

    nas33_req->change_duration_valid = TRUE;
    nas33_req->change_duration = NAS_POWER_CYCLE_V01;

    nas33_req->srv_domain_pref_valid = TRUE;
    nas33_req->srv_domain_pref =
      (req->action == QBI_SVC_BC_PACKET_SERVICE_ACTION_ATTACH) ?
        QMI_SRV_DOMAIN_PREF_PS_ATTACH_V01 : QMI_SRV_DOMAIN_PREF_PS_DETACH_V01;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_s_nas34_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_s_nas4e_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYS_INFO_IND, looking to send the
    response to a MBIM_CID_PACKET_SERVICE set operation

    @details
    This is a dynamic indication handler.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_nas4e_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  const qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_bc_nas_sys_info_s sys_info;
  const nas_sys_info_ind_msg_v01 *qmi_ind;
  const qbi_svc_bc_packet_service_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn->req.data);

  /*! @note Since the static indication handler
      qbi_svc_bc_nas_packet_service_e_nas4e_ind_cb() is always registered before
      this dynamic indication handler, we can assume that the packet service
      cache is up to date. */
  cache = qbi_svc_bc_nas_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_ind = (const nas_sys_info_ind_msg_v01 *) ind->buf->data;
  req = (const qbi_svc_bc_packet_service_s_req_s *) ind->txn->req.data;
  if (qbi_svc_bc_nas_packet_service_s_device_in_requested_state(ind->txn))
  {
    QBI_LOG_I_1("Packet attach action %d is now complete; sending response",
                req->action);
    action = qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(ind->txn);
  }
  else if (req->action == QBI_SVC_BC_PACKET_SERVICE_ACTION_ATTACH)
  {
    qbi_svc_bc_nas_sys_info_extract_ind(&sys_info, qmi_ind);
    if (qbi_svc_bc_nas_sys_info_is_reject_cause_valid(&sys_info) &&
        cache->packet_service.nw_error != 0 &&
        !qbi_svc_bc_nas_reject_cause_allows_retry(
          cache->packet_service.nw_error))
    {
      QBI_LOG_W_0("Packet attach action failed due to network reject");
      action = qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(ind->txn);
      ind->txn->status = QBI_MBIM_STATUS_FAILURE;
    }
  }
  else
  {
    QBI_LOG_I_2("Still waiting on completion of packet attach action %d "
                "(current state %d)",
                req->action, cache->packet_service.packet_service_state);
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_s_nas4e_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_s_timeout_cb
===========================================================================*/
/*!
    @brief Invoked when the modem has not performed the requested PS
    attach/detach operation within the expected period of time, so we assume
    that the network is not visible and send a response to the host.

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_packet_service_s_timeout_cb
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_bc_packet_service_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->req.data);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  req = (qbi_svc_bc_packet_service_s_req_s *) txn->req.data;
  QBI_LOG_E_3("Timeout while waiting on packet attach action %d to complete! "
              "RegisterState %d txn iid %d", req->action,
              cache->register_state.register_state, txn->iid);
  if (!qbi_svc_bc_nas_register_state_is_registered(
        cache->register_state.register_state))
  {
    txn->status = QBI_MBIM_STATUS_PROVIDER_NOT_VISIBLE;
  }
  else
  {
    txn->status = QBI_MBIM_STATUS_FAILURE;
  }
  (void) qbi_svc_proc_action(
    txn, qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(txn));
} /* qbi_svc_bc_nas_packet_service_s_timeout_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_s_wait_for_completion
===========================================================================*/
/*!
    @brief Adds dynamic indication handler & timeout for waiting on the
    requested operation to complete

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_wait_for_completion
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (qbi_svc_bc_nas_packet_service_s_device_in_requested_state(txn))
  {
    QBI_LOG_W_0("Entered requested state before expected");
    action = qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(txn);
  }
  else if (!qbi_svc_ind_reg_dynamic(
             txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PACKET_SERVICE,
             QBI_QMI_SVC_NAS, QMI_NAS_SYS_INFO_IND_MSG_V01,
             qbi_svc_bc_nas_packet_service_s_nas4e_ind_cb, txn, NULL) ||
           !qbi_svc_ind_reg_dynamic(
             txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PACKET_SERVICE,
             QBI_QMI_SVC_WDS, QMI_WDS_EVENT_REPORT_IND_V01,
             qbi_svc_bc_nas_packet_service_s_wds01_ind_cb, txn, NULL))
  {
    QBI_LOG_E_0("Couldn't register dynamic indication handler(s)!");
  }
  else
  {
    QBI_LOG_I_1("Result of PACKET_SERVICE set operation (txn iid %d) pending "
                "on indication with requested mode", txn->iid);
    qbi_txn_set_timeout(txn, QBI_SVC_BC_NAS_PS_ATTACH_TIMEOUT_MS,
                        qbi_svc_bc_nas_packet_service_s_timeout_cb);
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_s_wait_for_completion() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_s_wds01_ind_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDS_EVENT_REPORT_IND, looking to send the response to
    an MBIM_CID_PACKET_SERVICE set operation based on updated data system
    status

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_wds01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  const qbi_svc_bc_packet_service_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn->req.data);

  /*! We can assume that the cache has already been updated for this indication,
      so we only need to check whether the operation has completed. */
  req = (const qbi_svc_bc_packet_service_s_req_s *) ind->txn->req.data;
  if (qbi_svc_bc_nas_packet_service_s_device_in_requested_state(ind->txn))
  {
    QBI_LOG_I_1("Packet attach action %d is now complete; sending response",
                req->action);
    action = qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(ind->txn);
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_s_wds01_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_send_detached_event_if_deregistered
===========================================================================*/
/*!
    @brief Checks whether the UE is moving to deregistered without a discrete
    detached indication; if so, sends a PS detached event immediately

    @details
    Intended to be called from the REGISTER_STATE cache update routine, since
    the host expects to always see PS ATTACHED --> PS DETACHED *before*
    REGISTERED --> DEREGISTERED. Unless the UE explicitly performs a detach
    procedure before deregistering, the modem will typically move directly
    to the DEREGISTERED state. Since QBI's indication handling logic results
    in the REGISTER_STATE handler processing the indication before the
    PACKET_SERVICE one, we need this function to provide the CID event
    ordering the host expects.

    @param ctx
    @param sys_info
    @param old_register_state
    @param new_register_state
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_packet_service_send_detached_event_if_deregistered
(
  qbi_ctx_s                       *ctx,
  const qbi_svc_bc_nas_sys_info_s *sys_info,
  uint32                           old_register_state,
  uint32                           new_register_state
)
{
  qbi_txn_s *txn;
/*-------------------------------------------------------------------------*/
  if (qbi_svc_bc_nas_register_state_is_registered(old_register_state) &&
      !qbi_svc_bc_nas_register_state_is_registered(new_register_state) &&
      qbi_svc_bc_nas_is_attached(ctx))
  {
    QBI_LOG_I_0("Moving to deregistered without explicit detach; sending "
                "PACKET_SERVICE event first");
    if (qbi_svc_bc_nas_packet_service_update_cache_sys_info(ctx, sys_info))
    {
      /* Can't use qbi_svc_force_event() here, since we need to guarantee that
         the event is sent before returning from this function - using
         force_event would result in asynchronous query processing. */
      txn = qbi_txn_alloc_event(
        ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PACKET_SERVICE);
      QBI_CHECK_NULL_PTR_RET(txn);

      (void) qbi_svc_proc_action(
        txn, qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(txn));
    }
  }
} /* qbi_svc_bc_nas_packet_service_send_detached_event_if_deregistered() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_update_cache_channel_rates
===========================================================================*/
/*!
    @brief Updates the cached uplink & downlink bit rates used for
    MBIM_CID_PACKET_SERVICE

    @details

    @param ctx
    @param downlink_speed
    @param uplink_speed

    @return boolean TRUE if a value changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_packet_service_update_cache_channel_rates
(
  qbi_ctx_s *ctx,
  uint32     downlink_speed,
  uint32     uplink_speed
)
{
  qbi_svc_bc_nas_cache_s *cache;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (downlink_speed != cache->packet_service.downlink_speed)
  {
    QBI_LOG_I_2("Downlink speed changed from %d to %d (bps)",
                cache->packet_service.downlink_speed, downlink_speed);
    cache->packet_service.downlink_speed = downlink_speed;
    if (cache->packet_service.packet_service_state ==
          QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED)
    {
      changed = TRUE;
    }
  }

  if (uplink_speed != cache->packet_service.uplink_speed)
  {
    QBI_LOG_I_2("Uplink speed changed from %d to %d (bps)",
                cache->packet_service.uplink_speed, uplink_speed);
    cache->packet_service.uplink_speed = uplink_speed;
    if (cache->packet_service.packet_service_state ==
          QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED)
    {
      changed = TRUE;
    }
  }

  return changed;
} /* qbi_svc_bc_nas_packet_service_update_cache_channel_rates() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_update_cache_ds_status
===========================================================================*/
/*!
    @brief Updates the PACKET_SERVICE cache based on new data system status
    information

    @details

    @param ctx
    @param ds_status

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_packet_service_update_cache_ds_status
(
  const qbi_ctx_s                       *ctx,
  const wds_data_system_status_type_v01 *ds_status
)
{
  uint32 new_rat_mask;
  uint32 new_packet_service_state;
  qbi_svc_bc_nas_cache_s *cache;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ds_status);

  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (ds_status->preferred_network < 0 ||
      ds_status->preferred_network > ARR_SIZE(ds_status->network_info))
  {
    QBI_LOG_E_1("Invalid DS preferred network %d", ds_status->preferred_network);
  }
  else
  {
    new_rat_mask = ds_status->network_info[ds_status->preferred_network].rat_mask;
    if (new_rat_mask != cache->packet_service.qmi.rat_mask)
    {
      QBI_LOG_I_2("DSD RAT Mask changed from 0x%x to 0x%x",
                  cache->packet_service.qmi.rat_mask, new_rat_mask);
      cache->packet_service.qmi.rat_mask = new_rat_mask;
      new_packet_service_state =
        qbi_svc_bc_nas_packet_service_determine_packet_service_state(cache);
      if (!cache->register_state.cache_only &&
          new_packet_service_state != cache->packet_service.packet_service_state)
      {
        QBI_LOG_I_2("Packet service state changed from %d to %d due to DSD",
                    cache->packet_service.packet_service_state,
                    new_packet_service_state);
        cache->packet_service.packet_service_state = new_packet_service_state;
        changed = TRUE;
      }
    }
  }

  return changed;
} /* qbi_svc_bc_nas_packet_service_update_cache_ds_status() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_update_cache_sys_info
===========================================================================*/
/*!
    @brief Updates the PACKET_SERVICE cache with new NAS system information

    @details
    This function updates PacketServiceState and HighestAvailableDataClass.
    Assumes that the REGISTER_STATE cache is already up to date.

    @param ctx
    @param sys_info

    @return boolean TRUE if a cached value changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_packet_service_update_cache_sys_info
(
  qbi_ctx_s                       *ctx,
  const qbi_svc_bc_nas_sys_info_s *sys_info
)
{
  boolean changed = FALSE;
  qbi_svc_bc_nas_cache_s *cache;
  uint32 new_packet_service_state;
  uint32 new_highest_available_data_class;
  nas_service_domain_enum_type_v01 new_srv_domain = SYS_SRV_DOMAIN_NO_SRV_V01;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(sys_info);

  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (sys_info->common != NULL && sys_info->common->srv_domain_valid)
  {
    new_srv_domain = sys_info->common->srv_domain;
  }

  if (new_srv_domain != cache->packet_service.qmi.srv_domain)
  {
    QBI_LOG_I_2("Service domain changed from %d to %d",
                cache->packet_service.qmi.srv_domain, new_srv_domain);
    cache->packet_service.qmi.srv_domain = new_srv_domain;
  }

  new_packet_service_state =
    qbi_svc_bc_nas_packet_service_determine_packet_service_state(cache);
  if (!cache->register_state.cache_only &&
      new_packet_service_state != cache->packet_service.packet_service_state)
  {
    QBI_LOG_I_2("Packet service state changed from %d to %d",
                cache->packet_service.packet_service_state,
                new_packet_service_state);
    cache->packet_service.packet_service_state = new_packet_service_state;
    changed = TRUE;
    if (new_packet_service_state == QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED)
    {
      qbi_os_log_event(QBI_OS_LOG_EVENT_ATTACHED);
    }
  }

  new_highest_available_data_class =
    qbi_svc_bc_nas_packet_service_data_class_best_available(
      cache->register_state.available_data_class);
  if (cache->packet_service.highest_available_data_class !=
        new_highest_available_data_class)
  {
    QBI_LOG_I_2("HighestAvailableDataClass changed from 0x%x to 0x%x",
                cache->packet_service.highest_available_data_class,
                new_highest_available_data_class);
    cache->packet_service.highest_available_data_class =
      new_highest_available_data_class;
    if (cache->packet_service.packet_service_state ==
          QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED)
    {
      changed = TRUE;
    }
  }

  if (cache->register_state.current_cellular_class ==
        QBI_SVC_BC_CELLULAR_CLASS_GSM &&
      qbi_svc_bc_nas_sys_info_is_reject_cause_valid(sys_info) &&
      (sys_info->common_3gpp->reg_reject_info.reject_srv_domain ==
         SYS_SRV_DOMAIN_PS_ONLY_V01 ||
       sys_info->common_3gpp->reg_reject_info.reject_srv_domain ==
         SYS_SRV_DOMAIN_CS_PS_V01) &&
      cache->packet_service.nw_error !=
        sys_info->common_3gpp->reg_reject_info.rej_cause)
  {
    QBI_LOG_I_2("PACKET_SERVICE NwError changed from %d to %d",
                cache->packet_service.nw_error,
                sys_info->common_3gpp->reg_reject_info.rej_cause);
    cache->packet_service.nw_error =
      sys_info->common_3gpp->reg_reject_info.rej_cause;
    if (qbi_svc_bc_nas_packet_service_nw_error_is_perm(
          cache->packet_service.nw_error) &&
        cache->packet_service.perm_nw_error != cache->packet_service.nw_error)
    {
      QBI_LOG_W_2("PACKET_SERVICE Persistent NwError changed from %d to %d",
                  cache->packet_service.perm_nw_error,
                  cache->packet_service.nw_error);
      cache->register_state.perm_nw_error = cache->packet_service.nw_error;
    }
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_bc_nas_packet_service_update_cache_sys_info() */

/*! @} */

/*! @addtogroup MBIM_CID_SIGNAL_STATE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_build_nas03_req
===========================================================================*/
/*!
    @brief Constructs a QMI_NAS_INDICATION_REGISTER_REQ for (en|dis)abling
    SIG_INFO and ERR_RATE indications

    @details

    @param txn
    @param enable_inds Whether indications should be enabled, assuming the
    last MBIM_CID_SIGNAL_STATE set request did not disable them
    @param rsp_cb

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_signal_state_build_nas03_req
(
  qbi_txn_s            *txn,
  boolean               enable_inds,
  qbi_svc_qmi_rsp_cb_f *rsp_cb
)
{
  qbi_svc_bc_nas_cache_s *cache;
  nas_indication_register_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_req = (nas_indication_register_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01, rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  /* QMI NAS will remember our threshold list even after disabling indications,
     so no need to bother with QMI_NAS_CONFIG_SIG_INFO2 here */
  qmi_req->sig_info_valid = TRUE;
  qmi_req->sig_info =
    (enable_inds && cache->signal_state.rssi_threshold <= QBI_SVC_BC_RSSI_MAX);
  qmi_req->err_rate_valid = TRUE;
  qmi_req->err_rate =
    (enable_inds &&
     cache->signal_state.error_rate_threshold !=
       QBI_SVC_BC_SIGNAL_STATE_ERROR_RATE_THRESHOLD_DISABLED);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_nas_signal_state_build_nas03_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_custom_mapping_apply_nas6c_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_CONFIG_SIG_INFO2_RESP used to apply/reset the
    custom signal mapping

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_signal_state_custom_mapping_apply_nas6c_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_config_sig_info2_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_config_sig_info2_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("QMI returned error %d when trying to update SIG_INFO config",
                qmi_rsp->resp.error);
  }

  return QBI_SVC_ACTION_ABORT;
} /* qbi_svc_bc_nas_signal_state_custom_mapping_apply_nas6c_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_custom_mapping_free
===========================================================================*/
/*!
    @brief Zeros and frees memory associated with a custom signal mapping

    @details

    @param mapping
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_signal_state_custom_mapping_free
(
  qbi_svc_bc_nas_signal_mapping_s **mapping
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(mapping);

  if (*mapping != NULL)
  {
    QBI_MEMSET(*mapping, 0, sizeof(qbi_svc_bc_nas_signal_mapping_s));
    QBI_MEM_FREE(*mapping);
    *mapping = NULL;
  }
} /* qbi_svc_bc_nas_signal_state_custom_mapping_free() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_mapping_get
===========================================================================*/
/*!
    @brief Fetches a pointer to the signal mapping table for a given signal
    metric type

    @details
    Returns a custom mapping if available, otherwise default.

    @param ctx
    @param metric

    @return const qbi_svc_bc_nas_signal_mapping_s*
*/
/*=========================================================================*/
static const qbi_svc_bc_nas_signal_mapping_s *qbi_svc_bc_nas_signal_state_mapping_get
(
  const qbi_ctx_s               *ctx,
  qbi_svc_bc_nas_signal_metric_e metric
)
{
  const qbi_svc_bc_nas_cache_s *cache;
  const qbi_svc_bc_nas_signal_mapping_s *mapping = NULL;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(cache);

  if (!qbi_svc_bc_nas_signal_state_metric_is_valid(metric))
  {
    QBI_LOG_E_1("Tried to get mapping for invalid signal metric %d", metric);
  }
  else if (cache->signal_state.custom_mappings[metric] != NULL)
  {
    mapping = cache->signal_state.custom_mappings[metric];
  }
  else
  {
    mapping = &(qbi_svc_bc_nas_default_signal_mappings[metric]);
  }

  return mapping;
} /* qbi_svc_bc_nas_signal_state_mapping_get() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_mbim_rssi_to_bars
===========================================================================*/
/*!
    @brief Maps an MBIM coded RSSI value to expected number of UI display
    bars for Windows

    @details

    @param mbim_rssi

    @return uint32 Bar count [0-5]
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_signal_state_mbim_rssi_to_bars
(
  uint32 mbim_rssi
)
{
  const uint32 bar_thresholds[] = {
    2, 4, 7, 12, 17
  };
  uint32 num_bars = 0;
/*-------------------------------------------------------------------------*/
  if (mbim_rssi != QBI_SVC_BC_RSSI_UNKNOWN)
  {
    while (num_bars < ARR_SIZE(bar_thresholds) &&
           mbim_rssi >= bar_thresholds[num_bars])
    {
      num_bars++;
    }
  }

  return num_bars;
} /* qbi_svc_bc_nas_signal_state_mbim_rssi_to_bars() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_metric_is_valid
===========================================================================*/
/*!
    @brief Checks whether a signal metric enum falls in the valid range

    @details

    @param metric

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_signal_state_metric_is_valid
(
  qbi_svc_bc_nas_signal_metric_e metric
)
{
/*-------------------------------------------------------------------------*/
  return ((uint32) metric < QBI_SVC_BC_NAS_SIGNAL_METRIC_MAX);
} /* qbi_svc_bc_nas_signal_state_metric_is_valid() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi
===========================================================================*/
/*!
    @brief Maps a signal metric value, e.g. RSRP -118, to MBIM coded RSSI

    @details

    @param ctx
    @param metric
    @param value

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi
(
  const qbi_ctx_s               *ctx,
  qbi_svc_bc_nas_signal_metric_e metric,
  int16                          value
)
{
  uint32 index;
  int16 pseudo_rssi_dbm;
  uint32 mbim_rssi = QBI_SVC_BC_RSSI_UNKNOWN;
  const qbi_svc_bc_nas_signal_mapping_s *mapping;
/*-------------------------------------------------------------------------*/
  mapping = qbi_svc_bc_nas_signal_state_mapping_get(ctx, metric);
  if (mapping == NULL)
  {
    QBI_LOG_E_1("Couldn't get signal mapping for metric %d", metric);
  }
  else if (metric != QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSSNR && value == 0)
  {
    QBI_LOG_W_1("Zero is not valid for metric %d; reporting UNKNOWN", metric);
  }
  else
  {
    /* Find the first index in the table with value >= min_value */
    for (index = 0; index < QBI_SVC_BC_NAS_SIGNAL_MAPPING_ENTRIES_MAX &&
           value < mapping->table[index].min_value; index++);

    if (index >= QBI_SVC_BC_NAS_SIGNAL_MAPPING_ENTRIES_MAX)
    {
      QBI_LOG_W_2("Metric %d value %d out of expected range", metric, value);
      mbim_rssi = QBI_SVC_BC_RSSI_MIN;
    }
    else
    {
      pseudo_rssi_dbm = (int16)
        ((mapping->table[index].coefficient * value +
          mapping->table[index].constant) /
         QBI_SVC_BC_NAS_SIGNAL_MAPPING_FACTOR);
      mbim_rssi = qbi_svc_bc_nas_signal_state_rssi_to_mbim(pseudo_rssi_dbm);
      QBI_LOG_I_3("Mapped signal metric %d value %d to MBIM RSSI %d",
                  metric, value, mbim_rssi);
      QBI_LOG_D_4("Using mapping index %d coeff %d constant %d pseudo RSSI %d",
                  index, mapping->table[index].coefficient,
                  mapping->table[index].constant, pseudo_rssi_dbm);
    }
  }

  return mbim_rssi;
} /* qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_metrics_combine_to_mbim_rssi
===========================================================================*/
/*!
    @brief Combines two signal metric values into a single MBIM coded RSSI

    @details

    @param ctx
    @param metric1
    @param value1
    @param metric2
    @param value2

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_signal_state_metrics_combine_to_mbim_rssi
(
  const qbi_ctx_s               *ctx,
  qbi_svc_bc_nas_signal_metric_e metric1,
  int16                          value1,
  qbi_svc_bc_nas_signal_metric_e metric2,
  int16                          value2
)
{
  uint32 rssi = QBI_SVC_BC_RSSI_UNKNOWN;
  uint32 rssi2 = QBI_SVC_BC_RSSI_UNKNOWN;
/*-------------------------------------------------------------------------*/
  rssi = qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi(
    ctx, metric1, value1);
  rssi2 = qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi(
    ctx, metric2, value2);

  if (rssi == QBI_SVC_BC_RSSI_UNKNOWN || rssi > rssi2)
  {
    rssi = rssi2;
  }

  return rssi;
} /* qbi_svc_bc_nas_signal_state_metrics_combine_to_mbim_rssi() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list
===========================================================================*/
/*!
    @brief Populates an individual threshold list in
    QMI_NAS_CONFIG_SIG_INFO2_REQ based on the current signal mapping

    @details

    @param ctx
    @param metric
    @param threshold_list_len
    @param threshold_list
    @param threshold_list_size

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list
(
  const qbi_ctx_s               *ctx,
  qbi_svc_bc_nas_signal_metric_e metric,
  uint32_t                      *threshold_list_len,
  int16_t                       *threshold_list,
  uint32                         threshold_list_size
)
{
  uint32 i;
  boolean success = FALSE;
  const qbi_svc_bc_nas_signal_mapping_s *mapping;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(threshold_list_len);
  QBI_CHECK_NULL_PTR_RET_FALSE(threshold_list);

  mapping = qbi_svc_bc_nas_signal_state_mapping_get(ctx, metric);
  QBI_CHECK_NULL_PTR_RET_FALSE(mapping);

  if (threshold_list_size < QBI_SVC_BC_NAS_SIGNAL_MAPPING_ENTRIES_MAX)
  {
    QBI_LOG_E_2("Not enough room to populate threshold list! Got %d, "
                "expected at least %d", threshold_list_size,
                QBI_SVC_BC_NAS_SIGNAL_MAPPING_ENTRIES_MAX);
  }
  else
  {
    for (i = 0; i < QBI_SVC_BC_NAS_SIGNAL_MAPPING_ENTRIES_MAX; i++)
    {
      threshold_list[i] = (int16_t)
        (mapping->table[i].min_value * QBI_QMI_NAS_CONFIG_SIG_INFO2_FACTOR);
    }
    *threshold_list_len = QBI_SVC_BC_NAS_SIGNAL_MAPPING_ENTRIES_MAX;
    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_q_nas4f_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_GET_SIG_INFO_RESP for MBIM_CID_SIGNAL_STATE query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_signal_state_q_nas4f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  nas_get_sig_info_resp_msg_v01 *qmi_rsp;
  uint32 rssi;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_sig_info_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_INFO_UNAVAILABLE_V01)
  {
    QBI_LOG_E_1("QMI gave error code %d", qmi_rsp->resp.error);
  }
  else
  {
    rssi = qbi_svc_bc_nas_signal_state_qmi_sig_info_to_mbim_rssi(
      qmi_txn->ctx,
      qmi_rsp->cdma_sig_info_valid,  &qmi_rsp->cdma_sig_info,
      qmi_rsp->hdr_sig_info_valid,   &qmi_rsp->hdr_sig_info,
      qmi_rsp->gsm_sig_info_valid,    qmi_rsp->gsm_sig_info,
      qmi_rsp->wcdma_sig_info_valid, &qmi_rsp->wcdma_sig_info,
      qmi_rsp->lte_sig_info_valid,   &qmi_rsp->lte_sig_info,
      qmi_rsp->rscp_valid,            qmi_rsp->rscp);

    qbi_svc_bc_nas_signal_state_update_cache_signal(
      qmi_txn->ctx, rssi, QBI_SVC_BC_CACHE_NO_CHANGE_U32);

    action = (qmi_txn->parent->qmi_txns_pending > 0) ?
      QBI_SVC_ACTION_WAIT_ASYNC_RSP :
      qbi_svc_bc_nas_signal_state_eqs_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_nas_signal_state_q_nas4f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_q_nas52_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_GET_ERR_RATE_RESP for MBIM_CID_SIGNAL_STATE query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_signal_state_q_nas52_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  nas_get_err_rate_resp_msg_v01 *qmi_rsp;
  uint32 error_rate;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_err_rate_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_INFO_UNAVAILABLE_V01)
  {
    QBI_LOG_E_1("QMI gave error code %d", qmi_rsp->resp.error);
  }
  else
  {
    error_rate = qbi_svc_bc_nas_signal_state_qmi_error_rate_to_mbim(
      qmi_rsp->cdma_frame_err_rate_valid, qmi_rsp->cdma_frame_err_rate,
      qmi_rsp->hdr_packet_err_rate_valid, qmi_rsp->hdr_packet_err_rate,
      qmi_rsp->gsm_bit_err_rate_valid,    qmi_rsp->gsm_bit_err_rate);

    qbi_svc_bc_nas_signal_state_update_cache_signal(
      qmi_txn->ctx, QBI_SVC_BC_CACHE_NO_CHANGE_U32, error_rate);

    action = (qmi_txn->parent->qmi_txns_pending > 0) ?
      QBI_SVC_ACTION_WAIT_ASYNC_RSP :
      qbi_svc_bc_nas_signal_state_eqs_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_nas_signal_state_q_nas52_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_qmi_error_rate_to_mbim
===========================================================================*/
/*!
    @brief Maps error rate information from QMI_NAS_GET_ERR_RATE_RESP or
    QMI_NAS_ERR_RATE_IND to an MBIM coded ErrorRate value

    @details

    @param cdma_frame_err_rate_valid
    @param cdma_frame_err_rate
    @param hdr_packet_err_rate_valid
    @param hdr_packet_err_rate
    @param gsm_bit_err_rate_valid
    @param gsm_bit_err_rate

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_signal_state_qmi_error_rate_to_mbim
(
  uint8_t  cdma_frame_err_rate_valid,
  uint16_t cdma_frame_err_rate,
  uint8_t  hdr_packet_err_rate_valid,
  uint16_t hdr_packet_err_rate,
  uint8_t  gsm_bit_err_rate_valid,
  uint8_t  gsm_bit_err_rate
)
{
  uint32 mbim_error_rate = QBI_SVC_BC_ERROR_RATE_UNKNOWN;
/*-------------------------------------------------------------------------*/
  if (gsm_bit_err_rate_valid &&
      gsm_bit_err_rate != QBI_QMI_NAS_ERROR_RATE_UNKNOWN_3GPP)
  {
    /* GSM BER is given as RXQUAL[0,7], which maps directly to the MBIM value */
    mbim_error_rate = gsm_bit_err_rate;
  }
  else if (hdr_packet_err_rate_valid &&
           hdr_packet_err_rate != QBI_QMI_NAS_ERORR_RATE_UNKNOWN_3GPP2)
  {
    mbim_error_rate = qbi_svc_bc_nas_signal_state_qmi_error_rate_3gpp2_to_mbim(
      hdr_packet_err_rate);
  }
  else if (cdma_frame_err_rate_valid &&
           cdma_frame_err_rate != QBI_QMI_NAS_ERORR_RATE_UNKNOWN_3GPP2)
  {
    mbim_error_rate = qbi_svc_bc_nas_signal_state_qmi_error_rate_3gpp2_to_mbim(
      cdma_frame_err_rate);
  }

  return mbim_error_rate;
} /* qbi_svc_bc_nas_signal_state_qmi_error_rate_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_qmi_error_rate_3gpp2_to_mbim
===========================================================================*/
/*!
    @brief Maps a CDMA FER or HDR PER reading to its MBIM coded ErrorRate

    @details

    @param qmi_3gpp2_error_rate

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_signal_state_qmi_error_rate_3gpp2_to_mbim
(
  uint16_t qmi_3gpp2_error_rate
)
{
  uint32 mbim_error_rate;
/*-------------------------------------------------------------------------*/
  /* MBIM's mapping of FER/PER to its coded value follows this table:

      FER/PER (%) | MBIM ErrorRate
     -------------+----------------
           < 0.01 | 0
      0.01 - 0.1  | 1
      0.1  - 0.5  | 2
      0.5  - 1.0  | 3
      1.0  - 2.0  | 4
      2.0  - 4.0  | 5
      4.0  - 8.0  | 6
           > 8.0  | 7

     Since QMI values are FER/PER in % * 100, the following array gives us
     the list of threshold values where the MBIM ErrorRate should increase.
  */
  static uint16 qmi_fer_to_mbim_threshold_list[QBI_SVC_BC_ERROR_RATE_MAX] =
    {1, 10, 50, 100, 200, 400, 800};
  for (mbim_error_rate = QBI_SVC_BC_ERROR_RATE_MIN;
       mbim_error_rate < QBI_SVC_BC_ERROR_RATE_MAX;
       mbim_error_rate++)
  {
    if (qmi_3gpp2_error_rate <= qmi_fer_to_mbim_threshold_list[mbim_error_rate])
    {
      break;
    }
  }

  return mbim_error_rate;
} /* qbi_svc_bc_nas_signal_state_qmi_error_rate_3gpp2_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_qmi_ind_reg_nas03_rsp_cb
===========================================================================*/
/*!
    @brief Response callback for QMI_NAS_SET_EVENT_REPORT used when enabling
    or disabling QMI RSSI indications

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_signal_state_qmi_ind_reg_nas03_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  nas_indication_register_req_msg_v01 *qmi_req;
  nas_indication_register_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (nas_indication_register_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (nas_indication_register_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  /* Refresh the cache if we are enabling indications */
  if (qmi_req->sig_info == TRUE || qmi_req->err_rate == TRUE)
  {
    qbi_svc_internal_query(
      qmi_txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_SIGNAL_STATE);
  }

  return action;
} /* qbi_svc_bc_nas_signal_state_qmi_ind_reg_nas03_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_qmi_sig_info_to_mbim_rssi
===========================================================================*/
/*!
    @brief Converts the signal readings from QMI_NAS_GET_SIG_INFO_RESP or
    QMI_NAS_SIG_INFO_IND to a coded MBIM RSSI value based on the associated
    mappings

    @details

    @param ctx
    @param cdma_sig_info_valid
    @param cdma_sig_info
    @param hdr_sig_info_valid
    @param hdr_sig_info
    @param gsm_rssi_valid
    @param gsm_rssi
    @param wcdma_sig_info_valid
    @param wcdma_sig_info
    @param lte_sig_info_valid
    @param lte_sig_info
    @param tdscdma_rscp_valid
    @param tdscdma_rscp

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_signal_state_qmi_sig_info_to_mbim_rssi
(
  const qbi_ctx_s                          *ctx,
  uint8_t                                   cdma_sig_info_valid,
  const nas_common_sig_info_param_type_v01 *cdma_sig_info,
  uint8_t                                   hdr_sig_info_valid,
  const nas_hdr_sig_info_type_v01          *hdr_sig_info,
  uint8_t                                   gsm_rssi_valid,
  int8_t                                    gsm_rssi,
  uint8_t                                   wcdma_sig_info_valid,
  const nas_common_sig_info_param_type_v01 *wcdma_sig_info,
  uint8_t                                   lte_sig_info_valid,
  const nas_lte_sig_info_type_v01          *lte_sig_info,
  uint8_t                                   tdscdma_rscp_valid,
  int8_t                                    tdscdma_rscp
)
{
  uint32 mbim_rssi;
/*-------------------------------------------------------------------------*/
  if (lte_sig_info_valid && lte_sig_info != NULL)
  {
    mbim_rssi = qbi_svc_bc_nas_signal_state_metrics_combine_to_mbim_rssi(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSRP, lte_sig_info->rsrp,
      QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSSNR,
      (int16) (lte_sig_info->snr / QBI_QMI_NAS_SNR_FACTOR));
  }
  else if (wcdma_sig_info_valid && wcdma_sig_info != NULL)
  {
    mbim_rssi = qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_UMTS_RSCP, wcdma_sig_info->rssi);
  }
  else if (gsm_rssi_valid)
  {
    mbim_rssi = qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_GSM_RSSI, gsm_rssi);
  }
  else if (hdr_sig_info_valid && hdr_sig_info != NULL &&
           (qbi_svc_bc_get_available_data_class(ctx) &
              QBI_SVC_BC_DATA_CLASS_FAMILY_HDR) != 0)
  {
    mbim_rssi = qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_HDR_RSSI,
      hdr_sig_info->common_sig_str.rssi);
  }
  else if (cdma_sig_info_valid && cdma_sig_info != NULL)
  {
    mbim_rssi = qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_CDMA_RSSI, cdma_sig_info->rssi);
  }
  else if (tdscdma_rscp_valid)
  {
    mbim_rssi = qbi_svc_bc_nas_signal_state_metric_to_mbim_rssi(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_UMTS_RSCP, tdscdma_rscp);
  }
  else
  {
    QBI_LOG_I_0("Signal information not available");
    mbim_rssi = QBI_SVC_BC_RSSI_UNKNOWN;
  }

  return mbim_rssi;
} /* qbi_svc_bc_nas_signal_state_qmi_sig_info_to_mbim_rssi() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_rssi_to_mbim
===========================================================================*/
/*!
    @brief Converts an RSSI value (dBm) to MBIM coded value

    @details
    Trends lower when rounding, e.g. RSSI -112 = MBIM 0.

    @param rssi Negative RSSI value, valid range [-51,-113]

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_nas_signal_state_rssi_to_mbim
(
  int16 rssi
)
{
  uint32 mbim_rssi;
/*-------------------------------------------------------------------------*/
  if (rssi <= QBI_SVC_BC_RSSI_MIN_DBM)
  {
    mbim_rssi = QBI_SVC_BC_RSSI_MIN;
  }
  else if (rssi >= QBI_SVC_BC_RSSI_MAX_DBM)
  {
    mbim_rssi = QBI_SVC_BC_RSSI_MAX;
  }
  else
  {
    mbim_rssi = (uint32) ((rssi - QBI_SVC_BC_RSSI_MIN_DBM) /
                          QBI_SVC_BC_RSSI_FACTOR);
  }

  return mbim_rssi;
} /* qbi_svc_bc_nas_signal_state_rssi_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_s_detect_host
===========================================================================*/
/*!
    @brief Compares the given MBIM_CID_SIGNAL_STATE set request contents
    with known values for Windows hosts to determine if we should follow
    RssiThreshold or use our own reporting scheme based on the number of
    UI display bars

    @details

    @param ctx
    @param req
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_signal_state_s_detect_host
(
  const qbi_ctx_s                       *ctx,
  const qbi_svc_bc_signal_state_s_req_s *req
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(req);

  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if ((req->error_rate_threshold ==
         QBI_SVC_BC_NAS_ERROR_RATE_THRESHOLD_WINDOWS_8_1 &&
       req->rssi_interval == QBI_SVC_BC_NAS_RSSI_INTERVAL_WINDOWS_8_1 &&
       req->rssi_threshold == QBI_SVC_BC_NAS_RSSI_THRESHOLD_WINDOWS_8_1) ||
      (req->error_rate_threshold ==
         QBI_SVC_BC_NAS_ERROR_RATE_THRESHOLD_WINDOWS_8 &&
       req->rssi_interval == QBI_SVC_BC_NAS_RSSI_INTERVAL_WINDOWS_8 &&
       req->rssi_threshold == QBI_SVC_BC_NAS_RSSI_THRESHOLD_WINDOWS_8))
  {
    QBI_LOG_I_0("Windows host detected; using UI bar thresholds");
    cache->signal_state.use_bar_thresholds = TRUE;
  }
  else
  {
    QBI_LOG_W_0("Non-Windows or test environment detected; using "
                "RssiThreshold");
    cache->signal_state.use_bar_thresholds = FALSE;
  }
} /* qbi_svc_bc_nas_signal_state_s_detect_host() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_s_nas03_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_INDICATION_REGISTER_RESP for
    MBIM_CID_SIGNAL_STATE set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_signal_state_s_nas03_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_indication_register_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (nas_indication_register_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
  }

  return qbi_svc_bc_nas_signal_state_eqs_rsp(qmi_txn->parent);
} /* qbi_svc_bc_nas_signal_state_s_nas03_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_update_cache_signal
===========================================================================*/
/*!
    @brief Updates the cached RSSI/ErrorRate values for MBIM_CID_SIGNAL_STATE

    @details

    @param ctx
    @param rssi New MBIM coded RSSI, or QBI_SVC_BC_CACHE_NO_CHANGE_U32
    @param error_rate New MBIM coded ErrorRate, or
    QBI_SVC_BC_CACHE_NO_CHANGE_U32
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_signal_state_update_cache_signal
(
  const qbi_ctx_s *ctx,
  uint32           rssi,
  uint32           error_rate
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (rssi != QBI_SVC_BC_CACHE_NO_CHANGE_U32)
  {
    QBI_LOG_I_2("Update cached RSSI from %d to %d",
                cache->signal_state.rssi, rssi);
    cache->signal_state.rssi = rssi;
  }

  if (error_rate != QBI_SVC_BC_CACHE_NO_CHANGE_U32)
  {
    QBI_LOG_I_2("Update cached error rate from %d to %d",
                cache->signal_state.error_rate, error_rate);
    cache->signal_state.error_rate = error_rate;
  }
} /* qbi_svc_bc_nas_signal_state_update_cache_signal() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_update_cache_reporting
===========================================================================*/
/*!
    @brief Updates the cached RssiInterval, RssiThreshold, and
    ErrorRateThreshold values

    @details
    All 3 parameters must be provided.

    @param ctx
    @param rssi_interval
    @param rssi_threshold
    @param error_rate_threshold
*/
/*=========================================================================*/
static void qbi_svc_bc_nas_signal_state_update_cache_reporting
(
  const qbi_ctx_s *ctx,
  uint32           rssi_interval,
  uint32           rssi_threshold,
  uint32           error_rate_threshold
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  QBI_LOG_I_2("Update cached RSSI interval from %d to %d",
              cache->signal_state.rssi_interval, rssi_interval);
  cache->signal_state.rssi_interval = rssi_interval;

  QBI_LOG_I_2("Update cached RSSI threshold from %d to %d",
              cache->signal_state.rssi_threshold, rssi_threshold);
  cache->signal_state.rssi_threshold = rssi_threshold;

  QBI_LOG_I_2("Update cached error rate threshold from %d to %d",
              cache->signal_state.error_rate_threshold, error_rate_threshold);
  cache->signal_state.error_rate_threshold = error_rate_threshold;
} /* qbi_svc_bc_nas_signal_state_update_cache_reporting() */

/*! @} */

/*! @addtogroup MBIM_CID_EMERGENCY_MODE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_emergency_mode_eq_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Allocates and populates an InformationBuffer for
    MBIM_CID_EMERGENCY_MODE query or event based on the cache

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_emergency_mode_eq_build_rsp_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_bc_emergency_mode_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_bc_emergency_mode_rsp_s *)
    qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_emergency_mode_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->emergency_mode = cache->emergency_mode.emergency_mode;
  if (rsp->emergency_mode == QBI_SVC_BC_EMERGENCY_MODE_ON)
  {
    QBI_LOG_W_0("Emergency mode is enabled!");
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_nas_emergency_mode_eq_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_emergency_mode_q_nas34_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_RESP for
    MBIM_CID_EMERGENCY_MODE query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_nas_emergency_mode_q_nas34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_system_selection_preference_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    if (!qmi_rsp->emergency_mode_valid)
    {
      QBI_LOG_E_0("Missing emergency mode TLV in response! Assuming OFF");
      qmi_rsp->emergency_mode = QBI_QMI_NAS_EMERGENCY_MODE_OFF;
    }

    (void) qbi_svc_bc_nas_emergency_mode_update_cache(
      qmi_txn->ctx, qmi_rsp->emergency_mode);
    action = qbi_svc_bc_nas_emergency_mode_eq_build_rsp_from_cache(
      qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_nas_emergency_mode_q_nas34_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_emergency_mode_update_cache
===========================================================================*/
/*!
    @brief Updates the cached EmergencyMode value

    @details

    @param ctx
    @param qmi_emergency_mode

    @return boolean TRUE if the cached value changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_nas_emergency_mode_update_cache
(
  qbi_ctx_s *ctx,
  uint32     qmi_emergency_mode
)
{
  qbi_svc_bc_nas_cache_s *cache;
  boolean changed = FALSE;
  uint32 mbim_emergency_mode;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (qmi_emergency_mode == QBI_QMI_NAS_EMERGENCY_MODE_ON)
  {
    mbim_emergency_mode = QBI_SVC_BC_EMERGENCY_MODE_ON;
  }
  else
  {
    mbim_emergency_mode = QBI_SVC_BC_EMERGENCY_MODE_OFF;
  }

  if (cache->emergency_mode.emergency_mode != mbim_emergency_mode)
  {
    QBI_LOG_I_2("Emergency mode changed from %d to %d",
                cache->emergency_mode.emergency_mode, mbim_emergency_mode);
    cache->emergency_mode.emergency_mode = mbim_emergency_mode;
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_bc_nas_emergency_mode_update_cache() */

/*! @} */

/*=============================================================================

  Public Function Definitions, exposed in qbi_svc_bc_nas.h

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_cache_alloc
===========================================================================*/
/*!
    @brief Allocates the Basic Connectivity NAS module cache

    @details
    Sets module_cache->nas to the location of the NAS module cache. This
    value must be NULL before calling this function. Also performs limited
    initialization of the NAS cache.

    @param ctx
    @param module_cache

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_cache_alloc
(
  qbi_ctx_s                 *ctx,
  qbi_svc_bc_module_cache_s *module_cache
)
{
  uint32 concat_plmn_spn = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(module_cache);

  if (module_cache->sim != NULL)
  {
    QBI_LOG_E_0("Tried to allocate SIM module cache, but not NULL!");
  }
  else
  {
    module_cache->nas = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_bc_nas_cache_s));
    QBI_CHECK_NULL_PTR_RET_FALSE(module_cache->nas);

    /* Initialize the cache items that default to non-zero values */
    module_cache->nas->signal_state.rssi = QBI_SVC_BC_RSSI_UNKNOWN;
    module_cache->nas->signal_state.error_rate = QBI_SVC_BC_ERROR_RATE_UNKNOWN;
    module_cache->nas->signal_state.rssi_threshold =
      QBI_SVC_BC_NAS_RSSI_THRESHOLD_DEFAULT;
    module_cache->nas->signal_state.rssi_interval =
      QBI_SVC_BC_NAS_RSSI_INTERVAL_DEFAULT;
    module_cache->nas->signal_state.error_rate_threshold =
      QBI_SVC_BC_NAS_ERROR_RATE_THRESHOLD_DEFAULT;

    /* Fetch the PLMN+SPN concatenation flag from NV, and save it to cache */
    if (qbi_nv_store_cfg_item_read(
          ctx, QBI_NV_STORE_CFG_ITEM_CONCAT_PLMN_NAME_SPN, &concat_plmn_spn,
          sizeof(concat_plmn_spn)))
    {
      module_cache->nas->register_state.concat_plmn_spn =
        (concat_plmn_spn) ? TRUE : FALSE;
    }
    else
    {
      module_cache->nas->register_state.concat_plmn_spn = FALSE;
    }
    QBI_LOG_D_1("PLMN Name + SPN concatenation enabled: %d",
                module_cache->nas->register_state.concat_plmn_spn);
  }

  return TRUE;
} /* qbi_svc_bc_nas_cache_alloc() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_cache_free
===========================================================================*/
/*!
    @brief Frees the BC NAS module cache

    @details

    @param module_cache
*/
/*=========================================================================*/
void qbi_svc_bc_nas_cache_free
(
  qbi_svc_bc_module_cache_s *module_cache
)
{
  uint32 i;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(module_cache);
  QBI_CHECK_NULL_PTR_RET(module_cache->nas);

  for (i = 0; i < ARR_SIZE(module_cache->nas->signal_state.custom_mappings); i++)
  {
    qbi_svc_bc_nas_signal_state_custom_mapping_free(
      &(module_cache->nas->signal_state.custom_mappings[i]));
  }
  qbi_svc_bc_nas_register_state_eri_reset(module_cache->nas);
  QBI_MEM_FREE(module_cache->nas);
  module_cache->nas = NULL;
} /* qbi_svc_bc_nas_cache_free() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_is_attached
===========================================================================*/
/*!
    @brief Checks whether the device is PS attached

    @details

    @param ctx

    @return boolean TRUE if attached, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_is_attached
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  return (cache->packet_service.packet_service_state ==
            QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED);
} /* qbi_svc_bc_nas_is_attached() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_is_registered
===========================================================================*/
/*!
    @brief Checks whether the device is registered with the network

    @details

    @param ctx

    @return boolean TRUE if registered, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_is_registered
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  return qbi_svc_bc_nas_register_state_is_registered(
    cache->register_state.register_state);
} /* qbi_svc_bc_nas_is_registered() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_sim_removed_clear_perm_nw_error
===========================================================================*/
/*!
    @brief Called when the SIM is removed to clear persistent NwError for
    REGISTER_STATE and PACKET_SERVICE (if present)

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_nas_sim_removed_clear_perm_nw_error
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache->register_state.qmi.sim_rej_info == NAS_SIM_CS_INVALID_V01 ||
      cache->register_state.qmi.sim_rej_info == NAS_SIM_PS_INVALID_V01 ||
      cache->register_state.qmi.sim_rej_info == NAS_SIM_CS_PS_INVALID_V01)
  {
    if (cache->register_state.perm_nw_error != 0)
    {
      QBI_LOG_I_1("REGISTER_STATE Persistent NwError changed from %d to 0 "
                  "due to SIM removal", cache->register_state.perm_nw_error);
      cache->register_state.perm_nw_error = 0;
    }
    if (cache->packet_service.perm_nw_error != 0)
    {
      QBI_LOG_I_1("PACKET_SERVICE Persistent NwError changed from %d to 0 "
                  "due to SIM removal", cache->packet_service.perm_nw_error);
      cache->packet_service.perm_nw_error = 0;
    }
  }
} /* qbi_svc_bc_nas_sim_removed_clear_perm_nw_error() */

/*! @addtogroup MBIM_CID_VISIBLE_PROVIDERS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_get_next_plmn_name
===========================================================================*/
/*!
    @brief Issues a QMI_NAS_GET_PLMN_NAME_REQ for the next PLMN in the
    network scan result

    @details
    qbi_svc_bc_nas_visible_providers_q_prepare_info() must be called first.

    @param txn
    @param qmi_txn QMI_NAS_GET_PLMN_NAME transaction to re-use

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_get_next_plmn_name
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action;
  nas_get_plmn_name_req_msg_v01 *qmi_req;
  qbi_svc_bc_nas_visible_providers_q_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  info = (qbi_svc_bc_nas_visible_providers_q_info_s *) txn->info;
  if (info->cur_index >= info->ntw_count)
  {
    QBI_LOG_I_1("Queried PLMN names for all %d visible providers, sending "
                "response", info->ntw_count);
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    qbi_qmi_txn_reset(qmi_txn);
    qmi_req = (nas_get_plmn_name_req_msg_v01 *) qmi_txn->req.data;
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->plmn.mcc = info->ntw[info->cur_index].mcc;
    qmi_req->plmn.mnc = info->ntw[info->cur_index].mnc;

    qmi_req->suppress_sim_error_valid = TRUE;
    qmi_req->suppress_sim_error = TRUE;

    qmi_req->mnc_includes_pcs_digit_valid = TRUE;
    qmi_req->mnc_includes_pcs_digit = info->ntw[info->cur_index].mnc_is_3_digits;

    qmi_req->send_all_information_valid = TRUE;
    qmi_req->send_all_information = TRUE;

    qmi_req->rat_valid = TRUE;
    qmi_req->rat = (nas_radio_if_enum_v01) info->ntw[info->cur_index].rat;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_nas_visible_providers_q_get_next_plmn_name() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_qmi_ntw_status_to_provider_state
===========================================================================*/
/*!
    @brief Maps the QMI network status field returned from a network scan
    into a MBIM ProviderState value

    @details

    @param qmi_ntw_status

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_bc_nas_visible_providers_qmi_ntw_status_to_provider_state
(
  uint8 qmi_ntw_status
)
{
  uint8 in_use_status;
  uint8 roaming_status;
  uint8 forbidden_status;
  uint8 preferred_status;
  uint32 provider_state = 0;
/*-------------------------------------------------------------------------*/
  in_use_status = ((qmi_ntw_status & QMI_NAS_NETWORK_IN_USE_STATUS_BITS_V01) >>
                   QBI_QMI_NAS_NETWORK_IN_USE_STATUS_SHIFT);
  if (in_use_status == QMI_NAS_NETWORK_IN_USE_STATUS_CURRENT_SERVING_V01)
  {
    provider_state |= QBI_SVC_BC_PROVIDER_STATE_REGISTERED;
    provider_state |= QBI_SVC_BC_PROVIDER_STATE_VISIBLE;
  }
  else if (in_use_status == QMI_NAS_NETWORK_IN_USE_STATUS_AVAILABLE_V01)
  {
    provider_state |= QBI_SVC_BC_PROVIDER_STATE_VISIBLE;
  }

  roaming_status = ((qmi_ntw_status & QMI_NAS_NETWORK_ROAMING_STATUS_BITS_V01) >>
                    QBI_QMI_NAS_NETWORK_ROAMING_STATUS_SHIFT);
  if (roaming_status == QMI_NAS_NETWORK_ROAMING_STATUS_HOME_V01)
  {
    provider_state |= QBI_SVC_BC_PROVIDER_STATE_HOME;
  }

  forbidden_status = ((qmi_ntw_status & QMI_NAS_NETWORK_FORBIDDEN_STATUS_BITS_V01) >>
                      QBI_QMI_NAS_NETWORK_FORBIDDEN_STATUS_SHIFT);
  if (forbidden_status == QMI_NAS_NETWORK_FORBIDDEN_STATUS_FORBIDDEN_V01)
  {
    provider_state |= QBI_SVC_BC_PROVIDER_STATE_FORBIDDEN;
  }

  preferred_status = ((qmi_ntw_status & QMI_NAS_NETWORK_PREFERRED_STATUS_BITS_V01) >>
                      QBI_QMI_NAS_NETWORK_PREFERRED_STATUS_SHIFT);
  if (preferred_status == QMI_NAS_NETWORK_PREFERRED_STATUS_PREFERRED_V01)
  {
    provider_state |= QBI_SVC_BC_PROVIDER_STATE_PREFERRED;
  }

  return provider_state;
} /* qbi_svc_bc_nas_visible_providers_qmi_ntw_status_to_provider_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_visible_providers_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_VISIBLE_PROVIDERS query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_visible_providers_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_visible_providers_q_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_visible_providers_q_req_s *) txn->req.data;
  if (!qbi_svc_bc_check_device_state(txn, FALSE, TRUE))
  {
    QBI_LOG_E_0("Network scan can't be performed while radio is off");
  }
  else if (req->action == QBI_SVC_BC_VISIBLE_PROVIDERS_FULL_SCAN)
  {
    if (qbi_svc_bc_nas_visible_providers_q_need_mode_pref_workaround(
               txn->ctx))
    {
      /* Need to temporarily set the mode preference to 3GPP only, then
         perform the network scan, then restore the mode preference. This is
         a workaround tied to WHCK. */
      action = qbi_svc_bc_nas_visible_providers_q_set_mode_pref(
        txn, qbi_svc_bc_nas_visible_providers_q_set_mode_pref_nas33_rsp_cb,
        FALSE, 0);
    }
    else
    {
      /* Devices with 3GPP support perform a manual network scan. */
      action = qbi_svc_bc_nas_visible_providers_q_build_nas21_req(txn);
    }
  }
  else if (req->action == QBI_SVC_BC_VISIBLE_PROVIDERS_QUICK_SCAN)
  {
    QBI_LOG_E_0("Received quick scan request; not supported!");
    txn->status = QBI_MBIM_STATUS_NO_DEVICE_SUPPORT;
  }
  else
  {
    QBI_LOG_E_1("Invalid action %d", req->action);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }

  return action;
} /* qbi_svc_bc_nas_visible_providers_q_req() */

/*! @} */

/*! @addtogroup MBIM_CID_REGISTER_STATE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_call_disconnected_check_cache_only_flag
===========================================================================*/
/*!
    @brief As part of the OOS handling, register state cache only mode should
    be turn off when call is disconnected and register state event should be
    posted to keep host in sync

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_nas_register_state_call_disconnected_check_cache_only_flag
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache->register_state.cache_only)
  {
    QBI_LOG_I_0("Turn off register state cache only mode upon disconnect");
    cache->register_state.cache_only = FALSE;
    cache->register_state.cache_only_caused_disconnect = FALSE;
    qbi_svc_bc_nas_register_state_cache_only_set_timeout(ctx, FALSE);

    /* If we are disabling cache only mode due to disconnection, it means that
       we are detached and deregistered. Send events for those two states in
       order. */
    qbi_svc_bc_nas_send_packet_service_then_register_state_events(ctx);
  }
  else if (cache->register_state.cache_only_caused_disconnect)
  {
    /* While disconnecting due to the cache only timeout, we regained service
       so the cache only flag was disabled. We need to spoof detached and
       deactivated states, then force events for the current registration state.
       This ensures that the HLOS does not think we disconnected because of a
       network-initiated request, and after seeing the register state change, it
       will attempt to reconnect if automatic connection is enabled. This needs
       to be sent after the deactivated CONNECT CID event (this function is
       called just before that happens), so a task command is used. */
    QBI_LOG_W_0("Cache only caused disconnect request, but exited cache only "
                "mode before teardown completed");
    cache->register_state.cache_only_caused_disconnect = FALSE;
    if (!qbi_task_cmd_send(
          ctx, QBI_TASK_CMD_ID_DEFERRED_EVENT,
          qbi_svc_bc_nas_spoof_detached_deregistered_cmd_cb, NULL))
    {
      QBI_LOG_E_0("Couldn't post command to spoof detached+deregistered");
    }
  }
} /* qbi_svc_bc_nas_register_state_call_disconnected_check_cache_only_flag */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas34_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND, looking for
    changes to RegisterMode that would trigger a MBIM_CID_REGISTER_STATE
    event.

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas34_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const nas_system_selection_preference_ind_msg_v01 *qmi_ind;
  qbi_svc_bc_nas_cache_s *cache;
  boolean send_event = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  cache = qbi_svc_bc_nas_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_ind = (const nas_system_selection_preference_ind_msg_v01 *)
    ind->buf->data;
  if (qmi_ind->net_sel_pref_valid && qmi_ind->mode_pref_valid &&
      qbi_svc_bc_nas_register_state_update_cache_net_sel_pref(
        ind->txn->ctx, qmi_ind->net_sel_pref, qmi_ind->mode_pref))
  {
    send_event = TRUE;
  }

  if (qmi_ind->manual_net_sel_plmn_valid &&
      qbi_svc_bc_nas_register_state_update_cache_manual_reg_mcc_mnc(
        ind->txn->ctx, qmi_ind->manual_net_sel_plmn.mcc,
        qmi_ind->manual_net_sel_plmn.mnc,
        qmi_ind->manual_net_sel_plmn.mnc_includes_pcs_digit))
  {
    send_event = TRUE;
  }

  if (send_event)
  {
    QBI_LOG_I_0("Sending MBIM_CID_REGISTER_STATE event");
    action = qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query(
      ind->txn);
  }
  else
  {
    QBI_LOG_D_0("Received NAS sys sel pref IND but no change to RegisterMode");
  }

  return action;
} /* qbi_svc_bc_nas_register_state_e_nas34_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas3a_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_OPERATOR_NAME_DATA_IND, looking for changes to
    the ProviderName that would trigger an event

    @details
    Although this indication contains the updated network name, since it is
    in a different format than QMI_NAS_GET_PLMN_NAME, we simplify our
    processing by just querying that QMI message rather than attempting to
    decode this indication.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas3a_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);

  QBI_LOG_I_0("Querying PLMN name due to operator name data indication");
  return qbi_svc_bc_nas_register_state_eq_build_nas44_req(
    ind->txn, qbi_svc_bc_nas_register_state_e_nas44_rsp_cb);
} /* qbi_svc_bc_nas_register_state_e_nas3a_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas3b_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_CSP_PLMN_MODE_BIT_IND, looking for changes to
    the restrict manual network registration flag that would trigger a
    MBIM_CID_REGISTER_STATE unsolicited event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas3b_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const nas_csp_plmn_mode_bit_ind_msg_v01 *qmi_ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const nas_csp_plmn_mode_bit_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->plmn_mode_valid &&
      qbi_svc_bc_nas_register_state_update_cache_plmn_mode(
        ind->txn->ctx, qmi_ind->plmn_mode))
  {
    QBI_LOG_I_0("Sending MBIM_CID_REGISTER_STATE indication due to PLMN mode "
                "bit change");
    action = qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query(
      ind->txn);
  }
  else
  {
    QBI_LOG_D_0("Received NAS CSP PLMN mode bit IND but no change");
  }

  return action;
} /* qbi_svc_bc_nas_register_state_e_nas3b_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas4e_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYS_INFO_IND, and sends an
    MBIM_CID_REGISTER_STATE unsolicited event if necessary

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas4e_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const nas_sys_info_ind_msg_v01 *qmi_ind;
  qbi_svc_bc_nas_sys_info_s sys_info;
  qbi_svc_bc_nas_cache_s *cache;
  uint32 old_register_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  cache = qbi_svc_bc_nas_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  old_register_state = cache->register_state.register_state;
  qmi_ind = (const nas_sys_info_ind_msg_v01 *) ind->buf->data;
  qbi_svc_bc_nas_sys_info_extract_ind(&sys_info, qmi_ind);
  if (qbi_svc_bc_nas_register_state_update_cache_sys_info(
        ind->txn->ctx, &sys_info))
  {
    action = qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query(
      ind->txn);
    if (action == QBI_SVC_ACTION_SEND_QMI_REQ &&
        qbi_svc_bc_nas_register_state_is_registered(
          cache->register_state.register_state) &&
        !qbi_svc_bc_nas_register_state_is_registered(
          old_register_state))
    {
      QBI_LOG_I_0("Moved from deregistered to registered and need to perform "
                  "query; setting packet service hold_events flag");
      cache->packet_service.hold_events = TRUE;
      qbi_txn_set_completion_cb(
        ind->txn, qbi_svc_bc_nas_register_state_e_completion_cb);
    }
  }

  return action;
} /* qbi_svc_bc_nas_register_state_e_nas4e_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_e_nas61_ind_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_CURRENT_PLMN_NAME_IND

    @details
    Although this indication contains the updated network name, since it is
    in a different format than QMI_NAS_GET_PLMN_NAME, we simplify our
    processing by just quering that QMI message rather than attempting to
    decode this indication.

    @param ind

    @return qbi_svc_action_e

    @see qbi_svc_bc_nas_register_state_e_nas3a_ind_cb
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_e_nas61_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);

  QBI_LOG_I_0("Querying PLMN name due to current PLMN name indication");
  return qbi_svc_bc_nas_register_state_eq_build_nas44_req(
    ind->txn, qbi_svc_bc_nas_register_state_e_nas44_rsp_cb);
} /* qbi_svc_bc_nas_register_state_e_nas61_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_eri_init
===========================================================================*/
/*!
    @brief Detects whether QBI has Enhanced Roaming Indicator (ERI) support
    for the given HPLMN ID, and if true, enables carrier-specific ERI
    functionality

    @details

    @param ctx
    @param home_mcc
    @param home_mnc
*/
/*=========================================================================*/
void qbi_svc_bc_nas_register_state_eri_init
(
  qbi_ctx_s *ctx,
  uint16     home_mcc,
  uint16     home_mnc
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (home_mcc == QBI_SVC_BC_NAS_HPLMN_MCC_VZW &&
      home_mnc == QBI_SVC_BC_NAS_HPLMN_MNC_VZW)
  {
    if (cache->register_state.eri.algo != QBI_SVC_BC_NAS_ERI_ALGO_VZW)
    {
      QBI_LOG_I_0("Enabling Verizon ERI rules; attempting to parse ERI file");
      qbi_svc_bc_nas_register_state_eri_reset(cache);
      qbi_svc_bc_nas_register_state_eri_parse_vzw(ctx);
      cache->register_state.eri.algo = QBI_SVC_BC_NAS_ERI_ALGO_VZW;
    }
  }
  else if (home_mcc == QBI_SVC_BC_NAS_HPLMN_MCC_SPR &&
           home_mnc == QBI_SVC_BC_NAS_HPLMN_MNC_SPR)
  {
    if (cache->register_state.eri.algo != QBI_SVC_BC_NAS_ERI_ALGO_SPR)
    {
      QBI_LOG_I_0("Enabling Sprint ERI rules; attempting to parse ERI files");
      qbi_svc_bc_nas_register_state_eri_reset(cache);
      qbi_svc_bc_nas_register_state_eri_parse_spr(ctx);
      qbi_svc_bc_nas_register_state_eri_parse_spr_intl_roam(ctx);
      cache->register_state.eri.algo = QBI_SVC_BC_NAS_ERI_ALGO_SPR;
    }
  }
  else
  {
    qbi_svc_bc_nas_register_state_eri_reset(cache);
  }
} /* qbi_svc_bc_nas_register_state_eri_init() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_get_current_cellular_class
===========================================================================*/
/*!
    @brief Returns the CurrentCellularClass value reported in
    MBIM_CID_REGISTER_STATE

    @details

    @param ctx

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_bc_nas_register_state_get_current_cellular_class
(
  const qbi_ctx_s *ctx
)
{
  uint32 current_cellular_class;
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  if (cache->register_state.current_cellular_class == 0)
  {
    current_cellular_class = QBI_SVC_BC_CELLULAR_CLASS_GSM;
    QBI_LOG_D_0("Current cellular class not known; using GSM");
  }
  else
  {
    current_cellular_class = cache->register_state.current_cellular_class;
  }

  return current_cellular_class;
} /* qbi_svc_bc_nas_register_state_get_current_cellular_class() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_REGISTER_STATE query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action;
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  qbi_txn_req_buf_ignored(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  /* As part of the OOS handling, cache only mode requires using the cached
     values without getting updates from QMI */
  if (cache->register_state.cache_only)
  {
    action = qbi_svc_bc_nas_register_state_eqs_build_rsp_from_cache(txn);
  }
  else
  {
    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_NAS,
                                QMI_NAS_GET_SYS_INFO_REQ_MSG_V01,
                                qbi_svc_bc_nas_register_state_q_nas4d_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_NAS,
                                QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
                                qbi_svc_bc_nas_register_state_q_nas34_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_NAS,
                                QMI_NAS_GET_CSP_PLMN_MODE_BIT_REQ_MSG_V01,
                                qbi_svc_bc_nas_register_state_q_nas3b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_nas_register_state_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_qmi_ind_reg
===========================================================================*/
/*!
    @brief Handles QMI indication (de)registration based off the current
    event filter setting for MBIM_CID_REGISTER_STATE

    @details

    @param txn
    @param enable_inds

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_qmi_ind_reg
(
  qbi_txn_s *txn,
  boolean    enable_inds
)
{
/*-------------------------------------------------------------------------*/
  return qbi_svc_bc_nas_sys_info_qmi_ind_reg(txn, enable_inds);
} /* qbi_svc_bc_nas_register_state_qmi_ind_reg() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_REGISTER_STATE set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_register_state_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_register_state_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_register_state_s_req_s *) txn->req.data;
  if (!qbi_svc_bc_nas_register_state_s_sanity_check_req(txn))
  {
    QBI_LOG_E_0("REGISTER_STATE set request failed sanity check");
  }
  /* Check if the request is to set us to our current mode. Issuing a QMI
     request, even if it has the same information, may change the device state
     (for example, it may initiate network re-selection if currently roaming).
     This check is primarily for TAEF, as it tends to issue set requests to
     Automatic when we are already in Automatic. Note that we are not trying to
     prevent ALL cases where we are already in the requested mode, rather those
     where we are in the requested mode AND already registered on the type of
     network requested. */
  else if (qbi_svc_bc_nas_register_state_s_is_request_complete(txn) &&
           qbi_svc_bc_nas_register_state_is_in_requested_mode(
             txn->ctx, req->register_action, req->data_class))
  {
    QBI_LOG_W_0("RegisterState set request has no effect! Returning early");
    action = qbi_svc_bc_nas_register_state_es_build_rsp_from_cache_or_query(txn);
  }
  else
  {
    /* Ensure SYS_INFO indications are registered for the duration of this set
       request */
    qbi_svc_ind_qmi_reg_invoke(txn->ctx, txn->svc_id, txn->cid, TRUE, NULL);
    qbi_txn_set_completion_cb(txn, qbi_svc_bc_register_state_s_completion_cb);

    /* When PS is attached without 3GPP data class, it is likely due to an
       earlier PS detach from 3GPP network followed by 3GPP2 attach. In this
       scenario PS is removed from service domain preference on modem side,
       hence modem will not switch to 3GPP data class unless service
       domain preference is restored with PS. Issue an explicit PS attach
       first in this case. Note that service domain preference needs to be
       sent in a separate QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ message
       before sending mode preference. */
    if (qbi_svc_bc_device_supports_3gpp2(txn->ctx) &&
        qbi_svc_bc_nas_is_attached(txn->ctx) &&
        (qbi_svc_bc_get_available_data_class(txn->ctx) &
           QBI_SVC_BC_DATA_CLASS_3GPP_FAMILY) == 0 &&
        (req->data_class & QBI_SVC_BC_DATA_CLASS_3GPP_FAMILY) != 0)
    {
      action = qbi_svc_bc_nas_register_state_s_build_nas33_ps_attach_req(txn);
    }
    else
    {
      action = qbi_svc_bc_nas_register_state_s_build_nas33_req(txn);
    }
  }

  return action;
} /* qbi_svc_bc_nas_register_state_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_register_state_sync
===========================================================================*/
/*!
    @brief Synchronizes the modem with the network selection preference from
    the last MBIM_CID_REGISTER_STATE set request

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_nas_register_state_sync
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_BC_MBIM_CID_REGISTER_STATE, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  /* Start by querying the current information */
  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
    qbi_svc_bc_nas_register_state_sync_nas34_rsp_cb);
  if (qmi_txn == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate QMI transaction!");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_nas_register_state_sync() */

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_SERVICE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_e_nas4e_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYS_INFO_IND, and sends an
    MBIM_CID_PACKET_SERVICE unsolicited event if necessary

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_packet_service_e_nas4e_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const nas_sys_info_ind_msg_v01 *qmi_ind;
  qbi_svc_bc_nas_sys_info_s sys_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  cache = qbi_svc_bc_nas_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_ind = (const nas_sys_info_ind_msg_v01 *) ind->buf->data;
  qbi_svc_bc_nas_sys_info_extract_ind(&sys_info, qmi_ind);
  if (qbi_svc_bc_nas_packet_service_update_cache_sys_info(
        ind->txn->ctx, &sys_info))
  {
    /* Data class or packet service state changed - if we are attached, get the
       new channel rate before sending the event to the host so both sets of
       fields change at the same time. If we aren't attached, the channel rates
       will be 0, so no need for the query. If this event is going to be
       dropped, skip the query and use regular processing to set the
       event_pending flag. */
    if (cache->packet_service.hold_events == FALSE &&
        cache->packet_service.packet_service_state ==
          QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED)
    {
      action = qbi_svc_bc_nas_packet_service_eq_build_wds23_req(ind->txn);
    }
    else
    {
      action = qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(ind->txn);
    }
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_e_nas4e_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_e_wds01_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_EVENT_REPORT_IND, looking for changes to the
    current channel rate or data system status that would trigger an
    MBIM_CID_PACKET_SERVICE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_packet_service_e_wds01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const wds_event_report_ind_msg_v01 *qmi_ind;
  boolean send_event = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const wds_event_report_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->channel_rate_valid &&
      qbi_svc_bc_nas_packet_service_update_cache_channel_rates(
        ind->txn->ctx, qmi_ind->channel_rate.current_channel_rx_rate,
        qmi_ind->channel_rate.current_channel_tx_rate))
  {
    send_event = TRUE;
  }
  if (qmi_ind->system_status_valid &&
      qbi_svc_bc_nas_packet_service_update_cache_ds_status(
        ind->txn->ctx, &qmi_ind->system_status))
  {
    cache = qbi_svc_bc_nas_cache_get(ind->txn->ctx);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);

    /* Same as for QMI_NAS_SYS_INFO_IND, if we are newly attached, query the
       channel rates to get accurate information before sending the event. */
    if (cache->packet_service.hold_events == FALSE &&
        cache->packet_service.packet_service_state ==
          QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED)
    {
      action = qbi_svc_bc_nas_packet_service_eq_build_wds23_req(ind->txn);
    }
    else
    {
      send_event = TRUE;
    }
  }

  if (send_event)
  {
    action = qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(ind->txn);
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_e_wds01_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PACKET_SERVICE query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_packet_service_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_SYS_INFO_REQ_MSG_V01,
    qbi_svc_bc_nas_packet_service_q_nas4d_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_CURRENT_SYSTEM_STATUS_REQ_V01,
    qbi_svc_bc_nas_packet_service_q_wds6b_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return qbi_svc_bc_nas_packet_service_eq_build_wds23_req(txn);
} /* qbi_svc_bc_nas_packet_service_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_qmi_ind_reg
===========================================================================*/
/*!
    @brief Handles QMI indication (de)registration based off the current
    event filter setting for MBIM_CID_PACKET_SERVICE

    @details

    @param txn
    @param enable_inds

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_packet_service_qmi_ind_reg
(
  qbi_txn_s *txn,
  boolean    enable_inds
)
{
  qbi_svc_action_e action;
  wds_set_event_report_req_msg_v01 *wds01_req;
  nas_limit_sys_info_ind_reporting_req_msg_v01 *nas70_req;
/*-------------------------------------------------------------------------*/
  action = qbi_svc_bc_nas_sys_info_qmi_ind_reg(txn, enable_inds);
  if (action != QBI_SVC_ACTION_ABORT &&
      qbi_svc_ind_qmi_ref_cnt_update_then_check_if_significant(
        txn->ctx, QBI_QMI_SVC_WDS, QMI_WDS_EVENT_REPORT_IND_V01, enable_inds))
  {
    wds01_req = (wds_set_event_report_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_SET_EVENT_REPORT_REQ_V01,
        qbi_svc_bc_nas_packet_service_qmi_ind_reg_wds01_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(wds01_req);
    wds01_req->report_channel_rate_valid = TRUE;
    wds01_req->report_channel_rate = enable_inds;
    wds01_req->limited_data_system_status_valid = TRUE;
    wds01_req->limited_data_system_status = enable_inds;

    /* If PACKET_SERVICE events are disabled, the host won't care about updates
       for AvailableDataClass (which is in the PACKET_SERVICE OID). Limit
       QMI_NAS so that it only sends SYS_INFO_IND on an important change,
       which also will prevent it from sending indications that we don't care
       about anyway, like cell ID changes. */
    nas70_req = (nas_limit_sys_info_ind_reporting_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_NAS, QMI_NAS_LIMIT_SYS_INFO_IND_REPORTING_REQ_MSG_V01,
        qbi_svc_bc_nas_packet_service_qmi_ind_reg_nas70_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(nas70_req);
    if (!enable_inds)
    {
      nas70_req->limit_sys_info_chg_rpt = (NAS_LIMIT_BY_SRV_STATUS_V01 |
                                           NAS_LIMIT_BY_SRV_DOMAIN_V01 |
                                           NAS_LIMIT_BY_PLMN_ID_V01 |
                                           NAS_LIMIT_BY_SID_NID_V01 |
                                           NAS_LIMIT_BY_ROAM_STATUS_V01);
    }

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_qmi_ind_reg() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_packet_service_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PACKET_SERVICE set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_packet_service_s_req
(
  qbi_txn_s *txn
)
{
  const qbi_svc_bc_packet_service_s_req_s *req;
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_bc_nas_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  req = (const qbi_svc_bc_packet_service_s_req_s *) txn->req.data;
  if (req->action != QBI_SVC_BC_PACKET_SERVICE_ACTION_ATTACH &&
      req->action != QBI_SVC_BC_PACKET_SERVICE_ACTION_DETACH)
  {
    QBI_LOG_E_1("Unrecognized attach action %d", req->action);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else if (!qbi_svc_bc_check_device_state(txn, TRUE, TRUE))
  {
    QBI_LOG_E_0("Device not ready to perform packet service request");
  }
  else if (qbi_svc_bc_nas_packet_service_s_device_in_requested_state(txn))
  {
    QBI_LOG_W_1("Received request for packet service action %d but already in "
                "requested state", req->action);
    action = qbi_svc_bc_nas_packet_service_eqs_build_rsp_from_cache(txn);
  }
  else
  {
    QBI_LOG_I_1("Received request to perform packet service action %d",
                req->action);
    qbi_svc_ind_qmi_reg_invoke(
      txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PACKET_SERVICE, TRUE, NULL);
    qbi_txn_set_completion_cb(
      txn, qbi_svc_bc_nas_packet_service_s_completion_cb);

    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
      qbi_svc_bc_nas_packet_service_s_nas34_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_nas_packet_service_s_req() */

/*! @} */

/*! @addtogroup MBIM_CID_SIGNAL_STATE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_bar_threshold_crossed
===========================================================================*/
/*!
    @brief Checks whether the given MBIM RSSI change would result in either
    a change in the number of UI display bars or if it represents a change
    between a known value and QBI_SVC_BC_RSSI_UNKNOWN

    @details

    @param new_rssi
    @param old_rssi

    @return boolean TRUE if bar threshold crossed or change between known and
    unknown value, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_signal_state_bar_threshold_crossed
(
  uint32 new_rssi,
  uint32 old_rssi
)
{
/*-------------------------------------------------------------------------*/
  return ((new_rssi == QBI_SVC_BC_RSSI_UNKNOWN &&
           old_rssi != QBI_SVC_BC_RSSI_UNKNOWN) ||
          (new_rssi != QBI_SVC_BC_RSSI_UNKNOWN &&
           old_rssi == QBI_SVC_BC_RSSI_UNKNOWN) ||
          qbi_svc_bc_nas_signal_state_mbim_rssi_to_bars(new_rssi) !=
            qbi_svc_bc_nas_signal_state_mbim_rssi_to_bars(old_rssi));
} /* qbi_svc_bc_nas_signal_state_bar_threshold_crossed() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_custom_mapping_apply
===========================================================================*/
/*!
    @brief Applies the cached custom mappings by updating the SIG_INFO
    registration with QMI and forcing a SIGNAL_STATE event

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_nas_signal_state_custom_mapping_apply
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
  nas_config_sig_info2_req_msg_v01 *qmi_req;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  qbi_svc_force_event(ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_SIGNAL_STATE);
  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_BC_MBIM_CID_SIGNAL_STATE, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  qmi_req = (nas_config_sig_info2_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_CONFIG_SIG_INFO2_REQ_MSG_V01,
      qbi_svc_bc_nas_signal_state_custom_mapping_apply_nas6c_rsp_cb);
  if (qmi_req == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate QMI transaction!");
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    action = qbi_svc_bc_nas_signal_state_populate_nas6c_req(txn->ctx, qmi_req);
  }
  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_nas_signal_state_custom_mapping_apply() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_custom_mapping_reset_all
===========================================================================*/
/*!
    @brief Resets the cached custom signal mappings for all signal metrics

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_nas_signal_state_custom_mapping_reset_all
(
  const qbi_ctx_s *ctx
)
{
  uint32 i;
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  for (i = 0; i < ARR_SIZE(cache->signal_state.custom_mappings); i++)
  {
    qbi_svc_bc_nas_signal_state_custom_mapping_free(
      &(cache->signal_state.custom_mappings[i]));
  }
  QBI_LOG_D_0("Reset custom signal mappings");
} /* qbi_svc_bc_nas_signal_state_custom_mapping_reset_all() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_custom_mapping_set
===========================================================================*/
/*!
    @brief Updates the cache with a custom mapping for the given signal
    metric

    @details
    After custom mappings are applied for all signal metrics, the caller
    should invoke qbi_svc_bc_nas_signal_state_custom_mapping_apply()

    @param ctx
    @param metric
    @param mapping The custom mapping to use. Will be copied to the cache.

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_signal_state_custom_mapping_set
(
  const qbi_ctx_s                       *ctx,
  qbi_svc_bc_nas_signal_metric_e         metric,
  const qbi_svc_bc_nas_signal_mapping_s *mapping
)
{
  boolean success = FALSE;
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (!qbi_svc_bc_nas_signal_state_metric_is_valid(metric))
  {
    QBI_LOG_E_1("Attempted to set custom mapping for invalid metric %d", metric);
  }
  else
  {
    if (cache->signal_state.custom_mappings[metric] != NULL)
    {
      QBI_LOG_I_1("Overriding existing mapping for metric %d", metric);
      qbi_svc_bc_nas_signal_state_custom_mapping_free(
        &(cache->signal_state.custom_mappings[metric]));
    }

    cache->signal_state.custom_mappings[metric] = QBI_MEM_MALLOC(
      sizeof(qbi_svc_bc_nas_signal_mapping_s));
    QBI_CHECK_NULL_PTR_RET_FALSE(cache->signal_state.custom_mappings[metric]);

    QBI_MEMSCPY(cache->signal_state.custom_mappings[metric],
                sizeof(qbi_svc_bc_nas_signal_mapping_s),
                mapping, sizeof(qbi_svc_bc_nas_signal_mapping_s));

    QBI_LOG_I_1("Set custom signal mapping for metric %d", metric);
    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_nas_signal_state_custom_mapping_set() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_eqs_rsp
===========================================================================*/
/*!
    @brief Allocates & populates the response/indication InformationBuffer
    for MBIM_CID_SIGNAL_STATE

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_eqs_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_signal_state_rsp_s *rsp;
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_bc_signal_state_rsp_s *)
    qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_signal_state_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  if (!qbi_svc_bc_nas_signal_state_is_active(txn->ctx))
  {
    rsp->rssi = (cache->register_state.cache_only) ?
      QBI_SVC_BC_RSSI_MIN : QBI_SVC_BC_RSSI_UNKNOWN;
    rsp->error_rate = QBI_SVC_BC_ERROR_RATE_UNKNOWN;
  }
  else
  {
    rsp->rssi       = cache->signal_state.rssi;
    rsp->error_rate = cache->signal_state.error_rate;
  }

  rsp->rssi_interval        = cache->signal_state.rssi_interval;
  rsp->rssi_threshold       = cache->signal_state.rssi_threshold;
  rsp->error_rate_threshold = cache->signal_state.error_rate_threshold;

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_nas_signal_state_eqs_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_e_nas51_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SIG_INFO_IND, looking for changes to the MBIM
    RSSI that would trigger an MBIM_CID_SIGNAL_STATE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_e_nas51_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const nas_sig_info_ind_msg_v01 *qmi_ind;
  const qbi_svc_bc_nas_cache_s *cache;
  uint32 new_rssi;
  uint32 rssi_delta;
  boolean send_event = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  cache = qbi_svc_bc_nas_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_ind = (const nas_sig_info_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->gsm_sig_info_valid || qmi_ind->wcdma_sig_info_valid ||
      qmi_ind->lte_sig_info_valid || qmi_ind->cdma_sig_info_valid ||
      qmi_ind->hdr_sig_info_valid || qmi_ind->rscp_valid)
  {
    new_rssi = qbi_svc_bc_nas_signal_state_qmi_sig_info_to_mbim_rssi(
      ind->txn->ctx,
      qmi_ind->cdma_sig_info_valid,  &qmi_ind->cdma_sig_info,
      qmi_ind->hdr_sig_info_valid,   &qmi_ind->hdr_sig_info,
      qmi_ind->gsm_sig_info_valid,    qmi_ind->gsm_sig_info,
      qmi_ind->wcdma_sig_info_valid, &qmi_ind->wcdma_sig_info,
      qmi_ind->lte_sig_info_valid,   &qmi_ind->lte_sig_info,
      qmi_ind->rscp_valid,            qmi_ind->rscp);

    rssi_delta = (new_rssi > cache->signal_state.rssi) ?
      new_rssi - cache->signal_state.rssi : cache->signal_state.rssi - new_rssi;
    if (cache->signal_state.use_bar_thresholds == FALSE &&
        rssi_delta >= cache->signal_state.rssi_threshold)
    {
      QBI_LOG_I_3("RSSI changed from %d to %d (threshold %d) - sending event",
                  cache->signal_state.rssi, new_rssi,
                  cache->signal_state.rssi_threshold);
      send_event = TRUE;
    }
    else if (cache->signal_state.use_bar_thresholds == TRUE &&
             qbi_svc_bc_nas_signal_state_bar_threshold_crossed(
               new_rssi, cache->signal_state.rssi))
    {
      QBI_LOG_I_2("RSSI changed from %d to %d (bar threshold crossed) - "
                  "sending event", cache->signal_state.rssi, new_rssi);
      send_event = TRUE;
    }
    else
    {
      QBI_LOG_D_4("RSSI changed, but not enough for indication (change from %d "
                  "to %d, threshold %d use_bar_thresholds %d)",
                  cache->signal_state.rssi, new_rssi,
                  cache->signal_state.rssi_threshold,
                  cache->signal_state.use_bar_thresholds);
    }

    if (send_event)
    {
      qbi_svc_bc_nas_signal_state_update_cache_signal(
        ind->txn->ctx, new_rssi, QBI_SVC_BC_CACHE_NO_CHANGE_U32);
      /* Update the cache but don't send the event if inactive */
      if (qbi_svc_bc_nas_signal_state_is_active(ind->txn->ctx))
      {
        action = qbi_svc_bc_nas_signal_state_eqs_rsp(ind->txn);
      }
    }
  }

  return action;
} /* qbi_svc_bc_nas_signal_state_e_nas51_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_e_nas53_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_ERR_RATE_IND, looking for changes to the MBIM
    error rate that would trigger an MBIM_CID_SIGNAL_STATE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_e_nas53_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  const nas_err_rate_ind_msg_v01 *qmi_ind;
  const qbi_svc_bc_nas_cache_s *cache;
  uint32 new_error_rate = QBI_SVC_BC_ERROR_RATE_UNKNOWN;
  uint32 error_rate_delta;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  cache = qbi_svc_bc_nas_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_ind = (const nas_err_rate_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->cdma_frame_err_rate_valid || qmi_ind->hdr_packet_err_rate_valid ||
      qmi_ind->gsm_bit_err_rate_valid)
  {
    new_error_rate = qbi_svc_bc_nas_signal_state_qmi_error_rate_to_mbim(
      qmi_ind->cdma_frame_err_rate_valid, qmi_ind->cdma_frame_err_rate,
      qmi_ind->hdr_packet_err_rate_valid, qmi_ind->hdr_packet_err_rate,
      qmi_ind->gsm_bit_err_rate_valid,    qmi_ind->gsm_bit_err_rate);

    error_rate_delta = (new_error_rate > cache->signal_state.error_rate) ?
      new_error_rate - cache->signal_state.error_rate :
      cache->signal_state.error_rate - new_error_rate;
    if (cache->signal_state.error_rate_threshold !=
          QBI_SVC_BC_SIGNAL_STATE_ERROR_RATE_THRESHOLD_DISABLED &&
        error_rate_delta >= cache->signal_state.error_rate_threshold)
    {
      QBI_LOG_I_3("Error rate changed from %d to %d (threshold %d) - sending "
                  "event", cache->signal_state.error_rate, new_error_rate,
                  cache->signal_state.error_rate_threshold);
      qbi_svc_bc_nas_signal_state_update_cache_signal(
        ind->txn->ctx, QBI_SVC_BC_CACHE_NO_CHANGE_U32, new_error_rate);
      action = qbi_svc_bc_nas_signal_state_eqs_rsp(ind->txn);
    }
  }

  return action;
} /* qbi_svc_bc_nas_signal_state_e_nas53_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_is_active
===========================================================================*/
/*!
    @brief Checks whether MBIM_CID_SIGNAL_STATE should report the accurate
    information given by QMI, or if it should report UNKNOWN and suppress
    events

    @details
    Recommended implementation is to report UNKNOWN and not send signal
    events when the device is not registered or the radio is off. QMI does
    not follow this implementation (it sends RSSI information whenever
    available, whether registered or not), so filtering is done in QBI.

    @param ctx

    @return boolean
*/
/*=========================================================================*/
boolean qbi_svc_bc_nas_signal_state_is_active
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_bc_nas_cache_s *cache;
  boolean signal_is_active;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if ((cache->register_state.current_cellular_class ==
         QBI_SVC_BC_CELLULAR_CLASS_GSM &&
       !qbi_svc_bc_nas_register_state_is_registered(
         cache->register_state.register_state)) ||
      !qbi_svc_bc_radio_state_is_radio_on(ctx))
  {
    QBI_LOG_I_0("Using UNKNOWN for RSSI/ErrorRate and suppressing events "
                "because not registered in 3GPP or radio is off");
    signal_is_active = FALSE;
  }
  else if (cache->register_state.cache_only)
  {
    QBI_LOG_I_0("Returning RSSI as 0 since cache only mode enabled");
    signal_is_active = FALSE;
  }
  else
  {
    signal_is_active = TRUE;
  }

  return signal_is_active;
} /* qbi_svc_bc_nas_signal_state_is_active() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_populate_nas6c_req
===========================================================================*/
/*!
    @brief Populates an already allocated QMI_NAS_CONFIG_SIG_INFO2_REQ with
    thresholds for each signal metric based on the current signal mappings

    @details

    @param ctx
    @param qmi_req

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_populate_nas6c_req
(
  const qbi_ctx_s                  *ctx,
  nas_config_sig_info2_req_msg_v01 *qmi_req
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->gsm_rssi_threshold_list_valid =
    qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_GSM_RSSI,
      &(qmi_req->gsm_rssi_threshold_list_len), qmi_req->gsm_rssi_threshold_list,
      ARR_SIZE(qmi_req->gsm_rssi_threshold_list));

  /* Note that WCDMA RSCP is reported by QMI NAS as RSSI */
  qmi_req->wcdma_rssi_threshold_list_valid =
    qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_UMTS_RSCP,
      &(qmi_req->wcdma_rssi_threshold_list_len),
      qmi_req->wcdma_rssi_threshold_list,
      ARR_SIZE(qmi_req->wcdma_rssi_threshold_list));

  /*! @note WCDMA ECIO indications are not supported, as the typical mapping
      range (-10 to -16 dB) is small enough that it results in a high number
      of indications under idle situations, leading to increased power
      consumption. */

  qmi_req->lte_rsrp_threshold_list_valid =
    qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSRP,
      &(qmi_req->lte_rsrp_threshold_list_len), qmi_req->lte_rsrp_threshold_list,
      ARR_SIZE(qmi_req->lte_rsrp_threshold_list));

  qmi_req->lte_snr_threshold_list_valid =
    qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSSNR,
      &(qmi_req->lte_snr_threshold_list_len), qmi_req->lte_snr_threshold_list,
      ARR_SIZE(qmi_req->lte_snr_threshold_list));

  qmi_req->cdma_rssi_threshold_list_valid =
    qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_CDMA_RSSI,
      &(qmi_req->cdma_rssi_threshold_list_len),
      qmi_req->cdma_rssi_threshold_list,
      ARR_SIZE(qmi_req->cdma_rssi_threshold_list));

  qmi_req->hdr_rssi_threshold_list_valid =
    qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_HDR_RSSI,
      &(qmi_req->hdr_rssi_threshold_list_len),
      qmi_req->hdr_rssi_threshold_list,
      ARR_SIZE(qmi_req->hdr_rssi_threshold_list));

  qmi_req->tdscdma_rscp_threshold_list_valid =
    qbi_svc_bc_nas_signal_state_populate_nas6c_threshold_list(
      ctx, QBI_SVC_BC_NAS_SIGNAL_METRIC_UMTS_RSCP,
      &(qmi_req->tdscdma_rscp_threshold_list_len),
      qmi_req->tdscdma_rscp_threshold_list,
      ARR_SIZE(qmi_req->tdscdma_rscp_threshold_list));

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_nas_signal_state_populate_nas6c_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SIGNAL_STATE query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (!qbi_svc_bc_nas_signal_state_is_active(txn->ctx))
  {
    action = qbi_svc_bc_nas_signal_state_eqs_rsp(txn);
  }
  else
  {
    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_SIG_INFO_REQ_MSG_V01,
      qbi_svc_bc_nas_signal_state_q_nas4f_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_ERR_RATE_REQ_MSG_V01,
      qbi_svc_bc_nas_signal_state_q_nas52_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_nas_signal_state_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_qmi_ind_reg
===========================================================================*/
/*!
    @brief Performs QMI (de)registration for the QMI_NAS_SIG_INFO_IND
    used by MBIM_CID_SIGNAL_STATE

    @details

    @param txn
    @param enable_inds

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_qmi_ind_reg
(
  qbi_txn_s *txn,
  boolean    enable_inds
)
{
/*-------------------------------------------------------------------------*/
  return qbi_svc_bc_nas_signal_state_build_nas03_req(
    txn, enable_inds, qbi_svc_bc_nas_signal_state_qmi_ind_reg_nas03_rsp_cb);
} /* qbi_svc_bc_nas_signal_state_qmi_ind_reg() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_signal_state_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SIGNAL_STATE set request

    @details
    In the current implementation, RssiInterval is ignored, as sending
    indications every so many seconds is seen as power inefficient. Also,
    RssiThreshold does not have any bearing on the way indications are
    registered with QMI - currently this is solely based off of the intervals
    of the signal mapping. RssiThreshold does have an impact on how these
    CIDs are reported: if a Windows host is detected (by matching the contents
    of the set request with known values), then QBI will only send CID events
    when it knows the update will impact the number of bars displayed on the
    UI. Otherwise, RssiThreshold is used as would be expected, but since QMI
    indications will only be sent at the mapping interval (also based off of
    the Windows bar display), this has limited impact on the effective
    behavior of QBI. It would be possible to iterate through the signal
    mappings and construct QMI indication threshold lists to follow
    RssiThreshold exactly, but this is not seen as a current requirement.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_signal_state_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_signal_state_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_signal_state_s_req_s *) txn->req.data;
  QBI_LOG_I_2("Setting signal state notification threshold to %d at "
              "interval %d", req->rssi_threshold, req->rssi_interval);
  qbi_svc_bc_nas_signal_state_s_detect_host(txn->ctx, req);

  /* Apply defaults if requested */
  if (req->rssi_threshold == QBI_SVC_BC_SIGNAL_STATE_USE_DEFAULT)
  {
    req->rssi_threshold = QBI_SVC_BC_NAS_RSSI_THRESHOLD_DEFAULT;
  }
  if (req->rssi_interval == QBI_SVC_BC_SIGNAL_STATE_USE_DEFAULT)
  {
    req->rssi_interval = QBI_SVC_BC_NAS_RSSI_INTERVAL_DEFAULT;
  }
  if (req->error_rate_threshold == QBI_SVC_BC_SIGNAL_STATE_USE_DEFAULT)
  {
    req->error_rate_threshold = QBI_SVC_BC_NAS_ERROR_RATE_THRESHOLD_DEFAULT;
  }

  qbi_svc_bc_nas_signal_state_update_cache_reporting(
    txn->ctx, req->rssi_interval, req->rssi_threshold,
    req->error_rate_threshold);

  /* The enable_inds argument is hard-coded to TRUE, since it is assumed that
     the host will not send MBIM_CID_SIGNAL_STATE set requests while the event
     is disabled (e.g. while in Connected Standby) */
  return qbi_svc_bc_nas_signal_state_build_nas03_req(
    txn, TRUE, qbi_svc_bc_nas_signal_state_s_nas03_rsp_cb);
} /* qbi_svc_bc_nas_signal_state_s_req() */

/*! @} */

/*! @addtogroup MBIM_CID_EMERGENCY_MODE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_emergency_mode_e_nas34_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND, looking for
    changes to the emergency mode that would trigger a
    MBIM_CID_EMERGENCY_MODE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_emergency_mode_e_nas34_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const nas_system_selection_preference_ind_msg_v01 *qmi_ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const nas_system_selection_preference_ind_msg_v01 *)
    ind->buf->data;
  if (qmi_ind->emergency_mode_valid &&
      qbi_svc_bc_nas_emergency_mode_update_cache(
        ind->txn->ctx, qmi_ind->emergency_mode))
  {
    action = qbi_svc_bc_nas_emergency_mode_eq_build_rsp_from_cache(ind->txn);
  }

  return action;
} /* qbi_svc_bc_nas_emergency_mode_e_nas34_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_nas_emergency_mode_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_EMERGENCY_MODE query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_nas_emergency_mode_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
    qbi_svc_bc_nas_emergency_mode_q_nas34_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_nas_emergency_mode_q_req() */

/*! @} */

/*=============================================================================

  Public Function Definitions, exposed in qbi_svc_bc.h

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_get_available_data_class
===========================================================================*/
/*!
    @brief Returns the current AvailableDataClass reported in
    MBIM_CID_REGISTER_STATE

    @details

    @param ctx

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_bc_get_available_data_class
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  return cache->register_state.available_data_class;
} /* qbi_svc_bc_get_available_data_class() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_get_error_rate_threshold
===========================================================================*/
/*!
    @brief Returns the current Error Rate Threshold set via
    MBIM_CID_SIGNAL_STATE

    @details

    @param ctx

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_bc_get_error_rate_threshold
(
  const qbi_ctx_s *ctx
)
{
  uint32 error_rate_threshold = QBI_SVC_BC_ERROR_RATE_UNKNOWN;
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  if (cache == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
  }
  else
  {
    error_rate_threshold = cache->signal_state.error_rate_threshold;
  }

  return error_rate_threshold;
} /* qbi_svc_bc_get_error_rate_threshold() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_get_preferred_data_class
===========================================================================*/
/*!
    @brief Returns the current QMI NAS mode preference

    @details

    @param ctx

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_bc_get_preferred_data_class
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_nas_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_nas_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  return cache->register_state.qmi.mode_pref;
} /* qbi_svc_bc_get_preferred_data_class() */

