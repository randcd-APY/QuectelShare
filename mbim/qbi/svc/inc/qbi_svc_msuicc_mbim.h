/*!
  @file
  qbi_svc_msuicc_mbim.h

  @brief
  Microsoft UICC device service implementation, based on the "MBIM EXTENSION
  FOR LOW-LEVEL UICC ACCESS INTERFACE SPECIFICATION" document from Microsoft.
  This device service provides low-level access to the UICC.
*/

/*=============================================================================

  Copyright (c) 2016-17 Qualcomm Technologies, Inc.
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
03/19/18  mm   Increased response buffer size for esim reset
06/07/17  mm   moved struct related to MSUICC CID RESET here
04/29/17  mm   Added enum MSUICC CID Reset
02/15/17  vs   Removed ununsed macros
07/21/16  hz   Added module
=============================================================================*/

#ifndef QBI_SVC_MSUICC_MBIM_H
#define QBI_SVC_MSUICC_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of MS UICC device service CIDs
*/
typedef enum {
  QBI_SVC_MSUICC_CID_MIN = 0,

  QBI_SVC_MSUICC_MBIM_CID_ATR                 = 1,
  QBI_SVC_MSUICC_MBIM_CID_OPEN_CHANNEL        = 2,
  QBI_SVC_MSUICC_MBIM_CID_CLOSE_CHANNEL       = 3,
  QBI_SVC_MSUICC_MBIM_CID_APDU                = 4,
  QBI_SVC_MSUICC_MBIM_CID_TERMINAL_CAPABILITY = 5,
  QBI_SVC_MSUICC_MBIM_CID_RESET               = 6,

  QBI_SVC_MSUICC_CID_MAX
} qbi_svc_msuicc_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/* Status words definition */
#define QBI_SVC_MSUICC_STATUS_WORD_LEN_BYTES   (4)
#define QBI_SVC_MSUICC_STATUS_WORD_SW1_SUCCESS (0x90)
#define QBI_SVC_MSUICC_STATUS_WORD_SW2_SUCCESS (0x00)

/*! @addtogroup MBIM_CID_MS_UICC_ATR
    @{ */

/* Maximum sizes of the variable length fields */
#define QBI_SVC_MSUICC_ATR_DATA_MAX_LEN_BYTES (33)

/* MBIM_MS_ATR_INFO */
typedef PACK(struct){
  qbi_mbim_offset_size_pair_s atr_data;
  /*! @note Followed by DataBuffer containing atr_data */
} qbi_svc_msuicc_atr_q_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_OPEN_CHANNEL
    @{ */

/* Maximum sizes of the variable length fields */
#define QBI_SVC_MSUICC_OPEN_CHANNEL_APP_ID_MAX_LEN_BYTES (32)
#define QBI_SVC_MSUICC_OPEN_CHANNEL_RESPONSE_MAX_LEN_BYTES (256)

/* MBIM_MS_SET_UICC_OPEN_CHANNEL */
typedef PACK(struct){
  qbi_mbim_offset_size_pair_s app_id;
  uint32                      select_p2_arg;
  uint32                      channel_group;
  /*! @note Followed by DataBuffer containing AppId */
} qbi_svc_msuicc_open_channel_s_req_s;

/* MBIM_MS_UICC_OPEN_CHANNEL_INFO */
typedef PACK(struct){
  uint8                       status[QBI_SVC_MSUICC_STATUS_WORD_LEN_BYTES];
  uint32                      channel;
  qbi_mbim_offset_size_pair_s response;
  /*! @note Followed by DataBuffer containing response */
} qbi_svc_msuicc_open_channel_s_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_CLOSE_CHANNEL
    @{ */

/* MBIM_MS_SET_UICC_CLOSE_CHANNEL */
typedef PACK(struct){
  uint32 channel;
  uint32 channel_group;
} qbi_svc_msuicc_close_channel_s_req_s;

/* MBIM_MS_UICC_CLOSE_CHANNEL_INFO */
typedef PACK(struct){
  uint8 status[QBI_SVC_MSUICC_STATUS_WORD_LEN_BYTES];
} qbi_svc_msuicc_close_channel_s_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_APDU
    @{ */

/* Maximum sizes of the variable length fields */
#define QBI_SVC_MSUICC_APDU_COMMAND_MAX_LEN_BYTES  (261)
#define QBI_SVC_MSUICC_APDU_RESPONSE_MAX_LEN_BYTES (256)

/* MBIM_MS_SET_UICC_APDU */
typedef PACK(struct){
  uint32                      channel;
  uint32                      secure_messaging;
  uint32                      type;
  qbi_mbim_offset_size_pair_s command;
  /*! @note Followed by DataBuffer containing command */
} qbi_svc_msuicc_apdu_s_req_s;

/* MBIM_MS_UICC_APDU_INFO */
typedef PACK(struct){
  uint8                       status[QBI_SVC_MSUICC_STATUS_WORD_LEN_BYTES];
  qbi_mbim_offset_size_pair_s response;
  /*! @note Followed by DataBuffer containing response */
} qbi_svc_msuicc_apdu_s_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_UICC_TERMINAL_CAPABILITY
    @{ */

/* MBIM_MS_SET_UICC_TERMINAL_CAPABILITY,
   MBIM_MS_UICC_TERMINAL_CAPABILITY_INFO */
typedef PACK(struct){
  uint32 element_count;
  /*! @note Followed by element_count instances of qbi_mbim_offset_size_pair_s,
      then DataBuffer containing element_count actual terminal capability objects */
} qbi_svc_msuicc_terminal_capability_q_rsp_s,
  qbi_svc_msuicc_terminal_capability_s_req_s;

/*! @} */

/*! @addtogroup QBI_SVC_MSUICC_MBIM_CID_RESET
    @{ */

/* Pass through action*/
#define PASS_THROUGH_ACTION_DISABLE 0
#define PASS_THROUGH_ACTION_ENABLE  1
/* pass through status*/
#define PASS_THROUGH_STATUS_DISABLED 0
#define PASS_THROUGH_STATUS_ENABLED  1

/*MBIM_MS_UICC_RESET_ACTION */
typedef PACK(struct){
  uint32 pass_through_action;
} qbi_svc_msuicc_reset_s_req_s;

/*MBIM_MS_UICC_RESET_STATUS */
typedef PACK(struct){
  uint32 pass_through_status;
} qbi_svc_msuicc_reset_q_rsp_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_MSUICC_MBIM_H */

