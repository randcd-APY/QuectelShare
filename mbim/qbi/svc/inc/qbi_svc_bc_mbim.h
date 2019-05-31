/*!
  @file
  qbi_svc_bc_mbim.h

  @brief
  Basic Connectivity device service definitions provided by the MBIM
  specification, e.g. InformationBuffer structures, enums, etc.
*/

/*=============================================================================

  Copyright (c) 2011-2013, 2017 Qualcomm Technologies, Inc.
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
07/12/17  rv   Added Auth Type Auto
06/26/17  vs   Fixed eSIM no profile state value
03/13/13  hz   Add support for multiple provisioned contexts and data sessions
12/05/12  bd   Update maximum phone number length to 44 bytes per MBIM Errata
10/28/11  bd   Updated to MBIM v1.0 SC
09/02/11  bd   Updated to MBIM v0.81c
08/08/11  bd   Fix multiple issues with PIN set operations
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_SVC_BC_MBIM_H
#define QBI_SVC_BC_MBIM_H

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
  QBI_SVC_BC_CID_MIN = 0,

  QBI_SVC_BC_MBIM_CID_DEVICE_CAPS             = 1,
  QBI_SVC_BC_MBIM_CID_SUBSCRIBER_READY_STATUS = 2,
  QBI_SVC_BC_MBIM_CID_RADIO_STATE             = 3,
  QBI_SVC_BC_MBIM_CID_PIN                     = 4,
  QBI_SVC_BC_MBIM_CID_PIN_LIST                = 5,

  QBI_SVC_BC_MBIM_CID_HOME_PROVIDER       = 6,
  QBI_SVC_BC_MBIM_CID_PREFERRED_PROVIDERS = 7,
  QBI_SVC_BC_MBIM_CID_VISIBLE_PROVIDERS   = 8,
  QBI_SVC_BC_MBIM_CID_REGISTER_STATE      = 9,
  QBI_SVC_BC_MBIM_CID_PACKET_SERVICE      = 10,

  QBI_SVC_BC_MBIM_CID_SIGNAL_STATE          = 11,
  QBI_SVC_BC_MBIM_CID_CONNECT               = 12,
  QBI_SVC_BC_MBIM_CID_PROVISIONED_CONTEXTS  = 13,
  QBI_SVC_BC_MBIM_CID_SERVICE_ACTIVATION    = 14,
  QBI_SVC_BC_MBIM_CID_IP_CONFIGURATION_INFO = 15,

  QBI_SVC_BC_MBIM_CID_DEVICE_SERVICES               = 16,
  QBI_SVC_BC_MBIM_CID_RESERVED_1                    = 17,
  QBI_SVC_BC_MBIM_CID_RESERVED_2                    = 18,
  QBI_SVC_BC_MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST = 19,
  QBI_SVC_BC_MBIM_CID_PACKET_STATISTICS             = 20,

  QBI_SVC_BC_MBIM_CID_NETWORK_IDLE_HINT      = 21,
  QBI_SVC_BC_MBIM_CID_EMERGENCY_MODE         = 22,
  QBI_SVC_BC_MBIM_CID_PACKET_FILTERS         = 23,
  QBI_SVC_BC_MBIM_CID_MULTICARRIER_PROVIDERS = 24,

  QBI_SVC_BC_CID_MAX
} qbi_svc_bc_cid_e;

/*=============================================================================

  Definitions Specific to CIDs

=============================================================================*/

/* All message format structs are packed, so start 1 byte alignment here. Use
   push to save the previous alignment. */
#ifdef _WIN32
#pragma pack(push,1)
#endif

/*! @addtogroup MBIM_CID_DEVICE_CAPS
    @{ */

/* MBIM_DEVICE_TYPE */
#define QBI_SVC_BC_DEVICE_TYPE_UNKNOWN   (0)
#define QBI_SVC_BC_DEVICE_TYPE_EMBEDDED  (1)
#define QBI_SVC_BC_DEVICE_TYPE_REMOVABLE (2)
#define QBI_SVC_BC_DEVICE_TYPE_REMOTE    (3)

/* MBIM_CELLULAR_CLASS */
#define QBI_SVC_BC_CELLULAR_CLASS_GSM  (0x00000001)
#define QBI_SVC_BC_CELLULAR_CLASS_CDMA (0x00000002)

/* MBIM_VOICE_CLASS */
#define QBI_SVC_BC_VOICE_CLASS_UNKNOWN                 (0)
#define QBI_SVC_BC_VOICE_CLASS_NO_VOICE                (1)
#define QBI_SVC_BC_VOICE_CLASS_SEPARATE_VOICE_DATA     (2)
#define QBI_SVC_BC_VOICE_CLASS_SIMULTANEOUS_VOICE_DATA (3)

/* MBIM_SIM_CLASS */
#define QBI_SVC_BC_SIM_CLASS_LOGICAL   (1)
#define QBI_SVC_BC_SIM_CLASS_REMOVABLE (2)

