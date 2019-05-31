/*!
  @file
  qbi_svc_sms.c

  @brief
  SMS device service implementation

  @note
  Since MBIM requires that the device present a single logical storage when
  multiple physical storage mediums are available (i.e. NV and UIM), failover
  between storage types must be done when one becomes full. This is done
  on the modem by WMS only when FEATURE_SMS_MT_STORE_TO_SIM_IF_NV_FULL is
  defined. Note that on MDM9x15 and later targets, this feature flag has been
  replaced by NV 69707, which must be set to 1. Enabling that feature is
  currently a dependency for proper QBI functionality when the memory store(s)
  become full.

  UIM storage only configuration is also available if NV storage usage is
  restricted. In this configuration QBI only reports status for UIM storage
  and send SMS on UIM storage without failover, however MT SMS arrived on NV
  storage will still be processed.
*/

/*=============================================================================

  Copyright (c) 2011-2014, 2017 Qualcomm Technologies, Inc.
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
09/05/18  ar   Modified MO SMS routing to not store MO SMS during send
02/07/18  ar   Fixed issued related to sms routing table
09/07/17  rv   Added explicit QMI call to set route during init
08/09/17  rv   Fixed issue related to sms storage notification
05/12/17  rv   Adding SMSC Addr as part of PDU payload for Class 0 SMS
05/13/14  hz   Monitor WMS service ready indication
04/02/14  hz   Add UIM storage only support
06/19/12  bd   Ignore CDMA delete all errors in multimode build
01/13/12  bd   Add SMS transcoding support
10/28/11  bd   Updated to MBIM v1.0 SC
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_svc_sms.h"
#include "qbi_svc_sms_mbim.h"
#include "qbi_svc_sms_pdu.h"

#include "qbi_common.h"
#include "qbi_mbim.h"
#include "qbi_nv_store.h"
#include "qbi_qmi_txn.h"
#include "qbi_svc.h"
#include "qbi_svc_bc.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_txn.h"
#include "qbi_util.h"

#include "wireless_messaging_service_v01.h"

/*=============================================================================

  Private Macros

=============================================================================*/

/*! This macro statically defines a QMI indication handler and fills in the
    fields that are common to all handlers in this device service */
#define QBI_SVC_SMS_STATIC_IND_HDLR(qmi_svc_id, qmi_msg_id, cid, cb) \
  {qmi_svc_id, qmi_msg_id, cid, cb, NULL}

/*=============================================================================

  Private Typedefs and Constants

=============================================================================*/

/*! Do not change the cached value */
#define QBI_SVC_SMS_CACHE_NO_CHANGE_U32 (0xFFFFFFFF)

/*! @brief Device service local cache struct
*/
typedef struct {
  struct {
    wms_storage_type_enum_v01 storage_pref;
    uint32 format;

    struct {
      uint32 store_size_nv;
      uint32 store_size_uim;
    } qmi;
  } configuration;

  struct {
    struct {
      boolean registered;

      /*! IMS transport capability. Only valid if registered is TRUE. */
      wms_transport_capability_enum_v01 capability;
    } ims;
  } send;

  struct {
    uint32 flag;
    uint32 message_index;
  } status;
} qbi_svc_sms_cache_s;

/*! Single SMS record element */
typedef struct {
  uint32                    index;
  wms_storage_type_enum_v01 storage_type;
} qbi_svc_sms_msg_location_s;

/*! SMS record list entry for multiple SMS read (entry appearing in
    qbi_svc_sms_rec_list_s.list) */
typedef struct {
  /*! Must be first as we alias */
  qbi_util_list_entry_s list_entry;

  qbi_svc_sms_msg_location_s loc;
} qbi_svc_sms_rec_list_entry_s;

/*! Information field attached to a MBIM_CID_SMS_READ transaction to support
    reading multiple SMS records */
typedef struct {
  /*! Flag used to filter the list */
  uint32 flag;

  /*! Callback function invoked to process the record list once it is built */
  qbi_svc_cmd_hdlr_f *proc_record_list;

  /*! Total number of SMS records to be processed in this transaction. */
  uint32 num_records;

  /*! Number of records popped off the list via qbi_svc_sms_rec_list_get_next */
  uint32 records_processed;

  /*! List containing qbi_svc_sms_rec_list_entry_s values */
  qbi_util_list_s list;

  /*! List iterator used for traversing the list */
  qbi_util_list_iter_s iter;
} qbi_svc_sms_rec_list_s;


/*=============================================================================

  Private Function Prototypes

=============================================================================*/


static wms_message_tag_type_enum_v01 qbi_svc_sms_flag_to_qmi_tag_type
(
  uint32 flag
);

static qbi_svc_sms_cache_s *qbi_svc_sms_get_cache
(
  const qbi_ctx_s *ctx
);

static void qbi_svc_sms_get_failure_status
(
  qbi_txn_s                *txn,
  qmi_error_type_v01        error
);

static qbi_svc_action_e qbi_svc_sms_open
(
  qbi_txn_s *txn
);

static boolean qbi_svc_sms_init_cache
(
  qbi_ctx_s *ctx
);

static void qbi_svc_sms_modify_tag
(
  qbi_ctx_s                    *ctx,
  wms_storage_type_enum_v01     storage_type,
  uint32                        index,
  wms_message_tag_type_enum_v01 tag_type
);

static qbi_svc_action_e qbi_svc_sms_modify_tag_wms23_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_sms_rec_list_add_entries
(
  qbi_svc_sms_rec_list_s           *rec_list,
  wms_storage_type_enum_v01         storage_type,
  uint32                            num_records,
  const wms_message_tuple_type_v01 *records
);

static qbi_svc_action_e qbi_svc_sms_rec_list_build_qmi_req
(
  qbi_txn_s                *txn,
  uint32                    flag,
  wms_storage_type_enum_v01 storage_type
);

static void qbi_svc_sms_rec_list_completion_cb
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_rec_list_fetch_by_flag
(
  qbi_txn_s          *txn,
  uint32              flag,
  qbi_svc_cmd_hdlr_f *proc_record_list
);

static qbi_svc_action_e qbi_svc_sms_rec_list_fetch_index
(
  qbi_txn_s          *txn,
  uint32              index,
  qbi_svc_cmd_hdlr_f *proc_record_list
);

static void qbi_svc_sms_rec_list_item_skipped
(
  qbi_txn_s *txn
);

static uint32 qbi_svc_sms_rec_list_get_cur_mbim_index
(
  const qbi_txn_s *txn
);

static uint32 qbi_svc_sms_rec_list_get_cur_offset
(
  const qbi_txn_s *txn
);

static boolean qbi_svc_sms_rec_list_init
(
  qbi_txn_s          *txn,
  uint32              flag,
  qbi_svc_cmd_hdlr_f *proc_record_list
);

static boolean qbi_svc_sms_rec_list_get_next
(
  const qbi_txn_s           *txn,
  wms_storage_type_enum_v01 *storage_type,
  uint32                    *index
);

static uint32 qbi_svc_sms_rec_list_num_records
(
  const qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_rec_list_wms31_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_reg_inds_rsp
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_reg_inds_with_qmi
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_reg_inds_wms01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_reg_inds_wms47_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @addtogroup MBIM_CID_SMS_CONFIGURATION
    @{ */

static qbi_svc_action_e qbi_svc_sms_configuration_e_wms46_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_sms_configuration_e_wms5d_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static wms_storage_type_enum_v01 qbi_svc_sms_configuration_get_preferred_storage_type
(
  qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_sms_configuration_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_configuration_q_req_max_msgs
(
  qbi_txn_s                *txn,
  wms_storage_type_enum_v01 storage_type
);

static qbi_svc_action_e qbi_svc_sms_configuration_q_rsp
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_configuration_q_wms30_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_configuration_q_wms34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_configuration_q_wms36_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_configuration_s_change_sc_addr
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_configuration_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_configuration_s_wms35_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_sms_configuration_update_cache_format
(
  qbi_ctx_s *ctx,
  uint32     format
);

/*! @} */

/*! @addtogroup MBIM_CID_SMS_READ
    @{ */

static qbi_svc_action_e qbi_svc_sms_read_e_build_rsp
(
  qbi_txn_s                                    *txn,
  const wms_transfer_route_mt_message_type_v01 *msg_data
);

static void qbi_svc_sms_read_e_send_ack
(
  qbi_ctx_s *ctx,
  uint32     sms_transaction_id
);

static qbi_svc_action_e qbi_svc_sms_read_e_wms01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_sms_read_e_wms37_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static boolean qbi_svc_sms_read_eq_add_record_pdu
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *field_desc,
  uint32                       message_index,
  uint32                       message_status,
  wms_message_format_enum_v01  format,
  uint32                       data_len,
  const uint8_t               *data
);

static boolean qbi_svc_sms_read_q_copy_pdu_data
(
  qbi_txn_s                                *txn,
  const wms_read_raw_message_data_type_v01 *msg_data
);

static boolean qbi_svc_sms_read_q_downsize_rsp
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_read_q_get_next_record
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_read_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_read_q_wms22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static uint32 qbi_svc_sms_read_qmi_tag_type_to_status
(
  wms_message_tag_type_enum_v01 tag_type
);

/*! @} */

/*! @addtogroup MBIM_CID_SMS_SEND
    @{ */

static qbi_svc_action_e qbi_svc_sms_send_s_build_wms20_req
(
  qbi_txn_s *txn
);

static boolean qbi_svc_sms_send_s_get_req_pdu
(
  qbi_txn_s               *txn,
  qbi_svc_sms_send_pdu_s **req_pdu,
  uint8                  **pdu_data
);

static boolean qbi_svc_sms_send_s_is_gw_to_cdma_conversion_required
(
  qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_sms_send_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_send_s_wms20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_send_s_wms21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_send_s_write
(
  qbi_txn_s                *txn,
  wms_storage_type_enum_v01 storage_type
);

static void qbi_svc_sms_send_update_cache_ims
(
  qbi_ctx_s                        *ctx,
  uint8_t                           registered_ind,
  uint8_t                           transport_layer_info_valid,
  wms_transport_layer_info_type_v01 transport_layer_info
);

static void qbi_svc_sms_send_wms48_req
(
  qbi_ctx_s *ctx
);

static qbi_svc_action_e qbi_svc_sms_send_wms48_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_send_wms49_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

/*! @} */

/*! @addtogroup MBIM_CID_SMS_DELETE
    @{ */

static qbi_svc_action_e qbi_svc_sms_delete_s_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_delete_s_wms24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

/*! @} */

/*! @addtogroup MBIM_CID_SMS_STATUS
    @{ */

static qbi_svc_action_e qbi_svc_sms_status_e_c2g_convert_cdma_msg
(
  qbi_txn_s                     *txn,
  const wms_mt_message_type_v01 *mt_msg
);

static qbi_svc_action_e qbi_svc_sms_status_e_c2g_delete_cdma_msg
(
  qbi_txn_s                *txn,
  wms_storage_type_enum_v01 gw_msg_storage_type,
  uint32                    gw_msg_storage_index
);

static qbi_svc_action_e qbi_svc_sms_status_e_c2g_failover_write_to_sim
(
  qbi_txn_s                       *txn,
  const wms_raw_write_req_msg_v01 *qmi_req_nv
);

static qbi_svc_action_e qbi_svc_sms_status_e_c2g_wms21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_status_e_c2g_wms22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_status_e_c2g_wms24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_status_e_wms01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_sms_status_e_wms3f_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
);

static qbi_svc_action_e qbi_svc_sms_status_eq_build_rsp_from_cache
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_status_eq_build_wms36_req
(
  qbi_txn_s                *txn,
  wms_storage_type_enum_v01 storage_type,
  qbi_svc_qmi_rsp_cb_f     *rsp_cb
);

static qbi_svc_action_e qbi_svc_sms_status_q_build_wms31_req
(
  qbi_txn_s                *txn,
  wms_storage_type_enum_v01 storage_type
);

static qbi_svc_action_e qbi_svc_sms_status_q_req
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_status_q_rsp
(
  qbi_txn_s *txn
);

static qbi_svc_action_e qbi_svc_sms_status_q_wms31_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static qbi_svc_action_e qbi_svc_sms_status_q_wms36_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
);

static void qbi_svc_sms_status_update_flags_if_set
(
  qbi_ctx_s *ctx,
  uint32     flag
);

/*! @} */


/*=============================================================================

  Private Variables

=============================================================================*/


/*! @brief CID handler dispatch table
    @details Order must match qbi_svc_sms_cid_e. Entries are
    {query_func, min_query_infobuf_len, set_func, min_set_infobuf_len}
*/
static const qbi_svc_cmd_hdlr_tbl_entry_s qbi_svc_sms_cmd_hdlr_tbl[] = {
  /* MBIM_CID_SMS_CONFIGURATION */
  {qbi_svc_sms_configuration_q_req,  0,
    qbi_svc_sms_configuration_s_req, sizeof(qbi_svc_sms_configuration_s_req_s)},
  /* MBIM_CID_SMS_READ */
  {qbi_svc_sms_read_q_req,           sizeof(qbi_svc_sms_read_q_req_s),
    NULL,                            0},
  /* MBIM_CID_SMS_SEND */
  {NULL,                             0,
    qbi_svc_sms_send_s_req,          sizeof(qbi_svc_sms_send_s_req_s)},
  /* MBIM_CID_SMS_DELETE */
  {NULL,                             0,
    qbi_svc_sms_delete_s_req,        sizeof(qbi_svc_sms_delete_s_req_s)},
  /* MBIM_CID_SMS_STATUS */
  {qbi_svc_sms_status_q_req,         0,
    NULL,                            0}
};

/*! @brief Static QMI indication handlers (generally, CID event handlers)
*/
static const qbi_svc_ind_info_s qbi_svc_sms_static_ind_hdlrs[] = {
  QBI_SVC_SMS_STATIC_IND_HDLR(QBI_QMI_SVC_WMS, QMI_WMS_SMSC_ADDRESS_IND_V01,
                              QBI_SVC_SMS_MBIM_CID_SMS_CONFIGURATION,
                              qbi_svc_sms_configuration_e_wms46_ind_cb),

  QBI_SVC_SMS_STATIC_IND_HDLR(QBI_QMI_SVC_WMS, QMI_WMS_SERVICE_READY_IND_V01,
                              QBI_SVC_SMS_MBIM_CID_SMS_CONFIGURATION,
                              qbi_svc_sms_configuration_e_wms5d_ind_cb),

  QBI_SVC_SMS_STATIC_IND_HDLR(QBI_QMI_SVC_WMS, QMI_WMS_EVENT_REPORT_IND_V01,
                              QBI_SVC_SMS_MBIM_CID_SMS_READ,
                              qbi_svc_sms_read_e_wms01_ind_cb),

  QBI_SVC_SMS_STATIC_IND_HDLR(QBI_QMI_SVC_WMS,
                              QMI_WMS_TRANSPORT_LAYER_INFO_IND_V01,
                              QBI_SVC_SMS_MBIM_CID_SMS_SEND,
                              qbi_svc_sms_send_wms49_ind_cb),

  QBI_SVC_SMS_STATIC_IND_HDLR(QBI_QMI_SVC_WMS, QMI_WMS_EVENT_REPORT_IND_V01,
                              QBI_SVC_SMS_MBIM_CID_SMS_STATUS,
                              qbi_svc_sms_status_e_wms01_ind_cb),
  QBI_SVC_SMS_STATIC_IND_HDLR(QBI_QMI_SVC_WMS, QMI_WMS_MEMORY_FULL_IND_V01,
                              QBI_SVC_SMS_MBIM_CID_SMS_STATUS,
                              qbi_svc_sms_status_e_wms3f_ind_cb)
};


/*=============================================================================

  Private Function Definitions

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_svc_sms_flag_to_qmi_tag_type
===========================================================================*/
/*!
    @brief Maps a MBIM SMS_FLAG to QMI WMS tag type

    @details

    @param flag Valid MBIM SMS_FLAG. Must not be ALL or INDEX.

    @return wms_message_tag_type_enum_v01
*/
/*=========================================================================*/
static wms_message_tag_type_enum_v01 qbi_svc_sms_flag_to_qmi_tag_type
(
  uint32 flag
)
{
  wms_message_tag_type_enum_v01 tag_type;
/*-------------------------------------------------------------------------*/
  switch (flag)
  {
    case QBI_SVC_SMS_FLAG_NEW:
      tag_type = WMS_TAG_TYPE_MT_NOT_READ_V01;
      break;

    case QBI_SVC_SMS_FLAG_OLD:
      tag_type = WMS_TAG_TYPE_MT_READ_V01;
      break;

    case QBI_SVC_SMS_FLAG_SENT:
      tag_type = WMS_TAG_TYPE_MO_SENT_V01;
      break;

    case QBI_SVC_SMS_FLAG_DRAFT:
      tag_type = WMS_TAG_TYPE_MO_NOT_SENT_V01;
      break;

    default:
      QBI_LOG_E_1("Unexpected flag %d", flag);
      tag_type = WMS_TAG_TYPE_MT_NOT_READ_V01;
  }

  return tag_type;
} /* qbi_svc_sms_flag_to_qmi_tag_type() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_get_cache
===========================================================================*/
/*!
    @brief Returns a pointer to the SMS device service's cache

    @details

    @param ctx

    @return qbi_svc_sms_cache_s* Pointer to cache, or NULL on error
*/
/*=========================================================================*/
static qbi_svc_sms_cache_s *qbi_svc_sms_get_cache
(
  const qbi_ctx_s *ctx
)
{
/*-------------------------------------------------------------------------*/
  return (qbi_svc_sms_cache_s *) qbi_svc_cache_get(ctx, QBI_SVC_ID_SMS);
} /* qbi_svc_sms_get_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_get_failure_status
===========================================================================*/
/*!
    @brief Sets the appropriate status code for a failed operation using the
    given message mode

    @details
    Since QMI WMS tends to give QMI_ERR_DEVICE_NOT_READY in many cases where
    the SIM is not initialized, this checks the ReadyState to get a more
    descriptive error code if available.

    Note that this does not check that the radio is powered on. That check
    should be performed before calling this function if the failed action
    requires radio access.

    @param txn Transaction which will have its status field updated
    @param error QMI error given in the response
*/
/*=========================================================================*/
static void qbi_svc_sms_get_failure_status
(
  qbi_txn_s                *txn,
  qmi_error_type_v01        error
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  if (qbi_svc_bc_check_device_state(txn, TRUE, FALSE))
  {
    if (error == QMI_ERR_DEVICE_NOT_READY_V01)
    {
      txn->status = QBI_MBIM_STATUS_NOT_INITIALIZED;
    }
    else if (error == QMI_ERR_SMSC_ADDR_V01)
    {
      txn->status = QBI_MBIM_STATUS_SMS_UNKNOWN_SMSC_ADDRESS;
    }
    else if (error == QMI_ERR_INVALID_INDEX_V01)
    {
      txn->status = QBI_MBIM_STATUS_INVALID_MEMORY_INDEX;
    }
    else if (error == QMI_ERR_DEVICE_STORAGE_FULL_V01)
    {
      txn->status = QBI_MBIM_STATUS_MEMORY_FULL;
    }
    else
    {
      txn->status = QBI_MBIM_STATUS_FAILURE;
    }
  }
} /* qbi_svc_sms_get_failure_status() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_mbim_index_to_qmi_location
===========================================================================*/
/*!
    @brief Maps a MBIM record index to a QMI storage type and index pair

    @details

    @param ctx
    @param mbim_index
    @param qmi_storage

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_mbim_index_to_qmi_location
(
  qbi_ctx_s                 *ctx,
  uint32                     mbim_index,
  uint32_t                  *qmi_index,
  wms_storage_type_enum_v01 *qmi_storage_type
)
{
  qbi_svc_sms_cache_s *cache;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_index);
  QBI_CHECK_NULL_PTR_RET_FALSE(qmi_storage_type);

  cache = qbi_svc_sms_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (mbim_index == 0)
  {
    QBI_LOG_E_0("Index 0 does not map to a valid location!");
    success = FALSE;
  }
  else if (mbim_index <= cache->configuration.qmi.store_size_nv)
  {
    *qmi_index = mbim_index - 1;
    *qmi_storage_type = WMS_STORAGE_TYPE_NV_V01;
  }
  else
  {
    mbim_index -= cache->configuration.qmi.store_size_nv;
    if (mbim_index <= cache->configuration.qmi.store_size_uim)
    {
      *qmi_index = mbim_index - 1;
      *qmi_storage_type = WMS_STORAGE_TYPE_UIM_V01;
    }
    else
    {
      QBI_LOG_E_3("Couldn't map MBIM index %d to QMI storage/index pair! "
                  "NV store size %d UIM store size %d",
                  (mbim_index + cache->configuration.qmi.store_size_nv),
                  cache->configuration.qmi.store_size_nv,
                  cache->configuration.qmi.store_size_uim);
      success = FALSE;
    }
  }

  return success;
} /* qbi_svc_sms_mbim_index_to_qmi_location() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_set_route_wms32_s_rsp_cb
===========================================================================*/
/*!
    @brief Response handling for SMS SET ROUTE QMI Call

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_set_route_wms32_s_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_set_routes_resp_msg_v01 *qmi_rsp = NULL;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wms_set_routes_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }

  /* Even if SET ROUTE Fails try registering for indications */
  action = qbi_svc_sms_reg_inds_with_qmi(qmi_txn->parent);

  return action;
}/* qbi_svc_sms_set_route_wms32_s_rsp_cb */

