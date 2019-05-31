/*!
  @file
  qbi_txn.c

  @brief
  QBI transaction management
*/

/*=============================================================================

  Copyright (c) 2011-2013, 2017 Qualcomm Technologies, Inc.
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
08/19/17  vk   Add support to extract variable lenghth from a transaction 
               response buffer
03/14/13  bd   Fix next timeout calculation when timeout_cb is invoked
03/12/13  bd   Add transaction notify framework
01/22/13  bd   Support new unit test framework
11/08/12  bd   Improvements to fragmented request handling
10/08/12  hz   Add DIAG over MBIM support
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_txn.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_msg_mbim.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_util.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static void qbi_txn_default_timeout_cb
(
  qbi_txn_s *txn
);

static void qbi_txn_notify_proc
(
  const qbi_txn_s *txn
);

static void qbi_txn_release_mem
(
  qbi_txn_s *txn
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_txn_default_timeout_cb
===========================================================================*/
/*!
    @brief Default transaction timeout callback that aborts the transaction

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_txn_default_timeout_cb
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  /* Set the status to FAILURE so the completion callback (if any) can know
     that the transaction did not complete successfully. */
  txn->status = QBI_MBIM_STATUS_FAILURE;
  qbi_svc_abort_txn(txn, txn->status);
} /* qbi_txn_default_timeout_cb() */

/*===========================================================================
  FUNCTION: qbi_txn_notify_proc
===========================================================================*/
/*!
    @brief Processes notify callback related actions for a transaction that
    is about to be freed

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_txn_notify_proc
(
  const qbi_txn_s *txn
)
{
  qbi_util_list_iter_s iter;
  qbi_txn_s *pend_txn;
  qbi_txn_notify_cb_f *notify_cb = NULL;
  qbi_os_void_f *fptr = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->ctx);

  if (txn->listener_txn != NULL)
  {
    if (!qbi_os_fptr_decode(&txn->listener_txn->notify_cb,
                            (qbi_os_void_f **) &notify_cb, FALSE) ||
        txn->listener_txn->notifier_count == 0)
    {
      QBI_LOG_E_1("Listener iid %d failed sanity check",
                  txn->listener_txn->iid);
    }
    else
    {
      QBI_LOG_D_2("Invoking notify_cb for listener iid %d from notifier iid %d",
                  txn->listener_txn->iid, txn->iid);
      txn->listener_txn->notifier_count--;
      notify_cb(txn->listener_txn, txn);
      /* The listener txn must not be referenced after returning from notify_cb,
         since it may have been freed. */
    }
  }
  else if (qbi_os_fptr_decode(&txn->notify_cb, &fptr, FALSE) &&
           txn->notifier_count > 0)
  {
    /* We are freeing a notify parent (listener) with pending children
       (notifiers); ensure that references to the parent are removed */
    qbi_util_list_iter_init(&txn->ctx->qbi_txns, &iter);
    while ((pend_txn = qbi_util_list_iter_next(&iter)) != NULL)
    {
      if (pend_txn->listener_txn == txn)
      {
        QBI_LOG_D_2("Listener iid %d is being freed; unlinking notifier iid %d",
                    txn->iid, pend_txn->iid);
        pend_txn->listener_txn = NULL;
      }
    }
  }
} /* qbi_txn_notify_proc() */

/*===========================================================================
  FUNCTION: qbi_txn_release_mem
===========================================================================*/
/*!
    @brief Release all memory associated with a transaction

    @details
    Includes releasing the request & response buffers, all associated
    QMI transactions, and the transaction's memory itself. Therefore,
    the transaction must not be referenced after calling this function.

    @param txn
*/
/*=========================================================================*/
static void qbi_txn_release_mem
(
  qbi_txn_s *txn
)
{
  qbi_txn_completion_cb_f *txn_completion_cb = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  QBI_LOG_I_2("Freeing transaction iid %d (txn_id 0x%x)", txn->iid, txn->txn_id);
  /* Process notify callback before completion callback, to ensure all of the
     transaction's resources are available */
  qbi_txn_notify_proc(txn);

  /* Invoke the completion callback if present, so any additional cleanup for
     this transaction can be done before it is released. */
  if (qbi_os_fptr_decode(&txn->completion_cb,
                         (qbi_os_void_f **) &txn_completion_cb, FALSE))
  {
    txn_completion_cb(txn);
  }

  /* Try to free all memory in the reverse order that it was allocated */
  qbi_util_buf_chain_free_all(&txn->rsp_databuf);
  qbi_util_buf_free(&txn->rsp);
  qbi_svc_ind_dereg_txn(txn);
  if (txn->info != NULL)
  {
    QBI_MEM_FREE(txn->info);
  }
  qbi_qmi_txn_free_all_from_txn(txn);
  qbi_util_buf_free(&txn->req);
  QBI_MEMSET(txn, 0, sizeof(qbi_txn_s));
  QBI_MEM_FREE(txn);
} /* qbi_txn_release_mem() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_txn_alloc
===========================================================================*/
/*!
    @brief Allocates a new QBI transaction

    @details

    @param ctx Context to associate the transaction with
    @param svc_id Service ID enum
    @param txn_id Transaction ID
    @param cmd_type Command Type
    @param cid Command ID
    @param req_data_len Size of req_data in bytes
    @param req_data InformationBuffer included with the request

    @return qbi_txn_s* Pointer to new transaction, or NULL on failure
*/
/*=========================================================================*/
qbi_txn_s *qbi_txn_alloc
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       txn_id,
  uint32       cmd_type,
  uint32       cid,
  uint32       req_data_len,
  const void  *req_data
)
{
/*-------------------------------------------------------------------------*/
  return qbi_txn_alloc_fragmented(
    ctx, svc_id, txn_id, cmd_type, cid, req_data_len, req_data, req_data_len);
} /* qbi_txn_alloc() */

