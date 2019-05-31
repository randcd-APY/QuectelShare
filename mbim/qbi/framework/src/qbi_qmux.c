/*!
  @file
  qbi_qmux.c

  @brief
  Provides an interface to send and receive raw QMUX messages.
*/

/*=============================================================================

  Copyright (c) 2012, 2015, 2016 Qualcomm Technologies, Inc.
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
04/17/12  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_qmux.h"

#include "qbi_common.h"
#include "qbi_qmi.h"
#include "qbi_task.h"
#include "qbi_util.h"
#include "qbi_qmux_smd.h"

/*=============================================================================

  Private Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs and Constants

=============================================================================*/


/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static void qbi_qmux_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static void qbi_qmux_invoke_rx_from_modem_cb
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
);

static void qbi_qmux_rx_cb
(
  unsigned char *rx_msg,
  int            rx_msg_len
);

/*=============================================================================

  Private Variables

=============================================================================*/

/*! Reference count for qbi_qmux_qmi_handle */
static uint32 qbi_qmux_qmi_handle_ref_cnt = 0;

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_qmux_cmd_cb
===========================================================================*/
/*!
    @brief Processes QMUX data received from the modem in QBI task context.

    @details

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_qmux_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
  qbi_util_buf_s  *buf;
  qbi_qmux_msg_s  *qmux_msg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(data);
  QBI_ARG_NOT_USED(cmd_id);

  buf = (qbi_util_buf_s *) data;
  qmux_msg = (qbi_qmux_msg_s *) buf->data;

  if (qmux_msg->qmux_hdr.svc_type == QBI_QMUX_SVC_TYPE_QMI_CTL &&
      buf->size < QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES)
  {
    QBI_LOG_E_2("Received short QMI payload: %d bytes (minimum %d)",
                buf->size, QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES);
  }
  else if (qmux_msg->qmux_hdr.svc_type != QBI_QMUX_SVC_TYPE_QMI_CTL &&
           buf->size < QBI_QMUX_NON_QMI_CTL_MIN_MSG_LEN_BYTES)
  {
    QBI_LOG_E_2("Received short regular QMUX payload: %d bytes (minimum %d)",
                buf->size, QBI_QMUX_NON_QMI_CTL_MIN_MSG_LEN_BYTES);
  }
  else
  {
    ctx = qbi_task_get_ctx_by_id(QBI_CTX_ID_PRIMARY);

    #ifdef QBI_UNIT_TEST
    if (ctx != NULL && ctx->id == QBI_CTX_ID_PRIMARY &&
        ctx->state != QBI_CTX_STATE_OPENED)
    {
      ctx = qbi_task_get_ctx_by_id(QBI_CTX_ID_UNIT_TEST);
    }
    #endif /* QBI_UNIT_TEST */

    if (ctx == NULL)
    {
      QBI_LOG_W_0("Couldn't find context for QMI message");
    }
    else
    {
      qbi_qmux_invoke_rx_from_modem_cb(ctx, buf);
      return;
    }
  }

  /* Message is freed since it was not forwarded to registered callback */
  qbi_util_buf_free(buf);
  QBI_MEM_FREE(buf);
} /* qbi_qmux_cmd_cb() */

/*===========================================================================
  FUNCTION: qbi_qmux_invoke_rx_from_modem_cb
===========================================================================*/
/*!
    @brief Sends RX QMUX message to the callback registered in qbi_qmux_open

    @details

    @param ctx
    @param buf
*/
/*=========================================================================*/
static void qbi_qmux_invoke_rx_from_modem_cb
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
)
{
  qbi_qmux_rx_from_modem_cb_f *qmux_rx_cb = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(ctx->qmux_state);
  QBI_CHECK_NULL_PTR_RET(buf);

  qbi_os_fptr_decode(&ctx->qmux_state->rx_from_modem_cb,
                     (qbi_os_void_f **) &qmux_rx_cb, TRUE);
  QBI_CHECK_NULL_PTR_RET(qmux_rx_cb);

  qmux_rx_cb(ctx, buf);
} /* qbi_qmux_invoke_rx_from_modem_cb() */

/*===========================================================================
  FUNCTION: qbi_qmux_rx_cb
===========================================================================*/
/*!
    @brief Called in RX QMUX data thread context to pass data to QBI task.

    @details
    Creates a new QBI task command to process this data in the QBI task's
    context.
    QMUX data is copied to allocated buffer that is passed to QBI task.

    @param rx_msg
    @param rx_msg_len

    @note This function executes outside the QBI task
*/
/*=========================================================================*/
static void qbi_qmux_rx_cb
(
  unsigned char *rx_msg,
  int            rx_msg_len
)
{
  qbi_util_buf_s *buf;
/*-------------------------------------------------------------------------*/
  QBI_LOG_D_1("Received QMUX data buffer with size %d", rx_msg_len);

  buf = QBI_MEM_MALLOC_CLEAR(sizeof(*buf));
  QBI_CHECK_NULL_PTR_RET(buf);

  qbi_util_buf_init(buf);
  if (qbi_util_buf_alloc_dont_clear(buf, rx_msg_len) == NULL)
  {
    QBI_LOG_E_1("Couldn't allocate buffer of size %d", rx_msg_len);
  }
  else
  {
    QBI_MEMSCPY(buf->data, buf->size, rx_msg, rx_msg_len);
    if (!qbi_task_cmd_send(
          NULL, QBI_TASK_CMD_ID_QMUX_DATA, qbi_qmux_cmd_cb, buf))
    {
      QBI_LOG_E_0("Couldn't send command");
      qbi_util_buf_free(buf);
      QBI_MEM_FREE(buf);
    }
  }
} /* qbi_qmux_rx_cb() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_qmux_close
===========================================================================*/
/*!
    @brief Releases resources previously allocated for the raw QMUX
    connection in qbi_qmux_open

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_qmux_close
(
  qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(ctx->qmux_state);

  QBI_MEMSET(ctx->qmux_state, 0, sizeof(qbi_qmux_state_s));
  QBI_MEM_FREE(ctx->qmux_state);
  ctx->qmux_state = NULL;
  QBI_LOG_I_1("Closed raw QMUX connection on context ID %d", ctx->id);

  if (qbi_qmux_qmi_handle_ref_cnt > 0 &&
      --qbi_qmux_qmi_handle_ref_cnt == 0)
  {
    QBI_LOG_I_0("Last QBI bypass disabled: closing QMI QMUX SMD");
    qbi_qmux_smd_close();
  }
} /* qbi_qmux_close() */

