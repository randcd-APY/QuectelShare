/*!
  @file
  qbi_svc_pb.c

  @brief
  Phonebook device service implementation

  @note
  The Basic Connectivity device service has a dependency on this device service
  to send QMI_PBM_INDICATION_REGISTER. If this module is to be removed or
  disabled, ensure that indication registration is done in qbi_svc_bc.c. This
  registration cannot be done in BC while PB is enabled, since QMI PBM will send
  a ready indication immediately after registration if initialization completed
  before the registration, and PB depends on that indication for cache
  initialization.
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
02/25/13  bd   Update phonebook with full SIM hot swap support
02/25/13  bd   Return SIM-related status codes in failed configuration query
07/17/12  bd   Fix population of size field in nested structures of read resp
01/31/12  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_pb.h"
#include "qbi_svc_pb_mbim.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_qmi_defs.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_svc_bc.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_txn.h"
#include "qbi_util.h"

#include "phonebook_manager_service_v01.h"

/*=============================================================================

  Private Macros

=============================================================================*/

/*! This macro statically defines a QMI indication handler and fills in the
    fields that are common to all handlers in this device service */
#define QBI_SVC_PB_STATIC_IND_HDLR(qmi_svc_id, qmi_msg_id, cid, cb) \
  {qmi_svc_id, qmi_msg_id, cid, cb, NULL}

/*=============================================================================

  Private Typedefs and Constants

=============================================================================*/

/*! @brief Device service local cache struct
*/
typedef struct {
  struct {
    /* All the information that goes into the response is cached */
    qbi_svc_pb_configuration_info_q_rsp_s mbim;

    struct {
      /*! UICC session type that contains the phonebook we will use */
      pbm_session_type_enum_v01 session;

      /*! Set to TRUE when the session field is populated (does not mean the
          phonebook on the given session is ready though) */
      boolean session_valid;

      /*! Set to TRUE if the phonebook subsystem has initialized but no ADN
          was found */
      boolean no_adn_available;
    } qmi;
  } configuration;
} qbi_svc_pb_cache_s;

/*! Tracking information for a read request */
typedef struct {
  /*! Number of records that have been read into the response buffer so far */
  uint32 records_read;

  /*! Total number of records we are expecting to receive */
  uint32 records_total;
} qbi_svc_pb_read_info_s;

/*! The first phonebook session we try to use
    @see qbi_svc_pb_configuration_session_next() */
#define QBI_SVC_PB_SESSION_PRIMARY (PBM_SESSION_TYPE_GLOBAL_PB_SLOT1_V01)

/*! Phonebook type accessed by this device service */
#define QBI_SVC_PB_PHONEBOOK_TYPE (PBM_PB_TYPE_ADN_V01)

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_pb_cache_s *qbi_svc_pb_cache_get
(
  const qbi_ctx_s *ctx
);

static boolean qbi_svc_pb_check_state
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_pb_open
(
  qbi_txn_s *txn
);

static void qbi_svc_pb_qmi_error_to_mbim_status
(
  qbi_txn_s         *txn,
  qmi_error_type_v01 qmi_error
);

static qbi_svc_action_e qbi_svc_pb_reg_inds_pbm01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_pb_reg_inds_with_qmi
(
  qbi_txn_s *txn
);

static boolean qbi_svc_pb_sanity_check_read_or_delete_s_req
(
  qbi_txn_s *txn,
  uint32     flag,
  uint32     index
);

/*! @addtogroup MBIM_CID_PHONEBOOK_CONFIGURATION
    @{ */

static qbi_svc_action_e qbi_svc_pb_configuration_e_pbm09_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_pb_configuration_e_pbm0d_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_pb_configuration_e_pbm11_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_pb_configuration_eq_build_pbm11_req
(
  qbi_txn_s                *txn,
  pbm_session_type_enum_v01 session_type,
  qbi_svc_qmi_rsp_cb_f     *rsp_cb
);

static qbi_svc_action_e qbi_svc_pb_configuration_eq_build_rsp_from_cache
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_pb_configuration_q_pbm02_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_pb_configuration_q_pbm11_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_pb_configuration_q_req
(
  qbi_txn_s *txn
);

static qbi_mbim_status_e qbi_svc_pb_configuration_qmi_pb_state_to_mbim_status
(
  pbm_pb_state_enum_v01 qmi_pb_state
);

static pbm_session_type_enum_v01 qbi_svc_pb_configuration_session_get
(
  qbi_ctx_s *ctx
);

static boolean qbi_svc_pb_configuration_session_next
(
  qbi_ctx_s                 *ctx,
  pbm_session_type_enum_v01  session_prev,
  pbm_session_type_enum_v01 *session_next
);

static boolean qbi_svc_pb_configuration_update_cache_pbm02
(
  const qbi_ctx_s                          *ctx,
  const pbm_capability_basic_info_type_v01 *pb_info
);

/*! @} */

/*! @addtogroup MBIM_CID_PHONEBOOK_READ
    { */

static qbi_svc_action_e qbi_svc_pb_read_q_add_record
(
  qbi_txn_s                          *txn,
  const pbm_record_instance_type_v01 *record
);

static qbi_svc_action_e qbi_svc_pb_read_q_pbm04_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_pb_read_q_pbm04_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_pb_read_q_prepare_for_inds
(
  qbi_txn_s *txn,
  uint16     element_count
);

