/*!
  @file
  qbi_svc_bc.h

  @brief
  Basic Connectivity device service implementation
*/

/*=============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
03/12/18  mm...Moved protos here due to compialtion issue related to SSR
01/13/12  bd   Expose AvailableDataClass for SMS transcoding
10/28/11  bd   Updated to MBIM v1.0 SC
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_SVC_BC_H
#define QBI_SVC_BC_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*! Maximum number of simultaneous data sessions supported (per QBI context) */
#define QBI_SVC_BC_MAX_SESSIONS (8)

/*=============================================================================

  Typedefs

=============================================================================*/

/*=============================================================================

  Function Prototypes

=============================================================================*/

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
  struct qbi_txn_struct *txn,
  boolean                action_requires_ready_state_init,
  boolean                action_requires_radio
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_device_is_in_ftm
===========================================================================*/
/*!
    @brief Checks whether the modem is in Factory Test Mode (FTM)

    @details

    @param ctx

    @return boolean TRUE if device is in FTM, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_bc_device_is_in_ftm
(
  const qbi_ctx_s *ctx
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

#endif /* QBI_SVC_BC_H */

