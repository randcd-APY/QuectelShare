/*!
  @file
  qbi_svc_auth.c

  @brief
  Authentication device service implementation

  @note
  MBIM uses the opposite byte order as compared to 3GPP authentication
  specifications. Since QMI UIM follows 3GPP, byte order reversal is necessary
  for all authentication data passed between MBIM CIDs and QMI UIM.
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
04/25/14  hz   Add auth support for ICC sim
04/23/13  hz   Correct ik/ck extraction order
04/03/13  hz   Fix AKA response data boundary checking issue
06/14/12  bd   Reverse byte order of all authentication fields per MBIM Errata
01/20/12  hz   Initial release based on MBIM v1.0
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_auth.h"
#include "qbi_svc_auth_mbim.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_mem.h"
#include "qbi_qmi_defs.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_svc_bc.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_svc_bc_nas.h"
#include "qbi_svc_bc_sim.h"
#include "qbi_txn.h"

#include "user_identity_module_v01.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/
#define QBI_SVC_AUTH_AKA_SUCCESS_TAG          (0xDB)
#define QBI_SVC_AUTH_AKA_SYNC_FAILURE_TAG     (0xDC)
#define QBI_SVC_AUTH_AKA_AMF_OFFSET           (0x06)
#define QBI_SVC_AUTH_AKA_AMF_SEPARATION_BMASK (0x8000)
#define QBI_SVC_AUTH_AKA_SQN_LEN              (0x06)

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_action_e qbi_svc_auth_open
(
  qbi_txn_s *txn
);

/*! @addtogroup MBIM_CID_AKA_AUTH
    @{ */

static boolean qbi_svc_auth_aka_extract_auts
(
  const uint8 *data,
  uint32       data_len,
  uint8       *auts
);

static boolean qbi_svc_auth_aka_extract_res_ik_ck
(
  const uint8 *data,
  uint32       data_len,
  uint8       *res,
  uint8       *res_len,
  uint8       *ik,
  uint8       *ck
);

static boolean qbi_svc_auth_aka_extract_tag
(
  const uint8 *data,
  uint32       data_len,
  uint8       *tag
);

static boolean qbi_svc_auth_aka_is_amf_set
(
  const uint8 *autn
);

static boolean qbi_svc_auth_aka_pack_authenticate_data
(
  const uint8 *rand,
  const uint8 *autn,
  uint8       *data,
  uint32_t    *data_len
);

static qbi_svc_action_e qbi_svc_auth_aka_auth_q_build_rsp
(
  qbi_txn_s *txn,
  uint8_t   *content,
  uint32_t   content_len
);

static qbi_svc_action_e qbi_svc_auth_aka_auth_q_uim34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_auth_aka_auth_q_req
(
  qbi_txn_s *txn
);
/*! @} */

/*! @addtogroup MBIM_CID_SIM_AUTH
    @{ */

static qbi_svc_action_e qbi_svc_auth_sim_auth_build_next_uim34_req
(
  qbi_txn_s *txn,
  boolean    is_uicc
);

static qbi_svc_action_e qbi_svc_auth_sim_auth_cb_get_action
(
  qbi_txn_s *txn,
  boolean    is_uicc
);

static qbi_svc_action_e qbi_svc_auth_sim_auth_q_uim34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_auth_sim_auth_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_auth_sim_auth_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_auth_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
/*! @todo Remove set support once host connects AUTH command to query */
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_auth_cmd_hdlr_tbl[] = {
  {qbi_svc_auth_aka_auth_q_req,  sizeof(qbi_svc_auth_aka_auth_q_req_s),
     qbi_svc_auth_aka_auth_q_req,  sizeof(qbi_svc_auth_aka_auth_q_req_s)},
  /*! @todo Add AKA Prime authentication support */
  {NULL, 0, NULL, 0},
  {qbi_svc_auth_sim_auth_q_req,  sizeof(qbi_svc_auth_sim_auth_q_req_s),
     qbi_svc_auth_sim_auth_q_req,  sizeof(qbi_svc_auth_sim_auth_q_req_s)}
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_auth_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_auth_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_UIM))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_auth_open() */

