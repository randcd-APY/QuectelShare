/*!
  @file
  qbi_svc_ext_qmux.c

  @brief
  MBIM Extensibility Device Service providing tunneled access to QMI via
  encapsulating QMUX messages in an MBIM CID.
*/

/*=============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
07/21/17  rv   Fixed Memory Leak issue
04/17/12  bd   Move qmi_qmux_if function calls to new qbi_qmux framework layer
03/30/12  bd   Update to use new qmi_qmux_if APIs
12/09/11  bd   Added EXT_QMUX device service
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_ext_qmux.h"
#include "qbi_svc_ext_qmux_mbim.h"

#include "qbi_common.h"
#include "qbi_msg_mbim.h"
#include "qbi_os.h"
#include "qbi_qmux.h"
#include "qbi_svc.h"
#include "qbi_task.h"
#include "qbi_txn.h"
#include "qbi_util.h"

#include "control_service_v01.h"
#include "qmi_idl_lib.h"

/*=============================================================================

  Private Macros

=============================================================================*/

/*=============================================================================

  Private Typedefs and Constants

=============================================================================*/

/*! Long timeout (10m) for receiving QMI responses from the modem */
#define QBI_SVC_EXT_QMUX_TIMEOUT_MS  (600 * 1000)

/*! Device service local cache struct */
typedef struct {
  /*! List containing one qbi_svc_ext_qmux_qmi_client_id_s element for each
      client allocated to the host. */
  qbi_util_list_s host_client_id_list;

  /*! Transaction ID used with QMI_CTL requests allowing QMI_CTL responses to
      be properly tied to their CID transactions. */
  uint8 qmi_ctl_txn_id;
} qbi_svc_ext_qmux_cache_s;

/*! Information structure used to link QBI transactions to QMI requests */
typedef struct {
  /*! Set to TRUE if this transaction is related to a QMI_CTL message, so the
      qmi_ctl struct is used in the req union, otherwise qmux is used */
  boolean is_qmi_ctl;

  union {
    struct {
      uint8 host_txn_id;
      uint8 qbi_txn_id;
    } qmi_ctl;

    struct {
      uint8  svc_type;
      uint8  client_id;
      uint16 txn_id;
    } qmux;
  } req;
} qbi_svc_ext_qmux_info_s;

/*! QMI client ID and its associated QMI service */
typedef struct {
  /*! Must be first as we alias */
  qbi_util_list_entry_s list_entry;

  uint8 svc_type;
  uint8 client_id;
} qbi_svc_ext_qmux_qmi_client_id_s;

#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! Structure of a QMI_CTL_RELEASE_CLIENT_ID_REQ SDU sent to the QMUX API,
    including header space for QMUX to use */
typedef PACK(struct) {
  qbi_qmux_if_type_t         if_type;
  qbi_qmux_hdr_s             qmux_hdr;
  qbi_qmux_qmi_ctl_sdu_s     ctl_sdu;
  PACK(struct) {
    uint8  type;
    uint16 length;
    uint8  qmi_svc_type;
    uint8  client_id;
  } client_id_tlv;
} qbi_svc_ext_qmux_qmi_ctl_release_client_id_msg_s;

#ifdef _WIN32
#pragma pack(pop)
#endif

/*! TLV type of the Client ID mandatory TLV in QMI_CTL_RELEASE_CLIENT_ID_REQ */
#define QBI_SVC_EXT_QMUX_QMI_CTL23_REQ_TLV_TYPE_CLIENT_ID   (0x01)

/*! TLV length of the Client ID TLV in QMI_CTL_RELEASE_CLIENT_ID_REQ */
#define QBI_SVC_EXT_QMUX_QMI_CTL23_REQ_TLV_LENGTH_CLIENT_ID (0x0002)

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_ext_qmux_cache_s *qbi_svc_ext_qmux_cache_get
(
  const qbi_ctx_s *ctx
);

static boolean qbi_svc_ext_qmux_qmi_ctl_client_add
(
  qbi_ctx_s *ctx,
  uint8      qmi_svc_type,
  uint8      qmi_client_id
);

static boolean qbi_svc_ext_qmux_qmi_ctl_client_delete
(
  qbi_ctx_s *ctx,
  uint8      qmi_svc_type,
  uint8      qmi_client_id
);

static boolean qbi_svc_ext_qmux_client_is_registered
(
  qbi_ctx_s *ctx,
  uint8      qmi_svc_type,
  uint8      qmi_client_id
);

static void qbi_svc_ext_qmux_qmi_ctl_client_release
(
  qbi_ctx_s                              *ctx,
  const qbi_svc_ext_qmux_qmi_client_id_s *client_id
);

