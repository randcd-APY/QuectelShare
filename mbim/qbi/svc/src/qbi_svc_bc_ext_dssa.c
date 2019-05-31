
/*!
  @file
  qbi_svc_bc_ext_dssa.c

  @brief
  Basic Connectivity Extension device service definitions, to implement Dual
  SIM Single Active requirements.
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
06/26/18  mm   Removed persistent and added non persistent cache for DSSA
10/27/17  nk   Moved dssa features from bc_ext.c
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_bc_ext.h"
#include "qbi_svc_bc_ext_prov.h"
#include "qbi_svc_bc_ext_dssa.h"
#include "qbi_svc_bc_ext_mbim.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_msuicc.h"
#include "qbi_svc_bc_spdp.h"
#include "qbi_svc_bc_sim.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_nv_store.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_txn.h"
#include "qbi_msg_mbim.h"

#include "wireless_data_service_v01.h"
#include "data_system_determination_v01.h"
#include "persistent_device_configuration_v01.h"
#include "device_management_service_v01.h"
#include "user_identity_module_v01.h"
#include "network_access_service_v01.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! @addtogroup MBIM_CID_MS_DEVICE_SLOT_MAPPING
    @{ */

#define QBI_SVC_BC_EXT_DSSA_SLOT_1                              (0)
#define QBI_SVC_BC_EXT_DSSA_SLOT_2                              (1)
#define QBI_SVC_BC_EXT_DSSA_DEFAULT_SLOT  (0)

