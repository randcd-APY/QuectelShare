/*!
  @file
  qbi_svc_mssar_mbim.h

  @brief
  Microsoft Selective Absorption Rate device service implementation, based on
  the "Mobile Broadband Device Selective Absorption Rate (SAR) platfor support"
  document from Microsoft. This device service allows host to query and set
  Selective Absorption Rate on device.
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

$Header: //source/qcom/qct/platform/winmobile/EA/workspaces/WMData/MBIM/MBIM_Dev/Windows/QBI/qbi/svc/inc/qbi_svc_mssar_mbim.h#2 $

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
08/17/12  vk   Added module
=============================================================================*/

#ifndef QBI_SVC_MSSAR_MBIM_H
#define QBI_SVC_MSSAR_MBIM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_mbim.h"

/*=============================================================================

  Definitions Common to the Device Service

=============================================================================*/

/*! @brief Enumeration of MSSAR device service CIDs
*/
typedef enum {
  QBI_SVC_MSSAR_CID_MIN = 0,

  QBI_SVC_MSSAR_MBIM_CID_MS_SAR_CONFIG = 1,
  QBI_SVC_MSSAR_MBIM_CID_MS_TRANSMISSION_STATUS = 2,

  QBI_SVC_MSSAR_CID_MAX
} qbi_svc_mssar_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_MS_SAR_CONFIG
    @{ */

/* MBIM_MS_SAR_BACKOFF_STATUS */
#define QBI_SVC_MBIM_MS_SAR_BACKOFF_STATUS_DISABLED       (0)
#define QBI_SVC_MBIM_MS_SAR_BACKOFF_STATUS_ENABLED        (1)

/* MBIM_MS_SAR_CONTROL_MODE */
#define QBI_SVC_MBIM_MS_SAR_CONTROL_MODE_DEVICE           (0)
#define QBI_SVC_MBIM_MS_SAR_CONTROL_MODE_OS               (1)

/* MBIM_MS_SAR_HARDWARE_WIFI_INTEGRATION */
#define QBI_SVC_MBIM_MS_SAR_WIFI_HARDWARE_NOT_INTEGRATED  (0)
#define QBI_SVC_MBIM_MS_SAR_WIFI_HARDWARE_INTEGRATED      (1)

#define QBI_SVC_MBIM_MS_SAR_ANTENNA_INDEX   0xFFFFFFFF

/* MBIM_MS_SAR_BACKOFF_STATE */
typedef PACK(struct) {
  uint32 antenna_index;
  uint32 backoff_index;
} qbi_svc_mssar_config_state_s;

/* MBIM_MS_SAR_CONFIG */
typedef PACK(struct) {
  uint32 mode;
  uint32 backoff_status;
  uint32 wifi_intigration;
  uint32 element_count;
  /*! @note Followed by element_count instances of
      qbi_mbim_offset_size_pair_s, then element_count 
      qbi_svc_mssar_config_state_s */
} qbi_svc_mssar_sar_config_rsp_s;

/* MBIM_MS_SET_SAR_CONFIG */
typedef PACK(struct) {
  uint32 mode;
  uint32 backoff_status;
  uint32 element_count;
  /*! @note Followed by element_count instances of 
      qbi_mbim_offset_size_pair_s, then element_count 
      qbi_svc_mssar_config_state_s */
} qbi_svc_mssar_set_sar_config_s_req_s;

/*! @} */

/*! @addtogroup MBIM_CID_MS_SAR_TRANSMISSION_STATUS
    @{ */

/* MBIM_MS_SAR_TRANSMISSION_STATUS_NOTIFICATION */
#define QBI_SVC_MBIM_MS_SAR_TRANSMISSION_NOTIFICATION_DISABLED  (0)
#define QBI_SVC_MBIM_MS_SAR_TRANSMISSION_NOTIFICATION_ENABLED   (1)

/* MBIM_MS_SAR_TRANSMISSION_STATE */
#define QBI_SVC_MBIM_MS_SAR_TRANSMISSION_STATE_INACTIVE         (0)
#define QBI_SVC_MBIM_MS_SAR_TRANSMISSION_STATE_ACTIVE           (1)

/* MBIM_MS_SAR_TRANSMISSION_STATUS_INFO */
typedef PACK(struct) {
  uint32 channel_notification;
  uint32 transmission_status;
  uint32 hysteresis_timer;
} qbi_svc_mssar_transmission_status_info_s;

/* MBIM_MS_SET_TRANSMISSION_STATUS */
typedef PACK(struct) {
  uint32 channel_notification;
  uint32 hysteresis_timer;
} qbi_svc_mssar_set_transmission_status_s_req_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_MSSAR_MBIM_H */

