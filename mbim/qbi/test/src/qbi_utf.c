/*!
  @file
  qbi_utf.c

  @brief
  QBI Unit Test Framework
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

#include "qbi_utf.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_msg.h"
#include "qbi_msg_mbim.h"
#include "qbi_os.h"
#include "qbi_svc.h"
#include "qbi_task.h"
#include "qbi_txn.h"
#include "qbi_util.h"

#include "qbi_svc_bc_mbim.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! Maximum number of test suites that can be registered. This value must be
    increased as necessary to make room for future test suites. */
#define QBI_UTF_TS_MAX (32)

/*! Maximum length of a test suite name */
#define QBI_UTF_TS_NAME_MAX (128)

/*! Maximum CID transfer size negotiated in MBIM_OPEN */
#define QBI_UTF_MAX_XFER (4096)

/*=============================================================================

  Private Typedefs

=============================================================================*/

typedef enum {
  QBI_UTF_INTERNAL_STATE_NOT_INIT = 0,
  QBI_UTF_INTERNAL_STATE_TS_SETUP,
  QBI_UTF_INTERNAL_STATE_TS_EXECUTION,
  QBI_UTF_INTERNAL_STATE_TS_TEARDOWN,
  QBI_UTF_INTERNAL_STATE_COMPLETE,
  QBI_UTF_INTERNAL_STATE_ABORTED
} qbi_utf_internal_state_e;

typedef struct qbi_utf_ctx_internal_struct {
  qbi_utf_internal_state_e state;

  /*! Array containing test suite names to execute, in order */
  const char *tests_to_run[QBI_UTF_TS_MAX];

  /*! Next index to use in the tests_to_run array */
  uint32 next_ts_index;

  /*! Contains the last CID transaction ID sent to QBI */
  uint32 cid_txn_id;

  /*! Optional handler to process raw CID data */
  qbi_utf_cid_raw_handler_f *raw_cid_handler;

  /*! Set to TRUE when we have started dispatching transactions, reset to false
      when all transactions have been dispatched. Used to ensure that we don't
      build up a lot of duplicate dispatch commands. */
  boolean dispatching;
} qbi_utf_ctx_internal_s;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

/* List of test suite initialization functions */
extern void qbi_utf_shared_init(void);

#ifdef FEATURE_LINUX
extern void qbi_utf_qdu_linux_init(void);
#endif /* FEATURE_LINUX */

static void qbi_utf_action_proc
(
  qbi_utf_ctx_s   *utf_ctx,
  qbi_utf_action_e action
);

static const qbi_utf_ts_s *qbi_utf_ts_find_by_name
(
  const char *name
);

static void qbi_utf_ts_start_next
(
  qbi_utf_ctx_s *utf_ctx
);

static void qbi_utf_ts_setup
(
  qbi_utf_ctx_s *utf_ctx
);

static qbi_utf_action_e qbi_utf_setup_invoke_start
(
  qbi_utf_ctx_s *utf_ctx
);

static void qbi_utf_setup_mbim_open_send_req
(
  qbi_utf_ctx_s *utf_ctx
);

static qbi_utf_action_e qbi_utf_setup_mbim_open_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  const void    *data,
  uint32         len
);

static qbi_utf_action_e qbi_utf_setup_ready_status_q_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
);

static void qbi_utf_ts_teardown
(
  qbi_utf_ctx_s *utf_ctx,
  boolean        aborted
);

static void qbi_utf_ts_teardown_mbim_close_send_req
(
  qbi_utf_ctx_s *utf_ctx
);

static qbi_utf_action_e qbi_utf_ts_teardown_mbim_close_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  const void    *data,
  uint32         len
);

static void qbi_utf_txn_dispatch
(
  qbi_utf_ctx_s *utf_ctx
);

static void qbi_utf_txn_dispatch_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static boolean qbi_utf_txn_dispatch_pending
(
  qbi_utf_ctx_s *utf_ctx
);

static void qbi_utf_txn_event_handler_unregister_all
(
  qbi_utf_ctx_s *utf_ctx
);

static void qbi_utf_txn_free_all
(
  qbi_utf_ctx_s *utf_ctx
);

static void qbi_utf_txn_handle_event
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
);

static boolean qbi_utf_txn_handle_rsp
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
);

static void qbi_utf_txn_log_req
(
  const qbi_txn_s *txn
);

static void qbi_utf_txn_log_rsp
(
  const qbi_txn_s *txn
);

/*=============================================================================

  Private Variables

=============================================================================*/

static const qbi_utf_ts_s *qbi_utf_registered_test_suites[QBI_UTF_TS_MAX];

static qbi_utf_ctx_s qbi_utf_ctx;
static qbi_utf_ctx_internal_s qbi_utf_state;

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_utf_action_proc
===========================================================================*/
/*!
    @brief Processes an action returned by a test handler function

    @details

    @param utf_ctx
    @param action
*/
/*=========================================================================*/
static void qbi_utf_action_proc
(
  qbi_utf_ctx_s   *utf_ctx,
  qbi_utf_action_e action
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->ts);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->ts->name);

  QBI_LOG_STR_2("Processing action %d for test suite '%s'",
                action, utf_ctx->ts->name);
  switch (action)
  {
    case QBI_UTF_ACTION_ABORT:
      qbi_utf_ts_abort(utf_ctx);
      break;

    case QBI_UTF_ACTION_WAIT:
      break;

    case QBI_UTF_ACTION_SEND_REQ:
      qbi_utf_txn_dispatch(utf_ctx);
      break;

    case QBI_UTF_ACTION_FINISH:
      qbi_utf_ts_teardown(utf_ctx, FALSE);
      break;

    default:
      QBI_LOG_E_1("Received unknown action %d", action);
      qbi_utf_ts_abort(utf_ctx);
  }
} /* qbi_utf_action_proc() */

