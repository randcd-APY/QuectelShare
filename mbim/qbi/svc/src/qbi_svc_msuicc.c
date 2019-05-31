/*!
  @file
  qbi_svc_msuicc.c

  @brief
  Microsoft UICC device service implementation, based on the "MBIM EXTENSION
  FOR LOW-LEVEL UICC ACCESS INTERFACE SPECIFICATION" document from Microsoft.
  This device service provides low-level access to the UICC.
*/

/*=============================================================================

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
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
10/26/17  mm   Fixed issue related to open channel in Non-telecom mode
09/11/17  mm   Fix to block UICC commands during slot mapping
07/24/17  mm   Fixed issue related to close channel
07/05/17  mm   Fixed issue related to buffer overflow for slots
07/05/17  vs   Fixed issue related channel open in pass through mode
05/23/17  mm   Added NV updation for executor index at service init
04/29/17  mm   Added uicc reset functionality
03/22/17  rv   Updated APDU cmds to be independent of subscription state
03/03/17  rv   Handle scenario when get profile response is more than 1024 bytes
02/21/17  rv   Handle scenario when card sends only status bytes
02/14/17  vs   On failure in open channel return available sw1, sw2 and status
07/28/16  hz   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_msuicc.h"
#include "qbi_svc_msuicc_mbim.h"
#include "qbi_svc_bc_ext_mbim.h"
#include "qbi_svc_bc_ext.h"
#include "qbi_svc_bc_ext_dssa.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_nv_store.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_svc_bc.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_svc_bc_sim.h"
#include "qbi_txn.h"
#include "user_identity_module_v01.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/
#define MAX_BASIC_CHANNELS    3
#define MAX_EXTENDED_CHANNELS 19
#define MAX_SUPPORTED_SLOTS   2
/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! Cache used locally by CIDs processed in this file. */
typedef struct qbi_svc_msuicc_cache_struct {
  /*! List containing one qbi_svc_msuicc_logical_channel_s element for each
      logical channel opened. */
  qbi_util_list_s logical_channel_list;

  /*! Store UIM_TERMINAL_CAPABILITY_RESP message from last sucessful
      MBIM_CID_MS_UICC_TERMINAL_CAPABILITY set request. */
  uim_terminal_capability_resp_msg_v01 qmi_terminal_capability_resp;

  /*! Store token and data trunks from QMI_UIM_SEND_APDU_RESP and
      QMI_UIM_SEND_APDU_IND for reconstructing the APDU response. */
  struct {
    uint32_t token;
    uint8   *apdu;
    uint16_t total_length;
    uint16_t bytes_copied;
  } apdu_response;

  boolean slot_mapping_in_progress;
} qbi_svc_msuicc_cache_s;

typedef struct {
  uint32 channel_id;
  uint32 channel_group;
} qbi_svc_msuicc_channel_req_info;
/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_msuicc_cache_s *qbi_svc_msuicc_cache_get
(
  const qbi_ctx_s *ctx
);

static boolean qbi_svc_msuicc_logical_channel_add_to_cache
(
  qbi_ctx_s *ctx,
  uint32     channel_id,
  uint32     channel_group
);

static boolean qbi_svc_msuicc_logical_channel_id_is_valid
(
  qbi_ctx_s *ctx,
  uint32     channel_id
);

static boolean qbi_svc_msuicc_logical_channel_id_or_group_is_valid
(
  qbi_ctx_s *ctx,
  uint32     channel_id,
  uint32     channel_group
);

static qbi_svc_action_e qbi_svc_msuicc_open
(
  qbi_txn_s *txn
);

static void qbi_svc_msuicc_reverse_ol_pair
(
  qbi_mbim_offset_size_pair_s *ol_pair
);

/*! @addtogroup MBIM_CID_MS_UICC_ATR
    @{ */

