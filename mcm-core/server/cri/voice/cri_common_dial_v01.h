
#ifndef CRI_COMMON_DIAL_SERVICE_01_H
#define CRI_COMMON_DIAL_SERVICE_01_H
/**
  @file cri_common_dial_v01.h

  @brief This is the public header file which defines the cri_common_dial service Data structures.

  This header file defines the types and structures that were defined in
  cri_common_dial. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.5
   It was generated on: Mon Dec 16 2013 (Spin )
   From IDL File: cri_common_dial_v01.idl */

/** @defgroup cri_common_dial_qmi_consts Constant values defined in the IDL */
/** @defgroup cri_common_dial_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup cri_common_dial_qmi_enums Enumerated types used in QMI messages */
/** @defgroup cri_common_dial_qmi_messages Structures sent as QMI messages */
/** @defgroup cri_common_dial_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup cri_common_dial_qmi_accessor Accessor for QMI service object */
/** @defgroup cri_common_dial_qmi_version Constant values for versioning information */

#include <stdint.h>
//#include "qmi_idl_lib.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup cri_common_dial_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define CRI_COMMON_DIAL_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define CRI_COMMON_DIAL_V01_IDL_MINOR_VERS 0xFFFFFFFF
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define CRI_COMMON_DIAL_V01_IDL_TOOL_VERS 0x06

/**
    @}
  */


/** @addtogroup cri_common_dial_qmi_consts
    @{
  */
