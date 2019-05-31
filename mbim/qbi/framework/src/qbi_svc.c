/*!
  @file
  qbi_svc.c

  @brief
  Common device service processing functionality
*/

/*=============================================================================

  Copyright (c) 2011-2014, 2016-2017 Qualcomm Technologies, Inc.
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
06/02/17  vk   Add initialization API's for SAR and extension service
04/01/13  bd   Move platform-specific device service init calls to task_init
03/12/13  bd   Add QMI indication (de)registration framework
01/22/13  bd   Support new unit test framework
07/25/12  hz   Add Qualcomm Mobile Broadband MBIM Extensibility device service
07/02/12  bd   Add new function to generically force a CID event
05/01/12  bd   Fix improper indexing based off svc_id
04/24/12  bd   Fix double free when MBIM_OPEN fails
10/28/11  bd   Updated to MBIM v1.0 SC
09/02/11  bd   Updated to MBIM v0.81c
08/24/11  bd   Allow deregistration of indication handlers during the
               indication processing loop
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_msg.h"
#include "qbi_msg_mbim.h"
#include "qbi_qmi.h"
#include "qbi_svc_atds.h"
#include "qbi_svc_auth.h"
#include "qbi_svc_bc.h"
#include "qbi_svc_msfwid.h"
#include "qbi_svc_mshsd.h"
#include "qbi_svc_msuicc.h"
#include "qbi_svc_pb.h"
#include "qbi_svc_sms.h"
#include "qbi_svc_stk.h"
#include "qbi_svc_ussd.h"
#include "qbi_svc_bc_ext.h"
#include "qbi_svc_mssar.h"
#include "qbi_txn.h"
#include "qbi_util.h"

#ifdef QBI_UNIT_TEST
#include "qbi_utf.h"
#endif /* QBI_UNIT_TEST */

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! Maps a qbi_svc_id_e to a value that can be used as an array index
    @note The service ID should already be sanitized to be within the valid
    range by checking against qbi_svc_id_is_valid() */
#define QBI_SVC_ID_TO_INDEX(svc_id) (svc_id - QBI_SVC_ID_OFFSET)

/*! Short timeout used for QMI indication (de)registration */
#define QBI_SVC_QMI_REG_TIMEOUT_MS (2500)

/*! Maximum QMI indication reference count value used to prevent wraparound */
#define QBI_SVC_QMI_REF_CNT_MAX \
  (((1 << (sizeof(((qbi_svc_ind_qmi_ref_cnt_entry_s *) 0)->ref_cnt) * 8)) - 1))

/*! Indicates that a QMI service ID range value is not used */
#define QBI_SVC_QMI_SVC_ID_RANGE_INVALID (QBI_QMI_NUM_SVCS)

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! Struct containing information about a registered dynamic QMI
    indication handler */
typedef struct {
  /*! Must be first as we alias */
  qbi_util_list_entry_s list_entry;

  /*! Internal MBIM device service ID */
  qbi_svc_id_e svc_id;

  /*! Transaction to pass to the callback, or NULL if a new event transaction
      should be allocated. This is only relevant for dynamic indication
      handlers */
  struct qbi_txn_struct *txn;

  /*! Common indication handler information */
  qbi_svc_ind_info_s ind_info;

  /*! @brief Encoded function to invoke when the registered QMI indication is
      received
      @see qbi_svc_qmi_ind_cb_f */
  qbi_os_encoded_fptr_t encoded_cb;

  /*! @brief Start of the QMI service ID range (inclusive)
      @details If not set to QBI_SVC_QMI_SVC_ID_RANGE_INVALID, then this
      dynamic indication handler should match a range of QMI service IDs, rather
      than the single QMI service ID given in ind_info. */
  qbi_qmi_svc_e qmi_svc_id_range_start;

  /*! End of the QMI service ID range (inclusive) */
  qbi_qmi_svc_e qmi_svc_id_range_end;
} qbi_svc_ind_info_dynamic_s;

/*! Event filter transaction structure. */
typedef struct qbi_svc_event_filter_txn_struct {
  /*! Mirrors qbi_svc_state_s:event_enabled - for each service, a boolean array
      of length equal to the number of CIDs supported by the service. */
  boolean *event_enabled[QBI_SVC_NUM_SVCS];
} qbi_svc_event_filter_txn_s;

/*=============================================================================

  Private Variables

=============================================================================*/

/*! Array of pointers to device service configurations */
static const qbi_svc_cfg_s *qbi_svc_cfg[QBI_SVC_NUM_SVCS];

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static boolean qbi_svc_cid_is_valid
(
  const qbi_svc_cfg_s *svc_cfg,
  uint32               cid
);

static void qbi_svc_close_dev_svc
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id
);

static void qbi_svc_event_filter_free_all
(
  qbi_svc_state_s *svc_state
);

static boolean qbi_svc_event_filter_check
(
  qbi_txn_s *txn
);

static boolean qbi_svc_event_filter_init
(
  qbi_svc_state_s *svc_state
);

static boolean qbi_svc_event_filter_reset
(
  qbi_ctx_s *ctx,
  qbi_txn_s *listener_txn
);

static void qbi_svc_qmi_ind_ref_cnt_free_all
(
  qbi_svc_state_s *svc_state
);

static boolean qbi_svc_ind_qmi_ref_cnt_update
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id,
  boolean       increment,
  uint16       *new_ref_cnt
);

static boolean qbi_svc_id_is_valid
(
  qbi_svc_id_e svc_id
);

static boolean qbi_svc_ind_info_dynamic_is_equal
(
  const qbi_svc_ind_info_dynamic_s *dyn_ind_info,
  qbi_svc_id_e                      svc_id,
  uint32                            cid,
  qbi_qmi_svc_e                     qmi_svc_id_range_start,
  qbi_qmi_svc_e                     qmi_svc_id_range_end,
  uint16                            qmi_msg_id,
  qbi_svc_qmi_ind_cb_f             *cb,
  const qbi_txn_s                  *txn,
  const void                       *cb_data
);

static const qbi_svc_cfg_s *qbi_svc_look_up_cfg_by_svc_id
(
  qbi_svc_id_e svc_id
);

static const qbi_svc_cfg_s *qbi_svc_look_up_cfg_by_uuid
(
  const uint8 *uuid
);

static boolean qbi_svc_open_next_dev_svc
(
  qbi_txn_s *txn,
  boolean   *no_more_svcs
);

static void qbi_svc_open_notify_cb
(
  qbi_txn_s       *listener_txn,
  const qbi_txn_s *notifier_txn
);

static boolean qbi_svc_proc_open_done
(
  qbi_txn_s *txn
);

static boolean qbi_svc_sanity_check_ind_info
(
  const qbi_svc_ind_info_s *ind_info,
  qbi_svc_id_e              svc_id,
  boolean                   null_cb_allowed
);

static void qbi_svc_qmi_ind_dispatch_dynamic
(
  qbi_ctx_s              *ctx,
  qbi_svc_qmi_ind_data_s *ind_data
);

static void qbi_svc_qmi_ind_dispatch_static
(
  qbi_ctx_s              *ctx,
  qbi_svc_qmi_ind_data_s *ind_data
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_svc_cid_is_valid
===========================================================================*/
/*!
    @brief Determines whether a CID is in the valid range for the given
    device service ID

    @details

    @param svc_cfg
    @param cid

    @return boolean TRUE if CID is valid, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_cid_is_valid
(
  const qbi_svc_cfg_s *svc_cfg,
  uint32               cid
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(svc_cfg);
  return (cid > 0 && cid <= svc_cfg->num_tbl_entries);
} /* qbi_svc_cid_is_valid() */

/*===========================================================================
  FUNCTION: qbi_svc_close_dev_svc
===========================================================================*/
/*!
    @brief Closes a device service

    @details

    @param svc_id

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static void qbi_svc_close_dev_svc
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id
)
{
  qbi_txn_s *txn;
  qbi_svc_action_e action;
  const qbi_svc_cfg_s *svc_cfg;
/*-------------------------------------------------------------------------*/
  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
  if (svc_cfg == NULL)
  {
    QBI_LOG_D_1("Skipping close of service ID %d - not registered", svc_id);
  }
  else if (svc_cfg->close_fcn == NULL)
  {
    QBI_LOG_D_1("No close handler registered for service ID %d", svc_id);
  }
  else
  {
    txn = qbi_txn_alloc(
      ctx, svc_id, 0, QBI_TXN_CMD_TYPE_INTERNAL, QBI_SVC_CID_NONE, 0, NULL);
    if (txn == NULL)
    {
      QBI_LOG_E_0("Couldn't allocate transaction to track close operation! "
                  "Possible leak of device service resources!!!");
    }
    else
    {
      action = svc_cfg->close_fcn(txn);
      (void) qbi_svc_proc_action(txn, action);
    }
  }
} /* qbi_svc_close_dev_svc() */

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_free_all
===========================================================================*/
/*!
    @brief Frees all memory associated with the event filter

    @details

    @param svc_state
*/
/*=========================================================================*/
static void qbi_svc_event_filter_free_all
(
  qbi_svc_state_s *svc_state
)
{
  qbi_svc_id_e svc_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(svc_state);

  for (svc_id = QBI_SVC_ID_OFFSET; svc_id < QBI_SVC_ID_MAX; svc_id++)
  {
    if (svc_state->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] != NULL)
    {
      QBI_MEM_FREE(svc_state->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)]);
      svc_state->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] = NULL;
      QBI_LOG_D_1("Freed memory for event filter for svc_id %d", svc_id);
    }
  }
} /* qbi_svc_event_filter_free_all() */

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_check
===========================================================================*/
/*!
    @brief Checks whether an outgoing event transaction is allowed to be sent

    @details

    @param txn

    @return boolean TRUE if the event can be sent, FALSE if it should be
    dropped
*/
/*=========================================================================*/
static boolean qbi_svc_event_filter_check
(
  qbi_txn_s *txn
)
{
  const qbi_svc_cfg_s *svc_cfg;
  const qbi_svc_state_s *svc_state;
  boolean allow_txn = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->ctx->svc_state);

  svc_state = txn->ctx->svc_state;
  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(txn->svc_id);
  if (svc_cfg == NULL)
  {
    QBI_LOG_E_1("Invalid svc_id %d or event filter data missing!", txn->svc_id);
  }
  else if (svc_state->event_enabled[txn->svc_id - QBI_SVC_ID_OFFSET] == NULL)
  {
    QBI_LOG_E_1("Missing event filter information for svc_id %d", txn->svc_id);
  }
  else if (!qbi_svc_cid_is_valid(svc_cfg, txn->cid))
  {
    QBI_LOG_E_2("Invalid CID %d for svc_id %d", txn->cid, txn->svc_id);
  }
  else if (!svc_state->event_enabled[txn->svc_id - QBI_SVC_ID_OFFSET]
             [txn->cid - QBI_SVC_CID_OFFSET])
  {
    QBI_LOG_I_2("Dropping event for svc_id %d CID %d due to filter rule",
                txn->svc_id, txn->cid);
  }
  else
  {
    allow_txn = TRUE;
  }

  return allow_txn;
} /* qbi_svc_event_filter_check() */

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_init
===========================================================================*/
/*!
    @brief Allocates and initializes memory for the CID event filter

    @details
    This function allocates and zeros memory used to maintain the CID event
    filter. All CID events will be initially disabled.

    @param svc_state

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_event_filter_init
(
  qbi_svc_state_s *svc_state
)
{
  qbi_svc_id_e svc_id;
  boolean result = TRUE;
  const qbi_svc_cfg_s *svc_cfg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(svc_state);

  for (svc_id = QBI_SVC_ID_OFFSET; svc_id < QBI_SVC_ID_MAX; svc_id++)
  {
    svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
    if (svc_cfg != NULL)
    {
      svc_state->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] =
        QBI_MEM_MALLOC_CLEAR(sizeof(boolean) * svc_cfg->num_tbl_entries);
      if (svc_state->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] == NULL)
      {
        QBI_LOG_E_2("Couldn't allocate memory for event filter for svc_id %d "
                    "(%d total CIDs)", svc_id, svc_cfg->num_tbl_entries);
        result = FALSE;
        break;
      }
    }
  }

  if (!result)
  {
    qbi_svc_event_filter_free_all(svc_state);
  }

  return result;
} /* qbi_svc_event_filter_init() */

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_reset
===========================================================================*/
/*!
    @brief Initiates an event filter transaction to reset the event filter
    to its default state - vendor CID events disabled, rest enabled

    @details

    @param ctx
    @param listener_txn Optional transaction, already setup as a listener,
    to be notified of the result of any QMI registration activity

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_event_filter_reset
(
  qbi_ctx_s *ctx,
  qbi_txn_s *listener_txn
)
{
  boolean success = TRUE;
  qbi_svc_id_e svc_id;
  const qbi_svc_cfg_s *svc_cfg;
  qbi_svc_event_filter_txn_s *filter_txn;
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Resetting event filter to default state");

  filter_txn = qbi_svc_event_filter_txn_alloc();
  QBI_CHECK_NULL_PTR_RET_FALSE(filter_txn);

  for (svc_id = QBI_SVC_ID_OFFSET; svc_id < QBI_SVC_ID_MAX; svc_id++)
  {
    svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
    if (svc_cfg != NULL && !svc_cfg->is_vendor_svc &&
        !qbi_svc_event_filter_txn_enable_svc(filter_txn, svc_id))
    {
      QBI_LOG_E_0("Unexpected error when trying to enable default CID events");
      success = FALSE;
      break;
    }
  }

  if (success == TRUE &&
      !qbi_svc_event_filter_txn_commit(ctx, filter_txn, listener_txn))
  {
    QBI_LOG_E_0("Error committing default event filter transaction");
    success = FALSE;
  }
  qbi_svc_event_filter_txn_free(filter_txn);

  return success;
} /* qbi_svc_event_filter_reset() */

