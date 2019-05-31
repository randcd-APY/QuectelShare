/*!
  @file
  qbi_svc_qdu_mbim.h

  @brief
  Qualcomm Device Update service: a generic CID interface for firmware update
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
01/22/13  bd   Added module
=============================================================================*/

#ifndef QBI_SVC_QDU_MBIM_H
#define QBI_SVC_QDU_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of MSFWID device service CIDs
*/
typedef enum {
  QBI_SVC_QDU_CID_MIN = 0,

  QBI_SVC_QDU_MBIM_CID_QDU_UPDATE_SESSION = 1,
  QBI_SVC_QDU_MBIM_CID_QDU_FILE_OPEN = 2,
  QBI_SVC_QDU_MBIM_CID_QDU_FILE_WRITE = 3,

  QBI_SVC_QDU_CID_MAX
} qbi_svc_qdu_cid_e;

/*! Device service specific status code: file data received out of sequence */
#define QBI_SVC_QDU_MBIM_STATUS_DATA_OUT_OF_SEQUENCE \
  (QBI_MBIM_STATUS_EXT_DEV_SVC_FLAG | 0)

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_QDU_UPDATE_SESSION
    @{ */

/* QDU_SESSION_ACTION */
#define QBI_SVC_QDU_SESSION_ACTION_START (0)

/* QDU_SESSION_TYPE */
#define QBI_SVC_QDU_SESSION_TYPE_NONE (0)
#define QBI_SVC_QDU_SESSION_TYPE_LE   (1)

/* QDU_SESSION_STATUS */
#define QBI_SVC_QDU_SESSION_STATUS_INACTIVE      (0)
#define QBI_SVC_QDU_SESSION_STATUS_FILE_TRANSFER (1)
#define QBI_SVC_QDU_SESSION_STATUS_APPLY_UPDATE  (2)

/* QDU_SESSION_RESULT */
#define QBI_SVC_QDU_SESSION_RESULT_SUCCESS (0)
#define QBI_SVC_QDU_SESSION_RESULT_FAILURE (1)

/* Maximum size of the variable length LastSessionError field */
#define QBI_SVC_QDU_LAST_SESSION_ERROR_MAX_LEN_BYTES (510)

/* QDU_SET_UPDATE_SESSION */
typedef PACK(struct) {
  uint32 session_action;
  uint32 session_type;
} qbi_svc_qdu_update_session_s_req_s;

/* QDU_UPDATE_SESSION_INFO */
typedef PACK(struct) {
  uint32 current_session_type;
  uint32 current_session_status;
  uint32 last_session_type;
  uint32 last_session_result;

  qbi_mbim_offset_size_pair_s last_session_error;

  /*! @note Followed by DataBuffer containing LastSessionError */
} qbi_svc_qdu_update_session_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_QDU_FILE_OPEN
    @{ */

/* QDU_FILE_TYPE */
#define QBI_SVC_QDU_FILE_TYPE_LE_PACKAGE (0)

/* QDU_FILE_OPEN_INFO */
typedef PACK(struct) {
  uint32 file_type;
  uint32 file_size;
} qbi_svc_qdu_file_open_s_req_s;

/* QDU_FILE_TRANSFER_INFO */
typedef PACK(struct) {
  uint32 max_transfer_size;
  uint32 max_window_size;
} qbi_svc_qdu_file_open_s_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_QDU_FILE_WRITE
    @{ */

/* QDU_FILE_DATA: no structure definition as it is arbitrary data */

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_QDU_MBIM_H */

