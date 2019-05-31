/*!
  @file
  qbi_ut.c

  @brief
  QBI unit tests, using mock USB & QCCI layers
*/

/*=============================================================================

  Copyright (c) 2011-2014, 2016-2018 Qualcomm Technologies, Inc.
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
01/09/18  nk   Added UT for PCO info
06/28/17  rv   Updated UT for Prov Context V2
06/12/17  rv   Updated for LTE attach config
05/30/17  rv   Added ut for LTE attach config
05/23/17  mm   Add slot info unit test
04/29/17  mm   Add esim and device reset unit test
03/22/17  vk   Add slot mapping unit test
09/06/16  hz   Add uicc unit tests
03/13/13  hz   Support multiple provisioned contexts and multi-PDN unit test
01/22/13  bd   Support new unit test framework
10/08/12  hz   Add Diag-over-MBIM unit test
05/11/12  bd   Integrate PACKET_FILTERS unit tests
02/10/12  bd   Updates for Linux
09/02/11  bd   Updated to MBIM v0.81c
07/28/11  bd   Initial release based on MBIM v0.3+
07/28/11  hz   Add USSD tests based on MBIM v0.3+
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"

#ifdef QBI_UNIT_TEST

#include "qbi_ut.h"

#include "qbi_hc.h"
#include "qbi_msg.h"
#include "qbi_msg_mbim.h"
#include "qbi_mbim.h"
#include "qbi_os.h"
#include "qbi_qmux.h"
#include "qbi_svc.h"
#include "qbi_svc_atds_mbim.h"
#include "qbi_svc_bc_common.h"
#include "qbi_svc_bc_mbim.h"
#include "qbi_svc_ext_qmux_mbim.h"
#include "qbi_svc_msfwid_mbim.h"
#include "qbi_svc_mshsd_mbim.h"
#include "qbi_svc_msuicc_mbim.h"
#include "qbi_svc_pb_mbim.h"
#include "qbi_svc_qmbe_mbim.h"
#include "qbi_svc_ussd_mbim.h"
#include "qbi_svc_stk_mbim.h"
#include "qbi_svc_auth.h"
#include "qbi_svc_auth_mbim.h"
#include "qbi_task.h"
#include "qbi_utf.h"
#include "qbi_svc_bc_ext_mbim.h"

#include "control_service_v01.h"
#include "device_management_service_v01.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "user_identity_module_v01.h"
#include "qbi_svc_msuicc.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/* Define which context ID to use for tests. The primary context ID can be used
   to effectively inject requests to the context associated with the USB
   connection, while the unit test context ID is not directly affected by CIDs
   originating from a real host. */
//#define QBI_UT_CONTEXT_ID (QBI_CTX_ID_UNIT_TEST)
#define QBI_UT_CONTEXT_ID (QBI_CTX_ID_PRIMARY)

//#define QBI_UT_TEST_USSD
//#define QBI_UT_TEST_CONNECT
//#define QBI_UT_TEST_CONNECT_MPDP
//#define QBI_UT_TEST_EXT_QMUX
//#define QBI_UT_TEST_STK
//#define QBI_UT_TEST_PB
//#define QBI_UT_TEST_AUTH
//#define QBI_UT_TEST_MSHSD
//#define QBI_UT_TEST_PACKET_FILTER
//#define QBI_UT_TEST_QMBE
//#define QBI_UT_TEST_MSFWID
//#define QBI_UT_TEST_MSUICC
//#define QBI_UT_TEST_UICC_RESET
//#define QBI_UT_TEST_DEVICE_RESET
//#define QBI_UT_TEST_MISC
//#define QBI_UT_TEST_ATDS
//#define QBI_UT_TEST_UTF
//#define QBI_UT_TEST_PROVISIONED_CONTEXTS
//#define QBI_UT_TEST_PROVISIONED_CONTEXTS_V2
//#define QBI_UT_TEST_SLOT_MAPPING
//#define QBI_UT_TEST_SLOT_INFO
//#define QBI_UT_TEST_LTE_ATTACH

typedef enum {
  QBI_UT_INIT = 0,

  #ifdef QBI_UT_TEST_CONNECT
  QBI_UT_CONNECT_0,
  QBI_UT_IP_CFG_INFO_0,
  #ifdef QBI_UT_TEST_CONNECT_MPDP
  QBI_UT_CONNECT_1,
  QBI_UT_IP_CFG_INFO_1,
  QBI_UT_CONNECT_2,
  QBI_UT_IP_CFG_INFO_2,
  QBI_UT_CONNECT_3,
  QBI_UT_IP_CFG_INFO_3,
  QBI_UT_CONNECT_4,
  QBI_UT_IP_CFG_INFO_4,
  #endif
  QBI_UT_DISCONNECT_0,
  #ifdef QBI_UT_TEST_CONNECT_MPDP
  QBI_UT_DISCONNECT_1,
  QBI_UT_DISCONNECT_2,
  QBI_UT_DISCONNECT_3,
  QBI_UT_DISCONNECT_4,
  #endif
  #endif /* QBI_UT_TEST_CONNECT */

  #ifdef QBI_UT_TEST_USSD
  QBI_UT_USSD_ORIG,
  QBI_UT_USSD_ANSWER1,
  QBI_UT_USSD_ANSWER2,
  QBI_UT_USSD_ANSWER3,
  QBI_UT_USSD_ANSWER4,
  QBI_UT_USSD_ANSWER5,
  QBI_UT_USSD_CANCEL,
  #endif /* QBI_UT_TEST_USSD */

  #ifdef QBI_UT_TEST_EXT_QMUX
  QBI_UT_EXT_QMUX_CLID_ALLOC,
  QBI_UT_EXT_QMUX_SET_EVENT_REPORT,
  QBI_UT_EXT_QMUX_SET_OPRT_MODE_LPM,
  QBI_UT_EXT_QMUX_WAIT_OPRT_MODE_IND,
  QBI_UT_EXT_QMUX_SET_OPRT_MODE_ONLINE,
  QBI_UT_EXT_QMUX_CLID_DEALLOC,
  #endif /* QBI_UT_TEST_EXT_QMUX */

  #ifdef QBI_UT_TEST_STK
  QBI_UT_STK_PAC_Q,
  QBI_UT_STK_PAC_S,
  QBI_UT_STK_TR_S,
  QBI_UT_STK_ENV_Q,
  QBI_UT_STK_ENV_S,
  #endif /* QBI_UT_TEST_STK */

  #ifdef QBI_UT_TEST_PB
  QBI_UT_PB_CONFIG,
  QBI_UT_PB_DELETE,
  QBI_UT_PB_WRITE,
  QBI_UT_PB_READ,
  #endif /* QBI_UT_TEST_PB */

  #ifdef QBI_UT_TEST_AUTH
  QBI_UT_AUTH_AKA_Q,
  QBI_UT_AUTH_AKAP_Q,
  QBI_UT_AUTH_SIM_Q,
  #endif /* QBI_UT_TEST_AUTH */

  #ifdef QBI_UT_TEST_MSHSD
  QBI_UT_MSHSD,
  #endif /* QBI_UT_TEST_MSHSD */

  #ifdef QBI_UT_TEST_PACKET_FILTER
  QBI_UT_PACKET_FILTER_ENABLE_0,
  QBI_UT_PACKET_FILTER_QUERY_0,
  QBI_UT_PACKET_FILTER_ENABLE_1,
  QBI_UT_PACKET_FILTER_QUERY_1,
  QBI_UT_PACKET_FILTER_ENABLE_2,
  QBI_UT_PACKET_FILTER_QUERY_2,
  QBI_UT_PACKET_FILTER_ENABLE_3,
  QBI_UT_PACKET_FILTER_QUERY_3,
  QBI_UT_PACKET_FILTER_ENABLE_4,
  QBI_UT_PACKET_FILTER_QUERY_4,
  QBI_UT_PACKET_FILTER_DISABLE_0,
  QBI_UT_PACKET_FILTER_DISABLE_1,
  QBI_UT_PACKET_FILTER_DISABLE_2,
  QBI_UT_PACKET_FILTER_DISABLE_3,
  QBI_UT_PACKET_FILTER_DISABLE_4,
  #endif /* QBI_UT_TEST_PACKET_FILTER */

  #ifdef QBI_UT_TEST_QMBE
  QBI_UT_QMBE_DIAG_DATA_S,
  QBI_UT_QMBE_DIAG_CONFIG_Q,
  QBI_UT_QMBE_DIAG_CONFIG_S,
  #endif /* QBI_UT_TEST_QMBE */

  #ifdef QBI_UT_TEST_MSFWID
  QBI_UT_MSFWID,
  #endif /* QBI_UT_TEST_MSFWID */

  #ifdef QBI_UT_TEST_MSUICC
  QBI_UT_MSUICC_ATR_Q,
  QBI_UT_MSUICC_OPEN_CHANNEL_S,
  QBI_UT_MSUICC_APDU_S,
  QBI_UT_MSUICC_TERMINAL_CAPABILITY_S,
  QBI_UT_MSUICC_TERMINAL_CAPABILITY_Q,
  QBI_UT_MSUICC_CLOSE_CHANNEL_S,
  #endif /* QBI_UT_TEST_MSUICC */

  #ifdef QBI_UT_TEST_UICC_RESET
  QBI_UT_MSUICC_RESET_S_1,
  QBI_UT_MSUICC_ATR_Q_1,
  QBI_UT_MSUICC_OPEN_CHANNEL_S_1,
  QBI_UT_MSUICC_APDU_S_1,
  QBI_UT_MSUICC_CLOSE_CHANNEL_S_1,
  QBI_UT_MSUICC_RESET_Q_1,
  QBI_UT_MSUICC_RESET_S_2,
  QBI_UT_MSUICC_ATR_Q_2,
  QBI_UT_MSUICC_OPEN_CHANNEL_S_2,
  QBI_UT_MSUICC_APDU_S_2,
  QBI_UT_MSUICC_CLOSE_CHANNEL_S_2,
  QBI_UT_MSUICC_RESET_Q_2,
  QBI_UT_MSUICC_RESET_S_3,
  QBI_UT_MSUICC_ATR_Q_3,
  QBI_UT_MSUICC_OPEN_CHANNEL_S_3,
  QBI_UT_MSUICC_OPEN_CHANNEL_S_4,
  QBI_UT_MSUICC_OPEN_CHANNEL_S_5,
  QBI_UT_MSUICC_APDU_S_3,
  QBI_UT_MSUICC_CLOSE_CHANNEL_S_3,
  QBI_UT_MSUICC_RESET_Q_3,
  #endif/* QBI_UT_TEST_UICC_RESET */

  #ifdef QBI_UT_TEST_DEVICE_RESET
  QBI_UT_DEVICE_RESET_S,
  #endif/* QBI_UT_TEST_DEVICE_RESET */

  #ifdef QBI_UT_TEST_MISC
  QBI_UT_UIM_REFRESH,
  #endif /* QBI_UT_TEST_MISC */

  #ifdef QBI_UT_TEST_ATDS
  QBI_UT_ATDS_PROJECTION_TABLES_S,
  QBI_UT_ATDS_PROJECTION_TABLES_Q,
  QBI_UT_ATDS_SIGNAL_Q,
  QBI_UT_ATDS_LOCATION_INFO_Q,
  QBI_UT_ATDS_REGISTER_STATE_Q,
  QBI_UT_ATDS_OPERATORS_S,
  QBI_UT_ATDS_OPERATORS_Q,
  QBI_UT_ATDS_RAT_S,
  QBI_UT_ATDS_RAT_Q,
  #endif /* QBI_UT_TEST_ATDS */

  #ifdef QBI_UT_TEST_UTF
  QBI_UT_UTF_RUN,
  QBI_UT_UTF_RUNNING,
  #endif /* QBI_UT_TEST_UTF */

  #ifdef QBI_UT_TEST_PROVISIONED_CONTEXTS
  QBI_UT_PROVISIONED_CONTEXTS_Q,
  QBI_UT_PROVISIONED_CONTEXTS_S,
  QBI_UT_PROVISIONED_CONTEXTS_S_APPEND,
  #endif /* QBI_UT_TEST_PROVISIONED_CONTEXTS */

  #ifdef QBI_UT_TEST_PROVISIONED_CONTEXTS_V2
  QBI_UT_PROVISIONED_CONTEXTS_V2_Q,
  QBI_UT_PROVISIONED_CONTEXTS_V2_S,
  QBI_UT_PROVISIONED_CONTEXTS_V2_S_APPEND,
  #endif /* QBI_UT_TEST_PROVISIONED_CONTEXTS_V2 */
  
  #ifdef QBI_UT_TEST_SLOT_MAPPING
  QBI_UT_SLOT_MAPPING_Q,
  QBI_UT_SLOT_MAPPING_S,
  #endif /* QBI_UT_TEST_SLOT_MAPPING */

  #ifdef QBI_UT_TEST_SLOT_INFO
  QBI_UT_SLOT_INFO_Q,
  QBI_UT_SLOT_INFO_S,
  #endif /* QBI_UT_TEST_SLOT_INFO */
  
  #ifdef QBI_UT_TEST_LTE_ATTACH
  QBI_UT_LTE_ATTACH_CONFIG_Q,
  QBI_UT_LTE_ATTACH_CONFIG_S,
  #endif /* QBI_UT_TEST_LTE_ATTACH */

  #ifdef QBI_UT_TEST_PCO_INFO
  QBI_UT_TEST_PCO_INFO_Q,
  #endif /* QBI_UT_TEST_PCO_INFO */

  QBI_UT_MAX,

  /* Temporary storage for tests we want to skip */

  QBI_UT_OPEN,
  QBI_UT_DEV_CAPS,
  QBI_UT_RDY_INFO,
  QBI_UT_RF_STATE,
  QBI_UT_REG_STATE,
  QBI_UT_PKT_SRVC,
  QBI_UT_CLOSE,

  QBI_UT_VIS_PRO,
  QBI_UT_EVENT_FILTER,
  QBI_UT_DEV_SVCS,
  QBI_UT_RF_STATE_SET,
} qbi_ut_state_e;

#define QBI_UT_MAX_XFER (2048)
#define QBI_UT_USSD_ORIG_STRING_VOD "*100#"
#define QBI_UT_USSD_ORIG_STRING_ATT "*646#"

#define QBI_UT_CONNECT_SESSION_NUM (5)

#define QBI_UT_QMI_SVC_ID_DMS (0x02)

#define QBI_UT_PKT_FILTER_EFS "/var/lib/mbimd/0/PacketFilterData.bin"

/*=============================================================================

  Private Typedefs

=============================================================================*/

typedef void (qbi_ut_verify_rsp_cb)
(
  const uint8 *buf,
  uint32       buf_len
);

typedef struct {
  qbi_ut_state_e state;
  qbi_ctx_s *ctx;
  qbi_ctx_s ut_ctx;

  qbi_os_timer_t timer;
  qbi_os_timer_t timeout;

  /*! Current, pending transaction ID */
  uint32 txn_id;

  /*! Multipurpose counter */
  uint32 count;

  /*! Callback function used to perform detailed response validation */
  qbi_ut_verify_rsp_cb *verify_cb;

  /*! QMI Client IDs allocated for EXT_QMUX testing */
  struct {
    uint8 dms;
  } qmi_clids;
} qbi_ut_state_s;

#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! Type/length header for QMI TLV */
typedef PACK(struct) {
  uint8  type;
  uint16 length;
  /*! @note Followed by length bytes of data */
} qbi_ut_qmi_tlv_s;

#ifdef _WIN32
#pragma pack(pop)
#endif

/*=============================================================================

  Private Variables

=============================================================================*/

/* Basic Connectivity device service UUID */
static const uint8 qbi_ut_uuid_bc[QBI_MBIM_UUID_LEN] = {
  0xa2, 0x89, 0xcc, 0x33, 0xbc, 0xbb, 0x8b, 0x4f,
  0xb6, 0xb0, 0x13, 0x3e, 0xc2, 0xaa, 0xe6, 0xdf
};

static const uint8 qbi_ut_uuid_bc_ext[QBI_MBIM_UUID_LEN] = {
  0x3d, 0x01, 0xdc, 0xc5, 0xfe, 0xf5, 0x4d, 0x05,
  0x0d, 0x3a, 0xbe, 0xf7, 0x05, 0x8e, 0x9a, 0xaf
};

#ifdef QBI_UT_TEST_USSD
/* USSD service UUID */
static const uint8 qbi_ut_uuid_ussd[QBI_MBIM_UUID_LEN] = {
  0xe5, 0x50, 0xa0, 0xc8, 0x5e, 0x82, 0x47, 0x9e,
  0x82, 0xf7, 0x10, 0xab, 0xf4, 0xc3, 0x35, 0x1f
};
#endif /* QBI_UT_TEST_USSD */

#ifdef QBI_UT_TEST_CONNECT
static uint32 qbi_ut_connect_current_session_id = 0;
static const char* qbi_ut_connect_apn_names[QBI_UT_CONNECT_SESSION_NUM] = {
  "phone",
  "vzwapp",
  "vzwims",
  "vzwadmin",
  "ut_access_string"
};
#endif /* QBI_UT_TEST_CONNECT */

#ifdef QBI_UT_TEST_EXT_QMUX
/* EXT_QMUX service UUID */
static const uint8 qbi_ut_uuid_ext_qmux[QBI_MBIM_UUID_LEN] = {
  0xd1, 0xa3, 0x0b, 0xc2, 0xf9, 0x7a, 0x6e, 0x43,
  0xbf, 0x65, 0xc7, 0xe2, 0x4f, 0xb0, 0xf0, 0xd3
};
#endif /* QBI_UT_TEST_EXT_QMUX */

#ifdef QBI_UT_TEST_STK
/* STK device service UUID */
static const uint8 qbi_ut_uuid_stk[QBI_MBIM_UUID_LEN] = {
  0xd8, 0xf2, 0x01, 0x31, 0xfc, 0xb5, 0x4e, 0x17,
  0x86, 0x02, 0xd6, 0xed, 0x38, 0x16, 0x16, 0x4c
};
#endif /* QBI_UT_TEST_STK */

#ifdef QBI_UT_TEST_PB
/* Phonebook device service UUID */
static const uint8 qbi_ut_uuid_pb[QBI_MBIM_UUID_LEN] = {
  0x4b, 0xf3, 0x84, 0x76, 0x1e, 0x6a, 0x41, 0xdb,
  0xb1, 0xd8, 0xbe, 0xd2, 0x89, 0xc2, 0x5b, 0xdb
};
#endif /* QBI_UT_TEST_PB */

#ifdef QBI_UT_TEST_AUTH
/* AUTH device service UUID */
static const uint8 qbi_ut_uuid_auth[QBI_MBIM_UUID_LEN] = {
  0x1d, 0x2b, 0x5f, 0xf7, 0x0a, 0xa1, 0x48, 0xb2,
  0xaa, 0x52, 0x50, 0xf1, 0x57, 0x67, 0x17, 0x4e
};
#endif /* QBI_UT_TEST_AUTH */

#ifdef QBI_UT_TEST_MSHSD
/* Host shutdown notification device service UUID */
static const uint8 qbi_ut_uuid_mshsd[QBI_MBIM_UUID_LEN] = {
  0x88, 0x3b, 0x7c, 0x26, 0x98, 0x5f, 0x43, 0xfa,
  0x98, 0x04, 0x27, 0xd7, 0xfb, 0x80, 0x95, 0x9c
};
#endif /* QBI_UT_TEST_MSHSD */

#ifdef QBI_UT_TEST_QMBE
/* Qualcomm Mobile Broadband MBIM Extensibility device service UUID */
static const uint8 qbi_ut_uuid_qmbe[QBI_MBIM_UUID_LEN] = {
  0x2d, 0x0c, 0x12, 0xc9, 0x0e, 0x6a, 0x49, 0x5a,
  0x91, 0x5c, 0x8d, 0x17, 0x4f, 0xe5, 0xd6, 0x3c
};
static uint32 qbi_ut_qmbe_last_diag_config;
#endif /* QBI_UT_TEST_QMBE */

#ifdef QBI_UT_TEST_MSFWID
/* Microsoft Firmware ID - device service UUID */
static const uint8 qbi_ut_uuid_msfwid[QBI_MBIM_UUID_LEN] = {
  0xe9, 0xf7, 0xde, 0xa2, 0xfe, 0xaf, 0x40, 0x09,
  0x93, 0xce, 0x90, 0xa3, 0x69, 0x41, 0x03, 0xb6
};
#endif /* QBI_UT_TEST_MSFWID */

#if defined(QBI_UT_TEST_MSUICC) || defined(QBI_UT_TEST_UICC_RESET)
/* Microsoft UICC - device service UUID */
static const uint8 qbi_ut_uuid_msuicc[QBI_MBIM_UUID_LEN] = {
  0xc2, 0xf6, 0x58, 0x8e, 0xf0, 0x37, 0x4b, 0xc9,
  0x86, 0x65, 0xf4, 0xd4, 0x4b, 0xd0, 0x93, 0x67
};
static uint32 msuicc_channel;
static uint32 msuicc_channel_group;
#endif /* QBI_UT_TEST_MSUICC || QBI_UT_TEST_UICC_RESET*/

#ifdef QBI_UT_TEST_ATDS
/* AT&T - device service UUID */
static const uint8 qbi_ut_uuid_atds[QBI_MBIM_UUID_LEN] = {
  0x59, 0x67, 0xBD, 0xCC, 0x7F, 0xD2, 0x49, 0xA2,
  0x9f, 0x5C, 0xB2, 0xE7, 0x0E, 0x52, 0x7D, 0xB3
};
#endif /* QBI_UT_TEST_ATDS */

static qbi_ut_state_s qbi_ut_state;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

/* Common functions */
static void qbi_ut_append_field_ascii_to_utf16
(
  qbi_util_buf_s              *buf,
  qbi_mbim_offset_size_pair_s *field_desc,
  const char                  *ascii_str,
  uint32                       ascii_str_len
);

static boolean qbi_ut_build_cmd_req
(
  qbi_ut_state_s    *state,
  qbi_msg_cmd_req_s *req,
  qbi_svc_id_e       svc_id,
  uint32             cid,
  uint32             cmd_type,
  uint32             infobuf_len,
  const void        *infobuf
);

static void qbi_ut_cancel_pending_txn
(
  qbi_ut_state_s *state
);

