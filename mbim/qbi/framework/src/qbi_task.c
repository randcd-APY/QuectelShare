/*!
  @file
  qbi_task.c

  @brief
  Task-related functions for QBI
*/

/*=============================================================================

  Copyright (c) 2011-2013, 2015 Qualcomm Technologies, Inc.
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
11/05/15  hz   Add wake lock support for master timer
04/01/13  bd   Move platform-specific device service init calls to task_init
10/08/12  hz   Add DIAG over MBIM support
09/02/11  bd   Updated to MBIM v0.81c
08/23/11  bd   Tweak to initialization order
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_task.h"

#include "qbi_common.h"
#include "qbi_msg.h"
#include "qbi_os.h"
#include "qbi_svc.h"
#include "qbi_txn.h"
#include "qbi_util.h"

#ifdef QBI_UNIT_TEST
#include "qbi_ut.h"
#endif /* QBI_UNIT_TEST */

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! Describes a command (callback function + data) to be run in the QBI task's
    context */
typedef struct {
  /*! Linkage for adding to qbi_task_data.cmd_list; must be first as we alias */
  qbi_util_list_entry_s list_entry;

  /*! @brief Type of command
      @details This is currently only used for logging purposes */
  qbi_task_cmd_id_e id;

  /*! QBI context associated with the command */
  qbi_ctx_s *ctx;

  /*! Encoded callback function to invoke from the QBI task
      @see qbi_task_cmd_cb_f */
  qbi_os_encoded_fptr_t cb;

  /*! Data pointer to pass to the callback function */
  void *cb_data;
} qbi_task_cmd_s;

/*=============================================================================

  Private Variables

=============================================================================*/

/*! Local QBI task data */
struct {
  /*! Flag indicating whether we are ready to receive commands */
  boolean ready_for_cmds;

  /*! List containing commands to post to the QBI thread; list entries are
      qbi_task_cmd_s */
  qbi_util_list_s cmd_list;

  /*! Mutex protecting cmd_list */
  qbi_os_mutex_t  cmd_list_mutex;

  /*! List of registered QBI contexts. This list must only be accessed from
      the QBI task. */
  qbi_util_list_s ctx_list;

  /*! Thread control data */
  qbi_os_thread_info_t task_info;

  /*! Timer used to trigger checks for transaction timeout */
  qbi_os_timer_t timeout_timer;
} qbi_task_data = {FALSE,};


/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static void qbi_task_check_timeout
(
  void
);

static void qbi_task_cmd_free
(
  qbi_task_cmd_s *cmd
);

static void qbi_task_cmd_proc_all
(
  void
);

static void qbi_task_reg_ctx_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static void qbi_task_timeout_cb
(
  void *data
);


/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_task_check_timeout
===========================================================================*/
/*!
    @brief Checks for transaction timeout on each of the registered contexts

    @details
    Sets the timeout_timer to expire at the time of the next soonest
    transaction timeout
*/
/*=========================================================================*/
static void qbi_task_check_timeout
(
  void
)
{
  qbi_ctx_s *ctx;
  qbi_os_time_ms_t next_timeout;
  qbi_os_time_ms_t timer_delay = 0;
  qbi_util_list_iter_s iter;
/*-------------------------------------------------------------------------*/
  qbi_util_list_iter_init(&qbi_task_data.ctx_list, &iter);
  while ((ctx = (qbi_ctx_s *) qbi_util_list_iter_next(&iter)) != NULL)
  {
    next_timeout = qbi_txn_check_timeout(ctx);
    if (next_timeout != 0 && (timer_delay == 0 || next_timeout < timer_delay))
    {
      timer_delay = next_timeout;
    }
  }

  /* OS may go to sleep without clock running. Keep OS awake when master
     timer is active to maintain correct timer behaviour. */
  if (timer_delay != 0)
  {
    (void) qbi_os_timer_wake_lock();
    (void) qbi_os_timer_set(&qbi_task_data.timeout_timer, timer_delay);
  }
  else
  {
    (void) qbi_os_timer_clear(&qbi_task_data.timeout_timer);
    (void) qbi_os_timer_wake_unlock();
  }
} /* qbi_task_check_timeout() */

