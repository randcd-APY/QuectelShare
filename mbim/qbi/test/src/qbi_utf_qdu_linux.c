/*!
  @file
  qbi_utf_qdu_linux.c

  @brief
  QBI Unit Test Framework - Qualcomm Device Update (QDU) Linux platform tests
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
01/22/13  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_msg_mbim.h"
#include "qbi_txn.h"
#include "qbi_utf.h"
#include "qbi_svc_qdu_mbim.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs

=============================================================================*/

typedef struct {
  int fd;

  uint32 file_size;
  uint32 max_transfer_size;
  uint32 max_window_size;

  uint32 bytes_sent;
  uint32 pending_txns;
} qbi_utf_qdu_linux_info_s;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static void qbi_utf_qdu_linux_cleanup
(
  qbi_utf_ctx_s *utf_ctx,
  boolean        aborted
);

static qbi_utf_action_e qbi_utf_qdu_linux_file_open_s_req
(
  qbi_utf_ctx_s *utf_ctx
);

static qbi_utf_action_e qbi_utf_qdu_linux_file_open_s_rsp
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
);

static qbi_utf_action_e qbi_utf_qdu_linux_file_write_s_req
(
  qbi_utf_ctx_s *utf_ctx
);

static qbi_utf_action_e qbi_utf_qdu_linux_file_write_s_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
);

static qbi_utf_action_e qbi_utf_qdu_linux_update_session_e_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
);

static void qbi_utf_qdu_linux_update_session_print_rsp
(
  const qbi_svc_qdu_update_session_rsp_s *rsp
);

static qbi_utf_action_e qbi_utf_qdu_linux_update_session_q_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
);

static qbi_utf_action_e qbi_utf_qdu_linux_update_session_s_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
);

static qbi_utf_action_e qbi_utf_qdu_linux_start
(
  qbi_utf_ctx_s *utf_ctx
);

