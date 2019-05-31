/*!
  @file
  qbi_qmi_txn.c

  @brief
  Transaction management for QMI transactions associated with a QBI transaction
*/

/*=============================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies, Inc. All Rights Reserved.
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
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_qmi_txn.h"

#include "qbi_common.h"
#include "qbi_svc.h"
#include "qbi_txn.h"
#include "qbi_util.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_qmi_txn_release_mem
===========================================================================*/
/*!
    @brief Frees all memory associated with a QMI transaction

    @details
    Does not remove the transaction from any lists. The memory for the
    qmi_txn itself is freed in this function, so it must not be referenced
    again after this function returns.

    @param qmi_txn
*/
/*=========================================================================*/
static void qbi_qmi_txn_release_mem
(
  qbi_qmi_txn_s *qmi_txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);

  qbi_util_buf_free(&qmi_txn->rsp);
  qbi_util_buf_free(&qmi_txn->req);
  QBI_MEMSET(qmi_txn, 0, sizeof(qbi_qmi_txn_s));
  QBI_MEM_FREE(qmi_txn);
} /* qbi_qmi_txn_release_mem() */

/*=============================================================================

  Public Function Definitions

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
)
{
  qbi_qmi_txn_s *qmi_txn;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(txn);
  /* Note that we currently don't allow QMI transactions with a NULL callback
     function. If this changes, qbi_qmi_txn_sanity_check_rsp() needs to be
     updated as well. */
  QBI_CHECK_NULL_PTR_RET_NULL(rsp_cb);

  qmi_txn = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_qmi_txn_s));
  if (qmi_txn != NULL)
  {
    qbi_util_buf_init(&qmi_txn->req);
    qbi_util_buf_init(&qmi_txn->rsp);

    /* Determine the size of the decoded IDL C structure for both the request
       and the response, then allocate buffers for each. Note that while a
       request may be empty (contain no TLVs), a response must always have at
       least the status code TLV. */
    if (qbi_qmi_get_msg_size(
          txn->ctx, qmi_svc_id, qmi_msg_id, QBI_QMI_MSG_TYPE_REQUEST,
          &qmi_txn->req.size) &&
        qbi_qmi_get_msg_size(
          txn->ctx, qmi_svc_id, qmi_msg_id, QBI_QMI_MSG_TYPE_RESPONSE,
          &qmi_txn->rsp.size) &&
        (qmi_txn->req.size == 0 ||
         qbi_util_buf_alloc(&qmi_txn->req, qmi_txn->req.size) != NULL) &&
        (qmi_txn->rsp.size > 0 &&
         qbi_util_buf_alloc(&qmi_txn->rsp, qmi_txn->rsp.size) != NULL))
    {
      /* Populate the struct and associate the QMI txn with the QBI txn */
      qmi_txn->parent = txn;
      qmi_txn->ctx    = txn->ctx;
      qmi_txn->svc_id = qmi_svc_id;
      qmi_txn->msg_id = qmi_msg_id;
      qbi_os_fptr_encode(&qmi_txn->cb, (qbi_os_void_f *) rsp_cb);

      if (qbi_util_list_push_back(&txn->qmi_txns, qmi_txn))
      {
        success = TRUE;
      }
    }
  }

  if (!success && qmi_txn != NULL)
  {
    qbi_qmi_txn_release_mem(qmi_txn);
    qmi_txn = NULL;
  }
  else if (success)
  {
    QBI_LOG_D_5("Allocated new QMI transaction for svc_id %d msg_id 0x%04x "
                "req size %d rsp size %d parent txn iid %d", qmi_svc_id,
                qmi_msg_id, qmi_txn->req.size, qmi_txn->rsp.size, txn->iid);
  }

  return qmi_txn;
} /* qbi_qmi_txn_alloc() */

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
)
{
  qbi_qmi_txn_s *qmi_txn;
  void *data = NULL;
/*-------------------------------------------------------------------------*/
  qmi_txn = qbi_qmi_txn_alloc(txn, qmi_svc_id, qmi_msg_id, rsp_cb);
  if (qmi_txn != NULL)
  {
    data = qmi_txn->req.data;
  }

  return data;
} /* qbi_qmi_txn_alloc_ret_req_buf() */

