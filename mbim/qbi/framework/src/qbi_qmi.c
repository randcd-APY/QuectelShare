/*!
  @file
  qbi_qmi.c

  @brief
  Formatting and other common processing for QMI messages
*/

/*=============================================================================

  Copyright (c) 2011-2014, 2017-2018 Qualcomm Technologies, Inc.
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
03/12/18  mm   Added variable initialization
03/06/18  mm   Added call deactivation event to Host during SSR
02/05/18  mm   Added logic to handle modem SSR
06/02/17  vk   Add required service object for PDC,DSD and SAR services
03/20/13  bd   Add QMI SSCTL support
03/13/13  hz   Add support for multiple data sessions
02/11/13  bd   Add cross-platform support for QCCI setup, teardown, instance
05/21/12  bd   Add cross-platform QCCI OS parameters support
05/14/12  bd   Use new QCCI API to get transaction ID
05/08/12  bd   Determine QMI request structure size dynamically based on IDL
03/05/12  bd   Update to new QCCI API including client notifier
12/09/11  cy   Dual IP support
09/23/11  bd   Use new QMI WDA set data format message to configure NTB
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

/*=============================================================================
  Include Files
=============================================================================*/

#include "qbi_qmi.h"
#include "qbi_msg_mbim.h"
#include "qbi_msg.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_svc_bc_ext_mbim.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc.h"
#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_msg.h"
#include "qbi_os_qcci.h"
#include "qbi_qmi_txn.h"
#include "qbi_task.h"
#include "qbi_txn.h"
#include "qbi_util.h"
#include "qbi_nv_store.h"
#include "qmi_client.h"
/* Below is needed for qmi_client_get_async_txn_id(). Note that "deprecated"
   in this case means that the API will not be supported for new (non-QMUX)
   QMI services. */
#include "qmi_client_deprecated.h"
#include "qmi_idl_lib.h"

#include "card_application_toolkit_v02.h"
#include "device_management_service_v01.h"
#include "network_access_service_v01.h"
#include "phonebook_manager_service_v01.h"
#include "subsystem_control_v01.h"
#include "user_identity_module_v01.h"
#include "voice_service_v02.h"
#include "wireless_data_administrative_service_v01.h"
#include "wireless_data_service_v01.h"
#include "wireless_messaging_service_v01.h"
#include "data_system_determination_v01.h"
#include "persistent_device_configuration_v01.h"
#include "specific_absorption_rate_v01.h"

#include <stdint.h>

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! How long we will wait for a QMI service to become ready before giving up.
    Default: 500ms, can be set on a per-platform basis. */
#ifdef QBI_OS_QMI_INIT_TIMEOUT_MS
#define QBI_QMI_SVC_INIT_TIMEOUT_MS QBI_OS_QMI_INIT_TIMEOUT_MS
#else
#define QBI_QMI_SVC_INIT_TIMEOUT_MS (500)
#endif

/*! The QMI service ID which will be used to register the QCCI client error
    callback used to detect modem restart. Using QMI DMS as a client is held by
    QBI for the duration of the MBIM session. */
#define QBI_QMI_SVC_ID_FOR_ERROR_CB (QBI_QMI_SVC_DMS)

/*! Maximum control transfer size, negotiated with host */
#define QBI_QMI_MAX_CONTROL_SIZE (2048)

/*! Transactionid assumed to be 1 to perform MBIM_OPEN when modem SSR
  happened. It is fine to assume id as 1 because this will be first transaction.
  and qbi_msg_common_hdr_s  MBIM Message Header, common to all MBIM commands */
#define QBI_QMI_POST_INTERNAL_MBIM_OPEN_TX_ID (1)

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! @brief Data passed from QMI indication/response callback handler to QBI task
*/
typedef struct {
  /*! Command ID: either QBI_TASK_CMD_ID_QMI_RSP or QBI_TASK_CMD_ID_QMI_IND */
  qbi_task_cmd_id_e cmd_id;

  /*! QMI handle the callback was received on */
  qmi_client_type qmi_handle;

  /*! QMI message ID */
  uint16 qmi_msg_id;

  /*! Data specific to either the response or indication, depending on the value
      of cmd_id */
  union
  {
    struct
    {
      qbi_util_buf_s qmi_data;
    } ind;

    struct
    {
      qbi_qmi_txn_s        *txn;
      qmi_client_error_type transp_err;

      /* Parameters from QCCI that are used in the sanity check performed a
         second time from the context of the QBI task. */
      unsigned int msg_id;
      void        *rsp_buf;
      int          rsp_buf_len;
    } rsp;
  } data;
} qbi_qmi_cb_info_s;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_qmi_cb_info_s *qbi_qmi_alloc_cb_info
(
  qbi_task_cmd_id_e cmd_id,
  qmi_client_type   qmi_handle,
  uint16            qmi_msg_id,
  uint32            ind_buf_len
);

static void qbi_qmi_cmd_cancel_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static void qbi_qmi_client_error_cb
(
 qmi_client_type       user_handle,
 qmi_client_error_type error,
 void                 *err_cb_data
);

static boolean qbi_qmi_decode_ind
(
  qbi_ctx_s            *ctx,
  qbi_qmi_svc_e         qmi_svc_id,
  uint16                qmi_msg_id,
  const qbi_util_buf_s *wire_format,
  qbi_util_buf_s       *c_struct
);

static void qbi_qmi_free_cb_info
(
  qbi_qmi_cb_info_s *cb_info
);

static boolean qbi_qmi_get_service_obj
(
  qbi_qmi_svc_e                qmi_svc_id,
  qmi_idl_service_object_type *svc_obj
);

static qbi_qmi_svc_e qbi_qmi_handle_to_svc_id
(
  qbi_ctx_s      *ctx,
  qmi_client_type qmi_handle
);

static void qbi_qmi_ind_cb
(
  qmi_client_type user_handle,
  unsigned int    msg_id,
  void           *ind_buf,
  unsigned int    ind_buf_len,
  void           *ind_cb_data
);

static boolean qbi_qmi_init_client
(
  qbi_ctx_s          *ctx,
  qbi_qmi_svc_e       qmi_svc_id,
  qbi_qmi_svc_info_s *svc_info
);

static boolean qbi_qmi_is_svc_ready
(
  const qbi_qmi_svc_info_s *svc_info
);

static void qbi_qmi_proc_ind_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static void qbi_qmi_proc_modem_restart_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static void qbi_qmi_proc_rsp_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
);

static void qbi_qmi_release_client
(
  qbi_ctx_s          *ctx,
  qbi_qmi_svc_info_s *svc_info
);

static void qbi_qmi_rsp_cb
(
  qmi_client_type        user_handle,
  unsigned int           msg_id,
  void                  *rsp_buf,
  unsigned int           rsp_buf_len,
  void                  *user_data,
  qmi_client_error_type  transp_err
);

static boolean qbi_qmi_svc_id_is_valid
(
  qbi_qmi_svc_e qmi_svc_id
);

static qbi_qmi_svc_info_s *qbi_qmi_svc_info_by_id
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id
);

static boolean qbi_qmi_wait_for_svc_then_get_info
(
  const qbi_ctx_s            *ctx,
  qmi_idl_service_object_type svc_obj,
  qmi_service_info           *qmi_svc_info
);

static void qbi_qmi_ssr_handle
(
  qbi_ctx_s *ctx
);

