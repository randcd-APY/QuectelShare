/*!
  @file
  qbi_svc_atds.c

  @brief
  AT&T device service implementation, based on the "AT&T Windows 8
  Extended API Requirements - Joint Requirement Doc (JRD)" from AT&T.
  This device service allows host to set and query device/radio paremeters
  required for AT&T's acceptance.
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
09/27/13  bd   Use QMI_NAS_GET_PLMN_NAME for VISIBLE_PROVIDERS/ATDS_OPERATORS
06/27/13  bd   Rework signal reporting
04/16/13  bd   Add QMI indication (de)registration support to ATDS
04/16/13  bd   Switch to QMI_NAS_CONFIG_SIG_INFO2
04/10/13  bd   Send SIGNAL_STATE events based on UI signal bar thresholds
04/10/13  bd   Add support for WCDMA Ec/No in ATDS_SIGNAL query
09/01/12  cy   Initial release based on JRD v 1.6.1
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_bc.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc_nas.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_qmi.h"
#include "qbi_qmi_defs.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_txn.h"

#include "qbi_svc_atds.h"
#include "qbi_svc_atds_mbim.h"

#include "network_access_service_v01.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! Array index for each signal type used internally when storing projection
    tables */
typedef enum {
  QBI_SVC_ATDS_PROJECTION_INDEX_MIN   = 0,
  QBI_SVC_ATDS_PROJECTION_INDEX_RSSI  = 0,
  QBI_SVC_ATDS_PROJECTION_INDEX_RSCP  = 1,
  QBI_SVC_ATDS_PROJECTION_INDEX_ECNO  = 2,
  QBI_SVC_ATDS_PROJECTION_INDEX_RSRP  = 3,
  QBI_SVC_ATDS_PROJECTION_INDEX_RSSNR = 4,
  QBI_SVC_ATDS_PROJECTION_INDEX_MAX   = 4,
  QBI_SVC_ATDS_PROJECTION_INDEX_COUNT = 5
} qbi_svc_atds_projection_index_e;

