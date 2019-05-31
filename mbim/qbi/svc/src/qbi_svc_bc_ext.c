/*!
  @file
  qbi_svc_bc_ext.c

  @brief
  Basic Connectivity Extension device service definitions.
*/

/*=============================================================================

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ----------------------------------------------------------------
09/21/18  nk   Publishing meta build as part of firmware info
09/06/18  nk   Corrected interpretation of provider IDs in cell info
09/04/18  nk   Publishing correct status code for PCO info failure
08/16/18  nk   Removed usage of unsecure print API
07/31/18  nk   Added logic to populate additional info for LTE serving cell
07/06/18  nk   Added fix to raise PCO request on active handle
06/27/18  nk   Fixed issue related to GSM neighbor cell info
05/24/18  nk   Added support for location info status from modem
03/23/18  nk   Added support for cell based location info from modem
01/04/18  nk   Added support for PCO information from Modem
11/14/17  mm   Fixed issue related to device caps
10/27/17  nk   Splitting bc_ext file into CID group based sub-files
10/09/17  nk   Fixed KW p1 issues and warnings
09/29/17  mm   Fixed issue related to slot info status during hotswap
09/13/17  rv   Fixed Class 2 disable should disable all class 1/2/3/4 profile
               Query/Set Prov Context Should fail if SIM not initialized
09/11/17  rv   Fixed MBIM to WDS IP and vice versa Mapping
09/11/17  mm   Fixed closure of channel on incorrect slot
09/08/17  vk   Exposing BC EXT cache for usage in BC
09/04/17  rv   1. Fixed Modem should fail query and set OIDs when No SIM is
               present. 2. Fixed Disabling class 2 DM config does not persist
               across factory restore. 3. Fixed Multiple APN Names can be 
               used for class 1.
08/11/17  rv   Prov Cntxt V2 Query to return both enabled/disabled profiles
07/26/17  mm   Fixed issue related to provisioning at the boot and hot swap
07/21/17  rv   Fixed crash when exec slot NV couldnt be created
07/20/17  mm   Fixed issue related to MEID
07/18/17  rv   Add logic to blow away cache if version is not present
07/11/17  vk   Fixed LTE Attach Status and LTE Attach Config
07/05/17  mm   Added logic to clear msuicc cache
07/05/17  vs   Added logic to close logical channel for slot mapping
06/28/17  rv   Fixed Prov Cntxt and LTE Attach Config Fatory Restore
06/26/17  vs   Fixed transition state handling during slot mapping
06/21/17  mm   Fixed issue related to slot info query
06/15/17  rv   Updated element count for LTE Attach query
06/15/17  mm   Fixed issue related to slot mapping
06/12/17  rv   Updated LTE attach handling for duplicate APNs
06/02/17  vk   Fixed double free of info pointer and cache access
05/30/17  rv   Updated LTE attach handling for set with duplicate APNs
05/23/17  mm   Added logic for getting slot state info
04/29/17  mm   Added device reset logic
04/25/17  rv   Fixed Profile Modify request when profile read is zero
05/05/17  rv   Handling empty username/password for LTE ATTACH CONFIG SET
04/28/17  rv   Fixed device caps query to return correct device type
03/22/17  mm   Code cleanup for DSSA feature and updated slot mapping feature
02/15/17  vk   Fixed provisoned context for create and modified operations
12/08/16  vk   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_bc_ext.h"
#include "qbi_svc_bc_ext_prov.h"
#include "qbi_svc_bc_ext_lte.h"
#include "qbi_svc_bc_ext_dssa.h"
#include "qbi_svc_bc_ext_mbim.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc_spdp.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_nv_store.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_txn.h"
#include "qbi_msg_mbim.h"

#include "wireless_data_service_v01.h"
#include "data_system_determination_v01.h"
#include "persistent_device_configuration_v01.h"
#include "device_management_service_v01.h"
#include "user_identity_module_v01.h"
#include "network_access_service_v01.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! This macro statically defines a QMI indication handler and fills in the
    fields that are common to all handlers in this device service */
#define QBI_SVC_BC_EXT_STATIC_IND_HDLR(qmi_svc_id, qmi_msg_id, cid, cb) \
  {qmi_svc_id, qmi_msg_id, cid, cb, NULL}

#define QBI_SVC_BC_EXT_MAP_DEFAULT_IP_TYPE(ip_type) \
  (((ip_type) == QBI_SVC_BC_IP_TYPE_DEFAULT) ? \
    QBI_SVC_BC_IP_TYPE_IPV4V6 : (ip_type))

/*! Validate DUAL IP Stack with MBIM IP type */
#define QBI_SVC_BC_EXT_IS_MBIM_DUAL_IP(ip_type) \
  (QBI_SVC_BC_EXT_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV4V6 || \
   QBI_SVC_BC_EXT_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV4_AND_IPV6)

/*! Returns TRUE if the given IP type includes IPv4 */
#define QBI_SVC_BC_EXT_IPV4_REQUESTED(ip_type) \
   (QBI_SVC_BC_EXT_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV4 || \
    QBI_SVC_BC_EXT_IS_MBIM_DUAL_IP(ip_type))

/*! Returns TRUE if the given IP type includes IPv6 */
#define QBI_SVC_BC_EXT_IPV6_REQUESTED(ip_type) \
   (QBI_SVC_BC_EXT_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV6 || \
    QBI_SVC_BC_EXT_IS_MBIM_DUAL_IP(ip_type))

#define QBI_SVC_EXT_ID_TO_INDEX(svc_id) (svc_id - QBI_SVC_ID_OFFSET)

/*! Map session ID to QMI WDS IPv4 service ID */
#define QBI_SVC_BC_EXT_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id) \
   (session_id < QBI_SVC_BC_EXT_MAX_SESSIONS ? \
    (qbi_qmi_svc_e)(QBI_QMI_SVC_WDS_FIRST + session_id * 2) : \
    (qbi_qmi_svc_e)QBI_QMI_SVC_WDS_FIRST)

/*! Map session ID to QMI WDS IPv6 service ID */
#define QBI_SVC_BC_EXT_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id) \
   (session_id < QBI_SVC_BC_EXT_MAX_SESSIONS ? \
    ((qbi_qmi_svc_e)(QBI_QMI_SVC_WDS_FIRST + session_id * 2 + 1)) : \
    (qbi_qmi_svc_e)(QBI_QMI_SVC_WDS_FIRST + 1))

/*! Map QMI WDS service ID to session ID */
#define QBI_SVC_BC_EXT_WDS_SVC_ID_TO_SESSION_ID(wds_svc_id) \
   ((wds_svc_id >= QBI_QMI_SVC_WDS_FIRST && wds_svc_id <= QBI_QMI_SVC_WDS_LAST) ? \
    (wds_svc_id - QBI_QMI_SVC_WDS_FIRST) / 2 : 0)

/*! @} */

/*! @addtogroup MBIM_CID_MS_DEVICE_CAPS_V2
    @{ */
#define QBI_SVC_BC_EXT_FIRMWARE_INFO_PATH                  "/firmware/image/Ver_Info.txt"

#define QBI_SVC_BC_EXT_DEFAULT_EXECUTOR_INDEX              (0)

#define QBI_SVC_BC_EXT_MODEM_ID_INFO_MAX_LEN_BYTES         (36)
#define QBI_SVC_BC_EXT_MAX_SUBS_INFO_MAX_LEN_BYTES         (4)

#define MEIDCHAR_0 48 /* ascii character '0' */
#define MEIDCHAR_9 57 /* ascii character '9' */
#define MEIDCHAR_A 65 /* ascii character 'A' */
#define MEIDCHAR_F 70 /* ascii character 'F' */

/*! @} */

/*! @addtogroup MBIM_CID_MS_PCO
    @{ */
#define QBI_SVC_BC_EXT_PCO_OPERATOR_RESERVED_FORMAT_IEI                 (0x27)
#define QBI_SVC_BC_EXT_PCO_OPERATOR_RESERVED_FORMAT_SYNTHESIZED_HEADER  (0x80)
#define QBI_SVC_BC_EXT_PCO_DEFAULT_SESSION_ID                           (0)
#define QBI_SVC_BC_EXT_PCO_BUFFER_LENGTH_OFFSET                         (4)
#define QBI_SVC_BC_EXT_PCO_VALUE_OFFSET                                 (2)
/*! @} */

/*! @addtogroup MBIM_CID_MS_BASE_STATIONS_INFO
    @{ */
#define QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE            (0xFFFFFFFF)
#define QBI_SVC_BC_EXT_MCC_MNC_STR_LEN                                  (7)
#define QBI_SVC_BC_EXT_MCC_LEN                                          (3)
#define QBI_SVC_BC_EXT_MNC_LEN                                          (3)
#define QBI_SBC_BC_EXT_BASE_STATION_POWER_CONVERSION_PARAMETER          (10)
#define QBI_SVC_BC_EXT_BASE_STATION_RSSI_MIN                            (-110)
#define QBI_SVC_BC_EXT_BASE_STATION_RSSI_MAX                            (-47)
#define QBI_SVC_BC_EXT_BASE_STATION_GSM_RX_LEVEL_MIN                    (0)
#define QBI_SVC_BC_EXT_BASE_STATION_GSM_RX_LEVEL_MAX                    (63)
#define QBI_SVC_BC_EXT_PLMN_NIBBLE_MASK                                 (0x0F)
/*! @} */

/*! @} */

/*=============================================================================

  Private Typedefs

=============================================================================*/


/*! @addtogroup MBIM_CID_MS_PCO
    @{ */

typedef struct {
  uint32_t app_specific_info_len;
  uint8_t app_specific_info[QMI_WDS_APP_SPECIFIC_INFO_V01];
  uint16_t container_id;
} qbi_svc_bc_ext_pco_req_info_s;


/*! @} */


/*! @addtogroup MBIM_CID_MS_BASE_STATIONS_INFO
    @{ */

typedef struct{
  uint32 system_type;
} qbi_svc_bc_ext_base_station_data_class_info_s;

typedef enum{
  QBI_SVC_BC_EXT_BASE_STATION_RAT_GSM,
  QBI_SVC_BC_EXT_BASE_STATION_RAT_UMTS,
  QBI_SVC_BC_EXT_BASE_STATION_RAT_TDSCDMA,
  QBI_SVC_BC_EXT_BASE_STATION_RAT_LTE,
  QBI_SVC_BC_EXT_BASE_STATION_RAT_CDMA
}qbi_svc_bc_ext_base_station_info_rat_type_e;

/*! @} */

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static void qbi_svc_bc_ext_update_version
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_open_configure_qmi_inds
(
  qbi_txn_s *txn
);