/*! @addtogroup MBIM_CID_AKA_AUTH
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_auth_aka_extract_auts
===========================================================================*/
/*!
    @brief Extract auts from aka response data

    @details

    @param data buffer containing aka response data
    @param aka response data length
    @param data buffer to store extracted auts

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_auth_aka_extract_auts
(
  const uint8 *data,
  uint32       data_len,
  uint8       *auts
)
{
  uint8 auts_len;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(data);
  QBI_CHECK_NULL_PTR_RET_FALSE(auts);

  /* 3GPP TS 31.103 V9 7.1.2.1 */
  if (data_len < 2 || data_len > QBI_SVC_AUTH_AUTS_LEN + 2)
  {
    QBI_LOG_E_1("Invalid sync failure response length %d!", data_len);
    return FALSE;
  }

  auts_len = data[1];
  if (auts_len != data_len - 2)
  {
    QBI_LOG_E_1("Invalid AUTS data length %d!", auts_len);
    return FALSE;
  }

  qbi_mem_reverse_bytes(data + 2, auts, auts_len);

  return TRUE;
} /* qbi_svc_auth_aka_extract_auts */

/*===========================================================================
  FUNCTION: qbi_svc_auth_aka_extract_res_ik_ck
===========================================================================*/
/*!
    @brief Extract res/ik/ck from aka response data

    @details

    @param data buffer containing aka response data
    @param aka response data length
    @param buffer to store extracted res
    @param buffer to store res length
    @param data buffer to store extracted ik
    @param data buffer to store extracted ck

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_auth_aka_extract_res_ik_ck
(
  const uint8 *data,
  uint32       data_len,
  uint8       *res,
  uint8       *res_len,
  uint8       *ik,
  uint8       *ck
)
{
  uint8 ik_len;
  uint8 ck_len;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(data);
  QBI_CHECK_NULL_PTR_RET_FALSE(res);
  QBI_CHECK_NULL_PTR_RET_FALSE(res_len);
  QBI_CHECK_NULL_PTR_RET_FALSE(ik);
  QBI_CHECK_NULL_PTR_RET_FALSE(ck);

  /* 3GPP TS 31.102 V9 7.1.2.1 */
  if (data_len < 4 ||
      data_len >
        QBI_SVC_AUTH_RES_LEN + QBI_SVC_AUTH_IK_LEN + QBI_SVC_AUTH_CK_LEN +
        QBI_SVC_AUTH_KC_LEN + 5)
  {
    QBI_LOG_E_1("Invalid Successful operation response length %d!", data_len);
    return FALSE;
  }

  *res_len = data[1];
  if (*res_len > QBI_SVC_AUTH_RES_LEN)
  {
    QBI_LOG_E_1("Invalid RES data length %d!", *res_len);
    return FALSE;
  }

  ck_len = data[*res_len + 2];
  if (ck_len > QBI_SVC_AUTH_CK_LEN)
  {
    QBI_LOG_E_1("Invalid CK data length %d!", ck_len);
    return FALSE;
  }

  ik_len = data[*res_len + ck_len + 3];
  if (ik_len > QBI_SVC_AUTH_IK_LEN)
  {
    QBI_LOG_E_1("Invalid IK data length %d!", ik_len);
    return FALSE;
  }

  /*! @note MBIM has a fixed length buffer for RES but it also has a ResLen
     field that says how many bytes in RES are used. */
  qbi_mem_reverse_bytes(data + 2, res, *res_len);
  qbi_mem_reverse_bytes(data + *res_len + 3, ck, ck_len);
  qbi_mem_reverse_bytes(data + *res_len + ck_len + 4, ik, ik_len);

  return TRUE;
} /* qbi_svc_auth_aka_extract_res_ik_ck */