/*===========================================================================
  FUNCTION: qbi_task_cmd_free
===========================================================================*/
/*!
    @brief Free memory associated with a command buffer

    @details

    @param cmd
*/
/*=========================================================================*/
static void qbi_task_cmd_free
(
  qbi_task_cmd_s *cmd
)
{
/*-------------------------------------------------------------------------*/
  QBI_MEMSET(cmd, 0, sizeof(qbi_task_cmd_s));
  QBI_MEM_FREE(cmd);
} /* qbi_task_cmd_free() */

/*===========================================================================
  FUNCTION: qbi_task_cmd_proc_all
===========================================================================*/
/*!
    @brief Process all commands on the command list

    @details

*/
/*=========================================================================*/
static void qbi_task_cmd_proc_all
(
  void
)
{
  qbi_task_cmd_s *cmd = NULL;
  qbi_task_cmd_cb_f *cmd_cb = NULL;
/*-------------------------------------------------------------------------*/
  for (;;)
  {
    qbi_os_mutex_lock(&qbi_task_data.cmd_list_mutex);
    cmd = (qbi_task_cmd_s *) qbi_util_list_pop_front(&qbi_task_data.cmd_list);
    qbi_os_mutex_unlock(&qbi_task_data.cmd_list_mutex);

    if (cmd == NULL)
    {
      break;
    }
    else
    {
      if (qbi_os_fptr_decode(&cmd->cb, (qbi_os_void_f **) &cmd_cb, FALSE))
      {
        if (cmd->id == QBI_TASK_CMD_ID_DIAG_DATA)
        {
          qbi_log_silent_mode_set(TRUE);
        }

        QBI_LOG_I_1("Processing command ID %d", cmd->id);
        cmd_cb(cmd->ctx, cmd->id, cmd->cb_data);

        if (cmd->id == QBI_TASK_CMD_ID_DIAG_DATA)
        {
          qbi_log_silent_mode_set(FALSE);
        }
      }
      qbi_task_cmd_free(cmd);
    }
  }
} /* qbi_task_cmd_proc_all() */

/*===========================================================================
  FUNCTION: qbi_task_reg_ctx_cb
===========================================================================*/
/*!
    @brief Command callback that performs the addition of a QBI context
    to the task's context list

    @details

    @param ctx
    @param cmd_id
    @param data

    @see qbi_task_register_context
*/
/*=========================================================================*/
static void qbi_task_reg_ctx_cb
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

  qbi_msg_ctx_init(ctx);
  if (!qbi_util_list_push_back_aliased(
        &qbi_task_data.ctx_list, &ctx->list_entry))
  {
    QBI_LOG_E_1("Couldn't add context ID %d to task's list!", ctx->id);
  }
  else
  {
    QBI_LOG_I_1("Added context ID %d to task's context list", ctx->id);
  }
} /* qbi_task_reg_ctx_cb() */

/*===========================================================================
  FUNCTION: qbi_task_timeout_cb
===========================================================================*/
/*!
    @brief Callback function invoked when the timeout timer expires

    @details

    @param data
*/
/*=========================================================================*/
static void qbi_task_timeout_cb
(
  void *data
)
{
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(data);

  /* Send an empty command just to get the task loop to spin - it will invoke
     qbi_task_check_timeout() on its own, and this command will just get
     dropped since its callback function is NULL. */
  if (!qbi_task_cmd_send(NULL, QBI_TASK_CMD_ID_TIMER_CB, NULL, NULL))
  {
    QBI_LOG_E_0("Couldn't send command for timeout timer callback!");
  }
} /* qbi_task_timeout_cb() */