/*===========================================================================
  FUNCTION: qbi_svc_qmi_ind_ref_cnt_free_all
===========================================================================*/
/*!
    @brief Frees memory associated with QMI indication registration
    reference count entries

    @details

    @param svc_state
*/
/*=========================================================================*/
static void qbi_svc_qmi_ind_ref_cnt_free_all
(
  qbi_svc_state_s *svc_state
)
{
  qbi_util_list_iter_s iter;
  qbi_svc_ind_qmi_ref_cnt_entry_s *entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(svc_state);

  qbi_util_list_iter_init(&svc_state->qmi_ind_ref_cnts, &iter);
  while ((entry = qbi_util_list_iter_next(&iter)) != NULL)
  {
    qbi_util_list_iter_remove(&iter);
    QBI_MEM_FREE(entry);
  }
} /* qbi_svc_qmi_ind_ref_cnt_free_all() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_ref_cnt_update
===========================================================================*/
/*!
    @brief Searches for an existing QMI indication registration reference
    count entry, and if found, increments or decrements it

    @details
    Will remove an entry if its reference count is decremented to zero. This
    function is meant to be used as a helper to qbi_svc_ind_qmi_ref_cnt_inc
    and qbi_svc_ind_qmi_ref_cnt_dec, rather than being called directly to
    update the reference count, as it will not add a new entry when
    increment is requested and an entry was not found.

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id
    @param increment Provide to TRUE to increment the counter, FALSE to
    decrement
    @param new_ref_cnt Will be set to the new reference count value, or
    zero if an entry was not found

    @return boolean TRUE if a matching entry was found, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_ind_qmi_ref_cnt_update
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id,
  boolean       increment,
  uint16       *new_ref_cnt
)
{
  boolean entry_found = FALSE;
  qbi_util_list_iter_s iter;
  qbi_svc_ind_qmi_ref_cnt_entry_s *entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx->svc_state);
  QBI_CHECK_NULL_PTR_RET_FALSE(new_ref_cnt);

  *new_ref_cnt = 0;
  qbi_util_list_iter_init(&ctx->svc_state->qmi_ind_ref_cnts, &iter);
  while ((entry = qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (entry->qmi_svc_id == qmi_svc_id && entry->qmi_msg_id == qmi_msg_id)
    {
      entry_found = TRUE;
      if (increment)
      {
        if (entry->ref_cnt++ >= QBI_SVC_QMI_REF_CNT_MAX)
        {
          QBI_LOG_E_2("Tried to increment reference count when at max value! "
                      "qmi_svc_id %d qmi_msg_id 0x%02x", entry->qmi_svc_id,
                      entry->qmi_msg_id);
          entry->ref_cnt = QBI_SVC_QMI_REF_CNT_MAX;
        }
        *new_ref_cnt = entry->ref_cnt;
      }
      else
      {
        if (entry->ref_cnt-- == 0)
        {
          QBI_LOG_E_2("Tried to decrement reference count when already zero! "
                      "qmi_svc_id %d qmi_msg_id 0x%02x", entry->qmi_svc_id,
                      entry->qmi_msg_id);
          entry->ref_cnt = 0;
        }
        *new_ref_cnt = entry->ref_cnt;

        if (entry->ref_cnt == 0)
        {
          qbi_util_list_iter_remove(&iter);
          QBI_MEM_FREE(entry);
          entry = NULL;
        }
      }
      QBI_LOG_D_3("Reference count for qmi_svc_id %d qmi_msg_id 0x%02x updated "
                  "to %d", qmi_svc_id, qmi_msg_id, *new_ref_cnt);
      break;
    }
  }

  return entry_found;
} /* qbi_svc_ind_qmi_ref_cnt_update() */

/*===========================================================================
  FUNCTION: qbi_svc_id_is_valid
===========================================================================*/
/*!
    @brief Check whether a service ID is in the valid range

    @details

    @param svc_id

    @return boolean TRUE if a service ID is in the valid range, FALSE
    otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_id_is_valid
(
  qbi_svc_id_e svc_id
)
{
/*-------------------------------------------------------------------------*/
  return (svc_id > QBI_SVC_ID_UNKNOWN && svc_id < QBI_SVC_ID_MAX);
} /* qbi_svc_id_is_valid() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_info_dynamic_is_equal
===========================================================================*/
/*!
    @brief Check whether an qbi_svc_ind_info_dynamic_s contains the same
    data as a set of fields

    @details
    Compares a stored ind_info struct against a second set of fields to
    determine if they contain the same information, i.e. equal.

    @param ind_info
    @param svc_id
    @param cid
    @param qmi_svc_id_range_start Start of range, or individual QMI service
    ID
    @param qmi_svc_id_range_end End of range, or QBI_QMI_SVC_ID_RANGE_INVALID
    if single QMI service ID is used
    @param qmi_msg_id
    @param cb
    @param txn
    @param cb_data

    @return boolean TRUE if ind_info contains the same information as the
    rest of the args, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_ind_info_dynamic_is_equal
(
  const qbi_svc_ind_info_dynamic_s *dyn_ind_info,
  qbi_svc_id_e                      svc_id,
  uint32                            cid,
  qbi_qmi_svc_e                     qmi_svc_id_range_start,
  qbi_qmi_svc_e                     qmi_svc_id_range_end,
  uint16                            qmi_msg_id,
  qbi_svc_qmi_ind_cb_f             *cb,
  const qbi_txn_s                  *txn,
  const void                       *cb_data
)
{
  qbi_svc_qmi_ind_cb_f *qmi_ind_cb = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(dyn_ind_info);

  qbi_os_fptr_decode(&dyn_ind_info->encoded_cb,
                     (qbi_os_void_f **) &qmi_ind_cb, TRUE);

  return (dyn_ind_info->svc_id == svc_id &&
          dyn_ind_info->txn == txn &&
          dyn_ind_info->ind_info.cid == cid &&
          dyn_ind_info->ind_info.qmi_svc_id == qmi_svc_id_range_start &&
          dyn_ind_info->qmi_svc_id_range_end == qmi_svc_id_range_end &&
          dyn_ind_info->ind_info.qmi_msg_id == qmi_msg_id &&
          qmi_ind_cb == cb &&
          dyn_ind_info->ind_info.cb_data == cb_data);
} /* qbi_svc_ind_info_dynamic_is_equal() */

/*===========================================================================
  FUNCTION: qbi_svc_look_up_cfg_by_svc_id
===========================================================================*/
/*!
    @brief Find a service configuration by the internal ID

    @details

    @param svc_id

    @return const qbi_svc_cfg_s*
*/
/*=========================================================================*/
static const qbi_svc_cfg_s *qbi_svc_look_up_cfg_by_svc_id
(
  qbi_svc_id_e svc_id
)
{
  const qbi_svc_cfg_s *svc_cfg = NULL;
/*-------------------------------------------------------------------------*/
  if (qbi_svc_id_is_valid(svc_id))
  {
    svc_cfg = qbi_svc_cfg[QBI_SVC_ID_TO_INDEX(svc_id)];
  }
  return svc_cfg;
} /* qbi_svc_look_up_cfg_by_svc_id() */

/*===========================================================================
  FUNCTION: qbi_svc_look_up_cfg_by_uuid
===========================================================================*/
/*!
    @brief Find a service configuration by its UUID

    @details

    @param uuid

    @return const qbi_svc_cfg_s*
*/
/*=========================================================================*/
static const qbi_svc_cfg_s *qbi_svc_look_up_cfg_by_uuid
(
  const uint8 *uuid
)
{
  int i;
  const qbi_svc_cfg_s *svc_cfg = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(uuid);

  for (i = 0; i < ARR_SIZE(qbi_svc_cfg); i++)
  {
    if (qbi_svc_cfg[i] != NULL && qbi_svc_cfg[i]->uuid != NULL &&
        QBI_MEMCMP(uuid, qbi_svc_cfg[i]->uuid, QBI_MBIM_UUID_LEN) == 0)
    {
      svc_cfg = qbi_svc_cfg[i];
      break;
    }
  }

  if (svc_cfg == NULL)
  {
    QBI_LOG_E_0("Received unrecognized UUID");
  }
  return svc_cfg;
} /* qbi_svc_look_up_cfg_by_uuid() */