static boolean qbi_ut_check_cmd_rsp
(
  const uint8 *data,
  uint32       data_len,
  const uint8 *uuid,
  uint32       cid,
  uint32       status
);

static void qbi_ut_post_cmd
(
  qbi_ut_state_s *state
);

static void qbi_ut_run
(
  qbi_ut_state_s *state
);

static void qbi_ut_send_cmd_req
(
  qbi_ut_state_s    *state,
  qbi_svc_id_e       svc_id,
  uint32             cid,
  uint32             cmd_type,
  uint32             infobuf_len,
  const void        *infobuf
);

static void qbi_ut_timeout_cb
(
  void *data
);

static void qbi_ut_timer_cb
(
  void *data
);

static boolean qbi_ut_txn_id_matches
(
  const uint8 *buf,
  uint32       buf_len,
  uint32       expected_txn_id
);

static void qbi_ut_verify_open_rsp
(
  const uint8 *buf,
  uint32       buf_len
);

static void qbi_ut_verify_close_rsp
(
  const uint8 *buf,
  uint32       buf_len
);


/* Basic Connectivity device service functions */
static void qbi_ut_bc_device_services_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_device_services_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_device_service_subscribe_list_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_device_service_subscribe_list_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_device_caps_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_device_caps_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_ready_info_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_ready_info_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_radio_state_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_radio_state_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_radio_state_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_register_state_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_register_state_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_packet_service_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_packet_service_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_visible_providers_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_visible_providers_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

#ifdef QBI_UT_TEST_CONNECT
static void qbi_ut_bc_connect_s_activate_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_connect_s_activate_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_ip_configuration_info_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_ip_configuration_info_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_connect_s_deactivate_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_connect_s_deactivate_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_CONNECT */

#ifdef QBI_UT_TEST_EXT_QMUX
static boolean qbi_ut_qmi_idl_decode
(
  qmi_idl_service_object_type  svc_obj,
  uint16                       qmi_msg_id,
  qmi_idl_type_of_message_type idl_msg_type,
  const uint8                 *tlv_data,
  uint32                       tlv_data_size,
  qbi_util_buf_s              *decoded_buf
);

static void qbi_ut_ext_qmux_s_clid_alloc
(
  qbi_ut_state_s *state
);

static void qbi_ut_ext_qmux_s_clid_alloc_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_ext_qmux_s_set_event_report
(
  qbi_ut_state_s *state
);

static void qbi_ut_ext_qmux_s_set_oprt_mode
(
  qbi_ut_state_s *state,
  uint8           operating_mode
);

static void qbi_ut_ext_qmux_wait_for_ind
(
  qbi_ut_state_s *state
);

static void qbi_ut_ext_qmux_s_clid_dealloc
(
  qbi_ut_state_s *state
);

static void qbi_ut_ext_qmux_s_qmi_generic_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_EXT_QMUX */

#ifdef QBI_UT_TEST_STK
static void qbi_ut_stk_pac_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_stk_pac_q_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_stk_pac_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_stk_pac_s_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_stk_tr_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_stk_tr_s_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_stk_env_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_stk_env_q_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_stk_env_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_stk_env_s_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_STK */

#ifdef QBI_UT_TEST_PB
static void qbi_ut_pb_config_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_pb_config_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_pb_delete_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_pb_delete_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_pb_write_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_pb_write_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_pb_read_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_pb_read_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_PB */

#ifdef QBI_UT_TEST_AUTH
static void qbi_ut_auth_aka_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_auth_aka_q_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_auth_akap_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_auth_akap_q_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_auth_sim_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_auth_sim_q_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_AUTH */

#ifdef QBI_UT_TEST_MSHSD
static void qbi_ut_mshsd_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_mshsd_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_MSHSD */

#ifdef QBI_UT_TEST_QMBE
static void qbi_ut_qmbe_diag_config_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_qmbe_diag_config_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_qmbe_diag_config_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_qmbe_diag_config_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_qmbe_diag_data_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_qmbe_diag_data_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_QMBE */

#ifdef QBI_UT_TEST_MSFWID
static void qbi_ut_msfwid_firmwareid_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_msfwid_firmwareid_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_MSFWID */

#if defined(QBI_UT_TEST_MSUICC) || defined(QBI_UT_TEST_UICC_RESET)
static void qbi_ut_msuicc_atr_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_msuicc_atr_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_msuicc_open_channel_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_msuicc_open_channel_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_msuicc_close_channel_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_msuicc_close_channel_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_msuicc_reset_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_msuicc_reset_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_msuicc_reset_s_req
(
  qbi_ut_state_s *state,
  uint8 action
);

static void qbi_ut_msuicc_reset_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_msuicc_apdu_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_msuicc_apdu_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
static void qbi_ut_msuicc_terminal_capability_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_msuicc_terminal_capability_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_msuicc_terminal_capability_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_msuicc_terminal_capability_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_MSUICC || QBI_UT_TEST_UICC_RESET*/

#ifdef QBI_UT_TEST_DEVICE_RESET
static void qbi_ut_device_reset_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_device_reset_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_DEVICE_RESET */

#ifdef QBI_UT_TEST_MISC
static void qbi_ut_misc_uim_refresh
(
  qbi_ut_state_s *state
);
#endif /* QBI_UT_TEST_MISC */

#ifdef QBI_UT_TEST_ATDS
static void qbi_ut_atds_signal_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_atds_signal_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_atds_location_info_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_atds_location_info_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_atds_operators_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_atds_operators_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_atds_operators_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_atds_projection_tables_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_atds_projection_tables_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_atds_projection_tables_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_atds_rat_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_atds_rat_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_atds_rat_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_atds_register_state_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_atds_register_state_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_ATDS */

#ifdef QBI_UT_TEST_PROVISIONED_CONTEXTS
static void qbi_ut_bc_provisioned_contexts_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_provisioned_contexts_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_provisioned_contexts_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_provisioned_contexts_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_provisioned_contexts_s_append_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_provisioned_contexts_s_append_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_PROVISIONED_CONTEXTS */

#ifdef QBI_UT_TEST_SLOT_MAPPING
static void qbi_ut_bc_ext_slot_mapping_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_ext_slot_mapping_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_ext_slot_mapping_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_ext_slot_mapping_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

#endif /* QBI_UT_TEST_SLOT_MAPPING */

#ifdef QBI_UT_TEST_SLOT_INFO
static void qbi_ut_bc_ext_slot_info_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_ext_slot_info_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif /* QBI_UT_TEST_SLOT_INFO */

#ifdef QBI_UT_TEST_PROVISIONED_CONTEXTS_V2
static void qbi_ut_bc_provisioned_contexts_q_v2_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_provisioned_contexts_q_verify_v2_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_provisioned_contexts_s_v2_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_provisioned_contexts_s_v2_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif

#ifdef QBI_UT_LTE_ATTACH
static void qbi_ut_bc_lte_attach_q_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_lte_attach_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);

static void qbi_ut_bc_lte_attach_s_req
(
  qbi_ut_state_s *state
);

static void qbi_ut_bc_lte_attach_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
);
#endif

/*=============================================================================

  Private Function Definitions

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_ut_append_field_ascii_to_utf16
===========================================================================*/
/*!
    @brief Appends a variable length field following MBIM rules to a buffer

    @details

    @param buf
    @param field_desc
    @param ascii_str
    @param ascii_str_len
*/
/*=========================================================================*/
static void qbi_ut_append_field_ascii_to_utf16
(
  qbi_util_buf_s              *buf,
  qbi_mbim_offset_size_pair_s *field_desc,
  const char                  *ascii_str,
  uint32                       ascii_str_len
)
{
  uint32 pre_padding;
  uint32 post_padding;
  uint8 *utf16_str;
  uint32 offset;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);
  QBI_CHECK_NULL_PTR_RET(field_desc);
  QBI_CHECK_NULL_PTR_RET(ascii_str);

  pre_padding  = (buf->size % 4) ? (4 - (buf->size % 4)) : 0;
  field_desc->offset = buf->size + pre_padding;
  field_desc->size = QBI_STRLEN(ascii_str) * 2;
  post_padding = (field_desc->size % 4) ? (4 - (field_desc->size % 4)) : 0;

  offset = (uint8 *) field_desc - (uint8 *) buf->data;
  if (qbi_util_buf_resize(
        buf, (buf->size + pre_padding + field_desc->size + post_padding)))
  {
    field_desc = (qbi_mbim_offset_size_pair_s *) ((uint8 *) buf->data + offset);
    utf16_str = (uint8 *) buf->data + field_desc->offset;
    qbi_util_ascii_to_utf16(ascii_str, ascii_str_len,
                            utf16_str, field_desc->size);
  }
} /* qbi_ut_append_field_ascii_to_utf16() */

/*===========================================================================
  FUNCTION: qbi_ut_build_cmd_req
===========================================================================*/
/*!
    @brief Performs common setup of a command message request

    @details

    @param state
    @param req
    @param svc_id
    @param cid
    @param cmd_type
*/
/*=========================================================================*/
static boolean qbi_ut_build_cmd_req
(
  qbi_ut_state_s    *state,
  qbi_msg_cmd_req_s *req,
  qbi_svc_id_e       svc_id,
  uint32             cid,
  uint32             cmd_type,
  uint32             infobuf_len,
  const void        *infobuf
)
{
  uint8 *pkt_infobuf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(state);
  QBI_CHECK_NULL_PTR_RET_FALSE(req);

  req->hdr.msg_type = QBI_MSG_MBIM_COMMAND_MSG;
  req->hdr.txn_id = ++(state->txn_id);
  req->hdr.msg_len = sizeof(qbi_msg_cmd_req_s) + infobuf_len;

  req->total_frag = 1;
  req->cur_frag = 0;
  switch (svc_id)
  {
    case QBI_SVC_ID_BC:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_bc, QBI_MBIM_UUID_LEN);
      break;

    #ifdef QBI_UT_TEST_USSD
    case QBI_SVC_ID_USSD:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_ussd, QBI_MBIM_UUID_LEN);
      break;
    #endif /* QBI_UT_TEST_USSD */

    #ifdef QBI_UT_TEST_EXT_QMUX
    case QBI_SVC_ID_EXT_QMUX:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_ext_qmux, QBI_MBIM_UUID_LEN);
      break;
    #endif /* QBI_UT_TEST_EXT_QMUX */

    #ifdef QBI_UT_TEST_STK
    case QBI_SVC_ID_STK:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_stk, QBI_MBIM_UUID_LEN);
      break;
    #endif /* QBI_UT_TEST_STK */

    #ifdef QBI_UT_TEST_PB
    case QBI_SVC_ID_PB:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_pb, QBI_MBIM_UUID_LEN);
      break;
    #endif /* QBI_UT_TEST_PB */

    #ifdef QBI_UT_TEST_AUTH
    case QBI_SVC_ID_AUTH:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_auth, QBI_MBIM_UUID_LEN);
      break;
    #endif /* QBI_UT_TEST_AUTH */

    #ifdef QBI_UT_TEST_MSHSD
    case QBI_SVC_ID_MSHSD:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_mshsd, QBI_MBIM_UUID_LEN);
      break;
    #endif /* QBI_UT_TEST_MSHSD */

    #ifdef QBI_UT_TEST_QMBE
    case QBI_SVC_ID_QMBE:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_qmbe, QBI_MBIM_UUID_LEN);
      break;
    #endif /* QBI_UT_TEST_QMBE */

    #ifdef QBI_UT_TEST_MSFWID
    case QBI_SVC_ID_MSFWID:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_msfwid, QBI_MBIM_UUID_LEN);
      break;
    #endif /* QBI_UT_TEST_MSFWID */

    #if defined(QBI_UT_TEST_MSUICC) || defined(QBI_UT_TEST_UICC_RESET)
    case QBI_SVC_ID_MSUICC:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_msuicc, QBI_MBIM_UUID_LEN);
      break;
    #endif /* QBI_UT_TEST_MSUICC || QBI_UT_TEST_UICC_RESET*/

    #ifdef QBI_UT_TEST_ATDS
    case QBI_SVC_ID_ATDS:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_atds, QBI_MBIM_UUID_LEN);
      break;
    #endif /* QBI_UT_TEST_ATDS */

    case QBI_SVC_ID_BC_EXT:
      QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                  qbi_ut_uuid_bc_ext, QBI_MBIM_UUID_LEN);
      break;

    default:
      QBI_LOG_E_1("Unsupported/unknown device service ID %d", svc_id);
  }

  req->cid = cid;
  req->cmd_type = cmd_type;
  req->infobuf_len = infobuf_len;
  if (infobuf_len > 0)
  {
    QBI_CHECK_NULL_PTR_RET_FALSE(infobuf);

    pkt_infobuf = ((uint8 *) req + sizeof(qbi_msg_cmd_req_s));
    QBI_MEMSCPY(pkt_infobuf, infobuf_len, infobuf, infobuf_len);
  }

  return TRUE;
} /* qbi_ut_build_cmd_req() */

/*===========================================================================
  FUNCTION: qbi_ut_cancel_pending_txn
===========================================================================*/
/*!
    @brief Cancels the currently pending transaction by issuing an
    MBIM_HOST_ERROR_MSG containing MBIM_ERROR_CANCEL for the last used
    transaction ID

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_cancel_pending_txn
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_msg_error_s *error_msg;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);
  QBI_CHECK_NULL_PTR_RET(state->ctx);

  qbi_util_buf_init(&buf);
  error_msg = (qbi_msg_error_s *) qbi_util_buf_alloc(
    &buf, sizeof(qbi_msg_error_s));
  QBI_CHECK_NULL_PTR_RET(error_msg);

  error_msg->hdr.msg_type = QBI_MSG_MBIM_HOST_ERROR_MSG;
  error_msg->hdr.msg_len  = sizeof(qbi_msg_error_s);
  error_msg->hdr.txn_id   = state->txn_id;
  error_msg->error        = QBI_MBIM_ERROR_CANCEL;

  qbi_log_pkt(state->ctx->id, QBI_LOG_PKT_DIRECTION_RX, error_msg,
              error_msg->hdr.msg_len);
  qbi_msg_input(state->ctx, (qbi_util_buf_const_s *) &buf);

  qbi_util_buf_free(&buf);
} /* qbi_ut_cancel_pending_txn() */

/*===========================================================================
  FUNCTION: qbi_ut_check_cmd_rsp
===========================================================================*/
/*!
    @brief Performs common checks of the fixed length command response
    header

    @details

    @param data
    @param data_len
    @param uuid Expected UUID
    @param cid Expected CID
    @param status Expected status code

    @return boolean TRUE if all checks passed, FALSE otherwise
*/
/*=========================================================================*/
static boolean qbi_ut_check_cmd_rsp
(
  const uint8 *data,
  uint32       data_len,
  const uint8 *uuid,
  uint32       cid,
  uint32       status
)
{
  qbi_msg_cmd_rsp_s *rsp;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(data);
  QBI_CHECK_NULL_PTR_RET_FALSE(uuid);

  rsp = (qbi_msg_cmd_rsp_s *) data;
  if (rsp->hdr.msg_type != QBI_MSG_MBIM_COMMAND_DONE)
  {
    QBI_LOG_E_2("Unexpected MessageType! Expected 0x%08x, received 0x%08x",
                QBI_MSG_MBIM_COMMAND_DONE, rsp->hdr.msg_type);
  }
  else if (data_len < sizeof(qbi_msg_cmd_rsp_s))
  {
    QBI_LOG_E_2("Message too short to be response! Expected at least %d bytes, "
                "received %d", sizeof(qbi_msg_cmd_rsp_s), data_len);
  }
  else if (QBI_MEMCMP(rsp->dev_svc_uuid, uuid, QBI_MBIM_UUID_LEN))
  {
    QBI_LOG_E_0("Received response with incorrect UUID");
  }
  else if (rsp->cid != cid)
  {
    QBI_LOG_E_2("Received CID %d, expected %d", rsp->cid, cid);
  }
  else if (rsp->status != status)
  {
    QBI_LOG_E_2("Received status %d, expected %d", rsp->status, status);
  }
  else if (rsp->total_frag != 1)
  {
    QBI_LOG_E_1("Message is fragmented into %d messages (not supported/expected"
                " in QBI UT)", rsp->total_frag);
  }
  else if (data_len != (sizeof(qbi_msg_cmd_rsp_s) + rsp->infobuf_len))
  {
    QBI_LOG_E_2("Message length doesn't match header + InformationBufferLen! "
                "Expected %d, received %d",
                (sizeof(qbi_msg_cmd_rsp_s) + rsp->infobuf_len), data_len);
  }
  else if (rsp->cur_frag != 0)
  {
    QBI_LOG_E_1("Unexpected CurrentFragment value! Expected 0, received %d",
                rsp->cur_frag);
  }
  else
  {
    success = TRUE;
  }

  return success;
} /* qbi_ut_check_cmd_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_cmd_cb
===========================================================================*/
/*!
    @brief Callback invoked to process a QBI UT step

    @details

    @param ctx
    @param cmd_id
    @param data
*/
/*=========================================================================*/
static void qbi_ut_cmd_cb
(
  qbi_ctx_s        *ctx,
  qbi_task_cmd_id_e cmd_id,
  void             *data
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(data);
  qbi_ut_run((qbi_ut_state_s *) data);
} /* qbi_ut_cmd_cb() */

/*===========================================================================
  FUNCTION: qbi_ut_post_cmd
===========================================================================*/
/*!
    @brief Posts a command to execute qbi_ut_run() in the QBI task

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_post_cmd
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);
  if (!qbi_task_cmd_send(state->ctx, QBI_TASK_CMD_ID_UT_CB, qbi_ut_cmd_cb,
                         (void *) state))
  {
    QBI_LOG_E_0("Couldn't post command to QBI task!");
  }
} /* qbi_ut_post_cmd() */

/*===========================================================================
  FUNCTION: qbi_ut_send_cmd_req
===========================================================================*/
/*!
    @brief Allocates, populates, and sends a MBIM_COMMAND_MSG

    @details

    @param state
    @param svc_id
    @param cid
    @param cmd_type
    @param infobuf_len
    @param infobuf
*/
/*=========================================================================*/
static void qbi_ut_send_cmd_req
(
  qbi_ut_state_s    *state,
  qbi_svc_id_e       svc_id,
  uint32             cid,
  uint32             cmd_type,
  uint32             infobuf_len,
  const void        *infobuf
)
{
  qbi_util_buf_s buf;
  qbi_msg_cmd_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);
  QBI_CHECK_NULL_PTR_RET(state->ctx);

  qbi_util_buf_init(&buf);
  req = (qbi_msg_cmd_req_s *) qbi_util_buf_alloc(
    &buf, (sizeof(qbi_msg_cmd_req_s) + infobuf_len));
  QBI_CHECK_NULL_PTR_RET(req);

  if (qbi_ut_build_cmd_req(state, req, svc_id, cid, cmd_type,
                           infobuf_len, infobuf))
  {
    qbi_log_pkt(state->ctx->id, QBI_LOG_PKT_DIRECTION_RX, req, req->hdr.msg_len);
    qbi_msg_input(state->ctx, (qbi_util_buf_const_s *) &buf);
  }
  qbi_util_buf_free(&buf);
} /* qbi_ut_send_cmd_req() */

/*===========================================================================
  FUNCTION: qbi_ut_timer_cb
===========================================================================*/
/*!
    @brief Timer callback

    @details

    @param data
*/
/*=========================================================================*/
static void qbi_ut_timeout_cb
(
  void *data
)
{
  qbi_ut_state_s *state;
/*-------------------------------------------------------------------------*/
  state = (qbi_ut_state_s *) data;
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_E_1("QBI UT hit timeout - didn't get expected response from QBI in "
              "state %d... continuing to next step", state->state);
  qbi_ut_post_cmd(state);
} /* qbi_ut_timeout_cb() */

/*===========================================================================
  FUNCTION: qbi_ut_timer_cb
===========================================================================*/
/*!
    @brief Timer callback

    @details

    @param data
*/
/*=========================================================================*/
static void qbi_ut_timer_cb
(
  void *data
)
{
  qbi_ut_state_s *state;
/*-------------------------------------------------------------------------*/
  state = (qbi_ut_state_s *) data;
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("QBI UT got timer callback");
  qbi_ut_post_cmd(state);
} /* qbi_ut_timer_cb() */

/*===========================================================================
  FUNCTION: qbi_ut_txn_id_matches
===========================================================================*/
/*!
    @brief Checks whether we have received a response to our currently
    outstanding request by attempting to match up the transaction ID

    @details

    @param buf
    @param buf_len

    @return boolean
*/
/*=========================================================================*/
static boolean qbi_ut_txn_id_matches
(
  const uint8 *buf,
  uint32       buf_len,
  uint32       expected_txn_id
)
{
  qbi_msg_common_hdr_s *hdr;
  boolean txn_id_matches = FALSE;
/*-------------------------------------------------------------------------*/
  if (buf_len < sizeof(qbi_msg_common_hdr_s))
  {
    QBI_LOG_E_2("Message length (%d) too small to contain mandatory header "
                "(size %d)!", buf_len, sizeof(qbi_msg_common_hdr_s));
  }
  else
  {
    hdr = (qbi_msg_common_hdr_s *) buf;
    if (!(hdr->msg_type & QBI_MSG_MBIM_DEVICE_TO_HOST_FLAG))
    {
      QBI_LOG_E_1("Message type 0x%08x doesn't have device to host flag set!",
                  hdr->msg_type);
    }
    else if (hdr->txn_id == expected_txn_id)
    {
      txn_id_matches = TRUE;
    }
    else if (hdr->txn_id != QBI_MSG_TXN_ID_IND)
    {
      QBI_LOG_W_2("Received unexpected non-event transaction ID %d (expecting "
                  "%d)", hdr->txn_id, expected_txn_id);
    }
  }

  return txn_id_matches;
} /* qbi_ut_txn_id_matches() */

