/*!
  @file
  qbi_diag_sio.c

  @brief
  QBI DIAG layer for targets using DIAG over MBIM feature

  @details
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
09/25/11  bo   Initial release
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_hc.h"
#include "qbi_diag.h"

#include "qbi_common.h"
#include "qbi_log.h"
#include "qbi_msg.h"
#include "qbi_task.h"
#include "qbi_svc_qmbe.h"

#include "diag_lsm.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! Command buffer used to pass Diag DL data to the QBI task for processing */
typedef struct {
  qbi_util_buf_s buf;
} qbi_diag_cmd_s;

/*! @brief Command buffer used to pass packets received from the host into the
    QBI layer for processing
*/
typedef struct {
  /*! Set to TRUE when diag traffic has been suspended */
  boolean suspended;

  /* Encoded callback function to handle Diag packets from Diag task
     @see qbi_diag_rx_from_diag_cb_f */
  qbi_os_encoded_fptr_t qbi_diag_rx_data_cb;
} qbi_diag_info_s;

/*=============================================================================

  Private Variables

=============================================================================*/
static qbi_diag_info_s qbi_diag_info = {TRUE, };

/*=============================================================================

  Private Function Prototypes

=============================================================================*/
static void qbi_diag_cmd_cancel_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static void qbi_diag_cmd_free
(
  qbi_diag_cmd_s *cmd
);

static void qbi_diag_proc_rx_data
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static int qbi_diag_transmit
(
  uint8 *data,
  int    size,
  void  *context_data
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_diag_cmd_cancel_cb
===========================================================================*/
/*!
    @brief Callback invoked when a command is canceled; takes care of
    cleaning up memory

    @details

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_diag_cmd_cancel_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(ctx);
  QBI_ARG_NOT_USED(cmd_id);

  qbi_diag_cmd_free((qbi_diag_cmd_s *) data);
} /* qbi_diag_cmd_cancel_cb() */

/*===========================================================================
  FUNCTION: qbi_diag_cmd_free
===========================================================================*/
/*!
    @brief Releases memory allocated for a Diag QBI task command

    @details

    @param cmd
*/
/*=========================================================================*/
static void qbi_diag_cmd_free
(
  qbi_diag_cmd_s *cmd
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(cmd);
  qbi_util_buf_free(&cmd->buf);
  QBI_MEM_FREE(cmd);
} /* qbi_diag_cmd_free() */

/*===========================================================================
  FUNCTION: qbi_diag_proc_rx_data
===========================================================================*/
/*!
    @brief Process received data in the QBI task's context

    @details

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_diag_proc_rx_data
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
  qbi_diag_cmd_s *cmd;
  qbi_diag_rx_from_diag_cb_f *diag_rx_data_cb = NULL;
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(cmd_id);

  cmd = (qbi_diag_cmd_s *) data;
  QBI_CHECK_NULL_PTR_RET(cmd);

  if (qbi_os_fptr_decode(&qbi_diag_info.qbi_diag_rx_data_cb,
                         (qbi_os_void_f **) &diag_rx_data_cb, FALSE))
  {
    diag_rx_data_cb(ctx, &cmd->buf);
  }

  qbi_diag_cmd_free(cmd);
} /* qbi_diag_proc_rx_data() */

/*===========================================================================
  FUNCTION: qbi_diag_transmit
===========================================================================*/
/*!
    @brief This function handles DIAG packet from DIAG service to QBI by
    forwarding the data to QBI task for processing before send to host.

    @details

    @param data
    @param size
    @param context_data

    @return int

*/
/*=========================================================================*/
static int qbi_diag_transmit
(
  uint8 *data,
  int    size,
  void  *context_data
)
{
  qbi_diag_cmd_s *cmd;
  qbi_ctx_s      *ctx;
  int             bytes_sent = 0;
/*-------------------------------------------------------------------------*/
  ctx = qbi_task_get_ctx_by_id(QBI_CTX_ID_PRIMARY);
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx);

  if (ctx->state != QBI_CTX_STATE_OPENED ||
      qbi_diag_info.suspended)
  {
    if (!qbi_diag_info.suspended)
    {
      qbi_diag_info.suspended = TRUE;
      qbi_task_cmd_cancel(ctx, QBI_TASK_CMD_ID_DIAG_DATA,
                          qbi_diag_cmd_cancel_cb);
    }
  }
  else
  {
    cmd = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_diag_cmd_s));
    QBI_CHECK_NULL_PTR_RET_ZERO(cmd);

    qbi_util_buf_init(&cmd->buf);
    if (qbi_util_buf_alloc_dont_clear(&cmd->buf, size) == NULL)
    {
      QBI_LOG_E_1("Couldn't allocate %d bytes for diag pkt!", size);
      qbi_diag_cmd_free(cmd);
    }
    else
    {
      QBI_MEMSCPY(cmd->buf.data, cmd->buf.size, data, size);
      if (!qbi_task_cmd_send(ctx, QBI_TASK_CMD_ID_DIAG_DATA,
                             qbi_diag_proc_rx_data, cmd))
      {
        QBI_LOG_E_0("Couldn't send command for diag pkt!");
        qbi_diag_cmd_free(cmd);
      }
      else
      {
        bytes_sent = cmd->buf.size;
      }
    }
  }

  return bytes_sent;
} /* qbi_diag_transmit */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_diag_dispatch_ul_message
===========================================================================*/
/*!
    @brief To be called for QMBE DIAG_DATA UL set processing for
           each DIAG msg from host to device

    @details

    @param data
    @param size

    @return boolean
*/
/*=========================================================================*/
boolean qbi_diag_dispatch_ul_message
(
  void *data,
  uint32 size
)
{
  boolean result = FALSE;
  qbi_util_buf_s buf;
/*-------------------------------------------------------------------------*/
  /* Config Diag callback mode upon receiving first UL request from host */
  if (qbi_diag_info.suspended)
  {
    if (!Diag_LSM_Init(NULL))
    {
      QBI_LOG_E_0("Diag LSM initialization failure!");
      return result;
    }
    else
    {
      diag_register_callback(&qbi_diag_transmit, NULL);
      diag_switch_logging(CALLBACK_MODE, NULL);
      qbi_diag_info.suspended = FALSE;
    }
  }

  if (!qbi_diag_info.suspended)
  {
    qbi_util_buf_init(&buf);
    buf.size = size + sizeof(int);
    if (qbi_util_buf_alloc_dont_clear(&buf, buf.size) != NULL)
    {
      *(int *)buf.data = USER_SPACE_DATA_TYPE;
      QBI_MEMSCPY(buf.data + sizeof(int), (buf.size - sizeof(int)), data, size);

      if (diag_send_data(buf.data, buf.size) == 0)
      {
        result = TRUE;
      }
      else
      {
        QBI_LOG_E_0("Diag data send failure!");
      }

      qbi_util_buf_free(&buf);
    }
  }

  return result;
} /* qbi_diag_dispatch_ul_message */

