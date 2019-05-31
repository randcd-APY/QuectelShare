/*!
  @file
  qbi_qmux_smd.h

  @brief
  Interface to QMUX SMD transport layer.
*/

/*=============================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
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
09/10/15  hz   Added support for qmux over smd
=============================================================================*/

#ifndef QBI_QMUX_SMD_H
#define QBI_QMUX_SMD_H

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

/*! Callback invoked to process a QMUX message received from the modem.
    Executed from the context of the QMUX-SMD RX thread. */
typedef void (qbi_qmux_smd_msg_cb_f)
(
  unsigned char *rx_msg,
  int            rx_msg_len
);

/*=============================================================================

  Function Prototypes

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_qmux_smd_close
===========================================================================*/
/*!
    @brief Cancel receiver thread and close QMUX SMD port.

    @details
    Cancel rx thread and close SMD port

    @param

    @return void
*/
/*=========================================================================*/
void qbi_qmux_smd_close
(
  void
);

/*===========================================================================
  FUNCTION: qbi_qmux_smd_init
===========================================================================*/
/*!
    @brief Opens QMUX SMD port and starts receiver thread for QMUX msg IO.

    @details
    Open QMUX SMD device file.
    Create rx thread => qbi_qmux_smd_rx_thread().
    Send sync msg - no reply expected.

    @param rx_cb_f  Callback function executed for each received QMUX msg

    @return boolean
*/
/*=========================================================================*/
boolean qbi_qmux_smd_init
(
  qbi_qmux_smd_msg_cb_f *rx_cb_f
);

/*===========================================================================
  FUNCTION: qbi_qmux_smd_write_msg
===========================================================================*/
/*!
    @brief Write QMUX message to SMD port.

    @details

    @param write_msg
    @param write_msg_len

    @return boolean
*/
/*=========================================================================*/
boolean qbi_qmux_smd_write_msg
(
  unsigned char *write_msg,
  uint32         write_msg_len
);

#endif /* QBI_QMUX_SMD_H */