/*===========================================================================
  FUNCTION: qbi_svc_open_next_dev_svc
===========================================================================*/
/*!
    @brief Opens the next device service on the transaction

    @details
    This function is used to open all available device services in order.
    Note that this function may be called recursively if the device service
    open operation completes synchronously.

    This is the call flow leading to recursive invocation:
    qbi_svc_open_next_dev_svc() -->
    qbi_svc_proc_action(txn, QBI_SVC_ACTION_SEND_RSP) -->
    qbi_svc_proc_open_done() -->
    qbi_svc_open_next_dev_svc()

    @param txn
    @param no_more_svcs set to TRUE if all available services have been
    opened already

    @return boolean TRUE if an open handler was invoked and this resulted
    in the freeing of the transaction, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_open_next_dev_svc
(
  qbi_txn_s *txn,
  boolean   *no_more_svcs
)
{
  boolean txn_freed = FALSE;
  const qbi_svc_cfg_s *svc_cfg;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(no_more_svcs);

  *no_more_svcs = TRUE;
  for (txn->svc_id++; txn->svc_id < QBI_SVC_ID_MAX; txn->svc_id++)
  {
    svc_cfg = qbi_svc_look_up_cfg_by_svc_id(txn->svc_id);
    if (svc_cfg != NULL && svc_cfg->open_fcn != NULL)
    {
      action = svc_cfg->open_fcn(txn);
      /* Note that qbi_svc_proc_action may end up invoking this function
         again */
      txn_freed = qbi_svc_proc_action(txn, action);
      *no_more_svcs = FALSE;
      break;
    }
  }

  return txn_freed;
} /* qbi_svc_open_next_dev_svc() */

/*===========================================================================
  FUNCTION: qbi_svc_open_notify_cb
===========================================================================*/
/*!
    @brief Notifier callback used in the final stage of MBIM_OPEN processing

    @details

    @param listener_txn
    @param notifier_txn
*/
/*=========================================================================*/
static void qbi_svc_open_notify_cb
(
  qbi_txn_s       *listener_txn,
  const qbi_txn_s *notifier_txn
)
{
  boolean send_rsp = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(listener_txn);
  QBI_CHECK_NULL_PTR_RET(notifier_txn);

  if (notifier_txn->status != QBI_MBIM_STATUS_SUCCESS)
  {
    QBI_LOG_E_1("QMI registration txn iid %d failed!", notifier_txn->iid);
    listener_txn->status = QBI_MBIM_STATUS_FAILURE;
    send_rsp = TRUE;
  }
  else if (listener_txn->notifier_count == 0)
  {
    QBI_LOG_D_0("All QMI registration requests completed successfully");
    send_rsp = TRUE;
  }

  if (send_rsp)
  {
    qbi_msg_send_open_rsp(listener_txn);
  }
} /* qbi_svc_open_notify_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_proc_open_done
===========================================================================*/
/*!
    @brief Called when opening a single device service opened successfully

    @details
    Tries to open the next device service. If all services already opened,
    sends the response to the host. Note that this function may be called
    recursively via qbi_svc_open_next_dev_svc() --> qbi_svc_proc_action()
    if the open operation completes synchronously

    @param txn

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_proc_open_done
(
  qbi_txn_s *txn
)
{
  boolean txn_freed = FALSE;
  boolean no_more_svcs;
  boolean send_rsp = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->ctx->svc_state);

  QBI_LOG_I_1("Service %d opened successfully", txn->svc_id);
  txn_freed = qbi_svc_open_next_dev_svc(txn, &no_more_svcs);
  if (!txn_freed && no_more_svcs)
  {
    (void) qbi_txn_notify_setup_listener(txn, qbi_svc_open_notify_cb);
    if (!qbi_svc_event_filter_reset(txn->ctx, txn))
    {
      txn->status = QBI_MBIM_STATUS_FAILURE;
      send_rsp = TRUE;
    }
    else if (txn->notifier_count == 0)
    {
      send_rsp = TRUE;
    }

    /* Send the response now if we failed, otherwise the response will be sent
       from the notify callback */
    if (send_rsp)
    {
      qbi_msg_send_open_rsp(txn);
      txn_freed = TRUE;
    }
  }

  return txn_freed;
} /* qbi_svc_proc_open_done() */

/*===========================================================================
  FUNCTION: qbi_svc_sanity_check_ind_info
===========================================================================*/
/*!
    @brief Performs a sanity check on an indication info struct

    @details

    @param ind_info
    @param svc_id
    @param null_cb_allowed

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_sanity_check_ind_info
(
  const qbi_svc_ind_info_s *ind_info,
  qbi_svc_id_e              svc_id,
  boolean                   null_cb_allowed
)
{
  const qbi_svc_cfg_s *svc_cfg;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ind_info);

  if (ind_info->cb == NULL && !null_cb_allowed)
  {
    QBI_LOG_E_0("Callback function configuration is invalid");
  }
  else
  {
    svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
    if (svc_cfg == NULL)
    {
      QBI_LOG_E_1("Invalid service ID %d", svc_id);
    }
    else if (ind_info->cid != QBI_SVC_CID_NONE &&
             !qbi_svc_cid_is_valid(svc_cfg, ind_info->cid))
    {
      QBI_LOG_E_2("CID %d out of range for service ID %d",
                  ind_info->cid, svc_id);
    }
    else if (ind_info->qmi_svc_id >= QBI_QMI_NUM_SVCS)
    {
      QBI_LOG_E_1("Invalid QMI service ID %d", ind_info->qmi_svc_id);
    }
    else
    {
      success = TRUE;
    }
  }

  return success;
} /* qbi_svc_sanity_check_ind_info() */

/*===========================================================================
  FUNCTION: qbi_svc_qmi_ind_dispatch_dynamic
===========================================================================*/
/*!
    @brief Invokes registered dynamic indication handler callbacks for the
    given QMI indication data

    @details

    @param ctx
    @param ind_data Must be pre-populated with qmi_svc_id, qmi_msg_id, and
    qmi_ind_buf
*/
/*=========================================================================*/
static void qbi_svc_qmi_ind_dispatch_dynamic
(
  qbi_ctx_s              *ctx,
  qbi_svc_qmi_ind_data_s *ind_data
)
{
  qbi_svc_action_e action;
  qbi_util_list_iter_s iter;
  qbi_svc_ind_info_dynamic_s *dyn_ind_info;
  qbi_svc_qmi_ind_cb_f *qmi_ind_cb = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(ctx->svc_state);

  qbi_util_list_iter_init(&ctx->svc_state->inds_dynamic, &iter);
  qbi_util_list_iter_reg(&iter);
  while (qbi_util_list_iter_has_next(&iter))
  {
    dyn_ind_info = (qbi_svc_ind_info_dynamic_s *)
      qbi_util_list_iter_next(&iter);
    if (dyn_ind_info != NULL &&
        (dyn_ind_info->ind_info.qmi_svc_id == ind_data->qmi_svc_id ||
         (dyn_ind_info->qmi_svc_id_range_start !=
            QBI_SVC_QMI_SVC_ID_RANGE_INVALID &&
          ind_data->qmi_svc_id >= dyn_ind_info->qmi_svc_id_range_start &&
          ind_data->qmi_svc_id <= dyn_ind_info->qmi_svc_id_range_end)) &&
        dyn_ind_info->ind_info.qmi_msg_id == ind_data->qmi_msg_id &&
        qbi_svc_sanity_check_ind_info(
          &dyn_ind_info->ind_info, dyn_ind_info->svc_id, TRUE))
    {
      if (dyn_ind_info->txn != NULL)
      {
        ind_data->txn = dyn_ind_info->txn;
      }
      else
      {
        ind_data->txn = qbi_txn_alloc_event(
          ctx, dyn_ind_info->svc_id, dyn_ind_info->ind_info.cid);
      }

      if (ind_data->txn == NULL)
      {
        QBI_LOG_E_0("Unexpected NULL pointer!");
      }
      else
      {
        ind_data->cb_data = dyn_ind_info->ind_info.cb_data;

        if (!qbi_os_fptr_decode(&dyn_ind_info->encoded_cb,
                                (qbi_os_void_f **) &qmi_ind_cb, FALSE))
        {
          QBI_LOG_E_0("Unable to decode function pointer");
        }
        else
        {
          action = qmi_ind_cb(ind_data);
          (void) qbi_svc_proc_action(ind_data->txn, action);
        }
      }
    }
  }
  qbi_util_list_iter_dereg(&iter);
} /* qbi_svc_qmi_ind_dispatch_dynamic() */

/*===========================================================================
  FUNCTION: qbi_svc_qmi_ind_dispatch_static
===========================================================================*/
/*!
    @brief Invokes registered static indication handler callbacks for the
    given QMI indication

    @details

    @param ctx
    @param ind_data Must be pre-populated with qmi_svc_id, qmi_msg_id, and
    qmi_ind_buf
*/
/*=========================================================================*/
static void qbi_svc_qmi_ind_dispatch_static
(
  qbi_ctx_s              *ctx,
  qbi_svc_qmi_ind_data_s *ind_data
)
{
  uint32 i;
  qbi_svc_id_e svc_id;
  qbi_svc_action_e action;
  const qbi_svc_ind_info_s *ind_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(ctx->svc_state);
  QBI_CHECK_NULL_PTR_RET(ind_data);

  for (svc_id = QBI_SVC_ID_OFFSET; svc_id < QBI_SVC_ID_MAX; svc_id++)
  {
    if (qbi_svc_id_is_valid(svc_id) &&
        ctx->svc_state->inds_static[QBI_SVC_ID_TO_INDEX(svc_id)] != NULL)
    {
      for (i = 0; i < ctx->svc_state->num_static_ind_hdlrs[QBI_SVC_ID_TO_INDEX(svc_id)]; i++)
      {
        ind_info = &ctx->svc_state->inds_static[QBI_SVC_ID_TO_INDEX(svc_id)][i];
        if (ind_info->qmi_svc_id == ind_data->qmi_svc_id &&
            ind_info->qmi_msg_id == ind_data->qmi_msg_id)
        {
          ind_data->txn = qbi_txn_alloc_event(ctx, svc_id, ind_info->cid);
          QBI_CHECK_NULL_PTR_RET(ind_data->txn);

          ind_data->cb_data = ind_info->cb_data;
          action = ind_info->cb(ind_data);
          (void) qbi_svc_proc_action(ind_data->txn, action);
        }
      } /* for each registered static indication handler */
    }
  } /* for each service */
} /* qbi_svc_qmi_ind_dispatch_static() */


/*=============================================================================

  Public Function Definitions

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_svc_abort_txn
===========================================================================*/
/*!
    @brief Abort a transaction and notify the host of the error if the
    transaction is for a request

    @details

    @param txn
    @param status Status code to send to the host
*/
/*=========================================================================*/
void qbi_svc_abort_txn
(
  qbi_txn_s *txn,
  uint32     status
)
{
  boolean skip_txn_free = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  /* In this situation, the heap may be full, so we can't rely on
     qbi_msg_send_cmd_rsp as it uses the heap to build the response
     packet. Instead, use the stack-only qbi_msg_send_cmd_error_rsp. */
  if (txn->cmd_type == QBI_MSG_CMD_TYPE_QUERY ||
      txn->cmd_type == QBI_MSG_CMD_TYPE_SET)
  {
    #ifdef QBI_UNIT_TEST
    qbi_util_buf_chain_free_all(&txn->rsp_databuf);
    qbi_util_buf_free(&txn->rsp);
    txn->infobuf_len_total = 0;
    if (qbi_utf_txn_receive(txn))
    {
      return;
    }
    #endif /* QBI_UNIT_TEST */

    qbi_msg_send_cmd_error_rsp(txn->ctx, txn->txn_id, txn->cid,
                               qbi_svc_svc_id_to_uuid(txn->svc_id), status);
  }
  else if (txn->cmd_type == QBI_TXN_CMD_TYPE_MBIM_OPEN)
  {
    qbi_msg_send_open_rsp(txn);
    skip_txn_free = TRUE;
  }

  /* If MBIM_OPEN is aborted, then txn will already be freed by
     qbi_msg_send_open_rsp():qbi_msg_context_close():qbi_txn_free_all() */
  if (!skip_txn_free)
  {
    qbi_txn_free(txn);
  }
} /* qbi_svc_abort_txn() */