/*===========================================================================
  FUNCTION: qbi_txn_alloc_event
===========================================================================*/
/*!
    @brief Allocates a new QBI transaction representing a CID event.

    @details

    @param ctx
    @param svc_id
    @param cid

    @return qbi_txn_s*
*/
/*=========================================================================*/
qbi_txn_s *qbi_txn_alloc_event
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       cid
)
{
/*-------------------------------------------------------------------------*/
  return qbi_txn_alloc(ctx, svc_id, QBI_MSG_TXN_ID_IND, QBI_TXN_CMD_TYPE_EVENT,
                       cid, 0, NULL);
} /* qbi_txn_alloc_event() */

/*===========================================================================
  FUNCTION: qbi_txn_alloc_fragmented
===========================================================================*/
/*!
    @brief Allocates a new QBI transaction whose request buffer may be
    populated over multiple fragment transfers

    @details

    @param ctx Context to associate the transaction with
    @param svc_id Service ID enum
    @param txn_id Transaction ID
    @param cmd_type Command Type
    @param cid Command ID
    @param req_data_len Size of InformationBuffer in first fragment
    @param req_data InformationBuffer in first fragment
    @param req_infobuf_size Size of the entire request InformationBuffer
    including data from all fragments

    @return qbi_txn_s* Pointer to new transaction, or NULL on failure
*/
/*=========================================================================*/
qbi_txn_s *qbi_txn_alloc_fragmented
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       txn_id,
  uint32       cmd_type,
  uint32       cid,
  uint32       req_data_len,
  const void  *req_data,
  uint32       req_infobuf_size
)
{
  qbi_txn_s *txn = NULL;
  boolean success = FALSE;
  static uint32 next_iid = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(ctx);

  if (req_infobuf_size < req_data_len)
  {
    QBI_LOG_E_2("Bad request info buf size: %d with initial data size %d",
                req_infobuf_size, req_data_len);
  }
  else
  {
    txn = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_txn_s));
    if (txn != NULL)
    {
      qbi_util_buf_init(&txn->req);
      qbi_util_buf_init(&txn->rsp);
      qbi_util_buf_chain_init(&txn->rsp_databuf);

      if (req_infobuf_size == 0 ||
          qbi_util_buf_alloc_dont_clear(&txn->req, req_infobuf_size) != NULL)
      {
        /* Populate the transaction */
        txn->ctx = ctx;
        txn->iid = next_iid++;
        txn->svc_id = svc_id;
        txn->txn_id = txn_id;
        txn->cmd_type = cmd_type;
        txn->cid = cid;
        if (req_data_len > 0 && req_data != NULL)
        {
          QBI_MEMSCPY(txn->req.data, txn->req.size, req_data, req_data_len);
        }
        qbi_txn_set_timeout(txn, QBI_TXN_TIMEOUT_DEFAULT_MS, NULL);

        if (qbi_util_list_push_back(&ctx->qbi_txns, txn))
        {
          success = TRUE;
        }
      }
    }
  }

  if (!success && txn != NULL)
  {
    qbi_txn_release_mem(txn);
    txn = NULL;
  }
  else if (success)
  {
    QBI_LOG_D_5("Allocated new QBI transaction with iid %d for txn_id 0x%x "
                "svc_id %d cid %d cmd_type 0x%x", txn->iid, txn_id, txn->svc_id,
                txn->cid, txn->cmd_type);
  }

  return txn;
} /* qbi_txn_alloc() */

/*===========================================================================
  FUNCTION: qbi_txn_alloc_rsp_buf
===========================================================================*/
/*!
    @brief Allocate memory to hold the fixed length portion of the response
    InformationBuffer

    @details
    The buffer will be initialized to 0.

    @param txn
    @param size

    @return void* Pointer to new memory region, or NULL on failure
*/
/*=========================================================================*/
void *qbi_txn_alloc_rsp_buf
(
  qbi_txn_s *txn,
  uint32     size
)
{
  void *buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(txn);

  if (txn->rsp.size != 0)
  {
    QBI_LOG_W_3("Re-allocating response buffer for txn iid %d: old size %d "
                "new %d", txn->iid, txn->rsp.size, size);
    txn->infobuf_len_total -= txn->rsp.size;
    qbi_util_buf_free(&txn->rsp);
  }

  buf = qbi_util_buf_alloc(&txn->rsp, size);
  if (buf != NULL)
  {
    txn->infobuf_len_total += size;
  }

  return buf;
} /* qbi_txn_alloc_rsp_buf() */