static boolean qbi_svc_ext_qmux_qmi_ctl_client_release_all
(
  qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_ext_qmux_close
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_ext_qmux_open
(
  qbi_txn_s *txn
);

static void qbi_svc_ext_qmux_qmi_ctl_client_update
(
  qbi_ctx_s                  *ctx,
  const qbi_util_buf_const_s *buf
);

static boolean qbi_svc_ext_qmux_qmi_ctl_decode_client_info
(
  uint16                       qmi_msg_id,
  qmi_idl_type_of_message_type idl_msg_type,
  const qbi_util_buf_const_s  *raw_qmi_ctl_msg,
  ctl_svc_clid_type_v01       *client_info
);

static boolean qbi_svc_ext_qmux_qmi_ctl_extract_client_info
(
  uint16                 qmi_msg_id,
  const qbi_util_buf_s  *decoded_buf,
  ctl_svc_clid_type_v01 *client_info
);

static qmi_idl_type_of_message_type qbi_svc_ext_qmux_qmi_ctl_svc_flags_to_idl_msg_type
(
  uint8 svc_ctl_flags
);

static void qbi_svc_ext_qmux_rx_cb
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
);

static void qbi_svc_ext_qmux_rx_qmi_ctl
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
);

static void qbi_svc_ext_qmux_rx_qmux_sdu
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
);

/*! @addtogroup MBIM_CID_QMUX_DATA
    @{ */

static void qbi_svc_ext_qmux_data_e_send_event
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
);

static void qbi_svc_ext_qmux_data_es_send_rsp
(
  qbi_txn_s      *txn,
  qbi_util_buf_s *buf
);

static qbi_svc_action_e qbi_svc_ext_qmux_data_s_req
(
  qbi_txn_s *txn
);

static void qbi_svc_ext_qmux_data_s_timeout_cb
(
  qbi_txn_s *txn
);

/*! @} */

/*=============================================================================

  Private Variables

=============================================================================*/