/*===========================================================================
  FUNCTION: qbi_utf_ts_find_by_name
===========================================================================*/
/*!
    @brief Finds a test suite based on its name

    @details

    @param name

    @return const qbi_utf_ts_s* Pointer to test suite, or NULL if no match
    found
*/
/*=========================================================================*/
static const qbi_utf_ts_s *qbi_utf_ts_find_by_name
(
  const char *name
)
{
  uint32 i;
  const qbi_utf_ts_s *ts;
/*-------------------------------------------------------------------------*/
  for (i = 0; i < QBI_UTF_TS_MAX; i++)
  {
    ts = qbi_utf_registered_test_suites[i];
    if (ts == NULL)
    {
      break;
    }
    else if (QBI_STRNCMP(ts->name, name, QBI_UTF_TS_NAME_MAX) == 0)
    {
      break;
    }
  }

  if (i >= QBI_UTF_TS_MAX)
  {
    ts = NULL;
  }

  return ts;
} /* qbi_utf_ts_find_by_name() */

/*===========================================================================
  FUNCTION: qbi_utf_ts_start_next
===========================================================================*/
/*!
    @brief Starts the next test suite in the list of test suites to run

    @details

    @param utf_ctx
*/
/*=========================================================================*/
static void qbi_utf_ts_start_next
(
  qbi_utf_ctx_s *utf_ctx
)
{
  const char *next_ts_name;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->internal);

  next_ts_name =
    utf_ctx->internal->tests_to_run[utf_ctx->internal->next_ts_index];
  if (next_ts_name == NULL)
  {
    QBI_LOG_W_0("UTF tests complete");
    /*! @todo Support running tests in a neverending loop */
  }
  else
  {
    utf_ctx->internal->next_ts_index++;
    utf_ctx->ts = qbi_utf_ts_find_by_name(next_ts_name);
    if (utf_ctx->ts == NULL)
    {
      QBI_LOG_E_1("Couldn't run test at index %d: not registered",
                  (utf_ctx->internal->next_ts_index - 1));
      QBI_LOG_STR_1("Missing test suite name is '%s'", next_ts_name);
      qbi_utf_ts_start_next(utf_ctx);
    }
    else
    {
      qbi_utf_ts_setup(utf_ctx);
    }
  }
} /* qbi_utf_ts_start_next() */

/*===========================================================================
  FUNCTION: qbi_utf_ts_setup
===========================================================================*/
/*!
    @brief Performs setup for the current test suite

    @details

    @param utf_ctx
*/
/*=========================================================================*/
static void qbi_utf_ts_setup
(
  qbi_utf_ctx_s *utf_ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->ts);

  utf_ctx->ctx = qbi_task_get_ctx_by_id(utf_ctx->ts->ctx_id);
  if (utf_ctx->ctx == NULL)
  {
    QBI_LOG_E_1("Couldn't find QBI context ID %d", utf_ctx->ts->ctx_id);
    qbi_utf_ts_abort(utf_ctx);
  }
  else
  {
    QBI_LOG_STR_2("Performing setup action %d for test suite '%s'",
                  utf_ctx->ts->setup, utf_ctx->ts->name);
    switch (utf_ctx->ts->setup)
    {
      case QBI_UTF_SETUP_NONE:
        qbi_utf_action_proc(utf_ctx, qbi_utf_setup_invoke_start(utf_ctx));
        break;

      case QBI_UTF_SETUP_OPEN:
      case QBI_UTF_SETUP_BASIC:
        qbi_utf_setup_mbim_open_send_req(utf_ctx);
        break;

      default:
        QBI_LOG_E_1("Unsupported setup type %d", utf_ctx->ts->setup);
        qbi_utf_ts_abort(utf_ctx);
    }
  }
} /* qbi_utf_ts_setup() */

/*===========================================================================
  FUNCTION: qbi_utf_setup_invoke_start
===========================================================================*/
/*!
    @brief Invokes the test suite's start function

    @details

    @param utf_ctx

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_setup_invoke_start
(
  qbi_utf_ctx_s *utf_ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx->ts);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx->ts->name);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx->ts->start_fcn);

  QBI_LOG_STR_1("Starting test suite '%s'", utf_ctx->ts->name);
  return utf_ctx->ts->start_fcn(utf_ctx);
} /* qbi_utf_setup_invoke_start() */

/*===========================================================================
  FUNCTION: qbi_utf_setup_mbim_open_send_req
===========================================================================*/
/*!
    @brief Builds and sends an MBIM_OPEN request

    @details

    @param utf_ctx
*/
/*=========================================================================*/
static void qbi_utf_setup_mbim_open_send_req
(
  qbi_utf_ctx_s *utf_ctx
)
{
  qbi_msg_open_req_s *open_req;
  qbi_util_buf_s buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->internal);

  qbi_util_buf_init(&buf);
  open_req = (qbi_msg_open_req_s *) qbi_util_buf_alloc(
    &buf, sizeof(qbi_msg_open_req_s));
  QBI_CHECK_NULL_PTR_RET(open_req);
  open_req->hdr.msg_type = QBI_MSG_MBIM_OPEN_MSG;
  open_req->hdr.txn_id = ++(utf_ctx->internal->cid_txn_id);
  open_req->hdr.msg_len = sizeof(qbi_msg_open_req_s);
  open_req->max_xfer = QBI_UTF_MAX_XFER;

  qbi_utf_cid_register_raw_handler(utf_ctx, qbi_utf_setup_mbim_open_rsp_cb);

  QBI_LOG_I_0("Sending MBIM_OPEN_MSG as part of setup");
  qbi_utf_cid_send_raw(utf_ctx, &buf);
  qbi_util_buf_free(&buf);
} /* qbi_utf_setup_mbim_open_send_req() */