#define CRI_MAX_DIAL_STRING_LENGTH_V01 82
#define CRI_MAX_SUBADDRESS_LENGTH_V01 128
#define CRI_MAX_PASSWORD_LENGTH_V01 4
#define CRI_MAX_CALL_FORWARDING_INFO_V01 13
#define CRI_MAX_USSD_LENGTH_V01 128
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_REQUEST_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_REQUEST_TYPE_AUTOMATIC_V01 = 0,
  CRI_COMMON_DIAL_REQUEST_TYPE_EMERGENCY_AUTOMATIC_V01 = 1,
  CRI_COMMON_DIAL_REQUEST_TYPE_EMERGENCY_IMS_V01 = 2,
  CRI_COMMON_DIAL_REQUEST_TYPE_VOICE_V01 = 3,
  CRI_COMMON_DIAL_REQUEST_TYPE_SS_V01 = 4,
  CRI_COMMON_DIAL_REQUEST_TYPE_USSD_V01 = 5,
  CRI_COMMON_DIAL_REQUEST_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_request_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_RESPONSE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_RESPONSE_TYPE_VOICE_V01 = 0,
  CRI_COMMON_DIAL_RESPONSE_TYPE_SS_V01 = 1,
  CRI_COMMON_DIAL_RESPONSE_TYPE_USSD_V01 = 2,
  CRI_COMMON_DIAL_RESPONSE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_response_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_SS_TYPE_GET_CALL_FORWARDING_V01 = 0,
  CRI_COMMON_DIAL_SS_TYPE_SET_CALL_FORWARDING_V01 = 1,
  CRI_COMMON_DIAL_SS_TYPE_GET_CALL_WAITING_V01 = 2,
  CRI_COMMON_DIAL_SS_TYPE_SET_CALL_WAITING_V01 = 3,
  CRI_COMMON_DIAL_SS_TYPE_GET_CLIR_V01 = 4,
  CRI_COMMON_DIAL_SS_TYPE_SET_CLIR_V01 = 5,
  CRI_COMMON_DIAL_SS_TYPE_SET_FACILITY_LOCK_V01 = 6,
  CRI_COMMON_DIAL_SS_TYPE_CHANGE_CALL_BARRING_PASSWORD_V01 = 7,
  CRI_COMMON_DIAL_SS_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_USSD_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_SEND_USSD_V01 = 0,
  CRI_COMMON_DIAL_CANCEL_USSD_V01 = 1,
  CRI_COMMON_DIAL_USSD_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ussd_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_CALL_FORWARDING_REASON_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_CALL_FORWARD_UNCONDITIONALLY_V01 = 0,
  CRI_COMMON_DIAL_CALL_FORWARD_MOBILEBUSY_V01 = 1,
  CRI_COMMON_DIAL_CALL_FORWARD_NOREPLY_V01 = 2,
  CRI_COMMON_DIAL_CALL_FORWARD_UNREACHABLE_V01 = 3,
  CRI_COMMON_DIAL_CALL_FORWARD_ALLFORWARDING_V01 = 4,
  CRI_COMMON_DIAL_CALL_FORWARD_ALLCONDITIONAL_V01 = 5,
  CRI_COMMON_DIAL_SS_CALL_FORWARDING_REASON_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_call_forwarding_reason_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_CALL_FORWARDING_SERVICE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_ACTIVATE_V01 = 0,
  CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_DEACTIVATE_V01 = 1,
  CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_REGISTER_V01 = 2,
  CRI_COMMON_DIAL_CALL_FORWARDING_SERVICE_ERASE_V01 = 3,
  CRI_COMMON_DIAL_SS_CALL_FORWARDING_SERVICE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_call_forwarding_service_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_CALL_FORWARDING_STATUS_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_CALL_FORWARDING_DISABLED_V01 = 0,
  CRI_COMMON_DIAL_CALL_FORWARDING_ENABLED_V01 = 1,
  CRI_COMMON_DIAL_SS_CALL_FORWARDING_STATUS_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_call_forwarding_status_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_CALL_FORWARDING_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_CALL_FORWARDING_TYPE_VOICE_V01 = 0,
  CRI_COMMON_DIAL_CALL_FORWARDING_TYPE_DATA_V01 = 1,
  CRI_COMMON_DIAL_CALL_FORWARDING_TYPE_VOICE_DATA_V01 = 2,
  CRI_COMMON_DIAL_SS_CALL_FORWARDING_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_call_forwarding_type_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_CALL_WAITING_SERVICE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_CALL_WAITING_VOICE_ENABLED_V01 = 0,
  CRI_COMMON_DIAL_CALL_WAITING_DATA_ENABLED_V01 = 1,
  CRI_COMMON_DIAL_CALL_WAITING_VOICE_DATA_ENABLED_V01 = 2,
  CRI_COMMON_DIAL_CALL_WAITING_DISABLED_V01 = 3,
  CRI_COMMON_DIAL_SS_CALL_WAITING_SERVICE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_call_waiting_service_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_CLIR_ACTION_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_CLIR_INACTIVE_V01 = 0,
  CRI_COMMON_DIAL_CLIR_ACTIVE_V01 = 1,
  CRI_COMMON_DIAL_SS_CLIR_ACTION_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_clir_action_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_CLIR_PRESENTATION_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_CLIR_NOT_PROVISIONED_V01 = 0,
  CRI_COMMON_DIAL_CLIR_PROVISIONED_PERMANENT_MODE_V01 = 1,
  CRI_COMMON_DIAL_CLIR_PRESENTATION_RESTRICTED_V01 = 2,
  CRI_COMMON_DIAL_CLIR_PRESENTATION_ALLOWED_V01 = 3,
  CRI_COMMON_DIAL_SS_CLIR_PRESENTATION_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_clir_presentation_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_CLIR_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_CLIR_TYPE_SUPPRESSION = 0,
  CRI_COMMON_DIAL_CLIR_TYPE_INVOCATION = 1,
  CRI_COMMON_DIAL_SS_CLIR_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_clir_type_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_SET_FACILITY_LOCK_CODE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_FACILITY_CODE_AO_V01 = 0, /**<  BAOC (Barr All Outgoing Calls) (refer 3GPP TS 22.088 [6] clause 1) */
  CRI_COMMON_DIAL_FACILITY_CODE_OI_V01 = 1, /**<  BOIC (Barr Outgoing International Calls) (refer 3GPP TS 22.088 [6] clause 1) */
  CRI_COMMON_DIAL_FACILITY_CODE_OX_V01 = 2, /**<  BOIC-exHC (Barr Outgoing International Calls except to Home Country) (refer 3GPP TS 22.088 [6] clause 1) */
  CRI_COMMON_DIAL_FACILITY_CODE_AI_V01 = 3, /**<  BAIC (Barr All Incoming Calls) (refer 3GPP TS 22.088 [6] clause 2) */
  CRI_COMMON_DIAL_FACILITY_CODE_IR_V01 = 4, /**<  BIC-Roam (Barr Incoming Calls when Roaming outside the home country) (refer 3GPP TS 22.088 [6] clause 2) */
  CRI_COMMON_DIAL_SS_SET_FACILITY_LOCK_CODE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_set_facility_lock_code_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_SET_FACILITY_LOCK_STATUS_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_FACILITY_LOCK_DISABLE_V01 = 0,
  CRI_COMMON_DIAL_FACILITY_LOCK_ENABLE_V01 = 1,
  CRI_COMMON_DIAL_SS_SET_FACILITY_LOCK_STATUS_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_set_facility_lock_status_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_SS_CHANGE_CALL_BARRING_PASSWORD_REASON_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOING_V01 = 0,
  CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINT_V01 = 1,
  CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINTEXTOHOME_V01 = 2,
  CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMING_V01 = 3,
  CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_INCOMINGROAMING_V01 = 4,
  CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLBARRING_V01 = 5,
  CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOINGBARRING_V01 = 6,
  CRI_COMMON_DIAL_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMINGBARRING_V01 = 7,
  CRI_COMMON_DIAL_SS_CHANGE_CALL_BARRING_PASSWORD_REASON_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ss_change_call_barring_password_reason_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_USSD_ENCODING_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_USSD_ENCODING_ASCII_V01 = 0,
  CRI_COMMON_DIAL_USSD_ENCODING_8BIT_V01 = 1,
  CRI_COMMON_DIAL_USSD_ENCODING_UCS2_V01 = 2,
  CRI_COMMON_DIAL_USSD_ENCODING_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ussd_encoding_t_v01;
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_enums
    @{
  */
typedef enum {
  CRI_COMMON_DIAL_USSD_SEND_USSD_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CRI_COMMON_DIAL_USSD_SEND_USSD_TYPE_NEW_V01 = 0,
  CRI_COMMON_DIAL_USSD_SEND_USSD_TYPE_ANSWER_V01 = 1,
  CRI_COMMON_DIAL_USSD_SEND_USSD_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}cri_common_dial_ussd_send_ussd_type_t_v01;
/**
    @}
  */


/** @addtogroup cri_common_dial_qmi_aggregates
    @{
  */
typedef struct {

  cri_common_dial_ss_call_forwarding_type_t_v01 type;

  char number[CRI_MAX_DIAL_STRING_LENGTH_V01 + 1];
}cri_common_dial_ss_call_forwarding_info_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_messages
    @{
  */
/** Request Message; Common entry point for Voice/SS/USSD. */
typedef struct {

  /* Mandatory */
  char dialstring[CRI_MAX_DIAL_STRING_LENGTH_V01 + 1];

  /* Optional */
  uint8_t request_type_valid;  /**< Must be set to true if request_type is being passed */
  cri_common_dial_request_t_v01 request_type;

  /* Optional */
  uint8_t sub_address_valid;  /**< Must be set to true if sub_address is being passed */
  char sub_address[CRI_MAX_SUBADDRESS_LENGTH_V01 + 1];

  /* Optional */
  uint8_t ss_type_valid;  /**< Must be set to true if ss_type is being passed */
  cri_common_dial_ss_t_v01 ss_type;

  /* Optional */
  uint8_t ussd_type_valid;  /**< Must be set to true if ussd_type is being passed */
  cri_common_dial_ussd_t_v01 ussd_type;

  /* Optional */
  uint8_t ss_get_cf_reason_valid;  /**< Must be set to true if ss_get_cf_reason is being passed */
  cri_common_dial_ss_call_forwarding_reason_t_v01 ss_get_cf_reason;
  /**<   Get call forwarding status*/

  /* Optional */
  uint8_t ss_set_cf_service_valid;  /**< Must be set to true if ss_set_cf_service is being passed */
  cri_common_dial_ss_call_forwarding_service_t_v01 ss_set_cf_service;

  /* Optional */
  uint8_t ss_set_cf_reason_valid;  /**< Must be set to true if ss_set_cf_reason is being passed */
  cri_common_dial_ss_call_forwarding_reason_t_v01 ss_set_cf_reason;

  /* Optional */
  uint8_t ss_set_cf_number_valid;  /**< Must be set to true if ss_set_cf_number is being passed */
  char ss_set_cf_number[CRI_MAX_DIAL_STRING_LENGTH_V01 + 1];
  /**<   Set call forwarding*/

  /* Optional */
  uint8_t ss_set_cw_service_valid;  /**< Must be set to true if ss_set_cw_service is being passed */
  cri_common_dial_ss_call_waiting_service_t_v01 ss_set_cw_service;
  /**<   Set call waiting*/

  /* Optional */
  uint8_t ss_set_clir_clir_type_valid;  /**< Must be set to true if ss_set_clir_clir_type is being passed */
  cri_common_dial_ss_clir_type_t_v01 ss_set_clir_clir_type;

  /* Optional */
  uint8_t ss_set_fac_lock_code_valid;  /**< Must be set to true if ss_set_fac_lock_code is being passed */
  cri_common_dial_ss_set_facility_lock_code_t_v01 ss_set_fac_lock_code;

  /* Optional */
  uint8_t ss_set_fac_lock_status_valid;  /**< Must be set to true if ss_set_fac_lock_status is being passed */
  cri_common_dial_ss_set_facility_lock_status_t_v01 ss_set_fac_lock_status;

  /* Optional */
  uint8_t ss_set_fac_lock_password_valid;  /**< Must be set to true if ss_set_fac_lock_password is being passed */
  char ss_set_fac_lock_password[CRI_MAX_PASSWORD_LENGTH_V01 + 1];
  /**<   Set facility lock*/

  /* Optional */
  uint8_t ss_change_cb_pw_reason_valid;  /**< Must be set to true if ss_change_cb_pw_reason is being passed */
  cri_common_dial_ss_change_call_barring_password_reason_t_v01 ss_change_cb_pw_reason;

  /* Optional */
  uint8_t ss_change_cb_pw_old_password_valid;  /**< Must be set to true if ss_change_cb_pw_old_password is being passed */
  char ss_change_cb_pw_old_password[CRI_MAX_PASSWORD_LENGTH_V01 + 1];

  /* Optional */
  uint8_t ss_change_cb_pw_new_password_valid;  /**< Must be set to true if ss_change_cb_pw_new_password is being passed */
  char ss_change_cb_pw_new_password[CRI_MAX_PASSWORD_LENGTH_V01 + 1];
  /**<   Change call barring password*/

  /* Optional */
  uint8_t ussd_send_ussd_ussd_type_valid;  /**< Must be set to true if ussd_send_ussd_ussd_type is being passed */
  cri_common_dial_ussd_send_ussd_type_t_v01 ussd_send_ussd_ussd_type;

  /* Optional */
  uint8_t ussd_send_ussd_encoding_valid;  /**< Must be set to true if ussd_send_ussd_encoding is being passed */
  cri_common_dial_ussd_encoding_t_v01 ussd_send_ussd_encoding;

  /* Optional */
  uint8_t ussd_send_ussd_string_valid;  /**< Must be set to true if ussd_send_ussd_string is being passed */
  char ussd_send_ussd_string[CRI_MAX_USSD_LENGTH_V01 + 1];
  /**<   Send USSD*/
}cri_common_dial_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup cri_common_dial_qmi_messages
    @{
  */
/** Response Message; Common entry point for Voice/SS/USSD. */
typedef struct {

  /* Mandatory */
  cri_common_dial_response_t_v01 response_type;

  /* Optional */
  uint8_t ss_type_valid;  /**< Must be set to true if ss_type is being passed */
  cri_common_dial_ss_t_v01 ss_type;

  /* Optional */
  uint8_t ussd_type_valid;  /**< Must be set to true if ussd_type is being passed */
  cri_common_dial_ussd_t_v01 ussd_type;

  /* Optional */
  uint8_t ss_get_cf_status_valid;  /**< Must be set to true if ss_get_cf_status is being passed */
  cri_common_dial_ss_call_forwarding_status_t_v01 ss_get_cf_status;

  /* Optional */
  uint8_t ss_get_cf_status_info_valid;  /**< Must be set to true if ss_get_cf_status_info is being passed */
  uint32_t ss_get_cf_status_info_len;  /**< Must be set to # of elements in ss_get_cf_status_info */
  cri_common_dial_ss_call_forwarding_info_t_v01 ss_get_cf_status_info[CRI_MAX_CALL_FORWARDING_INFO_V01];
  /**<   Get call forwarding status*/

  /* Optional */
  uint8_t ss_get_cw_service_valid;  /**< Must be set to true if ss_get_cw_service is being passed */
  cri_common_dial_ss_call_waiting_service_t_v01 ss_get_cw_service;
  /**<   Get call waiting status*/

  /* Optional */
  uint8_t ss_get_clir_action_valid;  /**< Must be set to true if ss_get_clir_action is being passed */
  cri_common_dial_ss_clir_action_t_v01 ss_get_clir_action;

  /* Optional */
  uint8_t ss_get_clir_presentation_valid;  /**< Must be set to true if ss_get_clir_presentation is being passed */
  cri_common_dial_ss_clir_presentation_t_v01 ss_get_clir_presentation;
  /**<   Get CLIR*/
}cri_common_dial_resp_msg_v01;  /* Message */
/**
    @}
  */

/*Extern Definition of Type Table Object*/
/*THIS IS AN INTERNAL OBJECT AND SHOULD ONLY*/
/*BE ACCESSED BY AUTOGENERATED FILES*/
extern const qmi_idl_type_table_object cri_common_dial_qmi_idl_type_table_object_v01;


#ifdef __cplusplus
}
#endif
#endif