/*! @brief CID handler dispatch table
    @detailsOrder must match qbi_svc_ext_qmux_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_ext_qmux_cmd_hdlr_tbl[] = {
  /* MBIM_CID_QMUX_DATA */
  {NULL, 0,
   qbi_svc_ext_qmux_data_s_req, QBI_QMUX_MIN_MSG_LEN_BYTES},
};

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_sms_get_cache
===========================================================================*/
/*!
    @brief Returns a pointer to the device service's cache

    @details

    @param ctx

    @return qbi_svc_ext_qmux_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
static qbi_svc_ext_qmux_cache_s *qbi_svc_ext_qmux_cache_get
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_ext_qmux_cache_s *) qbi_svc_cache_get(
    ctx, QBI_SVC_ID_EXT_QMUX);
} /* qbi_svc_ext_qmux_cache_get() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_qmi_ctl_client_add
===========================================================================*/
/*!
    @brief Adds a new client ID to the collection of active client IDs
    allocated to the host

    @details

    @param ctx
    @param qmi_svc_type
    @param qmi_client_id

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_ext_qmux_qmi_ctl_client_add
(
  qbi_ctx_s *ctx,
  uint8      qmi_svc_type,
  uint8      qmi_client_id
)
{
  boolean success = FALSE;
  qbi_svc_ext_qmux_cache_s *cache;
  qbi_svc_ext_qmux_qmi_client_id_s *client_id;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ext_qmux_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  client_id = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_ext_qmux_qmi_client_id_s));
  QBI_CHECK_NULL_PTR_RET_FALSE(client_id);

  /*! @note Just using a list since we expect a QMI traffic to be low volume,
      and there to be only a few client IDs allocated to the host at a time.
      If this proves to be a performance problem, should switch to a data
      structure with faster lookup, e.g. hash table. */
  client_id->svc_type  = qmi_svc_type;
  client_id->client_id = qmi_client_id;
  if (!qbi_util_list_push_back_aliased(
        &cache->host_client_id_list, &client_id->list_entry))
  {
    QBI_LOG_E_2("Couldn't add client ID %d (service ID %d) to active list!",
                qmi_client_id, qmi_svc_type);
    QBI_MEM_FREE(client_id);
  }
  else
  {
    QBI_LOG_I_2("Saved svc_type %d client_id %d to host QMI client ID list",
                qmi_svc_type, qmi_client_id);
    success = TRUE;
  }

  return success;
} /* qbi_svc_ext_qmux_qmi_ctl_client_add() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_qmi_ctl_client_delete
===========================================================================*/
/*!
    @brief Removes a  client ID to the collection of active client IDs
    allocated to the host

    @details

    @param ctx
    @param qmi_svc_type
    @param qmi_client_id

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_ext_qmux_qmi_ctl_client_delete
(
  qbi_ctx_s *ctx,
  uint8      qmi_svc_type,
  uint8      qmi_client_id
)
{
  boolean success = FALSE;
  qbi_util_list_iter_s iter;
  qbi_svc_ext_qmux_cache_s *cache;
  qbi_svc_ext_qmux_qmi_client_id_s *client_id;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ext_qmux_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  qbi_util_list_iter_init(&cache->host_client_id_list, &iter);
  while ((client_id = (qbi_svc_ext_qmux_qmi_client_id_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (client_id->svc_type == qmi_svc_type &&
        client_id->client_id == qmi_client_id)
    {
      QBI_LOG_I_2("Removed svc_type %d client_id %d from host QMI client ID "
                  "list", qmi_svc_type, qmi_client_id);
      qbi_util_list_iter_remove(&iter);
      QBI_MEM_FREE(client_id);
    }
  }

  return success;
} /* qbi_svc_ext_qmux_qmi_ctl_client_delete() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_qmi_ctl_txn_id_get
===========================================================================*/
/*!
    @brief Returns a transaction ID for use with QMI_CTL requests

    @details

    @param ctx

    @return uint8 Transaction ID to use with the next QMI_CTL request, or 0
    on failure
*/
/*=========================================================================*/
static uint8 qbi_svc_ext_qmux_qmi_ctl_txn_id_get
(
  const qbi_ctx_s *ctx
)
{
  qbi_svc_ext_qmux_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ext_qmux_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  /* Increment the transaction ID, but skip 0 */
  while (++(cache->qmi_ctl_txn_id) == 0);

  return cache->qmi_ctl_txn_id;
} /* qbi_svc_ext_qmux_qmi_ctl_txn_id_get() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_client_is_registered
===========================================================================*/
/*!
    @brief Checks whether the host has registered a given client ID

    @details

    @param ctx
    @param qmi_svc_type
    @param qmi_client_id

    @return boolean TRUE if match found, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_ext_qmux_client_is_registered
(
  qbi_ctx_s *ctx,
  uint8      qmi_svc_type,
  uint8      qmi_client_id
)
{
  boolean found = FALSE;
  qbi_util_list_iter_s iter;
  qbi_svc_ext_qmux_cache_s *cache;
  qbi_svc_ext_qmux_qmi_client_id_s *client_id;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ext_qmux_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  qbi_util_list_iter_init(&cache->host_client_id_list, &iter);
  while ((client_id = (qbi_svc_ext_qmux_qmi_client_id_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    if (client_id->svc_type == qmi_svc_type &&
        client_id->client_id == qmi_client_id)
    {
      found = TRUE;
      break;
    }
  }

  return found;
} /* qbi_svc_ext_qmux_client_is_registered() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_qmi_ctl_client_release
===========================================================================*/
/*!
    @brief Sends a QMI_CTL_RELEASE_CLIENT_ID_REQ to the modem for the given
    client ID

    @details
    Sends the request to QMI but ignores the response.

    @param ctx
    @param client_id
*/
/*=========================================================================*/
static void qbi_svc_ext_qmux_qmi_ctl_client_release
(
  qbi_ctx_s                              *ctx,
  const qbi_svc_ext_qmux_qmi_client_id_s *client_id
)
{
  qbi_svc_ext_qmux_qmi_ctl_release_client_id_msg_s qmux_msg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(client_id);

  QBI_LOG_I_2("Sending QMI_CTL_RELEASE_CLIENT_ID_REQ request for svc_type %d "
              "client_id %d", client_id->svc_type, client_id->client_id);

  /* QMUX preamble and header */
  qmux_msg.if_type = QBI_QMUX_IF_TYPE_QMUX;
  qmux_msg.qmux_hdr.length    = sizeof(qmux_msg) - sizeof(qmux_msg.if_type);
  qmux_msg.qmux_hdr.ctl_flags = QBI_QMUX_CTL_FLAG_SENDER_CONTROL_POINT;
  qmux_msg.qmux_hdr.svc_type  = QBI_QMUX_SVC_TYPE_QMI_CTL;
  qmux_msg.qmux_hdr.client_id = QBI_QMUX_CLIENT_ID_QMI_CTL;

  /* QMI_CTL SDU header */
  qmux_msg.ctl_sdu.hdr.svc_ctl_flags = QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_REQUEST;
  qmux_msg.ctl_sdu.hdr.txn_id        = 0;

  /* QMI Message header: length is the size of the TLV data */
  qmux_msg.ctl_sdu.msg.msg_id     = QMI_CTL_RELEASE_CLIENT_ID_REQ_V01;
  qmux_msg.ctl_sdu.msg.msg_length = sizeof(qmux_msg.client_id_tlv);

  /* Mandatory TLV indicating the client ID to release */
  qmux_msg.client_id_tlv.type   =
    QBI_SVC_EXT_QMUX_QMI_CTL23_REQ_TLV_TYPE_CLIENT_ID;
  qmux_msg.client_id_tlv.length =
    QBI_SVC_EXT_QMUX_QMI_CTL23_REQ_TLV_LENGTH_CLIENT_ID;
  qmux_msg.client_id_tlv.qmi_svc_type = client_id->svc_type;
  qmux_msg.client_id_tlv.client_id    = client_id->client_id;

  if (!qbi_qmux_tx_to_modem(
        ctx, (qbi_qmux_msg_s *) &qmux_msg, sizeof(qmux_msg)))
  {
    QBI_LOG_E_0("Couldn't send release client ID request to modem");
  }
} /* qbi_svc_ext_qmux_qmi_ctl_client_release() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_qmi_ctl_client_release_all
===========================================================================*/
/*!
    @brief Releases resources for all QMI client IDs allocated by the host,
    including freeing associated memory and sending
    QMI_CTL_RELEASE_CLIENT_ID_REQ to the modem

    @details

    @param ctx

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_ext_qmux_qmi_ctl_client_release_all
(
  qbi_ctx_s *ctx
)
{
  qbi_util_list_iter_s iter;
  qbi_svc_ext_qmux_cache_s *cache;
  qbi_svc_ext_qmux_qmi_client_id_s *client_id;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_ext_qmux_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  qbi_util_list_iter_init(&cache->host_client_id_list, &iter);
  while ((client_id = (qbi_svc_ext_qmux_qmi_client_id_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    /*! @note After MBIM_CLOSE is issued, the host can't send any more CIDs,
        so we need to notify the modem to release all client IDs allocated to
        the host in order to prevent leaking their resources. */
    QBI_LOG_I_2("Removing svc_type %d client_id %d from host QMI client ID "
                "list", client_id->svc_type, client_id->client_id);
    qbi_svc_ext_qmux_qmi_ctl_client_release(ctx, client_id);
    qbi_util_list_iter_remove(&iter);
    QBI_MEM_FREE(client_id);
  }

  return TRUE;
} /* qbi_svc_ext_qmux_qmi_ctl_client_release_all() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_close
===========================================================================*/
/*!
    @brief Releases resources allocated by the device service

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ext_qmux_close
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (!qbi_svc_ext_qmux_qmi_ctl_client_release_all(txn->ctx))
  {
    QBI_LOG_E_0("Couldn't release external QMI clients!");
  }

  qbi_qmux_close(txn->ctx);
  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_ext_qmux_close() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ext_qmux_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (qbi_svc_cache_alloc(
        txn->ctx, QBI_SVC_ID_EXT_QMUX, sizeof(qbi_svc_ext_qmux_cache_s)) == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate cache");
  }
  else if (!qbi_qmux_open(txn->ctx, qbi_svc_ext_qmux_rx_cb))
  {
    QBI_LOG_E_0("Couldn't open raw QMUX connection!");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_ext_qmux_open() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_qmi_ctl_client_update
===========================================================================*/
/*!
    @brief Updates the local cache of active clients owned by the host based
    on the contents of QMI_CTL messages directed at the host

    @details

    @param ctx
    @param buf Buffer containing QMI_CTL message directed at the host
*/
/*=========================================================================*/
static void qbi_svc_ext_qmux_qmi_ctl_client_update
(
  qbi_ctx_s                  *ctx,
  const qbi_util_buf_const_s *buf
)
{
  const qbi_qmux_msg_s *qmux_msg;
  qmi_idl_type_of_message_type idl_msg_type;
  ctl_svc_clid_type_v01 client_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);
  QBI_CHECK_NULL_PTR_RET(buf->data);

  qmux_msg = (const qbi_qmux_msg_s *) buf->data;
  idl_msg_type = qbi_svc_ext_qmux_qmi_ctl_svc_flags_to_idl_msg_type(
    qmux_msg->sdu.qmi_ctl.hdr.svc_ctl_flags);
  if ((idl_msg_type == QMI_IDL_RESPONSE &&
       (qmux_msg->sdu.qmi_ctl.msg.msg_id == QMI_CTL_GET_CLIENT_ID_RESP_V01 ||
        qmux_msg->sdu.qmi_ctl.msg.msg_id == QMI_CTL_RELEASE_CLIENT_ID_RESP_V01)) ||
      (idl_msg_type == QMI_IDL_INDICATION &&
       (qmux_msg->sdu.qmi_ctl.msg.msg_id == QMI_CTL_REVOKE_CLIENT_ID_IND_V01 ||
        qmux_msg->sdu.qmi_ctl.msg.msg_id == QMI_CTL_INVALID_CLIENT_ID_IND_V01)))
  {
    if (qbi_svc_ext_qmux_qmi_ctl_decode_client_info(
          qmux_msg->sdu.qmi_ctl.msg.msg_id, idl_msg_type, buf, &client_info))
    {
      if (qmux_msg->sdu.qmi_ctl.msg.msg_id == QMI_CTL_GET_CLIENT_ID_RESP_V01)
      {
        qbi_svc_ext_qmux_qmi_ctl_client_add(
          ctx, client_info.qmi_svc_type, client_info.client_id);
      }
      else
      {
        qbi_svc_ext_qmux_qmi_ctl_client_delete(
          ctx, client_info.qmi_svc_type, client_info.client_id);
      }
    }
  }
} /* qbi_svc_ext_qmux_qmi_ctl_client_update() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_qmi_ctl_decode_client_info
===========================================================================*/
/*!
    @brief Decodes a raw QMUX QMI_CTL message into the relevant QMI service
    type + QMI client ID information

    @details

    @param qmi_msg_id
    @param idl_msg_type
    @param buf Buffer containing full raw QMUX message
    @param client_info

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_ext_qmux_qmi_ctl_decode_client_info
(
  uint16                       qmi_msg_id,
  qmi_idl_type_of_message_type idl_msg_type,
  const qbi_util_buf_const_s  *buf,
  ctl_svc_clid_type_v01       *client_info
)
{
  int32 idl_err;
  uint32_t decoded_size;
  qbi_util_buf_s decoded_buf;
  qmi_idl_service_object_type svc_obj;
  boolean success = FALSE;
  const uint8 *qmi_ctl_tlvs_start;
  uint32 qmi_ctl_tlvs_size;
  const qbi_qmux_msg_s *qmux_msg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(buf);
  QBI_CHECK_NULL_PTR_RET_FALSE(buf->data);

  qmux_msg = (const qbi_qmux_msg_s *) buf->data;
  qmi_ctl_tlvs_start = ((uint8 *) qmux_msg + QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES);
  qmi_ctl_tlvs_size = qmux_msg->sdu.qmi_ctl.msg.msg_length;
  if (qmi_ctl_tlvs_size > (buf->size - QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES))
  {
    QBI_LOG_E_2("Message length (%d) too large for actual amount of data (%d)",
                qmi_ctl_tlvs_size, (buf->size -
                                    QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES));
  }
  else
  {
    qbi_util_buf_init(&decoded_buf);
    svc_obj = ctl_get_service_object_v01();
    idl_err = qmi_idl_get_message_c_struct_len(
      svc_obj, idl_msg_type, qmi_msg_id, &decoded_size);
    if (idl_err != QMI_NO_ERR)
    {
      QBI_LOG_E_3("Error %d while trying to get decoded size for QMI_CTL "
                  "message ID 0x%04x (IDL msg type %d)", idl_err, qmi_msg_id,
                  idl_msg_type);
    }
    else if (qbi_util_buf_alloc(&decoded_buf, decoded_size) == NULL)
    {
      QBI_LOG_E_3("Couldn't allocate memory for decoding QMI_CTL message ID "
                  "0x%04x IDL msg type %d (needed %d bytes)", qmi_msg_id,
                  idl_msg_type, decoded_size);
    }
    else
    {
      idl_err = qmi_idl_message_decode(
        svc_obj, idl_msg_type, qmi_msg_id, qmi_ctl_tlvs_start,
        qmi_ctl_tlvs_size, decoded_buf.data, decoded_buf.size);
      if (idl_err != QMI_NO_ERR)
      {
        QBI_LOG_E_3("Error %d while trying to decode QMI_CTL message ID 0x%04x "
                    "IDL msg_type %d", idl_err, qmi_msg_id, idl_msg_type);
      }
      else
      {
        success = qbi_svc_ext_qmux_qmi_ctl_extract_client_info(
          qmi_msg_id, &decoded_buf, client_info);
      }
    }
    qbi_util_buf_free(&decoded_buf);
  }

  return success;
} /* qbi_svc_ext_qmux_qmi_ctl_decode_client_info() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_qmi_ctl_extract_client_info
===========================================================================*/
/*!
    @brief Extracts the ctl_svc_clid_type_v01 (client_info) TLV from a
    decoded QMI_CTL message relating to allocating or releasing a client

    @details

    @param qmi_msg_id
    @param decoded_buf Buffer containing all decoded TLV data
    @param client_info Decoded TLV data will be copied into this buffer

    @return boolean TRUE if extracted and the response indicates success,
    FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_ext_qmux_qmi_ctl_extract_client_info
(
  uint16                 qmi_msg_id,
  const qbi_util_buf_s  *decoded_buf,
  ctl_svc_clid_type_v01 *client_info
)
{
  union {
    ctl_get_client_id_resp_msg_v01     *ctl03;
    ctl_release_client_id_resp_msg_v01 *ctl04;
    ctl_revoke_client_id_ind_msg_v01   *ctl05;
    ctl_invalid_client_id_ind_msg_v01  *ctl06;
  } qmi_ptrs;
  boolean success = FALSE;
  qmi_result_type_v01 result;
  ctl_svc_clid_type_v01 *decoded_client_info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(decoded_buf);
  QBI_CHECK_NULL_PTR_RET_FALSE(decoded_buf->data);
  QBI_CHECK_NULL_PTR_RET_FALSE(client_info);

  switch (qmi_msg_id)
  {
    case QMI_CTL_GET_CLIENT_ID_RESP_V01:
      qmi_ptrs.ctl03 = (ctl_get_client_id_resp_msg_v01 *) decoded_buf->data;
      result = qmi_ptrs.ctl03->resp.result;
      decoded_client_info = &qmi_ptrs.ctl03->svc_client_id;
      break;

    case QMI_CTL_RELEASE_CLIENT_ID_RESP_V01:
      qmi_ptrs.ctl04 = (ctl_release_client_id_resp_msg_v01 *) decoded_buf->data;
      result = qmi_ptrs.ctl04->resp.result;
      decoded_client_info = &qmi_ptrs.ctl04->svc_client_id;
      break;

    case QMI_CTL_REVOKE_CLIENT_ID_IND_V01:
      qmi_ptrs.ctl05 = (ctl_revoke_client_id_ind_msg_v01 *) decoded_buf->data;
      result = QMI_RESULT_SUCCESS_V01;
      decoded_client_info = &qmi_ptrs.ctl05->svc_client_id;
      break;

    case QMI_CTL_INVALID_CLIENT_ID_IND_V01:
      qmi_ptrs.ctl06 = (ctl_invalid_client_id_ind_msg_v01 *) decoded_buf->data;
      result = QMI_RESULT_SUCCESS_V01;
      decoded_client_info = &qmi_ptrs.ctl06->svc_client_id;
      break;

    default:
      QBI_LOG_E_1("Invalid QMI_CTL message ID 0x%04x", qmi_msg_id);
      result = QMI_RESULT_FAILURE_V01;
      decoded_client_info = NULL;
  }

  if (result == QMI_RESULT_SUCCESS_V01)
  {
    QBI_MEMSCPY(client_info, sizeof(ctl_svc_clid_type_v01),
                decoded_client_info, sizeof(ctl_svc_clid_type_v01));
    success = TRUE;
  }

  return success;
} /* qbi_svc_ext_qmux_qmi_ctl_extract_client_info() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_qmi_ctl_svc_flags_to_idl_msg_type
===========================================================================*/
/*!
    @brief Maps a message type from the QMI_CTL header control flags byte to
    an IDL message type enum

    @details

    @param svc_ctl_flags

    @return qmi_idl_type_of_message_type
*/
/*=========================================================================*/
static qmi_idl_type_of_message_type qbi_svc_ext_qmux_qmi_ctl_svc_flags_to_idl_msg_type
(
  uint8 svc_ctl_flags
)
{
  qmi_idl_type_of_message_type msg_type = QMI_IDL_NUM_MSG_TYPES;
/*-------------------------------------------------------------------------*/
  switch (svc_ctl_flags & QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_MASK_MSG_TYPE)
  {
    case QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_REQUEST:
      msg_type = QMI_IDL_REQUEST;

    case QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_RESPONSE:
      msg_type = QMI_IDL_RESPONSE;
      break;

    case QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_INDICATION:
      msg_type = QMI_IDL_INDICATION;
      break;

    default:
      QBI_LOG_E_1("Bad QMI_CTL control flags from modem: 0x%02x",
                  svc_ctl_flags);
  }

  return msg_type;
} /* qbi_svc_ext_qmux_qmi_ctl_svc_flags_to_idl_msg_type() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_rx_cb
===========================================================================*/
/*!
    @brief Processes QMUX data from the modem

    @details
    Runs in the context of the QBI task. This function must eventually free
    the buffer containing the QMUX message received from the modem.

    @param ctx
    @param buf
*/
/*=========================================================================*/
static void qbi_svc_ext_qmux_rx_cb
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
)
{
  qbi_qmux_msg_s *qmux_msg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  QBI_CHECK_NULL_PTR_RET(buf);
  QBI_CHECK_NULL_PTR_RET(buf->data);

  qmux_msg = (qbi_qmux_msg_s *) buf->data;
  if (qmux_msg->qmux_hdr.svc_type == QBI_QMUX_SVC_TYPE_QMI_CTL)
  {
    qbi_svc_ext_qmux_rx_qmi_ctl(ctx, buf);
  }
  else
  {
    qbi_svc_ext_qmux_rx_qmux_sdu(ctx, buf);
  }
  /* Free buf if QMI_CTL and non QMI_CTL does not match any transactions */
  if(buf != NULL)
  {
    qbi_util_buf_free(buf);
    QBI_MEM_FREE(buf);
    buf = NULL;
  }
} /* qbi_svc_ext_qmux_rx_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_rx_qmi_ctl
===========================================================================*/
/*!
    @brief Handles a QMI_CTL message received from the modem

    @details

    @param ctx
    @param buf
*/
/*=========================================================================*/
static void qbi_svc_ext_qmux_rx_qmi_ctl
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
)
{
  qbi_txn_s *txn;
  qbi_util_list_iter_s iter;
  qbi_svc_ext_qmux_info_s *info;
  qbi_qmux_msg_s *qmux_msg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);
  QBI_CHECK_NULL_PTR_RET(buf->data);

  qmux_msg = (qbi_qmux_msg_s *) buf->data;
  QBI_LOG_I_2("Processing QMI_CTL message ID 0x%x (type 0x%x)",
              qmux_msg->sdu.qmi_ctl.msg.msg_id,
              qmux_msg->sdu.qmi_ctl.hdr.svc_ctl_flags);
  if ((qmux_msg->sdu.qmi_ctl.hdr.svc_ctl_flags &
       QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_MASK_MSG_TYPE) ==
        QBI_QMUX_QMI_CTL_SVC_CTL_FLAG_INDICATION)
  {
    /* All QMI_CTL indications are forwarded to the host */
    QBI_LOG_I_0("Forwarding QMI_CTL indication to the host");
    qbi_svc_ext_qmux_qmi_ctl_client_update(
      ctx, (const qbi_util_buf_const_s *) buf);
    qbi_svc_ext_qmux_data_e_send_event(ctx, buf);
  }
  else if (!qbi_txn_get_pending_txn_list_iter(ctx, &iter))
  {
    QBI_LOG_E_0("Couldn't get list iterator for pending CID transactions!");
  }
  else
  {
    /* Find a CID transaction with a matching QMI_CTL transaction ID */
    QBI_LOG_I_1("Received QMI_CTL response with transaction ID %d",
                qmux_msg->sdu.qmi_ctl.hdr.txn_id);
    while ((txn = qbi_util_list_iter_next(&iter)) != NULL)
    {
      if (txn->svc_id == QBI_SVC_ID_EXT_QMUX && txn->info != NULL)
      {
        info = (qbi_svc_ext_qmux_info_s *) txn->info;
        if (info->is_qmi_ctl &&
            info->req.qmi_ctl.qbi_txn_id == qmux_msg->sdu.qmi_ctl.hdr.txn_id)
        {
          /* Replace QBI's internal transaction ID with the one from the host's
             request. */
          QBI_LOG_I_2("Matched to host QMI_CTL txn_id %d; CID txn_id %d",
                      info->req.qmi_ctl.host_txn_id, txn->txn_id);
          qmux_msg->sdu.qmi_ctl.hdr.txn_id = info->req.qmi_ctl.host_txn_id;
          qbi_svc_ext_qmux_qmi_ctl_client_update(
            ctx, (const qbi_util_buf_const_s *) buf);
          qbi_svc_ext_qmux_data_es_send_rsp(txn, buf);
          break;
        }
      }
    }
  }
} /* qbi_svc_ext_qmux_rx_qmi_ctl() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_rx_qmux_sdu
===========================================================================*/
/*!
    @brief Handles QMUX message from the modem for a regular (non-QMI_CTL)
    QMI service

    @details

    @param ctx
    @param buf
*/
/*=========================================================================*/
static void qbi_svc_ext_qmux_rx_qmux_sdu
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
)
{
  qbi_txn_s *txn;
  qbi_util_list_iter_s iter;
  qbi_svc_ext_qmux_info_s *info;
  qbi_qmux_msg_s *qmux_msg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);
  QBI_CHECK_NULL_PTR_RET(buf->data);

  qmux_msg = (qbi_qmux_msg_s *) buf->data;
  if ((qmux_msg->sdu.qmux.hdr.svc_ctl_flags &
       QBI_QMUX_SVC_CTL_FLAG_MASK_MSG_TYPE) ==
        QBI_QMUX_SVC_CTL_FLAG_MSG_TYPE_INDICATION)
  {
    if (qmux_msg->qmux_hdr.client_id == QBI_QMUX_CLIENT_ID_BROADCAST ||
        qbi_svc_ext_qmux_client_is_registered(
          ctx, qmux_msg->qmux_hdr.svc_type, qmux_msg->qmux_hdr.client_id))
    {
      QBI_LOG_I_3("Forwarding indication with msg_id 0x%x from qmi_svc_type %d "
                  "with client_id 0x%x", qmux_msg->sdu.qmux.msg.msg_id,
                  qmux_msg->qmux_hdr.svc_type, qmux_msg->qmux_hdr.client_id);
      qbi_svc_ext_qmux_data_e_send_event(ctx, buf);
    }
  }
  else if (!qbi_txn_get_pending_txn_list_iter(ctx, &iter))
  {
    QBI_LOG_E_0("Couldn't get list iterator for pending CID transactions!");
  }
  else
  {
    while ((txn = qbi_util_list_iter_next(&iter)) != NULL)
    {
      if (txn->svc_id == QBI_SVC_ID_EXT_QMUX && txn->info != NULL)
      {
        info = (qbi_svc_ext_qmux_info_s *) txn->info;
        if (!info->is_qmi_ctl &&
            info->req.qmux.txn_id    == qmux_msg->sdu.qmux.hdr.txn_id &&
            info->req.qmux.svc_type  == qmux_msg->qmux_hdr.svc_type &&
            info->req.qmux.client_id == qmux_msg->qmux_hdr.client_id)
        {
          QBI_LOG_I_5("Found match for svc_type %d client_id 0x%x txn_id 0x%x "
                      "(msg_id 0x%x) - sending response on txn iid %d",
                      info->req.qmux.svc_type, info->req.qmux.client_id,
                      info->req.qmux.txn_id, qmux_msg->sdu.qmux.msg.msg_id,
                      txn->iid);
          qbi_svc_ext_qmux_data_es_send_rsp(txn, buf);
          break;
        }
      }
    }
  }
} /* qbi_svc_ext_qmux_rx_qmux_sdu() */