/*===========================================================================
  FUNCTION: qbi_svc_cache_alloc
===========================================================================*/
/*!
    @brief Allocate a region of memory to be used as a device service's cache

    @details
    The caller (device service) is responsible for ensuring that the cache
    is freed when it is no longer needed, e.g. after the device service is
    closed.

    @param ctx
    @param svc_id Device service associated with the cache
    @param size Size in bytes of the memory to allocate

    @return void* Pointer to the newly allocated cache area, or NULL on
    failure

    @see qbi_svc_cache_free, qbi_svc_cache_get
*/
/*=========================================================================*/
void *qbi_svc_cache_alloc
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       size
)
{
  void *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(ctx->svc_state);

  if (!qbi_svc_id_is_valid(svc_id))
  {
    QBI_LOG_E_1("Invalid service ID %d", svc_id);
  }
  else if (ctx->svc_state->cache[QBI_SVC_ID_TO_INDEX(svc_id)] != NULL)
  {
    QBI_LOG_E_1("Existing cache ptr for svc_id %d, expected NULL", svc_id);
  }
  else
  {
    cache = QBI_MEM_MALLOC_CLEAR(size);
    if (cache == NULL)
    {
      QBI_LOG_E_0("Couldn't alloc memory for cache");
    }
    else
    {
      ctx->svc_state->cache[QBI_SVC_ID_TO_INDEX(svc_id)] = cache;
    }
  }

  return cache;
} /* qbi_svc_cache_alloc() */

/*===========================================================================
  FUNCTION: qbi_svc_cache_free
===========================================================================*/
/*!
    @brief Free memory associated with a device service's cache area

    @details
    Device services are required to release their cache when it is no
    longer needed, e.g. when their close function is invoked.

    @param ctx
    @param svc_id
*/
/*=========================================================================*/
void qbi_svc_cache_free
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(ctx->svc_state);

  if (!qbi_svc_id_is_valid(svc_id))
  {
    QBI_LOG_E_1("Invalid service ID %d", svc_id);
  }
  else if (ctx->svc_state->cache[QBI_SVC_ID_TO_INDEX(svc_id)] == NULL)
  {
    QBI_LOG_D_1("Cache for svc_id %d already NULL", svc_id);
  }
  else
  {
    QBI_MEM_FREE(ctx->svc_state->cache[QBI_SVC_ID_TO_INDEX(svc_id)]);
    ctx->svc_state->cache[QBI_SVC_ID_TO_INDEX(svc_id)] = NULL;
  }
} /* qbi_svc_cache_free() */

/*===========================================================================
  FUNCTION: qbi_svc_cache_get
===========================================================================*/
/*!
    @brief Retrieve a pointer to the given device service's cache area

    @details
    The cache should have been previously allocated via qbi_svc_cache_alloc.

    @param ctx
    @param svc_id

    @return void* Pointer to cache area, or NULL if error/no cache is
    allocated
*/
/*=========================================================================*/
void *qbi_svc_cache_get
(
  const qbi_ctx_s *ctx,
  qbi_svc_id_e     svc_id
)
{
  void *cache = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(ctx->svc_state);

  if (!qbi_svc_id_is_valid(svc_id))
  {
    QBI_LOG_E_1("Invalid service ID %d", svc_id);
  }
  else
  {
    cache = ctx->svc_state->cache[QBI_SVC_ID_TO_INDEX(svc_id)];
  }

  return cache;
} /* qbi_svc_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_close
===========================================================================*/
/*!
    @brief Performs context-wide device service de-initialization

    @details
    Intended to be used when the device is closed via MBIM_CLOSE_MSG

    @param ctx

    @see qbi_svc_open
*/
/*=========================================================================*/
void qbi_svc_close
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_id_e svc_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(ctx->svc_state);

  /* Close all open device services. Since the close operation is not allowed to
     fail, all device services are given their own transaction to perform
     asynchronous operations in their close handler. Therefore multiple device
     services may be actively performing close operations in parallel, unlike
     the open operation which involves opening each device service in series. */
  for (svc_id = QBI_SVC_ID_OFFSET; svc_id < QBI_SVC_ID_MAX; svc_id++)
  {
    qbi_svc_close_dev_svc(ctx, svc_id);
  }

  /* Deregister all indication handlers and free all caches. */
  QBI_LOG_I_0("Releasing indication handlers and service caches");
  for (svc_id = QBI_SVC_ID_BC; svc_id < QBI_SVC_ID_MAX; svc_id++)
  {
    qbi_svc_ind_dereg_svc(ctx, svc_id);
    qbi_svc_cache_free(ctx, svc_id);
  }

  /* Release the service state memory. */
  qbi_svc_event_filter_free_all(ctx->svc_state);
  qbi_svc_qmi_ind_ref_cnt_free_all(ctx->svc_state);
  QBI_MEM_FREE(ctx->svc_state);
  ctx->svc_state = NULL;
} /* qbi_svc_close() */

/*===========================================================================
  FUNCTION: qbi_svc_dereg
===========================================================================*/
/*!
    @brief Deregisters a device service

    @details
    Removes the reference to the device service configuration previously
    registered via qbi_svc_reg. Support for the device service will be
    effectively removed until qbi_svc_reg is called again.

    @param svc_id
*/
/*=========================================================================*/
void qbi_svc_dereg
(
  qbi_svc_id_e svc_id
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_svc_id_is_valid(svc_id))
  {
    qbi_svc_cfg[QBI_SVC_ID_TO_INDEX(svc_id)] = NULL;
    QBI_LOG_I_1("Deregistered svc_id %d", svc_id);
  }
  else
  {
    QBI_LOG_E_1("Couldn't deregister svc_id %d: invalid service ID", svc_id);
  }
} /* qbi_svc_dereg() */

/*===========================================================================
  FUNCTION: qbi_svc_dispatch
===========================================================================*/
/*!
    @brief Dispatch a command to the appropriate CID handler

    @details

    @param txn
*/
/*=========================================================================*/
void qbi_svc_dispatch
(
  qbi_txn_s *txn
)
{
  const qbi_svc_cfg_s *svc_cfg;
  qbi_svc_cmd_hdlr_f *hdlr_fcn = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_NULL;
  uint32 min_req_size = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->ctx);
  QBI_CHECK_NULL_PTR_RET(txn->ctx->svc_state);

  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(txn->svc_id);
  if (svc_cfg == NULL)
  {
    QBI_LOG_E_1("Couldn't find device service ID %d", txn->svc_id);
  }
  else if (!qbi_svc_cid_is_valid(svc_cfg, txn->cid))
  {
    QBI_LOG_E_2("CID %d out of supported range of service ID %d",
                txn->cid, txn->svc_id);
  }
  else
  {
    /* Note that we will see CMD_TYPE_EVENT here if we are called by
       qbi_svc_force_event() */
    if (txn->cmd_type == QBI_MSG_CMD_TYPE_QUERY ||
        txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT)
    {
      hdlr_fcn = svc_cfg->dispatch_tbl[txn->cid - QBI_SVC_CID_OFFSET].query_fcn;
      min_req_size = svc_cfg->dispatch_tbl[txn->cid - QBI_SVC_CID_OFFSET].
        query_infobuf_min_size;
    }
    else if (txn->cmd_type == QBI_MSG_CMD_TYPE_SET)
    {
      hdlr_fcn = svc_cfg->dispatch_tbl[txn->cid - QBI_SVC_CID_OFFSET].set_fcn;
      min_req_size = svc_cfg->dispatch_tbl[txn->cid - QBI_SVC_CID_OFFSET].
        set_infobuf_min_size;
    }

    if (hdlr_fcn == NULL)
    {
      QBI_LOG_E_3("Unsupported cmd_type %d for CID %d (service ID %d)",
                  txn->cmd_type, txn->cid, txn->svc_id);
    }
    else if (txn->req.size < min_req_size)
    {
      QBI_LOG_E_4("Invalid InformationBuffer size %d in request for svc %d "
                  "cid %d: expected at least %d", txn->req.size, txn->svc_id,
                  txn->cid, min_req_size);
      txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
      action = QBI_SVC_ACTION_ABORT;
    }
    else
    {
      QBI_LOG_D_4("Dispatching txn_id 0x%08x svc_id %d cid %d cmd_type 0x%x",
                  txn->txn_id, txn->svc_id, txn->cid, txn->cmd_type);
      action = hdlr_fcn(txn);
    }
  }

  /* Handle the case where we didn't dispatch the transaction. The assumption
     here is that this will only happen when the service/CID is not supported */
  if (action == QBI_SVC_ACTION_NULL)
  {
    if (txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT)
    {
      QBI_LOG_E_2("Tried to force event for invalid svc_id %d cid %d",
                  txn->svc_id, txn->cid);
      action = QBI_SVC_ACTION_ABORT;
    }
    else
    {
      txn->status = QBI_MBIM_STATUS_NO_DEVICE_SUPPORT;
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }

  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_dispatch() */

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_txn_alloc
===========================================================================*/
/*!
    @brief Allocates and initializes a transaction used for updating the
    event filter

    @details
    An event filter transaction allows for the complete set of changes to
    the event filter to be prepared non-destructively, then all applied at
    once by the commit operation. All events are initially disabled in the
    transaction, and should be selectively enabled via
    qbi_svc_event_filter_txn_enable_cid(). Once all changes are ready, call
    qbi_svc_event_filter_txn_commit() to apply the changes to the active
    event filter, then qbi_svc_event_filter_txn_free() to release all memory
    allocated by qbi_svc_event_filter_txn_alloc().

    @return qbi_svc_event_filter_txn_s* Opaque handle to be used with further
    event filter transactions
*/
/*=========================================================================*/
qbi_svc_event_filter_txn_s *qbi_svc_event_filter_txn_alloc
(
  void
)
{
  qbi_svc_id_e svc_id;
  const qbi_svc_cfg_s *svc_cfg;
  qbi_svc_event_filter_txn_s *filter_txn;
/*-------------------------------------------------------------------------*/
  filter_txn = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_event_filter_txn_s));
  QBI_CHECK_NULL_PTR_RET_NULL(filter_txn);

  for (svc_id = QBI_SVC_ID_OFFSET; svc_id < QBI_SVC_ID_MAX; svc_id++)
  {
    svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
    if (svc_cfg != NULL)
    {
      filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] =
        QBI_MEM_MALLOC_CLEAR(sizeof(boolean) * svc_cfg->num_tbl_entries);
      if (filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] == NULL)
      {
        QBI_LOG_E_2("Couldn't allocate memory for event filter transaction "
                    "(svc_id %d num_cids %d)", svc_id, svc_cfg->num_tbl_entries);
        qbi_svc_event_filter_txn_free(filter_txn);
        filter_txn = NULL;
        break;
      }
    }
  }

  return filter_txn;
} /* qbi_svc_event_filter_txn_alloc() */

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_txn_commit
===========================================================================*/
/*!
    @brief Applies the changes in the transaction to the active event filter
    for the given context

    @details
    This function does not free any memory associated with the event filter
    transaction, so qbi_svc_event_filter_txn_free must be called. When this
    function returns successfully, the event filter will be copied into
    active memory, so the filter_txn can be safely freed. However, one or
    more QMI requests may have been dispatched to modify indication
    registration. The result of these requests may be monitored through
    the optional listener_txn.

    @param ctx
    @param filter_txn
    @param listener_txn Optional (may be NULL) transaction, already
    configured as a listener, to be notified of the result of asynchronous
    QMI registration activity created as a result of this event filter

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_event_filter_txn_commit
(
  qbi_ctx_s                  *ctx,
  qbi_svc_event_filter_txn_s *filter_txn,
  qbi_txn_s                  *listener_txn
)
{
  qbi_svc_id_e svc_id;
  const qbi_svc_cfg_s *svc_cfg;
  uint32 cid_offset;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx->svc_state);
  QBI_CHECK_NULL_PTR_RET_FALSE(filter_txn);

  for (svc_id = QBI_SVC_ID_OFFSET; success && svc_id < QBI_SVC_ID_MAX; svc_id++)
  {
    svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
    if (svc_cfg == NULL)
    {
      QBI_LOG_D_1("Skipping unsupported svc_id %d", svc_id);
    }
    else if (filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] == NULL ||
             ctx->svc_state->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] == NULL)
    {
      QBI_LOG_W_1("Missing event filter data for svc_id %d", svc_id);
    }
    else
    {
      /* This loop is based on CID array offset rather than the CID value
         itself. Add QBI_SVC_CID_OFFSET to cid_offset to get the actual CID
         value. */
      for (cid_offset = 0; cid_offset < svc_cfg->num_tbl_entries; cid_offset++)
      {
        /* Check if the setting in the event filter transaction differs from
           the active event filter, if it does then update the active event
           filter. If that fails, then bail out. */
        if (filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)][cid_offset] !=
              ctx->svc_state->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)][cid_offset] &&
            !qbi_svc_event_filter_update_cid(
              ctx, svc_id, cid_offset + QBI_SVC_CID_OFFSET,
              filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)]
                [cid_offset], listener_txn))
        {
          QBI_LOG_E_2("Couldn't update event filter for svc_id %d cid %d",
                      svc_id, cid_offset + QBI_SVC_CID_OFFSET);
          success = FALSE;
          break;
        }
      }
    }
  }

  return success;
} /* qbi_svc_event_filter_txn_commit() */

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_txn_enable_cid
===========================================================================*/
/*!
    @brief Enables events for the given CID in the event filter transaction

    @details
    CID events are not actually updated until the event filter transaction
    is committed.

    @param filter_txn
    @param svc_id
    @param cid

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_event_filter_txn_enable_cid
(
  qbi_svc_event_filter_txn_s *filter_txn,
  qbi_svc_id_e                svc_id,
  uint32                      cid
)
{
  boolean success = FALSE;
  const qbi_svc_cfg_s *svc_cfg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(filter_txn);

  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
  if (svc_cfg == NULL)
  {
    QBI_LOG_E_2("Tried to enable CID %d on invalid svc_id %d", cid, svc_id);
  }
  else if (filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] == NULL)
  {
    QBI_LOG_E_2("Tried to enable CID %d on svc_id %d, but event array is NULL",
                cid, svc_id);
  }
  else if (cid > svc_cfg->num_tbl_entries)
  {
    QBI_LOG_E_2("Invalid CID %d for svc_id %d", cid, svc_id);
  }
  else
  {
    filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)]
      [cid - QBI_SVC_CID_OFFSET] = TRUE;
    success = TRUE;
  }

  return success;
} /* qbi_svc_event_filter_txn_enable_cid() */

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_txn_enable_svc
===========================================================================*/
/*!
    @brief Enables all CID events for the given device service in the event
    filter transaction

    @details
    CID events are not actually updated until the event filter transaction
    is committed.

    @param filter_txn
    @param svc_id

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_event_filter_txn_enable_svc
(
  qbi_svc_event_filter_txn_s *filter_txn,
  qbi_svc_id_e                svc_id
)
{
  boolean success = FALSE;
  const qbi_svc_cfg_s *svc_cfg;
  uint32 i;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(filter_txn);

  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
  if (svc_cfg == NULL)
  {
    QBI_LOG_E_1("Tried to enable all CIDs on invalid svc_id %d", svc_id);
  }
  else if (filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] == NULL)
  {
    QBI_LOG_E_1("Tried to enable all CIDs on svc_id %d, but event array is "
                "NULL", svc_id);
  }
  else
  {
    for (i = 0; i < svc_cfg->num_tbl_entries; i++)
    {
      filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)][i] = TRUE;
    }
    success = TRUE;
  }

  return success;
} /* qbi_svc_event_filter_txn_enable_svc() */

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_txn_free
===========================================================================*/
/*!
    @brief Releases all memory associated with an event filter transaction

    @details

    @param filter_txn Filter transaction handle to free. Will be invalidated
    after this function returns, so it must not be used.
*/
/*=========================================================================*/
void qbi_svc_event_filter_txn_free
(
  qbi_svc_event_filter_txn_s *filter_txn
)
{
  qbi_svc_id_e svc_id;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(filter_txn);

  for (svc_id = QBI_SVC_ID_OFFSET; svc_id < QBI_SVC_ID_MAX; svc_id++)
  {
    if (filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] != NULL)
    {
      QBI_MEM_FREE(filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)]);
      filter_txn->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] = NULL;
    }
  }

  QBI_MEMSET(filter_txn, 0, sizeof(qbi_svc_event_filter_txn_s));
  QBI_MEM_FREE(filter_txn);
} /* qbi_svc_event_filter_txn_free() */