/*===========================================================================
  FUNCTION: qbi_svc_sms_set_route_wms32_s_req
===========================================================================*/
/*!
    @brief Initializes the SMS SET ROUTE to use specific configuration for
           storage for each class of SMS

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_set_route_wms32_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wms_set_routes_req_msg_v01 *qmi_req = NULL;
  qbi_svc_sms_cache_s *cache =  NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_sms_get_cache(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  qmi_req = (wms_set_routes_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_SET_ROUTES_REQ_V01,
                                  qbi_svc_sms_set_route_wms32_s_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qbi_nv_store_cfg_item_read(
        txn->ctx, QBI_NV_STORE_CFG_ITEM_SMS_STORAGE_PREF,
        &cache->configuration.storage_pref,
        sizeof(cache->configuration.storage_pref));

  QBI_LOG_D_0("Triggering Set Route QMI Request.");
  qmi_req->route_list_tuple_len = 6;
  qmi_req->route_list_tuple[0].message_class  = WMS_MESSAGE_CLASS_0_V01;
  qmi_req->route_list_tuple[0].message_type   = WMS_MESSAGE_TYPE_POINT_TO_POINT_V01;
  qmi_req->route_list_tuple[0].route_storage  = WMS_STORAGE_TYPE_NONE_V01;
  qmi_req->route_list_tuple[0].receipt_action = WMS_TRANSFER_ONLY_V01;

  qmi_req->route_list_tuple[1].message_class  = WMS_MESSAGE_CLASS_1_V01;
  qmi_req->route_list_tuple[1].message_type   = WMS_MESSAGE_TYPE_POINT_TO_POINT_V01;
  qmi_req->route_list_tuple[1].route_storage  = WMS_STORAGE_TYPE_NONE_V01;
  qmi_req->route_list_tuple[1].receipt_action = WMS_TRANSFER_ONLY_V01;

  qmi_req->route_list_tuple[2].message_class  = WMS_MESSAGE_CLASS_2_V01;
  qmi_req->route_list_tuple[2].message_type   = WMS_MESSAGE_TYPE_POINT_TO_POINT_V01;
  qmi_req->route_list_tuple[2].route_storage  = 
    (cache->configuration.storage_pref == WMS_STORAGE_TYPE_UIM_V01) 
    ? WMS_STORAGE_TYPE_UIM_V01 : WMS_STORAGE_TYPE_NV_V01;
  qmi_req->route_list_tuple[2].receipt_action = WMS_STORE_AND_NOTIFY_V01;

  qmi_req->route_list_tuple[3].message_class  = WMS_MESSAGE_CLASS_3_V01;
  qmi_req->route_list_tuple[3].message_type   = WMS_MESSAGE_TYPE_POINT_TO_POINT_V01;
  qmi_req->route_list_tuple[3].route_storage  = WMS_STORAGE_TYPE_NONE_V01;
  qmi_req->route_list_tuple[3].receipt_action = WMS_TRANSFER_ONLY_V01;

  qmi_req->route_list_tuple[4].message_class  = WMS_MESSAGE_CLASS_NONE_V01;
  qmi_req->route_list_tuple[4].message_type   = WMS_MESSAGE_TYPE_POINT_TO_POINT_V01;
  qmi_req->route_list_tuple[4].route_storage  = WMS_STORAGE_TYPE_NONE_V01;
  qmi_req->route_list_tuple[4].receipt_action = WMS_TRANSFER_ONLY_V01;

  qmi_req->route_list_tuple[5].message_class  = WMS_MESSAGE_CLASS_CDMA_V01;
  qmi_req->route_list_tuple[5].message_type   = WMS_MESSAGE_TYPE_POINT_TO_POINT_V01;
  qmi_req->route_list_tuple[5].route_storage  = WMS_STORAGE_TYPE_NONE_V01;
  qmi_req->route_list_tuple[5].receipt_action = WMS_TRANSFER_ONLY_V01;

  action = QBI_SVC_ACTION_SEND_QMI_REQ;

  return action;
}/* qbi_svc_sms_set_route_wms32_s_req */

/*===========================================================================
  FUNCTION: qbi_svc_sms_route_init_and_reg_inds_with_qmi
===========================================================================*/
/*!
    @brief Initializes the SMS SET ROUTE to use specific configuration for
           storage for each class of SMS as well as register for QMI
           Indications

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_route_init_and_reg_inds_with_qmi
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  action = qbi_svc_sms_set_route_wms32_s_req(txn);

  return action;
}/* qbi_svc_sms_route_init_and_reg_inds_with_qmi */