/*=============================================================================

  Public Function Definitions

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_task_get_ctx_by_id
===========================================================================*/
/*!
    @brief Retrieves a pointer to a QBI context based on the context ID

    @details
    The context must have been previously registered with the QBI task
    via qbi_task_register_context(). This function must only be called
    from the context of the QBI task.

    @param ctx_id

    @return qbi_ctx_s* Pointer to context with matching ID, NULL if not
    found or an error occurred
*/
/*=========================================================================*/
qbi_ctx_s *qbi_task_get_ctx_by_id
(
  qbi_ctx_id_e ctx_id
)
{
  qbi_ctx_s *ctx;
  qbi_util_list_iter_s iter;
/*-------------------------------------------------------------------------*/
  qbi_util_list_iter_init(&qbi_task_data.ctx_list, &iter);
  while ((ctx = (qbi_ctx_s *) qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (ctx->id == ctx_id)
    {
      break;
    }
  }

  return ctx;
} /* qbi_task_get_ctx_by_id() */

/*===========================================================================
  FUNCTION: qbi_task_init
===========================================================================*/
/*!
    @brief Performs initialization for the QBI task

    @details
    This function performs the minimum amount of initialization so that the
    QBI task can receive commands from other tasks, etc. It must be called
    from the context of the QBI task before calling qbi_task_run().
*/
/*=========================================================================*/
void qbi_task_init
(
  void
)
{
/*-------------------------------------------------------------------------*/
  qbi_os_mutex_init(&qbi_task_data.cmd_list_mutex);
  qbi_os_thread_init(&qbi_task_data.task_info);

  qbi_util_list_init(&qbi_task_data.cmd_list);
  qbi_util_list_init(&qbi_task_data.ctx_list);

  /* At this point we are ready to receive commands - they will be processed
     after the rest of the initialization happens */
  qbi_task_data.ready_for_cmds = TRUE;
} /* qbi_task_init() */

/*===========================================================================
  FUNCTION: qbi_task_register_context
===========================================================================*/
/*!
    @brief Registers a new QBI context with the QBI task

    @details
    Enables processing of transaction timeouts for the context, and ensures
    that the QBI context structure is initialized. Must be called before
    processing incoming messages for the context. The QBI task must be
    initialized before calling this function. A context must not already
    be registered with the same context ID.

    @param ctx
*/
/*=========================================================================*/
void qbi_task_register_context
(
  qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  /* The context list does not have a mutex, so it must only be accessed from
     the QBI task */
  if (!qbi_task_cmd_send(ctx, QBI_TASK_CMD_ID_REG_CTX, qbi_task_reg_ctx_cb,
                         NULL))
  {
    QBI_LOG_E_1("Couldn't send command to register context ID %d!", ctx->id);
  }
} /* qbi_task_register_context() */

/*===========================================================================
  FUNCTION: qbi_task_run
===========================================================================*/
/*!
    @brief QBI task's main loop

    @details
    The platform-specific task layer ensures that this function is called
    from the context of the QBI task after qbi_task_init(). This function
    should never return.
*/
/*=========================================================================*/
void qbi_task_run
(
  void
)
{
/*-------------------------------------------------------------------------*/
  /* Initialize the timer used for QBI transaction timeouts */
  (void) qbi_os_timer_init(&qbi_task_data.timeout_timer, qbi_task_timeout_cb,
                           NULL);

  #if defined(QBI_UNIT_TEST) && !defined(FEATURE_RCINIT)
  qbi_ut_init();
  #endif /* QBI_UNIT_TEST */

  QBI_LOG_I_0("Startup init completed, entering main loop");
  for (;;)
  {
    if (!qbi_os_thread_wait_cmd(&qbi_task_data.task_info))
    {
      QBI_LOG_E_0("Wait function returned with error!");
      break;
    }
    else if (qbi_os_thread_cmd_is_ready(&qbi_task_data.task_info))
    {
      qbi_os_thread_clear_cmd_ready_sig(&qbi_task_data.task_info);

      qbi_task_cmd_proc_all();
    }
    qbi_task_check_timeout();
  }

  /* Task no longer running - don't accept new commands */
  qbi_task_data.ready_for_cmds = FALSE;

  /*! @note If this task will be stopped in situations where the device is not
      powering off, we most likely end up leaking memory at this point. Would
      need to free all commands, and mimic a MBIM_CLOSE_MSG request to properly
      support that situation. */
} /* qbi_task_run() */

/*===========================================================================
  FUNCTION: qbi_task_cmd_cancel
===========================================================================*/
/*!
    @brief Cancels all pending commands with the given context and command
    ID.

    @details
    The purpose of the cancel_cb is to perform any cleanup needed before
    aborting the command, e.g. freeing memory allocated in user_data. Note
    that as commands can be posted from other threads, the caller should
    ensure that no new commands with the given ctx or cmd_id will be
    posted while this function is executing, otherwise they may not be
    cancelled.

    @param ctx
    @param cmd_id
    @param cancel_cb If not NULL, will be invoked for every pending command
    that was cancelled.

    @note WARNING: This function must only be called from the context of
    the QBI task.
*/
/*=========================================================================*/
void qbi_task_cmd_cancel
(
  qbi_ctx_s         *ctx,
  qbi_task_cmd_id_e  cmd_id,
  qbi_task_cmd_cb_f *cancel_cb
)
{
  qbi_task_cmd_s *cmd;
  qbi_util_list_iter_s iter;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  /* As this function must only be called from the QBI task's context, we
     can safely assume that no entries will be removed from the command list,
     so this list iterator does not need to be registered. New commands may
     be added to the list while we are processing, but this is safe due to
     our use of the mutex. Depending on our stage of processing in this
     function, a newly added command may or may not be processed, but the
     caller cannot assume one way or the other, so this is okay. */
  qbi_os_mutex_lock(&qbi_task_data.cmd_list_mutex);
  qbi_util_list_iter_init(&qbi_task_data.cmd_list, &iter);
  qbi_os_mutex_unlock(&qbi_task_data.cmd_list_mutex);

  /* Don't need to protect qbi_util_list_iter_has_next() with the mutex since
     this operation only checks data stored internally in the iterator */
  while (qbi_util_list_iter_has_next(&iter))
  {
    qbi_os_mutex_lock(&qbi_task_data.cmd_list_mutex);
    cmd = (qbi_task_cmd_s *) qbi_util_list_iter_next(&iter);
    qbi_os_mutex_unlock(&qbi_task_data.cmd_list_mutex);

    if (cmd == NULL)
    {
      QBI_LOG_E_0("Unexpected NULL pointer!");
      break;
    }
    else if (cmd->id == cmd_id && cmd->ctx == ctx)
    {
      if (cancel_cb != NULL)
      {
        cancel_cb(ctx, cmd_id, cmd->cb_data);
      }
      QBI_LOG_I_2("Cancelling command ID %d on context ID %d", cmd_id, ctx->id);

      qbi_os_mutex_lock(&qbi_task_data.cmd_list_mutex);
      qbi_util_list_iter_remove(&iter);
      qbi_os_mutex_unlock(&qbi_task_data.cmd_list_mutex);

      qbi_task_cmd_free(cmd);
    }
  }
} /* qbi_task_cmd_cancel() */

/*===========================================================================
  FUNCTION: qbi_task_cmd_send
===========================================================================*/
/*!
    @brief Posts a command to the QBI task

    @details

    @param ctx QBI context pointer to pass to the callback function
    @param cmd_id Informational command identifier
    @param cb Function to execute in the QBI task's context
    @param cb_data Data pointer passed to the callback function

    @return boolean TRUE on success, FALSE on failure

    @note Usually runs outside the QBI task
*/
/*=========================================================================*/
boolean qbi_task_cmd_send
(
  qbi_ctx_s         *ctx,
  qbi_task_cmd_id_e  cmd_id,
  qbi_task_cmd_cb_f *cb,
  void              *cb_data
)
{
  boolean result = FALSE;
  qbi_task_cmd_s *cmd = NULL;
/*-------------------------------------------------------------------------*/
  if (!qbi_task_data.ready_for_cmds)
  {
    QBI_LOG_E_0("Task not ready to receive commands!");
  }
  else
  {
    cmd = (qbi_task_cmd_s *) QBI_MEM_MALLOC_CLEAR(sizeof(qbi_task_cmd_s));
    if (cmd == NULL)
    {
      QBI_LOG_E_0("Failure allocating command!");
    }
    else
    {
      cmd->ctx     = ctx;
      cmd->id      = cmd_id;
      cmd->cb_data = cb_data;
      qbi_os_fptr_encode(&cmd->cb, (qbi_os_void_f *) cb);

      qbi_os_mutex_lock(&qbi_task_data.cmd_list_mutex);
      result = qbi_util_list_push_back_aliased(
        &qbi_task_data.cmd_list, &cmd->list_entry);
      qbi_os_mutex_unlock(&qbi_task_data.cmd_list_mutex);

      if (!result)
      {
        QBI_LOG_E_0("Couldn't add command to list");
      }
      else
      {
        result = qbi_os_thread_notify_cmd(&qbi_task_data.task_info);
      }
    }
  }

  if (!result && cmd != NULL)
  {
    qbi_task_cmd_free(cmd);
  }

  return result;
} /* qbi_task_cmd_send() */