/*===========================================================================
  FUNCTION: qbi_utf_setup_mbim_open_rsp_cb
===========================================================================*/
/*!
    @brief Raw CID handler that processes the response to the MBIM_OPEN
    request sent during the setup phase

    @details

    @param utf_ctx
    @param data
    @param len

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_setup_mbim_open_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  const void    *data,
  uint32         len
)
{
  qbi_utf_action_e action = QBI_UTF_ACTION_ABORT;
  const qbi_msg_open_rsp_s *open_rsp;
  qbi_txn_s *txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(data);

  /* Unregister the raw handler, as we expect exactly 1 response to our
     MBIM_OPEN request, which should be MBIM_OPEN_DONE. */
  qbi_utf_cid_register_raw_handler(utf_ctx, NULL);

  open_rsp = (const qbi_msg_open_rsp_s *) data;
  if (len != sizeof(qbi_msg_open_rsp_s))
  {
    QBI_LOG_E_2("Received unexpected response of size %d to open request; "
                "expected size %d", len, sizeof(qbi_msg_open_rsp_s));
  }
  else if (open_rsp->hdr.msg_type != QBI_MSG_MBIM_OPEN_DONE)
  {
    QBI_LOG_E_1("Received unexpected message type %d", open_rsp->hdr.msg_type);
  }
  else if (open_rsp->status != QBI_MBIM_STATUS_SUCCESS)
  {
    QBI_LOG_E_1("Open failed with status %d", open_rsp->status);
  }
  else
  {
    QBI_LOG_I_0("Received successful MBIM_OPEN_DONE");
    if (utf_ctx->ts->setup == QBI_UTF_SETUP_OPEN)
    {
      action = qbi_utf_setup_invoke_start(utf_ctx);
    }
    else if (utf_ctx->ts->setup == QBI_UTF_SETUP_BASIC)
    {
      QBI_LOG_I_0("Sending READY_STATUS query as part of setup");
      txn = qbi_utf_txn_alloc(
        utf_ctx, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS,
        QBI_MSG_CMD_TYPE_QUERY, 0, qbi_utf_setup_ready_status_q_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_UTF_ABORT(txn);
      action = QBI_UTF_ACTION_SEND_REQ;
    }
  }

  return action;
} /* qbi_utf_setup_mbim_open_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_utf_setup_ready_status_q_rsp_cb
===========================================================================*/
/*!
    @brief Response handler for MBIM_CID_SUBSCRIBER_READY_STATUS query sent
    during setup

    @details

    @param utf_ctx
    @param txn

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_setup_ready_status_q_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
)
{
  qbi_utf_action_e action = QBI_UTF_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(txn);

  if (txn->status != QBI_MBIM_STATUS_SUCCESS)
  {
    QBI_LOG_E_1("READY_STATUS query for test suite setup failed with status %d",
                txn->status);
  }
  else
  {
    action = qbi_utf_setup_invoke_start(utf_ctx);
  }

  return action;
} /* qbi_utf_setup_ready_status_q_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_utf_ts_teardown
===========================================================================*/
/*!
    @brief Performs teardown for the current test suite, then starts the
    next

    @details

    @param utf_ctx
*/
/*=========================================================================*/
static void qbi_utf_ts_teardown
(
  qbi_utf_ctx_s *utf_ctx,
  boolean        aborted
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->internal);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->ts);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->ts->name);

  QBI_LOG_STR_1("Test suite '%s' is complete", utf_ctx->ts->name);
  if (utf_ctx->ts->cleanup_fcn != NULL)
  {
    QBI_LOG_I_0("Invoking cleanup function");
    utf_ctx->ts->cleanup_fcn(utf_ctx, aborted);
  }

  /* Reset the UTF context in preparation for starting the next test suite */
  qbi_task_cmd_cancel(utf_ctx->ctx, QBI_TASK_CMD_ID_UTF_DISPATCH, NULL);
  utf_ctx->internal->dispatching = FALSE;
  utf_ctx->state = 0;
  utf_ctx->info  = NULL;
  qbi_utf_cid_register_raw_handler(utf_ctx, NULL);
  qbi_utf_txn_free_all(utf_ctx);
  qbi_utf_txn_event_handler_unregister_all(utf_ctx);

  QBI_LOG_I_1("Performing teardown action %d", utf_ctx->ts->teardown);
  switch (utf_ctx->ts->teardown)
  {
    case QBI_UTF_TEARDOWN_NONE:
      qbi_utf_ts_start_next(utf_ctx);
      break;

    case QBI_UTF_TEARDOWN_CLOSE:
      qbi_utf_ts_teardown_mbim_close_send_req(utf_ctx);
      break;

    default:
      QBI_LOG_E_1("Unexpected teardown type %d", utf_ctx->ts->teardown);
      qbi_utf_ts_start_next(utf_ctx);
  }
} /* qbi_utf_ts_teardown() */

/*===========================================================================
  FUNCTION: qbi_utf_ts_teardown_mbim_close_send_req
===========================================================================*/
/*!
    @brief Sends an MBIM_CLOSE request as part of test suite teardown

    @details

    @param utf_ctx
*/
/*=========================================================================*/
static void qbi_utf_ts_teardown_mbim_close_send_req
(
  qbi_utf_ctx_s *utf_ctx
)
{
  qbi_msg_close_req_s *close_req;
  qbi_util_buf_s buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->internal);

  qbi_util_buf_init(&buf);
  close_req = (qbi_msg_close_req_s *) qbi_util_buf_alloc(
    &buf, sizeof(qbi_msg_close_req_s));
  QBI_CHECK_NULL_PTR_RET(close_req);
  close_req->hdr.msg_type = QBI_MSG_MBIM_CLOSE_MSG;
  close_req->hdr.txn_id = ++(utf_ctx->internal->cid_txn_id);
  close_req->hdr.msg_len = sizeof(qbi_msg_close_req_s);

  qbi_utf_cid_register_raw_handler(utf_ctx, qbi_utf_ts_teardown_mbim_close_rsp_cb);

  QBI_LOG_I_0("Sending MBIM_CLOSE_MSG");
  qbi_utf_cid_send_raw(utf_ctx, &buf);
  qbi_util_buf_free(&buf);
} /* qbi_utf_ts_teardown_mbim_close_send_req() */

