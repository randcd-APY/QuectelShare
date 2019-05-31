/*!
  @file
  qbi_svc_qmbe_mbim.h

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

#ifndef QBI_SVC_QMBE_MBIM_H
#define QBI_SVC_QMBE_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of QMBE device service CIDs
*/
typedef enum {
  QBI_SVC_QMBE_CID_MIN = 0,

  QBI_SVC_QMBE_MBIM_CID_DIAG_CONFIG = 1,
  QBI_SVC_QMBE_MBIM_CID_DIAG_DATA   = 2,

  QBI_SVC_QMBE_CID_MAX
} qbi_svc_qmbe_cid_e;

/*! Device service specific status code: Unknown USB PID */
#define QBI_SVC_QMBE_MBIM_STATUS_DIAG_UNKNOWN_PID \
  (QBI_MBIM_STATUS_EXT_DEV_SVC_FLAG | 1)

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_QMBE_DIAG_CONFIG
    @{ */

/* MBIM_DIAG_CONFIG */
#define QBI_SVC_QMBE_MBIM_DIAG_EXCLUDE 0
#define QBI_SVC_QMBE_MBIM_DIAG_PRESENT 1

/* MBIM_SET_DIAG_CONFIG */
typedef PACK(struct) {
  uint32 diag_config;
} qbi_svc_qmbe_diag_config_s_req_s;

/* MBIM_DIAG_CONFIG_INFO */
typedef PACK(struct) {
  uint32 diag_config;
} qbi_svc_qmbe_diag_config_rsp_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_QMBE_MBIM_H */

