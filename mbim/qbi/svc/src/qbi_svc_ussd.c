/*!
  @file
  qbi_svc_ussd.c

  @brief
  USSD device service implementation

  @note
  Some example USSD call flows are outlined below:

  Mobile Originated (MO) USSD Session
  -> MBIM_CID_USSD set request from host with USSDAction = MBIMUSSDInitiate.
     QBI sends USSD origination request via QMI_VOICE_ORIG_USSD_NO_WAIT_REQ.
  <- Data payload received in QMI_VOICE_USSD_IND with notification of user
     action required.
     QBI sends response to host with USSDResponse = MBIMUSSDActionRequired.
  -> MBIM_CID_USSD set request from host with USSDAction = MBIMUSSDContinue.
     QBI sends USSD answer request via QMI_VOICE_ANSWER_USSD_REQ.
  <- Data payload received in QMI_VOICE_USSD_IND indication with notification of
     user action not required.
     QBI sends response to host with USSDResponse = MBIMUSSDNoActionRequired.
  <- Data payload received in QMI_VOICE_ORIG_USSD_NO_WAIT_IND indication.
     QBI sends event to host with USSDResponse = MBIMUSSDNoActionRequired.
  <- Network releases USSD session; QBI receives QMI_VOICE_USSD_RELEASE_IND.
     QBI sends event to host with USSDResponse = MBIMUSSDTerminatedByNW.

  Mobile Terminated (MT) USSD Session
  <- Data payload received from network in QMI_VOICE_USSD_IND indication with
     notification of user action required.
     QBI sends event to host with USSDSessionState = MBIMUSSDNewSession,
     USSDResponse = MBIMUSSDActionRequired.
  -> MBIM_CID_USSD set request from host with USSDAction = MBIMUSSDContinue
     QBI sends USSD answer request via QMI_VOICE_ANSWER_USSD_REQ.
  <- Data payload received from QMI in QMI_VOICE_USSD_IND indication with
     notification of user action not required.
     QBI sends response to host with USSDResponse = MBIMUSSDNoActionRequired.
  <- Network releases USSD session.
     ** Current NAS design does not forward release information to CM/QMI,
     ** MBIMUSSDTerminatedByNW event will not happen until NAS adds release
     ** support for MT USSD.

  For both MO and MT USSD sessions, there could be none or more than one
  QMI_VOICE_USSD_IND indications with notification of user action required.

  For MO USSD session, QMI_VOICE_ORIG_USSD_NO_WAIT_IND may contain an error
  code and no USSD payload.

  For MT USSD session, the session could end without final QMI_VOICE_USSD_IND
  indication after USSD answer.
*/

/*=============================================================================

  Copyright (c) 2011,2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
06/27/13  hz   Handle QMI_ERR_NO_NETWORK_FOUND when device is registered
06/04/13  hz   Add failure cause code to USSD response mapping
10/28/11  bd   Updated to MBIM v1.0 SC
09/09/11  hz   Updated to MBIM v0.81c
07/28/11  hz   Initial release based on MBIM v0.3+
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_ussd.h"
#include "qbi_svc_ussd_mbim.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_svc_bc.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc_nas.h"
#include "qbi_txn.h"

#include "voice_service_v02.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/* Current USSD sesion type */
#define QBI_SVC_USSD_SESSION_TYPE_NONE      (0)
#define QBI_SVC_USSD_SESSION_TYPE_MO        (1)
#define QBI_SVC_USSD_SESSION_TYPE_MT        (2)

/* USSD GSM 7-bit coding scheme mask */
#define QBI_SVC_USSD_DCS_7_BIT_MASK         (0x00)
/* USSD UCS2 coding preceded by language preference scheme mask */
#define QBI_SVC_USSD_DCS_UCS2_LANG_IND_MASK (0x01)
/* USSD 8-bit coding scheme mask */
#define QBI_SVC_USSD_DCS_8_BIT_MASK         (0x04)
/* USSD UCS2 coding scheme mask */
#define QBI_SVC_USSD_DCS_UCS2_MASK          (0x08)

#define QBI_SVC_USSD_CHAR_CR                (0x0D)

/* Maximum number of characters that can be packed into a USSD payload */
#define QBI_SVC_USSD_UNPACKED_DATA_LEN_MAX  (182)

/* Timeouts for USSD set if no response from network */
#define QBI_SVC_USSD_SET_TIMEOUT_MS  (60 * 1000)

/*! This macro statically defines a QMI indication handler and fills in the
    fields that are common to all handlers in this device service */
#define QBI_SVC_USSD_STATIC_IND_HDLR(qmi_svc_id, qmi_msg_id, cid, cb) \
  {qmi_svc_id, qmi_msg_id, cid, cb, NULL}

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! @brief USSD service local cache struct
*/
typedef struct {
  struct {
    qbi_txn_s *pending_txn;
    uint32     current_session_type;
  } ussd;
} qbi_svc_ussd_cache_s;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static uint32 qbi_svc_ussd_ascii_to_gsm_alphabet
(
  uint8       *gsm_alphabet_string,
  uint32       gsm_alphabet_string_size,
  const uint8 *ascii_string,
  uint32       num_chars
);

static qbi_svc_ussd_cache_s *qbi_svc_ussd_get_cache
(
  const qbi_ctx_s *ctx
);

static uint32 qbi_svc_ussd_gsm_alphabet_to_ascii
(
  uint8        *ascii_string,
  uint32        ascii_string_size,
  const uint8  *gsm_alphabet_string,
  uint32        num_bytes
);

static qbi_svc_action_e qbi_svc_ussd_open
(
  qbi_txn_s *txn
);

/*! @addtogroup MBIM_CID_USSD
    @{ */

static boolean qbi_svc_ussd_end_session
(
  qbi_ctx_s *ctx
);

static void qbi_svc_ussd_end_session_if_mt_no_action
(
  qbi_ctx_s *ctx,
  uint32     ussd_response
);

static qbi_svc_action_e qbi_svc_ussd_es_voice3d_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_ussd_es_voice3e_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_ussd_es_voice43_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_ussd_es_rsp
(
  qbi_txn_s        *txn,
  uint32            ussd_response,
  uint32            session_state,
  uss_dcs_enum_v02  qmi_data_coding_scheme,
  uint32            qmi_uss_data_len,
  const uint8_t    *qmi_uss_data
);