/*===========================================================================
  FUNCTION: qbi_svc_auth_aka_extract_tag
===========================================================================*/
/*!
    @brief Extract operation status tag from aka response data

    @details

    @param data buffer containing aka response data
    @param aka response data length
    @param operation status tag

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_auth_aka_extract_tag
(
  const uint8 *data,
  uint32       data_len,
  uint8       *tag
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(data);
  QBI_CHECK_NULL_PTR_RET_FALSE(tag);

  /* 3GPP TS 31.102 V9 7.1.2.1 */
  if (data_len < 2)
  {
    QBI_LOG_E_1("Invalid aka response length %d!", data_len);
    return FALSE;
  }

  /* Status tag is the first byte in aka response*/
  *tag = data[0];

  return TRUE;
} /* qbi_svc_auth_aka_extract_tag */

/*===========================================================================
  FUNCTION: qbi_svc_auth_aka_is_amf_set
===========================================================================*/
/*!
    @brief Check if amf separation bit is set in autn

    @details

    @param data buffer containing autn, in 3GPP byte order (not MBIM)

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_auth_aka_is_amf_set
(
  const uint8 *autn
)
{
  uint16 amf_value;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(autn);

  /* 3GPP TS 33.102 V9 Annex H */
  amf_value = *((uint16*)(autn + QBI_SVC_AUTH_AKA_AMF_OFFSET));
  amf_value = QBI_MEM_NTOHS(amf_value);

  return (amf_value & QBI_SVC_AUTH_AKA_AMF_SEPARATION_BMASK) ? TRUE : FALSE;
} /* qbi_svc_auth_aka_is_amf_set */

/*===========================================================================
  FUNCTION: qbi_svc_auth_aka_pack_authenticate_data
===========================================================================*/
/*!
    @brief Extract operation status tag from aka response data

    @details
    Performs byte reversal between CID data and QMI data

    @param data buffer containing rand data from CID (MBIM byte order)
    @param data buffer containing autn data from CID (MBIM byte order)
    @param data buffer to store packed authentication data for QMI UIM
    (3GPP byte order)
    @param authentication data length

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_auth_aka_pack_authenticate_data
(
  const uint8 *rand,
  const uint8 *autn,
  uint8       *data,
  uint32_t    *data_len
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(rand);
  QBI_CHECK_NULL_PTR_RET_FALSE(autn);
  QBI_CHECK_NULL_PTR_RET_FALSE(data);
  QBI_CHECK_NULL_PTR_RET_FALSE(data_len);

  /* 3GPP TS 31.102 Rel9 7.1.2.1 */
  *data_len = QBI_SVC_AUTH_RAND_LEN + QBI_SVC_AUTH_AUTN_LEN + 2;
  data[0] = QBI_SVC_AUTH_RAND_LEN;
  qbi_mem_reverse_bytes(rand, data + 1, QBI_SVC_AUTH_RAND_LEN);
  data[QBI_SVC_AUTH_RAND_LEN + 1] = QBI_SVC_AUTH_AUTN_LEN;
  qbi_mem_reverse_bytes(
    autn, data + QBI_SVC_AUTH_RAND_LEN + 2, QBI_SVC_AUTH_AUTN_LEN);

  /* After byte order restored to 3GPP spec, check if AMF is set in AUTN
     (informational/debugging purposes only) */
  if (!qbi_svc_auth_aka_is_amf_set(data + QBI_SVC_AUTH_RAND_LEN + 2))
  {
    QBI_LOG_I_0("AMF is not set in AUTN");
  }

  return TRUE;
} /* qbi_svc_auth_aka_pack_authenticate_data */

/*===========================================================================
  FUNCTION: qbi_svc_auth_aka_auth_q_build_rsp
===========================================================================*/
/*!
    @brief Build MBIM_AKA_AUTH_INFO based on aka response content

    @details

    @param qbi_txn_s

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_auth_aka_auth_q_build_rsp
(
  qbi_txn_s *txn,
  uint8_t   *content,
  uint32_t   content_len
)
{
  qbi_svc_auth_aka_auth_info_rsp_s *rsp;
  uint8 res_len;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint8 tag;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(content);

  rsp = (qbi_svc_auth_aka_auth_info_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_auth_aka_auth_info_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  if (!qbi_svc_auth_aka_extract_tag(content, content_len, &tag))
  {
    QBI_LOG_E_0("Cannot extract aka response tag!");
  }
  else
  {
    /* Successful 3G authentication tag */
    if (tag == QBI_SVC_AUTH_AKA_SUCCESS_TAG)
    {
      if (qbi_svc_auth_aka_extract_res_ik_ck(
            content, content_len, rsp->res, &res_len, rsp->ik, rsp->ck))
      {
        rsp->res_len = res_len;
        action = QBI_SVC_ACTION_SEND_RSP;
      }
    }
    else if (tag == QBI_SVC_AUTH_AKA_SYNC_FAILURE_TAG)
    {
      if (qbi_svc_auth_aka_extract_auts(content, content_len, rsp->auts))
      {
        txn->status = QBI_MBIM_STATUS_AUTH_SYNC_FAILURE;
        action = QBI_SVC_ACTION_SEND_RSP;
      }
    }
    else
    {
      QBI_LOG_E_1("Unknown aka response tag %d!", tag);
    }
  }

  return action;
} /* qbi_svc_auth_aka_auth_q_build_rsp */