/* MBIM_DATA_CLASS */
#define QBI_SVC_BC_DATA_CLASS_NONE  (0x00000000)

#define QBI_SVC_BC_DATA_CLASS_GPRS  (0x00000001)
#define QBI_SVC_BC_DATA_CLASS_EDGE  (0x00000002)
#define QBI_SVC_BC_DATA_CLASS_UMTS  (0x00000004)
#define QBI_SVC_BC_DATA_CLASS_HSDPA (0x00000008)
#define QBI_SVC_BC_DATA_CLASS_HSUPA (0x00000010)
#define QBI_SVC_BC_DATA_CLASS_LTE   (0x00000020)

#define QBI_SVC_BC_DATA_CLASS_1XRTT       (0x00010000)
#define QBI_SVC_BC_DATA_CLASS_1XEVDO      (0x00020000)
#define QBI_SVC_BC_DATA_CLASS_1XEVDO_REVA (0x00040000)
#define QBI_SVC_BC_DATA_CLASS_1XEVDV      (0x00080000)
#define QBI_SVC_BC_DATA_CLASS_3XRTT       (0x00100000)
#define QBI_SVC_BC_DATA_CLASS_1XEVDO_REVB (0x00200000)
#define QBI_SVC_BC_DATA_CLASS_UMB         (0x00400000)

#define QBI_SVC_BC_DATA_CLASS_CUSTOM (0x80000000)

/* MBIM_DATA_CLASS bitmasks including a full RAT family */
#define QBI_SVC_BC_DATA_CLASS_FAMILY_GSM \
  (QBI_SVC_BC_DATA_CLASS_GPRS | QBI_SVC_BC_DATA_CLASS_EDGE)

#define QBI_SVC_BC_DATA_CLASS_FAMILY_WCDMA \
  (QBI_SVC_BC_DATA_CLASS_UMTS | QBI_SVC_BC_DATA_CLASS_HSDPA | \
   QBI_SVC_BC_DATA_CLASS_HSUPA)

#define QBI_SVC_BC_DATA_CLASS_FAMILY_HDR \
  (QBI_SVC_BC_DATA_CLASS_1XEVDO | QBI_SVC_BC_DATA_CLASS_1XEVDO_REVA | \
   QBI_SVC_BC_DATA_CLASS_1XEVDO_REVB)

#define QBI_SVC_BC_DATA_CLASS_3GPP_FAMILY \
  (QBI_SVC_BC_DATA_CLASS_GPRS  | QBI_SVC_BC_DATA_CLASS_EDGE | \
   QBI_SVC_BC_DATA_CLASS_UMTS  | QBI_SVC_BC_DATA_CLASS_HSDPA | \
   QBI_SVC_BC_DATA_CLASS_HSUPA | QBI_SVC_BC_DATA_CLASS_LTE | \
   QBI_SVC_BC_DATA_CLASS_CUSTOM)

#define QBI_SVC_BC_DATA_CLASS_3GPP2_FAMILY \
  (QBI_SVC_BC_DATA_CLASS_1XRTT       | QBI_SVC_BC_DATA_CLASS_1XEVDO | \
   QBI_SVC_BC_DATA_CLASS_1XEVDO_REVA | QBI_SVC_BC_DATA_CLASS_1XEVDV | \
   QBI_SVC_BC_DATA_CLASS_3XRTT       | QBI_SVC_BC_DATA_CLASS_1XEVDO_REVB)

/* MBIM_SMS_CAPS */
#define QBI_SVC_BC_SMS_CAPS_NONE         (0x00000000)
#define QBI_SVC_BC_SMS_CAPS_PDU_RECEIVE  (0x00000001)
#define QBI_SVC_BC_SMS_CAPS_PDU_SEND     (0x00000002)
#define QBI_SVC_BC_SMS_CAPS_TEXT_RECEIVE (0x00000004)
#define QBI_SVC_BC_SMS_CAPS_TEXT_SEND    (0x00000008)

/* MBIM_CTRL_CAPS */
#define QBI_SVC_BC_CTRL_CAPS_NONE            (0x00000000)
#define QBI_SVC_BC_CTRL_CAPS_REG_MANUAL      (0x00000001)
#define QBI_SVC_BC_CTRL_CAPS_HW_RADIO_SWITCH (0x00000002)
#define QBI_SVC_BC_CTRL_CAPS_CDMA_MOBILE_IP  (0x00000004)
#define QBI_SVC_BC_CTRL_CAPS_CDMA_SIMPLE_IP  (0x00000008)
#define QBI_SVC_BC_CTRL_CAPS_MULTI_CARRIER   (0x00000010)