static qbi_svc_action_e qbi_svc_msuicc_atr_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_msuicc_atr_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_atr_q_uim41_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_OPEN_CHANNEL
    @{ */

static boolean qbi_svc_msuicc_open_channel_p2_to_file_control_information
(
  uint32                                 select_p2_arg,
  uim_file_control_information_enum_v01 *file_control_information
);

static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_uim3f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_apdu_req
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_1
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_2
(
  qbi_qmi_txn_s *qmi_txn
);
/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_CLOSE_CHANNEL
    @{ */

static qbi_svc_action_e qbi_svc_msuicc_close_channel_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_msuicc_close_channel_s_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_close_channel_s_uim3f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_close_channel_s_apdu_req
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_close_channel_s_apdu_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_APDU
    @{ */

static void qbi_svc_msuicc_apdu_s_clear_apdu_in_cache
(
  qbi_svc_msuicc_cache_s *cache
);

static qbi_svc_action_e qbi_svc_msuicc_apdu_s_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_msuicc_apdu_s_req_is_valid
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_msuicc_apdu_s_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_apdu_s_uim3b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_TERMINAL_CAPABILITY
    @{ */

static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_q_build_rsp
(
  qbi_txn_s                                  *txn,
  const uim_terminal_capability_resp_msg_v01 *qmi_rsp
);

static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_q_uim6a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_s_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_s_uim6a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_RESET
    @{ */

static qbi_svc_action_e qbi_svc_msuicc_reset_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_msuicc_reset_q_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_reset_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_msuicc_reset_s_req_get_card_status_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_msuicc_reset_s_req_power_down_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_muicc_reset_s_req_power_down_ind_rsp_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_msuicc_reset_s_req_power_up_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_msuicc_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_msuicc_cmd_hdlr_tbl[] = {
  /* MBIM_CID_MS_UICC_ATR */
  {qbi_svc_msuicc_atr_q_req, 0, NULL, 0},
  /* MBIM_CID_MS_UICC_OPEN_CHANNEL */
  {NULL, 0,
   qbi_svc_msuicc_open_channel_s_req,
     sizeof(qbi_svc_msuicc_open_channel_s_req_s)},
  /* MBIM_CID_MS_UICC_CLOSE_CHANNEL */
  {NULL, 0,
   qbi_svc_msuicc_close_channel_s_req,
     sizeof(qbi_svc_msuicc_close_channel_s_req_s)},
  /* MBIM_CID_MS_UICC_APDU */
  {NULL, 0,
   qbi_svc_msuicc_apdu_s_req, sizeof(qbi_svc_msuicc_apdu_s_req_s)},
  /* MBIM_CID_MS_UICC_TERMINAL_CAPABILITY */
  {qbi_svc_msuicc_terminal_capability_q_req, 0,
   qbi_svc_msuicc_terminal_capability_s_req,
     sizeof(qbi_svc_msuicc_terminal_capability_s_req_s)},
  /* MBIM_CID_MS_UICC_RESET */
  {qbi_svc_msuicc_reset_q_req, 0, qbi_svc_msuicc_reset_s_req, 0}
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_bc_msuicc_set_slot_mapping_in_progress_flag
===========================================================================*/
/*!
    @brief This function sets the slot mapping in progress flag

    @details If this flag is set to true then slot mapping currently
    is in progress. So that all msuicc operations should not take place.
    till this flag is set back to false.
    If this flag is set to false then slot mapping is completed or
    it is not in progress.

    @param ctx, flag

    @return none
*/
/*=========================================================================*/
void qbi_svc_bc_msuicc_set_slot_mapping_in_progress_flag
(
  const qbi_ctx_s *ctx,
  boolean flag
)
{
  qbi_svc_msuicc_cache_s *cache = NULL;
/*=========================================================================*/

  QBI_CHECK_NULL_PTR_RET(ctx);

  cache = qbi_svc_msuicc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  cache->slot_mapping_in_progress = flag;
  return;
}/* qbi_svc_bc_msuicc_set_slot_mapping_in_progress_flag */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the device service's cache

    @details

    @param ctx

    @return qbi_svc_msuicc_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
static qbi_svc_msuicc_cache_s *qbi_svc_msuicc_cache_get
(
  const qbi_ctx_s *ctx
)
{
  return (qbi_svc_msuicc_cache_s *) qbi_svc_cache_get(
    ctx, QBI_SVC_ID_MSUICC);
} /* qbi_svc_msuicc_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_logical_channel_add_to_cache
===========================================================================*/
/*!
    @brief Add a new logical chanel to the collection of active channels in
    cache

    @details

    @param ctx
    @param channel_id
    @param channel_group

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_msuicc_logical_channel_add_to_cache
(
  qbi_ctx_s *ctx,
  uint32     channel_id,
  uint32     channel_group
)
{
  qbi_svc_msuicc_cache_s *cache;
  qbi_svc_msuicc_logical_channel_s *logical_channel;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_msuicc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  logical_channel =
    QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_msuicc_logical_channel_s));
  QBI_CHECK_NULL_PTR_RET_FALSE(logical_channel);

  logical_channel->channel_id    = channel_id;
  logical_channel->channel_group = channel_group;

  if (!qbi_util_list_push_back_aliased(
        &cache->logical_channel_list, &logical_channel->list_entry))
  {
    QBI_LOG_E_2("Couldn't add logical channel %d (group %d) to active list!",
                channel_id, channel_group);
    QBI_MEM_FREE(logical_channel);
  }
  else
  {
    QBI_LOG_I_2("Saved logical channel %d group %d to logical channel list",
                channel_id, channel_group);
    success = TRUE;
  }

  return success;
} /* qbi_svc_msuicc_logical_channel_add_to_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_logical_channel_id_is_valid
===========================================================================*/
/*!
    @brief Check if a logical channel number is valid by scanning active
    channel list in cache

    @details

    @param ctx
    @param channel_id

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_msuicc_logical_channel_id_is_valid
(
  qbi_ctx_s *ctx,
  uint32     channel_id
)
{
  qbi_svc_msuicc_cache_s *cache;
  qbi_util_list_iter_s iter;
  qbi_svc_msuicc_logical_channel_s *logical_channel;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_msuicc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (channel_id != 0)
  {
    qbi_util_list_iter_init(&cache->logical_channel_list, &iter);
    while ((logical_channel = (qbi_svc_msuicc_logical_channel_s *)
              qbi_util_list_iter_next(&iter)) != NULL)
    {
      if (logical_channel->channel_id == channel_id)
      {
        success = TRUE;
        break;
      }
    }
  }

  return success;
} /* qbi_svc_msuicc_logical_channel_id_is_valid() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_logical_channel_id_or_group_is_valid
===========================================================================*/
/*!
    @brief Check if a logical channel number is valid or there are channels
    associated with a channel group by scanning active channel list in cache

    @details

    @param ctx
    @param uint32
    @param uint32

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_msuicc_logical_channel_id_or_group_is_valid
(
  qbi_ctx_s *ctx,
  uint32     channel_id,
  uint32     channel_group
)
{
  qbi_svc_msuicc_cache_s *cache;
  qbi_util_list_iter_s iter;
  qbi_svc_msuicc_logical_channel_s *logical_channel;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_msuicc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (channel_id != 0)
  {
    return qbi_svc_msuicc_logical_channel_id_is_valid(ctx, channel_id);
  }
  else
  {
    qbi_util_list_iter_init(&cache->logical_channel_list, &iter);
    while ((logical_channel = (qbi_svc_msuicc_logical_channel_s *)
              qbi_util_list_iter_next(&iter)) != NULL)
    {
      if (logical_channel->channel_group == channel_group)
      {
        success = TRUE;
        break;
      }
    }
  }

  return success;
} /* qbi_svc_msuicc_logical_channel_id_or_group_is_valid() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_logical_channel_remove_from_cache
===========================================================================*/
/*!
    @brief Remove a logical chanel from the collection of active channels in
    cache

    @details

    @param ctx
    @param channel_id

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_msuicc_logical_channel_remove_from_cache
(
  qbi_ctx_s *ctx,
  uint32     channel_id
)
{
  qbi_svc_msuicc_cache_s *cache;
  qbi_svc_msuicc_logical_channel_s *logical_channel;
  qbi_util_list_iter_s iter;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_msuicc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (channel_id == 0)
  {
    QBI_LOG_E_1("Invalid channel id %d", channel_id);
  }
  else
  {
    qbi_util_list_iter_init(&cache->logical_channel_list, &iter);
    while ((logical_channel = (qbi_svc_msuicc_logical_channel_s *)
              qbi_util_list_iter_next(&iter)) != NULL)
    {
      if (logical_channel->channel_id == channel_id)
      {
        QBI_LOG_I_1("Removed channel id %d from logical channel list",
                    logical_channel->channel_id);
        qbi_util_list_iter_remove(&iter);
        QBI_MEM_FREE(logical_channel);
        success = TRUE;
      }
    }
  }

  return success;
} /* qbi_svc_msuicc_logical_channel_remove_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (qbi_svc_cache_alloc(
        txn->ctx, QBI_SVC_ID_MSUICC, sizeof(qbi_svc_msuicc_cache_s)) == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate cache");
  }
  if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_UIM))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_msuicc_open() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_reverse_ol_pair
===========================================================================*/
/*!
    @brief Reverse offset and length fields in an OL_PAIR

    @details

    @param ol_pair
*/
/*=========================================================================*/
static void qbi_svc_msuicc_reverse_ol_pair
(
  qbi_mbim_offset_size_pair_s *ol_pair
)
{
  uint32 temp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ol_pair);

  temp = ol_pair->size;
  ol_pair->size = ol_pair->offset;
  ol_pair->offset = temp;
} /* qbi_svc_msuicc_reverse_ol_pair() */

/*! @addtogroup MBIM_CID_MS_UICC_ATR
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_atr_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_ATR query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_atr_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
    /* QMI_UIM_GET_CARD_STATUS (0x2f) */
    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
      qbi_svc_msuicc_atr_q_uim2f_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;

  return action;
} /* qbi_svc_msuicc_atr_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_atr_q_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for MBIM_CID_MS_UICC_ATR
    query request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_atr_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uim_get_atr_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_msuicc_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_msuicc_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (TRUE == cache->slot_mapping_in_progress)
  {
    QBI_LOG_E_0("Slot mapping is in progress, can't perform ATR request");
    qmi_txn->parent->status = QBI_MBIM_STATUS_BUSY;
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Missing card status TLV!");
  }
  else
  {
    qmi_req = (uim_get_atr_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
      qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_GET_ATR_REQ_V01,
      qbi_svc_msuicc_atr_q_uim41_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_msuicc_atr_q_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_atr_q_uim41_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_ATR_RESP for MBIM_CID_MS_UICC_ATR
    query request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_atr_q_uim41_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_msuicc_atr_q_rsp_s *rsp;
  uim_get_atr_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_get_atr_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    if (qmi_rsp->atr_value_valid)
    {
      rsp = (qbi_svc_msuicc_atr_q_rsp_s *) qbi_txn_alloc_rsp_buf(
        qmi_txn->parent, sizeof(qbi_svc_msuicc_atr_q_rsp_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

      if (!qbi_txn_rsp_databuf_add_field(
            qmi_txn->parent, &rsp->atr_data, 0, qmi_rsp->atr_value_len,
            qmi_rsp->atr_value))
      {
        QBI_LOG_E_0("Couldn't add ATR data to response!");
      }
      else
      {
        qbi_svc_msuicc_reverse_ol_pair(&rsp->atr_data);
        action = QBI_SVC_ACTION_SEND_RSP;
      }
    }
  }

  return action;
} /* qbi_svc_msuicc_atr_q_uim41_rsp_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_OPEN_CHANNEL
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_open_channel_p2_to_file_control_information
===========================================================================*/
/*!
    @brief Convert P2 value to coresponding file control information index

    @details

    @param uint32

    @return uim_file_control_information_enum_v01
*/
/*=========================================================================*/
static boolean qbi_svc_msuicc_open_channel_p2_to_file_control_information
(
  uint32                                 select_p2_arg,
  uim_file_control_information_enum_v01 *file_control_information
)
{
  boolean result = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(file_control_information);

  if (select_p2_arg == 0x00)
  {
    *file_control_information = UIM_FILE_CONTROL_INFORMATION_FCI_V01;
  }
  else if (select_p2_arg == 0x04)
  {
    *file_control_information = UIM_FILE_CONTROL_INFORMATION_FCP_V01;
  }
  else if (select_p2_arg == 0x08)
  {
    *file_control_information = UIM_FILE_CONTROL_INFORMATION_FMD_V01;
  }
  else if (select_p2_arg == 0x0C)
  {
    *file_control_information = UIM_FILE_CONTROL_INFORMATION_NO_DATA_V01;
  }
  else if (select_p2_arg == 0x10)
  {
    *file_control_information = UIM_FILE_CONTROL_INFORMATION_FCI_WITH_INTERFACES_V01;
  }
  else
  {
    QBI_LOG_E_1("p2 value %d not supported", select_p2_arg);
    result = FALSE;
  }

  return result;
} /* qbi_svc_msuicc_open_channel_p2_to_file_control_information() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_open_channel_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_OPEN_CHANNEL set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_msuicc_open_channel_s_req_s *req;
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_msuicc_open_channel_s_req_s *) txn->req.data;
  qbi_svc_msuicc_reverse_ol_pair(&req->app_id);

  /* QMI_UIM_GET_CARD_STATUS (0x2f) */
  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
    qbi_svc_msuicc_open_channel_s_uim2f_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  action = QBI_SVC_ACTION_SEND_QMI_REQ;

  return action;
} /* qbi_svc_msuicc_open_channel_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_open_channel_s_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_MS_UICC_OPEN_CHANNEL set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_msuicc_open_channel_s_req_s *req;
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uim_logical_channel_req_msg_v01 *qmi_req;
  uim_file_control_information_enum_v01 file_control_information;
  const uint8 *app_id;
  qbi_svc_msuicc_channel_req_info *info = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint32 card_status_index = 0;
  qbi_svc_msuicc_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  cache = qbi_svc_msuicc_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;
  card_status_index = qbi_svc_bc_sim_get_card_status_index();

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Missing card status TLV!");
  }
  else if (TRUE == cache->slot_mapping_in_progress)
  {
    QBI_LOG_E_0("Slot mapping is in progress, can't perform open channel");
    qmi_txn->parent->status = QBI_MBIM_STATUS_BUSY;
  }
  else
  {
    req = (qbi_svc_msuicc_open_channel_s_req_s *)qmi_txn->parent->req.data;
    if (qmi_rsp->card_mode[card_status_index] == UIM_CARD_MODE_TELECOM_CARD_V01)
    {
      if (qbi_svc_msuicc_open_channel_p2_to_file_control_information(
            req->select_p2_arg, &file_control_information))
      {
        qmi_req = (uim_logical_channel_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_LOGICAL_CHANNEL_REQ_V01,
          qbi_svc_msuicc_open_channel_s_uim3f_rsp_cb);
        QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

        qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
        qmi_req->file_control_information_valid = TRUE;
        qmi_req->file_control_information = file_control_information;

        app_id = qbi_txn_req_databuf_get_field(qmi_txn->parent, &req->app_id, 0,
          QBI_SVC_MSUICC_OPEN_CHANNEL_APP_ID_MAX_LEN_BYTES);
        QBI_CHECK_NULL_PTR_RET_ABORT(app_id);

        qmi_req->aid_valid = TRUE;
        qmi_req->aid_len = req->app_id.size;
        QBI_MEMSCPY(qmi_req->aid, QMI_UIM_AID_MAX_V01, app_id, req->app_id.size);

        action = QBI_SVC_ACTION_SEND_QMI_REQ;
      }
    }
    else if (qmi_rsp->card_mode[card_status_index] == UIM_CARD_MODE_NON_TELECOM_CARD_V01)
    {
      qmi_txn->parent->info = QBI_MEM_MALLOC_CLEAR(
        sizeof(qbi_svc_msuicc_channel_req_info));
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
      info = (qbi_svc_msuicc_channel_req_info *)qmi_txn->parent->info;
      info->channel_group = req->channel_group;
      action = qbi_svc_msuicc_open_channel_s_apdu_req(qmi_txn);
    }
  }

  return action;
} /* qbi_svc_msuicc_open_channel_s_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_open_channel_s_apdu_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_OPEN_CHANNEL set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_apdu_req
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_send_apdu_req_msg_v01 *qmi_req;
  uint8 command[5] = {0x00, 0x70, 0x00, 0x00, 0x01};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  QBI_LOG_D_0("In qbi_svc_msuicc_open_channel_s_apdu_req");
    qmi_req = (uim_send_apdu_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_SEND_APDU_REQ_V01,
        qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_1);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
    qmi_req->procedure_bytes_valid = TRUE;
    qmi_req->procedure_bytes = UIM_APDU_SKIP_PROCEDURE_BYTES_V01;
    /*Manage Channel APDU length will always be 5 bytes hence coded to 5*/
    qmi_req->apdu_len = 5;
    QBI_MEMSCPY(qmi_req->apdu, QMI_UIM_APDU_DATA_MAX_V01, command, 5);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
}/* qbi_svc_msuicc_open_channel_s_apdu_req */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_open_channel_s_uim3f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_LOGICAL_CHANNEL_RESP for
    MBIM_CID_MS_UICC_OPEN_CHANNEL set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_uim3f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  const qbi_svc_msuicc_open_channel_s_req_s *req;
  qbi_svc_msuicc_open_channel_s_rsp_s *rsp;
  const uim_logical_channel_resp_msg_v01 *qmi_rsp;
  uint8 *select_response;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  qmi_rsp = (uim_logical_channel_resp_msg_v01 *) qmi_txn->rsp.data;

  do
  {
    if (qmi_rsp->resp.result == QMI_RESULT_FAILURE_V01)
    {
      QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
      if (qmi_rsp->resp.error == QMI_ERR_INSUFFICIENT_RESOURCES_V01)
      {
        qmi_txn->parent->status = QBI_MBIM_STATUS_MS_NO_LOGICAL_CHANNEL;
      }
      else if (qmi_rsp->resp.error == QMI_ERR_SIM_FILE_NOT_FOUND_V01 ||
               qmi_rsp->resp.error == QMI_ERR_ACCESS_DENIED_V01 ||
               qmi_rsp->resp.error == QMI_ERR_INCOMPATIBLE_STATE_V01)
      {
        qmi_txn->parent->status = QBI_MBIM_STATUS_MS_SELECT_FAILED;
      }
      else
      {
        QBI_LOG_E_0("No response payload");
        break;
      }
    }

    req = (qbi_svc_msuicc_open_channel_s_req_s *) qmi_txn->parent->req.data;

    if (qmi_rsp->card_result_valid)
    {
      rsp = qbi_txn_alloc_rsp_buf(qmi_txn->parent,
              sizeof(qbi_svc_msuicc_open_channel_s_rsp_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

      rsp->status[0] = qmi_rsp->card_result.sw1;
      rsp->status[1] = qmi_rsp->card_result.sw2;

      if (qmi_rsp->channel_id_valid)
      {
        rsp->channel = qmi_rsp->channel_id;
        if (!qbi_svc_msuicc_logical_channel_add_to_cache(
              qmi_txn->ctx, qmi_rsp->channel_id, req->channel_group))
        {
          QBI_LOG_E_0("Couldn't save logical channel info to cache!");
        }
      }

      if(qmi_rsp->select_response_valid)
      {
        select_response = qbi_txn_rsp_databuf_add_field(
          qmi_txn->parent, &rsp->response, 0,
          qmi_rsp->select_response_len, qmi_rsp->select_response);

        QBI_CHECK_NULL_PTR_RET_ABORT(select_response);

        qbi_svc_msuicc_reverse_ol_pair(&rsp->response);
       }

      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }while (0);

  return action;
} /* qbi_svc_msuicc_open_channel_s_uim3f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_1
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_LOGICAL_CHANNEL_RESP for
    MBIM_CID_MS_UICC_OPEN_CHANNEL set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_1
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_send_apdu_req_msg_v01 *qmi_req;
  uim_send_apdu_resp_msg_v01 *qmi_rsp;
  qbi_svc_msuicc_open_channel_s_req_s *req;
  uint8 *app_id;
  uint8 apdu_header[5] = { 0, };
  qbi_svc_msuicc_channel_req_info *info = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_msuicc_open_channel_s_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  qmi_rsp = (uim_send_apdu_resp_msg_v01 *) qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (qmi_rsp->apdu_len == 2) 
  {
    /* 2 byte response means error for channel open */
    rsp = qbi_txn_alloc_rsp_buf(qmi_txn->parent,
            sizeof(qbi_svc_msuicc_open_channel_s_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);
    rsp->status[0] = qmi_rsp->apdu[qmi_rsp->apdu_len - 1];
    rsp->status[1] = qmi_rsp->apdu[qmi_rsp->apdu_len];
    qmi_txn->parent->status = QBI_MBIM_STATUS_MS_NO_LOGICAL_CHANNEL;
    QBI_LOG_E_1("Invalid apdu length %d",qmi_rsp->apdu_len);
  }
  else if (qmi_rsp->apdu_len == 3) 
  {
    /* 3 byte response means success and channel id obtained */
    QBI_LOG_D_0("Sending Second apdu req");
    info = (qbi_svc_msuicc_channel_req_info *)qmi_txn->parent->info;
    info->channel_id = qmi_rsp->apdu[0];/* 0th byte is channel id */
    req = (qbi_svc_msuicc_open_channel_s_req_s *)qmi_txn->parent->req.data;
    /* once channel open is successful then SELECT AID should be issued. 
       to SELECT the requested AID */
    qmi_req = (uim_send_apdu_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_SEND_APDU_REQ_V01,
        qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_2);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
    QBI_LOG_D_1("Valid APDU Length qmi_rsp->apdu[0] = %d",qmi_rsp->apdu[0]);
    qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
    qmi_req->procedure_bytes_valid = TRUE;
    qmi_req->procedure_bytes = UIM_APDU_SKIP_PROCEDURE_BYTES_V01;
    /* APDU length being 5 byte APDU header and payload being APP ID */
    qmi_req->apdu_len = 5 + req->app_id.size;
    /* Per ETSI 102.221 Class byte 0x00, Select instruction byte 0xA4, 
       Selection by AID 0x04 */

    if(info->channel_id <= MAX_BASIC_CHANNELS)
    {
      /* For basic logical channels,
         * b7 is 0.
         * for inter-industry, b8 is 0.
         * for non inter-industry (proprietary), b8 is 1. */
      /* Since SELECT is inter-industry we keep b8 as 0 */
      apdu_header[0] = info->channel_id ;
    }
    else if(info->channel_id <= MAX_EXTENDED_CHANNELS)
    {
      /* For extended logical channels,
         * b7 is 1.
         * for inter-industry, b8 is 0.
         * for non inter-industry (proprietary), b8 is 1. */
      /* Since SELECT is inter-industry we keep b8 as 0 */
      apdu_header[0] = (0x40 + info->channel_id - (MAX_BASIC_CHANNELS + 1));
    }
    QBI_LOG_D_2("Channel ID %d class byte %d",info->channel_id, apdu_header[0]);
    apdu_header[1] = 0xA4;
    apdu_header[2] = 0x04;
    apdu_header[3] = (uint8)req->select_p2_arg;
    apdu_header[4] = req->app_id.size;

    QBI_MEMSCPY(qmi_req->apdu, QMI_UIM_APDU_DATA_MAX_V01, apdu_header, 5);

    if (req->app_id.size != 0) 
    {
      QBI_LOG_D_1("qmi_req->apdu_len = %d ",qmi_req->apdu_len);
      app_id = qbi_txn_req_databuf_get_field(qmi_txn->parent, &req->app_id, 0,
        QBI_SVC_MSUICC_OPEN_CHANNEL_APP_ID_MAX_LEN_BYTES);
      QBI_CHECK_NULL_PTR_RET_ABORT(app_id);

      QBI_MEMSCPY(&(qmi_req->apdu[5]), 
                   (QMI_UIM_APDU_DATA_MAX_V01 - 5), app_id, req->app_id.size);
    }
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  return action;
} /* qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_1 */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_2
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_LOGICAL_CHANNEL_RESP for
    MBIM_CID_MS_UICC_OPEN_CHANNEL set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_2
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uint8 *select_response;
  uim_send_apdu_resp_msg_v01 *qmi_rsp;
  qbi_svc_msuicc_channel_req_info *info = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_msuicc_open_channel_s_rsp_s *rsp = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  QBI_LOG_D_0("In qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_2");

  qmi_rsp = (uim_send_apdu_resp_msg_v01 *) qmi_txn->rsp.data;
  rsp = qbi_txn_alloc_rsp_buf(qmi_txn->parent,
            sizeof(qbi_svc_msuicc_open_channel_s_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    rsp->status[0] = qmi_rsp->apdu[qmi_rsp->apdu_len - 1];
    rsp->status[1] = qmi_rsp->apdu[qmi_rsp->apdu_len];

  if (qmi_rsp->resp.result == QMI_RESULT_FAILURE_V01 )
  {
    qmi_txn->parent->status = QBI_MBIM_STATUS_MS_SELECT_FAILED;
    QBI_LOG_E_1("Received error code %d from QMI",
      qmi_rsp->resp.error);
  }
  else
  {
    info = (qbi_svc_msuicc_channel_req_info *)qmi_txn->parent->info;

    QBI_LOG_D_2("In info->channel_id = %d info->channel_group = %d",
                info->channel_id,info->channel_group);
    if (!qbi_svc_msuicc_logical_channel_add_to_cache(
          qmi_txn->ctx, info->channel_id, info->channel_group))
    {
      QBI_LOG_E_0("Couldn't save logical channel info to cache!");
    }
    else
    {
      rsp->channel = info->channel_id;
    }

    /* Copying apdu only when it's length is greater than 2 */
    if (qmi_rsp->apdu_len > 2)
    {
      select_response = qbi_txn_rsp_databuf_add_field(
        qmi_txn->parent, &rsp->response, 0,
        qmi_rsp->apdu_len - 2, qmi_rsp->apdu);

      QBI_CHECK_NULL_PTR_RET_ABORT(select_response);

      qbi_svc_msuicc_reverse_ol_pair(&rsp->response);
    }
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
}/* qbi_svc_msuicc_open_channel_s_apdu_rsp_cb_2 */

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_CLOSE_CHANNEL
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_close_channel_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_CLOSE_CHANNEL set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_close_channel_s_req
(
  qbi_txn_s *txn
)
{
  const qbi_svc_msuicc_close_channel_s_req_s *req;
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_msuicc_close_channel_s_req_s *) txn->req.data;
  if (!(qbi_svc_msuicc_logical_channel_id_or_group_is_valid(
               txn->ctx, req->channel, req->channel_group)))
  {
    QBI_LOG_E_0("Invalid channel id or channel group");
    txn->status = QBI_MBIM_STATUS_MS_INVALID_LOGICAL_CHANNEL;
  }
  else
  {
    /* QMI_UIM_GET_CARD_STATUS (0x2f) */
    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
      qbi_svc_msuicc_close_channel_s_uim2f_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_msuicc_close_channel_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_close_channel_s_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_MS_UICC_CLOSE_CHANNEL set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_close_channel_s_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  const qbi_svc_msuicc_close_channel_s_req_s *req;
  const uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uim_logical_channel_req_msg_v01 *qmi_req;
  qbi_svc_msuicc_cache_s *cache = NULL;
  qbi_util_list_iter_s iter;
  qbi_svc_msuicc_logical_channel_s *logical_channel;
  qbi_svc_msuicc_channel_req_info *info = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint32 card_status_index;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  cache = qbi_svc_msuicc_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;
  card_status_index = qbi_svc_bc_sim_get_card_status_index();

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Missing card status TLV!");
  }
  else if (TRUE == cache->slot_mapping_in_progress)
  {
    QBI_LOG_E_0("Slot mapping is in progress, can't perform close channel");
    qmi_txn->parent->status = QBI_MBIM_STATUS_BUSY;
  }
  else
  {
    /* Close a logical channel or close all logical channels in the same
       channel group */
    req = (qbi_svc_msuicc_close_channel_s_req_s *) qmi_txn->parent->req.data;

    if (qmi_rsp->card_mode[card_status_index] == UIM_CARD_MODE_TELECOM_CARD_V01)
    {
      if (req->channel != 0)
      {
        qmi_req = (uim_logical_channel_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
            qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_LOGICAL_CHANNEL_REQ_V01,
            qbi_svc_msuicc_close_channel_s_uim3f_rsp_cb);
        QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

        qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
        qmi_req->channel_id_valid = TRUE;
        qmi_req->channel_id = req->channel;
      }
      else
      {
        qbi_util_list_iter_init(&cache->logical_channel_list, &iter);
        while ((logical_channel = (qbi_svc_msuicc_logical_channel_s *)
                  qbi_util_list_iter_next(&iter)) != NULL)
        {
          if (logical_channel->channel_group == req->channel_group)
          {
            qmi_req = (uim_logical_channel_req_msg_v01 *)
              qbi_qmi_txn_alloc_ret_req_buf(
                qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_LOGICAL_CHANNEL_REQ_V01,
                qbi_svc_msuicc_close_channel_s_uim3f_rsp_cb);
            QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

            qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
            qmi_req->channel_id_valid = TRUE;
            qmi_req->channel_id = logical_channel->channel_id;
          }
        }
      }
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
    else if (qmi_rsp->card_mode[card_status_index] == UIM_CARD_MODE_NON_TELECOM_CARD_V01)
    {
      qmi_txn->parent->info = QBI_MEM_MALLOC_CLEAR(
        sizeof(qbi_svc_msuicc_channel_req_info));
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
      info = (qbi_svc_msuicc_channel_req_info *)qmi_txn->parent->info;
      info->channel_id = req->channel;
      info->channel_group = req->channel_group;
      action = qbi_svc_msuicc_close_channel_s_apdu_req(qmi_txn);
    }
  }

  return action;
} /* qbi_svc_msuicc_close_channel_s_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_close_channel_s_apdu_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_CLOSE_CHANNEL set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_close_channel_s_apdu_req
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_send_apdu_req_msg_v01 *qmi_req;
  uint8 command[4];
  qbi_svc_msuicc_channel_req_info *info = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_msuicc_cache_s *cache;
  qbi_svc_msuicc_logical_channel_s *logical_channel;
  qbi_util_list_iter_s iter;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  QBI_LOG_D_0("In qbi_svc_msuicc_close_channel_s_apdu_req");

  info = (qbi_svc_msuicc_channel_req_info *)qmi_txn->parent->info;

  if (info->channel_id != 0)
  {
    qmi_req = (uim_send_apdu_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_SEND_APDU_REQ_V01,
        qbi_svc_msuicc_close_channel_s_apdu_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    /* Per ETSI 102.221 Class byte 0x00, Select instruction byte 0x70, 
       Selection by AID 0x80 */
    command[0] = 0x00;
    command[1] = 0x70;
    command[2] = 0x80;
    command[3] = info->channel_id; /* Channel ID */

    qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
    qmi_req->procedure_bytes_valid = TRUE;
    qmi_req->procedure_bytes = UIM_APDU_SKIP_PROCEDURE_BYTES_V01;
    /* Manage Channel APDU length will always be 5 bytes hence coded to 5 */
    qmi_req->apdu_len = 4;
    QBI_MEMSCPY(qmi_req->apdu, QMI_UIM_APDU_DATA_MAX_V01, command, 4);
    action =  QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  else 
  {
    QBI_LOG_D_0("Closing group");
    cache = qbi_svc_msuicc_cache_get(qmi_txn->ctx);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);

    qbi_util_list_iter_init(&cache->logical_channel_list, &iter);

    while ((logical_channel = (qbi_svc_msuicc_logical_channel_s *)
      qbi_util_list_iter_next(&iter)) != NULL)
    {
      if (logical_channel->channel_group == info->channel_group)
      {
        qmi_req = (uim_send_apdu_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
            qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_SEND_APDU_REQ_V01,
            qbi_svc_msuicc_close_channel_s_apdu_rsp_cb);
        QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
       /* Per ETSI 102.221 Class byte 0x00, Select instruction byte 0x70, 
          Selection by AID 0x80 */
        command[0] = 0x00;
        command[1] = 0x70;
        command[2] = 0x80;
        command[3] = logical_channel->channel_id;/* Channel ID */
        qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
        qmi_req->procedure_bytes_valid = TRUE;
        qmi_req->procedure_bytes = UIM_APDU_SKIP_PROCEDURE_BYTES_V01;
        /* Manage Channel APDU length will always be 5 bytes hence coded to 5 */
        qmi_req->apdu_len = 4;
        QBI_MEMSCPY(qmi_req->apdu, QMI_UIM_APDU_DATA_MAX_V01, command, 4);
      }
    }
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  return action;
}/* qbi_svc_msuicc_close_channel_s_apdu_req */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_close_channel_s_uim3f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_LOGICAL_CHANNEL_RESP for
    MBIM_CID_MS_UICC_CLOSE_CHANNEL set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_close_channel_s_uim3f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_msuicc_close_channel_s_rsp_s *rsp;
  const uim_logical_channel_req_msg_v01 *qmi_req;
  const uim_logical_channel_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (uim_logical_channel_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (uim_logical_channel_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    /*! Remove closed logical channel from cached active channel list
        after receiving normal processing status. */
    if (!qbi_svc_msuicc_logical_channel_remove_from_cache(
          qmi_txn->ctx, qmi_req->channel_id))
    {
      QBI_LOG_E_1("Cannot remove channel %d from cache",
                  qmi_req->channel_id);
    }
    else
    {
      /* Wait for all channels to be closed before sending response */
      if (qmi_txn->parent->qmi_txns_pending == 0)
      {
        rsp = qbi_txn_alloc_rsp_buf(
          qmi_txn->parent, sizeof(qbi_svc_msuicc_close_channel_s_rsp_s));
        QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

        if (qmi_rsp->card_result_valid)
        {
          rsp->status[0] = qmi_rsp->card_result.sw1;
          rsp->status[1] = qmi_rsp->card_result.sw2;
        }
        else
        {
          rsp->status[0] = QBI_SVC_MSUICC_STATUS_WORD_SW1_SUCCESS;
          rsp->status[1] = QBI_SVC_MSUICC_STATUS_WORD_SW2_SUCCESS;
        }

        action = QBI_SVC_ACTION_SEND_RSP;
      }
      else
      {
        action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
      }
    }
  }

  return action;
} /* qbi_svc_msuicc_close_channel_s_uim3f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_close_channel_s_apdu_rsp_cb
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_CLOSE_CHANNEL set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_close_channel_s_apdu_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_msuicc_close_channel_s_rsp_s *rsp = NULL;
  uim_send_apdu_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uim_send_apdu_req_msg_v01 *qmi_req = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);

  QBI_LOG_D_0("In qbi_svc_msuicc_close_channel_s_apdu_rsp_cb");

  qmi_rsp = (uim_send_apdu_resp_msg_v01 *) qmi_txn->rsp.data;
  qmi_req = (uim_send_apdu_req_msg_v01 *)qmi_txn->req.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 || qmi_rsp->apdu_valid == FALSE)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_D_1("qmi_txn->parent->qmi_txns_pending = %d", 
                qmi_txn->parent->qmi_txns_pending);
    /*! Remove closed logical channel from cached active channel list
        after receiving normal processing status. */
    if (!qbi_svc_msuicc_logical_channel_remove_from_cache(
            qmi_txn->ctx, qmi_req->apdu[3]))
    {
      QBI_LOG_E_1("Cannot remove channel %d from cache",
            qmi_req->apdu[3]);
    }
    else
    {
      /* Wait for all channels to be closed before sending response */
      if (qmi_txn->parent->qmi_txns_pending == 0)
      {
        rsp = qbi_txn_alloc_rsp_buf(
              qmi_txn->parent, sizeof(qbi_svc_msuicc_close_channel_s_rsp_s));
        QBI_CHECK_NULL_PTR_RET_ABORT(rsp);
  
        rsp->status[0] = qmi_rsp->apdu[qmi_rsp->apdu_len - 2];
        rsp->status[1] = qmi_rsp->apdu[qmi_rsp->apdu_len - 1];
        QBI_LOG_D_0("All channels closed sending response");
        action = QBI_SVC_ACTION_SEND_RSP;
      }
      else
      {
        QBI_LOG_D_0("Waiting for all channels to close");
        action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
      }
    }
  }
  return action;
}/* qbi_svc_msuicc_close_channel_s_apdu_rsp_cb */
/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_APDU
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_apdu_s_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Build MBIM_CID_MS_UICC_APDU_INFO based on APDU response in cache

    @details

    @param qbi_txn_s

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_apdu_s_build_rsp_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_msuicc_apdu_s_rsp_s *rsp;
  qbi_svc_msuicc_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_msuicc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_msuicc_apdu_s_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_msuicc_apdu_s_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  if (cache->apdu_response.apdu != NULL &&
      cache->apdu_response.bytes_copied == cache->apdu_response.total_length &&
      cache->apdu_response.total_length >= 2)
  {
    rsp->status[0] =
      cache->apdu_response.apdu[cache->apdu_response.total_length - 2];
    rsp->status[1] =
      cache->apdu_response.apdu[cache->apdu_response.total_length - 1];
    
    /* Need to add data field only if total length is more than 2
    i.e. apart from sw1 and sw2 , data field also needs to be sent
    to upper layer*/
    if (cache->apdu_response.total_length > 2 &&
          (!qbi_txn_rsp_databuf_add_field(
          txn, &rsp->response, 0, cache->apdu_response.total_length - 2,
          cache->apdu_response.apdu)))
    {
      QBI_LOG_E_1("Couldn't add apdu response data to response! %d",cache->apdu_response.total_length);
    }
    else
    {
      qbi_svc_msuicc_reverse_ol_pair(&rsp->response);
    }
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  qbi_svc_msuicc_apdu_s_clear_apdu_in_cache(cache);
  return action;
} /* qbi_svc_msuicc_apdu_s_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_apdu_s_clear_apdu_in_cache
===========================================================================*/
/*!
    @brief Clean existing APDU data in cache

    @details

    @param qbi_svc_msuicc_cache_s
*/
/*=========================================================================*/
static void qbi_svc_msuicc_apdu_s_clear_apdu_in_cache
(
  qbi_svc_msuicc_cache_s *cache
)
{
  QBI_CHECK_NULL_PTR_RET(cache);

  cache->apdu_response.bytes_copied = 0;
  cache->apdu_response.token        = 0;
  cache->apdu_response.total_length = 0;
  if (cache->apdu_response.apdu != NULL)
  {
    QBI_MEM_FREE(cache->apdu_response.apdu);
    cache->apdu_response.apdu = NULL;
  }
} /* qbi_svc_msuicc_apdu_s_clear_apdu_in_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_apdu_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_APDU set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_apdu_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_msuicc_apdu_s_req_s *req;
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  req = (qbi_svc_msuicc_apdu_s_req_s *) txn->req.data;
  qbi_svc_msuicc_reverse_ol_pair(&req->command);

  if (!qbi_svc_msuicc_apdu_s_req_is_valid(txn))
  {
    QBI_LOG_E_0("APDU set request is invalid");
  }
  else
  {
    /* QMI_UIM_GET_CARD_STATUS (0x2f) */
    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
      qbi_svc_msuicc_apdu_s_uim2f_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_msuicc_apdu_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_apdu_s_req_is_valid
===========================================================================*/
/*!
    @brief Validate settings in MBIM_CID_MS_UICC_APDU set request

    @details Validate secure messaging, class byte and channel id

    @param txn

    @return boolean TRUE if valid, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_msuicc_apdu_s_req_is_valid
(
  qbi_txn_s *txn
)
{
  qbi_svc_msuicc_apdu_s_req_s *req;
  const uint8 *command = NULL;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  req = (qbi_svc_msuicc_apdu_s_req_s *) txn->req.data;
  command = qbi_txn_req_databuf_get_field(
    txn, &req->command, 0, QBI_SVC_MSUICC_APDU_COMMAND_MAX_LEN_BYTES);
  QBI_CHECK_NULL_PTR_RET_FALSE(command);

  if (req->command.size == 0)
  {
    QBI_LOG_E_0("Invalid APDU size");
  }
  else if (!qbi_svc_msuicc_logical_channel_id_is_valid(txn->ctx, req->channel))
  {
    QBI_LOG_E_1("Invalid channel id %d", req->channel);
    txn->status = QBI_MBIM_STATUS_MS_INVALID_LOGICAL_CHANNEL;
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_svc_msuicc_apdu_s_req_is_valid() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_apdu_s_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_MS_UICC_APDU set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_apdu_s_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_msuicc_apdu_s_req_s *req;
  const uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uim_send_apdu_req_msg_v01 *qmi_req;
  const uint8 *command;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint32 card_status_index = 0;
  qbi_svc_msuicc_cache_s *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  cache = qbi_svc_msuicc_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;
  card_status_index = qbi_svc_bc_sim_get_card_status_index();

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Missing card status TLV!");
  }
  else if (TRUE == cache->slot_mapping_in_progress)
  {
    QBI_LOG_E_0("Slot mapping is in progress, can't perform apdu set request");
    qmi_txn->parent->status = QBI_MBIM_STATUS_BUSY;
  }
  else
  {
    req = (qbi_svc_msuicc_apdu_s_req_s *) qmi_txn->parent->req.data;

    qmi_req = (uim_send_apdu_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_SEND_APDU_REQ_V01,
        qbi_svc_msuicc_apdu_s_uim3b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
    /* Only for telecom mode channel id is used*/
    if (qmi_rsp->card_mode[card_status_index] == UIM_CARD_MODE_TELECOM_CARD_V01)
    {
      qmi_req->channel_id_valid = TRUE;
      qmi_req->channel_id = req->channel;
    }

    qmi_req->procedure_bytes_valid = TRUE;
    qmi_req->procedure_bytes = UIM_APDU_SKIP_PROCEDURE_BYTES_V01;

    command = qbi_txn_req_databuf_get_field(
      qmi_txn->parent, &req->command, 0,
      QBI_SVC_MSUICC_APDU_COMMAND_MAX_LEN_BYTES);
    QBI_CHECK_NULL_PTR_RET_ABORT(command);

    qmi_req->apdu_len = req->command.size;
    QBI_MEMSCPY(qmi_req->apdu, QMI_UIM_APDU_DATA_MAX_V01, command,
                req->command.size);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_msuicc_apdu_s_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_apdu_s_uim3b_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_SEND_APDU_IND, copying data chunk of APDU
    response to cache in order to reconstruct complete APDU response for
    MBIM_CID_MS_UICC_APDU set request

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_apdu_s_uim3b_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  qbi_svc_msuicc_cache_s *cache;
  const uim_send_apdu_ind_msg_v01 *qmi_ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  cache = qbi_svc_msuicc_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_ind = (const uim_send_apdu_ind_msg_v01 *) ind->buf->data;
  if (cache->apdu_response.apdu != NULL &&
      qmi_ind->long_response_chunk.token == cache->apdu_response.token &&
      qmi_ind->long_response_chunk.total_length ==
        cache->apdu_response.total_length &&
      (cache->apdu_response.bytes_copied +
        qmi_ind->long_response_chunk.apdu_len) <=
          cache->apdu_response.total_length &&
      (qmi_ind->long_response_chunk.offset +
        qmi_ind->long_response_chunk.apdu_len) <=
          cache->apdu_response.total_length)
  {
    QBI_MEMSCPY(cache->apdu_response.apdu +
                  qmi_ind->long_response_chunk.offset,
                cache->apdu_response.total_length -
                  qmi_ind->long_response_chunk.offset,
                qmi_ind->long_response_chunk.apdu,
                qmi_ind->long_response_chunk.apdu_len);
    cache->apdu_response.bytes_copied += qmi_ind->long_response_chunk.apdu_len;

    if (cache->apdu_response.bytes_copied == cache->apdu_response.total_length)
    {
      action = qbi_svc_msuicc_apdu_s_build_rsp_from_cache(ind->txn);
    }
  }
  else
  {
    qbi_svc_msuicc_apdu_s_clear_apdu_in_cache(cache);
    QBI_LOG_E_0("Cannot reconstruct APDU response!");
  }

  return action;
} /* qbi_svc_msuicc_apdu_s_uim3b_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_apdu_s_uim3b_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_SEND_APDU_RESP for MBIM_CID_MS_UICC_APDU set
    request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_apdu_s_uim3b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  const uim_send_apdu_resp_msg_v01 *qmi_rsp;
  qbi_svc_msuicc_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_msuicc_cache_get(qmi_txn->parent->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = (uim_send_apdu_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error  != QMI_ERR_INSUFFICIENT_RESOURCES_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (qmi_rsp->resp.error == QMI_ERR_INSUFFICIENT_RESOURCES_V01 &&
           qmi_rsp->long_response_valid)
  {
    qbi_svc_msuicc_apdu_s_clear_apdu_in_cache(cache);
    cache->apdu_response.token = qmi_rsp->long_response.token;
    cache->apdu_response.bytes_copied = 0;
    cache->apdu_response.total_length = qmi_rsp->long_response.total_length;
    cache->apdu_response.apdu = QBI_MEM_MALLOC_CLEAR(
      qmi_rsp->long_response.total_length);
    QBI_CHECK_NULL_PTR_RET_FALSE(cache->apdu_response.apdu);

    if (qmi_rsp->apdu_valid &&
        qmi_rsp->apdu_len <= cache->apdu_response.total_length)
    {
      QBI_MEMSCPY(cache->apdu_response.apdu,
                  cache->apdu_response.total_length,
                  qmi_rsp->apdu, qmi_rsp->apdu_len);
      cache->apdu_response.bytes_copied += qmi_rsp->apdu_len;
    }

    if (!qbi_svc_ind_reg_dynamic(
          qmi_txn->ctx, QBI_SVC_ID_MSUICC, QBI_SVC_MSUICC_MBIM_CID_APDU,
          QBI_QMI_SVC_UIM, QMI_UIM_SEND_APDU_IND_V01,
          qbi_svc_msuicc_apdu_s_uim3b_ind_cb, qmi_txn->parent, NULL))
    {
      QBI_LOG_E_0("Failed to register dynamic indication handler");
      action = QBI_SVC_ACTION_ABORT;
    }
    else
    {
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
  }
  else if (qmi_rsp->apdu_valid)
  {
    qbi_svc_msuicc_apdu_s_clear_apdu_in_cache(cache);

    cache->apdu_response.bytes_copied = qmi_rsp->apdu_len;
    cache->apdu_response.total_length = qmi_rsp->apdu_len;

    cache->apdu_response.apdu = QBI_MEM_MALLOC_CLEAR(
      cache->apdu_response.total_length);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache->apdu_response.apdu);

    QBI_MEMSCPY(cache->apdu_response.apdu, cache->apdu_response.total_length,
                qmi_rsp->apdu, qmi_rsp->apdu_len);

    action = qbi_svc_msuicc_apdu_s_build_rsp_from_cache(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_msuicc_apdu_s_uim3b_rsp_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_TERMINAL_CAPABILITY
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_terminal_capability_q_build_rsp
===========================================================================*/
/*!
    @brief Allocates and populates the response for a query to
    MBIM_CID_MS_UICC_TERMINAL_CAPABILITY

    @details

    @param txn
    @param uim_terminal_capability_resp_msg_v01

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_q_build_rsp
(
  qbi_txn_s                                  *txn,
  const uim_terminal_capability_resp_msg_v01 *qmi_rsp
)
{
  qbi_svc_msuicc_terminal_capability_q_rsp_s *rsp;
  qbi_mbim_offset_size_pair_s *ol_pair;
  uint8 *tlv;
  uint32 i;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_rsp);

  if (!(qmi_rsp->resp.result == QMI_RESULT_SUCCESS_V01 ||
        qmi_rsp->terminal_capability_tlv_valid == TRUE))
  {
    QBI_LOG_E_0("Invalid uim_terminal_capability_resp message!");
  }
  else
  {
    rsp = (qbi_svc_msuicc_terminal_capability_q_rsp_s *) qbi_txn_alloc_rsp_buf(
      txn, (sizeof(qbi_svc_msuicc_terminal_capability_q_rsp_s) +
            (qmi_rsp->terminal_capability_tlv_len *
             sizeof(qbi_mbim_offset_size_pair_s))));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    for (i = 0; i < qmi_rsp->terminal_capability_tlv_len; i++)
    {
      ol_pair = (qbi_mbim_offset_size_pair_s *)
        ((uint8 *) rsp + sizeof(qbi_svc_msuicc_terminal_capability_q_rsp_s) +
         (i * sizeof(qbi_mbim_offset_size_pair_s)));

      tlv = qbi_txn_rsp_databuf_add_field(
        txn, ol_pair, 0, qmi_rsp->terminal_capability_tlv[i].value_len + 2, NULL);
      QBI_CHECK_NULL_PTR_RET_ABORT(tlv);

      tlv[0] = qmi_rsp->terminal_capability_tlv[i].tag;
      tlv[1] = qmi_rsp->terminal_capability_tlv[i].value_len;
      QBI_MEMSCPY(tlv + 2, qmi_rsp->terminal_capability_tlv[i].value_len,
                  qmi_rsp->terminal_capability_tlv[i].value,
                  qmi_rsp->terminal_capability_tlv[i].value_len);
    }

    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_msuicc_terminal_capability_q_build_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_terminal_capability_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_TERMINAL_CAPABILITY query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/

  /* QMI_UIM_GET_CARD_STATUS (0x2f) */
  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
    qbi_svc_msuicc_terminal_capability_q_uim2f_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  action = QBI_SVC_ACTION_SEND_QMI_REQ;

  return action;
} /* qbi_svc_msuicc_terminal_capability_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_terminal_capability_q_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_MS_UICC_TERMINAL_CAPABILITY query request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  const uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uim_terminal_capability_req_msg_v01 *qmi_req;
  qbi_svc_msuicc_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Missing card status TLV!");
  }
  else
  {
    cache = qbi_svc_msuicc_cache_get(qmi_txn->ctx);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);

    if (cache->qmi_terminal_capability_resp.terminal_capability_tlv_valid)
    {
      action = qbi_svc_msuicc_terminal_capability_q_build_rsp(
        qmi_txn->parent, &cache->qmi_terminal_capability_resp);
    }
    else
    {
      qmi_req = (uim_terminal_capability_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_TERMINAL_CAPABILITY_REQ_V01,
          qbi_svc_msuicc_terminal_capability_q_uim6a_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_msuicc_terminal_capability_q_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_terminal_capability_q_uim6a_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_TERMINAL_CAPABILITY_RESP for
    MBIM_CID_MS_UICC_TERMINAL_CAPABILITY query request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_q_uim6a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  const uim_terminal_capability_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_terminal_capability_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    action = qbi_svc_msuicc_terminal_capability_q_build_rsp(
      qmi_txn->parent, qmi_rsp);
  }

  return action;
} /* qbi_svc_msuicc_terminal_capability_q_uim6a_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_terminal_capability_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_TERMINAL_CAPABILITY set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_msuicc_terminal_capability_s_req_s *req;
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_msuicc_terminal_capability_s_req_s *) txn->req.data;
  if (req->element_count > QMI_UIM_TERMINAL_CAPABILITY_VALUE_MAX_V01)
  {
    QBI_LOG_E_1("TLV count %d is over limit", req->element_count);
  }
  else
  {
    /* QMI_UIM_GET_CARD_STATUS (0x2f) */
    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
      qbi_svc_msuicc_terminal_capability_s_uim2f_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_msuicc_terminal_capability_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_terminal_capability_s_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_MS_UICC_TERMINAL_CAPABILITY set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_s_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_msuicc_terminal_capability_s_req_s *req;
  const uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uim_terminal_capability_req_msg_v01 *qmi_req;
  qbi_mbim_offset_size_pair_s *ol_pair;
  uint32 i;
  uint8 *tlv;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *) qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->card_status_valid)
  {
    QBI_LOG_E_0("Missing card status TLV!");
  }
  else
  {
    req = (qbi_svc_msuicc_terminal_capability_s_req_s *)
      qmi_txn->parent->req.data;

    qmi_req = (uim_terminal_capability_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_TERMINAL_CAPABILITY_REQ_V01,
        qbi_svc_msuicc_terminal_capability_s_uim6a_rsp_cb);
        QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
    qmi_req->terminal_capability_tlv_valid = TRUE;

    for (i = 0; i < req->element_count; i++)
    {
      ol_pair = (qbi_mbim_offset_size_pair_s *)
        ((uint8 *) req + sizeof(qbi_svc_msuicc_terminal_capability_s_req_s) +
         (i * sizeof(qbi_mbim_offset_size_pair_s)));

      tlv = qbi_txn_req_databuf_get_field(
        qmi_txn->parent, ol_pair, 0,
        QMI_UIM_TERMINAL_CAPABILITY_VALUE_MAX_V01);
      QBI_CHECK_NULL_PTR_RET_ABORT(tlv);

      if (ol_pair->size < 2)
      {
        QBI_LOG_E_1("Invalid terminal capability TLV, index %d!", i);
        return QBI_SVC_ACTION_ABORT;
      }
      else
      {
        qmi_req->terminal_capability_tlv_len++;
        qmi_req->terminal_capability_tlv[i].remove_tlv = FALSE;
        qmi_req->terminal_capability_tlv[i].tag = tlv[0];
        qmi_req->terminal_capability_tlv[i].value_len = tlv[1];
        if (qmi_req->terminal_capability_tlv[i].value_len > 0)
        {
          QBI_MEMSCPY(qmi_req->terminal_capability_tlv[i].value,
                      QMI_UIM_TERMINAL_CAPABILITY_VALUE_MAX_V01,
                      tlv + 2, ol_pair->size - 2);
        }
      }
    }

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_msuicc_terminal_capability_s_uim2f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_terminal_capability_s_uim6a_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_TERMINAL_CAPABILITY_RESP for
    MBIM_CID_MS_UICC_TERMINAL_CAPABILITY set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_terminal_capability_s_uim6a_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  const uim_terminal_capability_resp_msg_v01 *qmi_rsp;
  qbi_svc_msuicc_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_terminal_capability_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    cache = qbi_svc_msuicc_cache_get(qmi_txn->ctx);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);

    QBI_MEMSCPY(&cache->qmi_terminal_capability_resp,
                sizeof(uim_terminal_capability_resp_msg_v01),
                qmi_rsp, sizeof(uim_terminal_capability_resp_msg_v01));

    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_msuicc_terminal_capability_s_uim6a_rsp_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_RESET
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_reset_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_RESET query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_reset_q_req
(
  qbi_txn_s *txn
)
{
  uim_get_card_status_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  qmi_req = (uim_get_card_status_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
      qbi_svc_msuicc_reset_q_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_msuicc_reset_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_reset_q_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_MS_UICC_RESET Query request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_reset_q_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  qbi_svc_msuicc_reset_q_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint32 card_status_index = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  QBI_LOG_D_0("In qbi_svc_msuicc_reset_q_rsp_cb");

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *)qmi_txn->rsp.data;
  card_status_index = qbi_svc_bc_sim_get_card_status_index();

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (qmi_rsp->card_mode_valid == FALSE) 
  {
    QBI_LOG_E_1("Invalid card mode %d", qmi_rsp->card_mode_valid);
  }
  else
  {
    /* Forming response*/
    rsp = (qbi_svc_msuicc_reset_q_rsp_s *)qbi_txn_alloc_rsp_buf(
          qmi_txn->parent, (sizeof(qbi_svc_msuicc_reset_q_rsp_s)));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    if (qmi_rsp->card_mode[card_status_index] == UIM_CARD_MODE_NON_TELECOM_CARD_V01)
    {
      rsp->pass_through_status = PASS_THROUGH_STATUS_ENABLED;
      QBI_LOG_D_2("card mode = %d pass through status = %d",
      qmi_rsp->card_mode[card_status_index],
      rsp->pass_through_status);
    }
    else if (qmi_rsp->card_mode[card_status_index] == UIM_CARD_MODE_TELECOM_CARD_V01)
    {
      rsp->pass_through_status = PASS_THROUGH_STATUS_DISABLED;
      QBI_LOG_D_2("card mode = %d pass through status = %d",
      qmi_rsp->card_mode[card_status_index],
      rsp->pass_through_status);
    }
    qbi_txn_rsp_databuf_consolidate(qmi_txn->parent);

    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_msuicc_reset_q_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_reset_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_MS_UICC_RESET set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_reset_s_req
(
  qbi_txn_s *txn
)
{
  uim_get_card_status_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  if (!qbi_svc_ind_reg_dynamic(
    txn->ctx, QBI_SVC_ID_MSUICC,
    QBI_SVC_MSUICC_MBIM_CID_RESET, QBI_QMI_SVC_UIM,
    QMI_UIM_STATUS_CHANGE_IND_V01,
    qbi_svc_muicc_reset_s_req_power_down_ind_rsp_cb,
    txn, NULL))
  {
    QBI_LOG_E_0("Couldn't register dynamic indication handler!");
  }
  else
  {
    qmi_req = (uim_get_card_status_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
        qbi_svc_msuicc_reset_s_req_get_card_status_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }
  return action;
}/* qbi_svc_msuicc_reset_s_req */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_reset_s_req_get_card_status_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_MS_UICC_RESET set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_reset_s_req_get_card_status_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_power_down_req_msg_v01 *qmi_req;
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  qmi_rsp = (uim_get_card_status_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    qmi_req = (uim_power_down_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
       qmi_txn->parent, QBI_QMI_SVC_UIM, QMI_UIM_POWER_DOWN_REQ_V01,
       qbi_svc_msuicc_reset_s_req_power_down_rsp_cb);

    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);
    qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();
    QBI_LOG_D_1("Slot requested to power down = %d",
      qmi_req->slot);

    action = QBI_SVC_ACTION_SEND_QMI_REQ ;
  }

  return action;
} /* qbi_svc_msuicc_reset_s_req_get_card_status_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_reset_s_req_power_down_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_MS_UICC_RESET set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_reset_s_req_power_down_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_power_down_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  qmi_rsp = (uim_power_down_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Power down failure %d ", qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_D_0("Power down successful, awaiting card event");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  return action;
}/* qbi_svc_msuicc_reset_s_req_power_down_rsp_cb */

/*===========================================================================
  FUNCTION: qbi_svc_muicc_reset_s_req_power_down_ind_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_MS_UICC_RESET set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_muicc_reset_s_req_power_down_ind_rsp_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_msuicc_reset_s_req_s *req;
  uim_power_up_req_msg_v01 *qmi_req;
  const uim_status_change_ind_msg_v01 *qmi_ind = NULL;
  uint32 card_status_index = 0;
/*-------------------------------------------------------------------------*/
  qmi_ind = (const uim_status_change_ind_msg_v01 *)ind->buf->data;
  card_status_index = qbi_svc_bc_sim_get_card_status_index();

  if (!qmi_ind->card_status_valid)
  {
    QBI_LOG_E_0("Received card info indication without card status");
    ind->txn->status = QBI_MBIM_STATUS_FAILURE;
    qbi_svc_ind_dereg_txn(ind->txn);
  }
  else if (qmi_ind->card_status.card_info[card_status_index].card_state == UIM_CARD_STATE_ERROR_V01)
  {
    req = (qbi_svc_msuicc_reset_s_req_s *)ind->txn->req.data;
    qmi_req = (uim_power_up_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        ind->txn, QBI_QMI_SVC_UIM, QMI_UIM_POWER_UP_REQ_V01,
        qbi_svc_msuicc_reset_s_req_power_up_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->slot = qbi_svc_bc_sim_get_logical_slot();

    if (req->pass_through_action == PASS_THROUGH_ACTION_ENABLE)
    {
      /* Card Mode is non telecom */
      qmi_req->card_mode_valid = TRUE;
      qmi_req->card_mode = UIM_CARD_MODE_NON_TELECOM_CARD_V01;
      QBI_LOG_D_3("Non -telecom qmi_req->slot = %d card mode = %d req->pass_through_action = %d ",
       qmi_req->slot,
       qmi_req->card_mode,
       req->pass_through_action);
    }
    else if(req->pass_through_action == PASS_THROUGH_ACTION_DISABLE)
    {
      /* Card Mode is telecom */
      qmi_req->card_mode_valid = TRUE;
      qmi_req->card_mode = UIM_CARD_MODE_TELECOM_CARD_V01;
      QBI_LOG_D_3("Telecom qmi_req->slot = %d card mode = %d req->pass_through_action = %d ",
       qmi_req->slot,
       qmi_req->card_mode,
       req->pass_through_action);
    }
    action = QBI_SVC_ACTION_SEND_QMI_REQ ;
  }
  else if (qmi_ind->card_status.card_info[card_status_index].card_state == UIM_CARD_STATE_PRESENT_V01)
  {
    QBI_LOG_D_0(" Card Up event received ");
    action = qbi_svc_msuicc_reset_q_req(ind->txn);
  }
  else 
  {
    QBI_LOG_D_0(" Card absent is invalid state for command.");
  }
  return action;
} /* qbi_svc_muicc_reset_s_req_power_down_ind_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_reset_s_req_power_up_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for
    MBIM_CID_MS_UICC_RESET set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_msuicc_reset_s_req_power_up_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_power_down_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  qmi_rsp = (uim_power_down_resp_msg_v01 *)qmi_txn->rsp.data;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Power up failure %d ", qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_D_0("Power up successful, awaiting card event");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  return action;
}/* qbi_svc_msuicc_reset_s_req_power_up_rsp_cb */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_msuicc_init
===========================================================================*/
/*!
    @brief One-time initialization of the MS Low_Level UICC Access device
    service

    @details
*/
/*=========================================================================*/
void qbi_svc_msuicc_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_msuicc_cfg = {
    {
      0xc2, 0xf6, 0x58, 0x8e, 0xf0, 0x37, 0x4b, 0xc9,
      0x86, 0x65, 0xf4, 0xd4, 0x4b, 0xd0, 0x93, 0x67
    },
    QBI_SVC_ID_MSUICC,
    FALSE,
    qbi_svc_msuicc_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_msuicc_cmd_hdlr_tbl),
    qbi_svc_msuicc_open,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_msuicc_cfg);
} /* qbi_svc_msuicc_init() */