/*===========================================================================
  FUNCTION: qbi_utf_ts_teardown_mbim_close_rsp_cb
===========================================================================*/
/*!
    @brief Raw handler used to process MBIM_CLOSE_DONE as part of test suite
    teardown

    @details

    @param utf_ctx
    @param data
    @param len

    @return qbi_utf_action_e
*/
/*=========================================================================*/
static qbi_utf_action_e qbi_utf_ts_teardown_mbim_close_rsp_cb
(
  qbi_utf_ctx_s *utf_ctx,
  const void    *data,
  uint32         len
)
{
  const qbi_msg_close_rsp_s *close_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(utf_ctx);
  QBI_CHECK_NULL_PTR_RET_UTF_ABORT(data);

  /* Unregister the raw handler, as we expect exactly 1 response to our
     MBIM_CLOSE request, which should be MBIM_CLOSE_DONE. */
  qbi_utf_cid_register_raw_handler(utf_ctx, NULL);

  close_rsp = (const qbi_msg_close_rsp_s *) data;
  if (len != sizeof(qbi_msg_close_rsp_s))
  {
    QBI_LOG_E_2("Received unexpected response of size %d to close request; "
                "expected size %d", len, sizeof(qbi_msg_close_rsp_s));
  }
  else if (close_rsp->hdr.msg_type != QBI_MSG_MBIM_CLOSE_DONE)
  {
    QBI_LOG_E_1("Received unexpected message type %d", close_rsp->hdr.msg_type);
  }
  else if (close_rsp->status != QBI_MBIM_STATUS_SUCCESS)
  {
    QBI_LOG_E_1("MBIM_CLOSE failed with status %d", close_rsp->status);
  }
  else
  {
    qbi_utf_ts_start_next(utf_ctx);
  }

  return QBI_UTF_ACTION_WAIT;
} /* qbi_utf_ts_teardown_mbim_close_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_dispatch
===========================================================================*/
/*!
    @brief Dispatches queued CID requests

    @details
    Rather than sending the requests right away, this function post a
    command to the QBI task to do it. This is closer to how real CIDs are
    handled, and helps keep the stack/heap from growing unnaturally due to
    the way unit test transactions are injected. Note that even though this
    posts a command to the QBI task, it is not safe to be called from other
    threads.

    @param utf_ctx
*/
/*=========================================================================*/
static void qbi_utf_txn_dispatch
(
  qbi_utf_ctx_s *utf_ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->internal);

  if (!utf_ctx->internal->dispatching)
  {
    if (!qbi_task_cmd_send(
          utf_ctx->ctx, QBI_TASK_CMD_ID_UTF_DISPATCH,
          qbi_utf_txn_dispatch_cmd_cb, utf_ctx))
    {
      QBI_LOG_E_0("Couldn't post command to dispatch UTF requests!");
    }
    else
    {
      utf_ctx->internal->dispatching = TRUE;
    }
  }
  else
  {
    QBI_LOG_W_0("Ignoring dispatch request since already processing...");
  }
} /* qbi_utf_txn_dispatch() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_dispatch_cmd_cb
===========================================================================*/
/*!
    @brief Handles the QBI task command callback to send queued requests to
    the QBI framework for processing

    @details
    This function should not be called directly. Instead, call
    qbi_utf_txn_dispatch().

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_utf_txn_dispatch_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
  qbi_utf_ctx_s *utf_ctx;
  qbi_util_list_iter_s iter;
  qbi_utf_txn_list_entry_s *txn_list_entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(data);

  utf_ctx = (qbi_utf_ctx_s *) data;
  QBI_CHECK_NULL_PTR_RET(utf_ctx->internal);

  qbi_util_list_iter_init(&utf_ctx->txn_list, &iter);
  while ((txn_list_entry = (qbi_utf_txn_list_entry_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (txn_list_entry->state == QBI_UTF_TXN_STATE_NOT_DISPATCHED &&
        txn_list_entry->txn != NULL)
    {
      txn_list_entry->state = QBI_UTF_TXN_STATE_WAITING_RSP;
      qbi_utf_txn_log_req(txn_list_entry->txn);
      qbi_svc_dispatch(txn_list_entry->txn);
      break;
    }
  }

  /* We only dispatch one CID at a time - if there are more CIDs waiting,
     post another command to the QBI task to dispatch it later. Note that in the
     current implementation, continuing to use the iterator from above will miss
     new transactions added to the list via synchronous processing of the
     dispatched transaction. */
  utf_ctx->internal->dispatching = FALSE;
  if (qbi_utf_txn_dispatch_pending(utf_ctx))
  {
    qbi_utf_txn_dispatch(utf_ctx);
  }
} /* qbi_utf_txn_dispatch_cmd_cb() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_dispatch_pending
===========================================================================*/
/*!
    @brief Checks whether there is one or more transactions pending dispatch

    @details

    @param utf_ctx

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_utf_txn_dispatch_pending
(
  qbi_utf_ctx_s *utf_ctx
)
{
  boolean txn_pending_dispatch = FALSE;
  qbi_util_list_iter_s iter;
  const qbi_utf_txn_list_entry_s *txn_list_entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(utf_ctx);

  qbi_util_list_iter_init(&utf_ctx->txn_list, &iter);
  while ((txn_list_entry = (const qbi_utf_txn_list_entry_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (txn_list_entry->state == QBI_UTF_TXN_STATE_NOT_DISPATCHED &&
        txn_list_entry->txn != NULL)
    {
      txn_pending_dispatch = TRUE;
      break;
    }
  }

  return txn_pending_dispatch;
} /* qbi_utf_txn_dispatch_pending() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_event_handler_unregister_all
===========================================================================*/
/*!
    @brief Unregisters all CID event handlers

    @details

    @param utf_ctx
*/
/*=========================================================================*/
static void qbi_utf_txn_event_handler_unregister_all
(
  qbi_utf_ctx_s *utf_ctx
)
{
  qbi_util_list_iter_s iter;
  qbi_utf_txn_event_handler_s *handler;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);

  qbi_util_list_iter_init(&utf_ctx->event_handler_list, &iter);
  while ((handler = (qbi_utf_txn_event_handler_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    qbi_util_list_iter_remove(&iter);
    QBI_MEMSET(handler, 0, sizeof(qbi_utf_txn_event_handler_s));
    QBI_MEM_FREE(handler);
  }
} /* qbi_utf_txn_event_handler_unregister_all() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_free_all
===========================================================================*/
/*!
    @brief Frees all QBI transactions allocated by the unit test framework.
    Note that this means that normal QBI processing of the CID will also be
    aborted.

    @details

    @param utf_ctx
*/
/*=========================================================================*/
static void qbi_utf_txn_free_all
(
  qbi_utf_ctx_s *utf_ctx
)
{
  qbi_util_list_iter_s iter;
  qbi_utf_txn_list_entry_s *txn_list_entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);

  qbi_util_list_iter_init(&utf_ctx->txn_list, &iter);
  while ((txn_list_entry = (qbi_utf_txn_list_entry_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    QBI_LOG_W_1("Freeing entry for txn in state %d due to test suite cleanup",
                txn_list_entry->state);
    qbi_util_list_iter_remove(&iter);
    QBI_MEMSET(txn_list_entry, 0, sizeof(qbi_utf_txn_list_entry_s));
    QBI_MEM_FREE(txn_list_entry);
  }
} /* qbi_utf_txn_free_all() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_handle_event
===========================================================================*/
/*!
    @brief Invokes any registered CID event handlers for the given transaction

    @details

    @param utf_ctx
    @param txn
*/
/*=========================================================================*/
static void qbi_utf_txn_handle_event
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
)
{
  qbi_util_list_iter_s iter;
  const qbi_utf_txn_event_handler_s *handler;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);

  qbi_util_list_iter_init(&utf_ctx->event_handler_list, &iter);
  qbi_util_list_iter_reg(&iter);
  while ((handler = (const qbi_utf_txn_event_handler_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (txn->svc_id == handler->svc_id && txn->cid == handler->cid)
    {
      QBI_CHECK_NULL_PTR_RET(handler->event_cb);
      qbi_utf_action_proc(utf_ctx, handler->event_cb(utf_ctx, txn));
    }
  }
  qbi_util_list_iter_dereg(&iter);
} /* qbi_utf_txn_handle_event() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_handle_rsp
===========================================================================*/
/*!
    @brief Determines whether the transaction was initiated by the unit test
    framework, and if so, invokes the response callback then frees the
    transaction's memory

    @details

    @param utf_ctx
    @param txn

    @return boolean TRUE if the transaction was processed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_utf_txn_handle_rsp
(
  qbi_utf_ctx_s *utf_ctx,
  qbi_txn_s     *txn
)
{
  boolean processed = FALSE;
  qbi_util_list_iter_s iter;
  qbi_utf_txn_list_entry_s *txn_list_entry;
  qbi_utf_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(utf_ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(utf_ctx->internal);

  qbi_util_list_iter_init(&utf_ctx->txn_list, &iter);
  while ((txn_list_entry = (qbi_utf_txn_list_entry_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (txn_list_entry->txn == txn &&
        txn_list_entry->state == QBI_UTF_TXN_STATE_WAITING_RSP)
    {
      QBI_LOG_I_4("Received response to UTF request: svc_id %d cid %d cmd_type "
                  "%d txn iid %d", txn->svc_id, txn->cid, txn->cmd_type,
                  txn->iid);
      QBI_CHECK_NULL_PTR_RET_FALSE(txn_list_entry->rsp_cb);
      txn_list_entry->state = QBI_UTF_TXN_STATE_RSP_RCVD;
      qbi_utf_txn_log_rsp(txn);
      action = txn_list_entry->rsp_cb(utf_ctx, txn);

      /* Free the transaction now that we are done processing it, but before we
         hand the action off to the rest of the framework. This ordering is
         required to avoid a double free if processing the action ends up
         freeing all transactions as part of cleanup. */
      qbi_util_list_iter_remove(&iter);
      qbi_txn_free(txn);
      QBI_MEMSET(txn_list_entry, 0, sizeof(qbi_utf_txn_list_entry_s));
      QBI_MEM_FREE(txn_list_entry);

      qbi_utf_action_proc(utf_ctx, action);
      processed = TRUE;
      break;
    }
  }

  return processed;
} /* qbi_utf_txn_handle_rsp() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_log_req
===========================================================================*/
/*!
    @brief Logs the raw CID format of a QBI transaction

    @details
    Since transactions are injected directly into the qbi_svc layer, they
    do not appear in regular CID logging. This function creates the raw
    CID format from the transaction and logs it for the purpose of debugging.
    Supports CID query/set requests. Records the entire InformationBuffer
    as a single CID (no fragmentation).

    @param txn
*/
/*=========================================================================*/
static void qbi_utf_txn_log_req
(
  const qbi_txn_s *txn
)
{
  qbi_util_buf_s buf;
  qbi_msg_cmd_req_s *cid_req;
  const uint8 *uuid;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->ctx);

  uuid = qbi_svc_svc_id_to_uuid(txn->svc_id);
  QBI_CHECK_NULL_PTR_RET(uuid);

  qbi_util_buf_init(&buf);
  cid_req = (qbi_msg_cmd_req_s *) qbi_util_buf_alloc(
    &buf, (sizeof(qbi_msg_cmd_req_s) + txn->req.size));
  QBI_CHECK_NULL_PTR_RET(cid_req);

  cid_req->hdr.msg_type = QBI_MSG_MBIM_COMMAND_MSG;
  cid_req->hdr.msg_len  = buf.size;
  cid_req->hdr.txn_id   = txn->txn_id;
  cid_req->total_frag   = 1;
  cid_req->cur_frag     = 0;
  cid_req->cid          = txn->cid;
  cid_req->cmd_type     = txn->cmd_type;
  cid_req->infobuf_len  = txn->req.size;

  QBI_MEMSCPY(cid_req->dev_svc_uuid, sizeof(cid_req->dev_svc_uuid),
              uuid, QBI_MBIM_UUID_LEN);
  if (txn->req.size > 0)
  {
    QBI_MEMSCPY(((uint8 *) buf.data + sizeof(qbi_msg_cmd_req_s)),
                (buf.size - sizeof(qbi_msg_cmd_req_s)),
                txn->req.data, txn->req.size);
  }

  qbi_log_pkt(txn->ctx->id, QBI_LOG_PKT_DIRECTION_RX, buf.data, buf.size);
  qbi_util_buf_free(&buf);
} /* qbi_utf_txn_log_req() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_log_rsp
===========================================================================*/
/*!
    @brief Logs the raw CID format of a QBI transaction

    @details
    Since transactions are injected directly into the qbi_svc layer, they
    do not appear in regular CID logging. This function creates the raw
    CID format from the transaction and logs it for the purpose of debugging.
    Supports CID responses and events.

    @param txn
*/
/*=========================================================================*/
static void qbi_utf_txn_log_rsp
(
  const qbi_txn_s *txn
)
{
  qbi_util_buf_s buf;
  qbi_msg_cmd_rsp_s *cid_rsp;
  const uint8 *uuid;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->ctx);

  uuid = qbi_svc_svc_id_to_uuid(txn->svc_id);
  QBI_CHECK_NULL_PTR_RET(uuid);

  qbi_util_buf_init(&buf);
  cid_rsp = (qbi_msg_cmd_rsp_s *) qbi_util_buf_alloc(
    &buf, (sizeof(qbi_msg_cmd_rsp_s) + txn->infobuf_len_total));
  QBI_CHECK_NULL_PTR_RET(cid_rsp);

  if (txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT)
  {
    cid_rsp->hdr.msg_type = QBI_MSG_MBIM_INDICATE_STATUS_MSG;
    cid_rsp->hdr.txn_id   = 0;
  }
  else
  {
    cid_rsp->hdr.msg_type = QBI_MSG_MBIM_COMMAND_DONE;
    cid_rsp->hdr.txn_id   = txn->txn_id;
  }
  cid_rsp->hdr.msg_len  = buf.size;
  cid_rsp->total_frag   = 1;
  cid_rsp->cur_frag     = 0;
  cid_rsp->cid          = txn->cid;
  cid_rsp->status       = txn->status;
  cid_rsp->infobuf_len  = txn->infobuf_len_total;

  QBI_MEMSCPY(cid_rsp->dev_svc_uuid, sizeof(cid_rsp->dev_svc_uuid),
              uuid, QBI_MBIM_UUID_LEN);
  if (txn->infobuf_len_total > 0)
  {
    /* We are removing the const qualifier from txn, but txn will not be changed
       in qbi_txn_rsp_databuf_extract(). Note that we can't change the param to
       const in qbi_txn because list iterators must be used (and list iterators
       can't use a const because they have the ability to modify the list). */
    (void) qbi_txn_rsp_databuf_extract(
      (qbi_txn_s *) txn, ((uint8 *) buf.data + sizeof(qbi_msg_cmd_rsp_s)),
      txn->infobuf_len_total, 0);
  }

  qbi_log_pkt(txn->ctx->id, QBI_LOG_PKT_DIRECTION_TX, buf.data, buf.size);
  qbi_util_buf_free(&buf);
} /* qbi_utf_txn_log_rsp() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_utf_cid_receive_raw
===========================================================================*/
/*!
    @brief Processes a CID sent from the messaging layer

    @details

    @param ctx
    @param data
    @param len
*/
/*=========================================================================*/
void qbi_utf_cid_receive_raw
(
  qbi_ctx_s  *ctx,
  const void *data,
  uint32      len
)
{
  qbi_utf_ctx_s *utf_ctx;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(data);

  qbi_log_pkt(ctx->id, QBI_LOG_PKT_DIRECTION_TX, data, len);

  /*! @note Should replace this with a lookup if more than one UTF context is
      used. */
  utf_ctx = &qbi_utf_ctx;
  if (utf_ctx != NULL && utf_ctx->ctx == ctx)
  {
    if (utf_ctx->internal != NULL && utf_ctx->internal->raw_cid_handler != NULL)
    {
      qbi_utf_action_proc(
        utf_ctx, utf_ctx->internal->raw_cid_handler(utf_ctx, data, len));
    }
    else
    {
      QBI_LOG_I_0("Ignoring raw CID data; no handler registered");
    }
  }
  else
  {
    QBI_LOG_I_0("Ignoring raw data received on unexpected context");
  }
} /* qbi_utf_cid_receive_raw() */