/*===========================================================================
  FUNCTION: qbi_svc_event_filter_update_cid
===========================================================================*/
/*!
    @brief Updates the active event filter for a given CID

    @details
    Invokes the relevant QMI indication registration function if the value
    changed.

    @param ctx
    @param svc_id
    @param cid
    @param event_enabled Set to TRUE to enable events for this CID, and
    FALSE to disable
    @param listener_txn Optional QBI transaction configured as a listener
    to be notified of the result of QMI indication registration

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_event_filter_update_cid
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       cid,
  boolean      event_enabled,
  qbi_txn_s   *listener_txn
)
{
  boolean old_value;
  boolean result = FALSE;
  const qbi_svc_cfg_s *svc_cfg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx->svc_state);

  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
  if (svc_cfg == NULL)
  {
    QBI_LOG_E_1("Couldn't look up service config for svc_id %d", svc_id);
  }
  else if (ctx->svc_state->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)] == NULL)
  {
    QBI_LOG_E_1("Missing event filter data for svc_id %d", svc_id);
  }
  else if (!qbi_svc_cid_is_valid(svc_cfg, cid))
  {
    QBI_LOG_E_2("Invalid CID %d for svc_id %d", cid, svc_id);
  }
  else
  {
    old_value = ctx->svc_state->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)]
      [cid - QBI_SVC_CID_OFFSET];
    if (old_value != event_enabled)
    {
      ctx->svc_state->event_enabled[QBI_SVC_ID_TO_INDEX(svc_id)]
        [cid - QBI_SVC_CID_OFFSET] = event_enabled;
      result = qbi_svc_ind_qmi_reg_invoke(
        ctx, svc_id, cid, event_enabled, listener_txn);
    }
    else
    {
      result = TRUE;
    }

    QBI_LOG_D_3("Event filter for CID %d in svc_id %d set to %d",
                cid, svc_id, event_enabled);
    result = TRUE;
  }

  return result;
} /* qbi_svc_event_filter_update_cid() */

/*===========================================================================
  FUNCTION: qbi_svc_force_event
===========================================================================*/
/*!
    @brief Allocates an event transaction and passes it to the CID's query
    handler function

    @details
    The usual flow for a CID event is to handle a QMI indication, judge
    whether it resulted in a change to the MBIM values, then send an event
    if it does. This function "forces" sending an event by treating it as
    a query: it will be sent without checking for a change.

    Since this function relies on the query path, the given CID must have
    a query handler registered, and it must allow an empty request buffer.
    If these conditions are not met, a custom event forcing method should
    be created.

    @param ctx
    @param svc_id
    @param cid
*/
/*=========================================================================*/
void qbi_svc_force_event
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       cid
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_2("Forcing event for svc_id %d cid %d", svc_id, cid);
  qbi_svc_dispatch(qbi_txn_alloc_event(ctx, svc_id, cid));
} /* qbi_svc_force_event() */

/*===========================================================================
  FUNCTION: qbi_svc_get_cid_list
===========================================================================*/
/*!
    @brief Populates an array with all the CIDs supported by the given
    device service

    @details

    @param svc_id
    @param cid_array
    @param cid_array_size Size of the array, as in the number of elements
    it can hold, i.e. NOT the size in bytes
*/
/*=========================================================================*/
void qbi_svc_get_cid_list
(
  qbi_svc_id_e svc_id,
  uint32      *cid_array,
  uint32       cid_array_size
)
{
  uint32 cid_offset;
  uint32 array_offset;
  const qbi_svc_cfg_s *svc_cfg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(cid_array);

  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
  if (svc_cfg != NULL)
  {
    for (cid_offset = 0, array_offset = 0;
          cid_offset < svc_cfg->num_tbl_entries &&
          array_offset < cid_array_size; cid_offset++)
    {
      if (svc_cfg->dispatch_tbl[cid_offset].query_fcn != NULL ||
          svc_cfg->dispatch_tbl[cid_offset].set_fcn != NULL)
      {
        cid_array[array_offset++] = cid_offset + QBI_SVC_CID_OFFSET;
      }
    }
  }
} /* qbi_svc_get_cid_list() */

/*===========================================================================
  FUNCTION: qbi_svc_get_next_svc_id
===========================================================================*/
/*!
    @brief Facilitates iteration through the supported device service IDs

    @details
    To get the first supported device service, input QBI_SVC_ID_UNKNOWN.

    @param svc_id

    @return qbi_svc_id_e Next supported device service ID, or
    QBI_SVC_ID_UNKNOWN if none remaining
*/
/*=========================================================================*/
qbi_svc_id_e qbi_svc_get_next_svc_id
(
  qbi_svc_id_e svc_id
)
{
/*-------------------------------------------------------------------------*/
  do
  {
    svc_id++;
  } while (qbi_svc_id_is_valid(svc_id) &&
           qbi_svc_cfg[QBI_SVC_ID_TO_INDEX(svc_id)] == NULL);

  if (svc_id >= QBI_SVC_ID_MAX)
  {
    svc_id = QBI_SVC_ID_UNKNOWN;
  }

  return svc_id;
} /* qbi_svc_get_next_svc_id() */

