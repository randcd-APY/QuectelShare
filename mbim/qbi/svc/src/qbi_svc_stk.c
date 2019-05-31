/*!
  @file
  qbi_svc_stk.c

  @brief
  PRELIMINARY STK device service implementation

  @note
  STK service relies on QMI CAT service to receive proactive command from SIM
  and deliver terminal response and envelope command to SIM. Proactive commands
  and envelope commands supports are characterized in serapate static tables
  based on GSTK terminal profile, but runtime tables will be generated in next
  phase once QMI CAT terminal profile command is available.

  Current limitations:
  1. Only unit tested. Need further integration test with host STK test suite
     once available.
  2. TR (Terminal response) response raw data is not supported. Status words
     are hardcoded for sucess case only. To be added once terminal response
     optional TLV is available from QMI.
  3. Not all proactive commands listed in ETSITS102223900 9.4 are supported
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
01/13/12  hz   Initial release based on MBIM v1.0
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_stk.h"
#include "qbi_svc_stk_mbim.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_qmi_defs.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_svc_bc.h"
#include "qbi_txn.h"

#include "card_application_toolkit_v02.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/* Type of command values defined in ETSITS102223900 9.4 */
#define QBI_SVC_STK_PAC_VAL_NONE                              (00)
#define QBI_SVC_STK_PAC_VAL_REFRESH                           (01)
#define QBI_SVC_STK_PAC_VAL_MORE_TIME                         (02)
#define QBI_SVC_STK_PAC_VAL_POLL_INTERVAL                     (03)
#define QBI_SVC_STK_PAC_VAL_POLLING_OFF                       (04)
#define QBI_SVC_STK_PAC_VAL_SET_UP_EVENT_LIST                 (05)
#define QBI_SVC_STK_PAC_VAL_SET_UP_CALL                       (10)
#define QBI_SVC_STK_PAC_VAL_SEND_SS                           (11)
#define QBI_SVC_STK_PAC_VAL_SEND_USSD                         (12)
#define QBI_SVC_STK_PAC_VAL_SEND_SHORT_MESSAGE                (13)
#define QBI_SVC_STK_PAC_VAL_SEND_DTMF                         (14)
#define QBI_SVC_STK_PAC_VAL_LAUNCH_BROWSER                    (15)
#define QBI_SVC_STK_PAC_VAL_GEOGRAPHICAL_LOCATION_REQUEST     (16)
#define QBI_SVC_STK_PAC_VAL_PLAY_TONE                         (20)
#define QBI_SVC_STK_PAC_VAL_DISPLAY_TEXT                      (21)
#define QBI_SVC_STK_PAC_VAL_GET_INKEY                         (22)
#define QBI_SVC_STK_PAC_VAL_GET_INPUT                         (23)
#define QBI_SVC_STK_PAC_VAL_SELECT_ITEM                       (24)
#define QBI_SVC_STK_PAC_VAL_SET_UP_MENU                       (25)
#define QBI_SVC_STK_PAC_VAL_PROVIDE_LOCAL_INFORMATION         (26)
#define QBI_SVC_STK_PAC_VAL_TIMER_MANAGEMENT                  (27)
#define QBI_SVC_STK_PAC_VAL_SET_UP_IDLE_MODE_TEXT             (28)
#define QBI_SVC_STK_PAC_VAL_PERFORM_CARD_APDU                 (30)
#define QBI_SVC_STK_PAC_VAL_POWER_ON_CARD                     (31)
#define QBI_SVC_STK_PAC_VAL_POWER_OFF_CARD                    (32)
#define QBI_SVC_STK_PAC_VAL_GET_READER_STATUS                 (33)
#define QBI_SVC_STK_PAC_VAL_RUN_AT_COMMAND                    (34)
#define QBI_SVC_STK_PAC_VAL_LANGUAGE_NOTIFICATION             (35)
#define QBI_SVC_STK_PAC_VAL_OPEN_CHANNEL                      (40)
#define QBI_SVC_STK_PAC_VAL_CLOSE_CHANNEL                     (41)
#define QBI_SVC_STK_PAC_VAL_RECEIVE_DATA                      (42)
#define QBI_SVC_STK_PAC_VAL_SEND_DATA                         (43)
#define QBI_SVC_STK_PAC_VAL_GET_CHANNEL_STATUS                (44)
#define QBI_SVC_STK_PAC_VAL_SERVICE_SEARCH                    (45)
#define QBI_SVC_STK_PAC_VAL_GET_SERVICE_INFORMATION           (46)
#define QBI_SVC_STK_PAC_VAL_DECLARE_SERVICE                   (47)
#define QBI_SVC_STK_PAC_VAL_SET_FRAMES                        (50)
#define QBI_SVC_STK_PAC_VAL_GET_FRAMES_STATUS                 (51)
#define QBI_SVC_STK_PAC_VAL_RETRIEVE_MULTIMEDIA_MESSAGE       (60)
#define QBI_SVC_STK_PAC_VAL_SUBMIT_MULTIMEDIA_MESSAGE         (61)
#define QBI_SVC_STK_PAC_VAL_DISPLAY_MULTIMEDIA_MESSAGE        (62)
#define QBI_SVC_STK_PAC_VAL_ACTIVATE                          (70)
#define QBI_SVC_STK_PAC_VAL_END_OF_THE_PROACTIVE_SESSION      (81)