/* Maximum sizes of the variable length fields */
#define QBI_SVC_BC_CUST_DATA_CLASS_MAX_LEN_BYTES (22)
#define QBI_SVC_BC_DEVICE_ID_MAX_LEN_BYTES       (36)
#define QBI_SVC_BC_FW_INFO_MAX_LEN_BYTES         (60)
#define QBI_SVC_BC_HW_INFO_MAX_LEN_BYTES         (60)

/* MBIM_DEVICE_CAPS_INFO */
typedef PACK(struct) {
  uint32 device_type;
  uint32 cellular_class;
  uint32 voice_class;
  uint32 sim_class;
  uint32 data_class;
  uint32 sms_caps;
  uint32 ctrl_caps;
  uint32 max_sessions;

  /*! Represent the custom data class as a UTF-16 string through field. Must
      also set the custom data class bit in data_class */
  qbi_mbim_offset_size_pair_s cust_data_class;

  /*! IMEI, or MEID if C2K only device */
  qbi_mbim_offset_size_pair_s device_id;

  /*! Firmware-specific information, i.e. build string */
  qbi_mbim_offset_size_pair_s fw_info;
  qbi_mbim_offset_size_pair_s hw_info;

  /*! @note Followed by DataBuffer containing CustomDataClass, DeviceId,
      FirmwareInfo, and HardwareInfo */
} qbi_svc_bc_device_caps_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_SUBSCRIBER_READY_STATUS
    @{ */

/* MBIM_SUBSCRIBER_READY_STATE */
#define QBI_SVC_BC_READY_STATE_NOT_INITIALIZED  (0)
#define QBI_SVC_BC_READY_STATE_INITIALIZED      (1)
#define QBI_SVC_BC_READY_STATE_SIM_NOT_INSERTED (2)
#define QBI_SVC_BC_READY_STATE_BAD_SIM          (3)
#define QBI_SVC_BC_READY_STATE_FAILURE          (4)
#define QBI_SVC_BC_READY_STATE_NOT_ACTIVATED    (5)
#define QBI_SVC_BC_READY_STATE_DEVICE_LOCKED    (6)
#define QBI_SVC_BC_READY_STATE_NO_ESIM_PROFILE  (7)

/* MBIM_UNIQUE_ID_FLAGS */
#define QBI_SVC_BC_UNIQUE_ID_FLAG_NONE              (0)
#define QBI_SVC_BC_UNIQUE_ID_FLAG_PROTECT_UNIQUE_ID (1)

/* Maximum sizes of the variable length fields */
#define QBI_SVC_BC_SUBSCRIBER_ID_MAX_LEN_BYTES (30)
#define QBI_SVC_BC_SIM_ICCID_MAX_LEN_BYTES     (40)
#define QBI_SVC_BC_PHONE_NUM_MAX_LEN_BYTES     (44)

/* MBIM_SUBSCRIBER_READY_INFO */
typedef PACK(struct) {
  uint32 ready_state;
  qbi_mbim_offset_size_pair_s subscriber_id;
  qbi_mbim_offset_size_pair_s sim_iccid;
  uint32 unique_id_flags;
  uint32 phone_num_count;
  /*! @note Followed by phone_num_count elements of
      qbi_mbim_offset_size_pair_s, then the DataBuffer
      containing TelephoneNumber(s), SubscriberId, and SimIccid */
} qbi_svc_bc_subscriber_ready_status_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_RADIO_STATE
    @{ */

#define QBI_SVC_BC_RADIO_STATE_RADIO_OFF (0)
#define QBI_SVC_BC_RADIO_STATE_RADIO_ON  (1)

typedef PACK(struct) {
  uint32 radio_state;
} qbi_svc_bc_radio_state_s_req_s;

typedef PACK(struct) {
  uint32 hw_radio_state;
  uint32 sw_radio_state;
} qbi_svc_bc_radio_state_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_PIN
    @{ */

/* MBIM_PIN_TYPE */
#define QBI_SVC_BC_PIN_TYPE_NONE                  (0)
#define QBI_SVC_BC_PIN_TYPE_CUSTOM                (1)
#define QBI_SVC_BC_PIN_TYPE_PIN1                  (2)
#define QBI_SVC_BC_PIN_TYPE_PIN2                  (3)
#define QBI_SVC_BC_PIN_TYPE_DEVICE_SIM_PIN        (4)
#define QBI_SVC_BC_PIN_TYPE_DEVICE_FIRST_SIM_PIN  (5)
#define QBI_SVC_BC_PIN_TYPE_NETWORK_PIN           (6)
#define QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PIN    (7)
#define QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PIN      (8)
#define QBI_SVC_BC_PIN_TYPE_CORPORATE_PIN         (9)
#define QBI_SVC_BC_PIN_TYPE_SUBSIDY_LOCK          (10)
#define QBI_SVC_BC_PIN_TYPE_PUK1                  (11)
#define QBI_SVC_BC_PIN_TYPE_PUK2                  (12)
#define QBI_SVC_BC_PIN_TYPE_DEVICE_FIRST_SIM_PUK  (13)
#define QBI_SVC_BC_PIN_TYPE_NETWORK_PUK           (14)
#define QBI_SVC_BC_PIN_TYPE_NETWORK_SUBSET_PUK    (15)
#define QBI_SVC_BC_PIN_TYPE_SVC_PROVIDER_PUK      (16)
#define QBI_SVC_BC_PIN_TYPE_CORPORATE_PUK         (17)

/* MBIM_PIN_STATE */
#define QBI_SVC_BC_PIN_STATE_UNLOCKED (0)
#define QBI_SVC_BC_PIN_STATE_LOCKED   (1)

/* MBIM_PIN_OPERATION */
#define QBI_SVC_BC_PIN_OP_ENTER   (0)
#define QBI_SVC_BC_PIN_OP_ENABLE  (1)
#define QBI_SVC_BC_PIN_OP_DISABLE (2)
#define QBI_SVC_BC_PIN_OP_CHANGE  (3)

/* Indicates that the device does not support reporting how many retries are
   left for the associated PIN */
#define QBI_SVC_BC_RETRIES_LEFT_NOT_SUPPORTED (0xFFFFFFFF)

/*! MBIM_PIN_INFO, provided in set and query response */
typedef PACK(struct) {
  uint32 pin_type;
  uint32 pin_state;
  uint32 retries_left;
} qbi_svc_bc_pin_rsp_s;

/* Maximum sizes of the variable length fields */
#define QBI_SVC_BC_PIN_MAX_LEN_BYTES (32)

/*! MBIM_SET_PIN */
typedef PACK(struct) {
  uint32 pin_type;
  uint32 pin_op;
  qbi_mbim_offset_size_pair_s pin;
  qbi_mbim_offset_size_pair_s new_pin;
  /*! @note Followed by DataBuffer containing Pin and NewPin */
} qbi_svc_bc_pin_s_req_s;

/*! @} */

/*! @addtogroup MBIM_CID_PIN_LIST
    @{ */

/* MBIM_PIN_MODE */
#define QBI_SVC_BC_PIN_MODE_NOT_SUPPORTED (0)
#define QBI_SVC_BC_PIN_MODE_ENABLED       (1)
#define QBI_SVC_BC_PIN_MODE_DISABLED      (2)

/* MBIM_PIN_FORMAT */
#define QBI_SVC_BC_PIN_FORMAT_UNKNOWN       (0)
#define QBI_SVC_BC_PIN_FORMAT_NUMERIC       (1)
#define QBI_SVC_BC_PIN_FORMAT_ALPHA_NUMERIC (2)

/*! Maximum length of a PIN in UTF-16 characters supported by MBIM. Even if we
    support longer than this, we must not report a higher value in
    qbi_svc_bc_pin_desc_s.pin_len_max */
#define QBI_SVC_BC_PIN_LEN_MAX_LEN (16)

/* Identifies that the min/max length for a key is not known */
#define QBI_SVC_BC_PIN_LEN_UNKNOWN (0xFFFFFFFF)

/* MBIM_PIN_DESC */
typedef PACK(struct) {
  uint32 pin_mode;
  uint32 pin_format;
  uint32 pin_len_min;
  uint32 pin_len_max;
} qbi_svc_bc_pin_desc_s;

/* MBIM_PIN_LIST, included in query respose */
typedef PACK(struct) {
  qbi_svc_bc_pin_desc_s pin1;
  qbi_svc_bc_pin_desc_s pin2;
  qbi_svc_bc_pin_desc_s device_sim_pin;
  qbi_svc_bc_pin_desc_s device_first_sim_pin;
  qbi_svc_bc_pin_desc_s network_pin;
  qbi_svc_bc_pin_desc_s network_subset_pin;
  qbi_svc_bc_pin_desc_s svc_provider_pin;
  qbi_svc_bc_pin_desc_s corporate_pin;
  qbi_svc_bc_pin_desc_s subsidy_lock;
  qbi_svc_bc_pin_desc_s custom;
} qbi_svc_bc_pin_list_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_HOME_PROVIDER
    @{ */

/* MBIM_PROVIDER_STATE */
#define QBI_SVC_BC_PROVIDER_STATE_UNKNOWN                (0x00000000)
#define QBI_SVC_BC_PROVIDER_STATE_HOME                   (0x00000001)
#define QBI_SVC_BC_PROVIDER_STATE_FORBIDDEN              (0x00000002)
#define QBI_SVC_BC_PROVIDER_STATE_PREFERRED              (0x00000004)
#define QBI_SVC_BC_PROVIDER_STATE_VISIBLE                (0x00000008)
#define QBI_SVC_BC_PROVIDER_STATE_REGISTERED             (0x00000010)
#define QBI_SVC_BC_PROVIDER_STATE_PREFERRED_MULTICARRIER (0x00000020)

/* Maximum sizes of the variable length fields */
#define QBI_SVC_BC_PROVIDER_ID_MAX_LEN_BYTES   (12)
#define QBI_SVC_BC_PROVIDER_NAME_MAX_LEN_BYTES (40)

/* MBIM_PROVIDER, provided in query response and used in other provider-related
   messages */
typedef PACK(struct) {
  /*! For 3GPP networks, ProviderId is MCC and MNC, concatenated (5 or 6
      digits). For 3GPP2 networks, it is SID and NID concatenated (5 digits). */
  qbi_mbim_offset_size_pair_s provider_id;
  uint32 provider_state;
  qbi_mbim_offset_size_pair_s provider_name;
  uint32 cellular_class;
  uint32 rssi;
  uint32 error_rate;
  /*! @note Followed by DataBuffer containing ProviderId and ProviderName */
} qbi_svc_bc_provider_s;

/*! @} */

/*! @addtogroup MBIM_CID_PREFERRED_PROVIDERS
    @{ */

/* MBIM_PROVIDERS */
typedef PACK(struct) {
  uint32 element_count;
  /*! @note Followed by element_count instances of
      qbi_mbim_offset_size_pair_s, then DataBuffer containing element_count
      qbi_svc_bc_provider_s structures */
} qbi_svc_bc_provider_list_s;

/*! @} */

/*! @addtogroup MBIM_CID_VISIBLE_PROVIDERS
    @{ */

/*! Normal 3GPP manual network scan */
#define QBI_SVC_BC_VISIBLE_PROVIDERS_FULL_SCAN  (0)
/*! From MBIM: Device should perform a restricted scan to locate preferred
    multicarrier providers. The device may also report a static list in case a
    scan is not possible.
    Expectation is that this will complete in <= 10 seconds. */
#define QBI_SVC_BC_VISIBLE_PROVIDERS_QUICK_SCAN (1)

/* MBIM_VISIBLE_PROVIDERS_REQ */
typedef PACK(struct) {
  uint32 action;
} qbi_svc_bc_visible_providers_q_req_s;

/*! @} */

/*! @addtogroup MBIM_CID_REGISTER_STATE
    @{ */

/* MBIM_REGISTER_ACTION */
#define QBI_SVC_BC_REGISTER_ACTION_AUTOMATIC (0)
#define QBI_SVC_BC_REGISTER_ACTION_MANUAL    (1)

/* MBIM_REGISTER_STATE */
#define QBI_SVC_BC_REGISTER_STATE_UNKNOWN       (0)
#define QBI_SVC_BC_REGISTER_STATE_DEREGISTERED  (1)
#define QBI_SVC_BC_REGISTER_STATE_SEARCHING     (2)
#define QBI_SVC_BC_REGISTER_STATE_HOME          (3)
#define QBI_SVC_BC_REGISTER_STATE_ROAMING       (4)
#define QBI_SVC_BC_REGISTER_STATE_PARTNER       (5)
#define QBI_SVC_BC_REGISTER_STATE_DENIED        (6)
#define QBI_SVC_BC_REGISTER_STATE_FORBIDDEN     (7)

/* MBIM_REGISTER_MODE */
#define QBI_SVC_BC_REGISTER_MODE_UNKNOWN   (0)
#define QBI_SVC_BC_REGISTER_MODE_AUTOMATIC (1)
#define QBI_SVC_BC_REGISTER_MODE_MANUAL    (2)

/* MBIM_REGISTRATION_FLAGS */
#define QBI_SVC_BC_REGISTRATION_FLAG_NONE                 (0x00000000)
#define QBI_SVC_BC_REGISTRATION_FLAG_MANUAL_REG_NOT_AVAIL (0x00000001)
#define QBI_SVC_BC_REGISTRATION_FLAG_AUTO_ATTACH          (0x00000002)

/* MBIM_CDMA_DEFAULT_PROVIDER_ID */
#define QBI_SVC_BC_CDMA_DEFAULT_PROVIDER_ID (0)

/* Maximum sizes of the variable length fields */
#define QBI_SVC_BC_ROAMING_TEXT_MAX_LEN_BYTES (126)

/* MBIM_SET_REGISTER_STATE */
typedef PACK(struct) {
  qbi_mbim_offset_size_pair_s provider_id;
  uint32 register_action;
  uint32 data_class;
  /*! @note Followed by a DataBuffer containing ProviderId */
} qbi_svc_bc_register_state_s_req_s;

/* MBIM_REGISTRATION_STATE, provided in event/query/set responses */
typedef PACK(struct) {
  uint32 nw_error;
  uint32 register_state;
  uint32 register_mode;
  uint32 available_data_class;
  uint32 current_cellular_class;
  qbi_mbim_offset_size_pair_s provider_id;
  qbi_mbim_offset_size_pair_s provider_name;
  qbi_mbim_offset_size_pair_s roaming_text;
  uint32 registration_flag;
  /*! @note Followed by a DataBuffer containing ProviderId, ProviderName, and
      RoamingText */
} qbi_svc_bc_register_state_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_SERVICE
    @{ */

/* MBIM_PACKET_SERVICE_ACTION */
#define QBI_SVC_BC_PACKET_SERVICE_ACTION_ATTACH (0)
#define QBI_SVC_BC_PACKET_SERVICE_ACTION_DETACH (1)

/* MBIM_PACKET_SERVICE_STATE */
#define QBI_SVC_BC_PACKET_SERVICE_STATE_UNKNOWN   (0)
#define QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHING (1)
#define QBI_SVC_BC_PACKET_SERVICE_STATE_ATTACHED  (2)
#define QBI_SVC_BC_PACKET_SERVICE_STATE_DETACHING (3)
#define QBI_SVC_BC_PACKET_SERVICE_STATE_DETACHED  (4)

/* MBIM_SET_PACKET_SERVICE, provided in set request */
typedef PACK(struct) {
  uint32 action;
} qbi_svc_bc_packet_service_s_req_s;

/* MBIM_PACKET_SERVICE, provided in event/query/set responses */
typedef PACK(struct) {
  uint32 nw_error;
  uint32 packet_service_state;
  uint32 highest_available_data_class;
  uint64 uplink_speed;
  uint64 downlink_speed;
} qbi_svc_bc_packet_service_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_SIGNAL_STATE
    @{ */

/* When provided in the set request, this means to use the device's default
   value. */
#define QBI_SVC_BC_SIGNAL_STATE_USE_DEFAULT (0)

/* Disable sending event reports based off changes to the error rate */
#define QBI_SVC_BC_SIGNAL_STATE_ERROR_RATE_THRESHOLD_DISABLED (0xFFFFFFFF)

#define QBI_SVC_BC_RSSI_MIN     (0)
#define QBI_SVC_BC_RSSI_MIN_DBM (-113)
#define QBI_SVC_BC_RSSI_MAX     (31)
#define QBI_SVC_BC_RSSI_MAX_DBM (-51)
#define QBI_SVC_BC_RSSI_UNKNOWN (99)
#define QBI_SVC_BC_RSSI_FACTOR  (2)

#define QBI_SVC_BC_ERROR_RATE_MIN     (0)
#define QBI_SVC_BC_ERROR_RATE_MAX     (7)
#define QBI_SVC_BC_ERROR_RATE_UNKNOWN (99)

/* MBIM_SET_SIGNAL_INDICATION, provided in set requests */
typedef PACK(struct) {
  uint32 rssi_interval;
  uint32 rssi_threshold;
  uint32 error_rate_threshold;
} qbi_svc_bc_signal_state_s_req_s;

/* MBIM_SIGNAL_STATE, provided in event/query/set responses */
typedef PACK(struct) {
  uint32 rssi;
  uint32 error_rate;
  uint32 rssi_interval;
  uint32 rssi_threshold;
  uint32 error_rate_threshold;
} qbi_svc_bc_signal_state_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_CONNECT
    @{ */

/* MBIM_ACTIVATION_COMMAND */
#define QBI_SVC_BC_ACTIVATION_CMD_DEACTIVATE (0)
#define QBI_SVC_BC_ACTIVATION_CMD_ACTIVATE   (1)

/* MBIM_COMPRESSION */
#define QBI_SVC_BC_COMPRESSION_NONE   (0)
#define QBI_SVC_BC_COMPRESSION_ENABLE (1)

/* MBIM_AUTH_PROTOCOL */
#define QBI_SVC_BC_AUTH_PROTOCOL_NONE      (0)
#define QBI_SVC_BC_AUTH_PROTOCOL_PAP       (1)
#define QBI_SVC_BC_AUTH_PROTOCOL_CHAP      (2)
#define QBI_SVC_BC_AUTH_PROTOCOL_MSCHAP_V2 (3)
#define QBI_SVC_BC_AUTH_PROTOCOL_AUTO      (4)

/* MBIM_CONTEXT_IP_TYPE */
#define QBI_SVC_BC_IP_TYPE_DEFAULT       (0)
#define QBI_SVC_BC_IP_TYPE_IPV4          (1)
#define QBI_SVC_BC_IP_TYPE_IPV6          (2)
#define QBI_SVC_BC_IP_TYPE_IPV4V6        (3)
#define QBI_SVC_BC_IP_TYPE_IPV4_AND_IPV6 (4)

/* MBIM_ACTIVATION_STATE */
#define QBI_SVC_BC_ACTIVATION_STATE_UNKNOWN      (0)
#define QBI_SVC_BC_ACTIVATION_STATE_ACTIVATED    (1)
#define QBI_SVC_BC_ACTIVATION_STATE_ACTIVATING   (2)
#define QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATED  (3)
#define QBI_SVC_BC_ACTIVATION_STATE_DEACTIVATING (4)

/* MBIM_VOICE_CALL_STATE */
#define QBI_SVC_BC_VOICE_CALL_STATE_NONE        (0)
#define QBI_SVC_BC_VOICE_CALL_STATE_IN_PROGRESS (1)
#define QBI_SVC_BC_VOICE_CALL_STATE_HANG_UP     (2)

/* Array sizes */
#define QBI_SVC_BC_ACCESS_STRING_MAX_LEN_BYTES (200)
#define QBI_SVC_BC_USERNAME_MAX_LEN_BYTES      (510)
#define QBI_SVC_BC_PASSWORD_MAX_LEN_BYTES      (510)

/* MBIM_SET_CONNECT, provided in set requests */
typedef PACK(struct) {
  uint32 session_id;
  uint32 activation_cmd;
  qbi_mbim_offset_size_pair_s access_string;
  qbi_mbim_offset_size_pair_s username;
  qbi_mbim_offset_size_pair_s password;
  uint32 compression;
  uint32 auth_protocol;
  uint32 ip_type;
  uint8  context_type[QBI_MBIM_UUID_LEN];
  /*! @note Followed by a DataBuffer containing AccessString, UserName, and
      Password */
} qbi_svc_bc_connect_s_req_s;

/* MBIM_CONNECT_INFO, provided in query requests and event/query/set responses */
typedef PACK(struct) {
  uint32 session_id;
  uint32 activation_state;
  uint32 voice_call_state;
  uint32 ip_type;
  uint8  context_type[QBI_MBIM_UUID_LEN];
  uint32 nw_error;
} qbi_svc_bc_connect_q_req_s, qbi_svc_bc_connect_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_PROVISIONED_CONTEXTS
    @{ */

#define QBI_SVC_BC_CONTEXT_ID_APPEND (0xFFFFFFFF)

/* MBIM_SET_CONTEXT used in set request */
typedef PACK(struct) {
  uint32 context_id;
  uint8  context_type[QBI_MBIM_UUID_LEN];

  qbi_mbim_offset_size_pair_s access_string;
  qbi_mbim_offset_size_pair_s username;
  qbi_mbim_offset_size_pair_s password;

  uint32 compression;
  uint32 auth_protocol;

  qbi_mbim_offset_size_pair_s provider_id;
  /*! @note Followed by DataBuffer containing AccessString, UserName, Password,
      ProviderId */
} qbi_svc_bc_provisioned_contexts_s_req_s;

/* MBIM_CONTEXT */
typedef PACK(struct) {
  uint32 context_id;
  uint8  context_type[QBI_MBIM_UUID_LEN];

  qbi_mbim_offset_size_pair_s access_string;
  qbi_mbim_offset_size_pair_s username;
  qbi_mbim_offset_size_pair_s password;

  uint32 compression;
  uint32 auth_protocol;
  /*! @note Followed by DataBuffer containing AccessString, UserName, Password */
} qbi_svc_bc_provisioned_contexts_context_s;

/* MBIM_PROVISIONED_CONTEXTS_INFO */
typedef PACK(struct) {
  uint32 element_count;
  /*! @note Followed by element_count instances of qbi_mbim_offset_size_pair_s,
      then element_count qbi_svc_bc_provisioned_contexts_context_s */
} qbi_svc_bc_provisioned_contexts_list_s;

/*! @} */

/*! @addtogroup MBIM_CID_IP_CONFIGURATION_INFO
    @{ */

/* Length of an IPv6 address in bytes */
#define QBI_SVC_BC_IPV6_ADDR_LEN (16)

/* IPv4 Element */
typedef PACK(struct) {
  uint32 prefix_len;
  uint32 ipv4_address;
} qbi_svc_bc_ip_configuration_info_ipv4_element_s;

/* IPv6 Element */
typedef PACK(struct) {
  uint32 prefix_len;
  uint8  ipv6_address[QBI_SVC_BC_IPV6_ADDR_LEN];
} qbi_svc_bc_ip_configuration_info_ipv6_element_s;

/* Bitmask for available configuration information */
#define QBI_SVC_BC_AVAILABLE_IP_CONFIG_ADDRESS (0x00000001)
#define QBI_SVC_BC_AVAILABLE_IP_CONFIG_GATEWAY (0x00000002)
#define QBI_SVC_BC_AVAILABLE_IP_CONFIG_DNS     (0x00000004)
#define QBI_SVC_BC_AVAILABLE_IP_CONFIG_MTU     (0x00000008)

/* MBIM_IP_CONFIGURATION_INFO for query requests and event/query responses */
typedef PACK(struct) {
  uint32 session_id;
  uint32 ipv4_config_avail;
  uint32 ipv6_config_avail;

  uint32 ipv4_address_count;
  uint32 ipv4_address_offset;
  uint32 ipv6_address_count;
  uint32 ipv6_address_offset;

  uint32 ipv4_gateway_offset;
  uint32 ipv6_gateway_offset;

  uint32 ipv4_dns_server_count;
  uint32 ipv4_dns_server_offset;
  uint32 ipv6_dns_server_count;
  uint32 ipv6_dns_server_offset;

  uint32 ipv4_mtu;
  uint32 ipv6_mtu;

  /*! @note Followed by zero or more IPv4 and/or IPv6 elements */
} qbi_svc_bc_ip_configuration_info_q_req_s,
  qbi_svc_bc_ip_configuration_info_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_DEVICE_SERVICES
    @{ */

/* DSS payload flags */
#define QBI_SVC_BC_DSS_PAYLOAD_NOT_SUPPORTED  (0x00000000)
#define QBI_SVC_BC_DSS_PAYLOAD_HOST_TO_DEVICE (0x00000001)
#define QBI_SVC_BC_DSS_PAYLOAD_DEVICE_TO_HOST (0x00000002)

/* MBIM_DEVICE_SERVICE_ELEMENT */
typedef PACK(struct) {
  uint8  uuid[QBI_MBIM_UUID_LEN];
  uint32 dss_payload_flags;
  uint32 max_dss_instances;
  uint32 cid_count;
  /*! @note Followed by cid_count instances of uint32 containing a CID */
} qbi_svc_bc_device_service_element_s;

/* MBIM_DEVICE_SERVICES_INFO */
typedef PACK(struct) {
  uint32 device_services_count;
  uint32 max_dss_sessions;
  /*! @note Followed by device_services_count qbi_mbim_offset_size_pair_s then
      the same number of qbi_svc_bc_device_service_element_s */
} qbi_svc_bc_device_services_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LIST
    @{ */

/* MBIM_EVENT_ENTRY */
typedef PACK(struct) {
  uint8  uuid[QBI_MBIM_UUID_LEN];
  uint32 cid_count;
  /*! @note Followed by cid_count uint32 elements */
} qbi_svc_bc_device_service_subscribe_list_entry_s;

/* MBIM_DEVICE_SERVICE_SUBSCRIBE_LIST */
typedef PACK(struct) {
  uint32 element_count;
  /*! @note Followed by element_count qbi_mbim_offset_size_pair_s elements, each
      describing the location and size of a single instance of
      qbi_svc_bc_device_service_subscribe_list_entry_s */
} qbi_svc_bc_device_service_subscribe_list_s_req_s;

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_STATISTICS
    @{ */

/* MBIM_PACKET_STATISTICS */
typedef PACK(struct) {
  uint32 in_discards;
  uint32 in_errors;
  uint64 in_octets;
  uint64 in_packets;
  uint64 out_octets;
  uint64 out_packets;
  uint32 out_errors;
  uint32 out_discards;
} qbi_svc_bc_packet_statistics_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_NETWORK_IDLE_HINT
    @{ */

/* MBIM_NETWORK_IDLE_HINT_STATES */
#define QBI_SVC_BC_NETWORK_IDLE_HINT_DISABLED (0)
#define QBI_SVC_BC_NETWORK_IDLE_HINT_ENABLED  (1)

/* MBIM_NETWORK_IDLE_HINT, used in both set request and responses */
typedef PACK(struct) {
  uint32 network_idle_hint_state;
} qbi_svc_bc_network_idle_hint_s;

/*! @} */

/*! @addtogroup MBIM_CID_EMERGENCY_MODE
    @{ */

/* MBIM_EMERGENCY_MODE_STATES */
#define QBI_SVC_BC_EMERGENCY_MODE_OFF (0)
#define QBI_SVC_BC_EMERGENCY_MODE_ON  (1)

/* MBIM_EMERGENCY_MODE */
typedef PACK(struct) {
  uint32 emergency_mode;
} qbi_svc_bc_emergency_mode_rsp_s;

/*! @} */

/*! @addtogroup MBIM_CID_PACKET_FILTERS
    @{ */

/* MBIM_SINGLE_PACKET_FILTER */
typedef PACK(struct) {
  uint32 size;
  uint32 filter_offset;
  uint32 mask_offset;
  /*! @note Followed by a DataBuffer containing PacketFilter and PacketMask */
} qbi_mbim_bc_single_packet_filter_s;

/* MBIM_PACKET_FILTERS header info with PFC */
typedef PACK(struct) {
  uint32 session_id;
  uint32 packet_filters_count;
  /*! @note Followed by packet_filters_count instances of
      PacketFilterRefList, then DataBuffer containing
      packet_filters_count qbi_mbim_bc_single_packet_filter_s structures */
} qbi_svc_bc_packet_filters_s;

/*! @} */

/* Revert alignment to what it was previously */
#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* QBI_SVC_BC_MBIM_H */
