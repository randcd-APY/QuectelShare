/*!
  @file
  qbi_svc_atds.h

  @brief
  AT&T device service implementation, based on the "AT&T Windows 8
  Extended API Requirements - Joint Requirement Doc (JRD)" from AT&T.
  This device service allows host to set and query device/radio paremeters
  required for AT&T's acceptance.
*/

/*=============================================================================

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
09/27/13  bd   Use QMI_NAS_GET_PLMN_NAME for VISIBLE_PROVIDERS/ATDS_OPERATORS
06/27/13  bd   Rework signal reporting
04/16/13  bd   Add QMI indication (de)registration support to ATDS
09/01/12  cy   Initial release based on JRD v 1.6.1
=============================================================================*/

#ifndef QBI_SVC_ATDS_H
#define QBI_SVC_ATDS_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*! @addtogroup MBIM_CID_ATDS_OPERATORS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_atds_operators_q_nas44_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_GET_PLMN_NAME_RESP for MBIM_CID_ATDS_OPERATORS
    query response

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_atds_operators_q_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*===========================================================================
  FUNCTION: qbi_svc_atds_init
===========================================================================*/
/*!
    @brief One-time initialization of the AT&T device service

    @details

*/
/*=========================================================================*/
void qbi_svc_atds_init
(
  void
);

/*===========================================================================
  FUNCTION: qbi_svc_atds_reset
===========================================================================*/
/*!
    @brief If active, resets and deactivates ATDS signal projection

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_atds_reset
(
  qbi_ctx_s *ctx
);

#endif /* QBI_SVC_ATDS_H */

