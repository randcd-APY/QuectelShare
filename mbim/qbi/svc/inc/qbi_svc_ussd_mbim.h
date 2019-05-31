/*!
  @file
  qbi_svc_ussd_mbim.h

  @brief
  USSD device service definitions provided by the MBIM specification, e.g.
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
09/09/11  hz   Updated to MBIM v0.81c
07/28/11  hz   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_SVC_USSD_MBIM_H
#define QBI_SVC_USSD_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of USSD device service CIDs
*/
typedef enum {
  QBI_SVC_USSD_CID_MIN = 0,

  QBI_SVC_USSD_MBIM_CID_USSD  = 1,

  QBI_SVC_USSD_CID_MAX
} qbi_svc_ussd_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_USSD
    @{ */

/* MBIM_USSD_ACTION */
#define QBI_SVC_USSD_ACTION_INITIATE                   (0)
#define QBI_SVC_USSD_ACTION_CONTINUE                   (1)
#define QBI_SVC_USSD_ACTION_CANCEL                     (2)

/* MBIM_USSD_RESPONSE */
#define QBI_SVC_USSD_RESPONSE_NO_ACTION_REQUIRED       (0)
#define QBI_SVC_USSD_RESPONSE_ACTION_REQUIRED          (1)
#define QBI_SVC_USSD_RESPONSE_TERMINATED_BY_NW         (2)
#define QBI_SVC_USSD_RESPONSE_OTHER_LOCAL_CLIENT       (3)
#define QBI_SVC_USSD_RESPONSE_OPERATION_NOT_SUPPORTED  (4)
#define QBI_SVC_USSD_RESPONSE_NETWORK_TIMEOUT          (5)

/* MBIM_USSD_SESSION_STATE */
#define QBI_SVC_USSD_SESSION_STATE_NEW                 (0)
#define QBI_SVC_USSD_SESSION_STATE_EXISTING            (1)

/* Data Coding Scheme defined in 3GPP TS 23.038 Section 5 */
#define QBI_SVC_USSD_3GPP_DCS_GSM_7BIT                 (0x0F)
#define QBI_SVC_USSD_3GPP_DCS_8BIT                     (0x44)
#define QBI_SVC_USSD_3GPP_DCS_UCS2                     (0x48)

/* Array Sizes */
#define QBI_SVC_USSD_DATA_LEN_MAX                      (160)

/* MBIM_SET_USSD request */
typedef PACK(struct){
  uint32 ussd_action;                             /*!< MBIM_USSD_ACTION */
  uint32 ussd_data_coding_scheme;                 /*!< Defined in 3GPP 23.038 */
  qbi_mbim_offset_size_pair_s ussd_payload;
  /*! @note Followed by DataBuffer containing USSDPayload */
} qbi_svc_ussd_s_req_s;

/* MBIM_USSD response */
typedef PACK(struct){
  uint32 ussd_response;                           /*!< MBIM_USSD_RESPONSE */
  uint32 ussd_session_state;                      /*!< MBIM_USSD_SESSION_STATE */
  uint32 ussd_data_coding_scheme;                 /*!< Defined in 3GPP 23.038*/
  qbi_mbim_offset_size_pair_s ussd_payload;
  /*! @note Followed by DataBuffer containing USSDPayload */
} qbi_svc_ussd_rsp_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_USSD_MBIM_H */