/*===========================================================================
  FUNCTION: qbi_utf_cid_register_raw_handler
===========================================================================*/
/*!
    @brief Registers a function to handle all raw CID data received from QBI

    @details
    The raw handler will receive all data sent by QBI (including
    MBIM_OPEN_RSP, etc) that makes it to the QBI host communications layer.
    This means it will not include data from QBI transactions intercepted
    at the QBI messaging layer, so CIDs will only be handled once.

    @param utf_ctx
    @param raw_handler_fcn

    @note This currently only supports receiving raw data on the unit test
    context ID
*/
/*=========================================================================*/
void qbi_utf_cid_register_raw_handler
(
  qbi_utf_ctx_s             *utf_ctx,
  qbi_utf_cid_raw_handler_f *raw_handler_fcn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->internal);

  /*! @todo Support raw handlers on the primary context by modifying qbi_hc &
      qbi_utf to pass CIDs to UTF first, then allow UTF to decide how to
      proceed with the data. */
  utf_ctx->internal->raw_cid_handler = raw_handler_fcn;
} /* qbi_utf_cid_register_raw_handler() */

/*===========================================================================
  FUNCTION: qbi_utf_cid_send_raw
===========================================================================*/
/*!
    @brief Logs and transmits raw CID data to the QBI messaging layer

    @details
    Does not free the buffer after sending it. The request is directly
    injected into the QBI framework, rather than posting to a command queue.

    @param utf_ctx
    @param buf
*/
/*=========================================================================*/
void qbi_utf_cid_send_raw
(
  qbi_utf_ctx_s  *utf_ctx,
  qbi_util_buf_s *buf
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->ctx);
  QBI_CHECK_NULL_PTR_RET(buf);

  qbi_log_pkt(utf_ctx->ctx->id, QBI_LOG_PKT_DIRECTION_RX, buf->data, buf->size);
  qbi_msg_input(utf_ctx->ctx, (qbi_util_buf_const_s *) buf);
} /* qbi_utf_cid_send_raw() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_receive
===========================================================================*/
/*!
    @brief Checks whether a QBI transaction was initiated by the unit test
    framework, and if so, passes it to the response callback

    @details
    Called by the QBI service layer before attempting to send a CID to the
    host.

    @param txn

    @return boolean TRUE if the transaction was processed and freed by the
    unit test framework, FALSE otherwise.
*/
/*=========================================================================*/
boolean qbi_utf_txn_receive
(
  qbi_txn_s *txn
)
{
  qbi_utf_ctx_s *utf_ctx;
  boolean captured = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  /*! @note Placeholder for future lookup if multiple UTF contexts are used */
  utf_ctx = &qbi_utf_ctx;
  if (txn->ctx == utf_ctx->ctx)
  {
    if (txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT)
    {
      qbi_utf_txn_handle_event(utf_ctx, txn);

      /* Unlike responses, events are forwarded to the host even if they are
         handled by the unit test framework. The one exception is if handling
         the event triggered MBIM_CLOSE teardown, which is done synchronously
         and results in freeing the transaction, so the qbi_svc layer must
         ignore the transaction. */
      if (utf_ctx->ctx != NULL && utf_ctx->ctx->state == QBI_CTX_STATE_OPENED)
      {
        captured = FALSE;
      }
      else
      {
        captured = TRUE;
      }
    }
    else if (txn->cmd_type == QBI_MSG_CMD_TYPE_QUERY ||
             txn->cmd_type == QBI_MSG_CMD_TYPE_SET)
    {
      captured = qbi_utf_txn_handle_rsp(utf_ctx, txn);
    }
  }

  return captured;
} /* qbi_utf_txn_receive() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_alloc
===========================================================================*/
/*!
    @brief Allocates a QBI CID transaction structure to be used for sending
    unit test CIDs

    @details
    The core QBI framework transaction API is leveraged for the unit test
    framework - a transaction allocated here is injected directly into the
    qbi_svc layer for dispatch to QBI's CID handlers. A raw API is also
    available at qbi_utf_cid_send_raw().

    @param utf_ctx
    @param svc_id
    @param cid
    @param cmd_type
    @param req_data_len
    @param rsp_cb

    @return qbi_txn_s*
*/
/*=========================================================================*/
qbi_txn_s *qbi_utf_txn_alloc
(
  qbi_utf_ctx_s          *utf_ctx,
  qbi_svc_id_e            svc_id,
  uint32                  cid,
  uint32                  cmd_type,
  uint32                  req_data_len,
  qbi_utf_txn_rsp_hdlr_f *rsp_cb
)
{
  qbi_txn_s *txn;
  qbi_utf_txn_list_entry_s *txn_list_entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(utf_ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(utf_ctx->internal);
  QBI_CHECK_NULL_PTR_RET_NULL(rsp_cb);

  txn = qbi_txn_alloc_fragmented(
    utf_ctx->ctx, svc_id, ++(utf_ctx->internal->cid_txn_id), cmd_type, cid,
    0, NULL, req_data_len);
  if (txn == NULL)
  {
    QBI_LOG_E_3("Couldn't allocate txn for svc_id %d cid %d cmd_type %d",
                svc_id, cid, cmd_type);
  }
  else
  {
    /*! @todo Handle timeout of a UTF originated transaction */
    if (txn->req.size > 0 && txn->req.data != NULL)
    {
      QBI_MEMSET(txn->req.data, 0, txn->req.size);
    }

    txn_list_entry = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_utf_txn_list_entry_s));
    if (txn_list_entry == NULL)
    {
      QBI_LOG_E_3("Couldn't allocate UTF txn list entry for svc_id %d cid %d "
                  "cmd_type %d", svc_id, cid, cmd_type);
      qbi_txn_free(txn);
      txn = NULL;
    }
    else
    {
      txn_list_entry->txn = txn;
      txn_list_entry->state = QBI_UTF_TXN_STATE_NOT_DISPATCHED;
      txn_list_entry->rsp_cb = rsp_cb;

      if (!qbi_util_list_push_back_aliased(
            &utf_ctx->txn_list, &txn_list_entry->list_entry))
      {
        QBI_LOG_E_0("Couldn't add transaction to list!");
        QBI_MEM_FREE(txn_list_entry);
        qbi_txn_free(txn);
        txn = NULL;
      }
    }
  }

  return txn;
} /* qbi_utf_txn_alloc() */

