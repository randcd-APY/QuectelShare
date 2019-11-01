/***************************************************************************************************
    @file
    hlos_event_lookup.c

    @brief
    Implements functions supported in hlos_event_lookup.h.

  Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#include "hlos_event_lookup.h"
#include "core_event_lookup.h"
#include "hlos_dms_core.h"
#include "hlos_nas_core.h"
#include "hlos_voice_core.h"
#include "hlos_sms_core.h"
#include "hlos_srv_mgr.h"
#include "mcm_uim_request.h"
#include "mcm_dm_v01.h"
#include "mcm_nw_v01.h"
#include "mcm_voice_v01.h"
#include "mcm_sms_v01.h"
#include "mcm_sim_v01.h"
#include "mcm_client_v01.h"

core_event_lookup_map_type hlos_request_map[] =
{
   {MCM_DM_SET_RADIO_MODE_REQ_V01, hlos_dms_set_modem_request_handler},
   {MCM_DM_GET_RADIO_MODE_REQ_V01, hlos_dms_get_modem_status_request_handler},
   {MCM_DM_GET_DEVICE_SERIAL_NUMBERS_REQ_V01, hlos_dms_get_device_serial_numbers_request_handler},
   {MCM_DM_GET_DEVICE_REV_ID_REQ_V01, hlos_dms_get_device_rev_id_request_handler},
   {MCM_NW_SCAN_REQ_V01, hlos_nas_network_scan_request_handler},
   {MCM_NW_GET_OPERATOR_NAME_REQ_V01, hlos_nas_get_operator_name_request_handler},
   {MCM_NW_SET_CONFIG_REQ_V01, hlos_nas_network_config_request_handler},
   {MCM_NW_GET_CONFIG_REQ_V01, hlos_nas_network_get_config_request_handler},
   {MCM_NW_GET_REGISTRATION_STATUS_REQ_V01, hlos_nas_network_get_status_request_handler},
   {MCM_NW_SELECTION_REQ_V01, hlos_nas_network_selection_handler},
   {MCM_NW_SCREEN_ON_OFF_REQ_V01, hlos_nas_network_screen_state_handler},
   {MCM_NW_GET_SIGNAL_STRENGTH_REQ_V01, hlos_nas_network_get_signal_strength_handler},
   {MCM_NW_GET_CELL_ACCESS_STATE_REQ_V01, hlos_nas_get_cell_access_state_handler},
   {MCM_NW_GET_NITZ_TIME_INFO_REQ_V01, hlos_nas_network_get_nitz_time_info_handler},
   /** add by tommy.zhang 20191023 start : for get cell info */
   {MCM_NW_GET_CELL_INFO_REQ_V01, hlos_nas_get_cell_info_handler},
   /** add by tommy.zhang 20191023 end */
   {MCM_VOICE_DIAL_REQ_V01,            hlos_voice_dial_request_handler},
   {MCM_VOICE_HANGUP_REQ_V01,          hlos_voice_hangup_request_handler},
   {MCM_VOICE_COMMAND_REQ_V01,         hlos_voice_command_request_handler},
   {MCM_VOICE_GET_CALL_STATUS_REQ_V01, hlos_voice_get_call_status_request_handler},
   {MCM_VOICE_GET_CALLS_REQ_V01,       hlos_voice_get_calls_request_handler},
   {MCM_VOICE_MUTE_REQ_V01,            hlos_voice_mute_request_handler},
   {MCM_VOICE_AUTO_ANSWER_REQ_V01,     hlos_voice_auto_answer_request_handler},
   {MCM_VOICE_START_DTMF_REQ_V01,      hlos_voice_start_dtmf_request_handler},
   {MCM_VOICE_STOP_DTMF_REQ_V01,       hlos_voice_stop_dtmf_request_handler},
   {MCM_VOICE_GET_CALL_FORWARDING_STATUS_REQ_V01,          hlos_voice_get_call_forwarding_status_request_handler},
   {MCM_VOICE_SET_CALL_FORWARDING_REQ_V01,                 hlos_voice_set_call_forwarding_request_handler},
   {MCM_VOICE_GET_CALL_WAITING_STATUS_REQ_V01,             hlos_voice_get_call_waiting_status_request_handler},
   {MCM_VOICE_SET_CALL_WAITING_REQ_V01,                    hlos_voice_set_call_waiting_request_handler},
   {MCM_VOICE_GET_CLIR_REQ_V01,                            hlos_voice_get_clir_request_handler},
   {MCM_VOICE_SET_CLIR_REQ_V01,                            hlos_voice_set_clir_request_handler},
   {MCM_VOICE_CHANGE_CALL_BARRING_PASSWORD_REQ_V01,        hlos_voice_change_call_barring_password_request_handler},
   {MCM_VOICE_SEND_USSD_REQ_V01,                           hlos_voice_send_ussd_request_handler},
   {MCM_VOICE_CANCEL_USSD_REQ_V01,                         hlos_voice_cancel_ussd_request_handler},
   {MCM_VOICE_CANCEL_USSD_REQ_V01,                         hlos_voice_cancel_ussd_request_handler},
   {MCM_VOICE_UPDATE_ECALL_MSD_REQ_V01,                    hlos_voice_update_ecall_msd_request_handler},
   {MCM_VOICE_GET_E911_STATE_REQ_V01, hlos_voice_get_e911_state_info_handler},
   {MCM_SMS_SEND_MO_MSG_REQ_V01, hlos_sms_send_mo_msg_request_handler},
   {MCM_SIM_GET_SUBSCRIBER_ID_REQ_V01, mcm_uim_request_get_imsi},
   {MCM_SIM_GET_CARD_ID_REQ_V01, mcm_uim_request_get_iccid},
   {MCM_SIM_GET_DEVICE_PHONE_NUMBER_REQ_V01, mcm_uim_request_get_phone_number},
   {MCM_SIM_GET_PREFERRED_OPERATOR_LIST_REQ_V01, mcm_uim_request_get_operator_plmn_list},
   {MCM_SIM_READ_FILE_REQ_V01, mcm_uim_request_read_file},
   {MCM_SIM_WRITE_FILE_REQ_V01, mcm_uim_request_write_file},
   {MCM_SIM_GET_FILE_SIZE_REQ_V01, mcm_uim_request_get_file_size},
   {MCM_SIM_VERIFY_PIN_REQ_V01, mcm_uim_request_verify_pin},
   {MCM_SIM_CHANGE_PIN_REQ_V01, mcm_uim_request_change_pin},
   {MCM_SIM_UNBLOCK_PIN_REQ_V01, mcm_uim_request_unblock_pin},
   {MCM_SIM_ENABLE_PIN_REQ_V01, mcm_uim_request_enable_pin},
   {MCM_SIM_DISABLE_PIN_REQ_V01, mcm_uim_request_disable_pin},
   {MCM_SIM_GET_CARD_STATUS_REQ_V01, mcm_uim_request_get_card_status},
   {MCM_SIM_DEPERSONALIZATION_REQ_V01, mcm_uim_request_depersonalization},
   {MCM_SIM_PERSONALIZATION_REQ_V01, mcm_uim_request_personalization},
   {MCM_CLIENT_REQUIRE_REQ_V01, hlos_srv_mgr_require_handle},
   {MCM_CLIENT_NOT_REQUIRE_REQ_V01, hlos_srv_mgr_not_require_handle}
};