static qbi_svc_bc_ext_module_location_info_cache_s *qbi_svc_bc_ext_location_info_cache_get
(
  qbi_ctx_s *ctx
);
/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_SYS_CAPS
    @{ */

static qbi_svc_action_e qbi_svc_bc_ext_sys_caps_info_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_sys_caps_info_q_dms25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_ext_sys_caps_info_q_get_meid
(
  qbi_qmi_txn_s *qmi_txn
);

static uint32  qbi_svc_bc_ext_sys_caps_info_q_get_meid_char_to_int
(
  char meid_char
);

static qbi_svc_action_e qbi_svc_bc_ext_sys_caps_info_q_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_CAPS_V2
    @{ */

static qbi_svc_action_e qbi_svc_bc_ext_device_caps_v2_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_device_caps_info_q_dms25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_device_caps_q_dms22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_device_caps_q_dms20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static uint32 qbi_svc_bc_ext_open_configure_qmi_radio_if_list_to_mbim_data_class
(
  const dms_radio_if_enum_v01 *radio_if_list,
  uint32                       radio_if_list_len
);

static qbi_svc_action_e qbi_svc_bc_ext_device_caps_q_rsp
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_ext_device_caps_q_build_info_rsp
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_RESET
    @{ */

static qbi_svc_action_e qbi_svc_bc_ext_device_reset_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_device_reset_s_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_PCO
    @{ */

static qbi_svc_action_e qbi_svc_bc_ext_pco_q_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_ext_pco_pre_check
(
  struct qbi_txn_struct *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_pco_prepare_rsp
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_pco_q_wds2d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_pco_wds03_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATION
    @{ */

static qbi_svc_action_e qbi_svc_bc_ext_base_stations_info_q_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_ext_base_station_pre_check
(
  qbi_txn_s *txn
);

static uint32 qbi_svc_bc_ext_base_station_rssi_to_rx_level_conversion
(
  int16_t rssi
);

static uint32 qbi_svc_bc_ext_base_station_get_initial_offset
(
  uint32 initial_offset,
  uint32 count,
  uint32 size
);

static void qbi_svc_bc_ext_base_station_plmn_to_ascii_mcc_mnc
(
  const char *plmn,
  uint32 plmn_len,
  char *mcc_mnc,
  uint32 mcc_mnc_len
);

static qbi_svc_action_e qbi_svc_bc_ext_base_station_gsm_lte_umts_info_nas43_rsp
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_base_station_umts_info_nas43_rsp
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_ext_base_station_umts_serving_cell_info
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_ext_base_station_umts_ngbr_cell_info
(
  qbi_qmi_txn_s *qmi_txn,
  uint32         rat_type
);

static qbi_svc_action_e qbi_svc_bc_ext_base_station_lte_info_nas43_rsp
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_ext_base_station_lte_serving_cell_info
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_ext_base_station_lte_ngbr_cell_info
(
  qbi_qmi_txn_s *qmi_txn,
  uint32         rat_type
);

static qbi_svc_action_e qbi_svc_bc_ext_base_station_gsm_info_nas43_rsp
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_ext_base_station_gsm_serving_cell_info
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_ext_base_station_gsm_ngbr_cell_info
(
  qbi_qmi_txn_s *qmi_txn, 
  uint32         rat_type
);

static qbi_svc_action_e qbi_svc_bc_ext_base_station_cdma_info_q_nas65_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_ext_base_station_cdma_cell_info
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_base_station_tdscdma_info_q_nas6d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_ext_base_station_tdscdma_serving_cell_info
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_ext_base_station_tdscdma_ngbr_cell_info
(
  qbi_qmi_txn_s *qmi_txn
);
/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_LOCATION_INFO_STATUS
    @{ */
static qbi_svc_action_e qbi_svc_bc_ext_location_info_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_location_info_q_nas_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_location_info_nas03_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_bc_ext_update_location_info_cache
(
  qbi_svc_bc_ext_location_info_rsp_s *rsp,
  qbi_svc_bc_ext_module_location_info_cache_s *cache
);
/*! @} */
/*=============================================================================

  Private Variables

=============================================================================*/
/*! @brief Static QMI indication handlers (generally, CID event handlers)
*/
static const qbi_svc_ind_info_s qbi_svc_bc_ext_static_ind_hdlrs[] = {
  QBI_SVC_BC_EXT_STATIC_IND_HDLR(QBI_QMI_SVC_WDS, 
                                QMI_WDS_PROFILE_EVENT_REGISTER_IND_V01,
                                QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
                                qbi_svc_bc_ext_prov_ctx_v2_wdsa8_ind_cb),
  QBI_SVC_BC_EXT_STATIC_IND_HDLR(QBI_QMI_SVC_PDC, 
                                QMI_PDC_ACTIVATE_CONFIG_IND_V01,
                                QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
                                qbi_svc_bc_ext_prov_ctx_v2_s_pdc27_ind_cb),
  QBI_SVC_BC_EXT_STATIC_IND_HDLR(QBI_QMI_SVC_PDC, 
                                QMI_PDC_REFRESH_IND_V01,
                                QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
                                qbi_svc_bc_ext_prov_ctx_pdc2f_ind_cb),
  QBI_SVC_BC_EXT_STATIC_IND_HDLR(QBI_QMI_SVC_WDS, 
                                QMI_WDS_LTE_ATTACH_PDN_LIST_IND_V01,
                                QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG,
                                qbi_svc_bc_ext_lte_attach_config_static_ind_e_wds95_ind_cb),
  QBI_SVC_BC_EXT_STATIC_IND_HDLR(QBI_QMI_SVC_DSD, 
                                QMI_DSD_SYSTEM_STATUS_IND_V01,
                                QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_STATUS,
                                qbi_svc_bc_ext_lte_attach_status_e_dsd26_ind_cb),
  QBI_SVC_BC_EXT_STATIC_IND_HDLR(QBI_QMI_SVC_UIM, 
                                QMI_UIM_SLOT_STATUS_CHANGE_IND_V01,
                                QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO_STATUS,
                                qbi_svc_bc_ext_dssa_slot_info_uim48_ind_cb),
  QBI_SVC_BC_EXT_STATIC_IND_HDLR(QBI_QMI_SVC_WDS,
                                QMI_WDS_APN_OP_RESERVED_PCO_LIST_CHANGE_IND_V01,
                                QBI_SVC_BC_EXT_MBIM_CID_MS_PCO,
                                qbi_svc_bc_ext_pco_wds03_ind_cb),
  QBI_SVC_BC_EXT_STATIC_IND_HDLR(QBI_QMI_SVC_NAS,
                                QMI_NAS_SYS_INFO_IND_MSG_V01,
                                QBI_SVC_BC_EXT_MBIM_CID_MS_LOCATION_INFO_STATUS,
                                qbi_svc_bc_ext_location_info_nas03_ind_cb),
};

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_bc_ext_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_bc_ext_cmd_hdlr_tbl[] = {
  /* MBIM_CID_MS_PROVISIONED_CONTEXT_V2 */
  {qbi_svc_bc_ext_prov_ctx_v2_q_req, 0,
   qbi_svc_bc_ext_prov_ctx_v2_s_req,
   sizeof(qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s)},
  /* MBIM_CID_MS_NETWORK_BLACKLIST */
  { NULL, 0, NULL, 0},
  /* MBIM_CID_MS_LTE_ATTACH_CONFIG */
  {qbi_svc_bc_ext_lte_attach_config_q_req, 0, 
  qbi_svc_bc_ext_lte_attach_config_s_req, 
  sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s)},
  /* MBIM_CID_MS_LTE_ATTACH_STATUS */
  { qbi_svc_bc_ext_lte_attach_status_q_req, 0, NULL, 0 },
  /* MBIM_CID_MS_SYS_CAPS */
  { qbi_svc_bc_ext_sys_caps_info_q_req, 0, NULL, 0 },
  /* MBIM_CID_MS_DEVICE_CAPS_V2 */
  { qbi_svc_bc_ext_device_caps_v2_q_req, 0, NULL, 0 },
  /* MBIM_CID_MS_DEVICE_SLOT_MAPPING */
  { qbi_svc_bc_ext_dssa_slot_mapping_q_req, 0, 
  qbi_svc_bc_ext_dssa_slot_mapping_s_req,
  sizeof (qbi_svc_bc_ext_slot_mapping_info_s) },
  /* MBIM_CID_MS_SLOT_INFO_STATUS */
  { qbi_svc_bc_ext_dssa_slot_info_status_q_req, 0, NULL, 0 },
  /* MBIM_CID_PCO */
  { qbi_svc_bc_ext_pco_q_req, 0, NULL, 0},
  /* MBIM_CID_MS_DEVICE_RESET */
  { NULL, 0, qbi_svc_bc_ext_device_reset_s_req, 0 },
  /* MBIM_CID_MS_BASE_STATION*/
  { qbi_svc_bc_ext_base_stations_info_q_req,
    sizeof(qbi_svc_bc_ext_base_station_info_req_s), NULL, 0},
  /* MBIM_CID_MS_LOCATION_INFO_STATUS*/
  { qbi_svc_bc_ext_location_info_q_req, 0, NULL, 0}
};


/*! @} */

/*=============================================================================

Private Function Definitions

=============================================================================*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_open_configure_qmi_inds
===========================================================================*/
/*!
    @brief Configures QMI indications

    @details
    Other QMI indications will be registered on-demand depending on CID filter
    status (see qbi_svc_bc_qmi_reg_tbl).

    @param txn

    @return boolean TRUE on success, FALSE otherwise
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_open_configure_qmi_inds
(
  qbi_txn_s *txn
)
{
  wds_indication_register_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (wds_indication_register_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_INDICATION_REGISTER_REQ_V01,
      qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_wdsa8_req);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->report_lte_attach_pdn_list_change_valid = TRUE;
  qmi_req->report_lte_attach_pdn_list_change = TRUE;

  qmi_req->report_profile_changed_events_valid = TRUE;
  qmi_req->report_profile_changed_events = TRUE;

  qmi_req->report_apn_op_reserved_pco_list_valid = TRUE;
  qmi_req->report_apn_op_reserved_pco_list.report_op_reserved_pco_list_change = TRUE;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_open_configure_qmi_inds() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity Extension device
    service's cache

    @details

    @param ctx

    @return qbi_svc_bc_ext_module_location_info_cache_s
*/
/*=========================================================================*/
qbi_svc_bc_ext_cache_s *qbi_svc_bc_ext_cache_get
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_bc_ext_cache_s *cache = NULL;
  qbi_svc_bc_ext_module_prov_cache_s prov_cache[QMI_WDS_PROFILE_LIST_MAX_V01] = { 0, };
  uint32 prov_cache_len = sizeof(qbi_svc_bc_ext_module_prov_cache_s)
    * QMI_WDS_PROFILE_LIST_MAX_V01;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(ctx);

  cache = (qbi_svc_bc_ext_cache_s *)qbi_svc_cache_get(ctx, QBI_SVC_ID_BC_EXT);

  if (cache == NULL)
  {
    cache = qbi_svc_cache_alloc(ctx, QBI_SVC_ID_BC_EXT,
      sizeof(qbi_svc_bc_ext_cache_s));

    if (cache == NULL)
    {
      QBI_LOG_E_0("Couldn't allocate cache!");
      return NULL;
    }

    if (qbi_nv_store_cfg_item_read(ctx,
      QBI_NV_STORE_CFG_ITEM_PROVISION_CONTEXT_PROFILE_DATA,
      prov_cache, prov_cache_len))
    {
      QBI_LOG_D_0("Read profile data");
      QBI_MEMSCPY(cache->prov_cache, prov_cache_len,
        prov_cache, prov_cache_len);
    }
  }

  return cache;
} /* qbi_svc_bc_ext_cache_get */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_module_prov_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity Extension device
    service's cache

    @details

    @param ctx

    @return qbi_svc_bc_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
qbi_svc_bc_ext_module_prov_cache_s *qbi_svc_bc_ext_module_prov_cache_get
(
  qbi_ctx_s *ctx,
  const uint32 cache_index
)
{
  qbi_svc_bc_ext_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(ctx);

  cache = qbi_svc_bc_ext_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(cache);

  return &cache->prov_cache[cache_index];
} /* qbi_svc_bc_ext_module_prov_cache_get */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_module_prov_cache_clear
===========================================================================*/
/*!
    @brief Clears the ctxt cache

    @details

    @param cache

    @return
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_module_prov_cache_clear
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_cache_s *cache = NULL;
  qbi_svc_bc_ext_module_prov_cache_s prov_cache[QMI_WDS_PROFILE_LIST_MAX_V01] = { 0, };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_ext_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  // clear cache
  QBI_MEMSET(cache, 0,
    sizeof(qbi_svc_bc_ext_module_prov_cache_s) * QMI_WDS_PROFILE_LIST_MAX_V01);

  if (qbi_nv_store_cfg_item_write(
    txn->ctx, QBI_NV_STORE_CFG_ITEM_PROVISION_CONTEXT_PROFILE_DATA,
    prov_cache, sizeof(qbi_svc_bc_ext_module_prov_cache_s)*
    QMI_WDS_PROFILE_LIST_MAX_V01))
  {
    QBI_LOG_D_0("Successfully cleared the persistent provisioning cache");
  }
  else
  {
    QBI_LOG_D_0("Failed to clear persistent provisioning cache");
  }

  return QBI_SVC_ACTION_NULL;
}/*qbi_svc_bc_ext_module_prov_cache_clear*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_location_info_cache_get
===========================================================================*/
/*!
    @brief Get location info cache

    @details

    @param ctx

    @return qbi_svc_bc_ext_module_location_info_cache_s
*/
/*=========================================================================*/
static qbi_svc_bc_ext_module_location_info_cache_s *qbi_svc_bc_ext_location_info_cache_get
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_bc_ext_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(ctx);

  cache = qbi_svc_bc_ext_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(cache);

  return &cache->location_info_cache;
} /* qbi_svc_bc_ext_location_info_cache_get */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_update_cache_ip_type
===========================================================================*/
/*!
    @brief Converts Modem PDP Type (WDS) to QBI/MBIM Class defined IP Type.
       Comman API used by both LTE Attach Config and Provisoned
       Context V2.

    @details

    @param txn
    @param operation
    @param cache
    @param ip_type
    @param pdp_type

    @return uint32 MBIM_COMPRESSION value
*/
/*=========================================================================*/
boolean qbi_svc_bc_ext_update_cache_ip_type
(
  qbi_txn_s *txn,
  const uint32 operation,
  qbi_svc_bc_ext_module_prov_cache_s *cache,
  const uint32 ip_type,
  const wds_pdp_type_enum_v01 pdp_type
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  switch (pdp_type)
  {
  case WDS_PDP_TYPE_PDP_IPV4_V01:
    cache->ip_type = QBI_SVC_BC_IP_TYPE_IPV4;
    break;
  case WDS_PDP_TYPE_PDP_IPV6_V01:
    cache->ip_type = QBI_SVC_BC_IP_TYPE_IPV6;
    break;
  case WDS_PDP_TYPE_PDP_IPV4V6_V01:
    if (txn->cmd_type == QBI_MSG_CMD_TYPE_SET ||
        (txn->cmd_type == QBI_TXN_CMD_TYPE_INTERNAL &&
         txn->req.data))
    {
      switch (operation)
      {
      case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_OPERATION_DEFAULT:
        cache->ip_type = ip_type == QBI_SVC_BC_IP_TYPE_DEFAULT ?
            QBI_SVC_BC_IP_TYPE_DEFAULT : QBI_SVC_BC_IP_TYPE_IPV4V6;
        break;
      case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_OPERATION_RESTORE_FACTORY:
      case QBI_SVC_MBIM_MS_CONTEXT_OPERATION_RESTORE_FACTORY:
        cache->ip_type = QBI_SVC_BC_IP_TYPE_IPV4V6;
        break;
      }
    }
    else
    {
      /* This is when Query is triggered first time before any Set */
      if (cache->prov_active || cache->lte_active)
      {
        cache->ip_type = cache->ip_type == QBI_SVC_BC_IP_TYPE_DEFAULT ?
            QBI_SVC_BC_IP_TYPE_DEFAULT : QBI_SVC_BC_IP_TYPE_IPV4V6;
      }
      else
      {
        cache->ip_type = pdp_type;
      }
    }
    break;
  default:
    QBI_LOG_E_1("Cache update:E: Unknown IP type: %d",
                pdp_type);
    return FALSE;
  }

  return TRUE;
} /* qbi_svc_bc_ext_update_cache_ip_type() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_update_nv_store
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity Extension device
    service's cache

    @details

    @param ctx

    @return qbi_svc_bc_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
void qbi_svc_bc_ext_update_nv_store
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_bc_ext_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  cache = (qbi_svc_bc_ext_cache_s *)qbi_svc_cache_get(ctx, QBI_SVC_ID_BC_EXT);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (!qbi_nv_store_cfg_item_write(
          ctx, QBI_NV_STORE_CFG_ITEM_PROVISION_CONTEXT_PROFILE_DATA,
          cache, sizeof(qbi_svc_bc_ext_module_prov_cache_s) * QMI_WDS_PROFILE_LIST_MAX_V01))
  {
    QBI_LOG_E_0("Couldn't save profile data to NV!");
  }
} /* qbi_svc_bc_ext_update_nv_store() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_match_pdp_type
===========================================================================*/
/*!
    @brief Compares IP Type of requested profile with that of modem
       Comman API used by both LTE Attach Config and Provisoned
       Context V2

    @details

    @param txn
    @param qmi_rsp

    @return TRUE/FALSE
*/
/*=========================================================================*/
boolean qbi_svc_bc_ext_match_pdp_type
(
  uint32  ip_type,
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);

  if (qmi_rsp->pdp_type_valid)
  {
    switch (ip_type)
    {
    case QBI_SVC_BC_IP_TYPE_IPV4:
      success = (qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_IPV4_V01 ||
                 qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_IPV4V6_V01);
      break;
    case QBI_SVC_BC_IP_TYPE_IPV6:
      success = (qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_IPV6_V01 ||
                 qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_IPV4V6_V01);
      break;
    case QBI_SVC_BC_IP_TYPE_IPV4V6:
    case QBI_SVC_BC_IP_TYPE_DEFAULT:
      success = (qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_IPV4_V01 ||
                 qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_IPV6_V01 ||
                 qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_IPV4V6_V01);
      break;
    }
  }

  QBI_LOG_D_3("Matching pdp_type(%d) <==> ip_type(%d), "
              "status %d", qmi_rsp->pdp_type, ip_type, success);
  return success;
} /* qbi_svc_bc_ext_match_pdp_type() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_ip_type_to_pdp_type
===========================================================================*/
/*!
    @brief Translates MBIM IP type to QMI PDP type

  @details

  @param ip_type

  @return wds_pdp_type_enum_v01
*/
/*=========================================================================*/
wds_pdp_type_enum_v01 qbi_svc_bc_ext_ip_type_to_pdp_type
(
  uint32 ip_type
)
{
/*-------------------------------------------------------------------------*/
  switch (ip_type)
  {
  case QBI_SVC_BC_IP_TYPE_IPV4:
    return WDS_PDP_TYPE_PDP_IPV4_V01;
  case QBI_SVC_BC_IP_TYPE_IPV6:
    return WDS_PDP_TYPE_PDP_IPV6_V01;
  case QBI_SVC_BC_IP_TYPE_IPV4V6:
  case QBI_SVC_BC_IP_TYPE_DEFAULT:
  default:
    return WDS_PDP_TYPE_PDP_IPV4V6_V01;
  }
} /* qbi_svc_bc_ext_ip_type_to_pdp_type */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_connect_mbim_auth_pref_to_qmi_auth_pref
===========================================================================*/
/*!
  @brief Converts MBIM_CID_AUTH_PROTOCOL to QMI authentication preference
  (3GPP only)

  @details

  @param auth_protocol

  @return uint8
*/
/*=========================================================================*/
wds_auth_pref_mask_v01 qbi_svc_bc_ext_connect_mbim_auth_pref_to_qmi_auth_pref
(
  uint32 auth_protocol
)
{
  wds_auth_pref_mask_v01 qmi_auth_pref;
/*-------------------------------------------------------------------------*/
  switch (auth_protocol)
  {
  case QBI_SVC_BC_AUTH_PROTOCOL_NONE:
    qmi_auth_pref = 0;
    break;

  case QBI_SVC_BC_AUTH_PROTOCOL_PAP:
    qmi_auth_pref = QMI_WDS_MASK_AUTH_PREF_PAP_V01;
    break;

  case QBI_SVC_BC_AUTH_PROTOCOL_CHAP:
  case QBI_SVC_BC_AUTH_PROTOCOL_MSCHAP_V2:
    qmi_auth_pref = QMI_WDS_MASK_AUTH_PREF_CHAP_V01;
    break;

  case QBI_SVC_BC_AUTH_PROTOCOL_AUTO:
    qmi_auth_pref = WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01;
    break;

  default:
    QBI_LOG_E_1("Unrecognized AuthentiticationProtocol %d - using PAP or "
                "CHAP", auth_protocol);
    /* May try PAP or CHAP */
    qmi_auth_pref = (QMI_WDS_MASK_AUTH_PREF_PAP_V01 |
                     QMI_WDS_MASK_AUTH_PREF_CHAP_V01);
  }

  return qmi_auth_pref;
} /* qbi_svc_bc_ext_connect_mbim_auth_pref_to_qmi_auth_pref() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_update_version
===========================================================================*/
/*!
    @brief Update QBI version if required

    @details
    Checks if QBI version is updated , if not creates version file

    @param txn

    @return void
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_update_version
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_qbi_version_s qbi_version = { 0 };
  uint32 status;
/*-------------------------------------------------------------------------*/
  if (!qbi_nv_store_cfg_item_read(
    txn->ctx, QBI_NV_STORE_CFG_ITEM_QBI_VERSION,
    &qbi_version, sizeof(qbi_svc_bc_qbi_version_s)))
  {
    QBI_LOG_D_0("Unable to read build version ! Creating it");

    qbi_version.version = QBI_VERSION;
    if (qbi_nv_store_cfg_item_write(
      txn->ctx, QBI_NV_STORE_CFG_ITEM_QBI_VERSION,
      &qbi_version, sizeof(qbi_svc_bc_qbi_version_s)))
    {
      QBI_LOG_D_0("Success : Version Written !!");
    }
    else
    {
      QBI_LOG_D_0("Failed : Version Written !!");
    }

    //Avoid compatibility issues with older QBI versions deleting context cache
    status = qbi_nv_store_cfg_item_delete(txn->ctx,
      QBI_NV_STORE_CFG_ITEM_PROVISION_CONTEXT_PROFILE_DATA);
    if (status == 0)
    {
      QBI_LOG_D_0("context_profile_data deleted successfully");
    }
  }
  else
  {
      QBI_LOG_D_1("Current QBI version is %d",qbi_version.version);
  }
}/* qbi_svc_bc_ext_update_version() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_open
===========================================================================*/
/*!
    @brief Runtime intiailization of the Basic Connectivity Extension service

    @details
    This is invoked per-context when the device receives a MBIM_OPEN_MSG.
    It is in charge of performing all runtime initialization so that the
    service can be operational.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_I_0("Processing Basic Extension Connectivity device service open...");
  if (!qbi_qmi_alloc_svc_handle(
    txn->ctx, QBI_SVC_BC_EXT_SESSION_ID_TO_WDS_SVC_ID_IPV4(0)) ||
    !qbi_qmi_alloc_svc_handle(
      txn->ctx, QBI_SVC_BC_EXT_SESSION_ID_TO_WDS_SVC_ID_IPV6(0)) ||
      !qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_DSD) ||
      !qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_PDC) ||
      !qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_UIM))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else if (!qbi_svc_ind_reg_static(
    txn->ctx, QBI_SVC_ID_BC_EXT, qbi_svc_bc_ext_static_ind_hdlrs,
    ARR_SIZE(qbi_svc_bc_ext_static_ind_hdlrs)))
  {
    QBI_LOG_E_0("Couldn't register QMI indication handlers");
  }
  else
  {
    /* Create qbi version file if not present and blow away the cache
    to avoid compatibility issues with older QBI versions */
    qbi_svc_bc_ext_update_version(txn);
    action = qbi_svc_bc_ext_open_configure_qmi_inds(txn);
  }

  return action;
} /* qbi_svc_bc_ext_open() */

