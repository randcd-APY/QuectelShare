/*!
  @file
  qbi_svc_ext_qmux.h

  @brief
  MBIM Extensibility Device Service providing tunneled access to QMI via
  encapsulating QMUX messages in an MBIM CID.
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

#ifndef QBI_SVC_EXT_QMUX_H
#define QBI_SVC_EXT_QMUX_H

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
  FUNCTION: qbi_svc_ext_qmux_init
===========================================================================*/
/*!
    @brief One-time initialization of the QMUX over MBIM extensibility
    device service

    @details
*/
/*=========================================================================*/
void qbi_svc_ext_qmux_init
(
  void
);

#endif /* QBI_SVC_EXT_QMUX_H */

