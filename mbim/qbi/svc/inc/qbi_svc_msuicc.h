/*!
  @file
  qbi_svc_msuicc.h

  @brief
  Microsoft UICC device service implementation, based on the "MBIM EXTENSION
  FOR LOW-LEVEL UICC ACCESS INTERFACE SPECIFICATION" document from Microsoft.
  This device service provides low-level access to the UICC.
*/

/*=============================================================================

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
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
07/05/17  mm   Moved structure and function proto here
04/29/17  mm   Added macros
07/28/16  hz   Added module
=============================================================================*/

#ifndef QBI_SVC_MSUICC_H
#define QBI_SVC_MSUICC_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "user_identity_module_v01.h"
#include "qbi_util.h"
#include "qbi_common.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*! Logical channel ID and its associated tag assigned by host */
typedef struct {
  /*! Must be first as we alias */
  qbi_util_list_entry_s list_entry;

  uint32 channel_id;
  uint32 channel_group;
} qbi_svc_msuicc_logical_channel_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

boolean qbi_svc_msuicc_logical_channel_remove_from_cache
(
  qbi_ctx_s *ctx,
  uint32     channel_id
);

/*===========================================================================
  FUNCTION: qbi_svc_bc_msuicc_set_slot_mapping_in_progress_flag
===========================================================================*/
/*!
    @brief This function sets the slot mapping in progress flag

    @details If this flag is set to true then slot mapping currently
    is in progress. So that all msuicc operations should not take place.
    till this flag is set back to false.
    If this flag is set to false then slot mapping is completed or
    it is not in progress.

    @param flag

    @return none
*/
/*=========================================================================*/
void qbi_svc_bc_msuicc_set_slot_mapping_in_progress_flag
(
  const qbi_ctx_s *ctx,
  boolean flag
);

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_init
===========================================================================*/
/*!
    @brief One-time initialization of the MS Low_Level UICC Access device
    service

    @details

*/
/*=========================================================================*/
void qbi_svc_msuicc_init
(
  void
);

#endif /* QBI_SVC_MSUICC_H */

