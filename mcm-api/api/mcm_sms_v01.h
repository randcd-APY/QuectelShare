#ifndef MCM_SMS_SERVICE_01_H
#define MCM_SMS_SERVICE_01_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <stdint.h>
#include "mcm_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup mcm_sms_consts
    @{
  */

/**  Maximum length of an MO SMS (9*160). */
#define MCM_SMS_MAX_MO_MSG_LENGTH_V01 1440

/**  Maximum length of an SMS. */
#define MCM_SMS_MAX_MT_MSG_LENGTH_V01 160

/**  Maximum string length. */
#define MCM_SMS_MAX_ADDR_LENGTH_V01 252
/**
    @}
  */

/** @addtogroup mcm_sms_enums
    @{
  */
typedef enum {
  MCM_SMS_MSG_FORMAT_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_SMS_MSG_FORMAT_TEXT_CDMA_V01 = 0x00, /**<  Message format CDMA text. */
  MCM_SMS_MSG_FORMAT_TEXT_GW_PP_V01 = 0x06, /**<  Message format GW_PP text. */
  MCM_SMS_MSG_FORMAT_TEXT_GW_BC_V01 = 0x07, /**<  Message format GW_BC text. */
  MCM_SMS_MSG_FORMAT_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_sms_msg_format_t_v01;
/**
    @}
  */

/** @addtogroup mcm_sms_enums
    @{
  */
typedef enum {
  MCM_SMS_MSG_SIZE_VALIDATION_MODE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_SMS_MSG_SIZE_VALIDATION_MODE_AUTO_BREAK_V01 = 1, /**<  Message size validation mode;
 Auto-break into 160-byte segments. */
  MCM_SMS_MSG_SIZE_VALIDATION_MODE_NO_AUTO_BREAK_V01 = 2, /**<  Message size validation mode;
 No auto-break. */
  MCM_SMS_MSG_SIZE_VALIDATION_MODE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_sms_msg_size_validation_mode_t_v01;
/**
    @}
  */

/** @addtogroup mcm_sms_enums
    @{
  */
typedef enum {
  MCM_SMS_RECEPTION_MODE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_SMS_RECEPTION_MODE_NO_RECEPTION_V01 = 1, /**<  No reception. */
  MCM_SMS_RECEPTION_MODE_ON_AUTO_CONFIRM_TO_NW_V01 = 2, /**<  Reception on with auto confirm to network. */
  MCM_SMS_RECEPTION_MODE_ON_WITHOUT_AUTO_CONFIRM_TO_NW_V01 = 3, /**<  Reception on without auto confirm to network. */
  MCM_SMS_RECEPTION_MODE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_sms_reception_mode_t_v01;
/**
    @}
  */

/** @addtogroup mcm_sms_enums
    @{
  */
typedef enum {
  MCM_SMS_MESSAGE_CLASS_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_SMS_MESSAGE_CLASS_0_V01 = 0, /**<  Class 0. */
  MCM_SMS_MESSAGE_CLASS_1_V01 = 1, /**<  Class 1. */
  MCM_SMS_MESSAGE_CLASS_2_V01 = 2, /**<  Class 2. */
  MCM_SMS_MESSAGE_CLASS_3_V01 = 3, /**<  Class 3. */
  MCM_SMS_MESSAGE_CLASS_NONE_V01 = 4, /**<  None. */
  MCM_SMS_MESSAGE_CLASS_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_sms_message_class_t_v01;
/**
    @}
  */

/** @addtogroup mcm_sms_enums
    @{
  */
typedef enum {
  MCM_CBS_CMAE_CATEGORY_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_CBS_CMAE_CATEGORY_INVALID_V01 = -1,
  MCM_CBS_CMAE_CATEGORY_GEO_V01 = 0, /**<  Geophysical, including landslide. */
  MCM_CBS_CMAE_CATEGORY_MET_V01 = 1, /**<  Meteorological, including flood. */
  MCM_CBS_CATEGORY_SAFETY_V01 = 2, /**<  Safety (general emergency and public safety). */
  MCM_CBS_CMAE_CATEGORY_SECURITY_V01 = 3, /**<  Security (law enforcement, military, homeland,
 and local/private security). */
  MCM_CBS_CMAE_CATEGORY_RESCUE_V01 = 4, /**<  Rescue (rescue and recovery). */
  MCM_CBS_CMAE_CATEGORY_FIRE_V01 = 5, /**<  Fire (fire suppression and rescue). */
  MCM_CBS_CMAE_CATEGORY_HEALTH_V01 = 6, /**<  Health (medical and public health). */
  MCM_CBS_CMAE_CATEGORY_ENV_V01 = 7, /**<  Environment (pollution and other environmental factors). */
  MCM_CBS_CMAE_CATEGORY_TRANSPORT_V01 = 8, /**<  Transport (public and private transportation). */
  MCM_CBS_CMAE_CATEGORY_INFRA_V01 = 9, /**<  Infrastructure (utility, telecommunication, and other
 nontransport infrastructure). */
  MCM_CBS_CMAE_CATEGORY_CBRNE_V01 = 10, /**<  CBRNE (chemical, biological, radiological, nuclear,
 or high-yield explosive thread or attack). */
  MCM_CBS_CMAE_CATEGORY_OTHER_V01 = 11, /**<  Other events. */
  MCM_CBS_CMAE_CATEGORY_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_cbs_cmae_category_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_sms_enums
    @{
  */
typedef enum {
  MCM_CBS_CMAE_RESPONSE_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_CBS_CMAE_RESPONSE_TYPE_INVALID_V01 = -1,
  MCM_CBS_CMAE_RESPONSE_TYPE_SHELTER_V01 = 0, /**<  Shelter (take shelter in place). */
  MCM_CBS_CMAE_RESPONSE_TYPE_EVACUATE_V01 = 1, /**<  Evacuate (relocate). */
  MCM_CBS_CMAE_RESPONSE_TYPE_PREPARE_V01 = 2, /**<  Prepare (make preparations). */
  MCM_CBS_CMAE_RESPONSE_TYPE_EXECUTE_V01 = 3, /**<  Execute (execute a preplanned activity). */
  MCM_CBS_CMAE_RESPONSE_TYPE_MONITOR_V01 = 4, /**<  Monitor (attend to information sources). */
  MCM_CBS_CMAE_RESPONSE_TYPE_AVOID_V01 = 5, /**<  Avoid (avoid hazards). */
  MCM_CBS_CMAE_RESPONSE_TYPE_ASSESS_V01 = 6, /**<  Assess (evaluate the information in this message). */
  MCM_CBS_CMAE_RESPONSE_TYPE_NONE_V01 = 7, /**<  None (no action recommended). */
  MCM_CBS_CMAE_RESPONSE_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_cbs_cmae_response_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_sms_enums
    @{
  */
typedef enum {
  MCM_CBS_CMAE_SEVERITY_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_CBS_CMAE_SEVERITY_INVALID_V01 = -1,
  MCM_CBS_CMAE_SEVERITY_EXTREME_V01 = 0, /**<  Extreme (extraodinary threat to life or property). */
  MCM_CBS_CMAE_SEVERITY_SEVERE_V01 = 1, /**<  Severe (significant threat to life or property). */
  MCM_CBS_CMAE_SEVERITY_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_cbs_cmae_severity_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_sms_enums
    @{
  */
typedef enum {
  MCM_CBS_CMAE_URGENCY_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_CBS_CMAE_URGENCY_INVALID_V01 = -1,
  MCM_CBS_CMAE_URGENCY_IMMEDIATE_V01 = 0, /**<  Immediate (responsive action should be taken immediately).  */
  MCM_CBS_CMAE_URGENCY_EXPECTED_V01 = 1, /**<  Expected (reponsive action should be taken soon, i.e., within the next hour). */
  MCM_CBS_CMAE_URGENCY_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_cbs_cmae_urgency_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_sms_enums
    @{
  */
typedef enum {
  MCM_CBS_CMAE_CERTAINTY_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_CBS_CMAE_CERTAINTY_INVALID_V01 = -1,
  MCM_CBS_CMAE_CERTAINTY_OBSERVED_V01 = 0, /**<  Observed (determined to have occurred or to be ongoing). */
  MCM_CBS_CMAE_CERTAINTY_LIKELY_V01 = 1, /**<  Likely (probabiltiy > ~50%). */
  MCM_CBS_CMAE_CERTAINTY_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_cbs_cmae_certainty_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_sms_aggregates
    @{
  */
typedef struct {

  uint8_t year;
  /**<   Year -- Range 00 to 99 (UTC).*/

  uint8_t month;
  /**<   Month -- Range 1 to 12 (UTC).*/

  uint8_t day;
  /**<   Day -- Range 1 to 31 (UTC).*/

  uint8_t hours;
  /**<   Hour -- Range 0 to 23 (UTC).*/

  uint8_t minutes;
  /**<   Minutes -- Range 0 to 59 (UTC).*/

  uint8_t seconds;
  /**<   Seconds -- Range 0 to 59 (UTC).*/
}mcm_cbs_cmae_expire_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup mcm_sms_aggregates
    @{
  */
typedef struct {

  uint32_t message_content_len;  /**< Must be set to the number of elements in message_content. */
  char message_content[MCM_SMS_MAX_MT_MSG_LENGTH_V01];
  /**<   Message content. @newpagetable */
}mcm_cbs_cmae_record_type_0_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup mcm_sms_aggregates
    @{
  */
typedef struct {

  mcm_cbs_cmae_category_type_t_v01 category;
  /**<   Category of the CMAS alert.*/

  mcm_cbs_cmae_response_type_t_v01 response;
  /**<   Response indicated for the CMAS alert.*/

  mcm_cbs_cmae_severity_type_t_v01 severity;
  /**<   Severity of the CMAS alert.*/

  mcm_cbs_cmae_urgency_type_t_v01 urgency;
  /**<   Urgency of the CMAS alert.*/

  mcm_cbs_cmae_certainty_type_t_v01 certainty;
  /**<   Certainty of the CMAS alert.*/
}mcm_cbs_cmae_record_type_1_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup mcm_sms_aggregates
    @{
  */
typedef struct {

  uint16_t id;
  /**<   Identification of the message.*/

  uint8_t alert_handling;
  /**<   Indicates whether this alert message requires special handling.*/

  mcm_cbs_cmae_expire_t_v01 expire;
  /**<   Expiration date and time of the CMAS alert.*/

  uint8_t language;
  /**<   Language used for the message content.*/
}mcm_cbs_cmae_record_type_2_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Request message; Sets the service center configuration type. */
typedef struct {

  /* Mandatory */
  char service_center_addr[MCM_SMS_MAX_ADDR_LENGTH_V01 + 1];
  /**<   Address of the service center.*/

  /* Optional */
  uint8_t validity_time_valid;  /**< Must be set to TRUE if validity_time is being passed.. */
  int64_t validity_time;
  /**<   Validity time.*/
}mcm_sms_set_service_center_cfg_type_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Response message; Sets the service center configuration type. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**<   Result code.*/
}mcm_sms_set_service_center_cfg_type_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @cond
*/

typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}mcm_sms_get_service_center_cfg_type_req_msg_v01;

/** @endcond */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Response message; Gets the service center configuration type. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**<   Result code.*/

  /* Optional */
  uint8_t service_center_addr_valid;  /**< Must be set to TRUE if service_center_addr is being passed. */
  char service_center_addr[MCM_SMS_MAX_ADDR_LENGTH_V01 + 1];
  /**<   Address of the service center.*/

  /* Optional */
  uint8_t validity_time_valid;  /**< Must be set to TRUE if validity_time is being passed. */
  int64_t validity_time;
  /**<   Validity time.*/
}mcm_sms_get_service_center_cfg_type_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Request message; Sends an MO message. */
typedef struct {

  /* Mandatory */
  mcm_sms_msg_format_t_v01 message_format;
  /**<   Message format. */

  /* Mandatory */
  char message_content[MCM_SMS_MAX_MO_MSG_LENGTH_V01 + 1];
  /**<   Message content. */

  /* Mandatory */
  char destination[MCM_SMS_MAX_ADDR_LENGTH_V01 + 1];
  /**<   Destination. */

  /* Optional */
  uint8_t size_validation_valid;  /**< Must be set to TRUE if size_validation is being passed. */
  mcm_sms_msg_size_validation_mode_t_v01 size_validation;
  /**<   Size validation. */
}mcm_sms_send_mo_msg_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Response message; Sends an MO message. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**<   Result code.*/
}mcm_sms_send_mo_msg_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Request message; Sets the message configutation. */
typedef struct {

  /* Optional */
  uint8_t default_size_validation_mode_valid;  /**< Must be set to TRUE if default_size_validation_mode is being passed. */
  mcm_sms_msg_size_validation_mode_t_v01 default_size_validation_mode;
  /**<   Default size validation mode. */

  /* Optional */
  uint8_t enable_cb_valid;  /**< Must be set to TRUE if enable_cb is being passed. */
  uint8_t enable_cb;
  /**<   Enable callback. */
}mcm_sms_set_msg_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Response message; Sets the message configutation. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**<   Result code.*/
}mcm_sms_set_msg_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @cond
*/

typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}mcm_sms_get_msg_config_req_msg_v01;

/** @endcond */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Response message; Gets the message configuration. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**<   Result code.*/

  /* Optional */
  uint8_t default_size_validation_mode_valid;  /**< Must be set to TRUE if default_size_validation_mode is being passed. */
  mcm_sms_msg_size_validation_mode_t_v01 default_size_validation_mode;
  /**<   Default size validation mode. */

  /* Optional */
  uint8_t enable_cb_valid;  /**< Must be set to TRUE if enable_cb is being passed. */
  uint8_t enable_cb;
  /**<   Enable callback. */
}mcm_sms_get_msg_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Request message; Sets the reception mode. */
typedef struct {

  /* Mandatory */
  mcm_sms_reception_mode_t_v01 reception_mode;
  /**<   Reception mode. */

  /* Optional */
  uint8_t last_absorbed_message_id_valid;  /**< Must be set to TRUE if last_absorbed_message_id is being passed. */
  int64_t last_absorbed_message_id;
  /**<   Last absorbed message ID. */
}mcm_sms_set_reception_mode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Response message; Sets the reception mode. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**<   Result code.*/
}mcm_sms_set_reception_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Request message; Registers for an indication of events. */
typedef struct {

  /* Optional */
  uint8_t register_sms_pp_event_valid;  /**< Must be set to TRUE if register_sms_pp_event is being passed. */
  uint8_t register_sms_pp_event;
  /**<   Receive a PP SMS event.*/
}mcm_sms_event_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Response message; Registers for an indication of events. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**<   Result code.*/
}mcm_sms_event_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Indication message; Point-to-point message indication. */
typedef struct {

  /* Mandatory */
  mcm_sms_msg_format_t_v01 message_format;
  /**<   Message format. */

  /* Mandatory */
  char message_content[MCM_SMS_MAX_MT_MSG_LENGTH_V01 + 1];
  /**<   Message content. */

  /* Mandatory */
  char source_address[MCM_SMS_MAX_ADDR_LENGTH_V01 + 1];
  /**<   Source address. */

  /* Mandatory */
  int64_t message_id;
  /**<   Message ID. */

  /* Optional */
  uint8_t message_class_valid;  /**< Must be set to TRUE if message_class is being passed. */
  mcm_sms_message_class_t_v01 message_class;
  /**<   Message class. */
}mcm_sms_pp_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Indication message; Cell broadcast message indication. */
typedef struct {

  /* Mandatory */
  mcm_sms_msg_format_t_v01 message_format;
  /**<   Message format. */

  /* Mandatory */
  char message_content[MCM_SMS_MAX_MT_MSG_LENGTH_V01 + 1];
  /**<   Message content. */
}mcm_sms_cb_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_sms_messages
    @{
  */
/** Indication message; Cell broadcast CMAS message indication. */
typedef struct {

  /* Optional */
  uint8_t type_0_record_valid;  /**< Must be set to TRUE if type_0_record is being passed. */
  mcm_cbs_cmae_record_type_0_t_v01 type_0_record;
  /**<   Type 0 record. */

  /* Optional */
  uint8_t type_1_record_valid;  /**< Must be set to TRUE if type_1_record is being passed. */
  mcm_cbs_cmae_record_type_1_t_v01 type_1_record;
  /**<   Type 1 record. */

  /* Optional */
  uint8_t type_2_record_valid;  /**< Must be set to TRUE if type_2_record is being passed. */
  mcm_cbs_cmae_record_type_2_t_v01 type_2_record;
  /**<   Type 2 record. */
}mcm_sms_cb_cmas_ind_msg_v01;  /* Message */
/**
    @}
  */

#define MCM_SMS_SET_SERVICE_CENTER_CFG_TYPE_REQ_V01 0x0700
#define MCM_SMS_SET_SERVICE_CENTER_CFG_TYPE_RESP_V01 0x0700
#define MCM_SMS_GET_SERVICE_CENTER_CFG_TYPE_REQ_V01 0x0701
#define MCM_SMS_GET_SERVICE_CENTER_CFG_TYPE_RESP_V01 0x0701
#define MCM_SMS_SEND_MO_MSG_REQ_V01 0x0702
#define MCM_SMS_SEND_MO_MSG_RESP_V01 0x0702
#define MCM_SMS_SET_MSG_CONFIG_REQ_V01 0x0703
#define MCM_SMS_SET_MSG_CONFIG_RESP_V01 0x0703
#define MCM_SMS_GET_MSG_CONFIG_REQ_V01 0x0704
#define MCM_SMS_GET_MSG_CONFIG_RESP_V01 0x0704
#define MCM_SMS_SET_RECEPTION_MODE_REQ_V01 0x0705
#define MCM_SMS_SET_RECEPTION_MODE_RESP_V01 0x0705
#define MCM_SMS_EVENT_REGISTER_REQ_V01 0x0706
#define MCM_SMS_EVENT_REGISTER_RESP_V01 0x0706
#define MCM_SMS_PP_IND_V01 0x0707
#define MCM_SMS_CB_IND_V01 0x0708
#define MCM_SMS_CB_CMAS_IND_V01 0x0709

#ifdef __cplusplus
}
#endif
#endif

