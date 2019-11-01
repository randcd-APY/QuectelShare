/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ R I L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_ril_service service Data structures.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved. 
 Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Jan 21 2016 (Spin )
   From IDL File: mcm_ril_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_voice_v01.h"
#include "mcm_common_v01.h"
#include "mcm_sms_v01.h"
#include "mcm_nw_v01.h"
#include "mcm_dm_v01.h"
#include "mcm_service_object_v01.h"


/*Type Definitions*/
/*Message Definitions*/
/* Type Table */
/* Message Table */
/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object mcm_ril_service_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mcm_ril_service_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_ril_service_qmi_idl_type_table_object_v01, &mcm_voice_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01, &mcm_sms_qmi_idl_type_table_object_v01, &mcm_nw_qmi_idl_type_table_object_v01, &mcm_dm_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object mcm_ril_service_qmi_idl_type_table_object_v01 = {
  0,
  0,
  1,
  NULL,
  NULL,
  mcm_ril_service_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry mcm_ril_service_service_command_messages_v01[] = {
  {MCM_DM_GET_RADIO_MODE_REQ_V01, QMI_IDL_TYPE16(5, 0), 0},
  {MCM_DM_SET_RADIO_MODE_REQ_V01, QMI_IDL_TYPE16(5, 2), 7},
  {MCM_DM_EVENT_REGISTER_REQ_V01, QMI_IDL_TYPE16(5, 4), 4},
  {MCM_DM_GET_DEVICE_SERIAL_NUMBERS_REQ_V01, QMI_IDL_TYPE16(5, 4), 0},
  {MCM_DM_GET_DEVICE_REV_ID_REQ_V01, QMI_IDL_TYPE16(5, 6), 0},
  {MCM_NW_SET_CONFIG_REQ_V01, QMI_IDL_TYPE16(4, 0), 18},
  {MCM_NW_GET_CONFIG_REQ_V01, QMI_IDL_TYPE16(4, 2), 0},
  {MCM_NW_GET_REGISTRATION_STATUS_REQ_V01, QMI_IDL_TYPE16(4, 4), 0},
  {MCM_NW_SCAN_REQ_V01, QMI_IDL_TYPE16(4, 6), 0},
  {MCM_NW_GET_OPERATOR_NAME_REQ_V01, QMI_IDL_TYPE16(4, 8), 0},
  {MCM_NW_SCREEN_ON_OFF_REQ_V01, QMI_IDL_TYPE16(4, 10), 4},
  {MCM_NW_SELECTION_REQ_V01, QMI_IDL_TYPE16(4, 12), 19},
  {MCM_NW_GET_SIGNAL_STRENGTH_REQ_V01, QMI_IDL_TYPE16(4, 14), 0},
  {MCM_NW_GET_CELL_ACCESS_STATE_REQ_V01, QMI_IDL_TYPE16(4, 16), 0},
  {MCM_NW_GET_NITZ_TIME_INFO_REQ_V01, QMI_IDL_TYPE16(4, 18), 0},
  {MCM_NW_EVENT_REGISTER_REQ_V01, QMI_IDL_TYPE16(4, 20), 20},

/**
* Tommy.zhang -2019/10/23: [added]
* Fix error: NO event report after registering event by QL_MCM_NW_EventRegister.
* Change the index of MCM_NW_GET_CELL_INFO_REQ_V01 in the Message Table
    @{
*/
  //{MCM_NW_GET_CELL_INFO_REQ_V01, QMI_IDL_TYPE16(4, 22), 0},
  {MCM_NW_GET_CELL_INFO_REQ_V01, QMI_IDL_TYPE16(4, 27), 0},
/** 
    @}
*/

  {MCM_SMS_SET_SERVICE_CENTER_CFG_TYPE_REQ_V01, QMI_IDL_TYPE16(3, 0), 266},
  {MCM_SMS_GET_SERVICE_CENTER_CFG_TYPE_REQ_V01, QMI_IDL_TYPE16(3, 2), 0},
  {MCM_SMS_SEND_MO_MSG_REQ_V01, QMI_IDL_TYPE16(3, 4), 1712},
  {MCM_SMS_SET_MSG_CONFIG_REQ_V01, QMI_IDL_TYPE16(3, 6), 11},
  {MCM_SMS_GET_MSG_CONFIG_REQ_V01, QMI_IDL_TYPE16(3, 8), 0},
  {MCM_SMS_SET_RECEPTION_MODE_REQ_V01, QMI_IDL_TYPE16(3, 10), 18},
  {MCM_SMS_EVENT_REGISTER_REQ_V01, QMI_IDL_TYPE16(3, 12), 4},
  {MCM_VOICE_GET_CALLS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {MCM_VOICE_DIAL_REQ_V01, QMI_IDL_TYPE16(1, 2), 275},
  {MCM_VOICE_GET_CALL_STATUS_REQ_V01, QMI_IDL_TYPE16(1, 4), 7},
  {MCM_VOICE_DTMF_REQ_V01, QMI_IDL_TYPE16(1, 6), 23},
  {MCM_VOICE_START_DTMF_REQ_V01, QMI_IDL_TYPE16(1, 8), 11},
  {MCM_VOICE_STOP_DTMF_REQ_V01, QMI_IDL_TYPE16(1, 10), 7},
  {MCM_VOICE_MUTE_REQ_V01, QMI_IDL_TYPE16(1, 12), 14},
  {MCM_VOICE_FLASH_REQ_V01, QMI_IDL_TYPE16(1, 14), 85},
  {MCM_VOICE_HANGUP_REQ_V01, QMI_IDL_TYPE16(1, 16), 7},
  {MCM_VOICE_COMMAND_REQ_V01, QMI_IDL_TYPE16(1, 18), 21},
  {MCM_VOICE_AUTO_ANSWER_REQ_V01, QMI_IDL_TYPE16(1, 20), 14},
  {MCM_VOICE_EVENT_REGISTER_REQ_V01, QMI_IDL_TYPE16(1, 22), 16},
  {MCM_VOICE_GET_CALL_FORWARDING_STATUS_REQ_V01, QMI_IDL_TYPE16(1, 27), 7},
  {MCM_VOICE_SET_CALL_FORWARDING_REQ_V01, QMI_IDL_TYPE16(1, 29), 99},
  {MCM_VOICE_GET_CALL_WAITING_STATUS_REQ_V01, QMI_IDL_TYPE16(1, 31), 0},
  {MCM_VOICE_SET_CALL_WAITING_REQ_V01, QMI_IDL_TYPE16(1, 33), 7},
  {MCM_VOICE_GET_CLIR_REQ_V01, QMI_IDL_TYPE16(1, 35), 0},
  {MCM_VOICE_SET_CLIR_REQ_V01, QMI_IDL_TYPE16(1, 37), 7},
  {MCM_VOICE_SET_FACILITY_LOCK_REQ_V01, QMI_IDL_TYPE16(1, 39), 21},
  {MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REQ_V01, QMI_IDL_TYPE16(1, 41), 21},
  {MCM_VOICE_SEND_USSD_REQ_V01, QMI_IDL_TYPE16(1, 43), 145},
  {MCM_VOICE_CANCEL_USSD_REQ_V01, QMI_IDL_TYPE16(1, 45), 0},
  {MCM_VOICE_COMMON_DIAL_REQ_V01, QMI_IDL_TYPE16(1, 48), 275},
  {MCM_VOICE_UPDATE_ECALL_MSD_REQ_V01, QMI_IDL_TYPE16(1, 50), 144},
  {MCM_VOICE_GET_E911_STATE_REQ_V01, QMI_IDL_TYPE16(1, 53), 0}
};

static const qmi_idl_service_message_table_entry mcm_ril_service_service_response_messages_v01[] = {
  {MCM_DM_GET_RADIO_MODE_RESP_V01, QMI_IDL_TYPE16(5, 1), 18},
  {MCM_DM_SET_RADIO_MODE_RESP_V01, QMI_IDL_TYPE16(5, 3), 15},
  {MCM_DM_EVENT_REGISTER_RESP_V01, QMI_IDL_TYPE16(5, 5), 11},
  {MCM_DM_GET_DEVICE_SERIAL_NUMBERS_RESP_V01, QMI_IDL_TYPE16(5, 5), 81},
  {MCM_DM_GET_DEVICE_REV_ID_RESP_V01, QMI_IDL_TYPE16(5, 7), 270},
  {MCM_NW_SET_CONFIG_RESP_V01, QMI_IDL_TYPE16(4, 1), 11},
  {MCM_NW_GET_CONFIG_RESP_V01, QMI_IDL_TYPE16(4, 3), 29},
  {MCM_NW_GET_REGISTRATION_STATUS_RESP_V01, QMI_IDL_TYPE16(4, 5), 189},
  {MCM_NW_SCAN_RESP_V01, QMI_IDL_TYPE16(4, 7), 41775},
  {MCM_NW_GET_OPERATOR_NAME_RESP_V01, QMI_IDL_TYPE16(4, 9), 1050},
  {MCM_NW_SCREEN_ON_OFF_RESP_V01, QMI_IDL_TYPE16(4, 11), 11},
  {MCM_NW_SELECTION_RESP_V01, QMI_IDL_TYPE16(4, 13), 11},
  {MCM_NW_GET_SIGNAL_STRENGTH_RESP_V01, QMI_IDL_TYPE16(4, 15), 53},
  {MCM_NW_GET_CELL_ACCESS_STATE_RESP_V01, QMI_IDL_TYPE16(4, 17), 18},
  {MCM_NW_GET_NITZ_TIME_INFO_RESP_V01, QMI_IDL_TYPE16(4, 19), 60},
  {MCM_NW_EVENT_REGISTER_RESP_V01, QMI_IDL_TYPE16(4, 21), 11},
  /**
* Tommy.zhang -2019/10/28: [modify]
* Fix error: NO event report after registering event by QL_MCM_NW_EventRegister.
* Change the index of MCM_NW_GET_CELL_INFO_RESP_V01 in the Message Table
    @{
*/
  //{MCM_NW_GET_CELL_INFO_RESP_V01, QMI_IDL_TYPE16(4, 23), 870},
  {MCM_NW_GET_CELL_INFO_RESP_V01, QMI_IDL_TYPE16(4, 28), 870},
/** 
    @}
*/
  {MCM_SMS_SET_SERVICE_CENTER_CFG_TYPE_RESP_V01, QMI_IDL_TYPE16(3, 1), 11},
  {MCM_SMS_GET_SERVICE_CENTER_CFG_TYPE_RESP_V01, QMI_IDL_TYPE16(3, 3), 277},
  {MCM_SMS_SEND_MO_MSG_RESP_V01, QMI_IDL_TYPE16(3, 5), 11},
  {MCM_SMS_SET_MSG_CONFIG_RESP_V01, QMI_IDL_TYPE16(3, 7), 11},
  {MCM_SMS_GET_MSG_CONFIG_RESP_V01, QMI_IDL_TYPE16(3, 9), 22},
  {MCM_SMS_SET_RECEPTION_MODE_RESP_V01, QMI_IDL_TYPE16(3, 11), 11},
  {MCM_SMS_EVENT_REGISTER_RESP_V01, QMI_IDL_TYPE16(3, 13), 11},
  {MCM_VOICE_GET_CALLS_RESP_V01, QMI_IDL_TYPE16(1, 1), 1079},
  {MCM_VOICE_DIAL_RESP_V01, QMI_IDL_TYPE16(1, 3), 18},
  {MCM_VOICE_GET_CALL_STATUS_RESP_V01, QMI_IDL_TYPE16(1, 5), 147},
  {MCM_VOICE_DTMF_RESP_V01, QMI_IDL_TYPE16(1, 7), 11},
  {MCM_VOICE_START_DTMF_RESP_V01, QMI_IDL_TYPE16(1, 9), 18},
  {MCM_VOICE_STOP_DTMF_RESP_V01, QMI_IDL_TYPE16(1, 11), 18},
  {MCM_VOICE_MUTE_RESP_V01, QMI_IDL_TYPE16(1, 13), 11},
  {MCM_VOICE_FLASH_RESP_V01, QMI_IDL_TYPE16(1, 15), 11},
  {MCM_VOICE_HANGUP_RESP_V01, QMI_IDL_TYPE16(1, 17), 11},
  {MCM_VOICE_COMMAND_RESP_V01, QMI_IDL_TYPE16(1, 19), 11},
  {MCM_VOICE_AUTO_ANSWER_RESP_V01, QMI_IDL_TYPE16(1, 21), 11},
  {MCM_VOICE_EVENT_REGISTER_RESP_V01, QMI_IDL_TYPE16(1, 23), 11},
  {MCM_VOICE_GET_CALL_FORWARDING_STATUS_RESP_V01, QMI_IDL_TYPE16(1, 28), 1153},
  {MCM_VOICE_SET_CALL_FORWARDING_RESP_V01, QMI_IDL_TYPE16(1, 30), 11},
  {MCM_VOICE_GET_CALL_WAITING_STATUS_RESP_V01, QMI_IDL_TYPE16(1, 32), 18},
  {MCM_VOICE_SET_CALL_WAITING_RESP_V01, QMI_IDL_TYPE16(1, 34), 11},
  {MCM_VOICE_GET_CLIR_RESP_V01, QMI_IDL_TYPE16(1, 36), 25},
  {MCM_VOICE_SET_CLIR_RESP_V01, QMI_IDL_TYPE16(1, 38), 11},
  {MCM_VOICE_SET_FACILITY_LOCK_RESP_V01, QMI_IDL_TYPE16(1, 40), 11},
  {MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_RESP_V01, QMI_IDL_TYPE16(1, 42), 11},
  {MCM_VOICE_SEND_USSD_RESP_V01, QMI_IDL_TYPE16(1, 44), 11},
  {MCM_VOICE_CANCEL_USSD_RESP_V01, QMI_IDL_TYPE16(1, 46), 11},
  {MCM_VOICE_COMMON_DIAL_RESP_V01, QMI_IDL_TYPE16(1, 49), 1188},
  {MCM_VOICE_UPDATE_ECALL_MSD_RESP_V01, QMI_IDL_TYPE16(1, 51), 11},
  {MCM_VOICE_GET_E911_STATE_RESP_V01, QMI_IDL_TYPE16(1, 54), 18}
};

static const qmi_idl_service_message_table_entry mcm_ril_service_service_indication_messages_v01[] = {
  {MCM_DM_RADIO_MODE_CHANGED_EVENT_IND_V01, QMI_IDL_TYPE16(5, 6), 7},
  {MCM_NW_VOICE_REGISTRATION_EVENT_IND_V01, QMI_IDL_TYPE16(4, 22), 89},
  {MCM_NW_DATA_REGISTRATION_EVENT_IND_V01, QMI_IDL_TYPE16(4, 23), 89},
  {MCM_NW_SIGNAL_STRENGTH_EVENT_IND_V01, QMI_IDL_TYPE16(4, 24), 42},
  {MCM_NW_CELL_ACCESS_STATE_CHANGE_EVENT_IND_V01, QMI_IDL_TYPE16(4, 25), 7},
  {MCM_NW_NITZ_TIME_IND_V01, QMI_IDL_TYPE16(4, 26), 49},
  {MCM_SMS_PP_IND_V01, QMI_IDL_TYPE16(3, 14), 443},
  {MCM_SMS_CB_IND_V01, QMI_IDL_TYPE16(3, 15), 170},
  {MCM_SMS_CB_CMAS_IND_V01, QMI_IDL_TYPE16(3, 16), 200},
  {MCM_VOICE_CALL_IND_V01, QMI_IDL_TYPE16(1, 24), 1068},
  {MCM_VOICE_MUTE_IND_V01, QMI_IDL_TYPE16(1, 25), 4},
  {MCM_VOICE_DTMF_IND_V01, QMI_IDL_TYPE16(1, 26), 32},
  {MCM_VOICE_RECEIVE_USSD_IND_V01, QMI_IDL_TYPE16(1, 47), 138},
  {MCM_VOICE_E911_STATE_IND_V01, QMI_IDL_TYPE16(1, 52), 7}
};

/*Service Object*/
struct qmi_idl_service_object mcm_ril_service_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x401,
  41775,
  { sizeof(mcm_ril_service_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_ril_service_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_ril_service_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { mcm_ril_service_service_command_messages_v01, mcm_ril_service_service_response_messages_v01, mcm_ril_service_service_indication_messages_v01},
  &mcm_ril_service_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type mcm_ril_service_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( MCM_RIL_SERVICE_V01_IDL_MAJOR_VERS != idl_maj_version || MCM_RIL_SERVICE_V01_IDL_MINOR_VERS != idl_min_version
       || MCM_RIL_SERVICE_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&mcm_ril_service_qmi_idl_service_object_v01;
}

