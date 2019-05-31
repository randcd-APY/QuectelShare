/*!
  @file
  qbi_svc_bc.c

  @brief
  Basic Connectivity device service implementation
*/

/*=============================================================================

  Copyright (c) 2011-2015, 2017-2018 Qualcomm Technologies, Inc.
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
09/16/18  sk   Added case-insensitive profile match logic.
07/17/18  rv   Fixed issue related to PDN request rejected by network
07/25/18  sk   PDN Type Check for CDMA Profiles.
06/26/18  mm   Removed persistent and added non persistent cache for DSSA
05/24/18  nk   Registered for indication of location info from modem
05/08/18  vk   Handling timeout for connect request
04/12/18  ar   Fixed issue related to long name during roam scenario
04/03/18  ks   Add Support for PCIe non-fast data path.
03/13/18  rv   Add timeout logic for CONNECT set request
03/12/18  mm   Moved protos from here due to compialtion issue related to SSR
02/05/18  mm   Modem SSR compilation fix
01/04/18  nk   Moved common functions to header
01/05/18  ha   Add Support for PCIe data path.
01/04/18  nk   Publish IP address to host even if DNS address not avilable
11/14/17  mm   Fixed issue related to PDN request rejected by network
10/28/17  rv   Avoid deregister in case of 3G
10/27/17  nk   Added header file for LTE ATTACH and PROVISION CONTEXT
10/04/17  vk   Added fix for SUPL
09/10/17  nk   Fixed KW p1 issues and warnings
09/08/17  vk   Added logic to set APSS created profiles (temp and LTE attach)
               to default profile when data calls is brought up using APSS profile.
               The original default profile is restored post data call teardown
               and the temp profile is deleted. LTE profiles will remain intact.
08/18/17  vk   Add Support For Single PDP
07/26/17  mm   Added indication for slot2
06/28/17  rv   Add initialization for operator config
06/07/17  mm   fixed memory leakage
06/02/17  vk   Initialization of executor slot index
06/02/17  vk   Fixed double free of info pointer and cache access
05/23/17  mm   Added registration for refresh indication
03/30/17  mm   Added logic for temp profile creation
02/15/17  vs   Allow 3GPP data profile to not be present in FTM mode
08/20/15  hz   Add EPC to legacy profile fallback
06/17/14  hz   Support APN bearer for 3GPP
06/03/14  hz   Add EPC profile support
11/19/13  bd   Get telephone numbers from QMI PBM
11/18/13  bd   Combine PLMN name and SPN per 3GPP 22.101 A.4 (NV configurable)
11/14/13  hz   Handle deactivate request received during deactivation
09/24/13  hz   Set IP family preference after received WDS bind response
06/21/13  bd   Rearrange QMI messages to reduce time for MBIM_OPEN
05/06/13  bd   Disallow RADIO_STATE set requests while in FTM
04/23/13  hz   Fix provisioned contexts 3gpp2 profile modify issue
04/22/13  hz   Klocwork patch
04/10/13  hz   Add multiple data sessions support detection
03/13/13  hz   Add support for multiple provisioned contexts and data sessions
03/12/13  bd   Add QMI indication (de)registration support
02/21/13  bd   Delay MBIM_OPEN while previous connection is deactivating
02/01/13  cy   Increase max number of PacketFilters to 32
01/25/13  bd   Support configuration of data path in qbi_hc layer
11/21/12  bd   Add runtime DeviceType configuration
07/02/12  bd   Switch from SERVING_SYSTEM to SYS_INFO QMI_NAS messages
05/30/12  bd   Split NAS functionality into new Basic Connectivity sub-module
05/14/12  bd   Use new QCCI API to get transaction ID
01/30/12  cy   Add NTB max num/size config and Reset function
01/30/12  cy   Packet filter support
01/13/12  bd   Expose AvailableDataClass for SMS transcoding
12/09/11  cy   Dual IP support
10/28/11  bd   Updated to MBIM v1.0 SC
09/23/11  bd   Use new QMI WDA set data format message to configure NTB
09/02/11  bd   Updated to MBIM v0.81c
08/08/11  bd   Fix multiple issues with PIN set operations
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/


/*=============================================================================

  Include Files

=============================================================================*/


#include "qbi_svc_bc.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_svc_bc_nas.h"
#include "qbi_svc_bc_sim.h"
#include "qbi_svc_bc_ext.h"
#include "qbi_svc_bc_ext_prov.h"
#include "qbi_svc_bc_spdp.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_msg.h"
#include "qbi_msg_mbim.h"
#include "qbi_nv_store.h"
#include "qbi_qmi.h"
#include "qbi_qmi_defs.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_txn.h"
#include "qbi_hc.h"

#include "device_management_service_v01.h"
#include "network_access_service_v01.h"
#include "phonebook_manager_service_v01.h"
#include "user_identity_module_v01.h"
#include "wireless_data_administrative_service_v01.h"
#include "wireless_data_service_v01.h"

#include <stdint.h>


/*=============================================================================

  Private Macros

=============================================================================*/


/*! This macro statically defines a QMI indication handler and fills in the
    fields that are common to all handlers in this device service */
#define QBI_SVC_BC_STATIC_IND_HDLR(qmi_svc_id, qmi_msg_id, cid, cb) \
  {qmi_svc_id, qmi_msg_id, cid, cb, NULL}


/*=============================================================================

  Private Constants

=============================================================================*/


/* Timeouts for QMI operation mode change request */
#define QBI_SVC_BC_RADIO_STATE_SET_TIMEOUT_MS   (9 * 1000)

/* Extended timeout for connect */
#define QBI_SVC_BC_CONNECT_TIMEOUT_MS           (300 * 1000)

/*! Map session ID to QMI WDS IPv4 service ID */
#define QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id) \
   (session_id < QBI_SVC_BC_MAX_SESSIONS ? \
    (qbi_qmi_svc_e)(QBI_QMI_SVC_WDS_FIRST + session_id * 2) : \
    (qbi_qmi_svc_e)QBI_QMI_SVC_WDS_FIRST)

/*! Map session ID to QMI WDS IPv6 service ID */
#define QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id) \
   (session_id < QBI_SVC_BC_MAX_SESSIONS ? \
    ((qbi_qmi_svc_e)(QBI_QMI_SVC_WDS_FIRST + session_id * 2 + 1)) : \
    (qbi_qmi_svc_e)(QBI_QMI_SVC_WDS_FIRST + 1))

/*! Map QMI WDS service ID to session ID */
#define QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(wds_svc_id) \
   ((wds_svc_id >= QBI_QMI_SVC_WDS_FIRST && wds_svc_id <= QBI_QMI_SVC_WDS_LAST) ? \
    (wds_svc_id - QBI_QMI_SVC_WDS_FIRST) / 2 : 0)

/*! Returns TRUE if the given service ID is mapped to IPv4 */
#define QBI_SVC_BC_WDS_SVC_ID_IS_IPV4(wds_svc_id) \
   ((wds_svc_id >= QBI_QMI_SVC_WDS_FIRST && wds_svc_id <= QBI_QMI_SVC_WDS_LAST) ? \
    (wds_svc_id - QBI_QMI_SVC_WDS_FIRST) % 2 == 0 : FALSE)

/*! Returns TRUE if the given service ID is mapped to IPv6 */
#define QBI_SVC_BC_WDS_SVC_ID_IS_IPV6(wds_svc_id) \
   ((wds_svc_id >= QBI_QMI_SVC_WDS_FIRST && wds_svc_id <= QBI_QMI_SVC_WDS_LAST) ? \
    (wds_svc_id - QBI_QMI_SVC_WDS_FIRST) % 2 : FALSE)

/*! Returns TRUE if the given IP type includes IPv4 */
#define QBI_SVC_BC_CONNECT_IPV4_REQUESTED(ip_type) \
   (QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV4 || \
    QBI_SVC_BC_IS_MBIM_DUAL_IP(ip_type))

/*! Returns TRUE if the given IP type includes IPv6 */
#define QBI_SVC_BC_CONNECT_IPV6_REQUESTED(ip_type) \
   (QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(ip_type) == QBI_SVC_BC_IP_TYPE_IPV6 || \
    QBI_SVC_BC_IS_MBIM_DUAL_IP(ip_type))

/*! Loopback APN string */
#define QBI_SVC_BC_CONNECT_LOOPBACK_APN_ASCII ("loopback")

/* Bitmask created from MBIM activation states */
#define QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED \
  (1 << QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
#define QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATING \
  (1 << QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING)
#define QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED \
  (1 << QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED)
#define QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING \
  (1 << QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING)

#define QBI_SVC_BC_ACTIVATION_STATE_BM_ANY \
  (QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED   | \
   QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATING  | \
   QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED | \
   QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING)

/* Max number and size: to be removed once HSU header defines them */
#define QBI_SVC_BC_PACKET_FILTER_MAX_NUM  (32)
#define QBI_SVC_BC_PACKET_FILTER_MAX_SIZE (128)

/*! Maximum number of provisioned contexts supported in QBI and the modem */
#define QBI_SVC_BC_PROVISIONED_CONTEXTS_MAX (255)

/*! Number of additional characters to separate PLMN name and SPN when both are
    returned. We use the form "PLMN_Name (SPN)", so there are 3. */
#define QBI_SVC_BC_CONCAT_PLMN_SPN_ADDL_CHARS (3)

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! @brief Packet filter handles stored for serialized queries
*/
typedef struct {
  uint32 pos;
  uint32 num;
  uint32 handles[QBI_SVC_BC_PACKET_FILTER_MAX_NUM];
} qbi_svc_bc_packet_filters_handles_info_s;

/*! @brief Profile list containing basic profile info */
typedef struct {
  uint32  num_of_profile;
  uint8_t profile_index[QMI_WDS_PROFILE_LIST_MAX_V01];
} qbi_svc_bc_profile_index_list_s;

/*! @brief Profile list containing basic profile info */
typedef struct {
  uint32                    num_of_profile;
  wds_profile_type_enum_v01 profile_type[QMI_WDS_PROFILE_LIST_MAX_V01];
  uint8_t                   profile_index[QMI_WDS_PROFILE_LIST_MAX_V01];
} qbi_svc_bc_profile_list_s;

/*! Tracking information for retrieving profiles */
typedef struct {
  /*! Total number of EPC, 3gpp and 3gpp2 profiles that have been
      retrieved so far */
  uint32 profiles_read;

  /*! Store EPC profile scan status and matching profile index */
  uint8 profile_found_epc;
  uint8 profile_index_epc;

  /*! Store 3gpp profile scan status and matching profile index */
  uint8 profile_found_3gpp;
  uint8 profile_index_3gpp;
  uint8 is_profile_3gpp_set_to_default;

  /*! Store 3gpp2 profile scan status and matching profile index */
  uint8 profile_found_3gpp2;
  uint8 profile_index_3gpp2;
  uint8 is_profile_3gpp2_set_to_default;

  /*! Store number of profiles from profile list */
  uint8 num_of_profile_epc;
  uint8 num_of_profile_3gpp;
  uint8 num_of_profile_3gpp2;

  /*! Buffer to store profile info from QMI profile list queries. When store
      profiles for multiple profile types, EPC profile will be stored first
      followed by 3GPP then 3GPP2 profiles. */
  qbi_svc_bc_profile_list_s profile_list;

  /*! Store network error code from SNI response */
  uint32 nw_error;
} qbi_svc_bc_connect_profiles_info_s;

/*! Collection of 3gpp settings used for finding matching 3gpp2 profile */
typedef struct {
  uint8_t apn_name_valid;
  char    apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  uint8_t username_valid;
  char    username[QMI_WDS_USER_NAME_MAX_V01 + 1];
} qbi_svc_bc_provisioned_contexts_3gpp_profile_settings_s;

/*! Tracking information for retrieving profiles */
typedef struct {
  /*! Number of profiles that have been retrieved so far */
  uint32 profiles_read;

  /*! Store 3gpp2 profile scan status and matching profile index */
  uint32 profile_found_3gpp2;
  uint32 profile_index_3gpp2;

  /*! 3gpp profile settings for finding matching 3gpp2 profile */
  qbi_svc_bc_provisioned_contexts_3gpp_profile_settings_s profile_settings_3gpp;

  /*! Buffer to store profile index info from last QMI profile list query */
  qbi_svc_bc_profile_list_s profile_list;
} qbi_svc_bc_provisioned_contexts_profiles_info_s;

/*! Collection of pointers to relevant TLVs in
    QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ and QMI_WDS_CREATE_PROFILE_REQ */
typedef struct {
  uint8_t                            *apn_name_valid;
  char                               *apn_name;
  uint8_t                            *user_id_valid;
  char                               *user_id;
  uint8_t                            *auth_password_valid;
  char                               *auth_password;
  uint8_t                            *auth_protocol_valid;
  wds_profile_auth_protocol_enum_v01 *auth_protocol;
  uint8_t                            *authentication_preference_valid;
  wds_auth_pref_mask_v01             *authentication_preference;
  uint8_t                            *app_user_data_valid;
  uint32_t                           *app_user_data;
  uint8_t                            *pdp_data_compression_type_valid;
  wds_pdp_data_compr_type_enum_v01   *pdp_data_compression_type;
  uint8_t                            *pdp_hdr_compression_type_valid;
  wds_pdp_hdr_compr_type_enum_v01    *pdp_hdr_compression_type;
} qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s;

/*! Structure for temp profile creation */
typedef struct {
  uint8_t                            *apn_name_valid;
  char                               *apn_name;
  uint8_t                            *user_id_valid;
  char                               *user_id;
  uint8_t                            *auth_password_valid;
  char                               *auth_password;
  uint8_t                            *auth_protocol_valid;
  wds_profile_auth_protocol_enum_v01 *auth_protocol;
  uint8_t                            *authentication_preference_valid;
  wds_auth_pref_mask_v01             *authentication_preference;
  uint8_t                            *app_user_data_valid;
  uint32_t                           *app_user_data;
  uint8_t                            *pdp_data_compression_type_valid;
  wds_pdp_data_compr_type_enum_v01   *pdp_data_compression_type;
  uint8_t                            *pdp_hdr_compression_type_valid;
  wds_pdp_hdr_compr_type_enum_v01    *pdp_hdr_compression_type;
  uint8_t                            *pdp_type_valid;
  wds_pdp_type_enum_v01              *pdp_type;
  uint8_t                            *persistent_valid;
  uint8_t                            *persistent;
} qbi_svc_bc_temp_profile_settings_ptrs_s;

typedef enum
{
  qbi_svc_bc_pcie_slow_datapath = 0,
  qbi_svc_bc_pcie_fast_datapath
} qbi_svc_bc_pcie_datapath;

#define QBI_SVC_BC_DEFAULT_PCIE_DATAPATH (qbi_svc_bc_pcie_slow_datapath)
/*=============================================================================

  Private Function Prototypes

=============================================================================*/


/*! @note
  Naming convention for QBI device service CID processing functions:
      qbi_svc_<svc>_<cid>_(<e|q|s>_<req|rsp>)|<name>

  Where:
    <svc> Service short name, e.g. bc = basic connectivity, etc.
    <cid> MBIM CID name, e.g. device_caps for MBIM_CID_DEVICE_CAPS
    <e|q|s> One or more of the letters pertaining to the message type
      (event, query, set)
    <req|rsp> req if parsing request InformationBuffer from the host, rsp if
      building response InformationBuffer to send to the host
    <name> If not a req or rsp type function, then a descriptive name of the
      function's purpose (<e|q|s>_<req|rsp> should not be included)

  Examples:
    qbi_svc_bc_radio_state_eqs_build_rsp_from_cache: Common function to build
      response for MBIM_CID_RADIO_STATE set/query response, and unsolicited
      event

    qbi_svc_bc_radio_state_qmi_oprt_mode_to_mbim: Helper function to map a QMI
      operating mode to a corresponding MBIM value

--------------------------------------------------------------------------------

  If the function is a callback for a QMI response or indication, use:
      qbi_svc_<svc>_<cid>_<e|q|s>_<qmi svc><qmi msgid>_<ind|rsp>_cb

  Where:
    <qmi svc> Lowercase QMI service short name, e.g. dms, nas, wds, etc.
    <qmi msgid> Lowercase QMI message ID in hex; 2 digits unless > 0xff
    <ind|rsp> Whether the associated QMI message is an indication or response

  Examples:
    qbi_svc_bc_radio_state_q_dms2d_rsp_cb: QMI_DMS_GET_OPERATING_MODE_RESP
      handler, used by both query MBIM_RADIO_STATE request

    qbi_svc_bc_radio_state_e_dms01_ind_cb: QMI_DMS_EVENT_REPORT_IND handler,
      used to (optionally) generate a MBIM_RADIO_STATE unsolicited event
*/

static boolean qbi_svc_bc_cache_alloc
(
  qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_bc_close
(
  qbi_txn_s *txn
);

uint32 qbi_svc_bc_max_sessions_get
(
  const qbi_ctx_s *ctx
);

static void qbi_svc_bc_max_sessions_set
(
  const qbi_ctx_s *ctx,
  uint32           max_sessions
);

static qbi_svc_action_e qbi_svc_bc_open
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_data_path
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_open_usb_configure_data_path
(
  qbi_txn_s *txn,
  wda_set_data_format_req_msg_v01 *wda01_req
);

static void qbi_svc_bc_open_pcie_configure_data_path
(
  qbi_txn_s *txn,
  wda_set_data_format_req_msg_v01 *wda01_req,
  qbi_svc_bc_pcie_datapath selected_datapath
);

static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_build_wds4d_req
(
  qbi_txn_s    *txn,
  qbi_qmi_svc_e wds_svc_id
);

static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_mpdp
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_open_configure_data_path_mdp_alloc_wds_clids
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_wda01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_wds4d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_wds89_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_wdsa2_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_dms20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_nas34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_get_action
(
  const qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_qmi_inds
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_qmi_inds_nas03_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_qmi_inds_nas6c_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_qmi_inds_uim2e_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_open_configure_refresh_register_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static uint32 qbi_svc_bc_open_configure_qmi_radio_if_list_to_mbim_data_class
(
  const dms_radio_if_enum_v01 *radio_if_list,
  uint32                       radio_if_list_len
);

static void qbi_svc_bc_open_hc_data_format_notify_cb
(
  qbi_txn_s       *listener_txn,
  const qbi_txn_s *notifier_txn
);

static boolean qbi_svc_bc_populate_profile_list
(
  qbi_svc_bc_profile_list_s         *profile_list,
  wds_get_profile_list_resp_msg_v01 *qmi_rsp
);

static uint32 qbi_svc_bc_qmi_plmn_name_to_provider_name_decode
(
  const nas_3gpp_eons_plmn_name_type_v01 *plmn_name,
  uint8                                  *provider_name_utf16,
  uint32                                  provider_name_utf16_len,
  boolean                                 use_spn,
  uint32                                  short_name_threshold
);

static boolean qbi_svc_bc_qmi_plmn_name_to_provider_name_should_concat
(
  const nas_get_plmn_name_resp_msg_v01 *nas44_rsp
);

/*! @addtogroup MBIM_CID_DEVICE_CAPS
    @{ */

static qbi_svc_action_e qbi_svc_bc_device_caps_q_dms22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_device_caps_q_dms23_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_device_caps_q_dms25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_device_caps_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_device_caps_q_rsp
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_RADIO_STATE
    @{ */

static qbi_svc_action_e qbi_svc_bc_radio_state_e_dms01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_bc_radio_state_eqs_build_rsp_from_cache
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_radio_state_qmi_ind_reg
(
  qbi_txn_s *txn,
  boolean    enable_inds
);

static qbi_svc_action_e qbi_svc_bc_radio_state_qmi_ind_reg_dms01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static uint32 qbi_svc_bc_radio_state_qmi_oprt_mode_to_mbim
(
  uint32 qmi_oprt_mode
);

static qbi_svc_action_e qbi_svc_bc_radio_state_q_dms2d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_radio_state_s_completion_cb
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_radio_state_s_dms01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_bc_radio_state_s_dms2e_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_radio_state_update_cache
(
  qbi_ctx_s *ctx,
  uint32     sw_radio_state,
  uint32     hw_radio_state
);

static void qbi_svc_bc_radio_state_update_ftm_flag
(
  const qbi_ctx_s            *ctx,
  dms_operating_mode_enum_v01 oprt_mode
);

/*! @} */

/*! @addtogroup MBIM_CID_CONNECT
    @{ */
static void qbi_svc_bc_connect_disable_loopback_state
(
  qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_bc_connect_e_wds22_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static uint32 qbi_svc_bc_connect_es_get_nw_error
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_connect_es_save_nw_error
(
  qbi_txn_s                           *txn,
  qbi_qmi_svc_e                        qmi_svc_id,
  uint8_t                              vcer_valid,
  wds_verbose_call_end_reason_type_v01 vcer
);

static qbi_svc_action_e qbi_svc_bc_connect_eqs_build_rsp_from_cache
(
  qbi_txn_s *txn,
  uint32     session_id
);

static void qbi_svc_bc_connect_get_failure_status
(
  qbi_txn_s                                      *txn,
  const wds_start_network_interface_resp_msg_v01 *qmi_rsp
);

static void qbi_svc_bc_connect_set_call_end_reason
(
  qbi_txn_s                                      *txn,
  const wds_start_network_interface_resp_msg_v01 *qmi_rsp
);

static void qbi_svc_bc_connect_set_verbose_call_end_reason
(
  qbi_txn_s                                      *txn,
  const wds_start_network_interface_resp_msg_v01 *qmi_rsp
);

static boolean qbi_svc_bc_connect_is_session_connected
(
  const qbi_ctx_s *ctx,
  uint32           session_id
);

static uint32 qbi_svc_bc_connect_connected_ip_type
(
  const qbi_ctx_s *ctx,
  uint32           session_id
);

static qbi_svc_bc_connect_ip_type_state_s *qbi_svc_bc_connect_get_cached_ip_type_state
(
  const qbi_ctx_s *ctx,
  qbi_qmi_svc_e    qmi_svc_id
);

static uint32 qbi_svc_bc_connect_get_ip_type_activation_state
(
  const qbi_ctx_s *ctx,
  qbi_qmi_svc_e    wds_svc_id
);

static wds_ip_family_preference_enum_v01 qbi_svc_bc_connect_svc_id_to_qmi_ip_family_pref
(
  qbi_qmi_svc_e wds_svc_id
);

static uint32 qbi_svc_bc_connect_overall_activation_state
(
  const qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_bc_connect_q_req
(
  qbi_txn_s *txn
);

static uint32 qbi_svc_bc_connect_qmi_conn_status_to_mbim
(
  wds_connection_status_enum_v01 connection_status
);

static qbi_svc_action_e qbi_svc_bc_connect_resync_cache_wds22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_connect_resync_session_cache
(
  qbi_ctx_s *ctx,
  uint32     session_id
);

static boolean qbi_svc_bc_connect_s_build_wds20_req
(
  qbi_txn_s    *txn,
  qbi_qmi_svc_e wds_svc_id
);

static qbi_svc_action_e qbi_svc_bc_connect_build_wds21_req
(
  qbi_txn_s            *txn,
  qbi_qmi_svc_e         wds_svc_id,
  qbi_svc_qmi_rsp_cb_f *rsp_cb
);

static qbi_svc_action_e qbi_svc_bc_connect_es_clean_up_then_send_rsp
(
  qbi_txn_s *txn,
  uint32     session_id
);

static qbi_svc_action_e qbi_svc_bc_connect_es_clean_up_wds21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_connect_is_svc_id_activated
(
  const qbi_ctx_s *ctx,
  qbi_qmi_svc_e    wds_svc_id
);

static void qbi_svc_bc_connect_s_abort
(
  const qbi_txn_s *req_txn
);

static void qbi_svc_bc_connect_s_abort_completion_cb
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_abort_rsp
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_abort_wds02_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_abort_wds20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_activate
(
  qbi_txn_s *txn
);

static uint32 qbi_svc_bc_connect_s_activate_apn_bearer_to_3gpp_data_class
(
  wds_apn_bearer_mask_v01 apn_bearer
);

static void qbi_svc_bc_connect_s_activate_completion_cb
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_activate_get_next_profile
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_build_temp_profile_wds27_req
(
  qbi_txn_s *txn
);


static boolean qbi_svc_bc_temp_profile_populate_type_agnostic_profile_settings
(
  wds_create_profile_req_msg_v01                 *qmi_req,
  qbi_svc_bc_temp_profile_settings_ptrs_s        *profile_settings,
  wds_profile_type_enum_v01                       profile_type
);


static qbi_svc_action_e qbi_svc_bc_temp_profile_populate_profile
(
  qbi_txn_s                                      *txn,
  wds_profile_type_enum_v01                       profile_type,
  qbi_svc_bc_temp_profile_settings_ptrs_s        *profile_settings
);

static boolean qbi_svc_bc_temp_profile_populate_profile_compression
(
  qbi_svc_bc_connect_s_req_s                      *req,
  wds_profile_type_enum_v01                        profile_type,
  qbi_svc_bc_temp_profile_settings_ptrs_s         *profile_settings
);

static boolean qbi_svc_bc_temp_profile_populate_profile_auth_protocol
(
  qbi_txn_s                                         *txn,
  qbi_svc_bc_connect_s_req_s                        *req,
  wds_profile_type_enum_v01                          profile_type,
  qbi_svc_bc_temp_profile_settings_ptrs_s           *profile_settings
);

static boolean qbi_svc_bc_temp_profile_populate_str
(
  qbi_txn_s                                         *txn,
  const qbi_mbim_offset_size_pair_s                 *field_desc,
  uint32                                             field_max_size,
  char                                              *qmi_field,
  uint32                                             qmi_field_size
);

static qbi_svc_action_e qbi_svc_bc_temp_profile_wds27_1_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_temp_profile_wds27_2_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_temp_profile_wds27_3_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_connect_s_activate_is_profile_matched
(
  qbi_qmi_txn_s                         *qmi_txn,
  wds_get_profile_settings_resp_msg_v01 *profile_settings
);

static boolean qbi_svc_bc_connect_s_activate_req_is_loopback
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_activate_wda29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_connect_s_activate_timeout_cb
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_connect_s_deactivate_completion_cb
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_deactivate_build_wds4a_req
(
  qbi_txn_s *txn,
  uint32 session_id
);

static qbi_svc_action_e qbi_svc_bc_connect_s_deactivate_wds22_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_bc_connect_s_enable_loopback_state_then_activate
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_connect_s_force_event
(
  qbi_ctx_s *ctx,
  uint32     session_id
);

static boolean qbi_svc_bc_connect_s_pre_activation_checks
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_connect_s_pre_deactivation_checks
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_connect_s_sanity_check_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_scan_profile_then_activate
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_activate_build_wds49_req
(
  qbi_txn_s *txn,
  wds_profile_type_enum_v01 profile_type
);

static qbi_svc_action_e qbi_svc_bc_connect_s_activate_build_wds2b_req
(
  qbi_txn_s *txn,
  wds_profile_type_enum_v01 profile_type
);

static boolean qbi_svc_bc_connect_s_wds20_rsp_process
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_wds20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_wds21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_connect_s_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

uint32 qbi_svc_bc_connect_session_activation_state
(
  const qbi_ctx_s *ctx,
  uint32           session_id
);

static qbi_svc_action_e qbi_svc_bc_connect_wait_for_teardown
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_connect_wait_for_teardown_wds22_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_bc_connect_wait_for_teardown_wds22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_connect_update_cache_svc_id
(
  qbi_ctx_s    *ctx,
  uint32        activation_state,
  uint32        requested_ip_type,
  const uint8  *context_type,
  boolean       is_resync,
  qbi_qmi_svc_e wds_svc_id
);

static void qbi_svc_bc_connect_update_cache_activating
(
  qbi_ctx_s    *ctx,
  uint32        session_id,
  uint32        requested_ip_type,
  const uint8  *context_type
);

static boolean qbi_svc_bc_connect_update_cache_activation_state
(
  qbi_ctx_s    *ctx,
  uint32        activation_state,
  qbi_qmi_svc_e wds_svc_id
);

static void qbi_svc_bc_connect_update_cache_loopback_state
(
  qbi_ctx_s    *ctx,
  uint32        session_id,
  boolean       loopback_state
);

static void qbi_svc_bc_connect_update_cache_qmi_pkt_handle
(
  qbi_ctx_s    *ctx,
  uint32        qmi_pkt_handle,
  qbi_qmi_svc_e wds_svc_id
);

static boolean qbi_svc_bc_connect_update_cache_resync_svc_id
(
  qbi_ctx_s    *ctx,
  uint32        activation_state,
  qbi_qmi_svc_e wds_svc_id
);

static qbi_svc_action_e qbi_svc_bc_connect_wda29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup MBIM_CID_PROVISIONED_CONTEXTS
    @{ */

static boolean qbi_svc_provisioned_contexts_add_context_to_rsp
(
  qbi_txn_s                                   *txn,
  qbi_mbim_offset_size_pair_s                 *field_desc,
  wds_profile_type_enum_v01                    profile_type,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp,
  uint32                                       context_id
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_build_wds27_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_provisioned_contexts_id_is_valid
(
  uint32 context_id
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_q_get_next_profile
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_q_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_q_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static const uint8 *qbi_svc_bc_provisioned_contexts_qmi_profile_to_context_type
(
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
);

static uint32 qbi_svc_bc_provisioned_contexts_qmi_profile_to_mbim_auth_proto
(
  wds_profile_type_enum_v01                    profile_type,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
);

static uint32 qbi_svc_bc_provisioned_contexts_qmi_profile_to_mbim_compression
(
  wds_profile_type_enum_v01                    profile_type,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_3gpp_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_3gpp2_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_build_3gpp_wds2b_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_build_wds28_req
(
  qbi_txn_s                *txn,
  wds_profile_type_enum_v01 profile_type
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_build_wds2a_req
(
  qbi_txn_s                *txn,
  wds_profile_type_enum_v01 profile_type
);

static boolean qbi_svc_bc_provisioned_contexts_s_compare_3gpp_3gpp2_profiles
(
  qbi_svc_bc_provisioned_contexts_3gpp_profile_settings_s *profile_settings_3gpp,
  wds_get_profile_settings_resp_msg_v01                   *qmi_rsp_3gpp2
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_get_next_3gpp2_profile
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds27
(
  wds_create_profile_req_msg_v01                          *qmi_req,
  qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s *profile_settings,
  wds_profile_type_enum_v01                                profile_type
);

static boolean qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds28
(
  wds_modify_profile_settings_req_msg_v01                 *qmi_req,
  qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s *profile_settings,
  wds_profile_type_enum_v01                                profile_type
);

static boolean qbi_svc_bc_provisioned_contexts_s_populate_str
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *field_desc,
  uint32                             field_max_size,
  char                              *qmi_field,
  uint32                             qmi_field_size
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_populate_profile
(
  qbi_txn_s                                               *txn,
  wds_profile_type_enum_v01                                profile_type,
  qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s *profile_settings
);

static boolean qbi_svc_bc_provisioned_contexts_s_populate_profile_auth_protocol
(
  qbi_txn_s                                               *txn,
  const qbi_svc_bc_provisioned_contexts_s_req_s           *req,
  wds_profile_type_enum_v01                                profile_type,
  qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s *profile_settings
);

static boolean qbi_svc_bc_provisioned_contexts_s_populate_profile_compression
(
   const qbi_svc_bc_provisioned_contexts_s_req_s           *req,
   wds_profile_type_enum_v01                                profile_type,
   qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s *profile_settings
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_req
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_provisioned_contexts_s_save_3gpp_profile_settings
(
  qbi_svc_bc_provisioned_contexts_3gpp_profile_settings_s *profile_settings_3gpp,
  wds_get_profile_settings_resp_msg_v01                   *qmi_rsp
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_wds27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_wds28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_provisioned_contexts_set_mbim_error_status
(
  qbi_txn_s                          *txn,
  qmi_error_type_v01                  qmi_error,
  uint8_t                             qmi_error_ds_ext_valid,
  wds_ds_extended_error_code_enum_v01 qmi_error_ds_ext
);

/*! @} */

/*! @addtogroup MBIM_CID_IP_CONFIGURATION_INFO
    @{ */

static qbi_svc_action_e qbi_svc_bc_ip_configuration_info_e_wds22_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static boolean qbi_svc_bc_ip_configuration_info_eq_build_rsp_ipv4
(
  qbi_txn_s                              *txn,
  qbi_svc_bc_ip_configuration_info_rsp_s *rsp
);

static boolean qbi_svc_bc_ip_configuration_info_eq_build_rsp_ipv6
(
  qbi_txn_s                              *txn,
  qbi_svc_bc_ip_configuration_info_rsp_s *rsp
);

static uint32 qbi_svc_bc_ip_configuration_info_ipv4_subnet_mask_to_prefix_len
(
  uint32 subnet_mask
);

static qbi_svc_action_e qbi_svc_bc_ip_configuration_info_q_wds2d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ip_configuration_info_q_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_DEVICE_SERVICES
    @{ */

static qbi_svc_action_e qbi_svc_bc_device_services_q_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST
    @{ */

static void qbi_svc_bc_device_service_subscribe_list_s_notify_cb
(
  qbi_txn_s       *listener_txn,
  const qbi_txn_s *notifier_txn
);

static qbi_svc_action_e qbi_svc_bc_device_service_subscribe_list_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_STATISTICS
    @{ */

static void qbi_svc_bc_packet_statistics_finalize_session
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e wds_svc_id
);

static qbi_svc_action_e qbi_svc_bc_packet_statistics_finalize_session_wds24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_packet_statistics_q_build_rsp_from_cache
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_packet_statistics_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_packet_statistics_q_wds24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_bc_packet_statistics_sync_current_session
(
  qbi_ctx_s *ctx
);

static boolean qbi_svc_bc_packet_statistics_update_cache_current_session
(
  qbi_ctx_s                                 *ctx,
  const wds_get_pkt_statistics_resp_msg_v01 *qmi_rsp,
  qbi_qmi_svc_e                              qmi_svc_id
);

static void qbi_svc_bc_packet_statistics_update_cache_totals
(
  qbi_ctx_s                                 *ctx,
  const wds_get_pkt_statistics_resp_msg_v01 *qmi_rsp,
  qbi_qmi_svc_e                              qmi_svc_id
);

/*! @} */

/*! @addtogroup MBIM_CID_NETWORK_IDLE_HINT
    @{ */

static qbi_svc_action_e qbi_svc_bc_network_idle_hint_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_network_idle_hint_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_network_idle_hint_s_wds25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_FILTERS
    @{ */

static qbi_svc_action_e qbi_svc_bc_packet_filters_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_req
(
  qbi_txn_s *txn
);

void qbi_svc_bc_wda_client_release
(
  qbi_ctx_s *ctx,
  uint8 action
);

static boolean qbi_svc_bc_packet_filters_q_fill_rule_rsp
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_q_build_rsp_list
(
  qbi_txn_s *txn,
  uint32     pfc
);

static boolean qbi_svc_bc_packet_filters_q_sanity_check_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_q_wda24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_q_wda27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_q_wda28_build_req
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_q_wda28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_disable
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_enable
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_packet_filters_s_sanity_check_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda22_build_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda23_build_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda23_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_packet_filters_s_wda25_build_req
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn,
  uint32         size,
  uint32        *pattern,
  uint32        *mask
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda26_build_req
(
  qbi_txn_s *txn,
  uint32     handle
);

static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda26_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/


/* MBIM_CONTEXT_TYPES UUID values */
static const uint8 qbi_svc_bc_context_types[QBI_SVC_BC_CONTEXT_TYPE_MAX][QBI_MBIM_UUID_LEN] =
{
  /* QBI_SVC_BC_CONTEXT_TYPE_NONE */
  {0xb4, 0x3f, 0x75, 0x8c, 0xa5, 0x60, 0x4b, 0x46,
   0xb3, 0x5e, 0xc5, 0x86, 0x96, 0x41, 0xfb, 0x54},
  /* QBI_SVC_BC_CONTEXT_TYPE_INTERNET */
  {0x7e, 0x5e, 0x2a, 0x7e, 0x4e, 0x6f, 0x72, 0x72,
   0x73, 0x6b, 0x65, 0x6e, 0x7e, 0x5e, 0x2a, 0x7e},
  /* QBI_SVC_BC_CONTEXT_TYPE_VPN */
  {0x9b, 0x9f, 0x7b, 0xbe, 0x89, 0x52, 0x44, 0xb7,
   0x83, 0xac, 0xca, 0x41, 0x31, 0x8d, 0xf7, 0xa0},
  /* QBI_SVC_BC_CONTEXT_TYPE_VOICE */
  {0x88, 0x91, 0x82, 0x94, 0x0e, 0xf4, 0x43, 0x96,
   0x8c, 0xca, 0xa8, 0x58, 0x8f, 0xbc, 0x02, 0xb2},
  /* QBI_SVC_BC_CONTEXT_TYPE_VIDEO_SHARE */
  {0x05, 0xa2, 0xa7, 0x16, 0x7c, 0x34, 0x4b, 0x4d,
   0x9a, 0x91, 0xc5, 0xef, 0x0c, 0x7a, 0xaa, 0xcc},
  /* QBI_SVC_BC_CONTEXT_TYPE_PURCHASE */
  {0xb3, 0x27, 0x24, 0x96, 0xac, 0x6c, 0x42, 0x2b,
   0xa8, 0xc0, 0xac, 0xf6, 0x87, 0xa2, 0x72, 0x17},
  /* QBI_SVC_BC_CONTEXT_TYPE_IMS */
  {0x21, 0x61, 0x0d, 0x01, 0x30, 0x74, 0x4b, 0xce,
   0x94, 0x25, 0xb5, 0x3a, 0x07, 0xd6, 0x97, 0xd6},
  /* QBI_SVC_BC_CONTEXT_TYPE_MMS */
  {0x46, 0x72, 0x66, 0x64, 0x72, 0x69, 0x6b, 0xc6,
   0x96, 0x24, 0xd1, 0xd3, 0x53, 0x89, 0xac, 0xa9},
  /* QBI_SVC_BC_CONTEXT_TYPE_LOCAL */
  {0xa5, 0x7a, 0x9a, 0xfc, 0xb0, 0x9f, 0x45, 0xd7,
   0xbb, 0x40, 0x03, 0x3c, 0x39, 0xf6, 0x0d, 0xb9},
  /* QBI_SVC_BC_CONTEXT_TYPE_CUSTOM */
  {0x9c, 0x49, 0x45, 0x42, 0xa4, 0x3b, 0x4e, 0xa5,
   0xb8, 0xb7, 0x53, 0x31, 0x0e, 0x71, 0xdf, 0x10},
  /* QBI_SVC_BC_CONTEXT_TYPE_ADMIN */
  {0x5f, 0x7e, 0x4c, 0x2e, 0xe8, 0x0b, 0x40, 0xa9,
   0xa2, 0x39, 0xf0, 0xab, 0xcf, 0xd1, 0x1f, 0x4b},
  /* QBI_SVC_BC_CONTEXT_TYPE_APP */
  {0x74, 0xd8, 0x8a, 0x3d, 0xdf, 0xbd, 0x47, 0x99,
   0x9a, 0x8c, 0x73, 0x10, 0xa3, 0x7b, 0xb2, 0xee},
  /* QBI_SVC_BC_CONTEXT_TYPE_XCAP */
  {0x50, 0xd3, 0x78, 0xa7, 0xba, 0xa5, 0x4a, 0x50,
   0xb8, 0x72, 0x3f, 0xe5, 0xbb, 0x46, 0x34, 0x11},
  /* QBI_SVC_BC_CONTEXT_TYPE_TETHERING */
  {0x5e, 0x4e, 0x06, 0x01, 0x48, 0xdc, 0x4e, 0x2b,
   0xac, 0xb8, 0x08, 0xb4, 0x01, 0x6b, 0xba, 0xac},
  /* QBI_SVC_BC_CONTEXT_TYPE_CALLING */
  {0x5f, 0x41, 0xad, 0xb8, 0x20, 0x4e, 0x4d, 0x31,
   0x9d, 0xa8, 0xb3, 0xc9, 0x70, 0xe3, 0x60, 0xf2}
};

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_bc_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_bc_cmd_hdlr_tbl[] = {
  /* MBIM_CID_DEVICE_CAPS */
  {qbi_svc_bc_device_caps_q_req, 0, NULL, 0},
  /* MBIM_CID_SUBSCRIBER_READY_STATUS */
  {qbi_svc_bc_sim_subscriber_ready_status_q_req, 0, NULL, 0},
  /* MBIM_CID_RADIO_STATE */
  {qbi_svc_bc_radio_state_q_req, 0,
   qbi_svc_bc_radio_state_s_req, sizeof(qbi_svc_bc_radio_state_s_req_s)},
  /* MBIM_CID_PIN */
  {qbi_svc_bc_sim_pin_q_req, 0,
   qbi_svc_bc_sim_pin_s_req, sizeof(qbi_svc_bc_pin_s_req_s)},
  /* MBIM_CID_PIN_LIST */
  {qbi_svc_bc_sim_pin_list_q_req, 0, NULL, 0},
  /* MBIM_CID_HOME_PROVIDER */
  {qbi_svc_bc_sim_home_provider_q_req, 0, NULL, 0},
  /* MBIM_CID_PREFERRED_PROVIDERS */
  {qbi_svc_bc_sim_preferred_providers_q_req, 0,
   qbi_svc_bc_sim_preferred_providers_s_req, sizeof(qbi_svc_bc_provider_list_s)},
  /* MBIM_CID_VISIBLE_PROVIDERS */
  {qbi_svc_bc_nas_visible_providers_q_req,
     sizeof(qbi_svc_bc_visible_providers_q_req_s), NULL, 0},
  /* MBIM_CID_REGISTER_STATE */
  {qbi_svc_bc_nas_register_state_q_req, 0,
   qbi_svc_bc_nas_register_state_s_req,
     sizeof(qbi_svc_bc_register_state_s_req_s)},
  /* MBIM_CID_PACKET_SERVICE */
  {qbi_svc_bc_nas_packet_service_q_req, 0,
   qbi_svc_bc_nas_packet_service_s_req,
     sizeof(qbi_svc_bc_packet_service_s_req_s)},
  /* MBIM_CID_SIGNAL_STATE */
  {qbi_svc_bc_nas_signal_state_q_req, 0,
   qbi_svc_bc_nas_signal_state_s_req, sizeof(qbi_svc_bc_signal_state_s_req_s)},
  /* MBIM_CID_CONNECT */
  {qbi_svc_bc_connect_q_req, 0,
   qbi_svc_bc_connect_s_req, sizeof(qbi_svc_bc_connect_s_req_s)},
  /* MBIM_CID_PROVISIONED_CONTEXTS */
  {qbi_svc_bc_provisioned_contexts_q_req, 0,
   qbi_svc_bc_provisioned_contexts_s_req,
     sizeof(qbi_svc_bc_provisioned_contexts_s_req_s)},
  /* MBIM_CID_SERVICE_ACTIVATION */
  {NULL, 0, NULL, 0},
  /* MBIM_CID_IP_CONFIGURATION_INFO */
  {qbi_svc_bc_ip_configuration_info_q_req, 0, NULL, 0},
  /* MBIM_CID_DEVICE_SERVICES */
  {qbi_svc_bc_device_services_q_req, 0, NULL, 0},
  /* Reserved (previously MBIM_CID_OPEN_DEVICE_SERVICE) */
  {NULL, 0, NULL, 0},
  /* Reserved (previously MBIM_CID_CLOSE_DEVICE_SERVICE) */
  {NULL, 0, NULL, 0},
  /* MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST */
  {NULL, 0, qbi_svc_bc_device_service_subscribe_list_s_req,
     sizeof(qbi_svc_bc_device_service_subscribe_list_s_req_s)},
  /* MBIM_CID_PACKET_STATISTICS */
  {qbi_svc_bc_packet_statistics_q_req, 0, NULL, 0},
  /* MBIM_CID_NETWORK_IDLE_HINT */
  {qbi_svc_bc_network_idle_hint_q_req, 0,
   qbi_svc_bc_network_idle_hint_s_req, sizeof(qbi_svc_bc_network_idle_hint_s)},
  /* MBIM_CID_EMERGENCY_MODE */
  {qbi_svc_bc_nas_emergency_mode_q_req, 0, NULL, 0},
  /* MBIM_CID_PACKET_FILTERS */
  {qbi_svc_bc_packet_filters_q_req, sizeof(qbi_svc_bc_packet_filters_s),
   qbi_svc_bc_packet_filters_s_req, sizeof(qbi_svc_bc_packet_filters_s)},
  /* MBIM_CID_MULTICARRIER_PROVIDERS */
  {NULL, 0, NULL, 0}
};

/*! @brief Static QMI indication handlers (generally, CID event handlers)
*/
static const qbi_svc_ind_info_s qbi_svc_bc_static_ind_hdlrs[] = {
  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_PBM, QMI_PBM_PB_READY_IND_V01,
                             QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS,
                             qbi_svc_bc_sim_subscriber_ready_status_e_pbm0b_ind_cb),
  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_UIM, QMI_UIM_STATUS_CHANGE_IND_V01,
                             QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS,
                             qbi_svc_bc_sim_subscriber_ready_status_e_uim2f_ind_cb),
  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_UIM, QMI_UIM_REFRESH_IND_V01,
                             QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS,
                             qbi_svc_bc_sim_subscriber_ready_status_e_uim33_ind_cb),

  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_DMS, QMI_DMS_EVENT_REPORT_IND_V01,
                             QBI_SVC_BC_MBIM_CID_RADIO_STATE,
                             qbi_svc_bc_radio_state_e_dms01_ind_cb),

  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_UIM, QMI_UIM_REFRESH_IND_V01,
                             QBI_SVC_BC_MBIM_CID_PREFERRED_PROVIDERS,
                             qbi_svc_bc_sim_preferred_providers_e_uim33_ind_cb),

  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_NAS,
                             QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01,
                             QBI_SVC_BC_MBIM_CID_REGISTER_STATE,
                             qbi_svc_bc_nas_register_state_e_nas34_ind_cb),
  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_NAS,
                             QMI_NAS_OPERATOR_NAME_DATA_IND_MSG_V01,
                             QBI_SVC_BC_MBIM_CID_REGISTER_STATE,
                             qbi_svc_bc_nas_register_state_e_nas3a_ind_cb),
  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_NAS,
                             QMI_NAS_CSP_PLMN_MODE_BIT_IND_MSG_V01,
                             QBI_SVC_BC_MBIM_CID_REGISTER_STATE,
                             qbi_svc_bc_nas_register_state_e_nas3b_ind_cb),
  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_NAS,
                             QMI_NAS_SYS_INFO_IND_MSG_V01,
                             QBI_SVC_BC_MBIM_CID_REGISTER_STATE,
                             qbi_svc_bc_nas_register_state_e_nas4e_ind_cb),
  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_NAS,
                             QMI_NAS_CURRENT_PLMN_NAME_IND_V01,
                             QBI_SVC_BC_MBIM_CID_REGISTER_STATE,
                             qbi_svc_bc_nas_register_state_e_nas61_ind_cb),

  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_NAS,
                             QMI_NAS_SYS_INFO_IND_MSG_V01,
                             QBI_SVC_BC_MBIM_CID_PACKET_SERVICE,
                             qbi_svc_bc_nas_packet_service_e_nas4e_ind_cb),
  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_WDS,
                             QMI_WDS_EVENT_REPORT_IND_V01,
                             QBI_SVC_BC_MBIM_CID_PACKET_SERVICE,
                             qbi_svc_bc_nas_packet_service_e_wds01_ind_cb),

  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_NAS,
                             QMI_NAS_SIG_INFO_IND_MSG_V01,
                             QBI_SVC_BC_MBIM_CID_SIGNAL_STATE,
                             qbi_svc_bc_nas_signal_state_e_nas51_ind_cb),
  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_NAS,
                             QMI_NAS_ERR_RATE_IND_MSG_V01,
                             QBI_SVC_BC_MBIM_CID_SIGNAL_STATE,
                             qbi_svc_bc_nas_signal_state_e_nas53_ind_cb),

  QBI_SVC_BC_STATIC_IND_HDLR(QBI_QMI_SVC_NAS,
                             QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01,
                             QBI_SVC_BC_MBIM_CID_EMERGENCY_MODE,
                             qbi_svc_bc_nas_emergency_mode_e_nas34_ind_cb)
};

/*! @brief QMI indication (de)registration handlers
*/
static const qbi_svc_qmi_reg_tbl_entry_s qbi_svc_bc_qmi_reg_tbl[] = {
  {QBI_SVC_BC_MBIM_CID_RADIO_STATE, qbi_svc_bc_radio_state_qmi_ind_reg},
  {QBI_SVC_BC_MBIM_CID_REGISTER_STATE, qbi_svc_bc_nas_register_state_qmi_ind_reg},
  {QBI_SVC_BC_MBIM_CID_PACKET_SERVICE, qbi_svc_bc_nas_packet_service_qmi_ind_reg},
  {QBI_SVC_BC_MBIM_CID_SIGNAL_STATE, qbi_svc_bc_nas_signal_state_qmi_ind_reg},
};


/*=============================================================================

  Private Function Definitions

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_svc_bc_cache_alloc
===========================================================================*/
/*!
    @brief Allocates the main Basic Connectivity cache and the BC module
    caches

    @details

    @param ctx

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_cache_alloc
(
  qbi_ctx_s *ctx
)
{
  boolean success = FALSE;
  qbi_svc_bc_module_cache_s *module_cache;
  qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_cache_alloc(ctx, QBI_SVC_ID_BC, sizeof(qbi_svc_bc_cache_s));
  if (cache == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate cache!");
  }
  else
  {
    module_cache = qbi_svc_bc_module_cache_get(ctx);
    if (!qbi_svc_bc_nas_cache_alloc(ctx, module_cache) ||
        !qbi_svc_bc_sim_cache_alloc(module_cache))
    {
      /*! @note If allocating one cache succeeds but another fails, the memory
          will be released when this device service is closed as a part of the
          cleanup procedure */
      QBI_LOG_E_0("Couldn't allocate a module cache!");
    }
    else
    {
      if (!qbi_nv_store_cfg_item_read(
            ctx, QBI_NV_STORE_CFG_ITEM_WDS_CALL_TYPE,
            &cache->connect.call_type, sizeof(cache->connect.call_type)))
      {
        cache->connect.call_type = (uint32) WDS_CALL_TYPE_LAPTOP_CALL_V01;
      }
      success = TRUE;
    }
  }

  return success;
} /* qbi_svc_bc_cache_alloc() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_close
===========================================================================*/
/*!
    @brief Release resources allocated by the device service

    @details

    @param ctx
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_close
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_module_cache_s *module_cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* The framework will free the top-level cache allocated via
     qbi_svc_cache_alloc, but we need to free module caches here. */
  module_cache = qbi_svc_bc_module_cache_get(txn->ctx);
  qbi_svc_bc_nas_cache_free(module_cache);
  qbi_svc_bc_sim_cache_free(module_cache);

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_close() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_max_sessions_get
===========================================================================*/
/*!
    @brief Report maxumim data sessions can be supported

    @details

    @param qbi_ctx_s

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_bc_max_sessions_get
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  return cache->device_caps.max_sessions;
} /* qbi_svc_bc_max_sessions_get */

/*===========================================================================
  FUNCTION: qbi_svc_bc_set_active_datacall_profile_index
===========================================================================*/
/*!
    @brief Initialize the active data profile indexs to default value

    @details

    @param qbi_ctx_s
*/
/*=========================================================================*/
static void qbi_svc_bc_set_active_datacall_profile_index
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_cache_s *cache = NULL;
  uint32 session_id;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  for (session_id = 0; session_id < qbi_svc_bc_max_sessions_get(ctx);
       session_id++)
  {
    {
      cache->connect.sessions[session_id].active_data_profile_3gpp_index =
        QBI_SVC_BC_CONNECT_PROFILE_INDEX_INVALID;
      cache->connect.sessions[session_id].active_data_profile_3gpp2_index =
        QBI_SVC_BC_CONNECT_PROFILE_INDEX_INVALID;
    }
  }
}/* qbi_svc_bc_set_active_datacall_profile_index */

/*===========================================================================
  FUNCTION: qbi_svc_bc_max_sessions_set
===========================================================================*/
/*!
    @brief Save number of maxumim data sessions can be supported in cache

    @details

    @param qbi_ctx_s
    @param max_sessions Maximum data sessions to be supported
*/
/*=========================================================================*/
static void qbi_svc_bc_max_sessions_set
(
  const qbi_ctx_s *ctx,
  uint32           max_sessions
)
{
  qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (max_sessions == 0 || max_sessions > QBI_SVC_BC_MAX_SESSIONS)
  {
    QBI_LOG_E_1("Invalid number of data sesisons %d", max_sessions);
  }
  else if (cache->device_caps.max_sessions != max_sessions)
  {
    cache->device_caps.max_sessions = max_sessions;
  }
} /* qbi_svc_bc_max_sessions_set() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open
===========================================================================*/
/*!
    @brief Runtime intiailization of the Basic Connectivity service

    @details
    This is invoked per-context when the device receives a MBIM_OPEN_MSG.
    It is in charge of performing all runtime initialization so that the
    service can be operational.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_operator_config_s operator_cfg = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_I_0("Processing Basic Connectivity device service open...");
  if (!qbi_svc_bc_cache_alloc(txn->ctx))
  {
    QBI_LOG_E_0("Couldn't allocate cache");
  }
  else if (!qbi_qmi_alloc_svc_handle(
             txn->ctx, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(0)) ||
           !qbi_qmi_alloc_svc_handle(
             txn->ctx, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(0)) ||
           !qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_DMS) ||
           !qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_NAS) ||
           !qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_UIM) ||
           !qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_WDA) ||
           !qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_PBM))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else if (!qbi_svc_ind_reg_static(
             txn->ctx, QBI_SVC_ID_BC, qbi_svc_bc_static_ind_hdlrs,
             ARR_SIZE(qbi_svc_bc_static_ind_hdlrs)))
  {
    QBI_LOG_E_0("Couldn't register QMI indication handlers");
  }
  else if (qbi_hc_data_format_is_configured_by_hc(txn->ctx) &&
           (!qbi_txn_notify_setup_listener(
              txn, qbi_svc_bc_open_hc_data_format_notify_cb) ||
            !qbi_hc_data_format_configure(txn->ctx, txn)))
  {
    QBI_LOG_E_0("Couldn't configure data path");
  }
  else
  {
    if (!qbi_nv_store_cfg_item_read(
         txn->ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
         &operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s)))
    {
      QBI_LOG_D_0("Unable to read operator_config NV. Setting default as NONE");
      operator_cfg.class1_disable = QBI_SVC_BC_EXT_OPERATOR_STATE_NONE;
      operator_cfg.class2_disable = QBI_SVC_BC_EXT_OPERATOR_STATE_NONE;
      if(!qbi_nv_store_cfg_item_write(
         txn->ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
         &operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s)))
      {
        QBI_LOG_D_0("Unable to write operator_config NV.");
      }
    }

    action = (txn->notifier_count > 0) ?
      QBI_SVC_ACTION_WAIT_ASYNC_RSP : qbi_svc_bc_open_configure(txn);
  }

  return action;
} /* qbi_svc_bc_open() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure
===========================================================================*/
/*!
    @brief Initiates all QMI configuration required for MBIM_OPEN

    @details
    Results in dispatching many QMI requests, which all eventually end
    up in qbi_svc_bc_open_configure_get_action().

    @param ctx

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  /* The processing of MBIM_OPEN should be as parallelized as possible to avoid
     unnecessary delay in showing up in the UI. Currently, the following
     dependencies exist:
      - Initialize cache->device_caps.data_class (QMI_DMS_GET_DEVICE_CAP) before
        calling qbi_svc_bc_nas_register_state_sync()
      - Receive QMI_WDS_BIND[_MUX]_DATA_PORT_RESP before sending
        QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_REQ
      - Complete IP family configuration (QMI_WDS_SET_CLIENT_IP_FAMILY_PREF)
        before calling qbi_svc_bc_connect_wait_for_teardown() */
  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_DMS, QMI_DMS_GET_DEVICE_CAP_REQ_V01,
    qbi_svc_bc_open_configure_dms20_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  /* Internal radio state query to ensure cache is initialized early */
  qbi_svc_internal_query(
    qmi_txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_RADIO_STATE);

  /* Hardware radio switch not supported, so initialize it to ON */
  (void) qbi_svc_bc_radio_state_update_cache(
    txn->ctx, QBI_SVC_BC_CACHE_NO_CHANGE_U32, QBI_SVC_BC_RADIO_STATE_RADIO_ON);

  action = qbi_svc_bc_open_configure_data_path(txn);
  if (action != QBI_SVC_ACTION_ABORT)
  {
    action = qbi_svc_bc_open_configure_qmi_inds(txn);
  }

  return action;
} /* qbi_svc_bc_open_configure() */
/*===========================================================================
  FUNCTION: qbi_svc_bc_open_usb_configure_data_path
===========================================================================*/
/*!
    @brief Performs usb data path configuration required for MBIM_OPEN

    @details
    Includes setting data format to MBIM (unless configured by HC layer).

    @param txn
    @param wda01_req

    @return void
*/
/*=========================================================================*/
static void qbi_svc_bc_open_usb_configure_data_path
(
  qbi_txn_s *txn,
  wda_set_data_format_req_msg_v01 *wda01_req
)
{
  wda01_req->qos_format_valid = TRUE;
  wda01_req->qos_format = FALSE;
  wda01_req->link_prot_valid = TRUE;
  wda01_req->link_prot = WDA_LINK_LAYER_IP_MODE_V01;
  wda01_req->dl_data_aggregation_protocol_valid = TRUE;
  wda01_req->dl_data_aggregation_protocol = WDA_DL_DATA_AGG_MBIM_ENABLED_V01;
  wda01_req->ul_data_aggregation_protocol_valid = TRUE;
  wda01_req->ul_data_aggregation_protocol = WDA_UL_DATA_AGG_MBIM_ENABLED_V01;
  wda01_req->ndp_signature_valid = TRUE;
  wda01_req->ndp_signature = QBI_MBIM_NDP16_SIGNATURE;

  wda01_req->dl_data_aggregation_max_size_valid = TRUE;
  wda01_req->dl_data_aggregation_max_size =
    qbi_hc_get_dl_ntb_max_size(txn->ctx);
  wda01_req->dl_data_aggregation_max_datagrams_valid = TRUE;
  wda01_req->dl_data_aggregation_max_datagrams =
    qbi_hc_get_dl_ntb_max_datagrams(txn->ctx);

  wda01_req->ep_id_valid = qbi_hc_dpm_get_data_port_info(
    txn->ctx, &wda01_req->ep_id, NULL);

} /* qbi_svc_bc_open_usb_configure_data_path() */
/*===========================================================================
  FUNCTION: qbi_svc_bc_open_pcie_configure_data_path
===========================================================================*/
/*!
    @brief Performs pcie data path configuration required for MBIM_OPEN

    @details
    Includes setting data format to MBIM (unless configured by HC layer).

    @param txn
    @param wda01_req

    @return void
*/
/*=========================================================================*/
static void qbi_svc_bc_open_pcie_configure_data_path
(
  qbi_txn_s *txn,
  wda_set_data_format_req_msg_v01 *wda01_req,
  qbi_svc_bc_pcie_datapath selected_datapath
)
{
  wda01_req->link_prot_valid = TRUE;
  wda01_req->link_prot = WDA_LINK_LAYER_IP_MODE_V01;

  switch(selected_datapath)
  {
    case qbi_svc_bc_pcie_slow_datapath:
    {
      QBI_LOG_I_0("Enable qbi_svc_bc_pcie_slow_datapath(DATA_AGG_MBIM_ENABLED)");
      wda01_req->dl_data_aggregation_protocol_valid = TRUE;
      wda01_req->dl_data_aggregation_protocol = WDA_DL_DATA_AGG_MBIM_ENABLED_V01;
      wda01_req->ul_data_aggregation_protocol_valid = TRUE;
      wda01_req->ul_data_aggregation_protocol = WDA_UL_DATA_AGG_MBIM_ENABLED_V01;
      wda01_req->ndp_signature_valid = TRUE;
      wda01_req->ndp_signature = QBI_MBIM_NDP16_SIGNATURE;
    }
    break;

    case qbi_svc_bc_pcie_fast_datapath:
    {
      QBI_LOG_I_0("Enable qbi_svc_bc_pcie_fast_datapath(DATA_AGG_QMAP_ENABLED)");
      wda01_req->dl_data_aggregation_protocol_valid = TRUE;
      wda01_req->dl_data_aggregation_protocol = WDS_DL_DATA_AGG_QMAP_ENABLED_V01;
      wda01_req->ul_data_aggregation_protocol_valid = TRUE;
      wda01_req->ul_data_aggregation_protocol = WDS_UL_DATA_AGG_QMAP_ENABLED_V01;
    }
    break;
  }

  wda01_req->dl_data_aggregation_max_size_valid = TRUE;
  wda01_req->dl_data_aggregation_max_size = qbi_hc_get_dl_ntb_max_size(txn->ctx);
  wda01_req->dl_data_aggregation_max_datagrams_valid = TRUE;
  wda01_req->dl_data_aggregation_max_datagrams = qbi_hc_get_dl_ntb_max_datagrams(txn->ctx);

  wda01_req->ep_id_valid = qbi_hc_dpm_get_data_port_info(
    txn->ctx, &wda01_req->ep_id, NULL);

} /* qbi_svc_bc_open_pcie_configure_data_path() */
/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_data_path
===========================================================================*/
/*!
    @brief Performs data path configuration required for MBIM_OPEN

    @details
    Includes setting data format to MBIM (unless configured by HC layer),
    MPDP client ID allocation + binding, IP family configuration, and
    registration of the dynamic WDS indication handlers that span all WDS
    client IDs.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_data_path
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
  wda_set_data_format_req_msg_v01 *wda01_req;
  wds_bind_data_port_req_msg_v01 *wds89_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* Send QMI_WDA_SET_DATA_FORMAT to configure to use MBIM format on the data
     channel, unless the data format was already configured by qbi_hc */
  if (!qbi_hc_data_format_is_configured_by_hc(txn->ctx))
  {
    wda01_req = (wda_set_data_format_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDA, QMI_WDA_SET_DATA_FORMAT_REQ_V01,
        qbi_svc_bc_open_configure_data_path_wda01_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(wda01_req);

    if (qbi_hc_pcie_is_enabled(txn->ctx))
    {
      QBI_LOG_I_0("Creating PCIe configure data path request");
      qbi_svc_bc_open_pcie_configure_data_path(txn, wda01_req, QBI_SVC_BC_DEFAULT_PCIE_DATAPATH);
    }
    else
    {
      QBI_LOG_I_0("Creating USB configure data path request");
      qbi_svc_bc_open_usb_configure_data_path(txn, wda01_req);
    }
  }

  qbi_svc_bc_max_sessions_set(txn->ctx, QBI_SVC_BC_MAX_SESSIONS);
  qbi_svc_bc_set_active_datacall_profile_index(txn->ctx);
  if (qbi_hc_dpm_is_used(txn->ctx))
  {
    /* DPM implies MPDP support */
    action = qbi_svc_bc_open_configure_data_path_mpdp(txn);
  }
  else
  {
    /* Detect MPDP capability by sending a single bind request */
    wds89_req = (wds_bind_data_port_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS_FIRST, QMI_WDS_BIND_DATA_PORT_REQ_V01,
        qbi_svc_bc_open_configure_data_path_wds89_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(wds89_req);
    wds89_req->data_port = qbi_hc_get_bind_data_port(0);
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  if (!qbi_svc_ind_reg_dynamic_qmi_svc_id_range(
        txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_CONNECT,
        QBI_QMI_SVC_WDS_FIRST, QBI_QMI_SVC_WDS_LAST,
        QMI_WDS_PKT_SRVC_STATUS_IND_V01, qbi_svc_bc_connect_e_wds22_ind_cb,
        NULL, NULL) ||
      !qbi_svc_ind_reg_dynamic_qmi_svc_id_range(
        txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_IP_CONFIGURATION_INFO,
        QBI_QMI_SVC_WDS_FIRST, QBI_QMI_SVC_WDS_LAST,
        QMI_WDS_PKT_SRVC_STATUS_IND_V01,
        qbi_svc_bc_ip_configuration_info_e_wds22_ind_cb, NULL, NULL))
  {
    QBI_LOG_E_0("Couldn't register dynamic indication handlers for packet "
                "service status!");
    action = QBI_SVC_ACTION_ABORT;
  }

  return action;
} /* qbi_svc_bc_open_configure_data_path() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_data_path_build_wds4d_req
===========================================================================*/
/*!
    @brief Allocates and populates QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_REQ

    @details

    @param txn
    @param wds_svc_id

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_build_wds4d_req
(
  qbi_txn_s    *txn,
  qbi_qmi_svc_e wds_svc_id
)
{
  wds_set_client_ip_family_pref_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  qmi_req = (wds_set_client_ip_family_pref_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, wds_svc_id, QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_REQ_V01,
      qbi_svc_bc_open_configure_data_path_wds4d_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->ip_preference = QBI_SVC_BC_WDS_SVC_ID_IS_IPV4(wds_svc_id) ?
    WDS_IP_FAMILY_IPV4_V01 : WDS_IP_FAMILY_IPV6_V01;
  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_open_configure_data_path_build_wds4d_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_data_path_mpdp
===========================================================================*/
/*!
    @brief Performs multi-PDP data path configuration, either for DPM (using
    QMI_WDS_BIND_MUX_DATA_PORT) or legacy (using QMI_WDS_BIND_DATA_PORT)

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_mpdp
(
  qbi_txn_s *txn
)
{
  qbi_qmi_svc_e wds_svc_id;
  wds_bind_data_port_req_msg_v01 *wds89_req;
  wds_bind_mux_data_port_req_msg_v01 *wdsa2_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_QMI_REQ;
/*-------------------------------------------------------------------------*/
  if (!qbi_svc_bc_open_configure_data_path_mdp_alloc_wds_clids(txn))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
    action = QBI_SVC_ACTION_ABORT;
  }
  else if (qbi_hc_dpm_is_used(txn->ctx))
  {
    for (wds_svc_id = QBI_QMI_SVC_WDS_FIRST; wds_svc_id <= QBI_QMI_SVC_WDS_LAST;
         wds_svc_id++)
    {
      wdsa2_req = (wds_bind_mux_data_port_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
          txn, wds_svc_id, QMI_WDS_BIND_MUX_DATA_PORT_REQ_V01,
          qbi_svc_bc_open_configure_data_path_wdsa2_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(wdsa2_req);

      wdsa2_req->ep_id_valid = qbi_hc_dpm_get_data_port_info(
        txn->ctx, &wdsa2_req->ep_id, NULL);
      wdsa2_req->mux_id_valid = TRUE;
      wdsa2_req->mux_id = qbi_hc_dpm_get_mux_id(
        txn->ctx, QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(wds_svc_id));
    }
  }
  else
  {
    /* For non-DPM, this function is called only after the first bind succeeds */
    for (wds_svc_id = (qbi_qmi_svc_e) (QBI_QMI_SVC_WDS_FIRST + 1);
         wds_svc_id <= QBI_QMI_SVC_WDS_LAST; wds_svc_id++)
    {
      wds89_req = (wds_bind_data_port_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
          txn, wds_svc_id, QMI_WDS_BIND_DATA_PORT_REQ_V01,
          qbi_svc_bc_open_configure_data_path_wds89_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(wds89_req);
      wds89_req->data_port = qbi_hc_get_bind_data_port(
        QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(wds_svc_id));
    }
  }

  return action;
} /* qbi_svc_bc_open_configure_data_path_mpdp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_data_path_mdp_alloc_wds_clids
===========================================================================*/
/*!
    @brief Allocate QMI WDS clients for secondary PDP sessions

    @details

    @param txn

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_open_configure_data_path_mdp_alloc_wds_clids
(
  qbi_txn_s *txn
)
{
  uint32 session_id;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  for (session_id = 1; session_id < qbi_svc_bc_max_sessions_get(txn->ctx);
       session_id++)
  {
    if (!qbi_qmi_alloc_svc_handle(
          txn->ctx, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id)) ||
        !qbi_qmi_alloc_svc_handle(
          txn->ctx, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id)))
    {
      success = FALSE;
      break;
    }
  }

  return success;
} /* qbi_svc_bc_open_configure_data_path_mdp_alloc_wds_clids() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_data_path_wda01_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDA_SET_DATA_FORMAT_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_wda01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wda_set_data_format_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint32 ntb_max_size;
  uint32 ntb_max_dgrams;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  ntb_max_size = qbi_hc_get_dl_ntb_max_size(qmi_txn->ctx);
  ntb_max_dgrams = qbi_hc_get_dl_ntb_max_datagrams(qmi_txn->ctx);

  qmi_rsp = (wda_set_data_format_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error setting data format!!! Error code %d",
                qmi_rsp->resp.error);
    return action;
  }

  if (
        !qmi_rsp->qos_format_valid || !qmi_rsp->link_prot_valid ||
        !qmi_rsp->dl_data_aggregation_protocol_valid ||
        !qmi_rsp->ul_data_aggregation_protocol_valid ||
        !qmi_rsp->ndp_signature_valid ||
        (ntb_max_dgrams != 0 &&
        !qmi_rsp->dl_data_aggregation_max_datagrams_valid) ||
        (ntb_max_size != 0 &&
        !qmi_rsp->dl_data_aggregation_max_size_valid)
    )
  {
    QBI_LOG_E_0("QMI response missing one or more expected TLVs!");
    return action;
  }

  if (
        qmi_rsp->qos_format != FALSE ||
        qmi_rsp->link_prot != WDA_LINK_LAYER_IP_MODE_V01 ||
        (ntb_max_dgrams != 0 &&
        qmi_rsp->dl_data_aggregation_max_datagrams > ntb_max_dgrams) ||
        (ntb_max_size != 0 &&
        qmi_rsp->dl_data_aggregation_max_size > ntb_max_size)
      )
  {
    QBI_LOG_E_2("QMI response indicates different configuration than "
                "requested! Host requested NTB maximums: %d / %d",
                ntb_max_size, ntb_max_dgrams);
    return action;
  }

  if(qbi_hc_pcie_is_enabled(qmi_txn->ctx))
  {
    switch(QBI_SVC_BC_DEFAULT_PCIE_DATAPATH)
    {
      case qbi_svc_bc_pcie_slow_datapath:
      {
        if(
           qmi_rsp->dl_data_aggregation_protocol != WDA_DL_DATA_AGG_MBIM_ENABLED_V01 ||
           qmi_rsp->ul_data_aggregation_protocol != WDA_UL_DATA_AGG_MBIM_ENABLED_V01 ||
           qmi_rsp->ndp_signature != QBI_MBIM_NDP16_SIGNATURE
          )
        {
          QBI_LOG_E_2("QMI response indicates different configuration than "
                  "requested for qbi_svc_bc_pcie_slow_datapath! "
                  "dl_data_aggregation_protocol/ul_data_aggregation_protocol: %d / %d",
                  qmi_rsp->dl_data_aggregation_protocol, qmi_rsp->ul_data_aggregation_protocol);
          success = FALSE;
        }
        else
        {
            success = TRUE;
        }
      }
      break;

      case qbi_svc_bc_pcie_fast_datapath:
      {
        if(
           qmi_rsp->dl_data_aggregation_protocol != WDS_DL_DATA_AGG_QMAP_ENABLED_V01 ||
           qmi_rsp->ul_data_aggregation_protocol != WDS_UL_DATA_AGG_QMAP_ENABLED_V01
          )
        {
          QBI_LOG_E_2("QMI response indicates different configuration than "
                  "requested for qbi_svc_bc_pcie_fast_datapath! "
                  "dl_data_aggregation_protocol/ul_data_aggregation_protocol: %d / %d",
                  qmi_rsp->dl_data_aggregation_protocol, qmi_rsp->ul_data_aggregation_protocol);
          success = FALSE;
        }
        else
        {
            success = TRUE;
        }
      }
      break;

      default:
      {
        QBI_LOG_E_1("QBI_SVC_BC_DEFAULT_PCIE_DATAPATH set to invalid value %d",
                  QBI_SVC_BC_DEFAULT_PCIE_DATAPATH);
        success = FALSE;
      }
    }
  }
  else
  {
    if(
        qmi_rsp->dl_data_aggregation_protocol != WDA_DL_DATA_AGG_MBIM_ENABLED_V01 ||
        qmi_rsp->ul_data_aggregation_protocol != WDA_UL_DATA_AGG_MBIM_ENABLED_V01 ||
        qmi_rsp->ndp_signature != QBI_MBIM_NDP16_SIGNATURE
      )
    {
        QBI_LOG_E_2("QMI response indicates different configuration than "
                  "requested for USB(non-pcie) path! "
                  "dl_data_aggregation_protocol/ul_data_aggregation_protocol: %d / %d",
                  qmi_rsp->dl_data_aggregation_protocol, qmi_rsp->ul_data_aggregation_protocol);
          success = FALSE;
    }
    else
    {
      success = TRUE;
    }
  }

  if(success)
  {
    QBI_LOG_D_2("Successfully configured data format. "
                "dl_data_aggregation_protocol/ul_data_aggregation_protocol: %d / %d",
                qmi_rsp->dl_data_aggregation_protocol, qmi_rsp->ul_data_aggregation_protocol);
    action = qbi_svc_bc_open_configure_get_action(qmi_txn);
  }

  QBI_LOG_D_0("Releasing WDA client");
  qbi_qmi_release_svc_handle(qmi_txn->ctx, QBI_QMI_SVC_WDA);

  return action;
} /* qbi_svc_bc_open_configure_data_path_wda01_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_data_path_wds4d_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_wds4d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_set_client_ip_family_pref_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_set_client_ip_family_pref_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_2("Error Setting IP Family Preference for WDS Client %d!!! "
                "Error code %d", qmi_txn->svc_id, qmi_rsp->resp.error);
  }
  else
  {
    action = qbi_svc_bc_open_configure_get_action(qmi_txn);
  }

  return action;
} /* qbi_svc_bc_open_configure_data_path_wds4d_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_data_path_wds89_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDS_BIND_DATA_PORT_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_wds89_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_bind_data_port_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_bind_data_port_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Bind failed with error code %d", qmi_rsp->resp.error);
    if (qmi_txn->svc_id == QBI_QMI_SVC_WDS_FIRST &&
        (qmi_rsp->resp.error == QMI_ERR_NOT_SUPPORTED_V01 ||
         qmi_rsp->resp.error == QMI_ERR_INVALID_ARG_V01))
    {
      QBI_LOG_W_0("Falling back to single PDP support");
      qbi_svc_bc_max_sessions_set(qmi_txn->ctx, 1);
      action = qbi_svc_bc_open_configure_data_path_build_wds4d_req(
        qmi_txn->parent, qmi_txn->svc_id);
      if (action == QBI_SVC_ACTION_SEND_QMI_REQ)
      {
        action = qbi_svc_bc_open_configure_data_path_build_wds4d_req(
          qmi_txn->parent, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(0));
      }
    }
  }
  else
  {
    /* Set corresponding IP family preference for each WDS client after bind
       operation. This ordering avoids a race condition on the modem side which
       could potentially reset the IP family preference if both requests are
       sent at the same time. */
    action = qbi_svc_bc_open_configure_data_path_build_wds4d_req(
      qmi_txn->parent, qmi_txn->svc_id);
    if (qmi_txn->svc_id == QBI_QMI_SVC_WDS_FIRST &&
        action == QBI_SVC_ACTION_SEND_QMI_REQ)
    {
      action = qbi_svc_bc_open_configure_data_path_mpdp(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_open_configure_data_path_wds89_rsp_cb */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_data_path_wdsa2_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDS_BIND_MUX_DATA_PORT_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_data_path_wdsa2_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_bind_mux_data_port_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_bind_mux_data_port_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Bind mux port failed with error code %d", qmi_rsp->resp.error);
  }
  else
  {
    action = qbi_svc_bc_open_configure_data_path_build_wds4d_req(
      qmi_txn->parent, qmi_txn->svc_id);
  }

  return action;
} /* qbi_svc_bc_open_configure_data_path_wdsa2_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_dms20_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_DMS_GET_DEVICE_CAP_RESP for
    MBIM_CID_DEVICE_CAPS query

    @details
    Populates the device cap fields in the response.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_dms20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_cache_s *cache;
  dms_get_device_cap_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_qmi_txn_s *nas34_txn;
  uint32 limit_to_band_pref = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_bc_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = (dms_get_device_cap_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    cache->device_caps.data_class =
      qbi_svc_bc_open_configure_qmi_radio_if_list_to_mbim_data_class(
        qmi_rsp->device_capabilities.radio_if_list,
        qmi_rsp->device_capabilities.radio_if_list_len);
    QBI_LOG_I_1("Initialized DataClass to 0x%x", cache->device_caps.data_class);

    if (qbi_nv_store_cfg_item_read(
          qmi_txn->ctx, QBI_NV_STORE_CFG_ITEM_LIMIT_DEV_CAP_TO_BAND_PREF,
          &limit_to_band_pref, sizeof(limit_to_band_pref)) &&
        limit_to_band_pref != 0)
    {
      /* If configured, we need to pick out any RATs in the supported data class
         which do not have any bands enabled in the band preference */
      nas34_txn = qbi_qmi_txn_alloc(
        qmi_txn->parent, QBI_QMI_SVC_NAS,
        QMI_NAS_GET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
        qbi_svc_bc_open_configure_nas34_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(nas34_txn);

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
    else
    {
      action = qbi_svc_bc_open_configure_get_action(qmi_txn);
    }
  }

  return action;
} /* qbi_svc_bc_open_configure_dms20_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_nas34_rsp_cb
===========================================================================*/
/*!
    @brief Removes RATs from the DataClass used in DEVICE_CAPS if all their
    bands are disabled in the current band preference

    @details
    If the appropriate NV configuration item is set, this function will be
    used to ensure that DEVICE_CAPS will only include data classes that are
    enabled in the current band preference. Note that there is no mechanism
    to update the DataClass value reported in DEVICE_CAPS (i.e. there is
    no CID event), but the band preference can be changed on the fly. So
    when this is enabled, it is expected that the band preference will not
    be changed without a device power cycle.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_nas34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uint32 new_data_class;
  qbi_svc_bc_cache_s *cache;
  nas_get_system_selection_preference_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_bc_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  new_data_class = cache->device_caps.data_class;
  qmi_rsp = (nas_get_system_selection_preference_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI - can't update device "
                "capabilities based on band preference", qmi_rsp->resp.error);
  }
  else
  {
    if (!qmi_rsp->band_pref_valid)
    {
      QBI_LOG_W_0("No CHGW band preference info available");
    }
    else
    {
      if ((new_data_class & QBI_SVC_BC_DATA_CLASS_3GPP2_FAMILY) != 0 &&
          (qmi_rsp->band_pref & QBI_QMI_NAS_BAND_PREF_FAMILY_3GPP2) == 0)
      {
        QBI_LOG_I_0("3GPP2 supported but all bands disabled");
        new_data_class &= ~QBI_SVC_BC_DATA_CLASS_3GPP2_FAMILY;
      }
      if ((new_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_GSM) != 0 &&
          (qmi_rsp->band_pref & QBI_QMI_NAS_BAND_PREF_FAMILY_GSM) == 0)
      {
        QBI_LOG_I_0("GSM supported but all bands disabled");
        new_data_class &= ~QBI_SVC_BC_DATA_CLASS_FAMILY_GSM;
      }
      if ((new_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_WCDMA) != 0 &&
          (qmi_rsp->band_pref & QBI_QMI_NAS_BAND_PREF_FAMILY_WCDMA) == 0)
      {
        QBI_LOG_I_0("WCDMA supported but all bands disabled");
        new_data_class &= ~QBI_SVC_BC_DATA_CLASS_FAMILY_WCDMA;
      }
    }

    if (!qmi_rsp->band_pref_ext_valid)
    {
      QBI_LOG_W_0("No LTE band preference info available");
    }
    else if ((new_data_class & QBI_SVC_BC_DATA_CLASS_LTE) != 0 &&
             qmi_rsp->band_pref_ext == 0)
    {
      QBI_LOG_I_0("LTE supported but all bands disabled");
      new_data_class &= ~QBI_SVC_BC_DATA_CLASS_LTE;
    }

    if (!qmi_rsp->tdscdma_band_pref_valid)
    {
      QBI_LOG_W_0("No TD-SCDMA band preference info available");
    }
    else if ((new_data_class & QBI_SVC_BC_DATA_CLASS_CUSTOM) != 0 &&
             qmi_rsp->tdscdma_band_pref == 0)
    {
      QBI_LOG_I_0("TD-SCDMA supported but all bands disabled");
      new_data_class &= ~QBI_SVC_BC_DATA_CLASS_CUSTOM;
    }

    if (new_data_class != cache->device_caps.data_class)
    {
      QBI_LOG_I_2("Reducing DataClass from 0x%x to 0x%x based on band pref",
                  cache->device_caps.data_class, new_data_class);
      cache->device_caps.data_class = new_data_class;
    }
  }

  return qbi_svc_bc_open_configure_get_action(qmi_txn);
} /* qbi_svc_bc_open_configure_nas34_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_get_action
===========================================================================*/
/*!
    @brief Waits for all outstanding QMI transactions to complete, then
    initiates the final stage of open processing

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_get_action
(
  const qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  if (qmi_txn->parent->qmi_txns_pending == 0)
  {
    cache = qbi_svc_bc_cache_get(qmi_txn->ctx);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);

    if((cache->device_caps.data_class & QBI_SVC_BC_DATA_CLASS_3GPP_FAMILY) == 0)
    {
      /* 3GPP2-only modems are no longer supported by QBI */
      QBI_LOG_E_0("No 3GPP profiles found");
    }

    if(qbi_svc_bc_device_is_in_ftm(qmi_txn->ctx))
    {
      QBI_LOG_E_0("Device is in FTM Mode");
    }

    qbi_svc_bc_connect_disable_loopback_state(qmi_txn->ctx);
    qbi_svc_bc_nas_register_state_sync(qmi_txn->ctx);
    action = qbi_svc_bc_connect_wait_for_teardown(qmi_txn->parent);
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_open_configure_get_action() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_qmi_inds
===========================================================================*/
/*!
    @brief Configures QMI indications

    @details
    Other QMI indications will be registered on-demand depending on CID filter
    status (see qbi_svc_bc_qmi_reg_tbl).

    @param ctx

    @return boolean TRUE on success, FALSE otherwise
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_qmi_inds
(
  qbi_txn_s *txn
)
{
  nas_indication_register_req_msg_v01 *nas03_req;
  nas_config_sig_info2_req_msg_v01 *nas6c_req;
  uim_event_reg_req_msg_v01 *uim2e_req;
  uim_refresh_register_req_msg_v01 *uim2a_req;
  uim_refresh_register_req_msg_v01 *uim2a_slot2_req;
/*-------------------------------------------------------------------------*/
  /* Register for QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND and disable
     QMI_NAS_SERVING_SYSTEM_IND */
  nas03_req = (nas_indication_register_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
      qbi_svc_bc_open_configure_qmi_inds_nas03_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(nas03_req);
  nas03_req->reg_sys_sel_pref_valid = TRUE;
  nas03_req->reg_sys_sel_pref = TRUE;
  nas03_req->req_serving_system_valid = TRUE;
  nas03_req->req_serving_system = FALSE;
  nas03_req->sys_info_valid = TRUE;
  nas03_req->sys_info = TRUE;

  /* Register for QMI_UIM_STATUS_CHANGE_IND */
  uim2e_req = (uim_event_reg_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_EVENT_REG_REQ_V01,
      qbi_svc_bc_open_configure_qmi_inds_uim2e_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(uim2e_req);
  uim2e_req->event_mask = (1 << QMI_UIM_EVENT_CARD_STATUS_BIT_V01);
  uim2e_req->event_mask |= (1 << QMI_UIM_EVENT_PHYSICAL_SLOT_STATUS_BIT_V01);

  /* Registering for refresh events */
  uim2a_req = (uim_refresh_register_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_REFRESH_REGISTER_REQ_V01,
      qbi_svc_bc_open_configure_refresh_register_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(uim2a_req);
  uim2a_req->session_information.session_type = UIM_SESSION_TYPE_CARD_ON_SLOT_1_V01;
  uim2a_req->register_refresh.register_flag = TRUE;
  uim2a_req->register_refresh.vote_for_init = FALSE;

  /* Registering for refresh events */
  uim2a_slot2_req = (uim_refresh_register_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_REFRESH_REGISTER_REQ_V01,
      qbi_svc_bc_open_configure_refresh_register_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(uim2a_slot2_req);
  uim2a_slot2_req->session_information.session_type = UIM_SESSION_TYPE_CARD_ON_SLOT_2_V01;
  uim2a_slot2_req->register_refresh.register_flag = TRUE;
  uim2a_slot2_req->register_refresh.vote_for_init = FALSE;

  /*! @note The BC service has a dependency on the PB service registering for
      QMI PBM events for MSISDN readiness. See the note in qbi_svc_pb.c if the
      PB service is to be removed. */

  /* Initialize the QMI_NAS_SIG_INFO_IND reporting thresholds */
  nas6c_req = (nas_config_sig_info2_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_NAS, QMI_NAS_CONFIG_SIG_INFO2_REQ_MSG_V01,
      qbi_svc_bc_open_configure_qmi_inds_nas6c_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(nas6c_req);

  return qbi_svc_bc_nas_signal_state_populate_nas6c_req(txn->ctx, nas6c_req);
} /* qbi_svc_bc_open_configure_qmi_inds() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_refresh_register_cb
===========================================================================*/
/*!
    @brief Handles QMI_UIM_EVENT_REG_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_refresh_register_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_refresh_register_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_refresh_register_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for card refresh indications!!! Error code %d",
                qmi_rsp->resp.error);
  }
  else
  {
    action = qbi_svc_bc_open_configure_get_action(qmi_txn);
  }

  return action;
} /* qbi_svc_bc_open_configure_refresh_register_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_qmi_inds_nas03_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_INDICATION_REGISTER_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_qmi_inds_nas03_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_indication_register_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_indication_register_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for NAS indications!!! Error code %d",
                qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    action = qbi_svc_bc_open_configure_get_action(qmi_txn);
  }

  return action;
} /* qbi_svc_bc_open_configure_qmi_inds_nas03_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_qmi_inds_nas6c_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_INDICATION_REGISTER_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_qmi_inds_nas6c_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_config_sig_info2_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_config_sig_info2_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error configuring SIG_INFO reporting!!! Error code %d",
                qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    action = qbi_svc_bc_open_configure_get_action(qmi_txn);
  }

  return action;
} /* qbi_svc_bc_open_configure_qmi_inds_nas6c_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_qmi_inds_uim2e_cb
===========================================================================*/
/*!
    @brief Handles QMI_UIM_EVENT_REG_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_open_configure_qmi_inds_uim2e_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_event_reg_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_event_reg_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for UIM indications!!! Error code %d",
                qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else if ((qmi_rsp->event_mask_valid) &&
           !((qmi_rsp->event_mask & (1 << QMI_UIM_EVENT_CARD_STATUS_BIT_V01)) && 
             (qmi_rsp->event_mask & (1 << QMI_UIM_EVENT_PHYSICAL_SLOT_STATUS_BIT_V01))))
  {
    QBI_LOG_E_1("QMI responded but we did not get registered to the expected "
                "event! Returned event mask 0x%08x", qmi_rsp->event_mask);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    QBI_LOG_D_2("QMI responded and got registered to the expected "
                  "event! Returned event mask 0x%08x and decimal value : %d", qmi_rsp->event_mask, qmi_rsp->event_mask);
    action = qbi_svc_bc_open_configure_get_action(qmi_txn);
  }

  return action;
} /* qbi_svc_bc_open_configure_qmi_inds_uim2e_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_configure_qmi_radio_if_list_to_mbim_data_class
===========================================================================*/
/*!
    @brief Convert QMI radio interface array to MBIM data class mask

    @details

    @param dms_radio_if_enum_v01
    @param uint32

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_open_configure_qmi_radio_if_list_to_mbim_data_class
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
          data_class |=  QBI_SVC_BC_DATA_CLASS_1XRTT;
          break;

        case DMS_RADIO_IF_1X_EVDO_V01:
          data_class |= (QBI_SVC_BC_DATA_CLASS_1XRTT  |
                         QBI_SVC_BC_DATA_CLASS_1XEVDO |
                         QBI_SVC_BC_DATA_CLASS_1XEVDO_REVA |
                         QBI_SVC_BC_DATA_CLASS_1XEVDO_REVB);
          break;

        case DMS_RADIO_IF_GSM_V01:
          data_class |= (QBI_SVC_BC_DATA_CLASS_GPRS |
                         QBI_SVC_BC_DATA_CLASS_EDGE);
          break;

        case DMS_RADIO_IF_UMTS_V01:
          data_class |= (QBI_SVC_BC_DATA_CLASS_UMTS  |
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
} /* qbi_svc_bc_open_configure_qmi_radio_if_list_to_mbim_data_class() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_open_hc_data_format_notify_cb
===========================================================================*/
/*!
    @brief Notifier callback invoked when async transaction(s) for data
    format configuration by the host communications platform layer complete

    @details

    @param listener_txn
    @param notifier_txn
*/
/*=========================================================================*/
static void qbi_svc_bc_open_hc_data_format_notify_cb
(
  qbi_txn_s       *listener_txn,
  const qbi_txn_s *notifier_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(listener_txn);
  QBI_CHECK_NULL_PTR_RET(notifier_txn);

  QBI_LOG_I_2("Received notify_cb with status %d, notifier_count %d",
              notifier_txn->status, listener_txn->notifier_count);
  if (notifier_txn->status != QBI_MBIM_STATUS_SUCCESS)
  {
    QBI_LOG_E_0("Data format configuration failed");
  }
  else
  {
    action = (listener_txn->notifier_count > 0) ?
      QBI_SVC_ACTION_WAIT_ASYNC_RSP : qbi_svc_bc_open_configure(listener_txn);
  }
  (void) qbi_svc_proc_action(listener_txn, action);
} /* qbi_svc_bc_open_hc_data_format_notify_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_populate_profile_list
===========================================================================*/
/*!
    @brief Process WDS_GET_PROFILE_LIST_RESP to populate profile indexes

    @details
    profile_list may already contain profile indexes from previous profile
    list query. Append new profile indexes to the list.

    @param profile_list
    @param qmi_rsp

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_populate_profile_list
(
  qbi_svc_bc_profile_list_s         *profile_list,
  wds_get_profile_list_resp_msg_v01 *qmi_rsp
)
{
  uint32 i;
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_list);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);

  /* When query all profile types and consolidate profile list, the total
     number of profiles should be within QMI_WDS_PROFILE_LIST_MAX_V01 */
  if (profile_list->num_of_profile + qmi_rsp->profile_list_len >
        QMI_WDS_PROFILE_LIST_MAX_V01)
  {
    QBI_LOG_E_1("Unexpected number of profiles %d",
                profile_list->num_of_profile);
  }
  else
  {
    for (i = 0; i < qmi_rsp->profile_list_len; i++)
    {
      profile_list->profile_type[profile_list->num_of_profile + i] =
        qmi_rsp->profile_list[i].profile_type;
      profile_list->profile_index[profile_list->num_of_profile + i] =
        qmi_rsp->profile_list[i].profile_index;
    }
    profile_list->num_of_profile += qmi_rsp->profile_list_len;
    result = TRUE;
  }

  return result;
} /* qbi_svc_bc_populate_profile_list() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_qmi_plmn_name_to_provider_name_decode
===========================================================================*/
/*!
    @brief Extracts the network name from the network name TLV in
    QMI_NAS_GET_PLMN_NAME_RESP, and decodes it into UTF-16

    @details

    @param plmn_name
    @param provider_name_utf16
    @param provider_name_utf16_len
    @param use_spn Set to TRUE to allow use of the SPN if it's available
    @param short_name_threshold If the long PLMN name is equal to or greater
    than this value in number characters, then the PLMN short name will be
    used. Set to 0 to use the default.

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_qmi_plmn_name_to_provider_name_decode
(
  const nas_3gpp_eons_plmn_name_type_v01 *plmn_name,
  uint8                                  *provider_name_utf16,
  uint32                                  provider_name_utf16_len,
  boolean                                 use_spn,
  uint32                                  short_name_threshold
)
{
  nas_coding_scheme_enum_v01 plmn_name_dcs =
    NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01;
  const char *plmn_name_encoded = NULL;
  uint32 plmn_name_len = 0;
  uint32 bytes_copied = 0;
  char provider_name_ascii[QBI_SVC_BC_PROVIDER_NAME_ASCII_MAX_LEN] = {0,};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(plmn_name);
  QBI_CHECK_NULL_PTR_RET_ZERO(provider_name_utf16);

  if (short_name_threshold == 0)
  {
    short_name_threshold = (QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES / 2);
  }

  QBI_LOG_D_6("spn_len = %u, plmn_long_name_len = %u, plmn_long_name_enc = %d,"
    " plmn_short_name_len = %u, short_name_threshold = %u, use_spn = %d",
    plmn_name->spn_len,
    plmn_name->plmn_long_name_len,
    plmn_name->plmn_long_name_enc,
    plmn_name->plmn_short_name_len,
    short_name_threshold,
    use_spn);

  if (plmn_name->spn_len > 0 &&
      (use_spn ||
       (plmn_name->plmn_long_name_len == 0 &&
        plmn_name->plmn_short_name_len == 0)))
  {
    QBI_LOG_D_0("Using SPN as ProviderName");
    plmn_name_dcs = plmn_name->spn_enc;
    plmn_name_len = plmn_name->spn_len;
    plmn_name_encoded = plmn_name->spn;
  }

  /* During roam, prefer long name over short name, when long name
     length is less than the maximum providername length with
     NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01 as its encoding scheme */
  else if (plmn_name->plmn_long_name_len > 0 &&
           ((plmn_name->plmn_long_name_enc ==
               NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01 &&
             plmn_name->plmn_long_name_len <= (QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES / 2)) ||
            (plmn_name->plmn_long_name_enc == NAS_CODING_SCHEME_UCS2_V01 &&
             plmn_name->plmn_long_name_len <= (short_name_threshold * 2)) ||
             plmn_name->plmn_short_name_len == 0))
  {
    QBI_LOG_D_0("Using long name as ProviderName");
    plmn_name_dcs = plmn_name->plmn_long_name_enc;
    plmn_name_len = plmn_name->plmn_long_name_len;
    plmn_name_encoded = plmn_name->plmn_long_name;
  }
  else if (plmn_name->plmn_short_name_len > 0)
  {
    QBI_LOG_D_0("Using short name as ProviderName");
    plmn_name_dcs = plmn_name->plmn_short_name_enc;
    plmn_name_len = plmn_name->plmn_short_name_len;
    plmn_name_encoded = plmn_name->plmn_short_name;
  }
  else
  {
    QBI_LOG_E_0("All ProviderName sources empty!");
  }

  if (plmn_name_encoded != NULL)
  {
    if (plmn_name_dcs == NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01)
    {
      if (plmn_name_len >= sizeof(provider_name_ascii))
      {
        QBI_LOG_W_2("ProviderName will be truncated from %d to %d characters",
                    plmn_name_len, (sizeof(provider_name_ascii) - 1));
        bytes_copied = sizeof(provider_name_ascii) - 1;
      }
      else
      {
        bytes_copied = plmn_name_len;
      }
      /* Convert GSM default alphabet (unpacked) to ASCII, and then ASCII to
         UTF-16 */
      QBI_MEMSCPY(provider_name_ascii, sizeof(provider_name_ascii),
                  plmn_name_encoded, bytes_copied);
      (void) qbi_util_convert_gsm_alphabet_to_ascii(
        (uint8 *) provider_name_ascii, bytes_copied);
      bytes_copied = qbi_util_ascii_to_utf16(
        provider_name_ascii, bytes_copied,
        provider_name_utf16, provider_name_utf16_len);
    }
    else if (plmn_name_dcs == NAS_CODING_SCHEME_UCS2_V01)
    {
      /* UCS2 can just be copied directly to the UTF-16 buffer */
      if (plmn_name_len > provider_name_utf16_len)
      {
        QBI_LOG_W_2("ProviderName will be truncated from %d to %d UTF-16 "
                    "bytes", plmn_name_len, provider_name_utf16_len);
        bytes_copied = provider_name_utf16_len;
      }
      else
      {
        bytes_copied = plmn_name_len;
      }
      QBI_MEMSCPY(provider_name_utf16, provider_name_utf16_len,
                  plmn_name_encoded, bytes_copied);
      QBI_LOG_D_1("Copied %d bytes of UCS2 encoded network name",
                  bytes_copied);
    }
    else
    {
      QBI_LOG_E_1("Unrecognized encoding scheme %d", plmn_name_dcs);
    }
  }

  return bytes_copied;
} /* qbi_svc_bc_qmi_plmn_name_to_provider_name_decode() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_qmi_plmn_name_to_provider_name_should_concat
===========================================================================*/
/*!
    @brief Checks whether the conditions are right to return both the PLMN
    name and SPN per 3GPP TS 22.101 A.4

    @details

    @param nas44_rsp

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_qmi_plmn_name_to_provider_name_should_concat
(
  const nas_get_plmn_name_resp_msg_v01 *nas44_rsp
)
{
/*-------------------------------------------------------------------------*/
  /* If both SPN and PLMN name are available, we will use both if:
    - We are on a home network and PLMN display bit is set, OR
    - We are not on a home network and SPN display bit is not set */
  return (nas44_rsp != NULL && nas44_rsp->eons_display_bit_info_valid &&
          nas44_rsp->is_home_network_valid &&
          nas44_rsp->eons_plmn_name_3gpp.spn_len > 0 &&
          (nas44_rsp->eons_plmn_name_3gpp.plmn_long_name_len > 0 ||
           nas44_rsp->eons_plmn_name_3gpp.plmn_short_name_len > 0) &&
          ((nas44_rsp->is_home_network == NAS_TRI_TRUE_V01 &&
            nas44_rsp->eons_display_bit_info.is_plmn_set == NAS_TRI_TRUE_V01) ||
           (nas44_rsp->is_home_network == NAS_TRI_FALSE_V01 &&
            nas44_rsp->eons_display_bit_info.is_spn_set == NAS_TRI_FALSE_V01)));
} /* qbi_svc_bc_qmi_plmn_name_to_provider_name_should_concat() */

/*! @addtogroup MBIM_CID_DEVICE_CAPS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_caps_q_dms22_rsp_cb
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
static qbi_svc_action_e qbi_svc_bc_device_caps_q_dms22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  dms_get_device_model_id_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_device_caps_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = (qbi_svc_bc_device_caps_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_rsp = (dms_get_device_model_id_resp_msg_v01 *) qmi_txn->rsp.data;
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
      action = qbi_svc_bc_device_caps_q_rsp(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_device_caps_q_dms22_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_caps_q_dms23_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_DMS_GET_DEVICE_REV_ID_RESP for
    MBIM_CID_DEVICE_CAPS query

    @details
    Populates the FirmwareInfo field in the response.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_device_caps_q_dms23_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  dms_get_device_rev_id_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_device_caps_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint32 offset;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = (qbi_svc_bc_device_caps_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_rsp = (dms_get_device_rev_id_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    /* The revision ID returned by QMI contains the boot block version and
       release date. These won't fit in the response, so truncate after the
       first space so that we only report the build ID */
    for (offset = 0; offset < sizeof(qmi_rsp->device_rev_id) &&
           qmi_rsp->device_rev_id[offset] != QBI_UTIL_ASCII_NULL; offset++)
    {
      if (qmi_rsp->device_rev_id[offset] == ' ')
      {
        qmi_rsp->device_rev_id[offset] = QBI_UTIL_ASCII_NULL;
        break;
      }
    }

    if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
           qmi_txn->parent, &rsp->fw_info, 0, QBI_SVC_BC_FW_INFO_MAX_LEN_BYTES,
           qmi_rsp->device_rev_id, sizeof(qmi_rsp->device_rev_id)))
    {
      QBI_LOG_E_0("Couldn't add firmware information to response!");
    }
    else
    {
      action = qbi_svc_bc_device_caps_q_rsp(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_device_caps_q_dms23_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_caps_q_dms25_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_RESP for
    MBIM_CID_DEVICE_CAPS query

    @details
    This QMI response is used to populate the DeviceId field in the
    response. This is not considered a mandatory field, so failures in
    this function will not cause the response contain an error status.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_device_caps_q_dms25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  dms_get_device_serial_numbers_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_device_caps_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = (qbi_svc_bc_device_caps_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_rsp = (dms_get_device_serial_numbers_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (qmi_rsp->imei_valid &&
           !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
             qmi_txn->parent, &rsp->device_id, 0,
             QBI_SVC_BC_DEVICE_ID_MAX_LEN_BYTES,
             qmi_rsp->imei, sizeof(qmi_rsp->imei)))
  {
    QBI_LOG_E_0("Couldn't populate DeviceId in response!");
  }
  else
  {
    /* If the IMEI TLV is missing, then we assume the device is not provisioned,
       and we don't include a DeviceId in the response (rather than failing the
       entire query) */
    if (!qmi_rsp->imei_valid)
    {
      QBI_LOG_W_0("Leaving DeviceId field blank: IMEI not provisioned");
    }
    action = qbi_svc_bc_device_caps_q_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_device_caps_q_dms25_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_caps_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_DEVICE_CAPS query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_device_caps_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_device_caps_rsp_s *rsp;
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_bc_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  qbi_txn_req_buf_ignored(txn);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_device_caps_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->voice_class       = QBI_SVC_BC_VOICE_CLASS_NO_VOICE;
  rsp->sms_caps          = (QBI_SVC_BC_SMS_CAPS_PDU_SEND |
                            QBI_SVC_BC_SMS_CAPS_PDU_RECEIVE);
  rsp->sim_class         = QBI_SVC_BC_SIM_CLASS_REMOVABLE;
  rsp->max_sessions      = qbi_svc_bc_max_sessions_get(txn->ctx);
  rsp->data_class        = cache->device_caps.data_class;
  rsp->cellular_class    = QBI_SVC_BC_CELLULAR_CLASS_GSM;
  rsp->ctrl_caps         = QBI_SVC_BC_CTRL_CAPS_REG_MANUAL;

  if (qbi_svc_bc_device_supports_3gpp2(txn->ctx))
  {
    rsp->cellular_class |= QBI_SVC_BC_CELLULAR_CLASS_CDMA;
    rsp->ctrl_caps      |= (QBI_SVC_BC_CTRL_CAPS_CDMA_SIMPLE_IP |
                            QBI_SVC_BC_CTRL_CAPS_CDMA_MOBILE_IP);
  }

  /* DeviceType given from NV configuration item */
  if (!qbi_nv_store_cfg_item_read(
        txn->ctx, QBI_NV_STORE_CFG_ITEM_DEVICE_TYPE, (void *) &rsp->device_type,
        sizeof(rsp->device_type)))
  {
    rsp->device_type = QBI_SVC_BC_DEVICE_TYPE_UNKNOWN;
  }

  if ((rsp->data_class & QBI_SVC_BC_DATA_CLASS_CUSTOM) != 0 &&
      !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
        txn, &rsp->cust_data_class, 0, QBI_SVC_BC_CUST_DATA_CLASS_MAX_LEN_BYTES,
        QBI_SVC_BC_CUSTOM_DATA_CLASS_NAME_ASCII,
        sizeof(QBI_SVC_BC_CUSTOM_DATA_CLASS_NAME_ASCII)))
  {
    QBI_LOG_E_0("Couldn't add CustomDataClass string to response");
  }
  else
  {
    /* QMI_DMS_GET_DEVICE_SERIAL_NUMBERS (0x25) */
    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_DMS,
                                QMI_DMS_GET_DEVICE_SERIAL_NUMBERS_REQ_V01,
                                qbi_svc_bc_device_caps_q_dms25_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    /* QMI_DMS_GET_DEVICE_REV_ID (0x23) */
    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_DMS,
                                QMI_DMS_GET_DEVICE_REV_ID_REQ_V01,
                                qbi_svc_bc_device_caps_q_dms23_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    /* QMI_DMS_GET_DEVICE_MODEL_ID (0x22) */
    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_DMS,
                                QMI_DMS_GET_DEVICE_MODEL_ID_REQ_V01,
                                qbi_svc_bc_device_caps_q_dms22_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_device_caps_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_caps_q_rsp
===========================================================================*/
/*!
    @brief Perform query response processing for MBIM_CID_DEVICE_CAPS

    @details
    Checks whether we have the information required to send the response.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_device_caps_q_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
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
} /* qbi_svc_bc_device_caps_q_rsp() */

/*! @} */

/*! @addtogroup MBIM_CID_RADIO_STATE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_e_dms01_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_DMS_EVENT_REPORT_IND, looking for operating mode
    changes to trigger a MBIM_CID_RADIO_STATE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_radio_state_e_dms01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const dms_event_report_ind_msg_v01 *qmi_ind;
  qbi_svc_bc_cache_s *cache;
  uint32 new_sw_radio_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  cache = qbi_svc_bc_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_ind = (const dms_event_report_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->operating_mode_valid)
  {
    new_sw_radio_state =
      qbi_svc_bc_radio_state_qmi_oprt_mode_to_mbim(qmi_ind->operating_mode);
    if (qbi_svc_bc_radio_state_update_cache(
          ind->txn->ctx, new_sw_radio_state, QBI_SVC_BC_CACHE_NO_CHANGE_U32))
    {
      action = qbi_svc_bc_radio_state_eqs_build_rsp_from_cache(ind->txn);

      if (new_sw_radio_state == QBI_SVC_BC_RADIO_STATE_RADIO_ON)
      {
        qbi_svc_bc_nas_register_state_sync(ind->txn->ctx);
      }
    }
    else
    {
      QBI_LOG_I_2("Operating mode changed to %d, but still maps to SwRadioState"
                  " %d", qmi_ind->operating_mode, new_sw_radio_state);
    }
    qbi_svc_bc_radio_state_update_ftm_flag(
      ind->txn->ctx, qmi_ind->operating_mode);
  }

  return action;
} /* qbi_svc_bc_radio_state_e_dms01_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_eqs_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Allocates & populates the response/indication InformationBuffer
    for MBIM_CID_RADIO_STATE based on the values currently in the cache

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_radio_state_eqs_build_rsp_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_cache_s *cache;
  qbi_svc_bc_radio_state_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  rsp = (qbi_svc_bc_radio_state_rsp_s *)
    qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_radio_state_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp->sw_radio_state = cache->radio_state.sw_radio_state;
  rsp->hw_radio_state = cache->radio_state.hw_radio_state;

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_radio_state_eqs_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_RADIO_STATE query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_radio_state_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  /* QMI_DMS_GET_OPERATING_MODE (0x2d) */
  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_DMS,
                              QMI_DMS_GET_OPERATING_MODE_REQ_V01,
                              qbi_svc_bc_radio_state_q_dms2d_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_radio_state_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_qmi_ind_reg
===========================================================================*/
/*!
    @brief Performs QMI (de)registration for the QMI_DMS_EVENT_REPORT_IND
    used by MBIM_CID_RADIO_STATE

    @details

    @param txn
    @param enable_inds

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_radio_state_qmi_ind_reg
(
  qbi_txn_s *txn,
  boolean    enable_inds
)
{
  qbi_svc_action_e action;
  dms_set_event_report_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (qbi_svc_ind_qmi_ref_cnt_update_then_check_if_significant(
        txn->ctx, QBI_QMI_SVC_DMS, QMI_DMS_EVENT_REPORT_IND_V01, enable_inds))
  {
    qmi_req = (dms_set_event_report_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_DMS, QMI_DMS_SET_EVENT_REPORT_REQ_V01,
        qbi_svc_bc_radio_state_qmi_ind_reg_dms01_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
    qmi_req->report_oprt_mode_state_valid = TRUE;
    qmi_req->report_oprt_mode_state = enable_inds;
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_bc_radio_state_qmi_ind_reg() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_qmi_ind_reg_dms01_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_DMS_SET_EVENT_REPORT_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_radio_state_qmi_ind_reg_dms01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action;
  dms_set_event_report_req_msg_v01 *qmi_req;
  dms_set_event_report_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (dms_set_event_report_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (dms_set_event_report_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error (de)registering for DMS indications!!! Error code %d",
                qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  /* Refresh the cache if we are enabling indications */
  if (qmi_req->report_oprt_mode_state == TRUE)
  {
    qbi_svc_internal_query(
      qmi_txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_RADIO_STATE);
  }

  return action;
} /* qbi_svc_bc_radio_state_qmi_ind_reg_dms01_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_qmi_oprt_mode_to_mbim
===========================================================================*/
/*!
    @brief Map a QMI operating mode value to MBIM software radio switch state

    @details

    @param qmi_oprt_mode

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_radio_state_qmi_oprt_mode_to_mbim
(
  uint32 qmi_oprt_mode
)
{
/*-------------------------------------------------------------------------*/
  return (qmi_oprt_mode == DMS_OP_MODE_ONLINE_V01) ?
           QBI_SVC_BC_RADIO_STATE_RADIO_ON :
           QBI_SVC_BC_RADIO_STATE_RADIO_OFF;
} /* qbi_svc_bc_radio_state_qmi_oprt_mode_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_q_dms2d_rsp_cb
===========================================================================*/
/*!
    @brief Process QMI_DMS_GET_OPERATING_MODE_RESP for MBIM_CID_RADIO_STATE query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_radio_state_q_dms2d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  dms_get_operating_mode_resp_msg_v01 *qmi_rsp;
  uint32 sw_radio_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (dms_get_operating_mode_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    if (qmi_rsp->resp.error == QMI_ERR_INFO_UNAVAILABLE_V01)
    {
      qmi_txn->parent->status = QBI_MBIM_STATUS_NOT_INITIALIZED;
    }
    else
    {
      qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
    }
  }
  else
  {
    sw_radio_state = qbi_svc_bc_radio_state_qmi_oprt_mode_to_mbim(
      qmi_rsp->operating_mode);
    (void) qbi_svc_bc_radio_state_update_cache(
      qmi_txn->ctx, sw_radio_state, QBI_SVC_BC_CACHE_NO_CHANGE_U32);
    qbi_svc_bc_radio_state_update_ftm_flag(
      qmi_txn->ctx, qmi_rsp->operating_mode);
  }

  return qbi_svc_bc_radio_state_eqs_build_rsp_from_cache(qmi_txn->parent);
} /* qbi_svc_bc_radio_state_q_dms2d_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_s_completion_cb
===========================================================================*/
/*!
    @brief MBIM_CID_RADIO_STATE set operation completion callback

    @details
    Used to notify the QMI registration callback that this transaction does
    not need the QMI operating mode indication anymore.

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_radio_state_s_completion_cb
(
  const qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  qbi_svc_ind_qmi_reg_invoke(txn->ctx, txn->svc_id, txn->cid, FALSE, NULL);
} /* qbi_svc_bc_radio_state_s_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_s_dms01_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_DMS_EVENT_REPORT_IND, looking for an operating mode
    change to the mode requested by MBIM_CID_RADIO_STATE set request

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_radio_state_s_dms01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  const dms_event_report_ind_msg_v01 *qmi_ind;
  uint32 new_sw_radio_state;
  qbi_svc_bc_radio_state_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  req = (qbi_svc_bc_radio_state_s_req_s *) ind->txn->req.data;
  qmi_ind = (const dms_event_report_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->operating_mode_valid)
  {
    new_sw_radio_state =
      qbi_svc_bc_radio_state_qmi_oprt_mode_to_mbim(qmi_ind->operating_mode);
    if (req->radio_state == new_sw_radio_state)
    {
      QBI_LOG_I_1("Request for radio state %d now complete - sending response",
                  new_sw_radio_state);
      (void) qbi_svc_bc_radio_state_update_cache(
        ind->txn->ctx, new_sw_radio_state, QBI_SVC_BC_CACHE_NO_CHANGE_U32);
      action = qbi_svc_bc_radio_state_eqs_build_rsp_from_cache(ind->txn);
    }
    else
    {
      QBI_LOG_I_3("Received new operating mode indication (%d) but RadioState "
                  "%d does not match requested %d", qmi_ind->operating_mode,
                  req->radio_state, new_sw_radio_state);
    }
  }

  return action;
} /* qbi_svc_bc_radio_state_s_dms01_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_s_dms2e_rsp_cb
===========================================================================*/
/*!
    @brief Process QMI_DMS_SET_OPERATING_MODE_RESP for MBIM_CID_RADIO_STATE
    set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_radio_state_s_dms2e_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  dms_set_operating_mode_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (dms_set_operating_mode_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if (qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
    {
      QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
      qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
    }
    action = qbi_svc_bc_radio_state_eqs_build_rsp_from_cache(qmi_txn->parent);
  }
  else
  {
    /* QMI returned success - hold off on sending the response to the host until
       we receive a QMI DMS operating mode indication with the requested mode */
    if (!qbi_svc_ind_reg_dynamic(qmi_txn->ctx, QBI_SVC_ID_BC,
                                 QBI_SVC_BC_MBIM_CID_RADIO_STATE,
                                 QBI_QMI_SVC_DMS, QMI_DMS_EVENT_REPORT_IND_V01,
                                 qbi_svc_bc_radio_state_s_dms01_ind_cb,
                                 qmi_txn->parent, NULL))
    {
      QBI_LOG_E_0("Couldn't register dynamic indication handler!");
    }
    else
    {
      QBI_LOG_I_1("Result of RADIO_STATE set operation (txn iid %d) pending on "
                  "indication with requested mode", qmi_txn->parent->iid);
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
  }

  return action;
} /* qbi_svc_bc_radio_state_s_dms2e_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_RADIO_STATE set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_radio_state_s_req
(
  qbi_txn_s *txn
)
{
  dms_set_operating_mode_req_msg_v01 *qmi_req;
  qbi_svc_bc_radio_state_s_req_s *req;
  qbi_svc_bc_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_QMI_REQ;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  req = (qbi_svc_bc_radio_state_s_req_s *) txn->req.data;
  if (cache->radio_state.sw_radio_state == req->radio_state)
  {
    QBI_LOG_W_1("Requested radio state %d matches current radio state!",
                req->radio_state);
    action = qbi_svc_bc_radio_state_eqs_build_rsp_from_cache(txn);
  }
  else if (cache->radio_state.ftm_enabled)
  {
    QBI_LOG_E_0("Blocking radio state set request due to FTM");
    txn->status = QBI_MBIM_STATUS_OPERATION_NOT_ALLOWED;
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    /* We will need the operating mode indication, so ensure it is registered
       for the duration of this transaction */
    qbi_svc_ind_qmi_reg_invoke(txn->ctx, txn->svc_id, txn->cid, TRUE, NULL);
    qbi_txn_set_completion_cb(txn, qbi_svc_bc_radio_state_s_completion_cb);

    qmi_req = (dms_set_operating_mode_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_DMS,
                                    QMI_DMS_SET_OPERATING_MODE_REQ_V01,
                                    qbi_svc_bc_radio_state_s_dms2e_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
    qbi_txn_set_timeout(txn, QBI_SVC_BC_RADIO_STATE_SET_TIMEOUT_MS, NULL);

    if (req->radio_state == QBI_SVC_BC_RADIO_STATE_RADIO_OFF)
    {
      qmi_req->operating_mode = DMS_OP_MODE_LOW_POWER_V01;
    }
    else if (req->radio_state == QBI_SVC_BC_RADIO_STATE_RADIO_ON)
    {
      qmi_req->operating_mode = DMS_OP_MODE_ONLINE_V01;
    }
    else
    {
      QBI_LOG_E_1("Unrecognized requested radio state %d", req->radio_state);
      txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }

  return action;
} /* qbi_svc_bc_radio_state_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_update_cache
===========================================================================*/
/*!
    @brief Updates the local cache of radio state information

    @details

    @param ctx
    @param sw_radio_state New software radio state, or
    QBI_SVC_BC_CACHE_NO_CHANGE_U32 to keep previous value
    @param hw_radio_state New hardware radio state, or
    QBI_SVC_BC_CACHE_NO_CHANGE_U32 to keep previous value

    @return boolean TRUE if the cached values changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_radio_state_update_cache
(
  qbi_ctx_s *ctx,
  uint32     sw_radio_state,
  uint32     hw_radio_state
)
{
  qbi_svc_bc_cache_s *cache;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (sw_radio_state != QBI_SVC_BC_CACHE_NO_CHANGE_U32 &&
      cache->radio_state.sw_radio_state != sw_radio_state)
  {
    QBI_LOG_I_2("SwRadioState changed from %d to %d",
                cache->radio_state.sw_radio_state, sw_radio_state);
    cache->radio_state.sw_radio_state = sw_radio_state;
    changed = TRUE;
    /* The radio state is used to help determine the current register state
       (searching vs. deregistered). Force an event to ensure it takes the new
       radio state into account. */
    qbi_svc_force_event(ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_REGISTER_STATE);
  }

  if (hw_radio_state != QBI_SVC_BC_CACHE_NO_CHANGE_U32 &&
      cache->radio_state.hw_radio_state != hw_radio_state)
  {
    QBI_LOG_I_2("HwRadioState changed from %d to %d",
                cache->radio_state.hw_radio_state, hw_radio_state);
    cache->radio_state.hw_radio_state = hw_radio_state;
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_bc_radio_state_update_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_update_ftm_flag
===========================================================================*/
/*!
    @brief Updates the flag in cache that indicates whether the current
    operating mode is FTM

    @details

    @param ctx
    @param oprt_mode
*/
/*=========================================================================*/
static void qbi_svc_bc_radio_state_update_ftm_flag
(
  const qbi_ctx_s            *ctx,
  dms_operating_mode_enum_v01 oprt_mode
)
{
  qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (oprt_mode == DMS_OP_MODE_FACTORY_TEST_MODE_V01)
  {
    QBI_LOG_W_0("Device is in factory test mode");
    cache->radio_state.ftm_enabled = TRUE;
  }
  else
  {
    cache->radio_state.ftm_enabled = FALSE;
  }
} /* qbi_svc_bc_radio_state_update_ftm_flag() */

/*! @} */

/*! @addtogroup MBIM_CID_CONNECT
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_disable_loopback_state
===========================================================================*/
/*!
    @brief Disable WDA loopback state

    @details

    @param ctx
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_disable_loopback_state
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
  wda_set_loopback_state_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/

  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_BC_MBIM_CID_CONNECT, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_WDA))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else
  {
    qmi_req = (wda_set_loopback_state_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDA, QMI_WDA_SET_LOOPBACK_STATE_REQ_V01,
        qbi_svc_bc_connect_wda29_rsp_cb);

    if (qmi_req == NULL)
    {
      QBI_LOG_E_0("Unexpected NULL pointer!");
    }
    else
    {
      qmi_req->loopback_state = FALSE;
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  (void) qbi_svc_proc_action(txn, action);

  if (QBI_SVC_ACTION_ABORT == action)
  {
    QBI_LOG_D_0("Releasing WDA client");
    qbi_qmi_release_svc_handle(ctx, QBI_QMI_SVC_WDA);
  }
} /* qbi_svc_bc_connect_disable_loopback_state */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_e_wds22_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_PKT_SRVC_STATUS_IND, looking for changes to the
    context ActivationState or IP Type that would trigger a MBIM_CID_CONNECT
    event.

    @details For QMI_WDS revision 1.35 and newer, QMI_WDS_PKT_SRVC_STATUS_IND
    has been changed from broadcast to unicast.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_e_wds22_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const wds_pkt_srvc_status_ind_msg_v01 *qmi_ind;
  uint32 new_activation_state;
  qbi_qmi_svc_e wds_svc_id;
  uint32 session_id;
  qbi_svc_bc_connect_ip_type_state_s *ip_type_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const wds_pkt_srvc_status_ind_msg_v01 *) ind->buf->data;
  if (ind->qmi_svc_id < QBI_QMI_SVC_WDS_FIRST ||
      ind->qmi_svc_id > QBI_QMI_SVC_WDS_LAST)
  {
    QBI_LOG_E_1("qmi_svc_id %d is out of range", ind->qmi_svc_id);
  }
  else
  {
    wds_svc_id = ind->qmi_svc_id;
    session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(wds_svc_id);

    ip_type_state = qbi_svc_bc_connect_get_cached_ip_type_state(
      ind->txn->ctx, wds_svc_id);
    QBI_CHECK_NULL_PTR_RET_ABORT(ip_type_state);

    /* We are only interested in sending an event from here when moving from
       ACTIVATED directly to DEACTIVATED, i.e. when the connection is lost and
       there was no explicit request to tear it down. */
    new_activation_state = qbi_svc_bc_connect_qmi_conn_status_to_mbim(
      qmi_ind->status.connection_status);
    if (new_activation_state == QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED &&
        ip_type_state->activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
    {
      QBI_LOG_I_1("Connection on qmi_svc_id %d ended due to modem/network",
                  wds_svc_id);
      (void) qbi_svc_bc_connect_update_cache_activation_state(
        ind->txn->ctx, new_activation_state, wds_svc_id);

      if (qbi_svc_bc_connect_session_activation_state(ind->txn->ctx, session_id) !=
            QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
      {
        qbi_svc_bc_connect_es_save_nw_error(
          ind->txn, wds_svc_id, qmi_ind->verbose_call_end_reason_valid,
          qmi_ind->verbose_call_end_reason);

        /* Ensure that both IP types are fully disconnected before sending
           the event - if the host requested IPv4AndIPv6, and the
           connection on only one IP type was lost, then we need to
           manually tear down the other. */
        action = qbi_svc_bc_connect_es_clean_up_then_send_rsp(
          ind->txn, session_id);
      }
      else
      {
        /*! @note If the connection on both IP types is ended by the
            modem/network then we will end up sending two CONNECT events:
            the first one indicating loss of connectivity on a single IP
            type, and the second indication total loss of connectivity.
            This is not entirely unavoidable, however - a short timer could
            ensure that only one event gets sent when both IP types go
            down in rapid succession. */
        QBI_LOG_I_0("Other IP type still active");
        action = qbi_svc_bc_connect_eqs_build_rsp_from_cache(
          ind->txn, session_id);
      }

      /* Restore default profile */
      if (QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED == 
        qbi_svc_bc_connect_session_activation_state(ind->txn->ctx, session_id))
      {
        QBI_LOG_I_0("Modem ended the call. Restore default profiles");
        action = qbi_svc_bc_connect_s_deactivate_build_wds4a_req(
          ind->txn, session_id);
      }
    }
  }

  return action;
} /* qbi_svc_bc_connect_e_wds22_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_es_get_nw_error
===========================================================================*/
/*!
    @brief Fetches the network error saved to the transaction's info field,
    if available

    @details

    @param txn

    @return uint32 3GPP-defined network error, or 0 if none
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_connect_es_get_nw_error
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_profiles_info_s *info = NULL;
  uint32 nw_error = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(txn);

  if (txn->info != NULL &&
      (txn->cmd_type == QBI_MSG_CMD_TYPE_SET ||
       txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT))
  {
    info = (qbi_svc_bc_connect_profiles_info_s *)txn->info;
    nw_error = info->nw_error;
  }

  return nw_error;
} /* qbi_svc_bc_connect_es_get_nw_error() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_es_save_nw_error
===========================================================================*/
/*!
    @brief Saves the network error from QMI WDS to the information field of
    a set or event transaction

    @details

    @param txn
    @param qmi_svc_id
    @param vcer_valid Whether the verbose call end reason information is
    available
    @param vcer Verbose call end reason from QMI WDS
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_es_save_nw_error
(
  qbi_txn_s                           *txn,
  qbi_qmi_svc_e                        qmi_svc_id,
  uint8_t                              vcer_valid,
  wds_verbose_call_end_reason_type_v01 vcer
)
{
  qbi_svc_bc_connect_profiles_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->info);

  if (vcer_valid &&
      vcer.call_end_reason_type == WDS_VCER_TYPE_3GPP_SPEC_DEFINED_V01)
  {
    info = (qbi_svc_bc_connect_profiles_info_s *)txn->info;

    if (QBI_SVC_BC_WDS_SVC_ID_IS_IPV6(qmi_svc_id))
    {
      QBI_LOG_I_2("Received 3GPP-defined call end reason %d on IPv6 instance, "
                  "but using existing IPv4 error %d",
                  vcer.call_end_reason, info->nw_error);
    }
    else
    {
      info->nw_error = vcer.call_end_reason;
      QBI_LOG_I_1("Using 3GPP-defined call end reason %d", info->nw_error);
    }
  }
} /* qbi_svc_bc_connect_es_save_nw_error() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_eqs_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Allocates and populates a MBIM_CID_CONNECT query/set response
    or event message based on the current values in the cache and the
    given network error

    @details

    @param txn
    @param session_id

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_eqs_build_rsp_from_cache
(
  qbi_txn_s *txn,
  uint32     session_id
)
{
  qbi_svc_bc_cache_s *cache;
  qbi_svc_bc_connect_rsp_s *rsp;
  const uint8 *context_type;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* Clear the completion callback, since we are sending a known response */
  qbi_txn_set_completion_cb(txn, NULL);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_bc_connect_rsp_s *)
    qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_connect_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->voice_call_state = QBI_SVC_BC_VOICE_CALL_STATE_NONE;
  rsp->activation_state =
    qbi_svc_bc_connect_session_activation_state(txn->ctx, session_id);
  rsp->nw_error = qbi_svc_bc_connect_es_get_nw_error(txn);

  /* Don't allow transient states in SET response. */
  if (txn->cmd_type == QBI_MSG_CMD_TYPE_SET &&
      (rsp->activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING ||
       rsp->activation_state == QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING))
  {
    rsp->activation_state = QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED;
  }

  if (rsp->activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING)
  {
    rsp->ip_type = cache->connect.sessions[session_id].requested_ip_type;
  }
  else if (rsp->activation_state != QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED)
  {
    rsp->ip_type = qbi_svc_bc_connect_connected_ip_type(txn->ctx, session_id);
  }

  /* Don't include the cached session ID and context type if we are reporting
     the state as deactivated in a query response; otherwise, include it */
  if (txn->cmd_type != QBI_MSG_CMD_TYPE_QUERY ||
      rsp->activation_state != QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED)
  {
    rsp->session_id = session_id;
    QBI_MEMSCPY(rsp->context_type, sizeof(rsp->context_type),
                cache->connect.sessions[session_id].context_type,
                sizeof(rsp->context_type));
  }
  else
  {
    context_type = qbi_svc_bc_context_type_id_to_uuid(
      QBI_SVC_BC_CONTEXT_TYPE_NONE);
    QBI_CHECK_NULL_PTR_RET_ABORT(context_type);

    QBI_MEMSCPY(rsp->context_type, sizeof(rsp->context_type),
                context_type, sizeof(rsp->context_type));
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_connect_eqs_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_get_failure_status
===========================================================================*/
/*!
    @brief Attempts to populate the transaction's status field with a more
    descriptive error code describing why activation failed, e.g.
    QBI_MBIM_STATUS_INVALID_USER_NAME_PWD

    @details

    @param txn
    @param qmi_rsp
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_get_failure_status
(
  qbi_txn_s                                      *txn,
  const wds_start_network_interface_resp_msg_v01 *qmi_rsp
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(qmi_rsp);

  /* Start out with STATUS_FAILURE, and try to get more descriptive */
  txn->status = QBI_MBIM_STATUS_FAILURE;

  if (qmi_rsp->call_end_reason_valid)
  {
    qbi_svc_bc_connect_set_call_end_reason(txn, qmi_rsp);
  }

  /* error handling for verbose call end reason*/
  if (qmi_rsp->verbose_call_end_reason_valid)
  {
    qbi_svc_bc_connect_set_verbose_call_end_reason(txn, qmi_rsp);
  }

} /* qbi_svc_bc_connect_get_failure_status() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_set_call_end_reason
===========================================================================*/
/*!
    @brief

    @details

    @param txn
    @param qmi_rsp
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_set_call_end_reason
(
  qbi_txn_s                                      *txn,
  const wds_start_network_interface_resp_msg_v01 *qmi_rsp
)
{
/*-------------------------------------------------------------------------*/
  switch (qmi_rsp->call_end_reason)
  {
    case WDS_CER_AUTH_FAILED_V01:
      txn->status = QBI_MBIM_STATUS_INVALID_USER_NAME_PWD;
      break;
    case WDS_CER_OPTION_UNSUBSCRIBED_V01:
      txn->status = QBI_MBIM_STATUS_SERVICE_NOT_ACTIVATED;
      break;
    default:
      txn->status = QBI_MBIM_STATUS_FAILURE;
      break;
  }

  QBI_LOG_I_2("QMI call end reason %d mapped to %d",qmi_rsp->call_end_reason,txn->status);

} /* qbi_svc_bc_connect_set_call_end_reason */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_set_verbose_call_end_reason
===========================================================================*/
/*!
    @brief

    @details

    @param txn
    @param qmi_rsp
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_set_verbose_call_end_reason
(
  qbi_txn_s                                      *txn,
  const wds_start_network_interface_resp_msg_v01 *qmi_rsp
)
{
/*-------------------------------------------------------------------------*/

  if (qmi_rsp->verbose_call_end_reason.call_end_reason_type ==
        WDS_VCER_TYPE_3GPP_SPEC_DEFINED_V01)
  {
    switch (qmi_rsp->verbose_call_end_reason.call_end_reason)
    {
      case WDS_VCER_3GPP_AUTH_FAILED_V01:
        QBI_LOG_I_0("Activation failed due to 3GPP authentication failure");
        txn->status = QBI_MBIM_STATUS_INVALID_USER_NAME_PWD;
        break;

      case WDS_VCER_3GPP_UNKNOWN_APN_V01:
        QBI_LOG_I_0("Activation failed due to unknown APN");
        txn->status = QBI_MBIM_STATUS_INVALID_ACCESS_STRING;
        break;

      case WDS_VCER_3GPP_OPTION_UNSUBSCRIBED_V01:
          QBI_LOG_I_0("Activation failed due to service or subscription inactive");
          txn->status = QBI_MBIM_STATUS_SERVICE_NOT_ACTIVATED;
          break;

      default:
        txn->status = QBI_MBIM_STATUS_FAILURE;
        QBI_LOG_I_0("verbose call end reason not matched");
        break;
    }

    QBI_LOG_I_2("QMI verbose call end reason %d mapped to %d",
      qmi_rsp->verbose_call_end_reason.call_end_reason,txn->status);
  }
  else if (qmi_rsp->verbose_call_end_reason.call_end_reason_type ==
             WDS_VCER_TYPE_PPP_V01)
  {
    switch (qmi_rsp->verbose_call_end_reason.call_end_reason)
    {
      case WDS_VCER_PPP_AUTH_FAILURE_V01:
      case WDS_VCER_PPP_PAP_FAILURE_V01:
      case WDS_VCER_PPP_CHAP_FAILURE_V01:
        QBI_LOG_I_1("Activation failed due to PPP authentication failure %d",
                    qmi_rsp->verbose_call_end_reason.call_end_reason);
        txn->status = QBI_MBIM_STATUS_INVALID_USER_NAME_PWD;
        break;
     default:
        QBI_LOG_I_0("verbose call end reason not matched");
        break;
    }

    QBI_LOG_I_2("QMI verbose call end reason %d mapped to %d",
      qmi_rsp->verbose_call_end_reason.call_end_reason,txn->status);
  }
} /* qbi_svc_bc_connect_set_verbose_call_end_reason() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_is_session_connected
===========================================================================*/
/*!
    @brief Checks whether the device has an active connection up

    @details

    @param ctx

    @return boolean TRUE if connected, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_is_session_connected
(
  const qbi_ctx_s *ctx,
  uint32           session_id
)
{
  uint32 activation_state;
/*-------------------------------------------------------------------------*/
  activation_state = qbi_svc_bc_connect_session_activation_state(
    ctx, session_id);
  return (activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED ||
          activation_state == QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING);
} /* qbi_svc_bc_connect_is_session_connected() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_get_cached_ip_type_state
===========================================================================*/
/*!
    @brief Returns a pointer to IP type state located within the cache

    @details

    @param ctx
    @param wds_svc_id

    @return qbi_svc_bc_connect_ip_type_state_s*
*/
/*=========================================================================*/
static qbi_svc_bc_connect_ip_type_state_s *qbi_svc_bc_connect_get_cached_ip_type_state
(
  const qbi_ctx_s *ctx,
  qbi_qmi_svc_e    qmi_svc_id
)
{
  qbi_svc_bc_cache_s *cache;
  qbi_svc_bc_connect_ip_type_state_s *state = NULL;
  uint32 session_id;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(cache);

  session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_svc_id);
  if (!QBI_SVC_BC_WDS_SVC_ID_IS_IPV6(qmi_svc_id))
  {
    state = &cache->connect.sessions[session_id].ipv4;
  }
  else
  {
    state = &cache->connect.sessions[session_id].ipv6;
  }

  return state;
} /* qbi_svc_bc_connect_get_cached_ip_type_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_get_ip_type_activation_state
===========================================================================*/
/*!
    @brief Gets the current cached activation state of the given IP type,
    identified by QMI WDS service ID

    @details

    @param ctx
    @param wds_svc_id

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_connect_get_ip_type_activation_state
(
  const qbi_ctx_s *ctx,
  qbi_qmi_svc_e    wds_svc_id
)
{
  uint32 activation_state = QBI_SVC_BC_ACTIVATION_STATE_UNKNOWN;
  qbi_svc_bc_connect_ip_type_state_s *ip_type_state = NULL;
/*-------------------------------------------------------------------------*/
  ip_type_state = qbi_svc_bc_connect_get_cached_ip_type_state(ctx, wds_svc_id);
  if (ip_type_state == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
  }
  else
  {
    activation_state = ip_type_state->activation_state;
  }

  return activation_state;
} /* qbi_svc_bc_connect_get_ip_type_activation_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_connected_ip_type
===========================================================================*/
/*!
    @brief Gets the MBIM IP type of the active connection

    @details

    @param ctx
    @param session_id

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_connect_connected_ip_type
(
  const qbi_ctx_s *ctx,
  uint32           session_id
)
{
  uint32 ip_type = 0;
/*-------------------------------------------------------------------------*/
  if (qbi_svc_bc_connect_is_session_ipv4_activated(ctx, session_id))
  {
    ip_type = QBI_SVC_BC_IP_TYPE_IPV4;
  }

  if (qbi_svc_bc_connect_is_session_ipv6_activated(ctx, session_id))
  {
    ip_type |= QBI_SVC_BC_IP_TYPE_IPV6;
  }

  if (ip_type == (QBI_SVC_BC_IP_TYPE_IPV4 | QBI_SVC_BC_IP_TYPE_IPV6))
  {
    ip_type = QBI_SVC_BC_IP_TYPE_IPV4V6;
  }

  return ip_type;
} /* qbi_svc_bc_connect_connected_ip_type() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_svc_id_to_qmi_ip_family_pref
===========================================================================*/
/*!
    @brief Maps a WDS SVC ID to QMI IP type (ipv4/ipv6)

    @param wds_svc_id

    @return wds_ip_family_preference_enum_v01
*/
/*=========================================================================*/
static wds_ip_family_preference_enum_v01 qbi_svc_bc_connect_svc_id_to_qmi_ip_family_pref
(
  qbi_qmi_svc_e wds_svc_id
)
{
  wds_ip_family_preference_enum_v01 ip_family;
/*-------------------------------------------------------------------------*/
  if (wds_svc_id >= QBI_QMI_SVC_WDS_FIRST && wds_svc_id <= QBI_QMI_SVC_WDS_LAST)
  {
    if (QBI_SVC_BC_WDS_SVC_ID_IS_IPV4(wds_svc_id))
    {
      ip_family = WDS_IP_FAMILY_PREF_IPV4_V01;
    }
    else
    {
      ip_family = WDS_IP_FAMILY_PREF_IPV6_V01;
    }
  }
  else
  {
    QBI_LOG_E_1("Unexpected QMI service type %d", wds_svc_id);
    ip_family = WDS_IP_FAMILY_PREF_IPV4_V01;
  }

  return ip_family;
} /* qbi_svc_bc_connect_svc_id_to_qmi_ip_family_pref() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_overall_activation_state
===========================================================================*/
/*!
    @brief Combines the current cached activation states of the IPV4 and
    IPV6 WDS instances into a single overall MBIM activation state value

    @details

    @param ctx

    @return uint32 MBIM activation state
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_connect_overall_activation_state
(
  const qbi_ctx_s *ctx
)
{
  uint32 activation_state;
  uint32 session_id;
  boolean found_activated_session = FALSE;
  boolean found_activating_session = FALSE;
  boolean found_deactivating_session = FALSE;
  boolean found_deactivated_session = FALSE;
/*-------------------------------------------------------------------------*/
  for (session_id = 0; session_id < qbi_svc_bc_max_sessions_get(ctx);
       session_id++)
  {
    activation_state = qbi_svc_bc_connect_session_activation_state(
      ctx, session_id);

    if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
    {
      found_activated_session = TRUE;
    }
    else if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING)
    {
      found_activating_session = TRUE;
    }
    else if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING)
    {
      found_deactivating_session = TRUE;
    }
    else if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED)
    {
      found_deactivated_session = TRUE;
    }
  }

  if (found_activated_session)
  {
    activation_state = QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED;
  }
  else if (found_activating_session)
  {
    activation_state = QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING;
  }
  else if (found_deactivating_session)
  {
    activation_state = QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING;
  }
  else if (found_deactivated_session)
  {
    activation_state = QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED;
  }
  else
  {
    activation_state = QBI_SVC_BC_ACTIVATION_STATE_UNKNOWN;
  }

  return activation_state;
} /* qbi_svc_bc_connect_overall_activation_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_CONNECT query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_q_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_connect_q_req_s *) txn->req.data;
  if (req->session_id >= qbi_svc_bc_max_sessions_get(txn->ctx))
  {
    QBI_LOG_E_2("Requested SessionId (%d) is past maximum (%d)",
                req->session_id, (qbi_svc_bc_max_sessions_get(txn->ctx) - 1));
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else
  {
    action = qbi_svc_bc_connect_eqs_build_rsp_from_cache(txn, req->session_id);
  }

  return action;
} /* qbi_svc_bc_connect_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_qmi_conn_status_to_mbim
===========================================================================*/
/*!
    @brief Converts a QMI connection status to a MBIM context activation
    state

    @details

    @param status

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_connect_qmi_conn_status_to_mbim
(
  wds_connection_status_enum_v01 connection_status
)
{
  uint32 activation_state;
/*-------------------------------------------------------------------------*/
  switch (connection_status)
  {
    case WDS_CONNECTION_STATUS_DISCONNECTED_V01:
      activation_state = QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED;
      break;

    case WDS_CONNECTION_STATUS_CONNECTED_V01:
      activation_state = QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED;
      break;

    case WDS_CONNECTION_STATUS_AUTHENTICATING_V01:
      activation_state = QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING;
      break;

    default:
      QBI_LOG_E_1("Unhandled connection status %d", connection_status);
      activation_state = QBI_SVC_BC_ACTIVATION_STATE_UNKNOWN;
  }

  return activation_state;
} /* qbi_svc_bc_connect_qmi_conn_status_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_resync_cache_wds22_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PKT_SRVC_STATUS_RESP for a CONNECT cache
    resync operation

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_resync_cache_wds22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_pkt_srvc_status_resp_msg_v01 *qmi_rsp;
  uint32 activation_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (wds_get_pkt_srvc_status_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    activation_state = qbi_svc_bc_connect_qmi_conn_status_to_mbim(
      qmi_rsp->connection_status);
    if (qbi_svc_bc_connect_update_cache_resync_svc_id(
          qmi_txn->ctx, activation_state, qmi_txn->svc_id))
    {
      QBI_LOG_W_0("Connect cache resync resulted in change to activation "
                  "state!");
    }
    else
    {
      QBI_LOG_I_1("Cache resync resulted in activation state staying at %d",
                  activation_state);
    }
  }

  return ((qmi_txn->parent->qmi_txns_pending > 0) ?
            QBI_SVC_ACTION_WAIT_ASYNC_RSP : QBI_SVC_ACTION_ABORT);
} /* qbi_svc_bc_connect_resync_cache_wds22_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_resync_session_cache
===========================================================================*/
/*!
    @brief Resync the cached session activation state with what is actually
    reported by QMI

    @details
    Results in dispatching an internal transaction to query the current
    status from QMI and update the cache with the response

    @param ctx
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_resync_session_cache
(
  qbi_ctx_s *ctx,
  uint32     session_id
)
{
  qbi_txn_s *txn;
  qbi_qmi_txn_s *qmi_txn_ipv4;
  qbi_qmi_txn_s *qmi_txn_ipv6;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_1("Resynchronizing CONNECT cache for session %d", session_id);

  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_BC_MBIM_CID_CONNECT, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  /* QMI_WDS_GET_PKT_SRVC_STATUS (0x22) */
  qmi_txn_ipv4 = qbi_qmi_txn_alloc(
    txn, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id),
    QMI_WDS_GET_PKT_SRVC_STATUS_REQ_V01,
    qbi_svc_bc_connect_resync_cache_wds22_rsp_cb);
  qmi_txn_ipv6 = qbi_qmi_txn_alloc(
    txn, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id),
    QMI_WDS_GET_PKT_SRVC_STATUS_REQ_V01,
    qbi_svc_bc_connect_resync_cache_wds22_rsp_cb);

  if (qmi_txn_ipv4 == NULL || qmi_txn_ipv6 == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate QMI transaction!");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_connect_resync_session_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_build_wds20_req
===========================================================================*/
/*!
    @brief Builds a QMI_WDS_START_NETWORK_INTERFACE_REQ for a given WDS
    service ID to activate a context based on a MBIM_CID_CONNECT set request

    @details

    @param txn
    @param wds_svc_id

    @return TRUE if no error
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_s_build_wds20_req
(
  qbi_txn_s    *txn,
  qbi_qmi_svc_e wds_svc_id
)
{
  qbi_svc_bc_connect_s_req_s *req;
  wds_start_network_interface_req_msg_v01 *qmi_req;
  const uint8 *field;
  uint32 bytes_copied;
  qbi_svc_bc_connect_profiles_info_s *info = NULL;
  uint32 current_cellular_class;
  const qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  req = (qbi_svc_bc_connect_s_req_s *) txn->req.data;
  QBI_LOG_I_1("Activating new context with session ID %d", req->session_id);

  qmi_req = (wds_start_network_interface_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, wds_svc_id, QMI_WDS_START_NETWORK_INTERFACE_REQ_V01,
      qbi_svc_bc_connect_s_wds20_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_req);

  /* Specify IP stream ID associated with the data call which is used for
     data stream muxing. */
  qmi_req->ips_id_valid = TRUE;
  qmi_req->ips_id = (uint8) req->session_id;

  /* Note that the requested IP type should already be sanitized by
     qbi_svc_bc_connect_s_pre_activation_checks() */
  qmi_req->ip_family_preference_valid = TRUE;
  qmi_req->ip_family_preference =
    qbi_svc_bc_connect_svc_id_to_qmi_ip_family_pref(wds_svc_id);

  qmi_req->call_type_valid = TRUE;
  qmi_req->call_type = (wds_call_type_enum_v01) cache->connect.call_type;

  /* If EPC profile found, use only EPC profile, otherwise fallback to
     legacy method of using profiles found with profile type matching
     current network type (e.g. 3GPP profile found when current network is
     3GPP, or 3GPP2 profile found when current network is 3GPP2), activate
     IPv4/IPv6 data sessions with 3GPP/3GPP2 profile ID TLV(s) for found
     profile(s). Use override TLVs when there is no matching profile. */
  current_cellular_class =
    qbi_svc_bc_nas_register_state_get_current_cellular_class(txn->ctx);
  info = (qbi_svc_bc_connect_profiles_info_s *) txn->info;
  if (info &&
      (info->profile_found_epc ||
       (info->profile_found_3gpp &&
        current_cellular_class == QBI_SVC_BC_CELLULAR_CLASS_GSM) ||
       (info->profile_found_3gpp2 &&
        current_cellular_class == QBI_SVC_BC_CELLULAR_CLASS_CDMA)))
  {
    if (info->profile_found_epc)
    {
      qmi_req->profile_index_valid = TRUE;
      qmi_req->profile_index = info->profile_index_epc;
    }
    else
    {
      if (info->profile_found_3gpp)
      {
        qmi_req->profile_index_valid = TRUE;
        qmi_req->profile_index = info->profile_index_3gpp;
      }

      if (info->profile_found_3gpp2)
      {
        qmi_req->profile_index_3gpp2_valid = TRUE;
        qmi_req->profile_index_3gpp2 = info->profile_index_3gpp2;
      }
    }
  }
  else
  {
    qmi_req->apn_name_valid = TRUE;
    if (req->access_string.offset == 0)
    {
      QBI_LOG_D_0("Attempt to use network-provided APN");
    }
    else
    {
      field = qbi_txn_req_databuf_get_field(
        txn, &req->access_string, 0, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
      QBI_CHECK_NULL_PTR_RET_FALSE(field);

      bytes_copied = qbi_util_utf16_to_ascii(field, req->access_string.size,
                                             qmi_req->apn_name,
                                             sizeof(qmi_req->apn_name));
      QBI_LOG_D_2("Populated %d/%d bytes of APN field with AccessString",
                  bytes_copied, sizeof(qmi_req->apn_name));
    }

    if (req->username.offset == 0)
    {
      QBI_LOG_D_0("No username provided");
    }
    else
    {
      field = qbi_txn_req_databuf_get_field(
        txn, &req->username, 0, QBI_SVC_BC_USERNAME_MAX_LEN_BYTES);
      QBI_CHECK_NULL_PTR_RET_FALSE(field);

      qmi_req->username_valid = TRUE;
      bytes_copied = qbi_util_utf16_to_ascii(field, req->username.size,
                                             qmi_req->username,
                                             sizeof(qmi_req->username));
      QBI_LOG_D_2("Populated %d/%d bytes of username field", bytes_copied,
                  sizeof(qmi_req->username));
    }

    if (req->password.offset == 0)
    {
      QBI_LOG_D_0("No password provided");
    }
    else
    {
      field = qbi_txn_req_databuf_get_field(
        txn, &req->password, 0, QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES);
      QBI_CHECK_NULL_PTR_RET_FALSE(field);

      qmi_req->password_valid = TRUE;
      bytes_copied = qbi_util_utf16_to_ascii(field, req->password.size,
                                             qmi_req->password,
                                             sizeof(qmi_req->password));
      QBI_LOG_D_2("Populated %d/%d bytes of password field", bytes_copied,
                  sizeof(qmi_req->password));
    }

    /*! @todo P3 Compression not currently supported by START_NETWORK_INTERFACE.
        Would have to either add to QMI or possibly workaround by setting it in
        a profile, then using that profile to connect. Currently ignoring
        compression setting in request. */

    qmi_req->authentication_preference_valid = TRUE;
    qmi_req->authentication_preference =
      qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_pref(req->auth_protocol);
  }

  return TRUE;
} /* qbi_svc_bc_connect_s_build_wds20_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_build_wds21_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WDS_STOP_NETWORK_INTERFACE_REQ to
    tear down an active connection with the IP type identified by the QMI
    WDS service ID, and updates the cache to DEACTIVATING

    @details
    The assumption before calling this function is that the IP type is
    activated: we can't tear down a connection in any other state

    @param txn
    @param wds_svc_id
    @param rsp_cb

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_build_wds21_req
(
  qbi_txn_s            *txn,
  qbi_qmi_svc_e         wds_svc_id,
  qbi_svc_qmi_rsp_cb_f *rsp_cb
)
{
  qbi_svc_bc_connect_ip_type_state_s *ip_type_state;
  wds_stop_network_interface_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  ip_type_state = qbi_svc_bc_connect_get_cached_ip_type_state(txn->ctx, wds_svc_id);
  QBI_CHECK_NULL_PTR_RET_ABORT(ip_type_state);

  if (ip_type_state->qmi.pkt_handle_valid)
  {
    qmi_req = (wds_stop_network_interface_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, wds_svc_id, QMI_WDS_STOP_NETWORK_INTERFACE_REQ_V01, rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->pkt_data_handle = ip_type_state->qmi.pkt_handle;
    (void) qbi_svc_bc_connect_update_cache_activation_state(
      txn->ctx, QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING, wds_svc_id);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    QBI_LOG_E_2("Invalid pkt_handle in activation state %d for WDS svc_id %d when "
                "building wds21 req", ip_type_state->activation_state,
                wds_svc_id);
  }

  return action;
} /* qbi_svc_bc_connect_build_wds21_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_es_clean_up_then_send_rsp
===========================================================================*/
/*!
    @brief Cleans up any lingering resources from a failed MBIM_CID_CONNECT
    activation request, then sends the response

    @details
    This function will result in tearing down an IP type that successfully
    connected in the event that the overall transaction failed, e.g. host
    requested IPv4AndIPv6 but only IPv4 connected: this will tear down the
    IPv4 connection then send the response.

    @param txn
    @param session_id

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_es_clean_up_then_send_rsp
(
  qbi_txn_s *txn,
  uint32     session_id
)
{
  qbi_qmi_svc_e wds_svc_id;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* Don't update the cache to DEACTIVATING, so the overall state can move
     directly from ACTIVATING back to DEACTIVATED */
  for (wds_svc_id = QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id);
       wds_svc_id <= QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id);
       wds_svc_id++)
  {
    if (qbi_svc_bc_connect_is_svc_id_activated(txn->ctx, wds_svc_id))
    {
      QBI_LOG_I_1("Tearing down connection on WDS service ID %d", wds_svc_id);
      action = qbi_svc_bc_connect_build_wds21_req(
        txn, wds_svc_id, qbi_svc_bc_connect_es_clean_up_wds21_rsp_cb);
    }
  }

  if (action == QBI_SVC_ACTION_SEND_RSP)
  {
    action = qbi_svc_bc_connect_eqs_build_rsp_from_cache(txn, session_id);
  }

  return action;
} /* qbi_svc_bc_connect_es_clean_up_then_send_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_es_clean_up_wds21_rsp_cb
===========================================================================*/
/*!
    @brief Processes a QMI_WDS_STOP_NETWORK_INTERFACE_RESP associated with a
    request to tear down a lingering connection when a connection could not
    be fully established.

    @details
    If successful, allocates & sends response

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_es_clean_up_wds21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_stop_network_interface_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_stop_network_interface_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    /* If this is cleanup after an event, and the cache update results in
       no change, then it means that we already sent an event for the cache
       change, so do a silent abort. */
    if (!qbi_svc_bc_connect_update_cache_activation_state(
          qmi_txn->ctx, QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED,
          qmi_txn->svc_id) &&
        qmi_txn->parent->cmd_type == QBI_TXN_CMD_TYPE_EVENT)
    {
      QBI_LOG_I_0("Aborting CONNECT event because no change to cache");
      action = QBI_SVC_ACTION_ABORT;
    }
    else if (qmi_txn->parent->qmi_txns_pending > 0)
    {
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
    else
    {
      action = qbi_svc_bc_connect_eqs_build_rsp_from_cache(
        qmi_txn->parent,
        QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_txn->svc_id));
    }
  }

  return action;
} /* qbi_svc_bc_connect_es_clean_up_wds21_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_is_session_ipv4_activated
===========================================================================*/
/*!
    @brief Checks whether the device has an active connection up on the given
    IP type identified by QMI WDS service ID

    @details

    @param ctx
    @param session_id

    @return boolean TRUE if connected, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_connect_is_session_ipv4_activated
(
  const qbi_ctx_s *ctx,
  uint32           session_id
)
{
  qbi_qmi_svc_e wds_svc_id;
  boolean activated = FALSE;
/*-------------------------------------------------------------------------*/
  wds_svc_id = QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id);
  if (qbi_svc_bc_connect_get_ip_type_activation_state(ctx, wds_svc_id) ==
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
  {
    activated = TRUE;
  }

  return activated;
} /* qbi_svc_bc_connect_is_session_ipv4_activated() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_is_session_ipv6_activated
===========================================================================*/
/*!
    @brief Checks whether the device has an active connection up on the given
    IP type identified by QMI WDS service ID

    @details

    @param ctx
    @param session_id

    @return boolean TRUE if connected, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_connect_is_session_ipv6_activated
(
  const qbi_ctx_s *ctx,
  uint32           session_id
)
{
  qbi_qmi_svc_e wds_svc_id;
  boolean activated = FALSE;
/*-------------------------------------------------------------------------*/
  wds_svc_id = QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id);
  if (qbi_svc_bc_connect_get_ip_type_activation_state(ctx, wds_svc_id) ==
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
  {
    activated = TRUE;
  }

  return activated;
} /* qbi_svc_bc_connect_is_session_ipv6_activated() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_is_svc_id_activated
===========================================================================*/
/*!
    @brief Checks whether the device has an active connection up on the given
    QMI WDS service ID

    @details

    @param ctx
    @param session_id

    @return boolean TRUE if connected, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_is_svc_id_activated
(
  const qbi_ctx_s *ctx,
  qbi_qmi_svc_e    wds_svc_id
)
{
  boolean activated = FALSE;
/*-------------------------------------------------------------------------*/
  if (qbi_svc_bc_connect_get_ip_type_activation_state(ctx, wds_svc_id) ==
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
  {
    activated = TRUE;
  }

  return activated;
} /* qbi_svc_bc_connect_is_svc_id_activated() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_timeout_cb
===========================================================================*/
/*!
    @brief Begins processing an abort operation on a failed context
    activation request

    @details

    @param req_txn Transaction containing the failed request
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_s_timeout_cb
(
  const qbi_txn_s *req_txn
)
{
  qbi_qmi_svc_e wds_svc_id;
  qbi_svc_action_e action = QBI_SVC_ACTION_NULL;
  qbi_svc_bc_connect_s_req_s *req;
  wds_abort_req_msg_v01 *qmi_req;
  qbi_qmi_txn_s *abort_qmi_txn;
  qbi_qmi_txn_s *req_qmi_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(req_txn);
  QBI_CHECK_NULL_PTR_RET(req_txn->req.data);

  req = (qbi_svc_bc_connect_s_req_s *) req_txn->req.data;
  for (wds_svc_id = QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(req->session_id);
       wds_svc_id <= QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(req->session_id);
       wds_svc_id++)
  {
    req_qmi_txn = qbi_qmi_txn_get(req_txn, wds_svc_id,
      QMI_WDS_START_NETWORK_INTERFACE_REQ_V01);
    if (req_qmi_txn != NULL &&
        req_qmi_txn->status == QBI_QMI_TXN_STATUS_WAITING_RSP)
    {
      QBI_LOG_W_2("Aborting start network interface request on qmi_svc_id %d"
                  " with QMI txn ID %d", wds_svc_id, req_qmi_txn->txn_handle);
      abort_qmi_txn = qbi_qmi_txn_alloc(req_txn, wds_svc_id,
        QMI_WDS_ABORT_REQ_V01, qbi_svc_bc_connect_s_abort_wds02_rsp_cb);
      if (abort_qmi_txn == NULL)
      {
        /* We couldn't allocate the abort request, but don't give up yet. We
           still have a shot at processing the wds20 response to save the packet
           data handle and tear down the context later. */
        QBI_LOG_E_1("Couldn't allocate abort request for qmi_svc_id %d! "
                    "Delaying teardown until SNI response received",
                    wds_svc_id);
      }
      else
      {
        qmi_req = (wds_abort_req_msg_v01 *) abort_qmi_txn->req.data;
        if (qmi_req == NULL)
        {
          QBI_LOG_E_0("Unexpected NULL pointer!");
          qbi_qmi_txn_free(abort_qmi_txn);
        }
        else if (!qbi_qmi_get_qmi_txn_id(req_qmi_txn->ctx, req_qmi_txn->svc_id,
          req_qmi_txn->txn_handle, (uint16 *) &qmi_req->tx_id))
        {
          QBI_LOG_E_0("Couldn't get transaction ID!");
          qbi_qmi_txn_free(abort_qmi_txn);
        }
        else
        {
          action = QBI_SVC_ACTION_SEND_QMI_REQ;
        }
      }

      /* Switch the response callback over to one that will always tear down the
         interface, even if it connected successfully. */
      qbi_os_fptr_encode(&req_qmi_txn->cb,
        (qbi_os_void_f *) qbi_svc_bc_connect_s_abort_wds20_rsp_cb);
    }
  }

  /* If no pending transactions to abort, tear down the interface if needed
     then send the event. */
  if (action == QBI_SVC_ACTION_NULL)
  {
    action = qbi_svc_bc_connect_es_clean_up_then_send_rsp(req_txn, req->session_id);
  }

  (void) qbi_svc_proc_action(req_txn, action);
} /* qbi_svc_bc_connect_s_timeout_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_abort
===========================================================================*/
/*!
    @brief Begins processing an abort operation on a failed context
    activation request

    @details

    @param req_txn Transaction containing the failed request
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_s_abort
(
  const qbi_txn_s *req_txn
)
{
  qbi_txn_s *txn;
  qbi_qmi_svc_e wds_svc_id;
  qbi_svc_action_e action = QBI_SVC_ACTION_NULL;
  qbi_svc_bc_connect_s_req_s *req;
  wds_abort_req_msg_v01 *qmi_req;
  qbi_qmi_txn_s *abort_qmi_txn;
  qbi_qmi_txn_s *req_qmi_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(req_txn);
  QBI_CHECK_NULL_PTR_RET(req_txn->req.data);

  /* Allocate a new transaction to own the abort requests. It will also be
     used to send out a DISCONNECTED event when tear down is complete. */
  txn = qbi_txn_alloc_event(
    req_txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_CONNECT);
  QBI_CHECK_NULL_PTR_RET(txn);

  /* Set the completion callback so we can do a resync after the abort is
     complete, to be sure we don't leave the cache in a transient state. */
  qbi_txn_set_completion_cb(txn, qbi_svc_bc_connect_s_abort_completion_cb);

  /* Make the new event transaction adopt any pending QMI transactions from
     the original request, so the new transaction can process the responses. */
  (void) qbi_txn_transfer_qmi_txns(req_txn, txn);
  if (txn->qmi_txns_pending > 0)
  {
    /* Initialize the action to wait on our pending adopted qmi_txn(s) */
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  req = (qbi_svc_bc_connect_s_req_s *) req_txn->req.data;
  txn->req.data = (qbi_svc_bc_connect_s_req_s *) req_txn->req.data;
  for (wds_svc_id = QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(req->session_id);
       wds_svc_id <= QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(req->session_id);
       wds_svc_id++)
  {
    req_qmi_txn = qbi_qmi_txn_get(
      txn, wds_svc_id, QMI_WDS_START_NETWORK_INTERFACE_REQ_V01);
    if (req_qmi_txn != NULL &&
        req_qmi_txn->status == QBI_QMI_TXN_STATUS_WAITING_RSP)
    {
      QBI_LOG_W_2("Aborting start network interface request on qmi_svc_id %d"
                  " with QMI txn ID %d", wds_svc_id, req_qmi_txn->txn_handle);
      abort_qmi_txn = qbi_qmi_txn_alloc(
        txn, wds_svc_id, QMI_WDS_ABORT_REQ_V01,
        qbi_svc_bc_connect_s_abort_wds02_rsp_cb);
      if (abort_qmi_txn == NULL)
      {
        /* We couldn't allocate the abort request, but don't give up yet. We
           still have a shot at processing the wds20 response to save the packet
           data handle and tear down the context later. */
        QBI_LOG_E_1("Couldn't allocate abort request for qmi_svc_id %d! "
                    "Delaying teardown until SNI response received",
                    wds_svc_id);
      }
      else
      {
        qmi_req = (wds_abort_req_msg_v01 *) abort_qmi_txn->req.data;
        if (qmi_req == NULL)
        {
          QBI_LOG_E_0("Unexpected NULL pointer!");
          qbi_qmi_txn_free(abort_qmi_txn);
        }
        else if (!qbi_qmi_get_qmi_txn_id(
                   req_qmi_txn->ctx, req_qmi_txn->svc_id,
                   req_qmi_txn->txn_handle, (uint16 *) &qmi_req->tx_id))
        {
          QBI_LOG_E_0("Couldn't get transaction ID!");
          qbi_qmi_txn_free(abort_qmi_txn);
        }
        else
        {
          action = QBI_SVC_ACTION_SEND_QMI_REQ;
        }
      }

      /* Switch the response callback over to one that will always tear down the
         interface, even if it connected successfully. */
      qbi_os_fptr_encode(&req_qmi_txn->cb,
                         (qbi_os_void_f *) qbi_svc_bc_connect_s_abort_wds20_rsp_cb);
    }
  }

  /* If no pending transactions to abort, tear down the interface if needed
     then send the event. */
  if (action == QBI_SVC_ACTION_NULL)
  {
    action = qbi_svc_bc_connect_es_clean_up_then_send_rsp(txn, req->session_id);
  }

  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_connect_s_abort() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_abort_completion_cb
===========================================================================*/
/*!
    @brief Completion callback for a context activation abort transaction

    @details
    Initiates a cache resync to ensure cache consistency.

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_s_abort_completion_cb
(
  const qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->req.data);

  req = (qbi_svc_bc_connect_s_req_s *)txn->req.data;
  qbi_svc_bc_connect_resync_session_cache(txn->ctx, req->session_id);
} /* qbi_svc_bc_connect_s_abort_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_abort_rsp
===========================================================================*/
/*!
    @brief Response processing for a connect abort transaction

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_abort_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
  const qbi_svc_bc_connect_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (const qbi_svc_bc_connect_s_req_s *) txn->req.data;
  if (txn->qmi_txns_pending > 0)
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  else
  {
    /* In the event that one of our ABORT requests failed because the call was
       already connected, this function will ensure that it gets torn down */
    action = qbi_svc_bc_connect_es_clean_up_then_send_rsp(txn, req->session_id);
  }

  return action;
} /* qbi_svc_bc_connect_s_abort_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_abort_wds02_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_ABORT_RESP sent when aborting a connect request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_abort_wds02_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_abort_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_abort_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    /* This should only happen if the abort came after the SNI request
       completed */
    QBI_LOG_E_2("Couldn't abort start network interface request on qmi_svc_id "
                "%d: error %d", qmi_txn->svc_id, qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_1("Successfully aborted start network interface request on "
                "qmi_svc_id %d", qmi_txn->svc_id);
  }

  return qbi_svc_bc_connect_s_abort_rsp(qmi_txn->parent);
} /* qbi_svc_bc_connect_s_abort_wds02_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_abort_wds20_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_START_NETWORK_INTERFACE_RESP on an connect abort
    transaction

    @details
    The abort transaction doesn't send the SNI request: it adopts it from
    the original CID request transaction in qbi_svc_bc_connect_s_abort().

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_abort_wds20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
/*-------------------------------------------------------------------------*/
  /* Use the common SNI response processing function to save the relevant
     information to the cache. */
  if (!qbi_svc_bc_connect_s_wds20_rsp_process(qmi_txn))
  {
    QBI_LOG_E_0("Couldn't process SNI response in abort handler");
  }

  return qbi_svc_bc_connect_s_abort_rsp(qmi_txn->parent);
} /* qbi_svc_bc_connect_s_abort_wds20_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate
===========================================================================*/
/*!
    @brief Builds a QMI_WDS_START_NETWORK_INTERFACE_REQ to activate a
    context based on a MBIM_CID_CONNECT set request

    @details

    @param txn
    @param cache

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_connect_s_req_s *) txn->req.data;
  QBI_LOG_I_1("Activating new context with requested IP type %d", req->ip_type);
  if (QBI_SVC_BC_CONNECT_IPV4_REQUESTED(req->ip_type) &&
      !qbi_svc_bc_connect_s_build_wds20_req(
        txn, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(req->session_id)))
  {
    QBI_LOG_E_0("Couldn't build request to activate IPv4 context!");
  }
  else if (QBI_SVC_BC_CONNECT_IPV6_REQUESTED(req->ip_type) &&
           !qbi_svc_bc_connect_s_build_wds20_req(
             txn, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(req->session_id)))
  {
    QBI_LOG_E_0("Couldn't build request to activate IPv6 context!");
  }
  else
  {
    qbi_svc_bc_connect_update_cache_activating(
      txn->ctx, req->session_id, req->ip_type, req->context_type);

    /* Set the completion callback to handle unexpected failures. */
    qbi_txn_set_completion_cb(txn, qbi_svc_bc_connect_s_activate_completion_cb);

    /* Connect can take a long time to complete, so extend the timeout before
       dispatching the QMI request. */
    qbi_txn_set_timeout(txn, QBI_SVC_BC_CONNECT_TIMEOUT_MS, 
      qbi_svc_bc_connect_s_timeout_cb);

    /* Notify the host that we are now in the activating state. This is somewhat
       assumed by the requesting entity, but there could be another client of
       the host's API that would like to be kept up to date. */
    qbi_svc_bc_connect_s_force_event(txn->ctx, req->session_id);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_connect_s_activate() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_timeout_cb
===========================================================================*/
/*!
    @brief Timeout Callback which is called when connect transaction is
           timed out

    @details

    @param txn

    @return void
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_s_activate_timeout_cb
(
  qbi_txn_s *txn
)
{
  uint32 session_id = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  QBI_LOG_E_0("Context activation request timedout: Aborting.");
  session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(txn->svc_id);
  (void) qbi_svc_bc_connect_update_cache_activation_state(
     txn->ctx , QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED, txn->svc_id);

} /* qbi_svc_bc_connect_s_activate_timeout_cb */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_apn_bearer_to_3gpp_data_class
===========================================================================*/
/*!
    @brief Map QMI APN bearer to 3GPP data class

    @details

    @param apn_bearer

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_connect_s_activate_apn_bearer_to_3gpp_data_class
(
  wds_apn_bearer_mask_v01 apn_bearer
)
{
  uint32 data_class = QBI_SVC_BC_DATA_CLASS_NONE;
/*-------------------------------------------------------------------------*/
  if (apn_bearer & WDS_MASK_APN_BEARER_ANY_V01)
  {
    data_class = QBI_SVC_BC_DATA_CLASS_3GPP_FAMILY;
  }
  else
  {
    if (apn_bearer & WDS_MASK_APN_BEARER_G_V01)
    {
      data_class |= QBI_SVC_BC_DATA_CLASS_FAMILY_GSM;
    }

    if (apn_bearer & WDS_MASK_APN_BEARER_W_V01)
    {
      data_class |= QBI_SVC_BC_DATA_CLASS_FAMILY_WCDMA;
    }

    if (apn_bearer & WDS_MASK_APN_BEARER_L_V01)
    {
      data_class |= QBI_SVC_BC_DATA_CLASS_LTE;
    }
  }

  return data_class;
} /* qbi_svc_bc_connect_s_activate_apn_bearer_to_3gpp_data_class */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_completion_cb
===========================================================================*/
/*!
    @brief Completion callback invoked when a MBIM_CID_CONNECT set request to
    activate the context is done

    @details
    In addition to failures, this is also invoked if the host cancels the
    request via a MBIM_HOST_ERROR message with MBIM_ERROR_CANCEL, so we need
    to properly abort any pending SNI requests and ensure the interface gets
    torn down.

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_s_activate_completion_cb
(
  const qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  if (txn->status != QBI_MBIM_STATUS_SUCCESS)
  {
    QBI_LOG_E_0("Context activation request failed unexpectedly: aborting...");
    qbi_svc_bc_connect_s_abort(txn);
  }
} /* qbi_svc_bc_connect_s_activate_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_get_next_profile
===========================================================================*/
/*!
    @brief Retrive next available configured profile.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate_get_next_profile
(
  qbi_txn_s *txn
)
{
  wds_get_profile_settings_req_msg_v01 *qmi_req_wds2b;
  qbi_svc_bc_connect_profiles_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_connect_profiles_info_s *) txn->info;
  if (info->profiles_read >= info->num_of_profile_epc +
        info->num_of_profile_3gpp + info->num_of_profile_3gpp2)
  {
    /*If a connect request is received for a profile that is present on the 
      modem, the profile shall be found and activation shall be attempted.
      However if profile is not found on the modem, a non-persistent profile 
      is created in order to support the connect request.*/
    if((info->profile_found_epc == TRUE) ||
       (info->profile_found_3gpp == TRUE) ||
       (info->profile_found_3gpp2 == TRUE))
    {
      action = qbi_svc_bc_connect_s_activate(txn);
    }
    else
    {
      action = qbi_svc_bc_build_temp_profile_wds27_req(txn);
    }
  }
  else
  {
    /* Issue a query to retrieve the profile details */
    qmi_req_wds2b = (wds_get_profile_settings_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_connect_s_wds2b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_wds2b);

    if (info->profiles_read < info->num_of_profile_epc)
    {
      qmi_req_wds2b->profile.profile_type = WDS_PROFILE_TYPE_EPC_V01;
      qmi_req_wds2b->profile.profile_index =
        info->profile_list.profile_index[info->profiles_read];
    }
    else if (info->profiles_read < info->num_of_profile_epc +
               info->num_of_profile_3gpp)
    {
      qmi_req_wds2b->profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      qmi_req_wds2b->profile.profile_index =
        info->profile_list.profile_index[info->profiles_read];
    }
    else
    {
      qmi_req_wds2b->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
      qmi_req_wds2b->profile.profile_index =
        info->profile_list.profile_index[info->profiles_read];
    }

    QBI_LOG_I_2("Reading profile type %d index %d",
                qmi_req_wds2b->profile.profile_type,
                qmi_req_wds2b->profile.profile_index);
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_connect_s_activate_get_next_profile() */

/*===========================================================================
FUNCTION: qbi_svc_bc_build_temp_profile_wds27_req
===========================================================================*/
/*!
  @brief Handles temp profile creation for  QBI_SVC_BC_MBIM_CID_CONNECT 
  set request

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_build_temp_profile_wds27_req
(
  qbi_txn_s *txn
)
{
  wds_create_profile_req_msg_v01 *qmi_req;
  qbi_svc_bc_temp_profile_settings_ptrs_s profile_settings;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (wds_create_profile_req_msg_v01 *)
             qbi_qmi_txn_alloc_ret_req_buf(
             txn, QBI_QMI_SVC_WDS, QMI_WDS_CREATE_PROFILE_REQ_V01,
             qbi_svc_bc_temp_profile_wds27_1_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile_type = WDS_PROFILE_TYPE_EPC_V01;

  QBI_LOG_D_0("Entry qbi_svc_bc_build_temp_profile_wds27_req");

  /* depending on the profile type populate the profile settings for
     creating the profile */
  if (!qbi_svc_bc_temp_profile_populate_type_agnostic_profile_settings(
       qmi_req, &profile_settings, qmi_req->profile_type))
  {
    QBI_LOG_E_0("Couldn't collect profile setting pointers!");
  }
  else
  {
    action = qbi_svc_bc_temp_profile_populate_profile(
             txn, qmi_req->profile_type,&profile_settings);
  }
  QBI_LOG_D_0("Exit qbi_svc_bc_build_temp_profile_wds27_req");

  return action;
}/* qbi_svc_bc_build_temp_profile_wds27_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_temp_profile_populate_type_agnostic_profile_settings
===========================================================================*/
/*!
  @brief Collect pointers to all relevant TLVs in
  QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ for a MBIM_CID_CONNECT

  @details This function allows us to populate a type agnostic profile
  setting structure

  @param qmi_req
  @param profile_settings
  @param profile_type

  @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_temp_profile_populate_type_agnostic_profile_settings
(
  wds_create_profile_req_msg_v01                     *qmi_req,
  qbi_svc_bc_temp_profile_settings_ptrs_s            *profile_settings,
  wds_profile_type_enum_v01                           profile_type
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_req);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  if (profile_type == WDS_PROFILE_TYPE_EPC_V01)
  {
    profile_settings->apn_name_valid = &qmi_req->apn_name_valid;
    profile_settings->apn_name = qmi_req->apn_name;
    profile_settings->user_id_valid = &qmi_req->common_user_id_valid;
    profile_settings->user_id = qmi_req->common_user_id;
    profile_settings->auth_password_valid = &qmi_req->common_auth_password_valid;
    profile_settings->auth_password = qmi_req->common_auth_password;
    profile_settings->auth_protocol_valid = &qmi_req->common_auth_protocol_valid;
    profile_settings->auth_protocol = &qmi_req->common_auth_protocol;
    profile_settings->app_user_data_valid = &qmi_req->common_app_user_data_valid;
    profile_settings->app_user_data = &qmi_req->common_app_user_data;
    profile_settings->pdp_data_compression_type_valid = &qmi_req->pdp_data_compression_type_valid;
    profile_settings->pdp_data_compression_type = &qmi_req->pdp_data_compression_type;
    profile_settings->pdp_hdr_compression_type_valid = &qmi_req->pdp_hdr_compression_type_valid;
    profile_settings->pdp_hdr_compression_type = &qmi_req->pdp_hdr_compression_type;
    profile_settings->pdp_type_valid = &qmi_req->pdp_type_valid;
    profile_settings->pdp_type = &qmi_req->pdp_type;
    profile_settings->persistent_valid = &qmi_req->persistent_valid;
    profile_settings->persistent = &qmi_req->persistent;
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    profile_settings->apn_name_valid = &qmi_req->apn_name_valid;
    profile_settings->apn_name = qmi_req->apn_name;
    profile_settings->user_id_valid = &qmi_req->username_valid;
    profile_settings->user_id = qmi_req->username;
    profile_settings->auth_password_valid = &qmi_req->password_valid;
    profile_settings->auth_password = qmi_req->password;
    profile_settings->authentication_preference_valid = &qmi_req->authentication_preference_valid;
    profile_settings->authentication_preference = &qmi_req->authentication_preference;
    profile_settings->app_user_data_valid = &qmi_req->app_user_data_3gpp_valid;
    profile_settings->app_user_data = &qmi_req->app_user_data_3gpp;
    profile_settings->pdp_data_compression_type_valid = &qmi_req->pdp_data_compression_type_valid;
    profile_settings->pdp_data_compression_type = &qmi_req->pdp_data_compression_type;
    profile_settings->pdp_hdr_compression_type_valid = &qmi_req->pdp_hdr_compression_type_valid;
    profile_settings->pdp_hdr_compression_type = &qmi_req->pdp_hdr_compression_type;
    profile_settings->pdp_type_valid = &qmi_req->pdp_type_valid;
    profile_settings->pdp_type = &qmi_req->pdp_type;
    profile_settings->persistent_valid = &qmi_req->persistent_valid;
    profile_settings->persistent = &qmi_req->persistent;
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    profile_settings->apn_name_valid = &qmi_req->apn_string_valid;
    profile_settings->apn_name = qmi_req->apn_string;
    profile_settings->user_id_valid = &qmi_req->user_id_valid;
    profile_settings->user_id = qmi_req->user_id;
    profile_settings->auth_password_valid = &qmi_req->auth_password_valid;
    profile_settings->auth_password = qmi_req->auth_password;
    profile_settings->auth_protocol_valid = &qmi_req->auth_protocol_valid;
    profile_settings->auth_protocol = &qmi_req->auth_protocol;
    profile_settings->app_user_data_valid = &qmi_req->app_user_data_3gpp2_valid;
    profile_settings->app_user_data = &qmi_req->app_user_data_3gpp2;
    profile_settings->pdp_type_valid = &qmi_req->pdp_type_valid;
    profile_settings->pdp_type = &qmi_req->pdp_type;
    profile_settings->persistent_valid = &qmi_req->persistent_valid;
    profile_settings->persistent = &qmi_req->persistent;
  }

  return TRUE;
} /* qbi_svc_bc_temp_profile_populate_type_agnostic_profile_settings() */

/*===========================================================================
FUNCTION: qbi_svc_bc_temp_profile_populate_profile
===========================================================================*/
/*!
 @brief Populates QMI profile settings for a MBIM_CID_CONNECT

 @details

 @param txn
 @param profile_type
    @param profile_index

 @return qbi_svc_action_e QBI_SVC_ACTION_SEND_QMI_REQ on success,
 QBI_SVC_ACTION_ABORT on failure
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_temp_profile_populate_profile
(
  qbi_txn_s                                               *txn,
  wds_profile_type_enum_v01                                profile_type,
  qbi_svc_bc_temp_profile_settings_ptrs_s                 *profile_settings
)
{
  qbi_svc_bc_connect_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->apn_name_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->user_id_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->auth_password_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->app_user_data_valid);

  req = (qbi_svc_bc_connect_s_req_s *)txn->req.data;

  if (!qbi_svc_bc_temp_profile_populate_profile_compression(
      req, profile_type, profile_settings) ||
      !qbi_svc_bc_temp_profile_populate_profile_auth_protocol(
      txn, req, profile_type, profile_settings))
  {
     QBI_LOG_E_0("Couldn't populate compression or auth protocol!");
  }
  else if (profile_type == WDS_PROFILE_TYPE_EPC_V01 ||
           profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    *profile_settings->apn_name_valid = TRUE;
    *profile_settings->user_id_valid = TRUE;
    *profile_settings->auth_password_valid = TRUE;
    *profile_settings->pdp_type_valid = TRUE;

     if(req->ip_type ==  QBI_SVC_BC_IP_TYPE_IPV4)
     {
        *profile_settings->pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
     }
     else if(req->ip_type ==  QBI_SVC_BC_IP_TYPE_IPV6)
     {
        *profile_settings->pdp_type = WDS_PDP_TYPE_PDP_IPV6_V01;
     }
     else
     {
        *profile_settings->pdp_type = WDS_PDP_TYPE_PDP_IPV4V6_V01;
     }

     *profile_settings->persistent_valid = TRUE;
     *profile_settings->persistent = FALSE;

      if (req->access_string.offset != 0 &&
         !qbi_svc_bc_temp_profile_populate_str(
         txn, &req->access_string, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES,
         profile_settings->apn_name, QMI_WDS_APN_NAME_MAX_V01))
      {
         QBI_LOG_E_0("Couldn't populate QMI request for APN name!");
      }
      else if (req->username.offset != 0 &&
              !qbi_svc_bc_temp_profile_populate_str(
              txn, &req->username, QBI_SVC_BC_USERNAME_MAX_LEN_BYTES,
              profile_settings->user_id, QMI_WDS_USER_NAME_MAX_V01))
      {
         QBI_LOG_E_0("Couldn't populate QMI request for username!");
      }
      else if (req->password.offset != 0 &&
              !qbi_svc_bc_temp_profile_populate_str(
              txn, &req->password, QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES,
              profile_settings->auth_password,
              QMI_WDS_PASSWORD_MAX_V01))
      {
         QBI_LOG_E_0("Couldn't populate QMI request for password!");
      }
      else
      {
      *profile_settings->app_user_data_valid =
              qbi_svc_bc_context_type_uuid_to_id(
              req->context_type,
              (uint32 *)profile_settings->app_user_data);
    
        action = QBI_SVC_ACTION_SEND_QMI_REQ;
      }
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    *profile_settings->apn_name_valid = TRUE;
    *profile_settings->user_id_valid = TRUE;
    *profile_settings->auth_password_valid = TRUE;
    *profile_settings->pdp_type_valid = TRUE;

    if(req->ip_type ==  QBI_SVC_BC_IP_TYPE_IPV4)
    {
      *profile_settings->pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
    }
    else if(req->ip_type ==  QBI_SVC_BC_IP_TYPE_IPV6)
    {
      *profile_settings->pdp_type = WDS_PDP_TYPE_PDP_IPV6_V01;
    }
    else
    {
      *profile_settings->pdp_type = WDS_PDP_TYPE_PDP_IPV4V6_V01;
    }

    *profile_settings->persistent_valid = TRUE;
    *profile_settings->persistent = FALSE;

    if (req->access_string.offset != 0 &&
        !qbi_svc_bc_temp_profile_populate_str(
        txn, &req->access_string, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES,
        profile_settings->apn_name, QMI_WDS_APN_NAME_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for 3GPP2 APN string!");
    }
    else if (req->username.offset != 0 &&
        !qbi_svc_bc_temp_profile_populate_str(
        txn, &req->username, QBI_SVC_BC_USERNAME_MAX_LEN_BYTES,
        profile_settings->user_id, QMI_WDS_USER_NAME_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for 3GPP2 username!");
    }
    else if (req->password.offset != 0 &&
          !qbi_svc_bc_temp_profile_populate_str(
          txn, &req->password, QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES,
          profile_settings->auth_password, QMI_WDS_PASSWORD_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for 3GPP2 password!");
    }
    else
    {
      *profile_settings->app_user_data_valid =
      qbi_svc_bc_context_type_uuid_to_id(
      req->context_type,
      (uint32 *)profile_settings->app_user_data);

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_bc_temp_profile_populate_profile() */

/*===========================================================================
FUNCTION: qbi_svc_bc_temp_profile_s_wds27_1_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_CREATE_PROFILE_RESP for
    MBIM_CID_CONNECT set request
    
  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_temp_profile_wds27_1_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_create_profile_req_msg_v01 *qmi_req = NULL;
  qbi_svc_bc_temp_profile_settings_ptrs_s profile_settings = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  /* EPC profile creation failure is ignored as benign, we proceed to create to 3GPP
     profile creation. */
  qmi_req = (wds_create_profile_req_msg_v01 *)
            qbi_qmi_txn_alloc_ret_req_buf( 
            qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_CREATE_PROFILE_REQ_V01,
            qbi_svc_bc_temp_profile_wds27_2_rsp_cb);

  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP_V01;

  if (!qbi_svc_bc_temp_profile_populate_type_agnostic_profile_settings(
          qmi_req, &profile_settings, WDS_PROFILE_TYPE_3GPP_V01))
  {
     QBI_LOG_E_0("Couldn't collect profile setting pointers!");
  }
  else
  {
     action = qbi_svc_bc_temp_profile_populate_profile(
     qmi_txn->parent, qmi_req->profile_type,&profile_settings);
  }

  return action;
} /* qbi_svc_bc_temp_profile_s_wds27_1_rsp_cb() */
/*===========================================================================
FUNCTION: qbi_svc_bc_temp_profile_wds27_2_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_CREATE_PROFILE_RESP for
    MBIM_CID_CONNECT set request
    
  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_temp_profile_wds27_2_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_create_profile_req_msg_v01 *qmi_req = NULL;
  wds_create_profile_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_temp_profile_settings_ptrs_s profile_settings = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_create_profile_resp_msg_v01 *)qmi_txn->rsp.data;
  
  /* 3GPP profile creation failure is considered catastrophic and the connect is 
     failed */
  if(qmi_rsp->resp.result == QMI_RESULT_FAILURE_V01)
  {
     QBI_LOG_E_0("3GPP profile creation failure: abortting");
     qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
  }
  else
  {
     /* A 3gpp2 profile should also be added to support data continuity */
     qmi_req = (wds_create_profile_req_msg_v01 *)
               qbi_qmi_txn_alloc_ret_req_buf(
               qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_CREATE_PROFILE_REQ_V01,
               qbi_svc_bc_temp_profile_wds27_3_rsp_cb);

     QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

     qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP2_V01;

     if (!qbi_svc_bc_temp_profile_populate_type_agnostic_profile_settings(
          qmi_req, &profile_settings, WDS_PROFILE_TYPE_3GPP2_V01))
     {
       QBI_LOG_E_0("Couldn't collect profile setting pointers!");
     }
     else
     {
       action = qbi_svc_bc_temp_profile_populate_profile(
       qmi_txn->parent, qmi_req->profile_type, &profile_settings);
     }
  }

  return action;
}

/*===========================================================================
FUNCTION: qbi_svc_bc_temp_profile_wds27_3_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_CREATE_PROFILE_RESP for
    MBIM_CID_CONNECT set request
    
  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_temp_profile_wds27_3_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  if (qmi_txn->parent->info)
  {
    QBI_MEM_FREE(qmi_txn->parent->info);
    qmi_txn->parent->info = NULL;
  }

  /* 3GPP2 profile creation failure is considered as benign, as long as 3GPP profile 
     is created proceed to connect*/
  action = qbi_svc_bc_connect_s_scan_profile_then_activate(qmi_txn->parent);

  return action;
}

/*===========================================================================
FUNCTION: qbi_svc_bc_temp_profile_populate_profile_compression
===========================================================================*/
/*!
  @brief

  @details

  @param req
  @param profile_type
  @param profile_settings

  @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_temp_profile_populate_profile_compression
(
  qbi_svc_bc_connect_s_req_s                        *req,
  wds_profile_type_enum_v01                          profile_type,
  qbi_svc_bc_temp_profile_settings_ptrs_s           *profile_settings
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(req);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  if (profile_type == WDS_PROFILE_TYPE_EPC_V01 ||
      profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
     QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->pdp_data_compression_type_valid);
     QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->pdp_data_compression_type);
     QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->pdp_hdr_compression_type_valid);
     QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->pdp_hdr_compression_type);

     *(profile_settings->pdp_data_compression_type_valid) = TRUE;
     *(profile_settings->pdp_hdr_compression_type_valid) = TRUE;
     if (req->compression == QBI_SVC_BC_COMPRESSION_ENABLE)
     {
        *(profile_settings->pdp_data_compression_type) =
        WDS_PDP_DATA_COMPR_TYPE_MANUFACTURER_PREF_V01;
        *(profile_settings->pdp_hdr_compression_type) =
        WDS_PDP_HDR_COMPR_TYPE_MANUFACTURER_PREF_V01;
     }
     else
     {
        *(profile_settings->pdp_data_compression_type) =
        WDS_PDP_DATA_COMPR_TYPE_OFF_V01;
        *(profile_settings->pdp_hdr_compression_type) =
        WDS_PDP_HDR_COMPR_TYPE_OFF_V01;
     }
  }

  return TRUE;
} /* qbi_svc_bc_temp_profile_populate_profile_compression() */

/*===========================================================================
FUNCTION: qbi_svc_bc_temp_profile_populate_profile_auth_protocol
===========================================================================*/
/*!
  @brief 

  @details

  @param txn
  @param req
  @param profile_type
  @param profile_settings

  @return boolean TRUE on success, FALSE on failure. May set txn->status
  if the request contained an invalid value
*/
/*=========================================================================*/
static boolean qbi_svc_bc_temp_profile_populate_profile_auth_protocol
(
  qbi_txn_s                                               *txn,
  qbi_svc_bc_connect_s_req_s                              *req,
  wds_profile_type_enum_v01                               profile_type,
  qbi_svc_bc_temp_profile_settings_ptrs_s                 *profile_settings
)
{
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(req);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  if (profile_type == WDS_PROFILE_TYPE_EPC_V01)
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->auth_protocol_valid);
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->auth_protocol);
    *(profile_settings->auth_protocol_valid) = TRUE;
    *(profile_settings->auth_protocol) =
    qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_protocol(
    req->auth_protocol);
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->authentication_preference_valid);
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->authentication_preference);
    *(profile_settings->authentication_preference_valid) = TRUE;
    *(profile_settings->authentication_preference) =
    qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_pref(req->auth_protocol);
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->auth_protocol_valid);
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->auth_protocol);
    if (req->auth_protocol == QBI_SVC_BC_AUTH_PROTOCOL_NONE)
    {
       QBI_LOG_W_0("Ignoring 3GPP2 AUTH_PROTOCOL_NONE setting");
    }
    else
    {
       *(profile_settings->auth_protocol_valid) = TRUE;
       if (req->auth_protocol == QBI_SVC_BC_AUTH_PROTOCOL_CHAP ||
           req->auth_protocol == QBI_SVC_BC_AUTH_PROTOCOL_MSCHAP_V2)
       {
         *(profile_settings->auth_protocol) = WDS_PROFILE_AUTH_PROTOCOL_CHAP_V01;
       }
       else if (req->auth_protocol == QBI_SVC_BC_AUTH_PROTOCOL_PAP)
       {
         *(profile_settings->auth_protocol) = WDS_PROFILE_AUTH_PROTOCOL_PAP_V01;
       }
       else
       {
         QBI_LOG_E_1("Invalid authentication protocol %d", req->auth_protocol);
              txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
         success = FALSE;
       }
     }
  }
  else
  {
    QBI_LOG_E_1("Invalid profile type %d", profile_type);
    success = FALSE;
  }

  return success;
} /* qbi_svc_bc_temp_profile_populate_profile_auth_protocol() */

/*===========================================================================
FUNCTION: qbi_svc_bc_temp_profile_populate_str
===========================================================================*/
/*!
    @brief 

    @details
    Ensures that the UTF-16 string was not truncated when copying into the
    ASCII buffer.

    @param txn
    @param field_desc
    @param field_max_size
    @param qmi_field
    @param qmi_field_size

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_temp_profile_populate_str
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *field_desc,
  uint32                             field_max_size,
  char                              *qmi_field,
  uint32                             qmi_field_size
)
{
  const uint8 *req_str_utf16;
  uint32 bytes_copied;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  req_str_utf16 = qbi_txn_req_databuf_get_field(
                  txn, field_desc, 0, field_max_size);
  QBI_CHECK_NULL_PTR_RET_FALSE(req_str_utf16);

  bytes_copied = qbi_util_utf16_to_ascii(
  req_str_utf16, field_desc->size, qmi_field, qmi_field_size);
  if (bytes_copied > qmi_field_size)
  {
    QBI_LOG_E_2("Couldn't fit entire MBIM string into QMI request! Need %d "
    "bytes, have room for %d", bytes_copied, qmi_field_size);
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_temp_profile_populate_str() */

/*===========================================================================
FUNCTION: qbi_svc_bc_match_is_pdn_pdp_type
===========================================================================*/
/*!
    @brief Check if settings in a configured profile match pdn or pdp.

    @details

    @param profile_identifier
    @param profile_settings
    @param req

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_match_is_pdn_pdp_type
(
  wds_profile_type_enum_v01 profile_type,
  wds_get_profile_settings_resp_msg_v01 *profile_settings,
  qbi_svc_bc_connect_s_req_s *req
)
{
  boolean result = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);
  QBI_CHECK_NULL_PTR_RET_FALSE(req);

  switch (profile_type)
  {
    case WDS_PROFILE_TYPE_3GPP_V01:
      if (profile_settings->pdp_type_valid &&
           (profile_settings->pdp_type == WDS_PDP_TYPE_PDP_PPP_V01 ||
             (profile_settings->pdp_type == WDS_PDP_TYPE_PDP_IPV4_V01 &&
               !QBI_SVC_BC_CONNECT_IPV4ONLY_REQUESTED(req->ip_type)) ||
             (profile_settings->pdp_type == WDS_PDP_TYPE_PDP_IPV6_V01 &&
               !QBI_SVC_BC_CONNECT_IPV6ONLY_REQUESTED(req->ip_type)) ||
             (profile_settings->pdp_type == WDS_PDP_TYPE_PDP_IPV4V6_V01 &&
               !QBI_SVC_BC_CONNECT_IPV4V6_REQUESTED(req->ip_type)) ||
             (profile_settings->pdp_type != WDS_PDP_TYPE_PDP_IPV4V6_V01 &&
               QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(req->ip_type) ==
               QBI_SVC_BC_IP_TYPE_IPV4_AND_IPV6)))
      {
        QBI_LOG_I_0("PDP type does not match");
        result = FALSE;
      }
    break;

    case WDS_PROFILE_TYPE_3GPP2_V01:
      if (profile_settings->pdn_type_valid &&
           (profile_settings->pdn_type == WDS_PROFILE_PDN_TYPE_IPV4_V01 &&
             !QBI_SVC_BC_CONNECT_IPV4ONLY_REQUESTED(req->ip_type)) ||
           (profile_settings->pdn_type == WDS_PROFILE_PDN_TYPE_IPV6_V01 &&
             !QBI_SVC_BC_CONNECT_IPV6ONLY_REQUESTED(req->ip_type)) ||
           (profile_settings->pdn_type == WDS_PROFILE_PDN_TYPE_IPV4_IPV6_V01 &&
             !QBI_SVC_BC_CONNECT_IPV4V6_REQUESTED(req->ip_type)) ||
           (profile_settings->pdn_type != WDS_PROFILE_PDN_TYPE_IPV4_IPV6_V01 &&
             QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(req->ip_type) ==
             QBI_SVC_BC_IP_TYPE_IPV4_AND_IPV6))
      {
        QBI_LOG_I_0("PDN type does not match");
        result = FALSE;
      }
    break;
}

  return result;
}/* qbi_svc_bc_match_is_pdn_pdp_type */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_is_profile_matched
===========================================================================*/
/*!
    @brief Check if settings in a configured profile match MBIM_CID_CONNECT
    set request

    @details

    @param qmi_txn
    @param profile_settings

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_s_activate_is_profile_matched
(
  qbi_qmi_txn_s                         *qmi_txn,
  wds_get_profile_settings_resp_msg_v01 *profile_settings
)
{
  qbi_svc_bc_connect_s_req_s *req;
  wds_get_profile_settings_req_msg_v01 *qmi_req;
  wds_auth_pref_mask_v01 req_auth_pref;
  wds_profile_auth_protocol_enum_v01 req_auth_protocol;
  const uint8 *field;
  char apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  char username[QMI_WDS_USER_NAME_MAX_V01 + 1];
  char password[QMI_WDS_PASSWORD_MAX_V01 + 1];
  uint32 available_data_class;
  uint32 apn_bearer_data_class;
  boolean result = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  req = (qbi_svc_bc_connect_s_req_s *) qmi_txn->parent->req.data;
  qmi_req = (wds_get_profile_settings_req_msg_v01 *) qmi_txn->req.data;

  /* Compare APN name */
  if (req->access_string.size != 0)
  {
    field = qbi_txn_req_databuf_get_field(
      qmi_txn->parent, &req->access_string, 0,
      QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
    QBI_CHECK_NULL_PTR_RET_FALSE(field);

    (void) qbi_util_utf16_to_ascii(
      field, req->access_string.size, apn_name, sizeof(apn_name));

    if ((qmi_req->profile.profile_type != WDS_PROFILE_TYPE_3GPP2_V01 &&
      (!profile_settings->apn_name_valid ||
      QBI_STRNICMP(apn_name, profile_settings->apn_name,
      sizeof(apn_name)))) ||
      (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP2_V01 &&
      (!profile_settings->apn_string_valid ||
      QBI_STRNICMP(apn_name, profile_settings->apn_string,
      sizeof(apn_name)))))
    {
      QBI_LOG_I_0("APN name does not match");
      result = FALSE;
    }
  }
  else if ((qmi_req->profile.profile_type != WDS_PROFILE_TYPE_3GPP2_V01 &&
            profile_settings->apn_name_valid &&
            QBI_STRLEN(profile_settings->apn_name) != 0) ||
           (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP2_V01 &&
            profile_settings->apn_string_valid &&
            QBI_STRLEN(profile_settings->apn_string) != 0))
  {
    QBI_LOG_I_0("APN name does not match");
    result = FALSE;
  }

  /* Compare username */
  if (result == TRUE)
  {
    if (req->username.size != 0)
    {
      field = qbi_txn_req_databuf_get_field(
        qmi_txn->parent, &req->username, 0, QBI_SVC_BC_USERNAME_MAX_LEN_BYTES);
      QBI_CHECK_NULL_PTR_RET_FALSE(field);

      (void) qbi_util_utf16_to_ascii(
        field, req->username.size, username, sizeof(username));

      if ((qmi_req->profile.profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
           (!profile_settings->common_user_id_valid ||
            QBI_STRNCMP(username, profile_settings->common_user_id,
                        sizeof(username)))) ||
          (qmi_req->profile.profile_type != WDS_PROFILE_TYPE_EPC_V01 &&
           (!profile_settings->username_valid ||
            QBI_STRNCMP(username, profile_settings->username,
                        sizeof(username)))))
      {
        QBI_LOG_I_0("user name does not match");
        result = FALSE;
      }
    }
    else if ((qmi_req->profile.profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
              profile_settings->common_user_id_valid &&
              QBI_STRLEN(profile_settings->common_user_id) != 0) ||
             (qmi_req->profile.profile_type != WDS_PROFILE_TYPE_EPC_V01 &&
              profile_settings->username_valid &&
              QBI_STRLEN(profile_settings->username) != 0))
    {
      QBI_LOG_I_0("user name does not match");
      result = FALSE;
    }
  }

  /* Compare password */
  if (result == TRUE &&
      qmi_req->profile.profile_type != WDS_PROFILE_TYPE_3GPP2_V01)
  {
    if (req->password.size != 0)
    {
      field = qbi_txn_req_databuf_get_field(
        qmi_txn->parent, &req->password, 0, QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES);
      QBI_CHECK_NULL_PTR_RET_FALSE(field);

      (void) qbi_util_utf16_to_ascii(
        field, req->password.size, password, sizeof(password));

      if ((qmi_req->profile.profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
           (!profile_settings->common_auth_password_valid ||
            QBI_STRNCMP(password, profile_settings->common_auth_password,
                        sizeof(password)))) ||
          (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP_V01 &&
           (!profile_settings->password_valid ||
            QBI_STRNCMP(password, profile_settings->password,
                        sizeof(password)))))
      {
        QBI_LOG_I_0("password does not match");
        result = FALSE;
      }
    }
    else if ((qmi_req->profile.profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
              profile_settings->common_auth_password_valid &&
              QBI_STRLEN(profile_settings->common_auth_password) != 0) ||
             (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP_V01 &&
              profile_settings->password_valid &&
              QBI_STRLEN(profile_settings->password) != 0))
    {
      QBI_LOG_I_0("password does not match");
      result = FALSE;
    }
  }

  /* Compare authentication type. */
  req_auth_pref = qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_pref(
    req->auth_protocol);
  req_auth_protocol = qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_protocol(
    req->auth_protocol);
  if (result == TRUE &&
      ((qmi_req->profile.profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
        profile_settings->auth_protocol_valid &&
        req_auth_protocol != profile_settings->auth_protocol) ||
       (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP_V01 &&
        profile_settings->authentication_preference_valid &&
        (req_auth_pref & profile_settings->authentication_preference) == 0 &&
        req_auth_pref != 0) ||
       (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP2_V01 &&
        profile_settings->auth_protocol_valid &&
        (req_auth_protocol & profile_settings->auth_protocol) == 0 &&
        req_auth_protocol != 0)))
  {
    QBI_LOG_I_0("auth protocol does not match");
    result = FALSE;
  }

  /* Compare IP type. */
  /* the profile matching is strict about the IP type match. If v4v6 is present
     on the modem and requested type is v4 only or v6 only it is not considered
     a match. Instead a profile with exact IP type match shall be sought and 
     if not available created on the fly*/
   if (result == TRUE)
    result = qbi_svc_bc_match_is_pdn_pdp_type(qmi_req->profile.profile_type, 
               profile_settings,req);

  /* Compare APN Bearer. If 3GPP APN bearer mask is specified in profile,
     we should match it with available data class to make sure we don't
     attempt data call using an incompitible profile. If matching profile
     is found, requested IP type should also be limited to the available
     PDP type. */
  available_data_class = qbi_svc_bc_get_available_data_class(qmi_txn->ctx);
  if (result == TRUE && profile_settings->apn_bearer_valid &&
      qmi_req->profile.profile_type != WDS_PROFILE_TYPE_3GPP2_V01 &&
      (available_data_class & QBI_SVC_BC_DATA_CLASS_3GPP2_FAMILY) == 0)
  {
    apn_bearer_data_class =
      qbi_svc_bc_connect_s_activate_apn_bearer_to_3gpp_data_class(
        profile_settings->apn_bearer);
    if ((available_data_class & apn_bearer_data_class) == 0)
    {
      QBI_LOG_I_2("available data class 0x%x is not in APN bearer data class"
                  " 0x%x", available_data_class, apn_bearer_data_class);
      result = FALSE;
    }
    else if (profile_settings->pdp_type_valid)
    {
      if (QBI_SVC_BC_CONNECT_IPV6_REQUESTED(req->ip_type) &&
          profile_settings->pdp_type == WDS_PDP_TYPE_PDP_IPV4_V01)
      {
        QBI_LOG_I_0("Force requested IP type to IPv4 only");
        req->ip_type = QBI_SVC_BC_IP_TYPE_IPV4;
      }
      else if (QBI_SVC_BC_CONNECT_IPV4_REQUESTED(req->ip_type) &&
               profile_settings->pdp_type == WDS_PDP_TYPE_PDP_IPV6_V01)
      {
        QBI_LOG_I_0("Force requested IP type to IPv6 only");
        req->ip_type = QBI_SVC_BC_IP_TYPE_IPV6;
      }
    }
  }

  QBI_LOG_I_2("Profile type %d match result %d",
              qmi_req->profile.profile_type, result);

  return result;
} /* qbi_svc_bc_connect_s_activate_is_profile_matched() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_req_is_loopback
===========================================================================*/
/*!
    @brief Check if MBIM_CID_CONNECT set request is for loopback

    @details

    @param txn
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_s_activate_req_is_loopback
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_s_req_s *req;
  const uint8 *field;
  char apn_name[QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES];
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  req = (qbi_svc_bc_connect_s_req_s *) txn->req.data;
  if (req->access_string.offset != 0)
  {
    field = qbi_txn_req_databuf_get_field(
      txn, &req->access_string, 0, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
    QBI_CHECK_NULL_PTR_RET_FALSE(field);

    (void) qbi_util_utf16_to_ascii(
      field, req->access_string.size, apn_name, sizeof(apn_name));

    /* Enable loopback state if loopback APN is detected */
    if (!QBI_STRNICMP(apn_name, QBI_SVC_BC_CONNECT_LOOPBACK_APN_ASCII,
      sizeof(apn_name)))
    {
      QBI_LOG_I_0("Connect mode is loopback.");
      result = TRUE;
    }
  }

  return result;
} /* qbi_svc_bc_connect_s_activate_req_is_loopback */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_wda29_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDA_SET_LOOPBACK_STATE_RESP_V01 for enabling loopback
           state

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate_wda29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wda_set_loopback_state_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wda_set_loopback_state_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    /*!@note loopback should always use session ID 0 */
    qbi_svc_bc_connect_update_cache_loopback_state(qmi_txn->ctx, 0, TRUE);
    action = qbi_svc_bc_connect_s_activate(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_connect_s_activate_wda29_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_deactivate_completion_cb
===========================================================================*/
/*!
    @brief Completion callback invoked when a MBIM_CID_CONNECT set request to
    activate the context is done

    @details
    Makes sure that the session_id_valid flag in the cache won't be left
    as TRUE in the event of internal failures.

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_s_deactivate_completion_cb
(
  const qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->req.data);

  if (txn->status != QBI_MBIM_STATUS_SUCCESS)
  {
    QBI_LOG_E_0("Context deactivation attempt failed unexpectedly");
    req = (qbi_svc_bc_connect_s_req_s *)txn->req.data;
    qbi_svc_bc_connect_resync_session_cache(txn->ctx, req->session_id);
  }
} /* qbi_svc_bc_connect_s_deactivate_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_deactivate_wds29_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_DELETE_PROFILE_RESP for MBIM_CID_CONNECT 
           set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_deactivate_wds29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{

  wds_delete_profile_req_msg_v01 *qmi_req = NULL;
  wds_delete_profile_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_cache_s *cache = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
  uint32 session_id = 0;
  /*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);

  qmi_req = (wds_delete_profile_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_delete_profile_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_3("Temporary profile deletion failed with error: %d "
      "for profile type: %d, index: %d", qmi_rsp->resp.error, 
      qmi_req->profile.profile_type, qmi_req->profile.profile_index);
  }

  /* Irrespective of error proceed to delete next temp profile */
  cache = qbi_svc_bc_cache_get(qmi_txn->parent->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_txn->svc_id);

  QBI_LOG_D_2("Deleting temp profile %d of profile type %d completed.",
    qmi_req->profile.profile_index, qmi_req->profile.profile_type);

  /* EPC profile configuration has been deprecated. Check only 3GPP and 3GPP2 */
  if (WDS_PROFILE_TYPE_3GPP_V01 == qmi_req->profile.profile_type)
  {
    cache->connect.sessions[session_id].is_temp_profile_3gpp = FALSE;
    action = qbi_svc_bc_connect_s_deactivate_build_wds4a_req(
      qmi_txn->parent, session_id);
  }
  else if (WDS_PROFILE_TYPE_3GPP2_V01 == qmi_req->profile.profile_type)
  {
    cache->connect.sessions[session_id].is_temp_profile_3gpp2 = FALSE;

    QBI_LOG_D_0("Deletion of all temp profiles completed. Send connect "
      "deactivation command response.");

    /* Clear any failure status as  disconnect has already succeeded and
       modem shall error handle if default profile is not available. */
    qmi_txn->parent->status = QBI_MBIM_STATUS_SUCCESS;
  }

  QBI_LOG_D_0("Releasing WDA client");
  qbi_qmi_release_svc_handle(qmi_txn->ctx, QBI_QMI_SVC_WDA);

  return action;
} /* qbi_svc_bc_connect_s_deactivate_wds29_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_deactivate_build_wds29_req
===========================================================================*/
/*!
    @brief Builds QMI_WDS_DELETE_PROFILE_REQ for MBIM_CID_CONNECT set request.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_deactivate_build_wds29_req
(
  qbi_txn_s *txn,
  uint32 session_id
)
{
  wds_delete_profile_req_msg_v01 *qmi_req;
  qbi_svc_bc_cache_s *cache = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_QMI_REQ;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (wds_delete_profile_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WDS, 
      QMI_WDS_DELETE_PROFILE_REQ_V01,
      qbi_svc_bc_connect_s_deactivate_wds29_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (cache->connect.sessions[session_id].is_temp_profile_3gpp)
  {
    qmi_req->profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
    qmi_req->profile.profile_index =
      cache->connect.sessions[session_id].temp_profile_index_3gpp;
  }
  else if (cache->connect.sessions[session_id].is_temp_profile_3gpp2)
  {
    qmi_req->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
    qmi_req->profile.profile_index =
      cache->connect.sessions[session_id].temp_profile_index_3gpp2;
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  if (QBI_SVC_ACTION_SEND_QMI_REQ == action)
  {
    QBI_LOG_D_2("Deleting profile index %d for profile type: %d.",
      qmi_req->profile.profile_index, qmi_req->profile.profile_type);
  }
  else
  {
    QBI_LOG_D_0("No more temp profiles to be deleted.");

    /* Clear any failure status as  disconnect has already succeeded and 
       modem shall error handle if default profile is not available. */
    txn->status = QBI_MBIM_STATUS_SUCCESS;
  }

  return action;
} /* qbi_svc_bc_connect_s_deactivate_build_wds29_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_deactivate_wds4a_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_SET_DEFAULT_PROFILE_NUM_RESP for
    MBIM_CID_CONNECT set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_deactivate_wds4a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{

  wds_set_default_profile_num_req_msg_v01 *qmi_req = NULL;
  wds_set_default_profile_num_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_cache_s *cache = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
  boolean is_temp_profile = FALSE;
  uint32 session_id = 0;
  /*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);

  qmi_req = (wds_set_default_profile_num_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_set_default_profile_num_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }

  /* Irrespective of error proceed to restore default profile */
  cache = qbi_svc_bc_cache_get(qmi_txn->parent->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_txn->svc_id);

  /* EPC profile configuration has been deprecated. Check only 3GPP and 3GPP2 */
  if (WDS_PROFILE_TYPE_3GPP_V01 ==
    qmi_req->profile_identifier.profile_type)
  {
    cache->connect.sessions[session_id].restore_default_profile_3gpp = FALSE;
    is_temp_profile = cache->connect.sessions[session_id].is_temp_profile_3gpp;
  }
  else if (WDS_PROFILE_TYPE_3GPP2_V01 ==
    qmi_req->profile_identifier.profile_type)
  {
    cache->connect.sessions[session_id].restore_default_profile_3gpp2 = FALSE;
    is_temp_profile = cache->connect.sessions[session_id].is_temp_profile_3gpp2;
  }

  QBI_LOG_D_1("Setting default profile for profile type: %d completed.",
    qmi_req->profile_identifier.profile_type);

  if (is_temp_profile)
  {
    action = qbi_svc_bc_connect_s_deactivate_build_wds29_req(
      qmi_txn->parent, session_id);
  }
  else
  {
    QBI_LOG_D_0("Setting default profile for all profile types completed. "
      " Send response for connect deactivation command.");

    /* Clear any failure status as  disconnect has already succeeded and
       modem shall error handle if default profile is not available. */
    qmi_txn->parent->status = QBI_MBIM_STATUS_SUCCESS;
  }

  return action;
} /* qbi_svc_bc_connect_s_deactivate_wds4a_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_deactivate_build_wds4a_req
===========================================================================*/
/*!
    @brief Builds QMI_WDS_SET_DEFAULT_PROFILE_NUM_REQ for
    MBIM_CID_CONNECT set request.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_deactivate_build_wds4a_req
(
  qbi_txn_s *txn,
  uint32 session_id
)
{
  wds_set_default_profile_num_req_msg_v01 *qmi_req;
  qbi_svc_bc_cache_s *cache = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_QMI_REQ;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (wds_set_default_profile_num_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WDS, 
      QMI_WDS_SET_DEFAULT_PROFILE_NUM_REQ_V01,
      qbi_svc_bc_connect_s_deactivate_wds4a_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  /* As the data call is deactivated set the index to default value */
  cache->connect.sessions[session_id].active_data_profile_3gpp_index =
     QBI_SVC_BC_CONNECT_PROFILE_INDEX_INVALID;

  cache->connect.sessions[session_id].active_data_profile_3gpp2_index =
     QBI_SVC_BC_CONNECT_PROFILE_INDEX_INVALID;

  if (cache->connect.sessions[session_id].restore_default_profile_3gpp)
  {
    qmi_req->profile_identifier.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
    qmi_req->profile_identifier.profile_index =
      cache->connect.sessions[session_id].default_profile_index_3gpp;
  }
  else if (cache->connect.sessions[session_id].restore_default_profile_3gpp2)
  {
    qmi_req->profile_identifier.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
    qmi_req->profile_identifier.profile_index =
      cache->connect.sessions[session_id].default_profile_index_3gpp2;
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  if (QBI_SVC_ACTION_SEND_QMI_REQ == action)
  {
    QBI_LOG_D_2("Restore profile index %d as default for profile type: %d.",
      qmi_req->profile_identifier.profile_index,
      qmi_req->profile_identifier.profile_type);
  }
  else
  {
    QBI_LOG_D_0("No default profile restore required for any profile type.");

    /* Clear any failure status as  disconnect has already succeeded and 
       modem shall error handle if default profile is not available. */
    txn->status = QBI_MBIM_STATUS_SUCCESS;
  }

  return action;
} /* qbi_svc_bc_connect_s_deactivate_build_wds4a_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_deactivate_wds22_ind_cb
===========================================================================*/
/*!
    @brief QMI_WDS_PKT_SRVC_STATUS_IND handler which waits for the data
    call to be completely torn down, before respond to MBIM_CID_CONNECT
    deactivation request received during DEACTIVATING stage, e.g. in OOS or
    network call drop senarios

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_deactivate_wds22_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action;
  uint32 session_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);

  session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(ind->qmi_svc_id);

  /* Notify host if session has been disconnected, otherwise keep waiting */
  if (!qbi_svc_bc_connect_is_session_connected(ind->txn->ctx, session_id))
  {
    QBI_LOG_I_0("Connection deactivated - proceeding with response");
    action = qbi_svc_bc_connect_eqs_build_rsp_from_cache(
      ind->txn, session_id);

    if (action == QBI_SVC_ACTION_SEND_RSP)
    {
      action = qbi_svc_bc_connect_s_deactivate_build_wds4a_req(
        ind->txn, session_id);
    }
  }
  else
  {
    QBI_LOG_I_0("Still waiting for teardown");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_connect_s_deactivate_wds22_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_enable_loopback_state_then_activate
===========================================================================*/
/*!
    @brief Enable WDA loopback state before sending connect request to QMI

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_enable_loopback_state_then_activate
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wda_set_loopback_state_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/

  if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_WDA))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else
  {
    qmi_req = (wda_set_loopback_state_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDA, QMI_WDA_SET_LOOPBACK_STATE_REQ_V01,
        qbi_svc_bc_connect_s_activate_wda29_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->loopback_state = TRUE;
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  if (QBI_SVC_ACTION_ABORT == action)
  {
    QBI_LOG_D_0("Releasing WDA client");
    qbi_qmi_release_svc_handle(txn->ctx, QBI_QMI_SVC_WDA);
  }

  return action;
} /* qbi_svc_bc_connect_s_enable_loopback_state_then_activate */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_force_event
===========================================================================*/
/*!
    @brief Forces sending an MBIM_CID_CONNECT event based on the current
    contents of the cache

    @details
    Used when a set request results in changing the activation state to
    ACTIVATING or DEACTIVATING

    @param ctx
    @param session_id
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_s_force_event
(
  qbi_ctx_s *ctx,
  uint32     session_id
)
{
  qbi_txn_s *txn;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  txn = qbi_txn_alloc_event(ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_CONNECT);
  QBI_CHECK_NULL_PTR_RET(txn);

  action = qbi_svc_bc_connect_eqs_build_rsp_from_cache(txn, session_id);
  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_connect_s_force_event() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_pre_activation_checks
===========================================================================*/
/*!
    @brief Performs device state checks before a request to activate a
    packet context can be issued

    @details
    Performs checks like whether the radio is powered on. In the event that
    a check fails, the transaction's status will be set to the appropriate
    status code, e.g. QBI_MBIM_STATUS_RADIO_POWER_OFF.

    @param txn

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_s_pre_activation_checks
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_s_req_s *req;
  boolean success = FALSE;
  uint32 activation_state;
  boolean is_loopback;
  qbi_svc_bc_spdp_cache_s cache = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  /* Loopback call should be allowed regardless of SIM state, RF state,
     Register state and packet service state. */
  is_loopback = qbi_svc_bc_connect_s_activate_req_is_loopback(txn);

  req = (qbi_svc_bc_connect_s_req_s *)txn->req.data;

  qbi_svc_bc_spdp_read_nv_store(txn->ctx, &cache);
  if (cache.spdp_support_flag && 0 != req->session_id)
  {
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    return FALSE;
  }

  activation_state = qbi_svc_bc_connect_session_activation_state(
    txn->ctx, req->session_id);

  if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_UNKNOWN)
  {
    QBI_LOG_E_0("Can't process activation request since current state unknown!");
    txn->status = QBI_MBIM_STATUS_NOT_INITIALIZED;
  }
  else if (!qbi_svc_bc_check_device_state(txn, TRUE, TRUE) && !is_loopback)
  {
    QBI_LOG_W_0("Can't process activation request - failed device state check");
  }
  else if (!qbi_svc_bc_nas_is_registered(txn->ctx) &&
           !is_loopback)
  {
    QBI_LOG_E_0("Can't activate new context since not registered");
    txn->status = QBI_MBIM_STATUS_NOT_REGISTERED;
  }
  else if (!qbi_svc_bc_nas_is_attached(txn->ctx) && !is_loopback)
  {
    QBI_LOG_E_0("Can't activate new context since not attached");
    txn->status = QBI_MBIM_STATUS_PACKET_SVC_DETACHED;
  }
  else if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING ||
           activation_state == QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING)
  {
    QBI_LOG_E_1("Can't activate new context when in transient state %d",
                activation_state);
    txn->status = QBI_MBIM_STATUS_BUSY;
  }
  else if (activation_state != QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED)
  {
    QBI_LOG_E_1("Can't activate new context while in state %d",
                activation_state);
    txn->status = QBI_MBIM_STATUS_MAX_ACTIVATED_CONTEXTS;
  }
  else
  {
    QBI_LOG_I_0("All pre-activation checks passed");
    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_connect_s_pre_activation_checks() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_pre_deactivation_checks
===========================================================================*/
/*!
    @brief Performs device state checks before a request to deactivate a
    packet context can be issued

    @details
    Performs checks like whether a packet context is active or not. Sets
    the status field of the transaction to the appropriate status code in
    the event a check fails, e.g. QBI_MBIM_STATUS_CONTEXT_NOT_ACTIVATED.

    @param txn

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_s_pre_deactivation_checks
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_s_req_s *req;
  qbi_svc_bc_cache_s *cache;
  boolean success = FALSE;
  uint32 activation_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  req = (qbi_svc_bc_connect_s_req_s *)txn->req.data;
  activation_state = qbi_svc_bc_connect_session_activation_state(
    txn->ctx, req->session_id);
  if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING)
  {
    QBI_LOG_E_1("Can't deactivate context while in transient state %d",
                activation_state);
    txn->status = QBI_MBIM_STATUS_BUSY;
  }
  else if (activation_state != QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED &&
           activation_state != QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING)
  {
    /* Note that we allow requests through while in DEACTIVATING state in case
       there was a problem and we were unable to completely tear down the
       connection. If it results in a duplicate QMI request, the last one will
       get QMI_ERR_NO_EFFECT, which is handled the same as success. */
    QBI_LOG_E_1("Can't deactivate context while in state %d", activation_state);
    txn->status = QBI_MBIM_STATUS_CONTEXT_NOT_ACTIVATED;
  }
  else
  {
    QBI_LOG_I_0("All pre-deactivation checks passed");
    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_connect_s_pre_deactivation_checks() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_CONNECT set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_connect_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_spdp_info_s *spdp_info = NULL;
  qbi_svc_bc_spdp_cache_s cache = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_connect_s_req_s *) txn->req.data;
  if (!qbi_svc_bc_connect_s_sanity_check_req(txn))
  {
    QBI_LOG_E_0("Request failed sanity check");
  }
  else if (req->activation_cmd == QBI_SVC_BC_ACTIVATION_CMD_ACTIVATE)
  {
    qbi_svc_bc_spdp_read_nv_store(txn->ctx, &cache);
    if (cache.spdp_support_flag)
    {
      /* Initially txn->info is NULL 
         After spdp confihgure we get the txn pointer 
         In next iteration flag spdp_cfg_complete is checked 
         If already set nothing needs to be done */
      if (!txn->info)
      {
        // Perform single pdp configuration
        return qbi_svc_bc_spdp_configure(txn);
      }
      else
      {
        spdp_info = (qbi_svc_bc_spdp_info_s*)txn->info;
        if (!spdp_info->spdp_cfg_complete)
        {
          return QBI_SVC_ACTION_ABORT;
        }

        QBI_MEM_FREE(txn->info);
        txn->info = NULL;
      }
    }

    qbi_os_log_event(QBI_OS_LOG_EVENT_CONNECT_REQ);
    if (!qbi_svc_bc_connect_s_pre_activation_checks(txn))
    {
      QBI_LOG_W_0("Request failed pre-activation checks");
    }
    else
    {
      /* Set WDA loopback state first if activate request is for loopback,
         otherwise directly activate connection */
      if (qbi_svc_bc_connect_s_activate_req_is_loopback(txn))
      {
        action = qbi_svc_bc_connect_s_enable_loopback_state_then_activate(txn);
      }
      else
      {
        action = qbi_svc_bc_connect_s_scan_profile_then_activate(txn);
      }
    }
  }
  else if (req->activation_cmd == QBI_SVC_BC_ACTIVATION_CMD_DEACTIVATE)
  {
    if (!qbi_svc_bc_connect_s_pre_deactivation_checks(txn))
    {
      QBI_LOG_W_0("Request failed pre-deactivation checks");
    }
    else
    {
      action = qbi_svc_bc_connect_s_deactivate(txn);
    }
  }

  return action;
} /* qbi_svc_bc_connect_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_sanity_check_req
===========================================================================*/
/*!
    @brief Performs sanity check of parameters provided by the host for a
    MBIM_CID_CONNECT set request

    @details
    Sets the status field in the transaction to the proper value if a check
    fails.

    These checks are for the validity of the parameters provided by the
    host, as opposed to the pre-(de)activation checks, which are checks
    of whether the device state permits the requested action.

    @param txn

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_s_sanity_check_req
(
  qbi_txn_s *txn
)
{
  boolean success = FALSE;
  qbi_svc_bc_connect_s_req_s *req;
  uint32 context_type_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  req = (qbi_svc_bc_connect_s_req_s *) txn->req.data;
  if (req->activation_cmd == QBI_SVC_BC_ACTIVATION_CMD_ACTIVATE)
  {
    if (req->ip_type > QBI_SVC_BC_IP_TYPE_IPV4_AND_IPV6)
    {
      QBI_LOG_E_1("Requested activation using invalid IP type %d",
                  req->ip_type);
      txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    }
    else if (req->auth_protocol > QBI_SVC_BC_AUTH_PROTOCOL_MSCHAP_V2)
    {
      QBI_LOG_E_1("Invalid/unsupported authentication protocol %d",
                  req->auth_protocol);
      txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    }
    else if (req->session_id >= qbi_svc_bc_max_sessions_get(txn->ctx))
    {
      QBI_LOG_E_2("Requested SessionId (%d) is past maximum (%d)",
                  req->session_id, (qbi_svc_bc_max_sessions_get(txn->ctx) - 1));
      txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    }
    else if (!qbi_svc_bc_context_type_uuid_to_id(req->context_type,
                                                 &context_type_id))
    {
      QBI_LOG_E_0("Received unsupported ContextType!");
      txn->status = QBI_MBIM_STATUS_CONTEXT_NOT_SUPPORTED;
    }
    else if (qbi_svc_bc_connect_s_activate_req_is_loopback(txn) &&
             (req->session_id != 0 ||
              !QBI_SVC_BC_CONNECT_IPV4_REQUESTED(req->ip_type) ||
              QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(req->ip_type) ==
                QBI_SVC_BC_IP_TYPE_IPV4_AND_IPV6))
    {
      QBI_LOG_E_2("Couldn't activate session id %d ip type %d in loopback mode!",
                  req->session_id, req->ip_type);
      txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    }
    else
    {
      if (qbi_svc_bc_connect_s_activate_req_is_loopback(txn) &&
         (req->ip_type == QBI_SVC_BC_IP_TYPE_DEFAULT ||
          req->ip_type == QBI_SVC_BC_IP_TYPE_IPV4V6))
      {
        QBI_LOG_D_1("Force ip type %d to ipv4 only", req->ip_type);
        req->ip_type = QBI_SVC_BC_IP_TYPE_IPV4;
      }

      QBI_LOG_D_0("Request passed sanity checks for activation request");
      success = TRUE;
    }
  }
  else if (req->activation_cmd == QBI_SVC_BC_ACTIVATION_CMD_DEACTIVATE)
  {
    /* Currently no parameter checks for DEACTIVATE. Will be necessary
       when support for multiple contexts is added */
    success = TRUE;
  }
  else
  {
    QBI_LOG_E_1("Unrecognized activation command %d", req->activation_cmd);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }

  return success;
} /* qbi_svc_bc_connect_s_sanity_check_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_scan_profile_then_activate
===========================================================================*/
/*!
    @brief Builds a QMI_WDS_START_NETWORK_INTERFACE_REQ to activate a
    context based on a MBIM_CID_CONNECT set request

    @details

    @param txn
    @param cache

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_scan_profile_then_activate
(
  qbi_txn_s *txn
)
{
  wds_get_profile_list_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  txn->info = QBI_MEM_MALLOC_CLEAR(
    sizeof(qbi_svc_bc_connect_profiles_info_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  qmi_req = (wds_get_profile_list_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_LIST_REQ_V01,
      qbi_svc_bc_connect_s_wds2a_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile_type_valid = TRUE;
  qmi_req->profile_type = WDS_PROFILE_TYPE_EPC_V01;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_connect_s_scan_profile_then_activate() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_wds4a_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_SET_DEFAULT_PROFILE_NUM_RESP for
    MBIM_CID_CONNECT set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate_cleanup_and_send_rsp
(
  qbi_txn_s *txn
)
{
  /*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* Clear any failure status as setting connect profile
     to default is not a mandatory requirement. */
  txn->status = QBI_MBIM_STATUS_SUCCESS;

  /* Free previously allocated info buffer used for profile handling.
  Moved freeing of info buffer from SNI request to SNI response as
  the information in info buffer is used for setting the profile
  for connect to default */
  if (txn->info != NULL)
  {
    QBI_MEM_FREE(txn->info);
    txn->info = NULL;
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_connect_s_activate_wds4a_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_wds4a_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_SET_DEFAULT_PROFILE_NUM_RESP for
    MBIM_CID_CONNECT set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate_wds4a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{

  wds_set_default_profile_num_req_msg_v01 *qmi_req = NULL;
  wds_set_default_profile_num_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_connect_profiles_info_s *info = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);

  qmi_req = (wds_set_default_profile_num_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_set_default_profile_num_resp_msg_v01 *)qmi_txn->rsp.data;

  /* Modem may reject EPC profile type depending on modem configuration,
     proceed wihtout failing the set. */
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    action = qbi_svc_bc_connect_s_activate_cleanup_and_send_rsp(qmi_txn->parent);
  }
  else
  {
    QBI_LOG_D_1("Setting default profile for profile type: %d completed.",
      qmi_req->profile_identifier.profile_type);
    info = (qbi_svc_bc_connect_profiles_info_s *)qmi_txn->parent->info;

    /* EPC profile configuration has been deprecated. Check only 3GPP and 3GPP2 */
    if (WDS_PROFILE_TYPE_3GPP_V01 == qmi_req->profile_identifier.profile_type)
    {
      info->is_profile_3gpp_set_to_default = TRUE;
      action = qbi_svc_bc_connect_s_activate_build_wds2b_req(
        qmi_txn->parent, WDS_PROFILE_TYPE_3GPP2_V01);
    }
    else if (WDS_PROFILE_TYPE_3GPP2_V01 == qmi_req->profile_identifier.profile_type)
    {
      info->is_profile_3gpp2_set_to_default = TRUE;

      QBI_LOG_D_0("Setting default profile for all profile types completed. "
        "Send reponse to connect activation command.");

      action = qbi_svc_bc_connect_s_activate_cleanup_and_send_rsp(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_connect_s_activate_wds4a_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_build_wds4a_req
===========================================================================*/
/*!
    @brief Builds QMI_WDS_SET_DEFAULT_PROFILE_NUM_REQ for
    MBIM_CID_CONNECT set request.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate_build_wds4a_req
(
  qbi_txn_s *txn,
  wds_profile_type_enum_v01 profile_type,
  uint32 session_id
)
{
  wds_set_default_profile_num_req_msg_v01 *qmi_req = NULL;
  qbi_svc_bc_connect_profiles_info_s *info = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_QMI_REQ;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_connect_profiles_info_s *) txn->info;

  qmi_req = (wds_set_default_profile_num_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WDS, 
      QMI_WDS_SET_DEFAULT_PROFILE_NUM_REQ_V01,
      qbi_svc_bc_connect_s_activate_wds4a_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile_identifier.profile_type = profile_type;

  if (profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    qmi_req->profile_identifier.profile_index = info->profile_index_3gpp;
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    qmi_req->profile_identifier.profile_index = info->profile_index_3gpp2;
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  if (QBI_SVC_ACTION_SEND_QMI_REQ == action)
  {
    QBI_LOG_D_3("Setting profile Index: %d as default for session: %d, "
      "profile type: %d.", qmi_req->profile_identifier.profile_index, 
      session_id, qmi_req->profile_identifier.profile_type);
  }
  else
  {
    QBI_LOG_D_0("Setting default profile for all profile types complete. "
      "Send response for connect activation command.");

    action = qbi_svc_bc_connect_s_activate_cleanup_and_send_rsp(txn);
  }

  return action;
} /* qbi_svc_bc_connect_s_activate_build_wds4a_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_wds49_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_DEFAULT_PROFILE_NUM_RESP for
    MBIM_CID_CONNECT set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate_wds49_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{

  wds_get_default_profile_num_req_msg_v01 *qmi_req = NULL;
  wds_get_default_profile_num_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_connect_profiles_info_s *info = NULL;
  qbi_svc_bc_cache_s *cache = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
  uint32 session_id = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);

  qmi_req = (wds_get_default_profile_num_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wds_get_default_profile_num_resp_msg_v01 *) qmi_txn->rsp.data;
  info = (qbi_svc_bc_connect_profiles_info_s *)qmi_txn->parent->info;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    
    /* If operation fails, continue with next profile type */
    if (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP_V01)
    {
      info->is_profile_3gpp_set_to_default = TRUE;
      action = qbi_svc_bc_connect_s_activate_build_wds2b_req(
        qmi_txn->parent, WDS_PROFILE_TYPE_3GPP2_V01);
    }
    else if (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
    {
      info->is_profile_3gpp2_set_to_default = TRUE;
    }

    action = qbi_svc_bc_connect_s_activate_cleanup_and_send_rsp(qmi_txn->parent);
  }
  else
  {
    cache = qbi_svc_bc_cache_get(qmi_txn->parent->ctx);
    QBI_CHECK_NULL_PTR_RET_FALSE(cache);

    session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_txn->svc_id);

    /* EPC profile configuration has been deprecated. Check only 3GPP and 3GPP2 */
    if (WDS_PROFILE_TYPE_3GPP_V01 == qmi_req->profile.profile_type)
    {
      if (qmi_rsp->profile_index != info->profile_index_3gpp)
      {
        cache->connect.sessions[session_id].restore_default_profile_3gpp = TRUE;
        cache->connect.sessions[session_id].default_profile_index_3gpp =
          qmi_rsp->profile_index;

        QBI_LOG_D_1("Cached 3GPP default profile Index: %d.",
          cache->connect.sessions[session_id].default_profile_index_3gpp);
      }
      else
      {
        QBI_LOG_D_0("3GPP Profile is already default profile.");
        cache->connect.sessions[session_id].restore_default_profile_3gpp = FALSE;
      }
    }
    else if (WDS_PROFILE_TYPE_3GPP2_V01 == qmi_req->profile.profile_type)
    {
      if (qmi_rsp->profile_index != info->profile_index_3gpp2)
      {
        cache->connect.sessions[session_id].restore_default_profile_3gpp2 = TRUE;
        cache->connect.sessions[session_id].default_profile_index_3gpp2 =
          qmi_rsp->profile_index;

        QBI_LOG_D_1("Cached 3GPP2 default profile Index: %d.",
          cache->connect.sessions[session_id].default_profile_index_3gpp2);
      }
      else
      {
        QBI_LOG_D_0("3GPP2 Profile is already default profile.");
        cache->connect.sessions[session_id].restore_default_profile_3gpp2 = FALSE;
      }
    }

    action = qbi_svc_bc_connect_s_activate_build_wds4a_req(
      qmi_txn->parent, qmi_req->profile.profile_type, session_id);
  }

  return action;
} /* qbi_svc_bc_connect_s_activate_wds49_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_build_wds49_req
===========================================================================*/
/*!
    @brief Builds QMI_WDS_GET_DEFAULT_PROFILE_NUM_REQ for
    MBIM_CID_CONNECT set request.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate_build_wds49_req
(
  qbi_txn_s *txn,
  wds_profile_type_enum_v01 profile_type
)
{
  wds_get_default_profile_num_req_msg_v01 *qmi_req;
  qbi_svc_bc_connect_profiles_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_connect_profiles_info_s *)txn->info;

  if ((WDS_PROFILE_TYPE_3GPP_V01 == profile_type && 
    !info->is_profile_3gpp_set_to_default) ||
    (WDS_PROFILE_TYPE_3GPP2_V01 == profile_type &&
      !info->is_profile_3gpp2_set_to_default))
  {
    qmi_req = (wds_get_default_profile_num_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WDS,
        QMI_WDS_GET_DEFAULT_PROFILE_NUM_REQ_V01,
        qbi_svc_bc_connect_s_activate_wds49_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->profile.profile_family = WDS_PROFILE_FAMILY_EMBEDDED_V01;
    qmi_req->profile.profile_type = profile_type;

    QBI_LOG_D_1("Getting default profile for profile type: %d.",
      qmi_req->profile.profile_type);
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    QBI_LOG_D_0("No more profiles to be set to default.");
    action = qbi_svc_bc_connect_s_activate_cleanup_and_send_rsp(txn);
  }

  return action;
} /* qbi_svc_bc_connect_s_activate_build_wds49_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_wds2b_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
    MBIM_CID_CONNECT set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{

  wds_get_profile_settings_req_msg_v01 *qmi_req = NULL;
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_cache_s *cache = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
  uint32 session_id = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);

  qmi_req = (wds_get_profile_settings_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *) qmi_txn->rsp.data;

  if (QMI_RESULT_SUCCESS_V01 != qmi_rsp->resp.result ||
    !(WDS_PROFILE_TYPE_3GPP_V01 == qmi_req->profile.profile_type ||
      WDS_PROFILE_TYPE_3GPP2_V01 == qmi_req->profile.profile_type))
  {
    QBI_LOG_E_2("E: Invalid profile type %d or Received error code %d from QMI. "
      "Send Connect response anyway.", qmi_req->profile.profile_type,
      qmi_rsp->resp.error);
    action = qbi_svc_bc_connect_s_activate_cleanup_and_send_rsp(
      qmi_txn->parent);
  }
  else
  {
    cache = qbi_svc_bc_cache_get(qmi_txn->parent->ctx);
    QBI_CHECK_NULL_PTR_RET_FALSE(cache);

    session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_txn->svc_id);

    QBI_LOG_D_3("Profile type: %d, persistant valid: %d, value: %d.",
      qmi_req->profile.profile_type, qmi_rsp->persistent_valid, qmi_rsp->persistent);

    if (qmi_rsp->persistent_valid && !qmi_rsp->persistent)
    {
      /* EPC profile configuration has been deprecated. Check only 3GPP and 3GPP2 */
      if (WDS_PROFILE_TYPE_3GPP_V01 == qmi_req->profile.profile_type)
      {
        cache->connect.sessions[session_id].is_temp_profile_3gpp = TRUE;
        cache->connect.sessions[session_id].temp_profile_index_3gpp =
          qmi_req->profile.profile_index;
      }
      else if (WDS_PROFILE_TYPE_3GPP_V01 == qmi_req->profile.profile_type)
      {
        cache->connect.sessions[session_id].is_temp_profile_3gpp2 = TRUE;
        cache->connect.sessions[session_id].temp_profile_index_3gpp2 =
          qmi_req->profile.profile_index;
      }

      QBI_LOG_D_2("Cached temp profile Index: %d for profile type: %d "
        "Set profile to default.", qmi_req->profile.profile_index, 
        qmi_req->profile.profile_type);
      
      action = qbi_svc_bc_connect_s_activate_build_wds49_req(
        qmi_txn->parent, qmi_req->profile.profile_type);
    }
    else
    {
      QBI_LOG_D_1("Profile at Index: %d of profile type %d is not temp profile.",
        cache->connect.sessions[session_id].temp_profile_index_3gpp2);
      action = WDS_PROFILE_TYPE_3GPP_V01 == qmi_req->profile.profile_type ?
        qbi_svc_bc_connect_s_activate_build_wds2b_req (
          qmi_txn->parent, WDS_PROFILE_TYPE_3GPP2_V01) :
        qbi_svc_bc_connect_s_activate_cleanup_and_send_rsp(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_connect_s_activate_wds2b_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_build_wds2b_req
===========================================================================*/
/*!
    @brief Builds QMI_WDS_GET_PROFILE_SETTINGS_REQ for
    MBIM_CID_CONNECT set request.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate_build_wds2b_req
(
  qbi_txn_s *txn,
  wds_profile_type_enum_v01 profile_type
)
{
  wds_get_profile_settings_req_msg_v01 *qmi_req;
  qbi_svc_bc_connect_profiles_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_connect_profiles_info_s *) txn->info;

  qmi_req = (wds_get_profile_settings_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WDS, 
      QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
      qbi_svc_bc_connect_s_activate_wds2b_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile.profile_type = profile_type;
  action = QBI_SVC_ACTION_SEND_QMI_REQ;

  switch (profile_type)
  {
  case WDS_PROFILE_TYPE_3GPP_V01:
    qmi_req->profile.profile_index = info->profile_index_3gpp;
    break;
  case WDS_PROFILE_TYPE_3GPP2_V01:
    qmi_req->profile.profile_index = info->profile_index_3gpp2;
    break;
  default:
    QBI_LOG_D_0("E: No connect 3GPP / 32GPP2 profile found.");
    action = qbi_svc_bc_connect_s_activate_cleanup_and_send_rsp(txn);  
    break;
  }

  return action;
} /* qbi_svc_bc_connect_s_activate_build_wds2b_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_configure_default_profile
===========================================================================*/
/*!
    @brief Configures connect profile to default if the profile is either an
           LTE attach or a temp profile.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_activate_configure_default_profile
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_profiles_info_s *info = NULL;
  qbi_svc_bc_ext_module_prov_cache_s *cache = NULL;
  wds_profile_type_enum_v01 profile_type = WDS_PROFILE_TYPE_ENUM_MIN_ENUM_VAL_V01;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_connect_profiles_info_s *) txn->info;

  if (info->profile_found_3gpp)
  {
    cache = qbi_svc_bc_ext_module_prov_cache_get(txn->ctx, info->profile_index_3gpp);
    profile_type = WDS_PROFILE_TYPE_3GPP_V01;
  }
  else if(info->profile_found_3gpp2)
  {
    cache = qbi_svc_bc_ext_module_prov_cache_get(txn->ctx, info->profile_index_3gpp2);
    profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
  }

  if (cache)
  {
    action = cache->lte_active &&
      QBI_SVC_BC_EXT_CONTEXT_FLAG_USER_DEFINED == cache->context_flag ?
      qbi_svc_bc_connect_s_activate_build_wds49_req(txn, profile_type) :
      qbi_svc_bc_connect_s_activate_build_wds2b_req(txn, profile_type);
  }
  else
  {
    QBI_LOG_D_0("No more profiles to be configured as default");
    action = qbi_svc_bc_connect_s_activate_cleanup_and_send_rsp(txn);
  }

  return action;
} /* qbi_svc_bc_connect_s_activate_configure_default_profile() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_set_connect_profile_to_default_precheck
===========================================================================*/
/*!
    @brief Performs checks if any of the activated contexts have been set to 
           default.

    @details

    @param txn

    @return TRUE if profile has to be set to default
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_s_set_connect_profile_to_default_precheck
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_cache_s *cache = NULL;
  uint32 session_id = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->info);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  for (session_id = 0; session_id < QBI_SVC_BC_MAX_SESSIONS; session_id++)
  {
    if (QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED ==
      cache->connect.sessions[session_id].ipv4.activation_state ||
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED ==
      cache->connect.sessions[session_id].ipv6.activation_state)
    {     
      if (cache->connect.sessions[session_id].restore_default_profile_3gpp ||
        cache->connect.sessions[session_id].restore_default_profile_3gpp2)
      {
        return FALSE;
      }
    }
  }

  return TRUE;
} /* qbi_svc_bc_connect_s_set_connect_profile_to_default_precheck() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_wds20_rsp_process
===========================================================================*/
/*!
    @brief Process QMI_WDS_START_NETWORK_INTERFACE_RESP for
    MBIM_CID_CONNECT set

    @details
    If a 3GPP-defined network error is available, it will be saved to the
    parent transaction's info structure.

    @param qmi_txn

    @return boolean TRUE if response successfully processed, FALSE on
    unexpected failure. Note that this return value does not indicate whether
    the QMI request returned success or not.
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_s_wds20_rsp_process
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_start_network_interface_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->rsp.data);

  QBI_LOG_I_1("Processing SNI resp from WDS service ID %d", qmi_txn->svc_id);
  qmi_rsp = (wds_start_network_interface_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    (void) qbi_svc_bc_connect_update_cache_activation_state(
      qmi_txn->ctx, QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED, qmi_txn->svc_id);

    /* Set the failure status code, and set nw_error if we received a
       3GPP-defined error code for most recent disconnection */
    qbi_svc_bc_connect_get_failure_status(qmi_txn->parent, qmi_rsp);
    qbi_svc_bc_connect_es_save_nw_error(
      qmi_txn->parent, qmi_txn->svc_id, qmi_rsp->verbose_call_end_reason_valid,
      qmi_rsp->verbose_call_end_reason);
  }
  else
  {
    /* Call is up! Save the QMI packet data handle to the cache so we can use
       it to tear down the call later, and also query to get the initial value
       of the data bearer. */
    QBI_LOG_I_1("Call connected successfully on qmi_svc_id %d", qmi_txn->svc_id);
    qbi_svc_bc_connect_update_cache_qmi_pkt_handle(
      qmi_txn->ctx, qmi_rsp->pkt_data_handle, qmi_txn->svc_id);
    (void) qbi_svc_bc_connect_update_cache_activation_state(
      qmi_txn->ctx, QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED, qmi_txn->svc_id);
  }

  return TRUE;
} /* qbi_svc_bc_connect_s_wds20_rsp_process() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_save_active_data_call_ctxt
===========================================================================*/
/*!
    @brief Save currently active data call profile index

    @details

    @param qmi_txn
    @param session_id

    @return void
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_s_save_active_data_call_ctxt
(
  qbi_qmi_txn_s *qmi_txn,
  wds_start_network_interface_req_msg_v01 *qmi_req,
  uint32 session_id
)
{
  qbi_svc_bc_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  cache = qbi_svc_bc_cache_get(qmi_txn->parent->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  /* Saving current active data call 3GPP/3GPP2 profile index */
  if (qmi_req->profile_index_valid)
  {
    QBI_LOG_D_2("Active 3GPP profile index for session id %d is %d",
                 session_id,qmi_req->profile_index);
    cache->connect.sessions[session_id].active_data_profile_3gpp_index =
       qmi_req->profile_index;
  }
  else
  {
    QBI_LOG_D_0("No valid Active 3GPP profile index");
    cache->connect.sessions[session_id].active_data_profile_3gpp_index =
       QBI_SVC_BC_CONNECT_PROFILE_INDEX_INVALID;
  }

  if (qmi_req->profile_index_3gpp2_valid)
  {
    QBI_LOG_D_2("Active 3GPP2 profile index for session id %d is %d",
                 session_id,qmi_req->profile_index);
    cache->connect.sessions[session_id].active_data_profile_3gpp2_index =
       qmi_req->profile_index_3gpp2;
  }
  else
  {
    QBI_LOG_D_0("No valid Active 3GPP2 profile index");
     cache->connect.sessions[session_id].active_data_profile_3gpp2_index =
       QBI_SVC_BC_CONNECT_PROFILE_INDEX_INVALID;
  }
}/* qbi_svc_bc_connect_s_save_active_data_call_ctxt */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_wds20_rsp_cb
===========================================================================*/
/*!
    @brief Process QMI_WDS_START_NETWORK_INTERFACE_RESP for
    MBIM_CID_CONNECT set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_wds20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_start_network_interface_req_msg_v01 *qmi_req = NULL;
  uint32 session_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);

  QBI_LOG_I_1("Received start network interface response from WDS svc_id %d",
              qmi_txn->svc_id);
  session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_txn->svc_id);
  qmi_req = (wds_start_network_interface_req_msg_v01 *)qmi_txn->req.data;

  if (!qbi_svc_bc_connect_s_wds20_rsp_process(qmi_txn))
  {
    QBI_LOG_E_0("Couldn't process QMI response!");
  }
  else if (qmi_txn->parent->qmi_txns_pending > 0)
  {
    QBI_LOG_I_0("Waiting on response to other SNI request");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  else if (!qbi_svc_bc_connect_is_session_connected(qmi_txn->ctx, session_id) ||
    qmi_txn->parent->timeout_invoked)
  {
    QBI_LOG_E_0("Connection request failed");
    action = qbi_svc_bc_connect_es_clean_up_then_send_rsp(
      qmi_txn->parent, session_id);
  }
  else
  {
    QBI_LOG_I_1("Successfully connected on IP type %d",
                qbi_svc_bc_connect_connected_ip_type(qmi_txn->ctx, session_id));
    qbi_os_log_event(QBI_OS_LOG_EVENT_CONNECTED);

    /* Clear any failure status set for an individual IP type failing, since the
       overall request suceeded. */
    qmi_txn->parent->status = QBI_MBIM_STATUS_SUCCESS;
    action = qbi_svc_bc_connect_eqs_build_rsp_from_cache(
      qmi_txn->parent, session_id);

    /* Save the currently activated 3GPP data profile index */
    qbi_svc_bc_connect_s_save_active_data_call_ctxt(qmi_txn,qmi_req,session_id);

    /* Set profile to default */
    if (QBI_SVC_ACTION_SEND_RSP == action)
    {
      if (qbi_svc_bc_connect_s_set_connect_profile_to_default_precheck(qmi_txn->parent))
      {
        QBI_LOG_I_0("Connect successful on temp profile, set profile to default.");
        action = qbi_svc_bc_connect_s_activate_configure_default_profile(qmi_txn->parent);
      }
    }
  }

  /* Free previously allocated info buffer used for profile handling.
     Moved freeing of info buffer from SNI request to SNI response as
     the information in info buffer is used for setting the profile
     for connect to default */
  if (QBI_SVC_ACTION_WAIT_ASYNC_RSP != action && 
    QBI_SVC_ACTION_SEND_QMI_REQ != action &&
    qmi_txn->parent->info != NULL)
  {
    QBI_MEM_FREE(qmi_txn->parent->info);
    qmi_txn->parent->info = NULL;
  }

  return action;
} /* qbi_svc_bc_connect_s_wds20_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_wds21_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_WDS_STOP_NETWORK_INTERFACE_RESP for
    MBIM_CID_CONNECT set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_wds21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_stop_network_interface_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_stop_network_interface_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    (void) qbi_svc_bc_connect_update_cache_activation_state(
      qmi_txn->ctx, QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED, qmi_txn->svc_id);

    if (qmi_txn->parent->qmi_txns_pending)
    {
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
    else
    {
      action = qbi_svc_bc_connect_eqs_build_rsp_from_cache(
        qmi_txn->parent,
        QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_txn->svc_id));

      if (action == QBI_SVC_ACTION_SEND_RSP)
      {
        action = qbi_svc_bc_connect_s_deactivate_build_wds4a_req(
          qmi_txn->parent, QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_txn->svc_id));
      }
    }
  }

  return action;
} /* qbi_svc_bc_connect_s_wds21_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_wait_for_teardown
===========================================================================*/
/*!
    @brief Called during MBIM_OPEN processing to ensure that any previous
    connection from earlier MBIM session is disconnected before proceeding

    @details
    In the event that the MBIM session is restarted while a call was up,
    that call can take several seconds to disconnect. This ensures that we
    wait long enough for the modem to complete this action before
    continuing.

    @param txn MBIM_OPEN transaction

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_wait_for_teardown
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  uint32 session_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  for (session_id = 0; session_id < qbi_svc_bc_max_sessions_get(txn->ctx);
       session_id++)
  {
    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id),
      QMI_WDS_GET_PKT_SRVC_STATUS_REQ_V01,
      qbi_svc_bc_connect_wait_for_teardown_wds22_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id),
      QMI_WDS_GET_PKT_SRVC_STATUS_REQ_V01,
      qbi_svc_bc_connect_wait_for_teardown_wds22_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  }

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_connect_wait_for_teardown() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_wait_for_teardown_wds22_ind_cb
===========================================================================*/
/*!
    @brief QMI_WDS_PKT_SRVC_STATUS_IND handler which waits for the data
    call to be torn down before allowing the MBIM_OPEN transaction to
    continue

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_wait_for_teardown_wds22_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);

  /* The static indication handler will process before this, updating the cache,
     so we do not need to look at the QMI response directly. */
  if (ind->txn->qmi_txns_pending == 0 &&
      !qbi_svc_bc_connect_is_connected(ind->txn->ctx))
  {
    /* This transaction is reused for opening other services, so we need to
       explicitly reset the indication handler */
    QBI_LOG_I_0("No longer connected - proceeding with MBIM_OPEN");
    qbi_svc_ind_dereg_txn(ind->txn);
    action = qbi_svc_bc_connect_s_deactivate_build_wds4a_req(ind->txn,
        QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(ind->qmi_svc_id));
  }
  else
  {
    QBI_LOG_I_0("Still waiting for teardown and/or cache init to finish");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_connect_wait_for_teardown_wds22_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_wait_for_teardown_wds22_rsp_cb
===========================================================================*/
/*!
    @brief QMI_WDS_GET_PKT_SRVC_STATUS response handler that checks if a
    data call from a previous MBIM session is still up

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_wait_for_teardown_wds22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_get_pkt_srvc_status_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_connect_ip_type_state_s *ip_type_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  ip_type_state = qbi_svc_bc_connect_get_cached_ip_type_state(
    qmi_txn->ctx, qmi_txn->svc_id);
  QBI_CHECK_NULL_PTR_RET_ABORT(ip_type_state);

  qmi_rsp = (wds_get_pkt_srvc_status_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    ip_type_state->activation_state =
      qbi_svc_bc_connect_qmi_conn_status_to_mbim(qmi_rsp->connection_status);
    QBI_LOG_I_2("Initialized activation state for wds_svc_id %d to %d",
                qmi_txn->svc_id, ip_type_state->activation_state);
    if (ip_type_state->activation_state !=
          QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED)
    {
      QBI_LOG_W_0("Previous connection not torn down yet! Delaying MBIM_OPEN");
      if (!qbi_svc_ind_reg_dynamic(
            qmi_txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_CONNECT,
            qmi_txn->svc_id, QMI_WDS_PKT_SRVC_STATUS_IND_V01,
            qbi_svc_bc_connect_wait_for_teardown_wds22_ind_cb,
            qmi_txn->parent, NULL))
      {
        QBI_LOG_E_0("Couldn't register dynamic indication handler!");
        action = QBI_SVC_ACTION_ABORT;
      }
    }
    else if (qmi_txn->parent->qmi_txns_pending == 0 &&
             !qbi_svc_bc_connect_is_connected(qmi_txn->ctx))
    {
      QBI_LOG_I_0("Basic connectivity device service now opened");
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }

  return action;
} /* qbi_svc_bc_connect_wait_for_teardown_wds22_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_wds2a_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_LIST_RESP for
    MBIM_CID_CONNECT set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_list_req_msg_v01 *qmi_req;
  wds_get_profile_list_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_connect_profiles_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);

  qmi_req = (wds_get_profile_list_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_list_resp_msg_v01 *) qmi_txn->rsp.data;

  /* Modem may reject EPC profile type depending on modem configuration,
     proceed wihtout failing the query. */
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      !(qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
        qmi_rsp->resp.error == QMI_ERR_EXTENDED_INTERNAL_V01 &&
        qmi_rsp->extended_error_code_valid && qmi_rsp->extended_error_code ==
          WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE_V01))
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    if (qmi_rsp->profile_list_len > QMI_WDS_PROFILE_LIST_MAX_V01)
    {
      QBI_LOG_E_1("Invalid number of profiles %d", qmi_rsp->profile_list_len);
    }
    else
    {
      info = (qbi_svc_bc_connect_profiles_info_s *) qmi_txn->parent->info;
      if (qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01)
      {
        if (qbi_svc_bc_populate_profile_list(&info->profile_list, qmi_rsp))
        {
          info->num_of_profile_epc = qmi_rsp->profile_list_len;

          qmi_req = (wds_get_profile_list_req_msg_v01 *)
            qbi_qmi_txn_alloc_ret_req_buf(
              qmi_txn->parent, QBI_QMI_SVC_WDS,
              QMI_WDS_GET_PROFILE_LIST_REQ_V01,
              qbi_svc_bc_connect_s_wds2a_rsp_cb);
          QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

          qmi_req->profile_type_valid = TRUE;
          qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP_V01;
          action = QBI_SVC_ACTION_SEND_QMI_REQ;
        }
      }
      else if (qmi_req->profile_type == WDS_PROFILE_TYPE_3GPP_V01)
      {
        if (qbi_svc_bc_populate_profile_list(&info->profile_list, qmi_rsp))
        {
          info->num_of_profile_3gpp = qmi_rsp->profile_list_len;

          if (qbi_svc_bc_device_supports_3gpp2(qmi_txn->ctx))
          {
            qmi_req = (wds_get_profile_list_req_msg_v01 *)
              qbi_qmi_txn_alloc_ret_req_buf(
                qmi_txn->parent, QBI_QMI_SVC_WDS,
                QMI_WDS_GET_PROFILE_LIST_REQ_V01,
                qbi_svc_bc_connect_s_wds2a_rsp_cb);
            QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

            qmi_req->profile_type_valid = TRUE;
            qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
            action = QBI_SVC_ACTION_SEND_QMI_REQ;
          }
        }
      }
      else
      {
        if (qbi_svc_bc_populate_profile_list(&info->profile_list, qmi_rsp))
        {
          info->num_of_profile_3gpp2 = qmi_rsp->profile_list_len;
        }
      }

      if (action != QBI_SVC_ACTION_SEND_QMI_REQ)
      {
        action = qbi_svc_bc_connect_s_activate_get_next_profile(
          qmi_txn->parent);
      }
    }
  }

  return action;
} /* qbi_svc_bc_connect_s_wds2a_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_wds2b_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
    MBIM_CID_CONNECT set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_s_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_connect_profiles_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  
  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    info = (qbi_svc_bc_connect_profiles_info_s *) qmi_txn->parent->info;
    info->profiles_read++;
    QBI_LOG_I_2("Received profile %d/%d", info->profiles_read,
                info->num_of_profile_epc + info->num_of_profile_3gpp +
                  info->num_of_profile_3gpp2);

    /* Compare APN name in profile with the one in CONNECT req */
    if (qbi_svc_bc_connect_s_activate_is_profile_matched(qmi_txn, qmi_rsp))
    {
      if ((info->profiles_read <= info->num_of_profile_epc) &&
          !info->profile_found_epc)
      {
        info->profile_found_epc = TRUE;
        info->profile_index_epc =
          info->profile_list.profile_index[info->profiles_read - 1];

        /* EPC profile is prefered profile type for SNI. Skip 3GPP/3GPP2 profile
           scanning if matching EPC is found */
        info->profiles_read = info->num_of_profile_epc +
          info->num_of_profile_3gpp + info->num_of_profile_3gpp2;
      }
      else if ((info->profiles_read <= info->num_of_profile_epc +
                 info->num_of_profile_3gpp) &&
               !info->profile_found_3gpp)
      {
        info->profile_found_3gpp = TRUE;
        info->profile_index_3gpp =
          info->profile_list.profile_index[info->profiles_read - 1];
        info->profiles_read =
          info->num_of_profile_epc + info->num_of_profile_3gpp;
      }
      else if ((info->profiles_read > info->num_of_profile_epc +
                 info->num_of_profile_3gpp) &&
               !info->profile_found_3gpp2)
      {
        info->profile_found_3gpp2 = TRUE;
        info->profile_index_3gpp2 =
          info->profile_list.profile_index[info->profiles_read - 1];
        info->profiles_read = info->num_of_profile_epc +
          info->num_of_profile_3gpp + info->num_of_profile_3gpp2;
      }
    }

    action = qbi_svc_bc_connect_s_activate_get_next_profile(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_connect_s_wds2b_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_session_activation_state
===========================================================================*/
/*!
    @brief Combines the current cached activation states of the IPV4 and
    IPV6 WDS instances into a single overall MBIM activation state value

    @details

    @param ctx
    @param session_id

    @return uint32 MBIM activation state
*/
/*=========================================================================*/
uint32 qbi_svc_bc_connect_session_activation_state
(
  const qbi_ctx_s *ctx,
  uint32           session_id
)
{
  qbi_svc_bc_cache_s *cache;
  uint32 requested_ip_type;
  uint32 activation_state;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  requested_ip_type = QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(
    cache->connect.sessions[session_id].requested_ip_type);
  switch (requested_ip_type)
  {
    case QBI_SVC_BC_IP_TYPE_IPV4:
      activation_state = cache->connect.sessions[session_id].ipv4.activation_state;
      break;

    case QBI_SVC_BC_IP_TYPE_IPV6:
      activation_state = cache->connect.sessions[session_id].ipv6.activation_state;
      break;

    case QBI_SVC_BC_IP_TYPE_IPV4V6:
      activation_state = qbi_svc_bc_connect_combine_activation_states(
        cache->connect.sessions[session_id].ipv4.activation_state,
        cache->connect.sessions[session_id].ipv6.activation_state, FALSE);
      break;

    case QBI_SVC_BC_IP_TYPE_IPV4_AND_IPV6:
      activation_state = qbi_svc_bc_connect_combine_activation_states(
        cache->connect.sessions[session_id].ipv4.activation_state,
        cache->connect.sessions[session_id].ipv6.activation_state, TRUE);
      break;

    default:
      QBI_LOG_E_1("Unexpected requested IP type %d", requested_ip_type);
      activation_state = QBI_SVC_BC_ACTIVATION_STATE_UNKNOWN;
      break;
  }

  return activation_state;
} /* qbi_svc_bc_connect_session_activation_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_update_cache_svc_id
===========================================================================*/
/*!
    @brief Updates the cached values pertaining to MBIM_CID_CONNECT

    @details
    QBI_SVC_BC_CACHE_NO_CHANGE_U32 is not supported for any of the values

    @param ctx
    @param activation_state
    @param requested_ip_type IP type requested by the host. Required if the
    new activation state is ACTIVATING, ignored otherwise.
    @param context_type Type of context being activated. Required if the new
    activation state is ACTIVATING, ignored otherwise.
    @param is_resync Set to TRUE if this cache update is due to a resync
    operation
    @param wds_svc_id

    @return boolean TRUE if the activation state changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_update_cache_svc_id
(
  qbi_ctx_s    *ctx,
  uint32        activation_state,
  uint32        requested_ip_type,
  const uint8  *context_type,
  boolean       is_resync,
  qbi_qmi_svc_e wds_svc_id
)
{
  qbi_svc_bc_cache_s *cache;
  boolean changed = FALSE;
  uint32 old_activation_state;
  uint32 session_id;
  qbi_svc_bc_connect_ip_type_state_s *ip_type_state = NULL;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  ip_type_state = qbi_svc_bc_connect_get_cached_ip_type_state(ctx, wds_svc_id);
  session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(wds_svc_id);
  QBI_CHECK_NULL_PTR_RET_FALSE(ip_type_state);

  if (activation_state != ip_type_state->activation_state)
  {
    QBI_LOG_I_3("Context activation state on wds_svc_id %d changed from %d to %d",
                wds_svc_id, ip_type_state->activation_state, activation_state);

    if (!is_resync)
    {
      if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING)
      {
        QBI_LOG_I_2("Set session ID to %d, requested IP type to %d, due to "
                    "ACTIVATING state", session_id, requested_ip_type);
        cache->connect.sessions[session_id].requested_ip_type = requested_ip_type;
        QBI_MEMSCPY(cache->connect.sessions[session_id].context_type,
                    sizeof(cache->connect.sessions[session_id].context_type),
                    context_type, QBI_MBIM_UUID_LEN);
      }
      else if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED &&
               !ip_type_state->qmi.pkt_handle_valid)
      {
        QBI_LOG_E_2("Session ID %d activated, but missing required QMI "
                    "values! wds_svc_id %d pkt_handle_valid %d",
                    wds_svc_id, ip_type_state->qmi.pkt_handle_valid);
      }
    }
    else if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED &&
             !ip_type_state->qmi.pkt_handle_valid)
    {
      /* In this situation, the interface is connected, but we don't have the
         proper handle to tear it down via a QMI_WDS_STOP_NETWORK_INTERFACE_REQ.
         This is generally only possible if we couldn't allocate memory to
         process a successful QMI_WDS_START_NETWORK_INTERFACE_RESP. The only
         recovery method is to release our WDS client handles, and to do that,
         we send a ERROR_UNKNOWN message to the host, which should result in it
         initiating a reset function operation. */
      QBI_LOG_E_0("Resync led to ACTIVATED state with no packet data handle!");
      qbi_msg_send_error(ctx, 0, QBI_MBIM_ERROR_UNKNOWN);
    }

    old_activation_state = ip_type_state->activation_state;
    ip_type_state->activation_state = activation_state;
    changed = TRUE;

    if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED)
    {
      ip_type_state->qmi.pkt_handle_valid = FALSE;

      if (old_activation_state != QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING)
      {
        qbi_svc_bc_packet_statistics_finalize_session(ctx, wds_svc_id);
        if (qbi_svc_bc_connect_overall_activation_state(ctx) ==
              QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED)
        {
          /* Make sure cache only flag is turned off upon disconnect */
          qbi_svc_bc_nas_register_state_call_disconnected_check_cache_only_flag(
            ctx);
        }
      }

      /* Loopback state should always be cleared to not affect regular connect */
      if (qbi_svc_bc_connect_is_loopback(ctx) &&
          qbi_svc_bc_connect_overall_activation_state(ctx) ==
            QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED)
      {
        qbi_svc_bc_connect_disable_loopback_state(ctx);
      }
    }
  }

  return changed;
} /* qbi_svc_bc_connect_update_cache_svc_id() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_update_cache_activating
===========================================================================*/
/*!
    @brief Updates the CONNECT cache with new activation state ACTIVATING,
    while also setting the other required values for this state

    @details

    @param ctx
    @param session_id
    @param requested_ip_type
    @param context_type
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_update_cache_activating
(
  qbi_ctx_s    *ctx,
  uint32        session_id,
  uint32        requested_ip_type,
  const uint8  *context_type
)
{
/*-------------------------------------------------------------------------*/
  if (QBI_SVC_BC_CONNECT_IPV4_REQUESTED(requested_ip_type))
  {
    (void) qbi_svc_bc_connect_update_cache_svc_id(
      ctx, QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING, requested_ip_type,
      context_type, FALSE,
      QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id));
  }
  if (QBI_SVC_BC_CONNECT_IPV6_REQUESTED(requested_ip_type))
  {
    (void) qbi_svc_bc_connect_update_cache_svc_id(
      ctx, QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING, requested_ip_type,
      context_type, FALSE,
      QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id));
  }
} /* qbi_svc_bc_connect_update_cache_activating() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_update_cache_activation_state
===========================================================================*/
/*!
    @brief Used to update the CONNECT cache with a new activation state

    @details
    Not used when setting the activation state to ACTIVATED - use
    qbi_svc_bc_connect_update_cache_activating() for that purpose, as
    additional values are required.

    @param ctx
    @param activation_state
    @param wds_svc_id

    @return boolean TRUE if cached values changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_update_cache_activation_state
(
  qbi_ctx_s    *ctx,
  uint32        activation_state,
  qbi_qmi_svc_e wds_svc_id
)
{
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  if (activation_state != QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING)
  {
    changed = qbi_svc_bc_connect_update_cache_svc_id(
      ctx, activation_state, 0, 0, FALSE, wds_svc_id);
  }
  else
  {
    QBI_LOG_E_0("Extra parameters required to set ACTIVATING state!");
  }

  return changed;
} /* qbi_svc_bc_connect_update_cache_activation_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_update_cache_loopback_state
===========================================================================*/
/*!
    @brief Used to update the CONNECT cache with a new loopback state

    @details

    @param ctx
    @param session_id
    @param loopback_state
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_update_cache_loopback_state
(
  qbi_ctx_s    *ctx,
  uint32        session_id,
  boolean       loopback_state
)
{
  qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache->connect.sessions[session_id].is_loopback != loopback_state)
  {
    QBI_LOG_I_2("Loopback state changed from %d to %d",
                cache->connect.sessions[session_id].is_loopback, loopback_state);
    cache->connect.sessions[session_id].is_loopback = loopback_state;
  }
} /* qbi_svc_bc_connect_update_cache_loopback_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_update_cache_qmi_pkt_handle
===========================================================================*/
/*!
    @brief Updates the CONNECT cache with a new QMI packet data handle,
    returned by QMI_WDS_START_NETWORK_INTERFACE

    @details
    This should only be called while the activation state is ACTIVATING

    @param ctx
    @param qmi_pkt_handle
    @param wds_svc_id
*/
/*=========================================================================*/
static void qbi_svc_bc_connect_update_cache_qmi_pkt_handle
(
  qbi_ctx_s    *ctx,
  uint32        qmi_pkt_handle,
  qbi_qmi_svc_e wds_svc_id
)
{
  qbi_svc_bc_connect_ip_type_state_s *ip_type_state = NULL;
/*-------------------------------------------------------------------------*/
  ip_type_state = qbi_svc_bc_connect_get_cached_ip_type_state(ctx, wds_svc_id);
  QBI_CHECK_NULL_PTR_RET(ip_type_state);

  if (ip_type_state->qmi.pkt_handle_valid &&
      qmi_pkt_handle != ip_type_state->qmi.pkt_handle)
  {
    QBI_LOG_W_2("Updating QMI packet data handle for wds_svc_id %d, but already "
                "valid with different value! Previous value 0x%x",
                wds_svc_id, ip_type_state->qmi.pkt_handle);
  }

  ip_type_state->qmi.pkt_handle_valid = TRUE;
  ip_type_state->qmi.pkt_handle = qmi_pkt_handle;
  QBI_LOG_I_2("Set QMI packet data handle for wds_svc_id %d to 0x%x",
              wds_svc_id, qmi_pkt_handle);
} /* qbi_svc_bc_connect_update_cache_qmi_pkt_handle() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_update_cache_resync_svc_id
===========================================================================*/
/*!
    @brief Updates the CONNECT cache based on the current activation state
    retrieved from QMI by query

    @details

    @param ctx
    @param activation_state
    @param wds_svc_id

    @return boolean TRUE if the activation state changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_connect_update_cache_resync_svc_id
(
  qbi_ctx_s    *ctx,
  uint32        activation_state,
  qbi_qmi_svc_e wds_svc_id
)
{
/*-------------------------------------------------------------------------*/
  return qbi_svc_bc_connect_update_cache_svc_id(
   ctx, activation_state, 0, 0, TRUE, wds_svc_id);
} /* qbi_svc_bc_connect_update_cache_resync_svc_id() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_wda29_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDA_SET_LOOPBACK_STATE_RESP_V01 for disabling loopback
           state

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_connect_wda29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wda_set_loopback_state_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wda_set_loopback_state_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_INVALID_QMI_CMD_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    qbi_svc_bc_connect_update_cache_loopback_state(qmi_txn->ctx, 0, FALSE);
  }

  QBI_LOG_D_0("Releasing WDA client");
  qbi_qmi_release_svc_handle(qmi_txn->ctx, QBI_QMI_SVC_WDA);

  return QBI_SVC_ACTION_ABORT;
} /* qbi_svc_bc_connect_wda29_rsp_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_PROVISIONED_CONTEXTS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_provisioned_contexts_add_context_to_rsp
===========================================================================*/
/*!
    @brief Allocates and populates a MBIM_CONTEXT_STATE structure on the
    response

    @details

    @param txn
    @param field_desc
    @param profile_type
    @param qmi_rsp
    @param context_id

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_provisioned_contexts_add_context_to_rsp
(
  qbi_txn_s                                   *txn,
  qbi_mbim_offset_size_pair_s                 *field_desc,
  wds_profile_type_enum_v01                    profile_type,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp,
  uint32                                       context_id
)
{
  boolean success = FALSE;
  uint32 initial_offset;
  qbi_svc_bc_provisioned_contexts_context_s *context;
  const uint8 *context_type;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);

  initial_offset = txn->infobuf_len_total;
  context = (qbi_svc_bc_provisioned_contexts_context_s *)
    qbi_txn_rsp_databuf_add_field(
      txn, field_desc, 0, sizeof(qbi_svc_bc_provisioned_contexts_context_s),
      NULL);
  QBI_CHECK_NULL_PTR_RET_FALSE(context);

  context_type =
    qbi_svc_bc_provisioned_contexts_qmi_profile_to_context_type(qmi_rsp);
  QBI_CHECK_NULL_PTR_RET_FALSE(context_type);

  QBI_MEMSCPY(context->context_type, sizeof(context->context_type),
              context_type, QBI_MBIM_UUID_LEN);

  context->context_id = context_id;
  context->compression =
    qbi_svc_bc_provisioned_contexts_qmi_profile_to_mbim_compression(
      profile_type, qmi_rsp);
  context->auth_protocol =
    qbi_svc_bc_provisioned_contexts_qmi_profile_to_mbim_auth_proto(
      profile_type, qmi_rsp);

  /* Populate the DataBuffer - note that the same information is contained
     in different TLVs for EPC, 3GPP and 3GPP2 profiles. For example, 3GPP
     username is in TLV 0x1B (username) while the 3GPP2 one is in TLV 0x9B
     (user_id) */
  if (profile_type == WDS_PROFILE_TYPE_EPC_V01)
  {
    if (qmi_rsp->apn_name_valid &&
        !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          txn, &context->access_string, initial_offset,
          QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES, qmi_rsp->apn_name,
          sizeof(qmi_rsp->apn_name)))
    {
      QBI_LOG_E_0("Couldn't add EPC AccessString to response!");
    }
    else if (qmi_rsp->common_user_id_valid &&
             !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
               txn, &context->username, initial_offset,
               QBI_SVC_BC_USERNAME_MAX_LEN_BYTES, qmi_rsp->common_user_id,
               sizeof(qmi_rsp->common_user_id)))
    {
      QBI_LOG_E_0("Couldn't add EPC Username to response!");
    }
    else if (qmi_rsp->common_auth_password_valid &&
             !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
               txn, &context->password, initial_offset,
               QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES,
               qmi_rsp->common_auth_password,
               sizeof(qmi_rsp->common_auth_password)))
    {
      QBI_LOG_E_0("Couldn't add EPC Password to response!");
    }
    else
    {
      success = TRUE;
    }
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    if (qmi_rsp->apn_name_valid &&
        !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          txn, &context->access_string, initial_offset,
          QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES, qmi_rsp->apn_name,
          sizeof(qmi_rsp->apn_name)))
    {
      QBI_LOG_E_0("Couldn't add 3GPP AccessString to response!");
    }
    else if (qmi_rsp->username_valid &&
             !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
               txn, &context->username, initial_offset,
               QBI_SVC_BC_USERNAME_MAX_LEN_BYTES, qmi_rsp->username,
               sizeof(qmi_rsp->username)))
    {
      QBI_LOG_E_0("Couldn't add 3GPP Username to response!");
    }
    else if (qmi_rsp->password_valid &&
             !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
               txn, &context->password, initial_offset,
               QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES,
               qmi_rsp->password, sizeof(qmi_rsp->password)))
    {
      QBI_LOG_E_0("Couldn't add 3GPP Password to response!");
    }
    else
    {
      success = TRUE;
    }
  }
  else
  {
    QBI_LOG_E_1("Unexpected profile type %d", profile_type);
  }

  if (success)
  {
    /* Update the size field to include DataBuffer items */
    field_desc->size = txn->infobuf_len_total - initial_offset;
    success = qbi_txn_rsp_databuf_consolidate(txn);
  }

  return success;
} /* qbi_svc_provisioned_contexts_add_context_to_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_build_wds27_req
===========================================================================*/
/*!
    @brief Allocates and populates a request for QMI_WDS_CREATE_PROFILE

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_build_wds27_req
(
  qbi_txn_s *txn
)
{
  wds_create_profile_req_msg_v01 *qmi_req;
  qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s profile_settings;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  qmi_req = (wds_create_profile_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_CREATE_PROFILE_REQ_V01,
      qbi_svc_bc_provisioned_contexts_s_wds27_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  /*! @note Starting from 9x35, EPC profile type is available to replace
      legacy 3gpp and 3gpp2 profiles. EPC profile contains tech specific
      settings for both 3gpp/3gpp2 and can support data continuity with
      single profile. Create EPC profile by populating common settings
      shared by 3gpp and 3gpp2.

      If EPC profile support is disabled on modem, create legacy 3gpp/3gpp2
      profiles instead. */
  qmi_req->profile_type = WDS_PROFILE_TYPE_EPC_V01;
  if (!qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds27(
        qmi_req, &profile_settings, WDS_PROFILE_TYPE_EPC_V01))
  {
    QBI_LOG_E_0("Couldn't collect profile setting pointers!");
  }
  else
  {
    action = qbi_svc_bc_provisioned_contexts_s_populate_profile(
      txn, qmi_req->profile_type, &profile_settings);
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_build_wds27_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_id_is_valid
===========================================================================*/
/*!
    @brief Checks whether a PROVISIONED_CONTEXTS ContextId is within the
    supported range

    @details

    @param context_id

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_provisioned_contexts_id_is_valid
(
  uint32 context_id
)
{
/*-------------------------------------------------------------------------*/
  return (context_id > 0 && context_id <= QBI_SVC_BC_PROVISIONED_CONTEXTS_MAX);
} /* qbi_svc_bc_provisioned_contexts_id_is_valid() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_q_get_next_profile
===========================================================================*/
/*!
    @brief Retrive next available configured profile.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_q_get_next_profile
(
  qbi_txn_s *txn
)
{
  wds_get_profile_settings_req_msg_v01 *qmi_req_wds2b;
  qbi_svc_bc_provisioned_contexts_profiles_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_provisioned_contexts_profiles_info_s *) txn->info;
  if (info->profiles_read >= info->profile_list.num_of_profile)
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    /* Issue a query to retrieve the profile details */
    qmi_req_wds2b = (wds_get_profile_settings_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_provisioned_contexts_q_wds2b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_wds2b);

    qmi_req_wds2b->profile.profile_type =
      info->profile_list.profile_type[info->profiles_read];
    qmi_req_wds2b->profile.profile_index =
      info->profile_list.profile_index[info->profiles_read];

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_q_get_next_profile */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PROVISIONED_CONTEXTS query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_q_req
(
  qbi_txn_s *txn
)
{
  wds_get_profile_list_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* Send QMI_WDS_GET_PROFILE_LIST_REQ to obtain EPC profile list first, then
     obtain 3GPP profile list afterwards. Starting from 9x35, EPC profile
     should be the primary profile when deployed by OEM, but legacy profile
     types are still supported for now to maintain backward compatibility. */
  qmi_req = (wds_get_profile_list_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_LIST_REQ_V01,
      qbi_svc_bc_provisioned_contexts_q_wds2a_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile_type_valid = TRUE;
  qmi_req->profile_type = WDS_PROFILE_TYPE_EPC_V01;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_provisioned_contexts_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_q_wds2a_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_LIST_RESP for
    MBIM_CID_PROVISIONED_CONTEXTS query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_q_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_list_req_msg_v01 *qmi_req;
  wds_get_profile_list_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_provisioned_contexts_profiles_info_s *info;
  qbi_svc_bc_provisioned_contexts_list_s *rsp_list;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wds_get_profile_list_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_list_resp_msg_v01 *) qmi_txn->rsp.data;

  /* Modem may reject EPC profile type depending on modem configuration,
     proceed wihtout failing the query. */
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      !(qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
        qmi_rsp->resp.error == QMI_ERR_EXTENDED_INTERNAL_V01 &&
        qmi_rsp->extended_error_code_valid && qmi_rsp->extended_error_code ==
          WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE_V01))
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_provisioned_contexts_set_mbim_error_status(
      qmi_txn->parent, qmi_rsp->resp.error, qmi_rsp->extended_error_code_valid,
      qmi_rsp->extended_error_code);
  }
  else if (qmi_rsp->profile_list_len > QMI_WDS_PROFILE_LIST_MAX_V01)
  {
    QBI_LOG_E_1("Invalid number of profiles %d", qmi_rsp->profile_list_len);
  }
  else
  {
    if (qmi_txn->parent->info == NULL)
    {
      qmi_txn->parent->info = QBI_MEM_MALLOC_CLEAR(
        sizeof(qbi_svc_bc_provisioned_contexts_profiles_info_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
    }

    /* Append new profile indexes to the profile list */
    info = (qbi_svc_bc_provisioned_contexts_profiles_info_s *)
      qmi_txn->parent->info;
    qbi_svc_bc_populate_profile_list(&info->profile_list, qmi_rsp);

    if (qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01)
    {
      /* Send another QMI_WDS_GET_PROFILE_LIST_REQ to obtain 3GPP profile
         list */
      qmi_req = (wds_get_profile_list_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_LIST_REQ_V01,
          qbi_svc_bc_provisioned_contexts_q_wds2a_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      qmi_req->profile_type_valid = TRUE;
      qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
    else
    {
      /* Allocate the fixed-length and offset/size pair portion of the
         response now. */
      rsp_list = (qbi_svc_bc_provisioned_contexts_list_s *)
        qbi_txn_alloc_rsp_buf(
          qmi_txn->parent, (sizeof(qbi_svc_bc_provisioned_contexts_list_s) +
                            sizeof(qbi_mbim_offset_size_pair_s) *
                            info->profile_list.num_of_profile));
      QBI_CHECK_NULL_PTR_RET_ABORT(rsp_list);
      rsp_list->element_count = info->profile_list.num_of_profile;

      info->profiles_read = 0;
      action = qbi_svc_bc_provisioned_contexts_q_get_next_profile(
        qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_q_wds2a_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_q_wds2b_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
    MBIM_CID_PROVISIONED_CONTEXTS query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_q_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_req_msg_v01 *qmi_req;
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp;
  qbi_mbim_offset_size_pair_s *field_desc;
  qbi_svc_bc_provisioned_contexts_profiles_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wds_get_profile_settings_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_provisioned_contexts_set_mbim_error_status(
      qmi_txn->parent, qmi_rsp->resp.error,
      qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }
  else
  {
    info = (qbi_svc_bc_provisioned_contexts_profiles_info_s *)
      qmi_txn->parent->info;

    field_desc = (qbi_mbim_offset_size_pair_s *)
      ((uint8 *) qmi_txn->parent->rsp.data +
       sizeof(qbi_svc_bc_provisioned_contexts_list_s) +
       sizeof(qbi_mbim_offset_size_pair_s) * info->profiles_read);

    info->profiles_read++;
    QBI_LOG_I_2("Received profile %d/%d", info->profiles_read,
                info->profile_list.num_of_profile);

    if (!qbi_svc_provisioned_contexts_add_context_to_rsp(
          qmi_txn->parent, field_desc, qmi_req->profile.profile_type,
          qmi_rsp, qmi_req->profile.profile_index))
    {
      QBI_LOG_E_0("Couldn't add context to response!");
    }
    else
    {
      action = qbi_svc_bc_provisioned_contexts_q_get_next_profile(
        qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_q_wds2b_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_qmi_profile_to_context_type
===========================================================================*/
/*!
    @brief Determines the ContextType UUID for QMI profile common app user
           data TLV

    @details

    @param context_id

    @return const uint8* Pointer to UUID, or NULL on unexpected error
*/
/*=========================================================================*/
static const uint8 *qbi_svc_bc_provisioned_contexts_qmi_profile_to_context_type
(
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
)
{
  const uint8 *context_type = NULL;
  uint32 app_user_data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(qmi_rsp);

  if (qmi_rsp->common_app_user_data_valid)
  {
    app_user_data = qmi_rsp->common_app_user_data;
  }
  else if (qmi_rsp->app_user_data_3gpp_valid)
  {
    app_user_data = qmi_rsp->app_user_data_3gpp;
  }
  else if (qmi_rsp->app_user_data_3gpp2_valid)
  {
    app_user_data = qmi_rsp->app_user_data_3gpp2;
  }
  else
  {
    QBI_LOG_E_0("App User Data TLVs are not present in response!");
    app_user_data = QBI_SVC_BC_CONTEXT_TYPE_INTERNET;
  }

  context_type = qbi_svc_bc_context_type_id_to_uuid(app_user_data);
  if (context_type == NULL)
  {
    QBI_LOG_I_0("Couldn't determine ContextType UUID based on context type");
    context_type = qbi_svc_bc_context_type_id_to_uuid(
      QBI_SVC_BC_CONTEXT_TYPE_INTERNET);
  }

  return context_type;
} /* qbi_svc_bc_provisioned_contexts_qmi_profile_to_context_type() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_qmi_profile_to_mbim_auth_proto
===========================================================================*/
/*!
    @brief Extracts the authentication protocol information from a QMI
    profile (EPC or 3GPP), if available, and returns it as an MBIM value

    @details

    @param profile_type
    @param qmi_rsp

    @return uint32 MBIM_AUTH_PROTOCOL value
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_provisioned_contexts_qmi_profile_to_mbim_auth_proto
(
  wds_profile_type_enum_v01                    profile_type,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
)
{
  uint32 mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_NONE;
/*-------------------------------------------------------------------------*/
  if (qmi_rsp == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
  }
  else if (profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
           qmi_rsp->common_auth_protocol_valid)
  {
    if (qmi_rsp->common_auth_protocol == WDS_PROFILE_AUTH_PROTOCOL_CHAP_V01 ||
        qmi_rsp->common_auth_protocol == WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01)
    {
      mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_CHAP;
    }
    else if (qmi_rsp->common_auth_protocol == WDS_PROFILE_AUTH_PROTOCOL_PAP_V01)
    {
      mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_PAP;
    }
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP_V01 &&
           qmi_rsp->authentication_preference_valid)
  {
    if (qmi_rsp->authentication_preference & QMI_WDS_MASK_AUTH_PREF_CHAP_V01)
    {
      mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_CHAP;
    }
    else if (qmi_rsp->authentication_preference & QMI_WDS_MASK_AUTH_PREF_PAP_V01)
    {
      mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_PAP;
    }
  }
  else
  {
    QBI_LOG_E_1("Invalid profile type %d", profile_type);
  }

  return mbim_auth_proto;
} /* qbi_svc_bc_provisioned_contexts_qmi_auth_pref_to_mbim_auth_proto() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_qmi_profile_to_mbim_compression
===========================================================================*/
/*!
    @brief Extracts the compression information from a QMI profile (3GPP or
    3GPP2), if available, and returns it as an MBIM value

    @details

    @param profile_type
    @param qmi_rsp

    @return uint32 MBIM_COMPRESSION value
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_provisioned_contexts_qmi_profile_to_mbim_compression
(
  wds_profile_type_enum_v01                    profile_type,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
)
{
  uint32 mbim_compression = QBI_SVC_BC_COMPRESSION_NONE;
/*-------------------------------------------------------------------------*/
  if (qmi_rsp == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
  }
  else if (profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
           ((qmi_rsp->pdp_data_compression_type_valid &&
             qmi_rsp->pdp_data_compression_type !=
               WDS_PDP_DATA_COMPR_TYPE_OFF_V01) ||
            (qmi_rsp->pdp_hdr_compression_type_valid &&
             qmi_rsp->pdp_hdr_compression_type !=
               WDS_PDP_HDR_COMPR_TYPE_OFF_V01)))
  {
    mbim_compression = QBI_SVC_BC_COMPRESSION_ENABLE;
  }

  return mbim_compression;
} /* qbi_svc_bc_provisioned_contexts_qmi_profile_to_mbim_compression() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_3gpp_wds2b_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
    MBIM_CID_PROVISIONED_CONTEXTS set request to retrieve settings for a
    3gpp profile

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_3gpp_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_provisioned_contexts_profiles_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_provisioned_contexts_set_mbim_error_status(
      qmi_txn->parent, qmi_rsp->resp.error,
      qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }
  else
  {
    /* If 3gpp2 is supported, issue a 3gpp2 profile list query then read
       3gpp2 profiles to compare with 3gpp profile settings stored in info
       buffer, until a matching profile found. Otherwise directly proceed
       with profile modification. */
    if (qbi_svc_bc_device_supports_3gpp2(qmi_txn->ctx))
    {
      info = (qbi_svc_bc_provisioned_contexts_profiles_info_s *)
        qmi_txn->parent->info;

      (void) qbi_svc_bc_provisioned_contexts_s_save_3gpp_profile_settings(
        &info->profile_settings_3gpp, qmi_rsp);

      action = qbi_svc_bc_provisioned_contexts_s_get_next_3gpp2_profile(
        qmi_txn->parent);
    }
    else
    {
      action = qbi_svc_bc_provisioned_contexts_s_build_wds28_req(
        qmi_txn->parent, WDS_PROFILE_TYPE_3GPP_V01);
    }
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_s_3gpp_wds2b_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_3gpp2_wds2b_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
    MBIM_CID_PROVISIONED_CONTEXTS set request to retrieve settings for a
    3gpp2 profile

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_3gpp2_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_provisioned_contexts_profiles_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_provisioned_contexts_set_mbim_error_status(
      qmi_txn->parent, qmi_rsp->resp.error,
      qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }
  else
  {
    info = (qbi_svc_bc_provisioned_contexts_profiles_info_s *)
      qmi_txn->parent->info;

    if (qbi_svc_bc_provisioned_contexts_s_compare_3gpp_3gpp2_profiles(
          &info->profile_settings_3gpp, qmi_rsp))
    {
      info->profile_found_3gpp2 = TRUE;
      info->profile_index_3gpp2 =
        info->profile_list.profile_index[info->profiles_read];

      action = qbi_svc_bc_provisioned_contexts_s_build_wds28_req(
        qmi_txn->parent, WDS_PROFILE_TYPE_3GPP_V01);
    }
    else
    {
      info->profiles_read++;
      action = qbi_svc_bc_provisioned_contexts_s_get_next_3gpp2_profile(
        qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_s_3gpp2_wds2b_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_build_3gpp_wds2b_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WDS_GET_PROFILE_SETTINGS_REQ
    request to retrieve settings for a 3gpp profile

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_build_3gpp_wds2b_req
(
  qbi_txn_s *txn
)
{
  const qbi_svc_bc_provisioned_contexts_s_req_s *req;
  wds_get_profile_settings_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (const qbi_svc_bc_provisioned_contexts_s_req_s *) txn->req.data;

  qmi_req = (wds_get_profile_settings_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
      qbi_svc_bc_provisioned_contexts_s_3gpp_wds2b_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
  qmi_req->profile.profile_index = (uint8) req->context_id;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_provisioned_contexts_s_build_3gpp_wds2b_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_build_wds28_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WDS_MODIFY_PROFILE_SETTINGS request

    @details

    @param txn
    @param profile_type

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_build_wds28_req
(
  qbi_txn_s                *txn,
  wds_profile_type_enum_v01 profile_type
)
{
  wds_modify_profile_settings_req_msg_v01 *qmi_req;
  const qbi_svc_bc_provisioned_contexts_s_req_s *req;
  qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s profile_settings;
  qbi_svc_bc_provisioned_contexts_profiles_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  req = (const qbi_svc_bc_provisioned_contexts_s_req_s *) txn->req.data;
  qmi_req = (wds_modify_profile_settings_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ_V01,
      qbi_svc_bc_provisioned_contexts_s_wds28_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile.profile_index = (uint8_t) req->context_id;
  qmi_req->profile.profile_type = profile_type;

  if (!qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds28(
        qmi_req, &profile_settings, profile_type))
  {
    QBI_LOG_E_0("Couldn't collect profile setting pointers!");
  }
  else
  {
    action = qbi_svc_bc_provisioned_contexts_s_populate_profile(
      txn, qmi_req->profile.profile_type, &profile_settings);
  }

  info = (qbi_svc_bc_provisioned_contexts_profiles_info_s *) txn->info;
  if (action == QBI_SVC_ACTION_SEND_QMI_REQ &&
      profile_type == WDS_PROFILE_TYPE_3GPP_V01 &&
      info->profile_found_3gpp2)
  {
    qmi_req = (wds_modify_profile_settings_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_provisioned_contexts_s_wds28_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->profile.profile_index = (uint8_t) info->profile_index_3gpp2;
    qmi_req->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;

    if (!qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds28(
          qmi_req, &profile_settings, WDS_PROFILE_TYPE_3GPP2_V01))
    {
      QBI_LOG_E_0("Couldn't collect profile setting pointers!");
      action = QBI_SVC_ACTION_ABORT;
    }
    else
    {
      action = qbi_svc_bc_provisioned_contexts_s_populate_profile(
        txn, WDS_PROFILE_TYPE_3GPP2_V01, &profile_settings);
    }
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_s_build_wds28_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_build_wds2a_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WDS_GET_PROFILE_LIST_REQ request
    for a MBIM_CID_PROVISIONED_CONTEXTS set request

    @details

    @param txn
    @param profile_type

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_build_wds2a_req
(
  qbi_txn_s                *txn,
  wds_profile_type_enum_v01 profile_type
)
{
  wds_get_profile_list_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (wds_get_profile_list_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_LIST_REQ_V01,
      qbi_svc_bc_provisioned_contexts_s_wds2a_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile_type_valid = TRUE;
  qmi_req->profile_type = profile_type;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_provisioned_contexts_s_build_wds2a_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_compare_3gpp_3gpp2_profiles
===========================================================================*/
/*!
    @brief Check if partially cached 3gpp profile and 3gpp2 profile have the
    same connectivity parameters

    @details

    @param profile_settings_3gpp
    @param qmi_rsp_3gpp2

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_provisioned_contexts_s_compare_3gpp_3gpp2_profiles
(
  qbi_svc_bc_provisioned_contexts_3gpp_profile_settings_s *profile_settings_3gpp,
  wds_get_profile_settings_resp_msg_v01                   *qmi_rsp_3gpp2
)
{
  boolean status = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings_3gpp);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp_3gpp2);

  if (!profile_settings_3gpp->apn_name_valid    ||
    !qmi_rsp_3gpp2->apn_string_valid ||
    QBI_STRNICMP(profile_settings_3gpp->apn_name, qmi_rsp_3gpp2->apn_string,
    QMI_WDS_APN_NAME_MAX_V01))
  {
    QBI_LOG_D_0("3GPP/3GPP2 APN names do not match!");
  }
  else if (!profile_settings_3gpp->username_valid ||
           !qmi_rsp_3gpp2->user_id_valid ||
           QBI_STRNCMP(profile_settings_3gpp->username, qmi_rsp_3gpp2->user_id,
                       QMI_WDS_USER_NAME_MAX_V01))
  {
    QBI_LOG_D_0("3GPP/3GPP2 usernames do not match!");
  }
  else
  {
    status = TRUE;
  }

  return status;
} /* qbi_svc_bc_provisioned_contexts_s_compare_3gpp_3gpp2_profiles() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_get_next_3gpp2_profile
===========================================================================*/
/*!
    @brief Retrive next available 3gpp2 configured profile.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_get_next_3gpp2_profile
(
  qbi_txn_s *txn
)
{
  wds_get_profile_settings_req_msg_v01 *qmi_req_wds2b;
  qbi_svc_bc_provisioned_contexts_profiles_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_provisioned_contexts_profiles_info_s *) txn->info;
  if (info->profiles_read >= info->profile_list.num_of_profile)
  {
    action = qbi_svc_bc_provisioned_contexts_s_build_wds28_req(
      txn, WDS_PROFILE_TYPE_3GPP_V01);
  }
  else
  {
    /* Issue a query to retrieve the profile details */
    qmi_req_wds2b = (wds_get_profile_settings_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_provisioned_contexts_s_3gpp2_wds2b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_wds2b);

    qmi_req_wds2b->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
    qmi_req_wds2b->profile.profile_index =
      info->profile_list.profile_index[info->profiles_read];

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_s_get_next_3gpp2_profile */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds27
===========================================================================*/
/*!
    @brief Collect pointers to all relevant TLVs in QMI_WDS_CREATE_PROFILE_REQ
    for a MBIM_CID_PROVISIONED_CONTEXTS set request

    @details

    @param qmi_req
    @param profile_settings
    @param profile_type

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds27
(
  wds_create_profile_req_msg_v01                          *qmi_req,
  qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s *profile_settings,
  wds_profile_type_enum_v01                                profile_type
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_req);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  if (profile_type == WDS_PROFILE_TYPE_EPC_V01)
  {
    profile_settings->apn_name_valid                  = &qmi_req->apn_name_valid;
    profile_settings->apn_name                        = qmi_req->apn_name;
    profile_settings->user_id_valid                   = &qmi_req->common_user_id_valid;
    profile_settings->user_id                         = qmi_req->common_user_id;
    profile_settings->auth_password_valid             = &qmi_req->common_auth_password_valid;
    profile_settings->auth_password                   = qmi_req->common_auth_password;

    profile_settings->auth_protocol_valid             = &qmi_req->common_auth_protocol_valid;
    profile_settings->auth_protocol                   = &qmi_req->common_auth_protocol;

    profile_settings->app_user_data_valid             = &qmi_req->common_app_user_data_valid;
    profile_settings->app_user_data                   = &qmi_req->common_app_user_data;

    profile_settings->pdp_data_compression_type_valid = &qmi_req->pdp_data_compression_type_valid;
    profile_settings->pdp_data_compression_type       = &qmi_req->pdp_data_compression_type;
    profile_settings->pdp_hdr_compression_type_valid  = &qmi_req->pdp_hdr_compression_type_valid;
    profile_settings->pdp_hdr_compression_type        = &qmi_req->pdp_hdr_compression_type;
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    profile_settings->apn_name_valid                  = &qmi_req->apn_name_valid;
    profile_settings->apn_name                        = qmi_req->apn_name;
    profile_settings->user_id_valid                   = &qmi_req->username_valid;
    profile_settings->user_id                         = qmi_req->username;
    profile_settings->auth_password_valid             = &qmi_req->password_valid;
    profile_settings->auth_password                   = qmi_req->password;

    profile_settings->authentication_preference_valid = &qmi_req->authentication_preference_valid;
    profile_settings->authentication_preference       = &qmi_req->authentication_preference;

    profile_settings->app_user_data_valid             = &qmi_req->app_user_data_3gpp_valid;
    profile_settings->app_user_data                   = &qmi_req->app_user_data_3gpp;

    profile_settings->pdp_data_compression_type_valid = &qmi_req->pdp_data_compression_type_valid;
    profile_settings->pdp_data_compression_type       = &qmi_req->pdp_data_compression_type;
    profile_settings->pdp_hdr_compression_type_valid  = &qmi_req->pdp_hdr_compression_type_valid;
    profile_settings->pdp_hdr_compression_type        = &qmi_req->pdp_hdr_compression_type;
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    profile_settings->apn_name_valid           = &qmi_req->apn_string_valid;
    profile_settings->apn_name                 = qmi_req->apn_string;
    profile_settings->user_id_valid            = &qmi_req->user_id_valid;
    profile_settings->user_id                  = qmi_req->user_id;
    profile_settings->auth_password_valid      = &qmi_req->auth_password_valid;
    profile_settings->auth_password            = qmi_req->auth_password;

    profile_settings->auth_protocol_valid      = &qmi_req->auth_protocol_valid;
    profile_settings->auth_protocol            = &qmi_req->auth_protocol;

    profile_settings->app_user_data_valid      = &qmi_req->app_user_data_3gpp2_valid;
    profile_settings->app_user_data            = &qmi_req->app_user_data_3gpp2;
  }

  return TRUE;
} /* qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds27() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds28
===========================================================================*/
/*!
    @brief Collect pointers to all relevant TLVs in
    QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ for a MBIM_CID_PROVISIONED_CONTEXTS
    set request

    @details

    @param qmi_req
    @param profile_settings
    @param profile_type

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds28
(
   wds_modify_profile_settings_req_msg_v01                 *qmi_req,
   qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s *profile_settings,
   wds_profile_type_enum_v01                                profile_type
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_req);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  if (profile_type == WDS_PROFILE_TYPE_EPC_V01)
  {
    profile_settings->apn_name_valid      = &qmi_req->apn_name_valid;
    profile_settings->apn_name            = qmi_req->apn_name;
    profile_settings->user_id_valid       = &qmi_req->common_user_id_valid;
    profile_settings->user_id             = qmi_req->common_user_id;
    profile_settings->auth_password_valid = &qmi_req->common_auth_password_valid;
    profile_settings->auth_password       = qmi_req->common_auth_password;

    profile_settings->auth_protocol_valid = &qmi_req->common_auth_protocol_valid;
    profile_settings->auth_protocol       = &qmi_req->common_auth_protocol;

    profile_settings->app_user_data_valid = &qmi_req->common_app_user_data_valid;
    profile_settings->app_user_data       = &qmi_req->common_app_user_data;

    profile_settings->pdp_data_compression_type_valid = &qmi_req->pdp_data_compression_type_valid;
    profile_settings->pdp_data_compression_type       = &qmi_req->pdp_data_compression_type;
    profile_settings->pdp_hdr_compression_type_valid  = &qmi_req->pdp_hdr_compression_type_valid;
    profile_settings->pdp_hdr_compression_type        = &qmi_req->pdp_hdr_compression_type;
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    profile_settings->apn_name_valid                  = &qmi_req->apn_name_valid;
    profile_settings->apn_name                        = qmi_req->apn_name;
    profile_settings->user_id_valid                   = &qmi_req->username_valid;
    profile_settings->user_id                         = qmi_req->username;
    profile_settings->auth_password_valid             = &qmi_req->password_valid;
    profile_settings->auth_password                   = qmi_req->password;

    profile_settings->authentication_preference_valid = &qmi_req->authentication_preference_valid;
    profile_settings->authentication_preference       = &qmi_req->authentication_preference;

    profile_settings->app_user_data_valid             = &qmi_req->app_user_data_3gpp_valid;
    profile_settings->app_user_data                   = &qmi_req->app_user_data_3gpp;

    profile_settings->pdp_data_compression_type_valid = &qmi_req->pdp_data_compression_type_valid;
    profile_settings->pdp_data_compression_type       = &qmi_req->pdp_data_compression_type;
    profile_settings->pdp_hdr_compression_type_valid  = &qmi_req->pdp_hdr_compression_type_valid;
    profile_settings->pdp_hdr_compression_type        = &qmi_req->pdp_hdr_compression_type;
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    profile_settings->apn_name_valid      = &qmi_req->apn_string_valid;
    profile_settings->apn_name            = qmi_req->apn_string;
    profile_settings->user_id_valid       = &qmi_req->user_id_valid;
    profile_settings->user_id             = qmi_req->user_id;
    profile_settings->auth_password_valid = &qmi_req->auth_password_valid;
    profile_settings->auth_password       = qmi_req->auth_password;

    profile_settings->auth_protocol_valid = &qmi_req->auth_protocol_valid;
    profile_settings->auth_protocol       = &qmi_req->auth_protocol;

    profile_settings->app_user_data_valid = &qmi_req->app_user_data_3gpp2_valid;
    profile_settings->app_user_data       = &qmi_req->app_user_data_3gpp2;
  }

  return TRUE;
} /* qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds28() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_populate_str
===========================================================================*/
/*!
    @brief Converts a UTF-16 string from the request DataBuffer into ASCII,
    then copies it into a QMI buffer

    @details
    Ensures that the UTF-16 string was not truncated when copying into the
    ASCII buffer.

    @param txn
    @param field_desc
    @param field_max_size
    @param qmi_field
    @param qmi_field_size

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_provisioned_contexts_s_populate_str
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *field_desc,
  uint32                             field_max_size,
  char                              *qmi_field,
  uint32                             qmi_field_size
)
{
  const uint8 *req_str_utf16;
  uint32 bytes_copied;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  req_str_utf16 = qbi_txn_req_databuf_get_field(
    txn, field_desc, 0, field_max_size);
  QBI_CHECK_NULL_PTR_RET_FALSE(req_str_utf16);

  bytes_copied = qbi_util_utf16_to_ascii(
    req_str_utf16, field_desc->size, qmi_field, qmi_field_size);
  if (bytes_copied > qmi_field_size)
  {
    QBI_LOG_E_2("Couldn't fit entire MBIM string into QMI request! Need %d "
                "bytes, have room for %d", bytes_copied, qmi_field_size);
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_provisioned_contexts_s_populate_str() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_populate_profile
===========================================================================*/
/*!
    @brief Populates QMI profile settings for a MBIM_CID_PROVISIONED_CONTEXTS
    set request

    @details

    @param txn
    @param profile_type
    @param profile_settings

    @return qbi_svc_action_e QBI_SVC_ACTION_SEND_QMI_REQ on success,
    QBI_SVC_ACTION_ABORT on failure
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_populate_profile
(
  qbi_txn_s                                               *txn,
  wds_profile_type_enum_v01                                profile_type,
  qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s *profile_settings
)
{
  const qbi_svc_bc_provisioned_contexts_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->apn_name_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->user_id_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->auth_password_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->app_user_data_valid);

  req = (const qbi_svc_bc_provisioned_contexts_s_req_s *) txn->req.data;
  if (!qbi_svc_bc_provisioned_contexts_s_populate_profile_compression(
        req, profile_type, profile_settings) ||
      !qbi_svc_bc_provisioned_contexts_s_populate_profile_auth_protocol(
        txn, req, profile_type, profile_settings))
  {
    QBI_LOG_E_0("Couldn't populate compression or auth protocol!");
  }
  else if (profile_type == WDS_PROFILE_TYPE_EPC_V01 ||
           profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    *profile_settings->apn_name_valid = TRUE;
    *profile_settings->user_id_valid = TRUE;
    *profile_settings->auth_password_valid = TRUE;
    if (req->access_string.offset != 0 &&
        !qbi_svc_bc_provisioned_contexts_s_populate_str(
          txn, &req->access_string, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES,
          profile_settings->apn_name, QMI_WDS_APN_NAME_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for APN name!");
    }
    else if (req->username.offset != 0 &&
             !qbi_svc_bc_provisioned_contexts_s_populate_str(
               txn, &req->username, QBI_SVC_BC_USERNAME_MAX_LEN_BYTES,
               profile_settings->user_id, QMI_WDS_USER_NAME_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for username!");
    }
    else if (req->password.offset != 0 &&
             !qbi_svc_bc_provisioned_contexts_s_populate_str(
               txn, &req->password, QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES,
               profile_settings->auth_password,
               QMI_WDS_PASSWORD_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for password!");
    }
    else
    {
      *profile_settings->app_user_data_valid =
         qbi_svc_bc_context_type_uuid_to_id(
           req->context_type,
           (uint32 *)profile_settings->app_user_data);

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    *profile_settings->apn_name_valid    = TRUE;
    *profile_settings->user_id_valid       = TRUE;
    *profile_settings->auth_password_valid = TRUE;
    if (req->access_string.offset != 0 &&
        !qbi_svc_bc_provisioned_contexts_s_populate_str(
          txn, &req->access_string, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES,
          profile_settings->apn_name, QMI_WDS_APN_NAME_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for 3GPP2 APN string!");
    }
    else if (req->username.offset != 0 &&
             !qbi_svc_bc_provisioned_contexts_s_populate_str(
               txn, &req->username, QBI_SVC_BC_USERNAME_MAX_LEN_BYTES,
               profile_settings->user_id, QMI_WDS_USER_NAME_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for 3GPP2 username!");
    }
    else if (req->password.offset != 0 &&
             !qbi_svc_bc_provisioned_contexts_s_populate_str(
               txn, &req->password, QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES,
               profile_settings->auth_password, QMI_WDS_PASSWORD_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for 3GPP2 password!");
    }
    else
    {
      *profile_settings->app_user_data_valid =
         qbi_svc_bc_context_type_uuid_to_id(
           req->context_type,
           (uint32 *)profile_settings->app_user_data);

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_s_populate_profile() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_populate_profile_auth_protocol
===========================================================================*/
/*!
    @brief Populates the authentication preference/protocol TLV of a
    QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ or QMI_WDS_CREATE_PROFILE_REQ using
    the information from the MBIM_CID_PROVISIONED_CONTEXTS set request

    @details

    @param txn
    @param req
    @param profile_type
    @param profile_settings

    @return boolean TRUE on success, FALSE on failure. May set txn->status
    if the request contained an invalid value
*/
/*=========================================================================*/
static boolean qbi_svc_bc_provisioned_contexts_s_populate_profile_auth_protocol
(
   qbi_txn_s                                               *txn,
   const qbi_svc_bc_provisioned_contexts_s_req_s           *req,
   wds_profile_type_enum_v01                                profile_type,
   qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s *profile_settings
)
{
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(req);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  if (profile_type == WDS_PROFILE_TYPE_EPC_V01)
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->auth_protocol_valid);
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->auth_protocol);
    *(profile_settings->auth_protocol_valid) = TRUE;
    *(profile_settings->auth_protocol) =
      qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_protocol(
        req->auth_protocol);
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->authentication_preference_valid);
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->authentication_preference);
    *(profile_settings->authentication_preference_valid) = TRUE;
    *(profile_settings->authentication_preference) =
      qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_pref(req->auth_protocol);
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->auth_protocol_valid);
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->auth_protocol);
    if (req->auth_protocol == QBI_SVC_BC_AUTH_PROTOCOL_NONE)
    {
      QBI_LOG_W_0("Ignoring 3GPP2 AUTH_PROTOCOL_NONE setting");
    }
    else
    {
      *(profile_settings->auth_protocol_valid) = TRUE;
      if (req->auth_protocol == QBI_SVC_BC_AUTH_PROTOCOL_CHAP ||
          req->auth_protocol == QBI_SVC_BC_AUTH_PROTOCOL_MSCHAP_V2)
      {
        *(profile_settings->auth_protocol) = WDS_PROFILE_AUTH_PROTOCOL_CHAP_V01;
      }
      else if (req->auth_protocol == QBI_SVC_BC_AUTH_PROTOCOL_PAP)
      {
        *(profile_settings->auth_protocol) = WDS_PROFILE_AUTH_PROTOCOL_PAP_V01;
      }
      else
      {
        QBI_LOG_E_1("Invalid authentication protocol %d", req->auth_protocol);
        txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
        success = FALSE;
      }
    }
  }
  else
  {
    QBI_LOG_E_1("Invalid profile type %d", profile_type);
    success = FALSE;
  }

  return success;
} /* qbi_svc_bc_provisioned_contexts_s_populate_profile_auth_protocol() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_populate_profile_compression
===========================================================================*/
/*!
    @brief Populates the compression TLVs of a
    QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ or QMI_WDS_CREATE_PROFILE_REQ for
    a MBIM_CID_PROVISIONED_CONTEXTS set request

    @details

    @param req
    @param profile_type
    @param profile_settings

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_provisioned_contexts_s_populate_profile_compression
(
  const qbi_svc_bc_provisioned_contexts_s_req_s           *req,
  wds_profile_type_enum_v01                                profile_type,
  qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s *profile_settings
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(req);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  if (profile_type == WDS_PROFILE_TYPE_EPC_V01 ||
      profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->pdp_data_compression_type_valid);
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->pdp_data_compression_type);
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->pdp_hdr_compression_type_valid);
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->pdp_hdr_compression_type);

    *(profile_settings->pdp_data_compression_type_valid) = TRUE;
    *(profile_settings->pdp_hdr_compression_type_valid)  = TRUE;
    if (req->compression == QBI_SVC_BC_COMPRESSION_ENABLE)
    {
      *(profile_settings->pdp_data_compression_type) =
        WDS_PDP_DATA_COMPR_TYPE_MANUFACTURER_PREF_V01;
      *(profile_settings->pdp_hdr_compression_type) =
        WDS_PDP_HDR_COMPR_TYPE_MANUFACTURER_PREF_V01;
    }
    else
    {
      *(profile_settings->pdp_data_compression_type) =
        WDS_PDP_DATA_COMPR_TYPE_OFF_V01;
      *(profile_settings->pdp_hdr_compression_type)  =
        WDS_PDP_HDR_COMPR_TYPE_OFF_V01;
    }
  }

  return TRUE;
} /* qbi_svc_bc_provisioned_contexts_s_populate_profile_compression() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PROVISIONED_CONTEXTS set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_provisioned_contexts_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint32 context_type_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_provisioned_contexts_s_req_s *) txn->req.data;
  if (!qbi_svc_bc_context_type_uuid_to_id(req->context_type, &context_type_id))
  {
    QBI_LOG_E_0("Received unsupported ContextType!");
    txn->status = QBI_MBIM_STATUS_CONTEXT_NOT_SUPPORTED;
  }
  else if (req->context_id == QBI_SVC_BC_CONTEXT_ID_APPEND)
  {
    /* Issue a create profile request */
    action = qbi_svc_bc_provisioned_contexts_build_wds27_req(txn);
  }
  else if (!qbi_svc_bc_provisioned_contexts_id_is_valid(req->context_id))
  {
    QBI_LOG_E_1("Invalid ContextId %d", req->context_id);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else
  {
    txn->info = QBI_MEM_MALLOC_CLEAR(
      sizeof(qbi_svc_bc_provisioned_contexts_profiles_info_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

    action = qbi_svc_bc_provisioned_contexts_s_build_wds2a_req(
      txn, WDS_PROFILE_TYPE_EPC_V01);
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_save_3gpp_profile_settings
===========================================================================*/
/*!
    @brief Save relevant 3gpp profiles settings from a
    QMI_WDS_GET_PROFILE_SETTINGS_RESP, to be used for finding matching 3gpp
    profile

    @details

    @param profile_settings_3gpp
    @param qmi_rsp
*/
/*=========================================================================*/
static void qbi_svc_bc_provisioned_contexts_s_save_3gpp_profile_settings
(
  qbi_svc_bc_provisioned_contexts_3gpp_profile_settings_s *profile_settings_3gpp,
  wds_get_profile_settings_resp_msg_v01                   *qmi_rsp
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(profile_settings_3gpp);
  QBI_CHECK_NULL_PTR_RET(qmi_rsp);

  profile_settings_3gpp->apn_name_valid = qmi_rsp->apn_name_valid;
  QBI_STRLCPY(profile_settings_3gpp->apn_name, qmi_rsp->apn_name,
              sizeof(profile_settings_3gpp->apn_name));
  profile_settings_3gpp->username_valid = qmi_rsp->username_valid;
  QBI_STRLCPY(profile_settings_3gpp->username, qmi_rsp->username,
              sizeof(profile_settings_3gpp->username));
} /* qbi_svc_bc_provisioned_contexts_s_save_3gpp_profile_settings() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_wds27_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_CREATE_PROFILE_RESP for
    MBIM_CID_PROVISIONED_CONTEXTS set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_wds27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_create_profile_req_msg_v01 *qmi_req;
  wds_create_profile_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_provisioned_contexts_profile_settings_ptrs_s profile_settings;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wds_create_profile_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wds_create_profile_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if (!(qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
          qmi_rsp->resp.error == QMI_ERR_EXTENDED_INTERNAL_V01 &&
          qmi_rsp->extended_error_code_valid && qmi_rsp->extended_error_code ==
            WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE_V01))
    {
      QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
      if (qmi_rsp->extended_error_code_valid)
      {
        QBI_LOG_E_1("Extended error code %d", qmi_rsp->extended_error_code);
      }
      qbi_svc_bc_provisioned_contexts_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
    }
    else
    {
      qmi_req = (wds_create_profile_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_CREATE_PROFILE_REQ_V01,
          qbi_svc_bc_provisioned_contexts_s_wds27_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      if (!qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds27(
            qmi_req, &profile_settings, WDS_PROFILE_TYPE_3GPP_V01))
      {
        QBI_LOG_E_0("Couldn't collect profile setting pointers!");
      }
      else
      {
        action = qbi_svc_bc_provisioned_contexts_s_populate_profile(
          qmi_txn->parent, qmi_req->profile_type, &profile_settings);
      }

      /* A 3gpp2 profile should also be added to support data continuity */
      if (action == QBI_SVC_ACTION_SEND_QMI_REQ &&
          qbi_svc_bc_device_supports_3gpp2(qmi_txn->ctx))
      {
        qmi_req = (wds_create_profile_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
            qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_CREATE_PROFILE_REQ_V01,
            qbi_svc_bc_provisioned_contexts_s_wds27_rsp_cb);
        QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

        qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP2_V01;

        if (!qbi_svc_bc_provisioned_contexts_s_get_profile_settings_ptrs_wds27(
              qmi_req, &profile_settings, WDS_PROFILE_TYPE_3GPP2_V01))
        {
          QBI_LOG_E_0("Couldn't collect profile setting pointers!");
        }
        else
        {
          action = qbi_svc_bc_provisioned_contexts_s_populate_profile(
            qmi_txn->parent, qmi_req->profile_type, &profile_settings);
        }
      }
    }
  }
  else
  {
    if (qmi_txn->parent->qmi_txns_pending > 0)
    {
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
    else
    {
      action = qbi_svc_bc_provisioned_contexts_q_req(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_s_wds27_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_wds28_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_MODIFY_PROFILE_SETTINGS_RESP for
    MBIM_CID_PROVISIONED_CONTEXTS set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_wds28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_modify_profile_settings_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_modify_profile_settings_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    if (qmi_rsp->extended_error_code_valid)
    {
      QBI_LOG_E_1("Extended error code %d", qmi_rsp->extended_error_code);
    }
    qbi_svc_bc_provisioned_contexts_set_mbim_error_status(
      qmi_txn->parent, qmi_rsp->resp.error,
      qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }
  else
  {
    if (qmi_txn->parent->qmi_txns_pending > 0)
    {
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
    else
    {
      if (qmi_txn->parent->info != NULL)
      {
        QBI_MEM_FREE(qmi_txn->parent->info);
        qmi_txn->parent->info = NULL;
      }

      QBI_LOG_D_0("Completed profile modify operation; performing query");
      action = qbi_svc_bc_provisioned_contexts_q_req(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_s_wds28_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_s_wds2a_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_LIST_RESP for a
    MBIM_CID_PROVISIONED_CONTEXTS set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_provisioned_contexts_s_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  const qbi_svc_bc_provisioned_contexts_s_req_s *req;
  wds_get_profile_list_req_msg_v01 *qmi_req;
  wds_get_profile_list_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_provisioned_contexts_profiles_info_s *info;
  uint32 i;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wds_get_profile_list_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_list_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      !(qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
        qmi_rsp->resp.error == QMI_ERR_EXTENDED_INTERNAL_V01 &&
        qmi_rsp->extended_error_code_valid && qmi_rsp->extended_error_code ==
          WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE_V01))
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_provisioned_contexts_set_mbim_error_status(
      qmi_txn->parent, qmi_rsp->resp.error,
      qmi_rsp->extended_error_code_valid,
      qmi_rsp->extended_error_code);
  }
  else if (qmi_rsp->profile_list_len > QMI_WDS_PROFILE_LIST_MAX_V01)
  {
    QBI_LOG_E_1("Invalid number of profiles %d", qmi_rsp->profile_list_len);
  }
  else
  {
    /* Append new profile indexes to the profile list */
    info = (qbi_svc_bc_provisioned_contexts_profiles_info_s *)
      qmi_txn->parent->info;
    qbi_svc_bc_populate_profile_list(&info->profile_list, qmi_rsp);

    if (qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01)
    {
      action = qbi_svc_bc_provisioned_contexts_s_build_wds2a_req(
        qmi_txn->parent, WDS_PROFILE_TYPE_3GPP_V01);
    }
    else if (qmi_req->profile_type == WDS_PROFILE_TYPE_3GPP_V01)
    {
      action = qbi_svc_bc_provisioned_contexts_s_build_wds2a_req(
        qmi_txn->parent, WDS_PROFILE_TYPE_3GPP2_V01);
    }
    else
    {
      for (i = 0; i < info->profile_list.num_of_profile; i++)
      {
        req = (const qbi_svc_bc_provisioned_contexts_s_req_s *)
          qmi_txn->parent->req.data;
        if (info->profile_list.profile_index[i] == req->context_id)
        {
          if (info->profile_list.profile_type[i] == WDS_PROFILE_TYPE_EPC_V01)
          {
            action = qbi_svc_bc_provisioned_contexts_s_build_wds28_req(
              qmi_txn->parent, WDS_PROFILE_TYPE_EPC_V01);
          }
          else if (info->profile_list.profile_type[i] ==
                   WDS_PROFILE_TYPE_3GPP_V01)
          {
            /* When modify a 3gpp profile, there may be a coresponding 3gpp2
               profile to be modified as well. Read 3gpp profile settings
               before modification, to be used for finding matching 3gpp2
               profile */
            info->profiles_read =
              info->profile_list.num_of_profile - qmi_rsp->profile_list_len;
            action = qbi_svc_bc_provisioned_contexts_s_build_3gpp_wds2b_req(
              qmi_txn->parent);
          }

          break;
        }
      }
    }
  }

  return action;
} /* qbi_svc_bc_provisioned_contexts_s_wds2a_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provisioned_contexts_set_mbim_error_status
===========================================================================*/
/*!
    @brief Attempts to map QMI error information into a descriptive MBIM
    error status for MBIM_CID_PROVISIONED_CONTEXTS

    @details

    @param txn
    @param qmi_error
    @param qmi_error_ds_ext_valid
    @param qmi_error_ds_ext
*/
/*=========================================================================*/
static void qbi_svc_bc_provisioned_contexts_set_mbim_error_status
(
  qbi_txn_s                          *txn,
  qmi_error_type_v01                  qmi_error,
  uint8_t                             qmi_error_ds_ext_valid,
  wds_ds_extended_error_code_enum_v01 qmi_error_ds_ext
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  /* Map extended error first, then map QMI error  */
  if (qmi_error == QMI_ERR_EXTENDED_INTERNAL_V01 && qmi_error_ds_ext_valid)
  {
    QBI_LOG_E_1("DS Profile extended error code 0x%x", qmi_error_ds_ext);
    switch (qmi_error_ds_ext)
    {
      case WDS_EEC_DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED_V01:
        txn->status = QBI_MBIM_STATUS_NOT_INITIALIZED;
        break;

      case WDS_EEC_DS_PROFILE_REG_RESULT_ERR_LEN_INVALID_V01:
        txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
        break;

      case WDS_EEC_DS_PROFILE_REG_3GPP_ACCESS_ERR_V01:
      case WDS_EEC_DS_PROFILE_3GPP_ACCESS_ERR_V01:
        txn->status = QBI_MBIM_STATUS_READ_FAILURE;
        break;

      case WDS_EEC_DS_PROFILE_REG_3GPP_ERR_OUT_OF_PROFILES_V01:
      case WDS_EEC_DS_PROFILE_REG_3GPP_READ_ONLY_FLAG_SET_V01:
      case WDS_EEC_DS_PROFILE_3GPP_ERR_OUT_OF_PROFILES_V01:
      case WDS_EEC_DS_PROFILE_3GPP_READ_ONLY_FLAG_SET_V01:
        txn->status = QBI_MBIM_STATUS_WRITE_FAILURE;
        break;

      default:
        txn->status = QBI_MBIM_STATUS_FAILURE;
    }
  }
  else
  {
    switch (qmi_error)
    {
      case QMI_ERR_INVALID_PROFILE_V01:
        txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
        break;

      case QMI_ERR_NO_FREE_PROFILE_V01:
        txn->status = QBI_MBIM_STATUS_WRITE_FAILURE;
        break;

      default:
        txn->status = QBI_MBIM_STATUS_FAILURE;
    }
  }
} /* qbi_svc_bc_provisioned_contexts_set_mbim_error_status() */

/*! @} */

/*! @addtogroup MBIM_CID_IP_CONFIGURATION_INFO
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ip_configuration_info_e_wds22_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_PKT_SRVC_STATUS_IND, looking for an indication
    that reconfiguration is required, meaning that the IP address info must
    be re-queried and sent out as an event

    @details
    Note that this callback is invoked twice for each QMI indication, as it
    is a broadcast indication and we have two WDS client IDs.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ip_configuration_info_e_wds22_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_bc_ip_configuration_info_rsp_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const wds_pkt_srvc_status_ind_msg_v01 *qmi_ind;
  uint32 session_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const wds_pkt_srvc_status_ind_msg_v01 *) ind->buf->data;

  /*! @note We are relying on the ordering of the static indication handler
      table here: the MBIM_CID_CONNECT event handler will always be processed
      before this one, so we can assume that if the cached overall connection
      state is connected and this indication contains a disconnected
      notification, then connectivity on a single IP type was lost. */
  session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(ind->qmi_svc_id);
  if (qbi_svc_bc_connect_session_activation_state(ind->txn->ctx, session_id) ==
        QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED &&
      (qmi_ind->status.reconfiguration_required ||
       qmi_ind->status.connection_status ==
         WDS_CONNECTION_STATUS_DISCONNECTED_V01))
  {
    QBI_LOG_W_0("Reconfiguration required or single IP connectivity lost in "
                "dual IP call");

    /* Allocate request to polulate session ID */
    req = qbi_util_buf_alloc(
      &ind->txn->req, sizeof(qbi_svc_bc_ip_configuration_info_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(req);

    req->session_id = session_id;
    action = qbi_svc_bc_ip_configuration_info_q_req(ind->txn);
  }

  return action;
} /* qbi_svc_bc_ip_configuration_info_e_wds22_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ip_configuration_info_eq_build_rsp_ipv4
===========================================================================*/
/*!
    @brief Allocates and populates the IPv4 address information in the
    MBIM_CID_IP_CONFIGURATION_INFO response

    @details

    @param txn
    @param rsp

    @return boolean TRUE if successfully populated values, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ip_configuration_info_eq_build_rsp_ipv4
(
  qbi_txn_s                              *txn,
  qbi_svc_bc_ip_configuration_info_rsp_s *rsp
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_bc_ip_configuration_info_rsp_s *req;
  wds_get_runtime_settings_resp_msg_v01 *qmi_rsp_ipv4;
  qbi_svc_bc_ip_configuration_info_ipv4_element_s *element_ipv4;
  void *data_ptr;
  qbi_mbim_offset_size_pair_s field;
  uint32 ipv4_addr;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(rsp);

  req = (qbi_svc_bc_ip_configuration_info_rsp_s *)txn->req.data;
  qmi_txn = qbi_qmi_txn_get(
    txn, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(req->session_id),
    QMI_WDS_GET_RUNTIME_SETTINGS_REQ_V01);
  if (qmi_txn == NULL)
  {
    QBI_LOG_E_0("Couldn't get QMI transaction for IPv4 runtime settings!");
  }
  else
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->rsp.data);
    qmi_rsp_ipv4 = (wds_get_runtime_settings_resp_msg_v01 *) qmi_txn->rsp.data;

    if (qmi_rsp_ipv4->ipv4_address_preference_valid &&
        qmi_rsp_ipv4->ipv4_subnet_mask_valid &&
        qmi_rsp_ipv4->ipv4_gateway_addr_valid &&
        qmi_rsp_ipv4->mtu_valid)
    {
      /* IP address */
      element_ipv4 = (qbi_svc_bc_ip_configuration_info_ipv4_element_s *)
        qbi_txn_rsp_databuf_add_field(
          txn, &field, 0,
          sizeof(qbi_svc_bc_ip_configuration_info_ipv4_element_s), NULL);
      QBI_CHECK_NULL_PTR_RET_FALSE(element_ipv4);

      element_ipv4->prefix_len =
        qbi_svc_bc_ip_configuration_info_ipv4_subnet_mask_to_prefix_len(
          qmi_rsp_ipv4->ipv4_subnet_mask);
      element_ipv4->ipv4_address =
        QBI_MEM_HTONL(qmi_rsp_ipv4->ipv4_address_preference);

      rsp->ipv4_config_avail = QBI_SVC_BC_AVAILABLE_IP_CONFIG_ADDRESS;
      rsp->ipv4_address_count = 1;
      rsp->ipv4_address_offset = field.offset;

      /* Gateway address */
      ipv4_addr = QBI_MEM_HTONL(qmi_rsp_ipv4->ipv4_gateway_addr);
      data_ptr = qbi_txn_rsp_databuf_add_field(
        txn, &field, 0, sizeof(uint32), &ipv4_addr);
      QBI_CHECK_NULL_PTR_RET_FALSE(data_ptr);

      rsp->ipv4_config_avail |= QBI_SVC_BC_AVAILABLE_IP_CONFIG_GATEWAY;
      rsp->ipv4_gateway_offset = field.offset;

      /* MTU */
      rsp->ipv4_config_avail |= QBI_SVC_BC_AVAILABLE_IP_CONFIG_MTU;
      rsp->ipv4_mtu =
        MIN(qmi_rsp_ipv4->mtu, qbi_hc_get_max_segment_size(txn->ctx));
    }

    if (qmi_rsp_ipv4->primary_DNS_IPv4_address_preference_valid)
    {
      rsp->ipv4_dns_server_count = 1;
      ipv4_addr =
        QBI_MEM_HTONL(qmi_rsp_ipv4->primary_DNS_IPv4_address_preference);
      data_ptr = qbi_txn_rsp_databuf_add_field(
        txn, &field, 0, sizeof(uint32), &ipv4_addr);
      QBI_CHECK_NULL_PTR_RET_FALSE(data_ptr);

      rsp->ipv4_config_avail |= QBI_SVC_BC_AVAILABLE_IP_CONFIG_DNS;
      rsp->ipv4_dns_server_offset = field.offset;

      if (qmi_rsp_ipv4->secondary_DNS_IPv4_address_preference_valid)
      {
        rsp->ipv4_dns_server_count = 2;
        ipv4_addr =
          QBI_MEM_HTONL(qmi_rsp_ipv4->secondary_DNS_IPv4_address_preference);
        data_ptr = qbi_txn_rsp_databuf_add_field(
          txn, &field, 0, sizeof(uint32), &ipv4_addr);
        QBI_CHECK_NULL_PTR_RET_FALSE(data_ptr);
      }
    }

    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_ip_configuration_info_eq_build_rsp_ipv4() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ip_configuration_info_eq_build_rsp_ipv6
===========================================================================*/
/*!
    @brief Allocates and populates the IPv6 address information in the
    MBIM_CID_IP_CONFIGURATION_INFO response

    @details

    @param txn
    @param rsp

    @return boolean TRUE if successfully populated values, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ip_configuration_info_eq_build_rsp_ipv6
(
  qbi_txn_s                              *txn,
  qbi_svc_bc_ip_configuration_info_rsp_s *rsp
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_bc_ip_configuration_info_rsp_s *req;
  wds_get_runtime_settings_resp_msg_v01 *qmi_rsp_ipv6;
  qbi_svc_bc_ip_configuration_info_ipv6_element_s *element_ipv6;
  void *data_ptr;
  qbi_mbim_offset_size_pair_s field;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(rsp);

  req = (qbi_svc_bc_ip_configuration_info_rsp_s *)txn->req.data;
  qmi_txn = qbi_qmi_txn_get(
    txn, QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(req->session_id),
    QMI_WDS_GET_RUNTIME_SETTINGS_REQ_V01);
  if (qmi_txn == NULL)
  {
    QBI_LOG_I_0("No IPv6 configuration information request sent");
  }
  else
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->rsp.data);
    qmi_rsp_ipv6 = (wds_get_runtime_settings_resp_msg_v01 *) qmi_txn->rsp.data;

      if (qmi_rsp_ipv6->ipv6_addr_valid &&
          qmi_rsp_ipv6->ipv6_gateway_addr_valid &&
          qmi_rsp_ipv6->mtu_valid)
      {
        /* IP address */
        element_ipv6 = (qbi_svc_bc_ip_configuration_info_ipv6_element_s *)
          qbi_txn_rsp_databuf_add_field(txn, &field, 0,
            sizeof(qbi_svc_bc_ip_configuration_info_ipv6_element_s), NULL);
        QBI_CHECK_NULL_PTR_RET_FALSE(element_ipv6);

        element_ipv6->prefix_len = qmi_rsp_ipv6->ipv6_addr.ipv6_prefix_length;
        QBI_MEMSCPY(element_ipv6->ipv6_address,
                    sizeof(element_ipv6->ipv6_address),
                    qmi_rsp_ipv6->ipv6_addr.ipv6_addr,
                    QBI_SVC_BC_IPV6_ADDR_LEN);

        rsp->ipv6_address_offset = field.offset;
        rsp->ipv6_config_avail = QBI_SVC_BC_AVAILABLE_IP_CONFIG_ADDRESS;
        rsp->ipv6_address_count = 1;

        /* Gateway address */
        data_ptr = qbi_txn_rsp_databuf_add_field(
          txn, &field, 0, QBI_SVC_BC_IPV6_ADDR_LEN,
          qmi_rsp_ipv6->ipv6_gateway_addr.ipv6_addr);
        QBI_CHECK_NULL_PTR_RET_FALSE(data_ptr);

        rsp->ipv6_config_avail |= QBI_SVC_BC_AVAILABLE_IP_CONFIG_GATEWAY;
        rsp->ipv6_gateway_offset = field.offset;

        /* MTU */
        rsp->ipv6_config_avail |= QBI_SVC_BC_AVAILABLE_IP_CONFIG_MTU;
        rsp->ipv6_mtu =
          MIN(qmi_rsp_ipv6->mtu, qbi_hc_get_max_segment_size(txn->ctx));
      }

      /* DNS address(es) (optional) */
    if (qmi_rsp_ipv6->primary_dns_IPv6_address_valid)
    {
      rsp->ipv6_dns_server_count = 1;
      data_ptr = qbi_txn_rsp_databuf_add_field(
        txn, &field, 0, QBI_SVC_BC_IPV6_ADDR_LEN,
        qmi_rsp_ipv6->primary_dns_IPv6_address);
      QBI_CHECK_NULL_PTR_RET_FALSE(data_ptr);

      rsp->ipv6_config_avail |= QBI_SVC_BC_AVAILABLE_IP_CONFIG_DNS;
      rsp->ipv6_dns_server_offset = field.offset;

      if (qmi_rsp_ipv6->secondary_dns_IPv6_address_valid)
      {
        rsp->ipv6_dns_server_count = 2;
        data_ptr = qbi_txn_rsp_databuf_add_field(
          txn, &field, 0, QBI_SVC_BC_IPV6_ADDR_LEN,
          qmi_rsp_ipv6->secondary_dns_IPv6_address);
        QBI_CHECK_NULL_PTR_RET_FALSE(data_ptr);
      }
    }

    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_ip_configuration_info_eq_build_rsp_ipv6() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ip_configuration_info_eq_build_rsp
===========================================================================*/
/*!
    @brief Allocates and populates the response for a query to
    MBIM_CID_IP_CONFIGURATION_INFO

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ip_configuration_info_eq_build_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ip_configuration_info_rsp_s *req;
  qbi_svc_bc_ip_configuration_info_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_ip_configuration_info_rsp_s *) txn->req.data;
  rsp = (qbi_svc_bc_ip_configuration_info_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_bc_ip_configuration_info_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  if (qbi_svc_bc_connect_is_session_ipv4_activated(txn->ctx, req->session_id) &&
      !qbi_svc_bc_ip_configuration_info_eq_build_rsp_ipv4(txn, rsp))
  {
    QBI_LOG_E_0("Couldn't populate IPv4 information!");
  }
  else if (qbi_svc_bc_connect_is_session_ipv6_activated(
             txn->ctx, req->session_id) &&
           !qbi_svc_bc_ip_configuration_info_eq_build_rsp_ipv6(txn, rsp))
  {
    QBI_LOG_E_0("Couldn't populate IPv6 information!");
  }
  else
  {
    if (txn->cmd_type == QBI_MSG_CMD_TYPE_QUERY)
    {
      qbi_os_log_event(QBI_OS_LOG_EVENT_IP_CFG_RESP);
    }
    rsp->session_id = req->session_id;
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_bc_ip_configuration_info_eq_build_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ip_configuration_info_ipv4_subnet_mask_to_prefix_len
===========================================================================*/
/*!
    @brief Calculates the prefix length (in bits) of a subnet mask provided
    in host byte order

    @details
    For example, converts 255.255.255.252 (0xFFFFFFFC) into 30.

    @param subnet_mask Subnet mask in host byte order

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_ip_configuration_info_ipv4_subnet_mask_to_prefix_len
(
  uint32 subnet_mask
)
{
  int32 i;
  uint32 prefix_len = 0;
/*-------------------------------------------------------------------------*/
  for (i = 31; i >= 0; i--)
  {
    if (subnet_mask & (1 << i))
    {
      prefix_len++;
    }
    else
    {
      break;
    }
  }

  return prefix_len;
} /* qbi_svc_bc_ip_configuration_info_ipv4_subnet_mask_to_prefix_len() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ip_configuration_info_q_wds2d_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_RUNTIME_SETTINGS_RESP for
    MBIM_CID_IP_CONFIGURATION_INFO query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ip_configuration_info_q_wds2d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_runtime_settings_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_runtime_settings_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->ip_family_valid)
  {
    QBI_LOG_E_0("IP family TLV not present in response!");
  }
  else if (qmi_rsp->ip_family == WDS_IP_FAMILY_IPV4_V01 ||
           qmi_rsp->ip_family == WDS_IP_FAMILY_IPV6_V01)
  {
    if (qmi_txn->parent->qmi_txns_pending > 0)
    {
      /* Keep the QMI response and wait for the other response */
      QBI_LOG_I_0("Waiting on second response for IP config info");
      qmi_txn->keep_after_rsp_cb = TRUE;
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
    else
    {
      /* Build ip config response as no QMI response is pending */
      action = qbi_svc_bc_ip_configuration_info_eq_build_rsp(
        qmi_txn->parent);
    }
  }
  else
  {
    QBI_LOG_E_1("Unexpected IP family %d in response!", qmi_rsp->ip_family);
  }

  return action;
} /* qbi_svc_bc_ip_configuration_info_q_wds2d_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ip_configuration_info_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_IP_CONFIGURATION_INFO query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ip_configuration_info_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ip_configuration_info_q_req_s *req;
  wds_get_runtime_settings_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_qmi_svc_e wds_svc_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  qbi_os_log_event(QBI_OS_LOG_EVENT_IP_CFG_REQ);
  req = (qbi_svc_bc_ip_configuration_info_q_req_s *) txn->req.data;
  if (req->session_id >= qbi_svc_bc_max_sessions_get(txn->ctx))
  {
    QBI_LOG_E_2("Requested SessionId (%d) is past maximum (%d)",
                req->session_id, (qbi_svc_bc_max_sessions_get(txn->ctx) - 1));
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else
  {
    if (qbi_svc_bc_connect_session_activation_state(txn->ctx, req->session_id) !=
          QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
    {
      QBI_LOG_W_0("Can't query IP address info with no active connection");
      txn->status = QBI_MBIM_STATUS_CONTEXT_NOT_ACTIVATED;
    }
    else
    {
      for (wds_svc_id = QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(req->session_id);
           wds_svc_id <= QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(req->session_id);
           wds_svc_id++)
      {
        if (qbi_svc_bc_connect_is_svc_id_activated(txn->ctx, wds_svc_id))
        {
          /* QMI_WDS_GET_RUNTIME_SETTINGS (0x2d) - Request contains bitmask of
             requested values. */
          qmi_req = (wds_get_runtime_settings_req_msg_v01 *)
            qbi_qmi_txn_alloc_ret_req_buf(
              txn, wds_svc_id, QMI_WDS_GET_RUNTIME_SETTINGS_REQ_V01,
              qbi_svc_bc_ip_configuration_info_q_wds2d_rsp_cb);
          QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

          qmi_req->requested_settings_valid = TRUE;
          qmi_req->requested_settings = (QMI_WDS_MASK_REQ_SETTINGS_DNS_ADDR_V01 |
                                         QMI_WDS_MASK_REQ_SETTINGS_IP_ADDR_V01 |
                                         QMI_WDS_MASK_REQ_SETTINGS_GATEWAY_INFO_V01 |
                                         QMI_WDS_MASK_REQ_SETTINGS_MTU_V01 |
                                         QMI_WDS_MASK_REQ_SETTINGS_IP_FAMILY_V01);
        }
      }

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_bc_ip_configuration_info_q_req() */

/*! @} */

/*! @addtogroup MBIM_CID_DEVICE_SERVICES
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_services_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_DEVICE_SERVICES query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_device_services_q_req
(
  qbi_txn_s *txn
)
{
  uint8 *rsp_buf;
  qbi_svc_bc_device_services_rsp_s *rsp;
  qbi_mbim_offset_size_pair_s *field_desc;
  qbi_svc_bc_device_service_element_s *dev_svc;
  uint32 num_dev_svcs;
  uint32 num_cids;
  uint32 *cid_list;
  qbi_svc_id_e svc_id;
  const uint8 *uuid;
/*-------------------------------------------------------------------------*/
  num_dev_svcs = qbi_svc_num_registered_svcs();

  /* Response is formatted as a static header, then num_dev_svcs device service
     elements, with each element also including a variable length array of
     CIDs. For example:
     [ Header | DevSvc0 | DevSvc0-Cid0 | DevSvc0-Cid1 | ... | DevSvc1 |
       DevSvc1-Cid0 | ... ]  */
  rsp_buf = (uint8 *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_bc_device_services_rsp_s) +
    (num_dev_svcs * sizeof(qbi_mbim_offset_size_pair_s)));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp_buf);

  rsp = (qbi_svc_bc_device_services_rsp_s *) rsp_buf;
  rsp->device_services_count = num_dev_svcs;

  /*! @note Device Service Streams (DSS), i.e. vendor specific payloads on the
      data channel, are currently not supported */
  rsp->max_dss_sessions = 0;

  field_desc = (qbi_mbim_offset_size_pair_s *)
    (rsp_buf + sizeof(qbi_svc_bc_device_services_rsp_s));
  svc_id = QBI_SVC_ID_UNKNOWN;
  while ((svc_id = qbi_svc_get_next_svc_id(svc_id)) != QBI_SVC_ID_UNKNOWN)
  {
    num_cids = qbi_svc_num_cids(svc_id);
    dev_svc = (qbi_svc_bc_device_service_element_s *)
      qbi_txn_rsp_databuf_add_field(
        txn, field_desc, 0,
        (sizeof(qbi_svc_bc_device_service_element_s) +
         (num_cids * sizeof(uint32))), NULL);
    QBI_CHECK_NULL_PTR_RET_ABORT(dev_svc);

    uuid = qbi_svc_svc_id_to_uuid(svc_id);
    QBI_CHECK_NULL_PTR_RET_ABORT(uuid);

    QBI_MEMSCPY(dev_svc->uuid, sizeof(dev_svc->uuid), uuid, QBI_MBIM_UUID_LEN);
    dev_svc->dss_payload_flags = QBI_SVC_BC_DSS_PAYLOAD_NOT_SUPPORTED;
    dev_svc->max_dss_instances = 0;
    dev_svc->cid_count = num_cids;

    cid_list = (uint32 *)
      ((uint8 *) dev_svc + sizeof(qbi_svc_bc_device_service_element_s));
    qbi_svc_get_cid_list(svc_id, cid_list, dev_svc->cid_count);

    field_desc++;
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_device_services_q_req() */

/*! @} */

/*! @addtogroup MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_service_subscribe_list_s_notify_cb
===========================================================================*/
/*!
    @brief Notify callback used to track the result of QMI registration
    activities resulting from updating the event filter

    @details

    @param listener_txn MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST transaction
    @param notifier_txn QMI (de)registration transaction
*/
/*=========================================================================*/
static void qbi_svc_bc_device_service_subscribe_list_s_notify_cb
(
  qbi_txn_s       *listener_txn,
  const qbi_txn_s *notifier_txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(listener_txn);
  QBI_CHECK_NULL_PTR_RET(notifier_txn);

  if (notifier_txn->status != QBI_MBIM_STATUS_SUCCESS)
  {
    QBI_LOG_E_1("QMI registration txn iid %d failed!", notifier_txn->iid);
    (void) qbi_svc_proc_action(listener_txn, QBI_SVC_ACTION_ABORT);
  }
  else if (listener_txn->notifier_count == 0)
  {
    QBI_LOG_D_0("All QMI registration requests completed successfully");
    (void) qbi_svc_proc_action(listener_txn, QBI_SVC_ACTION_SEND_RSP);
  }
} /* qbi_svc_bc_device_service_subscribe_list_s_notify_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_service_subscribe_list_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_device_service_subscribe_list_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_device_service_subscribe_list_s_req_s *req;
  qbi_svc_bc_device_service_subscribe_list_entry_s *entry;
  uint32 entry_index;
  uint32 cid_index;
  qbi_svc_id_e svc_id;
  uint32 *cid_array;
  qbi_mbim_offset_size_pair_s *entry_desc;
  qbi_svc_event_filter_txn_s *filter_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  filter_txn = qbi_svc_event_filter_txn_alloc();
  QBI_CHECK_NULL_PTR_RET_ABORT(filter_txn);

  req = (qbi_svc_bc_device_service_subscribe_list_s_req_s *) txn->req.data;
  entry_desc = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) txn->req.data +
     sizeof(qbi_svc_bc_device_service_subscribe_list_s_req_s));
  for (entry_index = 0; entry_index < req->element_count; entry_index++)
  {
    entry = (qbi_svc_bc_device_service_subscribe_list_entry_s *)
      qbi_txn_req_databuf_get_field(txn, &entry_desc[entry_index], 0, 0);
    if (entry == NULL)
    {
      QBI_LOG_E_1("Couldn't get event subscribe list for entry index %d",
                  entry_index);
      break;
    }

    svc_id = qbi_svc_uuid_to_svc_id(entry->uuid);
    if (svc_id == QBI_SVC_ID_UNKNOWN)
    {
      QBI_LOG_W_1("Ignoring unrecognized UUID at index %d", entry_index);
    }
    else if (entry->cid_count == 0)
    {
      if (!qbi_svc_event_filter_txn_enable_svc(filter_txn, svc_id))
      {
        QBI_LOG_W_1("Couldn't enable all CIDs for svc_id %d", svc_id);
      }
    }
    else
    {
      cid_array = (uint32 *)
        ((uint8 *) entry +
         sizeof(qbi_svc_bc_device_service_subscribe_list_entry_s));
      for (cid_index = 0; cid_index < entry->cid_count; cid_index++)
      {
        /* If we can't enable a CID, it's usually because we don't support it.
           Just log the error and continue. */
        if (!qbi_svc_event_filter_txn_enable_cid(
              filter_txn, svc_id, cid_array[cid_index]))
        {
          QBI_LOG_W_2("Couldn't enable event for CID %d in svc_id %d",
                      cid_array[cid_index], svc_id);
        }
      }
    }
  }

  if (txn->status != QBI_MBIM_STATUS_SUCCESS)
  {
    QBI_LOG_W_1("Parsing event filter failed with status %d", txn->status);
  }
  else
  {
    (void) qbi_txn_notify_setup_listener(
      txn, qbi_svc_bc_device_service_subscribe_list_s_notify_cb);
    if (!qbi_svc_event_filter_txn_commit(txn->ctx, filter_txn, txn))
    {
      QBI_LOG_E_0("Committing event filter transaction failed!");
      txn->status = QBI_MBIM_STATUS_FAILURE;
    }
    else
    {
      /* Copy the request InformationBuffer into the response */
      qbi_txn_set_rsp_buf(txn, &txn->req);
      if (txn->notifier_count > 0)
      {
        /* Response will be sent from the notify callback */
        action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
      }
    }
  }
  qbi_svc_event_filter_txn_free(filter_txn);

  return action;
} /* qbi_svc_bc_device_service_subscribe_list_s_req() */

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_STATISTICS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_stats_get_cur_stats_cache
===========================================================================*/
/*!
    @brief Gets a pointer to the current packet statistics in the cache for
    the given IP type as determined by QMI WDS service ID

    @details

    @param ctx
    @param qmi_svc_id

    @return qbi_svc_bc_packet_statistics_rsp_s* Pointer to cache containing
    current packet stats, or NULL on failure
*/
/*=========================================================================*/
static qbi_svc_bc_packet_statistics_rsp_s *qbi_svc_bc_packet_stats_get_cur_stats_cache
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id
)
{
  qbi_svc_bc_cache_s *cache;
  qbi_svc_bc_packet_statistics_rsp_s *cur_stats;
  uint32 session_id;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(cache);

  session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_svc_id);
  if (QBI_SVC_BC_WDS_SVC_ID_IS_IPV6(qmi_svc_id))
  {
    cur_stats = &cache->packet_statistics.cur_ipv6[session_id];
  }
  else
  {
    cur_stats = &cache->packet_statistics.cur_ipv4[session_id];
  }

  return cur_stats;
} /* qbi_svc_bc_packet_stats_get_cur_stats_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_statistics_finalize_session
===========================================================================*/
/*!
    @brief Starts the process of finalizing the session's packet statistics
    by adding them to the totals

    @details
    Should be called once after disconnecting a session

    @param ctx
    @param wds_svc_id
*/
/*=========================================================================*/
static void qbi_svc_bc_packet_statistics_finalize_session
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e wds_svc_id
)
{
  qbi_txn_s *txn;
  qbi_svc_action_e action;
  wds_get_pkt_statistics_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_BC_MBIM_CID_PACKET_STATISTICS, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  /* QMI_WDS_GET_PKT_STATISTICS (0x24) */
  qmi_req = (wds_get_pkt_statistics_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, wds_svc_id, QMI_WDS_GET_PKT_STATISTICS_REQ_V01,
      qbi_svc_bc_packet_statistics_finalize_session_wds24_rsp_cb);
  if (qmi_req == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    qmi_req->stats_mask = (QMI_WDS_MASK_STATS_TX_BYTES_OK_V01 |
                           QMI_WDS_MASK_STATS_RX_BYTES_OK_V01);
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_packet_statistics_finalize_session() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_statistics_finalize_session_wds24_rsp_cb
===========================================================================*/
/*!
    @brief Processes a QMI_WDS_GET_PKT_STATISTICS_RESP for finalizing the
    session's packet statistics count

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_statistics_finalize_session_wds24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_pkt_statistics_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_pkt_statistics_resp_msg_v01 *) qmi_txn->rsp.data;
  /* We expect to receive ERR_OUT_OF_CALL for this query. */
  if (qmi_rsp->resp.result != QMI_RESULT_FAILURE_V01 ||
      qmi_rsp->resp.error != QMI_ERR_OUT_OF_CALL_V01)
  {
    QBI_LOG_W_1("Unexpected response from QMI: error code %d",
                qmi_rsp->resp.error);
  }
  qbi_svc_bc_packet_statistics_update_cache_totals(
    qmi_txn->ctx, qmi_rsp, qmi_txn->svc_id);

  return QBI_SVC_ACTION_ABORT;
} /* qbi_svc_bc_packet_statistics_finalize_session_wds24_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_statistics_q_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Allocates and populates a MBIM_CID_PACKET_STATISTICS query
    response using the values saved in the cache

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_statistics_q_build_rsp_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_cache_s *cache;
  qbi_svc_bc_packet_statistics_rsp_s *rsp;
  uint32 session_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_bc_packet_statistics_rsp_s *)
    qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_packet_statistics_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  for (session_id = 0; session_id < qbi_svc_bc_max_sessions_get(txn->ctx);
       session_id++)
  {
    rsp->in_discards  +=
      cache->packet_statistics.cur_ipv4[session_id].in_discards +
      cache->packet_statistics.cur_ipv6[session_id].in_discards +
      cache->packet_statistics.total[session_id].in_discards;
    rsp->in_errors    +=
      cache->packet_statistics.cur_ipv4[session_id].in_errors +
      cache->packet_statistics.cur_ipv6[session_id].in_errors +
      cache->packet_statistics.total[session_id].in_errors;
    rsp->in_octets    +=
      cache->packet_statistics.cur_ipv4[session_id].in_octets +
      cache->packet_statistics.cur_ipv6[session_id].in_octets +
      cache->packet_statistics.total[session_id].in_octets;
    rsp->in_packets   +=
      cache->packet_statistics.cur_ipv4[session_id].in_packets +
      cache->packet_statistics.cur_ipv6[session_id].in_packets +
      cache->packet_statistics.total[session_id].in_packets;
    rsp->out_octets   +=
      cache->packet_statistics.cur_ipv4[session_id].out_octets +
      cache->packet_statistics.cur_ipv6[session_id].out_octets +
      cache->packet_statistics.total[session_id].out_octets;
    rsp->out_packets  +=
      cache->packet_statistics.cur_ipv4[session_id].out_packets +
      cache->packet_statistics.cur_ipv6[session_id].out_packets +
      cache->packet_statistics.total[session_id].out_packets;
    rsp->out_errors   +=
      cache->packet_statistics.cur_ipv4[session_id].out_errors +
      cache->packet_statistics.cur_ipv6[session_id].out_errors +
      cache->packet_statistics.total[session_id].out_errors;
    rsp->out_discards +=
      cache->packet_statistics.cur_ipv4[session_id].out_discards +
      cache->packet_statistics.cur_ipv6[session_id].out_discards +
      cache->packet_statistics.total[session_id].out_discards;
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_packet_statistics_q_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_statistics_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PACKET_STATISTICS query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_statistics_q_req
(
  qbi_txn_s *txn
)
{
  wds_get_pkt_statistics_req_msg_v01 *qmi_req;
  uint32 session_id;
  qbi_qmi_svc_e wds_svc_id;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (qbi_svc_bc_connect_overall_activation_state(txn->ctx) ==
        QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
  {
    for (session_id = 0; session_id < qbi_svc_bc_max_sessions_get(txn->ctx);
         session_id++)
    {
      for (wds_svc_id = QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id);
           wds_svc_id <= QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id);
           wds_svc_id++)
      {
        if (qbi_svc_bc_connect_is_svc_id_activated(txn->ctx, wds_svc_id))
        {
          /* QMI_WDS_GET_PKT_STATISTICS (0x24) */
          qmi_req = (wds_get_pkt_statistics_req_msg_v01 *)
            qbi_qmi_txn_alloc_ret_req_buf(
              txn, wds_svc_id, QMI_WDS_GET_PKT_STATISTICS_REQ_V01,
              qbi_svc_bc_packet_statistics_q_wds24_rsp_cb);
          QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

          qmi_req->stats_mask = (QMI_WDS_MASK_STATS_TX_PACKETS_OK_V01 |
                                 QMI_WDS_MASK_STATS_RX_PACKETS_OK_V01 |
                                 QMI_WDS_MASK_STATS_TX_PACKET_ERRORS_V01 |
                                 QMI_WDS_MASK_STATS_RX_PACKET_ERRORS_V01 |
                                 QMI_WDS_MASK_STATS_TX_OVERFLOWS_V01 |
                                 QMI_WDS_MASK_STATS_RX_OVERFLOWS_V01 |
                                 QMI_WDS_MASK_STATS_TX_BYTES_OK_V01 |
                                 QMI_WDS_MASK_STATS_RX_BYTES_OK_V01);
        }
      }
    }
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    action = qbi_svc_bc_packet_statistics_q_build_rsp_from_cache(txn);
  }

  return action;
} /* qbi_svc_bc_packet_statistics_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_statistics_q_wds24_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PKT_STATISTICS_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_statistics_q_wds24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_pkt_statistics_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (wds_get_pkt_statistics_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if (qmi_rsp->resp.error != QMI_ERR_OUT_OF_CALL_V01)
    {
      QBI_LOG_W_1("Received error code %d from QMI", qmi_rsp->resp.error);
    }
  }
  else
  {
    (void) qbi_svc_bc_packet_statistics_update_cache_current_session(
      qmi_txn->ctx, qmi_rsp, qmi_txn->svc_id);
  }

  if (qmi_txn->parent->qmi_txns_pending > 0)
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  else
  {
    action = qbi_svc_bc_packet_statistics_q_build_rsp_from_cache(
      qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_packet_statistics_q_wds24_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_statistics_sync_current_session
===========================================================================*/
/*!
    @brief Query the current packet statistics for the purpose of updating
    the cached information for the current session.

    @details

    @param ctx
*/
/*=========================================================================*/
static void qbi_svc_bc_packet_statistics_sync_current_session
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_BC_MBIM_CID_PACKET_STATISTICS, 0, NULL);
  if (txn != NULL)
  {
    action = qbi_svc_bc_packet_statistics_q_req(txn);
    (void) qbi_svc_proc_action(txn, action);
  }
} /* qbi_svc_bc_packet_statistics_sync_current_session() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_statistics_update_cache_current_session
===========================================================================*/
/*!
    @brief Updates the cached totals for the current session's packet
    statistics (per IP type as determined by QMI WDS service ID)

    @details

    @param ctx
    @param qmi_rsp
    @param qmi_svc_id

    @return boolean TRUE if the byte counts changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_packet_statistics_update_cache_current_session
(
  qbi_ctx_s                                 *ctx,
  const wds_get_pkt_statistics_resp_msg_v01 *qmi_rsp,
  qbi_qmi_svc_e                              qmi_svc_id
)
{
  qbi_svc_bc_packet_statistics_rsp_s *cur_stats;
  boolean byte_counts_changed = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);

  cur_stats = qbi_svc_bc_packet_stats_get_cur_stats_cache(ctx, qmi_svc_id);
  QBI_CHECK_NULL_PTR_RET_FALSE(cur_stats);

  if (qmi_rsp->tx_ok_count_valid &&
      qmi_rsp->tx_ok_count != QBI_QMI_WDS_PKT_STATS_UNKNOWN)
  {
    cur_stats->out_packets = qmi_rsp->tx_ok_count;
  }
  if (qmi_rsp->rx_ok_count_valid &&
      qmi_rsp->rx_ok_count != QBI_QMI_WDS_PKT_STATS_UNKNOWN)
  {
    cur_stats->in_packets = qmi_rsp->rx_ok_count;
  }
  if (qmi_rsp->tx_err_count_valid &&
      qmi_rsp->tx_err_count != QBI_QMI_WDS_PKT_STATS_UNKNOWN)
  {
    cur_stats->out_errors = qmi_rsp->tx_err_count;
  }
  if (qmi_rsp->rx_err_count_valid &&
      qmi_rsp->rx_err_count != QBI_QMI_WDS_PKT_STATS_UNKNOWN)
  {
    cur_stats->in_errors = qmi_rsp->rx_err_count;
  }
  if (qmi_rsp->tx_ofl_count_valid &&
      qmi_rsp->tx_ofl_count != QBI_QMI_WDS_PKT_STATS_UNKNOWN)
  {
    cur_stats->out_discards = qmi_rsp->tx_ofl_count;
  }
  if (qmi_rsp->rx_ofl_count_valid &&
      qmi_rsp->rx_ofl_count != QBI_QMI_WDS_PKT_STATS_UNKNOWN)
  {
    cur_stats->in_discards = qmi_rsp->rx_ofl_count;
  }
  if (qmi_rsp->tx_ok_bytes_count_valid &&
      cur_stats->out_octets != qmi_rsp->tx_ok_bytes_count)
  {
    cur_stats->out_octets = qmi_rsp->tx_ok_bytes_count;
    byte_counts_changed = TRUE;
  }
  if (qmi_rsp->rx_ok_bytes_count_valid &&
      cur_stats->in_octets != qmi_rsp->rx_ok_bytes_count)
  {
    cur_stats->in_octets = qmi_rsp->rx_ok_bytes_count;
    byte_counts_changed = TRUE;
  }

  return byte_counts_changed;
} /* qbi_svc_bc_packet_statistics_update_cache_current_session() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_statistics_update_cache_totals
===========================================================================*/
/*!
    @brief Finalizes the session's packet statistics by adding the session's
    values to the totals, then zeroing out the current session values

    @details

    @param ctx
    @param qmi_rsp
    @param qmi_svc_id
*/
/*=========================================================================*/
static void qbi_svc_bc_packet_statistics_update_cache_totals
(
  qbi_ctx_s                                 *ctx,
  const wds_get_pkt_statistics_resp_msg_v01 *qmi_rsp,
  qbi_qmi_svc_e                              qmi_svc_id
)
{
  qbi_svc_bc_cache_s *cache;
  qbi_svc_bc_packet_statistics_rsp_s *cur_stats;
  uint32 session_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_rsp);

  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  cur_stats = qbi_svc_bc_packet_stats_get_cur_stats_cache(ctx, qmi_svc_id);
  QBI_CHECK_NULL_PTR_RET(cur_stats);

  /* Use the last call byte count if available, since that is the most accurate.
     For all others, just increment from the the last known current session
     count. */
  session_id = QBI_SVC_BC_WDS_SVC_ID_TO_SESSION_ID(qmi_svc_id);
  if (qmi_rsp->last_call_rx_ok_bytes_count_valid)
  {
    cache->packet_statistics.total[session_id].in_octets +=
      qmi_rsp->last_call_rx_ok_bytes_count;
  }
  else
  {
    cache->packet_statistics.total[session_id].in_octets += cur_stats->in_octets;
  }

  if (qmi_rsp->last_call_tx_ok_bytes_count_valid)
  {
    cache->packet_statistics.total[session_id].out_octets +=
      qmi_rsp->last_call_tx_ok_bytes_count;
  }
  else
  {
    cache->packet_statistics.total[session_id].out_octets += cur_stats->out_octets;
  }

  cache->packet_statistics.total[session_id].in_discards  += cur_stats->in_discards;
  cache->packet_statistics.total[session_id].in_errors    += cur_stats->in_errors;
  cache->packet_statistics.total[session_id].in_packets   += cur_stats->in_packets;
  cache->packet_statistics.total[session_id].out_packets  += cur_stats->out_packets;
  cache->packet_statistics.total[session_id].out_errors   += cur_stats->out_errors;
  cache->packet_statistics.total[session_id].out_discards += cur_stats->out_discards;

  QBI_MEMSET(cur_stats, 0, sizeof(qbi_svc_bc_packet_statistics_rsp_s));
} /* qbi_svc_bc_packet_statistics_update_cache_totals() */

/*! @} */

/*! @addtogroup MBIM_CID_NETWORK_IDLE_HINT
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_network_idle_hint_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_NETWORK_IDLE_HINT query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_network_idle_hint_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_network_idle_hint_s *rsp;
/*-------------------------------------------------------------------------*/
  rsp = (qbi_svc_bc_network_idle_hint_s *)
    qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_network_idle_hint_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  /*! @note In the current implementation, the host really isn't expected to
      query this, so we always return enabled */
  rsp->network_idle_hint_state = QBI_SVC_BC_NETWORK_IDLE_HINT_ENABLED;

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_network_idle_hint_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_network_idle_hint_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_NETWORK_IDLE_HINT query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_network_idle_hint_s_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_network_idle_hint_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_network_idle_hint_s *) txn->req.data;
  if (req->network_idle_hint_state == QBI_SVC_BC_NETWORK_IDLE_HINT_DISABLED)
  {
    /* Currently we only expect the host to use the ENABLED parameter, so this
       has no effect. */
    QBI_LOG_W_0("Ignoring request to disable network idle hint state");
    action = qbi_svc_bc_network_idle_hint_q_req(txn);
  }
  else if (req->network_idle_hint_state == QBI_SVC_BC_NETWORK_IDLE_HINT_ENABLED)
  {
    if (qbi_svc_bc_connect_overall_activation_state(txn->ctx) ==
          QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
    {
      qmi_txn = qbi_qmi_txn_alloc(
        txn, qbi_svc_bc_connect_get_first_connected_svc_id(txn->ctx),
        QMI_WDS_GO_DORMANT_REQ_V01,
        qbi_svc_bc_network_idle_hint_s_wds25_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
    else
    {
      QBI_LOG_I_0("Ignoring network idle hint while not connected");
      action = qbi_svc_bc_network_idle_hint_q_req(txn);
    }
  }
  else
  {
    QBI_LOG_E_1("Requested invalid network idle hint state %d",
                req->network_idle_hint_state);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }

  return action;
} /* qbi_svc_bc_network_idle_hint_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_network_idle_hint_s_wds25_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GO_DORMANT_RESP for MBIM_CID_NETWORK_IDLE_HINT
    set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_network_idle_hint_s_wds25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_go_dormant_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_go_dormant_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_W_1("Go dormant request failed with error code %d",
                qmi_rsp->resp.error);
  }

  return qbi_svc_bc_network_idle_hint_q_req(qmi_txn->parent);
} /* qbi_svc_bc_network_idle_hint_s_wds25_rsp_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_FILTERS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_PACKET_FILTERS query

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_packet_filters_s *req;
  wda_packet_filter_get_state_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  if (!qbi_svc_bc_packet_filters_q_sanity_check_req(txn))
  {
    QBI_LOG_E_0("Request failed sanity check");
  }
  else if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_WDA))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else
  {
    /* Builds QMI_WDA_PACKET_FILTER_GET_STATE_REQ_V01 */
    qmi_req = (wda_packet_filter_get_state_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDA, QMI_WDA_PACKET_FILTER_GET_STATE_REQ_V01,
        qbi_svc_bc_packet_filters_q_wda24_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
    qmi_req->ips_id_valid = TRUE;
    qmi_req->ips_id = (uint8)req->session_id;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_packet_filters_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_wda_client_release
===========================================================================*/
/*!
    @brief Handles releasing of wda client

    @param ctx, action

    @return None
*/
/*=========================================================================*/
void  qbi_svc_bc_wda_client_release
(
  qbi_ctx_s               *ctx,
  uint8                    action
)
{
/*-------------------------------------------------------------------------*/

  if (QBI_SVC_ACTION_ABORT == action || QBI_SVC_ACTION_SEND_RSP == action)
  {
    QBI_LOG_D_0("Releasing WDA client");
    qbi_qmi_release_svc_handle(ctx, QBI_QMI_SVC_WDA);
  }

  return;
}/* qbi_svc_bc_wda_client_release */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_q_build_rsp_list
===========================================================================*/
/*!
    @brief Allocates and populates the response with PFC 0 for a query to
    MBIM_CID_PACKET_FILTERS

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_q_build_rsp_list
(
  qbi_txn_s *txn,
  uint32     pfc
)
{
  qbi_svc_bc_packet_filters_s *rsp;
  qbi_svc_bc_packet_filters_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  rsp = (qbi_svc_bc_packet_filters_s *) qbi_txn_alloc_rsp_buf(txn,
    sizeof(qbi_svc_bc_packet_filters_s) + (pfc * sizeof(qbi_mbim_offset_size_pair_s)));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
  rsp->session_id = req->session_id;
  rsp->packet_filters_count = pfc;

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_packet_filters_q_build_rsp_list() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_q_fill_rule_rsp
===========================================================================*/
/*!
    @brief Allocates and populates the response for a query to
    MBIM_CID_PACKET_FILTERS

    @param txn

    @return TRUE if success
*/
/*=========================================================================*/
static boolean qbi_svc_bc_packet_filters_q_fill_rule_rsp
(
  qbi_qmi_txn_s *qmi_txn
)
{
  boolean status = FALSE;
  qbi_svc_bc_packet_filters_handles_info_s *handles_buf;
  qbi_txn_s *txn;
  wda_packet_filter_get_rule_resp_msg_v01 *rule_rsp;
  qbi_mbim_offset_size_pair_s *list_ol;
  qbi_mbim_offset_size_pair_s ol_pair;
  qbi_mbim_bc_single_packet_filter_s *filter;
  void *pbuf;
  uint32 initial_size;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  rule_rsp = (wda_packet_filter_get_rule_resp_msg_v01 *)qmi_txn->rsp.data;
  QBI_CHECK_NULL_PTR_RET_ABORT(rule_rsp);

  if (rule_rsp->rule.pattern_len == 0 ||
      rule_rsp->rule.pattern_len > QBI_SVC_BC_PACKET_FILTER_MAX_SIZE ||
      rule_rsp->rule.pattern_len != rule_rsp->rule.mask_len)
  {
    QBI_LOG_E_2("GET_RULE_RESP length error pattern %d mask %d",
                rule_rsp->rule.pattern_len, rule_rsp->rule.mask_len);
  }
  else
  {
    txn = qmi_txn->parent;
    QBI_CHECK_NULL_PTR_RET_ABORT(txn);
    QBI_CHECK_NULL_PTR_RET_ABORT(txn->rsp.data);

    handles_buf =
      (qbi_svc_bc_packet_filters_handles_info_s *)qmi_txn->parent->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(handles_buf);

    list_ol = (qbi_mbim_offset_size_pair_s *)
      ((uint8 *)txn->rsp.data + sizeof(qbi_svc_bc_packet_filters_s) +
       handles_buf->pos * sizeof(qbi_mbim_offset_size_pair_s));

    initial_size = txn->infobuf_len_total;

    filter = (qbi_mbim_bc_single_packet_filter_s *)qbi_txn_rsp_databuf_add_field(
        txn, &ol_pair, 0, sizeof(qbi_mbim_bc_single_packet_filter_s), NULL);
    QBI_CHECK_NULL_PTR_RET_ABORT(filter);
    filter->size = rule_rsp->rule.pattern_len;
    list_ol->offset = ol_pair.offset;

    pbuf = qbi_txn_rsp_databuf_add_field(txn, &ol_pair, 0, filter->size,
                                         rule_rsp->rule.pattern);
    QBI_CHECK_NULL_PTR_RET_ABORT(pbuf);
    filter->filter_offset = ol_pair.offset;

    pbuf = qbi_txn_rsp_databuf_add_field(txn, &ol_pair, 0, filter->size,
                                         rule_rsp->rule.mask);
    QBI_CHECK_NULL_PTR_RET_ABORT(pbuf);
    filter->mask_offset = ol_pair.offset;
    list_ol->size = txn->infobuf_len_total - initial_size;

    status = qbi_txn_rsp_databuf_consolidate(txn);
  }

  return status;
} /* qbi_svc_bc_packet_filters_q_fill_rule_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_q_sanity_check_req
===========================================================================*/
/*!
    @brief Performs sanity check of parameters provided by the host for a
    MBIM_CID_PACKET_FILTERS set request

    @details
    Sets the status field in the transaction to the proper value if a check
    fails.

    These checks are for the validity of the parameters provided by the
    host, as opposed to the pre-(de)activation checks, which are checks
    of whether the device state permits the requested action.

    @param txn

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_packet_filters_q_sanity_check_req
(
  qbi_txn_s *txn
)
{
  boolean success = FALSE;
  qbi_svc_bc_packet_filters_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
  if (req->session_id >= qbi_svc_bc_max_sessions_get(txn->ctx))
  {
    QBI_LOG_E_2("Requested SessionId (%d) is past maximum (%d)",
                req->session_id, (qbi_svc_bc_max_sessions_get(txn->ctx) - 1));
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else
  {
    QBI_LOG_D_0("Request passed sanity checks for packet filters query request");
    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_packet_filters_q_sanity_check_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_q_wda24_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDA_PACKET_FILTER_GET_STATE_RESP_V01

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_q_wda24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wda_packet_filter_get_state_resp_msg_v01 *qmi_rsp;
  wda_packet_filter_get_rule_handles_req_msg_v01 *qmi_req;
  qbi_svc_bc_packet_filters_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  qmi_rsp = (wda_packet_filter_get_state_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (qmi_rsp->filtering_is_enabled_valid && qmi_rsp->filtering_is_enabled)
  {

    QBI_LOG_I_2("packet filter setting enabled alllowed(%d:%d) get list...",
                qmi_rsp->filter_is_restrictive_valid,
                qmi_rsp->filter_is_restrictive);

    qmi_req = (wda_packet_filter_get_rule_handles_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_WDA,
        QMI_WDA_PACKET_FILTER_GET_RULE_HANDLES_REQ_V01,
        qbi_svc_bc_packet_filters_q_wda27_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    req = (qbi_svc_bc_packet_filters_s *) qmi_txn->parent->req.data;
    qmi_req->ips_id_valid = TRUE;
    qmi_req->ips_id = (uint8)req->session_id;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    QBI_LOG_I_0("packet filter setting is disabled! Send qbi resp now.");
    /* swap req and response buffer */
    action = qbi_svc_bc_packet_filters_q_build_rsp_list(qmi_txn->parent, 0);
  }

  qbi_svc_bc_wda_client_release(qmi_txn->ctx,action);

  return action;
} /* qbi_svc_bc_packet_filters_q_wda24_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_q_wda27_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDA_PACKET_FILTER_GET_RULE_HANDLES_RESP_V01

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_q_wda27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wda_packet_filter_get_rule_handles_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_packet_filters_handles_info_s *handles_buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (wda_packet_filter_get_rule_handles_resp_msg_v01 *) qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->handle_valid)
  {
    QBI_LOG_E_0("packet filter get list error");
  }
  else if (qmi_rsp->handle_len == 0)
  {
    QBI_LOG_I_0("packet filter get list returns none. Send qbi resp now.");
    action = qbi_svc_bc_packet_filters_q_build_rsp_list(qmi_txn->parent, 0);
  }
  else if (qmi_rsp->handle_len > QBI_SVC_BC_PACKET_FILTER_MAX_NUM)
  {
    QBI_LOG_E_2("Too many packet filter (%d > %d) rules listed, rsp err",
                qmi_rsp->handle_len, QBI_SVC_BC_PACKET_FILTER_MAX_NUM);
  }
  else
  {
    qmi_txn->parent->info =
      QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_bc_packet_filters_handles_info_s));
    handles_buf = (qbi_svc_bc_packet_filters_handles_info_s *)
      qmi_txn->parent->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(handles_buf);

    handles_buf->num = qmi_rsp->handle_len;
    QBI_MEMSCPY(handles_buf->handles, sizeof(handles_buf->handles),
                qmi_rsp->handle, (qmi_rsp->handle_len * sizeof(uint32)));

    if (qbi_svc_bc_packet_filters_q_build_rsp_list(
          qmi_txn->parent, qmi_rsp->handle_len) == QBI_SVC_ACTION_ABORT)
    {
      QBI_LOG_E_0("failed to build rsp list head, abort");
    }
    else
    {
      QBI_LOG_I_1("packet filter lists (%d) rules, sending get rule req",
                  qmi_rsp->handle_len);
      action = qbi_svc_bc_packet_filters_q_wda28_build_req(
        qmi_txn->parent, NULL);
    }
  }

  qbi_svc_bc_wda_client_release(qmi_txn->ctx,action);

  return action;
} /* qbi_svc_bc_packet_filters_q_wda27_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_q_wda28_build_req
===========================================================================*/
/*!
    @brief Builds QMI_WDA_PACKET_FILTER_GET_RULE_REQ_V01

    @details

    @param txn
    @param handle

    @return TRUE if success
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_q_wda28_build_req
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_packet_filters_s *req;
  wda_packet_filter_get_rule_req_msg_v01 *qmi_req;
  qbi_svc_bc_packet_filters_handles_info_s *handles_buf;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  handles_buf = (qbi_svc_bc_packet_filters_handles_info_s *)txn->info;
  handles_buf->pos = (qmi_txn) ? (handles_buf->pos+1) : 0;

  if (handles_buf->num > QBI_SVC_BC_PACKET_FILTER_MAX_NUM)
  {
    QBI_LOG_E_2("packet filter lists (%d) rules over limit (%d)",
                handles_buf->num, QBI_SVC_BC_PACKET_FILTER_MAX_NUM);
  }
  else if (handles_buf->pos >= handles_buf->num)
  {
    QBI_LOG_I_1("packet filter got all %d rules, send rsp", handles_buf->num);
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    if (qmi_txn == NULL)
    {
      qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_WDA,
                                  QMI_WDA_PACKET_FILTER_GET_RULE_REQ_V01,
                                  qbi_svc_bc_packet_filters_q_wda28_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
    }
    else
    {
      qbi_qmi_txn_reset(qmi_txn);
    }

    qmi_req = (wda_packet_filter_get_rule_req_msg_v01 *) qmi_txn->req.data;
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
    qmi_req->handle = handles_buf->handles[handles_buf->pos];

    if (qmi_req->handle == 0)
    {
      QBI_LOG_E_2("packet filter rule %d invalid handle (%x)",
                  handles_buf->num, qmi_req->handle);
    }
    else
    {
      QBI_LOG_I_3("packet filter get rule req for rule %d/%d handle %x",
                  handles_buf->pos, handles_buf->num, qmi_req->handle);

      req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
      qmi_req->ips_id_valid = TRUE;
      qmi_req->ips_id = (uint8)req->session_id;

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_bc_packet_filters_q_wda28_build_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_q_wda28_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDA_PACKET_FILTER_GET_RULE_RESP_V01

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_q_wda28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wda_packet_filter_get_rule_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (wda_packet_filter_get_rule_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qbi_svc_bc_packet_filters_q_fill_rule_rsp(qmi_txn))
  {
    QBI_LOG_E_0("Received packet filter get rule resp! error filling rsp...");
  }
  else
  {
    QBI_LOG_I_0("Send next packet filter get rule qmi resp...");
    action = qbi_svc_bc_packet_filters_q_wda28_build_req(qmi_txn->parent,
                                                         qmi_txn);
  }

  qbi_svc_bc_wda_client_release(qmi_txn->ctx,action);

  return action;
} /* qbi_svc_bc_packet_filters_q_wda28_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_add_rule
===========================================================================*/
/*!
    @brief Uses filter infomation in qbi s_req to build qmi add rule requests

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_add_rule
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_packet_filters_s *req;
  qbi_mbim_offset_size_pair_s *list_ol;
  qbi_mbim_offset_size_pair_s ol_pair;
  qbi_mbim_bc_single_packet_filter_s *filter;
  uint32 *pattern;
  uint32 *mask;
  uint32 *pos;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  /* packet_filters_count has been sanity checked */
  req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
  QBI_LOG_I_2("_packet_filters_s_add_rules SessionId(%d) PFC(%d)",
              req->session_id, req->packet_filters_count);

  /* use txn->info to store the position of processed rule entry */
  if (txn->info == NULL)
  {
    txn->info = QBI_MEM_MALLOC_CLEAR(sizeof(uint32));
    QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
    pos = txn->info;
  }
  else
  {
    pos = txn->info;
    *pos = *pos + 1;
  }

  if (*pos >= req->packet_filters_count)
  {
    QBI_LOG_I_0("All packet filter rules are added!");

    /* build qmi request to enable filter */
    action = qbi_svc_bc_packet_filters_s_wda22_build_req(txn);
  }
  else
  {
    list_ol = (qbi_mbim_offset_size_pair_s *)
      ((uint8 *)txn->req.data + sizeof(qbi_svc_bc_packet_filters_s) +
       (*pos)*sizeof(qbi_mbim_offset_size_pair_s));

    QBI_LOG_D_3("_packet_filters_s_add_rule %d offset %d size %d",
                *pos, list_ol->offset, list_ol->size);

    filter = (qbi_mbim_bc_single_packet_filter_s *)qbi_txn_req_databuf_get_field(
        txn, list_ol, 0, 0);
    QBI_CHECK_NULL_PTR_RET_ABORT(filter);

    ol_pair.size = filter->size;
    ol_pair.offset = filter->filter_offset;
    pattern = (uint32 *)qbi_txn_req_databuf_get_field(
      txn, &ol_pair, list_ol->offset, QBI_SVC_BC_PACKET_FILTER_MAX_SIZE);
    QBI_CHECK_NULL_PTR_RET_ABORT(pattern);

    ol_pair.offset = filter->mask_offset;
    mask = (uint32 *)qbi_txn_req_databuf_get_field(
      txn, &ol_pair, list_ol->offset, QBI_SVC_BC_PACKET_FILTER_MAX_SIZE);
    QBI_CHECK_NULL_PTR_RET_ABORT(mask);

    if (!qbi_svc_bc_packet_filters_s_wda25_build_req(
          txn, qmi_txn, filter->size, pattern, mask))
    {
      QBI_LOG_E_0("_packet_filters_s_add_rules qmi build request failed");
    }
    else
    {
      QBI_LOG_I_0("_packet_filters_s_add_rules sending add rule req");
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_bc_packet_filters_s_add_rule() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_disable
===========================================================================*/
/*!
    @brief disable packet filters

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_disable
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_I_0("_packet_filters_s_disable");

  action = qbi_svc_bc_packet_filters_s_wda23_build_req(txn);

  return action;
} /* qbi_svc_bc_packet_filters_s_disable() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_enable
===========================================================================*/
/*!
    @brief enable packet filters

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_enable
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_packet_filters_s *req;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
  QBI_LOG_I_2("_packet_filters_s_enable SessionId(%d) PFC(%d)",
              req->session_id, req->packet_filters_count);

  /* Delete all entries first*/
  action = qbi_svc_bc_packet_filters_s_wda26_build_req(txn, 0);

  return action;
} /* qbi_svc_bc_packet_filters_s_enable() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_req
===========================================================================*/
/*!
    @brief Handles MBIM_CID_PACKET_FILTERS set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_packet_filters_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
  if (!qbi_svc_bc_packet_filters_s_sanity_check_req(txn))
  {
    QBI_LOG_E_0("Request failed sanity check");
  }
  else if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_WDA))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else if (req->packet_filters_count > 0)
  {
    if (qbi_svc_bc_connect_session_activation_state(txn->ctx, req->session_id) !=
          QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
    {
      QBI_LOG_E_0("Can't enable packet filter in non-activated state");
      txn->status = QBI_MBIM_STATUS_CONTEXT_NOT_ACTIVATED;
    }
    else
    {
      action = qbi_svc_bc_packet_filters_s_enable(txn);
    }
  }
  else
  {
    action = qbi_svc_bc_packet_filters_s_disable(txn);
  }

  return action;
} /* qbi_svc_bc_packet_filters_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_sanity_check_req
===========================================================================*/
/*!
    @brief Performs sanity check of parameters provided by the host for a
    MBIM_CID_PACKET_FILTERS set request

    @details
    Sets the status field in the transaction to the proper value if a check
    fails.

    These checks are for the validity of the parameters provided by the
    host, as opposed to the pre-(de)activation checks, which are checks
    of whether the device state permits the requested action.

    @param txn

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_packet_filters_s_sanity_check_req
(
  qbi_txn_s *txn
)
{
  boolean success = TRUE;
  qbi_svc_bc_packet_filters_s *req;
  qbi_mbim_bc_single_packet_filter_s *filter;
  qbi_mbim_offset_size_pair_s *list_ol;
  qbi_mbim_offset_size_pair_s ol_pair;
  uint32 index = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
  if (req->session_id >= qbi_svc_bc_max_sessions_get(txn->ctx))
  {
    QBI_LOG_E_2("Requested SessionId (%d) is past maximum (%d)",
                req->session_id, (qbi_svc_bc_max_sessions_get(txn->ctx) - 1));
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    success = FALSE;
  }
  else if (req->packet_filters_count > QBI_SVC_BC_PACKET_FILTER_MAX_NUM)
  {
    QBI_LOG_E_2("Requested PacketFiltersCount (%d) is past maximum (%d)",
                req->packet_filters_count, QBI_SVC_BC_PACKET_FILTER_MAX_NUM);
    /* Per MBIM Errata 11292012, when filter can't be fit, destroy previous
       loaded filter and return success with count set to 0.
       Update count in req to 0 here falls through the disable logic */
    req->packet_filters_count = 0;
  }
  else if (req->packet_filters_count > 0)
  {
    /* Sanity check filter size for each filter */
    for (index = 0; index < req->packet_filters_count; index++)
    {
      list_ol = (qbi_mbim_offset_size_pair_s *)
        ((uint8 *)req + sizeof(qbi_svc_bc_packet_filters_s) +
         index*sizeof(qbi_mbim_offset_size_pair_s));

      filter = (qbi_mbim_bc_single_packet_filter_s *)
        qbi_txn_req_databuf_get_field(txn, list_ol, 0, 0);
      if (!filter)
      {
        QBI_LOG_E_1("Filter at index %d can't be retrieved", index);
        txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
        success = FALSE;
        break;
      }
      else if (filter->size > QBI_SVC_BC_PACKET_FILTER_MAX_SIZE)
      {
        QBI_LOG_E_3("Req filter size(%d) at index(%d) is past maximum(%d)",
                    filter->size, index, QBI_SVC_BC_PACKET_FILTER_MAX_SIZE);
        /* Update count in req to 0 here falls through the disable logic */
        req->packet_filters_count = 0;
        break;
      }
      else
      {
        /* Since we're already checking sizes for each filter, run sanity check
           here for pattern/mask as well to avoid processing part of filters */
        ol_pair.size = filter->size;
        ol_pair.offset = filter->filter_offset;
        if (!qbi_txn_req_databuf_get_field(
              txn, &ol_pair, list_ol->offset,
              QBI_SVC_BC_PACKET_FILTER_MAX_SIZE))
        {
          QBI_LOG_E_2("Pattern at index %d offset(%d) can't be retrieved",
                      index, ol_pair.offset);
          txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
          success = FALSE;
          break;
        }

        ol_pair.offset = filter->mask_offset;
        if (!qbi_txn_req_databuf_get_field(
              txn, &ol_pair, list_ol->offset,
              QBI_SVC_BC_PACKET_FILTER_MAX_SIZE))
        {
          QBI_LOG_E_2("Mask at index %d offset(%d) can't be retrieved",
                      index, ol_pair.offset);
          txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
          success = FALSE;
          break;
        }
      }
    }
  }

  return success;
} /* qbi_svc_bc_packet_filters_s_sanity_check_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_wda22_build_req
===========================================================================*/
/*!
    @brief Builds QMI_WDA_PACKET_FILTER_ENABLE_REQ_V01

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda22_build_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_packet_filters_s *req;
  wda_packet_filter_enable_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  /* for each entry in the list */
  qmi_req = (wda_packet_filter_enable_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDA, QMI_WDA_PACKET_FILTER_ENABLE_REQ_V01,
      qbi_svc_bc_packet_filters_s_wda22_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->filter_is_restrictive = TRUE;

  req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
  qmi_req->ips_id_valid = TRUE;
  qmi_req->ips_id = (uint8)req->session_id;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_packet_filters_s_wda22_build_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_wda22_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDA_PACKET_FILTER_ENABLE_RESP_V01

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wda_packet_filter_enable_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (wda_packet_filter_enable_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_0("QMI_WDA_PACKET_FILTER_ENABLE_RESP_V01 received!");
    /* swap req and response buffer */
    qbi_txn_set_rsp_buf(qmi_txn->parent, &qmi_txn->parent->req);
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  qbi_svc_bc_wda_client_release(qmi_txn->ctx,action);

  return action;
} /* qbi_svc_bc_packet_filters_s_wda22_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_wda23_build_req
===========================================================================*/
/*!
    @brief Builds QMI_WDA_PACKET_FILTER_DISABLE_REQ_V01

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda23_build_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_packet_filters_s *req;
  wda_packet_filter_disable_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  qmi_req = (wda_packet_filter_disable_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDA, QMI_WDA_PACKET_FILTER_DISABLE_REQ_V01,
      qbi_svc_bc_packet_filters_s_wda23_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
  qmi_req->ips_id_valid = TRUE;
  qmi_req->ips_id = (uint8)req->session_id;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_packet_filters_s_wda23_build_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_wda23_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDA_PACKET_FILTER_DISABLE_RESP_V01

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda23_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wda_packet_filter_disable_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (wda_packet_filter_disable_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_0("QMI_WDA_PACKET_FILTER_DISABLE_RESP_V01 received!");
    /* swap req and response buffer */
    qbi_txn_set_rsp_buf(qmi_txn->parent, &qmi_txn->parent->req);
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  qbi_svc_bc_wda_client_release(qmi_txn->ctx,action);

  return action;
} /* qbi_svc_bc_packet_filters_s_wda23_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_wda25_build_req
===========================================================================*/
/*!
    @brief Builds QMI_WDA_PACKET_FILTER_ADD_RULE_REQ_V01

    @details

    @param txn
    @param size
    @param pattern
    @param mask

    @return TRUE if no error
*/
/*=========================================================================*/
static boolean qbi_svc_bc_packet_filters_s_wda25_build_req
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn,
  uint32         size,
  uint32        *pattern,
  uint32        *mask
)
{
  qbi_svc_bc_packet_filters_s *req;
  wda_packet_filter_add_rule_req_msg_v01 *qmi_req;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(pattern);
  QBI_CHECK_NULL_PTR_RET_FALSE(mask);

  if (qmi_txn == NULL)
  {
    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_WDA,
                                QMI_WDA_PACKET_FILTER_ADD_RULE_REQ_V01,
                                qbi_svc_bc_packet_filters_s_wda25_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn);
  }
  else
  {
    qbi_qmi_txn_reset(qmi_txn);
  }

  qmi_req = (wda_packet_filter_add_rule_req_msg_v01 *)qmi_txn->req.data;
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_req);

  if (size > 0 || size <= QBI_SVC_BC_PACKET_FILTER_MAX_SIZE)
  {
    qmi_req->rule.pattern_len = size;
    QBI_MEMSCPY(qmi_req->rule.pattern, sizeof(qmi_req->rule.pattern),
                pattern, size);

    qmi_req->rule.mask_len = size;
    QBI_MEMSCPY(qmi_req->rule.mask, sizeof(qmi_req->rule.mask), mask, size);

    req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
    qmi_req->ips_id_valid = TRUE;
    qmi_req->ips_id = (uint8)req->session_id;

    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_packet_filters_s_wda25_build_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_wda25_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDA_PACKET_FILTER_ADD_RULE_RESP_V01

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda25_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wda_packet_filter_add_rule_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (wda_packet_filter_add_rule_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_D_0("Packet filter add rule qmi response is received!");

    /* add next rule if it's available */
    action = qbi_svc_bc_packet_filters_s_add_rule(qmi_txn->parent, qmi_txn);
  }

  qbi_svc_bc_wda_client_release(qmi_txn->ctx,action);

  return action;
} /* qbi_svc_bc_packet_filters_s_wda25_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_wda26_build_req
===========================================================================*/
/*!
    @brief Builds QMI_WDA_PACKET_FILTER_DELETE_RULE_REQ_V01

    @details

    @param txn
    @param handle

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda26_build_req
(
  qbi_txn_s *txn,
  uint32     handle
)
{
  qbi_svc_bc_packet_filters_s *req;
  wda_packet_filter_delete_rule_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  qmi_req = (wda_packet_filter_delete_rule_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDA, QMI_WDA_PACKET_FILTER_DELETE_RULE_REQ_V01,
      qbi_svc_bc_packet_filters_s_wda26_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  if (handle == 0)
  {
    QBI_LOG_I_0("packet_filters_s: delete all filters");
    qmi_req->handle_valid = FALSE;
  }
  else
  {
    QBI_LOG_I_1("packet_filters_s: delete filter with handle %x", handle);
    qmi_req->handle = handle;
  }

  req = (qbi_svc_bc_packet_filters_s *) txn->req.data;
  qmi_req->ips_id_valid = TRUE;
  qmi_req->ips_id = (uint8)req->session_id;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_packet_filters_s_wda26_build_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_packet_filters_s_wda26_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDA_PACKET_FILTER_DELETE_RULE_RESP_V01

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_packet_filters_s_wda26_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wda_packet_filter_delete_rule_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (wda_packet_filter_delete_rule_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (qmi_rsp->handle_valid && qmi_rsp->handle)
  {
    QBI_LOG_E_1("packet filter delete rule w handle(%x) not supported",
                qmi_rsp->handle);
  }
  else
  {
    QBI_LOG_I_0("Packet filter delete all rule qmi response received!");

    /* build qmi request to add filter one by one */
    action = qbi_svc_bc_packet_filters_s_add_rule(qmi_txn->parent, NULL);
  }

  return action;
} /* qbi_svc_bc_packet_filters_s_wda26_rsp_cb() */


/*! @} */


/*=============================================================================

  Module Protected Function Definitions, exposed in qbi_svc_bc_common.h

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_append_3gpp_provider_id
===========================================================================*/
/*!
    @brief Convert a binary MCC+MNC into MBIM ProviderId representation
    (UTF-16, 5 or 6 decimal characters), and append it to the response

    @details

    @param txn
    @param provider_id
    @param initial_offset
    @param mcc
    @param mnc
    @param mnc_is_3_digits
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_append_3gpp_provider_id
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *provider_id,
  uint32                       initial_offset,
  uint16                       mcc,
  uint16                       mnc,
  boolean                      mnc_is_3_digits
)
{
  boolean success = TRUE;
  char provider_id_ascii[QBI_SVC_BC_PROVIDER_ID_ASCII_MAX_LEN];
/*-------------------------------------------------------------------------*/
  if (mnc_is_3_digits)
  {
    (void) QBI_SNPRINTF(provider_id_ascii, sizeof(provider_id_ascii),
                        "%03d%03d", mcc, mnc);
  }
  else
  {
    (void) QBI_SNPRINTF(provider_id_ascii, sizeof(provider_id_ascii),
                        "%03d%02d", mcc, mnc);
  }
  if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
        txn, provider_id, initial_offset, QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES,
        provider_id_ascii, sizeof(provider_id_ascii)))
  {
    QBI_LOG_E_0("Couldn't append ProviderId!");
    success = FALSE;
  }

  return success;
} /* qbi_svc_bc_provider_append_3gpp_provider_id() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_append_3gpp2_provider_id
===========================================================================*/
/*!
    @brief Convert a binary SID into MBIM ProviderId representation
    (UTF-16, 5 decimal characters), and append it to the response

    @details

    @param txn
    @param provider_id
    @param initial_offset
    @param sid
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_append_3gpp2_provider_id
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *provider_id,
  uint32                       initial_offset,
  uint16                       sid
)
{
  boolean success = TRUE;
  char provider_id_ascii[QBI_SVC_BC_PROVIDER_ID_ASCII_MAX_LEN];
/*-------------------------------------------------------------------------*/
  (void) QBI_SNPRINTF(provider_id_ascii, sizeof(provider_id_ascii), "%05d",
                      sid);
  if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
        txn, provider_id, initial_offset, QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES,
        provider_id_ascii, sizeof(provider_id_ascii)))
  {
    QBI_LOG_E_0("Couldn't append ProviderId!");
    success = FALSE;
  }

  return success;
} /* qbi_svc_bc_provider_append_3gpp2_provider_id() */

/*! @addtogroup MBIM_CID_CONNECT
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_deactivate_all_sessions
===========================================================================*/
/*!
    @brief Issues an internal request to disconnect all active sessions

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_bc_connect_deactivate_all_sessions
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
  qbi_svc_bc_connect_s_req_s *req;
  qbi_svc_action_e action;
  uint32 session_id;
/*-------------------------------------------------------------------------*/
  for (session_id = 0; session_id < qbi_svc_bc_max_sessions_get(ctx);
       session_id++)
  {
    /* Only deactivate connected sesisons */
    if (qbi_svc_bc_connect_is_session_connected(ctx, session_id))
    {
      txn = qbi_txn_alloc_event(
        ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_CONNECT);
      QBI_CHECK_NULL_PTR_RET(txn);

      req = qbi_util_buf_alloc(&txn->req, sizeof(qbi_svc_bc_connect_s_req_s));
      QBI_CHECK_NULL_PTR_RET(req);

      req->session_id = session_id;
      req->activation_cmd = QBI_SVC_BC_ACTIVATION_CMD_DEACTIVATE;

      /* Need to use an event transaction here to ensure the host is notified
         of the DEACTIVATED state. */
      action = qbi_svc_bc_connect_s_req(txn);

      (void) qbi_svc_proc_action(txn, action);
    }
  }
} /* qbi_svc_bc_connect_deactivate_all_sessions() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_get_first_connected_svc_id
===========================================================================*/
/*!
    @brief Locate the first connected WDS service ID

    @details
    Scan all sessions to find first WDS client with IPv4 connected. If not
    found, scan again for first WDS client with IPv6 connected.

    @param ctx

    @return qbi_qmi_svc_e
*/
/*=========================================================================*/
qbi_qmi_svc_e qbi_svc_bc_connect_get_first_connected_svc_id
(
  qbi_ctx_s *ctx
)
{
  qbi_qmi_svc_e wds_svc_id = QBI_QMI_SVC_WDS;
  qbi_svc_bc_cache_s *cache;
  uint32 session_id;
  boolean found_connected_ipv4 = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  if (cache == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
  }
  else
  {
    for (session_id = 0; session_id < qbi_svc_bc_max_sessions_get(ctx);
         session_id++)
    {
      if (qbi_svc_bc_connect_is_session_connected(ctx, session_id) &&
          qbi_svc_bc_connect_is_session_ipv4_activated(ctx, session_id))
      {
        wds_svc_id = QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id);
        found_connected_ipv4 = TRUE;
        break;
      }
    }

    if (found_connected_ipv4 == FALSE)
    {
      for (session_id = 0; session_id < qbi_svc_bc_max_sessions_get(ctx);
           session_id++)
      {
        if (qbi_svc_bc_connect_is_session_connected(ctx, session_id) &&
            qbi_svc_bc_connect_is_session_ipv6_activated(ctx, session_id))
        {
          wds_svc_id = QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id);
          break;
        }
      }
    }
  }

  return wds_svc_id;
} /* qbi_svc_bc_connect_get_first_connected_svc_id() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_is_connected
===========================================================================*/
/*!
    @brief Checks whether the device has an active connection up (includes
    deactivating state)

    @details

    @param ctx

    @return boolean TRUE if connected, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_connect_is_connected
(
  const qbi_ctx_s *ctx
)
{
  uint32 activation_state;
/*-------------------------------------------------------------------------*/
  activation_state = qbi_svc_bc_connect_overall_activation_state(ctx);
  return (activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED ||
          activation_state == QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING);
} /* qbi_svc_bc_connect_is_connected() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_is_loopback
===========================================================================*/
/*!
    @brief Checks the loopback state of current connection from cache

    @details

    @param ctx

    @return boolean TRUE if in loopback mode, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_connect_is_loopback
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  /*!@note loopback only uses session ID 0 */
  return cache->connect.sessions[0].is_loopback;
} /* qbi_svc_bc_connect_is_loopback */
/*! @} */

/*===========================================================================
  FUNCTION: qbi_svc_bc_context_type_id_to_uuid
===========================================================================*/
/*!
    @brief Maps the internal short representation of a context type (internal
    constant, e.g. QBI_SVC_BC_CONTEXT_TYPE_NONE) to its UUID representation

    @details

    @param context_type_id

    @return const uint8* Pointer to array of QBI_MBIM_UUID_LEN bytes
    containing UUID, or NULL if invalid context_type_id provided
*/
/*=========================================================================*/
const uint8 *qbi_svc_bc_context_type_id_to_uuid
(
  uint32 context_type_id
)
{
  const uint8 *context_type_uuid = NULL;
/*-------------------------------------------------------------------------*/
  if (context_type_id < QBI_SVC_BC_CONTEXT_TYPE_MAX)
  {
    context_type_uuid = qbi_svc_bc_context_types[context_type_id];
  }

  return context_type_uuid;
} /* qbi_svc_bc_context_type_id_to_uuid() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_context_type_uuid_to_id
===========================================================================*/
/*!
    @brief Maps a UUID to its short internal representation (internal
    constant, e.g. QBI_SVC_BC_CONTEXT_TYPE_NONE)

    @details

    @param context_type_uuid
    @param context_type_id If this function returns TRUE, will be populated
    with the context type ID for the given UUID. If this function returns
    FALSE, then undefined.

    @return boolean TRUE if the mapping was successful, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_context_type_uuid_to_id
(
  const uint8 *context_type_uuid,
  uint32      *context_type_id
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(context_type_uuid);
  QBI_CHECK_NULL_PTR_RET_FALSE(context_type_id);

  for (*context_type_id = 0; *context_type_id < QBI_SVC_BC_CONTEXT_TYPE_MAX;
       (*context_type_id)++)
  {
    if (QBI_MEMCMP(context_type_uuid,
                   qbi_svc_bc_context_types[*context_type_id],
                   QBI_MBIM_UUID_LEN) == 0)
    {
      break;
    }
  }

  if (*context_type_id < QBI_SVC_BC_CONTEXT_TYPE_MAX)
  {
    success = TRUE;
  }

  return success;
} /* qbi_svc_bc_context_type_uuid_to_id() */

/*! @addtogroup MBIM_CID_DEVICE_CAPS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_caps_get_data_class
===========================================================================*/
/*!
    @brief Returns the DataClass reported in MBIM_CID_DEVICE_CAPABILITIES

    @details
    This reflects the data capabilites of the modem

    @param ctx

    @return uint32 MBIM_DATA_CLASS value, or zero on failure
*/
/*=========================================================================*/
uint32 qbi_svc_bc_device_caps_get_data_class
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  return cache->device_caps.data_class;
} /* qbi_svc_bc_device_caps_get_data_class() */

/*! @} */

/*===========================================================================
  FUNCTION: qbi_svc_bc_module_cache_get
===========================================================================*/
/*!
    @brief Gets a pointer to the structure containing pointers to the cache
    areas owned by the individual modules of the Basic Connectivity device
    service

    @details

    @param ctx

    @return qbi_svc_bc_module_cache_s* Pointer to module cache, or NULL on
    failure
*/
/*=========================================================================*/
qbi_svc_bc_module_cache_s *qbi_svc_bc_module_cache_get
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(cache);

  return &cache->module_cache;
} /* qbi_svc_bc_module_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_add
===========================================================================*/
/*!
    @brief Allocates a new MBIM_PROVIDER structure in the data buffer of
    the response, and populates it with the given data

    @details

    @param txn
    @param provider_field
    @param mcc MCC if cellular_class is GSM, otherwise ignored
    @param mnc MNC if cellular_class is GSM, otherwise ignored
    @param mnc_is_3_digits
    @param sid SID if cellular_class is CDMA, otherwise ignored
    @param provider_state
    @param provider_name May be NULL if provider_name_len is 0
    @param provider_name_len Length of provider_name in bytes
    @param provider_name_is_ascii Set to TRUE if provider_name is encoded
    in ASCII, and should be converted to UTF-16
    @param cellular_class Must be either QBI_SVC_BC_CELLULAR_CLASS_GSM or
    QBI_SVC_BC_CELLULAR_CLASS_CDMA - can't be binary OR of the two!
    @param rssi
    @param error_rate

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_add
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *provider_field,
  uint16                       mcc,
  uint16                       mnc,
  boolean                      mnc_is_3_digits,
  uint16                       sid,
  uint32                       provider_state,
  const void                  *provider_name,
  uint32                       provider_name_len,
  boolean                      provider_name_is_ascii,
  uint32                       cellular_class,
  uint32                       rssi,
  uint32                       error_rate
)
{
  qbi_svc_bc_provider_s *provider;
  boolean result;
  uint32 initial_size;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  provider = (qbi_svc_bc_provider_s *) qbi_txn_rsp_databuf_add_field(
    txn, provider_field, 0, sizeof(qbi_svc_bc_provider_s), NULL);
  QBI_CHECK_NULL_PTR_RET_FALSE(provider);

  initial_size = txn->infobuf_len_total;
  result = qbi_svc_bc_provider_populate(
    txn, provider, mcc, mnc, mnc_is_3_digits, sid, provider_state,
    provider_name, provider_name_len, provider_name_is_ascii, cellular_class,
    rssi, error_rate);

  /* Account for the addition of any fields added to the DataBuffer in the
     total length of the MBIM_PROVIDER structure. */
  if (result)
  {
    provider_field->size += txn->infobuf_len_total - initial_size;
  }

  return result;
} /* qbi_svc_bc_provider_add() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_id_ascii_to_mcc_mnc
===========================================================================*/
/*!
    @brief Converts an ASCII encoded 3GPP ProviderId to binary MCC and MNC

    @details
    Input is expected to be a 5 or 6 digit ProviderId per MBIM convention.

    @param provider_id_ascii
    @param provider_id_ascii_len Size of the provider_id_ascii buffer in
    bytes (must be at least QBI_SVC_BC_PROVIDER_ID_MIN_LEN_CHARS)
    @param mcc
    @param mnc
    @param mnc_is_3_digits

    @return boolean
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_id_ascii_to_mcc_mnc
(
  const char *provider_id_ascii,
  uint32      provider_id_ascii_len,
  uint16     *mcc,
  uint16     *mnc,
  boolean    *mnc_is_3_digits
)
{
  uint32 i;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(provider_id_ascii);
  QBI_CHECK_NULL_PTR_RET_FALSE(mcc);
  QBI_CHECK_NULL_PTR_RET_FALSE(mnc);
  QBI_CHECK_NULL_PTR_RET_FALSE(mnc_is_3_digits);

  if (provider_id_ascii_len < QBI_SVC_BC_PROVIDER_ID_MIN_LEN_CHARS)
  {
    QBI_LOG_E_1("Short ProviderId buffer: %d", provider_id_ascii_len);
    success = FALSE;
  }
  else
  {
    for (i = 0;
         i < MIN(provider_id_ascii_len, QBI_SVC_BC_PROVIDER_ID_MAX_LEN_CHARS);
         i++)
    {
      if (provider_id_ascii[i] == QBI_UTIL_ASCII_NULL)
      {
        if (i < QBI_SVC_BC_PROVIDER_ID_MIN_LEN_CHARS)
        {
          QBI_LOG_E_1("Short ProviderId string! Length only %d characters", i);
          success = FALSE;
        }
        break;
      }
      else if (provider_id_ascii[i] < QBI_UTIL_ASCII_NUMERIC_MIN ||
               provider_id_ascii[i] > QBI_UTIL_ASCII_NUMERIC_MAX)
      {
        QBI_LOG_E_1("Invalid character '%c' in ProviderId",
                    provider_id_ascii[i]);
        success = FALSE;
        break;
      }
    }
  }

  if (success)
  {
    *mcc = (100 * (provider_id_ascii[0] - QBI_UTIL_ASCII_NUMERIC_MIN)) +
           (10  * (provider_id_ascii[1] - QBI_UTIL_ASCII_NUMERIC_MIN)) +
           (1   * (provider_id_ascii[2] - QBI_UTIL_ASCII_NUMERIC_MIN));

    if (provider_id_ascii_len > QBI_SVC_BC_PROVIDER_ID_MIN_LEN_CHARS &&
        provider_id_ascii[5] != QBI_UTIL_ASCII_NULL)
    {
      *mnc = (100 * (provider_id_ascii[3] - QBI_UTIL_ASCII_NUMERIC_MIN)) +
             (10  * (provider_id_ascii[4] - QBI_UTIL_ASCII_NUMERIC_MIN)) +
             (1   * (provider_id_ascii[5] - QBI_UTIL_ASCII_NUMERIC_MIN));
      *mnc_is_3_digits = TRUE;
    }
    else
    {
      *mnc = (10 * (provider_id_ascii[3] - QBI_UTIL_ASCII_NUMERIC_MIN)) +
             (1  * (provider_id_ascii[4] - QBI_UTIL_ASCII_NUMERIC_MIN));
      *mnc_is_3_digits = FALSE;
    }
    QBI_LOG_I_2("Parsed ProviderId as %03d-%02d", *mcc, *mnc);
  }

  return success;
} /* qbi_svc_bc_provider_id_ascii_to_mcc_mnc() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_id_to_mcc_mnc
===========================================================================*/
/*!
    @brief Parses a MBIM ProviderId UTF-16 string into a binary MCC and
    MNC

    @details

    @param txn
    @param provider_id
    @param initial_offset
    @param mcc
    @param mnc
    @param mnc_is_3_digits

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_id_to_mcc_mnc
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *provider_id,
  uint32                             initial_offset,
  uint16                            *mcc,
  uint16                            *mnc,
  boolean                           *mnc_is_3_digits
)
{
  boolean success;
  const uint8 *provider_id_utf16;
  char provider_id_ascii[QBI_SVC_BC_PROVIDER_ID_ASCII_MAX_LEN];
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(provider_id);
  QBI_CHECK_NULL_PTR_RET_FALSE(mcc);
  QBI_CHECK_NULL_PTR_RET_FALSE(mnc);

  provider_id_utf16 = qbi_txn_req_databuf_get_field(
    txn, provider_id, initial_offset, QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES);
  if (provider_id_utf16 == NULL)
  {
    QBI_LOG_E_0("Error extracting ProviderId");
    success = FALSE;
  }
  else
  {
    /* ProviderId is 3 digit MCC (zero-pad) followed by 2 or 3 digit MNC.
       Convert it from UTF-16 to ASCII, check validity, then convert to
       binary. */
    (void) qbi_util_utf16_to_ascii(provider_id_utf16, provider_id->size,
                                   provider_id_ascii, sizeof(provider_id_ascii));
    success = qbi_svc_bc_provider_id_ascii_to_mcc_mnc(
      provider_id_ascii, sizeof(provider_id_ascii), mcc, mnc, mnc_is_3_digits);
  }

  return success;
} /* qbi_svc_bc_provider_id_to_mcc_mnc() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_provider_populate
===========================================================================*/
/*!
    @brief Populates a MBIM_PROVIDER data structure with the given data

    @details
    Handles initial_offset appropriately whether the MBIM_PROVIDER is
    nested in a MBIM_PROVIDERS list or not. Performs DataBuffer
    consolidation when complete.

    @param txn
    @param provider
    @param mcc MCC if cellular_class is GSM, otherwise ignored
    @param mnc MNC if cellular_class is GSM, otherwise ignored
    @param mnc_is_3_digits
    @param sid SID if cellular_class is CDMA, otherwise ignored
    @param provider_state
    @param provider_name May be NULL if provider_name_len is 0
    @param provider_name_len Length of provider_name in bytes
    @param provider_name_is_ascii Set to TRUE if provider_name is encoded
    in ASCII, and should be converted to UTF-16
    @param cellular_class Must be either QBI_SVC_BC_CELLULAR_CLASS_GSM or
    QBI_SVC_BC_CELLULAR_CLASS_CDMA - can't be binary OR of the two!
    @param rssi
    @param error_rate

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_bc_provider_populate
(
  qbi_txn_s             *txn,
  qbi_svc_bc_provider_s *provider,
  uint16                 mcc,
  uint16                 mnc,
  boolean                mnc_is_3_digits,
  uint16                 sid,
  uint32                 provider_state,
  const void            *provider_name,
  uint32                 provider_name_len,
  boolean                provider_name_is_ascii,
  uint32                 cellular_class,
  uint32                 rssi,
  uint32                 error_rate
)
{
  boolean success = FALSE;
  uint32 initial_offset;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(provider);

  initial_offset = txn->infobuf_len_total - sizeof(qbi_svc_bc_provider_s);
  if (cellular_class != QBI_SVC_BC_CELLULAR_CLASS_GSM &&
      cellular_class != QBI_SVC_BC_CELLULAR_CLASS_CDMA)
  {
    QBI_LOG_E_1("Invalid CellularClass %d", cellular_class);
  }
  else if (cellular_class == QBI_SVC_BC_CELLULAR_CLASS_GSM &&
           !qbi_svc_bc_provider_append_3gpp_provider_id(
             txn, &provider->provider_id, initial_offset, mcc, mnc,
             mnc_is_3_digits))
  {
    QBI_LOG_E_0("Couldn't append 3GPP ProviderId!");
  }
  else if (cellular_class == QBI_SVC_BC_CELLULAR_CLASS_CDMA &&
           !qbi_svc_bc_provider_append_3gpp2_provider_id(
             txn, &provider->provider_id, initial_offset, sid))
  {
    QBI_LOG_E_0("Couldn't append 3GPP2 ProviderId!");
  }
  else if (provider_name_len > 0 && provider_name_is_ascii &&
           !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
             txn, &provider->provider_name, initial_offset,
             QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES,
             provider_name, provider_name_len))
  {
    QBI_LOG_E_0("Couldn't populate ASCII ProviderName!");
  }
  else if (provider_name_len > 0 && !provider_name_is_ascii &&
           !qbi_txn_rsp_databuf_add_field(
             txn, &provider->provider_name, initial_offset,
             provider_name_len, provider_name))
  {
    QBI_LOG_E_0("Couldn't populate UTF-16 ProviderName!");
  }
  else
  {
    provider->provider_state = provider_state;
    provider->cellular_class = cellular_class;
    provider->rssi = rssi;
    provider->error_rate = error_rate;
    success = qbi_txn_rsp_databuf_consolidate(txn);
  }

  return success;
} /* qbi_svc_bc_provider_populate() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_qmi_plmn_name_to_provider_name
===========================================================================*/
/*!
    @brief Converts the information returned by QMI_NAS_GET_PLMN_NAME into a
    UTF-16 encoded ProviderName

    @details

    @param nas44_rsp
    @param provider_name_utf16 Buffer which will be populated with the
    ProviderName
    @param provider_name_utf16_len Size of the provider_name_utf16 buffer in
    bytes
    @param name_pref Preferred network name source

    @return uint32 Number of bytes set in provider_name_utf16
*/
/*=========================================================================*/
uint32 qbi_svc_bc_qmi_plmn_name_to_provider_name
(
  const nas_get_plmn_name_resp_msg_v01 *nas44_rsp,
  uint8                                *provider_name_utf16,
  uint32                                provider_name_utf16_len,
  qbi_svc_bc_provider_name_pref_e       name_pref
)
{
  boolean use_spn;
  int32 short_name_threshold;
  uint32 bytes_copied = 0;
  boolean plmn_name_decoded;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(nas44_rsp);
  QBI_CHECK_NULL_PTR_RET_ZERO(provider_name_utf16);

  /* If requested and the conditions are right, combine both PLMN name and SPN
     as "PLMN_Name (SPN)" */
  if (name_pref == QBI_SVC_BC_PROVIDER_NAME_PREF_REGISTERED_WITH_CONCAT &&
      qbi_svc_bc_qmi_plmn_name_to_provider_name_should_concat(nas44_rsp))
  {
    /* Prefer the short PLMN name if it means we won't truncate the SPN. The
       threshold is (MAX_LEN - SEPARATOR_LEN - SPN_LEN) characters. */
    short_name_threshold = (QBI_SVC_BC_PROVIDER_NAME_ASCII_MAX_LEN - 1 -
                            QBI_SVC_BC_CONCAT_PLMN_SPN_ADDL_CHARS);
    if (nas44_rsp->eons_plmn_name_3gpp.spn_enc ==
          NAS_CODING_SCHEME_CELL_BROADCAST_GSM_V01)
    {
      short_name_threshold -= nas44_rsp->eons_plmn_name_3gpp.spn_len;
    }
    else /* Assumed: UCS2 encoding */
    {
      short_name_threshold -= (nas44_rsp->eons_plmn_name_3gpp.spn_len / 2);
    }
    if (short_name_threshold < 1)
    {
      short_name_threshold = 1;
    }

    /* We want PLMN name to be first, so set use_spn to FALSE */
    bytes_copied = qbi_svc_bc_qmi_plmn_name_to_provider_name_decode(
      &nas44_rsp->eons_plmn_name_3gpp, provider_name_utf16,
      provider_name_utf16_len, FALSE, (uint32) short_name_threshold);

    /* Only attempt to append the SPN if there is enough room for at least the
       first two separator characters and one character of the SPN. */
    if (bytes_copied <= (provider_name_utf16_len -
                         (QBI_SVC_BC_CONCAT_PLMN_SPN_ADDL_CHARS * 2)))
    {
      plmn_name_decoded = (bytes_copied > 0) ? TRUE : FALSE;
      if (plmn_name_decoded)
      {
        provider_name_utf16[bytes_copied++] = ' ';
        provider_name_utf16[bytes_copied++] = 0x00;
        provider_name_utf16[bytes_copied++] = '(';
        provider_name_utf16[bytes_copied++] = 0x00;
      }
      bytes_copied += qbi_svc_bc_qmi_plmn_name_to_provider_name_decode(
        &nas44_rsp->eons_plmn_name_3gpp, (provider_name_utf16 + bytes_copied),
        (provider_name_utf16_len - bytes_copied), TRUE, 0);
      if (plmn_name_decoded && bytes_copied <= (provider_name_utf16_len - 2))
      {
        provider_name_utf16[bytes_copied++] = ')';
        provider_name_utf16[bytes_copied++] = 0x00;
      }
    }
  }
  else
  {
    if (name_pref == QBI_SVC_BC_PROVIDER_NAME_PREF_REGISTERED ||
        name_pref == QBI_SVC_BC_PROVIDER_NAME_PREF_REGISTERED_WITH_CONCAT)
    {
      /* Legacy/non-concatenating behavior: Allow use of SPN unless we are in a
         non-home network, SPN display bit is set (meaning SPN display not
         required in non-home network), and PLMN name is available. */
      if (nas44_rsp->is_home_network_valid &&
          nas44_rsp->eons_display_bit_info_valid &&
          nas44_rsp->is_home_network == NAS_TRI_FALSE_V01 &&
          nas44_rsp->eons_display_bit_info.is_spn_set == NAS_TRI_TRUE_V01 &&
          (nas44_rsp->eons_plmn_name_3gpp.plmn_long_name_len > 0 ||
           nas44_rsp->eons_plmn_name_3gpp.plmn_short_name_len > 0))
      {
        use_spn = FALSE;
      }
      else
      {
        use_spn = TRUE;
      }
    }
    else
    {
      /* For network name queries outside of REGISTER_STATE, is_home_network is
         not relevant (since it applies to the RPLMN rather than the queried
         PLMN) so we explicitly define whether SPN or PLMN name is preferred */
      use_spn = (name_pref == QBI_SVC_BC_PROVIDER_NAME_PREF_SPN) ? TRUE : FALSE;
    }
    bytes_copied = qbi_svc_bc_qmi_plmn_name_to_provider_name_decode(
      &nas44_rsp->eons_plmn_name_3gpp, provider_name_utf16,
      provider_name_utf16_len, use_spn, 0);
  }

  return bytes_copied;
} /* qbi_svc_bc_qmi_plmn_name_to_provider_name() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_radio_state_is_radio_on
===========================================================================*/
/*!
    @brief Checks the effective state of the radio

    @details

    @param ctx

    @return boolean TRUE if both SW & HW radio switches are ON, FALSE
    otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_radio_state_is_radio_on
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  return (cache->radio_state.sw_radio_state ==
            QBI_SVC_BC_RADIO_STATE_RADIO_ON &&
          cache->radio_state.hw_radio_state ==
            QBI_SVC_BC_RADIO_STATE_RADIO_ON);
} /* qbi_svc_bc_radio_state_is_radio_on() */


/*=============================================================================

  Public Function Definitions, exposed in qbi_svc_bc.h

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_deactivate
===========================================================================*/
/*!
    @brief Builds a QMI_WDS_STOP_NETWORK_INTERFACE_REQ to tear down the data
    call with the given handle

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_connect_s_deactivate
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_connect_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint32 activation_state;
  qbi_qmi_svc_e wds_svc_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_connect_s_req_s *) txn->req.data;
  /* Before dispatching the stop network interface request, query the packet
     statistics one last time to get the totals for the session. */
  QBI_LOG_D_1("Triggering deactivate on session id %d",req->session_id);
  qbi_svc_bc_packet_statistics_sync_current_session(txn->ctx);

  /* Set the completion callback to resync the cache in the event of an
     unexpected error */
  qbi_txn_set_completion_cb(txn, qbi_svc_bc_connect_s_deactivate_completion_cb);

  /* Build QMI request(s) to disconnect all activated connections, and update
     the cache with the new DEACTIVATING state. If already in DEACTIVATING
     state, wait until deactivation finishes by monitoring
     QMI_WDS_PKT_SRVC_STATUS_IND. */
  for (wds_svc_id = QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV4(req->session_id);
       wds_svc_id <= QBI_SVC_BC_SESSION_ID_TO_WDS_SVC_ID_IPV6(req->session_id);
       wds_svc_id++)
  {
    activation_state = qbi_svc_bc_connect_get_ip_type_activation_state(
      txn->ctx, wds_svc_id);
    if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
    {
      action = qbi_svc_bc_connect_build_wds21_req(
        txn, wds_svc_id, qbi_svc_bc_connect_s_wds21_rsp_cb);
      if (action != QBI_SVC_ACTION_SEND_QMI_REQ)
      {
        break;
      }
    }
    else if (activation_state == QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING)
    {
      if (!qbi_svc_ind_reg_dynamic(
            txn->ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_CONNECT,
            wds_svc_id, QMI_WDS_PKT_SRVC_STATUS_IND_V01,
            qbi_svc_bc_connect_s_deactivate_wds22_ind_cb,
            txn, NULL))
      {
        QBI_LOG_E_0("Couldn't register dynamic indication handler!");
        action = QBI_SVC_ACTION_ABORT;
      }
      else
      {
        if (action != QBI_SVC_ACTION_SEND_QMI_REQ)
        {
          action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
        }
      }
    }
  }

  if (action == QBI_SVC_ACTION_SEND_QMI_REQ)
  {
    /* Notify the host that we have changed to the DEACTIVATING state */
    qbi_svc_bc_connect_s_force_event(txn->ctx, req->session_id);
  }

  return action;
} /* qbi_svc_bc_connect_s_deactivate() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_protocol
===========================================================================*/
/*!
    @brief Converts MBIM_CID_AUTH_PROTOCOL to QMI authentication protocol

    @details

    @param auth_protocol

    @return wds_profile_auth_protocol_enum_v01
*/
/*=========================================================================*/
wds_profile_auth_protocol_enum_v01 qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_protocol
(
  uint32 auth_protocol
)
{
  wds_profile_auth_protocol_enum_v01 qmi_auth_protocol;
/*-------------------------------------------------------------------------*/
  switch (auth_protocol)
  {
    case QBI_SVC_BC_AUTH_PROTOCOL_NONE:
      qmi_auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_NONE_V01;
      break;

    case QBI_SVC_BC_AUTH_PROTOCOL_PAP:
      qmi_auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_PAP_V01;
      break;

    case QBI_SVC_BC_AUTH_PROTOCOL_CHAP:
    case QBI_SVC_BC_AUTH_PROTOCOL_MSCHAP_V2:
      qmi_auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_CHAP_V01;
      break;

    default:
      QBI_LOG_E_1("Unrecognized Authentitication Protocol %d - using PAP or "
                  "CHAP", auth_protocol);
      /* May try PAP or CHAP */
      qmi_auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01;
  }

  return qmi_auth_protocol;
} /* qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_protocol() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_pref
===========================================================================*/
/*!
    @brief Converts MBIM_CID_AUTH_PROTOCOL to QMI authentication preference
    (3GPP only)

    @details

    @param auth_protocol

    @return uint8
*/
/*=========================================================================*/
wds_auth_pref_mask_v01 qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_pref
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

    default:
      QBI_LOG_E_1("Unrecognized AuthentiticationProtocol %d - using PAP or "
                  "CHAP", auth_protocol);
      /* May try PAP or CHAP */
      qmi_auth_pref = (QMI_WDS_MASK_AUTH_PREF_PAP_V01 |
                       QMI_WDS_MASK_AUTH_PREF_CHAP_V01);
  }

  return qmi_auth_pref;
} /* qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_pref() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_combine_activation_states
===========================================================================*/
/*!
    @brief Uses a lookup table to combine the activation state of IPV4 and
    IPV6 instances into a single effective activation state

    @details

    @param ipv4_activation_state MBIM activation state of the IPv4 instance
    @param ipv6_activation_state MBIM activation state of the IPv6 instance
    @param ipv4_and_v6 Set to TRUE if the requested IP type is IPv4AndIPv6

    @return uint32 Effective MBIM activation state
*/
/*=========================================================================*/
uint32 qbi_svc_bc_connect_combine_activation_states
(
  uint32  ipv4_activation_state,
  uint32  ipv6_activation_state,
  boolean ipv4_and_v6
)
{
  /* General rules for when states don't match (in priority order): if any is
     activating, then overall is activating, if any is deactivating, then
     overall is deactivating, if one is activated and the other is deactivated,
     then is activated (v4 or v6) or deactivating (v4 and v6). */
  static const qbi_svc_bc_activation_mapping_s mapping_ipv4_or_v6[] = {
    /* v4 Activated */
    {QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED,
      (QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED |
       QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED),
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING},

    /* v4 Activating */
    {QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_BM_ANY,
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING},

    /* v4 Deactivated - use v6 state */
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED},

    /* v4 Deactivating */
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING,
      (QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED |
       QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED |
       QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING),
      QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING}
  };

  static const qbi_svc_bc_activation_mapping_s mapping_ipv4_and_v6[] = {
    /* v4 Activated */
    {QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED,
      (QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED |
       QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING),
      QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING},

    /* v4 Activating */
    {QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_BM_ANY,
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING},

    /* v4 Deactivated */
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED,
      QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED,
      (QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED |
       QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING),
      QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING},

    /* v4 Deactivating */
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATING,
      QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING},
    {QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING,
      (QBI_SVC_BC_ACTIVATION_STATE_BM_ACTIVATED |
       QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATING |
       QBI_SVC_BC_ACTIVATION_STATE_BM_DEACTIVATED),
      QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING},
  };

  uint32 effective_activation_state = QBI_SVC_BC_ACTIVATION_STATE_UNKNOWN;
  uint8 index;
  uint8 size;
  const qbi_svc_bc_activation_mapping_s *p_map;
/*-------------------------------------------------------------------------*/
  if (ipv4_and_v6)
  {
    size = ARR_SIZE(mapping_ipv4_and_v6);
    p_map = mapping_ipv4_and_v6;
  }
  else
  {
    size = ARR_SIZE(mapping_ipv4_or_v6);
    p_map = mapping_ipv4_or_v6;
  }

  for (index = 0; index < size; index++)
  {
    if ((p_map[index].bm_ipv4 & (1 << ipv4_activation_state)) &&
        (p_map[index].bm_ipv6 & (1 << ipv6_activation_state)))
    {
      effective_activation_state = p_map[index].activation_state;
      QBI_LOG_D_4("Effective activation state %d: ipv4 %d ipv6 %d both "
                  "requested %d", effective_activation_state,
                  ipv4_activation_state, ipv6_activation_state, ipv4_and_v6);
      break;
    }
  }

  return effective_activation_state;
} /* qbi_svc_bc_connect_combine_activation_states() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity device service's
    cache

    @details

    @param ctx

    @return qbi_svc_bc_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
qbi_svc_bc_cache_s *qbi_svc_bc_cache_get
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_bc_cache_s *) qbi_svc_cache_get(ctx, QBI_SVC_ID_BC);
} /* qbi_svc_bc_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_check_device_state
===========================================================================*/
/*!
    @brief Checks the current device state in the cache to see if an action
    is allowed

    @details
    Sets the status field of the transaction to the applicable status code
    in the event the action is not allowed, e.g.
    QBI_MBIM_STATUS_RADIO_POWER_OFF.

    @param txn
    @param action_requires_ready_state_init Set to TRUE if the action
    requires the device to be in the Initialized ReadyState (e.g. device
    can't be locked, not activated, etc.). Note that even if this is FALSE,
    ReadyStateNotInitialized will result in a failure
    @param action_requires_radio

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_check_device_state
(
  qbi_txn_s *txn,
  boolean    action_requires_ready_state_init,
  boolean    action_requires_radio
)
{
  qbi_svc_bc_cache_s *cache;
  boolean all_checks_passed = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (!qbi_svc_bc_sim_subscriber_ready_status_is_ready(
        txn, action_requires_ready_state_init))
  {
    all_checks_passed = FALSE;
  }
  else if (action_requires_radio &&
           !qbi_svc_bc_radio_state_is_radio_on(txn->ctx))
  {
    QBI_LOG_E_2("Action can't be performed - radio is off (Sw %d Hw %d)",
                cache->radio_state.sw_radio_state,
                cache->radio_state.hw_radio_state);
    txn->status = QBI_MBIM_STATUS_RADIO_POWER_OFF;
    all_checks_passed = FALSE;
  }

  return all_checks_passed;
} /* qbi_svc_bc_check_device_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_is_in_ftm
===========================================================================*/
/*!
    @brief Checks whether the modem is in Field Test Mode (FTM)

    @details

    @param ctx

    @return boolean TRUE if device is in FTM, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_device_is_in_ftm
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_bc_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  return cache->radio_state.ftm_enabled;
} /* qbi_svc_bc_device_is_in_ftm() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_supports_3gpp2
===========================================================================*/
/*!
    @brief Check if device supports 3gpp2 from cache

    @param qbi_ctx_s

    @return boolean TRUE if 3gpp2 is supported, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_device_supports_3gpp2
(
  const qbi_ctx_s *ctx
)
{
  const qbi_svc_bc_cache_s *cache;
  boolean supported = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (cache->device_caps.data_class & QBI_SVC_BC_DATA_CLASS_3GPP2_FAMILY)
  {
    supported = TRUE;
  }

  return supported;
} /* qbi_svc_bc_device_supports_3gpp2() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_init
===========================================================================*/
/*!
    @brief One-time initialization of the Basic Connectivity service

    @details

*/
/*=========================================================================*/
void qbi_svc_bc_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_bc_cfg = {
    {
      0xa2, 0x89, 0xcc, 0x33, 0xbc, 0xbb, 0x8b, 0x4f,
      0xb6, 0xb0, 0x13, 0x3e, 0xc2, 0xaa, 0xe6, 0xdf
    },
    QBI_SVC_ID_BC,
    FALSE,
    qbi_svc_bc_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_bc_cmd_hdlr_tbl),
    qbi_svc_bc_open,
    qbi_svc_bc_close,
    qbi_svc_bc_qmi_reg_tbl,
    ARR_SIZE(qbi_svc_bc_qmi_reg_tbl)
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_bc_cfg);
} /* qbi_svc_bc_init() */