static void qbi_qmi_ssr_mbim_open
(
  qbi_ctx_s *ctx
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_qmi_alloc_cb_info
===========================================================================*/
/*!
    @brief Allocates memory to hold QMI ind cb info to pass to QBI task

    @details
    Runs outside the QBI task.

    @param cmd_id QBI task command ID (QBI_TASK_CMD_ID_QMI_RSP or
    QBI_TASK_CMD_ID_QMI_IND)
    @param qmi_handle
    @param qmi_msg_id
    @param ind_buf_len If this is a indication, set to the length of the
    buffer to allocate to hold the indication data. Otherwise, set to 0.

    @return qbi_qmi_cb_info_s*
*/
/*=========================================================================*/
static qbi_qmi_cb_info_s *qbi_qmi_alloc_cb_info
(
  qbi_task_cmd_id_e cmd_id,
  qmi_client_type   qmi_handle,
  uint16            qmi_msg_id,
  uint32            ind_buf_len
)
{
  qbi_qmi_cb_info_s *cb_info = NULL;
/*-------------------------------------------------------------------------*/
  cb_info = (qbi_qmi_cb_info_s *)
    QBI_MEM_MALLOC_CLEAR(sizeof(qbi_qmi_cb_info_s));

  if (cb_info == NULL)
  {
    QBI_LOG_E_0("Failure allocating indication callback info!");
  }
  else
  {
    cb_info->cmd_id = cmd_id;
    cb_info->qmi_handle = qmi_handle;
    cb_info->qmi_msg_id = qmi_msg_id;

    if (cmd_id == QBI_TASK_CMD_ID_QMI_IND)
    {
      qbi_util_buf_init(&cb_info->data.ind.qmi_data);
      if (ind_buf_len > 0 &&
          qbi_util_buf_alloc_dont_clear(
            &cb_info->data.ind.qmi_data, ind_buf_len) == NULL)
      {
        QBI_LOG_E_0("Failure allocating data to store indication info!");
        qbi_qmi_free_cb_info(cb_info);
        cb_info = NULL;
      }
    }
  }

  return cb_info;
} /* qbi_qmi_alloc_cb_info() */

/*===========================================================================
  FUNCTION: qbi_qmi_cmd_cancel_cb
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
static void qbi_qmi_cmd_cancel_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(ctx);
  QBI_ARG_NOT_USED(cmd_id);

  qbi_qmi_free_cb_info((qbi_qmi_cb_info_s *) data);
} /* qbi_qmi_cmd_cancel_cb() */

/*===========================================================================
  FUNCTION: qbi_qmi_client_error_cb
===========================================================================*/
/*!
    @brief Error callback registered with QCCI during client initialization

    @details
    Used to handle modem subsystem restart

    @param user_handle
    @param error
    @param err_cb_data

    @see qmi_client_register_error_cb
*/
/*=========================================================================*/
static void qbi_qmi_client_error_cb
(
 qmi_client_type       user_handle,
 qmi_client_error_type error,
 void                 *err_cb_data
)
{
  qbi_ctx_s *ctx;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(err_cb_data);
  QBI_ARG_NOT_USED(user_handle);
  ctx = (qbi_ctx_s *) err_cb_data;
  if (error == QMI_SERVICE_ERR &&
      !qbi_task_cmd_send(ctx, QBI_TASK_CMD_ID_MODEM_RESTART,
                         qbi_qmi_proc_modem_restart_cb, NULL))
  {
    QBI_LOG_E_0("Couldn't post command to handle modem restart!");
  }
  else
  {
    QBI_LOG_W_1("Ignoring QCCI error %d", error);
  }
} /* qbi_qmi_client_error_cb() */

/*===========================================================================
  FUNCTION: qbi_qmi_decode_ind
===========================================================================*/
/*!
    @brief Decodes a QMI indication from the wire format into its C struct

    @details

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id
    @param wire_format Buffer containing the wire format data
    @param c_struct Buffer to allocate and populate with C struct data.
    If this function returns TRUE, the caller must free this buffer via
    after it is used. If the function returns FALSE, then this buffer will
    be empty.

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_qmi_decode_ind
(
  qbi_ctx_s            *ctx,
  qbi_qmi_svc_e         qmi_svc_id,
  uint16                qmi_msg_id,
  const qbi_util_buf_s *wire_format,
  qbi_util_buf_s       *c_struct
)
{
  int32 idl_err;
  uint32 ind_size;
  boolean success = FALSE;
  const qbi_qmi_svc_info_s *svc_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(wire_format);
  QBI_CHECK_NULL_PTR_RET_FALSE(c_struct);

  svc_info = qbi_qmi_svc_info_by_id(ctx, qmi_svc_id);
  QBI_CHECK_NULL_PTR_RET_FALSE(svc_info);

  qbi_util_buf_init(c_struct);
  if (!qbi_qmi_get_msg_size(
        ctx, qmi_svc_id, qmi_msg_id, QBI_QMI_MSG_TYPE_INDICATION, &ind_size))
  {
    QBI_LOG_E_0("Couldn't get message size!");
  }
  else if (ind_size == 0)
  {
    /* No TLVs to decode if there are none defined in the IDL */
    QBI_LOG_W_2("C struct is empty for indication with service %d message "
                "0x%04x", qmi_svc_id, qmi_msg_id);
    success = TRUE;
  }
  else if (qbi_util_buf_alloc(c_struct, ind_size) == NULL)
  {
    QBI_LOG_E_3("Couldn't allocate memory for QMI indication for svc %d "
                "msg_id 0x%04x (needed %d bytes)", qmi_svc_id, qmi_msg_id,
                ind_size);
  }
  else
  {
    idl_err = qmi_idl_message_decode(svc_info->svc_obj,
                                     QMI_IDL_INDICATION,
                                     qmi_msg_id,
                                     wire_format->data,
                                     wire_format->size,
                                     c_struct->data,
                                     c_struct->size);
    if (idl_err != QMI_NO_ERR)
    {
      QBI_LOG_E_3("Error %d while trying to decode indication for service %d "
                  "message 0x%04x", idl_err, qmi_svc_id, qmi_msg_id);
      qbi_util_buf_free(c_struct);
    }
    else
    {
      success = TRUE;
    }
  }

  return success;
} /* qbi_qmi_decode_ind() */

/*===========================================================================
  FUNCTION: qbi_qmi_free_cb_info
===========================================================================*/
/*!
    @brief Free all memory associated with an ind cb info struct

    @details

    @param cb_info

    @see qbi_qmi_alloc_cb_info
*/
/*=========================================================================*/
static void qbi_qmi_free_cb_info
(
  qbi_qmi_cb_info_s *cb_info
)
{
  QBI_CHECK_NULL_PTR_RET(cb_info);

  if (cb_info->cmd_id == QBI_TASK_CMD_ID_QMI_IND)
  {
    qbi_util_buf_free(&cb_info->data.ind.qmi_data);
  }
  QBI_MEM_FREE(cb_info);
} /* qbi_qmi_free_cb_info() */

