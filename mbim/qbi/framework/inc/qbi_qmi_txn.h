/*!
  @file
  qbi_qmi_txn.h

  @brief
  Transaction management for QMI transactions associated with a QBI transaction
*/

/*=============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
05/08/12  bd   Determine QMI request structure size dynamically based on IDL
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_QMI_TXN_H
#define QBI_QMI_TXN_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_qmi.h"
#include "qbi_svc.h"
#include "qbi_txn.h"
#include "qbi_util.h"

#include "qmi_client.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*! @brief QMI transaction status
*/
typedef enum {
  /*! New transaction, not sent to QMI yet */
  QBI_QMI_TXN_STATUS_NOT_DISPATCHED = 0,

  /*! Request sent to QMI, waiting on response */
  QBI_QMI_TXN_STATUS_WAITING_RSP = 1,

  /*! Response received. Transactions with this status will only be kept after
      response processing if the keep_after_rsp_flag is set to TRUE */
  QBI_QMI_TXN_STATUS_RSP_RCVD = 2
} qbi_qmi_txn_status_e;

/*! @brief QMI transaction
*/
typedef struct qbi_qmi_txn_struct {
  /*! @brief Reference to associated QBI context
      @details Normally we could rely on parent->ctx, but the parent transaction
      may be released before the QMI transaction is, so we need a reference to
      the pend_qmi_txn list to remove the qbi_qmi_txn_s from when we get the
      QMI response. */
  qbi_ctx_s *ctx;

  /*! @brief Reference to parent QBI transaction
      @details This may be NULL if the parent transaction was freed before we
      received a response from QMI */
  qbi_txn_s *parent;

  /*! Current status of the QMI transaction */
  qbi_qmi_txn_status_e status;

  /*! QMI Service ID */
  qbi_qmi_svc_e svc_id;

  /*! QMI Message ID */
  uint16 msg_id;

  /*! QMI client transaction handle (ID) */
  qmi_txn_handle txn_handle;

  /*! QMI request buffer (C struct) */
  qbi_util_buf_s req;

  /*! QMI response buffer (C struct) */
  qbi_util_buf_s rsp;

  /*! Encoded callback function to handle the QMI response
      @see qbi_svc_qmi_rsp_cb_f */
  qbi_os_encoded_fptr_t cb;

  /*! @brief Flag indicating whether to keep the qbi_qmi_txn_s after cb returns
      @details Set to TRUE in the QMI response callback to prevent qbi_qmi from
      releasing the memory for the qbi_qmi_txn_s after the callback returns.
      In this case, memory will not be freed until the entire QBI transaction
      is freed */
  boolean keep_after_rsp_cb;
} qbi_qmi_txn_s;

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_qmi_txn_alloc
===========================================================================*/
/*!
    @brief Allocates a new QMI transaction

    @details

    @param txn QBI transaction this QMI transaction will be associated to
    @param qmi_svc_id
    @param qmi_msg_id
    @param rsp_cb Callback function to invoke to process the QMI response

    @return qbi_qmi_txn_s* Pointer to new transaction, or NULL on failure
*/
/*=========================================================================*/
qbi_qmi_txn_s *qbi_qmi_txn_alloc
(
  qbi_txn_s            *txn,
  qbi_qmi_svc_e         qmi_svc_id,
  uint16                qmi_msg_id,
  qbi_svc_qmi_rsp_cb_f *rsp_cb
);

/*===========================================================================
  FUNCTION: qbi_qmi_txn_alloc_ret_req_buf
===========================================================================*/
/*!
    @brief Helper/wrapper to allocate a QMI transaction and return the
    request data buffer

    @details
    Intended for use in CID handlers when only populating the request
    buffer is needed, and the overall qbi_qmi_txn_s can be ignored.

    @param txn
    @param qmi_svc_id
    @param qmi_msg_id
    @param rsp_cb

    @return void* Pointer to request data buffer, or NULL on failure

    @see qbi_qmi_txn_alloc
*/
/*=========================================================================*/
void *qbi_qmi_txn_alloc_ret_req_buf
(
  qbi_txn_s            *txn,
  qbi_qmi_svc_e         qmi_svc_id,
  uint16                qmi_msg_id,
  qbi_svc_qmi_rsp_cb_f *rsp_cb
);