/*===========================================================================
  FUNCTION: qbi_svc_handle_qmi_ind
===========================================================================*/
/*!
    @brief Processes an incoming QMI indication

    @details
    Searches for and invokes all callbacks registered with this QMI message.

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id
    @param qmi_ind_buf
*/
/*=========================================================================*/
void qbi_svc_handle_qmi_ind
(
  qbi_ctx_s                  *ctx,
  qbi_qmi_svc_e               qmi_svc_id,
  uint16                      qmi_msg_id,
  const qbi_util_buf_const_s *qmi_ind_buf
)
{
  qbi_svc_qmi_ind_data_s ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(qmi_ind_buf);

  QBI_MEMSET(&ind, 0, sizeof(ind));
  ind.qmi_svc_id = qmi_svc_id;
  ind.qmi_msg_id = qmi_msg_id;
  ind.buf        = qmi_ind_buf;

  /* Always invoke static indication handlers first */
  qbi_svc_qmi_ind_dispatch_static(ctx, &ind);
  qbi_svc_qmi_ind_dispatch_dynamic(ctx, &ind);
} /* qbi_svc_handle_qmi_ind() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_dereg
===========================================================================*/
/*!
    @brief Deregisters a dynamic QMI indication handler

    @details

    @param ctx
    @param svc_id
    @param cid
    @param qmi_svc_id
    @param qmi_msg_id
    @param cb
    @param txn
    @param cb_data

    @return boolean TRUE if at least one indication handler was
    deregistered, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_ind_dereg
(
  qbi_ctx_s            *ctx,
  qbi_svc_id_e          svc_id,
  uint32                cid,
  qbi_qmi_svc_e         qmi_svc_id,
  uint16                qmi_msg_id,
  qbi_svc_qmi_ind_cb_f *cb,
  const qbi_txn_s      *txn,
  const void           *cb_data
)
{
  qbi_util_list_iter_s iter;
  qbi_svc_ind_info_dynamic_s *dyn_ind_info;
  boolean removed = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx->svc_state);

  qbi_util_list_iter_init(&ctx->svc_state->inds_dynamic, &iter);
  while (qbi_util_list_iter_has_next(&iter))
  {
    dyn_ind_info = (qbi_svc_ind_info_dynamic_s *)
      qbi_util_list_iter_next(&iter);
    if (dyn_ind_info != NULL &&
        qbi_svc_ind_info_dynamic_is_equal(
          dyn_ind_info, svc_id, cid, qmi_svc_id,
          QBI_SVC_QMI_SVC_ID_RANGE_INVALID, qmi_msg_id, cb, txn, cb_data))
    {
      (void) qbi_util_list_iter_remove(&iter);
      QBI_MEM_FREE(dyn_ind_info);
      removed = TRUE;
    }
  }

  return removed;
} /* qbi_svc_ind_dereg() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_dereg_svc
===========================================================================*/
/*!
    @brief De-register all indication handlers associated with the given
    device service, including static indication handlers

    @details

    @param ctx
    @param svc_id
*/
/*=========================================================================*/
void qbi_svc_ind_dereg_svc
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id
)
{
  qbi_util_list_iter_s iter;
  qbi_svc_ind_info_dynamic_s *dyn_ind_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(ctx->svc_state);

  if (!qbi_svc_id_is_valid(svc_id))
  {
    QBI_LOG_E_1("Tried to deregister indications for invalid svc_id %d",
                svc_id);
  }
  else
  {
    /* Deregister all dynamic indication handlers with matching service ID */
    qbi_util_list_iter_init(&ctx->svc_state->inds_dynamic, &iter);
    while (qbi_util_list_iter_has_next(&iter))
    {
      dyn_ind_info = (qbi_svc_ind_info_dynamic_s *)
        qbi_util_list_iter_next(&iter);
      if (dyn_ind_info != NULL && dyn_ind_info->svc_id == svc_id)
      {
        (void) qbi_util_list_iter_remove(&iter);
        QBI_MEM_FREE(dyn_ind_info);
      }
    }

    /* Deregister static indication handlers */
    ctx->svc_state->inds_static[QBI_SVC_ID_TO_INDEX(svc_id)] = NULL;
    ctx->svc_state->num_static_ind_hdlrs[QBI_SVC_ID_TO_INDEX(svc_id)] = 0;
  }
} /* qbi_svc_ind_dereg_svc() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_dereg_txn
===========================================================================*/
/*!
    @brief Deregisters any dynamic indication handler matching the given
    transaction

    @details
    This is called when releasing a transaction to ensure that no indication
    handlers remain with a reference to the transaction that is about to be
    freed.

    @param txn
*/
/*=========================================================================*/
void qbi_svc_ind_dereg_txn
(
  const qbi_txn_s *txn
)
{
  qbi_util_list_iter_s iter;
  qbi_svc_ind_info_dynamic_s *dyn_ind_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->ctx);
  QBI_CHECK_NULL_PTR_RET(txn->ctx->svc_state);

  qbi_util_list_iter_init(&txn->ctx->svc_state->inds_dynamic, &iter);
  while (qbi_util_list_iter_has_next(&iter))
  {
    dyn_ind_info = (qbi_svc_ind_info_dynamic_s *)
      qbi_util_list_iter_next(&iter);
    if (dyn_ind_info != NULL && dyn_ind_info->txn == txn)
    {
      (void) qbi_util_list_iter_remove(&iter);
      QBI_MEM_FREE(dyn_ind_info);
    }
  }
} /* qbi_svc_ind_dereg_txn() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_ref_cnt_dec
===========================================================================*/
/*!
    @brief Decrements a reference counter meant to track users of the QMI
    indication with the given service and message ID

    @details

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id

    @return uint16 New reference count value

    @see qbi_svc_ind_qmi_ref_cnt_inc, qbi_svc_ind_qmi_ref_cnt_get
*/
/*=========================================================================*/
uint16 qbi_svc_ind_qmi_ref_cnt_dec
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id
)
{
  uint16 ref_cnt = 0;
/*-------------------------------------------------------------------------*/
  if (!qbi_svc_ind_qmi_ref_cnt_update(
        ctx, qmi_svc_id, qmi_msg_id, FALSE, &ref_cnt))
  {
    QBI_LOG_E_2("Tried to decrement reference count for qmi_svc_id %d "
                "qmi_msg_id 0x%02x, but no entry found!", qmi_svc_id,
                qmi_msg_id);
  }

  return ref_cnt;
} /* qbi_svc_ind_qmi_ref_cnt_dec() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_ref_cnt_get
===========================================================================*/
/*!
    @brief Retrieves the current reference count value for registration of a
    QMI indication identified by its service and message ID

    @details

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id

    @return uint16 Current reference count

    @see qbi_svc_ind_qmi_ref_cnt_inc, qbi_svc_ind_qmi_ref_cnt_dec
*/
/*=========================================================================*/
uint16 qbi_svc_ind_qmi_ref_cnt_get
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id
)
{
  uint16 ref_cnt = 0;
  qbi_util_list_iter_s iter;
  qbi_svc_ind_qmi_ref_cnt_entry_s *entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx->svc_state);

  qbi_util_list_iter_init(&ctx->svc_state->qmi_ind_ref_cnts, &iter);
  while ((entry = qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (entry->qmi_svc_id == qmi_svc_id && entry->qmi_msg_id == qmi_msg_id)
    {
      ref_cnt = entry->ref_cnt;
      break;
    }
  }

  return ref_cnt;
} /* qbi_svc_ind_qmi_ref_cnt_get() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_ref_cnt_inc
===========================================================================*/
/*!
    @brief Increments a reference counter meant to track users of the QMI
    indication with the given service and message ID

    @details
    The reference count is intended to be used in QMI (de)registration
    functions to arbitrate sharing of the QMI registration between multiple
    uses. For example, the same QMI indication may be used in 2 separate
    CID events, and also in a CID set operation. Filtering one of the CID
    events should not affect the other, and neither should impact the set.

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id

    @return uint16 New reference count value

    @see qbi_svc_ind_qmi_ref_cnt_dec, qbi_svc_ind_qmi_ref_cnt_get
*/
/*=========================================================================*/
uint16 qbi_svc_ind_qmi_ref_cnt_inc
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id
)
{
  uint16 ref_cnt = 0;
  qbi_svc_ind_qmi_ref_cnt_entry_s *ref_cnt_entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(ctx->svc_state);

  if (!qbi_svc_ind_qmi_ref_cnt_update(
        ctx, qmi_svc_id, qmi_msg_id, TRUE, &ref_cnt))
  {
    /* No existing entry for this QMI message, so add a new one */
    ref_cnt_entry = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_ind_qmi_ref_cnt_entry_s));
    QBI_CHECK_NULL_PTR_RET_ZERO(ref_cnt_entry);

    ref_cnt_entry->qmi_msg_id = qmi_msg_id;
    ref_cnt_entry->qmi_svc_id = qmi_svc_id;
    ref_cnt_entry->ref_cnt    = 1;
    if (!qbi_util_list_push_back_aliased(
          &ctx->svc_state->qmi_ind_ref_cnts, &ref_cnt_entry->entry))
    {
      QBI_LOG_E_2("Couldn't add reference count for qmi_svc_id %d qmi_msg_id "
                  "0x%02x to list!", qmi_svc_id, qmi_msg_id);
      QBI_MEM_FREE(ref_cnt_entry);
    }
    else
    {
      QBI_LOG_D_2("Initialized reference count for qmi_svc_id %d qmi_msg_id "
                  "0x%02x", qmi_svc_id, qmi_msg_id);
      ref_cnt = ref_cnt_entry->ref_cnt;
    }
  }

  return ref_cnt;
} /* qbi_svc_ind_qmi_ref_cnt_inc() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_ref_cnt_update_then_check_if_significant
===========================================================================*/
/*!
    @brief Increments/decrements the reference count, and returns TRUE if
    that update was a significant change

    @details
    If increment is TRUE, calls qbi_svc_ind_qmi_ref_cnt_inc and returns TRUE
    if the reference count changed to 1. If increment is FALSE, calls
    qbi_svc_ind_qmi_ref_cnt_dec and returns TRUE if the reference count
    changed to 0.

    @param ctx
    @param qmi_svc_id
    @param qmi_msg_id
    @param increment Set to TRUE to increment, FALSE to decrement (see
    details section)

    @return boolean TRUE if reference count change was significant (see
    details section)
*/
/*=========================================================================*/
boolean qbi_svc_ind_qmi_ref_cnt_update_then_check_if_significant
(
  qbi_ctx_s    *ctx,
  qbi_qmi_svc_e qmi_svc_id,
  uint16        qmi_msg_id,
  boolean       increment
)
{
  boolean update_is_significant = FALSE;
/*-------------------------------------------------------------------------*/
  if ((increment == TRUE &&
       qbi_svc_ind_qmi_ref_cnt_inc(ctx, qmi_svc_id, qmi_msg_id) == 1) ||
      (increment == FALSE &&
       qbi_svc_ind_qmi_ref_cnt_dec(ctx, qmi_svc_id, qmi_msg_id) == 0))
  {
    update_is_significant = TRUE;
  }

  return update_is_significant;
} /* qbi_svc_ind_qmi_ref_cnt_update_then_check_if_significant() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_qmi_reg_invoke
===========================================================================*/
/*!
    @brief Invokes the registered (optional) function to perform QMI
    indication (de)registration for the given CID event

    @details
    Called when the event filter for the given CID changes status. Assuming
    the underlying registration function uses the reference count framework,
    this is also an effective way for a set operation to ensure that
    required QMI indications are registered: call this function with
    event_enabled set to TRUE when starting processing the request, and
    ensure that the transaction's completion callback will call this
    function with event_enabled set to FALSE.

    QMI indication (de)registration functions are registered via qbi_svc_reg.
    These functions should be used to ensure that QMI indications will be
    disabled when the relevant CID events are filtered. When implementing a
    qmi_ind_reg function, keep in mind all potential uses of the indication,
    such as CID set operations and other CID events that share the
    indication. The reference count framework exists to help manage these
    cases. Also, when an indication is enabled, be aware that any related
    cached data should be refreshed. This should be done strictly after the
    indication registration is completed to avoid race conditions.

    @param ctx
    @param svc_id
    @param cid
    @param enable_inds Will be passed to the underlying registration
    function to indicate whether to enable or disable indications
    @param listener_txn Optional QBI transaction configured as a listener
    to be notified of the asynchronous result of QMI indication
    registration

    @return boolean TRUE on success, FALSE on failure, including early
    failure of the QMI registration function
*/
/*=========================================================================*/
boolean qbi_svc_ind_qmi_reg_invoke
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       cid,
  boolean      enable_inds,
  qbi_txn_s   *listener_txn
)
{
  uint32 i;
  qbi_txn_s *txn;
  boolean success = FALSE;
  boolean txn_freed;
  qbi_svc_action_e action;
  const qbi_svc_cfg_s *svc_cfg;
/*-------------------------------------------------------------------------*/
  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
  if (svc_cfg == NULL)
  {
    QBI_LOG_E_1("Couldn't look up service config for svc_id %d", svc_id);
  }
  else if (!qbi_svc_cid_is_valid(svc_cfg, cid))
  {
    QBI_LOG_E_2("Invalid CID %d for svc_id %d", cid, svc_id);
  }
  else
  {
    success = TRUE;
    for (i = 0; i < svc_cfg->num_qmi_reg_tbl_entries; i++)
    {
      if (svc_cfg->qmi_reg_tbl[i].cid == cid &&
          svc_cfg->qmi_reg_tbl[i].reg_cb != NULL)
      {
        txn = qbi_txn_alloc(
          ctx, svc_id, 0, QBI_TXN_CMD_TYPE_QMI_REG, cid, 0, NULL);
        QBI_CHECK_NULL_PTR_RET_FALSE(txn);
        qbi_txn_set_timeout(txn, QBI_SVC_QMI_REG_TIMEOUT_MS, NULL);

        action = svc_cfg->qmi_reg_tbl[i].reg_cb(txn, enable_inds);
        success = (action == QBI_SVC_ACTION_ABORT) ? FALSE : TRUE;
        txn_freed = qbi_svc_proc_action(txn, action);

        /* If txn wasn't freed, then it completes asynchronously, so the
           listener can be used */
        if (!txn_freed && listener_txn != NULL)
        {
          success = qbi_txn_notify_setup_notifier(listener_txn, txn);
        }
        break;
      }
    }
  }

  return success;
} /* qbi_svc_ind_qmi_reg_invoke() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_reg_dynamic
===========================================================================*/
/*!
    @brief Register a handler function with a QMI indication using dynamic
    memory

    @details
    The parameters will be copied to a qbi_svc_ind_info_s that has been
    dynamically allocated. Device services should use this function to
    register for an indication that shouldresult in sending the response
    to a host request, or for events that have a limited period of validity.
    Note that the device service is responsible for taking the proper steps
    to ensure that qbi_qmi will receive the indication in the first place,
    e.g. allocating a QMI service handle and registering for the indication
    as needed.

    Static indication handlers are always invoked before dynamic indication
    handlers.

    @param ctx
    @param svc_id
    @param cid MBIM CID associated with the QMI indication, or
    QBI_SVC_CID_NONE if not associated with one
    @param qmi_svc_id
    @param qmi_msg_id
    @param cb Indication handler function to invoke when the QMI indication
    is received
    @param txn Transaction to pass to the indication handler, or NULL if a
    new transaction should be allocated (pre-populated as an event)
    @param cb_data Arbitrary user data to pass to the indication handler

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_ind_reg_dynamic
(
  qbi_ctx_s            *ctx,
  qbi_svc_id_e          svc_id,
  uint32                cid,
  qbi_qmi_svc_e         qmi_svc_id,
  uint16                qmi_msg_id,
  qbi_svc_qmi_ind_cb_f *cb,
  qbi_txn_s            *txn,
  void                 *cb_data
)
{
/*-------------------------------------------------------------------------*/
  return qbi_svc_ind_reg_dynamic_qmi_svc_id_range(
    ctx, svc_id, cid, qmi_svc_id, QBI_SVC_QMI_SVC_ID_RANGE_INVALID, qmi_msg_id,
    cb, txn, cb_data);
} /* qbi_svc_ind_reg_dynamic() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_reg_dynamic_qmi_svc_id_range
===========================================================================*/
/*!
    @brief Register a dynamic indication handler that will match a QMI
    message ID from a range of QMI service IDs

    @details
    Intended for use with a single QMI service that spans multiple QBI
    enum values, e.g. QMI WDS which QBI allocates multiple clients, to
    avoid the need for a single dynamic indication handler per client.
    Currently there is no way to deregister this type of indication handler
    manually, i.e. qbi_svc_ind_dereg only supports single QMI service ID
    indication handlers.

    @param ctx
    @param svc_id
    @param cid
    @param qmi_svc_id_range_start
    @param qmi_svc_id_range_end
    @param qmi_msg_id
    @param cb
    @param txn
    @param cb_data

    @return boolean

    @see qbi_svc_ind_reg_dynamic
*/
/*=========================================================================*/
boolean qbi_svc_ind_reg_dynamic_qmi_svc_id_range
(
  qbi_ctx_s            *ctx,
  qbi_svc_id_e          svc_id,
  uint32                cid,
  qbi_qmi_svc_e         qmi_svc_id_range_start,
  qbi_qmi_svc_e         qmi_svc_id_range_end,
  uint16                qmi_msg_id,
  qbi_svc_qmi_ind_cb_f *cb,
  qbi_txn_s            *txn,
  void                 *cb_data
)
{
  qbi_svc_ind_info_dynamic_s *dyn_ind_info;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx->svc_state);

  dyn_ind_info = (qbi_svc_ind_info_dynamic_s *) QBI_MEM_MALLOC_CLEAR(
    sizeof(qbi_svc_ind_info_dynamic_s));
  if (dyn_ind_info == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate memory for indication info");
  }
  else
  {
    dyn_ind_info->svc_id = svc_id;
    dyn_ind_info->txn    = txn;

    dyn_ind_info->ind_info.cid        = cid;
    dyn_ind_info->ind_info.qmi_svc_id = qmi_svc_id_range_start;
    dyn_ind_info->ind_info.qmi_msg_id = qmi_msg_id;
    dyn_ind_info->ind_info.cb_data    = cb_data;

    qbi_os_fptr_encode(&dyn_ind_info->encoded_cb, (qbi_os_void_f *) cb);

    if (qmi_svc_id_range_end == QBI_SVC_QMI_SVC_ID_RANGE_INVALID)
    {
      dyn_ind_info->qmi_svc_id_range_start = QBI_SVC_QMI_SVC_ID_RANGE_INVALID;
      dyn_ind_info->qmi_svc_id_range_end   = QBI_SVC_QMI_SVC_ID_RANGE_INVALID;
    }
    else
    {
      dyn_ind_info->qmi_svc_id_range_start = qmi_svc_id_range_start;
      dyn_ind_info->qmi_svc_id_range_end   = qmi_svc_id_range_end;
    }

    if (qmi_svc_id_range_end != QBI_SVC_QMI_SVC_ID_RANGE_INVALID &&
        (qmi_svc_id_range_start > qmi_svc_id_range_end ||
         qmi_svc_id_range_end >= QBI_QMI_NUM_SVCS))
    {
      QBI_LOG_E_2("Invalid QMI service ID range %d-%d",
                  qmi_svc_id_range_start, qmi_svc_id_range_end);
    }
    else if (qbi_svc_sanity_check_ind_info(
               &dyn_ind_info->ind_info, svc_id, TRUE))
    {
      if (!qbi_util_list_push_back_aliased(
            &ctx->svc_state->inds_dynamic, &dyn_ind_info->list_entry))
      {
        QBI_LOG_E_0("Couldn't add indication info onto context");
      }
      else
      {
        success = TRUE;
      }
    }
  }

  if (!success && dyn_ind_info != NULL)
  {
    QBI_MEM_FREE(dyn_ind_info);
  }

  return success;
} /* qbi_svc_ind_reg_dynamic_qmi_svc_id_range() */

