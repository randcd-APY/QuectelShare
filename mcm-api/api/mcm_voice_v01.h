#ifndef MCM_VOICE_SERVICE_01_H
#define MCM_VOICE_SERVICE_01_H
/**
  @file mcm_voice_v01.h

  @brief This is the public header file which defines the mcm_voice service data structures.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include <stdint.h>
#include "mcm_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup mcm_voice_consts
    @{
  */

/** GSM provides up to 8 calls; 3GPP2 provides 2. */
#define MCM_MAX_VOICE_CALLS_V01 8

/** Maximum length for a phone number or SIP URI (81 + NULL). */
#define MCM_MAX_PHONE_NUMBER_V01 82

/**  Maximum user-to-user data. */
#define MCM_MAX_UUS_DATA_V01 20

/**  Maximum DTMF length. */
#define MCM_MAX_DTMF_LENGTH_V01 20

/**  Maximum USSD length. */
#define MCM_MAX_USSD_LENGTH_V01 128

/**  Maximum password length. */
#define MCM_MAX_PASSWORD_LENGTH_V01 4

/**  Maximum call forwarding information. */
#define MCM_MAX_CALL_FORWARDING_INFO_V01 13

/**  Maximum size of the MSD sent to the network with an eCall */
#define MCM_MAX_ECALL_MSD_V01 140