/*! @} */

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! @addtogroup MBIM_CID_MS_DEVICE_SLOT_MAPPING
    @{ */

typedef enum
{
  QBI_SVC_BC_EXT_DSSA_PROV_SESSION_MIN = 0,

  QBI_SVC_BC_EXT_DSSA_PROV_SESSION_DEACTIVATE_PRIMARY    = 1,
  QBI_SVC_BC_EXT_DSSA_PROV_SESSION_DEACTIVATE_SECONDARY  = 2,
  QBI_SVC_BC_EXT_DSSA_PROV_SESSION_ACTIVATE_PRIMARY      = 3,
  QBI_SVC_BC_EXT_DSSA_PROV_SESSION_ACTIVATE_SECONDARY    = 4,

  QBI_SVC_BC_EXT_DSSA_PROV_SESSION_MAX
}qbi_svc_bc_ext_dssa_prov_session_status_e;

/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO_STATUS
    @{ */
typedef struct {
  uint32 slot_state;
  uint8 is_esim;
  uint32 physical_card_status;
  uint32 app_info_len;
  uint8 physical_slot_state;
} qbi_svc_bc_ext_dssa_info_cache_each_slot;

typedef struct {
  qbi_svc_bc_ext_dssa_info_cache_each_slot card0;
  qbi_svc_bc_ext_dssa_info_cache_each_slot card1;
}qbi_svc_bc_ext_dssa_info_status_cache;

typedef struct {
  uint8 is_esim;
}qbi_svc_bc_ext_dssa_slot_info;

/*! @} */

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

/*! @addtogroup MBIM_CID_MS_DEVICE_SLOT_MAPPING
    @{ */

static qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_mapping_q_uim47_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_mapping_s_uim46_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO
    @{ */

static qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_info_q_uim47_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_bc_ext_dssa_slot_status_update_cache
(
  qbi_svc_bc_ext_dssa_info_cache_each_slot *cache,
  uint32 slot_state
);

static qbi_svc_bc_ext_dssa_info_status_cache *qbi_svc_bc_ext_dssa_slot_info_cache_get
(
  qbi_ctx_s *ctx
);

static void qbi_svc_bc_ext_dssa_slot_info_q_prepare_rsp
(
  qbi_txn_s           *txn,
  uint32              slot_idx,
  uim_physical_slot_status_type_v01  card_status,
  uim_physical_slot_information_type_v01  card_info
);

static uint32 qbi_svc_bc_ext_dssa_slot_info_from_uicc_slot_state
(
  qbi_svc_bc_ext_dssa_info_cache_each_slot *cache
);

static void qbi_svc_bc_ext_dssa_slot_info_force_card1_event
(
  qbi_ctx_s *ctx
);

/*! @} */

/*=============================================================================

Private Function Definitions

=============================================================================*/

/*! @addtogroup MBIM_CID_MS_DEVICE_SLOT_MAPPING
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_dssa_slot_mapping_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_DEVICE_SLOT_MAPPING query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_mapping_q_req
(
  qbi_txn_s *txn
)
{
  uim_get_slots_status_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (uim_get_slots_status_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_SLOTS_STATUS_REQ_V01,
        qbi_svc_bc_ext_dssa_slot_mapping_q_uim47_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_bc_ext_dssa_slot_mapping_q_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_dssa_slot_mapping_q_uim47_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_SLOT_STATUS_RESP for
            MBIM_CID_MS_DEVICE_SLOT_MAPPING query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_mapping_q_uim47_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_get_slots_status_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_slot_mapping_info_s *rsp = NULL;
  uint32 *slot_map = NULL;
  uint32   i = 0;
  uint32   j = 0;
  qbi_mbim_offset_size_pair_s *ol_pair = NULL;
  /*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_get_slots_status_resp_msg_v01 *)qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("DSSA::E: Received error code %d from QMI", qmi_rsp->resp.error);
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
  }
  else if (!qmi_rsp->physical_slot_status_valid)
      {
    QBI_LOG_E_0("DSSA::E: Slot status TLV is missing");
        qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
      }
      else
      {
    QBI_LOG_D_1("DSSA:Total no. of slots = %d", qmi_rsp->physical_slot_status_len);

        rsp = (qbi_svc_bc_ext_slot_mapping_info_s *)qbi_txn_alloc_rsp_buf(
      qmi_txn->parent, (sizeof(qbi_svc_bc_ext_slot_mapping_info_s) +
        sizeof(qbi_mbim_offset_size_pair_s) * QBI_SVC_BC_EXT_DSSA_MAX_SUPPORTED_EXECUTORS));
        QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

        rsp->map_count = QBI_SVC_BC_EXT_DSSA_MAX_SUPPORTED_EXECUTORS;
    
    for (i = 0; i < rsp->map_count; i++)
    {
      ol_pair = (qbi_mbim_offset_size_pair_s*)((uint8*)rsp + 
        (sizeof(qbi_svc_bc_ext_slot_mapping_info_s) + (sizeof(qbi_mbim_offset_size_pair_s) * i)));
      slot_map = (uint32 *)qbi_txn_rsp_databuf_add_field(
        qmi_txn->parent, ol_pair, 0, sizeof(uint32), NULL);
      QBI_CHECK_NULL_PTR_RET_ABORT(slot_map);

      for (j = 0; j < qmi_rsp->physical_slot_status_len; j++)
      {
        if ((qmi_rsp->physical_slot_status[j].physical_card_status == UIM_PHYSICAL_CARD_STATE_PRESENT_V01) &&
          (qmi_rsp->physical_slot_status[j].physical_slot_state == UIM_PHYSICAL_SLOT_STATE_ACTIVE_V01))
        {
          *slot_map = j;
          QBI_LOG_D_2("DSSA:slot_map_list value = %d and logical slot value : %d",
            *slot_map, qmi_rsp->physical_slot_status[j].logical_slot);
          break;
      }
    }
    }
      
    qbi_txn_rsp_databuf_consolidate(qmi_txn->parent);
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_bc_ext_dssa_slot_mapping_q_uim47_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_dssa_slot_mapping_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_DEVICE_SLOT_MAPPING set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_mapping_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uim_switch_slot_req_msg_v01                 *qmi_req = NULL;
  uint32 req_slot = 0;
  qbi_svc_bc_ext_slot_mapping_info_s *req = NULL;
  qbi_txn_s                                   *parent_txn   = NULL;
  qbi_svc_bc_subscriber_ready_status_rsp_s    *rsp          = NULL;
  qbi_mbim_offset_size_pair_s                 *ol_pair      = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  req = (qbi_svc_bc_ext_slot_mapping_info_s *)txn->req.data;
  ol_pair = (qbi_mbim_offset_size_pair_s*)((uint8*)req + 
    sizeof(qbi_svc_bc_ext_slot_mapping_info_s));

  /* Extracting requested slot */
  req_slot = (uint32)(*((uint8*)req + ol_pair->offset));
  if((req_slot != QBI_SVC_BC_EXT_DSSA_SLOT_1) && (req_slot != QBI_SVC_BC_EXT_DSSA_SLOT_2))
  {
    QBI_LOG_E_1("DSSA: Error: Invalid Requested slot %d ", req_slot);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    qbi_svc_bc_msuicc_set_slot_mapping_in_progress_flag(txn->ctx,TRUE);
      /* Sending subscriber_ready state as NOT_INIT */
      parent_txn = qbi_txn_alloc_event(txn->ctx,
        QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS);

      /* Allocate the fixed-length portion of the response now */
      rsp = qbi_txn_alloc_rsp_buf(parent_txn, sizeof(qbi_svc_bc_subscriber_ready_status_rsp_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

      rsp->ready_state = QBI_SVC_BC_READY_STATE_NOT_INITIALIZED;
      QBI_LOG_D_1("DSSA:rsp->ready_state = %d", rsp->ready_state);
      (void)qbi_svc_proc_action(parent_txn, QBI_SVC_ACTION_SEND_RSP);

      qmi_req = (uim_switch_slot_req_msg_v01*)
        qbi_qmi_txn_alloc_ret_req_buf(
          txn, QBI_QMI_SVC_UIM, QMI_UIM_SWITCH_SLOT_REQ_V01,
          qbi_svc_bc_ext_dssa_slot_mapping_s_uim46_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
      /* It always remains one in the case of DSSA */
      QBI_LOG_D_1("DSSA: Requested slot is : %d", req_slot);
      qmi_req->logical_slot = UIM_SLOT_1_V01;
      qmi_req->physical_slot = req_slot + UIM_SLOT_1_V01;
      QBI_LOG_D_2("DSSA:Logical_slot and physical_slot : %d and %d", qmi_req->logical_slot, qmi_req->physical_slot);
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }

  return action;
} /* qbi_svc_bc_ext_dssa_slot_mapping_s_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_dssa_slot_mapping_s_uim46_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
            MBIM_CID_MS_DEVICE_SLOT_MAPPING query

    @details This function handles activation and deactivation of 
             requested GW or 1x_pri slot 

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_mapping_s_uim46_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_slot_mapping_info_s *req = NULL;
  uim_switch_slot_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_slot_mapping_info_s *rsp = NULL;
  uint32 *slot_map = NULL;
  uint32 req_slot;
  qbi_mbim_offset_size_pair_s                 *ol_pair = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_switch_slot_resp_msg_v01 *)qmi_txn->rsp.data;
    req = (qbi_svc_bc_ext_slot_mapping_info_s *)qmi_txn->parent->req.data;
  ol_pair = (qbi_mbim_offset_size_pair_s*)((uint8*)req +
    sizeof(qbi_svc_bc_ext_slot_mapping_info_s));

  /* Extracting requested slot */
  req_slot = (uint32)(*((uint8*)req + ol_pair->offset));

  qbi_svc_bc_msuicc_set_slot_mapping_in_progress_flag(qmi_txn->ctx,FALSE);

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if (qmi_rsp->resp.error == QMI_ERR_NO_EFFECT_V01)
    {
      QBI_LOG_I_0("DSSA: Mapping is already in effect");
    }
    else
    {
      qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
      QBI_LOG_E_1("DSSA::E: Received error code %d from QMI", qmi_rsp->resp.error);
      return action;
    }
  }

  rsp = (qbi_svc_bc_ext_slot_mapping_info_s *)qbi_txn_alloc_rsp_buf(
    qmi_txn->parent, (sizeof(qbi_svc_bc_ext_slot_mapping_info_s)) +
    sizeof(qbi_mbim_offset_size_pair_s) * QBI_SVC_BC_EXT_DSSA_MAX_SUPPORTED_EXECUTORS);
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->map_count = QBI_SVC_BC_EXT_DSSA_MAX_SUPPORTED_EXECUTORS;
  ol_pair = (qbi_mbim_offset_size_pair_s*)((uint8*)rsp +
    sizeof(qbi_svc_bc_ext_slot_mapping_info_s));

  slot_map = (uint32 *)qbi_txn_rsp_databuf_add_field(
  qmi_txn->parent, ol_pair, 0, sizeof(uint32), NULL);
  QBI_CHECK_NULL_PTR_RET_ABORT(slot_map);

  *slot_map = req_slot;
  QBI_LOG_D_1("DSSA: Slot mapping complete. New slot is %d", *slot_map);
  qbi_txn_rsp_databuf_consolidate(qmi_txn->parent);

  action = QBI_SVC_ACTION_SEND_RSP;

  return action;
}/* qbi_svc_bc_ext_dssa_slot_mapping_s_uim46_rsp_cb */

/*! @} */

/*! @addtogroup MBIM_CID_MS_SLOT_INFO_STATUS
    @{ */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_dssa_slot_info_status_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_SLOT_INFO_STATUS query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_info_status_q_req
(
    qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_slot_info_req_s *req = NULL;
  uim_get_slots_status_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_ext_slot_info_req_s *)txn->req.data;

  if (req->slot_index > QBI_SVC_BC_EXT_DSSA_SLOT_2)
  {
    QBI_LOG_E_0("DSSA:Invalid Slot Index");
        txn->status = QBI_MBIM_STATUS_FAILURE;
  }
  else
  {
    qmi_req = (uim_get_slots_status_req_msg_v01 *)qbi_qmi_txn_alloc_ret_req_buf(
    txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_SLOTS_STATUS_REQ_V01,
        qbi_svc_bc_ext_dssa_slot_info_q_uim47_rsp_cb); 
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
    QBI_LOG_D_1("DSSA:sending slot status request %d",req->slot_index);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
}/* qbi_svc_bc_ext_dssa_slot_info_status_q_req */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_dssa_slot_info_q_prepare_rsp
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_SLOT_INFO_STATUS response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_dssa_slot_info_q_prepare_rsp
(
  qbi_txn_s           *txn,
  uint32              slot_idx,
  uim_physical_slot_status_type_v01  card_status,
  uim_physical_slot_information_type_v01  card_info
)
{
  qbi_svc_bc_ext_slot_info_rsp_s *rsp = NULL;
  qbi_svc_bc_ext_dssa_slot_info *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  info = (qbi_svc_bc_ext_dssa_slot_info *)txn->info;
  rsp = qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_ext_slot_info_rsp_s));
  QBI_CHECK_NULL_PTR_RET(rsp);

  rsp->slot_index = slot_idx;

  QBI_LOG_D_4("DSSA: index = %d, card_status = %d, is_euicc = %d, num_apps = %d", 
    rsp->slot_index, card_status.physical_card_status, card_info.is_euicc, card_info.num_app);

  switch (card_status.physical_card_status)
  {
  case UIM_PHYSICAL_CARD_STATE_ABSENT_V01:
    rsp->state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_EMPTY;
    break;

  case UIM_PHYSICAL_CARD_STATE_PRESENT_V01:
    if (card_status.physical_slot_state == UIM_PHYSICAL_SLOT_STATE_INACTIVE_V01)
    {
      if (card_info.is_euicc == TRUE)
    {
      rsp->state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE_ESIM_NOPROFILE;
        if (card_info.num_app > 0)
      {
        rsp->state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE_ESIM;
      }
    }
    else
    {
        rsp->state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_OFF;
    }
  }
    else
    {
      if (card_info.is_euicc == TRUE)
      {
        rsp->state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE_ESIM_NOPROFILE;
        if (card_info.num_app > 0)
  {
          rsp->state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE_ESIM;
  }
  }
  else
  {
        rsp->state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE;
      }
  }
    break;

  case UIM_PHYSICAL_CARD_STATE_UNKNOWN_V01:
  default:
    rsp->state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ERROR;
    break;
  }

  QBI_LOG_D_2("DSSA:Slot info: Card state %d at slot index %d", rsp->state, rsp->slot_index);
}/* qbi_svc_bc_ext_dssa_slot_info_q_prepare_rsp() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_dssa_slot_info_q_uim47_rsp_cb
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_SLOT_INFO_STATUS response

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_info_q_uim47_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_get_slots_status_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_bc_ext_slot_info_req_s *req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  req = (qbi_svc_bc_ext_slot_info_req_s *)qmi_txn->parent->req.data;
  qmi_rsp = (uim_get_slots_status_resp_msg_v01 *)qmi_txn->rsp.data;

  QBI_LOG_D_1("DSSA:Slot info: Received request for slot index %d",
    req->slot_index);

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("DSSA::E: Received error code %d from QMI",
      qmi_rsp->resp.error);

    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
  }
  else if (!qmi_rsp->physical_slot_status_valid)
  {
    QBI_LOG_E_0("DSSA::E: Card status invalid");
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
    }
    else
    {
    qbi_svc_bc_ext_dssa_slot_info_q_prepare_rsp(qmi_txn->parent,
      req->slot_index, qmi_rsp->physical_slot_status[req->slot_index], qmi_rsp->physical_slot_information[req->slot_index]);
  }

  return QBI_SVC_ACTION_SEND_RSP;
}/* qbi_svc_bc_ext_dssa_slot_info_q_uim47_rsp_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_dssa_slot_info_uim48_ind_cb
===========================================================================*/
/*!
    @brief Handles QMI_UIM_STATUS_CHANGE_IND

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_ext_dssa_slot_info_uim48_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  const uim_slot_status_change_ind_msg_v01 *qmi_ind = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_ext_dssa_info_status_cache *cache;
  uint32 new_slot_state;
  qbi_svc_bc_ext_slot_info_rsp_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);

  qmi_ind = (const uim_slot_status_change_ind_msg_v01 *)ind->buf->data;

  cache = qbi_svc_bc_ext_dssa_slot_info_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (qmi_ind->physical_slot_status_valid)
  {
    QBI_LOG_D_4("DSSA:first slot present card_status = %d app_len = %d slot_state = %d is_esim = %d",cache->card0.physical_card_status,cache->card0.app_info_len,cache->card0.physical_slot_state,cache->card0.is_esim);
    /* When indication comes , updating all cache entries for card 0 */
    cache->card0.physical_card_status = qmi_ind->physical_slot_status[0].physical_card_status;
    cache->card0.app_info_len = qmi_ind->physical_slot_information[0].num_app;
    cache->card0.physical_slot_state = qmi_ind->physical_slot_status[0].physical_slot_state;
    cache->card0.is_esim = qmi_ind->physical_slot_information[0].is_euicc;


    if (qmi_ind->physical_slot_status_len == 2)
    {
      /* When indication comes , updating all cache entries for card 1 */
      QBI_LOG_D_4("DSSA:second slot present card_status = %d app_len = %d slot_state = %d is_esim = %d",cache->card1.physical_card_status,cache->card1.app_info_len,cache->card1.physical_slot_state,cache->card1.is_esim);
      cache->card1.physical_card_status = qmi_ind->physical_slot_status[1].physical_card_status;
      cache->card1.app_info_len = qmi_ind->physical_slot_information[1].num_app;
      cache->card1.physical_slot_state = qmi_ind->physical_slot_status[1].physical_slot_state;
      cache->card1.is_esim = qmi_ind->physical_slot_information[1].is_euicc;
      qbi_svc_bc_ext_dssa_slot_info_force_card1_event(ind->txn->ctx);
    }

       new_slot_state = qbi_svc_bc_ext_dssa_slot_info_from_uicc_slot_state(&cache->card0);

      /* updating cache and checking slot state*/
      if (qbi_svc_bc_ext_dssa_slot_status_update_cache(&cache->card0, new_slot_state))
      {
        /* Forming response for card 0 */
        rsp = qbi_txn_alloc_rsp_buf(ind->txn, sizeof(qbi_svc_bc_ext_slot_info_rsp_s));
        QBI_CHECK_NULL_PTR_RET_ABORT(rsp);
        rsp->state = new_slot_state;
        rsp->slot_index = 0;
        action = QBI_SVC_ACTION_SEND_RSP;
      }
    }

  return action;
} /* qbi_svc_bc_ext_dssa_slot_info_uim48_ind_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_ext_dssa_slot_info_from_uicc_slot_state
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_SLOT_INFO_STATUS response

    @details This function returns the current state of the card

    @param cache

    @return state
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_ext_dssa_slot_info_from_uicc_slot_state
(
  qbi_svc_bc_ext_dssa_info_cache_each_slot *cache
)
{
  uint32 state;
/*-------------------------------------------------------------------------*/

  QBI_LOG_D_3("DSSA:  card_status = %d, is_euicc = %d, num_apps = %d",
     cache->physical_card_status, cache->is_esim, cache->app_info_len);
  switch (cache->physical_card_status)
  {
  case UIM_PHYSICAL_CARD_STATE_ABSENT_V01:
    state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_EMPTY;
    break;

  case UIM_PHYSICAL_CARD_STATE_PRESENT_V01:
    if (cache->physical_slot_state == UIM_PHYSICAL_SLOT_STATE_INACTIVE_V01)
    {
      if (cache->is_esim == TRUE)
      {
        state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE_ESIM_NOPROFILE;
        if (cache->app_info_len > 0)
        {
          state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE_ESIM;
        }
      }
      else
      {
        state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_OFF;
      }
    }
    else
    {
      if (cache->is_esim == TRUE)
    {
      state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE_ESIM_NOPROFILE;
      if (cache->app_info_len > 0)
      {
        state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE_ESIM;
      }
    }
    else
    {
      state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ACTIVE;
    }
    }
    break;
  case UIM_PHYSICAL_CARD_STATE_UNKNOWN_V01:
  default:
    state = QBI_SVC_MBIM_MS_UICC_SLOT_STATE_ERROR;

    break;
  }
  QBI_LOG_D_1("DSSA: Card State = %d", state);

  return state;
}/* qbi_svc_bc_ext_dssa_slot_info_from_uicc_slot_state() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_slot_info_force_card1_event
===========================================================================*/
/*!
    @brief Forces sending an QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO_STATUS
    event based on the current contents of the cache

    @details This function forces the event for card 1 if their is state
    change

    @param ctx

    @return none
*/
/*=========================================================================*/
static void qbi_svc_bc_ext_dssa_slot_info_force_card1_event
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uim_get_atr_req_msg_v01 *qmi_req = NULL;
  qbi_svc_bc_ext_slot_info_rsp_s *rsp = NULL;
  qbi_svc_bc_ext_dssa_info_status_cache *cache;
  uint32 new_slot_state;