/*===========================================================================
  FUNCTION: qbi_ut_tx_open_msg
===========================================================================*/
/*!
    @brief Send an MBIM_OPEN_MSG and setup to expect the response

    @details
*/
/*=========================================================================*/
static void qbi_ut_tx_open_msg
(
  qbi_ut_state_s *state
)
{
  qbi_msg_open_req_s *open_req;
  qbi_util_buf_s buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);
  QBI_CHECK_NULL_PTR_RET(state->ctx);

  qbi_util_buf_init(&buf);
  open_req = (qbi_msg_open_req_s *) qbi_util_buf_alloc(
    &buf, sizeof(qbi_msg_open_req_s));
  QBI_CHECK_NULL_PTR_RET(open_req);
  open_req->hdr.msg_type = QBI_MSG_MBIM_OPEN_MSG;
  open_req->hdr.txn_id = ++(state->txn_id);
  open_req->hdr.msg_len = sizeof(qbi_msg_open_req_s);
  open_req->max_xfer = QBI_UT_MAX_XFER;

  state->verify_cb = qbi_ut_verify_open_rsp;

  QBI_LOG_I_0("Sending MBIM_OPEN_MSG:");
  qbi_log_pkt(state->ctx->id, QBI_LOG_PKT_DIRECTION_RX, open_req,
              sizeof(qbi_msg_open_req_s));
  qbi_msg_input(state->ctx, (qbi_util_buf_const_s *) &buf);
  qbi_util_buf_free(&buf);
} /* qbi_ut_tx_open_msg() */

/*===========================================================================
  FUNCTION: qbi_ut_verify_open_rsp
===========================================================================*/
/*!
    @brief Verifies a MBIM_CLOSE_DONE response

    @details

    @param buf
    @param buf_len
*/
/*=========================================================================*/
static void qbi_ut_verify_open_rsp
(
  const uint8 *buf,
  uint32       buf_len
)
{
  qbi_msg_open_rsp_s *open_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);

  if (buf_len != sizeof(qbi_msg_open_rsp_s))
  {
    QBI_LOG_E_2("Invalid size for MBIM_OPEN_DONE! Expected %d, received "
                "%d", sizeof(qbi_msg_open_rsp_s), buf_len);
  }
  else
  {
    open_rsp = (qbi_msg_open_rsp_s *) buf;
    if (open_rsp->hdr.msg_type == QBI_MSG_MBIM_OPEN_DONE &&
        open_rsp->hdr.msg_len == sizeof(qbi_msg_open_rsp_s) &&
        open_rsp->status == QBI_MBIM_STATUS_SUCCESS)
    {
      QBI_LOG_I_0("Successfully verified MBIM_OPEN_DONE");
    }
    else
    {
      QBI_LOG_E_0("Open response failed validation");
    }
  }
} /* qbi_ut_verify_open_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_tx_close_msg
===========================================================================*/
/*!
    @brief Send an MBIM_CLOSE_MSG and setup to expect the response

    @details
*/
/*=========================================================================*/
static void qbi_ut_tx_close_msg
(
  qbi_ut_state_s *state
)
{
  qbi_msg_close_req_s *close_req;
  qbi_util_buf_s buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);
  QBI_CHECK_NULL_PTR_RET(state->ctx);

  qbi_util_buf_init(&buf);
  close_req = (qbi_msg_close_req_s *) qbi_util_buf_alloc(
    &buf, sizeof(qbi_msg_close_req_s));
  QBI_CHECK_NULL_PTR_RET(close_req);
  close_req->hdr.msg_type = QBI_MSG_MBIM_CLOSE_MSG;
  close_req->hdr.txn_id = ++(state->txn_id);
  close_req->hdr.msg_len = sizeof(qbi_msg_close_req_s);

  state->verify_cb = qbi_ut_verify_close_rsp;

  QBI_LOG_I_0("Sending MBIM_CLOSE_MSG:");
  qbi_log_pkt(state->ctx->id, QBI_LOG_PKT_DIRECTION_RX, close_req,
              sizeof(qbi_msg_close_req_s));
  qbi_msg_input(state->ctx, (qbi_util_buf_const_s *) &buf);
  qbi_util_buf_free(&buf);
} /* qbi_ut_tx_close_msg() */

/*===========================================================================
  FUNCTION: qbi_ut_verify_close_rsp
===========================================================================*/
/*!
    @brief Verifies a MBIM_CLOSE_DONE response

    @details

    @param buf
    @param buf_len
*/
/*=========================================================================*/
static void qbi_ut_verify_close_rsp
(
  const uint8 *buf,
  uint32       buf_len
)
{
  qbi_msg_close_rsp_s *close_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);

  if (buf_len != sizeof(qbi_msg_close_rsp_s))
  {
    QBI_LOG_E_2("Invalid size for MBIM_CLOSE_DONE! Expected %d, received "
                "%d", sizeof(qbi_msg_close_rsp_s), buf_len);
  }
  else
  {
    close_rsp = (qbi_msg_close_rsp_s *) buf;
    if (close_rsp->hdr.msg_type == QBI_MSG_MBIM_CLOSE_DONE &&
        close_rsp->hdr.msg_len == sizeof(qbi_msg_close_rsp_s) &&
        close_rsp->status == QBI_MBIM_STATUS_SUCCESS)
    {
      QBI_LOG_I_0("Successfully verified MBIM_CLOSE_DONE");
    }
    else
    {
      QBI_LOG_E_0("Close response failed validation");
    }
  }
} /* qbi_ut_verify_close_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_device_services_q_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_DEVICE_SERVICES query

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_device_services_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_DEVICE_SERVICES query");
  state->verify_cb = qbi_ut_bc_device_services_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_DEVICE_SERVICES,
    QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_bc_device_services_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_device_services_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_DEVICE_SERVICES response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_device_services_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc, QBI_SVC_BC_MBIM_CID_DEVICE_SERVICES,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_device_services_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_device_service_subscribe_list_s_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST set request

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_device_service_subscribe_list_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_device_service_subscribe_list_s_req_s *req;
  qbi_mbim_offset_size_pair_s *field_desc;
  qbi_svc_bc_device_service_subscribe_list_entry_s *entry;
  uint32 *cid;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(
    &buf, (sizeof(qbi_svc_bc_device_service_subscribe_list_s_req_s) +
           sizeof(qbi_mbim_offset_size_pair_s) +
           sizeof(qbi_svc_bc_device_service_subscribe_list_entry_s) +
           1 * sizeof(uint32)));
  QBI_CHECK_NULL_PTR_RET(req);
  field_desc = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) buf.data +
     sizeof(qbi_svc_bc_device_service_subscribe_list_s_req_s));
  entry = (qbi_svc_bc_device_service_subscribe_list_entry_s *)
    ((uint8 *) buf.data +
     sizeof(qbi_svc_bc_device_service_subscribe_list_s_req_s) +
     sizeof(qbi_mbim_offset_size_pair_s));

  req->element_count = 1;
  field_desc->offset =
    sizeof(qbi_svc_bc_device_service_subscribe_list_s_req_s) +
    sizeof(qbi_mbim_offset_size_pair_s);
  field_desc->size =
    sizeof(qbi_svc_bc_device_service_subscribe_list_entry_s) +
    1 * sizeof(uint32);

  #ifdef QBI_UT_TEST_EXT_QMUX
    QBI_MEMSCPY(entry->uuid, sizeof(entry->uuid),
                qbi_ut_uuid_ext_qmux, sizeof(qbi_ut_uuid_ext_qmux));
    entry->cid_count = 1;
    cid = (uint32 *)
      ((uint8 *) buf.data + 
       sizeof(qbi_svc_bc_device_service_subscribe_list_s_req_s) + 
       sizeof(qbi_mbim_offset_size_pair_s) + 
       sizeof(qbi_svc_bc_device_service_subscribe_list_entry_s));
    *cid = QBI_SVC_EXT_QMUX_MBIM_CID_QMUX_DATA;
  #else
    QBI_MEMSCPY(entry->uuid, sizeof(entry->uuid),
                qbi_ut_uuid_bc_ext, sizeof(qbi_ut_uuid_bc_ext));
    entry->cid_count = 1;
    cid = (uint32 *)
      ((uint8 *) buf.data + 
       sizeof(qbi_svc_bc_device_service_subscribe_list_s_req_s) + 
       sizeof(qbi_mbim_offset_size_pair_s) + 
       sizeof(qbi_svc_bc_device_service_subscribe_list_entry_s));
    *cid = QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO_STATUS;
  #endif /* QBI_UT_TEST_EXT_QMUX */

  QBI_LOG_I_0("Sending MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST set request");
  state->verify_cb = qbi_ut_bc_device_service_subscribe_list_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST,
    QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);

  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_device_service_subscribe_list_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_device_service_subscribe_list_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_DEVICE_SERVICES response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_device_service_subscribe_list_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_device_service_subscribe_list_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_device_caps_q_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_DEVICE_CAPS query

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_device_caps_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_DEVICE_CAPS query");
  state->verify_cb = qbi_ut_bc_device_caps_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_DEVICE_CAPS,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_bc_device_caps_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_device_caps_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_DEVICE_CAPS response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_device_caps_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc, QBI_SVC_BC_MBIM_CID_DEVICE_CAPS,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_device_caps_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_ready_info_q_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_SUBSCRIBER_READY_STATUS query

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_ready_info_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_SUBSCRIBER_READY_STATUS query");
  state->verify_cb = qbi_ut_bc_ready_info_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_bc_ready_info_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_ready_info_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_SUBSCRIBER_READY_STATUS response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_ready_info_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_ready_info_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_radio_state_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_RADIO_STATE query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_bc_radio_state_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_RADIO_STATE query");
  state->verify_cb = qbi_ut_bc_radio_state_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_RADIO_STATE,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_bc_radio_state_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_radio_state_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_RADIO_STATE set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_bc_radio_state_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_svc_bc_radio_state_s_req_s req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_RADIO_STATE set request for SwRadioOn");
  req.radio_state = QBI_SVC_BC_RADIO_STATE_RADIO_ON;

  state->verify_cb = qbi_ut_bc_radio_state_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_RADIO_STATE,
                      QBI_MSG_CMD_TYPE_SET, sizeof(req), &req);
} /* qbi_ut_bc_radio_state_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_radio_state_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_RADIO_STATE response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_radio_state_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_RADIO_STATE, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_radio_state_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_register_state_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_REGISTER_STATE query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_bc_register_state_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_REGISTER_STATE query");
  state->verify_cb = qbi_ut_bc_register_state_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_REGISTER_STATE,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_bc_register_state_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_register_state_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_REGISTER_STATE response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_register_state_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_REGISTER_STATE, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_register_state_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_packet_service_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_PACKET_SERVICE query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_bc_packet_service_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_PACKET_SERVICE query");
  state->verify_cb = qbi_ut_bc_packet_service_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_PACKET_SERVICE,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_bc_packet_service_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_packet_service_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PACKET_SERVICE response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_packet_service_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_PACKET_SERVICE, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_packet_service_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_visible_providers_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_VISIBLE_PROVIDERS query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_bc_visible_providers_q_req
(
  qbi_ut_state_s *state
)
{
  qbi_svc_bc_visible_providers_q_req_s req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_VISIBLE_PROVIDERS query");
  req.action = QBI_SVC_BC_VISIBLE_PROVIDERS_FULL_SCAN;
  state->verify_cb = qbi_ut_bc_visible_providers_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_VISIBLE_PROVIDERS,
                      QBI_MSG_CMD_TYPE_QUERY, sizeof(req), &req);
} /* qbi_ut_bc_visible_providers_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_visible_providers_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_VISIBLE_PROVIDERS response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_visible_providers_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_VISIBLE_PROVIDERS, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_visible_providers_verify_rsp() */

#ifdef QBI_UT_TEST_CONNECT
/*===========================================================================
  FUNCTION: qbi_ut_bc_connect_s_activate_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_CONNECT set request to connect a session

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_connect_s_activate_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_connect_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_1("Sending MBIM_CID_CONNECT set request (activate), session ID %d",
              qbi_ut_connect_current_session_id);
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_bc_connect_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->session_id = qbi_ut_connect_current_session_id;
  req->activation_cmd = QBI_SVC_BC_ACTIVATION_CMD_ACTIVATE;
  req->compression = QBI_SVC_BC_COMPRESSION_NONE;
  req->auth_protocol = QBI_SVC_BC_AUTH_PROTOCOL_NONE;
  req->ip_type = QBI_SVC_BC_IP_TYPE_DEFAULT;
  QBI_MEMSCPY(
    req->context_type, sizeof(req->context_type),
    qbi_svc_bc_context_type_id_to_uuid(QBI_SVC_BC_CONTEXT_TYPE_INTERNET),
    QBI_MBIM_UUID_LEN);
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->access_string,
    qbi_ut_connect_apn_names[qbi_ut_connect_current_session_id],
    QBI_STRLEN(qbi_ut_connect_apn_names[qbi_ut_connect_current_session_id]) + 1);
  req = (qbi_svc_bc_connect_s_req_s *)buf.data;

  state->verify_cb = qbi_ut_bc_connect_s_activate_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_CONNECT,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_connect_s_activate_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_connect_s_activate_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_CONNECT response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_connect_s_activate_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_CONNECT, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_connect_s_activate_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_ip_configuration_info_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_IP_CONFIGURATION_INFO query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_bc_ip_configuration_info_q_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_ip_configuration_info_rsp_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_1("Sending MBIM_CID_IP_CONFIGURATION_INFO query, session ID %d",
              qbi_ut_connect_current_session_id);
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_bc_ip_configuration_info_rsp_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->session_id = qbi_ut_connect_current_session_id;
  state->verify_cb = qbi_ut_bc_ip_configuration_info_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_IP_CONFIGURATION_INFO,
                      QBI_MSG_CMD_TYPE_QUERY, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_ip_configuration_info_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_ip_configuration_info_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_IP_CONFIGURATION_INFO response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_ip_configuration_info_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_IP_CONFIGURATION_INFO, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_ip_configuration_info_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_connect_s_deactivate_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_CONNECT set request to disconnect a session

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_connect_s_deactivate_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_connect_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_1("Sending MBIM_CID_CONNECT set request (deactivate) %d",
              qbi_ut_connect_current_session_id);
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_bc_connect_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->session_id = qbi_ut_connect_current_session_id;
  req->activation_cmd = QBI_SVC_BC_ACTIVATION_CMD_DEACTIVATE;

  state->verify_cb = qbi_ut_bc_connect_s_deactivate_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_CONNECT,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_connect_s_deactivate_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_connect_s_deactivate_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_CONNECT response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_connect_s_deactivate_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_CONNECT, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_connect_s_deactivate_verify_rsp() */
#endif /* QBI_UT_TEST_CONNECT */

#ifdef QBI_UT_TEST_EXT_QMUX
/*===========================================================================
  FUNCTION: qbi_ut_qmi_idl_decode
===========================================================================*/
/*!
    @brief Decodes QMI message data using the IDL API

    @details

    @param svc_obj
    @param qmi_msg_id
    @param idl_msg_type
    @param tlv_data
    @param tlv_data_size
    @param decoded_buf

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_ut_qmi_idl_decode
(
  qmi_idl_service_object_type  svc_obj,
  uint16                       qmi_msg_id,
  qmi_idl_type_of_message_type idl_msg_type,
  const uint8                 *tlv_data,
  uint32                       tlv_data_size,
  qbi_util_buf_s              *decoded_buf
)
{
  int idl_err;
  uint32_t decoded_size;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(tlv_data);
  QBI_CHECK_NULL_PTR_RET_FALSE(decoded_buf);

  idl_err = qmi_idl_get_message_c_struct_len(
    svc_obj, idl_msg_type, qmi_msg_id, &decoded_size);
  if (idl_err != QMI_NO_ERR)
  {
    QBI_LOG_E_1("Error %d while trying to get decoded size", idl_err);
  }
  else if (qbi_util_buf_alloc(decoded_buf, decoded_size) == NULL)
  {
    QBI_LOG_E_1("Couldn't allocate %d bytes for IDL decoding", decoded_size);
  }
  else
  {
    idl_err = qmi_idl_message_decode(
      svc_obj, idl_msg_type, qmi_msg_id, tlv_data, tlv_data_size,
      decoded_buf->data, decoded_buf->size);
    if (idl_err != QMI_NO_ERR)
    {
      QBI_LOG_E_1("Error %d while trying to decode QMI message", idl_err);
    }
    else
    {
      success = TRUE;
    }
  }

  return success;
} /* qbi_ut_qmi_idl_decode() */

/*===========================================================================
  FUNCTION: qbi_ut_ext_qmux_s_clid_alloc
===========================================================================*/
/*!
    @brief Sends a QMI_CTL_GET_CLIENT_ID_REQ (QMI DMS) via the EXT_QMUX
    device service

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_ext_qmux_s_clid_alloc
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_qmux_msg_s *qmux_msg;
  qbi_ut_qmi_tlv_s *tlv;
  uint8 *tlv_data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QMI_CTL_GET_CLIENT_ID request (DMS)");
  qbi_util_buf_init(&buf);
  qmux_msg = qbi_util_buf_alloc(
    &buf, (QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES + sizeof(qbi_ut_qmi_tlv_s) + 1));
  QBI_CHECK_NULL_PTR_RET(qmux_msg);

  qmux_msg->if_type = QBI_QMUX_IF_TYPE_QMUX;
  qmux_msg->qmux_hdr.length = buf.size - 1;
  qmux_msg->qmux_hdr.svc_type = QBI_QMUX_SVC_TYPE_QMI_CTL;
  qmux_msg->qmux_hdr.client_id = 0;
  qmux_msg->sdu.qmi_ctl.hdr.txn_id = 1;
  qmux_msg->sdu.qmi_ctl.msg.msg_id = QMI_CTL_GET_CLIENT_ID_REQ_V01;
  qmux_msg->sdu.qmi_ctl.msg.msg_length = 4;

  tlv = (qbi_ut_qmi_tlv_s *) ((uint8 *) qmux_msg +
                            QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES);
  tlv_data = (uint8 *) tlv + sizeof(qbi_ut_qmi_tlv_s);

  /* Request mandatory TLV: QMI service ID (set to 2 for QMI DMS) */
  tlv->type = 0x01;
  tlv->length = 1;
  *tlv_data = QBI_UT_QMI_SVC_ID_DMS;

  state->verify_cb = qbi_ut_ext_qmux_s_clid_alloc_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_EXT_QMUX,
                      QBI_SVC_EXT_QMUX_MBIM_CID_QMUX_DATA,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_ext_qmux_s_clid_alloc() */

/*===========================================================================
  FUNCTION: qbi_ut_ext_qmux_s_clid_alloc_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a QMI_CTL_GET_CLIENT_ID_REQ response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_ext_qmux_s_clid_alloc_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
  const qbi_msg_cmd_rsp_s *cid_rsp;
  const qbi_qmux_msg_s *qmux_msg;
  const uint8 *rsp_tlvs;
  uint32 rsp_tlvs_len;
  qbi_util_buf_s buf;
  ctl_get_client_id_resp_msg_v01 *qmi_rsp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(data);

  qbi_util_buf_init(&buf);
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_ext_qmux,
        QBI_SVC_EXT_QMUX_MBIM_CID_QMUX_DATA, QBI_MBIM_STATUS_SUCCESS))
  {
    cid_rsp = (const qbi_msg_cmd_rsp_s *) data;
    qmux_msg = (const qbi_qmux_msg_s *) (data + sizeof(qbi_msg_cmd_rsp_s));
    if (cid_rsp->infobuf_len < QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES)
    {
      QBI_LOG_E_2("InfoBuf too small: got %d, min %d", cid_rsp->infobuf_len,
                  QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES);
    }
    else if (qmux_msg->sdu.qmi_ctl.hdr.txn_id != 1)
    {
      QBI_LOG_E_1("Invalid QMI_CTL transaction ID: got %d, expected 1",
                  qmux_msg->sdu.qmi_ctl.hdr.txn_id);
    }
    else
    {
      rsp_tlvs = data + sizeof(qbi_msg_cmd_rsp_s) +
        QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES;
      rsp_tlvs_len = data_len - sizeof(qbi_msg_cmd_rsp_s) -
        QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES;

      if (qbi_ut_qmi_idl_decode(
            ctl_get_service_object_v01(), QMI_CTL_GET_CLIENT_ID_RESP_V01,
            QMI_IDL_RESPONSE, rsp_tlvs, rsp_tlvs_len, &buf))
      {
        qmi_rsp = (ctl_get_client_id_resp_msg_v01 *) buf.data;
        if (qmi_rsp->resp.result == QMI_RESULT_SUCCESS_V01)
        {
          qbi_ut_state.qmi_clids.dms = qmi_rsp->qmi_svc_type.client_id;
          QBI_LOG_I_1("QBI UT allocated QMI DMS client ID %d",
                      qbi_ut_state.qmi_clids.dms);
        }
        else
        {
          QBI_LOG_E_0("QBI couldn't allocate a client ID!");
        }
      }
    }
  }
  qbi_util_buf_free(&buf);
} /* qbi_ut_ext_qmux_s_clid_alloc_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_ext_qmux_s_set_event_report
===========================================================================*/
/*!
    @brief Sends a QMI_DMS_SET_EVENT_REPORT_REQ to register for operating
    mode change indications

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_ext_qmux_s_set_event_report
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_qmux_msg_s *qmux_msg;
  qbi_ut_qmi_tlv_s *tlv;
  uint8 *tlv_data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QMI_DMS_SET_EVENT_REPORT request");
  qbi_util_buf_init(&buf);
  qmux_msg = qbi_util_buf_alloc(
    &buf, (QBI_QMUX_NON_QMI_CTL_MIN_MSG_LEN_BYTES +
           sizeof(qbi_ut_qmi_tlv_s) + 1));
  QBI_CHECK_NULL_PTR_RET(qmux_msg);

  qmux_msg->if_type = QBI_QMUX_IF_TYPE_QMUX;
  qmux_msg->qmux_hdr.length = buf.size - 1;
  qmux_msg->qmux_hdr.svc_type = QBI_UT_QMI_SVC_ID_DMS;
  qmux_msg->qmux_hdr.client_id = state->qmi_clids.dms;
  qmux_msg->sdu.qmux.hdr.txn_id = 1;
  qmux_msg->sdu.qmux.msg.msg_id = QMI_DMS_SET_EVENT_REPORT_REQ_V01;
  qmux_msg->sdu.qmux.msg.msg_length = 4;

  tlv = (qbi_ut_qmi_tlv_s *) ((uint8 *) qmux_msg +
                            QBI_QMUX_NON_QMI_CTL_MIN_MSG_LEN_BYTES);
  tlv_data = (uint8 *) tlv + sizeof(qbi_ut_qmi_tlv_s);

  /* Request optional TLV: Operating Mode Reporting */
  tlv->type = 0x14;
  tlv->length = 1;
  *tlv_data = TRUE;

  state->verify_cb = qbi_ut_ext_qmux_s_qmi_generic_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_EXT_QMUX,
                      QBI_SVC_EXT_QMUX_MBIM_CID_QMUX_DATA,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_ext_qmux_s_set_event_report() */

