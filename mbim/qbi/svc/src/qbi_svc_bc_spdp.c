/*!
  @file
  qbi_svc_bc_spdp.c

  @brief
  Basic Connectivity device service implementation for single PDP
*/

/*=============================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: $

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
03/07/18  rv   Avoid waiting for NAS attach indication in case of 3G
11/29/17  rv   Avoiding deregister in case of 3G for LTE ATTACH and SPDP
11/24/17  nk   Removed redundant mbn check
10/28/17  rv   Avoid deregister in case of 3G
10/27/17  nk   Added header file for LTE ATTACH and PROVISION CONTEXT
10/09/17  nk   Fixed KW p1 issues and warnings
08/17/17  rv   Added logic for MBN Loading
08/08/17  vk   Initial release
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_svc_bc_ext_mbim.h"
#include "qbi_svc_bc_spdp.h"
#include "qbi_svc_bc_nas.h"
#include "qbi_svc_bc_ext.h"
#include "qbi_svc_bc_ext_prov.h"
#include "qbi_svc_bc_ext_lte.h"
#include "qbi_svc_bc_ext_dssa.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_msg.h"
#include "qbi_msg_mbim.h"
#include "qbi_nv_store.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_txn.h"

#include "persistent_device_configuration_v01.h"
#include "wireless_data_service_v01.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/* IMSI Range max length to be compared */
#define QBI_SVC_BC_SPDP_IMSI_RANGE_LEN_SB   5
/* IMSI Range for SB */
#define QBI_SVC_BC_SPDP_IMSI_RANGE_SB       "44020"

/* IMSI Range max length to be compared */
#define QBI_SVC_BC_SPDP_IMSI_RANGE_LEN_YM   8
/* IMSI Range for YM */
#define QBI_SVC_BC_SPDP_IMSI_RANGE_YM       "44020916"

//SB Modem Config
static uint8_t MBN_DESC[PDC_CONFIG_ID_SIZE_MAX_V01] = 
{0x87, 0xE6, 0x6C, 0x5F, 0x63, 0x5E, 0x12, 0xB1, 0xD6, 
0x5B, 0x30, 0xA9, 0x89, 0x42, 0x77, 0x99, 0xAF, 0x9E, 
0xC2, 0x5C};

#define SPDP_IND_TOKEN                  0x1234

/* YM Access String */
const char YM_ACCESS_STRING[24] = {0x70,0x00,0x6C,0x00,0x75,0x00,0x73,0x00,0x2E,0x00,0x61,0x00,0x63,0x00,0x73,0x00,0x2E,0x00,0x6A,0x00,0x70,0x00,0x00,0x00};
/* YM Username String */
const char YM_USERNAME[4]       = {0x79,0x00,0x6D,0x00};
/* YM Password String */
const char YM_PASSWORD[4]       = {0x79,0x00,0x6D,0x00};
/* SB  Access String */
const char SB_ACCESS_STRING[16] = {0x70,0x00,0x6C,0x00,0x75,0x00,0x73,0x00,0x2E,0x00,0x34,0x00,0x67,0x00,0x00,0x00};
/* SB Username String */
const char SB_USERNAME[8]       = {0x70,0x00,0x6C,0x00,0x75,0x00,0x73,0x00};
/* SB Password String */
const char SB_PASSWORD[4]       = {0x34,0x00,0x67,0x00};

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! @brief Function for preparing the request for the notifier transaction
*/
typedef boolean (qbi_svc_bc_spdp_prepare_req_f)
(
  struct qbi_txn_struct *listener_txn,
  struct qbi_txn_struct *notifier_txn
);

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static qbi_svc_action_e qbi_svc_bc_spdp_get_lte_attach_config
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_bc_spdp_set_lte_attach_config
(
  qbi_txn_s *txn
);

qbi_svc_action_e qbi_svc_bc_spdp_wds_event_report_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static void qbi_svc_bc_spdp_configure_single_pdp_evt_cb
(
  qbi_txn_s       *listener_txn,
  const qbi_txn_s *notifier_txn
);

static void qbi_svc_bc_spdp_get_connect_s_deactivate_evt_cb
(
  qbi_txn_s       *listener_txn,
  const qbi_txn_s *notifier_txn
);

/*=============================================================================

  Private Variables

=============================================================================*/