/*===========================================================================
  FUNCTION: qbi_txn_check_timeout
===========================================================================*/
/*!
    @brief Loops through the context's list of transactions, checking to see
    if any have timed out, and handling those that have accordingly

    @details

    @param ctx

    @return qbi_os_time_ms_t Delay in milliseconds until the next
    transaction will time out, or 0 if no transactions will time out
*/
/*=========================================================================*/
qbi_os_time_ms_t qbi_txn_check_timeout
(
  qbi_ctx_s *ctx
)
{
  qbi_os_time_ms_t cur_time;
  qbi_os_time_ms_t delta;
  qbi_os_time_ms_t next_timeout_delay_ms = 0;
  qbi_txn_s *txn;
  qbi_util_list_iter_s iter;
  boolean timeout_cb_called = FALSE;
  qbi_txn_timeout_cb_f *timeout_cb = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx);

  cur_time = qbi_os_time_get_ms();
  qbi_util_list_iter_init(&ctx->qbi_txns, &iter);
  qbi_util_list_iter_reg(&iter);
  while ((txn = (qbi_txn_s *) qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (txn->timeout != QBI_TXN_TIMEOUT_DISABLED)
    {
      delta = qbi_os_time_diff(txn->timeout, cur_time);
      if (delta <= 0)
      {
        QBI_LOG_W_1("Transaction with iid %d has timed out!", txn->iid);
        if (!qbi_os_fptr_decode(&txn->timeout_cb,
                                (qbi_os_void_f **) &timeout_cb, FALSE))
        {
          QBI_LOG_E_0("Timeout callback is invalid!");
        }
        else if (txn->timeout_invoked)
        {
          /* If this happens, there is a bug in the timeout callback. If it
             does not release the transaction, it must call
             qbi_txn_set_timeout to reset the timeout. */
          QBI_LOG_E_0("Timeout has already been invoked for this "
                      "transaction!!");
        }
        else
        {
          txn->timeout_invoked = TRUE;
          timeout_cb(txn);
          timeout_cb_called = TRUE;
        }
      }
      else if (next_timeout_delay_ms == 0 || delta < next_timeout_delay_ms)
      {
        next_timeout_delay_ms = delta;
      }
    }
  }
  qbi_util_list_iter_dereg(&iter);

  /* Recalculate the time until next timeout if we invoked a timeout callback
     above. The timeout callback may have altered the transaction list and/or
     timeout values, which invalidates the earlier next_timeout_delay_ms
     calculation. */
  if (timeout_cb_called)
  {
    next_timeout_delay_ms = 0;
    qbi_util_list_iter_init(&ctx->qbi_txns, &iter);
    while ((txn = (qbi_txn_s *) qbi_util_list_iter_next(&iter)) != NULL)
    {
      if (txn->timeout != QBI_TXN_TIMEOUT_DISABLED && !txn->timeout_invoked)
      {
        delta = qbi_os_time_diff(txn->timeout, cur_time);
        /* If the delta is < 1, the transaction is considered timed out - use a
           1ms delay to ensure we process it with minimal delay. */
        if (delta < 1)
        {
          delta = 1;
        }

        if (next_timeout_delay_ms == 0 || delta < next_timeout_delay_ms)
        {
          next_timeout_delay_ms = delta;
        }
      }
    }
  }

  if (next_timeout_delay_ms != 0)
  {
    QBI_LOG_D_2("Next transaction times out in %d ms (ctx id %d)",
                next_timeout_delay_ms, ctx->id);
  }

  return next_timeout_delay_ms;
} /* qbi_txn_check_timeout() */

/*===========================================================================
  FUNCTION: qbi_txn_find_by_id
===========================================================================*/
/*!
    @brief Looks for a pending transaction on the given context with the
    given transaction ID

    @details

    @param ctx
    @param txn_id

    @return qbi_txn_s* Pointer to transaction with matching transaction ID,
    or NULL if not found or an error occurred
*/
/*=========================================================================*/
qbi_txn_s *qbi_txn_find_by_id
(
  qbi_ctx_s *ctx,
  uint32     txn_id
)
{
  qbi_util_list_iter_s iter;
  qbi_txn_s *txn = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(ctx);

  qbi_util_list_iter_init(&ctx->qbi_txns, &iter);
  while ((txn = (qbi_txn_s *) qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (txn->txn_id == txn_id)
    {
      break;
    }
  }

  return txn;
} /* qbi_txn_find_by_id() */

/*===========================================================================
  FUNCTION: qbi_txn_free
===========================================================================*/
/*!
    @brief Frees a QBI transaction

    @details
    Completely releases the QBI transaction, including removing it from
    the context's transaction list, removal of all QMI transactions,
    deallocation of both request & response InformationBuffers, etc.

    @param txn
*/
/*=========================================================================*/
void qbi_txn_free
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->ctx);

  if (!qbi_util_list_remove(&txn->ctx->qbi_txns, txn))
  {
    QBI_LOG_E_0("Error removing QBI transaction from context list!");
  }
  qbi_txn_release_mem(txn);
} /* qbi_txn_free() */

/*===========================================================================
  FUNCTION: qbi_txn_free_all
===========================================================================*/
/*!
    @brief Free all pending QBI transactions for the given context

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_txn_free_all
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  while ((txn = (qbi_txn_s *) qbi_util_list_pop_front(&ctx->qbi_txns)) != NULL)
  {
    qbi_txn_release_mem(txn);
  }
} /* qbi_txn_free_all() */