static uint32 qbi_svc_ussd_failure_cause_code_to_response_mapping
(
  qmi_sups_errors_enum_v02 failure_cause
);

static qbi_txn_s *qbi_svc_ussd_get_rsp_txn
(
  qbi_txn_s *event_txn
);

static uint32 qbi_svc_ussd_get_session_state
(
  qbi_ctx_s *ctx
);

static uss_dcs_enum_v02 qbi_svc_ussd_mbim_to_qmi_dcs_mapping
(
  uint32 mbim_dcs
);

static qbi_svc_action_e qbi_svc_ussd_proc_pending_txn
(
  qbi_ctx_s       *ctx,
  qbi_svc_action_e action
);

static boolean qbi_svc_ussd_qmi_further_action_to_mbim_response
(
  further_user_action_enum_v02 qmi_further_action,
  uint32                      *mbim_response
);

static void qbi_svc_ussd_qmi_set_mbim_error
(
  qbi_txn_s          *txn,
  qmi_error_type_v01  qmi_error
);

static uint32 qbi_svc_ussd_qmi_to_mbim_dcs_mapping
(
  uss_dcs_enum_v02 qmi_dcs
);

static qbi_svc_action_e qbi_svc_ussd_s_cancel_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_ussd_s_cancel_voice3c_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_ussd_s_continue_req
(
  qbi_txn_s   *txn,
  const uint8 *uss_data
);

static qbi_svc_action_e qbi_svc_ussd_s_continue_voice3b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_ussd_s_initiate_req
(
  qbi_txn_s   *txn,
  const uint8 *uss_data
);

static qbi_svc_action_e qbi_svc_ussd_s_initiate_voice43_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_ussd_s_req
(
  qbi_txn_s *txn
);

static void qbi_svc_ussd_s_timeout_cb
(
  qbi_txn_s *txn
);

static void qbi_svc_ussd_update_cache_pending_txn
(
  qbi_ctx_s *ctx,
  qbi_txn_s *txn
);