/*===========================================================================
  FUNCTION: qbi_svc_ind_reg_static
===========================================================================*/
/*!
    @brief Registers an array of static indication handlers

    @details
    This version does not make a copy of the indication information data,
    so the calling function must ensure that it is not freed before the
    indication(s) are unregistered. The intent of this function is to
    register the constant indication handlers @ device service open, where
    qbi_svc_ind_reg is for dynamic registration. Also, static registration
    only toggles the pointer to an array of indication handlers, therefore
    calling this function twice for the same QBI service switches the list
    of static indication handlers rather than adding to them.

    Static indication handlers are always invoked before dynamic indication
    handlers.

    @param ctx
    @param svc_id
    @param ind_info_array Array of indication handlers
    @param num_entries Number of indication handlers in the array

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_svc_ind_reg_static
(
  qbi_ctx_s                *ctx,
  qbi_svc_id_e              svc_id,
  const qbi_svc_ind_info_s *ind_info_array,
  uint32                    num_entries
)
{
  uint32 i;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(ctx->svc_state);
  QBI_CHECK_NULL_PTR_RET_FALSE(ind_info_array);

  if (!qbi_svc_id_is_valid(svc_id))
  {
    QBI_LOG_E_1("Tried to register static indications for invalid svc_id %d",
                svc_id);
    success = FALSE;
  }
  else
  {
    QBI_LOG_I_2("Registering %d static QMI indication handlers for svc_id %d",
                num_entries, svc_id);

    for (i = 0; i < num_entries; i++)
    {
      if (!qbi_svc_sanity_check_ind_info(&ind_info_array[i], svc_id, FALSE))
      {
        QBI_LOG_E_1("Invalid indication handler detected at index %d!", i);
        success = FALSE;
        break;
      }
    }

    if (success)
    {
      ctx->svc_state->num_static_ind_hdlrs[QBI_SVC_ID_TO_INDEX(svc_id)] =
        num_entries;
      ctx->svc_state->inds_static[QBI_SVC_ID_TO_INDEX(svc_id)] =
        ind_info_array;
    }
  }

  return success;
} /* qbi_svc_ind_reg_static() */

/*===========================================================================
  FUNCTION: qbi_svc_init
===========================================================================*/
/*!
    @brief Perform startup initialization of QBI device services

    @details
*/
/*=========================================================================*/
void qbi_svc_init
(
  void
)
{
/*-------------------------------------------------------------------------*/
  /* Device services defined in MBIM */
  qbi_svc_bc_init();
  /*! @note BC has a dependency on PB - see the note in qbi_svc_pb.c before
      removing PB */
  qbi_svc_pb_init();
  qbi_svc_sms_init();
  qbi_svc_stk_init();
  qbi_svc_ussd_init();
  qbi_svc_auth_init();

  /* Custom "extensibility" device services */
  qbi_svc_atds_init();

  /* Device services defined by Microsoft, but not included in MBIM */
  qbi_svc_msfwid_init();
  qbi_svc_mshsd_init();
  qbi_svc_msuicc_init();
  qbi_svc_bc_ext_init();
  qbi_svc_mssar_init();

  /* Any device services with a platform-specific component or device services
     that may not be supported on all platforms should be initialized in the
     platform-specific qbi_task_init file. */
} /* qbi_svc_init() */