/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_clear_cache
===========================================================================*/
/*!
    @brief Clear spdp cache

    @details

    @param txn

    @return void
*/
/*=========================================================================*/
static void qbi_svc_bc_spdp_clear_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_spdp_cache_s cache = { 0 };
/*-------------------------------------------------------------------------*/

  QBI_LOG_D_0("Clearing SPDP Cache");
  cache.imsi_flag          = QBI_SVC_BC_SPDP_OPERATOR_NONE;
  cache.spdp_support_flag  = QBI_SVC_BC_SPDP_OPERATOR_NONE;
  QBI_MEMSET(cache.imsi, 0,
    sizeof(char) * (QBI_SVC_BC_SPDP_EF_IMSI_MAX_DIGITS + 1));

  qbi_svc_bc_spdp_update_nv_store(txn->ctx, &cache);
}/* qbi_svc_bc_spdp_clear_cache */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_databuf_add_field
===========================================================================*/
/*!
    @brief Appends a new variable length field to the DataBuffer portion of
    the InformationBuffer

    @details
    The field will be properly aligned per MBIM requirements. In the event
    of allocation failure, field_desc will not be modified.

    @param txn
    @param field_desc Offset/size pair that will be populated with the
    details for the newly added field (usually in the fixed length part
    of the response buffer)
    @param initial_offset Offset to the start of the CID structure: will
    be subtracted from the absolute offset. This is used for nested
    structures and in all other cases should be 0.
    @param field_len Size of the requested field to append to the response.
    Note that the allocated size may be greater than requested to allow for
    padding to a 4 byte boundary.
    @param offset Will be populated with the offset from the beginning
    of the response InformationBuffer to the new field
    @param data If not NULL, a buffer of size field_len to copy into the
    field after successful allocation. If NULL, the newly allocated field
    will be initialized to all zeros

    @return void* Pointer to the buffer to contain data for the new field,
    or NULL on failure
*/
/*=========================================================================*/
void *qbi_svc_bc_spdp_databuf_add_field
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *field_desc,
  uint32                      *req_buf_offset,
  uint32                      initial_offset,
  uint32                      field_len,
  const void                  *data
)
{
  uint32 infobuf_offset = *req_buf_offset;
  uint8 *databuf = NULL;
  uint8 post_pad;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(txn);
  QBI_CHECK_NULL_PTR_RET_NULL(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_NULL(field_desc);

  if (initial_offset > infobuf_offset)
  {
    QBI_LOG_E_2("Invalid initial_offset %d for response with current size %d",
                initial_offset, infobuf_offset);
  }
  else
  {
    /* The field must be padded to a 4 byte boundary at the end, even if it
       is the last field in the message. The start of the field must also
       be padded to a 4 byte boundary, but this is taken care of by careful
       definition of the fixed length portion of the InformationBuffer. */
    if (infobuf_offset % 4)
    {
      QBI_LOG_W_2("Transaction iid %d has invalid InformationBuffer length %d "
        "(must be a multiple of 4)", txn->iid, infobuf_offset);
    }

    post_pad = (field_len % 4);
    if (post_pad > 0)
    {
      post_pad = (4 - post_pad);
    }

    databuf = (uint8*)(txn->req.data) + infobuf_offset;
    /* Update the offset/size pair in the static structure to point to where
        this new field will be in the response. */
    field_desc->offset = (infobuf_offset - initial_offset);
    field_desc->size = field_len;

    if (data)
    {
      QBI_MEMSCPY(databuf, field_len, data, field_len);
    }
    /* Update the transaction's response length counter so we know what the
        offset will be for the next field in the DataBuffer */
    infobuf_offset += (field_len + post_pad);

    QBI_LOG_D_4("Added new field at offset %d with size %d (post pad %d) - "
      "new total rsp len %d", field_desc->offset,
      field_desc->size, post_pad, infobuf_offset);
  }

  *req_buf_offset = infobuf_offset;

  return databuf;
} /* qbi_svc_bc_spdp_databuf_add_field() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_compare_string
===========================================================================*/
/*!
    @brief Compares APN, username, pasword strings of listener and notifier.

    @details

    @param listener_txn Listener transaction
    @param notifier_txn Notifier transaction
    @param listener_txn Listener field descriptor
    @param notifier_txn Notifier field descriptor
    @param listener_txn Listener initial offset
    @param notifier_txn Notifier initial offset

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_spdp_compare_string
(
  qbi_txn_s                   *listener_txn,
  qbi_txn_s                   *notifier_txn,
  qbi_mbim_offset_size_pair_s *listener_fd,
  qbi_mbim_offset_size_pair_s *notifier_fd,
  uint32                      listener_initial_offset,
  uint32                      notifier_initial_offset
)
{
  uint8 *listener_str = NULL;
  uint8 *notifier_str = NULL;
  boolean match_found = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(listener_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(notifier_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(listener_fd);
  QBI_CHECK_NULL_PTR_RET_FALSE(notifier_fd);

  if (listener_fd->size == notifier_fd->size)
  {
    listener_str = (uint8 *) qbi_txn_req_databuf_get_field(listener_txn,
      listener_fd, listener_initial_offset, listener_fd->size);
    QBI_CHECK_NULL_PTR_RET_FALSE(listener_str);

    notifier_str = (uint8 *) qbi_txn_rsp_databuf_get_field(notifier_txn,
      notifier_fd, notifier_initial_offset, notifier_fd->size);
    QBI_CHECK_NULL_PTR_RET_FALSE(notifier_str);

    match_found = !QBI_MEMCMP(listener_str, notifier_str, notifier_fd->size);
  }

  QBI_LOG_D_1("SPDP:: match_found %d", match_found);
  return match_found;
} /* qbi_svc_bc_spdp_compare_string() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_compare_ip_type
===========================================================================*/
/*!
    @brief Compares IP type of listener and notifier.

    @details

    @param listener_txn Listener IP type
    @param notifier_txn Notifier IP type

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_spdp_compare_ip_type
(
  uint32 listener_ip_type,
  uint32 notifier_ip_type
)
{
/*-------------------------------------------------------------------------*/
  if (listener_ip_type == notifier_ip_type ||
    (QBI_SVC_BC_IP_TYPE_DEFAULT == listener_ip_type &&
      QBI_SVC_BC_IP_TYPE_IPV4V6 == notifier_ip_type) ||
      (QBI_SVC_BC_IP_TYPE_IPV4V6 == listener_ip_type &&
        QBI_SVC_BC_IP_TYPE_DEFAULT == notifier_ip_type))
  {
    return TRUE;
  }

  return FALSE;
} /* qbi_svc_bc_spdp_compare_ip_type() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_prepare_lte_attach_config_req
===========================================================================*/
/*!
    @brief Prepares LTE attach config set request

    @details

    @param txn LTE attach config set request transaction

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_spdp_prepare_lte_attach_config_req
(
  qbi_txn_s *notifier_txn,
  qbi_txn_s *listener_txn
)
{
  qbi_svc_bc_ext_lte_attach_config_s_req_s *req = NULL;
  qbi_svc_bc_ext_lte_attach_context_s *ctxt = NULL;
  qbi_mbim_offset_size_pair_s *field_desc = NULL;
  qbi_svc_bc_connect_s_req_s *connect_req = NULL;
  qbi_svc_bc_spdp_cache_s cache = { 0 };
  uint32 req_buf_len = 0;
  uint32 ctxt_len = 0;
  uint32 roam_type = 0;
  uint32 buf_offset = 0;
  uint32 initial_offset = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(notifier_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(listener_txn);

  qbi_svc_bc_spdp_read_nv_store(listener_txn->ctx, &cache);

  buf_offset = sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) +
    sizeof(qbi_mbim_offset_size_pair_s) * 3;

  QBI_LOG_D_2("SPDP:: Preparing LTE attach config req for cid %d spdp_support_flag %d",
               listener_txn->cid, cache.spdp_support_flag);

  if (QBI_SVC_BC_SPDP_OPERATOR_NONE == cache.spdp_support_flag)
  {
     return FALSE;
  }

  if (QBI_SVC_BC_MBIM_CID_CONNECT == listener_txn->cid)
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(listener_txn->req.data);
    connect_req = (qbi_svc_bc_connect_s_req_s *)listener_txn->req.data;
    ctxt_len = sizeof(qbi_svc_bc_ext_lte_attach_context_s) +
      connect_req->access_string.size + (connect_req->access_string.size % 4) +
      connect_req->username.size + (connect_req->username.size % 4) +
      connect_req->password.size + (connect_req->password.size % 4);
  }
  else if (QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS == listener_txn->cid)
  {
    if (QBI_SVC_BC_SPDP_OPERATOR_YM == cache.spdp_support_flag)
    {
      ctxt_len = sizeof(qbi_svc_bc_ext_lte_attach_context_s) +
        sizeof(YM_ACCESS_STRING) + sizeof(YM_USERNAME) +
        sizeof(YM_PASSWORD);
    }
    else
    {
      ctxt_len = sizeof(qbi_svc_bc_ext_lte_attach_context_s) +
        sizeof(SB_ACCESS_STRING) + sizeof(SB_USERNAME) +
        sizeof(SB_PASSWORD);
    }
  }

  req_buf_len = buf_offset + (ctxt_len * 3);
  req = (qbi_svc_bc_ext_lte_attach_config_s_req_s *)
    qbi_util_buf_alloc(&notifier_txn->req, req_buf_len);
  QBI_CHECK_NULL_PTR_RET_FALSE(req);

  req->element_count = 3;
  req->operation = QBI_SVC_MBIM_MS_LTE_ATTACH_CONTEXT_OPERATION_DEFAULT;

  for (roam_type = 0; roam_type < req->element_count; roam_type++)
  {
    field_desc = (qbi_mbim_offset_size_pair_s *)
      ((uint8 *)req + sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) +
        sizeof(qbi_mbim_offset_size_pair_s) * roam_type);
    QBI_CHECK_NULL_PTR_RET_FALSE(field_desc);

    ctxt = (qbi_svc_bc_ext_lte_attach_context_s *)
      qbi_svc_bc_spdp_databuf_add_field(notifier_txn, field_desc,
        &buf_offset, 0, ctxt_len, NULL);
    QBI_CHECK_NULL_PTR_RET_FALSE(ctxt);
    ctxt->roaming = roam_type;

    buf_offset -= (ctxt_len - sizeof(qbi_svc_bc_ext_lte_attach_context_s));
    initial_offset = buf_offset - sizeof(qbi_svc_bc_ext_lte_attach_context_s);

    if (QBI_SVC_BC_MBIM_CID_CONNECT == listener_txn->cid)
    {
      ctxt->ip_type       = connect_req->ip_type;
      ctxt->compression   = connect_req->compression;
      ctxt->auth_protocol = connect_req->auth_protocol;

      (void)qbi_svc_bc_spdp_databuf_add_field(notifier_txn, &ctxt->access_string,
        &buf_offset, initial_offset, connect_req->access_string.size, 
        qbi_txn_req_databuf_get_field(listener_txn, 
          &connect_req->access_string, 0, connect_req->access_string.size));

      (void)qbi_svc_bc_spdp_databuf_add_field(notifier_txn, &ctxt->username,
        &buf_offset, initial_offset, connect_req->username.size, 
        qbi_txn_req_databuf_get_field(listener_txn,
          &connect_req->username, 0, connect_req->username.size));

      (void)qbi_svc_bc_spdp_databuf_add_field(notifier_txn, &ctxt->password,
        &buf_offset, initial_offset, connect_req->password.size, 
        qbi_txn_req_databuf_get_field(listener_txn,
          &connect_req->password, 0, connect_req->password.size));
    }
    else if (QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS == listener_txn->cid)
    {
      ctxt->ip_type       = QBI_SVC_BC_IP_TYPE_IPV4V6;
      ctxt->compression   = QBI_SVC_BC_COMPRESSION_NONE;
      ctxt->auth_protocol = QBI_SVC_BC_AUTH_PROTOCOL_CHAP;

      if (QBI_SVC_BC_SPDP_OPERATOR_YM == cache.spdp_support_flag)
      {
        (void)qbi_svc_bc_spdp_databuf_add_field(notifier_txn, &ctxt->access_string,
          &buf_offset, initial_offset, sizeof(YM_ACCESS_STRING), YM_ACCESS_STRING);

        (void)qbi_svc_bc_spdp_databuf_add_field(notifier_txn, &ctxt->username,
          &buf_offset, initial_offset, sizeof(YM_USERNAME), YM_USERNAME);

        (void)qbi_svc_bc_spdp_databuf_add_field(notifier_txn, &ctxt->password,
          &buf_offset, initial_offset, sizeof(YM_PASSWORD), YM_PASSWORD);

      }
      else if (QBI_SVC_BC_SPDP_OPERATOR_SB == cache.spdp_support_flag)
      {
        (void)qbi_svc_bc_spdp_databuf_add_field(notifier_txn, &ctxt->access_string,
          &buf_offset, initial_offset, sizeof(SB_ACCESS_STRING), SB_ACCESS_STRING);

        (void)qbi_svc_bc_spdp_databuf_add_field(notifier_txn, &ctxt->username,
          &buf_offset, initial_offset, sizeof(SB_USERNAME), SB_USERNAME);

        (void)qbi_svc_bc_spdp_databuf_add_field(notifier_txn, &ctxt->password,
          &buf_offset, initial_offset, sizeof(SB_PASSWORD), SB_PASSWORD);
      }
    }
  }

  return TRUE;
} /* qbi_svc_bc_spdp_prepare_lte_attach_config_req() */

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_create_notifier_txn
===========================================================================*/
/*!
    @brief Creates a notifier transaction for the provided listerner
           transaction.Also registers for callback function that would be
           triggered once notifier transaction completes.

    @details

    @param txn is listerner transaction
    @param service id
    @param cid
    @param prepare the request payload function
    @param prepare the notifier callback which will be triggered after
           transaction is completed
    @param command handler function

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_spdp_create_notifier_txn
(
  qbi_txn_s                       *txn,
  qbi_svc_id_e                    svc_id,
  uint32                          cid,
  qbi_svc_bc_spdp_prepare_req_f   *prepare_req_func,
  qbi_txn_notify_cb_f             *notify_cb,
  qbi_svc_cmd_hdlr_f              *cmd_hdlr_fcn
)
{
  qbi_txn_s *notifier_txn = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* Setting up a listener callback to be invoked when
     LTE attach config complete event is received. */
  QBI_LOG_D_0("SPDP:: Setup listener callback.");
  (void)qbi_txn_notify_setup_listener(txn, notify_cb);

  /* Create a transaction to trigger LTE attach config set request */
  QBI_LOG_D_0("SPDP:: Alloc notifier txn.");
  notifier_txn = qbi_txn_alloc_event(txn->ctx, svc_id, cid);
  QBI_CHECK_NULL_PTR_RET_ABORT(notifier_txn);

  /* Setup the notifier to get notification that sub ready event has
     been posted  */
  QBI_LOG_D_0("SPDP:: Setup notifier.");
  qbi_txn_notify_setup_notifier(txn, notifier_txn);

  /* Default command type is QUERY. Set command type to SET explicitly. */
  notifier_txn->cmd_type = QBI_TXN_CMD_TYPE_INTERNAL;

  if (prepare_req_func)
  {
    /* Prepare LTE attach request buffer. */
    if (prepare_req_func(notifier_txn, txn))
    {
      /* Dispatch the event */
      QBI_LOG_D_0("SPDP:: Dispatch notifier txn.");
      (void)qbi_svc_proc_action(notifier_txn, cmd_hdlr_fcn(notifier_txn));
    }
    else
    {
      /* Dispatch the event */
      QBI_LOG_D_0("SPDP:: Dispatch notifier txn aborting.");
      (void)qbi_svc_proc_action(notifier_txn, QBI_SVC_ACTION_ABORT);
    }
  }
  else
  {
    /* Dispatch the event */
    QBI_LOG_D_0("SPDP:: Dispatch notifier txn.");
    (void)qbi_svc_proc_action(notifier_txn, cmd_hdlr_fcn(notifier_txn));
  }

  return QBI_SVC_ACTION_WAIT_ASYNC_RSP;
}/* qbi_svc_bc_spdp_create_notifier_txn */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_prepare_connect_s_deactivate
===========================================================================*/
/*!
    @brief Prepares deactivate set request

    @details

    @param txn deactivate set request transaction

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_spdp_prepare_connect_s_deactivate
(
  qbi_txn_s *notifier_txn,
  qbi_txn_s *listener_txn
)
{
  qbi_svc_bc_connect_s_req_s *req = NULL;
  qbi_svc_bc_connect_s_req_s *connect_req = NULL;
  uint32 buf_offset = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(notifier_txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(listener_txn);

  buf_offset = sizeof(qbi_svc_bc_connect_s_req_s) +
    sizeof(qbi_mbim_offset_size_pair_s) * 3;

  connect_req = (qbi_svc_bc_connect_s_req_s *)listener_txn->req.data;

  req = (qbi_svc_bc_ext_lte_attach_config_s_req_s *)
    qbi_util_buf_alloc(&notifier_txn->req, buf_offset);
  QBI_CHECK_NULL_PTR_RET_FALSE(req);

  /* Populating only cmd and session as rest payload is not required
     in case of DEACTIVATE */
  req->activation_cmd = QBI_SVC_BC_ACTIVATION_CMD_DEACTIVATE;
  req->session_id = connect_req->session_id;

  QBI_LOG_D_1("SPDP:: Sending deactivation on session id %d",
               connect_req->session_id);

  return TRUE;
} /* qbi_svc_bc_spdp_prepare_connect_s_deactivate() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_configure_deactivate_req
===========================================================================*/
/*!
    @brief Prepare deactivate request

    @details

    @param listener_txn Connect set request transaction
    @param notifier_txn Deactivation set request transaction
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_spdp_configure_deactivate_req
(
  qbi_txn_s *listener_txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(listener_txn);

  /* This call prepares the deactivation request by fill the request
     payload with session id and activation command set to deactivate */
  return qbi_svc_bc_spdp_create_notifier_txn(listener_txn, QBI_SVC_ID_BC_EXT,
           QBI_SVC_BC_MBIM_CID_CONNECT,
           qbi_svc_bc_spdp_prepare_connect_s_deactivate,
           qbi_svc_bc_spdp_get_connect_s_deactivate_evt_cb,
           qbi_svc_bc_connect_s_req);
} /* qbi_svc_bc_spdp_configure_deactivate_req() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_configure_single_pdp_evt_cb
===========================================================================*/
/*!
    @brief Notify callback used to track the result of sub ready status event
    sent at the end of the deactivation stage

    @details

    @param listener_txn Connect set request transaction
    @param notifier_txn Forced subcriber ready status event transaction or
                        lte attach config set request transaction
*/
/*=========================================================================*/
static void qbi_svc_bc_spdp_configure_single_pdp_evt_cb
(
  qbi_txn_s       *listener_txn,
  const qbi_txn_s *notifier_txn
)
{
  qbi_svc_bc_spdp_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(listener_txn);
  QBI_CHECK_NULL_PTR_RET(listener_txn->info);
  QBI_CHECK_NULL_PTR_RET(notifier_txn);

  info = (qbi_svc_bc_spdp_info_s *)listener_txn->info;
  info->spdp_cfg_complete =
    QBI_MBIM_STATUS_SUCCESS == notifier_txn->status ? TRUE : FALSE;
  QBI_LOG_D_1("SPDP: notifier status %d.", notifier_txn->status);

  if (listener_txn->cid == QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS)
  {
     QBI_LOG_D_0("SPDP:: Sending SUBSCRIBER READY STATUS Response");
     (void) qbi_svc_proc_action(listener_txn, QBI_SVC_ACTION_SEND_RSP);
  }
  else
  {
    /* No need to wait for set event report indication
       Procced to connect or deactivate based on the
       whether deactivation_required is set */
    if (info->wait_for_ind == FALSE)
    {
      if (info->deactivation_required &&
          listener_txn->cid == QBI_SVC_BC_MBIM_CID_CONNECT)
      {
        QBI_LOG_D_0("SPDP: Deactivation Required.");
        (void)qbi_svc_proc_action(listener_txn,
           qbi_svc_bc_spdp_configure_deactivate_req(listener_txn));
        info->deactivation_required = FALSE;
      }
      else
      {
        info = (qbi_svc_bc_spdp_info_s *)listener_txn->info;
        info->spdp_cfg_complete = TRUE;

        QBI_LOG_D_0("SPDP: LTE attach config was either skipped or returned error. "
        "Proceed/attempt to Connect.");
        (void)qbi_svc_proc_action(listener_txn,
           qbi_svc_bc_connect_s_req(listener_txn));
      }
    }
    else
    {
      QBI_LOG_D_0("SPDP: LTE attach config completed successfully. Wait for ind.");
    }
  }
} /* qbi_svc_bc_spdp_configure_single_pdp_evt_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_get_lte_attach_config_evt_cb
===========================================================================*/
/*!
    @brief Notify callback used to track the result of sub ready status event
    sent at the end of the deactivation stage

    @details

    @param listener_txn Slot mapping set request transaction
    @param notifier_txn Forced subcriber ready status event transaction
*/
/*=========================================================================*/
static void qbi_svc_bc_spdp_get_lte_attach_config_evt_cb
(
  qbi_txn_s *listener_txn,
  qbi_txn_s *notifier_txn
)
{
  qbi_svc_bc_spdp_info_s *info = NULL;
  qbi_svc_bc_connect_s_req_s *connect_req = NULL;
  qbi_svc_bc_ext_lte_attach_config_info_rsp_s *lte_attach_cfg_rsp = NULL;
  qbi_svc_bc_ext_lte_attach_context_s *lte_attach_ctxt = NULL;
  qbi_mbim_offset_size_pair_s *field_desc = NULL;
  uint32 initial_offset = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(listener_txn);
  QBI_CHECK_NULL_PTR_RET(listener_txn->req.data);
  QBI_CHECK_NULL_PTR_RET(listener_txn->info);
  QBI_CHECK_NULL_PTR_RET(notifier_txn);
  QBI_CHECK_NULL_PTR_RET(notifier_txn->rsp.data);

  QBI_LOG_D_1("SPDP: status %d.", notifier_txn->status);
  info = (qbi_svc_bc_spdp_info_s *)listener_txn->info;
  if (QBI_MBIM_STATUS_SUCCESS == notifier_txn->status)
  {
    connect_req = (qbi_svc_bc_connect_s_req_s *)listener_txn->req.data;
    lte_attach_cfg_rsp =
      (qbi_svc_bc_ext_lte_attach_config_info_rsp_s *)notifier_txn->rsp.data;
    field_desc = (qbi_mbim_offset_size_pair_s*)((uint8*)lte_attach_cfg_rsp +
      sizeof(qbi_svc_bc_ext_lte_attach_config_info_rsp_s));
    
    initial_offset = sizeof(qbi_svc_bc_ext_lte_attach_config_info_rsp_s) +
      sizeof(qbi_mbim_offset_size_pair_s) * lte_attach_cfg_rsp->element_count;

    lte_attach_ctxt = qbi_txn_rsp_databuf_get_field(
      notifier_txn, field_desc, 0, field_desc->size);
    QBI_CHECK_NULL_PTR_RET(lte_attach_ctxt);


    if (qbi_svc_bc_spdp_compare_ip_type(connect_req->ip_type, lte_attach_ctxt->ip_type) &&
      lte_attach_ctxt->compression == connect_req->compression &&
      lte_attach_ctxt->auth_protocol == connect_req->auth_protocol &&
      qbi_svc_bc_spdp_compare_string(listener_txn, notifier_txn,
        &lte_attach_ctxt->access_string, &connect_req->access_string, 0, initial_offset) &&
      qbi_svc_bc_spdp_compare_string(listener_txn, notifier_txn,
        &lte_attach_ctxt->username, &connect_req->username, 0, initial_offset) &&
      qbi_svc_bc_spdp_compare_string(listener_txn, notifier_txn,
        &lte_attach_ctxt->password, &connect_req->password, 0, initial_offset))
    {
      info->spdp_cfg_complete = TRUE;
      qbi_svc_proc_action(listener_txn, qbi_svc_bc_connect_s_req(listener_txn));
    }
    else
    {
      qbi_svc_bc_spdp_set_lte_attach_config(listener_txn);
    }
  }
  else
  {
    info->spdp_cfg_complete = TRUE;
    qbi_svc_proc_action(listener_txn, qbi_svc_bc_connect_s_req(listener_txn));
  }
} /* qbi_svc_bc_spdp_get_lte_attach_config_evt_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_get_lte_attach_status_evt_cb
===========================================================================*/
/*!
    @brief Notify callback used to track the result of sub ready status event
    sent at the end of the deactivation stage

    @details

    @param listener_txn Slot mapping set request transaction
    @param notifier_txn Forced subcriber ready status event transaction
*/
/*=========================================================================*/
static void qbi_svc_bc_spdp_get_lte_attach_status_evt_cb
(
  qbi_txn_s       *listener_txn,
  qbi_txn_s       *notifier_txn
)
{
  qbi_svc_bc_connect_s_req_s *connect_req = NULL;
  qbi_svc_bc_ext_lte_attach_status_rsp_s *lte_attach_status_rsp = NULL;
  qbi_svc_bc_spdp_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(listener_txn);
  QBI_CHECK_NULL_PTR_RET(listener_txn->req.data);
  QBI_CHECK_NULL_PTR_RET(listener_txn->info);
  QBI_CHECK_NULL_PTR_RET(notifier_txn);

  info = (qbi_svc_bc_spdp_info_s *)listener_txn->info;

  if (QBI_MBIM_STATUS_SUCCESS == notifier_txn->status)
  {
    QBI_CHECK_NULL_PTR_RET(notifier_txn->rsp.data);
    connect_req = (qbi_svc_bc_connect_s_req_s *)listener_txn->req.data;
    lte_attach_status_rsp = 
      (qbi_svc_bc_ext_lte_attach_status_rsp_s *)notifier_txn->rsp.data;

    if (QBI_SVC_MBIM_MS_LTE_ATTACH_STATE_ATTACHED ==
      lte_attach_status_rsp->lte_attach_state &&
      qbi_svc_bc_spdp_compare_ip_type(
        connect_req->ip_type, lte_attach_status_rsp->ip_type) &&
      qbi_svc_bc_spdp_compare_string(listener_txn, notifier_txn,
        &connect_req->access_string, &lte_attach_status_rsp->access_string, 0, 0) &&
      qbi_svc_bc_spdp_compare_string(listener_txn, notifier_txn,
        &connect_req->username, &lte_attach_status_rsp->username, 0, 0) && 
      qbi_svc_bc_spdp_compare_string(listener_txn, notifier_txn,
        &connect_req->password, &lte_attach_status_rsp->password, 0, 0) &&
      connect_req->compression == lte_attach_status_rsp->compression &&
      connect_req->auth_protocol == lte_attach_status_rsp->auth_protocol)
    {
      QBI_LOG_D_0("SPDP:: Device attached on LTE and attach parameters match. "
        "Proceed to connect.");
      info->spdp_cfg_complete = TRUE;
      qbi_svc_proc_action(listener_txn, qbi_svc_bc_connect_s_req(listener_txn));
    }
    else
    {
      QBI_LOG_D_0("SPDP:: Device is not attached on LTE, compare configured "
        "LTE attach params with connect param.");
      qbi_svc_bc_spdp_get_lte_attach_config(listener_txn);
    }
  }
  else
  {
    info->spdp_cfg_complete = TRUE;
    qbi_svc_proc_action(listener_txn, qbi_svc_bc_connect_s_req(listener_txn));
  }
} /* qbi_svc_bc_spdp_get_lte_attach_status_evt_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_get_lte_attach_config
===========================================================================*/
/*!
    @brief Notify callback used to track the result of sub ready status event
    sent at the end of the deactivation stage

    @details

    @param listener_txn Slot mapping set request transaction
    @param notifier_txn Forced subcriber ready status event transaction
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_spdp_get_lte_attach_config
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_D_0("SPDP:: Get LTE attach config.");
  return qbi_svc_bc_spdp_create_notifier_txn(txn, QBI_SVC_ID_BC_EXT,
    QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG, NULL,
    (qbi_txn_notify_cb_f *)qbi_svc_bc_spdp_get_lte_attach_config_evt_cb,
    qbi_svc_bc_ext_lte_attach_config_q_req);
} /* qbi_svc_bc_spdp_get_lte_attach_config() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_set_lte_attach_config
===========================================================================*/
/*!
    @brief Notify callback used to track the result of sub ready status event
    sent at the end of the deactivation stage

    @details

    @param listener_txn Slot mapping set request transaction
    @param notifier_txn Forced subcriber ready status event transaction
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_spdp_set_lte_attach_config
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_D_0("SPDP:: Set LTE attach config.");
  return qbi_svc_bc_spdp_create_notifier_txn(txn, QBI_SVC_ID_BC_EXT,
    QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG,
    qbi_svc_bc_spdp_prepare_lte_attach_config_req,
    (qbi_txn_notify_cb_f *)qbi_svc_bc_spdp_configure_single_pdp_evt_cb,
    qbi_svc_bc_ext_lte_attach_config_s_req);
} /* qbi_svc_bc_spdp_set_lte_attach_config() */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_get_lte_attach_status
===========================================================================*/
/*!
    @brief Notify callback used to track the result of sub ready status event
    sent at the end of the deactivation stage

    @details

    @param listener_txn Slot mapping set request transaction
    @param notifier_txn Forced subcriber ready status event transaction
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_spdp_get_lte_attach_status
(
  qbi_txn_s *txn
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_D_0("SPDP:: Get LTE attach status.");
  return qbi_svc_bc_spdp_create_notifier_txn(txn, QBI_SVC_ID_BC_EXT,
    QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_STATUS, NULL, 
    (qbi_txn_notify_cb_f *) qbi_svc_bc_spdp_get_lte_attach_status_evt_cb,
    qbi_svc_bc_ext_lte_attach_status_q_req);
} /* qbi_svc_bc_spdp_get_lte_attach_status() */

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_wds_event_report_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND, looking for 
    changes to the current channel rate or data system status that would 
    trigger an MBIM_CID_PACKET_SERVICE event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_spdp_wds_event_report_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  qbi_svc_bc_spdp_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  QBI_LOG_D_0("SPDP:: Received WDS event report indication");
  if (qbi_svc_bc_nas_is_attached(ind->txn->ctx) && ind->txn->info)
  {
    info = (qbi_svc_bc_spdp_info_s *)ind->txn->info;
    info->spdp_cfg_complete = TRUE;
    QBI_LOG_D_0("SPDP:: Device registered, proceed to connect.");
    action = qbi_svc_bc_connect_s_req(ind->txn);
  }

  return action;
} /* qbi_svc_bc_spdp_wds_event_report_ind_cb() */

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_extract_imsi
===========================================================================*/
/*!
    @brief Extract the first 8 bytes of the IMSI (MCC/MNC pair)

    @details

    @param qmi_txn

    @return none
*/
/*=========================================================================*/
static void qbi_svc_bc_spdp_extract_imsi
(
  qbi_txn_s *txn
)
{
  qbi_svc_bc_spdp_cache_s cache = { 0 };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->info);

  qbi_svc_bc_spdp_read_nv_store(txn->ctx, &cache);
  if (!QBI_STRNCMP(cache.imsi, QBI_SVC_BC_SPDP_IMSI_RANGE_YM,
    sizeof(char) * QBI_SVC_BC_SPDP_IMSI_RANGE_LEN_YM))
  {
    cache.imsi_flag = QBI_SVC_BC_SPDP_OPERATOR_YM;
  }
  else if (!QBI_STRNCMP(cache.imsi, QBI_SVC_BC_SPDP_IMSI_RANGE_SB,
    sizeof(char) * QBI_SVC_BC_SPDP_IMSI_RANGE_LEN_SB))
  {
    cache.imsi_flag = QBI_SVC_BC_SPDP_OPERATOR_SB;
  }
  else
  {
    cache.imsi_flag = QBI_SVC_BC_SPDP_OPERATOR_NONE;
  }

  qbi_svc_bc_spdp_update_nv_store(txn->ctx, &cache);
}/* qbi_svc_bc_spdp_extract_imsi */