/*===========================================================================
  FUNCTION: qbi_qmux_is_qmi_ctl_request
===========================================================================*/
/*!
    @brief Checks whether a data packet matches the signature of a QMI_CTL
    request

    @details
    The intent of this function is to allow the host communications layer to
    check the first encapsulated command to see whether the remote driver is
    MBIM-based or QMUX-based.

    @param data
    @param len

    @return boolean TRUE if the packet looks like a QMI_CTL request, FALSE
    otherwise
*/
/*=========================================================================*/
boolean qbi_qmux_is_qmi_ctl_request
(
  const void *data,
  uint32      len
)
{
  qbi_qmux_msg_s *qmux_msg;
  boolean is_qmi_ctl;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(data);

  qmux_msg = (qbi_qmux_msg_s *) data;
  if (len >= QBI_QMUX_MIN_MSG_LEN_BYTES &&
      qmux_msg->if_type == QBI_QMUX_IF_TYPE_QMUX &&
      qmux_msg->qmux_hdr.length == (len - sizeof(qbi_qmux_if_type_t)) &&
      qmux_msg->qmux_hdr.ctl_flags == QBI_QMUX_CTL_FLAG_SENDER_CONTROL_POINT &&
      qmux_msg->qmux_hdr.svc_type == QBI_QMUX_SVC_TYPE_QMI_CTL &&
      qmux_msg->qmux_hdr.client_id == QBI_QMUX_CLIENT_ID_QMI_CTL &&
      (qmux_msg->sdu.qmi_ctl.hdr.svc_ctl_flags &
       QBI_QMUX_SVC_CTL_FLAG_MASK_MSG_TYPE) ==
        QBI_QMUX_SVC_CTL_FLAG_MSG_TYPE_REQUEST)
  {
    QBI_LOG_I_0("Packet matches QMI_CTL request signature");
    is_qmi_ctl = TRUE;
  }
  else
  {
    QBI_LOG_I_0("Packet does not mach QMI_CTL request signature");
    is_qmi_ctl = FALSE;
  }

  return is_qmi_ctl;
} /* qbi_qmux_is_qmi_ctl_request() */

/*===========================================================================
  FUNCTION: qbi_qmux_open
===========================================================================*/
/*!
    @brief Initializes the raw QMI connection

    @details
    This function may either be called by the EXT_QMUX device service as a
    part of MBIM_OPEN processing, or by the host communications layer when
    switching to bypass mode.

    @param ctx
    @param rx_from_modem_cb

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_qmux_open
(
  qbi_ctx_s                   *ctx,
  qbi_qmux_rx_from_modem_cb_f *rx_from_modem_cb
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(rx_from_modem_cb);

  if (ctx->qmux_state != NULL)
  {
    QBI_LOG_E_1("Raw QMUX connection already opened on context ID %d!",
                ctx->id);
  }
  else
  {
    ctx->qmux_state = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_qmux_state_s));
    QBI_CHECK_NULL_PTR_RET_FALSE(ctx->qmux_state);

    qbi_os_fptr_encode(&ctx->qmux_state->rx_from_modem_cb,
                       (qbi_os_void_f *) rx_from_modem_cb);
    if (++qbi_qmux_qmi_handle_ref_cnt == 1)
    {
      QBI_LOG_I_0("QMUX opened on first QBI context: initializing handle");
      if (!qbi_qmux_smd_init(qbi_qmux_rx_cb))
      {
        QBI_LOG_E_0("Error initializing QMUX SMD");
      }
      else
      {
        QBI_LOG_I_1("Enabled QMUX SMD path on context ID %d", ctx->id);
        success = TRUE;
      }
    }
  }

  return success;
} /* qbi_qmux_open() */

/*===========================================================================
  FUNCTION: qbi_qmux_tx_to_modem
===========================================================================*/
/*!
    @brief Sends a raw QMUX message to the modem

    @details
    If this function returns TRUE, then the request was successfully
    transmitted to the modem.

    @param ctx
    @param data
    @param len

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_qmux_tx_to_modem
(
  qbi_ctx_s      *ctx,
  qbi_qmux_msg_s *qmux_msg,
  uint32          qmux_msg_len
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmux_msg);

  if (qbi_qmux_qmi_handle_ref_cnt == 0)
  {
    QBI_LOG_E_0("Tried to send QMUX message to modem, but handle ref_cnt == 0");
  }
  else if (qmux_msg_len < QBI_QMUX_MIN_MSG_LEN_BYTES)
  {
    QBI_LOG_E_2("Tried sending short QMUX message to the modem! Got %d bytes, "
                "min %d", qmux_msg_len, QBI_QMUX_MIN_MSG_LEN_BYTES);
  }
  else
  {
    success = qbi_qmux_smd_write_msg((unsigned char *)qmux_msg,
                                     qmux_msg_len);
  }

  return success;
} /* qbi_qmux_tx_to_modem() */