/*===========================================================================
  FUNCTION: qbi_svc_internal_query
===========================================================================*/
/*!
    @brief Allocates an internal transaction and passes it to the registered
    query handler for the given CID

    @details
    Used as a helper function to simplify performing an internal CID query
    for the purposes of updating the cache, etc. Note that the request
    InformationBuffer will always be empty when this function is used, so
    it can't be used for CIDs that require data in the request. Instead,
    manually allocate a transaction with QBI_TXN_CMD_TYPE_INTERNAL, populate
    the request, pass it to the relevant query handler, then call
    qbi_svc_proc_action() directly.

    @param ctx
    @param svc_id
    @param cid
*/
/*=========================================================================*/
void qbi_svc_internal_query
(
  qbi_ctx_s   *ctx,
  qbi_svc_id_e svc_id,
  uint32       cid
)
{
  qbi_txn_s *txn;
  const qbi_svc_cfg_s *svc_cfg;
  const qbi_svc_cmd_hdlr_tbl_entry_s *cmd_hdlr;
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_2("Performing internal query of svc_id %d cid %d", svc_id, cid);

  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
  if (svc_cfg == NULL)
  {
    QBI_LOG_E_1("Couldn't find device service ID %d", svc_id);
  }
  else if (!qbi_svc_cid_is_valid(svc_cfg, cid))
  {
    QBI_LOG_E_2("CID %d out of supported range of service ID %d", cid, svc_id);
  }
  else
  {
    cmd_hdlr = &svc_cfg->dispatch_tbl[cid - QBI_SVC_CID_OFFSET];
    if (cmd_hdlr->query_fcn == NULL)
    {
      QBI_LOG_E_0("Tried internal query, but no query handler registered!");
    }
    else if (cmd_hdlr->query_infobuf_min_size != 0)
    {
      QBI_LOG_E_1("Query InformationBuffer minimum size is %d - internal query "
                  "framework only supports empty requests",
                  cmd_hdlr->query_infobuf_min_size);
    }
    else
    {
      txn = qbi_txn_alloc(
        ctx, svc_id, 0, QBI_TXN_CMD_TYPE_INTERNAL, cid, 0, NULL);
      if (txn != NULL)
      {
        (void) qbi_svc_proc_action(txn, cmd_hdlr->query_fcn(txn));
      }
    }
  }
} /* qbi_svc_internal_query() */

/*===========================================================================
  FUNCTION: qbi_svc_num_cids
===========================================================================*/
/*!
    @brief Returns the number of CIDs supported by the given device service
    ID

    @details

    @param svc_id

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_num_cids
(
  qbi_svc_id_e svc_id
)
{
  uint32 count = 0;
  uint32 cid_offset;
  const qbi_svc_cfg_s *svc_cfg;
/*-------------------------------------------------------------------------*/
  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
  if (svc_cfg != NULL)
  {
    for (cid_offset = 0; cid_offset < svc_cfg->num_tbl_entries; cid_offset++)
    {
      if (svc_cfg->dispatch_tbl[cid_offset].query_fcn != NULL ||
          svc_cfg->dispatch_tbl[cid_offset].set_fcn != NULL)
      {
        count++;
      }
    }
  }

  return count;
} /* qbi_svc_num_cids() */

/*===========================================================================
  FUNCTION: qbi_svc_num_registered_svcs
===========================================================================*/
/*!
    @brief Returns the number of MBIM device services that are registered,
    i.e. supported in the current configuration

    @details

    @return uint32
*/
/*=========================================================================*/
uint32 qbi_svc_num_registered_svcs
(
  void
)
{
  qbi_svc_id_e svc_id;
  uint32 count = 0;
/*-------------------------------------------------------------------------*/
  for (svc_id = QBI_SVC_ID_OFFSET; svc_id < QBI_SVC_ID_MAX; svc_id++)
  {
    if (qbi_svc_id_is_valid(svc_id) &&
        qbi_svc_cfg[QBI_SVC_ID_TO_INDEX(svc_id)] != NULL)
    {
      count++;
    }
  }

  return count;
} /* qbi_svc_num_registered_svcs() */

/*===========================================================================
  FUNCTION: qbi_svc_open
===========================================================================*/
/*!
    @brief Performs context-wide device service initialization

    @details
    Called when the device is opened via MBIM_OPEN_MSG. Note that a return
    value of TRUE only means that the open operation started; qbi_svc will
    invoke qbi_msg_send_open_rsp with the result status code when the open
    operation is complete.

    @param txn

    @return boolean TRUE on successful dispatch (response will be async),
    FALSE on early failure.

    @see qbi_svc_close
*/
/*=========================================================================*/
boolean qbi_svc_open
(
  qbi_txn_s *txn
)
{
  boolean success = FALSE;
  boolean no_more_svcs;
  boolean txn_freed;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->ctx);

  if (txn->ctx->svc_state != NULL)
  {
    /* The most likely cause for this is that we are in the middle of processing
       a MBIM_OPEN_MSG, and the host sent us another MBIM_OPEN_MSG. */
    QBI_LOG_E_0("Service state not null!");
    txn->status = QBI_MBIM_STATUS_BUSY;
  }
  else
  {
    txn->ctx->svc_state = (qbi_svc_state_s *) QBI_MEM_MALLOC_CLEAR(
      sizeof(qbi_svc_state_s));
    if (txn->ctx->svc_state == NULL)
    {
      QBI_LOG_E_0("Couldn't allocate memory for service state");
    }
    else if (!qbi_svc_event_filter_init(txn->ctx->svc_state))
    {
      QBI_LOG_E_0("Couldn't initialize event filter!");
      QBI_MEM_FREE(txn->ctx->svc_state);
      txn->ctx->svc_state = NULL;
    }
    else
    {
      QBI_LOG_I_0("Opening device services");
      txn_freed = qbi_svc_open_next_dev_svc(txn, &no_more_svcs);
      QBI_LOG_I_2("Open operation started; may complete asynchronously: "
                  "txn_freed %d no_more_svcs %d", txn_freed, no_more_svcs);
      success = TRUE;
    }
  }

  return success;
} /* qbi_svc_open() */

/*===========================================================================
  FUNCTION: qbi_svc_proc_action
===========================================================================*/
/*!
    @brief Processes an action for a transaction

    @details
    Depending on the action, all memory for the transaction may be
    released, for example if the transaction is complete and its response
    is sent to the host. In this case, this function will return TRUE, and
    the calling function must ensure that it does not reference the
    trasaction or any of its child QMI transactions.

    @param txn
    @param status

    @see qbi_svc_action_e

    @return boolean TRUE if the transaction was freed, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_svc_proc_action
(
  qbi_txn_s       *txn,
  qbi_svc_action_e action
)
{
  boolean txn_freed = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  if (txn->cmd_type == QBI_TXN_CMD_TYPE_EVENT &&
      action == QBI_SVC_ACTION_SEND_RSP &&
      !qbi_svc_event_filter_check(txn))
  {
    action = QBI_SVC_ACTION_ABORT;
  }

  QBI_LOG_D_2("Processing action %d for txn iid %d", action, txn->iid);
  switch (action)
  {
    case QBI_SVC_ACTION_SEND_QMI_REQ:
      if (!qbi_qmi_dispatch(txn))
      {
        QBI_LOG_E_0("Aborting transaction due to QMI dispatch failure");
        qbi_svc_abort_txn(txn, QBI_MBIM_STATUS_FAILURE);
      }
      else
      {
        txn_freed = FALSE;
      }
      break;

    case QBI_SVC_ACTION_WAIT_ASYNC_RSP:
      txn_freed = FALSE;
      break;

    case QBI_SVC_ACTION_SEND_RSP:
      #ifdef QBI_UNIT_TEST
      if (qbi_utf_txn_receive(txn))
      {
        return TRUE;
      }
      #endif /* QBI_UNIT_TEST */

      switch (txn->cmd_type)
      {
        case QBI_MSG_CMD_TYPE_QUERY:
        case QBI_MSG_CMD_TYPE_SET:
          qbi_msg_send_cmd_rsp(txn);
          break;

        case QBI_TXN_CMD_TYPE_EVENT:
          qbi_msg_send_event(txn);
          break;

        case QBI_TXN_CMD_TYPE_MBIM_OPEN:
          txn_freed = qbi_svc_proc_open_done(txn);
          break;

        case QBI_TXN_CMD_TYPE_INTERNAL:
        case QBI_TXN_CMD_TYPE_QMI_REG:
        default:
          qbi_svc_abort_txn(txn, txn->status);
      }
      break;

    case QBI_SVC_ACTION_ABORT:
      if (txn->status == QBI_MBIM_STATUS_SUCCESS)
      {
        txn->status = QBI_MBIM_STATUS_FAILURE;
      }
      qbi_svc_abort_txn(txn, txn->status);
      break;

    default:
      QBI_LOG_E_3("Unexpected action %d from txn iid %d (txn_id 0x%x)",
                  action, txn->iid, txn->txn_id);
      qbi_svc_abort_txn(txn, QBI_MBIM_STATUS_FAILURE);
  }

  return txn_freed;
} /* qbi_svc_proc_action() */

/*===========================================================================
  FUNCTION: qbi_svc_reg
===========================================================================*/
/*!
    @brief Register a device service with the given configuration

    @details
    The configuration is NOT deep copied - only the pointer is kept.
    Therefore, the calling function must ensure that memory containing the
    configuration is never freed.

    @param svc_cfg Pointer to service configuration
*/
/*=========================================================================*/
void qbi_svc_reg
(
  const qbi_svc_cfg_s *svc_cfg
)
{
/*-------------------------------------------------------------------------*/
  if (svc_cfg != NULL && qbi_svc_id_is_valid(svc_cfg->svc_id))
  {
    qbi_svc_cfg[svc_cfg->svc_id - QBI_SVC_ID_OFFSET] = svc_cfg;
  }
  else
  {
    QBI_LOG_E_0("Invalid service configuration");
  }
} /* qbi_svc_reg() */

/*===========================================================================
  FUNCTION: qbi_svc_svc_id_to_uuid
===========================================================================*/
/*!
    @brief Convert an internal device service ID to its 16 byte UUID

    @details

    @param id

    @return const uint8* pointer to array of QBI_MBIM_UUID_LEN bytes or NULL
    if not found
*/
/*=========================================================================*/
const uint8 * qbi_svc_svc_id_to_uuid
(
  qbi_svc_id_e svc_id
)
{
  const qbi_svc_cfg_s *svc_cfg;
  const uint8 *uuid = NULL;
/*-------------------------------------------------------------------------*/
  svc_cfg = qbi_svc_look_up_cfg_by_svc_id(svc_id);
  if (svc_cfg != NULL)
  {
    uuid = svc_cfg->uuid;
  }

  return uuid;
} /* qbi_svc_svc_id_to_uuid() */

/*===========================================================================
  FUNCTION: qbi_svc_uuid_to_svc_id
===========================================================================*/
/*!
    @brief Convert a 16 byte device service UUID into the internal ID

    @details

    @param uuid pointer to array of QBI_MBIM_UUID_LEN bytes

    @return qbi_svc_id_e
*/
/*=========================================================================*/
qbi_svc_id_e qbi_svc_uuid_to_svc_id
(
  const uint8 *uuid
)
{
  const qbi_svc_cfg_s *svc_cfg;
  qbi_svc_id_e svc_id = QBI_SVC_ID_UNKNOWN;
/*-------------------------------------------------------------------------*/
  svc_cfg = qbi_svc_look_up_cfg_by_uuid(uuid);
  if (svc_cfg != NULL)
  {
    svc_id = svc_cfg->svc_id;
  }

  return svc_id;
} /* qbi_svc_uuid_to_svc_id() */