/* Envelope command tag value defined in ETSITS101220920 */
#define QBI_SVC_STK_ENVELOPE_SMS_PP_DOWNLOAD_TAG              (0xD1)
#define QBI_SVC_STK_ENVELOPE_CELL_BROADCAST_DOWNLOAD_TAG      (0xD2)
#define QBI_SVC_STK_ENVELOPE_MENU_SELECTION_TAG               (0xD3)
#define QBI_SVC_STK_ENVELOPE_CALL_CONTROL_TAG                 (0xD4)
#define QBI_SVC_STK_ENVELOPE_MO_SHORT_MESSAGE_CONTROL_TAG     (0xD5)
#define QBI_SVC_STK_ENVELOPE_EVENT_DOWNLOAD_TAG               (0xD6)
#define QBI_SVC_STK_ENVELOPE_TIMER_EXPIRY_TAG                 (0xD7)

#define QBI_SVC_STK_PAC_PROFILE_FUNC_NONE                 (0)
#define QBI_SVC_STK_PAC_PROFILE_DISPLAY_FUNC_ONLY         (1)
#define QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY           (2)
#define QBI_SVC_STK_PAC_PROFILE_ALL_FUNC                  (3)

/*! This macro statically defines a QMI indication handler and fills in the
    fields that are common to all handlers in this device service */
#define QBI_SVC_STK_STATIC_IND_HDLR(qmi_svc_id, qmi_msg_id, cid, cb) \
  {qmi_svc_id, qmi_msg_id, cid, cb, NULL}

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! @brief STK service local cache struct
*/
typedef struct {
  struct {
    uint8  pac_host_control[QBI_SVC_STK_PAC_HOST_CONTROL_LEN];
    uint32 uim_ref_id;
  } stk;
} qbi_svc_stk_cache_s;

/* Proactive command profile type */
typedef struct{
  uint8  pac_val;
  uint32 qmi_event_mask;
  uint32 default_profile;
} qbi_svc_stk_pac_profile_s;

/* Envelope command profile type */
typedef struct{
  uint8   envelope_tag;
  boolean supported;
} qbi_svc_stk_envelope_profile_s;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_stk_cache_s *qbi_svc_stk_get_cache
(
  const qbi_ctx_s *ctx
);