/*! @addtogroup MBIM_CID_DEVICE_CAPS
    @{ */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_open_configure_qmi_radio_if_list_to_mbim_data_class
===========================================================================*/
/*!
    @brief Convert QMI radio interface array to MBIM data class mask

    @details

    @param dms_radio_if_enum_v01
    @param uint32

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_ext_open_configure_qmi_radio_if_list_to_mbim_data_class
(
    const dms_radio_if_enum_v01 *radio_if_list,
    uint32                       radio_if_list_len
)
{
  uint32 i;
  uint32 data_class = QBI_SVC_BC_DATA_CLASS_NONE;
/*-------------------------------------------------------------------------*/
  /* Construct data class according to available technologies */
  if (radio_if_list_len > QMI_DMS_RADIO_IF_LIST_MAX_V01)
  {
      QBI_LOG_E_1("Invalid radio IF list length %d", radio_if_list_len);
  }
  else
  {
      for (i = 0; i < radio_if_list_len; i++)
      {
          switch (radio_if_list[i])
          {
          case DMS_RADIO_IF_1X_V01:
              data_class |= QBI_SVC_BC_DATA_CLASS_1XRTT;
              break;

          case DMS_RADIO_IF_1X_EVDO_V01:
              data_class |= (QBI_SVC_BC_DATA_CLASS_1XRTT |
                QBI_SVC_BC_DATA_CLASS_1XEVDO |
                  QBI_SVC_BC_DATA_CLASS_1XEVDO_REVA |
                  QBI_SVC_BC_DATA_CLASS_1XEVDO_REVB);
              break;

          case DMS_RADIO_IF_GSM_V01:
              data_class |= (QBI_SVC_BC_DATA_CLASS_GPRS |
                  QBI_SVC_BC_DATA_CLASS_EDGE);
              break;

          case DMS_RADIO_IF_UMTS_V01:
              data_class |= (QBI_SVC_BC_DATA_CLASS_UMTS |
                  QBI_SVC_BC_DATA_CLASS_HSDPA |
                  QBI_SVC_BC_DATA_CLASS_HSUPA);
              break;

          case DMS_RADIO_IF_LTE_V01:
              data_class |= QBI_SVC_BC_DATA_CLASS_LTE;
              break;

          case DMS_RADIO_IF_TDS_V01:
              data_class |= QBI_SVC_BC_DATA_CLASS_CUSTOM;
              break;

          default:
              QBI_LOG_E_1("Couldn't identify data class %d!",
                  radio_if_list[i]);
              break;
          }
      }
  }

  return data_class;
} /* qbi_svc_bc_ext_open_configure_qmi_radio_if_list_to_mbim_data_class() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_sys_caps_info_q_uim47_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_SYS_CAPS response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_sys_caps_info_q_uim47_rsp_cb
(
    qbi_qmi_txn_s *qmi_txn
)
{
  uim_get_slots_status_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_ext_sys_caps_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = (qbi_svc_bc_ext_sys_caps_rsp_s *)qmi_txn->parent->rsp.data;
  qmi_rsp = (uim_get_slots_status_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
      QBI_LOG_E_1("received error code %d from qmi", qmi_rsp->resp.error);
  }
  else
  {
    if (!qmi_rsp->physical_slot_status_valid)
      {
          QBI_LOG_E_0("leaving deviceid field blank: number of slots not provisioned");
      }
      else
      {
      rsp->num_of_slots = qmi_rsp->physical_slot_status_len;
          QBI_LOG_D_1("num_of_slots %d", rsp->num_of_slots);
      }

      rsp->concurrency = QBI_SVC_BC_EXT_DSSA_CONCURRENCY;
      rsp->num_of_executors = QBI_SVC_BC_EXT_DSSA_MAX_SUPPORTED_EXECUTORS;
      QBI_LOG_D_2("concurrency %d num_of_executors %d", rsp->concurrency, rsp->num_of_executors);
      action = qbi_svc_bc_ext_device_caps_q_rsp(qmi_txn->parent);
  }

  return action;
}/* qbi_svc_bc_ext_sys_caps_info_q_uim47_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_sys_caps_info_q_dms25_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_SYS_CAPS response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_sys_caps_info_q_dms25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  dms_get_device_serial_numbers_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("qbi_svc_bc_ext_sys_caps_info_q_dms25_rsp_cb");

  qmi_rsp = (dms_get_device_serial_numbers_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
      QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
      if (!qmi_rsp->meid_valid)
      {
          QBI_LOG_D_0("Leaving DeviceId field blank: invalid modemid");
      }
      else
      {
        qbi_svc_bc_ext_sys_caps_info_q_get_meid(qmi_txn);
      }
      action = qbi_svc_bc_ext_device_caps_q_rsp(qmi_txn->parent);

  }

  return action;
}/* qbi_svc_bc_ext_sys_caps_info_q_dms25_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_sys_caps_info_q_get_meid
===========================================================================*/
/*!
    @brief Handles a MEID extraction

    @details

    @param qmi_txn

    @return none
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_sys_caps_info_q_get_meid
(
  qbi_qmi_txn_s *qmi_txn
)
{
  dms_get_device_serial_numbers_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_ext_sys_caps_rsp_s *rsp;
  int i = 0;
  uint32_t temp1 = 0;
  uint32_t temp2 = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);

  rsp = (qbi_svc_bc_ext_sys_caps_rsp_s *)qmi_txn->parent->rsp.data;
  qmi_rsp = (dms_get_device_serial_numbers_resp_msg_v01 *)qmi_txn->rsp.data;

  /* An MEID is 56 bits long (14 hex digits) */
  /* Copying 1st hex digit into variable temp1 */
  temp1 =  qbi_svc_bc_ext_sys_caps_info_q_get_meid_char_to_int (qmi_rsp->meid[i]);
  /* Copying rest five hex digits into variable temp1, so repeating loop from
     1 to 5 hex digits */
  for (i = 1; i < 6;i++)
  {
    temp1 = temp1 << 4;
    temp1 |=  qbi_svc_bc_ext_sys_caps_info_q_get_meid_char_to_int (qmi_rsp->meid[i]);
  }

  /* Copying 6th hex digit into variable temp2 */
  temp2 =  qbi_svc_bc_ext_sys_caps_info_q_get_meid_char_to_int (qmi_rsp->meid[i]);
  /* Copying rest seven hex digits into variable temp2, so repeating loop from
     7 to 13 hex digits */
  for (i = 7; i < 14;i++)
  {
    temp2 = temp2 << 4;
    temp2 |=  qbi_svc_bc_ext_sys_caps_info_q_get_meid_char_to_int (qmi_rsp->meid[i]);
  }

  /* Shifting temp1 6 hex values to MSB of rsp->modem_id */
  rsp->modem_id = (0x00000000FFFFFFFFull & temp1) << 32;
  /* Inserting temp2 into LSB of rsp->modem_id */
  rsp->modem_id = rsp->modem_id | temp2;

} /* qbi_svc_bc_ext_sys_caps_info_q_get_meid */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_sys_caps_info_q_get_meid_char_to_int
===========================================================================*/
/*!
    @brief Handles a MEID extraction

    @details

    @param qmi_txn

    @return none
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_ext_sys_caps_info_q_get_meid_char_to_int
(
  char meid_char
)
{
  uint32_t meid_int = 0;
/*-------------------------------------------------------------------------*/
  if ((meid_char >= MEIDCHAR_0) && (meid_char <= MEIDCHAR_9))
  {
   /* characters 0 to 9 conversion to integer*/
    meid_int |= (meid_char - MEIDCHAR_0);
  }
  else if ((meid_char >= MEIDCHAR_A) && (meid_char <= MEIDCHAR_F))
  {
    /* characters A to F conversion to integer*/
    meid_int |= (meid_char - (MEIDCHAR_A - 10));
  }
  return meid_int;
}/* qbi_svc_bc_ext_sys_caps_info_q_get_meid_char_to_int */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_sys_caps_info_q_req
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_SYS_CAPS query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_sys_caps_info_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn = NULL;
  uim_get_slots_status_req_msg_v01 *qmi_req = NULL;
  qbi_svc_bc_ext_sys_caps_rsp_s *rsp = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;