static void qbi_svc_ussd_update_cache_session_type
(
  qbi_ctx_s *ctx,
  uint32     session_type
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_ussd_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_ussd_cmd_hdlr_tbl[] = {
  {NULL, 0,
    qbi_svc_ussd_s_req, sizeof(qbi_svc_ussd_s_req_s)}
};

/*! @brief Static USSD indication handlers (generally, CID event handlers)
*/
static const qbi_svc_ind_info_s qbi_svc_ussd_static_ind_hdlrs[] = {
  QBI_SVC_USSD_STATIC_IND_HDLR(QBI_QMI_SVC_VOICE,
                               QMI_VOICE_USSD_IND_V02,
                               QBI_SVC_USSD_MBIM_CID_USSD,
                               qbi_svc_ussd_es_voice3e_ind_cb),
  QBI_SVC_USSD_STATIC_IND_HDLR(QBI_QMI_SVC_VOICE,
                               QMI_VOICE_ORIG_USSD_NO_WAIT_IND_V02,
                               QBI_SVC_USSD_MBIM_CID_USSD,
                               qbi_svc_ussd_es_voice43_ind_cb),
  QBI_SVC_USSD_STATIC_IND_HDLR(QBI_QMI_SVC_VOICE,
                               QMI_VOICE_USSD_RELEASE_IND_V02,
                               QBI_SVC_USSD_MBIM_CID_USSD,
                               qbi_svc_ussd_es_voice3d_ind_cb)
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_ussd_ascii_to_gsm_alphabet
===========================================================================*/
/*!
    @brief Converts an ASCII string into packed GSM default alphabet
    representation

    @details

    @param gsm_alphabet_string
    @param gsm_alphabet_string_size
    @param ascii_string
    @param num_chars Number of characters in the input ASCII string

    @return uint32 Number of bytes set in gsm_alphabet_string
*/
/*=========================================================================*/
static uint32 qbi_svc_ussd_ascii_to_gsm_alphabet
(
  uint8       *gsm_alphabet_string,
  uint32       gsm_alphabet_string_size,
  const uint8 *ascii_string,
  uint32       num_chars
)
{
  uint8 temp_buffer[QBI_SVC_USSD_UNPACKED_DATA_LEN_MAX];
  uint32 bytes_written = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(ascii_string);

  if (num_chars > QBI_SVC_USSD_UNPACKED_DATA_LEN_MAX)
  {
    QBI_LOG_E_2("Too many characters in input string! Given %d, max %d",
                num_chars, QBI_SVC_USSD_UNPACKED_DATA_LEN_MAX);
  }
  else
  {
    /* Convert from ASCII coding into GSM default-alphabet coding with 1 char
       per byte */
    QBI_MEMSCPY(temp_buffer, sizeof(temp_buffer), ascii_string, num_chars);
    (void) qbi_util_convert_ascii_to_gsm_alphabet(temp_buffer, num_chars);

    /* Now pack the string down to 7-bit format */
    bytes_written = qbi_util_7bit_pack(
      gsm_alphabet_string, gsm_alphabet_string_size, temp_buffer, num_chars,
      TRUE);
  }

  return bytes_written;
} /* qbi_svc_ussd_ascii_to_gsm_alphabet */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_get_cache
===========================================================================*/
/*!
    @brief Returns a pointer to the USSD service's
    cache

    @details

    @param ctx

    @return qbi_svc_ussd_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
static qbi_svc_ussd_cache_s *qbi_svc_ussd_get_cache
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_ussd_cache_s *) qbi_svc_cache_get(ctx, QBI_SVC_ID_USSD);
} /* qbi_svc_ussd_get_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_gsm_alphabet_to_ascii
===========================================================================*/
/*!
    @brief Converts a packed GSM default alphabet string into an ASCII string

    @details
    The resulting ASCII string is NOT NULL terminated.

    @param ascii_string
    @param ascii_string_size
    @param gsm_alphabet_string
    @param num_bytes Size of packed GSM default alphabet string in bytes

    @return uint32 Number of bytes set in ascii_string, i.e. number of
    characters in the resulting ASCII string
*/
/*=========================================================================*/
static uint32 qbi_svc_ussd_gsm_alphabet_to_ascii
(
  uint8        *ascii_string,
  uint32        ascii_string_size,
  const uint8  *gsm_alphabet_string,
  uint32        num_bytes
)
{
  uint32 num_chars = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(ascii_string);

  if (num_bytes > QBI_SVC_USSD_DATA_LEN_MAX)
  {
    QBI_LOG_E_2("Too many bytes in packed input string! Given %d, max %d",
                num_bytes, QBI_SVC_USSD_DATA_LEN_MAX);
  }
  else
  {
    /* Unpack the string from 7-bit format into 1 char per byte format */
    num_chars = qbi_util_7bit_unpack(
      ascii_string, ascii_string_size, gsm_alphabet_string, num_bytes, TRUE);

    /* Now convert from GSM default alphabet coding into ASCII coding */
    (void) qbi_util_convert_gsm_alphabet_to_ascii(ascii_string, num_chars);
  }

  return num_chars;
} /* qbi_svc_ussd_gsm_alphabet_to_ascii */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (qbi_svc_cache_alloc(
        txn->ctx, QBI_SVC_ID_USSD, sizeof(qbi_svc_ussd_cache_s)) == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate cache");
  }
  else if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_VOICE))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else if (!qbi_svc_ind_reg_static(
             txn->ctx, QBI_SVC_ID_USSD, qbi_svc_ussd_static_ind_hdlrs,
             ARR_SIZE(qbi_svc_ussd_static_ind_hdlrs)))
  {
    QBI_LOG_E_0("Couldn't register QMI indication handlers");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_ussd_open() */

/*! @addtogroup MBIM_CID_USSD
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_end_session
===========================================================================*/
/*!
    @brief Ends the current USSD session

    @details

    @param ctx

    @return boolean TRUE if an active session was ended, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_ussd_end_session
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_ussd_cache_s *cache;
  boolean ended = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ussd_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (cache->ussd.current_session_type != QBI_SVC_USSD_SESSION_TYPE_NONE)
  {
    QBI_LOG_I_0("Ended USSD session");
    qbi_svc_ussd_update_cache_session_type(ctx, QBI_SVC_USSD_SESSION_TYPE_NONE);
    ended = TRUE;
  }

  return ended;
} /* qbi_svc_ussd_end_session() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_end_session_if_mt_no_action
===========================================================================*/
/*!
    @brief Ends the current session if it is a MT session and we received
    data from the network with no user action required

    @details
    This should be called each time a USSD payload is received from the
    network, to ensure proper session management due to an inconsistency
    in the way NAS handles RELEASE_IND from the network in a MT session.
    If NAS is changed to always send RELEASE_IND, then this function should
    be removed.

    @param ctx
    @param ussd_response
*/
/*=========================================================================*/
static void qbi_svc_ussd_end_session_if_mt_no_action
(
  qbi_ctx_s *ctx,
  uint32     ussd_response
)
{
  qbi_svc_ussd_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ussd_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (ussd_response == QBI_SVC_USSD_RESPONSE_NO_ACTION_REQUIRED &&
      cache->ussd.current_session_type == QBI_SVC_USSD_SESSION_TYPE_MT)
  {
    QBI_LOG_I_0("Ended MT USSD session w/o release due to no user action");
    qbi_svc_ussd_update_cache_session_type(
      ctx, QBI_SVC_USSD_SESSION_TYPE_NONE);
  }
} /* qbi_svc_ussd_end_session_if_mt_no_action() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_es_voice3d_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_VOICE_USSD_RELEASE_IND, checking whether we should
    send a MBIM_CID_USSD notifying the host that the network ended the
    session

    @details
    This is a static indication handler

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_es_voice3d_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_txn_s *rsp_txn;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);

  QBI_LOG_I_0("Received USSD_RELEASE indication");
  rsp_txn = qbi_svc_ussd_get_rsp_txn(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp_txn);

  if (qbi_svc_ussd_end_session(rsp_txn->ctx))
  {
    action = qbi_svc_ussd_es_rsp(
      rsp_txn, QBI_SVC_USSD_RESPONSE_TERMINATED_BY_NW,
      QBI_SVC_USSD_SESSION_STATE_EXISTING, USS_DCS_ASCII_V02, 0, NULL);
  }
  else
  {
    QBI_LOG_I_0("No active session - no USSD response needed");
  }

  return qbi_svc_ussd_proc_pending_txn(rsp_txn->ctx, action);
} /* qbi_svc_ussd_es_voice3d_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_es_voice3e_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_VOICE_USSD_IND

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_es_voice3e_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const voice_ussd_ind_msg_v02 *qmi_ind;
  qbi_txn_s *rsp_txn;
  uint32 mbim_response;
  uint32 ussd_session_state;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  QBI_LOG_I_0("Received USSD indication");
  rsp_txn = qbi_svc_ussd_get_rsp_txn(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp_txn);

  qmi_ind = (const voice_ussd_ind_msg_v02 *) ind->buf->data;
  ussd_session_state = qbi_svc_ussd_get_session_state(rsp_txn->ctx);
  if (!qbi_svc_ussd_qmi_further_action_to_mbim_response(
        qmi_ind->notification_type, &mbim_response))
  {
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    /*! @note If this is a MT session and no user action is required, we need
        to terminate the session here as current NAS design does not send a
        RELEASE_IND */
    qbi_svc_ussd_end_session_if_mt_no_action(rsp_txn->ctx, mbim_response);

    if (qmi_ind->uss_info_valid)
    {
      action = qbi_svc_ussd_es_rsp(
        rsp_txn, mbim_response, ussd_session_state, qmi_ind->uss_info.uss_dcs,
        qmi_ind->uss_info.uss_data_len, qmi_ind->uss_info.uss_data);
    }
    else
    {
      action = qbi_svc_ussd_es_rsp(
        rsp_txn, mbim_response, ussd_session_state, USS_DCS_ASCII_V02, 0, NULL);
    }
  }

  return qbi_svc_ussd_proc_pending_txn(rsp_txn->ctx, action);
} /* qbi_svc_ussd_es_voice3e_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_es_voice43_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_VOICE_ORIG_USSD_NO_WAIT_IND to see if we need to
    send a MBIM_CID_USSD event notifying of the end of MO session

    @details
    This is a static indication handler

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_es_voice43_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_txn_s *rsp_txn;
  const voice_orig_ussd_no_wait_ind_msg_v02 *qmi_ind;
  uint32 response = QBI_SVC_USSD_RESPONSE_NO_ACTION_REQUIRED;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  QBI_LOG_I_0("Received ORIG_USSD_NO_WAIT indication");
  rsp_txn = qbi_svc_ussd_get_rsp_txn(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp_txn);

  qmi_ind = (const voice_orig_ussd_no_wait_ind_msg_v02 *) ind->buf->data;
  if (qmi_ind->error_valid)
  {
    /* There is no release indicator if QMI_VOICE_ORIG_USSD_NO_WAIT_IND
       contains error, reset session type here */
    qbi_svc_ussd_update_cache_session_type(
      rsp_txn->ctx, QBI_SVC_USSD_SESSION_TYPE_NONE);

    if (qmi_ind->error == QMI_ERR_SUPS_FAILURE_CAUSE_V01 &&
        qmi_ind->failure_cause_valid)
    {
      QBI_LOG_E_1("Failure cause %d", qmi_ind->failure_cause);
      response = qbi_svc_ussd_failure_cause_code_to_response_mapping(
        qmi_ind->failure_cause);
    }
    else if (qmi_ind->error == QMI_ERR_NETWORK_ABORTED_V01 ||
             qmi_ind->error == QMI_ERR_ABORTED_V01)
    {
      response = QBI_SVC_USSD_RESPONSE_TERMINATED_BY_NW;
    }
    else if (qmi_ind->error == QMI_ERR_NO_NETWORK_FOUND_V01 &&
             qbi_svc_bc_nas_is_registered(rsp_txn->ctx))
    {
      response = QBI_SVC_USSD_RESPONSE_OPERATION_NOT_SUPPORTED;
    }
    else
    {
      qbi_svc_ussd_qmi_set_mbim_error(
        rsp_txn, (qmi_error_type_v01) qmi_ind->error);
    }
  }

  if (qmi_ind->uss_info_valid)
  {
    action = qbi_svc_ussd_es_rsp(
      rsp_txn, response, QBI_SVC_USSD_SESSION_STATE_EXISTING,
      qmi_ind->uss_info.uss_dcs, qmi_ind->uss_info.uss_data_len,
      qmi_ind->uss_info.uss_data);
  }
  else
  {
    action = qbi_svc_ussd_es_rsp(
      rsp_txn, response, QBI_SVC_USSD_SESSION_STATE_EXISTING,
      USS_DCS_ASCII_V02, 0, NULL);
  }

  return qbi_svc_ussd_proc_pending_txn(rsp_txn->ctx, action);
} /* qbi_svc_ussd_es_voice43_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_es_rsp
===========================================================================*/
/*!
    @brief Allocates and populates a MBIM_CID_USSD set or event response

    @details

    @param txn
    @param ussd_response
    @param session_state
    @param qmi_data_coding_scheme
    @param qmi_uss_data_len Size of qmi_uss_data in bytes
    @param qmi_uss_data Buffer containing USSD payload in the format given
    by qmi_data_coding_scheme

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_es_rsp
(
  qbi_txn_s        *txn,
  uint32            ussd_response,
  uint32            session_state,
  uss_dcs_enum_v02  qmi_data_coding_scheme,
  uint32            qmi_uss_data_len,
  const uint8_t    *qmi_uss_data
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_ussd_rsp_s *rsp;
  uint32 packed_ussd_payload_len = 0;
  uint8 packed_ussd_payload[QBI_SVC_USSD_DATA_LEN_MAX];
  uint32 mbim_data_coding_scheme;
/*-------------------------------------------------------------------------*/
  mbim_data_coding_scheme = qbi_svc_ussd_qmi_to_mbim_dcs_mapping(
    qmi_data_coding_scheme);
  if (mbim_data_coding_scheme == QBI_SVC_USSD_3GPP_DCS_GSM_7BIT &&
      qmi_uss_data_len > QBI_SVC_USSD_UNPACKED_DATA_LEN_MAX)
  {
    QBI_LOG_E_1("Unpacked USSD data overflow: length %d", qmi_uss_data_len);
  }
  else if (mbim_data_coding_scheme != QBI_SVC_USSD_3GPP_DCS_GSM_7BIT &&
           qmi_uss_data_len > QBI_SVC_USSD_DATA_LEN_MAX)
  {
    QBI_LOG_E_1("Packed USSD data overflow: length %d", qmi_uss_data_len);
  }
  else if (qmi_uss_data_len > 0 && qmi_uss_data == NULL)
  {
    QBI_LOG_E_1("Unexpected NULL pointer with payload length %d", qmi_uss_data_len);
  }
  else
  {
    if (qmi_uss_data != NULL)
    {
      if (mbim_data_coding_scheme == QBI_SVC_USSD_3GPP_DCS_GSM_7BIT)
      {
        packed_ussd_payload_len = qbi_svc_ussd_ascii_to_gsm_alphabet(
          packed_ussd_payload, sizeof(packed_ussd_payload),
          qmi_uss_data, qmi_uss_data_len);
      }
      else
      {
        packed_ussd_payload_len = qmi_uss_data_len;
        QBI_MEMSCPY(packed_ussd_payload, sizeof(packed_ussd_payload),
                    qmi_uss_data, qmi_uss_data_len);
      }
    }

    rsp = (qbi_svc_ussd_rsp_s *) qbi_txn_alloc_rsp_buf(
      txn, sizeof(qbi_svc_ussd_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    rsp->ussd_session_state = session_state;
    rsp->ussd_response = ussd_response;
    rsp->ussd_data_coding_scheme = mbim_data_coding_scheme;
    if (packed_ussd_payload_len > 0 &&
        qbi_txn_rsp_databuf_add_field(
          txn, &rsp->ussd_payload, 0, packed_ussd_payload_len,
          packed_ussd_payload) == NULL)
    {
      QBI_LOG_E_0("Couldn't add payload to response!");
    }
    else
    {
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }

  return action;
} /* qbi_svc_ussd_es_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_failure_cause_code_to_response_mapping
===========================================================================*/
/*!
    @brief Returns the appropriate USSD response value based on failure
    cause code

    @details

    @param failure_cause

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_ussd_failure_cause_code_to_response_mapping
(
  qmi_sups_errors_enum_v02 failure_cause
)
{
  uint32 response = QBI_SVC_USSD_RESPONSE_TERMINATED_BY_NW;
/*-------------------------------------------------------------------------*/
  switch (failure_cause)
  {
    case QMI_FAILURE_CAUSE_FACILITY_NOT_SUPPORTED_V02:
    case QMI_FAILURE_CAUSE_BEARER_SERVICE_NOT_IMPLEMENTED_V02:
    case QMI_FAILURE_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED_V02:
    case QMI_FAILURE_CAUSE_SERVICE_OR_OPTION_NOT_IMPLEMENTED_V02:
      response = QBI_SVC_USSD_RESPONSE_OPERATION_NOT_SUPPORTED;
      break;

    case QMI_FAILURE_CAUSE_NO_RESPONSE_FROM_BS_V02:
    case QMI_FAILURE_CAUSE_TIMEOUT_T42_V02:
    case QMI_FAILURE_CAUSE_TIMEOUT_T41_V02:
    case QMI_FAILURE_CAUSE_TIMEOUT_T40_V02:
    case QMI_FAILURE_CAUSE_RL_ACK_TIMEOUT_V02:
    case QMI_FAILURE_CAUSE_RECOVERY_ON_TIMER_EXPIRED_V02:
    case QMI_FAILURE_CAUSE_TIMER_T3230_EXPIRED_V02:
    case QMI_FAILURE_CAUSE_NETWORK_NO_RESP_TIME_OUT_V02:
    case QMI_FAILURE_CAUSE_NETWORK_NO_RESP_HOLD_FAIL_V02:
    case QMI_FAILURE_CAUSE_NO_USER_RESPONDING_V02:
    case QMI_FAILURE_CAUSE_USER_ALERTING_NO_ANSWER_V02:
    case QMI_FAILURE_CAUSE_NO_NETWORK_RESP_V02:
      response = QBI_SVC_USSD_RESPONSE_NETWORK_TIMEOUT;
      break;

    default:
      break;
  }

  return response;
} /* qbi_svc_ussd_failure_cause_code_to_response_mapping */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_get_rsp_txn
===========================================================================*/
/*!
    @brief Returns the transaction that should be associated with an outgoing
    USSD message

    @details
    If a transaction is pending (i.e. a set request), then this returns the
    pending transaction, as USSD payload data should be associated with the
    request that triggered it. If there is no pending transaction, then
    USSD payload data should be sent out as an event, using the transaction
    passed in.

    @param event_txn

    @return qbi_txn_s*
*/
/*=========================================================================*/
static qbi_txn_s *qbi_svc_ussd_get_rsp_txn
(
  qbi_txn_s *event_txn
)
{
  qbi_txn_s *rsp_txn;
  qbi_svc_ussd_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(event_txn);

  cache = qbi_svc_ussd_get_cache(event_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_NULL(cache);

  if (cache->ussd.pending_txn == NULL)
  {
    QBI_LOG_I_0("No pending transaction, send response as an event");
    rsp_txn = event_txn;
  }
  else
  {
    QBI_LOG_I_0("Use pending transaction for response");
    rsp_txn = cache->ussd.pending_txn;
  }

  return rsp_txn;
} /* qbi_svc_ussd_get_rsp_txn() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_get_session_state
===========================================================================*/
/*!
    @brief Gets the current USSDSessionState

    @details
    Per MBIM, USSDSessionState is only "New" for the first message from a
    MT session; otherwise it is "Existing".  If this function is called
    when there is no active session, then a new MT USSD session is assumed.

    @param ctx

    @return uint32 USSDSessionState value
*/
/*=========================================================================*/
static uint32 qbi_svc_ussd_get_session_state
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_ussd_cache_s *cache;
  uint32 ussd_session_state = QBI_SVC_USSD_SESSION_STATE_EXISTING;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ussd_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  if (cache->ussd.current_session_type == QBI_SVC_USSD_SESSION_TYPE_NONE)
  {
    QBI_LOG_I_0("Starting new MT USSD session");
    ussd_session_state = QBI_SVC_USSD_SESSION_STATE_NEW;
    qbi_svc_ussd_update_cache_session_type(
      ctx, QBI_SVC_USSD_SESSION_TYPE_MT);
  }
  else
  {
    QBI_LOG_I_1("Using existing session with type %d",
                cache->ussd.current_session_type);
  }

  return ussd_session_state;
} /* qbi_svc_ussd_get_session_state() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_mbim_to_qmi_dcs_mapping
===========================================================================*/
/*!
    @brief Map MBIM DCS to QMI DCS

    @details

    @param uint32
    @return uss_dcs_enum_v02
*/
/*=========================================================================*/
static uss_dcs_enum_v02 qbi_svc_ussd_mbim_to_qmi_dcs_mapping
(
  uint32 mbim_dcs
)
{
  uss_dcs_enum_v02 qmi_dcs;
  uint8 hi_dcs, lo_dcs;
/*-------------------------------------------------------------------------*/

  hi_dcs = ((mbim_dcs & 0xF0) >> 4);
  lo_dcs = (mbim_dcs & 0x0F);

  /* Refer to 3GPP TS 23.038 V6.1.0 , Section 5 : CBS Data Coding Scheme */
  /* GSM 7-bit alphabet */
  if ((hi_dcs == 0x00) ||
      ((hi_dcs == 0x01) &&
       (lo_dcs == QBI_SVC_USSD_DCS_7_BIT_MASK)) ||
      (hi_dcs == 0x02) ||
      (hi_dcs == 0x03) ||
      ((hi_dcs & 0x04) &&
       ((lo_dcs & 0x0C) == QBI_SVC_USSD_DCS_7_BIT_MASK)) ||
      ((hi_dcs == 0x09) &&
       ((lo_dcs & 0x0C) == QBI_SVC_USSD_DCS_7_BIT_MASK)) ||
      ((hi_dcs == 0x0F) &&
       ((lo_dcs & 0x04) == QBI_SVC_USSD_DCS_7_BIT_MASK)))
  {
    /* Convert from GSM 7-bit alphabet to UTF8 */
    QBI_LOG_I_2("USSD DCS 7-bit str, hi=%d lo=%d\n", hi_dcs, lo_dcs);
    qmi_dcs = USS_DCS_ASCII_V02;
  }
  /* 8-bit */
  else if (((hi_dcs & 0x04) &&
            ((lo_dcs & 0x0C) == QBI_SVC_USSD_DCS_8_BIT_MASK)) ||
           ((hi_dcs == 0x09) &&
            ((lo_dcs & 0x0C) == QBI_SVC_USSD_DCS_8_BIT_MASK)) ||
           ((hi_dcs == 0x0F) &&
            ((lo_dcs & 0x04) == QBI_SVC_USSD_DCS_8_BIT_MASK)))
  {
    /* Convert from GSM 8-bit alphabet to UTF8 */
    QBI_LOG_I_2("USSD DCS 8-bit str, hi=%d lo=%d\n", hi_dcs, lo_dcs);
    qmi_dcs = USS_DCS_8BIT_V02;
  }
  /* UCS2 preceded by two GSM 7-bit alphabet */
  else if ((hi_dcs == 0x01) &&
           (lo_dcs == QBI_SVC_USSD_DCS_UCS2_LANG_IND_MASK))
  {
    QBI_LOG_I_2("USSD DCS DCS2 str, hi=%d lo=%d\n", hi_dcs, lo_dcs);
    qmi_dcs = USS_DCS_UCS2_V02;
  }
  /* UCS2 */
  else if (((hi_dcs & 0x04) &&
            ((lo_dcs & 0x0C) == QBI_SVC_USSD_DCS_UCS2_MASK)) ||
           ((hi_dcs == 0x09) &&
            ((lo_dcs & 0x0C) == QBI_SVC_USSD_DCS_UCS2_MASK)))
  {
    QBI_LOG_I_2("USSD DCS DCS2 str, hi=%d lo=%d\n", hi_dcs, lo_dcs);
    qmi_dcs = USS_DCS_UCS2_V02;
  }
  else
  {
    QBI_LOG_I_2("USSD DCS Unknown str format, hi=%d lo=%d\n",
                hi_dcs, lo_dcs);
    qmi_dcs = USS_DCS_ASCII_V02;
  }

  QBI_LOG_I_2("USSD MBIM to QMI DCS map %d to %d", mbim_dcs, qmi_dcs);

  return qmi_dcs;
} /* qbi_svc_ussd_mbim_to_qmi_dcs_mapping() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_proc_pending_txn
===========================================================================*/
/*!
    @brief Processes the given action on the pending transaction

    @details

    @param ctx
    @param action Action to process on the response transaction

    @return qbi_svc_action_e Action to process on the event transaction,
    i.e. the action that an indication handler should return
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_proc_pending_txn
(
  qbi_ctx_s       *ctx,
  qbi_svc_action_e action
)
{
  qbi_svc_ussd_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ussd_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (cache->ussd.pending_txn != NULL)
  {
    QBI_LOG_I_2("Processing action %d on pending_txn iid %d",
                action, cache->ussd.pending_txn->iid);
    (void) qbi_svc_proc_action(cache->ussd.pending_txn, action);
    qbi_svc_ussd_update_cache_pending_txn(ctx, NULL);
    action = QBI_SVC_ACTION_ABORT;
  }

  return action;
} /* qbi_svc_ussd_proc_pending_txn() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_qmi_further_action_to_mbim_response
===========================================================================*/
/*!
    @brief Maps a QMI further user action enum to MBIM USSDResponse

    @details

    @param qmi_further_action
    @param mbim_response

    @return boolean TRUE if successfully mapped, FALSE on error
*/
/*=========================================================================*/
static boolean qbi_svc_ussd_qmi_further_action_to_mbim_response
(
  further_user_action_enum_v02 qmi_further_action,
  uint32                      *mbim_response
)
{
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(mbim_response);

  switch (qmi_further_action)
  {
    case FURTHER_USER_ACTION_NOT_REQUIRED_V02:
      QBI_LOG_I_0("USSD no user action required");
      *mbim_response = QBI_SVC_USSD_RESPONSE_NO_ACTION_REQUIRED;
      break;

    case FURTHER_USER_ACTION_REQUIRED_V02:
      QBI_LOG_I_0("USSD user action required");
      *mbim_response = QBI_SVC_USSD_RESPONSE_ACTION_REQUIRED;
      break;

    default:
      QBI_LOG_E_1("QMI gave invalid further action value %d",
                  qmi_further_action);
      success = FALSE;
  }

  return success;
} /* qbi_svc_ussd_qmi_further_action_to_mbim_response() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_qmi_set_mbim_error
===========================================================================*/
/*!
    @brief Map QMI error code to MBIM error code, also taking into account
    current device state from the basic connectivity service

    @details

    @param txn
    @param qmi_error_type_v01

    @return void
*/
/*=========================================================================*/
static void qbi_svc_ussd_qmi_set_mbim_error
(
  qbi_txn_s          *txn,
  qmi_error_type_v01  qmi_error
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  /* Check device state first when there is a QMI error */
  if (qmi_error != QMI_ERR_NONE_V01 &&
      !qbi_svc_bc_check_device_state(txn, TRUE, TRUE))
  {
    QBI_LOG_E_1("Override MBIM error with %d if device is not ready",
                txn->status);
    return;
  }

  switch (qmi_error)
  {
    case QMI_ERR_NONE_V01:
      txn->status = QBI_MBIM_STATUS_SUCCESS;
      break;

    case QMI_ERR_NO_NETWORK_FOUND_V01:
    case QMI_ERR_OP_NETWORK_UNSUPPORTED_V01:
      txn->status = QBI_MBIM_STATUS_NOT_REGISTERED;
      break;

    case QMI_ERR_INVALID_ARG_V01:
      txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
      break;

    default:
      txn->status = QBI_MBIM_STATUS_FAILURE;
      break;
  }

  QBI_LOG_I_2("USSD error map 0x%x to 0x%x", qmi_error, txn->status);
} /* qbi_svc_ussd_qmi_set_mbim_error() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_qmi_to_mbim_dcs_mapping
===========================================================================*/
/*!
    @brief Map QMI DCS to MBIM DCS

    @details

    @param uss_dcs_enum_v02
    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_ussd_qmi_to_mbim_dcs_mapping
(
  uss_dcs_enum_v02 qmi_dcs
)
{
  uint32 mbim_dcs;
/*-------------------------------------------------------------------------*/
  switch (qmi_dcs)
  {
    case USS_DCS_ASCII_V02:
      mbim_dcs = QBI_SVC_USSD_3GPP_DCS_GSM_7BIT;
      break;

    case USS_DCS_8BIT_V02:
      mbim_dcs = QBI_SVC_USSD_3GPP_DCS_8BIT;
      break;

    case USS_DCS_UCS2_V02:
      mbim_dcs = QBI_SVC_USSD_3GPP_DCS_UCS2;
      break;

    default:
      QBI_LOG_E_0("Coding scheme unknown.");
      mbim_dcs = QBI_SVC_USSD_3GPP_DCS_GSM_7BIT;
      break;
  }

  QBI_LOG_I_2("USSD QMI to MBIM DCS map %d to %d", qmi_dcs, mbim_dcs);

  return mbim_dcs;
} /* qbi_svc_ussd_qmi_to_mbim_dcs_mapping() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_s_cancel_req
===========================================================================*/
/*!
    @brief Handles a request to cancel a USSD session

    @details

    @param txn
    @param uss_data

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_s_cancel_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_ussd_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->ctx);

  QBI_LOG_I_0("USSD cancel request");
  cache = qbi_svc_ussd_get_cache(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (cache->ussd.current_session_type == QBI_SVC_USSD_SESSION_TYPE_NONE)
  {
    QBI_LOG_E_0("No USSD session to cancel");
  }
  else
  {
    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_VOICE,
                                QMI_VOICE_CANCEL_USSD_REQ_V02,
                                qbi_svc_ussd_s_cancel_voice3c_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    /* We can only have one transaction waiting for a response at a time. If
       one already exists, send an error response now so we can process the
       cancel. */
    (void) qbi_svc_ussd_proc_pending_txn(txn->ctx, QBI_SVC_ACTION_ABORT);
    qbi_svc_ussd_update_cache_pending_txn(txn->ctx, txn);
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_ussd_s_cancel_req() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_s_cancel_voice3c_rsp_cb
===========================================================================*/
/*!
    @brief Process QMI_VOICE_USSD_CANCEL_RESP for MBIM_CID_USSD set
    containing a request to cancel a USSD session

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_s_cancel_voice3c_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  voice_cancel_ussd_resp_msg_v02 *qmi_rsp;
  qbi_svc_ussd_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  cache = qbi_svc_ussd_get_cache(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  QBI_LOG_I_0("USSD cancel RESP callback");
  qmi_rsp = (voice_cancel_ussd_resp_msg_v02 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_ussd_qmi_set_mbim_error(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_0("USSD cancel succesful; waiting on No_WAIT_IND/RELEASE_IND");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_ussd_s_cancel_voice3c_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_s_continue_req
===========================================================================*/
/*!
    @brief Handles a request to contnue a USSD session

    @details

    @param txn
    @param uss_data

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_s_continue_req
(
  qbi_txn_s   *txn,
  const uint8 *uss_data
)
{
  qbi_svc_ussd_s_req_s *req;
  qbi_svc_ussd_cache_s *cache;
  voice_answer_ussd_req_msg_v02 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(uss_data);

  cache = qbi_svc_ussd_get_cache(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  QBI_LOG_I_0("USSD continue request");
  req = (qbi_svc_ussd_s_req_s *) txn->req.data;
  if (cache->ussd.current_session_type == QBI_SVC_USSD_SESSION_TYPE_NONE)
  {
    QBI_LOG_E_0("No USSD session to answer");
  }
  else if (cache->ussd.pending_txn != NULL)
  {
    QBI_LOG_E_0("Cannot answer USSD during pending transaction");
    txn->status = QBI_MBIM_STATUS_BUSY;
  }
  else
  {
    qmi_req = (voice_answer_ussd_req_msg_v02 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_VOICE, QMI_VOICE_ANSWER_USSD_REQ_V02,
        qbi_svc_ussd_s_continue_voice3b_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->uss_info.uss_dcs = qbi_svc_ussd_mbim_to_qmi_dcs_mapping(
      req->ussd_data_coding_scheme);
    if (qmi_req->uss_info.uss_dcs == USS_DCS_ASCII_V02)
    {
      qmi_req->uss_info.uss_data_len = qbi_svc_ussd_gsm_alphabet_to_ascii(
        qmi_req->uss_info.uss_data, sizeof(qmi_req->uss_info.uss_data),
        uss_data, req->ussd_payload.size);
    }
    else
    {
      qmi_req->uss_info.uss_data_len = req->ussd_payload.size;
      QBI_MEMSCPY(qmi_req->uss_info.uss_data, sizeof(qmi_req->uss_info.uss_data),
                  uss_data, req->ussd_payload.size);
    }

    /* Save txn into cache for later use */
    qbi_svc_ussd_update_cache_pending_txn(txn->ctx, txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_ussd_s_continue_req() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_s_continue_voice3b_rsp_cb
===========================================================================*/
/*!
    @brief Process QMI_VOICE_USSD_ANSWER_RESP for MBIM_CID_USSD set
    containing a request to continue a USSD session

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_s_continue_voice3b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  voice_answer_ussd_resp_msg_v02 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  QBI_LOG_I_0("USSD answer RESP callback");
  qmi_rsp = (voice_answer_ussd_resp_msg_v02 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_ussd_qmi_set_mbim_error(qmi_txn->parent, qmi_rsp->resp.error);
    qbi_svc_ussd_update_cache_pending_txn(qmi_txn->ctx, NULL);
  }
  else
  {
    QBI_LOG_I_0("USSD answer succesful. Waiting on indication");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_ussd_s_continue_voice3b_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_s_initiate_req
===========================================================================*/
/*!
    @brief Handles a request to initiate a USSD session

    @details

    @param txn
    @param uss_data

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_s_initiate_req
(
  qbi_txn_s   *txn,
  const uint8 *uss_data
)
{
  qbi_svc_ussd_s_req_s *req;
  qbi_svc_ussd_cache_s *cache;
  voice_orig_ussd_no_wait_req_msg_v02 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(uss_data);

  cache = qbi_svc_ussd_get_cache(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  QBI_LOG_I_0("USSD origination request");
  req = (qbi_svc_ussd_s_req_s *) txn->req.data;
  if (cache->ussd.current_session_type != QBI_SVC_USSD_SESSION_TYPE_NONE)
  {
    QBI_LOG_E_1("USSD session is already in progress, type %d",
                cache->ussd.current_session_type);
    txn->status = QBI_MBIM_STATUS_BUSY;
  }
  else
  {
    qmi_req = (voice_orig_ussd_no_wait_req_msg_v02 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_VOICE, QMI_VOICE_ORIG_USSD_NO_WAIT_REQ_V02,
        qbi_svc_ussd_s_initiate_voice43_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->uss_info.uss_dcs = qbi_svc_ussd_mbim_to_qmi_dcs_mapping(
      req->ussd_data_coding_scheme);
    if (qmi_req->uss_info.uss_dcs == USS_DCS_ASCII_V02)
    {
      qmi_req->uss_info.uss_data_len = qbi_svc_ussd_gsm_alphabet_to_ascii(
        qmi_req->uss_info.uss_data, sizeof(qmi_req->uss_info.uss_data),
        uss_data, req->ussd_payload.size);
    }
    else
    {
      qmi_req->uss_info.uss_data_len = req->ussd_payload.size;
      QBI_MEMSCPY(qmi_req->uss_info.uss_data, sizeof(qmi_req->uss_info.uss_data),
                  uss_data, req->ussd_payload.size);
    }

    /* Save txn into cache for later use */
    qbi_svc_ussd_update_cache_pending_txn(txn->ctx, txn);
    qbi_svc_ussd_update_cache_session_type(
        txn->ctx, QBI_SVC_USSD_SESSION_TYPE_MO);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_ussd_s_initiate_req() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_s_initiate_voice43_rsp_cb
===========================================================================*/
/*!
    @brief Process QMI_VOICE_ORIG_USSD_NO_WAIT_RESP for MBIM_CID_USSD set
    containing a request to initiate a USSD session

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_s_initiate_voice43_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  voice_orig_ussd_no_wait_resp_msg_v02 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);

  QBI_LOG_I_0("USSD no wait origination RESP callback");
  qmi_rsp = (voice_orig_ussd_no_wait_resp_msg_v02 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    qbi_svc_ussd_update_cache_session_type(
      qmi_txn->ctx, QBI_SVC_USSD_SESSION_TYPE_NONE);
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_ussd_qmi_set_mbim_error(qmi_txn->parent, qmi_rsp->resp.error);
    qbi_svc_ussd_update_cache_pending_txn(qmi_txn->ctx, NULL);
  }
  else
  {
    QBI_LOG_I_0("USSD no wait origination succesful; waiting on indication");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_ussd_s_initiate_voice43_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_USSD set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ussd_s_req
(
  qbi_txn_s *txn
)
{
  const uint8 *uss_data = NULL;
  qbi_svc_ussd_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_ussd_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  QBI_LOG_I_0("USSD set request");

  cache = qbi_svc_ussd_get_cache(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  req = (qbi_svc_ussd_s_req_s *) txn->req.data;
  if (req->ussd_action != QBI_SVC_USSD_ACTION_CANCEL)
  {
    uss_data = (const uint8 *) qbi_txn_req_databuf_get_field(
      txn, &req->ussd_payload, 0, QBI_SVC_USSD_DATA_LEN_MAX);
    QBI_CHECK_NULL_PTR_RET_ABORT(uss_data);
  }

  if (req->ussd_action == QBI_SVC_USSD_ACTION_INITIATE)
  {
    action = qbi_svc_ussd_s_initiate_req(txn, uss_data);
  }
  else if (req->ussd_action == QBI_SVC_USSD_ACTION_CONTINUE)
  {
    action = qbi_svc_ussd_s_continue_req(txn, uss_data);
  }
  else if (req->ussd_action == QBI_SVC_USSD_ACTION_CANCEL)
  {
    action = qbi_svc_ussd_s_cancel_req(txn);
  }
  else
  {
    QBI_LOG_E_1("Unrecognized USSD action %d", req->ussd_action);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }

  /* Set timeout in case there is no response from network */
  qbi_txn_set_timeout(txn, QBI_SVC_USSD_SET_TIMEOUT_MS,
                      qbi_svc_ussd_s_timeout_cb);

  return action;
} /* qbi_svc_ussd_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_s_timeout_cb
===========================================================================*/
/*!
    @brief Invoked when network has not responded to the USSD set request
    after a period of time

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_ussd_s_timeout_cb
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_LOG_E_1("Timeout while waiting for USSD response from network! "
              "txn iid %d", txn->iid);
  qbi_txn_set_timeout(txn, QBI_TXN_TIMEOUT_DEFAULT_MS, NULL);

  /* Clear session status*/
  qbi_svc_ussd_update_cache_pending_txn(txn->ctx, NULL);
  qbi_svc_ussd_update_cache_session_type(
    txn->ctx, QBI_SVC_USSD_SESSION_TYPE_NONE);

  action = qbi_svc_ussd_es_rsp(
    txn, QBI_SVC_USSD_RESPONSE_NETWORK_TIMEOUT,
    QBI_SVC_USSD_SESSION_STATE_EXISTING, USS_DCS_ASCII_V02, 0, NULL);

  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_ussd_s_timeout_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_update_cache_pending_txn
