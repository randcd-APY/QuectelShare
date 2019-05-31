/*!
  @file
  qbi_hc_linux.h

  @brief
  QBI Host Communications layer for the Linux platform.
*/

/*=============================================================================

  Copyright (c) 2012, 2018 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
03/19/18  ha   Add support for mhi uevent and deprecate pcie_enabled NV
02/07/12  bd   Added module
=============================================================================*/

#ifndef QBI_HC_LINUX_H
#define QBI_HC_LINUX_H

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
  FUNCTION: qbi_hc_linux_dev_init
===========================================================================*/
/*!
    @brief Opens the device special file, allowing IO through the MBIM USB
    driver

    @details
    This must be called after qbi_hc_init() and before the reader thread or
    main QBI task loop are started.

    @return boolean TRUE on success, FALSE on fatal error
*/
/*=========================================================================*/
boolean qbi_hc_linux_dev_init
(
  void
);

/*===========================================================================
  FUNCTION: qbi_hc_linux_reader_thread_entry
===========================================================================*/
/*!
    @brief Entry point for the reader thread used to handle commands received
    from the host by the USB driver

    @details

    @param arg

    @return void*
*/
/*=========================================================================*/
void *qbi_hc_linux_reader_thread_entry
(
  void *arg
);

/*===========================================================================
  FUNCTION: qbi_hc_linux_start_reader_thread
===========================================================================*/
/*!
    @brief starts reader thread. Reader thread is used to handle commands
    received from the host by the USB driver

    @details

    @param

    @return int
*/
/*=========================================================================*/
int qbi_hc_linux_start_reader_thread
(
  void
);

/*===========================================================================
  FUNCTION: qbi_hc_linux_crash_recovery_check_session_file
===========================================================================*/
/*!
    @brief Checks if QBI is recovering from a crash, and if so, takes
    appropriate action

    @details
*/
/*=========================================================================*/
void qbi_hc_linux_crash_recovery_check_session_file
(
  void
);
#endif /* QBI_HC_LINUX_H */

