/*!
  @file
  qbi_svc_mshsd_mbim.h

  @brief
  Microsoft Host Shutdown device service implementation, based on the "Device
  Service: Host shutdown notification" document from Microsoft. This device
  service provides a notification to the device that the system is about to
  power down, allowing the modem to deregister from the network.
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
04/27/12  bd   Added module
=============================================================================*/

#ifndef QBI_SVC_MSHSD_MBIM_H
#define QBI_SVC_MSHSD_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of MSHOSTSHUTDOWN device service CIDs
*/
typedef enum {
  QBI_SVC_MSHSD_CID_MIN = 0,

  QBI_SVC_MSHSD_MBIM_CID_MSHOSTSHUTDOWN = 1,

  QBI_SVC_MSHSD_CID_MAX
} qbi_svc_mshsd_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/* No CID-specific definitions */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_MSHSD_MBIM_H */