/*===========================================================================
  FUNCTION: qbi_svc_auth_aka_auth_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_AKA_AUTH query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_auth_aka_auth_q_req
(
  qbi_txn_s *txn
)
{
  uim_authenticate_req_msg_v01 *qmi_req;
  qbi_svc_auth_aka_auth_q_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  if (!qbi_svc_bc_check_device_state(txn, TRUE, FALSE))
  {
    QBI_LOG_E_0("Not ready for authentication operation");
  }
  else
  {
    req = (qbi_svc_auth_aka_auth_q_req_s *) txn->req.data;
    QBI_CHECK_NULL_PTR_RET_ABORT(req->autn);

    /* QMI_UIM_AUTHENTICATE (0x34) - AKA security */
    qmi_req = (uim_authenticate_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_UIM,
                                    QMI_UIM_AUTHENTICATE_REQ_V01,
                                    qbi_svc_auth_aka_auth_q_uim34_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->session_information.session_type =
      UIM_SESSION_TYPE_PRIMARY_GW_V01;
    qmi_req->session_information.aid_len = 0;
    qmi_req->authentication_data.context =
      UIM_AUTH_CONTEXT_3G_SEC_V01;

    if (qbi_svc_auth_aka_pack_authenticate_data(
          req->rand, req->autn, qmi_req->authentication_data.data,
          &qmi_req->authentication_data.data_len))
    {
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_auth_aka_auth_q_req */

/*===========================================================================
  FUNCTION: qbi_svc_auth_aka_auth_q_uim34_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_UIM_AUTHENTICATE_RESP for MBIM_CID_AKA_AUTH query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_auth_aka_auth_q_uim34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_authenticate_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (uim_authenticate_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    switch (qmi_rsp->resp.error)
    {
      case QMI_ERR_MALFORMED_MSG_V01:
      case QMI_ERR_INVALID_ARG_V01:
        qmi_txn->parent->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
        break;

      case QMI_ERR_AUTHENTICATION_FAILED_V01:
        /*! @note There is no spacific QMI error for INCORRECT_AUTN.
            For now assume authentication failure is caused by incorrect AUTN.
            Lower layer change is needed if there are reasons for
            authentication to fail in addition to incorrect AUTN. */
        qmi_txn->parent->status = QBI_MBIM_STATUS_AUTH_INCORRECT_AUTN;
        break;

      default:
        break;
    }
  }
  else
  {
    if (qmi_rsp->content_valid)
    {
      action = qbi_svc_auth_aka_auth_q_build_rsp(
        qmi_txn->parent, qmi_rsp->content, qmi_rsp->content_len);
    }
    else
    {
      QBI_LOG_E_0("Cannot locate AKA response content!");
    }
  }

  return action;
} /* qbi_svc_auth_aka_auth_q_uim34_rsp_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_SIM_AUTH
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_auth_sim_auth_build_next_uim34_req
===========================================================================*/
/*!
    @brief Build QMI UIM SIM authenticate request with next available rand

    @details

    @param qmi_txn
    @param is_uicc Set to TRUE if UIM app type is UICC

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_auth_sim_auth_build_next_uim34_req
(
  qbi_txn_s *txn,
  boolean    is_uicc
)
{
  uim_authenticate_req_msg_v01 *qmi_req;
  qbi_svc_auth_sim_auth_q_req_s *req;
  qbi_svc_auth_sim_auth_info_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_QMI_REQ;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->rsp.data);

  req = (qbi_svc_auth_sim_auth_q_req_s *) txn->req.data;
  rsp = (qbi_svc_auth_sim_auth_info_rsp_s *) txn->rsp.data;

  /* QMI_UIM_AUTHENTICATE (0x34) - GSM security */
  qmi_req = (uim_authenticate_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_AUTHENTICATE_REQ_V01,
      qbi_svc_auth_sim_auth_q_uim34_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->session_information.session_type =
    UIM_SESSION_TYPE_PRIMARY_GW_V01;
  qmi_req->session_information.aid_len = 0;

  if (is_uicc)
  {
    qmi_req->authentication_data.context = UIM_AUTH_CONTEXT_GSM_SEC_V01;
    qmi_req->authentication_data.data_len = QBI_SVC_AUTH_RAND_LEN + 1;
    qmi_req->authentication_data.data[0] = QBI_SVC_AUTH_RAND_LEN;

    if (rsp->n == 0)
    {
      qbi_mem_reverse_bytes(
        req->rand1, qmi_req->authentication_data.data + 1, QBI_SVC_AUTH_RAND_LEN);
    }
    else if (rsp->n == 1)
    {
      qbi_mem_reverse_bytes(
        req->rand2, qmi_req->authentication_data.data + 1, QBI_SVC_AUTH_RAND_LEN);
    }
    else if (rsp->n == 2 && req->n == 3)
    {
      qbi_mem_reverse_bytes(
        req->rand3, qmi_req->authentication_data.data + 1, QBI_SVC_AUTH_RAND_LEN);
    }
    else
    {
      QBI_LOG_E_2("Unexpected number of challenge %d or response %d",
                  req->n, rsp->n);
      action = QBI_SVC_ACTION_ABORT;
    }
  }
  else
  {
    qmi_req->authentication_data.context = UIM_AUTH_CONTEXT_RUN_GSM_ALG_V01;
    qmi_req->authentication_data.data_len = QBI_SVC_AUTH_RAND_LEN;

    if (rsp->n == 0)
    {
      qbi_mem_reverse_bytes(
        req->rand1, qmi_req->authentication_data.data, QBI_SVC_AUTH_RAND_LEN);
    }
    else if (rsp->n == 1)
    {
      qbi_mem_reverse_bytes(
        req->rand2, qmi_req->authentication_data.data, QBI_SVC_AUTH_RAND_LEN);
    }
    else if (rsp->n == 2 && req->n == 3)
    {
      qbi_mem_reverse_bytes(
        req->rand3, qmi_req->authentication_data.data, QBI_SVC_AUTH_RAND_LEN);
    }
    else
    {
      QBI_LOG_E_2("Unexpected number of challenge %d or response %d",
                  req->n, rsp->n);
      action = QBI_SVC_ACTION_ABORT;
    }
  }

  return action;
} /* qbi_svc_auth_sim_auth_build_next_uim34_req */