static qbi_svc_action_e qbi_svc_pb_read_q_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_PHONEBOOK_DELETE
    @{ */

static qbi_svc_action_e qbi_svc_pb_delete_s_pbm06_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_pb_delete_s_pbm07_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_pb_delete_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_PHONEBOOK_WRITE
    @{ */

static qbi_svc_action_e qbi_svc_pb_write_s_pbm05_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_pb_write_s_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_pb_write_s_sanity_check_req
(
  qbi_txn_s *txn
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_pb_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_pb_cmd_hdlr_tbl[] = {
  /* MBIM_CID_PHONEBOOK_CONFIGURATION */
  {qbi_svc_pb_configuration_q_req, 0, NULL, 0},
  /* MBIM_CID_PHONEBOOK_READ */
  {qbi_svc_pb_read_q_req, sizeof(qbi_svc_pb_read_q_req_s), NULL, 0},
  /* MBIM_CID_PHONEBOOK_DELETE */
  {NULL, 0, qbi_svc_pb_delete_s_req, sizeof(qbi_svc_pb_delete_s_req_s)},
  /* MBIM_CID_PHONEBOOK_WRITE */
  {NULL, 0, qbi_svc_pb_write_s_req, sizeof(qbi_svc_pb_write_s_req_s)}
};

/*! @brief Static QMI indication handlers (generally, CID event handlers)
*/
static const qbi_svc_ind_info_s qbi_svc_pb_static_ind_hdlrs[] = {
  QBI_SVC_PB_STATIC_IND_HDLR(QBI_QMI_SVC_PBM, QMI_PBM_RECORD_UPDATE_IND_V01,
                             QBI_SVC_PB_MBIM_CID_PHONEBOOK_CONFIGURATION,
                             qbi_svc_pb_configuration_e_pbm09_ind_cb),
  QBI_SVC_PB_STATIC_IND_HDLR(QBI_QMI_SVC_PBM, QMI_PBM_ALL_PB_INIT_DONE_IND_V01,
                             QBI_SVC_PB_MBIM_CID_PHONEBOOK_CONFIGURATION,
                             qbi_svc_pb_configuration_e_pbm0d_ind_cb)
};


/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_pb_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the Phonebook device service's cache

    @details

    @param ctx

    @return qbi_svc_pb_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
static qbi_svc_pb_cache_s *qbi_svc_pb_cache_get
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_pb_cache_s *) qbi_svc_cache_get(ctx, QBI_SVC_ID_PB);
} /* qbi_svc_pb_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_check_state
===========================================================================*/
/*!
    @brief Checks the cache to see whether the phonebook is ready for use

    @details
    Sets the appropriate MBIM status code in txn->status if the phonebook
    is not ready

    @param txn

    @return boolean TRUE if phonebook is ready for use, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_pb_check_state
(
  qbi_txn_s *txn
)
{
  boolean pb_is_ready = FALSE;
  qbi_svc_pb_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  cache = qbi_svc_pb_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (!qbi_svc_bc_check_device_state(txn, TRUE, FALSE))
  {
    QBI_LOG_W_0("Device not ready for phonebook operations");
  }
  else if (cache->configuration.qmi.no_adn_available)
  {
    QBI_LOG_E_0("No phonebook available");
    txn->status = QBI_MBIM_STATUS_NO_PHONEBOOK;
  }
  else if ((txn->cid != QBI_SVC_PB_MBIM_CID_PHONEBOOK_CONFIGURATION &&
            cache->configuration.mbim.state != QBI_SVC_PB_STATE_INITIALIZED) ||
           !cache->configuration.qmi.session_valid)
  {
    QBI_LOG_E_0("Phonebook not initialized");
    txn->status = QBI_MBIM_STATUS_NOT_INITIALIZED;
  }
  else
  {
    pb_is_ready = TRUE;
  }

  return pb_is_ready;
} /* qbi_svc_pb_check_state() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_D_0("Processing Phonebook device service open...");
  if (qbi_svc_cache_alloc(
        txn->ctx, QBI_SVC_ID_PB, sizeof(qbi_svc_pb_cache_s)) == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate cache");
  }
  else if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_PBM))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else if (!qbi_svc_ind_reg_static(
             txn->ctx, QBI_SVC_ID_PB, qbi_svc_pb_static_ind_hdlrs,
             ARR_SIZE(qbi_svc_pb_static_ind_hdlrs)))
  {
    QBI_LOG_E_0("Couldn't register QMI indication handlers");
  }
  else
  {
    action = qbi_svc_pb_reg_inds_with_qmi(txn);
  }

  return action;
} /* qbi_svc_pb_open() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_qmi_error_to_mbim_status
===========================================================================*/
/*!
    @brief Maps a QMI error code returned by QMI PBM into an MBIM status
    code, and saves the status code in the txn->status field

    @details

    @param txn
    @param qmi_error
*/
/*=========================================================================*/
static void qbi_svc_pb_qmi_error_to_mbim_status
(
  qbi_txn_s         *txn,
  qmi_error_type_v01 qmi_error
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  switch (qmi_error)
  {
    case QMI_ERR_INVALID_ID_V01:
      txn->status = QBI_MBIM_STATUS_INVALID_MEMORY_INDEX;
      break;

    case QMI_ERR_PB_NOT_READY_V01:
      txn->status = QBI_MBIM_STATUS_NOT_INITIALIZED;
      break;

    case QMI_ERR_PB_ACCESS_RESTRICTED_V01:
      txn->status = QBI_MBIM_STATUS_NO_PHONEBOOK;
      break;

    case QMI_ERR_NO_SIM_V01:
      txn->status = QBI_MBIM_STATUS_SIM_NOT_INSERTED;
      break;

    case QMI_ERR_PB_DELETE_IN_PROG_V01:
      txn->status = QBI_MBIM_STATUS_BUSY;
      break;

    case QMI_ERR_PIN_RESTRICTION_V01:
    case QMI_ERR_PIN2_RESTRICTION_V01:
    case QMI_ERR_PUK_RESTRICTION_V01:
    case QMI_ERR_PUK2_RESTRICTION_V01:
      txn->status = QBI_MBIM_STATUS_PIN_REQUIRED;
      break;

    default:
      txn->status = QBI_MBIM_STATUS_FAILURE;
  }
  QBI_LOG_I_2("Mapped QMI error code %d to MBIM status %d",
              qmi_error, txn->status);
} /* qbi_svc_pb_qmi_error_to_mbim_status() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_reg_inds_pbm01_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_INDICATION_REGISTER_RESP used for initial
    indication registration at device service open

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_reg_inds_pbm01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action;
  pbm_indication_register_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (pbm_indication_register_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for PBM indications!!! Error code %d",
                qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    QBI_LOG_D_0("Phonebook device service now opened");
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_pb_reg_inds_pbm01_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_reg_inds_with_qmi
===========================================================================*/
/*!
    @brief Registers for QMI indications handled by the static indication
    handlers

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_reg_inds_with_qmi
(
  qbi_txn_s *txn
)
{
  pbm_indication_register_req_msg_v01 *pbm01_req;
/*-------------------------------------------------------------------------*/
  pbm01_req = (pbm_indication_register_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_PBM, QMI_PBM_INDICATION_REGISTER_REQ_V01,
      qbi_svc_pb_reg_inds_pbm01_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(pbm01_req);

  pbm01_req->reg_mask = (PBM_REG_RECORD_UPDATE_EVENTS_V01 |
                         PBM_REG_PHONEBOOK_READY_EVENTS_V01);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_pb_reg_inds_with_qmi() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_sanity_check_read_or_delete_s_req
===========================================================================*/
/*!
    @brief Common sanity checks for a MBIM_CID_PHONEBOOK_READ or
    MBIM_CID_PHONEBOOK_DELETE set request, both of which include a filter
    flag and index field

    @details
    Sets txn->status to an appropriate MBIM status code when a check fails.

    @param txn
    @param flag
    @param index

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_pb_sanity_check_read_or_delete_s_req
(
  qbi_txn_s *txn,
  uint32     flag,
  uint32     index
)
{
  boolean all_checks_passed = FALSE;
  qbi_svc_pb_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  cache = qbi_svc_pb_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (!qbi_svc_pb_check_state(txn))
  {
    QBI_LOG_E_0("Read/delete request failed during device state checks");
  }
  else if (flag != QBI_SVC_PB_FILTER_FLAG_ALL &&
           flag != QBI_SVC_PB_FILTER_FLAG_INDEX)
  {
    QBI_LOG_E_1("Invalid filter flag %d", flag);
    txn->status = QBI_MBIM_STATUS_FILTER_NOT_SUPPORTED;
  }
  else if (flag == QBI_SVC_PB_FILTER_FLAG_INDEX &&
           (index == QBI_SVC_PB_INDEX_NONE ||
            index > cache->configuration.mbim.total_entries))
  {
    QBI_LOG_E_1("Invalid index %d", index);
    txn->status = QBI_MBIM_STATUS_INVALID_MEMORY_INDEX;
  }
  else
  {
    all_checks_passed = TRUE;
  }

  return all_checks_passed;
} /* qbi_svc_pb_sanity_check_read_or_delete_s_req() */

/*! @addtogroup MBIM_CID_PHONEBOOK_CONFIGURATION
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_e_pbm09_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_RECORD_UPDATE_IND; if we detect that the
    phonebook is now full, an MBIM_CID_CONFIGURATION event will be sent

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_configuration_e_pbm09_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_pb_cache_s *cache;
  pbm_record_update_ind_msg_v01 *qmi_ind;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);

  cache = qbi_svc_pb_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_ind = (pbm_record_update_ind_msg_v01 *) ind->buf->data;
  if (cache->configuration.mbim.state == QBI_SVC_PB_STATE_INITIALIZED &&
      qmi_ind->record_update_info.pb_type == QBI_SVC_PB_PHONEBOOK_TYPE &&
      qmi_ind->record_update_info.session_type ==
        qbi_svc_pb_configuration_session_get(ind->txn->ctx))
  {
    if (qmi_ind->record_update_info.operation == PBM_OPERATION_ADD_V01)
    {
      if (cache->configuration.mbim.used_entries >=
            cache->configuration.mbim.total_entries)
      {
        QBI_LOG_E_2("Received record add indication, but cache shows "
                    "phonebook is full! (%d/%d entries used)",
                    cache->configuration.mbim.used_entries,
                    cache->configuration.mbim.total_entries);
        /* Perform a query to try to resync the cache */
        action = qbi_svc_pb_configuration_q_req(ind->txn);
      }
      else
      {
        cache->configuration.mbim.used_entries++;
        QBI_LOG_I_3("New phonebook record added at index %d: %d/%d entries used",
                    qmi_ind->record_update_info.record_id,
                    cache->configuration.mbim.used_entries,
                    cache->configuration.mbim.total_entries);
        if (cache->configuration.mbim.used_entries >=
              cache->configuration.mbim.total_entries)
        {
          QBI_LOG_W_0("Phonebook is now full. Sending event");
          action = qbi_svc_pb_configuration_eq_build_rsp_from_cache(ind->txn);
        }
      }
    }
    else if (qmi_ind->record_update_info.operation == PBM_OPERATION_DELETE_V01)
    {
      if (cache->configuration.mbim.used_entries == 0)
      {
        QBI_LOG_E_2("Received record delete indication, but cache shows "
                    "phonebook is empty! (%d/%d entries used)",
                    cache->configuration.mbim.used_entries,
                    cache->configuration.mbim.total_entries);
        /* Perform a query to try to resync the cache */
        action = qbi_svc_pb_configuration_q_req(ind->txn);
      }
      else
      {
        cache->configuration.mbim.used_entries--;
        QBI_LOG_D_3("Phonebook record at index %d deleted: %d/%d entries used",
                    qmi_ind->record_update_info.record_id,
                    cache->configuration.mbim.used_entries,
                    cache->configuration.mbim.total_entries);
      }
    }
  }

  return action;
} /* qbi_svc_pb_configuration_e_pbm09_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_e_pbm0d_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_ALL_PB_INIT_DONE_IND, checking to see whether
    we should send a MBIM_CID_PHONEBOOK_CONFIGURATION event to notify the
    host that the phonebook is ready

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_configuration_e_pbm0d_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_pb_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);

  cache = qbi_svc_pb_cache_get(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (cache->configuration.mbim.state == QBI_SVC_PB_STATE_NOT_INITIALIZED)
  {
    action = qbi_svc_pb_configuration_eq_build_pbm11_req(
      ind->txn, QBI_SVC_PB_SESSION_PRIMARY,
      qbi_svc_pb_configuration_e_pbm11_rsp_cb);
  }
  else
  {
    QBI_LOG_D_0("Received ALL_INIT_DONE_IND but already initialized");
  }

  return action;
} /* qbi_svc_pb_configuration_e_pbm0d_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_e_pbm11_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_GET_PB_STATE_RESP to try to select the session
    to use for phonebook services

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_configuration_e_pbm11_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_pb_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  pbm_get_pb_state_req_msg_v01 *qmi_req;
  pbm_get_pb_state_resp_msg_v01 *qmi_rsp;
  pbm_session_type_enum_v01 session_next;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_pb_cache_get(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_req = (pbm_get_pb_state_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (pbm_get_pb_state_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);

    /* We may get QMI_ERR_NO_SIM in situations where an application does not
       exist on the UICC: for example, if we tried querying the GW session on
       a UICC containing only a CSIM. For this function, this error code is
       treated the same as PB_STATE_NOT_AVAILABLE */
    if (qmi_rsp->resp.error == QMI_ERR_NO_SIM_V01)
    {
      QBI_LOG_I_0("Mapping QMI_ERR_NO_SIM to PB_STATE_NOT_AVAILABLE");
      qmi_rsp->phonebook_state_valid = TRUE;
      qmi_rsp->phonebook_state.session_type =
        qmi_req->phonebook_info.session_type;
      qmi_rsp->phonebook_state.pb_type = qmi_req->phonebook_info.pb_type;
      qmi_rsp->phonebook_state.state = PBM_PB_STATE_NOT_AVAILABLE_V01;
    }
  }

  if (qmi_rsp->phonebook_state_valid)
  {
    QBI_LOG_I_3("Phonebook type 0x%x on session %d in state %d",
                qmi_rsp->phonebook_state.pb_type,
                qmi_rsp->phonebook_state.session_type,
                qmi_rsp->phonebook_state.state);
    if (qmi_rsp->phonebook_state.state == PBM_PB_STATE_NOT_AVAILABLE_V01)
    {
      QBI_LOG_I_0("Phonebook not available");
      if (qbi_svc_pb_configuration_session_next(
            qmi_txn->ctx, qmi_rsp->phonebook_state.session_type, &session_next))
      {
        QBI_LOG_I_1("Trying next session type %d", session_next);
        action = qbi_svc_pb_configuration_eq_build_pbm11_req(
          qmi_txn->parent, session_next,
          qbi_svc_pb_configuration_e_pbm11_rsp_cb);
      }
      else
      {
        QBI_LOG_I_0("No phonebook available on SIM!");
        cache->configuration.qmi.no_adn_available = TRUE;
      }
    }
    else if (qmi_rsp->phonebook_state.state == PBM_PB_STATE_NOT_READY_V01)
    {
      /* The phonebook on this session is still initializing - we can't make
         any assumptions about whether it exists, etc. so just wait for another
         indication */
      QBI_LOG_I_0("Phonebook not ready; waiting for init to complete");
    }
    else
    {
      QBI_LOG_I_1("Session %d will be used for phonebook services",
                  qmi_rsp->phonebook_state.session_type);
      cache->configuration.qmi.session = qmi_rsp->phonebook_state.session_type;
      cache->configuration.qmi.session_valid = TRUE;
      cache->configuration.qmi.no_adn_available = FALSE;

      if (qmi_rsp->phonebook_state.state == PBM_PB_STATE_READY_V01 &&
          cache->configuration.mbim.state == QBI_SVC_PB_STATE_NOT_INITIALIZED)
      {
        QBI_LOG_I_0("Phonebook is now ready");

        /* Use the query to update the cache and send an event */
        action = qbi_svc_pb_configuration_q_req(qmi_txn->parent);
      }
    }
  }

  return action;
} /* qbi_svc_pb_configuration_e_pbm11_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_eq_build_pbm11_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_PBM_GET_PB_STATE request

    @details

    @param txn
    @param session_type
    @param rsp_cb

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_configuration_eq_build_pbm11_req
(
  qbi_txn_s                *txn,
  pbm_session_type_enum_v01 session_type,
  qbi_svc_qmi_rsp_cb_f     *rsp_cb
)
{
  pbm_get_pb_state_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  qmi_req = (pbm_get_pb_state_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_PBM, QMI_PBM_GET_PB_STATE_REQ_V01, rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->phonebook_info.session_type = session_type;
  qmi_req->phonebook_info.pb_type = QBI_SVC_PB_PHONEBOOK_TYPE;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_pb_configuration_eq_build_pbm11_req() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_eq_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Allocates and populates an MBIM_CID_PHONEBOOK_CONFIGURATION
    event/query response based on the current contents of the cache

    @details
    Assumes the cache is fully initialized.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_configuration_eq_build_rsp_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_pb_cache_s *cache;
  qbi_svc_pb_configuration_info_q_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_pb_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_pb_configuration_info_q_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_pb_configuration_info_q_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  /* All information in the response is kept in the cache, so just copy
     directly out of the cache */
  QBI_MEMSCPY(rsp, txn->rsp.size, &cache->configuration.mbim,
              sizeof(qbi_svc_pb_configuration_info_q_rsp_s));

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_pb_configuration_eq_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_q_pbm02_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_GET_PB_CAPABILITIES_RESP for a
    MBIM_CID_PHONEBOOK_CONFIGURATION query (or query triggered for an event)

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_configuration_q_pbm02_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  boolean event_allowed;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  pbm_get_pb_capabilities_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (pbm_get_pb_capabilities_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_pb_qmi_error_to_mbim_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->capability_basic_info_valid)
  {
    QBI_LOG_E_0("Missing TLV in response");
  }
  else
  {
    event_allowed = qbi_svc_pb_configuration_update_cache_pbm02(
      qmi_txn->ctx, &qmi_rsp->capability_basic_info);
    if (qmi_txn->parent->cmd_type == QBI_TXN_CMD_TYPE_EVENT && !event_allowed)
    {
      action = QBI_SVC_ACTION_ABORT;
    }
    else
    {
      action = qbi_svc_pb_configuration_eq_build_rsp_from_cache(
        qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_pb_configuration_q_pbm02_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_q_pbm11_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_GET_PB_STATE_RESP for
    MBIM_CID_PHONEBOOK_CONFIGURATION query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_configuration_q_pbm11_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  pbm_get_pb_state_resp_msg_v01 *qmi_rsp;
  pbm_get_pb_capabilities_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (pbm_get_pb_state_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_pb_qmi_error_to_mbim_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->phonebook_state_valid)
  {
    QBI_LOG_E_0("Missing TLV in response!");
  }
  else
  {
    if (qmi_rsp->phonebook_state.state != PBM_PB_STATE_READY_V01)
    {
      qmi_txn->parent->status =
        qbi_svc_pb_configuration_qmi_pb_state_to_mbim_status(
          qmi_rsp->phonebook_state.state);
    }
    else
    {
      qmi_req = (pbm_get_pb_capabilities_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
          qmi_txn->parent, QBI_QMI_SVC_PBM, QMI_PBM_GET_PB_CAPABILITIES_REQ_V01,
          qbi_svc_pb_configuration_q_pbm02_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

      qmi_req->phonebook_info.session_type =
        qmi_rsp->phonebook_state.session_type;
      qmi_req->phonebook_info.pb_type = qmi_rsp->phonebook_state.pb_type;

      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_pb_configuration_q_pbm11_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_q_req
===========================================================================*/
/*!
    @brief Performs a query of the phonebook configuration

    @details
    This may be called as a part of event processing: it will update the
    cache with the newly queried information, and if the change to the
    cache should not result in an event, the transaction will be aborted.

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_configuration_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_pb_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_pb_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (!qbi_svc_pb_check_state(txn))
  {
    QBI_LOG_E_0("Phonebook not ready to query configuration");
  }
  else
  {
    action = qbi_svc_pb_configuration_eq_build_pbm11_req(
      txn, cache->configuration.qmi.session,
      qbi_svc_pb_configuration_q_pbm11_rsp_cb);
  }

  return action;
} /* qbi_svc_pb_configuration_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_qmi_pb_state_to_mbim_status
===========================================================================*/
/*!
    @brief Maps a QMI phonebook state enum to MBIM failure status enum

    @details
    This should only be used to map the phonebook state to an error status,
    as it does not include a mapping for PBM_PB_STATE_READY to
    MBIM_STATUS_SUCCESS

    @param qmi_pb_state

    @return qbi_mbim_status_e
*/
/*=========================================================================*/
static qbi_mbim_status_e qbi_svc_pb_configuration_qmi_pb_state_to_mbim_status
(
  pbm_pb_state_enum_v01 qmi_pb_state
)
{
  qbi_mbim_status_e status;
/*-------------------------------------------------------------------------*/
  switch (qmi_pb_state)
  {
    case PBM_PB_STATE_NOT_READY_V01:
      status = QBI_MBIM_STATUS_NOT_INITIALIZED;
      break;

    case PBM_PB_STATE_PIN_RESTRICTION_V01:
    case PBM_PB_STATE_PUK_RESTRICTION_V01:
      status = QBI_MBIM_STATUS_PIN_REQUIRED;
      break;

    case PBM_PB_STATE_SYNC_V01:
      status = QBI_MBIM_STATUS_BUSY;
      break;

    case PBM_PB_STATE_NOT_AVAILABLE_V01:
    case PBM_PB_STATE_INVALIDATED_V01:
      /*! @note Invalidated means that FDN restriction is in effect, so ADN
          access is restricted. MBIM doesn't have a good mapping for this, so we
          just act like no phonebook is available to the user. */
      status = QBI_MBIM_STATUS_NO_PHONEBOOK;
      break;

    default:
      QBI_LOG_E_1("Unexpected pb_state %d", qmi_pb_state);
      status = QBI_MBIM_STATUS_FAILURE;
  }

  QBI_LOG_I_2("Mapped QMI pb_state %d to MBIM status %d", qmi_pb_state, status);
  return status;
} /* qbi_svc_pb_configuration_qmi_pb_state_to_mbim_status() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_session_get
===========================================================================*/
/*!
    @brief Retrieves the currently cached QMI session type

    @details
    This function will only return a valid session type if
    cache->configuration.qmi.session_valid is set to TRUE. The recommended
    way to check this condition, as well as others that will prevent a
    QMI PBM request from succeeding is via qbi_svc_pb_check_state()

    @param ctx

    @return pbm_session_type_enum_v01
*/
/*=========================================================================*/
static pbm_session_type_enum_v01 qbi_svc_pb_configuration_session_get
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_pb_cache_s *cache;
  pbm_session_type_enum_v01 session;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_pb_cache_get(ctx);
  if (cache == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
    session = QBI_SVC_PB_SESSION_PRIMARY;
  }
  else
  {
    session = cache->configuration.qmi.session;
  }

  return session;
} /* qbi_svc_pb_configuration_session_get() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_session_next
===========================================================================*/
/*!
    @brief Gets the next session type based on priority and device
    capabilities

    @details
    Highest priority session is given by the constant
    QBI_SVC_PB_SESSION_PRIMARY.

    @param ctx
    @param session_prev
    @param session_next

    @return boolean TRUE if session_next was populated with a new session
    type, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_pb_configuration_session_next
(
  qbi_ctx_s                 *ctx,
  pbm_session_type_enum_v01  session_prev,
  pbm_session_type_enum_v01 *session_next
)
{
  boolean next_avail = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(session_next);

  switch (session_prev)
  {
    case PBM_SESSION_TYPE_GLOBAL_PB_SLOT1_V01:
      *session_next = PBM_SESSION_TYPE_GW_PRIMARY_V01;
      next_avail = TRUE;
      break;

    case PBM_SESSION_TYPE_GW_PRIMARY_V01:
      if (qbi_svc_bc_device_supports_3gpp2(ctx))
      {
        *session_next = PBM_SESSION_TYPE_1X_PRIMARY_V01;
        next_avail = TRUE;
      }
      break;

    case PBM_SESSION_TYPE_1X_PRIMARY_V01:
      break;

    default:
      QBI_LOG_E_1("Unexpected session type %d", session_prev);
  }

  if (!next_avail)
  {
    QBI_LOG_D_1("No session available after %d", session_prev);
  }

  return next_avail;
} /* qbi_svc_pb_configuration_session_next() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_configuration_update_cache_pbm02
===========================================================================*/
/*!
    @brief Updates the cache with information from
    QMI_PBM_GET_PB_CAPABILITIES_RESP

    @details
    Assumes that the phonebook is considered initialized whenever this
    function is called

    @param ctx
    @param pb_info

    @return boolean TRUE if the cached phonebook information has changed in
    a way that should result in an MBIM_PHONEBOOK_CONFIGURATION event: these
    situations are only when the phonebook becomes initialized for the first
    time, or when the phonebook becomes full
*/
/*=========================================================================*/
static boolean qbi_svc_pb_configuration_update_cache_pbm02
(
  const qbi_ctx_s                          *ctx,
  const pbm_capability_basic_info_type_v01 *pb_info
)
{
  boolean event_allowed = FALSE;
  qbi_svc_pb_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(pb_info);

  cache = qbi_svc_pb_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (cache->configuration.mbim.state != QBI_SVC_PB_STATE_INITIALIZED)
  {
    QBI_LOG_I_4("Phonebook is now initialized with %d/%d records used and max "
                "number/name length %d/%d",
                pb_info->used_records, pb_info->max_records,
                (pb_info->max_num_len * 2), pb_info->max_name_len);
    cache->configuration.mbim.state = QBI_SVC_PB_STATE_INITIALIZED;
    event_allowed = TRUE;
  }
  else if (cache->configuration.mbim.used_entries != pb_info->used_records &&
           pb_info->used_records >= pb_info->max_records)
  {
    QBI_LOG_I_2("Phonebook is now full with %d/%d records used",
                pb_info->used_records, pb_info->max_records);
    event_allowed = TRUE;
  }
  cache->configuration.mbim.total_entries = pb_info->max_records;
  cache->configuration.mbim.used_entries  = pb_info->used_records;

  /* Max number length is multiplied by two, since QMI uses ASCII for the
     number, while MBIM uses UTF-16 */
  cache->configuration.mbim.max_num_length  = (pb_info->max_num_len * 2);
  cache->configuration.mbim.max_name_length = pb_info->max_name_len;

  return event_allowed;
} /* qbi_svc_pb_configuration_update_cache_pbm02() */

/*! @} */

/*! @addtogroup MBIM_CID_PHONEBOOK_READ
    { */

/*===========================================================================
  FUNCTION: qbi_svc_pb_read_q_add_record
===========================================================================*/
/*!
    @brief Adds data for a single phonebook record to the transaction's
    response buffer

    @details

    @param txn
    @param record

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_read_q_add_record
(
  qbi_txn_s                          *txn,
  const pbm_record_instance_type_v01 *record
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_pb_read_info_s *info;
  qbi_svc_pb_cache_s *cache;
  qbi_mbim_offset_size_pair_s *field;
  qbi_svc_pb_entry_s *entry;
  uint32 initial_size;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(record);

  cache = qbi_svc_pb_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  info = (qbi_svc_pb_read_info_s *) txn->info;
  if (info->records_read >= info->records_total)
  {
    QBI_LOG_E_2("Tried to add new record, but all records read! (%d/%d)",
                info->records_read, info->records_total);
  }
  else
  {
    field = (qbi_mbim_offset_size_pair_s *)
      ((uint8 *) txn->rsp.data + sizeof(qbi_svc_pb_read_q_rsp_s) +
       (info->records_read * sizeof(qbi_mbim_offset_size_pair_s)));

    entry = (qbi_svc_pb_entry_s *) qbi_txn_rsp_databuf_add_field(
      txn, field, 0, sizeof(qbi_svc_pb_entry_s), NULL);
    QBI_CHECK_NULL_PTR_RET_ABORT(entry);

    initial_size = txn->infobuf_len_total;
    entry->index = record->record_id;
    if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          txn, &entry->number, field->offset,
          cache->configuration.mbim.max_num_length, record->number,
          record->number_len))
    {
      QBI_LOG_E_0("Couldn't add number to response");
    }
    else if (!qbi_txn_rsp_databuf_add_field(
               txn, &entry->name, field->offset, record->name_len,
               record->name))
    {
      QBI_LOG_E_0("Couldn't add name to response");
    }
    else if (!qbi_txn_rsp_databuf_consolidate(txn))
    {
      QBI_LOG_E_0("Couldn't consolidate fields");
    }
    else
    {
      /* Account for the addition of the variable length number and name fields
         in the total phonebook entry size */
      field->size += txn->infobuf_len_total - initial_size;

      info->records_read++;
      if (info->records_read >= info->records_total)
      {
        action = QBI_SVC_ACTION_SEND_RSP;
      }
      else
      {
        action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
      }
    }
  }

  return action;
} /* qbi_svc_pb_read_q_add_record() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_read_q_pbm04_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_READ_RECORDS_RESP for MBIM_CID_PHONEBOOK_READ
    query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_read_q_pbm04_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  pbm_read_records_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (pbm_read_records_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Phonebook read failed with error %d", qmi_rsp->resp.error);
    qbi_svc_pb_qmi_error_to_mbim_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->num_of_recs_valid)
  {
    QBI_LOG_E_0("Missing TLV in response!");
  }
  else
  {
    QBI_LOG_I_1("Read request retured %d records", qmi_rsp->num_of_recs);

    /* Prepare the transaction to receive record data in a series of
       indications */
    action = qbi_svc_pb_read_q_prepare_for_inds(
      qmi_txn->parent, qmi_rsp->num_of_recs);
  }

  return action;
} /* qbi_svc_pb_read_q_pbm04_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_read_q_pbm04_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_RECORD_READ_IND for MBIM_CID_PHONEBOOK_READ
    query

    @details
    Adds the record(s) in the indication to the response buffer, and sends
    the response to the host if all the records have been read.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_read_q_pbm04_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  uint32 i;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const pbm_record_read_ind_msg_v01 *qmi_ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const pbm_record_read_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->basic_record_data.pb_type != QBI_SVC_PB_PHONEBOOK_TYPE)
  {
    QBI_LOG_W_1("Received data for unexpected PB type 0x%x, ignoring",
                qmi_ind->basic_record_data.pb_type);
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  else
  {
    QBI_LOG_I_2("Received phonebook read indication with %d records (seq_num "
                "%d)", qmi_ind->basic_record_data.record_instances_len,
                qmi_ind->basic_record_data.seq_num);
    for (i = 0; i < qmi_ind->basic_record_data.record_instances_len; i++)
    {
      action = qbi_svc_pb_read_q_add_record(
        ind->txn, &qmi_ind->basic_record_data.record_instances[i]);
      if (action == QBI_SVC_ACTION_ABORT)
      {
        break;
      }
    }
  }

  return action;
} /* qbi_svc_pb_read_q_pbm04_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_read_q_prepare_for_inds
===========================================================================*/
/*!
    @brief Prepares an MBIM_CID_PHONEBOOK_READ query transaction for
    receiving phonebook record data via QMI_PBM_RECORD_READ_IND

    @details
    Includes allocating and populating the fixed length portion of the
    response, the information structure, and registering the dynamic
    indication handler. If element_count == 0, the last two steps are
    not performed and this function returns QBI_SVC_ACTION_SEND_RSP.

    @param txn
    @param element_count

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_read_q_prepare_for_inds
(
  qbi_txn_s *txn,
  uint16     element_count
)
{
  qbi_svc_pb_read_q_rsp_s *rsp;
  qbi_svc_pb_read_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  rsp = (qbi_svc_pb_read_q_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, (sizeof(qbi_svc_pb_read_q_rsp_s) +
          (element_count * sizeof(qbi_mbim_offset_size_pair_s))));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->element_count = element_count;
  if (element_count == 0)
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    txn->info = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_pb_read_info_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

    info = (qbi_svc_pb_read_info_s *) txn->info;
    info->records_total = element_count;

    if (!qbi_svc_ind_reg_dynamic(
          txn->ctx, QBI_SVC_ID_PB, QBI_SVC_PB_MBIM_CID_PHONEBOOK_READ,
          QBI_QMI_SVC_PBM, QMI_PBM_RECORD_READ_IND_V01,
          qbi_svc_pb_read_q_pbm04_ind_cb, txn, NULL))
    {
      QBI_LOG_E_0("Couldn't register dynamic indication handler!");
    }
    else
    {
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
  }

  return action;
} /* qbi_svc_pb_read_q_prepare_for_inds() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_read_q_req
===========================================================================*/
/*!
    @brief Handles an MBIM_CID_PHONEBOOK_READ query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_read_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_pb_cache_s *cache;
  qbi_svc_pb_read_q_req_s *req;
  pbm_read_records_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  cache = qbi_svc_pb_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  req = (qbi_svc_pb_read_q_req_s *) txn->req.data;
  if (!qbi_svc_pb_sanity_check_read_or_delete_s_req(
        txn, req->filter_flag, req->filter_index))
  {
    QBI_LOG_E_0("Read request failed during sanity check");
  }
  else
  {
    qmi_req = (pbm_read_records_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_PBM, QMI_PBM_READ_RECORDS_REQ_V01,
      qbi_svc_pb_read_q_pbm04_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->record_info.session_type =
      qbi_svc_pb_configuration_session_get(txn->ctx);
    qmi_req->record_info.pb_type = QBI_SVC_PB_PHONEBOOK_TYPE;
    if (req->filter_flag == QBI_SVC_PB_FILTER_FLAG_ALL)
    {
      qmi_req->record_info.record_start_id = QBI_QMI_PBM_RECORD_ID_START;
      qmi_req->record_info.record_end_id   =
        (uint16_t) cache->configuration.mbim.total_entries;
    }
    else
    {
      qmi_req->record_info.record_start_id = (uint16_t) req->filter_index;
      qmi_req->record_info.record_end_id   = (uint16_t) req->filter_index;
    }

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_pb_read_q_req() */

