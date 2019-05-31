/*!
  @file
  qbi_msg.c

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
04/13/18  nk   Fixing SSR yellow bang issue after host wakes up from sleep
03/13/18  mm   Added support for SSR in Airplane mode
02/05/18  mm   Added logic to handle modem SSR
07/25/13  bd   Simplify qbi_msg_send_open_rsp so the txn is always freed
11/08/12  bd   Improvements to fragmented request handling
09/21/12  bd   Drop duplicate MBIM_OPEN request received while already opening
01/30/12  cy   Add NTB max num/size config and Reset function
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_msg.h"
#include "qbi_msg_mbim.h"
#include "qbi_qmi.h"
#include "qbi_hc.h"
#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_qmi.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_task.h"
#include "qbi_txn.h"
#include "qbi_util.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! @brief Minimum control transfer size accepted by the device
    @note Attempting to use a value smaller than this is not possible as it
    does not allow sending command responses
*/
#define QBI_MSG_MIN_CONTROL_TRANSFER (sizeof(qbi_msg_cmd_req_s))

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static void qbi_msg_abort_cmd_frag
(
  qbi_ctx_s *ctx
);

static uint32 qbi_msg_calc_total_frags
(
  qbi_ctx_s *ctx,
  uint32     infobuf_len,
  uint32     initial_hdr_size
);

static void qbi_msg_cmd_frag_final
(
  qbi_ctx_s *ctx
);

static void qbi_msg_cmd_frag_initial
(
  qbi_ctx_s *ctx,
  qbi_txn_s *txn,
  uint32     infobuf_rcvd,
  uint32     total_frag
);

static void qbi_msg_cmd_frag_next
(
  qbi_ctx_s            *ctx,
  qbi_util_buf_const_s *buf
);

static void qbi_msg_cmd_frag_timeout_cb
(
  void *cb_data
);

static void qbi_msg_cmd_frag_timeout_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static void qbi_msg_handle_close
(
  qbi_ctx_s  *ctx,
  const void *data,
  uint32      data_len
);

static void qbi_msg_handle_cmd
(
  qbi_ctx_s            *ctx,
  qbi_util_buf_const_s *buf
);

static void qbi_msg_reset_cmd_frag
(
  qbi_ctx_s *ctx
);

static boolean qbi_msg_sanity_check_cmd_frag
(
  qbi_ctx_s            *ctx,
  qbi_util_buf_const_s *buf
);

static boolean qbi_msg_sanity_check_cmd_req
(
  qbi_ctx_s  *ctx,
  const void *data,
  uint32      data_len
);

static boolean qbi_msg_sanity_check_completed_txn
(
  const qbi_txn_s *txn,
  const uint8    **dev_svc_uuid
);

static uint32 qbi_msg_send_cmd_rsp_initial
(
  qbi_txn_s   *txn,
  const uint8 *dev_svc_uuid,
  uint8       *pkt_buf,
  uint32       pkt_buf_len
);

void qbi_msg_send_cmd_frags
(
  qbi_txn_s *txn,
  uint32     infobuf_bytes_sent,
  uint8     *pkt_buf,
  uint32     pkt_buf_len
);

static uint32 qbi_msg_send_event_initial
(
  qbi_txn_s   *txn,
  const uint8 *dev_svc_uuid,
  uint8       *pkt_buf,
  uint32       pkt_buf_len
);

