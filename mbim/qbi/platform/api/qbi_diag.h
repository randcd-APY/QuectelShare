/*!
  @file
  qbi_diag.h

  @brief
  Internal definitions for QBI Diag interface
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
09/25/12  bo   Added QBI DIAG SIO module
=============================================================================*/

#ifndef QBI_DIAG_I_H
#define QBI_DIAG_I_H

/*=============================================================================

  Include Files

=============================================================================*/

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/
/*! Callback invoked to process a Diag message received from Diag task.
    Executed from the context of the QBI task. */
typedef void (qbi_diag_rx_from_diag_cb_f)
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
);

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_diag_dispatch_ul_message
===========================================================================*/
/*!
    @brief To be called for QMBE DIAG_DATA UL set processing for
           each DIAG msg

    @details

    @param data
    @param size

    @return boolean
*/
/*=========================================================================*/
boolean qbi_diag_dispatch_ul_message
(
  void *data,
  uint32 size
);

/*===========================================================================
  FUNCTION: qbi_diag_init
===========================================================================*/
/*!
    @brief Initialize QBI DIAG SIO device internals

    @details

    @param void
*/
/*=========================================================================*/
void qbi_diag_init
(
  void
);

/*===========================================================================
  FUNCTION: qbi_diag_reg_rx_data_cb
===========================================================================*/
/*!
    @brief Register Diag rx data callback function

    @details

    @param qbi_diag_rx_from_diag_cb_f
*/
/*=========================================================================*/
void qbi_diag_reg_rx_data_cb
(
  qbi_diag_rx_from_diag_cb_f *rx_data_cb
);

/*===========================================================================
  FUNCTION: qbi_diag_suspend
===========================================================================*/
/*!
    @brief Suspend Diag traffic when needed

    @details
*/
/*=========================================================================*/
void qbi_diag_suspend
(
  void
);

#endif /* QBI_DIAG_I_H */