/*===========================================================================
  FUNCTION: qbi_svc_sms_init_cache
===========================================================================*/
/*!
    @brief Initializes the SMS device service cache to default values

    @details

    @param ctx

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_svc_sms_init_cache
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
  qbi_svc_sms_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_sms_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  if (!qbi_nv_store_cfg_item_read(
        ctx, QBI_NV_STORE_CFG_ITEM_SMS_STORAGE_PREF,
        &cache->configuration.storage_pref,
        sizeof(cache->configuration.storage_pref)))
  {
    cache->configuration.storage_pref = WMS_STORAGE_TYPE_UIM_V01;
    if (qbi_nv_store_cfg_item_write(
        ctx, QBI_NV_STORE_CFG_ITEM_SMS_STORAGE_PREF,
        &cache->configuration.storage_pref,
        sizeof(cache->configuration.storage_pref)))
    {
      QBI_LOG_D_1("Set sms storage preference to %d",
                   cache->configuration.storage_pref);
    }
    else
    {
      QBI_LOG_D_0("Writing sms storage preference failed.");
    }
  }
  else
  {
    QBI_LOG_D_1("Read sms storage preference %d",
               cache->configuration.storage_pref);
  }

  /* Perform background query of transport layer info in case we are being
     initialized after the modem has registered to IMS */
  qbi_svc_sms_send_wms48_req(ctx);

  /* Allocate an internal transaction to query the SMS configuration for the
     purpose of initializing the cache. */
  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_SMS, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_CID_NONE, 0, NULL);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  /* If qbi_svc_proc_action() returns TRUE, it means the transaction was freed.
     This will only happen if qbi_svc_sms_configuration_q_req encountered an
     error, so this function should return FALSE. */
  return !qbi_svc_proc_action(txn, qbi_svc_sms_configuration_q_req(txn));
} /* qbi_svc_sms_init_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_modify_tag
===========================================================================*/
/*!
    @brief Allocates an internal transaction to issue a
    QMI_WMS_MODIFY_TAG_REQ to change the tag for the given record

    @details

    @param ctx
    @param storage_type
    @param index
*/
/*=========================================================================*/
static void qbi_svc_sms_modify_tag
(
  qbi_ctx_s                    *ctx,
  wms_storage_type_enum_v01     storage_type,
  uint32                        index,
  wms_message_tag_type_enum_v01 tag_type
)
{
  qbi_txn_s *txn;
  wms_modify_tag_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_3("Changing tag to %d for index %d in storage type %d",
              tag_type, index, storage_type);

  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_SMS, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_CID_NONE, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  qmi_req = (wms_modify_tag_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_MODIFY_TAG_REQ_V01,
                                  qbi_svc_sms_modify_tag_wms23_rsp_cb);
  if (qmi_req == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate QMI transaction");
    qbi_txn_free(txn);
  }
  else
  {
    qmi_req->wms_message_tag.storage_type  = storage_type;
    qmi_req->wms_message_tag.storage_index = index;
    qmi_req->wms_message_tag.tag_type      = tag_type;

    qmi_req->message_mode_valid = TRUE;
    qmi_req->message_mode       = WMS_MESSAGE_MODE_GW_V01;

    (void) qbi_svc_proc_action(txn, QBI_SVC_ACTION_SEND_QMI_REQ);
  }
} /* qbi_svc_sms_modify_tag() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_modify_tag_wms23_rsp_cb
===========================================================================*/
/*!
    @brief Processes a QMI_WMS_MODIFY_TAG_RESP for an internal transaction
    used to modify a record's tag after it has been read or sent

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_modify_tag_wms23_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_modify_tag_req_msg_v01 *qmi_req;
  wms_modify_tag_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wms_modify_tag_req_msg_v01 *) qmi_txn->req.data;
  QBI_LOG_I_2("Received response for modify tag operation on index %d, "
              "storage %d", qmi_req->wms_message_tag.storage_index,
              qmi_req->wms_message_tag.storage_type);

  qmi_rsp = (wms_modify_tag_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }

  /* This is an internal transaction, so just silently abort it... */
  return QBI_SVC_ACTION_ABORT;
} /* qbi_svc_sms_modify_tag_wms23_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_open
===========================================================================*/
/*!
    @brief Initializes the device service on the current context

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_open
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  QBI_LOG_I_0("Processing SMS device service open...");
  if (qbi_svc_cache_alloc(txn->ctx, QBI_SVC_ID_SMS, sizeof(qbi_svc_sms_cache_s))
        == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate cache");
  }
  else if (!qbi_qmi_alloc_svc_handle(txn->ctx, QBI_QMI_SVC_WMS))
  {
    QBI_LOG_E_0("Failure allocating QMI client service handle");
  }
  else if (!qbi_svc_ind_reg_static(
             txn->ctx, QBI_SVC_ID_SMS, qbi_svc_sms_static_ind_hdlrs,
             ARR_SIZE(qbi_svc_sms_static_ind_hdlrs)))
  {
    QBI_LOG_E_0("Couldn't register QMI indication handlers");
  }
  else if (!qbi_svc_sms_init_cache(txn->ctx))
  {
    QBI_LOG_E_0("Couldn't initialize cache");
  }
  else
  {
    action = qbi_svc_sms_route_init_and_reg_inds_with_qmi(txn);
  }

  return action;
} /* qbi_svc_sms_open() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_qmi_location_to_mbim_index
===========================================================================*/
/*!
    @brief Converts a QMI record index and storage type pair into a MBIM
    logical index

    @details

    @param ctx
    @param storage_type
    @param index

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_sms_qmi_location_to_mbim_index
(
  const qbi_ctx_s          *ctx,
  wms_storage_type_enum_v01 storage_type,
  uint32                    index
)
{
  uint32 mbim_index = 0;
  qbi_svc_sms_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_sms_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET_ZERO(cache);

  mbim_index = index;
  if (storage_type == WMS_STORAGE_TYPE_UIM_V01)
  {
    mbim_index += cache->configuration.qmi.store_size_nv;
  }
  /* MBIM indexing starts at 1 */
  mbim_index++;

  return mbim_index;
} /* qbi_svc_sms_qmi_location_to_mbim_index() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_add_entries
===========================================================================*/
/*!
    @brief Allocates and populates the txn->info struct used to track
    fetching multiple SMS records

    @details

    @param txn
    @param storage_type
    @param num_records Number of records to fetch
    @param records Array of size num_records, containing information for
    the records we will fetch

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_rec_list_add_entries
(
  qbi_svc_sms_rec_list_s           *rec_list,
  wms_storage_type_enum_v01         storage_type,
  uint32                            num_records,
  const wms_message_tuple_type_v01 *records
)
{
  uint32 i;
  qbi_svc_sms_rec_list_entry_s *entry;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(rec_list);
  QBI_CHECK_NULL_PTR_RET_FALSE(records);

  QBI_LOG_I_2("Setting up index list for %d records from storage type %d",
              num_records, storage_type);
  rec_list->num_records += num_records;
  for (i = 0; i < num_records; i++)
  {
    QBI_LOG_I_3("  %d. Index %d (tag %d)", i, records[i].message_index,
                records[i].tag_type);

    entry = (qbi_svc_sms_rec_list_entry_s *) QBI_MEM_MALLOC_CLEAR(
      sizeof(qbi_svc_sms_rec_list_entry_s));
    QBI_CHECK_NULL_PTR_RET_FALSE(entry);

    entry->loc.index = records[i].message_index;
    entry->loc.storage_type = storage_type;
    if (!qbi_util_list_push_back_aliased(&rec_list->list, &entry->list_entry))
    {
      QBI_LOG_E_0("Couldn't add entry to list!");
      success = FALSE;
      break;
    }
  }

  return success;
} /* qbi_svc_sms_rec_list_add_entries() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_build_qmi_req
===========================================================================*/
/*!
    @brief Builds a QMI_WMS_LIST_MESSAGES_REQ

    @details

    @param txn
    @param flag MBIM flag to filter results on
    @param storage_type Storage type to query

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_rec_list_build_qmi_req
(
  qbi_txn_s                *txn,
  uint32                    flag,
  wms_storage_type_enum_v01 storage_type
)
{
  qbi_svc_sms_cache_s *cache;
  wms_list_messages_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  cache = qbi_svc_sms_get_cache(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_req = (wms_list_messages_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_LIST_MESSAGES_REQ_V01,
                                  qbi_svc_sms_rec_list_wms31_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->storage_type = storage_type;

  qmi_req->message_mode_valid = TRUE;
  qmi_req->message_mode       = WMS_MESSAGE_MODE_GW_V01;

  if (flag != QBI_SVC_SMS_FLAG_ALL)
  {
    qmi_req->tag_type_valid = TRUE;
    qmi_req->tag_type = qbi_svc_sms_flag_to_qmi_tag_type(flag);
  }

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_sms_rec_list_build_qmi_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_completion_cb
===========================================================================*/
/*!
    @brief Completion callback for transactions using a record list

    @details
    Frees all list entries in the record list structure set in the info
    field. The record list structure itself is not freed here, since it
    will automatically be freed when the transaction is released.

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_sms_rec_list_completion_cb
(
  const qbi_txn_s *txn
)
{
  qbi_svc_sms_rec_list_s *rec_list;
  qbi_svc_sms_rec_list_entry_s *entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);
  QBI_CHECK_NULL_PTR_RET(txn->info);

  rec_list = (qbi_svc_sms_rec_list_s *) txn->info;
  while ((entry = (qbi_svc_sms_rec_list_entry_s *)
            qbi_util_list_pop_back(&rec_list->list)) != NULL)
  {
    QBI_MEM_FREE(entry);
  }
} /* qbi_svc_sms_rec_list_completion_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_fetch_by_flag
===========================================================================*/
/*!
    @brief Fetches a list of SMS record indices based on the given flag,
    then executes proc_record_list() to process the list of record indices

    @details
    The info field in the transaction struct will be used to store the
    pointer to the transaction list. Therefore it must be NULL when
    calling this function.

    @param txn
    @param flag
    @param proc_record_list

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_rec_list_fetch_by_flag
(
  qbi_txn_s          *txn,
  uint32              flag,
  qbi_svc_cmd_hdlr_f *proc_record_list
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  if (qbi_svc_sms_rec_list_init(txn, flag, proc_record_list))
  {
    /* First query NV - UIM will be queried second if needed */
    action = qbi_svc_sms_rec_list_build_qmi_req(
      txn, flag, WMS_STORAGE_TYPE_NV_V01);
  }

  return action;
} /* qbi_svc_sms_rec_list_fetch_by_flag() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_fetch_index
===========================================================================*/
/*!
    @brief Allocates a record list containing a single entry with the given
    logical index

    @details
    This function allocates a record list, but populates it with the value
    for the given index rather than querying QMI for a list of records. It
    is used to keep the processing logic common between cases where only
    a single index is needed and when a list of indices are used.

    @param txn
    @param index

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_rec_list_fetch_index
(
  qbi_txn_s          *txn,
  uint32              index,
  qbi_svc_cmd_hdlr_f *proc_record_list
)
{
  qbi_svc_sms_rec_list_s *rec_list;
  wms_storage_type_enum_v01 storage_type;
  wms_message_tuple_type_v01 record;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(proc_record_list);

  /* The tag_type is not used, but initialize it to a known value for good
     measure */
  record.tag_type = WMS_TAG_TYPE_MT_READ_V01;
  if (!qbi_svc_sms_mbim_index_to_qmi_location(
        txn->ctx, index, &record.message_index, &storage_type))
  {
    QBI_LOG_E_1("Invalid index %d!", index);
    txn->status = QBI_MBIM_STATUS_INVALID_MEMORY_INDEX;
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else if (qbi_svc_sms_rec_list_init(
             txn, QBI_SVC_SMS_FLAG_INDEX, proc_record_list))
  {
    QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
    rec_list = (qbi_svc_sms_rec_list_s *) txn->info;

    if (!qbi_svc_sms_rec_list_add_entries(rec_list, storage_type, 1, &record))
    {
      QBI_LOG_E_0("Couldn't add entry to list!");
    }
    else
    {
      qbi_util_list_iter_init(&rec_list->list, &rec_list->iter);
      action = proc_record_list(txn);
    }
  }

  return action;
} /* qbi_svc_sms_rec_list_fetch_index() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_item_skipped
===========================================================================*/
/*!
    @brief Updates the counters to handle the case where a list item was
    skipped

    @details
    Needed to ensure that qbi_svc_sms_rec_list_get_cur_offset() will return
    the correct offset when an item in the list can't be processed. For
    example: if a SMS entry can't be read (error code NO_ENTRY), then the
    entry is skipped, and the response is downsized accordingly. To make
    sure that the offset into the response buffer does not increment past
    an unused entry, this function must be called.

    @param txn
*/
/*=========================================================================*/
static void qbi_svc_sms_rec_list_item_skipped
(
  qbi_txn_s *txn
)
{
  qbi_svc_sms_rec_list_s *rec_list;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(txn);

  rec_list = (qbi_svc_sms_rec_list_s *) txn->info;
  if (rec_list->records_processed == 0)
  {
    QBI_LOG_E_0("Trying to skip record, but none processed!");
  }
  else
  {
    rec_list->records_processed--;
  }
} /* qbi_svc_sms_rec_list_item_skipped() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_get_cur_mbim_index
===========================================================================*/
/*!
    @brief Retrieves the MBIM logical index of the record currently being
    processed

    @details

    @param txn

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_sms_rec_list_get_cur_mbim_index
(
  const qbi_txn_s *txn
)
{
  uint32 cur_index = 0;
  qbi_svc_sms_rec_list_s *rec_list;
  qbi_svc_sms_rec_list_entry_s *entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(txn);
  QBI_CHECK_NULL_PTR_RET_ZERO(txn->info);

  rec_list = (qbi_svc_sms_rec_list_s *) txn->info;
  entry = (qbi_svc_sms_rec_list_entry_s *) qbi_util_list_iter_cur(
    &rec_list->iter);

  if (entry == NULL)
  {
    QBI_LOG_W_0("Current list position is NULL!");
  }
  else
  {
    cur_index = qbi_svc_sms_qmi_location_to_mbim_index(
      txn->ctx, entry->loc.storage_type, entry->loc.index);
  }

  return cur_index;
} /* qbi_svc_sms_rec_list_get_cur_mbim_index() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_init
===========================================================================*/
/*!
    @brief Initializes a transaction to be used with record list functions

    @details
    Allocates and populates the qbi_svc_sms_rec_list_s structure, and
    sets the transaction's info field to point to it. Sets the completion
    callback to qbi_svc_sms_rec_list_completion_cb, so memory can be cleaned
    up when the transaction is released.

    Note that this does not intialize the list iterator - it should be
    initialized just before calling the proc_record_list callback, so that
    it will work properly with the populated list.

    @param txn
    @param flag
    @param proc_record_list

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_rec_list_init
(
  qbi_txn_s          *txn,
  uint32              flag,
  qbi_svc_cmd_hdlr_f *proc_record_list
)
{
  qbi_svc_sms_rec_list_s *rec_list;
  boolean success = FALSE;
  qbi_os_void_f *fptr = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);

  if (txn->info != NULL ||
      qbi_os_fptr_decode(&txn->completion_cb, &fptr, FALSE))
  {
    QBI_LOG_E_0("Transaction info and completion callback fields must be "
                "NULL!");
  }
  else
  {
    rec_list = (qbi_svc_sms_rec_list_s *) QBI_MEM_MALLOC_CLEAR(
      sizeof(qbi_svc_sms_rec_list_s));
    if (rec_list != NULL)
    {
      qbi_util_list_init(&rec_list->list);
      rec_list->flag = flag;
      rec_list->proc_record_list = proc_record_list;
      txn->info = (void *) rec_list;
      qbi_txn_set_completion_cb(txn, qbi_svc_sms_rec_list_completion_cb);

      success = TRUE;
    }
  }

  return success;
} /* qbi_svc_sms_rec_list_init() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_get_cur_offset
===========================================================================*/
/*!
    @brief Gets the offset to the current record being processed

    @details
    The function qbi_svc_sms_rec_list_get_next() should have been called
    at least once before calling this function.

    @param txn

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_sms_rec_list_get_cur_offset
(
  const qbi_txn_s *txn
)
{
  uint32 cur_offset = 0;
  qbi_svc_sms_rec_list_s *rec_list;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(txn);
  QBI_CHECK_NULL_PTR_RET_ZERO(txn->info);

  rec_list = (qbi_svc_sms_rec_list_s *) txn->info;
  if (rec_list->records_processed == 0)
  {
    QBI_LOG_W_0("Asked for current offset, but no records retrieved yet!");
  }
  else
  {
    cur_offset = (rec_list->records_processed - 1);
  }

  return cur_offset;
} /* qbi_svc_sms_rec_list_get_cur_offset() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_get_next
===========================================================================*/
/*!
    @brief Gets the QMI location information of the next record on the list

    @details

    @param txn
    @param storage_type
    @param index

    @return boolean TRUE if a record was found, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_sms_rec_list_get_next
(
  const qbi_txn_s           *txn,
  wms_storage_type_enum_v01 *storage_type,
  uint32                    *index
)
{
  qbi_svc_sms_rec_list_s *rec_list;
  qbi_svc_sms_rec_list_entry_s *entry;
  boolean entry_found = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->info);
  QBI_CHECK_NULL_PTR_RET_FALSE(storage_type);
  QBI_CHECK_NULL_PTR_RET_FALSE(index);

  rec_list = (qbi_svc_sms_rec_list_s *) txn->info;
  if (qbi_util_list_iter_has_next(&rec_list->iter))
  {
    entry = (qbi_svc_sms_rec_list_entry_s *)
      qbi_util_list_iter_next(&rec_list->iter);
    QBI_CHECK_NULL_PTR_RET_FALSE(entry);

    *storage_type = entry->loc.storage_type;
    *index = entry->loc.index;

    rec_list->records_processed++;
    entry_found = TRUE;
  }

  return entry_found;
} /* qbi_svc_sms_rec_list_get_next() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_num_records
===========================================================================*/
/*!
    @brief Returns the total number of records returned by the initial
    qbi_svc_sms_rec_list_fetch_by_flag request.

    @details
    This is not the number of records waiting to be processed on the list -
    see qbi_svc_sms_rec_list_get_cur_index for that.

    @param txn

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_sms_rec_list_num_records
(
  const qbi_txn_s *txn
)
{
  qbi_svc_sms_rec_list_s *rec_list;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(txn);
  QBI_CHECK_NULL_PTR_RET_ZERO(txn->info);

  rec_list = (qbi_svc_sms_rec_list_s *) txn->info;
  return rec_list->num_records;
} /* qbi_svc_sms_rec_list_num_records() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_rec_list_wms31_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_LIST_MESSAGES_RESP and invokes the record
    list processing function on the created list.

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_rec_list_wms31_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_list_messages_req_msg_v01 *qmi_req;
  wms_list_messages_resp_msg_v01 *qmi_rsp;
  qbi_svc_sms_rec_list_s *rec_list;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rec_list = (qbi_svc_sms_rec_list_s *) qmi_txn->parent->info;
  QBI_CHECK_NULL_PTR_RET_ABORT(rec_list->proc_record_list);

  qmi_req = (wms_list_messages_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wms_list_messages_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_sms_get_failure_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else if (!qbi_svc_sms_rec_list_add_entries(
              rec_list, qmi_req->storage_type, qmi_rsp->message_tuple_len,
              qmi_rsp->message_tuple))
  {
    QBI_LOG_E_0("Couldn't add entries to list!");
  }
  else
  {
    if (qmi_req->storage_type == WMS_STORAGE_TYPE_NV_V01)
    {
      action = qbi_svc_sms_rec_list_build_qmi_req(
        qmi_txn->parent, rec_list->flag, WMS_STORAGE_TYPE_UIM_V01);
    }
    else
    {
      QBI_LOG_I_1("Starting processing of record list with %d entries",
                  rec_list->num_records);
      qbi_util_list_iter_init(&rec_list->list, &rec_list->iter);
      action = rec_list->proc_record_list(qmi_txn->parent);
    }
  }

  return action;
} /* qbi_svc_sms_rec_list_wms31_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_reg_inds_rsp
===========================================================================*/
/*!
    @brief Determines the proper action to take from a QMI response callback
    while registering for indications

    @details

    @param txn

    @return qbi_svc_action_e QBI_SVC_ACTION_SEND_RSP if no QMI transactions
    pending, otherwise QBI_SVC_ACTION_WAIT_ASYNC_RSP
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_reg_inds_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (txn->qmi_txns_pending == 0)
  {
    QBI_LOG_I_0("SMS device service now opened");
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_sms_reg_inds_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_reg_inds_with_qmi
===========================================================================*/
/*!
    @brief Registers for QMI indications that are handled by the static
    indication handlers

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_reg_inds_with_qmi
(
  qbi_txn_s *txn
)
{
  wms_set_event_report_req_msg_v01 *wms01_req;
  wms_indication_register_req_msg_v01 *wms47_req;
/*-------------------------------------------------------------------------*/
  wms01_req = (wms_set_event_report_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_SET_EVENT_REPORT_REQ_V01,
                                  qbi_svc_sms_reg_inds_wms01_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(wms01_req);
  wms01_req->report_mt_message_valid = TRUE;
  wms01_req->report_mt_message = TRUE;

  wms47_req = (wms_indication_register_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_INDICATION_REGISTER_REQ_V01,
                                  qbi_svc_sms_reg_inds_wms47_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(wms47_req);
  wms47_req->reg_transport_layer_info_events_valid = TRUE;
  wms47_req->reg_transport_layer_info_events = TRUE;

  wms47_req->reg_service_ready_events_valid = TRUE;
  wms47_req->reg_service_ready_events = TRUE;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_sms_reg_inds_with_qmi() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_reg_inds_wms01_rsp_cb
===========================================================================*/
/*!
    @brief Handles QMI_WMS_SET_EVENT_REPORT_RESP used for initial indication
    registration at device service open

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_reg_inds_wms01_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_set_event_report_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wms_set_event_report_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for WMS indications!!! Error code %d",
                qmi_rsp->resp.error);
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    action = qbi_svc_sms_reg_inds_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_sms_reg_inds_wms01_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_reg_inds_wms47_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_INDICATION_REGISTER_RESP used for initial
    indication registration at device service open

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_reg_inds_wms47_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wms_indication_register_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wms_indication_register_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Error registering for WMS indications!!! Error code %d",
                qmi_rsp->resp.error);
  }
  else
  {
    action = qbi_svc_sms_reg_inds_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_sms_reg_inds_wms47_rsp_cb() */

/*! @addtogroup MBIM_CID_SMS_CONFIGURATION
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_e_wms46_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_SMSC_ADDRESS_IND

    @details
    As this QMI indication is sent either when the SMSC address has changed,
    it will always result in a MBIM_CID_SMS_CONFIGURATION event.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_e_wms46_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);

  /* Since we rely on QMI to only send the indication when something has
     changed, simply perform the same processing as for a query. */
  return qbi_svc_sms_configuration_q_req(ind->txn);
} /* qbi_svc_sms_configuration_e_wms46_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_e_wms5d_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_SERVICE_READY_IND

    @details
    As this QMI indication is sent when the SMS subsystem becomes available,
    it will always result in a MBIM_CID_SMS_CONFIGURATION event.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_e_wms5d_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);

  /* Since we rely on QMI to only send the indication when something has
     changed, simply perform the same processing as for a query. */
  return qbi_svc_sms_configuration_q_req(ind->txn);
} /* qbi_svc_sms_configuration_e_wms5d_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SMS_CONFIGURATION query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_q_req
(
  qbi_txn_s *txn
)
{
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_sms_configuration_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  /* Pre-allocate the response buffer so it can be used by the QMI callbacks */
  rsp = (qbi_svc_sms_configuration_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_sms_configuration_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  /* QMI_WMS_GET_MESSAGE_PROTOCOL (0x30) */
  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_WMS,
                              QMI_WMS_GET_MESSAGE_PROTOCOL_REQ_V01,
                              qbi_svc_sms_configuration_q_wms30_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  /* QMI_WMS_GET_SMSC_ADDRESS (0x34) */
  qmi_txn = qbi_qmi_txn_alloc(txn, QBI_QMI_SVC_WMS,
                              QMI_WMS_GET_SMSC_ADDRESS_REQ_V01,
                              qbi_svc_sms_configuration_q_wms34_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_sms_configuration_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_q_req_max_msgs
===========================================================================*/
/*!
    @brief Based on the given message format, builds the appropriate query
    to determine the message capacity via QMI_WMS_GET_STORE_MAX_SIZE

    @details

    @param txn
    @param storage_type

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_q_req_max_msgs
(
  qbi_txn_s                *txn,
  wms_storage_type_enum_v01 storage_type
)
{
  wms_get_store_max_size_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (wms_get_store_max_size_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_GET_STORE_MAX_SIZE_REQ_V01,
                                  qbi_svc_sms_configuration_q_wms36_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->message_mode_valid = TRUE;
  qmi_req->storage_type = storage_type;
  qmi_req->message_mode = WMS_MESSAGE_MODE_GW_V01;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_sms_configuration_q_req_max_msgs() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_q_rsp
===========================================================================*/
/*!
    @brief Decides whether we have enough information to send the response
    to the host

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_q_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  if (txn->qmi_txns_pending == 0)
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }
  return action;
} /* qbi_svc_sms_configuration_q_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_q_wms30_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_GET_MESSAGE_PROTOCOL_RESP for
    MBIM_CID_SMS_CONFIGURATION query

    @details
    Note that except in case of error, this will also result in a new
    query for QMI_WMS_GET_STORE_MAX_SIZE

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_q_wms30_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_sms_configuration_rsp_s *rsp;
  wms_get_message_protocol_resp_msg_v01 *qmi_rsp;
  boolean failure = FALSE;
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = (qbi_svc_sms_configuration_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_rsp = (wms_get_message_protocol_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    /* DEVICE_UNSUPPORTED means that the device can support both GW and CDMA
       message protocols. In this case, we default to GW per MBIM convention. */
    if (qmi_rsp->resp.error == QMI_ERR_OP_DEVICE_UNSUPPORTED_V01)
    {
      QBI_LOG_I_0("Device supports both CDMA and GW SMS protocols");
      rsp->format = QBI_SVC_SMS_FORMAT_PDU;
    }
    else
    {
      QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
      qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
      failure = TRUE;
    }
  }
  else
  {
    if (qmi_rsp->message_protocol == WMS_MESSAGE_PROTOCOL_WCDMA_V01 ||
        qmi_rsp->message_protocol == WMS_MESSAGE_PROTOCOL_CDMA_V01)
    {
      rsp->format = QBI_SVC_SMS_FORMAT_PDU;
    }
    else
    {
      QBI_LOG_E_1("Received unrecognized/unsupported message protocol %d!",
                  qmi_rsp->message_protocol);
      qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
      failure = TRUE;
    }
  }

  if (failure)
  {
    action = qbi_svc_sms_configuration_q_rsp(qmi_txn->parent);
  }
  else
  {
    /* We need to present multiple physical storages as a single logical storage,
       so always query NV storage size first - if UIM is supported, then that
       will be queried second. */
    (void) qbi_svc_sms_configuration_update_cache_format(
      qmi_txn->ctx, rsp->format);
    action = qbi_svc_sms_configuration_q_req_max_msgs(
      qmi_txn->parent, WMS_STORAGE_TYPE_NV_V01);
  }

  return action;
} /* qbi_svc_sms_configuration_q_wms30_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_q_wms34_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_GET_SMSC_ADDRESS_RESP for
    MBIM_CID_SMS_CONFIGURATION query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_q_wms34_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_sms_configuration_rsp_s *rsp;
  wms_get_smsc_address_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  rsp = (qbi_svc_sms_configuration_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_rsp = (wms_get_smsc_address_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_DEVICE_NOT_READY_V01)
  {
    if (qmi_rsp->resp.error == QMI_ERR_NOT_PROVISIONED_V01)
    {
      QBI_LOG_W_0("SMSC address is not provisioned!");
      /* In this case, we leave the SMSC address field set to all 0x00. */
    }
    else
    {
      QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
      qbi_svc_sms_get_failure_status(qmi_txn->parent, qmi_rsp->resp.error);
    }
  }
  else if (qmi_rsp->resp.error == QMI_ERR_DEVICE_NOT_READY_V01)
  {
    QBI_LOG_W_0("Device not ready to retrieve SMSC address");
    rsp->sms_storage_state = QBI_SVC_SMS_STORAGE_STATE_NOT_INIT;
  }
  else
  {
    rsp->sms_storage_state = QBI_SVC_SMS_STORAGE_STATE_INITIALIZED;
    if (!qbi_txn_rsp_databuf_add_string_ascii_to_utf16(
          qmi_txn->parent, &rsp->sc_address, 0,
          QBI_SVC_SMS_SC_ADDR_MAX_LEN_BYTES,
          qmi_rsp->smsc_address.smsc_address_digits,
          qmi_rsp->smsc_address.smsc_address_digits_len))
    {
      QBI_LOG_E_0("Couldn't append SMSC address to response!");
      qmi_txn->parent->status = QBI_MBIM_STATUS_FAILURE;
    }
  }

  return qbi_svc_sms_configuration_q_rsp(qmi_txn->parent);
} /* qbi_svc_sms_configuration_q_wms34_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_q_wms36_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_GET_STORE_MAX_SIZE_RESP for
    MBIM_CID_SMS_CONFIGURATION query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_q_wms36_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_sms_cache_s *cache;
  qbi_svc_sms_configuration_rsp_s *rsp;
  wms_get_store_max_size_req_msg_v01 *qmi_req;
  wms_get_store_max_size_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_sms_get_cache(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_sms_configuration_rsp_s *) qmi_txn->parent->rsp.data;
  qmi_req = (wms_get_store_max_size_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wms_get_store_max_size_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_sms_get_failure_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else
  {
    /* NV storage capacity is always tracked, but it is not reported in UIM
       storage only configuration */
    if (qmi_req->storage_type == WMS_STORAGE_TYPE_UIM_V01 ||
        qbi_svc_sms_configuration_get_preferred_storage_type(qmi_txn->ctx) !=
          WMS_STORAGE_TYPE_UIM_V01)
    {
      rsp->max_messages += qmi_rsp->mem_store_max_size;
    }

    if (qmi_req->storage_type == WMS_STORAGE_TYPE_NV_V01)
    {
      QBI_LOG_I_1("NV storage size is %d", qmi_rsp->mem_store_max_size);
      cache->configuration.qmi.store_size_nv = qmi_rsp->mem_store_max_size;

      /* Query the size of UIM storage */
      action = qbi_svc_sms_configuration_q_req_max_msgs(
        qmi_txn->parent, WMS_STORAGE_TYPE_UIM_V01);
    }
    else if (qmi_req->storage_type == WMS_STORAGE_TYPE_UIM_V01)
    {
      QBI_LOG_I_1("UIM storage size is %d", qmi_rsp->mem_store_max_size);
      cache->configuration.qmi.store_size_uim = qmi_rsp->mem_store_max_size;
      action = qbi_svc_sms_configuration_q_rsp(qmi_txn->parent);
    }
    else
    {
      QBI_LOG_E_1("Unexpected storage_type %d!", qmi_req->storage_type);
    }
  }

  return action;
} /* qbi_svc_sms_configuration_q_wms36_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_s_change_sc_addr
===========================================================================*/
/*!
    @brief Builds a QMI_WMS_SET_SMSC_ADDRESS request

    @details
    Error checking of the request should have already been performed

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_s_change_sc_addr
(
  qbi_txn_s *txn
)
{
  uint32 bytes_needed;
  qbi_svc_action_e action;
  qbi_svc_sms_configuration_s_req_s *req;
  const uint8 *new_sc_address;
  wms_set_smsc_address_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  qmi_req = (wms_set_smsc_address_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_SET_SMSC_ADDRESS_REQ_V01,
                                  qbi_svc_sms_configuration_s_wms35_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  req = (qbi_svc_sms_configuration_s_req_s *) txn->req.data;
  new_sc_address = (const uint8 *) qbi_txn_req_databuf_get_field(
    txn, &req->sc_address, 0, QBI_SVC_SMS_SC_ADDR_MAX_LEN_BYTES);
  QBI_CHECK_NULL_PTR_RET_ABORT(new_sc_address);

  bytes_needed = qbi_util_utf16_to_ascii(
    new_sc_address, req->sc_address.size, qmi_req->smsc_address_digits,
    sizeof(qmi_req->smsc_address_digits));
  if (bytes_needed > sizeof(qmi_req->smsc_address_digits))
  {
    QBI_LOG_E_2("Couldn't change SMSC address - request too long! Needed %d "
                "bytes, have room for %d", bytes_needed,
                sizeof(qmi_req->smsc_address_digits));
    action = QBI_SVC_ACTION_ABORT;
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_sms_configuration_s_change_sc_addr() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SMS_CONFIGURATION set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_sms_configuration_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_sms_configuration_s_req_s *) txn->req.data;
  if (req->format != QBI_SVC_SMS_FORMAT_PDU)
  {
    /*! @note Only PDU format is supported */
    QBI_LOG_E_1("Changing SMS format to %d not supported!", req->format);
    txn->status = QBI_MBIM_STATUS_SMS_FORMAT_NOT_SUPPORTED;
  }
  else if (req->sc_address.offset == 0)
  {
    QBI_LOG_I_0("SMSC address in request is empty; no change");
    action = qbi_svc_sms_configuration_q_req(txn);
  }
  else
  {
    action = qbi_svc_sms_configuration_s_change_sc_addr(txn);
  }

  return action;
} /* qbi_svc_sms_configuration_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_s_wms35_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_SET_SMSC_ADDRESS_RESP for a
    MBIM_CID_SMS_CONFIGURATION set request to change the Service Center
    address

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_configuration_s_wms35_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_set_smsc_address_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wms_set_smsc_address_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_sms_get_failure_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_0("Successfully changed SMSC address");
  }

  /* Issue a query to populate the response data */
  return qbi_svc_sms_configuration_q_req(qmi_txn->parent);
} /* qbi_svc_sms_configuration_s_wms35_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_get_preferred_storage_type
===========================================================================*/
/*!
    @brief Get preferred SMS storage type used

    @details NV storage is by default the preferred SMS storage for multiple
    storage configuration. UIM storage is preferred storage in UIM storage
    only configuration where NV storage usage is restricted.

    @param ctx

    @return wms_storage_type_enum_v01
*/
/*=========================================================================*/
static wms_storage_type_enum_v01 qbi_svc_sms_configuration_get_preferred_storage_type
(
  qbi_ctx_s *ctx
)
{
  qbi_svc_sms_cache_s *cache;
  wms_storage_type_enum_v01 storage_pref = WMS_STORAGE_TYPE_NV_V01;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_sms_get_cache(ctx);
  if (cache == NULL)
  {
    QBI_LOG_E_0("Unexpected NULL pointer!");
  }
  else
  {
    if (cache->configuration.storage_pref == WMS_STORAGE_TYPE_UIM_V01)
    {
      storage_pref = WMS_STORAGE_TYPE_UIM_V01;
    }
  }
  QBI_LOG_D_1("SMS storage preference is %d", storage_pref);
  return storage_pref;
} /* qbi_svc_sms_configuration_get_preferred_storage_type() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_configuration_update_cache_format
===========================================================================*/
/*!
    @brief Updates the cached SMS format

    @details

    @param ctx
    @param format

    @return boolean TRUE if the cached value changed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_svc_sms_configuration_update_cache_format
(
  qbi_ctx_s *ctx,
  uint32     format
)
{
  qbi_svc_sms_cache_s *cache;
  boolean changed = FALSE;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_sms_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  if (format != cache->configuration.format)
  {
    QBI_LOG_I_2("SMS format changed from %d to %d", cache->configuration.format,
                format);
    cache->configuration.format = format;
    changed = TRUE;
  }

  return changed;
} /* qbi_svc_sms_configuration_update_cache_format() */