static void qbi_msg_send_open_done
(
  qbi_ctx_s *ctx,
  uint32     status,
  uint32     txn_id
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_msg_abort_cmd_frag
===========================================================================*/
/*!
    @brief Aborts the current multi-fragment command request

    @details
    ctx->cmd_frag will be reset, so any subsequent packets for the aborted
    transaction should be dropped during qbi_msg_sanity_check_cmd_req

    @param ctx
*/
/*=========================================================================*/
static void qbi_msg_abort_cmd_frag
(
  qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_LOG_W_1("Aborting multi-fragment command request (txn_id 0x%08x)",
              ctx->cmd_frag.txn->txn_id);
  qbi_txn_free(ctx->cmd_frag.txn);
  qbi_msg_reset_cmd_frag(ctx);
} /* qbi_msg_abort_cmd_frag() */

/*===========================================================================
  FUNCTION: qbi_msg_calc_total_frags
===========================================================================*/
/*!
    @brief Calculate the number of fragments required to transfer the given
    InformationBufferLength

    @details

    @param ctx
    @param infobuf_len
    @param initial_hdr_size Size of the initial header
    (i.e. sizeof(qbi_msg_cmd_req_s) or sizeof(qbi_msg_cmd_rsp_s))

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_msg_calc_total_frags
(
  qbi_ctx_s *ctx,
  uint32     infobuf_len,
  uint32     initial_hdr_size
)
{
  uint32 total_frags = 0;
  uint32 first_pkt_max_data;
  uint32 other_pkt_max_data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx);

  first_pkt_max_data = ctx->max_xfer - initial_hdr_size;
  if (infobuf_len <= first_pkt_max_data)
  {
    total_frags = 1;
  }
  else
  {
    other_pkt_max_data = ctx->max_xfer - sizeof(qbi_msg_cmd_frag_s);
    total_frags = 2 + (infobuf_len - first_pkt_max_data) / other_pkt_max_data;
  }

  return total_frags;
} /* qbi_msg_calc_total_frags() */

/*===========================================================================
  FUNCTION: qbi_msg_cmd_frag_final
===========================================================================*/
/*!
    @brief Finalizes the current multi-fragment request

    @details
    Verifies that we have received all expected data, then dispatches the
    request and cleans up ctx->cmd_frag.

    @param ctx
*/
/*=========================================================================*/
static void qbi_msg_cmd_frag_final
(
  qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  if (ctx->cmd_frag.bytes_rcvd != ctx->cmd_frag.txn->req.size)
  {
    QBI_LOG_E_2("Wrong amount of data received across multi-fragment request! "
                "Got %d bytes, expected %d",
                ctx->cmd_frag.bytes_rcvd, ctx->cmd_frag.txn->req.size);
    qbi_msg_send_error(ctx, ctx->cmd_frag.txn->txn_id, QBI_MBIM_ERROR_UNKNOWN);
    qbi_msg_abort_cmd_frag(ctx);
  }
  else
  {
    qbi_txn_set_timeout(ctx->cmd_frag.txn, QBI_TXN_TIMEOUT_DEFAULT_MS, NULL);
    qbi_svc_dispatch(ctx->cmd_frag.txn);
    qbi_msg_reset_cmd_frag(ctx);
  }
} /* qbi_msg_cmd_frag_final() */

/*===========================================================================
  FUNCTION: qbi_msg_cmd_frag_initial
===========================================================================*/
/*!
    @brief Prepares for receiving the remainder of a multi-frag command

    @details
    Sets up the cmd_frag struct in the QBI context to expect more fragments
    for the given transaction. This function must only be called after
    processing the first fragment of a multi-fragment transaction, and when
    no other multi-fragment transaction is pending.

    @param ctx
    @param txn
    @param infobuf_rcvd Bytes of InformationBuffer received in first packet
    @param total_frag Total number of fragments expected (should be > 1)

    @see qbi_msg_cmd_frag_next
*/
/*=========================================================================*/
static void qbi_msg_cmd_frag_initial
(
  qbi_ctx_s *ctx,
  qbi_txn_s *txn,
  uint32     infobuf_rcvd,
  uint32     total_frag
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(txn);

  qbi_msg_reset_cmd_frag(ctx);
  ctx->cmd_frag.pending    = TRUE;
  ctx->cmd_frag.total_frag = total_frag;
  ctx->cmd_frag.next_frag  = 1;
  ctx->cmd_frag.txn        = txn;
  ctx->cmd_frag.bytes_rcvd = infobuf_rcvd;
  (void) qbi_os_timer_set(&ctx->cmd_frag.timer, QBI_MSG_FRAGMENT_TIMEOUT_MS);

  /* Disable the overall transaction timeout - this timer will be started
     after all fragments are received. */
  qbi_txn_set_timeout(txn, QBI_TXN_TIMEOUT_DISABLED, NULL);

  QBI_LOG_D_1("Prepared for command request with %d fragments", total_frag);
} /* qbi_msg_cmd_frag_initial() */

/*===========================================================================
  FUNCTION: qbi_msg_cmd_frag_next
===========================================================================*/
/*!
    @brief Handles a subsequent command fragment

    @details
    On success, updates ctx->cmd_frag for the next expected fragment, and
    invokes qbi_msg_cmd_frag_final if this was the last fragment.

    @param ctx
    @param buf

    @see qbi_msg_cmd_frag_initial
*/
/*=========================================================================*/
static void qbi_msg_cmd_frag_next
(
  qbi_ctx_s            *ctx,
  qbi_util_buf_const_s *buf
)
{
  const uint8 *infobuf;
  uint8 *dst;
  uint32 infobuf_len;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(buf);
  QBI_CHECK_NULL_PTR_RET(buf->data);
  QBI_CHECK_NULL_PTR_RET(ctx->cmd_frag.txn);

  if (qbi_msg_sanity_check_cmd_frag(ctx, buf))
  {
    infobuf = (const uint8 *) buf->data + sizeof(qbi_msg_cmd_frag_s);
    infobuf_len = buf->size - sizeof(qbi_msg_cmd_frag_s);
    dst = (uint8 *) ctx->cmd_frag.txn->req.data + ctx->cmd_frag.bytes_rcvd;

    /* Make sure we are not going to overflow the buffer */
    if (infobuf_len > (ctx->cmd_frag.txn->req.size -
                       ctx->cmd_frag.bytes_rcvd))
    {
      QBI_LOG_E_2("Received more data than expected! Got %d, expected <= %d",
                  infobuf_len,
                  (ctx->cmd_frag.txn->req.size - ctx->cmd_frag.bytes_rcvd));
      qbi_msg_send_error(ctx, ctx->cmd_frag.txn->txn_id,
                         QBI_MBIM_ERROR_LENGTH_MISMATCH);
      qbi_msg_abort_cmd_frag(ctx);
    }
    else
    {
      QBI_MEMSCPY(dst, (ctx->cmd_frag.txn->req.size - ctx->cmd_frag.bytes_rcvd),
                  infobuf, infobuf_len);
      ctx->cmd_frag.bytes_rcvd += infobuf_len;
      ctx->cmd_frag.next_frag++;

      /* Finalize and dispatch the command if this is the final fragment,
       * otherwise keep waiting for additional fragments. */
      if (ctx->cmd_frag.next_frag == ctx->cmd_frag.total_frag)
      {
        qbi_msg_cmd_frag_final(ctx);
      }
      else
      {
        (void) qbi_os_timer_set(&ctx->cmd_frag.timer,
                                QBI_MSG_FRAGMENT_TIMEOUT_MS);
      }
    }
  }
} /* qbi_msg_cmd_frag_next() */

/*===========================================================================
  FUNCTION: qbi_msg_cmd_frag_timeout_cb
===========================================================================*/
/*!
    @brief Timer callback that gets triggered when the time between fragments
    is too long per spec and the transaction should be aborted.

    @details
    This runs outside the QBI task.

    @param cb_data
*/
/*=========================================================================*/
static void qbi_msg_cmd_frag_timeout_cb
(
  void *cb_data
)
{
  qbi_ctx_s *ctx;
/*-------------------------------------------------------------------------*/
  ctx = (qbi_ctx_s *) cb_data;

  if (!qbi_task_cmd_send(ctx, QBI_TASK_CMD_ID_CMD_FRAG_TIMEOUT,
                         qbi_msg_cmd_frag_timeout_cmd_cb, NULL))
  {
    QBI_LOG_E_0("Couldn't send command for fragment timeout!");
  }
} /* qbi_msg_cmd_frag_timeout_cb() */

/*===========================================================================
  FUNCTION: qbi_msg_cmd_frag_timeout_cmd_cb
===========================================================================*/
/*!
    @brief QBI task command callback function for command fragment timeout

    @details
    Sends error message and aborts the pending fragmented transaction.
    Runs in the context of the QBI task.

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_msg_cmd_frag_timeout_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_ARG_NOT_USED(cmd_id);
  QBI_ARG_NOT_USED(data);

  if (!ctx->cmd_frag.pending || ctx->cmd_frag.txn == NULL)
  {
    QBI_LOG_W_0("Got command fragment timeout callback but no pending "
                "transaction!");
  }
  else
  {
    QBI_LOG_E_1("Timeout while receiving fragments for transaction ID 0x%08x",
                ctx->cmd_frag.txn->txn_id);
    qbi_msg_send_error(ctx, ctx->cmd_frag.txn->txn_id,
                       QBI_MBIM_ERROR_TIMEOUT_FRAGMENT);
    qbi_msg_abort_cmd_frag(ctx);
  }
} /* qbi_msg_cmd_frag_timeout_cmd_cb() */

/*===========================================================================
  FUNCTION: qbi_msg_handle_close
===========================================================================*/
/*!
    @brief Handles a MBIM_CLOSE_MSG request

    @details

    @param ctx
    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_msg_handle_close
(
  qbi_ctx_s  *ctx,
  const void *data,
  uint32      data_len
)
{
  const qbi_msg_close_req_s *close_req;
  qbi_msg_close_rsp_s close_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(data);

  QBI_LOG_I_0("Handling MBIM_CLOSE_MSG");
  close_req = (const qbi_msg_close_req_s *) data;
  if (data_len < sizeof(qbi_msg_close_req_s))
  {
    QBI_LOG_E_2("Short close request! Got %d bytes, expected %d", data_len,
                sizeof(qbi_msg_close_req_s));
    qbi_msg_send_error(ctx, close_req->hdr.txn_id, QBI_MBIM_ERROR_UNKNOWN);
  }
  else
  {
    if (data_len > sizeof(qbi_msg_close_req_s))
    {
      QBI_LOG_W_2("Close request contains additional data which will be "
                  "ignored. Got %d bytes, expected %d", data_len,
                  sizeof(qbi_msg_close_req_s));
    }
    qbi_msg_context_close(ctx);

    /* Build the response */
    close_rsp.hdr.msg_type = QBI_MSG_MBIM_CLOSE_DONE;
    close_rsp.hdr.txn_id   = close_req->hdr.txn_id;
    close_rsp.hdr.msg_len  = sizeof(close_rsp);
    close_rsp.status = QBI_MBIM_STATUS_SUCCESS;

    qbi_hc_tx(ctx, &close_rsp, close_rsp.hdr.msg_len);
  }
} /* qbi_msg_handle_close() */