/*===========================================================================
  FUNCTION: qbi_ut_ext_qmux_s_set_oprt_mode
===========================================================================*/
/*!
    @brief Sends a QMI_DMS_SET_OPERATING_MODE_REQ via the EXT_QMUX
    device service

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_ext_qmux_s_set_oprt_mode
(
  qbi_ut_state_s *state,
  uint8           operating_mode
)
{
  qbi_util_buf_s buf;
  qbi_qmux_msg_s *qmux_msg;
  qbi_ut_qmi_tlv_s *tlv;
  uint8 *tlv_data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_1("Sending QMI_DMS_SET_OPERATING_MODE request (%d)",
              operating_mode);
  qbi_util_buf_init(&buf);
  qmux_msg = qbi_util_buf_alloc(
    &buf, (QBI_QMUX_NON_QMI_CTL_MIN_MSG_LEN_BYTES +
           sizeof(qbi_ut_qmi_tlv_s) + 1));
  QBI_CHECK_NULL_PTR_RET(qmux_msg);

  qmux_msg->if_type = QBI_QMUX_IF_TYPE_QMUX;
  qmux_msg->qmux_hdr.length = buf.size - 1;
  qmux_msg->qmux_hdr.svc_type = QBI_UT_QMI_SVC_ID_DMS;
  qmux_msg->qmux_hdr.client_id = state->qmi_clids.dms;
  qmux_msg->sdu.qmux.hdr.txn_id = 1;
  qmux_msg->sdu.qmux.msg.msg_id = QMI_DMS_SET_OPERATING_MODE_REQ_V01;
  qmux_msg->sdu.qmux.msg.msg_length = 4;

  tlv = (qbi_ut_qmi_tlv_s *) ((uint8 *) qmux_msg +
                            QBI_QMUX_NON_QMI_CTL_MIN_MSG_LEN_BYTES);
  tlv_data = (uint8 *) tlv + sizeof(qbi_ut_qmi_tlv_s);

  /* Request mandatory TLV: Operating Mode */
  tlv->type = 0x01;
  tlv->length = 1;
  *tlv_data = operating_mode;

  state->verify_cb = qbi_ut_ext_qmux_s_qmi_generic_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_EXT_QMUX,
                      QBI_SVC_EXT_QMUX_MBIM_CID_QMUX_DATA,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_ext_qmux_s_set_oprt_mode() */

/*===========================================================================
  FUNCTION: qbi_ut_ext_qmux_wait_for_ind
===========================================================================*/
/*!
    @brief Pauses QBI_UT for 8 seconds to allow for a LPM operating mode
    indication to come to the device

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_ext_qmux_wait_for_ind
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  qbi_os_timer_set(&state->timer, 8000);
  QBI_LOG_I_0("Pausing for 8 seconds to allow manual verifiction of QMI "
              "indication");
} /* qbi_ut_ext_qmux_wait_for_ind() */

/*===========================================================================
  FUNCTION: qbi_ut_ext_qmux_s_clid_dealloc
===========================================================================*/
/*!
    @brief Sends a QMI_CTL_RELEASE_CLIENT_ID_REQ (QMI DMS) via the EXT_QMUX
    device service

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_ext_qmux_s_clid_dealloc
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_qmux_msg_s *qmux_msg;
  qbi_ut_qmi_tlv_s *tlv;
  uint8 *tlv_data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QMI_CTL_RELEASE_CLIENT_ID_REQ request (DMS)");
  qbi_util_buf_init(&buf);
  qmux_msg = qbi_util_buf_alloc(
    &buf, (QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES + sizeof(qbi_ut_qmi_tlv_s) + 2));
  QBI_CHECK_NULL_PTR_RET(qmux_msg);

  qmux_msg->if_type = QBI_QMUX_IF_TYPE_QMUX;
  qmux_msg->qmux_hdr.length = buf.size - 1;
  qmux_msg->qmux_hdr.svc_type = QBI_QMUX_SVC_TYPE_QMI_CTL;
  qmux_msg->qmux_hdr.client_id = 0;
  qmux_msg->sdu.qmi_ctl.hdr.txn_id = 2;
  qmux_msg->sdu.qmi_ctl.msg.msg_id = QMI_CTL_RELEASE_CLIENT_ID_REQ_V01;
  qmux_msg->sdu.qmi_ctl.msg.msg_length = 5;

  tlv = (qbi_ut_qmi_tlv_s *) ((uint8 *) qmux_msg +
                            QBI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES);
  tlv_data = (uint8 *) tlv + sizeof(qbi_ut_qmi_tlv_s);

  /* Request mandatory TLV: QMI service ID (set to 2 for QMI DMS) */
  tlv->type = 0x01;
  tlv->length = 2;
  *tlv_data++ = QBI_UT_QMI_SVC_ID_DMS;
  *tlv_data = state->qmi_clids.dms;

  state->verify_cb = qbi_ut_ext_qmux_s_qmi_generic_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_EXT_QMUX,
                      QBI_SVC_EXT_QMUX_MBIM_CID_QMUX_DATA,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_ext_qmux_s_clid_dealloc() */

/*===========================================================================
  FUNCTION: qbi_ut_ext_qmux_s_qmi_generic_verify_rsp
===========================================================================*/
/*!
    @brief Performs generic (CID level only) verification of the contents
    of a MBIM_CID_QMUX_DATA set response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_ext_qmux_s_qmi_generic_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_ext_qmux,
        QBI_SVC_EXT_QMUX_MBIM_CID_QMUX_DATA, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_ext_qmux_s_qmi_generic_verify_rsp() */
#endif /* QBI_UT_TEST_EXT_QMUX */

#ifdef QBI_UT_TEST_USSD
/*===========================================================================
  FUNCTION: qbi_ut_tx_ussd_orig_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_USSD set command

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_tx_ussd_orig_req
(
  qbi_ut_state_s *state
)
{
  qbi_msg_cmd_req_s *req;
  qbi_util_buf_s buf;
  qbi_svc_ussd_s_req_s *infobuf;
  uint8 *req_buf;
/*-------------------------------------------------------------------------*/
  qbi_util_buf_init(&buf);
  req_buf = qbi_util_buf_alloc(&buf, (sizeof(qbi_msg_cmd_req_s) +
                                  sizeof(qbi_svc_ussd_s_req_s)));
  QBI_CHECK_NULL_PTR_RET(req_buf);
  req = (qbi_msg_cmd_req_s *) req_buf;
  infobuf = (qbi_svc_ussd_s_req_s *) (req_buf + sizeof(qbi_msg_cmd_req_s));

  req->hdr.msg_type = QBI_MSG_MBIM_COMMAND_MSG;
  req->hdr.txn_id = ++(state->txn_id);
  req->hdr.msg_len = sizeof(qbi_msg_cmd_req_s) +
    sizeof(qbi_svc_ussd_s_req_s);


  req->total_frag = 1;
  req->cur_frag = 0;
  QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
              qbi_ut_uuid_ussd, QBI_MBIM_UUID_LEN);
  req->cid = (uint32) QBI_SVC_USSD_CID_MBB_USSD;
  req->cmd_type = QBI_MSG_CMD_TYPE_SET;
  req->infobuf_len = sizeof(qbi_svc_ussd_s_req_s);
  infobuf->ussd_action = QBI_SVC_USSD_ACTION_INITIATE;
  infobuf->ussd_data_coding_scheme = 0;
  QBI_MEMSCPY(infobuf->ussd_string, sizeof(infobuf->ussd_string),
              QBI_UT_USSD_ORIG_STRING_VOD,
              std_strlen(QBI_UT_USSD_ORIG_STRING_VOD));
  infobuf->ussd_string_len = std_strlen(QBI_UT_USSD_ORIG_STRING_VOD);

  /* Don't verify response for now.. */
  state->rsp_len = 0;
  state->verify_len = 0;

  QBI_LOG_I_0("Sending MBIM_CID_USSD Origination:");
  qbi_log_pkt(state->ctx->id, QBI_LOG_PKT_DIRECTION_RX, req_buf,
              req->hdr.msg_len);
  qbi_msg_input(state->ctx, (qbi_util_buf_const_s *) &buf);
  qbi_util_buf_free(&buf);
} /* qbi_ut_tx_ussd_orig_req() */



/*===========================================================================
  FUNCTION: qbi_ut_tx_ussd_orig_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_USSD set command

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_tx_ussd_answer_req
(
  qbi_ut_state_s *state
)
{
    qbi_msg_cmd_req_s *req;
    qbi_msg_cmd_rsp_s *rsp;
    qbi_util_buf_s buf;
    qbi_svc_ussd_s_req_s *infobuf;
    uint8 *req_buf;
  /*-------------------------------------------------------------------------*/
    qbi_util_buf_init(&buf);
    req_buf = qbi_util_buf_alloc(&buf, (sizeof(qbi_msg_cmd_req_s) +
                                    sizeof(qbi_svc_ussd_s_req_s)));
    QBI_CHECK_NULL_PTR_RET(req_buf);
    req = (qbi_msg_cmd_req_s *) req_buf;
    infobuf = (qbi_svc_ussd_s_req_s *) (req_buf + sizeof(qbi_msg_cmd_req_s));

    req->hdr.msg_type = QBI_MSG_MBIM_COMMAND_MSG;
    req->hdr.txn_id = ++(state->txn_id);
    req->hdr.msg_len = sizeof(qbi_msg_cmd_req_s) +
      sizeof(qbi_svc_ussd_s_req_s);


    req->total_frag = 1;
    req->cur_frag = 0;
    QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                qbi_ut_uuid_ussd, QBI_MBIM_UUID_LEN);
    req->cid = (uint32) QBI_SVC_USSD_CID_MBB_USSD;
    req->cmd_type = QBI_MSG_CMD_TYPE_SET;
    req->infobuf_len = sizeof(qbi_svc_ussd_s_req_s);
    infobuf->ussd_action = QBI_SVC_USSD_ACTION_CONTINUE;
    infobuf->ussd_data_coding_scheme = 0;
    QBI_MEMSCPY(infobuf->ussd_string, sizeof(infobuf->ussd_string), "1", 1);
    infobuf->ussd_string_len = 1;


    rsp = (qbi_msg_cmd_rsp_s *) state->rsp_buf;
    rsp->hdr.msg_type = QBI_MSG_MBIM_COMMAND_DONE;
    rsp->hdr.txn_id = req->hdr.txn_id;
    rsp->hdr.msg_len = sizeof(qbi_msg_cmd_rsp_s) + sizeof(qbi_svc_ussd_rsp_s);
    rsp->total_frag = 1;
    rsp->cur_frag = 0;
    QBI_MEMSCPY(rsp->dev_svc_uuid, sizeof(rsp->dev_svc_uuid),
                qbi_ut_uuid_ussd, QBI_MBIM_UUID_LEN);
    rsp->cid = 1;
    rsp->status = QBI_MBIM_STATUS_SUCCESS;
    rsp->infobuf_len = sizeof(qbi_svc_ussd_rsp_s);

    state->rsp_len = rsp->hdr.msg_len;
    state->verify_len = sizeof(qbi_msg_cmd_rsp_s);

    QBI_LOG_I_0("Sending MBIM_CID_USSD Answer:");
    qbi_log_pkt(state->ctx->id, QBI_LOG_PKT_DIRECTION_RX, req_buf,
                req->hdr.msg_len);
    qbi_msg_input(state->ctx, (qbi_util_buf_const_s *) &buf);
    qbi_util_buf_free(&buf);
} /* qbi_ut_tx_ussd_answer_req() */



/*===========================================================================
  FUNCTION: qbi_ut_tx_ussd_cancel_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_USSD set command

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_tx_ussd_cancel_req
(
  qbi_ut_state_s *state
)
{
    qbi_msg_cmd_req_s *req;
    qbi_msg_cmd_rsp_s *rsp;
    qbi_util_buf_s buf;
    qbi_svc_ussd_s_req_s *infobuf;
    uint8 *req_buf;
  /*-------------------------------------------------------------------------*/
    qbi_util_buf_init(&buf);
    req_buf = qbi_util_buf_alloc(&buf, (sizeof(qbi_msg_cmd_req_s) +
                                    sizeof(qbi_svc_ussd_s_req_s)));
    QBI_CHECK_NULL_PTR_RET(req_buf);
    req = (qbi_msg_cmd_req_s *) req_buf;
    infobuf = (qbi_svc_ussd_s_req_s *) (req_buf + sizeof(qbi_msg_cmd_req_s));

    req->hdr.msg_type = QBI_MSG_MBIM_COMMAND_MSG;
    req->hdr.txn_id = ++(state->txn_id);
    req->hdr.msg_len = sizeof(qbi_msg_cmd_req_s) +
      sizeof(qbi_svc_ussd_s_req_s);


    req->total_frag = 1;
    req->cur_frag = 0;
    QBI_MEMSCPY(req->dev_svc_uuid, sizeof(req->dev_svc_uuid),
                qbi_ut_uuid_ussd, QBI_MBIM_UUID_LEN);
    req->cid = (uint32) QBI_SVC_USSD_CID_MBB_USSD;
    req->cmd_type = QBI_MSG_CMD_TYPE_SET;
    req->infobuf_len = sizeof(qbi_svc_ussd_s_req_s);
    infobuf->ussd_action = QBI_SVC_USSD_ACTION_CANCEL;
    infobuf->ussd_data_coding_scheme = 1;
    infobuf->ussd_string_len = 0;


    rsp = (qbi_msg_cmd_rsp_s *) state->rsp_buf;
    rsp->hdr.msg_type = QBI_MSG_MBIM_COMMAND_DONE;
    rsp->hdr.txn_id = req->hdr.txn_id;
    rsp->hdr.msg_len = sizeof(qbi_msg_cmd_rsp_s) + sizeof(qbi_svc_ussd_rsp_s);
    rsp->total_frag = 1;
    rsp->cur_frag = 0;
    QBI_MEMSCPY(rsp->dev_svc_uuid, sizeof(rsp->dev_svc_uuid),
                qbi_ut_uuid_ussd, QBI_MBIM_UUID_LEN);
    rsp->cid = 1;
    rsp->status = QBI_MBIM_STATUS_SUCCESS;
    rsp->infobuf_len = sizeof(qbi_svc_ussd_rsp_s);

    state->rsp_len = rsp->hdr.msg_len;
    state->verify_len = sizeof(qbi_msg_cmd_rsp_s);

    QBI_LOG_I_0("Sending MBIM_CID_USSD Cancel:");
    qbi_log_pkt(state->ctx.id, QBI_LOG_PKT_DIRECTION_RX, req_buf,
                req->hdr.msg_len);
    qbi_msg_input(state->ctx, (qbi_util_buf_const_s *) &buf);
    qbi_util_buf_free(&buf);
} /* qbi_ut_tx_ussd_cancel_req() */
#endif /* QBI_UT_TEST_USSD */

#ifdef QBI_UT_TEST_STK
/*===========================================================================
  FUNCTION: qbi_ut_stk_pac_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_STK_PAC query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_stk_pac_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_STK_MBIM_CID_STK_PAC query");
  state->verify_cb = qbi_ut_stk_pac_q_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_STK, QBI_SVC_STK_MBIM_CID_STK_PAC,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_stk_pac_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_stk_pac_q_rsp
===========================================================================*/
/*!
    @brief Performs verification of a QBI_SVC_STK_MBIM_CID_STK_PAC response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_stk_pac_q_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_stk, QBI_SVC_STK_MBIM_CID_STK_PAC,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_stk_pac_q_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_stk_pac_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_STK_PAC set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_stk_pac_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_stk_pac_s_req_s *req;
  uint8 pac_host_control[QBI_SVC_STK_PAC_HOST_CONTROL_LEN];
  uint8 pac_type_val = 25; /* SET UP MENU */
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_STK_MBIM_CID_STK_PAC set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_stk_pac_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  QBI_MEMSET(pac_host_control, 0, QBI_SVC_STK_PAC_HOST_CONTROL_LEN);
  pac_host_control[pac_type_val/8] |= 1 << (pac_type_val % 8);

  QBI_MEMSCPY(req->pac_host_control, sizeof(req->pac_host_control),
              pac_host_control, QBI_SVC_STK_PAC_HOST_CONTROL_LEN);

  state->verify_cb = qbi_ut_stk_pac_s_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_STK,
                      QBI_SVC_STK_MBIM_CID_STK_PAC,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_stk_pac_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_stk_pac_s_rsp
===========================================================================*/
/*!
    @brief Performs verification of a QBI_SVC_STK_MBIM_CID_STK_PAC set
    response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_stk_pac_s_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_stk, QBI_SVC_STK_MBIM_CID_STK_PAC,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_stk_pac_s_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_stk_tr_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_STK_TERMINAL_RESPONSE set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_stk_tr_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_stk_terminal_response_s_req_s *req;
  /* USER_ACTIVITY_EVENT_EVENT_DOWNLOAD_IDLE_SCREEN_27.22.7.6.1 */
  uint8 tr_request_length = 12;
  uint8 tr_request[32] = {
    0x81, 0x03, 0x01, 0x05, 0x00, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_STK_MBIM_CID_STK_TERMINAL_RESPONSE set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(
    &buf, sizeof(qbi_svc_stk_terminal_response_s_req_s) + tr_request_length - 1);
  QBI_CHECK_NULL_PTR_RET(req);

  req->response_length = tr_request_length;
  QBI_MEMSCPY(req->data_buffer, tr_request_length, tr_request, tr_request_length);

  state->verify_cb = qbi_ut_stk_tr_s_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_STK,
                      QBI_SVC_STK_MBIM_CID_STK_TERMINAL_RESPONSE,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_stk_tr_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_stk_tr_s_rsp
===========================================================================*/
/*!
    @brief Performs verification of a
    QBI_SVC_STK_MBIM_CID_STK_TERMINAL_RESPONSE set response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_stk_tr_s_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_stk,
        QBI_SVC_STK_MBIM_CID_STK_TERMINAL_RESPONSE, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_stk_tr_s_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_stk_env_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_STK_ENVELOPE query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_stk_env_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_STK_MBIM_CID_STK_ENVELOPE query");
  state->verify_cb = qbi_ut_stk_env_q_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_STK, QBI_SVC_STK_MBIM_CID_STK_ENVELOPE,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_stk_env_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_stk_env_q_rsp
===========================================================================*/
/*!
    @brief Performs verification of a QBI_SVC_STK_MBIM_CID_STK_ENVELOPE response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_stk_env_q_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_stk, QBI_SVC_STK_MBIM_CID_STK_ENVELOPE,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_stk_env_q_rsp() */



/*===========================================================================
  FUNCTION: qbi_ut_stk_env_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_STK_ENVELOPE set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_stk_env_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_stk_envelope_s_req_s *req;
  /* USER_ACTIVITY_EVENT_EVENT_DOWNLOAD_IDLE_SCREEN_27.22.7.6.1 */
  uint8 envelope_length = 9;
  uint8 envelope_data[32] = {
    0xD6, 0x07, 0x19, 0x01, 0x05, 0x82, 0x02, 0x02, 0x81};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_STK_MBIM_CID_STK_ENVELOPE set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, envelope_length);
  QBI_CHECK_NULL_PTR_RET(req);

  QBI_MEMSCPY(req, envelope_length, envelope_data, envelope_length);
  state->verify_cb = qbi_ut_stk_env_s_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_STK,
                      QBI_SVC_STK_MBIM_CID_STK_ENVELOPE,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_stk_env_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_stk_env_s_rsp
===========================================================================*/
/*!
    @brief Performs verification of a
    QBI_SVC_STK_MBIM_CID_STK_ENVELOPE set response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_stk_env_s_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_stk,
        QBI_SVC_STK_MBIM_CID_STK_ENVELOPE, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_stk_env_s_rsp() */
#endif /* QBI_UT_TEST_STK */