/*! @} */

/*! @addtogroup MBIM_CID_SMS_READ
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_e_build_rsp
===========================================================================*/
/*!
    @brief Allocates and populates the InformationBuffer for a new
    MBIM_CID_SMS_READ event using the given broadcast SMS data from QMI

    @details

    @param txn
    @param msg_data

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_read_e_build_rsp
(
  qbi_txn_s                                    *txn,
  const wms_transfer_route_mt_message_type_v01 *msg_data
)
{
  qbi_svc_sms_read_rsp_s *rsp;
  qbi_mbim_offset_size_pair_s *field_desc;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  uint8 transcoded_pdu[WMS_MESSAGE_LENGTH_MAX_V01];
  uint32 transcoded_pdu_len;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(msg_data);

  rsp = (qbi_svc_sms_read_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, sizeof(qbi_svc_sms_read_rsp_s) + sizeof(qbi_mbim_offset_size_pair_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->format = QBI_SVC_SMS_FORMAT_PDU;
  rsp->element_count = 1;

  field_desc = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) rsp + sizeof(qbi_svc_sms_read_rsp_s));

  if (msg_data->format == WMS_MESSAGE_FORMAT_CDMA_V01)
  {
    QBI_LOG_I_0("Transcoding incoming CDMA PDU to GW PP format");

    transcoded_pdu_len = qbi_svc_sms_pdu_cdma_to_gw(
      (const uint8 *) msg_data->data, msg_data->data_len, transcoded_pdu,
      sizeof(transcoded_pdu));
    if (transcoded_pdu_len == 0 ||
        !qbi_svc_sms_read_eq_add_record_pdu(
          txn, field_desc, QBI_SVC_SMS_INDEX_NONE, QBI_SVC_SMS_STATUS_NEW,
          WMS_MESSAGE_FORMAT_GW_PP_V01, transcoded_pdu_len, transcoded_pdu))
    {
      QBI_LOG_E_0("Couldn't add transcoded record for SMS_READ event!");
    }
    else
    {
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }
  else if (!qbi_svc_sms_read_eq_add_record_pdu(
             txn, field_desc, QBI_SVC_SMS_INDEX_NONE, QBI_SVC_SMS_STATUS_NEW,
             msg_data->format, msg_data->data_len, msg_data->data))
  {
    QBI_LOG_E_0("Couldn't add record for SMS_READ event!");
  }
  else
  {
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_sms_read_e_build_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_e_send_ack
===========================================================================*/
/*!
    @brief Allocates an internal transaction and issues a
    QMI_WMS_SEND_ACK_REQ for the given transaction ID

    @details

    @param ctx
    @param sms_transaction_id
*/
/*=========================================================================*/
static void qbi_svc_sms_read_e_send_ack
(
  qbi_ctx_s *ctx,
  uint32     sms_transaction_id
)
{
  qbi_txn_s *txn;
  wms_send_ack_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_1("Sending ACK for SMS transaction ID %d", sms_transaction_id);
  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_SMS, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_CID_NONE, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  qmi_req = (wms_send_ack_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_SEND_ACK_REQ_V01,
                                  qbi_svc_sms_read_e_wms37_rsp_cb);
  QBI_CHECK_NULL_PTR_RET(qmi_req);

  qmi_req->ack_information.message_protocol = WMS_MESSAGE_PROTOCOL_WCDMA_V01;
  qmi_req->ack_information.transaction_id   = sms_transaction_id;
  qmi_req->ack_information.success = TRUE;

  (void) qbi_svc_proc_action(txn, QBI_SVC_ACTION_SEND_QMI_REQ);
} /* qbi_svc_sms_read_e_send_ack() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_e_wms01_ind_cb
===========================================================================*/
/*!
    @brief Handles QMI_WMS_EVENT_REPORT_IND, looking for a new broadcast
    SMS message

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_read_e_wms01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wms_event_report_ind_msg_v01 *qmi_ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (wms_event_report_ind_msg_v01 *) ind->buf->data;

  //In case of class 0 sms , SMSC is received as seperate TLV
  if(qmi_ind->mt_message_smsc_address_valid)
  {
    if((qmi_ind->transfer_route_mt_message.data_len +
      qmi_ind->mt_message_smsc_address.data_len + 1) <
	  WMS_MESSAGE_LENGTH_MAX_V01)
    {
      QBI_LOG_I_2("data_len smsc %d data_len pdu %d",qmi_ind->mt_message_smsc_address.data_len,
                qmi_ind->transfer_route_mt_message.data_len);

      //Copy and move ahead actual PDU payload by smsc data_len bytes
      QBI_MEMMOVE(qmi_ind->transfer_route_mt_message.data + 1 + qmi_ind->mt_message_smsc_address.data_len,
           qmi_ind->transfer_route_mt_message.data,qmi_ind->transfer_route_mt_message.data_len);
      //Increase the data length of PDU after adding SMSC data to PDU payload including the SMSC data len
      qmi_ind->transfer_route_mt_message.data_len += (qmi_ind->mt_message_smsc_address.data_len + 1);
      //Copy the smsc data len as first byte in the PDU payload
      qmi_ind->transfer_route_mt_message.data[0] = qmi_ind->mt_message_smsc_address.data_len;
      //Copy the smsc data in the PDU payload
      QBI_MEMSCPY(qmi_ind->transfer_route_mt_message.data + 1,qmi_ind->transfer_route_mt_message.data_len,
               qmi_ind->mt_message_smsc_address.data,qmi_ind->mt_message_smsc_address.data_len);
    }
    else
    {
         QBI_LOG_E_1("Received unexpected/unsupported data_len for Class 0 %d",
                      qmi_ind->transfer_route_mt_message.data_len +
                      qmi_ind->mt_message_smsc_address.data_len + 1);
         return action;
    }

  }

  if (qmi_ind->transfer_route_mt_message_valid)
  {
    if (qmi_ind->transfer_route_mt_message.format ==
          WMS_MESSAGE_FORMAT_GW_PP_V01 ||
        qmi_ind->transfer_route_mt_message.format ==
          WMS_MESSAGE_FORMAT_CDMA_V01)
    {
      QBI_LOG_I_0("Received new broadcast SMS! Sending SMS_READ event");
      action = qbi_svc_sms_read_e_build_rsp(
        ind->txn, &qmi_ind->transfer_route_mt_message);

      if (qmi_ind->transfer_route_mt_message.ack_indicator ==
            WMS_ACK_INDICATOR_SEND_ACK_V01)
      {
        qbi_svc_sms_read_e_send_ack(
          ind->txn->ctx, qmi_ind->transfer_route_mt_message.transaction_id);
      }
    }
    else
    {
      QBI_LOG_E_1("Received transfer route MT message indication with "
                  "unexpected/unsupported format %d",
                  qmi_ind->transfer_route_mt_message.format);
    }
  }

  return action;
} /* qbi_svc_sms_read_e_wms01_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_e_wms37_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_SEND_ACK_RESP for the internal transaction
    generated by a broadcast SMS requiring an ACK

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_read_e_wms37_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_send_ack_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wms_send_ack_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }

  return QBI_SVC_ACTION_ABORT;
} /* qbi_svc_sms_read_e_wms37_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_eq_add_record_pdu
===========================================================================*/
/*!
    @brief Allocates and populates a qbi_svc_sms_pdu_record_s, including
    PduData

    @details
    The PDU record is allocated in the DataBuffer.

    @param txn
    @param field_desc Offset/size pair to populate with information for the
    newly added record
    @param message_index
    @param message_status
    @param format QMI format of the raw SMS data
    @param data_len Length of the raw SMS data
    @param data Buffer containing raw SMS data

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_read_eq_add_record_pdu
(
  qbi_txn_s                   *txn,
  qbi_mbim_offset_size_pair_s *field_desc,
  uint32                       message_index,
  uint32                       message_status,
  wms_message_format_enum_v01  format,
  uint32                       data_len,
  const uint8_t               *data
)
{
  qbi_svc_sms_pdu_record_s *rsp_record;
  uint8 *pdu_data;
  boolean success = FALSE;
  uint32 initial_offset;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(data);
  QBI_CHECK_NULL_PTR_RET_FALSE(field_desc);

  if (format != WMS_MESSAGE_FORMAT_GW_PP_V01 &&
      format != WMS_MESSAGE_FORMAT_CDMA_V01)
  {
    QBI_LOG_E_1("Unexpected message format %d", format);
  }
  else if (data_len > QBI_SVC_SMS_PDU_DATA_LEN_BYTES)
  {
    QBI_LOG_E_2("PDU data is too long! Length is %d, max is %d",
                data_len, QBI_SVC_SMS_PDU_DATA_LEN_BYTES);
  }
  else
  {
    initial_offset = txn->infobuf_len_total;
    rsp_record = (qbi_svc_sms_pdu_record_s *) qbi_txn_rsp_databuf_add_field(
      txn, field_desc, 0, sizeof(qbi_svc_sms_pdu_record_s), NULL);
    QBI_CHECK_NULL_PTR_RET_FALSE(rsp_record);

    rsp_record->message_index  = message_index;
    rsp_record->message_status = message_status;

    pdu_data = qbi_txn_rsp_databuf_add_field(
      txn, &rsp_record->pdu_data, initial_offset, data_len, data);
    QBI_CHECK_NULL_PTR_RET_FALSE(pdu_data);

    field_desc->size += data_len;
    success = qbi_txn_rsp_databuf_consolidate(txn);
  }

  return success;
} /* qbi_svc_sms_read_eq_add_record_pdu() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_q_copy_pdu_data
===========================================================================*/
/*!
    @brief Copies the SMS record data from QMI format into MBIM
    SMS_PDU_RECORD format in a (potentially) multi-record response

    @details

    @param txn
    @param msg_data

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_read_q_copy_pdu_data
(
  qbi_txn_s                                *txn,
  const wms_read_raw_message_data_type_v01 *msg_data
)
{
  qbi_mbim_offset_size_pair_s *field_desc;
  uint32 message_index;
  uint32 message_status;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(msg_data);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->rsp.data);

  field_desc = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) txn->rsp.data + sizeof(qbi_svc_sms_read_rsp_s) +
     (qbi_svc_sms_rec_list_get_cur_offset(txn) *
      sizeof(qbi_mbim_offset_size_pair_s)));
  message_index = qbi_svc_sms_rec_list_get_cur_mbim_index(txn);
  message_status = qbi_svc_sms_read_qmi_tag_type_to_status(
    msg_data->tag_type);

  return qbi_svc_sms_read_eq_add_record_pdu(
    txn, field_desc, message_index, message_status, msg_data->format,
    msg_data->data_len, msg_data->data);
} /* qbi_svc_sms_read_q_copy_pdu_data() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_q_downsize_rsp
===========================================================================*/
/*!
    @brief Re-allocates the response buffer to hold one less SMS record

    @details

    @param txn

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_read_q_downsize_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_sms_read_rsp_s *rsp;
  uint32 new_size;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->rsp.data);

  rsp = (qbi_svc_sms_read_rsp_s *) txn->rsp.data;
  if (rsp->element_count == 0)
  {
    QBI_LOG_E_0("Can't downsize response when element count already 0!");
  }
  else
  {
    rsp->element_count--;
    QBI_LOG_I_1("Reducing number of elements in response to %d",
                rsp->element_count);
    new_size = sizeof(qbi_svc_sms_read_rsp_s) +
      (rsp->element_count * sizeof(qbi_mbim_offset_size_pair_s));
    txn->infobuf_len_total -= sizeof(qbi_mbim_offset_size_pair_s);

    if (!qbi_util_buf_resize(&txn->rsp, new_size))
    {
      QBI_LOG_E_2("Couldn't resize response buffer from %d to %d bytes!",
                  txn->rsp.size, new_size);
    }
    else
    {
      qbi_svc_sms_rec_list_item_skipped(txn);
      success = TRUE;
    }
  }

  return success;
} /* qbi_svc_sms_read_q_downsize_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_q_get_next_record
===========================================================================*/
/*!
    @brief Fetches the next entry in a multi-record MBIM_CID_SMS_READ query

    @details

    @param txn
    @param qmi_txn Pointer to QMI_WMS_RAW_READ transaction to re-use, or NULL
    if one should be allocated

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_read_q_get_next_record
(
  qbi_txn_s     *txn,
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action;
  wms_raw_read_req_msg_v01 *qmi_req;
  wms_storage_type_enum_v01 storage_type;
  uint32 index;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (!qbi_svc_sms_rec_list_get_next(txn, &storage_type, &index))
  {
    QBI_LOG_I_0("No records left to read!");
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    QBI_LOG_I_2("Setting up read of index %d from storage type %d",
                index, storage_type);
    if (qmi_txn == NULL)
    {
      qmi_txn = qbi_qmi_txn_alloc(
        txn, QBI_QMI_SVC_WMS, QMI_WMS_RAW_READ_REQ_V01,
        qbi_svc_sms_read_q_wms22_rsp_cb);
      QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
    }
    else
    {
      qbi_qmi_txn_reset(qmi_txn);
    }

    qmi_req = (wms_raw_read_req_msg_v01 *) qmi_txn->req.data;
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->message_memory_storage_identification.storage_type = storage_type;
    qmi_req->message_memory_storage_identification.storage_index = index;

    qmi_req->message_mode_valid = TRUE;
    qmi_req->message_mode       = WMS_MESSAGE_MODE_GW_V01;

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_sms_read_q_get_next_record() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_q_proc_list
===========================================================================*/
/*!
    @brief Starts processing the record list for a multi-record
    MBIM_CID_SMS_READ query

    @details
    Allocates the response InformationBuffer and issues the first call
    to qbi_svc_sms_read_q_get_next_record()

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_read_q_proc_list
(
  qbi_txn_s *txn
)
{
  qbi_svc_sms_read_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  /* Allocate the static part of the response buffer. */
  rsp = (qbi_svc_sms_read_rsp_s *) qbi_txn_alloc_rsp_buf(
    txn, (sizeof(qbi_svc_sms_read_rsp_s) +
          (sizeof(qbi_mbim_offset_size_pair_s) *
           qbi_svc_sms_rec_list_num_records(txn))));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->format = QBI_SVC_SMS_FORMAT_PDU;
  rsp->element_count = qbi_svc_sms_rec_list_num_records(txn);

  /* Start reading the records. Only issue one QMI request at a time to
     avoid running out of memory if there are many records to fetch. */
  return qbi_svc_sms_read_q_get_next_record(txn, NULL);
} /* qbi_svc_sms_read_q_get_next_record() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SMS_READ query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_read_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_sms_read_q_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_sms_read_q_req_s *) txn->req.data;
  if (req->format != QBI_SVC_SMS_FORMAT_PDU)
  {
    QBI_LOG_E_1("Request to read unsupported SMS format %d", req->format);
    txn->status = QBI_MBIM_STATUS_SMS_FORMAT_NOT_SUPPORTED;
  }
  else if (req->flag > QBI_SVC_SMS_FLAG_MAX)
  {
    QBI_LOG_E_1("Request to read unexpected flag %d", req->flag);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
  }
  else if (req->flag == QBI_SVC_SMS_FLAG_INDEX)
  {
    action = qbi_svc_sms_rec_list_fetch_index(
      txn, req->message_index, qbi_svc_sms_read_q_proc_list);
  }
  else
  {
    action = qbi_svc_sms_rec_list_fetch_by_flag(
      txn, req->flag, qbi_svc_sms_read_q_proc_list);
  }

  return action;
} /* qbi_svc_sms_read_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_q_wms22_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_RAW_READ_RESP for MBIM_CID_SMS_READ query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_read_q_wms22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_raw_read_req_msg_v01 *qmi_req;
  wms_raw_read_resp_msg_v01 *qmi_rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wms_raw_read_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wms_raw_read_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      qmi_rsp->resp.error != QMI_ERR_NO_ENTRY_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_sms_get_failure_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else if (qmi_rsp->resp.error == QMI_ERR_NO_ENTRY_V01)
  {
    QBI_LOG_W_2("No entry found at storage %d index %d",
                qmi_req->message_memory_storage_identification.storage_type,
                qmi_req->message_memory_storage_identification.storage_index);
    if (qbi_svc_sms_read_q_downsize_rsp(qmi_txn->parent))
    {
      action = qbi_svc_sms_read_q_get_next_record(qmi_txn->parent, qmi_txn);
    }
  }
  else if (!qbi_svc_sms_read_q_copy_pdu_data(qmi_txn->parent,
                                             &qmi_rsp->raw_message_data))
  {
    QBI_LOG_E_0("Error copying record data");
  }
  else
  {
    if (qmi_rsp->raw_message_data.tag_type == WMS_TAG_TYPE_MT_NOT_READ_V01)
    {
      qbi_svc_sms_modify_tag(
        qmi_txn->ctx,
        qmi_req->message_memory_storage_identification.storage_type,
        qmi_req->message_memory_storage_identification.storage_index,
        WMS_TAG_TYPE_MT_READ_V01);
    }
    action = qbi_svc_sms_read_q_get_next_record(qmi_txn->parent, qmi_txn);
  }

  if (action == QBI_SVC_ACTION_SEND_RSP)
  {
    qbi_svc_sms_status_update_flags_if_set(
      qmi_txn->ctx, QBI_SVC_SMS_STATUS_FLAG_NEW_MESSAGE);
  }

  return action;
} /* qbi_svc_sms_read_q_wms22_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_read_qmi_tag_type_to_status
===========================================================================*/
/*!
    @brief Maps a QMI WMS tag_type value to MBIM SMS_MESSAGE_STATUS

    @details

    @param tag_type

    @return uint32
*/
/*=========================================================================*/
static uint32 qbi_svc_sms_read_qmi_tag_type_to_status
(
  wms_message_tag_type_enum_v01 tag_type
)
{
  uint32 status;
/*-------------------------------------------------------------------------*/
  switch (tag_type)
  {
    case WMS_TAG_TYPE_MT_READ_V01:
      status = QBI_SVC_SMS_STATUS_OLD;
      break;

    case WMS_TAG_TYPE_MT_NOT_READ_V01:
      status = QBI_SVC_SMS_STATUS_NEW;
      break;

    case WMS_TAG_TYPE_MO_SENT_V01:
      status = QBI_SVC_SMS_STATUS_SENT;
      break;

    case WMS_TAG_TYPE_MO_NOT_SENT_V01:
      status = QBI_SVC_SMS_STATUS_DRAFT;
      break;

    default:
      QBI_LOG_E_1("Unexpected tag type %d", tag_type);
      status = QBI_SVC_SMS_STATUS_OLD;
  }

  return status;
} /* qbi_svc_sms_read_qmi_tag_type_to_status() */