/*===========================================================================
  FUNCTION: qbi_msg_handle_cmd
===========================================================================*/
/*!
    @brief Handles a MBIM_COMMAND_MSG request

    @details
    Command messages may be split into multiple fragments, so that additional
    complexity is reflected in this function. However, per specification, the
    host must not send the fragments out of order, or intermix other requests
    with the request fragments.

    @param ctx
    @param buf

    @see qbi_msg_cmd_frag_initial, qbi_msg_cmd_frag_next
*/
/*=========================================================================*/
static void qbi_msg_handle_cmd
(
  qbi_ctx_s            *ctx,
  qbi_util_buf_const_s *buf
)
{
  const qbi_msg_cmd_req_s *cmd_req;
  qbi_svc_id_e svc_id;
  qbi_txn_s *txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(buf);
  QBI_CHECK_NULL_PTR_RET(buf->data);

  cmd_req = (const qbi_msg_cmd_req_s *) buf->data;
  if (ctx->cmd_frag.pending == TRUE)
  {
    qbi_msg_cmd_frag_next(ctx, buf);
  }
  else if (qbi_msg_sanity_check_cmd_req(ctx, buf->data, buf->size))
  {
    svc_id = qbi_svc_uuid_to_svc_id(cmd_req->dev_svc_uuid);
    if (svc_id == QBI_SVC_ID_UNKNOWN)
    {
      QBI_LOG_E_0("Received CID for unrecognized device service");
      qbi_msg_send_cmd_error_rsp(ctx, cmd_req->hdr.txn_id, cmd_req->cid,
                                 cmd_req->dev_svc_uuid,
                                 QBI_MBIM_STATUS_NO_DEVICE_SUPPORT);
    }
    else
    {
      txn = qbi_txn_alloc_fragmented(
        ctx, svc_id, cmd_req->hdr.txn_id, cmd_req->cmd_type, cmd_req->cid,
        buf->size - sizeof(qbi_msg_cmd_req_s),
        (const uint8 *) buf->data + sizeof(qbi_msg_cmd_req_s),
        cmd_req->infobuf_len);
      if (txn == NULL)
      {
        QBI_LOG_E_0("Couldn't allocate new transaction!");
        qbi_msg_send_cmd_error_rsp(ctx, cmd_req->hdr.txn_id, cmd_req->cid,
                                   cmd_req->dev_svc_uuid,
                                   QBI_MBIM_STATUS_FAILURE);
      }
      else if (cmd_req->total_frag == 1)
      {
        /* Free the raw packet before dispatching the command, since we have
           copied all the useful data out of it already. This frees up some
           memory to use for allocating QMI requests, etc. */
        qbi_util_buf_free((qbi_util_buf_s *) buf);
        qbi_svc_dispatch(txn);
      }
      else
      {
        qbi_msg_cmd_frag_initial(ctx, txn,
                                 (buf->size - sizeof(qbi_msg_cmd_req_s)),
                                 cmd_req->total_frag);
      }
    }
  }
} /* qbi_msg_handle_cmd() */

/*===========================================================================
  FUNCTION: qbi_msg_handle_host_error
===========================================================================*/
/*!
    @brief Handles a MBIM_HOST_ERROR_MSG

    @details

    @param ctx
    @param data
    @param size
*/
/*=========================================================================*/
static void qbi_msg_handle_host_error
(
  qbi_ctx_s  *ctx,
  const void *data,
  uint32      size
)
{
  const qbi_msg_error_s *error_msg;
  qbi_txn_s *txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(data);

  error_msg = (const qbi_msg_error_s *) data;
  if (size < sizeof(qbi_msg_error_s))
  {
    QBI_LOG_E_2("Received short MBIM_HOST_ERROR_MSG! Size %d expected %d",
                size, sizeof(qbi_msg_error_s));
  }
  else
  {
    QBI_LOG_E_3("Received error code %d with txn_id %d from host on context %d",
                error_msg->error, error_msg->hdr.txn_id, ctx->id);
    if (error_msg->error == QBI_MBIM_ERROR_CANCEL)
    {
      if (ctx->state == QBI_CTX_STATE_OPENED)
      {
        txn = qbi_txn_find_by_id(ctx, error_msg->hdr.txn_id);
        if (txn == NULL)
        {
          QBI_LOG_E_1("Received request to cancel transaction ID %d but not "
                      "found in context!", error_msg->hdr.txn_id);
        }
        else
        {
          /* Set the status flag to FAILURE so any completion callback registered
             with the transaction can know that the request failed. */
          QBI_LOG_W_1("Canceling transaction with iid %d", txn->iid);
          txn->status = QBI_MBIM_STATUS_FAILURE;
          qbi_txn_free(txn);
        }
      }
      else
      {
        QBI_LOG_E_0("Can't cancel transaction ID when context not opened!");
        qbi_msg_send_error(ctx, error_msg->hdr.txn_id, QBI_MBIM_ERROR_NOT_OPENED);
      }
    }
  }
} /* qbi_msg_handle_host_error() */

/*===========================================================================
  FUNCTION: void qbi_msg_log_for_recovery_function
===========================================================================*/
/*!
    @brief Decide whether its a SSR or MBIMD_RECOVERY logging

    @details

    @param ctx
*/
/*=========================================================================*/
char *qbi_msg_log_for_recovery_function
(
  qbi_ctx_s  *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  if(ctx->mbim_recovery)
  {
    return "MBIMD_RECOVERY";
  }
  else
  {
    return "SSR";
  }

}/* qbi_msg_log_for_recovery_function() */