/*===========================================================================
  FUNCTION: qbi_svc_auth_sim_auth_cb_get_action
===========================================================================*/
/*!
    @brief Determines the proper action based on finished authentication
           challenges

    @details

    @param qmi_txn
    @param is_uicc Set to TRUE if UIM app type is UICC

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_auth_sim_auth_cb_get_action
(
  qbi_txn_s *txn,
  boolean    is_uicc
)
{
  qbi_svc_auth_sim_auth_q_req_s *req;
  qbi_svc_auth_sim_auth_info_rsp_s *rsp;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->rsp.data);

  req = (qbi_svc_auth_sim_auth_q_req_s *) txn->req.data;
  rsp = (qbi_svc_auth_sim_auth_info_rsp_s *) txn->rsp.data;
  if (req->n == rsp->n)
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    action = qbi_svc_auth_sim_auth_build_next_uim34_req(txn, is_uicc);
  }

  return action;
} /* qbi_svc_auth_sim_auth_cb_get_action() */

/*===========================================================================
  FUNCTION: qbi_svc_auth_sim_auth_q_uim34_rsp_cb
===========================================================================*/
/*!
    @brief Process a QMI_UIM_AUTHENTICATE_RESP to extract SRES and KC

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_auth_sim_auth_q_uim34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  uim_authenticate_req_msg_v01 *qmi_req;
  uim_authenticate_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_auth_sim_auth_info_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);

  qmi_req = (uim_authenticate_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (uim_authenticate_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else
  {
    if (qmi_rsp->content_valid)
    {
      /* Response from 2G SIM contains 12 bytes data as specified in 3GPP
         TS 51.011, clause 9.2.16. Response from 3G USIM card contains 14
         bytes data as specified in 3GPP TS 31.102, clause 7.1.2.1. Extract
         SRES and Kc accordingly based on response data length. */
      if (qmi_req->authentication_data.context ==
            UIM_AUTH_CONTEXT_GSM_SEC_V01 &&
          qmi_rsp->content_len == QBI_SVC_AUTH_SRES_LEN + QBI_SVC_AUTH_KC_LEN + 2 &&
          qmi_rsp->content[0] == QBI_SVC_AUTH_SRES_LEN &&
          qmi_rsp->content[QBI_SVC_AUTH_SRES_LEN + 1] == QBI_SVC_AUTH_KC_LEN )
      {
        rsp = (qbi_svc_auth_sim_auth_info_rsp_s *) qmi_txn->parent->rsp.data;
        if (rsp->n == 0)
        {
          qbi_mem_reverse_bytes(
            qmi_rsp->content + 1, rsp->sres1, QBI_SVC_AUTH_SRES_LEN);
          qbi_mem_reverse_bytes(
            qmi_rsp->content + QBI_SVC_AUTH_SRES_LEN + 2, rsp->kc1,
            QBI_SVC_AUTH_KC_LEN);
        }
        else if (rsp->n == 1)
        {
          qbi_mem_reverse_bytes(
            qmi_rsp->content + 1, rsp->sres2, QBI_SVC_AUTH_SRES_LEN);
          qbi_mem_reverse_bytes(
            qmi_rsp->content + QBI_SVC_AUTH_SRES_LEN + 2, rsp->kc2,
            QBI_SVC_AUTH_KC_LEN);
        }
        else if (rsp->n == 2)
        {
          qbi_mem_reverse_bytes(
            qmi_rsp->content + 1, rsp->sres3, QBI_SVC_AUTH_SRES_LEN);
          qbi_mem_reverse_bytes(
            qmi_rsp->content + QBI_SVC_AUTH_SRES_LEN + 2, rsp->kc3,
            QBI_SVC_AUTH_KC_LEN);
        }
        rsp->n++;
        action = qbi_svc_auth_sim_auth_cb_get_action(qmi_txn->parent, TRUE);
      }
      else if (qmi_req->authentication_data.context ==
                 UIM_AUTH_CONTEXT_RUN_GSM_ALG_V01 &&
               qmi_rsp->content_len ==
                 QBI_SVC_AUTH_SRES_LEN + QBI_SVC_AUTH_KC_LEN)
      {
        rsp = (qbi_svc_auth_sim_auth_info_rsp_s *) qmi_txn->parent->rsp.data;
        if (rsp->n == 0)
        {
          qbi_mem_reverse_bytes(
            qmi_rsp->content, rsp->sres1, QBI_SVC_AUTH_SRES_LEN);
          qbi_mem_reverse_bytes(
            qmi_rsp->content + QBI_SVC_AUTH_SRES_LEN, rsp->kc1,
            QBI_SVC_AUTH_KC_LEN);
        }
        else if (rsp->n == 1)
        {
          qbi_mem_reverse_bytes(
            qmi_rsp->content, rsp->sres2, QBI_SVC_AUTH_SRES_LEN);
          qbi_mem_reverse_bytes(
            qmi_rsp->content + QBI_SVC_AUTH_SRES_LEN, rsp->kc2,
            QBI_SVC_AUTH_KC_LEN);
        }
        else if (rsp->n == 2)
        {
          qbi_mem_reverse_bytes(
            qmi_rsp->content, rsp->sres3, QBI_SVC_AUTH_SRES_LEN);
          qbi_mem_reverse_bytes(
            qmi_rsp->content + QBI_SVC_AUTH_SRES_LEN, rsp->kc3,
            QBI_SVC_AUTH_KC_LEN);
        }
        rsp->n++;
        action = qbi_svc_auth_sim_auth_cb_get_action(qmi_txn->parent, FALSE);
      }
      else
      {
        QBI_LOG_E_3("Invalid SIM auth response content, context %d valid %d len %d!",
                    qmi_req->authentication_data.context,
                    qmi_rsp->content_valid, qmi_rsp->content_len);
      }
    }
    else
    {
      QBI_LOG_E_0("No SIM auth response content TLV!");
    }
  }

  return action;
} /* qbi_svc_auth_sim_auth_q_uim34_rsp_cb */

