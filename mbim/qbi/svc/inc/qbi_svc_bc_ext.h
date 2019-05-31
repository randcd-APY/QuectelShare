/*!
  @file
  qbi_svc_bc_ext.h

  @brief
  Basic Connectivity Extension device service definitions.
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
05/24/18  nk   Added cache sturcture for location info status
10/09/17  nk   Fixed warnings and KW p1 issues
09/08/17  vk   Exposing BC EXT cache for usage in BC
07/26/17  mm   Added header files and moved function proto here
06/02/17  vk   Added module
=============================================================================*/

#ifndef QBI_SVC_BC_EXT_H
#define QBI_SVC_BC_EXT_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc.h"
#include "qbi_txn.h"
#include "wireless_data_service_v01.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*! Maximum number of simultaneous data sessions supported (per QBI context) */
#define QBI_SVC_BC_EXT_MAX_SESSIONS (8)

/*! @addtogroup MBIM_CID_MS_SYS_CAPS
    @{ */
/* Currently supporting only 1 executor */
#define QBI_SVC_BC_EXT_DSSA_MAX_SUPPORTED_EXECUTORS             (1)
/* For DSSA feature only one executor will be supported and concurrency
will be always one. */
#define QBI_SVC_BC_EXT_DSSA_CONCURRENCY                         (1)
/*! @} */

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Cache used locally by CIDs processed in this file. This is a child of the
main qbi_svc_bc_ext_cache_s structure */

typedef enum {
  QBI_SVC_BC_EXT_ROAMING_FLAG_MIN = 0x00,

  QBI_SVC_BC_EXT_ROAMING_FLAG_HOME = 0x01,
  QBI_SVC_BC_EXT_ROAMING_FLAG_PARTNER = 0x02,
  QBI_SVC_BC_EXT_ROAMING_FLAG_NON_PARTNER = 0x04,

  QBI_SVC_BC_EXT_ROAMING_FLAG_MAX
} qbi_svc_bc_ext_roaming_flag_e;

typedef enum {
  QBI_SVC_BC_EXT_CONTEXT_FLAG_MIN = 0,

  QBI_SVC_BC_EXT_CONTEXT_FLAG_MODEM = 1,
  QBI_SVC_BC_EXT_CONTEXT_FLAG_USER_DEFINED = 2,
  QBI_SVC_BC_EXT_CONTEXT_FLAG_USER_MODIFIED = 3,

  QBI_SVC_BC_EXT_CONTEXT_FLAG_MAX
} qbi_svc_bc_ext_flag_e;

typedef struct {
/*This cache is for prov context and LTE*/
  uint32 ip_type;
  uint32 source;
  uint32 roaming;
  uint32 media_type;
  uint32 enable;
  uint32 prov_active;
  uint32 lte_active;
  uint32 lte_attach_state;
  uint32 roaming_flag;
  uint32 context_flag;
} qbi_svc_bc_ext_module_prov_cache_s;

typedef struct {
/*This cache is for location info*/
  uint32 location_area_code;
  uint32 tracking_area_code;
} qbi_svc_bc_ext_module_location_info_cache_s;

typedef PACK(struct) {
  qbi_svc_bc_ext_module_prov_cache_s  prov_cache[QMI_WDS_PROFILE_LIST_MAX_V01];
  qbi_svc_bc_ext_module_location_info_cache_s location_info_cache;
} qbi_svc_bc_ext_cache_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

wds_auth_pref_mask_v01 qbi_svc_bc_ext_connect_mbim_auth_pref_to_qmi_auth_pref
(
  uint32 auth_protocol
);

wds_pdp_type_enum_v01 qbi_svc_bc_ext_ip_type_to_pdp_type
(
  uint32 ip_type
);

boolean qbi_svc_bc_ext_match_pdp_type
(
  uint32                                 ip_type,
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp
);

void qbi_svc_bc_ext_update_nv_store
(
  qbi_ctx_s *ctx
);

boolean qbi_svc_bc_ext_update_cache_ip_type
(
  qbi_txn_s                           *txn,
  const uint32                         operation,
  qbi_svc_bc_ext_module_prov_cache_s  *cache,
  const uint32                         ip_type,
  const wds_pdp_type_enum_v01          pdp_type
);

qbi_svc_bc_ext_cache_s *qbi_svc_bc_ext_cache_get
(
  qbi_ctx_s *ctx
);

qbi_svc_action_e qbi_svc_bc_ext_module_prov_cache_clear
(
  qbi_txn_s *txn
);

qbi_svc_bc_ext_module_prov_cache_s *qbi_svc_bc_ext_module_prov_cache_get
(
  qbi_ctx_s *ctx,
  const uint32 cache_index
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_provision_card
===========================================================================*/
/*!
    @brief Configures card provision sessions

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_provision_card
(
  qbi_txn_s *txn
);


/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_init
===========================================================================*/
/*!
    @brief One-time initialization of the Basic Connectivity Extension device
    service

    @details

*/
/*=========================================================================*/
void qbi_svc_bc_ext_init
(
  void
);

#endif /* QBI_SVC_BC_EXT_H */