#ifdef QBI_UT_TEST_PB
/*===========================================================================
  FUNCTION: qbi_ut_pb_config_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_PB_CONFIGURATION query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_pb_config_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_PB_CONFIGURATION query");
  state->verify_cb = qbi_ut_pb_config_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_PB,
                      QBI_SVC_PB_MBIM_CID_PHONEBOOK_CONFIGURATION,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_pb_config_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_pb_config_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PB_CONFIGURATION response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_pb_config_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
  const qbi_svc_pb_configuration_info_q_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_pb,
        QBI_SVC_PB_MBIM_CID_PHONEBOOK_CONFIGURATION, QBI_MBIM_STATUS_SUCCESS))
  {
    rsp = (const qbi_svc_pb_configuration_info_q_rsp_s *)
      (data + sizeof(qbi_msg_cmd_rsp_s));
    if (rsp->state == QBI_SVC_PB_STATE_NOT_INITIALIZED)
    {
      QBI_LOG_W_0("Phonebook not initialized");
    }
    else
    {
      QBI_LOG_I_4("Phonebook initialized with %d/%d used entries, max name/num "
                  "length %d/%d", rsp->used_entries, rsp->total_entries,
                  rsp->max_name_length, rsp->max_num_length);
    }
  }
} /* qbi_ut_pb_config_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_pb_delete_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_PHONEBOOK_DELETE set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_pb_delete_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_svc_pb_delete_s_req_s req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  req.filter_flag  = QBI_SVC_PB_FILTER_FLAG_ALL;
  req.filter_index = QBI_SVC_PB_INDEX_NONE;

  QBI_LOG_I_0("Sending MBIM_CID_PHONEBOOK_DELETE set");
  state->verify_cb = qbi_ut_pb_delete_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_PB,
                      QBI_SVC_PB_MBIM_CID_PHONEBOOK_DELETE,
                      QBI_MSG_CMD_TYPE_SET, sizeof(req), &req);
} /* qbi_ut_pb_delete_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_pb_delete_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PHONEBOOK_DELETE response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_pb_delete_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_pb,
        QBI_SVC_PB_MBIM_CID_PHONEBOOK_DELETE, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_pb_delete_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_pb_write_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_PB_WRITE set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_pb_write_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_svc_pb_write_s_req_s *req;
  qbi_util_buf_s buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_pb_write_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->save_flag = QBI_SVC_PB_WRITE_FLAG_SAVE_UNUSED;
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->number, "18588457445", 12);
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->name, "Test Entry", 11);

  QBI_LOG_I_0("Sending MBIM_CID_PHONEBOOK_WRITE set");
  state->verify_cb = qbi_ut_pb_write_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_PB,
                      QBI_SVC_PB_MBIM_CID_PHONEBOOK_WRITE,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_pb_write_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_pb_write_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PHONEBOOK_WRITE response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_pb_write_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_pb,
        QBI_SVC_PB_MBIM_CID_PHONEBOOK_WRITE, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_pb_write_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_pb_read_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_PHONEBOOK_READ query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_pb_read_q_req
(
  qbi_ut_state_s *state
)
{
  qbi_svc_pb_read_q_req_s req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  req.filter_flag  = QBI_SVC_PB_FILTER_FLAG_ALL;
  req.filter_index = QBI_SVC_PB_INDEX_NONE;

  QBI_LOG_I_0("Sending MBIM_CID_PHONEBOOK_READ query");
  state->verify_cb = qbi_ut_pb_read_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_PB,
                      QBI_SVC_PB_MBIM_CID_PHONEBOOK_READ,
                      QBI_MSG_CMD_TYPE_QUERY, sizeof(req), &req);
} /* qbi_ut_pb_read_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_pb_read_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PHONEBOOK_READ response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_pb_read_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
  const qbi_svc_bc_read_q_rsp_s *rsp;
  const qbi_mbim_offset_size_pair_s *field;
  const qbi_svc_pb_entry_s *entry;
  const uint8 *str_utf16;
  char str_ascii[256];
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_pb,
        QBI_SVC_PB_MBIM_CID_PHONEBOOK_READ, QBI_MBIM_STATUS_SUCCESS))
  {
    rsp = (const qbi_svc_bc_read_q_rsp_s *)
      (data + sizeof(qbi_msg_cmd_rsp_s));
    field = (const qbi_mbim_offset_size_pair_s *)
      (data + sizeof(qbi_msg_cmd_rsp_s) + sizeof(qbi_svc_bc_read_q_rsp_s));
    entry = (const qbi_svc_pb_entry_s *)
      (data + sizeof(qbi_msg_cmd_rsp_s) + field->offset);
    if (rsp->element_count != 1)
    {
      QBI_LOG_E_2("Unexpected ElementCount: got %d, expected %d",
                  rsp->element_count, 1);
    }
    else
    {
      QBI_LOG_I_1("Entry is at index %d", entry->index);

      str_utf16 = ((uint8 *) entry + entry->name.offset);
      qbi_util_utf16_to_ascii(str_utf16, entry->name.size,
                              str_ascii, sizeof(str_ascii));
      QBI_LOG_STR_1("Name is '%s'", str_ascii);

      str_utf16 = ((uint8 *) entry + entry->number.offset);
      qbi_util_utf16_to_ascii(str_utf16, entry->number.size,
                              str_ascii, sizeof(str_ascii));
      QBI_LOG_STR_1("Number is '%s'", str_ascii);
    }
  }
} /* qbi_ut_pb_read_verify_rsp() */
#endif /* QBI_UT_TEST_PB */

#ifdef QBI_UT_TEST_AUTH
/*===========================================================================
  FUNCTION: qbi_ut_auth_aka_q_req
===========================================================================*/
/*!
    @brief Send a AUTH AKA query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_auth_aka_q_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_auth_aka_auth_q_req_s *req;
  uint8 rand[QBI_SVC_AUTH_RAND_LEN] = {
    0x81, 0xe9, 0x2b, 0x6c, 0x0e, 0xe0, 0xe1, 0x2e,
    0xbc, 0xeb, 0xa8, 0xd9, 0x2a, 0x99, 0xdf, 0xa5};
  uint8 autn[QBI_SVC_AUTH_AUTN_LEN] = {
    0xbb, 0x52, 0xe9, 0x1c, 0x74, 0x7a, 0xc3, 0xab,
    0x2a, 0x5c, 0x23, 0xd1, 0x5e, 0xe3, 0x51, 0xd5};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_AUTH_MBIM_CID_AKA_AUTH set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_auth_aka_auth_q_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  QBI_MEMSCPY(req->rand, sizeof(req->rand), rand, QBI_SVC_AUTH_RAND_LEN);
  QBI_MEMSCPY(req->autn, sizeof(req->autn), autn, QBI_SVC_AUTH_AUTN_LEN);
  state->verify_cb = qbi_ut_auth_aka_q_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_AUTH,
                      QBI_SVC_AUTH_MBIM_CID_AKA_AUTH,
                      QBI_MSG_CMD_TYPE_QUERY, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_auth_aka_q_req */

/*===========================================================================
  FUNCTION: qbi_ut_auth_aka_q_rsp
===========================================================================*/
/*!
    @brief Performs verification of a QBI_SVC_AUTH_MBIM_CID_AKA_AUTH response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_auth_aka_q_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_auth, QBI_SVC_AUTH_MBIM_CID_AKA_AUTH,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_auth_aka_q_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_auth_akap_q_req
===========================================================================*/
/*!
    @brief Send a AUTH AKAP query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_auth_akap_q_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_auth_akap_auth_q_req_s *req;
  qbi_mbim_offset_size_pair_s *field_desc;
  uint8 *network_name_p;
  uint8 rand[QBI_SVC_AUTH_RAND_LEN] = {
    0x81, 0xe9, 0x2b, 0x6c, 0x0e, 0xe0, 0xe1, 0x2e,
    0xbc, 0xeb, 0xa8, 0xd9, 0x2a, 0x99, 0xdf, 0xa5};
  uint8 autn[QBI_SVC_AUTH_AUTN_LEN] = {
    0xbb, 0x52, 0xe9, 0x1c, 0x74, 0x7a, 0xc3, 0xab,
    0x2a, 0x5c, 0x23, 0xd1, 0x5e, 0xe3, 0x51, 0xd5};
  uint8 network_name[4] = "WLAN";
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_AUTH_MBIM_CID_AKAP_AUTH set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_auth_akap_auth_q_req_s) +
                           sizeof(qbi_mbim_offset_size_pair_s) +
                           QBI_STRLEN(network_name));
  QBI_CHECK_NULL_PTR_RET(req);

  field_desc = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) buf.data + sizeof(qbi_svc_auth_akap_auth_q_req_s));

  network_name_p = (uint8 *)
     ((uint8 *) buf.data + sizeof(qbi_svc_auth_akap_auth_q_req_s) +
     sizeof(qbi_mbim_offset_size_pair_s));

  QBI_MEMSCPY(req->rand, sizeof(req->rand), rand, QBI_SVC_AUTH_RAND_LEN);
  QBI_MEMSCPY(req->autn, sizeof(req->autn), autn, QBI_SVC_AUTH_AUTN_LEN);
  field_desc->offset =
    sizeof(qbi_svc_auth_akap_auth_q_req_s) +
    sizeof(qbi_mbim_offset_size_pair_s);
  field_desc->size = QBI_STRLEN(network_name);
  QBI_MEMSCPY(network_name_p, field_desc->size, network_name, field_desc->size);

  state->verify_cb = qbi_ut_auth_akap_q_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_AUTH,
                      QBI_SVC_AUTH_MBIM_CID_AKAP_AUTH,
                      QBI_MSG_CMD_TYPE_QUERY, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_auth_aka_q_req */

/*===========================================================================
  FUNCTION: qbi_ut_auth_akap_q_rsp
===========================================================================*/
/*!
    @brief Performs verification of a QBI_SVC_AUTH_MBIM_CID_AKAP_AUTH response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_auth_akap_q_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_auth, QBI_SVC_AUTH_MBIM_CID_AKAP_AUTH,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_auth_akap_q_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_auth_sim_q_req
===========================================================================*/
/*!
    @brief Send a AUTH SIM query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_auth_sim_q_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_auth_sim_auth_q_req_s *req;
  uint8  rand1[QBI_SVC_AUTH_RAND_LEN] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  uint8  rand2[QBI_SVC_AUTH_RAND_LEN] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};
  uint8  rand3[QBI_SVC_AUTH_RAND_LEN] = {
    0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99,
    0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_AUTH_MBIM_CID_SIM_AUTH set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_auth_sim_auth_q_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  QBI_MEMSCPY(req->rand1, sizeof(req->rand1), rand1, QBI_SVC_AUTH_RAND_LEN);
  QBI_MEMSCPY(req->rand2, sizeof(req->rand2), rand2, QBI_SVC_AUTH_RAND_LEN);
  QBI_MEMSCPY(req->rand3, sizeof(req->rand3), rand3, QBI_SVC_AUTH_RAND_LEN);
  req->n = 3;

  state->verify_cb = qbi_ut_auth_sim_q_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_AUTH,
                      QBI_SVC_AUTH_MBIM_CID_SIM_AUTH,
                      QBI_MSG_CMD_TYPE_QUERY, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_auth_sim_q_req */

/*===========================================================================
  FUNCTION: qbi_ut_auth_sim_q_rsp
===========================================================================*/
/*!
    @brief Performs verification of a QBI_SVC_AUTH_MBIM_CID_SIM_AUTH response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_auth_sim_q_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_auth, QBI_SVC_AUTH_MBIM_CID_SIM_AUTH,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_auth_sim_q_rsp() */
#endif /* QBI_UT_TEST_AUTH */

#ifdef QBI_UT_TEST_MSHSD
/*===========================================================================
  FUNCTION: qbi_ut_mshsd_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MSHOSTSHUTDOWN set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_mshsd_s_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MSHOSTSHUTDOWN set");

  state->verify_cb = qbi_ut_mshsd_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSHSD,
                      QBI_SVC_MSHSD_MBIM_CID_MSHOSTSHUTDOWN,
                      QBI_MSG_CMD_TYPE_SET, 0, NULL);
} /* qbi_ut_mshsd_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_mshsd_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MSHOSTSHUTDOWN response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_mshsd_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_mshsd,
        QBI_SVC_MSHSD_MBIM_CID_MSHOSTSHUTDOWN, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_mshsd_verify_rsp() */
#endif /* QBI_UT_TEST_MSHSD */

#ifdef QBI_UT_TEST_PACKET_FILTER
/*===========================================================================
  FUNCTION: QBI_UT_BC_PACKET_FILTER_SET_VERIFY_RSP
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PACKET_FILTERS set response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_packet_filter_set_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_PACKET_FILTERS, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_packet_filter_set_verify_rsp() */


/*===========================================================================
  FUNCTION: QBI_UT_BC_PACKET_FILTER_ENABLE_REQ
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_PACKET_FILTERS set request

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_packet_filter_enable_req
(
  qbi_ut_state_s *state,
  uint8 session

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_packet_filter_enable_filter128_req
(
  qbi_ut_state_s *state
)
{
  uint8 information_buffer[]=
  {
        0,    0,    0,    0,     //session
        1,    0,    0,    0,     //PFC
     0x10,    0,    0,    0,     //OL Offset 1
     0x14,    0,    0,    0,     //OL Len

     0x04,    0,    0,    0,     //Size
     0x0C,    0,    0,    0,     //Pattern Offset
     0x10,    0,    0,    0,     //Mask    Offset
     0x4A, 0x7D,    0,    0,     //Pattern

     0xff, 0xff,    0,    0      //mask
  };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_PACKET_FILTERS set request (enable)");
  information_buffer[0] = session;
  if (session >= QBI_TARGET_CFG_MAX_SESSIONS)
  {
    information_buffer[0] = 0;
  }

  state->verify_cb = qbi_ut_bc_packet_filter_set_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_PACKET_FILTERS,
                      QBI_MSG_CMD_TYPE_SET,
                      sizeof(information_buffer),
                      information_buffer);
} /* qbi_ut_bc_packet_filter_enable_req() */

/*===========================================================================
  FUNCTION: QBI_UT_BC_PACKET_FILTER_DISABLE_REQ
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_PACKET_FILTERS set request

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_packet_filter_disable_req
(
  qbi_ut_state_s *state,
  uint8 session
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_packet_filters_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_PACKET_FILTERS set request (disable)");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_bc_packet_filters_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->session_id = session;
  req->packet_filters_count = 0;

  state->verify_cb = qbi_ut_bc_packet_filter_set_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_PACKET_FILTERS,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_packet_filter_disable_req() */

/*===========================================================================
  FUNCTION: QBI_UT_BC_PACKET_FILTER_QUERY_VERIFY_RSP
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PACKET_FILTERS query response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_packet_filter_query_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc,
        QBI_SVC_BC_MBIM_CID_PACKET_FILTERS, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_packet_filter_query_verify_rsp() */

/*===========================================================================
  FUNCTION: QBI_UT_BC_PACKET_FILTER_QUERY_REQ
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_PACKET_FILTERS set request

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_packet_filter_query_req
(
  qbi_ut_state_s *state,
  uint8 session
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_packet_filters_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_PACKET_FILTERS query request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_bc_packet_filters_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->session_id = session;

  state->verify_cb = qbi_ut_bc_packet_filter_query_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC,
                      QBI_SVC_BC_MBIM_CID_PACKET_FILTERS,
                      QBI_MSG_CMD_TYPE_QUERY, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_packet_filter_query_req() */

#endif /* QBI_UT_TEST_PACKET_FILTER */

#ifdef QBI_UT_TEST_QMBE
/*===========================================================================
  FUNCTION: qbi_ut_qmbe_diag_config_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_DIAG_CONFIG query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_qmbe_diag_config_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_DIAG_CONFIG query");
  state->verify_cb = qbi_ut_qmbe_diag_config_q_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_QMBE,
                      QBI_SVC_QMBE_MBIM_CID_DIAG_CONFIG,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_qmbe_diag_config_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_qmbe_diag_config_q_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_DIAG_CONFIG query response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_qmbe_diag_config_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
  const qbi_svc_qmbe_diag_config_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_qmbe, QBI_SVC_QMBE_MBIM_CID_DIAG_CONFIG,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");

    rsp = (const qbi_svc_qmbe_diag_config_rsp_s *)
      (data + sizeof(qbi_msg_cmd_rsp_s));
    if (rsp->diag_config == QBI_SVC_QMBE_MBIM_DIAG_EXCLUDE)
    {
      qbi_ut_qmbe_last_diag_config = QBI_SVC_QMBE_MBIM_DIAG_EXCLUDE;
      QBI_LOG_I_0("DIAG is excluded in the configured MBIM USB composition");
    }
    else
    {
      qbi_ut_qmbe_last_diag_config = QBI_SVC_QMBE_MBIM_DIAG_PRESENT;
      QBI_LOG_I_0("DIAG is present in the configured MBIM USB composition");
    }
  }
} /* qbi_ut_qmbe_diag_config_q_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_qmbe_diag_config_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_DIAG_CONFIG set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_qmbe_diag_config_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_qmbe_diag_config_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_DIAG_CONFIG set");

  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_qmbe_diag_config_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  if(qbi_ut_qmbe_last_diag_config == QBI_SVC_QMBE_MBIM_DIAG_PRESENT)
  {
    req->diag_config = QBI_SVC_QMBE_MBIM_DIAG_EXCLUDE;
  }
  else
  {
    req->diag_config = QBI_SVC_QMBE_MBIM_DIAG_PRESENT;
  }

  state->verify_cb = qbi_ut_qmbe_diag_config_s_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_QMBE,
                      QBI_SVC_QMBE_MBIM_CID_DIAG_CONFIG,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_qmbe_diag_config_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_qmbe_diag_config_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_DIAG_CONFIG set response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_qmbe_diag_config_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_qmbe, QBI_SVC_QMBE_MBIM_CID_DIAG_CONFIG,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_qmbe_diag_config_s_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_qmbe_diag_data_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_QMBE_DIAG_DATA set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_qmbe_diag_data_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  uint32 diag_data_buf_len = 5;
  uint8 diag_data_buffer[5] = {0x7E, 0x1D, 0x1C, 0x3B, 0x7E};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_QMBE_DIAG_DATA set");

  qbi_util_buf_init(&buf);
  if (qbi_util_buf_alloc(&buf, diag_data_buf_len) == NULL)
  {
    return;
  }

  QBI_MEMSCPY(buf.data, buf.size, diag_data_buffer, diag_data_buf_len);

  state->verify_cb = qbi_ut_qmbe_diag_data_s_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_QMBE,
                      QBI_SVC_QMBE_MBIM_CID_DIAG_DATA,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_qmbe_diag_data_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_qmbe_diag_data_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_QMBE_DIAG_DATA set response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_qmbe_diag_data_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_qmbe, QBI_SVC_QMBE_MBIM_CID_DIAG_DATA,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_qmbe_diag_data_s_verify_rsp() */
#endif /* QBI_UT_TEST_QMBE */

#ifdef QBI_UT_TEST_MSFWID
/*===========================================================================
  FUNCTION: qbi_ut_msfwid_firmwareid_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MSFWID_FIRMWAREID query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_msfwid_firmwareid_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MSFWID_FIRMWAREID query");

  state->verify_cb = qbi_ut_msfwid_firmwareid_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSFWID,
                      QBI_SVC_MSFWID_MBIM_CID_FIRMWAREID,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_msfwid_firmwareid_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_msfwid_firmwareid_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MSFWID_FIRMWAREID response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_msfwid_firmwareid_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_msfwid,
        QBI_SVC_MSFWID_MBIM_CID_FIRMWAREID, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_msfwid_firmwareid_verify_rsp() */
#endif /* QBI_UT_TEST_MSFWID */