/*===========================================================================
  FUNCTION: qbi_qmi_get_service_obj
===========================================================================*/
/*!
    @brief Retrieves the service object for a QMI service

    @details

    @param qmi_svc_id
    @param svc_obj Set to the service object associated with the qmi_svc_id.
    Will be set to NULL on failure.

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_qmi_get_service_obj
(
  qbi_qmi_svc_e                qmi_svc_id,
  qmi_idl_service_object_type *svc_obj
)
{
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(svc_obj);

  switch (qmi_svc_id)
  {
    case QBI_QMI_SVC_DMS:
      *svc_obj = dms_get_service_object_v01();
      break;

    case QBI_QMI_SVC_NAS:
      *svc_obj = nas_get_service_object_v01();
      break;

    case QBI_QMI_SVC_WMS:
      *svc_obj = wms_get_service_object_v01();
      break;

    case QBI_QMI_SVC_UIM:
      *svc_obj = uim_get_service_object_v01();
      break;

    case QBI_QMI_SVC_CAT:
      *svc_obj = cat_get_service_object_v02();
      break;

    case QBI_QMI_SVC_PBM:
      *svc_obj = pbm_get_service_object_v01();
      break;

    case QBI_QMI_SVC_VOICE:
      *svc_obj = voice_get_service_object_v02();
      break;

    case QBI_QMI_SVC_WDA:
      *svc_obj = wda_get_service_object_v01();
      break;

    case QBI_QMI_SVC_SSCTL:
      *svc_obj = ssctl_get_service_object_v01();
      break;

    case QBI_QMI_SVC_DSD:
      *svc_obj = dsd_get_service_object_v01();
      break;

    case QBI_QMI_SVC_PDC:
      *svc_obj = pdc_get_service_object_v01();
      break;

    case QBI_QMI_SVC_SAR:
      *svc_obj = sar_get_service_object_v01();
      break;

    default:
      if (qmi_svc_id >= QBI_QMI_SVC_WDS_FIRST &&
          qmi_svc_id <= QBI_QMI_SVC_WDS_LAST)
      {
        *svc_obj = wds_get_service_object_v01();
      }
      else
      {
        QBI_LOG_E_1("Unrecognized service ID %d", qmi_svc_id);
        *svc_obj = NULL;
      }
  }

  if (*svc_obj == NULL)
  {
    QBI_LOG_E_1("Couldn't get service object for qmi_svc_id %d", qmi_svc_id);
    success = FALSE;
  }

  return success;
} /* qbi_qmi_get_service_obj() */

/*===========================================================================
  FUNCTION: qbi_qmi_handle_to_svc_id
===========================================================================*/
/*!
    @brief Maps a QMI client handle to the associated QMI service ID

    @details

    @param ctx
    @param qmi_handle

    @return qbi_qmi_svc_e QMI service ID, or QBI_QMI_NUM_SVCS if no match
*/
/*=========================================================================*/
static qbi_qmi_svc_e qbi_qmi_handle_to_svc_id
(
  qbi_ctx_s      *ctx,
  qmi_client_type qmi_handle
)
{
  qbi_qmi_svc_e qmi_svc_id = QBI_QMI_NUM_SVCS;
  int i;
/*-------------------------------------------------------------------------*/
  if (ctx == NULL || ctx->qmi_state == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer");
  }
  else
  {
    for (i = 0; i < (int) QBI_QMI_NUM_SVCS; i++)
    {
      if (qbi_qmi_is_svc_ready(&ctx->qmi_state->svc_info[i]) &&
          ctx->qmi_state->svc_info[i].handle == qmi_handle)
      {
        qmi_svc_id = (qbi_qmi_svc_e) i;
        break;
      }
    }
  }

  return qmi_svc_id;
} /* qbi_qmi_handle_to_svc_id() */

/*===========================================================================
  FUNCTION: qbi_qmi_ind_cb
===========================================================================*/
/*!
    @brief Callback handling

    @details
    Runs outside of QBI task, so no QBI internal data should be accessed
    in this function without a mutex. Posts command to QBI task for
    processing.

    @param user_handle
    @param msg_id
    @param ind_buf
    @param ind_buf_len
    @param ind_cb_data

    @see qbi_qmi_proc_ind_cb
*/
/*=========================================================================*/
static void qbi_qmi_ind_cb
(
  qmi_client_type user_handle,
  unsigned int    msg_id,
  void           *ind_buf,
  unsigned int    ind_buf_len,
  void           *ind_cb_data
)
{
  qbi_qmi_cb_info_s *cb_info;
  qbi_ctx_s *ctx;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ind_cb_data);

  ctx = (qbi_ctx_s *) ind_cb_data;
  cb_info = qbi_qmi_alloc_cb_info(QBI_TASK_CMD_ID_QMI_IND, user_handle,
                                  (uint16) msg_id, ind_buf_len);
  if (cb_info != NULL)
  {
    QBI_MEMSCPY(cb_info->data.ind.qmi_data.data, cb_info->data.ind.qmi_data.size,
                ind_buf, ind_buf_len);

    if (!qbi_task_cmd_send(ctx, cb_info->cmd_id, qbi_qmi_proc_ind_cb, cb_info))
    {
      QBI_LOG_E_1("Couldn't send command to QBI task for QMI indication with "
                  "msg_id 0x%04x", msg_id);
      qbi_qmi_free_cb_info(cb_info);
    }
  }
} /* qbi_qmi_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_qmi_init_client
===========================================================================*/
/*!
    @brief Initializes a client handle with a QMI service

    @details
    Performs actual initialization of the QMI service's client handle with
    the underlying QMI client API. It is assumed that before entering this
    function, the QMI service handle's reference count is 0. On success,
    the reference count will be set to 1.

    @param ctx
    @param qmi_svc_id
    @param svc_info

    @return boolean TRUE on success, FALSE on failure

    @see qbi_qmi_release_client
*/
/*=========================================================================*/
static boolean qbi_qmi_init_client
(
  qbi_ctx_s          *ctx,
  qbi_qmi_svc_e       qmi_svc_id,
  qbi_qmi_svc_info_s *svc_info
)
{
  qmi_client_error_type err;
  qmi_service_info qmi_svc_info;
  boolean success = FALSE;
  qmi_client_os_params os_params;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(svc_info);

  if (!qbi_qmi_get_service_obj(qmi_svc_id, &svc_info->svc_obj))
  {
    QBI_LOG_E_1("Couldn't initialize client for unrecognized service ID %d",
                qmi_svc_id);
  }
  else if (!qbi_qmi_wait_for_svc_then_get_info(
             ctx, svc_info->svc_obj, &qmi_svc_info))
  {
    QBI_LOG_E_1("Error getting info for QMI service %d", qmi_svc_id);
  }
  else
  {
    qbi_os_qcci_os_params_init(&os_params);
    err = qmi_client_init(
      &qmi_svc_info, svc_info->svc_obj, qbi_qmi_ind_cb, (void *) ctx,
      &os_params, &svc_info->handle);
    if (err != QMI_NO_ERR)
    {
      QBI_LOG_E_1("Error %d while trying to initialize client", err);
    }
    else
    {
      QBI_LOG_I_1("Successfully allocated client for qmi_svc_id %d",
                  qmi_svc_id);
      svc_info->ref_cnt = 1;
      success = TRUE;

      if (qmi_svc_id == QBI_QMI_SVC_ID_FOR_ERROR_CB)
      {
        (void) qmi_client_register_error_cb(
          svc_info->handle, qbi_qmi_client_error_cb, (void *) ctx);
      }
    }
  }

  return success;
} /* qbi_qmi_init_client() */