/*===========================================================================
  FUNCTION: qbi_msg_handle_open
===========================================================================*/
/*!
    @brief Handles a MBIM_OPEN_MSG request

    @details

    @param ctx
    @param data
    @param data_len
*/
/*=========================================================================*/
void qbi_msg_handle_open
(
  qbi_ctx_s  *ctx,
  const void *data,
  uint32      data_len
)
{
  const qbi_msg_open_req_s *open_req;
  qbi_txn_s *txn;
  uint32 status = QBI_MBIM_STATUS_FAILURE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(data);

  QBI_LOG_I_0("Handling MBIM_OPEN_MSG");
  qbi_os_log_event(QBI_OS_LOG_EVENT_MBIM_OPEN);
  open_req = (const qbi_msg_open_req_s *) data;
  if (data_len < sizeof(qbi_msg_open_req_s))
  {
    QBI_LOG_E_2("Short open request! Got %d bytes, expected %d.", data_len,
                sizeof(qbi_msg_open_req_s));
    qbi_msg_send_error(ctx, open_req->hdr.txn_id, QBI_MBIM_ERROR_UNKNOWN);
  }
  else if (ctx->state == QBI_CTX_STATE_OPENING &&
           qbi_txn_find_by_id(ctx, open_req->hdr.txn_id) != NULL)
  {
    /* If the host thinks we dropped an MBIM_OPEN request but we are still
       processing it, it may send another one with the same transaction ID. In
       this case, just drop the duplicate request and allow the existing one to
       finish. */
    QBI_LOG_W_0("Got duplicate MBIM_OPEN with same transaction ID; dropping");
  }
  else
  {
    if (data_len > sizeof(qbi_msg_open_req_s))
    {
      QBI_LOG_W_2("Open request contains additional data which will be "
                  "ignored. Got %d bytes, expected %d", data_len,
                  sizeof(qbi_msg_open_req_s));
    }

    /* Process a close if we are already opened/opening. */
    qbi_msg_context_close(ctx);

    if (open_req->max_xfer < QBI_MSG_MIN_CONTROL_TRANSFER)
    {
      QBI_LOG_E_1("Invalid max control transfer size %d", open_req->max_xfer);
    }
    else if (open_req->max_xfer > qbi_hc_get_max_xfer())
    {
      QBI_LOG_E_2("Max control transfer size %d is past max %d",
                  open_req->max_xfer, qbi_hc_get_max_xfer());
      qbi_msg_send_error(ctx, open_req->hdr.txn_id,
                         QBI_MBIM_ERROR_MAX_TRANSFER);
    }
    else if (!qbi_qmi_open(ctx))
    {
      QBI_LOG_E_1("Couldn't initialize QMI for context ID %d!", ctx->id);
    }
    else
    {
      ctx->max_xfer = open_req->max_xfer;
      QBI_LOG_I_1("Using maximum control transfer size = %d", ctx->max_xfer);

      txn = qbi_txn_alloc(ctx, QBI_SVC_ID_UNKNOWN, open_req->hdr.txn_id,
                          QBI_TXN_CMD_TYPE_MBIM_OPEN, QBI_SVC_CID_NONE, 0, NULL);
      if (txn == NULL)
      {
        QBI_LOG_E_1("Couldn't allocate transaction to process open request for "
                    "context ID %d!", ctx->id);
      }
      else if (!qbi_svc_open(txn))
      {
        QBI_LOG_E_1("Couldn't open device services for context ID %d!", ctx->id);
        qbi_txn_free(txn);
      }
      else
      {
        ctx->state = QBI_CTX_STATE_OPENING;
        status = QBI_MBIM_STATUS_SUCCESS;
      }
    }

    /* If we failed early, send the error response now. Otherwise, the response
       will be sent asynchronously */
    if (status != QBI_MBIM_STATUS_SUCCESS)
    {
      qbi_msg_send_open_done(ctx, status, open_req->hdr.txn_id);
      qbi_msg_context_close(ctx);
    }
  }
} /* qbi_msg_handle_open() */

/*===========================================================================
  FUNCTION: qbi_msg_reset_cmd_frag
===========================================================================*/
/*!
    @brief Resets ctx->cmd_frag to its initial state

    @details

    @param ctx
*/
/*=========================================================================*/
static void qbi_msg_reset_cmd_frag
(
  qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  (void) qbi_os_timer_clear(&ctx->cmd_frag.timer);
  ctx->cmd_frag.bytes_rcvd = 0;
  ctx->cmd_frag.next_frag  = 0;
  ctx->cmd_frag.pending    = FALSE;
  ctx->cmd_frag.total_frag = 0;
  ctx->cmd_frag.txn        = NULL;
} /* qbi_msg_reset_cmd_frag() */

/*===========================================================================
  FUNCTION: qbi_msg_sanity_check_cmd_frag
===========================================================================*/
/*!
    @brief Performs sanity checks on a command fragment, i.e.
    MBIM_COMMAND_MSG with CurrentFragment > 0

    @details
    Sends an error message and aborts the multi-fragment transaction on
    failure.

    @param ctx
    @param buf

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_msg_sanity_check_cmd_frag
(
  qbi_ctx_s            *ctx,
  qbi_util_buf_const_s *buf
)
{
  const qbi_msg_cmd_frag_s *cmd_frag;
  boolean result = FALSE;
  boolean reprocess = FALSE;
  qbi_mbim_error_e error = QBI_MBIM_ERROR_UNKNOWN;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(buf);
  QBI_CHECK_NULL_PTR_RET_FALSE(buf->data);

  cmd_frag = (const qbi_msg_cmd_frag_s *) buf->data;
  if (ctx->state != QBI_CTX_STATE_OPENED)
  {
    QBI_LOG_E_0("Received command fragment when device is not opened!");
    error = QBI_MBIM_ERROR_NOT_OPENED;
  }
  else if (buf->size <= sizeof(qbi_msg_cmd_frag_s))
  {
    QBI_LOG_E_2("Received short fragment! Got %d bytes, expected > %d",
                buf->size, sizeof(qbi_msg_cmd_frag_s));
  }
  else if (cmd_frag->hdr.txn_id != ctx->cmd_frag.txn->txn_id)
  {
    QBI_LOG_E_2("Transaction ID mismatch! Received 0x%08x, expected 0x%08x",
                cmd_frag->hdr.txn_id, ctx->cmd_frag.txn->txn_id);
    QBI_LOG_E_4("Fragment info of received message is %d/%d; expecting %d/%d",
                cmd_frag->cur_frag, cmd_frag->total_frag,
                ctx->cmd_frag.next_frag, ctx->cmd_frag.total_frag);
    error = QBI_MBIM_ERROR_FRAGMENT_OUT_OF_SEQUENCE;
    if (cmd_frag->cur_frag == 0)
    {
      reprocess = TRUE;
    }
    else
    {
      /* Send an error for the received transaction ID. An error will also be
         sent for the expected transaction ID. */
      qbi_msg_send_error(ctx, cmd_frag->hdr.txn_id, error);
    }
  }
  else if (cmd_frag->total_frag != ctx->cmd_frag.total_frag ||
           cmd_frag->cur_frag   != ctx->cmd_frag.next_frag)
  {
    QBI_LOG_E_4("Invalid/out of order fragment! Received fragment %d/%d, "
                "expected %d/%d", cmd_frag->cur_frag, cmd_frag->total_frag,
                ctx->cmd_frag.next_frag, ctx->cmd_frag.total_frag);
    error = QBI_MBIM_ERROR_FRAGMENT_OUT_OF_SEQUENCE;
  }
  else
  {
    /* All checks passed */
    result = TRUE;
  }

  if (!result)
  {
    qbi_msg_send_error(ctx, ctx->cmd_frag.txn->txn_id, error);
    qbi_msg_abort_cmd_frag(ctx);

    /* If there was a transaction ID mismatch because we received a new
       transaction before the last fragment of the previous transaction,
       try to process the new transaction by itself. */
    if (reprocess)
    {
      qbi_msg_input(ctx, buf);
    }
  }

  return result;
} /* qbi_msg_sanity_check_cmd_frag() */