#if defined (QBI_UT_TEST_MSUICC) || defined(QBI_UT_TEST_UICC_RESET)
/*===========================================================================
  FUNCTION: qbi_ut_msuicc_atr_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MS_UICC_ATR query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_msuicc_atr_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MSUICC_ATR query");

  state->verify_cb = qbi_ut_msuicc_atr_q_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSUICC,
                      QBI_SVC_MSUICC_MBIM_CID_ATR,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_msuicc_atr_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_atr_q_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_UICC_ATR query response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_msuicc_atr_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_msuicc,
        QBI_SVC_MSUICC_MBIM_CID_ATR, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_msuicc_atr_q_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_open_channel_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MS_UICC_OPEN_CHANNEL set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_msuicc_open_channel_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_msuicc_open_channel_s_req_s *req;
  uint32 appid_len = 7;
  uint8 appid[7] = {0xA0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x02};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MS_UICC_OPEN_CHANNEL set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(
    &buf, sizeof(qbi_svc_msuicc_open_channel_s_req_s) + appid_len);
  QBI_CHECK_NULL_PTR_RET(req);

  req->channel_group = 1;
  msuicc_channel_group = req->channel_group;

  req->app_id.size = sizeof(qbi_svc_msuicc_open_channel_s_req_s);
  req->app_id.offset = appid_len;
  QBI_MEMSCPY((uint8 *) buf.data + sizeof(qbi_svc_msuicc_open_channel_s_req_s),
              req->app_id.offset, appid, appid_len);

  state->verify_cb = qbi_ut_msuicc_open_channel_s_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSUICC,
                      QBI_SVC_MSUICC_MBIM_CID_OPEN_CHANNEL,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_msuicc_open_channel_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_open_channel_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_UICC_OPEN_CHANNEL set
    response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_msuicc_open_channel_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
  const qbi_svc_msuicc_open_channel_s_rsp_s *rsp;
/*-------------------------------------------------------------------------*/
  QBI_LOG_E_0("qbi_ut_msuicc_open_channel_s_verify_rsp");
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_msuicc,
        QBI_SVC_MSUICC_MBIM_CID_OPEN_CHANNEL, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
    rsp = (const qbi_svc_msuicc_open_channel_s_rsp_s *)
      (data + sizeof(qbi_msg_cmd_rsp_s));
    msuicc_channel = rsp->channel;
  }
} /* qbi_ut_msuicc_open_channel_s_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_close_channel_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MS_UICC_CLOSE_CHANNEL set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_msuicc_close_channel_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_svc_msuicc_close_channel_s_req_s *req;
  qbi_util_buf_s buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MS_UICC_CLOSE_CHANNEL set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_msuicc_close_channel_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->channel = 2;
  req->channel_group = 1;

  state->verify_cb = qbi_ut_msuicc_close_channel_s_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSUICC,
                      QBI_SVC_MSUICC_MBIM_CID_CLOSE_CHANNEL,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_msuicc_close_channel_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_close_channel_s_req_V2
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MS_UICC_CLOSE_CHANNEL set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_msuicc_close_channel_s_req_v2
(
  qbi_ut_state_s *state
)
{
  qbi_svc_msuicc_close_channel_s_req_s *req;
  qbi_util_buf_s buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MS_UICC_CLOSE_CHANNEL set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_msuicc_close_channel_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->channel = 1;
  req->channel_group = 1;

  state->verify_cb = qbi_ut_msuicc_close_channel_s_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSUICC,
                      QBI_SVC_MSUICC_MBIM_CID_CLOSE_CHANNEL,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_msuicc_close_channel_s_req_v2() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_close_channel_s_req_V3
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MS_UICC_CLOSE_CHANNEL set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_msuicc_close_channel_s_req_v3
(
  qbi_ut_state_s *state
)
{
  qbi_svc_msuicc_close_channel_s_req_s *req;
  qbi_util_buf_s buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MS_UICC_CLOSE_CHANNEL set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_msuicc_close_channel_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->channel = 0;
  req->channel_group = 1;

  state->verify_cb = qbi_ut_msuicc_close_channel_s_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSUICC,
                      QBI_SVC_MSUICC_MBIM_CID_CLOSE_CHANNEL,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_msuicc_close_channel_s_req_v3() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_close_channel_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_UICC_CLOSE_CHANNEL set
    response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_msuicc_close_channel_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_msuicc,
        QBI_SVC_MSUICC_MBIM_CID_CLOSE_CHANNEL, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_msuicc_close_channel_s_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_apdu_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MS_UICC_APDU set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_msuicc_apdu_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_msuicc_apdu_s_req_s *req;
  uint32 command_len = 9;
  uint8 command[9] = {0x03, 0xA4, 0x08, 0x04, 0x04, 0x7F, 0xFF, 0x6F, 0x07};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MS_UICC_APDU set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_msuicc_apdu_s_req_s) +
                           command_len);
  QBI_CHECK_NULL_PTR_RET(req);

  req->channel = 2;
  req->secure_messaging = command[0] & 0x20;
  req->type = command[0] & 0x80;

  req->command.size = sizeof(qbi_svc_msuicc_apdu_s_req_s);
  req->command.offset = command_len;

  QBI_MEMSCPY((uint8 *) buf.data + sizeof(qbi_svc_msuicc_apdu_s_req_s),
              req->command.offset, command, command_len);

  state->verify_cb = qbi_ut_msuicc_apdu_s_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSUICC,
                      QBI_SVC_MSUICC_MBIM_CID_APDU,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_msuicc_apdu_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_apdu_s_req_v2
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MS_UICC_APDU set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_msuicc_apdu_s_req_v2
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_msuicc_apdu_s_req_s *req;
  uint32 command_len = 9;
  uint8 command[9] = {0x03, 0xA4, 0x08, 0x04, 0x04, 0x7F, 0xFF, 0x6F, 0x07};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MS_UICC_APDU set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_msuicc_apdu_s_req_s) +
                           command_len);
  QBI_CHECK_NULL_PTR_RET(req);

  req->channel = 1;
  req->secure_messaging = command[0] & 0x20;
  req->type = command[0] & 0x80;

  req->command.size = sizeof(qbi_svc_msuicc_apdu_s_req_s);
  req->command.offset = command_len;

  QBI_MEMSCPY((uint8 *) buf.data + sizeof(qbi_svc_msuicc_apdu_s_req_s),
              req->command.offset, command, command_len);

  state->verify_cb = qbi_ut_msuicc_apdu_s_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSUICC,
                      QBI_SVC_MSUICC_MBIM_CID_APDU,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_msuicc_apdu_s_req_v2() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_apdu_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_UICC_APDU set response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_msuicc_apdu_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_msuicc,
        QBI_SVC_MSUICC_MBIM_CID_APDU, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_msuicc_apdu_s_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_terminal_capability_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MS_UICC_TERMINAL_CAPABILITY query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_msuicc_terminal_capability_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MS_UICC_TERMINAL_CAPABILITY query");

  state->verify_cb = qbi_ut_msuicc_terminal_capability_q_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSUICC,
                      QBI_SVC_MSUICC_MBIM_CID_TERMINAL_CAPABILITY,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_msuicc_terminal_capability_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_terminal_capability_q_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_UICC_TERMINAL_CAPABILITY
    query response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_msuicc_terminal_capability_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_msuicc,
        QBI_SVC_MSUICC_MBIM_CID_TERMINAL_CAPABILITY, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_msuicc_terminal_capability_q_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_terminal_capability_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MS_UICC_TERMINAL_CAPABILITY set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_msuicc_terminal_capability_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_msuicc_terminal_capability_s_req_s *req;
  qbi_mbim_offset_size_pair_s *field_desc;
  uint32 element_count = 2;
  uint32 tlv_1_len = 3;
  uint8 tlv_1[3] = {0x82, 0x01, 0x00};
  uint32 tlv_2_len = 3;
  uint8 tlv_2[3] = {0x83, 0x01, 0x03};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MS_UICC_TERMINAL_CAPABILITY set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(
    &buf, (sizeof(qbi_svc_msuicc_terminal_capability_s_req_s) +
           sizeof(qbi_mbim_offset_size_pair_s) * element_count +
           tlv_1_len + 1 + tlv_2_len + 1));
  QBI_CHECK_NULL_PTR_RET(req);

  req->element_count = element_count;

  field_desc = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) buf.data +
     sizeof(qbi_svc_msuicc_terminal_capability_s_req_s));

  field_desc->size =
    sizeof(qbi_svc_msuicc_terminal_capability_s_req_s) +
    sizeof(qbi_mbim_offset_size_pair_s) * element_count;
  field_desc->offset = tlv_1_len;
  QBI_MEMSCPY((uint8 *) buf.data + field_desc->size,
              field_desc->offset, tlv_1, tlv_1_len);

  field_desc = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) buf.data +
     sizeof(qbi_svc_msuicc_terminal_capability_s_req_s) +
     sizeof(qbi_mbim_offset_size_pair_s));

  field_desc->size =
    sizeof(qbi_svc_msuicc_terminal_capability_s_req_s) +
    sizeof(qbi_mbim_offset_size_pair_s) * element_count +
    tlv_1_len + 1;
  field_desc->offset = tlv_2_len;
  QBI_MEMSCPY((uint8 *) buf.data + field_desc->size,
              field_desc->offset, tlv_2, tlv_2_len);

  state->verify_cb = qbi_ut_msuicc_terminal_capability_s_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_MSUICC,
                      QBI_SVC_MSUICC_MBIM_CID_TERMINAL_CAPABILITY,
                      QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_msuicc_terminal_capability_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_terminal_capability_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_UICC_TERMINAL_CAPABILITY
    set response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_msuicc_terminal_capability_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_msuicc,
        QBI_SVC_MSUICC_MBIM_CID_TERMINAL_CAPABILITY, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_msuicc_terminal_capability_s_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_reset_q_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_MSUICC_RESET query

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_msuicc_reset_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MSUICC_RESET query");
  state->verify_cb = qbi_ut_msuicc_reset_q_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_MSUICC, QBI_SVC_MSUICC_MBIM_CID_RESET,
    QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_msuicc_reset_q_req */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_reset_q_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MSUICC_RESET
           response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_msuicc_reset_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Checking MBIM_CID_MSUICC_RESET query response");
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_msuicc, QBI_SVC_MSUICC_MBIM_CID_RESET,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_msuicc_reset_q_verify_rsp */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_reset_s_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_MSUICC_RESET set request

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_msuicc_reset_s_req
(
  qbi_ut_state_s *state,
  uint8 action
)
{
  qbi_util_buf_s buf;
  qbi_svc_msuicc_reset_s_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(
    &buf, (sizeof(qbi_svc_msuicc_reset_s_req_s)));
  QBI_CHECK_NULL_PTR_RET(req);

  req->pass_through_action = action;

  QBI_LOG_I_0("Sending MBIM_CID_MSUICC_RESET set request");
  state->verify_cb = qbi_ut_msuicc_reset_s_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_MSUICC, QBI_SVC_MSUICC_MBIM_CID_RESET,
    QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);

  qbi_util_buf_free(&buf);
} /* qbi_ut_msuicc_reset_s_req */

/*===========================================================================
  FUNCTION: qbi_ut_msuicc_reset_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MSUICC_RESET
           response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_msuicc_reset_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_msuicc, QBI_SVC_MSUICC_MBIM_CID_RESET,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_msuicc_reset_s_verify_rsp */

#endif /* QBI_UT_TEST_MSUICC || QBI_UT_TEST_UICC_RESET*/

#ifdef QBI_UT_TEST_DEVICE_RESET
/*===========================================================================
  FUNCTION: qbi_ut_device_reset_s_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_MS_DEVICE_RESET set request

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_device_reset_s_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_RESET set request");
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC_EXT, QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_RESET,
    QBI_MSG_CMD_TYPE_SET, 0, NULL);

} /* qbi_ut_device_reset_s_req */

/*===========================================================================
  FUNCTION: qbi_ut_device_reset_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_DEVICE_RESET
           response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_device_reset_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc, 
        QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_RESET,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_device_reset_s_verify_rsp */
#endif /* QBI_UT_TEST_DEVICE_RESET*/

#ifdef QBI_UT_TEST_MISC
/*===========================================================================
  FUNCTION: qbi_ut_misc_uim_refresh
===========================================================================*/
/*!
    @brief Simulate UIM refresh

    @details
*/
/*=========================================================================*/
static void qbi_ut_misc_uim_refresh
(
  qbi_ut_state_s *state
)
{
  uim_refresh_ind_msg_v01 *uim_refresh_msg;
  qbi_util_buf_s buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Simulate UIM refresh");

  qbi_util_buf_init(&buf);
  uim_refresh_msg = qbi_util_buf_alloc(&buf,
                                       sizeof(uim_refresh_ind_msg_v01));
  QBI_CHECK_NULL_PTR_RET(uim_refresh_msg);

  uim_refresh_msg->refresh_event_valid = TRUE;
  uim_refresh_msg->refresh_event.stage = UIM_REFRESH_STAGE_START_V01;
  uim_refresh_msg->refresh_event.mode = UIM_REFRESH_MODE_RESET_V01;
  uim_refresh_msg->refresh_event.session_type = UIM_SESSION_TYPE_PRIMARY_GW_V01;
  uim_refresh_msg->refresh_event.aid_len = 0;
  uim_refresh_msg->refresh_event.files_len = 0;

  qbi_svc_handle_qmi_ind(state->ctx, QBI_QMI_SVC_UIM, QMI_UIM_REFRESH_IND_V01,
                         (const qbi_util_buf_const_s *)&buf);
  qbi_util_buf_free(&buf);
} /* qbi_ut_msfwid_firmwareid_q_req() */
#endif /* QBI_UT_TEST_MISC */

#ifdef QBI_UT_TEST_ATDS
/*===========================================================================
  FUNCTION: qbi_ut_atds_location_info_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_ATDS_LOCATION_INFO query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_atds_location_info_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_ATDS_LOCATION_INFO query");

  state->verify_cb = qbi_ut_atds_location_info_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_ATDS,
                      QBI_SVC_ATDS_MBIM_CID_ATDS_LOCATION,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_atds_location_info_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_location_info_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a QBI_SVC_ATDS_MBIM_CID_ATDS_LOCATION
           response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_atds_location_info_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_atds,
        QBI_SVC_ATDS_MBIM_CID_ATDS_LOCATION, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_atds_location_info_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_operators_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_ATDS_OPERATORS query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_atds_operators_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_ATDS_OPERATORS query");

  state->verify_cb = qbi_ut_atds_operators_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_ATDS,
                      QBI_SVC_ATDS_MBIM_CID_ATDS_OPERATORS,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_atds_operators_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_operators_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_ATDS_OPERATORS set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_atds_operators_s_req
(
  qbi_ut_state_s *state
)
{
  static const uint8 provider_id[] = {
    /* 310-410 (AT&T) */
    0x33, 0x00, 0x31, 0x00, 0x30, 0x00,
    0x34, 0x00, 0x31, 0x00, 0x30, 0x00
  };
  PACK(struct) {
    qbi_svc_bc_provider_list_s header;
    qbi_mbim_offset_size_pair_s oprt_ol;
    qbi_svc_bc_provider_s oprt;
    uint8 provider_id[12];
  } req;
  static uint32 number = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_ATDS_OPERATORS set");
  QBI_MEMSET(&req, 0, sizeof(req));

  if (number == 0)
  {
    /* Automatic */
    qbi_ut_send_cmd_req(state, QBI_SVC_ID_ATDS,
                        QBI_SVC_ATDS_MBIM_CID_ATDS_OPERATORS,
                        QBI_MSG_CMD_TYPE_SET, sizeof(uint32), &number);
  }
  else
  {
    /* Manual selection */
    req.header.element_count = 1;
    req.oprt_ol.offset =
      sizeof(qbi_svc_bc_provider_list_s) + sizeof(qbi_mbim_offset_size_pair_s);
    req.oprt_ol.size = sizeof(qbi_svc_bc_provider_s);

    req.oprt.provider_id.offset = sizeof(qbi_svc_bc_provider_s);
    req.oprt.provider_id.size = sizeof(provider_id);

    QBI_MEMSCPY(req.provider_id, req.oprt.provider_id.size,
                provider_id, sizeof(provider_id));

    state->verify_cb = qbi_ut_atds_operators_verify_rsp;
    qbi_ut_send_cmd_req(state, QBI_SVC_ID_ATDS,
                        QBI_SVC_ATDS_MBIM_CID_ATDS_OPERATORS,
                        QBI_MSG_CMD_TYPE_SET, sizeof(req), &req);
  }

  number = 1 - number;
} /* qbi_ut_atds_operators_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_operators_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_ATDS_OPERATORS response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_atds_operators_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_atds,
        QBI_SVC_ATDS_MBIM_CID_ATDS_OPERATORS, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_atds_operators_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_projection_tables_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_ATDS_PROJECTION_TABLES query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_atds_projection_tables_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_ATDS_PROJECTION_TABLES query");

  state->verify_cb = qbi_ut_atds_projection_tables_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_ATDS,
                      QBI_SVC_ATDS_MBIM_CID_ATDS_PROJECTION_TABLES,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_atds_projection_tables_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_projection_tables_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_ATDS_PROJECTION_TABLES set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_atds_projection_tables_s_req
(
  qbi_ut_state_s *state
)
{
  uint8 Hexdump[] = {
            5, 0, 0, 0, 44, 0, 0, 0,
            76, 0, 0, 0, 120, 0, 0, 0,
            76, 0, 0, 0, 196, 0, 0, 0,
            76, 0, 0, 0, 16, 1, 0, 0,
            76, 0, 0, 0, 92, 1, 0, 0,
            76, 0, 0, 0, 0, 0, 0, 0,
            176, 255, 255, 255, 232, 3, 0, 0,
            0, 0, 0, 0, 167, 255, 255, 255,
            86, 4, 0, 0, 176, 34, 0, 0,
            158, 255, 255, 255, 86, 4, 0, 0,
            176, 34, 0, 0, 152, 255, 255, 255,
            232, 3, 0, 0, 48, 248, 255, 255,
            143, 255, 255, 255, 183, 1, 0, 0,
            134, 22, 255, 255, 142, 255, 255, 255,
            0, 0, 0, 0, 152, 70, 254, 255,
            1, 0, 0, 0, 176, 255, 255, 255,
            253, 1, 0, 0, 87, 105, 255, 255,
            166, 255, 255, 255, 232, 3, 0, 0,
            0, 0, 0, 0, 156, 255, 255, 255,
            232, 3, 0, 0, 0, 0, 0, 0,
            150, 255, 255, 255, 232, 3, 0, 0,
            0, 0, 0, 0, 136, 255, 255, 255,
            24, 1, 0, 0, 67, 216, 254, 255,
            135, 255, 255, 255, 0, 0, 0, 0,
            152, 70, 254, 255, 2, 0, 0, 0,
            246, 255, 255, 255, 233, 11, 0, 0,
            194, 62, 255, 255, 244, 255, 255, 255,
            136, 19, 0, 0, 208, 138, 255, 255,
            242, 255, 255, 255, 136, 19, 0, 0,
            208, 138, 255, 255, 240, 255, 255, 255,
            184, 11, 0, 0, 112, 29, 255, 255,
            232, 255, 255, 255, 244, 1, 0, 0,
            48, 129, 254, 255, 231, 255, 255, 255,
            0, 0, 0, 0, 64, 162, 255, 255,
            3, 0, 0, 0, 171, 255, 255, 255,
            177, 2, 0, 0, 195, 172, 255, 255,
            161, 255, 255, 255, 232, 3, 0, 0,
            136, 19, 0, 0, 151, 255, 255, 255,
            232, 3, 0, 0, 136, 19, 0, 0,
            141, 255, 255, 255, 58, 2, 0, 0,
            120, 111, 255, 255, 116, 255, 255, 255,
            159, 0, 0, 0, 209, 169, 254, 255,
            115, 255, 255, 255, 0, 0, 0, 0,
            152, 70, 254, 255, 7, 0, 0, 0,
            13, 0, 0, 0, 164, 6, 0, 0,
            231, 112, 254, 255, 4, 0, 0, 0,
            145, 4, 0, 0, 198, 139, 254, 255,
            1, 0, 0, 0, 33, 11, 0, 0,
            63, 110, 254, 255, 253, 255, 255, 255,
            220, 5, 0, 0, 132, 115, 254, 255,
            251, 255, 255, 255, 208, 7, 0, 0,
            96, 121, 254, 255, 251, 255, 255, 255,
            0, 0, 0, 0, 152, 70, 254, 255
         };
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_ATDS_PROJECTION_TABLES set");

  state->verify_cb = qbi_ut_atds_projection_tables_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_ATDS,
                      QBI_SVC_ATDS_MBIM_CID_ATDS_PROJECTION_TABLES,
                      QBI_MSG_CMD_TYPE_SET, sizeof(Hexdump), Hexdump);
} /* qbi_ut_atds_projection_tables_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_projection_tables_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_ATDS_PROJECTION_TABLES response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_atds_projection_tables_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_atds,
        QBI_SVC_ATDS_MBIM_CID_ATDS_PROJECTION_TABLES, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_atds_projection_tables_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_rat_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_ATDS_RAT_INFO query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_atds_rat_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_ATDS_RAT query");

  state->verify_cb = qbi_ut_atds_rat_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_ATDS,
                      QBI_SVC_ATDS_MBIM_CID_ATDS_RAT,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_atds_rat_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_rat_s_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_ATDS_RAT_INFO set request

    @details
*/
/*=========================================================================*/
static void qbi_ut_atds_rat_s_req
(
  qbi_ut_state_s *state
)
{
  static uint32 mode = QBI_SVC_ATDS_RAT_PREF_AUTOMATIC;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_1("Sending MBIM_CID_ATDS_RAT set mode %d", mode);

  state->verify_cb = qbi_ut_atds_rat_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_ATDS,
                      QBI_SVC_ATDS_MBIM_CID_ATDS_RAT,
                      QBI_MSG_CMD_TYPE_SET, sizeof(uint32), &mode);

  mode++;
  if (mode > QBI_SVC_ATDS_RAT_PREF_4G_ONLY)
  {
    mode = QBI_SVC_ATDS_RAT_PREF_AUTOMATIC;
  }
} /* qbi_ut_atds_rat_s_req() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_rat_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_ATDS_RAT response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_atds_rat_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_atds,
        QBI_SVC_ATDS_MBIM_CID_ATDS_RAT, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_atds_rat_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_register_state_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_ATDS_REGISTER_STATE query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_atds_register_state_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_ATDS_REGISTER_STATE query");

  state->verify_cb = qbi_ut_atds_register_state_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_ATDS,
                      QBI_SVC_ATDS_MBIM_CID_ATDS_REGISTER_STATE,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_atds_register_state_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_register_state_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a
    QBI_SVC_ATDS_MBIM_CID_ATDS_REGISTER_STATE response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_atds_register_state_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_atds,
        QBI_SVC_ATDS_MBIM_CID_ATDS_REGISTER_STATE, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_atds_register_state_verify_rsp() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_signal_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_ATDS_SIGNAL query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_atds_signal_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_ATDS_SIGNAL query");

  state->verify_cb = qbi_ut_atds_signal_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_ATDS,
                      QBI_SVC_ATDS_MBIM_CID_ATDS_SIGNAL,
                      QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_atds_signal_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_atds_signal_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_ATDS_SIGNAL response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_atds_signal_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_atds,
        QBI_SVC_ATDS_MBIM_CID_ATDS_SIGNAL, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_atds_signal_verify_rsp() */

#endif /* QBI_UT_TEST_ATDS */

#ifdef QBI_UT_TEST_PROVISIONED_CONTEXTS
/*===========================================================================
  FUNCTION: qbi_ut_bc_device_services_q_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_PROVISIONED_CONTEXTS query

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_provisioned_contexts_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_PROVISIONED_CNTEXTS query");
  state->verify_cb = qbi_ut_bc_provisioned_contexts_q_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PROVISIONED_CONTEXTS,
    QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_bc_provisioned_contexts_q_req */

/*===========================================================================
  FUNCTION: qbi_ut_bc_provisioned_contexts_q_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PROVISIONED_CONTEXTS response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_provisioned_contexts_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Checking MBIM_CID_PROVISIONED_CNTEXTS query response");
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc, QBI_SVC_BC_MBIM_CID_PROVISIONED_CONTEXTS,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_provisioned_contexts_q_verify_rsp */

/*===========================================================================
  FUNCTION: qbi_ut_bc_provisioned_contexts_s_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_PROVISIONED_CONTEXTS set request

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_provisioned_contexts_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_provisioned_contexts_s_req_s *req;
  uint32 ut_access_string_length = 17;
  const char ut_access_string[17] = "vzwinternet";
  uint32 ut_username_length = 12;
  const char ut_username[12] = "ut_username";
  uint32 ut_password_length = 12;
  const char ut_password[12] = "ut_password";
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_PROVISIONED_CNTEXTS set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_bc_provisioned_contexts_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->context_id = 5;
  QBI_MEMSCPY(
    req->context_type, sizeof(req->context_type),
    qbi_svc_bc_context_type_id_to_uuid(QBI_SVC_BC_CONTEXT_TYPE_INTERNET),
    QBI_MBIM_UUID_LEN);
  req->compression = QBI_SVC_BC_COMPRESSION_NONE;
  req->auth_protocol = QBI_SVC_BC_AUTH_PROTOCOL_NONE;
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->access_string, ut_access_string, ut_access_string_length);
  req = (qbi_svc_bc_provisioned_contexts_s_req_s *)buf.data;
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->username, ut_username, ut_username_length);
  req = (qbi_svc_bc_provisioned_contexts_s_req_s *)buf.data;
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->password, ut_password, ut_password_length);

  state->verify_cb = qbi_ut_bc_provisioned_contexts_s_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PROVISIONED_CONTEXTS,
    QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_provisioned_contexts_s_req */

/*===========================================================================
  FUNCTION: qbi_ut_bc_provisioned_contexts_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PROVISIONED_CONTEXTS response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_provisioned_contexts_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc, QBI_SVC_BC_MBIM_CID_PROVISIONED_CONTEXTS,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_provisioned_contexts_s_verify_rsp */

/*===========================================================================
  FUNCTION: qbi_ut_bc_provisioned_contexts_s_append_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_PROVISIONED_CONTEXTS set request to append a new
           provision context

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_provisioned_contexts_s_append_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_provisioned_contexts_s_req_s *req;
  uint32 ut_access_string_length = 24;
  const char ut_access_string[24] = "ut_access_string_append";
  uint32 ut_username_length = 19;
  const char ut_username[19] = "ut_username_append";
  uint32 ut_password_length = 19;
  const char ut_password[19] = "ut_password_append";
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_PROVISIONED_CNTEXTS set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_bc_provisioned_contexts_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->context_id = QBI_SVC_BC_CONTEXT_ID_APPEND;
  QBI_MEMSCPY(
    req->context_type, sizeof(req->context_type),
    qbi_svc_bc_context_type_id_to_uuid(QBI_SVC_BC_CONTEXT_TYPE_INTERNET),
    QBI_MBIM_UUID_LEN);
  req->compression = QBI_SVC_BC_COMPRESSION_NONE;
  req->auth_protocol = QBI_SVC_BC_AUTH_PROTOCOL_NONE;
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->access_string, ut_access_string, ut_access_string_length);
  req = (qbi_svc_bc_provisioned_contexts_s_req_s *)buf.data;
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->username, ut_username, ut_username_length);
  req = (qbi_svc_bc_provisioned_contexts_s_req_s *)buf.data;
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->password, ut_password, ut_password_length);

  state->verify_cb = qbi_ut_bc_provisioned_contexts_s_append_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC, QBI_SVC_BC_MBIM_CID_PROVISIONED_CONTEXTS,
    QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_provisioned_contexts_s_append_req */