/*===========================================================================
  FUNCTION: qbi_qmi_is_svc_ready
===========================================================================*/
/*!
    @brief Checks whether a QMI service handle is ready to receive requests

    @details

    @param svc_info

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_qmi_is_svc_ready
(
  const qbi_qmi_svc_info_s *svc_info
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(svc_info);

  return (svc_info->ref_cnt > 0);
} /* qbi_qmi_is_svc_ready() */

/*===========================================================================
  FUNCTION: qbi_qmi_proc_ind_cb
===========================================================================*/
/*!
    @brief Processes a QMI indication callback

    @details
    Runs in the QBI thread.

    @param ctx
    @param cmd_id
    @param data

    @see qbi_qmi_ind_cb
*/
/*=========================================================================*/
static void qbi_qmi_proc_ind_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
  qbi_qmi_svc_e qmi_svc_id;
  qbi_qmi_cb_info_s *cb_info;
  qbi_util_buf_s ind_c_struct;
  const qbi_util_buf_const_s *ind_c_struct_const;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(data);
  QBI_ARG_NOT_USED(cmd_id);

  cb_info = (qbi_qmi_cb_info_s *) data;
  qmi_svc_id = qbi_qmi_handle_to_svc_id(ctx, cb_info->qmi_handle);
  if (qbi_qmi_decode_ind(
        ctx, qmi_svc_id, cb_info->qmi_msg_id, &cb_info->data.ind.qmi_data,
        &ind_c_struct))
  {
    QBI_LOG_I_2("Processing QMI indication with svc_id %d msg_id 0x%04x",
                qmi_svc_id, cb_info->qmi_msg_id);
    ind_c_struct_const = (const qbi_util_buf_const_s *) &ind_c_struct;
    qbi_svc_handle_qmi_ind(
      ctx, qmi_svc_id, cb_info->qmi_msg_id, ind_c_struct_const);
    qbi_util_buf_free(&ind_c_struct);
  }

  qbi_qmi_free_cb_info(cb_info);
} /* qbi_qmi_proc_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_qmi_proc_modem_restart_cb
===========================================================================*/
/*!
    @brief Processes a modem susbsystem restart notification from the QBI
    task's context

    @details
    Notifies the host of the error by sending an MBIM_FUNCTION_ERROR_MSG

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_qmi_proc_modem_restart_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
  qbi_qmi_svc_info_s *svc_info;
  uint8 ssr_enable = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_ARG_NOT_USED(cmd_id);
  QBI_ARG_NOT_USED(data);

  QBI_LOG_E_1("Received modem restart notification on context ID %d", ctx->id);
  svc_info = qbi_qmi_svc_info_by_id(ctx, QBI_QMI_SVC_ID_FOR_ERROR_CB);
  if (!qbi_qmi_is_svc_ready(svc_info))
  {
    QBI_LOG_E_0("QMI not initialized - ignoring notification");
  }
  else
  {
    /* Send an UNKNOWN error to the host; it should reply with RESET_FUNCTION,
       and reinitialize QBI with MBIM_OPEN */
    qbi_msg_send_error(ctx, 0, QBI_MBIM_ERROR_NOT_OPENED);
    /* Is SSR functionality enabled */
    if (!qbi_nv_store_cfg_item_read(ctx, QBI_NV_STORE_CFG_ITEM_SSR_ENABLE,
      &ssr_enable, sizeof(ssr_enable)))
    {
      QBI_LOG_W_0("Unable to read SSR supported NV.");
    }

    if (ssr_enable)
    {
      QBI_LOG_I_0("SSR event received. Proceed for re-init.");
      qbi_qmi_ssr_handle(ctx);
    }
    else
    {
      QBI_LOG_I_0("SSR is NOT supported.");
    }
  }
} /* qbi_qmi_proc_modem_restart_cb() */

/*===========================================================================
  FUNCTION: qbi_qmi_proc_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI response callback in the QBI task

    @details

    @param ctx
    @param cmd_id
    @param data

    @see qbi_qmi_rsp_cb
*/
/*=========================================================================*/
static void qbi_qmi_proc_rsp_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
  qbi_qmi_cb_info_s *cb_info;
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_qmi_rsp_cb_f *rsp_cb = NULL;
  boolean txn_freed;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(data);
  QBI_ARG_NOT_USED(cmd_id);

  cb_info = (qbi_qmi_cb_info_s *) data;
  qmi_txn = cb_info->data.rsp.txn;
  QBI_CHECK_NULL_PTR_RET(qmi_txn);

  QBI_LOG_I_2("Processing QMI response for svc_id %d msg_id 0x%04x",
              qmi_txn->svc_id, qmi_txn->msg_id);
  if (!qbi_qmi_txn_sanity_check_rsp(
        qmi_txn, cb_info->data.rsp.msg_id, cb_info->data.rsp.rsp_buf,
        cb_info->data.rsp.rsp_buf_len))
  {
    QBI_LOG_E_0("QMI transaction sanity check failed!");
  }
  else if (!qbi_util_list_remove(&ctx->pend_qmi_txns, qmi_txn))
  {
    /* This is a serious error, possible causes include receiving the same
       response twice, a bug in QBI, or possible memory corruption. */
    QBI_LOG_E_0("Couldn't remove QMI transaction from context's pending list!");
  }
  else if (qmi_txn->parent == NULL)
  {
    /* Handle the special case of parent transaction release before a QMI
       response is received. All we need to do is free the qmi_txn */
    QBI_LOG_W_0("QMI transaction's parent gone - dropping QMI response");
    qbi_qmi_txn_free(qmi_txn);
  }
  else if (cb_info->data.rsp.transp_err != QMI_NO_ERR)
  {
    /* Couldn't send QMI request, so abort the entire QBI transaction */
    QBI_LOG_E_1("QMI transport gave error %d", cb_info->data.rsp.transp_err);
    /* Make sure the status is updated so release of the parent transasction
       will result in release of this QMI transaction. */
    qmi_txn->status = QBI_QMI_TXN_STATUS_RSP_RCVD;
    qbi_svc_abort_txn(qmi_txn->parent, QBI_MBIM_STATUS_FAILURE);
  }
  else
  {
    if (qmi_txn->parent->qmi_txns_pending > 0)
    {
      qmi_txn->parent->qmi_txns_pending--;
    }
    else
    {
      QBI_LOG_E_0("Received QMI response when qmi_txns_pending == 0!!!");
    }

    if (!qbi_os_fptr_decode(&qmi_txn->cb, (qbi_os_void_f **) &rsp_cb, FALSE))
    {
      QBI_LOG_E_0("Unable to decode function pointer");
      qbi_qmi_txn_free(qmi_txn);
    }
    else
    {
      qmi_txn->status = QBI_QMI_TXN_STATUS_RSP_RCVD;
      txn_freed = qbi_svc_proc_action(qmi_txn->parent, rsp_cb(qmi_txn));

      /* If the transaction was not freed in qbi_svc_proc_action, and the
         keep flag for the QMI transaction is not set, then release the QMI
         transaction here - it's memory is no longer needed. */
      if (!txn_freed && !qmi_txn->keep_after_rsp_cb)
      {
        qbi_qmi_txn_free(qmi_txn);
      }
    }
  }

  qbi_qmi_free_cb_info(cb_info);
} /* qbi_qmi_proc_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_qmi_release_client
===========================================================================*/
/*!
    @brief Releases a QMI service handle

    @details
    Sets the reference count for the handle to 0.

    @param ctx
    @param svc_info
*/
/*=========================================================================*/
static void qbi_qmi_release_client
(
  qbi_ctx_s          *ctx,
  qbi_qmi_svc_info_s *svc_info
)
{
  qmi_client_error_type err;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(svc_info);

  err = qmi_client_release(svc_info->handle);
  if (err != QMI_NO_ERR)
  {
    QBI_LOG_E_1("Error %d while releasing client", err);
  }
  svc_info->ref_cnt = 0;
} /* qbi_qmi_release_client() */

