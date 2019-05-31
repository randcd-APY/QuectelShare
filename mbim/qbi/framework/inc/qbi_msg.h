/*!
  @file
  qbi_msg.h

  @brief
  Common processing of MBIM messages
*/

/*=============================================================================

  Copyright (c) 2011-2013, 2018 Qualcomm Technologies, Inc.
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
02/05/18  mm   Moved function proto here due to SSR compilation issue
07/25/13  bd   Simplify qbi_msg_send_open_rsp so the txn is always freed
01/30/12  cy   Add NTB max num/size config and Reset function
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_MSG_H
#define QBI_MSG_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_util.h"

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
  FUNCTION: qbi_msg_context_close
===========================================================================*/
/*!
    @brief Performs de-initialization ("closing") of a QBI context

    @details

    @param ctx

    @see qbi_msg_handle_close, qbi_msg_context_open
*/
/*=========================================================================*/
void qbi_msg_context_close
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_msg_ctx_init
===========================================================================*/
/*!
    @brief One-time initialization of a QBI context structure for the
    purposes of the MBIM message processing layer

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_msg_ctx_init
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_msg_input
===========================================================================*/
/*!
    @brief Dispatch incoming data to the associated MessageType handler

    @details
    The buffer may be freed before returning from this function, but it
    might not. Therefore, qbi_util_buf_free() must still always be invoked
    by the caller on the buffer passed in to this function after it returns.

    @param ctx Associated QBI context
    @param buf Pointer to buffer containing the message data
*/
/*=========================================================================*/
void qbi_msg_input
(
  qbi_ctx_s            *ctx,
  qbi_util_buf_const_s *buf
);

/*===========================================================================
  FUNCTION: qbi_msg_send_cmd_error_rsp
===========================================================================*/
/*!
    @brief Sends a command response with the given status

    @details
    Does not perform dynamic memory allocation and sets the
    InformationBufferLength field to 0. This function is meant to be used
    for situations where we are unable to process the request and we want to
    notify the host, but we may not have a valid qbi_txn_s, e.g. when
    memory allocation for the qbi_txn_s fails.

    @param ctx
    @param txn_id
    @param cid
    @param dev_svc_uuid
    @param status Status code to send with the response
*/
/*=========================================================================*/
void qbi_msg_send_cmd_error_rsp
(
  qbi_ctx_s   *ctx,
  uint32       txn_id,
  uint32       cid,
  const uint8 *dev_svc_uuid,
  uint32       status
);

/*===========================================================================
  FUNCTION: qbi_msg_send_cmd_rsp
===========================================================================*/
/*!
    @brief Sends the command response for a completed QBI transaction

    @details
    Based on the data contained in the transaction struct, builds and
    transmits a MBIM_COMMAND_DONE_MSG to the host. Upon returning from this
    function, the transaction will be deallocated and must no longer be
    used.

    @param txn
*/
/*=========================================================================*/
void qbi_msg_send_cmd_rsp
(
  struct qbi_txn_struct *txn
);

/*===========================================================================
  FUNCTION: qbi_msg_send_error
===========================================================================*/
/*!
    @brief Sends a MBIM_FUNCTION_ERROR_MSG with the given transaction
    ID and error code

    @details

    @param ctx
    @param error
*/
/*=========================================================================*/
void qbi_msg_send_error
(
  qbi_ctx_s       *ctx,
  uint32           txn_id,
  qbi_mbim_error_e error
);

/*===========================================================================
  FUNCTION: qbi_msg_send_event
===========================================================================*/
/*!
    @brief Sends the event to the host for a completed QBI indication

    @details
    Based on the data contained in the transaction struct, builds and
    transmits a MBIM_INDICATE_STATUS_MSG to the host. Upon returning from
    this function, the transaction will be deallocated and must no longer
    be used.

    @param txn
*/
/*=========================================================================*/
void qbi_msg_send_event
(
  struct qbi_txn_struct *txn
);

/*===========================================================================
  FUNCTION: qbi_msg_send_open_rsp
===========================================================================*/
/*!
    @brief Sends a MBIM_OPEN_DONE to the host and updates the context state

    @details
    The transaction will be freed after returning from this function.

    @param txn MBIM_OPEN transaction
*/
/*=========================================================================*/
void qbi_msg_send_open_rsp
(
  struct qbi_txn_struct *txn
);

/*===========================================================================
  FUNCTION: qbi_msg_handle_open
===========================================================================*/
/*!
    @brief Handles a MBIM_OPEN_MSG request

    @details

    @param ctx
    @param data
    @param data_len

    @return void
*/
/*=========================================================================*/
void qbi_msg_handle_open
(
  qbi_ctx_s  *ctx,
  const void *data,
  uint32      data_len
);

#endif /* QBI_MSG_H */

