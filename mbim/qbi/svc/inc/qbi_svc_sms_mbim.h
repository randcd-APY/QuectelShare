/*!
  @file
  qbi_svc_sms_mbim.h

  @brief
  SMS device service definitions provided by the MBIM specification, e.g.
  InformationBuffer structures, enums, etc.
*/

/*=============================================================================

  Copyright (c) 2011 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
10/28/11  bd   Updated to MBIM v1.0 SC
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_SVC_SMS_MBIM_H
#define QBI_SVC_SMS_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of Basic Connectivity device service CIDs
*/
typedef enum {
  QBI_SVC_SMS_CID_MIN = 0,

  QBI_SVC_SMS_MBIM_CID_SMS_CONFIGURATION = 1,
  QBI_SVC_SMS_MBIM_CID_SMS_READ          = 2,
  QBI_SVC_SMS_MBIM_CID_SMS_SEND          = 3,
  QBI_SVC_SMS_MBIM_CID_SMS_DELETE        = 4,
  QBI_SVC_SMS_MBIM_CID_SMS_STATUS        = 5,

  QBI_SVC_SMS_CID_MAX
} qbi_svc_sms_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_SMS_CONFIGURATION
    @{ */

/* MBIM_SMS_STORAGE_STATE */
#define QBI_SVC_SMS_STORAGE_STATE_NOT_INIT    (0)
#define QBI_SVC_SMS_STORAGE_STATE_INITIALIZED (1)

/* MBIM_SMS_FORMAT */
#define QBI_SVC_SMS_FORMAT_PDU  (0)
#define QBI_SVC_SMS_FORMAT_CDMA (1)

/* Maximum sizes of the variable length fields */
#define QBI_SVC_SMS_SC_ADDR_MAX_LEN_BYTES (40)

/* MBIM_SET_SMS_CONFIGURATION */
typedef PACK(struct) {
  uint32 format;
  qbi_mbim_offset_size_pair_s sc_address;
  /*! @note Followed by DataBuffer containing ScAddress */
} qbi_svc_sms_configuration_s_req_s;

/* MBIM_SMS_CONFIGURATION */
typedef PACK(struct) {
  uint32 sms_storage_state;
  uint32 format;
  uint32 max_messages;
  uint32 cdma_sms_size;
  qbi_mbim_offset_size_pair_s sc_address;
  /*! @note Followed by DataBuffer containing ScAddress */
} qbi_svc_sms_configuration_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_SMS_READ
    @{ */

/* MBIM_SMS_FLAG */
#define QBI_SVC_SMS_FLAG_ALL   (0)
#define QBI_SVC_SMS_FLAG_INDEX (1)
#define QBI_SVC_SMS_FLAG_NEW   (2)
#define QBI_SVC_SMS_FLAG_OLD   (3)
#define QBI_SVC_SMS_FLAG_SENT  (4)
#define QBI_SVC_SMS_FLAG_DRAFT (5)
#define QBI_SVC_SMS_FLAG_MAX   (5)

/* SMS_MESSAGE_STATUS */
#define QBI_SVC_SMS_STATUS_NEW   (0)
#define QBI_SVC_SMS_STATUS_OLD   (1)
#define QBI_SVC_SMS_STATUS_DRAFT (2)
#define QBI_SVC_SMS_STATUS_SENT  (3)

/* Record index used for broadcast SMS */
#define QBI_SVC_SMS_INDEX_NONE   (0)

/* Array sizes */
#define QBI_SVC_SMS_PDU_DATA_LEN_BYTES (255)

/* MBIM_SMS_READ_REQ */
typedef PACK(struct) {
  uint32 format;
  uint32 flag;
  uint32 message_index;
} qbi_svc_sms_read_q_req_s;

/* SMS_PDU_RECORD */
typedef PACK(struct) {
  uint32 message_index;
  uint32 message_status;
  qbi_mbim_offset_size_pair_s pdu_data;
  /*! @note Followed by DataBuffer containing PduData */
} qbi_svc_sms_pdu_record_s;

/* MBIM_SMS_RECEIVE */
typedef PACK(struct) {
  uint32 format;
  uint32 element_count;
  /*! @note Followed by element_count instances of qbi_mbim_offset_size_pair_s,
      then element_count qbi_svc_sms_pdu_record_s */
} qbi_svc_sms_read_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_SMS_SEND
    @{ */

/* SMS_SEND_PDU */
typedef PACK(struct) {
  qbi_mbim_offset_size_pair_s pdu_data;
  /*! @note Followed by DataBuffer containing PduData */
} qbi_svc_sms_send_pdu_s;

/* MBIM_SMS_SEND */
typedef PACK(struct) {
  uint32 format;
  /*! @note Followed by SMS_SEND_PDU or SMS_SEND_CDMA depending on format */
} qbi_svc_sms_send_s_req_s;

/* MBIM_SMS_SEND_STATUS */
typedef PACK(struct) {
  uint32 message_ref;
} qbi_svc_sms_send_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_SMS_DELETE
    @{ */

/* MBIM_SMS_DELETE */
typedef PACK(struct) {
  uint32 flag;
  uint32 message_index;
} qbi_svc_sms_delete_s_req_s;

/*! @} */

/*! @addtogroup MBIM_CID_SMS_STATUS
    @{ */

/* SMS_STATUS_FLAGS */
#define QBI_SVC_SMS_STATUS_FLAG_NONE               (0x00000000)
#define QBI_SVC_SMS_STATUS_FLAG_MESSAGE_STORE_FULL (0x00000001)
#define QBI_SVC_SMS_STATUS_FLAG_NEW_MESSAGE        (0x00000002)

/* MBIM_SMS_STATUS */
typedef PACK(struct) {
  uint32 flag;
  uint32 message_index;
} qbi_svc_sms_status_rsp_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_SMS_MBIM_H */

