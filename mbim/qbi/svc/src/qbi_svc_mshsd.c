/*!
  @file
  qbi_svc_mshsd.c

  @brief
  Microsoft Host Shutdown device service implementation, based on the "Device
  Service: Host shutdown notification" document from Microsoft. This device
  service provides a notification to the device that the system is about to
  power down, allowing the modem to deregister from the network.
*/

/*=============================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
03/20/13  bd   Add QMI SSCTL support
11/21/12  bd   Add runtime DeviceType configuration
06/11/12  bd   Add support for remote device types with external power
04/27/12  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_mshsd.h"
#include "qbi_svc_mshsd_mbim.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_nv_store.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_svc_bc.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_txn.h"

#include "device_management_service_v01.h"
#include "subsystem_control_v01.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! Timeout for receiving QMI_SSCTL_SHUTDOWN_READY_IND after sending
    QMI_SSCTL_SHUTDOWN_REQ */
#define QBI_SVC_MSHSD_SHUTDOWN_TIMEOUT_MS (15000)

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_action_e qbi_svc_mshsd_open
(
  qbi_txn_s *txn
);

/*! @addtogroup MBIM_CID_MSHOSTSHUTDOWN
    @{ */

static qbi_svc_action_e qbi_svc_mshsd_mshostshutdown_s_build_ssctl21_req_if_needed
(
  qbi_txn_s *txn
);

static void qbi_svc_mshsd_mshostshutdown_s_completion_cb
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_mshsd_mshostshutdown_s_dms2e_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_mshsd_mshostshutdown_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_mshsd_mshostshutdown_s_ssctl21_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_mshsd_mshostshutdown_s_ssctl21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_mshsd_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_mshsd_cmd_hdlr_tbl[] = {
  /* MBIM_CID_MSHOSTSHUTDOWN */
  {NULL, 0, qbi_svc_mshsd_mshostshutdown_s_req, 0}
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_mshsd_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mshsd_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_DMS))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_mshsd_open() */

/*! @addtogroup MBIM_CID_MSHOSTSHUTDOWN
    @{ */