/*===========================================================================
  FUNCTION: qbi_msg_sanity_check_cmd_req
===========================================================================*/
/*!
    @brief Perform sanity checks on the contents of a MBIM_COMMAND_MSG
    (initial fragment only)

    @details
    Sends an error message if sanity check failed.

    @param ctx
    @param data
    @param data_len

    @return boolean TRUE if checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_msg_sanity_check_cmd_req
(
  qbi_ctx_s  *ctx,
  const void *data,
  uint32      data_len
)
{
  boolean result = FALSE;
  const qbi_msg_cmd_req_s *cmd_req;
  uint32 infobuf_len;
  qbi_mbim_error_e error = QBI_MBIM_ERROR_UNKNOWN;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(data);

  cmd_req = (const qbi_msg_cmd_req_s *) data;
  if (ctx->state != QBI_CTX_STATE_OPENED)
  {
    QBI_LOG_E_0("Received command request when device is not opened!");
    error = QBI_MBIM_ERROR_NOT_OPENED;
  }
  else if (data_len < sizeof(qbi_msg_cmd_req_s))
  {
    QBI_LOG_E_2("Short command request! Got %d bytes, expected at least %d.",
                data_len, sizeof(qbi_msg_cmd_req_s));
  }
  else if (cmd_req->cmd_type != QBI_MSG_CMD_TYPE_QUERY &&
           cmd_req->cmd_type != QBI_MSG_CMD_TYPE_SET)
  {
    QBI_LOG_E_1("Invalid command type 0x%08x", cmd_req->cmd_type);
  }
  else if (cmd_req->total_frag == 0)
  {
    QBI_LOG_E_0("Invalid total fragments value");
  }
  else if (cmd_req->cur_frag != 0)
  {
    /* Probably the result of an aborted multi-fragment transaction */
    QBI_LOG_E_2("Unexpected non-zero current fragment %d (txn_id 0x%08x)",
                cmd_req->cur_frag, cmd_req->hdr.txn_id);
    error = QBI_MBIM_ERROR_FRAGMENT_OUT_OF_SEQUENCE;
  }
  else
  {
    /* Verify that the InformationBufferLength field matches what we expect.
       For single fragment transactions, it should exactly match the amount of
       data following the header. For multi-fragment transactions, it should
       be greater than the amount we have received. */
    infobuf_len = data_len - sizeof(qbi_msg_cmd_req_s);
    if (cmd_req->total_frag == 1 && cmd_req->infobuf_len != infobuf_len)
    {
      QBI_LOG_E_2("Invalid InformationBufferLength! Got %d, expected %d",
                  cmd_req->infobuf_len, infobuf_len);
      error = QBI_MBIM_ERROR_LENGTH_MISMATCH;
    }
    else if (cmd_req->total_frag > 1 && cmd_req->infobuf_len <= infobuf_len)
    {
      QBI_LOG_E_2("Invalid InformationBufferLength! Got %d, expected > %d",
                  cmd_req->infobuf_len, infobuf_len);
      error = QBI_MBIM_ERROR_LENGTH_MISMATCH;
    }
    else if (qbi_txn_find_by_id(ctx, cmd_req->hdr.txn_id) != NULL)
    {
      QBI_LOG_E_1("Received request with duplicate transaction ID %d!",
                  cmd_req->hdr.txn_id);
      error = QBI_MBIM_ERROR_DUPLICATED_TID;
    }
    else
    {
      /* All tests have passed */
      result = TRUE;
    }
  }

  if (!result)
  {
    qbi_msg_send_error(ctx, cmd_req->hdr.txn_id, error);
  }

  return result;
} /* qbi_msg_sanity_check_cmd_req() */

/*===========================================================================
  FUNCTION: qbi_msg_sanity_check_completed_txn
===========================================================================*/
/*!
    @brief Perform sanity checks on a completed, outgoing QBI transaction

    @details
    If dev_svc_uuid is not NULL, on success (return TRUE) it will be set
    to the buffer containing the device service UUID associated with the
    transaction.

    @param txn
    @param dev_svc_uuid If not NULL,

    @return boolean TRUE on success, FALSE if sanity check failed
*/
/*=========================================================================*/
static boolean qbi_msg_sanity_check_completed_txn
(
  const qbi_txn_s *txn,
  const uint8    **dev_svc_uuid
)
{
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  if (txn != NULL && txn->ctx != NULL)
  {
    if (txn->ctx->state != QBI_CTX_STATE_OPENED)
    {
      QBI_LOG_E_1("Trying to send CID response/event on unopened context ID %d",
                  txn->ctx->id);
    }
    else if (dev_svc_uuid != NULL)
    {
      *dev_svc_uuid = qbi_svc_svc_id_to_uuid(txn->svc_id);
      if (*dev_svc_uuid != NULL)
      {
        result = TRUE;
      }
    }
    else
    {
      result = TRUE;
    }
  }

  return result;
} /* qbi_msg_sanity_check_completed_txn() */