/*! @} */

/*! @addtogroup MBIM_CID_PHONEBOOK_DELETE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_pb_delete_s_pbm06_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_DELETE_RECORD_RESP for MBIM_CID_PHONEBOOK_DELETE
    set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_delete_s_pbm06_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  pbm_delete_record_req_msg_v01 *qmi_req;
  pbm_delete_record_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (pbm_delete_record_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (pbm_delete_record_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_2("Delete of phonebook record at index %d failed with error %d",
                qmi_req->record_info.record_id, qmi_rsp->resp.error);
    qbi_svc_pb_qmi_error_to_mbim_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_1("Successfully deleted phonebook record at index %d",
                qmi_req->record_info.record_id);
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_pb_delete_s_pbm06_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_delete_s_pbm07_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_DELETE_ALL_PB_RECORDS_RESP for
    MBIM_CID_PHONEBOOK_DELETE set request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_delete_s_pbm07_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  pbm_delete_all_pb_records_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (pbm_delete_all_pb_records_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Delete of all phonebook records failed with error %d",
                qmi_rsp->resp.error);
    qbi_svc_pb_qmi_error_to_mbim_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_0("Successfully deleted all phonebook records");
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_pb_delete_s_pbm07_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_delete_s_req
===========================================================================*/
/*!
    @brief Handles an MBIM_CID_PHONEBOOK_DELETE set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_delete_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_pb_delete_s_req_s *req;
  pbm_delete_record_req_msg_v01 *pbm06_req;
  pbm_delete_all_pb_records_req_msg_v01 *pbm07_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_pb_delete_s_req_s *) txn->req.data;
  if (!qbi_svc_pb_sanity_check_read_or_delete_s_req(
        txn, req->filter_flag, req->filter_index))
  {
    QBI_LOG_E_0("Delete request failed sanity checks");
  }
  else
  {
    if (req->filter_flag == QBI_SVC_PB_FILTER_FLAG_ALL)
    {
      pbm07_req = (pbm_delete_all_pb_records_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
          txn, QBI_QMI_SVC_PBM, QMI_PBM_DELETE_ALL_PB_RECORDS_REQ_V01,
          qbi_svc_pb_delete_s_pbm07_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(pbm07_req);

      pbm07_req->phonebook_info.session_type =
        qbi_svc_pb_configuration_session_get(txn->ctx);
      pbm07_req->phonebook_info.pb_type = QBI_SVC_PB_PHONEBOOK_TYPE;
    }
    else
    {
      pbm06_req = (pbm_delete_record_req_msg_v01 *)
        qbi_qmi_txn_alloc_ret_req_buf(
          txn, QBI_QMI_SVC_PBM, QMI_PBM_DELETE_RECORD_REQ_V01,
          qbi_svc_pb_delete_s_pbm06_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(pbm06_req);

      pbm06_req->record_info.session_type =
        qbi_svc_pb_configuration_session_get(txn->ctx);
      pbm06_req->record_info.pb_type = QBI_SVC_PB_PHONEBOOK_TYPE;
      pbm06_req->record_info.record_id = (uint16_t) req->filter_index;
    }

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_pb_delete_s_req() */

