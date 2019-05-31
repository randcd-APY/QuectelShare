/*!
  @file
  qbi_svc_stk_mbim.h

  @brief
  STK device service definitions provided by the MBIM specification, e.g.
  InformationBuffer structures, enums, etc.
*/

/*=============================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
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

#ifndef QBI_SVC_STK_MBIM_H
#define QBI_SVC_STK_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of STK device service CIDs
*/
typedef enum {
  QBI_SVC_STK_CID_MIN = 0,

  QBI_SVC_STK_MBIM_CID_STK_PAC                = 1,
  QBI_SVC_STK_MBIM_CID_STK_TERMINAL_RESPONSE  = 2,
  QBI_SVC_STK_MBIM_CID_STK_ENVELOPE           = 3,

  QBI_SVC_STK_CID_MAX
} qbi_svc_stk_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_STK_PAC
    @{ */

/* MBIM_STK_PAC_PROFILE */
#define QBI_SVC_STK_NOT_HANDLED_BY_FUNCTION_CANNOT_BE_HANDLED_BY_HOST     (0)
#define QBI_SVC_STK_NOT_HANDLED_BY_FUNCTION_MAY_BE_HANDLED_BY_HOST        (1)
#define QBI_SVC_STK_HANDLED_BY_FUNCTION_ONLY_TRANSPARENT_TO_HOST          (2)
#define QBI_SVC_STK_HANDLED_BY_FUNCTION_NOTIFICATION_TO_HOST_POSSIBLE     (3)
#define QBI_SVC_STK_HANDLED_BY_FUNCTION_NOTIFICATION_TO_HOST_ENABLED      (4)
#define QBI_SVC_STK_HANDLED_BY_FUNCTION_CAN_BE_OVERRIDDEN_BY_HOST         (5)
#define QBI_SVC_STK_HANDLED_BY_HOST_FUNCTION_NOT_ABLE_TO_HANDLE           (6)
#define QBI_SVC_STK_HANDLED_BY_HOST_FUNCTION_ABLE_TO_HANDLE               (7)

/* MBIM_STK_PAC_TYPE */
#define QBI_SVC_STK_PROACTIVE_COMMAND       (0)
#define QBI_SVC_STK_NOTIFICATION            (1)

/* Array Sizes */
#define QBI_SVC_STK_PAC_HOST_CONTROL_LEN           (32)
#define QBI_SVC_STK_PAC_INFO_LEN                   (256)

/* MBIM_SET_STK_PAC request */
typedef PACK(struct){
  uint8 pac_host_control[QBI_SVC_STK_PAC_HOST_CONTROL_LEN];
} qbi_svc_stk_pac_s_req_s;

/* MBIM_STK_PAC_INFO response */
typedef PACK(struct){
  uint8 pac_profile[QBI_SVC_STK_PAC_INFO_LEN];
} qbi_svc_stk_pac_info_rsp_s;

/* MBIM_STK_PAC notification */
typedef PACK(struct){
  uint32 pac_type;
  /*! @note Followed by a DataBuffer containing proactive command
      in BER-TLV format */
} qbi_svc_stk_pac_event_s;
/*! @} */

/*! @addtogroup MBIM_CID_STK_TERMINAL_RESPONSE
    @{ */

/* MBIM_CID_STK_TERMINAL_RESPONSE request */
typedef PACK(struct){
  uint32 response_length;
  /*! @note Followed by DataBuffer containing raw terminal response data */
  /*! @todo Place holder for variable length data, to be removed. DataBuffer
      Pointer can be directly located based on offset */
  uint8 data_buffer[1];
} qbi_svc_stk_terminal_response_s_req_s;

/* MBIM_STK_TERMINAL_RESPONSE_INFO
response */
typedef PACK(struct){
  qbi_mbim_offset_size_pair_s result_data_string;
  uint32 status_words;
  /*! @note Followed by a DataBuffer containing APDU response data string */
} qbi_svc_stk_terminal_response_info_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_STK_ENVELOPE
    @{ */

/* Array Sizes */
#define QBI_SVC_STK_ENVELOPE_SUPPORT_LEN               (32)

/* MBIM_SET_STK_ENVELOPE request */
typedef PACK(struct){
  /*! @note DataBuffer containing envelope command in BER-TLV format */
  /*! @todo Place holder for variable length data, to be removed. DataBuffer
      Pointer can be directly located at the beginning of the request data */
  uint8 data_buffer[1];
} qbi_svc_stk_envelope_s_req_s;

/* MBIM_STK_ENVELOPE_INFO
response */
typedef PACK(struct){
  uint8 envelope_support[QBI_SVC_STK_ENVELOPE_SUPPORT_LEN];
} qbi_svc_stk_envelope_info_rsp_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_STK_MBIM_H */