/*===========================================================================
  FUNCTION: qbi_txn_free_req_buf
===========================================================================*/
/*!
    @brief Releases memory allocated for a transaction's request

    @details

    @param txn
*/
/*=========================================================================*/
void qbi_txn_free_req_buf
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  qbi_util_buf_free(&txn->req);
} /* qbi_txn_free_req_buf() */

/*===========================================================================
  FUNCTION: qbi_txn_get_pending_txn_list_iter
===========================================================================*/
/*!
    @brief Initializes a list iterator to allow iteration over the pending
    QBI CID transactions for a given context

    @details

    @param ctx
    @param iter

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_txn_get_pending_txn_list_iter
(
  qbi_ctx_s            *ctx,
  qbi_util_list_iter_s *iter
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(iter);

  qbi_util_list_iter_init(&ctx->qbi_txns, iter);
  return TRUE;
} /* qbi_txn_get_pending_txn_list_iter() */

/*===========================================================================
  FUNCTION: qbi_txn_notify_setup_listener
===========================================================================*/
/*!
    @brief Configures a transaction to become a listener, so that a
    notification callback will be invoked when other notifier transactions
    linked to it complete

    @details
    The transaction notification framework allows for a 'listener'
    transaction to receive a callback when a 'notifier' transaction
    completes. A listener may have multiple notifiers, but a listener
    cannot be a notifier (and vice versa), and a notifier can only have one
    listener. Once setup, a listener may not be changed to a notifier, and
    a notifier cannot change its associated listener. The listener must be
    setup before notifiers can be linked to it.

    Please use discretion when deciding whether to use this API, as typically
    an operation can be completed without the need for allocating 'notifier'
    transactions.

    @param listener_txn Transaction to setup as listener
    @param notify_cb Callback function to invoke when a linked notifier
    transaction completes

    @return boolean TRUE on success, FALSE on failure

    @see qbi_txn_notify_setup_listener
*/
/*=========================================================================*/
boolean qbi_txn_notify_setup_listener
(
  qbi_txn_s           *listener_txn,
  qbi_txn_notify_cb_f *notify_cb
)
{
  boolean success = FALSE;
  qbi_os_void_f *fptr = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(listener_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(notify_cb);

  if (listener_txn->listener_txn != NULL)
  {
    QBI_LOG_E_2("Tried to setup txn iid %d as listener but it is already a "
                "notifier to txn iid %d", listener_txn->iid,
                listener_txn->listener_txn->iid);
  }
  else
  {
    if (listener_txn->notifier_count > 0 ||
        qbi_os_fptr_decode(&listener_txn->notify_cb, &fptr, FALSE))
    {
      QBI_LOG_W_2("Reconfiguring notify callback for listener iid %d with %d "
                  "notifiers", listener_txn->iid, listener_txn->notifier_count);
    }
    qbi_os_fptr_encode(&listener_txn->notify_cb, (qbi_os_void_f *) notify_cb);
    success = TRUE;

    QBI_LOG_D_1("Setup txn iid %d as listener", listener_txn->iid);
  }

  return success;
} /* qbi_txn_notify_setup_listener() */

/*===========================================================================
  FUNCTION: qbi_txn_notify_setup_notifier
===========================================================================*/
/*!
    @brief Configures a transaction to become a notifier to a previously
    setup listener transaction. When the notifier completes, it invokes the
    listener's callback function.

    @details
    See qbi_txn_notify_setup_listener() for details.

    @param listener_txn Already configured listener transaction
    @param notifier_txn Transaction to setup as a notifier to the given
    listener

    @return boolean TRUE on success, FALSE on failure

    @see qbi_txn_notify_setup_listener
*/
/*=========================================================================*/
boolean qbi_txn_notify_setup_notifier
(
  qbi_txn_s *listener_txn,
  qbi_txn_s *notifier_txn
)
{
  boolean success = FALSE;
  qbi_os_void_f *fptr = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(listener_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(notifier_txn);

  if (!qbi_os_fptr_decode(&listener_txn->notify_cb, &fptr, FALSE))
  {
    QBI_LOG_E_2("Tried to setup txn iid %d as notifier to txn iid %d but "
                "listener not setup yet!", notifier_txn->iid, listener_txn->iid);
  }
  else if (qbi_os_fptr_decode(&notifier_txn->notify_cb, &fptr, FALSE))
  {
    QBI_LOG_E_1("Tried to setup txn iid %d as notifier but it is already a "
                "listener!", notifier_txn->iid);
  }
  else if (notifier_txn->listener_txn != NULL)
  {
    QBI_LOG_E_3("Tried to move notifier txn iid %d from listener iid %d to "
                "iid %d; not supported", notifier_txn->iid,
                notifier_txn->listener_txn->iid, listener_txn->iid);
  }
  else
  {
    notifier_txn->listener_txn = listener_txn;
    listener_txn->notifier_count++;
    success = TRUE;

    QBI_LOG_D_3("Setup txn iid %d as notifier to iid %d (notifier count now "
                "%d)", notifier_txn->iid, listener_txn->iid,
                listener_txn->notifier_count);
  }

  return success;
} /* qbi_txn_notify_setup_notifier() */

/*===========================================================================
  FUNCTION: qbi_txn_req_buf_ignored
===========================================================================*/
/*!
    @brief Processing for when a request InformationBuffer is not expected

    @details
    If the buffer is present, then memory for it is released.

    @param txn
*/
/*=========================================================================*/
void qbi_txn_req_buf_ignored
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  if (txn->req.size > 0)
  {
    QBI_LOG_I_1("Ignoring request InformationBuffer with length %d",
                txn->req.size);
    qbi_txn_free_req_buf(txn);
  }
} /* qbi_txn_req_buf_ignored() */

/*===========================================================================
  FUNCTION: qbi_txn_req_databuf_get_field
===========================================================================*/
/*!
    @brief Gets a pointer to a variable length field in the transaction's
    request buffer

    @details
    Checks that the offset is 4-byte aligned as required by spec, and that
    the InformationBuffer is large enough to contain the size indicated.
    If these checks fail, sets the status field in the transaction to the
    relevant status for the error encountered, and returns NULL.

    @param txn
    @param field_desc Contains the offset to the variable length field
    (starting from the beginning of the InformationBuffer) and the size of
    the data in the field, not including any padding added by the host
    @param initial_offset Offset to the start of the CID structure: will
    be added to the offset value in the field_desc. This is used for nested
    structures and in all other cases should be 0.
    @param max_size Maximum size allowed for the field per spec. If there
    is no limit, use 0.

    @return void* Pointer to field, or NULL on failure
*/
/*=========================================================================*/
void *qbi_txn_req_databuf_get_field
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *field_desc,
  uint32                             initial_offset,
  uint32                             max_size
)
{
  uint8 *field = NULL;
  uint32 absolute_offset;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(txn);
  QBI_CHECK_NULL_PTR_RET_NULL(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_NULL(field_desc);

  /* The absolute offset is relative to the start of the InformationBuffer */
  absolute_offset = initial_offset + field_desc->offset;
  if (field_desc->offset == 0 || field_desc->size == 0)
  {
    QBI_LOG_E_0("Field is empty!");
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else if (max_size > 0 && field_desc->size > max_size)
  {
    QBI_LOG_E_3("Field at offset %d is too big! Host provided size %d, but "
                "maximum allowed is %d", field_desc->offset, field_desc->size,
                max_size);
    txn->status = QBI_MBIM_STATUS_PARAMETER_TOO_LONG;
  }
  else if (txn->req.size < (absolute_offset + field_desc->size))
  {
    QBI_LOG_E_4("Request buffer of size %d too small for field at offset %d+%d "
                "with size %d!", txn->req.size, initial_offset,
                field_desc->offset, field_desc->size);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else if ((absolute_offset % 4) != 0)
  {
    QBI_LOG_E_2("Field at invalid (non-aligned) offset %d+%d!",
                initial_offset, field_desc->offset);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else
  {
    field = ((uint8 *) txn->req.data) + absolute_offset;
  }

  return field;
} /* qbi_txn_req_databuf_get_field() */

/*===========================================================================
  FUNCTION: qbi_txn_rsp_databuf_get_field
===========================================================================*/
/*!
    @brief Gets a pointer to a variable length field in the transaction's
    response buffer

    @details
    Checks that the offset is 4-byte aligned as required by spec, and that
    the InformationBuffer is large enough to contain the size indicated.
    If these checks fail, sets the status field in the transaction to the
    relevant status for the error encountered, and returns NULL.

    @param txn
    @param field_desc Contains the offset to the variable length field
    (starting from the beginning of the InformationBuffer) and the size of
    the data in the field, not including any padding added by the host
    @param initial_offset Offset to the start of the CID structure: will
    be added to the offset value in the field_desc. This is used for nested
    structures and in all other cases should be 0.
    @param max_size Maximum size allowed for the field per spec. If there
    is no limit, use 0.

    @return void* Pointer to field, or NULL on failure
*/
/*=========================================================================*/
void *qbi_txn_rsp_databuf_get_field
(
  qbi_txn_s                         *txn,
  const qbi_mbim_offset_size_pair_s *field_desc,
  uint32                             initial_offset,
  uint32                             max_size
)
{
  uint8 *field = NULL;
  uint32 absolute_offset;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(txn);
  QBI_CHECK_NULL_PTR_RET_NULL(txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_NULL(field_desc);

  /* The absolute offset is relative to the start of the InformationBuffer */
  absolute_offset = initial_offset + field_desc->offset;
  if (field_desc->offset == 0 || field_desc->size == 0)
  {
    QBI_LOG_E_0("Field is empty!");
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else if (max_size > 0 && field_desc->size > max_size)
  {
    QBI_LOG_E_3("Field at offset %d is too big! Host provided size %d, but "
                "maximum allowed is %d", field_desc->offset, field_desc->size,
                max_size);
    txn->status = QBI_MBIM_STATUS_PARAMETER_TOO_LONG;
  }
  else if (txn->rsp.size < (absolute_offset + field_desc->size))
  {
    QBI_LOG_E_4("Response buffer of size %d too small for field at offset %d+%d "
                "with size %d!", txn->rsp.size, initial_offset,
                field_desc->offset, field_desc->size);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else if ((absolute_offset % 4) != 0)
  {
    QBI_LOG_E_2("Field at invalid (non-aligned) offset %d+%d!",
                initial_offset, field_desc->offset);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else
  {
    field = ((uint8 *) txn->rsp.data) + absolute_offset;
  }

  return field;
} /* qbi_txn_rsp_databuf_get_field() */

/*===========================================================================
  FUNCTION: qbi_txn_rsp_databuf_add_field
===========================================================================*/
/*!
    @brief Appends a new variable length field to the DataBuffer portion of
    the InformationBuffer

    @details
    The field will be properly aligned per MBIM requirements. In the event
    of allocation failure, field_desc will not be modified.

    @param txn
    @param field_desc Offset/size pair that will be populated with the
    details for the newly added field (usually in the fixed length part
    of the response buffer)
    @param initial_offset Offset to the start of the CID structure: will
    be subtracted from the absolute offset. This is used for nested
    structures and in all other cases should be 0.
    @param field_len Size of the requested field to append to the response.
    Note that the allocated size may be greater than requested to allow for
    padding to a 4 byte boundary.
    @param offset Will be populated with the offset from the beginning
    of the response InformationBuffer to the new field
    @param data If not NULL, a buffer of size field_len to copy into the
    field after successful allocation. If NULL, the newly allocated field
    will be initialized to all zeros

    @return void* Pointer to the buffer to contain data for the new field,
    or NULL on failure
*/
/*=========================================================================*/
void *qbi_txn_rsp_databuf_add_field
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *field_desc,
  uint32                       initial_offset,
  uint32                       field_len,
  const void                  *data
)
{
  uint8 *databuf = NULL;
  uint8 post_pad;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(txn);
  QBI_CHECK_NULL_PTR_RET_NULL(field_desc);

  if (initial_offset > txn->infobuf_len_total)
  {
    QBI_LOG_E_2("Invalid initial_offset %d for response with current size %d",
                initial_offset, txn->infobuf_len_total);
  }
  else
  {
    /* To properly calculate the offset, everything in the
       InformationBuffer that comes before the DataBuffer must have already
       been allocated. A CID with no fixed-length structure is the only
       situation where this would be allowed. */
    if (txn->infobuf_len_total == 0)
    {
      QBI_LOG_W_1("Adding field to DataBuffer with no static response "
                  "buffer allocated! (txn iid %d)", txn->iid);
    }

    /* The field must be padded to a 4 byte boundary at the end, even if it
       is the last field in the message. The start of the field must also
       be padded to a 4 byte boundary, but this is taken care of by careful
       definition of the fixed length portion of the InformationBuffer. */
    if (txn->infobuf_len_total % 4)
    {
      QBI_LOG_W_2("Transaction iid %d has invalid InformationBuffer length %d "
                  "(must be a multiple of 4)", txn->iid, txn->infobuf_len_total);
    }
    post_pad = (field_len % 4);
    if (post_pad > 0)
    {
      post_pad = (4 - post_pad);
    }

    databuf = qbi_util_buf_chain_add(
      &txn->rsp_databuf, (field_len + post_pad), data, field_len);
    if (databuf == NULL)
    {
      QBI_LOG_E_0("Couldn't allocate new response DataBuffer field!");
    }
    else
    {
      /* Update the offset/size pair in the static structure to point to where
         this new field will be in the response. */
      field_desc->offset = (txn->infobuf_len_total - initial_offset);
      field_desc->size = field_len;

      /* Update the transaction's response length counter so we know what the
         offset will be for the next field in the DataBuffer */
      txn->infobuf_len_total += (field_len + post_pad);

      QBI_LOG_D_4("Added new field at offset %d with size %d (post pad %d) - "
                  "new total rsp len %d", field_desc->offset,
                  field_desc->size, post_pad, txn->infobuf_len_total);
    }
  }

  return databuf;
} /* qbi_txn_rsp_databuf_add_field() */

/*===========================================================================
  FUNCTION: qbi_txn_rsp_databuf_add_string_ascii_to_utf16
===========================================================================*/
/*!
    @brief Appends a field to the response DataBuffer, and populates it with
    the contents of a string after converting it from ASCII to UTF-16

    @details
    The response buffer will be returned to its original state in the
    event of failure.

    @param txn
    @param field_desc Will be populated with the offset to the start of the
    field and the size of data in the field
    @param initial_offset Offset to the start of the CID structure: will
    be subtracted from the absolute offset. This is used for nested
    structures and in all other cases should be 0.
    @param max_size Maximum size to allowed for the new field
    @param ascii_str ASCII string used to populate the field
    @param ascii_str_len Size of the buffer holding the ASCII string

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_txn_rsp_databuf_add_string_ascii_to_utf16
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *field_desc,
  uint32                       initial_offset,
  uint32                       max_size,
  const char                  *ascii_str,
  uint32                       ascii_str_len
)
{
  uint8 *utf16_str;
  uint32 field_size;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(field_desc);
  QBI_CHECK_NULL_PTR_RET_FALSE(ascii_str);

  field_size = QBI_STRLEN(ascii_str) * 2;
  if (field_size == 0)
  {
    QBI_LOG_W_0("Input string is empty!");
    field_desc->offset = 0;
    field_desc->size = 0;
    success = TRUE;
  }
  else
  {
    if (field_size > max_size)
    {
      QBI_LOG_W_2("String is too long to fit in field! Truncating from %d to %d"
                  " characters", field_size / 2, max_size / 2);
      field_size = max_size;
    }

    utf16_str = qbi_txn_rsp_databuf_add_field(
      txn, field_desc, initial_offset, field_size, NULL);
    if (utf16_str != NULL)
    {
      field_desc->size = qbi_util_ascii_to_utf16(ascii_str, ascii_str_len,
                                                 utf16_str, field_size);
      if (field_desc->size == field_size)
      {
        QBI_LOG_STR_3("Added string '%s' converted to UTF-16 at offset %d with "
                      "size %d", ascii_str, field_desc->offset,
                      field_desc->size);
        success = TRUE;
      }
      else
      {
        QBI_LOG_E_2("Conversion to UTF-16 failed! Returned %d, expected %d",
                    field_desc->size, field_size);
        field_desc->offset = 0;
        field_desc->size = 0;

        /* Remove the field we just added to the DataBuffer, since string
           conversion failed */
        qbi_util_buf_chain_free_last(&txn->rsp_databuf);
      }
    }
  }

  return success;
} /* qbi_txn_rsp_databuf_add_string_ascii_to_utf16() */

/*===========================================================================
  FUNCTION: qbi_txn_rsp_databuf_consolidate
===========================================================================*/
/*!
    @brief Performs consolidation of smaller DataBuffer items into larger
    ones to reduce memory overhead. See important note below.

    @details
    This function should be called periodically in situations where
    many (>20) small DataBuffer fields may be added to the transaction.

    @param txn

    @return boolean TRUE on success, FALSE on unexpected failure that could
    leave the DataBuffer in a bad state. A return value of TRUE does not
    necessarily mean that consolidation was actually performed.

    @note As consolidation involves freeing DataBuffer fields, any pointers
    returned by previous calls to qbi_txn_rsp_databuf_add_field() must be
    considered invalid after this function returns. Therefore this
    function should only be called after modifying the already allocated
    DataBuffer fields is complete.

    @see qbi_util_buf_consolidate
*/
/*=========================================================================*/
boolean qbi_txn_rsp_databuf_consolidate
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  return qbi_util_buf_chain_consolidate(&txn->rsp_databuf);
} /* qbi_txn_rsp_databuf_consolidate() */

/*===========================================================================
  FUNCTION: qbi_txn_rsp_databuf_extract
===========================================================================*/
/*!
    @brief Populates a buffer with the concatenated InformationBuffer data:
    fixed length structure + DataBuffer fields w/padding (the MBIM wire
    format of InformationBuffer)

    @details

    @param txn
    @param pkt_buf Buffer to store wire format representation
    @param pkt_buf_len Maximum number of bytes to copy into pkt_buf
    @param info_buf_offset Offset into the InformationBuffer, i.e. where
    we should start copying from

    @return uint32 Number of bytes set in pkt_buf
*/
/*=========================================================================*/
uint32 qbi_txn_rsp_databuf_extract
(
  qbi_txn_s *txn,
  uint8     *pkt_buf,
  uint32     pkt_buf_len,
  uint32     info_buf_offset
)
{
  uint32 bytes_to_copy;
  uint32 bytes_copied = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(txn);
  QBI_CHECK_NULL_PTR_RET_ZERO(pkt_buf);

  /* First, extract the fixed length part of the InformationBuffer, assuming
     the requested offset hasn't already passed its full length */
  if (info_buf_offset < txn->rsp.size)
  {
    /* We can only copy the smaller of: amount of available data (starting @
       given offset), or the size of the destination buffer */
    bytes_to_copy = MIN((txn->rsp.size - info_buf_offset), pkt_buf_len);
    QBI_MEMSCPY(pkt_buf, pkt_buf_len,
                ((uint8 *) txn->rsp.data + info_buf_offset), bytes_to_copy);

    pkt_buf         += bytes_to_copy;
    pkt_buf_len     -= bytes_to_copy;
    info_buf_offset += bytes_to_copy;
    bytes_copied    += bytes_to_copy;
  }

  /* Next, populate the packet with the DataBuffer */
  if (info_buf_offset >= txn->rsp.size && pkt_buf_len > 0)
  {
    bytes_to_copy = qbi_util_buf_chain_extract(
      &txn->rsp_databuf, (info_buf_offset - txn->rsp.size), pkt_buf_len,
      pkt_buf);
    if (bytes_to_copy == 0)
    {
      QBI_LOG_E_0("Error extracting data from response DataBuffer!");
    }
    bytes_copied += bytes_to_copy;
  }

  return bytes_copied;
} /* qbi_txn_rsp_databuf_extract() */

/*===========================================================================
  FUNCTION: qbi_txn_set_completion_cb
===========================================================================*/
/*!
    @brief Update a transaction's completion callback function

    @details

    @param txn
    @param completion_cb
*/
/*=========================================================================*/
void qbi_txn_set_completion_cb
(
  qbi_txn_s               *txn,
  qbi_txn_completion_cb_f *completion_cb
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  qbi_os_fptr_encode(&txn->completion_cb, (qbi_os_void_f *) completion_cb);
} /* qbi_txn_set_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_txn_set_rsp_buf
===========================================================================*/
/*!
    @brief Sets the transaction's response buffer to an a buffer that has
    already been allocated.

    @details
    This serves as a memory optimization: rather than allocating a new
    buffer, then copying the data over, only the buffer handle is copied.
    Takes no action if one of the parameters is NULL.

    @param txn
    @param buf Buffer to use as the transaction's response buffer. Upon
    successful return from this function, this will be reinitialized as
    an empty buffer.
*/
/*=========================================================================*/
void qbi_txn_set_rsp_buf
(
  qbi_txn_s      *txn,
  qbi_util_buf_s *buf
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(buf);

  qbi_util_buf_swap(&txn->rsp, buf);

  /* Update the size of the InformationBuffer: buf now contains the old
     txn->rsp buffer. */
  txn->infobuf_len_total -= buf->size;
  txn->infobuf_len_total += txn->rsp.size;

  /* Free any data from the old txn->rsp buffer */
  qbi_util_buf_free(buf);
} /* qbi_txn_set_rsp_buf() */

/*===========================================================================
  FUNCTION: qbi_txn_set_timeout
===========================================================================*/
/*!
    @brief Update a transaction's timeout settings

    @details
    If a custom timeout callback does not result in releasing the
    transaction, it must reset the timeout by calling this function again.

    @param txn
    @param delay Delay in milliseconds until the timeout callback should
    be executed. Can be QBI_TXN_TIMEOUT_DISABLED to disable the timeout.
    @param timeout_cb Function to invoke when the timeout is reached, or
    NULL to leave the default, which will abort the transaction
*/
/*=========================================================================*/
void qbi_txn_set_timeout
(
  qbi_txn_s            *txn,
  qbi_os_time_ms_t      delay,
  qbi_txn_timeout_cb_f *timeout_cb
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  if (delay < 0)
  {
    QBI_LOG_E_2("Can't use negative timeout %d for txn iid %d", delay, txn->iid);
  }
  else if (delay == QBI_TXN_TIMEOUT_DISABLED)
  {
    QBI_LOG_W_1("Disabling timeout for txn iid %d", txn->iid);
    txn->timeout = QBI_TXN_TIMEOUT_DISABLED;
  }
  else
  {
    QBI_LOG_D_2("Setting timeout in %d ms for txn iid %d", delay, txn->iid);
    txn->timeout = qbi_os_time_get_ms() + delay;
  }

  if (timeout_cb == NULL)
  {
    qbi_os_fptr_encode(&txn->timeout_cb,
                       (qbi_os_void_f *) qbi_txn_default_timeout_cb);
  }
  else
  {
    qbi_os_fptr_encode(&txn->timeout_cb, (qbi_os_void_f *) timeout_cb);
  }

  txn->timeout_invoked = FALSE;
} /* qbi_txn_set_timeout() */

/*===========================================================================
  FUNCTION: qbi_txn_transfer_qmi_txns
===========================================================================*/
/*!
    @brief Transfer all QMI transactions from one QBI transaction to another

    @details
    This function is generally used when one transaction has failed
    unexpectedly or was cancelled by the host, but we still need to know the
    response to any outstanding QMI transactions. By transferring the QMI
    transactions to a new internal transaction in the completion callback,
    we can still process the QMI response.

    @param src_txn
    @param dst_txn

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_txn_transfer_qmi_txns
(
  const qbi_txn_s *src_txn,
  qbi_txn_s       *dst_txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_util_list_iter_s iter;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(src_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(dst_txn);

  /*! @note We are removing the const classifier here from src_txn->qmi_txns,
      but the list will not be modified. The list iterator needs a non-const
      pointer to support removal at the iterator's position, but we will not
      use this function here, so src_txn itself will not be modified. */
  qbi_util_list_iter_init((qbi_util_list_s *) &src_txn->qmi_txns, &iter);
  while ((qmi_txn = (qbi_qmi_txn_s *) qbi_util_list_iter_next(&iter)) != NULL)
  {
    QBI_LOG_I_2("Transaction iid %d is adopting a qmi_txn from parent iid %d",
                dst_txn->iid, src_txn->iid);
    if (!qbi_util_list_push_back(&dst_txn->qmi_txns, qmi_txn))
    {
      /* Couldn't allocate memory for the new list entry. Stop copying here and
         do not change the qmi_txn's parent pointer so at least we can avoid a
         memory leak. */
      QBI_LOG_E_0("Couldn't add QMI transaction to adoptive parent's list!");
      success = FALSE;
      break;
    }

    /* Note that we change the pointer to the parent transaction, but don't
       remove the qmi_txn from src_txn's list. We do this because this function
       is normally called in a completion callback, where src_txn should not be
       modified. To compensate, the function qbi_qmi_txn_free_all_from_txn()
       will not free the qmi_txn unless parent pointer matches. */
    qmi_txn->parent = dst_txn;
    if (qmi_txn->status == QBI_QMI_TXN_STATUS_WAITING_RSP)
    {
      dst_txn->qmi_txns_pending++;
    }
  }

  return success;
} /* qbi_txn_transfer_qmi_txns() */