/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  qbi_txn_req_buf_ignored(txn);

  rsp = qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_ext_sys_caps_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  /* QMI_DMS_GET_DEVICE_SERIAL_NUMBERS (0x25) */
  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_DMS,
        QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_REQ_V01,
        qbi_svc_bc_ext_sys_caps_info_q_dms25_rsp_cb);

  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  /*QMI_UIM_GET_SLOTS_STATUS (0x47)*/
  qmi_req = (uim_get_slots_status_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
          txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_SLOTS_STATUS_REQ_V01,
          qbi_svc_bc_ext_sys_caps_info_q_uim47_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  action = QBI_SVC_ACTION_SEND_QMI_REQ;

  return action;
}/* qbi_svc_bc_ext_sys_caps_info_q_req() */

/*! @} */

/*! @addtogroup MBIM_CID_DEVICE_CAPS_V2
    @{ */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_device_caps_v2_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_DEVICE_CAPS_V2 query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_device_caps_v2_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_device_caps_v2_rsp_s *rsp;
  qbi_qmi_txn_s *qmi_txn = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  qbi_txn_req_buf_ignored(txn);

  rsp = qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_ext_device_caps_v2_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->voice_class = QBI_SVC_BC_VOICE_CLASS_NO_VOICE;
  rsp->sms_caps = (QBI_SVC_BC_SMS_CAPS_PDU_SEND |
      QBI_SVC_BC_SMS_CAPS_PDU_RECEIVE);
  rsp->sim_class = QBI_SVC_BC_SIM_CLASS_REMOVABLE;
  rsp->max_sessions = QBI_SVC_BC_EXT_MAX_SESSIONS;
  rsp->cellular_class = QBI_SVC_BC_CELLULAR_CLASS_GSM;
  rsp->ctrl_caps = QBI_SVC_BC_CTRL_CAPS_REG_MANUAL;

  /* DeviceType given from NV configuration item */
  if (!qbi_nv_store_cfg_item_read(
      txn->ctx, QBI_NV_STORE_CFG_ITEM_DEVICE_TYPE, (void *) &rsp->device_type,
      sizeof(rsp->device_type)))
  {
    rsp->device_type = QBI_SVC_BC_DEVICE_TYPE_UNKNOWN;
  }
  
  if (qbi_svc_bc_device_supports_3gpp2(txn->ctx))
  {
    rsp->cellular_class |= QBI_SVC_BC_CELLULAR_CLASS_CDMA;
    rsp->ctrl_caps |= (QBI_SVC_BC_CTRL_CAPS_CDMA_SIMPLE_IP |
        QBI_SVC_BC_CTRL_CAPS_CDMA_MOBILE_IP);
  }

  /* Updating firmware meta build info*/
  if (!qbi_svc_bc_ext_device_caps_q_build_info_rsp(txn))
  {
    QBI_LOG_E_0("Couldn't update the firmware build info");
  }

  /* QMI_DMS_GET_DEVICE_SERIAL_NUMBERS (0x25) */
  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_DMS,
      QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_REQ_V01,
      qbi_svc_bc_ext_device_caps_info_q_dms25_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  /* QMI_DMS_GET_DEVICE_MODEL_ID (0x22) */
  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_DMS,
      QMI_DMS_GET_DEVICE_MODEL_ID_REQ_V01,
      qbi_svc_bc_ext_device_caps_q_dms22_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  /* QMI_DMS_GET_GET_DEVICE_CAP (0x20) */
  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_DMS,
      QMI_DMS_GET_DEVICE_CAP_REQ_V01,
      qbi_svc_bc_ext_device_caps_q_dms20_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  action = QBI_SVC_ACTION_SEND_QMI_REQ;

  return action;
} /* qbi_svc_bc_ext_device_caps_v2_q_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_device_caps_q_rsp
===========================================================================*/
/*!
    @brief Perform query response processing for MBIM_CID_DEVICE_CAPS

    @details
    Checks whether we have the information required to send the response.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_device_caps_q_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (txn->qmi_txns_pending == 0)
  {
      action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_ext_device_caps_q_rsp() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_device_caps_q_build_info_rsp
===========================================================================*/
/*!
    @brief  Populating firmware meta build info in the fw_info field in
           response strucuture

    @details

    @param  qbi_txn_s

    @return  boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_device_caps_q_build_info_rsp
(
  qbi_txn_s *txn
)
{
  FILE     *firmware_info_fp;
  size_t    buf_len = 0;
  char     *metabuild_info_buffer = NULL;
  char     *metabuild_info_ptr = NULL;
  char      target_sub_str[] = "\"Meta_Build_ID\": \"";
  char      fw_build_id[QBI_SVC_BC_FW_INFO_MAX_LEN_BYTES] = {0, };
  boolean   success = FALSE;
  int       index = 0;
  qbi_svc_bc_ext_device_caps_v2_rsp_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(txn);
  QBI_CHECK_NULL_PTR_RET_ZERO(txn->rsp.data);

  rsp = (qbi_svc_bc_ext_device_caps_v2_rsp_s *)txn->rsp.data;
  QBI_CHECK_NULL_PTR_RET_ZERO(rsp);

  QBI_LOG_I_0("Entered firmware info response function");
  firmware_info_fp = fopen(QBI_SVC_BC_EXT_FIRMWARE_INFO_PATH, "r");
  if (NULL == firmware_info_fp)
  {
    QBI_LOG_E_1("Could not open file %s",QBI_SVC_BC_EXT_FIRMWARE_INFO_PATH);
  }
  else
  {
    while (getline(&metabuild_info_buffer, &buf_len, firmware_info_fp) != -1)
    {
      if (metabuild_info_ptr = strstr(metabuild_info_buffer, target_sub_str))
      {
        metabuild_info_ptr += strlen(target_sub_str);
        while (*metabuild_info_ptr != '\"')
        {
          /*Copying meta build info into local array*/
          fw_build_id[index++] = *metabuild_info_ptr;
          metabuild_info_ptr++;
        }

        if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          txn, &rsp->fw_info, 0, QBI_SVC_BC_FW_INFO_MAX_LEN_BYTES,
          fw_build_id, sizeof(fw_build_id)))
        {
          QBI_LOG_E_0("Couldn't add firmware information to response!");
        }
        success = TRUE;
        break;
      }
    }
    fclose(firmware_info_fp);
    if (metabuild_info_buffer)
    {
      QBI_MEM_FREE(metabuild_info_buffer);
    }
  }

  return success;
}/*qbi_svc_bc_ext_device_caps_q_build_info_rsp */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_device_caps_q_dms20_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_DMS_GET_DEVICE_CAPS_RESP for
    MBIM_CID_DEVICE_CAPS query

    @details
    Populates the executor_index field in the response.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_device_caps_q_dms20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  dms_get_device_cap_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_ext_device_caps_v2_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("qbi_svc_bc_ext_device_caps_q_dms20_rsp_cb");
  rsp = (qbi_svc_bc_ext_device_caps_v2_rsp_s *)qmi_txn->parent->rsp.data;
  qmi_rsp = (dms_get_device_cap_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
            QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    if (!qmi_rsp->max_active_data_subscriptions_valid)
    {
            QBI_LOG_E_0("Leaving DeviceId field blank: invalid executor_index");
    }
    else
    {
     //Implementation currently supports Single SIM and DSSA hence using default executor index
      rsp->executor_index = QBI_SVC_BC_EXT_DEFAULT_EXECUTOR_INDEX;
      QBI_LOG_D_1("executor_index %d", rsp->executor_index);
     }

     rsp->data_class = qbi_svc_bc_ext_open_configure_qmi_radio_if_list_to_mbim_data_class(
                       qmi_rsp->device_capabilities.radio_if_list,
                       qmi_rsp->device_capabilities.radio_if_list_len);
     if ((rsp->data_class & QBI_SVC_BC_DATA_CLASS_CUSTOM) != 0 &&
         !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
         qmi_txn->parent, &rsp->cust_data_class, 0, 
         QBI_SVC_BC_CUST_DATA_CLASS_MAX_LEN_BYTES,
         QBI_SVC_BC_CUSTOM_DATA_CLASS_NAME_ASCII,
         sizeof(QBI_SVC_BC_CUSTOM_DATA_CLASS_NAME_ASCII)))
     {
       QBI_LOG_E_0("Couldn't add CustomDataClass string to response");
     }
     action = qbi_svc_bc_ext_device_caps_q_rsp(qmi_txn->parent);
  }
  return action;
} /* qbi_svc_bc_ext_device_caps_q_dms20_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_device_caps_q_dms22_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_DMS_GET_DEVICE_MODEL_ID_RESP for
    MBIM_CID_DEVICE_CAPS query

    @details
    Populates the HardwareInfo field in the response.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_device_caps_q_dms22_rsp_cb
(
    qbi_qmi_txn_s *qmi_txn
)
{
  dms_get_device_model_id_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_ext_device_caps_v2_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("qbi_svc_bc_ext_device_caps_q_dms22_rsp_cb");
  rsp = (qbi_svc_bc_ext_device_caps_v2_rsp_s *)qmi_txn->parent->rsp.data;
  qmi_rsp = (dms_get_device_model_id_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
      QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
      qmi_txn->parent, &rsp->hw_info, 0, QBI_SVC_BC_HW_INFO_MAX_LEN_BYTES,
      qmi_rsp->device_model_id, sizeof(qmi_rsp->device_model_id)))
    {
      QBI_LOG_E_0("Couldn't add hardware information to response!");
    }
    else
    {
      action = qbi_svc_bc_ext_device_caps_q_rsp(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_ext_device_caps_q_dms22_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_device_caps_info_q_dms25_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_CAPS response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_device_caps_info_q_dms25_rsp_cb
(
    qbi_qmi_txn_s *qmi_txn
)
{
  dms_get_device_serial_numbers_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_ext_device_caps_v2_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("qbi_svc_bc_ext_device_caps_info_q_dms25_rsp_cb");
  rsp = (qbi_svc_bc_ext_device_caps_v2_rsp_s *)qmi_txn->parent->rsp.data;
  qmi_rsp = (dms_get_device_serial_numbers_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
      QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (qmi_rsp->imei_valid &&
      !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          qmi_txn->parent, &rsp->device_id, 0,
          QBI_SVC_BC_EXT_MODEM_ID_INFO_MAX_LEN_BYTES,
          qmi_rsp->imei, sizeof(qmi_rsp->imei)))
  {
      QBI_LOG_E_0("Couldn't populate device id in response!");
  }
  else
  {
      if (!qmi_rsp->imei_valid)
      {
          QBI_LOG_E_0("Leaving DeviceId field blank: invalid device id");
      }
      action = qbi_svc_bc_ext_device_caps_q_rsp(qmi_txn->parent);
  }

  return action;
}/* qbi_svc_bc_ext_device_caps_info_q_dms25_rsp_cb() */

/*! @} */
 
