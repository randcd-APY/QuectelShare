#ifndef QL_MANAGER_ACCESS_MSGR_WIFI_SERVICE_01_H
#define QL_MANAGER_ACCESS_MSGR_WIFI_SERVICE_01_H
/**
  @file ql_manager_access_msgr_wifi_v01.h

  @brief This is the public header file which defines the ql_manager_access_msgr_wifi service Data structures.

  This header file defines the types and structures that were defined in
  ql_manager_access_msgr_wifi. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  

  
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Sat Aug 11 2018 (Spin 0)
   From IDL File: ql_manager_access_msgr_wifi_v01.idl */

/** @defgroup ql_manager_access_msgr_wifi_qmi_consts Constant values defined in the IDL */
/** @defgroup ql_manager_access_msgr_wifi_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ql_manager_access_msgr_wifi_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ql_manager_access_msgr_wifi_qmi_messages Structures sent as QMI messages */
/** @defgroup ql_manager_access_msgr_wifi_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ql_manager_access_msgr_wifi_qmi_accessor Accessor for QMI service object */
/** @defgroup ql_manager_access_msgr_wifi_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "ql_manager_access_msgr_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ql_manager_access_msgr_wifi_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_WIFI_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_WIFI_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QL_MANAGER_ACCESS_MSGR_WIFI_V01_IDL_TOOL_VERS 0x06

/**
    @}
  */


/** @addtogroup ql_manager_access_msgr_wifi_qmi_consts
    @{
  */

/**  the maximum station numbers. */
#define WIFI_MAX_AP_STA_INFO_CNT_V01 16