/*! @} */

/*! @addtogroup MBIM_CID_SMS_SEND
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_s_build_wms20_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WMS_RAW_SEND_REQ with the SMS PDU
    in the CID request InformationBuffer, performing GW to CDMA transcoding
    if necessary

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_send_s_build_wms20_req
(
  qbi_txn_s *txn
)
{
  uint8 *pdu_data;
  qbi_svc_sms_send_pdu_s *req_pdu;
  wms_raw_send_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (!qbi_svc_sms_send_s_get_req_pdu(txn, &req_pdu, &pdu_data))
  {
    QBI_LOG_E_0("Couldn't extract PDU from request!");
  }
  else
  {
    qmi_req = (wms_raw_send_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WMS, QMI_WMS_RAW_SEND_REQ_V01,
        qbi_svc_sms_send_s_wms20_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    if (qbi_svc_sms_send_s_is_gw_to_cdma_conversion_required(txn->ctx))
    {
      qmi_req->raw_message_data.format = WMS_MESSAGE_FORMAT_CDMA_V01;
      qmi_req->raw_message_data.raw_message_len =
        qbi_svc_sms_pdu_gw_to_cdma(
          pdu_data, req_pdu->pdu_data.size,
          qmi_req->raw_message_data.raw_message,
          sizeof(qmi_req->raw_message_data.raw_message));
    }
    else
    {
      qmi_req->raw_message_data.format = WMS_MESSAGE_FORMAT_GW_PP_V01;
      qmi_req->raw_message_data.raw_message_len = req_pdu->pdu_data.size;
      QBI_MEMSCPY(qmi_req->raw_message_data.raw_message,
                  sizeof(qmi_req->raw_message_data.raw_message),
                  pdu_data, req_pdu->pdu_data.size);
    }

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_sms_send_s_build_wms20_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_s_get_req_pdu
===========================================================================*/
/*!
    @brief Gets pointers to the PDU information in a MBIM_SMS_SEND set
    request

    @details
    If return value is TRUE, then req_pdu and pdu_data are guaranteed to be
    set to a non-NULL value

    @param txn
    @param req_pdu
    @param pdu_data

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_svc_sms_send_s_get_req_pdu
(
  qbi_txn_s               *txn,
  qbi_svc_sms_send_pdu_s **req_pdu,
  uint8                  **pdu_data
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(txn);
  QBI_CHECK_NULL_PTR_RET_FALSE(txn->req.data);
  QBI_CHECK_NULL_PTR_RET_FALSE(req_pdu);
  QBI_CHECK_NULL_PTR_RET_FALSE(pdu_data);

  *req_pdu = (qbi_svc_sms_send_pdu_s *)
    ((uint8 *) txn->req.data + sizeof(qbi_svc_sms_send_s_req_s));
  *pdu_data = (uint8 *) qbi_txn_req_databuf_get_field(
    txn, &((*req_pdu)->pdu_data), sizeof(qbi_svc_sms_send_s_req_s),
    QBI_SVC_SMS_PDU_DATA_LEN_BYTES);
  QBI_CHECK_NULL_PTR_RET_FALSE(*pdu_data);

  return TRUE;
} /* qbi_svc_sms_send_s_get_req_pdu() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_s_is_gw_to_cdma_conversion_required
===========================================================================*/
/*!
    @brief Checks whether an outbound SMS should first be converted from GW
    to CDMA format

    @details
    Transcoding from GW to CDMA is required only if: the primary message
    mode is GW (i.e. host will send us GW encoded PDUs) and either we are
    registered on IMS using CDMA, or we currently have service on 1x but
    not GSM/WCDMA.

    @param ctx

    @return boolean TRUE if GW to CDMA conversion should be done
*/
/*=========================================================================*/
static boolean qbi_svc_sms_send_s_is_gw_to_cdma_conversion_required
(
  qbi_ctx_s *ctx
)
{
  boolean conversion_required = FALSE;
  uint32 available_data_class;
  qbi_svc_sms_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_sms_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET_FALSE(cache);

  available_data_class = qbi_svc_bc_get_available_data_class(ctx);
  if (cache->send.ims.registered &&
      cache->send.ims.capability == WMS_TRANSPORT_CAP_CDMA_V01)
  {
    QBI_LOG_I_0("SMS over IMS capability is CDMA: transcoding required");
    conversion_required = TRUE;
  }
  else if ((available_data_class & QBI_SVC_BC_DATA_CLASS_1XRTT) != 0 &&
           (available_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_GSM) == 0 &&
           (available_data_class & QBI_SVC_BC_DATA_CLASS_FAMILY_WCDMA) == 0)
  {
    QBI_LOG_I_0("Non-IMS service on 1x but not GW: transcoding required");
    conversion_required = TRUE;
  }

  return conversion_required;
} /* qbi_svc_sms_send_s_is_gw_to_cdma_conversion_required() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SMS_SEND set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_send_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_sms_send_s_req_s *req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_sms_send_s_req_s *) txn->req.data;
  if (!qbi_svc_bc_check_device_state(txn, FALSE, TRUE))
  {
    /* Radio is powered off or we aren't initialized, so we can't send an SMS */
    QBI_LOG_E_0("Can't send SMS message - device not ready");
  }
  else if (req->format != QBI_SVC_SMS_FORMAT_PDU)
  {
    QBI_LOG_E_1("Request to send SMS in unsupported format %d", req->format);
    txn->status = QBI_MBIM_STATUS_SMS_FORMAT_NOT_SUPPORTED;
  }
  else if (txn->req.size < (sizeof(qbi_svc_sms_send_s_req_s) +
                            sizeof(qbi_svc_sms_send_pdu_s)))
  {
    QBI_LOG_E_2("Message too short! Received %d bytes, expected at least %d",
                txn->req.size, (sizeof(qbi_svc_sms_send_s_req_s) +
                                sizeof(qbi_svc_sms_send_pdu_s)));
  }
  else
  {
      action = qbi_svc_sms_send_s_build_wms20_req(txn);
  }

  return action;
} /* qbi_svc_sms_send_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_s_wms20_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_RAW_SEND_RESP for MBIM_CID_SMS_SEND set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_send_s_wms20_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_raw_send_resp_msg_v01 *qmi_rsp;
  qbi_svc_sms_send_rsp_s *rsp;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wms_raw_send_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    if (qmi_rsp->gw_cause_info_valid)
    {
      QBI_LOG_E_2("SMS_SEND failed with RP cause %d TP cause %d",
                  qmi_rsp->gw_cause_info.rp_cause,
                  qmi_rsp->gw_cause_info.tp_cause);
    }

    qbi_svc_sms_get_failure_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_1("SMS sent successfully with message ID %d",
                qmi_rsp->message_id);

    rsp = (qbi_svc_sms_send_rsp_s *)
      qbi_txn_alloc_rsp_buf(qmi_txn->parent, sizeof(qbi_svc_sms_send_rsp_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(rsp);
    rsp->message_ref = qmi_rsp->message_id;
	
    action = QBI_SVC_ACTION_SEND_RSP;
  }

  return action;
} /* qbi_svc_sms_send_s_wms20_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_s_wms21_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_RAW_WRITE_RESP, used to save the outbound SMS
    before sending it over the air

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_send_s_wms21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wms_raw_write_req_msg_v01 *qmi_req;
  wms_raw_write_resp_msg_v01 *qmi_rsp;
  qbi_svc_sms_msg_location_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wms_raw_write_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wms_raw_write_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_2("Couldn't save MO SMS to storage type %d: error %d",
                qmi_req->raw_message_write_data.storage_type,
                qmi_rsp->resp.error);
    if (qmi_rsp->resp.error == QMI_ERR_DEVICE_STORAGE_FULL_V01 &&
        qmi_req->raw_message_write_data.storage_type == WMS_STORAGE_TYPE_NV_V01)
    {
      QBI_LOG_I_0("NV storage full: trying SIM");
      action = qbi_svc_sms_send_s_write(
        qmi_txn->parent, WMS_STORAGE_TYPE_UIM_V01);
    }
    else
    {
      qbi_svc_sms_get_failure_status(qmi_txn->parent, qmi_rsp->resp.error);
    }
  }
  else
  {
    /* Save the location where we stored the message, so we can change its tag
       from NOT_SENT to SENT after we complete the RAW_SEND command. */
    info = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_sms_msg_location_s));
    QBI_CHECK_NULL_PTR_RET_ABORT(info);
    qmi_txn->parent->info = info;

    info->storage_type = qmi_req->raw_message_write_data.storage_type;
    info->index = qmi_rsp->storage_index;

    QBI_LOG_I_2("Wrote MO SMS to index %d in storage type %d",
                info->index, info->storage_type);
    action = qbi_svc_sms_send_s_build_wms20_req(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_sms_send_s_wms21_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_s_write
===========================================================================*/
/*!
    @brief Saves the SMS PDU in the MBIM_SMS_SEND request payload to device
    storage

    @details

    @param txn
    @param storage_type

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_send_s_write
(
  qbi_txn_s                *txn,
  wms_storage_type_enum_v01 storage_type
)
{
  uint8 *pdu_data;
  qbi_svc_sms_send_pdu_s *req_pdu;
  wms_raw_write_req_msg_v01 *qmi_req;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (!qbi_svc_sms_send_s_get_req_pdu(txn, &req_pdu, &pdu_data))
  {
    QBI_LOG_E_0("Couldn't extract PDU from request!");
  }
  else
  {
    qmi_req = (wms_raw_write_req_msg_v01 *)
      qbi_qmi_txn_alloc_ret_req_buf(
        txn, QBI_QMI_SVC_WMS, QMI_WMS_RAW_WRITE_REQ_V01,
        qbi_svc_sms_send_s_wms21_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->raw_message_write_data.format = WMS_MESSAGE_FORMAT_GW_PP_V01;
    qmi_req->raw_message_write_data.storage_type    = storage_type;
    qmi_req->raw_message_write_data.raw_message_len = req_pdu->pdu_data.size;
    QBI_MEMSCPY(qmi_req->raw_message_write_data.raw_message,
                sizeof(qmi_req->raw_message_write_data.raw_message),
                pdu_data, req_pdu->pdu_data.size);

    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  return action;
} /* qbi_svc_sms_send_s_write() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_update_cache_ims
===========================================================================*/
/*!
    @brief Updates the cache with the current IMS registration information

    @details

    @param ctx
    @param registered_ind
    @param transport_layer_info_valid
    @param transport_layer_info
*/
/*=========================================================================*/
static void qbi_svc_sms_send_update_cache_ims
(
  qbi_ctx_s                        *ctx,
  uint8_t                           registered_ind,
  uint8_t                           transport_layer_info_valid,
  wms_transport_layer_info_type_v01 transport_layer_info
)
{
  qbi_svc_sms_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_sms_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (registered_ind && transport_layer_info_valid &&
      transport_layer_info.transport_type == WMS_TRANSPORT_TYPE_IMS_V01)
  {
    QBI_LOG_I_1("Registered on IMS with transport capability %d",
                transport_layer_info.transport_cap);
    cache->send.ims.registered = TRUE;
    cache->send.ims.capability = transport_layer_info.transport_cap;
  }
  else if (!registered_ind)
  {
    QBI_LOG_I_0("Not registered on IMS");
    cache->send.ims.registered = FALSE;
  }
  else
  {
    QBI_LOG_W_2("WMS transport layer is registered, but with unknown transport "
                "type %d (valid %d)", transport_layer_info.transport_type,
                transport_layer_info_valid);
    cache->send.ims.registered = FALSE;
  }
} /* qbi_svc_sms_send_update_cache_ims() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_wms48_req
===========================================================================*/
/*!
    @brief Performs an internal query of QMI_WMS_GET_TRANSPORT_LAYER_INFO

    @details

    @param ctx
*/
/*=========================================================================*/
static void qbi_svc_sms_send_wms48_req
(
  qbi_ctx_s *ctx
)
{
  qbi_txn_s *txn;
  qbi_qmi_txn_s *qmi_txn;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
/*-------------------------------------------------------------------------*/
  txn = qbi_txn_alloc(ctx, QBI_SVC_ID_BC, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                      QBI_SVC_SMS_MBIM_CID_SMS_SEND, 0, NULL);
  QBI_CHECK_NULL_PTR_RET(txn);

  qmi_txn = qbi_qmi_txn_alloc(
    txn, QBI_QMI_SVC_WMS, QMI_WMS_GET_TRANSPORT_LAYER_INFO_REQ_V01,
    qbi_svc_sms_send_wms48_rsp_cb);
  if (qmi_txn != NULL)
  {
    action = QBI_SVC_ACTION_SEND_QMI_REQ;
  }

  (void) qbi_svc_proc_action(txn, action);
} /* qbi_svc_sms_send_wms48_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_wms48_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_GET_TRANSPORT_LAYER_INFO_RESP: updates the
    cache with the current information

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_send_wms48_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_get_transport_layer_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wms_get_transport_layer_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("QMI returned error code %d", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->registered_ind_valid)
  {
    QBI_LOG_E_0("Missing registered indication TLV");
  }
  else
  {
    qbi_svc_sms_send_update_cache_ims(
      qmi_txn->ctx, qmi_rsp->registered_ind,
      qmi_rsp->transport_layer_info_valid, qmi_rsp->transport_layer_info);
  }

  return QBI_SVC_ACTION_ABORT;
} /* qbi_svc_sms_send_wms48_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_send_wms49_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_TRANSPORT_LAYER_INFO_IND by updating the
    current cached values

    @details
    This indication callback is not associated with a CID event: its purpose
    is to keep the cache updated so we can know whether outgoing messages
    should use IMS.

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_send_wms49_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  const wms_transport_layer_info_ind_msg_v01 *qmi_ind;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  qmi_ind = (const wms_transport_layer_info_ind_msg_v01 *) ind->buf->data;
  qbi_svc_sms_send_update_cache_ims(
    ind->txn->ctx, qmi_ind->registered_ind, qmi_ind->transport_layer_info_valid,
    qmi_ind->transport_layer_info);

  return QBI_SVC_ACTION_ABORT;
} /* qbi_svc_sms_send_wms49_ind_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_SMS_DELETE
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_sms_delete_s_build_qmi_req
===========================================================================*/
/*!
    @brief Builds a QMI request to delete SMS records from the device

    @details
    The parameter flag must already be verified to be in the valid range.

    @param txn
    @param flag Flag to delete (will be mapped to a QMI tag_type)
    @param index Logical index to delete; ignored unless flag is
    QBI_SVC_SMS_FLAG_INDEX
    @param message_mode Message mode of storage_type
    @param storage_type Storage type to delete from; this will be ignored
    if flag is QBI_SVC_SMS_FLAG_INDEX

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_delete_s_build_qmi_req
(
  qbi_txn_s                *txn,
  uint32                    flag,
  uint32                    index,
  wms_message_mode_enum_v01 message_mode,
  wms_storage_type_enum_v01 storage_type
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_QMI_REQ;
  wms_delete_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  qmi_req = (wms_delete_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_DELETE_REQ_V01,
                                  qbi_svc_sms_delete_s_wms24_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->storage_type = storage_type;
  qmi_req->message_mode_valid = TRUE;
  qmi_req->message_mode = message_mode;

  if (flag == QBI_SVC_SMS_FLAG_INDEX)
  {
    qmi_req->index_valid = TRUE;
    if (!qbi_svc_sms_mbim_index_to_qmi_location(
          txn->ctx, index, &qmi_req->index, &qmi_req->storage_type))
    {
      QBI_LOG_E_1("Couldn't map index %d to QMI location!", index);
      txn->status = QBI_MBIM_STATUS_INVALID_MEMORY_INDEX;
      action = QBI_SVC_ACTION_SEND_RSP;
    }
  }
  else if (flag != QBI_SVC_SMS_FLAG_ALL)
  {
    qmi_req->tag_type_valid = TRUE;
    qmi_req->tag_type = qbi_svc_sms_flag_to_qmi_tag_type(flag);
  }

  return action;
} /* qbi_svc_sms_delete_s_build_qmi_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_delete_s_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SMS_DELETE set request

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_delete_s_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
  qbi_svc_sms_delete_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->req.data);

  req = (qbi_svc_sms_delete_s_req_s *) txn->req.data;
  if (req->flag > QBI_SVC_SMS_FLAG_MAX)
  {
    QBI_LOG_E_1("Request to read unexpected flag %d", req->flag);
    txn->status = QBI_MBIM_STATUS_INVALID_PARAMETERS;
    action = QBI_SVC_ACTION_SEND_RSP;
  }
  else
  {
    /* Build the first QMI delete request to delete from NV. If it is
       successful, another request may be built to delete from UIM. */
    action = qbi_svc_sms_delete_s_build_qmi_req(
      txn, req->flag, req->message_index, WMS_MESSAGE_MODE_GW_V01,
      WMS_STORAGE_TYPE_NV_V01);
  }

  return action;
} /* qbi_svc_sms_delete_s_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_delete_s_wms24_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_DELETE_RESP for MBIM_CID_SMS_DELETE set

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_delete_s_wms24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_SEND_RSP;
  qbi_svc_sms_delete_s_req_s *req;
  wms_delete_req_msg_v01 *qmi_req;
  wms_delete_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  req = (qbi_svc_sms_delete_s_req_s *) qmi_txn->parent->req.data;
  qmi_req = (wms_delete_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wms_delete_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      req->flag == QBI_SVC_SMS_FLAG_ALL &&
      qmi_req->message_mode == WMS_MESSAGE_MODE_CDMA_V01)
  {
    /* When we are deleting from CDMA storage as part of a delete all request
       on a multimode build, ignore errors arising from the CDMA storage areas,
       since it is an optional, secondary storage area. */
    QBI_LOG_W_2("Deleting from CDMA storage %d failed with error %d; ignoring",
                qmi_req->storage_type, qmi_rsp->resp.error);
    qmi_rsp->resp.result = QMI_RESULT_SUCCESS_V01;
  }

  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01 &&
      !(qmi_rsp->resp.error == QMI_ERR_NO_ENTRY_V01 &&
        req->flag == QBI_SVC_SMS_FLAG_INDEX))
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_sms_get_failure_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else if (qmi_rsp->resp.error == QMI_ERR_NO_ENTRY_V01 &&
           req->flag == QBI_SVC_SMS_FLAG_INDEX)
  {
    QBI_LOG_W_2("No entry found at storage %d index %d",
                qmi_req->storage_type, qmi_req->index);
  }
  else if (req->flag != QBI_SVC_SMS_FLAG_INDEX &&
           qmi_req->storage_type == WMS_STORAGE_TYPE_NV_V01)
  {
    QBI_LOG_I_0("Deletion from NV successful; issuing request for UIM");

    action = qbi_svc_sms_delete_s_build_qmi_req(
      qmi_txn->parent, req->flag, 0, qmi_req->message_mode,
      WMS_STORAGE_TYPE_UIM_V01);
  }
  else if (req->flag == QBI_SVC_SMS_FLAG_ALL &&
           qmi_req->message_mode == WMS_MESSAGE_MODE_GW_V01 &&
           qbi_svc_bc_device_supports_3gpp2(qmi_txn->ctx))
  {
    /* It's possible for CDMA storage to fill up in a multimode device: for
       example, if all GW storage is full, we will not be able to convert
       incoming CDMA messages to GW. If the CDMA storage area fills up before
       room is made in the GW storage area, we could get stuck in a situation
       where we can't receive any new CDMA messages, even though the SMS storage
       that the user sees (only GW) is empty, and there would otherwise be no
       method to recover. To avoid this situation, we clear both GW and CDMA
       storage areas when receiving a request to delete all SMS messages. */
    QBI_LOG_I_0("Deletion of all messages from GW storage successful; issuing"
                "request for CDMA storage");

    action = qbi_svc_sms_delete_s_build_qmi_req(
      qmi_txn->parent, req->flag, 0, WMS_MESSAGE_MODE_CDMA_V01,
      WMS_STORAGE_TYPE_NV_V01);
  }

  if (action == QBI_SVC_ACTION_SEND_RSP)
  {
    qbi_svc_sms_status_update_flags_if_set(
      qmi_txn->ctx, QBI_SVC_SMS_STATUS_FLAG_MESSAGE_STORE_FULL);
  }

  return action;
} /* qbi_svc_sms_delete_s_wms24_rsp_cb() */