/*===========================================================================
  FUNCTION: qbi_msg_send_cmd_rsp_initial
===========================================================================*/
/*!
    @brief Populates and sends an initial command response packet

    @details
    The packet buffer must already be allocated, and the transaction should
    already be checked for sanity before calling this function. This
    function only sends the first packet, so qbi_msg_send_cmd_frags may
    need to be invoked to transmit the rest of the InformationBuffer.

    @param txn
    @param dev_svc_uuid Pointer to device service UUID
    @param pkt_buf Pre-allocated buffer to be used for outgoing data
    @param pkt_buf_len Size of packet buffer

    @return uint32 Number of bytes of InformationBuffer transmitted

    @see qbi_msg_send_cmd_frags
*/
/*=========================================================================*/
static uint32 qbi_msg_send_cmd_rsp_initial
(
  qbi_txn_s   *txn,
  const uint8 *dev_svc_uuid,
  uint8       *pkt_buf,
  uint32       pkt_buf_len
)
{
  qbi_msg_cmd_rsp_s *rsp;
  uint32 infobuf_bytes_sent = 0;
  uint32 bytes_to_extract = 0;
/*-------------------------------------------------------------------------*/
  if (txn != NULL && pkt_buf != NULL)
  {
    rsp = (qbi_msg_cmd_rsp_s *) pkt_buf;

    rsp->hdr.msg_type = QBI_MSG_MBIM_COMMAND_DONE;
    rsp->hdr.msg_len  = pkt_buf_len;
    rsp->hdr.txn_id   = txn->txn_id;
    rsp->total_frag   = qbi_msg_calc_total_frags(txn->ctx, txn->infobuf_len_total,
                                                 sizeof(qbi_msg_cmd_rsp_s));
    rsp->cur_frag     = 0;
    rsp->cid          = txn->cid;
    rsp->status       = (uint32) txn->status;
    rsp->infobuf_len  = txn->infobuf_len_total;
    QBI_MEMSCPY(rsp->dev_svc_uuid, sizeof(rsp->dev_svc_uuid),
                dev_svc_uuid, sizeof(rsp->dev_svc_uuid));
    if (txn->infobuf_len_total > 0)
    {
      bytes_to_extract = MIN(pkt_buf_len - sizeof(qbi_msg_cmd_rsp_s),
                             txn->infobuf_len_total);
      infobuf_bytes_sent = qbi_txn_rsp_databuf_extract(
        txn, (pkt_buf + sizeof(qbi_msg_cmd_rsp_s)), bytes_to_extract, 0);
    }

    if (infobuf_bytes_sent != bytes_to_extract)
    {
      QBI_LOG_E_3("Couldn't send response to txn_id 0x%08x: Failure extracting "
                  "InformationBuffer from transaction! Asked for %d bytes, got "
                  "%d", rsp->hdr.txn_id, bytes_to_extract, infobuf_bytes_sent);
      qbi_msg_send_error(txn->ctx, rsp->hdr.txn_id, QBI_MBIM_ERROR_UNKNOWN);
      /* Set infobuf_bytes_sent to the total to prevent sending any command
         fragments for this transaction */
      infobuf_bytes_sent = txn->infobuf_len_total;
    }
    else
    {
      QBI_LOG_D_3("Sending command response with %d/%d data bytes (total_frag "
                  "%d)", infobuf_bytes_sent, txn->infobuf_len_total,
                  rsp->total_frag);
      qbi_hc_tx(txn->ctx, pkt_buf, rsp->hdr.msg_len);
    }
  }

  return infobuf_bytes_sent;
} /* qbi_msg_send_cmd_rsp_initial() */

/*===========================================================================
  FUNCTION: qbi_msg_send_cmd_frags
===========================================================================*/
/*!
    @brief Sends remaining InformationBuffer data as command fragments

    @details
    If the InformationBuffer was too large to be sent entirely in the first
    packet, this function will send the rest of the data in one or more
    additional fragments. The packet buffer must already be allocated and
    populated with header information from the first packet, as this
    function will only modify CurrentFragment.

    @param txn QBI transaction containing outgoing InformationBuffer
    @param infobuf_bytes_sent Bytes of InformationBuffer already sent
    @param pkt_buf Pointer to pre-populated packet data buffer
    @param pkt_buf_len Size of packet data buffer
*/
/*=========================================================================*/
void qbi_msg_send_cmd_frags
(
  qbi_txn_s *txn,
  uint32     infobuf_bytes_sent,
  uint8     *pkt_buf,
  uint32     pkt_buf_len
)
{
  qbi_msg_cmd_frag_s *frag;
  uint32 max_infobuf_len;
  uint32 pkt_infobuf_len;
  uint32 bytes_to_extract;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(pkt_buf);

  frag = (qbi_msg_cmd_frag_s *) pkt_buf;
  max_infobuf_len = pkt_buf_len - sizeof(qbi_msg_cmd_frag_s);
  QBI_LOG_D_2("Sending remaining data in fragments (already sent %d max len "
              "%d)", infobuf_bytes_sent, max_infobuf_len);
  while (infobuf_bytes_sent < txn->infobuf_len_total)
  {
    bytes_to_extract = MIN(txn->infobuf_len_total - infobuf_bytes_sent,
                           max_infobuf_len);
    pkt_infobuf_len = qbi_txn_rsp_databuf_extract(
      txn, (pkt_buf + sizeof(qbi_msg_cmd_frag_s)), bytes_to_extract,
      infobuf_bytes_sent);
    if (pkt_infobuf_len != bytes_to_extract)
    {
      QBI_LOG_E_2("Failure extracting InformationBuffer from transaction! "
                  "Asked for %d bytes, got %d", bytes_to_extract,
                  pkt_infobuf_len);
      qbi_msg_send_error(txn->ctx, frag->hdr.txn_id, QBI_MBIM_ERROR_UNKNOWN);
      break;
    }

    frag->cur_frag++;
    frag->hdr.msg_len = sizeof(qbi_msg_cmd_frag_s) + pkt_infobuf_len;
    qbi_hc_tx(txn->ctx, pkt_buf, frag->hdr.msg_len);

    infobuf_bytes_sent += pkt_infobuf_len;
    QBI_LOG_D_4("Sent fragment %d with %d bytes of data (%d/%d sent so far)",
                frag->cur_frag, pkt_infobuf_len, infobuf_bytes_sent,
                txn->infobuf_len_total);
  }
} /* qbi_msg_send_cmd_frags() */