/*! @} */

/*! @addtogroup MBIM_CID_PHONEBOOK_WRITE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_pb_write_s_pbm05_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_PBM_WRITE_RECORD_RESP for MBIM_CID_PHONEBOOK_WRITE

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_write_s_pbm05_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  pbm_write_record_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (pbm_write_record_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Phonebook write failed with QMI error %d",
                qmi_rsp->resp.error);
    qbi_svc_pb_qmi_error_to_mbim_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_1("Phonebook record successfully written to record ID %d",
                qmi_rsp->record_id);
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_pb_write_s_pbm05_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_write_s_req
===========================================================================*/
/*!
    @brief Handles an MBIM_CID_PHONEBOOK_WRITE set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_pb_write_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_pb_write_s_req_s *req;
  const uint8 *utf16_str;
  pbm_write_record_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_pb_write_s_req_s *) txn->req.data;
  if (!qbi_svc_pb_write_s_sanity_check_req(txn))
  {
    QBI_LOG_E_0("Phonebook write request failed sanity checks");
  }
  else
  {
    qmi_req = (pbm_write_record_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_PBM, QMI_PBM_WRITE_RECORD_REQ_V01,
      qbi_svc_pb_write_s_pbm05_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->record_information.session_type =
      qbi_svc_pb_configuration_session_get(txn->ctx);
    qmi_req->record_information.phonebook_type = QBI_SVC_PB_PHONEBOOK_TYPE;
    if (req->save_flag == QBI_SVC_PB_WRITE_FLAG_SAVE_UNUSED)
    {
      qmi_req->record_information.record_id =
        QBI_QMI_PBM_RECORD_ID_FIRST_AVAILABLE;
    }
    else
    {
      qmi_req->record_information.record_id = (uint16_t) req->save_index;
    }
    qmi_req->record_information.num_type = PBM_NUM_TYPE_UNKNOWN_V01;
    qmi_req->record_information.num_plan = PBM_NUM_PLAN_UNKNOWN_V01;

    utf16_str = qbi_txn_req_databuf_get_field(
      txn, &req->number, 0, (sizeof(qmi_req->record_information.number) * 2));
    QBI_CHECK_NULL_PTR_RET_ABORT(utf16_str);

    qmi_req->record_information.number_len = qbi_util_utf16_to_ascii(
      utf16_str, req->number.size, qmi_req->record_information.number,
      sizeof(qmi_req->record_information.number));

    utf16_str = qbi_txn_req_databuf_get_field(
      txn, &req->name, 0, sizeof(qmi_req->record_information.name));
    QBI_CHECK_NULL_PTR_RET_ABORT(utf16_str);

    qmi_req->record_information.name_len = req->name.size;
    QBI_MEMSCPY(qmi_req->record_information.name,
                sizeof(qmi_req->record_information.name),
                utf16_str, req->name.size);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_pb_write_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_write_s_sanity_check_req
===========================================================================*/
/*!
    @brief Ensures that the data provided by the host in an
    MBIM_CID_PHONEBOOK_WRITE set request is valid

    @details
    Sets txn->status to the appropriate MBIM status code if an error is
    detected.

    @param txn

    @return boolean TRUE if all sanity checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_pb_write_s_sanity_check_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_pb_cache_s *cache;
  qbi_svc_pb_write_s_req_s *req;
  boolean all_checks_passed = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  cache = qbi_svc_pb_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  req = (qbi_svc_pb_write_s_req_s *) txn->req.data;
  if (!qbi_svc_pb_check_state(txn))
  {
    QBI_LOG_E_0("Write request failed during device state checks");
  }
  else if (req->save_flag != QBI_SVC_PB_WRITE_FLAG_SAVE_UNUSED &&
           req->save_flag != QBI_SVC_PB_WRITE_FLAG_SAVE_INDEX)
  {
    QBI_LOG_E_1("Invalid save flag %d", req->save_flag);
    txn->status = QBI_MBIM_STATUS_FILTER_NOT_SUPPORTED;
  }
  else if (req->save_flag == QBI_SVC_PB_WRITE_FLAG_SAVE_INDEX &&
           (req->save_index == QBI_SVC_PB_INDEX_NONE ||
            req->save_index > cache->configuration.mbim.total_entries))
  {
    QBI_LOG_E_1("Invalid save index %d", req->save_index);
    txn->status = QBI_MBIM_STATUS_INVALID_MEMORY_INDEX;
  }
  else if (req->number.size > cache->configuration.mbim.max_num_length ||
           req->name.size > cache->configuration.mbim.max_name_length)
  {
    /*! @todo Would prefer to allow QMI to do this check, but the specific error
        codes are only supported on newer targets. */
    QBI_LOG_E_4("Number len (%d) or name len (%d) too big (max %d/%d)",
                req->number.size, req->name.size,
                cache->configuration.mbim.max_num_length,
                cache->configuration.mbim.max_name_length);
    txn->status = QBI_MBIM_STATUS_PARAMETER_TOO_LONG;
  }
  else if (req->save_flag == QBI_SVC_PB_WRITE_FLAG_SAVE_UNUSED &&
           cache->configuration.mbim.used_entries >=
             cache->configuration.mbim.total_entries)
  {
    QBI_LOG_E_2("Attempted to add new phonebook entry, but memory is full "
                "(%d/%d entries used)", cache->configuration.mbim.used_entries,
                cache->configuration.mbim.total_entries);
    txn->status = QBI_MBIM_STATUS_MEMORY_FULL;
  }
  else
  {
    all_checks_passed = TRUE;
  }

  return all_checks_passed;
} /* qbi_svc_pb_write_s_sanity_check_req() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_pb_init
===========================================================================*/
/*!
    @brief One-time initialization of the Phonebook device service

    @details
*/
/*=========================================================================*/
void qbi_svc_pb_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_pb_cfg = {
    {
      0x4b, 0xf3, 0x84, 0x76, 0x1e, 0x6a, 0x41, 0xdb,
      0xb1, 0xd8, 0xbe, 0xd2, 0x89, 0xc2, 0x5b, 0xdb
    },
    QBI_SVC_ID_PB,
    FALSE,
    qbi_svc_pb_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_pb_cmd_hdlr_tbl),
    qbi_svc_pb_open,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_pb_cfg);
} /* qbi_svc_pb_init() */

/*===========================================================================
  FUNCTION: qbi_svc_pb_sim_removed
===========================================================================*/
/*!
    @brief Notifies the phonebook device service that the SIM is removed,
    allowing it to update the host as required

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_pb_sim_removed
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
  qbi_svc_pb_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_pb_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache->configuration.mbim.state == QBI_SVC_PB_STATE_INITIALIZED)
  {
    /* Reset the entire cache, send an event notifying the host that PB is now
       in QBI_SVC_PB_STATE_NOT_INITIALIZED. We will re-init based on QMI PBM
       indications after a SIM is inserted. */
    QBI_LOG_W_0("SIM removed; de-initing phonebook");
    QBI_MEMSET(&cache->configuration, 0, sizeof(cache->configuration));

    txn = qbi_txn_alloc_event(
      ctx, QBI_SVC_ID_PB, QBI_SVC_PB_MBIM_CID_PHONEBOOK_CONFIGURATION);
    QBI_CHECK_NULL_PTR_RET(txn);

    (void) qbi_svc_proc_action(
      txn, qbi_svc_pb_configuration_eq_build_rsp_from_cache(txn));
  }
} /* qbi_svc_pb_sim_removed() */