===========================================================================*/
/*!
    @brief Updates the ussd cached information associated with
    pending transaction

    @details

    @param ctx
    @param txn Pending transaction
*/
/*=========================================================================*/
static void qbi_svc_ussd_update_cache_pending_txn
(
  qbi_ctx_s *ctx,
  qbi_txn_s *txn
)
{
  qbi_svc_ussd_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ussd_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  QBI_LOG_I_2("Set USSD pending transaction from 0x%x to 0x%x",
              cache->ussd.pending_txn, txn);
  cache->ussd.pending_txn = txn;
} /* qbi_svc_ussd_update_cache_pending_txn() */

/*===========================================================================
  FUNCTION: qbi_svc_ussd_update_cache_session_type
===========================================================================*/
/*!
    @brief Updates the ussd cached information associated with
    current session

    @details

    @param ctx
    @param session_type New session type
*/
/*=========================================================================*/
static void qbi_svc_ussd_update_cache_session_type
(
  qbi_ctx_s *ctx,
  uint32     session_type
)
{
  qbi_svc_ussd_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ussd_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  QBI_LOG_I_2("Set USSD session type from %d to %d",
              cache->ussd.current_session_type,
              session_type);
  cache->ussd.current_session_type = session_type;
} /* qbi_svc_ussd_update_cache_session_type() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_ussd_init
===========================================================================*/
/*!
    @brief One-time initialization of the USSD device service

    @details
*/
/*=========================================================================*/
void qbi_svc_ussd_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_ussd_cfg = {
    {
      0xe5, 0x50, 0xa0, 0xc8, 0x5e, 0x82, 0x47, 0x9e,
      0x82, 0xf7, 0x10, 0xab, 0xf4, 0xc3, 0x35, 0x1f
    },
    QBI_SVC_ID_USSD,
    FALSE,
    qbi_svc_ussd_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_ussd_cmd_hdlr_tbl),
    qbi_svc_ussd_open,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_ussd_cfg);
} /* qbi_svc_ussd_init() */