/*===========================================================================
  FUNCTION: qbi_diag_init
===========================================================================*/
/*!
    @brief Initialize QBI DIAG SIO device internals

    @details

    @param void
*/
/*=========================================================================*/
void qbi_diag_init
(
  void
)
{
/*-------------------------------------------------------------------------*/
} /* qbi_diag_init() */

/*===========================================================================
  FUNCTION: qbi_diag_reg_rx_data_cb
===========================================================================*/
/*!
    @brief Register Diag rx data callback function

    @details

    @param qbi_diag_rx_from_diag_cb_f
*/
/*=========================================================================*/
void qbi_diag_reg_rx_data_cb
(
  qbi_diag_rx_from_diag_cb_f *rx_data_cb
)
{
/*-------------------------------------------------------------------------*/
  qbi_os_fptr_encode(&qbi_diag_info.qbi_diag_rx_data_cb,
                     (qbi_os_void_f *) rx_data_cb);
} /* qbi_diag_reg_rx_data_cb */

/*===========================================================================
  FUNCTION: qbi_diag_suspend
===========================================================================*/
/*!
    @brief Suspend Diag traffic when needed

    @details
*/
/*=========================================================================*/
void qbi_diag_suspend
(
  void
)
{
/*-------------------------------------------------------------------------*/
  if (!qbi_diag_info.suspended)
  {
    qbi_diag_info.suspended = TRUE;
    Diag_LSM_DeInit();
  }
} /* qbi_diag_suspend */
