/*!
  @file
  qbi_svc_qmbe.c

  @brief
  Qualcomm Mobile Broadband MBIM Extensibility device service implementation.
  This device service provides Qualcomm specific functionalities in addition to
  MBIM specification.
*/

/*=============================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
11/20/12  bd   Differentiate active and next diag config
10/08/12  hz   Add DIAG over MBIM support
07/23/12  hz   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_qmbe.h"
#include "qbi_svc_qmbe_mbim.h"

#include "qbi_common.h"
#include "qbi_diag.h"
#include "qbi_hc.h"
#include "qbi_mbim.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_txn.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_action_e qbi_svc_qmbe_close
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_qmbe_open
(
  qbi_txn_s *txn
);

/*! @addtogroup MBIM_CID_QMBE_DIAG_CONFIG
    @{ */

static qbi_hc_diag_config_e qbi_svc_qmbe_diag_config_mbim_to_qbi_hc
(
  uint32 mbim_diag_config
);

static qbi_svc_action_e qbi_svc_qmbe_diag_config_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_qmbe_diag_config_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_QMBE_DIAG_DATA
    @{ */

static qbi_svc_action_e qbi_svc_qmbe_diag_data_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_qmbe_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_qmbe_cmd_hdlr_tbl[] = {
  /* MBIM_CID_QMBE_DIAG_CONFIG */
  {qbi_svc_qmbe_diag_config_q_req, 0,
   qbi_svc_qmbe_diag_config_s_req, sizeof(qbi_svc_qmbe_diag_config_s_req_s)},
  /* MBIM_CID_QMBE_DIAG_DATA */
  {NULL, 0,
   qbi_svc_qmbe_diag_data_s_req, 0}
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_qmbe_close
===========================================================================*/
/*!
    @brief Releases resources allocated by the QMBE device service

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qmbe_close
(
  qbi_txn_s *txn
)
{
  qbi_hc_diag_config_e diag_config;
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(txn);
  if (qbi_hc_diag_config_get_active(&diag_config) &&
      diag_config == QBI_HC_DIAG_CONFIG_EXCLUDED)
  {
    qbi_diag_suspend();
  }
  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_qmbe_close() */

/*===========================================================================
  FUNCTION: qbi_svc_qmbe_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qmbe_open
(
  qbi_txn_s *txn
)
{
  qbi_hc_diag_config_e diag_config;
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(txn);
  if (qbi_hc_diag_config_get_active(&diag_config) &&
      diag_config == QBI_HC_DIAG_CONFIG_EXCLUDED)
  {
    qbi_diag_init();

    QBI_LOG_I_0("Register Diag rx data callback");
    qbi_diag_reg_rx_data_cb(qbi_svc_qmbe_diag_data_cb);
  }
  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_qmbe_open() */

/*! @addtogroup MBIM_CID_QMBE_DIAG_CONFIG
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_qmbe_diag_config_mbim_to_qbi_hc
===========================================================================*/
/*!
    @brief Converts a QMBE diag config enum to an internal QBI HC enum

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_hc_diag_config_e qbi_svc_qmbe_diag_config_mbim_to_qbi_hc
(
  uint32 mbim_diag_config
)
{
  qbi_hc_diag_config_e qbi_diag_config;
/*-------------------------------------------------------------------------*/
  switch (mbim_diag_config)
  {
    case QBI_SVC_QMBE_MBIM_DIAG_EXCLUDE:
      qbi_diag_config = QBI_HC_DIAG_CONFIG_EXCLUDED;
      break;

    case QBI_SVC_QMBE_MBIM_DIAG_PRESENT:
      qbi_diag_config = QBI_HC_DIAG_CONFIG_INCLUDED;
      break;

    default:
      QBI_LOG_E_1("Invalid MBIM diag config %d", mbim_diag_config);
      qbi_diag_config = QBI_HC_DIAG_CONFIG_UNKNOWN;
  }

  return qbi_diag_config;
} /* qbi_svc_qmbe_diag_config_mbim_to_qbi_hc() */

/*===========================================================================
  FUNCTION: qbi_svc_qmbe_diag_config_q_req
===========================================================================*/
/*!
    @brief Handles an MBIM_CID_DIAG_CONFIG query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qmbe_diag_config_q_req
(
  qbi_txn_s *txn
)
{
  qbi_hc_diag_config_e diag_config;
  qbi_svc_qmbe_diag_config_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (!qbi_hc_diag_config_get_next(&diag_config))
  {
    QBI_LOG_E_0("Couldn't determine diag config!");
  }
  else if (diag_config != QBI_HC_DIAG_CONFIG_EXCLUDED &&
           diag_config != QBI_HC_DIAG_CONFIG_INCLUDED)
  {
    txn->status = QBI_SVC_QMBE_MBIM_STATUS_DIAG_UNKNOWN_PID;
  }
  else
  {
    rsp = (qbi_svc_qmbe_diag_config_rsp_s *) qbi_txn_alloc_rsp_buf(
      txn, sizeof(qbi_svc_qmbe_diag_config_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    rsp->diag_config = (diag_config == QBI_HC_DIAG_CONFIG_EXCLUDED) ?
      QBI_SVC_QMBE_MBIM_DIAG_EXCLUDE : QBI_SVC_QMBE_MBIM_DIAG_PRESENT;

    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_qmbe_diag_config_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_qmbe_diag_config_s_req
===========================================================================*/
/*!
    @brief Handles an MBIM_CID_DIAG_CONFIG set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qmbe_diag_config_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_qmbe_diag_config_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_hc_diag_config_set_status_e status;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_qmbe_diag_config_s_req_s *) txn->req.data;
  status = qbi_hc_diag_config_set_next(
    qbi_svc_qmbe_diag_config_mbim_to_qbi_hc(req->diag_config));
  if (status != QBI_HC_DIAG_CONFIG_SET_STATUS_SUCCESS)
  {
    QBI_LOG_E_1("Setting diag config failed with status %d", status);
    switch (status)
    {
      case QBI_HC_DIAG_CONFIG_SET_STATUS_INVALID_PARAM:
        txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
        break;

      case QBI_HC_DIAG_CONFIG_SET_STATUS_UNKNOWN_CONFIG:
        txn->status = QBI_SVC_QMBE_MBIM_STATUS_DIAG_UNKNOWN_PID;
        break;

      case QBI_HC_DIAG_CONFIG_SET_STATUS_READ_FAILURE:
      case QBI_HC_DIAG_CONFIG_SET_STATUS_WRITE_FAILURE:
      default:
        txn->status = QBI_MBIM_STATUS_FAILURE;
    }
  }
  else
  {
    /* Copy the request InformationBuffer into the response */
    qbi_txn_set_rsp_buf(txn, &txn->req);
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_qmbe_diag_config_s_req() */

/*! @} */

/*! @addtogroup MBIM_CID_QMBE_DIAG_DATA
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_qmbe_diag_data_s_req
===========================================================================*/
/*!
    @brief Handles an MBIM_CID_QMBE_DIAG_DATA set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_qmbe_diag_data_s_req
(
  qbi_txn_s *txn
)
{
  qbi_hc_diag_config_e diag_config;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (!qbi_hc_diag_config_get_active(&diag_config) ||
      diag_config != QBI_HC_DIAG_CONFIG_EXCLUDED)
  {
    QBI_LOG_E_1("Incorrect diag configuration %d!", diag_config);
  }
  else
  {
    if (qbi_diag_dispatch_ul_message(txn->req.data, txn->req.size))
    {
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }
  return action;
} /* qbi_svc_qmbe_diag_data_s_req */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*! @addtogroup MBIM_CID_QMBE_DIAG_DATA
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_qmbe_diag_data_cb
===========================================================================*/
/*!
    @brief Callback function handling Diag DL messages

    @details

    @param ctx
    @param buf

    @return qbi_svc_action_e
*/
/*=========================================================================*/
void qbi_svc_qmbe_diag_data_cb
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
)
{
  qbi_txn_s *txn;
/*-------------------------------------------------------------------------*/
  txn = qbi_txn_alloc_event(
    ctx, QBI_SVC_ID_QMBE, QBI_SVC_QMBE_MBIM_CID_DIAG_DATA);
  if (txn == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate new transaction to send event!");
  }
  else
  {
    qbi_txn_set_rsp_buf(txn, buf);
    (void) qbi_svc_proc_action(txn, QBI_SVC_ACTION_SEND_RSP);
  }
} /* qbi_svc_qmbe_diag_data_cb */

/*! @} */

/*===========================================================================
  FUNCTION: qbi_svc_qmbe_init
===========================================================================*/
/*!
    @brief One-time initialization of the QMBE device service

    @details
*/
/*=========================================================================*/
void qbi_svc_qmbe_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_qmbe_cfg = {
    {
      0x2d, 0x0c, 0x12, 0xc9, 0x0e, 0x6a, 0x49, 0x5a,
      0x91, 0x5c, 0x8d, 0x17, 0x4f, 0xe5, 0xd6, 0x3c
    },
    QBI_SVC_ID_QMBE,
    TRUE,
    qbi_svc_qmbe_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_qmbe_cmd_hdlr_tbl),
    qbi_svc_qmbe_open,
    qbi_svc_qmbe_close
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_qmbe_cfg);
} /* qbi_svc_qmbe_init() */