/*=============================================================================

  Private Variables

=============================================================================*/

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*! @addtogroup UTF: qdu_linux
    @{ */

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_cleanup
===========================================================================*/
/*!
    @brief Performs cleanup of resources allocated during the test suite

    @details

    @param utf_ctx
    @param aborted
*/
/*=========================================================================*/
static void qbi_utf_qdu_linux_cleanup
(
  qbi_utf_ctx_s *utf_ctx,
  boolean        aborted
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);

  QBI_LOG_I_0("QDU Linux cleanup fcn called");
  if (utf_ctx->info != NULL)
  {
    QBI_MEM_FREE(utf_ctx->info);
    utf_ctx->info = NULL;
  }
} /* qbi_utf_qdu_linux_cleanup() */

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_file_open_s_req
===========================================================================*/
/*!
    @brief Opens the local update package file and issues a FILE_OPEN set
    request

    @details

    @param utf_ctx

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_qdu_linux_file_open_s_req
(
  qbi_utf_ctx_s *utf_ctx
)
{
  qbi_txn_s *txn;
  qbi_utf_qdu_linux_info_s *info;
  qbi_utf_action_e action = QBI_UTF_ACTION_ABORT;
  qbi_svc_qdu_file_open_s_req_s *req;
  struct stat statbuf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx->info);

  QBI_MEMSET(&statbuf, 0, sizeof(statbuf));
  info = (qbi_utf_qdu_linux_info_s *) utf_ctx->info;
  info->fd = open("/usr/update.zip", O_RDONLY);
  if (info->fd == -1)
  {
    QBI_LOG_E_1("Couldn't open update package file: %d", errno);
    QBI_LOG_STR_1("%s", strerror(errno));
  }
  else if (fstat(info->fd, &statbuf) == -1)
  {
    QBI_LOG_E_1("Couldn't stat update package file: %d", errno);
    QBI_LOG_STR_1("%s", strerror(errno));
  }
  else
  {
    QBI_LOG_D_1("Sending FILE_OPEN request for file size %d", statbuf.st_size);
    info->file_size = statbuf.st_size;
    txn = qbi_utf_txn_alloc(
      utf_ctx, QBI_SVC_ID_QDU, QBI_SVC_QDU_MBIM_CID_QDU_FILE_OPEN,
      QBI_MSG_CMD_TYPE_SET, sizeof(qbi_svc_qdu_file_open_s_req_s),
      qbi_utf_qdu_linux_file_open_s_rsp);
    QBI_CHECK_NULL_PTR_RET_UTF_ABORT(txn);

    req = txn->req.data;
    req->file_type = QBI_SVC_QDU_FILE_TYPE_LE_PACKAGE;
    req->file_size = info->file_size;

    action = QBI_UTF_ACTION_SEND_REQ;
  }

  return action;
} /* qbi_utf_qdu_linux_file_open_s_req() */

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_file_open_s_rsp
===========================================================================*/
/*!
    @brief Processes a FILE_OPEN set response

    @details

    @param utf_ctx
    @param txn

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_qdu_linux_file_open_s_rsp
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
)
{
  qbi_utf_qdu_linux_info_s *info;
  qbi_svc_qdu_file_open_s_rsp_s *rsp;
  qbi_utf_action_e action = QBI_UTF_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx->info);

  if (!qbi_utf_txn_check_status(txn, sizeof(qbi_svc_qdu_file_open_s_rsp_s)))
  {
    QBI_LOG_E_0("FILE_OPEN set request failed");
  }
  else
  {
    rsp = (qbi_svc_qdu_file_open_s_rsp_s *) txn->rsp.data;
    info = (qbi_utf_qdu_linux_info_s *) utf_ctx->info;

    QBI_LOG_D_0("Received response to FILE_OPEN set request");
    QBI_LOG_D_2("MaxTransferSize: %d MaxWindowSize: %d",
                rsp->max_transfer_size, rsp->max_window_size);
    info->max_transfer_size = rsp->max_transfer_size;
    info->max_window_size = rsp->max_window_size;
    info->pending_txns = 0;
    action = qbi_utf_qdu_linux_file_write_s_req(utf_ctx);
  }

  return action;
} /* qbi_utf_qdu_linux_file_open_s_rsp() */

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_file_write_s_req
===========================================================================*/
/*!
    @brief Sends one or more FILE_WRITE set requests

    @details

    @param utf_ctx

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_qdu_linux_file_write_s_req
(
  qbi_utf_ctx_s *utf_ctx
)
{
  qbi_txn_s *txn;
  uint32 bytes_to_send;
  qbi_utf_qdu_linux_info_s *info;
  ssize_t bytes_read, total_bytes_read;
  qbi_utf_action_e action = QBI_UTF_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx->info);

  info = (qbi_utf_qdu_linux_info_s *) utf_ctx->info;
  while (info->pending_txns < info->max_window_size)
  {
    bytes_to_send = info->max_transfer_size;
    if ((info->file_size - info->bytes_sent) == 0)
    {
      QBI_LOG_D_0("Sent entire file");
      if (action != QBI_UTF_ACTION_SEND_REQ)
      {
        action = QBI_UTF_ACTION_WAIT;
      }
      break;
    }
    else if ((info->file_size - info->bytes_sent) < info->max_transfer_size)
    {
      bytes_to_send = (info->file_size - info->bytes_sent);
    }

    txn = qbi_utf_txn_alloc(
      utf_ctx, QBI_SVC_ID_QDU, QBI_SVC_QDU_MBIM_CID_QDU_FILE_WRITE,
      QBI_MSG_CMD_TYPE_SET, bytes_to_send, qbi_utf_qdu_linux_file_write_s_rsp_cb);
    if (txn == NULL)
    {
      QBI_LOG_E_0("Couldn't allocate transaction to send request!");
      break;
    }

    total_bytes_read = 0;
    do
    {
      bytes_read = read(info->fd, ((uint8 *) txn->req.data + total_bytes_read),
                        (bytes_to_send - total_bytes_read));
    } while (bytes_read > 0 && (total_bytes_read += bytes_read) < bytes_to_send);

    if (bytes_read <= 0)
    {
      QBI_LOG_E_3("Error trying to read %d bytes from file with %d/%d bytes "
                  "read", (bytes_to_send - total_bytes_read), total_bytes_read,
                  bytes_to_send);
      QBI_LOG_STR_3("Return value: %d Errno: %d '%s'", bytes_read, errno, strerror(errno));
      action = QBI_UTF_ACTION_ABORT;
      break;
    }

    info->bytes_sent += bytes_to_send;
    info->pending_txns++;
    action = QBI_UTF_ACTION_SEND_REQ;
  }

  return action;
} /* qbi_utf_qdu_linux_file_write_s_req() */

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_file_write_s_rsp_cb
===========================================================================*/
/*!
    @brief Processes a FILE_WRITE set response

    @details

    @param utf_ctx
    @param txn

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_qdu_linux_file_write_s_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
)
{
  qbi_utf_action_e action = QBI_UTF_ACTION_ABORT;
  qbi_utf_qdu_linux_info_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx->info);

  info = (qbi_utf_qdu_linux_info_s *) utf_ctx->info;
  if (!qbi_utf_txn_check_status(txn, 0))
  {
    QBI_LOG_E_0("FILE_WRITE failed");
    action = QBI_UTF_ACTION_WAIT;
  }
  else
  {
    if (info->pending_txns == 0)
    {
      QBI_LOG_E_0("Unexpected FILE_WRITE response when no transactions pending!");
    }
    else
    {
      info->pending_txns--;
      action = qbi_utf_qdu_linux_file_write_s_req(utf_ctx);
    }
  }

  return action;
} /* qbi_utf_qdu_linux_file_write_s_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_update_session_e_cb
===========================================================================*/
/*!
    @brief Processes an UPDATE_SESSION event

    @details

    @param utf_ctx
    @param txn

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_qdu_linux_update_session_e_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
)
{
  qbi_svc_qdu_update_session_rsp_s *rsp;
  qbi_utf_action_e action = QBI_UTF_ACTION_WAIT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(txn->rsp.data);

  QBI_LOG_D_0("Received UPDATE_SESSION event");
  rsp = (qbi_svc_qdu_update_session_rsp_s *) txn->rsp.data;
  qbi_utf_qdu_linux_update_session_print_rsp(rsp);

  if (rsp->current_session_type == QBI_SVC_QDU_SESSION_TYPE_NONE &&
      rsp->current_session_status == QBI_SVC_QDU_SESSION_STATUS_INACTIVE &&
      rsp->last_session_type == QBI_SVC_QDU_SESSION_TYPE_LE &&
      rsp->last_session_result == QBI_SVC_QDU_SESSION_RESULT_FAILURE)
  {
    QBI_LOG_E_0("Update session failed! Aborting...");
    action = QBI_UTF_ACTION_ABORT;
  }
  else if (rsp->current_session_type == QBI_SVC_QDU_SESSION_TYPE_LE &&
           rsp->current_session_status == QBI_SVC_QDU_SESSION_STATUS_APPLY_UPDATE)
  {
    QBI_LOG_I_0("Session status transitioned to ApplyUpdate");
  }

  return action;
} /* qbi_utf_qdu_linux_update_session_e_cb() */

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_update_session_print_rsp
===========================================================================*/
/*!
    @brief Prints the contents of the InformationBuffer in an UPDATE_SESSION
    response/event

    @details

    @param rsp
*/
/*=========================================================================*/
static void qbi_utf_qdu_linux_update_session_print_rsp
(
  const qbi_svc_qdu_update_session_rsp_s *rsp
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(rsp);

  QBI_LOG_D_2("CurrentSessionType: %d CurrentSessionStatus: %d",
              rsp->current_session_type, rsp->current_session_status);
  QBI_LOG_D_2("LastSessionType: %d LastSessionResult: %d",
              rsp->last_session_type, rsp->last_session_result);
} /* qbi_utf_qdu_linux_update_session_print_rsp() */

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_update_session_q_rsp_cb
===========================================================================*/
/*!
    @brief Processes the response to UPDATE_SESSION query

    @details

    @param utf_ctx
    @param txn

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_qdu_linux_update_session_q_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
)
{
  qbi_utf_action_e action = QBI_UTF_ACTION_ABORT;
  qbi_svc_qdu_update_session_rsp_s *rsp;
  qbi_txn_s *txn_req;
  qbi_svc_qdu_update_session_s_req_s *req;
/*-------------------------------------------------------------------------*/
  if (!qbi_utf_txn_check_status(txn, sizeof(qbi_svc_qdu_update_session_rsp_s)))
  {
    QBI_LOG_E_0("UPDATE_SESSION set request failed");
  }
  else
  {
    QBI_LOG_D_0("Received response to UPDATE_SESSION query");
    rsp = (qbi_svc_qdu_update_session_rsp_s *) txn->rsp.data;
    qbi_utf_qdu_linux_update_session_print_rsp(rsp);
    if (rsp->last_session_type != QBI_SVC_QDU_SESSION_TYPE_NONE)
    {
      /* If last session type is populated, we already did this test, so we are
         done. */
      action = QBI_UTF_ACTION_FINISH;
    }
    else
    {
      /* Send the request to start the update session */
      QBI_LOG_D_0("Sending UPDATE_SESSION set request");
      txn_req = qbi_utf_txn_alloc(
        utf_ctx, QBI_SVC_ID_QDU, QBI_SVC_QDU_MBIM_CID_QDU_UPDATE_SESSION,
        QBI_MSG_CMD_TYPE_SET, sizeof(qbi_svc_qdu_update_session_s_req_s),
        qbi_utf_qdu_linux_update_session_s_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_UTF_ABORT(txn_req);

      req = (qbi_svc_qdu_update_session_s_req_s *) txn_req->req.data;
      req->session_action = QBI_SVC_QDU_SESSION_ACTION_START;
      req->session_type = QBI_SVC_QDU_SESSION_TYPE_LE;

      action = QBI_UTF_ACTION_SEND_REQ;
    }
  }

  return action;
} /* qbi_utf_qdu_linux_update_session_q_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_update_session_s_rsp_cb
===========================================================================*/
/*!
    @brief Processes the response to UPDATE_SESSION set

    @details

    @param utf_ctx
    @param txn

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_qdu_linux_update_session_s_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
)
{
  qbi_utf_action_e action = QBI_UTF_ACTION_ABORT;
  qbi_svc_qdu_update_session_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  if (!qbi_utf_txn_check_status(txn, sizeof(qbi_svc_qdu_update_session_rsp_s)))
  {
    QBI_LOG_E_0("UPDATE_SESSION set request failed");
  }
  else
  {
    QBI_LOG_D_0("Received response to UPDATE_SESSION set request");
    rsp = (qbi_svc_qdu_update_session_rsp_s *) txn->rsp.data;
    qbi_utf_qdu_linux_update_session_print_rsp(rsp);
    if (rsp->current_session_type != QBI_SVC_QDU_SESSION_TYPE_LE ||
        rsp->current_session_status != QBI_SVC_QDU_SESSION_STATUS_FILE_TRANSFER)
    {
      QBI_LOG_E_0("Invalid current session type/status");
    }
    else if (!qbi_utf_txn_event_handler_register(
               utf_ctx, QBI_SVC_ID_QDU, QBI_SVC_QDU_MBIM_CID_QDU_UPDATE_SESSION,
               qbi_utf_qdu_linux_update_session_e_cb))
    {
      QBI_LOG_E_0("Couldn't add event handler for update session status "
                  "changes!");
    }
    else
    {
      action = qbi_utf_qdu_linux_file_open_s_req(utf_ctx);
    }
  }

  return action;
} /* qbi_utf_qdu_linux_update_session_s_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_start
===========================================================================*/
/*!
    @brief Begins running the QDU Linux test suite

    @details
    This test suite contains a positive QDU scenario using this flow:
      1. Query UPDATE_SESSION: if last session info available, then abort
      2. UPDATE_SESSION set to start new session, register for UPDATE_SESSION
         events
      3. FILE_OPEN set to prepare transfer. The unit test code reads from
         /usr/update.zip
      4. FILE_WRITE to send all data from the update package
      5. Listen for UPDATE_SESSION event notifying that the update will be
         applied
      6. Device will reboot into recovery mode to apply the update, at next
         boot UTF will continue at step 1 and see that last session info is
         available

    @param utf_ctx

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_qdu_linux_start
(
  qbi_utf_ctx_s *utf_ctx
)
{
  qbi_txn_s *txn;
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Start of QDU Linux test suite: querying update session info");

  utf_ctx->info = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_utf_qdu_linux_info_s));
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx->info);

  txn = qbi_utf_txn_alloc(
    utf_ctx, QBI_SVC_ID_QDU, QBI_SVC_QDU_MBIM_CID_QDU_UPDATE_SESSION,
    QBI_MSG_CMD_TYPE_QUERY, 0, qbi_utf_qdu_linux_update_session_q_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(txn);

  return QBI_UTF_ACTION_SEND_REQ;
} /* qbi_utf_qdu_linux_start() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_utf_qdu_linux_init
===========================================================================*/
/*!
    @brief Registers the QDU Linux unit tests

    @details
*/
/*=========================================================================*/
void qbi_utf_qdu_linux_init
(
  void
)
{
  static const qbi_utf_ts_s ts = {
    "qdu_linux",
    QBI_CTX_ID_UNIT_TEST,
    QBI_UTF_SETUP_BASIC,
    QBI_UTF_TEARDOWN_CLOSE,
    qbi_utf_qdu_linux_start,
    qbi_utf_qdu_linux_cleanup
  };
/*-------------------------------------------------------------------------*/
  qbi_utf_ts_register(&ts);
} /* qbi_utf_qdu_linux_init() */