core_event_lookup_map_type hlos_control_map[] =
{



};





/***************************************************************************************************
    @function
    hlos_event_lookup_request_handler

    @implementation detail
    None.
***************************************************************************************************/
void* hlos_event_lookup_request_handler(hlos_core_hlos_request_data_type
                                        *hlos_core_hlos_request_data)
{
    void *event_handler;
    unsigned long event_id;
    core_event_lookup_map_type *event_map;
    int event_map_len;
    mcm_response_t_v01 resp;

    event_handler = NULL;
    event_id = NIL;
    event_map = NULL;
    event_map_len = NIL;

    if(hlos_core_hlos_request_data && (CRI_CORE_GEN_OPERATIONAL_STATUS_SUSPENDED == cri_core_get_operational_status()))
    {
        resp.result = MCM_RESULT_FAILURE_V01;
        resp.error = hlos_map_qmi_ril_error_to_mcm_error(CRI_ERR_RADIO_RESET_V01);
        hlos_core_send_response(NIL,
                                CRI_ERR_RADIO_RESET_V01,
                                hlos_core_hlos_request_data,
                                &resp,
                                sizeof(resp));
    }
    else
    {
        if(hlos_core_hlos_request_data)
        {
            event_id = hlos_core_hlos_request_data->event_id;
            event_map = hlos_request_map;
            event_map_len = UTIL_ARR_SIZE(hlos_request_map);
            event_handler = core_event_lookup_map_checker(event_id,
                                                      event_map,
                                                      event_map_len);
        }
        if(event_handler == NULL)
        {
            /*For the message ids that are in the Range, but are not supported.*/
            hlos_core_send_response(NIL,
                                CRI_ERR_NOT_SUPPORTED_V01,
                                NULL,
                                NIL,
                                0);
        }
    }

    return event_handler;
}

/***************************************************************************************************
    @function
    hlos_event_lookup_control_handler

    @implementation detail
    None.
***************************************************************************************************/
void* hlos_event_lookup_control_handler(control_core_control_event_data_type
                                        *control_core_control_event_data)
{
    void *event_handler;
    unsigned long event_id;
    core_event_lookup_map_type *event_map;
    int event_map_len;

    event_handler = NULL;
    event_id = NIL;
    event_map = NULL;
    event_map_len = NIL;

    if(control_core_control_event_data)
    {
        event_id = control_core_control_event_data->event_id;
        event_map = hlos_control_map;
        event_map_len = UTIL_ARR_SIZE(hlos_control_map);
        event_handler = core_event_lookup_map_checker(event_id,
                                                      event_map,
                                                      event_map_len);
    }

    return event_handler;
}