/*===========================================================================
  FUNCTION: qbi_utf_init
===========================================================================*/
/*!
    @brief Initializes the unit test framework module and the test suites
    using the framework

    @details
    Prepares the tests, but does not start them
*/
/*=========================================================================*/
void qbi_utf_init
(
  void
)
{
/*-------------------------------------------------------------------------*/
  /* Initialize framework variables */
  /*! @todo TBD: More obvious/easier to manage method of defining which tests to
      run? */
  qbi_utf_state.tests_to_run[0] = "qmi_ind_reg";

  qbi_utf_ctx.internal = &qbi_utf_state;
  qbi_util_list_init(&qbi_utf_ctx.txn_list);
  qbi_util_list_init(&qbi_utf_ctx.event_handler_list);

  /* Initialize test suites (results in calls to qbi_utf_ts_register) */
  qbi_utf_shared_init();

  #ifdef FEATURE_LINUX
  qbi_utf_qdu_linux_init();
  #endif /* FEATURE_LINUX */
} /* qbi_utf_init() */

/*===========================================================================
  FUNCTION: qbi_utf_ts_abort
===========================================================================*/
/*!
    @brief Aborts the current test suite

    @details

    @param utf_ctx
*/
/*=========================================================================*/
void qbi_utf_ts_abort
(
  qbi_utf_ctx_s *utf_ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->ts);
  QBI_CHECK_NULL_PTR_RET(utf_ctx->ts->name);

  QBI_LOG_E_0("Test suite failed!! Aborting");
  qbi_utf_ts_teardown(utf_ctx, TRUE);
} /* qbi_utf_ts_abort() */