/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_RESET
    @{ */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_device_reset_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_DEVICE_RESET set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_device_reset_s_req
(
  qbi_txn_s *txn
)
{
  dms_set_operating_mode_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Device is Resetting");
  qmi_req = (dms_set_operating_mode_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_DMS, QMI_DMS_SET_OPERATING_MODE_REQ_V01,
      qbi_svc_bc_ext_device_reset_s_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->operating_mode = DMS_OP_MODE_RESETTING_V01;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
}/* qbi_svc_bc_ext_device_reset_s_req */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_device_reset_s_rsp_cb
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_DEVICE_RESET set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_device_reset_s_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  dms_set_operating_mode_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (dms_set_operating_mode_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
  {
    QBI_LOG_W_1("Received error code %d from QMI", qmi_rsp->resp.error);
    return QBI_SVC_ACTION_ABORT;
  }

  return QBI_SVC_ACTION_SEND_RSP;
}/* qbi_svc_bc_ext_device_reset_s_rsp_cb */

/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_PCO
    @{ */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_pco_q_req
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_PCO query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_pco_q_req
(
  qbi_txn_s *txn
)
{
  wds_get_runtime_settings_req_msg_v01 *qmi_req = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_qmi_svc_e wds_svc_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_LOG_I_0(" Received Query Request for PCO Info");

  if (!qbi_svc_bc_ext_pco_pre_check(txn))
  {
     QBI_LOG_E_1("PCO::E: Error state %d ", txn->status);
  }
  else
  {
     /*Current implementation requires the WDS client ID with IPv4  or v6 connected
       corresponding to session id zero*/
     wds_svc_id = qbi_svc_bc_connect_get_first_connected_svc_id(txn->ctx);

     qmi_req = (wds_get_runtime_settings_req_msg_v01 *)
     qbi_qmi_txn_alloc_ret_req_buf(txn,
       wds_svc_id, QMI_WDS_GET_RUNTIME_SETTINGS_REQ_V01,
       qbi_svc_bc_ext_pco_q_wds2d_rsp_cb);
     QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

     qmi_req->requested_settings_valid = TRUE;
     qmi_req->requested_settings = QMI_WDS_MASK_REQ_SETTINGS_OP_RES_PCO_V01 |
       QMI_WDS_MASK_REQ_SETTINGS_PDP_TYPE_V01;

     action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
}/* qbi_svc_bc_ext_pco_q_req */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_pco_pre_check
===========================================================================*/
/*!
    @brief Performs validation before reading pco values

    @details

    @param txn

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_pco_pre_check
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_nas_cache_s *cache;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/

  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (!qbi_svc_bc_radio_state_is_radio_on(txn->ctx))
  {
    QBI_LOG_E_0("PCO::E: Radio state is off");
    txn->status = QBI_MBIM_STATUS_RADIO_POWER_OFF;
    success = FALSE;
  }

  else if (!qbi_svc_bc_nas_register_state_is_registered(cache->register_state.register_state))
  {
    QBI_LOG_E_0("PCO::E: Device is not registered ");
    txn->status = QBI_MBIM_STATUS_NOT_REGISTERED;
    success = FALSE;
  }

  else if (!qbi_svc_bc_nas_is_attached(txn->ctx))
  {
    QBI_LOG_E_0("PCO::E: Packet serivce is not attached ");
    txn->status = QBI_MBIM_STATUS_PACKET_SVC_DETACHED;
    success = FALSE;
  }

  else if (!(qbi_svc_bc_connect_is_session_ipv4_activated(txn->ctx,
    QBI_SVC_BC_EXT_PCO_DEFAULT_SESSION_ID) ||qbi_svc_bc_connect_is_session_ipv6_activated(
    txn->ctx, QBI_SVC_BC_EXT_PCO_DEFAULT_SESSION_ID)))
  {
    QBI_LOG_E_0("PCO::E: IPv4 or v6 session is not activated ");
    txn->status = QBI_MBIM_STATUS_CONTEXT_NOT_ACTIVATED;
    success = FALSE;
  }

  return success;
}/*qbi_svc_bc_ext_pco_pre_check*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_pco_q_wds2d_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_PCO query response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_pco_q_wds2d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_runtime_settings_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_pco_req_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_runtime_settings_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
  }
  else
  {
    if (qmi_rsp->operator_reserved_pco_valid != TRUE)
    {
      QBI_LOG_E_0("PCO service is not activated with current operator");
      qmi_txn->parent->status = QBI_MBIM_STATUS_SERVICE_NOT_ACTIVATED;
    }
    else
    {
      QBI_LOG_I_0(" Received QMI response for Runtime Settings");
      qmi_txn->parent->info = QBI_MEM_MALLOC_CLEAR(
        sizeof(qbi_svc_bc_ext_pco_req_info_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

      info = (qbi_svc_bc_ext_pco_req_info_s *)qmi_txn->parent->info;
      info->app_specific_info_len = qmi_rsp->operator_reserved_pco.app_specific_info_len;
      QBI_MEMSCPY(&info->app_specific_info, sizeof(info->app_specific_info),
        &qmi_rsp->operator_reserved_pco.app_specific_info, sizeof(info->app_specific_info));
      info->container_id = qmi_rsp->operator_reserved_pco.container_id;

      action = qbi_svc_bc_ext_pco_prepare_rsp (qmi_txn->parent);
    }
  }

  return action;
}/*qbi_svc_bc_ext_pco_q_wds2d_rsp_cb*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_pco_wds2d_ind_cb
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_PCO indication

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_pco_wds03_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  const wds_apn_op_reserved_pco_list_change_ind_msg_v01 *qmi_ind = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_pco_req_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);

  qmi_ind = (const wds_apn_op_reserved_pco_list_change_ind_msg_v01 *)ind->buf->data;

  ind->txn->info = QBI_MEM_MALLOC_CLEAR(
    sizeof(qbi_svc_bc_ext_pco_req_info_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn->info);

  info = (qbi_svc_bc_ext_pco_req_info_s *)ind->txn->info;
  info->app_specific_info_len = qmi_ind->pco_list.pco_list_info.pco_info[0].app_specific_info_len;
  QBI_MEMSCPY(&info->app_specific_info, sizeof(info->app_specific_info),
    &qmi_ind->pco_list.pco_list_info.pco_info[0].app_specific_info,
    sizeof(info->app_specific_info));
  info->container_id = qmi_ind->pco_list.pco_list_info.pco_info[0].container_id;
  action = qbi_svc_bc_ext_pco_prepare_rsp (ind->txn);

  return action;
}/* qbi_svc_bc_ext_pco_wds2d_ind_cb */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_pco_prepare_rsp
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_PCO response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_pco_prepare_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_pco_req_info_s *info = NULL;
  qbi_mbim_pco_value_s *rsp = NULL;
  uint8  *pco_data = NULL;
  uint8   pco_buffer_length;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_ext_pco_req_info_s *)txn->info;
  pco_buffer_length = info->app_specific_info_len + sizeof(info->container_id) +
    QBI_SVC_BC_EXT_PCO_BUFFER_LENGTH_OFFSET;
  if (pco_buffer_length > 256)
  {
    QBI_LOG_E_0("PCO::E: Buffer length exceeds max data size ");
    txn->status = QBI_MBIM_STATUS_MEMORY_FAILURE;
  }
  else
  {
    rsp = (qbi_mbim_pco_value_s *)qbi_txn_alloc_rsp_buf(
      txn, (sizeof(qbi_mbim_pco_value_s) + pco_buffer_length));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    rsp->session_id = QBI_SVC_BC_EXT_PCO_DEFAULT_SESSION_ID;
    rsp->pco_data_type = QBI_SVC_BC_EXT_MBIM_PCO_TYPE_PARTIAL;
    rsp->pco_data_size = pco_buffer_length;

    pco_data = (uint8 *) rsp + sizeof(qbi_mbim_pco_value_s);

    pco_data[0] = QBI_SVC_BC_EXT_PCO_OPERATOR_RESERVED_FORMAT_IEI;
    pco_data[1] = pco_buffer_length - QBI_SVC_BC_EXT_PCO_VALUE_OFFSET;
    /* PCO value length (total number of octets in the PCO structure -2)*/
    pco_data[2] = QBI_SVC_BC_EXT_PCO_OPERATOR_RESERVED_FORMAT_SYNTHESIZED_HEADER;

    QBI_MEMSCPY(&pco_data[3], sizeof(info->container_id),
      &(info->container_id), sizeof(info->container_id));
    pco_data[5] = (uint8) info->app_specific_info_len;
    QBI_MEMSCPY(&pco_data[6], info->app_specific_info_len,
      &(info->app_specific_info), info->app_specific_info_len);

    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
}/*qbi_svc_bc_ext_pco_prepare_rsp*/

/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO
    @{ */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_stations_info_q_req
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_base_stations_info_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_base_station_data_class_info_s *info = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_qmi_txn_s *qmi_txn = NULL;
  qbi_svc_bc_nas_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  QBI_LOG_I_0("BS_Info::Entered base info query function");
  if (!qbi_svc_bc_ext_base_station_pre_check(txn))
  {
    QBI_LOG_E_1("BS_Info:: Error state %d ", txn->status);
  }
  else
  {
    txn->info = QBI_MEM_MALLOC_CLEAR(
      sizeof(qbi_svc_bc_ext_base_station_data_class_info_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

    info = (qbi_svc_bc_ext_base_station_data_class_info_s *)txn->info;
    info->system_type = cache->register_state.available_data_class;

    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_NAS,
      QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01,
      qbi_svc_bc_ext_base_station_gsm_lte_umts_info_nas43_rsp);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /*qbi_svc_bc_ext_base_stations_info_q_req*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_base_station_pre_check
===========================================================================*/
/*!
    @brief Performs validation before getting cell info

    @details

    @param txn

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_base_station_pre_check
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_nas_cache_s *cache;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/

  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  cache = qbi_svc_bc_nas_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (!qbi_svc_bc_radio_state_is_radio_on(txn->ctx))
  {
    QBI_LOG_E_0("BS_Info::E: Radio state is off");
    txn->status = QBI_MBIM_STATUS_RADIO_POWER_OFF;
    success = FALSE;
  }

  else if (!qbi_svc_bc_nas_register_state_is_registered(cache->register_state.register_state))
  {
    QBI_LOG_E_0("BS_Info::E: Device is not registered ");
    txn->status = QBI_MBIM_STATUS_NOT_REGISTERED;
    success = FALSE;
  }

  else if (!qbi_svc_bc_nas_is_attached(txn->ctx))
  {
    QBI_LOG_E_0("BS_Info::E: Packet serivce is not attached ");
    txn->status = QBI_MBIM_STATUS_PACKET_SVC_DETACHED;
    success = FALSE;
  }

  return success;
}/*qbi_svc_bc_ext_base_station_pre_check*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_rssi_to_rx_level_conversion
===========================================================================*/
/*!
    @brief Handles conversion of received signal strength to receiver level

    @details This function converts the RSSI to RX_LEVEL in case of GSM
             neighboring cells as per MSFT's spec for cell based info

    @param rssi

    @return rx_level
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_ext_base_station_rssi_to_rx_level_conversion
(
  int16_t rssi
)
{
  uint32 rx_level = 0;
/*-------------------------------------------------------------------------*/

/*Conversion to dB*/
  rssi /= QBI_SBC_BC_EXT_BASE_STATION_POWER_CONVERSION_PARAMETER;

  if (rssi < QBI_SVC_BC_EXT_BASE_STATION_RSSI_MIN)
    rx_level = QBI_SVC_BC_EXT_BASE_STATION_GSM_RX_LEVEL_MIN;
  else if (rssi > QBI_SVC_BC_EXT_BASE_STATION_RSSI_MAX)
    rx_level = QBI_SVC_BC_EXT_BASE_STATION_GSM_RX_LEVEL_MAX;
  else
    rx_level = rssi - QBI_SVC_BC_EXT_BASE_STATION_RSSI_MIN;

  return rx_level;
}/*qbi_svc_bc_ext_base_station_rssi_to_rx_level_conversion*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_get_initial_offset
===========================================================================*/
/*!
    @brief Handles initial_offset calculation for qbi_txn_rsp_databuf_add_field
           function before allocating buffer for provider_id of neighboring
           cells

    @details

    @param initial_offset, count, skip_size

    @return modified initial_offset
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_ext_base_station_get_initial_offset
(
  uint32 initial_offset,
  uint32 count,
  uint32 size
)
{
/*-------------------------------------------------------------------------*/
  initial_offset += count * size;
  return initial_offset;
}/*qbi_svc_bc_ext_base_station_get_initial_offset*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_plmn_to_ascii_mcc_mnc
===========================================================================*/
/*!
    @brief Converts plmn coded in nibble format to mcc mnc ascii string

    @details

    @param const char *plmn
           uint32 plmn_len
           char *mcc_mnc_ascii
           uint32 mcc_mnc_len

    @return void
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_base_station_plmn_to_ascii_mcc_mnc
(
  const char *plmn,
  uint32 plmn_len,
  char *mcc_mnc_ascii,
  uint32 mcc_mnc_len
)
{
  uint8   mcc_arr[QBI_SVC_BC_EXT_MCC_LEN] = {0, };
  uint8   mnc_arr[QBI_SVC_BC_EXT_MNC_LEN] = {0, };
  uint16  mcc = 0;
  uint16  mnc = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(plmn);
  QBI_CHECK_NULL_PTR_RET(mcc_mnc_ascii);

  /*Extracting MCC-MNC from three bytes PLMN as follows
  MCC digit 2 + MCC digit 1    ------> 1st byte
  MNC digit 3 + MCC digit 3    ------> 2nd byte
  MNC digit 2 + MNC digit 1    ------> 3rd byte  */

  /* Extracting MCC digits from PLMN*/
  mcc_arr[0] = plmn[0] & QBI_SVC_BC_EXT_PLMN_NIBBLE_MASK;
  mcc_arr[1] = (plmn[0] >> 4) & QBI_SVC_BC_EXT_PLMN_NIBBLE_MASK;
  mcc_arr[2] = plmn[1] & QBI_SVC_BC_EXT_PLMN_NIBBLE_MASK;

  /* Extracting MNC digits from PLMN*/
  mnc_arr[0] = plmn[2] & QBI_SVC_BC_EXT_PLMN_NIBBLE_MASK;
  mnc_arr[1] = (plmn[2] >> 4) & QBI_SVC_BC_EXT_PLMN_NIBBLE_MASK;
  mnc_arr[2] = (plmn[1] >> 4) & QBI_SVC_BC_EXT_PLMN_NIBBLE_MASK;

  mcc = mcc_arr[0] * 100 + mcc_arr[1] * 10 + mcc_arr[2];

  /* Checking whether MNC is two or three digit*/
  if (mnc_arr[2] == QBI_SVC_BC_EXT_PLMN_NIBBLE_MASK)
  {
    mnc = mnc_arr[0] * 10 + mnc_arr[1];
  }
  else
  {
    mnc = mnc_arr[0] * 100 + mnc_arr[1] * 10 + mnc_arr[2];
  }
  QBI_LOG_I_2("BS_Info:: Parsed MCC and MNC from PLMN ID is %d and %d",
    mcc, mnc);

  QBI_SNPRINTF(mcc_mnc_ascii, mcc_mnc_len, "%03d%03d",
    mcc, mnc);

  return;
}/*qbi_svc_bc_ext_base_station_plmn_to_ascii_mcc_mnc*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_gsm_lte_umts_info_nas43_rsp
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO rsp

    @details Handles LTE/UMTS/GSM data class

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_base_station_gsm_lte_umts_info_nas43_rsp
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_cell_location_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_cell_location_info_resp_msg_v01 *)qmi_txn->rsp.data;

  QBI_LOG_I_0("BS_Info::Entered into common function for GSM/LTE/UMTS info");
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    qmi_txn = qbi_qmi_txn_alloc(qmi_txn->parent, QBI_QMI_SVC_NAS,
      QMI_NAS_GET_CDMA_POSITION_INFO_REQ_MSG_V01,
      qbi_svc_bc_ext_base_station_cdma_info_q_nas65_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    if (TRUE == qmi_rsp->umts_info_valid)
    {
      action = qbi_svc_bc_ext_base_station_umts_info_nas43_rsp(qmi_txn);
    }
    else if (TRUE == qmi_rsp->lte_intra_valid)
    {
      action = qbi_svc_bc_ext_base_station_lte_info_nas43_rsp(qmi_txn);
    }
    else if (TRUE == qmi_rsp->geran_info_valid)
    {
      action = qbi_svc_bc_ext_base_station_gsm_info_nas43_rsp(qmi_txn);
    }
  }

  return action;
}/*qbi_svc_bc_ext_base_station_gsm_lte_umts_info_nas43_rsp*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_gsm_info_nas43_rsp
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Handles GSM response

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_base_station_gsm_info_nas43_rsp
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_ext_base_station_data_class_info_s *info = NULL;
  nas_get_cell_location_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  info = (qbi_svc_bc_ext_base_station_data_class_info_s *)qmi_txn->parent->info;
  qmi_rsp = (nas_get_cell_location_info_resp_msg_v01 *)qmi_txn->rsp.data;

  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *)qbi_txn_alloc_rsp_buf(
  qmi_txn->parent, (sizeof(qbi_svc_bc_ext_base_station_info_rsp_s)));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->system_type = info->system_type;
  qbi_svc_bc_ext_base_station_gsm_serving_cell_info(qmi_txn);
  qbi_svc_bc_ext_base_station_gsm_ngbr_cell_info(
    qmi_txn, QBI_SVC_BC_EXT_BASE_STATION_RAT_GSM);

  return QBI_SVC_ACTION_SEND_RSP;
}/*qbi_svc_bc_ext_base_station_gsm_info_nas43_rsp*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_gsm_serving_cell_info
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Provides GSM serving cell info

    @param qmi_txn, rsp

    @return None
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_base_station_gsm_serving_cell_info
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_ext_gsm_serving_cell_info *gsc_info = NULL;
  nas_get_cell_location_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
  uint32 initial_offset = 0;
  uint8 *provider_id = NULL;
  char mcc_mnc_ascii_str[QBI_SVC_BC_EXT_MCC_MNC_STR_LEN] = {0, };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->rsp.data);

  qmi_rsp = (nas_get_cell_location_info_resp_msg_v01 *)qmi_txn->rsp.data;
  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *) qmi_txn->parent->rsp.data;

  initial_offset = qmi_txn->parent->infobuf_len_total;
  /*Appending serving cell info data buffer*/
  gsc_info = (qbi_svc_bc_ext_gsm_serving_cell_info *)
    qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &rsp->gsm_serving_cell, 
    0, sizeof(qbi_svc_bc_ext_gsm_serving_cell_info), NULL);
  QBI_CHECK_NULL_PTR_RET(gsc_info);
  /*Converting qmi response plmn to ascii string*/
  qbi_svc_bc_ext_base_station_plmn_to_ascii_mcc_mnc(
    qmi_rsp->geran_info.plmn, sizeof(qmi_rsp->geran_info.plmn),
    mcc_mnc_ascii_str, QBI_SVC_BC_EXT_MCC_MNC_STR_LEN);
  /*Appending provider ID data buffer after converting to UTF-16*/
  if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
    qmi_txn->parent, &gsc_info->provider_id, initial_offset,
    QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES,
    mcc_mnc_ascii_str, sizeof(mcc_mnc_ascii_str)))
  {
    QBI_LOG_E_0("Couldn't append ProviderId!");
  }

  rsp->gsm_serving_cell.size += gsc_info->provider_id.size;
  gsc_info->lac = qmi_rsp->geran_info.lac;
  gsc_info->cell_id = qmi_rsp->geran_info.cell_id;
  gsc_info->timing_advance = qmi_rsp->geran_info.timing_advance;
  gsc_info->arfcn = qmi_rsp->geran_info.arfcn;
  gsc_info->base_station_id = qmi_rsp->geran_info.bsic;
  gsc_info->rx_level = qmi_rsp->geran_info.rx_lev;

  return;
}/*qbi_svc_bc_ext_base_station_gsm_serving_cell_info*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_gsm_ngbr_cell_info
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Handles GSM neighbouring cells info

    @param qmi_txn, rsp

    @return None
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_base_station_gsm_ngbr_cell_info
(
  qbi_qmi_txn_s *qmi_txn, 
  uint32         rat_type
)
{
  qbi_svc_bc_ext_base_station_info_req_s *req = NULL;
  nas_get_cell_location_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
  qbi_svc_bc_ext_gsm_nmr *gnmr = NULL;
  qbi_svc_bc_ext_gsm_nmr_info *gnmr_info = NULL;
  qbi_mbim_offset_size_pair_s nmr_info_offset;
  uint8 *provider_id = NULL;
  uint32 cell_info_len = 0, buffer_offset = 0;
  uint32 nbr_cell_cnt = 0, freq_cnt = 0;
  char mcc_mnc_ascii_str[QBI_SVC_BC_EXT_MCC_MNC_STR_LEN] = {0, };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->rsp.data);

  qmi_rsp = (nas_get_cell_location_info_resp_msg_v01 *)qmi_txn->rsp.data;
  req = (qbi_svc_bc_ext_base_station_info_req_s *)qmi_txn->parent->req.data;
  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *) qmi_txn->parent->rsp.data;

  if (QBI_SVC_BC_EXT_BASE_STATION_RAT_GSM == rat_type)
  {
    cell_info_len = qmi_rsp->geran_info.nmr_cell_info_len;
  }
  else if (QBI_SVC_BC_EXT_BASE_STATION_RAT_UMTS == rat_type)
  {
    cell_info_len = qmi_rsp->umts_info.umts_geran_nbr_cell_len;
  }
  else if (QBI_SVC_BC_EXT_BASE_STATION_RAT_LTE == rat_type)
  {
    while (nbr_cell_cnt < qmi_rsp->lte_gsm.freqs_len)
    {
      cell_info_len += qmi_rsp->lte_gsm.freqs[nbr_cell_cnt].cells_len;
      nbr_cell_cnt++;
    }
  }
  cell_info_len = req->max_gsm_count >= cell_info_len ? 
    cell_info_len : req->max_gsm_count;

  /*Points to the start of GSM NMR response structure*/
  buffer_offset = qmi_txn->parent->infobuf_len_total;

  gnmr = (qbi_svc_bc_ext_gsm_nmr *)
    qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &rsp->gsm_nmr,
    0, sizeof(qbi_svc_bc_ext_gsm_nmr), NULL);
  QBI_CHECK_NULL_PTR_RET(gnmr);

  if (QBI_SVC_BC_EXT_BASE_STATION_RAT_GSM == rat_type)
  {
    for (nbr_cell_cnt = 0; nbr_cell_cnt < cell_info_len; nbr_cell_cnt++)
    {
      gnmr->nmr_entries_count++;
      buffer_offset = qmi_txn->parent->infobuf_len_total;
      /*Appending neighbor cell info data buffer*/
      gnmr_info = (qbi_svc_bc_ext_gsm_nmr_info *)
        qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &nmr_info_offset,
        0, sizeof(qbi_svc_bc_ext_gsm_nmr_info), NULL);
      QBI_CHECK_NULL_PTR_RET(gnmr_info);
      rsp->gsm_nmr.size += nmr_info_offset.size;
      /*Ignore plmn if the nmr_cell_id is invalid as per QMI API*/
      if (QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE !=
        qmi_rsp->geran_info.nmr_cell_info[nbr_cell_cnt].nmr_cell_id)
      {
        /*Converting qmi response plmn to ascii string*/
        qbi_svc_bc_ext_base_station_plmn_to_ascii_mcc_mnc(
          qmi_rsp->geran_info.nmr_cell_info[nbr_cell_cnt].nmr_plmn,
          sizeof(qmi_rsp->geran_info.nmr_cell_info[nbr_cell_cnt].nmr_plmn),
          mcc_mnc_ascii_str, QBI_SVC_BC_EXT_MCC_MNC_STR_LEN);
        /*Appending provider ID data buffer after converting to UTF-16*/
        if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          qmi_txn->parent, &gnmr_info->provider_id, buffer_offset,
          QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES,
          mcc_mnc_ascii_str, sizeof(mcc_mnc_ascii_str)))
        {
          QBI_LOG_E_0("Couldn't append ProviderId!");
        }
      }
      rsp->gsm_nmr.size += gnmr_info->provider_id.size;
      gnmr_info->lac =
        qmi_rsp->geran_info.nmr_cell_info[nbr_cell_cnt].nmr_lac;
      gnmr_info->cell_id =
        qmi_rsp->geran_info.nmr_cell_info[nbr_cell_cnt].nmr_cell_id;
      gnmr_info->arfcn =
        qmi_rsp->geran_info.nmr_cell_info[nbr_cell_cnt].nmr_arfcn;
      gnmr_info->base_station_id =
        qmi_rsp->geran_info.nmr_cell_info[nbr_cell_cnt].nmr_bsic;
      gnmr_info->rx_level =
        qmi_rsp->geran_info.nmr_cell_info[nbr_cell_cnt].nmr_rx_lev;
    }
  }
  else if (QBI_SVC_BC_EXT_BASE_STATION_RAT_UMTS == rat_type)
  {
    for (nbr_cell_cnt = 0; nbr_cell_cnt < cell_info_len; nbr_cell_cnt++)
    {
      gnmr->nmr_entries_count++;

      gnmr_info = (qbi_svc_bc_ext_gsm_nmr_info *)
        qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &nmr_info_offset,
        0, sizeof(qbi_svc_bc_ext_gsm_nmr_info), NULL);
      QBI_CHECK_NULL_PTR_RET(gnmr_info);

      rsp->gsm_nmr.size += nmr_info_offset.size;
      gnmr_info->lac =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
      gnmr_info->cell_id =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
      gnmr_info->arfcn =
        qmi_rsp->umts_info.umts_geran_nbr_cell[nbr_cell_cnt].geran_arfcn;
      gnmr_info->base_station_id =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
      gnmr_info->rx_level =
        qbi_svc_bc_ext_base_station_rssi_to_rx_level_conversion(
        qmi_rsp->umts_info.umts_geran_nbr_cell[nbr_cell_cnt].geran_rssi);
    }
  }
  else if (QBI_SVC_BC_EXT_BASE_STATION_RAT_LTE == rat_type)
  {
    for (freq_cnt = 0; freq_cnt < qmi_rsp->lte_gsm.freqs_len; freq_cnt++)
    {
      for (nbr_cell_cnt = 0; gnmr->nmr_entries_count < cell_info_len &&
        nbr_cell_cnt < qmi_rsp->lte_gsm.freqs[freq_cnt].cells_len; nbr_cell_cnt++)
      {
        gnmr->nmr_entries_count++;
        gnmr_info = (qbi_svc_bc_ext_gsm_nmr_info *)
          qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &nmr_info_offset,
          0, sizeof(qbi_svc_bc_ext_gsm_nmr_info), NULL);
        QBI_CHECK_NULL_PTR_RET(gnmr_info);

        rsp->gsm_nmr.size += nmr_info_offset.size;
        gnmr_info->lac =
          QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
        gnmr_info->cell_id =
          QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
        gnmr_info->arfcn =
          qmi_rsp->lte_gsm.freqs[freq_cnt].cells[nbr_cell_cnt].arfcn;
        gnmr_info->base_station_id =
          qmi_rsp->lte_gsm.freqs[freq_cnt].cells[nbr_cell_cnt].bsic_id;
        gnmr_info->rx_level =
          qbi_svc_bc_ext_base_station_rssi_to_rx_level_conversion(
          qmi_rsp->lte_gsm.freqs[freq_cnt].cells[nbr_cell_cnt].rssi);
      }
    }
  }

  return;
}/*qbi_svc_bc_ext_base_station_gsm_ngbr_cell_info*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_umts_info_nas43_rsp
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Provides UMTS info

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_base_station_umts_info_nas43_rsp
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_ext_base_station_data_class_info_s *info = NULL;
  nas_get_cell_location_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  info = (qbi_svc_bc_ext_base_station_data_class_info_s *)qmi_txn->parent->info;
  qmi_rsp = (nas_get_cell_location_info_resp_msg_v01 *)qmi_txn->rsp.data;

  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *)qbi_txn_alloc_rsp_buf(
    qmi_txn->parent, (sizeof(qbi_svc_bc_ext_base_station_info_rsp_s)));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->system_type = info->system_type;
  qbi_svc_bc_ext_base_station_umts_serving_cell_info(qmi_txn);
  qbi_svc_bc_ext_base_station_umts_ngbr_cell_info(
    qmi_txn, QBI_SVC_BC_EXT_BASE_STATION_RAT_UMTS);
  qbi_svc_bc_ext_base_station_gsm_ngbr_cell_info(
    qmi_txn, QBI_SVC_BC_EXT_BASE_STATION_RAT_UMTS);
  qbi_svc_bc_ext_base_station_lte_ngbr_cell_info(
    qmi_txn, QBI_SVC_BC_EXT_BASE_STATION_RAT_UMTS);

  return QBI_SVC_ACTION_SEND_RSP;
}/*qbi_svc_bc_ext_base_station_umts_info_nas43_rsp*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_umts_serving_cell_info
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Provides UMTS serving cell info

    @param qmi_txn, rsp

    @return None
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_base_station_umts_serving_cell_info
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_ext_umts_serving_cell_info *usc_info = NULL;
  nas_get_cell_location_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
  uint8 *provider_id = NULL;
  uint32 initial_offset = 0;
  char mcc_mnc_ascii_str[QBI_SVC_BC_EXT_MCC_MNC_STR_LEN] = {0, };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->rsp.data);

  qmi_rsp = (nas_get_cell_location_info_resp_msg_v01 *)qmi_txn->rsp.data;
  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *) qmi_txn->parent->rsp.data;

  initial_offset = qmi_txn->parent->infobuf_len_total;
  /*Appending serving cell info data buffer*/
  usc_info = (qbi_svc_bc_ext_umts_serving_cell_info *)
    qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &rsp->umts_serving_cell, 
    0, sizeof(qbi_svc_bc_ext_umts_serving_cell_info), NULL);
  QBI_CHECK_NULL_PTR_RET(usc_info);
  /*Converting qmi response plmn to ascii string*/
  qbi_svc_bc_ext_base_station_plmn_to_ascii_mcc_mnc(
    qmi_rsp->umts_info.plmn, sizeof(qmi_rsp->umts_info.plmn),
    mcc_mnc_ascii_str, QBI_SVC_BC_EXT_MCC_MNC_STR_LEN);
  /*Appending provider ID data buffer after converting to UTF-16*/
  if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
    qmi_txn->parent, &usc_info->provider_id, initial_offset,
    QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES,
    mcc_mnc_ascii_str, sizeof(mcc_mnc_ascii_str)))
  {
    QBI_LOG_E_0("Couldn't append ProviderId!");
  }
  rsp->umts_serving_cell.size += usc_info->provider_id.size;
  usc_info->lac = qmi_rsp->umts_info.lac;
  usc_info->cell_id = qmi_rsp->umts_info.cell_id;
  usc_info->frequency_info_ul =
    QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
  usc_info->frequency_info_dl =
    QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
  usc_info->frequency_info_nt =
    QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
  usc_info->uarfcn = qmi_rsp->umts_info.uarfcn;
  usc_info->psc = qmi_rsp->umts_info.psc;
  usc_info->rscp = qmi_rsp->umts_info.rscp;
  usc_info->ecno = qmi_rsp->umts_info.ecio;
  usc_info->pathloss = QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;

  return;
}/*qbi_svc_bc_ext_base_station_umts_serving_cell_info*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_umts_ngbr_cell_info
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Provides UMTS neighbouring cells info

    @param qmi_txn, rsp

    @return None
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_base_station_umts_ngbr_cell_info
(
  qbi_qmi_txn_s *qmi_txn,
  uint32         rat_type
)
{
  qbi_svc_bc_ext_base_station_info_req_s *req = NULL;
  nas_get_cell_location_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
  qbi_svc_bc_ext_umts_mrl *umrl = NULL;
  qbi_svc_bc_ext_umts_mrl_info *umrl_info = NULL;
  uint32 cell_info_len = 0;
  uint32 nbr_cell_cnt = 0, freq_cnt = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->rsp.data);

  qmi_rsp = (nas_get_cell_location_info_resp_msg_v01 *)qmi_txn->rsp.data;
  req = (qbi_svc_bc_ext_base_station_info_req_s *)qmi_txn->parent->req.data;
  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *)qmi_txn->parent->rsp.data;

  if (QBI_SVC_BC_EXT_BASE_STATION_RAT_UMTS == rat_type)
  {
    cell_info_len = qmi_rsp->umts_info.umts_monitored_cell_len;
  }
  else if (QBI_SVC_BC_EXT_BASE_STATION_RAT_LTE == rat_type)
  {
    while (nbr_cell_cnt < qmi_rsp->lte_wcdma.freqs_len)
    {
      cell_info_len += qmi_rsp->lte_wcdma.freqs[nbr_cell_cnt].cells_len;
      nbr_cell_cnt++;
    }
  }
  cell_info_len = req->max_umts_count >= cell_info_len ?
    cell_info_len : req->max_umts_count;

  umrl = (qbi_svc_bc_ext_umts_mrl *)qbi_txn_rsp_databuf_add_field(
    qmi_txn->parent, &rsp->umts_mrl, 0, sizeof(qbi_svc_bc_ext_umts_mrl) +
    cell_info_len*sizeof(qbi_svc_bc_ext_umts_mrl_info), NULL);
  QBI_CHECK_NULL_PTR_RET(umrl);

  umrl_info = (uint8 *)umrl + sizeof(qbi_svc_bc_ext_umts_mrl);

  if (QBI_SVC_BC_EXT_BASE_STATION_RAT_UMTS == rat_type)
  {
    for (nbr_cell_cnt = 0; nbr_cell_cnt < cell_info_len; nbr_cell_cnt++)
    {
      umrl->mrl_entries_count++;

      umrl_info->lac =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
      umrl_info->cell_id =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
      umrl_info->uarfcn =
        qmi_rsp->umts_info.umts_monitored_cell[nbr_cell_cnt].umts_uarfcn;
      umrl_info->psc =
        qmi_rsp->umts_info.umts_monitored_cell[nbr_cell_cnt].umts_psc;
      umrl_info->rscp =
        qmi_rsp->umts_info.umts_monitored_cell[nbr_cell_cnt].umts_rscp;
      umrl_info->ecno =
        qmi_rsp->umts_info.umts_monitored_cell[nbr_cell_cnt].umts_ecio;
      umrl_info->pathloss =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;

      umrl_info = (uint8 *) umrl_info + sizeof(qbi_svc_bc_ext_umts_mrl_info);
    }
  }
  else if (QBI_SVC_BC_EXT_BASE_STATION_RAT_LTE == rat_type)
  {
    for (freq_cnt = 0; freq_cnt < qmi_rsp->lte_wcdma.freqs_len; freq_cnt++)
    {
      for (nbr_cell_cnt = 0; umrl->mrl_entries_count < cell_info_len && 
        nbr_cell_cnt < qmi_rsp->lte_wcdma.freqs[freq_cnt].cells_len; nbr_cell_cnt++)
      {
        umrl->mrl_entries_count++;

        umrl_info->lac =
          QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
        umrl_info->cell_id =
          QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
        umrl_info->uarfcn =
          QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
        umrl_info->psc =
          qmi_rsp->lte_wcdma.freqs[freq_cnt].cells[nbr_cell_cnt].psc;
        umrl_info->rscp =
          qmi_rsp->lte_wcdma.freqs[freq_cnt].cells[nbr_cell_cnt].cpich_rscp /
          QBI_SBC_BC_EXT_BASE_STATION_POWER_CONVERSION_PARAMETER;
        umrl_info->ecno =
          qmi_rsp->lte_wcdma.freqs[freq_cnt].cells[nbr_cell_cnt].cpich_ecno /
          QBI_SBC_BC_EXT_BASE_STATION_POWER_CONVERSION_PARAMETER;
        umrl_info->pathloss =
          QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;

        umrl_info = (uint8 *) umrl_info + sizeof(qbi_svc_bc_ext_umts_mrl_info); 
      }
    }
  }

  return;
}/*qbi_svc_bc_ext_base_station_umts_ngbr_cell_info*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_lte_info_nas43_rsp
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Handles LTE response

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_base_station_lte_info_nas43_rsp
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_ext_base_station_data_class_info_s *info = NULL;
  nas_get_cell_location_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  info =
    (qbi_svc_bc_ext_base_station_data_class_info_s *)qmi_txn->parent->info;
  qmi_rsp = (nas_get_cell_location_info_resp_msg_v01 *)qmi_txn->rsp.data;

  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *)qbi_txn_alloc_rsp_buf(
    qmi_txn->parent, (sizeof(qbi_svc_bc_ext_base_station_info_rsp_s)));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->system_type = info->system_type;
  qbi_svc_bc_ext_base_station_lte_serving_cell_info(qmi_txn);
  qbi_svc_bc_ext_base_station_lte_ngbr_cell_info(
    qmi_txn, QBI_SVC_BC_EXT_BASE_STATION_RAT_LTE);
  qbi_svc_bc_ext_base_station_gsm_ngbr_cell_info(
    qmi_txn, QBI_SVC_BC_EXT_BASE_STATION_RAT_LTE);
  qbi_svc_bc_ext_base_station_umts_ngbr_cell_info(
    qmi_txn, QBI_SVC_BC_EXT_BASE_STATION_RAT_LTE);

  return QBI_SVC_ACTION_SEND_RSP;
}/*qbi_svc_bc_ext_base_station_lte_info_nas43_rsp*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_lte_serving_cell_info
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Provides LTE serving cell info

    @param qmi_txn, rsp

    @return None
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_base_station_lte_serving_cell_info
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_ext_lte_serving_cell_info *lsc_info = NULL;
  nas_get_cell_location_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
  uint8 *provider_id = NULL;
  uint32 initial_offset = 0;
  uint32 cell_info_len = 0;
  char mcc_mnc_ascii_str[QBI_SVC_BC_EXT_MCC_MNC_STR_LEN] = {0, };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->rsp.data);

  qmi_rsp = (nas_get_cell_location_info_resp_msg_v01 *)qmi_txn->rsp.data;
  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *) qmi_txn->parent->rsp.data;

  initial_offset = qmi_txn->parent->infobuf_len_total;
  /*Appending serving cell info data buffer*/
  lsc_info = (qbi_svc_bc_ext_lte_serving_cell_info *)
    qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &rsp->lte_serving_cell, 
    0, sizeof(qbi_svc_bc_ext_lte_serving_cell_info), NULL);
  QBI_CHECK_NULL_PTR_RET(lsc_info);
  /*Converting qmi response plmn to ascii string*/
  qbi_svc_bc_ext_base_station_plmn_to_ascii_mcc_mnc(
    qmi_rsp->lte_intra.plmn, sizeof(qmi_rsp->lte_intra.plmn),
    mcc_mnc_ascii_str, QBI_SVC_BC_EXT_MCC_MNC_STR_LEN);
  /*Appending provider ID data buffer after converting to UTF-16*/
  if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
    qmi_txn->parent, &lsc_info->provider_id, initial_offset,
    QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES,
    mcc_mnc_ascii_str, sizeof(mcc_mnc_ascii_str)))
  {
    QBI_LOG_E_0("Couldn't append ProviderId!");
  }
  rsp->lte_serving_cell.size += lsc_info->provider_id.size;
  lsc_info->cell_id = qmi_rsp->lte_intra.global_cell_id;
  lsc_info->earfcn = qmi_rsp->lte_intra.earfcn;
  lsc_info->tac = qmi_rsp->lte_intra.tac;
  lsc_info->timing_advance = qmi_rsp->timing_advance;

  for (cell_info_len = 0; cell_info_len < qmi_rsp->lte_intra.cells_len; cell_info_len++)
  {
    if (qmi_rsp->lte_intra.cells[cell_info_len].pci ==
      qmi_rsp->lte_intra.serving_cell_id)
    {
      lsc_info->pci = qmi_rsp->lte_intra.cells[cell_info_len].pci;
      lsc_info->rsrp = qmi_rsp->lte_intra.cells[cell_info_len].rsrp/
        QBI_SBC_BC_EXT_BASE_STATION_POWER_CONVERSION_PARAMETER;
      lsc_info->rsrq = qmi_rsp->lte_intra.cells[cell_info_len].rsrq/
        QBI_SBC_BC_EXT_BASE_STATION_POWER_CONVERSION_PARAMETER;
      break;
    }
  }

  return;
}/*qbi_svc_bc_ext_base_station_lte_serving_cell_info*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_lte_ngbr_cell_info
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Provides LTE neighbouring cells info

    @param qmi_txn, rsp

    @return None
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_base_station_lte_ngbr_cell_info
(
  qbi_qmi_txn_s *qmi_txn,
  uint32         rat_type
)
{
  qbi_svc_bc_ext_base_station_info_req_s *req = NULL;
  nas_get_cell_location_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
  qbi_svc_bc_ext_lte_serving_cell_info *lsc_info = NULL;
  qbi_svc_bc_ext_lte_mrl *lmrl = NULL;
  qbi_svc_bc_ext_lte_mrl_info *lmrl_info = NULL;
  uint8 *provider_id = NULL;
  int cell_info_len = 0, nbr_cell_index = 0, cell_info_cnt = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->rsp.data);

  qmi_rsp = (nas_get_cell_location_info_resp_msg_v01 *)qmi_txn->rsp.data;
  req = (qbi_svc_bc_ext_base_station_info_req_s *)qmi_txn->parent->req.data;
  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *) qmi_txn->parent->rsp.data;

  if (QBI_SVC_BC_EXT_BASE_STATION_RAT_LTE == rat_type)
  {
    cell_info_len = qmi_rsp->lte_intra.cells_len;
  }
  else if (QBI_SVC_BC_EXT_BASE_STATION_RAT_UMTS == rat_type)
  {
    cell_info_len = qmi_rsp->wcdma_lte.umts_lte_nbr_cell_len;
  }

  cell_info_len = req->max_lte_count >= cell_info_len ? 
    cell_info_len : req->max_lte_count;

  lmrl = (qbi_svc_bc_ext_lte_mrl *)
    qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &rsp->lte_mrl,
    0, sizeof(qbi_svc_bc_ext_lte_mrl) + cell_info_len
    *sizeof(qbi_svc_bc_ext_lte_mrl_info), NULL);
  QBI_CHECK_NULL_PTR_RET(lmrl);

  lmrl->mrl_entries_count = cell_info_len;
  if (QBI_SVC_BC_EXT_BASE_STATION_RAT_LTE == rat_type)
  {
    for (cell_info_cnt = 0; cell_info_cnt < lmrl->mrl_entries_count; cell_info_cnt++)
    {
      lmrl_info = (uint8 *)lmrl + sizeof(qbi_svc_bc_ext_lte_mrl)
        + sizeof(qbi_svc_bc_ext_lte_mrl_info)*nbr_cell_index;
      /*filling information related to neighbouring cells*/
      lmrl_info->cell_id =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
      lmrl_info->earfcn =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
      lmrl_info->pci = qmi_rsp->lte_intra.cells[cell_info_cnt].pci;
      lmrl_info->tac =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
      lmrl_info->rsrp =
        qmi_rsp->lte_intra.cells[cell_info_cnt].rsrp /
        QBI_SBC_BC_EXT_BASE_STATION_POWER_CONVERSION_PARAMETER;
      lmrl_info->rsrq =
        qmi_rsp->lte_intra.cells[cell_info_cnt].rsrq/
        QBI_SBC_BC_EXT_BASE_STATION_POWER_CONVERSION_PARAMETER;
      nbr_cell_index++;
    }
  }
  else if (QBI_SVC_BC_EXT_BASE_STATION_RAT_UMTS == rat_type)
  {
    for (cell_info_len = 0; cell_info_len < lmrl->mrl_entries_count; cell_info_len++)
    {
      lmrl_info = (uint8 *)lmrl + sizeof(qbi_svc_bc_ext_lte_mrl)
        + sizeof(qbi_svc_bc_ext_lte_mrl_info)*cell_info_len;
      /*filling information related to neighbouring cells*/
      lmrl_info->cell_id =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
      lmrl_info->earfcn =
        qmi_rsp->wcdma_lte.umts_lte_nbr_cell[cell_info_len].earfcn;
      lmrl_info->pci = qmi_rsp->wcdma_lte.umts_lte_nbr_cell[cell_info_len].pci;
      lmrl_info->tac =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
      lmrl_info->rsrp =
        (int32) qmi_rsp->wcdma_lte.umts_lte_nbr_cell[cell_info_len].rsrp;
      lmrl_info->rsrq =
        (int32) qmi_rsp->wcdma_lte.umts_lte_nbr_cell[cell_info_len].rsrq;
    }
  }

  return;
}/*qbi_svc_bc_ext_base_station_lte_ngbr_cell_info*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_cdma_info_q_nas65_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Handles CDMA response

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_base_station_cdma_info_q_nas65_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_ext_base_station_data_class_info_s *info = NULL;
  nas_get_cdma_position_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  info = (qbi_svc_bc_ext_base_station_data_class_info_s *)qmi_txn->parent->info;
  qmi_rsp = (nas_get_cdma_position_info_resp_msg_v01 *)qmi_txn->rsp.data;

  if (QMI_RESULT_SUCCESS_V01 != qmi_rsp->resp.result)
  {
    qmi_txn = qbi_qmi_txn_alloc(qmi_txn->parent, QBI_QMI_SVC_NAS,
      QMI_NAS_GET_TDS_CELL_AND_POSITION_INFO_REQ_MSG_V01,
      qbi_svc_bc_ext_base_station_tdscdma_info_q_nas6d_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *)qbi_txn_alloc_rsp_buf(
      qmi_txn->parent, sizeof(qbi_svc_bc_ext_base_station_info_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    rsp->system_type = info->system_type;
    qbi_svc_bc_ext_base_station_cdma_cell_info(qmi_txn);

    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
}/*qbi_svc_bc_ext_base_station_cdma_info_q_nas65_rsp_cb*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_cdma_cell_info
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Provides CDMA serving and neighbouring cells info

    @param qmi_txn, rsp

    @return None
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_base_station_cdma_cell_info
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_cdma_position_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
  qbi_svc_bc_ext_cdma_mrl *cmrl = NULL;
  qbi_svc_bc_ext_cdma_mrl_info *cmrl_info = NULL;
  qbi_svc_bc_ext_base_station_info_req_s *req = NULL;
  uint32 cell_info_len = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->rsp.data);

  qmi_rsp = (nas_get_cdma_position_info_resp_msg_v01 *)qmi_txn->rsp.data;
  req = (qbi_svc_bc_ext_base_station_info_req_s *)qmi_txn->parent->req.data;
  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *) qmi_txn->parent->rsp.data;

  cell_info_len = req->max_cdma_count >= qmi_rsp->info.bs_len ?
    qmi_rsp->info.bs_len : req->max_cdma_count;

  cmrl = (qbi_svc_bc_ext_cdma_mrl *)
    qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &rsp->cdma_mrl,
    0, sizeof(qbi_svc_bc_ext_cdma_mrl) +
    cell_info_len*sizeof(qbi_svc_bc_ext_cdma_mrl_info), NULL);
  QBI_CHECK_NULL_PTR_RET(cmrl);

  cmrl->mrl_entries_count = cell_info_len;
  for (cell_info_len = 0;cell_info_len < cmrl->mrl_entries_count;cell_info_len++)
  {
    cmrl_info = (uint8 *)cmrl + sizeof(qbi_svc_bc_ext_cdma_mrl)
      + sizeof(qbi_svc_bc_ext_cdma_mrl_info)*cell_info_len;

    /*Filling information related to serving cell and neighbouring cell*/
    cmrl_info->serving_cell_flag =
      qmi_rsp->info.bs[cell_info_len].pilot_type;
    cmrl_info->nid = qmi_rsp->info.bs[cell_info_len].nid;
    cmrl_info->sid = qmi_rsp->info.bs[cell_info_len].sid;
    cmrl_info->base_station_id =
      qmi_rsp->info.bs[cell_info_len].base_id;
    cmrl_info->base_lat =
      qmi_rsp->info.bs[cell_info_len].base_lat;
    cmrl_info->base_long =
      qmi_rsp->info.bs[cell_info_len].base_long;
    cmrl_info->refpn = qmi_rsp->info.bs[cell_info_len].pilot_pn;
    cmrl_info->gps_seconds =
      qmi_rsp->info.bs[cell_info_len].time_stamp;
    cmrl_info->pilot_strength =
      qmi_rsp->info.bs[cell_info_len].pilot_strength;
  }

  return;
}/*qbi_svc_bc_ext_base_station_cdma_cell_info*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_tdscdma_info_q_nas6d_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Handles TD-SCDMA response

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_base_station_tdscdma_info_q_nas6d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_ext_base_station_data_class_info_s *info = NULL;
  nas_get_tds_cell_and_position_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  info =
    (qbi_svc_bc_ext_base_station_data_class_info_s *)qmi_txn->parent->info;
  qmi_rsp =
    (nas_get_tds_cell_and_position_info_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("BS_Info::Received error code %d from QMI", qmi_rsp->resp.error);
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
  }
  else
  {
    rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *)qbi_txn_alloc_rsp_buf(
      qmi_txn->parent, (sizeof(qbi_svc_bc_ext_base_station_info_rsp_s)));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    rsp->system_type = info->system_type;
    qbi_svc_bc_ext_base_station_tdscdma_serving_cell_info(qmi_txn);
    qbi_svc_bc_ext_base_station_tdscdma_ngbr_cell_info(qmi_txn);

    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
}/*qbi_svc_bc_ext_base_station_tdscdma_info_q_nas6d_rsp_cb*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_tdscdma_serving_cell_info
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Provides TD-SCDMA serving cell info

    @param qmi_txn, rsp

    @return None
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_base_station_tdscdma_serving_cell_info
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_ext_tdscdma_serving_cell_info *tsc_info = NULL;
  nas_get_tds_cell_and_position_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
  uint8 *provider_id = NULL;
  uint32 initial_offset = 0;
  char mcc_mnc_ascii_str[QBI_SVC_BC_EXT_MCC_MNC_STR_LEN] = {0, };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->rsp.data);

  qmi_rsp = (nas_get_tds_cell_and_position_info_resp_msg_v01 *)qmi_txn->rsp.data;
  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *) qmi_txn->parent->rsp.data;

  initial_offset = qmi_txn->parent->infobuf_len_total;
  /*Appending serving cell info data buffer*/
  tsc_info = (qbi_svc_bc_ext_tdscdma_serving_cell_info *)
    qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &rsp->tdscdma_serving_cell, 
    0, sizeof(qbi_svc_bc_ext_tdscdma_serving_cell_info), NULL);
  QBI_CHECK_NULL_PTR_RET(tsc_info);

  QBI_SNPRINTF(mcc_mnc_ascii_str, QBI_SVC_BC_EXT_MCC_MNC_STR_LEN, "%03d%03d",
    qmi_rsp->tds_cell_info.plmn.mcc, qmi_rsp->tds_cell_info.plmn.mnc);
  /*Appending provider ID data buffer after converting to UTF-16*/
  if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
    qmi_txn->parent, &tsc_info->provider_id, initial_offset,
    QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES,
    mcc_mnc_ascii_str, sizeof(mcc_mnc_ascii_str)))
  {
    QBI_LOG_E_0("Couldn't append ProviderId!");
  }
  rsp->tdscdma_serving_cell.size += tsc_info->provider_id.size;
  tsc_info->lac = qmi_rsp->tds_cell_info.lac;
  tsc_info->cell_id = qmi_rsp->tds_cell_info.cell_id;
  tsc_info->uarfcn = qmi_rsp->tds_cell_info.uarfcn;
  tsc_info->cell_parameter_id = qmi_rsp->tds_cell_info.cell_parameter_id;
  tsc_info->timing_advance = (uint32) qmi_rsp->tds_cell_info.timing_advance;
  tsc_info->rscp = (int32) qmi_rsp->tds_cell_info.rscp;
  tsc_info->pathloss = qmi_rsp->tds_cell_info.pathloss;

  return;
}/*qbi_svc_bc_ext_base_station_tdscdma_serving_cell_info*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_base_station_tdscdma_ngbr_cell_info
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_BASE_STATIONS_INFO query rsp

    @details Provides TD-SCDMA neighbouring cells info

    @param qmi_txn, rsp

    @return None
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_base_station_tdscdma_ngbr_cell_info
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_tds_cell_and_position_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_base_station_info_rsp_s *rsp = NULL;
  qbi_svc_bc_ext_tdscdma_mrl *tmrl = NULL;
  qbi_svc_bc_ext_tdscdma_mrl_info *tmrl_info = NULL;
  qbi_svc_bc_ext_base_station_info_req_s *req = NULL;
  int cell_info_len = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->parent->rsp.data);

  qmi_rsp = (nas_get_tds_cell_and_position_info_resp_msg_v01 *)qmi_txn->rsp.data;
  req = (qbi_svc_bc_ext_base_station_info_req_s *) qmi_txn->parent->req.data;
  rsp = (qbi_svc_bc_ext_base_station_info_rsp_s *) qmi_txn->parent->rsp.data;

  cell_info_len = req->max_tdscdma_count >= qmi_rsp->tds_nbr_cell_info_len ? 
    qmi_rsp->tds_nbr_cell_info_len : req->max_tdscdma_count;

  tmrl = (qbi_svc_bc_ext_tdscdma_mrl *)
    qbi_txn_rsp_databuf_add_field(qmi_txn->parent, &rsp->tdscdma_mrl, 
    0, sizeof(qbi_svc_bc_ext_tdscdma_mrl) +
    cell_info_len*sizeof(qbi_svc_bc_ext_tdscdma_mrl_info), NULL);
  QBI_CHECK_NULL_PTR_RET(tmrl);

  tmrl->mrl_entries_count = cell_info_len;
  for (cell_info_len = 0;cell_info_len < tmrl->mrl_entries_count;cell_info_len++)
  {
    tmrl_info = (uint8 *)tmrl + sizeof(qbi_svc_bc_ext_tdscdma_mrl)
      + sizeof(qbi_svc_bc_ext_tdscdma_mrl_info)*cell_info_len;

  /*Filling information related to neighbouring cells*/
    tmrl_info->lac =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
    tmrl_info->cell_id =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
    tmrl_info->uarfcn =
        qmi_rsp->tds_nbr_cell_info[cell_info_len].uarfcn;
    tmrl_info->cell_parameter_id =
        qmi_rsp->tds_nbr_cell_info[cell_info_len].cell_parameter_id;
    tmrl_info->timing_advance =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
    tmrl_info->rscp =
        (int32) qmi_rsp->tds_nbr_cell_info[cell_info_len].rscp;
    tmrl_info->pathloss =
        QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
  }

  return;
}/*qbi_svc_bc_ext_base_station_tdscdma_ngbr_cell_info*/

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_LOCATION_INFO
    @{ */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_location_info_q_req
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_LOCATION_INFO query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_location_info_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_base_station_data_class_info_s *info = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  nas_get_sys_info_req_msg_v01 *qmi_txn = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);


  QBI_LOG_I_0("Location_Info::Entered location info query function");
  if (!qbi_svc_bc_ext_base_station_pre_check(txn))
  {
    QBI_LOG_E_1("Location_Info:: Error state %d ", txn->status);
  }
  else
  {
    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_NAS,
      QMI_NAS_GET_SYS_INFO_REQ_MSG_V01,
      qbi_svc_bc_ext_location_info_q_nas_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /*qbi_svc_bc_ext_location_info_q_req*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_location_info_q_nas_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_LOCATION_INFO query rsp

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_location_info_q_nas_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_sys_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_location_info_rsp_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_sys_info_resp_msg_v01 *)qmi_txn->rsp.data;

  rsp = (qbi_svc_bc_ext_location_info_rsp_s *)qbi_txn_alloc_rsp_buf(
  qmi_txn->parent, (sizeof(qbi_svc_bc_ext_location_info_rsp_s)));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  if (qmi_rsp->gsm_sys_info_valid == TRUE)
  {
    rsp->location_area_code = qmi_rsp->gsm_sys_info.threegpp_specific_sys_info.lac;
    rsp->tracking_area_code = QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
    rsp->cell_id = qmi_rsp->gsm_sys_info.threegpp_specific_sys_info.cell_id;
  }
  else if (qmi_rsp->wcdma_sys_info_valid == TRUE)
  {
    rsp->location_area_code = qmi_rsp->wcdma_sys_info.threegpp_specific_sys_info.lac;
    rsp->tracking_area_code = QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
    rsp->cell_id = qmi_rsp->wcdma_sys_info.threegpp_specific_sys_info.cell_id;
  }
  else if (qmi_rsp->lte_sys_info_valid == TRUE)
  {
    rsp->location_area_code = QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
    rsp->tracking_area_code = qmi_rsp->lte_sys_info.lte_specific_sys_info.tac;
    rsp->cell_id = qmi_rsp->lte_sys_info.threegpp_specific_sys_info.cell_id;
  }
  else
  {
    rsp->location_area_code = QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
    rsp->tracking_area_code = QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
    rsp->cell_id = QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
  }

  return QBI_SVC_ACTION_SEND_RSP;
}/*qbi_svc_bc_ext_location_info_q_nas_rsp_cb*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_location_info_nas03_ind_cb
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_LOCATION_INFO indication

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_location_info_nas03_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  const nas_sys_info_ind_msg_v01 *qmi_ind = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_module_location_info_cache_s *cache = NULL;
  qbi_svc_bc_ext_location_info_rsp_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const nas_sys_info_ind_msg_v01 *)ind->buf->data;

  cache = qbi_svc_bc_ext_location_info_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_bc_ext_location_info_rsp_s *)qbi_txn_alloc_rsp_buf(
    ind->txn, (sizeof(qbi_svc_bc_ext_location_info_rsp_s)));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  if (!qbi_svc_bc_ext_base_station_pre_check(ind->txn))
  {
    QBI_LOG_E_1("Location_Info:: Error state %d ", ind->txn->status);
  }
  else
  {
    QBI_LOG_I_0("Location_Info::Received unsolicited indication from Modem");
    if (qmi_ind->gsm_sys_info_valid == TRUE)
    {
      if (cache->location_area_code != qmi_ind->gsm_sys_info.threegpp_specific_sys_info.lac)
      {
        rsp->location_area_code = qmi_ind->gsm_sys_info.threegpp_specific_sys_info.lac;
        rsp->tracking_area_code = QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
        rsp->cell_id = qmi_ind->gsm_sys_info.threegpp_specific_sys_info.cell_id;
        action = qbi_svc_bc_ext_update_location_info_cache(rsp, cache);
      }
    }
    else if (qmi_ind->wcdma_sys_info_valid == TRUE)
    {
      if (cache->location_area_code != qmi_ind->wcdma_sys_info.threegpp_specific_sys_info.lac)
      {
        rsp->location_area_code = qmi_ind->wcdma_sys_info.threegpp_specific_sys_info.lac;
        rsp->tracking_area_code = QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
        rsp->cell_id = qmi_ind->wcdma_sys_info.threegpp_specific_sys_info.cell_id;
        action = qbi_svc_bc_ext_update_location_info_cache(rsp, cache);
      }
    }
    else if (qmi_ind->lte_sys_info_valid == TRUE)
    {
      if (cache->tracking_area_code != qmi_ind->lte_sys_info.lte_specific_sys_info.tac)
      {
        rsp->location_area_code = QBI_SVC_BC_EXT_BASE_STATION_PARAMETERS_DEFAULT_VALUE;
        rsp->tracking_area_code = qmi_ind->lte_sys_info.lte_specific_sys_info.tac;
        rsp->cell_id = qmi_ind->lte_sys_info.threegpp_specific_sys_info.cell_id;
        action = qbi_svc_bc_ext_update_location_info_cache(rsp, cache);
      }
    }
  }

  return action;
}/* qbi_svc_bc_ext_location_info_nas03_ind_cb*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_update_location_info_cache
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_LOCATION_INFO cache update

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_update_location_info_cache
(
  qbi_svc_bc_ext_location_info_rsp_s *rsp,
  qbi_svc_bc_ext_module_location_info_cache_s *cache
)
{
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  cache->location_area_code = rsp->location_area_code;
  cache->tracking_area_code = rsp->tracking_area_code;

  return QBI_SVC_ACTION_SEND_RSP;
}/* qbi_svc_bc_ext_update_location_info_cache*/

/*! @} */
/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_init
===========================================================================*/
/*!
    @brief One-time initialization of the MSHOSTSHUTDOWN device service

    @details
*/
/*=========================================================================*/
void qbi_svc_bc_ext_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_bc_ext_cfg = {
    {
      0x3d, 0x01, 0xdc, 0xc5, 0xfe, 0xf5, 0x4d, 0x05,
      0x0d, 0x3a, 0xbe, 0xf7, 0x05, 0x8e, 0x9a, 0xaf
    },
    QBI_SVC_ID_BC_EXT,
    FALSE,
    qbi_svc_bc_ext_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_bc_ext_cmd_hdlr_tbl),
    qbi_svc_bc_ext_open,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_bc_ext_cfg);
} /* qbi_svc_bc_ext_init() */
