
/*!
  @file
  qbi_task.h

  @brief
  Task-related functions for QBI
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
01/22/13  bd   Add QDU device service, new unit test framework
10/08/12  hz   Add DIAG over MBIM support
03/05/12  bd   Add modem subsystem restart command
01/30/12  cy   Add NTB max num/size config and Reset function
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_TASK_H
#define QBI_TASK_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*=============================================================================

  Typedefs

=============================================================================*/

/*! List of command IDs. Purely informative - used for debugging purposes. */
typedef enum {
  QBI_TASK_CMD_ID_RX_DATA   = 0, /*!< Packet received from host */
  QBI_TASK_CMD_ID_QMI_RSP   = 1, /*!< QMI response received from modem */
  QBI_TASK_CMD_ID_QMI_IND   = 2, /*!< QMI indication received from modem */
  QBI_TASK_CMD_ID_REG_CTX   = 3, /*!< Register a new QBI context */
  QBI_TASK_CMD_ID_TIMER_CB  = 4, /*!< Transaction timeout timer callback */
  QBI_TASK_CMD_ID_UT_CB     = 5, /*!< Unit test timer callback */
  QBI_TASK_CMD_ID_MEM_DBG   = 6, /*!< Memory debug statistics timer callback */
  QBI_TASK_CMD_ID_QMUX_DATA = 7, /*!< EXT_QMUX RX data from modem */
  QBI_TASK_CMD_ID_CMD_FRAG_TIMEOUT = 8, /*!< Command fragment timeout */
  QBI_TASK_CMD_ID_RESET     = 9, /*!< Reset function from host */
  QBI_TASK_CMD_ID_MODEM_RESTART = 10, /*!< Modem subsystem restart detected */
  QBI_TASK_CMD_ID_DIAG_DATA = 11, /*!< Diagnistic messages from SIO */
  QBI_TASK_CMD_ID_QDU_SESSION_CHECK = 12, /*!< QDU session status check */
  QBI_TASK_CMD_ID_UTF_DISPATCH = 13, /*!< Unit test framework dispatch */
  QBI_TASK_CMD_ID_DEFERRED_EVENT = 14, /*!< Deferred event processing */

  QBI_TASK_CMD_ID_MAX
} qbi_task_cmd_id_e;

typedef void (qbi_task_cmd_cb_f)
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

/*=============================================================================

  Function Prototypes

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
);

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
);

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
);

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
);

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
);

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
);

#endif /* QBI_TASK_H */

