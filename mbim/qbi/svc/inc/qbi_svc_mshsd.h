/*!
  @file
  qbi_svc_mshsd.h

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

#ifndef QBI_SVC_MSHSD_H
#define QBI_SVC_MSHSD_H

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
  FUNCTION: qbi_svc_mshsd_init
===========================================================================*/
/*!
    @brief One-time initialization of the MSHOSTSHUTDOWN device service

    @details

*/
/*=========================================================================*/
void qbi_svc_mshsd_init
(
  void
);

#endif /* QBI_SVC_MSHSD_H */