/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CALL_STATE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CALL_STATE_INCOMING_V01 = 0x0000, /**< MT incoming; CC setup. */
  MCM_VOICE_CALL_STATE_DIALING_V01 = 0x0001, /**< Dialing state. */
  MCM_VOICE_CALL_STATE_ALERTING_V01 = 0x0002, /**< MT call waiting; MO alterting. */
  MCM_VOICE_CALL_STATE_ACTIVE_V01 = 0x0003, /**< Call is active. */
  MCM_VOICE_CALL_STATE_HOLDING_V01 = 0x0004, /**< Call is on hold. */
  MCM_VOICE_CALL_STATE_END_V01 = 0x0005, /**<  Call is disconnected. */
  MCM_VOICE_CALL_STATE_WAITING_V01 = 0x0006, /**< Call is waiting. */
  MCM_VOICE_CALL_STATE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_call_state_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CALL_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CALL_TYPE_NOT_SPECIFIED_V01 = 0x0000, /**< Placeholder for a zero value. */
  MCM_VOICE_CALL_TYPE_VOICE_V01 = 0x0001, /**< Voice call. */
  MCM_VOICE_CALL_TYPE_EMERGENCY_V01 = 0x0002, /**< Emergency call. */
  MCM_VOICE_CALL_TYPE_ECALL_AUTO_V01 = 0x0003, /**< Automatically triggered eCall */
  MCM_VOICE_CALL_TYPE_ECALL_MANUAL_V01 = 0x0004, /**< Manually triggered eCall */
  MCM_VOICE_CALL_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_call_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CALL_DIRECTION_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CALL_MOBILE_ORIGINATED_V01 = 0x0000, /**< Mobile-originated. */
  MCM_VOICE_CALL_MOBILE_TERMINATED_V01 = 0x0001, /**< Mobile-terminated. */
  MCM_VOICE_CALL_DIRECTION_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_call_direction_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CALL_NUMBER_PRESENTATION_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CALL_NUMBER_UNKNOWN_V01 = 0x0000,
  MCM_VOICE_CALL_NUMBER_ALLOWED_V01 = 0x0001, /**< Number allowed. */
  MCM_VOICE_CALL_NUMBER_RESTRICTED_V01 = 0x0002, /**< Number restricted. */
  MCM_VOICE_CALL_NUMBER_PAYPHONE_V01 = 0x0003, /**< Payhone number. */
  MCM_VOICE_CALL_NUMBER_PRESENTATION_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_call_number_presentation_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_REASON_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_REASON_NONE_V01 = 1, /**< Placeholder for a zero value. */
  MCM_VOICE_REASON_NORMAL_V01 = 2, /**< Call ended normally. */
  MCM_VOICE_REASON_BUSY_V01 = 3, /**< Call was rejected (busy). */
  MCM_VOICE_REASON_CONGESTION_V01 = 4, /**< Network congestion. */
  MCM_VOICE_REASON_CALL_BARRED_V01 = 5, /**< Incoming calls barred. */
  MCM_VOICE_REASON_FDN_BLOCKED_V01 = 6, /**< Blocked by fixed dialing. */
  MCM_VOICE_REASON_DIAL_MODIFIED_TO_USSD_V01 = 7, /**< Converted to a USSD message. */
  MCM_VOICE_REASON_DIAL_MODIFIED_TO_SS_V01 = 8, /**< Converted to a SUP. */
  MCM_VOICE_REASON_DIAL_MODIFIED_TO_DIAL_V01 = 9, /**< Converted to another call type. */
  MCM_VOICE_REASON_ACM_LIMIT_EXCEEDED_V01 = 10, /**< No funds. */
  MCM_VOICE_REASON_NETWORK_GENERIC_ERROR_V01 = 11,
  MCM_VOICE_REASON_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_reason_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CALL_OPERATION_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CALL_ANSWER_V01 = 0, /**< Answer the call. */
  MCM_VOICE_CALL_END_V01 = 1, /**< End the call. */
  MCM_VOICE_CALL_HOLD_V01 = 2, /**< Hold the call. */
  MCM_VOICE_CALL_UNHOLD_V01 = 3, /**< Release the call from hold. */
  MCM_VOICE_CALL_CONFERENCE_V01 = 4, /**< Conference call. */
  MCM_VOICE_CALL_GO_PRIVATE_V01 = 5, /**< Private call. */
  MCM_VOICE_CALL_END_ALL_V01 = 6, /**< End all calls. */
  MCM_VOICE_CALL_OPERATION_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_call_operation_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_UUS_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_UUS_TYPE1_IMPLICIT_V01 = 0, /**< Type 1 implicit. */
  MCM_VOICE_UUS_TYPE1_REQUIRED_V01 = 1, /**< Type 1 required. */
  MCM_VOICE_UUS_TYPE1_NOT_REQUIRED_V01 = 2, /**< Type 1 not required. */
  MCM_VOICE_UUS_TYPE2_REQUIRED_V01 = 3, /**< Type 2 required. */
  MCM_VOICE_UUS_TYPE2_NOT_REQUIRED_V01 = 4, /**< Type 2 not required. */
  MCM_VOICE_UUS_TYPE3_REQUIRED_V01 = 5, /**< Type 3 required. */
  MCM_VOICE_UUS_TYPE3_NOT_REQUIRED_V01 = 6, /**< Type 3 not required. */
  MCM_VOICE_UUS_TYPE_DATA_V01 = 7, /**< Data. */
  MCM_VOICE_UUS_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_uus_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_UUS_DCS_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_UUS_DCS_IA5_V01 = 0, /**< IA5. */
  MCM_VOICE_UUS_DCS_OHLP_V01 = 1, /**< OHLP. */
  MCM_VOICE_UUS_DCS_USP_V01 = 2, /**< USP. */
  MCM_VOICE_UUS_DCS_X244_V01 = 3, /**< x244. */
  MCM_VOICE_UUS_DCS_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_uus_dcs_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_TECH_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_TECH_NONE_V01 = 0, /**< None. */
  MCM_VOICE_TECH_3GPP_V01 = 1, /**< 3GPP. */
  MCM_VOICE_TECH_3GPP2_V01 = 2, /**< 3GPP2. */
  MCM_VOICE_TECH_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_tech_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CALL_FORWARDING_STATUS_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CALL_FORWARDING_DISABLED_V01 = 0, /**< Disabled. */
  MCM_VOICE_CALL_FORWARDING_ENABLED_V01 = 1, /**< Enabled. */
  MCM_VOICE_CALL_FORWARDING_STATUS_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_call_forwarding_status_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CALL_FORWARDING_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CALL_FORWARDING_TYPE_VOICE_V01 = 0, /**< Voice. */
  MCM_VOICE_CALL_FORWARDING_TYPE_DATA_V01 = 1, /**< Data. */
  MCM_VOICE_CALL_FORWARDING_TYPE_VOICE_DATA_V01 = 2, /**< Voice and data. */
  MCM_VOICE_CALL_FORWARDING_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_call_forwarding_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CALL_WAITING_SERVICE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CALL_WAITING_VOICE_ENABLED_V01 = 0, /**< Voice call waiting enabled. */
  MCM_VOICE_CALL_WAITING_DATA_ENABLED_V01 = 1, /**< Data call waiting enabled. */
  MCM_VOICE_CALL_WAITING_VOICE_DATA_ENABLED_V01 = 2, /**< Voice and data call waiting enabled. */
  MCM_VOICE_CALL_WAITING_DISABLED_V01 = 3, /**< Voice call waiting disabled. */
  MCM_VOICE_CALL_WAITING_SERVICE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_call_waiting_service_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CALL_SERVICE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_SERVICE_ACTIVATE_V01 = 0, /**< Activate. */
  MCM_VOICE_SERVICE_DEACTIVATE_V01 = 1, /**< Deactivate. */
  MCM_VOICE_SERVICE_REGISTER_V01 = 2, /**< Register. */
  MCM_VOICE_SERVICE_ERASE_V01 = 3, /**< Erase. */
  MCM_VOICE_CALL_SERVICE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_call_service_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CALL_FORWARDING_REASON_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CALL_FORWARD_UNCONDITIONALLY_V01 = 0, /**< Unconditional call forwarding. */
  MCM_VOICE_CALL_FORWARD_MOBILEBUSY_V01 = 1, /**< Forward when the mobile device is busy. */
  MCM_VOICE_CALL_FORWARD_NOREPLY_V01 = 2, /**< Forward when there is no reply. */
  MCM_VOICE_CALL_FORWARD_UNREACHABLE_V01 = 3, /**< Forward when the call is unreachable. */
  MCM_VOICE_CALL_FORWARD_ALLFORWARDING_V01 = 4, /**< All forwarding. */
  MCM_VOICE_CALL_FORWARD_ALLCONDITIONAL_V01 = 5, /**< All conditional forwarding. */
  MCM_VOICE_CALL_FORWARDING_REASON_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_call_forwarding_reason_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CLIR_ACTION_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CLIR_INVOCATION_V01 = 0, /**< Invocation. */
  MCM_VOICE_CLIR_SUPPRESSION_V01 = 1, /**< Suppression. */
  MCM_VOICE_CLIR_ACTION_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_clir_action_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CLIR_PRESENTATION_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CLIR_NOT_PROVISIONED_V01 = 0, /**< Not provisioned. */
  MCM_VOICE_CLIR_PROVISIONED_PERMANENT_MODE_V01 = 1, /**< Permanently provisioned. */
  MCM_VOICE_CLIR_PRESENTATION_RESTRICTED_V01 = 2, /**< Restricted presentation. */
  MCM_VOICE_CLIR_PRESENTATION_ALLOWED_V01 = 3, /**< Allowed presentation. */
  MCM_VOICE_CLIR_PRESENTATION_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_clir_presentation_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_FACILITY_LOCK_STATUS_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_FACILITY_LOCK_ENABLE_V01 = 0, /**< Enable. */
  MCM_VOICE_FACILITY_LOCK_DISABLE_V01 = 1, /**< Disable. */
  MCM_VOICE_FACILITY_LOCK_STATUS_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_facility_lock_status_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_FACILITY_CODE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_FACILITY_CODE_AO_V01 = 0, /**< BAOC (Bar All Outgoing Calls) (refer to 3GPP TS 22.088 @xhyperref{S9,[S9]} clause 1). */
  MCM_VOICE_FACILITY_CODE_OI_V01 = 1, /**< BOIC (Bar Outgoing International Calls) (refer to 3GPP TS 22.088 @xhyperref{S9,[S9]}
 clause 1). */
  MCM_VOICE_FACILITY_CODE_OX_V01 = 2, /**< BOIC-exHC (Bar Outgoing International Calls except to Home Country)
 (refer to 3GPP TS 22.088 @xhyperref{S9,[S9]} clause 1). */
  MCM_VOICE_FACILITY_CODE_AI_V01 = 3, /**< BAIC (Bar All Incoming Calls) (refer 3GPP TS 22.088 @xhyperref{S9,[S9]} clause 2). */
  MCM_VOICE_FACILITY_CODE_IR_V01 = 4, /**< BIC-Roam (Bar Incoming Calls when Roaming outside the home country)
 (refer to 3GPP TS 22.088 @xhyperref{S9,[S9]} clause 2). */
  MCM_VOICE_FACILITY_CODE_AB_V01 = 5, /**< All barring services (refer to 3GPP TS 22.030 @xhyperref{S10,[S10]}) (applicable only
 for mode=0). */
  MCM_VOICE_FACILITY_CODE_AG_V01 = 6, /**< All outgoing barring services (refer to 3GPP TS 22.030 @xhyperref{S10,[S10]})
 (applicable only for mode=0). */
  MCM_VOICE_FACILITY_CODE_AC_V01 = 7, /**< All incoming barring services (refer to 3GPP TS 22.030 @xhyperref{S10,[S10]})
 (applicable only for mode=0). */
  MCM_VOICE_FACILITY_CODE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_facility_code_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOING_V01 = 0, /**< All outgoing. */
  MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINT_V01 = 1, /**< Outgoing internal. */
  MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_OUTGOINGINTEXTOHOME_V01 = 2, /**< Outgoing external to home. */
  MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMING_V01 = 3, /**< All incoming. */
  MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_INCOMINGROAMING_V01 = 4, /**< Roaming incoming. */
  MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLBARRING_V01 = 5, /**< All calls are barred. */
  MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLOUTGOINGBARRING_V01 = 6, /**< All outgoing calls are barred. */
  MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_ALLINCOMINGBARRING_V01 = 7, /**< All incoming calls are barred. */
  MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REASON_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_change_call_barring_password_reason_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_USSD_ENCODING_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_USSD_ENCODING_ASCII_V01 = 0, /**< ASCII coding scheme. */
  MCM_VOICE_USSD_ENCODING_8BIT_V01 = 1, /**< 8-bit coding scheme. */
  MCM_VOICE_USSD_ENCODING_UCS2_V01 = 2, /**< UCS2. */
  MCM_VOICE_USSD_ENCODING_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_ussd_encoding_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_COMMON_DIAL_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_COMMON_DIAL_VOICE_V01 = 0, /**< Voice. */
  MCM_VOICE_COMMON_DIAL_SS_V01 = 1, /**< Supplementary service. */
  MCM_VOICE_COMMON_DIAL_USSD_V01 = 2, /**< Unstructured supplementary service. */
  MCM_VOICE_COMMON_DIAL_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_common_dial_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_MUTE_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_MUTE_V01 = 0, /**< Mute. */
  MCM_VOICE_UNMUTE_V01 = 1, /**< Unmute. */
  MCM_VOICE_MUTE_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_mute_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_AUTO_ANSWER_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_AUTO_ANSWER_ENABLE_V01 = 0, /**< Enable auto-answer. */
  MCM_VOICE_AUTO_ANSWER_DISABLE_V01 = 1, /**< Disable auto-answer. */
  MCM_VOICE_AUTO_ANSWER_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_auto_answer_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_DTMF_EVENT_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /* To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_DTMF_EVENT_BURST_V01 = 0, /**< Burst DTMF. */
  MCM_VOICE_DTMF_EVENT_START_CONT_V01 = 1, /**< Continuous DTMF start. */
  MCM_VOICE_DTMF_EVENT_STOP_CONT_V01 = 2, /**< Continuous DTMF stop. */
  MCM_VOICE_DTMF_EVENT_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /* To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_dtmf_event_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_USSD_MSG_TYPE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_USSD_MSG_TYPE_NEW_MESSAGE_V01 = 0, /**<  Initiate a new USSD sesion with network. */
  MCM_VOICE_USSD_MSG_TYPE_REPLY_TO_IND_V01 = 1, /**<  Reply to a USSD indication from the network. */
  MCM_VOICE_USSD_MSG_TYPE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_ussd_msg_type_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_USSD_IND_NOTIFICATION_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_USSD_INDICATION_FURTHER_ACTION_NOT_REQUIRED_V01 = 0, /**<  USSD indication requires a USSD reply. */
  MCM_VOICE_USSD_INDICATION_FURTHER_ACTION_REQUIRED_V01 = 1, /**<  USSD indication does not require a reply. */
  MCM_VOICE_USSD_IND_NOTIFICATION_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_ussd_ind_notification_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_enums
    @{
  */
typedef enum {
  MCM_VOICE_EMERGENCY_CAT_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_EMER_CAT_POLICE_V01 = 0, /**<  Police */
  MCM_VOICE_EMER_CAT_AMBULANCE_V01 = 1, /**<  Ambulance */
  MCM_VOICE_EMER_CAT_FIRE_BRIGADE_V01 = 2, /**<  Fire brigade */
  MCM_VOICE_EMER_CAT_MARINE_GUARD_V01 = 3, /**<  Marine guard */
  MCM_VOICE_EMER_CAT_MOUNTAIN_RESCUE_V01 = 4, /**<  Mountain rescue */
  MCM_VOICE_EMERGENCY_CAT_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_emergency_cat_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_qmi_enums
    @{
  */
typedef enum {
  MCM_VOICE_E911_STATE_T_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MCM_VOICE_E911_INACTIVE_V01 = 0, /**<  E911 INACTIVE. */
  MCM_VOICE_E911_ACTIVE_V01 = 1, /**<  E911 ACTIVE. */
  MCM_VOICE_E911_STATE_T_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}mcm_voice_e911_state_t_v01;
/**
    @}
  */

/** @addtogroup mcm_voice_aggregates
    @{
  */
typedef struct {

  mcm_voice_uus_type_t_v01 type;
  /**< UUS type; range -- 0 to 6.*/

  mcm_voice_uus_dcs_type_t_v01 dcs;
  /**< UUS data coding scheme; range -- 0 to 4.*/

  uint32_t uus_data_len;  /**< Must be set to the number of elements in uus_data. */
  uint8_t uus_data[MCM_MAX_UUS_DATA_V01];
  /**< Voice call UUS data.*/
}mcm_voice_uusdata_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup mcm_voice_aggregates
    @{
  */
typedef struct {

  uint32_t call_id;
  /**< Call ID associated with this call.*/

  mcm_voice_call_state_t_v01 state;
  /**< Current call state (mcm_voice_call_state).*/

  mcm_voice_tech_t_v01 tech;
  /**< Technology (mcm_tech).*/

  char number[MCM_MAX_PHONE_NUMBER_V01 + 1];
  /**< Phone number.*/

  mcm_voice_call_number_presentation_type_t_v01 number_presentation;
  /**< Number presentation.*/

  mcm_voice_call_direction_type_t_v01 direction;
  /**< Voice call direction.*/

  uint8_t uusdata_valid;
  /**< Indicates whether UUS data is valid.*/

  mcm_voice_uusdata_t_v01 uusdata;
  /**< User-to-user signaling data.*/
}mcm_voice_call_record_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup mcm_voice_aggregates
    @{
  */
typedef struct {

  uint32_t call_id;
  /**< Call ID associated with this DTMF event.*/

  mcm_voice_dtmf_event_type_t_v01 dtmf_event;
  /**< DTMF event type.*/

  uint32_t digit_len;  /**< Must be set to the number of elements in digit. */
  char digit[MCM_MAX_DTMF_LENGTH_V01];
  /**< DTMF character.*/
}mcm_voice_dtmf_info_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup mcm_voice_aggregates
    @{
  */
typedef struct {

  mcm_voice_call_forwarding_type_t_v01 type;
  /**< Call forwarding type.*/

  char number[MCM_MAX_PHONE_NUMBER_V01 + 1];
  /**< Call forwarding number.*/
}mcm_voice_call_forwarding_info_t_v01;  /* Type */
/**
    @}
  */

/** @cond
*/

typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}mcm_voice_get_calls_req_msg_v01;

/** @endcond */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Gets the list of current calls. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */

  /* Optional */
  uint8_t calls_valid;  /**< Must be set to TRUE if calls is being passed. */
  uint32_t calls_len;  /**< Must be set to the number of elements in calls. */
  mcm_voice_call_record_t_v01 calls[MCM_MAX_VOICE_CALLS_V01];
  /**< Calls. */
}mcm_voice_get_calls_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Dials a call to a specified address and returns a connection ID. */
typedef struct {

  /* Optional */
  uint8_t address_valid;  /**< Must be set to TRUE if address is being passed. */
  char address[MCM_MAX_PHONE_NUMBER_V01 + 1];
  /**< End point address of the connection to make.*/

  /* Optional */
  uint8_t call_type_valid;  /**< Must be set to TRUE if call_type is being passed. */
  mcm_voice_call_type_t_v01 call_type;
  /**< Connection (call) details, or NULL.*/

  /* Optional */
  uint8_t uusdata_valid;  /**< Must be set to TRUE if uusdata is being passed. */
  mcm_voice_uusdata_t_v01 uusdata;
  /**< Token ID used to track this command; NULL is OK.*/

  /* Optional */
  uint8_t emergency_cat_valid;  /**< Must be set to true if emergency_cat is being passed */
  mcm_voice_emergency_cat_t_v01 emergency_cat;
  /**<   Emergency call category.*/

 /* Optional */
 uint8_t ecall_msd_valid;  /**< Must be set to true if ecall_msd is being passed */
 uint32_t ecall_msd_len;  /**< Must be set to # of elements in ecall_msd */
 uint8_t ecall_msd[MCM_MAX_ECALL_MSD_V01];
 /**<  Minimum Set of Data. Only honored when
 call_type is MCM_VOICE_CALL_TYPE_ECALL_AUTO or
 MCM_VOICE_CALL_TYPE_ECALL_MANUAL. Ignored otherwise.*/
}mcm_voice_dial_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Dials a call to a specified address and returns a connection ID. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */

  /* Optional */
  uint8_t call_id_valid;  /**< Must be set to TRUE if call_id is being passed. */
  uint32_t call_id;  /**< Call ID. */
}mcm_voice_dial_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Gets the status associated with the connection ID. */
typedef struct {

  /* Mandatory */
  uint32_t call_id;
  /**< Call ID of the connection to query.*/
}mcm_voice_get_call_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Gets the status associated with the connection ID. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */

  /* Optional */
  uint8_t status_valid;  /**< Must be set to TRUE if status is being passed. */
  mcm_voice_call_record_t_v01 status;
  /**< Call status. */
}mcm_voice_get_call_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Sends a DTMF character over the connection ID. */
typedef struct {

  /* Mandatory */
  char dtmf[MCM_MAX_DTMF_LENGTH_V01 + 1];
  /**< DTMF character to be sent.
                           Valid DTMF characters are 0-9, A-D, '*', '#'.*/
}mcm_voice_dtmf_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Sends a DTMF character over the connection ID. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. @newpagetable */
}mcm_voice_dtmf_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Starts sending a DTMF character over the call ID. */
typedef struct {

  /* Mandatory */
  uint32_t call_id;  /**< Call ID. */

  /* Mandatory */
  char digit;
  /**< DTMF character to be sent.
                           Valid DTMF characters are 0-9, A-D, '*', '#'.*/
}mcm_voice_start_dtmf_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Starts sending a DTMF character over the call ID. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */

  /* Optional */
  uint8_t call_id_valid;  /**< Must be set to TRUE if call_id is being passed. */
  uint32_t call_id;  /**< Call ID. */
}mcm_voice_start_dtmf_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Stops sending a DTMF character over the call ID. */
typedef struct {

  /* Mandatory */
  uint32_t call_id;  /**< Call ID. */
}mcm_voice_stop_dtmf_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Stops sending a DTMF character over the call ID. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */

  /* Optional */
  uint8_t call_id_valid;  /**< Must be set to TRUE if call_id is being passed */
  uint32_t call_id;  /**< Call ID. @newpagetable */
}mcm_voice_stop_dtmf_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Mutes/unmutes a voice call. */
typedef struct {

  /* Mandatory */
  uint32_t call_id;
  /**< Call ID of the connection to mute/unmute.*/

  /* Mandatory */
  mcm_voice_mute_type_t_v01 mute_type;
  /**< Mute or unmute the voice call.*/
}mcm_voice_mute_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Mutes/unmutes a voice call. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */
}mcm_voice_mute_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Sends a flash sequence character over the connection call ID. */
typedef struct {

  /* Mandatory */
  char sflash_string[MCM_MAX_PHONE_NUMBER_V01 + 1];
  /**< A NULL-terminated flash string to be sent;
 Maximum 82 characters.*/
}mcm_voice_flash_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Sends a flash sequence character over the connection call ID. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */
}mcm_voice_flash_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Hangs up or disconnects a voice call connection with the specified call ID. */
typedef struct {

  /* Mandatory */
  uint32_t call_id;
  /**< Call ID associated with the connection.*/
}mcm_voice_hangup_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Hangs up or disconnects a voice call connection with the specified call ID. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */
}mcm_voice_hangup_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Provides various operations for a voice call. */
typedef struct {

  /* Mandatory */
  mcm_voice_call_operation_t_v01 call_operation;  /**< Call operation. */

  /* Optional */
  uint8_t call_id_valid;  /**< Must be set to TRUE if call_id is being passed. */
  uint32_t call_id;  /**< Call ID. */

  /* Optional */
  uint8_t cause_valid;  /**< Must be set to TRUE if cause is being passed. */
  uint32_t cause;  /**< Cause. */
}mcm_voice_command_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Provides various operations for a voice call. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */
}mcm_voice_command_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Enables/disables an incoming voice call. */
typedef struct {

  /* Mandatory */
  mcm_voice_auto_answer_type_t_v01 auto_answer_type;  /**< Auto-answer type. */

  /* Optional */
  uint8_t anto_answer_timer_valid;  /**< Must be set to TRUE if anto_answer_timer is being passed */
  uint32_t anto_answer_timer;  /**< Auto-answer timer. */
}mcm_voice_auto_answer_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Enables/disables an incoming voice call. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */
}mcm_voice_auto_answer_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Registers for an indication of events. */
typedef struct {

  /* Optional */
  uint8_t register_voice_call_event_valid;  /**< Must be set to TRUE if register_voice_call_event is being passed. */
  uint8_t register_voice_call_event;  /**< Register for a voice call event indication. */
 
  /* Optional */
  uint8_t register_mute_event_valid;  /**< Must be set to TRUE if register_mute_event is being passed. */
  uint8_t register_mute_event;  /**< Register for a mute event indication. */

  /* Optional */
  uint8_t register_dtmf_event_valid;  /**< Must be set to TRUE if register_dtmf_event is being passed */
  uint8_t register_dtmf_event;  /**< Register for a DTMF event indication. */

  /* Optional */
  uint8_t register_e911_state_event_valid;  /**< Must be set to true if register_e911_state_event is being passed */
  uint8_t register_e911_state_event;
  /**<   MCM_VOICE_E911_STATE_INDICATION*/
}mcm_voice_event_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Registers for an indication of events. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;  /**< Result code. */
}mcm_voice_event_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Indication message; Indication for MCM_VOICE_CONNECTION_EV. */
typedef struct {

  /* Mandatory */
  uint32_t calls_len;  /**< Must be set to the number of elements in calls. */
  mcm_voice_call_record_t_v01 calls[MCM_MAX_VOICE_CALLS_V01];  /**< Calls. */
}mcm_voice_call_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Indication message; Indication for MCM_VOICE_MUTE_EV. */
typedef struct {

  /* Mandatory */
  uint8_t is_mute;  /**< Indicates whether a call is muted. */
}mcm_voice_mute_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Indication message; Indication for DTMF. */
typedef struct {

  /* Mandatory */
  mcm_voice_dtmf_info_t_v01 dtmf_info;  /**< DTMF information. */
}mcm_voice_dtmf_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Call forwarding status query. */
typedef struct {

  /* Mandatory */
  mcm_voice_call_forwarding_reason_t_v01 reason;  /**< Call forwarding reason. */
}mcm_voice_get_call_forwarding_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Call forwarding status query. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**< Result code.*/

  /* Mandatory */
  mcm_voice_call_forwarding_status_t_v01 status;
  /**< Call forwarding status.*/

  /* Optional */
  uint8_t info_valid;  /**< Must be set to TRUE if info is being passed. */
  uint32_t info_len;  /**< Must be set to the number of elements in info. */
  mcm_voice_call_forwarding_info_t_v01 info[MCM_MAX_CALL_FORWARDING_INFO_V01];
  /**< Call forwarding information.*/
}mcm_voice_get_call_forwarding_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Sets call forwarding. */
typedef struct {

  /* Mandatory */
  mcm_voice_call_service_t_v01 fwdservice;
  /**< Call forwarding service.*/

  /* Mandatory */
  mcm_voice_call_forwarding_reason_t_v01 reason;
  /**< Call forwarding reason.*/

  /* Optional */
  uint8_t forwarding_number_valid;  /**< Must be set to TRUE if forwarding_number is being passed. */
  char forwarding_number[MCM_MAX_PHONE_NUMBER_V01 + 1];
  /**< Call forwarding number.*/
}mcm_voice_set_call_forwarding_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Sets call forwarding. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**< Result code. @newpagetable */
}mcm_voice_set_call_forwarding_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @cond
*/

typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}mcm_voice_get_call_waiting_status_req_msg_v01;

/** @endcond */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Call waiting status query. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**< Result code.*/

  /* Mandatory */
  mcm_voice_call_waiting_service_t_v01 status;
  /**< Call waiting status.*/
}mcm_voice_get_call_waiting_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Sets call waiting. */
typedef struct {

  /* Mandatory */
  mcm_voice_call_waiting_service_t_v01 cwservice;
  /**< Call waiting service.*/
}mcm_voice_set_call_waiting_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Sets call waiting. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**< Result code.*/
}mcm_voice_set_call_waiting_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @cond
*/

typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}mcm_voice_get_clir_req_msg_v01;

/** @endcond */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; CLIR status query. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**< Result code.*/

  /* Mandatory */
  mcm_voice_clir_action_t_v01 action;
  /**< CLIR action. */

  /* Mandatory */
  mcm_voice_clir_presentation_t_v01 presentation;
  /**<   CLIR presentation.*/
}mcm_voice_get_clir_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request Message; Set CLIR. */
typedef struct {

  /* Mandatory */
  mcm_voice_clir_action_t_v01 clir_action;
}mcm_voice_set_clir_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response Message; Set CLIR. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
}mcm_voice_set_clir_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Sets a facility lock. */
typedef struct {

  /* Mandatory */
  mcm_voice_facility_code_t_v01 code;
  /**< Facility code. Refer to 3GPP TS 27.007 @xhyperref{S8,[S8]}, Section 7.4. */

  /* Mandatory */
  mcm_voice_facility_lock_status_t_v01 status;
  /**< Facility lock status.*/

  /* Mandatory */
  char password[MCM_MAX_PASSWORD_LENGTH_V01 + 1];
  /**< Facility lock password.*/
}mcm_voice_set_facility_lock_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Sets a facility lock. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**< Result code.*/
}mcm_voice_set_facility_lock_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Changes the call barring password. */
typedef struct {

  /* Mandatory */
  mcm_voice_change_call_barring_password_reason_t_v01 reason;
  /**< Reason for the password change. Refer to 3GPP TS 27.007 @xhyperref{S8,[S8]}, Section 7.4. */

  /* Mandatory */
  char old_password[MCM_MAX_PASSWORD_LENGTH_V01 + 1];
  /**< Old password.*/

  /* Mandatory */
  char new_password[MCM_MAX_PASSWORD_LENGTH_V01 + 1];
  /**< New password.*/
}mcm_voice_change_call_barring_password_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Changes the call barring password. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**< Result code.*/
}mcm_voice_change_call_barring_password_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Sends Unstructured Supplementary Service Data (USSD). */
typedef struct {

  /* Mandatory */
  mcm_voice_ussd_msg_type_t_v01 type;

  /* Mandatory */
  mcm_voice_ussd_encoding_t_v01 encoding;
  /**< USSD encoding.*/

  /* Mandatory */
  char ussd_string[MCM_MAX_USSD_LENGTH_V01 + 1];
  /**< USSD string.*/
}mcm_voice_send_ussd_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Sends Unstructured Supplementary Service Data (USSD). */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**< Result code.*/
}mcm_voice_send_ussd_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @cond
*/

typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}mcm_voice_cancel_ussd_req_msg_v01;

/** @endcond */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Cancels USSD. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**< Result code. @newpagetable */
}mcm_voice_cancel_ussd_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Indication message; Receives a USSD indication. */
typedef struct {

  /* Mandatory */
  mcm_voice_ussd_ind_notification_t_v01 notification;

  /* Mandatory */
  char ussd[MCM_MAX_USSD_LENGTH_V01 + 1];
  /**< USSD indication message.*/
}mcm_voice_receive_ussd_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Voice/SS/USSD common dial API. */
typedef struct {

  /* Mandatory */
  char request[MCM_MAX_PHONE_NUMBER_V01 + 1];  /**< Request. */

  /* Optional */
  uint8_t call_type_valid;  /**< Must be set to TRUE if call_type is being passed. */
  mcm_voice_call_type_t_v01 call_type;
  /**< Connection (call) details, or NULL,*/

  /* Optional */
  uint8_t uusdata_valid;  /**< Must be set to TRUE if uusdata is being passed. */
  mcm_voice_uusdata_t_v01 uusdata;
  /**< Token ID used to track this command; NULL is OK.*/

  /* Optional */
  uint8_t emergency_cat_valid;  /**< Must be set to true if emergency_cat is being passed */
  mcm_voice_emergency_cat_t_v01 emergency_cat;
  /**<   Emergency call category.*/

  /* Optional */
  uint8_t ecall_msd_valid;  /**< Must be set to true if ecall_msd is being passed */
  uint32_t ecall_msd_len;  /**< Must be set to # of elements in ecall_msd */
  uint8_t ecall_msd[MCM_MAX_ECALL_MSD_V01];
  /**<  Minimum Set of Data. Only honored when
 call_type is MCM_VOICE_CALL_TYPE_ECALL_AUTO or
 MCM_VOICE_CALL_TYPE_ECALL_MANUAL. Ignored otherwise.*/
}mcm_voice_common_dial_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Voice/SS/USSD common dial API. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**< Result code.*/

  /* Optional */
  uint8_t req_changed_to_type_valid;  /**< Must be set to TRUE if req_changed_to_type is being passed. */
  mcm_voice_common_dial_type_t_v01 req_changed_to_type;
  /**< If SS, check optional SS fields. If not present, the voice call went through.*/

  /* Optional */
  uint8_t call_id_valid;  /**< Must be set to TRUE if call_id is being passed. */
  uint32_t call_id;
  /**< Call ID.*/

  /* Optional */
  uint8_t ss_get_cf_status_valid;  /**< Must be set to TRUE if ss_get_cf_status is being passed. */
  mcm_voice_call_forwarding_status_t_v01 ss_get_cf_status;
  /**< Get the call forwarding status.*/

  /* Optional */
  uint8_t ss_get_cf_info_valid;  /**< Must be set to TRUE if ss_get_cf_info is being passed. */
  uint32_t ss_get_cf_info_len;  /**< Must be set to the number of elements in ss_get_cf_info. */
  mcm_voice_call_forwarding_info_t_v01 ss_get_cf_info[MCM_MAX_CALL_FORWARDING_INFO_V01];
  /**< Call forwarding information.*/

  /* Optional */
  uint8_t ss_get_cw_status_valid;  /**< Must be set to TRUE if ss_get_cw_status is being passed. */
  mcm_voice_call_waiting_service_t_v01 ss_get_cw_status;
  /**< Call waiting status.*/

  /* Optional */
  uint8_t ss_get_clir_action_valid;  /**< Must be set to TRUE if ss_get_clir_action is being passed. */
  mcm_voice_clir_action_t_v01 ss_get_clir_action;
  /**< CLIR action.*/

  /* Optional */
  uint8_t ss_get_clir_presentation_valid;  /**< Must be set to TRUE if ss_get_clir_presentation is being passed. */
  mcm_voice_clir_presentation_t_v01 ss_get_clir_presentation;
  /**< CLIR presentation.*/
}mcm_voice_common_dial_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_messages
    @{
  */
/** Request message; Update the Minimum Set of Data (MSD) for an ongoing or subsequent eCall call. */
typedef struct {

  /* Optional */
  uint8_t ecall_msd_valid;  /**< Must be set to true if ecall_msd is being passed */
  uint32_t ecall_msd_len;  /**< Must be set to the number of elements in ecall_msd */
  uint8_t ecall_msd[MCM_MAX_ECALL_MSD_V01];
  /**<  Minimum Set of Data. in ASN.1 PER unaligned format.
 Only honored when enable_msd is set to TRUE.*/
}mcm_voice_update_msd_req_msg_v01;  /* Message */
/**
    @}
  */
/** @addtogroup mcm_voice_messages
    @{
  */
/** Response message; Update the Minimum Set of Data (MSD) for an ongoing or subsequent eCall call. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**<  Result code.*/
}mcm_voice_update_msd_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_qmi_messages
    @{
  */
/** Indication Message; Indication for MCM_VOICE_E911_STATE_IND. */
typedef struct {

  /* Mandatory */
  mcm_voice_e911_state_t_v01 e911_state;
  /**<   E911 state.*/
}mcm_voice_e911_state_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_qmi_messages
    @{
  */
/** Request Message; Indication for MCM_VOICE_GET_E911_STATE. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}mcm_voice_get_e911_state_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup mcm_voice_qmi_messages
    @{
  */
/** Response Message; Indication for MCM_VOICE_GET_E911_STATE. */
typedef struct {

  /* Mandatory */
  mcm_response_t_v01 response;
  /**<  Result code.*/

  /* Optional */
  uint8_t e911_state_valid;  /**< Must be set to true if e911_state is being passed */
  mcm_voice_e911_state_t_v01 e911_state;
  /**<   E911 state.*/
}mcm_voice_get_e911_state_resp_msg_v01;  /* Message */
/**
    @}
  */

#define MCM_VOICE_GET_CALLS_REQ_V01 0x1000
#define MCM_VOICE_GET_CALLS_RESP_V01 0x1000
#define MCM_VOICE_DIAL_REQ_V01 0x1001
#define MCM_VOICE_DIAL_RESP_V01 0x1001
#define MCM_VOICE_GET_CALL_STATUS_REQ_V01 0x1002
#define MCM_VOICE_GET_CALL_STATUS_RESP_V01 0x1002
#define MCM_VOICE_DTMF_REQ_V01 0x1003
#define MCM_VOICE_DTMF_RESP_V01 0x1003
#define MCM_VOICE_START_DTMF_REQ_V01 0x1004
#define MCM_VOICE_START_DTMF_RESP_V01 0x1004
#define MCM_VOICE_STOP_DTMF_REQ_V01 0x1005
#define MCM_VOICE_STOP_DTMF_RESP_V01 0x1005
#define MCM_VOICE_MUTE_REQ_V01 0x1006
#define MCM_VOICE_MUTE_RESP_V01 0x1006
#define MCM_VOICE_FLASH_REQ_V01 0x1007
#define MCM_VOICE_FLASH_RESP_V01 0x1007
#define MCM_VOICE_HANGUP_REQ_V01 0x1008
#define MCM_VOICE_HANGUP_RESP_V01 0x1008
#define MCM_VOICE_COMMAND_REQ_V01 0x1009
#define MCM_VOICE_COMMAND_RESP_V01 0x1009
#define MCM_VOICE_AUTO_ANSWER_REQ_V01 0x100A
#define MCM_VOICE_AUTO_ANSWER_RESP_V01 0x100A
#define MCM_VOICE_EVENT_REGISTER_REQ_V01 0x100B
#define MCM_VOICE_EVENT_REGISTER_RESP_V01 0x100B
#define MCM_VOICE_GET_CALL_FORWARDING_STATUS_REQ_V01 0x100C
#define MCM_VOICE_GET_CALL_FORWARDING_STATUS_RESP_V01 0x100C
#define MCM_VOICE_SET_CALL_FORWARDING_REQ_V01 0x100D
#define MCM_VOICE_SET_CALL_FORWARDING_RESP_V01 0x100D
#define MCM_VOICE_GET_CALL_WAITING_STATUS_REQ_V01 0x100E
#define MCM_VOICE_GET_CALL_WAITING_STATUS_RESP_V01 0x100E
#define MCM_VOICE_SET_CALL_WAITING_REQ_V01 0x100F
#define MCM_VOICE_SET_CALL_WAITING_RESP_V01 0x100F
#define MCM_VOICE_GET_CLIR_REQ_V01 0x1010
#define MCM_VOICE_GET_CLIR_RESP_V01 0x1010
#define MCM_VOICE_SET_CLIR_REQ_V01 0x1011
#define MCM_VOICE_SET_CLIR_RESP_V01 0x1011
#define MCM_VOICE_SET_FACILITY_LOCK_REQ_V01 0x1012
#define MCM_VOICE_SET_FACILITY_LOCK_RESP_V01 0x1012
#define MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REQ_V01 0x1013
#define MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_RESP_V01 0x1013
#define MCM_VOICE_SEND_USSD_REQ_V01 0x1014
#define MCM_VOICE_SEND_USSD_RESP_V01 0x1014
#define MCM_VOICE_CANCEL_USSD_REQ_V01 0x1015
#define MCM_VOICE_CANCEL_USSD_RESP_V01 0x1015
#define MCM_VOICE_COMMON_DIAL_REQ_V01 0x1016
#define MCM_VOICE_COMMON_DIAL_RESP_V01 0x1016
#define MCM_VOICE_CALL_IND_V01 0x1017
#define MCM_VOICE_MUTE_IND_V01 0x1018
#define MCM_VOICE_DTMF_IND_V01 0x1019
#define MCM_VOICE_RECEIVE_USSD_IND_V01 0x101A
#define MCM_VOICE_UPDATE_ECALL_MSD_REQ_V01 0x101B
#define MCM_VOICE_UPDATE_ECALL_MSD_RESP_V01 0x101B
#define MCM_VOICE_E911_STATE_IND_V01 0x101C
#define MCM_VOICE_GET_E911_STATE_REQ_V01 0x101D
#define MCM_VOICE_GET_E911_STATE_RESP_V01 0x101D

#ifdef __cplusplus
}
#endif
#endif

