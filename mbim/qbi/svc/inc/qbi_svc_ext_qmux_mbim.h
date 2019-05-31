/*!
  @file
  qbi_svc_ext_qmux_mbim.h

  @brief
  MBIM message definitions for QMUX over MBIM extensibility device service.
*/

/*=============================================================================

  Copyright (c) 2011 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
12/09/11  bd   Added EXT_QMUX device service
=============================================================================*/

#ifndef QBI_SVC_EXT_QMUX_MBIM_H
#define QBI_SVC_EXT_QMUX_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of USSD device service CIDs
*/
typedef enum {
  QBI_SVC_EXT_QMUX_MBIM_CID_MIN = 0,

  QBI_SVC_EXT_QMUX_MBIM_CID_QMUX_DATA = 1,

  QBI_SVC_EXT_QMUX_MBIM_CID_MAX
} qbi_svc_ext_qmux_mbim_cid_e;

/*! Device service specific status code: timeout while waiting for QMI
    response */
#define QBI_SVC_EXT_QMUX_MBIM_STATUS_TIMEOUT \
  (QBI_MBIM_STATUS_EXT_DEV_SVC_FLAG | 0)

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_QMUX_DATA
    @{ */

/*! @note The InformationBuffer in MBIM_CID_QMUX_DATA consists of pure QMUX
    data for all messages: set request/response and events. */

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_EXT_QMUX_MBIM_H */