/*! @} */

/*! @addtogroup MBIM_CID_SMS_STATUS
    @{ */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_e_c2g_convert_cdma_msg
===========================================================================*/
/*!
    @brief Initiates processing of a new MT CDMA SMS when the primary message
    mode is GW.

    @details
    As the message can't be presented to the host in CDMA format, it is
    handled via the following procedure:
      1. Read new CDMA message out of storage
      2. Transcode CDMA PDU to GW PDU
      3. Save GW PDU to GW storage area
      4. Delete CDMA message
      5. Send MBIM_CID_SMS_STATUS event to host with GW storage information

    @param txn
    @param mt_msg

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_e_c2g_convert_cdma_msg
(
  qbi_txn_s                     *txn,
  const wms_mt_message_type_v01 *mt_msg
)
{
  wms_raw_read_req_msg_v01 *qmi_req;
  qbi_svc_sms_msg_location_s *info;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(mt_msg);

  /* Save the storage type + index to the info struct so we can delete the
     message after it's moved to GW. */
  txn->info = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_svc_sms_msg_location_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);
  info = (qbi_svc_sms_msg_location_s *) txn->info;
  info->index = mt_msg->storage_index;
  info->storage_type = mt_msg->storage_type;

  qmi_req = (wms_raw_read_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
    txn, QBI_QMI_SVC_WMS, QMI_WMS_RAW_READ_REQ_V01,
    qbi_svc_sms_status_e_c2g_wms22_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->message_memory_storage_identification.storage_type =
    mt_msg->storage_type;
  qmi_req->message_memory_storage_identification.storage_index =
    mt_msg->storage_index;

  qmi_req->message_mode_valid = TRUE;
  qmi_req->message_mode = WMS_MESSAGE_MODE_CDMA_V01;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_sms_status_e_c2g_convert_cdma_msg() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_e_c2g_delete_cdma_msg
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WMS_DELETE_REQ for the original
    CDMA message that has already been converted to GW format and saved

    @details

    @param txn
    @param gw_msg_storage_type
    @param gw_msg_storage_index

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_e_c2g_delete_cdma_msg
(
  qbi_txn_s                *txn,
  wms_storage_type_enum_v01 gw_msg_storage_type,
  uint32                    gw_msg_storage_index
)
{
  qbi_svc_sms_msg_location_s *info;
  wms_delete_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(txn->info);

  info = (qbi_svc_sms_msg_location_s *) txn->info;
  qmi_req = (wms_delete_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
    txn, QBI_QMI_SVC_WMS, QMI_WMS_DELETE_REQ_V01,
    qbi_svc_sms_status_e_c2g_wms24_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->message_mode_valid = TRUE;
  qmi_req->message_mode = WMS_MESSAGE_MODE_CDMA_V01;
  qmi_req->storage_type = info->storage_type;
  qmi_req->index_valid = TRUE;
  qmi_req->index = info->index;

  /* After the deletion is complete, we need to send an event to the host with
     the location in GW storage where the new message is, so update the info
     structure to save it until we get the delete response. */
  info->storage_type = gw_msg_storage_type;
  info->index = gw_msg_storage_index;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_sms_status_e_c2g_delete_cdma_msg() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_e_c2g_failover_write_to_sim
===========================================================================*/
/*!
    @brief Allocates a new QMI_WMS_RAW_WRITE_REQ, and populates it with the
    same data as the provided request, except changes the storage type from
    NV to SIM

    @details

    @param txn
    @param qmi_req_nv

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_e_c2g_failover_write_to_sim
(
  qbi_txn_s                       *txn,
  const wms_raw_write_req_msg_v01 *qmi_req_nv
)
{
  wms_raw_write_req_msg_v01 *qmi_req_sim;
/*-------------------------------------------------------------------------*/
  qmi_req_sim = (wms_raw_write_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
    txn, QBI_QMI_SVC_WMS, QMI_WMS_RAW_WRITE_REQ_V01,
    qbi_svc_sms_status_e_c2g_wms21_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req_sim);

  QBI_MEMSCPY(qmi_req_sim, sizeof(wms_raw_write_req_msg_v01),
              qmi_req_nv, sizeof(wms_raw_write_req_msg_v01));
  qmi_req_sim->raw_message_write_data.storage_type = WMS_STORAGE_TYPE_UIM_V01;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_sms_status_e_c2g_failover_write_to_sim() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_e_c2g_wms21_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_RAW_WRITE_RESP for writing the transcoded GW
    SMS to storage.

    @details
    If the write is successful, initiates step 4: deletion of the original
    CDMA message.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_e_c2g_wms21_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wms_raw_write_req_msg_v01 *qmi_req;
  wms_raw_write_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_req = (wms_raw_write_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wms_raw_write_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);

    if (qmi_rsp->resp.error == QMI_ERR_DEVICE_STORAGE_FULL_V01 &&
        qmi_req->raw_message_write_data.storage_type == WMS_STORAGE_TYPE_NV_V01)
    {
      QBI_LOG_W_0("NV storage full: trying SIM");
      action = qbi_svc_sms_status_e_c2g_failover_write_to_sim(
        qmi_txn->parent, qmi_req);
    }
  }
  else
  {
    action = qbi_svc_sms_status_e_c2g_delete_cdma_msg(
      qmi_txn->parent, qmi_req->raw_message_write_data.storage_type,
      qmi_rsp->storage_index);
  }

  return action;
} /* qbi_svc_sms_status_e_c2g_wms21_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_e_c2g_wms22_rsp_cb
===========================================================================*/
/*!
    @brief Processes a QMI_WMS_RAW_READ_RESP of a new CDMA message.
    Transcodes it to GW format, then issues a QMI request to store the new
    message.

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_e_c2g_wms22_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  wms_raw_read_resp_msg_v01 *qmi_rsp;
  wms_raw_write_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  qmi_rsp = (wms_raw_read_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Couldn't read new SMS: error code %d", qmi_rsp->resp.error);
  }
  else
  {
    qmi_req = (wms_raw_write_req_msg_v01 *) qbi_qmi_txn_alloc_ret_req_buf(
      qmi_txn->parent, QBI_QMI_SVC_WMS, QMI_WMS_RAW_WRITE_REQ_V01,
      qbi_svc_sms_status_e_c2g_wms21_rsp_cb);
    QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

    qmi_req->raw_message_write_data.format = WMS_MESSAGE_FORMAT_GW_PP_V01;
    qmi_req->raw_message_write_data.storage_type =
      qbi_svc_sms_configuration_get_preferred_storage_type(qmi_txn->ctx);
    qmi_req->raw_message_write_data.raw_message_len =
      qbi_svc_sms_pdu_cdma_to_gw(
        qmi_rsp->raw_message_data.data, qmi_rsp->raw_message_data.data_len,
        qmi_req->raw_message_write_data.raw_message,
        sizeof(qmi_req->raw_message_write_data.raw_message));

    if (qmi_req->raw_message_write_data.raw_message_len == 0)
    {
      QBI_LOG_E_0("Couldn't transcode stored CDMA SMS to GW!");
    }
    else
    {
      action = QBI_SVC_ACTION_SEND_QMI_REQ;
    }
  }

  return action;
} /* qbi_svc_sms_status_e_c2g_wms22_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_e_c2g_wms24_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_DELETE_RESP for deleting the original CDMA
    message from storage. Results in sending a MBIM_CID_SMS_STATUS event
    to the host.

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_e_c2g_wms24_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_delete_req_msg_v01 *qmi_req;
  wms_delete_resp_msg_v01 *qmi_rsp;
  qbi_svc_sms_msg_location_s *info;
  qbi_svc_sms_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent->info);

  cache = qbi_svc_sms_get_cache(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  info = (qbi_svc_sms_msg_location_s *) qmi_txn->parent->info;
  qmi_req = (wms_delete_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wms_delete_resp_msg_v01 *) qmi_txn->rsp.data;
  /* A failure here doesn't stop us from sending the event to the host, so
     just log it. */
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Couldn't delete message: error %d", qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_4("Successfully converted MT SMS from CDMA storage type %d index "
                "%d to GW storage type %d index %d", qmi_req->storage_type,
                qmi_req->index, info->storage_type, info->index);
  }

  /* Send the indication to the host. */
  cache->status.flag |= QBI_SVC_SMS_STATUS_FLAG_NEW_MESSAGE;
  cache->status.message_index = qbi_svc_sms_qmi_location_to_mbim_index(
    qmi_txn->ctx, info->storage_type, info->index);
  return qbi_svc_sms_status_eq_build_rsp_from_cache(qmi_txn->parent);
} /* qbi_svc_sms_status_e_c2g_wms24_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_e_wms01_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_EVENT_REPORT_IND, looking for a new MT message
    which would trigger a MBIM_CID_SMS_STATUS event

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_e_wms01_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  const wms_event_report_ind_msg_v01 *qmi_ind;
  qbi_svc_sms_cache_s *cache;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  cache = qbi_svc_sms_get_cache(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_ind = (wms_event_report_ind_msg_v01 *) ind->buf->data;
  if (qmi_ind->mt_message_valid && qmi_ind->message_mode_valid)
  {
    if (qmi_ind->mt_message.storage_type != WMS_STORAGE_TYPE_UIM_V01 &&
        qmi_ind->mt_message.storage_type != WMS_STORAGE_TYPE_NV_V01)
    {
      QBI_LOG_W_1("Received MT SMS on unexpected storage type %d",
                  qmi_ind->mt_message.storage_type);
    }
    else
    {
      QBI_LOG_I_3("Received new MT SMS on message mode %d storage type %d at "
                  "index %d", qmi_ind->message_mode,
                  qmi_ind->mt_message.storage_type,
                  qmi_ind->mt_message.storage_index);

      /* MT SMS may still arrive on NV storage in UIM storage only
         configuration if NV store is used in modem SMS route
         configuration. */
      if (qmi_ind->mt_message.storage_type == WMS_STORAGE_TYPE_NV_V01 &&
          qbi_svc_sms_configuration_get_preferred_storage_type(ind->txn->ctx) ==
            WMS_STORAGE_TYPE_UIM_V01)
      {
        QBI_LOG_E_0("Received MT SMS on NV in UIM storage only configuration");
      }

      if (qmi_ind->message_mode == WMS_MESSAGE_MODE_CDMA_V01)
      {
        QBI_LOG_I_0("Need to transcode and move message from CDMA to GW "
                    "storage");
        action = qbi_svc_sms_status_e_c2g_convert_cdma_msg(
          ind->txn, &qmi_ind->mt_message);
      }
      else
      {
        /* Send an indication, even if the flag is already set. */
        cache->status.flag |= QBI_SVC_SMS_STATUS_FLAG_NEW_MESSAGE;
        cache->status.message_index = qbi_svc_sms_qmi_location_to_mbim_index(
          ind->txn->ctx, qmi_ind->mt_message.storage_type,
          qmi_ind->mt_message.storage_index);
        action = qbi_svc_sms_status_eq_build_rsp_from_cache(ind->txn);
      }
    }
  }

  return action;
} /* qbi_svc_sms_status_e_wms01_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_e_wms36_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_GET_STORE_MAX_SIZE_RESP for a
    MBIM_CID_SMS_STATUS event

    @details
    This QMI message is queried when we receive a storage full indication.
    Its purpose is to check if the alternate storage (the one we didn't just
    receive a full indication for) has space available. If it is also full,
    then we set the storage full flag and send an event.

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_e_wms36_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_get_store_max_size_resp_msg_v01 *qmi_rsp;
  qbi_svc_sms_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  boolean send_event = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_sms_get_cache(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_rsp = (wms_get_store_max_size_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->free_slots_valid)
  {
    QBI_LOG_E_0("No free slots TLV in response! Assuming full...");
  }
  else if (qmi_rsp->free_slots > 0)
  {
    QBI_LOG_I_1("%d free slots available in alternate storage",
                qmi_rsp->free_slots);
    send_event = FALSE;
  }

  if (send_event)
  {
    QBI_LOG_I_0("Sending message store full event");
    cache->status.flag |= QBI_SVC_SMS_STATUS_FLAG_MESSAGE_STORE_FULL;
    action = qbi_svc_sms_status_eq_build_rsp_from_cache(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_sms_status_e_wms36_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_e_wms3f_ind_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_MEMORY_FULL_IND, looking to send a
    MBIM_CID_SMS_STATUS event indicating that the memory store is full

    @details

    @param ind

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_e_wms3f_ind_cb
(
  const qbi_svc_qmi_ind_data_s *ind
)
{
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  qbi_svc_sms_cache_s *cache;
  const wms_memory_full_ind_msg_v01 *qmi_ind;
  wms_storage_type_enum_v01 failover_storage;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(ind);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf);
  QBI_CHECK_NULL_PTR_RET_ABORT(ind->buf->data);

  cache = qbi_svc_sms_get_cache(ind->txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_ind = (const wms_memory_full_ind_msg_v01 *) ind->buf->data;
  QBI_LOG_I_2("Received memory full indication for message mode %d storage "
              "type %d", qmi_ind->memory_full_info.message_mode,
              qmi_ind->memory_full_info.storage_type);
  if (qmi_ind->memory_full_info.storage_type != WMS_STORAGE_TYPE_UIM_V01 &&
      qmi_ind->memory_full_info.storage_type != WMS_STORAGE_TYPE_NV_V01)
  {
    QBI_LOG_E_1("Received memory full indication for unsupported storage "
                "type %d", qmi_ind->memory_full_info.storage_type);
  }
  else if (cache->status.flag & QBI_SVC_SMS_STATUS_FLAG_MESSAGE_STORE_FULL)
  {
    QBI_LOG_I_0("Flag already set for message store full");
    QBI_LOG_D_0("Storage memory is full sending indication");
    action = qbi_svc_sms_status_eq_build_rsp_from_cache(ind->txn);
  }
  else
  {
    /* For UIM storage only configuration, report memory full once UIM
       storage is full. For multiple storage configuration if one storage
       is full, check to see if the other one still has space. */
    if (qmi_ind->memory_full_info.storage_type == WMS_STORAGE_TYPE_UIM_V01 &&
        qbi_svc_sms_configuration_get_preferred_storage_type(ind->txn->ctx) ==
          WMS_STORAGE_TYPE_UIM_V01)
    {
      QBI_LOG_I_0("Sending message store full event");
      cache->status.flag |= QBI_SVC_SMS_STATUS_FLAG_MESSAGE_STORE_FULL;
      action = qbi_svc_sms_status_eq_build_rsp_from_cache(ind->txn);
    }
    else
    {
      if (qmi_ind->memory_full_info.storage_type == WMS_STORAGE_TYPE_NV_V01)
      {
        failover_storage = WMS_STORAGE_TYPE_UIM_V01;
      }
      else
      {
        failover_storage = WMS_STORAGE_TYPE_NV_V01;
      }
      action = qbi_svc_sms_status_eq_build_wms36_req(
        ind->txn, failover_storage, qbi_svc_sms_status_e_wms36_rsp_cb);
    }
  }

  return action;
} /* qbi_svc_sms_status_e_wms3f_ind_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_eq_build_rsp_from_cache
===========================================================================*/
/*!
    @brief Allocates and populates a MBIM_CID_SMS_STATUS query response/event
    based on the information in the cache

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_eq_build_rsp_from_cache
(
  qbi_txn_s *txn
)
{
  qbi_svc_sms_status_rsp_s *rsp;
  qbi_svc_sms_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_sms_get_cache(txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  rsp = (qbi_svc_sms_status_rsp_s *)
    qbi_txn_alloc_rsp_buf(txn, sizeof(qbi_svc_sms_status_rsp_s));
  QBI_CHECK_NULL_PTR_RET_ABORT(rsp);

  rsp->flag = cache->status.flag;
  rsp->message_index = cache->status.message_index;

  return QBI_SVC_ACTION_SEND_RSP;
} /* qbi_svc_sms_status_eq_build_rsp_from_cache() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_eq_build_wms36_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WMS_GET_STORE_MAX_SIZE_REQ for
    the given storage_type

    @details

    @param txn
    @param storage_type

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_eq_build_wms36_req
(
  qbi_txn_s                *txn,
  wms_storage_type_enum_v01 storage_type,
  qbi_svc_qmi_rsp_cb_f     *rsp_cb
)
{
  wms_get_store_max_size_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* QMI_WMS_GET_STORE_MAX_SIZE (0x36) - See if the given storage is full */
  qmi_req = (wms_get_store_max_size_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_GET_STORE_MAX_SIZE_REQ_V01, rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->message_mode_valid = TRUE;
  qmi_req->message_mode = WMS_MESSAGE_MODE_GW_V01;
  qmi_req->storage_type = storage_type;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_sms_status_eq_build_wms36_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_q_build_wms31_req
===========================================================================*/
/*!
    @brief Allocates and populates a QMI_WMS_LIST_MESSAGES_REQ to check for
    MT unread messages on the given storage type

    @details

    @param txn
    @param storage_type

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_q_build_wms31_req
(
  qbi_txn_s                *txn,
  wms_storage_type_enum_v01 storage_type
)
{
  wms_list_messages_req_msg_v01 *qmi_req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  /* QMI_WMS_LIST_MESSAGES (0x31) - See if there are any unread messages in the
     given storage type */
  qmi_req = (wms_list_messages_req_msg_v01 *)
    qbi_qmi_txn_alloc_ret_req_buf(txn, QBI_QMI_SVC_WMS,
                                  QMI_WMS_LIST_MESSAGES_REQ_V01,
                                  qbi_svc_sms_status_q_wms31_rsp_cb);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_req);

  qmi_req->storage_type = storage_type;
  qmi_req->tag_type_valid = TRUE;
  qmi_req->tag_type = WMS_TAG_TYPE_MT_NOT_READ_V01;
  qmi_req->message_mode_valid = TRUE;
  qmi_req->message_mode = WMS_MESSAGE_MODE_GW_V01;

  return QBI_SVC_ACTION_SEND_QMI_REQ;
} /* qbi_svc_sms_status_q_build_wms31_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_q_req
===========================================================================*/
/*!
    @brief Handles a MBIM_CID_SMS_STATUS query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_q_req
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
  wms_storage_type_enum_v01 storage_pref;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  storage_pref = qbi_svc_sms_configuration_get_preferred_storage_type(txn->ctx);

  action = qbi_svc_sms_status_q_build_wms31_req(txn, storage_pref);
  if (action != QBI_SVC_ACTION_ABORT)
  {
    action = qbi_svc_sms_status_eq_build_wms36_req(
      txn, storage_pref, qbi_svc_sms_status_q_wms36_rsp_cb);
  }

  return action;
} /* qbi_svc_sms_status_q_req() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_q_rsp
===========================================================================*/
/*!
    @brief Determines whether we have received enough information to return
    the response to a MBIM_CID_SMS_STATUS query

    @details

    @param txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_q_rsp
(
  qbi_txn_s *txn
)
{
  qbi_svc_action_e action;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(txn);

  if (txn->qmi_txns_pending == 0)
  {
    action = qbi_svc_sms_status_eq_build_rsp_from_cache(txn);
  }
  else
  {
    action = QBI_SVC_ACTION_WAIT_ASYNC_RSP;
  }

  return action;
} /* qbi_svc_sms_status_q_rsp() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_q_wms31_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_LIST_MESSAGES_RESP for MBIM_CID_SMS_STATUS query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_q_wms31_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_list_messages_req_msg_v01 *qmi_req;
  wms_list_messages_resp_msg_v01 *qmi_rsp;
  qbi_svc_sms_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  boolean new_query = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_sms_get_cache(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_req = (wms_list_messages_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wms_list_messages_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_sms_get_failure_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else
  {
    QBI_LOG_I_1("QMI reports %d unread messages", qmi_rsp->message_tuple_len);
    if (qmi_rsp->message_tuple_len > 0)
    {
      /* If the cache isn't already indicating a specific message index, then
         just use the first one. But don't change a cached message index, so
         we can be consistent with what was reported by the last event. */
      if (cache->status.flag & QBI_SVC_SMS_STATUS_FLAG_NEW_MESSAGE)
      {
        QBI_LOG_I_1("Cache already indicates message waiting at index %d",
                    cache->status.message_index);
      }
      else
      {
        cache->status.flag |= QBI_SVC_SMS_STATUS_FLAG_NEW_MESSAGE;
        cache->status.message_index = qbi_svc_sms_qmi_location_to_mbim_index(
          qmi_txn->ctx, qmi_req->storage_type,
          qmi_rsp->message_tuple[0].message_index);
        QBI_LOG_I_1("New message waiting at index %d",
                    cache->status.message_index);
      }
    }
    else
    {
      if (qmi_req->storage_type == WMS_STORAGE_TYPE_NV_V01)
      {
        QBI_LOG_I_0("No unread messages in NV; trying UIM");
        action = qbi_svc_sms_status_q_build_wms31_req(
          qmi_txn->parent, WMS_STORAGE_TYPE_UIM_V01);
        new_query = TRUE;
      }
      else
      {
        QBI_LOG_I_0("No unread messages in NV or UIM");
        cache->status.flag &= ~(QBI_SVC_SMS_STATUS_FLAG_NEW_MESSAGE);
        cache->status.message_index = 0;
      }
    }
  }

  if (!new_query)
  {
    action = qbi_svc_sms_status_q_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_sms_status_q_wms31_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_q_wms36_rsp_cb
===========================================================================*/
/*!
    @brief Handles a QMI_WMS_GET_STORE_MAX_SIZE_RESP for MBIM_CID_SMS_STATUS
    query

    @details

    @param qmi_txn

    @return qbi_svc_action_e
*/
/*=========================================================================*/
static qbi_svc_action_e qbi_svc_sms_status_q_wms36_rsp_cb
(
  qbi_qmi_txn_s *qmi_txn
)
{
  wms_get_store_max_size_req_msg_v01 *qmi_req;
  wms_get_store_max_size_resp_msg_v01 *qmi_rsp;
  qbi_svc_sms_cache_s *cache;
  qbi_svc_action_e action = QBI_SVC_ACTION_ABORT;
  boolean new_query = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->parent);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->req.data);
  QBI_CHECK_NULL_PTR_RET_ABORT(qmi_txn->rsp.data);

  cache = qbi_svc_sms_get_cache(qmi_txn->ctx);
  QBI_CHECK_NULL_PTR_RET_ABORT(cache);

  qmi_req = (wms_get_store_max_size_req_msg_v01 *) qmi_txn->req.data;
  qmi_rsp = (wms_get_store_max_size_resp_msg_v01 *) qmi_txn->rsp.data;
  if (qmi_rsp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QBI_LOG_E_1("Received error code %d from QMI", qmi_rsp->resp.error);
    qbi_svc_sms_get_failure_status(qmi_txn->parent, qmi_rsp->resp.error);
  }
  else if (!qmi_rsp->free_slots_valid)
  {
    QBI_LOG_E_0("No free slots TLV in response - can't tell if memory full!");
  }
  else
  {
    QBI_LOG_I_3("QMI reports %d/%d memory slots are free in storage type %d",
                qmi_rsp->free_slots, qmi_rsp->mem_store_max_size,
                qmi_req->storage_type);

    if (qmi_rsp->free_slots == 0)
    {
      if (qmi_req->storage_type == WMS_STORAGE_TYPE_NV_V01)
      {
        QBI_LOG_I_0("NV storage is full - checking UIM");
        action = qbi_svc_sms_status_eq_build_wms36_req(
          qmi_txn->parent, WMS_STORAGE_TYPE_UIM_V01,
          qbi_svc_sms_status_q_wms36_rsp_cb);
        new_query = TRUE;
      }
      else
      {
        QBI_LOG_W_0("Both NV and UIM storage full!");
        cache->status.flag |= QBI_SVC_SMS_STATUS_FLAG_MESSAGE_STORE_FULL;
      }
    }
    else
    {
      cache->status.flag &= ~(QBI_SVC_SMS_STATUS_FLAG_MESSAGE_STORE_FULL);
    }
  }

  if (!new_query)
  {
    action = qbi_svc_sms_status_q_rsp(qmi_txn->parent);
  }

  return action;
} /* qbi_svc_sms_status_q_wms36_rsp_cb() */