/**  the maximum mac address acl numbers. */
#define WIFI_MAX_MAC_ACL_CNT_V01 32
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_INDEX_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_IDX_AP_0_V01 = 1, 
  WIFI_IDX_AP_1_V01 = 2, 
  WIFI_IDX_STA_0_V01 = 3, 
  WIFI_INDEX_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_index_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_WORK_MODE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_WORK_MODE_AP_V01 = 0, 
  WIFI_WORK_MODE_STA_V01 = 1, 
  WIFI_WORK_MODE_AP_STA_V01 = 2, 
  WIFI_WORK_MODE_AP_AP_V01 = 3, 
  WIFI_WORK_MODE_MAX_V01 = 4, 
  WIFI_WORK_MODE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_work_mode_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_ACTIVE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_START_V01 = 0, 
  WIFI_STOP_V01 = 1, 
  WIFI_RESTART_V01 = 2, 
  WIFI_ACTIVE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_active_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_IEEE80211_MODE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_MODE_11A_V01 = 0, 
  WIFI_MODE_11AN_V01 = 1, 
  WIFI_MODE_11B_V01 = 2, 
  WIFI_MODE_11BG_V01 = 3, 
  WIFI_MODE_11BGN_V01 = 4, 
  WIFI_MODE_11AC_V01 = 5, 
  WIFI_IEEE80211_MODE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_ieee80211_mode_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_BANDWIDTH_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_BANDWIDTH_20MHZ_V01 = 0, 
  WIFI_BANDWIDTH_40MHZ_V01 = 1, 
  WIFI_BANDWIDTH_80MHZ_V01 = 2, 
  WIFI_BANDWIDTH_160MHZ_V01 = 3, 
  WIFI_BANDWIDTH_MAX_V01 = 4, 
  WIFI_BANDWIDTH_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_bandwidth_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_AUTH_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_AUTH_OPEN_V01 = 0, 
  WIFI_AUTH_WEP_V01 = 1, 
  WIFI_AUTH_WPA_PSK_V01 = 2, 
  WIFI_AUTH_WPA2_PSK_V01 = 3, 
  WIFI_AUTH_WPA_WPA2_PSK_BOTH_V01 = 4, 
  WIFI_AUTH_WPA_V01 = 5, 
  WIFI_AUTH_WPA2_V01 = 6, 
  WIFI_AUTH_WPA_WPA2_BOTH_V01 = 7, 
  WIFI_AUTH_WPS_V01 = 8, 
  WIFI_AUTH_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_auth_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_WPA_PAIRWISE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_AUTH_WPA_PAIRWISE_TKIP_V01 = 0, 
  WIFI_AUTH_WPA_PAIRWISE_AES_V01 = 1, 
  WIFI_AUTH_WPA_PAIRWISE_BOTH_V01 = 2, 
  WIFI_WPA_PAIRWISE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_wpa_pairwise_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_RSSI_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_RSSI_MIN_V01 = 0, 
  WIFI_RSSI_MAX_V01 = 95, 
  WIFI_RSSI_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_rssi_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_ENABLE_STATE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_ENABLE_SUCCESS_V01 = 0, 
  WIFI_ENABLE_FAILED_DRIVER_V01 = 1, 
  WIFI_ENABLE_FAILED_SOFTWARE_V01 = 2, 
  WIFI_ENABLE_FAILED_NOT_SUPPORT_V01 = 3, 
  WIFI_ENABLE_STATE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_enable_state_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_AP_STA_ACTION_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_AP_STA_CONNECTED_V01 = 0, 
  WIFI_AP_STA_DISCONNECTD_V01 = 1, 
  WIFI_AP_STA_ACTION_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_ap_sta_action_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_enums
    @{
  */
typedef enum {
  WIFI_MAC_ACL_RULE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WIFI_MAC_ACL_RULE_NONE_V01 = 0, 
  WIFI_MAC_ACL_RULE_BLACK_V01 = 1, 
  WIFI_MAC_ACL_RULE_WHITE_V01 = 2, 
  WIFI_MAC_ACL_RULE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wifi_mac_acl_rule_e_v01;
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t work_mode;
}ql_manager_msgr_set_wifi_work_mode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_work_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_wifi_work_mode_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t work_mode;
}ql_manager_msgr_get_wifi_work_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  char essid[32];
}ql_manager_msgr_set_wifi_essid_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_essid_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_essid_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  char essid[32];
}ql_manager_msgr_get_wifi_essid_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t country_code[2];
}ql_manager_msgr_set_wifi_country_code_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_country_code_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_wifi_country_code_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t country_code[2];
}ql_manager_msgr_get_wifi_country_code_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t mode;
}ql_manager_msgr_set_wifi_mode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_mode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t mode;
}ql_manager_msgr_get_wifi_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint16_t channel;
}ql_manager_msgr_set_wifi_channel_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_channel_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_channel_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint16_t channel;
}ql_manager_msgr_get_wifi_channel_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t bandwidth;
}ql_manager_msgr_set_wifi_bandwidth_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_bandwidth_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_bandwidth_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t bandwidth;
}ql_manager_msgr_get_wifi_bandwidth_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t max_num;
}ql_manager_msgr_set_wifi_max_num_sta_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_max_num_sta_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_max_num_sta_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t max_num;
}ql_manager_msgr_get_wifi_max_num_sta_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t ht;
}ql_manager_msgr_set_wifi_require_ht_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_require_ht_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_require_ht_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t ht;
}ql_manager_msgr_get_wifi_require_ht_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t ht;
}ql_manager_msgr_set_wifi_require_vht_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_require_vht_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_require_vht_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t ht;
}ql_manager_msgr_get_wifi_require_vht_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t ap_isolate;
}ql_manager_msgr_set_wifi_ap_isolate_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_ap_isolate_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_ap_isolate_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t ap_isolate;
}ql_manager_msgr_get_wifi_ap_isolate_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t default_index;

  char key0[64];

  char key1[64];

  char key2[64];

  char key3[64];
}__auth_wep_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t wpa;

  uint8_t wpa_key_mgmt;

  uint8_t wpa_pairwise;

  char wpa_passphrase[64];

  uint32_t wpa_group_rekey;
}__auth_psk_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t auth;

  /* Mandatory */
  __auth_wep_v01 wep;

  /* Mandatory */
  __auth_psk_v01 psk;
}ql_manager_msgr_set_wifi_auth_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_auth_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_auth_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t auth;

  /* Mandatory */
  __auth_wep_v01 wep;

  /* Mandatory */
  __auth_psk_v01 psk;
}ql_manager_msgr_get_wifi_auth_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t dfs;
}ql_manager_msgr_set_wifi_dfs_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_dfs_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_dfs_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t dfs;
}ql_manager_msgr_get_wifi_dfs_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t rssi;
}ql_manager_msgr_set_wifi_rssi_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_rssi_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_rssi_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t rssi;
}ql_manager_msgr_get_wifi_rssi_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t hidden;
}ql_manager_msgr_set_wifi_ssid_hide_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_ssid_hide_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_ssid_hide_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t hidden;
}ql_manager_msgr_get_wifi_ssid_hide_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t state;
}ql_manager_msgr_set_wifi_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t reason_code;
}ql_manager_msgr_set_wifi_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_wifi_enable_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t state;
}ql_manager_msgr_get_wifi_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t action_type;
}ql_manager_msgr_set_wifi_active_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_active_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t action_state;
}ql_manager_msgr_set_wifi_active_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_sta_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  char essid[32];

  /* Mandatory */
  uint8_t state;
}ql_manager_msgr_get_wifi_sta_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  char essid[32];

  /* Mandatory */
  uint8_t state;
}ql_manager_msgr_get_wifi_sta_status_event_ind_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t idx;

  uint8_t mac[6];

  char hostname[256];

  uint32_t ipv4;

  char ipv6[16];
}__ap_sta_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}ql_manager_msgr_get_wifi_ap_sta_info_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint32_t cnt;

  /* Mandatory */
  __ap_sta_info_v01 sta[WIFI_MAX_AP_STA_INFO_CNT_V01];
}ql_manager_msgr_get_wifi_ap_sta_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t action;

  /* Mandatory */
  uint32_t idx;

  /* Mandatory */
  uint8_t mac[6];
}ql_manager_msgr_get_wifi_ap_sta_info_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx_invald;

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_set_wifi_restore_default_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_restore_default_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  uint8_t rule;
}ql_manager_msgr_set_wifi_mac_acl_rule_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_set_wifi_mac_acl_rule_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_mac_acl_rule_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  uint8_t rule;
}ql_manager_msgr_get_wifi_mac_acl_rule_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t mac[6];
}__wifi_mac_acl_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  __wifi_mac_acl_v01 acl;
}ql_manager_msgr_add_wifi_mac_acl_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_add_wifi_mac_acl_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;

  /* Mandatory */
  __wifi_mac_acl_v01 acl;
}ql_manager_msgr_del_wifi_mac_acl_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;
}ql_manager_msgr_del_wifi_mac_acl_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  uint8_t idx;
}ql_manager_msgr_get_wifi_mac_acl_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ql_manager_access_msgr_wifi_qmi_messages
    @{
  */
/**  Message;  */
typedef struct {

  /* Mandatory */
  result_v01 res;

  /* Mandatory */
  int32_t cnt;

  /* Mandatory */
  __wifi_mac_acl_v01 acl[WIFI_MAX_MAC_ACL_CNT_V01];
}ql_manager_msgr_get_wifi_mac_acl_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 

/*Extern Definition of Type Table Object*/
/*THIS IS AN INTERNAL OBJECT AND SHOULD ONLY*/
/*BE ACCESSED BY AUTOGENERATED FILES*/
extern const qmi_idl_type_table_object ql_manager_access_msgr_wifi_qmi_idl_type_table_object_v01;


#ifdef __cplusplus
}
#endif
#endif