/*===========================================================================
  FUNCTION: qbi_qmi_rsp_cb
===========================================================================*/
/*!
    @brief Callback received to process QMI responses

    @details
    Runs outside QBI task.

    @param user_handle
    @param msg_id
    @param rsp_buf QMI response data, in C struct format
    @param rsp_buf_len Length of response data
    @param user_data User data passed to qmi_client_send_msg_async. In
    this case, it should be the associated qbi_qmi_txn_s
    @param transp_err Error code from QMI transport layer

    @see qbi_qmi_proc_rsp_cb
*/
/*=========================================================================*/
static void qbi_qmi_rsp_cb
(
  qmi_client_type        user_handle,
  unsigned int           msg_id,
  void                  *rsp_buf,
  unsigned int           rsp_buf_len,
  void                  *user_data,
  qmi_client_error_type  transp_err
)
{
  qbi_qmi_cb_info_s *cb_info;
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(user_data);

  qmi_txn = (qbi_qmi_txn_s *) user_data;
  if (!qbi_qmi_txn_sanity_check_rsp(qmi_txn, msg_id, rsp_buf, rsp_buf_len))
  {
    /* This signals a bug in the qmi_client API (returning wrong resp_cb_data),
       a bug in the QBI handler (didn't set cb func or modified data after
       sending request), or possibly memory corruption. */
    QBI_LOG_E_1("QMI transaction sanity check failed! (msg_id 0x%04x)", msg_id);
  }
  else
  {
    /* Note that if anything fails beyond this point, the QMI transaction will
       only be freed by the timeout */
    cb_info = qbi_qmi_alloc_cb_info(QBI_TASK_CMD_ID_QMI_RSP, user_handle,
                                    (uint16) msg_id, 0);
    if (cb_info != NULL)
    {
      cb_info->data.rsp.txn         = qmi_txn;
      cb_info->data.rsp.transp_err  = transp_err;
      cb_info->data.rsp.msg_id      = msg_id;
      cb_info->data.rsp.rsp_buf     = rsp_buf;
      cb_info->data.rsp.rsp_buf_len = rsp_buf_len;

      if (!qbi_task_cmd_send(qmi_txn->ctx, cb_info->cmd_id, qbi_qmi_proc_rsp_cb,
                             cb_info))
      {
        qbi_qmi_free_cb_info(cb_info);
      }
    }
  }
} /* qbi_qmi_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_qmi_svc_id_is_valid
===========================================================================*/
/*!
    @brief Performs range checking of a qbi_qmi_svc_e value

    @details

    @param qmi_svc_id

    @return boolean TRUE if in valid range, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_qmi_svc_id_is_valid
(
  qbi_qmi_svc_e qmi_svc_id
)
{
  return (qmi_svc_id < QBI_QMI_NUM_SVCS);
} /* qbi_qmi_svc_id_is_valid() */

/*===========================================================================
  FUNCTION: qbi_qmi_svc_info_by_id
===========================================================================*/
/*!
    @brief Return the service information pointer for a given service ID

    @details

    @param ctx
    @param qmi_svc_id

    @return qbi_qmi_svc_info_s*
*/
/*=========================================================================*/
static qbi_qmi_svc_info_s *qbi_qmi_svc_info_by_id
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id
)
{
  qbi_qmi_svc_info_s *svc_info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(ctx->qmi_state);

  if (qbi_qmi_svc_id_is_valid(qmi_svc_id))
  {
    svc_info = &ctx->qmi_state->svc_info[qmi_svc_id];
  }

  return svc_info;
} /* qbi_qmi_svc_info_by_id() */

/*===========================================================================
  FUNCTION: qbi_qmi_wait_for_svc_then_get_info
===========================================================================*/
/*!
    @brief Waits for a QMI service to become ready, then fetches the
    qmi_service_info data associated with the QMI service on the connection
    QBI uses

    @details

    @param ctx
    @param svc_obj
    @param qmi_svc_info Will be populated with QMI service information to
    be used in a call to qmi_client_init()

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_qmi_wait_for_svc_then_get_info
(
  const qbi_ctx_s            *ctx,
  qmi_idl_service_object_type svc_obj,
  qmi_service_info           *qmi_svc_info
)
{
  qmi_client_os_params os_params;
  qmi_client_type notifier_handle;
  qmi_client_error_type err;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_svc_info);

  qbi_os_qcci_os_params_init(&os_params);
  err = qmi_client_notifier_init(svc_obj, &os_params, &notifier_handle);
  if (err != QMI_NO_ERR)
  {
    QBI_LOG_E_1("Error %d while trying to initialize client notifier", err);
  }
  else
  {
    /* QMI QMUX may not be available when QMI_CCI_OS_SIGNAL_WAIT is unblocked
       by signal for non-QMUX transport. Additional attempts are necessary in
       order to get qmi service instance over QMUX. */
    while (success != TRUE)
    {
      QMI_CCI_OS_SIGNAL_WAIT(&os_params, QBI_QMI_SVC_INIT_TIMEOUT_MS);
      if (QMI_CCI_OS_SIGNAL_TIMED_OUT(&os_params))
      {
        QBI_LOG_E_0("Timeout waiting for signal");
        break;
      }

      QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
      err = qmi_client_get_service_instance(
        svc_obj, qbi_os_qcci_get_service_instance(ctx), qmi_svc_info);
      if (err != QMI_NO_ERR)
      {
        QBI_LOG_E_1("Error %d while trying to get service info", err);
      }
      else
      {
        success = TRUE;
      }
    }

    /*! @note The notifier handle is similar to a client but it has very limited
        functionality, which we only use to wait for the service to become
        ready. Once we are done waiting, we need to release it using the same
        function as used for a regular client handle. This must happen after
        calling qmi_client_get_service_instance(), otherwise that function will
        fail with QMI_SERVICE_ERR. */
    err = qmi_client_release(notifier_handle);
    if (err != QMI_NO_ERR)
    {
      QBI_LOG_W_1("Error %d while releasing client notifier handle", err);
    }
  }

  return success;
} /* qbi_qmi_wait_for_svc_then_get_info() */


/*=============================================================================

  Public Function Definitions

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_qmi_alloc_svc_handle
===========================================================================*/
/*!
    @brief Initializes a QMI service handle

    @details
    QMI service handles are maintained per QBI context, and are a
    shared resource between MBIM device services. When a MBIM device
    service is opened, it will request one or more QMI service client
    handles be initialized, and the reverse when it is closed. A reference
    count is maintained to determine when actual (de)allocation of the
    service client handle should be performed.

    @param ctx
    @param qmi_svc_id

    @return boolean TRUE on success, FALSE on failure

    @see qbi_qmi_release_svc_handle
*/
/*=========================================================================*/
boolean qbi_qmi_alloc_svc_handle
(
  qbi_ctx_s     *ctx,
  qbi_qmi_svc_e  qmi_svc_id
)
{
  qbi_qmi_svc_info_s *svc_info;
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);

  svc_info = qbi_qmi_svc_info_by_id(ctx, qmi_svc_id);
  if (svc_info == NULL)
  {
    QBI_LOG_E_1("Failure looking up service information for service %d",
                qmi_svc_id);
  }
  else if (qbi_qmi_is_svc_ready(svc_info))
  {
    svc_info->ref_cnt++;
    result = TRUE;
    QBI_LOG_I_2("Increment reference count for qmi_svc_id %d to %d", qmi_svc_id,
                svc_info->ref_cnt);
  }
  else
  {
    QBI_LOG_I_1("Acquiring new QMI client handle for qmi_svc_id %d",
                qmi_svc_id);
    result = qbi_qmi_init_client(ctx, qmi_svc_id, svc_info);
  }

  return result;
} /* qbi_qmi_alloc_svc_handle() */

