/*!
  @file
  qbi_svc_mssar.h

  @brief
  Microsoft Selective Absorption Rate device service implementation, based on 
  the "Mobile Broadband Device Selective Absorption Rate (SAR) platfor support" 
  document from Microsoft. This device service allows host to query and set 
  Selective Absorption Rate on device.
*/

/*=============================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/platform/winmobile/EA/workspaces/WMData/MBIM/MBIM_Dev/Windows/QBI/qbi/svc/inc/qbi_svc_mssar.h#2 $

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
01/14/17  vk   Added module
=============================================================================*/

#ifndef QBI_SVC_MSSAR_H
#define QBI_SVC_MSSAR_H

/*=============================================================================

  Include Files

=============================================================================*/

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*! Maximum number of simultaneous data sessions supported (per QBI context) */
#define QBI_SVC_MSSAR_MAX_SESSIONS (8)

/*=============================================================================

  Typedefs

=============================================================================*/

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_mssar_init
===========================================================================*/
/*!
    @brief One-time initialization of the MSSAR device service

    @details

*/
/*=========================================================================*/
void qbi_svc_mssar_init
(
  void
);

#endif /* QBI_SVC_MSSAR_H */