static boolean qbi_svc_stk_init_cache
(
  qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_stk_open
(
  qbi_txn_s *txn
);

static boolean qbi_svc_stk_update_uim_ref_id
(
  qbi_ctx_s *ctx,
  uint32     ref_id
);

/*! @addtogroup MBIM_CID_STK_PAC
    @{ */
static uint8 qbi_svc_stk_pac_default_profile_to_current_profile
(
  uint32  default_profile,
  boolean host_requested,
  boolean event_report_set
);

static qbi_svc_action_e qbi_svc_stk_pac_e_build_rsp
(
  qbi_txn_s                          *txn,
  const cat_event_report_ind_msg_v02 *qmi_ind
);

static qbi_svc_action_e qbi_svc_stk_pac_e_cat01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static boolean qbi_svc_stk_pac_host_control_to_qmi_evt_report_mask
(
  qbi_svc_stk_pac_s_req_s    *req,
  uint32_t                   *evt_report_mask
);

static qbi_svc_action_e qbi_svc_stk_pac_q_cat20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_stk_pac_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_stk_pac_qs_build_rsp
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_stk_pac_s_cat01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_stk_pac_s_req
(
  qbi_txn_s *txn
);
/*! @} */

/*! @addtogroup MBIM_CID_STK_TERMINAL_RESPONSE
    @{ */
static qbi_svc_action_e qbi_svc_stk_terminal_response_s_cat21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_stk_terminal_response_s_req
(
  qbi_txn_s *txn
);
/*! @} */

/*! @addtogroup MBIM_CID_STK_ENVELOPE
    @{ */
static qbi_svc_action_e qbi_svc_stk_envelope_q_build_rsp
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_stk_envelope_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_stk_envelope_s_cat22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_stk_envelope_s_req
(
  qbi_txn_s *txn
);
/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_stk_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_stk_cmd_hdlr_tbl[] = {
  {qbi_svc_stk_pac_q_req, 0,
     qbi_svc_stk_pac_s_req, sizeof(qbi_svc_stk_pac_s_req_s)},
  {NULL, 0,
     qbi_svc_stk_terminal_response_s_req,
       sizeof(qbi_svc_stk_terminal_response_s_req_s)},
  {qbi_svc_stk_envelope_q_req, 0,
     qbi_svc_stk_envelope_s_req, sizeof(qbi_svc_stk_envelope_s_req_s)}
};

/*! @brief Static STK indication handlers (generally, CID event handlers)
*/
static const qbi_svc_ind_info_s qbi_svc_stk_static_ind_hdlrs[] = {
  QBI_SVC_STK_STATIC_IND_HDLR(QBI_QMI_SVC_CAT,
                              QMI_CAT_EVENT_REPORT_IND_V02,
                              QBI_SVC_STK_MBIM_CID_STK_PAC,
                              qbi_svc_stk_pac_e_cat01_ind_cb)
};

/*! @todo To be removed. Use terminal profile query to retrive command profiles
    once available through QMI */
/*! @brief Proactive command profile table
*/
static const qbi_svc_stk_pac_profile_s qbi_svc_stk_pac_profile_tbl[] = {
  {QBI_SVC_STK_PAC_VAL_NONE,
     0,
     QBI_SVC_STK_PAC_PROFILE_FUNC_NONE},
  {QBI_SVC_STK_PAC_VAL_REFRESH,
     QMI_CAT_EVT_REPORT_REQ_REFRESH_MASK_V02,
     /*! Refresh command is supported through QMI but not in raw */
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_MORE_TIME,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_POLL_INTERVAL,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_POLLING_OFF,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_SET_UP_EVENT_LIST,
     QMI_CAT_EVT_REPORT_REQ_SETUP_EVENT_USER_ACTIVITY_MASK_V02 |
       QMI_CAT_EVT_REPORT_REQ_SETUP_EVENT_IDLE_SCREEN_NOTIF_MASK_V02 |
       QMI_CAT_EVT_REPORT_REQ_SETUP_EVENT_LANGUAGE_SEL_NOTIF_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_SET_UP_CALL,
     QMI_CAT_EVT_REPORT_REQ_SETUP_CALL_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_DISPLAY_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_SEND_SS,
     QMI_CAT_EVT_REPORT_REQ_SEND_SS_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_DISPLAY_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_SEND_USSD,
     QMI_CAT_EVT_REPORT_REQ_SEND_USSD_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_DISPLAY_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_SEND_SHORT_MESSAGE,
     QMI_CAT_EVT_REPORT_REQ_SEND_SMS_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_DISPLAY_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_SEND_DTMF,
     QMI_CAT_EVT_REPORT_REQ_SEND_DTMF_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_DISPLAY_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_LAUNCH_BROWSER,
     QMI_CAT_EVT_REPORT_REQ_LAUNCH_BROWSER_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_GEOGRAPHICAL_LOCATION_REQUEST,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_PLAY_TONE,
     QMI_CAT_EVT_REPORT_REQ_PLAY_TONE_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_DISPLAY_TEXT,
     QMI_CAT_EVT_REPORT_REQ_DISPLAY_TEXT_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_GET_INKEY,
     QMI_CAT_EVT_REPORT_REQ_GET_INKEY_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_GET_INPUT,
     QMI_CAT_EVT_REPORT_REQ_GET_INPUT_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_SELECT_ITEM,
     QMI_CAT_EVT_REPORT_REQ_SELECT_ITEM_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_SET_UP_MENU,
     QMI_CAT_EVT_REPORT_REQ_SETUP_MENU_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_PROVIDE_LOCAL_INFORMATION,
     QMI_CAT_EVT_REPORT_REQ_PROVIDE_LOCAL_INFO_LANGUAGE_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_TIMER_MANAGEMENT,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_SET_UP_IDLE_MODE_TEXT,
     QMI_CAT_EVT_REPORT_REQ_SETUP_IDLE_MODE_TEXT_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_PERFORM_CARD_APDU,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_POWER_ON_CARD,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_POWER_OFF_CARD,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_GET_READER_STATUS,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_RUN_AT_COMMAND,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_LANGUAGE_NOTIFICATION,
     QMI_CAT_EVT_REPORT_REQ_LANGUAGE_NOTIF_MASK_V02,
     QBI_SVC_STK_PAC_PROFILE_ALL_FUNC},
  {QBI_SVC_STK_PAC_VAL_OPEN_CHANNEL,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_CLOSE_CHANNEL,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_RECEIVE_DATA,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_SEND_DATA,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_GET_CHANNEL_STATUS,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_SERVICE_SEARCH,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_GET_SERVICE_INFORMATION,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_DECLARE_SERVICE,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_SET_FRAMES,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_GET_FRAMES_STATUS,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_RETRIEVE_MULTIMEDIA_MESSAGE,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_SUBMIT_MULTIMEDIA_MESSAGE,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_DISPLAY_MULTIMEDIA_MESSAGE,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_ACTIVATE,
     0,
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY},
  {QBI_SVC_STK_PAC_VAL_END_OF_THE_PROACTIVE_SESSION,
     QMI_CAT_EVT_REPORT_REQ_END_PROACTIVE_SESSION_MASK_V02,
    /*! End session command is supported through QMI but not in raw */
     QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY}
};

/*! @todo To be removed. Use terminal profile query to retrive envelope command
    profiles once available through QMI */
/*! @brief Envelope command support table
*/
static const qbi_svc_stk_envelope_profile_s qbi_svc_stk_envelope_profile_tbl[] = {
  {QBI_SVC_STK_ENVELOPE_SMS_PP_DOWNLOAD_TAG,          TRUE},
  {QBI_SVC_STK_ENVELOPE_CELL_BROADCAST_DOWNLOAD_TAG,  TRUE},
  {QBI_SVC_STK_ENVELOPE_MENU_SELECTION_TAG,           TRUE},
  {QBI_SVC_STK_ENVELOPE_CALL_CONTROL_TAG,             TRUE},
  {QBI_SVC_STK_ENVELOPE_MO_SHORT_MESSAGE_CONTROL_TAG, TRUE},
  {QBI_SVC_STK_ENVELOPE_EVENT_DOWNLOAD_TAG,           TRUE},
  {QBI_SVC_STK_ENVELOPE_TIMER_EXPIRY_TAG,             TRUE}
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_stk_get_cache
===========================================================================*/
/*!
    @brief Returns a pointer to the STK service's
    cache

    @details

    @param ctx

    @return qbi_svc_stk_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
static qbi_svc_stk_cache_s *qbi_svc_stk_get_cache
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_stk_cache_s *) qbi_svc_cache_get(ctx, QBI_SVC_ID_STK);
} /* qbi_svc_stk_get_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_stk_init_cache
===========================================================================*/
/*!
    @brief Initializes the local cache

    @details

    @param ctx

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_stk_init_cache
(
  qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  qbi_svc_stk_update_uim_ref_id(ctx, QBI_QMI_CAT_CMD_REF_ID_NO_TR);
  return TRUE;
} /* qbi_svc_stk_init_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_stk_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (qbi_svc_cache_alloc(
        txn->ctx, QBI_SVC_ID_STK, sizeof(qbi_svc_stk_cache_s)) == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate cache");
  }
  else if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_CAT))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else if (!qbi_svc_ind_reg_static(txn->ctx, QBI_SVC_ID_STK, qbi_svc_stk_static_ind_hdlrs,
                                   ARR_SIZE(qbi_svc_stk_static_ind_hdlrs)))
  {
    QBI_LOG_E_0("Couldn't register QMI indication handlers");
  }
  else if (!qbi_svc_stk_init_cache(txn->ctx))
  {
    QBI_LOG_E_0("Couldn't initialize cache");
  }
  else
  {
    QBI_LOG_I_0("STK device service opened");
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_stk_open() */

/*===========================================================================
  FUNCTION: qbi_svc_stk_update_uim_ref_id
===========================================================================*/
/*!
    @brief Update uim reference value in cache

    @details

    @param qbi_ctx_s
    @param uint32

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_stk_update_uim_ref_id
(
  qbi_ctx_s *ctx,
  uint32     ref_id
)
{
  qbi_svc_stk_cache_s *cache;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_stk_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (cache->stk.uim_ref_id != ref_id)
  {
    QBI_LOG_I_2("UIM ref id changed from %d to %d",
                cache->stk.uim_ref_id, ref_id);
    cache->stk.uim_ref_id = ref_id;
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_stk_update_uim_ref_id() */

/*! @addtogroup MBIM_CID_STK_PAC
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_stk_pac_default_profile_to_current_profile
===========================================================================*/
/*!
    @brief Map default PAC profile to current profile based on host control
    and event report.

    @details

    @param uint32
    @param boolean
    @param boolean

    @return uint8
*/
/*=========================================================================*/
static uint8 qbi_svc_stk_pac_default_profile_to_current_profile
(
  uint32  default_profile,
  boolean host_requested,
  boolean event_report_set
)
{
  uint8  current_profile;
/*-------------------------------------------------------------------------*/
  switch(default_profile)
  {
    case QBI_SVC_STK_PAC_PROFILE_FUNC_NONE:
      current_profile =
        QBI_SVC_STK_NOT_HANDLED_BY_FUNCTION_CANNOT_BE_HANDLED_BY_HOST;
      break;

    case QBI_SVC_STK_PAC_PROFILE_DISPLAY_FUNC_ONLY:
      if (host_requested && event_report_set)
      {
        current_profile =
          QBI_SVC_STK_HANDLED_BY_HOST_FUNCTION_NOT_ABLE_TO_HANDLE;
      }
      else
      {
        current_profile =
          QBI_SVC_STK_NOT_HANDLED_BY_FUNCTION_MAY_BE_HANDLED_BY_HOST;
      }
      break;

    case QBI_SVC_STK_PAC_PROFILE_MODEM_FUNC_ONLY:
      current_profile =
        QBI_SVC_STK_HANDLED_BY_FUNCTION_ONLY_TRANSPARENT_TO_HOST;
      break;

    case QBI_SVC_STK_PAC_PROFILE_ALL_FUNC:
      if (host_requested && event_report_set)
      {
        current_profile =
          QBI_SVC_STK_HANDLED_BY_HOST_FUNCTION_ABLE_TO_HANDLE;
      }
      else
      {
        current_profile =
          QBI_SVC_STK_HANDLED_BY_FUNCTION_CAN_BE_OVERRIDDEN_BY_HOST;
      }
      break;

    default:
      current_profile =
        QBI_SVC_STK_NOT_HANDLED_BY_FUNCTION_CANNOT_BE_HANDLED_BY_HOST;
      break;
  }

  return current_profile;
} /* qbi_svc_stk_pac_default_profile_to_current_profile */


/*===========================================================================
  FUNCTION: qbi_svc_stk_pac_e_build_rsp
===========================================================================*/
/*!
    @brief Allocates and populates fields in MBIM_CID_STK_PAC event using
    the given QMI_CAT_EVENT_REPORT_IND

    @details

    @param txn
    @param msg_data

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_pac_e_build_rsp
(
  qbi_txn_s                          *txn,
  const cat_event_report_ind_msg_v02 *qmi_ind
)
{
  qbi_svc_stk_pac_event_s *rsp;
  qbi_svc_stk_cache_s *cache;
  uint32 cmd_len = 0;
  uint32 pac_val = QBI_QMI_CAT_CMD_REF_ID_NO_TR;
  const uint8_t *cmd_ptr = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_ind);

  cache = qbi_svc_stk_get_cache(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  if (qmi_ind->display_text_valid) /* Display Text Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_DISPLAY_TEXT;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->display_text.uim_ref_id);
    cmd_len = qmi_ind->display_text.pc_display_text_len;
    cmd_ptr = qmi_ind->display_text.pc_display_text;
  }
  else if (qmi_ind->get_inkey_valid) /* Get Inkey Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_GET_INKEY;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->get_inkey.uim_ref_id);
    cmd_len = qmi_ind->get_inkey.pc_get_inkey_len;
    cmd_ptr = qmi_ind->get_inkey.pc_get_inkey;
  }
  else if (qmi_ind->get_input_valid) /* Get Input Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_GET_INPUT;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->get_input.uim_ref_id);
    cmd_len = qmi_ind->get_input.pc_get_input_len;
    cmd_ptr = qmi_ind->get_input.pc_get_input;
  }
  else if (qmi_ind->setup_menu_valid) /* Setup Menu Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_SET_UP_MENU;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->setup_menu.uim_ref_id);
    cmd_len = qmi_ind->setup_menu.pc_setup_menu_len;
    cmd_ptr = qmi_ind->setup_menu.pc_setup_menu;
  }
  else if (qmi_ind->select_item_valid) /* Select Item Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_SELECT_ITEM;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->select_item.uim_ref_id);
    cmd_len = qmi_ind->select_item.pc_select_item_len;
    cmd_ptr = qmi_ind->select_item.pc_select_item;
  }
  else if (qmi_ind->idle_mode_text_valid) /* Setup Idle Mode Text Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_SET_UP_IDLE_MODE_TEXT;
    qbi_svc_stk_update_uim_ref_id(txn->ctx,
                                  qmi_ind->idle_mode_text.uim_ref_id);
    cmd_len = qmi_ind->idle_mode_text.pc_setup_idle_mode_text_len;
    cmd_ptr = qmi_ind->idle_mode_text.pc_setup_idle_mode_text;
  }
  else if (qmi_ind->lang_notification_valid) /* Language Notification Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_LANGUAGE_NOTIFICATION;
    qbi_svc_stk_update_uim_ref_id(txn->ctx,
                                  qmi_ind->lang_notification.uim_ref_id);
    cmd_len = qmi_ind->lang_notification.pc_lang_notification_len;
    cmd_ptr = qmi_ind->lang_notification.pc_lang_notification;
  }
  else if (qmi_ind->play_tone_valid) /* Play Tone Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_PLAY_TONE;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->play_tone.uim_ref_id);
    cmd_len = qmi_ind->play_tone.pc_play_tone_len;
    cmd_ptr = qmi_ind->play_tone.pc_play_tone;
  }
  else if (qmi_ind->setup_call_valid) /* Setup Call Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_SET_UP_CALL;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->setup_call.uim_ref_id);
    cmd_len = qmi_ind->setup_call.pc_setup_call_len;
    cmd_ptr = qmi_ind->setup_call.pc_setup_call;
  }
  else if (qmi_ind->send_dtmf_valid) /* Send DTMF Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_SEND_DTMF;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->send_dtmf.uim_ref_id);
    cmd_len = qmi_ind->send_dtmf.pc_send_dtmf_len;
    cmd_ptr = qmi_ind->send_dtmf.pc_send_dtmf;
  }
  else if (qmi_ind->launch_browser_valid) /* Launch Browser Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_LAUNCH_BROWSER;
    qbi_svc_stk_update_uim_ref_id(txn->ctx,
                                  qmi_ind->launch_browser.uim_ref_id);
    cmd_len = qmi_ind->launch_browser.pc_launch_browser_len;
    cmd_ptr = qmi_ind->launch_browser.pc_launch_browser;
  }
  else if (qmi_ind->send_sms_valid) /* Send SMS Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_SEND_SHORT_MESSAGE;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->send_sms.uim_ref_id);
    cmd_len = qmi_ind->send_sms.pc_send_sms_len;
    cmd_ptr = qmi_ind->send_sms.pc_send_sms;
  }
  else if (qmi_ind->send_ss_valid) /* Send SS Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_SEND_SS;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->send_ss.uim_ref_id);
    cmd_len = qmi_ind->send_ss.pc_send_ss_len;
    cmd_ptr = qmi_ind->send_ss.pc_send_ss;
  }
  else if (qmi_ind->send_ussd_valid) /* Send USSD Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_SEND_USSD;
    qbi_svc_stk_update_uim_ref_id(txn->ctx, qmi_ind->send_ussd.uim_ref_id);
    cmd_len = qmi_ind->send_ussd.pc_send_ussd_len;
    cmd_ptr = qmi_ind->send_ussd.pc_send_ussd;
  }
  else if (qmi_ind->provide_local_info_valid)
    /* Provide Local Info Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_PROVIDE_LOCAL_INFORMATION;
    qbi_svc_stk_update_uim_ref_id(
      txn->ctx, qmi_ind->provide_local_info.uim_ref_id);
    cmd_len =
      qmi_ind->provide_local_info.pc_provide_local_info_len;
    cmd_ptr =
      qmi_ind->provide_local_info.pc_provide_local_info;
  }
  else if (qmi_ind->setup_event_list_raw_valid)
    /* Setup Event List Raw Event */
  {
    pac_val = QBI_SVC_STK_PAC_VAL_SET_UP_EVENT_LIST;
    qbi_svc_stk_update_uim_ref_id(txn->ctx,
                                  qmi_ind->setup_event_list_raw.uim_ref_id);
    cmd_len = qmi_ind->setup_event_list_raw.pc_setup_event_list_len;
    cmd_ptr = qmi_ind->setup_event_list_raw.pc_setup_event_list;
  }
  else
  {
    QBI_LOG_E_0("Unsupported PAC from card!");
    action = QBI_SVC_ACTION_ABORT;
  }

  if (action == QBI_SVC_ACTION_SEND_RSP)
  {
    QBI_LOG_I_1("Received PAC from card %d.", pac_val);
    QBI_CHECK_NULL_PTR_RET_ABORT(cmd_ptr);
    if (cmd_len > QMI_CAT_RAW_PROACTIVE_CMD_MAX_LENGTH_V02)
    {
      QBI_LOG_E_1("Unexpected raw command length %d!", cmd_len);
      return QBI_SVC_ACTION_ABORT;
    }
    else
    {
      rsp = (qbi_svc_stk_pac_event_s *) qbi_txn_alloc_rsp_buf(
        txn, sizeof(qbi_svc_stk_pac_event_s) + cmd_len);
      QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

      /* Notify host that no terminal response is expected for buffered
         command if no TR ref id is used, otherwise use default command
         type */
      if (cache->stk.uim_ref_id == QBI_QMI_CAT_CMD_REF_ID_NO_TR)
      {
        rsp->pac_type = QBI_SVC_STK_NOTIFICATION;
      }
      else
      {
        rsp->pac_type = QBI_SVC_STK_PROACTIVE_COMMAND;
      }

      QBI_MEMSCPY((uint8 *) rsp + sizeof(qbi_svc_stk_pac_event_s),
                  (txn->rsp.size - sizeof(qbi_svc_stk_pac_event_s)),
                  cmd_ptr, cmd_len);
    }
  }

  return action;
} /* qbi_svc_stk_pac_e_build_rsp */

/*===========================================================================
  FUNCTION: qbi_svc_stk_pac_e_cat01_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_CAT_EVENT_REPORT_IND, looking for proactive command
           that would trigger a MBIM_CID_STK_PAC event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_pac_e_cat01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  const cat_event_report_ind_msg_v02 *qmi_ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);

  QBI_LOG_I_0("Received QMI_CAT_EVENT_REPORT_IND indication");
  qmi_ind = (const cat_event_report_ind_msg_v02 *) ind->buf->data;
  return qbi_svc_stk_pac_e_build_rsp(ind->txn, qmi_ind);
} /* qbi_svc_stk_pac_e_cat01_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_stk_pac_host_control_to_qmi_evt_report_mask
===========================================================================*/
/*!
    @brief Map STK PAC host control to QMI event report mask

    @details

    @param uint32_t
    @param qbi_svc_stk_pac_info_rsp_s

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_stk_pac_host_control_to_qmi_evt_report_mask
(
  qbi_svc_stk_pac_s_req_s    *req,
  uint32_t                   *evt_report_mask
)
{
  uint8 i;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(req);
  QBI_CHECK_NULL_PTR_RET_FALSE(evt_report_mask);

  for(i = 0; i < ARR_SIZE(qbi_svc_stk_pac_profile_tbl); i++)
  {
    if ((req->pac_host_control[qbi_svc_stk_pac_profile_tbl[i].pac_val / 8] &
          (1 << (qbi_svc_stk_pac_profile_tbl[i].pac_val % 8))) &&
        (qbi_svc_stk_pac_profile_tbl[i].default_profile ==
           QBI_SVC_STK_PAC_PROFILE_DISPLAY_FUNC_ONLY ||
         qbi_svc_stk_pac_profile_tbl[i].default_profile ==
           QBI_SVC_STK_PAC_PROFILE_ALL_FUNC))
    {
      *evt_report_mask |= qbi_svc_stk_pac_profile_tbl[i].qmi_event_mask;
    }
  }

  return TRUE;
} /* qbi_svc_stk_pac_host_control_to_qmi_evt_report_mask */

/*===========================================================================
  FUNCTION: qbi_svc_stk_pac_q_cat20_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_CAT_GET_SERVICE_STATE_RESP for
    MBIM_CID_STK_PAC query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_pac_q_cat20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  cat_get_service_state_resp_msg_v02 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (cat_get_service_state_resp_msg_v02 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    switch (qmi_rsp->resp.error)
    {
      case QMI_ERR_DEVICE_IN_USE_V01:
        qmi_txn->parent->status = QBI_MBIM_STATUS_BUSY;
        break;

      default:
        qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
        break;
    }
  }
  else
  {
    action = qbi_svc_stk_pac_qs_build_rsp(qmi_txn);
  }

  return action;
} /* qbi_svc_stk_pac_q_cat20_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_stk_pac_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_STK_PAC query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_pac_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  if (!qbi_svc_bc_check_device_state(txn, TRUE, FALSE))
  {
    QBI_LOG_E_0("Cannot propogate proactive cmds");
  }
  else
  {
    /* QMI_CAT_GET_SERVICE_STATE (0x20) */
    qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_CAT,
                                QMI_CAT_GET_SERVICE_STATE_REQ_V02,
                                qbi_svc_stk_pac_q_cat20_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_stk_pac_q_req */

/*===========================================================================
  FUNCTION: qbi_svc_stk_pac_qs_build_rsp
===========================================================================*/
/*!
    @brief Build MBIM_STK_PAC_INFO response based on
    QMI_CAT_GET_SERVICE_STATE_RESP and cached host control mask.

    @details Proactive command profile mapping
    GSTK_HANDLE_FUNC_NONE
    Not set ->
    QBI_SVC_STK_NOT_HANDLED_BY_FUNCTION_CANNOT_BE_HANDLED_BY_HOST
    Set -> NA

    GSTK_HANDLE_DISPLAY_FUNC_ONLY
    Not set ->
    QBI_SVC_STK_NOT_HANDLED_BY_FUNCTION_MAY_BE_HANDLED_BY_HOST
    Set ->
    QBI_SVC_STK_HANDLED_BY_HOST_FUNCTION_NOT_ABLE_TO_HANDLE

    GSTK_HANDLE_MODEM_FUNC_ONLY
    Not set ->
    QBI_SVC_STK_HANDLED_BY_FUNCTION_ONLY_TRANSPARENT_TO_HOST
    Set -> NA

    GSTK_HANDLE_ALL_FUNC
    Not set ->
    QBI_SVC_STK_HANDLED_BY_FUNCTION_ONLY_TRANSPARENT_TO_HOST
    Set ->
    QBI_SVC_STK_HANDLED_BY_HOST_FUNCTION_ABLE_TO_HANDLE

    @param txn
    @param uint32_t

    @return boolean
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_pac_qs_build_rsp
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uint8 i;
  boolean event_report_set;
  boolean host_requested;
  qbi_svc_stk_cache_s *cache;
  qbi_svc_stk_pac_info_rsp_s *rsp;
  cat_get_service_state_resp_msg_v02 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = qbi_txn_alloc_rsp_buf(qmi_txn->parent,
                              sizeof(qbi_svc_stk_pac_info_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  cache = qbi_svc_stk_get_cache(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = qmi_txn->rsp.data;
  for(i = 0; i < ARR_SIZE(qbi_svc_stk_pac_profile_tbl); i++)
  {
    host_requested =
      (cache->stk.pac_host_control[qbi_svc_stk_pac_profile_tbl[i].pac_val / 8] &
       (1 << (qbi_svc_stk_pac_profile_tbl[i].pac_val % 8)));

    event_report_set = ((qmi_rsp->cat_service_state.pc_evt_report_mask &
                         qbi_svc_stk_pac_profile_tbl[i].qmi_event_mask) != 0);

    rsp->pac_profile[qbi_svc_stk_pac_profile_tbl[i].pac_val] =
      qbi_svc_stk_pac_default_profile_to_current_profile(
        qbi_svc_stk_pac_profile_tbl[i].default_profile,
        host_requested, event_report_set);
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_stk_pac_qs_build_rsp */

/*===========================================================================
  FUNCTION: qbi_svc_stk_pac_s_cat01_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_CAT_SET_EVENT_REPORT_RESP for
    MBIM_CID_STK_PAC set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_pac_s_cat01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  cat_set_event_report_resp_msg_v02 *qmi_rsp;
  qbi_svc_stk_pac_s_req_s *req;
  qbi_svc_stk_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);

  qmi_rsp = (cat_set_event_report_resp_msg_v02 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    switch (qmi_rsp->resp.error)
    {
      case QMI_ERR_INVALID_OPERATION_V01:
        if (qmi_rsp->pc_evt_report_req_err_mask_valid)
        {
          QBI_LOG_E_1("Received event report err mask %d from QMI",
                      qmi_rsp->pc_evt_report_req_err_mask);
        }
        qmi_txn->parent->status = QBI_MBIM_STATUS_STK_BUSY;
        break;

      default:
        qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
        break;
    }
  }
  else
  {
    cache = qbi_svc_stk_get_cache(qmi_txn->ctx);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);

    /* Save host control bit mask to cache */
    req = (qbi_svc_stk_pac_s_req_s*) qmi_txn->parent->req.data;
    QBI_MEMSCPY(cache->stk.pac_host_control, sizeof(cache->stk.pac_host_control),
                req->pac_host_control, QBI_SVC_STK_PAC_HOST_CONTROL_LEN);

    action = qbi_svc_stk_pac_q_req(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_stk_pac_s_cat01_rsp_cb */

/*===========================================================================
  FUNCTION: qbi_svc_stk_pac_s_req
===========================================================================*/
/*!
    @brief Processes a request to set event report mask

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_pac_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_stk_pac_s_req_s *req;
  cat_set_event_report_req_msg_v02 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  if (!qbi_svc_bc_check_device_state(txn, TRUE, FALSE))
  {
    QBI_LOG_E_0("Cannot propogate proactive cmds");
  }
  else
  {
    qmi_req = (cat_set_event_report_req_msg_v02 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_CAT,
                                    QMI_CAT_SET_EVENT_REPORT_REQ_V02,
                                    qbi_svc_stk_pac_s_cat01_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    req = (qbi_svc_stk_pac_s_req_s *) txn->req.data;

    qmi_req->pc_evt_report_req_mask_valid = TRUE;
    if (qbi_svc_stk_pac_host_control_to_qmi_evt_report_mask(
       req, &qmi_req->pc_evt_report_req_mask))
    {
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
    else
    {
      QBI_LOG_E_0("Cannot map STK host control to QMI event report mask!");
    }
  }

  return action;
} /* qbi_svc_stk_pac_s_req */

/*! @} */

/*! @addtogroup MBIM_CID_STK_TERMINAL_RESPONSE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_stk_terminal_response_s_cat21_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_CAT_SEND_TR_RESP for
    MBIM_CID_STK_TERMINAL_RESPONSE set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_terminal_response_s_cat21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  cat_send_tr_resp_msg_v02 *qmi_rsp;
  qbi_svc_stk_terminal_response_info_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (cat_send_tr_resp_msg_v02 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    switch (qmi_rsp->resp.error)
    {
      case QMI_ERR_INVALID_ARG_V01:
        qmi_txn->parent->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
        break;

      default:
        qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
        break;
    }
  }
  else
  {
    rsp = qbi_txn_alloc_rsp_buf(qmi_txn->parent,
            sizeof(qbi_svc_stk_terminal_response_info_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    /*! @todo Use hardcoded status words until QMI provides raw support */
    rsp->status_words = QBI_QMI_CAT_STATUS_WORDS_SUCCESS;

    /*! @todo Add raw APDU data from QMI once support is available */

    /* Clean up UIM ref id */
    qbi_svc_stk_update_uim_ref_id(qmi_txn->ctx, QBI_QMI_CAT_CMD_REF_ID_NO_TR);
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_stk_terminal_response_s_cat21_rsp_cb */

/*===========================================================================
  FUNCTION: qbi_svc_stk_terminal_response_s_req
===========================================================================*/
/*!
    @brief

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_terminal_response_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_stk_terminal_response_s_req_s *req;
  cat_send_tr_req_msg_v02 *qmi_req;
  qbi_svc_stk_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  if (!qbi_svc_bc_check_device_state(txn, TRUE, FALSE))
  {
    QBI_LOG_E_0("Cannot send terminal reponse to proactive cmd");
  }
  else
  {
    qmi_req = (cat_send_tr_req_msg_v02 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_CAT,
                                    QMI_CAT_SEND_TR_REQ_V02,
                                    qbi_svc_stk_terminal_response_s_cat21_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    req = (qbi_svc_stk_terminal_response_s_req_s *) txn->req.data;
    if (req->response_length > QMI_CAT_TERMINAL_RESPONSE_MAX_LENGTH_V02)
    {
      QBI_LOG_E_1("Unexpected terminal response data length %d!",
                  req->response_length);
    }
    else
    {
      cache = qbi_svc_stk_get_cache(txn->ctx);
      QBI_CHECK_NULL_PTR_RET_ABORT(cache);

      if (cache->stk.uim_ref_id == QBI_QMI_CAT_CMD_REF_ID_NO_TR)
      {
        QBI_LOG_E_0("Unexpected terminal response request!");
      }
      else
      {
        /*! @todo p3 only support default slot for now which is slot 1 */
        qmi_req->terminal_response.uim_ref_id = cache->stk.uim_ref_id;
        qmi_req->terminal_response.terminal_response_len = req->response_length;
        QBI_MEMSCPY(qmi_req->terminal_response.terminal_response,
                    sizeof(qmi_req->terminal_response.terminal_response),
                    req->data_buffer, req->response_length);
        action = QBI_SVC_ACTION_SEND_QMI_REQ;
      }
    }
  }

  return action;
} /* qbi_svc_stk_terminal_response_s_req */