/*===========================================================================
  FUNCTION: qbi_svc_mshsd_mshostshutdown_s_build_ssctl21_req_if_needed
===========================================================================*/
/*!
    @brief Determines if a QMI_SSCTL_SHUTDOWN_REQ should be sent to the
    modem, and if so, populates the request

    @details

    @param txn

    @return qbi_svc_action_e QBI_SVC_ACTION_SEND_QMI_REQ if the request was
    allocated and populated, QBI_SVC_ACTION_SEND_RSP if the SSCTL request
    is not required, or QBI_SVC_ACTION_ABORT on unexpected error
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mshsd_mshostshutdown_s_build_ssctl21_req_if_needed
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  uint32 device_type = QBI_SVC_BC_DEVICE_TYPE_UNKNOWN;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* If device is embedded and QMI SSCTL service is supported, send
     QMI_SSCTL_SHUTDOWN_REQ */
  if (!qbi_nv_store_cfg_item_read(
        txn->ctx, QBI_NV_STORE_CFG_ITEM_DEVICE_TYPE, &device_type,
        sizeof(device_type)) ||
      device_type != QBI_SVC_BC_DEVICE_TYPE_EMBEDDED)
  {
    QBI_LOG_I_1("Device type is %d - skipping SSCTL request", device_type);
  }
  else if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_SSCTL))
  {
    QBI_LOG_I_0("SSCTL service not supported");
  }
  else
  {
    QBI_LOG_I_0("Sending SSCTL shutdown request");
    qbi_txn_set_completion_cb(
      txn, qbi_svc_mshsd_mshostshutdown_s_completion_cb);
    qbi_txn_set_timeout(txn, QBI_SVC_MSHSD_SHUTDOWN_TIMEOUT_MS, NULL);
    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_QMI_SVC_SSCTL, QMI_SSCTL_SHUTDOWN_REQ_V01,
      qbi_svc_mshsd_mshostshutdown_s_ssctl21_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_mshsd_mshostshutdown_s_build_ssctl21_req_if_needed() */

/*===========================================================================
  FUNCTION: qbi_svc_mshsd_mshostshutdown_s_completion_cb
===========================================================================*/
/*!
    @brief Completion callback used to release QMI SSCTL client ID

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_mshsd_mshostshutdown_s_completion_cb
(
  const qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  (void) qbi_qmi_release_svc_handle(txn->ctx, QBI_QMI_SVC_SSCTL);
} /* qbi_svc_mshsd_mshostshutdown_s_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_mshsd_mshostshutdown_s_dms2e_rsp_cb
===========================================================================*/
/*!
    @brief Process QMI_DMS_SET_OPERATING_MODE_RESP for a
    MBIM_CID_MSHOSTSHUTDOWN set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mshsd_mshostshutdown_s_dms2e_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  dms_set_operating_mode_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  /* Only log errors, since this is a notification from the host, not a
     request to perform a specific operation */
  qmi_rsp = (dms_set_operating_mode_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
  {
    QBI_LOG_W_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }

  return qbi_svc_mshsd_mshostshutdown_s_build_ssctl21_req_if_needed(
    qmi_txn->parent);
} /* qbi_svc_mshsd_mshostshutdown_s_dms2e_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_mshsd_mshostshutdown_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MSHOSTSHUTDOWN set request

    @details

    @param txn

    @return qbi_svc_action_e

    @note The implementation of this CID should be tweaked by the OEM as
    necessary.
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mshsd_mshostshutdown_s_req
(
  qbi_txn_s *txn
)
{
  uint32 device_type;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
  dms_set_operating_mode_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  qbi_txn_req_buf_ignored(txn);

  if (!qbi_nv_store_cfg_item_read(
        txn->ctx, QBI_NV_STORE_CFG_ITEM_DEVICE_TYPE, &device_type,
        sizeof(device_type)))
  {
    QBI_LOG_E_0("Couldn't determine device type - no action");
  }
  else if (device_type == QBI_SVC_BC_DEVICE_TYPE_REMOTE)
  {
    /*! @note We expect that a remote device type will be externally powered,
      so unlike the other device types which are powered over USB, no action
      is necessary in response to this CID. Host shutdown will not impact
      device power, so the device should remain in its current operating mode
      to enable other functionality. */
    QBI_LOG_I_0("Host shutting down - no change to operating mode");
  }
  else if (qbi_svc_bc_device_is_in_ftm(txn->ctx))
  {
    QBI_LOG_W_0("Radio is in FTM - no change to operating mode");
    action = qbi_svc_mshsd_mshostshutdown_s_build_ssctl21_req_if_needed(txn);
  }
  else
  {
    /*! @note This behavior is only desired when the device relies on the host
        for its power supply, to allow power down registration, etc. before power
        is removed from the device. */
    QBI_LOG_I_0("Host is shutting down. Going to LPM");
    qmi_req = (dms_set_operating_mode_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_DMS, QMI_DMS_SET_OPERATING_MODE_REQ_V01,
        qbi_svc_mshsd_mshostshutdown_s_dms2e_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->operating_mode = DMS_OP_MODE_LOW_POWER_V01;
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_mshsd_mshostshutdown_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_mshsd_mshostshutdown_s_ssctl21_ind_cb
===========================================================================*/
/*!
    @brief Handles QMI_SSCTL_SHUTDOWN_READY_IND_V01

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mshsd_mshostshutdown_s_ssctl21_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
/*-------------------------------------------------------------------------*/
  QBI_ARG_NOT_USED(ind);
  QBI_LOG_I_0("Modem now ready for shutdown");
  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_mshsd_mshostshutdown_s_ssctl21_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_mshsd_mshostshutdown_s_ssctl21_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_SSCTL_SHUTDOWN_RESP_V01

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_mshsd_mshostshutdown_s_ssctl21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qmi_ssctl_shutdown_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (qmi_ssctl_shutdown_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qbi_svc_ind_reg_dynamic(
             qmi_txn->ctx, QBI_SVC_ID_MSHSD,
             QBI_SVC_MSHSD_MBIM_CID_MSHOSTSHUTDOWN, QBI_QMI_SVC_SSCTL,
             QMI_SSCTL_SHUTDOWN_READY_IND_V01,
             qbi_svc_mshsd_mshostshutdown_s_ssctl21_ind_cb, qmi_txn->parent,
             NULL))
  {
    QBI_LOG_E_0("Failed to register dynamic indication handler");
  }
  else
  {
    QBI_LOG_I_0("Waiting on shutdown ready indication");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_mshsd_mshostshutdown_s_ssctl21_rsp_cb() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_mshsd_init
===========================================================================*/
/*!
    @brief One-time initialization of the MSHOSTSHUTDOWN device service

    @details
*/
/*=========================================================================*/
void qbi_svc_mshsd_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_mshsd_cfg = {
    {
      0x88, 0x3b, 0x7c, 0x26, 0x98, 0x5f, 0x43, 0xfa,
      0x98, 0x04, 0x27, 0xd7, 0xfb, 0x80, 0x95, 0x9c
    },
    QBI_SVC_ID_MSHSD,
    FALSE,
    qbi_svc_mshsd_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_mshsd_cmd_hdlr_tbl),
    qbi_svc_mshsd_open,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_mshsd_cfg);
} /* qbi_svc_mshsd_init() */