/*===========================================================================
  FUNCTION: qbi_qmi_txn_free
===========================================================================*/
/*!
    @brief Frees a QMI transaction

    @details
    Removes the transaction from both the parent QBI transaction's list
    (if parent transaction not already freed) and the context's pending
    QMI transactions list (if present on that list), then releases all
    memory allocated by the QMI transaction. Note that this is not
    appropriate for cases when the QMI request has been sent, but the
    response has not been received yet.

    @param qmi_txn
*/
/*=========================================================================*/
void qbi_qmi_txn_free
(
  qbi_qmi_txn_s *qmi_txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->ctx);

  (void) qbi_util_list_remove(&qmi_txn->ctx->pend_qmi_txns, qmi_txn);
  if (qmi_txn->parent != NULL)
  {
    qbi_util_list_remove(&qmi_txn->parent->qmi_txns, qmi_txn);
  }

  qbi_qmi_txn_release_mem(qmi_txn);
} /* qbi_qmi_txn_free() */

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
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  while ((qmi_txn = (qbi_qmi_txn_s *)
          qbi_util_list_pop_front(&ctx->pend_qmi_txns)) != NULL)
  {
    if (qmi_txn->parent != NULL)
    {
      /* At this point, the only QMI transactions we should encounter are
         orphans. Assuming the MBIM_CLOSE_MSG handler didn't reorder the
         function calls so that this gets called earlier (which would be wrong
         and need to be changed), then this signals a serious problem with no
         recovery option. */
      QBI_LOG_E_1("Found QMI txn on context with non-NULL parent txn iid %d",
                  qmi_txn->parent->iid);
    }
    else
    {
      qbi_qmi_txn_release_mem(qmi_txn);
    }
  }
} /* qbi_qmi_txn_free_all_from_ctx() */

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
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  while ((qmi_txn = (qbi_qmi_txn_s *)
            qbi_util_list_pop_back(&txn->qmi_txns)) != NULL)
  {
    if (qmi_txn->parent != txn && qmi_txn->parent != NULL)
    {
      /*! @note Under special circumstances, QMI transactions may be transferred
          from one parent transaction to another, so we must not free the QMI
          transaction when the old parent goes away.
          @see qbi_txn_transfer_qmi_txns */
      QBI_LOG_W_2("QMI txn was transferred from parent iid %d to parent iid %d",
                  txn->iid, qmi_txn->parent->iid);
    }
    else if (qmi_txn->status == QBI_QMI_TXN_STATUS_WAITING_RSP)
    {
      qmi_txn->parent = NULL;
      QBI_LOG_W_0("Parent transaction freed while QMI transaction in flight; "
                  "QMI transaction will be freed when response rcvd");
    }
    else
    {
      qbi_qmi_txn_release_mem(qmi_txn);
    }
  }
} /* qbi_qmi_txn_free_all_from_txn() */

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
)
{
  qbi_qmi_txn_s *qmi_txn;
  boolean found = FALSE;
  qbi_util_list_iter_s iter;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(txn);

  qbi_util_list_iter_init(&txn->qmi_txns, &iter);
  while ((qmi_txn = (qbi_qmi_txn_s *) qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (qmi_txn->svc_id == qmi_svc_id && qmi_txn->msg_id == qmi_msg_id)
    {
      found = TRUE;
      break;
    }
  }

  if (!found)
  {
    qmi_txn = NULL;
  }

  return qmi_txn;
} /* qbi_qmi_txn_get() */

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
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_txn);
  QBI_CHECK_NULL_PTR_RET(qmi_txn->rsp.data);

  qmi_txn->keep_after_rsp_cb = TRUE;
  qmi_txn->status = QBI_QMI_TXN_STATUS_NOT_DISPATCHED;
  QBI_MEMSET(&qmi_txn->txn_handle, 0, sizeof(qmi_txn->txn_handle));
  if (qmi_txn->req.size > 0 && qmi_txn->req.data != NULL)
  {
    QBI_MEMSET(qmi_txn->req.data, 0, qmi_txn->req.size);
  }
  QBI_MEMSET(qmi_txn->rsp.data, 0, qmi_txn->rsp.size);
} /* qbi_qmi_txn_reset() */

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
    @param rsp_data Pointer to buffer containing QMI response
    @param rsp_data_len Length of QMI response in bytes

    @return boolean TRUE if all checks passed, FALSE otherwise

    @note Executes outside of QBI task
*/
/*=========================================================================*/
boolean qbi_qmi_txn_sanity_check_rsp
(
  const qbi_qmi_txn_s *qmi_txn,
  uint32               msg_id,
  const void          *rsp_data,
  uint32               rsp_data_len
)
{
  boolean success = FALSE;
  qbi_os_void_f *fptr = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_txn);

  if (qmi_txn->ctx == NULL)
  {
    QBI_LOG_E_0("Context pointer is NULL");
  }
  else if (qmi_txn->ctx->qmi_state == NULL)
  {
    QBI_LOG_E_0("Context's QMI state is NULL");
  }
  else if (!qbi_os_fptr_decode(&qmi_txn->cb, &fptr, FALSE))
  {
    QBI_LOG_E_0("Missing callback function!");
  }
  else if (msg_id != qmi_txn->msg_id)
  {
    QBI_LOG_E_2("Wrong message ID for transaction! Got 0x%04x, expected 0x%04x",
                msg_id, qmi_txn->msg_id);
  }
  else if (rsp_data != qmi_txn->rsp.data)
  {
    QBI_LOG_E_0("Response data in unexpected buffer!");
  }
  else if (rsp_data_len > qmi_txn->rsp.size)
  {
    QBI_LOG_E_2("Response data too large! Got %d, expected <= %d",
                rsp_data_len, qmi_txn->rsp.size);
  }
  else if (qmi_txn->status != QBI_QMI_TXN_STATUS_WAITING_RSP)
  {
    QBI_LOG_E_1("Not expecting QMI response when status is %d", qmi_txn->status);
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_qmi_txn_sanity_check_rsp() */