/*===========================================================================
  FUNCTION: qbi_ut_bc_provisioned_contexts_s_append_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PROVISIONED_CONTEXTS response
           for appending a provision context

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_provisioned_contexts_s_append_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc, QBI_SVC_BC_MBIM_CID_PROVISIONED_CONTEXTS,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_provisioned_contexts_s_append_verify_rsp */
#endif /* QBI_UT_TEST_PROVISIONED_CONTEXTS */

#ifdef QBI_UT_TEST_PROVISIONED_CONTEXTS_V2
/*===========================================================================
  FUNCTION: qbi_ut_bc_device_services_q_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_PROVISIONED_CONTEXTS query

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_provisioned_contexts_q_v2_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 query");
  state->verify_cb = qbi_ut_bc_provisioned_contexts_q_verify_v2_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC_EXT, QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
    QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_bc_provisioned_contexts_q_req */

/*===========================================================================
  FUNCTION: qbi_ut_bc_provisioned_contexts_q_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PROVISIONED_CONTEXTS response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_provisioned_contexts_q_verify_v2_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Checking QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 query response");
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc_ext, QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_provisioned_contexts_q_verify_rsp */

/*===========================================================================
  FUNCTION: qbi_ut_bc_provisioned_contexts_s_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_PROVISIONED_CONTEXTS set request

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_provisioned_contexts_s_v2_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s *req;
  uint32 ut_access_string_length = 20;
  char ut_access_string[20] = "vzwims";
  uint32 ut_username_length = 15;
  const char ut_username[15] = "ut_username_v2";
  uint32 ut_password_length = 15;
  const char ut_password[15] = "ut_password_v2";
  static int step = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request");
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_bc_ext_provisioned_contexts_v2_s_req_s));
  QBI_CHECK_NULL_PTR_RET(req);

  if (step == 0)
  {
     req->source = 2;
     req->ip_type = 3;
     req->media_type = 0;
     req->operation = 0;
     req->enable = 1;
     req->roaming = 3;
     QBI_LOG_D_0("Sending enable as 1");
     step++;
  }
  else if (step == 1)
  {
     req->source = 1;
     req->ip_type = 3;
     req->media_type = 2;
     req->operation = 0;
     req->enable = 0;
     QBI_LOG_D_0("Sending enable as 0");
     step++;
  }
  else if (step == 2)
  {
     req->source = 1;
     req->ip_type = 3;
     req->media_type = 2;
     req->operation = 0;
     req->enable = 1;
     QBI_LOG_D_0("Sending enable as 1");
     step++;
  }
  else if (step == 3)
  {
     req->source = 1;
     req->ip_type = 3;
     req->media_type = 2;
     req->operation = 0;
     req->enable = 1;
     QBI_LOG_D_0("Sending enable as 1 vzwadmin");
     QBI_MEMSCPY(ut_access_string,20,"vzwadmin",9);
     step++;
  }
  else if (step == 4)
  {
     req->source = 1;
     req->ip_type = 3;
     req->media_type = 2;
     req->operation = 0;
     req->enable = 0;
     QBI_LOG_D_0("Sending enable as 0 vzwadmin");
     QBI_MEMSCPY(ut_access_string,20,"vzwadmin",9);
     step++;
  }
  else if (step == 5)
  {
     req->source = 1;
     req->ip_type = 3;
     req->media_type = 2;
     req->operation = 0;
     req->enable = 1;
     QBI_LOG_D_0("Sending enable as 1 vzwadmin");
     QBI_MEMSCPY(ut_access_string,20,"vzwadmin",9);
     step++;
  }
  else if (step == 6)
  {
     req->source = 1;
     req->ip_type = 3;
     req->media_type = 2;
     req->operation = 0;
     req->enable = 1;
     QBI_LOG_D_0("Sending enable as 1 vzwinternet");
     QBI_MEMSCPY(ut_access_string,20,"vzwinternet",12);
     step++;
  }
  else if (step == 7)
  {
     req->source = 1;
     req->ip_type = 3;
     req->media_type = 2;
     req->operation = 0;
     req->enable = 1;
     QBI_LOG_D_0("Sending enable as 1 airtelgprs.com");
     QBI_MEMSCPY(ut_access_string,20,"airtelgprs.com",15);
     step++;
  }
  else if (step == 8)
  {
     req->source = 1;
     req->ip_type = 3;
     req->media_type = 2;
     req->operation = 1;
     req->enable = 1;
     QBI_LOG_D_0("Sending Req To Deleting Profile eeeeeee");
     QBI_MEMSCPY(ut_access_string,20,"eeeeeee",8);
     step = 0;
  }
  else
  {
     req->source = 1;
     req->ip_type = 3;
     req->media_type = 2;
     req->operation = 2;
     req->enable = 1;
     QBI_LOG_D_0("Sending Req To Factory Restore");
     step = 0;
  }

  QBI_MEMSCPY(
    req->context_type, sizeof(req->context_type),
    qbi_svc_bc_context_type_id_to_uuid(QBI_SVC_BC_CONTEXT_TYPE_INTERNET),
    QBI_MBIM_UUID_LEN);
  req->compression = QBI_SVC_BC_COMPRESSION_NONE;
  req->auth_protocol = QBI_SVC_BC_AUTH_PROTOCOL_NONE;
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->access_string, ut_access_string, ut_access_string_length);
  req = (qbi_svc_bc_provisioned_contexts_s_req_s *)buf.data;
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->username, ut_username, ut_username_length);
  req = (qbi_svc_bc_provisioned_contexts_s_req_s *)buf.data;
  qbi_ut_append_field_ascii_to_utf16(
    &buf, &req->password, ut_password, ut_password_length);

  state->verify_cb = qbi_ut_bc_provisioned_contexts_s_v2_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC_EXT, QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
    QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);

  QBI_LOG_I_0("Sent QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set request waiting for rsp");
  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_provisioned_contexts_v2_s_req */

/*===========================================================================
  FUNCTION: qbi_ut_bc_provisioned_contexts_s_v2_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_PROVISIONED_CONTEXTS response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_provisioned_contexts_s_v2_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Checking QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2 set response");
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc, QBI_SVC_BC_EXT_MBIM_CID_MS_PROVISIONED_CONTEXT_V2,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_provisioned_contexts_v2_s_verify_rsp */

#endif /* QBI_UT_TEST_PROVISIONED_CONTEXTS_V2 */

#ifdef QBI_UT_TEST_SLOT_MAPPING

/*===========================================================================
  FUNCTION: qbi_ut_bc_ext_slot_mapping_q_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_MS_DEVICE_SLOT_MAPPING query

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_ext_slot_mapping_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending MBIM_CID_MS_DEVICE_SLOT_MAPPING query");
  state->verify_cb = qbi_ut_bc_ext_slot_mapping_q_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC_EXT, QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_SLOT_MAPPING,
    QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_bc_ext_slot_mapping_q_req */

/*===========================================================================
  FUNCTION: qbi_ut_bc_ext_slot_mapping_q_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_DEVICE_SLOT_MAPPING
           response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_ext_slot_mapping_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Checking MBIM_CID_MS_DEVICE_SLOT_MAPPING query response");
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc_ext, QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_SLOT_MAPPING,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_ext_slot_mapping_q_verify_rsp */

/*===========================================================================
  FUNCTION: qbi_ut_bc_ext_slot_mapping_s_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_MS_DEVICE_SLOT_MAPPING set request

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_ext_slot_mapping_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_ext_slot_mapping_info_s *req;
  uint32 *slot_id;
  static uint8 var = 1;
  uint32 *cid;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(
    &buf, (sizeof(qbi_svc_bc_ext_slot_mapping_info_s) + sizeof(uint32)));
  QBI_CHECK_NULL_PTR_RET(req);
  
  req->map_count = 1;
  req->slot_map_list.offset = sizeof(qbi_svc_bc_ext_slot_mapping_info_s);
  req->slot_map_list.size = sizeof(uint32);

  // first slot mapping

  slot_id = (uint32 *)((uint8 *) buf.data + req->slot_map_list.offset);
  if(var == 1)
  {
     var--;
    *slot_id = 0;
  }
  else
  {
	   var++;
	  *slot_id = 1;
  }

  QBI_LOG_I_0("Sending MBIM_CID_MS_DEVICE_SLOT_MAPPING set request");
  state->verify_cb = qbi_ut_bc_ext_slot_mapping_s_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC_EXT, QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_SLOT_MAPPING,
    QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);

  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_ext_slot_mapping_s_req */

/*===========================================================================
  FUNCTION: qbi_ut_bc_ext_slot_mapping_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_DEVICE_SLOT_MAPPING
           response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_ext_slot_mapping_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc_ext, QBI_SVC_BC_EXT_MBIM_CID_MS_DEVICE_SLOT_MAPPING,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_ext_slot_mapping_s_verify_rsp */

#endif /* QBI_UT_TEST_SLOT_MAPPING */

#ifdef QBI_UT_TEST_SLOT_INFO

/*===========================================================================
  FUNCTION: qbi_ut_bc_ext_slot_mapping_q_req
===========================================================================*/
/*!
    @brief Sends a MBIM_CID_MS_DEVICE_SLOT_MAPPING query

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_ext_slot_info_q_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_ext_slot_info_req_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, (sizeof(qbi_svc_bc_ext_slot_info_req_s)));
  QBI_CHECK_NULL_PTR_RET(req);

  req->slot_index = 0;

  QBI_LOG_I_0("Sending QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO_STATUS query");
  state->verify_cb = qbi_ut_bc_ext_slot_info_q_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC_EXT, QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO_STATUS,
    QBI_MSG_CMD_TYPE_QUERY, buf.size, buf.data);

  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_ext_slot_info_q_req */

/*===========================================================================
  FUNCTION: qbi_ut_bc_ext_slot_info_q_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_DEVICE_SLOT_MAPPING
           response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_ext_slot_info_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Checking QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO_STATUS query response");
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc_ext, QBI_SVC_BC_EXT_MBIM_CID_MS_SLOT_INFO_STATUS,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_ext_slot_info_q_verify_rsp */

#endif /* QBI_UT_TEST_SLOT_INFO */

#ifdef QBI_UT_TEST_LTE_ATTACH

/*===========================================================================
  FUNCTION: qbi_ut_bc_device_services_q_req
===========================================================================*/
/*!
    @brief Sends a QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG query

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_lte_attach_q_req
(
  qbi_ut_state_s *state
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG query");
  //state->verify_cb = qbi_ut_bc_lte_attach_q_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC_EXT, QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG,
    QBI_MSG_CMD_TYPE_QUERY, 0, NULL);
} /* qbi_ut_bc_lte_attach_q_req */

/*===========================================================================
  FUNCTION: qbi_ut_bc_provisioned_contexts_q_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG
    response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_lte_attach_q_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Checking QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG query response");
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc_ext, QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_lte_attach_q_verify_rsp */

/*===========================================================================
  FUNCTION: qbi_ut_bc_lte_attach_s_req
===========================================================================*/
/*!
    @brief Sends a QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG
    set request

    @details

    @param state
*/
/*=========================================================================*/
static void qbi_ut_bc_lte_attach_s_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_svc_bc_ext_lte_attach_context_s *req;
  qbi_svc_bc_ext_lte_attach_config_s_req_s *req_s;
  qbi_mbim_offset_size_pair_s *field_desc_one;
  qbi_mbim_offset_size_pair_s *field_desc_two;
  qbi_mbim_offset_size_pair_s *field_desc_three;
  static int step = 0;
  const char test_apn[16]= {0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x00,0x00};
  const char test_apn_2[16]= {0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x00,0x00};
  const char test_apn_3[16]= {0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x64,0x00,0x00,0x00};
  const char test_apn_4[16]= {0x65,0x00,0x65,0x00,0x65,0x00,0x65,0x00,0x65,0x00,0x65,0x00,0x65,0x00,0x00,0x00};
  const char test_apn_5[16]= {0x66,0x00,0x66,0x00,0x66,0x00,0x66,0x00,0x66,0x00,0x66,0x00,0x66,0x00,0x00,0x00};
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_0("Sending QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG set request");
  qbi_util_buf_init(&buf);
  req_s = qbi_util_buf_alloc(&buf, sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) + 
  sizeof(qbi_mbim_offset_size_pair_s)*3 + sizeof(qbi_svc_bc_ext_lte_attach_context_s)* 3 + 16*3);
  req = (qbi_svc_bc_ext_lte_attach_context_s *)((uint8*)req_s + 
  sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) + sizeof(qbi_mbim_offset_size_pair_s) * 3);
  
  QBI_CHECK_NULL_PTR_RET(req);
  QBI_CHECK_NULL_PTR_RET(req_s);

  req_s->operation = 0;
  req_s->element_count = 3;

  field_desc_one = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) buf.data +
    sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s));
  field_desc_one->offset = 32;
  field_desc_one->size = 60;

  field_desc_two = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) field_desc_one +
    sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s));
  field_desc_two->offset = 92;
  field_desc_two->size = 60;

  field_desc_three = (qbi_mbim_offset_size_pair_s *)
    ((uint8 *) field_desc_two +
    sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s));
  field_desc_three->offset = 152;
  field_desc_three->size = 60;


//Starting 1st payload
  req->source = 1;
  req->roaming = 0;
  req->ip_type = 3;
  
  req->access_string.offset = 44;
  req->access_string.size = 14;

  req->username.offset = 0;
  req->username.size = 0;

  req->password.offset = 0;
  req->password.size = 0; 

  req->compression = QBI_SVC_BC_COMPRESSION_NONE;
  req->auth_protocol = QBI_SVC_BC_AUTH_PROTOCOL_CHAP;

  QBI_MEMSCPY((uint8 *) buf.data + sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) + sizeof(qbi_mbim_offset_size_pair_s)*3 + req->access_string.offset,16,
               test_apn, 16);
  
//Starting 2nd payload
  req = (qbi_svc_bc_ext_lte_attach_context_s *)((uint8*)req + 60);
  req->source = 1;
  req->roaming = 1;
  req->ip_type = 3;
  
  req->access_string.offset = 44;
  req->access_string.size = 14;

  req->username.offset = 0;
  req->username.size = 0;

  req->password.offset = 0;
  req->password.size = 0; 

  req->compression = QBI_SVC_BC_COMPRESSION_NONE;
  req->auth_protocol = QBI_SVC_BC_AUTH_PROTOCOL_CHAP;

  if (step == 0) 
  {
    QBI_MEMSCPY((uint8 *) buf.data + sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) + sizeof(qbi_mbim_offset_size_pair_s)*3 + req->access_string.offset + 60,16,
                 test_apn_2, 16);
  }
  else
  {
    QBI_MEMSCPY((uint8 *) buf.data + sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) + sizeof(qbi_mbim_offset_size_pair_s)*3 + req->access_string.offset + 60,16,
                 test_apn_4, 16);
  }
  //Starting 3rd payload
  req = (qbi_svc_bc_ext_lte_attach_context_s *)((uint8*)req + 60);
  req->source = 1;
  req->roaming = 2;
  req->ip_type = 3;
  
  req->access_string.offset = 44;
  req->access_string.size = 14;

  req->username.offset = 0;
  req->username.size = 0;

  req->password.offset = 0;
  req->password.size = 0;

  req->compression = QBI_SVC_BC_COMPRESSION_NONE;
  req->auth_protocol = QBI_SVC_BC_AUTH_PROTOCOL_CHAP;

  if (step == 0) 
  {
    QBI_MEMSCPY((uint8 *) buf.data + sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) + sizeof(qbi_mbim_offset_size_pair_s)*3 + req->access_string.offset + 120,16,
               test_apn_3, 16);
    step ++;
  }
  else
  {
    QBI_MEMSCPY((uint8 *) buf.data + sizeof(qbi_svc_bc_ext_lte_attach_config_s_req_s) + sizeof(qbi_mbim_offset_size_pair_s)*3 + req->access_string.offset + 120,16,
               test_apn_5, 16);
    step = 0;
  }
  //state->verify_cb = qbi_ut_bc_lte_attach_s_verify_rsp;
  qbi_ut_send_cmd_req(
    state, QBI_SVC_ID_BC_EXT, QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG,
    QBI_MSG_CMD_TYPE_SET, buf.size, buf.data);
  QBI_LOG_I_0("Sent QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG set request waiting ...");
  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_lte_attach_s_req */

/*===========================================================================
  FUNCTION: qbi_ut_bc_lte_attach_s_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG
    response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_lte_attach_s_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_I_0("Checking QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG set response");
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc, QBI_SVC_BC_EXT_MBIM_CID_MS_LTE_ATTACH_CONFIG,
        QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_lte_attach_s_verify_rsp */

#endif /* QBI_UT_TEST_LTE_ATTACH */

#ifdef QBI_UT_TEST_PCO_INFO

/*===========================================================================
  FUNCTION: qbi_ut_bc_ext_pco_info_q_req
===========================================================================*/
/*!
    @brief Send a MBIM_CID_MS_PCO_INFO query request

    @details
*/
/*=========================================================================*/
static void qbi_ut_bc_ext_pco_info_q_req
(
  qbi_ut_state_s *state
)
{
  qbi_util_buf_s buf;
  qbi_mbim_pco_value_s *req;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  QBI_LOG_I_1("Sending MBIM_CID_MS_PCO_INFO query, session ID %d",
              qbi_ut_connect_current_session_id);
  qbi_util_buf_init(&buf);
  req = qbi_util_buf_alloc(&buf, sizeof(qbi_mbim_pco_value_s));
  QBI_CHECK_NULL_PTR_RET(req);

  req->session_id = qbi_ut_connect_current_session_id;
  state->verify_cb = qbi_ut_bc_ext_pco_info_verify_rsp;
  qbi_ut_send_cmd_req(state, QBI_SVC_ID_BC_EXT,
                      QBI_SVC_BC_EXT_MBIM_CID_MS_PCO,
                      QBI_MSG_CMD_TYPE_QUERY, buf.size, buf.data);
  qbi_util_buf_free(&buf);
} /* qbi_ut_bc_ext_pco_info_q_req() */

/*===========================================================================
  FUNCTION: qbi_ut_bc_ext_pco_info_verify_rsp
===========================================================================*/
/*!
    @brief Performs verification of a MBIM_CID_MS_PCO_INFO response

    @details

    @param data
    @param data_len
*/
/*=========================================================================*/
static void qbi_ut_bc_ext_pco_info_verify_rsp
(
  const uint8 *data,
  uint32       data_len
)
{
/*-------------------------------------------------------------------------*/
  if (qbi_ut_check_cmd_rsp(
        data, data_len, qbi_ut_uuid_bc_ext,
        QBI_SVC_BC_EXT_MBIM_CID_MS_PCO, QBI_MBIM_STATUS_SUCCESS))
  {
    QBI_LOG_I_0("QBI gave expected response");
  }
} /* qbi_ut_bc_ext_pco_info_verify_rsp() */
#endif /* QBI_UT_TEST_PCO_INFO */