/*===========================================================================
  FUNCTION: qbi_utf_ts_register
===========================================================================*/
/*!
    @brief Registers a test suite to the unit test framework

    @details
    Registered test suites are available for execution, but need to be
    enabled so they can be run.

    @param test_suite Pointer to a structure containing information about
    the test suite. The data pointed to by this structure must persist
    throughout the lifetime of the test, i.e. if it is dynamically allocated,
    then it must not be freed.
*/
/*=========================================================================*/
void qbi_utf_ts_register
(
  const qbi_utf_ts_s *test_suite
)
{
  uint32 i;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(test_suite);
  QBI_CHECK_NULL_PTR_RET(test_suite->start_fcn);

  for (i = 0; i < QBI_UTF_TS_MAX; i++)
  {
    if (qbi_utf_registered_test_suites[i] == NULL)
    {
      QBI_LOG_STR_2("Registered test suite '%s' at index %d",
                    test_suite->name, i);
      qbi_utf_registered_test_suites[i] = test_suite;
      break;
    }
  }

  if (i >= QBI_UTF_TS_MAX)
  {
    QBI_LOG_E_0("Couldn't register test suite! Out of available space");
    QBI_LOG_STR_1("Failed test suite name was '%s'", test_suite->name);
  }
} /* qbi_utf_ts_register() */

