/*!
  @file
  qbi_svc_mssar.c

  @brief
  Microsoft Selective Absorption Rate device service implementation, based on 
  the "Mobile Broadband Device Selective Absorption Rate (SAR) platfor support" 
  document from Microsoft. This device service allows host to query and set 
  Selective Absorption Rate on device.
*/

/*=============================================================================

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
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
09/05/18  sk   Modified the timer delete logic
07/31/18  sk   Channel notification and hysteresis timer range
07/25/18  mm   Sending Tx request for all WDS Clients
04/24/18  vk   Added input validation check for SAR Config
03/30/18  vk   Added input validation check for channel notification
01/15/18  mm   Added condition in SAR config to check element count
12/18/17  rv   Add logic to delete timer
08/11/17  mm   Fixed issue related to SAR transmission off indication
07/14/17  mm   SAR hysteresis timer range input validation
07/10/17  mm   Fixed issue related to SAR transmission notification
05/30/17  vk   Removal of duplicate rsp assignment
01/14/17  vk   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_mssar.h"
#include "qbi_svc_mssar_mbim.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_txn.h"

#include "wireless_data_service_v01.h"
#include "specific_absorption_rate_v01.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! This macro statically defines a QMI indication handler and fills in the
    fields that are common to all handlers in this device service */
#define QBI_SVC_MSSAR_STATIC_IND_HDLR(qmi_svc_id, qmi_msg_id, cid, cb) \
  {qmi_svc_id, qmi_msg_id, cid, cb, NULL}

/*! Map session ID to QMI WDS IPv4 service ID */
#define QBI_SVC_MSSAR_SESSION_ID_TO_WDS_SVC_ID_IPV4(session_id) \
   (session_id < QBI_SVC_MSSAR_MAX_SESSIONS ? \
    (qbi_qmi_svc_e)(QBI_QMI_SVC_WDS_FIRST + session_id * 2) : \
    (qbi_qmi_svc_e)QBI_QMI_SVC_WDS_FIRST)

/*! Map session ID to QMI WDS IPv6 service ID */
#define QBI_SVC_MSSAR_SESSION_ID_TO_WDS_SVC_ID_IPV6(session_id) \
   (session_id < QBI_SVC_MSSAR_MAX_SESSIONS ? \
    ((qbi_qmi_svc_e)(QBI_QMI_SVC_WDS_FIRST + session_id * 2 + 1)) : \
    (qbi_qmi_svc_e)(QBI_QMI_SVC_WDS_FIRST + 1))

/*! Map QMI WDS service ID to session ID */
#define QBI_SVC_MSSAR_WDS_SVC_ID_TO_SESSION_ID(wds_svc_id) \
   ((wds_svc_id >= QBI_QMI_SVC_WDS_FIRST && wds_svc_id <= QBI_QMI_SVC_WDS_LAST) ? \
    (wds_svc_id - QBI_QMI_SVC_WDS_FIRST) / 2 : 0)

/*! @addtogroup MBIM_CID_MS_SAR_TRANSMISSION_STATUS
    @{ */

/*! Default hysteresis timer */
#define QBI_SVC_MBIM_MS_SAR_DEFAULT_HYSTERESIS_TIMER  (5)

/*! Hysteresis timer range */
#define MIN_HYSTERESIS_TIMER 0
#define MAX_HYSTERESIS_TIMER 60


/*! @} */

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! Cache used locally by CIDs processed in this file. */
typedef PACK(struct) {
  /*! Client notification enabled flag */
  uint32 notification_enabled;

  /*! Timer for packet transmission status check (1-5 secs) */
  uint32 hysteresis_timer;

  /*! Packet transmission status */
  uint32 transmission_status;

  /*! Number of packets transmitted without error */
  uint32 tx_count;

  /*! Number of packets received without error */
  uint32 rx_count;

  qbi_os_timer_t timer;
} qbi_svc_mssar_cache_s;