/*===========================================================================
  FUNCTION: qbi_msg_send_event_initial
===========================================================================*/
/*!
    @brief Populates and sends the initial unsolicited event message

    @details
    The packet buffer must already be allocated, and the transaction should
    already be checked for sanity before calling this function. This
    function only sends the first packet, so qbi_msg_send_cmd_frags may
    need to be invoked to transmit the rest of the InformationBuffer.

    @param txn
    @param dev_svc_uuid
    @param pkt_buf
    @param pkt_buf_len

    @return uint32 Number of bytes of InformationBuffer transmitted

    @see qbi_msg_send_cmd_frags
*/
/*=========================================================================*/
static uint32 qbi_msg_send_event_initial
(
  qbi_txn_s   *txn,
  const uint8 *dev_svc_uuid,
  uint8       *pkt_buf,
  uint32       pkt_buf_len
)
{
  qbi_msg_status_ind_s *ind;
  uint32 infobuf_bytes_sent = 0;
  uint32 bytes_to_extract = 0;
/*-------------------------------------------------------------------------*/
  if (txn != NULL && pkt_buf != NULL)
  {
    ind = (qbi_msg_status_ind_s *) pkt_buf;

    ind->hdr.msg_type = QBI_MSG_MBIM_INDICATE_STATUS_MSG;
    ind->hdr.msg_len  = pkt_buf_len;
    ind->hdr.txn_id   = QBI_MSG_TXN_ID_IND;
    ind->total_frag   = qbi_msg_calc_total_frags(txn->ctx, txn->infobuf_len_total,
                                                 sizeof(qbi_msg_status_ind_s));
    ind->cur_frag     = 0;
    ind->cid          = txn->cid;
    ind->infobuf_len  = txn->infobuf_len_total;
    QBI_MEMSCPY(ind->dev_svc_uuid, sizeof(ind->dev_svc_uuid),
                dev_svc_uuid, sizeof(ind->dev_svc_uuid));
    if (txn->infobuf_len_total > 0)
    {
      bytes_to_extract = MIN(pkt_buf_len - sizeof(qbi_msg_status_ind_s),
                             txn->infobuf_len_total);
      infobuf_bytes_sent = qbi_txn_rsp_databuf_extract(
        txn, (pkt_buf + sizeof(qbi_msg_status_ind_s)), bytes_to_extract, 0);
    }

    if (infobuf_bytes_sent != bytes_to_extract)
    {
      QBI_LOG_E_2("Couldn't send event: Failure extracting InformationBuffer "
                  "from transaction! Asked for %d bytes, got %d",
                  bytes_to_extract, infobuf_bytes_sent);
      qbi_msg_send_error(txn->ctx, ind->hdr.txn_id, QBI_MBIM_ERROR_UNKNOWN);
      /* Set infobuf_bytes_sent to the total to prevent sending any command
         fragments for this transaction */
      infobuf_bytes_sent = txn->infobuf_len_total;
    }
    else
    {
      QBI_LOG_D_3("Sending event with %d/%d data bytes (total_frag %d)",
                  infobuf_bytes_sent, txn->infobuf_len_total, ind->total_frag);
      qbi_hc_tx(txn->ctx, pkt_buf, ind->hdr.msg_len);
    }
  }

  return infobuf_bytes_sent;
} /* qbi_msg_send_event_initial() */

/*===========================================================================
  FUNCTION: qbi_msg_send_open_done
===========================================================================*/
/*!
    @brief Sends a MBIM_OPEN_DONE to the host

    @details
    Only sends the message; does not modify the context state.

    @param ctx
    @param status
    @param txn_id
*/
/*=========================================================================*/
static void qbi_msg_send_open_done
(
  qbi_ctx_s *ctx,
  uint32     status,
  uint32     txn_id
)
{
  qbi_msg_open_rsp_s open_rsp;
/*-------------------------------------------------------------------------*/
  open_rsp.hdr.msg_type = QBI_MSG_MBIM_OPEN_DONE;
  open_rsp.hdr.txn_id   = txn_id;
  open_rsp.hdr.msg_len  = sizeof(open_rsp);
  open_rsp.status       = status;

  qbi_os_log_event(QBI_OS_LOG_EVENT_MBIM_OPEN_DONE);
  qbi_hc_tx(ctx, &open_rsp, open_rsp.hdr.msg_len);
} /* qbi_msg_send_open_done() */


/*=============================================================================

  Public Function Definitions

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
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  if (ctx->state != QBI_CTX_STATE_CLOSED)
  {
    if (ctx->cmd_frag.pending)
    {
      qbi_msg_abort_cmd_frag(ctx);
    }
    qbi_txn_free_all(ctx);
    qbi_svc_close(ctx);
    qbi_qmi_close(ctx);
    qbi_qmi_txn_free_all_from_ctx(ctx);

    ctx->state = QBI_CTX_STATE_CLOSED;
    QBI_LOG_I_1("Closed context ID %d", ctx->id);
  }
} /* qbi_msg_context_close() */

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
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  if (!qbi_os_timer_init(
        &ctx->cmd_frag.timer, qbi_msg_cmd_frag_timeout_cb, ctx))
  {
    QBI_LOG_E_0("Couldn't initialize command fragment timer!");
  }
} /* qbi_msg_ctx_init() */

