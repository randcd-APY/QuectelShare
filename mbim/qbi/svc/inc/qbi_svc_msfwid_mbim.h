/*!
  @file
  qbi_svc_msfwid_mbim.h

  @brief
  Microsoft Firmware ID device service implementation, based on the "Mobile
  Broadband Device Firmware Update on Windows 8" document from Microsoft.
  This device service allows host to query Firmware ID from device.
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
08/17/12  hz   Added module
=============================================================================*/

#ifndef QBI_SVC_MSFWID_MBIM_H
#define QBI_SVC_MSFWID_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of MSFWID device service CIDs
*/
typedef enum {
  QBI_SVC_MSFWID_CID_MIN = 0,

  QBI_SVC_MSFWID_MBIM_CID_FIRMWAREID = 1,

  QBI_SVC_MSFWID_CID_MAX
} qbi_svc_msfwid_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_MSFIRMWAREID
    @{ */
typedef PACK(struct){
  uint8 firmware_id[QBI_MBIM_UUID_LEN];
} qbi_svc_msfwid_firmwareid_rsp_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_MSFWID_MBIM_H */