/*===========================================================================
  FUNCTION: qbi_utf_run
===========================================================================*/
/*!
    @brief Starts running the configured unit test framework test suite(s)

    @details
*/
/*=========================================================================*/
void qbi_utf_run
(
  void
)
{
/*-------------------------------------------------------------------------*/
  qbi_utf_ts_start_next(&qbi_utf_ctx);
} /* qbi_utf_run() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_check_status
===========================================================================*/
/*!
    @brief Performs sanity checks on the given transaction

    @details
    Verifies that the transaction is not NULL, its status is SUCCESS, that
    the response buffer (fixed-length portion only) meets the given minimum
    length, and that the response buffer is not NULL (assuming the expected
    size is greater than zero).

    @param txn
    @param expected_min_fixed_rsp_size

    @return boolean TRUE if checks passed, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_utf_txn_check_status
(
  const qbi_txn_s *txn,
  uint32           expected_min_fixed_rsp_size
)
{
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  if (txn->status != QBI_MBIM_STATUS_SUCCESS)
  {
    QBI_LOG_E_1("Request failed with status %d", txn->status);
  }
  else if (txn->rsp.size < expected_min_fixed_rsp_size)
  {
    QBI_LOG_E_2("Fixed-length portion of response buffer too small: got %d, "
                "expected %d", txn->rsp.size, expected_min_fixed_rsp_size);
  }
  else if (expected_min_fixed_rsp_size > 0 && txn->rsp.data == NULL)
  {
    QBI_LOG_E_0("Missing response buffer");
  }
  else
  {
    result = TRUE;
  }

  return result;
} /* qbi_utf_txn_check_status() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_event_handler_register
===========================================================================*/
/*!
    @brief Registers a callback function to be invoked when an event
    transaction for the given CID is received. Also ensures that the event
    will not be dropped by the event filtering mechanism.

    @details

    @param utf_ctx
    @param svc_id
    @param cid
    @param event_cb

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_utf_txn_event_handler_register
(
  qbi_utf_ctx_s          *utf_ctx,
  qbi_svc_id_e            svc_id,
  uint32                  cid,
  qbi_utf_txn_rsp_hdlr_f *event_cb
)
{
  boolean success = FALSE;
  qbi_utf_txn_event_handler_s *handler;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(utf_ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(event_cb);

  handler = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_utf_txn_event_handler_s));
  QBI_CHECK_NULL_PTR_RET_FALSE(handler);

  handler->svc_id = svc_id;
  handler->cid = cid;
  handler->event_cb = event_cb;
  if (!qbi_util_list_push_back_aliased(
        &utf_ctx->event_handler_list, &handler->list_entry))
  {
    QBI_LOG_E_0("Couldn't add event handler to list!");
    QBI_MEM_FREE(handler);
  }
  else if (!qbi_svc_event_filter_update_cid(
             utf_ctx->ctx, svc_id, cid, TRUE, NULL))
  {
    QBI_LOG_E_2("Couldn't enable CID events for svc_id %d cid %d", svc_id, cid);
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_utf_txn_event_handler_register() */

/*===========================================================================
  FUNCTION: qbi_utf_txn_event_handler_unregister
===========================================================================*/
/*!
    @brief Unregisters an event handler

    @details

    @param utf_ctx
    @param svc_id
    @param cid
    @param event_cb
*/
/*=========================================================================*/
void qbi_utf_txn_event_handler_unregister
(
  qbi_utf_ctx_s          *utf_ctx,
  qbi_svc_id_e            svc_id,
  uint32                  cid,
  qbi_utf_txn_rsp_hdlr_f *event_cb
)
{
  qbi_util_list_iter_s iter;
  qbi_utf_txn_event_handler_s *handler;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(utf_ctx);

  qbi_util_list_iter_init(&utf_ctx->event_handler_list, &iter);
  while ((handler = (qbi_utf_txn_event_handler_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (handler->svc_id == svc_id && handler->cid == cid &&
        handler->event_cb == event_cb)
    {
      qbi_util_list_iter_remove(&iter);
      QBI_MEMSET(handler, 0, sizeof(qbi_utf_txn_event_handler_s));
      QBI_MEM_FREE(handler);
    }
  }
} /* qbi_utf_txn_event_handler_unregister() */

