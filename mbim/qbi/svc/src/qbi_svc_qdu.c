/*!
  @file
  qbi_svc_qdu.c

  @brief
  Qualcomm Device Update service: a generic CID interface for firmware update
*/

/*=============================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
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

#include "qbi_svc_qdu.h"
#include "qbi_svc_qdu_mbim.h"

#include "qbi_common.h"
#include "qbi_fwupd.h"
#include "qbi_mbim.h"
#include "qbi_svc.h"
#include "qbi_task.h"
#include "qbi_txn.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs

=============================================================================*/

typedef struct {
  qbi_fwupd_session_handle_s *handle;

  struct {
    uint32 current_session_type;
    uint32 current_session_status;
  } update_session;

  struct {
    uint32 last_txn_id;
    uint32 bytes_remaining;
  } file_write;
} qbi_svc_qdu_cache_s;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_qdu_cache_s *qbi_svc_qdu_cache_get
(
  const qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_qdu_close
(
  qbi_txn_s *txn
);

static uint32 qbi_svc_qdu_fwupd_file_result_to_mbim_status
(
  qbi_fwupd_file_result_e fwupd_file_result
);

static qbi_svc_action_e qbi_svc_qdu_open
(
  qbi_txn_s *txn
);

/*! @addtogroup MBIM_CID_QDU_UPDATE_SESSION
    @{ */

static void qbi_svc_qdu_update_session_e_check_status
(
  qbi_ctx_s *ctx
);

static void qbi_svc_qdu_update_session_e_check_status_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static qbi_svc_action_e qbi_svc_qdu_update_session_eqs_build_rsp_from_cache
(
  qbi_txn_s *txn
);

static uint32 qbi_svc_qdu_update_session_status_fwupd_to_mbim
(
  qbi_fwupd_session_status_e session_status_fwupd
);

static uint32 qbi_svc_qdu_update_session_type_fwupd_to_mbim
(
  qbi_fwupd_session_type_e session_type_fwupd
);

static qbi_fwupd_session_type_e qbi_svc_qdu_update_session_type_mbim_to_fwupd
(
  uint32 session_type_mbim
);

static qbi_svc_action_e qbi_svc_qdu_update_session_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_qdu_update_session_s_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_qdu_update_session_update_cache
(
  const qbi_ctx_s *ctx
);

/*! @} */

/*! @addtogroup MBIM_CID_QDU_FILE_OPEN
    @{ */

static qbi_fwupd_file_id_e qbi_svc_qdu_file_open_file_type_mbim_to_fwpud
(
  uint32 file_type_mbim
);

static qbi_svc_action_e qbi_svc_qdu_file_open_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_QDU_FILE_WRITE
    @{ */

static qbi_svc_action_e qbi_svc_qdu_file_write_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_qdu_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_qdu_cmd_hdlr_tbl[] = {
  /* MBIM_CID_QDU_UPDATE_SESSION */
  {qbi_svc_qdu_update_session_q_req, 0,
   qbi_svc_qdu_update_session_s_req, sizeof(qbi_svc_qdu_update_session_s_req_s)},
  /* MBIM_CID_QDU_FILE_OPEN */
  {NULL, 0, qbi_svc_qdu_file_open_s_req, sizeof(qbi_svc_qdu_file_open_s_req_s)},
  /* MBIM_CID_QDU_FILE_WRITE */
  {NULL, 0, qbi_svc_qdu_file_write_s_req, 0}
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_qdu_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the QDU device service's cache

    @details

    @param ctx

    @return qbi_svc_qdu_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
static qbi_svc_qdu_cache_s *qbi_svc_qdu_cache_get
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_qdu_cache_s *) qbi_svc_cache_get(ctx, QBI_SVC_ID_QDU);
} /* qbi_svc_bc_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_close
===========================================================================*/
/*!
    @brief Releases resources allocated by the MSFWID device service

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qdu_close
(
  qbi_txn_s *txn
)
{
  qbi_svc_qdu_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_qdu_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (cache->handle != NULL &&
      qbi_fwupd_session_status_get(cache->handle) !=
        QBI_FWUPD_SESSION_STATUS_REBOOTING)
  {
    qbi_fwupd_session_close(&cache->handle, "Aborted due to MBIM_CLOSE");
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_qdu_close() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_fwupd_file_result_to_mbim_status
===========================================================================*/
/*!
    @brief Maps an internal firmware update API file operation result enum
    to an MBIM CID status value

    @details

    @param fwupd_file_result

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_qdu_fwupd_file_result_to_mbim_status
(
  qbi_fwupd_file_result_e fwupd_file_result
)
{
  uint32 mbim_status;
/*-------------------------------------------------------------------------*/
  switch (fwupd_file_result)
  {
    case QBI_FWUPD_FILE_RESULT_SUCCESS:
      mbim_status = QBI_MBIM_STATUS_SUCCESS;
      break;

    case QBI_FWUPD_FILE_RESULT_FAILURE:
      mbim_status = QBI_MBIM_STATUS_FAILURE;
      break;

    case QBI_FWUPD_FILE_RESULT_INVALID_STATE:
      mbim_status = QBI_MBIM_STATUS_OPERATION_NOT_ALLOWED;
      break;

    case QBI_FWUPD_FILE_RESULT_UNSUPPORTED_FILE:
      mbim_status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
      break;

    case QBI_FWUPD_FILE_RESULT_NO_SPACE:
      mbim_status = QBI_MBIM_STATUS_MEMORY_FULL;
      break;

    case QBI_FWUPD_FILE_RESULT_WRITE_ERROR:
      mbim_status = QBI_MBIM_STATUS_WRITE_FAILURE;
      break;

    default:
      QBI_LOG_W_1("Unexpected file result %d", fwupd_file_result);
      mbim_status = QBI_MBIM_STATUS_FAILURE;
  }

  return mbim_status;
} /* qbi_svc_qdu_fwupd_file_result_to_mbim_status() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qdu_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (qbi_svc_cache_alloc(
        txn->ctx, QBI_SVC_ID_QDU, sizeof(qbi_svc_qdu_cache_s)) == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate cache!");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_qdu_open() */

