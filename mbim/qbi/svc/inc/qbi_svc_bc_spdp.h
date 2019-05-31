/*!
  @file
  qbi_svc_bc_spdp.h

  @brief
  Basic Connectivity device service implementation for single PDP
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

$Header: $

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
11/29/17  rv   Avoiding deregister in case of 3G for LTE ATTACH
10/28/17  rv   Avoid deregister in case of 3G
08/10/17  vk   Added file
=============================================================================*/

#ifndef QBI_SVC_BC_SPDP_H
#define QBI_SVC_BC_SPDP_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_txn.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*! Maximum number of digits of actual IMSI value in EF-IMSI */
#define QBI_SVC_BC_SPDP_EF_IMSI_MAX_DIGITS    (15)

/*=============================================================================

  Typedefs

=============================================================================*/

typedef PACK(struct) {
  uint32 spdp_support_flag;
  uint32 imsi_flag;
  char imsi[QBI_SVC_BC_SPDP_EF_IMSI_MAX_DIGITS + 1];
} qbi_svc_bc_spdp_cache_s;

/* Keeps Track Of Presently loaded MBN */
typedef enum {
  QBI_SVC_BC_SPDP_OPERATOR_NONE = 0,
  QBI_SVC_BC_SPDP_OPERATOR_YM = 1,
  QBI_SVC_BC_SPDP_OPERATOR_SB = 2
} qbi_svc_bc_spdp_operator_flag_e;

/*! Tracking information for SPDP configuration */
typedef struct {
  /*! Single PDP configuration completion flag */
  boolean spdp_cfg_complete;
  boolean wait_for_ind;
  boolean deactivation_required;
} qbi_svc_bc_spdp_info_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_s_check_activation_state
===========================================================================*/
/*!
    @brief Check the current activation state ,if active triggers
           GET_PROFILE_SETTING_REQ msg

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_spdp_s_check_activation_state
(
  qbi_txn_s *txn
);

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_read_nv_store
===========================================================================*/
/*!
    @brief Reads single PDP support flag from NV

    @details

    @param ctx
    @param spdp_support_flag

    @return void
*/
/*=========================================================================*/
void qbi_svc_bc_spdp_read_nv_store
(
  qbi_ctx_s               *ctx,
  qbi_svc_bc_spdp_cache_s *cache
);

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_update_nv_store
===========================================================================*/
/*!
    @brief Updates single PDP support flag in NV

    @details

    @param ctx
    @param spdp_support_flag

    @return void
*/
/*=========================================================================*/
void qbi_svc_bc_spdp_update_nv_store
(
  qbi_ctx_s               *ctx,
  qbi_svc_bc_spdp_cache_s *cache
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_configure
===========================================================================*/
/*!
    @brief Configures device for single pdp

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_spdp_configure
(
  qbi_txn_s *txn
);

#endif /* QBI_SVC_BC_SPDP_H */

