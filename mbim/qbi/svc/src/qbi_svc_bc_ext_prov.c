/*!
  @file
  qbi_svc_bc_ext_prov.c

  @brief
  Basic Connectivity Extension device service definitions, based on PROVISIONING
  CONTEXT feature requirements.
*/

/*=============================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
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
10/27/17  nk   Moved provision context features from bc_ext.c
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_bc_ext.h"
#include "qbi_svc_bc_ext_prov.h"
#include "qbi_svc_bc_ext_lte.h"
#include "qbi_svc_bc_ext_mbim.h"
#include "qbi_svc_bc_ext_dssa.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_msuicc.h"
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
#include "qbi_svc_bc_sim.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/* Provision context_v2 */
#define PROV_V2_IND_TOKEN 0x1234

/*! Map QMI WDS service ID to session ID */
#define QBI_SVC_BC_EXT_INVALID_PROFILE_INDEX               (0)

/*! Number of APN 's Rows */
#define QBI_SVC_BC_EXT_OPERATOR_APN_ROW 4
/*! Number of APN 's Columns */
#define QBI_SVC_BC_EXT_OPERATOR_APN_COL 2

/*! Time we wait for a SIM State to get initiliased  (milliseconds) */
#define QBI_SVC_BC_EXT_PROV_V2_TIMEOUT_MS (5000)

/*! MCFG supports only one logical slot 0. Mapping from two physical
  slots (0, 1) to one logical slot 0 is handled by UIM So from MCFG
  perspective there only one logical slot 0 */
#define QBI_SVC_BC_EXT_PROV_V2_UIM_SLOT_INDEX (0)
/*=============================================================================

  Private Typedefs

=============================================================================*/

static boolean cmd_in_progress_ignore_indication = FALSE;

/*! IMSI range check for operator specific customization */
#define QBI_SVC_BC_EXT_IMSI_311_480 "311480"
#define QBI_SVC_BC_EXT_IMSI_311_270 "311270"
#define QBI_SVC_BC_EXT_IMSI_312_770 "312770"

/*! IMSI Range max length to be compared */
#define QBI_SVC_BC_EXT_IMSI_311_480_MAX_LEN 6
#define QBI_SVC_BC_EXT_IMSI_311_270_MAX_LEN 6
#define QBI_SVC_BC_EXT_IMSI_312_770_MAX_LEN 6


/*! @addtogroup MBIM_CID_MS_SLOT_INFO_STATUS
    @{ */

static uint32_t active_config_id_len;
static uint8_t active_config_id[PDC_CONFIG_ID_SIZE_MAX_V01];

/*! @addtogroup MBIM_CID_MS_PROVISIONED_CONTEXT_V2
    @{ */

/* APN Listed Class wise for specific operator
   APN names in each row are synonymous as per operator requirement */
static const char* qbi_svc_bc_ext_provisioned_contexts_v2_operator_apn
[QBI_SVC_BC_EXT_OPERATOR_APN_ROW][QBI_SVC_BC_EXT_OPERATOR_APN_COL] =
{
  {"vzwims", "ims"  },   /*! Class 1 */
  {"vzwadmin", ""   },   /*! Class 2 */
  {"vzwinternet", ""},   /*! Class 3 */
  {"vzwapp" , ""     }    /*! Class 4 */
};

/*! @brief Profile list containing basic profile info */
typedef struct
{
  uint32                    num_of_profile;
  wds_profile_type_enum_v01 profile_type[QMI_WDS_PROFILE_LIST_MAX_V01];
  uint8_t                   profile_index[QMI_WDS_PROFILE_LIST_MAX_V01];
} qbi_svc_bc_ext_prov_ctx_profile_list_s;

/*! Collection of 3gpp settings used for finding matching 3gpp2 profile */
typedef struct
{
  uint8_t apn_name_valid;
  char    apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  uint8_t username_valid;
  char    username[QMI_WDS_USER_NAME_MAX_V01 + 1];
} qbi_svc_bc_ext_prov_ctx_v2_3gpp_profile_settings_s;

/*! Tracking information for retrieving profiles */
typedef struct
{
  /*! Number of profiles that have been retrieved so far */
  uint32 profiles_read;

  /*! Store EPC profile scan status and matching profile index */
  uint8 profile_found_epc;
  uint8 profile_index_epc;

  /*! Store 3gpp profile scan status and matching profile index */
  uint8 profile_found_3gpp;
  uint8 profile_index_3gpp;

  /*! Store 3gpp2 profile scan status and matching profile index */
  uint32 profile_found_3gpp2;
  uint32 profile_index_3gpp2;

  /*! Store number of profiles from profile list */
  uint32 num_of_profile_epc;
  int32 num_of_profile_3gpp;
  uint32 num_of_profile_3gpp2;

  /*! 3gpp profile settings for finding matching 3gpp2 profile */
  qbi_svc_bc_ext_prov_ctx_v2_3gpp_profile_settings_s profile_settings_3gpp;

  /*! Buffer to store profile index info from last QMI profile list query */
  qbi_svc_bc_ext_prov_ctx_profile_list_s profile_list;

  /* This is used to keep track whether any matching profile was found while in set request */
  boolean profile_matched;

  /* This is used to save the profile index for the matched request while in set request */
  uint8_t matched_index;

  /* This keeps track whether the requested operation has been completed */
  boolean operation_completed;
} qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s;

/*! Collection of pointers to relevant TLVs in
    QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ and QMI_WDS_CREATE_PROFILE_REQ */
typedef struct
{
  uint8_t *apn_name_valid;
  char *apn_name;
  uint8_t *user_id_valid;
  char *user_id;
  uint8_t *auth_password_valid;
  char *auth_password;
  uint8_t *auth_protocol_valid;
  wds_profile_auth_protocol_enum_v01 *auth_protocol;
  uint8_t *authentication_preference_valid;
  wds_auth_pref_mask_v01 *authentication_preference;
  uint8_t *app_user_data_valid;
  uint32_t *app_user_data;
  uint8_t *pdp_data_compression_type_valid;
  wds_pdp_data_compr_type_enum_v01 *pdp_data_compression_type;
  uint8_t *pdp_hdr_compression_type_valid;
  wds_pdp_hdr_compr_type_enum_v01 *pdp_hdr_compression_type;
  uint8_t *common_apn_disabled_flag_valid;
  uint8_t *common_apn_disabled_flag;
  uint8_t *apn_disabled_flag_valid;
  uint8_t *apn_disabled_flag;
  uint8_t *apn_enabled_3gpp2_valid;
  uint8_t *apn_enabled_3gpp2;
  uint8_t *common_pdp_type_valid;
  wds_common_pdp_type_enum_v01 *common_pdp_type;
  uint8_t *pdp_type_valid;
  wds_pdp_type_enum_v01 *pdp_type;
} qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s;

/*! @} */

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

/*! @addtogroup MBIM_CID_MS_PROVISIONED_CONTEXT_V2
    @{ */