/*===========================================================================
  FUNCTION: qbi_ut_run
===========================================================================*/
/*!
    @brief Entry point to run the QBI unit tests

    @details
    Should be called once during QBI initialization. Unit tests will start
    after a timed delay.
*/
/*=========================================================================*/
static void qbi_ut_run
(
  qbi_ut_state_s *state
)
{
  boolean use_timeout = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(state);

  /* The contexts won't be registered when processing QBI_UT_INIT, so initialize
     the state->ctx pointer on the next state change */
  QBI_LOG_W_2("UT run state %d ctx %d", state->state, state->ctx);
  if (state->state != QBI_UT_INIT && state->ctx == NULL)
  {
    state->ctx = qbi_task_get_ctx_by_id(QBI_UT_CONTEXT_ID);
    if (state->ctx == NULL)
    {
      QBI_LOG_W_1("Couldn't get context pointer for context ID %d",
                  QBI_UT_CONTEXT_ID);
    }
    else
    {
      if (state->ctx->id == QBI_CTX_ID_UNIT_TEST)
      {
        qbi_ctx_s *primary_ctx = qbi_task_get_ctx_by_id(QBI_CTX_ID_PRIMARY);
        if (primary_ctx != NULL)
        {
          state->ctx->hc_state = primary_ctx->hc_state;
        }
      }
      QBI_LOG_I_1("Initialized QBI UT to use context ID %d", QBI_UT_CONTEXT_ID);
    }
  }

  switch (state->state)
  {
    case QBI_UT_INIT:
      QBI_MEMSET(state, 0, sizeof(qbi_ut_state_s));
      state->ut_ctx.id = QBI_CTX_ID_UNIT_TEST;
      qbi_task_register_context(&state->ut_ctx);
      qbi_os_timer_init(&state->timer, qbi_ut_timer_cb, state);
      qbi_os_timer_init(&state->timeout, qbi_ut_timeout_cb, state);
      qbi_os_timer_set(&state->timer, 30000);
      use_timeout = FALSE;
      QBI_LOG_I_0("QBI unit test module initiated. Testing starts in 30s");
      break;

    case QBI_UT_OPEN:
      qbi_ut_tx_open_msg(state);
      break;

    case QBI_UT_DEV_SVCS:
      qbi_ut_bc_device_services_q_req(state);
      break;

    case QBI_UT_EVENT_FILTER:
      qbi_ut_bc_device_service_subscribe_list_s_req(state);
      break;

    case QBI_UT_DEV_CAPS:
      qbi_ut_bc_device_caps_q_req(state);
      break;

    case QBI_UT_RDY_INFO:
      qbi_ut_bc_ready_info_q_req(state);
      break;

    case QBI_UT_RF_STATE:
      qbi_ut_bc_radio_state_q_req(state);
      break;

    case QBI_UT_RF_STATE_SET:
      qbi_ut_bc_radio_state_s_req(state);
      break;

    case QBI_UT_REG_STATE:
      qbi_ut_bc_register_state_q_req(state);
      break;

    case QBI_UT_PKT_SRVC:
      qbi_ut_bc_packet_service_q_req(state);
      break;

    case QBI_UT_VIS_PRO:
      qbi_ut_bc_visible_providers_q_req(state);
      use_timeout = FALSE;
      break;

    #ifdef QBI_UT_TEST_CONNECT
    case QBI_UT_CONNECT_0:
    #ifdef QBI_UT_TEST_CONNECT_MPDP
    case QBI_UT_CONNECT_1:
    case QBI_UT_CONNECT_2:
    case QBI_UT_CONNECT_3:
    case QBI_UT_CONNECT_4:
    #endif
      qbi_ut_bc_connect_s_activate_req(state);
      use_timeout = FALSE;
      break;

    case QBI_UT_IP_CFG_INFO_0:
    #ifdef QBI_UT_TEST_CONNECT_MPDP
    case QBI_UT_IP_CFG_INFO_1:
    case QBI_UT_IP_CFG_INFO_2:
    case QBI_UT_IP_CFG_INFO_3:
    case QBI_UT_IP_CFG_INFO_4:
    #endif
      qbi_ut_bc_ip_configuration_info_q_req(state);
      #ifdef QBI_UT_TEST_CONNECT_MPDP
      if (qbi_ut_connect_current_session_id >= QBI_UT_CONNECT_SESSION_NUM - 1)
      {
        qbi_ut_connect_current_session_id = 0;
      }
      else
      {
        qbi_ut_connect_current_session_id++;
      }
      #endif
      break;

    case QBI_UT_DISCONNECT_0:
    #ifdef QBI_UT_TEST_CONNECT_MPDP
    case QBI_UT_DISCONNECT_1:
    case QBI_UT_DISCONNECT_2:
    case QBI_UT_DISCONNECT_3:
    case QBI_UT_DISCONNECT_4:
    #endif
      qbi_ut_bc_connect_s_deactivate_req(state);
      #ifdef QBI_UT_TEST_CONNECT_MPDP
      if (qbi_ut_connect_current_session_id >= QBI_UT_CONNECT_SESSION_NUM - 1)
      {
        qbi_ut_connect_current_session_id = 0;
      }
      else
      {
        qbi_ut_connect_current_session_id++;
      }
      #endif
      break;
    #endif /* QBI_UT_TEST_CONNECT */

    #ifdef QBI_UT_TEST_EXT_QMUX
    case QBI_UT_EXT_QMUX_CLID_ALLOC:
      qbi_ut_ext_qmux_s_clid_alloc(state);
      break;

    case QBI_UT_EXT_QMUX_SET_EVENT_REPORT:
      qbi_ut_ext_qmux_s_set_event_report(state);
      break;

    case QBI_UT_EXT_QMUX_SET_OPRT_MODE_LPM:
      qbi_ut_ext_qmux_s_set_oprt_mode(state, 1);
      break;

    case QBI_UT_EXT_QMUX_WAIT_OPRT_MODE_IND:
      qbi_ut_ext_qmux_wait_for_ind(state);
      use_timeout = FALSE;
      break;

    case QBI_UT_EXT_QMUX_SET_OPRT_MODE_ONLINE:
      qbi_ut_ext_qmux_s_set_oprt_mode(state, 0);
      break;

    case QBI_UT_EXT_QMUX_CLID_DEALLOC:
      qbi_ut_ext_qmux_s_clid_dealloc(state);
      break;
    #endif

    #ifdef QBI_UT_TEST_USSD
    case QBI_UT_USSD_ORIG:
      qbi_ut_tx_ussd_orig_req(state);
      break;

    case QBI_UT_USSD_ANSWER1:
    case QBI_UT_USSD_ANSWER2:
    case QBI_UT_USSD_ANSWER3:
    case QBI_UT_USSD_ANSWER4:
    case QBI_UT_USSD_ANSWER5:
      qbi_ut_tx_ussd_answer_req(state);
      break;

    case QBI_UT_USSD_CANCEL:
      qbi_ut_tx_ussd_cancel_req(state);
      break;
    #endif /* QBI_UT_TEST_USSD */

    #ifdef QBI_UT_TEST_STK
    case QBI_UT_STK_PAC_Q:
      QBI_LOG_W_0("STK Unit test: PAC query");
      qbi_ut_stk_pac_q_req(state);
      use_timeout = FALSE;
      qbi_os_timer_set(&state->timer, 10000);
      break;

    case QBI_UT_STK_PAC_S:
      QBI_LOG_W_0("STK Unit test: PAC set");
      qbi_ut_stk_pac_s_req(state);
      use_timeout = FALSE;
      qbi_os_timer_set(&state->timer, 60000);
      break;

    case QBI_UT_STK_TR_S:
      QBI_LOG_W_0("STK Unit test: TR set");
      qbi_ut_stk_tr_s_req(state);
      use_timeout = FALSE;
      qbi_os_timer_set(&state->timer, 10000);
      break;

    case QBI_UT_STK_ENV_Q:
      QBI_LOG_W_0("STK Unit test: ENV query");
      qbi_ut_stk_env_q_req(state);
      use_timeout = FALSE;
      qbi_os_timer_set(&state->timer, 10000);
      break;

    case QBI_UT_STK_ENV_S:
      QBI_LOG_W_0("STK Unit test: ENV set");
      qbi_ut_stk_env_s_req(state);
      use_timeout = FALSE;
      qbi_os_timer_set(&state->timer, 10000);
      break;
    #endif /* QBI_UT_TEST_STK */

    #ifdef QBI_UT_TEST_PB
    case QBI_UT_PB_CONFIG:
      qbi_ut_pb_config_q_req(state);
      break;

    case QBI_UT_PB_DELETE:
      qbi_ut_pb_delete_s_req(state);
      break;

    case QBI_UT_PB_WRITE:
      qbi_ut_pb_write_s_req(state);
      break;

    case QBI_UT_PB_READ:
      qbi_ut_pb_read_q_req(state);
      break;
    #endif /* QBI_UT_TEST_PB */

    #ifdef QBI_UT_TEST_AUTH
    case QBI_UT_AUTH_AKA_Q:
      QBI_LOG_W_0("AUTH Unit test: aka query");
      qbi_ut_auth_aka_q_req(state);
      break;

    case QBI_UT_AUTH_AKAP_Q:
      QBI_LOG_W_0("AUTH Unit test: akap query");
      qbi_ut_auth_akap_q_req(state);
      break;

    case QBI_UT_AUTH_SIM_Q:
      QBI_LOG_W_0("AUTH Unit test: sim query");
      qbi_ut_auth_sim_q_req(state);
      break;
    #endif /* QBI_UT_TEST_AUTH */

    #ifdef QBI_UT_TEST_MSHSD
    case QBI_UT_MSHSD:
      qbi_ut_mshsd_s_req(state);
      break;
    #endif /* QBI_UT_TEST_MSHSD */

    #ifdef QBI_UT_TEST_PACKET_FILTER
    case QBI_UT_PACKET_FILTER_ENABLE_0:
      qbi_ut_bc_packet_filter_enable_req(state, 0);
      QBI_LOG_E_0("Packet filter enabled for session 0");
      break;
      //qbi_ut_bc_packet_filter_enable_req(state);
    case QBI_UT_PACKET_FILTER_ENABLE_1:
      qbi_ut_bc_packet_filter_enable_req(state, 1);
      QBI_LOG_E_0("Packet filter enabled for session 1");
      break;
      //qbi_os_timer_set(&state->timer, 1000);
    case QBI_UT_PACKET_FILTER_ENABLE_2:
      qbi_ut_bc_packet_filter_enable_req(state, 2);
      QBI_LOG_E_0("Packet filter enabled for session 2");
      break;

    case QBI_UT_PACKET_FILTER_ENABLE_3:
      qbi_ut_bc_packet_filter_enable_req(state, 3);
      QBI_LOG_E_0("Packet filter enabled for session 3");
      break;

    case QBI_UT_PACKET_FILTER_ENABLE_4:
      qbi_ut_bc_packet_filter_enable_req(state, 4);
      QBI_LOG_E_0("Packet filter enabled for session 4");
      break;

    case QBI_UT_PACKET_FILTER_DISABLE_0:
      qbi_ut_bc_packet_filter_disable_req(state, 0);
      QBI_LOG_E_0("Packet filter disabled for session 0");
      break;
      //qbi_os_timer_set(&state->timer, 1000);
    case QBI_UT_PACKET_FILTER_DISABLE_1:
      qbi_ut_bc_packet_filter_disable_req(state, 1);
      QBI_LOG_E_0("Packet filter disabled for session 1");
      break;

    case QBI_UT_PACKET_FILTER_DISABLE_2:
      qbi_ut_bc_packet_filter_disable_req(state, 2);
      QBI_LOG_E_0("Packet filter disabled for session 2");
      break;

    case QBI_UT_PACKET_FILTER_DISABLE_3:
      qbi_ut_bc_packet_filter_disable_req(state, 3);
      QBI_LOG_E_0("Packet filter disabled for session 3");
      break;

    case QBI_UT_PACKET_FILTER_DISABLE_4:
      qbi_ut_bc_packet_filter_disable_req(state, 4);
      qbi_os_timer_set(&state->timer, 600000);
      use_timeout = FALSE;
      QBI_LOG_E_0("Packet filter disabled for session 4");
      break;

    case QBI_UT_PACKET_FILTER_QUERY_0:
      qbi_ut_bc_packet_filter_query_req(state, 0);
      break;

    case QBI_UT_PACKET_FILTER_QUERY_1:
      qbi_ut_bc_packet_filter_query_req(state, 1);
      break;

    case QBI_UT_PACKET_FILTER_QUERY_2:
      qbi_ut_bc_packet_filter_query_req(state, 2);
      break;

    case QBI_UT_PACKET_FILTER_QUERY_3:
      qbi_ut_bc_packet_filter_query_req(state, 3);
      break;

    case QBI_UT_PACKET_FILTER_QUERY_4:
      qbi_ut_bc_packet_filter_query_req(state, 4);
      qbi_os_timer_set(&state->timer, 600000);
      use_timeout = FALSE;
      break;
    #endif /* QBI_UT_TEST_PACKET_FILTER */

    #ifdef QBI_UT_TEST_QMBE
    case QBI_UT_QMBE_DIAG_CONFIG_Q:
      qbi_ut_qmbe_diag_config_q_req(state);
      break;

    case QBI_UT_QMBE_DIAG_CONFIG_S:
      qbi_ut_qmbe_diag_config_s_req(state);
      break;

     case QBI_UT_QMBE_DIAG_DATA_S:
       qbi_ut_qmbe_diag_data_s_req(state);
       break;
    #endif /* QBI_UT_TEST_QMBE */

    #ifdef QBI_UT_TEST_MSFWID
    case QBI_UT_MSFWID:
      qbi_ut_msfwid_firmwareid_q_req(state);
      break;
    #endif /* QBI_UT_TEST_MSFWID */

    #ifdef QBI_UT_TEST_MSUICC
    case QBI_UT_MSUICC_ATR_Q:
      qbi_ut_msuicc_atr_q_req(state);
      break;

    case QBI_UT_MSUICC_OPEN_CHANNEL_S:
      qbi_ut_msuicc_open_channel_s_req(state);
      break;

    case QBI_UT_MSUICC_CLOSE_CHANNEL_S:
      qbi_ut_msuicc_close_channel_s_req(state);
      break;

    case QBI_UT_MSUICC_APDU_S:
      qbi_ut_msuicc_apdu_s_req(state);
      break;

    case QBI_UT_MSUICC_TERMINAL_CAPABILITY_Q:
      qbi_ut_msuicc_terminal_capability_q_req(state);
      break;

    case QBI_UT_MSUICC_TERMINAL_CAPABILITY_S:
      qbi_ut_msuicc_terminal_capability_s_req(state);
      break;
    #endif /* QBI_UT_TEST_MSUICC */

    #ifdef QBI_UT_TEST_UICC_RESET
    case QBI_UT_MSUICC_RESET_S_1:
      qbi_ut_msuicc_reset_s_req(state, 1);
      break;

    case QBI_UT_MSUICC_ATR_Q_1:
      qbi_ut_msuicc_atr_q_req(state);
      break;

    case QBI_UT_MSUICC_OPEN_CHANNEL_S_1:
      qbi_ut_msuicc_open_channel_s_req(state);
      break;

    case QBI_UT_MSUICC_APDU_S_1:
      qbi_ut_msuicc_apdu_s_req_v2(state);
      break;

    case QBI_UT_MSUICC_CLOSE_CHANNEL_S_1:
      qbi_ut_msuicc_close_channel_s_req_v2(state);
      break;

    case QBI_UT_MSUICC_RESET_Q_1:
      qbi_ut_msuicc_reset_q_req(state);
      break;

    case QBI_UT_MSUICC_RESET_S_2:
      qbi_ut_msuicc_reset_s_req(state, 0);
      break;

    case QBI_UT_MSUICC_ATR_Q_2:
      qbi_ut_msuicc_atr_q_req(state);
      break;

    case QBI_UT_MSUICC_OPEN_CHANNEL_S_2:
      qbi_ut_msuicc_open_channel_s_req(state);
      break;

    case QBI_UT_MSUICC_APDU_S_2:
      qbi_ut_msuicc_apdu_s_req(state);
      break;

    case QBI_UT_MSUICC_CLOSE_CHANNEL_S_2:
      qbi_ut_msuicc_close_channel_s_req(state);
      break;

    case QBI_UT_MSUICC_RESET_Q_2:
      qbi_ut_msuicc_reset_q_req(state);
      break;

    case QBI_UT_MSUICC_RESET_S_3:
      qbi_ut_msuicc_reset_s_req(state, 1);
      break;

    case QBI_UT_MSUICC_ATR_Q_3:
      qbi_ut_msuicc_atr_q_req(state);
      break;

    case QBI_UT_MSUICC_OPEN_CHANNEL_S_3:
      qbi_ut_msuicc_open_channel_s_req(state);
      break;

    case QBI_UT_MSUICC_OPEN_CHANNEL_S_4:
      qbi_ut_msuicc_open_channel_s_req(state);
      break;

    case QBI_UT_MSUICC_OPEN_CHANNEL_S_5:
      qbi_ut_msuicc_open_channel_s_req(state);
      break;

    case QBI_UT_MSUICC_APDU_S_3:
      qbi_ut_msuicc_apdu_s_req_v2(state);
      break;

    case QBI_UT_MSUICC_CLOSE_CHANNEL_S_3:
      qbi_ut_msuicc_close_channel_s_req_v3(state);
      break;

    case QBI_UT_MSUICC_RESET_Q_3:
      qbi_ut_msuicc_reset_q_req(state);
      break;

    #endif/* QBI_UT_TEST_UICC_RESET */

    #ifdef QBI_UT_TEST_DEVICE_RESET
    case QBI_UT_DEVICE_RESET_S:
      qbi_ut_device_reset_s_req(state);
      break;
    #endif/* QBI_UT_TEST_DEVICE_RESET */

    #ifdef QBI_UT_TEST_MISC
    case QBI_UT_UIM_REFRESH:
      qbi_ut_misc_uim_refresh(state);
      break;
    #endif

    #ifdef QBI_UT_TEST_ATDS
    case QBI_UT_ATDS_PROJECTION_TABLES_S:
      qbi_ut_atds_projection_tables_s_req(state);
      break;

    case QBI_UT_ATDS_PROJECTION_TABLES_Q:
      qbi_ut_atds_projection_tables_q_req(state);
      break;

    case QBI_UT_ATDS_SIGNAL_Q:
      qbi_ut_atds_signal_q_req(state);
      break;

    case QBI_UT_ATDS_LOCATION_INFO_Q:
      qbi_ut_atds_location_info_q_req(state);
      break;

    case QBI_UT_ATDS_RAT_S:
      qbi_ut_atds_rat_s_req(state);
      break;

    case QBI_UT_ATDS_RAT_Q:
      qbi_ut_atds_rat_q_req(state);
      break;

    case QBI_UT_ATDS_OPERATORS_S:
      qbi_ut_atds_operators_s_req(state);
      break;

    case QBI_UT_ATDS_OPERATORS_Q:
      qbi_ut_atds_operators_q_req(state);
      use_timeout = FALSE;
      break;

    case QBI_UT_ATDS_REGISTER_STATE_Q:
      qbi_ut_atds_register_state_q_req(state);
      break;
    #endif /* QBI_UT_TEST_ATDS */

    #ifdef QBI_UT_TEST_PROVISIONED_CONTEXTS
    case QBI_UT_PROVISIONED_CONTEXTS_Q:
      qbi_ut_bc_provisioned_contexts_q_req(state);
      break;

    case QBI_UT_PROVISIONED_CONTEXTS_S:
      qbi_ut_bc_provisioned_contexts_s_req(state);
    break;

    case QBI_UT_PROVISIONED_CONTEXTS_S_APPEND:
      qbi_ut_bc_provisioned_contexts_s_append_req(state);
      break;

    #endif /* QBI_UT_TEST_PROVISIONED_CONTEXTS */

    #ifdef QBI_UT_TEST_PROVISIONED_CONTEXTS_V2
    case QBI_UT_PROVISIONED_CONTEXTS_V2_Q:
      qbi_ut_bc_provisioned_contexts_q_v2_req(state);
      break;

    case QBI_UT_PROVISIONED_CONTEXTS_V2_S:
      qbi_ut_bc_provisioned_contexts_s_v2_req(state);
      break;

    case QBI_UT_PROVISIONED_CONTEXTS_V2_S_APPEND:
      //Need to add UT
      break;

    #endif /* QBI_UT_TEST_PROVISIONED_CONTEXTS_V2 */

    #ifdef QBI_UT_TEST_SLOT_MAPPING
    case QBI_UT_SLOT_MAPPING_Q:
      qbi_ut_bc_ext_slot_mapping_q_req(state);
      break;

    case QBI_UT_SLOT_MAPPING_S:
      qbi_ut_bc_ext_slot_mapping_s_req(state);
      break;
    #endif /* QBI_UT_TEST_SLOT_MAPPING */

    #ifdef QBI_UT_TEST_SLOT_INFO
    case QBI_UT_SLOT_INFO_Q:
      qbi_ut_bc_ext_slot_info_q_req(state);
      break;
    #endif /* QBI_UT_TEST_SLOT_INFO */

    #ifdef QBI_UT_TEST_LTE_ATTACH
    case QBI_UT_LTE_ATTACH_CONFIG_Q:
      qbi_ut_bc_lte_attach_q_req(state);
      break;

    case QBI_UT_LTE_ATTACH_CONFIG_S:
      qbi_ut_bc_lte_attach_s_req(state);
      break;
    #endif

    #ifdef QBI_UT_TEST_PCO_INFO
    case QBI_UT_TEST_PCO_INFO_Q:
      qbi_ut_bc_ext_pco_info_q_req(state);
      break;
    #endif /* QBI_UT_TEST_PCO_INFO */

    case QBI_UT_CLOSE:
      qbi_ut_tx_close_msg(state);
      break;

    #ifdef QBI_UT_TEST_UTF
    case QBI_UT_UTF_RUN:
      qbi_utf_run();
      use_timeout = FALSE;
      break;

    case QBI_UT_UTF_RUNNING:
      /* Once the unit test framework is started, QBI UT cannot run any more
         of its own tests. */
      QBI_LOG_W_0("QBI UT tried to advance past UTF; not supported");
      state->state--;
      use_timeout = FALSE;
      break;
    #endif /* QBI_UT_TEST_UTF */

    default:
      QBI_LOG_W_0("Unit testing completed. Restart in 120s");
      qbi_os_timer_set(&state->timer, 120000);
      use_timeout = FALSE;
      state->state = QBI_UT_INIT; /* Will get incremented past init */
  }

  if (use_timeout)
  {
    qbi_os_timer_set(&state->timeout, 10000);
  }
  else
  {
    qbi_os_timer_clear(&state->timeout);
  }
  state->state++;
  QBI_LOG_W_2("UT run finish state %d ctx %d", state->state, state->ctx);
} /* qbi_ut_run() */


/*=============================================================================

  Public Function Definitions

=============================================================================*/


/*===========================================================================
  FUNCTION: qbi_ut_init
===========================================================================*/
/*!
    @brief Initialize and start the unit test procedure

    @details

*/
/*=========================================================================*/
void qbi_ut_init
(
  void
)
{
/*-------------------------------------------------------------------------*/
  QBI_LOG_W_1("qbi UT init, state %d", qbi_ut_state.state);
  qbi_utf_init();
  qbi_ut_state.state = QBI_UT_INIT;
  qbi_ut_run(&qbi_ut_state);
} /* qbi_ut_init() */

/*===========================================================================
  FUNCTION: qbi_ut_rx
===========================================================================*/
/*!
    @brief Receive a packet from QBI

    @details

    @param len
*/
/*=========================================================================*/
void qbi_ut_rx
(
  qbi_ctx_s  *ctx,
  const void *data,
  uint32      len
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(ctx);
  #ifdef QBI_UT_TEST_UTF
  if (qbi_ut_state.state == QBI_UT_UTF_RUN ||
      qbi_ut_state.state == QBI_UT_UTF_RUNNING)
  {
    qbi_utf_cid_receive_raw(ctx, data, len);
    return;
  }
  #endif /* QBI_UT_TEST_UTF */

  qbi_log_pkt(ctx->id, QBI_LOG_PKT_DIRECTION_TX, data, len);

  /*! @note This currently assumes only one instance of qbi_ut_state. For
      multiple instances, could look up qbi_ut_state based on ctx */
  if (qbi_ut_txn_id_matches(data, len, qbi_ut_state.txn_id))
  {
    if (qbi_ut_state.verify_cb == NULL)
    {
      QBI_LOG_I_0("Not verifying response");
    }
    else
    {
      qbi_ut_state.verify_cb(data, len);
    }
    qbi_ut_state.verify_cb = NULL;
    QBI_LOG_I_0("Continuing test in 500ms");
    qbi_os_timer_set(&qbi_ut_state.timer, 500);
  }
  else
  {
    QBI_LOG_I_0("Ignoring message, not expected");
  }
} /* qbi_ut_rx() */

#endif /* QBI_UNIT_TEST */