/*===========================================================================
  FUNCTION: qbi_qmi_txn_free
===========================================================================*/
/*!
    @brief Release a QMI transaction

    @details
    In the case where a QMI transaction has been dispatched but the
    response has not been received yet, the QMI transaction is modified so
    that it's reference to the parent QBI transaction is set to NULL. This
    ensures that when we receive the response from QMI, the qbi_qmi_txn_s
    passed back as user_data is still valid memory, but since the parent
    transaction is gone, it will be dropped.

    @param qmi_txn
*/
/*=========================================================================*/
void qbi_qmi_txn_free
(
  qbi_qmi_txn_s *qmi_txn
);

/*===========================================================================
  FUNCTION: qbi_qmi_txn_free_all_from_ctx
===========================================================================*/
/*!
    @brief Frees all pending QMI transactions in the given QBI context

    @details
    The intent of this function is to free QMI transactions that have been
    orphaned from their parent QBI transactions when the QBI context is
    closing. This must only be called when there is no chance of receiving
    a QMI response from QCCI, for example when the QMI client IDs have
    been released.

    @param ctx
*/
/*=========================================================================*/
void qbi_qmi_txn_free_all_from_ctx
(
  qbi_ctx_s *ctx
);

/*===========================================================================
  FUNCTION: qbi_qmi_txn_free_all_from_txn
===========================================================================*/
/*!
    @brief Frees all QMI transactions associated with a QBI transaction

    @details
    In the case where a QMI transaction has been dispatched but the
    response has not been received yet, the QMI transaction is modified so
    that it's reference to the parent QBI transaction is set to NULL. This
    ensures that when we receive the response from QMI, the qbi_qmi_txn_s
    passed back as user_data is still valid memory, but since the parent
    transaction is gone, it will be dropped.

    @param txn
*/
/*=========================================================================*/
void qbi_qmi_txn_free_all_from_txn
(
  qbi_txn_s *txn
);

/*===========================================================================
  FUNCTION: qbi_qmi_txn_get
===========================================================================*/
/*!
    @brief Fetches a QMI transaction with matching QMI service & message ID
    from the QMI transaction list contained in a QBI transaction structure

    @details
    The intent of this function is to allow CID handlers to access QMI
    transactions after the QMI response callback. Note that QMI transactions
    are normally released after the response callback is complete - you must
    set the keep_after_rsp_cb flag in the QMI transaction to change this.

    @param txn
    @param qmi_svc_id
    @param qmi_msg_id

    @return qbi_qmi_txn_s* Pointer to matching QMI transaction, or NULL if
    not found or there was an error
*/
/*=========================================================================*/
qbi_qmi_txn_s *qbi_qmi_txn_get
(
  qbi_txn_s    *txn,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id
);

/*===========================================================================
  FUNCTION: qbi_qmi_txn_reset
===========================================================================*/
/*!
    @brief Resets a QMI transaction to its initial state after it was
    allocated (other than setting keep_after_rsp_cb to TRUE) to allow reuse
    for the same QMI request

    @details
    Allows multiple consecutive queries to the same QMI message but with
    different parameters without needing a new memory allocation for each.
    This can help prevent the heap from becoming fragmented.

    @param qmi_txn
*/
/*=========================================================================*/
void qbi_qmi_txn_reset
(
  qbi_qmi_txn_s *qmi_txn
);

/*===========================================================================
  FUNCTION: qbi_qmi_txn_sanity_check_rsp
===========================================================================*/
/*!
    @brief Check that a QMI transaction passed back with a response is valid

    @details
    Intended for use when receiving a qbi_qmi_txn_s as a user data pointer
    with an asynchronous callback containing the QMI response. Checks that
    the QMI transaction is valid, and that it can be dispatched. Note that
    this does not check whether the parent QBI transaction pointer is
    valid - that check should be handled separately from this function.

    @param qmi_txn
    @param msg_id QMI message ID associated with response
    @param resp_data Pointer to buffer containing QMI response
    @param resp_data_len Length of QMI response in bytes

    @return boolean TRUE if all checks passed, FALSE otherwise

    @note Executes outside of QBI task
*/
/*=========================================================================*/
boolean qbi_qmi_txn_sanity_check_rsp
(
  const qbi_qmi_txn_s *qmi_txn,
  uint32               msg_id,
  const void          *resp_data,
  uint32               resp_data_len
);

#endif /* QBI_QMI_TXN_H */