/*-------------------------------------------------------------------------*/
  txn = qbi_txn_alloc_event(ctx, QBI_SVC_ID_BC_EXT, QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO_STATUS);
  QBI_CHECK_NULL_PTR_RET(txn);

  cache = qbi_svc_bc_ext_dssa_slot_info_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

    new_slot_state = qbi_svc_bc_ext_dssa_slot_info_from_uicc_slot_state(&cache->card1);

    /* updating cache and checking slot state */
    if (qbi_svc_bc_ext_dssa_slot_status_update_cache(&cache->card1, new_slot_state))
    {
      /* forming response for card 1*/
      rsp = qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_bc_ext_slot_info_rsp_s));
      QBI_CHECK_NULL_PTR_RET(rsp);
      rsp->state = new_slot_state;
      rsp->slot_index = 1;
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_bc_ext_dssa_slot_info_force_card1_event() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_dssa_slot_status_update_cache
===========================================================================*/
/*!
    @brief Updates the cached information associated with
    MBIM_CID_SUBSCRIBER_READY_STATUS

    @details

    @param ctx
    @param ready_state New ReadyState
    @param is_perso_locked New flag indicating whether the SIM is locked
    due to a personalization key

    @return boolean TRUE if the cache changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_bc_ext_dssa_slot_status_update_cache
(
  qbi_svc_bc_ext_dssa_info_cache_each_slot *cache,
  uint32 slot_state
)
{
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/

  if (cache->slot_state != slot_state)
  {
    QBI_LOG_D_2("DSSA:slot info changed from %d to %d ", 
      cache->slot_state, slot_state);
    cache->slot_state = slot_state;
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_bc_ext_dssa_slot_status_update_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_ext_dssa_slot_info_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity Extension device
    service's cache

    @details

    @param ctx

    @return qbi_svc_bc_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
static qbi_svc_bc_ext_dssa_info_status_cache *qbi_svc_bc_ext_dssa_slot_info_cache_get
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_bc_ext_dssa_info_status_cache *cache = NULL;
/*-------------------------------------------------------------------------*/
  cache = (qbi_svc_bc_ext_dssa_info_status_cache *)qbi_svc_cache_get(ctx, QBI_SVC_ID_MM);
  if (cache == NULL)
  {
    cache = qbi_svc_cache_alloc(ctx, QBI_SVC_ID_MM, 
      sizeof(qbi_svc_bc_ext_dssa_info_status_cache));

    if (cache == NULL)
    {
      QBI_LOG_E_0("DSSA:Couldn't allocate cache for slot info status!");
      return NULL;
    }
  }

  return cache;
} /* qbi_svc_bc_ext_dssa_slot_info_cache_get() */

/*! @} */