/*! @brief Device service local cache struct
*/
typedef struct {
  struct {
    uint32 count;
    boolean present[QBI_SVC_ATDS_PROJECTION_INDEX_COUNT];
    qbi_svc_atds_projection_entry_s tables
      [QBI_SVC_ATDS_PROJECTION_INDEX_COUNT][QBI_SVC_ATDS_PROJECTION_TABLE_COUNT];
  } projection_tables;
} qbi_svc_atds_cache_s;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_atds_cache_s *qbi_svc_atds_cache_get
(
  const qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_atds_open
(
  qbi_txn_s *txn
);

/*! @addtogroup MBIM_CID_ATDS_LOCATION
    @{ */

static qbi_svc_action_e qbi_svc_atds_location_q_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_OPERATORS
    @{ */

static boolean qbi_svc_atds_operators_add
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *provider_field,
  uint16                       mcc,
  uint16                       mnc,
  boolean                      mnc_is_3_digits,
  uint32                       provider_state,
  const void                  *provider_name,
  uint32                       provider_name_len,
  boolean                      provider_name_is_ascii,
  uint32                       plmn_mode,
  uint32                       rssi,
  uint32                       error_rate
);

static uint32 qbi_svc_atds_operators_plmn_mode_to_data_class
(
  uint32 plmn_mode
);

static uint32 qbi_svc_atds_operators_qmi_rat_to_plmn_mode
(
  uint8 rat
);

static qbi_svc_action_e qbi_svc_atds_operators_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_atds_operators_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_PROJECTION_TABLES
    @{ */

static uint32 qbi_svc_atds_projection_index_to_type
(
  qbi_svc_atds_projection_index_e index
);

static qbi_svc_bc_nas_signal_metric_e qbi_svc_atds_projection_index_to_signal_metric
(
  qbi_svc_atds_projection_index_e projection_index
);

static void qbi_svc_atds_projection_table_to_signal_mapping
(
  const qbi_svc_atds_projection_entry_s *table,
  qbi_svc_bc_nas_signal_mapping_s       *mapping
);

static qbi_svc_action_e qbi_svc_atds_projection_tables_q_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_atds_projection_tables_s_apply_cached_projection
(
  qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_atds_projection_tables_s_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_atds_projection_tables_s_sanity_check_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_atds_projection_tables_s_update_cache
(
  qbi_txn_s *txn
);

static boolean qbi_svc_atds_projection_type_is_valid
(
  uint32 type
);

static qbi_svc_atds_projection_index_e qbi_svc_atds_projection_type_to_index
(
  uint32 projection_type
);

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_RAT
    @{ */

static qbi_svc_action_e qbi_svc_atds_rat_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_atds_rat_s_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_atds_rat_s_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_REGISTER_STATE
    @{ */

static qbi_svc_action_e qbi_svc_atds_register_state_q_req
(
  qbi_txn_s *txn
);

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_SIGNAL
    @{ */

static qbi_svc_action_e qbi_svc_atds_signal_q_nas4f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_atds_signal_q_nas52_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_atds_signal_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_atds_signal_q_rsp
(
  qbi_txn_s *txn
);

static uint32 qbi_svc_atds_signal_qmi_ecio_to_mbim_ecno
(
  int16_t qmi_ecio
);

static uint32 qbi_svc_atds_signal_qmi_rscp_to_mbim
(
  int16_t qmi_rscp
);

static uint32 qbi_svc_atds_signal_qmi_rsrp_to_mbim
(
  int16_t qmi_rsrp
);

static uint32 qbi_svc_atds_signal_qmi_rsrq_to_mbim
(
  int8_t qmi_rsrq
);

static uint32 qbi_svc_atds_signal_qmi_rssi_to_mbim
(
  int8_t qmi_rssi
);

static uint32 qbi_svc_atds_signal_qmi_snr_to_mbim
(
  int16_t qmi_snr
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_atds_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_atds_cmd_hdlr_tbl[] = {
  /* 1 MBIM_CID_ATDS_SIGNAL */
  {qbi_svc_atds_signal_q_req, 0, NULL, 0},

  /* 2 MBIM_CID_ATDS_LOCATION */
  {qbi_svc_atds_location_q_req, 0, NULL, 0},

  /* 3 MBIM_CID_ATDS_OPERATORS */
  {qbi_svc_atds_operators_q_req, 0,
   qbi_svc_atds_operators_s_req, sizeof(qbi_svc_bc_provider_list_s)},

  /* 4 MBIM_CID_ATDS_RAT */
  {qbi_svc_atds_rat_q_req, 0,
   qbi_svc_atds_rat_s_req, sizeof(qbi_svc_atds_rat_s)},

  /* place holder 5678 */
  {NULL, 0, NULL, 0},
  {NULL, 0, NULL, 0},
  {NULL, 0, NULL, 0},
  {NULL, 0, NULL, 0},

  /* 9 MBIM_CID_ATDS_REGISTER_STATE */
  {qbi_svc_atds_register_state_q_req, 0, NULL, 0},

  /* 10 MBIM_CID_ATDS_PROJECTION_TABLES */
  {qbi_svc_atds_projection_tables_q_req, 0,
   qbi_svc_atds_projection_tables_s_req, sizeof(qbi_svc_atds_projection_tables_s)},

  /* 11 MBIM_CID_ATDS_RAT_DISPLAY ~ Not Supported */
  {NULL, 0, NULL, 0}
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_atds_cache_get
===========================================================================*/
/*!
    @brief Returns a pointer to the Basic Connectivity device service's
    cache

    @details

    @param ctx

    @return qbi_svc_atds_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
static qbi_svc_atds_cache_s *qbi_svc_atds_cache_get
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_atds_cache_s *) qbi_svc_cache_get(ctx, QBI_SVC_ID_ATDS);
} /* qbi_svc_atds_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (qbi_svc_cache_alloc(
        txn->ctx, QBI_SVC_ID_ATDS, sizeof(qbi_svc_atds_cache_s)) == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate cache");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_atds_open() */

/*! @addtogroup MBIM_CID_ATDS_LOCATION
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_q_nas4f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_SERVING_SYSTEM_RESP for
    MBIM_CID_ATDS_LOCATION query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_location_q_nas24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_serving_system_resp_msg_v01 *qmi_rsp;
  qbi_svc_atds_location_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_get_serving_system_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("QMI gave error code %d", qmi_rsp->resp.error);
  }
  else
  {
    rsp = (qbi_svc_atds_location_rsp_s *) qbi_txn_alloc_rsp_buf(
      qmi_txn->parent, sizeof(qbi_svc_atds_location_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    rsp->lac = (qmi_rsp->lac_valid) ? (qmi_rsp->lac) : 0;
    rsp->tac = (qmi_rsp->tac_valid) ? (qmi_rsp->tac) : 0;
    rsp->cellid = (qmi_rsp->cell_id_valid) ? (qmi_rsp->cell_id) : 0;

    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_atds_location_q_nas24_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_location_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_ATDS_LOCATION query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_location_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
/*-------------------------------------------------------------------------*/
  /* QMI_NAS_GET_SERVING_SYSTEM (0x24) */
  qmi_txn = (qbi_qmi_txn_s *) qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_SERVING_SYSTEM_REQ_MSG_V01,
    qbi_svc_atds_location_q_nas24_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_atds_location_q_req() */

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_OPERATORS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_atds_operators_add
===========================================================================*/
/*!
    @brief Allocates a new MBIM_OPERATOR structure in the data buffer of
    the response, and populates it with the given data

    @details MBIM_OPERATOR is the same as MBIM_PROVIDER with the field
    CellularClass replaced a field PLMNMode in the same

    @param txn
    @param provider_field
    @param mcc MCC if plmn_mode is GSM, otherwise ignored
    @param mnc MNC if plmn_mode is GSM, otherwise ignored
    @param mnc_is_3_digits
    @param provider_state
    @param provider_name May be NULL if provider_name_len is 0
    @param provider_name_len Length of provider_name in bytes
    @param provider_name_is_ascii Set to TRUE if provider_name is encoded
    in ASCII, and should be converted to UTF-16
    @param plmn_mode
    @param rssi
    @param error_rate

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_atds_operators_add
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *provider_field,
  uint16                       mcc,
  uint16                       mnc,
  boolean                      mnc_is_3_digits,
  uint32                       provider_state,
  const void                  *provider_name,
  uint32                       provider_name_len,
  boolean                      provider_name_is_ascii,
  uint32                       plmn_mode,
  uint32                       rssi,
  uint32                       error_rate
)
{
  qbi_svc_bc_provider_s *provider;
  boolean result;
  uint32 initial_size;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  provider = (qbi_svc_bc_provider_s *) qbi_txn_rsp_databuf_add_field(
    txn, provider_field, 0, sizeof(qbi_svc_bc_provider_s), NULL);
  QBI_CHECK_NULL_PTR_RET_FALSE(provider);

  initial_size = txn->infobuf_len_total;
  result = qbi_svc_bc_provider_populate(
    txn, provider, mcc, mnc, mnc_is_3_digits, 0, provider_state, provider_name,
    provider_name_len, provider_name_is_ascii,
    QBI_SVC_BC_CELLULAR_CLASS_GSM, rssi, error_rate);

  /* Replace CellularClass with PLMNMode in the same offset */
  provider->cellular_class = plmn_mode;

  /* Account for the addition of any fields added to the DataBuffer in the
     total length of the MBIM_PROVIDER structure. */
  if (result)
  {
    provider_field->size += txn->infobuf_len_total - initial_size;
  }

  return result;
} /* qbi_svc_atds_operators_add() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_operators_plmn_mode_to_data_class
===========================================================================*/
/*!
    @brief Converts a ATDS PLMN Mode to data class

    @details

    @param plmn_mode

    @return uint32 QBI_SVC_BC_DATA_CLASS, or QBI_SVC_BC_DATA_CLASS_NONE
    if plmn_mode is not supported
*/
/*=========================================================================*/
static uint32 qbi_svc_atds_operators_plmn_mode_to_data_class
(
  uint32 plmn_mode
)
{
  uint32 data_class;
/*-------------------------------------------------------------------------*/
  switch (plmn_mode)
  {
    case QBI_SVC_ATDS_PLMN_MODE_GSM:
    case QBI_SVC_ATDS_PLMN_MODE_GSM_COMPACT:
    case QBI_SVC_ATDS_PLMN_MODE_GSM_EGPRS:
      data_class = QBI_SVC_BC_DATA_CLASS_FAMILY_GSM;
      break;

    case QBI_SVC_ATDS_PLMN_MODE_UTRAN:
    case QBI_SVC_ATDS_PLMN_MODE_UTRAN_HSDPA:
    case QBI_SVC_ATDS_PLMN_MODE_UTRAN_HSUPA:
    case QBI_SVC_ATDS_PLMN_MODE_UTRAN_HSPA:
      data_class = QBI_SVC_BC_DATA_CLASS_FAMILY_WCDMA;
      break;

    case QBI_SVC_ATDS_PLMN_MODE_LTE:
      data_class = QBI_SVC_BC_DATA_CLASS_LTE;
      break;

    default:
      QBI_LOG_E_1("Unsupported plmn_mode %d", plmn_mode);
      data_class = QBI_SVC_BC_DATA_CLASS_NONE;
      break;
  }

  return data_class;
} /* qbi_svc_atds_operators_plmn_mode_to_data_class() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_operators_qmi_rat_to_plmn_mode
===========================================================================*/
/*!
    @brief Converts a QMI RAT to ATDS PLMN Mode

    @details

    @param rat

    @return uint32 Valid PLMNMode value
*/
/*=========================================================================*/
static uint32 qbi_svc_atds_operators_qmi_rat_to_plmn_mode
(
  uint8 rat
)
{
  uint32 plmn_mode;
/*-------------------------------------------------------------------------*/
  switch (rat)
  {
    case QBI_QMI_NAS_RAT_GSM:
      plmn_mode = QBI_SVC_ATDS_PLMN_MODE_GSM;
      break;

    case QBI_QMI_NAS_RAT_UMTS:
      plmn_mode = QBI_SVC_ATDS_PLMN_MODE_UTRAN_HSPA;
      break;

    case QBI_QMI_NAS_RAT_LTE:
      plmn_mode = QBI_SVC_ATDS_PLMN_MODE_LTE;
      break;

    case QBI_QMI_NAS_RAT_TDSCDMA:
    default:
      QBI_LOG_E_1("Unsupported RAT %d, assuming UMTS", rat);
      plmn_mode = QBI_SVC_ATDS_PLMN_MODE_UTRAN_HSPA;
      break;
  }

  return plmn_mode;
} /* qbi_svc_atds_operators_qmi_rat_to_plmn_mode() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_operators_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_ATDS_OPERATORS query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_operators_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_visible_providers_q_req_s *bc_req;
  qbi_util_buf_s bc_req_buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qbi_util_buf_init(&bc_req_buf);
  bc_req = qbi_util_buf_alloc(&bc_req_buf,
                              sizeof(qbi_svc_bc_visible_providers_q_req_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(bc_req);

  bc_req->action = QBI_SVC_BC_VISIBLE_PROVIDERS_FULL_SCAN;

  qbi_util_buf_swap(&txn->req, &bc_req_buf);
  qbi_util_buf_free(&bc_req_buf);

  QBI_LOG_I_0("Forwarding ATDS_OPERATORS query to VISIBLE_PROVIDERS");
  return qbi_svc_bc_nas_visible_providers_q_req(txn);
} /* qbi_svc_atds_operators_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_operators_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_ATDS_OPERATORS set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_operators_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_provider_list_s *req;
  qbi_svc_bc_register_state_s_req_s *bc_req;
  qbi_util_buf_s bc_req_buf;
  qbi_mbim_offset_size_pair_s *provider_field;
  qbi_svc_bc_provider_s *provider;
  const uint8 *provider_id_utf16;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_bc_provider_list_s *) txn->req.data;
  if (req->element_count > 1)
  {
    QBI_LOG_E_1("Invalid count %d in operators request", req->element_count);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else
  {
    qbi_util_buf_init(&bc_req_buf);
    if (req->element_count == 0)
    {
      bc_req = (qbi_svc_bc_register_state_s_req_s *) qbi_util_buf_alloc(
        &bc_req_buf, sizeof(qbi_svc_bc_register_state_s_req_s));
      QBI_CHECK_NULL_PTR_RET_ABORT(bc_req);

      bc_req->register_action = QBI_SVC_BC_REGISTER_ACTION_AUTOMATIC;
      bc_req->data_class = QBI_SVC_BC_DATA_CLASS_NONE;
    }
    else
    {
      provider_field = (qbi_mbim_offset_size_pair_s *)
        ((uint8 *) req + sizeof(qbi_svc_bc_provider_list_s));

      provider = (qbi_svc_bc_provider_s *) qbi_txn_req_databuf_get_field(
        txn, provider_field, 0, 0);
      QBI_CHECK_NULL_PTR_RET_ABORT(provider);

      provider_id_utf16 = (const uint8 *) qbi_txn_req_databuf_get_field(
        txn, &provider->provider_id, provider_field->offset,
        QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES);
      QBI_CHECK_NULL_PTR_RET_ABORT(provider_id_utf16);

      bc_req = (qbi_svc_bc_register_state_s_req_s *) qbi_util_buf_alloc(
        &bc_req_buf, (sizeof(qbi_svc_bc_register_state_s_req_s) +
                      QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES));
      QBI_CHECK_NULL_PTR_RET_ABORT(bc_req);

      bc_req->provider_id.offset = sizeof(qbi_svc_bc_register_state_s_req_s);
      bc_req->provider_id.size = provider->provider_id.size;
      QBI_MEMSCPY((uint8 *) bc_req + bc_req->provider_id.offset,
                  bc_req->provider_id.size,
                  provider_id_utf16, bc_req->provider_id.size);

      bc_req->register_action = QBI_SVC_BC_REGISTER_ACTION_MANUAL;

      /* Convert PLMN mode in cellular class field to data class*/
      bc_req->data_class = qbi_svc_atds_operators_plmn_mode_to_data_class(
         provider->cellular_class);
    }

    qbi_util_buf_swap(&txn->req, &bc_req_buf);
    qbi_util_buf_free(&bc_req_buf);

    QBI_LOG_I_0("Forwarding ATDS_OPERATORS set request as REGISTER_STATE set");
    action = qbi_svc_bc_nas_register_state_s_req(txn);
  }

  return action;
} /* qbi_svc_atds_operators_s_req() */

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_PROJECTION_TABLES
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_atds_projection_index_to_type
===========================================================================*/
/*!
    @brief Maps a projection index to its corresponding MBIM_PROJECTION_TYPE

    @details
    Assumes the projection index is valid - unrecognized values are mapped
    to QBI_SVC_ATDS_PROJECTION_TYPE_RSSI.

    @param index

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_atds_projection_index_to_type
(
  qbi_svc_atds_projection_index_e index
)
{
  uint32 projection_type;
/*-------------------------------------------------------------------------*/
  switch (index)
  {
    case QBI_SVC_ATDS_PROJECTION_INDEX_RSSI:
      projection_type = QBI_SVC_ATDS_PROJECTION_TYPE_RSSI;
      break;

    case QBI_SVC_ATDS_PROJECTION_INDEX_RSCP:
      projection_type = QBI_SVC_ATDS_PROJECTION_TYPE_RSCP;
      break;

    case QBI_SVC_ATDS_PROJECTION_INDEX_ECNO:
      projection_type = QBI_SVC_ATDS_PROJECTION_TYPE_ECNO;
      break;

    case QBI_SVC_ATDS_PROJECTION_INDEX_RSRP:
      projection_type = QBI_SVC_ATDS_PROJECTION_TYPE_RSRP;
      break;

    case QBI_SVC_ATDS_PROJECTION_INDEX_RSSNR:
      projection_type = QBI_SVC_ATDS_PROJECTION_TYPE_RSSNR;
      break;

    default:
      QBI_LOG_E_1("Invalid projection index %d", index);
      projection_type = QBI_SVC_ATDS_PROJECTION_TYPE_RSSI;
      break;
  }

  return projection_type;
} /* qbi_svc_atds_projection_index_to_type() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_projection_index_to_signal_metric
===========================================================================*/
/*!
    @brief Converts an ATDS projection index enum to a signal metric enum

    @details

    @param projection_index

    @return qbi_svc_bc_nas_signal_metric_e
*/
/*=========================================================================*/
static qbi_svc_bc_nas_signal_metric_e qbi_svc_atds_projection_index_to_signal_metric
(
  qbi_svc_atds_projection_index_e projection_index
)
{
  qbi_svc_bc_nas_signal_metric_e metric;
/*-------------------------------------------------------------------------*/
  switch (projection_index)
  {
    case QBI_SVC_ATDS_PROJECTION_INDEX_RSSI:
      metric = QBI_SVC_BC_NAS_SIGNAL_METRIC_GSM_RSSI;
      break;

    case QBI_SVC_ATDS_PROJECTION_INDEX_RSCP:
      metric = QBI_SVC_BC_NAS_SIGNAL_METRIC_UMTS_RSCP;
      break;

    case QBI_SVC_ATDS_PROJECTION_INDEX_RSRP:
      metric = QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSRP;
      break;

    case QBI_SVC_ATDS_PROJECTION_INDEX_RSSNR:
      metric = QBI_SVC_BC_NAS_SIGNAL_METRIC_LTE_RSSNR;
      break;

    default:
      QBI_LOG_E_1("No signal metric for projection index %d", projection_index);
      metric = QBI_SVC_BC_NAS_SIGNAL_METRIC_MAX;
  }

  return metric;
} /* qbi_svc_atds_projection_index_to_signal_metric() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_projection_table_to_signal_mapping
===========================================================================*/
/*!
    @brief Converts an ATDS signal projection table to the signal mapping
    structure used with MBIM_CID_SIGNAL_STATE

    @details

    @param table
    @param mapping
*/
/*=========================================================================*/
static void qbi_svc_atds_projection_table_to_signal_mapping
(
  const qbi_svc_atds_projection_entry_s *table,
  qbi_svc_bc_nas_signal_mapping_s       *mapping
)
{
  uint32 i;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(table);
  QBI_CHECK_NULL_PTR_RET(mapping);

  QBI_MEMSET(mapping, 0, sizeof(qbi_svc_bc_nas_signal_mapping_s));
  for (i = 0; i < QBI_SVC_BC_NAS_SIGNAL_MAPPING_ENTRIES_MAX; i++)
  {
    /* Assumption used below:
       QBI_SVC_ATDS_PROJECTION_FACTOR == QBI_SVC_BC_NAS_SIGNAL_MAPPING_FACTOR */
    mapping->table[i].min_value   = table[i].bar_min;
    mapping->table[i].coefficient = table[i].a;
    mapping->table[i].constant    = table[i].b;
  }
} /* qbi_svc_atds_projection_table_to_signal_mapping() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_projection_tables_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_ATDS_PROJECTION_TABLES query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_projection_tables_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_atds_projection_tables_s *rsp;
  qbi_svc_atds_cache_s *cache;
  qbi_svc_atds_projection_index_e index;
  qbi_svc_atds_projection_table_s *table;
  qbi_mbim_offset_size_pair_s *list_ol;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_atds_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_atds_projection_tables_s *) qbi_txn_alloc_rsp_buf(
    txn, (sizeof(qbi_svc_atds_projection_tables_s) +
          (cache->projection_tables.count *
           sizeof(qbi_mbim_offset_size_pair_s))));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->count = cache->projection_tables.count;
  list_ol = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) rsp + sizeof(qbi_svc_atds_projection_tables_s));
  for (index = QBI_SVC_ATDS_PROJECTION_INDEX_MIN;
       index <= QBI_SVC_ATDS_PROJECTION_INDEX_MAX; index++)
  {
    if (cache->projection_tables.present[index] == TRUE)
    {
      table = (qbi_svc_atds_projection_table_s *)
        qbi_txn_rsp_databuf_add_field(
          txn, list_ol, 0, sizeof(qbi_svc_atds_projection_table_s), NULL);
      QBI_CHECK_NULL_PTR_RET_ABORT(table);

      table->type = qbi_svc_atds_projection_index_to_type(index);
      QBI_MEMSCPY(table->entries, sizeof(table->entries),
                  &(cache->projection_tables.tables[index][0]),
                  (sizeof(qbi_svc_atds_projection_entry_s) *
                   QBI_SVC_ATDS_PROJECTION_TABLE_COUNT));

      list_ol++;
    }
  }

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_atds_projection_tables_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_projection_tables_s_apply_cached_projection
===========================================================================*/
/*!
    @brief Loads the cached projection tables as custom mappings for
    MBIM_CID_SIGNAL_STATE

    @details

    @param ctx

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_atds_projection_tables_s_apply_cached_projection
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_atds_projection_index_e index;
  const qbi_svc_atds_cache_s *cache;
  qbi_svc_bc_nas_signal_mapping_s mapping;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_atds_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  qbi_svc_bc_nas_signal_state_custom_mapping_reset_all(ctx);
  for (index = QBI_SVC_ATDS_PROJECTION_INDEX_MIN;
       success == TRUE && index <= QBI_SVC_ATDS_PROJECTION_INDEX_MAX; index++)
  {
    if (index != QBI_SVC_ATDS_PROJECTION_INDEX_ECNO &&
        cache->projection_tables.present[index])
    {
      qbi_svc_atds_projection_table_to_signal_mapping(
        cache->projection_tables.tables[index], &mapping);
      success = qbi_svc_bc_nas_signal_state_custom_mapping_set(
        ctx, qbi_svc_atds_projection_index_to_signal_metric(index), &mapping);
    }
  }

  if (success)
  {
    qbi_svc_bc_nas_signal_state_custom_mapping_apply(ctx);
  }

  return success;
} /* qbi_svc_atds_projection_tables_s_apply_cached_projection() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_projection_tables_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_ATDS_PROJECTION_TABLES set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_projection_tables_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  if (!qbi_svc_atds_projection_tables_s_sanity_check_req(txn))
  {
    QBI_LOG_E_0("Projection tables set request failed sanity check");
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else if (!qbi_svc_atds_projection_tables_s_update_cache(txn) ||
           !qbi_svc_atds_projection_tables_s_apply_cached_projection(txn->ctx))
  {
    QBI_LOG_E_0("Failed to update cache or apply projection");
    qbi_svc_atds_reset(txn->ctx);
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_atds_projection_tables_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_projection_tables_s_sanity_check_req
===========================================================================*/
/*!
    @brief Sanity checks a MBIM_CID_ATDS_PROJECTION_TABLES set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static boolean qbi_svc_atds_projection_tables_s_sanity_check_req
(
  qbi_txn_s *txn
)
{
  const qbi_svc_atds_projection_tables_s *req;
  const qbi_mbim_offset_size_pair_s *list_ol;
  const qbi_svc_atds_projection_table_s *projection_table;
  uint8 i;
  boolean result;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  req = (const qbi_svc_atds_projection_tables_s *) txn->req.data;
  if (req->count > QBI_SVC_ATDS_PROJECTION_INDEX_COUNT)
  {
    QBI_LOG_E_1("Invalid projection table count %d", req->count);
    result = FALSE;
  }
  else
  {
    list_ol = (const qbi_mbim_offset_size_pair_s *)
      ((uint8 *) txn->req.data + sizeof(qbi_svc_atds_projection_tables_s));
    result = TRUE;
    for (i = 0; i < req->count; i++)
    {
      projection_table = (const qbi_svc_atds_projection_table_s *)
        qbi_txn_req_databuf_get_field(txn, list_ol, 0, 0);
      QBI_CHECK_NULL_PTR_RET_FALSE(projection_table);

      if (!qbi_svc_atds_projection_type_is_valid(projection_table->type))
      {
        QBI_LOG_E_2("Invalid projection type %d at index %d",
                     projection_table->type, i);
        result = FALSE;
        break;
      }
      else if (list_ol->size != sizeof(qbi_svc_atds_projection_table_s))
      {
        QBI_LOG_E_1("Invalid projection table size %d", list_ol->size);
        result = FALSE;
        break;
      }
      list_ol++;
    }
  }

  return result;
} /* qbi_svc_atds_projection_tables_s_sanity_check_req() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_projection_tables_s_update_cache
===========================================================================*/
/*!
    @brief Update Projection table in cache

    @details

    @param txn

    @return TRUE if success
*/
/*=========================================================================*/
static boolean qbi_svc_atds_projection_tables_s_update_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_atds_projection_tables_s *req;
  qbi_mbim_offset_size_pair_s *list_ol;
  qbi_svc_atds_projection_table_s *projection_table;
  qbi_svc_atds_cache_s *cache;
  uint32 i, j;
  qbi_svc_atds_projection_index_e index;
  boolean result = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);

  cache = qbi_svc_atds_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  QBI_MEMSET(&(cache->projection_tables), 0, sizeof(cache->projection_tables));
  req = (qbi_svc_atds_projection_tables_s *) txn->req.data;
  cache->projection_tables.count = req->count;

  list_ol = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) txn->req.data + sizeof(qbi_svc_atds_projection_tables_s));
  for(i = 0; i < req->count; i++)
  {
    projection_table = (qbi_svc_atds_projection_table_s *)
      qbi_txn_req_databuf_get_field(txn, list_ol, 0, 0);
    QBI_CHECK_NULL_PTR_RET_FALSE(projection_table);

    index = qbi_svc_atds_projection_type_to_index(projection_table->type);
    if (index > QBI_SVC_ATDS_PROJECTION_INDEX_MAX)
    {
      QBI_LOG_E_1("Received invalid projection type %d",
                  projection_table->type);
      result = FALSE;
      QBI_MEMSET(&(cache->projection_tables), 0,
                 sizeof(cache->projection_tables));
      break;
    }

    QBI_MEMSCPY(cache->projection_tables.tables[index],
                sizeof(cache->projection_tables.tables[index]),
                projection_table->entries, sizeof(projection_table->entries));
    cache->projection_tables.present[index] = TRUE;

    /* ATDS spec incorrectly calculated the A & B values for the 2-bar mapping
       of RSRP. This is also seen in the AT&T MBAE, so correct it here to
       achieve the intended result. */
    if (index == QBI_SVC_ATDS_PROJECTION_INDEX_RSRP &&
        cache->projection_tables.tables[index][3].bar_min == -115 &&
        cache->projection_tables.tables[index][3].a == -20600 &&
        cache->projection_tables.tables[index][3].b == -2263000)
    {
      QBI_LOG_I_0("Applying hotfix for incorrectly calculated RSRP bar 2");
      cache->projection_tables.tables[index][3].a = 600;
      cache->projection_tables.tables[index][3].b = -37000;
    }
    /* AT&T MBAE sends the wrong bar_min for LTE RSSNR, correct it here */
    if (index == QBI_SVC_ATDS_PROJECTION_INDEX_RSSNR &&
        cache->projection_tables.tables[index][1].bar_min == -4 &&
        cache->projection_tables.tables[index][1].a == 1169 &&
        cache->projection_tables.tables[index][1].b == -95290)
    {
      QBI_LOG_I_0("Applying hotfix for minimum RSSNR level for 4 bars");
      cache->projection_tables.tables[index][1].bar_min = 4;
    }
    /* ATDS spec contains -5 as the lower bound of the 1 bar mapping for LTE
       RSSNR. Actual range goes down to -20 in practice. Also, the projected
       RSSI for -5 is (-5 * 2) - 100 = -110, which maps to 0 bars. Use a
       different mapping to comply with the requirement to always display at
       least 1 bar when signal is detectable. */
    if (index == QBI_SVC_ATDS_PROJECTION_INDEX_RSSNR &&
        cache->projection_tables.tables[index][4].bar_min == -5 &&
        cache->projection_tables.tables[index][4].a == 2000 &&
        cache->projection_tables.tables[index][4].b == -100000)
    {
      QBI_LOG_I_0("Using modified projection for RSSNR 1 bar");
      cache->projection_tables.tables[index][4].bar_min = -20;
      cache->projection_tables.tables[index][4].a = 188;
      cache->projection_tables.tables[index][4].b = -105250;
    }

    list_ol++;

    /* print project table */
    QBI_LOG_D_1("Projection table type %d", projection_table->type);
    for (j = 0; j < QBI_SVC_ATDS_PROJECTION_TABLE_COUNT; j++)
    {
      QBI_LOG_D_4("Bar%dMin: %4d A: %6d B: %6d",
                  (QBI_SVC_ATDS_PROJECTION_TABLE_COUNT - 1 - j),
                  projection_table->entries[j].bar_min,
                  projection_table->entries[j].a,
                  projection_table->entries[j].b);
    }
  }

  return result;
} /* qbi_svc_atds_projection_tables_s_update_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_projection_type_is_valid
===========================================================================*/
/*!
    @brief check if projection type is valid

    @details

    @param type

    @return TRUE if valid or FALSE if invalid
*/
/*=========================================================================*/
static boolean qbi_svc_atds_projection_type_is_valid
(
  uint32 type
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_atds_projection_type_to_index(type) <=
            QBI_SVC_ATDS_PROJECTION_INDEX_MAX);
} /* qbi_svc_atds_projection_type_is_valid() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_projection_type_to_index
===========================================================================*/
/*!
    @brief Maps PROJECTION TYPE to index

    @details

    @param projection_type

    @return index or QBI_SVC_ATDS_PROJECTION_INDEX_COUNT for invalid type
*/
/*=========================================================================*/
static qbi_svc_atds_projection_index_e qbi_svc_atds_projection_type_to_index
(
  uint32 projection_type
)
{
  qbi_svc_atds_projection_index_e index;
/*-------------------------------------------------------------------------*/
  switch (projection_type)
  {
    case QBI_SVC_ATDS_PROJECTION_TYPE_RSSI:
      index = QBI_SVC_ATDS_PROJECTION_INDEX_RSSI;
      break;

    case QBI_SVC_ATDS_PROJECTION_TYPE_RSCP:
      index = QBI_SVC_ATDS_PROJECTION_INDEX_RSCP;
      break;

    case QBI_SVC_ATDS_PROJECTION_TYPE_ECNO:
      index = QBI_SVC_ATDS_PROJECTION_INDEX_ECNO;
      break;

    case QBI_SVC_ATDS_PROJECTION_TYPE_RSRP:
      index = QBI_SVC_ATDS_PROJECTION_INDEX_RSRP;
      break;

    case QBI_SVC_ATDS_PROJECTION_TYPE_RSSNR:
      index = QBI_SVC_ATDS_PROJECTION_INDEX_RSSNR;
      break;

    default:
      index = QBI_SVC_ATDS_PROJECTION_INDEX_COUNT;
      break;
  }

  return index;
} /* qbi_svc_atds_projection_type_to_index() */

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_RAT
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_atds_rat_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_ATDS_RAT query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_rat_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_atds_rat_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  rsp = (qbi_svc_atds_rat_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_atds_rat_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  /* Retrieve the current mode preference from the cache. Note that although
     the mode preference is a bitmask, ATDS does not allow for combinations,
     e.g. 3G+4G, so those are mapped to automatic */
  switch (qbi_svc_bc_get_preferred_data_class(txn->ctx))
  {
    case QMI_NAS_RAT_MODE_PREF_GSM_V01:
      rsp->mode_pref = QBI_SVC_ATDS_RAT_PREF_2G_ONLY;
      break;

    case QMI_NAS_RAT_MODE_PREF_UMTS_V01:
      rsp->mode_pref = QBI_SVC_ATDS_RAT_PREF_3G_ONLY;
      break;

    case QMI_NAS_RAT_MODE_PREF_LTE_V01:
      rsp->mode_pref = QBI_SVC_ATDS_RAT_PREF_4G_ONLY;
      break;

    default:
      rsp->mode_pref = QBI_SVC_ATDS_RAT_PREF_AUTOMATIC;
      break;
  }

  QBI_LOG_D_1("Current RAT mode pref is %d", rsp->mode_pref);

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_atds_rat_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_rat_s_nas33_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_RESP for
    MBIM_CID_ATDS_RAT set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_rat_s_nas33_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_set_system_selection_preference_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (nas_set_system_selection_preference_resp_msg_v01 *)
    qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_NO_EFFECT_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    /*! @note The mode preference cache used for MBIM_CID_REGISTER_STATE will
        be updated via QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND. */
    QBI_LOG_D_0("Successfully set registration mode");
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_atds_rat_s_nas33_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_rat_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_ATDS_RAT set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_rat_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_atds_rat_s *req;
  nas_set_system_selection_preference_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_atds_rat_s *) txn->req.data;
  if (req->mode_pref > QBI_SVC_ATDS_RAT_PREF_4G_ONLY)
  {
    QBI_LOG_E_1("Requested invalid mode preference %d", req->mode_pref);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else
  {
    QBI_LOG_I_1("Setting ATDS_RAT mode preference to %d", req->mode_pref);

    qmi_req = (nas_set_system_selection_preference_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_NAS,
        QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE_REQ_MSG_V01,
        qbi_svc_atds_rat_s_nas33_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    switch (req->mode_pref)
    {
      case QBI_SVC_ATDS_RAT_PREF_2G_ONLY:
        qmi_req->mode_pref = QMI_NAS_RAT_MODE_PREF_GSM_V01;
        break;

      case QBI_SVC_ATDS_RAT_PREF_3G_ONLY:
        qmi_req->mode_pref = QMI_NAS_RAT_MODE_PREF_UMTS_V01;
        break;

      case QBI_SVC_ATDS_RAT_PREF_4G_ONLY:
        qmi_req->mode_pref = QMI_NAS_RAT_MODE_PREF_LTE_V01;
        break;

      case QBI_SVC_ATDS_RAT_PREF_AUTOMATIC:
      default: /* Input already sanitized above */
        qmi_req->mode_pref = (QMI_NAS_RAT_MODE_PREF_GSM_V01 |
                              QMI_NAS_RAT_MODE_PREF_UMTS_V01 |
                              QMI_NAS_RAT_MODE_PREF_LTE_V01);
        break;
    }

    qmi_req->mode_pref_valid = TRUE;
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_atds_rat_s_req() */

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_REGISTER_STATE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_atds_register_state_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_ATDS_REGISTER_STATE query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_register_state_q_req
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_D_0("Forwarding ATDS_REGISTER_STATE query to REGISTER_STATE");

  return qbi_svc_bc_nas_register_state_q_req(txn);
} /* qbi_svc_atds_register_state_q_req() */

/*! @} */

/*! @addtogroup MBIM_CID_ATDS_SIGNAL
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_q_nas4f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_SIG_INFO_RESP for MBIM_CID_ATDS_SIGNAL query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_signal_q_nas4f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_sig_info_resp_msg_v01 *qmi_rsp;
  qbi_svc_atds_signal_info_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = (qbi_svc_atds_signal_info_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_rsp = (nas_get_sig_info_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("QMI gave error code %d", qmi_rsp->resp.error);
  }
  else
  {
    if (qmi_rsp->gsm_sig_info_valid)
    {
      rsp->rssi =
        qbi_svc_atds_signal_qmi_rssi_to_mbim(qmi_rsp->gsm_sig_info);
    }

    if (qmi_rsp->wcdma_sig_info_valid)
    {
      /* For WCDMA, RSSI field is RSCP Value, leave rsp->rssi unknown */
      rsp->rscp = qbi_svc_atds_signal_qmi_rscp_to_mbim(
        qmi_rsp->wcdma_sig_info.rssi);
      rsp->ecno = qbi_svc_atds_signal_qmi_ecio_to_mbim_ecno(
        qmi_rsp->wcdma_sig_info.ecio);
    }

    if (qmi_rsp->lte_sig_info_valid)
    {
      rsp->rssi =
        qbi_svc_atds_signal_qmi_rssi_to_mbim(qmi_rsp->lte_sig_info.rssi);
      rsp->rsrq =
        qbi_svc_atds_signal_qmi_rsrq_to_mbim(qmi_rsp->lte_sig_info.rsrq);
      rsp->rsrp =
        qbi_svc_atds_signal_qmi_rsrp_to_mbim(qmi_rsp->lte_sig_info.rsrp);
      rsp->rssnr =
        qbi_svc_atds_signal_qmi_snr_to_mbim(qmi_rsp->lte_sig_info.snr);
    }
  }

  return qbi_svc_atds_signal_q_rsp(qmi_txn->parent);
} /* qbi_svc_atds_signal_q_nas4f_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_q_nas52_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_GET_ERR_RATE_RESP for
    MBIM_CID_ATDS_SIGNAL query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_signal_q_nas52_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  nas_get_err_rate_resp_msg_v01 *qmi_rsp;
  qbi_svc_atds_signal_info_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = (qbi_svc_atds_signal_info_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_rsp = (nas_get_err_rate_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("QMI gave error code %d", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->gsm_bit_err_rate_valid)
  {
    QBI_LOG_I_0("Error rate TLV not included in response!");
  }
  else
  {
    rsp->ber = qmi_rsp->gsm_bit_err_rate;
  }

  return qbi_svc_atds_signal_q_rsp(qmi_txn->parent);
} /* qbi_svc_atds_signal_q_nas52_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_ATDS_SIGNAL query request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_signal_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_atds_signal_info_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  /* QMI_NAS_GET_SIG_INFO (0x4f) */
  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_SIG_INFO_REQ_MSG_V01,
    qbi_svc_atds_signal_q_nas4f_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  /* QMI_NAS_GET_ERR_RATE (0x52) */
  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_NAS, QMI_NAS_GET_ERR_RATE_REQ_MSG_V01,
    qbi_svc_atds_signal_q_nas52_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  rsp = (qbi_svc_atds_signal_info_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_atds_signal_info_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->rssi  = QBI_SVC_ATDS_RSSI_UNKNOWN;
  rsp->ber   = QBI_SVC_ATDS_BER_UNKNOWN;
  rsp->rscp  = QBI_SVC_ATDS_RSCP_UNKNOWN;
  rsp->ecno  = QBI_SVC_ATDS_ECNO_UNKNOWN;
  rsp->rsrq  = QBI_SVC_ATDS_RSRQ_UNKNOWN;
  rsp->rsrp  = QBI_SVC_ATDS_RSRP_UNKNOWN;
  rsp->rssnr = QBI_SVC_ATDS_RSSNR_UNKNOWN;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_atds_signal_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_q_rsp
===========================================================================*/
/*!
    @brief Perform query response processing for MBIM_CID_ATDS_SIGNAL

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_atds_signal_q_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (txn->qmi_txns_pending > 0)
  {
    /* Keep the QMI response and wait for the other response */
    QBI_LOG_I_0("Waiting on other responses");
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_atds_signal_q_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_qmi_ecio_to_mbim_ecno
===========================================================================*/
/*!
    @brief Converts a QMI ECIO value into MBIM's coded ECNO value

    @details

    @param qmi_ecio

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_atds_signal_qmi_ecio_to_mbim_ecno
(
  int16_t qmi_ecio
)
{
/*-------------------------------------------------------------------------*/
  /* QMI ECIO <--> MBIM ECNO mapping table (refer to 3GPP TS 25.133 9.1.2.3):

      QMI ECIO  |  Ec/Io (dB) | MBIM ECNO
     -----------+-------------+------------
       0        |     0       |  49
       1        |    -0.5     |  48
       2        |    -1.0     |  47
               ...           ...
       47       |   -23.5     |  2
       48       |   -24.0     |  1
       49       |   -24.5     |  0
  */
  if (qmi_ecio < QBI_SVC_ATDS_ECNO_MIN)
  {
    qmi_ecio = QBI_SVC_ATDS_ECNO_MIN;
  }
  else if (qmi_ecio > QBI_SVC_ATDS_ECNO_MAX)
  {
    qmi_ecio = QBI_SVC_ATDS_ECNO_MAX;
  }

  return (uint32) (QBI_SVC_ATDS_ECNO_MAX - qmi_ecio);
} /* qbi_svc_atds_signal_qmi_ecio_to_mbim_ecno() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_qmi_rscp_to_mbim
===========================================================================*/
/*!
    @brief Converts a QMI RSCP value into MBIM's coded RSCP value

    @details

    @param qmi_rscp

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_atds_signal_qmi_rscp_to_mbim
(
  int16_t qmi_rscp
)
{
  uint32 mbim_rscp;
/*-------------------------------------------------------------------------*/
  /* QMI RSCP <--> MBIM RSCP mapping table:

     QMI RSCP/dBm   |  MBIM RSCP
     ---------------+-----------
      -120          |    0
      -119          |    1
      -118          |    2
                   ....
      -25           |    95
      -24           |    96
  */
  if (qmi_rscp == 0)
  {
    mbim_rscp = QBI_SVC_ATDS_RSCP_UNKNOWN;
  }
  else if (qmi_rscp <= QBI_SVC_ATDS_RSCP_MIN_DBM)
  {
    mbim_rscp = QBI_SVC_ATDS_RSCP_MIN;
  }
  else if (qmi_rscp >= QBI_SVC_ATDS_RSCP_MAX_DBM)
  {
    mbim_rscp = QBI_SVC_ATDS_RSCP_MAX;
  }
  else
  {
    mbim_rscp = (uint32) (qmi_rscp - QBI_SVC_ATDS_RSCP_MIN_DBM);
  }

  QBI_LOG_D_2("Converted QMI RSCP %4d to MBIM RSCP %4d", qmi_rscp, mbim_rscp);
  return mbim_rscp;
} /* qbi_svc_atds_signal_qmi_rscp_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_qmi_rsrp_to_mbim
===========================================================================*/
/*!
    @brief Converts a QMI RSRP value into MBIM's coded RSRP value

    @details

    @param qmi_rsrp

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_atds_signal_qmi_rsrp_to_mbim
(
  int16_t qmi_rsrp
)
{
  uint32 mbim_rsrp;
/*-------------------------------------------------------------------------*/
  /* QMI RSRP <--> MBIM RSRP mapping table:

      QMI RSRP/dBm   |  MBIM RSRP
     ----------------+-----------
      -140           |    0
      -139           |    1
      -138           |    2
                    ....
      -44            |    96
      -43            |    97
  */
  if (qmi_rsrp <= QBI_SVC_ATDS_RSRP_MIN_DBM)
  {
    mbim_rsrp = QBI_SVC_ATDS_RSRP_MIN;
  }
  else if (qmi_rsrp >= QBI_SVC_ATDS_RSRP_MAX_DBM)
  {
    mbim_rsrp = QBI_SVC_ATDS_RSRP_MAX;
  }
  else
  {
    mbim_rsrp = (uint32) (qmi_rsrp - QBI_SVC_ATDS_RSRP_MIN_DBM);
  }

  QBI_LOG_D_2("Converted QMI RSRP %4d to MBIM RSRP %4d", qmi_rsrp, mbim_rsrp);
  return mbim_rsrp;
} /* qbi_svc_atds_signal_qmi_rsrp_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_qmi_rsrq_to_mbim
===========================================================================*/
/*!
    @brief Converts a QMI RSRQ value into MBIM's coded RSRQ value

    @details

    @param qmi_rsrq

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_atds_signal_qmi_rsrq_to_mbim
(
  int8_t qmi_rsrq
)
{
  uint32 mbim_rsrq;
/*-------------------------------------------------------------------------*/
  /* QMI RSRQ <--> MBIM RSRQ mapping table:

      dBm   | MBIM RSRQ|  QMI RSRQ
     -------+----------+------------
      -20   |    0     |    -20
      -19.5 |    0     |
      -19   |    1     |    -19
      -18.5 |    2     |
      -18   |    3     |    -18
                ....
       -4   |    31    |    -4
       -3.5 |    32    |
       -3   |    33    |    -3
       -2.5 |    34    |
     > -2.5 |    34    |    -2
  */
  if (qmi_rsrq < (int8_t) QBI_SVC_ATDS_RSRQ_MIN_DBM)
  {
    mbim_rsrq = QBI_SVC_ATDS_RSRQ_MIN;
  }
  else if (qmi_rsrq > (int8_t) QBI_SVC_ATDS_RSRQ_MAX_DBM)
  {
    mbim_rsrq = QBI_SVC_ATDS_RSRQ_MAX;
  }
  else
  {
    mbim_rsrq = (uint32) (QBI_SVC_ATDS_RSRQ_MAX + 5 + (2 * qmi_rsrq));
  }

  QBI_LOG_D_2("Converted QMI RSRQ %4d to MBIM RSRQ %4d", qmi_rsrq, mbim_rsrq);
  return mbim_rsrq;
} /* qbi_svc_atds_signal_qmi_rsrq_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_qmi_rssi_to_mbim
===========================================================================*/
/*!
    @brief Converts a QMI RSSI value into MBIM's coded RSSI value

    @details

    @param qmi_rssi

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_atds_signal_qmi_rssi_to_mbim
(
  int8_t qmi_rssi
)
{
  uint32 mbim_rssi;
/*-------------------------------------------------------------------------*/
  /* QMI RSSI <--> MBIM RSSI mapping table:

      dBm   | MBIM RSSI|  QMI RSSI
     -------+----------+------------
      -113  |    0     |    -113
      -111  |    1     |
      -109  |    2     |
                ....
       -53  |    30    |    -53
       -51  |    31    |    -51
  */
  if (qmi_rssi <= QBI_SVC_ATDS_RSSI_MIN_DBM)
  {
    mbim_rssi = QBI_SVC_ATDS_RSSI_MIN;
  }
  else if (qmi_rssi >= QBI_SVC_ATDS_RSSI_MAX_DBM)
  {
    mbim_rssi = QBI_SVC_ATDS_RSSI_MAX;
  }
  else
  {
    mbim_rssi = (uint32)
      (QBI_SVC_ATDS_RSSI_MAX - (QBI_SVC_ATDS_RSSI_MAX_DBM - qmi_rssi) / 2);
  }

  QBI_LOG_D_2("Converted QMI RSSI %4d to MBIM RSSI %4d", qmi_rssi, mbim_rssi);
  return mbim_rssi;
} /* qbi_svc_atds_signal_qmi_rssi_to_mbim() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_signal_qmi_snr_to_mbim
===========================================================================*/
/*!
    @brief Converts a QMI SNR value into MBIM's coded SNR value

    @details

    @param qmi_snr

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_atds_signal_qmi_snr_to_mbim
(
  int16_t qmi_snr
)
{
  uint32 mbim_snr;
/*-------------------------------------------------------------------------*/
  /* QMI SNR MBIM SNR mapping table:

      dB    | MBIM SNR|  QMI SNR
     -------+----------+------------
      -5    |    0     |    -50
      -4.5  |    2     |    -45
                ....
      29.5  |    33    |    295
      30    |    34    |    300
  */
  if (qmi_snr < (int16_t)
        (QBI_SVC_ATDS_RSSNR_MIN_DBM * QBI_QMI_NAS_SNR_FACTOR))
  {
    mbim_snr = QBI_SVC_ATDS_RSSNR_MIN;
  }
  else if (qmi_snr > (int16_t)
             (QBI_SVC_ATDS_RSSNR_MAX_DBM * QBI_QMI_NAS_SNR_FACTOR))
  {
    mbim_snr = QBI_SVC_ATDS_RSSNR_MAX;
  }
  else
  {
    /*! @note The ATDS spec gives 32 data points to a 34 dB range, so we need
        to scale. The simplified version of the formula used below is
        mbim_snr = 2 + (32/34)*(qmi_snr/10 + 4.5) but we rearrange it to avoid
        any floating point calculations */
    mbim_snr = (uint32)
      (qmi_snr - (int16) (QBI_SVC_ATDS_RSSNR_MIN_DBM * QBI_QMI_NAS_SNR_FACTOR));
    mbim_snr *= 32;
    mbim_snr /= (uint32)
      ((QBI_SVC_ATDS_RSSNR_MAX_DBM - QBI_SVC_ATDS_RSSNR_MIN_DBM) *
       QBI_QMI_NAS_SNR_FACTOR);
    mbim_snr += 2;
  }

  QBI_LOG_D_2("Converted QMI SNR  %4d to MBIM SNR  %4d", qmi_snr, mbim_snr);
  return mbim_snr;
} /* qbi_svc_atds_signal_qmi_snr_to_mbim() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*! @addtogroup MBIM_CID_ATDS_OPERATORS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_atds_operators_q_nas44_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_NAS_GET_PLMN_NAME_RESP for MBIM_CID_ATDS_OPERATORS
    query response

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_atds_operators_q_nas44_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  nas_get_plmn_name_resp_msg_v01 *qmi_rsp;
  qbi_svc_bc_nas_visible_providers_q_info_s *info;
  uint32 provider_name_len = 0;
  uint8 provider_name[QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES];
  uint32 provider_state;
  uint32 plmn_mode;
  qbi_mbim_offset_size_pair_s *provider_field;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);

  info = (qbi_svc_bc_nas_visible_providers_q_info_s *) qmi_txn->parent->info;
  qmi_rsp = (nas_get_plmn_name_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_3("Couldn't get PLMN name for %03d-%02d: error %d",
                info->ntw[info->cur_index].mcc,
                info->ntw[info->cur_index].mnc, qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->eons_plmn_name_3gpp_valid)
  {
    QBI_LOG_E_0("Missing PLMN name TLV in successful response!");
  }
  else
  {
    provider_field = (qbi_mbim_offset_size_pair_s *)
      ((uint8 *) qmi_txn->parent->rsp.data + sizeof(qbi_svc_bc_provider_list_s) +
       (info->cur_index * sizeof(qbi_mbim_offset_size_pair_s)));

    plmn_mode = qbi_svc_atds_operators_qmi_rat_to_plmn_mode(
      info->ntw[info->cur_index].rat);
    provider_name_len = qbi_svc_bc_qmi_plmn_name_to_provider_name(
      qmi_rsp, provider_name, sizeof(provider_name),
      QBI_SVC_BC_PROVIDER_NAME_PREF_PLMN_NAME);
    provider_state =
      qbi_svc_bc_nas_visible_providers_qmi_ntw_status_to_provider_state(
        info->ntw[info->cur_index].network_status);
    if (!qbi_svc_atds_operators_add(
          qmi_txn->parent, provider_field, info->ntw[info->cur_index].mcc,
          info->ntw[info->cur_index].mnc,
          info->ntw[info->cur_index].mnc_is_3_digits, provider_state,
          provider_name, provider_name_len, FALSE, plmn_mode,
          QBI_SVC_BC_RSSI_UNKNOWN, QBI_SVC_BC_ERROR_RATE_UNKNOWN))
    {
      QBI_LOG_E_0("Couldn't add provider!");
    }
    else
    {
      info->cur_index++;
      action = qbi_svc_bc_nas_visible_providers_q_get_next_plmn_name(
        qmi_txn->parent, qmi_txn);
    }
  }

  return action;
} /* qbi_svc_atds_operators_q_nas44_rsp_cb() */