/*===========================================================================
  FUNCTION: qbi_msg_input
===========================================================================*/
/*!
    @brief Dispatch incoming data to the associated MessageType handler

    @details

    @param ctx Associated QBI context
    @param buf Pointer to buffer containing the message data.

    @note The buf argument can't be made const qbi_util_buf_const_s *
    because we may free the buffer before returning from this function. This
    enables a memory optimization whereby the request memory is freed before
    dispatching the command.
*/
/*=========================================================================*/
void qbi_msg_input
(
  qbi_ctx_s            *ctx,
  qbi_util_buf_const_s *buf
)
{
  const qbi_msg_common_hdr_s *hdr;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(buf);
  QBI_CHECK_NULL_PTR_RET(buf->data);

  hdr = (const qbi_msg_common_hdr_s *) buf->data;
  if (buf->size >= sizeof(qbi_msg_common_hdr_s))
  {
    if (hdr->msg_len != buf->size)
    {
      QBI_LOG_E_2("Invalid MessageLength! Got %d, expected %d", hdr->msg_len,
                  buf->size);
      qbi_msg_send_error(ctx, hdr->txn_id, QBI_MBIM_ERROR_LENGTH_MISMATCH);
    }
    else if (ctx->ssr_info.ssr_in_progress == TRUE)
    {
      qbi_msg_send_error(ctx, hdr->txn_id, QBI_MBIM_ERROR_NOT_OPENED);
      QBI_LOG_I_0("SSR in progress so cannot handle any request");
    }
    else if (TRUE == ctx->mbim_recovery)
    {
      qbi_msg_send_error(ctx, hdr->txn_id, QBI_MBIM_ERROR_NOT_OPENED);
      QBI_LOG_I_0("MBIMD_RECOVERY : MBIM attempting to recover, unable "
                  "to handle request.");
    }
    else
    {
      switch (hdr->msg_type)
      {
        case QBI_MSG_MBIM_OPEN_MSG:
          qbi_msg_handle_open(ctx, buf->data, buf->size);
          break;

        case QBI_MSG_MBIM_CLOSE_MSG:
          qbi_msg_handle_close(ctx, buf->data, buf->size);
          break;

        case QBI_MSG_MBIM_COMMAND_MSG:
          qbi_msg_handle_cmd(ctx, buf);
          break;

        case QBI_MSG_MBIM_HOST_ERROR_MSG:
          qbi_msg_handle_host_error(ctx, buf->data, buf->size);
          break;

        default:
          QBI_LOG_E_1("Unexpected message type 0x%08x", hdr->msg_type);
          qbi_msg_send_error(ctx, hdr->txn_id, QBI_MBIM_ERROR_UNKNOWN);
      }
    }
  }
  else
  {
    QBI_LOG_E_1("Payload too short (%d bytes)", buf->size);
    qbi_msg_send_error(ctx, hdr->txn_id, QBI_MBIM_ERROR_UNKNOWN);
  }
} /* qbi_msg_input() */

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
)
{
  qbi_msg_cmd_rsp_s err_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(dev_svc_uuid);

  err_rsp.hdr.msg_type = QBI_MSG_MBIM_COMMAND_DONE;
  err_rsp.hdr.msg_len  = sizeof(qbi_msg_cmd_rsp_s);
  err_rsp.hdr.txn_id   = txn_id;
  err_rsp.total_frag   = 1;
  err_rsp.cur_frag     = 0;
  QBI_MEMSCPY(err_rsp.dev_svc_uuid, sizeof(err_rsp.dev_svc_uuid),
              dev_svc_uuid, sizeof(err_rsp.dev_svc_uuid));
  err_rsp.cid          = cid;
  err_rsp.status       = status;
  err_rsp.infobuf_len  = 0;

  qbi_hc_tx(ctx, &err_rsp, err_rsp.hdr.msg_len);
} /* qbi_msg_send_cmd_error_rsp() */

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
  qbi_txn_s *txn
)
{
  qbi_util_buf_s pkt_buf;
  uint32 infobuf_bytes_sent;
  const uint8 *dev_svc_uuid;
/*-------------------------------------------------------------------------*/
  if (!qbi_msg_sanity_check_completed_txn(txn, &dev_svc_uuid))
  {
    QBI_LOG_E_0("Completed transaction sanity check failed! Dropping response");
  }
  else
  {
    QBI_LOG_D_1("Sending command response for transaction ID 0x%08x",
                txn->txn_id);

    qbi_util_buf_init(&pkt_buf);
    pkt_buf.size = MIN(sizeof(qbi_msg_cmd_rsp_s) + txn->infobuf_len_total,
                       txn->ctx->max_xfer);
    if (qbi_util_buf_alloc_dont_clear(&pkt_buf, pkt_buf.size) == NULL)
    {
      QBI_LOG_E_1("Couldn't allocate buffer (size %d) for response!",
                  MIN(sizeof(qbi_msg_cmd_rsp_s) + txn->infobuf_len_total,
                      txn->ctx->max_xfer));
      qbi_msg_send_cmd_error_rsp(txn->ctx, txn->txn_id, txn->cid, dev_svc_uuid,
                                 QBI_MBIM_STATUS_FAILURE);
    }
    else
    {
      infobuf_bytes_sent = qbi_msg_send_cmd_rsp_initial(txn, dev_svc_uuid,
                                                        pkt_buf.data,
                                                        pkt_buf.size);
      if (infobuf_bytes_sent < txn->infobuf_len_total)
      {
        qbi_msg_send_cmd_frags(txn, infobuf_bytes_sent, pkt_buf.data,
                               pkt_buf.size);
      }

      qbi_util_buf_free(&pkt_buf);
    }
  }

  qbi_txn_free(txn);
} /* qbi_msg_send_cmd_rsp() */

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
)
{
  qbi_msg_error_s error_msg;
/*-------------------------------------------------------------------------*/
  QBI_LOG_E_2("Sending error code %d to host for transaction ID %d", error,
              txn_id);

  error_msg.hdr.msg_type = QBI_MSG_MBIM_FUNCTION_ERROR_MSG;
  error_msg.hdr.msg_len  = sizeof(qbi_msg_error_s);
  error_msg.hdr.txn_id   = txn_id;
  error_msg.error        = error;

  qbi_hc_tx(ctx, &error_msg, error_msg.hdr.msg_len);
} /* qbi_msg_send_error() */

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
  qbi_txn_s *txn
)
{
  qbi_util_buf_s pkt_buf;
  uint32 infobuf_bytes_sent;
  const uint8 *dev_svc_uuid;
/*-------------------------------------------------------------------------*/
  if (!qbi_msg_sanity_check_completed_txn(txn, &dev_svc_uuid))
  {
    QBI_LOG_E_0("Completed transaction sanity check failed! Dropping event");
  }
  else
  {
    QBI_LOG_D_2("Sending event for svc_id %d cid %d", txn->svc_id, txn->cid);

    qbi_util_buf_init(&pkt_buf);
    pkt_buf.size = MIN(sizeof(qbi_msg_status_ind_s) + txn->infobuf_len_total,
                       txn->ctx->max_xfer);
    if (qbi_util_buf_alloc_dont_clear(&pkt_buf, pkt_buf.size) == NULL)
    {
      QBI_LOG_E_1("Couldn't allocate buffer (size %d) for event!",
                  pkt_buf.size);
    }
    else
    {
      infobuf_bytes_sent = qbi_msg_send_event_initial(txn, dev_svc_uuid,
                                                      pkt_buf.data,
                                                      pkt_buf.size);

      if (infobuf_bytes_sent < txn->infobuf_len_total)
      {
        qbi_msg_send_cmd_frags(txn, infobuf_bytes_sent, pkt_buf.data,
                               pkt_buf.size);
      }

      qbi_util_buf_free(&pkt_buf);
    }
  }

  qbi_txn_free(txn);
} /* qbi_msg_send_event() */

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
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->ctx);

  if ((FALSE == txn->ctx->ssr_info.ssr_in_progress) &&
      (FALSE == txn->ctx->mbim_recovery))
  {
    qbi_msg_send_open_done(txn->ctx, txn->status, txn->txn_id);
  }
  /* This is an SSR triggered MBIM open, hence no Open done message needs to be sent to OS.*/
  else
  {
    if (TRUE == txn->ctx->mbim_recovery)
    {
      if (QBI_MBIM_STATUS_SUCCESS == txn->status)
      {
        txn->ctx->state = QBI_CTX_STATE_OPENED;
        qbi_qmi_internal_pre_mbim_open_notifications(txn->ctx);
      }
    }
    qbi_qmi_internal_post_mbim_open_notifications(txn->ctx);
  }

  if (txn->status == QBI_MBIM_STATUS_SUCCESS)
  {
    txn->ctx->state = QBI_CTX_STATE_OPENED;
    QBI_LOG_I_1("Context ID %d now opened", txn->ctx->id);
    qbi_txn_free(txn);
  }
  else
  {
    QBI_LOG_E_2("Failure opening context ID %d: status %d",
                txn->ctx->id, txn->status);
    qbi_msg_context_close(txn->ctx);
  }
} /* qbi_msg_send_open_rsp() */