/*! @addtogroup MBIM_CID_QDU_UPDATE_SESSION
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_update_session_e_check_status
===========================================================================*/
/*!
    @brief Posts a command to the QBI task to check the update session's
    status, perform any needed processing, and send an event to notify the
    host of any changes

    @details
    This is done via command callback in order to ensure that events get
    sent after relevant CID responses. For example, on the last FILE_WRITE,
    we want to notify the host of a change to SessionStatusApplyUpdate
    strictly after sending the successful CID response.

    @param ctx
*/
/*=========================================================================*/
static void qbi_svc_qdu_update_session_e_check_status
(
  qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  if (!qbi_task_cmd_send(
        ctx, QBI_TASK_CMD_ID_QDU_SESSION_CHECK,
        qbi_svc_qdu_update_session_e_check_status_cb, NULL))
  {
    QBI_LOG_E_0("Couldn't post command to check update session status");
  }
} /* qbi_svc_qdu_update_session_e_check_status() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_update_session_e_check_status_cb
===========================================================================*/
/*!
    @brief Checks the current session's status, performs any necessary
    internal processing to advance or clean up the session, and sends an
    MBIM_CID_QDU_UPDATE_SESSION event to notify the host of changes in the
    current session

    @details
    This is a QBI task command callback initiated by
    qbi_svc_qdu_update_session_e_check_status().

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_svc_qdu_update_session_e_check_status_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
  qbi_txn_s *txn;
  qbi_svc_action_e action;
  qbi_svc_qdu_cache_s *cache;
  qbi_fwupd_session_status_e status;
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(cmd_id);
  QBI_ARG_NOT_USED(data);

  cache = qbi_svc_qdu_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  /* Send an MBIM_CID_QDU_UPDATE_SESSION event if the current session changed */
  if (qbi_svc_qdu_update_session_update_cache(ctx))
  {
    txn = qbi_txn_alloc_event(
      ctx, QBI_SVC_ID_QDU, QBI_SVC_QDU_MBIM_CID_QDU_UPDATE_SESSION);
    if (txn == NULL)
    {
      QBI_LOG_E_0("Couldn't allocate update session event!");
    }
    else
    {
      action = qbi_svc_qdu_update_session_eqs_build_rsp_from_cache(txn);
      (void) qbi_svc_proc_action(txn, action);
    }
  }

  /* Check to see whether QDU needs to take internal action on the session */
  if (cache->handle != NULL)
  {
    status = qbi_fwupd_session_status_get(cache->handle);
    if (status == QBI_FWUPD_SESSION_STATUS_ERROR)
    {
      QBI_LOG_W_0("Closing firmware update session due to error");
      qbi_fwupd_session_close(&cache->handle, NULL);
    }
    else if (status == QBI_FWUPD_SESSION_STATUS_READY_TO_REBOOT)
    {
      QBI_LOG_I_0("Session is ready for reboot - continuing");
      if (!qbi_fwupd_session_continue(cache->handle))
      {
        QBI_LOG_E_0("Continuing session failed!");
        /* Check the status again to notify the host of failure and close
           the session */
        qbi_svc_qdu_update_session_e_check_status(ctx);
      }
    }
    else
    {
      QBI_LOG_I_1("Current internal firmware update session status is %d - no "
                  "internal action required", status);
    }
  }
} /* qbi_svc_qdu_update_session_e_check_status_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_update_session_eqs_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Allocates an MBIM_CID_QDU_UPDATE_SESSION response, and populates
    it with current session information from the cache and last session
    information from the internal firmware update layer

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qdu_update_session_eqs_build_rsp_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_qdu_cache_s *cache;
  qbi_svc_qdu_update_session_rsp_s *rsp;
  qbi_fwupd_session_last_result_s last_result;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_qdu_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_qdu_update_session_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->current_session_type   = cache->update_session.current_session_type;
  rsp->current_session_status = cache->update_session.current_session_status;

  QBI_MEMSET(&last_result, 0, sizeof(last_result));
  if (!qbi_fwupd_session_last_result_get(txn->ctx, cache->handle, &last_result))
  {
    /* Last session result is primarily used for debugging - if there was an
       error trying to get it, then don't fail the request */
    QBI_LOG_W_0("Couldn't retrieve last update session result - leaving empty");
  }
  else
  {
    rsp->last_session_type = qbi_svc_qdu_update_session_type_fwupd_to_mbim(
      last_result.session_type);
    rsp->last_session_result = (last_result.failed) ?
      QBI_SVC_QDU_SESSION_RESULT_FAILURE : QBI_SVC_QDU_SESSION_RESULT_SUCCESS;
    if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          txn, &rsp->last_session_error, 0,
          QBI_SVC_QDU_LAST_SESSION_ERROR_MAX_LEN_BYTES,
          last_result.error_msg, sizeof(last_result.error_msg)))
    {
      QBI_LOG_E_0("Couldn't add last session error string to response!");
    }
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_qdu_update_session_eqs_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_update_session_status_fwupd_to_mbim
===========================================================================*/
/*!
    @brief Maps an internal firmware update session status enum to the MBIM
    CID version shared with the host

    @details

    @param session_status_fwupd

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_qdu_update_session_status_fwupd_to_mbim
(
  qbi_fwupd_session_status_e session_status_fwupd
)
{
  uint32 session_status_mbim;
/*-------------------------------------------------------------------------*/
  switch (session_status_fwupd)
  {
    case QBI_FWUPD_SESSION_STATUS_NONE:
    case QBI_FWUPD_SESSION_STATUS_ERROR:
      session_status_mbim = QBI_SVC_QDU_SESSION_STATUS_INACTIVE;
      break;

    case QBI_FWUPD_SESSION_STATUS_WAITING_FOR_FILE:
    case QBI_FWUPD_SESSION_STATUS_WRITING_FILE:
      session_status_mbim = QBI_SVC_QDU_SESSION_STATUS_FILE_TRANSFER;
      break;

    case QBI_FWUPD_SESSION_STATUS_READY_TO_REBOOT:
    case QBI_FWUPD_SESSION_STATUS_REBOOTING:
      session_status_mbim = QBI_SVC_QDU_SESSION_STATUS_APPLY_UPDATE;
      break;

    default:
      QBI_LOG_E_1("Unexpected fwupd session status %d", session_status_fwupd);
      session_status_mbim = QBI_SVC_QDU_SESSION_STATUS_INACTIVE;
  }

  return session_status_mbim;
} /* qbi_svc_qdu_update_session_status_fwupd_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_update_session_type_fwupd_to_mbim
===========================================================================*/
/*!
    @brief Maps an internal firmware update session type enum to the MBIM
    CID version shared with the host

    @details

    @param session_type_fwupd

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_qdu_update_session_type_fwupd_to_mbim
(
  qbi_fwupd_session_type_e session_type_fwupd
)
{
  uint32 session_type_mbim;
/*-------------------------------------------------------------------------*/
  switch (session_type_fwupd)
  {
    case QBI_FWUPD_SESSION_TYPE_NONE:
      session_type_mbim = QBI_SVC_QDU_SESSION_TYPE_NONE;
      break;

    case QBI_FWUPD_SESSION_TYPE_LE:
      session_type_mbim = QBI_SVC_QDU_SESSION_TYPE_LE;
      break;

    default:
      QBI_LOG_E_1("Unexpected fwupd session type %d", session_type_fwupd);
      session_type_mbim = QBI_SVC_QDU_SESSION_TYPE_NONE;
  }

  return session_type_mbim;
} /* qbi_svc_qdu_update_session_type_fwupd_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_update_session_type_mbim_to_fwupd
===========================================================================*/
/*!
    @brief Maps an MBIM session type value from the host to the internal
    firmware update session type enum

    @details

    @param session_type_mbim

    @return qbi_fwupd_session_type_e
*/
/*=========================================================================*/
static qbi_fwupd_session_type_e qbi_svc_qdu_update_session_type_mbim_to_fwupd
(
  uint32 session_type_mbim
)
{
  qbi_fwupd_session_type_e session_type_fwupd;
/*-------------------------------------------------------------------------*/
  switch (session_type_mbim)
  {
    case QBI_SVC_QDU_SESSION_TYPE_NONE:
      session_type_fwupd = QBI_FWUPD_SESSION_TYPE_NONE;
      break;

    case QBI_SVC_QDU_SESSION_TYPE_LE:
      session_type_fwupd = QBI_FWUPD_SESSION_TYPE_LE;
      break;

    default:
      QBI_LOG_E_1("Unsupported MBIM session type %d", session_type_mbim);
      session_type_fwupd = QBI_FWUPD_SESSION_TYPE_NONE;
  }

  return session_type_fwupd;
} /* qbi_svc_qdu_update_session_type_mbim_to_fwupd() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_update_session_q_req
===========================================================================*/
/*!
    @brief Handles an MBIM_CID_QDU_UPDATE_SESSION query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qdu_update_session_q_req
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  (void) qbi_svc_qdu_update_session_update_cache(txn->ctx);
  return qbi_svc_qdu_update_session_eqs_build_rsp_from_cache(txn);
} /* qbi_svc_qdu_update_session_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_update_session_s_req
===========================================================================*/
/*!
    @brief Handles an MBIM_CID_UPDATE_SESSION set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qdu_update_session_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_qdu_cache_s *cache;
  qbi_svc_qdu_update_session_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  cache = qbi_svc_qdu_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  req = (qbi_svc_qdu_update_session_s_req_s *) txn->req.data;
  if (req->session_action != QBI_SVC_QDU_SESSION_ACTION_START)
  {
    QBI_LOG_E_1("Host requested invalid session action %d",
                req->session_action);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else if (!qbi_fwupd_session_type_supported(
             qbi_svc_qdu_update_session_type_mbim_to_fwupd(req->session_type)))
  {
    QBI_LOG_E_1("Host requested to start new session with invalid type %d",
                req->session_type);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else
  {
    /* If there is an active session, we need to close it first */
    if (cache->update_session.current_session_status !=
          QBI_SVC_QDU_SESSION_STATUS_INACTIVE)
    {
      QBI_LOG_I_0("Ending current update session to start new one");
      qbi_fwupd_session_close(&cache->handle, "Aborted to start new session");
    }

    if (!qbi_fwupd_session_open(
          txn->ctx,
          qbi_svc_qdu_update_session_type_mbim_to_fwupd(req->session_type),
          &cache->handle))
    {
      QBI_LOG_E_0("Couldn't open new session");
    }
    else
    {
      action = qbi_svc_qdu_update_session_q_req(txn);
    }
  }

  return action;
} /* qbi_svc_qdu_update_session_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_update_session_update_cache
===========================================================================*/
/*!
    @brief Updates the cache with the details of the current session

    @details

    @param ctx

    @return boolean TRUE if a value in the cache changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_qdu_update_session_update_cache
(
  const qbi_ctx_s *ctx
)
{
  uint32 new_session_type;
  uint32 new_session_status;
  qbi_svc_qdu_cache_s *cache;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_qdu_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  new_session_type = qbi_svc_qdu_update_session_type_fwupd_to_mbim(
    qbi_fwupd_session_type_get(cache->handle));
  new_session_status = qbi_svc_qdu_update_session_status_fwupd_to_mbim(
    qbi_fwupd_session_status_get(cache->handle));

  if ((new_session_type == QBI_SVC_QDU_SESSION_TYPE_NONE &&
       new_session_status != QBI_SVC_QDU_SESSION_STATUS_INACTIVE) ||
      (new_session_type != QBI_SVC_QDU_SESSION_TYPE_NONE &&
       new_session_status == QBI_SVC_QDU_SESSION_STATUS_INACTIVE))
  {
    /* This is possible if the session is in an error state but QDU has not
       closed the session yet - the session type will be valid but the status
       will be mapped to inactive. Ensure that we always report session type
       none and session status inactive together as a pair. */
    QBI_LOG_W_2("Mismatched session type %d and status %d",
                new_session_type, new_session_status);
    new_session_type = QBI_SVC_QDU_SESSION_TYPE_NONE;
    new_session_status = QBI_SVC_QDU_SESSION_STATUS_INACTIVE;
  }

  if (new_session_type != cache->update_session.current_session_type)
  {
    QBI_LOG_I_2("CurrentSessionType changed from %d to %d",
                cache->update_session.current_session_type, new_session_type);
    cache->update_session.current_session_type = new_session_type;
    changed = TRUE;
  }

  if (new_session_status != cache->update_session.current_session_status)
  {
    QBI_LOG_I_2("CurrentSessionStatus changed from %d to %d",
                cache->update_session.current_session_status,
                new_session_status);
    cache->update_session.current_session_status = new_session_status;
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_qdu_update_session_update_cache() */

/*! @} */

/*! @addtogroup MBIM_CID_QDU_FILE_OPEN
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_file_open_file_type_mbim_to_fwpud
===========================================================================*/
/*!
    @brief Maps an MBIM file type value to its matching internal file ID enum

    @details

    @param file_type_mbim

    @return qbi_fwupd_file_id_e
*/
/*=========================================================================*/
static qbi_fwupd_file_id_e qbi_svc_qdu_file_open_file_type_mbim_to_fwpud
(
  uint32 file_type_mbim
)
{
  qbi_fwupd_file_id_e file_id_fwupd;
/*-------------------------------------------------------------------------*/
  switch (file_type_mbim)
  {
    case QBI_SVC_QDU_FILE_TYPE_LE_PACKAGE:
      file_id_fwupd = QBI_FWUPD_FILE_ID_LE_PACKAGE;
      break;

    default:
      QBI_LOG_E_1("Unsupported MBIM file type %d", file_type_mbim);
      file_id_fwupd = QBI_FWUPD_FILE_ID_INVALID;
  }

  return file_id_fwupd;
} /* qbi_svc_qdu_file_open_file_type_mbim_to_fwpud() */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_file_open_s_req
===========================================================================*/
/*!
    @brief Handles an MBIM_CID_QDU_FILE_OPEN set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qdu_file_open_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_qdu_cache_s *cache;
  qbi_fwupd_file_id_e file_id;
  qbi_fwupd_file_result_e result;
  qbi_fwupd_file_transfer_params_s xfer_params;
  qbi_svc_qdu_file_open_s_req_s *req;
  qbi_svc_qdu_file_open_s_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  cache = qbi_svc_qdu_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  req = (qbi_svc_qdu_file_open_s_req_s *) txn->req.data;
  if (cache->handle == NULL)
  {
    QBI_LOG_E_0("Host tried to open file when there is no active session");
    txn->status = QBI_MBIM_STATUS_OPERATION_NOT_ALLOWED;
  }
  else if (req->file_size == 0)
  {
    QBI_LOG_E_0("Host tried to open file without specifying length");
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    qbi_fwupd_session_close(
      &cache->handle, "Open request contained invalid length");
    qbi_svc_qdu_update_session_e_check_status(txn->ctx);
  }
  else
  {
    rsp = (qbi_svc_qdu_file_open_s_rsp_s *) qbi_txn_alloc_rsp_buf(
      txn, sizeof(qbi_svc_qdu_file_open_s_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    file_id = qbi_svc_qdu_file_open_file_type_mbim_to_fwpud(req->file_type);
    result = qbi_fwupd_file_open(
      cache->handle, file_id, req->file_size, &xfer_params);
    if (result != QBI_FWUPD_FILE_RESULT_SUCCESS)
    {
      QBI_LOG_E_2("Attempt to open file ID %d failed with result %d",
                  file_id, result);
      txn->status = qbi_svc_qdu_fwupd_file_result_to_mbim_status(result);
      qbi_svc_qdu_update_session_e_check_status(txn->ctx);
    }
    else
    {
      cache->file_write.last_txn_id = txn->txn_id;
      cache->file_write.bytes_remaining = req->file_size;

      rsp->max_transfer_size = xfer_params.max_write_size;
      rsp->max_window_size   = xfer_params.max_window_size;

      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }

  return action;
} /* qbi_svc_qdu_file_open_s_req() */

/*! @} */

/*! @addtogroup MBIM_CID_QDU_FILE_WRITE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_qdu_file_write_s_req
===========================================================================*/
/*!
    @brief Handles an MBIM_CID_QDU_FILE_WRITE set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qdu_file_write_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_qdu_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_fwupd_file_result_e result;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  cache = qbi_svc_qdu_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (cache->handle == NULL)
  {
    QBI_LOG_E_0("Host tried to write to file when there is no active session");
    txn->status = QBI_MBIM_STATUS_OPERATION_NOT_ALLOWED;
  }
  else if (txn->txn_id <= cache->file_write.last_txn_id)
  {
    QBI_LOG_E_2("Received file data out of sequence! Current txn_id %d last %d",
                txn->txn_id, cache->file_write.last_txn_id);
    txn->status = QBI_SVC_QDU_MBIM_STATUS_DATA_OUT_OF_SEQUENCE;
    qbi_fwupd_session_close(
      &cache->handle, "File data received out of sequence");
    qbi_svc_qdu_update_session_e_check_status(txn->ctx);
  }
  else if (txn->req.size > cache->file_write.bytes_remaining)
  {
    QBI_LOG_E_2("Host tried to write too much data! Received %d bytes, %d "
                "remaining", txn->req.size, cache->file_write.bytes_remaining);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    qbi_fwupd_session_close(
      &cache->handle, "Received more data than expected");
    qbi_svc_qdu_update_session_e_check_status(txn->ctx);
  }
  else
  {
    result = qbi_fwupd_file_write(cache->handle, txn->req.data, txn->req.size);
    if (result != QBI_FWUPD_FILE_RESULT_SUCCESS)
    {
      QBI_LOG_E_1("Attempt to write %d bytes to file failed", txn->req.size);
      txn->status = qbi_svc_qdu_fwupd_file_result_to_mbim_status(result);
      qbi_svc_qdu_update_session_e_check_status(txn->ctx);
    }
    else
    {
      cache->file_write.last_txn_id = txn->txn_id;
      cache->file_write.bytes_remaining -= txn->req.size;
      if (cache->file_write.bytes_remaining == 0)
      {
        QBI_LOG_I_0("Received all expected file data - closing file");
        qbi_fwupd_file_close(cache->handle);
        qbi_svc_qdu_update_session_e_check_status(txn->ctx);
      }
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }

  return action;
} /* qbi_svc_qdu_file_write_s_req() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_qdu_init
===========================================================================*/
/*!
    @brief One-time initialization of the QDU device service

    @details
*/
/*=========================================================================*/
void qbi_svc_qdu_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_qdu_cfg = {
    {
      0x64, 0x27, 0x01, 0x5f, 0x57, 0x9d, 0x48, 0xf5,
      0x8c, 0x54, 0xf4, 0x3e, 0xd1, 0xe7, 0x6f, 0x83
    },
    QBI_SVC_ID_QDU,
    TRUE,
    qbi_svc_qdu_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_qdu_cmd_hdlr_tbl),
    qbi_svc_qdu_open,
    qbi_svc_qdu_close
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_qdu_cfg);
} /* qbi_svc_qdu_init() */