/*! @} */

/*===========================================================================
  FUNCTION: qbi_svc_atds_init
===========================================================================*/
/*!
    @brief One-time initialization of the AT&T device service

    @details
*/
/*=========================================================================*/
void qbi_svc_atds_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_atds_cfg = {
    {
      0x59, 0x67, 0xbd, 0xcc, 0x7f, 0xd2, 0x49, 0xa2,
      0x9f, 0x5c, 0xb2, 0xe7, 0x0e, 0x52, 0x7d, 0xb3
     },
    QBI_SVC_ID_ATDS,
    TRUE,
    qbi_svc_atds_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_atds_cmd_hdlr_tbl),
    qbi_svc_atds_open,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_atds_cfg);
} /* qbi_svc_atds_init() */

/*===========================================================================
  FUNCTION: qbi_svc_atds_reset
===========================================================================*/
/*!
    @brief If active, resets and deactivates ATDS signal projection

    @details

    @param ctx
*/
/*=========================================================================*/
void qbi_svc_atds_reset
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_atds_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_atds_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache->projection_tables.count > 0)
  {
    QBI_LOG_I_0("Resetting ATDS signal projection");
    QBI_MEMSET(&(cache->projection_tables), 0, sizeof(cache->projection_tables));
    qbi_svc_bc_nas_signal_state_custom_mapping_reset_all(ctx);
    qbi_svc_bc_nas_signal_state_custom_mapping_apply(ctx);
  }
} /* qbi_svc_atds_reset() */