static void qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status
(
  qbi_txn_s *txn,
  qmi_error_type_v01                  qmi_error,
  uint8_t                             qmi_error_ds_ext_valid,
  wds_ds_extended_error_code_enum_v01 qmi_error_ds_ext
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_wds29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static wds_profile_auth_protocol_enum_v01 qbi_svc_bc_ext_prov_ctx_connect_mbim_auth_pref_to_qmi_auth_protocol
(
  uint32 auth_protocol
);

static boolean qbi_svc_bc_ext_prov_ctx_populate_profile_list
(
  qbi_svc_bc_ext_prov_ctx_profile_list_s     *profile_list,
  wds_get_profile_list_resp_msg_v01          *qmi_rsp
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_q_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_q_get_next_profile
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_q_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_ext_prov_ctx_v2_q_add_context_to_rsp
(
  qbi_txn_s                                   *txn,
  qbi_mbim_offset_size_pair_s                 *field_desc,
  wds_profile_type_enum_v01                    profile_type,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp,
  uint32                                       context_id
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_build_wds2a_req
(
  qbi_txn_s                *txn,
  wds_profile_type_enum_v01 profile_type
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req
(
  qbi_txn_s                *txn,
  wds_profile_type_enum_v01 profile_type,
  uint32                    index
);

static const uint8* qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_context_type
(
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_compare_3gpp_3gpp2_profiles
(
  qbi_svc_bc_ext_prov_ctx_v2_3gpp_profile_settings_s             *profile_settings_3gpp,
  wds_get_profile_settings_resp_msg_v01                          *qmi_rsp_3gpp2
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_3gpp2_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_get_next_3gpp2_profile
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_get_next_profile
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_name_matched
(
  qbi_qmi_txn_s                         *qmi_txn,
  wds_get_profile_settings_resp_msg_v01 *profile_settings
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_wds28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile
(
  qbi_txn_s                                             *txn,
  wds_profile_type_enum_v01                              profile_type,
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s    *profile_settings
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_populate_str
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *field_desc,
  uint32                             field_max_size,
  char                              *qmi_field,
  uint32                             qmi_field_size
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds28
(
  wds_modify_profile_settings_req_msg_v01              *qmi_req,
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s   *profile_settings,
  wds_profile_type_enum_v01                             profile_type
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_wds27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_build_wds27_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds27
(
  wds_create_profile_req_msg_v01                       *qmi_req,
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s   *profile_settings,
  wds_profile_type_enum_v01                             profile_type
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile_compression
(
  const qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s   *req,
  wds_profile_type_enum_v01                               profile_type,
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s     *profile_settings
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile_auth_protocol
(
  qbi_txn_s                                                      *txn,
  const qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s           *req,
  wds_profile_type_enum_v01                                       profile_type,
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s             *profile_settings
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_wds29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_is_pdp_type_matched
(
  qbi_qmi_txn_s                         *qmi_txn,
  wds_get_profile_settings_resp_msg_v01 *profile_settings
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_wdsa8_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_pdc20_res
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc23_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_factory_reset
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1
(
  qbi_ctx_s  *ctx,
  const char *apn_name
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class2
(
  const char *apn_name
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator
(
  const char *apn_name
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_pdc_get_next_profile
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_ext_prov_ctx_v2_s_register_for_pdc_ind
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_wds29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds29_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc_prep_wds29_req
(
  qbi_txn_s *txn
);

/*=============================================================================

Private Function Definitions

=============================================================================*/

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_prov_ctx_get_user_defined_profile_from_cache
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity Extension device
    service's cache

    @details

    @param ctx

    @return Profile index
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_ext_prov_ctx_get_user_defined_profile_from_cache
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_bc_ext_module_prov_cache_s  *cache = NULL;
  qbi_svc_bc_spdp_cache_s cache_spdp  = { 0 };
  uint32                  cache_index = 0;
/*-------------------------------------------------------------------------*/
  while (QMI_WDS_PROFILE_LIST_MAX_V01 > cache_index)
  {
    cache = (qbi_svc_bc_ext_module_prov_cache_s *)
        qbi_svc_bc_ext_module_prov_cache_get(ctx, cache_index);
    if (cache == NULL)
    {
      QBI_LOG_E_0("Cache is empty !!");
      return FALSE;
    }

    qbi_svc_bc_spdp_read_nv_store(ctx, &cache_spdp);

    if (cache != NULL &&
        (cache->prov_active == TRUE || cache->lte_active == TRUE) &&
        cache->context_flag == QBI_SVC_BC_EXT_CONTEXT_FLAG_USER_DEFINED &&
        (cache->lte_active && QBI_SVC_BC_SPDP_OPERATOR_NONE == cache_spdp.spdp_support_flag) &&
        (cache->source == QBI_SVC_MBIM_MS_CONTEXT_SOURCE_USER ||
         cache->source == QBI_SVC_MBIM_MS_CONTEXT_SOURCE_OPERATOR ||
         cache->source == QBI_SVC_MBIM_MS_CONTEXT_SOURCE_DEVICE ||
         cache->source == QBI_SVC_MBIM_MS_CONTEXT_SOURCE_ADMIN))
    {
      QBI_LOG_I_1("Found user defined profile at index %d", cache_index);
      break;
    }
    cache_index++;
  }
  return cache_index;
} /* qbi_svc_bc_ext_prov_ctx_get_user_defined_profile_from_cache() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_clear_cache_profiles
===========================================================================*/
/*!
    @brief Handles a QMI_PDC_REFRESH_IND_V01

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_clear_cache_profiles
(
  qbi_txn_s *txn,
  uint32    slot_id
)
{
  qbi_svc_action_e                    action   = QBI_SVC_ACTION_ABORT;
  wds_delete_profile_req_msg_v01     *qmi_req  = NULL;
  uint32                             *info     = NULL;
  uint32                         profile_index = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  txn->info = QBI_MEM_MALLOC_CLEAR(sizeof(uint32));
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
  info = (uint32 *)txn->info;

  // delete user defined profiles and reset cache
  profile_index =
      qbi_svc_bc_ext_prov_ctx_get_user_defined_profile_from_cache(txn->ctx);

  if (QBI_SVC_BC_EXT_INVALID_PROFILE_INDEX != profile_index)
  {
    qmi_req = (wds_delete_profile_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_DELETE_PROFILE_REQ_V01,
        qbi_svc_bc_ext_prov_ctx_wds29_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->profile.profile_index = profile_index;
    *info = profile_index;
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else
  {
    action = qbi_svc_bc_ext_module_prov_cache_clear(txn);

    if (action != QBI_SVC_ACTION_ABORT)
    {
      action = qbi_svc_bc_ext_prov_ctx_v2_q_req(txn);
      QBI_LOG_D_1("ext_clear_cache_profiles %d", action);
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_clear_cache_profiles() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_wds29_rsp_cb
===========================================================================*/
/*!
    @brief Populates QMI profile settings for a MBIM_CID_PROVISIONED_CONTEXTS
    set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e QBI_SVC_ACTION_SEND_QMI_REQ on success,
    QBI_SVC_ACTION_ABORT on failure
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_wds29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_delete_profile_resp_msg_v01 *qmi_rsp       = NULL;
  qbi_svc_bc_ext_module_prov_cache_s  *cache     = NULL;
  qbi_svc_action_e                action         = QBI_SVC_ACTION_ABORT;
  uint32                          *profile_index = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  qmi_rsp = (wds_delete_profile_resp_msg_v01 *)qmi_txn->rsp.data;
  if ((qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01) &&
      (qmi_rsp->resp.error == QMI_ERR_EXTENDED_INTERNAL_V01) &&
      (qmi_rsp->extended_error_code_valid && qmi_rsp->extended_error_code ==
           WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE_V01))
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid,
        qmi_rsp->extended_error_code);
  }

  profile_index = (uint32 *)qmi_txn->parent->info;
  cache = (qbi_svc_bc_ext_module_prov_cache_s *)
      qbi_svc_bc_ext_module_prov_cache_get(qmi_txn->ctx, *profile_index);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  cache->prov_active = FALSE;
  cache->context_flag = QBI_SVC_BC_EXT_CONTEXT_FLAG_MIN;
  cache->lte_active = FALSE;

  // TODO: slot ID hardcoding to be worked out
  action = qbi_svc_bc_ext_prov_ctx_clear_cache_profiles(qmi_txn->parent, 0);

  return action;
} /* qbi_svc_bc_ext_prov_ctx_wds29_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_connect_mbim_auth_pref_to_qmi_auth_protocol
===========================================================================*/
/*!
  @brief Converts MBIM_CID_AUTH_PROTOCOL to QMI authentication protocol

  @details

  @param auth_protocol

  @return wds_profile_auth_protocol_enum_v01
*/
/*=========================================================================*/
static wds_profile_auth_protocol_enum_v01 qbi_svc_bc_ext_prov_ctx_connect_mbim_auth_pref_to_qmi_auth_protocol
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

  case QBI_SVC_BC_AUTH_PROTOCOL_AUTO:
    qmi_auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01;
    break;

  default:
    QBI_LOG_E_1("Unrecognized Authentitication Protocol %d - using PAP or "
                "CHAP", auth_protocol);
    /* May try PAP or CHAP */
    qmi_auth_protocol = WDS_PROFILE_AUTH_PROTOCOL_PAP_CHAP_V01;
  }

  return qmi_auth_protocol;
} /* qbi_svc_bc_ext_prov_ctx_connect_mbim_auth_pref_to_qmi_auth_protocol() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_read_operator_nv
===========================================================================*/
/*!
    @brief Reads operator_config from NV

    @details

    @param ctx
    @param operator_config

    @return void
*/
/*=========================================================================*/
void qbi_svc_bc_ext_prov_ctx_read_operator_nv
(
  qbi_ctx_s               *ctx,
  qbi_svc_bc_ext_operator_config_s *operator_cfg
)
{
/*-------------------------------------------------------------------------*/
  if (!qbi_nv_store_cfg_item_read(
    ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
    operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s)))
  {
    QBI_LOG_E_0("Prov Contxt V2:: E:Couldn't read profile data from NV!");
  }
} /* qbi_svc_bc_ext_prov_ctx_read_operator_nv() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_compare_imsi_for_operator
===========================================================================*/
/*!
    @brief Compares I/P imsi with that of a particular operator

    @details

    @param imsi to be matched

    @return TRUE is match found else FALSE
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_prov_ctx_compare_imsi_for_operator
(
  const char *imsi
)
{
/*-------------------------------------------------------------------------*/
  if ((!QBI_STRNCMP(imsi, QBI_SVC_BC_EXT_IMSI_311_480,
                    sizeof(char) * QBI_SVC_BC_EXT_IMSI_311_480_MAX_LEN)) ||
      (!QBI_STRNCMP(imsi, QBI_SVC_BC_EXT_IMSI_311_270,
                    sizeof(char) * QBI_SVC_BC_EXT_IMSI_311_270_MAX_LEN)) ||
      (!QBI_STRNCMP(imsi, QBI_SVC_BC_EXT_IMSI_312_770,
                    sizeof(char) * QBI_SVC_BC_EXT_IMSI_312_770_MAX_LEN)))
  {
    return TRUE;
  }

  return FALSE;
} /* qbi_svc_bc_ext_prov_ctx_compare_imsi_for_operator() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_configure_nv_for_operator
===========================================================================*/
/*!
    @brief Configures NV for specific operator if required

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_configure_nv_for_operator
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e                 action       = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_spdp_cache_s          cache        = { 0 };
  qbi_svc_bc_ext_operator_config_s operator_cfg = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qbi_svc_bc_spdp_read_nv_store(txn->ctx, &cache);
  if (qbi_svc_bc_ext_prov_ctx_compare_imsi_for_operator(cache.imsi))
  {
    qbi_svc_bc_ext_prov_ctx_read_operator_nv(txn->ctx, &operator_cfg);
    if (operator_cfg.class1_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_NONE ||
        operator_cfg.class2_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_NONE)
    {
      QBI_LOG_D_0("Prov Contx V2 Sub ready: Operator Configuration Required.");
      if (txn->info)
      {
        QBI_MEM_FREE(txn->info);
        txn->info = NULL;
      }
      /* Performing profile updates as part of processing sub ready.
         Hence ignoring profile update events. */
      cmd_in_progress_ignore_indication = TRUE;
      /* Leveraging the existing prov ctx factory restore processing
         functions to perform the profile disable/enable updates */
      action = qbi_svc_bc_ext_prov_ctx_v2_s_pdc_prep_wds29_req(txn);
    }
    else
    {
      QBI_LOG_D_0("Prov Contx V2 Sub ready: Already Provisioned.");
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }
  else
  {
    QBI_LOG_D_0("Prov Contx V2 Sub ready: Customization Not Reqrd.");
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_configure_nv_for_operator */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_update_operator_nv
===========================================================================*/
/*!
    @brief Updates operator_config NV

    @details

    @param ctx
    @param operator_config

    @return void
*/
/*=========================================================================*/
void qbi_svc_bc_ext_prov_ctx_update_operator_nv
(
  qbi_ctx_s               *ctx,
  qbi_svc_bc_ext_operator_config_s *operator_cfg
)
{
/*-------------------------------------------------------------------------*/
  if (!qbi_nv_store_cfg_item_write(
    ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG, 
    operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s)))
  {
    QBI_LOG_E_0("Prov Contxt V2:: E:Couldn't save profile data to NV!");
  }
} /* qbi_svc_bc_ext_prov_ctx_update_operator_nv() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_update_operator_nv
===========================================================================*/
/*!
    @brief Updates class1/2 NV based on whether apn is enabled/disabled
       conditions

    @details

    @param ctx
    @param GET_PROFILE_SETTINGS QMI RSP
    @param operator_cfg NV
    @param class for which update is required

    @return void
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_prov_ctx_v2_update_operator_nv
(
  qbi_ctx_s *ctx,
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp,
  qbi_svc_bc_ext_operator_config_s *operator_cfg
)
{
  qbi_svc_bc_spdp_cache_s cache = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(operator_cfg);
  QBI_CHECK_NULL_PTR_RET(qmi_rsp);

  qbi_svc_bc_spdp_read_nv_store(ctx, &cache);
  if ((qmi_rsp->apn_disabled_flag_valid == TRUE) &&
      qbi_svc_bc_ext_prov_ctx_compare_imsi_for_operator(cache.imsi))
  {
    if (qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1(ctx, qmi_rsp->apn_name))
    {
      QBI_LOG_D_1("Prov Contxt V2 : Update Operator NV for class 1"
                  " with apn %d", qmi_rsp->apn_disabled_flag);
      if (qmi_rsp->apn_disabled_flag == FALSE)
      {
        operator_cfg->class1_disable = QBI_SVC_BC_EXT_OPERATOR_STATE_UNSET;
      }
      else
      {
        operator_cfg->class1_disable = QBI_SVC_BC_EXT_OPERATOR_STATE_SET;
      }
      qbi_svc_bc_ext_prov_ctx_update_operator_nv(ctx, operator_cfg);
    }
    if (qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class2(qmi_rsp->apn_name))
    {
      QBI_LOG_D_1("Prov Contxt V2 : Update Operator NV for class 2"
                  " with apn %d", qmi_rsp->apn_disabled_flag);
      if (qmi_rsp->apn_disabled_flag == FALSE)
      {
        operator_cfg->class2_disable = QBI_SVC_BC_EXT_OPERATOR_STATE_UNSET;
      }
      else
      {
        operator_cfg->class2_disable = QBI_SVC_BC_EXT_OPERATOR_STATE_SET;
      }
      qbi_svc_bc_ext_prov_ctx_update_operator_nv(ctx, operator_cfg);
    }
  }
} /*! qbi_svc_bc_ext_prov_ctx_v2_update_operator_nv */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_update_operator_nv_if_reqd
===========================================================================*/
/*!
    @brief Check the operator NV,if its NONE sets to to either SET or UNSET
       based on the current profile apn_disable_flag setting

    @details

    @param txn
    @param GET_PROFILE_SETTINGS QMI RSP

    @return void
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_prov_ctx_v2_update_operator_nv_if_reqd
(
  qbi_txn_s *txn,
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp,
  qbi_svc_bc_ext_operator_config_s *operator_cfg
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(qmi_rsp);

  QBI_LOG_D_0("Prov Contxt V2 : Update Operator Status If Required.");
  if (operator_cfg->class1_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_NONE ||
      operator_cfg->class2_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_NONE)
  {
    qbi_svc_bc_ext_prov_ctx_v2_update_operator_nv(txn->ctx,
                                                     qmi_rsp,
                                                     operator_cfg);
  }
} /* qbi_svc_bc_ext_prov_ctx_v2_update_operator_nv_if_reqd */


/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds28_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_MODIFY_PROFILE_SETTINGS_RESP for
  MBIM_CID_PROVISIONED_CONTEXTS set request

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                         action   = QBI_SVC_ACTION_ABORT;
  wds_modify_profile_settings_resp_msg_v01 *qmi_rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_modify_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;

  QBI_LOG_I_0("Rsp Callback For PDC Modify Req");
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    if (qmi_rsp->extended_error_code_valid)
    {
      QBI_LOG_E_1("Extended error code %d", qmi_rsp->extended_error_code);
    }
    if (QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS == qmi_txn->parent->cid)
    {
      /* If modify profile settings QMI resp fails which causes transaction to ABORT
         we need to check if the D is subscriber ready.If so we need to send
         proper response to this CID so that SUBSCRIBER_READY_STATE functinality
         is not affected by this failure */
      QBI_LOG_D_0("Get Profile Setting Failed.Returning to Sub Ready.");
      cmd_in_progress_ignore_indication = FALSE;
      action = QBI_SVC_ACTION_SEND_RSP;
    }
    else
    {
      qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
          qmi_txn->parent, qmi_rsp->resp.error,
          qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
    }
  }
  else
  {
    QBI_LOG_D_0("Modify Complete For PDC Going To Get Next Profile");
    action = qbi_svc_bc_ext_prov_ctx_v2_pdc_get_next_profile(
        qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds28_rsp_cb */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_build_pdc_wds28_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WDS_MODIFY_PROFILE_SETTINGS request

    @details

    @param txn
    @param profile_type

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_build_pdc_wds28_req
(
  qbi_txn_s *txn,
  wds_profile_type_enum_v01 profile_type,
  uint32 index
)
{
  qbi_svc_action_e                        action   = QBI_SVC_ACTION_ABORT;
  wds_modify_profile_settings_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  QBI_LOG_D_0("Initiating Prov Context V2 PDC Profile Modify Req");
  qmi_req = (wds_modify_profile_settings_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ_V01,
      qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds28_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile.profile_index = (uint8_t)index;
  qmi_req->profile.profile_type = profile_type;

  QBI_LOG_D_2("profile_type %d profile_index %d",
              qmi_req->profile.profile_type, qmi_req->profile.profile_index);

  qmi_req->apn_disabled_flag_valid = TRUE;
  qmi_req->apn_disabled_flag = TRUE;
  action = QBI_SVC_ACTION_SEND_QMI_REQ;

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_build_pdc_wds28_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_update_disable_flag
===========================================================================*/
/*!
  @brief Modify Operator Class APN profiles based on current NV setting

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_update_disable_flag
(
  qbi_qmi_txn_s *qmi_txn,
  wds_get_profile_settings_req_msg_v01 *qmi_req,
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp
)
{
  qbi_svc_action_e                 action       = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_operator_config_s operator_cfg = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_rsp);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qbi_svc_bc_ext_prov_ctx_read_operator_nv(qmi_txn->ctx, &operator_cfg);
  if ((QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS == qmi_txn->parent->cid) &&
      (qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1(
              qmi_txn->parent->ctx, qmi_rsp->apn_name) ||
       qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class2(qmi_rsp->apn_name)))
  {
    (void)qbi_svc_bc_ext_prov_ctx_v2_update_operator_nv_if_reqd(qmi_txn->parent, qmi_rsp, &operator_cfg);
    /* Need to read again as we might have changed the NV state */
    (void)qbi_svc_bc_ext_prov_ctx_read_operator_nv(qmi_txn->ctx, &operator_cfg);
  }
  if ((operator_cfg.class1_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_SET &&
       qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1(qmi_txn->parent->ctx, qmi_rsp->apn_name) &&
       (qmi_rsp->apn_disabled_flag_valid == TRUE &&
        qmi_rsp->apn_disabled_flag == FALSE)) ||
      ((operator_cfg.class2_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_SET) &&
       qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator(qmi_rsp->apn_name) &&
       (qmi_rsp->apn_disabled_flag_valid == TRUE &&
        qmi_rsp->apn_disabled_flag == FALSE)))
  {
    QBI_LOG_D_0("Prov Context V2 : Sending Modify Req as part of PDC Seq");
    action = qbi_svc_bc_ext_prov_ctx_v2_s_build_pdc_wds28_req(
        qmi_txn->parent, qmi_req->profile.profile_type,
        qmi_req->profile.profile_index);
  }
  else
  {
    QBI_LOG_D_0("Prov Context V2 : Initiating get next profile");
    action = qbi_svc_bc_ext_prov_ctx_v2_pdc_get_next_profile(
        qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_update_disable_flag */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_timeout_cb
===========================================================================*/
/*!
    @brief Timeout callback invoked when waiting on a
    QMI_UIM_STATUS_CHANGE_IND with the completed operation takes longer than
    expected

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_prov_ctx_v2_s_timeout_cb
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  QBI_LOG_I_0("PROV_V2 ::  No card status indication received with result of factory operation!"
              " Sending response now");

  /* Disable Timer */
  qbi_txn_set_timeout(txn, QBI_TXN_TIMEOUT_DISABLED, NULL);

  (void) qbi_svc_proc_action(txn, qbi_svc_bc_ext_prov_ctx_v2_q_req(txn));
} /* qbi_svc_bc_ext_prov_ctx_v2_s_timeout_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_factory_reset_ind_cb
===========================================================================*/
/*!
    @brief Indication invoked when UIM state changes

    @details Waits for card to get initiliased beforee triggering the query

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_factory_reset_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  const uim_status_change_ind_msg_v01 *qmi_ind;
  uint8 primary_app_index;
  uint8 primary_card_index;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  QBI_LOG_I_0("PROV_V2 :: Processing Indication Callback For Factory Reset");

  qmi_ind = (const uim_status_change_ind_msg_v01 *)ind->buf->data;

  if (qmi_ind->card_status_valid)
  {
    if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
          ind->txn->ctx, &qmi_ind->card_status, &primary_card_index,
          &primary_app_index))
    {
      QBI_LOG_E_0(" PROV_V2 :: Couldn't determine primary app location!");
    }
    else if (!qbi_svc_bc_is_sim_card_initialized(
               &qmi_ind->card_status.card_info[primary_card_index],
               primary_app_index))
    {
      QBI_LOG_I_0(" PROV_V2 :: Waiting on card to become ready");
    }
    else
    {
      QBI_LOG_I_0("PROV_V2 :: Card is ready, Trigerring the query ");
      qbi_txn_set_timeout(ind->txn, QBI_TXN_TIMEOUT_DISABLED, NULL);
      action = qbi_svc_bc_ext_prov_ctx_v2_q_req(ind->txn);
    }
  }
  else
  {
    QBI_LOG_E_0("PROV_V2 ::  Received card info indication without card status");
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_factory_reset_ind_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_pdc_wds2b_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
  QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 Set

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_pdc_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                                       action   = QBI_SVC_ACTION_ABORT;
  wds_get_profile_settings_req_msg_v01                   *qmi_req = NULL;
  wds_get_profile_settings_resp_msg_v01                  *qmi_rsp = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info    = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wds_get_profile_settings_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
    if (QMI_ERR_EXTENDED_INTERNAL_V01 == qmi_rsp->resp.error &&
        qmi_rsp->extended_error_code_valid &&
        WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM_V01 ==
            qmi_rsp->extended_error_code)
    {
      QBI_LOG_D_0("Prov Context V2 Factory Op :: Bad profile. Continue to get next profile");
      info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)qmi_txn->parent->info;

      info->profiles_read++;
      action = qbi_svc_bc_ext_prov_ctx_v2_pdc_get_next_profile(qmi_txn->parent);
    }
    else if (QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS == qmi_txn->parent->cid)
    {
      /* Leveraging existing function to enable/disable during sub ready processing.
         If get profile settings QMI resp fails which causes transaction to ABORT
         we need to check if the D is subscriber ready.If so we need to send
         proper response to this CID so that SUBSCRIBER_READY_STATE functinality
         is not affected by this failure */
      QBI_LOG_D_0("Get Profile Setting Failed! Returning to Sub Ready.");
      cmd_in_progress_ignore_indication = FALSE;
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }
  else
  {
    info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)
        qmi_txn->parent->info;

    info->profiles_read++;
    QBI_LOG_I_2("Received profile %d/%d", info->profiles_read,
                info->profile_list.num_of_profile);

    if (info->profiles_read < info->profile_list.num_of_profile)
    {
      if (qmi_rsp->apn_name_valid &&
          qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator(qmi_rsp->apn_name))
      {
        action = qbi_svc_bc_ext_prov_ctx_v2_s_update_disable_flag(qmi_txn, qmi_req, qmi_rsp);
      }
      else
      {
        action = qbi_svc_bc_ext_prov_ctx_v2_pdc_get_next_profile(
            qmi_txn->parent);
      }
    }
    else
    {
      QBI_LOG_E_0("Profiles exhausted, Sending Response");

      if (qmi_txn->parent->qmi_txns_pending > 0)
      {
        QBI_LOG_D_0("Pending Transaction For Prov Cntxt Factory Set Req");
        action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
      }
      else
      {
        if (qmi_txn->parent->info != NULL)
        {
          QBI_MEM_FREE(qmi_txn->parent->info);
          qmi_txn->parent->info = NULL;
        }

        if (QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG == qmi_txn->parent->cid)
        {
          // Trigger detach
          QBI_LOG_D_0("Factory restore completed successfully. Trigger Detach.");
          cmd_in_progress_ignore_indication = FALSE;
          action = qbi_svc_bc_ext_lte_attach_config_q_req(qmi_txn->parent);
        }
        else if (QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS == qmi_txn->parent->cid)
        {
          /* Leveraging existing function to enable/disable during sub ready processing */
          QBI_LOG_D_0("Operator Flag Validation Done.Returning to Subscriver Ready.");
          cmd_in_progress_ignore_indication = FALSE;
          action = QBI_SVC_ACTION_SEND_RSP;
        }
        else
        {
          /* Setting timeout for dynamic inidation for factory restore */
          if ((NULL != qmi_txn->parent->req.data) &&
            (QBI_SVC_MBIM_MS_CONTEXT_OPERATION_RESTORE_FACTORY ==
            (((qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)qmi_txn->parent->req.data)->operation)))
          {
              /* Setting the timeout so that infinite wait is avoided. */
              qbi_txn_set_timeout(qmi_txn->parent, QBI_SVC_BC_EXT_PROV_V2_TIMEOUT_MS,
                qbi_svc_bc_ext_prov_ctx_v2_s_timeout_cb);
              QBI_LOG_I_1("PROV_V2 ::  Result of factory operation (txn iid %d) pending on indication",
                qmi_txn->parent->iid);
            action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
          }
          else
          {
            QBI_LOG_D_0("Prov Context V2 Request Completed. Triggering Query.");
            action = qbi_svc_bc_ext_prov_ctx_v2_q_req(qmi_txn->parent);
          }
        }
      }
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_pdc_wds2b_rsp_cb */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_pdc_get_next_profile
===========================================================================*/
/*!
  @brief Retrive next available configured profile.

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_pdc_get_next_profile
(
  qbi_txn_s *txn
  )
{
  qbi_svc_action_e                                        action        = QBI_SVC_ACTION_ABORT;
  wds_get_profile_settings_req_msg_v01                   *qmi_req_wds2b = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info          = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)txn->info;
  if (info->profiles_read >= info->profile_list.num_of_profile)
  {
    cmd_in_progress_ignore_indication = FALSE;
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    /* Issue a query to retrieve the profile details */
    qmi_req_wds2b = (wds_get_profile_settings_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_ext_prov_ctx_v2_pdc_wds2b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_wds2b);

    qmi_req_wds2b->profile.profile_type =
        info->profile_list.profile_type[info->profiles_read];
    qmi_req_wds2b->profile.profile_index =
        info->profile_list.profile_index[info->profiles_read];

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_pdc_get_next_profile */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_pdc_wds2a_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_GET_PROFILE_LIST_RESP for
  QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 Set

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_pdc_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                                       action   = QBI_SVC_ACTION_ABORT;
  wds_get_profile_list_req_msg_v01                       *qmi_req = NULL;
  wds_get_profile_list_resp_msg_v01                      *qmi_rsp = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info    = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wds_get_profile_list_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_list_resp_msg_v01 *)qmi_txn->rsp.data;

  /* Modem may reject EPC profile type depending on modem configuration,
  proceed wihtout failing the query. */
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      !(qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
        qmi_rsp->resp.error == QMI_ERR_EXTENDED_INTERNAL_V01 &&
        qmi_rsp->extended_error_code_valid && qmi_rsp->extended_error_code ==
            WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE_V01))
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
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
          sizeof(qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
    }

    /* Append new profile indexes to the profile list */
    info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)
        qmi_txn->parent->info;
    qbi_svc_bc_ext_prov_ctx_populate_profile_list(&info->profile_list, qmi_rsp);

    if (qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01)
    {
      /* Send another QMI_WDS_GET_PROFILE_LIST_REQ to obtain 3GPP profile
      list */
      qmi_req = (wds_get_profile_list_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_LIST_REQ_V01,
          qbi_svc_bc_ext_prov_ctx_v2_pdc_wds2a_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      qmi_req->profile_type_valid = TRUE;
      qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
    else
    {
      QBI_LOG_D_1("Received Total %d EPC/3GPP Profiles via GET_LIST",
                  info->profile_list.num_of_profile);

      info->profiles_read = 0;
      action = qbi_svc_bc_ext_prov_ctx_v2_pdc_get_next_profile(
          qmi_txn->parent);
    }
  }

  if (action == QBI_SVC_ACTION_ABORT &&
      QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS == qmi_txn->parent->cid)
  {
    action = QBI_SVC_ACTION_SEND_RSP;
    cmd_in_progress_ignore_indication = FALSE;
  }
  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_pdc_wds2a_rsp_cb */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_pdc_prep_wds29_req
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_DELETE_PROFILE_LIST_REQ for
  QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 Set.Additionally
  Leveraging the existing prov ctx factory restore processing
  functions to perform the profile disable/enable updates

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc_prep_wds29_req
(
  qbi_txn_s *txn
)
{
  wds_get_profile_list_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_LOG_D_0("Sending Request For Get Profile List");
  qmi_req = (wds_get_profile_list_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_LIST_REQ_V01,
      qbi_svc_bc_ext_prov_ctx_v2_pdc_wds2a_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile_type_valid = TRUE;
  qmi_req->profile_type = WDS_PROFILE_TYPE_EPC_V01;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_pdc_prep_wds29_req */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds29_req
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_DELETE_PROFILE_LIST_REQ for
  QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 Set

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds29_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e               action         = QBI_SVC_ACTION_ABORT;
  wds_delete_profile_req_msg_v01 *qmi_req_wds29 = NULL;
  uint32                         *info          = NULL;
  uint32                         profile_index  = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  txn->info = QBI_MEM_MALLOC_CLEAR(sizeof(uint32));
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
  info = (uint32 *)txn->info;

  // delete user defined profiles and reset cache
  profile_index =
      qbi_svc_bc_ext_prov_ctx_get_user_defined_profile_from_cache(txn->ctx);
  QBI_LOG_D_1("Initiating Delete request for profile index %d",
              profile_index);
  if (QBI_SVC_BC_EXT_INVALID_PROFILE_INDEX != profile_index &&
      profile_index < QMI_WDS_PROFILE_LIST_MAX_V01)
  {
    qmi_req_wds29 = (wds_delete_profile_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_DELETE_PROFILE_REQ_V01,
        qbi_svc_bc_ext_prov_ctx_v2_wds29_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_wds29);

    qmi_req_wds29->profile.profile_index = profile_index;
    *info = profile_index;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;

    QBI_LOG_D_1("Initiating Delete request for profile index %d",
                profile_index);
  }
  else
  {
    action = qbi_svc_bc_ext_module_prov_cache_clear(txn);
    if (action != QBI_SVC_ACTION_ABORT)
    {
      if (txn->info != NULL)
      {
        QBI_MEM_FREE(txn->info);
        txn->info = NULL;
      }

    action = qbi_svc_bc_ext_prov_ctx_v2_s_pdc_prep_wds29_req(txn);
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds29_req */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_pdc27_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PDC_ACTIVATE_REPORT_IND sttaic IND,
    looking for operating mode changes to trigger a
    QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc27_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e                      action   = QBI_SVC_ACTION_ABORT;
  const pdc_activate_config_ind_msg_v01 *act_ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  act_ind = (const pdc_activate_config_ind_msg_v01 *)ind->buf->data;
  if (act_ind->error != 0)
  {
    QBI_LOG_E_1("Received error code %d from QMI", act_ind->error);
  }
  else
  {
    if (!cmd_in_progress_ignore_indication)
    {
      QBI_LOG_D_0("Static Ind : PDC Activation Ind Received,Triggering Query");
      cmd_in_progress_ignore_indication = TRUE;
      action = qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds29_req(ind->txn);
    }
  }

  QBI_LOG_I_0("Static Ind : Processed PDC Activate Indication");
  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_pdc27_ind_cb() */


/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_pdc27_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PDC_ACTIVATE_REPORT_IND, looking for operating mode
    changes to trigger a QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_pdc27_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e                      action   = QBI_SVC_ACTION_ABORT;
  const pdc_activate_config_ind_msg_v01 *act_ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  act_ind = (const pdc_activate_config_ind_msg_v01 *)ind->buf->data;
  if (act_ind->error != 0)
  {
    QBI_LOG_E_1("Received error code %d from QMI", act_ind->error);
  }
  else
  {
    action = qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds29_req(ind->txn);
  }

  QBI_LOG_I_0("Processed PDC Activate Indication");
  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_pdc27_ind_cb() */

/*===========================================================================*/
/*!
  @brief Returns the slot according to MCFG perspective

  @details MCFG supports only one logical slot 0.
  Mapping from two physical slots (0, 1) to one logical slot 0 is
  handled by UIM So from MCFG perspective there only one logical
  slot 0

  @param None

  @return Slot index
*/
/*=========================================================================*/
uint32 qbi_svc_bc_ext_prov_slot_index
(
  void
)
{
  return QBI_SVC_BC_EXT_PROV_V2_UIM_SLOT_INDEX;
} /* qbi_svc_bc_ext_prov_slot_index */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_pdc23_ind_cb
===========================================================================*/
/*!
  @brief Handles a QMI_PDC_SET_SELECTED_CONFIG_REPORT_IND, looking for operating mode
  changes to trigger a QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 event

  @details

  @param ind

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_pdc23_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e                          action         = QBI_SVC_ACTION_ABORT;
  const pdc_set_selected_config_ind_msg_v01 *setsel_ind;
  pdc_activate_config_req_msg_v01           *qmi_req_pdc27 = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  setsel_ind = (const pdc_set_selected_config_ind_msg_v01 *)ind->buf->data;
  if (setsel_ind->error != 0)
  {
    QBI_LOG_E_1("Received error code %d from QMI", setsel_ind->error);
  }
  else
  {
    QBI_LOG_I_0("Initiating Req For PDC Activate Config");
    if (setsel_ind->ind_token_valid == TRUE && setsel_ind->ind_token == PROV_V2_IND_TOKEN)
    {
      qmi_req_pdc27 = (pdc_activate_config_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          ind->txn, QBI_QMI_SVC_PDC, QMI_PDC_ACTIVATE_CONFIG_REQ_V01,
          qbi_svc_bc_ext_prov_ctx_v2_s_pdc27_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_pdc27);


      qmi_req_pdc27->config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
      qmi_req_pdc27->ind_token_valid = TRUE;
      qmi_req_pdc27->ind_token = PROV_V2_IND_TOKEN;
      qmi_req_pdc27->activation_type_valid = TRUE;
      qmi_req_pdc27->activation_type = PDC_ACTIVATION_REGULAR_V01;
      qmi_req_pdc27->subscription_id_valid = TRUE;
      qmi_req_pdc27->subscription_id = 0;
      qmi_req_pdc27->slot_id_valid = TRUE;
      qmi_req_pdc27->slot_id = qbi_svc_bc_ext_prov_slot_index();
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_pdc23_ind_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_pdc22_ind_cb
===========================================================================*/
/*!
  @brief Handles a QMI_PDC_GET_SELECTED_CONFIG_REPORT_IND, looking for operating mode
  changes to trigger a QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 event

  @details

  @param ind

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_pdc22_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e                          action         = QBI_SVC_ACTION_ABORT;
  const pdc_get_selected_config_ind_msg_v01 *getsel_ind;
  pdc_deactivate_config_req_msg_v01         *qmi_req_pdc2b = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  getsel_ind = (const pdc_get_selected_config_ind_msg_v01 *)ind->buf->data;
  if (getsel_ind->error != 0)
  {
    QBI_LOG_E_1("Received error code %d from QMI", getsel_ind->error);
  }
  else
  {
    if (getsel_ind->ind_token_valid == TRUE && getsel_ind->ind_token == PROV_V2_IND_TOKEN)
    {
      QBI_LOG_I_0("Processing GET SELECTED CONFIG REPORT Ind");
      if (getsel_ind->active_config_id_valid == TRUE)
      {
        active_config_id_len = getsel_ind->active_config_id_len;
        QBI_MEMSCPY(active_config_id, PDC_CONFIG_ID_SIZE_MAX_V01,
                    getsel_ind->active_config_id, sizeof(getsel_ind->active_config_id) / sizeof(uint8_t));
      }

      qmi_req_pdc2b = (pdc_deactivate_config_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          ind->txn, QBI_QMI_SVC_PDC, QMI_PDC_DEACTIVATE_CONFIG_REQ_V01,
          qbi_svc_bc_ext_prov_ctx_v2_s_pdc2b_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_pdc2b);

      qmi_req_pdc2b->config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
      qmi_req_pdc2b->ind_token_valid = TRUE;
      qmi_req_pdc2b->ind_token = PROV_V2_IND_TOKEN;
      qmi_req_pdc2b->subscription_id_valid = TRUE;
      qmi_req_pdc2b->subscription_id = 0;
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  QBI_LOG_I_0("Exiting GET SELECTED CONFIG REPORT Ind");
  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_pdc22_ind_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_pdc2b_ind_cb
===========================================================================*/
/*!
    @brief  Handles a QMI_PDC_DEACTIVATE_REPORT_IND, looking for operating mode
    changes to trigger a QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_pdc2b_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e                        action         = QBI_SVC_ACTION_ABORT;
  const pdc_deactivate_config_ind_msg_v01 *deact_ind;
  pdc_set_selected_config_req_msg_v01     *qmi_req_pdc23 = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  deact_ind = (const pdc_deactivate_config_ind_msg_v01 *)ind->buf->data;
  if (deact_ind->error != 0)
  {
    QBI_LOG_E_1("Received error code %d from QMI", deact_ind->error);
  }
  else if (!qbi_svc_ind_reg_dynamic( ind->txn->ctx,
    QBI_SVC_ID_BC_EXT, QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
    QBI_QMI_SVC_UIM, QMI_UIM_STATUS_CHANGE_IND_V01,
    qbi_svc_bc_ext_prov_ctx_v2_factory_reset_ind_cb, ind->txn, NULL))
  {
    QBI_LOG_E_0("PROV_V2 ::  Couldn't register dynamic indication handler!");
  }
  else
  {
    QBI_LOG_I_0("Processing PDC DEACTIVATE REPORT Ind");
    if (deact_ind->ind_token_valid == TRUE && deact_ind->ind_token == PROV_V2_IND_TOKEN)
    {
      qmi_req_pdc23 = (pdc_set_selected_config_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          ind->txn, QBI_QMI_SVC_PDC, QMI_PDC_SET_SELECTED_CONFIG_REQ_V01,
          qbi_svc_bc_ext_prov_ctx_v2_s_pdc23_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_pdc23);

      qmi_req_pdc23->new_config_info.config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;

      if (active_config_id_len != 0)
      {
        qmi_req_pdc23->new_config_info.config_id_len = active_config_id_len;
        QBI_MEMSCPY(qmi_req_pdc23->new_config_info.config_id, PDC_CONFIG_ID_SIZE_MAX_V01,
                    active_config_id, sizeof(active_config_id) / sizeof(uint8_t));
        QBI_LOG_I_1("config_id_len %d", qmi_req_pdc23->new_config_info.config_id_len);
      }
      qmi_req_pdc23->ind_token_valid = TRUE;
      qmi_req_pdc23->ind_token = PROV_V2_IND_TOKEN;
      qmi_req_pdc23->subscription_id_valid = TRUE;
      qmi_req_pdc23->subscription_id = 0;
      qmi_req_pdc23->slot_id = qbi_svc_bc_ext_prov_slot_index();
      qmi_req_pdc23->slot_id_valid = TRUE;
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_pdc2b_ind_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_wdsa8_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_EVENT_REPORT_IND, looking for operating mode
    changes to trigger a QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_wdsa8_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e                      action = QBI_SVC_ACTION_ABORT;
  const wds_profile_changed_ind_msg_v01 *resp  = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);

  resp = (const wds_profile_changed_ind_msg_v01 *)ind->buf->data;

  if (resp->profile_changed_ind_valid)
  {
    QBI_LOG_D_3("Received Indication for Profile Type %d, Profile Index %d,Change Evt %d",
                resp->profile_changed_ind.profile_type,
                resp->profile_changed_ind.profile_index,
                resp->profile_changed_ind.profile_change_evt);
  }

  if (!cmd_in_progress_ignore_indication)
  {
    QBI_LOG_D_0("Profile Changed Ind Received,Triggering Query");
    action = qbi_svc_bc_ext_prov_ctx_v2_q_req(ind->txn);
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_wdsa8_ind_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_wdsa8_rsp_cb
===========================================================================*/
/*!
  @brief Handles QMI_DMS_SET_EVENT_REPORT_RESP

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_wdsa8_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                     action   = QBI_SVC_ACTION_ABORT;
  wds_config_profile_list_resp_msg_v01 *qmi_rsp = NULL;
  pdc_indication_register_req_msg_v01  *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("Received QMI_WDS_CONFIGURE_PROFILE_EVENT_LIST_RESP_V01");

  qmi_rsp = (wds_config_profile_list_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    qmi_req = (pdc_indication_register_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_PDC, QMI_PDC_INDICATION_REGISTER_REQ_V01,
        qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_pdc20_res);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->reg_config_change_valid = TRUE;
    qmi_req->reg_config_change = TRUE;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_wdsa8_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_pdc27_rsp_cb
===========================================================================*/
/*!
  @brief Handles QMI_PDC_ACTIVATE_RESP

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                 action          = QBI_SVC_ACTION_ABORT;
  pdc_activate_config_resp_msg_v01 *qmi_resp_pdc27 = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_resp_pdc27 = (pdc_activate_config_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_resp_pdc27->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_resp_pdc27->resp.error);
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  QBI_LOG_D_0("Received PDC Activation Response");
  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_pdc27_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_pdc23_rsp_cb
===========================================================================*/
/*!
 @brief Handles QMI_PDC_SET_SELECTED_CONFIG_RESP

 @details

 @param qmi_txn

 @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc23_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                     action          = QBI_SVC_ACTION_ABORT;
  pdc_get_selected_config_resp_msg_v01 *qmi_resp_pdc23 = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_resp_pdc23 = (pdc_get_selected_config_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_resp_pdc23->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_resp_pdc23->resp.error);
  }
  else
  {
    QBI_LOG_D_0("Got PDC SELECT CONFIG resp Callback");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_pdc23_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_pdc22_rsp_cb
===========================================================================*/
/*!
 @brief Handles QMI_PDC_GET_SELECTED_CONFIG_RESP

 @details

 @param qmi_txn

 @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                     action          = QBI_SVC_ACTION_ABORT;
  pdc_get_selected_config_resp_msg_v01 *qmi_resp_pdc22 = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_resp_pdc22 = (pdc_get_selected_config_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_resp_pdc22->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_resp_pdc22->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    QBI_LOG_D_0("Got PDC SELECT CONFIG response");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_pdc22_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_pdc2b_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_PDC_DEACTIVATE_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_pdc2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                   action   = QBI_SVC_ACTION_ABORT;
  pdc_deactivate_config_resp_msg_v01 *qmi_rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (pdc_deactivate_config_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  QBI_LOG_D_0("Received PDC DeActivation Callback");
  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_pdc2b_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_pdc20_res
===========================================================================*/
/*!
  @brief Handles QMI_PDC_INDICATION_REGISTER_RESP

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_pdc20_res
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                     action   = QBI_SVC_ACTION_ABORT;
  pdc_indication_register_resp_msg_v01 *qmi_rsp = NULL;
  dsd_system_status_change_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("Received QMI_PDC_INDICATION_REGISTER_RESP");

  qmi_rsp = (pdc_indication_register_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    /* Register for QMI_UIM_STATUS_CHANGE_IND */
    qmi_req = (dsd_system_status_change_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_DSD, QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01,
        qbi_svc_bc_ext_lte_attach_status_reg_ind_dsd25_rsp);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->limit_so_mask_change_ind_valid = TRUE;
    qmi_req->limit_so_mask_change_ind = TRUE;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_pdc20_res() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_wdsa8_req
===========================================================================*/
/*!
    @brief Handles QMI_DMS_SET_EVENT_REPORT_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_wdsa8_req
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                     action   = QBI_SVC_ACTION_SEND_RSP;
  wds_config_profile_list_req_msg_v01  *qmi_req = NULL;
  wds_indication_register_resp_msg_v01 *qmi_rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (wds_indication_register_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for Profile change indications!!! Error code %d",
                qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    QBI_LOG_D_0("Setting INDICATION Params");
    qmi_req = (wds_config_profile_list_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_CONFIGURE_PROFILE_EVENT_LIST_REQ_V01,
        qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_wdsa8_rsp_cb);

    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
    qmi_req->profile_event_register_valid = TRUE;
    qmi_req->profile_event_register_len = 1;
    qmi_req->profile_event_register[0].profile_type = 0xFF;
    qmi_req->profile_event_register[0].profile_index = 0xFF;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }


  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_qmi_ind_reg_wdsa8_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status
===========================================================================*/
/*!
    @brief Attempts to map QMI error information into a descriptive MBIM
    error status for MBIM_CID_PROVISIONED_CONTEXT_V2

    @details

    @param txn
    @param qmi_error
    @param qmi_error_ds_ext_valid
    @param qmi_error_ds_ext
*/
/*=========================================================================*/
void qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status
(
  qbi_txn_s *txn,
  qmi_error_type_v01                  qmi_error,
  uint8_t                             qmi_error_ds_ext_valid,
  wds_ds_extended_error_code_enum_v01 qmi_error_ds_ext
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  /* For all aborts setting the status, hence setting this to
  FALSE to handle new request */
  cmd_in_progress_ignore_indication =  FALSE;

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
} /* qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_mbim_auth_proto
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
static uint32 qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_mbim_auth_proto
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
} /* qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_mbim_auth_proto() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_context_type
===========================================================================*/
/*!
  @brief Determines the ContextType UUID for QMI profile common app user
  data TLV

  @details

  @param context_id

  @return const uint8* Pointer to UUID, or NULL on unexpected error
*/
/*=========================================================================*/
static const uint8* qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_context_type
(
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
  )
{
  const uint8 *context_type = NULL;
  uint32      app_user_data;
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
} /* qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_context_type() */


/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_mbim_compression
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
static uint32 qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_mbim_compression
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
} /* qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_mbim_compression() */


/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_q_req
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 query 
    request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e                 action   = QBI_SVC_ACTION_ABORT;
  wds_get_profile_list_req_msg_v01 *qmi_req = NULL;
  boolean is_cmd_factory_restore = FALSE;
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (NULL != txn->req.data)
  {
    req = (qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)txn->req.data;
    if (QBI_SVC_MBIM_MS_CONTEXT_OPERATION_RESTORE_FACTORY == req->operation)
    {
      QBI_LOG_D_0(" Under Factory Restore and card is Ready ");
      is_cmd_factory_restore = TRUE;
    }
  }

  if ((FALSE == is_cmd_factory_restore) && (!qbi_svc_bc_sim_subscriber_ready_status_is_ready(
          txn, TRUE)))
  {
    QBI_LOG_D_0("Device Not Ready Unable to Process Query Request! ");
    txn->status = QBI_MBIM_STATUS_FAILURE;
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    cmd_in_progress_ignore_indication = TRUE;

    QBI_LOG_D_0("Received Query For Prov Context V2");
    /* Getting List for EPC,3GPP and then 3GPP2 sequentially */
    qmi_req = (wds_get_profile_list_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_LIST_REQ_V01,
        qbi_svc_bc_ext_prov_ctx_v2_q_wds2a_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->profile_type_valid = TRUE;
    qmi_req->profile_type = WDS_PROFILE_TYPE_EPC_V01;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_q_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_q_wds2a_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_GET_PROFILE_LIST_RESP for
  QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 query

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_q_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_list_req_msg_v01                       *qmi_req  = NULL;
  wds_get_profile_list_resp_msg_v01                      *qmi_rsp  = NULL;
  qbi_svc_action_e                                       action    = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info     = NULL;
  qbi_svc_bc_ext_provisioned_contexts_v2_list_s          *rsp_list = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wds_get_profile_list_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_list_resp_msg_v01 *)qmi_txn->rsp.data;

  /* Modem may reject EPC profile type depending on modem configuration,
  proceed wihtout failing the query. */
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      !(qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
        qmi_rsp->resp.error == QMI_ERR_EXTENDED_INTERNAL_V01 &&
        qmi_rsp->extended_error_code_valid && qmi_rsp->extended_error_code ==
            WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE_V01))
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
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
          sizeof(qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
    }

    /* Append new profile indexes to the profile list */
    info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)
        qmi_txn->parent->info;
    qbi_svc_bc_ext_prov_ctx_populate_profile_list(&info->profile_list, qmi_rsp);

    if (qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01)
    {
      /* Send another QMI_WDS_GET_PROFILE_LIST_REQ to obtain 3GPP profile
      list */
      qmi_req = (wds_get_profile_list_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_LIST_REQ_V01,
          qbi_svc_bc_ext_prov_ctx_v2_q_wds2a_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      qmi_req->profile_type_valid = TRUE;
      qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
    else
    {
      QBI_LOG_D_1("Received Total %d EPC/3GPP Profiles via GET_LIST",
                  info->profile_list.num_of_profile);
      QBI_LOG_D_0("Initiating QMI_WDS_GET_PROFILE_SETTINGS_REQ Req");
      /* Allocate the fixed-length and offset/size pair portion of the
      response now. */
      rsp_list = (qbi_svc_bc_ext_provisioned_contexts_v2_list_s *)
          qbi_txn_alloc_rsp_buf(
          qmi_txn->parent, (sizeof(qbi_svc_bc_ext_provisioned_contexts_v2_list_s) +
                            sizeof(qbi_mbim_offset_size_pair_s) *
                            info->profile_list.num_of_profile));
      QBI_CHECK_NULL_PTR_RET_ABORT(rsp_list);
      rsp_list->element_count = info->profile_list.num_of_profile;

      info->profiles_read = 0;
      action = qbi_svc_bc_ext_prov_ctx_v2_q_get_next_profile(
          qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_q_wds2a_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_q_get_next_profile
===========================================================================*/
/*!
  @brief Retrive next available configured profile.

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_q_get_next_profile
(
  qbi_txn_s *txn
)
{
  wds_get_profile_settings_req_msg_v01                   *qmi_req_wds2b = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info          = NULL;
  qbi_svc_action_e                                       action         = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)txn->info;
  if (info->profiles_read >= info->profile_list.num_of_profile)
  {
    cmd_in_progress_ignore_indication = FALSE;
    action = QBI_SVC_ACTION_SEND_RSP;
    txn->status = QBI_MBIM_STATUS_SUCCESS;
  }
  else
  {
    /* Issue a query to retrieve the profile details */
    qmi_req_wds2b = (wds_get_profile_settings_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_ext_prov_ctx_v2_q_wds2b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_wds2b);

    qmi_req_wds2b->profile.profile_type =
        info->profile_list.profile_type[info->profiles_read];
    qmi_req_wds2b->profile.profile_index =
        info->profile_list.profile_index[info->profiles_read];

    QBI_LOG_D_2("profile type %d index %d",
                qmi_req_wds2b->profile.profile_type, qmi_req_wds2b->profile.profile_index);
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_q_get_next_profile */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_q_wds2b_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
  QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 query

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_q_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_req_msg_v01                   *qmi_req    = NULL;
  wds_get_profile_settings_resp_msg_v01                  *qmi_rsp    = NULL;
  qbi_mbim_offset_size_pair_s                            *field_desc = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info       = NULL;
  qbi_svc_action_e                                       action      = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wds_get_profile_settings_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }
  else
  {
    info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)
        qmi_txn->parent->info;

    field_desc = (qbi_mbim_offset_size_pair_s *)
        ((uint8 *)qmi_txn->parent->rsp.data +
         sizeof(qbi_svc_bc_ext_provisioned_contexts_v2_list_s) +
         sizeof(qbi_mbim_offset_size_pair_s) * info->profiles_read);

    info->profiles_read++;
    QBI_LOG_I_2("Received profile %d/%d", info->profiles_read,
                info->profile_list.num_of_profile);

    if (!qbi_svc_ext_prov_ctx_v2_q_add_context_to_rsp(
            qmi_txn->parent, field_desc, qmi_req->profile.profile_type,
            qmi_rsp, qmi_req->profile.profile_index))
    {
      QBI_LOG_E_0("Couldn't add context to response!");
    }
    else
    {
      action = qbi_svc_bc_ext_prov_ctx_v2_q_get_next_profile(
          qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_q_wds2b_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_compare_3gpp_3gpp2_profiles
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
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_compare_3gpp_3gpp2_profiles
(
  qbi_svc_bc_ext_prov_ctx_v2_3gpp_profile_settings_s *profile_settings_3gpp,
  wds_get_profile_settings_resp_msg_v01              *qmi_rsp_3gpp2
)
{
  boolean status = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings_3gpp);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp_3gpp2);

  if (!profile_settings_3gpp->apn_name_valid ||
     !qmi_rsp_3gpp2->apn_string_valid ||
     QBI_STRNICMP(profile_settings_3gpp->apn_name, qmi_rsp_3gpp2->apn_string,
     QMI_WDS_APN_NAME_MAX_V01))
  {
    QBI_LOG_D_0("3GPP/3GPP2 APN names do not match!");
  }
  else if (!profile_settings_3gpp->username_valid ||
    !qmi_rsp_3gpp2->user_id_valid ||
    QBI_STRNICMP(profile_settings_3gpp->username, qmi_rsp_3gpp2->user_id,
    QMI_WDS_USER_NAME_MAX_V01))
  {
    QBI_LOG_D_0("3GPP/3GPP2 usernames do not match!");
  }
  else
  {
    status = TRUE;
  }

  return status;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_compare_3gpp_3gpp2_profiles() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_3gpp2_wds2b_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
  QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request to retrieve settings for a
  3gpp2 profile

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_3gpp2_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_resp_msg_v01                  *qmi_rsp = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info    = NULL;
  qbi_svc_action_e                                       action   = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }
  else
  {
    info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)
        qmi_txn->parent->info;

    if (qbi_svc_bc_ext_prov_ctx_v2_s_compare_3gpp_3gpp2_profiles(
            &info->profile_settings_3gpp, qmi_rsp))
    {
      info->profile_found_3gpp2 = TRUE;
      info->profile_index_3gpp2 =
          info->profile_list.profile_index[info->profiles_read];

      action = qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req(
          qmi_txn->parent, WDS_PROFILE_TYPE_3GPP_V01, info->profile_index_3gpp2);
    }
    else
    {
      info->profiles_read++;
      action = qbi_svc_bc_ext_prov_ctx_v2_s_get_next_3gpp2_profile(
          qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_3gpp2_wds2b_rsp_cb() */


/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_get_next_3gpp2_profile
===========================================================================*/
/*!
  @brief Retrive next available 3gpp2 configured profile.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_get_next_3gpp2_profile
(
  qbi_txn_s *txn
)
{
  wds_get_profile_settings_req_msg_v01                   *qmi_req_wds2b = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info          = NULL;
  qbi_svc_action_e                                       action         = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)txn->info;

  if (info->profiles_read >= info->profile_list.num_of_profile)
  {
    action = qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req(
        txn, WDS_PROFILE_TYPE_3GPP_V01, info->profile_index_3gpp);
  }
  else
  {
    /* Issue a query to retrieve the profile details */
    qmi_req_wds2b = (wds_get_profile_settings_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_ext_prov_ctx_v2_s_3gpp2_wds2b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_wds2b);

    qmi_req_wds2b->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
    qmi_req_wds2b->profile.profile_index = info->profile_index_3gpp2;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_get_next_3gpp2_profile */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_enable_to_mbim_enable
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
static uint32 qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_enable_to_mbim_enable
(
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
)
{
  uint32 enable = QBI_SVC_MBIM_MS_CONTEXT_DISABLED;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_rsp);

  if (qmi_rsp->common_apn_disabled_flag_valid == TRUE)
  {
    enable = qmi_rsp->common_apn_disabled_flag == 1 ?
        QBI_SVC_MBIM_MS_CONTEXT_DISABLED : QBI_SVC_MBIM_MS_CONTEXT_ENABLED;
    QBI_LOG_D_2("common apn disabled flag valid %d enable %d", qmi_rsp->common_apn_disabled_flag_valid, enable);
  }
  else if (qmi_rsp->apn_disabled_flag_valid == TRUE)
  {
    enable = qmi_rsp->apn_disabled_flag == 1 ?
        QBI_SVC_MBIM_MS_CONTEXT_DISABLED : QBI_SVC_MBIM_MS_CONTEXT_ENABLED;
    QBI_LOG_D_2("apn disabled flag valid %d enable %d", qmi_rsp->apn_disabled_flag_valid, enable);
  }
  else if (qmi_rsp->apn_enabled_3gpp2_valid == TRUE)
  {
    enable = qmi_rsp->apn_enabled_3gpp2 == 1 ?
        QBI_SVC_MBIM_MS_CONTEXT_ENABLED : QBI_SVC_MBIM_MS_CONTEXT_DISABLED;
    QBI_LOG_D_2("apn enabled 3gpp2 valid %d enable %d", qmi_rsp->apn_enabled_3gpp2_valid, enable);
  }

  return enable;
} /* qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_enable_to_mbim_enable() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_update_cache
===========================================================================*/
/*!
    @brief Allocates and populates a MBIM_CONTEXT_STATE structure on the
    response

    @details

    @param txn
    @param qmi_rsp

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
qbi_svc_bc_ext_module_prov_cache_s* qbi_svc_bc_ext_prov_ctx_v2_update_cache
(
  qbi_txn_s *txn,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
)
{
  boolean                                                set_default = TRUE;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info       = NULL;
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s         *req        = NULL;
  qbi_svc_bc_ext_module_prov_cache_s                     *cache      = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(txn);
  QBI_CHECK_NULL_PTR_RET_NULL(txn->info);
  QBI_CHECK_NULL_PTR_RET_NULL(qmi_rsp);

  info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)txn->info;

  cache = qbi_svc_bc_ext_module_prov_cache_get(txn->ctx,
                                   info->profile_list.profile_index[info->profiles_read]);
  QBI_CHECK_NULL_PTR_RET_NULL(cache);

  QBI_LOG_D_0("Prov Cntxt V2 : Updating Cache");
  if (txn->req.data)
  {
    // This is set case. Update cache.
    QBI_LOG_D_0("Cache Updating As Part Of Set Operation");
    req = (qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)txn->req.data;

    switch (req->operation)
    {
    case QBI_SVC_MBIM_MS_CONTEXT_OPERATION_RESTORE_FACTORY:
      cache->prov_active = FALSE;
      cache->lte_active = FALSE;
      set_default = TRUE;
      break;
    case QBI_SVC_MBIM_MS_CONTEXT_OPERATION_DELETE:
      cache->prov_active = FALSE;
      cache->lte_active = FALSE;
      set_default = FALSE;
      break;
    case QBI_SVC_MBIM_MS_CONTEXT_OPERATION_DEFAULT:
      cache->prov_active = TRUE;
      set_default = FALSE;

      cache->source = req->source;
      switch (req->roaming)
      {
      case QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_HOME_ONLY:
        cache->roaming_flag = QBI_SVC_BC_EXT_ROAMING_FLAG_HOME;
        break;

      case QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_HOME_AND_PARTNER:
        cache->roaming_flag = QBI_SVC_BC_EXT_ROAMING_FLAG_HOME |
            QBI_SVC_BC_EXT_ROAMING_FLAG_PARTNER;
        break;

      case QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_HOME_AND_NON_PARTNER:
        cache->roaming_flag = QBI_SVC_BC_EXT_ROAMING_FLAG_HOME |
            QBI_SVC_BC_EXT_ROAMING_FLAG_NON_PARTNER;
        break;

      case QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_PARTNER_ONLY:
        cache->roaming_flag = QBI_SVC_BC_EXT_ROAMING_FLAG_PARTNER;
        break;

      case QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_PARTNER_AND_NON_PARTNER:
        cache->roaming_flag = QBI_SVC_BC_EXT_ROAMING_FLAG_PARTNER |
            QBI_SVC_BC_EXT_ROAMING_FLAG_NON_PARTNER;
        break;
      case QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_NON_PARTNER_ONLY:
        cache->roaming_flag = QBI_SVC_BC_EXT_ROAMING_FLAG_NON_PARTNER;
        break;

      case QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_ALLOW_ALL:
      default:
        cache->roaming_flag = QBI_SVC_BC_EXT_ROAMING_FLAG_HOME |
            QBI_SVC_BC_EXT_ROAMING_FLAG_PARTNER |
            QBI_SVC_BC_EXT_ROAMING_FLAG_NON_PARTNER;
        break;
      }
      cache->media_type = req->media_type;
      cache->enable =
          qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_enable_to_mbim_enable(qmi_rsp);
      break;
    }
    if (qmi_rsp->pdp_type_valid)
    {
      (void)qbi_svc_bc_ext_update_cache_ip_type(txn,
                                                     req->operation, cache, req->ip_type, qmi_rsp->pdp_type);
    }
  }

  if (set_default && !cache->prov_active)
  {
    // This is query case. Update default value.
    QBI_LOG_D_0("Cache Updating As Part Of Query Operation");
    cache->source = QBI_SVC_MBIM_MS_CONTEXT_SOURCE_MODEM;
    cache->media_type = QBI_SVC_MBIM_MS_CONTEXT_MEDIA_TYPE_CELLULAR;
    cache->enable =
        qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_enable_to_mbim_enable(qmi_rsp);
    if (qmi_rsp->pdp_type_valid)
    {
      (void)qbi_svc_bc_ext_update_cache_ip_type(txn,
                                                     0, cache, 0, qmi_rsp->pdp_type);
    }
    cache->prov_active = TRUE;
  }

  qbi_svc_bc_ext_update_nv_store(txn->ctx);

  return cache;
} /* qbi_svc_bc_ext_prov_ctx_v2_update_cache() */


/*===========================================================================
  FUNCTION: qbi_svc_ext_prov_ctx_v2_q_add_context_to_rsp
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
static boolean qbi_svc_ext_prov_ctx_v2_q_add_context_to_rsp
(
  qbi_txn_s *txn,
  qbi_mbim_offset_size_pair_s *field_desc,
  wds_profile_type_enum_v01                    profile_type,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp,
  uint32                                       context_id
)
{
  boolean                                          success        = FALSE;
  uint32                                           initial_offset = 0;
  qbi_svc_bc_ext_provisioned_contexts_context_v2_s *context       = NULL;
  const uint8                                      *context_type  = NULL;
  qbi_svc_bc_ext_module_prov_cache_s               *cache         = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);

  initial_offset = txn->infobuf_len_total;
  context = (qbi_svc_bc_ext_provisioned_contexts_context_v2_s *)
      qbi_txn_rsp_databuf_add_field(
      txn, field_desc, 0, sizeof(qbi_svc_bc_ext_provisioned_contexts_context_v2_s),
      NULL);
  QBI_CHECK_NULL_PTR_RET_FALSE(context);

  cache = qbi_svc_bc_ext_prov_ctx_v2_update_cache(txn, qmi_rsp);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  context_type =
      qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_context_type(qmi_rsp);
  QBI_CHECK_NULL_PTR_RET_FALSE(context_type);

  QBI_MEMSCPY(context->context_type, sizeof(context->context_type),
              context_type, QBI_MBIM_UUID_LEN);

  context->context_id = context_id;

  context->ip_type = cache->ip_type;

  context->enable = cache->enable;

  switch (cache->roaming_flag)
  {
  case 1:
    context->roaming = QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_HOME_ONLY;
    break;

  case 2:
    context->roaming = QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_PARTNER_ONLY;
    break;

  case 3:
    context->roaming = QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_HOME_AND_PARTNER;
    break;

  case 4:
    context->roaming = QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_NON_PARTNER_ONLY;
    break;

  case 5:
    context->roaming = QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_HOME_AND_NON_PARTNER;
    break;

  case 6:
    context->roaming = QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_PARTNER_AND_NON_PARTNER;
    break;

  case 0:
  case 7:
  default:
    context->roaming = QBI_SVC_MBIM_MS_CONTEXT_ROAMING_CONTROL_ALLOW_ALL;
    break;
  }
  context->media_type = cache->media_type;
  context->source = cache->source;

  context->compression =
      qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_mbim_compression(
      profile_type, qmi_rsp);
  context->auth_protocol =
      qbi_svc_bc_ext_prov_ctx_v2_qmi_profile_to_mbim_auth_proto(
      profile_type, qmi_rsp);

  QBI_LOG_D_0("Cache Status :");
  QBI_LOG_D_4("context_id %d enable %d roaming %d ip type %d",
              context->context_id, context->enable, context->roaming,
              context->ip_type);
  QBI_LOG_D_4("media_type %d source %d compression %d auth_protocol %d ",
              context->media_type, context->source, context->compression,
              context->auth_protocol);

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
  else if (profile_type == WDS_PROFILE_TYPE_3GPP_V01 ||
             profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    if (qmi_rsp->apn_name_valid &&
        !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
            txn, &context->access_string, initial_offset,
            QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES, qmi_rsp->apn_name,
            sizeof(qmi_rsp->apn_name)))
    {
      QBI_LOG_E_0("Couldn't add 3GPP/3GPP2 AccessString to response!");
    }
    else if (qmi_rsp->username_valid &&
               !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
            txn, &context->username, initial_offset,
            QBI_SVC_BC_USERNAME_MAX_LEN_BYTES, qmi_rsp->username,
            sizeof(qmi_rsp->username)))
    {
      QBI_LOG_E_0("Couldn't add 3GPP/3GPP2 Username to response!");
    }
    else if (qmi_rsp->password_valid &&
               !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
            txn, &context->password, initial_offset,
            QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES,
            qmi_rsp->password, sizeof(qmi_rsp->password)))
    {
      QBI_LOG_E_0("Couldn't add 3GPP/3GPP2 Password to response!");
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
    QBI_LOG_D_0("Response Looks Good");
    field_desc->size = txn->infobuf_len_total - initial_offset;
    success = qbi_txn_rsp_databuf_consolidate(txn);
  }

  return success;
} /* qbi_svc_ext_prov_ctx_v2_q_add_context_to_rsp() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_wds27_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_CREATE_PROFILE_RESP for
  QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_wds27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                                               action           = QBI_SVC_ACTION_ABORT;
  wds_create_profile_req_msg_v01                                 *qmi_req         = NULL;
  wds_create_profile_resp_msg_v01                                *qmi_rsp         = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s             profile_settings = { 0 };
  qbi_svc_bc_ext_module_prov_cache_s                             *cache           = NULL;
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s                 *req             = NULL;
  qbi_svc_bc_ext_operator_config_s                               operator_cfg     = { 0 };
  char                                                           apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  const uint8                                                    *field;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  qmi_req = (wds_create_profile_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_create_profile_resp_msg_v01 *)qmi_txn->rsp.data;
  req = (qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)qmi_txn->parent->req.data;

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
      qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
          qmi_txn->parent, qmi_rsp->resp.error,
          qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
    }
    else
    {
      qmi_req = (wds_create_profile_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_CREATE_PROFILE_REQ_V01,
          qbi_svc_bc_ext_prov_ctx_v2_s_wds27_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP_V01;

      if (!qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds27(
              qmi_req, &profile_settings, WDS_PROFILE_TYPE_3GPP_V01))
      {
        QBI_LOG_E_0("Couldn't collect profile setting pointers!");
      }
      else
      {
        action = qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile(
            qmi_txn->parent, qmi_req->profile_type, &profile_settings); /* Need to check and remove further */
      }
    }
  }
  else
  {
    // update cache
    cache = qbi_svc_bc_ext_module_prov_cache_get(
        qmi_txn->ctx, qmi_rsp->profile.profile_index);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);
    cache->context_flag = QBI_SVC_BC_EXT_CONTEXT_FLAG_USER_DEFINED;

    if (req->access_string.size != 0)
    {
      field = qbi_txn_req_databuf_get_field(
          qmi_txn->parent, &req->access_string, 0,
          QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
      QBI_CHECK_NULL_PTR_RET_FALSE(field);

      (void)qbi_util_utf16_to_ascii(
          field, req->access_string.size, apn_name, sizeof(apn_name));
      if (qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator(apn_name))
      {
        QBI_LOG_D_0("Requested APN is Operator APN");
        qbi_nv_store_cfg_item_read(
            qmi_txn->ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
            &operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s));
        if ((operator_cfg.class1_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_UNSET &&
             qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1(qmi_txn->parent->ctx, apn_name) &&
             req->enable == FALSE))
        {
          QBI_LOG_D_0("Setting class1_disable to TRUE");
          operator_cfg.class1_disable = QBI_SVC_BC_EXT_OPERATOR_STATE_SET;
          if (!qbi_nv_store_cfg_item_write(
                  qmi_txn->ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
                  &operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s)))
          {
            QBI_LOG_E_0("Couldn't save operator_config NV for class 1!!");
          }
        }
        if (operator_cfg.class2_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_UNSET &&
            qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class2(apn_name) &&
            req->enable == FALSE)
        {
          QBI_LOG_D_0("Setting class2_disable to TRUE");
          operator_cfg.class2_disable = QBI_SVC_BC_EXT_OPERATOR_STATE_SET;
          if (!qbi_nv_store_cfg_item_write(
                  qmi_txn->ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
                  &operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s)))
          {
            QBI_LOG_E_0("Couldn't save operator_config NV for class 2!!");
          }
        }
      }
    }
    if (qmi_txn->parent->qmi_txns_pending > 0)
    {
      QBI_LOG_D_0("Pending Transaction For Prov Cntxt V2 Set Req");
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
    else
    {
      if (qmi_txn->parent->info != NULL)
      {
        QBI_MEM_FREE(qmi_txn->parent->info);
        qmi_txn->parent->info = NULL;
      }
      QBI_LOG_D_0("Create Profile Completed "
                  "Triggering Check whether Class 2 disabled required.");

      cmd_in_progress_ignore_indication = TRUE;
      action = qbi_svc_bc_ext_prov_ctx_v2_s_pdc_prep_wds29_req(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_wds27_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_build_wds27_req
===========================================================================*/
/*!
  @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 query request

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_build_wds27_req
(
  qbi_txn_s *txn
)
{
  wds_create_profile_req_msg_v01                                 *qmi_req         = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s             profile_settings;
  qbi_svc_action_e                                               action           = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  qmi_req = (wds_create_profile_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_CREATE_PROFILE_REQ_V01,
      qbi_svc_bc_ext_prov_ctx_v2_s_wds27_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile_type = WDS_PROFILE_TYPE_EPC_V01;

  if (!qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds27(
          qmi_req, &profile_settings, qmi_req->profile_type))
  {
    QBI_LOG_E_0("Couldn't collect profile setting pointers!");
  }
  else
  {
    QBI_LOG_D_1("Building Create request for profile_type %d", qmi_req->profile_type);
    action = qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile(
        txn, qmi_req->profile_type, &profile_settings);
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_build_wds27_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_wds29_rsp_cb
===========================================================================*/
/*!
    @brief Delete Profile Response for a 
    QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e QBI_SVC_ACTION_SEND_QMI_REQ on success,
    QBI_SVC_ACTION_ABORT on failure
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_wds29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_delete_profile_resp_msg_v01 *qmi_rsp       = NULL;
  uint32                          *profile_index;
  qbi_svc_bc_ext_module_prov_cache_s     *cache  = NULL;
  qbi_svc_action_e                action         = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  qmi_rsp = (wds_delete_profile_resp_msg_v01 *)qmi_txn->rsp.data;
  if ((qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01) &&
      (qmi_rsp->resp.error == QMI_ERR_EXTENDED_INTERNAL_V01) &&
      (qmi_rsp->extended_error_code_valid && qmi_rsp->extended_error_code ==
           (WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE_V01 ||
            WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM_V01)))
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid,
        qmi_rsp->extended_error_code);
  }

  profile_index = (uint32 *)qmi_txn->parent->info;

  cache = (qbi_svc_bc_ext_module_prov_cache_s *)
      qbi_svc_bc_ext_module_prov_cache_get(qmi_txn->ctx, *profile_index);

  if (cache != NULL)
  {
    QBI_MEMSET(cache, 0, sizeof(qbi_svc_bc_ext_module_prov_cache_s));
    cache = NULL;
  }

  QBI_LOG_D_1("Profile Index %d Deleted", *profile_index);
  //This is a recursive func so need to free info as we would reallocate
  if (qmi_txn->parent->info != NULL)
  {
    QBI_MEM_FREE(qmi_txn->parent->info);
    qmi_txn->parent->info = NULL;
  }

  action = qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds29_req(
      qmi_txn->parent);

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_wds29_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_register_for_pdc_ind
===========================================================================*/
/*!
  @brief Registers for QMI_PDC_GET_SELECTED_CONFIG_IND_V01,
  QMI_PDC_DEACTIVATE_CONFIG_IND_V01, QMI_PDC_SET_SELECTED_CONFIG_IND_V01
  and QMI_PDC_ACTIVATE_CONFIG_IND_V01 indications

  @details

  @param txn

  @return boolean returns true if registeration is successful
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_register_for_pdc_ind
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_D_0("LTE attach config:: Registering for detach/attach indications");
  if (!qbi_svc_ind_reg_dynamic(
          txn->ctx, QBI_SVC_ID_BC_EXT,
          QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
          QBI_QMI_SVC_PDC, QMI_PDC_GET_SELECTED_CONFIG_IND_V01,
          qbi_svc_bc_ext_prov_ctx_v2_pdc22_ind_cb, txn, NULL))
  {
    QBI_LOG_E_0("Prov Contxt V2 :: Failed to register GET SELECT CONFIG indication.");
    return FALSE;
  }
  else if (!qbi_svc_ind_reg_dynamic(
          txn->ctx, QBI_SVC_ID_BC_EXT,
          QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
          QBI_QMI_SVC_PDC, QMI_PDC_DEACTIVATE_CONFIG_IND_V01,
          qbi_svc_bc_ext_prov_ctx_v2_pdc2b_ind_cb, txn, NULL))
  {
    QBI_LOG_E_0("Prov Contxt V2:: Failed to register PDC DEACTIVATE indication.");
    return FALSE;
  }
  else if (!qbi_svc_ind_reg_dynamic(
          txn->ctx, QBI_SVC_ID_BC_EXT,
          QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
          QBI_QMI_SVC_PDC, QMI_PDC_SET_SELECTED_CONFIG_IND_V01,
          qbi_svc_bc_ext_prov_ctx_v2_pdc23_ind_cb, txn, NULL))
  {
    QBI_LOG_E_0("Prov Contxt V2:: Failed to register SET SELECT CONFIG indication.");
    return FALSE;
  }
  else if (!qbi_svc_ind_reg_dynamic(
          txn->ctx, QBI_SVC_ID_BC_EXT,
          QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
          QBI_QMI_SVC_PDC, QMI_PDC_ACTIVATE_CONFIG_IND_V01,
          qbi_svc_bc_ext_prov_ctx_v2_pdc27_ind_cb, txn, NULL))
  {
    QBI_LOG_E_0("Prov Contxt V2:: Failed to register PDC ACTIVATE indication.");
    return FALSE;
  }

  return TRUE;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_register_for_pdc_ind() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_factory_reset
===========================================================================*/
/*!
    @brief Request QMI_WDS_DELETE_PROFILE_REQ_V01 and
    QMI_PDC_GET_SELECTED_CONFIG_REQ_V01

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_factory_reset
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e                    action         = QBI_SVC_ACTION_ABORT;
  pdc_get_selected_config_req_msg_v01 *qmi_req_pdc22 = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_D_0("Prov Cntxt V2:: Registering for PDC indications");
  if (!qbi_svc_bc_ext_prov_ctx_v2_s_register_for_pdc_ind(txn))
  {
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    qmi_req_pdc22 = (pdc_get_selected_config_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_PDC, QMI_PDC_GET_SELECTED_CONFIG_REQ_V01,
        qbi_svc_bc_ext_prov_ctx_v2_s_pdc22_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_pdc22);

    qmi_req_pdc22->config_type = PDC_CONFIG_TYPE_MODEM_SW_V01;
    qmi_req_pdc22->ind_token_valid = TRUE;
    qmi_req_pdc22->ind_token = PROV_V2_IND_TOKEN;
    qmi_req_pdc22->subscription_id_valid = TRUE;
    qmi_req_pdc22->subscription_id = 0;
    qmi_req_pdc22->slot_id = qbi_svc_bc_ext_prov_slot_index();
    qmi_req_pdc22->slot_id_valid = TRUE;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_factory_reset() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_req
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *req            = NULL;
  qbi_svc_action_e                               action          = QBI_SVC_ACTION_ABORT;
  uint32                                         context_type_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  cmd_in_progress_ignore_indication = TRUE;

  req = (qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)txn->req.data;
  if (!qbi_svc_bc_sim_subscriber_ready_status_is_ready(
          txn, TRUE))
  {
    QBI_LOG_D_0("Device Not Ready Unable to Process Set Request! ");
    txn->status = QBI_MBIM_STATUS_FAILURE;
    action = QBI_SVC_ACTION_ABORT;
  }
  else if (req->operation !=
                 QBI_SVC_MBIM_MS_CONTEXT_OPERATION_DEFAULT &&
             req->operation != QBI_SVC_MBIM_MS_CONTEXT_OPERATION_DELETE &&
             req->operation != QBI_SVC_MBIM_MS_CONTEXT_OPERATION_RESTORE_FACTORY)
  {
    QBI_LOG_E_0("Invalid Operation!");
    return QBI_SVC_ACTION_ABORT;
  } else if (req->operation ==
                 QBI_SVC_MBIM_MS_CONTEXT_OPERATION_RESTORE_FACTORY)
  {
    /* This factory reset case */
    QBI_LOG_D_0("Initiating factory reset of profiles");

    action = qbi_svc_bc_ext_prov_ctx_v2_factory_reset(txn);
  }
  else
  {
    if (!qbi_svc_bc_context_type_uuid_to_id(
            req->context_type, &context_type_id))
    {
      QBI_LOG_E_0("Received unsupported ContextType!");
      txn->status = QBI_MBIM_STATUS_CONTEXT_NOT_SUPPORTED;
    }
    else
    {
      txn->info = QBI_MEM_MALLOC_CLEAR(
          sizeof(qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

      QBI_LOG_I_0("Initiating Prov Context V2 Set Request");
      action = qbi_svc_bc_ext_prov_ctx_v2_s_build_wds2a_req(
          txn, WDS_PROFILE_TYPE_EPC_V01);
    }
  }

  QBI_LOG_D_5("Received media_type %d ip_type %d source %d operation %d enable %d as Set Req",
              req->media_type, req->ip_type, req->source, req->operation, req->enable);

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_build_wds2a_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WDS_GET_PROFILE_LIST_REQ request
    for a QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request

    @details

    @param txn
    @param profile_type

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_build_wds2a_req
(
  qbi_txn_s *txn,
  wds_profile_type_enum_v01 profile_type
)
{
  wds_get_profile_list_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (wds_get_profile_list_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_LIST_REQ_V01,
      qbi_svc_bc_ext_prov_ctx_v2_s_wds2a_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile_type_valid = TRUE;
  qmi_req->profile_type = profile_type;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_build_wds2a_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_wds2a_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_LIST_RESP for a
    QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_wds2a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_list_req_msg_v01                       *qmi_req = NULL;
  wds_get_profile_list_resp_msg_v01                      *qmi_rsp = NULL;
  qbi_svc_action_e                                       action   = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info    = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wds_get_profile_list_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_list_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      !(qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
        qmi_rsp->resp.error == QMI_ERR_EXTENDED_INTERNAL_V01 &&
        qmi_rsp->extended_error_code_valid && qmi_rsp->extended_error_code ==
            WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE_V01))
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
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
    if (qmi_rsp->profile_list_len > QMI_WDS_PROFILE_LIST_MAX_V01)
    {
      QBI_LOG_E_1("Invalid number of profiles %d", qmi_rsp->profile_list_len);
    }
    else
    {
      info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)
          qmi_txn->parent->info;

      qbi_svc_bc_ext_prov_ctx_populate_profile_list(&info->profile_list, qmi_rsp);
      if (qmi_req->profile_type == WDS_PROFILE_TYPE_EPC_V01)
      {
        info->num_of_profile_epc = qmi_rsp->profile_list_len;

        qmi_req = (wds_get_profile_list_req_msg_v01 *)
            qbi_qmi_txn_alloc_ret_req_buf(
            qmi_txn->parent, QBI_QMI_SVC_WDS,
            QMI_WDS_GET_PROFILE_LIST_REQ_V01,
            qbi_svc_bc_ext_prov_ctx_v2_s_wds2a_rsp_cb);
        QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

        qmi_req->profile_type_valid = TRUE;
        qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP_V01;
        action = QBI_SVC_ACTION_SEND_QMI_REQ;
      }
      else if (qmi_req->profile_type == WDS_PROFILE_TYPE_3GPP_V01)
      {
        info->num_of_profile_3gpp = qmi_rsp->profile_list_len;

        /* Using BC Service API to get the device capability instead of creating a new one */
        if (qbi_svc_bc_device_supports_3gpp2(qmi_txn->ctx))
        {
          qmi_req = (wds_get_profile_list_req_msg_v01 *)
              qbi_qmi_txn_alloc_ret_req_buf(
              qmi_txn->parent, QBI_QMI_SVC_WDS,
              QMI_WDS_GET_PROFILE_LIST_REQ_V01,
              qbi_svc_bc_ext_prov_ctx_v2_s_wds2a_rsp_cb);
          QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

          qmi_req->profile_type_valid = TRUE;
          qmi_req->profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
          action = QBI_SVC_ACTION_SEND_QMI_REQ;
        }
      }
      else
      {
        info->num_of_profile_3gpp2 = qmi_rsp->profile_list_len;
      }
    }
    if (action != QBI_SVC_ACTION_SEND_QMI_REQ)
    {
      QBI_LOG_D_0("Complete Get Profile List Request");
      QBI_LOG_D_3("Number Of EPC / 3GPP / 3GPP2 profiles %d / %d / %d",
                  info->num_of_profile_epc, info->num_of_profile_3gpp,
                  info->num_of_profile_3gpp2);

      action = qbi_svc_bc_ext_prov_ctx_v2_s_get_next_profile(
          qmi_txn->parent, qmi_txn);
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_wds2a_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_get_next_profile
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_SETTING_REQ for a
    QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_get_next_profile
(
  qbi_txn_s *txn,
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s         *req           = NULL;
  wds_delete_profile_req_msg_v01                         *qmi_req_del   = NULL;
  wds_get_profile_settings_req_msg_v01                   *qmi_req_wds2b = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info          = NULL;
  qbi_svc_bc_ext_operator_config_s                       operator_cfg   = { 0 };
  const uint8                                            *field         = NULL;
  char                                                   apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  qbi_svc_action_e                                       action         = QBI_SVC_ACTION_ABORT;

/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)txn->info;
  req = (qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)txn->req.data;

  QBI_LOG_D_2("profiles read %d profile matched %d",
              info->profiles_read, info->profile_matched);
  QBI_LOG_D_3("Index Of EPC / 3GPP / 3GPP2 Profiles -> %d / %d / %d",
              info->profile_index_epc, info->profile_index_3gpp,
              info->profile_index_3gpp2);

  /* For the first time this will never be true because we haven't called get settings on
  each profile index yet so qmi_rsp/qmi_txn param can be ignored for first iteration */
  if (info->profile_matched ==  TRUE && (info->profiles_read >= (info->num_of_profile_epc +
                                                                 info->num_of_profile_3gpp + info->num_of_profile_3gpp2)))
  {
    QBI_LOG_I_0("Matching profile found");
    /* We want to delete an existing profile */
    if (req->operation == QBI_SVC_MBIM_MS_CONTEXT_OPERATION_DELETE)
    {
      qmi_req_del = (wds_delete_profile_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          txn, QBI_QMI_SVC_WDS, QMI_WDS_DELETE_PROFILE_REQ_V01,
          qbi_svc_bc_ext_prov_ctx_v2_s_wds29_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_del);

      if (info->profile_found_epc == TRUE)
      {
        qmi_req_del->profile.profile_index = (uint8_t)info->profile_index_epc;
        qmi_req_del->profile.profile_type = WDS_PROFILE_TYPE_EPC_V01;
      }
      else if (info->profile_found_3gpp == TRUE)
      {
        qmi_req_del->profile.profile_index = (uint8_t)info->profile_index_3gpp;
        qmi_req_del->profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      }
      else if (info->profile_found_3gpp2 == TRUE)
      {
        qmi_req_del->profile.profile_index = (uint8_t)info->profile_index_3gpp2;
        qmi_req_del->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
      }

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
      QBI_LOG_I_2("Deleting profile at index %d type %d", qmi_req_del->profile.profile_index, qmi_req_del->profile.profile_type);

      info->operation_completed = TRUE;
      return action;
    }
    else if (req->operation == QBI_SVC_MBIM_MS_CONTEXT_OPERATION_DEFAULT) /* Request seems to either create or modify a exisiting profile */
    {
      if (info->profile_found_epc == TRUE)
      {
        QBI_LOG_D_0("Sending EPC modify request");
        action = qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req(
            qmi_txn->parent, WDS_PROFILE_TYPE_EPC_V01, info->profile_index_epc);
      }
      else
      {
        /* When modify a 3gpp profile, there may be a coresponding 3gpp2
           profile to be modified as well. Read 3gpp profile settings
           before modification, to be used for finding matching 3gpp2
           profile */
        if (info->profile_found_3gpp == TRUE)
        {
          QBI_LOG_D_0("Sending 3gpp modify request");
          action = qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req(
              qmi_txn->parent, WDS_PROFILE_TYPE_3GPP_V01, info->profile_index_3gpp);
        }
        else
        {
          if (info->profile_found_3gpp2 == TRUE)
          {
            QBI_LOG_D_0("Sending 3gpp2 modify request");
            action = qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req(
                qmi_txn->parent, WDS_PROFILE_TYPE_3GPP2_V01, info->profile_index_3gpp2);
          }
        }
      }
      info->operation_completed = TRUE;
    }
  }
  else if (info->profile_matched == FALSE && (info->profiles_read >= (info->num_of_profile_epc +
                                                                        info->num_of_profile_3gpp + info->num_of_profile_3gpp2)))
  {
    if (req->operation == QBI_SVC_MBIM_MS_CONTEXT_OPERATION_DEFAULT)
    {
      QBI_LOG_I_0("Profile did not match , Creating a new one");
      if (req->access_string.size != 0)
      {
        field = qbi_txn_req_databuf_get_field(
            qmi_txn->parent, &req->access_string, 0,
            QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
        QBI_CHECK_NULL_PTR_RET_FALSE(field);

        (void)qbi_util_utf16_to_ascii(
            field, req->access_string.size, apn_name, sizeof(apn_name));
        if (qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator(apn_name))
        {
          QBI_LOG_D_0("APN is Operator APN");
          qbi_nv_store_cfg_item_read(
              qmi_txn->ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
              &operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s));

          /* We dont allow user to create if class1/2 disable is TRUE */
          if ((operator_cfg.class1_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_SET &&
               qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1(qmi_txn->parent->ctx, apn_name) &&
               req->enable == TRUE) ||
              ((operator_cfg.class2_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_SET) &&
               (qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator(apn_name))))
          {
            QBI_LOG_D_0("Create : Unable to enable profile !! Bad request, Aborting");
            qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
                qmi_txn->parent, QMI_ERR_NONE_V01,
                0,
                QMI_ERR_NO_FREE_PROFILE_V01);
            action = QBI_SVC_ACTION_ABORT;
            return action;
          }
        }
      }
      action = qbi_svc_bc_ext_prov_ctx_v2_s_build_wds27_req(txn);
    }
    else
    {
      QBI_LOG_D_1("No Profile present with modem to perform operation %d,Aborting!",
                  req->operation);
      qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
          qmi_txn->parent, QMI_ERR_NONE_V01,
          0,
          QMI_ERR_INVALID_PROFILE_V01);
      action = QBI_SVC_ACTION_ABORT;
    }
  }
  else
  {
    QBI_LOG_D_0("Getting Next profile");
    /* Issue a query to retrieve the profile settings */
    qmi_req_wds2b = (wds_get_profile_settings_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_ext_prov_ctx_v2_s_wds2b_rsp_cb);
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

    QBI_LOG_I_2("Reading profile profile type %d at profile index %d ",
                qmi_req_wds2b->profile.profile_type,
                qmi_req_wds2b->profile.profile_index);
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_get_next_profile() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_wds2b_rsp_cb
===========================================================================*/
/*!
 @brief Handles a QMI_WDS_GET_PROFILE_SETTING_RESP for a
 MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request

 @details

 @param qmi_txn

 @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                                       action       = QBI_SVC_ACTION_ABORT;
  wds_get_profile_settings_resp_msg_v01                  *qmi_rsp     = NULL;
  wds_get_profile_settings_req_msg_v01                   *qmi_req     = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info        = NULL;
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s         *req         = NULL;
  qbi_svc_bc_ext_operator_config_s                       operator_cfg = { 0 };
  char                                                   apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  const uint8                                            *field       = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;
  qmi_req = (wds_get_profile_settings_req_msg_v01 *)qmi_txn->req.data;
  req = (qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)qmi_txn->parent->req.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Prov Context V2 : Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(qmi_txn->parent,
                                                                      qmi_rsp->resp.error, qmi_rsp->extended_error_code_valid,
                                                                      qmi_rsp->extended_error_code);

    if (QMI_ERR_EXTENDED_INTERNAL_V01 == qmi_rsp->resp.error &&
        qmi_rsp->extended_error_code_valid &&
        WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM_V01 ==
            qmi_rsp->extended_error_code)
    {
      QBI_LOG_D_0("Prov Context V2:: Bad profile. Continue to get next profile");
      info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)qmi_txn->parent->info;

      info->profiles_read++;
      action = qbi_svc_bc_ext_prov_ctx_v2_s_get_next_profile(qmi_txn->parent, qmi_txn);
    }
  }
  else
  {
    info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)qmi_txn->parent->info;

    info->profiles_read++;

    QBI_LOG_I_2("Set : Received profile %d/%d", info->profiles_read,
                info->num_of_profile_epc + info->num_of_profile_3gpp +
                info->num_of_profile_3gpp2);

    /* Compare APN name/PDP Type in profile with the one in response req */
    if (qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_name_matched(qmi_txn, qmi_rsp) &&
        qbi_svc_bc_ext_prov_ctx_v2_s_is_pdp_type_matched(qmi_txn, qmi_rsp))
    {

      QBI_LOG_I_1("APN and IP type match found for Modem Index %d",
                  qmi_req->profile.profile_index);
      QBI_LOG_I_3("Profile Found EPC / 3GPP / 3GPP2 -> %d / %d / %d",
                  info->profile_found_epc, info->profile_found_3gpp, info->profile_found_3gpp2);
      QBI_LOG_I_4("Profiles Read %d Number Of EPC / 3GPP / 3GPP2 Profiles -> %d / %d / %d",
                  info->profiles_read, info->num_of_profile_epc, info->num_of_profile_3gpp,
                  info->num_of_profile_3gpp2);

      if (req->access_string.size != 0)
      {
        field = qbi_txn_req_databuf_get_field(
            qmi_txn->parent, &req->access_string, 0,
            QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
        QBI_CHECK_NULL_PTR_RET_FALSE(field);

        (void)qbi_util_utf16_to_ascii(
            field, req->access_string.size, apn_name, sizeof(apn_name));

        if (qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator(apn_name))
        {
          QBI_LOG_D_0("APN is Operator APN");
          qbi_nv_store_cfg_item_read(
              qmi_txn->ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
              &operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s));

          /* We dont allow profile modification if class 1/2 disabled is TRUE */
          if ((operator_cfg.class1_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_SET &&
               qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1(qmi_txn->parent->ctx, apn_name) &&
               req->enable == TRUE) ||
              ((operator_cfg.class2_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_SET) &&
               (qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator(apn_name))))
          {
            QBI_LOG_D_0("Modify : Unable to enable profile !! Bad request, Aborting");
            qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
                qmi_txn->parent, QMI_ERR_NONE_V01,
                0,
                QMI_ERR_NO_FREE_PROFILE_V01);
            action = QBI_SVC_ACTION_ABORT;
            return action;
          }
        }
      }
      if ((info->profiles_read <= info->num_of_profile_epc) &&
          !info->profile_found_epc)
      {
        info->profile_found_epc = TRUE;
        info->profile_index_epc =
            info->profile_list.profile_index[info->profiles_read - 1];

        info->profiles_read = info->num_of_profile_epc;
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

      QBI_LOG_I_0("Updated Profile Data");
      QBI_LOG_I_3("Profile Found EPC / 3GPP / 3GPP2 -> %d / %d / %d",
                  info->profile_found_epc, info->profile_found_3gpp, info->profile_found_3gpp2);
      QBI_LOG_I_4("Profiles Read %d Number Of EPC / 3GPP / 3GPP2 Profiles -> %d / %d / %d",
                  info->profiles_read, info->num_of_profile_epc, info->num_of_profile_3gpp,
                  info->num_of_profile_3gpp2);

      /* Matching profile found in modem */
      info->profile_matched = TRUE;
    }
    action = qbi_svc_bc_ext_prov_ctx_v2_s_get_next_profile(qmi_txn->parent, qmi_txn);
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_wds2b_rsp_cb */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_is_pdp_type_matched
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_SETTING_REQ for a
    QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request

    @details

    @param qmi_txn
    @param qmi_rsp

    @return TRUE/FALSE
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_is_pdp_type_matched
(
  qbi_qmi_txn_s *qmi_txn,
  wds_get_profile_settings_resp_msg_v01 *profile_settings
)
{
  boolean                                        result = FALSE;
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *req   = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->parent->req.data);

  req = (qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)qmi_txn->parent->req.data;

  result = qbi_svc_bc_ext_match_pdp_type(req->ip_type, profile_settings);

  return result;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_is_pdp_type_matched */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class2
===========================================================================*/
/*!
    @brief Handles Request whether the APN name is Operator APN or not

    @details

    @param qmi_txn

    @return BOOLEAN
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class2
(
  const char *apn_name
)
{
  boolean class2_apn_found = FALSE;
  uint8   i                = 0;
  uint8   j                = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(apn_name);

  for (i = 0, j = 1; i < QBI_SVC_BC_EXT_OPERATOR_APN_COL; i++)
  {
    if (QBI_STRLEN(apn_name) &&
      (!QBI_STRNICMP(apn_name, qbi_svc_bc_ext_provisioned_contexts_v2_operator_apn[j][i],
      QBI_STRLEN(apn_name))))
    {
      class2_apn_found = TRUE;
      break;
    }
  }

  QBI_LOG_D_2("class2 apn found %d apn name length %d",
              class2_apn_found, QBI_STRLEN(apn_name));
  return class2_apn_found;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class2 */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1
===========================================================================*/
/*!
    @brief Handles Request whether the APN name is Operator APN or not

    @details

    @param qmi_txn

    @return BOOLEAN
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1
(
  qbi_ctx_s *ctx,
  const char *apn_name
)
{
  boolean                 class1_apn_found = FALSE;
  qbi_svc_bc_spdp_cache_s cache            = { 0 };
  uint8                   i                = 0;
  uint8                   j                = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(apn_name);

  /* Compare APN name */
  qbi_svc_bc_spdp_read_nv_store(ctx, &cache);
  for (i = 0, j = 0; i < QBI_SVC_BC_EXT_OPERATOR_APN_COL; i++)
  {
    if ((QBI_STRLEN(apn_name) &&
      (!QBI_STRNICMP(apn_name, qbi_svc_bc_ext_provisioned_contexts_v2_operator_apn[j][i],
      QBI_STRLEN(apn_name)))) ||
      (!QBI_STRNICMP(apn_name, qbi_svc_bc_ext_provisioned_contexts_v2_operator_apn[j][i],
      QBI_STRLEN(apn_name)) &&
      qbi_svc_bc_ext_prov_ctx_compare_imsi_for_operator(cache.imsi)))
    {
      class1_apn_found = TRUE;
      break;
    }
  }

  QBI_LOG_D_2("class1 apn found %d apn name length %d",
              class1_apn_found, QBI_STRLEN(apn_name));
  return class1_apn_found;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1 */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator
===========================================================================*/
/*!
    @brief Handles Request whether the APN name is Operator APN or not

    @details

    @param qmi_txn

    @return BOOLEAN
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator
(
  const char *apn_name
)
{
  uint8   i                  = 0;
  uint8   j                  = 0;
  boolean operator_apn_found = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(apn_name);

  for (i = 0; i < QBI_SVC_BC_EXT_OPERATOR_APN_ROW; i++)
  {
    for (j = 0; j < QBI_SVC_BC_EXT_OPERATOR_APN_COL; j++)
    {
      if (QBI_STRLEN(apn_name) &&
        !QBI_STRNICMP(apn_name, qbi_svc_bc_ext_provisioned_contexts_v2_operator_apn[i][j],
        QBI_STRLEN(apn_name)))
      {
        QBI_LOG_STR_1("%s is a Operator apn", apn_name);
        operator_apn_found = TRUE;
        break;
      }
    }
  }

  QBI_LOG_D_2("operator apn found %d apn name length %d",
              operator_apn_found, QBI_STRLEN(apn_name));
  return operator_apn_found;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_name_matched
===========================================================================*/
/*!
    @brief Handles Request whether the APN name matches with that of Request
    of QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_name_matched
(
  qbi_qmi_txn_s *qmi_txn,
  wds_get_profile_settings_resp_msg_v01 *profile_settings
)
{
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *req      = NULL;
  wds_get_profile_settings_req_msg_v01           *qmi_req  = NULL;
  const uint8                                    *field    = NULL;
  char                                            apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1]  = { 0, };

  boolean                                        apn_found = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  req = (qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)qmi_txn->parent->req.data;
  qmi_req = (wds_get_profile_settings_req_msg_v01 *)qmi_txn->req.data;
  /* Compare APN name */
  if (req->access_string.size != 0)
  {
    field = qbi_txn_req_databuf_get_field(
        qmi_txn->parent, &req->access_string, 0,
        QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
    QBI_CHECK_NULL_PTR_RET_FALSE(field);

    (void)qbi_util_utf16_to_ascii(
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
      apn_found = FALSE;
    }
  }
  else if ((qmi_req->profile.profile_type != WDS_PROFILE_TYPE_3GPP2_V01 &&
              profile_settings->apn_name_valid &&
              QBI_STRLEN(profile_settings->apn_name) != 0) ||
             (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP2_V01 &&
              profile_settings->apn_string_valid &&
              QBI_STRLEN(profile_settings->apn_string) != 0))
  {
    QBI_LOG_I_0("APN name does not match !");
    apn_found = FALSE;
  }

  QBI_LOG_I_2("For Profile type %d apn found is %d",
              qmi_req->profile.profile_type, apn_found);

  return apn_found;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_name_matched */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WDS_MODIFY_PROFILE_SETTINGS request

    @details

    @param txn
    @param profile_type

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req
(
  qbi_txn_s *txn,
  wds_profile_type_enum_v01 profile_type,
  uint32 index
)
{
  wds_modify_profile_settings_req_msg_v01                       *qmi_req         = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s             profile_settings;
  qbi_svc_action_e                                               action          = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  QBI_LOG_D_0("Initiating Prov Context V2 Modify Req");

  qmi_req = (wds_modify_profile_settings_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ_V01,
      qbi_svc_bc_ext_prov_ctx_v2_s_wds28_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile.profile_index = (uint8_t)index;
  qmi_req->profile.profile_type = profile_type;

  QBI_LOG_D_2("profile_type %d profile_index %d",
              qmi_req->profile.profile_type, qmi_req->profile.profile_index);

  if (!qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds28(
          qmi_req, &profile_settings, profile_type))
  {
    QBI_LOG_E_0("Couldn't collect profile setting pointers!");
  }
  else
  {
    action = qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile(
        txn, qmi_req->profile.profile_type, &profile_settings);
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_wds28_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_MODIFY_PROFILE_SETTINGS_RESP for
  MBIM_CID_PROVISIONED_CONTEXTS set request

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_wds28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                                       action       = QBI_SVC_ACTION_ABORT;
  wds_modify_profile_settings_resp_msg_v01               *qmi_rsp     = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info        = NULL;
  wds_modify_profile_settings_req_msg_v01                *qmi_req     = NULL;
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s         *req         = NULL;
  qbi_svc_bc_ext_module_prov_cache_s                     *cache       = NULL;
  qbi_svc_bc_ext_operator_config_s                       operator_cfg = { 0 };
  char                                                   apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1]     = { 0, };
  const uint8                                            *field       = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  qmi_rsp = (wds_modify_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;
  qmi_req = (wds_modify_profile_settings_req_msg_v01 *)qmi_txn->req.data;
  req = (qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)qmi_txn->parent->req.data;

  QBI_LOG_I_0("Rsp Callback For Modify Req");
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    if (qmi_rsp->extended_error_code_valid)
    {
      QBI_LOG_E_1("Extended error code %d", qmi_rsp->extended_error_code);
    }
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }
  else
  {
    cache = qbi_svc_bc_ext_module_prov_cache_get(qmi_txn->ctx, qmi_req->profile.profile_index);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);
    cache->context_flag = QBI_SVC_BC_EXT_CONTEXT_FLAG_USER_MODIFIED;

    info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)qmi_txn->parent->info;
    QBI_LOG_D_3("For Profile Type %d Found 3GPP / 3GPP2 -> %d / %d",
                qmi_req->profile.profile_type, info->profile_found_3gpp, info->profile_found_3gpp2);
    if (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_EPC_V01 && info->profile_found_3gpp)
    {
      action = qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req(
          qmi_txn->parent, WDS_PROFILE_TYPE_3GPP_V01, info->profile_index_3gpp);

      return action;
    }
    else if (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP_V01 && info->profile_found_3gpp2)
    {
      action = qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req(
          qmi_txn->parent, WDS_PROFILE_TYPE_3GPP2_V01, info->profile_index_3gpp2);

      return action;
    }
    else if (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_EPC_V01 && info->profile_found_3gpp2)
    {
      action = qbi_svc_bc_ext_prov_ctx_v2_s_build_wds28_req(
          qmi_txn->parent, WDS_PROFILE_TYPE_3GPP2_V01, info->profile_index_3gpp2);

      return action;
    }

    if (info->operation_completed == TRUE)
    {
      if (req->access_string.size != 0)
      {
        field = qbi_txn_req_databuf_get_field(
            qmi_txn->parent, &req->access_string, 0,
            QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
        QBI_CHECK_NULL_PTR_RET_FALSE(field);

        (void)qbi_util_utf16_to_ascii(
            field, req->access_string.size, apn_name, sizeof(apn_name));

        if (qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_operator(apn_name))
        {
          QBI_LOG_D_0("Requested APN is Operator APN");
          qbi_nv_store_cfg_item_read(
              qmi_txn->ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
              &operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s));

          if ((operator_cfg.class1_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_UNSET &&
               qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class1(qmi_txn->parent->ctx, apn_name) &&
               req->enable == FALSE))
          {
            QBI_LOG_D_0("Setting class1_disable to TRUE");
            operator_cfg.class1_disable = QBI_SVC_BC_EXT_OPERATOR_STATE_SET;
            if (!qbi_nv_store_cfg_item_write(
                    qmi_txn->ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
                    &operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s)))
            {
              QBI_LOG_E_0("Couldn't save operator_config NV for class 1!!");
            }
          }
          if (operator_cfg.class2_disable == QBI_SVC_BC_EXT_OPERATOR_STATE_UNSET &&
              qbi_svc_bc_ext_prov_ctx_v2_s_is_apn_class2(apn_name) &&
              req->enable == FALSE)
          {
            QBI_LOG_D_0("Setting class2_disable to TRUE");
            operator_cfg.class2_disable = QBI_SVC_BC_EXT_OPERATOR_STATE_SET;
            if (!qbi_nv_store_cfg_item_write(
                    qmi_txn->ctx, QBI_NV_STORE_CFG_ITEM_EXECUTOR_OPERATOR_CONFIG,
                    &operator_cfg, sizeof(qbi_svc_bc_ext_operator_config_s)))
            {
              QBI_LOG_E_0("Couldn't save operator_config NV for class 2!!");
            }
          }
        }
      }
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

        QBI_LOG_D_0("Completed profile modify operation."
                    "Triggering Check whether Class 2 disabled required.");

        cmd_in_progress_ignore_indication = TRUE;
        action = qbi_svc_bc_ext_prov_ctx_v2_s_pdc_prep_wds29_req(qmi_txn->parent);
      }
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_wds28_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_wds29_rsp_cb
===========================================================================*/
/*!
 @brief Delete Response for a MBIM_CID_MS_PROVISIONED_CONTEXTS_V2
 delete request

 @details

    @param qmi_txn

 @return qbi_svc_action_e QBI_SVC_ACTION_SEND_QMI_REQ on success,
 QBI_SVC_ACTION_ABORT on failure
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_wds29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_delete_profile_req_msg_v01                         *qmi_req = NULL;
  wds_delete_profile_resp_msg_v01                        *qmi_rsp = NULL;
  qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s             *info    = NULL;
  qbi_svc_action_e                                       action   = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  qmi_req = (wds_delete_profile_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_delete_profile_resp_msg_v01 *)qmi_txn->rsp.data;

  if ((qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01) &&
      (qmi_rsp->resp.error == QMI_ERR_EXTENDED_INTERNAL_V01) &&
      (qmi_rsp->extended_error_code_valid && (qmi_rsp->extended_error_code ==
                                                  WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE_V01 ||
                                              WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM_V01)))
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_bc_ext_prov_ctx_v2_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid,
        qmi_rsp->extended_error_code);
  }
  else
  {
    info = (qbi_svc_bc_ext_prov_ctx_v2_profiles_info_s *)
        qmi_txn->parent->info;
    if (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
        info->profile_found_3gpp == TRUE)
    {
      qmi_req = (wds_delete_profile_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_DELETE_PROFILE_REQ_V01,
          qbi_svc_bc_ext_prov_ctx_v2_s_wds29_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      qmi_req->profile.profile_index = (uint8_t)info->profile_index_3gpp;
      qmi_req->profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
      QBI_LOG_I_2("Deleting profile at index %d of type %d",
                  qmi_req->profile.profile_index,
                  qmi_req->profile.profile_type);
      return action;
    }
    else if (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP_V01 &&
               info->profile_found_3gpp2 == TRUE)
    {
      qmi_req = (wds_delete_profile_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_DELETE_PROFILE_REQ_V01,
          qbi_svc_bc_ext_prov_ctx_v2_s_wds29_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      qmi_req->profile.profile_index = (uint8_t)info->profile_index_3gpp2;
      qmi_req->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
      QBI_LOG_I_2("Deleting profile at index %d type %d", qmi_req->profile.profile_index,
                  qmi_req->profile.profile_type);
      return action;
    }
    else if (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
               info->profile_found_3gpp2 == TRUE)
    {
      qmi_req = (wds_delete_profile_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_DELETE_PROFILE_REQ_V01,
          qbi_svc_bc_ext_prov_ctx_v2_s_wds29_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      qmi_req->profile.profile_index = (uint8_t)info->profile_index_3gpp2;
      qmi_req->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
      QBI_LOG_I_2("Deleting profile at index %d type %d", qmi_req->profile.profile_index,
                  qmi_req->profile.profile_type);
      return action;
    }

    if (info->operation_completed == TRUE)
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

        QBI_LOG_D_0("Completed profile delete operation,Triggering Query");
        action = qbi_svc_bc_ext_prov_ctx_v2_q_req(qmi_txn->parent);
      }
    }
  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_wds29_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile
===========================================================================*/
/*!
 @brief Populates QMI profile settings for a MBIM_CID_PROVISIONED_CONTEXTS
 set request

 @details

 @param txn
 @param profile_type
    @param profile_index

 @return qbi_svc_action_e QBI_SVC_ACTION_SEND_QMI_REQ on success,
 QBI_SVC_ACTION_ABORT on failure
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile
(
  qbi_txn_s *txn,
  wds_profile_type_enum_v01                                profile_type,
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s      *profile_settings
)
{
  const qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *req   = NULL;
  qbi_svc_action_e                                     action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->apn_name_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->user_id_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->auth_password_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->app_user_data_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->common_apn_disabled_flag_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->common_apn_disabled_flag);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->common_pdp_type_valid);
  QBI_CHECK_NULL_PTR_RET_ABORT(profile_settings->common_pdp_type);

  req = (const qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *)txn->req.data;

  QBI_LOG_D_1("Populate Profile for Set Req with profile type %d", profile_type);
  QBI_LOG_D_5("Received media_type %d ip_type %d source %d operation %d enable %d as Set Req",
              req->media_type, req->ip_type, req->source, req->operation, req->enable);

  if (!qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile_compression(
          req, profile_type, profile_settings) ||
      !qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile_auth_protocol(
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
    *profile_settings->common_pdp_type_valid = TRUE;
    *profile_settings->common_pdp_type =
        qbi_svc_bc_ext_ip_type_to_pdp_type(req->ip_type);

    if (req->access_string.offset != 0 &&
        !qbi_svc_bc_ext_prov_ctx_v2_s_populate_str(
            txn, &req->access_string, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES,
            profile_settings->apn_name, QMI_WDS_APN_NAME_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for APN name!");
    }
    else if (req->username.offset != 0 &&
               !qbi_svc_bc_ext_prov_ctx_v2_s_populate_str(
            txn, &req->username, QBI_SVC_BC_USERNAME_MAX_LEN_BYTES,
            profile_settings->user_id, QMI_WDS_USER_NAME_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for username!");
    }
    else if (req->password.offset != 0 &&
               !qbi_svc_bc_ext_prov_ctx_v2_s_populate_str(
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
    *profile_settings->common_pdp_type_valid = TRUE;
    *profile_settings->common_pdp_type =
        qbi_svc_bc_ext_ip_type_to_pdp_type(req->ip_type);

    if (req->access_string.offset != 0 &&
        !qbi_svc_bc_ext_prov_ctx_v2_s_populate_str(
            txn, &req->access_string, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES,
            profile_settings->apn_name, QMI_WDS_APN_NAME_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for 3GPP2 APN string!");
    }
    else if (req->username.offset != 0 &&
               !qbi_svc_bc_ext_prov_ctx_v2_s_populate_str(
            txn, &req->username, QBI_SVC_BC_USERNAME_MAX_LEN_BYTES,
            profile_settings->user_id, QMI_WDS_USER_NAME_MAX_V01))
    {
      QBI_LOG_E_0("Couldn't populate QMI request for 3GPP2 username!");
    }
    else if (req->password.offset != 0 &&
               !qbi_svc_bc_ext_prov_ctx_v2_s_populate_str(
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

  if (req->enable == TRUE && (profile_type != WDS_PROFILE_TYPE_3GPP2_V01))
  {
    *profile_settings->common_apn_disabled_flag_valid = TRUE;
    *profile_settings->common_apn_disabled_flag = FALSE;
  }
  else if (req->enable == FALSE && (profile_type != WDS_PROFILE_TYPE_3GPP2_V01))
  {
    *profile_settings->common_apn_disabled_flag_valid = TRUE;
    *profile_settings->common_apn_disabled_flag = TRUE;
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP2_V01)
  {
    *profile_settings->common_apn_disabled_flag_valid = TRUE;
    *profile_settings->common_apn_disabled_flag = req->enable;

  }

  return action;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_populate_str
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
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_populate_str
(
  qbi_txn_s *txn,
  const qbi_mbim_offset_size_pair_s *field_desc,
  uint32                             field_max_size,
  char *qmi_field,
  uint32                             qmi_field_size
)
{
  const uint8 *req_str_utf16 = NULL;
  uint32      bytes_copied   = 0;
  boolean     success        = FALSE;
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
} /* qbi_svc_bc_ext_prov_ctx_v2_s_populate_str() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds27
===========================================================================*/
/*!
  @brief Collect pointers to all relevant TLVs in
  QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ for a MBIM_CID_MS_PROVISIONED_CONTEXTS_V2
  set request

  @details

  @param qmi_req
  @param profile_settings
  @param profile_type

  @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds27
(
  wds_create_profile_req_msg_v01                     *qmi_req,
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s *profile_settings,
  wds_profile_type_enum_v01                           profile_type
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_req);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  QBI_LOG_D_1("Get Profile Setting Ptrs For Profile Type %d", profile_type);
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
    profile_settings->common_apn_disabled_flag_valid = &qmi_req->apn_disabled_flag_valid;
    profile_settings->common_apn_disabled_flag = &qmi_req->apn_disabled_flag;
    profile_settings->common_pdp_type_valid = &qmi_req->pdp_type_valid;
    profile_settings->common_pdp_type = &qmi_req->pdp_type;
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
    profile_settings->common_apn_disabled_flag_valid = &qmi_req->apn_disabled_flag_valid;
    profile_settings->common_apn_disabled_flag = &qmi_req->apn_disabled_flag;
    profile_settings->common_pdp_type_valid = &qmi_req->pdp_type_valid;
    profile_settings->common_pdp_type = &qmi_req->pdp_type;
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
    profile_settings->common_apn_disabled_flag_valid = &qmi_req->apn_enabled_3gpp2_valid;
    profile_settings->common_apn_disabled_flag = &qmi_req->apn_enabled_3gpp2;
    profile_settings->common_pdp_type_valid = &qmi_req->pdn_type_valid;
    profile_settings->common_pdp_type = &qmi_req->pdn_type;
  }

  return TRUE;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds27() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds28
===========================================================================*/
/*!
  @brief Collect pointers to all relevant TLVs in
  QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ for a MBIM_CID_MS_PROVISIONED_CONTEXTS_V2
  set request

  @details

  @param qmi_req
  @param profile_settings
  @param profile_type

  @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds28
(
  wds_modify_profile_settings_req_msg_v01            *qmi_req,
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s *profile_settings,
  wds_profile_type_enum_v01                           profile_type
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_req);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  QBI_LOG_D_1("Prepare Common Req Struct For Profile Type %d", profile_type);
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
    profile_settings->common_apn_disabled_flag_valid = &qmi_req->apn_disabled_flag_valid;
    profile_settings->common_apn_disabled_flag = &qmi_req->apn_disabled_flag;
    profile_settings->common_pdp_type_valid = &qmi_req->pdp_type_valid;
    profile_settings->common_pdp_type = &qmi_req->pdp_type;
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
    profile_settings->common_apn_disabled_flag_valid = &qmi_req->apn_disabled_flag_valid;
    profile_settings->common_apn_disabled_flag = &qmi_req->apn_disabled_flag;
    profile_settings->common_pdp_type_valid = &qmi_req->pdp_type_valid;
    profile_settings->common_pdp_type = &qmi_req->pdp_type;
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
    profile_settings->common_apn_disabled_flag_valid = &qmi_req->apn_enabled_3gpp2_valid;
    profile_settings->common_apn_disabled_flag = &qmi_req->apn_enabled_3gpp2;
    profile_settings->common_pdp_type_valid = &qmi_req->pdn_type_valid;
    profile_settings->common_pdp_type = &qmi_req->pdn_type;
  }

  return TRUE;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_get_profile_settings_ptrs_wds28() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile_compression
===========================================================================*/
/*!
  @brief Populates the compression TLVs of a
  QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ or QMI_WDS_CREATE_PROFILE_REQ for
  a MBIM_CID_MS_PROVISIONED_CONTEXTS_V2 set request

  @details

  @param req
  @param profile_type
  @param profile_settings

  @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile_compression
(
  const qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *req,
  wds_profile_type_enum_v01                                profile_type,
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s      *profile_settings
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
} /* qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile_compression() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile_auth_protocol
===========================================================================*/
/*!
  @brief Populates the authentication preference/protocol TLV of a
  QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ or QMI_WDS_CREATE_PROFILE_REQ using
  the information from the MBIM_CID_MS_PROVISIONED_CONTEXTS_V2 set request

  @details

  @param txn
  @param req
  @param profile_type
  @param profile_settings

  @return boolean TRUE on success, FALSE on failure. May set txn->status
  if the request contained an invalid value
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile_auth_protocol
(
  qbi_txn_s *txn,
  const qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s    *req,
  wds_profile_type_enum_v01                                profile_type,
  qbi_svc_bc_ext_prov_ctx_v2_profile_settings_ptrs_s      *profile_settings
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
        qbi_svc_bc_ext_prov_ctx_connect_mbim_auth_pref_to_qmi_auth_protocol(
        req->auth_protocol);
  }
  else if (profile_type == WDS_PROFILE_TYPE_3GPP_V01)
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->authentication_preference_valid);
    QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings->authentication_preference);
    *(profile_settings->authentication_preference_valid) = TRUE;
    *(profile_settings->authentication_preference) =
        qbi_svc_bc_ext_connect_mbim_auth_pref_to_qmi_auth_pref(req->auth_protocol);
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

  QBI_LOG_D_1("Auth Protocol Returned %d", success);
  return success;
} /* qbi_svc_bc_ext_prov_ctx_v2_s_populate_profile_auth_protocol() */


/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_populate_profile_list
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
static boolean qbi_svc_bc_ext_prov_ctx_populate_profile_list
(
  qbi_svc_bc_ext_prov_ctx_profile_list_s *profile_list,
  wds_get_profile_list_resp_msg_v01      *qmi_rsp
)
{
  uint32  i;
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
    QBI_LOG_D_1("Adding %d Profiles To Local List", qmi_rsp->profile_list_len);
    for (i = 0; i < qmi_rsp->profile_list_len; i++)
    {
      profile_list->profile_type[profile_list->num_of_profile + i] =
          qmi_rsp->profile_list[i].profile_type;
      profile_list->profile_index[profile_list->num_of_profile + i] =
          qmi_rsp->profile_list[i].profile_index;
      QBI_LOG_I_2("profile index %d profile type %d", qmi_rsp->profile_list[i].profile_index,
                  qmi_rsp->profile_list[i].profile_type);
    }
    profile_list->num_of_profile += qmi_rsp->profile_list_len;
    result = TRUE;
    QBI_LOG_D_1("Number of profiles added to list %d", profile_list->num_of_profile);
  }

  return result;
} /* qbi_svc_bc_ext_prov_ctx_populate_profile_list() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_prov_ctx_pdc2f_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PDC_REFRESH_IND_V01, looking for operating mode
    changes to trigger a QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2
    event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_prov_ctx_pdc2f_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e              action   = QBI_SVC_ACTION_ABORT;
  const pdc_refresh_ind_msg_v01 *qmi_ind = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const pdc_refresh_ind_msg_v01 *)ind->buf->data;

  switch (qmi_ind->refresh_event)
  {
  case PDC_EVENT_REFRESH_START_V01:
    break;
  case PDC_EVENT_REFRESH_COMPLETE_V01:
    if (qmi_ind->slot_id_valid)
    {
      // delete user defined profiles and reset cache
      QBI_LOG_E_0("PDC refresh completed. Clean up user defined profiles.");
      action = qbi_svc_bc_ext_prov_ctx_v2_s_pdc_wds29_req(ind->txn);
    }
    break;
  default:
    break;
  }

  QBI_LOG_D_1("Prov Contxt V2 : Received PDC 0x2f IND CB %d", action);
  return action;
} /* qbi_svc_bc_ext_prov_ctx_pdc2f_ind_cb() */