/*! @addtogroup MBIM_CID_QMUX_DATA
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_data_e_send_event
===========================================================================*/
/*!
    @brief Allocates and sends a MBIM_CID_QMUX_DATA event

    @details

    @param ctx
    @param buf
*/
/*=========================================================================*/
static void qbi_svc_ext_qmux_data_e_send_event
(
  qbi_ctx_s      *ctx,
  qbi_util_buf_s *buf
)
{
  qbi_txn_s *txn;
/*-------------------------------------------------------------------------*/
  txn = qbi_txn_alloc_event(
    ctx, QBI_SVC_ID_EXT_QMUX, QBI_SVC_EXT_QMUX_MBIM_CID_QMUX_DATA);
  if (txn == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate new transaction to send event!");
  }
  else
  {
    qbi_svc_ext_qmux_data_es_send_rsp(txn, buf);
  }
} /* qbi_svc_ext_qmux_data_e_send_event() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_data_es_send_rsp
===========================================================================*/
/*!
    @brief Sends the response payload for an event or set MBIM_CID_QMUX_DATA
    transaction to the host

    @details

    @param txn
    @param buf
*/
/*=========================================================================*/
static void qbi_svc_ext_qmux_data_es_send_rsp
(
  qbi_txn_s      *txn,
  qbi_util_buf_s *buf
)
{
/*-------------------------------------------------------------------------*/
  qbi_txn_set_rsp_buf(txn, buf);
  (void) qbi_svc_proc_action(txn, QBI_SVC_ACTION_SEND_RSP);
} /* qbi_svc_ext_qmux_data_es_send_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_data_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_QMUX_DATA set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_ext_qmux_data_s_req
(
  qbi_txn_s *txn
)
{
  qbi_qmux_msg_s *qmux_msg;
  qbi_svc_ext_qmux_info_s *info;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  qmux_msg = (qbi_qmux_msg_s *) txn->req.data;
  if (qmux_msg->if_type != QBI_QMUX_IF_TYPE_QMUX)
  {
    QBI_LOG_E_1("Unsupported I/F Type %d", qmux_msg->if_type);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else if (qmux_msg->qmux_hdr.svc_type != QBI_QMUX_SVC_TYPE_QMI_CTL &&
           !qbi_svc_ext_qmux_client_is_registered(
             txn->ctx, qmux_msg->qmux_hdr.svc_type,
             qmux_msg->qmux_hdr.client_id))
  {
    QBI_LOG_E_2("Received request with unrecognized client ID %d for "
                "service %d", qmux_msg->qmux_hdr.client_id,
                qmux_msg->qmux_hdr.svc_type);
    /*! @todo Should we add a new error code to cover this case? */
  }
  else
  {
    txn->info = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_ext_qmux_info_s));
    info = (qbi_svc_ext_qmux_info_s *) txn->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(info);

    if (qmux_msg->qmux_hdr.svc_type == QBI_QMUX_SVC_TYPE_QMI_CTL)
    {
      /* Save the host's transaction ID. Replace it with a value that is unique
         to QBI, so the response can be uniquely matched to the CID transaction.
         This will then be transparently replaced by the QMUX API with its own
         unique transaction ID before sending to the modem. */
      info->is_qmi_ctl = TRUE;
      info->req.qmi_ctl.host_txn_id = qmux_msg->sdu.qmi_ctl.hdr.txn_id;
      info->req.qmi_ctl.qbi_txn_id = qbi_svc_ext_qmux_qmi_ctl_txn_id_get(
        txn->ctx);
      qmux_msg->sdu.qmi_ctl.hdr.txn_id = info->req.qmi_ctl.qbi_txn_id;
      QBI_LOG_I_2("Mapped host's QMI_CTL transaction ID %d to QBI's ID %d",
                  info->req.qmi_ctl.host_txn_id, info->req.qmi_ctl.qbi_txn_id);
    }
    else
    {
      /* Save the client ID, service ID, and transaction ID so we can match up
         the response with this CID transaction */
      info->req.qmux.svc_type  = qmux_msg->qmux_hdr.svc_type;
      info->req.qmux.client_id = qmux_msg->qmux_hdr.client_id;
      info->req.qmux.txn_id    = qmux_msg->sdu.qmux.hdr.txn_id;
    }

    if (!qbi_qmux_tx_to_modem(
          txn->ctx, (qbi_qmux_msg_s *) txn->req.data, txn->req.size))
    {
      QBI_LOG_E_0("Couldn't send QMI message to QMUX");
    }
    else
    {
      /* Extend the transaction's timeout since some QMI messages take longer
         than the default timeout, and add a custom timeout handler to use the
         device service specific timeout status code */
      qbi_txn_set_timeout(
        txn, QBI_SVC_EXT_QMUX_TIMEOUT_MS, qbi_svc_ext_qmux_data_s_timeout_cb);

      /* Don't need the QMUX request message anymore - the important bits are
         saved to txn->info */
      qbi_txn_free_req_buf(txn);

      /* Wait for the response to come in qbi_svc_ext_qmux_rx_cb() */
      action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
    }
  }

  return action;
} /* qbi_svc_ext_qmux_data_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_data_s_timeout_cb
===========================================================================*/
/*!
    @brief Timeout callback invoke when we don't receive a response from
    the modem for a QMI request

    @details

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_ext_qmux_data_s_timeout_cb
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  txn->status = QBI_SVC_EXT_QMUX_MBIM_STATUS_TIMEOUT;
  (void) qbi_svc_proc_action(txn, QBI_SVC_ACTION_ABORT);
} /* qbi_svc_ext_qmux_data_s_timeout_cb() */

/*! @} */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_svc_ext_qmux_init
===========================================================================*/
/*!
    @brief One-time initialization of the QMUX over MBIM extensibility
    device service

    @details
*/
/*=========================================================================*/
void qbi_svc_ext_qmux_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_ext_qmux_cfg = {
    {
      0xd1, 0xa3, 0x0b, 0xc2, 0xf9, 0x7a, 0x6e, 0x43,
      0xbf, 0x65, 0xc7, 0xe2, 0x4f, 0xb0, 0xf0, 0xd3
    },
    QBI_SVC_ID_EXT_QMUX,
    TRUE,
    qbi_svc_ext_qmux_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_ext_qmux_cmd_hdlr_tbl),
    qbi_svc_ext_qmux_open,
    qbi_svc_ext_qmux_close
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_ext_qmux_cfg);
} /* qbi_svc_ext_qmux_init() */

