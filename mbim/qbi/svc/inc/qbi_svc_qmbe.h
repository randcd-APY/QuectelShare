/*!
  @file
  qbi_svc_qmbe.h

  @brief
  Qualcomm Mobile Broadband MBIM Extensibility device service implementation.
  This device service provides Qualcomm specific functionalities in addition to
  MBIM specification.
*/

/*=============================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
10/08/12  hz   Add DIAG over MBIM support
07/23/12  hz   Added module
=============================================================================*/

#ifndef QBI_SVC_QMBE_H
#define QBI_SVC_QMBE_H

/*=============================================================================

  Include Files

=============================================================================*/
#include "qbi_common.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*=============================================================================

  Function Prototypes

=============================================================================*/
/*===========================================================================
  FUNCTION: qbi_svc_qmbe_diag_data_cb
===========================================================================*/
/*!
    @brief Callback function handling Diag DL messages

    @details

    @param ctx
    @param buf

    @return qbi_svc_action_e
*/
/*=========================================================================*/
void qbi_svc_qmbe_diag_data_cb
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
);

/*===========================================================================
  FUNCTION: qbi_svc_qmbe_init
===========================================================================*/
/*!
    @brief One-time initialization of the QMBE device service

    @details

*/
/*=========================================================================*/
void qbi_svc_qmbe_init
(
  void
);

#endif /* QBI_SVC_QMBE_H */