/*===========================================================================
  FUNCTION: qbi_svc_auth_sim_auth_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SIM_AUTH query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_auth_sim_auth_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_auth_sim_auth_q_req_s *req;
  qbi_svc_auth_sim_auth_info_rsp_s *rsp;
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_auth_sim_auth_q_req_s *) txn->req.data;
  if (!qbi_svc_bc_check_device_state(txn, TRUE, FALSE))
  {
    QBI_LOG_E_0("Not ready for authentication operation");
  }
  else if (req->n != 2 && req->n != 3)
  {
    QBI_LOG_E_1("Incorrect number of random number challenges %d!", req->n);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else
  {
    /* Pre-allocate response buffer, to be filled later in callbacks */
    rsp = (qbi_svc_auth_sim_auth_info_rsp_s *) qbi_txn_alloc_rsp_buf(
      txn, sizeof(qbi_svc_auth_sim_auth_info_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

    /* QMI_UIM_GET_CARD_STATUS (0x2f) */
    qmi_txn = qbi_qmi_txn_alloc(
      txn, QBI_QMI_SVC_UIM, QMI_UIM_GET_CARD_STATUS_REQ_V01,
      qbi_svc_auth_sim_auth_q_uim2f_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_auth_sim_auth_q_req */

/*===========================================================================
  FUNCTION: qbi_svc_auth_sim_auth_q_uim2f_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_UIM_GET_CARD_STATUS_RESP for MBIM_CID_SIM_AUTH
    query request

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_auth_sim_auth_q_uim2f_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uim_get_card_status_resp_msg_v01 *qmi_rsp;
  uint8 card_index;
  uint8 app_index;
  uim_app_type_enum_v01 app_type;
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
  else if (!qbi_svc_bc_sim_qmi_card_status_to_primary_app(
             qmi_txn->ctx, &qmi_rsp->card_status, &card_index, &app_index))
  {
    QBI_LOG_E_0("Card is ready but couldn't get primary app location!");
  }
  else
  {
    app_type =
      qmi_rsp->card_status.card_info[card_index].app_info[app_index].app_type;

    if (app_type == UIM_APP_TYPE_USIM_V01 ||
        app_type == UIM_APP_TYPE_CSIM_V01)
    {
      action = qbi_svc_auth_sim_auth_build_next_uim34_req(
        qmi_txn->parent, TRUE);
    }
    else
    {
      action = qbi_svc_auth_sim_auth_build_next_uim34_req(
        qmi_txn->parent, FALSE);
    }
  }

  return action;
} /* qbi_svc_auth_sim_auth_q_uim2f_rsp_cb() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_auth_init
===========================================================================*/
/*!
    @brief One-time initialization of the Authentication device service

    @details
*/
/*=========================================================================*/
void qbi_svc_auth_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_auth_cfg = {
    {
      0x1d, 0x2b, 0x5f, 0xf7, 0x0a, 0xa1, 0x48, 0xb2,
      0xaa, 0x52, 0x50, 0xf1, 0x57, 0x67, 0x17, 0x4e
    },
    QBI_SVC_ID_AUTH,
    FALSE,
    qbi_svc_auth_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_auth_cmd_hdlr_tbl),
    qbi_svc_auth_open,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_auth_cfg);
} /* qbi_svc_auth_init() */