/*! @} */

/*! @addtogroup MBIM_CID_STK_ENVELOPE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_stk_envelope_q_build_rsp
===========================================================================*/
/*!
    @brief Build MBIM_STK_ENVELOPE_INFO response.

    @details

    @param txn

    @return boolean
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_envelope_q_build_rsp
(
  qbi_txn_s *txn
)
{
  uint8 i;
  qbi_svc_stk_envelope_info_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  rsp = qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_stk_envelope_info_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  for(i = 0; i < ARR_SIZE(qbi_svc_stk_envelope_profile_tbl); i++)
  {
    if (qbi_svc_stk_envelope_profile_tbl[i].supported)
    {
      rsp->envelope_support[qbi_svc_stk_envelope_profile_tbl[i].envelope_tag / 8] |=
        1 << qbi_svc_stk_envelope_profile_tbl[i].envelope_tag % 8;
    }
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_stk_envelope_q_build_rsp */

/*===========================================================================
  FUNCTION: qbi_svc_stk_envelope_q_req
===========================================================================*/
/*!
    @brief

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_envelope_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  if (!qbi_svc_bc_check_device_state(txn, TRUE, FALSE))
  {
    QBI_LOG_E_0("Cannot send envelope cmd");
  }
  else
  {
    action = qbi_svc_stk_envelope_q_build_rsp(txn);
  }

  return action;
} /* qbi_svc_stk_envelope_q_req */

