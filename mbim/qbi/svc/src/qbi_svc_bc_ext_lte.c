/*!
  @file
  qbi_svc_bc_ext_lte.c

  @brief
  Basic Connectivity Extension device service definitions, based on LTE ATTACH
  feature requirements.
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
--------  ---  ---------------------------------------------------------------
09/20/18  ar   Modified user defined profile delete logic in LTE ATTACH CONFIG
09/16/18  sk   Added case-insensitive profile match logic.
03/19/18  vk   LTE Attach config HLK failure when NV 850 is set to PS only
03/07/18  rv   Avoid waiting for NAS attach indication in case of 3G
11/29/17  rv   Avoiding deregister in case of 3G for LTE ATTACH and SPDP
10/28/17  rv   Avoid deregister in case of 3G
10/27/17  nk   Moved lte attach features from bc_ext.c
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_bc_ext.h"
#include "qbi_svc_bc_ext_prov.h"
#include "qbi_svc_bc_ext_lte.h"
#include "qbi_svc_bc_ext_mbim.h"
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

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG
    @{ */
#define QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE     (3)

/*! Map QMI WDS service ID to session ID */
#define QBI_SVC_BC_EXT_INVALID_PROFILE_INDEX          (0)

/*=============================================================================

  Private Typedefs

=============================================================================*/

static boolean cmd_in_progress_ignore_indication = FALSE;

/*! @addtogroup MBIM_CID_MS_LTE_ATTACH_CONFIG
    @{ */

/*! Collection of pointers to relevant TLVs in
    QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ and QMI_WDS_CREATE_PROFILE_REQ */
typedef struct
{
  uint8_t                           roaming_disallowed_valid;
  uint8_t                           roaming_disallowed;
  uint8_t                           pdp_type_valid;
  wds_pdp_type_enum_v01             pdp_type;
  uint8_t                          *authentication_preference_valid;
  wds_auth_pref_mask_v01           *authentication_preference;
  uint8_t                          *pdp_data_compression_type_valid;
  wds_pdp_data_compr_type_enum_v01 *pdp_data_compression_type;
  uint8_t                          *pdp_hdr_compression_type_valid;
  wds_pdp_hdr_compr_type_enum_v01  *pdp_hdr_compression_type;
} qbi_svc_bc_ext_lte_attach_config_profile_settings_s;

/*! Tracking information for retrieving profiles */
typedef struct
{
  /*! Maximum number of attached PDNs supported by the device. */
  uint32 max_supported_profile_num;

  /*! Number of sets of the attach_pdn_list elements */
  uint32 num_of_profile;

  /*! Number of profiles that have been retrieved so far */
  uint32 profiles_read;

  /*! Current element for which profile info is being retrieved */
  uint32 element_read;

  /*! Profile IDs to matched element mapping. Its size will be
  element_count of qbi_svc_bc_ext_lte_attach_config_info_rsp_s*/
  uint32 element_match_index[QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE];

  /*! PDN profile IDs to attach to, listed in order of decreasing priority. */
  uint16 profile_index[QMI_WDS_ATTACH_PDN_MAX_V01];

  /*! Keeps track of current RAT. 
      TRUE if LTE else FALSE */
  uint32 rat_is_lte;

  /*! Maximum number of user defined profiles */
  uint32 max_user_defined_profiles;

  /*! Current index user defined profile*/
  uint32 index_user_defined_profiles;

  /*! Array of user defined profiles present in Modem*/
  uint32 user_defined_profiles[QMI_WDS_ATTACH_PDN_MAX_V01];

   /*! Flag is used to trigger the LTE Attach Query if
     Attach PDN List is same as set PDN list */
  boolean skip_attach_pdn_list_ind;

  /*! Used to store the Attach Pdn List */
  uint32 current_attach_pdn_list[QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE];

  /*! Attach PDN List length */
  uint32 current_attach_pdn_list_len;
} qbi_svc_bc_ext_lte_attach_config_info_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_LTE_ATTACH_STATUS
    @{ */

/*! Collection of pointers to relevant TLVs in
    QMI_WDS_GET_PROFILE_SETTINGS_REQ */
typedef struct
{
  uint8_t                           pdp_type_valid;
  wds_pdp_type_enum_v01             pdp_type;
  uint8_t                           authentication_preference_valid;
  wds_auth_pref_mask_v01            authentication_preference;
  uint8_t                           pdp_data_compression_type_valid;
  wds_pdp_data_compr_type_enum_v01  pdp_data_compression_type;
  uint8_t                           pdp_hdr_compression_type_valid;
  wds_pdp_hdr_compr_type_enum_v01   pdp_hdr_compression_type;
  uint8_t                           apn_name_valid;
  char                              apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  uint8_t                           username_valid;
  char                              username[QMI_WDS_USER_NAME_MAX_V01 + 1];
  uint8_t                           password_valid;
  char                              password[QMI_WDS_PASSWORD_MAX_V01 + 1];
} qbi_svc_bc_ext_lte_attach_status_profile_settings_s;

/*! Tracking information for retrieving profiles */
typedef struct
{
  /*! LTE attach state. */
  uint32  lte_attach_state;

  /*! Registeration status. */
  uint32  status_registered;

  /*! Maximum number of attached PDNs supported by the device. */
  uint32  max_supported_profile_num;

  /*! Number of sets of the LTE attach PDNs */
  uint32  num_of_profile;

  /*! Number of profiles that have been retrieved so far */
  uint32  profiles_read;

  /*! PDN profile index of LTE attach PDNs. */
  uint16  profile_index[QMI_WDS_ATTACH_PDN_MAX_V01];

  /*! PDN profile APN for LTE attach PDN. */
  char    apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];

  /*! PDN profile IP type for LTE attach PDN. */
  wds_ip_support_type_enum_v01 ip_type;
} qbi_svc_bc_ext_lte_attach_status_info_s;

/*! @} */

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static boolean qbi_svc_bc_ext_lte_attach_match_string
(
  const uint8   qmi_str_valid,
  const char *qmi_str,
  const char *str,
  boolean is_case_insensitive
);

static uint32 qbi_svc_bc_ext_lte_attach_roam_type_to_roam_flag
(
  const uint32  roam_type
);

/*! @addtogroup MBIM_CID_MS_LTE_ATTACH_CONFIG
    @{ */

static uint32 qbi_svc_bc_ext_lte_attach_qmi_profile_to_mbim_compression
(
  wds_profile_type_enum_v01                    profile_type,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
);

static uint32 qbi_svc_bc_ext_lte_attach_qmi_profile_to_mbim_auth_proto
(
  wds_profile_type_enum_v01                    profile_type,
  const wds_get_profile_settings_resp_msg_v01 *qmi_rsp
);

qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_nas34_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_q_wds92_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds93_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_q_wds94_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds9f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_q_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_build_wds28_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_build_wds29_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_ext_lte_attach_config_s_get_profile_list
(
  qbi_txn_s                                *txn,
  wds_get_lte_attach_pdn_list_resp_msg_v01 *qmi_rsp
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_operation_factory_restore
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_operation_default
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_ext_lte_attach_config_s_register_for_detach_attach
(
  qbi_txn_s *txn
);

static void qbi_svc_bc_ext_lte_attach_config_set_mbim_error_status
(
  qbi_txn_s                          *txn,
  qmi_error_type_v01                  qmi_error,
  uint8_t                             qmi_error_ds_ext_valid,
  wds_ds_extended_error_code_enum_v01 qmi_error_ds_ext
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds94_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_build_wds93_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_bc_ext_lte_attach_config_s_is_profile_match_pending
(
  qbi_svc_bc_ext_lte_attach_config_info_s *info
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds95_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_build_nas4d_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_open_configure_qmi_inds_uim2e_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup MBIM_CID_MS_LTE_ATTACH_STATUS
    @{ */

qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_e_dsd26_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_dsd24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_wds85_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_wds92_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_wds94_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*=============================================================================

Private Function Definitions

=============================================================================*/

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_match_string
===========================================================================*/
/*!
    @brief Compares modem and apps strings

    @details

    @param qmi_str_valid
    @param qmi_str
    @param field
    @param field_size
    @param is_case_insensitive

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_match_string
(
  const uint8   qmi_str_valid,
  const char *qmi_str,
  const char *str,
  boolean is_case_insensitive
)
{
  uint32  qmi_str_size = 0;
  uint32  str_size     = 0;
  boolean match_found  = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_str);

  if (qmi_str_valid)
  {
    qmi_str_size = QBI_STRLEN(qmi_str);
    str_size = QBI_STRLEN(str);

    QBI_LOG_STR_2("LTEAttachStatus::Q: Matching %s <==> %s", qmi_str, str);
    // If unequal size return FALSE immediately.
    if (str_size != qmi_str_size)
    {
      QBI_LOG_I_2("LTEAttachStatus::Q: String length did not match. %d <==> %d",
                  qmi_str_size, str_size);
      match_found = FALSE;
    }
    // If size of both string equals, return TRUE immediately.
    else if (!str_size && !qmi_str_size)
    {
      QBI_LOG_I_0("LTEAttachStatus::Q: Matched NULL string");
      match_found = TRUE;
    }
    else if(is_case_insensitive)
    {
      match_found = !QBI_STRNICMP(str, qmi_str, str_size);
      QBI_LOG_I_1("LTEAttachStatus::Q: Match status %d", match_found);
    }
  }

  return match_found;
} /* qbi_svc_bc_ext_lte_attach_match_string() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_roam_type_to_roam_flag
===========================================================================*/
/*!
    @brief Maps MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL locally

    @details

    @param qmi_str_valid
    @param qmi_str
    @param field
    @param field_size

    @return boolean
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_ext_lte_attach_roam_type_to_roam_flag
(
  const uint32  roam_type
)
{
/*-------------------------------------------------------------------------*/
  switch (roam_type)
  {
  case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_HOME:
    return QBI_SVC_BC_EXT_ROAMING_FLAG_HOME;
  case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_PARTNER:
    return QBI_SVC_BC_EXT_ROAMING_FLAG_PARTNER;
  case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_NON_PARTNER:
    return QBI_SVC_BC_EXT_ROAMING_FLAG_NON_PARTNER;
  }

  return QBI_SVC_BC_EXT_ROAMING_FLAG_HOME;
} /* qbi_svc_bc_ext_lte_attach_roam_type_to_roam_flag() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_lte_attach_config_update_cache
===========================================================================*/
/*!
    @brief Updates Basic Connectivity Extension device service's cache

    @details

    @param txn
    @param qmi_rsp

    @return Pointer to the cache
*/
/*=========================================================================*/
static qbi_svc_bc_ext_module_prov_cache_s* qbi_svc_bc_ext_lte_attach_config_update_cache
(
  qbi_txn_s *txn,
  const boolean is_user_defined,
  const qbi_svc_bc_ext_lte_attach_config_profile_settings_s profile_settings
)
{
  qbi_svc_bc_ext_lte_attach_config_info_s *info       = NULL;
  qbi_mbim_offset_size_pair_s             *field_desc = NULL;
  qbi_svc_bc_ext_lte_attach_context_s     *context    = NULL;
  qbi_svc_bc_ext_module_prov_cache_s      *cache      = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(txn);
  QBI_CHECK_NULL_PTR_RET_NULL(txn->info);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  cache = qbi_svc_bc_ext_module_prov_cache_get(txn->ctx,
                                   info->profile_index[info->profiles_read]);
  QBI_CHECK_NULL_PTR_RET_NULL(cache);

  if (txn->cmd_type == QBI_MSG_CMD_TYPE_SET ||
      (txn->cmd_type == QBI_TXN_CMD_TYPE_INTERNAL &&
       txn->req.data != NULL))
  {
    // This is set case. update cache.
    qbi_svc_bc_ext_lte_attach_config_s_req_s *req =
        (qbi_svc_bc_ext_lte_attach_config_s_req_s *)txn->req.data;
    QBI_CHECK_NULL_PTR_RET_NULL(req);

    switch (req->operation)
    {
    case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_OPERATION_DEFAULT:
      field_desc = (qbi_mbim_offset_size_pair_s *)((uint8_t *)req +
                                                   sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) +
                                                   sizeof(qbi_mbim_offset_size_pair_s) * info->element_read);
      QBI_CHECK_NULL_PTR_RET_NULL(field_desc);

      context = (qbi_svc_bc_ext_lte_attach_context_s *)
          qbi_txn_req_databuf_get_field(txn, field_desc, 0, field_desc->size);
      QBI_CHECK_NULL_PTR_RET_FALSE(context);

      cache->source = context->source;
      cache->roaming = context->roaming;
      break;
    case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_OPERATION_RESTORE_FACTORY:
      cache->source = QBI_SVC_MBIM_MS_CONTEXT_SOURCE_MODEM;
      break;
    }

    if (profile_settings.pdp_type_valid)
    {
      QBI_CHECK_NULL_PTR_RET_FALSE(context);
      qbi_svc_bc_ext_update_cache_ip_type(txn, req->operation,
                                               cache, context->ip_type, profile_settings.pdp_type);
    }
    else
    {
      QBI_LOG_E_0("LTEAttachConfig::Cache update:E: IP type TLV missing.");
    }

    if (is_user_defined)
    {
      cache->context_flag = QBI_SVC_BC_EXT_CONTEXT_FLAG_USER_DEFINED;
    }
    else
    {
      if (QBI_SVC_BC_EXT_CONTEXT_FLAG_USER_DEFINED != cache->context_flag)
      {
        cache->context_flag = QBI_SVC_BC_EXT_CONTEXT_FLAG_USER_MODIFIED;
      }
    }
  }
  else
  {
    if (!cache->lte_active)
    {
      cache->context_flag = QBI_SVC_BC_EXT_CONTEXT_FLAG_MODEM;
    }

    if (QBI_SVC_BC_EXT_CONTEXT_FLAG_MODEM == cache->context_flag)
    {
      cache->source = QBI_SVC_MBIM_MS_CONTEXT_SOURCE_MODEM;
    }

    if (profile_settings.pdp_type_valid)
    {
      qbi_svc_bc_ext_update_cache_ip_type(txn, 0, cache,
                                               0, profile_settings.pdp_type);
    }
    else
    {
      QBI_LOG_E_0("LTEAttachConfig::Cache update:E: IP type TLV missing.");
    }
  }

  cache->lte_active = TRUE;
  cache->roaming_flag |= qbi_svc_bc_ext_lte_attach_roam_type_to_roam_flag(cache->roaming);

  QBI_LOG_D_6("Cache update:: ip_type: %d, source: %d, roaming: %d, lte_active: %d, "
              "context_flag: %d, roaming_flag: %d", cache->ip_type, cache->source, cache->roaming,
              cache->lte_active, cache->context_flag, cache->roaming_flag);

  qbi_svc_bc_ext_update_nv_store(txn->ctx);

  return cache;
} /* qbi_svc_bc_ext_lte_attach_config_update_cache() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_roam_flag_to_roam_type
===========================================================================*/
/*!
    @brief Compares modem and apps strings

    @details

    @param qmi_str_valid
    @param qmi_str
    @param field
    @param field_size

    @return boolean
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_ext_roam_flag_to_roam_type
(
  const uint32 roam_flag,
  const uint32 roam_type
)
{
/*-------------------------------------------------------------------------*/
  switch (roam_type)
  {
  case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_HOME:
    if (roam_flag & QBI_SVC_BC_EXT_ROAMING_FLAG_HOME)
    {
      return QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_HOME;
    }
    break;
  case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_PARTNER:
    if (roam_flag & QBI_SVC_BC_EXT_ROAMING_FLAG_PARTNER)
    {
      return QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_PARTNER;
    }
    break;
  case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_NON_PARTNER:
    if (roam_flag & QBI_SVC_BC_EXT_ROAMING_FLAG_NON_PARTNER)
    {
      return QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_NON_PARTNER;
    }
    break;
  }

  return QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_HOME;
} /* qbi_svc_bc_ext_roam_flag_to_roam_type() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_qmi_profile_to_mbim_compression
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
static uint32 qbi_svc_bc_ext_lte_attach_qmi_profile_to_mbim_compression
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
} /* qbi_svc_bc_ext_lte_attach_qmi_profile_to_mbim_compression() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_qmi_profile_to_mbim_auth_proto
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
static uint32 qbi_svc_bc_ext_lte_attach_qmi_profile_to_mbim_auth_proto
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
    if (qmi_rsp->authentication_preference == QMI_WDS_MASK_AUTH_PREF_CHAP_V01)
    {
      mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_CHAP;
    }
    else if (qmi_rsp->authentication_preference == QMI_WDS_MASK_AUTH_PREF_PAP_V01)
    {
      mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_PAP;
    }
    else if (qmi_rsp->authentication_preference ==
                   (QMI_WDS_MASK_AUTH_PREF_CHAP_V01 | QMI_WDS_MASK_AUTH_PREF_PAP_V01))
    {
      mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_AUTO;
    }
  }
  else
  {
    QBI_LOG_E_1("Invalid profile type %d", profile_type);
  }

  return mbim_auth_proto;
} /* qbi_svc_bc_ext_lte_attach_qmi_profile_to_mbim_auth_proto() */

/*===========================================================================
  FUNCTION: qbi_svc_lte_attach_config_add_context_to_rsp
===========================================================================*/
/*!
    @brief Allocates and populates a MBIM_MS_LTE_ATTACH_CONTEXT structure 
    on the response

    @details

    @param txn
    @param field_desc
    @param qmi_rsp

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_lte_attach_config_add_context_to_rsp
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_req_msg_v01                *qmi_req         = NULL;
  wds_get_profile_settings_resp_msg_v01               *qmi_rsp         = NULL;
  boolean                                             success          = FALSE;
  uint32                                              initial_offset   = 0;
  qbi_mbim_offset_size_pair_s                         *field_desc      = NULL;
  qbi_svc_bc_ext_lte_attach_context_s                 *context         = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s             *info            = NULL;
  qbi_svc_bc_ext_module_prov_cache_s                  *cache           = NULL;
  qbi_svc_bc_ext_lte_attach_config_profile_settings_s profile_settings = { 0 };
  uint32                                              roam_type        = QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_HOME;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->rsp.data);

  qmi_req = (wds_get_profile_settings_req_msg_v01 *)qmi_txn->req.data;
  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;
  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;

  cache = qbi_svc_bc_ext_module_prov_cache_get(qmi_txn->parent->ctx,
                                   qmi_req->profile.profile_index);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  profile_settings.roaming_disallowed_valid = qmi_rsp->roaming_disallowed_valid;
  profile_settings.roaming_disallowed = qmi_rsp->roaming_disallowed;

  profile_settings.pdp_type_valid = qmi_rsp->pdp_type_valid;
  profile_settings.pdp_type = qmi_rsp->pdp_type;

  qbi_svc_bc_ext_lte_attach_config_update_cache(
      qmi_txn->parent, FALSE, profile_settings);

  roam_type = qbi_svc_bc_ext_roam_flag_to_roam_type(
      cache->roaming_flag, info->element_read);
  QBI_LOG_D_1(" LTEAttachConfig::Q: Adding response for roaming type %d", roam_type);

  if (!info->element_match_index[roam_type])
  {
    initial_offset = qmi_txn->parent->infobuf_len_total;
    field_desc = (qbi_mbim_offset_size_pair_s *)
        ((uint8 *)qmi_txn->parent->rsp.data +
         sizeof(qbi_svc_bc_ext_lte_attach_config_info_rsp_s) +
         sizeof(qbi_mbim_offset_size_pair_s) * roam_type);
    QBI_CHECK_NULL_PTR_RET_FALSE(field_desc);

    context = (qbi_svc_bc_ext_lte_attach_context_s *)
        qbi_txn_rsp_databuf_add_field(qmi_txn->parent, field_desc, 0,
                                      sizeof(qbi_svc_bc_ext_lte_attach_context_s), NULL);
    QBI_CHECK_NULL_PTR_RET_FALSE(context);

    context->ip_type = cache->ip_type;
    context->source = cache->source;
    context->roaming = roam_type;

    context->compression =
        qbi_svc_bc_ext_lte_attach_qmi_profile_to_mbim_compression(
        WDS_PROFILE_TYPE_EPC_V01, qmi_rsp);
    context->auth_protocol =
        qbi_svc_bc_ext_lte_attach_qmi_profile_to_mbim_auth_proto(
        WDS_PROFILE_TYPE_3GPP_V01, qmi_rsp);

    success = TRUE;

    /* Populate the DataBuffer */
    if (qmi_rsp->apn_name_valid &&
        !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
            qmi_txn->parent, &context->access_string, initial_offset,
            QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES, qmi_rsp->apn_name,
            sizeof(qmi_rsp->apn_name)))
    {
      success = FALSE;
      QBI_LOG_E_0("Couldn't add 3GPP AccessString to response!");
    }

    if (qmi_rsp->username_valid &&
        !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
            qmi_txn->parent, &context->username, initial_offset,
            QBI_SVC_BC_USERNAME_MAX_LEN_BYTES, qmi_rsp->username,
            sizeof(qmi_rsp->username)))
    {
      success = FALSE;
      QBI_LOG_E_0("Couldn't add 3GPP Username to response!");
    }

    if (qmi_rsp->password_valid &&
        !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
            qmi_txn->parent, &context->password, initial_offset,
            QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES,
            qmi_rsp->password, sizeof(qmi_rsp->password)))
    {
      success = FALSE;
      QBI_LOG_E_0("Couldn't add 3GPP Password to response!");
    }

    QBI_LOG_D_5(" LTEAttachConfig::Q: ip_type: %d, roaming: %d, source: %d, "
                "compression: %d, auth_protocol: %d", context->ip_type, context->roaming,
                context->source, context->compression, context->auth_protocol);

    if (success)
    {
      /* Update the size field to include DataBuffer items */
      field_desc->size = qmi_txn->parent->infobuf_len_total - initial_offset;
      success = qbi_txn_rsp_databuf_consolidate(qmi_txn->parent);
      info->element_match_index[roam_type] = qmi_req->profile.profile_index;
    }
  }
  else
  {
    success = TRUE;
  }

  info->element_read++;
  info->profiles_read++;

  return success;
} /* qbi_svc_lte_attach_config_add_context_to_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_lte_attach_config_add_dummy_context_to_rsp
===========================================================================*/
/*!
    @brief Allocates and populates a MBIM_MS_LTE_ATTACH_CONTEXT structure 
    on the response

    @details

    @param txn
    @param field_desc
    @param qmi_rsp

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static void qbi_svc_lte_attach_config_add_dummy_context_to_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_lte_attach_config_info_s *info          = NULL;
  qbi_svc_bc_ext_lte_attach_context_s     *context       = NULL;
  qbi_mbim_offset_size_pair_s             *field_desc    = NULL;
  uint32                                  initial_offset = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->info);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  for (info->element_read = 0;
       info->element_read < QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE;
       info->element_read++)
  {
    if (!info->element_match_index[info->element_read])
    {
      QBI_LOG_D_1("LTEAttachConfig::Q: Adding dummy context to roam type: %d",
                  info->element_read);

      field_desc = (qbi_mbim_offset_size_pair_s *)((uint8 *)txn->rsp.data +
                                                   sizeof(qbi_svc_bc_ext_lte_attach_config_info_rsp_s) +
                                                   sizeof(qbi_mbim_offset_size_pair_s) * info->element_read);

      initial_offset = txn->infobuf_len_total;
      context = (qbi_svc_bc_ext_lte_attach_context_s *)
          qbi_txn_rsp_databuf_add_field(txn, field_desc, 0,
                                        sizeof(qbi_svc_bc_ext_lte_attach_context_s), NULL);
      QBI_CHECK_NULL_PTR_RET(context);

      QBI_MEMSET(context, 0, sizeof(qbi_svc_bc_ext_lte_attach_context_s));

      context->source = QBI_SVC_MBIM_MS_CONTEXT_SOURCE_MODEM;

      switch (info->element_read)
      {
      case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_HOME:
        context->roaming = QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_HOME;
        break;
      case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_PARTNER:
        context->roaming = QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_PARTNER;
        break;

      case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_NON_PARTNER:
        context->roaming = QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_NON_PARTNER;
        break;
      }

      /* Update the size field to include DataBuffer items */
      field_desc->size = txn->infobuf_len_total - initial_offset;
      qbi_txn_rsp_databuf_consolidate(txn);

      info->element_match_index[info->element_read] = 1;
    }
  }
} /* qbi_svc_lte_attach_config_add_dummy_context_to_rsp() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_q_get_next_profile
===========================================================================*/
/*!
    @brief Retrive next available configured LTE attach profile.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_q_get_next_profile
(
  qbi_txn_s *txn
)
{
  wds_get_profile_settings_req_msg_v01    *qmi_req = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s *info    = NULL;
  qbi_svc_action_e                        action   = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  QBI_LOG_D_0("LTEAttachConfig::Q: Get next profile");
  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  //Don't send more that 3 response
  if ((info->element_read > 2) || (info->profiles_read >= info->num_of_profile))
  {
    qbi_svc_lte_attach_config_add_dummy_context_to_rsp(txn);
    if (QBI_TXN_CMD_TYPE_INTERNAL == txn->cmd_type)
    {
      qbi_util_buf_s buf;
      qbi_util_buf_init(&buf);
      qbi_util_buf_alloc(&buf, txn->infobuf_len_total);
      QBI_CHECK_NULL_PTR_RET_FALSE(buf.data);
      (void)qbi_txn_rsp_databuf_extract(
          txn, buf.data, buf.size, 0);
      qbi_util_buf_free(&txn->rsp);
      txn->rsp = buf;
    }
    action = QBI_SVC_ACTION_SEND_RSP;
    txn->status = QBI_MBIM_STATUS_SUCCESS;
  }
  else
  {
    /* Issue a query to retrieve the profile details */
    qmi_req = (wds_get_profile_settings_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_ext_lte_attach_config_q_wds2b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    QBI_LOG_D_1("LTEAttachConfig::Q: Get profile Setting for index %d",
                info->profile_index[info->profiles_read]);
    qmi_req->profile.profile_index = (uint8_t)
        info->profile_index[info->profiles_read];

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_q_get_next_profile */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_lte_attach_config_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_LTE_ATTACH_CONFIG query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_q_req
(
  qbi_txn_s *txn
)
{
  wds_get_lte_max_attach_pdn_num_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_D_0("LTEAttachConfig::Q: Query request");

  qmi_req = (wds_get_lte_max_attach_pdn_num_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WDS,
                                    QMI_WDS_GET_LTE_MAX_ATTACH_PDN_NUM_REQ_V01,
                                    qbi_svc_bc_ext_lte_attach_config_q_wds92_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_lte_attach_config_q_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_q_wds92_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_LTE_MAX_ATTACH_PDN_NUM_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG query request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_q_wds92_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                            action   = QBI_SVC_ACTION_ABORT;
  wds_get_lte_attach_pdn_list_req_msg_v01     *qmi_req = NULL;
  wds_get_lte_max_attach_pdn_num_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s     *info    = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_lte_max_attach_pdn_num_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachConfig::Q: Received error code %d from QMI",
                qmi_rsp->resp.error);
  }
  else
  {
    if (!qmi_txn->parent->info)
    {
      qmi_txn->parent->info = QBI_MEM_MALLOC_CLEAR(
          sizeof(qbi_svc_bc_ext_lte_attach_config_info_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
    }
    else
    {
      QBI_MEMSET(qmi_txn->parent->info, 0,
                 sizeof(qbi_svc_bc_ext_lte_attach_config_info_s));
    }

    info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(info);

    if (qmi_rsp->max_attach_pdn_num_valid)
    {
      info->max_supported_profile_num = qmi_rsp->max_attach_pdn_num;

      QBI_LOG_D_1("LTEAttachConfig::Q: Max Supported Profile %d",
                  info->max_supported_profile_num);

      qmi_req = (wds_get_lte_attach_pdn_list_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_WDS,
          QMI_WDS_GET_LTE_ATTACH_PDN_LIST_REQ_V01,
          qbi_svc_bc_ext_lte_attach_config_q_wds94_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
    else
    {
      QBI_LOG_E_0("LTEAttachConfig::Q: E: max_attach_pdn_num TLV missing.");
    }
  }
  return action;
} /* qbi_svc_bc_ext_lte_attach_config_q_wds92_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_q_wds94_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_LTE_ATTACH_PDN_LIST_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG query request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_q_wds94_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                            action        = QBI_SVC_ACTION_ABORT;
  wds_get_lte_attach_pdn_list_resp_msg_v01    *qmi_rsp      = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s     *info         = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_rsp_s *rsp;
  uint32                                      profile_count = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("LTEAttachConfig::Q: Processing get PDN list response");
  qmi_rsp = (wds_get_lte_attach_pdn_list_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1(" LTEAttachConfig::Q: Received error code %d from QMI",
                qmi_rsp->resp.error);
  }
  else
  {
    info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(info);
    if (qmi_rsp->attach_pdn_list_valid)
    {
      info->num_of_profile = qmi_rsp->attach_pdn_list_len;

      QBI_LOG_D_1("LTEAttachConfig::Q: No Of Profiles %d", info->num_of_profile);
      if (info->num_of_profile < info->max_supported_profile_num)
      {
        for (profile_count = 0; profile_count < info->num_of_profile; profile_count++)
        {
          info->profile_index[profile_count] =
              qmi_rsp->attach_pdn_list[profile_count];
          QBI_LOG_D_2("LTEAttachConfig::Q: profile_list[%d].index: %d",
                      profile_count, info->profile_index[profile_count]);
        }

        /* Allocate the fixed-length and offset/size pair portion of the
        response now. NOTE : Only 3 are needed because we return ONLY 3
        contexts home/partner/nonpartner*/
        rsp = (qbi_svc_bc_ext_lte_attach_config_info_rsp_s *)
            qbi_txn_alloc_rsp_buf(qmi_txn->parent,
                                  sizeof(qbi_svc_bc_ext_lte_attach_config_info_rsp_s) +
                                  sizeof(qbi_mbim_offset_size_pair_s) *
                                  QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE);
        QBI_CHECK_NULL_PTR_RET_ABORT(rsp);
        rsp->element_count = QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE;

        info->element_read = 0;
        action = qbi_svc_bc_ext_lte_attach_config_q_get_next_profile(
            qmi_txn->parent);
      }
      else
      {
        QBI_LOG_E_2("LTEAttachConfig::Q: Attach PDN list exceeds max supported "
                    "PDN. attach_pdn_list_len = %d and max_supported_profile_num = %d",
                    info->num_of_profile, info->max_supported_profile_num);
        action = QBI_SVC_ACTION_ABORT;
      }
    }
    else
    {
      QBI_LOG_E_0("LTEAttachConfig::Q: No LTE attac profile exists");
      action = QBI_SVC_ACTION_ABORT;
    }
  }
  return action;
} /* qbi_svc_bc_ext_lte_attach_config_q_wds94_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_lte_attach_config_q_wds2b_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG query request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_q_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e                      action   = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("LTEAttachConfig::Q: Resp Received For Get Profile Settings");
  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1(" LTEAttachConfig::Q: Received error code %d from QMI",
                qmi_rsp->resp.error);
    qbi_svc_bc_ext_lte_attach_config_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);

    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    if (qbi_svc_lte_attach_config_add_context_to_rsp(qmi_txn))
    {
      action = qbi_svc_bc_ext_lte_attach_config_q_get_next_profile(
          qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_q_wds2b_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_populate_profile_auth_protocol
===========================================================================*/
/*!
  @brief Populates the authentication preference/protocol TLV of a
  QMI_WDS_CREATE_PROFILE_REQ request

  @details

  @param txn
  @param req
  @param profile_type
  @param profile_settings

  @return boolean TRUE on success, FALSE on failure. May set txn->status
  if the request contained an invalid value
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_config_s_populate_profile_auth_protocol
(
  const qbi_svc_bc_ext_lte_attach_context_s *req,
  qbi_svc_bc_ext_lte_attach_config_profile_settings_s profile_settings
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(req);

  *profile_settings.authentication_preference_valid = TRUE;
  *profile_settings.authentication_preference =
      qbi_svc_bc_ext_connect_mbim_auth_pref_to_qmi_auth_pref(req->auth_protocol);
  success = TRUE;

  QBI_LOG_D_3("Success %d Requested Pref %d Returned Pref %d", success,
              req->auth_protocol, *profile_settings.authentication_preference);

  return success;
} /* qbi_svc_bc_ext_lte_attach_config_s_populate_profile_auth_protocol() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_populate_profile_compression
===========================================================================*/
/*!
  @brief Populates the compression TLVs of a
  QMI_WDS_CREATE_PROFILE_REQ

  @details

  @param req
  @param profile_type
  @param profile_settings

  @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_config_s_populate_profile_compression
(
  const qbi_svc_bc_ext_lte_attach_context_s *req,
  qbi_svc_bc_ext_lte_attach_config_profile_settings_s profile_settings
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(req);

  *profile_settings.pdp_data_compression_type_valid = TRUE;
  *profile_settings.pdp_hdr_compression_type_valid = TRUE;
  if (req->compression == QBI_SVC_BC_COMPRESSION_ENABLE)
  {
    *profile_settings.pdp_data_compression_type =
        WDS_PDP_DATA_COMPR_TYPE_MANUFACTURER_PREF_V01;
    *profile_settings.pdp_hdr_compression_type =
        WDS_PDP_HDR_COMPR_TYPE_MANUFACTURER_PREF_V01;
  }
  else
  {
    *profile_settings.pdp_data_compression_type =
        WDS_PDP_DATA_COMPR_TYPE_OFF_V01;
    *profile_settings.pdp_hdr_compression_type =
        WDS_PDP_HDR_COMPR_TYPE_OFF_V01;
  }

  return TRUE;
} /* qbi_svc_bc_ext_lte_attach_config_s_populate_profile_compression() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_build_wds27_req
===========================================================================*/
/*!
    @brief Allocates and populates a request for QMI_WDS_CREATE_PROFILE

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_build_wds27_req
(
  qbi_txn_s *txn
)
{
  wds_create_profile_req_msg_v01                      *qmi_req         = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s             *info            = NULL;
  qbi_svc_bc_ext_lte_attach_context_s                 *context         = NULL;
  qbi_mbim_offset_size_pair_s                         *field_desc      = NULL;
  qbi_svc_bc_ext_lte_attach_config_profile_settings_s profile_settings = { 0 };
  char                                                apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1]         = { 0, };
  char                                                username[QMI_WDS_USER_NAME_MAX_V01 + 1]         = { 0, };
  char                                                password[QMI_WDS_PASSWORD_MAX_V01 + 1]         = { 0, };
  const uint8                                         *field           = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  QBI_LOG_D_1(" LTEAttachConfig::S: Create profile for roam type %d",
              info->element_read);

  field_desc = (qbi_mbim_offset_size_pair_s *)((uint8_t *)txn->req.data +
                                               sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) +
                                               sizeof(qbi_mbim_offset_size_pair_s) * info->element_read);
  QBI_CHECK_NULL_PTR_RET_ABORT(field_desc);

  context = (qbi_svc_bc_ext_lte_attach_context_s *)
      qbi_txn_req_databuf_get_field(txn, field_desc, 0, field_desc->size);
  QBI_CHECK_NULL_PTR_RET_ABORT(context);

  if (context->access_string.size != 0)
  {
    field = qbi_txn_req_databuf_get_field(
        txn, &context->access_string, field_desc->offset,
        QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
    QBI_CHECK_NULL_PTR_RET_FALSE(field);

    qbi_util_utf16_to_ascii(field, context->access_string.size,
                            apn_name, sizeof(apn_name));
  }

  if (context->username.size != 0)
  {
    field = qbi_txn_req_databuf_get_field(
        txn, &context->username, field_desc->offset,
        QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
    QBI_CHECK_NULL_PTR_RET_FALSE(field);

    qbi_util_utf16_to_ascii(field, context->username.size,
                            username, sizeof(username));
  }

  if (context->password.size != 0)
  {
    field = qbi_txn_req_databuf_get_field(
        txn, &context->password, field_desc->offset,
        QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
    QBI_CHECK_NULL_PTR_RET_FALSE(field);

    qbi_util_utf16_to_ascii(field, context->password.size,
                            password, sizeof(password));
  }
  /* Issue create profile */
  qmi_req = (wds_create_profile_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_WDS, QMI_WDS_CREATE_PROFILE_REQ_V01,
      qbi_svc_bc_ext_lte_attach_config_s_wds27_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->apn_name_valid = TRUE;
  QBI_STRLCPY(qmi_req->apn_name, apn_name, sizeof(qmi_req->apn_name));

  qmi_req->username_valid = TRUE;
  QBI_STRLCPY(qmi_req->username, username, sizeof(qmi_req->username));

  qmi_req->password_valid = TRUE;
  QBI_STRLCPY(qmi_req->password, password, sizeof(qmi_req->password));

  qmi_req->pdp_type_valid = TRUE;
  qmi_req->pdp_type = qbi_svc_bc_ext_ip_type_to_pdp_type(context->ip_type);

  profile_settings.pdp_data_compression_type_valid =
      &qmi_req->pdp_data_compression_type_valid;
  profile_settings.pdp_data_compression_type =
      &qmi_req->pdp_data_compression_type;
  profile_settings.pdp_hdr_compression_type_valid =
      &qmi_req->pdp_hdr_compression_type_valid;
  profile_settings.pdp_hdr_compression_type =
      &qmi_req->pdp_hdr_compression_type;
  profile_settings.authentication_preference_valid =
      &qmi_req->authentication_preference_valid;
  profile_settings.authentication_preference =
      &qmi_req->authentication_preference;

  if (!qbi_svc_bc_ext_lte_attach_config_s_populate_profile_compression(
          context, profile_settings) ||
      !qbi_svc_bc_ext_lte_attach_config_s_populate_profile_auth_protocol(
          context, profile_settings))
  {
    QBI_LOG_E_0("LTEAttachConfig::S: Couldn't populate compression or auth protocol!");
  }

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_lte_attach_config_s_build_wds27_req */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_get_next_profile
===========================================================================*/
/*!
    @brief Retrive next available configured LTE attach profile.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_get_next_profile
(
  qbi_txn_s *txn
)
{
  wds_get_profile_settings_req_msg_v01    *qmi_req = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s *info    = NULL;
  qbi_svc_action_e                        action   = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  QBI_LOG_D_2("LTEAttachConfig::S: Profiles Read %d/%d",
              info->profiles_read, info->num_of_profile);
  if (info->profiles_read < info->num_of_profile)
  {
    /* Issue a query to retrieve the profile details */
    qmi_req = (wds_get_profile_settings_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_ext_lte_attach_config_s_wds2b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->profile.profile_index =
        (uint8_t)info->profile_index[info->profiles_read];

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else if (qbi_svc_bc_ext_lte_attach_config_s_is_profile_match_pending(info))
  {
    action = qbi_svc_bc_ext_lte_attach_config_s_build_wds27_req(txn);
  }
  else
  {
    QBI_LOG_D_0("LTEAttachConfig::S: Registering for detach/attach indications");
    if (!qbi_svc_bc_ext_lte_attach_config_s_register_for_detach_attach(txn))
    {
      action = QBI_SVC_ACTION_ABORT;
    }
    else
    {
      action = qbi_svc_bc_ext_lte_attach_config_s_build_wds93_req(txn);
    }
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_get_next_profile */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_input_validation
===========================================================================*/
/*!
    @brief Peroforms input validation

    @details

    @param req

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_config_s_input_validation
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_lte_attach_config_s_req_s *req        = NULL;
  qbi_svc_bc_ext_lte_attach_context_s      *context    = NULL;
  qbi_mbim_offset_size_pair_s              *field_desc = NULL;
  uint32                                   roam_type   = QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_HOME;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  req = (qbi_svc_bc_ext_lte_attach_config_s_req_s *)txn->req.data;

  if (req->element_count == QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE)
  {
    for (roam_type = QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_ROAMING_CONTROL_HOME;
         roam_type < QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE; roam_type++)
    {
      field_desc = (qbi_mbim_offset_size_pair_s *)((uint8_t *)req +
                                                   sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) +
                                                   sizeof(qbi_mbim_offset_size_pair_s) * roam_type);
      QBI_CHECK_NULL_PTR_RET_FALSE(field_desc);

      context = (qbi_svc_bc_ext_lte_attach_context_s *)
          qbi_txn_req_databuf_get_field(txn, field_desc, 0,
                                        field_desc->size);
      QBI_CHECK_NULL_PTR_RET_FALSE(context);

      if (context->roaming != roam_type)
      {
        QBI_LOG_E_2("LTEAttachConfig::S: E: Invalid roam type (%d) for element (%d)",
                    context->roaming, roam_type);
        return FALSE;
      }
    }
  }
  else
  {
    QBI_LOG_E_0("LTEAttachConfig::S: E: Invalid element count");
    return FALSE;
  }

  return TRUE;
} /* qbi_svc_bc_ext_lte_attach_config_s_input_validation() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e                         action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_lte_attach_config_s_req_s *req   = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  cmd_in_progress_ignore_indication = TRUE;

  req = (qbi_svc_bc_ext_lte_attach_config_s_req_s *)txn->req.data;
  QBI_LOG_D_2("LTEAttachConfig::S: operation = %d, element_count = %d",
              req->operation, req->element_count);

  /* Allocate the fixed-length and offset/size pair portion of the
  response now. */
  txn->info = QBI_MEM_MALLOC_CLEAR(
      sizeof(qbi_svc_bc_ext_lte_attach_config_info_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  switch (req->operation)
  {
  case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_OPERATION_RESTORE_FACTORY:
    action = qbi_svc_bc_ext_lte_attach_config_s_operation_factory_restore(txn);
    break;
  case QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_OPERATION_DEFAULT:
    if (qbi_svc_bc_ext_lte_attach_config_s_input_validation(txn))
    {
      action = qbi_svc_bc_ext_lte_attach_config_s_operation_default(txn);
    }
    break;
  default:
    QBI_LOG_E_1("Operation %d not supported", req->operation);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    break;
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_build_wds94_req
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_GET_LTE_ATTACH_PDN_LIST_REQ_V01 for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_build_wds94_req
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                        action   = QBI_SVC_ACTION_ABORT;
  wds_get_lte_attach_pdn_list_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  QBI_LOG_D_0("LTEAttachConfig::S: Getting PDN list");
  qmi_req = (wds_get_lte_attach_pdn_list_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
      qmi_txn->parent, QBI_QMI_SVC_WDS,
      QMI_WDS_GET_LTE_ATTACH_PDN_LIST_REQ_V01,
      qbi_svc_bc_ext_lte_attach_config_s_wds94_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  action = QBI_SVC_ACTION_SEND_QMI_REQ;

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_build_wds94_req */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_wds92_rsp_cb
===========================================================================*/
/*!
  @brief Handles a QMI_WDS_GET_LTE_MAX_ATTACH_PDN_NUM_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds92_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                            action   = QBI_SVC_ACTION_ABORT;
  wds_get_lte_max_attach_pdn_num_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s     *info    = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_lte_max_attach_pdn_num_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1(" LTEAttachConfig::Q: Received error code %d from QMI",
                qmi_rsp->resp.error);
    cmd_in_progress_ignore_indication = FALSE;
  }
  else
  {
    info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(info);
    info->max_supported_profile_num = qmi_rsp->max_attach_pdn_num;

    QBI_LOG_D_1("LTEAttachConfig::S: max_supported_profile_num = %d",
                info->max_supported_profile_num);

    action = qbi_svc_bc_ext_lte_attach_config_s_build_wds94_req(qmi_txn);
  }
  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_wds92_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_register_for_detach_attach
===========================================================================*/
/*!
  @brief Registers for QMI_WDS_LTE_ATTACH_PDN_LIST_IND and
  QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND indications

  @details

  @param txn

  @return boolean returns true if registeration is successful
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_config_s_register_for_detach_attach
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_D_0("LTEAttachConfig::S: Registering for detach/attach indications");
  if (!qbi_svc_ind_reg_dynamic(
          txn->ctx, QBI_SVC_ID_BC_EXT,
          QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG,
          QBI_QMI_SVC_WDS, QMI_WDS_LTE_ATTACH_PDN_LIST_IND_V01,
          qbi_svc_bc_ext_lte_attach_config_s_wds95_ind_cb, txn, NULL))
  {
    QBI_LOG_E_0("LTEAttachConfig::S: Failed to register PDN list indication.");
    return FALSE;
  }
  else if (!qbi_svc_ind_reg_dynamic(
          txn->ctx, QBI_SVC_ID_BC_EXT,
          QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG,
          QBI_QMI_SVC_NAS, QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01,
          qbi_svc_bc_ext_lte_attach_config_s_nas34_ind_cb, txn, NULL))
  {
    QBI_LOG_E_0("LTEAttachConfig::S: Failed to register SSP indication.");
    return FALSE;
  }

  return TRUE;
} /* qbi_svc_bc_ext_lte_attach_config_s_register_for_detach_attach() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_add_profile_list_from_cache
===========================================================================*/
/*!
  @brief Allocates and populates a QMI_WDS_DELETE_PROFILE_REQ request

  @details

  @param txn

  @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_config_s_add_profile_list_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_module_prov_cache_s           *cache = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s      *info  = NULL;
  uint32                                  i      = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->info);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  for (i = 0; i < QMI_WDS_PROFILE_LIST_MAX_V01; i++)
  {
    cache = qbi_svc_bc_ext_module_prov_cache_get(txn->ctx, i);
    QBI_CHECK_NULL_PTR_RET_FALSE(cache);

    if (cache->lte_active && info->num_of_profile < QMI_WDS_ATTACH_PDN_MAX_V01)
    {
      info->profile_index[info->num_of_profile++] = i;
    }
  }

  if (!info->num_of_profile)
  {
    QBI_LOG_E_0("LTEAttachConfig::S: E:No LTE attach profile exists.");
    return FALSE;
  }

  return TRUE;
} /* qbi_svc_bc_ext_lte_attach_config_s_add_profile_list_from_cache() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_add_profile_list_from_modem
===========================================================================*/
/*!
  @brief Allocates and populates a QMI_WDS_DELETE_PROFILE_REQ request

  @details

  @param txn

  @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_config_s_add_profile_list_from_modem
(
  qbi_txn_s *txn,
  uint32    modem_pdn_list_len,
  uint16 *modem_pdn_list
)
{
  qbi_svc_bc_ext_lte_attach_config_info_s *info     = NULL;
  uint32                                  list_size = 0;
  uint32                                  i         = 0;
  uint32                                  j         = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->info);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  QBI_LOG_D_2("LTEAttachConfig::S: Modem PDN List Len %d Num Of Profile %d",
              modem_pdn_list_len, info->num_of_profile);

  if (!info->num_of_profile)
  {
    for (i = 0; i < modem_pdn_list_len; i++)
    {
      info->profile_index[j] = modem_pdn_list[i];
      info->num_of_profile++;

      QBI_LOG_D_1("LTEAttachConfig::S: Added Profile index %d to list.",
                  info->profile_index[j]);
    }
  }
  else
  {
    for (i = 0; i < modem_pdn_list_len; i++)
    {
      for (j = 0; j < info->num_of_profile; j++)
      {
        // if the index match, do not add to list.
        if (modem_pdn_list[i] == info->profile_index[j])
        {
          QBI_LOG_D_1("LTEAttachConfig::S: Profile index %d, exists in cache.",
                      modem_pdn_list[i]);
          break;
        }
        // List retrieved from cache is sorted. if index is lesser than the
        // cache index, insert profile index to the list in sorted order.
        else if (modem_pdn_list[i] < info->profile_index[j])
        {
          if (info->num_of_profile < info->max_supported_profile_num)
          {
            list_size = sizeof(uint8_t) * (info->num_of_profile - j);

            QBI_MEMSCPY(&info->profile_index[j + 1], list_size,
                        &info->profile_index[j], list_size);

            info->profile_index[j] = modem_pdn_list[i];
            info->num_of_profile++;

            QBI_LOG_D_1("LTEAttachConfig::S: Added Profile index %d to list.",
                        info->profile_index[j]);
            break;
          }
          else
          {
            QBI_LOG_E_2("LTEAttachConfig::S: num_of_profile(%d) exceeds "
                        "max_supported_profile_num (%d). Skipping profile add but "
                        "continue comparision", info->num_of_profile,
                        info->max_supported_profile_num);
            break;
          }
        }
      }
    }
  }

  return TRUE;
} /* qbi_svc_bc_ext_lte_attach_config_s_add_profile_list_from_modem() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_get_profile_list
===========================================================================*/
/*!
  @brief Allocates and populates a QMI_WDS_DELETE_PROFILE_REQ request

  @details

  @param txn

  @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_config_s_get_profile_list
(
  qbi_txn_s *txn,
  wds_get_lte_attach_pdn_list_resp_msg_v01 *qmi_rsp
)
{
  qbi_svc_bc_ext_lte_attach_config_info_s  *info = NULL;
  qbi_svc_bc_ext_lte_attach_config_s_req_s *req  = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->info);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);

  req = (qbi_svc_bc_ext_lte_attach_config_s_req_s *)txn->req.data;
  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  // 0th profile is for 3GPP2. Skip for LTE profiles.
  qbi_svc_bc_ext_lte_attach_config_s_add_profile_list_from_cache(txn);
  QBI_LOG_D_1("LTEAttachConfig::S: No. of profiles from cache list: %d",
              info->num_of_profile);

  if (QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_OPERATION_DEFAULT == req->operation)
  {
    // Add active LTE profiles.
    if (qmi_rsp->attach_pdn_list_valid)
    {
      qbi_svc_bc_ext_lte_attach_config_s_add_profile_list_from_modem(
          txn, qmi_rsp->attach_pdn_list_len, qmi_rsp->attach_pdn_list);
      QBI_LOG_D_1("LTEAttachConfig::S: No. of profiles from active list: %d",
                  info->num_of_profile);
    }

    // Add pending LTE profiles.
    if (qmi_rsp->pending_attach_pdn_list_valid)
    {
      qbi_svc_bc_ext_lte_attach_config_s_add_profile_list_from_modem(txn,
                                                                     qmi_rsp->pending_attach_pdn_list_len, qmi_rsp->pending_attach_pdn_list);
      QBI_LOG_D_1("LTEAttachConfig::S: No. of profiles from pending list: %d",
                  info->num_of_profile);
    }
  }

  if (!info->num_of_profile)
  {
    QBI_LOG_E_0("LTEAttachConfig::S: E:No LTE attach profile exists.");
    return FALSE;
  }

  return TRUE;
} /* qbi_svc_bc_ext_lte_attach_config_s_get_profile_list() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_operation_factory_restore
===========================================================================*/
/*!
  @brief Allocates and populates a QMI_WDS_DELETE_PROFILE_REQ request

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_operation_factory_restore
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *req   = NULL;
  qbi_svc_action_e                               action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  QBI_LOG_D_0("LTEAttachConfig::S: Factory restore.");

  if (!qbi_svc_bc_ext_lte_attach_config_s_register_for_detach_attach(txn))
  {
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    QBI_MEM_FREE(txn->req.data);
    txn->req.data = NULL;

    req = qbi_util_buf_alloc(&txn->req,
                             sizeof(qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(req);

    req->operation = QBI_SVC_MBIM_MS_CONTEXT_OPERATION_RESTORE_FACTORY;
    action = qbi_svc_bc_ext_prov_ctx_v2_s_req(txn);
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_operation_factory_restore() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_cache_user_defined_profile
===========================================================================*/
/*!
  @brief  Performs storage of user created profiles for
      MBIM_CID_MS_LTE_ATTACH_CONFIG set request

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_cache_user_defined_profile
(
  qbi_txn_s *txn
)
{
  wds_get_lte_max_attach_pdn_num_req_msg_v01 *qmi_req = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s    *info    = NULL;
  qbi_svc_bc_ext_module_prov_cache_s         *cache   = NULL;
  qbi_svc_action_e                           action   = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  info->max_user_defined_profiles = 0;
  info->index_user_defined_profiles = 0;

  while (info->profiles_read < info->num_of_profile)
  {
    cache = qbi_svc_bc_ext_module_prov_cache_get(txn->ctx,
      info->profile_index[info->profiles_read]);
    QBI_CHECK_NULL_PTR_RET_FALSE(cache);

    QBI_LOG_D_4(" LTEAttachConfig::S: Cached profile[%d/%d]: %d, flag: %d",
      info->profiles_read, info->num_of_profile,
      info->profile_index[info->profiles_read], cache->context_flag);

    if (QBI_SVC_BC_EXT_CONTEXT_FLAG_USER_DEFINED == cache->context_flag)
    {
      QBI_LOG_D_2(" LTEAttachConfig::S: Added profile %d to user defined profiles,"
        "max_user_defined_profiles= %d",
        info->profile_index[info->profiles_read], info->max_user_defined_profiles);
      info->user_defined_profiles[info->max_user_defined_profiles] =
        info->profile_index[info->profiles_read];
      info->max_user_defined_profiles++;
    }
    
      info->profiles_read++;
    }

  if (QBI_SVC_ACTION_ABORT == action)
  {
    qmi_req = (wds_get_lte_max_attach_pdn_num_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WDS,
      QMI_WDS_GET_LTE_MAX_ATTACH_PDN_NUM_REQ_V01,
      qbi_svc_bc_ext_lte_attach_config_s_wds92_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_cache_user_defined_profile() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_operation_default
===========================================================================*/
/*!
  @brief Allocates and populates a QMI_WDS_DELETE_PROFILE_REQ request

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_operation_default
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qbi_svc_bc_ext_lte_attach_config_s_add_profile_list_from_cache(txn);

  return qbi_svc_bc_ext_lte_attach_config_s_cache_user_defined_profile(txn);
} /* qbi_svc_bc_ext_lte_attach_config_s_operation_default() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_wds94_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_LTE_ATTACH_PDN_LIST_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds94_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                         action   = QBI_SVC_ACTION_ABORT;
  wds_get_lte_attach_pdn_list_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s *info = NULL;
  uint32 current_match_index = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;

  qmi_rsp = (wds_get_lte_attach_pdn_list_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1(" LTEAttachConfig::S: E: Received error code %d from QMI",
                qmi_rsp->resp.error);
    cmd_in_progress_ignore_indication = FALSE;
  }
  else
  {
    /* Storing the Modem PDN list */
    if (qmi_rsp->attach_pdn_list_valid)
    {
      info->current_attach_pdn_list_len = qmi_rsp->attach_pdn_list_len;
      for (current_match_index = 0;
           current_match_index < qmi_rsp->attach_pdn_list_len &&
             current_match_index < QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE;
           current_match_index++)
      {
        info->current_attach_pdn_list[current_match_index] =
          qmi_rsp->attach_pdn_list[current_match_index];
        QBI_LOG_D_2(" LTEAttachConfig::S: Attach PDN_List[%d] from QMI is %d",
          current_match_index,
          info->current_attach_pdn_list[current_match_index]);
      }
    }

    if (qbi_svc_bc_ext_lte_attach_config_s_get_profile_list(
            qmi_txn->parent, qmi_rsp))
    {
      action = qbi_svc_bc_ext_lte_attach_config_s_get_next_profile(
          qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_wds94_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_match_string_field
===========================================================================*/
/*!
    @brief Compares modem and apps strings

    @details

    @param qmi_str_valid
    @param qmi_str
    @param field
    @param field_size
    @param is_case_insensitive

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_match_string_field
(
  const uint8   qmi_str_valid,
  const char *qmi_str,
  const uint8 *field,
  const uint32  field_size,
  boolean is_case_insensitive
)
{
  char    *str         = NULL;
  uint32  str_size     = 0;
  uint32  qmi_str_size = 0;
  boolean match_found  = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_str);

  if (qmi_str_valid)
  {
    str = QBI_MEM_MALLOC_CLEAR(field_size + 2);
    QBI_CHECK_NULL_PTR_RET_FALSE(str);

    qbi_util_utf16_to_ascii(field, field_size, str, field_size);

    qmi_str_size = QBI_STRLEN(qmi_str);
    str_size = QBI_STRLEN(str);

    QBI_LOG_STR_2("LTEAttachConfig::S: Matching %s <==> %s", qmi_str, str);
    // If unequal size return FALSE immediately.
    if (str_size != qmi_str_size)
    {
      QBI_LOG_I_2("LTEAttachConfig::S: String length did not match. %d <==> %d",
                  qmi_str_size, str_size);
      match_found = FALSE;
    }
    // If size of both string equals, return TRUE immediately.
    else if (!str_size && !qmi_str_size)
    {
      QBI_LOG_I_0("LTEAttachConfig::S: Matched NULL string");
      match_found = TRUE;
    }
    else
    {
      match_found = is_case_insensitive ?
        !QBI_STRNICMP(str, qmi_str, str_size)
        : !QBI_STRNCMP(str, qmi_str, str_size);
    }

    QBI_MEM_FREE(str);
    str = NULL;
  }

  return match_found;
} /* qbi_svc_bc_ext_match_string_field() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_match_profile
===========================================================================*/
/*!
    @brief Compares LTE attach profiles of modem and apps

    @details

    @param txn
    @param qmi_rsp

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_config_s_match_profile
(
  qbi_txn_s *txn,
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp
)
{
  qbi_svc_bc_ext_lte_attach_config_s_req_s *req        = NULL;
  qbi_svc_bc_ext_lte_attach_context_s      *context    = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s  *info       = NULL;
  qbi_mbim_offset_size_pair_s              *field_desc = NULL;
  const uint8                              *field      = NULL;
  boolean                                  match_found = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->info);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);

  req = (qbi_svc_bc_ext_lte_attach_config_s_req_s *)txn->req.data;
  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  info->element_read = 0;

  do
  {
    if (info->element_match_index[info->element_read] == 0)
    {
      QBI_LOG_I_3("LTEAttachConfig::S: Matching profile for roam type: %d, Modem Index: %d",
                  info->element_read, info->profile_index[info->profiles_read],
                  info->element_match_index[info->element_read]);

      field_desc = (qbi_mbim_offset_size_pair_s *)((uint8 *)txn->req.data +
                                                   sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) +
                                                   sizeof(qbi_mbim_offset_size_pair_s) * info->element_read);

      context = (qbi_svc_bc_ext_lte_attach_context_s *)
          qbi_txn_req_databuf_get_field(txn, field_desc, 0,
                                        field_desc->size);
      QBI_CHECK_NULL_PTR_RET_FALSE(context);

      match_found = qbi_svc_bc_ext_match_pdp_type(context->ip_type, qmi_rsp) ? TRUE : FALSE;

      if (match_found)
      {
        QBI_LOG_I_0("LTEAttachConfig::S: Matching APN");

        field = qbi_txn_req_databuf_get_field(txn, &context->access_string,
                                              field_desc->offset, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);

        match_found = qbi_svc_bc_ext_match_string_field(qmi_rsp->apn_name_valid,
          qmi_rsp->apn_name, field, context->access_string.size, TRUE);
      }

      if (match_found)
      {
        QBI_LOG_I_0("LTEAttachConfig::S: Matching username");

        field = qbi_txn_req_databuf_get_field(txn, &context->username,
                                              field_desc->offset, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);

        match_found = qbi_svc_bc_ext_match_string_field(qmi_rsp->username_valid,
          qmi_rsp->username, field, context->username.size, FALSE);
      }

      if (match_found)
      {
        QBI_LOG_I_0("LTEAttachConfig::S: Matching password");

        field = qbi_txn_req_databuf_get_field(txn, &context->password,
                                              field_desc->offset, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);

        match_found = qbi_svc_bc_ext_match_string_field(qmi_rsp->password_valid,
          qmi_rsp->password, field, context->password.size, FALSE);
      }

      if (match_found)
      {
        info->element_match_index[info->element_read] =
            info->profile_index[info->profiles_read];

        QBI_LOG_I_2("LTEAttachConfig::S: Match found. Added profile index: %d to roam type %d",
                    info->element_match_index[info->element_read], info->element_read);

        return TRUE;
      }
      else
      {
        QBI_LOG_I_0("LTEAttachConfig::S: Match not found.");
      }
    }

    info->element_read++;
  }while (info->element_read < req->element_count);

  return FALSE;
} /* qbi_svc_bc_ext_lte_attach_config_s_match_profile() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_wds2b_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_resp_msg_v01   *qmi_rsp = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s *info    = NULL;
  qbi_svc_action_e                        action   = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("LTEAttachConfig::S: Received Resp For GET PROFILE SETTINGS Req");
  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachConfig::S: E:Received error code %d from QMI",
                qmi_rsp->resp.error);

    qbi_svc_bc_ext_lte_attach_config_set_mbim_error_status(qmi_txn->parent,
                                                                qmi_rsp->resp.error, qmi_rsp->extended_error_code_valid,
                                                                qmi_rsp->extended_error_code);

    if (QMI_ERR_EXTENDED_INTERNAL_V01 == qmi_rsp->resp.error &&
        qmi_rsp->extended_error_code_valid &&
        WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM_V01 ==
            qmi_rsp->extended_error_code)
    {
      QBI_LOG_D_0("LTEAttachConfig::S: Bad profile with modem. Continue to next profile");

      info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;
      info->profiles_read++;
      action = qbi_svc_bc_ext_lte_attach_config_s_get_next_profile(
          qmi_txn->parent);
    }
  }
  else
  {
    if (qbi_svc_bc_ext_lte_attach_config_s_match_profile(
            qmi_txn->parent, qmi_rsp))
    {
      action = qbi_svc_bc_ext_lte_attach_config_s_build_wds28_req(
          qmi_txn->parent);
    }
    else
    {
      info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;
      info->profiles_read++;
      action = qbi_svc_bc_ext_lte_attach_config_s_get_next_profile(
          qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_wds2b_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_wds27_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_CREATE_PROFILE_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds27_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_create_profile_req_msg_v01                      *qmi_req         = NULL;
  wds_create_profile_resp_msg_v01                     *qmi_rsp         = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s             *info            = NULL;
  qbi_svc_action_e                                    action           = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_lte_attach_config_profile_settings_s profile_settings = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_create_profile_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachConfig::S: Received error code %d from QMI",
                qmi_rsp->resp.error);
    qbi_svc_bc_ext_lte_attach_config_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }
  else
  {
    QBI_LOG_D_1("LTEAttachConfig::S: Profile created successfully at index %d",
                qmi_rsp->profile.profile_index);

    info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;
    info->element_match_index[info->element_read] =
        qmi_rsp->profile.profile_index;
    info->profile_index[info->profiles_read] = qmi_rsp->profile.profile_index;

    QBI_LOG_D_2("LTEAttachConfig::S: Added profile index: %d to roam type: %d",
                info->profile_index[info->profiles_read], info->profiles_read);

    qmi_req = (wds_create_profile_req_msg_v01 *)qmi_txn->req.data;

    profile_settings.roaming_disallowed_valid = qmi_req->roaming_disallowed_valid;
    profile_settings.roaming_disallowed = qmi_req->roaming_disallowed;

    profile_settings.pdp_type_valid = qmi_req->pdp_type_valid;
    profile_settings.pdp_type = qmi_req->pdp_type;

    qbi_svc_bc_ext_lte_attach_config_update_cache(
        qmi_txn->parent, TRUE, profile_settings);

    if (qbi_svc_bc_ext_lte_attach_config_s_is_profile_match_pending(info))
    {
      info->profiles_read = 0;
    }

    info->num_of_profile++;
    action = qbi_svc_bc_ext_lte_attach_config_s_get_next_profile(
        qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_wds27_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_build_wds28_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WDS_MODIFY_PROFILE_SETTINGS request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_build_wds28_req
(
  qbi_txn_s *txn
)
{
  wds_modify_profile_settings_req_msg_v01             *qmi_req         = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s             *info            = NULL;
  qbi_svc_bc_ext_lte_attach_config_profile_settings_s profile_settings = { 0 };
  qbi_svc_bc_ext_lte_attach_context_s                 *context         = NULL;
  qbi_mbim_offset_size_pair_s                         *field_desc      = NULL;
  const uint8                                         *field           = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  QBI_LOG_D_0("Initiating Modify Profile Request");
  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  field_desc = (qbi_mbim_offset_size_pair_s *)((uint8_t *)txn->req.data +
                                               sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) +
                                               sizeof(qbi_mbim_offset_size_pair_s) * info->element_read);
  QBI_CHECK_NULL_PTR_RET_ABORT(field_desc);

  context = (qbi_svc_bc_ext_lte_attach_context_s *)
      qbi_txn_req_databuf_get_field(txn, field_desc, 0, field_desc->size);
  QBI_CHECK_NULL_PTR_RET_ABORT(context);

  qmi_req = (wds_modify_profile_settings_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WDS,
                                    QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ_V01,
                                    qbi_svc_bc_ext_lte_attach_config_s_wds28_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->apn_name_valid = TRUE;
  if (context->access_string.size != 0)
  {
    field = qbi_txn_req_databuf_get_field(txn, &context->access_string,
                                          field_desc->offset, QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES);
    QBI_CHECK_NULL_PTR_RET_FALSE(field);

    qbi_util_utf16_to_ascii(field, context->access_string.size,
                            qmi_req->apn_name, sizeof(qmi_req->apn_name));
  }

  qmi_req->username_valid = TRUE;
  if (context->username.size != 0)
  {
    field = qbi_txn_req_databuf_get_field(txn, &context->username,
                                          field_desc->offset, QBI_SVC_BC_USERNAME_MAX_LEN_BYTES);
    QBI_CHECK_NULL_PTR_RET_FALSE(field);

    qbi_util_utf16_to_ascii(field, context->username.size,
                            qmi_req->username, sizeof(qmi_req->username));
  }

  qmi_req->password_valid = TRUE;
  if (context->password.size != 0)
  {
    field = qbi_txn_req_databuf_get_field(txn, &context->password,
                                          field_desc->offset, QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES);
    QBI_CHECK_NULL_PTR_RET_FALSE(field);

    qbi_util_utf16_to_ascii(field, context->password.size,
                            qmi_req->password, sizeof(qmi_req->password));
  }

  qmi_req->profile.profile_index =
      (uint8_t)info->profile_index[info->profiles_read];

  qmi_req->pdp_type_valid = TRUE;
  qmi_req->pdp_type = qbi_svc_bc_ext_ip_type_to_pdp_type(context->ip_type);

  qmi_req->roaming_disallowed_valid = TRUE;
  qmi_req->roaming_disallowed = FALSE;

  profile_settings.pdp_data_compression_type_valid =
      &qmi_req->pdp_data_compression_type_valid;
  profile_settings.pdp_data_compression_type =
      &qmi_req->pdp_data_compression_type;
  profile_settings.pdp_hdr_compression_type_valid =
      &qmi_req->pdp_hdr_compression_type_valid;
  profile_settings.pdp_hdr_compression_type =
      &qmi_req->pdp_hdr_compression_type;
  profile_settings.authentication_preference_valid =
      &qmi_req->authentication_preference_valid;
  profile_settings.authentication_preference =
      &qmi_req->authentication_preference;

  if (!qbi_svc_bc_ext_lte_attach_config_s_populate_profile_compression(
          context, profile_settings) ||
      !qbi_svc_bc_ext_lte_attach_config_s_populate_profile_auth_protocol(
          context, profile_settings))
  {
    QBI_LOG_E_0("LTEAttachConfig::S: Couldn't populate compression or auth protocol!");
  }

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_lte_attach_config_s_build_wds28_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_is_profile_match_pending
===========================================================================*/
/*!
    @brief Return TRUE/FALSE whether there is a pending profile create 

    @details

    @param qmi_txn

    @return boolean
*/
/*=========================================================================*/
boolean qbi_svc_bc_ext_lte_attach_config_s_is_profile_match_pending
(
  qbi_svc_bc_ext_lte_attach_config_info_s *info
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(info);

  info->element_read = 0;
  while (info->element_read < QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE)
  {
    if (info->element_match_index[info->element_read] == 0)
    {
      return TRUE;
    }

    info->element_read++;
  }

  QBI_LOG_D_0("Created/modified Profiles for all roaming type completed.");
  return FALSE;
} /* qbi_svc_bc_ext_lte_attach_config_s_is_profile_match_pending() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_wds28_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_MODIFY_PROFILE_SETTINGS_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds28_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                                    action           = QBI_SVC_ACTION_ABORT;
  wds_modify_profile_settings_resp_msg_v01            *qmi_rsp         = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s             *info            = NULL;
  wds_modify_profile_settings_req_msg_v01             *qmi_req         = NULL;
  qbi_svc_bc_ext_lte_attach_config_profile_settings_s profile_settings = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("LTEAttachConfig::S: Received Modified Complete Response.");
  qmi_rsp = (wds_modify_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachConfig::S: Received error code %d from QMI.",
                qmi_rsp->resp.error);
    qbi_svc_bc_ext_lte_attach_config_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }
  else
  {
    //Send SET PDN LIST request for the modified profile
    qmi_req = (wds_modify_profile_settings_req_msg_v01 *)qmi_txn->req.data;
    info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;

    info->element_match_index[info->element_read] =
        qmi_req->profile.profile_index;

    profile_settings.roaming_disallowed_valid = qmi_req->roaming_disallowed_valid;
    profile_settings.roaming_disallowed = qmi_req->roaming_disallowed;

    profile_settings.pdp_type_valid = qmi_req->pdp_type_valid;
    profile_settings.pdp_type = qmi_req->pdp_type;

    (void)qbi_svc_bc_ext_lte_attach_config_update_cache(
        qmi_txn->parent, FALSE, profile_settings);

    if (qbi_svc_bc_ext_lte_attach_config_s_is_profile_match_pending(info))
    {
      info->profiles_read = 0;
    }

    action = qbi_svc_bc_ext_lte_attach_config_s_get_next_profile(
        qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_wds28_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_build_wds93_req
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_SET_LTE_ATTACH_PDN_LIST for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_build_wds93_req
(
  qbi_txn_s *txn
)
{
  wds_set_lte_attach_pdn_list_req_msg_v01 *qmi_req      = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s *info         = NULL;
  uint16                                  profile_index = 0;
  uint32 current_index = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  // Set profile as LTE profile
  QBI_LOG_D_0("LTEAttachConfig::S: Setting PDN List");
  qmi_req = (wds_set_lte_attach_pdn_list_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WDS,
                                    QMI_WDS_SET_LTE_ATTACH_PDN_LIST_REQ_V01,
                                    qbi_svc_bc_ext_lte_attach_config_s_wds93_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;

  //Copying as it is, we need three profiles in modem this might help in query
  for (profile_index = 0;
       profile_index < QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE;
       profile_index++)
  {
    if (info->element_match_index[profile_index])
    {
      qmi_req->attach_pdn_list[qmi_req->attach_pdn_list_len] =
          info->element_match_index[profile_index];

      QBI_LOG_D_2("LTEAttachConfig::S: attach_pdn_list[%d] = %d ",
                  qmi_req->attach_pdn_list_len, qmi_req->attach_pdn_list[profile_index]);

      qmi_req->attach_pdn_list_len++;
    }
  }

  /* Comparing the Modem PDN list with set request PDN list */
  if (info->current_attach_pdn_list_len ==
      QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE)
  {
    info->skip_attach_pdn_list_ind = TRUE;
    for (current_index = 0;
         current_index < info->current_attach_pdn_list_len;
         current_index++)
    {
      if (info->current_attach_pdn_list[current_index] !=
          info->element_match_index[current_index])
      {
        info->skip_attach_pdn_list_ind = FALSE;
        break;
      }
    }
    QBI_LOG_D_1("LTEAttachConfig::S: Skip attach PDN list ind = %d",
                info->skip_attach_pdn_list_ind);
  }

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_lte_attach_config_s_build_wds93_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_wds93_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_SET_LTE_ATTACH_PDN_LIST for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds93_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                                    action   = QBI_SVC_ACTION_ABORT;
  wds_update_lte_attach_pdn_list_profiles_req_msg_v01 *qmi_req = NULL;
  wds_set_lte_attach_pdn_list_resp_msg_v01            *qmi_rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("LTEAttachConfig::S: Set PDN list response received");
  qmi_rsp = (wds_set_lte_attach_pdn_list_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachConfig::S: Received error code %d from QMI",
                qmi_rsp->resp.error);
    cmd_in_progress_ignore_indication = FALSE;
  }
  else
  {
    QBI_LOG_D_0("LTEAttachConfig::S: Initiating get sys info request.");
    action = qbi_svc_bc_ext_lte_attach_config_build_nas4d_req(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_wds93_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_check_current_rat
===========================================================================*/
/*!
    @brief Check whether current RAT is 3G

    @details

    @param nas_get_sys_info_resp_msg_v01

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_config_s_check_current_rat
(
  nas_get_sys_info_resp_msg_v01 *qmi_rsp
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_rsp);
  if ((qmi_rsp->cdma_srv_status_info_valid  &&
       qmi_rsp->cdma_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)||
      (qmi_rsp->hdr_srv_status_info_valid &&
       qmi_rsp->hdr_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01) ||
      (qmi_rsp->wcdma_srv_status_info_valid &&
       qmi_rsp->wcdma_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01)||
      (qmi_rsp->gsm_srv_status_info_valid &&
       qmi_rsp->gsm_srv_status_info.srv_status == NAS_SYS_SRV_STATUS_SRV_V01))
  {
    QBI_LOG_D_0("LTEAttachConfig::S: Device is in 3G or 2G.");
    return TRUE;
  }

  return FALSE;
}/* qbi_svc_bc_ext_lte_attach_config_s_check_current_rat */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_nas4d_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_GET_SYS_INFO_RESP_MSG_V01 response msg

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_nas4d_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_sys_info_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_lte_attach_config_info_s *info = NULL;
  wds_update_lte_attach_pdn_list_profiles_req_msg_v01* qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  qmi_rsp = (nas_get_sys_info_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;
    /* Update the info pointer whether current RAT is lte or not */
    info->rat_is_lte = qbi_svc_bc_ext_lte_attach_config_s_check_current_rat(qmi_rsp) ?
                         FALSE : TRUE;

    qmi_req = (wds_update_lte_attach_pdn_list_profiles_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(qmi_txn->parent, QBI_QMI_SVC_WDS,
        QMI_WDS_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES_REQ_V01,
        qbi_svc_bc_ext_lte_attach_config_s_wds9f_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
}/* qbi_svc_bc_ext_lte_attach_config_nas4d_rsp_cb */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_build_nas4d_req
===========================================================================
*/
/*!
    @brief Allocates and populates a QMI_NAS_GET_SYS_INFO_REQ_MSG_V01 
           request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_build_nas4d_req
(
  qbi_txn_s *txn
)
{
  nas_get_sys_info_req_msg_v01 *qmi_req = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  qmi_req = (nas_get_sys_info_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_NAS,
      QMI_NAS_GET_SYS_INFO_REQ_MSG_V01,
      qbi_svc_bc_ext_lte_attach_config_nas4d_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  action = QBI_SVC_ACTION_SEND_QMI_REQ;

  return action;
}/* qbi_svc_bc_ext_lte_attach_config_build_nas4d_req */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_wds9f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds9f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wds_update_lte_attach_pdn_list_profiles_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s *info = NULL;
  qbi_svc_bc_spdp_cache_s cache = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  QBI_LOG_D_0("LTEAttachConfig::S: Received update PDN list response");
  qmi_rsp = (wds_update_lte_attach_pdn_list_profiles_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachConfig::S: Received error code %d from QMI", 
      qmi_rsp->resp.error);
    cmd_in_progress_ignore_indication = FALSE;
  }
  else
  {
    info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;
    qbi_svc_bc_spdp_read_nv_store(qmi_txn->parent->ctx, &cache);
    /* Check if the RAT is 2G or 3G */
    if (!info->rat_is_lte)
    {
      /* Make decision whether to proceed with connect or to send
         trigger lte attach config query based on whether spdp
         flag is set */
      if (cache.spdp_support_flag != QBI_SVC_BC_SPDP_OPERATOR_NONE &&
          qmi_txn->parent->cmd_type == QBI_TXN_CMD_TYPE_INTERNAL)
      {
      /* In the Single PDP use case when SPDP flag is set and 
         command type is internal check if data session is active
         on the requested session id */
        action = qbi_svc_bc_spdp_s_check_activation_state(qmi_txn->parent);
      }
      else if (TRUE == info->skip_attach_pdn_list_ind)
      {
        QBI_LOG_D_1("LTEAttachConfig::S: PDN List is same %d, Triggering Query",
                    info->skip_attach_pdn_list_ind);
        action == qbi_svc_bc_ext_lte_attach_config_q_req(qmi_txn->parent);
      }
      else
      {
        /* CID is QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG 
           set Request */
        QBI_LOG_D_0("Waiting for UPDATE PDN List Indication to Trigger Query.");
        action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
      }
    }
    else
    {
      /* This is LTE case.Proceed to register for WDS event report indication
         when CID is MBIM_CID_CONNECT */
      if (cache.spdp_support_flag != QBI_SVC_BC_SPDP_OPERATOR_NONE &&
          qmi_txn->parent->cmd_type == QBI_TXN_CMD_TYPE_INTERNAL)
      {
        action = qbi_svc_bc_spdp_s_register_for_wds01_ind(qmi_txn->parent);
        if (action == QBI_SVC_ACTION_ABORT)
        {
          return action;
        }
      }

      QBI_LOG_D_0("LTEAttachConfig::S: Initiating detach.");
      action = qbi_svc_bc_ext_lte_attach_config_s_build_nas33_req(
        qmi_txn->parent, TRUE);
    }
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_wds9f_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_build_nas33_req
===========================================================================*/
/*!
  @brief Allocates and populates a QMI_WDS_MODIFY_PROFILE_SETTINGS request

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_build_nas33_req
(
  qbi_txn_s *txn,
  boolean flag
)
{
  nas_set_system_selection_preference_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  qmi_req = (nas_set_system_selection_preference_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_NAS,
                                    QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
                                    qbi_svc_bc_ext_lte_attach_config_s_nas33_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->change_duration_valid = TRUE;
  qmi_req->change_duration = NAS_POWER_CYCLE_V01;

  qmi_req->srv_domain_pref_valid = TRUE;

  qmi_req->srv_domain_pref = flag ?
      QMI_SRV_DOMAIN_PREF_PS_DETACH_V01 : QMI_SRV_DOMAIN_PREF_PS_ATTACH_V01;

  QBI_LOG_STR_1("LTEAttachConfig::S: Initiating %s",
                flag ? "Detach" : "Attach");

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_lte_attach_config_s_build_nas33_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_nas33_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_set_system_selection_preference_resp_msg_v01 *qmi_rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_set_system_selection_preference_resp_msg_v01 *)
      qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    cmd_in_progress_ignore_indication = FALSE;
  }

  return QBI_SVC_ACTION_WAIT_ASYNC_RSP;
} /* qbi_svc_bc_ext_lte_attach_config_s_nas33_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_build_wds29_req
===========================================================================*/
/*!
  @brief Allocates and populates a QMI_WDS_DELETE_PROFILE_REQ request

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_build_wds29_req
(
  qbi_txn_s *txn
)
{
  wds_delete_profile_req_msg_v01          *qmi_req = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s *info    = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;
  QBI_LOG_D_1("LTEAttachConfig::Send delete for profile index= %d",
    info->user_defined_profiles[info->index_user_defined_profiles]);

  qmi_req = (wds_delete_profile_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
    txn, QBI_QMI_SVC_WDS, QMI_WDS_DELETE_PROFILE_REQ_V01,
    qbi_svc_bc_ext_lte_attach_config_s_wds29_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->profile.profile_index = (uint8_t)
    info->user_defined_profiles[info->index_user_defined_profiles];

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_lte_attach_config_s_build_wds29_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_delete_user_defined_profile
===========================================================================*/
/*!
  @brief  Performs deletion of user created profiles for
  MBIM_CID_MS_LTE_ATTACH_CONFIG set request

  @details

  @param txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_delete_user_defined_profile
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_lte_attach_config_info_s *info = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)txn->info;
  QBI_LOG_I_2("LTEAttachConfig::max_user_defined_profiles = %d, "
    "index_user_defined_profiles = %d",
    info->max_user_defined_profiles, info->index_user_defined_profiles);
  if (info->index_user_defined_profiles < info->max_user_defined_profiles)
  {
    for (int i = 0; i < QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_MAX_PROFILE; i++)
    {
      if (info->user_defined_profiles[info->index_user_defined_profiles] ==
        info->element_match_index[i])
      {
        QBI_LOG_I_1("LTEAttachConfig::input matched,skip profile index = %d",
          info->element_match_index[i]);
        info->index_user_defined_profiles++;
        action = qbi_svc_bc_ext_lte_attach_config_s_delete_user_defined_profile(txn);
      }
    }

    QBI_LOG_I_1("LTEAttachConfig::Delete profile index = %d",
      info->index_user_defined_profiles);
    action = qbi_svc_bc_ext_lte_attach_config_s_build_wds29_req(txn);
  }
  else {
    QBI_LOG_D_0("LTEAttachConfig::S:: Triggering Query");
    action = qbi_svc_bc_ext_lte_attach_config_q_req(txn);
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_delete_user_defined_profile() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_wds29_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_DELETE_PROFILE_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds29_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_delete_profile_resp_msg_v01         *qmi_rsp = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_s *info    = NULL;
  qbi_svc_bc_ext_module_prov_cache_s      *cache   = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_delete_profile_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachConfig::S: E: Received error code %d from QMI",
                qmi_rsp->resp.error);
    qbi_svc_bc_ext_lte_attach_config_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }

  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)qmi_txn->parent->info;

  cache = qbi_svc_bc_ext_module_prov_cache_get(qmi_txn->parent->ctx,
    info->user_defined_profiles[info->index_user_defined_profiles]);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  QBI_MEMSET(cache, 0, sizeof(qbi_svc_bc_ext_module_prov_cache_s));

  info->index_user_defined_profiles++;

  return qbi_svc_bc_ext_lte_attach_config_s_delete_user_defined_profile(qmi_txn->parent);
} /* qbi_svc_bc_ext_lte_attach_config_s_wds29_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_wds95_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_LTE_ATTACH_PDN_LIST_IND, looking for changes to 
    the current channel rate or data system status that would trigger an
    MBIM_CID_PACKET_SERVICE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_wds95_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  cmd_in_progress_ignore_indication = FALSE;

  if (ind->qmi_svc_id < QBI_QMI_SVC_WDS_FIRST ||
      ind->qmi_svc_id > QBI_QMI_SVC_WDS_LAST)
  {
    QBI_LOG_E_1("LTEAttachConfig::I: qmi_svc_id %d is out of range",
                ind->qmi_svc_id);
  }
  else
  {
    QBI_LOG_D_0("LTEAttachConfig::I: Delete user defined profiles");
    action = qbi_svc_bc_ext_lte_attach_config_s_delete_user_defined_profile(ind->txn);
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_wds95_ind_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_s_nas24_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND, looking for
    changes to the current channel rate or data system status that would
    trigger an MBIM_CID_PACKET_SERVICE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_s_nas34_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e                                  action   = QBI_SVC_ACTION_ABORT;
  const nas_system_selection_preference_ind_msg_v01 *qmi_ind;
  qbi_svc_bc_ext_lte_attach_config_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn->info);

  qmi_ind = (const nas_system_selection_preference_ind_msg_v01 *)ind->buf->data;
  info = (qbi_svc_bc_ext_lte_attach_config_info_s *)ind->txn->info;

  /* When device is configured as PS only device, service domain preference
     will be none if PS detach was requested. QMI NAS in this case does not
     add service domain preference TLV. QBI shall consider this as PS detach
     successful. */
  if (!qmi_ind->srv_domain_pref_valid || (qmi_ind->srv_domain_pref_valid &&
    QMI_SRV_DOMAIN_PREF_CS_ONLY_V01 == qmi_ind->srv_domain_pref))
  {
    QBI_LOG_D_0("LTEAttachConfig::S: Detach successful. Trigger attach.");
    action = qbi_svc_bc_ext_lte_attach_config_s_build_nas33_req(
      ind->txn, FALSE);
  }
  else
  {
    if(TRUE == info->skip_attach_pdn_list_ind)
    {
      QBI_LOG_D_1("LTEAttachConfig::S: Attach successful AND "
                  "PDN List is same %d, Triggering Query",info->skip_attach_pdn_list_ind);
      action = qbi_svc_bc_ext_lte_attach_config_q_req(ind->txn);
    }
    else
    {
      QBI_LOG_D_0("LTEAttachConfig::S: Attach successful. "
                  "Waiting For UPDATE PDN LIST Ind");
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_s_nas24_ind_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_static_ind_e_wds95_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_LTE_ATTACH_PDN_LIST_IND, looking for changes to 
    the current channel rate or data system status that would trigger an
    MBIM_CID_PACKET_SERVICE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_config_static_ind_e_wds95_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  /*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  QBI_LOG_D_0("LTEAttachConfig::I Static Ind Received for UPDATE LIST");
  if (!cmd_in_progress_ignore_indication)
  {
    if (ind->qmi_svc_id < QBI_QMI_SVC_WDS_FIRST ||
        ind->qmi_svc_id > QBI_QMI_SVC_WDS_LAST)
    {
      QBI_LOG_E_1("LTEAttachConfig::I: qmi_svc_id %d is out of range",
                  ind->qmi_svc_id);
    }
    else
    {
      QBI_LOG_D_0("LTEAttachConfig::I: Triggering Query");
      action = qbi_svc_bc_ext_lte_attach_config_q_req(ind->txn);
    }
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_config_static_ind_e_wds95_ind_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_MS_LTE_ATTACH_STATUS
    @{ */

/*===========================================================================
FUNCTION: qbi_svc_lte_attach_status_q_match_ip_type
===========================================================================*/
/*!
    @brief Compares MBIM IP type with QMI PDP type

  @details

  @param ip_type

  @return wds_pdp_type_enum_v01
*/
/*=========================================================================*/
static wds_pdp_type_enum_v01 qbi_svc_lte_attach_status_q_match_ip_type
(
  wds_ip_support_type_enum_v01 ip_type,
  wds_pdp_type_enum_v01 pdp_type
)
{
/*-------------------------------------------------------------------------*/

  QBI_LOG_D_2("LTEAttachStatus::Q: Matching ip_type (%d) <==> pdp_type (%d).",
              ip_type, pdp_type);

  switch (ip_type)
  {
  case WDS_IP_SUPPORT_TYPE_IPV4_V01:
    if (WDS_PDP_TYPE_PDP_IPV4_V01 == pdp_type ||
        WDS_PDP_TYPE_PDP_IPV4V6_V01 == pdp_type)
    {
      return TRUE;
    }
  case WDS_IP_SUPPORT_TYPE_IPV6_V01:
    if (WDS_PDP_TYPE_PDP_IPV6_V01 == pdp_type ||
        WDS_PDP_TYPE_PDP_IPV4V6_V01 == pdp_type)
    {
      return TRUE;
    }
  case WDS_IP_SUPPORT_TYPE_IPV4V6_V01:
    if (WDS_PDP_TYPE_PDP_IPV4_V01 == pdp_type ||
        WDS_PDP_TYPE_PDP_IPV6_V01 == pdp_type ||
        WDS_PDP_TYPE_PDP_IPV4V6_V01 == pdp_type)
    {
      return TRUE;
    }
  default:
    break;
  }
  if ((WDS_IP_SUPPORT_TYPE_IPV4_V01 == ip_type &&
       WDS_PDP_TYPE_PDP_IPV4_V01 == pdp_type) ||
      (WDS_IP_SUPPORT_TYPE_IPV6_V01 == ip_type &&
       WDS_PDP_TYPE_PDP_IPV6_V01 == pdp_type) ||
      (WDS_IP_SUPPORT_TYPE_IPV4V6_V01 == ip_type &&
       WDS_PDP_TYPE_PDP_IPV4V6_V01 == pdp_type))
  {
    return TRUE;
  }

  QBI_LOG_D_0("LTEAttachStatus::Q: IP type did not match");
  return FALSE;
} /* qbi_svc_lte_attach_status_q_match_ip_type */

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
static wds_pdp_type_enum_v01 qbi_svc_lte_attach_status_q_ip_type_to_pdp_type
(
  wds_ip_support_type_enum_v01 ip_type
)
{
  wds_pdp_type_enum_v01 pdp_type = WDS_PDP_TYPE_PDP_NON_IP_V01;
/*-------------------------------------------------------------------------*/
  switch (ip_type)
  {
  case WDS_IP_SUPPORT_TYPE_IPV4_V01:
    pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
    break;

  case WDS_IP_SUPPORT_TYPE_IPV6_V01:
    pdp_type = WDS_PDP_TYPE_PDP_IPV4_V01;
    break;

  case WDS_IP_SUPPORT_TYPE_IPV4V6_V01:
    pdp_type = WDS_PDP_TYPE_PDP_IPV4V6_V01;
    break;

  default:
    break;
  }

  QBI_LOG_D_2("LTEAttachStatus::Q: Returning PDP type (%d) for IP type (%d)",
              pdp_type, ip_type);

  return pdp_type;
} /* qbi_svc_bc_ext_ip_type_to_pdp_type */

/*===========================================================================
FUNCTION: qbi_svc_lte_attach_status_q_qmi_profile_to_mbim_auth_proto
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
static uint32 qbi_svc_lte_attach_status_q_qmi_profile_to_mbim_auth_proto
(
  wds_profile_type_enum_v01                                 profile_type,
  const qbi_svc_bc_ext_lte_attach_status_profile_settings_s profile_settings
)
{
  uint32 mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_NONE;
/*-------------------------------------------------------------------------*/
  if (profile_type == WDS_PROFILE_TYPE_3GPP_V01 &&
      profile_settings.authentication_preference_valid)
  {
    if (profile_settings.authentication_preference &
        QMI_WDS_MASK_AUTH_PREF_CHAP_V01)
    {
      mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_CHAP;
    }
    else if (profile_settings.authentication_preference &
               QMI_WDS_MASK_AUTH_PREF_PAP_V01)
    {
      mbim_auth_proto = QBI_SVC_BC_AUTH_PROTOCOL_PAP;
    }
  }
  else
  {
    QBI_LOG_E_1("Invalid profile type %d", profile_type);
  }

  return mbim_auth_proto;
} /* qbi_svc_lte_attach_status_q_qmi_profile_to_mbim_auth_proto() */

/*===========================================================================
FUNCTION: qbi_svc_lte_attach_status_q_qmi_profile_to_mbim_compression
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
static uint32 qbi_svc_lte_attach_status_q_qmi_profile_to_mbim_compression
(
  wds_profile_type_enum_v01                                 profile_type,
  const qbi_svc_bc_ext_lte_attach_status_profile_settings_s profile_settings
)
{
  uint32 mbim_compression = QBI_SVC_BC_COMPRESSION_NONE;
  /*-------------------------------------------------------------------------*/
  if (profile_type == WDS_PROFILE_TYPE_EPC_V01 &&
      ((profile_settings.pdp_data_compression_type_valid &&
        profile_settings.pdp_data_compression_type !=
            WDS_PDP_DATA_COMPR_TYPE_OFF_V01) ||
       (profile_settings.pdp_hdr_compression_type_valid &&
        profile_settings.pdp_hdr_compression_type !=
            WDS_PDP_HDR_COMPR_TYPE_OFF_V01)))
  {
    mbim_compression = QBI_SVC_BC_COMPRESSION_ENABLE;
  }

  return mbim_compression;
} /* qbi_svc_lte_attach_status_q_qmi_profile_to_mbim_compression() */

/*===========================================================================
FUNCTION: qbi_svc_lte_attach_status_q_get_last_active_profile_from_cache
===========================================================================*/
/*!
  @brief Allocates and populates a QMI_WDS_DELETE_PROFILE_REQ request

  @details

  @param txn

  @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_lte_attach_status_q_get_last_active_profile_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_ext_lte_attach_status_info_s  *info;
  qbi_svc_bc_ext_module_prov_cache_s       *cache = NULL;
  uint32                                        i = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->info);

  for (i = 0; i < QMI_WDS_PROFILE_LIST_MAX_V01; i++)
  {
    cache = qbi_svc_bc_ext_module_prov_cache_get(txn->ctx, i);
    QBI_CHECK_NULL_PTR_RET_FALSE(cache);

    if (cache->lte_attach_state == QBI_SVC_MBIM_MS_LTE_ATTACH_STATE_ATTACHED)
    {
      info = (qbi_svc_bc_ext_lte_attach_status_info_s *)txn->info;

      info->num_of_profile = 1;
      info->profile_index[0] = i;
      info->profiles_read = 0;

      QBI_LOG_D_1("LTEAttachStatus::Q: Last LTE Attach profile index %d.", i);
      return TRUE;
    }
  }

  return FALSE;
} /* qbi_svc_lte_attach_status_q_get_last_active_profile_from_cache() */

/*===========================================================================
FUNCTION: qbi_svc_lte_attach_status_q_update_cache
===========================================================================*/
/*!
  @brief Updates cache with LTE attach status

  @details

  @param txn

  @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_lte_attach_status_q_update_cache
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_req_msg_v01 *qmi_req = NULL;
  qbi_svc_bc_ext_module_prov_cache_s     *cache = NULL;
  uint32                                i       = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->req.data);

  qmi_req = (wds_get_profile_settings_req_msg_v01 *)qmi_txn->req.data;
  for (i = 0; i < QMI_WDS_PROFILE_LIST_MAX_V01; i++)
  {
    cache = qbi_svc_bc_ext_module_prov_cache_get(qmi_txn->ctx, i);
    QBI_CHECK_NULL_PTR_RET_FALSE(cache);

    cache->lte_attach_state = (i == qmi_req->profile.profile_index) ?
        QBI_SVC_MBIM_MS_LTE_ATTACH_STATE_ATTACHED :
        QBI_SVC_MBIM_MS_LTE_ATTACH_STATE_DETACHED;
  }

  qbi_svc_bc_ext_update_nv_store(qmi_txn->ctx);

  return TRUE;
} /* qbi_svc_lte_attach_status_q_update_cache() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_status_q_populate_profile_settings
===========================================================================*/
/*!
    @brief Populates profile settings from QMI_WDS_GET_PROFILE_SETTINGS_RESP
    for MBIM_CID_MS_LTE_ATTACH_CONFIG query

    @details

    @param qmi_rsp
    @param profile_settings

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_lte_attach_status_q_populate_profile_settings
(
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp,
  qbi_svc_bc_ext_lte_attach_status_profile_settings_s *profile_settings
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_rsp);
  QBI_CHECK_NULL_PTR_RET_FALSE(profile_settings);

  profile_settings->pdp_type_valid = qmi_rsp->pdp_type_valid;
  profile_settings->pdp_type = qmi_rsp->pdp_type;
  profile_settings->authentication_preference_valid =
      qmi_rsp->authentication_preference_valid;
  profile_settings->authentication_preference =
      qmi_rsp->authentication_preference;
  profile_settings->pdp_data_compression_type_valid =
      qmi_rsp->pdp_data_compression_type_valid;
  profile_settings->pdp_data_compression_type =
      qmi_rsp->pdp_data_compression_type;
  profile_settings->pdp_hdr_compression_type_valid =
      qmi_rsp->pdp_hdr_compression_type_valid;
  profile_settings->pdp_hdr_compression_type =
      qmi_rsp->pdp_hdr_compression_type;
  profile_settings->apn_name_valid = qmi_rsp->apn_name_valid;
  profile_settings->username_valid = qmi_rsp->username_valid;
  profile_settings->password_valid = qmi_rsp->password_valid;

  if (profile_settings->apn_name_valid)
  {
    QBI_STRLCPY(profile_settings->apn_name,
                qmi_rsp->apn_name, sizeof(qmi_rsp->apn_name));
  }

  if (profile_settings->username_valid)
  {
    QBI_STRLCPY(profile_settings->username,
                qmi_rsp->username, sizeof(qmi_rsp->username));
  }
  if (profile_settings->password_valid)
  {
    QBI_STRLCPY(profile_settings->password,
                qmi_rsp->password, sizeof(qmi_rsp->password));
  }

  return TRUE;
} /* qbi_svc_bc_ext_lte_attach_status_q_populate_profile_settings() */

/*===========================================================================
FUNCTION: qbi_svc_lte_attach_status_prepare_rsp
===========================================================================*/
/*!
    @brief Allocates and populates a MBIM_MS_LTE_ATTACH_STATUS structure on 
    the response

    @details

    @param qmi_txn
    @param profile_settings

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_lte_attach_status_prepare_rsp
(
  qbi_qmi_txn_s *qmi_txn,
  qbi_svc_bc_ext_lte_attach_status_profile_settings_s profile_settings
)
{
  boolean                                 success        = FALSE;
  qbi_svc_bc_ext_lte_attach_status_rsp_s  *rsp;
  qbi_svc_bc_ext_lte_attach_status_info_s *info;
  uint32                                  initial_offset = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn->parent->info);

  rsp = (qbi_svc_bc_ext_lte_attach_status_rsp_s *)qbi_txn_alloc_rsp_buf(
      qmi_txn->parent, sizeof(qbi_svc_bc_ext_lte_attach_status_rsp_s));
  QBI_CHECK_NULL_PTR_RET_FALSE(rsp);

  info = (qbi_svc_bc_ext_lte_attach_status_info_s *)qmi_txn->parent->info;

  if (info->status_registered)
  {
    if (profile_settings.pdp_type_valid)
    {
      switch (profile_settings.pdp_type)
      {
      case WDS_PDP_TYPE_PDP_IPV4_V01:
        rsp->ip_type = QBI_SVC_BC_IP_TYPE_IPV4;
        break;
      case WDS_PDP_TYPE_PDP_IPV6_V01:
        rsp->ip_type = QBI_SVC_BC_IP_TYPE_IPV6;
        break;
      case WDS_PDP_TYPE_PDP_IPV4V6_V01:
        rsp->ip_type = QBI_SVC_BC_IP_TYPE_IPV4V6;
        break;
      default:
        break;
      }
    }

    rsp->lte_attach_state = info->lte_attach_state;

    rsp->compression = qbi_svc_lte_attach_status_q_qmi_profile_to_mbim_compression(
        WDS_PROFILE_TYPE_EPC_V01, profile_settings);
    rsp->auth_protocol = qbi_svc_lte_attach_status_q_qmi_profile_to_mbim_auth_proto(
        WDS_PROFILE_TYPE_3GPP_V01, profile_settings);

    /* Populate the DataBuffer */
    if (profile_settings.apn_name_valid &&
        !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
            qmi_txn->parent, &rsp->access_string, initial_offset,
            QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES, profile_settings.apn_name,
            sizeof(profile_settings.apn_name)))
    {
      QBI_LOG_E_0("Couldn't add 3GPP AccessString to response!");
      success = FALSE;
    }
    else if (profile_settings.username_valid &&
               !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
            qmi_txn->parent, &rsp->username, initial_offset,
            QBI_SVC_BC_USERNAME_MAX_LEN_BYTES, profile_settings.username,
            sizeof(profile_settings.username)))
    {
      QBI_LOG_E_0("Couldn't add 3GPP Username to response!");
      success = FALSE;
    }
    else if (profile_settings.password_valid &&
               !qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
            qmi_txn->parent, &rsp->password, initial_offset,
            QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES,
            profile_settings.password, sizeof(profile_settings.password)))
    {
      QBI_LOG_E_0("Couldn't add 3GPP Password to response!");
      success = FALSE;
    }
    else
    {
      success = TRUE;
    }

    if (success && QBI_TXN_CMD_TYPE_INTERNAL == qmi_txn->parent->cmd_type)
    {
      qbi_util_buf_s buf;
      qbi_util_buf_init(&buf);
      qbi_util_buf_alloc(&buf, qmi_txn->parent->infobuf_len_total);
      QBI_CHECK_NULL_PTR_RET_FALSE(buf.data);
      (void)qbi_txn_rsp_databuf_extract(
          qmi_txn->parent, buf.data, buf.size, 0);
      qbi_util_buf_free(&qmi_txn->parent->rsp);
      qmi_txn->parent->rsp = buf;
    }
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_svc_lte_attach_status_prepare_rsp() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_status_q_get_next_profile
===========================================================================*/
/*!
    @brief Retrive next available configured profile.

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_get_next_profile
(
  qbi_txn_s *txn
)
{
  wds_get_profile_settings_req_msg_v01    *qmi_req_wds2b;
  qbi_svc_bc_ext_lte_attach_status_info_s *info;
  qbi_svc_action_e                         action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_bc_ext_lte_attach_status_info_s *)txn->info;
  if (info->profiles_read >= info->num_of_profile)
  {
    QBI_LOG_E_0("No LTE attach profile found.");
  }
  else
  {
    /* Issue a query to retrieve the profile details */
    qmi_req_wds2b = (wds_get_profile_settings_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
        qbi_svc_bc_ext_lte_attach_status_q_wds2b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_wds2b);

    qmi_req_wds2b->profile.profile_index = (uint8_t)
        info->profile_index[info->profiles_read];

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_status_q_get_next_profile */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_lte_attach_status_q_req
===========================================================================*/
/*!
    @brief Handles a QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_STATUS query
    request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_req
(
  qbi_txn_s *txn
)
{
  wds_get_lte_attach_params_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (txn->info == NULL)
  {
    txn->info = QBI_MEM_MALLOC_CLEAR(
        sizeof(qbi_svc_bc_ext_lte_attach_status_info_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
  }

  qmi_req = (wds_get_lte_attach_params_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WDS,
                                    QMI_WDS_GET_LTE_ATTACH_PARAMS_REQ_V01,
                                    qbi_svc_bc_ext_lte_attach_status_q_wds85_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_lte_attach_status_q_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_status_q_dsd24_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_DSD_GET_SYSTEM_STATUS_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_dsd24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                                    action           = QBI_SVC_ACTION_ABORT;
  dsd_get_system_status_resp_msg_v01                  *qmi_rsp         = NULL;
  qbi_svc_bc_ext_lte_attach_status_info_s             *info            = NULL;
  uint32                                              i                = 0;
  qbi_svc_bc_ext_lte_attach_status_profile_settings_s profile_settings = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (dsd_get_system_status_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachStatus::Q: E:Received error code %d from QMI",
                qmi_rsp->resp.error);
  }
  else if (qmi_rsp->avail_sys_valid)
  {
    info = (qbi_svc_bc_ext_lte_attach_status_info_s *)qmi_txn->parent->info;

    for (i = 0; !info->status_registered && i < qmi_rsp->avail_sys_len; i++)
    {
      switch (qmi_rsp->avail_sys[i].rat_value)
      {
      case DSD_SYS_RAT_EX_3GPP_WCDMA_V01:
      case DSD_SYS_RAT_EX_3GPP_GERAN_V01:
      case DSD_SYS_RAT_EX_3GPP_TDSCDMA_V01:
      case DSD_SYS_RAT_EX_3GPP2_1X_V01:
      case DSD_SYS_RAT_EX_3GPP2_HRPD_V01:
      case DSD_SYS_RAT_EX_3GPP2_EHRPD_V01:
        QBI_LOG_D_1("LTEAttachStatus::Q: Device is registered on RAT: %d.",
                    qmi_rsp->avail_sys[i].rat_value);
        info->status_registered = TRUE;
        break;
      default:
        break;
      }
    }

    info->lte_attach_state = QBI_SVC_MBIM_MS_LTE_ATTACH_STATE_DETACHED;
    if (info->status_registered)
    {
      if (qbi_svc_lte_attach_status_q_get_last_active_profile_from_cache(qmi_txn->parent))
      {
        action = qbi_svc_bc_ext_lte_attach_status_q_get_next_profile(qmi_txn->parent);
      } else
      {
        info->status_registered = FALSE;
        if (qbi_svc_lte_attach_status_prepare_rsp(qmi_txn, profile_settings))
        {
          QBI_LOG_D_0("LTEAttachStatus::Q: Device had never camped on LTE.");
          action = QBI_SVC_ACTION_SEND_RSP;
        }
      }
    }
    else
    {
      QBI_LOG_D_0("LTEAttachStatus::Q: Device is not registered on any RAT.");
      if (qbi_svc_lte_attach_status_prepare_rsp(qmi_txn, profile_settings))
      {
        action = QBI_SVC_ACTION_SEND_RSP;
      }
    }
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_status_q_dsd24_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_status_q_wds85_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_LTE_ATTACH_PARAMS_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_wds85_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                           action       = QBI_SVC_ACTION_ABORT;
  wds_get_lte_attach_params_resp_msg_v01     *qmi_rsp     = NULL;
  qbi_svc_bc_ext_lte_attach_status_info_s    *info        = NULL;
  dsd_get_system_status_req_msg_v01          *qmi_req_dsd = NULL;
  wds_get_lte_max_attach_pdn_num_req_msg_v01 *qmi_req     = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_lte_attach_params_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if (qmi_rsp->resp.error == QMI_ERR_INFO_UNAVAILABLE_V01)
    {
      QBI_LOG_E_0("LTEAttachStatus::Q: LTE not attached. "
                  "Check for other available data capable RATs");
      qmi_req_dsd = (dsd_get_system_status_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(qmi_txn->parent, QBI_QMI_SVC_DSD,
                                        QMI_DSD_GET_SYSTEM_STATUS_REQ_V01,
                                        qbi_svc_bc_ext_lte_attach_status_q_dsd24_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_dsd);

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
    else
    {
      QBI_LOG_E_1("LTEAttachStatus::Q: E:Received error code %d from QMI",
                  qmi_rsp->resp.error);
    }
  }
  else
  {
    info = (qbi_svc_bc_ext_lte_attach_status_info_s *)qmi_txn->parent->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(info);

    info->status_registered = TRUE;
    info->lte_attach_state = QBI_SVC_MBIM_MS_LTE_ATTACH_STATE_ATTACHED;

    if (qmi_rsp->apn_string_valid)
    {
      QBI_STRLCPY(info->apn_name, qmi_rsp->apn_string,
                  sizeof(qmi_rsp->apn_string));

      info->ip_type = qmi_rsp->ip_type;
      QBI_LOG_STR_1("LTEAttachStatus::Q: APN: %s", info->apn_name);

      qmi_req = (wds_get_lte_max_attach_pdn_num_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(qmi_txn->parent, QBI_QMI_SVC_WDS,
                                        QMI_WDS_GET_LTE_MAX_ATTACH_PDN_NUM_REQ_V01,
                                        qbi_svc_bc_ext_lte_attach_status_q_wds92_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
    else
    {
      QBI_LOG_E_0("LTEAttachStatus::Q: E: APN TLV missing");
    }
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_status_q_wds85_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_status_q_wds92_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_LTE_MAX_ATTACH_PDN_NUM_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_wds92_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                            action   = QBI_SVC_ACTION_ABORT;
  wds_get_lte_attach_pdn_list_req_msg_v01     *qmi_req = NULL;
  wds_get_lte_max_attach_pdn_num_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_lte_attach_status_info_s     *info    = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_lte_max_attach_pdn_num_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachStatus::Q: E:Received error code %d from QMI",
                qmi_rsp->resp.error);
  }
  else
  {
    info = (qbi_svc_bc_ext_lte_attach_status_info_s *)qmi_txn->parent->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(info);
    info->max_supported_profile_num = qmi_rsp->max_attach_pdn_num;
    QBI_LOG_D_1("LTEAttachStatus::Q: max_supported_profile_num: %d",
                info->max_supported_profile_num);

    qmi_req = (wds_get_lte_attach_pdn_list_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_WDS,
        QMI_WDS_GET_LTE_ATTACH_PDN_LIST_REQ_V01,
        qbi_svc_bc_ext_lte_attach_status_q_wds94_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  return action;
} /* qbi_svc_bc_ext_lte_attach_status_q_wds92_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_status_q_wds94_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_LTE_ATTACH_PDN_LIST_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_wds94_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                         action   = QBI_SVC_ACTION_ABORT;
  wds_get_lte_attach_pdn_list_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_lte_attach_status_info_s  *info    = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_lte_attach_pdn_list_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachStatus::Q: E:Received error code %d from QMI",
                qmi_rsp->resp.error);
  }
  else
  {
    info = (qbi_svc_bc_ext_lte_attach_status_info_s *)qmi_txn->parent->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(info);
    if (qmi_rsp->attach_pdn_list_valid)
    {
      info->num_of_profile = qmi_rsp->attach_pdn_list_len;
    }

    if (info->num_of_profile <= info->max_supported_profile_num)
    {
      uint32 profile_count = 0;

      info->num_of_profile = qmi_rsp->attach_pdn_list_len;

      for (profile_count = 0; profile_count < info->num_of_profile; profile_count++)
      {
        info->profile_index[profile_count] =
            qmi_rsp->attach_pdn_list[profile_count];
        QBI_LOG_D_2("LTEAttachStatus::Q: Adding profile_index[%d]: %d",
                    profile_count, info->profile_index[profile_count]);
        break;
      }

      QBI_LOG_D_1("LTEAttachStatus::Q: num_of_profile: %d", info->num_of_profile);

      info->profiles_read = 0;
      action = qbi_svc_bc_ext_lte_attach_status_q_get_next_profile(qmi_txn->parent);
    }
    else
    {
      QBI_LOG_E_2("LTEAttachStatus::Q: Attach PDN List is either empty or "
                  "exceeds max supported PDN. attach_pdn_list_len = %d and "
                  "max_supported_profile_num = %d", info->num_of_profile,
                  info->max_supported_profile_num);
    }
  }
  return action;
} /* qbi_svc_bc_ext_lte_attach_status_q_wds94_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_status_q_wds2b_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_GET_PROFILE_SETTINGS_RESP for
    MBIM_CID_MS_LTE_ATTACH_CONFIG query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_q_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_resp_msg_v01               *qmi_rsp         = NULL;
  qbi_svc_bc_ext_lte_attach_status_info_s             *info            = NULL;
  qbi_svc_action_e                                    action           = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_lte_attach_status_profile_settings_s profile_settings = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("LTEAttachStatus::Q: Received error code %d from QMI",
                qmi_rsp->resp.error);
    qbi_svc_bc_ext_lte_attach_config_set_mbim_error_status(
        qmi_txn->parent, qmi_rsp->resp.error,
        qmi_rsp->extended_error_code_valid, qmi_rsp->extended_error_code);
  }
  else
  {
    info = (qbi_svc_bc_ext_lte_attach_status_info_s *)qmi_txn->parent->info;
    qbi_svc_bc_ext_lte_attach_status_q_populate_profile_settings(
        qmi_rsp, &profile_settings);

    if (qbi_svc_lte_attach_status_q_match_ip_type(
            info->ip_type, profile_settings.pdp_type))
    {
      if (qbi_svc_bc_ext_lte_attach_match_string(profile_settings.apn_name_valid,
        profile_settings.apn_name, info->apn_name, TRUE))
      {
        QBI_LOG_D_0("LTEAttachStatus::Q: Attach profile found.");
        qbi_svc_lte_attach_status_q_update_cache(qmi_txn);
      }
      else
      {
        QBI_LOG_D_0("LTEAttachStatus::Q: Default attach profile. Add APN and "
                    "IP type from LTE attach params to response.");

        QBI_MEMSET(&profile_settings, 0,
                   sizeof(qbi_svc_bc_ext_lte_attach_status_profile_settings_s));

        profile_settings.apn_name_valid = 1;
        profile_settings.pdp_type_valid = 1;
        QBI_STRLCPY(profile_settings.apn_name,
                    info->apn_name, sizeof(info->apn_name));
        profile_settings.pdp_type =
            qbi_svc_lte_attach_status_q_ip_type_to_pdp_type(info->ip_type);
      }

      if (qbi_svc_lte_attach_status_prepare_rsp(qmi_txn, profile_settings))
      {
        action = QBI_SVC_ACTION_SEND_RSP;
      }
    }
    else if (QBI_SVC_MBIM_MS_LTE_ATTACH_STATE_DETACHED == info->lte_attach_state)
    {
      if (qbi_svc_lte_attach_status_prepare_rsp(qmi_txn, profile_settings))
      {
        action = QBI_SVC_ACTION_SEND_RSP;
      }
    }
    else
    {
      info->profiles_read++;
      action = qbi_svc_bc_ext_lte_attach_status_q_get_next_profile(
          qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_status_q_wds2b_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_open_configure_qmi_inds_uim2e_cb
===========================================================================*/
/*!
    @brief Handles QMI_UIM_EVENT_REG_RESP

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_open_configure_qmi_inds_uim2e_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_event_reg_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e           action   = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("qbi_svc_bc_ext_open_configure_qmi_inds_uim2e_cb");
  qmi_rsp = (uim_event_reg_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if (qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
    {
      QBI_LOG_E_1("Error registering for UIM indications!!! Error code %d",
                  qmi_rsp->resp.error);
    } else
    {
      QBI_LOG_D_0("UIM indication registration already complete");
    }

    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else if ((qmi_rsp->event_mask_valid) &&
           !((qmi_rsp->event_mask & (1 << QMI_UIM_EVENT_CARD_STATUS_BIT_V01)) && 
             (qmi_rsp->event_mask & (1 << QMI_UIM_EVENT_PHYSICAL_SLOT_STATUS_BIT_V01))))
  {
    QBI_LOG_E_1("QMI event registration failed. Returned event mask 0x%08x",
                qmi_rsp->event_mask);

    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    QBI_LOG_D_2("QMI responded and got registered to the expected "
      "event! Returned event mask 0x%08x and decimal value : %d", qmi_rsp->event_mask, qmi_rsp->event_mask);
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_bc_ext_lte_attach_open_configure_qmi_inds_uim2e_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_status_reg_ind_dsd25_rsp
===========================================================================*/
/*!
  @brief Handles QMI_DSD_SYSTEM_STATUS_CHANGE_RESP

  @details

  @param qmi_txn

  @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_reg_ind_dsd25_rsp
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e                      action   = QBI_SVC_ACTION_ABORT;
  dsd_system_status_change_resp_msg_v01 *qmi_rsp = NULL;
  uim_event_reg_req_msg_v01             *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  QBI_LOG_D_0("Received QMI_DSD_SYSTEM_STATUS_CHANGE_RESP");

  qmi_rsp = (dsd_system_status_change_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    /* Register for QMI_UIM_STATUS_CHANGE_IND */
    qmi_req = (uim_event_reg_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_EVENT_REG_REQ_V01,
        qbi_svc_bc_ext_lte_attach_open_configure_qmi_inds_uim2e_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
    qmi_req->event_mask = (1 << QMI_UIM_EVENT_CARD_STATUS_BIT_V01);
    qmi_req->event_mask |= (1 << QMI_UIM_EVENT_PHYSICAL_SLOT_STATUS_BIT_V01);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  return action;
} /* qbi_svc_bc_ext_lte_attach_status_reg_ind_dsd25_rsp() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_status_e_dsd26_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_DSD_SYSTEM_STATUS_IND, looking for changes to the
    current channel rate or data system status that would trigger an
    MBIM_CID_PACKET_SERVICE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_lte_attach_status_e_dsd26_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);

  QBI_LOG_D_0("Received DSD system status indication");
  return qbi_svc_bc_ext_lte_attach_status_q_req(ind->txn);
} /* qbi_svc_bc_ext_lte_attach_status_e_dsd26_ind_cb() */

/*! @} */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_lte_attach_config_set_mbim_error_status
===========================================================================*/
/*!
    @brief Attempts to map QMI error information into a descriptive MBIM
    error status for MBIM_CID_MS_LTE_ATTACH_CONFIG

    @details

    @param txn
    @param qmi_error
    @param qmi_error_ds_ext_valid
    @param qmi_error_ds_ext
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_lte_attach_config_set_mbim_error_status
(
  qbi_txn_s                          *txn,
  qmi_error_type_v01                  qmi_error,
  uint8_t                             qmi_error_ds_ext_valid,
  wds_ds_extended_error_code_enum_v01 qmi_error_ds_ext
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  /* For all aborts setting the status, hence setting this to
  FALSE to handle new request */
  cmd_in_progress_ignore_indication = FALSE;

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
} /* qbi_svc_bc_ext_lte_attach_config_set_mbim_error_status() */
