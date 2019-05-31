/*!
  @file
  qbi_svc_msfwid.h

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

#ifndef QBI_SVC_MSFWID_H
#define QBI_SVC_MSFWID_H

/*=============================================================================

  Include Files

=============================================================================*/

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
  FUNCTION: qbi_svc_msfwid_init
===========================================================================*/
/*!
    @brief One-time initialization of the MSFIRMWAREID device service

    @details

*/
/*=========================================================================*/
void qbi_svc_msfwid_init
(
  void
);

#endif /* QBI_SVC_MSFWID_H */