/*===========================================================================
  FUNCTION: qbi_svc_bc_connect_s_activate_is_profile_matched
===========================================================================*/
/*!
    @brief Check if auth and pref matches for the currently active profile
           with that of MBIM_CID_CONNECT attributes
    set request

    @details

    @param qmi_txn
    @param qmi_rsp
    @param connect_req

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_bc_spdp_s_is_auth_pref_matched
(
  qbi_qmi_txn_s *qmi_txn,
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp,
  qbi_svc_bc_connect_s_req_s *connect_req
)
{
  wds_auth_pref_mask_v01 req_auth_pref;
  wds_profile_auth_protocol_enum_v01 req_auth_protocol;
/*-------------------------------------------------------------------------*/

   /* Compare authentication type. */
  req_auth_pref = qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_pref(
    connect_req->auth_protocol);
  req_auth_protocol = qbi_svc_bc_connect_mbim_auth_pref_to_qmi_auth_protocol(
    connect_req->auth_protocol);
  if ((qmi_rsp->authentication_preference_valid &&
     (req_auth_pref & qmi_rsp->authentication_preference) == 0 &&
      req_auth_pref != 0) &&
     (qmi_rsp->pdp_type_valid &&
     (qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_PPP_V01 ||
     (qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_IPV4_V01 &&
     !QBI_SVC_BC_CONNECT_IPV4ONLY_REQUESTED(connect_req->ip_type)) ||
     (qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_IPV6_V01 &&
     !QBI_SVC_BC_CONNECT_IPV6ONLY_REQUESTED(connect_req->ip_type)) ||
     (qmi_rsp->pdp_type == WDS_PDP_TYPE_PDP_IPV4V6_V01 &&
     !QBI_SVC_BC_CONNECT_IPV4V6_REQUESTED(connect_req->ip_type)) ||
     (qmi_rsp->pdp_type != WDS_PDP_TYPE_PDP_IPV4V6_V01 &&
      QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(connect_req->ip_type) ==
      QBI_SVC_BC_IP_TYPE_IPV4V6))))
  {
    QBI_LOG_I_0("auth protocol/preference does not match");
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_get_connect_s_deactivate_evt_cb
===========================================================================*/
/*!
    @brief Notify callback used to track the result of deactivation request
    sent at the end of the deactivation stage

    @details

    @param listener_txn Connect set request transaction
    @param notifier_txn Deactivation Request transaction
*/
/*=========================================================================*/
static void qbi_svc_bc_spdp_get_connect_s_deactivate_evt_cb
(
  qbi_txn_s       *listener_txn,
  const qbi_txn_s *notifier_txn
)
{
  qbi_svc_bc_spdp_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(listener_txn);
  QBI_CHECK_NULL_PTR_RET(notifier_txn);
  QBI_CHECK_NULL_PTR_RET(listener_txn->info);

  if (QBI_MBIM_STATUS_SUCCESS == notifier_txn->status)
  {
    /* Deactivation was successful Proceeding to connect */
    QBI_LOG_D_0("SPDP :: Profile Deactivated Succesfully."
                "Proceed to connect");
    info = (qbi_svc_bc_spdp_info_s *)listener_txn->info;
    info->spdp_cfg_complete = TRUE;
    qbi_svc_proc_action(listener_txn,
        qbi_svc_bc_connect_s_req(listener_txn));
  }
  else
  {
    /* Deactivation failed hence return error.
       Will not proceed with connect */
    QBI_LOG_D_0("SPDP :: Profile Deactivation Failed.");
    listener_txn->status = QBI_MBIM_STATUS_MAX_ACTIVATED_CONTEXTS;
    (void) qbi_svc_proc_action(listener_txn, QBI_SVC_ACTION_SEND_RSP);
  }

}/* qbi_svc_bc_spdp_get_connect_s_deactivate_evt_cb */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_s_build_wds2b_rsp_cb
===========================================================================*/
/*!
    @brief Handled QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01 response

    @details

    @param qmi_txn
    @param profile_settings

    @return boolean
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_bc_spdp_s_build_wds2b_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wds_get_profile_settings_resp_msg_v01 *qmi_rsp = NULL;
  wds_get_profile_settings_req_msg_v01 *qmi_req = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_connect_s_req_s *connect_req = NULL;
  qbi_svc_bc_cache_s *cache = NULL;
  qbi_svc_bc_spdp_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->listener_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->listener_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->listener_txn->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wds_get_profile_settings_resp_msg_v01 *) qmi_txn->rsp.data;
  qmi_req = (wds_get_profile_settings_req_msg_v01 *) qmi_txn->req.data;

  info = (qbi_svc_bc_spdp_info_s *)qmi_txn->parent->listener_txn->info;

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    /* If GET PROFILE SETTINGS failed we know a 3G profile is active
       but it doesnt match with one requested in connect hence
       send response*/
    action = QBI_SVC_ACTION_SEND_RSP;
    qmi_txn->parent->status = QBI_MBIM_STATUS_SUCCESS;
    info->wait_for_ind = FALSE;
  }
  else
  {
    cache = qbi_svc_bc_cache_get(qmi_txn->parent->ctx);
    QBI_CHECK_NULL_PTR_RET_ABORT(cache);

    connect_req = (qbi_svc_bc_connect_s_req_s *)
      qmi_txn->parent->listener_txn->req.data;

    if(qbi_svc_bc_spdp_s_is_auth_pref_matched(qmi_txn,qmi_rsp,connect_req) &&
      (qbi_svc_bc_spdp_compare_string(qmi_txn->parent->listener_txn,
                                      qmi_txn->parent,
                                      &connect_req->access_string,
                                      &qmi_rsp->apn_name, 0, 0)) &&
      (qbi_svc_bc_spdp_compare_string(qmi_txn->parent->listener_txn,
                                      qmi_txn->parent,
                                      &connect_req->username,
                                      &qmi_rsp->username, 0, 0)) &&
      (qbi_svc_bc_spdp_compare_string(qmi_txn->parent->listener_txn,
                                      qmi_txn->parent,
                                      &connect_req->password,
                                      &qmi_rsp->password, 0, 0)))
    {
      QBI_LOG_D_0("SPDP :: Profile Matched the once in connect request."
                  "No Need to Activate/Deactivate");
      action = QBI_SVC_ACTION_SEND_RSP;
      qmi_txn->parent->status = QBI_MBIM_STATUS_SUCCESS;
      info->wait_for_ind = FALSE;
    }
    else
    {
      /* Fetch profile settings for the current active profile index for 3GPP2 Profile */
       if (qmi_req->profile.profile_type == WDS_PROFILE_TYPE_3GPP_V01)
       {
          QBI_LOG_D_0("SPDP :: Getting profiles setting for 3GPP2 profile");
          qmi_req = (wds_get_profile_settings_req_msg_v01 *)
          qbi_qmi_txn_alloc_ret_req_buf(
             qmi_txn->parent, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
             qbi_svc_bc_spdp_s_build_wds2b_rsp_cb);
          QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

          qmi_req->profile.profile_type = WDS_PROFILE_TYPE_3GPP2_V01;
          qmi_req->profile.profile_index =
             cache->connect.sessions[connect_req->session_id].active_data_profile_3gpp2_index;

          action = QBI_SVC_ACTION_SEND_QMI_REQ;
       }
       else
       {
          /*Current active profile did not match with one
            requested in connect */
          QBI_LOG_D_0("SPDP :: Current Active Profile did not match requested "
                      "3GPP/3GPP2 profile");
          action = QBI_SVC_ACTION_SEND_RSP;
          qmi_txn->parent->status = QBI_MBIM_STATUS_SUCCESS;
          info->wait_for_ind = FALSE;
          info->deactivation_required = TRUE;
       }
    }
  }

  return action;
}/* qbi_svc_bc_spdp_s_build_wds2b_rsp_cb */