/*===========================================================================
  FUNCTION: qbi_qmi_close
===========================================================================*/
/*!
    @brief Release the QMI state for the given context

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_qmi_close
(
  qbi_ctx_s *ctx
)
{
  qbi_qmi_svc_e qmi_svc;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(ctx->qmi_state);

  /* Release all active QMI client handles */
  for (qmi_svc = QBI_QMI_SVC_FIRST; qmi_svc < QBI_QMI_NUM_SVCS; qmi_svc++)
  {
    if (ctx->qmi_state->svc_info[qmi_svc].ref_cnt > 0)
    {
      ctx->qmi_state->svc_info[qmi_svc].ref_cnt = 1;
      qbi_qmi_release_svc_handle(ctx, qmi_svc);
    }
  }

  QBI_MEMSET(ctx->qmi_state, 0, sizeof(qbi_qmi_state_s));
  QBI_MEM_FREE(ctx->qmi_state);
  ctx->qmi_state = NULL;

  /* Close the QCCI transport */
  if (!qbi_os_qcci_teardown(ctx))
  {
    QBI_LOG_W_0("QCCI teardown failed");
  }

  /* Cancel all pending QMI response callbacks, since they reference dynamic QMI
     transaction data that will be released. Note that QMI indication
     callbacks don't need to be cancelled since they don't hold references
     to freed memory; also they will be aborted during qbi_qmi_proc_ind_cb()
     because ctx->qmi_state is NULL.
     We meet the requirement that no new commands of this type can be added
     during processing because the sanity check in qbi_qmi_rsp_cb() ensures
     that ctx->qmi_state != NULL (which was just set to NULL above) before
     posting the command. */
  qbi_task_cmd_cancel(ctx, QBI_TASK_CMD_ID_QMI_RSP, qbi_qmi_cmd_cancel_cb);
} /* qbi_qmi_close() */

/*===========================================================================
  FUNCTION: qbi_qmi_get_msg_size
===========================================================================*/
/*!
    @brief Determine the amount of memory needed to allocate to hold the
    decoded (IDL C structure) representation of a QMI message

    @details

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id
    @param qmi_msg_type
    @param msg_size Variable to populate with the size of the request (may
    be 0 if no request TLVs)

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_qmi_get_msg_size
(
  qbi_ctx_s         *ctx,
  qbi_qmi_svc_e      qmi_svc_id,
  uint16             qmi_msg_id,
  qbi_qmi_msg_type_e qmi_msg_type,
  uint32            *msg_size
)
{
  boolean success = FALSE;
  const qbi_qmi_svc_info_s *svc_info;
  int32 err;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(msg_size);

  svc_info = qbi_qmi_svc_info_by_id(ctx, qmi_svc_id);
  if (!qbi_qmi_is_svc_ready(svc_info))
  {
    QBI_LOG_E_1("QMI service ID %d not ready!", qmi_svc_id);
  }
  else
  {
    err = qmi_idl_get_message_c_struct_len(
      svc_info->svc_obj, (qmi_idl_type_of_message_type) qmi_msg_type,
      qmi_msg_id, (uint32_t *) msg_size);
    if (err != QMI_NO_ERR)
    {
      QBI_LOG_E_4("Error %d while trying to get msg type %d size for svc %d "
                  "msg_id 0x%02x", err, qmi_msg_type, qmi_svc_id, qmi_msg_id);
      *msg_size = 0;
    }
    else
    {
      success = TRUE;
    }
  }

  return success;
} /* qbi_qmi_get_msg_size() */

/*===========================================================================
  FUNCTION: qbi_qmi_get_qmi_txn_id
===========================================================================*/
/*!
    @brief Retrieves a QMI transaction ID

    @details
    The transaction ID returned by this function is the one used in the QMUX
    protocol. It can be used with QMI abort requests, such as QMI_WDS_ABORT.

    @param ctx
    @param qmi_svc_id
    @param qmi_txn_handle
    @param txn_id

    @return boolean TRUE on success, FALSE on failure. txn_id will only be
    modified if this function returns TRUE.
*/
/*=========================================================================*/
boolean qbi_qmi_get_qmi_txn_id
(
  qbi_ctx_s       *ctx,
  qbi_qmi_svc_e    qmi_svc_id,
  qmi_txn_handle   qmi_txn_handle,
  uint16          *txn_id
)
{
  boolean success = FALSE;
  qbi_qmi_svc_info_s *svc_info;
  qmi_client_error_type qmi_err;
  uint32_t qmi_txn_id = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn_id);

  svc_info = qbi_qmi_svc_info_by_id(ctx, qmi_svc_id);
  QBI_CHECK_NULL_PTR_RET_FALSE(svc_info);

  qmi_err = qmi_client_get_async_txn_id(
    svc_info->handle, qmi_txn_handle, &qmi_txn_id);
  if (qmi_err != QMI_NO_ERR)
  {
    QBI_LOG_E_1("Couldn't get QMI transaction ID: error %d", qmi_err);
  }
  else
  {
    /* QMI transaction IDs are uint16 values in the QMUX protocol, so casting
       down from uint32 is safe here. */
    *txn_id = (uint16) qmi_txn_id;
    success = TRUE;
  }

  return success;
} /* qbi_qmi_get_qmi_txn_id() */

/*===========================================================================
  FUNCTION: qbi_qmi_open
===========================================================================*/
/*!
    @brief Initialize the QMI state for the given QBI context

    @details

    @param ctx

    @return boolean
*/
/*=========================================================================*/
boolean qbi_qmi_open
(
  qbi_ctx_s *ctx
)
{
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);

  if (ctx->qmi_state != NULL)
  {
    /* Either we received a MBIM_OPEN from the host while processing another
       MBIM_OPEN, or something went horribly wrong in QBI. */
    QBI_LOG_E_0("QMI state not null!");
  }
  else if (!qbi_os_qcci_setup(ctx))
  {
    QBI_LOG_E_0("QCCI setup failed");
  }
  else
  {
    ctx->qmi_state = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_qmi_state_s));
    if (ctx->qmi_state == NULL)
    {
      QBI_LOG_E_0("Couldn't allocate memory for QMI state");
    }
    else
    {
      success = TRUE;
    }
  }

  return success;
} /* qbi_qmi_open() */