typedef struct {
  uint32 wds_client_id;
} qbi_svc_mssar_tranmission_status_info;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_action_e qbi_svc_mssar_open_configure_qmi_inds
(
  qbi_txn_s *txn
);

/*! @addtogroup QBI_SVC_MSSAR_MBIM_CID_MS_SAR_CONFIG
    @{ */

static qbi_svc_action_e qbi_svc_mssar_config_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_mssar_config_q_sar02_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_mssar_config_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_mssar_config_s_sar01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup MBIM_CID_MS_SAR_TRANSMISSION_STATUS
    @{ */

qbi_svc_action_e qbi_svc_mssar_transmission_status_e_wds01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_mssar_transmission_status_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_mssar_transmission_status_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_mssar_transmission_status_s_build_wds01_req
(
  qbi_txn_s *txn,
  uint32 hysteresis_timer
);

static boolean qbi_sar_timer_init
(
  qbi_txn_s *txn
);

static void qbi_sar_timeout_cb
(
  void *data
);

static qbi_svc_action_e qbi_svc_mssar_close
(
  qbi_txn_s *txn
);


/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief Static QMI indication handlers (generally, CID event handlers)
*/
static const qbi_svc_ind_info_s qbi_svc_mssar_static_ind_hdlrs[] = {
  QBI_SVC_MSSAR_STATIC_IND_HDLR(QBI_QMI_SVC_WDS, QMI_WDS_EVENT_REPORT_IND_V01,
  QBI_SVC_MSSAR_MBIM_CID_MS_TRANSMISSION_STATUS,
                               qbi_svc_mssar_transmission_status_e_wds01_ind_cb),
};

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_mssar_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_mssar_cmd_hdlr_tbl[] = {
  /* MBIM_CID_MS_SAR_CONFIG */
  { qbi_svc_mssar_config_q_req, 0, 
  qbi_svc_mssar_config_s_req, sizeof(qbi_svc_mssar_set_sar_config_s_req_s)},
  /* MBIM_CID_MS_SAR_TRANSMISSION_STATUS */
  { qbi_svc_mssar_transmission_status_q_req, 0, 
  qbi_svc_mssar_transmission_status_s_req, 
  sizeof(qbi_svc_mssar_set_transmission_status_s_req_s)}
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_mssar_cache_alloc
===========================================================================*/
/*!
    @brief Allocates the cache for Selective Adaptation Rate service
    caches

    @details

    @param ctx

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_mssar_cache_alloc
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_mssar_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_cache_alloc(ctx, 
    QBI_SVC_ID_MSSAR, sizeof(qbi_svc_mssar_cache_s));
  if (cache == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate cache!");
  }

  return TRUE;
} /* qbi_svc_mssar_cache_alloc() */

/*===========================================================================
FUNCTION: qbi_svc_mssar_cache_get
===========================================================================*/
/*!
  @brief Returns a pointer to the Selective Adaptation Rate service's cache

  @details

  @param ctx

  @return qbi_svc_bc_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
static qbi_svc_mssar_cache_s *qbi_svc_mssar_cache_get
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  
  return (qbi_svc_mssar_cache_s *)qbi_svc_cache_get(ctx, QBI_SVC_ID_MSSAR);
} /* qbi_svc_mssar_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_mssar_open
===========================================================================*/
/*!
    @brief Runtime intiailization of the Selective Adaptation Rate service

    @details
    This is invoked per-context when the device receives a MBIM_OPEN_MSG.
    It is in charge of performing all runtime initialization so that the
    service can be operational.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_I_0("Processing selective absorption rate service open...");
  if (!qbi_svc_mssar_cache_alloc(txn->ctx))
  {
    QBI_LOG_E_0("Couldn't allocate cache");
  }
  else if (!qbi_qmi_alloc_svc_handle(
    txn->ctx, QBI_SVC_MSSAR_SESSION_ID_TO_WDS_SVC_ID_IPV4(0)) ||
    !qbi_qmi_alloc_svc_handle(
      txn->ctx, QBI_SVC_MSSAR_SESSION_ID_TO_WDS_SVC_ID_IPV6(0))||
    !qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_SAR))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
    action = QBI_SVC_ACTION_ABORT;
  }
  else if(!qbi_sar_timer_init(txn))
  {
    QBI_LOG_E_0("Timer init failed aborting");
    action = QBI_SVC_ACTION_ABORT;
  }
  else if (!qbi_svc_ind_reg_static(
    txn->ctx, QBI_SVC_ID_MSSAR, qbi_svc_mssar_static_ind_hdlrs,
    ARR_SIZE(qbi_svc_mssar_static_ind_hdlrs)))
  {
    QBI_LOG_E_0("Couldn't register QMI indication handlers");
  }
  else
  {
    action = qbi_svc_mssar_open_configure_qmi_inds(txn);
  }

  return action;
} /* qbi_svc_mssar_open() */

/*===========================================================================
  FUNCTION: qbi_svc_mssar_close
===========================================================================*/
/*!
    @brief Releases resources allocated by Selective Absorption Rate service

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_close
(
  qbi_txn_s *txn
)
{
  qbi_svc_mssar_cache_s *cache = NULL;
  qbi_os_timer_t *timer = NULL;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_mssar_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  timer = &cache->timer;

  if (timer != NULL )
  {
    QBI_LOG_W_0("Cancelling SAR timer in mssar_close");
    (void)qbi_os_timer_clear(timer);
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_mssar_close() */

/*===========================================================================
  FUNCTION: qbi_sar_timer_init
===========================================================================*/
/*!
    @brief Timer initialization for transmission state indication

    @details

    @param txn

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_sar_timer_init
(
  qbi_txn_s *txn
)
{
  qbi_svc_mssar_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_mssar_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if(qbi_os_timer_init(&cache->timer, qbi_sar_timeout_cb, txn->ctx))
  {
    return TRUE;
  }
  return FALSE;
}/* qbi_sar_timer_init */

  /*===========================================================================
  FUNCTION: qbi_svc_mssar_open_configure_qmi_inds
  ===========================================================================*/
  /*!
  @brief Configures QMI indications

  @details
  Other QMI indications will be registered on-demand depending on CID filter
  status (see qbi_svc_mssar_cmd_hdlr_tbl).

  @param ctx

  @return boolean TRUE on success, FALSE otherwise
  */
  /*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_open_configure_qmi_inds
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  return qbi_svc_mssar_transmission_status_s_build_wds01_req(txn,
    QBI_SVC_MBIM_MS_SAR_DEFAULT_HYSTERESIS_TIMER);;
} /* qbi_svc_mssar_open_configure_qmi_inds() */

/*! @addtogroup MBIM_CID_MS_SAR_CONFIG
    @{ */

/*===========================================================================
FUNCTION: qbi_svc_mssar_config_populate_rsp
===========================================================================*/
/*!
    @brief Allocates and populates a MBIM_MS_SAR_CONFIG structure on the
    response

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_config_populate_rsp
(
  qbi_txn_s *txn,
  sar_rf_get_state_resp_msg_v01 *qmi_rsp
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_mssar_sar_config_rsp_s *rsp = NULL;
  qbi_mbim_offset_size_pair_s *field_desc = NULL;
  qbi_svc_mssar_config_state_s* config_state = NULL;
  /*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  rsp = (qbi_svc_mssar_sar_config_rsp_s *) qbi_txn_alloc_rsp_buf(txn,
      (sizeof(qbi_svc_mssar_sar_config_rsp_s) +
        sizeof(qbi_mbim_offset_size_pair_s)));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->mode = QBI_SVC_MBIM_MS_SAR_CONTROL_MODE_DEVICE;
  rsp->backoff_status = QBI_SVC_MBIM_MS_SAR_BACKOFF_STATUS_ENABLED;
  rsp->wifi_intigration = QBI_SVC_MBIM_MS_SAR_WIFI_HARDWARE_NOT_INTEGRATED;
  rsp->element_count = 1;

  field_desc = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *)txn->rsp.data +
      sizeof(qbi_svc_mssar_sar_config_rsp_s));

  config_state = (qbi_svc_mssar_config_state_s *)
    qbi_txn_rsp_databuf_add_field(txn, field_desc, 0,
      sizeof(qbi_svc_mssar_config_state_s), NULL);
  QBI_CHECK_NULL_PTR_RET_ABORT(config_state);

  config_state->antenna_index = QBI_SVC_MBIM_MS_SAR_ANTENNA_INDEX;
  config_state->backoff_index = qmi_rsp->sar_rf_state_valid ?
    qmi_rsp->sar_rf_state : QMI_SAR_RF_STATE_DEFAULT_V01;

  /* Update the size field to include DataBuffer items */
  action = qbi_txn_rsp_databuf_consolidate(txn) ?
    QBI_SVC_ACTION_SEND_RSP : QBI_SVC_ACTION_ABORT;

  return action;
} /* qbi_svc_mssar_config_populate_rsp() */

/*===========================================================================
FUNCTION: qbi_svc_mssar_config_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_SAR_CONFIG query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_config_q_req
(
  qbi_txn_s *txn
)
{
  sar_rf_get_state_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (sar_rf_get_state_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_SAR, QMI_SAR_RF_GET_STATE_REQ_MSG_V01,
      qbi_svc_mssar_config_q_sar02_rsp_cb);
  
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_mssar_config_q_req() */

/*===========================================================================
FUNCTION: qbi_svc_mssar_config_q_sar02_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_SAR_RF_GET_STATE_RESP for MBIM_CID_MS_SAR_CONFIG
    query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_config_q_sar02_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  sar_rf_get_state_resp_msg_v01 *qmi_rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (sar_rf_get_state_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    action = qbi_svc_mssar_config_populate_rsp(qmi_txn->parent, qmi_rsp);
  }

  return action;
} /* qbi_svc_mssar_config_q_sar02_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_mssar_config_s_precheck
===========================================================================*/
/*!
    @brief Performs input validation for MBIM_CID_MS_SAR_CONFIG set request

    @details

    @param txn

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_mssar_config_s_precheck
(
  qbi_txn_s *txn
)
{
  qbi_svc_mssar_set_sar_config_s_req_s *req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_mssar_set_sar_config_s_req_s *)txn->req.data;

  if (QBI_SVC_MBIM_MS_SAR_CONTROL_MODE_DEVICE != req->mode ||
    QBI_SVC_MBIM_MS_SAR_BACKOFF_STATUS_ENABLED != req->backoff_status ||
    req->element_count != 1)
  {
    QBI_LOG_E_2("Unsupported mode %d or backoff status %d",
      req->mode, req->backoff_status);
    txn->status = QBI_MBIM_STATUS_OPERATION_NOT_ALLOWED;
    return FALSE;
  }

  return TRUE;
} /* qbi_svc_mssar_config_s_precheck() */

/*===========================================================================
FUNCTION: qbi_svc_mssar_config_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_SAR_CONFIG set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_config_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_mssar_set_sar_config_s_req_s *req = NULL;
  qbi_mbim_offset_size_pair_s *pair = NULL;
  qbi_svc_mssar_config_state_s* config_state = NULL;
  sar_rf_set_state_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_mssar_set_sar_config_s_req_s *)txn->req.data;

  if (!qbi_svc_mssar_config_s_precheck(txn))
  {
    QBI_LOG_E_2("Unsupported mode %d or backoff status %d", 
      req->mode, req->backoff_status);
    txn->status = QBI_MBIM_STATUS_OPERATION_NOT_ALLOWED;  
  }
  else
  {
    /* Process only first element */
    pair = (qbi_mbim_offset_size_pair_s *)((uint8*)txn->req.data +
        sizeof(qbi_svc_mssar_set_sar_config_s_req_s));

    config_state = (qbi_svc_mssar_config_state_s *)
      ((uint8*)txn->req.data + pair->offset);

    qmi_req = (sar_rf_set_state_req_msg_v01 *)qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_SAR, QMI_SAR_RF_SET_STATE_REQ_MSG_V01,
        qbi_svc_mssar_config_s_sar01_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->sar_rf_state = config_state->backoff_index;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_mssar_config_s_req() */

/*===========================================================================
FUNCTION: qbi_svc_mssar_config_s_sar01_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_SAR_RF_SET_STATE_RESP for MBIM_CID_MS_SAR_CONFIG set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_config_s_sar01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
  )
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  sar_rf_set_state_resp_msg_v01 *qmi_rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (sar_rf_set_state_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    action = qbi_svc_mssar_config_q_req(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_mssar_config_s_sar01_rsp_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_MS_SAR_TRANSMISSION_STATUS
    @{ */

/*===========================================================================
FUNCTION: qbi_svc_mssar_transmission_status_qmi_ind_reg_wds01_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WDS_SET_EVENT_REPORT_RESP from enabling/disabling
    reporting of Tx and Rx packets transmitted and received without error

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_transmission_status_qmi_ind_reg_wds01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
  )
{
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
  wds_set_event_report_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_mssar_transmission_status_info_s * rsp = NULL;
  qbi_svc_mssar_cache_s *cache = NULL;
  qbi_svc_mssar_tranmission_status_info *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  info = (qbi_svc_mssar_tranmission_status_info *)qmi_txn->parent->info;

  cache = qbi_svc_mssar_cache_get(qmi_txn->parent->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  info->wds_client_id++;
  if (info->wds_client_id <= QBI_QMI_SVC_WDS_LAST)
  {
    action = qbi_svc_mssar_transmission_status_s_build_wds01_req(
      qmi_txn->parent,cache->hysteresis_timer);
    return action;
  }

  qmi_rsp = (wds_set_event_report_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for WDS indications!!! Error code %d",
      qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    if (qmi_txn->parent->req.data)
    {
      rsp = (qbi_svc_mssar_transmission_status_info_s *)
        qbi_txn_alloc_rsp_buf(qmi_txn->parent, 
          sizeof(qbi_svc_mssar_transmission_status_info_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

      rsp->channel_notification = cache->notification_enabled;
      rsp->transmission_status = cache->transmission_status;
      rsp->hysteresis_timer = cache->hysteresis_timer;
      action = QBI_SVC_ACTION_SEND_RSP;
    }
    else
    {
      cache->notification_enabled = 
        QBI_SVC_MBIM_MS_SAR_TRANSMISSION_NOTIFICATION_DISABLED;
      cache->transmission_status = 
        QBI_SVC_MBIM_MS_SAR_TRANSMISSION_STATE_INACTIVE;
      cache->hysteresis_timer =
        QBI_SVC_MBIM_MS_SAR_DEFAULT_HYSTERESIS_TIMER;

      cache->tx_count = 0;
      cache->rx_count = 0;

      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }

  if (qmi_txn->parent->info != NULL)
  {
    QBI_MEM_FREE(qmi_txn->parent->info);
    qmi_txn->parent->info = NULL;
  }

  return action;
} /* qbi_svc_mssar_transmission_status_qmi_ind_reg_wds01_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_mssar_transmission_status_s_build_wds01_req
===========================================================================*/
/*!
    @brief Allocates and populates a request for QMI_WDS_SET_EVENT_REPORT

    @details

    @param txn
    @param wds_svc_id

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_transmission_status_s_build_wds01_req
(
  qbi_txn_s *txn,
  uint32 hysteresis_timer
)
{
  wds_set_event_report_req_msg_v01 *wds01_req;
  qbi_svc_mssar_cache_s *cache = NULL;
  qbi_svc_mssar_tranmission_status_info *info = NULL;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_mssar_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (NULL == txn->info)
  {
    txn->info = QBI_MEM_MALLOC_CLEAR(
      sizeof(qbi_svc_mssar_tranmission_status_info));
    QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
    ((qbi_svc_mssar_tranmission_status_info *)txn->info)->wds_client_id =
      QBI_QMI_SVC_WDS;
  }

  info = (qbi_svc_mssar_tranmission_status_info *)txn->info;
  QBI_LOG_D_1(" wds_client_id = %d",info->wds_client_id);

  wds01_req = (wds_set_event_report_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, info->wds_client_id,
      QMI_WDS_SET_EVENT_REPORT_REQ_V01,
      qbi_svc_mssar_transmission_status_qmi_ind_reg_wds01_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(wds01_req);

  wds01_req->report_stats_valid = TRUE;
  wds01_req->report_stats.stats_period = hysteresis_timer;

  if (cache->notification_enabled == QBI_SVC_MBIM_MS_SAR_TRANSMISSION_NOTIFICATION_ENABLED)
  {
    wds01_req->report_stats.stats_mask =
      QMI_WDS_MASK_STATS_TX_PACKETS_OK_V01 |
      QMI_WDS_MASK_STATS_RX_PACKETS_OK_V01;
    qbi_os_timer_set(&cache->timer, (cache->hysteresis_timer * 1000) + 500);
  }
  else
  {
    wds01_req->report_stats.stats_mask = 0;
  }

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_mssar_transmission_status_s_build_wds01_req */

/*===========================================================================
  FUNCTION: qbi_svc_mssar_transmission_status_e_wds01_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WDS_EVENT_REPORT_IND, looking for number of tx and 
    Rx packets transmitted and received without error for
    MBIM_CID_MS_SAR_TRANSMISSION_STATUS event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_mssar_transmission_status_e_wds01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const wds_event_report_ind_msg_v01 *qmi_ind = NULL;
  qbi_svc_mssar_transmission_status_info_s *rsp = NULL;
  qbi_svc_mssar_cache_s *cache = NULL;
  uint32 last_rx_count = 0;
  uint32 last_tx_count = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const wds_event_report_ind_msg_v01 *) ind->buf->data;
  cache = qbi_svc_mssar_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  last_rx_count = cache->rx_count;
  last_tx_count = cache->tx_count;

  cache->rx_count = qmi_ind->rx_ok_count_valid ?
    qmi_ind->rx_ok_count : cache->rx_count;
  cache->tx_count = qmi_ind->tx_ok_count_valid ?
    qmi_ind->tx_ok_count : cache->tx_count;

  cache->transmission_status = 
    (cache->rx_count == last_rx_count && cache->tx_count == last_tx_count) ?
    QBI_SVC_MBIM_MS_SAR_TRANSMISSION_STATE_INACTIVE :
    QBI_SVC_MBIM_MS_SAR_TRANSMISSION_STATE_ACTIVE;

  if (cache->notification_enabled)
  {
    /* Waking up the timer*/
    qbi_os_timer_set(&cache->timer, (cache->hysteresis_timer * 1000) + 500);

    rsp = (qbi_svc_mssar_transmission_status_info_s *)qbi_txn_alloc_rsp_buf(
      ind->txn, sizeof(qbi_svc_mssar_transmission_status_info_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    rsp->channel_notification = cache->notification_enabled;
    rsp->transmission_status = cache->transmission_status;
    rsp->hysteresis_timer = cache->hysteresis_timer;

    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_mssar_transmission_status_e_wds01_ind_cb() */

/*===========================================================================
FUNCTION: qbi_svc_mssar_transmission_status_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_SAR_TRANSMISSION_STATUS query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_transmission_status_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_mssar_cache_s *cache = NULL;
  qbi_svc_mssar_transmission_status_info_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  
  cache = qbi_svc_mssar_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_mssar_transmission_status_info_s *)qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_mssar_transmission_status_info_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->channel_notification = cache->notification_enabled;
  rsp->transmission_status = cache->transmission_status;
  rsp->hysteresis_timer = cache->hysteresis_timer;

  action = QBI_SVC_ACTION_SEND_RSP;

  return action;
} /* qbi_svc_mssar_transmission_status_q_req() */

/*===========================================================================
FUNCTION: qbi_svc_mssar_transmission_status_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_SAR_TRANSMISSION_STATUS set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mssar_transmission_status_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_mssar_set_transmission_status_s_req_s *req = NULL;
  qbi_svc_mssar_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_mssar_set_transmission_status_s_req_s*)txn->req.data;
  cache = qbi_svc_mssar_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (req->hysteresis_timer < MIN_HYSTERESIS_TIMER ||
      req->hysteresis_timer > MAX_HYSTERESIS_TIMER ||
      (req->channel_notification !=
        QBI_SVC_MBIM_MS_SAR_TRANSMISSION_NOTIFICATION_DISABLED &&
       req->channel_notification !=
        QBI_SVC_MBIM_MS_SAR_TRANSMISSION_NOTIFICATION_ENABLED))
  {
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    QBI_LOG_E_1("Invalid timer parameter %d", req->hysteresis_timer);
  }
  else if (req->channel_notification == 
    QBI_SVC_MBIM_MS_SAR_TRANSMISSION_NOTIFICATION_DISABLED)
  {
    QBI_LOG_E_1("Channel Notification Disabled[%d]cannot process Hystersis Timer",
    req->channel_notification);
  }
  else
  {
    cache->notification_enabled = req->channel_notification;
    cache->hysteresis_timer = (req->hysteresis_timer == 0 ) ? 
      QBI_SVC_MBIM_MS_SAR_DEFAULT_HYSTERESIS_TIMER : req->hysteresis_timer;
    action = qbi_svc_mssar_transmission_status_s_build_wds01_req(
      txn, req->hysteresis_timer);
  }

  return action;
} /* qbi_svc_mssar_transmission_status_s_req() */

/*===========================================================================
  FUNCTION: qbi_sar_timeout_cb
===========================================================================*/
/*!
    @brief Callback function invoked when the timer expires

    @details

    @param data
*/
/*=========================================================================*/
static void qbi_sar_timeout_cb
(
  void *data
)
{
  qbi_svc_mssar_cache_s *cache = NULL;
  qbi_ctx_s    *ctx;
/*-------------------------------------------------------------------------*/
  QBI_LOG_D_0("qbi_sar_timeout_cb");

  ctx = (qbi_ctx_s *) data;
  QBI_CHECK_NULL_PTR_RET(ctx);

  cache = qbi_svc_mssar_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache->notification_enabled)
  {
     cache->transmission_status = 
        QBI_SVC_MBIM_MS_SAR_TRANSMISSION_STATE_INACTIVE;
    /* Forcing the indication */
    qbi_svc_force_event(
        ctx, QBI_SVC_ID_MSSAR,
      QBI_SVC_MSSAR_MBIM_CID_MS_TRANSMISSION_STATUS);
  }
} /* qbi_sar_timeout_cb() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_mssar_init
===========================================================================*/
/*!
    @brief One-time initialization of the MSSAR device service

    @details
*/
/*=========================================================================*/
void qbi_svc_mssar_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_mssar_cfg = {
    {
      0x68, 0x22, 0x3d, 0x04, 0x9f, 0x6c, 0x4e, 0x0f,
      0x82, 0x2d, 0x28, 0x44, 0x1f, 0xb7, 0x23, 0x40
    },
    QBI_SVC_ID_MSSAR,
    FALSE,
    qbi_svc_mssar_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_mssar_cmd_hdlr_tbl),
    qbi_svc_mssar_open,
    qbi_svc_mssar_close
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_mssar_cfg);
} /* qbi_svc_mssar_init() */