/*===========================================================================
  FUNCTION: qbi_svc_sms_status_update_flags_deleted
===========================================================================*/
/*!
    @brief If the given status flag is set, performs an internal query to
    see whether it should be unset

    @details

    @param ctx
    @param flag Only perform the update if this flag is set in the cache
*/
/*=========================================================================*/
static void qbi_svc_sms_status_update_flags_if_set
(
  qbi_ctx_s *ctx,
  uint32     flag
)
{
  qbi_txn_s *txn;
  qbi_svc_sms_cache_s *cache;
/*-------------------------------------------------------------------------*/
  cache = qbi_svc_sms_get_cache(ctx);
  QBI_CHECK_NULL_PTR_RET(cache);

  if (cache->status.flag & flag)
  {
    /* Note that we perform an internal query rather than sending an event, as
       MBIM explicitly states that we should not send an event when a flag is
       unset */
    txn = qbi_txn_alloc(ctx, QBI_SVC_ID_SMS, 0, QBI_TXN_CMD_TYPE_INTERNAL,
                        QBI_SVC_SMS_MBIM_CID_SMS_STATUS, 0, NULL);
    QBI_CHECK_NULL_PTR_RET(txn);

    (void) qbi_svc_proc_action(txn, qbi_svc_sms_status_q_req(txn));
  }
} /* qbi_svc_sms_status_update_flags_if_set() */

/*! @} */


/*=============================================================================

  Public Function Definitions

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_svc_sms_init
===========================================================================*/
/*!
    @brief One-time initialization of the SMS device service

    @details
*/
/*=========================================================================*/
void qbi_svc_sms_init
(
  void
)
{
  static const qbi_svc_cfg_s qbi_svc_sms_cfg = {
    {
      0x53, 0x3f, 0xbe, 0xeb, 0x14, 0xfe, 0x44, 0x67,
      0x9f, 0x90, 0x33, 0xa2, 0x23, 0xe5, 0x6c, 0x3f
    },
    QBI_SVC_ID_SMS,
    FALSE,
    qbi_svc_sms_cmd_hdlr_tbl,
    ARR_SIZE(qbi_svc_sms_cmd_hdlr_tbl),
    qbi_svc_sms_open,
    NULL
  };
/*-------------------------------------------------------------------------*/
  qbi_svc_reg(&qbi_svc_sms_cfg);
} /* qbi_svc_sms_init() */