/*===========================================================================
  FUNCTION: qbi_svc_bc_spdp_s_get_connect_session_activation_state
===========================================================================*/
/*!
    @brief Combines the current cached activation states of the IPV4 and
    IPV6 WDS instances into a single overall MBIM activation state value

    @details

    @param ctx
    @param session_id
    @param ip_type

    @return uint32 MBIM activation state
*/
/*=========================================================================*/
static uint32 qbi_svc_bc_spdp_s_get_connect_session_activation_state
(
  const qbi_ctx_s *ctx,
  uint32 session_id,
  uint32 requested_mbim_ip_type
)
{
  qbi_svc_bc_cache_s *cache = NULL;
  uint32 requested_ip_type;
  uint32 activation_state;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_bc_cache_get(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  requested_ip_type = QBI_SVC_BC_MAP_DEFAULT_IP_TYPE(requested_mbim_ip_type);
  switch (requested_ip_type)
  {
    case QBI_SVC_BC_IP_TYPE_IPV4:
      activation_state = cache->connect.sessions[session_id].ipv4.activation_state;
      break;

    case QBI_SVC_BC_IP_TYPE_IPV6:
      activation_state = cache->connect.sessions[session_id].ipv6.activation_state;
      break;

    case QBI_SVC_BC_IP_TYPE_IPV4V6:
      activation_state = qbi_svc_bc_connect_combine_activation_states(
        cache->connect.sessions[session_id].ipv4.activation_state,
        cache->connect.sessions[session_id].ipv6.activation_state, FALSE);
      break;

    case QBI_SVC_BC_IP_TYPE_IPV4_AND_IPV6:
      activation_state = qbi_svc_bc_connect_combine_activation_states(
        cache->connect.sessions[session_id].ipv4.activation_state,
        cache->connect.sessions[session_id].ipv6.activation_state, TRUE);
      break;

    default:
      QBI_LOG_E_1("Unexpected requested IP type %d", requested_ip_type);
      activation_state = QBI_SVC_BC_ACTIVATION_STATE_UNKNOWN;
      break;
  }

  QBI_LOG_D_1("SPDP :: activation state is %d",activation_state);
  return activation_state;
} /* qbi_svc_bc_spdp_s_get_connect_session_activation_state() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_s_register_for_wds01_ind
===========================================================================*/
/*!
    @brief Register for WDS event report indication in case of LTE

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_spdp_s_register_for_wds01_ind
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_spdp_cache_s cache = { 0 };
  qbi_svc_bc_spdp_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->listener_txn);

  /* The RAT is LTE */
  /* In the Single PDP use case when SPDP flag is set,
     command type is internal and RAT is LTE then we need
     to register for WDS Indication.This indication will
     cause connect to proceed */
  if (QBI_SVC_BC_MBIM_CID_CONNECT == txn->listener_txn->cid)
  {
    if (!qbi_svc_ind_reg_dynamic(txn->listener_txn->ctx, QBI_SVC_ID_BC,
       QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG,
       QBI_QMI_SVC_WDS, QMI_WDS_EVENT_REPORT_IND_V01,
       qbi_svc_bc_spdp_wds_event_report_ind_cb, txn->listener_txn, NULL))
    {
      QBI_LOG_E_0("SPDP:: Failed to register WDS event report indication.");
      return QBI_SVC_ACTION_ABORT;
    }

    QBI_LOG_D_0("SPDP:: Registered for WDS Event Report Indication.");
    info = (qbi_svc_bc_spdp_info_s *)txn->listener_txn->info;
    QBI_CHECK_NULL_PTR_RET_ABORT(info);
    info->wait_for_ind = TRUE;
  }

  return QBI_SVC_ACTION_NULL;
}/* qbi_svc_bc_spdp_s_register_for_wds01_ind */

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_s_check_activation_state
===========================================================================*/
/*!
    @brief Check the current activation state ,if active triggers
           GET_PROFILE_SETTING_REQ msg

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_spdp_s_check_activation_state
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_connect_s_req_s *connect_req = NULL;
  wds_get_profile_settings_req_msg_v01 *qmi_req = NULL;
  qbi_svc_bc_cache_s *cache = NULL;
  qbi_svc_bc_spdp_info_s *info = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->listener_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->listener_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->listener_txn->info);

  cache = qbi_svc_bc_cache_get(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  info = (qbi_svc_bc_spdp_info_s *)txn->listener_txn->info;

  connect_req = (qbi_svc_bc_connect_s_req_s *)
    txn->listener_txn->req.data;

   /* Validate if call is active on the requested session id and the profile
      index is a valid index */
   if (qbi_svc_bc_spdp_s_get_connect_session_activation_state(txn->ctx,
         connect_req->session_id,connect_req->ip_type) ==
         QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED)
   {
     /* Fetch profile settings for the current active profile index if any */

     QBI_LOG_D_1("SPDP:: Session %d in Activated State.Proceed to get settings",
                  connect_req->session_id);
     qmi_req = (wds_get_profile_settings_req_msg_v01 *)
     qbi_qmi_txn_alloc_ret_req_buf(
       txn, QBI_QMI_SVC_WDS, QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
       qbi_svc_bc_spdp_s_build_wds2b_rsp_cb);
     QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

     qmi_req->profile.profile_type = WDS_PROFILE_TYPE_3GPP_V01;
     qmi_req->profile.profile_index =
       cache->connect.sessions[connect_req->session_id].active_data_profile_3gpp_index;

     action = QBI_SVC_ACTION_SEND_QMI_REQ;
   }
   else
   {
     QBI_LOG_D_1("SPDP:: Session %d not in Activated State.",
                  connect_req->session_id);
     QBI_LOG_D_0("Waiting for UPDATE PDN List Indication to Trigger Query.");
     info->wait_for_ind = FALSE;
     info->deactivation_required = FALSE;
     action = QBI_SVC_ACTION_WAIT_ASYNC_RSP ;
   }

  return action;
}/* qbi_svc_bc_spdp_s_check_activation_state */

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_read_nv_store
===========================================================================*/
/*!
    @brief Reads single PDP support flag from NV

    @details

    @param ctx
    @param spdp_support_flag

    @return void
*/
/*=========================================================================*/
void qbi_svc_bc_spdp_read_nv_store
(
  qbi_ctx_s               *ctx,
  qbi_svc_bc_spdp_cache_s *cache
)
{
/*-------------------------------------------------------------------------*/
  if (!qbi_nv_store_cfg_item_read(
    ctx, QBI_NV_STORE_CFG_ITEM_SINGLE_PDP_SUPPORT, 
    cache, sizeof(qbi_svc_bc_spdp_cache_s)))
  {
    QBI_LOG_E_0("SPDP:: E:Couldn't read profile data from NV!");
  }
} /* qbi_svc_bc_spdp_read_nv_store() */

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_update_nv_store
===========================================================================*/
/*!
    @brief Updates single PDP support flag in NV

    @details

    @param ctx
    @param spdp_support_flag

    @return void
*/
/*=========================================================================*/
void qbi_svc_bc_spdp_update_nv_store
(
  qbi_ctx_s               *ctx,
  qbi_svc_bc_spdp_cache_s *cache
)
{
/*-------------------------------------------------------------------------*/
  if (!qbi_nv_store_cfg_item_write(
    ctx, QBI_NV_STORE_CFG_ITEM_SINGLE_PDP_SUPPORT, 
    cache, sizeof(qbi_svc_bc_spdp_cache_s)))
  {
    QBI_LOG_E_0("SPDP:: E:Couldn't save profile data to NV!");
  }
} /* qbi_svc_bc_spdp_update_nv_store() */