/*===========================================================================
  FUNCTION: qbi_qmi_release_svc_handle
===========================================================================*/
/*!
    @brief Release interest in a QMI service handle

    @details
    The service handle contains a reference count and may not actually
    be released.

    @param ctx
    @param qmi_svc_id

    @see qbi_qmi_alloc_svc_handle
*/
/*=========================================================================*/
void qbi_qmi_release_svc_handle
(
  qbi_ctx_s     *ctx,
  qbi_qmi_svc_e  qmi_svc_id
)
{
  qbi_qmi_svc_info_s *svc_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  svc_info = qbi_qmi_svc_info_by_id(ctx, qmi_svc_id);
  if (svc_info == NULL)
  {
    QBI_LOG_E_1("Failure looking up service information for service %d",
                qmi_svc_id);
  }
  else if (!qbi_qmi_is_svc_ready(svc_info))
  {
    QBI_LOG_E_1("Release of inactive service handle for service %d",
                qmi_svc_id);
  }
  else if (svc_info->ref_cnt > 1)
  {
    svc_info->ref_cnt--;
    QBI_LOG_I_2("Decrement reference count for qmi_svc_id %d to %d",
                qmi_svc_id, svc_info->ref_cnt);
  }
  else
  {
    QBI_LOG_I_1("Releasing client handle for qmi_svc_id %d", qmi_svc_id);
    qbi_qmi_release_client(ctx, svc_info);
  }
} /* qbi_qmi_release_svc_handle() */

/*===========================================================================
  FUNCTION: qbi_qmi_dispatch
===========================================================================*/
/*!
    @brief Dispatches QMI transactions associated with a QBI transaction

    @details
    All qbi_qmi_txn_s entities on the qbi_txn_s's list with status set to
    QBI_QMI_TXN_STATUS_AWAITING_DISPATCH will be placed on the context's
    pending QMI transaction list, and dispatched to the modem.

    @param txn

    @return boolean FALSE if a QMI request could not be dispatched due to
    an error, otherwise TRUE
*/
/*=========================================================================*/
boolean qbi_qmi_dispatch
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_util_list_iter_s iter;
  boolean at_least_one_dispatched = FALSE;
  boolean result = TRUE;
  qbi_qmi_svc_info_s *svc_info;
  qmi_client_error_type err;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_LOG_I_1("Dispatching QMI requests for QBI transaction iid %d", txn->iid);

  qbi_util_list_iter_init(&txn->qmi_txns, &iter);
  while (qbi_util_list_iter_has_next(&iter))
  {
    qmi_txn = (qbi_qmi_txn_s *) qbi_util_list_iter_next(&iter);
    if (qmi_txn != NULL &&
        qmi_txn->status == QBI_QMI_TXN_STATUS_NOT_DISPATCHED)
    {
      svc_info = qbi_qmi_svc_info_by_id(txn->ctx, qmi_txn->svc_id);
      if (!qbi_qmi_is_svc_ready(svc_info))
      {
        QBI_LOG_E_1("QMI service ID %d not ready to receive requests!",
                    qmi_txn->svc_id);
        result = FALSE;
        break;
      }
      else
      {
        /* Update the status before issuing the request - this way we can
           perform the sanity check in the initial response callback (outside of
           the QBI task). */
        qmi_txn->status = QBI_QMI_TXN_STATUS_WAITING_RSP;
        err = qmi_client_send_msg_async(svc_info->handle, qmi_txn->msg_id,
                                        qmi_txn->req.data, qmi_txn->req.size,
                                        qmi_txn->rsp.data, qmi_txn->rsp.size,
                                        qbi_qmi_rsp_cb, qmi_txn,
                                        &qmi_txn->txn_handle);
        if (err != QMI_NO_ERR)
        {
          QBI_LOG_E_1("Error %d while attempting to send QMI request", err);
          result = FALSE;

          /* Reset the status to indicate that it is OK to free this QMI
             transaction */
          qmi_txn->status = QBI_QMI_TXN_STATUS_NOT_DISPATCHED;
          break;
        }
        else
        {
          if (!qbi_util_list_push_back(&txn->ctx->pend_qmi_txns, qmi_txn))
          {
            QBI_LOG_E_0("Couldn't add transaction to pending list!");
            result = FALSE;
            break;
          }
          txn->qmi_txns_pending++;
          QBI_LOG_I_2("Sent QMI request for svc_id %d msg_id 0x%04x",
                      qmi_txn->svc_id, qmi_txn->msg_id);
          at_least_one_dispatched = TRUE;
        }
      }
    }
  }

  if (!at_least_one_dispatched)
  {
    QBI_LOG_E_0("No QMI requests dispatched!");
    result = FALSE;
  }

  return result;
} /* qbi_qmi_dispatch() */

/*===========================================================================
  FUNCTION: qbi_qmi_ssr_handle
===========================================================================*/
/*!
    @brief Triggers internal MBIM_CLOSE and MBIM_OPEN
           Triggers QBI events to host

    @details
    Used to handle modem subsystem restart

    @param ctx

    @return void
*/
/*=========================================================================*/
void qbi_qmi_ssr_handle
(
  qbi_ctx_s  *ctx
)
{
  qbi_svc_bc_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  /* Setting SSR flag to TRUE, to indicate SSR is in progress */
  ctx->ssr_info.ssr_in_progress = TRUE;
  ctx->ssr_info.ssr_curr_radio_state = cache->radio_state.sw_radio_state;

  QBI_LOG_D_2("SSR:: ssr in progress flag = %d curr radio state = %d",
    ctx->ssr_info.ssr_in_progress , ctx->ssr_info.ssr_curr_radio_state);

  qbi_qmi_internal_pre_mbim_open_notifications(ctx);
  QBI_LOG_D_0("SSR::Manual MBIM_CLOSE");
  qbi_msg_context_close(ctx);
  qbi_qmi_internal_mbim_open(ctx);

  return;
} /* qbi_qmi_ssr_handle */

/*===========================================================================
  FUNCTION: qbi_qmi_internal_mbim_open
===========================================================================*/
/*!
    @brief Triggers internal MBIM_CLOSE and MBIM_OPEN

    @details
    Used to handle modem subsystem restart

    @param ctx

    @return void
*/
/*=========================================================================*/
void qbi_qmi_internal_mbim_open
(
  qbi_ctx_s  *ctx
)
{
  qbi_msg_open_req_s                              *open_req;
  qbi_util_buf_s                                   buf;
/*-------------------------------------------------------------------------*/
   QBI_CHECK_NULL_PTR_RET(ctx);

  qbi_util_buf_init(&buf);
  open_req = (qbi_msg_open_req_s *) qbi_util_buf_alloc(
    &buf,sizeof(qbi_msg_open_req_s));
  QBI_CHECK_NULL_PTR_RET(open_req);

  open_req->hdr.msg_type = QBI_MSG_MBIM_OPEN_MSG;
  open_req->hdr.txn_id = QBI_QMI_POST_INTERNAL_MBIM_OPEN_TX_ID;
  open_req->hdr.msg_len = sizeof(qbi_msg_open_req_s);
  open_req->max_xfer = QBI_QMI_MAX_CONTROL_SIZE;

  QBI_LOG_STR_1("%s : Manual MBIM_OPEN ",
                 qbi_msg_log_for_recovery_function(ctx));
  qbi_msg_handle_open(ctx, buf.data, buf.size);
  qbi_util_buf_free(&buf);

  return;
} /* qbi_qmi_internal_mbim_open */

