/*!
  @file
  qbi_svc_pb.h

  @brief
  Phonebook device service implementation.
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
02/25/13  bd   Update phonebook with full SIM hot swap support
01/31/12  bd   Added module
=============================================================================*/

#ifndef QBI_SVC_PB_H
#define QBI_SVC_PB_H

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
  FUNCTION: qbi_svc_pb_init
===========================================================================*/
/*!
    @brief One-time initialization of the Phonebook device service

    @details

*/
/*=========================================================================*/
void qbi_svc_pb_init
(
  void
);

/*===========================================================================
  FUNCTION: qbi_svc_pb_sim_removed
===========================================================================*/
/*!
    @brief Notifies the phonebook device service that the SIM is removed,
    allowing it to update the host as required

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_pb_sim_removed
(
  qbi_ctx_s *ctx
);

#endif /* QBI_SVC_PB_H */