/*===========================================================================
FUNCTION: qbi_svc_bc_spdp_configure
===========================================================================*/
/*!
    @brief Configures device for single pdp

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
qbi_svc_action_e qbi_svc_bc_spdp_configure
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_bc_spdp_cache_s cache = { 0 };
  qbi_svc_bc_subscriber_ready_status_rsp_s* sub_ready_status = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (txn->cid == QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS)
  {
    /* This API would extract IMSI and store it in uint32 type*/
    qbi_svc_bc_spdp_extract_imsi(txn);
    qbi_svc_bc_spdp_read_nv_store(txn->ctx, &cache);

        sub_ready_status = (qbi_svc_bc_subscriber_ready_status_rsp_s *)txn->rsp.data;
    if (QBI_SVC_BC_READY_STATE_INITIALIZED == sub_ready_status->ready_state)
    {
      /* This API would extract default MBN Loaded */
      if ((cache.imsi_flag != cache.spdp_support_flag) &&
        (QBI_SVC_BC_SPDP_OPERATOR_SB == cache.imsi_flag ||
          QBI_SVC_BC_SPDP_OPERATOR_YM == cache.imsi_flag))
      {
        QBI_LOG_STR_1("SPDP:: SPDP Configuration required for Operator: %s.",
          (QBI_SVC_BC_SPDP_OPERATOR_SB == cache.imsi_flag) ? "SB" : "YM");

        cache.spdp_support_flag = cache.imsi_flag;
        qbi_svc_bc_spdp_update_nv_store(txn->ctx, &cache);

        action = qbi_svc_bc_spdp_set_lte_attach_config(txn);
      }
      else
      {
        if (cache.imsi_flag == cache.spdp_support_flag)
        {
          if (QBI_SVC_BC_SPDP_OPERATOR_SB == cache.imsi_flag ||
            QBI_SVC_BC_SPDP_OPERATOR_YM == cache.imsi_flag)
          {
            QBI_LOG_STR_1("SPDP:: SPDP Configuration for Operator %d already completed.",
              cache.imsi_flag);
          }
        }
        else
        {
          QBI_LOG_D_0("SPDP:: SPDP configuration not required for the operator");
          qbi_svc_bc_spdp_clear_cache(txn);
        }
        action = QBI_SVC_ACTION_SEND_RSP;
      }
    }
    else
    {
      QBI_LOG_D_0("SPDP:: SIM is not ready, skip SPDP config.");
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }
  else
  {
    txn->info = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_bc_spdp_info_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

    QBI_LOG_D_0("SPDP:: Connect request SPDP precheck.");
    action = qbi_svc_bc_spdp_get_lte_attach_status(txn);
  }

  return action;
}/* qbi_svc_bc_spdp_configure */