/*===========================================================================
  FUNCTION: qbi_qmi_internal_pre_mbim_open_notifications
===========================================================================*/
/*!
    @brief Dispatches QBI evnets to host

    @details
    Used to handle modem subsystem restart

    @param ctx

    @return void
*/
/*=========================================================================*/
void qbi_qmi_internal_pre_mbim_open_notifications
(
  qbi_ctx_s  *ctx
)
{
  qbi_svc_bc_radio_state_rsp_s                *rsp_radio = NULL;
  qbi_svc_bc_subscriber_ready_status_rsp_s    *rsp_subready = NULL;
  qbi_svc_bc_connect_q_req_s                  *rsp_connect = NULL;
  qbi_txn_s                                   *txn_radio = NULL;
  qbi_txn_s                                   *txn_subready = NULL;
  qbi_txn_s                                   *txn_connect = NULL;
  uint32                                       session_id = 0;
  uint32                                       activation_state = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  QBI_LOG_STR_1("%s : pre mbim open triggered.",
                 qbi_msg_log_for_recovery_function(ctx));
  if (QBI_SVC_BC_RADIO_STATE_RADIO_ON == ctx->ssr_info.ssr_curr_radio_state)
  {
    /* Forcing Radio State event,for OS to know that modem is offline*/
    txn_radio = qbi_txn_alloc_event(ctx,
      QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_RADIO_STATE);
    QBI_CHECK_NULL_PTR_RET(txn_radio);
    /* Allocate the fixed-length portion of the response now */
    rsp_radio = qbi_txn_alloc_rsp_buf(txn_radio,
      sizeof(qbi_svc_bc_radio_state_rsp_s));
    QBI_CHECK_NULL_PTR_RET(rsp_radio);

    rsp_radio->hw_radio_state = QBI_SVC_BC_RADIO_STATE_RADIO_ON;
    rsp_radio->sw_radio_state = QBI_SVC_BC_RADIO_STATE_RADIO_OFF;
    (void)qbi_svc_proc_action(txn_radio, QBI_SVC_ACTION_SEND_RSP);
  }
  /* Forcing subscriber_ready state event,for OS to know that
     subscription is not_init */
  txn_subready = qbi_txn_alloc_event(ctx,
    QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS);
  QBI_CHECK_NULL_PTR_RET(txn_subready);

  /* Allocate the fixed-length portion of the response now */
  rsp_subready = qbi_txn_alloc_rsp_buf(txn_subready,
    sizeof(qbi_svc_bc_subscriber_ready_status_rsp_s));
  QBI_CHECK_NULL_PTR_RET(rsp_subready);

  rsp_subready->ready_state = QBI_SVC_BC_READY_STATE_NOT_INITIALIZED;
  (void)qbi_svc_proc_action(txn_subready, QBI_SVC_ACTION_SEND_RSP);

  /* Forcing event(deactivated state) to host regarding all active
     data calls */
  for (session_id = 0; session_id < qbi_svc_bc_max_sessions_get(ctx);
       session_id++)
  {
    activation_state = qbi_svc_bc_connect_session_activation_state(
      ctx, session_id);
    /* Forcing CONNECT event only when connection state is activated */
    /* Forcing CONNECT event if mbim daemon was killed becasue its a new
       instance of mbim process having default state DEACTIVATED */
    if ((activation_state == QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED) ||
        (TRUE == ctx->mbim_recovery))
    {
      /* Forcing CONNECT event, for OS to know that data call disconnect*/
      txn_connect = qbi_txn_alloc_event(ctx,
        QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_CONNECT);
      QBI_CHECK_NULL_PTR_RET(txn_connect);

      /* Allocate the fixed-length portion of the response now */
      rsp_connect = qbi_txn_alloc_rsp_buf(txn_connect, 
        sizeof(qbi_svc_bc_connect_q_req_s));
      QBI_CHECK_NULL_PTR_RET(rsp_connect);

      rsp_connect->session_id = session_id;
      rsp_connect->activation_state = QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED;
      QBI_LOG_D_2("session_id = %d activation_state = %d",
                  rsp_connect->session_id, rsp_connect->activation_state);
      (void)qbi_svc_proc_action(txn_connect, QBI_SVC_ACTION_SEND_RSP);
    }
  }

  return;
} /* qbi_qmi_internal_pre_mbim_open_notifications */

/*===========================================================================
  FUNCTION: qbi_qmi_internal_post_mbim_open_notifications
===========================================================================*/
/*!
    @brief Dispatches QMI request
           Dispatches QBI evnets to host

    @details
    Used to handle modem subsystem restart

    @param ctx

    @return void
*/
/*=========================================================================*/
void  qbi_qmi_internal_post_mbim_open_notifications
(
  qbi_ctx_s  *ctx
)
{
  qbi_txn_s                                   *txn_radio_set = NULL;
  qbi_txn_s                                   *txn_radio = NULL;
  qbi_svc_bc_radio_state_s_req_s              *req_radio_state = NULL;
  qbi_txn_s                                   *txn_subready = NULL;
  qbi_svc_bc_subscriber_ready_status_rsp_s    *rsp_subready = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);

  QBI_LOG_STR_1("%s : post mbim open triggered.",
                qbi_msg_log_for_recovery_function(ctx));
  /* If radio state is ON then trigger set request */
  if (QBI_SVC_BC_RADIO_STATE_RADIO_ON == ctx->ssr_info.ssr_curr_radio_state)
  {
    /* Triggering Radio State Set, to bring the modem online*/
    txn_radio_set = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0,
      QBI_TXN_CMD_TYPE_INTERNAL, QBI_SVC_BC_MBIM_CID_RADIO_STATE, 0, NULL);
    QBI_CHECK_NULL_PTR_RET(txn_radio_set);

    txn_radio_set->req.data =
      QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_bc_radio_state_s_req_s));
    QBI_CHECK_NULL_PTR_RET(txn_radio_set->req.data);

    req_radio_state = (qbi_svc_bc_radio_state_s_req_s *) txn_radio_set->req.data;
    req_radio_state->radio_state = QBI_SVC_BC_RADIO_STATE_RADIO_ON;
    QBI_LOG_D_1("SSR::Triggering Radio state ON = %d",
      req_radio_state->radio_state);
    (void) qbi_svc_proc_action(txn_radio_set,
    qbi_svc_bc_radio_state_s_req(txn_radio_set));
  }

  /* Triggering Radio State Event, For OS to know that modem is back online*/
  txn_radio = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_EVENT,
    QBI_SVC_BC_MBIM_CID_RADIO_STATE, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn_radio);

  (void) qbi_svc_proc_action(txn_radio, qbi_svc_bc_radio_state_q_req(txn_radio));

  /* Triggering Subscriber ready state Event, for OS to know that subscription
     is available*/
  txn_subready = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_EVENT,
    QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn_subready);

  (void) qbi_svc_proc_action(txn_subready,
    qbi_svc_bc_sim_subscriber_ready_status_q_req(txn_subready));

  /* Setting SSR/recovery flag to FALSE indicating that SSR/recovery is complete */
    if(ctx->mbim_recovery)
    {
      ctx->mbim_recovery = FALSE;
      QBI_LOG_D_1("MBIMD_RECOVERY : flag set to %d",ctx->mbim_recovery);
    }
    else
    {
      ctx->ssr_info.ssr_in_progress = FALSE;
      QBI_LOG_D_1("SSR::SSR is completed , flag is set to %d",
        ctx->ssr_info.ssr_in_progress);
    }

  return;
} /* qbi_qmi_internal_post_mbim_open_notifications */