/*===========================================================================
  FUNCTION: qbi_svc_stk_envelope_s_cat22_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_CAT_SEND_ENVELOPE_CMD_RESP for
    MBIM_CID_STK_ENVELOPE set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_envelope_s_cat22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  cat_send_envelope_cmd_resp_msg_v02 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (cat_send_envelope_cmd_resp_msg_v02 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
  }
  else
  {
    qmi_txn->parent->status = QBI_MBIM_STATUS_SUCCESS;
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_stk_envelope_s_cat22_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_stk_envelope_s_req
===========================================================================*/
/*!
    @brief Processes a request to send envelope command to SIM

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_stk_envelope_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_stk_envelope_s_req_s *req;
  cat_send_envelope_cmd_req_msg_v02 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  if (!qbi_svc_bc_check_device_state(txn, TRUE, FALSE))
  {
    QBI_LOG_E_0("Cannot send envelope cmd");
  }
  else
  {
    qmi_req = (cat_send_envelope_cmd_req_msg_v02 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_CAT,
                                    QMI_CAT_SEND_ENVELOPE_CMD_REQ_V02,
                                    qbi_svc_stk_envelope_s_cat22_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    if (txn->req.size > QMI_CAT_ENVELOPE_DATA_MAX_LENGTH_V02)
    {
      QBI_LOG_E_1("Unexpected envelope command length %d!", txn->req.size);
    }
    else
    {
      req = (qbi_svc_stk_envelope_s_req_s *) txn->req.data;
      qmi_req->envelope_cmd.env_cmd_type = CAT_ENVELOPE_CMD_TYPE_UNKNOWN_V02;
      qmi_req->envelope_cmd.envelope_data_len = txn->req.size;
      QBI_MEMSCPY(qmi_req->envelope_cmd.envelope_data,
                  sizeof(qmi_req->envelope_cmd.envelope_data),
                  req, qmi_req->envelope_cmd.envelope_data_len);
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_stk_envelope_s_req */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_stk_init
===========================================================================*/
/*!
    @brief One-time initialization of the STK device service

    @details
*/
/*=========================================================================*/
void qbi_svc_stk_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_stk_cfg = {
    {
      0xd8, 0xf2, 0x01, 0x31, 0xfc, 0xb5, 0x4e, 0x17,
      0x86, 0x02, 0xd6, 0xed, 0x38, 0x16, 0x16, 0x4c
    },
    QBI_SVC_ID_STK,
    FALSE,
    qbi_svc_stk_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_stk_cmd_hdlr_tbl),
    qbi_svc_stk_open,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_stk_cfg);
} /* qbi_svc_stk_init() */

